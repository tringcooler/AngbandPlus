/* File: spells2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"

/*
 * Increase players hit points, notice effects
 */
bool hp_player(int num)
{
	/* Healing needed */
	if (p_ptr->chp < p_ptr->mhp)
	{
		/* Gain hitpoints */
		p_ptr->chp += num;

		/* Enforce maximum */
		if (p_ptr->chp >= p_ptr->mhp)
		{
			p_ptr->chp = p_ptr->mhp;
			p_ptr->chp_frac = 0;
		}

		/* Redraw */
		p_ptr->redraw |= (PR_HP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);

		/* Heal 0-4 */
		if (num < 5)
		{
			msg_print("You feel a little better.");
		}

		/* Heal 5-14 */
		else if (num < 15)
		{
			msg_print("You feel better.");
		}

		/* Heal 15-34 */
		else if (num < 35)
		{
			msg_print("You feel much better.");
		}

		/* Heal 35+ */
		else
		{
			msg_print("You feel very good.");
		}

		/* Notice */
		return (TRUE);
	}

	/* Ignore */
	return (FALSE);
}



/*
 * Leave a "glyph of warding" which prevents monster movement.
 * Return TRUE on success
 */
bool warding_glyph(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	/* XXX XXX XXX */
	if (!cave_clean_bold(py, px))
	{
		msg_print("The object resists the spell.");
		/* Failure */
		return (FALSE);
	}

	/* Require suitable grid. Ignore monsters */
 	if (!cave_trappable_bold(py, px))
	{
		char name[80];

		feature_desc(name, sizeof(name), cave_feat[py][px], FALSE, TRUE);

		msg_format("The %s resists the spell.", name);

		/* Failure */
		return (FALSE);
	}

	/* Create a glyph */
	set_effect_glyph(py, px);

	/* Remember this square */
	cave_info[py][px] |= (CAVE_MARK);

	/* Success */
	return (TRUE);
}

/*
 * Create a "glacier" that allows LOS but prevents movement and projections
 * Return TRUE on success
 */
bool create_glacier(void)
{
	int y, x;
	int status = FALSE;

	/* Select a grid */
	if (!target_set_interactive(TARGET_GRID)) return (FALSE);

	/* Paranoia */
	if (!p_ptr->target_set) return (FALSE);

	/* Get coordinates */
	y = p_ptr->target_row;
	x = p_ptr->target_col;

	/* Must be in the line of fire */
	if (!player_can_fire_bold(y, x))
	{
		msg_print("That grid is out of your line of fire!");
	}
	/* Must be a passable grid free of monsters */
	else if (!cave_empty_bold(y, x))
	{
		msg_print("The grid resists your spell!");
	}
	/* Valid grid */
	else
	{
		/* Get the index of the first effect of that grid */
		s16b x_idx = cave_x_idx[y][x];

		/* Remove some effects */
		while (x_idx)
		{
			/* Get the effect */
			effect_type *x_ptr = &x_list[x_idx];

			/* Point to the next effect */
			x_idx = x_ptr->next_x_idx;

			/* Remove only the cloud effects */
			if ((x_ptr->x_type == EFFECT_SHIMMERING_CLOUD) ||
				(x_ptr->x_type == EFFECT_LINGERING_CLOUD) ||
				(x_ptr->x_type == EFFECT_PERMANENT_CLOUD))
			{
				delete_effect_idx((s16b)(x_ptr - x_list));
			}
		}

		/* Create the glacier */
		set_effect_glacier(FEAT_GLACIER, y, x, SOURCE_EFFECT, 0);

		/* Show that grid */
		cave_info[y][x] |= (CAVE_MARK);
		lite_spot(y, x);

		/* Success */
		status = TRUE;
	}

	/* Reset the target info */
	target_set_monster(0);

	return (status);
}

/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_pos[] =
{
	"strong",
	"smart",
	"wise",
	"dextrous",
	"healthy",
	"agile",
	"sneaky",
	"perceptive",
	"lucky"
};


/*
 * Array of stat "descriptions"
 */
static cptr desc_stat_neg[] =
{
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"slow",
	"noisy",
	"short-sighted",
	"unlucky"
};


/*
 * Lose a "point"
 */
