/* File: xtra2.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

#include "angband.h"



/*
 * Set "p_ptr->blind", notice observable changes
 *
 * Note the use of "PU_FORGET_VIEW" and "PU_UPDATE_VIEW", which are needed
 * because "p_ptr->blind" affects the "CAVE_SEEN" flag, and "PU_MONSTERS",
 * because "p_ptr->blind" affects monster visibility, and "PU_MAP", because
 * "p_ptr->blind" affects the way in which many cave grids are displayed.
 */
bool set_blind(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->blind)
		{
			msg_print("You are blind!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blind)
		{
			msg_print("You can see again.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->blind = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Fully update the visuals */
	p_ptr->update |= (PU_FORGET_VIEW | PU_UPDATE_VIEW | PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Redraw the "blind" */
	p_ptr->redraw |= (PR_BLIND);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->confused", notice observable changes
 */
bool set_confused(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->confused)
		{
			msg_print("You are confused!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->confused)
		{
			msg_print("You feel less confused now.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->confused = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the "confused" */
	p_ptr->redraw |= (PR_CONFUSED);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->poisoned", notice observable changes
 */
bool set_poisoned(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->poisoned)
		{
			msg_print("You are poisoned!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->poisoned)
		{
			msg_print("You are no longer poisoned.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->poisoned = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the "poisoned" */
	p_ptr->redraw |= (PR_POISONED);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->afraid", notice observable changes
 */
bool set_afraid(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->afraid)
		{
			msg_print("You are terrified!");
			notice = TRUE;
			p_ptr->counter = FALSE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->afraid)
		{
			msg_print("You feel bolder now.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->afraid = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the "afraid" */
	p_ptr->redraw |= (PR_AFRAID);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->paralyzed", notice observable changes
 */
bool set_paralyzed(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->paralyzed)
		{
			msg_print("You are paralyzed!");
			p_ptr->counter = FALSE;
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->paralyzed)
		{
			msg_print("You can move again.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->paralyzed = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->image", notice observable changes
 *
 * Note the use of "PR_MAP", which is needed because "p_ptr->image" affects
 * the way in which monsters, objects, and some normal grids, are displayed.
 */
bool set_image(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->image)
		{
			msg_print("You feel drugged!");
			notice = TRUE;

			/* Update the monster vis window */
			p_ptr->window |= PW_VISIBLE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->image)
		{
			msg_print("You can see clearly again.");
			notice = TRUE;

			/* Update the monster vis window */
			p_ptr->window |= PW_VISIBLE;
		}
	}

	/* Use the value */
	p_ptr->image = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->fast", notice observable changes
 */
bool set_fast(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->fast)
		{
			msg_print("You feel yourself moving faster!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->fast)
		{
			msg_print("You feel yourself slow down.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->fast = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->slow", notice observable changes
 */
bool set_slow(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->slow)
		{
			msg_print("You feel yourself moving slower!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->slow)
		{
			msg_print("You feel yourself speed up.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->slow = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shield", notice observable changes
 */
bool set_shield(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->shield)
		{
			msg_print("A mystic shield forms around your body!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->shield)
		{
			msg_print("Your mystic shield crumbles away.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shield = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}



/*
 * Set "p_ptr->blessed", notice observable changes
 */
bool set_blessed(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->blessed)
		{
			msg_print("You feel righteous!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->blessed)
		{
			msg_print("The prayer has expired.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->blessed = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate bonuses */
	p_ptr->redraw |= (PR_EXTRA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->hero", notice observable changes
 */
bool set_hero(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->hero)
		{
			msg_print("You feel like a hero!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->hero)
		{
			msg_print("The heroism wears off.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->hero = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate bonuses */
	p_ptr->redraw |= (PR_EXTRA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->shero", notice observable changes
 */
bool set_shero(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->shero)
		{
			msg_print("You feel like a killing machine!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->shero)
		{
			msg_print("You feel less Berserk.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->shero = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Recalculate bonuses */
	p_ptr->redraw |= (PR_EXTRA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->protevil", notice observable changes
 */
bool set_protevil(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->protevil)
		{
			msg_print("You feel safe from evil!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->protevil)
		{
			msg_print("You no longer feel safe from evil.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->protevil = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->redraw |= (PR_EXTRA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}
/*
 * Set "p_ptr->set_shadow", notice observable changes
 */
bool set_shadow(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_wraith)
		{
			msg_print("You leave the physical world!");
			notice = TRUE;

			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD);
			}
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_wraith)
		{
			msg_print("You feel opaque.");
			notice = TRUE;

			{
				/* Redraw map */
				p_ptr->redraw |= (PR_MAP);

				/* Update monsters */
				p_ptr->update |= (PU_MONSTERS);

				/* Window stuff */
				p_ptr->window |= (PW_OVERHEAD);
			}
		}
	}

	/* Use the value */
	p_ptr->tim_wraith = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Redraw the state */
	p_ptr->redraw |= (PR_STATE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->invuln", notice observable changes
 */
bool set_invuln(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->invuln)
		{
			msg_print("You feel invulnerable!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->invuln)
		{
			msg_print("You feel vulnerable once more.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->invuln = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_esp", notice observable changes
 */
bool set_tim_esp(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_esp)
		{
			msg_print("You feel your consciousness expand!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_esp)
		{
			msg_print("Your consciousness contracts again.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_esp = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_invis(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (p_ptr->see_inv)
	{
		p_ptr->tim_invis = 0;
		return FALSE;
	}

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_invis)
		{
			msg_print("Your eyes feel very sensitive!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_invis)
		{
			msg_print("Your eyes feel less sensitive.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_invis = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);
	
	p_ptr->redraw |= (PR_ARMOR);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_light", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_light" affects monster visibility.
 */
bool set_tim_light(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_light)
		{
			msg_print("You are surrounded by light!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_light)
		{
			msg_print("It gets darker.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_light = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_demonspell", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_demonspell" affects monster visibility.
 */
bool set_tim_demonspell(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_demonspell)
		{
			msg_print("You are infused with unholy energy!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_demonspell)
		{
			msg_print("You feel energy leave you.");
			
			/* set mana to 0; */
			p_ptr->csp = 0;
			notice = TRUE;
			
		}
	}

	/* Use the value */
	p_ptr->tim_demonspell = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);
	
	p_ptr->update |= (PU_MANA);
	p_ptr->redraw |= (PR_MANA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_demonhealth(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_demonhealth)
		{
			msg_print("You are infused with unholy power!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_demonhealth)
		{
			msg_print("You feel energy leave you.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_demonhealth = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_wormsense(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_wormsense)
		{
			msg_print("You are infused with unholy power!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_wormsense)
		{
			msg_print("You feel energy leave you.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_wormsense = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_voorish(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_voorish)
		{
			msg_print("You are attuned to dark power!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_voorish)
		{
			msg_print("You feel less attuned.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_voorish = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	p_ptr->update |= (PU_MANA);
	p_ptr->redraw |= (PR_MANA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_stygian(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_stygian)
		{
			msg_print("You are attuned to dark power!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_stygian)
		{
			msg_print("You feel less attuned.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_stygian = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_muscle(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_muscle)
		{
			msg_print("You feel very strong!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_muscle)
		{
			msg_print("You feel less strong.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_muscle = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}
/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_vigor(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_vigor)
		{
			msg_print("You feel very healthy!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_vigor)
		{
			msg_print("You feel less healthy.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_vigor = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_no_tele(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_no_tele)
		{
			msg_print("You feel very stable!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_no_tele)
		{
			msg_print("You no longer feel very stable.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_no_tele = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_free_act(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_free_act)
		{
			msg_print("Your motion feels protected!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_free_act)
		{
			msg_print("You motion no longer feels protected.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_free_act = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_anti_magic(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_anti_magic)
		{
			msg_print("Magic energy is stifled!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_anti_magic)
		{
			msg_print("Magic energy begins to flow again.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_anti_magic = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_infra", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because because
 * "p_ptr->tim_infra" affects monster visibility.
 */
bool set_tim_infra(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_infra)
		{
			msg_print("Your eyes begin to tingle!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_infra)
		{
			msg_print("Your eyes stop tingling.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_infra = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}
/*
 * Set "p_ptr->tim_harding", notice observable changes
 * Steam-Mecha only
 */
bool set_tim_harding(int v)
{
	bool notice = FALSE;
	
	/* hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	if (v)
	{
		if (!p_ptr->tim_harding)
		{
			msg_print("Voltaic-Magneto shielding activated!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_harding)
		{
			msg_print("Voltaic-Magneto shielding shutting down.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_harding = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);
	
	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invisiblity", notice observable changes
 */
bool set_tim_invisiblity(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_invisiblity)
		{
			msg_print("You are invisible!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_invisiblity)
		{
			msg_print("You feel visible now.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_invisiblity = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "confused" */
	p_ptr->redraw |= (PR_ARMOR);
	
	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_invis", notice observable changes
 *
 * Note the use of "PU_MONSTERS", which is needed because
 * "p_ptr->tim_image" affects monster visibility.
 */
bool set_tim_evade(int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_evade)
		{
			msg_print("You are dodgeing monsters!");
			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_evade)
		{
			msg_print("You stop dodging.");
			notice = TRUE;
		}
	}

	/* Use the value */
	p_ptr->tim_evade = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Update the monsters XXX */
	p_ptr->update |= (PU_MONSTERS);

	p_ptr->update |= (PU_MANA);
	p_ptr->redraw |= (PR_MANA);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->eyes_research", notice observable changes
 */
bool set_tim_eyes_research(int v)
{
	bool notice = FALSE;

	/* Open */
	if (v)
	{
		/* nothing */
	}

	/* Shut */
	else
	{
		if (p_ptr->eyes_research)
		{
			msg_print("Your steamware eyes research is finished.");
			notice = TRUE;
			if (p_ptr->eyes_level < MAX_UPGRADE - 1) p_ptr->eyes_level++;
		}
	}

	/* Use the value */
	p_ptr->eyes_research = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->eyes_research", notice observable changes
 */
bool set_tim_reflex_research(int v)
{
	bool notice = FALSE;

	/* Open */
	if (v)
	{
		/* nothing */
	}

	/* Shut */
	else
	{
		if (p_ptr->reflex_research)
		{
			msg_print("Your steamware wired reflex research is finished.");
			notice = TRUE;
			if (p_ptr->reflex_level < MAX_UPGRADE - 1) p_ptr->reflex_level++;
		}
	}

	/* Use the value */
	p_ptr->reflex_research = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->eyes_research", notice observable changes
 */
bool set_tim_plate_research(int v)
{
	bool notice = FALSE;

	/* Open */
	if (v)
	{
		/* nothing */
	}

	/* Shut */
	else
	{
		if (p_ptr->plate_research)
		{
			msg_print("Your steamware dermal plate research is finished.");
			notice = TRUE;
			if (p_ptr->plate_level < MAX_UPGRADE - 1) p_ptr->plate_level++;
		}
	}

	/* Use the value */
	p_ptr->plate_research = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->eyes_research", notice observable changes
 */
bool set_tim_core_research(int v)
{
	bool notice = FALSE;

	/* Open */
	if (v)
	{
		/* nothing */
	}

	/* Shut */
	else
	{
		if (p_ptr->core_research)
		{
			msg_print("Your steamware furnace core research is finished.");
			notice = TRUE;
			if (p_ptr->core_level < MAX_UPGRADE - 1) p_ptr->core_level++;
		}
	}

	/* Use the value */
	p_ptr->core_research = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->eyes_research", notice observable changes
 */
bool set_tim_spur_research(int v)
{
	bool notice = FALSE;

	/* Open */
	if (v)
	{
		/* nothing */
	}

	/* Shut */
	else
	{
		if (p_ptr->spur_research)
		{
			msg_print("Your steamware spur research is finished.");
			notice = TRUE;
			if (p_ptr->spur_level < MAX_UPGRADE - 1) p_ptr->spur_level++;
		}
	}

	/* Use the value */
	p_ptr->spur_research = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}

/*
 * Set "p_ptr->tim_res[type]", notice observable changes
 */
bool set_tim_res(int type, int v)
{
	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Open */
	if (v)
	{
		if (!p_ptr->tim_res[type])
		{
			if (p_ptr->dis_res[type])
				message_format(MSG_GENERIC, 0, 
				"You feel more resistant to %s!", resist_names[type]);
			else
				message_format(MSG_GENERIC, 0, 
				"You feel resistant to %s!", resist_names[type]);

			notice = TRUE;
		}
	}

	/* Shut */
	else
	{
		if (p_ptr->tim_res[type])
		{
			/* Only notice if it makes some difference */
			if (p_ptr->dis_res[type] <= MAX_USEFUL_RESIST)
			{
				if (p_ptr->dis_res[type] <= TEMP_RES_BONUS)
					message_format(MSG_GENERIC, 0, 
					"You no longer feel resistant to %s.", resist_names[type]);
				else
					message_format(MSG_GENERIC, 0, 
					"You feel less resistant to %s.", resist_names[type]);
				
				notice = TRUE;
			}
		}
	}

	/* Use the value */
	p_ptr->tim_res[type] = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Window stuff */
	/* p_ptr->window |= (PW_CONDITION); */

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->stun", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_stun(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Knocked out */
	if (p_ptr->stun > 100)
	{
		old_aux = 3;
	}

	/* Heavy stun */
	else if (p_ptr->stun > 50)
	{
		old_aux = 2;
	}

	/* Stun */
	else if (p_ptr->stun > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Knocked out */
	if (v > 100)
	{
		new_aux = 3;
	}

	/* Heavy stun */
	else if (v > 50)
	{
		new_aux = 2;
	}

	/* Stun */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Stun */
			case 1:
			{
				msg_print("You have been stunned.");
				break;
			}

			/* Heavy stun */
			case 2:
			{
				msg_print("You have been heavily stunned.");
				break;
			}

			/* Knocked out */
			case 3:
			{
				msg_print("You have been knocked out.");
				break;
			}
		}

		/* Notice */
		notice = TRUE;
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
			{
				msg_print("You are no longer stunned.");
				if (disturb_state) disturb(0, 0);
				break;
			}
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->stun = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "stun" */
	p_ptr->redraw |= (PR_STUN);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->cut", notice observable changes
 *
 * Note the special code to only notice "range" changes.
 */
bool set_cut(int v)
{
	int old_aux, new_aux;

	bool notice = FALSE;

	/* Hack -- Force good values */
	v = (v > 10000) ? 10000 : (v < 0) ? 0 : v;

	/* Mortal wound */
	if (p_ptr->cut > 1000)
	{
		old_aux = 7;
	}

	/* Deep gash */
	else if (p_ptr->cut > 200)
	{
		old_aux = 6;
	}

	/* Severe cut */
	else if (p_ptr->cut > 100)
	{
		old_aux = 5;
	}

	/* Nasty cut */
	else if (p_ptr->cut > 50)
	{
		old_aux = 4;
	}

	/* Bad cut */
	else if (p_ptr->cut > 25)
	{
		old_aux = 3;
	}

	/* Light cut */
	else if (p_ptr->cut > 10)
	{
		old_aux = 2;
	}

	/* Graze */
	else if (p_ptr->cut > 0)
	{
		old_aux = 1;
	}

	/* None */
	else
	{
		old_aux = 0;
	}

	/* Mortal wound */
	if (v > 1000)
	{
		new_aux = 7;
	}

	/* Deep gash */
	else if (v > 200)
	{
		new_aux = 6;
	}

	/* Severe cut */
	else if (v > 100)
	{
		new_aux = 5;
	}

	/* Nasty cut */
	else if (v > 50)
	{
		new_aux = 4;
	}

	/* Bad cut */
	else if (v > 25)
	{
		new_aux = 3;
	}

	/* Light cut */
	else if (v > 10)
	{
		new_aux = 2;
	}

	/* Graze */
	else if (v > 0)
	{
		new_aux = 1;
	}

	/* None */
	else
	{
		new_aux = 0;
	}

	/* Increase cut */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Graze */
			case 1:
			{
				msg_print("You have been given a graze.");
				break;
			}

			/* Light cut */
			case 2:
			{
				msg_print("You have been given a light cut.");
				break;
			}

			/* Bad cut */
			case 3:
			{
				msg_print("You have been given a bad cut.");
				break;
			}

			/* Nasty cut */
			case 4:
			{
				msg_print("You have been given a nasty cut.");
				break;
			}

			/* Severe cut */
			case 5:
			{
				msg_print("You have been given a severe cut.");
				break;
			}

			/* Deep gash */
			case 6:
			{
				msg_print("You have been given a deep gash.");
				break;
			}

			/* Mortal wound */
			case 7:
			{
				msg_print("You have been given a mortal wound.");
				break;
			}
		}

		/* Notice */
		notice = TRUE;
	}

	/* Decrease cut */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* None */
			case 0:
			{
				if ((p_ptr->prace == RACE_AUTOMATA) || 
					(p_ptr->prace == RACE_STEAM_MECHA))
				{
					msg_print("You are no longer leaking.");			
				}
				else msg_print("You are no longer bleeding.");
				if (disturb_state) disturb(0, 0);
				break;
			}
		}

		/* Notice */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->cut = v;

	/* No change */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw the "cut" */
	p_ptr->redraw |= (PR_CUT);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}


/*
 * Set "p_ptr->food", notice observable changes
 *
 * The "p_ptr->food" variable can get as large as 20000, allowing the
 * addition of the most "filling" item, Elvish Waybread, which adds
 * 7500 food units, without overflowing the 32767 maximum limit.
 *
 * Perhaps we should disturb the player with various messages,
 * especially messages about hunger status changes.  XXX XXX XXX
 *
 * Digestion of food is handled in "dungeon.c", in which, normally,
 * the player digests about 20 food units per 100 game turns, more
 * when "fast", more when "regenerating", less with "slow digestion",
 * but when the player is "gorged", he digests 100 food units per 10
 * game turns, or a full 1000 food units per 100 game turns.
 *
 * Note that the player's speed is reduced by 10 units while gorged,
 * so if the player eats a single food ration (5000 food units) when
 * full (15000 food units), he will be gorged for (5000/100)*10 = 500
 * game turns, or 500/(100/5) = 25 player turns (if nothing else is
 * affecting the player speed).
 */
bool set_food(int v)
{
	int old_aux, new_aux;
	u32b f1, f2, f3;
	bool notice = FALSE;
	
	player_flags(&f1, &f2, &f3);


	/* Hack -- Force good values */
	v = (v > 20000) ? 20000 : (v < 0) ? 0 : v;

	/* Fainting / Starving */
	if (p_ptr->food < PY_FOOD_FAINT)
	{
		old_aux = 0;
	}

	/* Weak */
	else if (p_ptr->food < PY_FOOD_WEAK)
	{
		old_aux = 1;
	}

	/* Hungry */
	else if (p_ptr->food < PY_FOOD_ALERT)
	{
		old_aux = 2;
	}

	/* Normal */
	else if (p_ptr->food < PY_FOOD_FULL)
	{
		old_aux = 3;
	}

	/* Full */
	else if (p_ptr->food < PY_FOOD_MAX)
	{
		old_aux = 4;
	}

	/* Gorged */
	else
	{
		old_aux = 5;
	}

	/* Fainting / Starving */
	if (v < PY_FOOD_FAINT)
	{
		new_aux = 0;
	}

	/* Weak */
	else if (v < PY_FOOD_WEAK)
	{
		new_aux = 1;
	}

	/* Hungry */
	else if (v < PY_FOOD_ALERT)
	{
		new_aux = 2;
	}

	/* Normal */
	else if (v < PY_FOOD_FULL)
	{
		new_aux = 3;
	}

	/* Full */
	else if (v < PY_FOOD_MAX)
	{
		new_aux = 4;
	}

	/* Gorged */
	else
	{
		new_aux = 5;
	}

	/* Food increase */
	if (new_aux > old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Weak */
			case 1:
			{
				msg_print("You are still weak.");
				break;
			}

			/* Hungry */
			case 2:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("You are still in need of oil.");
				}
				else msg_print("You are still hungry.");
				break;
			}

			/* Normal */
			case 3:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("Your engine has sufficient lubricant.");
				}
				else msg_print("You are no longer hungry.");
				break;
			}

			/* Full */
			case 4:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("Your engine is in tip-top shape!");
				}
				else msg_print("You are full!");
				break;
			}

			/* Bloated */
			case 5:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("You are coated in oil making it difficult to move!");
				}
				else msg_print("You have gorged yourself!");
				break;
			}
		}

		/* Change */
		notice = TRUE;
	}

	/* Food decrease */
	else if (new_aux < old_aux)
	{
		/* Describe the state */
		switch (new_aux)
		{
			/* Fainting / Starving */
			case 0:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("You engine is going to seize up!");
				}
				else msg_print("You are getting faint from hunger!");
				break;
			}

			/* Weak */
			case 1:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("Your engine begins to grind!");
				}
				else msg_print("You are getting weak from hunger!");
				break;
			}

			/* Hungry */
			case 2:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("Your engine needs some oil.");
				}
				else  msg_print("You are getting hungry.");
				break;
			}

			/* Normal */
			case 3:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("Your engine warms up.");
				}
				else msg_print("You are no longer full.");
				break;
			}

			/* Full */
			case 4:
			{
				if (f3 & (TR3_AUTOMATA))
				{
					msg_print("The excess oil wears off.");
				}
				else msg_print("You are no longer gorged.");
				break;
			}
		}

		/* Change */
		notice = TRUE;
	}

	/* Use the value */
	p_ptr->food = v;

	/* Nothing to notice */
	if (!notice) return (FALSE);

	/* Disturb */
	if (disturb_state) disturb(0, 0);

	/* Recalculate bonuses */
	p_ptr->update |= (PU_BONUS);

	/* Redraw hunger */
	p_ptr->redraw |= (PR_HUNGER);

	/* Handle stuff */
	handle_stuff();

	/* Result */
	return (TRUE);
}





/*
 * Advance experience levels and print experience
 */
void check_experience(void)
{
	int pre_lgain;
	int post_lgain;
	int bonus_skillpts, bskilllevel_factor;
	int i;
	u32b f1, f2, f3;
	player_flags(&f1, &f2, &f3);

	
	/* Hack -- lower limit */
	if (p_ptr->exp < 0) p_ptr->exp = 0;

	/* Hack -- lower limit */
	if (p_ptr->max_exp < 0) p_ptr->max_exp = 0;

	/* Hack -- upper limit */
	if (p_ptr->exp > PY_MAX_EXP) p_ptr->exp = PY_MAX_EXP;

	/* Hack -- upper limit */
	if (p_ptr->max_exp > PY_MAX_EXP) p_ptr->max_exp = PY_MAX_EXP;


	/* Hack -- maintain "max" experience */
	if (p_ptr->exp > p_ptr->max_exp) p_ptr->max_exp = p_ptr->exp;

	/* Redraw experience */
	p_ptr->redraw |= (PR_EXP);

	/* Handle stuff */
	handle_stuff();


	/* Lose levels while possible */
	while ((p_ptr->lev > 1) &&
	       (p_ptr->exp < (player_exp[p_ptr->lev-2] *
	                      p_ptr->expfact / 100L)))
	{
		/* Lose a level */
		p_ptr->lev--;

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);

		/* Handle stuff */
		handle_stuff();
	}


	/* Gain levels while possible */
	while ((p_ptr->lev < PY_MAX_LEVEL) &&
	       (p_ptr->exp >= (player_exp[p_ptr->lev-1] *
	                       p_ptr->expfact / 100L)))
	{
		/* Gain a level */
		p_ptr->lev++;
		
		/* Get bonus skilpoints - lower levels give more */
		bskilllevel_factor = (40 - p_ptr->lev) / 10;
		
		/* Paranoia */
		if (bskilllevel_factor < 0) bskilllevel_factor = 0;
			
		/* determine bonus skill points */
		bonus_skillpts = rand_int(2) + 1;
		
		bonus_skillpts += bskilllevel_factor;
		/* bonus_skillpts += (50 - p_ptr->lev) / 10; */

		/* Get the value of max_lev */
		pre_lgain = p_ptr->max_lev;

		/* Save the highest level */
		if (p_ptr->lev > p_ptr->max_lev) p_ptr->max_lev = p_ptr->lev;
		
		/* Get the value of max_lev post checking to see if it increases */
		post_lgain = p_ptr->max_lev;
		
		/* Do stuff on level gain. If max_lev went up - then run level reward */
		/* I attempted to put this in the next loop, but . . . It never triggered */
		/* I should probably look into a better way to do this. */
		if (pre_lgain < post_lgain) 
		{
			if (f3 & (TR3_AUTOMATA))
			{
				/* Do nothing */
			}
			else 
			{
				/* mostly you get one free stat gain a level */
				/* sometimes you get 0-3 (should a skill affect this?)*/
				if (randint(100) < 95)
				{
					p_ptr->free_sgain += 1;
				}
				else 
				{
					/* Note the possiblity of 0 */
					p_ptr->free_sgain += rand_int(3);
				}
			}
			/* gain 3-5 skill points per level */
			p_ptr->free_skpts += (1 + bonus_skillpts);
		
			if (!rand_int(1000))
			{
				/* Gain the Luck skill */
				if (p_ptr->skills[SK_LUCK].skill_max == -2)
				{
					p_ptr->skills[SK_LUCK].skill_rank = p_ptr->skills[SK_LUCK].skill_max = p_ptr->skills[SK_LUCK].skill_raise = 1;
				}
			}
			/* Allow skills to be raised */
			for (i = 0; i < N_SKILLS; i++)
			{
				/* if the value in the of the skill */
				if (p_ptr->skills[i].skill_max > 0)
				{
					/* set skill_rank, and skill_max to one */
					p_ptr->skills[i].skill_raise += 1;		 
				}
			}
 		}

		/* Message */
		message_format(MSG_LEVEL, p_ptr->lev, "Welcome to level %d.", p_ptr->lev);

		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);


		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);
		
		/* Redraw Level status */
		p_ptr->redraw |= (PR_LEVEL);

		/* Handle stuff */
		handle_stuff();
		heal_player(100, 100);
		wp_player(500);
		heal_player_sp(100, 100);
	}

	/* Gain max levels while possible */
	while ((p_ptr->max_lev < PY_MAX_LEVEL) &&
	       (p_ptr->max_exp >= (player_exp[p_ptr->max_lev-1] *
	                           p_ptr->expfact / 100L)))
	{

		/* Gain max level */
		p_ptr->max_lev++;		
		
		/* Update some stuff */
		p_ptr->update |= (PU_BONUS | PU_HP | PU_MANA | PU_SPELLS);

		/* Redraw some stuff */
		p_ptr->redraw |= (PR_LEV | PR_TITLE | PR_EXP);

		/* Window stuff */
		p_ptr->window |= (PW_PLAYER_0 | PW_PLAYER_1);

		/* Handle stuff */
		handle_stuff();
	}
}


/*
 * Gain experience
 */
void gain_exp(s32b amount)
{
	/* Gain some experience */
	p_ptr->exp += amount;

/* HACK!! XCCCX */
/* commented out because level gain doesn't trigger correctly */
#if 0
	/* Slowly recover from experience drainage */
	if (p_ptr->exp < p_ptr->max_exp)
	{
		/* Gain max experience (10%) */
		p_ptr->max_exp += amount / 10;
	}
#endif
	/* Check Experience */
	check_experience();
}


/*
 * Lose experience
 */
void lose_exp(s32b amount)
{
	/* Never drop below zero experience */
	if (amount > p_ptr->exp) amount = p_ptr->exp;

	/* Lose some experience */
	p_ptr->exp -= amount;

	/* Check Experience */
	check_experience();
}




/*
 * Hack -- Return the "automatic coin type" of a monster race
 * Used to allocate proper treasure when "Creeping coins" die
 *
 * Note the use of actual "monster names".  XXX XXX XXX
 */
static int get_coin_type(const monster_race *r_ptr)
{
	cptr name = (r_name + r_ptr->name);

	/* Analyze "coin" monsters */
	if (r_ptr->d_char == '$')
	{
		/* Look for textual clues */
		if (strstr(name, " copper ")) return (2);
		if (strstr(name, " silver ")) return (5);
		if (strstr(name, " gold ")) return (10);
		if (strstr(name, " mithril ")) return (16);
		if (strstr(name, " adamantite ")) return (17);

		/* Look for textual clues */
		if (strstr(name, "Copper ")) return (2);
		if (strstr(name, "Silver ")) return (5);
		if (strstr(name, "Gold ")) return (10);
		if (strstr(name, "Mithril ")) return (16);
		if (strstr(name, "Adamantite ")) return (17);
	}

	/* Assume nothing */
	return (0);
}


/*
 * Create magical stairs after finishing a quest monster.
 */
static void build_quest_stairs(int y, int x)
{
	int ny, nx;


	/* Stagger around */
	while (!cave_valid_bold(y, x))
	{
		int d = 1;

		/* Pick a location */
		scatter(&ny, &nx, y, x, d, 0);

		/* Stagger */
		y = ny; x = nx;
	}

	/* Destroy any objects */
	delete_object(y, x);

	/* Explain the staircase */
	msg_print("A magical staircase appears...");

	/* Create stairs down */
	cave_set_feat(y, x, FEAT_MORE);

	/* Update the visuals */
	p_ptr->update |= (PU_UPDATE_VIEW | PU_MONSTERS);

	/* Fully update the flow */
	p_ptr->update |= (PU_FORGET_FLOW | PU_UPDATE_FLOW);
}


/*
 * Handle the "death" of a monster.
 *
 * Disperse treasures centered at the monster location based on the
 * various flags contained in the monster flags fields.
 *
 * Check for "Quest" completion when a quest monster is killed.
 *
 * Note that only the player can induce "monster_death()" on Uniques.
 * Thus (for now) all Quest monsters should be Uniques.
 *
 * Note that monsters can now carry objects, and when a monster dies,
 * it drops all of its objects, which may disappear in crowded rooms.
 */
void monster_death(int m_idx)
{
	int i, j, y, x;

	int dump_item = 0;
	int dump_gold = 0;

	int number = 0;
	int total = 0;

	bool questlevel = FALSE;
	bool completed = FALSE;
	bool fixedquest = FALSE;

	s16b this_o_idx, next_o_idx = 0;

	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool visible = (m_ptr->ml || (r_ptr->flags1 & (RF1_UNIQUE)));

	bool good = (r_ptr->flags1 & (RF1_DROP_GOOD)) ? TRUE : FALSE;
	bool great = (r_ptr->flags1 & (RF1_DROP_GREAT)) ? TRUE : FALSE;

	bool do_gold = (!(r_ptr->flags1 & (RF1_ONLY_ITEM)));
	bool do_item = (!(r_ptr->flags1 & (RF1_ONLY_GOLD)));

	int force_coin = get_coin_type(r_ptr);

	object_type *i_ptr;
	object_type object_type_body;


	/* Get the location */
	y = m_ptr->fy;
	x = m_ptr->fx;


	/* Drop objects being carried */
	for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Paranoia */
		o_ptr->held_m_idx = 0;

		/* Get local object */
		i_ptr = &object_type_body;

		/* Copy the object */
		object_copy(i_ptr, o_ptr);

		/* Delete the object */
		delete_object_idx(this_o_idx);

		/* Drop it */
		drop_near(i_ptr, -1, y, x);
	}

	/* Forget objects */
	m_ptr->hold_o_idx = 0;


	/* Mega-Hack -- drop "winner" treasures */
	if (r_ptr->flags1 & (RF1_DROP_CHOSEN))
	{
		/* Get local object */
		/*i_ptr = &object_type_body;*/

		/* Mega-Hack -- Prepare to make "Grond" */
		/*object_prep(i_ptr, lookup_kind(TV_HAFTED, SV_GROND));*/

		/* Mega-Hack -- Mark this item as "Grond" */
		/*i_ptr->name1 = ART_GROND;*/

		/* Mega-Hack -- Actually create "Grond" */
		/*apply_magic(i_ptr, -1, TRUE, TRUE, TRUE);*/

		/* Drop it in the dungeon */
		/*drop_near(i_ptr, -1, y, x);*/


		/* Get local object */
		/*i_ptr = &object_type_body;*/

		/* Mega-Hack -- Prepare to make "Morgoth" */
		/* object_prep(i_ptr, lookup_kind(TV_CROWN, SV_MORGOTH)); */

		/* Mega-Hack -- Mark this item as "Morgoth" */
		/*i_ptr->name1 = ART_MORGOTH;*/

		/* Mega-Hack -- Actually create "Morgoth" */
		/*apply_magic(i_ptr, -1, TRUE, TRUE, TRUE);*/

		/* Drop it in the dungeon */
		/*drop_near(i_ptr, -1, y, x);*/
	}


	/* Determine how much we can drop */
	if ((r_ptr->flags1 & (RF1_DROP_5)) && (rand_int(100) < 5)) number++;
	if ((r_ptr->flags1 & (RF1_DROP_10)) && (rand_int(100) < 10)) number++;
	if ((r_ptr->flags1 & (RF1_DROP_20)) && (rand_int(100) < 20)) number++;
	if ((r_ptr->flags1 & (RF1_DROP_40)) && (rand_int(100) < 40)) number++;
	if ((r_ptr->flags1 & (RF1_DROP_80)) && (rand_int(100) < 80)) number++;
	if (r_ptr->flags1 & (RF1_DROP_1D2)) number += damroll(1, 2);

	/* Hack -- handle creeping coins */
	coin_type = force_coin;

	/* Average dungeon and monster levels */
	object_level = (p_ptr->depth + r_ptr->level) / 2;

	/* Drop some objects */
	for (j = 0; j < number; j++)
	{
		/* Get local object */
		i_ptr = &object_type_body;

		/* Wipe the object */
		object_wipe(i_ptr);

		/* Make Gold */
		if (do_gold && (!do_item || (rand_int(100) < 50)))
		{
			/* Make some gold */
			if (!make_gold(i_ptr)) continue;

			/* Assume seen XXX XXX XXX */
			dump_gold++;
		}

		/* Make Object */
		else
		{
			/* Make an object */
			if (!make_object(i_ptr, good, great, FALSE)) continue;

			/* Assume seen XXX XXX XXX */
			dump_item++;
		}

		/* Drop it in the dungeon */
		drop_near(i_ptr, -1, y, x);
	}

	/* Reset the object level */
	object_level = p_ptr->depth;

	/* Reset "coin" type */
	coin_type = 0;


	/* Take note of any dropped treasure */
	if (visible && (dump_item || dump_gold))
	{
		/* Take notes on treasure */
		lore_treasure(m_idx, dump_item, dump_gold);
	}

	/* Update monster list window */
	p_ptr->window |= PW_VISIBLE;

	/* Only process dungeon kills */
	if (!p_ptr->depth) return;

	/* Count incomplete quests */
	for (i = 0; i < z_info->q_max; i++)
	{
		quest_type *q_ptr = &q_info[i];

		/*hack - don't count if player didn't kill
		 *this assumes only player can kill uniques!!!!!
		 * This line is also ugly codeing. :)
		 */
		/* if (who != -1) continue; */

		/* Quest level? */
		if (q_ptr->active_level == p_ptr->depth)
		{
			/* One on the level */
			questlevel = TRUE;

			/* Require "Quest Monsters" */
			if ((q_ptr->mon_idx == m_ptr->r_idx) && (!(m_ptr->mflag & MFLAG_CLON)))
			{

				char race_name[80];

				/* Get the monster race name (singular)*/
				monster_desc_race(race_name, sizeof(race_name), q_ptr->mon_idx);

				/* Mark kills */
				q_ptr->cur_num++;

				/* Completed quest? */
				if (q_ptr->cur_num == q_ptr->max_num)
				{
					/* Mark complete */
					q_ptr->active_level = 0;

					/* Mark fixed quests */
					if ((q_ptr->type == QUEST_FIXED) || (q_ptr->type == QUEST_FIXED_U))
						fixedquest = TRUE;

					/* One complete */
					completed = TRUE;

				}

				/*let the player know how many left*/
				else if (q_ptr->type == QUEST_GUILD)
				{
						int remaining = q_ptr->max_num - q_ptr->cur_num;

						/* Multiple quest monsters */
						if (remaining > 1)
						{
							plural_aux(race_name, sizeof(race_name));
						}

						msg_format("You have %d %s remaining to complete your quest.",
								remaining, race_name);
				}
			}

		}

		/* Count incomplete fixed quests */
		if (q_ptr->active_level && (fixedquest)) total++;
	}

	/* Require a quest level */
	if (!questlevel) return;

	/* Require all quests on this level to be completed */
	if (!completed) return;

	/* Check quest type */
	if (!fixedquest)
	{
		/* Give a message */
		msg_print("You have completed your quest - collect your reward at the library!");
		return;
	}

	/* Need some stairs */
	else if (total)
	{
		/* Build magical stairs */
		build_quest_stairs(y, x);

		p_ptr->fame += 10;
	}

	/* Nothing left, game over... */
	else 
	{
		/* Total winner */
		p_ptr->total_winner = TRUE;

		/* Redraw the "title" */
		p_ptr->redraw |= (PR_TITLE);

		/* Congratulations */
		msg_print("*** CONGRATULATIONS ***");
		msg_print("You have won the game!");
		msg_print("You may retire (commit suicide) when you are ready.");
	}
}




/*
 * Decrease a monster's hit points, handle monster death.
 *
 * We return TRUE if the monster has been killed (and deleted).
 *
 * We announce monster death (using an optional "death message"
 * if given, and a otherwise a generic killed/destroyed message).
 *
 * Only "physical attacks" can induce the "You have slain" message.
 * Missile and Spell attacks will induce the "dies" message, or
 * various "specialized" messages.  Note that "You have destroyed"
 * and "is destroyed" are synonyms for "You have slain" and "dies".
 *
 * Invisible monsters induce a special "You have killed it." message.
 *
 * Hack -- we "delay" fear messages by passing around a "fear" flag.
 *
 * Consider decreasing monster experience over time, say, by using
 * "(m_exp * m_lev * (m_lev)) / (p_lev * (m_lev + n_killed))" instead
 * of simply "(m_exp * m_lev) / (p_lev)", to make the first monster
 * worth more than subsequent monsters.  This would also need to
 * induce changes in the monster recall code.  XXX XXX XXX
 */
bool mon_take_hit(int m_idx, int dam, bool *fear, cptr note)
{
	monster_type *m_ptr = &m_list[m_idx];

	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	monster_lore *l_ptr = &l_list[m_ptr->r_idx];

	s32b div, new_exp, new_exp_frac;
	bool seen = FALSE;
	bool fully_seen = FALSE;
	/* Redraw (later) if needed */
	if (p_ptr->health_who == m_idx) p_ptr->redraw |= (PR_HEALTH);


	/* Get visibility */
	if (m_ptr->ml)
	{
		seen = TRUE;
		if (mon_fully_visible(m_ptr)) fully_seen = TRUE;
	}


	/* Wake it up */
	m_ptr->csleep = 0;

	/* Hurt it */
	m_ptr->hp -= dam;

	/* It is dead now */
	if (m_ptr->hp < 0)
	{
		char m_name[80];

		/* Extract monster name */
		monster_desc(m_name, m_ptr, 0);

		/* Increase the noise level slightly. */
		if (add_wakeup_chance <= 8000) add_wakeup_chance += 300;

		/* Clone deaths */
		if (m_ptr->mflag & MFLAG_CLON)
		{
			message_format(MSG_KILL, m_ptr->r_idx, "%^s clone melts into nothingness.", m_name);
		}

		/* Death by Missile/Spell attack */
		else if (note && seen)
		{
			message_format(MSG_KILL, m_ptr->r_idx, "%^s%s", m_name, note);
		}

		/* Death by physical attack -- invisible monster */
		else if (!m_ptr->ml)
		{
			message_format(MSG_KILL, m_ptr->r_idx, "You have killed %s.", m_name);
		}

		/* Death by Physical attack -- non-living monster */
		else if ((r_ptr->flags3 & (RF3_DEMON)) ||
		         (r_ptr->flags3 & (RF3_UNDEAD)) ||
		         (r_ptr->flags2 & (RF2_STUPID)) ||
		         (strchr("EvgaC", r_ptr->d_char)))
		{
			message_format(MSG_KILL, m_ptr->r_idx, "You have destroyed %s.", m_name);
		}

		/* Death by Physical attack -- living monster */
		else
		{
			message_format(MSG_KILL, m_ptr->r_idx, "You have slain %s.", m_name);
		}

		/* Player level */
		div = p_ptr->lev;

		/* Give some experience for the kill */
		new_exp = ((long)r_ptr->mexp * r_ptr->level) / div;

		/* Handle fractional experience */
		new_exp_frac = ((((long)r_ptr->mexp * r_ptr->level) % div)
		                * 0x10000L / div) + p_ptr->exp_frac;

		/* Keep track of experience */
		if (new_exp_frac >= 0x10000L)
		{
			new_exp++;
			p_ptr->exp_frac = (u16b)(new_exp_frac - 0x10000L);
		}
		else
		{
			p_ptr->exp_frac = (u16b)new_exp_frac;
		}

		if (m_ptr->mflag & MFLAG_CLON) new_exp = 0;

		/* Gain experience */
		gain_exp(new_exp);

		/* Generate treasure */
		if (!(m_ptr->mflag & MFLAG_CLON)) monster_death(m_idx);

		/* When the player kills a Unique, it stays dead */
		if ((!(m_ptr->mflag & MFLAG_CLON)) && (r_ptr->flags1 & (RF1_UNIQUE))) r_ptr->max_num = 0;

		/* Recall even invisible uniques or winners */
		if (m_ptr->ml || (r_ptr->flags1 & (RF1_UNIQUE)))
		{
			/* Count kills this life */
			if (l_ptr->r_pkills < MAX_SHORT) l_ptr->r_pkills++;

			/* Count kills in all lives */
			if (l_ptr->r_tkills < MAX_SHORT) l_ptr->r_tkills++;

			/* Hack -- Auto-recall */
			monster_race_track(m_ptr->r_idx);
		}

		/* Delete the monster */
		delete_monster_idx(m_idx);

		/* Not afraid */
		(*fear) = FALSE;

		/* Monster is dead */
		return (TRUE);
	}


#ifdef ALLOW_FEAR

	/* Mega-Hack -- Pain cancels fear */
	if (m_ptr->monfear && (dam > 0))
	{
		int tmp = randint(dam);

		/* Cure a little fear */
		if (tmp < m_ptr->monfear)
		{
			/* Reduce fear */
			m_ptr->monfear -= tmp;
		}

		/* Cure all the fear */
		else
		{
			/* Cure fear */
			m_ptr->monfear = 0;

			/* No more fear */
			(*fear) = FALSE;
		}
	}

	/* Sometimes a monster gets scared by damage */
	if (!m_ptr->monfear && !(r_ptr->flags3 & (RF3_NO_FEAR)) && (dam > 0))
	{
		int percentage;

		/* Percentage of fully healthy */
		percentage = (100L * m_ptr->hp) / m_ptr->maxhp;

		/*
		 * Run (sometimes) if at 10% or less of max hit points,
		 * or (usually) when hit for half its current hit points
		 */
		if ((randint(10) >= percentage) ||
		    ((dam >= m_ptr->hp) && (rand_int(100) < 80)))
		{
			/* Hack -- note fear */
			(*fear) = TRUE;

			/* Hack -- Add some timed fear */
			m_ptr->monfear = (randint(10) +
			                  (((dam >= m_ptr->hp) && (percentage > 7)) ?
			                   20 : ((11 - percentage) * 5)));
		}
	}

#endif /* ALLOW_FEAR */


	/* Not dead yet */
	return (FALSE);
}


/*
 * Modify the current panel to the given coordinates, adjusting only to
 * ensure the coordinates are legal, and return TRUE if anything done.
 *
 * Hack -- The town should never be scrolled around.
 *
 * Note that monsters are no longer affected in any way by panel changes.
 *
 * As a total hack, whenever the current panel changes, we assume that
 * the "overhead view" window should be updated.
 */
bool modify_panel(int wy, int wx)
{
	/* Verify wy, adjust if needed */
	if (p_ptr->depth == 0) wy = 0;
	else if (wy > DUNGEON_HGT - SCREEN_HGT) wy = DUNGEON_HGT - SCREEN_HGT;
	else if (wy < 0) wy = 0;

	/* Verify wx, adjust if needed */
	if (p_ptr->depth == 0) wx = 0;
	else if (wx > DUNGEON_WID - SCREEN_WID) wx = DUNGEON_WID - SCREEN_WID;
	else if (wx < 0) wx = 0;

	/* React to changes */
	if ((p_ptr->wy != wy) || (p_ptr->wx != wx))
	{
		/* Save wy, wx */
		p_ptr->wy = wy;
		p_ptr->wx = wx;

		/* Redraw map */
		p_ptr->redraw |= (PR_MAP);

		/* Hack -- Window stuff */
		p_ptr->window |= (PW_OVERHEAD);

		/* Changed */
		return (TRUE);
	}

	/* No change */
	return (FALSE);
}


/*
 * Perform the minimum "whole panel" adjustment to ensure that the given
 * location is contained inside the current panel, and return TRUE if any
 * such adjustment was performed.
 */
bool adjust_panel(int y, int x)
{
	int wy = p_ptr->wy;
	int wx = p_ptr->wx;

	/* Adjust as needed */
	while (y >= wy + SCREEN_HGT) wy += SCREEN_HGT;
	while (y < wy) wy -= SCREEN_HGT;

	/* Adjust as needed */
	while (x >= wx + SCREEN_WID) wx += SCREEN_WID;
	while (x < wx) wx -= SCREEN_WID;

	/* Use "modify_panel" */
	return (modify_panel(wy, wx));
}


/*
 * Change the current panel to the panel lying in the given direction.
 *
 * Return TRUE if the panel was changed.
 */
bool change_panel(int dir)
{
	int wy = p_ptr->wy + ddy[dir] * PANEL_HGT;
	int wx = p_ptr->wx + ddx[dir] * PANEL_WID;

	/* Use "modify_panel" */
	return (modify_panel(wy, wx));
}


/*
 * Verify the current panel (relative to the player location).
 *
 * By default, when the player gets "too close" to the edge of the current
 * panel, the map scrolls one panel in that direction so that the player
 * is no longer so close to the edge.
 *
 * The "center_player" option allows the current panel to always be centered
 * around the player, which is very expensive, and also has some interesting
 * gameplay ramifications.
 */
void verify_panel(void)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int wy = p_ptr->wy;
	int wx = p_ptr->wx;


	/* Scroll screen vertically when off-center */
	if (center_player && (!p_ptr->running || !run_avoid_center) &&
	    (py != wy + SCREEN_HGT / 2))
	{
		wy = py - SCREEN_HGT / 2;
	}

	/* Scroll screen vertically when 2 grids from top/bottom edge */
	else if ((py < wy + 2) || (py >= wy + SCREEN_HGT - 2))
	{
		wy = ((py - PANEL_HGT / 2) / PANEL_HGT) * PANEL_HGT;
	}


	/* Scroll screen horizontally when off-center */
	if (center_player && (!p_ptr->running || !run_avoid_center) &&
	    (px != wx + SCREEN_WID / 2))
	{
		wx = px - SCREEN_WID / 2;
	}

	/* Scroll screen horizontally when 4 grids from left/right edge */
	else if ((px < wx + 4) || (px >= wx + SCREEN_WID - 4))
	{
		wx = ((px - PANEL_WID / 2) / PANEL_WID) * PANEL_WID;
	}


	/* Scroll if needed */
	if (modify_panel(wy, wx))
	{
		/* Optional disturb on "panel change" */
		if (disturb_panel && !center_player) disturb(0, 0);
	}
}



/*
 * Monster health description
 */
cptr look_mon_desc(char *buf, int m_idx)
{
	monster_type *m_ptr = &m_list[m_idx];
	monster_race *r_ptr = &r_info[m_ptr->r_idx];

	bool living = TRUE;
	int perc;

	/* Determine if the monster is "living" (vs "undead") */
	if (r_ptr->flags3 & (RF3_UNDEAD)) living = FALSE;
	if (r_ptr->flags3 & (RF3_DEMON)) living = FALSE;
	if (strchr("Egv", r_ptr->d_char)) living = FALSE;


	/* Healthy monsters */
	if (m_ptr->hp >= m_ptr->maxhp)
	{
		/* No damage */
		strcpy(buf, (living ? "unhurt" : "undamaged"));
	}
	else
	{
		/* Calculate a health "percentage" */
		perc = 100L * m_ptr->hp / m_ptr->maxhp;

		if (perc >= 60)
			strcpy(buf, (living ? "somewhat wounded" : "somewhat damaged"));
		else if (perc >= 25)
			strcpy(buf, (living ? "wounded" : "damaged"));
		else if (perc >= 10)
			strcpy(buf, (living ? "badly wounded" : "badly damaged"));
		else
			strcpy(buf, (living ? "almost dead" : "almost destroyed"));
	}

	if (m_ptr->csleep && living) strcat(buf, ", asleep");
	else if (m_ptr->csleep) strcat(buf, ", inactive");
	if (m_ptr->confused) strcat(buf, ", confused");
	if (m_ptr->monfear) strcat(buf, ", afraid");
	if (m_ptr->stunned) strcat(buf, ", stunned");
	
	return buf;
}



/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort_aux(vptr u, vptr v, int p, int q)
{
	int z, a, b;

	/* Done sort */
	if (p >= q) return;

	/* Pivot */
	z = p;

	/* Begin */
	a = p;
	b = q;

	/* Partition */
	while (TRUE)
	{
		/* Slide i2 */
		while (!(*ang_sort_comp)(u, v, b, z)) b--;

		/* Slide i1 */
		while (!(*ang_sort_comp)(u, v, z, a)) a++;

		/* Done partition */
		if (a >= b) break;

		/* Swap */
		(*ang_sort_swap)(u, v, a, b);

		/* Advance */
		a++, b--;
	}

	/* Recurse left side */
	ang_sort_aux(u, v, p, b);

	/* Recurse right side */
	ang_sort_aux(u, v, b+1, q);
}


/*
 * Angband sorting algorithm -- quick sort in place
 *
 * Note that the details of the data we are sorting is hidden,
 * and we rely on the "ang_sort_comp()" and "ang_sort_swap()"
 * function hooks to interact with the data, which is given as
 * two pointers, and which may have any user-defined form.
 */
void ang_sort(vptr u, vptr v, int n)
{
	/* Sort the array */
	ang_sort_aux(u, v, 0, n-1);
}





/*** Targetting Code ***/


/*
 * Given a "source" and "target" location, extract a "direction",
 * which will move one step from the "source" towards the "target".
 *
 * Note that we use "diagonal" motion whenever possible.
 *
 * We return "5" if no motion is needed.
 */
sint motion_dir(int y1, int x1, int y2, int x2)
{
	/* No movement required */
	if ((y1 == y2) && (x1 == x2)) return (5);

	/* South or North */
	if (x1 == x2) return ((y1 < y2) ? 2 : 8);

	/* East or West */
	if (y1 == y2) return ((x1 < x2) ? 6 : 4);

	/* South-east or South-west */
	if (y1 < y2) return ((x1 < x2) ? 3 : 1);

	/* North-east or North-west */
	if (y1 > y2) return ((x1 < x2) ? 9 : 7);

	/* Paranoia */
	return (5);
}


/*
 * Extract a direction (or zero) from a character
 */
sint target_dir(char ch)
{
	int d = 0;

	int mode;

	cptr act;

	cptr s;


	/* Already a direction? */
	if (isdigit(ch))
	{
		d = D2I(ch);
	}
	else
	{
		/* Roguelike */
		if (rogue_like_commands)
		{
			mode = KEYMAP_MODE_ROGUE;
		}

		/* Original */
		else
		{
			mode = KEYMAP_MODE_ORIG;
		}

		/* Extract the action (if any) */
		act = keymap_act[mode][(byte)(ch)];

		/* Analyze */
		if (act)
		{
			/* Convert to a direction */
			for (s = act; *s; ++s)
			{
				/* Use any digits in keymap */
				if (isdigit(*s)) d = D2I(*s);
			}
		}
	}

	/* Paranoia */
	if (d == 5) d = 0;

	/* Return direction */
	return (d);
}



/*
 * Determine is a monster makes a reasonable target
 *
 * The concept of "targeting" was stolen from "Morgul" (?)
 *
 * The player can target any location, or any "target-able" monster.
 *
 * There are now two different rules for targeting monsters:
 * If this function is called in "use_sight" mode, we allow monsters in
 * line of sight.  Otherwise, we require line of fire.
 * This (hopefully) will allow the player to both use "closest target"
 * macros and to keep an eye on viewable monsters with the monster health
 * bar.
 */
bool target_able(int m_idx, bool use_sight)
{
	monster_type *m_ptr;

	/* No monster */
	if (m_idx <= 0) return (FALSE);

	/* Get monster */
	m_ptr = &m_list[m_idx];

	/* Monster must be alive */
	if (!m_ptr->r_idx) return (FALSE);

	/* Monster must be visible */
	if (!m_ptr->ml) return (FALSE);

	/* Monster must not be hidden FLAG not implimented yet */
	/* if (m_ptr->mflag & (MFLAG_MIME)) return (FALSE); */

	/* Hack -- no targeting hallucinations */
	if (p_ptr->image) return (FALSE);

	/* In manual mode */
	if (use_sight)
	{
		/* Monster must be in line of sight */
		if (!player_has_los_bold(m_ptr->fy, m_ptr->fx)) return (FALSE);
	}

	/* In automatic mode */
	else
	{
		/* Monster must be in line of fire (which means also line of sight) */
		if (!player_can_fire_bold(m_ptr->fy, m_ptr->fx)) return (FALSE);
	}

	/* Assume okay */
	return (TRUE);
}





/*
 * Update (if necessary) and verify (if possible) the target.
 *
 * We return TRUE if the target is "okay" and FALSE otherwise.
 */
bool target_okay(void)
{
	/* No target */
	if (!p_ptr->target_set) return (FALSE);

	/* Accept "location" targets */
	if (p_ptr->target_who == 0) return (TRUE);

	/* Check "monster" targets */
	if (p_ptr->target_who > 0)
	{
		int m_idx = p_ptr->target_who;

		/* Accept reasonable targets */
		if (target_able(m_idx, FALSE))
		{
			monster_type *m_ptr = &m_list[m_idx];

			/* Get the monster location */
			p_ptr->target_row = m_ptr->fy;
			p_ptr->target_col = m_ptr->fx;

			/* Good target */
			return (TRUE);
		}
	}

	/* Assume no target */
	return (FALSE);
}


/*
 * Set the target to a monster (or nobody)
 */
void target_set_monster(int m_idx)
{
	/* Acceptable target */
	if ((m_idx > 0) && target_able(m_idx, TRUE))
	{
		monster_type *m_ptr = &m_list[m_idx];

		/* Save target info */
		p_ptr->target_set = TRUE;
		p_ptr->target_who = m_idx;
		p_ptr->target_row = m_ptr->fy;
		p_ptr->target_col = m_ptr->fx;
	}

	/* Clear target */
	else
	{
		/* Reset target info */
		p_ptr->target_set = FALSE;
		p_ptr->target_who = 0;
		p_ptr->target_row = 0;
		p_ptr->target_col = 0;
	}
}


/*
 * Set the target to a location
 */
void target_set_location(int y, int x)
{
	/* Legal target */
	if (in_bounds_fully(y, x))
	{
		/* Save target info */
		p_ptr->target_set = TRUE;
		p_ptr->target_who = 0;
		p_ptr->target_row = y;
		p_ptr->target_col = x;
	}

	/* Clear target */
	else
	{
		/* Reset target info */
		p_ptr->target_set = FALSE;
		p_ptr->target_who = 0;
		p_ptr->target_row = 0;
		p_ptr->target_col = 0;
	}
}


/*
 * Sorting hook -- comp function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by double-distance to the player.
 */
static bool ang_sort_comp_distance(const void *u, const void *v, int a, int b)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	int da, db, kx, ky;

	/* Absolute distance components */
	kx = x[a]; kx -= px; kx = ABS(kx);
	ky = y[a]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	da = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Absolute distance components */
	kx = x[b]; kx -= px; kx = ABS(kx);
	ky = y[b]; ky -= py; ky = ABS(ky);

	/* Approximate Double Distance to the first point */
	db = ((kx > ky) ? (kx + kx + ky) : (ky + ky + kx));

	/* Compare the distances */
	return (da <= db);
}


/*
 * Sorting hook -- swap function -- by "distance to player"
 *
 * We use "u" and "v" to point to arrays of "x" and "y" positions,
 * and sort the arrays by distance to the player.
 */
static void ang_sort_swap_distance(void *u, void *v, int a, int b)
{
	byte *x = (byte*)(u);
	byte *y = (byte*)(v);

	byte temp;

	/* Swap "x" */
	temp = x[a];
	x[a] = x[b];
	x[b] = temp;

	/* Swap "y" */
	temp = y[a];
	y[a] = y[b];
	y[b] = temp;
}



/*
 * Hack -- help "select" a location (see below)
 */
static s16b target_pick(int y1, int x1, int dy, int dx)
{
	int i, v;

	int x2, y2, x3, y3, x4, y4;

	int b_i = -1, b_v = 9999;


	/* Scan the locations */
	for (i = 0; i < temp_n; i++)
	{
		/* Point 2 */
		x2 = temp_x[i];
		y2 = temp_y[i];

		/* Directed distance */
		x3 = (x2 - x1);
		y3 = (y2 - y1);

		/* Verify quadrant */
		if (dx && (x3 * dx <= 0)) continue;
		if (dy && (y3 * dy <= 0)) continue;

		/* Absolute distance */
		x4 = ABS(x3);
		y4 = ABS(y3);

		/* Verify quadrant */
		if (dy && !dx && (x4 > y4)) continue;
		if (dx && !dy && (y4 > x4)) continue;

		/* Approximate Double Distance */
		v = ((x4 > y4) ? (x4 + x4 + y4) : (y4 + y4 + x4));

		/* Penalize location XXX XXX XXX */

		/* Track best */
		if ((b_i >= 0) && (v >= b_v)) continue;

		/* Track best */
		b_i = i; b_v = v;
	}

	/* Result */
	return (b_i);
}


/*
 * Hack -- determine if a given location is "interesting"
 */
static bool target_set_interactive_accept(int y, int x)
{
	s16b this_o_idx, next_o_idx = 0;


	/* Player grids are always interesting */
	if (cave_m_idx[y][x] < 0) return (TRUE);


	/* Handle hallucination */
	if (p_ptr->image) return (FALSE);


	/* Visible monsters */
	if (cave_m_idx[y][x] > 0)
	{
		monster_type *m_ptr = &m_list[cave_m_idx[y][x]];

		/* Visible monsters */
		if (m_ptr->ml) return (TRUE);
	}

	/* Scan all objects in the grid */
	for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
	{
		object_type *o_ptr;

		/* Get the object */
		o_ptr = &o_list[this_o_idx];

		/* Get the next object */
		next_o_idx = o_ptr->next_o_idx;

		/* Memorized object */
		if (o_ptr->marked) return (TRUE);
	}

	/* Interesting memorized features */
	if (cave_info[y][x] & (CAVE_MARK))
	{
		/* Notice glyphs */
		if (cave_feat[y][x] == FEAT_GLYPH) return (TRUE);

		/* Notice doors */
		if (cave_feat[y][x] == FEAT_OPEN) return (TRUE);
		if (cave_feat[y][x] == FEAT_BROKEN) return (TRUE);

		/* Notice stairs */
		if (cave_feat[y][x] == FEAT_LESS) return (TRUE);
		if (cave_feat[y][x] == FEAT_MORE) return (TRUE);

		/* Notice shops */
		if ((cave_feat[y][x] >= FEAT_SHOP_HEAD) &&
		    (cave_feat[y][x] <= FEAT_SHOP_TAIL)) return (TRUE);

		/* Notice traps */
		if ((cave_feat[y][x] >= FEAT_TRAP_HEAD) &&
		    (cave_feat[y][x] <= FEAT_TRAP_TAIL)) return (TRUE);

		/* Notice doors */
		if ((cave_feat[y][x] >= FEAT_DOOR_HEAD) &&
		    (cave_feat[y][x] <= FEAT_DOOR_TAIL)) return (TRUE);

		/* Notice rubble */
		if (cave_feat[y][x] == FEAT_RUBBLE) return (TRUE);

		/* Notice veins with treasure */
		if (cave_feat[y][x] == FEAT_MAGMA_K) return (TRUE);
		if (cave_feat[y][x] == FEAT_QUARTZ_K) return (TRUE);
	}

	/* Nope */
	return (FALSE);
}


/*
 * Prepare the "temp" array for "target_interactive_set"
 *
 * Return the number of target_able monsters in the set.
 */
static void target_set_interactive_prepare(int mode)
{
	int y, x;

	/* Reset "temp" array */
	temp_n = 0;

	/* Scan the current panel */
	for (y = p_ptr->wy; y < p_ptr->wy + SCREEN_HGT; y++)
	{
		for (x = p_ptr->wx; x < p_ptr->wx + SCREEN_WID; x++)
		{
			/* Require line of sight, unless "look" is "expanded" */
			if (!expand_look && !player_has_los_bold(y, x)) continue;

			/* Require "interesting" contents */
			if (!target_set_interactive_accept(y, x)) continue;

			/* Special mode */
			if (mode & (TARGET_KILL))
			{
				/* Must contain a monster */
				if (!(cave_m_idx[y][x] > 0)) continue;

				/* Must be a targettable monster */
			 	if (!target_able(cave_m_idx[y][x], FALSE)) continue;
			}

			/* Save the location */
			temp_x[temp_n] = x;
			temp_y[temp_n] = y;
			temp_n++;
		}
	}

	/* Set the sort hooks */
	ang_sort_comp = ang_sort_comp_distance;
	ang_sort_swap = ang_sort_swap_distance;

	/* Sort the positions */
	ang_sort(temp_x, temp_y, temp_n);
}


/*
 * Examine a grid, return a keypress.
 *
 * The "mode" argument contains the "TARGET_LOOK" bit flag, which
 * indicates that the "space" key should scan through the contents
 * of the grid, instead of simply returning immediately.  This lets
 * the "look" command get complete information, without making the
 * "target" command annoying.
 *
 * The "info" argument contains the "commands" which should be shown
 * inside the "[xxx]" text.  This string must never be empty, or grids
 * containing monsters will be displayed with an extra comma.
 *
 * Note that if a monster is in the grid, we update both the monster
 * recall info and the health bar info to track that monster.
 *
 * This function correctly handles multiple objects per grid, and objects
 * and terrain features in the same grid, though the latter never happens.
 *
 * This function must handle blindness/hallucination.
 */
static int target_set_interactive_aux(int y, int x, int mode, cptr info)
{
	s16b this_o_idx, next_o_idx = 0;

	cptr s1, s2, s3;

	bool boring;

	bool floored;

	int feat;

	int query;

	char out_val[256];


	/* Repeat forever */
	while (1)
	{
		/* Paranoia */
		query = ' ';

		/* Assume boring */
		boring = TRUE;

		/* Default */
		s1 = "You see ";
		s2 = "";
		s3 = "";


		/* The player */
		if (cave_m_idx[y][x] < 0)
		{
			/* Description */
			s1 = "You are ";

			/* Preposition */
			s2 = "on ";
		}


		/* Hack -- hallucination */
		if (p_ptr->image)
		{
			cptr name = "something strange";

			/* Display a message */
			if (p_ptr->wizard)
				sprintf(out_val, "%s%s%s%s [%s] (%d:%d)", s1, s2, s3, name, info, y, x);
			else
				sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Stop on everything but "return" */
			if ((query != '\n') && (query != '\r')) break;

			/* Repeat forever */
			continue;
		}


		/* Actual monsters */
		if (cave_m_idx[y][x] > 0)
		{
			monster_type *m_ptr = &m_list[cave_m_idx[y][x]];
			monster_race *r_ptr = &r_info[m_ptr->r_idx];

			/* Visible */
			if (m_ptr->ml)
			{
				bool recall = FALSE;

				char m_name[80];

				/* Not boring */
				boring = FALSE;

				/* Get the monster name ("a kobold") */
				monster_desc(m_name, m_ptr, 0x08);

				/* Hack -- track this monster race */
				monster_race_track(m_ptr->r_idx);

				/* Hack -- health bar for this monster */
				health_track(cave_m_idx[y][x]);

				/* Hack -- handle stuff */
				handle_stuff();

				/* Interact */
				while (1)
				{
					/* Recall */
					if (recall)
					{
						/* Save screen */
						screen_save();

						while (TRUE)
						{

							/* Recall on screen */
							screen_roff(m_ptr->r_idx, p_ptr->monster_mem_fmt);

							/* Hack -- Complete the prompt (again) */
							Term_addstr(-1, TERM_WHITE, format("  [r,%s]", info));

							/* Command */
							query = inkey();
							
							/* Switch display mode */
							if (query == '\t') {
								if (p_ptr->monster_mem_fmt == FALSE) p_ptr->monster_mem_fmt = TRUE;
								else p_ptr->monster_mem_fmt = FALSE;
								/* Update recall window */
								p_ptr->window |= (PW_MONSTER);
							}
							else break;
						}

						/* Load screen */
						screen_load();
					}

					/* Normal */
					else
					{
						cptr attitude;
						char buf[80];
						
						if (is_pet(m_ptr))
							attitude = " (pet) ";
						else if (is_friendly(m_ptr))
							attitude = " (friendly) ";
						else
							attitude = " ";

						

						/* Describe the monster */
						look_mon_desc(buf, cave_m_idx[y][x]);

						/* Describe, and prompt for recall */
						if (p_ptr->wizard)
						{
							sprintf(out_val, "%s%s%s%s (%s)%s[r,%s] (%d:%d)",
						            s1, s2, s3, m_name, buf, attitude, info, y, x);
						}
						else
						{
							sprintf(out_val, "%s%s%s%s (%s) %s[r,%s]",
							        s1, s2, s3, m_name, buf, attitude, info);
						}

						prt(out_val, 0, 0);

						/* Place cursor */
						move_cursor_relative(y, x);

						/* Command */
						query = inkey();
					}

					/* Normal commands */
					if (query != 'r') break;

					/* Toggle recall */
					recall = !recall;
				}

				/* Stop on everything but "return"/"space" */
				if ((query != '\n') && (query != '\r') && (query != ' ')) break;

				/* Sometimes stop at "space" key */
				if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

				/* Change the intro */
				s1 = "It is ";

				/* Hack -- take account of gender */
				if (r_ptr->flags1 & (RF1_FEMALE)) s1 = "She is ";
				else if (r_ptr->flags1 & (RF1_MALE)) s1 = "He is ";

#if 1

				/* Use a preposition */
				s2 = "carrying ";

				/* Scan all objects being carried */
				for (this_o_idx = m_ptr->hold_o_idx; this_o_idx; this_o_idx = next_o_idx)
				{
					char o_name[80];

					object_type *o_ptr;

					/* Get the object */
					o_ptr = &o_list[this_o_idx];

					/* Get the next object */
					next_o_idx = o_ptr->next_o_idx;

					/* Obtain an object description */
					object_desc(o_name, o_ptr, TRUE, 3);

					/* Describe the object */
					if (p_ptr->wizard)
						sprintf(out_val, "%s%s%s%s [%s] (%d:%d)", s1, s2, s3, o_name, info, y, x);
					else
						sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
					prt(out_val, 0, 0);
					move_cursor_relative(y, x);
					query = inkey();

					/* Stop on everything but "return"/"space" */
					if ((query != '\n') && (query != '\r') && (query != ' ')) break;

					/* Sometimes stop at "space" key */
					if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

					/* Change the intro */
					s2 = "also carrying ";
				}

				/* Double break */
				if (this_o_idx) break;

#endif

				/* Use a preposition */
				s2 = "on ";
			}
		}


		/* Assume not floored */
		floored = FALSE;

#ifdef ALLOW_EASY_FLOOR

		/* Scan all objects in the grid */
		if (easy_floor)
		{
			int floor_list[24];
			int floor_num;

			/* Scan for floor objects */
			floor_num = scan_floor(floor_list, 24, y, x, 0x02);

			/* Actual pile */
			if (floor_num > 1)
			{
				/* Not boring */
				boring = FALSE;

				/* Floored */
				floored = TRUE;

				/* Describe */
				while (1)
				{
					/* Describe the pile */
					if (p_ptr->wizard)
						sprintf(out_val, "%s%s%sa pile of %d objects [r,%s] (%d:%d)", s1, s2, s3, floor_num, info, y, x);
					else
						sprintf(out_val, "%s%s%sa pile of %d objects [r,%s]",
							s1, s2, s3, floor_num, info);
					prt(out_val, 0, 0);
					move_cursor_relative(y, x);
					query = inkey();

					/* Display objects */
					if (query == 'r')
					{
						/* Save screen */
						screen_save();

						/* Display */
						show_floor(floor_list, floor_num);

						/* Describe the pile */
						prt(out_val, 0, 0);
						query = inkey();

						/* Load screen */
						screen_load();

						/* Continue on 'r' only */
						if (query == 'r') continue;
					}

					/* Done */
					break;
				}

				/* Stop on everything but "return"/"space" */
				if ((query != '\n') && (query != '\r') && (query != ' ')) break;

				/* Sometimes stop at "space" key */
				if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

				/* Change the intro */
				s1 = "It is ";

				/* Preposition */
				s2 = "on ";
			}
		}

#endif /* ALLOW_EASY_FLOOR */

		/* Scan all objects in the grid */
		for (this_o_idx = cave_o_idx[y][x]; this_o_idx; this_o_idx = next_o_idx)
		{
			object_type *o_ptr;

			/* Get the object */
			o_ptr = &o_list[this_o_idx];

			/* Get the next object */
			next_o_idx = o_ptr->next_o_idx;

			/* Skip objects if floored */
			if (floored) continue;

			/* Describe it */
			if (o_ptr->marked)
			{
				char o_name[80];

				/* Not boring */
				boring = FALSE;

				/* Obtain an object description */
				object_desc(o_name, o_ptr, TRUE, 3);

				/* Describe the object */
				if (p_ptr->wizard)
					sprintf(out_val, "%s%s%s%s [%s] (%d:%d)", s1, s2, s3, o_name, info, y, x);
				else
					sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, o_name, info);
				prt(out_val, 0, 0);
				move_cursor_relative(y, x);
				query = inkey();

				/* Stop on everything but "return"/"space" */
				if ((query != '\n') && (query != '\r') && (query != ' ')) break;

				/* Sometimes stop at "space" key */
				if ((query == ' ') && !(mode & (TARGET_LOOK))) break;

				/* Change the intro */
				s1 = "It is ";

				/* Plurals */
				if (o_ptr->number != 1) s1 = "They are ";

				/* Preposition */
				s2 = "on ";
			}
		}

		/* Double break */
		if (this_o_idx) break;


		/* Feature (apply "mimic") */
		feat = f_info[cave_feat[y][x]].mimic;

		/* Require knowledge about grid, or ability to see grid */
		if (!(cave_info[y][x] & (CAVE_MARK)) && !player_can_see_bold(y,x))
		{
			/* Forget feature */
			feat = FEAT_NONE;
		}

		/* Terrain feature if needed */
		if (boring || (feat > FEAT_INVIS))
		{
			cptr name = f_name + f_info[feat].name;

			/* Hack -- handle unknown grids */
			if (feat == FEAT_NONE) name = "unknown grid";

			/* Pick a prefix */
			if (*s2 && (feat >= FEAT_DOOR_HEAD)) s2 = "in ";

			/* Pick proper indefinite article */
			s3 = (is_a_vowel(name[0])) ? "an " : "a ";

			/* Hack -- special introduction for store doors */
			if ((feat >= FEAT_SHOP_HEAD) && (feat <= FEAT_SHOP_TAIL))
			{
				s3 = "the entrance to the ";
			}

			/* Display a message */
			if (p_ptr->wizard)
				sprintf(out_val, "%s%s%s%s [%s] (%d:%d)", s1, s2, s3, name, info, y, x);
			else
				sprintf(out_val, "%s%s%s%s [%s]", s1, s2, s3, name, info);
			prt(out_val, 0, 0);
			move_cursor_relative(y, x);
			query = inkey();

			/* Stop on everything but "return"/"space" */
			if ((query != '\n') && (query != '\r') && (query != ' ')) break;
		}

		/* Stop on everything but "return" */
		if ((query != '\n') && (query != '\r')) break;
	}

	/* Keep going */
	return (query);
}


/*
 * Handle "target" and "look".
 *
 * Note that this code can be called from "get_aim_dir()".
 *
 * All locations must be on the current panel, unless the "scroll_target"
 * option is used, which allows changing the current panel during "look"
 * and "target" commands.  Currently, when "flag" is true, that is, when
 * "interesting" grids are being used, and a directional key is used, we
 * only scroll by a single panel, in the direction requested, and check
 * for any interesting grids on that panel.  The "correct" solution would
 * actually involve scanning a larger set of grids, including ones in
 * panels which are adjacent to the one currently scanned, but this is
 * overkill for this function.  XXX XXX
 *
 * Hack -- targeting/observing an "outer border grid" may induce
 * problems, so this is not currently allowed.
 *
 * The player can use the direction keys to move among "interesting"
 * grids, or the "space", "+", and "-" keys to move through these grids
 * sequentially, or can enter "location" mode, and use the direction
 * keys to move one grid at a time in any direction.  The "t" (set
 * target) command will only target a monster (as opposed to a
 * location) if the monster is target_able and the "interesting" mode
 * is being used.
 *
 * The current grid is described using the "look" method above, and
 * a new command may be entered at any time, but note that if the
 * "TARGET_LOOK" bit flag is set (or if we are in "location" mode,
 * where "space" has no obvious meaning) then "space" will scan
 * through the description of the current grid until done, instead
 * of immediately jumping to the next "interesting" grid.  This
 * allows the "target" command to retain its old effects.
 *
 * The "*", "+", and "-" keys may always be used to jump immediately
 * to the next (or previous) interesting grid, in the proper mode.
 *
 * The "return" key may always be used to scan through a complete
 * grid description (forever).
 *
 * This command will cancel any old target, even if used from
 * inside the "look" command.
 */
bool target_set_interactive(u16b mode)
{
	int py = p_ptr->py;
	int px = p_ptr->px;

	int i, d, m, t, bd;

	int y = py;
	int x = px;

	bool done = FALSE;

	/* bool flag = TRUE; */
	bool flag = ((mode & TARGET_FREE) ? FALSE :TRUE);

	bool jump = TRUE;

	char query;

	char info[80];

	/* Cancel target  XXX (even when looking?) */
	target_set_monster(0);

	/*
	 * Hack -- Start out by selecting any grid by using the TARGET_GRID
	 * flag.  -TNB-
	 */
	if (mode & (TARGET_GRID))
	{
		if (!(mode & (TARGET_FIRE))) flag = FALSE;
		mode &= ~(TARGET_GRID);
	}



	/* Prepare the "temp" array */
	target_set_interactive_prepare(mode);

	/* Start near the player */
	m = 0;

	/* Interact */
	while (!done)
	{
		/* Interesting grids */
		if (flag && temp_n)
		{
			if (jump)
			{
				y = temp_y[m];
				x = temp_x[m];
			}
			jump = TRUE;

			/* Allow target */
			if ((cave_m_idx[y][x] > 0) &&
			    (target_able(cave_m_idx[y][x], !(mode & (TARGET_FIRE)))))
			{
				strcpy(info, "t,p,o,+,-,?,<dir>");
			}
			else
			{
				strcpy(info, "p,o,+,-,?,<dir>");
			}

			/* Describe and Prompt */
			query = target_set_interactive_aux(y, x, mode, info);

			/* Assume no "direction" */
			d = 0;

			/* Analyze */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '=':
				{
					if (++m == temp_n)
					{
						m = 0;
					}
					break;
				}

				case '-':
				{
					if (m-- == 0)
					{
						m = temp_n - 1;
					}
					break;
				}

				case 'p':
				{
					/* Recenter around player */
					verify_panel();

					/* Handle stuff */
					handle_stuff();

					y = py;
					x = px;

					/* Do not jump anywhere */
					jump = FALSE;

					break;
				}

				case 'o':
				{
					flag = !flag;
					break;
				}

				case 'm':
				{
					break;
				}

				case 't':
				case '5':
				case '0':
				case '.':
				{
					int m_idx = cave_m_idx[y][x];

					if ((m_idx > 0) && target_able(m_idx, !(mode & (TARGET_FIRE))))
					{
						health_track(m_idx);
						target_set_monster(m_idx);
						done = TRUE;
					}
					else
					{
						bell("Illegal target!");
					}
					break;
				}

				case '?':
				{
					/* p_ptr->get_help_index = HELP_TARGET; */
					do_cmd_help();
					break;
				}

				default:
				{
					/* Extract direction */
					d = target_dir(query);

					/* Oops */
					if (!d) bell("Illegal command for target mode!");

					break;
				}
			}

			/* Hack -- move around */
			if (d)
			{
				int old_y = temp_y[m];
				int old_x = temp_x[m];

				/* Find a new monster */
				i = target_pick(old_y, old_x, ddy[d], ddx[d]);

				/* Scroll to find interesting grid */
				if (i < 0)
				{
					int old_wy = p_ptr->wy;
					int old_wx = p_ptr->wx;

					/* Change if legal */
					if (change_panel(d))
					{
						/* Recalculate interesting grids */
						target_set_interactive_prepare(mode);

						/* Find a new monster */
						i = target_pick(old_y, old_x, ddy[d], ddx[d]);

						/* Restore panel if needed */
						if ((i < 0) && modify_panel(old_wy, old_wx))
						{

							/* Recalculate interesting grids */
							target_set_interactive_prepare(mode);
						}

						/* Handle stuff */
						handle_stuff();
					}
				}

				/* Use interesting grid if found */
				if (i >= 0) m = i;
			}
		}

		/* Arbitrary grids */
		else
		{
			/* Default prompt */
			if (!(mode & TARGET_FREE)) strcpy(info, "q,t,p,m,<dir>");
			strcpy(info, "t,p,m,+,-,?,<dir>");

			/* Describe and Prompt (enable "TARGET_LOOK") */
			query = target_set_interactive_aux(y, x, mode | TARGET_LOOK, info);

			/* Assume no direction */
			d = 0;

			/* Analyze the keypress */
			switch (query)
			{
				case ESCAPE:
				case 'q':
				{
					done = TRUE;
					break;
				}

				case ' ':
				case '*':
				case '+':
				case '=':
				case '-':
				{
					break;
				}

				case 'p':
				{
					/* Scroll screen and move cursor to player */
					verify_panel();

					/* Handle stuff */
					handle_stuff();

					y = py;
					x = px;

					break;
				}

				case 'o':
				{
					break;
				}

				case 'm':
				{
					if (!(mode & TARGET_FREE)) 
					{
						flag = TRUE;

						m = 0;
						bd = 999;

						/* Pick a nearby monster */
						for (i = 0; i < temp_n; i++)
						{
							t = distance(y, x, temp_y[i], temp_x[i]);

							/* Pick closest */
							if (t < bd)
							{
								m = i;
								bd = t;
							}
						}

						/* Nothing interesting */
						if (bd == 999) flag = FALSE;
					}

					break;
				}

				case 't':
				case '5':
				case '0':
				{
					target_set_location(y, x);
					done = TRUE;
					break;
				}

				case '?':
				{
					/* p_ptr->get_help_index = HELP_TARGET; */
					do_cmd_help();
					break;
				}

				default:
				{
					/* Extract a direction */
					d = target_dir(query);

					/* Oops */
					if (!d) bell("Illegal command for target mode!");

					break;
				}
			}

			/* Handle "direction" */
			if (d)
			{
				/* Move quickly */
				if (isupper(query))
				{
					int mag = MIN(DUNGEON_WID / 2, DUNGEON_HGT / 2);

					x += ddx[d] * mag;
					y += ddy[d] * mag;
				}

				/* Move one grid */
				else
				{
					x += ddx[d];
					y += ddy[d];
				}

				/* Cannot leave the dungeon */
				if      (x > DUNGEON_WID - 2) x = DUNGEON_WID - 2;
				else if (x < 1) x = 1;
				if      (y > DUNGEON_HGT - 2) y = DUNGEON_HGT - 2;
				else if (y < 1) y = 1;

				/* Adjust panel if needed */
				if (adjust_panel(y, x))
				{
					/* Handle stuff */
					handle_stuff();

					/* Recalculate interesting grids */
					target_set_interactive_prepare(mode);
				}
			}
		}
	}

	/* Forget */
	temp_n = 0;

	/* Clear the top line */
	prt("", 0, 0);

	/* Recenter the map around the player */
	verify_panel();

	/* Update the target display */
	/* left_panel_display(DISPLAY_TARGET, 0); */

	/* Update stuff */
	p_ptr->update |= (PU_MONSTERS);

	/* Redraw map */
	p_ptr->redraw |= (PR_MAP);

	/* Window stuff */
	p_ptr->window |= (PW_OVERHEAD);

	/* Handle stuff */
	handle_stuff();

	/* Failure to set target */
	if (!p_ptr->target_set) return (FALSE);

	/* Success */
	return (TRUE);
}


/*
 * Get an "aiming direction" (1,2,3,4,6,7,8,9 or 5) from the user.
 *
 * Return TRUE if a direction was chosen, otherwise return FALSE.
 *
 * The direction "5" is special, and means "use current target".
 *
 * This function tracks and uses the "global direction", and uses
 * that as the "desired direction", if it is set.
 *
 * Note that "Force Target", if set, will pre-empt user interaction,
 * if there is a usable target already set.
 *
 * Currently this function applies confusion directly.
 */
bool get_aim_dir(int *dp)
{
	int dir;

	char ch;

	cptr p;

#ifdef ALLOW_REPEAT

	if (repeat_pull(dp))
	{
		/* Verify */
		if (!(*dp == 5 && !target_okay()))
		{
			return (TRUE);
		}
	}

#endif /* ALLOW_REPEAT */

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = p_ptr->command_dir;

	/* Hack -- auto-target if requested */
	if (use_old_target && target_okay()) dir = 5;

	/* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
			p = "Direction ('*' to choose a target, '.' for closest, Escape to cancel)?";
		}
		else
		{
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)?";
		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &ch)) break;

		/* Analyze */
		switch (ch)
		{
			/* Set new target, use target if legal */
			case '*':
			case ' ':
			case '\r':
			case '\n':
			{
				if (target_set_interactive(TARGET_KILL | TARGET_FIRE)) dir = 5;
				break;
			}

			/* Target the closest visible monster in line of fire */
			/* putting this command on hold temporarly */
			case '.':
			{
				int n = 0;

				/* Check closest monsters */
				while (TRUE)
				{
					int y = 0, x = 0;

					/* Get next monster */
					n++;

					/* Find the 'n'th closest viewable, visible monster */
					get_closest_los_monster(n, p_ptr->py, p_ptr->px, &y, &x, TRUE);

					/* We have a valid target */
					if ((y) && (x) && (cave_m_idx[y][x] > 0))
					{
						/* Get monster index */
						int m_idx = cave_m_idx[y][x];

						/* Monster must be in line of fire */
						if (target_able(m_idx, FALSE))
						{
							health_track(m_idx);
							target_set_monster(m_idx);
							dir = 5;
							break;
						}
					}

					/* We've run out of targetable monsters */
					else
					{
						bell("No targetable monsters!");
						break;
					}
				}
				break;
			}
			/* Use current target, if set and legal */
			case 't':
			case '5':
			case '0':
			{
				if (target_okay()) dir = 5;
				break;
			}

			/* Possible direction */
			default:
			{
				dir = target_dir(ch);
				break;
			}
		}

		/* Error */
		if (!dir) bell("Illegal aim direction!");
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	p_ptr->command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* Random direction */
		dir = ddd[rand_int(8)];
	}

	/* Notice confusion */
	if (p_ptr->command_dir != dir)
	{
		/* Warn the user */
		msg_print("You are confused.");
	}

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT

	repeat_push(dir);

#endif /* ALLOW_REPEAT */

	/* A "valid" direction was entered */
	return (TRUE);
}

/* I'm not sure exactly what kind of direction this gets, but it looks ok-ccc */
bool get_hack_dir(int *dp)
{
	int		dir, command_dir;
    cptr    p;
    char command;


	/* Initialize */
	(*dp) = 0;

	/* Global direction */
    dir = 0;

    /* (No auto-targetting */

     /* Ask until satisfied */
	while (!dir)
	{
		/* Choose a prompt */
		if (!target_okay())
		{
			p = "Direction ('*' to choose a target, Escape to cancel)? ";
		}
		else
		{
			p = "Direction ('5' for target, '*' to re-target, Escape to cancel)? ";
		}

		/* Get a command (or Cancel) */
		if (!get_com(p, &command)) break;

		/* Convert various keys to "standard" keys */
		switch (command)
		{
			/* Use current target */
			case 'T':
			case 't':
			case '.':
			case '5':
			case '0':
			{
				dir = 5;
				break;
			}

			/* Set new target */
			case '*':
			{
				if (target_set_interactive(TARGET_KILL)) dir = 5;
				break;
			}

			default:
			{
				/* Look up the direction */
				dir = get_keymap_dir(command);

				break;
			}
		}

		/* Verify requested targets */
		if ((dir == 5) && !target_okay()) dir = 0;

		/* Error */
		if (!dir) bell("No Direction!");
	}

	/* No direction */
	if (!dir) return (FALSE);

	/* Save the direction */
	command_dir = dir;

	/* Check for confusion */
	if (p_ptr->confused)
	{
		/* XXX XXX XXX */
		/* Random direction */
		dir = ddd[rand_int(8)];
	}

	/* Notice confusion */
	if (command_dir != dir)
	{
		/* Warn the user */
		msg_print("You are confused.");
	}

	/* Save direction */
	(*dp) = dir;

	/* A "valid" direction was entered */
	return (TRUE);
}


/*
 * Request a "movement" direction (1,2,3,4,6,7,8,9) from the user.
 *
 * Return TRUE if a direction was chosen, otherwise return FALSE.
 *
 * This function should be used for all "repeatable" commands, such as
 * run, walk, open, close, bash, disarm, spike, tunnel, etc, as well
 * as all commands which must reference a grid adjacent to the player,
 * and which may not reference the grid under the player.
 *
 * Directions "5" and "0" are illegal and will not be accepted.
 *
 * This function tracks and uses the "global direction", and uses
 * that as the "desired direction", if it is set.
 */
bool get_rep_dir(int *dp)
{
	int dir;

	char ch;

	cptr p;

#ifdef ALLOW_REPEAT

	if (repeat_pull(dp))
	{
		return (TRUE);
	}

#endif /* ALLOW_REPEAT */

	/* Initialize */
	(*dp) = 0;

	/* Global direction */
	dir = p_ptr->command_dir;

	/* Get a direction */
	while (!dir)
	{
		/* Choose a prompt */
		p = "Direction (Escape to cancel)? ";

		/* Get a command (or Cancel) */
		if (!get_com(p, &ch)) break;

		/* Convert keypress into a direction */
		dir = target_dir(ch);

		/* Oops */
		if (!dir) bell("Illegal repeatable direction!");
	}

	/* Aborted */
	if (!dir) return (FALSE);

	/* Save desired direction */
	p_ptr->command_dir = dir;

	/* Save direction */
	(*dp) = dir;

#ifdef ALLOW_REPEAT

	repeat_push(dir);

#endif /* ALLOW_REPEAT */

	/* Success */
	return (TRUE);
}


/*
 * Apply confusion, if needed, to a direction
 *
 * Display a message and return TRUE if direction changes.
 */
bool confuse_dir(int *dp)
{
	int dir;

	/* Default */
	dir = (*dp);

	/* Apply "confusion" */
	if (p_ptr->confused)
	{
		/* Apply confusion XXX XXX XXX */
		if ((dir == 5) || (rand_int(100) < 75))
		{
			/* Random direction */
			dir = ddd[rand_int(8)];
		}
	}

	/* Notice confusion */
	if ((*dp) != dir)
	{
		/* Warn the user */
		msg_print("You are confused.");

		/* Save direction */
		(*dp) = dir;

		/* Confused */
		return (TRUE);
	}

	/* Not confused */
	return (FALSE);
}


