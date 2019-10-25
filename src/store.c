/* File: store.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



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
	"May Morgoth find you tasty!  Perhaps %s gold pieces?",
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
	msg_print(comment_1[rand_int(MAX_COMMENT_1)]);
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
 * Charisma adjustment runs from 80 to 130
 * Racial adjustment runs from 95 to 130
 *
 * Since greed/charisma/racial adjustments are centered at 100, we need
 * to adjust (by 200) to extract a usable multiplier.  Note that the
 * "greed" value is always something (?).
 */
 /* the values above are not correct. */
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

	/* Add in the charisma factor */
	/* @STAT@ */
	factor += (280 - (p_ptr->stat_use[A_CHR] / 5));

	if (factor > 180) factor = 180;

	/* Shop is buying */
	if (flip)
	{
		/* Adjust for greed */
		adjust = 100 + (300 - (greed + factor));

		/* Never get "silly" */
		if (adjust > 100) adjust = 100;

		/* Mega-Hack -- Black market sucks */
		if (store_num == STORE_B_MARKET) price = price / 2;
		
		if (p_ptr->skills[SK_ART_APPRECIATION].skill_max > 0)
		{
			/* HACK */
			if ((o_ptr->ident & (IDENT_STORE))) price = ((price * (100 - p_ptr->skills[SK_ART_APPRECIATION].skill_rank)) / 100);
			else price = ((price * (100 + p_ptr->skills[SK_ART_APPRECIATION].skill_rank)) / 100);
		}

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

		if (p_ptr->skills[SK_ART_APPRECIATION].skill_max > 0)
		{
			/* HACK */
			price = ((price * (100 - p_ptr->skills[SK_ART_APPRECIATION].skill_rank)) / 100);
		}
	}
	
	if (store_num == STORE_LIBRARY) adjust = 100;

	/* Compute the final price (with rounding) */
	price = (price * adjust + 50L) / 100L;

	/* Note -- Never become "free" */
	if (price <= 0L) return (1L);

	/* Return the price */
	return (price);
}


/*
 * Special "mass production" computation.
 */
static int mass_roll(int num, int max)
{
	int i, t = 0;
	for (i = 0; i < num; i++)
	{
		t += ((max > 1) ? rand_int(max) : 1);
	}
	return (t);
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
			if (cost <= 5L) size += mass_roll(3, 5);
			if (cost <= 20L) size += mass_roll(3, 5);
			break;
		}

		case TV_TONIC:
		case TV_MECHANISM:
		{
			if (cost <= 60L) size += mass_roll(3, 5);
			if (cost <= 240L) size += mass_roll(1, 5);
			break;
		}

		case TV_MAGIC_BOOK:
		{
			if (cost <= 50L) size += mass_roll(2, 3);
			if (cost <= 500L) size += mass_roll(1, 3);
			break;
		}

		case TV_SOFT_ARMOR:
		case TV_HARD_ARMOR:
		case TV_LEG:
		case TV_GLOVES:
		case TV_BOOTS:
		case TV_CLOAK:
		case TV_HELM:
		case TV_CROWN:
		case TV_SWORD:
		case TV_POLEARM:
		case TV_HAFTED:
		case TV_DAGGER:
		case TV_AXES:
		case TV_BLUNT:
		case TV_DIGGING:
		case TV_GUN:
		{
			if (o_ptr->name2) break;
			if (cost <= 10L) size += mass_roll(3, 5);
			if (cost <= 100L) size += mass_roll(3, 5);
			break;
		}

		case TV_SPIKE:
		case TV_AMMO:
		case TV_BULLET:
		case TV_SHOT:
		{
			if (cost <= 5L) size += mass_roll(5, 5);
			if (cost <= 50L) size += mass_roll(5, 5);
			if (cost <= 500L) size += mass_roll(5, 5);
			break;
		}
	}


	/* Pick a discount */
	if (cost < 5)
	{
		discount = 0;
	}
	else if (rand_int(25) == 0)
	{
		discount = 10;
	}
	else if (rand_int(50) == 0)
	{
		discount = 25;
	}
	else if (rand_int(150) == 0)
	{
		discount = 50;
	}
	else if (rand_int(300) == 0)
	{
		discount = 75;
	}
	else if (rand_int(500) == 0)
	{
		discount = 90;
	}


	/* Save the discount */
	o_ptr->discount = discount;

	/* Save the total pile size */
	o_ptr->number = size - (size * discount / 100);
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
	i = (islower(c) ? A2I(c) : -1);

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

	/* Different charges (etc) cannot be stacked, except ray guns */
	if (o_ptr->tval != TV_RAY && o_ptr->pval != j_ptr->pval) return (0);

	/* Require many identical values */
	if (o_ptr->to_h != j_ptr->to_h) return (0);
	if (o_ptr->to_d != j_ptr->to_d) return (0);
	if (o_ptr->to_a != j_ptr->to_a) return (0);

	/* Require identical "artifact" names */
	if (o_ptr->name1 != j_ptr->name1) return (0);

	/* Require identical "ego-item" names */
	if (o_ptr->name2 != j_ptr->name2) return (0);

	/* Hack -- Never stack "powerful" items */
	if (o_ptr->xtra1 || j_ptr->xtra1) return (0);

	/* Hack -- Never stack recharging items */
	if (o_ptr->timeout || j_ptr->timeout) return (0);

	/* Require many identical values */
	if (o_ptr->ac != j_ptr->ac) return (0);
	if (o_ptr->dd != j_ptr->dd) return (0);
	if (o_ptr->ds != j_ptr->ds) return (0);

	/* Hack -- Never stack chests */
	if (o_ptr->tval == TV_CHEST) return (0);

	/* Require matching "discount" fields */
	if (o_ptr->discount != j_ptr->discount) return (0);

	/* They match, so they must be similar */
	return (TRUE);
}


/*
 * Allow a store object to absorb another object
 */
static void store_object_absorb(object_type *o_ptr, object_type *j_ptr)
{
	int total = o_ptr->number + j_ptr->number;

	if (o_ptr->tval == TV_RAY)
	{
		/* just in case the count of charges gets too high */
		o_ptr->pval = (o_ptr->pval + j_ptr->pval < 32768) ? o_ptr->pval + j_ptr->pval: 32768;
	}

	/* Combine quantity, lose excess items */
	o_ptr->number = (total > 99) ? 99 : total;
}


/*
 * Check to see if the shop will be carrying too many objects
 *
 * Note that the shop, just like a player, will not accept things
 * it cannot hold.  Before, one could "nuke" objects this way, by
 * adding them to a pile which was already full.
 *
 * I want to figure out how to have a large number of screens
 * for storage of the books in the library. . . Should
 * check heng code for that here in a second XXXCCCXXX
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
 * Determine if a weapon is 'blessed'
 */
