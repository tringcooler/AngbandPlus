/* File: monster2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



/*
 * Delete a monster by index.
 *
 * When a monster is deleted, all of its objects are deleted.
 */
void delete_monster_idx(int i)
{
	int x, y;

	monster_type *m_ptr = &mon_list[i];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	s16b this_o_idx, next_o_idx = 0;


	/* Get location */
	y = m_ptr->fy;
	x = m_ptr->fx;


	/* Hack -- Reduce the racial counter */
	r_ptr->cur_num--;

	/* Hack -- count the number of "reproducers" */
	if (r_ptr->flags2 & (RF2_MULTIPLY)) num_repro--;


	/* Hack -- remove target monster */
	if (p_ptr->target_who == i) target_set_monster(0);

	/* Hack -- remove tracked monster */
	if (p_ptr->health_who == i) health_track(0);

	/* monster is no longer holding the PC */
	if (p_ptr->held_m_idx == i)
	{
		p_ptr->held_m_idx = 0;
		clear_timed(TMD_BEAR_HOLD);
	}

	/* Monster is gone */
	cave_m_idx[y][x] = 0;


	/* Delete objects */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Hack -- efficiency */
		o_ptr->held_m_idx = 0;

		/* Delete the object */
		delete_object_idx(this_o_idx);
	}


	/* Wipe the Monster */
	(void)WIPE(m_ptr, monster_type);

	/* Count monsters */
	mon_cnt--;


	/* Visual update */
	lite_spot(y, x);
}


/*
 * Delete the monster, if any, at a given location
 */
void delete_monster(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Delete the monster (if any) */
	if (cave_m_idx[y][x] > 0) delete_monster_idx(cave_m_idx[y][x]);
}


/*
 * Move an object from index i1 to index i2 in the object list
 */
static void compact_monsters_aux(int i1, int i2)
{
	int y, x;

	monster_type *m_ptr;

	s16b this_o_idx, next_o_idx = 0;


	/* Do nothing */
	if (i1 == i2) return;


	/* Old monster */
	m_ptr = &mon_list[i1];

	/* Location */
	y = m_ptr->fy;
	x = m_ptr->fx;

	/* Update the cave */
	cave_m_idx[y][x] = i2;

	/* Repair objects being carried by monster */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Reset monster pointer */
		o_ptr->held_m_idx = i2;
	}

	/* Hack -- Update the target */
	if (p_ptr->target_who == i1) p_ptr->target_who = i2;

	/* Hack -- Update the health bar */
	if (p_ptr->health_who == i1) p_ptr->health_who = i2;

	/* Hack -- move monster */
	COPY(&mon_list[i2], &mon_list[i1], monster_type);

	/* Hack -- wipe hole */
	(void)WIPE(&mon_list[i1], monster_type);
}


/*
 * Compact and Reorder the monster list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" monsters, we base the saving throw
 * on a combination of monster level, distance from player, and
 * current "desperation".
 *
 * After "compacting" (if needed), we "reorder" the monsters into a more
 * compact order, and we reset the allocation info, and the "live" array.
 */
void compact_monsters(int size)
{
	int i, num, cnt;

	int cur_lev, cur_dis, chance;


	/* Message (only if compacting) */
	if (size) msg_print("Compacting monsters...");


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Check all the monsters */
		for (i = 1; i < mon_max; i++)
		{
			monster_type *m_ptr = &mon_list[i];

			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Paranoia -- skip "dead" monsters */
			if (!m_ptr->r_idx) continue;

			/* Hack -- High level monsters start out "immune" */
			if (r_ptr->level > cur_lev) continue;

			/* Ignore nearby monsters */
			if ((cur_dis > 0) && (m_ptr->cdis < cur_dis)) continue;

			/* Saving throw chance */
			chance = 90;

			/* Only compact "Quest" Monsters in emergencies */
			if ((r_ptr->flags1 & (RF1_QUESTOR)) && (cnt < 1000)) chance = 100;

			/* Try not to compact Unique Monsters */
			if (r_ptr->flags1 & (RF1_UNIQUE)) chance = 99;

			/* All monsters get a saving throw */
			if (rand_int(100) < chance) continue;

			/* Delete the monster */
			delete_monster_idx(i);

			/* Count the monster */
			num++;
		}
	}


	/* Excise dead monsters (backwards!) */
	for (i = mon_max - 1; i >= 1; i--)
	{
		/* Get the i'th monster */
		monster_type *m_ptr = &mon_list[i];

		/* Skip real monsters */
		if (m_ptr->r_idx) continue;

		/* Move last monster into open hole */
		compact_monsters_aux(mon_max - 1, i);

		/* Compress "mon_max" */
		mon_max--;
	}
}


/*
 * Delete/Remove all the monsters when the player leaves the level
 *
 * This is an efficient method of simulating multiple calls to the
 * "delete_monster()" function, with no visual effects.
 */
void wipe_mon_list(void)
{
	int i;

	/* Delete all the monsters */
	for (i = mon_max - 1; i >= 1; i--)
	{
		monster_type *m_ptr = &mon_list[i];

		monster_race *r_ptr = &r_info[m_ptr->r_idx];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Mega-Hack -- preserve Uniques XXX XXX XXX */

		/* Hack -- Reduce the racial counter */
		r_ptr->cur_num--;

		/* Monster is gone */
		cave_m_idx[m_ptr->fy][m_ptr->fx] = 0;

		/* Wipe the Monster */
		(void)WIPE(m_ptr, monster_type);
	}

	/* Reset "mon_max" */
	mon_max = 1;

	/* Reset "mon_cnt" */
	mon_cnt = 0;

	/* Hack -- reset "reproducer" count */
	num_repro = 0;

	/* Hack -- no more target */
	target_set_monster(0);

	/* Hack -- no more tracking */
	health_track(0);

	/* no monster is holding the player */
	p_ptr->held_m_idx = 0;
	clear_timed(TMD_BEAR_HOLD);
}


/*
 * Get and return the index of a "free" monster.
 *
 * This routine should almost never fail, but it *can* happen.
 */
s16b mon_pop(void)
{
	int i;


	/* Normal allocation */
	if (mon_max < z_info->m_max)
	{
		/* Get the next hole */
		i = mon_max;

		/* Expand the array */
		mon_max++;

		/* Count monsters */
		mon_cnt++;

		/* Return the index */
		return (i);
	}


	/* Recycle dead monsters */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr;

		/* Get the monster */
		m_ptr = &mon_list[i];

		/* Skip live monsters */
		if (m_ptr->r_idx) continue;

		/* Count monsters */
		mon_cnt++;

		/* Use this monster */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
	if (character_dungeon) msg_print("Too many monsters!");

	/* Try not to crash */
	return (0);
}


/*
 * Apply a "monster restriction function" to the "monster allocation table"
 */
errr get_mon_num_prep(void)
{
	int i;

	/* Scan the allocation table */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Get the entry */
		alloc_entry *entry = &alloc_race_table[i];

		/* Accept monsters which pass the restriction, if any */
		if (!get_mon_num_hook || (*get_mon_num_hook)(entry->index))
		{
			/* Accept this monster */
			entry->prob2 = entry->prob1;
		}

		/* Do not use this monster */
		else
		{
			/* Decline this monster */
			entry->prob2 = 0;
		}
	}

	/* Success */
	return (0);
}



/*
 * Choose a monster race that seems "appropriate" to the given level
 *
 * This function uses the "prob2" field of the "monster allocation table",
 * and various local information, to calculate the "prob3" field of the
 * same table, which is then used to choose an "appropriate" monster, in
 * a relatively efficient manner.
 *
 * Note that "town" monsters will *only* be created in the town, and
 * "normal" monsters will *never* be created in the town, unless the
 * "level" is "modified", for example, by polymorph or summoning.
 *
 * There is a small chance (1/50) of "boosting" the given depth by
 * a small amount (up to four levels), except in the town.
 *
 * It is (slightly) more likely to acquire a monster of the given level
 * than one of a lower level.  This is done by choosing several monsters
 * appropriate to the given level and keeping the "hardest" one.
 *
 * Note that if no monsters are "appropriate", then this function will
 * fail, and return zero, but this should *almost* never happen.
 */
s16b get_mon_num(int level)
{
	int i, j, p;

	int r_idx;

	long value, total;

	monster_race *r_ptr;

	alloc_entry *table = alloc_race_table;


	/* Boost the level */
	if (level > 0)
	{
		/* Occasional "nasty" monster */
		if (rand_int(NASTY_MON) == 0)
		{
			/* Pick a level bonus */
			int d = level / 4 + 2;

			/* Boost the level */
			level += ((d < 5) ? d : 5);
		}

		/* Occasional "nasty" monster */
		if (rand_int(NASTY_MON) == 0)
		{
			/* Pick a level bonus */
			int d = level / 4 + 2;

			/* Boost the level */
			level += ((d < 5) ? d : 5);
		}
	}


	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Monsters are sorted by depth */
		if (table[i].level > level) break;

		/* Default */
		table[i].prob3 = 0;

		/* Hack -- No town monsters in dungeon */
		if ((level > 0) && (table[i].level <= 0)) continue;

		/* Get the "r_idx" of the chosen monster */
		r_idx = table[i].index;

		/* Get the actual race */
		r_ptr = &r_info[r_idx];

		/* Hack -- "unique" monsters must be "unique" */
		if ((r_ptr->flags1 & (RF1_UNIQUE)) &&
		    (r_ptr->cur_num >= r_ptr->max_num))
		{
			continue;
		}

		/* Depth Monsters never appear out of depth */
		if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) && (r_ptr->level > p_ptr->depth))
		{
			continue;
		}

		/* Accept */
		table[i].prob3 = table[i].prob2;

		/* Total */
		total += table[i].prob3;
	}

	/* No legal monsters */
	if (total <= 0) return (0);


	/* Pick a monster */
	value = rand_int(total);

	/* Find the monster */
	for (i = 0; i < alloc_race_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}


	/* Power boost */
	p = rand_int(100);

	/* Try for a "harder" monster once (50%) or twice (10%) */
	if (p < 60)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = rand_int(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}

	/* Try for a "harder" monster twice (10%) */
	if (p < 10)
	{
		/* Save old */
		j = i;

		/* Pick a monster */
		value = rand_int(total);

		/* Find the monster */
		for (i = 0; i < alloc_race_size; i++)
		{
			/* Found the entry */
			if (value < table[i].prob3) break;

			/* Decrement */
			value = value - table[i].prob3;
		}

		/* Keep the "best" one */
		if (table[i].level < table[j].level) i = j;
	}


	/* Result */
	return (table[i].index);
}

/*
 * Display visible monsters in a window (the monster list)
 */
