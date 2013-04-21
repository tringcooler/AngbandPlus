/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/* The maximum number of items per page in stores */
#define MAX_ENTRIES (Term->hgt - 12)

#define MAX_COMMENT_1	6

static cptr comment_1[MAX_COMMENT_1] =
{
	"Okay.",
	"Fine.",
	"Accepted!",
	"Agreed!",
	"Done!",
	"Taken!"
};

#define MAX_COMMENT_2A	2

static cptr comment_2a[MAX_COMMENT_2A] =
{
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};

#define MAX_COMMENT_2B	12

static cptr comment_2b[MAX_COMMENT_2B] =
{
	"I can take no less than %s gold pieces.",
	"I will accept no less than %s gold pieces.",
	"Ha!  No less than %s gold pieces.",
	"You knave!  No less than %s gold pieces.",
	"That's a pittance!  I want %s gold pieces.",
	"That's an insult!  I want %s gold pieces.",
	"As if!  How about %s gold pieces?",
	"My arse!  How about %s gold pieces?",
	"May the fleas of 1000 orcs molest you!  Try %s gold pieces.",
	"May your most favourite parts go moldy!  Try %s gold pieces.",
	"May the Witch-King find you tasty!  Perhaps %s gold pieces?",
	"Your mother was an Ogre!  Perhaps %s gold pieces?"
};

#define MAX_COMMENT_3A	2

static cptr comment_3a[MAX_COMMENT_3A] =
{
	"You try my patience.  %s is final.",
	"My patience grows thin.  %s is final."
};


#define MAX_COMMENT_3B	12

static cptr comment_3b[MAX_COMMENT_3B] =
{
	"Perhaps %s gold pieces?",
	"How about %s gold pieces?",
	"I will pay no more than %s gold pieces.",
	"I can afford no more than %s gold pieces.",
	"Be reasonable.  How about %s gold pieces?",
	"I'll buy it as scrap for %s gold pieces.",
	"That is too much!  How about %s gold pieces?",
	"That looks war surplus!  Say %s gold pieces?",
	"Never!  %s is more like it.",
	"That's an insult!  %s is more like it.",
	"%s gold pieces and be thankful for it!",
	"%s gold pieces and not a copper more!"
};

#define MAX_COMMENT_4A	4

static cptr comment_4a[MAX_COMMENT_4A] =
{
	"Enough!  You have abused me once too often!",
	"Arghhh!  I have had enough abuse for one day!",
	"That does it!  You shall waste my time no more!",
	"This is getting nowhere!  I'm going to Londis!"
};

#define MAX_COMMENT_4B	4

static cptr comment_4b[MAX_COMMENT_4B] =
{
	"Leave my store!",
	"Get out of my sight!",
	"Begone, you scoundrel!",
	"Out, out, out!"
};

#define MAX_COMMENT_5	8

static cptr comment_5[MAX_COMMENT_5] =
{
	"Try again.",
	"Ridiculous!",
	"You will have to do better than that!",
	"Do you wish to do business or not?",
	"You've got to be kidding!",
	"You'd better be kidding!",
	"You try my patience.",
	"Hmmm, nice weather we're having."
};

#define MAX_COMMENT_6	4

static cptr comment_6[MAX_COMMENT_6] =
{
	"I must have heard you wrong.",
	"I'm sorry, I missed that.",
	"I'm sorry, what was that?",
	"Sorry, what was that again?"
};



/*
 * Successful haggle.
 */
static void say_comment_1(void)
{
	message(MSG_STORE5, 0, comment_1[rand_int(MAX_COMMENT_1)]);
}


/*
 * Continue haggling (player is buying)
 */
static void say_comment_2(s32b value, int annoyed)
{
	char tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_2a[rand_int(MAX_COMMENT_2A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_2b[rand_int(MAX_COMMENT_2B)], tmp_val);
	}
}


/*
 * Continue haggling (player is selling)
 */
static void say_comment_3(s32b value, int annoyed)
{
	char tmp_val[80];

	/* Prepare a string to insert */
	sprintf(tmp_val, "%ld", (long)value);

	/* Final offer */
	if (annoyed > 0)
	{
		/* Formatted message */
		msg_format(comment_3a[rand_int(MAX_COMMENT_3A)], tmp_val);
	}

	/* Normal offer */
	else
	{
		/* Formatted message */
		msg_format(comment_3b[rand_int(MAX_COMMENT_3B)], tmp_val);
	}
}


/*
 * You must leave my store
 */
static void say_comment_4(void)
{
	msg_print(comment_4a[rand_int(MAX_COMMENT_4A)]);
	msg_print(comment_4b[rand_int(MAX_COMMENT_4B)]);
}


/*
 * You are insulting me
 */
static void say_comment_5(void)
{
	msg_print(comment_5[rand_int(MAX_COMMENT_5)]);
}


/*
 * You are making no sense.
 */
static void say_comment_6(void)
{
	msg_print(comment_6[rand_int(MAX_COMMENT_6)]);
}



/*
 * Messages for reacting to purchase prices.
 */

#define MAX_COMMENT_7A	4

static cptr comment_7a[MAX_COMMENT_7A] =
{
	"Arrgghh!",
	"You bastard!",
	"You hear someone sobbing...",
	"The shopkeeper howls in agony!"
};

#define MAX_COMMENT_7B	4

static cptr comment_7b[MAX_COMMENT_7B] =
{
	"Damn!",
	"You fiend!",
	"The shopkeeper curses at you.",
	"The shopkeeper glares at you."
};

#define MAX_COMMENT_7C	4

static cptr comment_7c[MAX_COMMENT_7C] =
{
	"Cool!",
	"You've made my day!",
	"The shopkeeper giggles.",
	"The shopkeeper laughs loudly."
};

#define MAX_COMMENT_7D	4

static cptr comment_7d[MAX_COMMENT_7D] =
{
	"Yipee!",
	"I think I'll retire!",
	"The shopkeeper jumps for joy.",
	"The shopkeeper smiles gleefully."
};


/*
 * Let a shop-keeper React to a purchase
 *
 * We paid "price", it was worth "value", and we thought it was worth "guess"
 */
static void purchase_analyze(s32b price, s32b value, s32b guess)
{
	/* Item was worthless, but we bought it */
	if ((value <= 0) && (price > value))
	{
		/* Comment */
		message(MSG_STORE1, 0, comment_7a[rand_int(MAX_COMMENT_7A)]);
	}

	/* Item was cheaper than we thought, and we paid more than necessary */
	else if ((value < guess) && (price > value))
	{
		/* Comment */
		message(MSG_STORE2, 0, comment_7b[rand_int(MAX_COMMENT_7B)]);
	}

	/* Item was a good bargain, and we got away with it */
	else if ((value > guess) && (value < (4 * guess)) && (price < value))
	{
		/* Comment */
		message(MSG_STORE3, 0, comment_7c[rand_int(MAX_COMMENT_7C)]);
	}

	/* Item was a great bargain, and we got away with it */
	else if ((value > guess) && (price < value))
	{
		/* Comment */
		message(MSG_STORE4, 0, comment_7d[rand_int(MAX_COMMENT_7D)]);
	}
}





/*
 * We store the current "store number" here so everyone can access it
 */
static int store_num = 7;

static bool show_services;
static bool store_has_services;

/*
 * We store the current "store page" here so everyone can access it
 */
static int store_top = 0;

/*
 * We store the current "store pointer" here so everyone can access it
 */
static store_type *st_ptr = NULL;

/*
 * We store the current "owner type" here so everyone can access it
 */
static owner_type *ot_ptr = NULL;

/*List of various store services allowed*/

#define SERVICE_ENCHANT_ARMOR	0
#define SERVICE_ENCHANT_TO_HIT	1
#define SERVICE_ENCHANT_TO_DAM	2
#define SERVICE_ELEM_BRAND_WEAP	3
#define SERVICE_ELEM_BRAND_AMMO	4
#define SERVICE_RECHARGING		5
#define	SERVICE_IDENTIFY		6
#define SERVICE_IDENTIFY_FULLY	7
#define	SERVICE_CURE_CRITICAL	8
#define	SERVICE_RESTORE_LIFE_LEVELS	9
#define SERVICE_REMOVE_CURSE	10
#define SERVICE_REMOVE_HEAVY_CURSE	11
#define SERVICE_RESTORE_STAT	12
#define SERVICE_INCREASE_STAT	13
#define SERVICE_CREATE_RANDART	14
#define SERVICE_PROBE_QUEST_MON	15
#define SERVICE_BUY_HEALING_POTION	16
#define SERVICE_BUY_LIFE_POTION	17
#define SERVICE_ABANDON_QUEST	18
#define SERVICE_FIREPROOF_BOOK  19

#define STORE_SERVICE_MAX 		20

/* Indicates which store offers the service*/
static byte service_store[STORE_SERVICE_MAX] =
{
	STORE_ARMOR,		/*  SERVICE_ENCHANT_ARMOR   	*/
	STORE_WEAPON,		/*  SERVICE_ENCHANT_TO_HIT   	*/
	STORE_WEAPON,		/*  SERVICE_ENCHANT_TO_DAM   	*/
	STORE_WEAPON,		/*  SERVICE_ELEM_BRAND_WEAP   	*/
	STORE_WEAPON,		/*  SERVICE_ELEM_BRAND_AMMO   	*/
	STORE_MAGIC, 		/*  SERVICE_RECHARGING   		*/
	STORE_MAGIC, 		/*  SERVICE_IDENTIFY   			*/
	STORE_MAGIC, 		/*  SERVICE_IDENTIFY_FULLY		*/
	STORE_TEMPLE, 		/*  SERVICE_CURE_CRITICAL  		*/
	STORE_TEMPLE, 		/*  SERVICE_RESTORE_LIFE_LEVELS	*/
	STORE_TEMPLE, 		/*  SERVICE_REMOVE_CURSE   		*/
	STORE_TEMPLE, 		/*  SERVICE_REMOVE_HEAVY_CURSE	*/
	STORE_ALCHEMY, 		/*  SERVICE_RESTORE_STAT   		*/
	STORE_ALCHEMY, 		/*  SERVICE_INCREASE_STAT   	*/
	STORE_GUILD,		/*  SERVICE_CREATE_RANDART   	*/
	STORE_GUILD,		/*	SERVICE_PROBE_QUEST_MON		*/
	STORE_TEMPLE,		/*	SERVICE_BUY_HEALING_POTION	*/
	STORE_TEMPLE,		/*	SERVICE_BUY_LIFE_POTION		*/
	STORE_GUILD,		/*	SERVICE_ABANDON_QUEST		*/
	STORE_BOOKSHOP		/*	SERVICE_FIREPROOF_BOOK		*/
};

/* Indicates the base price of the service*/
static u32b service_price[STORE_SERVICE_MAX] =
{
	125,				/*  SERVICE_ENCHANT_ARMOR   	*/
	125,				/*  SERVICE_ENCHANT_TO_HIT   	*/
	125,				/*  SERVICE_ENCHANT_TO_DAM   	*/
	35000,				/*  SERVICE_ELEM_BRAND_WEAP   	*/
	17500,				/*  SERVICE_ELEM_BRAND_AMMO   	*/
	175, 				/*  SERVICE_RECHARGING   		*/
	75, 				/*  SERVICE_IDENTIFY   			*/
	4500,		 		/*  SERVICE_IDENTIFY_FULLY		*/
	75, 				/*  SERVICE_CURE_CRITICAL  		*/
	1000, 				/*  SERVICE_RESTORE_LIFE_LEVELS	*/
	300, 				/*  SERVICE_REMOVE_CURSE   		*/
	15000, 				/*  SERVICE_REMOVE_HEAVY_CURSE	*/
	700, 				/*  SERVICE_RESTORE_STAT   		*/
	37500L, 			/*  SERVICE_INCREASE_STAT   	*/
	750000L,			/*  SERVICE_CREATE_RANDART   	*/
	150,				/*	SERVICE_PROBE_QUEST_MON		*/
	20000L,				/*	SERVICE_BUY_HEALING_POTION	*/
	125000L,			/*	SERVICE_BUY_LIFE_POTION		*/
	0,					/*  SERVICE_ABANDON_QUEST */
	100000L				/*  SERVICE_FIREPROOF_BOOK */

};

/*
 * Indicates the base price of the service.  [v] means the price varies depending on the item
 */
static cptr service_names[STORE_SERVICE_MAX] =
{
	"Enchant armor [v]",						/*  SERVICE_ENCHANT_ARMOR   	*/
	"Enchant to-hit [v]",						/*  SERVICE_ENCHANT_TO_HIT   	*/
	"Enchant to-dam [v]",						/*  SERVICE_ENCHANT_TO_DAM   	*/
	"Enchant weapon with elemental brand [v]",	/*  SERVICE_ELEM_BRAND_WEAP   	*/
	"Enchant ammunition with elemental brand [v]",	/*  SERVICE_ELEM_BRAND_AMMO */
	"Recharge item [v]",						/*  SERVICE_RECHARGING   		*/
	"Identify item",							/*  SERVICE_IDENTIFY   			*/
	"*Identify* item",							/*  SERVICE_IDENTIFY_FULLY		*/
	"Cure Critical Wounds",	 					/*  SERVICE_CURE_CRITICAL  		*/
	"Restore Life Levels",						/*  SERVICE_RESTORE_LIFE_LEVELS	*/
	"Remove curse", 							/*  SERVICE_REMOVE_CURSE   		*/
	"Remove *curse*", 							/*  SERVICE_REMOVE_HEAVY_CURSE	*/
	"Restore stat", 							/*  SERVICE_RESTORE_STAT   		*/
	"Increase stat", 							/*  SERVICE_INCREASE_STAT   	*/
	"Create Artifact[v]", 						/*  SERVICE_CREATE_RANDART   	*/
	"Inquire About Quest Monster[v]",			/*	SERVICE_PROBE_QUEST_MON		*/
	"Purchase Potion of Healing",				/*	SERVICE_BUY_HEALING_POTION	*/
	"Purchase Potion of Life",					/*	SERVICE_BUY_LIFE_POTION		*/
	"Abandon Your Quest",						/* SERVICE_ABANDON_QUEST */
	"Make a Dungeon Spell Book Fireproof[v]",	/* SERVICE_FIREPROOF_BOOK */
};