bool do_dec_stat(int stat)
{
	bool sust = FALSE;

	/* Get the "sustain" */
	switch (stat)
	{
		case A_STR: if (p_ptr->sustain_str) sust = TRUE; break;
		case A_INT: if (p_ptr->sustain_int) sust = TRUE; break;
		case A_WIS: if (p_ptr->sustain_wis) sust = TRUE; break;
		case A_DEX: if (p_ptr->sustain_dex) sust = TRUE; break;
		case A_CON: if (p_ptr->sustain_con) sust = TRUE; break;
		case A_AGI: if (p_ptr->sustain_agi) sust = TRUE; break;
		case A_STE: if (p_ptr->sustain_ste) sust = TRUE; break;
		case A_PER: if (p_ptr->sustain_per) sust = TRUE; break;
		case A_LUC: if (p_ptr->sustain_luc) sust = TRUE; break;
	}

	/* Sustain */
	if (sust)
	{
		/* Message */
		msg_format("You feel very %s for a moment, but the feeling passes.",
		           desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Allow a check for resistance if stat is already low */

	if ((3 + randint(6) + randint(6)) > p_ptr->stat_cur[stat]){
		/* Message */
		msg_format("You feel a bit %s, but no more than usual.",
		           desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	} 

	/* Attempt to reduce the stat */
	if (dec_stat(stat, 10, FALSE))
	{
		/* Message */
		message_format(MSG_DRAIN_STAT, stat, "You feel very %s.", desc_stat_neg[stat]);

		/* Notice effect */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Restore lost "points" in a stat
 */
bool do_res_stat(int stat)
{
	/* Attempt to increase */
	if (res_stat(stat))
	{
		/* Message */
		msg_format("You feel less %s.", desc_stat_neg[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}


/*
 * Gain a "point" in a stat
 */
bool do_inc_stat(int stat)
{
	bool res;

	/* Restore strength */
	res = res_stat(stat);

	/* Attempt to increase */
	if (inc_stat(stat))
	{
		/* Message */
		msg_format("You feel very %s!", desc_stat_pos[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Restoration worked */
	if (res)
	{
		/* Message */
		msg_format("You feel less %s.", desc_stat_neg[stat]);

		/* Notice */
		return (TRUE);
	}

	/* Nothing obvious */
	return (FALSE);
}



/*
 * Identify everything being carried.
 * Done by a potion of "self knowledge".
 */
void identify_pack(void)
{
	int i;

	/* Simply identify and know every item */
	for (i = 0; i < INVEN_TOTAL; i++)
	{
		object_type *o_ptr = &inventory[i];

		bool aware = FALSE;

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Remember awareness */
		if (object_aware_p(o_ptr)) aware = TRUE;

		/* Aware and Known */
		object_aware(o_ptr);
		object_known(o_ptr);

		if (!aware) apply_autoinscription(o_ptr);
	}

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_NATIVE);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
}



#define ENCHANT_MAX 9


/*
 * Used by the "enchant" function (chance of failure)
 */
static const int enchant_table[ENCHANT_MAX + 1] =
{
	0, 20,  50, 100, 200,
	300, 400, 500,
	700, 900
};


/*
 * Hack -- Removes curse from an object.
 */
void uncurse_object(object_type *o_ptr)
{
	/* Uncurse it */
	o_ptr->ident &= ~(IDENT_CURSED);

	/* Remove special inscription, if any */
	if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

	/* Take note if allowed */
	if (o_ptr->discount == 0) o_ptr->discount = INSCRIP_UNCURSED;

	/* The object has been "sensed" */
	o_ptr->ident |= (IDENT_SENSE);
}


/*
 * Removes curses from items in inventory.
 *
 * Note that Items which are "Perma-Cursed" (The One Ring,
 * The Crown of Morgoth) can NEVER be uncursed.
 *
 * Note that if "all" is FALSE, then Items which are
 * "Heavy-Cursed" (Mormegil, Calris, and Weapons of Morgul)
 * will not be uncursed.
 */
static int remove_curse_aux(bool star_curse)
{
	int i, cnt = 0;
	char o_name[80];

	/* Attempt to uncurse items being worn */
	for (i = INVEN_WIELD; i < INVEN_TOTAL; i++)
	{
		u32b f1, f2, f3, fn;

		object_type *o_ptr = &inventory[i];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Uncursed already */
		if (!cursed_p(o_ptr)) continue;

		/* Extract the flags */
		object_flags(o_ptr, &f1, &f2, &f3, &fn);

		/* Heavily Cursed Items need a special spell */
		if (!star_curse && (f3 & (TR3_HEAVY_CURSE))){
			object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);
			if ((o_ptr->to_h < 0) && (o_ptr->to_h < o_ptr->to_d) && (wield_slot(o_ptr)!=(INVEN_BODY))){
				msg_format("The %s glows with a sickly light...",o_name);
				enchant(o_ptr, randint(3)+1, ENCH_TOHIT);
				msg_format("Its hit penalty is reduced, however it is still cursed.");
			} else if ((o_ptr->to_d < 0) && (o_ptr->to_d <= o_ptr->to_h)){
				msg_format("The %s glows with a sickly light...",o_name);
				enchant(o_ptr, randint(3)+1, ENCH_TODAM);
				msg_format("Its damage penalty is reduced, however it is still cursed.");
			} else if (o_ptr->to_a < 0){
				msg_format("The %s glows with a sickly light...",o_name);
				enchant(o_ptr, randint(3)+1, ENCH_TOAC);
				msg_format("Its AC penalty is reduced, however it is still cursed.");
			} else {
				msg_format("The powerful curse on the %s resists the spell!",o_name);
			}
			msg_format("You need a scroll or spell of *Remove Curse*.");
			continue;
		}

		/* Perma-Cursed Items can NEVER be uncursed */
		if (f3 & (TR3_PERMA_CURSE)) continue;

		/* Uncurse the object */
		uncurse_object(o_ptr);

		/* Recalculate the bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP);

		/* Count the uncursings */
		cnt++;
	}

	/* Return "something uncursed" */
	return (cnt);
}


/*
 * Remove most curses
 */
bool remove_curse(bool star_curse)
{
	return (remove_curse_aux(star_curse));
}



/*
 * Restores any drained experience
 */
bool restore_level(void)
{
	/* Restore experience */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Message */
		msg_print("You feel your life energies returning.");

		/* Restore the experience */
		p_ptr->exp = p_ptr->max_exp;

		/* Check the experience */
		check_experience();

		/* Did something */
		return (TRUE);
	}

	/* No effect */
	return (FALSE);
}


/*
 * Hack -- acquire self knowledge
 *
 * List various information about the player and/or his current equipment.
 *
 * See also "identify_fully()".
 *
 * Use the "roff()" routines, perhaps.  XXX XXX XXX
 *
 * Use the "show_file()" method, perhaps.  XXX XXX XXX
 *
 * This function cannot display more than 20 lines.  XXX XXX XXX
 */
void self_knowledge(void)
{
	int i = 0, j, k;

	u32b f1 = 0L, f2 = 0L, f3 = 0L, fn = 0L;

	object_type *o_ptr;

	cptr info[135];

	/* Get item flags from equipment */
	for (k = INVEN_WIELD; k < END_EQUIPMENT; k++)
	{
		u32b t1, t2, t3, tn;

		o_ptr = &inventory[k];

		/* Skip non-objects */
		if (!o_ptr->k_idx) continue;

		/* Extract the flags */
		object_flags(o_ptr, &t1, &t2, &t3, &tn);

		/* Extract flags */
		f1 |= t1;
		f2 |= t2;
		f3 |= t3;
		fn |= tn;
	}

	if (cp_ptr->flags & CF_BLESS_WEAPON)
	{
		info[i++] = "You are only comfortable wielding blunt weapons or blessed weapons.";
	}

	if (cp_ptr->flags & CF_CUMBER_GLOVE)
	{
		info[i++] = "You are only comfortable wearing gloves that aid your ability to move freely or increase your dexterity.";
	}

	if (cp_ptr->flags & CF_ROGUE_COMBAT)
	{
		info[i++] = "You can sometimes steal objects and gold from monsters.";
		info[i++] = "You are extraordinally precise with throwing weapons.";
		info[i++] = "You do an extraordinary amount of damage when attacking sleeping monsters.";
		info[i++] = "You are extraordinally precise and deadly when using a sling.";
	}

	if (cp_ptr->flags & CF_SET_TRAPS)
	{
		info[i++] = "You can set traps.";
	}

	if (cp_ptr->flags & CF_EXTRA_ATTACK)
	{
		if (p_ptr->lev >= LEV_EXTRA_COMBAT) info[i++] = "Your attacking speed is naturally increased.";
		else info[i++] = "After you gain more experience, your attacking speed will be naturally increased.";
	}

	if (cp_ptr->flags & CF_EXTRA_SHOT)
	{
		if (p_ptr->lev >= LEV_EXTRA_COMBAT) info[i++] = "Your shooting speed is increased when using a sling.";
		else info[i++] = "After you gain more experience, your shooting speed will be increased when using a sling.";
	}

	if (cp_ptr->flags & CF_EXTRA_ARROW)
	{
		if (p_ptr->lev >= LEV_EXTRA_COMBAT) info[i++] = "Your shooting speed is increased when using a bow.";
		else info[i++] = "After you gain more experience, your shooting speed will be increased when using a bow.";
	}

	if (cp_ptr->flags & CF_BRAVERY_30)
	{
		if (p_ptr->lev >= LEV_BRAVERY) info[i++] = "You are naturally resistant to fear.";
		else info[i++] = "After you gain more experience, you will be naturally resistant to fear.";
	}



	if (p_ptr->blind)
	{
		info[i++] = "You cannot see.";
	}
	if (p_ptr->confused)
	{
		info[i++] = "You are confused.";
	}
	if (p_ptr->afraid)
	{
		info[i++] = "You are terrified.";
	}
	if (p_ptr->cut)
	{
		info[i++] = "You are bleeding.";
	}
	if (p_ptr->stun)
	{
		info[i++] = "You are stunned.";
	}
	if (p_ptr->poisoned)
	{
		info[i++] = "You are poisoned.";
	}
	if (p_ptr->image)
	{
		info[i++] = "You are hallucinating.";
	}

	if (p_ptr->aggravate)
	{
		info[i++] = "You aggravate monsters.";
	}
	if (p_ptr->teleport)
	{
		info[i++] = "Your position is very uncertain.";
	}

	if (p_ptr->blessed)
	{
		info[i++] = "You feel righteous.";
	}
	if (p_ptr->hero)
	{
		info[i++] = "You feel heroic.";
	}
	if (p_ptr->shero)
	{
		info[i++] = "You are in a battle rage.";
	}
	if (p_ptr->protevil)
	{
		info[i++] = "You are protected from evil.";
	}
	if (p_ptr->shield)
	{
		info[i++] = "You are protected by a mystic shield.";
	}
	if (p_ptr->invuln)
	{
		info[i++] = "You are temporarily invulnerable.";
	}
	if (p_ptr->confusing)
	{
		info[i++] = "Your hands are glowing dull red.";
	}
	if (p_ptr->searching)
	{
		info[i++] = "You are looking around very carefully.";
	}
	if (p_ptr->new_spells)
	{
		info[i++] = "You can learn some spells/prayers.";
	}
	if (p_ptr->word_recall)
	{
		info[i++] = "You will soon be recalled.";
	}
	if (p_ptr->teleport_delay)
	{
		info[i++] = "You will soon teleport.";
	}
	if (p_ptr->see_infra)
	{
		info[i++] = "Your eyes are sensitive to infrared light.";
	}

	if (p_ptr->slow_digest)
	{
		info[i++] = "Your appetite is small.";
	}
	if (p_ptr->ffall)
	{
		info[i++] = "You land gently.";
	}
	if (p_ptr->flying)
	{
		info[i++] = "You are flying.";
	}
	if (p_ptr->lite)
	{
		info[i++] = "You are glowing with light.";
	}
	if (p_ptr->regenerate)
	{
		info[i++] = "You regenerate quickly.";
	}
	if (p_ptr->telepathy)
	{
		info[i++] = "You have ESP.";
	}
	if (p_ptr->see_inv)
	{
		info[i++] = "You can see invisible creatures.";
	}
	if (p_ptr->free_act)
	{
		info[i++] = "You have free action.";
	}
	if (p_ptr->hold_life)
	{
		info[i++] = "You have a firm hold on your life force.";
	}

	if (p_ptr->immune_acid)
	{
		info[i++] = "You are completely immune to acid.";
	}
	else if ((p_ptr->resist_acid) && (p_ptr->oppose_acid))
	{
		info[i++] = "You resist acid exceptionally well.";
	}
	else if ((p_ptr->resist_acid) || (p_ptr->oppose_acid))
	{
		info[i++] = "You are resistant to acid.";
	}

	if (p_ptr->immune_elec)
	{
		info[i++] = "You are completely immune to lightning.";
	}
	else if ((p_ptr->resist_elec) && (p_ptr->oppose_elec))
	{
		info[i++] = "You resist lightning exceptionally well.";
	}
	else if ((p_ptr->resist_elec) || (p_ptr->oppose_elec))
	{
		info[i++] = "You are resistant to lightning.";
	}

	if (p_ptr->immune_fire)
	{
		info[i++] = "You are completely immune to fire.";
	}
	else if ((p_ptr->resist_fire) && (p_ptr->oppose_fire))
	{
		info[i++] = "You resist fire exceptionally well.";
	}
	else if ((p_ptr->resist_fire) || (p_ptr->oppose_fire))
	{
		info[i++] = "You are resistant to fire.";
	}

	if (p_ptr->immune_cold)
	{
		info[i++] = "You are completely immune to cold.";
	}
	else if ((p_ptr->resist_cold) && (p_ptr->oppose_cold))
	{
		info[i++] = "You resist cold exceptionally well.";
	}
	else if ((p_ptr->resist_cold) || (p_ptr->oppose_cold))
	{
		info[i++] = "You are resistant to cold.";
	}

	if (p_ptr->immune_pois)
	{
		info[i++] = "You are completely immune to poison.";
	}
	else if ((p_ptr->resist_pois) && (p_ptr->oppose_pois))
	{
		info[i++] = "You resist poison exceptionally well.";
	}
	else if ((p_ptr->resist_pois) || (p_ptr->oppose_pois))
	{
		info[i++] = "You are resistant to poison.";
	}


	if (p_ptr->resist_fear)
	{
		info[i++] = "You are completely fearless.";
	}

	if (p_ptr->resist_lite)
	{
		info[i++] = "You are resistant to bright light.";
	}
	if (p_ptr->resist_dark)
	{
		info[i++] = "You are resistant to darkness.";
	}
	if (p_ptr->resist_blind)
	{
		info[i++] = "Your eyes are resistant to blindness.";
	}
	if (p_ptr->resist_confu || p_ptr->oppose_conf)
	{
		info[i++] = "You are resistant to confusion attacks.";
	}
	if (p_ptr->resist_sound)
	{
		info[i++] = "You are resistant to sonic attacks.";
	}
	if (p_ptr->resist_shard)
	{
		info[i++] = "You are resistant to blasts of shards.";
	}
	if (p_ptr->resist_nexus)
	{
		info[i++] = "You are resistant to nexus attacks.";
	}
	if (p_ptr->resist_nethr)
	{
		info[i++] = "You are resistant to nether forces.";
	}
	if (p_ptr->resist_chaos)
	{
		info[i++] = "You are resistant to chaos.";
	}
	if (((p_ptr->resist_confu) && (!p_ptr->resist_chaos)) ||
		((!p_ptr->resist_confu) && (p_ptr->resist_chaos)))
	{
		info[i++] = "You are resistant to being confused.";
	}
	if (p_ptr->resist_disen)
	{
		info[i++] = "You are resistant to disenchantment.";
	}
	if (p_ptr->p_native_known & P_NATIVE_LAVA)
	{
		info[i++] = "You are native to lava.";
	}
	if (p_ptr->p_native_known & P_NATIVE_ICE)
	{
		info[i++] = "You are native to ice.";
	}
	if (p_ptr->p_native_known & P_NATIVE_OIL)
	{
		info[i++] = "You are native to oil.";
	}
	if (p_ptr->p_native_known & P_NATIVE_FIRE)
	{
		info[i++] = "You are native to fire.";
	}
	if (p_ptr->p_native_known & P_NATIVE_SAND)
	{
		info[i++] = "You are native to sand.";
	}
	if (p_ptr->p_native_known & P_NATIVE_FOREST)
	{
		info[i++] = "You are native to forests.";
	}
	if (p_ptr->p_native_known & P_NATIVE_WATER)
	{
		info[i++] = "You are native to water.";
	}
	if (p_ptr->p_native_known & P_NATIVE_ACID)
	{
		info[i++] = "You are native to acid.";
	}
	if (p_ptr->p_native_known & P_NATIVE_MUD)
	{
		info[i++] = "You are native to mud.";
	}
	if (p_ptr->sustain_str)
	{
		info[i++] = "Your strength is sustained.";
	}
	if (p_ptr->sustain_int)
	{
		info[i++] = "Your intelligence is sustained.";
	}
	if (p_ptr->sustain_wis)
	{
		info[i++] = "Your wisdom is sustained.";
	}
	if (p_ptr->sustain_con)
	{
		info[i++] = "Your constitution is sustained.";
	}
	if (p_ptr->sustain_dex)
	{
		info[i++] = "Your dexterity is sustained.";
	}
	if (p_ptr->sustain_agi)
	{
		info[i++] = "Your agility is sustained.";
	}
	if (p_ptr->sustain_ste)
	{
		info[i++] = "Your stealth is sustained.";
	}
	if (p_ptr->sustain_per)
	{
		info[i++] = "Your perception is sustained.";
	}
	if (p_ptr->sustain_luc)
	{
		info[i++] = "Your luck is sustained.";
	}

	if (f1 & (TR1_STR))
	{
		info[i++] = "Your strength is affected by your equipment.";
	}
	if (f1 & (TR1_INT))
	{
		info[i++] = "Your intelligence is affected by your equipment.";
	}
	if (f1 & (TR1_WIS))
	{
		info[i++] = "Your wisdom is affected by your equipment.";
	}
	if (f1 & (TR1_DEX))
	{
		info[i++] = "Your dexterity is affected by your equipment.";
	}
	if (f1 & (TR1_CON))
	{
		info[i++] = "Your constitution is affected by your equipment.";
	}
	if (f1 & (TR1_AGI))
	{
		info[i++] = "Your agility is affected by your equipment.";
	}
	if (f1 & (TR1_STE))
	{
		info[i++] = "Your stealth is affected by your equipment.";
	}
	if (f1 & (TR1_PER))
	{
		info[i++] = "Your perception is affected by your equipment.";
	}
	if (f1 & (TR1_LUC))
	{
		info[i++] = "Your luck is affected by your equipment.";
	}

	if (f1 & (TR1_SEARCH))
	{
		info[i++] = "Your searching ability is affected by your equipment.";
	}
	if (f1 & (TR1_INFRA))
	{
		info[i++] = "Your infravision is affected by your equipment.";
	}
	if (f1 & (TR1_TUNNEL))
	{
		info[i++] = "Your digging ability is affected by your equipment.";
	}
	if (f1 & (TR1_SPEED))
	{
		info[i++] = "Your speed is affected by your equipment.";
	}
	if (f1 & (TR1_BLOWS))
	{
		info[i++] = "Your attack speed is affected by your equipment.";
	}
	if (f1 & (TR1_SHOTS))
	{
		info[i++] = "Your shooting speed is affected by your equipment.";
	}
	if (f1 & (TR1_MIGHT))
	{
		info[i++] = "Your shooting might is affected by your equipment.";
	}

	/* Get the current weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Analyze the weapon */
	if (o_ptr->k_idx)
	{
		/* Special "Attack Bonuses" */
		if (f1 & (TR1_BRAND_ACID))
		{
			info[i++] = "Your weapon melts your foes.";
		}
		if (f1 & (TR1_BRAND_ELEC))
		{
			info[i++] = "Your weapon shocks your foes.";
		}
		if (f1 & (TR1_BRAND_FIRE))
		{
			info[i++] = "Your weapon burns your foes.";
		}
		if (f1 & (TR1_BRAND_COLD))
		{
			info[i++] = "Your weapon freezes your foes.";
		}
		if (f1 & (TR1_BRAND_POIS))
		{
			info[i++] = "Your weapon poisons your foes.";
		}

		/* Special "slay" flags */
		if (f1 & (TR1_SLAY_ANIMAL))
		{
			info[i++] = "Your weapon strikes at animals with extra force.";
		}
		if (f1 & (TR1_SLAY_EVIL))
		{
			info[i++] = "Your weapon strikes at evil with extra force.";
		}
		if (f1 & (TR1_SLAY_UNDEAD))
		{
			info[i++] = "Your weapon strikes at undead with holy wrath.";
		}
		if (f1 & (TR1_SLAY_DEMON))
		{
			info[i++] = "Your weapon strikes at demons with holy wrath.";
		}
		if (f1 & (TR1_SLAY_ORC))
		{
			info[i++] = "Your weapon is especially deadly against orcs.";
		}
		if (f1 & (TR1_SLAY_TROLL))
		{
			info[i++] = "Your weapon is especially deadly against trolls.";
		}
		if (f1 & (TR1_SLAY_GIANT))
		{
			info[i++] = "Your weapon is especially deadly against giants.";
		}
		if (f1 & (TR1_SLAY_DRAGON))
		{
			info[i++] = "Your weapon is especially deadly against dragons.";
		}

		/* Special "kill" flags */
		if (f1 & (TR1_KILL_DRAGON))
		{
			info[i++] = "Your weapon is a great bane of dragons.";
		}
		if (f1 & (TR1_KILL_DEMON))
		{
			info[i++] = "Your weapon is a great bane of demons.";
		}
		if (f1 & (TR1_KILL_UNDEAD))
		{
			info[i++] = "Your weapon is a great bane of undead.";
		}


		/* Indicate Blessing */
		if (f3 & (TR3_BLESSED))
		{
			info[i++] = "Your weapon has been blessed by the gods.";
		}

		/* Hack */
		if (f3 & (TR3_IMPACT))
		{
			info[i++] = "Your weapon can induce earthquakes.";
		}
	}


	/* Save screen */
	screen_save();


	/* Clear the screen */
	Term_clear();

	/* Label the information */
	prt("     Your Attributes:", 1, 0);

	/* Dump the info */
	for (k = 2, j = 0; j < i; j++)
	{
		/* Show the info */
		prt(info[j], k++, 0);

		/* Page wrap */
		if ((k == 22) && (j+1 < i))
		{
			prt("-- more --", k, 0);
			inkey();

			/* Clear the screen */
			Term_clear();

			/* Label the information */
			prt("     Your Attributes:", 1, 0);

			/* Reset */
			k = 2;
		}
	}

	/* Pause */
	prt("[Press any key to continue]", k, 0);
	(void)inkey();


	/* Load screen */
	screen_load();
}

/*
 * Set word of recall as appropriate
 */
void set_recall(void)
{
	/* Ironman */
	if (adult_ironman && !p_ptr->total_winner)
	{
		msg_print("Nothing happens.");
		return;
	}

	/* Activate recall */
	if (!p_ptr->word_recall)
	{
		/* Reset recall depth */
		if ((p_ptr->depth > 0) && (p_ptr->depth != p_ptr->recall_depth))
		{
			/*
			 * ToDo: Add a new player_type field "recall_depth"
			 * ToDo: Poll: Always reset recall depth?
			 */
			 if (get_check("Reset recall depth? "))
				p_ptr->recall_depth = p_ptr->depth;
		}

		p_ptr->word_recall = rand_int(20) + 15;
		msg_print("The air about you becomes charged...");
	}

	/* Deactivate recall */
	else
	{
		p_ptr->word_recall = 0;
		msg_print("A tension leaves the air around you...");
	}
}

void delayed_teleport_player(int dam)
{
	p_ptr->teleport_delay = randint(3) + 2;
	p_ptr->teleport_range = dam;
}


/*
 * Detect all traps on current panel
 */
bool detect_traps(void)
{
	int y, x;

	bool detect = FALSE;

	/* Scan the current panel */
	for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
	{
		for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Detect traps. Avoid glyphs */
			if (cave_player_trap_bold(y, x) ||
				cave_monster_trap_bold(y, x))
			{
				effect_type *x_ptr = &x_list[cave_x_idx[y][x]];

				/* Hack -- Memorize */
				x_ptr->x_flags &= ~(EF1_HIDDEN);

				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of traps!");
	}

	/* Result */
	return (detect);
}



/*
 * Detect all doors on current panel
 */
bool detect_doors(void)
{
	int y, x;

	bool detect = FALSE;


	/* Scan the panel */
	for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
	{
		for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Detect doors */
			if (cave_door_bold(y, x))
			{

				/* Detect secret doors */
				if (cave_ff1_match(y, x, FF1_SECRET))
				{
					/* Place a door */
					cave_alter_feat(y, x, FS_SECRET);
				}

				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of doors!");
	}

	/* Result */
	return (detect);
}


/*
 * Detect all stairs on current panel
 */
bool detect_stairs(void)
{
	int y, x;

	bool detect = FALSE;


	/* Scan the panel */
	for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
	{
		for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			/* Detect stairs */
			if (cave_stair_bold(y, x))
			{
				/* Find secrets */
				if (cave_ff1_match(y, x, FF1_SECRET))
				{
					cave_alter_feat(y, x, FS_SECRET);
				}

				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				/* Obvious */
				detect = TRUE;
			}
		}
	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of stairs!");
	}

	/* Result */
	return (detect);
}


/*
 * Detect any treasure on the current panel
 */
bool detect_treasure(void)
{
	int y, x;

	bool detect = FALSE;


	/* Scan the current panel */
	for (y = Term->offset_y; y < Term->offset_y + SCREEN_HGT; y++)
	{
		for (x = Term->offset_x; x < Term->offset_x + SCREEN_WID; x++)
		{
			if (!in_bounds_fully(y, x)) continue;

			if (cave_ff1_match(y, x, FF1_HAS_GOLD))
			{
				/* Detect secrets */
				if (cave_ff1_match(y, x, FF1_SECRET))
				{

					/*Find secrets*/
					cave_alter_feat(y, x, FS_SECRET);

				}

				/* Hack -- Memorize */
				cave_info[y][x] |= (CAVE_MARK);

				/* Redraw */
				lite_spot(y, x);

				detect = TRUE;
			}

		}
	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of buried treasure!");
	}

	/* Result */
	return (detect);
}



/*
 * Detect all "gold" objects on the current panel
 */
bool detect_objects_gold(void)
{
	int i, y, x;

	bool detect = FALSE;


	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (!panel_contains(y, x)) continue;

		/* Detect "gold" objects */
		if (o_ptr->tval == TV_GOLD)
		{
			/* Hack -- memorize it */
			o_ptr->marked = TRUE;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Scan monsters, looking for mimics */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/*
		 *we want to detect treasure mimics
		 */
		if (!(strchr("$", r_ptr->d_char))) continue;

		/* XXX XXX - Mimics aren't detected */
		if (!m_ptr->mimic_k_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/*mark them as a mimic*/
		m_ptr->mflag |= (MFLAG_MIMIC | MFLAG_MARK | MFLAG_SHOW);

		/* Optimize -- Repair flags */
		repair_mflag_mark = TRUE;
		repair_mflag_show = TRUE;

		/* Update the monster */
		update_mon(i, FALSE);

		/* Detect */
		detect = TRUE;

	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of treasure!");
	}

	/* Result */
	return (detect);
}


/*
 * Detect all "normal" objects on the current panel
 */
bool detect_objects_normal(void)
{
	int i, y, x;

	bool detect = FALSE;


	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (!panel_contains(y, x)) continue;

		/* Detect "real" objects */
		if (o_ptr->tval != TV_GOLD)
		{

			/* Hack -- memorize it */
			o_ptr->marked = TRUE;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Scan monsters, looking for mimics */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/*we want to detect mimics of objects,
		 *note I did not include all possible magic items
		 */
		if (!(strchr("!?-_=~", r_ptr->d_char))) continue;

		/* XXX XXX - Mimics aren't detected */
		if (!m_ptr->mimic_k_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/*mark them as a mimic*/
		m_ptr->mflag |= (MFLAG_MIMIC | MFLAG_MARK | MFLAG_SHOW);

		/* Optimize -- Repair flags */
		repair_mflag_mark = TRUE;
		repair_mflag_show = TRUE;

		/* Update the monster */
		update_mon(i, FALSE);

		/* Detect */
		detect = TRUE;

	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of objects!");
	}

	/* Result */
	return (detect);
}



/*
 * Detect all "magic" objects on the current panel.
 *
 * This will light up all spaces with "magic" items, including artifacts,
 * ego-items, potions, scrolls, books, rods, wands, staves, amulets, rings,
 * and "enchanted" items of the "good" variety.
 *
 * It can probably be argued that this function is now too powerful.
 */
bool detect_objects_magic(void)
{
	int i, y, x, tv;

	bool detect = FALSE;


	/* Scan all objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (!panel_contains(y, x)) continue;

		/* Examine the tval */
		tv = o_ptr->tval;

		/* Artifacts, misc magic items, or enchanted wearables */
		if (artifact_p(o_ptr) || ego_item_p(o_ptr) ||
		    (tv == TV_AMULET) || (tv == TV_RING) ||
		    (tv == TV_TALISMAN) || (tv == TV_WAND) || (tv == TV_ROD) ||
		    (tv == TV_SCROLL) || (tv == TV_POTION) ||
		    (tv == TV_MAGIC_BOOK) || (tv == TV_PRAYER_BOOK) || (tv == TV_DRUID_BOOK) ||
		    ((o_ptr->to_a > 0) || (o_ptr->to_h + o_ptr->to_d > 0)))
		{
			/* Memorize the item */
			o_ptr->marked = TRUE;

			/* Redraw */
			lite_spot(y, x);

			/* Detect */
			detect = TRUE;
		}
	}

	/* Scan monsters, looking for mimics */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/*
		 *we want to detect magical appearing mimics,
		 *note I did not include all possible magic items
		 */
		if (!(strchr("!?-_=", r_ptr->d_char))) continue;

		/* XXX XXX - Mimics aren't detected */
		if (!m_ptr->mimic_k_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/*mark them as a mimic*/
		m_ptr->mflag |= (MFLAG_MIMIC | MFLAG_MARK | MFLAG_SHOW);

		/* Optimize -- Repair flags */
		repair_mflag_mark = TRUE;
		repair_mflag_show = TRUE;

		/* Update the monster */
		update_mon(i, FALSE);

		/* Detect */
		detect = TRUE;

	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of magic objects!");
	}

	/* Return result */
	return (detect);
}


/*
 * Detect all "normal" monsters on the current panel
 */
bool detect_monsters_normal(void)
{
	int i, y, x;

	bool flag = FALSE;


	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* XXX XXX - Unknown Mimics stay hidden */
		if (m_ptr->mimic_k_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/* Detect all non-invisible monsters */
		if (!(r_ptr->flags2 & (RF2_INVISIBLE)))
		{
			/* Optimize -- Repair flags */
			repair_mflag_mark = TRUE;
			repair_mflag_show = TRUE;

			/* Hack -- Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print("You sense the presence of monsters!");
	}

	/* Result */
	return (flag);
}

/*
 * Detect all "living" monsters on the current panel, visible and invisible.
 */
bool detect_monsters_living(void)
{
	int i, y, x;

	bool flag = FALSE;

	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* XXX XXX - Unknown Mimics stay hidden */
		if (m_ptr->mimic_k_idx) continue;

		/*Only detect living monsters*/
		if monster_nonliving(r_ptr) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/* Optimize -- Repair flags */
		repair_mflag_mark = TRUE;
		repair_mflag_show = TRUE;

		/* Hack -- Detect the monster */
		m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

		/* Update the monster */
		update_mon(i, FALSE);

		/* Detect */
		flag = TRUE;

	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print("You sense the presence of living creatures!");
	}

	/* Result */
	return (flag);
}


/*
 * Detect all "invisible" monsters on current panel
 */
bool detect_monsters_invis(void)
{
	int i, y, x;

	bool flag = FALSE;

	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		monster_lore *l_ptr = &l_list[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/* Detect invisible monsters */
		if (r_ptr->flags2 & (RF2_INVISIBLE))
		{
			/* XXX XXX - Mimics stay hidden */
			if (m_ptr->mimic_k_idx) continue;

			/* Take note that they are invisible */
			l_ptr->r_l_flags2 |= (RF2_INVISIBLE);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Optimize -- Repair flags */
			repair_mflag_mark = TRUE;
			repair_mflag_show = TRUE;

			/* Hack -- Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print("You sense the presence of invisible creatures!");
	}

	/* Result */
	return (flag);
}



/*
 * Detect all "evil" monsters on current panel
 */
bool detect_monsters_evil(void)
{
	int i, y, x;

	bool flag = FALSE;


	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		monster_lore *l_ptr = &l_list[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/* XXX XXX - Mimics stay hidden */
		if (m_ptr->mimic_k_idx) continue;

		/* Detect evil monsters */
		if (r_ptr->flags3 & (RF3_EVIL))
		{
			/* Take note that they are evil */
			l_ptr->r_l_flags3 |= (RF3_EVIL);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Optimize -- Repair flags */
			repair_mflag_mark = TRUE;
			repair_mflag_show = TRUE;

			/* Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print("You sense the presence of evil creatures!");
	}

	/* Result */
	return (flag);
}

/*
 * Detect all "animal" monsters on current panel
 */
bool detect_monsters_animal(void)
{
	int i, y, x;

	bool flag = FALSE;


	/* Scan monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];
		monster_lore *l_ptr = &l_list[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Only detect nearby monsters */
		if (!panel_contains(y, x)) continue;

		/* XXX XXX - Mimics stay hidden */
		if (m_ptr->mimic_k_idx) continue;

		/* Detect animal monsters */
		if (r_ptr->flags3 & (RF3_ANIMAL))
		{
			/* Take note that they are animal */
			l_ptr->r_l_flags3 |= (RF3_ANIMAL);

			/* Update monster recall window */
			if (p_ptr->monster_race_idx == m_ptr->r_idx)
			{
				/* Window stuff */
				p_ptr->window |= (PW_MONSTER);
			}

			/* Optimize -- Repair flags */
			repair_mflag_mark = TRUE;
			repair_mflag_show = TRUE;

			/* Detect the monster */
			m_ptr->mflag |= (MFLAG_MARK | MFLAG_SHOW);

			/* Update the monster */
			update_mon(i, FALSE);

			/* Detect */
			flag = TRUE;
		}
	}

	/* Describe */
	if (flag)
	{
		/* Describe result */
		msg_print("You sense the presence of animals!");
	}

	/* Result */
	return (flag);
}

/*
 * Detect Terrain
 */
bool detect_terrain(void)
{
	int y, x;

	bool detect = FALSE;

	/* Scan all normal grids */
	for (y = 0; y < p_ptr->cur_map_hgt-1; y++)
	{
		/* Scan all normal grids */
		for (x = 0; x < p_ptr->cur_map_wid-1; x++)
		{
			/* Process all terrain*/
			if (feat_ff3_match(cave_feat[y][x], TERRAIN_MASK))
			{

				/* Memorize the grid */
				cave_info[y][x] |= (CAVE_MARK | CAVE_GLOW);

				/* We have seen the feature */
				f_info[cave_feat[y][x]].f_everseen = TRUE;

				/* Detect */
				detect = TRUE;
			}
		}
	}

	/* Describe */
	if (detect)
	{
		msg_print("You sense the presence of unusual terrain!");
	}

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_MAP);

	return (detect);
}



/*
 * Detect everything
 */
bool detect_all(void)
{
	bool detect = FALSE;

	/* Detect everything */
	if (detect_traps()) detect = TRUE;
	if (detect_doors()) detect = TRUE;
	if (detect_stairs()) detect = TRUE;
	if (detect_treasure()) detect = TRUE;
	if (detect_objects_gold()) detect = TRUE;
	if (detect_objects_normal()) detect = TRUE;
	if (detect_monsters_invis()) detect = TRUE;
	if (detect_monsters_normal()) detect = TRUE;

	/* Result */
	return (detect);
}



/*
 * Create stairs at the player location
 */
void stair_creation(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	/* XXX XXX XXX */
	if (!cave_valid_bold(py, px))
	{
		msg_print("The object resists the spell.");
		return;
	}

	/* XXX XXX XXX */
	delete_object(py, px);

	place_random_stairs(py, px);
}

/*
 * Hook to specify "weapon"
 */
bool item_tester_hook_wieldable_ided_weapon(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		{
			if (object_known_p(o_ptr)) return (TRUE);
			else return (FALSE);
		}
	}

	return (FALSE);
}


/*
 * Hook to specify "weapon"
 */
bool item_tester_hook_wieldable_weapon(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Hook to specify "weapon"
 */
bool item_tester_hook_weapon(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		case TV_BOW:
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}

/*
 * Hook to specify "weapon"
 */
bool item_tester_hook_ided_weapon(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DIGGING:
		case TV_BOW:
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			if (object_known_p(o_ptr)) return (TRUE);
			else return (FALSE);
		}
	}

	return (FALSE);
}



/*
 * Hook to specify "armour"
 */
bool item_tester_hook_ided_armour(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		case TV_DRAG_SHIELD:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_BOOTS:
		case TV_GLOVES:
		{
			if (object_known_p(o_ptr)) return (TRUE);
			else return (FALSE);
		}
	}

	return (FALSE);
}



/*
 * Hook to specify "armour"
 */
bool item_tester_hook_armour(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		case TV_DRAG_SHIELD:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_SHIELD:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_BOOTS:
		case TV_GLOVES:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


static bool item_tester_unknown(const object_type *o_ptr)
{
	if (object_known_p(o_ptr))
		return FALSE;
	else
		return TRUE;
}


static bool item_tester_unknown_star(const object_type *o_ptr)
{
	if (o_ptr->ident & IDENT_MENTAL)
		return FALSE;
	else
		return TRUE;
}


/*
 * Enchant an item
 *
 * Revamped!  Now takes item pointer, number of times to try enchanting,
 * and a flag of what to try enchanting.  Artifacts resist enchantment
 * some of the time, and successful enchantment to at least +0 might
 * break a curse on the item.  -CFT
 *
 * Note that an item can technically be enchanted all the way to +15 if
 * you wait a very, very, long time.  Going from +9 to +10 only works
 * about 5% of the time, and from +10 to +11 only about 1% of the time.
 *
 * Note that this function can now be used on "piles" of items, and
 * the larger the pile, the lower the chance of success.
 */
bool enchant(object_type *o_ptr, int n, int eflag)
{
	int i, chance, prob, ceiling;

	bool res = FALSE;

	bool a = artifact_p(o_ptr);

	u32b f1, f2, f3, fn;

	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3, &fn);

	/* Large piles resist enchantment */
	prob = o_ptr->number * 100;

	/* Missiles are easy to enchant */
	if ((o_ptr->tval == TV_BOLT) ||
	    (o_ptr->tval == TV_ARROW) ||
	    (o_ptr->tval == TV_SHOT))
	{
		prob = prob / 20;
	}

	/* Try "n" times */
	for (i=0; i<n; i++)
	{
		/* Hack -- Roll for pile resistance */
		if ((prob > 100) && (rand_int(prob) >= 100)) continue;

		/* Enchant to hit */
		if (eflag & (ENCH_TOHIT))
		{
			if (o_ptr->to_h < 0) chance = 0;
			else if (o_ptr->to_h > ENCHANT_MAX) chance = 1000;
			else chance = enchant_table[o_ptr->to_h];

			/* Attempt to enchant */
			if ((randint(1000) > chance) && (!a || (rand_int(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_h++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f3 & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_h >= 0) && (rand_int(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			}
		}

		/* Enchant to damage */
		if (eflag & (ENCH_TODAM))
		{
			if (o_ptr->to_d < 0) chance = 0;
			else if (o_ptr->to_d > ENCHANT_MAX) chance = 1000;
			else chance = enchant_table[o_ptr->to_d];

			/* Attempt to enchant */
			if ((randint(1000) > chance) && (!a || (rand_int(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_d++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f3 & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_d >= 0) && (rand_int(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			}
		}

		/* Enchant to armor class */
		if (eflag & (ENCH_TOAC))
		{
			ceiling = ac_ceiling(o_ptr);
				
			if (o_ptr->to_a <= 0) chance = 0;
			else if (o_ptr->to_a >= ceiling+2) chance = 990;
			else if (o_ptr->to_a >= ceiling) chance = 950;
			else chance = enchant_table[((o_ptr->to_a * ENCHANT_MAX) / ceiling)];

			/* Attempt to enchant */
			if ((randint(1000) > chance) && (!a || (rand_int(100) < 50)))
			{
				res = TRUE;

				/* Enchant */
				o_ptr->to_a++;

				/* Break curse */
				if (cursed_p(o_ptr) &&
				    (!(f3 & (TR3_PERMA_CURSE))) &&
				    (o_ptr->to_a >= 0) && (rand_int(100) < 25))
				{
					msg_print("The curse is broken!");

					/* Uncurse the object */
					uncurse_object(o_ptr);
				}
			} 
		}
	}

	/* Failure */
	if (!res && (eflag & (ENCH_TOAC)) && o_ptr->to_a >= ceiling){
		msg_print("That armour is not heavy enough to receive the enchantment.");
	}

	if (!res) return (FALSE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

	/* Success */
	return (TRUE);
}



/*
 * Enchant an item (in the inventory or on the floor)
 * Note that "num_ac" requires armour, else weapon
 * Returns TRUE if attempted, FALSE if cancelled
 */
bool enchant_spell(int num_hit, int num_dam, int num_ac)
{
	int item;
	bool okay = FALSE;

	object_type *o_ptr;

	char o_name[80];

	cptr q, s;


	/* Assume enchant weapon */
	item_tester_hook = item_tester_hook_weapon;

	/* Enchant armor if requested */
	if (num_ac) item_tester_hook = item_tester_hook_armour;

	/* Get an item */
	q = "Enchant which item? ";
	s = "You have nothing to enchant.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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


	/* Description */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

	/* Describe */
	msg_format("%s %s glow%s brightly!",
	           ((item >= 0) ? "Your" : "The"), o_name,
	           ((o_ptr->number > 1) ? "" : "s"));

	/* Enchant */
	if (enchant(o_ptr, num_hit, ENCH_TOHIT)) okay = TRUE;
	if (enchant(o_ptr, num_dam, ENCH_TODAM)) okay = TRUE;
	if (enchant(o_ptr, num_ac, ENCH_TOAC)) okay = TRUE;

	/* Failure */
	if (!okay)
	{
		/* Flush */
		if (flush_failure) flush();

		/* Message */
		msg_print("The enchantment failed.");
	}

	/* Something happened */
	return (TRUE);
}


/*
 * Identify an object in the inventory (or on the floor)
 * This routine does *not* automatically combine objects.
 * Returns TRUE if something was identified, else FALSE.
 */
bool ident_spell(void)
{
	int item;

	int squelch;

	object_type *o_ptr;

	cptr q, s;

	/* Only un-id'ed items */
	item_tester_hook = item_tester_unknown;

	/* Get an item */
	q = "Identify which item? ";
	s = "You have nothing to identify.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* Identify the object and get squelch setting */
	squelch = do_ident_item(item, o_ptr, 0);

	/* Now squelch it if needed */
	do_squelch_item(squelch, item, o_ptr);

	/* Something happened */
	return (TRUE);
}



/*
 * Fully "identify" an object in the inventory
 *
 * This routine returns TRUE if an item was identified.
 */
bool identify_fully(void)
{
	int item;

	int squelch;

	object_type *o_ptr;

	cptr q, s;

	/* Only un-*id*'ed items */
	item_tester_hook = item_tester_unknown_star;

	/* Get an item */
	q = "Identify which item? ";
	s = "You have nothing to identify.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* Mark the item as fully known */
	o_ptr->ident |= (IDENT_MENTAL);

	/* Identify the object and get the squelch setting */
	squelch = do_ident_item(item, o_ptr, 0);

	/* Now squelch it if needed */
	if (squelch == SQUELCH_YES)
	{
		do_squelch_item(squelch, item, o_ptr);
	}

	else
	{
		/* Describe it fully */
		object_info_screen(o_ptr);
	}

	/* Success */
	return (TRUE);
}




/*
 * Hook for "get_item()".  Determine if something is rechargable.
 */
bool item_tester_hook_recharge(const object_type *o_ptr)
{
	return (FALSE);
}



/************************************************************************
 *                                                                      *
 *                           Projection types                           *
 *                                                                      *
 ************************************************************************/


/*
 * Handle bolt spells.
 *
 * Bolts stop as soon as they hit a monster, whiz past missed targets, and
 * (almost) never affect items on the floor.
 */
bool project_bolt(int who, int rad, int y0, int x0, int y1, int x1, int dam,
                  int typ, u32b flg)
{
	/* Add the bolt bitflags */
	flg |= PROJECT_STOP | PROJECT_KILL | PROJECT_THRU | PROJECT_EFCT;

	/* Hurt the character unless he controls the spell */
	if (who != SOURCE_PLAYER) flg |= PROJECT_PLAY;

	/* Limit range */
	if ((rad > MAX_RANGE) || (rad <= 0)) rad = MAX_RANGE;

	/* Cast a bolt */
	return (project(who, rad, y0, x0, y1, x1, dam, typ, flg, 0, 0));
}

/*
 * Handle beam spells.
 *
 * Beams affect every grid they touch, go right through monsters, and
 * (almost) never affect items on the floor.
 */
bool project_beam(int who, int rad, int y0, int x0, int y1, int x1, int dam,
                  int typ, u32b flg)
{
	/* Add the beam bitflags */
	flg |= PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_EFCT;

	/* Hurt the character unless he controls the spell */
	if (who != SOURCE_PLAYER) flg |= (PROJECT_PLAY);

	/* Limit range */
	if ((rad > MAX_RANGE) || (rad <= 0)) rad = MAX_RANGE;

	/* Cast a beam */
	return (project(who, rad, y0, x0, y1, x1, dam, typ, flg, 0, 0));
}


/*
 * Handle ball spells.
 *
 * Balls act like bolt spells, except that they do not pass their target,
 * and explode when they hit a monster, a wall, their target, or the edge
 * of sight.  Within the explosion radius, they affect items on the floor.
 *
 * Balls may jump to the target, and have any source diameter (which affects
 * how quickly their damage falls off with distance from the center of the
 * explosion).
 */
bool project_ball(int who, int rad, int y0, int x0, int y1, int x1, int dam,
                  int typ, u32b flg, int source_diameter)
{
	/* Add the ball bitflags */
	flg |= PROJECT_BOOM | PROJECT_GRID | PROJECT_ITEM | PROJECT_KILL |
		PROJECT_WALL | PROJECT_EFCT;

	/* Add the STOP flag if appropriate */
	if ((who == SOURCE_PLAYER) &&
	    (!target_okay() || y1 != p_ptr->target_row || x1 != p_ptr->target_col))
	{
		flg |= (PROJECT_STOP);
	}

	/* Hurt the character unless he controls the spell */
	if (who != SOURCE_PLAYER) flg |= (PROJECT_PLAY);

	/* Limit radius to nine (up to 256 grids affected) */
	if (rad > 9) rad = 9;

	/* Cast a ball */
	return (project(who, rad, y0, x0, y1, x1, dam, typ, flg,
	                0, source_diameter));
}

/*
 * Handle ball spells that explode immediately on the target.
 * Whether monsters or the player is hurt must be determined
 * by the code that calls this function.
 */
bool explosion(int who, int rad, int y0, int x0, int dam, int typ, u32b flg)
{
	/* Add the explosion bitflags */
	flg |= 	PROJECT_BOOM | PROJECT_GRID | PROJECT_JUMP | PROJECT_ITEM;

	/* Explode */
	return (project_ball(who, rad, y0, x0, y0, x0, dam, typ, flg, 0));
}

/*
 * Handle monster-centered explosions.
 */
bool mon_explode(int who, int rad, int y0, int x0, int dam, int typ)
{
	return (project_ball(who, rad, y0, x0, y0, x0, dam, typ, 0L, 20));
}


/*
 * Handle arc spells.
 *
 * Arcs are a pie-shaped segment (with a width determined by "degrees")
 * of a explosion outwards from the source grid.  They are centered
 * along a line extending from the source towards the target.  -LM-
 *
 * Because all arcs start out as being one grid wide, arc spells with a
 * value for degrees of arc less than (roughly) 60 do not dissipate as
 * quickly.  In the extreme case where degrees of arc is 0, the arc is
 * actually a defined length beam, and loses no strength at all over the
 * ranges found in the game.
 *
 * Arcs affect items on the floor.
 */
bool project_arc(int who, int rad, int y0, int x0, int y1, int x1, int dam,
                 int typ, u32b flg, int degrees)
{
	/* Diameter of source of energy is normally, but not always, 20. */
	int source_diameter = 20;

	/* Radius of zero means no fixed limit. */
	if (rad == 0) rad = MAX_SIGHT;

	/* Calculate the effective diameter of the energy source, if necessary. */
	if (degrees < ARC_STANDARD_WIDTH)
	{
		if (degrees <= 9) source_diameter = rad * 10;
		else source_diameter = source_diameter * ARC_STANDARD_WIDTH / degrees;
	}

	/* If the arc has no spread, it's actually a beam */
	if (degrees <= 0)
	{
		/* Add the beam bitflags */
		flg |= (PROJECT_BEAM | PROJECT_KILL | PROJECT_THRU | PROJECT_WALL);

		source_diameter = 0;
	}

	/* If a full circle is asked for, we cast a ball spell. */
	else if (degrees >= 360)
	{
		/* Add the ball bitflags */
		flg |= PROJECT_STOP | PROJECT_BOOM | PROJECT_GRID |
		       PROJECT_ITEM | PROJECT_KILL | PROJECT_WALL;

		source_diameter = 0;
	}

	/* Otherwise, we fire an arc */
	else
	{
		/* Add the arc bitflags */
		flg |= PROJECT_ARC  | PROJECT_BOOM | PROJECT_GRID |
		       PROJECT_ITEM | PROJECT_KILL | PROJECT_THRU | PROJECT_EFCT;
	}

	/* Hurt the character unless he controls the spell */
	if (who != SOURCE_PLAYER) flg |= (PROJECT_PLAY);

	/* Cast an arc (or a ball) */
	return (project(who, rad, y0, x0, y1, x1, dam, typ, flg, degrees,
	                (byte)source_diameter));
}

/*
 * Handle starburst spells.
 *
 * Starbursts are randomized balls that use the same sort of code that
 * governs the shape of starburst rooms in the dungeon.  -LM-
 *
 * Starbursts always do full damage to every grid they affect:  however,
 * the chances of affecting grids drop off significantly towards the
 * edge of the starburst.  They always "jump" to their target and affect
 * items on the floor.
 */
bool project_star(int who, int rad, int y0, int x0, int dam, int typ, u32b flg)
{
	/* Add the star bitflags */
	flg |= PROJECT_STAR | PROJECT_BOOM | PROJECT_GRID | PROJECT_JUMP |
	       PROJECT_ITEM | PROJECT_KILL | PROJECT_EFCT;

	/* Hurt the character unless he controls the spell */
	if (who != SOURCE_PLAYER) flg |= (PROJECT_PLAY);

	/* Cast a star */
	return (project(who, rad, y0, x0, y0, x0, dam, typ, flg, 0, 0));
}

/*
 * Handle target grids for projections under the control of
 * the character.  - Chris Wilde, Morgul
 */
static void adjust_target(int dir, int y0, int x0, int *y1, int *x1)
{
	/* If no direction is given, and a target is, use the target. */
	if ((dir == 5) && target_okay())
	{
		*y1 = p_ptr->target_row;
		*x1 = p_ptr->target_col;
	}

	/* Otherwise, use the given direction */
	else
	{
		*y1 = y0 + MAX_RANGE * ddy[dir];
		*x1 = x0 + MAX_RANGE * ddx[dir];
	}
}



/*
 * Apply a "project()" directly to all monsters in view of a certain spot.
 *
 * Note that affected monsters are NOT auto-tracked by this usage.
 *
 * We are able to check LOS from either the character (in which case we
 * use line of fire for speed and accuracy), or from any given grid.
 *
 * To avoid misbehavior when monster deaths have side-effects,
 * this is done in two passes. -- JDL
 */
bool project_los(int y0, int x0, int dam, int typ)
{
	int i, d, x, y;

	u32b flg = PROJECT_JUMP | PROJECT_KILL | PROJECT_HIDE;

	bool obvious = FALSE;

	int who;

	/* Determine whether we are using LOF or LOS */
	bool line_of_fire = FALSE;

	if ((y0 == p_ptr->py) && (x0 == p_ptr->px))
	{
		line_of_fire = TRUE;
		who = SOURCE_PLAYER;
	}
	else if (cave_monster_trap_bold(y0, x0)) who = SOURCE_TRAP;
 	else if (cave_player_trap_bold(y0, x0))  who = SOURCE_EFFECT;
	else who = SOURCE_OTHER;

	/* Mark monsters in LOS */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Location */
		y = m_ptr->fy;
		x = m_ptr->fx;

		/* Apply character-centered test */
		if (line_of_fire)
		{
			/* Require line of fire */
			if (!player_can_fire_bold(y, x)) continue;
		}

		/* Apply generic grid test */
		else
		{
			/* Get distance between source and monster */
			d = distance(y0, x0, y, x);

			/* LOS extends only to max sight range */
			if (d > MAX_RANGE) continue;

			/* Check LOS if not at grid or adjacent */
			if (d > 1)
			{
				/* Ignore if not in LOS */
				if (!los(y0, x0, y, x)) continue;
			}
		}

		/* Mark the monster */
		m_ptr->mflag |= (MFLAG_TEMP);
	}

	/* Affect all (nearby) monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Skip unmarked monsters */
		if (!(m_ptr->mflag & (MFLAG_TEMP))) continue;

		/* Remove mark */
		m_ptr->mflag &= ~(MFLAG_TEMP);

		/* Jump directly to the monster */
		if (project_m(who, m_ptr->fy, m_ptr->fx, dam, typ, flg))
		{
			obvious = TRUE;
		}
	}

	/* Result */
	return (obvious);
}


/*
 * This routine clears the entire "temp" set.
 */
void clear_temp_array(void)
{
	int i;

	/* Apply flag changes */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);
	}

	/* None left */
	temp_n = 0;
}


/*
 * Aux function -- see below
 */
void cave_temp_mark(int y, int x, bool room)
{
	/* Avoid infinite recursion */
	if (cave_info[y][x] & (CAVE_TEMP)) return;

	/* Option -- do not leave the current room */
	if ((room) && (!(cave_info[y][x] & (CAVE_ROOM)))) return;

	/* Verify space */
	if (temp_n == TEMP_MAX) return;

	/* Mark the grid */
	cave_info[y][x] |= (CAVE_TEMP);

	/* Add it to the marked set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}

/*
 * Mark the nearby area with CAVE_TEMP flags.  Allow limited range.
 */
void spread_cave_temp(int y1, int x1, int range, bool room, bool pass_walls)
{
	int i, y, x;

	/* Add the initial grid */
	cave_temp_mark(y1, x1, room);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get marked, but stop further spread, unless pass_walls is TRUE */
		if (!pass_walls && !cave_project_bold(y, x)) continue;

		/* Note limited range (note:  we spread out one grid further) */
		if ((range) && (distance(y1, x1, y, x) >= range)) continue;

		/* Spread adjacent */
		cave_temp_mark(y + 1, x, room);
		cave_temp_mark(y - 1, x, room);
		cave_temp_mark(y, x + 1, room);
		cave_temp_mark(y, x - 1, room);

		/* Spread diagonal */
		cave_temp_mark(y + 1, x + 1, room);
		cave_temp_mark(y - 1, x - 1, room);
		cave_temp_mark(y - 1, x + 1, room);
		cave_temp_mark(y + 1, x - 1, room);
	}
}


/*
 * Speed monsters
 */
bool speed_monsters(void)
{
	return (project_los(p_ptr->py, p_ptr->px, p_ptr->lev, GF_OLD_SPEED));
}

/*
 * Slow monsters
 */
bool slow_monsters(int power)
{
	return (project_los(p_ptr->py, p_ptr->px, power, GF_OLD_SLOW));
}

/*
 * Sleep monsters
 */
bool sleep_monsters(int power)
{
	return (project_los(p_ptr->py, p_ptr->px, power, GF_OLD_SLEEP));
}


/*
 * Banish evil monsters
 */
bool banish_evil(int dist)
{
	return (project_los(p_ptr->py, p_ptr->px, dist, GF_AWAY_EVIL));
}


/*
 * Turn undead
 */
bool turn_undead(int power)
{
	return (project_los(p_ptr->py, p_ptr->px, power, GF_TURN_UNDEAD));
}


/*
 * Dispel undead monsters
 */
bool dispel_undead(int dam)
{
	return (project_los(p_ptr->py, p_ptr->px, dam, GF_DISP_UNDEAD));
}

/*
 * Dispel evil monsters
 */
bool dispel_evil(int dam)
{
	return (project_los(p_ptr->py, p_ptr->px, dam, GF_DISP_EVIL));
}

/*
 * Dispel all monsters
 */
bool dispel_monsters(int dam)
{
	return (project_los(p_ptr->py, p_ptr->px, dam, GF_DISP_ALL));
}





/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void aggravate_monsters(int who)
{
	int y, x;

	/*if a monster, use monster coordinates*/
	if(who > SOURCE_MONSTER_START)
	{
		monster_type *m_ptr = &mon_list[who];

		x = m_ptr->fx;
		y = m_ptr->fy;
	}

	/*use the player coordinates*/
	else
	{
		x = p_ptr->px;
		y = p_ptr->py;
	}

	/* Messages */
	(void)project_los(y, x, (250 + rand_int(250)), GF_OLD_SPEED);

}


/*
 * Wake up all monsters, and speed up "los" monsters.
 */
void mass_aggravate_monsters(int who)
{
	int i;

	/* Aggravate everyone */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip aggravating monster (or player) */
		if (i == who) continue;

		/* Wake up all monsters */
		if (m_ptr->csleep)
		{
			/* Wake up */
			m_ptr->csleep = 0;
		}

		/* Speed up monsters in line of sight */
		if (player_has_los_bold(m_ptr->fy, m_ptr->fx))
		{
			set_monster_haste(cave_m_idx[m_ptr->fy][m_ptr->fx],
							  m_ptr->hasted + 50 + rand_int(50), m_ptr->ml);
		}

		/*possibly update the monster health bar*/
		if (p_ptr->health_who == i) p_ptr->redraw |= (PR_HEALTH);

	}
}

/*
 * Delete all non-unique monsters of a given "type" from the level
 */
bool banishment(void)
{
	int i;

	char typ;

	/* Mega-Hack -- Get a monster symbol */
	if (!get_com("Choose a monster race (by symbol) to banish: ", &typ))
		return FALSE;

	/* Delete the monsters of that "type" */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Hack -- Skip Unique Monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) continue;

		/* Quest monsters can only be "killed" by the player */
		if (m_ptr->mflag & (MFLAG_QUEST)) continue;

		/* Skip "wrong" monsters */
		if (r_ptr->d_char != typ) continue;

		/* Delete the monster */
		delete_monster_idx(i);

		/* Take some damage */
		take_hit(randint(4), "the strain of casting Banishment");

	}

	/* Update monster list window */
	p_ptr->window |= PW_MONLIST;

	/* Success */
	return TRUE;

}


/*
 * Delete all nearby (non-unique) monsters
 */
bool mass_banishment(void)
{
	int i;

	bool result = FALSE;


	/* Delete the (nearby) monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];
		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Paranoia -- Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Hack -- Skip unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE)) continue;

		/* Skip distant monsters */
		if (m_ptr->cdis > MAX_SIGHT) continue;

		/* Quest monsters can only be "killed" by the player */
		if (m_ptr->mflag & (MFLAG_QUEST)) continue;

		/* Delete the monster */
		delete_monster_idx(i);

		/* Take some damage */
		take_hit(randint(3), "the strain of casting Mass Banishment");

		/* Note effect */
		result = TRUE;
	}

	/* Update monster list window */
	p_ptr->window |= PW_MONLIST;

	return (result);
}



/*
 * Learn a great deal about a given monster
 */
bool probing(void)
{


	/*let the player select one monster or feature*/
	if (!(target_set_interactive(TARGET_LOOK)))
	{
		return(FALSE);
	}

	/*Nothing set - paranoia*/
	if (!p_ptr->target_set) return (FALSE);

	/*We selected a terrain*/
	if (!p_ptr->target_who)
	{
		int f_idx = cave_feat[p_ptr->target_row][p_ptr->target_col];

		/*Learn about the feature*/
   		lore_do_probe_feature(f_idx);

		/* Save screen */
		screen_save();

		/*display the monster info*/
		display_feature_roff(f_idx);

		/*give the player a look at the updated feature info*/
		put_str("Press any key to continue.  ", 0, 40);

		inkey();

		/* Load screen */
		screen_load();
	}

	/*We selected a monster*/
	else
	{
		int m_idx = p_ptr->target_who;
		monster_type *m_ptr = &mon_list[m_idx];
		char m_name[80];

		/* Learn about the monsters */
		lore_do_probe_monster(m_idx);

		/* Save screen */
		screen_save();

		/*display the monster info*/
		display_roff(m_ptr->r_idx);

		/*give the player a look at the updated monster info*/
		put_str("Press any key to continue.  ", 0, 40);

		inkey();

		/* Load screen */
		screen_load();

		/* Get "the monster" or "something" */
		monster_desc(m_name, sizeof(m_name), m_ptr, 0x04);

		/* Describe the monster */
		msg_format("%^s has %d hit points.", m_name, m_ptr->hp);
	}

	/* Result */
	return (TRUE);
}



/*
 * The spell of destruction
 *
 * This spell "deletes" monsters (instead of "killing" them).
 *
 * Later we may use one function for both "destruction" and
 * "earthquake" by using the "full" to select "destruction".
 */
void destroy_area(int y1, int x1, int r, bool full)
{
	int y, x, k, t;

	bool flag = FALSE;

	/* Unused parameter */
	(void)full;

	/* No effect in town */
	if (!p_ptr->depth)
	{
		msg_print("The ground shakes for a moment.");
		return;
	}

	/* Big area of affect */
	for (y = (y1 - r); y <= (y1 + r); y++)
	{
		for (x = (x1 - r); x <= (x1 + r); x++)
		{
			/* Skip illegal grids */
			if (!in_bounds_fully(y, x)) continue;

			/* Extract the distance */
			k = distance(y1, x1, y, x);

			/* Stay in the circle of death */
			if (k > r) continue;

			/* Lose room and vault */
			cave_info[y][x] &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			cave_info[y][x] &= ~(CAVE_GLOW | CAVE_MARK);

			/* Hack -- Notice player affect */
			if (cave_m_idx[y][x] < 0)
			{
				/* Hurt the player later */
				flag = TRUE;

				/* Do not hurt this grid */
				continue;
			}

			/* Hack -- Quest monsters are unaffected */
			else if (cave_m_idx[y][x] > 0)
			{
				monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];

				if (m_ptr->mflag & (MFLAG_QUEST | MFLAG_QUEST_SUMMON)) continue;
			}


			/* Hack -- Skip the epicenter */
			if ((y == y1) && (x == x1)) continue;

			/* Delete the monster (if any) */
			delete_monster(y, x);

			if (cave_valid_bold(y, x))
			{
				int feat = FEAT_FLOOR;

				/* Delete objects */
				delete_object(y, x);

				/* Wall (or floor) type */
				t = rand_int(200);

				/* Burn stuff */
				if (cave_ff2_match(y, x, FF2_HURT_FIRE))
				{
					feat = feat_state(cave_feat[y][x],
						FS_HURT_FIRE);
				}

				/* Granite */
				else if (t < 20)
				{
					/* Create granite wall */
					feat = FEAT_WALL_EXTRA;
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					feat = FEAT_QUARTZ;
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					feat = FEAT_MAGMA;
				}

				/* Rubble */
				else if (t < 130)
				{
					/* Create rubble */
					feat = FEAT_RUBBLE;
				}

				/* Change the feature */
				cave_set_feat(y, x, feat);
			}
		}
	}

	/* Hack -- Affect player */
	if (flag)
	{
		/* Message */
		msg_print("There is a searing blast of light!");

		/* Blind the player */
		if (!p_ptr->resist_blind && !p_ptr->resist_lite)
		{
			/* Become blind */
			(void)set_blind(p_ptr->blind + 10 + randint(10));
		}
	}

	/* Hard not to notice */
	add_wakeup_chance = 10000;

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_MAP | PW_MONLIST);
}


/*
 * Induce an "earthquake" of the given radius at the given location.
 *
 * This will turn some walls into floors and some floors into walls.
 *
 * The player will take damage and "jump" into a safe grid if possible,
 * otherwise, he will "tunnel" through the rubble instantaneously.
 *
 * Monsters will take damage, and "jump" into a safe grid if possible,
 * otherwise they will be "buried" in the rubble, disappearing from
 * the level in the same way that they do when banished.
 *
 * Note that players and monsters (except eaters of walls and passers
 * through walls) will never occupy the same grid as a wall (or door).
 */
void earthquake(int cy, int cx, int r)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, t, y, x, yy, xx, dy, dx;

	int damage = 0;

	int sn = 0, sy = 0, sx = 0;

	bool hurt = FALSE;

	bool map[32][32];

	/* No effect in town */
	if (!p_ptr->depth)
	{
		msg_print("The ground shakes for a moment.");
		return;
	}

	/* Paranoia -- Enforce maximum range */
	if (r > 12) r = 12;

	/* Clear the "maximal blast" area */
	for (y = 0; y < 32; y++)
	{
		for (x = 0; x < 32; x++)
		{
			map[y][x] = FALSE;
		}
	}

	/* Check around the epicenter */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip illegal grids */
			if (!in_bounds_fully(yy, xx)) continue;

			/* Skip distant grids */
			if (distance(cy, cx, yy, xx) > r) continue;

			/* Lose room and vault */
			cave_info[yy][xx] &= ~(CAVE_ROOM | CAVE_ICKY);

			/* Lose light and knowledge */
			cave_info[yy][xx] &= ~(CAVE_GLOW | CAVE_MARK);

			/* Skip the epicenter */
			if (!dx && !dy) continue;

			/* Skip most grids */
			if (rand_int(100) < 85) continue;

			/* Hack - make quest monsters immune*/
			if (cave_m_idx[yy][xx] > 0)
			{
				monster_type *m_ptr = &mon_list[cave_m_idx[yy][xx]];
				if (m_ptr->mflag & (MFLAG_QUEST | MFLAG_QUEST_SUMMON))  continue;
			}

			/* Damage this grid */
			map[16+yy-cy][16+xx-cx] = TRUE;

			/* Hack -- Take note of player damage */
			if ((yy == py) && (xx == px)) hurt = TRUE;
		}
	}

	/* First, affect the player (if necessary) */
	if (hurt)
	{
		/* Check around the player */
		for (i = 0; i < 8; i++)
		{
			/* Get the location */
			y = py + ddy_ddd[i];
			x = px + ddx_ddd[i];

			/* Skip non-empty grids */
			if (!cave_empty_bold(y, x)) continue;

			/* Important -- Skip "quake" grids */
			if (map[16+y-cy][16+x-cx]) continue;

			/* Count "safe" grids, apply the randomizer */
			if ((++sn > 1) && (rand_int(sn) != 0)) continue;

			/* Save the safe location */
			sy = y; sx = x;
		}

		/* Random message */
		switch (randint(3))
		{
			case 1:
			{
				msg_print("The cave ceiling collapses!");
				break;
			}
			case 2:
			{
				msg_print("The cave floor twists in an unnatural way!");
				break;
			}
			default:
			{
				msg_print("The cave quakes!");
				msg_print("You are pummeled with debris!");
				break;
			}
		}

		/* Hurt the player a lot */
		if (!sn)
		{
			/* Message and damage */
			msg_print("You are severely crushed!");
			damage = 300;
		}

		/* Destroy the grid, and push the player to safety */
		else
		{
			/* Calculate results */
			switch (randint(3))
			{
				case 1:
				{
					msg_print("You nimbly dodge the blast!");
					damage = 0;
					break;
				}
				case 2:
				{
					msg_print("You are bashed by rubble!");
					damage = damroll(10, 4);
					(void)set_stun(p_ptr->stun + randint(50));
					break;
				}
				case 3:
				{
					msg_print("You are crushed between the floor and ceiling!");
					damage = damroll(10, 4);
					(void)set_stun(p_ptr->stun + randint(50));
					break;
				}
			}

			/* Move player */
			monster_swap(py, px, sy, sx);
		}

		/* Take some damage */
		if (damage) take_hit(damage, "an earthquake");
	}


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Process monsters */
			if (cave_m_idx[yy][xx] > 0)
			{
				monster_type *m_ptr = &mon_list[cave_m_idx[yy][xx]];
				monster_race *r_ptr = &r_info[m_ptr->r_idx];

				/* Most monsters cannot co-exist with rock */
				if (!(r_ptr->flags2 & (RF2_KILL_WALL)) &&
				    !(r_ptr->flags2 & (RF2_PASS_WALL)))
				{
					char m_name[80];

					/* Assume not safe */
					sn = 0;

					/* Monster can move to escape the wall */
					if (!(r_ptr->flags1 & (RF1_NEVER_MOVE)))
					{
						/* Look for safety */
						for (i = 0; i < 8; i++)
						{
							/* Get the grid */
							y = yy + ddy_ddd[i];
							x = xx + ddx_ddd[i];

							/* Skip non-empty grids */
							if (!cave_empty_bold(y, x)) continue;

							/* Hack -- no safety on glyph of warding */
							if (cave_player_glyph_bold(y, x)) continue;

							/* Important -- Skip "quake" grids */
							if (map[16+y-cy][16+x-cx]) continue;

							/* Count "safe" grids, apply the randomizer */
							if ((++sn > 1) && (rand_int(sn) != 0)) continue;

							/* Save the safe grid */
							sy = y;
							sx = x;
						}
					}

					/* Describe the monster */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Scream in pain */
					msg_format("%^s wails out in pain!", m_name);

					/* Take damage from the quake */
					damage = (sn ? damroll(4, 8) : (m_ptr->hp + 1));

					/* Monster is certainly awake */
					m_ptr->csleep = 0;

					/* Apply damage directly */
					m_ptr->hp -= damage;

					/* Delete (not kill) "dead" monsters */
					if (m_ptr->hp < 0)
					{
						/* Message */
						msg_format("%^s is embedded in the rock!", m_name);

						/* Delete the monster */
						delete_monster(yy, xx);

						/* No longer safe */
						sn = 0;
					}

					/* Hack -- Escape from the rock */
					if (sn)
					{
						/* Move the monster */
						monster_swap(yy, xx, sy, sx);
					}
				}
			}
		}
	}


	/* XXX XXX XXX */

	/* New location */
	py = p_ptr->py;
	px = p_ptr->px;

	/* Important -- no wall on player */
	map[16+py-cy][16+px-cx] = FALSE;


	/* Examine the quaked region */
	for (dy = -r; dy <= r; dy++)
	{
		for (dx = -r; dx <= r; dx++)
		{
			/* Extract the location */
			yy = cy + dy;
			xx = cx + dx;

			/* Skip unaffected grids */
			if (!map[16+yy-cy][16+xx-cx]) continue;

			/* Paranoia -- never affect player */
			if ((yy == py) && (xx == px)) continue;

			/* Destroy location (if valid) */
			if (cave_valid_bold(yy, xx))
			{
				int feat = FEAT_FLOOR;

				bool floor = (cave_ff1_match(yy, xx, FF1_MOVE) != 0);

				/* Delete objects */
				delete_object(yy, xx);

				/* Wall (or floor) type */
				t = (floor ? rand_int(100) : 200);

				/* Granite */
				if (t < 20)
				{
					/* Create granite wall */
					feat = FEAT_WALL_EXTRA;
				}

				/* Quartz */
				else if (t < 70)
				{
					/* Create quartz vein */
					feat = FEAT_QUARTZ;
				}

				/* Magma */
				else if (t < 100)
				{
					/* Create magma vein */
					feat = FEAT_MAGMA;
				}

				/* Change the feature */
				cave_set_feat(yy, xx, feat);
			}
		}
	}

	/* Hard not to notice */
	add_wakeup_chance = MAX(add_wakeup_chance, 8000);

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Update the health bar */
	p_ptr->redraw |= (PR_HEALTH | PR_MON_MANA);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD | PW_MAP | PW_MONLIST);
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will Perma-Lite all "temp" grids.
 *
 * This routine is used (only) by "lite_room()"
 *
 * Dark grids are illuminated.
 *
 * Also, process all affected monsters.
 *
 * SMART monsters always wake up when illuminated
 * NORMAL monsters wake up 1/4 the time when illuminated
 * STUPID monsters wake up 1/10 the time when illuminated
 */
static void cave_temp_room_lite(void)
{
	int i;

	/* Apply flag changes */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Perma-Lite */
		cave_info[y][x] |= (CAVE_GLOW);
	}

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Update stuff */
	update_stuff();

	/* Process the grids */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		/* Redraw the grid */
		lite_spot(y, x);

		/* Process affected monsters */
		if (cave_m_idx[y][x] > 0)
		{
			int chance = 25;

			monster_type *m_ptr = &mon_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Stupid monsters rarely wake up */
			if (r_ptr->flags2 & (RF2_STUPID)) chance = 10;

			/* Smart monsters always wake up */
			if (r_ptr->flags2 & (RF2_SMART)) chance = 100;

			/* Sometimes monsters wake up */
			if (m_ptr->csleep && (rand_int(100) < chance))
			{
				/* Wake up! */
				m_ptr->csleep = 0;

				/* Notice the "waking up" */
				if ((m_ptr->ml) && (!m_ptr->mimic_k_idx) && (disturb_wakeup))
				{
					char m_name[80];

					/* Get the monster name */
					monster_desc(m_name, sizeof(m_name), m_ptr, 0);

					/* Dump a message */
					msg_format("%^s wakes up.", m_name);
				} else if (!(m_ptr->mimic_k_idx))
				{
					p_ptr->n_woken += 1;
				}

				/*possibly update the monster health bar*/
				if (p_ptr->health_who == cave_m_idx[m_ptr->fy][m_ptr->fx])
					p_ptr->redraw |= (PR_HEALTH);
			}
		}
	}

	/* None left */
	temp_n = 0;
}



/*
 * This routine clears the entire "temp" set.
 *
 * This routine will "darken" all "temp" grids.
 *
 * In addition, some of these grids will be "unmarked".
 *
 * This routine is used (only) by "unlite_room()"
 */
static void cave_temp_room_unlite(void)
{
	int i;

	/* Apply flag changes */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		/* No longer in the array */
		cave_info[y][x] &= ~(CAVE_TEMP);

		/* Turn off the light */
		cave_info[y][x] &= ~(CAVE_GLOW);

		/* Hack -- Forget "boring" grids */
		if (!(cave_info[y][x] & (CAVE_HALO)) &&
			!cave_ff1_match(y, x, FF1_REMEMBER) &&
			(!cave_any_trap_bold(y, x) ||
				(x_list[cave_x_idx[y][x]].x_flags &
				(EF1_HIDDEN))))
		{
			/* Forget the grid */
			cave_info[y][x] &= ~(CAVE_MARK);
		}
	}

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Update stuff */
	update_stuff();

	/* Process the grids */
	for (i = 0; i < temp_n; i++)
	{
		int y = temp_y[i];
		int x = temp_x[i];

		/* Redraw the grid */
		lite_spot(y, x);
	}

	/* None left */
	temp_n = 0;
}




/*
 * Aux function -- see below
 */
static void cave_temp_room_aux(int y, int x)
{
	/* Avoid infinite recursion */
	if (cave_info[y][x] & (CAVE_TEMP)) return;

	/* Do not "leave" the current room */
	if (!(cave_info[y][x] & (CAVE_ROOM))) return;

	/* Paranoia -- verify space */
	if (temp_n == TEMP_MAX) return;

	/* Mark the grid as "seen" */
	cave_info[y][x] |= (CAVE_TEMP);

	/* Add it to the "seen" set */
	temp_y[temp_n] = y;
	temp_x[temp_n] = x;
	temp_n++;
}




/*
 * Illuminate any room containing the given location.
 */
void lite_room(int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_room_aux(y1, x1);

	/* While grids are in the queue, add their neighbors */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get lit, but stop light */
		if (!cave_project_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_room_aux(y + 1, x);
		cave_temp_room_aux(y - 1, x);
		cave_temp_room_aux(y, x + 1);
		cave_temp_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_room_aux(y + 1, x + 1);
		cave_temp_room_aux(y - 1, x - 1);
		cave_temp_room_aux(y - 1, x + 1);
		cave_temp_room_aux(y + 1, x - 1);
	}

	/* Now, lite them all up at once */
	cave_temp_room_lite();
}


/*
 * Darken all rooms containing the given location
 */
void unlite_room(int y1, int x1)
{
	int i, x, y;

	/* Add the initial grid */
	cave_temp_room_aux(y1, x1);

	/* Spread, breadth first */
	for (i = 0; i < temp_n; i++)
	{
		x = temp_x[i], y = temp_y[i];

		/* Walls get dark, but stop darkness */
		if (!cave_project_bold(y, x)) continue;

		/* Spread adjacent */
		cave_temp_room_aux(y + 1, x);
		cave_temp_room_aux(y - 1, x);
		cave_temp_room_aux(y, x + 1);
		cave_temp_room_aux(y, x - 1);

		/* Spread diagonal */
		cave_temp_room_aux(y + 1, x + 1);
		cave_temp_room_aux(y - 1, x - 1);
		cave_temp_room_aux(y - 1, x + 1);
		cave_temp_room_aux(y + 1, x - 1);
	}

	/* Now, darken them all at once */
	cave_temp_room_unlite();
}



/*
 * Hack -- call light around the player
 * Affect all monsters in the projection radius
 */
bool lite_area(int dam, int rad)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
		msg_print("You are surrounded by a white light.");
	}

	/* Hook into the "project()" function */
	(void)project(SOURCE_PLAYER, rad, py, px, py, px, dam, GF_LITE_WEAK, flg, 0, 0);

	/* Lite up the room */
	lite_room(py, px);

	/* Assume seen */
	return (TRUE);
}


