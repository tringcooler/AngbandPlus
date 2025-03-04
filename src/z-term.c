#define Z_TERM_C
/* File: z-term.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

/* Purpose: a generic, efficient, terminal window package -BEN- */

#include "z-term.h"

#include "z-virt.h"

#include "externs.h"

/*
 * This file provides a generic, efficient, terminal window package,
 * which can be used not only on standard terminal environments such
 * as dumb terminals connected to a Unix box, but also in more modern
 * "graphic" environments, such as the Macintosh or Unix/X11.
 *
 * Each "window" works like a standard "dumb terminal", that is, it
 * can display a two dimensional array of grids containing colored
 * textual symbols, plus an optional cursor, and it can be used to
 * get keypress events from the user.
 *
 * In fact, this package can simply be used, if desired, to support
 * programs which will look the same on a dumb terminal as they do
 * on a graphic platform such as the Macintosh.
 *
 * This package was designed to help port the game "Angband" to a wide
 * variety of different platforms.  Angband, like many other games in
 * the "rogue-like" heirarchy, requires, at the minimum, the ability
 * to display "colored textual symbols" in a standard 80x24 "window",
 * such as that provided by most dumb terminals, and many old personal
 * computers, and to check for "keypresses" from the user.  The major
 * concerns were thus portability and efficiency, so Angband could be
 * easily ported to many different systems, with minimal effort, and
 * yet would run quickly on each of these systems, no matter what kind
 * of underlying hardware/software support was being used.
 *
 * It is important to understand the differences between the older
 * "dumb terminals" and the newer "graphic interface" machines, since
 * this package was designed to work with both types of systems.
 *
 * New machines:
 *   waiting for a keypress is complex
 *   checking for a keypress is often cheap
 *   changing "colors" may be expensive
 *   the "color" of a "blank" is rarely important
 *   moving the "cursor" is relatively cheap
 *   use a "software" cursor (only moves when requested)
 *   drawing characters normally will not erase old ones
 *   drawing a character on the cursor often erases it
 *   may have fast routines for "clear a region"
 *   the bottom right corner is usually not special
 *
 * Old machines:
 *   waiting for a keypress is simple
 *   checking for a keypress is often expensive
 *   changing "colors" is usually cheap
 *   the "color" of a "blank" may be important
 *   moving the "cursor" may be expensive
 *   use a "hardware" cursor (moves during screen updates)
 *   drawing new symbols automatically erases old ones
 *   characters may only be drawn at the cursor location
 *   drawing a character on the cursor will move the cursor
 *   may have fast routines for "clear entire window"
 *   may have fast routines for "clear to end of line"
 *   the bottom right corner is often dangerous
 *
 *
 * This package provides support for multiple windows, each of an
 * arbitrary size (up to 255x255), each with its own set of flags,
 * and its own hooks to handle several low-level procedures which
 * differ from platform to platform.  Then the main program simply
 * creates one or more "term" structures, setting the various flags
 * and hooks in a manner appropriate for the current platform, and
 * then it can use the various "term" structures without worrying
 * about the underlying platform.
 *
 *
 * This package allows each "grid" in each window to hold an attr/char
 * pair, with each ranging from 0 to 255, and makes very few assumptions
 * about the meaning of any attr/char values.  Normally, we assume that
 * "attr 0" is "black", with the semantics that "black" text should be
 * sent to "Term_wipe()" instead of "Term_text()", but this sematics is
 * modified if either the "always_pict" or the "always_text" flags are
 * set.  We assume that "char 0" is "dangerous", since placing such a
 * "char" in the middle of a string "terminates" the string, and usually
 * we prevent its use.
 *
 * Finally, we use a special attr/char pair, defaulting to "attr 0" and
 * "char 32", also known as "black space", when we "erase" or "clear"
 * any window, but this pair can be redefined to any pair, including
 * the standard "white space", or the bizarre "emptiness" ("attr 0"
 * and "char 0"), as long as various obscure restrictions are met.
 *
 *
 * This package provides several functions which allow a program to
 * interact with the "term" structures.  Most of the functions allow
 * the program to "request" certain changes to the current "term",
 * such as moving the cursor, drawing an attr/char pair, erasing a
 * region of grids, hiding the cursor, etc.  Then there is a special
 * function which causes all of the "pending" requests to be performed
 * in an efficient manner.  There is another set of functions which
 * allow the program to query the "requested state" of the current
 * "term", such as asking for the cursor location, or what attr/char
 * is at a given location, etc.  There is another set of functions
 * dealing with "keypress" events, which allows the program to ask if
 * the user has pressed any keys, or to forget any keys the user pressed.
 * There is a pair of functions to allow this package to memorize the
 * contents of the current "term", and to restore these contents at
 * a later time.  There is a special function which allows the program
 * to specify which "term" structure should be the "current" one.  At
 * the lowest level, there is a set of functions which allow a new
 * "term" to be initialized or destroyed, and which allow this package,
 * or a program, to access the special "hooks" defined for the current
 * "term", and a set of functions which those "hooks" can use to inform
 * this package of the results of certain occurances, for example, one
 * such function allows this package to learn about user keypresses,
 * detected by one of the special "hooks".
 *
 * We provide, among other things, the functions "Term_keypress()"
 * to "react" to keypress events, and "Term_redraw()" to redraw the
 * entire window, plus "Term_resize()" to note a new size.
 *
 *
 * Note that the current "term" contains two "window images".  One of
 * these images represents the "requested" contents of the "term", and
 * the other represents the "actual" contents of the "term", at the time
 * of the last performance of pending requests.  This package uses these
 * two images to determine the "minimal" amount of work needed to make
 * the "actual" contents of the "term" match the "requested" contents of
 * the "term".  This method is not perfect, but it often reduces the
 * amount of work needed to perform the pending requests, which thus
 * increases the speed of the program itself.  This package promises
 * that the requested changes will appear to occur either "all at once"
 * or in a "top to bottom" order.  In addition, a "cursor" is maintained,
 * and this cursor is updated along with the actual window contents.
 *
 * Currently, the "Term_fresh()" routine attempts to perform the "minimum"
 * number of physical updates, in terms of total "work" done by the hooks
 * Term_wipe(), Term_text(), and Term_pict(), making use of the fact that
 * adjacent characters of the same color can both be drawn together using
 * the "Term_text()" hook, and that "black" text can often be sent to the
 * "Term_wipe()" hook instead of the "Term_text()" hook, and if something
 * is already displayed in a window, then it is not necessary to display
 * it again.  Unfortunately, this may induce slightly non-optimal results
 * in some cases, in particular, those in which, say, a string of ten
 * characters needs to be written, but the fifth character has already
 * been displayed.  Currently, this will cause the "Term_text()" routine
 * to be called once for each half of the string, instead of once for the
 * whole string, which, on some machines, may be non-optimal behavior.
 *
 * The new formalism includes a "displayed" screen image (old) which
 * is actually seen by the user, a "requested" screen image (scr)
 * which is being prepared for display, a "memorized" screen image
 * (mem) which is used to save and restore screen images, and a
 * "temporary" screen image (tmp) which is currently unused.
 *
 *
 * Several "flags" are available in each "term" to allow the underlying
 * visual system (which initializes the "term" structure) to "optimize"
 * the performance of this package for the given system, or to request
 * certain behavior which is helpful/required for the given system.
 *
 * The "soft_cursor" flag indicates the use of a "soft" cursor, which
 * only moves when explicitly requested,and which is "erased" when
 * any characters are drawn on top of it.  This flag is used for all
 * "graphic" systems which handle the cursor by "drawing" it.
 *
 * The "icky_corner" flag indicates that the bottom right "corner"
 * of the windows are "icky", and "printing" anything there may
 * induce "messy" behavior, such as "scrolling".  This flag is used
 * for most old "dumb terminal" systems.
 *
 *
 * The "term" structure contains the following function "hooks":
 *
 *   Term->init_hook = Init the term
 *   Term->nuke_hook = Nuke the term
 *   Term->user_hook = Perform user actions
 *   Term->xtra_hook = Perform extra actions
 *   Term->curs_hook = Draw (or Move) the cursor
 *   Term->wipe_hook = Draw some blank spaces
 *   Term->text_hook = Draw some text in the window
 *   Term->pict_hook = Draw some attr/chars in the window
 *
 * The "Term->user_hook" hook provides a simple hook to an implementation
 * defined function, with application defined semantics.  It is available
 * to the program via the "Term_user()" function.
 *
 * The "Term->xtra_hook" hook provides a variety of different functions,
 * based on the first parameter (which should be taken from the various
 * TERM_XTRA_* defines) and the second parameter (which may make sense
 * only for some first parameters).  It is available to the program via
 * the "Term_xtra()" function, though some first parameters are only
 * "legal" when called from inside this package.
 *
 * The "Term->curs_hook" hook provides this package with a simple way
 * to "move" or "draw" the cursor to the grid "x,y", depending on the
 * setting of the "soft_cursor" flag.  Note that the cursor is never
 * redrawn if "nothing" has happened to the screen (even temporarily).
 * This hook is required.
 *
 * The "Term->wipe_hook" hook provides this package with a simple way
 * to "erase", starting at "x,y", the next "n" grids.  This hook assumes
 * that the input is valid.  This hook is required, unless the setting
 * of the "always_pict" or "always_text" flags makes it optional.
 *
 * The "Term->text_hook" hook provides this package with a simple way
 * to "draw", starting at "x,y", the "n" chars contained in "cp", using
 * the attr "a".  This hook assumes that the input is valid, and that
 * "n" is between 1 and 256 inclusive, but it should NOT assume that
 * the contents of "cp" are null-terminated.  This hook is required,
 * unless the setting of the "always_pict" flag makes it optional.
 *
 * The "Term->pict_hook" hook provides this package with a simple way
 * to "draw", starting at "x,y", the "n" attr/char pairs contained in
 * the arrays "ap" and "cp".  This hook assumes that the input is valid,
 * and that "n" is between 1 and 256 inclusive, but it should NOT assume
 * that the contents of "cp" are null-terminated.  This hook is optional,
 * unless the setting of the "always_pict" or "higher_pict" flags make
 * it required.  Note that recently, this hook was changed from taking
 * a byte "a" and a char "c" to taking a length "n", an array of bytes
 * "ap" and an array of chars "cp".  Old implementations of this hook
 * should now iterate over all "n" attr/char pairs.
 *
 *
 * The game "Angband" uses a set of files called "main-xxx.c", for
 * various "xxx" suffixes.  Most of these contain a function called
 * "init_xxx()", that will prepare the underlying visual system for
 * use with Angband, and then create one or more "term" structures,
 * using flags and hooks appropriate to the given platform, so that
 * the "main()" function can call one (or more) of the "init_xxx()"
 * functions, as appropriate, to prepare the required "term" structs
 * (one for each desired sub-window), and these "init_xxx()" functions
 * are called from a centralized "main()" function in "main.c".  Other
 * "main-xxx.c" systems contain their own "main()" function which, in
 * addition to doing everything needed to initialize the actual program,
 * also does everything that the normal "init_xxx()" functions would do.
 *
 * The game "Angband" defines, in addition to "attr 0", all of the
 * attr codes from 1 to 15, using definitions in "defines.h", and
 * thus the "main-xxx.c" files used by Angband must handle these
 * attr values correctly.  Also, they must handle all other attr
 * values, though they may do so in any way they wish, for example,
 * by always taking every attr code mod 16.  Many of the "main-xxx.c"
 * files use "white space" ("attr 1" / "char 32") to "erase" or "clear"
 * any window, for efficiency.
 *
 * The game "Angband" uses the "Term_user" hook to allow any of the
 * "main-xxx.c" files to interact with the user, by calling this hook
 * whenever the user presses the "!" key when the game is waiting for
 * a new command.  This could be used, for example, to provide "unix
 * shell commands" to the Unix versions of the game.
 *
 * See "main-xxx.c" for a simple skeleton file which can be used to
 * create a "visual system" for a new platform when porting Angband.
 */

