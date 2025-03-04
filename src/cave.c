/* File: cave.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Support for Adam Bolt's tileset, lighting and transparency effects
 * by Robert Ruehlmann (rr9@angband.org)
 */


/*
 * Approximate Distance between two points.
 *
 * When either the X or Y component dwarfs the other component,
 * this function is almost perfect, and otherwise, it tends to
 * over-estimate about one grid per fifteen grids of distance.
 *
 * Algorithm: hypot(dy,dx) = max(dy,dx) + min(dy,dx) / 2
 */
int distance(int y1, int x1, int y2, int x2)
{
	int dy, dx, d;

	/* Find the absolute y/x distance components */
	dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);
	dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);

	/* Hack -- approximate the distance */
	d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

	/* Return the distance */
	return (d);
}


/*
 * A simple, fast, integer-based line-of-sight algorithm.  By Joseph Hall,
 * 4116 Brewster Drive, Raleigh NC 27606.  Email to jnh@ecemwl.ncsu.edu.
 *
 * Returns TRUE if a line of sight can be traced from (x1,y1) to (x2,y2).
 *
 * The LOS begins at the center of the tile (x1,y1) and ends at the center of
 * the tile (x2,y2).  If los() is to return TRUE, all of the tiles this line
 * passes through must be floor tiles, except for (x1,y1) and (x2,y2).
 *
 * We assume that the "mathematical corner" of a non-floor tile does not
 * block line of sight.
 *
 * Because this function uses (short) ints for all calculations, overflow may
 * occur if dx and dy exceed 90.
 *
 * Once all the degenerate cases are eliminated, the values "qx", "qy", and
 * "m" are multiplied by a scale factor "f1 = abs(dx * dy * 2)", so that
 * we can use integer arithmetic.
 *
 * We travel from start to finish along the longer axis, starting at the border
 * between the first and second tiles, where the y offset = .5 * slope, taking
 * into account the scale factor.  See below.
 *
 * Also note that this function and the "move towards target" code do NOT
 * share the same properties.  Thus, you can see someone, target them, and
 * then fire a bolt at them, but the bolt may hit a wall, not them.  However,
 * by clever choice of target locations, you can sometimes throw a "curve".
 *
 * Note that "line of sight" is not "reflexive" in all cases.
 *
 * Use the "projectable()" routine to test "spell/missile line of sight".
 *
 * Use the "update_view()" function to determine player line-of-sight.
 */
