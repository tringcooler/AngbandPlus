/* File: types.h */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/*
 * Note that "char" may or may not be signed, and that "signed char"
 * may or may not work on all machines.  So always use "s16b" or "s32b"
 * for signed values.  Also, note that unsigned values cause math problems
 * in many cases, so try to only use "u16b" and "u32b" for "bit flags",
 * unless you really need the extra bit of information, or you really
 * need to restrict yourself to a single byte for storage reasons.
 *
 * Also, if possible, attempt to restrict yourself to sub-fields of
 * known size (use "s16b" or "s32b" instead of "int", and "byte" instead
 * of "bool"), and attempt to align all fields along four-byte words, to
 * optimize storage issues on 32-bit machines.  Also, avoid "bit flags"
 * since these increase the code size and slow down execution.  When
 * you need to store bit flags, use one byte per flag, or, where space
 * is an issue, use a "byte" or "u16b" or "u32b", and add special code
 * to access the various bit flags.
 *
 * Many of these structures were developed to reduce the number of global
 * variables, facilitate structured program design, allow the use of ascii
 * template files, simplify access to indexed data, or facilitate efficient
 * clearing of many variables at once.
 *
 * Note that certain data is saved in multiple places for efficient access,
 * and when modifying the data in one place it must also be modified in the
 * other places, to prevent the creation of inconsistant data.
 */

/**** Available Types ****/

/*
 * An array of 256 byte's
 */
typedef byte byte_256[256];

/*
 * An array of 256 s16b's
 */
typedef s16b s16b_256[256];

/*
 * An array of MAX_DUNGEON_WID byte's
 */
typedef byte byte_wid[MAX_DUNGEON_WID];

/*
 * An array of MAX_DUNGEON_WID s16b's
 */
typedef s16b s16b_wid[MAX_DUNGEON_WID];

/**** Available Structs ****/

typedef struct maxima maxima;
typedef struct feature_type feature_type;
typedef struct object_kind object_kind;
typedef struct artifact_type artifact_type;
typedef struct ego_item_type ego_item_type;
typedef struct item_prefix_type item_prefix_type;
typedef struct monster_blow monster_blow;
typedef struct monster_unique monster_unique;
typedef struct monster_race monster_race;
typedef struct monster_lore monster_lore;
typedef struct monster_list_entry monster_list_entry;
typedef struct vault_type vault_type;
typedef struct object_type object_type;
typedef struct monster_type monster_type;
typedef struct alloc_entry alloc_entry;
typedef struct quest_type quest_type;
typedef struct owner_type owner_type;
typedef struct store_type store_type;
typedef struct magic_type magic_type;
typedef struct alchemy_info alchemy_info;
typedef struct spell_book spell_book;
typedef struct player_magic player_magic;
typedef struct player_sex player_sex;
typedef struct player_race player_race;
typedef struct player_race_special player_race_special;
typedef player_race_special *ptr_player_race_special;
typedef struct player_class player_class;
typedef struct hist_type hist_type;
typedef struct player_other player_other;
typedef struct option_type option_type;
typedef struct player_type player_type;
typedef struct start_item start_item;

/**** Available structs ****/

/*
 * Information about maximal indices of certain arrays
 * Actually, these are not the maxima, but the maxima plus one
 */
struct maxima
{
	u32b fake_text_size;
	u32b fake_name_size;

	u16b f_max;		/* Max size for "f_info[]" */

	u16b k_max;		/* Max size for "k_info[]" */
	u16b a_max;		/* Max size for "a_info[]" */
	u16b e_max;		/* Max size for "e_info[]" */

	u16b r_max;		/* Max size for "r_info[]" */
	u16b u_max;		/* Max size for "u_info[]" */
	
	u16b v_max;		/* Max size for "v_info[]" */

	u16b p_max;		/* Max size for "p_info[]" */
	u16b h_max;		/* Max size for "h_info[]" */

	u16b b_max;		/* Max size per element of "b_info[]" */
	u16b c_max; 	/* Max size for "c_info[]" */

	u16b o_max;		/* Max size for "o_list[]" */
	u16b m_max;		/* Max size for "m_list[]" */

	u16b q_max;		/* Max size for "q_info[]" */

	u16b a_min_normal;	/* First normal artifact */
};

/*
 * Information about terrain "features"
 */
struct feature_type
{
	u32b name;			/* Name (offset) */

	byte mimic;			/* Feature to mimic */

	byte extra;			/* Extra byte (unused) */

	s16b unused;		/* Extra bytes (unused) */

	byte d_attr;		/* Default feature attribute */
	char d_char;		/* Default feature character */

	byte x_attr;		/* Desired feature attribute */
	char x_char;		/* Desired feature character */
};

/*
 * Information about object "kinds", including player knowledge.
 */
struct object_kind
{
	u32b name;					/* Name (offset) */

