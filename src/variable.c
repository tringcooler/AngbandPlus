#define VARIABLE_C
/* File: variable.c */

/* Purpose: Angband variables */

/*
 * Copyright (c) 1989 James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research, and
 * not for profit purposes provided that this copyright and statement are
 * included in all such copies.
 */

#include "angband.h"


/*
 * Hack -- Link a copyright message into the executable
 */
cptr copyright[5] =
{
	"Copyright (c) 1989 James E. Wilson, Robert A. Keoneke",
	"",
	"This software may be copied and distributed for educational, research,",
	"and not for profit purposes provided that this copyright and statement",
	"are included in all such copies."
};

/*
 * Run-time arguments (arg_wizard only exists to keep system-specific files
 * happy).
 */
bool arg_fiddle = FALSE; /* Command arg -- Request fiddle mode */
bool arg_wizard = FALSE; /* Command arg -- Request wizard mode (unused) */
bool arg_sound = FALSE; /* Command arg -- Request special sounds */
bool arg_graphics = FALSE; /* Command arg -- Request graphics mode */
bool arg_force_original = FALSE; /* Command arg -- Request original keyset */
bool arg_force_roguelike = FALSE; /* Command arg -- Request roguelike keyset */

/*
 * Various things
 */

bool character_generated; /* The character exists */
bool character_dungeon; /* The character has a dungeon */
bool character_loaded; /* The character was loaded from a savefile */
bool character_saved; /* The character was just saved to a savefile */

bool character_icky; /* The game is in an icky full screen mode */
bool character_xtra; /* The game is in an icky startup mode */

u32b seed_flavor; /* Hack -- consistent object colors */
u32b seed_wild; /* Hack -- consistent wilderness layout */

//s16b command_cmd; /* Current "Angband Command" */

//s16b command_arg; /* Gives argument of current command */
//s16b command_rep; /* Gives repetition of current command */
//s16b command_dir; /* Gives direction of current command */

//s16b command_see; /* See "cmd1.c" */
//s16b command_wrk; /* See "cmd1.c" */

//s16b command_gap = 50; /* See "cmd1.c" */

//s16b command_new; /* Command chaining from inven/equip view */

//s16b energy_use; /* Energy use this turn */
//s16b old_energy_use; /* Energy use last turn */

bool msg_flag; /* Used in msg_print() for "buffering" */

//bool alive; /* True if game is running */

//bool death; /* True if player has died */

s16b cur_hgt; /* Current dungeon height */
s16b cur_wid; /* Current dungeon width */
s16b dun_level; /* Current dungeon level */
s16b dun_offset;       /* Monster/Object offset for current dungeon */
u16b dun_bias; /* Summon flag used to give the dungeon a bias */
s16b num_repro; /* Current reproducer count */
s16b object_level; /* Current object creation level */

s32b turn; /* Current game turn */
s32b old_turn; /* Turn when level began (feelings) */
s32b curse_turn; /* Turn when autocurse activates */

bool cheat_wzrd; /* Is the player currently in Wizard mode? */

bool use_sound; /* The "sound" mode is enabled */
bool use_graphics; /* The "graphics" mode is enabled */

u16b noscore; /* Track various "cheating" conditions */

bool use_transparency = FALSE; /* Use transparent tiles */

s16b signal_count; /* Hack -- Count interupts */

bool inkey_base; /* See the "inkey()" function */
bool inkey_scan; /* See the "inkey()" function */
bool inkey_flag; /* See the "inkey()" function */

bool shimmer_monsters; /* Hack -- optimize multi-hued monsters */

bool repair_monsters; /* Hack -- optimize detect monsters */
bool repair_mflag_born;	/* Hack -- repair monster flags (born) */
bool repair_mflag_nice;	/* Hack -- repair monster flags (nice) */
bool repair_mflag_show;	/* Hack -- repair monster flags (show) */
bool repair_mflag_mark;	/* Hack -- repair monster flags (mark) */