/*
 * Hack -- call darkness around the player
 * Affect all monsters in the projection radius
 */
bool unlite_area(int dam, int rad)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_GRID | PROJECT_KILL;

	/* Hack -- Message */
	if (!p_ptr->blind)
	{
		msg_print("Darkness surrounds you.");
	}

	/* Hook into the "project()" function */
	(void)project(SOURCE_PLAYER, rad, py, px, py, px, dam, GF_DARK_WEAK, flg, 0, 0);

	/* Lite up the room */
	unlite_room(py, px);

	/* Assume seen */
	return (TRUE);
}

/*
 * Character casts a special-purpose bolt or beam spell.
 */
bool fire_bolt_beam_special(int typ, int dir, int dam, int rad, u32b flg)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* This is a beam spell */
	if (flg & (PROJECT_BEAM))
	{
		/* Cast a beam */
		return (project_beam(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam,
	                        typ, flg));
	}

	/* This is a bolt spell */
	else
	{
		/* Cast a bolt */
		return (project_bolt(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam,
									typ, flg));
	}
}

/*
 * Player casts a orb spell that creates an effect, but does not affect anything else.
 */
bool fire_effect_orb(int typ, int dir, int dam, int rad)
{
	int y1, x1;
	u32b flg = 0L;

	/* Add the ball bitflags */
	flg |= (PROJECT_BOOM | PROJECT_WALL | PROJECT_EFCT | PROJECT_CLOUD);

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Add the STOP flag if appropriate */
	if (!target_okay() || y1 != p_ptr->target_row || x1 != p_ptr->target_col)
	{
		flg |= (PROJECT_STOP);
	}

	/* Limit radius to nine (up to 256 grids affected) */
	if (rad > 9) rad = 9;

	/* Cast a ball */
	return (project(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam, typ, flg, 0L, 10 + rad * 10));
}