	byte tval;					/* Object type */
	byte sval;					/* Object sub type */

	byte material;				/* Object type material */

	s16b pval;					/* Object extra info */

	s16b to_h;					/* Bonus to hit */
	s16b to_d;					/* Bonus to damage */
	s16b to_a;					/* Bonus to armor */

	s16b ac;					/* Base armor */

	byte dd, ds;				/* Damage dice/sides */

	s16b weight;				/* Weight */

	byte qd, qs;				/* Quantity dice/sides */
	
	byte breakage;				/* Chance of breaking when thrown/fired */

	s32b cost;					/* Object "base cost" */

	u32b flags1;				/* Flags, set 1 */
	u32b flags2;				/* Flags, set 2 */
	u32b flags3;				/* Flags, set 3 */
	u32b flags4;				/* Flags, set 4 */

	byte locale[MAX_OBJ_ALLOC];	/* Allocation level(s) */
	byte chance[MAX_OBJ_ALLOC];	/* Allocation chance(s) */

	byte level;					/* Level */
	byte extra;					/* Something */

	byte d_attr;				/* Default object attribute */
	char d_char;				/* Default object character */

	byte x_attr;				/* Desired object attribute */
	char x_char;				/* Desired object character */

	byte flavor;				/* Special object flavor (or zero) */

	bool easy_know;				/* This object is always known (if aware) */

	bool aware;					/* The player is "aware" of the item's effects */

	bool tried;					/* The player has "tried" one of the items */

	bool squelch;				/* squelch item if known            */
	bool everseen;				/* Used to despoilify squelch menus */
};

/*
 * Information about "artifacts".
 */
struct artifact_type
{
	u32b name;			/* Name (offset) */

	byte tval;			/* Artifact type */
	byte sval;			/* Artifact sub type */

	s16b pval;			/* Artifact extra info */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to damage */
	s16b to_a;			/* Bonus to armor */

	s16b ac;			/* Base armor */

	byte dd, ds;		/* Damage when hits */

	s16b weight;		/* Weight */

	s32b cost;			/* Artifact "cost" */

	u32b flags1;		/* Artifact Flags, set 1 */
	u32b flags2;		/* Artifact Flags, set 2 */
	u32b flags3;		/* Artifact Flags, set 3 */
	u32b flags4;		/* Artifact Flags, set 4 */

	byte level;			/* Artifact level */
	byte rarity;		/* Artifact rarity */

	byte status;		/* Status of the artifact (created, etc.) */

	byte activation;	/* Activation to use */
	u16b time;			/* Activation time */
	u16b randtime;		/* Activation time dice */
};

/*
 * Information about "ego-items".
 */
struct ego_item_type
{
	u32b name;			/* Name (offset) */

	byte slot;			/* Standard slot value */
	byte rating;		/* Rating boost */

	byte level;			/* Minimum level */
	byte rarity;		/* Object rarity */

	byte tval[3];		/* Legal tval */
	byte min_sval[3];	/* Minimum legal sval */
	byte max_sval[3];	/* Maximum legal tval */
	byte xtra;			/* Extra Sustain/Resist/Power */

	byte max_to_h;		/* Maximum to-hit bonus */
	byte max_to_d;		/* Maximum to-dam bonus */
	byte max_to_a;		/* Maximum to-ac bonus */

	byte max_pval;		/* Maximum pval */

	s32b cost;			/* Ego-item "cost" */

	u32b flags1;		/* Ego-Item Flags, set 1 */
	u32b flags2;		/* Ego-Item Flags, set 2 */
	u32b flags3;		/* Ego-Item Flags, set 3 */
	u32b flags4;		/* Ego-Item Flags, set 4 */
};

/*
 * Information about "ego-items".
 */
struct item_prefix_type
{
	cptr name;			/* Name */

	byte rarity;		/* Rarity - the higher this is, the less chance of being generated */
	byte material;		/* Material */

	s16b to_h;			/* Modifer to hit */
	s16b to_d;			/* Modifer to damage */
	
	s16b dd;			/* Modifer to damage dice */
	s16b ds;			/* Modifer to damage dice sides*/

	u16b weight;		/* Weight (percentage) */
	u16b cost;			/* Cost (percentage) */

	bool store;			/* If true, this object can be generated in stores */
};

/*
 * Monster blow structure
 *
 *	- Method (RBM_*)
 *	- Effect (RBE_*)
 *	- Damage Dice
 *	- Damage Sides
 */
struct monster_blow
{
	byte method;
	byte effect;
	byte d_dice;
	byte d_side;
};

/*
 * Monster "unique" information.
 *
 * Note that "d_attr" is used for MORE than "visual" stuff. There is no "d_char"
 * since that must always be the same as the monster_race info.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 */
struct monster_unique
{
	s16b r_idx;				/* Monster race index */

	u32b name;				/* Name (offset) */
	u32b text;				/* Text (offset) */