void display_monlist(void)
{
	int i, max;
	int line = 1, x = 0;
	int cur_x;
	unsigned total_count = 0, disp_count = 0;
	unsigned count_asleep = 0, count_normal = 0;
	unsigned count_snolos = 0, count_nolos = 0;
	bool uniq, messy;

	byte attr;

	char *m_name;
	char buf[80];
	monster_type *m_ptr;
	monster_race *r_ptr;

	u16b *race_count;
	u16b *race_count_nolos;
	u16b *race_count_snolos;
	u16b *race_count_asleep;


	/* Clear the term if in a subwindow, set x otherwise */
	if (Term != angband_term[0])
	{
		clear_from(0);
		max = Term->hgt - 1;
	}
	else
	{
		x = 13;
		max = Term->hgt - 2;
	}

	/* Allocate the array */
	C_MAKE(race_count, z_info->r_max, u16b);
	C_MAKE(race_count_nolos, z_info->r_max, u16b);
	C_MAKE(race_count_snolos, z_info->r_max, u16b);
	C_MAKE(race_count_asleep, z_info->r_max, u16b);

	uniq = FALSE;
	messy = FALSE;
	if (p_ptr->timed[TMD_IMAGE]) messy = TRUE;

	/* Scan the monster list */
	for (i = 1; i < mon_max; i++)
	{
		m_ptr = &mon_list[i];
		r_ptr = &r_info[m_ptr->r_idx];

		/* Only visible monsters */
		if (!m_ptr->ml) continue;
		
        /* display flags should be correct even when hallucenating */
        if (m_ptr->csleep) m_ptr->display_sleep = TRUE;
        else m_ptr->display_sleep = FALSE;
	    
        /* halucenation messes things up */
        if ((messy) && (randint(40) < 30 + (badluck/2)))
        {
           int mess = randint(4);
           int friedx = randint(682) + 17;
           if (mess == 1)
           {
              race_count_asleep[friedx] += 1;
              count_asleep += 1;
           }
           else if (mess == 2)
           {
              race_count[friedx] += 1;
              count_normal += 1;
           }
           else if (mess == 3)
           {
              race_count_snolos[friedx] += 1;
              count_snolos += 1;
           }
           else if (mess == 4)
           {
              race_count_nolos[friedx] += 1;
              count_nolos += 1;
           }
		   /* sometimes see more monsters than there really are */
		   if (randint(100) < 9) i -= 1;
        }
		/* Bump the count for this race, and the total count */
		/* separate race count for los / no los / asleep */
        else if ((!(m_ptr->mflag & (MFLAG_VIEW))) && (m_ptr->csleep))
	    {
            race_count_snolos[m_ptr->r_idx]++;
            count_snolos++;
        }
        else if (!(m_ptr->mflag & (MFLAG_VIEW)))
	    {
            race_count_nolos[m_ptr->r_idx]++;
            count_nolos++;
        }
        else if (m_ptr->csleep)
        {
            race_count_asleep[m_ptr->r_idx]++;
            count_asleep++;
        }
        else
        {
            race_count[m_ptr->r_idx]++;
            count_normal++;
        }
        if ((r_ptr->flags1 & RF1_UNIQUE) && (!messy)) uniq = TRUE;
	
		total_count++;
	}

	/* Note no visible monsters */
	if (!total_count)
	{
		/* Clear display and print note */
		c_prt(TERM_SLATE, "You see no monsters.", 0, 0);
		if (Term == angband_term[0])
		    Term_addstr(-1, TERM_WHITE, "  (Press any key to continue.)");

		/* Free up memory */
		FREE(race_count);
		FREE(race_count_asleep);
		FREE(race_count_nolos);
		FREE(race_count_snolos);

		/* Done */
		return;
	}

    /* if (count_asleep) */
    if (((count_asleep) || (count_snolos)) && (!messy))
    {
		/* Reset position */
		cur_x = x;

        /* my_strcpy(buf, "  The following monsters are in sight but aren't aware of you:", sizeof(buf)); */
        my_strcpy(buf, " (darker shade means unaware of you, blue means out of line of sight) ", sizeof(buf));

        /* Print and bump line counter */
		c_prt(TERM_L_WHITE, buf, line, cur_x);
		line++;
    }

    if ((uniq) && (!messy))
    {
		/* Reset position */
		cur_x = x;

        my_strcpy(buf, " (uniques in red if in sight, or brown if out of LOS) ", sizeof(buf));

        /* Print and bump line counter */
		c_prt(TERM_L_WHITE, buf, line, cur_x);
		line++;
    }

	/* Go over (line of sight and awake) */
	for (i = 1; (i < z_info->r_max) && (line < max); i++)
	{
		/* Reset position */
		cur_x = x;

        /* no monsters in los who are aware of you */
        if (!count_normal) break;
        
		/* No monsters of this race are visible (and awake) */
		if (!race_count[i]) continue;

		/* Note that these have been displayed */
		disp_count += race_count[i];

		/* Get monster race and name */
		r_ptr = &r_info[i];
		m_name = r_name + r_ptr->name;

		/* Display uniques in a special colour */
		if (r_ptr->flags1 & RF1_UNIQUE)
			attr = TERM_L_RED;
		else
			attr = TERM_WHITE;

		/* Build the monster name */
		if (race_count[i] == 1)
			my_strcpy(buf, m_name, sizeof(buf));
		else
			strnfmt(buf, sizeof(buf), "%s (x%d) ", m_name, race_count[i]);

		/* Display the pict */
		Term_putch(cur_x++, line, r_ptr->x_attr, r_ptr->x_char);
		if (use_bigtile) Term_putch(cur_x++, line, 255, -1);
		Term_putch(cur_x++, line, TERM_WHITE, ' ');

		/* Print and bump line counter */
		c_prt(attr, buf, line, cur_x);
		line++;
	}
        
	/* Go over (line of sight and unaware of the player) */
	for (i = 1; (i < z_info->r_max) && (line < max); i++)
	{
		/* Reset position */
		cur_x = x;

        /* no sleeping (or roaming) monsters */
        if (!count_asleep) break;
        
        /* No monsters of this race are visible and unaware */
		if (!race_count_asleep[i]) continue;

		/* Note that these have been displayed */
		disp_count += race_count_asleep[i];

		/* Get monster race and name */
		r_ptr = &r_info[i];
		m_name = r_name + r_ptr->name;

		/* Display uniques in a special colour */
		if (r_ptr->flags1 & RF1_UNIQUE)
			attr = TERM_RED;
		else
			attr = TERM_L_WHITE;

		/* Build the monster name */
		if (race_count_asleep[i] == 1)
			my_strcpy(buf, m_name, sizeof(buf));
		else
			strnfmt(buf, sizeof(buf), "%s (x%d) ", m_name, race_count_asleep[i]);

		/* Display the pict */
		Term_putch(cur_x++, line, r_ptr->x_attr, r_ptr->x_char);
		if (use_bigtile) Term_putch(cur_x++, line, 255, -1);
		Term_putch(cur_x++, line, TERM_WHITE, ' ');

		/* Print and bump line counter */
		c_prt(attr, buf, line, cur_x);
		line++;
	}

/*#if removed        
    if ((count_nolos) || (count_snolos))
    {
        my_strcpy(buf, "  The following monsters are not in line of sight:", sizeof(buf));

        /* Print and bump line counter *
		c_prt(TERM_L_BLUE, buf, line, cur_x);
		line++;
    }
#endif */

	/* Go over (for not in LOS monsters) */
	for (i = 1; (i < z_info->r_max) && (line < max); i++)
	{
		/* Reset position */
		cur_x = x;

        /* no monsters out of LOS and aware */
        if (!count_nolos) break;
        
		/* No monsters of this race are visible */
		if (!race_count_nolos[i]) continue;

		/* Note that these have been displayed */
		disp_count += race_count_nolos[i];

		/* Get monster race and name */
		r_ptr = &r_info[i];
		m_name = r_name + r_ptr->name;

		/* Display uniques in a special colour */
		if (r_ptr->flags1 & RF1_UNIQUE)
			attr = TERM_L_UMBER;
		else
			attr = TERM_L_BLUE;

		/* Build the monster name */
		if (race_count_nolos[i] == 1)
			my_strcpy(buf, m_name, sizeof(buf));
		else
			strnfmt(buf, sizeof(buf), "%s (x%d) ", m_name, race_count_nolos[i]);

		/* Display the pict */
		Term_putch(cur_x++, line, r_ptr->x_attr, r_ptr->x_char);
		if (use_bigtile) Term_putch(cur_x++, line, 255, -1);
		Term_putch(cur_x++, line, TERM_WHITE, ' ');

		/* Print and bump line counter */
		c_prt(attr, buf, line, cur_x);
		line++;
	}

	/* Go over (for out of LOS and not aware) */
	for (i = 1; (i < z_info->r_max) && (line < max); i++)
	{
		/* Reset position */
		cur_x = x;

        /* no monsters out of LOS and unaware */
        if (!count_snolos) break;

        /* no message */
        
		/* No monsters of this race are visible */
		if (!race_count_snolos[i]) continue;

		/* Note that these have been displayed */
		disp_count += race_count_snolos[i];

		/* Get monster race and name */
		r_ptr = &r_info[i];
		m_name = r_name + r_ptr->name;

		/* Display uniques in a special colour */
		if (r_ptr->flags1 & RF1_UNIQUE)
			attr = TERM_UMBER;
		else
			attr = TERM_BLUE;
		

		/* Build the monster name */
		if (race_count_snolos[i] == 1)
			my_strcpy(buf, m_name, sizeof(buf));
		else
			strnfmt(buf, sizeof(buf), "%s (x%d) ", m_name, race_count_snolos[i]);

		/* Display the pict */
		Term_putch(cur_x++, line, r_ptr->x_attr, r_ptr->x_char);
		if (use_bigtile) Term_putch(cur_x++, line, 255, -1);
		Term_putch(cur_x++, line, TERM_WHITE, ' ');

		/* Print and bump line counter */
		c_prt(attr, buf, line, cur_x);
		line++;
	}

	/* Print "and others" message if we've run out of space */
	if (disp_count != total_count)
	{
		strnfmt(buf, sizeof buf, "  ...and %d others.", total_count - disp_count);
		c_prt(TERM_WHITE, buf, line, x);
	}

	/* Otherwise clear a line at the end, for main-term display */
	else
	{
		prt("", line, x);
	}

	/* Message */
	prt(format("You can see %d monster%s:",
	           total_count, (total_count > 1 ? "s" : "")), 0, 0);

	if (Term == angband_term[0])
		Term_addstr(-1, TERM_WHITE, "  (Press any key to continue.)");

	/* Free the race counters */
	FREE(race_count);
	FREE(race_count_asleep);
	FREE(race_count_nolos);
	FREE(race_count_snolos);
}

/*
 * Build a string describing a monster in some way.
 *
 * We can correctly describe monsters based on their visibility.
 * We can force all monsters to be treated as visible or invisible.
 * We can build nominatives, objectives, possessives, or reflexives.
 * We can selectively pronominalize hidden, visible, or all monsters.
 * We can use definite or indefinite descriptions for hidden monsters.
 * We can use definite or indefinite descriptions for visible monsters.
 *
 * Pronominalization involves the gender whenever possible and allowed,
 * so that by cleverly requesting pronominalization / visibility, you
 * can get messages like "You hit someone.  She screams in agony!".
 *
 * Reflexives are acquired by requesting Objective plus Possessive.
 *
 * I am assuming that no monster name is more than 65 characters long,
 * so that "char desc[80];" is sufficiently large for any result, even
 * when the "offscreen" notation is added.
 *
 * Note that the "possessive" for certain unique monsters will look
 * really silly, as in "Morgoth, King of Darkness's".  We should
 * perhaps add a flag to "remove" any "descriptives" in the name.
 *
 * Note that "offscreen" monsters will get a special "(offscreen)"
 * notation in their name if they are visible but offscreen.  This
 * may look silly with possessives, as in "the rat's (offscreen)".
 * Perhaps the "offscreen" descriptor should be abbreviated.
 *
 * Mode Flags:
 *   0x01 --> Objective (or Reflexive)
 *   0x02 --> Possessive (or Reflexive)
 *   0x04 --> Use indefinites for hidden monsters ("something")
 *   0x08 --> Use indefinites for visible monsters ("a kobold")
 *   0x10 --> Pronominalize hidden monsters
 *   0x20 --> Pronominalize visible monsters
 *   0x40 --> Assume the monster is hidden
 *   0x80 --> Assume the monster is visible
 *
 * Useful Modes:
 *   0x00 --> Full nominative name ("the kobold") or "it"
 *   0x04 --> Full nominative name ("the kobold") or "something"
 *   0x80 --> Banishment resistance name ("the kobold")
 *   0x88 --> Killing name ("a kobold")
 *   0x22 --> Possessive, genderized if visable ("his") or "its"
 *   0x23 --> Reflexive, genderized if visable ("himself") or "itself"
 */