bool los(int y1, int x1, int y2, int x2)
{
	/* Delta */
	int dx, dy;

	/* Absolute */
	int ax, ay;

	/* Signs */
	int sx, sy;

	/* Fractions */
	int qx, qy;

	/* Scanners */
	int tx, ty;

	/* Scale factors */
	int f1, f2;

	/* Slope, or 1/Slope, of LOS */
	int m;


	/* Extract the offset */
	dy = y2 - y1;
	dx = x2 - x1;

	/* Extract the absolute offset */
	ay = ABS(dy);
	ax = ABS(dx);


	/* Handle adjacent (or identical) grids */
	if ((ax < 2) && (ay < 2)) return (TRUE);


	/* Directly South/North */
	if (!dx)
	{
		/* South -- check for walls */
		if (dy > 0)
		{
			for (ty = y1 + 1; ty < y2; ty++)
			{
				if (!cave_floor_bold(ty, x1)) return (FALSE);
			}
		}

		/* North -- check for walls */
		else
		{
			for (ty = y1 - 1; ty > y2; ty--)
			{
				if (!cave_floor_bold(ty, x1)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}

	/* Directly East/West */
	if (!dy)
	{
		/* East -- check for walls */
		if (dx > 0)
		{
			for (tx = x1 + 1; tx < x2; tx++)
			{
				if (!cave_floor_bold(y1, tx)) return (FALSE);
			}
		}

		/* West -- check for walls */
		else
		{
			for (tx = x1 - 1; tx > x2; tx--)
			{
				if (!cave_floor_bold(y1, tx)) return (FALSE);
			}
		}

		/* Assume los */
		return (TRUE);
	}


	/* Extract some signs */
	sx = (dx < 0) ? -1 : 1;
	sy = (dy < 0) ? -1 : 1;


	/* Vertical "knights" */
	if (ax == 1)
	{
		if (ay == 2)
		{
			if (cave_floor_bold(y1 + sy, x1)) return (TRUE);
		}
	}

	/* Horizontal "knights" */
	else if (ay == 1)
	{
		if (ax == 2)
		{
			if (cave_floor_bold(y1, x1 + sx)) return (TRUE);
		}
	}


	/* Calculate scale factor div 2 */
	f2 = (ax * ay);

	/* Calculate scale factor */
	f1 = f2 << 1;


	/* Travel horizontally */
	if (ax >= ay)
	{
		/* Let m = dy / dx * 2 * (dy * dx) = 2 * dy * dy */
		qy = ay * ay;
		m = qy << 1;

		tx = x1 + sx;

		/* Consider the special case where slope == 1. */
		if (qy == f2)
		{
			ty = y1 + sy;
			qy -= f1;
		}
		else
		{
			ty = y1;
		}

		/* Note (below) the case (qy == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (x2 - tx)
		{
			if (!cave_floor_bold(ty, tx)) return (FALSE);

			qy += m;

			if (qy < f2)
			{
				tx += sx;
			}
			else if (qy > f2)
			{
				ty += sy;
				if (!cave_floor_bold(ty, tx)) return (FALSE);
				qy -= f1;
				tx += sx;
			}
			else
			{
				ty += sy;
				qy -= f1;
				tx += sx;
			}
		}
	}

	/* Travel vertically */
	else
	{
		/* Let m = dx / dy * 2 * (dx * dy) = 2 * dx * dx */
		qx = ax * ax;
		m = qx << 1;

		ty = y1 + sy;

		if (qx == f2)
		{
			tx = x1 + sx;
			qx -= f1;
		}
		else
		{
			tx = x1;
		}

		/* Note (below) the case (qx == f2), where */
		/* the LOS exactly meets the corner of a tile. */
		while (y2 - ty)
		{
			if (!cave_floor_bold(ty, tx)) return (FALSE);

			qx += m;

			if (qx < f2)
			{
				ty += sy;
			}
			else if (qx > f2)
			{
				tx += sx;
				if (!cave_floor_bold(ty, tx)) return (FALSE);
				qx -= f1;
				ty += sy;
			}
			else
			{
				tx += sx;
				qx -= f1;
				ty += sy;
			}
		}
	}

	/* Assume los */
	return (TRUE);
}






/*
 * Can the player "see" the given grid?
 *
 * He must have vision, illumination, and line of sight.
 *
 * If the player is "blind", then no grids can be "seen".
 *
 * Note that "CAVE_LITE" means that a grid is lit by the player's torch,
 * which means that the grid is "lit" and "viewable", so all such grids
 * can, by definition, be "seen".
 *
 * Note that "CAVE_GLOW" means that a grid is "permanently illuminated",
 * which, normally, means that if the grid is "viewable", then it can be
 * "seen".  But for wall/door grids, since they are not "translucent",
 * this is only true if, in addition, the grid which is closest to the
 * player and adjacent to the wall grid is also "permanently lit".
 *
 * This bizarre method for handling perma-lit wall grids may be more
 * expensive than simply assuming that perma-lit wall grids, like the
 * floor grids, can be seen when viewable, but it provides a somewhat
 * more "correct" semantics, in particular, it prevents the player from
 * "seeing" the wall of a room from a hallway running next to the room.
 *
 * Technically, we should check all eight grids adjacent to the wall grid,
 * and if any of them are "perma-lit" non-wall grids which are "viewable"
 * to the player, then the wall grid, if perma-lit itself, can be "seen",
 * but this is extremely expensive, and dangerous, if the wall grid is
 * adjacent to any "illegal" grids.  But the semantics would be slightly
 * better, for example, currently, wall grids which frame the "entrance"
 * to a lit room cannot be "seen" by a player in a dark hallway.
 *
 * We must be very careful about combining this function, in particular,
 * the parts which deal with "perma-lit" wall grids, with any optimized
 * algorithms, such as the "update_view()" code, which only calls the
 * "note_spot()" function (which uses an "inline" version of this code)
 * for grids which "change" viewability, since strange things may happen
 * if we are not careful.  For example, when a player enters a lit room,
 * from a dark hall, through a closed door, with no torch, we must ensure
 * that the walls adjacent to the door are "seen" from the doorway.
 *
 * The "doorway" example is responsible for the "optional" method shown
 * below, in which we check three adjacent grids instead of just a single
 * adjacent grid, and we forbid these grids from being walls.
 *
 * See also the various "inline" versions of "player_can_see_bold()", for
 * example, in the "note_spot()" and "map_info()" functions.
 */
bool player_can_see_bold(int y, int x)
{
	/* Blind players see nothing */
	if (p_ptr->blind) return (FALSE);

	/* Note that "torch-lite" yields "illumination" */
	if (cave_info[y][x] & (CAVE_LITE)) return (TRUE);

	/* Require line of sight to the grid */
	if (!player_has_los_bold(y, x)) return (FALSE);

	/* Require "perma-lite" of the grid */
	if (!(cave_info[y][x] & (CAVE_GLOW))) return (FALSE);

	/* Floors are simple */
	if (cave_floor_bold(y, x)) return (TRUE);

	/* Walls */
	if (TRUE)
	{
		int py = p_ptr->py;
		int px = p_ptr->px;

		/* Hack -- move towards player */
		int yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
		int xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

#if 0
		/* Alternative method */
		if ((cave_floor_bold(yy, xx) && (cave_info[yy][xx] & (CAVE_GLOW))) ||
		    (cave_floor_bold(y, xx) && (cave_info[y][xx] & (CAVE_GLOW))) ||
		    (cave_floor_bold(yy, x) && (cave_info[yy][x] & (CAVE_GLOW))))
		{
			/* Assume the wall is really illuminated */
			return (TRUE);
		}
#endif

		/* Check for "local" illumination */
		if (cave_info[yy][xx] & (CAVE_GLOW))
		{
			/* Assume the wall is really illuminated */
			return (TRUE);
		}
	}

	/* Assume not visible */
	return (FALSE);
}



/*
 * Returns true if the player's grid is dark
 */
bool no_lite(void)
{
	int px = p_ptr->px;
	int py = p_ptr->py;

	return (!player_can_see_bold(py, px));
}




/*
 * Determine if a given location may be "destroyed"
 *
 * Used by destruction spells, and for placing stairs, etc.
 */
bool cave_valid_bold(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Forbid perma-grids */
	if (cave_perma_bold(y, x)) return (FALSE);

	/* Check objects */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Acquire object */
		o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Forbid artifact grids */
		if (allart_p(o_ptr)) return (FALSE);
	}

	/* Accept */
	return (TRUE);
}




/*
 * Hack -- Legal monster codes
 */
static cptr image_monster_hack = \
"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

static cptr image_monster_hack_ibm = \
"\006\026\037\311\321\326\334\341\340\342\344\347\351\350\352\353\355\357\361\364\366\365\372\371\373\374\260\200\256\251\201\330\203\276\204\245\265\217\205\275\346\253\303\213\214\271\327\215\360\336\376\375\267\222\223\302\313\310\312\315\316\317\314\323\324\225\322\332\333\331\236\226\227\257\230\231\232\270\233\234";


/*
 * Mega-Hack -- Hallucinatory monster
 */
static void image_monster(byte *ap, char *cp)
{
	if (use_graphics)
	{
		int n = strlen(image_monster_hack);
		(*cp) = (image_monster_hack[rand_int(n)]);

		/* Random color */
		(*ap) = randint(15);
	}
	else if (streq(ANGBAND_SYS, "ibm"))
	{
		int n = strlen(image_monster_hack_ibm);
		(*cp) = (image_monster_hack_ibm[rand_int(n)]);
		/* Random color */
		(*ap) = randint(15);
	}
	else
	{
		monster_race *r_ptr;
		do
		{
			r_ptr = r_info+rand_int(MAX_R_IDX);
		}
		while (is_fake_monster(r_ptr));

		(*cp) = r_ptr->gfx.xc;
		(*ap) = r_ptr->gfx.xa;
	}
}




/*
 * Hack -- Legal object codes
 */
static cptr image_object_hack = \
"?/|\\\"!$()_-=[]{},~";

static cptr image_object_hack_ibm = \
"����󧶴�Ɔ������������Ց���";

/*
 * Mega-Hack -- Hallucinatory object
 */
static void image_object(byte *ap, char *cp)
{
	int n = strlen(image_object_hack);

	if (use_graphics)
	{
		(*cp) = (image_object_hack[rand_int(n)]);
		/* Random color */
		(*ap) = randint(15);
	}
	else if (streq(ANGBAND_SYS, "ibm"))
	{
		n = strlen(image_object_hack_ibm);
		(*cp) = (image_object_hack_ibm[rand_int(n)]);
		/* Random color */
		(*ap) = randint(15);
	}
	else
	{
		(*cp) = object_kind_char(&k_info[randint(MAX_K_IDX-1)]);
		(*ap) = object_kind_attr(&k_info[randint(MAX_K_IDX-1)]);
	}
}


/*
 * Hack -- Random hallucination
 */
static void image_random(byte *ap, char *cp)
{
	/* Normally, assume monsters */
	if (rand_int(100) < 75)
	{
		image_monster(ap, cp);
	}

	/* Otherwise, assume objects */
	else
	{
		image_object(ap, cp);
	}
}


/*
 * Change the colour of a monster if the player uses the look function.
 *
 * Hack - colour-changing uniques don't reach this function when they're doing
 * something special.
 */
static bool do_violet_unique(monster_race *r_ptr, byte *ap, char *cp)
{
	int a;

	/* This isn't the correct turn. */
	if (!violet_uniques) return FALSE;

	/* Multi-hued monsters are handled elsewhere. */
	if (r_ptr->flags1 & RF1_ATTR_MULTI) return FALSE;

	/* Uniques usually become violet. */
	if (r_ptr->flags1 & RF1_UNIQUE) a = TERM_VIOLET;

	/* Monsters which would otherwise be invisible become red. */
	else if ((*ap) == r_ptr->gfx.xa && (*cp) == r_ptr->gfx.xc) a = TERM_RED;

	/* Only the above types of monster are modified here. */
	else return FALSE;

	/* Monsters which are the colour in question anyway become yellow. */
	if (a == r_ptr->gfx.xa) a = TERM_YELLOW;

	/* Store the result. */
	(*ap) = a;
	(*cp) = r_ptr->gfx.xc;

	/* Let the game know that something has happened. */
	return TRUE;
}


/*
 * Extract the attr/char to display at the given (legal) map location
 *
 * Basically, we "paint" the chosen attr/char in several passes, starting
 * with any known "terrain features" (defaulting to darkness), then adding
 * any known "objects", then adding any known "monsters", and then adding
 * the player if needed.  This is not the fastest method but since most of
 * the calls to this function are made for grids with no objects, monsters,
 * or players, it should be fast enough.
 *
 * Note that the "zero" entry in the feature/object/monster arrays are
 * used to provide "special" attr/char codes, with "monster zero" being
 * used for the player attr/char, "object zero" being used for the "stack"
 * attr/char, and "feature zero" being used for the "nothing" attr/char,
 * though this function makes use of only "feature zero".  XXX XXX XXX
 *
 * Note that monsters can have some "special" flags, including "ATTR_MULTI",
 * which means their color changes, and "ATTR_CLEAR", which means they take
 * the color of whatever is under them, and "CHAR_CLEAR", which means that
 * they take the symbol of whatever is under them.  Technically, the flag
 * "CHAR_MULTI" is supposed to indicate that a monster looks strange when
 * examined, but this flag is currently ignored.  All of these flags are
 * ignored if the "avoid_other" option is set, since checking for these
 * conditions is expensive and annoying on some systems.
 *
 * Currently, we do nothing with multi-hued objects, because there are
 * not any.  If there were, they would have to set "shimmer_objects"
 * when they were created, and then new "shimmer" code in "dungeon.c"
 * would have to be created handle the "shimmer" effect, and the code
 * in "cave.c" would have to be updated to create the shimmer effect.
 *
 * Note the effects of hallucination.  Objects always appear as random
 * "objects", monsters as random "monsters", and normal grids occasionally
 * appear as random "monsters" or "objects", but note that these random
 * "monsters" and "objects" are really just "colored ascii symbols".
 *
 * Note that "floors" and "invisible traps" (and "zero" features) are
 * drawn as "floors" using a special check for optimization purposes,
 * and these are the only features which get drawn using the special
 * lighting effects activated by "view_special_lite".
 *
 * Note the use of the "mimic" field in the "terrain feature" processing,
 * which allows any feature to "pretend" to be another feature.  This is
 * used to "hide" secret doors, and to make all "doors" appear the same,
 * and all "walls" appear the same, and "hidden" treasure stay hidden.
 * It is possible to use this field to make a feature "look" like a floor,
 * but the "special lighting effects" for floors will not be used.
 *
 * Note the use of the new "terrain feature" information.  Note that the
 * assumption that all interesting "objects" and "terrain features" are
 * memorized allows extremely optimized processing below.  Note the use
 * of separate flags on objects to mark them as memorized allows a grid
 * to have memorized "terrain" without granting knowledge of any object
 * which may appear in that grid.
 *
 * Note the efficient code used to determine if a "floor" grid is
 * "memorized" or "viewable" by the player, where the test for the
 * grid being "viewable" is based on the facts that (1) the grid
 * must be "lit" (torch-lit or perma-lit), (2) the grid must be in
 * line of sight, and (3) the player must not be blind, and uses the
 * assumption that all torch-lit grids are in line of sight.
 *
 * Note that floors (and invisible traps) are the only grids which are
 * not memorized when seen, so only these grids need to check to see if
 * the grid is "viewable" to the player (if it is not memorized).  Since
 * most non-memorized grids are in fact walls, this induces *massive*
 * efficiency, at the cost of *forcing* the memorization of non-floor
 * grids when they are first seen.  Note that "invisible traps" are
 * always treated exactly like "floors", which prevents "cheating".
 *
 * Note the "special lighting effects" which can be activated for floor
 * grids using the "view_special_lite" option (for "white" floor grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the grid is "dark", we will use "dark gray", else
 * if the grid is not "viewable", and the "view_bright_lite" option is
 * set, and the we will use "slate" (gray).  We will use "white" for all
 * other cases, in particular, for illuminated viewable floor grids.
 *
 * Note the "special lighting effects" which can be activated for wall
 * grids using the "view_granite_lite" option (for "white" wall grids),
 * causing certain grids to be displayed using special colors.  If the
 * player is "blind", we will use "dark gray", else if the grid is lit
 * by the torch, and the "view_yellow_lite" option is set, we will use
 * "yellow", else if the "view_bright_lite" option is set, and the grid
 * is not "viewable", or is "dark", or is glowing, but not when viewed
 * from the player's current location, we will use "slate" (gray).  We
 * will use "white" for all other cases, in particular, for correctly
 * illuminated viewable wall grids.
 *
 * Note that, when "view_granite_lite" is set, we use an inline version
 * of the "player_can_see_bold()" function to check the "viewability" of
 * grids when the "view_bright_lite" option is set, and we do NOT use
 * any special colors for "dark" wall grids, since this would allow the
 * player to notice the walls of illuminated rooms from a hallway that
 * happened to run beside the room.
 *
 * Note that bizarre things must be done when the "attr" and/or "char"
 * codes have the "high-bit" set, since these values are used to encode
 * various "special" pictures in some versions, and certain situations,
 * such as "multi-hued" or "clear" monsters, cause the attr/char codes
 * to be "scrambled" in various ways.
 *
 * Note that eventually we may use the "&" symbol for embedded treasure,
 * and use the "*" symbol to indicate multiple objects, though this will
 * have to wait for Angband 2.8.2 or later.  Currently, we simply use
 * the attr/char of the first "marked" object in the stack.  If we did
 * use some special symbol, it could be stored in "f_info[0]".
 *
 * Note the assumption that doing "x_ptr = &x_info[x]" plus a few of
 * "x_ptr->xxx", is quicker than "x_info[x].xxx", if this is incorrect
 * then a whole lot of code should be changed...  XXX XXX XXX
 */
void map_info(int y, int x, byte *ap, char *cp, byte *tap, char *tcp)
{
	feature_type *f_ptr;

	object_type *o_ptr;
	bool seen_obj;

	int feat;

	byte a;
	char c;

	/* Illegal locations are simple. */
	if (!in_bounds(y, x))
	{
		f_ptr = f_info+FEAT_ILLEGAL;
		*ap = f_ptr->gfx.xa;
		*cp = f_ptr->gfx.xc;

		*tap = f_ptr->gfx.xa;
		*tcp = f_ptr->gfx.xc;

		return;
	}

	/* Feature code */
	feat = cave_rfeat[y][x];

	/* Floors (etc) */
	if ((feat <= FEAT_INVIS) || (feat == FEAT_PATH))
	{
		/* Memorized (or visible) floor */
		if ((cave_info[y][x] & (CAVE_MARK)) ||
			(((cave_info[y][x] & (CAVE_LITE)) ||
				((cave_info[y][x] & (CAVE_GLOW)) &&
				(cave_info[y][x] & (CAVE_VIEW)))) &&
			!p_ptr->blind))
		{
			/* Access floor */
			f_ptr = &f_info[FEAT_FLOOR];

			/* Hack - path uses a different graphic */
			if (feat == FEAT_PATH) f_ptr = &f_info[FEAT_PATH];

			/* Normal char */
			c = f_ptr->gfx.xc;

			/* Normal attr */
			a = f_ptr->gfx.xa;

			/* Special lighting effects */
			if (view_special_lite && ((a == TERM_WHITE) || use_graphics))
			{
				/* Handle "blind" */
				if (p_ptr->blind)
				{
					if (use_graphics)
					{
						/* Use a dark tile */
						c++;
					}
					else
					{
						/* Use "dark grey" */
						a = TERM_L_DARK;
					}
				}

				/* Handle "torch-lit" grids */
				else if (cave_info[y][x] & (CAVE_LITE))
				{
					/* Torch lite */
					if (view_yellow_lite)
					{
						if (use_graphics)
						{
							/* Use a brightly lit tile */
							c += 2;
						}
						else
						{
							/* Use "yellow" */
							a = TERM_YELLOW;
						}
					}
				}

				/* Handle "dark" grids */
				else if (!(cave_info[y][x] & (CAVE_GLOW)))
				{
					if (use_graphics)
					{
						/* Use a dark tile */
						c++;
					}
					else
					{
						/* Use "dark grey" */
						a = TERM_L_DARK;
					}
				}

				/* Handle "out-of-sight" grids */
				else if (!(cave_info[y][x] & (CAVE_VIEW)))
				{
					/* Special flag */
					if (view_bright_lite)
					{
						if (use_graphics)
						{
							/* Use a dark tile */
							c++;
						}
						else
						{
							/* Use "grey" */
							a = TERM_SLATE;
						}
					}
				}
			}
		}

		/* Unknown */
		else
		{
			if (cave_info[y][x] & CAVE_TRAP)
			{
				f_ptr = &f_info[FEAT_NONE_TD];
			}
			else
			{
				/* Access darkness */
				f_ptr = &f_info[FEAT_NONE];
			}

			/* Normal attr */
			a = f_ptr->gfx.xa;

			/* Normal char */
			c = f_ptr->gfx.xc;
		}
	}

	/* Non floors */
	else
	{
		/* Memorized grids */
		if (cave_info[y][x] & (CAVE_MARK))
		{
			/* Apply "mimic" field */
			feat = f_info[feat].mimic;

			/* Access feature */
			f_ptr = &f_info[feat];

			/* Normal char */
			c = f_ptr->gfx.xc;

			/* Normal attr */
			a = f_ptr->gfx.xa;

			/* Special lighting effects */
			if (view_granite_lite && ((a == TERM_WHITE) || (use_graphics)) &&
				(((feat >= FEAT_SECRET) && (feat <= FEAT_PERM_SOLID) &&
				(feat !=FEAT_MAGMA_K) && (feat !=FEAT_QUARTZ_K) &&
				(feat !=FEAT_RUBBLE)) || ((feat == FEAT_TREE) || (feat == FEAT_BUSH)
				|| (feat == FEAT_WATER))))
			{
				/* Handle "blind" */
				if (p_ptr->blind)
				{
					if (use_graphics)
					{
						/* Use a dark tile */
						c++;
					}
					else
					{
						/* Use "dark grey" */
						a = TERM_L_DARK;
					}
				}

				/* Handle "torch-lit" grids */
				else if (cave_info[y][x] & (CAVE_LITE))
				{
					/* Torch lite */
					if (view_yellow_lite)
					{
						if (use_graphics)
						{
							/* Use a brightly lit tile */
							c += 2;
						}
						else
						{
							/* Use "yellow" */
							a = TERM_YELLOW;
						}
					}
				}

				/* Handle "view_bright_lite" */
				else if (view_bright_lite)
				{
					/* Not viewable */
					if (!(cave_info[y][x] & (CAVE_VIEW)))
					{
						if (use_graphics)
						{
							/* Use a lit tile */
						if ((feat == FEAT_TREE) || (feat == FEAT_BUSH) || (feat == FEAT_WATER))
							{
								c++;
							}
						}
						else
						{
							/* Use "grey" */
							a = TERM_SLATE;
						}
					}

					/* Not glowing */
					else if (!(cave_info[y][x] & (CAVE_GLOW)))
					{
						if (use_graphics)
						{
							/* Use a lit tile */
							if ((feat == FEAT_TREE) || (feat == FEAT_BUSH) || (feat == FEAT_WATER))
							{
								c++;
							}
						}
						else
						{
							/* Use "grey" */
							a = TERM_SLATE;
						}
					}

					/* Not glowing correctly */
					else
					{
						int py = p_ptr->py;
						int px = p_ptr->px;

						/* Hack -- move towards player */
						int yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
						int xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

						/* Check for "local" illumination */
						if (!(cave_info[yy][xx] & (CAVE_GLOW)))
						{
							if (use_graphics)
							{
								/* Use a lit tile */
							}
							else
							{
								/* Use "grey" */
								a = TERM_SLATE;
							}
						}
					}
				}
			}
		}

		/* Unknown */
		else
		{
			if (cave_info[y][x] & CAVE_TRAP)
			{
				f_ptr = &f_info[FEAT_NONE_TD];
			}
			else
			{
			/* Access darkness */
			f_ptr = &f_info[FEAT_NONE];
			}


			/* Normal attr */
			a = f_ptr->gfx.xa;

			/* Normal char */
			c = f_ptr->gfx.xc;
		}
	}

	/* Hack -- rare random hallucination, except on outer dungeon walls */
	if (p_ptr->image && (!rand_int(256)) && (cave_rfeat[y][x] < FEAT_PERM_SOLID))
	{
		/* Hallucinate */
		image_random(ap, cp);
	}

	/* Save the terrain info for the transparency effects */
	(*tap) = a;
	(*tcp) = c;

	/* Save the info */
	(*ap) = a;
	(*cp) = c;

	/* Objects */
	for (o_ptr = o_list+cave_o_idx[y][x], seen_obj = FALSE; o_ptr != o_list;
		o_ptr = o_list+o_ptr->next_o_idx)
	{
		/* Only show memorised objects. */
		if (!o_ptr->marked)
		{
			continue;
		}
		/* Hack -- hallucination */
		else if (p_ptr->image)
		{
			image_object(ap, cp);
			break;
		}
		else if (seen_obj)
		{
			/* Use a special stack object, if required. */
			(*cp) = object_kind_char(k_info+OBJ_STACK);
			(*ap) = object_kind_attr(k_info+OBJ_STACK);
			break;
		}
		else
		{
			/* Normal char */
			(*cp) = object_char(o_ptr);

			/* Normal attr */
			(*ap) = object_attr(o_ptr);

			/* Done, if no special stack image is being used. */
			if (!show_piles) break;

			/* One object has been seen on this square. */
			seen_obj = TRUE;
		}
	}


	/* Handle monsters */
	if (cave_m_idx[y][x] > 0)
	{
		monster_type *m_ptr = &m_list[cave_m_idx[y][x]];

		/* Visible monster */
		if (m_ptr->ml)
		{
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Desired attr */
			a = r_ptr->gfx.xa;

			/* Desired char */
			c = r_ptr->gfx.xc;

			/* Ignore weird codes */
			if (avoid_other)
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Special attr/char codes */
			else if ((a & 0x80) && (c & 0x80))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}


			/* Hack - allow non-clear uniques to be set uniformly violet,
			or yellow if violet normally. */
			else if (do_violet_unique(r_ptr, ap, cp));
		/* Multi-hued monster */
		else if (r_ptr->flags1 & (RF1_ATTR_MULTI))
			{
		/* Is it a shapechanger? */
		if (r_ptr->flags2 & (RF2_SHAPECHANGER))
		{
			if (use_graphics)
			{
				image_monster(ap, cp);
			}
			else
			{
				(*cp) = (randint(25)==1?
				image_object_hack[randint(strlen(image_object_hack))]:
				image_monster_hack[randint(strlen(image_monster_hack))]);
			}
		}
		else
			(*cp) = c;

				/* Multi-hued attr */
		if (r_ptr->flags2 & (RF2_ATTR_ANY))
				(*ap) = randint(15);
		else switch (randint(7))
						{  case 1:
						(*ap)=TERM_RED;
						break;
					case 2:
						(*ap)=TERM_L_RED;
						break;
					case 3:
			(*ap)=TERM_WHITE;
						break;
					case 4:
						(*ap)=TERM_L_GREEN;
						break;
					case 5:
						(*ap)=TERM_BLUE;
						break;
					case 6:
			(*ap)=TERM_L_DARK;
						break;
			case 7:
			(*ap)=TERM_GREEN;
			break;
						}
			}

			/* Normal monster (not "clear" in any way) */
			else if (!(r_ptr->flags1 & (RF1_ATTR_CLEAR | RF1_CHAR_CLEAR)))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Hack -- Bizarre grid under monster */
			else if ((*ap & 0x80) || (*cp & 0x80))
			{
				/* Use char */
				(*cp) = c;

				/* Use attr */
				(*ap) = a;
			}

			/* Normal */
			else
			{
				/* Normal (non-clear char) monster */
				if (!(r_ptr->flags1 & (RF1_CHAR_CLEAR)))
				{
					/* Normal char */
					(*cp) = c;
				}

				/* Normal (non-clear attr) monster */
				else if (!(r_ptr->flags1 & (RF1_ATTR_CLEAR)))
				{
					/* Normal attr */
					(*ap) = a;
				}
			}

			/* Hack -- hallucination */
			if (p_ptr->image)
			{
				/* Hallucinatory monster */
				image_monster(ap, cp);
			}
		}
	}

	/* Handle "player" */
	if (cave_m_idx[y][x] < 0)
	{
		monster_race *r_ptr = &r_info[0];

		/* Get the "player" attr */
		a = r_ptr->gfx.xa;

		/* Get the "player" char */
		c = r_ptr->gfx.xc;

		/* Save the info */
		(*ap) = a;
		(*cp) = c;
	}
}

/* A factor to convert between map and screen co-ordinates. */

#define Y_SCREEN_ADJ (panel_row_prt-PRT_MINY)
#define X_SCREEN_ADJ (panel_col_prt-PRT_MINX)

/*
 * Does the (x,y) point on the map appear on-screen?
 */
/* static bool panel_contains_prt(int y, int x)
{
	y -= Y_SCREEN_ADJ;
	x -= X_SCREEN_ADJ;
	if (y < PRT_MINY || y >= PRT_MAXY) return FALSE;
	if (x < PRT_MINX || x >= PRT_MAXX) return FALSE;
	return TRUE;
} */


/*
 * Move the cursor to a given map location
 */
void move_cursor_relative(int y, int x)
{
	unsigned ky, kx;
	unsigned vy, vx;

	/* Location relative to panel */
	ky = (unsigned)(y - p_ptr->wy);

	/* Verify location */
	if (ky >= (unsigned)(SCREEN_HGT)) return;

	/* Location relative to panel */
	kx = (unsigned)(x - p_ptr->wx);

	/* Verify location */
	if (kx >= (unsigned)(SCREEN_WID)) return;

	/* Location in window */
	vy = ky + ROW_MAP;

	/* Verify location */
	if (vy >= (unsigned)(Term->hgt)) return;

	/* Location in window */
	vx = kx + COL_MAP;

	/* Verify location */
	if (vx >= (unsigned)(Term->wid)) return;

	/* Go there */
	Term_gotoxy(vx, vy);
}



/*
 * Hack - provide fake monochrome under certain circumstances.
 */
static void fake_colour(byte *a)
{
	if (!allow_fake_colour) return;

	/* Graphics prevent fake colours, except with main-ibm.c. */
	if (use_graphics && strcmp(ANGBAND_SYS, "ibm")) return;

	if (p_ptr->invuln) *a = TERM_WHITE;
	else if (p_ptr->wraith_form) *a = TERM_L_DARK;
}

/*
 * Display an attr/char pair at the given map location
 *
 * Note the inline use of "panel_contains()" for efficiency.
 *
 * Note the use of "Term_queue_char()" for efficiency.
 */
void print_rel(char c, byte a, int y, int x)
{
	unsigned ky, kx;
	unsigned vy, vx;

	/* Location relative to panel */
	ky = (unsigned)(y - p_ptr->wy);

	/* Verify location */
	if (ky >= (unsigned)(SCREEN_HGT)) return;

	/* Location relative to panel */
	kx = (unsigned)(x - p_ptr->wx);

	/* Verify location */
	if (kx >= (unsigned)(SCREEN_WID)) return;

	/* Location in window */
	vy = ky + ROW_MAP;

	/* Verify location */
	if (vy >= (unsigned)(Term->hgt)) return;

	/* Location in window */
	vx = kx + COL_MAP;

	/* Verify location */
	if (vx >= (unsigned)(Term->wid)) return;

	/* Hack -- Queue it */
	Term_queue_char(vx, vy, a, c, 0, 0);
}

/*
 * Activate various external displays based on the contents of a specified
 * map square.
 */
static void highlight_map_square(const int y, const int x)
{
	monster_type *m_ptr = m_list+cave_m_idx[y][x];
	object_type *o_ptr = o_list+cave_o_idx[y][x];

	/* Track a monster, if any. */
	if (cave_m_idx[y][x] && m_ptr->ml)
	{
		health_track(cave_m_idx[y][x]);
		monster_race_track(m_ptr->r_idx);
	}

	/* Track an object, if any. */
	if (cave_o_idx[y][x] && o_ptr->marked)
	{
		object_track(o_ptr);
	}

	/* Track the square. */
	cave_track(y, x);
}

/*
 * Highlight a square in a particular window.
 */
void highlight_square(int win, int y, int x)
{
	static int win2=-1, y2, x2;

	/* Do nothing if the cursor remains within the same square. */
	if (win == win2 && y == y2 && x == x2) return;

	/* Remember the square for next time. */
	win2 = win;
	y2 = y;
	x2 = x;

	/* If over a known section of map, display details.
	 * Note that this does not check where the map is actually visible.
	 */
	if (character_dungeon && track_mouse && win == 0)
	{
		unsigned ky, kx;
		unsigned vy, vx;

		/* Location relative to panel */
		ky = (unsigned)(y - p_ptr->wy);

		/* Verify location */
		if (ky >= (unsigned)(SCREEN_HGT)) return;

		/* Location relative to panel */
		kx = (unsigned)(x - p_ptr->wx);

		/* Verify location */
		if (kx >= (unsigned)(SCREEN_WID)) return;

		/* Location in window */
		vy = ky + ROW_MAP;

		/* Verify location */
		if (vy >= (unsigned)(Term->hgt)) return;

		/* Location in window */
		vx = kx + COL_MAP;

		/* Verify location */
		if (vx >= (unsigned)(Term->wid)) return;
	
		highlight_map_square(vy, vx);
	}
}




/*
 * Permanently learn the identity of the feature in a square.
 */
void mark_spot(int y, int x)
{
	/* Remember the feature in this square. */
	cave_rfeat[y][x] = cave_feat[y][x];

	/* Remember that the feature should be displayed. */
	cave_info[y][x] |= CAVE_MARK;
}


/*
 * Memorize interesting viewable object/features in the given grid
 *
 * This function should only be called on "legal" grids.
 *
 * This function will memorize the object and/or feature in the given
 * grid, if they are (1) viewable and (2) interesting.  Note that all
 * objects are interesting, all terrain features except floors (and
 * invisible traps) are interesting, and floors (and invisible traps)
 * are interesting sometimes (depending on various options involving
 * the illumination of floor grids).
 *
 * The automatic memorization of all objects and non-floor terrain
 * features as soon as they are displayed allows incredible amounts
 * of optimization in various places, especially "map_info()".
 *
 * Note that the memorization of objects is completely separate from
 * the memorization of terrain features, preventing annoying floor
 * memorization when a detected object is picked up from a dark floor,
 * and object memorization when an object is dropped into a floor grid
 * which is memorized but out-of-sight.
 *
 * This function should be called every time the "memorization" of
 * a grid (or the object in a grid) is called into question, such
 * as when an object is created in a grid, when a terrain feature
 * "changes" from "floor" to "non-floor", when any grid becomes
 * "illuminated" or "viewable", and when a "floor" grid becomes
 * "torch-lit".
 *
 * Note the relatively efficient use of this function by the various
 * "update_view()" and "update_lite()" calls, to allow objects and
 * terrain features to be memorized (and drawn) whenever they become
 * viewable or illuminated in any way, but not when they "maintain"
 * or "lose" their previous viewability or illumination.
 *
 * Note the butchered "internal" version of "player_can_see_bold()",
 * optimized primarily for the most common cases, that is, for the
 * non-marked floor grids.
 */
void note_spot(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Blind players see nothing */
	if (p_ptr->blind) return;

	/* Analyze non-torch-lit grids */
	if (!(cave_info[y][x] & (CAVE_LITE)))
	{
		/* Require line of sight to the grid */
		if (!(cave_info[y][x] & (CAVE_VIEW))) return;

		/* Require "perma-lite" of the grid */
		if (!(cave_info[y][x] & (CAVE_GLOW))) return;
	}


	/* Hack -- memorize objects */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr = &o_list[this_o_idx];

		/* Acquire next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorize legal objects */
		if (!hidden_p(o_ptr)) o_ptr->marked = TRUE;
	}


	/* Hack -- memorise features */
	cave_rfeat[y][x] = cave_feat[y][x];

	/* Hack -- memorize grids */
	if (!(cave_info[y][x] & (CAVE_MARK)))
	{
		/* Handle floor grids first */
		if (cave_feat[y][x] <= FEAT_INVIS)
		{
			/* Option -- memorize all torch-lit floors */
			if (view_torch_grids && (cave_info[y][x] & (CAVE_LITE)))
			{
				/* Memorize */
				cave_info[y][x] |= (CAVE_MARK);
			}

			/* Option -- memorize all perma-lit floors */
			else if (view_perma_grids && (cave_info[y][x] & (CAVE_GLOW)))
			{
				/* Memorize */
				cave_info[y][x] |= (CAVE_MARK);
			}
		}

		/* Memorize normal grids */
		else if (cave_floor_bold(y,x))
		{
			/* Memorize */
			cave_info[y][x] |= (CAVE_MARK);
		}

		/* Memorize torch-lit walls */
		else if (cave_info[y][x] & (CAVE_LITE))
		{
			/* Memorize */
			cave_info[y][x] |= (CAVE_MARK);
		}

		/* Memorize certain non-torch-lit wall grids */
		else
		{
			int px = p_ptr->px;
			int py = p_ptr->py;

			/* Hack -- move one grid towards player */
			int yy = (y < py) ? (y + 1) : (y > py) ? (y - 1) : y;
			int xx = (x < px) ? (x + 1) : (x > px) ? (x - 1) : x;

			/* Check for "local" illumination */
			if (cave_info[yy][xx] & (CAVE_GLOW))
			{
				/* Memorize */
				cave_info[y][x] |= (CAVE_MARK);
			}
		}
	}
}

static byte priority_map[MAX_HGT][MAX_WID];

/*
 * Redraw (on the screen) a given map location
 *
 * Note the inline use of "panel_contains()" for efficiency.
 *
 * Note the use of "Term_queue_char()" for efficiency.
 */
void lite_spot(int y, int x)
{
	byte a, da;
	char c, dc;

	unsigned ky, kx;
	unsigned vy, vx;

	/* Recalculate the priority. */
	priority_map[y][x]=0;

	/* Location relative to panel */
	ky = (unsigned)(y - p_ptr->wy);

	/* Verify location */
	if (ky >= (unsigned)(SCREEN_HGT)) return;

	/* Location relative to panel */
	kx = (unsigned)(x - p_ptr->wx);

	/* Verify location */
	if (kx >= (unsigned)(SCREEN_WID)) return;

	/* Location in window */
	vy = ky + ROW_MAP;

	/* Verify location */
	if (vy >= (unsigned)(Term->hgt)) return;

	/* Location in window */
	vx = kx + COL_MAP;

	/* Verify location */
	if (vx >= (unsigned)(Term->wid)) return;

	/* Examine the grid */
	map_info(y, x, &a, &c, &da, &dc);

	/* Hack -- Queue it */
	Term_queue_char(vx, vy, a, c, da, dc);
}




/*
 * Prints the map of the dungeon
 *
 * Note that, for efficiency, we contain an "optimized" version
 * of both "lite_spot()" and "print_rel()", and that we use the
 * "lite_spot()" function to display the player grid, if needed.
 *
 * reset controls whether the game should reset the priority map. This is
 * usually the case, as this function redraws the map for arbitrary changes,
 * but panel changes do not require this.
 */
void prt_map(void)
{
	byte a;
	char c;

	int y, x;
	int vy, vx;
	int ty, tx;

	/* Assume screen */
	ty = ROW_MAP + SCREEN_HGT;
	tx = COL_MAP + SCREEN_WID;

	/* Take account of reduced windows */
	if (ty > Term->hgt) ty = Term->hgt;
	if (tx > Term->wid) tx = Term->wid;

	/* Dump the map */
	for (y = p_ptr->wy, vy = ROW_MAP; vy < ty; vy++, y++)
	{
		for (x = p_ptr->wx, vx = COL_MAP; vx < tx; vx++, x++)
		{
		        byte ta;
			char tc;

			/* Determine what is there */
			map_info(y, x, &a, &c, &ta, &tc);

			/* Hack -- Queue it */
			Term_queue_char(vx, vy, a, c, ta, tc);
		}
	}
}





/*
 * Hack -- a priority function (see below)
 *
 * This uses the priorities of terrians which are never shown because they are
 * mimics, as these can be capable of being shown if f_info.txt is written in
 * an obfuscated way.
 */
static int get_priority(int y, int x)
{
	char c, dc;
	byte a, da;
	int i;
	feature_type *f_ptr = f_info+f_info[cave_feat[y][x]].mimic;

	/* Extract the image of the square. */
	map_info(y, x, &a, &c, &da, &dc);

	/* Use the priority of this terrain type if the terrain is visible. */
	if (f_ptr->gfx.xc == c && f_ptr->gfx.xa == a) return f_ptr->priority;

	/* Otherwise, look to see if it looks like terrain. */
	for (i = 0; i < feature_priorities; i++)
	{
		f_ptr = priority_table[i];
		if (f_ptr->gfx.xc == c && f_ptr->gfx.xa == a) return f_ptr->priority;
	}

	/* Not the char/attr of a feature, so give a known priority. */
	return 100;
}

/*
 * Deduce the grid with the highest priority (as determined above) in the
 * specified square area of the level.
 */
static void get_best_priority(byte *a, char *c,
	int minx, int miny, int maxx, int maxy)
{
	int x, y, mx, my, mpri;
	byte da[1];
	char dc[1];

	for (mpri = my = mx = 0, x = minx; x <= maxx; x++)
	{
		for (y = miny; y <= maxy; y++)
		{
			int pri = priority_map[y][x];
			if (!pri) priority_map[y][x] = pri = get_priority(y, x);

			if (pri > mpri)
			{
				mpri = pri;
				mx = x;
				my = y;
			}
		}
	}

	/* Extract the image of the "best" grid in the area. */
	map_info(my, mx, a, c, da, dc);
}

/*
 * Display a "small-scale" map of the dungeon in the active Term
 *
 * Note that this function must "disable" the special lighting
 * effects so that the "priority" function will work.
 *
 * Note the use of a specialized "priority" function to allow this
 * function to work with any graphic attr/char mappings, and the
 * attempts to optimize this function where possible.
 *
 * If present, cy and cx are set to the player's co-ordinates.
 * If present, my and mx are set to the bottom right corner of the map.
 */
void display_map(int *cy, int *cx, int *my, int *mx)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int x, y;

	byte ta;
	char tc;

	bool old_view_special_lite;
	bool old_view_granite_lite;

	int ratio, map_hgt, map_wid;

	Term_get_size(&map_wid, &map_hgt);

	ratio = MAX(MAX((cur_wid+map_wid-1)/map_wid, (cur_hgt+map_hgt-1)/map_hgt), 1);

	map_hgt = (cur_hgt+ratio-1)/ratio + 1;
	map_wid = (cur_wid+ratio-1)/ratio + 1;

	/* Save lighting effects */
	old_view_special_lite = view_special_lite;
	old_view_granite_lite = view_granite_lite;

	/* Draw the horizontal edges */
	mc_put_fmt(0, 0, "+%v+", repeat_string_f2, "-", map_wid-1);
	mc_put_fmt(map_hgt, 0, "+%v+", repeat_string_f2, "-", map_wid-1);

	/* Disable lighting effects */
	view_special_lite = FALSE;
	view_granite_lite = FALSE;

	/* Display each map line in order */
	for (y = 1; y < map_hgt; ++y)
	{
		/* Start a new line */
		Term_putch(0, y, TERM_WHITE, '|');

		/* Display the line */
		for (x = 1; x < map_wid; ++x)
		{
			/* Find the character to print for the squares this covers. */
			get_best_priority(&ta, &tc,
				(x-1)*ratio, (y-1)*ratio, x*ratio-1, y*ratio-1);

			/* Add the character */
			Term_addch(ta, tc);
		}

		/* Add the right-hand edge. */
		Term_addch(TERM_WHITE, '|');
	}

	/* Restore lighting effects */
	view_special_lite = old_view_special_lite;
	view_granite_lite = old_view_granite_lite;

	/* Edge of map */
	if (my) (*my) = map_hgt;
	if (mx) (*mx) = map_wid;

	/* Player location */
	if (cy) (*cy) = py / ratio + 1;
	if (cx) (*cx) = px / ratio + 1;
}