bool hack_mind; /* Prevent sanity_blast() on the first turn. */
bool hack_chaos_feature; /* Give a new character a chaos feature. */

s16b o_max = 1; /* Number of allocated objects */
s16b o_cnt = 0; /* Number of live objects */

s16b m_max = 1; /* Number of allocated monsters */
s16b m_cnt = 0; /* Number of live monsters */

bool multi_rew; /* Don't call gain_level_reward() more than once per turn. */

/*
 * Software options (set via the '=' command).  See "tables.c"
 */


/* Option Set 1 -- User Interface */

bool rogue_like_commands; /* Rogue-like commands */
bool quick_messages; /* Activate quick messages */
bool quick_prompt; /* Activate quick [y/n] prompts */
bool other_query_flag; /* Prompt for various information */
bool carry_query_flag; /* Prompt before picking things up */
bool use_old_target; /* Use old target by default */
bool always_pickup; /* Pick things up by default */
bool always_repeat; /* Repeat obvious commands */
bool depth_in_feet; /* Show dungeon level in feet */

bool stack_force_notes; /* Merge inscriptions when stacking */
bool stack_force_notes_all; /* Merge all inscriptions when stacking */
bool stack_force_costs; /* Merge discounts when stacking */

bool show_labels; /* Show labels in object listings */
bool show_weights; /* Show weights in object listings */
bool show_choices; /* Show choices in certain sub-windows */
bool show_details; /* Show more detailed monster descriptons */
bool show_choices_main; /* Show choices in main window */

bool ring_bell; /* Ring the bell (on errors, etc) */
bool use_color; /* Use color if possible (slow) */
bool verbose_haggle; /* Verbose messages if auto_haggle is set */
bool scroll_edge; /* Scroll until detection reaches the edge. */
bool show_piles; /* Show stacks with a special colour/character. */
bool beginner_help; /* Show a special help file at start-up. */
bool allow_fake_colour; /* Allow "fake" colours */


/* Option Set 2 -- Disturbance */

bool find_ignore_stairs; /* Run past stairs */
bool find_ignore_doors; /* Run through open doors */
bool find_cut; /* Run past known corners */
bool stop_corner; /* Stop at corners */
bool find_examine; /* Run into potential corners */

bool disturb_move; /* Disturb whenever any monster moves */
bool disturb_near; /* Disturb whenever viewable monster moves */
bool disturb_panel; /* Disturb whenever map panel changes */
bool disturb_state; /* Disturn whenever player state changes */
bool disturb_dawn; /* Disturb at sunrise or sunset on the surface. */
bool disturb_minor; /* Disturb whenever boring things happen */


bool alert_failure; /* Alert user to various failures */
bool last_words; /* Get last words upon dying */
bool small_levels; /* Allow unusually small dungeon levels */
bool empty_levels; /* Allow empty 'arena' levels */
bool equippy_chars; /* Back by popular demand... */
bool skip_chaos_features; /* Skip chaos feature screen even if we have it */
bool plain_descriptions; /* Plain object descriptions */
bool stupid_monsters; /* Monsters use old AI */
bool auto_destroy; /* No query to destroy known worthless items */
bool confirm_stairs; /* Prompt before staircases... */
bool wear_confirm; /* Confirm before putting on known cursed items */
bool confirm_wear_all; /* Confirm to wear/wield potentially cursed items */
bool disturb_allies; /* Allies moving nearby disturb us */
bool multi_stair;         /* Multiple level staircases */
bool unify_commands; /* Combine object commands into a single 'u'se command */
bool centre_view; /* Centre view on player */
bool macro_edit; /* Use macros as edit keys in string prompts */
bool no_centre_run; /* Stop centring when running */
bool track_mouse; /* Track the cursor in extra windows */
bool auto_more; /* Automatically clear '-more-' prompts */

/* Option Set B -- Birth Options */