	byte hdice;				/* Creatures hit dice count */
	byte hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	byte freq_spell;		/* Other spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */

	monster_blow blow[4];	/* Up to four blows per round */

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */

	byte d_attr;			/* Default monster attribute */

	/* Calculated fields */

	byte x_attr;			/* Desired monster attribute */
	byte x_char;			/* Desired monster character */

	s16b depth;				/* The unique's current depth (or depth it died in) */
	bool dead;				/* Marked when dead */
};

/*
 * Monster "race" information, including racial memories
 *
 * Note that "d_attr" and "d_char" are used for MORE than "visual" stuff.
 *
 * Note that "x_attr" and "x_char" are used ONLY for "visual" stuff.
 *
 * Note that "cur_num" (and "max_num") represent the number of monsters
 * of the given race currently on (and allowed on) the current level.
 * This information yields the "dead" flag for Unique monsters.
 *
 * Note that "max_num" is reset when a new player is created.
 * Note that "cur_num" is reset when a new level is created.
 *
 */
struct monster_race
{
	u32b name;				/* Name (offset) */
	u32b text;				/* Text (offset) */

	byte hdice;				/* Creatures hit dice count */
	byte hside;				/* Creatures hit dice sides */

	s16b ac;				/* Armour Class */

	s16b sleep;				/* Inactive counter (base) */
	byte aaf;				/* Area affect radius (1-100) */
	byte speed;				/* Speed (normally 110) */

	s32b mexp;				/* Exp value for kill */

	s16b extra;				/* Unused (for now) */

	byte freq_spell;		/* Other spell frequency */

	u32b flags1;			/* Flags 1 (general) */
	u32b flags2;			/* Flags 2 (abilities) */
	u32b flags3;			/* Flags 3 (race/resist) */
	u32b flags4;			/* Flags 4 (inate/breath) */
	u32b flags5;			/* Flags 5 (normal spells) */
	u32b flags6;			/* Flags 6 (special spells) */

	monster_blow blow[4];	/* Up to four blows per round */

	byte level;				/* Level of creature */
	byte rarity;			/* Rarity of creature */

	byte d_attr;			/* Default monster attribute */
	char d_char;			/* Default monster character */

	byte x_attr;			/* Desired monster attribute */
	char x_char;			/* Desired monster character */

	/* Special */

	byte max_unique;		/* How many uniques exist for this monster */

	/* Calculated fields */

	byte cur_num;			/* Monster population on current level */

	byte cur_unique;		/* How many uniques remain for this monster */
};

/*
 * Monster "lore" information
 *
 * Note that these fields are related to the "monster recall" and can
 * be scrapped if space becomes an issue, resulting in less "complete"
 * monster recall (no knowledge of spells, etc). XXX XXX XXX
 *
 * ToDo: The "r_" prefix is no longer needed and should be removed.
 */
struct monster_lore
{
	s16b r_sights;			/* Count sightings of this monster */
	s16b r_deaths;			/* Count deaths from this monster */

	s16b r_pkills;			/* Count monsters killed in this life */
	s16b r_tkills;			/* Count monsters killed in all lives */

	byte r_wake;			/* Number of times woken up (?) */
	byte r_ignore;			/* Number of times ignored (?) */

	byte r_drop_gold;		/* Max number of gold dropped at once */
	byte r_drop_item;		/* Max number of item dropped at once */

	byte r_cast;			/* Max number of spells seen */

	byte r_blows[4];		/* Number of times each blow type was seen */

	u32b r_flags1;			/* Observed racial flags */
	u32b r_flags2;			/* Observed racial flags */
	u32b r_flags3;			/* Observed racial flags */
	u32b r_flags4;			/* Observed racial flags */
	u32b r_flags5;			/* Observed racial flags */
	u32b r_flags6;			/* Observed racial flags */
};

/*
 * Structure for building monster "lists"
 */
struct monster_list_entry
{
	s16b r_idx;			/* Monster race index */
	s16b u_idx;			/* Unique index (for uniques) */

	byte amount;
};

/*
 * Information about "vault generation"
 */
struct vault_type
{
	u32b name;			/* Name (offset) */
	u32b text;			/* Text (offset) */

	byte typ;			/* Vault type */

	byte rat;			/* Vault rating */

	byte hgt;			/* Vault height */
	byte wid;			/* Vault width */
};