/*
 * Display a "small-scale" map of the wilderness
 */
void display_wild_map(uint xmin)
{
	s16b x,y;
	int i, j, num_towns;
	uint l;
	char wild_map_symbol;
	byte wild_map_attr;
	cptr tmp;
	C_TNEW(dungeon_has_guardians, MAX_CAVES, bool);

	/* First work out which dungeons have guardians left */
	for(i=0;i<MAX_CAVES;i++)
	{
		dungeon_has_guardians[i]=FALSE;
	}
	for(i=0;i<MAX_Q_IDX;i++)
	{
		if(q_list[i].level || (q_list[i].cur_num != q_list[i].max_num))/* If the quest exists then flag its dungeon*/
		{
			dungeon_has_guardians[q_list[i].dungeon]=TRUE;
		}
	}
	/* Now print out the colour-coded map */
	for(y=0;y<12;y++)
	{
		for(x=0;x<12;x++)
		{
			wild_type *w_ptr = &wild_grid[y][x];

			if((p_ptr->wildx == x) && (p_ptr->wildy == y))
			{
				wild_map_symbol = '@';
				wild_map_attr = TERM_YELLOW;
			}
			else if (w_ptr->dungeon < MAX_CAVES)
			{
				wild_map_symbol = dun_defs[w_ptr->dungeon].sym;
				if(dungeon_has_guardians[w_ptr->dungeon])
				{
					wild_map_attr = TERM_RED;
				}
				else if (is_town_p(y, x))
				{
					wild_map_attr = TERM_WHITE;
				}
				else
				{
					wild_map_attr = TERM_UMBER;
				}
			}
			else if((x == 0) || (y == 0) || (x == 11) || (y == 11))
			{
				wild_map_symbol = '~';
				wild_map_attr = TERM_BLUE;
			}
			else
			{
				wild_map_symbol = '^';
				wild_map_attr = TERM_GREEN;
			}
			Term_putch(xmin+x+1, y+2, wild_map_attr, wild_map_symbol);
		}
	}
	/* Print border */
	put_str("+------------+",1,xmin);
	for(y=0;y<12;y++)
	{
		put_str("|",y+2,xmin);
		put_str("|",y+2,xmin+13);
	}
	put_str("+------------+",14,xmin);

	/* Find the longest legend. */
	for (l = i = 0; i < MAX_TOWNS; i++)
	{
		l = MAX(l, strlen(town_name+town_defs[i].name));
	}

	/* Print legend */
	for (num_towns=i=0;i<MAX_TOWNS;i++)
	{
		dun_type *d_ptr = dun_defs+i;
		town_type *t_ptr = town_defs+i;

		/* Not a town. */
		if (!t_ptr->name) continue;

		if (l+xmin+23 > Term->wid)
			tmp = dun_name+d_ptr->shortname;
		else
			tmp = town_name+t_ptr->name;
		c_put_str(TERM_WHITE, format("%c = %s", d_ptr->sym, tmp),
			num_towns+1,xmin+19);
		num_towns++;
	}

	/* Print dungeon legend */
	for (i = j = 0; i < MAX_CAVES; i++)
	{
		dun_type *d_ptr = dun_defs+i;

		/* Already shown. */
		if (is_town_p(d_ptr->y, d_ptr->x)) continue;

		x = j % 2;
		x = (xmin+(x*Term->wid))/(1+x);
		y = MAX(16, num_towns+1) + j/2;

		tmp = dun_name+d_ptr->name;

		/* Trim an initial "the ". */
		if (prefix(tmp, "the ")) tmp += strlen("the ");

		/* If the name is too long, use the short name instead. */
		if ((strlen(tmp)+4)*2+xmin >= Term->wid)
			tmp = format("%c = %s", d_ptr->sym, dun_name+d_ptr->shortname);
		else
			tmp = format("%c = %s", d_ptr->sym, tmp);
		c_put_str(TERM_WHITE, tmp, y, x);

		j++;
	}

	c_put_str(TERM_UMBER,"* = dungeon entrance",num_towns+2,xmin+19);
	tmp = "(Places that still have guardians are marked in red)";
	if (xmin+strlen(tmp)+18 > Term->wid)
		tmp = "(red = has guardian)";
	c_put_str(TERM_RED,tmp,num_towns+4,xmin+18);
	c_put_str(TERM_YELLOW,"@ = you",num_towns+6,xmin+18);

	TFREE(dungeon_has_guardians);
}