/*
 * N.B. There are a number of void functions with commented out return codes.
 * This isn't a claim that the return code isn't meaningful, but that the
 * meaning could be redefined according to need without disrupting existing
 * functions.
 */





/*
 * The current "term"
 */
term *Term = NULL;




/*** Local routines ***/


/*
 * Nuke a term_win (see below)
 */
static void term_win_nuke(term_win *s)
{
	/* Free the window access arrays */
	KILL(s->a);
	KILL(s->c);

	/* Free the window content arrays */
	KILL(s->va);
	KILL(s->vc);

	/* Free the terrain access arrays */
	KILL(s->ta);
	KILL(s->tc);

	/* Free the terrain content arrays */
	KILL(s->vta);
	KILL(s->vtc);

	/* Success */
	return;
}


/*
 * Initialize a "term_win" (using the given window size)
 */
static void term_win_init(term_win *s, int w, int h)
{
	int y;

	/* Make the window access arrays */
	C_MAKE(s->a, h, byte*);
	C_MAKE(s->c, h, char*);

	/* Make the window content arrays */
	C_MAKE(s->va, h * w, byte);
	C_MAKE(s->vc, h * w, char);

	/* Make the terrain access arrays */
	C_MAKE(s->ta, h, byte*);
	C_MAKE(s->tc, h, char*);

	/* Make the terrain content arrays */
	C_MAKE(s->vta, h * w, byte);
	C_MAKE(s->vtc, h * w, char);

	/* Prepare the window access arrays */
	for (y = 0; y < h; y++)
	{
		s->a[y] = s->va + w * y;
		s->c[y] = s->vc + w * y;

		s->ta[y] = s->vta + w * y;
		s->tc[y] = s->vtc + w * y;
	}

	/* Success */
	return;
}


/*
 * Copy a "term_win" from another
 */
static void term_win_copy(term_win *s, term_win *f, int w, int h)
{
	int x, y;

	/* Copy contents */
	for (y = 0; y < h; y++)
	{
		byte *f_aa = f->a[y];
		char *f_cc = f->c[y];

		byte *s_aa = s->a[y];
		char *s_cc = s->c[y];

		byte *f_taa = f->ta[y];
		char *f_tcc = f->tc[y];

		byte *s_taa = s->ta[y];
		char *s_tcc = s->tc[y];

		for (x = 0; x < w; x++)
		{
			*s_aa++ = *f_aa++;
			*s_cc++ = *f_cc++;

			*s_taa++ = *f_taa++;
			*s_tcc++ = *f_tcc++;
		}
	}

	/* Copy cursor */
	s->cx = f->cx;
	s->cy = f->cy;
	s->cu = f->cu;
	s->cv = f->cv;

	/* Success */
	return;
}



/*** External hooks ***/


/*
 * Execute the "Term->user_hook" hook, if available (see above).
 */
void Term_user(void)
{
	/* Verify the hook */
	if (!Term->user_hook) return;

	/* Call the hook */
	(*Term->user_hook)(0);
}

/*
 * Execute the "Term->xtra_hook" hook, if available (see above).
 */
void Term_xtra(int n, int v)
{
	/* Verify the hook */
	if (!Term->xtra_hook) return;

	/* Call the hook */
	(*Term->xtra_hook)(n, v);
}



/*** Fake hooks ***/


/*
 * Hack -- fake hook for "Term_curs()" (see above)
 */
static errr Term_curs_hack(int x, int y)
{
	/* Compiler silliness */
	if (x || y) return (-2);

	/* Oops */
	return (-1);
}

/*
 * Hack -- fake hook for "Term_wipe()" (see above)
 */
static errr Term_wipe_hack(int x, int y, int n)
{
	/* Compiler silliness */
	if (x || y || n) return (-2);

	/* Oops */
	return (-1);
}

/*
 * Hack -- fake hook for "Term_text()" (see above)
 */
static errr Term_text_hack(int x, int y, int n, byte a, const char *cp)
{
	/* Compiler silliness */
	if (x || y || n || a || cp) return (-2);

	/* Oops */
	return (-1);
}


/*
 * Hack -- fake hook for "Term_pict()" (see above)
 */
static errr Term_pict_hack(int x, int y, int n, const byte *ap, const char *cp, const byte *tap, const char *tcp)
{
	/* Compiler silliness */
	if (x || y || n || ap || cp || tap || tcp) return (-2);

	/* Oops */
	return (-1);
}


/*** Efficient routines ***/


/*
 * Mentally draw an attr/char at a given location
 *
 * Assumes given location and values are valid.
 */
