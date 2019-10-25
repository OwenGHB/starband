/* File: object2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"


/*
 * Excise a dungeon object from any stacks
 */
void excise_object_idx(int o_idx)
{
	object_type *j_ptr;

	s16b this_o_idx, next_o_idx = 0;

	s16b prev_o_idx = 0;


	/* Object */
	j_ptr = &o_list[o_idx];

	/* Monster */
	if (j_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Monster */
		m_ptr = &m_list[j_ptr->held_m_idx];

		/* Scan all objects in the grid */
		for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Get the object */
			o_ptr = &o_list[this_o_idx];

			/* Get the next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					m_ptr->hold_o_idx = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *i_ptr;

					/* Previous object */
					i_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					i_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}

	/* Dungeon */
	else
	{
		int y = j_ptr->iy;
		int x = j_ptr->ix;

		/* Scan all objects in the grid */
		for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Get the object */
			o_ptr = &o_list[this_o_idx];

			/* Get the next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Done */
			if (this_o_idx == o_idx)
			{
				/* No previous */
				if (prev_o_idx == 0)
				{
					/* Remove from list */
					cave_o_idx[y][x] = next_o_idx;
				}

				/* Real previous */
				else
				{
					object_type *i_ptr;

					/* Previous object */
					i_ptr = &o_list[prev_o_idx];

					/* Remove from list */
					i_ptr->next_o_idx = next_o_idx;
				}

				/* Forget next pointer */
				o_ptr->next_o_idx = 0;

				/* Done */
				break;
			}

			/* Save prev_o_idx */
			prev_o_idx = this_o_idx;
		}
	}
}


/*
 * Delete a dungeon object
 *
 * Handle "stacks" of objects correctly.
 */
void delete_object_idx(int o_idx)
{
	object_type *j_ptr;

	/* Excise */
	excise_object_idx(o_idx);

	/* Object */
	j_ptr = &o_list[o_idx];

	/* Dungeon floor */
	if (!(j_ptr->held_m_idx))
	{
		int y, x;

		/* Location */
		y = j_ptr->iy;
		x = j_ptr->ix;

		/* Visual update */
		lite_spot(y, x);
	}

	/* Wipe the object */
	object_wipe(j_ptr);

	/* Count objects */
	o_cnt--;
}


/*
 * Deletes all objects at given location
 */
void delete_object(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Paranoia */
	if (!in_bounds(y, x)) return;


	/* Scan all objects in the grid */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Wipe the object */
		object_wipe(o_ptr);

		/* Count objects */
		o_cnt--;
	}

	/* Objects are gone */
	cave_o_idx[y][x] = 0;

	/* Visual update */
	lite_spot(y, x);
}



/*
 * Move an object from index i1 to index i2 in the object list
 */
static void compact_objects_aux(int i1, int i2)
{
	int i;

	object_type *o_ptr;


	/* Do nothing */
	if (i1 == i2) return;


	/* Repair objects */
	for (i = 1; i < o_max; i++)
	{
		/* Get the object */
		o_ptr = &o_list[i];

		/* Skip "dead" objects */
		if (!o_ptr->k_idx) continue;

		/* Repair "next" pointers */
		if (o_ptr->next_o_idx == i1)
		{
			/* Repair */
			o_ptr->next_o_idx = i2;
		}
	}


	/* Get the object */
	o_ptr = &o_list[i1];


	/* Monster */
	if (o_ptr->held_m_idx)
	{
		monster_type *m_ptr;

		/* Get the monster */
		m_ptr = &m_list[o_ptr->held_m_idx];

		/* Repair monster */
		if (m_ptr->hold_o_idx == i1)
		{
			/* Repair */
			m_ptr->hold_o_idx = i2;
		}
	}

	/* Dungeon */
	else
	{
		int y, x;

		/* Get location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Repair grid */
		if (cave_o_idx[y][x] == i1)
		{
			/* Repair */
			cave_o_idx[y][x] = i2;
		}
	}


	/* Hack -- move object */
	COPY(&o_list[i2], &o_list[i1], object_type);

	/* Hack -- wipe hole */
	object_wipe(o_ptr);
}


/*
 * Compact and Reorder the object list
 *
 * This function can be very dangerous, use with caution!
 *
 * When actually "compacting" objects, we base the saving throw on a
 * combination of object level, distance from player, and current
 * "desperation".
 *
 * After "compacting" (if needed), we "reorder" the objects into a more
 * compact order, and we reset the allocation info, and the "live" array.
 *
 * Should possibly rewrite this to match sang implimention,
 * but most likely will not need to due to the smaller number of 
 * objects.
 */
void compact_objects(int size)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, y, x, num, cnt;

	int cur_lev, cur_dis, chance;

	/* Paranoia -- refuse to wipe too many objects at one time */
	if (size > z_info->o_max / 2) size = z_info->o_max / 2;

	/* Compact */
	if (size)
	{
		/* Message */
		msg_print("Compacting objects...");

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Window stuff */
		p_ptr->window |= (PW_OVERHEAD);
	}


	/* Compact at least 'size' objects */
	for (num = 0, cnt = 1; num < size; cnt++)
	{
		/* Get more vicious each iteration */
		cur_lev = 5 * cnt;

		/* Get closer each iteration */
		cur_dis = 5 * (20 - cnt);

		/* Examine the objects */
		for (i = 1; i < o_max; i++)
		{
			object_type *o_ptr = &o_list[i];

			object_kind *k_ptr = &k_info[o_ptr->k_idx];

			/* Skip dead objects */
			if (!o_ptr->k_idx) continue;

			/* Hack -- High level objects start out "immune" */
			if (k_ptr->level > cur_lev) continue;

			/* Monster */
			if (o_ptr->held_m_idx)
			{
				monster_type *m_ptr;

				/* Get the monster */
				m_ptr = &m_list[o_ptr->held_m_idx];

				/* Get the location */
				y = m_ptr->fy;
				x = m_ptr->fx;

				/* Monsters protect their objects */
				if (rand_int(100) < 90) continue;
			}

			/* Dungeon */
			else
			{
				/* Get the location */
				y = o_ptr->iy;
				x = o_ptr->ix;
			}

			/* Nearby objects start out "immune" */
			if ((cur_dis > 0) && (distance(py, px, y, x) < cur_dis)) continue;

			/* Saving throw */
			chance = 90;

			/* Hack -- only compact artifacts in emergencies */
			if (artifact_p(o_ptr) && (cnt < 1000)) chance = 100;

			/* Apply the saving throw */
			if (rand_int(100) < chance) continue;

			/* Delete the object */
			delete_object_idx(i);

			/* Count it */
			num++;
		}
	}


	/* Excise dead objects (backwards!) */
	for (i = o_max - 1; i >= 1; i--)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip real objects */
		if (o_ptr->k_idx) continue;

		/* Move last object into open hole */
		compact_objects_aux(o_max - 1, i);

		/* Compress "o_max" */
		o_max--;
	}
}




/*
 * Delete all the items when player leaves the level
 *
 * Note -- we do NOT visually reflect these (irrelevant) changes
 *
 * Hack -- we clear the "cave_o_idx[y][x]" field for every grid,
 * and the "m_ptr->next_o_idx" field for every monster, since
 * we know we are clearing every object.  Technically, we only
 * clear those fields for grids/monsters containing objects,
 * and we clear it once for every such object.
 */
void wipe_o_list(void)
{
	int i;

	/* Delete the existing objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Mega-Hack -- preserve artifacts */
		if (!character_dungeon || adult_preserve)
		{
			/* Hack -- Preserve unknown artifacts */
			if (artifact_p(o_ptr) && !object_known_p(o_ptr))
			{
				/* Mega-Hack -- Preserve the artifact */
				a_info[o_ptr->name1].cur_num = 0;
			}
		}

		/* Monster */
		if (o_ptr->held_m_idx)
		{
			monster_type *m_ptr;

			/* Monster */
			m_ptr = &m_list[o_ptr->held_m_idx];

			/* Hack -- see above */
			m_ptr->hold_o_idx = 0;
		}

		/* Dungeon */
		else
		{
			/* Get the location */
			int y = o_ptr->iy;
			int x = o_ptr->ix;

			/* Hack -- see above */
			cave_o_idx[y][x] = 0;
		}

		/* Wipe the object */
		(void)WIPE(o_ptr, object_type);
	}

	/* Reset "o_max" */
	o_max = 1;

	/* Reset "o_cnt" */
	o_cnt = 0;
}


/*
 * Get and return the index of a "free" object.
 *
 * This routine should almost never fail, but in case it does,
 * we must be sure to handle "failure" of this routine.
 */
s16b o_pop(void)
{
	int i;


	/* Initial allocation */
	if (o_max < z_info->o_max)
	{
		/* Get next space */
		i = o_max;

		/* Expand object array */
		o_max++;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Recycle dead objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[i];

		/* Skip live objects */
		if (o_ptr->k_idx) continue;

		/* Count objects */
		o_cnt++;

		/* Use this object */
		return (i);
	}


	/* Warn the player (except during dungeon creation) */
	if (character_dungeon) msg_print("Too many objects!");

	/* Oops */
	return (0);
}


/*
 * Apply a "object restriction function" to the "object allocation table"
 *
 * (code stolen from Sangband 1.0.0 note)
 * In code stolen from Sangband, applying an object allocation restriction 
 * is a matter of toggling permissions for individual allocations,
 * which are ordered not by level, but by object index.
 * 
 */
errr get_obj_num_prep(void)
{
	int i;

	/* Clear restrictions */
	if (get_obj_num_hook == NULL)
	{
		if ((p_ptr->prace == RACE_AUTOMATA) || (p_ptr->prace == RACE_STEAM_MECHA))
		{
			/* Accept all objects */
			for (i = 0; i < z_info->k_max; i++) permit_kind_table[i] = TRUE;
		}
		else 
		{
			for (i = 0; i < z_info->k_max; i++)
			{
				object_kind *k_ptr = &k_info[i];
	        	if (k_ptr->flags3 & (TR3_MECHA_GEN)) permit_kind_table[i] = FALSE;
				else permit_kind_table[i] = TRUE;
			}
		}
	}

	/* Apply restrictions */
	else
	{
		/* Scan all objects */
		for (i = 0; i < z_info->k_max; i++)
		{
			/* Apply the restriction */
			permit_kind_table[i] = (*get_obj_num_hook)(i);
		}
	}

	/* Success */
	return (0);
}



/*
 * Choose an object kind that seems "appropriate" to the given level
 *
 * This function calculates level-dependant probabilities for all allowed
 * objects, sums them up, and chooses among them.
 *
 * If no objects match the restrictions laid down, this function will
 * fail, and return zero, but this should *almost* never happen.
 *
 * This is very slow code.  XXX XXX
 */
s16b get_obj_num(int level)
{
	int i, j, z;
	int lev1, lev2, chance1, chance2;

	object_kind *k_ptr;

	s32b value;
	bool quick = FALSE;


	/* Sometimes boost object level */
	if ((level > 0) && (one_in_(GREAT_OBJ)))
	{
		/* Boost on 20th level can (rarely) be 16 + 4 * 20 / 3, or 42 */
		int boost = ABS(Rand_normal(0, 4 + level / 3));

		/* Boost the depth */
		level += boost;
	}

	/* Restrict level */
	if (level > MAX_DEPTH) level = MAX_DEPTH;

	/* We are using the same generation level as last time */
	if ((level == old_object_level) && (required_tval == old_required_tval))
	{
		/* We are using the same generation restrictions as last time */
		if (get_obj_num_hook == old_get_obj_num_hook)
		{
			/* There is no need to rebuild the generation table */
			if (alloc_kind_total) quick = TRUE;
		}
	}

	/* We are not using quick generation */
	if (!quick)
	{
		/* Remember the generation level we are using */
		old_object_level = level;

		/* Remember the restrictions we are using */
		old_get_obj_num_hook = get_obj_num_hook;

		/* Remember the object restriction we are using */
		old_required_tval = required_tval;

		/* Clear generation total */
		alloc_kind_total = 0L;


		/* Scan all objects */
		for (i = 0; i < z_info->k_max; i++)
		{
			/* Assume that object cannot be made */
			chance_kind_table[i] = 0;

			/* Object is forbidden */
			if (!permit_kind_table[i]) continue;

			/* Clear some variables */
			lev1 = -1; lev2 = MAX_DEPTH + 1; chance1 = 0; chance2 = 0;

			/* Get the object index */
			k_ptr = &k_info[i];

			/* Scan allocation pairs */
			for (j = 0; j < 3; j++)
			{
				/* Require a non-empty allocation, check permissions */
				if (k_ptr->chance[j])
				{
					/* Look for the closest allocation <= than the current depth */
					if ((k_ptr->locale[j] <= level) && (lev1 <= k_ptr->locale[j]))
					{
						lev1    = k_ptr->locale[j];
						chance1 = k_ptr->chance[j];
					}

					/* Look for the closest allocation > than the current depth */
					else if ((k_ptr->locale[j] > level) && (lev2 > k_ptr->locale[j]))
					{
						lev2    = k_ptr->locale[j];
						chance2 = k_ptr->chance[j];
					}
				}
			}

			/* Simple case - object is too high-level */
			if (lev1 < 0)
			{
				continue;
			}

			/* Simple case - no allocations exceed the current depth */
			else if (lev2 == MAX_DEPTH + 1)
			{
				chance_kind_table[i] = chance1;
			}

			/* Simple case - current depth matches an allocation entry */
			else if ((lev1 == level) || (lev2 == lev1))
			{
				chance_kind_table[i] = chance1;
			}

			/* Usual case - apply the weighted average of two allocations */
			else
			{
				z = chance1 + (level - lev1) * (chance2 - chance1) / (lev2 - lev1);
				chance_kind_table[i] = (byte)z;
			}

			/* Sum up allocation chances */
			alloc_kind_total += chance_kind_table[i];
		}
	}

	/* No legal objects */
	if (alloc_kind_total <= 0L) return (0);

	/* Pick an object */
	value = rand_int(alloc_kind_total);

	/* Find the object */
	for (i = 0; i < z_info->k_max; i++)
	{
		/* Found the entry */
		if (value < chance_kind_table[i]) break;

		/* Decrement */
		value -= chance_kind_table[i];
	}

	/* Return the object index */
	return (i);
}






/*
 * Known is true when the "attributes" of an object are "known".
 *
 * These attributes include tohit, todam, toac, cost, and pval (charges).
 *
 * Note that "knowing" an object gives you everything that an "awareness"
 * gives you, and much more.  In fact, the player is always "aware" of any
 * item of which he has full "knowledge".
 *
 * But having full knowledge of, say, one "wand of wonder", does not, by
 * itself, give you knowledge, or even awareness, of other "wands of wonder".
 * It happens that most "identify" routines (including "buying from a shop")
 * will make the player "aware" of the object as well as fully "know" it.
 *
 * This routine also removes any inscriptions generated by "feelings".
 */