void monster_desc(char *desc, size_t max, const monster_type *m_ptr, int mode)
{
	cptr res;
	cptr name;
	bool seen, pron;

	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	
	/* Hallucenation */
    if ((p_ptr->timed[TMD_IMAGE]) && (randint(40) < 35 + ((badluck+3)/4)))
	{
       r_ptr = &r_info[randint(682) + 17];
    }

	name = (r_name + r_ptr->name);


	/* Can we "see" it (forced, or not hidden + visible) */
	seen = ((mode & (0x80)) || (!(mode & (0x40)) && m_ptr->ml));

	/* Sexed Pronouns (seen and forced, or unseen and allowed) */
	pron = ((seen && (mode & (0x20))) || (!seen && (mode & (0x10))));


	/* First, try using pronouns, or describing hidden monsters */
	if (!seen || pron)
	{
		/* an encoding of the monster "sex" */
		int kind = 0x00;

		/* Extract the gender (if applicable) */
		if (r_ptr->flags1 & (RF1_FEMALE)) kind = 0x20;
		else if (r_ptr->flags1 & (RF1_MALE)) kind = 0x10;

		/* Ignore the gender (if desired) */
		if (!m_ptr || !pron) kind = 0x00;


		/* Assume simple result */
		res = "it";

		/* Brute force: split on the possibilities */
		switch (kind + (mode & 0x07))
		{
			/* Neuter, or unknown */
			case 0x00: res = "it"; break;
			case 0x01: res = "it"; break;
			case 0x02: res = "its"; break;
			case 0x03: res = "itself"; break;
			case 0x04: res = "something"; break;
			case 0x05: res = "something"; break;
			case 0x06: res = "something's"; break;
			case 0x07: res = "itself"; break;

			/* Male (assume human if vague) */
			case 0x10: res = "he"; break;
			case 0x11: res = "him"; break;
			case 0x12: res = "his"; break;
			case 0x13: res = "himself"; break;
			case 0x14: res = "someone"; break;
			case 0x15: res = "someone"; break;
			case 0x16: res = "someone's"; break;
			case 0x17: res = "himself"; break;

			/* Female (assume human if vague) */
			case 0x20: res = "she"; break;
			case 0x21: res = "her"; break;
			case 0x22: res = "her"; break;
			case 0x23: res = "herself"; break;
			case 0x24: res = "someone"; break;
			case 0x25: res = "someone"; break;
			case 0x26: res = "someone's"; break;
			case 0x27: res = "herself"; break;
		}

		/* Copy the result */
		my_strcpy(desc, res, max);
	}


	/* Handle visible monsters, "reflexive" request */
	else if ((mode & 0x02) && (mode & 0x01))
	{
		/* The monster is visible, so use its gender */
		if (r_ptr->flags1 & (RF1_FEMALE)) my_strcpy(desc, "herself", max);
		else if (r_ptr->flags1 & (RF1_MALE)) my_strcpy(desc, "himself", max);
		else my_strcpy(desc, "itself", max);
	}


	/* Handle all other visible monster requests */
	else
	{
		/* It could be a Unique */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			/* Start with the name (thus nominative and objective) */
			my_strcpy(desc, name, max);
		}

		/* It could be an indefinite monster */
		else if (mode & 0x08)
		{
			/* XXX Check plurality for "some" */

			/* Indefinite monsters need an indefinite article */
			my_strcpy(desc, is_a_vowel(name[0]) ? "an " : "a ", max);
			my_strcat(desc, name, max);
		}

		/* It could be a normal, definite, monster */
		else
		{
			/* Definite monsters need a definite article */
			my_strcpy(desc, "the ", max);
			my_strcat(desc, name, max);
		}

		/* Handle the Possessive as a special afterthought */
		if (mode & 0x02)
		{
			/* XXX Check for trailing "s" */

			/* Simply append "apostrophe" and "s" */
			my_strcat(desc, "'s", max);
		}

		/* Mention "offscreen" monsters XXX XXX */
		if (!panel_contains(m_ptr->fy, m_ptr->fx))
		{
			/* Append special notation */
			my_strcat(desc, " (offscreen)", max);
		}
	}
}



/*
 * Learn about a monster (by "probing" it)
 */
void lore_do_probe(int m_idx)
{
	monster_type *m_ptr = &mon_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];


	/* Hack -- Memorize some flags, DJA: changed to include spells */
	l_ptr->flags1 = r_ptr->flags1;
	l_ptr->flags2 = r_ptr->flags2;
	l_ptr->flags3 = r_ptr->flags3;
	if (randint(100) < 34)
	{
	   l_ptr->flags4 = r_ptr->flags4;
	   l_ptr->flags5 = r_ptr->flags5;
	   l_ptr->flags6 = r_ptr->flags6;
    }

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}
}


/*
 * Take note that the given monster just dropped some treasure
 *
 * Note that learning the "GOOD"/"GREAT" flags gives information
 * about the treasure (even when the monster is killed for the first
 * time, such as uniques, and the treasure has not been examined yet).
 *
 * This "indirect" method is used to prevent the player from learning
 * exactly how much treasure a monster can drop from observing only
 * a single example of a drop.  This method actually observes how much
 * gold and items are dropped, and remembers that information to be
 * described later by the monster recall code.
 */
void lore_treasure(int m_idx, int num_item, int num_gold)
{
	monster_type *m_ptr = &mon_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];


	/* Note the number of things dropped */
	if (num_item > l_ptr->drop_item) l_ptr->drop_item = num_item;
	if (num_gold > l_ptr->drop_gold) l_ptr->drop_gold = num_gold;

	/* Hack -- memorize the good/great flags */
	if (r_ptr->flags1 & (RF1_DROP_GOOD)) l_ptr->flags1 |= (RF1_DROP_GOOD);
	if (r_ptr->flags1 & (RF1_DROP_GREAT)) l_ptr->flags1 |= (RF1_DROP_GREAT);

	/* Update monster recall window */
	if (p_ptr->monster_race_idx == m_ptr->r_idx)
	{
		/* Window stuff */
		p_ptr->window |= (PW_MONSTER);
	}
}



/*
 * This function updates the monster record of the given monster
 *
 * This involves extracting the distance to the player (if requested),
 * and then checking for visibility (natural, stealth, see-invis,
 * telepathy), updating the monster visibility flag, redrawing (or
 * erasing) the monster when its visibility changes, and taking note
 * of any interesting monster flags (cold-blooded, invisible, etc).
 *
 * Note the new "mflag" field which encodes several monster state flags,
 * including "view" for when the monster is currently in line of sight,
 * and "mark" for when the monster is currently visible via detection.
 *
 * The only monster fields that are changed here are "cdis" (the
 * distance from the player), "ml" (visible to the player), and
 * "mflag" (to maintain the "MFLAG_VIEW" flag).
 *
 * Note the special "update_monsters()" function which can be used to
 * call this function once for every monster.
 *
 * Note the "full" flag which requests that the "cdis" field be updated,
 * this is only needed when the monster (or the player) has moved.
 *
 * Every time a monster moves, we must call this function for that
 * monster, and update the distance, and the visibility.  Every time
 * the player moves, we must call this function for every monster, and
 * update the distance, and the visibility.  Whenever the player "state"
 * changes in certain ways ("blindness", "telepathy",
 * and "see invisible"), we must call this function for every monster,
 * and update the visibility.
 *
 * Routines that change the "illumination" of a grid must also call this
 * function for any monster in that grid, since the "visibility" of some
 * monsters may be based on the illumination of their grid.
 *
 * Note that this function is called once per monster every time the
 * player moves.  When the player is running, this function is one
 * of the primary bottlenecks, along with "update_view()" and the
 * "process_monsters()" code, so efficiency is important.
 *
 * Note the optimized "inline" version of the "distance()" function.
 *
 * A monster is "visible" to the player if (1) it has been detected
 * by the player, (2) it is close to the player and the player has
 * telepathy, or (3) it is close to the player, and in line of sight
 * of the player, and it is "illuminated" by some combination of
 * torch light, or permanent light (invisible monsters
 * are only affected by "light" if the player can see invisible).
 *
 * Stealthy monsters who are visible and in line of sight can still 
 * escape notice.
 *
 * Monsters which are not on the current panel may be "visible" to
 * the player, and their descriptions will include an "offscreen"
 * reference.  Currently, offscreen monsters cannot be targetted
 * or viewed directly, but old targets will remain set.  XXX XXX
 *
 * The player can choose to be disturbed by several things, including
 * "disturb_move" (monster which is viewable moves in some way), and
 * "disturb_near" (monster which is "easily" viewable moves in some
 * way).  Note that "moves" includes "appears" and "disappears".
 *
 * DJA: "disturb_near" now only triggers when the monster is aware of you.
 * (or when you're running and about to run into a monster)
 */