void Term_queue_char(int x, int y, byte a, char c, byte ta, char tc)
{
	byte *scr_aa = Term->scr->a[y];
	char *scr_cc = Term->scr->c[y];

	int oa = scr_aa[x];
	int oc = scr_cc[x];

	byte *scr_taa = Term->scr->ta[y];
	char *scr_tcc = Term->scr->tc[y];

	int ota = scr_taa[x];
	int otc = scr_tcc[x];

	/* Don't change is the terrain value is 0 */
	if (!ta) ta = ota;
	if (!tc) tc = otc;

	/* Hack -- Ignore non-changes */
	if ((oa == a) && (oc == c) && (ota == ta) && (otc == tc)) return;

	/* Hack -- fake monochrome */
	if (a != TERM_DARK && !use_graphics && !use_color) a = TERM_WHITE;
	if (ta != TERM_DARK && !use_graphics && !use_color) ta = TERM_WHITE;

	/* Save the "literal" information */
	scr_aa[x] = a;
	scr_cc[x] = c;

	scr_taa[x] = ta;
	scr_tcc[x] = tc;

	/* Check for new min/max row info */
	if (y < Term->y1) Term->y1 = y;
	if (y > Term->y2) Term->y2 = y;

	/* Check for new min/max col info for this row */
	if (x < Term->x1[y]) Term->x1[y] = x;
	if (x > Term->x2[y]) Term->x2[y] = x;
}


/*
 * Mentally draw some attr/chars at a given location
 *
 * Assumes that (x,y) is a valid location, that the first "n" characters
 * of the string "s" are all valid (non-zero), and that (x+n-1,y) is also
 * a valid location, so the first "n" characters of "s" can all be added
 * starting at (x,y) without causing any illegal operations.
 */
/*static*/ void Term_queue_chars(int x, int y, int n, byte a, cptr s)
{
	int x1 = -1, x2 = -1;

	byte *scr_aa = Term->scr->a[y];
	char *scr_cc = Term->scr->c[y];

	byte *scr_taa = Term->scr->ta[y];
	char *scr_tcc = Term->scr->tc[y];

	/* Hack -- fake monochrome */
	if (a != TERM_DARK && !use_graphics && !use_color) a = TERM_WHITE;

	/* Queue the attr/chars */
	for ( ; n; x++, s++, n--)
	{
		int oa = scr_aa[x];
		int oc = scr_cc[x];

		int ota = scr_taa[x];
		int otc = scr_tcc[x];

		/* Hack -- Ignore non-changes */
		if ((oa == a) && (oc == *s) && (ota == 0) && (otc == 0)) continue;

		/* Save the "literal" information */
		scr_aa[x] = a;
		scr_cc[x] = *s;

		scr_taa[x] = 0;
		scr_tcc[x] = 0;

		/* Note the "range" of window updates */
		if (x1 < 0) x1 = x;
		x2 = x;
	}

	/* Expand the "change area" as needed */
	if (x1 >= 0)
	{
		/* Check for new min/max row info */
		if (y < Term->y1) Term->y1 = y;
		if (y > Term->y2) Term->y2 = y;

		/* Check for new min/max col info in this row */
		if (x1 < Term->x1[y]) Term->x1[y] = x1;
		if (x2 > Term->x2[y]) Term->x2[y] = x2;
	}
}



/*** Refresh routines ***/


/*
 * Flush a row of the current window (see "Term_fresh")
 *
 * Display text using "Term_pict()"
 */
static void Term_fresh_row_pict(int y, int x1, int x2)
{
	int x;

	byte *old_aa = Term->old->a[y];
	char *old_cc = Term->old->c[y];

	byte *scr_aa = Term->scr->a[y];
	char *scr_cc = Term->scr->c[y];

	byte *old_taa = Term->old->ta[y];
	char *old_tcc = Term->old->tc[y];

	byte *scr_taa = Term->scr->ta[y];
	char *scr_tcc = Term->scr->tc[y];

	byte ota;
	char otc;

	byte nta;
	char ntc;

	/* Pending length */
	int fn = 0;

	/* Pending start */
	int fx = 0;

	byte oa;
	char oc;

	byte na;
	char nc;

	/* Scan "modified" columns */
	for (x = x1; x <= x2; x++)
	{
		/* See what is currently here */
		oa = old_aa[x];
		oc = old_cc[x];

		/* See what is desired there */
		na = scr_aa[x];
		nc = scr_cc[x];

		ota = old_taa[x];
		otc = old_tcc[x];

		nta = scr_taa[x];
		ntc = scr_tcc[x];

		/* Handle unchanged grids */
		if ((na == oa) && (nc == oc) && (nta == ota) && (ntc == otc))
		{
			/* Flush */
			if (fn)
			{
				/* Draw pending attr/char pairs */
				(void)((*Term->pict_hook)(fx, y, fn,
						&scr_aa[fx], &scr_cc[fx],&scr_taa[fx], &scr_tcc[fx]));

				/* Forget */
				fn = 0;
			}

			/* Skip */
			continue;
		}
		/* Save new contents */
		old_aa[x] = na;
		old_cc[x] = nc;

		old_taa[x] = nta;
		old_tcc[x] = ntc;

		/* Restart and Advance */
		if (fn++ == 0) fx = x;
	}

	/* Flush */
	if (fn)
	{
		/* Draw pending attr/char pairs */
		(void)((*Term->pict_hook)(fx, y, fn,
			&scr_aa[fx], &scr_cc[fx], &scr_taa[fx], &scr_tcc[fx]));
	}
}



/*
 * Flush a row of the current window (see "Term_fresh")
 *
 * Display text using "Term_text()" and "Term_wipe()",
 * but use "Term_pict()" for high-bit attr/char pairs
 */