void object_known(object_type *o_ptr)
{
	/* Remove special inscription, if any */
	if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

	/* The object is not "sensed" */
	o_ptr->ident &= ~(IDENT_SENSE);

	/* Clear the "Empty" info */
	o_ptr->ident &= ~(IDENT_EMPTY);

	/* Now we know about the item */
	o_ptr->ident |= (IDENT_KNOWN);
}





/*
 * The player is now aware of the effects of the given object.
 */
void object_aware(object_type *o_ptr)
{
	/* Fully aware of the effects */
	k_info[o_ptr->k_idx].aware = TRUE;
}


/*
 * Add the "mental" flag to an object, then apply "object_known()".
 */
void object_mental(object_type *o_ptr)
{
	/* Now we know the item completely */
	o_ptr->ident |= (IDENT_MENTAL);

	/* Apply normal knowledge */
	object_known(o_ptr);
}

/*
 * Something has been "sampled"
 */
void object_tried(object_type *o_ptr)
{
	/* Mark it as tried (even if "aware") */
	k_info[o_ptr->k_idx].tried = TRUE;
}


/*
 * Return the "value" of an "unknown" item
 * Make a guess at the value of non-aware items
 */
static s32b object_value_base(const object_type *o_ptr)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];

	/* Use template cost for aware objects */
	if (object_aware_p(o_ptr)) return (k_ptr->cost);

	/* Analyze the type */
	switch (o_ptr->tval)
	{
		/* Un-aware Food */
		case TV_FOOD: return (5L);

		/* Un-aware tonics */
		case TV_TONIC: return (20L);

		/* Un-aware Scrolls */
		case TV_MECHANISM: return (20L);

		/* Un-aware tools */
		case TV_TOOL: return (70L);

		/* Un-aware Wands */
		case TV_RAY: return (50L);

		/* Un-aware Rods */
		case TV_APPARATUS: return (90L);

		/* Un-aware Rings */
		case TV_RING: return (45L);

		/* Un-aware Amulets */
		case TV_AMULET: return (45L);
	}

	/* Paranoia -- Oops */
	return (0L);
}


/*
 * Return the "real" price of a "known" item, not including discounts.
 *
 * Wand and tools get cost for each charge.
 *
 * Armor is worth an extra 100 gold per bonus point to armor class.
 *
 * Weapons are worth an extra 100 gold per bonus point (AC,TH,TD).
 *
 * Missiles are only worth 5 gold per bonus point, since they
 * usually appear in groups of 20, and we want the player to get
 * the same amount of cash for any "equivalent" item.  Note that
 * missiles never have any of the "pval" flags, and in fact, they
 * only have a few of the available flags, primarily of the "slay"
 * and "brand" and "ignore" variety.
 *
 * Weapons with negative hit+damage bonuses are worthless.
 *
 * Every wearable item with a "pval" bonus is worth extra (see below).
 */
static s32b object_value_real(const object_type *o_ptr)
{
	s32b value;

	u32b f1, f2, f3;

	object_kind *k_ptr = &k_info[o_ptr->k_idx];



	/* Base cost */
	value = k_ptr->cost;


	/* Extract some flags */
	object_flags(o_ptr, &f1, &f2, &f3);


	/* Artifact */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- "worthless" artifacts */
		if (!a_ptr->cost) return (0L);

		/* Hack -- Use the artifact cost instead */
		value = a_ptr->cost;
	}

	/* Ego-Item */
	else if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Hack -- "worthless" ego-items */
		if (!e_ptr->cost) return (0L);

		/* Hack -- Reward the ego-item with a bonus */
		value += e_ptr->cost;
	}

	/* Hack -- "worthless" items */
	if ((!k_ptr->cost) && (!(o_ptr->name1))) return (0L);

	/* Analyze pval bonus */
	switch (o_ptr->tval)
	{
		case TV_AMMO:
		case TV_BULLET:
		case TV_SHOT:
		case TV_GUN:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		case TV_LEG:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		/* case TV_LITE: */
		case TV_AMULET:
		case TV_RING:
		{
			/* Hack -- Negative "pval" is always bad */
			/* if (o_ptr->pval < 0) return (0L); */

			/* No pval */
			if (!o_ptr->pval) break;

			/* Give credit for stat bonuses */
			value += (get_object_pval(o_ptr, TR_PVAL_MUS) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_AGI) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_VIG) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_SCH) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_EGO) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_CHR) * 20L);

			/* Give credit for stealth and searching */
			value += (get_object_pval(o_ptr, TR_PVAL_STEALTH) * 40L);
			value += (get_object_pval(o_ptr, TR_PVAL_SEARCH) * 40L);

			/* Give credit for infra-vision and tunneling */
			value += (get_object_pval(o_ptr, TR_PVAL_INFRA) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_TUNNEL) * 20L);

			/* Give credit for extra attacks */
			value += (get_object_pval(o_ptr, TR_PVAL_BLOWS)  * 2000L);

			/* Give credit for speed bonus */
			value += (get_object_pval(o_ptr, TR_PVAL_SPEED)  * 10000L);
			value += (get_object_pval(o_ptr, TR_PVAL_MANA)  * 4000L);
			value += (get_object_pval(o_ptr, TR_PVAL_HEALTH)  * 4000L);
			value += (get_object_pval(o_ptr, TR_PVAL_LIGHT)  * 2000L);
			value += (get_object_pval(o_ptr, TR_PVAL_SAVE)  * 400L);
			value += (get_object_pval(o_ptr, TR_PVAL_MAGIC_MASTER)  * 6000L);
			break;
		}
		case TV_MECHA_TORSO:
		case TV_MECHA_HEAD:
		case TV_MECHA_ARMS:
		case TV_MECHA_FEET:
		{
			/* Hack -- Negative "pval" is always bad */
			/* if (o_ptr->pval < 0) return (0L); */

			/* No pval */
			if (!o_ptr->pval) break;

			/* Give credit for stat bonuses */
			value += (get_object_pval(o_ptr, TR_PVAL_MUS) * 5L);
			value += (get_object_pval(o_ptr, TR_PVAL_AGI) * 5L);
			value += (get_object_pval(o_ptr, TR_PVAL_VIG) * 5L);
			value += (get_object_pval(o_ptr, TR_PVAL_SCH) * 5L);
			value += (get_object_pval(o_ptr, TR_PVAL_EGO) * 5L);
			value += (get_object_pval(o_ptr, TR_PVAL_CHR) * 5L);

			/* Give credit for stealth and searching */
			value += (get_object_pval(o_ptr, TR_PVAL_STEALTH) * 40L);
			value += (get_object_pval(o_ptr, TR_PVAL_SEARCH) * 40L);

			/* Give credit for infra-vision and tunneling */
			value += (get_object_pval(o_ptr, TR_PVAL_INFRA) * 20L);
			value += (get_object_pval(o_ptr, TR_PVAL_TUNNEL) * 20L);

			/* Give credit for extra attacks */
			value += (get_object_pval(o_ptr, TR_PVAL_BLOWS)  * 2000L);

			/* Give credit for speed bonus */
			value += (get_object_pval(o_ptr, TR_PVAL_SPEED)  * 10000L);
			value += (get_object_pval(o_ptr, TR_PVAL_MANA)  * 4000L);
			value += (get_object_pval(o_ptr, TR_PVAL_HEALTH)  * 4000L);
			value += (get_object_pval(o_ptr, TR_PVAL_LIGHT)  * 2000L);
			value += (get_object_pval(o_ptr, TR_PVAL_SAVE)  * 4000L);
			value += (get_object_pval(o_ptr, TR_PVAL_MAGIC_MASTER)  * 6000L);

			break;
		}
	}


	/* Analyze the item */
	switch (o_ptr->tval)
	{
		/* Wands/tools */
		case TV_RAY:
		{
			/* Pay extra for charges */
			value += ((value / 20) * o_ptr->pval / o_ptr->number);

			/* Done */
			break;
		}
		case TV_TOOL:
		{
			/* Pay extra for charges */
			value += ((value / 20) * o_ptr->pval);// / o_ptr->number);

			/* Done */
			break;
		}

		/* Rings/Amulets */
		case TV_RING:
		case TV_AMULET:
		{
			/* Hack -- negative bonuses are bad */
			if (o_ptr->to_a < 0) return (0L);
			if (o_ptr->to_h < 0) return (0L);
			if (o_ptr->to_d < 0) return (0L);

			/* Give credit for bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 100L);

			/* Done */
			break;
		}

		/* Armor */
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_CLOAK:
		case TV_CROWN:
		case TV_HELM:
		case TV_LEG:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_MECHA_TORSO:
		case TV_MECHA_HEAD:
		case TV_MECHA_ARMS:
		case TV_MECHA_FEET:

		{
			/* Give credit for hit bonus */
			value += ((o_ptr->to_h - k_ptr->to_h) * 100L);

			/* Give credit for damage bonus */
			value += ((o_ptr->to_d - k_ptr->to_d) * 100L);

			/* Give credit for armor bonus */
			value += (o_ptr->to_a * 100L);

			/* Done */
			break;
		}

		/* Guns/Weapons */
		case TV_GUN:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) * 100L);

			/* Hack -- Factor in extra damage dice */
			if ((o_ptr->dd > k_ptr->dd) && (o_ptr->ds == k_ptr->ds))
			{
				value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 100L;
			}

			/* Done */
			break;
		}

		/* Ammo */
		case TV_AMMO:
		case TV_BULLET:
		case TV_SHOT:
		{
			/* Hack -- negative hit/damage bonuses */
			if (o_ptr->to_h + o_ptr->to_d < 0) return (0L);

			/* Factor in the bonuses */
			value += ((o_ptr->to_h + o_ptr->to_d) * 5L);

			/* Hack -- Factor in extra damage dice */
			if ((o_ptr->dd > k_ptr->dd) && (o_ptr->ds == k_ptr->ds))
			{
				value += (o_ptr->dd - k_ptr->dd) * o_ptr->ds * 5L;
			}

			/* Done */
			break;
		}
	}

	/* No negative value */
	if (value < 0) value = 0;

	/* Return the value */
	return (value);
}


/*
 * Return the price of an item including plusses (and charges).
 *
 * This function returns the "value" of the given item (qty one).
 *
 * Never notice "unknown" bonuses or properties, including "curses",
 * since that would give the player information he did not have.
 *
 * Note that discounted items stay discounted forever.
 */
s32b object_value(const object_type *o_ptr)
{
	s32b value;


	/* Unknown items -- acquire a base value */
	if (object_known_p(o_ptr))
	{
		/* Broken items -- worthless */
		if ((broken_p(o_ptr)) && (!(artifact_p(o_ptr)))) return (0L);

		/* Cursed items -- worthless */
		if (cursed_p(o_ptr)) return (0L);

		/* Real value (see above) */
		value = object_value_real(o_ptr);
	}

	/* Known items -- acquire the actual value */
	else
	{
		/* Hack -- Felt broken items */
		if (((o_ptr->ident & (IDENT_SENSE)) && broken_p(o_ptr)) &&
			(!(artifact_p(o_ptr)))) return (0L);

		/* Hack -- Felt cursed items */
		if ((o_ptr->ident & (IDENT_SENSE)) && cursed_p(o_ptr)) return (0L);

		/* Base value (see above) */
		value = object_value_base(o_ptr);
	}

	/* Apply discount (if any) */
	if (o_ptr->discount > 0 && o_ptr->discount < INSCRIP_NULL)
	{
		value -= (value * o_ptr->discount / 100L);
	}

	/* Return the final value */
	return (value);
}

/*
 * Distribute charges of rods or wands.
 *
 * o_ptr = source item
 * i_ptr = target item, must be of the same type as o_ptr
 * amt = number of items that are transferred
 */
void distribute_charges(object_type *o_ptr, object_type *i_ptr, int amt)
{
	/*
	 * Hack -- If rods or wands are dropped, the total maximum timeout or
	 * charges need to be allocated between the two stacks.   If all the items
	 * are being dropped, it makes for a neater message to leave the original
	 * stack's pval alone.  -LM-
	 */
	if ((o_ptr->tval == TV_RAY) || (o_ptr->tval == TV_APPARATUS))
	{
		i_ptr->pval = o_ptr->pval * amt / o_ptr->number;

		if (amt < o_ptr->number) o_ptr->pval -= i_ptr->pval;

		/*
		 * Hack -- Rods also need to have their timeouts distributed.  The
		 * dropped stack will accept all time remaining to charge up to its
		 * maximum.
		 */
		if ((o_ptr->tval == TV_APPARATUS) && (o_ptr->timeout))
		{
			if (i_ptr->pval > o_ptr->timeout) i_ptr->timeout = o_ptr->timeout;
			else                              i_ptr->timeout = i_ptr->pval;

			if (amt < o_ptr->number) o_ptr->timeout -= i_ptr->timeout;
		}
	}
}

/*
 * Hack -- If rods or wands are destroyed, the total maximum timeout or
 * charges of the stack needs to be reduced, unless all the items are
 * being destroyed.  -LM-
 */
void reduce_charges(object_type *o_ptr, int amt)
{
	if (((o_ptr->tval == TV_RAY) || (o_ptr->tval == TV_APPARATUS)) &&
		(amt < o_ptr->number))
	{
		o_ptr->pval -= o_ptr->pval * amt / o_ptr->number;
	}
}



/*
 * Determine if an item can "absorb" a second item
 *
 * See "object_absorb()" for the actual "absorption" code.
 *
 * If permitted, we allow rayguns (if both are either known or confirmed
 * empty) and apparatuses (in all cases) to combine.
 *
 * Tools are big and bulky.  They never stack.
 *
 * If permitted, we allow weapons/armor to stack, if fully "known".
 *
 * Missiles combine if both stacks have the same "known" status, but only
 * if both stacks have been "worn".  This helps make unidentified stacks
 * of missiles useful, but prevents the character learning that two
 * collections of missiles are the same, and thus probably both average,
 * without taking the risk of placing them in his quiver.  -LM-
 * Stacks of missiles without plusses will combine if both are at least
 * known to be "average".  -RML-
 *
 * Food, potions, scrolls, and "easy know" items always stack.
 *
 * Chests, and activatable items, never stack (for various reasons).
 */