bool preserve_mode; /* Don't lose missed artifacts */
bool maximise_mode; /* Unify stat bonuses */
bool use_autoroller; /* Autoroll characters */
bool spend_points; /* Spend points on stats */
bool ironman_shop; /* Not allowed in shops */
bool ironman_feeling; /* Only give real feeling after 2500 turns. */
bool speak_unique; /* Speaking uniques + shopkeepers */
#ifdef SCORE_QUITTERS
bool score_quitters; /* Quitting can give a high score */
#endif /* SCORE_QUITTERS */
bool chaos_patrons; /* Chaos patrons exist in the game. */

/* Option Set 3 -- Game-Play */

bool auto_haggle; /* Auto-haggle in stores */

bool auto_scum; /* Auto-scum for good levels */

bool stack_allow_items; /* Allow weapons and armor to stack */
bool stack_allow_wands; /* Allow wands/staffs/rods to stack */

bool expand_look; /* Expand the power of the look command */
bool expand_list; /* Expand the power of the list commands */

bool view_perma_grids; /* Map remembers all perma-lit grids */
bool view_torch_grids; /* Map remembers all torch-lit grids */

bool dungeon_align; /* Generate dungeons with aligned rooms */
bool dungeon_stair; /* Generate dungeons with connected stairs */
bool dungeon_small;         /* Generate dungeons with small levels always */

bool flow_by_sound; /* Monsters track new player location */
bool flow_by_smell; /* Monsters track old player location */

bool smart_learn; /* Monsters learn from their mistakes */
bool smart_cheat; /* Monsters exploit player weaknesses */


/* Option Set 4 -- Efficiency */

bool view_reduce_lite; /* Reduce lite-radius when running */
bool view_reduce_view; /* Reduce view-radius in town */

bool avoid_abort; /* Avoid checking for user abort */
bool avoid_other; /* Avoid processing special colors */

bool flush_error; /* Flush input on incorrect keypresses. */
bool flush_failure; /* Flush input on any failure */
bool flush_disturb; /* Flush input on disturbance */

bool fresh_before; /* Flush output before normal commands */
bool fresh_after; /* Flush output after normal commands */
bool fresh_message; /* Flush output after all messages */

bool compress_savefile; /* Compress messages in savefiles */

bool hilite_player; /* Hilite the player with the cursor */

bool view_yellow_lite; /* Use special colors for torch-lit grids */
bool view_bright_lite; /* Use special colors for 'viewable' grids */

bool view_granite_lite; /* Use special colors for wall grids (slow) */
bool view_special_lite; /* Use special colors for floor grids (slow) */

/* Option set 5 -- Testing */

bool testing_stack; /* Test the stacking code */

bool testing_carry; /* Test the carrying code */


/* Spoiler options */
bool spoil_art; /* Benefit from artefact spoilers */
bool spoil_mon; /* Know complete monster info */
bool spoil_ego; /* Know complete ego item info */
bool spoil_value; /* Know the apparent prices of items */
bool spoil_base; /* Know complete info about base items */
bool spoil_stat; /* Know the significance of stat values */
bool spoil_dam; /* Know the damage done by a melee weapon */
bool spoil_flag; /* Know the effects of various flags */
bool spoil_skll; /* Shows an additional digit in Skill screen */

/* Cheating options */

bool cheat_peek; /* Peek into object creation */
bool cheat_hear; /* Peek into monster creation */
bool cheat_room; /* Peek into dungeon creation */
bool cheat_xtra; /* Peek into something else */
bool cheat_item; /* Know complete item info */
bool cheat_live; /* Allow player to avoid death */
bool cheat_skll;        /* Peek into skill rolls */
bool cheat_save; /* Prevent automatic saving on game events. */

/* Special options */

bool allow_quickstart; /* Allow Quick-Start */
#ifdef USE_MAIN_C
bool display_credits; /* Require a keypress to clear the initial screen. */
#endif
bool allow_pickstats; /* Allow the player to choose a stat template. */

s16b hitpoint_warn = 20; /* Hitpoint warning (0 to 9) */

s16b delay_factor = 64; /* Delay factor (0 to 9) */