/*
 * Display a "small-scale" map of the dungeon for the player
 *
 * Currently, the "player" is displayed on the map.  XXX XXX XXX
 */
void do_cmd_view_map(void)
{
	cptr str;
	int cy, cx, my, mx;

	/* Enter "icky" mode */
	character_icky = TRUE;

	/* Save the screen */
	Term_save();

	/* Note */
	prt("Please wait...", 0, 0);

	/* Flush */
	Term_fresh();

	/* Clear the screen */
	Term_clear();

	/* Display the map */
	if (dun_level == 0)
	{
		display_wild_map(1);
		Term_get_size(&mx, &my);
		my--;
		cy = -1;
	} 
	else 
	{
		display_map(&cy, &cx, &my, &mx);
	}

	/* Wait for it */
	str = "Hit any key to continue";
	put_str(str, my, (mx-strlen(str))/2);

	/* Hilite the player */
	if (cy >= 0) move_cursor(cy, cx);

	/* Get any key */
	inkey();

	/* Restore the screen */
	Term_load();

	/* Leave "icky" mode */
	character_icky = FALSE;
}





/*
 * Some comments on the cave grid flags.
 *
 *
 * One of the major bottlenecks in ancient versions of Angband was in
 * the calculation of "line of sight" from the player to various grids,
 * such as monsters, using the relatively expensive "los()" function.
 * This was such a nasty bottleneck that a lot of silly things were done
 * to reduce the dependancy on "line of sight", for example, you could
 * not "see" any grids in a lit room until you actually entered the room,
 * at which point every grid in the room became "illuminated", and there
 * were all kinds of bizarre grid flags to support this behavior.  There
 * were other somewhat related bottlenecks involved in the determination
 * of which grids were illuminated by the player's light source.
 *
 * These bottlenecks led to the developement of special new functions to
 * optimize issues involved with "line of sight" and "torch lit grids".
 *
 * The "update_view()" function, below, is used to pre-calculate the
 * entire "field of view" for the player, which, once calculated, can
 * be used to provide an extremely fast method for determining if a grid
 * is in "line of sight" of the player.  It sets the "CAVE_VIEW" flag
 * on every cave grid in the player's field of view, and maintains an
 * array of all such grids in the global "view_n" array.  See also the
 * "player_has_los_bold()" function.
 *
 * The "update_lite()" function, below, is used to pre-calculate the
 * entire "set of torch lit grids", which, once calculated, can be used
 * by the "player_can_see_bold()" function to provide an extremely fast
 * method for determining if a particular grid is illuminated.  It sets
 * the "CAVE_LITE" flag on every cave grid lit by the player's torch, and
 * and maintains an array of all such grids in the global "lite_n" array.
 * See also the "player_has_los_bold()" function.
 *
 * The special arrays maintained by the "update_view()" and "update_lite()"
 * functions can be used not only to quickly scan through all the grids in
 * the dungeon with one of the two related cave grid flags set, but also,
 * in conjunction with another special temporary array, and the "CAVE_TEMP"
 * cave grid flag, are used to redraw any grid which enters or leaves the
 * player's field of view or torch lit range.  This allows the use of some
 * interesting "special lighting effects" (see elsewhere).
 *
 * The "CAVE_TEMP" flag, and the array of "CAVE_TEMP" grids, is also used
 * for various other purposes, all temporary, such as spreading lite or
 * darkness during "lite_room()" / "unlite_room()", and for calculating
 * monster flow.  This flag is always cleared when we are done.
 *
 * The current "update_view()" algorithm uses the "CAVE_XTRA" flag as a
 * temporary internal flag to mark those grids which are not only in view,
 * but which are also "easily" in line of sight of the player.  This flag
 * is always cleared when we are done.
 *
 * For various subtle reasons described in the "player_can_see_bold()"
 * function, any perma-lit wall grids which "remain" in the field of view
 * must be redrawn each time the field of view is recalculated, as if they
 * are just entering the field of view.
 *
 * For efficiency, the "update_lite()" function uses the results of the
 * "update_view()" function, so "update_view()" must be called before
 * "update_lite()".
 *
 * For efficiency, the "update_lite()" function is optimized for the
 * standard cases of a torch lite radius less than or equal to three.
 *
 * For efficiency, the "update_view()" function is only called when the
 * player moves, and when a wall grid becomes a floor grid, or vice versa,
 * while in the player's field of view.
 *
 * For efficiency, the "update_lite()" function is only called when the
 * player moves, when a wall grid becomes a floor grid, or vice versa,
 * while in the player's field of view, or when the player's torch lite
 * radius changes.
 *
 * Note that when the player is resting, or performing most repeated
 * actions, such as digging, there is no need to call either of these
 * two functions.  This means that the game is extremely fast unless
 * the player is running through the dungeon.
 *
 * I wouldn't be surprised if slight modifications to the "update_view()"
 * function would allow us to determine "reverse line-of-sight" as well
 * as "normal line-of-sight", which would allow monsters to use a more
 * "correct" calculation to determine if they can "see" the player.  For
 * now, monsters simply "cheat" somewhat and assume that if the player
 * has "line of sight" to the monster, then the monster can "pretend"
 * that it has "line of sight" to the player.
 *
 *
 * Any grid can be marked as "CAVE_GLOW" which means that the grid itself is
 * in some way permanently lit.  However, for the player to "see" anything
 * in the grid, as determined by "player_can_see_bold()", the player must not
 * be blind, the grid must be marked as "CAVE_VIEW", and, in addition, "wall"
 * grids, even if marked as "perma lit", are only illuminated if they touch
 * a grid which is not a wall and is marked both "CAVE_GLOW" and "CAVE_VIEW",
 * and that grid must be closer to the player than the wall grid.  Note that
 * this last condition can cause problems, see "player_can_see_bold()".
 *
 * To simplify various things, a grid may be marked as "CAVE_MARK", meaning
 * that even if the player cannot "see" the grid, he "knows" the terrain in
 * that grid.  This is used to "remember" walls/doors/stairs/floors when they
 * are "seen" or "detected", and also to "memorize" floors, after "wiz_lite()",
 * or when one of the "memorize floor grids" options induces memorization.
 *
 * Objects are "memorized" in a different way, using a special "marked" flag
 * on the object itself, which is set when an object is observed or detected.
 *
 * A grid may be marked as "CAVE_ROOM" which means that it is part of a "room",
 * and should be illuminated by "lite room" and "darkness" spells.
 *
 * A grid may be marked as "CAVE_ICKY" which means it is part of a "vault",
 * and should be unavailable for "teleportation" destinations.
 *
 *
 * The "view_perma_grids" allows the player to "memorize" every perma-lit grid
 * which is observed, and the "view_torch_grids" allows the player to memorize
 * every torch-lit grid.  The player will always memorize important walls,
 * doors, stairs, and other terrain features, as well as any "detected" grids.
 *
 * Note that the new "update_view()" method allows, among other things, a room
 * to be "partially" seen as the player approaches it, with a growing cone of
 * floor appearing as the player gets closer to the door.  Also, by not turning
 * on the "memorize perma-lit grids" option, the player will only "see" those
 * floor grids which are actually in line of sight.
 *
 * And my favorite "plus" is that you can now use a special option to draw the
 * "floors" in the "viewable region" brightly (actually, to draw the *other*
 * grids dimly), providing a "pretty" effect as the player runs around, and
 * to efficiently display the "torch lite" in a special color.
 */