void update_mon(int m_idx, bool full)
{
	bool do_invisible = FALSE;
	bool darkvs, msilent, lore;
	int hearcheck, discernmod, bluff;
	u32b f4;
	int d;

	monster_type *m_ptr = &mon_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];
	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	/* Current location */
	int fy = m_ptr->fy;
	int fx = m_ptr->fx;

	/* Seen at all */
	bool flag = FALSE;

	/* Seen by vision */
	bool easy = FALSE;


	int py = p_ptr->py;
	int px = p_ptr->px;

	/* Compute distance */
	if (full)
	{
		/* Distance components */
		int dy = (py > fy) ? (py - fy) : (fy - py);
		int dx = (px > fx) ? (px - fx) : (fx - px);

		/* Approximate distance */
		d = (dy > dx) ? (dy + (dx>>1)) : (dx + (dy>>1));

		/* Restrict distance */
		if (d > 255) d = 255;

		/* Save the distance */
		m_ptr->cdis = d;
	}

	/* Extract distance */
	else
	{
		/* Extract the distance */
		d = m_ptr->cdis;
	}

	/* paranoia */
	if ((p_ptr->timed[TMD_BEAR_HOLD]) && (p_ptr->held_m_idx == m_idx))
	{
		/* prevent non-adjacent monsters holding the player */
		if (m_ptr->cdis > 2)
		{
			p_ptr->held_m_idx = 0;
			clear_timed(TMD_BEAR_HOLD);
		}
	}

	/* Detected */
	if (m_ptr->mflag & (MFLAG_MARK)) flag = TRUE;


	/* Nearby */
	if (d <= MAX_SIGHT)
	{

		/* Basic telepathy */
		if (p_ptr->telepathy)
		{
			/* Empty mind, no telepathy */
			if (r_ptr->flags2 & (RF2_EMPTY_MIND))
			{
				/* Memorize flags */
				l_ptr->flags2 |= (RF2_EMPTY_MIND);
			}

			/* Weird mind, occasional telepathy */
			else if (r_ptr->flags2 & (RF2_WEIRD_MIND))
			{
				/* One in nine individuals are detectable */
				if ((m_idx % 9) == 5)
				{
					/* Detectable */
					flag = TRUE;
					
					/* monster moved into esp range */
					if ((!m_ptr->ml) && (disturb_espmove)) disturb(1, 0);

					/* Memorize flags */
					l_ptr->flags2 |= (RF2_WEIRD_MIND);

					/* Hack -- Memorize mental flags */
					if (r_ptr->flags2 & (RF2_SMART)) l_ptr->flags2 |= (RF2_SMART);
					if (r_ptr->flags2 & (RF2_STUPID)) l_ptr->flags2 |= (RF2_STUPID);
				}
			}

			/* Normal mind, allow telepathy */
			else
			{
				/* Detectable */
				flag = TRUE;
					
				/* monster moved into esp range */
				if ((!m_ptr->ml) && (disturb_espmove)) disturb(1, 0);

				/* Hack -- Memorize mental flags */
				if (r_ptr->flags2 & (RF2_SMART)) l_ptr->flags2 |= (RF2_SMART);
				if (r_ptr->flags2 & (RF2_STUPID)) l_ptr->flags2 |= (RF2_STUPID);
			}
		}

		/* Normal line of sight, and not blind */
		if (player_has_los_bold(fy, fx) && !p_ptr->timed[TMD_BLIND])
		{
			/* Use "illumination" */
			if (player_can_see_bold(fy, fx))
			{
				/* Handle "invisible" monsters */
				if (r_ptr->flags2 & (RF2_INVISIBLE))
				{
					/* Take note */
					do_invisible = TRUE;

					/* See invisible */
					if (p_ptr->see_inv)
					{
						/* Easy to see */
						easy = TRUE;
					}
				}

				/* Handle "normal" monsters */
				else
				{
					/* Easy to see */
					easy = TRUE;
				}
			}

			/* Visible */
			if ((flag) || (easy))
			{
				/* Memorize flags */
				if (do_invisible) l_ptr->flags2 |= (RF2_INVISIBLE);
			}
			
			/* Temporary invisibility */
			if (m_ptr->tinvis)
			{
                m_ptr->tinvis -= 1;
                if (easy)
                {
				   do_invisible = TRUE;

				   /* See invisible */
				   if (!p_ptr->see_inv)
				   {
					  /* dissapears! */
					  easy = FALSE;
				   }
                }
            }
		}
	}

    /* Darkvision: If monster is in line of sight, the player is not blind, */ 
    /* and the monster is not invisible, then the player can see it */
    /* (even if the space is not lit) */
    /* (should darkvision let you see walls without light?) */
	darkvs = FALSE;
    if (((p_ptr->darkvis) || (p_ptr->timed[TMD_DARKVIS])) &&
	   (player_has_los_bold(fy, fx)) && (!p_ptr->timed[TMD_BLIND]))
	{   
		/* make sure invisible monsters are marked as invisible for darkvision */
		if (r_ptr->flags2 & (RF2_INVISIBLE)) do_invisible = TRUE;

		if ((p_ptr->see_inv) || (!do_invisible))
		{
			easy = TRUE;
			darkvs = TRUE;
		}
    }

	/* if a monster would otherwise be seen easily, check monster stealth */
	/* palert is evaluated in calc_bonuses in xtra1.c */
	if (easy)
	{
        int mstealth = (r_ptr->stealth * 11) + 1;
        if ((r_ptr->stealth > 1) && (d < 15)) mstealth += d * 2; /* distance */
        if (goodluck > 5) mstealth -= (goodluck/2);
        else if (goodluck > 3) mstealth -= 1;
        if (badluck > 3) mstealth += (badluck/2);
        
        if ((d > 14) && (r_ptr->stealth > 2)) mstealth += (r_ptr->stealth*10);
        else if ((d > 14) && (r_ptr->stealth == 2)) mstealth += 30;
        else if (d > 14) mstealth += 16;
        else if ((d > 3) && (r_ptr->stealth > 3)) mstealth += (d-3) * (r_ptr->stealth - 2);
        else if (d > 3) mstealth += (d-3);
        else if (d < 2) mstealth -= 16;

        if ((m_ptr->stunned) || (m_ptr->confused) || (m_ptr->charmed)) mstealth -= 16;
        if (p_ptr->depth == 0) mstealth -= 16;
                
        /* monster hasn't been noticed yet (it is still hiding) */
        /* darkvision makes it hard to hide in the shadows */
        /* this helps dwarves who have darkvision but horrible alertness */
        if ((!m_ptr->monseen) && (r_ptr->stealth > 2) && (!darkvs)) mstealth += 20;
		else if ((!m_ptr->monseen) && (r_ptr->stealth > 2)) mstealth += 8;

        /* much easier to notice if you've noticed it before */
        if ((m_ptr->monseen > 3) && (d < 5) && (!m_ptr->monfear)) mstealth -= m_ptr->monseen * 15;
        else if ((m_ptr->monseen > 0) && (d < 7) && (!m_ptr->monfear)) mstealth -= 60;
        else if (m_ptr->monseen > 2) mstealth -= m_ptr->monseen * 4;
        else if ((m_ptr->monseen > 0) || (m_ptr->heard)) mstealth -= 12;
        
        /* sleeping monsters can't actively hide */
        if ((m_ptr->csleep) && (!m_ptr->roaming)) mstealth -= 10;

        /* NEVER_MOVE monsters can't try to hide once they've been noticed */
        if ((m_ptr->monseen > 0) && (r_ptr->flags1 & (RF1_NEVER_MOVE))) mstealth -= 20;

#if irrevelentnow
        /* always visible if it just hit you or you hit it */
        /* may be irrevelent now that you always notice adjacent monsters */
        if (m_ptr->monseen > 9)
        {
           m_ptr->monseen -= 1;
           mstealth = 1;
        }
#endif
        /* prevent randint(negative mstealth) because that makes it crash */
        if (mstealth < 1) mstealth = 1;

        /* examples which assume monseen is 0 and luck is 0: */
        /* monster of stealth 1 at a distance of 8 now has mstealth of 16 */
        /* monster of stealth 1 at a distance of 15 now has mstealth of 27 */
        /* monster of stealth 3 at a distance of 15 now has mstealth of 71 */
        /* monster of stealth 5 at a distance of 15 now has mstealth of 113 */
        /* monster of stealth 2 at a distance of 9 now has mstealth of 54 */
        /* monster of stealth 3 at a distance of 7 now has mstealth of 59 */
        /* monster of stealth 4 at a distance of 6 now has mstealth of 70 */
        /* monster of stealth 5 at a distance of 5 now has mstealth of 79 */
        /* monster of stealth 6 at a distance of 4 now has mstealth of 86 */
        /* monster of stealth 5 at a distance of 1 now has mstealth of 53 */
        /* monster of stealth 6 at a distance of 8 now has mstealth of 110 */
        /* does the player notice the monster? */
        if (d > 2)
        {
              /* give monsters a few chances to avoid being noticed */
              if ((mstealth > (palert + 50)) && (m_ptr->monseen < 1)) easy = FALSE;
              if ((mstealth > (palert * 2)) && (randint(mstealth) > palert + 4)) easy = FALSE;
              if (randint(mstealth) > palert) easy = FALSE;
              if (randint(mstealth) > palert + rp_ptr->r_fos) easy = FALSE;
              if ((palert > mstealth) && (r_ptr->stealth > 1) && 
                  (randint(palert * 20) < (mstealth/2 + d + badluck/2 - goodluck/2))) easy = FALSE;
        }
        else if (d == 2)
        {
              /* only one chance to escape notice when very close */
              if (randint(mstealth) > palert) easy = FALSE;
        }
        /* d == 1: always notice if adjacent */
    }
    /* an extremely alert character may notice signs of an invisible monster */
    else if ((!p_ptr->see_inv) && (r_ptr->flags2 & (RF2_INVISIBLE)))
    {
        int mstealth = 50 + (r_ptr->stealth * 5) + (d * 2);
        if (goodluck > 4) mstealth -= (goodluck/4 + randint(goodluck/2));
        if (badluck > 4) mstealth += (badluck/4 + rand_int(badluck/3));
        if ((r_ptr->flags3 & (RF3_UNDEAD)) || (r_ptr->flags3 & (RF3_SILVER)))
        {
           mstealth += 24;
        }

        if ((d > 3) && (r_ptr->stealth > 3)) mstealth += (d-3) * 2;
        else if (d > 3) mstealth += (d-3);
        else if (d < 2) mstealth -= 10;
        if ((m_ptr->monseen > 0) && (r_ptr->flags1 & (RF1_NEVER_MOVE))) mstealth -= (5 + m_ptr->monseen);
        if (m_ptr->monseen > 0) mstealth -= m_ptr->monseen;

        /* minimum stealth for invisibile monsters */
        if (mstealth < 30) mstealth = 30;
        if (palert < 1) palert = 1;

        /* impossible to notice invisible monsters further than 6 spaces away */
        if ((d < 7) && (r_ptr->stealth < 5))
        {
              if (randint(palert) > mstealth)
              {
                  easy = TRUE;
                  msg_format("You notice the signs of an invisible monster!");
              }
        }
    }

    /* once it's noticed by the player it's easier to see afterwords */
    if ((easy) && (m_ptr->monseen < 1)) m_ptr->monseen = 2;

    /* monster has been seen but is hiding again */
    if ((!easy) && (m_ptr->monseen > 2)) m_ptr->monseen -= 1;
    else if ((!easy) && (m_ptr->monseen > 1) && (randint(200) < r_ptr->stealth*5))
          m_ptr->monseen -= 1;
    if (m_ptr->monseen > 5) m_ptr->monseen -= 1;

    /* is it still visible after monster stealth is checked? */
    if (easy) flag = TRUE;

#if notyet
	/* not yet because I want to see if this happens anyway */
    /* when running, disturb when adjacent instead of after running into it */
    if ((flag) && (d == 1) && (p_ptr->running) && (disturb_near)) disturb(1, 0);