/*
 * Character casts a (simple) ball spell.
 */
bool fire_ball(int typ, int dir, int dam, int rad)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast a (simple) ball */
	return (project_ball(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam, typ,
	                     0L, 0));
}

/*
 * Character casts an orb spell (a ball that loses no strength out
 * from the origin).
 */
bool fire_orb(int typ, int dir, int dam, int rad)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast an orb */
	return (project_ball(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam, typ,
	                     0L, 10 + rad * 10));
}

/*
 * Character casts a ball spell with a specified source diameter, that
 * jumps to the target, or does various other special things.
 */
bool fire_ball_special(int typ, int dir, int dam, int rad, u32b flg,
                       int source_diameter)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast a ball with specified source diameter */
	return (project_ball(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam, typ,
	                     flg, source_diameter));
}

/*
 * Character casts an arc spell.
 */
bool fire_arc(int typ, int dir, int dam, int rad, int degrees)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast an arc */
	return (project_arc(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam, typ,
	                    0L, degrees));
}

/*
 * Character casts a star-shaped spell.
 */
bool fire_star(int typ, int dam, int rad, u32b flg)
{
	/* Cast a star */
	return (project_star(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, dam, typ, flg));
}


/*
 * Fire a number of bolts, beams, or arcs that start in semi-random grids
 * near the character, and head in totally random directions.  The larger
 * the number of grids in the area of fire, and the more monsters inhabit
 * those grids, the more effective this spell is.
 * -LM-
 */