bool object_similar(const object_type *o_ptr, const object_type *j_ptr)
{
	u32b f1, f2, f3; 


	int total = o_ptr->number + j_ptr->number;


	/* Extract the flags */
	object_flags(o_ptr, &f1, &f2, &f3); 

	/* Require identical object types */
	if (o_ptr->k_idx != j_ptr->k_idx) return (0);


	/* Analyze the items */
	switch (o_ptr->tval)
	{
		/* Chests */
		case TV_CHEST:
		case TV_BOOK:
		{
			/* Never okay */
			return (FALSE);
		}

		/* Food and tonics and Scrolls */
		case TV_TEXT:
		{
			if (o_ptr->sval == 0)
			{
				return (FALSE);
			}
			else break;
		}
		case TV_FOOD:
		case TV_TONIC:
		case TV_MECHANISM:
		{
			/* Assume okay */
			break;
		}

		/* tools and Ray guns */
		case TV_TOOL:
		{
			return (FALSE);
		}
		case TV_RAY:
		{
			/* Require knowledge */
			if (!object_known_p(o_ptr) || !object_known_p(j_ptr))
			{
				/* Allow two wands known to be "empty" to stack */
				if (!(o_ptr->ident & (IDENT_EMPTY)) ||
				    !(j_ptr->ident & (IDENT_EMPTY)))
				{
					return (FALSE);
				}
			}

			/* Enforce a maximum pval  -KBB- */
			if ((long)j_ptr->pval + o_ptr->pval > (long)MAX_SHORT)
			{
				return (FALSE);
			}

			/* Wand charges can combine.  */

			/* Assume okay */
			break;
		}

		/* tools and Ray guns and appartuses */
		case TV_APPARATUS:
		{
#if 0
			/* Enforce a maximum pval  -KBB- */
			if ((long)j_ptr->pval + o_ptr->pval > (long)MAX_SHORT)
			{
				return (FALSE);
			}

			/* Assume okay. */
			break;
#endif 
			return (FALSE);
		}

		/* Weapons and Armor */
		case TV_GUN:
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		case TV_LEG:
		case TV_CLOAK:
		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_MECHA_TORSO:
		case TV_MECHA_HEAD:
		case TV_MECHA_ARMS:
		case TV_MECHA_FEET:

		{
			/* Fall through */
		}

		/* Rings, Amulets, Lites */
		case TV_RING:
		case TV_AMULET:
		case TV_LITE:
		{
			/* If it's a throwing weapon, skip full knowledge just like bullets */
			if (f2 & (TR2_THROW))
			{
				/* fallthrough */
			}
			/* Require full knowledge of both items */
			else if (!object_known_p(o_ptr) || !object_known_p(j_ptr)) return (0);

			/* Fall through */
		}

		/* Missiles */
		case TV_SHOT:
		case TV_BULLET:
		case TV_AMMO:
		{
			/* Require identical knowledge of both items */
			if (object_known_p(o_ptr) != object_known_p(j_ptr)) return (0);

			/* Require identical "bonuses" */
			if (o_ptr->to_h != j_ptr->to_h) return (FALSE);
			if (o_ptr->to_d != j_ptr->to_d) return (FALSE);
			if (o_ptr->to_a != j_ptr->to_a) return (FALSE);

			/* Require identical "pval" code */
			if (o_ptr->pval != j_ptr->pval) return (FALSE);

			/* Require identical "artifact" names */
			if (o_ptr->name1 != j_ptr->name1) return (FALSE);

			/* Require identical "ego-item" names */
			if (o_ptr->name2 != j_ptr->name2) return (FALSE);

			/* Hack -- Never stack "powerful" items */
			if (o_ptr->xtra1 || j_ptr->xtra1) return (FALSE);

			/* Hack -- Never stack recharging items */
			if (o_ptr->timeout || j_ptr->timeout) return (FALSE);

			/* Require identical "values" */
			if (o_ptr->ac != j_ptr->ac) return (FALSE);
			if (o_ptr->dd != j_ptr->dd) return (FALSE);
			if (o_ptr->ds != j_ptr->ds) return (FALSE);

			/* Probably okay */
			break;
		}

		/* Various */
		default:
		{
			/* Require knowledge */
			if (!object_known_p(o_ptr) || !object_known_p(j_ptr)) return (0);

			/* Probably okay */
			break;
		}
	}


	/* Hack -- Require identical "cursed" and "broken" status */
	if (((o_ptr->ident & (IDENT_CURSED)) != (j_ptr->ident & (IDENT_CURSED))) ||
	    ((o_ptr->ident & (IDENT_BROKEN)) != (j_ptr->ident & (IDENT_BROKEN))))
	{
		return (FALSE);
	}


	/* Hack -- Require compatible inscriptions */
	if (o_ptr->note != j_ptr->note)
	{
		/* Always force notes */
		/* Normally require matching inscriptions */
		/* if (!stack_force_notes) return (FALSE); */

		/* Never combine different inscriptions */
		if (o_ptr->note && j_ptr->note) return (FALSE);
	}


	/* Hack -- Require compatible "discount" fields */
	if (o_ptr->discount != j_ptr->discount)
	{
		/* Both are (different) special inscriptions */
		if ((o_ptr->discount >= INSCRIP_NULL) &&
		    (j_ptr->discount >= INSCRIP_NULL))
		{
			/* Normally require matching inscriptions */
			return (FALSE);
		}

		/* One is a special inscription, one is a discount or nothing */
		else if ((o_ptr->discount >= INSCRIP_NULL) ||
		         (j_ptr->discount >= INSCRIP_NULL))
		{
  	  /* Always force notes */
		  /* Normally require matching inscriptions */
		  /* if (!stack_force_notes) return (FALSE); */

			/* Hack -- Never merge a special inscription with a discount */
			if ((o_ptr->discount > 0) && (j_ptr->discount > 0)) return (FALSE);
		}

#if 0 /* removing force costs */
		/* One is a discount, one is a (different) discount or nothing */
		else
		{
			/* Normally require matching discounts */
			if (!stack_force_costs) return (FALSE);
		}
#endif
	}


	/* Maximal "stacking" limit */
	if (total >= MAX_STACK_SIZE) return (FALSE);


	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow one item to "absorb" another, assuming they are similar.
 *
 * The blending of the "note" field assumes that either (1) one has an
 * inscription and the other does not, or (2) neither has an inscription.
 * In both these cases, we can simply use the existing note, unless the
 * blending object has a note, in which case we use that note.
 *
 * The blending of the "discount" field assumes that either (1) one is a
 * special inscription and one is nothing, or (2) one is a discount and
 * one is a smaller discount, or (3) one is a discount and one is nothing,
 * or (4) both are nothing.  In all of these cases, we can simply use the
 * "maximum" of the two "discount" fields.
 *
 * These assumptions are enforced by the "object_similar()" code.
 */
void object_absorb(object_type *o_ptr, const object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	/* Add together the item counts */
	o_ptr->number = ((total < MAX_STACK_SIZE) ? total : (MAX_STACK_SIZE - 1));

	/* Hack -- Blend "known" status */
	if (object_known_p(j_ptr)) object_known(o_ptr);

	/* Hack -- Blend "rumour" status */
	if (j_ptr->ident & (IDENT_RUMOUR)) o_ptr->ident |= (IDENT_RUMOUR);

	/* Hack -- Blend "mental" status */
	if (j_ptr->ident & (IDENT_MENTAL)) o_ptr->ident |= (IDENT_MENTAL);

	/* Hack -- Blend "notes" */
	if (j_ptr->note != 0) o_ptr->note = j_ptr->note;

	/* Mega-Hack -- Blend "discounts" */
	if (o_ptr->discount < j_ptr->discount) o_ptr->discount = j_ptr->discount;

	/*
	 * Hack -- if rods are stacking, add the pvals (maximum timeouts) and
	 * current timeouts together.
	 */
	if (o_ptr->tval == TV_APPARATUS)
	{
		o_ptr->pval += j_ptr->pval;
		o_ptr->timeout += j_ptr->timeout;
	}

	/* Hack -- if wands are stacking, combine the charges. */
	if (o_ptr->tval == TV_RAY)
	{
		o_ptr->pval += j_ptr->pval;
	}
}



/*
 * Find the index of the object_kind with the given tval and sval
 */
s16b lookup_kind(int tval, int sval)
{
	int k;

	/* Look for it */
	for (k = 1; k < z_info->k_max; k++)
	{
		object_kind *k_ptr = &k_info[k];

		/* Found a match */
		if ((k_ptr->tval == tval) && (k_ptr->sval == sval)) return (k);
	}

	/* Oops */
	msg_format("No object (%d,%d)", tval, sval);

	/* Oops */
	return (0);
}


/*
 * Wipe an object clean.
 */
void object_wipe(object_type *o_ptr)
{
	/* Wipe the structure */
	(void)WIPE(o_ptr, object_type);
}


/*
 * Prepare an object based on an existing object
 */
void object_copy(object_type *o_ptr, const object_type *j_ptr)
{
	/* Copy the structure */
	COPY(o_ptr, j_ptr, object_type);
}


/*
 * Prepare an object based on an object kind.
 */
void object_prep(object_type *o_ptr, int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Clear the record */
	(void)WIPE(o_ptr, object_type);

	/* Save the kind index */
	o_ptr->k_idx = k_idx;

	/* Efficiency -- tval/sval */
	o_ptr->tval = k_ptr->tval;
	o_ptr->sval = k_ptr->sval;

	/* Default "pval" */
	o_ptr->pval = k_ptr->pval;

	/* Default pval-dependent flags */
	o_ptr->flags_pval1 = k_ptr->flags_pval;

	/* Default object flags */
	o_ptr->flags1 = k_ptr->flags1;
	o_ptr->flags2 = k_ptr->flags2;
	o_ptr->flags3 = k_ptr->flags3;

	/* Default number */
	o_ptr->number = 1;

	/* Default weight */
	o_ptr->weight = k_ptr->weight;

	/* Default magic */
	o_ptr->to_h = k_ptr->to_h;
	o_ptr->to_d = k_ptr->to_d;
	o_ptr->to_a = k_ptr->to_a;

	/* Default power */
	o_ptr->force = k_ptr->force;
	o_ptr->ac = k_ptr->ac;
	o_ptr->dd = k_ptr->dd;
	o_ptr->ds = k_ptr->ds;

	/* Default ranged */
	o_ptr->ammo_tval = k_ptr->ammo_tval;
	o_ptr->ammo_mult = k_ptr->ammo_mult;
	o_ptr->num_fire = k_ptr->num_fire;
	o_ptr->range = k_ptr->range;
	o_ptr->degree = k_ptr->degree;

	/* Hack -- worthless items are always "broken" */
	if (k_ptr->cost <= 0) o_ptr->ident |= (IDENT_BROKEN);

	/* Hack -- cursed items are always "cursed" */
	if (k_ptr->flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED);
	/* if (cursed_p(k_ptr)) o_ptr->ident |= (IDENT_CURSED); */
}


/*
 * Help determine an "enchantment bonus" for an object.
 *
 * To avoid floating point but still provide a smooth distribution of bonuses,
 * we simply round the results of division in such a way as to "average" the
 * correct floating point value.
 *
 * This function has been changed.  It uses "Rand_normal()" to choose values
 * from a normal distribution, whose mean moves from zero towards the max as
 * the level increases, and whose standard deviation is equal to 1/4 of the
 * max, and whose values are forced to lie between zero and the max, inclusive.
 *
 * Since the "level" rarely passes 100 before Morgoth is dead, it is very
 * rare to get the "full" enchantment on an object, even at deep levels.
 *
 * It is always possible (albeit unlikely) to get the "full" enchantment.
 *
 * Note:
 * Maximum level can now vary.
 *
 * A sample distribution of values from "m_bonus(10, N, 128)" is shown below:
 *
 *   N       0     1     2     3     4     5     6     7     8     9    10
 * ---    ----  ----  ----  ----  ----  ----  ----  ----  ----  ----  ----
 *   0   66.37 13.01  9.73  5.47  2.89  1.31  0.72  0.26  0.12  0.09  0.03
 *   8   46.85 24.66 12.13  8.13  4.20  2.30  1.05  0.36  0.19  0.08  0.05
 *  16   30.12 27.62 18.52 10.52  6.34  3.52  1.95  0.90  0.31  0.15  0.05
 *  24   22.44 15.62 30.14 12.92  8.55  5.30  2.39  1.63  0.62  0.28  0.11
 *  32   16.23 11.43 23.01 22.31 11.19  7.18  4.46  2.13  1.20  0.45  0.41
 *  40   10.76  8.91 12.80 29.51 16.00  9.69  5.90  3.43  1.47  0.88  0.65
 *  48    7.28  6.81 10.51 18.27 27.57 11.76  7.85  4.99  2.80  1.22  0.94
 *  56    4.41  4.73  8.52 11.96 24.94 19.78 11.06  7.18  3.68  1.96  1.78
 *  64    2.81  3.07  5.65  9.17 13.01 31.57 13.70  9.30  6.04  3.04  2.64
 *  72    1.87  1.99  3.68  7.15 10.56 20.24 25.78 12.17  7.52  4.42  4.62
 *  80    1.02  1.23  2.78  4.75  8.37 12.04 27.61 18.07 10.28  6.52  7.33
 *  88    0.70  0.57  1.56  3.12  6.34 10.06 15.76 30.46 12.58  8.47 10.38
 *  96    0.27  0.60  1.25  2.28  4.30  7.60 10.77 22.52 22.51 11.37 16.53
 * 104    0.22  0.42  0.77  1.36  2.62  5.33  8.93 13.05 29.54 15.23 22.53
 * 112    0.15  0.20  0.56  0.87  2.00  3.83  6.86 10.06 17.89 27.31 30.27
 * 120    0.03  0.11  0.31  0.46  1.31  2.48  4.60  7.78 11.67 25.53 45.72
 * 128    0.02  0.01  0.13  0.33  0.83  1.41  3.24  6.17  9.57 14.22 64.07
 */
static s16b m_bonus(int max, int level, int max_level)
{
	int bonus, stand, value;


	/* Level cannot go above maximum */
	if (level >= max_level) level = max_level - 1;


	/* Average bonus depends on level  (use perfect rounding) */
	bonus = div_round(max * level, max_level);

	/* The standard deviation is equal to one quarter of the max */
	stand = div_round(max, 4);

	/* Calculate the bonus */
	value = Rand_normal(bonus, stand);


	/* Enforce the minimum value */
	if (value < 0) return (0);

	/* Enforce the maximum value */
	if (value > max) return (max);

	/* Result */
	return (value);
}




/*
 * Cheat -- describe a created object for the user
 */
void object_mention(const object_type *o_ptr)
{
	char o_name[80];

	/* Describe */
	object_desc_store(o_name, o_ptr, FALSE, 0);

	/* Artifact */
	if (artifact_p(o_ptr))
	{
		/* Silly message */
		msg_format("Artifact (%s)", o_name);
	}

	/* Ego-item */
	else if (ego_item_p(o_ptr))
	{
		/* Silly message */
		msg_format("Ego-item (%s)", o_name);
	}

	/* Normal item */
	else
	{
		/* Silly message */
		msg_format("Object (%s)", o_name);
	}
}


/*
 * Attempt to change an object into an ego-item -MWK-
 * Better only called by apply_magic().
 * The return value says if we picked a cursed item (if allowed) and is
 * passed on to a_m_aux1/2().
 * If no legal ego item is found, this routine returns 0, resulting in
 * an unenchanted item.
 */
static int make_ego_item(object_type *o_ptr, bool only_good)
{
	int i, j;
	int level = object_level;

	int e_idx;

	long value, total;

	u32b cursed_flags = TR3_LIGHT_CURSE | TR3_HEAVY_CURSE | TR3_PERMA_CURSE;

	ego_item_type *e_ptr;

	alloc_entry *table = alloc_ego_table;


	/* Fail if object already is ego or artifact */
	if (o_ptr->name1) return (FALSE);
	if (o_ptr->name2) return (FALSE);

	/* Boost level (like with object base types) */
	if (object_level > 0)
	{
		/* Occasional "boost" */
		if (one_in_(GREAT_EGO))
		{
			/* Boost on 20th level can (rarely) be 16 + 4 * 20 / 3, or 42 */
			int boost = ABS(Rand_normal(0, 4 + level / 3));

			/* Boost the depth */
			level += boost;
			
			/* The bizarre calculation again */
			/* level = 1 + (level * MAX_DEPTH / randint(MAX_DEPTH)); */
		}
	}

	/* Reset total */
	total = 0L;

	/* Process probabilities */
	for (i = 0; i < alloc_ego_size; i++)
	{
		/* Default */
		table[i].prob3 = 0;

		/* Objects are sorted by depth */
		if (table[i].level > level) continue;

		/* Get the index */
		e_idx = table[i].index;

		/* Get the actual kind */
		e_ptr = &e_info[e_idx];

		/* Test if this is a possible ego-item for value of (cursed) */
		if (only_good && (e_ptr->flags3 & (cursed_flags))) continue;
		if (!only_good && !(e_ptr->flags3 & (cursed_flags))) continue;

		/* Test if this is a legal ego-item type for this object */
		for (j = 0; j < 3; j++)
		{
			/* Require identical base type */
			if (o_ptr->tval == e_ptr->tval[j])
			{
				/* Require sval in bounds, lower */
				if (o_ptr->sval >= e_ptr->min_sval[j])
				{
					/* Require sval in bounds, upper */
					if (o_ptr->sval <= e_ptr->max_sval[j])
					{
						/* Accept */
						table[i].prob3 = table[i].prob2;
					}
				}
			}
		}

		/* Total */
		total += table[i].prob3;
	}

	/* No legal ego-items -- create a normal unenchanted one */
	if (total == 0) return (0);


	/* Pick an ego-item */
	value = rand_int(total);

	/* Find the object */
	for (i = 0; i < alloc_ego_size; i++)
	{
		/* Found the entry */
		if (value < table[i].prob3) break;

		/* Decrement */
		value = value - table[i].prob3;
	}

	/* We have one */
	e_idx = table[i].index;
	o_ptr->name2 = e_idx;

	return ((e_info[e_idx].flags3 & TR3_LIGHT_CURSE) ? -2 : 2);
}


/*
 * Mega-Hack -- Attempt to create one of the "Special Objects".
 *
 * We are only called from "make_object()", and we assume that
 * "apply_magic()" is called immediately after we return.
 *
 * Note -- see "make_artifact()" and "apply_magic()".
 *
 * We *prefer* to create the special artifacts in order, but this is
 * normally outweighed by the "rarity" rolls for those artifacts.  The
 * only major effect of this logic is that the Phial (with rarity one)
 * is always the first special artifact created.
 */
static bool make_artifact_special(object_type *o_ptr)
{
	int i, first_pick;

	int k_idx;


	/* No artifacts, do nothing */
	if (adult_no_artifacts) return (FALSE);

	/* No artifacts in the town */
	if (!p_ptr->depth) return (FALSE);

	/* Pick an initial artifact at random */
	first_pick = rand_int(ART_MIN_NORMAL);

	/* Check the special artifacts */
	for (i = first_pick; i < first_pick + ART_MIN_NORMAL; ++i)
	{
		/* Convert i into an index from 1 to ART_MIN_NORMAL - 1. */
		int choice = (i % (ART_MIN_NORMAL - 1)) + 1;
		artifact_type *a_ptr = &a_info[choice];

		/* Skip "empty" artifacts */
		if (!a_ptr->name) continue;

		/* Cannot make an artifact twice */
		if (a_ptr->cur_num) continue;

		/* Enforce minimum "depth" (loosely) */
		if (a_ptr->level > p_ptr->depth)
		{
			/* Get the "out-of-depth factor" */
			int d = (a_ptr->level - p_ptr->depth) * 5;

			/* Roll for out-of-depth creation (stay sane) */
			if ((d > 30) || (!one_in_(d))) continue;
		}

		/* Artifact "rarity roll" */
		if (!one_in_(a_ptr->rarity)) continue;

		/* Find the base object */
		k_idx = lookup_kind(a_ptr->tval, a_ptr->sval);

		/* Assign the template */
		object_prep(o_ptr, k_idx);

		/* Mark the item as an artifact */
		o_ptr->name1 = choice;

		/* Success */
		return (TRUE);
	}

	/* Failure */
	return (FALSE);
}


/*
 * Attempt to change an object into an artifact
 *
 * This routine should only be called by "apply_magic()"
 *
 * Note -- see "make_artifact_special()" and "apply_magic()"
 */
static bool make_artifact(object_type *o_ptr)
{
	s16b i, chance;
	s32b total = 0L;
	s32b roll;


	/* No artifacts, do nothing */
	if (adult_no_artifacts) return (FALSE);

	/* No artifacts in the town */
	if (!p_ptr->depth) return (FALSE);

	if (TRUE)
	{
		/* Table of artifact chances */
		s16b *art_chance;

		/* Allocate the "art_chance" array */
		C_MAKE(art_chance, z_info->a_max, s16b);

		/* Initialize the chance table */
		art_chance[ART_MIN_NORMAL - 1] = 0;

		/* Check the artifact list (skip the "specials") */
		for (i = ART_MIN_NORMAL; i < z_info->a_max; i++)
		{
			artifact_type *a_ptr = &a_info[i];
	
			/* Clear chance */
			chance = 10000;
			art_chance[i] = art_chance[i-1];

			/* Skip "empty" items */
			if (!a_ptr->name) continue;
	
			/* Cannot make an artifact twice */
			if (a_ptr->cur_num) continue;
	
			/* Must have the correct fields */
			if (a_ptr->tval != o_ptr->tval) continue;
			if (a_ptr->sval != o_ptr->sval) continue;
	
			/* Artifacts never show up when they would seem wimpy XCCCX */
			if ((a_ptr->level + 20) < p_ptr->depth) continue;
			
			/* XXX XXX Enforce minimum "depth" (loosely) */
			if (a_ptr->level > p_ptr->depth)
			{
				/* Get the "out-of-depth factor" */
				int d = (a_ptr->level - p_ptr->depth) * 2;

				/* Penalize out-of-depth creation */
				chance /= d;
			}
	
			/* Artifact rarity reduces chance */
			if (a_ptr->rarity > 1)
			{
				chance /= a_ptr->rarity;
			}
	
			/* Tally up totals */
			total += (s32b)chance;

			/* Store this chance */
			art_chance[i] += chance;
		}

		/* No artifact templates are available */
		if (!total) return (FALSE);

		/* Roll for artifact -- at least a 50% chance of failure */
		roll = rand_int((total < 5000L) ? 10000L : (total * 2L));

		/* Try to make the artifact */
		if (roll < total)
		{
			/* Check the artifact list (skip the "specials") */
			for (i = ART_MIN_NORMAL; i < z_info->a_max; i++)
			{
				/* Find that artifact whose chance value contains our roll */
				if (art_chance[i] > roll)
				{
					/* Mark the item as an artifact */
					o_ptr->name1 = i;

					/* Paranoia -- no "plural" artifacts */
					if (o_ptr->number != 1) o_ptr->number = 1;

					/* Success */
					break;
				}
			}
		}
		/* Free the "art_chance" array */
		FREE(art_chance, s16b);
	}

	/* Return "object is now an artifact" */
	return ((bool)o_ptr->name1);
}


/*
 * Apply magic to an item known to be a "weapon"
 *
 * Hack -- note special base damage dice boosting
 * Hack -- note special processing for weapon/digger
 * Hack -- note special rating boost for dragon scale mail
 */
static void a_m_aux_1(object_type *o_ptr, int level, int power)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	int tohit1 = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);
	int todam1 = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);
	/* int toac1  = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH); */

	int tohit2 = m_bonus(10, level, MAX_ITEM_GEN_DEPTH);
	int todam2 = m_bonus(10, level, MAX_ITEM_GEN_DEPTH);
	/* int toac2  = m_bonus(10, level, MAX_ITEM_GEN_DEPTH); */
	
	/*
	 * Weapon is an ego-item.  We alter values in the same direction as
	 * the ego-item template does.  If it doesn't make any changes, we
	 * give good weapons bonuses and bad ones penalties.
	 */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Handle bonuses or penalties to Skill and Deadliness */
		if (power > 0)
		{
			/* Good weapons with negative bonuses are left alone */
			if (e_ptr->max_to_h >= 0) o_ptr->to_h += (tohit1 + tohit2);
			if (e_ptr->max_to_d >= 0) o_ptr->to_d += (todam1 + todam2);
			/* if (e_ptr->max_to_a >= 0) o_ptr->to_a += (toac1 + toac2); */
		}
		else if (power < 0)
		{
			/* Bad weapons with positive bonuses are left alone */
			if (e_ptr->max_to_h <= 0) o_ptr->to_h -= (tohit1 + tohit2);
			if (e_ptr->max_to_d <= 0) o_ptr->to_d -= (todam1 + todam2);
			/* if (e_ptr->max_to_a <= 0) o_ptr->to_a -= (toac1 + toac2); */
		}
	}

	/* Weapon is not an ego-item */
	else
	{
		/* Good */
		if (power > 0)
		{
			/* Enchant */
			o_ptr->to_h += tohit1;
			o_ptr->to_d += todam1;
	
			/* Very good */
			if (power > 1)
			{
				/* Enchant again */
				o_ptr->to_h += tohit2;
				o_ptr->to_d += todam2;
			}
		}
	
		/* Cursed */
		else if (power < 0)
		{
			/* Penalize */
			o_ptr->to_h -= tohit1;
			o_ptr->to_d -= todam1;
	
			/* Very cursed */
			if (power < -1)
			{
				/* Penalize again */
				o_ptr->to_h -= tohit2;
				o_ptr->to_d -= todam2;
			}
	
			/* Cursed (if "bad") */
		  	if (o_ptr->to_h + o_ptr->to_d < 0) o_ptr->ident |= (IDENT_CURSED);
		}
	}
	
	/* Analyze type */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		{
			/* Very good */
			if (power > 1)
			{
				/* Special Ego-item */
				/* o_ptr->name2 = EGO_DIGGING; */
			}

			/* Very bad */
			else if (power < -1)
			{
				/* Hack -- Horrible digging bonus */
				o_ptr->pval = 0 - rand_range(5, 10);
			}

			/* Bad */
			else if (power < 0)
			{
				/* Hack -- Reverse digging bonus */
				o_ptr->pval = 0 - (o_ptr->pval);
			}

			break;
		}


		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		{
			int i;
			int extra_depth = object_level - k_ptr->level;
			int sides = o_ptr->ds;

			/* Randomize the limit slightly (swords are penalized) */
			int limit = (o_ptr->tval == TV_SWORD ? rand_range(30, 34) :
			             rand_range(38, 42));

			/* Roll for better dice */
			for (i = 0; i < 3; i++)
			{
				/* One in 12 chance to improve a dagger to 1d6 */
				int chance = 8 + (o_ptr->dd * sides);

				/* Require that weapon be well in depth */
				if (extra_depth >= rand_range(3, 8) + (i * 8)) break;

				/* Weapon must not be over-powerful */
				if (o_ptr->dd * sides > limit) break;

				/* Must make the rarity roll */
				if (!one_in_(chance)) break;

				/* Improve the dice (with some randomization) */
				if      (i == 0) sides = div_round(3 * o_ptr->ds, 2);
				else if (i == 1) sides = 2 * o_ptr->ds;
				else             sides = div_round(5 * o_ptr->ds, 2);
			}

			/* Adjust dice */
			if (sides > o_ptr->ds) o_ptr->ds = sides;


			/* Upon occasion, throwing weapons may be perfectly balanced. */
			if (k_ptr->flags2 & (TR2_THROW))
			{
				o_ptr->flags2 |= (TR2_THROW);

				if ((power >= 0) && (one_in_(5)))
				{
					o_ptr->flags2 |= (TR2_PERFECT_BALANCE);
				}
			}

			/*
			 * On very rare occasions, non-throwing weapons will get an
			 * extra damage die.
			 */
			else
			{
				/* Must not have super dice already */
				if (o_ptr->ds <= 3 * k_ptr->ds / 2)
				{
					/* Must have low dice and be well in depth */
					if ((o_ptr->dd == 1) && (extra_depth >= 10))
					{
						if (one_in_(100)) o_ptr->dd = 2;
					}
					else if ((o_ptr->dd == 2) && (extra_depth >= 10))
					{
						if (one_in_(200)) o_ptr->dd = 3;
					}
				}
			}

			break;
		}


		case TV_SHOT:
		case TV_BULLET:
		case TV_AMMO:
		{
			/* Up to three chances to enhance damage dice. */
			if ((object_level >= 25) && one_in_(10))
			{
				o_ptr->ds += 2;
				if ((object_level >= 45) && one_in_(5))
				{
					o_ptr->ds += 2;
					if ((object_level >= 65) && one_in_(5))
					{
						 o_ptr->ds += 2;
					}
				}
			}

			break;
		}
	}
}