/*
 * Object information, for a specific object.
 *
 * Note that a "discount" on an item is permanent and never goes away.
 *
 * Note that inscriptions are now handled via the "quark_str()" function
 * applied to the "note" field, which will return NULL if "note" is zero.
 *
 * Note that "object" records are "copied" on a fairly regular basis,
 * and care must be taken when handling such objects.
 *
 * Note that "object flags" must now be derived from the object kind,
 * the artifact and ego-item indexes, and the two "xtra" fields.
 *
 * Each cave grid points to one (or zero) objects via the "o_idx"
 * field (above).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a "stack" of objects in the same grid.
 *
 * Each monster points to one (or zero) objects via the "hold_o_idx"
 * field (below).  Each object then points to one (or zero) objects
 * via the "next_o_idx" field, forming a singly linked list, which
 * in game terms, represents a pile of objects held by the monster.
 *
 * The "held_m_idx" field is used to indicate which monster, if any,
 * is holding the object.  Objects being held have "ix=0" and "iy=0".
 */
struct object_type
{
	s16b k_idx;			/* Kind index (zero if "dead") */

	byte iy;			/* Y-position on map, or zero */
	byte ix;			/* X-position on map, or zero */

	byte tval;			/* Item type (from kind) */
	byte sval;			/* Item sub-type (from kind) */

	s16b pval;			/* Item extra-parameter */

	byte discount;		/* Discount (if any) */

	byte number;		/* Number of items */

	s16b weight;		/* Item weight */

	byte a_idx;			/* Artifact type, if any */
	byte e_idx;			/* Ego-Item type, if any */
	byte prefix_idx;	/* Prefix type, if any */

	byte xtra1;			/* Extra info type */
	byte xtra2;			/* Extra info index */

	s16b to_h;			/* Plusses to hit */
	s16b to_d;			/* Plusses to damage */
	s16b to_a;			/* Plusses to AC */

	s16b ac;			/* Normal AC */

	byte dd, ds;		/* Damage dice/sides */

	s16b timeout;		/* Timeout Counter */

	byte ident;			/* Special flags  */

	byte marked;		/* Object is marked */

	u16b note;			/* Inscription index */

	byte inscrip;		/* INSCRIP_XXX constant */

	s16b next_o_idx;	/* Next object in stack (if any) */

	s16b held_m_idx;	/* Monster holding us (if any) */
};

/*
 * A structure to hold a tval and its description
 */
typedef struct tval_desc_type tval_desc_type;

struct tval_desc_type
{
	int	 tval;
	cptr desc;
};

/*
 * Monster information, for a specific monster.
 *
 * Note: fy, fx constrain dungeon size to 256x256
 *
 * The "hold_o_idx" field points to the first object of a stack
 * of objects (if any) being carried by the monster (see above).
 */
struct monster_type
{
	s16b r_idx;			/* Monster race index */
	s16b u_idx;			/* Unique index (for uniques) */

	byte fy;			/* Y location on map */
	byte fx;			/* X location on map */

	s16b hp;			/* Current Hit points */
	s16b maxhp;			/* Max Hit points */

	s16b csleep;		/* Inactive counter */

	byte mspeed;		/* Monster "speed" */
	byte energy;		/* Monster "energy" */

	byte stunned;		/* Monster is stunned */
	byte confused;		/* Monster is confused */
	byte monfear;		/* Monster is afraid */
	byte blinded;		/* Monster is blinded */ 
	byte calmed;		/* Monster is calmed */  

	u16b bleeding;		/* Monster is bleeding */ 
	u16b poisoned;		/* Monster is poisoned */ 

	byte cdis;			/* Current dis from player */

	byte mflag;			/* Extra monster flags */

	bool ml;			/* Monster is "visible" */

	s16b hold_o_idx;	/* Object being held (if any) */

	u32b smart;			/* Field for "smart_learn" */

	byte attr;			/* Monster's attribute */
};

/*
 * An entry for the object/monster allocation functions
 *
 * Pass 1 is determined from allocation information
 * Pass 2 is determined from allocation restriction
 * Pass 3 is determined from allocation calculation
 */
struct alloc_entry
{
	s16b index;		/* The actual index */

	byte level;		/* Base dungeon level */
	byte prob1;		/* Probability, pass 1 */
	byte prob2;		/* Probability, pass 2 */
	byte prob3;		/* Probability, pass 3 */

	u16b total;		/* Unused for now */
};

/*
 * Structure for the "quests"
 */
struct quest_type
{
	u32b name;			/* Name (offset) */

	byte type;			/* Quest Type */
	byte reward;		/* Quest Reward */

	byte active_level;	/* Equals dungeon level if not completed, 0 if completed */
	byte base_level;	/* The dungeon level on which the quest was assigned*/

	s16b r_idx;			/* Monster race */

	s16b cur_num;		/* Number killed */
	s16b max_num;		/* Number required */

	bool started;		/* Has the player start the quest */
};

/*
 * A store owner
 */
struct owner_type
{
	u32b owner_name;	/* Name (offset) */
	s16b max_cost;		/* Purse limit */
	byte owner_race;	/* Owner race */

#ifdef ALLOW_HAGGLE
	byte max_inflate;	/* Inflation (max) */
	byte min_inflate;	/* Inflation (min) */

	byte haggle_per;	/* Haggle unit */