void fire_storm(int who, int typ0, int y0, int x0, int dam, int rad, int len,
	byte projection, bool lingering)
{
	int i, j;
	int y, x = 0, y1, x1, last_y, last_x;
	int dir;
	int typ;
	long num_missiles;
	int choice;
	monster_type *m_ptr;

	/* Save standard delay */
	int std_delay = op_ptr->delay_factor;

	/* Array of grids (max radius is 20) */
	u16b grid[1681];

	/* Grid count */
	int grid_count = 0;

	/* Array of monsters to hurt (indexes, initial HPs) */
	s16b mon_array[100][2];

	/* Monster count */
	int mon_count = 0;

	/* Allow spell graphics to accumulate */
	u32b flg = (lingering ? PROJECT_NO_REDRAW : 0L);


	/* We can't handle a radius of more than 20 */
	if (rad > 20) rad = 20;

	/* Very little delay while projecting each missile */
	op_ptr->delay_factor = (std_delay + 1) / 2;


	/* Build up an array of all nearby projectable grids */
	for (y = y0 - rad; y <= y0 + rad; y++)
	{
		for (x = x0 - rad; x <= x0 + rad; x++)
		{
			/* Stay legal */
			if (!in_bounds(y, x)) continue;

			/* Require that grid be projectable */
			if (projectable(y0, x0, y, x, 0L))
			{
				/* Convert location to a grid, save and count it */
				grid[grid_count++] = GRID(y, x);
			}
		}
	}


	/* Scan the monster list */
	for (i = 0; i < mon_max; i++)
	{
		/* Get this monster */
		m_ptr = &mon_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Skip monsters not in LOF (or LOS), and if too far away */
		if ((y0 == p_ptr->py) && (x0 == p_ptr->px))
		{
			if (!player_can_fire_bold(m_ptr->fy, m_ptr->fx)) continue;
			if (m_ptr->cdis > rad) continue;
		}
		else
		{
			if (!los(y0, x0, m_ptr->fy, m_ptr->fx)) continue;
			if (distance(y0, x0, m_ptr->fy, m_ptr->fx) > rad) continue;
		}

		/* Store this monster and its current HPs */
		if (mon_count < 100)
		{
			mon_array[mon_count][0] = i;
			mon_array[mon_count][1] = m_ptr->hp;
			mon_count++;
		}
	}


	/* Calculate the minimum number of missiles */
	num_missiles = MAX(1L, grid_count / 8);

	/* Handle each missile in turn */
	for (i = 0;; i++)
	{
		/* Limit -- never fire more than num_missiles * 8 */
		if (i > num_missiles * 8) break;

		/* We've used up our guaranteed missiles */
		if (i >= num_missiles)
		{
			/* Assume we stop */
			bool stop = TRUE;

			/* Keep firing until all monsters have been hurt */
			for (j = 0; j < mon_count; j++)
			{
				/* Get this monster */
				m_ptr = &mon_list[mon_array[j][0]];

				/* Skip dead monsters */
				if (!m_ptr->r_idx) continue;

				/* Skip monsters with HPs < initial value */
				if (m_ptr->hp < mon_array[j][1]) continue;

				/* This monster hasn't been hurt - keep firing */
				stop = FALSE;
				break;
			}

			/* Stop if all monsters have been hurt */
			if (stop) break;
		}


		/* Start with a very far away location */
		last_y = -255;
		last_x = -255;

		/* Bias for closer grids */
		for (j = 0; j < 3; j++)
		{
			/* Choose a grid at random */
			choice = rand_int(grid_count);

			/* Get the coordinates */
			y = GRID_Y(grid[choice]);
			x = GRID_X(grid[choice]);

			/* Save if less than previous distance */
			if (distance(y, x, y0, x0) < distance(last_x, last_x, y0, x0))
			{
				/* Save these coordinates */
				last_y = y;
				last_x = x;
			}
		}

		/* No movement */
		dir = 5;

		/* Get any direction other than 5 */
		while (dir == 5) dir = randint(9);

		/* Get target grid */
		y1 = last_y + ddy[dir];
		x1 = last_x + ddx[dir];


		/* Allow wizardly projection types */
		if (typ0 == -1)
		{
			choice = rand_int(12);

			if      (choice ==  1) typ = GF_FIRE;
			else if (choice ==  2) typ = GF_COLD;
			else if (choice ==  3) typ = GF_ACID;
			else if (choice ==  4) typ = GF_ELEC;
			else if (choice ==  5) typ = GF_POIS;
			else if (choice ==  6) typ = GF_LITE;
			else if (choice ==  7) typ = GF_DARK;
			else if (choice ==  8) typ = GF_NEXUS;
			else if (choice ==  9) typ = GF_CONFUSION;
			else if (choice == 10) typ = GF_SOUND;
			else if (choice == 11) typ = GF_SHARD;
			else                   typ = GF_CHAOS;
		}

		/* Allow light, darkness, and confusion */
		else if (typ0 == -2)
		{
			choice = rand_int(3);

			if      (choice == 1) typ = GF_LITE;
			else if (choice == 2) typ = GF_DARK;
			else                  typ = GF_CONFUSION;
		}

		/* Use given projection */
		else
		{
			typ = typ0;
		}

		/* Fire a projection using the calculated data */
		if (projection == 0)
		{
			(void)project_bolt(who, len, last_y, last_x, y1, x1, dam, typ, flg);
		}
		else if (projection == 1)
		{
			(void)project_beam(who, len, last_y, last_x, y1, x1, dam, typ, flg);
		}
		else if (projection == 2)
		{
			/* Used for the "Prismatic Armageddon" spell */
			(void)project_arc(who, rand_range(len - 1, len + 1), last_y, last_x,
				y1, x1, dam, typ, flg, rand_range(40, 55));
		}
		else if (projection == 3)
		{
			(void)project_ball(who, rad, y1, x1, y1, x1, dam, typ, flg, 0);
		}
	}

	/* We allowed spell graphics to accumulate */
	if (lingering)
	{

		int ty = Term->offset_y + SCREEN_HGT;
		int tx = Term->offset_x + SCREEN_WID;

		/* Clear all lingering spell effects on screen XXX */
		for (y = Term->offset_y; y < ty; y++)
		{
			for (y = Term->offset_x; x < tx; x++)
			{
				lite_spot(y, x);
			}
		}
	}

	/* Restore standard delay */
	op_ptr->delay_factor = std_delay;
}