/*
 * Apply magic to an item known to be "armor"
 *
 * Hack -- note special processing for crown/helm
 * Hack -- note special processing for robe of permanence
 */
static void a_m_aux_2(object_type *o_ptr, int level, int power)
{
	int toac1 = randint(3) + m_bonus(3, level, MAX_ITEM_GEN_DEPTH);

	int toac2 = m_bonus(8, level, MAX_ITEM_GEN_DEPTH);

	/* Armour is an ego-item. */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Handle bonuses or penalties to AC */
		if (power > 0)
		{
			/* Good armours with negative bonuses are left alone */
			if (e_ptr->max_to_a >= 0) o_ptr->to_a += (toac1 + toac2);
		}
		else if (power < 0)
		{
			/* Bad armours with positive bonuses are left alone */
			if (e_ptr->max_to_a <= 0) o_ptr->to_a -= (toac1 + toac2);
		}
	}

	/* Armour is not an ego-item */
	else
	{
		/* Good */
		if (power > 0)
		{
			/* Enchant */
			o_ptr->to_a += toac1;
	
			/* Very good */
			if (power > 1)
			{
				/* Enchant again */
				o_ptr->to_a += toac2;
			}
		}
	
		/* Cursed */
		else if (power < 0)
		{
			/* Penalize */
			o_ptr->to_a -= toac1;
	
			/* Very cursed */
			if (power < -1)
			{
				/* Penalize again */
				o_ptr->to_a -= toac2;
			}
	
			/* Cursed (if "bad") */
			if (o_ptr->to_a < 0) o_ptr->ident |= (IDENT_CURSED);
		}

	}
	/* Analyze type */
	switch (o_ptr->tval)
	{
		case TV_DRAG_ARMOR:
		{
			/* nothing */
			break;
		}
	}
}