/*
 * Determine the price of an object (qty one) in a store.
 *
 * This function takes into account the player's charisma, and the
 * shop-keepers friendliness, and the shop-keeper's base greed, but
 * never lets a shop-keeper lose money in a transaction.
 *
 * The "greed" value should exceed 100 when the player is "buying" the
 * object, and should be less than 100 when the player is "selling" it.
 *
 * Hack -- the black market always charges twice as much as it should.
 *
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
static s32b price_item(const object_type *o_ptr, int greed, bool flip)
{
	int factor;
	int adjust;
	s32b price;

	/* Get the value of one of the items */
	price = object_value(o_ptr);

	/* Worthless items */
	if (price <= 0) return (0L);

	/* Compute the racial factor */
	factor = g_info[(ot_ptr->owner_race * z_info->p_max) + p_ptr->prace];

	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (store_num == STORE_B_MARKET) price = price / 2;
	}

	/* Shop is selling */
	else
	{
		/* Adjust for greed */
		adjust = 100 + ((greed + factor) - 300);

		/* Never get "silly" */
		if (adjust < 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (store_num == STORE_B_MARKET) price = price * 2;
	}

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Certain "cheap" objects should be created in "piles".
 *
 * Some objects can be sold at a "discount" (in smaller piles).
 *
 * Standard percentage discounts include 10, 25, 50, 75, and 90.
 */
static void mass_produce(object_type *o_ptr)
{
	int size = 1;

	int discount = 0;

	s32b cost = object_value(o_ptr);

	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Food, Flasks, and Lites */
		case TV_FOOD:
		case TV_FLASK:
		case TV_LITE:
		{
			if (cost <= 5L) size += damroll(2, 5);
			if (cost <= 20L) size += damroll(2, 5);
			break;
		}

		case TV_POTION:
		case TV_SCROLL:
		{
			if (cost <= 60L) size += damroll(3, 5);
			if (cost <= 240L) size += damroll(1, 5);
			break;
		}

		case TV_MAGIC_BOOK:
		case TV_PRAYER_BOOK:
		case TV_DRUID_BOOK:
		{
			if (cost <= 50L) size += damroll(2, 3);
			if (cost <= 500L) size += damroll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SHIELD:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DIGGING:
		case TV_BOW:
		{
			if (o_ptr->ego_num) break;
			if (cost <= 10L) size += damroll(2, 2);
			if (cost <= 100L) size += damroll(2, 2);
			break;
		}
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		case TV_SPIKE:
		{
			if (o_ptr->ego_num) size += damroll(3, 5);
			else if ((o_ptr->to_h > 0) || (o_ptr->to_d > 0)) size += damroll(4, 6);
			else size += damroll(8, 7);
			break;
		}

	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (one_in_(25))
	{
		discount = 10;
	}
	else if (one_in_(50))
	{
		discount = 25;
	}
	else if (one_in_(150))
	{
		discount = 50;
	}
	else if (one_in_(300))
	{
		discount = 75;
	}
	else if (one_in_(500))
	{
		discount = 90;
	}

	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = MAX(size - (size * discount / 100), 1);

	/* Hack -- rods need to increase PVAL if stacked */
	if (o_ptr->tval == TV_ROD)
	{
		o_ptr->pval = o_ptr->number * k_info[o_ptr->k_idx].pval;
	}
}



/*
 * Convert a store item index into a one character label
 *
 * We use labels "a"-"l" for page 1, and labels "m"-"x" for page 2.
 */
static s16b store_to_label(int i)
{
	/* Assume legal */
	return (I2A(i));
}


/*
 * Convert a one character label into a store item index.
 *
 * Return "-1" if the label does not indicate a real store item.
 */
static s16b label_to_store(int c)
{
	int i;

	/* Convert */
	i = (islower((unsigned char)c) ? A2I(c) : -1);

	/* Verify the index */
	if ((i < 0) || (i >= st_ptr->stock_num)) return (-1);

	/* Return the index */
	return (i);
}



/*
 * Determine if a store object can "absorb" another object.
 *
 * See "object_similar()" for the same function for the "player".
 *
 * This function can ignore many of the checks done for the player,
 * since stores (but not the home) only get objects under certain
 * restricted circumstances.
 */
static bool store_object_similar(const object_type *o_ptr, const object_type *j_ptr)
{

	/* Hack -- Identical items cannot be stacked */
	if (o_ptr == j_ptr) return (0);

	/* Different objects cannot be stacked */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);

	/* Different charges (etc) cannot be stacked. */
	if ((o_ptr->pval != j_ptr->pval)) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "artifact" names */
	if (o_ptr->art_num != j_ptr->art_num) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->ego_num != j_ptr->ego_num) return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Mega-Hack -- Handle lites */
	if (fuelable_lite_p(o_ptr))
	{
		if (o_ptr->timeout != j_ptr->timeout) return 0;
	}

	/* Hack -- Never stack recharging items */
	else if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac) return (0);
	if (o_ptr->dd != j_ptr->dd) return (0);
	if (o_ptr->ds != j_ptr->ds) return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);

	/* Require matching "discount" fields */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/*Allow well balanced items to stack only with other
			 *well balanced items*/
	if ((o_ptr->ident & IDENT_PERFECT_BALANCE) !=
        (o_ptr->ident & IDENT_PERFECT_BALANCE)) return (FALSE);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store object to absorb another object
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;

	/* Combine the histories */
	stack_histories(o_ptr, j_ptr);
}


/*
 * Check to see if the shop will be carrying too many objects
 *
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.  Before, one could "nuke" objects this way, by
 * adding them to a pile which was already full.
 */
static bool store_check_num(const object_type *o_ptr)
{
	int i;
	object_type *j_ptr;

	/* Free space is always usable */
	if (st_ptr->stock_num < st_ptr->stock_size) return TRUE;

	/* The "home" acts like the player */
	if (store_num == STORE_HOME)
	{
		/* Check all the objects */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing object */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* Normal stores do special stuff */
	else
	{
		/* Check all the objects */
		for (i = 0; i < st_ptr->stock_num; i++)
		{
			/* Get the existing object */
			j_ptr = &st_ptr->stock[i];

			/* Can the new object be combined with the old one? */
			if (store_object_similar(j_ptr, o_ptr)) return (TRUE);
		}
	}

	/* But there was no room at the inn... */
	return (FALSE);
}

/*
 * Determine if the current store will purchase the given object
 *
 * Note that a shop-keeper must refuse to buy "worthless" objects
 */
static bool store_will_buy(int store_num, const object_type *o_ptr)
{
	/* Hack -- The Home is simple */
	if (store_num == STORE_HOME) return (TRUE);

	/* Switch on the store */
	switch (store_num)
	{
		/* General Store */
		case STORE_GENERAL:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_FOOD:
				case TV_LITE:
				case TV_FLASK:
				case TV_SPIKE:
				case TV_SHOT:
				case TV_ARROW:
				case TV_BOLT:
				case TV_DIGGING:
				case TV_CLOAK:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Armoury */
		case STORE_ARMOR:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_SHIELD:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
				case TV_DRAG_SHIELD:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Weapon Shop */
		case STORE_WEAPON:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_SHOT:
				case TV_BOLT:
				case TV_ARROW:
				case TV_BOW:
				case TV_DIGGING:
				case TV_HAFTED:
				case TV_POLEARM:
				case TV_SWORD:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Temple */
		case STORE_TEMPLE:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{

				case TV_SCROLL:
				case TV_POTION:
				case TV_HAFTED:
			break;
				case TV_POLEARM:
				case TV_SWORD:
				{
					/* Known blessed blades are accepted too */
					if (is_blessed(o_ptr) && object_known_p(o_ptr)) break;
				}
				default:
				return (FALSE);
			}
			break;
		}

		/* Alchemist */
		case STORE_ALCHEMY:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{

				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Magic Shop */
		case STORE_MAGIC:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_AMULET:
				case TV_RING:
				case TV_TALISMAN:
				case TV_WAND:
				case TV_ROD:
				case TV_SCROLL:
				case TV_POTION:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		case STORE_BOOKSHOP:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_MAGIC_BOOK:
				case TV_DRUID_BOOK:
				case TV_PRAYER_BOOK:
				break;
				default:
				return (FALSE);
			}
		}
	}

	/* Ignore "worthless" items XXX XXX XXX */
	if (object_value(o_ptr) <= 0) return (FALSE);

	/* Assume okay */
	return (TRUE);
}



/*
 * Determine if the current store will purchase the given object
 */
static bool store_will_buy_tester(const object_type *o_ptr)
{
	return store_will_buy(store_num, o_ptr);
}


/*
 * Add an object to the inventory of the "Home"
 *
 * In all cases, return the slot (or -1) where the object was placed.
 *
 * Note that this is a hacked up version of "inven_carry()".
 *
 * Also note that it may not correctly "adapt" to "knowledge" bacoming
 * known, the player may have to pick stuff up and drop it again.
 */
static int home_carry(object_type *o_ptr)
{
	int i, slot;
	s32b value, j_value;
	object_type *j_ptr;


	/* Check each existing object (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing object */
		j_ptr = &st_ptr->stock[slot];

		/* The home acts just like the player */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Save the new number of items */
			object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Determine the "value" of the object */
	value = object_value(o_ptr);

	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that object */
		j_ptr = &st_ptr->stock[slot];

		/* Hack -- readable books always come first */
		if ((o_ptr->tval == cp_ptr->spell_book) &&
		    (j_ptr->tval != cp_ptr->spell_book)) break;
		if ((j_ptr->tval == cp_ptr->spell_book) &&
		    (o_ptr->tval != cp_ptr->spell_book)) continue;

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Can happen in the home */
		if (!object_aware_p(o_ptr)) continue;
		if (!object_aware_p(j_ptr)) break;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Objects in the home can be unknown */
		if (!object_known_p(o_ptr)) continue;
		if (!object_known_p(j_ptr)) break;

		/* Objects sort by decreasing value */
		j_value = object_value(j_ptr);
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		/* Hack -- slide the objects */
		object_copy(&st_ptr->stock[i], &st_ptr->stock[i-1]);
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Hack -- Insert the new object */
	object_copy(&st_ptr->stock[slot], o_ptr);

	/* Return the location */
	return (slot);
}


/*
 * Add an object to a real stores inventory.
 *
 * If the object is "worthless", it is thrown away (except in the home).
 *
 * If the object cannot be combined with an object already in the inventory,
 * make a new slot for it, and calculate its "per item" price.  Note that
 * this price will be negative, since the price will not be "fixed" yet.
 * Adding an object to a "fixed" price stack will not change the fixed price.
 *
 * In all cases, return the slot (or -1) where the object was placed
 */
static int store_carry(object_type *o_ptr)
{
	int i, slot;
	s32b value, j_value;
	object_type *j_ptr;


	/* Evaluate the object */
	value = object_value(o_ptr);

	/* Cursed/Worthless items "disappear" when sold */
	if (value <= 0) return (-1);

	/* Erase the inscription */
	o_ptr->obj_note = 0;

	/* Remove special inscription, if any */
	if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;


	/* Check each existing object (try to combine) */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get the existing object */
		j_ptr = &st_ptr->stock[slot];

		/* Can the existing items be incremented? */
		if (store_object_similar(j_ptr, o_ptr))
		{
			/* Absorb (some of) the object */
			store_object_absorb(j_ptr, o_ptr);

			/* All done */
			return (slot);
		}
	}

	/* No space? */
	if (st_ptr->stock_num >= st_ptr->stock_size) return (-1);


	/* Check existing slots to see if we must "slide" */
	for (slot = 0; slot < st_ptr->stock_num; slot++)
	{
		/* Get that object */
		j_ptr = &st_ptr->stock[slot];

		/* Objects sort by decreasing type */
		if (o_ptr->tval > j_ptr->tval) break;
		if (o_ptr->tval < j_ptr->tval) continue;

		/* Objects sort by increasing sval */
		if (o_ptr->sval < j_ptr->sval) break;
		if (o_ptr->sval > j_ptr->sval) continue;

		/* Evaluate that slot */
		j_value = object_value(j_ptr);

		/* Objects sort by decreasing value */
		if (value > j_value) break;
		if (value < j_value) continue;
	}

	/* Slide the others up */
	for (i = st_ptr->stock_num; i > slot; i--)
	{
		/* Hack -- slide the objects */
		object_copy(&st_ptr->stock[i], &st_ptr->stock[i-1]);
	}

	/* More stuff now */
	st_ptr->stock_num++;

	/* Hack -- Insert the new object */
	object_copy(&st_ptr->stock[slot], o_ptr);

	/* Return the location */
	return (slot);
}