bool autosave_l;        /* Autosave before entering new levels */
bool autosave_t;        /* Timed autosave */
bool autosave_q;        /* Quiet autosave */
s16b autosave_freq;     /* Autosave frequency */
bool allow_squelch = TRUE; /* Allow squelching to take place. */


/*
 * Dungeon variables
 */

s16b feeling; /* Most recent feeling */
s16b rating; /* Level's current rating */

/* 
 * There is no room further than this many squares from the player for monster
 * creation.
 */
int full_grid; 



/*
 * Health bar variable -DRS-
 */
//s16b health_who;

/*
 * Monster race to track
 */
s16b monster_race_idx;

/*
 * Object kind to track
 */
s16b object_kind_idx;

/*
 * Object to track
 */
object_type *tracked_o_ptr;

/*
 * Floor square to track
 */
co_ord tracked_co_ord;

/*
 * User info
 */
int player_uid;
int player_euid;
int player_egid;

/*
 * Current player's character name
 */
char player_name[NAME_LEN];

/*
 * Stripped version of "player_name"
 */
char player_base[NAME_LEN];

/*
 * What killed the player
 */
cptr died_from;

/*
 * Hack -- Textual "history" for the Player
 */
char history[4][60];

/*
 * Buffer to hold the current savefile name
 */
char savefile[1024]="";



/*
 * Array of grids for use by various functions (see "cave.c")
 */
s16b temp_n;
byte temp_y[TEMP_MAX];
byte temp_x[TEMP_MAX];


/*
 * Number of active macros.
 */
s16b macro__num;

/*
 * Array of macro patterns [MACRO_MAX]
 */
cptr *macro__pat;

/*
 * Array of macro actions [MACRO_MAX]
 */
cptr *macro__act;

/*
 * Current macro action [1024]
 */
char *macro__buf;


#ifdef ALLOW_MACROS
/*
 * Pointer in macro__buf for recording a keymap action.
 */
char *keymap_buf_ptr;

/*
 * Pointer in macro__buf to show where the current command invocation began.
 */
char *keymap_cmd_ptr;
#endif /* ALLOW_MACROS */

/*
 * The pointers to the quarks [QUARK_MAX]
 */
cptr *quark__str;


/*
 * The array of offsets, by index [MESSAGE_MAX]
 */
u16b *message__ptr;

/*
 * The array of chars, by offset [MESSAGE_BUF]
 */
char *message__buf;

/*
 * Boolean - the next message belongs to a new turn.
 */
bool new_message_turn; /* Allow message__turn to be reset. */

/*
 * Global table of color definitions
 */
byte angband_color_table[256][4] =
{
	{0x00, 0x00, 0x00, 0x00}, /* TERM_DARK */
	{0x00, 0xFF, 0xFF, 0xFF}, /* TERM_WHITE */
	{0x00, 0x80, 0x80, 0x80}, /* TERM_SLATE */
	{0x00, 0xFF, 0x80, 0x00}, /* TERM_ORANGE */
	{0x00, 0xC0, 0x00, 0x00}, /* TERM_RED */
	{0x00, 0x00, 0x80, 0x40}, /* TERM_GREEN */
	{0x00, 0x00, 0x40, 0xFF}, /* TERM_BLUE */
	{0x00, 0x80, 0x40, 0x00}, /* TERM_UMBER */
	{0x00, 0x60, 0x60, 0x60}, /* TERM_L_DARK */
	{0x00, 0xC0, 0xC0, 0xC0}, /* TERM_L_WHITE */
	{0x00, 0xFF, 0x00, 0xFF}, /* TERM_VIOLET */
	{0x00, 0xFF, 0xFF, 0x00}, /* TERM_YELLOW */
	{0x00, 0xFF, 0x00, 0x00}, /* TERM_L_RED */
	{0x00, 0x00, 0xFF, 0x00}, /* TERM_L_GREEN */
	{0x00, 0x00, 0xFF, 0xFF}, /* TERM_L_BLUE */
	{0x00, 0xC0, 0x80, 0x40} /* TERM_L_UMBER */
};