static bool is_blessed(const object_type *o_ptr)
{
	u32b f1, f2, f3;

	/* Get the flags */
	object_flags(o_ptr, &f1, &f2, &f3);

	/* Is the object blessed? */
	return ((f2 & TR2_BLESSED) ? TRUE : FALSE);
}


/*
 * Determine if the current store will purchase the given object
 *
 * Note that a shop-keeper must refuse to buy "worthless" objects
 */
static bool store_will_buy(const object_type *o_ptr)
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
				case TV_AMMO:
				case TV_BULLET:
				case TV_SHOT:
				case TV_DIGGING:
				case TV_CLOAK:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Armoury */
		case STORE_CLOTHING:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_BOOTS:
				case TV_GLOVES:
				case TV_CROWN:
				case TV_HELM:
				case TV_LEG:
				case TV_CLOAK:
				case TV_SOFT_ARMOR:
				case TV_HARD_ARMOR:
				case TV_DRAG_ARMOR:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Weapon Shop */
		case STORE_GUN:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_AMMO:
				case TV_SHOT:
				case TV_BULLET:
				case TV_GUN:
				case TV_DIGGING:
				case TV_HAFTED:
				case TV_POLEARM:
				case TV_SWORD:
				case TV_DAGGER:
				case TV_AXES:
				case TV_BLUNT:
				break;
				default:
				return (FALSE);
			}
			break;
		}

		/* Temple */
		case STORE_MACHINE:
		{
			/* Analyze the type */
			switch (o_ptr->tval)
			{
				case TV_MAGIC_BOOK:
				{
					switch (o_ptr->sval)
					{
						case SV_BOOK_DEVICE1:
						case SV_BOOK_DEVICE2:
						case SV_BOOK_DEVICE3:
						case SV_BOOK_DEVICE4:
						case SV_BOOK_DEVICE5:
						case SV_BOOK_DEVICE6:
						case SV_BOOK_DEVICE7:
						case SV_BOOK_DEVICE8:
						case SV_BOOK_DEVICE9:
						case SV_BOOK_DEVICE10:
						break;
						default:
						return (FALSE);
					}
				}
				case TV_MECHANISM:
				case TV_MECHA_TORSO:
				case TV_MECHA_HEAD:
				case TV_MECHA_ARMS:
				case TV_MECHA_FEET:
				break;
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
				case TV_FOOD:
				case TV_MECHANISM:
				case TV_TONIC:
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
				case TV_MAGIC_BOOK:
				switch (o_ptr->sval)
				{
					case SV_BOOK_MAGE1:
					case SV_BOOK_MAGE2:
					case SV_BOOK_MAGE3:
					case SV_BOOK_MAGE4:
					case SV_BOOK_MAGE5:
					case SV_BOOK_MAGE6:
					case SV_BOOK_MAGE7:
					case SV_BOOK_MAGE8:
					case SV_BOOK_MAGE9:
					case SV_BOOK_MAGE10:
					case SV_BOOK_PRIEST1:
					case SV_BOOK_PRIEST2:
					case SV_BOOK_PRIEST3:
					case SV_BOOK_PRIEST4:
					case SV_BOOK_PRIEST5:
					case SV_BOOK_PRIEST6:
					case SV_BOOK_PRIEST7:
					case SV_BOOK_PRIEST8:
					break;
					default:
					return (FALSE);
				}
				case TV_AMULET:
				case TV_RING:
				case TV_TOOL:
				case TV_RAY:
				case TV_APPARATUS:
				case TV_MECHANISM:
				case TV_TONIC:
				break;
				default:
				return (FALSE);
			}
			break;
		}
		case STORE_LIBRARY:
		{
			switch (o_ptr->tval)
			{
				case TV_BOOK:
				{
					break;
				}
				default:
				return(FALSE);
			}
			break;
		}
		case STORE_WETWARE:
		{
			return(FALSE);
		}
	}

	/* Ignore "worthless" items XXX XXX XXX */
	if (object_value(o_ptr) <= 0) return (FALSE);

	/* Assume okay */
	return (TRUE);
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
	o_ptr->note = 0;

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

	/* Hack -- adjust the maximum timeouts and total charges of rods and wands. */
	if (o_ptr->tval == TV_RAY)
	{
		o_ptr->pval += num * o_ptr->pval / o_ptr->number;
	}


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
	if (o_ptr->name2) return (FALSE);

	/* Good items are never crap */
	if (o_ptr->to_a > 0) return (FALSE);
	if (o_ptr->to_h > 0) return (FALSE);
	if (o_ptr->to_d > 0) return (FALSE);

	/* Check the other stores */
	for (i = 0; i < MAX_STORES; i++)
	{
		/* Skip home and black market */
		if (i == STORE_B_MARKET || i == STORE_HOME)
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


/*
 * Attempt to delete (some of) a random object from the store
 * Hack -- we attempt to "maintain" piles of items when possible.
 */
static void store_delete(void)
{
	int what, num;

	/* Paranoia */
	if (st_ptr->stock_num <= 0) return;

	/* Pick a random slot */
	what = rand_int(st_ptr->stock_num);

	/* Determine how many objects are in the slot */
	num = st_ptr->stock[what].number;
	
#if 0
	if (st_ptr->stock[what].tval == TV_FOOD)
	{
		if (st_ptr->stock[what].sval == SV_FOOD_MEAT_PIE)
		{
			if (num > 60) num = (num + 1) / 2;
			else return;
		}
	}

	else if (st_ptr->stock[what].tval == TV_LITE)
	{
		if (st_ptr->stock[what].sval == SV_LITE_TORCH)
		{
			if (num > 60) num = (num + 1) / 2;
			else return;
		}
	}
	
	else if (st_ptr->stock[what].tval == TV_FLASK)
	{
		if (num > 60) num = (num + 1) / 2;
		else return;
	}
	
	else if ((st_ptr->stock[what].tval == TV_AMMO) ||
		(st_ptr->stock[what].tval == TV_BULLET) ||
		(st_ptr->stock[what].tval == TV_SHOT))
	{
		if (num > 90) num = (num - randint(10));
		else return;
	}
#endif	
	/* Hack -- sometimes, only destroy half the objects */
	if (rand_int(100) < 50) num = (num + 1) / 2;

	/* Hack -- sometimes, only destroy a single object */
	if (rand_int(100) < 50) num = 1;

	/* Actually destroy (part of) the object */
	store_item_increase(what, -num);
	store_item_optimize(what);
}


/*
 * Creates a random object and gives it to a store
 * This algorithm needs to be rethought.  A lot.
 * Currently, "normal" stores use a pre-built array.
 *
 * Note -- the "level" given to "obj_get_num()" is a "favored"
 * level, that is, there is a much higher chance of getting
 * items with a level approaching that of the given level...
 *
 * Should we check for "permission" to have the given object?
 */
static void store_create(void)
{
	int k_idx, tries, level;

	object_type *i_ptr;
	object_type object_type_body;


	/* Paranoia -- no room left */
	if (st_ptr->stock_num >= st_ptr->stock_size) return;


	/* Hack -- consider up to four items */
	for (tries = 0; tries < 4; tries++)
	{
		/* Black Market */
		if (store_num == STORE_B_MARKET)
		{
			/* Pick a level for object/magic */
			level = 15 + rand_int(10) + (p_ptr->lev / 5);

			/* Random object kind (usually of given level) */
			k_idx = get_obj_num(level);

			/* Handle failure */
			if (!k_idx) continue;
		}

		/* Normal Store */
		else
		{
			/* Hack -- Pick an object kind to sell */
			k_idx = st_ptr->table[rand_int(st_ptr->table_num)];

			/* Hack -- fake level for apply_magic() */
			level = rand_range(1, STORE_OBJ_LEVEL);
		}


		/* stores don't generate mecha items either*/
		if ((p_ptr->prace != RACE_AUTOMATA) && 
		(p_ptr->prace != RACE_STEAM_MECHA))
		{
			object_kind *k_ptr = &k_info[k_idx];
			if (k_ptr->flags3 & TR3_MECHA_GEN)
			{
				tries--;
				continue;
			}
		}

		/* Get local object */
		i_ptr = &object_type_body;

		/* Create a new object of the chosen kind */
		object_prep(i_ptr, k_idx);


		/* Apply some "low-level" magic (no artifacts) */
		apply_magic(i_ptr, level, FALSE, FALSE, FALSE);

		/* Hack -- Charge lite's */
		if (i_ptr->tval == TV_LITE)
		{
			if (i_ptr->sval == SV_LITE_TORCH) i_ptr->pval = FUEL_TORCH / 2;
			if (i_ptr->sval == SV_LITE_LANTERN) i_ptr->pval = FUEL_LAMP / 2;
			if (i_ptr->sval == SV_LITE_TAPER) i_ptr->pval = FUEL_TAPER;
			if (i_ptr->sval == SV_LITE_CANDLE_WAX) i_ptr->pval = FUEL_CANDLE;
			if (i_ptr->sval == SV_LITE_CANDLE_TALLOW) i_ptr->pval = FUEL_CANDLE;
		}


		/* The object is "known" */
		object_known(i_ptr);

		/* Mega-Hack -- no chests in stores */
		if (i_ptr->tval == TV_CHEST) continue;

		/* Prune the black market */
		if (store_num == STORE_B_MARKET)
		{
			/* Hack -- No "crappy" items */
			if (black_market_crap(i_ptr)) continue;

			/* Hack -- No "cheap" items */
			if (object_value(i_ptr) < 10) continue;

			/* No "worthless" items */
			/* if (object_value(i_ptr) <= 0) continue; */
		}

		/* Prune normal stores */
		else
		{
			/* No "worthless" items */
			if (object_value(i_ptr) <= 0) continue;
		}


		/* Mass produce and/or Apply discount */
		mass_produce(i_ptr);
		
		/* HACK -- The item came from a store */
		i_ptr->ident |= (IDENT_STORE);

		/* Attempt to carry the (known) object */
		(void)store_carry(i_ptr);

		/* Definitely done */
		break;
	}
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
	if (!((item >= store_top) && (item < store_top + 12))) return;


	/* Get the object */
	o_ptr = &st_ptr->stock[item];

	/* Get the row */
	y = (item % 12);

	/* Label it, clear the line --(-- */
	sprintf(out_val, "%c) ", store_to_label(y));
	prt(out_val, y + 6, 0);

	/* Describe an object in the home */
	if (store_num == STORE_HOME)
	{
		byte attr;

		maxwid = 75;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 10;

		/* Describe the object */
		object_desc(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		if (o_ptr->tval != TV_MAGIC_BOOK) attr = tval_to_attr[o_ptr->tval & 0x7F];
		else
		{
			if (cp_ptr->spell_book[o_ptr->sval]) attr = k_info[o_ptr->k_idx].d_attr;
			else attr = TERM_L_DARK;
		}

		/* Display the object */
		c_put_str(attr, o_name, y + 6, 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of a single object */
			int wgt = o_ptr->weight;
			sprintf(out_val, "%3d.%d lb", wgt / 10, wgt % 10);
			put_str(out_val, y + 6, 68);
		}
	}

	/* Describe an object (fully) in a store */
	else
	{
		byte attr;

		/* Must leave room for the "price" */
		maxwid = 65;

		/* Leave room for weights, if necessary -DRS- */
		if (show_weights) maxwid -= 7;

		/* Describe the object (fully) */
		object_desc_store(o_name, o_ptr, TRUE, 3);
		o_name[maxwid] = '\0';

		/* Get inventory color */
		if (o_ptr->tval != TV_MAGIC_BOOK) attr = tval_to_attr[o_ptr->tval & 0x7F];
		else
		{
			if (cp_ptr->spell_book[o_ptr->sval]) attr = k_info[o_ptr->k_idx].d_attr;
			else attr = TERM_L_DARK;
		}

		/* Display the object */
		c_put_str(attr, o_name, y + 6, 3);

		/* Show weights */
		if (show_weights)
		{
			/* Only show the weight of a single object */
			int wgt = o_ptr->weight;
			sprintf(out_val, "%3d.%d", wgt / 10, wgt % 10);
			put_str(out_val, y + 6, 61);
		}

		/* Display a "fixed" cost */
		if (o_ptr->ident & (IDENT_FIXED))
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			sprintf(out_val, "%9ld F", (long)x);
			put_str(out_val, y + 6, 68);
		}

		/* Display a "taxed" cost */
		else if (auto_haggle)
		{
			/* Extract the "minimum" price */
			x = price_item(o_ptr, ot_ptr->min_inflate, FALSE);

			/* Hack -- Apply Sales Tax if needed */
			/* if (!noneedtobargain(x)) x += x / 10; */

			/* Actually draw the price (with tax) */
			sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, y + 6, 68);
		}

		/* Display a "haggle" cost */
		else
		{
			/* Extrect the "maximum" price */
			x = price_item(o_ptr, ot_ptr->max_inflate, FALSE);

			/* Actually draw the price (not fixed) */
			sprintf(out_val, "%9ld  ", (long)x);
			put_str(out_val, y + 6, 68);
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

	/* Display the next 12 items */
	for (k = 0; k < 12; k++)
	{
		/* Stop when we run out of items */
		if (store_top + k >= st_ptr->stock_num) break;

		/* Display that line */
		display_entry(store_top + k);
	}

	/* Erase the extra lines and the "more" prompt */
	for (i = k; i < 13; i++) prt("", i + 6, 0);

	/* Assume "no current page" */
	put_str("        ", 5, 20);

	/* Visual reminder of "more items" */
	if (st_ptr->stock_num > 12)
	{
		/* Show "more" reminder (after the last object ) */
		prt("-more-", k + 6, 3);

		/* Indicate the "current page" */
		put_str(format("(Page %d)", store_top/12 + 1), 5, 20);
	}
}


/*
 * Display players gold
 */
static void store_prt_gold(void)
{
	char out_val[64];

	prt("Gold Remaining: ", 19, 53);

	sprintf(out_val, "%9ld", (long)p_ptr->au);
	prt(out_val, 19, 68);
}


/*
 * Display store (after clearing screen)
 */
static void display_store(void)
{
	char buf[80];


	/* Clear screen */
	Term_clear();

	/* The "Home" is special */
	if (store_num == STORE_HOME)
	{
		/* Put the owner name */
		put_str("Your Home", 3, 30);

		/* Label the object descriptions */
		put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights)
		{
			put_str("Weight", 5, 70);
		}
	}
	/* Normal stores */
	else
	{
		cptr store_name = (f_name + f_info[FEAT_SHOP_HEAD + store_num].name);
		cptr owner_name = &(b_name[ot_ptr->owner_name]);
		cptr race_name = p_name + p_info[ot_ptr->owner_race].name;

		/* Put the owner name and race */
		sprintf(buf, "%s (%s)", owner_name, race_name);
		put_str(buf, 3, 10);

		/* Show the max price in the store (above prices) */
		sprintf(buf, "%s (%ld)", store_name, (long)(ot_ptr->max_cost));
		if (store_num != STORE_WETWARE) prt(buf, 3, 50);

		/* Label the object descriptions */
		if (store_num != STORE_WETWARE) put_str("Item Description", 5, 3);

		/* If showing weights, show label */
		if (show_weights && (store_num != STORE_WETWARE))
		{
			put_str("Weight", 5, 60);
		}

		/* Label the asking price (in stores) */
		if (store_num != STORE_WETWARE) put_str("Price", 5, 72);
	}

	/* Display the current gold */
	store_prt_gold();

	/* Draw in the inventory */
	display_inventory();
}



/*
 * Get the index of a store object
 *
 * Return TRUE if an object was selected
 */
static bool get_stock(int *com_val, cptr pmt, int i, int j)
{
	int item;

	char which;

	char buf[160];

	char o_name[80];

	char out_val[160];

	object_type *o_ptr;

#ifdef ALLOW_REPEAT

	/* Get the item index */
	if (repeat_pull(com_val))
	{
		/* Verify the item */
		if ((*com_val >= 0) && (*com_val <= (st_ptr->stock_num - 1)))
		{
			/* Success */
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT */

	/* Assume failure */
	*com_val = (-1);

	/* Build the prompt */
	sprintf(buf, "(Items %c-%c, ESC to exit) %s",
	        store_to_label(i), store_to_label(j),
	        pmt);

	/* Ask until done */
	while (TRUE)
	{
		bool verify;

		/* Escape */
		if (!get_com(buf, &which)) return (FALSE);

		/* Note verify */
		verify = (isupper(which) ? TRUE : FALSE);

		/* Lowercase */
		which = tolower(which);

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

		/* Home */
		if (store_num == STORE_HOME)
		{
			/* Describe */
			object_desc(o_name, o_ptr, TRUE, 3);
		}

		/* Shop */
		else
		{
			/* Describe */
			object_desc_store(o_name, o_ptr, TRUE, 3);
		}

		/* Prompt */
		sprintf(out_val, "Try %s? ", o_name);

		/* Query */
		if (!get_check(out_val)) return (FALSE);

		/* Done */
		break;
	}

	/* Save item */
	(*com_val) = item;

#ifdef ALLOW_REPEAT

	repeat_push(*com_val);

#endif /* ALLOW_REPEAT */

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
		sprintf(buf, "%s [accept] ", pmt);
	}

	/* Old (negative) increment, and not final */
	else if (last_inc < 0)
	{
		sprintf(buf, "%s [-%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Old (positive) increment, and not final */
	else if (last_inc > 0)
	{
		sprintf(buf, "%s [+%ld] ", pmt, (long)(ABS(last_inc)));
	}

	/* Normal haggle */
	else
	{
		sprintf(buf, "%s ", pmt);
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
		if (!get_string(buf, out_val, 80)) return (FALSE);

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

			/* Extract a number */
			i = atol(p);

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
			/* final_ask += (final_ask / 10); */
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
			sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
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
	if (noneed || auto_haggle || (final_ask >= purse) || (store_num == STORE_LIBRARY))
	{
		/* Apply Sales Tax (if needed) */
		if (auto_haggle && !noneed)
		{
			final_ask -= final_ask / 10;
		}

		if (store_num == STORE_LIBRARY)
		{
			msg_print("You return the book for safekeeping.");
			message_flush();

			/* Ignore haggling */
			ignore = TRUE;
		}
		
		/* No reason to haggle */
		else if (final_ask >= purse)
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

			sprintf(out_val, "%s :  %ld", pmt, (long)cur_ask);
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

/* 
 * Gain a skill point
 */
static void store_train(void)
{

	do_cmd_gain_level(TRUE);

#if 0
	if (p_ptr->au < (p_ptr->lev * 750)) msg_print("You lack the gold!");
	else 
	{
		/* spend the gold */
		p_ptr->au -= (p_ptr->lev * 750);

		/* Update the display */
		store_prt_gold();

		/* gain a skill point */
		p_ptr->free_skpts += 1;

		/* feedback */
		msg_print("You gain a skill point!");
	}
#endif
}

static void steamware_research(void)
{
	int i, j, x, y;
	int level, selection;
	int research, time_remaining;
	
	part_data *s_ptr;
	
	char buf[160];
	char which;

	x = 1;
	y = 5;
	j = 0;
	
	/* Paranoia */
	level = research = time_remaining = 0;
	
	if (p_ptr->prace == RACE_GHOST)
	{
		msg_print("Ack! I'm haunted!");
		return;
	}
	if ((p_ptr->prace == RACE_AUTOMATA) ||
		(p_ptr->prace == RACE_STEAM_MECHA))
	{
		msg_print("Silly robot! Steamware is for fleshbags!");
		return;
	}
	if (p_ptr->prace == RACE_OLD_ONE)
	{
		msg_print("No way am I working for you! MY BRAIN IS MELTING!!!!");
		return;
	}
	if (p_ptr->prace == RACE_DJINN)
	{
		msg_print("I can't make anything to go in your body!");
		return;
	}
	
	put_str("Name", 5, 5);
	put_str("Cost Research Time (remaining)", 5, 34);
	
	for (i = 0; i < MAX_STEAMWARE_PARTS; i++)
	{
		j++;
		
		/* ugh. this is ugly. Must be a better way */
		if (i == SW_EYES) 
		{
			level = p_ptr->eyes_level;
			time_remaining = p_ptr->eyes_research;
		}
		else if (i == SW_WIRED_REFLEX) 
		{
			level = p_ptr->reflex_level;
			time_remaining = p_ptr->reflex_research;
		}
		else if (i == SW_DERMAL_PLATE)
		{
			level = p_ptr->plate_level;
			time_remaining = p_ptr->plate_research;
		}
		else if (i == SW_FURNACE_CORE)
		{
			level = p_ptr->core_level;
			time_remaining = p_ptr->core_research;
		}
		else if (i == SW_SPURS)
		{
			level = p_ptr->spur_level;
			time_remaining = p_ptr->spur_research;
		}

		s_ptr = &wares[i].data[level];

		prt("", y + j + 1, x);
		put_str(format("  %c) ", I2A(i)), y + j + 1, x);
		put_str(format("%-27s", s_ptr->name), y + j + 1, x + 5);
		put_str(format("%6d %7d~", s_ptr->cost, (s_ptr->research_time + 1000)), y + j + 1, x + 32);
		put_str(format("(%7d)", time_remaining), y + j + 1, x + 50);

	}
		
	/* build a prompt */
	sprintf(buf, "(Select desired research %c-%c, ESC to exit)",
	        store_to_label(0), store_to_label(MAX_STEAMWARE_PARTS - 1));
	
	while (TRUE)
	{
		bool verify;

		/* Escape */
		if (!get_com(buf, &which)) 
		{
			/* Clear screen */
			Term_clear();
			do_cmd_store();
			return;
		}
		/* Note verify */
		verify = (isupper(which) ? TRUE : FALSE);

		/* Lowercase */
		which = tolower(which);

		/* Convert response to item */
		selection = (islower(which) ? A2I(which) : -1);


		/* Oops */
		if ((selection < 0) || (selection > (MAX_STEAMWARE_PARTS-1)))
		{
			/* Oops */
			msg_print("Illegal store object choice!");

			continue;
		}

		if (selection == SW_EYES) 
		{
			level = p_ptr->eyes_level;
			research = p_ptr->eyes_research;
		}
		else if (selection == SW_WIRED_REFLEX) 
		{
			level = p_ptr->reflex_level;
			research = p_ptr->reflex_research;
		}
		else if (selection == SW_DERMAL_PLATE) 
		{
			level = p_ptr->plate_level;
			research = p_ptr->plate_research;
		}
		else if (selection == SW_FURNACE_CORE) 
		{
			level = p_ptr->core_level;
			research = p_ptr->core_research;
		}
		else if (selection == SW_SPURS) 
		{
			level = p_ptr->spur_level;
			research = p_ptr->spur_research;
		}
		
		if (research)
		{
			/* Oops */
			msg_print("I am still researching the current level!");
			clear_from(4);
			return;
		}

		s_ptr = &wares[selection].data[level];
		break;
	}
	if (level == 4)
	{
			/* Oops */
			msg_print("I have completed all the research I can do on that!");
			clear_from(4);
			return;
		
	}
	if (p_ptr->au >= s_ptr->cost)
	{
		if (selection == SW_EYES) p_ptr->eyes_research = s_ptr->research_time + rand_int(2000);
		if (selection == SW_WIRED_REFLEX) p_ptr->reflex_research = s_ptr->research_time + rand_int(2000);
		if (selection == SW_DERMAL_PLATE) p_ptr->plate_research = s_ptr->research_time + rand_int(2000);
		if (selection == SW_FURNACE_CORE) p_ptr->core_research = s_ptr->research_time + rand_int(2000);
		if (selection == SW_SPURS) p_ptr->spur_research = s_ptr->research_time + rand_int(2000);
		
		p_ptr->au -= s_ptr->cost;

		/* Update the display */
		store_prt_gold();

		msg_format("You paid %ld gold. for research on (%c) %s.",
				           (long)s_ptr->cost, store_to_label(selection),
				           s_ptr->name);
		clear_from(4);

	}
	else
	{
		/* Oops */
		msg_print("You lack the gold!");
		clear_from(4);
	}
}

static void steamware_install(void)
{

	int i, x, y;
	int level, selection; 
	int sel_count = 0;
	byte sel_items[5] = {0,0,0,0,0};

	part_data *s_ptr;

	char buf[160];
	char which;
	
	if (p_ptr->prace == RACE_GHOST)
	{
		msg_print("I can't put steam-ware in a body that isn't there!");
		return;
	}
	if ((p_ptr->prace == RACE_AUTOMATA) ||
		(p_ptr->prace == RACE_STEAM_MECHA))
	{
		msg_print("Why don't you just go down to the Machinist Shop!");
		return;
	}
	if (p_ptr->prace == RACE_OLD_ONE)
	{
		msg_print("I guess I could put this here-OHFORTHELOVEOFGODMYEYES-");
		return;
	}
	if (p_ptr->prace == RACE_DJINN)
	{
		msg_print("Maybe you should find a lamp!");
		return;
	}

	x = 1;
	y = 5;
//	j = 0;

	put_str("I can only install the most advanced part available!", 3, 3);

	put_str("Name", 5, 5);
	put_str("Install Price", 5, 34);

	for (i = 0; i < MAX_STEAMWARE_PARTS; i++)
	{
//		j++;
		
		/* ugh. this is ugly. Must be a better way */
		if (i == SW_EYES) 
		{
			level = p_ptr->eyes_level - 1;
			if ((level == 0 && p_ptr->muta6 & MUT6_ALPHA_EYES) ||
				(level == 1 && p_ptr->muta6 & MUT6_BETA_EYES) ||
				(level == 2 && p_ptr->muta6 & MUT6_GAMMA_EYES) ||
				(level == 3 && p_ptr->muta6 & MUT6_DELTA_EYES))
				level = -1;
		}
		else if (i == SW_WIRED_REFLEX) 
		{
			level = p_ptr->reflex_level - 1;
			if ((level == 0 && p_ptr->muta6 & MUT6_ALPHA_REFLEX) ||
				(level == 1 && p_ptr->muta6 & MUT6_BETA_REFLEX) ||
				(level == 2 && p_ptr->muta6 & MUT6_GAMMA_REFLEX) ||
				(level == 3 && p_ptr->muta6 & MUT6_DELTA_REFLEX))
				level = -1;
		}
		else if (i == SW_DERMAL_PLATE)
		{
			level = p_ptr->plate_level - 1;
			if ((level == 0 && p_ptr->muta6 & MUT6_ALPHA_PLATING) ||
				(level == 1 && p_ptr->muta6 & MUT6_BETA_PLATING) ||
				(level == 2 && p_ptr->muta6 & MUT6_GAMMA_PLATING) ||
				(level == 3 && p_ptr->muta6 & MUT6_DELTA_PLATING))
				level = -1;
		}
		else if (i == SW_FURNACE_CORE)
		{
			level = p_ptr->core_level - 1;
			if ((level == 0 && p_ptr->muta6 & MUT6_ALPHA_CORE) ||
				(level == 1 && p_ptr->muta6 & MUT6_BETA_CORE) ||
				(level == 2 && p_ptr->muta6 & MUT6_GAMMA_CORE) ||
				(level == 3 && p_ptr->muta6 & MUT6_DELTA_CORE))
				level = -1;
		}
		else if (i == SW_SPURS)
		{
			level = p_ptr->spur_level - 1;
			if ((level == 0 && p_ptr->muta4 & MUT4_ALPHA_SPURS) ||
				(level == 1 && p_ptr->muta4 & MUT4_BETA_SPURS) ||
				(level == 2 && p_ptr->muta4 & MUT4_GAMMA_SPURS) ||
				(level == 3 && p_ptr->muta4 & MUT4_DELTA_SPURS))
				level = -1;
		}
		
/*		if (level < 0)
		{
			prt("I have nothing to install!", y + j + 1, x);
		}
		else if (level > 4)
		{
			prt("Max level already installed!", y + j + 1, x);
		}
		else 
*/		if (level >= 0)
		{
			s_ptr = &wares[i].data[level];

			sel_items[sel_count] = i;
			sel_count++;
			prt("", y + sel_count + 1, x);
			put_str(format("  %c) ", I2A(sel_count-1)), y + sel_count + 1, x);
			put_str(format("%-27s", s_ptr->name), y + sel_count + 1, x + 5);
			put_str(format("%6d", s_ptr->install_price), y + sel_count + 1, x + 32);
		}
	}
	if (!sel_count)
	{
		msg_print("Nothing to install.");
		return;
	}

	/* build a prompt */
	sprintf(buf, "(Select desired installation %c-%c, ESC to exit)",
	        store_to_label(0), store_to_label(sel_count-1)); //store_to_label(MAX_STEAMWARE_PARTS - 1));

	while (TRUE)
	{
		bool verify;

		/* Escape */
		if (!get_com(buf, &which)) 
		{
			/* Clear screen */
			Term_clear();
			do_cmd_store();
			return;
		}
		/* Note verify */
		verify = (isupper(which) ? TRUE : FALSE);

		/* Lowercase */
		which = tolower(which);

		/* Convert response to item */
		selection = (islower(which) ? A2I(which) : -1);

		/* Oops */
		if ((selection < 0) || (selection > sel_count)) //(MAX_STEAMWARE_PARTS-1)))
		{
			/* Oops */
			msg_print("Illegal store object choice!");

			continue;
		}

		selection = sel_items[selection];

		if (selection == SW_EYES) 
		{
			level = p_ptr->eyes_level;
		}
		else if (selection == SW_WIRED_REFLEX) 
		{
			level = p_ptr->reflex_level;
		}
		else if (selection == SW_DERMAL_PLATE) 
		{
			level = p_ptr->plate_level;
		}
		else if (selection == SW_FURNACE_CORE) 
		{
			level = p_ptr->core_level;
		}
		else if (selection == SW_SPURS) 
		{
			level = p_ptr->spur_level;
		}

		s_ptr = &wares[selection].data[level - 1];
		break;
	}
	if (p_ptr->au >= s_ptr->install_price)
	{
		switch (selection)
		{
		case SW_EYES:
		{
			if (p_ptr->eyes_level == 1) 
			{
				gain_random_mutation(200);
			}
			if (p_ptr->eyes_level == 2) 
			{
				gain_random_mutation(201);
			}
			if (p_ptr->eyes_level == 3) 
			{
				gain_random_mutation(202);
			}
			if (p_ptr->eyes_level == 4) 
			{
				gain_random_mutation(203);
			}
		}
		break;
		case SW_WIRED_REFLEX:
		{
			if (p_ptr->reflex_level == 1) 
			{
				gain_random_mutation(204);
			}
			if (p_ptr->reflex_level == 2) 
			{
				gain_random_mutation(205);
			}
			if (p_ptr->reflex_level == 3) 
			{
				gain_random_mutation(206);
			}
			if (p_ptr->reflex_level == 4) 
			{
				gain_random_mutation(207);
			}
		}
		break;
		case SW_DERMAL_PLATE: 
		{
			if (p_ptr->plate_level == 1) 
			{
				gain_random_mutation(208);
			}
			if (p_ptr->plate_level == 2) 
			{
				gain_random_mutation(209);
			}
			if (p_ptr->plate_level == 3) 
			{
				gain_random_mutation(210);
			}
			if (p_ptr->plate_level == 4) 
			{
				gain_random_mutation(211);
			}
		}
		break;
		case SW_FURNACE_CORE:
		{
			if (p_ptr->core_level == 1) 
			{
				gain_random_mutation(212);
			}
			if (p_ptr->core_level == 2) 
			{
				gain_random_mutation(213);
			}
			if (p_ptr->core_level == 3) 
			{
				gain_random_mutation(214);
			}
			if (p_ptr->core_level == 4) 
			{
				gain_random_mutation(215);
			}
		}
		break;
		case SW_SPURS:
		{
			if (p_ptr->spur_level == 1) 
			{
				gain_random_mutation(216);
			}
			if (p_ptr->spur_level == 2) 
			{
				gain_random_mutation(217);
			}
			if (p_ptr->spur_level == 3) 
			{
				gain_random_mutation(218);
			}
			if (p_ptr->spur_level == 4) 
			{
				gain_random_mutation(219);
			}
		}
		break;
		default: 
			return;
		}
		p_ptr->au -= s_ptr->install_price;

		/* Update the display */
		store_prt_gold();

		msg_format("ARRRRGGGHHH!!! That hurt! You paid %ld gold.", (long)s_ptr->install_price);
		msg_format("(%c) %s is now installed.", store_to_label(selection), s_ptr->name);

		/* Ouch */
		p_ptr->chp = 0;
		take_hit(randint(randint(p_ptr->lev)), "surgery", TRUE);
		clear_from(3);
	}
	else
	{
		/* Oops */
		msg_print("You lack the gold!");
		clear_from(3);
	}

}

/*
 * Buy an object from a store
 */
static void store_purchase(void)
{
	int n, i;
	int amt, choice;
	int item, item_new;

	s32b price;

	object_type *o_ptr;

	object_type *i_ptr;
	object_type object_type_body;

	char o_name[80];

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

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

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
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Get a quantity */
	amt = get_quantity(NULL, o_ptr->number);

	/* Allow user abort */
	if (amt <= 0) return;

	/* Get local object */
	i_ptr = &object_type_body;

	/* Get desired object */
	object_copy(i_ptr, o_ptr);

	/*
	 * Hack -- If a rod or wand, allocate a portion of the total maximum
	 * timeouts or charges.
	 */
	if ((o_ptr->tval == TV_RAY) && (amt < o_ptr->number))
	{
		i_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

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
		object_desc_store(o_name, i_ptr, TRUE, 3);

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
				/* Say "okay" */
				say_comment_1();

				/* Be happy */
				decrease_insults();

				/* Spend the money */
				p_ptr->au -= price;

				/* Update the display */
				store_prt_gold();

				/* Buying an object makes you aware of it */
				object_aware(i_ptr);

				/* Combine / Reorder the pack (later) */
				p_ptr->notice |= (PN_COMBINE | PN_REORDER);

				/* Clear the "fixed" flag from the object */
				i_ptr->ident &= ~(IDENT_FIXED);

				/* Describe the transaction */
				object_desc(o_name, i_ptr, TRUE, 3);

				/* Message */
				msg_format("You bought %s (%c) for %ld gold.",
				           o_name, store_to_label(item),
				           (long)price);

				/* Erase the inscription */
				i_ptr->note = 0;

				/* Remove special inscription, if any */
				if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

				/* Give it to the player */
				item_new = inven_carry(i_ptr);

				/* Describe the final result */
				object_desc(o_name, &inventory[item_new], TRUE, 3);

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
					if (rand_int(STORE_SHUFFLE) == 0)
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

					/* Redraw everything */
					display_inventory();
				}

				/* The object is gone */
				else if (st_ptr->stock_num != n)
				{
					/* Only one screen left */
					if (st_ptr->stock_num <= 12)
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

#if 0

		/* Describe the object */
		object_desc(o_name, i_ptr, TRUE, 3);

		/* Message */
		msg_format("You pick up %s (%c).",
		           o_name, store_to_label(item));

#endif

		/* Give it to the player */
		item_new = inven_carry(i_ptr);

		/* Describe just the result */
		object_desc(o_name, &inventory[item_new], TRUE, 3);

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
			if (st_ptr->stock_num <= 12)
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

	char o_name[80];


	/* Home */
	q = "Drop which item? ";

	/* Real store */
	if (store_num != STORE_HOME)
	{
		/* New prompt */
		q = "Sell which item? ";

		/* Only allow items the store will buy */
		item_tester_hook = store_will_buy;
	}

	p_ptr->command_wrk = (USE_INVEN);
	
	/* Get an item */
	/* There's a bug with selling worn equipment */
	s = "You have nothing that I want.";
	if (!get_item(&item, q, s, (USE_INVEN | USE_EQUIP | USE_FLOOR))) return;

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

	/*
	 * Hack -- If a rod or wand, allocate some of the total maximum
	 * timeouts or charges to those being sold.
	 */
	if (o_ptr->tval == TV_RAY)
	{
		i_ptr->pval = o_ptr->pval * amt / o_ptr->number;
	}

	/* Get a full description */
	object_desc(o_name, i_ptr, TRUE, 3);


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
		if (store_num == STORE_LIBRARY)
			msg_format("Returning %s (%c).", o_name, index_to_label(item));
		else
			msg_format("Selling %s (%c).", o_name, index_to_label(item));
		
		message_flush();

		/* Haggle for it */
		choice = sell_haggle(i_ptr, &price);

		/* Kicked out */
		if (st_ptr->store_open >= turn) return;

		/* Sold... */
		if (choice == 0)
		{
			/* Say "okay" */
			say_comment_1();

			/* Be happy */
			decrease_insults();

			/* Get some money */
			if (store_num == STORE_LIBRARY) 
			{
				p_ptr->free_skpts += randint(price);
				msg_print("You gain some insight!");
				message_flush();
				
				if ((p_ptr->prace == RACE_AUTOMATA) || 
				(p_ptr->prace == RACE_STEAM_MECHA))
				{
					/* nothing */
				}
				else if (rand_int(100) < price) 
				{
					p_ptr->free_sgain++;
					msg_print("You can get stronger!");
					message_flush();
				}
			}
			else p_ptr->au += price;

			/* Update the display */
			store_prt_gold();

			/* Get the "apparent" value */
			dummy = object_value(i_ptr) * i_ptr->number;

			/* Erase the inscription */
			i_ptr->note = 0;

			/* Remove special inscription, if any */
			if (o_ptr->discount >= INSCRIP_NULL) o_ptr->discount = 0;

			/* Identify original object */
			object_aware(o_ptr);
			object_known(o_ptr);
			
			/* HACK -- The item came from a store */
			o_ptr->ident |= (IDENT_STORE);
		

			/* Combine / Reorder the pack (later) */
			p_ptr->notice |= (PN_COMBINE | PN_REORDER);

			/* Window stuff */
			p_ptr->window |= (PW_INVEN | PW_EQUIP | PW_PLAYER_0 | PW_PLAYER_1);

			/* Get local object */
			i_ptr = &object_type_body;

			/* Get a copy of the object */
			object_copy(i_ptr, o_ptr);

			/* Modify quantity */
			i_ptr->number = amt;

			/*
			 * Hack -- Allocate charges between those wands or rods sold
			 * and retained, unless all are being sold.
			 */
			distribute_charges(o_ptr, i_ptr, amt);

			/* Get the "actual" value */
			value = object_value(i_ptr) * i_ptr->number;

			/* Get the description all over again */
			object_desc(o_name, i_ptr, TRUE, 3);

			/* Describe the result (in message buffer) */
			if (store_num != STORE_LIBRARY) 
			{
				msg_format("You sold %s (%c) for %ld gold.",
			           o_name, index_to_label(item), (long)price);
			}
						
			/* Analyze the prices (and comment verbally) */
			purchase_analyze(price, value, dummy);
#if 0
		if (item >= INVEN_WIELD)
		{
				inven_takeoff(item, 255);
		}
		else
		{
#endif			
			/* Take the object from the player */
			inven_item_increase(item, -amt);
			inven_item_describe(item);
			inven_item_optimize(item);

			/* Handle stuff */
			handle_stuff();

#if 0
			/* Take note if we add a new item */
			n = st_ptr->stock_num;
#endif

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
		/* Distribute charges of wands/rods */
		distribute_charges(o_ptr, i_ptr, amt);

		if (item >= INVEN_WIELD)
		{
				inven_takeoff(item, 255);
		}
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
		}
#if 0
		/* Take note if we add a new item */
		n = st_ptr->stock_num;
#endif

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
	int         item, i;
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

	/* Find the number of objects on this and following pages */
	i = (st_ptr->stock_num - store_top);

	/* And then restrict it to the current page */
	if (i > 12) i = 12;

	/* Prompt */
	if (rogue_like_commands)
		sprintf(out_val, "Which item do you want to examine? ");
	else
		sprintf(out_val, "Which item do you want to look at? ");

	/* Get the item number to be examined */
	if (!get_stock(&item, out_val, 0, i - 1)) return;

	/* Get the actual index */
	item = item + store_top;

	/* Get the actual object */
	o_ptr = &st_ptr->stock[item];

	/* Description */
	/* Examine the item. */
	do_cmd_observe(o_ptr, (store_num == STORE_HOME ? FALSE : TRUE));

	/* Describe */
	/* msg_format("Examining %s...", o_name); */
	msg_print("Examining item");

	return;
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
static void store_process_command(void)
{

#ifdef ALLOW_REPEAT

	/* Handle repeating the last command */
	repeat_check();

#endif /* ALLOW_REPEAT */

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
			if (st_ptr->stock_num <= 12)
			{
				/* Nothing to see */
				msg_print("Entire inventory is shown.");
			}
			else 
			{
				store_top += 12;
                if (store_top >= st_ptr->stock_num) store_top = 0;
 
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
			do_cmd_redraw();
			display_store();
			break;
		}

		/* Get (purchase) */
		case 'g':
		{
			if (store_num < STORE_LIBRARY) store_purchase();
			if (store_num == STORE_LIBRARY)
			{
				Term_clear();
				display_guild();
			}
			break;
		}

		/* Drop (Sell) */
		case 'd':
		{
			if (store_num != STORE_WETWARE) store_sell();
			break;
		}

		/* Examine */
		case 'l':
		{
			if (store_num != STORE_WETWARE) store_examine();
			break;
		}
		case 'a':
		/* HACK!!! - Can't find roguelike keybindings for stores */
		case 'z':
		{
			if (store_num == STORE_HOME) store_train();
			if (store_num == STORE_LIBRARY) guild_purchase();
			if (store_num == STORE_WETWARE) steamware_install();
			break;
		}
		case 'r':
		{
			if (store_num == STORE_WETWARE) steamware_research();
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

#if 0

		/* Drop an item */
		case 'd':
		{
			do_cmd_drop();
			break;
		}

#endif

		/* Destroy an item */
		case 'k':
		{
			do_cmd_destroy(0);
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
			do_cmd_observe(NULL, FALSE);
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
			do_cmd_note();
			break;
		}

		/* Version info */
		case 'V':
		{
			do_cmd_version();
			break;
		}

		/* Repeat level feeling */
		case KTRL('F'):
		{
			do_cmd_feeling();
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
			msg_print("That command does not work in stores.");
			break;
		}
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

	int which;

	int tmp_chr;
	char skill_cost[60];
	skill_cost[0] = '\0';
				
	/* Verify a store */
	if (!((cave_feat[py][px] >= FEAT_SHOP_HEAD) &&
	      (cave_feat[py][px] <= FEAT_SHOP_TAIL)))
	{
		msg_print("You see no store here.");
		return;
	}

	/* Hack -- Extract the store code */
	which = (cave_feat[py][px] - FEAT_SHOP_HEAD);

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

	/* Save the store and owner pointers */
	st_ptr = &store[store_num];
	ot_ptr = &b_info[(store_num * z_info->b_max) + st_ptr->owner];


	/* Start at the beginning */
	store_top = 0;

	/* Display the store */
	display_store();

	/* Do not leave */
	leave_store = FALSE;

	strcat(skill_cost, format(" a) Skill Max Up (%6dau).", (p_ptr->lev * 750))); 

	/* Interact with player */
	while (!leave_store)
	{
		/* Hack -- Clear line 1 */
		prt("", 1, 0);

		/* Hack -- Check the charisma */
		tmp_chr = p_ptr->stat_use[A_CHR];

		/* Clear */
		clear_from(21);

		/* Basic commands */
		prt(" ESC) Exit from Building.", 21, 0);

		/* Browse if necessary */
		if (st_ptr->stock_num > 12)
		{
			prt(" SPACE) Next page of stock", 22, 0);
		}

		/* Commands */
		if (store_num < STORE_LIBRARY)
			prt(" g) Get/Purchase an item.", 21, 31);
		if (store_num == STORE_LIBRARY)
			prt(" g) Get quest information.", 21, 31);		
		if (store_num != STORE_WETWARE)
			prt(" d) Drop/Sell an item.", 22, 31);
		if (store_num == STORE_WETWARE)
			prt(" r) Research Steamware.", 21, 31);
		if (store_num == STORE_WETWARE)
			prt(" a) Surgery Installation.", 22, 31);

		if (store_num == STORE_HOME) prt(skill_cost, 23, 31);
		
		/* Add in the eXamine option */
		if (rogue_like_commands && (store_num != STORE_WETWARE))
			prt(" x) eXamine an item.", 21, 56);
		else if (store_num != STORE_WETWARE)
			prt(" l) Look at an item.", 21, 56);
			

		/* Prompt */
		prt("You may: ", 20, 0);

		/* Get a command */
		request_command(TRUE);

		/* Process the command */
		store_process_command();

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
				object_desc(o_name, i_ptr, TRUE, 3);

				/* Message */
				msg_format("You drop %s (%c).", o_name, index_to_label(item));

				/* Remove it from the players inventory */
				inven_item_increase(item, -255);
				inven_item_describe(item);
				inven_item_optimize(item);

				/* Handle stuff */
				handle_stuff();

#if 0
				/* Take note if we add a new item */
				n = st_ptr->stock_num;
#endif
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

		/* Hack -- Handle charisma changes */
		if (tmp_chr != p_ptr->stat_use[A_CHR])
		{
			/* Redisplay wares */
			display_inventory();
		}

		/* Hack -- get kicked out of the store */
		if (st_ptr->store_open >= turn) leave_store = TRUE;
	}


	/* Take a turn */
	p_ptr->energy_use = 100;


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
	p_ptr->window |= (PW_OVERHEAD);
}



/*
 * Shuffle one of the stores.
 */
void store_shuffle(int which)
{
	int i, j;


	/* Ignore home */
	if (which == STORE_HOME) return;


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

	if (store_num == STORE_LIBRARY) return;
	
	/* Discount all the items */
	for (i = 0; i < st_ptr->stock_num; i++)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &st_ptr->stock[i];

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
	int j;

	int old_rating = rating;


	/* Ignore home */
	if (which == STORE_HOME) return;
	if (which == STORE_LIBRARY) return;
	if (which == STORE_WETWARE) return;

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

	/* Hack -- prevent "underflow" */
	if (j < 0) j = 0;

	/* Destroy objects until only "j" slots are left */
	while (st_ptr->stock_num > j) store_delete();


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

	/* Create some new items */
	while (st_ptr->stock_num < j) store_create();


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