/*
 * Increase, by a given amount, the number of a certain item
 * in a certain store.  This can result in zero items.
 */
static void store_item_increase(int item, int num)
{
	int cnt;
	object_type *o_ptr;

	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Verify the number */
	cnt = o_ptr->number + num;
	if (cnt > 255) cnt = 255;
	else if (cnt < 0) cnt = 0;
	num = cnt - o_ptr->number;

	/* Save the new number */
	o_ptr->number += num;
}


/*
 * Remove a slot if it is empty
 */
static void store_item_optimize(int item)
{
	int j;
	object_type *o_ptr;

	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Must exist */
	if (!o_ptr->k_idx) return;

	/* Must have no items */
	if (o_ptr->number) return;

	/* One less object */
	st_ptr->stock_num--;

	/* Slide everyone */
	for (j = item; j < st_ptr->stock_num; j++)
	{
		st_ptr->stock[j] = st_ptr->stock[j + 1];
	}

	/* Nuke the final slot */
	object_wipe(&st_ptr->stock[j]);
}


/*
 * This function will keep 'crap' out of the black market.
 * Crap is defined as any object that is "available" elsewhere
 * Based on a suggestion by "Lee Vogt" <lvogt@cig.mcel.mot.com>
 */
static bool black_market_crap(const object_type *o_ptr)
{
	int i, j;

	/* Ego items are never crap */
	if (o_ptr->ego_num) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check the other stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		/* Skip home and black market, and guild */
		if ((i == STORE_B_MARKET) || (i == STORE_HOME) || (i == STORE_GUILD))
		  continue;

		/* Check every object in the store */
		for (j = 0; j < store[i].stock_num; j++)
		{
			object_type *j_ptr = &store[i].stock[j];

			/* Duplicate object "type", assume crappy */
			if (o_ptr->k_idx == j_ptr->k_idx) return (TRUE);
		}
	}

	/* Assume okay */
	return (FALSE);
}

/* Keep certain objects (undiscounted only*/
static bool keep_in_stock(const object_type *o_ptr, int which)
{

	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/*Discounted items, ego items, or artifacts don't stay in stock*/
	if (o_ptr->discount) return (FALSE);
	if (o_ptr->art_num) return (FALSE);
	if (o_ptr->ego_num) return (FALSE);

	/* Analyze the item type */
	switch (k_ptr->tval)
	{
		/* Certain kinds of food is sold there*/
		case TV_FOOD:
		{
			/*only keep in the general store*/
			if (which != STORE_GENERAL) return (FALSE);
			if (k_ptr->sval == SV_FOOD_RATION) return (TRUE);
			return (FALSE);
		}

		/* Non artifact Lite Sources should be kept */
		case TV_LITE:
		{
			/*only keep in the general store*/
			if (which != STORE_GENERAL) return (FALSE);
			if (k_ptr->sval == SV_LITE_TORCH &&
			    o_ptr->timeout > 0) return (TRUE);
			return (FALSE);
		}
		case TV_SHOT:
		case TV_ARROW:
		case TV_BOLT:
		{
			/*only keep in the general store or weaponsmith*/
			if ((which != STORE_GENERAL) && (which != STORE_WEAPON)) return (FALSE);
			if (k_ptr->sval == SV_AMMO_NORMAL)
			{
				/*only normal ammo that isn't enchanted*/
				if ((o_ptr->to_h > 0) || (o_ptr->to_d > 0)) return FALSE;
				return (TRUE);
			}
			return (FALSE);
		}
		case TV_POTION:
		{
			/*only keep in the temple*/
			if (which != STORE_TEMPLE) return (FALSE);
			if (k_ptr->sval == SV_POTION_CURE_CRITICAL) return (TRUE);
			if (k_ptr->sval == SV_POTION_RESTORE_EXP) return (TRUE);
			if (k_ptr->sval == SV_POTION_HEROISM) return (TRUE);
			return (FALSE);
		}
		case TV_SCROLL:
		{
			/*only keep in the alchemy shop*/
			if (which != STORE_ALCHEMY) return (FALSE);
			if (k_ptr->sval == SV_SCROLL_PHASE_DOOR) return (TRUE);
			if (k_ptr->sval == SV_SCROLL_SATISFY_HUNGER) return (TRUE);
			if (k_ptr->sval == SV_SCROLL_IDENTIFY) return (TRUE);
			if (k_ptr->sval == SV_SCROLL_HOLY_CHANT) return (TRUE);
			return (FALSE);

		}
		/* Flasks should be kept */
		case TV_FLASK:
		{
			return (TRUE);
		}
		case TV_PRAYER_BOOK:
		case TV_MAGIC_BOOK:
		case TV_DRUID_BOOK:
		{
			if (k_ptr->sval < SV_BOOK_MIN_GOOD) return (TRUE);
			return (FALSE);
		}

	}
	return (FALSE);
}


/*
 * Attempt to delete (some of) a random object from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(int which)
{
	int what, num;

	object_type *o_ptr;

	/* Paranoia */
	if (st_ptr->stock_num <= 0) return;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Get the object */
	o_ptr = &st_ptr->stock[what];

	/* Determine how many objects are in the slot */
	num = o_ptr->number;

	/* Some stores keep large amounts of certain objects in stock objects*/
	if ((store_num != STORE_B_MARKET) && (keep_in_stock(o_ptr, which)))
	{
		if (o_ptr->number > 60) num = num / 2;
		else return;
	}

	/* Hack -- sometimes, only destroy half the objects */
	else if (one_in_(2)) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single object */
	else if (one_in_(2)) num = 1;

	/*Wipe the randart if necessary*/
	if (o_ptr->art_num) artifact_wipe(o_ptr->art_num, FALSE);

	/* Actually destroy (part of) the object */
	store_item_increase(what, -num);
	store_item_optimize(what);
}

/*
 * Creates a random object and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 *
 */
static void store_create(void)
{
	int tries, k_idx;

	object_type *i_ptr;
	object_type object_type_body;

	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size) return;

	/* Hack -- consider up to ten items */
	for (tries = 0; tries < 10; tries++)
	{

		s16b magic_level;

		/* Pick a level for object/creation */
		magic_level = 15 + (p_ptr->lev / 2) + rand_int(p_ptr->lev);

		/* Get local object */
		i_ptr = &object_type_body;

		/*wipe the object*/
		object_wipe(i_ptr);

		/*
		 * Get the object level.  The object level of 100 is a hack
		 * to ensure that all items that are part of the regular store
		 * inventory are not out of depth.
		 */
		if ((store_num == STORE_B_MARKET) || (allow_altered_inventory))
		{
		 	object_level = magic_level;
		}
		else object_level = 100;

		/* Pick a random object */
		k_idx = get_obj_num(object_level);

		/* Handle failure - but this should never happen*/
		if (!k_idx) continue;

		/* Prepare the object */
		object_prep(i_ptr, k_idx);

		/* Apply magic (dis-allow artifacts) */
		apply_magic(i_ptr, magic_level, FALSE, FALSE, FALSE, FALSE);

		/* Hack -- Charge lite's */
		if (i_ptr->tval == TV_LITE)
		{
			if (i_ptr->sval == SV_LITE_TORCH) i_ptr->timeout = FUEL_TORCH / 2;
			if (i_ptr->sval == SV_LITE_LANTERN) i_ptr->timeout = FUEL_LAMP / 2;
		}

		/* The object is "known" */
		object_known(i_ptr);

		/* Item belongs to a store */
		i_ptr->ident |= IDENT_STORE;

		/* Remember history */
		object_history(i_ptr, ORIGIN_STORE, 0);

		/* Prune the black market */
		if (store_num == STORE_B_MARKET)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(i_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(i_ptr) < 10) continue;
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(i_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(i_ptr);

		/* Attempt to carry the (known) object */
		(void)store_carry(i_ptr);

		/* Definitely done */
		break;
	}

	/* Reset the object level */
	object_level = danger(p_ptr->depth);

}



/*
 * Eliminate need to bargain if player has haggled well in the past
 */
static bool noneedtobargain(s32b minprice)
{
	s32b good = st_ptr->good_buy;
	s32b bad = st_ptr->bad_buy;

	/* Cheap items are "boring" */
	if (minprice < 10L) return (TRUE);

	/* Perfect haggling */
	if (good == MAX_SHORT) return (TRUE);

	/* Reward good haggles, punish bad haggles, notice price */
	if (good > ((3 * bad) + (5 + (minprice/50)))) return (TRUE);

	/* Return the flag */
	return (FALSE);
}


/*
 * Update the bargain info
 */
static void updatebargain(s32b price, s32b minprice)
{
	/* Hack -- auto-haggle */
	if (auto_haggle) return;

	/* Cheap items are "boring" */
	if (minprice < 10L) return;

	/* Count the successful haggles */
	if (price == minprice)
	{
		/* Just count the good haggles */
		if (st_ptr->good_buy < MAX_SHORT)
		{
			st_ptr->good_buy++;
		}
	}

	/* Count the failed haggles */
	else
	{
		/* Just count the bad haggles */
		if (st_ptr->bad_buy < MAX_SHORT)
		{
			st_ptr->bad_buy++;
		}
	}
}



/*
 * Redisplay a single store entry
 */
static void display_entry(int item)
{
	int y;
	object_type *o_ptr;
	s32b x;

	char o_name[80];
	char out_val[160];
	int maxwid;


	/* Must be on current "page" to get displayed */
	if (!((item >= store_top) && (item < store_top + MAX_ENTRIES))) return;

	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Get the row */
	y = (item % MAX_ENTRIES) + 6;

	/* Label it, clear the line --(-- */
	sprintf(out_val, "%c) ", store_to_label(item));
	prt(out_val, y, 0);

	/* Describe an object in the home */
	if (store_num == STORE_HOME)
	{
		int wgt;

		byte attr;

		maxwid = 65;

		/* Describe the object */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		attr = tval_to_attr[o_ptr->tval & 0x7F];

		/* Display the object */
		c_put_str(attr, o_name, y, 3);

		/* Only show the weight of a single object */
		wgt = o_ptr->weight;
		sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
		put_str(out_val, y, 68);
	}

	/* Describe an object (fully) in a store */
	else
	{
		byte attr;
		int wgt;

		/* Must leave room for the "price" */
		maxwid = 58;

		/* Describe the object (fully) */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		attr = tval_to_attr[o_ptr->tval & 0x7F];

		/* Display the object */
		c_put_str(attr, o_name, y, 3);

		/* Only show the weight of a single object */
		wgt = o_ptr->weight;
		sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
		put_str(out_val, y, 61);

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			sprintf(out_val, "%9ld F", (long)x);
		}

		/* Display a "taxed" cost */
		else if (auto_haggle)
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			if (!noneedtobargain(x)) x += x / 10;

			/* Actually draw the price (with tax) */
			if (((o_ptr->tval == TV_WAND)|| (o_ptr->tval == TV_TALISMAN))
			    && (o_ptr->number > 1)) sprintf(out_val, "%9ld avg", (long)x);
			else sprintf(out_val, "%9ld  ", (long)x);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			sprintf(out_val, "%9ld  ", (long)x);

		}

		/* Print the price, gray if player can't afford it */
		if ((long)x > (long)p_ptr->au)
		{
			c_put_str(TERM_SLATE, out_val, y, 68);
		}
		else
		{
			put_str(out_val, y, 68);
		}
	}
}


/*
 * Display a store's inventory
 *
 * All prices are listed as "per individual object"
 */
static void display_inventory(void)
{
	int i, k;

	/* Display the next group of items */
	for (k = 0; k < MAX_ENTRIES; k++)
	{
		/* Stop when we run out of items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < (MAX_ENTRIES + 1); i++) prt("", i + 6, 0);

	/* Assume "no current page" */
	put_str("        ", 5, 20);

	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > MAX_ENTRIES)
	{
		/* Show "more" reminder (after the last object ) */
		prt("-more-", k + 6, 3);

		/* Indicate the "current page" */
		put_str(format("(Page %d)", store_top/MAX_ENTRIES + 1), 5, 20);
	}
}


/*
 * Display players gold
 */
static void store_prt_gold(void)
{
	char out_val[64];
	int row = TERM_LAST_ROW - 4;

	prt("Gold Remaining: ", row, 53);

	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, row, 68);
}


/*
 * Calculate the service price.  The guild has no race preferences, so we
 * use the player fame instead.
 */
static s32b get_service_price(int choice)
{

	/* get the service price*/
	u32b price = service_price[choice];

	/*adjust price, but not for the guild*/
	if (store_num != STORE_GUILD)
	{
		/* Compute the racial factor */
		int factor = g_info[(ot_ptr->owner_race * z_info->p_max) + p_ptr->prace];

		/* Extract the "minimum" price */
		price = ((price * factor) / 100L);
	}

	/*Guild price factoring*/
	else
	{
		if (p_ptr->fame < 100) price += price * (100 - p_ptr->fame) / 100;
	}

	return(price);
}


/*
 * Display and handle store inventory (after clearing screen)
 */
