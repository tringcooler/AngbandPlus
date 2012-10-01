/* File: spells1.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Helper function -- return a "nearby" race for polymorphing
 *
 * Note that this function is one of the more "dangerous" ones...
 *
 * "Power" is used to somewhat control the monster choice. High power
 * means greater likelihood of easier monsters.
 */
static s16b poly_r_idx(int r_idx, int power)
{
	monster_race *r_ptr = &r_info[r_idx];

	int i, r, lev1, lev2;

	/* Hack -- Uniques never polymorph */
	if (r_ptr->flags1 & (RF1_UNIQUE)) return (r_idx);

	/* Allowable range of "levels" for resulting monster */
	lev1 = r_ptr->level - ((randint(20)/randint(9))+1) - (power / 10);
	lev2 = r_ptr->level + ((randint(20)/randint(9))+1) - ((power - 3) / 7);

	/* Pick a (possibly new) non-unique race */
	for (i = 0; i < 1000; i++)
	{
		/* Pick a new race, using a level calculation */
		r = get_mon_num((p_ptr->depth + r_ptr->level) / 2 + 5);

		/* Handle failure */
		if (!r) break;

		/* Obtain race */
		r_ptr = &r_info[r];

		/* Ignore unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) continue;

		/* Ignore monsters with incompatible levels */
		if ((r_ptr->level < lev1) || (r_ptr->level > lev2)) continue;

		/* Use that index */
		r_idx = r;

		/* Done */
		break;
	}

	/* Result */
	return (r_idx);
}

/*
 * Teleport a monster, normally up to "dis" grids away.
 *
 * Attempt to move the monster at least "dis/2" grids away.
 *
 * But allow variation to prevent infinite loops.
 */
void teleport_away(int m_idx, int dis)
{
	int ny, nx, oy, ox, d, i, min;

	bool look = TRUE;

	monster_type *m_ptr = &m_list[m_idx];


	/* Paranoia */
	if (!m_ptr->r_idx) return;

	/* Save the old location */
	oy = m_ptr->fy;
	ox = m_ptr->fx;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while (look)
	{
		/* Verify max distance */
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (TRUE)
			{
				ny = rand_spread(oy, dis);
				nx = rand_spread(ox, dis);
				d = distance(oy, ox, ny, nx);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds_fully(ny, nx)) continue;

			/* Require "empty" floor space */
			if (!cave_empty_bold(ny, nx)) continue;

			/* Hack -- no teleport onto glyph of warding */
			if (cave_feat[ny][nx] == FEAT_GLYPH) continue;

			/* No teleporting into vaults and such */
			/* if (cave_info[ny][nx] & (CAVE_ICKY)) continue; */

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;
	}

	/* Sound */
	sound(MSG_TPOTHER);

	/* Swap the monsters */
	monster_swap(oy, ox, ny, nx);
}

/*
 * Teleport the player to a location up to "dis" grids away.
 *
 * If no such spaces are readily available, the distance may increase.
 * Try very hard to move the player at least a quarter that distance.
 */
void teleport_player(int dis)
{
	int d, i, min;

	int y = p_ptr->py;
	int x = p_ptr->px;

	bool look = TRUE;

	/* Minimum distance */
	min = dis / 2;

	/* Look until done */
	while (look)
	{
		/* Verify max distance */
		if (dis > 200) dis = 200;

		/* Try several locations */
		for (i = 0; i < 500; i++)
		{
			/* Pick a (possibly illegal) location */
			while (TRUE)
			{
				y = rand_spread(p_ptr->py, dis);
				x = rand_spread(p_ptr->px, dis);
				d = distance(p_ptr->py, p_ptr->px, y, x);
				if ((d >= min) && (d <= dis)) break;
			}

			/* Ignore illegal locations */
			if (!in_bounds_fully(y, x)) continue;

			/* Require "naked" floor space */
			if (!cave_naked_bold(y, x)) continue;

			/* No teleporting into vaults and such */
			if (cave_info[y][x] & (CAVE_ICKY)) continue;

			/* This grid looks good */
			look = FALSE;

			/* Stop looking */
			break;
		}

		/* Increase the maximum distance */
		dis = dis * 2;

		/* Decrease the minimum distance */
		min = min / 2;
	}

	/* Sound */
	sound(MSG_TELEPORT);

	/* Move player */
	monster_swap(p_ptr->py, p_ptr->px, y, x);

	/* Handle stuff XXX XXX XXX */
	handle_stuff();
}

/*
 * Teleport monster to a grid near the given location
 *
 * This function is slightly obsessive about correctness.
 * This function allows teleporting into vaults (!)
 */
void teleport_monster_to(int m_idx, int ny, int nx)
{
	int my, mx;

	int y, x;

	int dis = 0, ctr = 0;

	monster_type *m_ptr = &m_list[m_idx];

	/* Paranoia */
	if (!m_ptr->r_idx) return;

	my = m_ptr->fy;
	mx = m_ptr->fx;

	/* Initialize */
	y = my;
	x = mx;

	/* Find a usable location */
	while (TRUE)
	{
		/* Pick a nearby legal location */
		while (TRUE)
		{
			y = rand_spread(ny, dis);
			x = rand_spread(nx, dis);
			if (in_bounds_fully(y, x)) break;
		}

		/* Accept "naked" floor grids */
		if (cave_naked_bold(y, x)) break;

		/* Occasionally advance the distance */
		if (++ctr > (4 * dis * dis + 4 * dis + 1))
		{
			ctr = 0;
			dis++;
		}
	}

	/* Sound */
	sound(MSG_TPOTHER);

	/* Move player */
	monster_swap(my, mx, y, x);

	/* Handle stuff XXX XXX XXX */
	handle_stuff();
}

/*
 * Teleport player to a grid near the given location
 *
 * This function is slightly obsessive about correctness.
 * This function allows teleporting into vaults (!)
 */
void teleport_player_to(int ny, int nx)
{
	int y = p_ptr->py;
	int x = p_ptr->px;

	int dis = 0, ctr = 0;

	/* Find a usable location */
	while (TRUE)
	{
		/* Pick a nearby legal location */
		while (TRUE)
		{
			y = rand_spread(ny, dis);
			x = rand_spread(nx, dis);
			if (in_bounds_fully(y, x)) break;
		}

		/* Accept "naked" floor grids */
		if (cave_naked_bold(y, x)) break;

		/* Occasionally advance the distance */
		if (++ctr > (4 * dis * dis + 4 * dis + 1))
		{
			ctr = 0;
			dis++;
		}
	}

	/* Sound */
	sound(MSG_TELEPORT);

	/* Move player */
	monster_swap(p_ptr->py, p_ptr->px, y, x);

	/* Handle stuff XXX XXX XXX */
	handle_stuff();
}

/*
 * Teleport the player one level up or down (random when legal)
 */
void teleport_player_level(void)
{
	if (adult_ironman)
	{
		message(MSG_FAIL, 0, "Nothing happens.");
		return;
	}

	if (!p_ptr->depth)
	{
		message(MSG_TPLEVEL, 0, "You sink through the floor.");

		/* New depth */
		p_ptr->depth++;

		/* Leaving */
		p_ptr->leaving = TRUE;
	}

	else if ((quest_check(p_ptr->depth) == QUEST_FIXED) || (p_ptr->depth >= MAX_DEPTH-1))
	{
		message(MSG_TPLEVEL, 0, "You rise up through the ceiling.");

		/* New depth */
		p_ptr->depth--;

		/* Leaving */
		p_ptr->leaving = TRUE;
	}

	else if (rand_int(100) < 50)
	{
		message(MSG_TPLEVEL, TRUE, "You rise up through the ceiling.");

		/* New depth */
		p_ptr->depth--;

		/* Leaving */
		p_ptr->leaving = TRUE;
	}

	else
	{
		message(MSG_TPLEVEL, TRUE, "You sink through the floor.");

		/* New depth */
		p_ptr->depth++;

		/* Leaving */
		p_ptr->leaving = TRUE;
	}
}

/*
 * Return a color to use for the bolt/ball spells
 */
static byte spell_color(int type)
{
	/* Analyze */
	switch (type)
	{
		case GF_MISSILE:	return (TERM_VIOLET);
		case GF_ACID:		return (TERM_SLATE);
		case GF_ELEC:		return (TERM_BLUE);
		case GF_FIRE:		return (TERM_RED);
		case GF_COLD:		return (TERM_WHITE);
		case GF_POIS:		return (TERM_GREEN);
		case GF_DISEASE:	return (TERM_L_GREEN);
		case GF_HOLY_ORB:	return (TERM_L_DARK);
		case GF_MANA:		return (TERM_L_DARK);
		case GF_ARROW:		return (TERM_WHITE);
		case GF_WATER:		return (TERM_SLATE);
		case GF_NETHER:		return (TERM_L_GREEN);
		case GF_CHAOS:		return (TERM_VIOLET);
		case GF_DISENCHANT:	return (TERM_VIOLET);
		case GF_NEXUS:		return (TERM_L_RED);
		case GF_CONFUSION:	return (TERM_L_UMBER);
		case GF_SOUND:		return (TERM_YELLOW);
		case GF_SHARD:		return (TERM_UMBER);
		case GF_FORCE:		return (TERM_UMBER);
		case GF_INERTIA:	return (TERM_L_WHITE);
		case GF_GRAVITY:	return (TERM_L_WHITE);
		case GF_TIME:		return (TERM_L_BLUE);
		case GF_LITE_WEAK:	return (TERM_ORANGE);
		case GF_LITE:		return (TERM_ORANGE);
		case GF_DARK_WEAK:	return (TERM_L_DARK);
		case GF_DARK:		return (TERM_L_DARK);
		case GF_PLASMA:		return (TERM_RED);
		case GF_ASTRAL:		return (TERM_BLUE);
		case GF_ICE:		return (TERM_WHITE);
	}

	/* Standard "color" */
	return (TERM_WHITE);
}

/*
 * Find the attr/char pair to use for a spell effect
 *
 * It is moving (or has moved) from (x,y) to (nx,ny).
 *
 * If the distance is not "one", we (may) return "*".
 */
static u16b bolt_pict(int y, int x, int ny, int nx, int typ)
{
	int base;

	byte k;

	byte a;
	char c;

	/* No motion (*) */
	if ((ny == y) && (nx == x)) base = 0x30;

	/* Vertical (|) */
	else if (nx == x) base = 0x40;

	/* Horizontal (-) */
	else if (ny == y) base = 0x50;

	/* Diagonal (/) */
	else if ((ny-y) == (x-nx)) base = 0x60;

	/* Diagonal (\) */
	else if ((ny-y) == (nx-x)) base = 0x70;

	/* Weird (*) */
	else base = 0x30;

	/* Basic spell color */
	k = spell_color(typ);

	/* Obtain attr/char */
	a = misc_to_attr[base+k];
	c = misc_to_char[base+k];

	/* Create pict */
	return (PICT(a,c));
}

/*
 * Hack Apply the actual hit and check for absorbance.
 */
void take_hit(int damage, cptr ddesc)
{
	if (!p_ptr->absorb) damage_player(damage, ddesc);
	else 
	{
		if (p_ptr->absorb > damage)
		{
			message_format(MSG_EFFECT, 0, "you have absorbed %d damage.", damage);
			hp_player(damage);
			set_absorb(p_ptr->absorb - damage);
		}
		else
		{
			int ab_limit = p_ptr->absorb * 2;

			message_format(MSG_EFFECT, 0, "you have absorbed %d damage.", p_ptr->absorb);
			if (damage > ab_limit) damage_player(damage - ab_limit, ddesc);
			else hp_player(ab_limit - damage);
			set_absorb(0);
		}
	}
}

/*
 * Does an item ignore the damage from acid? 
 */
bool ignores_acid_p(u32b f2, u32b f3, u32b f4)
{
	if (f3 & TR3_IGNORE_ELEM) return TRUE;
	if (f2 & TR2_RES_ACID) return TRUE;
	if (f4 & TR4_BRAND_ACID) return TRUE;
	if (f2 & TR2_IM_ACID) return TRUE;

	return FALSE;
}

/*
 * Does an item ignore the damage from fire? 
 */
bool ignores_fire_p(u32b f2, u32b f3, u32b f4)
{
	if (f3 & TR3_IGNORE_ELEM) return TRUE;
	if (f2 & TR2_RES_FIRE) return TRUE;
	if (f4 & TR4_BRAND_FIRE) return TRUE;
	if (f2 & TR2_IM_FIRE) return TRUE;

	return FALSE;
}

/*
 * Does an item ignore the damage from cold? 
 */