/*
 * Array of grids lit by player lite.
 */
static s16b lite_n;
static byte lite_y[LITE_MAX];
static byte lite_x[LITE_MAX];

/*
 * Some comments on the "update_lite()" function...
 *
 * The "update_lite()" function uses the results of "update_view()".
 *
 * Note that "blindness" does NOT affect "torch lite".  But it does affect
 * the importance of torch lite.  See "player_can_see_bold()" for more info.
 *
 * We optimize most lites (all non-artifact lites) by using "obvious"
 * facts about the results of "small" lite radius, and we attempt to
 * list the "nearby" grids before the more "distant" ones in the
 * array of torch-lit grids.
 *
 * We will correctly handle "large" radius lites, though currently,
 * it is impossible for the player to have more than radius 3 lite.
 *
 * We assume that "radius zero" lite is in fact no lite at all.
 *
 *     Torch     Lantern     Artifacts
 *     (etc)
 *                              ***
 *                 ***         *****
 *      ***       *****       *******
 *      *@*       **@**       ***@***
 *      ***       *****       *******
 *                 ***         *****
 *                              ***
 */

 
 
/*
 * Forget the "CAVE_LITE" grids, redrawing as needed.
 */
void forget_lite(void)
{
	int i, x, y;

	/* None to forget */
	if (!lite_n) return;

	/* Clear them all */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		/* Forget "LITE" flag */
		cave_info[y][x] &= ~(CAVE_LITE);

		/* Redraw */
		lite_spot(y, x);
	}

	/* None left */
	lite_n = 0;
}


/*
 * Simplify the "update_lite()" function XXX XXX XXX
 *
 * This macro allows us to efficiently add a grid to the "lite" array,
 * note that we are never called for illegal grids, or for grids which
 * have already been placed into the "lite" array, and we are never
 * called when the "lite" array is full.
 */
#define cave_lite_hack(Y,X) \
	do { \
		cave_info[Y][X] |= (CAVE_LITE); \
		lite_y[lite_n] = (Y); \
		lite_x[lite_n] = (X); \
		lite_n++; \
	} while (0)



/*
 * Update the "CAVE_LITE" grids, redrawing as needed.
 */
void update_lite(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int r = p_ptr->cur_lite;

	int i, y, x;


	/*** Special case ***/

	/* Hack -- Player has no lite */
	if (r <= 0)
	{
		/* Forget the old lite */
		forget_lite();

		/* Draw the player */
		lite_spot(py, px);

		/* All done */
		return;
	}


	/*** Save the old "lite" grids for later ***/

	/* Clear them all */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		/* Mark the grid as not "lite" */
		cave_info[y][x] &= ~(CAVE_LITE);

		/* Mark the grid as "seen" */
		cave_info[y][x] |= (CAVE_TEMP);

		/* Add it to the "seen" set */
		temp_y[temp_n] = y;
		temp_x[temp_n] = x;
		temp_n++;
	}

	/* None left */
	lite_n = 0;


	/*** Collect the new "lite" grids ***/

	/* Player grid */
	cave_lite_hack(py, px);

	/* Radius 1 -- torch radius */
	if (r >= 1)
	{
		/* Adjacent grid */
		cave_lite_hack(py+1, px);
		cave_lite_hack(py-1, px);
		cave_lite_hack(py, px+1);
		cave_lite_hack(py, px-1);

		/* Diagonal grids */
		cave_lite_hack(py+1, px+1);
		cave_lite_hack(py+1, px-1);
		cave_lite_hack(py-1, px+1);
		cave_lite_hack(py-1, px-1);
	}

	/* Radius 2 -- lantern radius */
	if (r >= 2)
	{
		/* South of the player */
		if (cave_floor_bold(py+1, px))
		{
			cave_lite_hack(py+2, px);
			cave_lite_hack(py+2, px+1);
			cave_lite_hack(py+2, px-1);
		}

		/* North of the player */
		if (cave_floor_bold(py-1, px))
		{
			cave_lite_hack(py-2, px);
			cave_lite_hack(py-2, px+1);
			cave_lite_hack(py-2, px-1);
		}

		/* East of the player */
		if (cave_floor_bold(py, px+1))
		{
			cave_lite_hack(py, px+2);
			cave_lite_hack(py+1, px+2);
			cave_lite_hack(py-1, px+2);
		}

		/* West of the player */
		if (cave_floor_bold(py, px-1))
		{
			cave_lite_hack(py, px-2);
			cave_lite_hack(py+1, px-2);
			cave_lite_hack(py-1, px-2);
		}
	}

	/* Radius 3+ -- artifact radius */
	if (r >= 3)
	{
		int y1, y2, x1, x2;

		/* Paranoia */
		if (r > 5) r = 5;

		/* South-East of the player */
		if (cave_floor_bold(py+1, px+1))
		{
			cave_lite_hack(py+2, px+2);
		}

		/* South-West of the player */
		if (cave_floor_bold(py+1, px-1))
		{
			cave_lite_hack(py+2, px-2);
		}

		/* North-East of the player */
		if (cave_floor_bold(py-1, px+1))
		{
			cave_lite_hack(py-2, px+2);
		}

		/* North-West of the player */
		if (cave_floor_bold(py-1, px-1))
		{
			cave_lite_hack(py-2, px-2);
		}

		/* Maximal north */
		y1 = py - r;
		if (y1 < 0) y1 = 0;

		/* Maximal south */
		y2 = py + r;
		if (y2 > DUNGEON_HGT-1) y2 = DUNGEON_HGT-1;

		/* Maximal west */
		x1 = px - r;
		if (x1 < 0) x1 = 0;

		/* Maximal east */
		x2 = px + r;
		if (x2 > DUNGEON_WID-1) x2 = DUNGEON_WID-1;

		/* Scan the maximal box */
		for (y = y1; y <= y2; y++)
		{
			for (x = x1; x <= x2; x++)
			{
				int d;

				int dy = (py > y) ? (py - y) : (y - py);
				int dx = (px > x) ? (px - x) : (x - px);

				/* Skip the "central" grids (above) */
				if ((dy <= 2) && (dx <= 2)) continue;

				/* Hack -- approximate the distance */
				d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

				/* Skip distant grids */
				if (d > r) continue;

				/* Viewable grids get "torch lit" */
				if (cave_info[y][x] & (CAVE_VIEW))
				{
					/* This grid is "torch lit" */
					cave_lite_hack(y, x);
				}
			}
		}
	}


	/*** Complete the algorithm ***/

	/* Process "new" grids */
	for (i = 0; i < lite_n; i++)
	{
		y = lite_y[i];
		x = lite_x[i];

		/* Update/Redraw "new" grids */
		if (!(cave_info[y][x] & (CAVE_TEMP)))
		{
			/* Note */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);
		}
	}

	/* Process "old" grids */
	for (i = 0; i < temp_n; i++)
	{
		y = temp_y[i];
		x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Redraw "old" grids */
		if (!(cave_info[y][x] & (CAVE_LITE)))
		{
			/* Redraw */
			lite_spot(y, x);
		}
	}

	/* None left */
	temp_n = 0;
}