/*
 * Apply magic to an item known to be a "ring" or "amulet"
 *
 * Hack -- note special rating boost for ring of speed
 * Hack -- note special rating boost for amulet of the magi
 * Hack -- note special "pval boost" code for ring of speed
 * Hack -- note that some items must be cursed (or blessed)
 */
static void a_m_aux_3(object_type *o_ptr, int level, int power)
{
	/* object_kind *k_ptr = &k_info[o_ptr->k_idx]; */
	
	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		case TV_RING:
		{			
#if 0
			/* BZZZZT!!! WRONG - THANKS FOR PLAYING */
			/* This alters the actual template for resist fire, not the specific object */
			for (i = 0; i < RS_MAX; i++)
			{
				/* randomize a large resist some */
				if (k_ptr->res[i] > 40)
				{
					k_ptr->res[i] = rand_range(k_ptr->res[i] - 20, k_ptr->res[i] + 20);
				}
				else if (k_ptr->res[i] >= 5)
				{
					k_ptr->res[i] = rand_range(k_ptr->res[i] - 10, k_ptr->res[i] + 10);
					if (k_ptr->res[i] < 0) k_ptr->res[i] = 0;
				}
				else 
				{
					/* Nothing */
				}		
			}
#endif		
			/* Analyze */
			switch (o_ptr->sval)
			{
				/* Strength, Constitution, Dexterity, Intelligence */
				case SV_RING_MUS:
				case SV_RING_VIG:
				case SV_RING_AGI:
				case SV_RING_SCH:
				case SV_RING_EGO:
				case SV_RING_CHR:
				{
					/* Stat bonus */
					o_ptr->pval = 3 + m_bonus(10, level, MAX_ITEM_GEN_DEPTH);

					/* Possible super-charging */
					if (one_in_(20)) o_ptr->pval += randint(10);
	
					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Ring of Protection */
				case SV_RING_PROTECTION:
				{
					/* Bonus to armor class */
					o_ptr->to_a = rand_range(6, 12) + m_bonus(10, level, MAX_ITEM_GEN_DEPTH);

					/* Possible super-charging */
					if (one_in_(10)) o_ptr->to_a += randint(30);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse toac */
						o_ptr->to_a = 0 - (o_ptr->to_a);
					}

					break;
				}
				
				/* Searching */
				case SV_RING_STEALTH:
				case SV_RING_SEARCHING:
				{
					/* Bonus to searching */
					o_ptr->pval = 1 + m_bonus(10, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Ring of Accuracy */
				case SV_RING_ACCURACY:
				{
					/* Bonus to hit */
					o_ptr->to_h = rand_range(6, 10) + m_bonus(20, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse tohit */
						o_ptr->to_h = 0 - (o_ptr->to_h);
					}

					break;
				}

				/* Ring of damage */
				case SV_RING_DAMAGE:
				{
					/* Bonus to damage */
					o_ptr->to_d = rand_range(6, 10) + m_bonus(20, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse bonus */
						o_ptr->to_d = 0 - (o_ptr->to_d);
					}

					break;
				}
				/* Ring of Slaying */
				case SV_RING_SLAYING:
				{
					/* Bonus to damage and to hit */
					o_ptr->to_d = rand_range(6, 8) + m_bonus(15, level, MAX_ITEM_GEN_DEPTH);
					o_ptr->to_h = rand_range(6, 8) + m_bonus(15, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse bonuses */
						o_ptr->to_h = 0 - (o_ptr->to_h);
						o_ptr->to_d = 0 - (o_ptr->to_d);
					}

					break;
				}
				/* Ring of Speed! */
				case SV_RING_SPEED:
				case SV_RING_MANA:
				case SV_RING_HEALTH:
				{
					/* Base speed (1 to 10) */
					o_ptr->pval = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);

					/* Super-charge the ring */
					while (rand_int(100) < 60) o_ptr->pval++;

					/* Cursed Ring */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse pval */
						o_ptr->pval = 0 - (o_ptr->pval);

						break;
					}

					/* Rating boost */
					rating += 25;

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}

				/* Weakness, Stupidity */
				case SV_RING_WEAKNESS:
				case SV_RING_STUPIDITY:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->ident |= (IDENT_CURSED);

					/* Penalize */
					o_ptr->pval = 0 - (1 + m_bonus(5, level, MAX_ITEM_GEN_DEPTH));

					break;
				}

				/* WOE, Stupidity */
				case SV_RING_WOE:
				{
					/* Broken */
					o_ptr->ident |= (IDENT_BROKEN);

					/* Cursed */
					o_ptr->ident |= (IDENT_CURSED);

					/* Penalize */
					o_ptr->to_a = 0 - (5 + m_bonus(10, level, MAX_ITEM_GEN_DEPTH));
					o_ptr->pval = 0 - (1 + m_bonus(5, level, MAX_ITEM_GEN_DEPTH));

					break;
				}


			}
			break;
		}

		case TV_AMULET:
		{
			/* Analyze */
			switch (o_ptr->sval)
			{
				/* Amulet of stat */
				case SV_AMULET_MUSCLE:
				case SV_AMULET_VIGOR:
				case SV_AMULET_EGO:
				case SV_AMULET_SCHOOLING:
				case SV_AMULET_AGILITY:
				case SV_AMULET_CHARM:
				{
					o_ptr->pval = 1 + m_bonus(10, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Amulet of searching */
				case SV_AMULET_MANA:
				case SV_AMULET_HEALTH:
				{
					o_ptr->pval = 1 + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);

					/* Boost the rating */
					rating += 15;

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				case SV_AMULET_SEARCHING:
				case SV_AMULET_STEALTH:
				case SV_AMULET_SAVE:
				case SV_AMULET_SHADOW:
				{
					o_ptr->pval = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);

					/* Cursed */
					if (power < 0)
					{
						/* Broken */
						o_ptr->ident |= (IDENT_BROKEN);

						/* Cursed */
						o_ptr->ident |= (IDENT_CURSED);

						/* Reverse bonuses */
						o_ptr->pval = 0 - (o_ptr->pval);
					}

					break;
				}

				/* Amulet of the Magi -- never cursed */
				case SV_AMULET_THE_MAGI:
				{
					o_ptr->pval = 2 + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);
					o_ptr->to_a = randint(5) + m_bonus(5, level, MAX_ITEM_GEN_DEPTH);

					/* Boost the rating */
					rating += 25;

					/* Mention the item */
					if (cheat_peek) object_mention(o_ptr);

					break;
				}
			}
			break;
		}
	}
}


/*
 * Apply magic to an item known to be "boring"
 *
 * Hack -- note the special code for various items
 */
static void a_m_aux_4(object_type *o_ptr, int level, int power)
{
	object_kind *k_ptr = &k_info[o_ptr->k_idx];
	/* Unused parameters */
	(void)level;
	(void)power;

	/* Apply magic (good or bad) according to type */
	switch (o_ptr->tval)
	{
		case TV_LITE:
		{
			/* Give random fuel */
			if (o_ptr->pval > 0) o_ptr->pval = randint(o_ptr->pval);

			/* Mark it as a light source */
			o_ptr->flags_pval2 |= (TR_PVAL_LIGHT);

			/* Hack -- torches have a light radius of 2 (normally) */
			if (o_ptr->sval == SV_LITE_TAPER) o_ptr->pval2 = 1;

			/* Hack -- torches have a light radius of 2 (normally) */			
			if (
				(o_ptr->sval == SV_LITE_CANDLE_WAX) || 
				(o_ptr->sval == SV_LITE_CANDLE_TALLOW)
			   ) 
			{
			   		o_ptr->pval2 = 1;
			}
			/* Hack -- torches have a light radius of 2 (normally) */
			if (o_ptr->sval == SV_LITE_TORCH) o_ptr->pval2 = 2;

			/* Hack -- lanterns have a light radius of 2 */
			if (o_ptr->sval == SV_LITE_LANTERN) o_ptr->pval2 = 3;

			break;
		}
		case TV_RAY:
		case TV_TOOL:
		{
			/*
			 * The wand or staff gets a number of initial charges equal
			 * to between 1/2 (rounded up) and the full object kind's pval.
			 */
			o_ptr->pval = rand_range((k_ptr->pval + 1) / 2, k_ptr->pval);

			/* Hack -- handle multiple wands */
			if ((o_ptr->tval == TV_RAY) && (o_ptr->number > 1))
				o_ptr->pval *= o_ptr->number;

			break;
		}
		case TV_CHEST:
		{
			/* Hack -- skip ruined chests */
			if (k_info[o_ptr->k_idx].level <= 0) break;

			/* Hack -- pick a "difficulty" */
			o_ptr->pval = randint(k_info[o_ptr->k_idx].level);

			/* Never exceed "difficulty" of 55 to 59 */
			if (o_ptr->pval > 55) o_ptr->pval = (s16b)(55 + rand_int(5));

			break;
		}
	}
}





/*
 * Calculate the plusses on an ego-item
 */
static s16b ego_item_bonus(int val)
{
	/* Half the bonus is random, the other half is guaranteed */
	int tmp = rand_range(ABS(val / 2), ABS(val));

	/* Return a positive or negative value */
	if (val < 0) return (-tmp);
	else         return (tmp);
}

/*
 * Complete the "creation" of an object by applying "magic" to the item
 *
 * This includes not only rolling for random bonuses, but also putting the
 * finishing touches on ego-items and artifacts, giving charges to wands and
 * tools, giving fuel to lites, and placing traps on chests.
 *
 * In particular, note that "Instant Artifacts", if "created" by an external
 * routine, must pass through this function to complete the actual creation.
 *
 * The base "chance" of the item being "good" increases with the "level"
 * parameter, which is usually derived from the dungeon level, being equal
 * to the level plus 10, up to a maximum of 75.  If "good" is true, then
 * the object is guaranteed to be "good".  If an object is "good", then
 * the chance that the object will be "great" (ego-item or artifact), also
 * increases with the "level", being equal to half the level, plus 5, up to
 * a maximum of 20.  If "great" is true, then the object is guaranteed to be
 * "great".  At dungeon level 65 and below, 15/100 objects are "great".
 *
 * If the object is not "good", there is a chance it will be "cursed", and
 * if it is "cursed", there is a chance it will be "broken".  These chances
 * are related to the "good" / "great" chances above.
 *
 * Otherwise "normal" rings and amulets will be "good" half the time and
 * "cursed" half the time, unless the ring/amulet is always good or cursed.
 *
 * If "okay" is true, and the object is going to be "great", then there is
 * a chance that an artifact will be created.  This is true even if both the
 * "good" and "great" arguments are false.  Objects which are forced "great"
 * get three extra "attempts" to become an artifact.
 */