bool ignores_cold_p(u32b f2, u32b f3, u32b f4)
{
	if (f3 & TR3_IGNORE_ELEM) return TRUE;
	if (f2 & TR2_RES_COLD) return TRUE;
	if (f4 & TR4_BRAND_COLD) return TRUE;
	if (f2 & TR2_IM_COLD) return TRUE;

	return FALSE;
}

/*
 * Does an item ignore the damage from elec? 
 */
bool ignores_elec_p(u32b f2, u32b f3, u32b f4)
{
	if (f3 & TR3_IGNORE_ELEM) return TRUE;
	if (f2 & TR2_RES_ELEC) return TRUE;
	if (f4 & TR4_BRAND_ELEC) return TRUE;
	if (f2 & TR2_IM_ELEC) return TRUE;

	return FALSE;
}

/*
 * Does a given class of objects (usually) hate acid?
 * Note that acid can either melt or corrode something.
 */
static bool hates_acid(object_type *o_ptr)
{
	/* Analyze the type */
	switch (object_material(o_ptr))
	{
		/* Stuff that is always damaged by acid */
		case MATERIAL_PAPER:
		case MATERIAL_CLOTH:
		case MATERIAL_BONE:
		case MATERIAL_LEATHER:
		case MATERIAL_STEEL:
		case MATERIAL_STONE:
		case MATERIAL_S_WOOD:
		{
			return (TRUE);
		}

		/* Other metals, very rarely */
		case MATERIAL_METAL:
		{
			if (rand_int(10) == 0) return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Does a given object (usually) hate electricity?
 */
static bool hates_elec(object_type *o_ptr)
{
	switch (object_material(o_ptr))
	{
		/* Various metals (not steel)*/
		case MATERIAL_METAL:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Does a given object (usually) hate fire?
 */
static bool hates_fire(object_type *o_ptr)
{
	switch (object_material(o_ptr))
	{
		/* Stuff that is always damaged by fire */
		case MATERIAL_PAPER:
		case MATERIAL_CLOTH:
		case MATERIAL_S_WOOD:
		case MATERIAL_H_WOOD:
		case MATERIAL_BONE:
		case MATERIAL_FOOD:
		{
			return (TRUE);
		}

		/* Leather, rarely */
		case MATERIAL_LEATHER:
		{
			if (rand_int(3) == 0) return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Does a given object (usually) hate cold?
 */
static bool hates_cold(object_type *o_ptr)
{
	switch (object_material(o_ptr))
	{
		/* Liquids */
		case MATERIAL_LIQUID:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Does a given object (usually) hate rust?
 */
static bool hates_rust(object_type *o_ptr)
{
	switch (object_material(o_ptr))
	{
		/* Liquids */
		case MATERIAL_STEEL:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Does a given object (usually) hate rust?
 */
static bool hates_rot(object_type *o_ptr)
{
	switch (object_material(o_ptr))
	{
		/* Wood and food */
		case MATERIAL_S_WOOD:
		case MATERIAL_H_WOOD:
		case MATERIAL_FOOD:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Melt something - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_acid_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_acid(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (ignores_acid_p(f2,f3,f4)) return (-1);
	return (1);
}

/*
 * Electrical damage - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_elec_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_elec(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (ignores_elec_p(f2,f3,f4)) return (-1);
	return (1);
}

/*
 * Burn something - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_fire_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_fire(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (ignores_fire_p(f2,f3,f4)) return (-1);
	return (1);
}

/*
 * Freeze something - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_cold_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_cold(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (ignores_cold_p(f2,f3,f4)) return (-1);
	return (1);
}

/*
 * Rust something - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_rust_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_rust(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (f3 & (TR3_IGNORE_NON_ELEM)) return (-1);

	return (1);
}

/*
 * Rot something - returns 1 if succeptible, 0 if not, -1 if immune
 */
static int set_rot_destroy(object_type *o_ptr)
{
	u32b f1, f2, f3, f4;
	if (!hates_rot(o_ptr)) return (0);
	object_flags(o_ptr, &f1, &f2, &f3, &f4);
	if (f3 & (TR3_IGNORE_NON_ELEM)) return (-1);

	return (1);
}

/*
 * This seems like a pretty standard "typedef"
 */
typedef int (*inven_func)(object_type *);

/*
 * Destroys a type of item on a given percent chance
 * Note that missiles are no longer necessarily all destroyed
 *
 * Returns number of items destroyed.
 */
static void inven_damage(int dam, inven_func typ, bool allow_wield, cptr verb)
{
	int i, j, k ,l ,n;
	int count, amt;
	int perc = (dam < 30) ? 10 : (dam < 60) ? 20 : 30;
	int max = (dam < 50) ? 1 : (dam / 50);

	object_type *o_ptr;

	char o_name[80];

	/* Count the casualties */
	count = 0;

	/* Start from a random item */
 	k = rand_int(INVEN_TOTAL);

	/* Scan through the slots backwards */
	for (l = INVEN_TOTAL; (l >= 0); l--)
	{
		/* Reached maximum allowance */
		if (count >= max) break;

		/* Find the inventory slot */
		i = ((k + l) % INVEN_TOTAL);

		/* Hack - Skip certain slots */
		if (!allow_wield && i == INVEN_WIELD) continue;
		if (i == INVEN_BOW) continue; /* don't harm wielded bows */
		if ((i == INVEN_RIGHT) || (i == INVEN_LEFT)) continue; /* don't harm wielded rings */
		if (i == INVEN_NECK) continue; /* don't harm wielded amulets */
		if (i == INVEN_LITE) continue; /* don't harm wielded lights */
		if (i == INVEN_MUSIC) continue; /* don't harm wielded musical instruments */
		
		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Hack -- for now, skip artifacts */
		if (artifact_p(o_ptr)) continue;

		/* Object resists */
		n = ((*typ)(o_ptr));

		/* Item just isn't normally affected by element */
		if (!n) continue;

		/* Count the casualties */
		for (amt = j = 0; j < o_ptr->number; ++j)
		{
			if (rand_int(100) < perc) amt++;
		}

		/* Some casualities */
		if (amt)
		{
			/* Describe */
			object_desc(o_name, o_ptr, FALSE, 0);

			/* Item ignores element */
			if (n == -1)
			{
				message_format(MSG_ITEM_RESIST, o_ptr->k_idx, 
					"Your %s is unaffected!", o_name);

				count++;

				continue;
			}

			/* Item was damaged, behavior depends on type */
			switch (o_ptr->tval)
			{
				case TV_CHEST:
				case TV_SHOT:
				case TV_ARROW:		
				case TV_BOLT:			
				case TV_LITE:	
				case TV_LITE_SPECIAL:
				case TV_AMULET:
				case TV_RING:			
				case TV_MUSIC:		
				case TV_STAFF:		
				case TV_WAND:			
				case TV_TALISMAN:
				case TV_ROD:	
				case TV_SCROLL:
				case TV_POTION:		
				case TV_POWDER:		
				case TV_FLASK:		
				case TV_FOOD:			
				case TV_MAGIC_BOOK:
				{
					/* Simple objects, just reduce in quantities by amt */

					/* Message */
					message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, "%sour %s (%c) %s!",
						((o_ptr->number > 1) ?
						((amt == o_ptr->number) ? "All of y" :
						(amt > 1 ? "Some of y" : "One of y")) : "Y"),
						o_name, index_to_label(i),
						verb);

					/* Destroy "amt" items */
					inven_item_increase(i, -amt);
					inven_item_optimize(i);

					break;
				}

				case TV_BOOTS:	
				case TV_GLOVES:		
				case TV_HEADGEAR:
				case TV_SHIELD:	
				case TV_CLOAK:		
				case TV_BODY_ARMOR:
				case TV_DRAG_ARMOR:	
				{
					/* Armor. Reduce AC, maybe destroy */

					/* XXX XXX Note that piles will be reduced as single items */
					
					/* No damage left to be done */
					if (o_ptr->ac + o_ptr->to_a <= 0) continue;

					/* Damage the item */
					o_ptr->to_a--;

					/* Totally destroy the item, if not an ego item */
					if (!ego_item_p(o_ptr) && (o_ptr->ac + o_ptr->to_a == 0))
					{
						inven_item_increase(i, -1);
						inven_item_optimize(i);

						message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, "%sour %s (%c) %s!",
							((o_ptr->number > 1) ?
							((amt == o_ptr->number) ? "All of y" :
							(amt > 1 ? "Some of y" : "One of y")) : "Y"),
							o_name, index_to_label(i),
							verb);
					}
					/* Message */
					else message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, 
						"Your %s is damaged!", o_name);

					break;
				}

				case TV_BOW:		
				case TV_DIGGING:
				case TV_HAFTED:	
				case TV_POLEARM:	
				case TV_SWORD:	
				{
					/* Weapon. Reduce either to hit or to dam down to -2 */

					/* XXX XXX Note that piles will be reduced as single items */

					if (rand_int(2) == 0) 
					/* Reduce to damage */
					{ 
						if (o_ptr->to_d <= -2) continue;
						o_ptr->to_d--;
					}
					else 
					/* Reduce to hit */
					{ 
						if (o_ptr->to_h <= -2) continue;
						o_ptr->to_h--;
					}

					/* Message */
					message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, "Your %s is damaged!", o_name);

					break;
				}
			}

			/* Count the casualties */
			count++;
		}
	}

	/* Calculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

	/* Return the casualty count */
	return;
}

/*
 * Hurt the player with Acid
 */
void acid_dam(int dam, cptr kb_str)
{
	/* Total Immunity */
	if (p_ptr->immune_acid || (dam <= 0)) return;

	/* Resist the damage */
	if (p_ptr->resist_acid) dam = (dam + 2) / 3;
	if (p_ptr->oppose_acid) dam = (dam + 2) / 3;

	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_acid_destroy, FALSE, "melted");
}

/*
 * Hurt the player with electricity
 */
void elec_dam(int dam, cptr kb_str)
{
	/* Total immunity */
	if (p_ptr->immune_elec || (dam <= 0)) return;

	/* Resist the damage */
	if (p_ptr->oppose_elec) dam = (dam + 2) / 3;
	if (p_ptr->resist_elec) dam = (dam + 2) / 3;

	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_elec_destroy, FALSE, "exploded");
}

/*
 * Hurt the player with Fire
 */
void fire_dam(int dam, cptr kb_str)
{
	/* Totally immune */
	if (p_ptr->immune_fire || (dam <= 0)) return;

	/* Resist the damage */
	if (p_ptr->resist_fire) dam = (dam + 2) / 3;
	if (p_ptr->oppose_fire) dam = (dam + 2) / 3;

	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_fire_destroy, FALSE, "burnt up");
}

/*
 * Hurt the player with Cold
 */
void cold_dam(int dam, cptr kb_str)
{
	/* Total immunity */
	if (p_ptr->immune_cold || (dam <= 0)) return;

	/* Resist the damage */
	if (p_ptr->resist_cold) dam = (dam + 2) / 3;
	if (p_ptr->oppose_cold) dam = (dam + 2) / 3;

	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_cold_destroy, FALSE, "froze");
}


/*
 * Hurt the player with rust
 */
void rust_dam(int dam, cptr kb_str)
{
	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_rust_destroy, TRUE, "crumbled");
}

/*
 * Hurt the player with rust
 */
void rot_dam(int dam, cptr kb_str)
{
	/* Take damage */
	take_hit(dam, kb_str);

	/* Damage armor/inventory */
	inven_damage(dam, set_rot_destroy, TRUE, "decayed");
}

/*
 * Apply disenchantment to the player's stuff
 *
 * This function is also called from the "melee" code.
 *
 * The "mode" is currently unused.
 *
 * Return "TRUE" if the player notices anything.
 */
bool apply_disenchant(void)
{
	int t = 0;
	u32b f1, f2, f3, f4;

	object_type *o_ptr;

	char o_name[80];

	/* Pick a random slot */
	switch (randint(8))
	{
		case 1: t = INVEN_WIELD; break;
		case 2: t = INVEN_BOW; break;
		case 3: t = INVEN_BODY; break;
		case 4: t = INVEN_OUTER; break;
		case 5: t = INVEN_ARM; break;
		case 6: t = INVEN_HEAD; break;
		case 7: t = INVEN_HANDS; break;
		case 8: t = INVEN_FEET; break;
	}

	/* Get the item */
	o_ptr = &inventory[t];

	/* No item, nothing happens */
	if (!o_ptr->k_idx) return (FALSE);

	object_flags(o_ptr, &f1, &f2, &f3, &f4);

	/* Item resists disenchatment */
	if (f3 & TR3_IGNORE_DISEN) return (FALSE);

	/* Nothing to disenchant */
	if ((o_ptr->to_h <= 0) && (o_ptr->to_d <= 0) && (o_ptr->to_a <= 0))
	{
		/* Nothing to notice */
		return (FALSE);
	}

	/* Describe the object */
	object_desc(o_name, o_ptr, FALSE, 0);

	/* Artifacts have 60% chance to resist */
	if (artifact_p(o_ptr) && (rand_int(100) < 60))
	{
		/* Message */
		message_format(MSG_ITEM_RESIST, o_ptr->k_idx, "Your %s (%c) resist%s disenchantment!",
		           o_name, index_to_label(t),
		           ((o_ptr->number != 1) ? "" : "s"));

		/* Disturb */
		if (disturb_minor) disturb(0);

		/* Notice */
		return (TRUE);
	}

	/* Disenchant tohit */
	if (o_ptr->to_h > 0) o_ptr->to_h--;
	if ((o_ptr->to_h > 5) && (rand_int(100) < 20)) o_ptr->to_h--;

	/* Disenchant todam */
	if (o_ptr->to_d > 0) o_ptr->to_d--;
	if ((o_ptr->to_d > 5) && (rand_int(100) < 20)) o_ptr->to_d--;

	/* Disenchant toac */
	if (o_ptr->to_a > 0) o_ptr->to_a--;
	if ((o_ptr->to_a > 5) && (rand_int(100) < 20)) o_ptr->to_a--;

	/* Message */
	message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, "Your %s (%c) %s disenchanted!",
	           o_name, index_to_label(t),
	           ((o_ptr->number != 1) ? "were" : "was"));

	/* Disturb */
	if (disturb_minor) disturb(0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	p_ptr->window |= (PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

	/* Notice */
	return (TRUE);
}

/*
 * Apply Nexus
 */
static void apply_nexus(monster_type *m_ptr)
{

	switch (randint(7))
	{
		case 1: case 2: case 3:
		{
			teleport_player(200);
			break;
		}

		case 4: case 5:
		{
			teleport_player_to(m_ptr->fy, m_ptr->fx);
			break;
		}

		case 6:
		{
			if (rand_int(100) < p_ptr->skill[SK_SAV])
			{
				message(MSG_RESIST, 0, "You resist the effects!");
				break;
			}

			/* Teleport Level */
			teleport_player_level();
			break;
		}

		case 7:
		{
			if (rand_int(100) < p_ptr->skill[SK_SAV])
			{
				message(MSG_RESIST, 0, "You resist the effects!");
				break;
			}

			message(MSG_RESIST, 0, "Your body starts to scramble...");

			/*scramble a pair of stats */
			scramble_stats(); 
			break;
		}
	}
}

/*
 * Mega-Hack -- track "affected" monsters (see "project()" comments)
 */
static int project_m_n;
static int project_m_x;
static int project_m_y;

/*
 * We are called from "project()" to "damage" terrain features
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 *
 * Hack -- We also "see" grids which are "memorized".
 *
 * Perhaps we should affect doors and/or walls.
 */
static bool project_f(int who, int r, int y, int x, int dam, int typ)
{
	bool obvious = FALSE;

	/* Analyze the type */
	switch (typ)
	{
		/* Ignore most effects */
		case GF_ACID:
		case GF_ELEC:
		case GF_FIRE:
		case GF_COLD:
		case GF_PLASMA:
		case GF_ASTRAL:
		case GF_ICE:
		case GF_SHARD:
		case GF_FORCE:
		case GF_SOUND:
		case GF_MANA:
		case GF_HOLY_ORB:
		{
			break;
		}

		/* Destroy Traps (and Locks) */
		case GF_KILL_TRAP:
		{
			/* Reveal secret doors */
			if (cave_feat[y][x] == FEAT_SECRET)
			{
				place_closed_door(y, x);

				/* Check line of sight */
				if (player_has_los_bold(y, x))
				{
					obvious = TRUE;
				}
			}

			/* Destroy traps */
			if ((cave_feat[y][x] == FEAT_INVIS) ||
			    ((cave_feat[y][x] >= FEAT_TRAP_HEAD) &&
			     (cave_feat[y][x] <= FEAT_TRAP_TAIL)))
			{
				/* Check line of sight */
				if (player_has_los_bold(y, x))
				{
					message(MSG_EFFECT, 0, "There is a bright flash of light!");
					obvious = TRUE;
				}

				/* Forget the trap */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the trap */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Locked doors are unlocked */
			else if ((cave_feat[y][x] >= FEAT_DOOR_HEAD + 0x01) &&
			          (cave_feat[y][x] <= FEAT_DOOR_HEAD + 0x07))
			{
				/* Unlock the door */
				cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x00);

				/* Check line of sound */
				if (player_has_los_bold(y, x))
				{
					message(MSG_EFFECT, 0, "Click!");
					obvious = TRUE;
				}
			}

			break;
		}

		/* Destroy Doors (and traps) */
		case GF_KILL_DOOR:
		{
			/* Destroy all doors and traps */
			if ((cave_feat[y][x] == FEAT_OPEN) ||
			    (cave_feat[y][x] == FEAT_BROKEN) ||
			    (cave_feat[y][x] == FEAT_INVIS) ||
			    ((cave_feat[y][x] >= FEAT_TRAP_HEAD) &&
			     (cave_feat[y][x] <= FEAT_TRAP_TAIL)) ||
			    ((cave_feat[y][x] >= FEAT_DOOR_HEAD) &&
			     (cave_feat[y][x] <= FEAT_DOOR_TAIL)))
			{
				/* Check line of sight */
				if (player_has_los_bold(y, x))
				{
					/* Message */
					message(MSG_EFFECT, 0, "There is a bright flash of light!");
					obvious = TRUE;

					/* Visibility change */
					if ((cave_feat[y][x] >= FEAT_DOOR_HEAD) &&
					    (cave_feat[y][x] <= FEAT_DOOR_TAIL))
					{
						/* Update the visuals */
						p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);
					}
				}

				/* Forget the door */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the feature */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			break;
		}

		/* Destroy walls (and doors) */
		case GF_KILL_WALL:
		{
			/* Non-walls (etc) */
			if (cave_floor_bold(y, x)) break;

			/* Permanent walls */
			if (cave_feat[y][x] >= FEAT_PERM_EXTRA) break;

			/* Granite */
			if (cave_feat[y][x] >= FEAT_WALL_EXTRA)
			{
				/* Message */
				if (cave_info[y][x] & (CAVE_MARK))
				{
					message(MSG_EFFECT, 0, "The wall turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Quartz / Magma with treasure */
			else if (cave_feat[y][x] >= FEAT_MAGMA_H)
			{
				/* Message */
				if (cave_info[y][x] & (CAVE_MARK))
				{
					message(MSG_EFFECT, 0, "The vein turns into mud!");
					message(MSG_EFFECT, -1, "You have found something!");
					obvious = TRUE;
				}

				/* Forget the wall */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);

				/* Place some gold */
				place_gold(y, x);
			}

			/* Quartz / Magma */
			else if (cave_feat[y][x] >= FEAT_MAGMA)
			{
				/* Message */
				if (cave_info[y][x] & (CAVE_MARK))
				{
					message(MSG_EFFECT, 0, "The vein turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the wall */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Rubble */
			else if (cave_feat[y][x] == FEAT_RUBBLE)
			{
				/* Message */
				if (cave_info[y][x] & (CAVE_MARK))
				{
					message(MSG_EFFECT, 0, "The rubble turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the rubble */
				cave_set_feat(y, x, FEAT_FLOOR);

				/* Hack -- place an object */
				if (rand_int(100) < 10)
				{
					/* Found something */
					if (player_can_see_bold(y, x))
					{
						message(MSG_EFFECT, 0, "There was something buried in the rubble!");
						obvious = TRUE;
					}

					/* Place gold */
					place_object(y, x, FALSE, FALSE);
				}
			}

			/* Destroy doors (and secret doors) */
			else /* if (cave_feat[y][x] >= FEAT_DOOR_HEAD) */
			{
				/* Hack -- special message */
				if (cave_info[y][x] & (CAVE_MARK))
				{
					message(MSG_EFFECT, 0, "The door turns into mud!");
					obvious = TRUE;
				}

				/* Forget the wall */
				cave_info[y][x] &= ~(CAVE_MARK);

				/* Destroy the feature */
				cave_set_feat(y, x, FEAT_FLOOR);
			}

			/* Update the visuals */
			p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

			/* Fully update the flow */
			p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);

			break;
		}

		/* Make doors */
		case GF_MAKE_DOOR:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Create closed door */
			cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x00);

			/* Observe */
			if (cave_info[y][x] & (CAVE_MARK)) obvious = TRUE;

			/* Update the visuals */
			p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

			break;
		}

		/* Make traps */
		case GF_MAKE_TRAP:
		{
			/* Require a "naked" floor grid */
			if (!cave_naked_bold(y, x)) break;

			/* Place a trap */
			place_trap(y, x);

			break;
		}

		/* Lite up the grid */
		case GF_LITE_WEAK:
		case GF_LITE:
		{
			/* Turn on the light */
			cave_info[y][x] |= (CAVE_GLOW);

			/* Grid is in line of sight */
			if (player_has_los_bold(y, x))
			{
				/* Observe */
				obvious = TRUE;

				/* Fully update the visuals */
				p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);
			}

			break;
		}

		/* Darken the grid */
		case GF_DARK_WEAK:
		case GF_DARK:
		{
			/* Turn off the light */
			cave_info[y][x] &= ~(CAVE_GLOW);

			/* Hack -- Forget "boring" grids */
			if (cave_feat[y][x] <= FEAT_INVIS)
			{
				/* Forget */
				cave_info[y][x] &= ~(CAVE_MARK);
			}

			/* Grid is in line of sight */
			if (player_has_los_bold(y, x))
			{
				/* Observe */
				obvious = TRUE;

				/* Fully update the visuals */
				p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);
			}

			/* All done */
			break;
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}

/*
 * We are called from "project()" to "damage" objects
 *
 * We are called both for "beam" effects and "ball" effects.
 *
 * Perhaps we should only SOMETIMES damage things on the ground.
 *
 * The "r" parameter is the "distance from ground zero".
 *
 * Note that we determine if the player can "see" anything that happens
 * by taking into account: blindness, line-of-sight, and illumination.
 *
 * Hack -- We also "see" objects which are "memorized".
 *
 * We return "TRUE" if the effect of the projection is "obvious".
 */
static bool project_o(int who, int r, int y, int x, int dam, int typ)
{
	s16b this_o_idx, next_o_idx = 0;

	bool obvious = FALSE;

	u32b f1, f2, f3, f4;

	char o_name[80];

	/* Scan all objects in the grid */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		bool is_art = FALSE;
		bool ignore = FALSE;
		bool plural = FALSE;
		bool do_kill = FALSE;

		cptr note_kill = NULL;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Extract the flags */
		object_flags(o_ptr, &f1, &f2, &f3, &f4);

		/* Get the "plural"-ness */
		if (o_ptr->number > 1) plural = TRUE;

		/* Check for artifact */
		if (artifact_p(o_ptr)) is_art = TRUE;

		/* Analyze the type */
		switch (typ)
		{
			/* Acid -- Lots of things */
			case GF_ACID:
			{
				if (hates_acid(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " melt!" : " melts!");
					if (ignores_acid_p(f2,f3,f4)) ignore = TRUE;
				}
				break;
			}

			/* Elec -- Rings and Wands */
			case GF_ELEC:
			{
				if (hates_elec(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
					if (ignores_elec_p(f2,f3,f4)) ignore = TRUE;
				}
				break;
			}

			/* Fire -- Flammable objects */
			case GF_FIRE:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " burn up!" : " burns up!");
					if (ignores_fire_p(f2,f3,f4)) ignore = TRUE;
				}
				break;
			}

			/* Cold -- potions and flasks */
			case GF_COLD:
			{
				if (hates_cold(o_ptr))
				{
					note_kill = (plural ? " shatter!" : " shatters!");
					do_kill = TRUE;
					if (ignores_cold_p(f2,f3,f4)) ignore = TRUE;
				}
				break;
			}

			/* Fire + Elec */
			case GF_PLASMA:
			{
				if (hates_fire(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " burn up!" : " burns up!");
					if (ignores_fire_p(f2,f3,f4)) ignore = TRUE;
				}
				if (hates_elec(o_ptr))
				{
					ignore = FALSE;
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
					if (ignores_elec_p(f2,f3,f4)) ignore = TRUE;
				}
				break;
			}

			/* Hack -- break potions and such */
			case GF_ICE:
			case GF_SHARD:
			case GF_FORCE:
			case GF_SOUND:
			{
				if (hates_cold(o_ptr))
				{
					note_kill = (plural ? " shatter!" : " shatters!");
					do_kill = TRUE;
				}
				break;
			}

			/* Mana -- destroys everything */
			case GF_MANA:
			{
				do_kill = TRUE;
				note_kill = (plural ? " are destroyed!" : " is destroyed!");
				break;
			}

			/* Holy Orb -- destroys cursed non-artifacts */
			case GF_HOLY_ORB:
			{
				if (cursed_p(o_ptr))
				{
					do_kill = TRUE;
					note_kill = (plural ? " are destroyed!" : " is destroyed!");
				}
				break;
			}

			/* Unlock chests */
			case GF_KILL_TRAP:
			case GF_KILL_DOOR:
			{
				/* Chests are noticed only if trapped or locked */
				if (o_ptr->tval == TV_CHEST)
				{
					/* Disarm/Unlock traps */
					if (o_ptr->pval > 0)
					{
						/* Disarm or Unlock */
						o_ptr->pval = (0 - o_ptr->pval);

						/* Identify */
						object_known(o_ptr);

						/* Notice */
						if (o_ptr->marked)
						{
							message(MSG_EFFECT, 0, "Click!");
							obvious = TRUE;
						}
					}
				}

				break;
			}
		}

		/* Attempt to destroy the object */
		if (do_kill)
		{
			/* Effect "observed" */
			if (o_ptr->marked)
			{
				obvious = TRUE;
				object_desc(o_name, o_ptr, FALSE, 0);
			}

			/* Artifacts, and other objects, get to resist */
			if (is_art || ignore)
			{
				/* Observe the resist */
				if (o_ptr->marked)
				{
					message_format(MSG_ITEM_RESIST, o_ptr->k_idx, "The %s %s unaffected!",
					           o_name, (plural ? "are" : "is"));
				}
			}

			/* Kill it */
			else
			{
				/* Describe if needed */
				if (o_ptr->marked && note_kill)
				{
					message_format(MSG_ITEM_DAMAGE, o_ptr->k_idx, "The %s%s", o_name, note_kill);
				}

				/* Delete the object */
				delete_object_idx(this_o_idx);

				/* Redraw */
				lite_spot(y, x);
			}
		}
	}

	/* Return "Anything seen?" */
	return (obvious);
}

/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to a monster.
 *
 * This routine takes a "source monster" (by index) which is mostly used to
 * determine if the player is causing the damage, and a "radius" (see below),
 * which is used to decrease the power of explosions with distance, and a
 * location, via integers which are modified by certain types of attacks
 * (polymorph and teleport being the obvious ones), a default damage, which
 * is modified as needed based on various properties, and finally a "damage
 * type" (see below).
 *
 * Note that this routine can handle "no damage" attacks (like teleport) by
 * taking a "zero" damage, and can even take "parameters" to attacks (like
 * confuse) by accepting a "damage", using it to calculate the effect, and
 * then setting the damage to zero.  Note that the "damage" parameter is
 * divided by the radius, so monsters not at the "epicenter" will not take
 * as much damage (or whatever)...
 *
 * Note that "polymorph" is dangerous, since a failure in "place_monster()"'
 * may result in a dereference of an invalid pointer.  XXX XXX XXX
 *
 * Various messages are produced, and damage is applied.
 *
 * - Eytan Zweig, EyAngband 0.2.0 -
 * In addition to those possessing the appropriate resist flag, all monsters
 * that can breathe something resist it. Note that the monster lore gets updated 
 * with a "false" resistance flag, even if the breather doesn't have the 
 * resistance seperately (which it has no reason to do). This flag will
 * be removed once it is known that the monster can also breathe this substance.
 *
 * We assume "Nether" is an evil, necromantic force, so it doesn't hurt undead,
 * and hurts evil less.  If can breath nether, then it resists it as well.
 *
 * Damage reductions use the following formulas:
 *   Note that "dam = dam * 6 / (randint(6) + 6);"
 *     gives avg damage of .655, ranging from .858 to .500
 *   Note that "dam = dam * 5 / (randint(6) + 6);"
 *     gives avg damage of .544, ranging from .714 to .417
 *   Note that "dam = dam * 4 / (randint(6) + 6);"
 *     gives avg damage of .444, ranging from .556 to .333
 *   Note that "dam = dam * 3 / (randint(6) + 6);"
 *     gives avg damage of .327, ranging from .427 to .250
 *   Note that "dam = dam * 2 / (randint(6) + 6);"
 *     gives something simple.
 *
 * In this function, "result" messages are postponed until the end, where
 * the "note" string is appended to the monster name, if not NULL.  So,
 * to make a spell have "no effect" just set "note" to NULL.  You should
 * also set "notice" to FALSE, or the player will learn what the spell does.
 *
 * We attempt to return "TRUE" if the player saw anything "useful" happen.
 */
static bool project_m(int who, int r, int y, int x, int dam, int typ)
{
	int tmp;

	monster_type *m_ptr;
	monster_race *r_ptr;

	cptr name;

	int resist = 1; /* Resistance level */

	/* Is the monster "seen"? */
	bool seen = FALSE;

	/* Were the effects "obvious" (if seen)? */
	bool obvious = FALSE;

	/* Were the effects "irrelevant"? */
	bool skipped = FALSE;

	/* Polymorph setting (true or false) */
	int do_poly = 0;

	/* Teleport setting (max distance) */
	int do_dist = 0;

	/* Call monster setting (true or false) */
	int do_call = 0;

	/* Confusion setting (amount to confuse) */
	int do_conf = 0;

	/* Stunning setting (amount to stun) */
	int do_stun = 0;

	/* Sleep amount (amount to sleep) */
	int do_sleep = 0;

	/* Fear amount (amount to fear) */
	int do_fear = 0;

	/* Poison amount (amount to poison) */
	int do_pois = 0;

	/* Blindness setting (amount to blind) */
	int do_blind = 0;

	/* Calm setting (amount to calm) */
	int do_calm = 0;

	/* Cut amount (amount to cut) */
	int do_cut = 0;

	/* Hold the monster name */
	char m_name[80];

	/* Assume no note */
	cptr note = NULL;

	/* Assume a default death */
	cptr note_dies = " dies.";

	/* Walls protect monsters */
	if (!cave_floor_bold(y,x)) return (FALSE);

	/* No monster here */
	if (!(cave_m_idx[y][x] > 0)) return (FALSE);

	/* Never affect projector */
	if (cave_m_idx[y][x] == who) return (FALSE);

	/* Obtain monster info */
	m_ptr = &m_list[cave_m_idx[y][x]];
	r_ptr = get_monster_real(m_ptr);
	name = monster_name(m_ptr);
	if (m_ptr->ml) seen = TRUE;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);

	/* Get the monster name (BEFORE polymorphing) */
	monster_desc(m_name, m_ptr, 0);

	/* Some monsters get "destroyed" */
	if ((r_ptr->flags2 & (RF2_DEMON)) ||
	    (r_ptr->flags2 & (RF2_UNDEAD)) ||
	    (r_ptr->flags2 & (RF2_STUPID)) ||
	    (r_ptr->flags2 & (RF2_PLANT)) ||
	    (strchr("Evg$|!?~=", r_ptr->d_char)))
	{
		/* Special note at death */
		note_dies = " is destroyed.";
	}

	/* Uniques get a higher resistance level */
	if (m_ptr->u_idx) resist = 10;

	/* Analyze the damage type */
	switch (typ)
	{
		/* Magic Missile -- pure damage */
		case GF_MISSILE:
		{
			if (seen) obvious = TRUE;
			break;
		}

		/* Holy Orb -- hurts Evil */
		case GF_HOLY_ORB:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags2 & (RF2_EVIL))
			{
				dam *= 2;
				note = " is hit hard.";
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);
			}
			break;
		}

		/* Arrow -- no defense XXX */
		case GF_ARROW:
		{
			if (seen) obvious = TRUE;
			break;
		}

		/* Acid */
		case GF_ACID:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_ACID)) || (r_ptr->flags4 & (RF4_BR_ACID)))
			{
				note = " resists a lot.";
				dam /= 9;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_ACID , FALSE);
			}
			break;
		}

		/* Electricity */
		case GF_ELEC:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_ELEC)) || (r_ptr->flags4 & (RF4_BR_ELEC)))
			{
				note = " resists a lot.";
				dam /= 9;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_ELEC, FALSE);
			}
			break;
		}

		/* Fire damage */
		case GF_FIRE:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_FIRE)) || (r_ptr->flags4 & (RF4_BR_FIRE)))
			{
				note = " resists a lot.";
				dam /= 9;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_FIRE, FALSE);
			}
			break;
		}

		/* Cold */
		case GF_COLD:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_COLD)) || (r_ptr->flags4 & (RF4_BR_COLD)))
			{
				note = " resists a lot.";
				dam /= 9;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_COLD, FALSE);
			}
			break;
		}

		/* Poison */
		case GF_POIS:
		{
			if (seen) obvious = TRUE;
			/* 25% of poisoning */
			if (rand_int(4) == 0) do_pois = (10 + randint(11) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_POIS)) || (r_ptr->flags4 & (RF4_BR_POIS)))
			{
				note = " resists a lot.";
				dam /= 9;
				do_pois = 0;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_POIS, FALSE);
			}
			break;
		}

		/* Disease */
		case GF_DISEASE:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_DISEASE)) || (r_ptr->flags4 & (RF4_BR_DISEASE)))
			{
				note = " resists a lot.";
				dam /= 8;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_DISEASE, FALSE);
			}
			break;
		}

		/* Plasma -- perhaps check ELEC or FIRE XXX */
		case GF_PLASMA:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_PLAS)) || (r_ptr->flags4 & (RF4_BR_PLAS)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_PLAS, FALSE);
			}
			break;
		}

		/* Nether -- see above */
		case GF_NETHER:
		{
			if (seen) obvious = TRUE;
			if (r_ptr->flags2 & (RF2_UNDEAD))
			{
				note = " is immune.";
				dam = 0;
				lore_learn(m_ptr, LRN_FLAG2, RF2_UNDEAD, FALSE);
			}
			else if ((r_ptr->flags3 & (RF3_RES_NETH)) || (r_ptr->flags4 & (RF4_BR_NETH)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_NETH, FALSE);
			}
			else if (r_ptr->flags2 & (RF2_EVIL))
			{
				dam /= 2;
				note = " resists somewhat.";
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);
			}
			break;
		}

		/* Water */
		case GF_WATER:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_WATER)) || (r_ptr->flags4 & (RF4_BR_WATER)))
			{
				note = " resists a lot.";
				dam /= 9;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_WATER, FALSE);
			}
			break;
		}

		/* Chaos - Chaotic creatures resist somewhat*/
		case GF_CHAOS:
		{
			if (seen) obvious = TRUE;
			do_poly = TRUE;
			do_conf = (5 + randint(11) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_CHAO)) || (r_ptr->flags4 & (RF4_BR_CHAO)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				do_poly = FALSE;
				do_conf = 0;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_CHAO, FALSE);
			}
			else if (r_ptr->flags2 & (RF2_CHAOTIC))
			{
				note = " resists somewhat.";
				dam /= 2;
				do_poly = FALSE;
				do_conf = 0;
				lore_learn(m_ptr, LRN_FLAG2, RF2_CHAOTIC, FALSE);
			}
			break;
		}

		/* Shards */
		case GF_SHARD:
		{
			if (seen) obvious = TRUE;
			/* Sometimes cut monsters */
			if (rand_int(3)==0) do_cut = (10 + randint(15) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_SHAR)) || (r_ptr->flags4 & (RF4_BR_SHAR)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				do_cut = 0;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_SHAR, FALSE);
			}
			break;
		}

		/* Sound */
		case GF_SOUND:
		{
			if (seen) obvious = TRUE;
			do_stun = (10 + randint(15) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_SOUN)) || (r_ptr->flags4 & (RF4_BR_SOUN)))
			{
				note = " resists.";
				dam *= 2; dam /= (randint(6)+6);
				do_stun = 0;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_SOUN, FALSE);
			}
			break;
		}

		/* Confusion */
		case GF_CONFUSION:
		{
			if (seen) obvious = TRUE;
			do_conf = (10 + randint(15) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_CONF)) || (r_ptr->flags4 & (RF4_BR_CONF)))
			{
				note = " resists.";
				dam *= 2; dam /= (randint(6)+6);
				do_conf = 0;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_CONF, FALSE);
			}
			break;
		}

		/* Disenchantment */
		case GF_DISENCHANT:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_DISE)) || (r_ptr->flags4 & (RF4_BR_DISE)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_DISE, FALSE);
			}
			break;
		}

		/* Nexus */
		case GF_NEXUS:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_NEXU)) || (r_ptr->flags4 & (RF4_BR_NEXU)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_NEXU, FALSE);
			}
			break;
		}

		/* Force */
		case GF_FORCE:
		{
			if (seen) obvious = TRUE;
			do_stun = (randint(15) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_FORCE)) || (r_ptr->flags4 & (RF4_BR_FORCE)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				do_stun = 0;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_FORCE, FALSE);
			}
			break;
		}

		/* Inertia */
		case GF_INERTIA:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_INER)) || (r_ptr->flags4 & (RF4_BR_INER)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_INER, FALSE);
			}
			break;
		}

		/* Time */
		case GF_TIME:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_TIME)) || (r_ptr->flags4 & (RF4_BR_TIME)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_TIME, FALSE);
			}
			break;
		}

		/* Gravity */
		case GF_GRAVITY:
		{
			if (seen) obvious = TRUE;
			do_dist = 10;
			if ((r_ptr->flags3 & (RF3_RES_GRAV)) || (r_ptr->flags4 & (RF4_BR_GRAV)))
			{
				note = " resists.";
				dam *= 3; dam /= (randint(6)+6);
				do_dist = 0;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_GRAV, FALSE);
			}
			break;
		}

		/* Mana */
		case GF_MANA:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags3 & (RF3_RES_MANA)) || (r_ptr->flags4 & (RF4_BR_MANA)))
			{
				note = " resists somewhat.";
				dam /= 2;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_MANA, FALSE);
			}
			break;
		}

		/* Ice -- Cold + Cuts + Stun */
		case GF_ICE:
		{
			if (seen) obvious = TRUE;
			do_stun = (randint(15) + 1) / (r + 1);
			if (rand_int(3)==0) do_cut = (10 + randint(15) + r) / (r + 1);
			if ((r_ptr->flags3 & (RF3_RES_COLD)) || (r_ptr->flags4 & (RF4_BR_COLD)))
			{
				note = " resists a lot.";
				dam /= 9;
				do_cut = 0;
				do_stun = 0;
				/* Monster lore - give a resistance flag unless you know you don't need it */
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_COLD, FALSE);
			}
			break;
		}

		/* Astral */
		case GF_ASTRAL:
		{
			int amount;

			/* Skip monsters with only 1 HP - Astral attacks never kill */
			if (m_ptr->hp == 1)
			{
				note = " is unaffected";
				dam = 0;
				break;
			}

			/* Do "dam" percent damage out of total HP */
			amount = (m_ptr->hp * dam) / 100;

			if (seen) obvious = TRUE;
			if (m_ptr->u_idx)
			{
				note = " resists.";
				amount /= 3;
			}

			/* Do some damage */
			dam = ((amount > 1) ? amount : 1);

			break;
		}

		/* Drain Life */
		case GF_OLD_DRAIN:
		{
			if (seen) obvious = TRUE;
			if ((r_ptr->flags2 & (RF2_UNDEAD)) ||
			    (r_ptr->flags2 & (RF2_DEMON)) ||
			    (strchr("Egv$", r_ptr->d_char)))
			{
				if (r_ptr->flags2 & (RF2_UNDEAD))
				{
					lore_learn(m_ptr, LRN_FLAG2, RF2_UNDEAD, FALSE);
				}
				if (r_ptr->flags2 & (RF2_DEMON))
				{
					lore_learn(m_ptr, LRN_FLAG2, RF2_DEMON, FALSE);
				}

				note = " is unaffected!";
				obvious = FALSE;
				dam = 0;
			}

			break;
		}

		/* Polymorph monster (Use "dam" as "power") */
		case GF_OLD_POLY:
		{
			if (seen) obvious = TRUE;

			/* Attempt to polymorph (see below) */
			do_poly = TRUE;

			/* Powerful monsters can resist, uniques are immune */
			if ((m_ptr->u_idx) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				note = " is unaffected!";
				do_poly = FALSE;
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Clone monsters (Ignore "dam") */
		case GF_OLD_CLONE:
		{
			if (seen) obvious = TRUE;

			/* Heal fully */
			m_ptr->hp = m_ptr->maxhp;

			/* Speed up */
			if (m_ptr->mspeed < 150) m_ptr->mspeed += 10;

			/* Attempt to clone. */
			if (multiply_monster(cave_m_idx[y][x]))
			{
				note = " spawns!";
			}

			/* No "real" damage */
			dam = 0;

			break;
		}


		/* Heal Monster (use "dam" as amount of healing) */
		case GF_OLD_HEAL:
		{
			if (seen) obvious = TRUE;

			/* Wake up */
			m_ptr->csleep = 0;

			/* Heal */
			m_ptr->hp += dam;

			/* No overflow */
			if (m_ptr->hp > m_ptr->maxhp) m_ptr->hp = m_ptr->maxhp;

			/* Redraw (later) if needed */
			if (p_ptr->health_who == cave_m_idx[y][x]) p_ptr->redraw |= (PR_HEALTH);

			/* Message */
			note = " looks healthier.";

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Speed Monster (Ignore "dam") */
		case GF_OLD_SPEED:
		{
			if (seen) obvious = TRUE;

			/* Speed up */
			if (m_ptr->mspeed < 150) m_ptr->mspeed += 10;
			note = " starts moving faster.";

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Slow Monster (Use "dam" as "power") */
		case GF_OLD_SLOW:
		{
			if (seen) obvious = TRUE;

			/* Powerful monsters can resist */
			if (r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10))
			{
				note = " is unaffected!";
				obvious = FALSE;
			}

			/* Normal monsters slow down */
			else
			{
				if (m_ptr->mspeed > 60) m_ptr->mspeed -= 10;
				note = " starts moving slower.";
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Sleep (Use "dam" as "power") */
		case GF_OLD_SLEEP:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags3 & (RF3_NO_SLEEP)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_SLEEP))
				{
					lore_learn(m_ptr, LRN_FLAG3, RF3_NO_SLEEP, FALSE);
				}

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else
			{
				/* Go to sleep (much) later */
				note = " falls asleep!";
				do_sleep = 500;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Blind (Use "dam" as "power") */
		case GF_OLD_BLIND:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags3 & (RF3_NO_BLIND)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_BLIND))
				{
					lore_learn(m_ptr, LRN_FLAG3, RF3_NO_BLIND, FALSE);
				}

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else
			{
				/* blind (much) later */
				note = " is blinded!";
				do_blind = damroll(3, (dam / 2)) + 1;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Calm (Use "dam" as "power") */
		case GF_OLD_CALM:
		{
			if (seen) obvious = TRUE;

			/* Attempt a saving throw */
			if ((r_ptr->flags3 & (RF3_NO_CALM)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_NO_CALM))
				{
					lore_learn(m_ptr, LRN_FLAG3, RF3_NO_CALM, FALSE);
				}

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}
			else
			{
				/* calm (much) later */
				note = " is pacified!";
				do_calm = (2*(damroll(5, (dam / 2)) + 1)) / resist;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Confusion (Use "dam" as "power") */
		case GF_OLD_CONF:
		{
			if (seen) obvious = TRUE;

			/* Get confused later */
			do_conf = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags3 & (RF3_RES_CONF)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				/* Memorize a flag */
				if (r_ptr->flags3 & (RF3_RES_CONF))
				{
					lore_learn(m_ptr, LRN_FLAG3, RF3_RES_CONF, FALSE);
				}

				/* Resist */
				do_conf = 0;

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Lite, but only hurts susceptible creatures */
		case GF_LITE_WEAK:
		{
			/* Hurt by light */
			if (r_ptr->flags3 & (RF3_HURT_LITE))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				lore_learn(m_ptr, LRN_FLAG3, RF3_HURT_LITE, FALSE);

				/* Special effect */
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Lite -- opposite of Dark */
		case GF_LITE:
		{
			if (seen) obvious = TRUE;
			do_blind = (randint(5) + 1) / (r + 1);

			if ((r_ptr->flags3 & (RF3_RES_LITE)) || (r_ptr->flags4 & (RF4_BR_LITE)))
			{
				note = " resists.";
				dam *= 2; dam /= (randint(6)+6);
				do_blind = 0;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_LITE, FALSE);
			}
			else if (r_ptr->flags3 & (RF3_HURT_LITE))
			{
				lore_learn(m_ptr, LRN_FLAG3, RF3_HURT_LITE, FALSE);
				note = " cringes from the light!";
				note_dies = " shrivels away in the light!";
				dam *= 2;
			}
			break;
		}


		/* Dark, but only hurts susceptible creatures */
		case GF_DARK_WEAK:
		{
			/* Hurt by light */
			if (r_ptr->flags3 & (RF3_HURT_DARK))
			{
				/* Obvious effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				lore_learn(m_ptr, LRN_FLAG3, RF3_HURT_DARK, FALSE);

				/* Special effect */
				note = " cringes from the dark!";
				note_dies = " shrivels away in the dark!";
			}

			/* Normally no damage */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dark -- opposite of Lite */
		case GF_DARK:
		{
			if (seen) obvious = TRUE;
			do_blind = (randint(5) + 1) / (r + 1);

			if ((r_ptr->flags3 & (RF3_RES_DARK)) || (r_ptr->flags4 & (RF4_BR_DARK)))
			{
				note = " resists.";
				dam *= 2; dam /= (randint(6)+6);
				do_blind = 0;
				lore_learn(m_ptr, LRN_FLAG3, RF3_RES_DARK, FALSE);
			}
			else if (r_ptr->flags3 & (RF3_HURT_DARK))
			{
				lore_learn(m_ptr, LRN_FLAG3, RF3_HURT_DARK, FALSE);
				note = " cringes from the dark!";
				note_dies = " shrivels away in the dark!";
				dam *= 2;
			}
			break;
		}

		/* Stone to Mud */
		case GF_KILL_WALL:
		{
			/* Hurt by rock remover */
			if (r_ptr->flags3 & (RF3_HURT_ROCK))
			{
				/* Notice effect */
				if (seen) obvious = TRUE;

				/* Memorize the effects */
				lore_learn(m_ptr, LRN_FLAG3, RF3_HURT_ROCK, FALSE);

				/* Cute little message */
				note = " loses some skin!";
				note_dies = " dissolves!";
			}

			/* Usually, ignore the effects */
			else
			{
				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Teleport undead (Use "dam" as "power") */
		case GF_AWAY_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_UNDEAD))
			{
				if (seen) obvious = TRUE;
				lore_learn(m_ptr, LRN_FLAG2, RF2_UNDEAD, FALSE);
				do_dist = dam;
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport evil (Use "dam" as "power") */
		case GF_AWAY_EVIL:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_EVIL))
			{
				if (seen) obvious = TRUE;
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);
				do_dist = dam;
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Teleport monster (Use "dam" as "power") */
		case GF_AWAY_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Prepare to teleport */
			do_dist = dam;

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Call monster */
		case GF_CALL_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Prepare to teleport */
			do_call = TRUE;

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Turn undead (Use "dam" as "power") */
		case GF_TURN_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_UNDEAD))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_UNDEAD, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (r_ptr->level > randint((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				{
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn evil (Use "dam" as "power") */
		case GF_TURN_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags2 & (RF2_EVIL))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Apply some fear */
				do_fear = damroll(3, (dam / 2)) + 1;

				/* Attempt a saving throw */
				if (r_ptr->level > randint((dam - 10) < 1 ? 1 : (dam - 10)) + 10)
				{
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
					do_fear = 0;
				}
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}


		/* Turn monster (Use "dam" as "power") */
		case GF_TURN_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Apply some fear */
			do_fear = damroll(3, (dam / 2)) + 1;

			/* Attempt a saving throw */
			if ((r_ptr->flags3 & (RF3_NO_FEAR)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
			{
				lore_learn(m_ptr, LRN_FLAG3, RF3_NO_FEAR, FALSE);

				/* No obvious effect */
				note = " is unaffected!";
				obvious = FALSE;
				do_fear = 0;
			}

			/* No "real" damage */
			dam = 0;
			break;
		}

		/* Dispel animals */
		case GF_DISP_ANIMAL:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_ANIMAL))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_ANIMAL, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel animals */
		case GF_DISP_PLANT:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_PLANT))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_PLANT, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel undead */
		case GF_DISP_UNDEAD:
		{
			/* Only affect undead */
			if (r_ptr->flags2 & (RF2_UNDEAD))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_UNDEAD, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel evil */
		case GF_DISP_EVIL:
		{
			/* Only affect evil */
			if (r_ptr->flags2 & (RF2_EVIL))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel non-evil */
		case GF_DISP_NON_EVIL:
		{
			/* Only affect non-evil */
			if (!(r_ptr->flags2 & (RF2_EVIL)))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Message */
				note = " shudders.";
				note_dies = " dissolves!";
			}

			/* Others ignore */
			else
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);

				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Dispel monster */
		case GF_DISP_ALL:
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Message */
			note = " shudders.";
			note_dies = " dissolves!";

			break;
		}

		/* Calm Animals */
		case GF_CALM_ANIMALS:
		{
			/* Only affect animals */
			if (r_ptr->flags2 & (RF2_ANIMAL))
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_ANIMAL, FALSE);

				/* Obvious */
				if (seen) obvious = TRUE;

				/* Attempt a saving throw */
				if ((r_ptr->flags3 & (RF3_NO_CALM)) ||
				(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
				{
					/* Memorize a flag */
					if (r_ptr->flags3 & (RF3_NO_CALM))
					{
						lore_learn(m_ptr, LRN_FLAG3, RF3_NO_CALM, FALSE);
					}
	
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
				}
				else
				{
					/* calm (much) later */
					note = " is pacified!";
					do_calm = (2*(damroll(5, (dam / 2)) + 1)) / resist;
				}

				/* No "real" damage */
				dam = 0;
			}

			/* Others ignore */
			else
			{
				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Calm non-evil */
		case GF_CALM_NON_EVIL:
		{
			/* Only affect animals */
			if (!(r_ptr->flags2 & (RF2_EVIL)))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Attempt a saving throw */
				if ((r_ptr->flags3 & (RF3_NO_CALM)) ||
					(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
				{
					/* Memorize a flag */
					if (r_ptr->flags3 & (RF3_NO_CALM))
					{
						lore_learn(m_ptr, LRN_FLAG3, RF3_NO_CALM, FALSE);
					}
	
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
				}
				else
				{
					/* calm (much) later */
					note = " is pacified!";
					do_calm = (2*(damroll(5, (dam / 2)) + 1)) / resist;
				}

				/* No "real" damage */
				dam = 0;
			}

			/* Others ignore */
			else
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_EVIL, FALSE);

				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Calm non chaos */
		case GF_CALM_NON_CHAOS:
		{
			/* Only affect animals */
			if (!(r_ptr->flags2 & (RF2_CHAOTIC)))
			{
				/* Obvious */
				if (seen) obvious = TRUE;

				/* Attempt a saving throw */
				if ((r_ptr->flags3 & (RF3_NO_CALM)) ||
					(r_ptr->level + resist > rand_int((dam > 5) ? dam * 2 : 10)))
				{
					/* Memorize a flag */
					if (r_ptr->flags3 & (RF3_NO_CALM))
					{
						lore_learn(m_ptr, LRN_FLAG3, RF3_NO_CALM, FALSE);
					}
	
					/* No obvious effect */
					note = " is unaffected!";
					obvious = FALSE;
				}
				else
				{
					/* calm (much) later */
					note = " is pacified!";
					do_calm = (2*(damroll(5, (dam / 2)) + 1)) / resist;
				}

				/* No "real" damage */
				dam = 0;
			}

			/* Others ignore */
			else
			{
				/* Learn about type */
				lore_learn(m_ptr, LRN_FLAG2, RF2_CHAOTIC, FALSE);

				/* Irrelevant */
				skipped = TRUE;

				/* No damage */
				dam = 0;
			}

			break;
		}

		/* Default */
		default:
		{
			/* Irrelevant */
			skipped = TRUE;

			/* No damage */
			dam = 0;

			break;
		}
	}

	/* Absolutely no effect */
	if (skipped) return (FALSE);

	if (m_ptr->u_idx) 
	{
		/* "Unique" monsters cannot be polymorphed */
		do_poly = FALSE;

		/* Uniques may only be killed by the player */
		if ((who > 0) && (dam > m_ptr->hp)) dam = m_ptr->hp;
	}

	/* Poison */
	if (do_pois &&
	    !(r_ptr->flags3 & (RF3_RES_POIS)) &&
	    !(r_ptr->flags4 & (RF4_BR_POIS)))
	{
		/* Already partially poisoned */
		if (m_ptr->poisoned) note = " is more poisoned.";
		/* Was not poisoned */
		else note = " is poisoned.";
		m_ptr->poisoned += do_pois;
	}

	/* Cut */
	if (do_cut &&
	    !(r_ptr->flags3 & (RF3_NO_CUT)))
	{
		/* Already partially bleeding */
		if (m_ptr->bleeding) note = " is bleeding more strongly.";
		/* Was not poisoned */
		else note = " is bleeding.";
		m_ptr->bleeding += do_cut;
	}

	/* Check for death */
	if (dam > m_ptr->hp)
	{
		/* Extract method of death */
		note = note_dies;
	}

	/* Mega-Hack -- Handle "polymorph" */
	else if (do_poly && (randint(90) > r_ptr->level))
	{
		/* Default -- assume no polymorph */
		note = " is unaffected!";

		/* Pick a "new" monster race */
		tmp = poly_r_idx(m_ptr->r_idx, dam);

		/* Handle polymorh */
		if (tmp != m_ptr->r_idx)
		{
			/* Obvious */
			if (seen) obvious = TRUE;

			/* Monster polymorphs */
			note = " changes!";

			/* Turn off the damage */
			dam = 0;

			/* "Kill" the "old" monster */
			delete_monster_idx(cave_m_idx[y][x]);

			/* Create a new monster (no groups) */
			(void)place_monster_aux(y, x, tmp, FALSE, FALSE, PLACE_NO_U);

			/* Hack -- Assume success XXX XXX XXX */

			/* Hack -- Get new monster */
			m_ptr = &m_list[cave_m_idx[y][x]];

			/* Hack -- Get new race */
			r_ptr = &r_info[m_ptr->r_idx];
		}
	}

	/* Handle "call" */
	else if (do_call)
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Message */
		note = " is brought forth";

		/* Teleport */
		teleport_monster_to(cave_m_idx[y][x], p_ptr->py, p_ptr->px);

		/* Hack -- get new location */
		y = m_ptr->fy;
		x = m_ptr->fx;
	}

	/* Handle "teleport" */
	else if (do_dist)
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Message */
		note = " disappears!";

		/* Teleport */
		teleport_away(cave_m_idx[y][x], do_dist);

		/* Hack -- get new location */
		y = m_ptr->fy;
		x = m_ptr->fx;
	}

	else if (do_stun &&
	         !(r_ptr->flags3 & (RF3_NO_STUN)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Get confused */
		if (m_ptr->stunned)
		{
			note = " is more dazed.";
			tmp = m_ptr->stunned + (do_stun / 2);
		}
		else
		{
			note = " is dazed.";
			tmp = do_stun;
		}

		/* Apply stun */
		m_ptr->stunned = (tmp < 200) ? tmp : 200;
	}

	/* Confusion breathers (and sleepers) never confuse */
	else if (do_conf &&
	         !(r_ptr->flags3 & (RF3_RES_CONF)) &&
	         !(r_ptr->flags4 & (RF4_BR_CONF)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Already partially confused */
		if (m_ptr->confused)
		{
			note = " looks more confused.";
			tmp = m_ptr->confused + (do_conf / 2);
		}

		/* Was not confused */
		else
		{
			note = " looks confused.";
			tmp = do_conf;
		}

		/* Apply confusion */
		m_ptr->confused = (tmp < 200) ? tmp : 200;
	}

	/* Fear */
	if (do_fear)
	{
		/* Increase fear */
		tmp = m_ptr->monfear + do_fear;

		/* Set fear */
		m_ptr->monfear = (tmp < 200) ? tmp : 200;
	}

	/* Blindness */
	if (do_blind && !(r_ptr->flags3 & (RF3_NO_BLIND)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Already partially blinded */
		if (m_ptr->blinded)
		{
			note = " looks more blinded.";
			tmp = m_ptr->blinded + (do_blind / 2);
		}

		/* Was not blinded */
		else
		{
			note = " looks blinded.";
			tmp = do_blind;
		}

		/* Apply blindness */
		m_ptr->blinded = (tmp < 200) ? tmp : 200;
	}

	/* If another monster did the damage, hurt the monster by hand */
	if (who > 0)
	{
		/* Redraw (later) if needed */
		if (p_ptr->health_who == cave_m_idx[y][x]) p_ptr->redraw |= (PR_HEALTH);

		/* Wake the monster up */
		m_ptr->csleep = 0;

		/* Hurt the monster */
		m_ptr->hp -= dam;

		/* Dead monster */
		if (m_ptr->hp < 0)
		{
			/* Generate treasure, etc */
			monster_death(cave_m_idx[y][x]);

			/* Delete the monster */
			delete_monster_idx(cave_m_idx[y][x]);

			/* Give detailed messages if destroyed */
			if (note) message_format(MSG_MONSTER, m_ptr->r_idx, "%^s%s", m_name, note);
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen) message_format(MSG_MONSTER, m_ptr->r_idx, "%^s%s", m_name, note);

			/* Hack -- Pain message */
			else if (dam > 0) message_pain(cave_m_idx[y][x], dam);

			/* Hack -- handle sleep */
			if (do_sleep) m_ptr->csleep = do_sleep;
		}
	}

	/* If the player did it, give him experience, check fear */
	else
	{
		bool fear = FALSE;

		/* Hurt the monster, check for fear and death */
		if (mon_take_hit(cave_m_idx[y][x], dam, &fear, note_dies))
		{
			/* Dead monster */
		}

		/* Damaged monster */
		else
		{
			/* Give detailed messages if visible or destroyed */
			if (note && seen) message_format(MSG_MONSTER, m_ptr->r_idx, "%^s%s", m_name, note);

			/* Hack -- Pain message */
			else if (dam > 0) message_pain(cave_m_idx[y][x], dam);

			/* Take note */
			if ((fear || do_fear) && (m_ptr->ml))
			{
				/* Message */
				message_format(MSG_FLEE, m_ptr->r_idx, "%^s flees in terror!", m_name);
			}

			/* Hack -- handle sleep */
			if (do_sleep) m_ptr->csleep = do_sleep;
		}
	}

	/* Hack - Passivity */
	if (do_calm && !(r_ptr->flags3 & (RF3_NO_CALM)))
	{
		/* Obvious */
		if (seen) obvious = TRUE;

		/* Already partially pacified */
		if (m_ptr->calmed)
		{
			note = " looks calmer.";
			tmp = m_ptr->calmed + (do_calm / 2);
		}

		/* Was not pacified */
		else
		{
			note = " looks calm.";
			tmp = do_calm;
		}

		/* Apply passivity */
		m_ptr->calmed = (tmp < 500) ? tmp : 500;
	}

	/* Verify this code XXX XXX XXX */

	/* Update the monster */
	update_mon(cave_m_idx[y][x], FALSE);

	/* Redraw the monster grid */
	lite_spot(y, x);

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}

	/* Track it */
	project_m_n++;
	project_m_x = x;
	project_m_y = y;

	/* Return "Anything seen?" */
	return (obvious);
}

/*
 * Helper function for "project()" below.
 *
 * Handle a beam/bolt/ball causing damage to the player.
 *
 * This routine takes a "source monster" (by index), a "distance", a default
 * "damage", and a "damage type".  See "project_m()" above.
 *
 * If "rad" is non-zero, then the blast was centered elsewhere, and the damage
 * is reduced (see "project_m()" above).  This can happen if a monster breathes
 * at the player and hits a wall instead.
 *
 * We return "TRUE" if any "obvious" effects were observed.
 *
 * Actually, for historical reasons, we just assume that the effects were
 * obvious.  XXX XXX XXX
 */
static bool project_p(int who, int r, int y, int x, int dam, int typ)
{
	int k = 0;

	/* Hack -- assume obvious */
	bool obvious = TRUE;

	/* Player blind-ness */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Player needs a "description" (he is blind) */
	bool fuzzy = FALSE;

	/* Source monster */
	monster_type *m_ptr;

	/* Monster name (for attacks) */
	char m_name[80];

	/* Monster name (for damage) */
	char killer[80];

	/* Hack -- messages */
	cptr act = NULL;

	/* No player here */
	if (!(cave_m_idx[y][x] < 0)) return (FALSE);

	/* Never affect projector */
	if (cave_m_idx[y][x] == who) return (FALSE);

	/* Limit maximum damage XXX XXX XXX */
	if (dam > 1600) dam = 1600;

	/* Reduce damage by distance */
	dam = (dam + r) / (r + 1);

	/* If the player is blind, be more descriptive */
	if (blind) fuzzy = TRUE;

	/* Get the source monster */
	m_ptr = &m_list[who];

	/* Get the monster name */
	monster_desc(m_name, m_ptr, 0);

	/* Get the monster's real name */
	monster_desc(killer, m_ptr, 0x88);

	/* Analyze the damage */
	switch (typ)
	{
		/* Standard damage -- hurts inventory too */
		case GF_ACID:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by acid!");
			acid_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_FIRE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by fire!");
			fire_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_COLD:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by cold!");
			cold_dam(dam, killer);
			break;
		}

		/* Standard damage -- hurts inventory too */
		case GF_ELEC:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by lightning!");
			elec_dam(dam, killer);
			break;
		}

		/* Standard damage -- also poisons player */
		case GF_POIS:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by poison!");
			if (p_ptr->resist_pois) dam = (dam + 2) / 3;
			if (p_ptr->oppose_pois) dam = (dam + 2) / 3;
			take_hit(dam, killer);
			if (!(p_ptr->resist_pois || p_ptr->oppose_pois))
			{
				(void)set_poisoned(p_ptr->poisoned + rand_int(dam) + 10);
			}
			break;
		}

		/* Standard damage -- also infects player with disease*/
		case GF_DISEASE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by disease!");
			if (p_ptr->resist_disease) dam = (dam + 2) / 3;
			if (p_ptr->oppose_disease) dam = (dam + 2) / 3;

			take_hit(dam, killer);
			if (!(p_ptr->resist_disease || p_ptr->oppose_disease))
			{
				(void)set_diseased(p_ptr->diseased + (rand_int(dam) * 3));
			}
			break;
		}

		/* Standard damage */
		case GF_MISSILE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			take_hit(dam, killer);
			break;
		}

		/* Holy Orb -- Player only takes partial damage */
		case GF_HOLY_ORB:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			dam /= 2;
			take_hit(dam, killer);
			break;
		}

		/* Arrow -- no dodging XXX */
		case GF_ARROW:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something sharp!");
			take_hit(dam, killer);
			break;
		}

		/* Plasma -- No resist XXX */
		case GF_PLASMA:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			take_hit(dam, killer);
			if (!p_ptr->resist_sound)
			{
				int k = (randint((dam > 40) ? 35 : (dam * 3 / 4 + 5)));
				(void)set_stun(p_ptr->stun + k);
			}
			break;
		}

		/* Nether -- drain experience */
		case GF_NETHER:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			if (p_ptr->resist_nethr)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				if (p_ptr->hold_life && (rand_int(100) < 75))
				{
					message(MSG_RESIST, 0, "You keep hold of your life force!");
				}
				else if (p_ptr->hold_life)
				{
					message(MSG_EFFECT, 0, "You feel your life slipping away!");
					lose_exp(200 + (p_ptr->exp/500));
				}
				else
				{
					message(MSG_EFFECT, 0, "You feel your life draining away!");
					lose_exp(200 + (p_ptr->exp/50));
				}
			}
			take_hit(dam, killer);
			break;
		}

		/* Water -- stun/confuse */
		case GF_WATER:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_water)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				if (!p_ptr->resist_sound)
				{
					(void)set_stun(p_ptr->stun + randint(40));
				}
				if (!p_ptr->resist_confu)
				{
					(void)set_confused(p_ptr->confused + randint(5) + 5);
				}
			}
			take_hit(dam, killer);
			break;
		}

		/* Chaos -- many effects */
		case GF_CHAOS:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			if (p_ptr->resist_chaos)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				(void)set_image(p_ptr->image + randint(10));

				if (!p_ptr->resist_confu)
				{
					(void)set_confused(p_ptr->confused + rand_int(20) + 10);
				}
				if (!p_ptr->resist_nethr)
				{
					if (p_ptr->hold_life && (rand_int(100) < 75))
					{
						message(MSG_RESIST, 0, "You keep hold of your life force!");
					}
					else if (p_ptr->hold_life)
					{
						message(MSG_EFFECT, 0, "You feel your life slipping away!");
						lose_exp(500 + (p_ptr->exp/500));
					}
					else
					{
						message(MSG_EFFECT, 0, "You feel your life draining away!");
						lose_exp(5000 + (p_ptr->exp/50));
					}
				}
			}
			take_hit(dam, killer);
			break;
		}

		/* Shards -- mostly cutting */
		case GF_SHARD:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something sharp!");
			if (p_ptr->resist_shard)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				(void)set_cut(p_ptr->cut + dam);
			}
			take_hit(dam, killer);
			break;
		}

		/* Sound -- mostly stunning */
		case GF_SOUND:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_sound)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				int k = (randint((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_stun(p_ptr->stun + k);
			}
			take_hit(dam, killer);
			break;
		}

		/* Pure confusion */
		case GF_CONFUSION:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_confu)
			{
				dam *= 5; dam /= (randint(6) + 6);
			}
			if (!p_ptr->resist_confu)
			{
				(void)set_confused(p_ptr->confused + randint(20) + 10);
			}
			take_hit(dam, killer);
			break;
		}

		/* Disenchantment -- see above */
		case GF_DISENCHANT:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			if (p_ptr->resist_disen)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}
			else
			{
				(void)apply_disenchant();
			}
			take_hit(dam, killer);
			break;
		}

		/* Nexus -- see above */
		case GF_NEXUS:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			if (p_ptr->resist_nexus)
			{
				dam *= 5; dam /= (randint(6) + 6);
			}
			else
			{
				apply_nexus(m_ptr);
			}
			take_hit(dam, killer);
			break;
		}

		/* Force -- mostly stun */
		case GF_FORCE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint(20));
			}
			take_hit(dam, killer);
			break;
		}

		/* Inertia -- slowness */
		case GF_INERTIA:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			(void)set_slow(p_ptr->slow + rand_int(4) + 4);
			take_hit(dam, killer);
			break;
		}

		/* Lite -- blinding */
		case GF_LITE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_lite)
			{
				dam *= 4; dam /= (randint(6) + 6);
			}
			else if (!blind && !p_ptr->no_blind)
			{
				(void)set_blind(p_ptr->blind + randint(5) + 2);
			}
			take_hit(dam, killer);
			break;
		}

		/* Dark -- blinding */
		case GF_DARK:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_dark)
			{
				dam *= 4; dam /= (randint(6) + 6);
			}
			else if (!blind && !p_ptr->no_blind)
			{
				(void)set_blind(p_ptr->blind + randint(5) + 2);
			}
			take_hit(dam, killer);
			break;
		}

		/* Time -- powerful draining */
		case GF_TIME:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			if (p_ptr->resist_time)
			{
				dam *= 6; dam /= (randint(6) + 6);
			}

			else switch (randint(10))
			{
				case 1: case 2: case 3: case 4: case 5:
				{
					message(MSG_EFFECT, 0, "You feel life has clocked back.");
					lose_exp(100 + (p_ptr->exp / 50));
					break;
				}

				case 6: case 7: case 8: case 9:
				{
					k = rand_int(A_MAX);

					do_dec_stat(k, 1 + (p_ptr->stat_cur[k] / 4), FALSE, FALSE);
					p_ptr->update |= (PU_BONUS);
					break;
				}

				case 10:
				{
					for (k = 0; k < A_MAX; k++)
					{
						do_dec_stat(k, 1 + (p_ptr->stat_cur[k] / 4), FALSE, FALSE);
					}
					p_ptr->update |= (PU_BONUS);
					break;
				}
			}
			take_hit(dam, killer);
			break;
		}

		/* Gravity -- stun plus slowness plus teleport */
		case GF_GRAVITY:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something strange!");
			message(MSG_EFFECT, 0, "Gravity warps around you.");
			if (!p_ptr->ffall) teleport_player(5);
			(void)set_slow(p_ptr->slow + rand_int(4) + 4);
			if (!p_ptr->resist_sound)
			{
				int k = (randint((dam > 90) ? 35 : (dam / 3 + 5)));
				(void)set_stun(p_ptr->stun + k);
			}
			take_hit(dam, killer);
			break;
		}

		/* Pure damage */
		case GF_MANA:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something!");
			if (p_ptr->resist_mana)
			{
				dam *= 7; dam /= (randint(6) + 6);
			}
			take_hit(dam, killer);
			break;
		}

		/* Ice -- cold plus stun plus cuts */
		case GF_ICE:
		{
			if (fuzzy) message(MSG_EFFECT, 0, "You are hit by something sharp!");
			cold_dam(dam, killer);
			if (!p_ptr->resist_shard)
			{
				(void)set_cut(p_ptr->cut + damroll(5, 8));
			}
			if (!p_ptr->resist_sound)
			{
				(void)set_stun(p_ptr->stun + randint(15));
			}
			break;
		}


		/* Default */
		default:
		{
			/* No damage */
			dam = 0;

			break;
		}
	}

	/* Disturb */
	disturb(1);

	/* Return "Anything seen?" */
	return (obvious);
}