/*
 * Array of grids viewable to the player
 */
static s16b view_n;
static byte view_y[VIEW_MAX];
static byte view_x[VIEW_MAX];


/*
 * Some comments on the "update_view()" algorithm...
 *
 * Algorithm summary:
 *
 *  1: Process the player
 *  1a: The player is always (easily) viewable
 *  2: Process the diagonals
 *  2a: The diagonals are (easily) viewable up to the first wall
 *  2b: But never go more than 2/3 of the "full" distance
 *  3: Process the main axes
 *  3a: The main axes are (easily) viewable up to the first wall
 *  3b: But never go more than the "full" distance
 *  4: Process sequential "strips" in each of the eight octants
 *  4a: Each strip runs along the previous strip
 *  4b: The main axes are "previous" to the first strip
 *  4c: Process both "sides" of each "direction" of each strip
 *  4c1: Each side aborts as soon as possible
 *  4c2: Each side tells the next strip how far it has to check
 *
 * Note that the octant processing involves some pretty interesting
 * observations involving when a grid might possibly be viewable from
 * a given grid, and on the order in which the strips are processed.
 *
 * Note the use of the mathematical facts shown below, which derive
 * from the fact that (1 < sqrt(2) < 1.5), and that the length of the
 * hypotenuse of a right triangle is primarily determined by the length
 * of the longest side, when one side is small, and is strictly less
 * than one-and-a-half times as long as the longest side when both of
 * the sides are large.
 *
 *   if (manhatten(dy,dx) < R) then (hypot(dy,dx) < R)
 *   if (manhatten(dy,dx) > R*3/2) then (hypot(dy,dx) > R)
 *
 *   hypot(dy,dx) is approximated by (dx+dy+MAX(dx,dy)) / 2
 *
 * These observations are important because the calculation of the actual
 * value of "hypot(dx,dy)" is extremely expensive, involving square roots,
 * while for small values (up to about 20 or so), the approximations above
 * are correct to within an error of at most one grid or so.
 *
 * Observe the use of "full" and "over" in the code below, and the use of
 * the specialized calculation involving "limit", all of which derive from
 * the observations given above.  Basically, we note that the "circle" of
 * view is completely contained in an "octagon" whose bounds are easy to
 * determine, and that only a few steps are needed to derive the actual
 * bounds of the circle given the bounds of the octagon.
 *
 * Note that by skipping all the grids in the corners of the octagon, we
 * place an upper limit on the number of grids in the field of view, given
 * that "full" is never more than 20.  Of the 1681 grids in the "square" of
 * view, only about 1475 of these are in the "octagon" of view, and even
 * fewer are in the "circle" of view, so 1500 or 1536 is more than enough
 * entries to completely contain the actual field of view.
 *
 * Note also the care taken to prevent "running off the map".  The use of
 * explicit checks on the "validity" of the "diagonal", and the fact that
 * the loops are never allowed to "leave" the map, lets "update_view_aux()"
 * use the optimized "cave_floor_bold()" macro, and to avoid the overhead
 * of multiple checks on the validity of grids.  Note that for months we
 * were using a meaningless test for "East strip, South side", so it looks
 * like perhaps the first half of each of those tests is obsolete given the
 * test for the "optimizations" below.
 *
 * The bizarre "se","sw","ne","nw","es","en","ws","wn" variables are very
 * important.  They indicate, for each "strip" (named by major axis and
 * then minor direction), the maximal distance which may be traveled along
 * that strip.  They start out as the maximum viewable distance, modified
 * if needed to avoid leaving the dungeon.  Then, for example, consider the
 * "se" variable.  While moving down the south-bound strip just to the east
 * of the main south axis, as soon as we get to a grid which does not allow
 * "viewing" to pass, if all previous related strips (including the primary
 * "south" axis) have terminated at or before the same point, then we can
 * stop, and reset the "max distance" to ourself.  Draw a picture if needed.
 *
 * Note the use of efficiency macros.  The "cave_view_hack()" macro is a
 * chunk of code which adds the given location to the "view" array if it
 * is not already there, using both the actual location and a pointer to
 * the cave grid.  See above.
 *
 * By the way, the purpose of this code is to reduce the dependancy on the
 * "los()" function which is slow, and, in some cases, not very accurate.
 *
 * It is very possible that I am the only person who fully understands this
 * function, and for that I am truly sorry, but efficiency was very important
 * and the "simple" version of this function was just not fast enough.  I am
 * more than willing to replace this function with a simpler one, if it is
 * equally efficient, and especially willing if the new function happens to
 * derive "reverse-line-of-sight" at the same time, since currently monsters
 * just use an optimized hack of "you see me, so I see you", and then use the
 * actual "projectable()" function to check spell attacks.
 *
 * The algorithm is very fast, since it spreads "obvious" grids very quickly,
 * and only has to call "los()" on the borderline cases.  The major axes/diags
 * even terminate early when they hit walls.  I need to find a quick way
 * to "terminate" the other scans.
 *
 * Note that in the worst case (a big empty area with say 5% scattered walls),
 * each of the 1500 or so nearby grids is checked once, most of them getting
 * an "instant" rating, and only a small portion requiring a call to "los()".
 *
 * The only time that the algorithm appears to be "noticeably" too slow is
 * when running, and this is usually only important in town, since the town
 * provides about the worst scenario possible, with large open regions and
 * a few scattered obstructions.  There is a special "efficiency" option to
 * allow the player to reduce his field of view in town, if needed.
 *
 * In the "best" case (say, a normal stretch of corridor), the algorithm
 * makes one check for each viewable grid, and makes no calls to "los()".
 * So running in corridors is very fast, and if a lot of monsters are
 * nearby, it is much faster than the old methods.
 *
 * Note that resting, most normal commands, and several forms of running,
 * plus all commands executed near large groups of monsters, are strictly
 * more efficient with "update_view()" that with the old "compute los() on
 * demand" method, primarily because once the "field of view" has been
 * calculated, it does not have to be recalculated until the player moves
 * (or a wall or door is created or destroyed).
 *
 * Note that we no longer have to do as many "los()" checks, since once the
 * "view" region has been built, very few things cause it to be "changed"
 * (player movement, and the opening/closing of doors, changes in wall status).
 * Note that door/wall changes are only relevant when the door/wall itself is
 * in the "view" region.
 *
 * The algorithm seems to only call "los()" from zero to ten times, usually
 * only when coming down a corridor into a room, or standing in a room, just
 * misaligned with a corridor.  So if, say, there are five "nearby" monsters,
 * we will be reducing the calls to "los()".
 *
 * I am thinking in terms of an algorithm that "walks" from the central point
 * out to the maximal "distance", at each point, determining the "view" code
 * (above).  For each grid not on a major axis or diagonal, the "view" code
 * depends on the "cave_floor_bold()" and "view" of exactly two other grids
 * (the one along the nearest diagonal, and the one next to that one, see
 * "update_view_aux()"...).
 *
 * In the worst "normal" case (in the middle of the town), the reachable space
 * actually reaches to more than half of the largest possible "circle" of view,
 * or about 800 grids, and in the worse case (in the middle of a dungeon level
 * where all the walls have been removed), the reachable space actually reaches
 * the theoretical maximum size of just under 1500 grids.
 *
 * The picture below shows the player and one quandrant (two octants) of the
 * viewable region, and the order in which the grids are processed.  The pure
 * diagonals and main axes are processed first, up to the appropriate distance,
 * and then the sequential strips are processed (one in each octant), also up
 * to the appropriate distance.
 *
 *    @11111111
 *    112222222
 *    12133333:
 *    12314444:
 *    1234155::
 *    123451::.
 *    12345::..
 *    1234::...
 *    12:::....
 */



/*
 * Forget the "CAVE_VIEW" grids, redrawing as needed
 */
void forget_view(void)
{
	int i;

	/* None to forget */
	if (!view_n) return;

	/* Clear them all */
	for (i = 0; i < view_n; i++)
	{
		int y = view_y[i];
		int x = view_x[i];

		/* Forget that the grid is viewable */
		cave_info[y][x] &= ~(CAVE_VIEW);

		/* Update the screen */
		lite_spot(y, x);
	}

	/* None left */
	view_n = 0;
}





/*
 * Simplify the "update_view()" function XXX XXX XXX
 *
 * This macro allows us to efficiently add a grid to the "view" array,
 * note that we are never called for illegal grids, or for grids which
 * have already been placed into the "view" array, and we are never
 * called when the "view" array is full.
 */
#define cave_view_hack(Y,X) \
	do { \
		cave_info[Y][X] |= (CAVE_VIEW); \
		view_y[view_n] = (Y); \
		view_x[view_n] = (X); \
		view_n++; \
	} while (0)



/*
 * Helper function for "update_view()" below
 *
 * This function processes the "viewability" of the grid (y,x).
 *
 * Grid (y1,x1) is on the "diagonal" between (py,px) and (y,x)
 * Grid (y2,x2) is "adjacent", also between (py,px) and (y,x).
 *
 * This function returns "TRUE" if vision is "blocked" by grid (y,x).
 */
static bool update_view_aux(int y, int x, int y1, int x1, int y2, int x2)
{
	bool f1, f2, v1, v2, z1, z2, wall;


	/* Check for walls */
	f1 = (cave_floor_bold(y1, x1));
	f2 = (cave_floor_bold(y2, x2));

	/* Totally blocked by physical walls */
	if (!f1 && !f2) return (TRUE);


	/* Check for visibility */
	v1 = (f1 && (cave_info[y1][x1] & (CAVE_VIEW)));
	v2 = (f2 && (cave_info[y2][x2] & (CAVE_VIEW)));

	/* Totally blocked by "unviewable neighbors" */
	if (!v1 && !v2) return (TRUE);


	/* Check for walls */
	wall = (!cave_floor_bold(y, x));


	/* Check the "ease" of visibility */
	z1 = (v1 && (cave_info[y1][x1] & (CAVE_XTRA)));
	z2 = (v2 && (cave_info[y2][x2] & (CAVE_XTRA)));

	/* Hack -- "easy" plus "easy" yields "easy" */
	if (z1 && z2)
	{
		cave_info[y][x] |= (CAVE_XTRA);

		cave_view_hack(y, x);

		return (wall);
	}

	/* Hack -- primary "easy" yields "viewed" */
	if (z1)
	{
		cave_view_hack(y, x);

		return (wall);
	}


	/* Hack -- "view" plus "view" yields "view" */
	if (v1 && v2)
	{
		/* cave_info[y][x] |= (CAVE_XTRA); */

		cave_view_hack(y, x);

		return (wall);
	}


	/* Mega-Hack -- the "los()" function works poorly on walls */
	if (wall)
	{
		cave_view_hack(y, x);

		return (wall);
	}


	/* Actual line of sight test */
	if (TRUE)
	{
		int py = p_ptr->py;
		int px = p_ptr->px;

		/* Hack -- check line of sight */
		if (los(py, px, y, x))
		{
			cave_view_hack(y, x);

			return (wall);
		}
	}


	/* Assume no line of sight. */
	return (TRUE);
}


/*
 * Hack -- keep the code below simple
 */
#define DY1 0
#define DY2 (DUNGEON_HGT-1)
#define DX1 0
#define DX2 (DUNGEON_WID-1)


/*
 * Update the "CAVE_VIEW" grids, redrawing as needed
 */