#endif

	/*** DJA: hearing out-of-sight monsters ***/
	/* maybe should add a 'move silently' stat for monsters separate from 'hide' stealth */
    hearcheck = (palert * 4) + (goodluck/2);
	/* hear better when you're blind */
	if (p_ptr->timed[TMD_BLIND]) hearcheck += 100;
	/* not attentive when you're running */
    if (p_ptr->running) hearcheck = hearcheck/2;
	/* hear it better when you're listening for it */
    else if (m_ptr->heard) hearcheck += 100;

	/* PASS_WALL and NEVER_MOVE monsters are considered completely silent */
	/* (unless their stealth is 0: poltergeists, green glutton ghosts, and earth elementals) */
	msilent = FALSE; /* assume not completely silent */
	if ((r_ptr->flags2 & (RF2_PASS_WALL)) && (r_ptr->stealth)) msilent = TRUE;
	if (r_ptr->flags1 & (RF1_NEVER_MOVE)) msilent = TRUE;
	f4 = r_ptr->flags4; /* check for THROW flag */
	/* poltergeists make a lot of noise */
	if (r_ptr->flags4 & (RF4_THROW)) hearcheck += 30;
	/* sleeping monsters are silent */
	if ((m_ptr->csleep) && (!m_ptr->roaming)) msilent = TRUE;
	/* don't hear offscreen monsters */
	if (!panel_contains(fy, fx)) msilent = TRUE;
	/* alertness isn't high enough */
	if ((r_ptr->stealth == 3) && (palert < 26)) msilent = TRUE;
	if ((r_ptr->stealth == 2) && (palert < 20)) msilent = TRUE;

	/* if monster is nearby and not already visible there's a chance to hear it */
	/* the town is considered safe, so don't bother if monster level is 0 */
	if ((!flag) && (r_ptr->stealth < 4) && (d < MAX_SIGHT) &&
	   (!p_ptr->timed[TMD_IMAGE]) && (r_ptr->level) && (!msilent) &&
	   (randint(2500 + (r_ptr->stealth * 500) + (d * 25)) < hearcheck))
	{
		bool eardisturb = FALSE;

		/* Draw the monster (always grey color) */
		lite_spot(fy, fx);

		/* decide whether to disturb */
		if (disturb_move) eardisturb = TRUE;
		/* don't disturb when running, resting, or digging */
		if ((p_ptr->resting) || (p_ptr->running) || (p_ptr->command_rep)) eardisturb = FALSE;
		/* ..unless it's in line of sight */
		if ((disturb_near) && (player_has_los_bold(fy, fx))) eardisturb = TRUE;

		/* disturb on hearing */
		if ((!m_ptr->heard) && (eardisturb))
		{
			/* (no message if no disturb) */
			msg_format("You hear a monster nearby but out of sight.");
			disturb(1, 0);
		}

        /* you heard it, now you're listening for it */
        m_ptr->heard = TRUE;

        /* you heard it, now you're watching for it */
        if (m_ptr->monseen < 1) m_ptr->monseen = 1;

		/* can't target or examine the monster */
		/* does not appear on visible monster list (isn't visible) */
    }
    else m_ptr->heard = FALSE;

	/* The monster is now visible */
	if (flag)
	{
		/* It was previously unseen */
		if (!m_ptr->ml)
		{
			/* Mark as visible */
			m_ptr->ml = TRUE;

			/* Draw the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);

			/* Hack -- Count "fresh" sightings */
			if (l_ptr->sights < MAX_SHORT) l_ptr->sights++;

			/* Disturb on appearance */
			if (disturb_move) disturb(1, 0);

			/* Window stuff */
			p_ptr->window |= PW_MONLIST;
		}
	}

	/* The monster is not visible */
	else
	{
		/* It was previously seen */
		if (m_ptr->ml)
		{
			/* Mark as not visible */
			m_ptr->ml = FALSE;

			/* Erase the monster */
			lite_spot(fy, fx);

			/* Update health bar as needed */
			if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);

			/* Disturb on disappearance */
			if (disturb_move) disturb(1, 0);

			/* Window stuff */
			p_ptr->window |= PW_MONLIST;
		}
	}

	/* The monster is now easily visible */
	if (easy)
	{
		/* Change */
		if (!(m_ptr->mflag & (MFLAG_VIEW)))
		{
			/* Mark as easily visible */
			m_ptr->mflag |= (MFLAG_VIEW);

			/* Disturb on appearance (even if roaming) */
			if (disturb_near) disturb(1, 0);

			/* Window stuff */
			p_ptr->window |= PW_MONLIST;
		}
	}

	/* The monster is not easily visible */
	else
	{
		/* Change */
		if (m_ptr->mflag & (MFLAG_VIEW))
		{
			/* Mark as not easily visible */
			m_ptr->mflag &= ~(MFLAG_VIEW);

			/* Disturb on appearance */
			if ((disturb_near) && ((!m_ptr->roaming) || (m_ptr->roaming < 20))) disturb(1, 0);

			/* Window stuff */
			p_ptr->window |= PW_MONLIST;
		}
	}

	
    /* monster now has LOS to the player */
    if ((m_ptr->cdis <= MAX_RANGE) && (projectable(m_ptr->fy, m_ptr->fx, py, px)))
	{
        if (!(m_ptr->mflag & (MFLAG_MLOS)))
        {
			/* Mark as having LOS to the player */
			m_ptr->mflag |= (MFLAG_MLOS);
			
            /* only note if awake and sensed by telepathy */
            if ((disturb_espmove) && (flag) && (!easy) && (!m_ptr->csleep))
            {
               disturb(1, 0);
#if nomessage
               /* get monster name */
		       char m_name[80];
               monster_desc(m_name, sizeof(m_name), m_ptr, 0x04);
                       
               /* should there be a message here? */
               msg_format("You sense that %s can see you.", m_name);
#endif
            }
        }
    }
    else if (m_ptr->mflag & (MFLAG_MLOS))
    {
		/* Mark as not having LOS to the player */
		m_ptr->mflag &= ~(MFLAG_MLOS);
    }

	/* check monster's awareness of you */
	/* asleep but displayed as awake in monster list */
	if ((m_ptr->csleep) && (!m_ptr->display_sleep))
	{
	   /* update monster list */
	   p_ptr->window |= PW_MONLIST;
    }
    /* awake but displayed as asleep in monster list */
	if ((!m_ptr->csleep) && (m_ptr->display_sleep))
	{
	   /* update monster list */
	   p_ptr->window |= PW_MONLIST;
    }

   /* done if you've met the monster before or if you're not meeting it now */
   if ((m_ptr->meet) || (!easy)) return;

   /* "Nice to meet you" (First impression) */
   if (easy)
   {
       /* Meeting the monster for the first time */
       m_ptr->meet = 1;
      
       /*** Can you discern if the monster is evil? ***/

       /* discernment based on wisdom */
       discernmod = (adj_mag_study[p_ptr->stat_ind[A_WIS]] / 10);

       /* paladins almost always recognise evil */
       if ((cp_ptr->spell_book == TV_PRAYER_BOOK) && (!cp_ptr->flags & CF_ZERO_FAIL))
       {
          discernmod += 27;
       }
       /* aligned classes recognise alignment easier */
       /* (priests and druids but not healers) */
       else if ((cp_ptr->flags & CF_BLESS_WEAPON) ||
            (cp_ptr->spell_book == TV_DARK_BOOK))
       {
            /* wielding equipment with alignment that matches yours */
            if ((magicmod == 4) || (magicmod == 6)) discernmod += 14;
            else discernmod += 7;
       }
       /* small bonus for healers */
       else if (cp_ptr->spell_book == TV_PRAYER_BOOK)
       {
            discernmod += 3;
       }

       /* chance realm casters get modifier from luck */
       if (cp_ptr->spell_book == TV_LUCK_BOOK)
       {
          if (goodluck > 2) discernmod += goodluck/3;
          if (badluck > 2) discernmod -= badluck/3;
       }
       /* everyone else gets much weaker modifier from luck */
       else
       {
           if (goodluck > 5) discernmod += goodluck/6;
           if (badluck > 4) discernmod -= badluck/5;
       }

       lore = TRUE;

       /* always recognise if you know the race is always evil */
       if (l_ptr->flags3 & (RF3_EVIL)) discernmod += 50;
       /* easier to recognise if you know it's sometimes evil */
       else if (l_ptr->flags2 & (RF2_S_EVIL2)) discernmod += 4;
       else if (l_ptr->flags2 & (RF2_S_EVIL1)) discernmod += 2;
       else lore = FALSE;
   
       if (!lore)
       {
          /* always evil monsters are easier to recognise */
          if (r_ptr->flags3 & (RF3_EVIL)) discernmod += 2;
          /* occationally evil monsters are harder to recognise */
          /* if you don't know that they're sometimes evil */
          else if (r_ptr->flags2 & (RF2_S_EVIL2)) discernmod -= 5;
          else if (r_ptr->flags2 & (RF2_S_EVIL1)) discernmod -= 10;
       }
   
       /* discern */
       if (r_ptr->level/20 + randint(25) < discernmod)
       {
          /* If it's always evil then the monster lore tells you that it's evil */
          if (r_ptr->flags3 & (RF3_EVIL))
          {
             /* remember */
             l_ptr->flags3 |= (RF3_EVIL);

             /* don't need a separate message telling you it's evil */
          }
          else if (m_ptr->evil)
          {
              /* can tell (later) that the monster is evil */
              m_ptr->meet = 2;
              
              bluff = r_ptr->level + ((r_ptr->stealth + 2) * 2);
              
              /* remember */
              if (l_ptr->sights + (discernmod/2) > bluff)
              {
                 if (r_ptr->flags2 & (RF2_S_EVIL2)) l_ptr->flags2 |= (RF2_S_EVIL2);
                 else if (r_ptr->flags2 & (RF2_S_EVIL1)) l_ptr->flags2 |= (RF2_S_EVIL1);
              }
          }
          /* (much harder to tell for sure that a monster is NOT evil) */
          else if (r_ptr->level/10 + randint(60) < discernmod)
          {
              /* can tell (later) that the monster is not evil */
              m_ptr->meet = 3;
          }
       }
       /* if you fail this first chance, a detect evil spell is the only */
       /* way to find out if an individual monster is evil */
       /* and there's no other way to know that a specific monster is not evil */
   }
}




/*
 * This function simply updates all the (non-dead) monsters (see above).
 */
void update_monsters(bool full)
{
	int i;

	/* Update each (live) monster */
	for (i = 1; i < mon_max; i++)
	{
		monster_type *m_ptr = &mon_list[i];

		/* Skip dead monsters */
		if (!m_ptr->r_idx) continue;

		/* Update the monster */
		update_mon(i, full);
	}
}




/*
 * Make a monster carry an object
 */
s16b monster_carry(int m_idx, object_type *j_ptr)
{
	s16b o_idx;

	s16b this_o_idx, next_o_idx = 0;

	monster_type *m_ptr = &mon_list[m_idx];


	/* Scan objects already being held for combination */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Check for combination */
		if (object_similar(o_ptr, j_ptr))
		{
			/* Combine the items */
			object_absorb(o_ptr, j_ptr);

			/* Result */
			return (this_o_idx);
		}
	}


	/* Make an object */
	o_idx = o_pop();

	/* Success */
	if (o_idx)
	{
		object_type *o_ptr;

		/* Get new object */
		o_ptr = &o_list[o_idx];

		/* Copy object */
		object_copy(o_ptr, j_ptr);

		/* Forget mark */
		o_ptr->marked = FALSE;

		/* Forget location */
		o_ptr->iy = o_ptr->ix = 0;

		/* Link the object to the monster */
		o_ptr->held_m_idx = m_idx;

		/* Link the object to the pile */
		o_ptr->next_o_idx = m_ptr->hold_o_idx;

		/* Link the monster to the object */
		m_ptr->hold_o_idx = o_idx;
	}

	/* Result */
	return (o_idx);
}


/*
 * Swap the players/monsters (if any) at two locations XXX XXX XXX
 */
void monster_swap(int y1, int x1, int y2, int x2)
{
	int m1, m2;

	monster_type *m_ptr;


	/* Monsters */
	m1 = cave_m_idx[y1][x1];
	m2 = cave_m_idx[y2][x2];


	/* Update grids */
	cave_m_idx[y1][x1] = m2;
	cave_m_idx[y2][x2] = m1;


	/* Monster 1 */
	if (m1 > 0)
	{
		m_ptr = &mon_list[m1];

		/* Move monster */
		m_ptr->fy = y2;
		m_ptr->fx = x2;

		/* Update monster */
		update_mon(m1, TRUE);
	}

	/* Player 1 */
	else if (m1 < 0)
	{
		/* Move player */
		p_ptr->py = y2;
		p_ptr->px = x2;

		/* Update the panel */
		p_ptr->update |= (PU_PANEL);

		/* Update the visuals (and monster distances) */
		p_ptr->update |= (PU_UPDATE_VIEW | PU_DISTANCE);

		/* Update the flow */
		p_ptr->update |= (PU_UPDATE_FLOW);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_MAP);
	}

	/* Monster 2 */
	if (m2 > 0)
	{
		m_ptr = &mon_list[m2];

		/* Move monster */
		m_ptr->fy = y1;
		m_ptr->fx = x1;

		/* Update monster */
		update_mon(m2, TRUE);
	}

	/* Player 2 */
	else if (m2 < 0)
	{
		/* Move player */
		p_ptr->py = y1;
		p_ptr->px = x1;

		/* Update the panel */
		p_ptr->update |= (PU_PANEL);

		/* Update the visuals (and monster distances) */
		p_ptr->update |= (PU_UPDATE_VIEW | PU_DISTANCE);

		/* Update the flow */
		p_ptr->update |= (PU_UPDATE_FLOW);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD | PW_MAP);
	}


	/* Redraw */
	lite_spot(y1, x1);
	lite_spot(y2, x2);
}