void apply_magic(object_type *o_ptr, int lev, bool okay, bool good, bool great)
{
	int i, rolls, f1, f2, power, adjust;


	/* Maximum "level" for various things */
	if (lev > MAX_DEPTH - 1) lev = MAX_DEPTH - 1;


	/* Base chance of being "good" */
	f1 = 4 * lev / 5 + 20;

	/* Maximal chance of being "good" */
	if (f1 > 80) f1 = 80;

	/* Base chance of being "great" */
	f2 = f1 * 3 / 4;

	/* Maximal chance of being "great" */
	if (f2 > 40) f2 = 40;


	/* Assume normal */
	power = 0;

	/* Roll for "good" */
	if (good || (rand_int(100) < f1))
	{
		/* Assume "good" */
		power = 1;

		/* Roll for "great" */
		if (great || (rand_int(100) < f2)) power = 2;
	}

	/* Roll for "cursed" making fewer cursed items */
	else if (rand_int(200) < f1)
	{
		/* Assume "cursed" */
		power = -1;

		/* Roll for "broken" */
		if (rand_int(100) < f2) power = -2;
	}

	/* Assume no rolls */
	rolls = 0;

	/* Get one roll if excellent */
	if (power >= 2) rolls = 2;

	/* Get four rolls if forced great */
	if (great) rolls = 6;

	/* Get no rolls if not allowed */
	if (!okay || o_ptr->name1) rolls = 0;

	/* Roll for artifacts if allowed */
	for (i = 0; i < rolls; i++)
	{
		/* Roll for an artifact */
		if (make_artifact(o_ptr)) break;
	}


	/* Hack -- analyze artifacts */
	if (o_ptr->name1)
	{
		artifact_type *a_ptr = &a_info[o_ptr->name1];

		/* Hack -- Mark the artifact as "created" */
		a_ptr->cur_num = 1;

		/* Extract the other fields */
		o_ptr->ac = a_ptr->ac;
		o_ptr->dd = a_ptr->dd;
		o_ptr->ds = a_ptr->ds;
		o_ptr->force = a_ptr->force;
		o_ptr->to_a = a_ptr->to_a;
		o_ptr->to_h = a_ptr->to_h;
		o_ptr->to_d = a_ptr->to_d;
		o_ptr->weight = a_ptr->weight;

		/* Extract pvals and related flags */
		o_ptr->pval = a_ptr->pval1;
		o_ptr->flags_pval1 = a_ptr->flags_pval1;
		o_ptr->pval2 = a_ptr->pval2;
		o_ptr->flags_pval2 = a_ptr->flags_pval2;
		o_ptr->pval3 = a_ptr->pval3;
		o_ptr->flags_pval3 = a_ptr->flags_pval3;

		o_ptr->flags1 = a_ptr->flags1;
		o_ptr->flags2 = a_ptr->flags2;
		o_ptr->flags3 = a_ptr->flags3;

		/* Hack -- extract the "broken" flag */
		if (!a_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- extract the "cursed" flag */
		if (a_ptr->flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED);
		/* if (cursed_p(a_ptr)) o_ptr->ident |= (IDENT_CURSED); */

		/* Mega-Hack -- increase the rating */
		rating += 10;

		/* Mega-Hack -- increase the rating again */
		if (a_ptr->cost > 50000L) rating += 10;

		/* Set the good item flag */
		good_item_flag = TRUE;

		/* Cheat -- peek at the item */
		if (cheat_peek) object_mention(o_ptr);

		/* Done */
		return;
	}


	/* Apply magic */
	switch (o_ptr->tval)
	{
		case TV_DIGGING:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_SWORD:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		case TV_GUN:
		case TV_AMMO:
		case TV_BULLET:
		case TV_SHOT:
		{
			if (ABS(power) > 1)
			{
				int ego_power;

				ego_power = make_ego_item(o_ptr, (bool)(good || great));

				if (ego_power) power = ego_power;
			}

			if (power) a_m_aux_1(o_ptr, lev, power);

			break;
		}

		case TV_DRAG_ARMOR:
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_LEG:
		case TV_HELM:
		case TV_CROWN:
		case TV_CLOAK:
		case TV_GLOVES:
		case TV_BOOTS:
		{
			if (ABS(power) > 1)
			{
				int ego_power;

				ego_power = make_ego_item(o_ptr, (bool)(good || great));

				if (ego_power) power = ego_power;
			}

			if (power) a_m_aux_2(o_ptr, lev, power);

			break;
		}
		case TV_MECHA_TORSO:
		case TV_MECHA_HEAD:
		case TV_MECHA_ARMS:
		case TV_MECHA_FEET:
		{
			power = make_ego_item(o_ptr, TRUE);
			a_m_aux_2(o_ptr, lev, power);
			break;
		}
		case TV_RING:
		case TV_AMULET:
		{
			if (!power && (rand_int(100) < 30)) power = -1;
			a_m_aux_3(o_ptr, lev, power);
			break;
		}

		default:
		{
			a_m_aux_4(o_ptr, lev, power);
			break;
		}
	}


	/* Hack -- analyze ego-items */
	if (o_ptr->name2)
	{
		ego_item_type *e_ptr = &e_info[o_ptr->name2];

		/* Extra powers */
		if (e_ptr->xtra)
		{
#if 0 /* this is likely not working at all. */
			if (e_ptr->xtra == OBJECT_XTRA_TYPE_RESIST)
			{
				if (rand_int(3) == 0) o_ptr->xtra1 = OBJECT_XTRA_TYPE_HIGH_RESIST;
				else o_ptr->xtra1 = OBJECT_XTRA_TYPE_MID_RESIST;
			}
			else o_ptr->xtra1 = e_ptr->xtra;

			switch (o_ptr->xtra1)
			{
				case OBJECT_XTRA_TYPE_SUSTAIN:
				{
					o_ptr->xtra2 = (byte)rand_int(OBJECT_XTRA_SIZE_SUSTAIN);
					break;
				}

				case OBJECT_XTRA_TYPE_MID_RESIST:
				{
					/* Mega hack - fire - poison resist */
					o_ptr->xtra2 = (byte)rand_int(RS_TIM);
					break;
				}

				case OBJECT_XTRA_TYPE_HIGH_RESIST:
				{
					/* Mega hack Time - nether resist */
					o_ptr->xtra2 = (byte)rand_int(4) + RS_TIM;
					break;
				}

				case OBJECT_XTRA_TYPE_POWER:
				{
					o_ptr->xtra2 = (byte)rand_int(OBJECT_XTRA_SIZE_POWER);
					break;
				}
			}
#endif
		}

		/* Hack! -- Only allow one, unless item comes in bunches */
		if ((k_info[o_ptr->k_idx].gen_dice *
		     k_info[o_ptr->k_idx].gen_side <= 1))
		{
			o_ptr->number = 1;
		}


		/* Hack -- acquire "broken" flag */
		if (!e_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (e_ptr->flags3 & (TR3_LIGHT_CURSE)) o_ptr->ident |= (IDENT_CURSED); 
		/* if (cursed_p(e_ptr)) o_ptr->ident |= (IDENT_CURSED);*/

		/* Apply extra bonuses or penalties. */
		if (e_ptr->max_to_h != 0)
			o_ptr->to_h += ego_item_bonus(e_ptr->max_to_h);
		if (e_ptr->max_to_d != 0)
			o_ptr->to_d += ego_item_bonus(e_ptr->max_to_d);
		if (e_ptr->max_to_a != 0)
			o_ptr->to_a += ego_item_bonus(e_ptr->max_to_a);

		/* Determine pvals and related flags */
		for (i = 0; i < 2; i++)
		{
			/* Get the right pval and set of flags */
			s16b e_pval =
				((i == 0) ? e_ptr->max_pval1 : e_ptr->max_pval2);
			u32b e_flags_pval =
				((i == 0) ? e_ptr->flags_pval1 : e_ptr->flags_pval2);

			/* Ego-item has no pval, or it doesn't affect anything */
			if (!e_pval || !e_flags_pval) continue;

			/* Pvals vary */
			adjust = rand_range(ABS(e_pval / 2), ABS(e_pval));
			if (e_pval < 0) adjust = -(adjust);

			/* Always have some adjustment */
			if (adjust == 0) adjust = (e_pval > 0 ? 1 : -1);
			
			/* XCCCX _-=*MEGA*=-_ HACK Improve automata pvals based off sval */
			if ((o_ptr->tval >= TV_MECHA_TORSO) && (o_ptr->tval <= TV_MECHA_FEET))
			{
				adjust += o_ptr->sval / 2;
			}

			/* If we already have this attribute, adjust the existing pval */
			if      (o_ptr->flags_pval1 & (e_flags_pval)) o_ptr->pval  += adjust;
			else if (o_ptr->flags_pval2 & (e_flags_pval)) o_ptr->pval2 += adjust;
			else if (o_ptr->flags_pval3 & (e_flags_pval)) o_ptr->pval3 += adjust;

			/* If we don't have this attribute already, we'll have to add it */
			else if (!o_ptr->pval)
			{
				o_ptr->pval = adjust;
				o_ptr->flags_pval1 = e_flags_pval;
			}
			else if (!o_ptr->pval2)
			{
				o_ptr->pval2 = adjust;
				o_ptr->flags_pval2 = e_flags_pval;
			}
			else if (!o_ptr->pval3)
			{
				o_ptr->pval3 = adjust;
				o_ptr->flags_pval3 = e_flags_pval;
			}
		}

		/* Hack -- apply rating bonus */
		rating += e_ptr->rating;

		/* Cheat -- describe the item */
		if (cheat_peek) object_mention(o_ptr);

		/* Done */
		return;
	}


	/* Examine real objects */
	if (o_ptr->k_idx)
	{
		object_kind *k_ptr = &k_info[o_ptr->k_idx];

		/* Hack -- acquire "broken" flag */
		if (!k_ptr->cost) o_ptr->ident |= (IDENT_BROKEN);

		/* Hack -- acquire "cursed" flag */
		if (o_ptr->flags3 & (TR3_LIGHT_CURSE | TR3_HEAVY_CURSE |
		                     TR3_PERMA_CURSE))
		{
			o_ptr->ident |= (IDENT_CURSED);
		}
		/* Item is considered cursed if total plusses have gone down  XXX */
		else if ((o_ptr->to_h + o_ptr->to_d + o_ptr->to_a) <
		         (k_ptr->to_h + k_ptr->to_d + k_ptr->to_a))
		{
			o_ptr->ident |= (IDENT_CURSED);
		}

		/* Objects marked as cursed have (at least) light cursing  XXX */
		if (o_ptr->ident & (IDENT_CURSED)) o_ptr->flags3 |= (TR3_LIGHT_CURSE);

	}
}



/*
 * Hack -- determine if a template is "good".
 *
 * Note that this test only applies to the object *kind*, so it is
 * possible to choose a kind which is "good", and then later cause
 * the actual object to be cursed.  We do explicitly forbid objects
 * which are known to be boring or which start out somewhat damaged.
 */
static bool kind_is_good(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* Analyze the item type */
	switch (k_ptr->tval)
	{
		/* Armor -- Good unless damaged */
		case TV_HARD_ARMOR:
		case TV_SOFT_ARMOR:
		case TV_DRAG_ARMOR:
		case TV_LEG:
		case TV_CLOAK:
		case TV_BOOTS:
		case TV_GLOVES:
		case TV_HELM:
		case TV_CROWN:
		{
			if (k_ptr->to_a < 0) return (FALSE);
			return (TRUE);
		}
		case TV_MECHA_TORSO:
		case TV_MECHA_HEAD:
		case TV_MECHA_ARMS:
		case TV_MECHA_FEET:
		{
			if ((p_ptr->prace == RACE_AUTOMATA) || (p_ptr->prace == RACE_STEAM_MECHA))
			{
				if (k_ptr->to_a < 0) return (FALSE);
				return (TRUE);
			}
			else return (FALSE);
		}

		/* Weapons -- Good unless damaged */
		case TV_GUN:
		case TV_SWORD:
		case TV_HAFTED:
		case TV_POLEARM:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		case TV_DIGGING:
		{
			if (k_ptr->to_h < 0) return (FALSE);
			if (k_ptr->to_d < 0) return (FALSE);
			return (TRUE);
		}

		/* Ammo -- Arrows/Bolts are good */
		case TV_SHOT:
		case TV_BULLET:
		{
			return (TRUE);
		}

		/* Books -- High level books are good */
		/* need to fix this */
		case TV_MAGIC_BOOK:
		{
			if (k_ptr->sval >= SV_BOOK_MIN_GOOD) return (TRUE);
			return (FALSE);
		}

		/* Rings -- Rings of Speed are good */
		case TV_RING:
		{
			if (k_ptr->sval == SV_RING_SPEED) return (TRUE);
			return (FALSE);
		}

		/* Amulets -- Amulets of the Magi are good */
		case TV_AMULET:
		{
			if (k_ptr->sval == SV_AMULET_THE_MAGI) return (TRUE);
			return (FALSE);
		}
	}

	/* Assume not good */
	return (FALSE);
}

/*
 * Hack -- determine if a template is the right kind of object. -LM-
 */
static bool kind_is_right_kind(int k_idx)
{
	object_kind *k_ptr = &k_info[k_idx];

	/* We are only looking for non-worthless items of the tval asked for. */
	if ((k_ptr->tval == required_tval) && (k_ptr->cost > 0)) return (TRUE);
	else return (FALSE);
}




/*
 * Attempt to make an object (normal or good/great)
 *
 * This routine plays nasty games to generate the "special artifacts".
 *
 * This routine uses "object_level" for the "generation level".
 *
 * We assume that the given object has been "wiped".
 */
bool make_object(object_type *j_ptr, bool good, bool great, bool exact_kind)
{
	object_kind *k_ptr;

	int prob, base;
	int k_idx;
	u32b f1, f2, f3;
	u32b p1, p2, p3;
	player_flags(&p1, &p2, &p3);
	
	/* Chance of "special object" */
	prob = (good ? 4 : 20);

	/* Base level for the object */
	base = (good ? (object_level + 10) : object_level);


	/* Attempt to make a special artifact */
	/* This is probably where books instead of ammo are getting made. */
	if ((one_in_(prob)) && (make_artifact_special(j_ptr)) && (exact_kind == FALSE)) 
	{
		/* Process later */
	}

	/* Generate a normal object */
	else
	{
		/* Good objects */
		if (good)
		{
			/* Activate restriction */
			get_obj_num_hook = kind_is_good;

			/* Prepare allocation table */
			get_obj_num_prep();
		}

		/* Objects with a particular tval.  Only activate if there is a 
		 * valid tval to restrict to. */
		if (exact_kind && required_tval)
		{
			/* Activate restriction */
			/* XXX There may be a bug here by not resetting */
			/* get_obj_num_hook */
			get_obj_num_hook = kind_is_right_kind;

			/* Prepare allocation table */
			get_obj_num_prep();
		}

		/* Pick a random object */
		k_idx = get_obj_num(base);

		/*
		 * Clear any special object restrictions, and prepare the standard
		 * object allocation table.
		 */
		if (get_obj_num_hook)
		{
			/* Clear restriction */
			get_obj_num_hook = NULL;

			/* Prepare allocation table */
			get_obj_num_prep();
		}

		/* Handle failure */
		if (!k_idx) return (FALSE);

		/* Prepare the object */
		object_prep(j_ptr, k_idx);
	}

	/* Get the object kind */
	k_ptr = &k_info[j_ptr->k_idx];


	/* Generate multiple items */
	if ((k_ptr->gen_mult_prob) && ((k_ptr->gen_mult_prob >= 100) ||
	    (k_ptr->gen_mult_prob >= randint(100))))
	{
		/* Keep the potions and scrolls under control */
		if ((k_ptr->tval == TV_TONIC) || (k_ptr->tval == TV_MECHANISM))
		{
			/* Require sufficient extra depth */
			if ((p_ptr->depth >= 35) ||
			    (p_ptr->depth >= k_ptr->level + 2 +
			     randint(2 + k_ptr->level / 6)))
			{
				j_ptr->number = damroll(k_ptr->gen_dice, k_ptr->gen_side);
			}
		}
		else
		{
			j_ptr->number = damroll(k_ptr->gen_dice, k_ptr->gen_side);
		}
	}
	
	/* Apply magic (allow artifacts) */
	/* This is where artifacts are handled */
	apply_magic(j_ptr, object_level, TRUE, good, great);

	/* Notice "okay" out-of-depth objects */
	if (!cursed_p(j_ptr) && !broken_p(j_ptr) &&
	    (k_info[j_ptr->k_idx].level > p_ptr->depth))
	{
		/* Rating increase */
		rating += (k_info[j_ptr->k_idx].level - p_ptr->depth);

		/* Cheat -- peek at items */
		if (cheat_peek) object_mention(j_ptr);
	}
	
	/* Extract the flags */
	object_flags(j_ptr, &f1, &f2, &f3);
	
	/* This *should* prevent mecha stuff from being generated */
	/* for non-mecha */
	if ((f3 & (TR3_MECHA_GEN)) && (!(p3 & (TR3_AUTOMATA))))
	{
		return (FALSE);
	}

	/* Success */
	return (TRUE);
}



/*
 * XXX XXX XXX Do not use these hard-coded values.
 */
#define OBJ_GOLD_LIST	950	/* First "gold" entry */
#define MAX_GOLD	18	/* Number of "gold" entries */

/*
 * HACK - make a quest chest
 *
 * This routine plays nasty games to generate the chest.
 *
 * We assume that the given object has been "wiped".
 */
bool make_quest_chest(object_type *j_ptr, bool good, bool great)
{

	/* Prepare a large jeweled chest */
	object_prep(j_ptr, lookup_kind(TV_CHEST, SV_QUEST_CHEST));

	/* Apply magic*/
	apply_magic(j_ptr, object_level, TRUE, good, great);

	/*no items inside, you just return it*/
	j_ptr->xtra1 = 0;

	/*don't allow to open, no traps*/
	j_ptr->pval = 0;

	/*mark the chest as a quest chest*/
	j_ptr->ident |= (IDENT_QUEST);

	return(TRUE);
}


/*
 * Make a treasure object
 *
 * The location must be a legal, clean, floor grid.
 */
bool make_gold(object_type *j_ptr)
{
	int i;

	s32b base;


	/* Hack -- Pick a Treasure variety */
	i = ((randint(object_level + 2) + 2) / 2) - 1;

	/* Apply "extra" magic */
	if (rand_int(GREAT_OBJ) == 0)
	{
		i += randint(object_level + 1);
	}

	/* Hack -- Creeping Coins only generate "themselves" */
	if (coin_type) i = coin_type;

	/* Do not create "illegal" Treasure Types */
	if (i >= MAX_GOLD) i = MAX_GOLD - 1;

	/* Prepare a gold object */
	object_prep(j_ptr, OBJ_GOLD_LIST + i);

	/* Hack -- Base coin cost */
	base = k_info[OBJ_GOLD_LIST+i].cost;

	/* Determine how much the treasure is "worth" */
	j_ptr->pval = (base + (8L * randint(base)) + randint(8));

	/* Success */
	return (TRUE);
}



/*
 * Let the floor carry an object
 */
s16b floor_carry(int y, int x, object_type *j_ptr)
{
	s16b o_idx;

	s16b this_o_idx, next_o_idx = 0;


	/* Scan objects in that grid for combination */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
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

		/* Get the object */
		o_ptr = &o_list[o_idx];

		/* Structure Copy */
		object_copy(o_ptr, j_ptr);

		/* Location */
		o_ptr->iy = y;
		o_ptr->ix = x;

		/* Forget monster */
		o_ptr->held_m_idx = 0;

		/* Link the object to the pile */
		o_ptr->next_o_idx = cave_o_idx[y][x];

		/* Link the floor to the object */
		cave_o_idx[y][x] = o_idx;

		/* Notice */
		note_spot(y, x);

		/* Redraw */
		lite_spot(y, x);
	}

	/* Result */
	return (o_idx);
}


/*
 * Let an object fall to the ground at or near a location.
 *
 * The initial location is assumed to be "in_bounds_fully()".
 *
 * This function takes a parameter "chance".  This is the percentage
 * chance that the item will "disappear" instead of drop.  If the object
 * has been thrown, then this is the chance of disappearance on contact.
 *
 * Hack -- this function uses "chance" to determine if it should produce
 * some form of "description" of the drop event (under the player).
 *
 * We check several locations to see if we can find a location at which
 * the object can combine, stack, or be placed.  Artifacts will try very
 * hard to be placed, including "teleporting" to a useful grid if needed.
 */
void drop_near(object_type *j_ptr, int chance, int y, int x)
{
	int i, k, d, s;

	int bs, bn;
	int by, bx;
	int dy, dx;
	int ty, tx;

	s16b this_o_idx, next_o_idx = 0;

	char o_name[80];

	bool flag = FALSE;

	bool plural = FALSE;


	/* Extract plural */
	if (j_ptr->number != 1) plural = TRUE;

	/* Describe object */
	object_desc(o_name, j_ptr, FALSE, 0);


	/* Handle normal "breakage" */
	if (!artifact_p(j_ptr) && (rand_int(100) < chance))
	{
		/* Potions shatter */
		if (j_ptr->tval == TV_TONIC)
		{
			/* Message */
			msg_format("The %s shatter%s!", o_name, (plural ? "" : "s"));

			/* Smash the potion -- do not learn anything  XXX XXX */
			item_smash_effect(-1, y, x, j_ptr);
		}

		/* Some objects are "shattered" */
		else if ((j_ptr->tval == TV_BOTTLE) || (j_ptr->tval == TV_FLASK))
		{
			msg_format("The %s shatter%s.", o_name, (plural ? "" : "s"));
		}

		/* Some objects are "ruined" */
		else if ((j_ptr->tval == TV_MECHANISM) || (j_ptr->tval == TV_CHEST) ||
		         (j_ptr->tval == TV_FOOD) ||
		         (j_ptr->tval == TV_MAGIC_BOOK))
		{
			msg_format("The %s %s ruined.", o_name, (plural ? "are" : "is"));
		}

		/* Other objects "break" */
		else
		{
			/* Message */
			msg_format("The %s break%s.", o_name, (plural ? "" : "s"));
		}
		/* Debug */
		if (p_ptr->wizard) msg_print("Breakage (breakage).");

		/* Return */
		return;
	}


	/* Score */
	bs = -1;

	/* Picker */
	bn = 0;

	/* Default */
	by = y;
	bx = x;

	/* Scan local grids */
	for (dy = -3; dy <= 3; dy++)
	{
		/* Scan local grids */
		for (dx = -3; dx <= 3; dx++)
		{
			bool comb = FALSE;

			/* Calculate actual distance */
			d = (dy * dy) + (dx * dx);

			/* Ignore distant grids */
			if (d > 10) continue;

			/* Location */
			ty = y + dy;
			tx = x + dx;

			/* Skip illegal grids */
			if (!in_bounds_fully(ty, tx)) continue;

			/* Require line of sight */
			if (!los(y, x, ty, tx)) continue;

			/* Require floor space */
			if (cave_feat[ty][tx] != FEAT_FLOOR) continue;

			/* No objects */
			k = 0;

			/* Scan objects in that grid */
			for (this_o_idx = cave_o_idx[ty][tx]; this_o_idx; this_o_idx = next_o_idx)
			{
				object_type *o_ptr;

				/* Get the object */
				o_ptr = &o_list[this_o_idx];

				/* Get the next object */
				next_o_idx = o_ptr->next_o_idx;

				/* Check for possible combination */
				if (object_similar(o_ptr, j_ptr)) comb = TRUE;

				/* Count objects */
				k++;
			}

			/* Add new object */
			if (!comb) k++;

			/* Paranoia */
			if (k > 99) continue;

			/* Calculate score */
			s = 1000 - (d + k * 5);

			/* Skip bad values */
			if (s < bs) continue;

			/* New best value */
			if (s > bs) bn = 0;

			/* Apply the randomizer to equivalent values */
			if ((++bn >= 2) && (rand_int(bn) != 0)) continue;

			/* Keep score */
			bs = s;

			/* Track it */
			by = ty;
			bx = tx;

			/* Okay */
			flag = TRUE;
		}
	}


	/* Handle lack of space */
	if (!flag && !artifact_p(j_ptr))
	{
		/* Message */
		msg_format("The %s disappear%s.",
		           o_name, (plural ? "" : "s"));

		/* Debug */
		if (p_ptr->wizard) msg_print("Breakage (no floor space).");

		/* Failure */
		return;
	}


	/* Find a grid */
	for (i = 0; !flag; i++)
	{
		/* Bounce around */
		if (i < 1000)
		{
			ty = rand_spread(by, 1);
			tx = rand_spread(bx, 1);
		}

		/* Random locations */
		else
		{
			ty = rand_int(DUNGEON_HGT);
			tx = rand_int(DUNGEON_WID);
		}

		/* Require floor space */
		if (cave_feat[ty][tx] != FEAT_FLOOR) continue;

		/* Bounce to that location */
		by = ty;
		bx = tx;

		/* Require floor space */
		if (!cave_clean_bold(by, bx)) continue;

		/* Okay */
		flag = TRUE;
	}


	/* Give it to the floor */
	if (!floor_carry(by, bx, j_ptr))
	{
		/* Message */
		msg_format("The %s disappear%s.",
		           o_name, (plural ? "" : "s"));

		/* Debug */
		if (p_ptr->wizard) msg_print("Breakage (too many objects).");

		/* Hack -- Preserve artifacts */
		a_info[j_ptr->name1].cur_num = 0;

		/* Failure */
		return;
	}


	/* Sound */
	sound(MSG_DROP);

	/* Mega-Hack -- no message if "dropped" by player */
	/* Message when an object falls under the player */
	if (chance && (cave_m_idx[by][bx] < 0))
	{
		msg_print("You feel something roll beneath your feet.");
	}
}


/*
 * Scatter some "great" objects near the player
 */
void acquirement(int y1, int x1, int num, bool great)
{
	object_type *i_ptr;
	object_type object_type_body;

	/* Acquirement */
	while (num--)
	{
		/* Get local object */
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Make a good (or great) object (if possible) */
		if (!make_object(i_ptr, TRUE, great, FALSE))
		{
			num++;
			continue;
		}

		/* Drop the object */
		drop_near(i_ptr, -1, y1, x1);
	}
}


/*
 * Attempt to place an object (normal or good/great) at the given location.
 */
void place_object(int y, int x, bool good, bool great)
{
	object_type *i_ptr;
	object_type object_type_body;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Hack -- clean floor space */
	if (!cave_clean_bold(y, x)) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(i_ptr);

	/* Make an object (if possible) */
	if (make_object(i_ptr, good, great, FALSE))
	{
		/* Give it to the floor */
		if (!floor_carry(y, x, i_ptr))
		{
			/* Hack -- Preserve artifacts */
			a_info[i_ptr->name1].cur_num = 0;
		}
	}
}

/*
 * Attempt to place a quest_chest at the given location.
 */
void place_quest_chest(int y, int x, bool good, bool great)
{
	object_type *i_ptr;
	object_type object_type_body;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Hack -- clean floor space */
	if (!cave_clean_bold(y, x)) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(i_ptr);

	/* Make a quest chest (should never fail) */
	if (make_quest_chest(i_ptr, good, great))
	{
		/* Give it to the floor */
		floor_carry(y, x, i_ptr);

	}
}

/*
 * Places a treasure (Gold or Gems) at given location
 */
void place_gold(int y, int x)
{
	object_type *i_ptr;
	object_type object_type_body;

	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Require clean floor space */
	if (!cave_clean_bold(y, x)) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Wipe the object */
	object_wipe(i_ptr);

	/* Make some gold */
	if (make_gold(i_ptr))
	{
		/* Give it to the floor */
		(void)floor_carry(y, x, i_ptr);
	}
}



/*
 * Hack -- instantiate a trap
 *
 * XXX XXX XXX This routine should be redone to reflect trap "level".
 * That is, it does not make sense to have spiked pits at 50 feet.
 * Actually, it is not this routine, but the "trap instantiation"
 * code, which should also check for "trap doors" on quest levels.
 */
void pick_trap(int y, int x)
{
	int feat;

	/* Paranoia */
	if (cave_feat[y][x] != FEAT_INVIS) return;

	/* Pick a trap */
	while (1)
	{
		/* Hack -- pick a trap */
		feat = FEAT_TRAP_HEAD + rand_int(16);
		
		if ((p_ptr->depth < 10) && (feat == FEAT_TRAP_HEAD + 0x03)) continue;
		if ((p_ptr->depth < 8) && (feat == FEAT_TRAP_HEAD + 0x0E)) continue;

		/* Hack -- no trap doors on quest levels */
		if ((feat == FEAT_TRAP_HEAD + 0x00) && quest_check(p_ptr->depth)) continue;

		/* Hack -- no trap doors on the deepest level */
		if ((feat == FEAT_TRAP_HEAD + 0x00) && (p_ptr->depth >= MAX_DEPTH-1)) continue;

		/* Done */
		break;
	}

	/* Activate the trap */
	cave_set_feat(y, x, feat);
}



/*
 * Places a random trap at the given location.
 *
 * The location must be a legal, naked, floor grid.
 *
 * Note that all traps start out as "invisible" and "untyped", and then
 * when they are "discovered" (by detecting them or setting them off),
 * the trap is "instantiated" as a visible, "typed", trap.
 */
void place_trap(int y, int x)
{
	/* Paranoia */
	if (!in_bounds(y, x)) return;

	/* Require empty, clean, floor grid */
	if (!cave_naked_bold(y, x)) return;

	/* Place an invisible trap */
	cave_set_feat(y, x, FEAT_INVIS);
}


/*
 * Place a secret door at the given location
 */
void place_secret_door(int y, int x)
{
	/* Create secret door */
	cave_set_feat(y, x, FEAT_SECRET);
}


/*
 * Place a random type of closed door at the given location.
 */
void place_closed_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(400);

	/* Closed doors (300/400) */
	if (tmp < 300)
	{
		/* Create closed door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x00);
	}

	/* Locked doors (99/400) */
	else if (tmp < 399)
	{
		/* Create locked door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + randint(7));
	}

	/* Stuck doors (1/400) */
	else
	{
		/* Create jammed door */
		cave_set_feat(y, x, FEAT_DOOR_HEAD + 0x08 + rand_int(8));
	}
}


/*
 * Place a random type of door at the given location.
 */
void place_random_door(int y, int x)
{
	int tmp;

	/* Choose an object */
	tmp = rand_int(1000);

	/* Open doors (300/1000) */
	if (tmp < 300)
	{
		/* Create open door */
		cave_set_feat(y, x, FEAT_OPEN);
	}

	/* Broken doors (100/1000) */
	else if (tmp < 400)
	{
		/* Create broken door */
		cave_set_feat(y, x, FEAT_BROKEN);
	}

	/* Secret doors (200/1000) */
	else if (tmp < 600)
	{
		/* Create secret door */
		cave_set_feat(y, x, FEAT_SECRET);
	}

	/* Closed, locked, or stuck doors (400/1000) */
	else
	{
		/* Create closed door */
		place_closed_door(y, x);
	}
}


/*
 * Describe the charges on an item in the inventory.
 */
void inven_item_charges(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Require tool/ray gun */
	if ((o_ptr->tval != TV_TOOL) && (o_ptr->tval != TV_RAY)) return;

	/* Require known item */
	if (!object_known_p(o_ptr)) return;

	/* Print a message */
	msg_format("You have %d charge%s remaining.", o_ptr->pval,
	           (o_ptr->pval != 1) ? "s" : "");
}


/*
 * Describe an item in the inventory.
 */
void inven_item_describe(int item)
{
	object_type *o_ptr = &inventory[item];

	char o_name[80];

	if (artifact_p(o_ptr) && object_known_p(o_ptr))
	{
		/* Get a description */
		object_desc(o_name, o_ptr, FALSE, 3);

		/* Print a message */
		msg_format("You no longer have the %s (%c).", o_name, index_to_label(item));
	}
	else
	{
		/* Get a description */
		object_desc(o_name, o_ptr, TRUE, 3);

		/* Print a message */
		msg_format("You have %s (%c).", o_name, index_to_label(item));
	}
}


/*
 * Increase the "number" of an item in the inventory
 */
void inven_item_increase(int item, int num)
{
	object_type *o_ptr = &inventory[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number and weight */
	if (num)
	{
		/* Add the number */
		o_ptr->number += num;

		/* Add the weight */
		p_ptr->total_weight += (num * o_ptr->weight);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate Hit points */
		p_ptr->update |= (PU_HP);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Combine the pack */
		p_ptr->notice |= (PN_COMBINE);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN | PW_EQUIP);
	}
}


/*
 * Erase an inventory slot if it has no more items
 */
void inven_item_optimize(int item)
{
	object_type *o_ptr = &inventory[item];

	/* Only optimize real items */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* The item is in the pack */
	if (item < INVEN_WIELD)
	{
		int i;

		/* One less item */
		p_ptr->inven_cnt--;

		/* Slide everything down */
		for (i = item; i < INVEN_PACK; i++)
		{
			/* Hack -- slide object */
			COPY(&inventory[i], &inventory[i+1], object_type);
		}

		/* Hack -- wipe hole */
		(void)WIPE(&inventory[i], object_type);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* The item is being wielded */
	else
	{
		/* One less item */
		p_ptr->equip_cnt--;

		/* Erase the empty slot */
		object_wipe(&inventory[item]);

		/* Recalculate bonuses */
		p_ptr->update |= (PU_BONUS);

		/* Recalculate torch */
		p_ptr->update |= (PU_TORCH);

		/* Recalculate Hit points */
		p_ptr->update |= (PU_HP);

		/* Recalculate mana */
		p_ptr->update |= (PU_MANA);

		/* Window stuff */
		p_ptr->window |= (PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);
	}
}


/*
 * Describe the charges on an item on the floor.
 */
void floor_item_charges(int item)
{
	object_type *o_ptr = &o_list[item];

	/* Require tool/Ray gun */
	if ((o_ptr->tval != TV_TOOL) && (o_ptr->tval != TV_RAY)) return;

	/* Require known item */
	if (!object_known_p(o_ptr)) return;

	/* Print a message */
	msg_format("There are %d charge%s remaining.", o_ptr->pval,
	           (o_ptr->pval != 1) ? "s" : "");
}



/*
 * Describe an item in the inventory.
 */
void floor_item_describe(int item)
{
	object_type *o_ptr = &o_list[item];

	char o_name[80];

	/* Get a description */
	object_desc(o_name, o_ptr, TRUE, 3);

	/* Print a message */
	msg_format("You see %s.", o_name);
}


/*
 * Increase the "number" of an item on the floor
 */
void floor_item_increase(int item, int num)
{
	object_type *o_ptr = &o_list[item];

	/* Apply */
	num += o_ptr->number;

	/* Bounds check */
	if (num > 255) num = 255;
	else if (num < 0) num = 0;

	/* Un-apply */
	num -= o_ptr->number;

	/* Change the number */
	o_ptr->number += num;
}


/*
 * Optimize an item on the floor (destroy "empty" items)
 */
void floor_item_optimize(int item)
{
	object_type *o_ptr = &o_list[item];

	/* Paranoia -- be sure it exists */
	if (!o_ptr->k_idx) return;

	/* Only optimize empty items */
	if (o_ptr->number) return;

	/* Delete the object */
	delete_object_idx(item);
}


/*
 * Check if we have space for an item in the pack without overflow
 */
bool inven_carry_okay(const object_type *o_ptr)
{
	int j;

	/* Empty slot? */
	if (p_ptr->inven_cnt < INVEN_PACK) return (TRUE);

	/* Similar slot? */
	for (j = 0; j < INVEN_PACK; j++)
	{
		object_type *j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr)) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Add an item to the players inventory, and return the slot used.
 *
 * If the new item can combine with an existing item in the inventory,
 * it will do so, using "object_similar()" and "object_absorb()", else,
 * the item will be placed into the "proper" location in the inventory.
 *
 * This function can be used to "over-fill" the player's pack, but only
 * once, and such an action must trigger the "overflow" code immediately.
 * Note that when the pack is being "over-filled", the new item must be
 * placed into the "overflow" slot, and the "overflow" must take place
 * before the pack is reordered, but (optionally) after the pack is
 * combined.  This may be tricky.  See "dungeon.c" for info.
 *
 * Note that this code must remove any location/stack information
 * from the object once it is placed into the inventory.
 */
s16b inven_carry(object_type *o_ptr)
{
	int i, j, k;
	int n = -1;

	object_type *j_ptr;


	/* Check for combining */
	for (j = 0; j < INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Skip non-objects */
		if (!j_ptr->k_idx) continue;

		/* Hack -- track last item */
		n = j;

		/* Check if the two items can be combined */
		if (object_similar(j_ptr, o_ptr))
		{
			/* Combine the items */
			object_absorb(j_ptr, o_ptr);

			/* Increase the weight */
			p_ptr->total_weight += (o_ptr->number * o_ptr->weight);

			/* Recalculate bonuses */
			p_ptr->update |= (PU_BONUS);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN);

			/* Success */
			return (j);
		}
	}


	/* Paranoia */
	if (p_ptr->inven_cnt > INVEN_PACK) return (-1);


	/* Find an empty slot */
	for (j = 0; j <= INVEN_PACK; j++)
	{
		j_ptr = &inventory[j];

		/* Use it if found */
		if (!j_ptr->k_idx) break;
	}

	/* Use that slot */
	i = j;


	/* Reorder the pack */
	if (i < INVEN_PACK)
	{
		s32b o_value, j_value;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			j_ptr = &inventory[j];

			/* Use empty slots */
			if (!j_ptr->k_idx) break;

			/* Objects sort by decreasing type */
			if (o_ptr->tval > j_ptr->tval) break;
			if (o_ptr->tval < j_ptr->tval) continue;

			/* Non-aware (flavored) items always come last */
			if (!object_aware_p(o_ptr)) continue;
			if (!object_aware_p(j_ptr)) break;

			/* Objects sort by increasing sval */
			if (o_ptr->sval < j_ptr->sval) break;
			if (o_ptr->sval > j_ptr->sval) continue;

			/* Unidentified objects always come last */
			if (!object_known_p(o_ptr)) continue;
			if (!object_known_p(j_ptr)) break;

			/* Rods sort by increasing recharge time */
			if (o_ptr->tval == TV_APPARATUS)
			{
				if (o_ptr->pval < j_ptr->pval) break;
				if (o_ptr->pval > j_ptr->pval) continue;
			}

			/* Wands/tools sort by decreasing charges */
			if ((o_ptr->tval == TV_RAY) || (o_ptr->tval == TV_TOOL))
			{
				if (o_ptr->pval > j_ptr->pval) break;
				if (o_ptr->pval < j_ptr->pval) continue;
			}

			/* Lites sort by decreasing fuel */
			if (o_ptr->tval == TV_LITE)
			{
				if (o_ptr->pval > j_ptr->pval) break;
				if (o_ptr->pval < j_ptr->pval) continue;
			}

			/* Determine the "value" of the pack item */
			j_value = object_value(j_ptr);

			/* Objects sort by decreasing value */
			if (o_value > j_value) break;
			if (o_value < j_value) continue;
		}

		/* Use that slot */
		i = j;

		/* Slide objects */
		for (k = n; k >= i; k--)
		{
			/* Hack -- Slide the item */
			object_copy(&inventory[k+1], &inventory[k]);
		}

		/* Wipe the empty slot */
		object_wipe(&inventory[i]);
	}


	/* Copy the item */
	object_copy(&inventory[i], o_ptr);

	/* Get the new object */
	j_ptr = &inventory[i];

	/* Forget stack */
	j_ptr->next_o_idx = 0;

	/* Forget monster */
	j_ptr->held_m_idx = 0;

	/* Forget location */
	j_ptr->iy = j_ptr->ix = 0;

	/* No longer marked */
	j_ptr->marked = FALSE;

	/* Increase the weight */
	p_ptr->total_weight += (j_ptr->number * j_ptr->weight);

	/* Count the items */
	p_ptr->inven_cnt++;

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Combine and Reorder pack */
	p_ptr->notice |= (PN_COMBINE | PN_REORDER);

	/* Window stuff */
	p_ptr->window |= (PW_INVEN);

	/* Return the slot */
	return (i);
}


/*
 * Take off (some of) a non-cursed equipment item
 *
 * Note that only one item at a time can be wielded per slot.
 *
 * Note that taking off an item when "full" may cause that item
 * to fall to the ground.
 *
 * Return the inventory slot into which the item is placed.
 */
s16b inven_takeoff(int item, int amt)
{
	int slot;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	cptr act;

	char o_name[80];
	
	/* Get the item to take off */
	o_ptr = &inventory[item];

	/* Paranoia */
	if (amt <= 0) return (-1);

	/* Verify */
	if (amt > o_ptr->number) amt = o_ptr->number;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain a local object */
	object_copy(i_ptr, o_ptr);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Describe the object */
	object_desc(o_name, i_ptr, TRUE, 3);

	/* If no bullets weilded */
	if (!o_ptr->k_idx) return (-1);

	/* Took off weapon */
	if (item == INVEN_WIELD)
	{
		act = "You were wielding";
	}

	/* Took off gun */
	else if (item == INVEN_GUN)
	{
		act = "You were holding";
		(void)inven_takeoff(INVEN_LOADEDGUN, 255);
	}

	/* Took off light */
	else if (item == INVEN_LITE)
	{
		act = "You were holding";
	}

	else if (item == INVEN_LOADEDGUN)
	{
		act = "You unloaded";
	}

	/* Took off something */
	else
	{
		act = "You were wearing";
	}

	/* Modify, Optimize */
	inven_item_increase(item, -amt);
	inven_item_optimize(item);

	/* Carry the object */
	slot = inven_carry(i_ptr);

	if ((i_ptr->tval == TV_MECHA_TORSO) ||
	 	(i_ptr->tval == TV_MECHA_HEAD) ||
	 	(i_ptr->tval == TV_MECHA_ARMS) ||
	 	(i_ptr->tval == TV_MECHA_FEET))
	{
	 	p_ptr->total_weight += i_ptr->weight;
	}

	/* Message */
	msg_format("%s %s (%c).", act, o_name, index_to_label(slot));

	/* Return slot */
	return (slot);
}


/*
 * Drop (some of) a non-cursed inventory/equipment item
 *
 * The object will be dropped "near" the current location
 */
void inven_drop(int item, int amt)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[80];


	/* Get the original object */
	o_ptr = &inventory[item];

	/* Error check */
	if (amt <= 0) return;

	/* Not too many */
	if (amt > o_ptr->number) amt = o_ptr->number;


	/* Take off equipment */
	if (item >= INVEN_WIELD)
	{
		/* Take off first */
		item = inven_takeoff(item, amt);

		/* Get the original object */
		o_ptr = &inventory[item];
	}


	/* Get local object */
	i_ptr = &object_type_body;

	/* Obtain local object */
	object_copy(i_ptr, o_ptr);

	/* Distribute charges of wands or rods */
	distribute_charges(o_ptr, i_ptr, amt);

	/* Modify quantity */
	i_ptr->number = amt;

	/* Describe local object */
	object_desc(o_name, i_ptr, TRUE, 3);

	/* Message */
	msg_format("You drop %s (%c).", o_name, index_to_label(item));

	/* Drop it near the player */
	drop_near(i_ptr, 0, py, px);

	/* Modify, Describe, Optimize */
	inven_item_increase(item, -amt);
	inven_item_describe(item);
	inven_item_optimize(item);
}