static void display_store_services(void)
{
	char buf[80];

	s16b max_service = 0;

	bool show_footnote = FALSE;

	s16b i;
	s16b services[STORE_SERVICE_MAX];
	s32b price;
	byte row = 7;
	byte col_name = 3;
	byte col_price = 65;

	char store_name[80];
	cptr owner_name = &(b_name[ot_ptr->owner_name]);
	cptr race_name = p_name + p_info[ot_ptr->owner_race].name;

	/* Get the name of the store */
	feature_desc(store_name, sizeof(store_name),
		cave_feat[p_ptr->py][p_ptr->px], FALSE, FALSE);

	/* Clear screen */
	Term_clear();

	/*The guild doesn't have a rotating shopkeeper*/
	if (store_num != STORE_GUILD)
	{
		/* Put the owner name and race */
		strnfmt(buf, sizeof(buf), "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);
	}

	/* Show the store name */
	strnfmt(buf, sizeof(buf), "%s", store_name);
	prt(buf, 3, 50);
	put_str("Price", 5, col_price + 6);

	/*get the store services*/
	for (i = 0; i < STORE_SERVICE_MAX; i++)
	{
		if (service_store[i] == store_num) services[max_service++] = i;
	}

	/* Label the object descriptions */
	put_str("Service", 5, 3);

	/* Display the current gold */
	store_prt_gold();

	/*
	 * Paranoia - this should have been checked before
	 * ever being allowed to enter this function
	 */
	if (!max_service)
	{
		c_put_str(TERM_RED, "This store does not offer any services.", 5, 3);

		return;
	}

	/*
	 * "Terminate" the list of services, unless 1 store has all the services
	 */
	if (max_service < STORE_SERVICE_MAX) services[max_service] = -1;

	/*dump the list of services and prices*/
	for (i = 0; i < STORE_SERVICE_MAX; i++)
	{

		char s_name[80];

		int name_len;

		/*we are at the end of the services*/
		if (services[i] == -1) break;

		my_strcpy(s_name, service_names[services[i]], sizeof(s_name));

		name_len = strlen(s_name);

		/* Acquire the "name" of the service */
	    strnfmt(buf, sizeof(buf), "%c) %s",  store_to_label(i), s_name);

		if (streq(&(s_name[name_len - 3]), "[v]")) show_footnote = TRUE;

		/*Print it*/
		put_str(buf, row + i, col_name);

		/* get the service price*/
		price = get_service_price(services[i]);

		/* Actually draw the price*/
		my_strcpy(buf, format("%11ld ", (long)price), sizeof(buf));
		put_str(buf, row + i, col_price);

	}

	/*Print the price footnote if necessary*/
	if (show_footnote == TRUE)
	{
		my_strcpy(buf, "[v] - price varies depending on the item(s) selected", sizeof(buf));

		put_str(buf, row + max_service + 1, col_name);
	}

}



/*
 * Display store inventory (after clearing screen)
 */
static void display_store_inventory(void)
{
	char buf[80];

	/* Clear screen */
	clear_from(0);

	/* The "Home" is special */
	if (store_num == STORE_HOME)
	{
		/* Put the owner name */
		put_str("Your Home", 3, 30);

		/* Label the object descriptions */
		put_str("Item Description", 5, 3);

		/* show label */
		put_str("Weight", 5, 70);
	}

	/* Normal stores */
	else
	{
		char store_name[80];
		cptr race_name = p_name + p_info[ot_ptr->owner_race].name;
		cptr owner_name = &(b_name[ot_ptr->owner_name]);

		/* Get the name of the store */
		feature_desc(store_name, sizeof(store_name),
			cave_feat[p_ptr->py][p_ptr->px], FALSE, FALSE);

		/* Put the owner name and race */
		strnfmt(buf, sizeof(buf), "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		strnfmt(buf, sizeof(buf), "%s (%ld)", store_name, (long)(ot_ptr->max_cost));
		prt(buf, 3, 50);

		/* Label the object descriptions */
		put_str("Item Description", 5, 3);

		/* show label */
		put_str("Weight", 5, 60);

		/* Label the asking price (in stores) */
		put_str("Price", 5, 72);
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}

/*
 * Display store inventory (after clearing screen)
 */
static void display_store(void)
{

	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (store_num == STORE_HOME)
	{
		display_store_inventory();
	}

	/* Normal stores */
	else
	{
		/*there is one function to handle services*/
		if (show_services) display_store_services();

		/* The "guild has a separate menu from other stores */
		else if (store_num == STORE_GUILD) display_guild();

		else display_store_inventory();
	}

}



/*
 * Get the index of a store object
 *
 * Return TRUE if an object was selected
 */
static bool get_stock(int *com_val, cptr pmt)
{
	int item;

	char which;

	char buf[160];

	char o_name[80];

	char out_val[160];

	object_type *o_ptr;


	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= 0) && (*com_val <= (st_ptr->stock_num - 1)))
		{
			/* Success */
			return (TRUE);
		}
		else
		{
			/* Invalid repeat - reset it */
			repeat_clear();
		}
	}


	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	strnfmt(buf, sizeof(buf), "(Items %c-%c, ESC to exit) %s",
	        store_to_label(0), store_to_label(st_ptr->stock_num - 1),
	        pmt);

	/* Ask until done */
	while (TRUE)
	{
		bool verify;

		/* Escape */
		if (!get_com(buf, &which)) return (FALSE);

		/* Note verify */
		verify = (isupper((unsigned char)which) ? TRUE : FALSE);

		/* Lowercase */
		which = tolower((unsigned char)which);

		/* Convert response to item */
		item = label_to_store(which);

		/* Oops */
		if (item < 0)
		{
			/* Oops */
			bell("Illegal store object choice!");

			continue;
		}

		/* No verification */
		if (!verify) break;

		/* Object */
		o_ptr = &st_ptr->stock[item];

		/* Describe */
		object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

		/* Prompt */
		strnfmt(out_val, sizeof(out_val), "Try %s? ", o_name);

		/* Query */
		if (!get_check(out_val)) return (FALSE);

		/* Done */
		break;
	}

	/* Save item */
	(*com_val) = item;



	repeat_push(*com_val);



	/* Success */
	return (TRUE);
}


/*
 * Increase the insult counter and get angry if necessary
 */
static int increase_insults(void)
{
	/* Increase insults */
	st_ptr->insult_cur++;

	/* Become insulted */
	if (st_ptr->insult_cur > ot_ptr->insult_max)
	{
		/* Complain */
		say_comment_4();

		/* Reset insults */
		st_ptr->insult_cur = 0;
		st_ptr->good_buy = 0;
		st_ptr->bad_buy = 0;

		/* Open tomorrow */
		st_ptr->store_open = turn + 25000 + randint(25000);

		/* Closed */
		return (TRUE);
	}

	/* Not closed */
	return (FALSE);
}


/*
 * Decrease the insult counter
 */
static void decrease_insults(void)
{
	/* Decrease insults */
	if (st_ptr->insult_cur) st_ptr->insult_cur--;
}


/*
 * The shop-keeper has been insulted
 */
static int haggle_insults(void)
{
	/* Increase insults */
	if (increase_insults()) return (TRUE);

	/* Display and flush insult */
	say_comment_5();

	/* Still okay */
	return (FALSE);
}


/*
 * Mega-Hack -- Enable "increments"
 */
static bool allow_inc = FALSE;

/*
 * Mega-Hack -- Last "increment" during haggling
 */
static s32b last_inc = 0L;


/*
 * Get a haggle
 */