static void Term_fresh_row_both(int y, int x1, int x2)
{
	int x;

	byte *old_aa = Term->old->a[y];
	char *old_cc = Term->old->c[y];
	byte *scr_aa = Term->scr->a[y];
	char *scr_cc = Term->scr->c[y];

	byte *old_taa = Term->old->ta[y];
	char *old_tcc = Term->old->tc[y];
	byte *scr_taa = Term->scr->ta[y];
	char *scr_tcc = Term->scr->tc[y];

	byte ota;
	char otc;
	byte nta;
	char ntc;

	/* The "always_text" flag */
	int always_text = Term->always_text;

	/* Pending length */
	int fn = 0;

	/* Pending start */
	int fx = 0;

	/* Pending attr */
	byte fa = Term->attr_blank;

	byte oa;
	char oc;

	byte na;
	char nc;

	/* Scan "modified" columns */
	for (x = x1; x <= x2; x++)
	{
		/* See what is currently here */
		oa = old_aa[x];
		oc = old_cc[x];

		/* See what is desired there */
		na = scr_aa[x];
		nc = scr_cc[x];

		ota = old_taa[x];
		otc = old_tcc[x];

		nta = scr_taa[x];
		ntc = scr_tcc[x];

		/* Handle unchanged grids */
		if ((na == oa) && (nc == oc) && (nta == ota) && (ntc == otc))
		{
			/* Flush */
			if (fn)
			{
				/* Draw pending chars (normal) */
				if (fa || always_text)
				{
					(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
				}

				/* Draw pending chars (black) */
				else
				{
					(void)((*Term->wipe_hook)(fx, y, fn));
				}

				/* Forget */
				fn = 0;
			}

			/* Skip */
			continue;
		}

		/* Save new contents */
		old_aa[x] = na;
		old_cc[x] = nc;

		old_taa[x] = nta;
		old_tcc[x] = ntc;

		/* Handle high-bit attr/chars */
		if ((na & 0x80) && (nc & 0x80))
		{
			/* Flush */
			if (fn)
			{
				/* Draw pending chars (normal) */
				if (fa || always_text)
				{
					(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
				}

				/* Draw pending chars (black) */
				else
				{
					(void)((*Term->wipe_hook)(fx, y, fn));
				}

				/* Forget */
				fn = 0;
			}

			/* Hack -- Draw the special attr/char pair */
			(void)((*Term->pict_hook)(x, y, 1, &na, &nc, &nta, &ntc));

			/* Skip */
			continue;
		}

		/* Notice new color */
		if (fa != na)
		{
			/* Flush */
			if (fn)
			{
				/* Draw the pending chars */
				if (fa || always_text)
				{
					(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
				}

				/* Hack -- Erase "leading" spaces */
				else
				{
					(void)((*Term->wipe_hook)(fx, y, fn));
				}

				/* Forget */
				fn = 0;
			}

			/* Save the new color */
			fa = na;
		}

		/* Restart and Advance */
		if (fn++ == 0) fx = x;
	}

	/* Flush */
	if (fn)
	{
		/* Draw pending chars (normal) */
		if (fa || always_text)
		{
			(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
		}

		/* Draw pending chars (black) */
		else
		{
			(void)((*Term->wipe_hook)(fx, y, fn));
		}
	}
}


/*
 * Flush a row of the current window (see "Term_fresh")
 *
 * Display text using "Term_text()" and "Term_wipe()"
 */
static void Term_fresh_row_text(int y, int x1, int x2)
{
	int x;

	byte *old_aa = Term->old->a[y];
	char *old_cc = Term->old->c[y];

	byte *scr_aa = Term->scr->a[y];
	char *scr_cc = Term->scr->c[y];

	/* The "always_text" flag */
	int always_text = Term->always_text;

	/* Pending length */
	int fn = 0;

	/* Pending start */
	int fx = 0;

	/* Pending attr */
	byte fa = Term->attr_blank;

	byte oa;
	char oc;

	byte na;
	char nc;


	/* Scan "modified" columns */
	for (x = x1; x <= x2; x++)
	{
		/* See what is currently here */
		oa = old_aa[x];
		oc = old_cc[x];

		/* See what is desired there */
		na = scr_aa[x];
		nc = scr_cc[x];

		/* Handle unchanged grids */
		if ((na == oa) && (nc == oc))
		{
			/* Flush */
			if (fn)
			{
				/* Draw pending chars (normal) */
				if (fa || always_text)
				{
					(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
				}

				/* Draw pending chars (black) */
				else
				{
					(void)((*Term->wipe_hook)(fx, y, fn));
				}

				/* Forget */
				fn = 0;
			}

			/* Skip */
			continue;
		}

		/* Save new contents */
		old_aa[x] = na;
		old_cc[x] = nc;

		/* Notice new color */
		if (fa != na)
		{
			/* Flush */
			if (fn)
			{
				/* Draw the pending chars */
				if (fa || always_text)
				{
					(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
				}

				/* Hack -- Erase "leading" spaces */
				else
				{
					(void)((*Term->wipe_hook)(fx, y, fn));
				}

				/* Forget */
				fn = 0;
			}

			/* Save the new color */
			fa = na;
		}

		/* Restart and Advance */
		if (fn++ == 0) fx = x;
	}

	/* Flush */
	if (fn)
	{
		/* Draw pending chars (normal) */
		if (fa || always_text)
		{
			(void)((*Term->text_hook)(fx, y, fn, fa, &scr_cc[fx]));
		}

		/* Draw pending chars (black) */
		else
		{
			(void)((*Term->wipe_hook)(fx, y, fn));
		}
	}
}




/*
 * Clear the remembered version of a term_win on the current term and
 * move the cursor to the top-left.
 */
static void Term_win_clear(term_win *win)
{
	int x, y;

	int w = Term->wid;
	int h = Term->hgt;

	byte na = Term->attr_blank;
	char nc = Term->char_blank;

	/* Cursor usable */
	win->cu = 0;

	/* Cursor to the top left */
	win->cx = win->cy = 0;

	/* Wipe each row */
	for (y = 0; y < h; y++)
	{
		byte *aa = win->a[y];
		char *cc = win->c[y];

		byte *taa = win->ta[y];
		char *tcc = win->tc[y];

		/* Wipe each column */
		for (x = 0; x < w; x++)
		{
			*aa++ = na;
			*cc++ = nc;

			*taa++ = 0;
			*tcc++ = 0;
		}

		/* This row has changed */
		Term->x1[y] = 0;
		Term->x2[y] = w - 1;
	}

	/* Every row has changed */
	Term->y1 = 0;
	Term->y2 = h - 1;
}


/*
 * Actually perform all requested changes to the window
 *
 * If aboslutely nothing has changed, not even temporarily, or if the
 * current "Term" is not mapped, then this function will return 1 and
 * do absolutely nothing.
 *
 * Note that when "soft_cursor" is true, we erase the cursor (if needed)
 * whenever anything has changed, and redraw it (if needed) after all of
 * the screen updates are complete.  This will induce a small amount of
 * "cursor flicker" but only when the screen has been updated.  If the
 * screen is updated and then restored, you may still get this flicker.
 *
 * When "soft_cursor" is not true, we make the cursor invisible before
 * doing anything else if it is supposed to be invisible by the time we
 * are done, and we make it visible after moving it to its final location
 * after all of the screen updates are complete.
 *
 * Note that "Term_xtra(TERM_XTRA_CLEAR,0)" must erase the entire screen,
 * including the cursor, if needed, and may place the cursor anywhere.
 *
 * Note that "Term_xtra(TERM_XTRA_FROSH,y)" will be always be called
 * after any row "y" has been "flushed", unless the "Term->never_frosh"
 * flag is set, and "Term_xtra(TERM_XTRA_FRESH,0)" will be called after
 * all of the rows have been "flushed".
 *
 * Note the use of three different functions to handle the actual flush,
 * based on the settings of the "Term->always_pict" and "Term->higher_pict"
 * flags (see below).
 *
 * The three helper functions (above) work by collecting similar adjacent
 * grids into stripes, and then sending each stripe to "Term->pict_hook",
 * "Term->text_hook", or "Term->wipe_hook", based on the settings of the
 * "Term->always_pict" and "Term->higher_pict" flags, which select which
 * of the helper functions to call to flush each row.
 *
 * The helper functions currently "skip" any grids which already contain
 * the desired contents.  This may or may not be the best method, especially
 * when the desired content fits nicely into the current stripe.  For example,
 * it might be better to go ahead and queue them while allowed, but keep a
 * count of the "trailing skipables", then, when time to flush, or when a
 * "non skippable" is found, force a flush if there are too many skippables.
 *
 * Perhaps an "initialization" stage, where the "text" (and "attr")
 * buffers are "filled" with information, converting "blanks" into
 * a convenient representation, and marking "skips" with "zero chars",
 * and then some "processing" is done to determine which chars to skip.
 *
 * Currently, the helper functions are optimal for systems which prefer
 * to "print a char + move a char + print a char" to "print three chars",
 * and for applications that do a lot of "detailed" color printing.
 *
 * In the two "queue" functions, total "non-changes" are "pre-skipped".
 * The helper functions must also handle situations in which the contents
 * of a grid are changed, but then changed back to the original value,
 * and situations in which two grids in the same row are changed, but
 * the grids between them are unchanged.
 *
 * If the "Term->always_pict" flag is set, then "Term_fresh_row_pict()"
 * will be used instead of "Term_fresh_row_text()".  This allows all the
 * modified grids to be collected into stripes of attr/char pairs, which
 * are then sent to the "Term->pict_hook" hook, which can draw these pairs
 * in whatever way it would like.
 *
 * If the "Term->higher_pict" flag is set, then "Term_fresh_row_both()"
 * will be used instead of "Term_fresh_row_text()".  This allows all the
 * "special" attr/char pairs (in which both the attr and char have the
 * high-bit set) to be sent (one pair at a time) to the "Term->pict_hook"
 * hook, which can draw these pairs in whatever way it would like.
 *
 * Normally, the "Term_wipe()" function is used only to display "blanks"
 * that were induced by "Term_clear()" or "Term_erase()", and then only
 * if the "attr_blank" and "char_blank" fields have not been redefined
 * to use "white space" instead of the default "black space".  Actually,
 * the "Term_wipe()" function is used to display all "black" text, such
 * as the default "spaces" created by "Term_clear()" and "Term_erase()".
 *
 * Note that the "Term->always_text" flag will disable the use of the
 * "Term_wipe()" function hook entirely, and force all text, even text
 * drawn in the color "black", to be explicitly drawn.  This is useful
 * for machines which implement "Term_wipe()" by just drawing spaces.
 *
 * Note that the "Term->always_pict" flag will disable the use of the
 * "Term_wipe()" function entirely, and force everything, even text
 * drawn in the attr "black", to be explicitly drawn.
 *
 * Note that if no "black" text is ever drawn, and if "attr_blank" is
 * not "zero", then the "Term_wipe" hook will never be used, even if
 * the "Term->always_text" flag is not set.
 *
 * This function does nothing unless the "Term" is "mapped", which allows
 * certain systems to optimize the handling of "closed" windows.
 *
 * On systems with a "soft" cursor, we must explicitly erase the cursor
 * before flushing the output, if needed, to prevent a "jumpy" refresh.
 * The actual method for this is horrible, but there is very little that
 * we can do to simplify it efficiently.  XXX XXX XXX
 *
 * On systems with a "hard" cursor, we will "hide" the cursor before
 * flushing the output, if needed, to avoid a "flickery" refresh.  It
 * would be nice to *always* hide the cursor during the refresh, but
 * this might be expensive (and/or ugly) on some machines.
 *
 * The "Term->icky_corner" flag is used to avoid calling "Term_wipe()"
 * or "Term_pict()" or "Term_text()" on the bottom right corner of the
 * window, which might induce "scrolling" or other nasty stuff on old
 * dumb terminals.  This flag is handled very efficiently.  We assume
 * that the "Term_curs()" call will prevent placing the cursor in the
 * corner, if needed, though I doubt such placement is ever a problem.
 * Currently, the use of "Term->icky_corner" and "Term->soft_cursor"
 * together may result in undefined behavior.
 */
void Term_fresh(void)
{
	int y;

	int w = Term->wid;
	int h = Term->hgt;

	int y1 = Term->y1;
	int y2 = Term->y2;

	term_win *old = Term->old;
	term_win *scr = Term->scr;


	/* Do nothing unless "mapped" */
	if (!Term->mapped_flag) return /*(1)*/;


	/* Trivial Refresh */
	if ((y1 > y2) &&
		(scr->cu == old->cu) &&
		(scr->cv == old->cv) &&
		(scr->cx == old->cx) &&
		(scr->cy == old->cy) &&
		!(Term->total_erase))
	{
		/* Nothing */
		return /*(1)*/;
	}


	/* Paranoia -- use "fake" hooks to prevent core dumps */
	if (!Term->curs_hook) Term->curs_hook = Term_curs_hack;
	if (!Term->wipe_hook) Term->wipe_hook = Term_wipe_hack;
	if (!Term->text_hook) Term->text_hook = Term_text_hack;
	if (!Term->pict_hook) Term->pict_hook = Term_pict_hack;


	/* Handle "total erase" */
	if (Term->total_erase)
	{
		/* Physically erase the entire window */
		Term_xtra(TERM_XTRA_CLEAR, 0);

		/* Hide the cursor. */
		old->cv = 0;

		/* Clear the displayed screen image. */
		Term_win_clear(old);

		/* Forget "total erase" */
		Term->total_erase = FALSE;

		/* Update local stuff. */
		y1 = Term->y1;
		y2 = Term->y2;
	}


	/* Cursor update -- Erase old Cursor */
	if (Term->soft_cursor)
	{
		/* Cursor was visible */
		if (!old->cu && old->cv)
		{
			int tx = old->cx;
			int ty = old->cy;

			byte *old_aa = old->a[ty];
			char *old_cc = old->c[ty];

			byte oa = old_aa[tx];
			char oc = old_cc[tx];

			byte *old_taa = old->ta[ty];
			char *old_tcc = old->tc[ty];

			byte ota = old_taa[tx];
			char otc = old_tcc[tx];

			/* Hack -- use "Term_pict()" always */
			if (Term->always_pict)
			{
				(void)((*Term->pict_hook)(tx, ty, 1, &oa, &oc, &ota, &otc));
			}

			/* Hack -- use "Term_pict()" sometimes */
			else if (Term->higher_pict && (oa & 0x80) && (oc & 0x80))
			{
				(void)((*Term->pict_hook)(tx, ty, 1, &oa, &oc, &ota, &otc));
			}

			/* Hack -- restore the actual character */
			else if (oa || Term->always_text)
			{
				(void)((*Term->text_hook)(tx, ty, 1, oa, &oc));
			}

			/* Hack -- erase the grid */
			else
			{
				(void)((*Term->wipe_hook)(tx, ty, 1));
			}
		}
	}

	/* Cursor Update -- Erase old Cursor */
	else
	{
		/* Cursor will be invisible */
		if (scr->cu || !scr->cv)
		{
			/* Make the cursor invisible */
			Term_xtra(TERM_XTRA_SHAPE, 0);
		}
	}


	/* Something to update */
	if (y1 <= y2)
	{
		/* Handle "icky corner" */
		if (Term->icky_corner)
		{
			/* Avoid the corner */
			if (y2 >= h - 1)
			{
				/* Avoid the corner */
				if (Term->x2[h - 1] > w - 2)
				{
					/* Avoid the corner */
					Term->x2[h - 1] = w - 2;
				}
			}
		}


		/* Scan the "modified" rows */
		for (y = y1; y <= y2; ++y)
		{
			int x1 = Term->x1[y];
			int x2 = Term->x2[y];

			/* Flush each "modified" row */
			if (x1 <= x2)
			{
				/* Always use "Term_pict()" */
				if (Term->always_pict)
				{
					/* Flush the row */
					Term_fresh_row_pict(y, x1, x2);
				}

				/* Sometimes use "Term_pict()" */
				else if (Term->higher_pict)
				{
					/* Flush the row */
					Term_fresh_row_both(y, x1, x2);
				}

				/* Never use "Term_pict()" */
				else
				{
					/* Flush the row */
					Term_fresh_row_text(y, x1, x2);
				}

				/* This row is all done */
				Term->x1[y] = w;
				Term->x2[y] = 0;

				/* Hack -- Flush that row (if allowed) */
				if (!Term->never_frosh) Term_xtra(TERM_XTRA_FROSH, y);
			}
		}

		/* No rows are invalid */
		Term->y1 = h;
		Term->y2 = 0;
	}


	/* Cursor update -- Show new Cursor */
	if (Term->soft_cursor)
	{
		/* Draw the cursor */
		if (!scr->cu && scr->cv)
		{
			/* Call the cursor display routine */
			(void)((*Term->curs_hook)(scr->cx, scr->cy));
		}
	}

	/* Cursor Update -- Show new Cursor */
	else
	{
		/* The cursor is useless, hide it */
		if (scr->cu)
		{
			/* Paranoia -- Put the cursor NEAR where it belongs */
			(void)((*Term->curs_hook)(w - 1, scr->cy));

			/* Make the cursor invisible */
			/* Term_xtra(TERM_XTRA_SHAPE, 0); */
		}

		/* The cursor is invisible, hide it */
		else if (!scr->cv)
		{
			/* Paranoia -- Put the cursor where it belongs */
			(void)((*Term->curs_hook)(scr->cx, scr->cy));

			/* Make the cursor invisible */
			/* Term_xtra(TERM_XTRA_SHAPE, 0); */
		}

		/* The cursor is visible, display it correctly */
		else
		{
			/* Put the cursor where it belongs */
			(void)((*Term->curs_hook)(scr->cx, scr->cy));

			/* Make the cursor visible */
			Term_xtra(TERM_XTRA_SHAPE, 1);
		}
	}


	/* Save the "cursor state" */
	old->cu = scr->cu;
	old->cv = scr->cv;
	old->cx = scr->cx;
	old->cy = scr->cy;


	/* Actually flush the output */
	Term_xtra(TERM_XTRA_FRESH, 0);


	/* Success */
	return /*(0)*/;
}



/*** Output routines ***/


/*
 * Set the cursor visibility
 */
void Term_set_cursor(bool v)
{
	/* Already done */
	if (Term->scr->cv == v) return /*TERM_ERROR_TRIVIAL_REQUEST*/;

	/* Change */
	Term->scr->cv = v;

	return /*SUCCESS*/;
}


/*
 * Place the cursor at a given location
 *
 * Note -- "illegal" requests do not move the cursor.
 */
errr Term_gotoxy(int x, int y)
{
	int w = Term->wid;
	int h = Term->hgt;

	/* Verify */
	if ((x < 0) || (x >= w)) return TERM_ERROR_OUT_OF_BOUNDS;
	if ((y < 0) || (y >= h)) return TERM_ERROR_OUT_OF_BOUNDS;

	/* Remember the cursor */
	Term->scr->cx = x;
	Term->scr->cy = y;

	/* The cursor is not useless */
	Term->scr->cu = 0;

	return SUCCESS;
}


/*
 * At a given location, place an attr/char
 * Do not change the cursor position
 * No visual changes until "Term_fresh()".
 */
void Term_draw(int x, int y, byte a, char c)
{
	int w = Term->wid;
	int h = Term->hgt;

	/* Verify location */
	if ((x < 0) || (x >= w)) return /*TERM_ERROR_OUT_OF_BOUNDS*/;
	if ((y < 0) || (y >= h)) return /*TERM_ERROR_OUT_OF_BOUNDS*/;

	/* Paranoia -- illegal char */
	if (!c) return /*TERM_ERROR_BAD_INPUT*/;

	/* Queue it for later */
	Term_queue_char(x, y, a, c, 0, 0);

	return /*SUCCESS*/;
}


/*
 * Using the given attr, add the given char at the cursor.
 *
 * We return "BAD_INPUT" if the character is "illegal". XXX XXX
 *
 * We return "OUT_OF_BOUNDS" if the cursor is currently unusable.
 *
 * We queue the given attr/char for display at the current
 * cursor location, and advance the cursor to the right,
 * marking it as unuable and returning "WARN(OUT_OF_BOUNDS)" if it leaves
 * the screen, and otherwise returning "0".
 *
 * So when this function, or the following one, return a
 * positive value (warning), future calls to either function will
 * return negative ones (errors).
 */
void Term_addch(byte a, char c)
{
	int w = Term->wid;

	/* Handle "unusable" cursor */
	if (Term->scr->cu) return /* TERM_ERROR_OUT_OF_BOUNDS */;

	/* Paranoia -- no illegal chars */
	if (!c) return /* TERM_ERROR_BAD_INPUT */;

	/* Queue the given character for display */
	Term_queue_char(Term->scr->cx, Term->scr->cy, a, c, 0, 0);

	/* Advance the cursor */
	Term->scr->cx++;

	/* Success */
	if (Term->scr->cx < w) return /* SUCCESS */;

	/* Note "Useless" cursor */
	Term->scr->cu = 1;

	/* Note "Useless" cursor */
	return /* WARN(TERM_ERROR_OUT_OF_BOUNDS) */;
}


/*
 * At the current location, using an attr, add a string
 *
 * We also take a length "n", using negative values to imply
 * the largest possible value, and then we use the minimum of
 * this length and the "actual" length of the string as the
 * actual number of characters to attempt to display, never
 * displaying more characters than will actually fit, since
 * we do NOT attempt to "wrap" the cursor at the screen edge.
 *
 * We return "OUT_OF_BOUNDS" if the cursor is currently unusable.
 * We return "N" if we were "only" able to write "N" chars,
 * even if all of the given characters fit on the screen,
 * and mark the cursor as unusable for future attempts.
 *
 * So when this function, or the preceding one, return a
 * positive value, future calls to either function will
 * return negative ones.
 */
void Term_addstr(int n, byte a, cptr s)
{
	int k;

	int w = Term->wid;

	errr res = 0;

	/* Handle "unusable" cursor */
	if (Term->scr->cu) return /* TERM_ERROR_OUT_OF_BOUNDS */;

	/* Obtain maximal length */
	k = (n < 0) ? (w + 1) : n;

	/* Obtain the usable string length */
	for (n = 0; (n < k) && s[n]; n++) /* loop */;

	/* React to reaching the edge of the screen */
	if (Term->scr->cx + n >= w) res = n = w - Term->scr->cx;

	/* Queue the first "n" characters for display */
	Term_queue_chars(Term->scr->cx, Term->scr->cy, n, a, s);

	/* Advance the cursor */
	Term->scr->cx += n;

	/* Hack -- Notice "Useless" cursor */
	if (res) Term->scr->cu = 1;

	/* Success (usually) */
	return /* (res) */;
}


/*
 * Move to a location and, using an attr, add a char
 */
void Term_putch(int x, int y, byte a, char c)
{
	/* Move first */
	if (Term_gotoxy(x, y)) return;

	/* Then add the char */
	Term_addch(a, c);
}


/*
 * Move to a location and, using an attr, add a string
 */
void Term_putstr(int x, int y, int n, byte a, cptr s)
{
	/* Move first */
	if (Term_gotoxy(x, y)) return;

	/* Then add the string */
	Term_addstr(n, a, s);
}



/*
 * Place cursor at (x,y), and clear the next "n" chars
 */
errr Term_erase(int x, int y, int n)
{
	int i;

	int w = Term->wid;
	/* int h = Term->hgt; */

	int x1 = -1;
	int x2 = -1;

	int na = Term->attr_blank;
	int nc = Term->char_blank;

	byte *scr_aa;
	char *scr_cc;

	byte *scr_taa;
	char *scr_tcc;

	/* Place cursor */
	if (Term_gotoxy(x, y)) return TERM_ERROR_OUT_OF_BOUNDS;

	/* Force legal size */
	if (x + n > w) n = w - x;

	/* Fast access */
	scr_aa = Term->scr->a[y];
	scr_cc = Term->scr->c[y];

	scr_taa = Term->scr->ta[y];
	scr_tcc = Term->scr->tc[y];

	/* Scan every column */
	for (i = 0; i < n; i++, x++)
	{
		int oa = scr_aa[x];
		int oc = scr_cc[x];

		/* Hack -- Ignore "non-changes" */
		if ((oa == na) && (oc == nc)) continue;

		/* Save the "literal" information */
		scr_aa[x] = na;
		scr_cc[x] = nc;

		scr_taa[x] = 0;
		scr_tcc[x] = 0;

		/* Track minumum changed column */
		if (x1 < 0) x1 = x;

		/* Track maximum changed column */
		x2 = x;
	}

	/* Expand the "change area" as needed */
	if (x1 >= 0)
	{
		/* Check for new min/max row info */
		if (y < Term->y1) Term->y1 = y;
		if (y > Term->y2) Term->y2 = y;

		/* Check for new min/max col info in this row */
		if (x1 < Term->x1[y]) Term->x1[y] = x1;
		if (x2 > Term->x2[y]) Term->x2[y] = x2;
	}

	/* Success */
	return SUCCESS;
}


/*
 * Clear the entire window, and move to the top left corner
 *
 * Note the use of the special "total_erase" code
 */
void Term_clear(void)
{
	/* Clear the requested screen image. */
	Term_win_clear(Term->scr);

	/* Force "total erase" */
	Term->total_erase = TRUE;
}





/*
 * Redraw (and refresh) the whole window.
 */
void Term_redraw(void)
{
	/* Force "total erase" */
	Term->total_erase = TRUE;

	/* Hack -- Refresh */
	Term_fresh();
}


/*
 * Redraw part of a widow.
 */
void Term_redraw_section(int x1, int y1, int x2, int y2)
{
	int i, j;

	char *c_ptr;

	/* Bounds checking */
	if (y2 >= Term->hgt) y2 = Term->hgt - 1;
	if (x2 >= Term->wid) x2 = Term->wid - 1;
	if (y1 < 0) y1 = 0;
	if (x1 < 0) x1 = 0;


	/* Set y limits */
	Term->y1 = y1;
	Term->y2 = y2;

	/* Set the x limits */
	for (i = Term->y1; i <= Term->y2; i++)
	{
		Term->x1[i] = x1;
		Term->x2[i] = x2;

		c_ptr = Term->old->c[i];

		/* Clear the section so it is redrawn */
		for (j = x1; j <= x2; j++)
		{
			/* Hack - set the old character to "none" */
			c_ptr[j] = 0;
		}
	}

	/* Hack -- Refresh */
	Term_fresh();
}





/*** Access routines ***/


/*
 * Extract the cursor visibility
 */
void Term_get_cursor(bool *v)
{
	/* Extract visibility */
	(*v) = Term->scr->cv;
}


/*
 * Extract the current window size
 */
void Term_get_size(int *w, int *h)
{
	/* Access the cursor */
	(*w) = Term->wid;
	(*h) = Term->hgt;
}


/*
 * Extract the current cursor location
 */
void Term_locate(int *x, int *y)
{
	/* Access the cursor */
	(*x) = Term->scr->cx;
	(*y) = Term->scr->cy;
}


/*
 * At a given location, determine the "current" attr and char
 * Note that this refers to what will be on the window after the
 * next call to "Term_fresh()".  It may or may not already be there.
 */
void Term_what(int x, int y, byte *a, char *c)
{
	int w = Term->wid;
	int h = Term->hgt;

	/* Verify location */
	if ((x < 0) || (x >= w)) return /*TERM_ERROR_OUT_OF_BOUNDS*/;
	if ((y < 0) || (y >= h)) return /*TERM_ERROR_OUT_OF_BOUNDS*/;

	/* Direct access */
	(*a) = Term->scr->a[y][x];
	(*c) = Term->scr->c[y][x];

	return /*SUCCESS*/;
}



/*** Input routines ***/


/*
 * Flush and forget the input
 */
void Term_flush(void)
{
	/* Hack -- Flush all events */
	Term_xtra(TERM_XTRA_FLUSH, 0);

	/* Forget all keypresses */
	Term->key_head = Term->key_tail = 0;
}



/*
 * Add a keypress to the "queue"
 * Do nothing if there is insufficient space.
 */
errr Term_keypress(int k)
{
	uint next_char;

	/* Hack -- Refuse to enqueue non-keys */
	if (!k) return TERM_ERROR_BAD_INPUT;

	/* Do nothing without a queue. */
	if (!Term->key_size) return TERM_ERROR_OUT_OF_MEMORY;

	/* Find where the following character should be copied. */
	next_char = Term->key_head+1;
	if (next_char == Term->key_size) next_char = 0;

	/* Prevent overflow. */
	if (next_char == Term->key_tail) return TERM_ERROR_OUT_OF_MEMORY;

	/* Store the char, advance the queue */
	Term->key_queue[Term->key_head] = k;

	Term->key_head = next_char;

	return SUCCESS;
}


/*
 * Add a keypress to the FRONT of the "queue"
 * Forget the last existing character if there is insufficient space.
 */
errr Term_key_push(int k)
{
	/* Hack -- Refuse to enqueue non-keys */
	if (!k) return TERM_ERROR_BAD_INPUT;

	/* Do nothing without a queue. */
	if (!Term->key_size) return TERM_ERROR_OUT_OF_MEMORY;

	/* Hack -- Overflow may induce circular queue */
	if (Term->key_tail == 0) Term->key_tail = Term->key_size;

	/* Back up, Store the char */
	Term->key_queue[--Term->key_tail] = k;

	/* Success (unless overflow) */
	if (Term->key_head != Term->key_tail) return SUCCESS;

	/* Forget the last character on the queue. */
	if (Term->key_head) Term->key_head--;
	else Term->key_head = Term->key_size-1;

	/* Problem */
	return WARN(TERM_ERROR_OUT_OF_MEMORY);
}


/*
 * Calculate how much space there is in the key queue for the current term.
 */
int Term_queue_space(void)
{
	/* Find the gap if the tail is before the head. */
	int space = Term->key_tail - Term->key_head;

	/* Otherwise, add in the extra for looping. */
	if (space <= 0) space = Term->key_size - space;

	/* The last space is never used as that would be interpreted as leaving
	 * no pending keypresses. */
	return space-1;
}





/*
 * Check for a pending keypress on the key queue.
 *
 * Store the keypress, if any, in "ch", and return SUCCESS.
 * Otherwise store "zero" in "ch", and return NO_OUTPUT.
 *
 * Wait for a keypress if "wait" is true.
 *
 * Remove the keypress if "take" is true.
 */
errr Term_inkey(char *ch, bool wait, bool take)
{
	/* Assume no key */
	(*ch) = '\0';

	/* Hack -- get bored */
	if (!Term->never_bored)
	{
		/* Process random events */
		Term_xtra(TERM_XTRA_BORED, 0);
	}

	/* Wait */
	if (wait)
	{
		/* Process pending events while necessary */
		while (Term->key_head == Term->key_tail)
		{
			/* Process events (wait for one) */
			Term_xtra(TERM_XTRA_EVENT, TRUE);

			/* Handle various in-game things any event may trigger. */
			if (Term->key_head == Term->key_tail) event_stuff();
		}
	}

	/* Do not Wait */
	else
	{
		/* Process pending events if necessary */
		if (Term->key_head == Term->key_tail)
		{
			/* Process events (do not wait) */
			Term_xtra(TERM_XTRA_EVENT, FALSE);
		}
	}

	/* No keys are ready */
	if (Term->key_head == Term->key_tail) return TERM_ERROR_NO_OUTPUT;

	/* Extract the next keypress */
	(*ch) = Term->key_queue[Term->key_tail];

	/* If requested, advance the queue, wrap around if necessary */
	if (take && (++Term->key_tail == Term->key_size)) Term->key_tail = 0;

	/* Success */
	return SUCCESS;
}



/*** Extra routines ***/

/*
 * term_win routines.
 *
 * As windows are protected from destruction until they are loaded, a window
 * saved with Term_save_aux() will remain until it is loaded again.
 *
 * After a window has been loaded, it is only certain to be safe until
 * Term_save_aux() is next called.
 *
 * Term_save() is different as it effectively uses a single term_win to save
 * everything. In this case, only the last Term_save() has results which can
 * be retrieved, and then only until the first Term_load() call.
 */

/* The size of the term_wins[] array. This should be chosen so that it is at
 * least as high as the greatest number of displays the game will have saved
 * at once. */
#define NUM_TERM_WINS 8

static int cur_saved_term = 0;

typedef struct term_win2 term_win2;

struct term_win2
{
	term_win win; /* The "normal" term_win structure. */
	byte wid; /* The width of the window. */
	byte hgt; /* The height of the window. */
	bool mem; /* Does this window need to be remembered? */
};

static term_win2 *term_wins;

void init_term_wins(void)
{
	C_MAKE(term_wins, NUM_TERM_WINS, term_win2);
}

/*
 * Release a saved term window.
 */
void Term_release(int win)
{
	/* Paranoia */
	if (win < 1 || win > NUM_TERM_WINS) return;

	/* The term is no longer needed. */
	term_wins[win-1].mem = FALSE;
}

/*
 * Save the "requested" screen into the "memorized" screen
 *
 * Every "Term_save()" should match exactly one "Term_load()"
 */
int Term_save_aux(void)
{
	int w = Term->wid;
	int h = Term->hgt;
	int i,blank,free;
	term_win2 *w_ptr;

	/* Look for a term_win which is currently unused but already has the
	 * correct size. */
	for (i = blank = free = 0; i < NUM_TERM_WINS; i++)
	{
		w_ptr = term_wins+i;
		if (w_ptr->mem) continue;
		if (!free) free = i+1;
		if (!blank && !w_ptr->wid && !w_ptr->hgt) blank = i+1;
		if (w_ptr->wid == w && w_ptr->hgt == h) break;
	}

	/* No space available, so give up rather than risk overwriting something
	 * important. */
	if (!free && !blank && i == NUM_TERM_WINS)
	{
		return 0;
	}

	/* There's a spare term_win, but it's the wrong size. */
	if (!blank && i == NUM_TERM_WINS)
	{
		w_ptr = term_wins+free-1;
		term_win_nuke(&w_ptr->win);
		blank = free;
	}

	/* There is (now) no term_win. */
	if (i == NUM_TERM_WINS)
	{
		w_ptr = term_wins+blank-1;
		w_ptr->wid = Term->wid;
		w_ptr->hgt = Term->hgt;
		term_win_init(&w_ptr->win, w_ptr->wid, w_ptr->hgt);
		i = blank-1;
	}

	w_ptr = term_wins+i;

	/* Grab */
	term_win_copy(&w_ptr->win, Term->scr, w_ptr->wid, w_ptr->hgt);

	/* Flag the window as saved. */
	w_ptr->mem = TRUE;

	/* Return an index. */
	return i+1;
}




/*
 * Save the "requested" screen into cur_saved_term.
 */
void Term_save(void)
{
	/* Paranoia - forget any term_win currently saved. */
	if (cur_saved_term) Term_release(cur_saved_term);

	/* Save the window, remember the index. */
	cur_saved_term = Term_save_aux();
}

/*
 * Load a saved window from w_ptr to Term->scr.
 * Return FALSE on error.
 */
static bool Term_load_aux_do(term_win2 *w_ptr)
{
	int y;

	/* Load */
	term_win_copy(Term->scr, &w_ptr->win,
		MIN(Term->wid, w_ptr->wid), MIN(Term->hgt, w_ptr->hgt));

	/* Assume change */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Assume change */
		Term->x1[y] = 0;
		Term->x2[y] = Term->wid - 1;
	}

	/* Assume change */
	Term->y1 = 0;
	Term->y2 = Term->hgt - 1;

	/* Unexpected size, so part of the window may not have been copied. */
	return (w_ptr->wid == Term->wid && w_ptr->hgt == Term->hgt);
}

/*
 * Restore the "requested" contents from a specified saved term.
 *
 * Does not alter the term_wins array.
 */
void Term_load_aux(int win)
{
	/* Attempt to redraw, but catch errors. */
	if (win <= 0 || win > NUM_TERM_WINS || !Term_load_aux_do(term_wins+win-1))
	{
		/* Try the resize hook if possible. */
		if (Term->resize_hook != 0) (*Term->resize_hook)();
	}
}

/*
 * Restore the "requested" screen from cur_saved_term.
 */
void Term_load(void)
{
	/* Paranoia - no saved term_win. */
	if (!cur_saved_term) return;

	/* Restore the window from the remembered index. */
	Term_load_aux(cur_saved_term);

	/* Don't use this image again. */
	Term_release(cur_saved_term);

	/* Forget the saved term_win. */
	cur_saved_term = 0;
}



/*
 * React to a new physical window size.
 */
void Term_resize(int w, int h)
{
	int i;

	int wid, hgt;

	byte *hold_x1;
	byte *hold_x2;

	term_win *hold_old;
	term_win *hold_scr;
	term_win *hold_tmp;


	/* Resizing is forbidden */
	if (Term->fixed_shape) return /*TERM_ERROR_FORBIDDEN*/;


	/* Ignore illegal changes */
	if ((w < 1) || (h < 1)) return /*TERM_ERROR_BAD_INPUT*/;


	/* Ignore non-changes */
	if ((Term->wid == w) && (Term->hgt == h))
		return /*WARN(TERM_ERROR_TRIVIAL_REQUEST)*/;


	/* Minimum dimensions */
	wid = MIN(Term->wid, w);
	hgt = MIN(Term->hgt, h);

	/* Save scanners */
	hold_x1 = Term->x1;
	hold_x2 = Term->x2;

	/* Save old window */
	hold_old = Term->old;

	/* Save old window */
	hold_scr = Term->scr;

	/* Save old window */
	hold_tmp = Term->tmp;

	/* Create new scanners */
	C_MAKE(Term->x1, h, byte);
	C_MAKE(Term->x2, h, byte);

	/* Create new window */
	MAKE(Term->old, term_win);

	/* Initialize new window */
	term_win_init(Term->old, w, h);

	/* Save the contents */
	term_win_copy(Term->old, hold_old, wid, hgt);

	/* Create new window */
	MAKE(Term->scr, term_win);

	/* Initialize new window */
	term_win_init(Term->scr, w, h);

	/* Save the contents */
	term_win_copy(Term->scr, hold_scr, wid, hgt);

	/* If needed */
	if (hold_tmp)
	{
		/* Create new window */
		MAKE(Term->tmp, term_win);

		/* Initialize new window */
		term_win_init(Term->tmp, w, h);

		/* Save the contents */
		term_win_copy(Term->tmp, hold_tmp, wid, hgt);
	}

	/* Free some arrays */
	KILL(hold_x1);
	KILL(hold_x2);

	/* Nuke */
	term_win_nuke(hold_old);

	/* Kill */
	KILL(hold_old);

	/* Illegal cursor */
	if (Term->old->cx >= w) Term->old->cu = 1;
	if (Term->old->cy >= h) Term->old->cu = 1;

	/* Nuke */
	term_win_nuke(hold_scr);

	/* Kill */
	KILL(hold_scr);

	/* Illegal cursor */
	if (Term->scr->cx >= w) Term->scr->cu = 1;
	if (Term->scr->cy >= h) Term->scr->cu = 1;

	/* If needed */
	if (hold_tmp)
	{
		/* Nuke */
		term_win_nuke(hold_tmp);

		/* Kill */
		KILL(hold_tmp);

		/* Illegal cursor */
		if (Term->tmp->cx >= w) Term->tmp->cu = 1;
		if (Term->tmp->cy >= h) Term->tmp->cu = 1;
	}

	/* Save new size */
	Term->wid = w;
	Term->hgt = h;

	/* Force "total erase" */
	Term->total_erase = TRUE;

	/* Assume change */
	for (i = 0; i < h; i++)
	{
		/* Assume change */
		Term->x1[i] = 0;
		Term->x2[i] = w - 1;
	}

	/* Assume change */
	Term->y1 = 0;
	Term->y2 = h - 1;

	/* Execute the "resize_hook" hook, if available */
	if (Term->resize_hook)
	{
		Term->resize_hook();
	}

	/* Success */
	return /*SUCCESS*/;
}



/*
 * Activate a new Term (and deactivate the current Term)
 *
 * This function is extremely important, and also somewhat bizarre.
 * It is the only function that should "modify" the value of "Term".
 *
 * To "create" a valid "term", one should do "term_init(t)", then
 * set the various flags and hooks, and then do "Term_activate(t)".
 */
void Term_activate(term *t)
{
	/* Hack -- already done */
	if (Term == t) return /*TERM_ERROR_TRIVIAL_REQUEST*/;

	/* Deactivate the old Term */
	if (Term) Term_xtra(TERM_XTRA_LEVEL, 0);

	/* Hack -- Call the special "init" hook */
	if (t && !t->active_flag)
	{
		/* Call the "init" hook */
		if (t->init_hook) (*t->init_hook)(t);

		/* Remember */
		t->active_flag = TRUE;

		/* Assume mapped */
		t->mapped_flag = TRUE;
	}

	/* Remember the Term */
	Term = t;

	/* Activate the new Term */
	if (Term) Term_xtra(TERM_XTRA_LEVEL, 1);

	return /*SUCCESS*/;
}



/*
 * Nuke a term
 */
void term_nuke(term *t)
{
	/* Hack -- Call the special "nuke" hook */
	if (t->active_flag)
	{
		/* Call the "nuke" hook */
		if (t->nuke_hook) (*t->nuke_hook)(t);

		/* Remember */
		t->active_flag = FALSE;

		/* Assume not mapped */
		t->mapped_flag = FALSE;
	}


	/* Nuke "displayed" */
	term_win_nuke(t->old);

	/* Kill "displayed" */
	KILL(t->old);

	/* Nuke "requested" */
	term_win_nuke(t->scr);

	/* Kill "requested" */
	KILL(t->scr);

	/* If needed */
	if (t->tmp)
	{
		/* Nuke "temporary" */
		term_win_nuke(t->tmp);

		/* Kill "temporary" */
		KILL(t->tmp);
	}

	/* Free some arrays */
	KILL(t->x1);
	KILL(t->x2);

	/* Free the input queue */
	KILL(t->key_queue);
}


/*
 * Initialize a term, using a window of the given size.
 * Also prepare the "input queue" for "k" keypresses
 * By default, the cursor starts out "invisible"
 * By default, we "erase" using "black spaces"
 */
void term_init(term *t, int w, int h, int k)
{
	int y;


	/* Wipe it */
	WIPE(t, term);


	/* Prepare the input queue */
	t->key_head = t->key_tail = 0;

	/* Determine the input queue size */
	t->key_size = k;

	/* Allocate the input queue */
	C_MAKE(t->key_queue, t->key_size, char);


	/* Save the size */
	t->wid = w;
	t->hgt = h;

	/* Allocate change arrays */
	C_MAKE(t->x1, h, byte);
	C_MAKE(t->x2, h, byte);


	/* Allocate "displayed" */
	MAKE(t->old, term_win);

	/* Initialize "displayed" */
	term_win_init(t->old, w, h);


	/* Allocate "requested" */
	MAKE(t->scr, term_win);

	/* Initialize "requested" */
	term_win_init(t->scr, w, h);


	/* Assume change */
	for (y = 0; y < h; y++)
	{
		/* Assume change */
		t->x1[y] = 0;
		t->x2[y] = w - 1;
	}

	/* Assume change */
	t->y1 = 0;
	t->y2 = h - 1;

	/* Force "total erase" */
	t->total_erase = TRUE;


	/* Default "blank" */
	t->attr_blank = 0;
	t->char_blank = ' ';
}