void update_view(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int n, m, d, k, y, x, z;

	int se, sw, ne, nw, es, en, ws, wn;

	int full, over;


	/*** Initialize ***/

	/* Optimize */
	if (view_reduce_view && !p_ptr->depth)
	{
		/* Full radius (10) */
		full = MAX_SIGHT / 2;

		/* Octagon factor (15) */
		over = MAX_SIGHT * 3 / 4;
	}

	/* Normal */
	else
	{
		/* Full radius (20) */
		full = MAX_SIGHT;

		/* Octagon factor (30) */
		over = MAX_SIGHT * 3 / 2;
	}


	/*** Step 0 -- Begin ***/

	/* Save the old "view" grids for later */
	for (n = 0; n < view_n; n++)
	{
		y = view_y[n];
		x = view_x[n];

		/* Mark the grid as not in "view" */
		cave_info[y][x] &= ~(CAVE_VIEW);

		/* Mark the grid as "seen" */
		cave_info[y][x] |= (CAVE_TEMP);

		/* Add it to the "seen" set */
		temp_y[temp_n] = y;
		temp_x[temp_n] = x;
		temp_n++;
	}

	/* Start over with the "view" array */
	view_n = 0;


	/*** Step 1 -- adjacent grids ***/

	/* Now start on the player */
	y = py;
	x = px;

	/* Assume the player grid is easily viewable */
	cave_info[y][x] |= (CAVE_XTRA);

	/* Assume the player grid is viewable */
	cave_view_hack(y, x);


	/*** Step 2 -- Major Diagonals ***/

	/* Hack -- Limit */
	z = full * 2 / 3;

	/* Scan south-east */
	for (d = 1; d <= z; d++)
	{
		cave_info[y+d][x+d] |= (CAVE_XTRA);
		cave_view_hack(y+d, x+d);
		if (!cave_floor_bold(y+d, x+d)) break;
	}

	/* Scan south-west */
	for (d = 1; d <= z; d++)
	{
		cave_info[y+d][x-d] |= (CAVE_XTRA);
		cave_view_hack(y+d, x-d);
		if (!cave_floor_bold(y+d, x-d)) break;
	}

	/* Scan north-east */
	for (d = 1; d <= z; d++)
	{
		cave_info[y-d][x+d] |= (CAVE_XTRA);
		cave_view_hack(y-d, x+d);
		if (!cave_floor_bold(y-d, x+d)) break;
	}

	/* Scan north-west */
	for (d = 1; d <= z; d++)
	{
		cave_info[y-d][x-d] |= (CAVE_XTRA);
		cave_view_hack(y-d, x-d);
		if (!cave_floor_bold(y-d, x-d)) break;
	}


	/*** Step 3 -- major axes ***/

	/* Scan south */
	for (d = 1; d <= full; d++)
	{
		cave_info[y+d][x] |= (CAVE_XTRA);
		cave_view_hack(y+d, x);
		if (!cave_floor_bold(y+d, x)) break;
	}

	/* Initialize the "south strips" */
	se = sw = d;

	/* Scan north */
	for (d = 1; d <= full; d++)
	{
		cave_info[y-d][x] |= (CAVE_XTRA);
		cave_view_hack(y-d, x);
		if (!cave_floor_bold(y-d, x)) break;
	}

	/* Initialize the "north strips" */
	ne = nw = d;

	/* Scan east */
	for (d = 1; d <= full; d++)
	{
		cave_info[y][x+d] |= (CAVE_XTRA);
		cave_view_hack(y, x+d);
		if (!cave_floor_bold(y, x+d)) break;
	}

	/* Initialize the "east strips" */
	es = en = d;

	/* Scan west */
	for (d = 1; d <= full; d++)
	{
		cave_info[y][x-d] |= (CAVE_XTRA);
		cave_view_hack(y, x-d);
		if (!cave_floor_bold(y, x-d)) break;
	}

	/* Initialize the "west strips" */
	ws = wn = d;


	/*** Step 4 -- Divide each "octant" into "strips" ***/

	/* Now check each "strip" */
	for (n = 1; n <= over / 2; n++)
	{
		int ypn, ymn, ypnpd, ymnmd;
		int xpn, xmn, xpnpd, xmnmd;


		/* Acquire the "bounds" of the maximal circle */
		z = over - n - n;
		if (z > full - n) z = full - n;
		while ((z + n + (n>>1)) > full) z--;


		/* Access the four diagonal grids */
		ypn = y + n;
		ymn = y - n;
		xpn = x + n;
		xmn = x - n;


		/* South strip */
		if (ypn < DY2)
		{
			/* Maximum distance */
			m = MIN(z, DY2 - ypn);

			/* East side */
			if (n < se)
			{
				/* Scan */
				for (k = n, ypnpd = ypn+1, d = 1; d <= m; d++, ypnpd++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypnpd, xpn, ypnpd-1, xpn-1, ypnpd-1, xpn))
					{
						if (n + d >= se) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				se = k + 1;
			}

			/* West side */
			if (n < sw)
			{
				/* Scan */
				for (k = n, ypnpd = ypn+1, d = 1; d <= m; d++, ypnpd++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypnpd, xmn, ypnpd-1, xmn+1, ypnpd-1, xmn))
					{
						if (n + d >= sw) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				sw = k + 1;
			}
		}


		/* North strip */
		if (ymn > DY1)
		{
			/* Maximum distance */
			m = MIN(z, ymn - DY1);

			/* East side */
			if (n < ne)
			{
				/* Scan */
				for (k = n, ymnmd = ymn-1, d = 1; d <= m; d++, ymnmd--)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymnmd, xpn, ymnmd+1, xpn-1, ymnmd+1, xpn))
					{
						if (n + d >= ne) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				ne = k + 1;
			}

			/* West side */
			if (n < nw)
			{
				/* Scan */
				for (k = n, ymnmd = ymn-1, d = 1; d <= m; d++, ymnmd--)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymnmd, xmn, ymnmd+1, xmn+1, ymnmd+1, xmn))
					{
						if (n + d >= nw) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				nw = k + 1;
			}
		}


		/* East strip */
		if (xpn < DX2)
		{
			/* Maximum distance */
			m = MIN(z, DX2 - xpn);

			/* South side */
			if (n < es)
			{
				/* Scan */
				for (k = n, xpnpd = xpn+1, d = 1; d <= m; d++, xpnpd++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn, xpnpd, ypn-1, xpnpd-1, ypn, xpnpd-1))
					{
						if (n + d >= es) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				es = k + 1;
			}

			/* North side */
			if (n < en)
			{
				/* Scan */
				for (k = n, xpnpd = xpn+1, d = 1; d <= m; d++, xpnpd++)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn, xpnpd, ymn+1, xpnpd-1, ymn, xpnpd-1))
					{
						if (n + d >= en) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				en = k + 1;
			}
		}


		/* West strip */
		if (xmn > DX1)
		{
			/* Maximum distance */
			m = MIN(z, xmn - DX1);

			/* South side */
			if (n < ws)
			{
				/* Scan */
				for (k = n, xmnmd = xmn-1, d = 1; d <= m; d++, xmnmd--)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ypn, xmnmd, ypn-1, xmnmd+1, ypn, xmnmd+1))
					{
						if (n + d >= ws) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				ws = k + 1;
			}

			/* North side */
			if (n < wn)
			{
				/* Scan */
				for (k = n, xmnmd = xmn-1, d = 1; d <= m; d++, xmnmd--)
				{
					/* Check grid "d" in strip "n", notice "blockage" */
					if (update_view_aux(ymn, xmnmd, ymn+1, xmnmd+1, ymn, xmnmd+1))
					{
						if (n + d >= wn) break;
					}

					/* Track most distant "non-blockage" */
					else
					{
						k = n + d;
					}
				}

				/* Limit the next strip */
				wn = k + 1;
			}
		}
	}


	/*** Step 5 -- Complete the algorithm ***/

	/* Process "new" grids */
	for (n = 0; n < view_n; n++)
	{
		y = view_y[n];
		x = view_x[n];

		/* Clear the "CAVE_XTRA" flag */
		cave_info[y][x] &= ~(CAVE_XTRA);

		/* Update/Redraw "new" grids */
		if (!(cave_info[y][x] & (CAVE_TEMP)))
		{
			/* Note */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);
		}

		/* Update/Redraw all perma-lit wall grids */
		else if ((cave_info[y][x] & (CAVE_GLOW)) &&
		         !cave_floor_bold(y, x))
		{
			/* Note */
			note_spot(y, x);

			/* Redraw */
			lite_spot(y, x);
		}
	}

	/* Process "old" grids */
	for (n = 0; n < temp_n; n++)
	{
		y = temp_y[n];
		x = temp_x[n];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Redraw "old" grids */
		if (!(cave_info[y][x] & (CAVE_VIEW)))
		{
			/* Redraw */
			lite_spot(y, x);
		}
	}

	/* None left */
	temp_n = 0;
}





/*
 * Hack -- provide some "speed" for the "flow" code
 * This entry is the "current index" for the "when" field
 * Note that a "when" value of "zero" means "not used".
 *
 * Note that the "cost" indexes from 1 to 127 are for
 * "old" data, and from 128 to 255 are for "new" data.
 *
 * This means that as long as the player does not "teleport",
 * then any monster up to 128 + MONSTER_FLOW_DEPTH will be
 * able to track down the player, and in general, will be
 * able to track down either the player or a position recently
 * occupied by the player.
 */
static int flow_n = 0;




#ifdef MONSTER_FLOW

/*
 * Hack -- Allow us to treat the "seen" array as a queue
 */
static int flow_head = 0;
static int flow_tail = 0;


/*
 * Take note of a reachable grid.  Assume grid is legal.
 */
static void update_flow_aux(int y, int x, int n)
{
	int old_head = flow_head;


	/* Ignore "pre-stamped" entries */
	if (cave_when[y][x] == flow_n) return;

	/* Ignore "walls" and "rubble" */
	if (cave_feat[y][x] >= FEAT_RUBBLE) return;

	/* Save the time-stamp */
	cave_when[y][x] = flow_n;

	/* Save the flow cost */
	cave_cost[y][x] = n;

	/* Hack -- limit flow depth */
	if (n == MONSTER_FLOW_DEPTH) return;

	/* Enqueue that entry */
	temp_y[flow_head] = y;
	temp_x[flow_head] = x;

	/* Advance the queue */
	if (++flow_head == TEMP_MAX) flow_head = 0;

	/* Hack -- notice overflow by forgetting new entry */
	if (flow_head == flow_tail) flow_head = old_head;
}

#endif


/*
 * Hack -- fill in the "cost" field of every grid that the player
 * can "reach" with the number of steps needed to reach that grid.
 * This also yields the "distance" of the player from every grid.
 *
 * In addition, mark the "when" of the grids that can reach
 * the player with the incremented value of "flow_n".
 *
 * Hack -- use the "seen" array as a "circular queue".
 *
 * We do not need a priority queue because the cost from grid
 * to grid is always "one" and we process them in order.
 */
void update_flow(void)
{

#ifdef MONSTER_FLOW

	int py = p_ptr->py;
	int px = p_ptr->px;

	int x, y, d;

	/* Hack -- disabled */
	if (!flow_by_sound) return;

	/* Paranoia -- make sure the array is empty */
	if (temp_n) return;

	/* Cycle the old entries (once per 128 updates) */
	if (flow_n == 255)
	{
		/* Rotate the time-stamps */
		for (y = 0; y < DUNGEON_HGT; y++)
		{
			for (x = 0; x < DUNGEON_WID; x++)
			{
				int w;
				w = cave_when[y][x];
				cave_when[y][x] = (w > 128) ? (w - 128) : 0;
			}
		}

		/* Restart */
		flow_n = 127;
	}

	/* Start a new flow (never use "zero") */
	flow_n++;


	/* Reset the "queue" */
	flow_head = flow_tail = 0;

	/* Queue the player grid */
	update_flow_aux(py, px, 0);

	/* Now process the queue */
	while (flow_head != flow_tail)
	{
		/* Extract the next entry */
		y = temp_y[flow_tail];
		x = temp_x[flow_tail];

		/* Forget that entry */
		if (++flow_tail == TEMP_MAX) flow_tail = 0;

		/* Add the "children" */
		for (d = 0; d < 8; d++)
		{
			/* Add that child if "legal" */
			update_flow_aux(y+ddy_ddd[d], x+ddx_ddd[d], cave_cost[y][x]+1);
		}
	}

	/* Forget the flow info */
	flow_head = flow_tail = 0;

#endif

}







/*
 * Map the current panel (plus some) ala "magic mapping"
 *
 * We must never attempt to map the outer dungeon walls, or we
 * might induce illegal cave grid references.
 */
void map_area(void)
{
	int i, x, y, y1, y2, x1, x2;


	/* Pick an area to map */
	y1 = p_ptr->wy - randint(10);
	y2 = p_ptr->wy+SCREEN_HGT + randint(10);
	x1 = p_ptr->wx - randint(20);
	x2 = p_ptr->wx+SCREEN_WID + randint(20);

	/* Efficiency -- shrink to fit legal bounds */
	if (y1 < 1) y1 = 1;
	if (y2 > DUNGEON_HGT-1) y2 = DUNGEON_HGT-1;
	if (x1 < 1) x1 = 1;
	if (x2 > DUNGEON_WID-1) x2 = DUNGEON_WID-1;

	/* Scan that area */
	for (y = y1; y < y2; y++)
	{
		for (x = x1; x < x2; x++)
		{
			/* All non-walls are "checked" */
			if (cave_feat[y][x] < FEAT_SECRET)
			{
				/* Memorize normal features */
				if (cave_feat[y][x] > FEAT_INVIS)
				{
					/* Memorize the object */
					cave_info[y][x] |= (CAVE_MARK);
				}

				/* Memorize known walls */
				for (i = 0; i < 8; i++)
				{
					int yy = y + ddy_ddd[i];
					int xx = x + ddx_ddd[i];

					/* Memorize walls (etc) */
					if (cave_feat[yy][xx] >= FEAT_SECRET)
					{
						/* Memorize the walls */
						cave_info[yy][xx] |= (CAVE_MARK);
					}
				}
			}
		}
	}

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}