static int get_haggle(cptr pmt, s32b *poffer, s32b price, int final)
{
	char buf[128];


	/* Clear old increment if necessary */
	if (!allow_inc) last_inc = 0L;


	/* Final offer */
	if (final)
	{
		strnfmt(buf, sizeof(buf), "%s [accept] ", pmt);
	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
		strnfmt(buf, sizeof(buf), "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
		strnfmt(buf, sizeof(buf), "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Normal haggle */
	else
	{
		strnfmt(buf, sizeof(buf), "%s ", pmt);
	}


	/* Paranoia XXX XXX XXX */
	message_flush();


	/* Ask until done */
	while (TRUE)
	{
		cptr p;

		char out_val[80];

		/* Default */
		strcpy(out_val, "");

		/* Ask the user for a response */
		if (!get_string(buf, out_val, sizeof(out_val))) return (FALSE);

		/* Skip leading spaces */
		for (p = out_val; *p == ' '; p++) /* loop */;

		/* Empty response */
		if (*p == '\0')
		{
			/* Accept current price */
			if (final)
			{
				*poffer = price;
				last_inc = 0L;
				break;
			}

			/* Use previous increment */
			if (allow_inc && last_inc)
			{
				*poffer += last_inc;
				break;
			}
		}

		/* Normal response */
		else
		{
			s32b i;
			char *err_ptr;

			/* Extract a number */
			i = strtol(p, &err_ptr, 10);

			/* Handle "incremental" number */
			if ((*p == '+' || *p == '-'))
			{
				/* Allow increments */
				if (allow_inc)
				{
					/* Use the given "increment" */
					*poffer += i;
					last_inc = i;
					break;
				}
			}

			/* Handle normal number */
			else
			{
				/* Use the given "number" */
				*poffer = i;
				last_inc = 0L;
				break;
			}
		}

		/* Warning */
		msg_print("Invalid response.");
		message_flush();
	}

	/* Success */
	return (TRUE);
}


/*
 * Receive an offer (from the player)
 *
 * Return TRUE if offer is NOT okay
 */
static bool receive_offer(cptr pmt, s32b *poffer,
                          s32b last_offer, int factor,
                          s32b price, int final)
{
	/* Haggle till done */
	while (TRUE)
	{
		/* Get a haggle (or cancel) */
		if (!get_haggle(pmt, poffer, price, final)) return (TRUE);

		/* Acceptable offer */
		if (((*poffer) * factor) >= (last_offer * factor)) break;

		/* Insult, and check for kicked out */
		if (haggle_insults()) return (TRUE);

		/* Reject offer (correctly) */
		(*poffer) = last_offer;
	}

	/* Success */
	return (FALSE);
}


/*
 * Haggling routine XXX XXX
 *
 * Return TRUE if purchase is NOT successful
 */
static bool purchase_haggle(object_type *o_ptr, s32b *price)
{
	s32b cur_ask, final_ask;
	s32b last_offer, offer;
	s32b x1, x2, x3;
	s32b min_per, max_per;
	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;

	bool ignore = FALSE;

	bool cancel = FALSE;

	cptr pmt = "Asking";

	char out_val[160];


	*price = 0;


	/* Extract the starting offer and final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, FALSE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* No need to haggle */
	if (auto_haggle || noneed || (o_ptr->ident & (IDENT_FIXED)))
	{
		/* Already haggled */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Message summary */
			msg_print("You instantly agree upon the price.");
			message_flush();
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message summary */
			msg_print("You eventually agree upon the price.");
			message_flush();
		}

		/* Auto-haggle */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			message_flush();

			/* Ignore haggling */
			ignore = TRUE;

			/* Apply sales tax */
			final_ask += (final_ask / 10);
		}

		/* Jump to final price */
		cur_ask = final_ask;

		/* Go to final offer */
		pmt = "Final Offer";
		final = TRUE;
	}


	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Haggle parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * (200 - (int)(ot_ptr->max_inflate)) / 100L;
	if (last_offer <= 0) last_offer = 1;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle until done */
	for (flag = FALSE; !flag; )
	{
		loop_flag = TRUE;

		while (!flag && loop_flag)
		{
			strnfmt(out_val, sizeof(out_val), "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
			cancel = receive_offer("What do you offer? ",
			                       &offer, last_offer, 1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer > cur_ask)
			{
				say_comment_6();
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}
		}

		if (!flag)
		{
			x1 = 100 * (offer - last_offer) / (cur_ask - last_offer);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((cur_ask - offer) * x2 / 100L) + 1;
			/* don't let the price go up */
			if (x3 < 0) x3 = 0;
			cur_ask -= x3;

			/* Too little */
			if (cur_ask < final_ask)
			{
				final = TRUE;
				cur_ask = final_ask;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					(void)(increase_insults());
					cancel = TRUE;
					flag = TRUE;
				}
			}
			else if (offer >= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				sprintf(out_val, "Your last offer: %ld",
				              (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_2(cur_ask, annoyed);
			}
		}
	}

	/* Mark price as fixed */
	if (!ignore && (*price == final_ask))
	{
		/* Mark as fixed price */
		o_ptr->ident |= (IDENT_FIXED);
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update haggling info */
	if (!ignore)
	{
		/* Update haggling info */
		updatebargain(*price, final_ask);
	}

	/* Do not cancel */
	return (FALSE);
}


/*
 * Haggling routine XXX XXX
 *
 * Return TRUE if purchase is NOT successful
 */
static bool sell_haggle(object_type *o_ptr, s32b *price)
{
	s32b purse, cur_ask, final_ask;
	s32b last_offer, offer = 0;
	s32b x1, x2, x3;
	s32b min_per, max_per;

	int flag, loop_flag, noneed;
	int annoyed = 0, final = FALSE;

	bool ignore = FALSE;

	bool cancel = FALSE;

	cptr pmt = "Offer";

	char out_val[160];


	*price = 0;


	/* Obtain the starting offer and the final offer */
	cur_ask = price_item(o_ptr, ot_ptr->max_inflate, TRUE);
	final_ask = price_item(o_ptr, ot_ptr->min_inflate, TRUE);

	/* Determine if haggling is necessary */
	noneed = noneedtobargain(final_ask);

	/* Get the owner's payout limit */
	purse = (s32b)(ot_ptr->max_cost);

	/* No need to haggle */
	if (noneed || auto_haggle || (final_ask >= purse))
	{
		/* Apply Sales Tax (if needed) */
		if (auto_haggle && !noneed)
		{
			final_ask -= final_ask / 10;
		}

		/* No reason to haggle */
		if (final_ask >= purse)
		{
			/* Message */
			msg_print("You instantly agree upon the price.");
			message_flush();

			/* Ignore haggling */
			ignore = TRUE;

			/* Offer full purse */
			final_ask = purse;
		}

		/* No need to haggle */
		else if (noneed)
		{
			/* Message */
			msg_print("You eventually agree upon the price.");
			message_flush();
		}

		/* No haggle option */
		else
		{
			/* Message summary */
			msg_print("You quickly agree upon the price.");
			message_flush();

			/* Ignore haggling */
			ignore = TRUE;
		}

		/* Final price */
		cur_ask = final_ask;

		/* Final offer */
		final = TRUE;
		pmt = "Final Offer";
	}

	/* Haggle for the whole pile */
	cur_ask *= o_ptr->number;
	final_ask *= o_ptr->number;


	/* Display commands XXX XXX XXX */

	/* Haggling parameters */
	min_per = ot_ptr->haggle_per;
	max_per = min_per * 3;

	/* Mega-Hack -- artificial "last offer" value */
	last_offer = object_value(o_ptr) * o_ptr->number;
	last_offer = last_offer * ot_ptr->max_inflate / 100L;

	/* No offer yet */
	offer = 0;

	/* No incremental haggling yet */
	allow_inc = FALSE;

	/* Haggle */
	for (flag = FALSE; !flag; )
	{
		while (1)
		{
			loop_flag = TRUE;

			strnfmt(out_val, sizeof(out_val), "%s :  %ld", pmt, (long)cur_ask);
			put_str(out_val, 1, 0);
			cancel = receive_offer("What price do you ask? ",
			                       &offer, last_offer, -1, cur_ask, final);

			if (cancel)
			{
				flag = TRUE;
			}
			else if (offer < cur_ask)
			{
				say_comment_6();
				/* rejected, reset offer for incremental haggling */
				offer = last_offer;
			}
			else if (offer == cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}
			else
			{
				loop_flag = FALSE;
			}

			/* Stop */
			if (flag || !loop_flag) break;
		}

		if (!flag)
		{
			x1 = 100 * (last_offer - offer) / (last_offer - cur_ask);
			if (x1 < min_per)
			{
				if (haggle_insults())
				{
					flag = TRUE;
					cancel = TRUE;
				}
			}
			else if (x1 > max_per)
			{
				x1 = x1 * 3 / 4;
				if (x1 < max_per) x1 = max_per;
			}
			x2 = rand_range(x1-2, x1+2);
			x3 = ((offer - cur_ask) * x2 / 100L) + 1;
			/* don't let the price go down */
			if (x3 < 0) x3 = 0;
			cur_ask += x3;

			if (cur_ask > final_ask)
			{
				cur_ask = final_ask;
				final = TRUE;
				pmt = "Final Offer";
				annoyed++;
				if (annoyed > 3)
				{
					flag = TRUE;
					(void)(increase_insults());
				}
			}
			else if (offer <= cur_ask)
			{
				flag = TRUE;
				*price = offer;
			}

			if (!flag)
			{
				last_offer = offer;
				allow_inc = TRUE;
				prt("", 1, 0);
				sprintf(out_val,
				              "Your last bid %ld", (long)last_offer);
				put_str(out_val, 1, 39);
				say_comment_3(cur_ask, annoyed);
			}
		}
	}

	/* Cancel */
	if (cancel) return (TRUE);

	/* Update haggling info */
	if (!ignore)
	{
		/* Update haggling info */
		updatebargain(*price, final_ask);
	}

	/* Do not cancel */
	return (FALSE);
}


static bool check_gold(s32b price)
{
	if (price > p_ptr->au)
	{
		msg_format("It would cost you %d gold.  You don't have enough.", price);
		return (TRUE);
	}

	return (FALSE);
}


/*
 * The "randart" tester
 */
bool item_tester_hook_randart(const object_type *o_ptr)
{
	/*Hack - don't allow cursed items*/
	if(o_ptr->ident & (IDENT_CURSED)) return (FALSE);

	/*Hack - don't allow broken items*/
	if(o_ptr->ident & (IDENT_BROKEN)) return (FALSE);

	/*Hack - don't allow unidentified items*/
	if(!(o_ptr->ident & (IDENT_KNOWN))) return (FALSE);

	if (can_be_randart(o_ptr))
	{

		/*We don't use current artifacts or ego-items, unless dragon armor*/
		if ((o_ptr->tval != TV_DRAG_ARMOR) && (o_ptr->tval != TV_DRAG_SHIELD))
		{
			if ((o_ptr->art_num) || (o_ptr->ego_num)) return(FALSE);
		}

		/*don't make artifacts out of stacks of items*/
		if (o_ptr->number > 1) return (FALSE);

		/*eligible to be a randart*/
		return (TRUE);
	}

	/* Assume cannot be a randart */
	return (FALSE);
}

/*
 * The flammable book tester
 */
bool item_tester_hook_flammable_book(const object_type *o_ptr)
{
	u32b f1, f2, f3, fn;

	if 	((o_ptr->tval != TV_PRAYER_BOOK) && (o_ptr->tval != TV_DRUID_BOOK) &&
	 	 (o_ptr->tval != TV_MAGIC_BOOK)) return (FALSE);

	/* Get the "known" flags */
	object_flags(o_ptr, &f1, &f2, &f3, &fn);

	/*already flammable*/
	if (f3 & TR3_IGNORE_FIRE) return (FALSE);

	/* Immune to lava, so they should resist fire. */
	if (fn & ELEMENT_LAVA) return (FALSE);

	/* Flammable spellbook */
	return (TRUE);
}



#define DISPLAY_STAT_ROW		8
#define DISPLAY_STAT_COL		10

/*
 * Process the chosen service from a store
 */
static void store_service_aux(s16b choice)
{
	object_type *o_ptr;
	object_kind *k_ptr;

	char o_name[80];

	byte lev;

	cptr q, s;

	int item, i;

	char prompt[160];

	u32b price = get_service_price(choice);

	switch (choice)
	{
		case SERVICE_ENCHANT_ARMOR:
		case SERVICE_ENCHANT_TO_HIT:
		case SERVICE_ENCHANT_TO_DAM:
		{
			s16b add_to;
			s16b counter = 1;

			u32b f1, f2, f3, fn;

			/* Enchant armor if requested */
			if (choice == SERVICE_ENCHANT_ARMOR)
			{
				item_tester_hook = item_tester_hook_ided_armour;
			}
			else item_tester_hook = item_tester_hook_ided_weapon;

			/* Get an item */
			q = "Enchant which item? ";
			s = "You have nothing to enchant.";
			if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

			/*Got the item*/
			o_ptr = &inventory[item];
			k_ptr = &k_info[o_ptr->k_idx];

			/* Extract the flags */
			object_flags(o_ptr, &f1, &f2, &f3, &fn);

			if (choice == SERVICE_ENCHANT_ARMOR) add_to = o_ptr->to_a;
			else if (choice == SERVICE_ENCHANT_TO_HIT) add_to = o_ptr->to_h;
			/* to-damage*/
			else add_to = o_ptr->to_d;

			/* Description, shorten it for artifacts */
			if (o_ptr->art_num) object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 0);
			else object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 1);

			/*
			 * We will eventually run into the u32 variable max number, so
			 * cut off the + allowed
			 */
			if (add_to >= 15)
			{
				msg_format("%s %s cannot be enchanted any further",
	           ((item >= 0) ? "Your" : "The"), o_name);

			 	break;
			}

			/* Missiles are easier to enchant */
			if ((o_ptr->tval == TV_BOLT) ||
	    		(o_ptr->tval == TV_ARROW) ||
	    		(o_ptr->tval == TV_SHOT))
			{
				price = price / 20;
			}

			/* Greater to-hit and to-dam makes things more expensive*/
			while (add_to >= counter)
			{
				price += (price * 8) / 10;

				counter ++;
			}

			/*multiply for quantity*/
			price *= o_ptr->number;

			/*artifacts are double*/
			if (o_ptr->art_num) price *= 2;

			/*Too expensive*/
			if (check_gold(price)) break;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to enchant %s? ",
							price, o_name);
			if (!get_check(prompt)) break;

			/*reduce the gold*/
			p_ptr->au -= price;

			/* Description */
			object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

			/* Describe */
			msg_format("%s %s glow%s brightly!",
	           ((item >= 0) ? "Your" : "The"), o_name,
	           ((o_ptr->number > 1) ? "" : "s"));

			if (choice == SERVICE_ENCHANT_ARMOR) o_ptr->to_a ++;
			else if (choice == SERVICE_ENCHANT_TO_HIT) o_ptr->to_h ++;
			/* to-damage*/
			else o_ptr->to_d++;

			/* Break curse */
			if (cursed_p(o_ptr) &&
				(!(k_ptr->k_flags3 & (TR3_PERMA_CURSE))) &&
				 (add_to >= 0) && (rand_int(100) < 25))
			{
				msg_print("The curse is broken!");

				/* Uncurse the object */
				uncurse_object(o_ptr);
			}

			break;
		}

		case SERVICE_ELEM_BRAND_WEAP:
		case SERVICE_ELEM_BRAND_AMMO:
		{
			byte brand_type;

			/* Enchant weapon if requested */
			if (choice == SERVICE_ELEM_BRAND_WEAP)
			{
				item_tester_hook = item_tester_hook_wieldable_ided_weapon;
			}
			/*Ammo*/
			else item_tester_hook = item_tester_hook_ided_ammo;

			/* Get an item */
			q = "Brand which item? ";
			s = "You have nothing to Brand.";
			if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) return;

			/*Got the item*/
			o_ptr = &inventory[item];

			/* Description, shorten it for artifacts */
			if (o_ptr->art_num) object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 0);
			else object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 1);

			/*If artifact, or ego item, don't bother*/
			if ((o_ptr->art_num) || (o_ptr->ego_num))
			{
				msg_format("%^s cannot be branded!", o_name);

				break;
			}

			/* Missiles are easier to enchant */
			if ((o_ptr->tval == TV_BOLT) ||
	    		(o_ptr->tval == TV_ARROW) ||
	    		(o_ptr->tval == TV_SHOT))
			{
				price = price / 20;
			}

			/*multiply for quantity*/
			price *= o_ptr->number;

			/*Too expensive*/
			if (check_gold(price)) break;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to brand %s? ",
							price, o_name);
			if (!get_check(prompt)) break;

			if (choice == SERVICE_ELEM_BRAND_WEAP)
			{
				if (one_in_(2)) brand_type = BRAND_OFFSET_FLAME;
				else brand_type = BRAND_OFFSET_FROST;

			}
			/*ammo*/
			else
			{
				/* Select the brand */
				if (one_in_(3))
					brand_type = EGO_AMMO_FLAME;
				else if (one_in_(2))
					brand_type = EGO_AMMO_FROST;
				else brand_type = EGO_AMMO_VENOM;
			}

			/*Brand*/
			if (brand_object(o_ptr, brand_type, FALSE)) p_ptr->au -= price;

			break;
		}

		case SERVICE_RECHARGING:
		{

			/* Only accept legal items, which are wands and staffs */
			item_tester_hook = item_tester_hook_recharge;

			/* Get an item */
			q = "Recharge which item? ";
			s = "You have nothing to recharge.";
			if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) break;

			/*Got the item*/
			o_ptr = &inventory[item];

			/* Description */
			object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 1);

			/* Extract the object "level" */
			lev = k_info[o_ptr->k_idx].k_level;

			/*base price on level*/
			price += price * (lev / 2);

			/*get the price for rods*/
			if (o_ptr->tval == TV_ROD)
			{
				if (!o_ptr->timeout)
				{
					/* Describe */
					msg_format("The %s %s not require re-charging!",
	          			o_name, (o_ptr->number > 1 ? "do" : "does"));

					break;
				}
				else
				{
					price += (price * o_ptr->timeout) / 20;
				}
			}

			/*Wands, and Staffs*/
			else
			{
				price += o_ptr->pval * price;

				/*Bigger charage for a stack of staffs or wands*/
				if (o_ptr->number > 1) price += (o_ptr->number - 1) * price;
			}

			/*Too expensive*/
			if (check_gold(price)) break;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to recharge %s?  ",
							price, o_name);
			if (!get_check(prompt)) break;


			/*We re-charged an item*/
			p_ptr->au -= price;

			break;
		}
		case SERVICE_IDENTIFY:
		{
			/*Too expensive*/
			if (check_gold(price)) return;

			/*We identified an item*/
			if (ident_spell()) p_ptr->au -= price;

			break;
		}
		case SERVICE_IDENTIFY_FULLY:
		{
			/*Too expensive*/
			if (check_gold(price)) return;

			/*We identified an item*/
			if (identify_fully()) p_ptr->au -= price;

			break;
		}
		case SERVICE_CURE_CRITICAL:
		{
			bool healed = FALSE;

			/*Too expensive*/
			if (check_gold(price)) return;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to cure critical wounds? ",
							price);
			if (!get_check(prompt)) break;

			/*Heal the player, note if they actually need healing*/
			if (hp_player(damroll(12, 12))) healed = TRUE;
			if (set_blind(0)) healed = TRUE;
			if (set_confused(0)) healed = TRUE;
			if (set_poisoned(0)) healed = TRUE;
			if (set_stun(0)) healed = TRUE;
			if (set_cut(0)) healed = TRUE;

			/*We identified an item*/
			if (healed) p_ptr->au -= price;
			else msg_format("You do not require any healing services.");

			break;
		}
		case SERVICE_RESTORE_LIFE_LEVELS:
		{
			/*Too expensive*/
			if (check_gold(price)) return;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to restore life levels? ",
							price);
			if (!get_check(prompt)) break;

			/*We restored the player*/
			if (restore_level()) p_ptr->au -= price;
			/* Not needed*/
			else msg_format("Your life levels do not require restoring.");

			break;
		}
		case SERVICE_REMOVE_CURSE:
		case SERVICE_REMOVE_HEAVY_CURSE:
		{
			/*Too expensive*/
			if (check_gold(price)) return;

			/*We removed a curse an item, charge the player*/
			if (remove_curse(choice == SERVICE_REMOVE_HEAVY_CURSE ? TRUE : FALSE))
			{
				p_ptr->au -= price;
			}

			else msg_format("No items had a curse removed.");

			break;
		}
		case SERVICE_RESTORE_STAT:
		case SERVICE_INCREASE_STAT:
		{

			bool stats_healthy = TRUE;
			bool stats_maxed = TRUE;

			/*Too expensive*/
			if (check_gold(price)) return;

			/* Check the stats for need */
			for (i = 0; i < A_MAX; i++)
			{

				/* Display "injured" stat */
				if (p_ptr->stat_cur[i] < p_ptr->stat_max[i])
				{
					stats_healthy = FALSE;

				}

				/* Mark natural maximum */
				if (p_ptr->stat_max[i] < 18+100)
				{
					stats_maxed = FALSE;

				}

				/*print out the letter (lines up with the next command to display stat info)*/
				c_put_str(TERM_WHITE, format("%c) ",  store_to_label(i)), 10 + i, 30);

			}

			/* All Modes Use Stat info */
			display_player_stat_info(10, 33);

			if ((choice == SERVICE_RESTORE_STAT) && (stats_healthy))
			{
				msg_format("None of your stats need restoring.");

				break;
			}
			else if ((choice == SERVICE_INCREASE_STAT) && (stats_maxed))
			{
				msg_format("Your stats cannot be increased any further.");

				break;
			}

			if  (choice == SERVICE_RESTORE_STAT)
			{
				/* Copy the string over */
				my_strcpy(prompt, "Which stat do you wish to restore? (ESC to cancel):",
					sizeof(prompt));
			}
			/*must be SERVICE_INCREASE_STAT*/
			else
			{
				/* Copy the string over */
				my_strcpy(prompt, "Which stat do you wish to increase? (ESC to cancel):",
					sizeof(prompt));
			}


			/* Get the object number to be bought */
			i = get_menu_choice(A_MAX, prompt);

			/*player chose escape - do nothing */
			if (i == -1) break;

			/*restore the stat*/
			if (choice == SERVICE_RESTORE_STAT)
			{
				/*charge it*/
				if (do_res_stat(i)) p_ptr->au -= price;
				else msg_format("Your %s does not need restoring.",
										stat_names_full[i]);

			}
			/*must be SERVICE_INCREASE_STAT*/
			else
			{
				if (do_inc_stat(i)) p_ptr->au -= price;
				else msg_format("Your %s cannot be increased any further.",
									stat_names_full[i]);
			}

			break;
		}

		case SERVICE_CREATE_RANDART:
		{
			s32b o_value;

			if ((adult_no_artifacts) || (adult_no_xtra_artifacts))
			{
				msg_print("Nothing happens.");
				break;
			}

			/* Only accept legal items */
			item_tester_hook = item_tester_hook_randart;

			/* Get an item */
			q = "Choose an item to be made into an artifact. ";
			s = "You have no eligible item.";
			if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN))) break;

			/*Got the item*/
			o_ptr = &inventory[item];

			/*Got the object kind*/
			k_ptr = &k_info[o_ptr->k_idx];

			/* Description */
			object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 1);

			/* Get the "value" of the item */
			o_value = k_ptr->cost * 50;

			/*Get the price for the item*/
			price = price + o_value;

			/*Too expensive*/
			if (check_gold(price)) break;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to make %s into an artifact? ",
							price, o_name);
			if (!get_check(prompt)) break;

			/*the line above keeps showing up again -JG*/
			Term_erase(0, 0, 255);

			/*re-use the o_value variable for a completely different purpose*/
			/*extra power bonus for expensive items and high player fame*/
			o_value = p_ptr->fame / 2 + MAX((k_ptr->cost / 2000), p_ptr->fame / 5);

		   	/*Hack - add in any to-hit and to-value, since they will be erased*/
			o_value += (o_ptr->to_h + o_ptr->to_d + o_ptr->to_a / 2);

			/*actually create the Randart, or handle failure*/
			if (make_one_randart(o_ptr, o_value, TRUE))
			{
				p_ptr->au -= price;

				/* Identify it fully */
				object_aware(o_ptr);
				object_known(o_ptr);

				/* Mark the item as fully known */
				o_ptr->ident |= (IDENT_MENTAL);

				/*Let the player know what they just got*/
				object_info_screen(o_ptr);
			}
			else msg_print("The attempt at making an artifact has failed");
			break;
		}

		case SERVICE_PROBE_QUEST_MON:
		{
			char race_name[80];

			quest_type *q_ptr = &q_info[GUILD_QUEST_SLOT];
			monster_race *r_ptr = &r_info[q_ptr->mon_idx];
			monster_lore *l_ptr = &l_list[q_ptr->mon_idx];

			if (((q_ptr->type != QUEST_MONSTER) &&
			     (q_ptr->type != QUEST_UNIQUE)) ||
				(q_ptr->mon_idx == 0))
			{
				msg_print("You are not currently questing for a specific creature.");
				break;
			}

			/* Not a vault quest, so get the monster race name (singular)*/
			monster_desc_race(race_name, sizeof(race_name), q_ptr->mon_idx);

			/* Make it plural if necessary*/
			if (q_ptr->max_num > 1) plural_aux(race_name, sizeof(race_name));

			price += r_ptr->level * 100;

			/*Too expensive*/
			if (check_gold(price)) return;

			/*confirm*/
			strnfmt(prompt, sizeof(prompt), "Spend %d gold to probe %s? ",
							price, race_name);
			if (!get_check(prompt)) break;

			/*charge the player*/
			p_ptr->au -= price;

			/*learn something about the monster*/
			lore_probe_monster_aux(q_ptr->mon_idx);

			/* Hack -- Increse the sightings, and ranged attacks around 50% of the time */
			l_ptr->sights = MAX_SHORT;
			l_ptr->ranged = MAX_UCHAR;

			/* Know "race" flags */
			l_ptr->r_l_flags3 |= (r_ptr->flags3 & RF3_RACE_MASK);
			/* Know "forced" flags */
			l_ptr->r_l_flags1 |= (r_ptr->flags1 & (RF1_FORCE_DEPTH | RF1_FORCE_MAXHP));

			/* Save screen */
			screen_save();

			/* Begin recall */
			Term_gotoxy(0, 1);

			/* Output to the screen */
			text_out_hook = text_out_to_screen;

			/* Recall monster */
			describe_monster(q_ptr->mon_idx, FALSE);

			/* Describe monster */
			roff_top(q_ptr->mon_idx);

			/*give the player a look at the updated monster info*/
			put_str("Press any key to continue.  ", 0, 40);

			inkey();

			/* Load screen */
			screen_load();
			break;
		}
		case SERVICE_BUY_HEALING_POTION:
		case SERVICE_BUY_LIFE_POTION:
		{
			char o_name[80];
			int k_idx;

			object_type *i_ptr;
			object_type object_type_body;

			i_ptr = &object_type_body;

			/*Too expensive*/
			if (check_gold(price)) break;

			/*get the book index*/
			if (choice == SERVICE_BUY_HEALING_POTION)
			{
				k_idx = lookup_kind(TV_POTION, SV_POTION_HEALING);
			}
			else k_idx = lookup_kind(TV_POTION, SV_POTION_LIFE);

			/*get the book kind*/
			k_ptr = &k_info[k_idx];

			/*Too expensive*/
			if (check_gold(price)) break;

			strnfmt(prompt, sizeof(prompt), "Spend %d gold to purchase a potion of %s? ",
							price, (k_name + k_ptr->name));
			if (!get_check(prompt)) break;

			/*charge the player*/
			p_ptr->au -= price;

			/* Make the potion */
			object_prep(i_ptr, k_idx);

			/* Identify it */
			k_info[k_idx].aware = TRUE;

			/* Describe the result */
			object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

			/* Remember history */
			object_history(i_ptr, ORIGIN_STORE, 0);

			/* Note that the pack is too full */
			if (!inven_carry_okay(i_ptr))
			{
				msg_format("You have no room in your backpack.");

				/* Drop the object */
				drop_near(i_ptr, -1, p_ptr->py, p_ptr->px);

				/* Inform the player */
				msg_format("Your %s is waiting outside!", o_name);

			}

			/* Give it to the player */
			else
			{
				int item_new;

				/* Give it to the player */
				item_new = inven_carry(i_ptr);

				/* Describe just the result */
				object_desc(o_name, sizeof(o_name), &inventory[item_new], TRUE, 3);

				/* Message */
				msg_format("You have (%c) %s.", index_to_label(item_new), o_name);
			}

			message_flush();
			break;
		}
		case SERVICE_ABANDON_QUEST:
		{
			char title[40];

			/*Get the current title*/
			get_title(title, sizeof(title));

			/* Check for current quest */
			if (!p_ptr->cur_quest)
			{
		    	msg_format("You don't have a current quest, %s.", title);
				message_flush();
		    	break;
			}

			/* Ask confirmation */
			if (!get_check(format("Abandon your quest, %s?", title))) break;

			/* Remove the current quest */
			quest_fail();

			/*Get the new title, and give a message*/
			get_title(title, sizeof(title));
			msg_format("The guild is disappointed in you, %s.", title);
			message_flush();

			/* Apply a punishment if fame is low enough*/
			if (p_ptr->fame < 5) store[STORE_GUILD].store_open = turn + 10000 + randint(10000);

			break;
		}
		case SERVICE_FIREPROOF_BOOK:
		{

		  	int i;

		  	/*Too expensive*/
		  	if (check_gold(price)) break;

			/* Restrict choices to spell books */
			item_tester_tval = cp_ptr->spell_book;

			/* Only accept legal items, which are burnable books */
			item_tester_hook = item_tester_hook_flammable_book;

			/* Get an item */
			q = "Fireproof which book? ";
			if (cp_ptr->spell_book == TV_PRAYER_BOOK) s = "You have no flammable prayer books!";
			else s = "You have no flammable spell books!";
			if (!get_item(&item, q, s, (USE_INVEN | USE_FLOOR))) return;

		  	/*Got the item*/
			o_ptr = &inventory[item];
			k_ptr = &k_info[o_ptr->k_idx];

			/*Adjust the price for the book and the nubmer of books*/
		  	price += (k_ptr->cost * 10);
			price *= o_ptr->number;

		  	/*Too expensive*/
		  	if (check_gold(price)) break;

			/* Description */
			object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

		  	/*confirm*/
		  	strnfmt(prompt, sizeof(prompt),
		      	"Spend %d gold to fireproof %s? ", 	price, o_name);
		  	if (!get_check(prompt)) break;

			/*find the ego-item*/
			for (i = 0; i < z_info->e_max; i++)
			{
				ego_item_type *e_ptr = &e_info[i];

				if (strstr((e_name + e_ptr->name), "Fireproof"))
				{
					int j;
					bool right_type = FALSE;

					/*Confirm the right tval*/
					for (j = 0; j < EGO_TVALS_MAX; j++)
					{
						if (e_ptr->tval[j] == cp_ptr->spell_book) right_type = TRUE;
					}

					/*We found it*/
					if (right_type)
					{
						/*charge the player*/
		  				p_ptr->au -= price;

						o_ptr->ego_num = i;

						/* Description */
						object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

						/*Confirm it worked*/
						msg_format("You have %s", o_name);

						break;
					}
				}
			}

 		  	break;
 		}
	}
}