/*
 * Generic "beam"/"bolt"/"ball" projection routine.
 *
 * Input:
 *   who: Index of "source" monster (negative for "player")
 *   rad: Radius of explosion (0 = beam/bolt, 1 to 9 = ball)
 *   y,x: Target location (or location to travel "towards")
 *   dam: Base damage roll to apply to affected monsters (or player)
 *   typ: Type of damage to apply to monsters (and objects)
 *   flg: Extra bit flags (see PROJECT_xxxx in "defines.h")
 *
 * Return:
 *   TRUE if any "effects" of the projection were observed, else FALSE
 *
 * Allows a monster (or player) to project a beam/bolt/ball of a given kind
 * towards a given location (optionally passing over the heads of interposing
 * monsters), and have it do a given amount of damage to the monsters (and
 * optionally objects) within the given radius of the final location.
 *
 * A "bolt" travels from source to target and affects only the target grid.
 * A "beam" travels from source to target, affecting all grids passed through.
 * A "ball" travels from source to the target, exploding at the target, and
 *   affecting everything within the given radius of the target location.
 *
 * Traditionally, a "bolt" does not affect anything on the ground, and does
 * not pass over the heads of interposing monsters, much like a traditional
 * missile, and will "stop" abruptly at the "target" even if no monster is
 * positioned there, while a "ball", on the other hand, passes over the heads
 * of monsters between the source and target, and affects everything except
 * the source monster which lies within the final radius, while a "beam"
 * affects every monster between the source and target, except for the casting
 * monster (or player), and rarely affects things on the ground.
 *
 * Two special flags allow us to use this function in special ways, the
 * "PROJECT_HIDE" flag allows us to perform "invisible" projections, while
 * the "PROJECT_JUMP" flag allows us to affect a specific grid, without
 * actually projecting from the source monster (or player).
 *
 * The player will only get "experience" for monsters killed by himself
 * Unique monsters can only be destroyed by attacks from the player
 *
 * Only 256 grids can be affected per projection, limiting the effective
 * "radius" of standard ball attacks to nine units (diameter nineteen).
 *
 * One can project in a given "direction" by combining PROJECT_THRU with small
 * offsets to the initial location (see "line_spell()"), or by calculating
 * "virtual targets" far away from the player.
 *
 * One can also use PROJECT_THRU to send a beam/bolt along an angled path,
 * continuing until it actually hits somethings (useful for "stone to mud").
 *
 * Bolts and Beams explode INSIDE walls, so that they can destroy doors.
 *
 * Balls must explode BEFORE hitting walls, or they would affect monsters
 * on both sides of a wall.  Some bug reports indicate that this is still
 * happening in 2.7.8 for Windows, though it appears to be impossible.
 *
 * We "pre-calculate" the blast area only in part for efficiency.
 * More importantly, this lets us do "explosions" from the "inside" out.
 * This results in a more logical distribution of "blast" treasure.
 * It also produces a better (in my opinion) animation of the explosion.
 * It could be (but is not) used to have the treasure dropped by monsters
 * in the middle of the explosion fall "outwards", and then be damaged by
 * the blast as it spreads outwards towards the treasure drop location.
 *
 * Walls and doors are included in the blast area, so that they can be
 * "burned" or "melted" in later versions.
 *
 * This algorithm is intended to maximize simplicity, not necessarily
 * efficiency, since this function is not a bottleneck in the code.
 *
 * We apply the blast effect from ground zero outwards, in several passes,
 * first affecting features, then objects, then monsters, then the player.
 * This allows walls to be removed before checking the object or monster
 * in the wall, and protects objects which are dropped by monsters killed
 * in the blast, and allows the player to see all affects before he is
 * killed or teleported away.  The semantics of this method are open to
 * various interpretations, but they seem to work well in practice.
 *
 * We process the blast area from ground-zero outwards to allow for better
 * distribution of treasure dropped by monsters, and because it provides a
 * pleasing visual effect at low cost.
 *
 * Note that the damage done by "ball" explosions decreases with distance.
 * This decrease is rapid, grids at radius "dist" take "1/dist" damage.
 *
 * Notice the "napalm" effect of "beam" weapons.  First they "project" to
 * the target, and then the damage "flows" along this beam of destruction.
 * The damage at every grid is the same as at the "center" of a "ball"
 * explosion, since the "beam" grids are treated as if they ARE at the
 * center of a "ball" explosion.
 *
 * Currently, specifying "beam" plus "ball" means that locations which are
 * covered by the initial "beam", and also covered by the final "ball", except
 * for the final grid (the epicenter of the ball), will be "hit twice", once
 * by the initial beam, and once by the exploding ball.  For the grid right
 * next to the epicenter, this results in 150% damage being done.  The center
 * does not have this problem, for the same reason the final grid in a "beam"
 * plus "bolt" does not -- it is explicitly removed.  Simply removing "beam"
 * grids which are covered by the "ball" will NOT work, as then they will
 * receive LESS damage than they should.  Do not combine "beam" with "ball".
 *
 * The array "gy[],gx[]" with current size "grids" is used to hold the
 * collected locations of all grids in the "blast area" plus "beam path".
 *
 * Note the rather complex usage of the "gm[]" array.  First, gm[0] is always
 * zero.  Second, for N>1, gm[N] is always the index (in gy[],gx[]) of the
 * first blast grid (see above) with radius "N" from the blast center.  Note
 * that only the first gm[1] grids in the blast area thus take full damage.
 * Also, note that gm[rad+1] is always equal to "grids", which is the total
 * number of blast grids.
 *
 * Note that once the projection is complete, (y2,x2) holds the final location
 * of bolts/beams, and the "epicenter" of balls.
 *
 * Note also that "rad" specifies the "inclusive" radius of projection blast,
 * so that a "rad" of "one" actually covers 5 or 9 grids, depending on the
 * implementation of the "distance" function.  Also, a bolt can be properly
 * viewed as a "ball" with a "rad" of "zero".
 *
 * Note that if no "target" is reached before the beam/bolt/ball travels the
 * maximum distance allowed (MAX_RANGE), no "blast" will be induced.  This
 * may be relevant even for bolts, since they have a "1x1" mini-blast.
 *
 * Note that for consistency, we "pretend" that the bolt actually takes "time"
 * to move from point A to point B, even if the player cannot see part of the
 * projection path.  Note that in general, the player will *always* see part
 * of the path, since it either starts at the player or ends on the player.
 *
 * Hack -- we assume that every "projection" is "self-illuminating".
 *
 * Hack -- when only a single monster is affected, we automatically track
 * (and recall) that monster, unless "PROJECT_JUMP" is used.
 *
 * Note that all projections now "explode" at their final destination, even
 * if they were being projected at a more distant destination.  This means
 * that "ball" spells will *always* explode.
 *
 * Note that we must call "handle_stuff()" after affecting terrain features
 * in the blast radius, in case the "illumination" of the grid was changed,
 * and "update_view()" and "update_monsters()" need to be called.
 */