/*
 * Fire beams in random directions.
 */
bool beam_burst(int y, int x, int typ, int num, int dam)
{
	int i, yy, xx;

	bool notice = FALSE;

	int old_delay = op_ptr->delay_factor;

	/* Require legal centerpoint */
	if (!in_bounds_fully(y, x)) return (FALSE);


	/* Hack -- lower delay factor */
	if (op_ptr->delay_factor)
	{
		op_ptr->delay_factor = (op_ptr->delay_factor + 1) / 2;
	}

	/* Fire beams in all directions */
	for (i = 0; i < num; i++)
	{
		/* Get a totally random grid within six grids from current position */
		yy = rand_spread(y, 6);
		xx = rand_spread(x, 6);

		/* Fire a beam of (strong) light towards it */
		if (project(-1, 0, y, x, yy, xx, dam, typ,
			PROJECT_BEAM | PROJECT_KILL | PROJECT_EFCT, 0, 0)) notice = TRUE;
	}

	/* Restore standard delay */
	op_ptr->delay_factor = old_delay;

	/* Return "anything noticed" */
	return (notice);
}

/*
 * Cast multiple non-jumping ball spells at the same target.
 *
 * Targets absolute coordinates instead of a specific monster, so that
 * the death of the monster doesn't change the target's location.
 */