/*
 * Buy a service from a store
 */
static void store_service_purchase(void)
{

	int choice;

	s16b max_service = 0;

	s16b i;

	s16b services[STORE_SERVICE_MAX];

	char prompt[80];

	/*get the store services*/
	for (i = 0; i < STORE_SERVICE_MAX; i++)
	{
		if (service_store[i] == store_num) services[max_service++] = i;
	}

	/*
	 * "Terminate" the list of services, unless 1 store has all the services
	 */
	if (max_service < STORE_SERVICE_MAX) services[max_service] = -1;

	/* Copy the string over */
	my_strcpy(prompt, "Which service do you wish to purchase (ESC to cancel):",
					sizeof(prompt));

	/* Get the object number to be bought */
	choice = get_menu_choice(max_service, prompt);

	/*player chose excape*/
	if ((choice == -1) || (choice >= max_service)) return;

	/*give the player the service*/
	store_service_aux(services[choice]);

	/*flush the output before re-drawing*/
	message_flush();

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

	/* Handle stuff */
	handle_stuff();

	/* Redraw everything, in case Charisma changes */
	display_store();

	/* Done */
	return;
}



/*
 * Buy an object from a store
 */
static void store_purchase(void)
{
	int n;
	int amt, choice;
	int item, item_new;

	s32b price;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[120];

	char out_val[160];

	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (store_num == STORE_HOME)
		{
			msg_print("Your home is empty.");
		}
		else
		{
			msg_print("I am currently out of stock.");
		}
		return;
	}

	/* Prompt */
	if (store_num == STORE_HOME)
	{
		sprintf(out_val, "Which item do you want to take? ");
	}
	else
	{
		sprintf(out_val, "Which item are you interested in? ");
	}

	/* Get the object number to be bought */
	if (!get_stock(&item, out_val)) return;

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Get a quantity */
	if ((auto_haggle && o_ptr->number > 1) && (store_num != STORE_HOME))
	{
		/* Get price */
		long x;
		x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);
		/* Hack -- Apply Sales Tax if needed */
		if (!noneedtobargain(x)) x += x / 10;

		amt = (long)p_ptr->au / x;

		if (amt > o_ptr->number) amt = o_ptr->number;

		/* Can't afford it */
		if (amt == 0)
		{
			msg_print("You cannot afford to buy this.");
			return;
		}
	}
	else
	{
		amt = o_ptr->number;
	}

	/* Get a quantity */
	amt = get_quantity(NULL, amt);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Get desired object */
	object_copy(i_ptr, o_ptr);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Hack -- require room in pack */
	if (!inven_carry_okay(i_ptr))
	{
		msg_print("You cannot carry that many items.");
		return;
	}

	/* Attempt to buy it */
	if (store_num != STORE_HOME)
	{
		/* Describe the object (fully) */
		object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

		/* Message */
		msg_format("Buying %s (%c).",
		           o_name, store_to_label(item));
		message_flush();

		/* Haggle for a final price */
		choice = purchase_haggle(i_ptr, &price);

		/* Hack -- Got kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Hack -- Maintain fixed prices */
		if (i_ptr->ident & (IDENT_FIXED))
		{
			/* Mark as fixed price */
			o_ptr->ident |= (IDENT_FIXED);
		}

		/* Player wants it */
		if (choice == 0)
		{
			/* Player can afford it */
			if (p_ptr->au >= price)
			{
				bool aware = FALSE;

				/* Say "okay" */
				say_comment_1();

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Remember awareness */
				if (object_aware_p(i_ptr)) aware = TRUE;

				/* Buying an object makes you aware of it */
				object_aware(i_ptr);

				/* Combine / Reorder the pack (later) */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				/* Clear the "fixed" flag from the object */
				i_ptr->ident &= ~(IDENT_FIXED);

				/* The object no longer belongs to the store */
				i_ptr->ident &= ~(IDENT_STORE);

				/* Describe the transaction */
				object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

				/* Message */
				msg_format("You bought %s (%c) for %ld gold.",
				           o_name, store_to_label(item),
				           (long)price);

				/* Erase the inscription */
				i_ptr->obj_note = 0;

				/* Remove special inscription, if any */
				if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

				/* Give it to the player */
				item_new = inven_carry(i_ptr);

				/* Hack -- Apply autoinscriptions if we become aware of the object */
				if (!aware) apply_autoinscription(&inventory[item_new]);

				/* Describe the final result */
				object_desc(o_name, sizeof(o_name), &inventory[item_new], TRUE, 3);

				/* Message */
				msg_format("You have %s (%c).",
				           o_name, index_to_label(item_new));

				/* Handle stuff */
				handle_stuff();

				/* Note how many slots the store used to have */
				n = st_ptr->stock_num;

				/* Remove the bought objects from the store */
				store_item_increase(item, -amt);
				store_item_optimize(item);

				/* Store is empty */
				if (st_ptr->stock_num == 0)
				{
					int i;

					/* Shuffle */
					if (one_in_(STORE_SHUFFLE))
					{
						/* Message */
						msg_print("The shopkeeper retires.");

						/* Shuffle the store */
						store_shuffle(store_num);
					}

					/* Maintain */
					else
					{
						/* Message */
						msg_print("The shopkeeper brings out some new stock.");
					}

					/* New inventory */
					for (i = 0; i < 10; ++i)
					{
						/* Maintain the store */
						store_maint(store_num);
					}

					/* Start over */
					store_top = 0;
				}

				/* The object is gone */
				else if (st_ptr->stock_num != n)
				{
					/* Only one screen left */
					if (st_ptr->stock_num <= MAX_ENTRIES)
					{
						store_top = 0;
					}
				}

				/* Redraw everything */
				display_inventory();
			}

			/* Player cannot afford it */
			else
			{
				/* Simple message (no insult) */
				msg_print("You do not have enough gold.");
			}
		}
	}

	/* Home is much easier */
	else
	{

		/* Give it to the player */
		item_new = inven_carry(i_ptr);

		/* Describe just the result */
		object_desc(o_name, sizeof(o_name), &inventory[item_new], TRUE, 3);

		/* Message */
		msg_format("You have %s (%c).", o_name, index_to_label(item_new));

		/* Handle stuff */
		handle_stuff();

		/* Take note if we take the last one */
		n = st_ptr->stock_num;

		/* Remove the items from the home */
		store_item_increase(item, -amt);
		store_item_optimize(item);

		/* The object is gone */
		if (st_ptr->stock_num != n)
		{
			/* Only one screen left */
			if (st_ptr->stock_num <= MAX_ENTRIES)
			{
				store_top = 0;
			}

			/* Redraw everything */
			display_inventory();
		}

		/* The object is still here */
		else
		{
			/* Redraw the object */
			display_entry(item);
		}
	}

	/* Not kicked out */
	return;
}