	byte insult_max;	/* Insult limit */
#else /* ALLOW_HAGGLE */
	byte inflate;		/* Inflation */
#endif /* ALLOW_HAGGLE */

};

/*
 * A store, with an owner, various state flags, a current stock
 * of items, and a table of items that are often purchased.
 */
struct store_type
{
	byte owner;				/* Owner index */

#ifdef ALLOW_HAGGLE

	s16b insult_cur;		/* Insult counter */

	s16b good_buy;			/* Number of "good" buys */
	s16b bad_buy;			/* Number of "bad" buys */

	s32b store_open;		/* Closed until this turn */
#endif

	s16b table_num;			/* Table -- Number of entries */
	s16b table_size;		/* Table -- Total Size of Array */
	s16b *table;			/* Table -- Legal item kinds */

	byte stock_num;			/* Stock -- Number of entries */
	s16b stock_size;		/* Stock -- Total Size of Array */
	object_type *stock;		/* Stock -- Actual stock items */
};

/*
 * The "name" of spell 'N' is stored as spell_names[X][N],
 * where X is 0 for mage-spells and 1 for priest-spells.
 */
struct magic_type
{
	byte index;			/* The internal spell index. */
	cptr sname;			/* The name of the spell in the spellbook */
	byte slevel;		/* Required level (to learn) */
	byte smana;			/* Required mana (to cast) */
	byte sfail;			/* Minimum chance of failure */
	byte sexp;			/* Encoded experience bonus */
};

/*
 * Alchemy Information
 */
struct alchemy_info
{
	byte sval1;
	bool known1;
	byte sval2;
	bool known2;
};

/* 
 * Information about a spell book's contents
 */
struct spell_book
{
	byte flags;		/* Spellbook Flags */

	magic_type contents[MAX_BOOK_SPELLS];
};

/*
 * Player sex info
 */
struct player_sex
{
	cptr title;			/* Type of sex */

	cptr winner;		/* Name of winner */
};

/*
 * Player racial info
 */
struct player_race
{
	u32b name;			/* Name (offset) */

	s16b r_adj[A_MAX];	/* Racial stat bonuses */

	s16b r_skill[SK_MAX];	/* Skills */

	byte r_mhp;			/* Race hit-dice modifier */
	byte r_exp;			/* Race experience factor */

	byte b_age;			/* base age */
	byte m_age;			/* mod age */

	byte m_b_ht;		/* base height (males) */
	byte m_m_ht;		/* mod height (males) */
	byte m_b_wt;		/* base weight (males) */
	byte m_m_wt;		/* mod weight (males) */

	byte f_b_ht;		/* base height (females) */
	byte f_m_ht;		/* mod height (females)	  */
	byte f_b_wt;		/* base weight (females) */
	byte f_m_wt;		/* mod weight (females) */

	byte infra;			/* Infra-vision	range */

	byte choice;		/* Legal class choices */

	s16b hist;			/* Starting history index */

	byte special;		/* Race special abilities index */

	byte prefix;		/* Race's starting weapon prefix */

	u32b flags1;		/* Racial Flags, set 1 */
	u32b flags2;		/* Racial Flags, set 2 */
	u32b flags3;		/* Racial Flags, set 3 */
	u32b flags4;		/* Racial Flags, set 4 */
};

/*
 * Player race special info
 */
struct player_race_special
{
	cptr name;			/* name */
	s16b r_adj[A_MAX];	/* Racial stat bonuses */

	s16b r_skill[SK_MAX];	/* Skills */

	u32b flags1;		/* Racial Flags, set 1 */
	u32b flags2;		/* Racial Flags, set 2 */
	u32b flags3;		/* Racial Flags, set 3 */
	u32b flags4;		/* Racial Flags, set 4 */

	u16b power;			/* Special power for that level */

};

/*
 * Starting equipment entry 
 */
struct start_item
{
	byte tval;	/* Item's Tval */
	byte sval;	/* Item's Sval */
	byte ego;   /* Item's Name2 */
	byte min;	/* Minimum starting amount */
	byte max;	/* Maximum starting amount */
};

/*
 * Player class info
 */
struct player_class
{
	u32b name;								/* Name (offset) */

	u32b title[10];							/* Titles - offset */

	s16b c_adj[A_MAX];						/* Class stat modifier */

	s16b c_skill[SK_MAX];					/* Class skills */
	s16b x_skill[SK_MAX];					/* Skill extra points */

	s16b c_mhp;								/* Class hit-dice adjustment */
	s16b c_exp;								/* Class experience factor */

	u32b flags;								/* Class Flags */

	u16b max_attacks;						/* Maximum possible attacks */
	u16b min_weight;						/* Minimum weapon weight for calculations */
	u16b att_multiply;						/* Multiplier for attack calculations */

