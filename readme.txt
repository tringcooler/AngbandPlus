

                          *************************
                          ** sCthangband 1.0.18  **
                          *************************

           Based on Moria:    Copyright (c) 1985 Robert Alan Koeneke
               and Umoria:    Copyright (c) 1989 James E. Wilson

                Angband 2.7 - 2.8: Ben Harrison (benh@phial.com)

               Zangband code:    (Mr) Topi Ylinen <f1toyl@uta.fi>

             Cthangband 2.1 - 4.0: Dean Anderson (no current address)
             
             Cthangband 4.1: David Thornley <david@thornley.net>

       Current maintainer : Kieron Dunbar <kieron@dimetrodon.demon.co.uk>

   sCthangband Web-site <http://www.dimetrodon.demon.co.uk/scthangband.html>


=== General Info ===

sCthangband is a "graphical" dungeon adventure game using textual characters
to represent the walls and floors of a dungeon and the inhabitants therein,
in the vein of "rogue", "hack", "nethack", and "moria".

There are some ascii "on line help" files in the "lib/help" directory.

See the Official sCthangband Web-site for a list (mostly complete) of what
has changed in each recent version.

See the above site, and also the various Angband ftp sites (including
"clockwork.dementia.org" and "ftp.sunet.se") for the latest files, patches,
and executables.

Contact Kieron Dunbar < kieron@dimetrodon.demon.co.uk > to report bugs.
Use the newsgroup "rec.games.roguelike.angband" to ask general questions
about the game, including compilation questions.

This version of sCthangband should run on Macintosh, Windows, Acorn, Amiga,
Unix (X11/Curses), Linux (X11/Curses), various DOS machines, and many
others...

See the Makefiles, h-config.h, and config.h in the src directory for details
on compiling.
See "Makefile.xxx" and "main-xxx.c" for various supported systems.


=== Quick and dirty compilation instructions ===

There may be a pre-compiled archive available, which contains everything
you need to install and play Angband.  If not, you must compile the source
code yourself.  Try the following non-trivial steps:

Step 1: Acquire.  Go to the sCthangband Web-site
                  Download the latest source code
Step 2: Extract.  Try "unzip *.zip"
Step 3: Prepare.  Try "cd scthangband*/src", then edit the "makefile.org" file
                  You may also edit "h-config.h" and "config.h"
Step 4: Compile.  Try "make -f makefile.org", and then "cd .." if successful
Step 5: Execute.  Try "./Cthangband -uTest" to initialize stuff
Step 6: Play....  Read the "online help" via the "?" command.

The above should work on most UNIX-style systems, and should work with some
further modifications otherwise.

Of course, if you have a compiler, you can compile a (possibly customized)
executable on almost any system.  You will need the "source archive" (as
above), which contains the standard "src" and "lib" directories, and for
some platforms (including Macintosh* and Windows), you will also need an
appropriate "extra archive", which contains some extra platform specific
files, and instructions about how to use them.  Some "extra archives" may
be found at the sCthangband Web-site (including "ext-dos.zip" and
"ext-win.zip"), but be sure that you get a version of the "extra archive"
designed for this version of sCthangband.

* There is no Macintosh archive at present, as I do not have a Linux dearchiver
for the versions I would otherwise adapt. A recent Macintosh archive for
another variant may be suitable, however.

=== Special instructions for certain platforms ===

The Macintosh requires that the "lib" folder be in the same folder as
the executable.  Also, note that System 7.5 (and perhaps others) are
brain damaged, and may default to the incorrect folder for opening
savefiles.  Make sure you keep all your savefiles in the proper place,
and if you load a savefile from the wrong place, note that the game
may decide to re-save your savefile in the proper place when you quit.
If you move the "lib" folder (or any ancestor folder) while a game is
in progress, you may not be able to save your game.

Some archive generation programs refuse to handle empty directories,
so special "fake" files with names like "DELETEME.TXT" may have been
placed into certain directories to avoid this problem.  You may safely
delete these files if you so desire.


=== Upgrading from older versions (and/or other platforms) ===

If you have been using an older version of sCthangband (and/or playing on
a different platform), or either version 4.0.1 or 4.1.0 of Cthangband, you
can "upgrade" (or "sidegrade") to this version, bringing your old savefiles,
high score list, and other files with you.

sCthangband uses a platform independant file format for the binary files
that store information about games in progress, known as "savefiles", and
is able to translate savefiles from all known versions of sCthangband.  To
use an "old" savefile, simply copy it into the "lib/save" directory, changing
the name of the savefile (if necessary) to satisfy the requirements of the
platform you are using. In general, the savefile should be named "UUU.NNN"
or "NNN" where "UUU" is the userid of the player (on "multiuser" systems),
and "NNN" is the name of the "character" in the savefile.  Note that only
"multiuser" platforms use the "UUU.NNN" form, and the "dot" is required.

sCthangband uses a platform independant file format for the binary file
used to store the high score list.  This file is named "scores.raw".  To
use an "old" high score list, simply copy it into the "lib/apex" directory.

sCthangband uses a set of special ascii "configuration files" which are
kept in the "lib/file" directory.  These files should not be modified (or
imported from older versions) unless you know exactly what you are doing,
but often you can use "old" versions of these files with little trouble.

sCthangband uses a set of ascii "user pref files" which are kept in the
"lib/user" directory.  Most of these files can only be used on a small set
of platforms, and may need slight modifications when imported from older
versions.  Note that only some of these files are auto-loaded by the game.