/*
 * Sell an object to the store (or home)
 */
static void store_sell(void)
{
	int choice;
	int item, item_pos;
	int amt;

	s32b price, value, dummy;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	cptr q, s;

	char o_name[120];


	/* Home */
	q = "Drop which item? ";

	/* Real store */
	if (store_num != STORE_HOME)
	{
		/* New prompt */
		q = "Sell which item? ";

		/* Only allow items the store will buy */
		item_tester_hook = store_will_buy_tester;
	}

	/* Get an item */
	s = "You have nothing that I want.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return;

	/* Get the item (in the pack) */
	if (item >= 0)
	{
		o_ptr = &inventory[item];
	}

	/* Get the item (on the floor) */
	else
	{
		o_ptr = &o_list[0 - item];
	}


	/* Hack -- Cannot remove cursed objects */
	if ((item >= INVEN_WIELD) && cursed_p(o_ptr))
	{
		/* Oops */
		msg_print("Hmmm, it seems to be cursed.");

		/* Nope */
		return;
	}

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Get a copy of the object */
	object_copy(i_ptr, o_ptr);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Get a full description */
	object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

	/* Is there room in the store (or the home?) */
	if (!store_check_num(i_ptr))
	{
		if (store_num == STORE_HOME)
		{
			msg_print("Your home is full.");
		}
		else
		{
			msg_print("I have not the room in my store to keep it.");
		}
		return;
	}

	/* Real store */
	if (store_num != STORE_HOME)
	{
		/* Describe the transaction */
		msg_format("Selling %s (%c).", o_name, index_to_label(item));
		message_flush();

		/* Haggle for it */
		choice = sell_haggle(i_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Sold... */
		if (choice == 0)
		{
			/*Do we squelch the item upon identification*/
			int squelch = SQUELCH_NO;
			bool aware = FALSE;

			/* Say "okay" */
			say_comment_1();

			/* Be happy */
			decrease_insults();

			/* Get some money */
			p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(i_ptr) * i_ptr->number;

			/* Erase the inscription */
			i_ptr->obj_note = 0;

			/* Remove special inscription, if any */
			if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

			/* Remember if we were aware of this object */
			if (object_aware_p(o_ptr)) aware = TRUE;

			/* Identify original object */
			object_aware(o_ptr);
			object_known(o_ptr);

			/* Squelch it?  Important!!!  Only if there will be items left over! */
			if (amt < o_ptr->number)
			{
				squelch = squelch_itemp(o_ptr, 0, TRUE);

				/* Apply an autoiscription if the object survived */
				if ((squelch == SQUELCH_NO) && !aware) apply_autoinscription(o_ptr);
			}

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

			p_ptr->update |= (PU_NATIVE);

			p_ptr->redraw |= (PR_EQUIPPY | PR_RESIST | PR_EXP | PR_STATS);

			/* Get local object */
			i_ptr = &object_type_body;

			/* Get a copy of the object */
			object_copy(i_ptr, o_ptr);

			/* Modify quantity */
			i_ptr->number = amt;

			/* The object belongs to the store now */
			i_ptr->ident |= IDENT_STORE;

			/* Get the "actual" value */
			value = object_value(i_ptr) * i_ptr->number;

			/* Get the "actual" value */
			value = object_value(i_ptr) * i_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

			/* Describe the result (in message buffer) */
			msg_format("You sold %s (%c) for %ld gold.",
			           o_name, index_to_label(item), (long)price);

			/* Analyze the prices (and comment verbally) */
			purchase_analyze(price, value, dummy);

			/*
			 * Check to see if anything left in the pack should be squelched.
			 * We must make sure to do this before the item is sold
			 */
			if (squelch == SQUELCH_YES)
			{
				msg_format("In your pack: %s (%c).  %s",
          					 o_name, index_to_label(item),
		   					squelch_to_label(squelch));

    			do_squelch_item(squelch, item, o_ptr);
    		}
			else
			{
				/* Take the object from the player */
				inven_item_increase(item, -amt);
				inven_item_describe(item);
				inven_item_optimize(item);
			}

			/* Handle stuff */
			handle_stuff();

			/* The store gets that (known) object */
			item_pos = store_carry(i_ptr);

			/* Update the display */
			if (item_pos >= 0)
			{
				/* Redisplay wares */
				display_inventory();
			}
		}
	}

	/* Player is at home */
	else
	{

		/* Describe */
		msg_format("You drop %s (%c).", o_name, index_to_label(item));

		/* Take it from the players inventory */
		inven_item_increase(item, -amt);
		inven_item_describe(item);
		inven_item_optimize(item);

		/* Handle stuff */
		handle_stuff();

		/* Let the home carry it */
		item_pos = home_carry(i_ptr);

		/* Update store display */
		if (item_pos >= 0)
		{
			/* Redisplay wares */
			display_inventory();
		}
	}
}


/*
 * Examine an item in a store
 */
static void store_examine(void)
{
	int         item;
	object_type *o_ptr;
	char        out_val[160];


	/* Empty? */
	if (st_ptr->stock_num <= 0)
	{
		if (store_num == STORE_HOME)
		{
			msg_print("Your home is empty.");
		}
		else
		{
			msg_print("I am currently out of stock.");
		}
		return;
	}


	/* Prompt */
	if (rogue_like_commands)
		sprintf(out_val, "Which item do you want to examine? ");
	else
		sprintf(out_val, "Which item do you want to look at? ");

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val)) return;

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Describe it fully */
	object_info_screen(o_ptr);
}



/*
 * Hack -- set this to leave the store
 */
static bool leave_store = FALSE;


/*
 * Process a command in a store
 *
 * Note that we must allow the use of a few "special" commands
 * in the stores which are not allowed in the dungeon, and we
 * must disable some commands which are allowed in the dungeon
 * but not in the stores, to prevent chaos.
 *
 * Hack -- note the bizarre code to handle the "=" command,
 * which is needed to prevent the "redraw" from affecting
 * the display of the store.  XXX XXX XXX
 */