bool fire_swarm(int num, int typ, int dir, int dam, int rad)
{
	bool noticed = FALSE;

	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	while (num--)
	{
		/* Analyze the "dir" and the "target".  Hurt items on floor. */
		if (project_ball(SOURCE_PLAYER, rad, p_ptr->py, p_ptr->px, y1, x1, dam,
				typ, 0L, 0)) noticed = TRUE;

		/* Hack -- Avoid cloned messages */
		if (size_mon_msg > 0) flush_monster_messages();
	}

	return noticed;
}


/*
 * Character casts a bolt spell.
 */
bool fire_bolt(int typ, int dir, int dam)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast a bolt */
	return (project_bolt(SOURCE_PLAYER, MAX_RANGE, p_ptr->py, p_ptr->px, y1, x1, dam,
	                     typ, 0L));
}

/*
 * Character casts a beam spell.
 */
bool fire_beam(int typ, int dir, int dam, u32b flg)
{
	int y1, x1;

	/* Get target */
	adjust_target(dir, p_ptr->py, p_ptr->px, &y1, &x1);

	/* Cast a beam */
	return (project_beam(-1, MAX_RANGE, p_ptr->py, p_ptr->px, y1, x1, dam,
	                     typ, flg));
}


/*
 * Cast a bolt or a beam spell
 */
bool fire_bolt_or_beam(int prob, int typ, int dir, int dam)
{
	if (rand_int(100) < prob)
	{
		return (fire_beam(typ, dir, dam, 0L));
	}
	else
	{
		return (fire_bolt(typ, dir, dam));
	}
}

/*
 * Some of the old functions
 */

bool lite_line(int dir, int dam)
{
	u32b flg = PROJECT_BEAM | PROJECT_GRID;
	return (fire_bolt_beam_special(GF_LITE_WEAK, dir, dam, MAX_RANGE, flg));
}

bool strong_lite_line(int dir)
{
	u32b flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_KILL;
	return (fire_bolt_beam_special(GF_LITE, dir, damroll(10, 8), MAX_RANGE, flg));
}

bool drain_life(int dir, int dam)
{
	u32b flg = PROJECT_STOP | PROJECT_KILL;
	return (fire_bolt_beam_special(GF_LIFE_DRAIN, dir, dam, MAX_RANGE, flg));
}

bool wall_to_mud(int dir, int dam)
{
	u32b flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (fire_bolt_beam_special(GF_KILL_WALL, dir, dam, MAX_RANGE, flg));
}

bool destroy_door(int dir)
{
	u32b flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (fire_bolt_beam_special(GF_KILL_DOOR, dir, 0, MAX_RANGE, flg));
}

bool disarm_trap(int dir)
{
	u32b flg = PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM;
	return (fire_bolt_beam_special(GF_KILL_TRAP, dir, 0, MAX_RANGE, flg));
}

bool heal_monster(int dir, int dam)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_HEAL, dir, dam, MAX_RANGE, flg));
}

bool speed_monster(int dir)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_SPEED, dir, p_ptr->lev, MAX_RANGE, flg));
}

bool slow_monster(int dir)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_SLOW, dir, damroll (2, p_ptr->lev), MAX_RANGE, flg));
}

bool sleep_monster(int dir)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_SLEEP, dir, damroll(2, p_ptr->lev), MAX_RANGE, flg));
}

bool confuse_monster(int dir, int plev)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_CONF, dir, damroll (2, plev), MAX_RANGE, flg));
}

bool poly_monster(int dir)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_POLY, dir, p_ptr->lev, MAX_RANGE, flg));
}

bool clone_monster(int dir)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_OLD_CLONE, dir, 0, MAX_RANGE, flg));
}

bool fear_monster(int dir, int plev)
{
	u32b flg = PROJECT_STOP;
	return (fire_bolt_beam_special(GF_TURN_ALL, dir, plev, MAX_RANGE, flg));
}

bool teleport_monster(int dir)
{
	return (fire_beam(GF_AWAY_ALL, dir, MAX_SIGHT * 5, 0L));
}



/*
 * Hooks -- affect adjacent grids (radius 1 ball attack)
 */

bool door_creation(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;
	return (project(SOURCE_PLAYER, 1, py, px, py, px, 0, GF_MAKE_DOOR, flg, 0,0));
}

bool trap_creation(int who)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE | PROJECT_EFCT;
	return (project(who, 1, py, px, py, px, 0, GF_MAKE_TRAP, flg, 0, 0));
}

bool destroy_doors_touch(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE;

	return (project(SOURCE_PLAYER, 1, py, px, py, px, 0, GF_KILL_DOOR, flg, 0, 0));
}

bool sleep_monsters_touch(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	u32b flg = PROJECT_BOOM | PROJECT_KILL | PROJECT_HIDE;
	return (project(SOURCE_PLAYER, 1, py, px, py, px, damroll(2, p_ptr->lev), GF_OLD_SLEEP, flg, 0, 20));
}


/*
 * Curse the players armor
 */