/*
 * Combine items in the pack
 *
 * Note special handling of the "overflow" slot
 */
void combine_pack(void)
{
	int i, j, k;

	object_type *o_ptr;
	object_type *j_ptr;

	bool flag = FALSE;


	/* Combine the pack (backwards) */
	for (i = INVEN_PACK; i > 0; i--)
	{
		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip empty items */
		if (!o_ptr->k_idx) continue;

		/* Scan the items above that item */
		for (j = 0; j < i; j++)
		{
			/* Get the item */
			j_ptr = &inventory[j];

			/* Skip empty items */
			if (!j_ptr->k_idx) continue;

			/* Can we drop "o_ptr" onto "j_ptr"? */
			if (object_similar(j_ptr, o_ptr))
			{
				/* Take note */
				flag = TRUE;

				/* Add together the item counts */
				object_absorb(j_ptr, o_ptr);

				/* One object is gone */
				p_ptr->inven_cnt--;

				/* Slide everything down */
				for (k = i; k < INVEN_PACK; k++)
				{
					/* Hack -- slide object */
					COPY(&inventory[k], &inventory[k+1], object_type);
				}

				/* Hack -- wipe hole */
				object_wipe(&inventory[k]);

				/* Window stuff */
				p_ptr->window |= (PW_INVEN);

				/* Done */
				break;
			}
		}
	}

	/* Message */
	if (flag) msg_print("You combine some items in your pack.");
}