static void store_process_command(bool guild_cmd)
{
	bool legal;



	/* Handle repeating the last command */
	repeat_check();



	legal = TRUE;

	/* Parse the command */
	switch (p_ptr->command_cmd)
	{
		/* Leave */
		case ESCAPE:
		{
			leave_store = TRUE;
			break;
		}

		/* Browse */
		case ' ':
		{
			if (guild_cmd)
			{
				legal = FALSE;
				break;
			}

			/*don't do anything if we are in service mode*/
			if (show_services) break;

			if (st_ptr->stock_num <= MAX_ENTRIES)
			{
				/* Nothing to see */
				msg_print("Entire inventory is shown.");
			}

			else if (store_top == 0)
			{
				/* Page 2 */
				store_top = MAX_ENTRIES;

				/* Redisplay wares */
				display_inventory();
			}

			else
			{
				/* Page 1 */
				store_top = 0;

				/* Redisplay wares */
				display_inventory();
			}

			break;
		}

		/* Ignore */
		case '\n':
		case '\r':
		{
			break;
		}


		/* Redraw */
		case KTRL('R'):
		{
			if ((!guild_cmd) || (show_services))
			{
				display_store();
			}
			else display_guild();

			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			if (show_services) store_service_purchase();
			else if (!guild_cmd) store_purchase();
			else guild_purchase();
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			/*don't do anything if we are in service mode*/
			if (show_services) break;
			if (!guild_cmd) store_sell();
			else legal = FALSE;
			break;
		}

		/* Examine */
		case 'l':
		{
			/*don't do anything if we are in service mode*/
			if (show_services) break;

			if (!guild_cmd) store_examine();
			else legal = FALSE;
			break;
		}
		/* Toggle between services and inventory */
		case 'V':
		case 'v':
		{
			if (show_services) show_services = FALSE;
			else if (store_has_services)
			{
				show_services = TRUE;
			}

			do_cmd_redraw();
			if ((!guild_cmd) || (show_services)) display_store();
			else display_guild();

			break;
		}

		/*** Inventory Commands ***/

		/* Wear/wield equipment */
		case 'w':
		{
			do_cmd_wield();
			break;
		}

		/* Take off equipment */
		case 't':
		{
			do_cmd_takeoff();
			break;
		}

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy();
			break;
		}

		/* Equipment list */
		case 'e':
		{
			do_cmd_equip();
			break;
		}

		/* Inventory list */
		case 'i':
		{
			do_cmd_inven();
			break;
		}


		/*** Various commands ***/

		/* Identify an object */
		case 'I':
		{
			do_cmd_observe();
			break;
		}

		/* Hack -- toggle windows */
		case KTRL('E'):
		{
			toggle_inven_equip();
			break;
		}

		/*** Use various objects ***/

		/* Browse a book */
		case 'b':
		{
			do_cmd_browse();
			break;
		}

		/* Inscribe an object */
		case '{':
		{
			do_cmd_inscribe();
			break;
		}

		/* Uninscribe an object */
		case '}':
		{
			do_cmd_uninscribe();
			break;
		}



		/*** Help and Such ***/

		/* Help */
		case '?':
		{
			do_cmd_help();
			break;
		}

		/* Identify symbol */
		case '/':
		{
			do_cmd_query_symbol();
			break;
		}

		/* Character description */
		case 'C':
		{
			do_cmd_change_name();
			break;
		}


		/*** System Commands ***/

		/* Hack -- User interface */
		case '!':
		{
			(void)Term_user(0);
			break;
		}

		/* Single line from a pref file */
		case '"':
		{
			do_cmd_pref();
			break;
		}

		/* Interact with macros */
		case '@':
		{
			do_cmd_macros();
			break;
		}

		/* Interact with visuals */
		case '%':
		{
			do_cmd_visuals();
			break;
		}

		/* Interact with colors */
		case '&':
		{
			do_cmd_colors();
			break;
		}

		/* Interact with options */
		case '=':
		{
			do_cmd_options();
			do_cmd_redraw();
			display_store();
			break;
		}


		/*** Misc Commands ***/

		/* Take notes */
		case ':':
		{
			do_cmd_note("",  p_ptr->depth);
			break;
		}

		/* Show previous message */
		case KTRL('O'):
		{
			do_cmd_message_one();
			break;
		}

		/* Show previous messages */
		case KTRL('P'):
		{
			do_cmd_messages();
			break;
		}

		/* Check knowledge */
		case '~':
		case '|':
		{
			do_cmd_knowledge();
			break;
		}

		/* Load "screen dump" */
		case '(':
		{
			do_cmd_load_screen();
			break;
		}

		/* Save "screen dump" */
		case ')':
		{
			do_cmd_save_screen();
			break;
		}


		/* Hack -- Unknown command */
		default:
		{
			legal = FALSE;
			break;
		}
	}

	if (!legal)
	{
		if (!guild_cmd) msg_print("That command does not work in stores.");
		else msg_print("That command does not work in the adventurer's guild.");
	}
}


/*
 * Enter a store, and interact with it.
 *
 * Note that we use the standard "request_command()" function
 * to get a command, allowing us to use "p_ptr->command_arg" and all
 * command macros and other nifty stuff, but we use the special
 * "shopping" argument, to force certain commands to be converted
 * into other commands, normally, we convert "p" (pray) and "m"
 * (cast magic) into "g" (get), and "s" (search) into "d" (drop).
 */
void do_cmd_store(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int which, i;

	show_services = FALSE;
	store_has_services = FALSE;

	/* Verify a store */
	if (!(cave_shop_bold(py,px)))
	{
		msg_print("You see no store here.");
		return;
	}

	/* Hack -- Extract the store */
	which = (f_info[cave_feat[py][px]].f_power);

	/* Hack -- Check the "locked doors" */
	if (adult_no_stores || store[which].store_open >= turn)
	{
		msg_print("The doors are locked.");
		return;
	}

	/* Forget the view */
	forget_view();

	/* Hack -- Increase "icky" depth */
	character_icky++;

	/* No command argument */
	p_ptr->command_arg = 0;

	/* No repeated command */
	p_ptr->command_rep = 0;

	/* No automatic command */
	p_ptr->command_new = 0;


	/* Save the store number */
	store_num = which;


	/*find out of the store sells services*/
	/*But, remember the game option for no store services*/
	if (!adult_no_store_services) for (i = 0; i < STORE_SERVICE_MAX; i++)
	{
		if (service_store[i] == store_num) store_has_services = TRUE;
	}

	/* Save the store and owner pointers */
	st_ptr = &store[store_num];
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];

	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Do not leave */
	leave_store = FALSE;

	/*
	 * Play a special sound if entering the
	 * home, otherwise play a generic store sound.
	 */
	if (store_num == STORE_HOME)
		sound(MSG_STORE_HOME);
	else
		sound(MSG_STORE_ENTER);

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Clear */
		 clear_from(TERM_LAST_ROW - 2);

		/* Basic commands */
		prt(" ESC) Exit from Building.", TERM_LAST_ROW - 1, 0);

		/* Browse if necessary */
		if ((!show_services) && (st_ptr->stock_num > MAX_ENTRIES))
		{
			prt(" SPACE) Next page of stock.", TERM_LAST_ROW, 0);
		}


		/* Commands */
		if (store_num != STORE_GUILD)
		{
			/*display inventory*/
			if (!show_services)
			{
				/* Commands */
				prt(" g) Get/Purchase an item.", TERM_LAST_ROW - 1, 29);
				prt(" d) Drop/Sell an item.", TERM_LAST_ROW, 29);

				/* Add in the eXamine option */
				if (rogue_like_commands)
					prt(" x) eXamine an item.", TERM_LAST_ROW, 56);
				else
					prt(" l) Look at an item.", TERM_LAST_ROW, 56);

				/*toggle to services*/
				if (store_has_services)
				{
					prt(" v) View store services.", TERM_LAST_ROW - 1, 56);
				}
			}
			/*display services*/
			else
			{
				/* Commands */
				prt(" g) Get/Purchase a service.", TERM_LAST_ROW - 1, 27);

				/*toggle to inventory*/
				prt(" v) View store inventory.", TERM_LAST_ROW - 1, 56);
			}
		}

		else
		{
			/*display inventory*/
			if (!show_services)
			{
				prt(" g) Get a quest.", TERM_LAST_ROW - 1, 29);
				if (store_has_services)
				{
					prt(" v) View store services.", TERM_LAST_ROW - 1, 56);
				}
			}
			else
			{
				prt(" g) Get/Purchase a service.", TERM_LAST_ROW - 1, 27);
				prt(" v) View quest status.", TERM_LAST_ROW - 1, 56);
			}

		}

		/* Prompt */
		prt("You may: ", TERM_LAST_ROW - 2, 0);

		/* Get a command */
		request_command(TRUE);

		if (store_num != STORE_GUILD) store_process_command(FALSE);
		else store_process_command(TRUE);

		/* Notice stuff */
		notice_stuff();

		/* Handle stuff */
		handle_stuff();

		/* Pack Overflow XXX XXX XXX */
		if (inventory[INVEN_PACK].k_idx)
		{
			int item = INVEN_PACK;

			object_type *o_ptr = &inventory[item];

			/* Hack -- Flee from the store */
			if (store_num != STORE_HOME)
			{
				/* Message */
				msg_print("Your pack is so full that you flee the store...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Flee from the home */
			else if (!store_check_num(o_ptr))
			{
				/* Message */
				msg_print("Your pack is so full that you flee your home...");

				/* Leave */
				leave_store = TRUE;
			}

			/* Hack -- Drop items into the home */
			else
			{
				int item_pos;

				object_type *i_ptr;
				object_type object_type_body;

				char o_name[80];


				/* Give a message */
				msg_print("Your pack overflows!");

				/* Get local object */
				i_ptr = &object_type_body;

				/* Grab a copy of the object */
				object_copy(i_ptr, o_ptr);

				/* Describe it */
				object_desc(o_name, sizeof(o_name), i_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

				/* Let the home carry it */
				item_pos = home_carry(i_ptr);

				/* Redraw the home */
				if (item_pos >= 0)
				{
					/* Redisplay wares */
					display_inventory();
				}
			}
		}

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}

	/* Leave the store sound */
	sound(MSG_STORE_LEAVE);

	/* Take a turn */
	p_ptr->p_energy_use = BASE_ENERGY_MOVE;

	/* Hack -- Cancel automatic command */
	p_ptr->command_new = 0;

	/* Hack -- Cancel "see" mode */
	p_ptr->command_see = FALSE;


	/* Flush messages XXX XXX XXX */
	message_flush();

	/* Hack -- Decrease "icky" depth */
	character_icky--;

	/* Clear the screen */
	Term_clear();

	/* Update the visuals */
	p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw entire screen */
	p_ptr->redraw |= (PR_BASIC | PR_EXTRA);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_MAP);
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;

	/* Ignore home & guild*/
	if ((which == STORE_HOME) || (which == STORE_GUILD)) return;

	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Pick a new owner */
	for (j = st_ptr->owner; j == st_ptr->owner; )
	{
		st_ptr->owner = (byte)rand_int(z_info->b_max);
	}

	/* Activate the new owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Reset the owner data */
	st_ptr->insult_cur = 0;
	st_ptr->store_open = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;


	/* Discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &st_ptr->stock[i];

		/*don't discount the essential stock items*/
		if (keep_in_stock(o_ptr, which)) continue;

		/* Discount non-discounted items by 40 percent */
		if (o_ptr->discount == 0) o_ptr->discount = 40;

		/* Clear the "fixed price" flag */
		o_ptr->ident &= ~(IDENT_FIXED);
	}
}


/*
 * Maintain the inventory at the stores.
 */
void store_maint(int which)
{
	int j, x;
	int alt_min = 0;

	int old_rating = rating;

	/* Ignore home */
	if ((which == STORE_HOME) || (which == STORE_GUILD)) return;

	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Activate the owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];

	/* Store keeper forgives the player */
	st_ptr->insult_cur = 0;

	/* Mega-Hack -- prune the black market */
	if (store_num == STORE_B_MARKET)
	{
		/* Destroy crappy black market items */
		for (j = st_ptr->stock_num - 1; j >= 0; j--)
		{
			object_type *o_ptr = &st_ptr->stock[j];

			/* Destroy crappy items */
			if (black_market_crap(o_ptr))
			{
				/*Wipe a randart if necessary*/
				if (o_ptr->art_num) artifact_wipe(o_ptr->art_num, FALSE);

				/* Destroy the object */
				store_item_increase(j, 0 - o_ptr->number);
				store_item_optimize(j);
			}
		}
	}

	/* Choose the number of slots to keep */
	j = st_ptr->stock_num;

	/* Sell a few items */
	j = j - randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/*go through the store inventory and count how many items must be kept*/
	for (x = 0; x < st_ptr->stock_num; x++)
	{
		object_type *o_ptr = &st_ptr->stock[x];

		/* Nothing here */
		if (!o_ptr->k_idx) continue;

		if (keep_in_stock(o_ptr, which)) alt_min ++;

	}

	/*
	 * The while loop below will lock up game if j
	 * is less than the # of "must-keep" items in store
	 */
	if (j < alt_min) j = alt_min;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete(which);

	/* Choose the number of slots to fill */
	j = st_ptr->stock_num;

	/* Buy some more items */
	j = j + randint(STORE_TURNOVER);

	/* Never keep more than "STORE_MAX_KEEP" slots */
	if (j > STORE_MAX_KEEP) j = STORE_MAX_KEEP;

	/* Always "keep" at least "STORE_MIN_KEEP" items */
	if (j < STORE_MIN_KEEP) j = STORE_MIN_KEEP;

	/* Hack -- prevent "overflow" */
	if (j >= st_ptr->stock_size) j = st_ptr->stock_size - 1;

	/*
	 * Paranoia:
	 * This should never be true unless a new store isn't set up properly.
	 * Note this function sets the allocation table, which must be undone at the bottom.
	 */
	if (!prep_store_object(which)) return;

	/* Create some new items */
	while (st_ptr->stock_num < j) store_create();

	/* Clear restriction */
	get_obj_num_hook = NULL;

	/* Re-Set the allocation table */
	get_obj_num_prep();

	/* No longer in store create mode*/
	object_generation_mode = OB_GEN_MODE_NORMAL;

	/* Hack -- Restore the rating */
	rating = old_rating;
}


/*
 * Initialize the stores
 */
void store_init(int which)
{
	int k;


	/* Save the store index */
	store_num = which;

	/* Activate that store */
	st_ptr = &store[store_num];

	/* Pick an owner */
	st_ptr->owner = (byte)rand_int(z_info->b_max);

	/* Activate the new owner */
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];

	/* Initialize the store */
	st_ptr->store_open = 0;
	st_ptr->insult_cur = 0;
	st_ptr->good_buy = 0;
	st_ptr->bad_buy = 0;

	/* Nothing in stock */
	st_ptr->stock_num = 0;

	/* Clear any old items */
	for (k = 0; k < st_ptr->stock_size; k++)
	{
		object_wipe(&st_ptr->stock[k]);
	}
}