bool project(int who, int rad, int y, int x, int dam, int typ, int flg)
{
	int i, t, dist;

	int y1, x1;
	int y2, x2;

	int msec = op_ptr->delay_factor * op_ptr->delay_factor;

	/* Assume the player sees nothing */
	bool notice = FALSE;

	/* Assume the player has seen nothing */
	bool visual = FALSE;

	/* Assume the player has seen no blast grids */
	bool drawn = FALSE;

	/* Is the player blind? */
	bool blind = (p_ptr->blind ? TRUE : FALSE);

	/* Number of grids in the "path" */
	int path_n = 0;

	/* Actual grids in the "path" */
	u16b path_g[512];

	/* Number of grids in the "blast area" (including the "beam" path) */
	int grids = 0;

	/* Coordinates of the affected grids */
	byte gx[256], gy[256];

	/* Encoded "radius" info (see above) */
	byte gm[16];


	/* Hack -- Jump to target */
	if (flg & (PROJECT_JUMP))
	{
		x1 = x;
		y1 = y;

		/* Clear the flag */
		flg &= ~(PROJECT_JUMP);
	}

	/* Start at player */
	else if (who < 0)
	{
		x1 = p_ptr->px;
		y1 = p_ptr->py;
	}

	/* Start at monster */
	else if (who > 0)
	{
		x1 = m_list[who].fx;
		y1 = m_list[who].fy;
	}

	/* Oops */
	else
	{
		x1 = x;
		y1 = y;
	}

	/* Default "destination" */
	y2 = y;
	x2 = x;

	/* Hack -- verify stuff */
	if (flg & (PROJECT_THRU))
	{
		if ((x1 == x2) && (y1 == y2))
		{
			flg &= ~(PROJECT_THRU);
		}
	}

	/* Hack -- Assume there will be no blast (max radius 16) */
	for (dist = 0; dist < 16; dist++) gm[dist] = 0;

	/* Initial grid */
	y = y1;
	x = x1;

	/* Collect beam grids */
	if (flg & (PROJECT_BEAM))
	{
		gy[grids] = y;
		gx[grids] = x;
		grids++;
	}

	/* Calculate the projection path */
	path_n = project_path(path_g, MAX_RANGE, y1, x1, y2, x2, flg);

	/* Hack -- Handle stuff */
	handle_stuff();

	/* Project along the path */
	for (i = 0; i < path_n; ++i)
	{
		int oy = y;
		int ox = x;

		int ny = GRID_Y(path_g[i]);
		int nx = GRID_X(path_g[i]);

		/* Hack -- Balls explode before reaching walls */
		if (!cave_floor_bold(ny, nx) && (rad > 0)) break;

		/* Advance */
		y = ny;
		x = nx;

		/* Collect beam grids */
		if (flg & (PROJECT_BEAM))
		{
			gy[grids] = y;
			gx[grids] = x;
			grids++;
		}

		/* Only do visuals if requested */
		if (!blind && !(flg & (PROJECT_HIDE)))
		{
			/* Only do visuals if the player can "see" the bolt */
			if (panel_contains(y, x) && player_has_los_bold(y, x))
			{
				u16b p;

				byte a;
				char c;

				/* Obtain the bolt pict */
				p = bolt_pict(oy, ox, y, x, typ);

				/* Extract attr/char */
				a = PICT_A(p);
				c = PICT_C(p);

				/* Visual effects */
				print_rel(c, a, y, x);
				move_cursor_relative(y, x);
				if (fresh_before) Term_fresh();
				Term_xtra(TERM_XTRA_DELAY, msec);
				lite_spot(y, x);
				if (fresh_before) Term_fresh();

				/* Display "beam" grids */
				if (flg & (PROJECT_BEAM))
				{
					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects */
					print_rel(c, a, y, x);
				}

				/* Hack -- Activate delay */
				visual = TRUE;
			}

			/* Hack -- delay anyway for consistency */
			else if (visual)
			{
				/* Delay for consistency */
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}
	}


	/* Save the "blast epicenter" */
	y2 = y;
	x2 = x;

	/* Start the "explosion" */
	gm[0] = 0;

	/* Hack -- make sure beams get to "explode" */
	gm[1] = grids;

	/* Explode */
	/* Hack -- remove final beam grid */
	if (flg & (PROJECT_BEAM))
	{
		grids--;
	}

	/* Determine the blast area, work from the inside out */
	for (dist = 0; dist <= rad; dist++)
	{
		/* Scan the maximal blast area of radius "dist" */
		for (y = y2 - dist; y <= y2 + dist; y++)
		{
			for (x = x2 - dist; x <= x2 + dist; x++)
			{
				/* Ignore "illegal" locations */
				if (!in_bounds(y, x)) continue;

				/* Enforce a "circular" explosion */
				if (distance(y2, x2, y, x) != dist) continue;

				/* Ball explosions are stopped by walls */
				if (!los(y2, x2, y, x)) continue;

				/* Save this grid */
				gy[grids] = y;
				gx[grids] = x;
				grids++;
			}
		}

		/* Encode some more "radius" info */
		gm[dist+1] = grids;
	}

	/* Speed -- ignore "non-explosions" */
	if (!grids) return (FALSE);

	/* Display the "blast area" if requested */
	if (!blind && !(flg & (PROJECT_HIDE)))
	{
		/* Then do the "blast", from inside out */
		for (t = 0; t <= rad; t++)
		{
			/* Dump everything with this radius */
			for (i = gm[t]; i < gm[t+1]; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Only do visuals if the player can "see" the blast */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					u16b p;

					byte a;
					char c;

					drawn = TRUE;

					/* Obtain the explosion pict */
					p = bolt_pict(y, x, y, x, typ);

					/* Extract attr/char */
					a = PICT_A(p);
					c = PICT_C(p);

					/* Visual effects -- Display */
					print_rel(c, a, y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(y2, x2);

			/* Flush each "radius" seperately */
			if (fresh_before) Term_fresh();

			/* Delay (efficiently) */
			if (visual || drawn)
			{
				Term_xtra(TERM_XTRA_DELAY, msec);
			}
		}

		/* Flush the erasing */
		if (drawn)
		{
			/* Erase the explosion drawn above */
			for (i = 0; i < grids; i++)
			{
				/* Extract the location */
				y = gy[i];
				x = gx[i];

				/* Hack -- Erase if needed */
				if (panel_contains(y, x) && player_has_los_bold(y, x))
				{
					lite_spot(y, x);
				}
			}

			/* Hack -- center the cursor */
			move_cursor_relative(y2, x2);

			/* Flush the explosion */
			if (fresh_before) Term_fresh();
		}
	}

	/* Check features */
	if (flg & (PROJECT_GRID))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for features */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the feature in that grid */
			if (project_f(who, dist, y, x, dam, typ)) notice = TRUE;
		}
	}

	/* Update stuff if needed */
		update_stuff();

	/* Check objects */
	if (flg & (PROJECT_ITEM))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for objects */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the object in the grid */
			if (project_o(who, dist, y, x, dam, typ)) notice = TRUE;
		}
	}

	/* Check monsters */
	if (flg & (PROJECT_KILL))
	{
		/* Mega-Hack */
		project_m_n = 0;
		project_m_x = 0;
		project_m_y = 0;

		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for monsters */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the monster in the grid */
			if (project_m(who, dist, y, x, dam, typ)) notice = TRUE;
		}

		/* Player affected one monster (without "jumping") */
		if ((who < 0) && (project_m_n == 1) && !(flg & (PROJECT_JUMP)))
		{
			/* Location */
			x = project_m_x;
			y = project_m_y;

			/* Track if possible */
			if (cave_m_idx[y][x] > 0)
			{
				monster_type *m_ptr = &m_list[cave_m_idx[y][x]];

				/* Hack -- auto-recall */
				if (m_ptr->ml) monster_track(m_ptr->r_idx, m_ptr->u_idx);

				/* Hack - auto-track */
				if (m_ptr->ml) health_track(cave_m_idx[y][x]);
			}
		}
	}

	/* Check player */
	if (flg & (PROJECT_KILL))
	{
		/* Start with "dist" of zero */
		dist = 0;

		/* Scan for player */
		for (i = 0; i < grids; i++)
		{
			/* Hack -- Notice new "dist" values */
			if (gm[dist+1] == i) dist++;

			/* Get the grid location */
			y = gy[i];
			x = gx[i];

			/* Affect the player */
			if (project_p(who, dist, y, x, dam, typ)) notice = TRUE;
		}
	}

	/* Return "something was noticed" */
	return (notice);
}

/*
 * Controlled teleportation.  
 * Idea from PsiAngband, through Zangband and finally Oangband.
 */
void dimen_door(int dis, int fail)
{
	int ny;
	int nx;
	bool okay;
	bool old_expand_look = expand_look;

	expand_look = TRUE;
	okay = target_set_interactive(TARGET_FREE);
	expand_look = old_expand_look;
	if (!okay) return;

	/* grab the target coords. */
	ny = p_ptr->target_row;
	nx = p_ptr->target_col;

	/* Test for empty floor, forbid vaults or too large a
	 * distance, and insure that this spell is never certain.
	 */
	if (!cave_empty_bold(ny,nx) || (cave_info[ny][nx] & CAVE_ICKY) ||
		(distance(ny,nx,p_ptr->py,p_ptr->px) > dis) || 
		(rand_int(100) < fail))
	{
		message(MSG_EFFECT, 0, "You fail to exit the astral plane correctly!");
		p_ptr->energy -= 50;
		if (dis > 5) teleport_player(dis - 5);
	}

	/* Controlled teleport. */
	else teleport_player_to(ny,nx);
}