bool curse_armor(void)
{
	object_type *o_ptr;

	char o_name[80];


	/* Curse the body armor */
	o_ptr = &inventory[INVEN_BODY];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 3);

	/* Attempt a saving throw for artifacts */
	if (artifact_p(o_ptr) && (rand_int(100) < 50))
	{
		/* Cool */
		msg_format("A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your armor", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format("A terrible black aura blasts your %s!", o_name);

		/* Blast the armor */
		o_ptr->art_num = 0;
		o_ptr->ego_num = EGO_BLASTED;
		o_ptr->to_a = 0 - randint(5) - randint(5);
		o_ptr->to_h = 0;
		o_ptr->to_d = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= (IDENT_CURSED);

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS | PU_NATIVE);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
	}

	return (TRUE);
}


/*
 * Curse the players weapon
 */
bool curse_weapon(void)
{
	object_type *o_ptr;

	char o_name[80];


	/* Curse the weapon */
	o_ptr = &inventory[INVEN_WIELD];

	/* Nothing to curse */
	if (!o_ptr->k_idx) return (FALSE);


	/* Describe */
	object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 3);

	/* Attempt a saving throw */
	if (artifact_p(o_ptr) && (rand_int(100) < 50))
	{
		/* Cool */
		msg_format("A %s tries to %s, but your %s resists the effects!",
		           "terrible black aura", "surround your weapon", o_name);
	}

	/* not artifact or failed save... */
	else
	{
		/* Oops */
		msg_format("A terrible black aura blasts your %s!", o_name);

		/* Shatter the weapon */
		o_ptr->art_num = 0;
		o_ptr->ego_num = EGO_SHATTERED;
		o_ptr->to_h = 0 - randint(5) - randint(5);
		o_ptr->to_d = 0 - randint(5) - randint(5);
		o_ptr->to_a = 0;
		o_ptr->ac = 0;
		o_ptr->dd = 0;
		o_ptr->ds = 0;

		/* Curse it */
		o_ptr->ident |= (IDENT_CURSED);

		/* Break it */
		o_ptr->ident |= (IDENT_BROKEN);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS | PU_NATIVE);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
	}

	/* Notice */
	return (TRUE);
}


/*
 * Brand weapons (or ammo)
 *
 * Turns the (non-magical) object into an ego-item of 'brand_type'.
 */
bool brand_object(object_type *o_ptr, byte brand_type, bool do_enchant)
{
	/* you can never modify artifacts / ego-items */
	/* you can never modify broken / cursed items */
	if ((o_ptr->k_idx) &&
	    (!artifact_p(o_ptr)) && (!ego_item_p(o_ptr)) &&
	    (!broken_p(o_ptr)) && (!cursed_p(o_ptr)))
	{
		cptr act = "magical";
		char o_name[80];

		object_desc(o_name, sizeof(o_name), o_ptr, FALSE, 0);

		/*Handle weapons differently than ammo*/
		if (wield_slot(o_ptr) == INVEN_WIELD)
		{
			/* Brand the object */
			o_ptr->ego_num = EGO_BRAND_ELEMENTS;

			o_ptr->xtra1 = OBJECT_XTRA_TYPE_BRAND;
			o_ptr->xtra2 = 1 << brand_type;
		}
		else
		{

			/* Brand the object */
			o_ptr->ego_num = brand_type;
		}

		switch (brand_type)
		{
			case BRAND_OFFSET_FLAME:
			case EGO_AMMO_FLAME:
			{
				act = "fiery";
				break;
			}
			case BRAND_OFFSET_FROST:
			case EGO_AMMO_FROST:
			{
				act = "frosty";
				break;
			}
			case BRAND_OFFSET_VENOM:
			case EGO_AMMO_VENOM:
			{
				act = "sickly";
				break;
			}
		}

		/* Describe */
		msg_format("A %s aura surrounds the %s.", act, o_name);

		/* Combine / Reorder the pack (later) */
		p_ptr->notice |= (PN_COMBINE | PN_REORDER);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);

		/* Enchant */
		if (do_enchant) enchant(o_ptr, rand_int(3) + 4, ENCH_TOHIT | ENCH_TODAM);

		/* Hack - Identify it */
		object_aware(o_ptr);
		object_known(o_ptr);

		return (TRUE);
	}
	else
	{
		if (flush_failure) flush();
		msg_print("The Branding failed.");
	}

	return (FALSE);
}


/*
 * Brand the current weapon
 */
bool brand_weapon(bool enchant)
{
	object_type *o_ptr;
	byte brand_type;

	o_ptr = &inventory[INVEN_WIELD];

	/* Select a brand */
	if (one_in_(3))
		brand_type = BRAND_OFFSET_FLAME;
	else
		brand_type = BRAND_OFFSET_FROST;

	/* Brand the weapon */
	return (brand_object(o_ptr, brand_type, enchant));
}

/*
 * Hook to specify "ammo"
 */
bool item_tester_hook_ided_ammo(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			if (object_known_p(o_ptr)) return (TRUE);
			else return FALSE;
		}
	}

	return (FALSE);
}

/*
 * Hook to specify "ammo"
 */
bool item_tester_hook_ammo(const object_type *o_ptr)
{
	switch (o_ptr->tval)
	{
		case TV_BOLT:
		case TV_ARROW:
		case TV_SHOT:
		{
			return (TRUE);
		}
	}

	return (FALSE);
}


/*
 * Brand some (non-magical) ammo
 */
bool brand_ammo(bool enchant)
{
	int item;
	object_type *o_ptr;
	cptr q, s;
	byte brand_type;

	/* Only accept ammo */
	item_tester_hook = item_tester_hook_ammo;

	/* Get an item */
	q = "Brand which kind of ammunition? ";
	s = "You have nothing to brand.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* Select the brand */
	if (one_in_(3))
		brand_type = EGO_AMMO_FLAME;
	else if (one_in_(2))
		brand_type = EGO_AMMO_FROST;
	else brand_type = EGO_AMMO_VENOM;

	/* Brand the ammo */
	return (brand_object(o_ptr, brand_type, enchant));

}


/*
 * Enchant some (non-magical) bolts
 */
bool brand_bolts(bool enchant)
{
	int item;
	object_type *o_ptr;
	cptr q, s;


	/* Restrict choices to bolts */
	item_tester_tval = TV_BOLT;

	/* Get an item */
	q = "Brand which bolts? ";
	s = "You have no bolts to brand.";
	if (!get_item(&item, q, s, (USE_EQUIP | USE_INVEN | USE_FLOOR))) return (FALSE);

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

	/* Brand the bolts */
	return (brand_object(o_ptr, EGO_AMMO_FLAME, enchant));

}


/*
 * Hack -- activate the ring of power
 */
void ring_of_power(int dir)
{
	/* Pick a random effect */
	switch (randint(10))
	{
		case 1:
		case 2:
		{
			/* Message */
			msg_print("You are surrounded by a malignant aura.");

			/* Decrease all stats (permanently) */
			(void)dec_stat(A_STR, 50, TRUE);
			(void)dec_stat(A_INT, 50, TRUE);
			(void)dec_stat(A_WIS, 50, TRUE);
			(void)dec_stat(A_DEX, 50, TRUE);
			(void)dec_stat(A_CON, 50, TRUE);
			(void)dec_stat(A_AGI, 50, TRUE);
			(void)dec_stat(A_STE, 50, TRUE);
			(void)dec_stat(A_PER, 50, TRUE);
			(void)dec_stat(A_LUC, 50, TRUE);

			/* Lose some experience (permanently) */
			p_ptr->exp -= (p_ptr->exp / 4);
			p_ptr->max_exp -= (p_ptr->max_exp / 4);
			check_experience();

			break;
		}

		case 3:
		{
			/* Message */
			msg_print("You are surrounded by a powerful aura.");

			/* Dispel monsters */
			dispel_monsters(1000);

			break;
		}

		case 4:
		case 5:
		case 6:
		{
			/* Mana Ball */
			fire_ball(GF_MANA, dir, 300, 3);

			break;
		}

		case 7:
		case 8:
		case 9:
		case 10:
		{
			/* Mana Bolt */
			fire_bolt(GF_MANA, dir, 250);

			break;
		}
	}
}

/*
 * Identifies all objects in the equipment and inventory.
 * Applies quality/ego-item squelch in the inventory.
 */
void identify_and_squelch_pack(void)
{
  	int item, squelch;
	object_type *o_ptr;

	/* Identify equipment */
	for (item = INVEN_WIELD; item < INVEN_TOTAL; item++)
	{
		/* Get the object */
		o_ptr = &inventory[item];

		/* Ignore empty objects */
		if (!o_ptr->k_idx) continue;

		/* Ignore known objects */
		if (object_known_p(o_ptr)) continue;

		/* Identify it */
		(void)do_ident_item(item, o_ptr, 0);
	}

	/* Identify inventory */
	for (item = 0; item < INVEN_WIELD; item++)
	{
	  	while (TRUE)
		{
	  		/* Get the object */
			o_ptr = &inventory[item];

			/* Ignore empty objects */
			if (!o_ptr->k_idx) break;

			/* Ignore known objects */
			if (object_known_p(o_ptr)) break;

			/* Identify it and get the squelch setting */
			squelch = do_ident_item(item, o_ptr, 0);

			/*
			 * If the object was squelched, keep analyzing
			 * the same slot (the inventory was displaced). -DG-
			 */
			if (squelch != SQUELCH_YES) break;

			/* Now squelch the object */
			do_squelch_item(squelch, item, o_ptr);
		}
	}
}

/* Mass-identify handler */
bool mass_identify (int rad)
{
	/*record the old target*/
	s16b old_target_set = p_ptr->target_set;
	s16b old_target_who = p_ptr->target_who;
	s16b old_target_row = p_ptr->target_row;
	s16b old_target_col	= p_ptr->target_col;

	/* Direct the ball to the player */
  	target_set_location(p_ptr->py, p_ptr->px);

	/* Cast the ball spell */
	fire_ball(GF_MASS_IDENTIFY, 5, 0, rad);

  	/* Identify equipment and inventory, apply quality squelch */
  	identify_and_squelch_pack();

	/*re-set to old target*/
	p_ptr->target_set = old_target_set;
	p_ptr->target_who = old_target_who;
	p_ptr->target_row = old_target_row;
	p_ptr->target_col = old_target_col;

	/* This spell always works */
	return (TRUE);
}


/*
 * Execute some common code of the identify spells.
 * "item" is used to print the slot occupied by an object in equip/inven.
 * ANY negative value assigned to "item" can be used for specifying an object
 * on the floor (they don't have a slot, example: the code used to handle
 * GF_MASS_IDENTIFY in project_o).
 * It returns the value returned by squelch_itemp.
 * The object is NOT squelched here.
 */
int do_ident_item(int item, object_type *o_ptr, int quiet)
{
	char o_name[80];
	u16b msgt = MSG_GENERIC;
	int squelch = SQUELCH_NO;

	/* Identify it */
	object_aware(o_ptr);
	object_known(o_ptr);

	/* Apply an autoinscription, if necessary */
	apply_autoinscription(o_ptr);

	/* Squelch it? */
	if (item < INVEN_WIELD && !quiet) squelch = squelch_itemp(o_ptr, 0, TRUE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS | PU_NATIVE);

	p_ptr->redraw |= (PR_EXP | PR_STATS);

	/* Combine / Reorder the pack (later) */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

	/* Description */
	object_desc(o_name, sizeof(o_name), o_ptr, TRUE, 3);

	/* Possibly play a sound depending on object quality. */
	if (o_ptr->pval < 0)
	{
		/* This is a bad item. */
		sound(MSG_IDENT_BAD);
	}
	else if (o_ptr->art_num != 0)
	{
		/* We have a good artifact. */
		sound(MSG_IDENT_ART);
	}
	else if (o_ptr->ego_num != 0)
	{
		/* We have a good ego item. */
		sound(MSG_IDENT_EGO);
	}

	/* Display independent of cursedness */
	if(o_ptr->art_num)
		msgt = MSG_IDENT_ART;
	else if (o_ptr->ego_num)
		msgt = MSG_IDENT_EGO;

	/* Describe */
	if (item >= INVEN_WIELD && !quiet)
	{
		msg_c_format(msgt, "%^s: %s (%c).",
			describe_use(item), o_name, index_to_label(item));
	}
	else if (item >= 0 && !quiet)
	{
		msg_c_format(msgt, "In your pack: %s (%c).  %s",
			o_name, index_to_label(item),
			squelch_to_label(squelch));
 	}
	else if (!quiet)
	{
		msg_c_format(msgt, "On the ground: %s.  %s", o_name,
			squelch_to_label(squelch));
	}

	/*
	 * If the item was an artifact, and if the auto-note is selected,
	 * write a message.
	 */
    	if ((adult_take_notes) && artifact_p(o_ptr) && (o_ptr->xtra1 >= 1))
	{
		int artifact_depth;
        	char note[120];
		char shorter_desc[120];

		/* Get a shorter description to fit the notes file */
		object_desc(shorter_desc, sizeof(shorter_desc), o_ptr, TRUE, 0);

		/* Build note and write */
        	sprintf(note, "Found %s", shorter_desc);

		/* Record the depth where the artifact was created */
		artifact_depth = o_ptr->xtra1;

        	do_cmd_note(note, artifact_depth);

		/*
		 * Mark item creation depth 0, which will indicate the artifact
		 * has been previously identified.  This prevents an artifact
		 * from showing up on the notes list twice ifthe artifact had
		 * been previously identified.  JG
		 */
		o_ptr->xtra1 = 0 ;
	}

	return (squelch);
}

/*
 * Return TRUE if the player is immune to the effects of the given
 * spell type
 */
bool is_player_immune(int gf_type)
{
	/* Check immunities */
	switch (gf_type)
	{
		case GF_ACID:
		{
			if (p_ptr->immune_acid) return (TRUE);

			break;
		}
		case GF_ICE:
		case GF_COLD:
		{
			if (p_ptr->immune_cold) return (TRUE);

			break;
		}
		case GF_FIRE:
		{
			if (p_ptr->immune_fire) return (TRUE);

			break;
		}
		case GF_ELEC:
		{
			if (p_ptr->immune_elec) return (TRUE);

			break;
		}
		case GF_POIS:
		{
			if (p_ptr->immune_pois) return (TRUE);

			break;
		}
	}

	return (FALSE);
}