/*
 * Light up the dungeon using "claravoyance"
 *
 * This function "illuminates" every grid in the dungeon, memorizes all
 * "objects", memorizes all grids as with magic mapping, and, under the
 * standard option settings (view_perma_grids but not view_torch_grids)
 * memorizes all floor grids too.
 *
 * Note that if "view_perma_grids" is not set, we do not memorize floor
 * grids, since this would defeat the purpose of "view_perma_grids", not
 * that anyone seems to play without this option.
 *
 * Note that if "view_torch_grids" is set, we do not memorize floor grids,
 * since this would prevent the use of "view_torch_grids" as a method to
 * keep track of what grids have been observed directly.
 */
void wiz_lite(void)
{
	int i, y, x;


	/* Memorize objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Memorize */
		o_ptr->marked = TRUE;
	}

	/* Scan all normal grids */
	for (y = 1; y < DUNGEON_HGT-1; y++)
	{
		/* Scan all normal grids */
		for (x = 1; x < DUNGEON_WID-1; x++)
		{
			/* Process all non-walls */
			if (cave_feat[y][x] < FEAT_SECRET)
			{
				/* Scan all neighbors */
				for (i = 0; i < 9; i++)
				{
					int yy = y + ddy_ddd[i];
					int xx = x + ddx_ddd[i];

					/* Perma-lite the grid */
					cave_info[yy][xx] |= (CAVE_GLOW);

					/* Memorize normal features */
					if (cave_feat[yy][xx] > FEAT_INVIS)
					{
						/* Memorize the grid */
						cave_info[yy][xx] |= (CAVE_MARK);
					}
					cave_rfeat[yy][xx] = cave_feat[yy][xx];
					
					/* Normally, memorize floors (see above) */
					if (view_perma_grids && !view_torch_grids)
					{
						/* Memorize the grid */
						cave_info[yy][xx] |= (CAVE_MARK);
					}

					/* HACK - Recalculate priority map for overhead display. */
					priority_map[yy][xx] = 0;
				}
			}
		}
	}

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);
}


/*
 * Forget the dungeon map (ala "Thinking of Maud...").
 */
void wiz_dark(void)
{
	int i, y, x;


	/* Forget every grid */
	for (y = 0; y < DUNGEON_HGT; y++)
	{
		for (x = 0; x < DUNGEON_WID; x++)
		{
			/* Process the grid */
			cave_info[y][x] &= ~(CAVE_MARK);
			cave_rfeat[y][x] = FEAT_NONE;
		}
	}

	/* Forget all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Forget the object */
		o_ptr->marked = FALSE;
	}

	/* Mega-Hack -- Forget the view and lite */
	p_ptr->update |= (PU_UN_VIEW | PU_UN_LITE);

	/* Update the view and lite */
	p_ptr->update |= (PU_VIEW | PU_LITE);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);
}





/*
 * Change the "feat" flag for a grid, and notice/redraw the grid
 */
void cave_set_feat(int y, int x, int feat)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	/* Change the feature */
	cave_feat[y][x] = feat;

	/* Notice */
	note_spot(y, x);

	/* Redraw */
	lite_spot(y, x);

	/* Window stuff, if needed. */
	if (y == py && x == px) p_ptr->window |= PW_FLOOR;
}



/*
 * Calculate "incremental motion". Used by project() and shoot().
 * Assumes that (*y,*x) lies on the path from (y1,x1) to (y2,x2).
 */
void mmove2(int *y, int *x, int y1, int x1, int y2, int x2)
{
	int dy, dx, dist, shift;

	/* Extract the distance travelled */
	dy = (*y < y1) ? y1 - *y : *y - y1;
	dx = (*x < x1) ? x1 - *x : *x - x1;

	/* Number of steps */
	dist = (dy > dx) ? dy : dx;

	/* We are calculating the next location */
	dist++;


	/* Calculate the total distance along each axis */
	dy = (y2 < y1) ? (y1 - y2) : (y2 - y1);
	dx = (x2 < x1) ? (x1 - x2) : (x2 - x1);

	/* Paranoia -- Hack -- no motion */
	if (!dy && !dx) return;


	/* Move mostly vertically */
	if (dy > dx)
	{

		/* Extract a shift factor */
		shift = (dist * dx + (dy-1) / 2) / dy;

		/* Sometimes move along the minor axis */
		(*x) = (x2 < x1) ? (x1 - shift) : (x1 + shift);

		/* Always move along major axis */
		(*y) = (y2 < y1) ? (y1 - dist) : (y1 + dist);
	}

	/* Move mostly horizontally */
	else
	{

		/* Extract a shift factor */
		shift = (dist * dy + (dx-1) / 2) / dx;

		/* Sometimes move along the minor axis */
		(*y) = (y2 < y1) ? (y1 - shift) : (y1 + shift);

		/* Always move along major axis */
		(*x) = (x2 < x1) ? (x1 - dist) : (x1 + dist);
	}
}



/*
 * Move from (ox,oy) to (nx,ny), stopping at the last square which was "okay".
 *
 * Return TRUE if the entire path (excluding the starting position) is "okay".
 */
bool move_in_direction(int *xx, int *yy, int x1, int y1, int x2, int y2,
	int (*okay)(int, int, int))
{
	int x, y, d, r;
	
	assert(okay); /* Caller */

	/* Set (*xx, *yy) to the square before the first on which okay() fail. */
	for (d = 0, x = x1, y = y1; ; d++)
	{
		r = (d) ? (*okay)(y, x, d) : MVD_CONTINUE;

		if (r == MVD_STOP_BEFORE_HERE) break;

		/* Save the new location if desired. */
		if (xx) *xx = x;
		if (yy) *yy = y;

		/* Okay for the entire distance. */
		if (x == x2 && y == y2) return TRUE;

		if (r == MVD_STOP_HERE) break;

		mmove2(&y, &x, y1, x1, y2, x2);
	}

	/* Not okay. */
	return FALSE;
}

/*
 * Return TRUE if (x,y) is a "floor" grid, part of the pattern or an explosive
 * rune.
 */
static int PURE mvd_projectable(int y, int x, int d)
{
	/* Too far. */
	if (d > MAX_RANGE || !in_bounds(y, x)) return MVD_STOP_BEFORE_HERE;

	/* Floor */
	else if (cave_floor_bold(y, x)) return MVD_CONTINUE;

	/* Pattern or explosive rune. */
	else if ((cave_feat[y][x] <= FEAT_PATTERN_XTRA2) &&
		(cave_feat[y][x] >= FEAT_MINOR_GLYPH)) return MVD_CONTINUE;

	/* Bad terrain feature. */
	else return MVD_STOP_BEFORE_HERE;
}

/*
 * Determine if a bolt spell cast from (y1,x1) to (y2,x2) will arrive
 * at the final destination, assuming no monster gets in the way.
 *
 * This is slightly (but significantly) different from "los(y1,x1,y2,x2)".
 */
bool projectable(int y1, int x1, int y2, int x2)
{
	/* See "project()" */
	return move_in_direction(NULL, NULL, x1, y1, x2, y2, mvd_projectable);
}

/*
 * A general "find a location" function.
 * Obtains a location on the map which matches "accept" with v as its parameter.
 *
 * This isn`t very efficient as it calls accept at least once on every grid,
 * but it doesn't need to use large arrays to do this.
 */
bool rand_location(int *yp, int *xp, bool (*accept)(int, int, vptr), vptr v)
{
	int x, y, t;

	for (t = y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			if ((*accept)(y, x, v)) t++;
		}
	}

	/* No matching squares exist. */
	if (!t) return FALSE;

	t = rand_int(t);

	for (y = 0; y < cur_hgt; y++)
	{
		for (x = 0; x < cur_wid; x++)
		{
			if ((*accept)(y, x, v) && !t--)
			{
				*yp = y;
				*xp = x;
				return TRUE;
			}
		}
	}

	/* Paranoia. */
	assert(!"the acceptable grid list isn't constant!");
	return FALSE;
}

/*
 * Return TRUE if the new location is suitable for scatter() below.
 */
static bool PURE scatter_good(int ny, int nx, int y, int x, int d,
	bool (*accept)(int, int))
{
	/* Ignore annoying locations */
	if (!in_bounds_fully(ny, nx)) return FALSE;

	/* Ignore "excessively distant" locations */
	if ((d > 1) && (distance(y, x, ny, nx) > d)) return FALSE;

	/* Require "line of sight" */
	if (!los(y, x, ny, nx)) return FALSE;

	/* Require extra restriction, if any. */
	if (accept && !(*accept)(ny, nx)) return FALSE;

	return TRUE;
}

/*
 * Standard "find me a location" function
 *
 * Obtains a legal location within the given distance of the initial
 * location, and with "los()" from the source to destination location.
 *
 * This function is often called from inside a loop which searches for
 * locations while increasing the "d" distance.
 *
 * accept is an extra restriction which must be satisfied by the location,
 * if any.
 */
bool scatter(int *yp, int *xp, int y, int x, int d, bool (*accept)(int, int))
{
	int nx, ny, t;

	for (ny = y-d, t = 0; ny <= y+d; ny++)
	{
		for (nx = x-d; nx <= x+d; nx++)
		{
			if (scatter_good(ny, nx, y, x, d, accept)) t++;
		}
	}

	/* No valid locations found. */
	if (!t) return FALSE;

	/* Pick a location. */
	t = rand_int(t);

	for (ny = y-d; ny <= y+d; ny++)
	{
		for (nx = x-d; nx <= x+d; nx++)
		{
			/* Not the chosen location. */
			if (!scatter_good(ny, nx, y, x, d, accept) || t--) continue;

			(*yp) = ny;
			(*xp) = nx;
			return TRUE;
		}
	}

	/* Paranoia - scatter_good() should be TRUE as many times in each loop. */
	return FALSE;
}




/*
 * Track a new monster
 */
void health_track(int m_idx)
{
	/* Track a new guy */
	p_ptr->health_who = m_idx;

	/* Redraw (later) */
	p_ptr->redraw |= (PR_HEALTH);
}



/*
 * Hack -- track the given monster race
 */
void monster_race_track(int r_idx)
{
	/* Save this monster ID */
	p_ptr->monster_race_idx = r_idx;

	/* Window stuff */
	p_ptr->window |= (PW_MONSTER);
}



/*
 * Hack -- track the given object kind
 */
void object_kind_track(int k_idx)
{
	/* Save this object ID */
	p_ptr->object_kind_idx = k_idx;

	/* Window stuff */
	p_ptr->window |= (PW_OBJECT);
}



/*
 * Track the given item.
 */
void object_track(object_type *o_ptr)
{
	/* object_track(0) means "forget it, whatever it was". */
	if (!o_ptr)
	{
		tracked_o_ptr = o_ptr;
	}
	/* As does a request to track a hidden object. */
	else if (hidden_p(o_ptr))
	{
		tracked_o_ptr = NULL;
	}
	/* Always remember real objects. */
	else if (o_ptr->k_idx)
	{
		/* Save the object ID */
		tracked_o_ptr = o_ptr;
	}
	/* A repeated call to a now-blank object means "forget it". */
	else if (o_ptr == tracked_o_ptr)
	{
		tracked_o_ptr = NULL;
	}
	/* A call to an untracked blank object is ignored totally. */
	else
	{
		return;
	}

	/* Window stuff */
	p_ptr->window |= (PW_OBJECT_DETAILS);
}

/*
 * Track the given floor square.
 */
void cave_track(const int y, const int x)
{
	/* Remember the new cave square (or 0 to clear). */
	tracked_co_ord.x = x;
	tracked_co_ord.y = y;

	p_ptr->window |= (PW_FLOOR);
}



/*
 * Something has happened to disturb the player.
 *
 * The first arg indicates a major disturbance, which affects search.
 *
 * The second arg is currently unused, but could induce output flush.
 *
 * All disturbance cancels repeated commands, resting, and running.
 */
void disturb(int stop_stealth)
{
	/* Cancel auto-commands */
	/* p_ptr->command_new = 0; */

	/* Cancel repeated commands */
	if (p_ptr->command_rep)
	{
		/* Cancel */
		p_ptr->command_rep = 0;

		/* Redraw the state (later) */
		p_ptr->redraw |= (PR_STATE);

		/* Calculate torch radius */
		p_ptr->update |= (PU_TORCH);
	}

	/* Cancel stealth if requested */
	if (stop_stealth && p_ptr->sneaking)
	{
		/* Cancel */
		do_cmd_toggle_sneak();
	}

	/* Flush the input if requested */
	if (flush_disturb) flush();
}





/*
 * Hack -- Check if a level is a "quest" level
 */
bool is_quest(void)
{
	int i;

	/* Town is never a quest */
	if (!dun_level) return (FALSE);

	/* Check quests */
	for (i = 0; i < MAX_Q_IDX; i++)
	{
		/* Check for quest */
		if ((q_list[i].level == dun_level) &&
			(q_list[i].dungeon == p_ptr->cur_dungeon)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}