/*
 * Standard sound names
 */
char angband_sound_name[SOUND_MAX][16] =
{
	"",
	"hit",
	"miss",
	"flee",
	"drop",
	"kill",
	"level",
	"death",
	"study",
	"teleport",
	"shoot",
	"quaff",
	"zap",
	"walk",
	"tpother",
	"hitwall",
	"eat",
	"store1",
	"store2",
	"store3",
	"store4",
	"dig",
	"opendoor",
	"shutdoor",
	"tplevel",
	"scroll",
	"buy",
	"sell",
	"warn"
};

#ifdef MONSTER_FLOW

/*
 * The array of cave grid flow "cost" values
 */
byte cave_cost[DUNGEON_HGT][DUNGEON_WID];

/*
 * The array of cave grid flow "when" stamps
 */
byte cave_when[DUNGEON_HGT][DUNGEON_WID];

#endif	/* MONSTER_FLOW */

/*
 * The array of cave grid info flags
 */
byte cave_info[DUNGEON_HGT][DUNGEON_WID];

/*
 * The array of cave grid feature codes
 */
byte cave_feat[DUNGEON_HGT][DUNGEON_WID];

/*
 * The array of cave grid remembered feature codes. 
 */
byte cave_rfeat[DUNGEON_HGT][DUNGEON_WID];

/*
 * The array of cave grid object indexes
 *
 * Note that this array yields the index of the top object in the stack of
 * objects in a given grid, using the "next_o_idx" field in that object to
 * indicate the next object in the stack, and so on, using zero to indicate
 * "nothing".  This array replicates the information contained in the object
 * list, for efficiency, providing extremely fast determination of whether
 * any object is in a grid, and relatively fast determination of which objects
 * are in a grid.
 */
s16b cave_o_idx[DUNGEON_HGT][DUNGEON_WID];

/*
 * The array of cave grid monster indexes
 *
 * Note that this array yields the index of the monster or player in a grid,
 * where negative numbers are used to represent the player, positive numbers
 * are used to represent a monster, and zero is used to indicate "nobody".
 * This array replicates the information contained in the monster list and
 * the player structure, but provides extremely fast determination of which,
 * if any, monster or player is in any given grid.
 */
s16b cave_m_idx[DUNGEON_HGT][DUNGEON_WID];



/*
 * The array of dungeon items [MAX_O_IDX]
 */
object_type *o_list;

/*
 * The array of dungeon monsters [MAX_M_IDX]
 */
monster_type *m_list;

/*
 * The stores [MAX_STORES_TOTAL]
 */
store_type *store;

/*
 * The player's inventory [INVEN_TOTAL]
 */
object_type *inventory;


/*
 * The size of "alloc_kind_table" (at most MAX_K_IDX * 4)
 */
s16b alloc_kind_size;

/*
 * The entries in the "kind allocator table"
 */
alloc_entry *alloc_kind_table;


/*
 * The size of "alloc_race_table" (at most MAX_R_IDX)
 */
s16b alloc_race_size;

/*
 * The entries in the "race allocator table"
 */
alloc_entry *alloc_race_table;


/*
 * Specify attr/char pairs for visual special effects
 * Be sure to use "index & 0x7F" to avoid illegal access
 */
/* byte misc_to_attr[128]; */
/* char misc_to_char[128]; */


/*
 * Keymaps for each "mode" associated with each keypress.
 */
cptr keymap_act[KEYMAP_MODES][256];



/*** Player information ***/

/*
 * Player info record
 */
player_type p_body;

/*
 * Pointer to the player info
 */
player_type *p_ptr = &p_body;


/*
 * Pointer to the player tables
 * (sex, race, template, magic)
 */
player_sex *sp_ptr;
player_race *rp_ptr;
player_template *cp_ptr;



/*
 * The player other record (static)
 */
static player_other player_other_body;

/*
 * Pointer to the player other record
 */