/*
 * Place the player in the dungeon XXX XXX
 */
s16b player_place(int y, int x)
{
	/* Paranoia XXX XXX */
	if (cave_m_idx[y][x] != 0) return (0);


	/* Save player location */
	p_ptr->py = y;
	p_ptr->px = x;

	/* Mark cave grid */
	cave_m_idx[y][x] = -1;

	/* Success */
	return (-1);
}


/*
 * Place a copy of a monster in the dungeon XXX XXX
 */
s16b monster_place(int y, int x, monster_type *n_ptr)
{
	s16b m_idx;

	monster_type *m_ptr;
	monster_race *r_ptr;


	/* Paranoia XXX XXX */
	if (cave_m_idx[y][x] != 0) return (0);


	/* Get a new record */
	m_idx = mon_pop();

	/* Oops */
	if (m_idx)
	{
		/* Make a new monster */
		cave_m_idx[y][x] = m_idx;

		/* Get the new monster */
		m_ptr = &mon_list[m_idx];

		/* Copy the monster XXX */
		COPY(m_ptr, n_ptr, monster_type);

		/* Location */
		m_ptr->fy = y;
		m_ptr->fx = x;

		/* Update the monster */
		update_mon(m_idx, TRUE);

		/* Get the new race */
		r_ptr = &r_info[m_ptr->r_idx];

		/* Hack -- Notice new multi-hued monsters */
		if (r_ptr->flags1 & (RF1_ATTR_MULTI)) shimmer_monsters = TRUE;

		/* Hack -- Count the number of "reproducers" */
		if (r_ptr->flags2 & (RF2_MULTIPLY)) num_repro++;

		/* Count racial occurances */
		r_ptr->cur_num++;
	}

	/* Result */
	return (m_idx);
}



/*
 * Attempt to place a monster of the given race at the given location.
 *
 * To give the player a sporting chance, any monster that appears in
 * line-of-sight and is extremely dangerous can be marked as
 * "FORCE_SLEEP", which will cause them to be placed with low energy,
 * which often (but not always) lets the player move before they do.
 *
 * This routine refuses to place out-of-depth "FORCE_DEPTH" monsters.
 *
 * XXX XXX XXX Use special "here" and "dead" flags for unique monsters,
 * remove old "cur_num" and "max_num" fields.
 *
 * XXX XXX XXX Actually, do something similar for artifacts, to simplify
 * the "preserve" mode, and to make the "what artifacts" flag more useful.
 *
 * This is the only function which may place a monster in the dungeon,
 * except for the savefile loading code.
 */
static bool place_monster_one(int y, int x, int r_idx, bool slp, bool group)
{
	int i;
	byte isroam;
	bool roamflag;
	int evilchance;

	monster_race *r_ptr;

	monster_type *n_ptr;
	monster_type monster_type_body;

	cptr name;


	/* Paranoia */
	if (!in_bounds(y, x)) return (FALSE);

	/* Require empty space */
	if (!cave_empty_bold(y, x)) return (FALSE);

	/* Hack -- no creation on glyph of warding */
	if (cave_feat[y][x] == FEAT_GLYPH) return (FALSE);


	/* Paranoia */
	if (!r_idx) return (FALSE);

	/* Race */
	r_ptr = &r_info[r_idx];

	/* Paranoia */
	if (!r_ptr->name) return (FALSE);

	/* Name */
	name = (r_name + r_ptr->name);


	/* Hack -- "unique" monsters must be "unique" */
	if ((r_ptr->flags1 & (RF1_UNIQUE)) && (r_ptr->cur_num >= r_ptr->max_num))
	{
		/* Cannot create */
		return (FALSE);
	}


	/* Depth monsters may NOT be created out of depth */
	if ((r_ptr->flags1 & (RF1_FORCE_DEPTH)) && (p_ptr->depth < r_ptr->level))
	{
		/* Cannot create */
		return (FALSE);
	}


	/* Powerful monster */
	if (r_ptr->level > p_ptr->depth)
	{
		/* Unique monsters */
		if (r_ptr->flags1 & (RF1_UNIQUE))
		{
			/* Message for cheaters */
			if (cheat_hear) msg_format("Deep Unique (%s).", name);

			/* Boost rating by twice delta-depth */
			rating += (r_ptr->level - p_ptr->depth) * 2;
		}

		/* Normal monsters */
		else
		{
			/* Message for cheaters */
			if (cheat_hear) msg_format("Deep Monster (%s).", name);

			/* Boost rating by delta-depth */
			rating += (r_ptr->level - p_ptr->depth);
		}
	}

	/* Note the monster */
	else if (r_ptr->flags1 & (RF1_UNIQUE))
	{
		/* Unique monsters induce message */
		if (cheat_hear) msg_format("Unique (%s).", name);
	}


	/* Get local monster */
	n_ptr = &monster_type_body;

	/* Clean out the monster */
	(void)WIPE(n_ptr, monster_type);


	/* Save the race */
	n_ptr->r_idx = r_idx;


	/* likeliness of being awake without noticing the player */
	/* currently no monster has both ROAM1 and ROAM2 but could use that to stack */
	/* slp is only FALSE when monster is being generated over time after dungeon creation */
	isroam = 1;
	roamflag = FALSE;
	if ((r_ptr->flags2 & (RF2_ROAM2)) || (r_ptr->flags2 & (RF2_ROAM1))) roamflag = TRUE;
	if (r_ptr->flags2 & (RF2_ROAM1)) isroam += 25;
	if (r_ptr->flags2 & (RF2_ROAM2)) isroam += 74;
	/* monsters who are immune to sleep shouldn't sleep */
	if ((r_ptr->flags3 & (RF3_NO_SLEEP)) && (!roamflag)) isroam += 15;
	else if (r_ptr->flags3 & (RF3_NO_SLEEP)) isroam += 5;
	/* townspeople shouldn't just stand there (shouldn't have ROAM2) */
	if (r_ptr->level == 0) isroam += 55;
	/* some types of monsters rarely roam */
	if ((strchr("eglLsVwWXz$.", r_ptr->d_char)) && (!roamflag)) isroam = isroam/3;
	/* some types of monsters never roam */
	if ((!r_ptr->sleep) && (!roamflag)) isroam = 0;
	if ((strchr(",dDEjmQvZ%?!=_~", r_ptr->d_char)) && (!roamflag)) isroam = 0;

	/* is it roaming? */
	if (randint(100) < isroam)
    {
        n_ptr->roaming = 1;
        /* roams as part of a group */
        if (group) n_ptr->roaming = 11;
    }
	else n_ptr->roaming = 0;

	/* Enforce sleeping if needed */
	/* (old: all monsters start out asleep unless it was random generation over time */
	/* or unless they have 0 in the monster alertness field in monster.txt) */
	/* now: n_ptr->csleep means hasn't noticed the player yet, not nessesarily sleeping */
	/* if n_ptr->csleep > 0 and n_ptr->roaming = 0 then monster is asleep */
	if ((slp) && ((r_ptr->sleep) || (n_ptr->roaming)))
	{
		int val = r_ptr->sleep;
		
        /* town monsters ignore you most of the time */
       	if (!r_ptr->level) n_ptr->csleep = (val * 4) + randint(val * 9);
       	else n_ptr->csleep = (val * 2) + randint(val * 10);
	}


	/* Assign maximal hitpoints */
	if (r_ptr->flags1 & (RF1_FORCE_MAXHP))
	{
		n_ptr->maxhp = maxroll(r_ptr->hdice, r_ptr->hside);
	}
	else
	{
		n_ptr->maxhp = damroll(r_ptr->hdice, r_ptr->hside);
	}

	/* And start out fully healthy */
	n_ptr->hp = n_ptr->maxhp;

	/* Extract the monster base speed */
	n_ptr->mspeed = r_ptr->speed;

	/* Some monster races are sometimes evil but not always */
	evilchance = 0;
	/* uniques never have S_EVIL1 or S_EVIL2 */
	/* sometimes evil (should never be invisible except on very high levels) */
	if (r_ptr->flags2 & (RF2_S_EVIL1)) evilchance = 22;
	/* usually evil */
	else if (r_ptr->flags2 & (RF2_S_EVIL2)) evilchance = 78;
	
	/* if it's invisible, it's evil more often so it'll be easier to find */
	if ((evilchance) && (r_ptr->flags2 & (RF2_INVISIBLE)))
    {
        if (p_ptr->depth < 60) evilchance += (61 - p_ptr->depth)/3 + 1;
    }

    if ((randint(100) < evilchance) || (r_ptr->flags3 & (RF3_EVIL)))
    {
        n_ptr->evil = 1;
    }
    else
    {
        n_ptr->evil = 0;
    }
    
    /* nonevil monsters take less notice of you */
    if ((evilchance) && (n_ptr->csleep) && (!n_ptr->evil))
    {
        if (n_ptr->csleep < r_ptr->sleep * 6) n_ptr->csleep += r_ptr->sleep * 2 + 1;
    }
    else if ((evilchance) && (n_ptr->csleep))
    {
        if (n_ptr->csleep > r_ptr->sleep * 8) n_ptr->csleep -= r_ptr->sleep * 2;
    }

    /* you've never met this monster */
	n_ptr->meet = 0;

	/* Hack -- small racial variety */
	if (!(r_ptr->flags1 & (RF1_UNIQUE)))
	{
		/* Allow some small variation per monster */
		i = extract_energy[r_ptr->speed] / 10;
		if (i) n_ptr->mspeed += rand_spread(0, i);
	}


	/* Give a random starting energy */
	n_ptr->energy = (byte)rand_int(100);

	/* Force monster to wait for player */
	if (r_ptr->flags1 & (RF1_FORCE_SLEEP))
	{
		/* Monster is still being nice */
		n_ptr->mflag |= (MFLAG_NICE);

		/* Optimize -- Repair flags */
		repair_mflag_nice = TRUE;
	}

	/* Place the monster in the dungeon */
	if (!monster_place(y, x, n_ptr)) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Maximum size of a group of monsters
 */
#define GROUP_MAX	31


/*
 * Attempt to place a "group" of monsters around the given location
 */
static bool place_monster_group(int y, int x, int r_idx, bool slp)
{
	monster_race *r_ptr = &r_info[r_idx];

	int old, n, i;
	int tag = 0;
	int total, extra = 0;
	int hack_n;

	byte hack_y[GROUP_MAX];
	byte hack_x[GROUP_MAX];


	/* Pick a group size */
	total = randint(13);

	/* Hard monsters, small groups */
	if (r_ptr->level > p_ptr->depth)
	{
		extra = r_ptr->level - p_ptr->depth;
		extra = 0 - randint(extra);
	}

	/* Easy monsters, large groups */
	else if (r_ptr->level < p_ptr->depth)
	{
		extra = p_ptr->depth - r_ptr->level;
		extra = randint(extra);
	}

	/* Hack -- limit group reduction */
	if (extra > 12) extra = 12;

	/* Modify the group size */
	total += extra;

	/* Minimum size */
	if (total < 1) total = 1;

	/* Maximum size */
	if (total > GROUP_MAX) total = GROUP_MAX;

	/* Pairs */
	if ((r_ptr->flags1 & RF1_FRIEND) && (total > 2))
	{
       	if ((extra == 12) && (randint(100) < 60)) total = 4;
       	else if ((extra > 7) && (randint(100) < 20)) total = 3;
       	else total = 2;
    }

	/* small groups */
	if ((r_ptr->flags2 & RF2_FRIEND1) && (total > 7))
	{
       	if ((extra > 8) && (randint(100) < 50)) total = ((total * 3) / 4);
       	else total = ((total * 2) / 3);
	    if ((r_ptr->level > p_ptr->depth) && (total > 8)) total = 8;
    }

	/* Save the rating */
	old = rating;

	/* Start on the monster */
	hack_n = 1;
	hack_x[0] = x;
	hack_y[0] = y;

	/* maybe add a tribe shaman to the group */
	if ((r_ptr->flags3 & RF3_ORC) && (p_ptr->depth > 9))
	{
		int die = randint(101 - badluck);
		if (die < 35) tag = 155; /* orc shaman */
		else if ((die < 40) && (p_ptr->depth > 14)) tag = 187; /* fire orc */
	}
	else if (strchr("x", r_ptr->d_char))
	{
		int die = randint(70 + p_ptr->depth);
		if (die > 135) tag = 547; /* riverflow gargoyle */
		else if (die > 100) tag = 556; /* margoyle shaman */
	}
	else if (strchr("O", r_ptr->d_char))
	{
		int die = randint(70 + p_ptr->depth);
		if (die > 80) tag = 397; /* ogre shaman */
	}

	/* Puddle monsters, breadth first, up to total */
	for (n = 0; (n < hack_n) && (hack_n < total); n++)
	{
		/* Grab the location */
		int hx = hack_x[n];
		int hy = hack_y[n];

		/* Check each direction, up to total */
		for (i = 0; (i < 8) && (hack_n < total); i++)
		{
			int mx = hx + ddx_ddd[i];
			int my = hy + ddy_ddd[i];

			/* Walls and Monsters block flow */
			if (!cave_empty_bold(my, mx)) continue;

			/* maybe add a tribe shaman to the group */
			if ((hack_n == (total - 1)) && (tag))
			{
				if (place_monster_one(my, mx, tag, slp, TRUE))
				{
					/* Add it to the "hack" set */
					hack_y[hack_n] = my;
					hack_x[hack_n] = mx;
					hack_n++;
					continue;
				}
			}

			/* Attempt to place another monster */
			if (place_monster_one(my, mx, r_idx, slp, TRUE))
			{
				/* Add it to the "hack" set */
				hack_y[hack_n] = my;
				hack_x[hack_n] = mx;
				hack_n++;
			}
		}
	}

	/* Hack -- restore the rating */
	rating = old;


	/* Success */
	return (TRUE);
}


/*
 * Hack -- help pick an escort type
 */
static int place_monster_idx = 0;

/*
 * Hack -- help pick an escort type
 */
static bool place_monster_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[place_monster_idx];

	monster_race *z_ptr = &r_info[r_idx];

	/* Require similar "race" */
	if (z_ptr->d_char != r_ptr->d_char) return (FALSE);

	/* Skip more advanced monsters */
	if (z_ptr->level > r_ptr->level) return (FALSE);

	/* Skip unique monsters */
	if (z_ptr->flags1 & (RF1_UNIQUE)) return (FALSE);

	/* Paranoia -- Skip identical monsters */
	if (place_monster_idx == r_idx) return (FALSE);

	/* Okay */
	return (TRUE);
}