	s16b spell_stat1;						/* First stat for spells (if any)  */
	s16b spell_stat2;						/* Second stat for spells (if any)  */
	s16b spell_weight;						/* Weight that hurts spells */
	bool spell_book[SV_BOOK_MAX];			/* The list of legal books */
	s16b spell_handicap[SV_BOOK_MAX];		/* Spell handicap per realm */

	start_item start_items[MAX_START_ITEMS];/* The starting inventory */
};

/*
 * Player background information
 */
struct hist_type
{
	u32b unused;			/* Unused */
	u32b text;			    /* Text (offset) */

	byte roll;			    /* Frequency of this entry */
	byte chart;			    /* Chart index */
	byte next;			    /* Next chart index */
	byte bonus;			    /* Social Class Bonus + 50 */
};

/*
 * Some more player information
 *
 * This information is retained across player lives
 */
struct player_other
{
	char full_name[32];		/* Full name */
	char base_name[32];		/* Base name */

	bool opt[OPT_NORMAL];			/* Options */
	bool opt_birth[OPT_BIRTH];		/* Options */
	bool opt_adult[OPT_BIRTH];		/* Options */
	bool opt_cheat[OPT_CHEAT];		/* Options */
	bool opt_score[OPT_CHEAT];		/* Options */
	bool opt_squelch[OPT_SQUELCH];	/* Options */

	u16b window_flag[ANGBAND_TERM_MAX];	/* Window flags */

	byte hitpoint_warn;		/* Hitpoint warning (0 to 9) */
	byte delay_factor;		/* Delay factor (0 to 9) */

	byte squelch_level[MAX_SQ_TYPES];	/* Squelch levels for the secondary squelching */
};

/*
 * The descriptions and default values of the in-game options
 */
struct option_type
{
	cptr text;
	cptr descript;
	bool norm;
};

/*
 * Most of the "player" information goes here.
 *
 * This stucture gives us a large collection of player variables.
 *
 * This entire structure is wiped when a new character is born.
 *
 * This structure is more or less laid out so that the information
 * which must be saved in the savefile precedes all the information
 * which can be recomputed as needed.
 */
struct player_type
{
	s16b py;			/* Player location */
	s16b px;			/* Player location */

	byte psex;			/* Sex index */
	byte prace;			/* Race index */
	byte pclass;		/* Class index */
	byte oops;			/* unused */

	byte hitdie;		/* Hit dice (sides) */
	u16b expfact;		/* Experience factor */

	s16b age;			/* Characters age */
	s16b ht;			/* Height */
	s16b wt;			/* Weight */
	s16b sc;			/* Social Class */

	s32b au;			/* Current Gold */

	s16b max_depth;		/* Max depth */
	s16b depth;			/* Cur depth */

	s16b max_lev;		/* Max level */
	s16b lev;			/* Cur level */

	s32b max_exp;		/* Max experience */
	s32b exp;			/* Cur experience */
	u16b exp_frac;		/* Cur exp frac (times 2^16) */

	s16b mhp;			/* Max hit pts */
	s16b chp;			/* Cur hit pts */
	u16b chp_frac;		/* Cur hit frac (times 2^16) */

	s16b msp;			/* Max mana pts */
	s16b csp;			/* Cur mana pts */
	u16b csp_frac;		/* Cur mana frac (times 2^16) */

	byte stat_max[A_MAX];	/* Current "maximal" stat values */
	byte stat_cur[A_MAX];	/* Current "natural" stat values */

	s16b fast;			/* Timed -- Fast */
	s16b slow;			/* Timed -- Slow */
	s16b blind;			/* Timed -- Blindness */
	s16b paralyzed;		/* Timed -- Paralysis */
	s16b confused;		/* Timed -- Confusion */
	s16b afraid;		/* Timed -- Fear */
	s16b image;			/* Timed -- Hallucination */
	s16b poisoned;		/* Timed -- Poisoned */
	s16b cut;			/* Timed -- Cut */
	s16b stun;			/* Timed -- Stun */
	s16b diseased;		/* Timed -- Diseased */

	s16b protevil;		/* Timed -- Protection */
	s16b resilient;		/* Timed -- Resilience */
	s16b absorb;		/* Timed -- Absorb next hit */
	s16b hero;			/* Timed -- Heroism */
	s16b rage;			/* Timed -- Berserk */
	s16b shield;		/* Timed -- Shield Spell */
	s16b blessed;		/* Timed -- Blessed */
	s16b tim_invis;		/* Timed -- Invisiblity */

	s16b tim_infra;		/* Timed -- Infra Vision */
	s16b tim_see_invis;	/* Timed -- See Invisible */

	s16b oppose_acid;	/* Timed -- oppose acid */
	s16b oppose_elec;	/* Timed -- oppose lightning */
	s16b oppose_fire;	/* Timed -- oppose heat */
	s16b oppose_cold;	/* Timed -- oppose cold */
	s16b oppose_pois;	/* Timed -- oppose poison */
	s16b oppose_disease; /* Timed -- oppose disease */
	