player_other *op_ptr = &player_other_body;

/* Various maxima */
maxima *z_info = NULL;

/*
 * The vault generation arrays
 */
vault_type *v_info;
cptr v_name;
cptr v_text;

/*
 * The terrain feature arrays
 */
feature_type *f_info;
cptr f_name;
/* cptr f_text; */

/*
 * A table of features in order of priority, for the small scale dungeon map.
 */
feature_type **priority_table;
int feature_priorities;


/*
 * The object kind arrays
 */
object_kind *k_info;
cptr k_name;
cptr k_text;

/*
 * The unidentified object arrays
 */
unident_type *u_info;
char *u_name; /* This is written to in name_scrolls(). */

/*
 * The base object arrays
 */
o_base_type *o_base;
cptr ob_name;

/*
 * The artifact arrays
 */
artifact_type *a_info;
cptr a_name;
/* char *a_text; */

/*
 * The ego-item arrays
 */
ego_item_type *e_info;
cptr e_name;
/* char *e_text; */


/*
 * The monster race arrays
 */
monster_race *r_info;
char *r_name; /* This is written to during ghost creation. */
cptr r_text;

/*
 * The death event arrays
 */
death_event_type *death_event;
cptr event_name;
cptr event_text;

/*
 * The dungeon arrays.
 */
dun_type *dun_defs;
cptr dun_name;

/*
 * The town arrays.
 */
town_type *town_defs;
cptr town_name;

/*
 * The quest array.
 */
quest_type *q_list;

/*
 * The shopkeeper array.
 */
owner_type *owners;
cptr s_name;

/*
 * The player template array.
 */
player_template *template_info;
cptr tp_name;

/*
 * Hack -- The special Angband "System Suffix"
 * This variable is used to choose an appropriate "pref-xxx" file
 */
cptr ANGBAND_SYS = "xxx";

/*
 * Hack -- The special Angband "Graphics Suffix"
 * This variable is used to choose an appropriate "graf-xxx" file
 */
cptr ANGBAND_GRAF = "old";

/*
 * High score files (binary)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_APEX;

/*
 * Bone files for player ghosts (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_BONE;

/*
 * Binary image files for the "*_info" arrays (binary)
 * These files are not portable between platforms
 */
cptr ANGBAND_DIR_DATA;

/*
 * Textual template files for the "*_info" arrays (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_EDIT;

/*
 * Various extra files (ascii)
 * These files may be portable between platforms
 */
cptr ANGBAND_DIR_FILE;

/*
 * Help files (normal) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_HELP;

/*
 * Help files (spoilers) for the online help (ascii)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_INFO;

/*
 * Default user "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_PREF;

/*
 * Savefiles for current characters (binary)
 * These files are portable between platforms
 */
cptr ANGBAND_DIR_SAVE;

/*
 * User "preference" files (ascii)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_USER;

/*
 * Various extra files (binary)
 * These files are rarely portable between platforms
 */
cptr ANGBAND_DIR_XTRA;


/*
 * Here is a "hook" used during calls to "get_item()" and
 * "show_inven()", and the choice window routines.
 */
bool (*item_tester_hook)(object_ctype*);



/* Hack, violet uniques */
bool violet_uniques = FALSE;

#ifdef ALLOW_EASY_OPEN
bool easy_open = TRUE;
#endif /* ALLOW_EASY_OPEN -- TNB */

#ifdef ALLOW_EASY_DISARM
bool easy_disarm = TRUE;
#endif /* ALLOW_EASY_DISARM -- TNB */

/*
 * Player stat defaults
 */
stat_default_type *stat_default;
s16b stat_default_total;

/*
 * Initialisation macros
 */
#ifdef ALLOW_TEMPLATES

init_macro_type *macro_info = NULL;
char *macro_name;
char *macro_text;

u32b rebuild_raw = 0;

#endif /* ALLOW_TEMPLATES */

/* Number of skill checks the player has had since touching a new object. */
byte object_skill_count;