/*
 * Attempt to place a monster of the given race at the given location
 *
 * Note that certain monsters are now marked as requiring "friends".
 * These monsters, if successfully placed, and if the "grp" parameter
 * is TRUE, will be surrounded by a "group" of identical monsters.
 *
 * Note that certain monsters are now marked as requiring an "escort",
 * which is a collection of monsters with similar "race" but lower level.
 *
 * Some monsters induce a fake "group" flag on their escorts.
 *
 * Note the "bizarre" use of non-recursion to prevent annoying output
 * when running a code profiler.
 *
 * Note the use of the new "monster allocation table" code to restrict
 * the "get_mon_num()" function to "legal" escort types.
 */
bool place_monster_aux(int y, int x, int r_idx, bool slp, bool grp)
{
	int i;
	int place;

	monster_race *r_ptr = &r_info[r_idx];


	/* Place one monster, or fail */
	if (!place_monster_one(y, x, r_idx, slp, FALSE)) return (FALSE);


	/* Require the "group" flag */
	if (!grp) return (TRUE);


	/* Friends for certain monsters */
	if ((r_ptr->flags1 & (RF1_FRIENDS)) || (r_ptr->flags1 & (RF1_FRIEND)) || (r_ptr->flags2 & (RF2_FRIEND1)))
	{
		/* Attempt to place a group */
		(void)place_monster_group(y, x, r_idx, slp);
	}


	/* Escorts for certain monsters */
	if ((r_ptr->flags1 & (RF1_ESCORT)) || (r_ptr->flags2 & (RF2_ESCORT1)))
	{
 
        /* how many escorts to try to place */
        if (r_ptr->flags2 & (RF2_ESCORT1)) place = 20;
        else place = 50;
 
		/* Try to place several "escorts" */
		for (i = 0; i < place; i++)
		{
			int nx, ny, z, d = 3;

			/* Pick a location */
			scatter(&ny, &nx, y, x, d, 0);

			/* Require empty grids */
			if (!cave_empty_bold(ny, nx)) continue;


			/* Set the escort index */
			place_monster_idx = r_idx;


			/* Set the escort hook */
			get_mon_num_hook = place_monster_okay;

			/* Prepare allocation table */
			get_mon_num_prep();


			/* Pick a random race */
			z = get_mon_num(r_ptr->level);


			/* Remove restriction */
			get_mon_num_hook = NULL;

			/* Prepare allocation table */
			get_mon_num_prep();


			/* Handle failure */
			if (!z) break;

			/* Place a single escort */
			(void)place_monster_one(ny, nx, z, slp, TRUE);

			/* Place a "group" of escorts if needed */
			if ((r_info[z].flags1 & (RF1_FRIENDS)) ||
			    (r_ptr->flags1 & (RF1_ESCORTS)))
			{
				/* Place a group of monsters */
				(void)place_monster_group(ny, nx, z, slp);
			}
		}
	}


	/* Success */
	return (TRUE);
}


/*
 * Hack -- attempt to place a monster at the given location
 *
 * Attempt to find a monster appropriate to the "monster_level"
 */
bool place_monster(int y, int x, bool slp, bool grp, bool vault)
{
	int r_idx;

#if vaultmonsteroption
	while (1)
	{
		bool forbid = FALSE;
#endif
		/* Pick a monster */
		r_idx = get_mon_num(monster_level);

#if vaultmonsteroption
		/* forbid KILL_WALL monsters in vaults */
		if ((vault) && (adult_no_diggers))
		{
			if (r_ptr->flags2 & (RF2_KILL_WALL)) forbid = TRUE;
		}

		if (!forbid) break;
	}
#endif

	/* Handle failure */
	if (!r_idx) return (FALSE);

	/* Attempt to place the monster */
	if (place_monster_aux(y, x, r_idx, slp, grp)) return (TRUE);

	/* Oops */
	return (FALSE);
}



/*
 * XXX XXX XXX Player Ghosts are such a hack, they have been completely
 * removed.
 *
 * An idea for reintroducing them is to create a small number of
 * "unique" monsters which will serve as the "player ghosts".
 * Each will have a place holder for the "name" of a deceased player,
 * which will be extracted from a "bone" file, or replaced with a
 * "default" name if a real name is not available.  Each ghost will
 * appear exactly once and will not induce a special feeling.
 *
 * Possible methods:
 *   (s) 1 Skeleton
 *   (z) 1 Zombie
 *   (M) 1 Mummy
 *   (G) 1 Polterguiest, 1 Spirit, 1 Ghost, 1 Shadow, 1 Phantom
 *   (W) 1 Wraith
 *   (V) 1 Vampire, 1 Vampire Lord
 *   (L) 1 Lich
 *
 * Possible change: Lose 1 ghost, Add "Master Lich"
 *
 * Possible change: Lose 2 ghosts, Add "Wraith", Add "Master Lich"
 *
 * Possible change: Lose 4 ghosts, lose 1 vampire lord
 *
 * Note that ghosts should never sleep, should be very attentive, should
 * have maximal hitpoints, drop only good (or great) items, should be
 * cold blooded, evil, undead, immune to poison, sleep, confusion, fear.
 *
 * Base monsters:
 *   Skeleton
 *   Zombie
 *   Mummy
 *   Poltergeist
 *   Spirit
 *   Ghost
 *   Vampire
 *   Wraith
 *   Vampire Lord
 *   Shadow
 *   Phantom
 *   Lich
 *
 * This routine will simply extract ghost names from files, and
 * attempt to allocate a player ghost somewhere in the dungeon,
 * note that normal allocation may also attempt to place ghosts,
 * so we must work with some form of default names.
 *
 * XXX XXX XXX
 */




/*
 * Attempt to allocate a random monster in the dungeon.
 *
 * Place the monster at least "dis" distance from the player.
 *
 * Use "slp" to choose the initial "sleep" status
 *
 * Use "monster_level" for the monster level
 */
bool alloc_monster(int dis, bool slp)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int y, x;
	int	attempts_left = 10000;

	/* Find a legal, distant, unoccupied, space */
	while (--attempts_left)
	{
		/* Pick a location */
		y = rand_int(DUNGEON_HGT);
		x = rand_int(DUNGEON_WID);

		/* Require "naked" floor grid */
		if (!cave_naked_bold(y, x)) continue;

		/* Accept far away grids */
		if (distance(y, x, py, px) > dis) break;
	}

	if (!attempts_left)
	{
		if (cheat_xtra || cheat_hear)
		{
			msg_print("Warning! Could not allocate a new monster.");
		}

		return FALSE;
	}

	/* Attempt to place the monster, allow groups */
	if (place_monster(y, x, slp, TRUE, FALSE)) return (TRUE);

	/* Nope */
	return (FALSE);
}




/*
 * Hack -- the "type" of the current "summon specific"
 */
static int summon_specific_type = 0;


/*
 * Hack -- help decide if a monster race is "okay" to summon
 */