	s16b tim_res_lite;		/* Timed -- resist light */
	s16b tim_res_dark;		/* Timed -- resist darkness */
	s16b tim_res_confu;		/* Timed -- resist confusion */
	s16b tim_res_sound;		/* Timed -- resist sound */
	s16b tim_res_shard;		/* Timed -- resist shards */
	s16b tim_res_nexus;		/* Timed -- resist nexus */
	s16b tim_res_nethr;		/* Timed -- resist nether */
	s16b tim_res_chaos;		/* Timed -- resist chaos */
	s16b tim_res_water;		/* Timed -- resist water */

	s16b racial_power;	/* Timed -- Racial power recharge */

	s16b word_recall;	/* Word of recall counter */

	s16b energy;		/* Current energy */

	s16b food;			/* Current nutrition */

	byte searching;		/* Currently searching */

	u16b spell_learned[SV_BOOK_MAX];	/* Spell flags */
	u16b spell_worked[SV_BOOK_MAX];		/* Spell flags */
	u16b spell_forgotten[SV_BOOK_MAX];	/* Spell flags */

	byte spell_order[SV_BOOK_MAX * MAX_BOOK_SPELLS][2];	/* Spell order */

	s16b player_hp[PY_MAX_LEVEL];	/* HP Array */

	char died_from[80];		/* Cause of death */
	char history[5][55];	/* Initial history */

	u16b total_winner;		/* Total winner */
	u16b panic_save;		/* Panic save */

	u16b noscore;			/* Cheating flags */

	bool is_dead;			/* Player is dead */

	/*** Temporary fields ***/

	bool playing;			/* True if player is playing */

	bool leaving;			/* True if player is leaving */

	bool create_up_stair;	/* Create up stair on next level */
	bool create_down_stair;	/* Create down stair on next level */

	s16b wy;				/* Dungeon panel */
	s16b wx;				/* Dungeon panel */

	byte cur_hgt;			/* Current dungeon level hight */
	byte cur_wid;			/* Current dungeon level width */

	s32b total_weight;		/* Total weight being carried */

	s16b inven_cnt;			/* Number of items in inventory */
	s16b equip_cnt;			/* Number of items in equipment */

	s16b target_set;		/* Target flag */
	s16b target_who;		/* Target identity */
	s16b target_row;		/* Target location */
	s16b target_col;		/* Target location */

	s16b health_who;		/* Health bar trackee */

	s16b monster_race_idx;		/* Monster race trackee */
	s16b monster_unique_idx;	/* Monster unique trackee */

	s16b object_kind_idx;	/* Object kind trackee */
	s16b object_pval;		/* Object pval trackee */

	s16b energy_use;		/* Energy use this turn */

	s16b resting;			/* Resting counter */
	s16b running;			/* Running counter */

	s16b run_cur_dir;		/* Direction we are running */
	s16b run_old_dir;		/* Direction we came from */
	bool run_unused;		/* Unused (padding field) */
	bool run_open_area;		/* Looking for an open area */
	bool run_break_right;	/* Looking for a break (right) */
	bool run_break_left;	/* Looking for a break (left) */

	s16b command_cmd;		/* Gives identity of current command */
	s16b command_arg;		/* Gives argument of current command */
	s16b command_rep;		/* Gives repetition of current command */
	s16b command_dir;		/* Gives direction of current command */

	s16b command_see;		/* See "cmd1.c" */
	s16b command_wrk;		/* See "cmd1.c" */

	s16b command_new;		/* Hack -- command chaining XXX XXX */

	s16b new_spells;		/* Number of spells available */

	s16b old_spells;

	u16b old_cumber_armor; /* Amount */
	bool old_cumber_glove;
	bool old_heavy_wield;
	bool old_heavy_shoot;
	bool old_icky_wield;

	s16b old_lite;		/* Old radius of lite (if any) */
	s16b old_view;		/* Old radius of view (if any) */

	s16b old_food_aux;	/* Old value of food */

	u16b cumber_armor;	/* Mana draining armor - amount */
	bool cumber_glove;	/* Mana draining gloves */
	bool heavy_wield;	/* Heavy weapon */
	bool heavy_shoot;	/* Heavy shooter */
	bool icky_wield;	/* Icky weapon */

	s16b cur_lite;		/* Radius of lite (if any) */

	u32b notice;		/* Special Updates (bit flags) */
	u32b update;		/* Pending Updates (bit flags) */
	u32b redraw;		/* Normal Redraws (bit flags) */
	u32b window;		/* Window Redraws (bit flags) */

	byte stat_use[A_MAX];	/* Current modified stats */
	byte stat_top[A_MAX];	/* Maximal modified stats */

	/*** Extracted fields ***/

	s16b stat_add[A_MAX];	/* Equipment stat bonuses */