/*
 * Reorder items in the pack
 *
 * Note special handling of the "overflow" slot
 */
void reorder_pack(void)
{
	int i, j, k;

	s32b o_value;
	s32b j_value;

	object_type *o_ptr;
	object_type *j_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	bool flag = FALSE;


	/* Re-order the pack (forwards) */
	for (i = 0; i < INVEN_PACK; i++)
	{
		/* Mega-Hack -- allow "proper" over-flow */
		if ((i == INVEN_PACK) && (p_ptr->inven_cnt == INVEN_PACK)) break;

		/* Get the item */
		o_ptr = &inventory[i];

		/* Skip empty slots */
		if (!o_ptr->k_idx) continue;

		/* Get the "value" of the item */
		o_value = object_value(o_ptr);

		/* Scan every occupied slot */
		for (j = 0; j < INVEN_PACK; j++)
		{
			/* Get the item already there */
			j_ptr = &inventory[j];

			/* Use empty slots */
			if (!j_ptr->k_idx) break;

			/* Objects sort by decreasing type */
			if (o_ptr->tval > j_ptr->tval) break;
			if (o_ptr->tval < j_ptr->tval) continue;

			/* Non-aware (flavored) items always come last */
			if (!object_aware_p(o_ptr)) continue;
			if (!object_aware_p(j_ptr)) break;

			/* Objects sort by increasing sval */
			if (o_ptr->sval < j_ptr->sval) break;
			if (o_ptr->sval > j_ptr->sval) continue;

			/* Unidentified objects always come last */
			if (!object_known_p(o_ptr)) continue;
			if (!object_known_p(j_ptr)) break;

			/* Rods sort by increasing recharge time */
			if (o_ptr->tval == TV_APPARATUS)
			{
				if (o_ptr->pval < j_ptr->pval) break;
				if (o_ptr->pval > j_ptr->pval) continue;
			}

			/* Wands/tools sort by decreasing charges */
			if ((o_ptr->tval == TV_RAY) || (o_ptr->tval == TV_TOOL))
			{
				if (o_ptr->pval > j_ptr->pval) break;
				if (o_ptr->pval < j_ptr->pval) continue;
			}

			/* Lites sort by decreasing fuel */
			if (o_ptr->tval == TV_LITE)
			{
				if (o_ptr->pval > j_ptr->pval) break;
				if (o_ptr->pval < j_ptr->pval) continue;
			}

			/* Determine the "value" of the pack item */
			j_value = object_value(j_ptr);

			/* Objects sort by decreasing value */
			if (o_value > j_value) break;
			if (o_value < j_value) continue;
		}

		/* Never move down */
		if (j >= i) continue;

		/* Take note */
		flag = TRUE;

		/* Get local object */
		i_ptr = &object_type_body;

		/* Save a copy of the moving item */
		object_copy(i_ptr, &inventory[i]);

		/* Slide the objects */
		for (k = i; k > j; k--)
		{
			/* Slide the item */
			object_copy(&inventory[k], &inventory[k-1]);
		}

		/* Insert the moving item */
		object_copy(&inventory[j], i_ptr);

		/* Window stuff */
		p_ptr->window |= (PW_INVEN);
	}

	/* Message */
	if (flag) msg_print("You reorder some items in your pack.");
}


/*
 * Hack -- display an object kind in the current window
 *
 * Include list of usable spells for readible books
 */
void display_koff(int k_idx)
{
	int y;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[80];


	/* Erase the window */
	for (y = 0; y < Term->hgt; y++)
	{
		/* Erase the line */
		Term_erase(0, y, 255);
	}

	/* No info */
	if (!k_idx) return;


	/* Get local object */
	i_ptr = &object_type_body;

	/* Prepare the object */
	object_wipe(i_ptr);

	/* Prepare the object */
	object_prep(i_ptr, k_idx);


	/* Describe */
	object_desc_store(o_name, i_ptr, FALSE, 0);

	/* Mention the object name */
	Term_putstr(0, 0, -1, TERM_WHITE, o_name);

}