static bool summon_specific_okay(int r_idx)
{
	monster_race *r_ptr = &r_info[r_idx];

	bool okay = FALSE;


	/* Hack -- no specific type specified */
	if (!summon_specific_type) return (TRUE);


	/* Check our requirements */
	switch (summon_specific_type)
	{
		case SUMMON_ANIMAL:
		{
			okay = ((r_ptr->flags3 & (RF3_ANIMAL)) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_SPIDER:
		{
			okay = ((r_ptr->d_char == 'S') &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_HOUND:
		{
			okay = (((r_ptr->d_char == 'C') || (r_ptr->d_char == 'Z')) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_HYDRA:
		{
			okay = ((r_ptr->d_char == 'M') &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_ANGEL: /* (ape) */
		{
			okay = ((r_ptr->d_char == 'A') &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_DEMON:
		{
			okay = ((r_ptr->flags3 & (RF3_DEMON)) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_UNDEAD: 
		{
			okay = ((r_ptr->flags3 & (RF3_UNDEAD)) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_DRAGON:
		{
			okay = ((r_ptr->flags3 & (RF3_DRAGON)) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_SILVER:
		{
			okay = ((r_ptr->flags3 & (RF3_SILVER)) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_KIN:
		{
			okay = ((r_ptr->d_char == summon_kin_type) &&
			        !(r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_HI_UNDEAD:
		{
			okay = ((r_ptr->d_char == 'L') ||
			        (r_ptr->d_char == 'V') ||
			        (r_ptr->d_char == 'W'));
			break;
		}

		case SUMMON_HI_DRAGON:
		{
			okay = (r_ptr->d_char == 'D');
			break;
		}

		case SUMMON_HI_DEMON:
		{
			okay = ((r_ptr->d_char == 'U') ||
			        (r_ptr->d_char == '&'));
			break;
		}

		case SUMMON_WRAITH:
		{
			okay = ((r_ptr->d_char == 'W') &&
			        (r_ptr->flags1 & (RF1_UNIQUE)));
			break;
		}

		case SUMMON_UNIQUE:
		{
			okay = (r_ptr->flags1 & (RF1_UNIQUE)) ? TRUE : FALSE;
			break;
		}
	}

	/* Result */
	return (okay);
}


/*
 * Place a monster (of the specified "type") near the given
 * location.  Return TRUE if a monster was actually summoned.
 *
 * We will attempt to place the monster up to 10 times before giving up.
 *
 * Note: SUMMON_UNIQUE and SUMMON_WRAITH (XXX) will summon Uniques
 * Note: SUMMON_HI_UNDEAD and SUMMON_HI_DRAGON may summon Uniques
 * Note: None of the other summon codes will ever summon Uniques.
 *
 * This function has been changed.  We now take the "monster level"
 * of the summoning monster as a parameter, and use that, along with
 * the current dungeon level, to help determine the level of the
 * desired monster.  Note that this is an upper bound, and also
 * tends to "prefer" monsters of that level.  Currently, we use
 * the average of the dungeon and monster levels, and then add
 * five to allow slight increases in monster power.
 *
 * Note that we use the new "monster allocation table" creation code
 * to restrict the "get_mon_num()" function to the set of "legal"
 * monsters, making this function much faster and more reliable.
 *
 * Note that this function may not succeed, though this is very rare.
 */
bool summon_specific(int y1, int x1, int lev, int type)
{
	int i, x, y, r_idx;


	/* Look for a location */
	for (i = 0; i < 20; ++i)
	{
		/* Pick a distance */
		int d = (i / 15) + 1;

		/* Pick a location */
		scatter(&y, &x, y1, x1, d, 0);

		/* Require "empty" floor grid */
		if (!cave_empty_bold(y, x)) continue;

		/* Hack -- no summon on glyph of warding */
		if (cave_feat[y][x] == FEAT_GLYPH) continue;

		/* Okay */
		break;
	}

	/* Failure */
	if (i == 20) return (FALSE);


	/* Save the "summon" type */
	summon_specific_type = type;


	/* Require "okay" monsters */
	get_mon_num_hook = summon_specific_okay;

	/* Prepare allocation table */
	get_mon_num_prep();


	/* Pick a monster, using the level calculation */
	r_idx = get_mon_num((p_ptr->depth + lev) / 2 + 5);


	/* Remove restriction */
	get_mon_num_hook = NULL;

	/* Prepare allocation table */
	get_mon_num_prep();


	/* Handle failure */
	if (!r_idx) return (FALSE);

	/* Attempt to place the monster (awake, allow groups) */
	if (!place_monster_aux(y, x, r_idx, FALSE, TRUE)) return (FALSE);

	/* Success */
	return (TRUE);
}





/*
 * Let the given monster attempt to reproduce.
 *
 * Note that "reproduction" REQUIRES empty space.
 */
bool multiply_monster(int m_idx)
{
	monster_type *m_ptr = &mon_list[m_idx];

	int i, y, x;

	bool result = FALSE;

	/* Try up to 18 times */
	for (i = 0; i < 18; i++)
	{
		int d = 1;

		/* Pick a location */
		scatter(&y, &x, m_ptr->fy, m_ptr->fx, d, 0);

		/* Require an "empty" floor grid */
		if (!cave_empty_bold(y, x)) continue;

		/* Create a new monster (awake, no groups) */
		result = place_monster_aux(y, x, m_ptr->r_idx, FALSE, FALSE);

		/* Done */
		break;
	}

	/* Result */
	return (result);
}





/*
 * Dump a message describing a monster's reaction to damage
 *
 * Technically should attempt to treat "Beholder"'s as jelly's
 */
void message_pain(int m_idx, int dam)
{
	long oldhp, newhp, tmp;
	int percentage;

	monster_type *m_ptr = &mon_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	char m_name[80];


	/* Get the monster name */
	monster_desc(m_name, sizeof(m_name), m_ptr, 0);

	/* Notice non-damage */
	if (dam == 0)
	{
		msg_format("%^s is unharmed.", m_name);
		return;
	}

	/* Note -- subtle fix -CFT */
	newhp = (long)(m_ptr->hp);
	oldhp = newhp + (long)(dam);
	tmp = (newhp * 100L) / oldhp;
	percentage = (int)(tmp);


	/* Jelly's, Mold's, Vortex's, Quthl's */
	if (strchr("jmvQJwe,", r_ptr->d_char))
	{
		if (percentage > 95)
			msg_format("%^s barely notices.", m_name);
		else if (percentage > 75)
			msg_format("%^s flinches.", m_name);
		else if (percentage > 50)
			msg_format("%^s squelches.", m_name);
		else if (percentage > 35)
			msg_format("%^s quivers in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s writhes about.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s jerks limply.", m_name);
	}

	/* Dogs and Hounds */
	else if (strchr("CZ", r_ptr->d_char))
	{
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s snarls with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s yelps in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s howls in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s howls in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s yelps feebly.", m_name);
	}
    
   	/* monsters which don't feel pain (ignores) */
	else if (strchr("szgW", r_ptr->d_char))
	{
		if (percentage > 95)
			msg_format("%^s ignores the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s is slightly damaged.", m_name);
		else if (percentage > 50)
			msg_format("%^s is damaged.", m_name);
		else if (percentage > 35)
			msg_format("%^s is severely damaged.", m_name);
		else if (percentage > 20)
			msg_format("%^s has deep gashes but doesn't notice.", m_name);
		else if (percentage > 10)
			msg_format("%^s ignores the loss of its limbs.", m_name);
		else
			msg_format("%^s is literally falling apart.", m_name);
	}

	/* One type of monsters (ignore,squeal,shriek) */
	else if (strchr("FIMRSXabclqrt", r_ptr->d_char))
	{
		if (percentage > 95)
			msg_format("%^s ignores the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s squeals in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s shrieks in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s shrieks in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
	}

	/* Another type of monsters (shrug,cry,scream) */
	else
	{
		if (percentage > 95)
			msg_format("%^s shrugs off the attack.", m_name);
		else if (percentage > 75)
			msg_format("%^s grunts with pain.", m_name);
		else if (percentage > 50)
			msg_format("%^s cries out in pain.", m_name);
		else if (percentage > 35)
			msg_format("%^s screams in pain.", m_name);
		else if (percentage > 20)
			msg_format("%^s screams in agony.", m_name);
		else if (percentage > 10)
			msg_format("%^s writhes in agony.", m_name);
		else
			msg_format("%^s cries out feebly.", m_name);
	}
}



/*
 * Learn about an "observed" resistance.
 */
void update_smart_learn(int m_idx, int what)
{

#ifdef DRS_SMART_OPTIONS

	monster_type *m_ptr = &mon_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];


	/* Not allowed to learn */
	if (!adult_ai_learn) return;

	/* Too stupid to learn anything */
	if (r_ptr->flags2 & (RF2_STUPID)) return;

	/* Not intelligent, only learn sometimes */
	if (!(r_ptr->flags2 & (RF2_SMART)) && (rand_int(100) < 50)) return;


	/* XXX XXX XXX */

	/* Analyze the knowledge */
	switch (what)
	{
		case DRS_FREE:
		{
			if (p_ptr->free_act) m_ptr->smart |= (SM_IMM_FREE);
			break;
		}

		case DRS_MANA:
		{
			if (!p_ptr->msp) m_ptr->smart |= (SM_IMM_MANA);
			break;
		}

		case DRS_RES_ACID:
		{
			if (p_ptr->resist_acid) m_ptr->smart |= (SM_RES_ACID);
			if (p_ptr->timed[TMD_OPP_ACID]) m_ptr->smart |= (SM_OPP_ACID);
			if (p_ptr->immune_acid) m_ptr->smart |= (SM_IMM_ACID);
			break;
		}

		case DRS_RES_ELEC:
		{
			if (p_ptr->resist_elec) m_ptr->smart |= (SM_RES_ELEC);
			if (p_ptr->timed[TMD_OPP_ELEC]) m_ptr->smart |= (SM_OPP_ELEC);
			if (p_ptr->immune_elec) m_ptr->smart |= (SM_IMM_ELEC);
			break;
		}

		case DRS_RES_FIRE:
		{
			if (p_ptr->resist_fire) m_ptr->smart |= (SM_RES_FIRE);
			if (p_ptr->timed[TMD_OPP_FIRE]) m_ptr->smart |= (SM_OPP_FIRE);
			if (p_ptr->immune_fire) m_ptr->smart |= (SM_IMM_FIRE);
			break;
		}

		case DRS_RES_COLD:
		{
			if (p_ptr->resist_cold) m_ptr->smart |= (SM_RES_COLD);
			if (p_ptr->timed[TMD_OPP_COLD]) m_ptr->smart |= (SM_OPP_COLD);
			if (p_ptr->immune_cold) m_ptr->smart |= (SM_IMM_COLD);
			break;
		}

		case DRS_RES_POIS:
		{
			if (p_ptr->resist_pois) m_ptr->smart |= (SM_RES_POIS);
			if (p_ptr->timed[TMD_OPP_POIS]) m_ptr->smart |= (SM_OPP_POIS);
			break;
		}

		case DRS_RES_FEAR:
		{
			if (p_ptr->resist_fear) m_ptr->smart |= (SM_RES_FEAR);
			break;
		}

		case DRS_RES_CHARM:
		{
		    /* if (p_ptr->resist_charm) m_ptr->smart |= (SM_RES_CHARM); */
			break;
		}
		
		case DRS_RES_LITE:
		{
			if (p_ptr->resist_lite) m_ptr->smart |= (SM_RES_LITE);
			break;
		}

		case DRS_RES_DARK:
		{
			if (p_ptr->resist_dark) m_ptr->smart |= (SM_RES_DARK);
			break;
		}

		case DRS_RES_BLIND:
		{
			if (p_ptr->resist_blind) m_ptr->smart |= (SM_RES_BLIND);
			break;
		}

		case DRS_RES_CONFU:
		{
			if (p_ptr->resist_confu) m_ptr->smart |= (SM_RES_CONFU);
			break;
		}

		case DRS_RES_SOUND:
		{
			if (p_ptr->resist_sound) m_ptr->smart |= (SM_RES_SOUND);
			break;
		}

		case DRS_RES_SHARD:
		{
			if (p_ptr->resist_shard) m_ptr->smart |= (SM_RES_SHARD);
			break;
		}

		case DRS_RES_NEXUS:
		{
			if (p_ptr->resist_nexus) m_ptr->smart |= (SM_RES_NEXUS);
			break;
		}

		case DRS_RES_NETHR:
		{
			if (p_ptr->resist_nethr) m_ptr->smart |= (SM_RES_NETHR);
			break;
		}

		case DRS_RES_CHAOS:
		{
			if (p_ptr->resist_chaos) m_ptr->smart |= (SM_RES_CHAOS);
			break;
		}

		case DRS_RES_DISEN:
		{
			if (p_ptr->resist_disen) m_ptr->smart |= (SM_RES_DISEN);
			break;
		}
	}

#endif /* DRS_SMART_OPTIONS */

}