=== Directory "src" ===

The "src" directory contains the complete set of "standard" source files.


=== Directory "lib" ===

The "lib" directory contains all of sCthangband's special sub-directories.


=== Directory "lib/apex" ===

The "lib/apex" directory contains the "high score" files.

The "scores.raw" file contains the "high score" table, in a "semi-binary" form,
that is, all the bytes in the file are normal ascii values, but this includes
the special "nul" or "zero" byte, which is used to separate and pad records.
You should probably not attempt to modify this file with a normal text editor.
This file should be (more or less) portable between different platforms.  It
must be present (or creatable) for the game to run correctly.

The "record.txt" file contains a list of every game played since the file was
created in text format. It should normally be present, but need be neither
present nor creatable for the rest of the game to operate normally.

=== Directory "lib/bone" ===

The "lib/bone" directory contains special "player ghost" template files.

The files in this directory have filenames of the form "bone.NNN" where
"NNN" is the level on which the player died.  They contain information
about the dead player, currently, one line each for the player name,
maximum hitpoints, race, and class.  These files are probably portable,
and are probably compatible with older bone files, if those files are
renamed, and a final newline is added for compatibility.  Actually,
only the "player name" from these files is actually used.

=== Directory "lib/data" ===

The "lib/data" directory contains various special binary data files.

The *.raw files are binary image files constructed by parsing the ascii
template files in "lib/edit", described below.  These files are required,
but can be created by the game if the "lib/edit" directory contains the
proper files, and if the game was compiled to allow this creation.

=== Directory "lib/edit" ===

The "lib/edit" directory contains various special ascii data files.

The *.txt files are ascii template files used to construct the binary image
files in "lib/data", described above.  These files describe the "terrain
features", "object kinds", "artefacts", "ego-items", "monster races", and
"dungeon vaults", "unidentified object names", "base object names" and "maxima".

The ascii template files are easier to edit and more portable than hard-coded
arrays, prevent compilation errors on some machines, shrink the size of the
binary executable, and also provide a user-readable spoiler file of sorts.

These files should not be modified unless you know exactly what you are doing.

These files are optional if the game is distributed with pre-created
binary raw files for the system being used in "lib/data".


=== Directory "lib/file" ===

The "lib/file" directory contains various special ascii data files.

The 'news.txt' file is displayed to the user when the game starts up.  It
contains basic information such as my name and email address, and the names
of some of the people who have been responsible for previous versions of
sCthangband.  You may edit this file (slightly) to include local "site
specific" information such as who compiled the local executable.  You should
refer the user to a special "online help" file, if necessary, that describes
any local modifications in detail.  The first two lines of this file should
be blank, and only the next 20 lines should contain information.

The 'dead.txt' file is displayed to the user when the player dies.  It
contains a picture of a tombstone which is filled in with interesting
information about the dead player.  You should not edit this file.

The optional file 'wizards.txt' may be used to specify which users may enter
'wizard' mode.  A missing file provides no restrictions, and an empty file
prevents everyone from entering 'wizard' mode.  This file is only used on
multi-user machines, otherwise there are no restrictions.

The optional file 'time.txt' may be used to restrict the "times" at which
the game may be played, by providing specification of which hours of each day
of the week are legal for playing the game.  See 'files.c' for more details.
A missing file provides no restrictions, and an empty file will, by default,
forbid the playing of the game from 8am-5pm on weekdays.  This file is only
used on multi-user machines, and only if CHECK_TIME is defined, otherwise,
there are no restrictions.

The optional file 'load.txt' may be used to restrict the "load" which the game
may impose on the system.  See 'files.c' for more details.  A missing file
provides no restrictions, and an empty file will, by default, restrict the
"current load" to a maximal value of 100*FSCALE.  This file is only used on
multi-user machines, and only if CHECK_LOAD is defined, otherwise, there are
no restrictions.

There are various other files, which are described in the 'readme!' file in
the same directory.

These files should not be modified unless you know exactly what you are doing.


=== Directory "lib/help" ===

The "lib/help" directory contains the "online help" files, including the
version history in update.txt.

This directory is used to search for normal "online help" files.


=== Directory "lib/info" ===

The "lib/info" directory contains the "online spoiler" files.

This directory is used to search for any "online help" file that cannot
be found in the "lib/help" directory.

This directory is empty by default.  Many people use this directory for
"online spoiler files", many of which are available.

Note that the default "help.hlp" file allows the "9" key to access a help
file called "spoiler.hlp", and allows the "0" key to access "user.hlp".

These special help files can thus be placed in the user's own "info"
directory to allow the on line help to access his files.


=== Directory "lib/save" ===

The "lib/save" directory contains "savefiles" for the players.

Each savefile is named "NNN" where "NNN" is the name of the character, or,
on some machines, the name of the character, or, on multi-user machines,
"UUU.NNN", where "UUU" is the player uid and "NNN" is the character name.

The savefiles should be portable between systems, assuming that the
appropriate renaming is perfomed.


=== Directory "lib/user" ===

The "lib/user" directory contains the "user pref files", if any.

In general, these files are used to "customize" aspects of the game for
a given site or a given player.

See "src/files.c" for information on the proper "format" of these files.


=== Directory "lib/xtra" ===

The "lib/xtra" directory contains special system files, if any.




=== NO WARRANTY ===

    BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

    IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.


--- Ben Harrison, Robert Ruehlmann and Kieron Dunbar ---