	s16b mana_add; /* Equipment mana bonus */
	s16b hp_add; /*Equipment health bonus */

	u32b tim_flag1; /* Item flags for timed effects (for display) */
	u32b tim_flag2;	/* Item flags for timed effects (for display) */
	u32b tim_flag3; /* Item flags for timed effects (for display) */
	u32b tim_flag4; /* Item flags for timed effects (for display) */

	bool immune_acid;	/* Immunity to acid */
	bool immune_elec;	/* Immunity to lightning */
	bool immune_fire;	/* Immunity to fire */
	bool immune_cold;	/* Immunity to cold */

	bool resist_acid;	/* Resist acid */
	bool resist_elec;	/* Resist lightning */
	bool resist_fire;	/* Resist fire */
	bool resist_cold;	/* Resist cold */

	bool resist_pois;	/* Resist poison */

	bool resist_disease;/* Resist disease */
	bool resist_lite;	/* Resist light */
	bool resist_dark;	/* Resist darkness */
	bool resist_confu;	/* Resist confusion */
	bool resist_sound;	/* Resist sound */
	bool resist_shard;	/* Resist shards */
	bool resist_water;	/* Resist water */
	bool resist_nexus;	/* Resist nexus */
	bool resist_nethr;	/* Resist nether */
	bool resist_chaos;	/* Resist chaos */
	bool resist_disen;	/* Resist disenchant */
	bool resist_time;	/* Resist time */
	bool resist_mana;	/* Resist mana */
	
	bool free_act;		/* Free action */
	bool hold_life;		/* Hold life */
	bool bravery;		/* Bravery */
	bool no_blind;		/* Resist blindness */

	bool sustain_str;	/* Keep strength */
	bool sustain_int;	/* Keep intelligence */
	bool sustain_wis;	/* Keep wisdom */
	bool sustain_dex;	/* Keep dexterity */
	bool sustain_con;	/* Keep constitution */
	bool sustain_chr;	/* Keep charisma */

	bool slow_digest;	/* Slower digestion */
	bool ffall;			/* Feather falling */
	bool lite;			/* Permanent light */
	bool regenerate;	/* Regeneration */
	bool telepathy;		/* Telepathy */
	bool invis;			/* Invisible */
	bool see_inv;		/* See invisible */
	bool luck;			/* Luck */

	bool disrupt;		/* Disrupt spells */
	bool aggravate;		/* Aggravate monsters */
	bool teleport;		/* Random teleporting */
	bool exp_drain;		/* Experience draining */
	bool item_drain;	/* Item draining */

	bool bless_blade;	/* Blessed blade */

	s16b dis_to_h;		/* Known bonus to hit */
	s16b dis_to_d;		/* Known bonus to dam */
	s16b dis_to_a;		/* Known bonus to ac */

	s16b dis_ac;		/* Known base ac */

	s16b to_h;			/* Bonus to hit */
	s16b to_d;			/* Bonus to dam */
	s16b to_a;			/* Bonus to ac */

	s16b ac;			/* Base ac */

	s16b see_infra;		/* Infravision range */

	s16b skill[SK_MAX];	/* Skills */

	u32b noise;			/* Derived from stealth */

	s16b num_blow;		/* Number of blows */
	s16b num_fire;		/* Number of shots */

	byte ammo_mult;		/* Ammo multiplier */

	byte ammo_tval;		/* Ammo variety */

	s16b pspeed;		/* Current speed */

	u16b cur_quest;		/* Current quest */

	/* Other fields */
	s16b obj_depth;		/* Depth for object generation purposes*/

	byte feeling;		/* Level feeling */
	s32b feeling_cnt;	/* Hack - counter for level feeling */

};

/*
 * Semi-Portable High Score List Entry (128 bytes)
 *
 * All fields listed below are null terminated ascii strings.
 *
 * In addition, the "number" fields are right justified, and
 * space padded, to the full available length (minus the "null").
 *
 * Note that "string comparisons" are thus valid on "pts".
 */
typedef struct high_score high_score;

struct high_score
{
	char what[8];		/* Version info (string) */

	char pts[10];		/* Total Score (number) */

	char gold[10];		/* Total Gold (number) */

	char turns[10];		/* Turns Taken (number) */

	char day[10];		/* Time stamp (string) */

	char who[16];		/* Player Name (string) */

	char uid[8];		/* Player UID (number) */

	char sex[2];		/* Player Sex (string) */
	char p_r[3];		/* Player Race (number) */
	char p_c[3];		/* Player Class (number) */

	char cur_lev[4];		/* Current Player Level (number) */
	char cur_dun[4];		/* Current Dungeon Level (number) */
	char max_lev[4];		/* Max Player Level (number) */
	char max_dun[4];		/* Max Dungeon Level (number) */

	char how[32];		/* Method of death (string) */
};
