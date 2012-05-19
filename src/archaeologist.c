/****************************************************************
 * The Archaeologist
 ****************************************************************/

#include "angband.h"

/* Confirm 1 or 2 whip weapons for whip techniques.  Fail if no whip
   is worn, or if a non-whip weapon is warn.  Do not fail if shields
   or capture balls are equipped.  Handle empty inventory slots. */
static bool _whip_check(void)
{
	bool result = FALSE;
	if (inventory[INVEN_RARM].k_idx)
	{
		if ( inventory[INVEN_RARM].tval != TV_SHIELD
		  && inventory[INVEN_RARM].tval != TV_CAPTURE )
		{
			if (inventory[INVEN_RARM].tval == TV_HAFTED && inventory[INVEN_RARM].sval == SV_WHIP)
				result = TRUE;
			else
				return FALSE;
		}
	}
	if (inventory[INVEN_LARM].k_idx)
	{
		if ( inventory[INVEN_LARM].tval != TV_SHIELD
		  && inventory[INVEN_LARM].tval != TV_CAPTURE )
		{
			if (inventory[INVEN_LARM].tval == TV_HAFTED && inventory[INVEN_LARM].sval == SV_WHIP)
				result = TRUE;
			else
				return FALSE;
		}
	}
	return result;
}

/* A special fetch(), that places item in player's inventory */
static bool _whip_fetch(int dir, int rng)
{
	int             ty, tx;
	cave_type       *c_ptr;
	object_type     *o_ptr;
	char            o_name[MAX_NLEN];

	/* Use a target */
	if (dir == 5 && target_okay())
	{
		tx = target_col;
		ty = target_row;

		if (distance(py, px, ty, tx) > rng)
		{
			msg_print("You can't fetch something that far away!");
			return FALSE;
		}

		c_ptr = &cave[ty][tx];

		/* We need an item to fetch */
		if (!c_ptr->o_idx)
		{
			msg_print("There is no object at this place.");
			return TRUE;  /* didn't work, but charge the player energy anyway */
		}

		/* Fetching from a vault is OK */

		/* Line of sight is required */
		if (!player_has_los_bold(ty, tx))
		{
			msg_print("You have no direct line of sight to that location.");
			return FALSE;
		}
		else if (!projectable(py, px, ty, tx))
		{
			msg_print("You have no direct line of sight to that location.");
			return FALSE;
		}
	}
	else
	{
		/* Use a direction */
		ty = py; /* Where to drop the item */
		tx = px;

		do
		{
			ty += ddy[dir];
			tx += ddx[dir];
			c_ptr = &cave[ty][tx];

			if ((distance(py, px, ty, tx) > MAX_RANGE) ||
			    !in_bounds(ty, tx) ||
				!cave_have_flag_bold(ty, tx, FF_PROJECT))
			{
				return TRUE; /* didn't work, but charge the player energy anyway */
			}
		}
		while (!c_ptr->o_idx);
	}

	o_ptr = &o_list[c_ptr->o_idx];

	if (o_ptr->weight > p_ptr->lev * 15)
	{
		msg_print("The object is too heavy.");
		return TRUE; /* didn't work, but charge the player energy anyway */
	}

	object_desc(o_name, o_ptr, OD_NAME_ONLY);

	/* Get the object */
	if (!inven_carry_okay(o_ptr))
	{
		msg_format("You fail to fetch %^s since your pack is full.", o_name);
		/* Leave the object where it is */
	}
	else
	{
		msg_format("You skillfully crack your whip and fetch %^s.", o_name);
		py_pickup_aux(c_ptr->o_idx);
	}

	return TRUE;
}

static bool _sense_great_discovery(int range)
{
	int i, y, x;
	int range2 = range;

	bool detect = FALSE;

	if (d_info[dungeon_type].flags1 & DF1_DARKNESS) range2 /= 3;

	/* Scan objects */
	for (i = 1; i < o_max; i++)
	{
		object_type *o_ptr = &o_list[i];

		/* Skip dead objects */
		if (!o_ptr->k_idx) continue;

		/* Skip held objects */
		if (o_ptr->held_m_idx) continue;

		/* Only alert to great discoveries */
		if (!object_is_artifact(o_ptr)) continue;

		/* Only alert to new discoveries */
	    if (object_is_known(o_ptr)) continue;

		/* Location */
		y = o_ptr->iy;
		x = o_ptr->ix;

		/* Only detect nearby objects */
		if (distance(py, px, y, x) > range2) continue;

		/* Hack -- memorize it */
		o_ptr->marked |= OM_FOUND;

		/* Redraw */
		lite_spot(y, x);

		/* Detect */
		detect = TRUE;
	}

	return (detect);
}

/****************************************************************
 * Private Spells
 ****************************************************************/
static void _ancient_protection_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Ancient Protection");
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 50)
			var_set_string(res, "Sets a glyph on the floor beneath you. Monsters cannot attack you if you are on a glyph, but can try to break glyph.");
		else
			var_set_string(res, "Sets glyphs on nearby floors. Monsters cannot attack you if you are on a glyph, but can try to break glyph.");
		break;
	case SPELL_CAST:
		warding_glyph();
		if (p_ptr->lev >= 50)
			glyph_creation();
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _double_crack_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Double Crack");
		break;
	case SPELL_DESC:
		var_set_string(res, "Attack a monster normally with your whip, and then randomly attack an adjacent monster.");
		break;
	case SPELL_CAST:
		if (_whip_check())
		{
			int dir = 5;
			bool b = FALSE;

			if ( get_rep_dir2(&dir)
			  && dir != 5 )
			{
				int x, y;
				int num = 1;
				int attempts = 0;

				/* First we attack where the player selected */
				y = py + ddy[dir];
				x = px + ddx[dir];
				if (in_bounds(y, x) && cave[y][x].m_idx)
					py_attack(y, x, 0);
				else
					msg_print("Your whip cracks in empty air.");

				/* Now the whip cracks randomly!
				   Note that we favor fighting in hallways, or
				   with ones back up against the wall. */
				while (num > 0)
				{
					if (attempts > 3 * num)
					{
						while (num > 0)
						{
							msg_print("Your whip cracks in empty air.");
							num--;
						}
						break;
					}

					/* random direction, but we don't penalize for choosing the player (5) */
					dir = randint0(9);
					if (dir == 5) continue;
					
					attempts++;
					y = py + ddy[dir];
					x = px + ddx[dir];

					if ( !in_bounds(y, x) 
					  || cave_have_flag_bold(y, x, FF_WALL)
				      || cave_have_flag_bold(y, x, FF_TREE) 
					  || cave_have_flag_bold(y, x, FF_CAN_DIG) )
					{
						continue;
					}

					
					if (cave[y][x].m_idx)
						py_attack(y, x, 0);
					else
						msg_print("Your whip cracks in empty air.");

					num--;
				}

				b = TRUE;
			}
			var_set_bool(res, b);
		}
		else
		{
			msg_print("Whip techniques can only be used if you are fighting with whips.");
			var_set_bool(res, FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _evacuation_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Escape Rope");
		break;
	case SPELL_DESC:
		var_set_string(res, "Danger!  Abandon this expedition and escape to a new level.");
		break;
	case SPELL_CAST:
		var_set_bool(res, FALSE);
		if (get_check("Are you sure? (Escape Rope)"))
		{
			teleport_level(0);
			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _excavation_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Excavation");
		break;
	case SPELL_DESC:
		var_set_string(res, "You break walls on your quest for treasure!  This takes a bit more time, though.");
		break;
	case SPELL_ENERGY:
		{
			int n = 200;
			n -= 80 * p_ptr->lev / 50;	
			var_set_int(res, n);
		}
		break;
	case SPELL_CAST:
		{
			int dir = 5;
			bool b = FALSE;

			if ( get_rep_dir2(&dir)
			  && dir != 5 )
			{
				int x, y;
				y = py + ddy[dir];
				x = px + ddx[dir];

				if (!in_bounds(y, x))
				{
					msg_print("You may excavate no further.");
				}
				else if ( cave_have_flag_bold(y, x, FF_WALL)
				       || cave_have_flag_bold(y, x, FF_TREE) 
					   || cave_have_flag_bold(y, x, FF_CAN_DIG) )
				{
					msg_print("You dig your way to treasure!");
					cave_alter_feat(y, x, FF_TUNNEL);
					teleport_player_to(y, x, TELEPORT_NONMAGICAL); /*??*/
					b = TRUE;
				}
				else
				{
					msg_print("There is nothing to excavate.");
				}
			}
			var_set_bool(res, b);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _extended_whip_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Extended Crack");
		break;
	case SPELL_DESC:
		var_set_string(res, "This spell extends the range of your whip based melee attack.");
		break;
	case SPELL_CAST:
		if (_whip_check())
		{
			int dir = 5;
			bool b = FALSE;

			project_length = 2;
			if (get_aim_dir(&dir))
			{
				project_hook(GF_ATTACK, dir, HISSATSU_2, PROJECT_STOP | PROJECT_KILL);
				b = TRUE;
			}
			var_set_bool(res, b);
		}
		else
		{
			msg_print("Whip techniques can only be used if you are fighting with whips.");
			var_set_bool(res, FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _fetch_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Fetch");
		break;
	case SPELL_DESC:
		var_set_string(res, "Use your whip to fetch a nearby item.");
		break;
	case SPELL_CAST:
		if (_whip_check())
		{
			int dir = 5;
			bool b = FALSE;
			int rng = 3 + p_ptr->lev/25;

			project_length = rng;
			if (get_aim_dir(&dir))
			{
				b = _whip_fetch(dir, rng);
			}
			var_set_bool(res, b);
		}
		else
		{
			msg_print("Whip techniques can only be used if you are fighting with whips.");
			var_set_bool(res, FALSE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _first_aid_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "First Aid");
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 8)
			var_set_string(res, "Heals HP and Stun.");
		else if (p_ptr->lev < 12)
			var_set_string(res, "Heals HP and Stun.  Cures cuts.");
		else if (p_ptr->lev < 16)
			var_set_string(res, "Heals HP and Stun.  Cures cuts and slows poison.");
		else if (p_ptr->lev < 20)
			var_set_string(res, "Heals HP and Stun.  Cures cuts and poison.");
		else if (p_ptr->lev < 30)
			var_set_string(res, "Heals HP and Stun.  Cures cuts, poison and blindness.");
		else if (p_ptr->lev < 40)
			var_set_string(res, "Heals HP and Stun.  Cures cuts, poison and blindness.  Restores Con.");
		else if (p_ptr->lev < 45)
			var_set_string(res, "Heals HP and Stun.  Cures cuts, poison and blindness.  Restores Con and Chr.");
		else
			var_set_string(res, "Heals HP and Stun.  Cures cuts, poison and blindness.  Restores Con, Chr and Str.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Heals HP and Stun.  Slows Poison (L12).  Cures cuts (L8), poison (L16) and blindness (L20).  Restores Con (L30), Chr (L40) and Str (L45).");
		break;
	case SPELL_INFO:
		var_set_string(res, info_heal(0, 0, spell_power(p_ptr->lev)));
		break;
	case SPELL_CAST:
		hp_player(spell_power(p_ptr->lev));
		set_stun(0, TRUE);

		if (p_ptr->lev >= 8)
			set_cut(0, TRUE);
		if (p_ptr->lev >= 12 && p_ptr->lev < 16)
			set_poisoned(p_ptr->poisoned / 2, TRUE);
		if (p_ptr->lev >= 16)
			set_poisoned(0, TRUE);
		if (p_ptr->lev >= 20)
			set_blind(0, TRUE);
		if (p_ptr->lev >= 30)
			do_res_stat(A_CON);
		if (p_ptr->lev >= 40)
			do_res_stat(A_CHR);
		if (p_ptr->lev >= 45)
			do_res_stat(A_STR);

		var_set_bool(res, TRUE);
		break;
	case SPELL_COST_EXTRA:
		{
			int n = 0;
			if (p_ptr->lev > 49)
				n += 10;
			if (p_ptr->lev > 44)
				n += 4;
			if (p_ptr->lev > 39)
				n += 4;
			if (p_ptr->lev > 29)
				n += 4;
			if (p_ptr->lev > 19)
				n += 2;
			if (p_ptr->lev > 15)
				n += 2;
			if (p_ptr->lev > 11)
				n += 2;
			if (p_ptr->lev > 7)
				n += 2;
			var_set_int(res, n);
		}
		break;

	default:
		default_spell(cmd, res);
		break;
	}
}

static void _identify_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Identify");
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 25)
			var_set_string(res, "New Treasure!  You examine your new discovery.");
		else
			var_set_string(res, "New Treasure!  You examine your new discovery and learn its deepest truths.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Identifies an object. At L25, fully identifies an object.");
		break;
	case SPELL_CAST:
		{
			bool b = TRUE;
			if (p_ptr->lev < 25)
				b = ident_spell(FALSE);
			else
				b = identify_fully(FALSE);
			var_set_bool(res, b);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _magic_blueprint_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Magic Blueprint");
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 20)
			var_set_string(res, "A map to treasure!  Maps the surrounding area.");
		else if (p_ptr->lev < 25)
			var_set_string(res, "A map to treasure!  Maps the surrounding area and detects traps and doors.");
		else if (p_ptr->lev < 30)
			var_set_string(res, "A map to treasure!  Maps the surrounding area and detects traps, doors and objects.");
		else if (p_ptr->lev < 35)
			var_set_string(res, "A map to treasure!  Maps the entire level and detects traps, doors and objects.");
		else
			var_set_string(res, "A map to treasure!  Maps and lights the entire level and detects traps, doors and objects.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Maps nearby area or the entire level (L35). Detects treasure, traps (L20), doors (L20) and objects (25).");
		break;
	case SPELL_CAST:
		{
			int rad = DETECT_RAD_DEFAULT;

			if (p_ptr->lev >= 30)
				rad = DETECT_RAD_ALL;

			map_area(rad);
			detect_treasure(rad);
			detect_objects_gold(rad);
			if (p_ptr->lev >= 20)
			{
				detect_traps(rad, TRUE);
				detect_doors(rad);
			}
			if (p_ptr->lev >= 25)
				detect_objects_normal(rad);

			if (p_ptr->lev >= 35)
				wiz_lite(p_ptr->tim_superstealth > 0);	/* somewhat redundant, but I want level wide trap detection! */

			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _pharaohs_curse_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Pharaoh's Curse");
		break;
	case SPELL_DESC:
		var_set_string(res, "Curses all nearby monsters, doing great damage and various effects.");
		break;
	case SPELL_CAST:
		{
			int power = spell_power(p_ptr->lev * 4);
			project_hack(GF_PHARAOHS_CURSE, p_ptr->lev + randint1(p_ptr->lev));
			if (p_ptr->lev >= 46)
				confuse_monsters(power);
			if (p_ptr->lev >= 47)
				slow_monsters();
			if (p_ptr->lev >= 48)
				turn_monsters(power);
			if (p_ptr->lev >= 49)
				stun_monsters(power);
			if (p_ptr->lev >= 50)		/* originally, an extra 50hp damage was suggested */
				stasis_monsters(power);
			if (one_in_(5))
			{
				int mode = 0;
				if (one_in_(2))
					mode = PM_FORCE_PET;
				if (summon_named_creature(0, py, px, MON_GREATER_MUMMY, mode))
				{
					msg_print("You have disturbed the rest of an ancient pharaoh!");
				}
			}
			take_hit(DAMAGE_USELIFE, p_ptr->lev + randint1(p_ptr->lev), "the Pharaoh's Curse", -1);
			var_set_bool(res, TRUE);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _remove_curse_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Remove Curse");
		break;
	case SPELL_DESC:
		if (p_ptr->lev < 40)
			var_set_string(res, "Cursed Treasure!  Removes any weak curses from your equipment.");
		else
			var_set_string(res, "Cursed Treasure!  Removes any curses from your equipment.");
		break;
	case SPELL_SPOIL_DESC:
		var_set_string(res, "Removes weak curses. At L40, also removes heavy curses.");
		break;
	case SPELL_CAST:
		if (p_ptr->lev < 40)
		{
			if (remove_curse()) msg_print("You feel the curse has lifted.");
			else msg_print("Hmmm ... nothing happens.");
		}
		else
		{
			if (remove_all_curse()) msg_print("You feel the curse has lifted.");
			else msg_print("Hmmm ... nothing happens.");
		}
		var_set_bool(res, TRUE);
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}

static void _remove_obstacles_spell(int cmd, variant *res)
{
	switch (cmd)
	{
	case SPELL_NAME:
		var_set_string(res, "Remove Obstacles");
		break;
	case SPELL_DESC:
		var_set_string(res, "Clears a path to treasure!  Traps, doors and trees will be removed.");
		break;
	case SPELL_CAST:
		{
			bool b = FALSE;
			int dir = 5;
			if (get_aim_dir(&dir))
			{
				project(0, 1, py, px, 0, GF_REMOVE_OBSTACLE, PROJECT_GRID | PROJECT_ITEM | PROJECT_HIDE, -1);
				project_hook(GF_REMOVE_OBSTACLE, dir, 0, PROJECT_BEAM | PROJECT_GRID | PROJECT_ITEM);
				b = TRUE;
			}
			var_set_bool(res, b);
		}
		break;
	default:
		default_spell(cmd, res);
		break;
	}
}



/****************************************************************
 * Spell Table and Exports
 ****************************************************************/

#define MAX_ARCHAEOLOGIST_SPELLS	16

static spell_info _spells[MAX_ARCHAEOLOGIST_SPELLS] = 
{
    /*lvl cst fail spell */
	{  1,   3, 10, _extended_whip_spell },
	{  2,   3, 20, detect_traps_spell },
	{  3,   5, 20, light_area_spell },
	{  5,   5, 30, _first_aid_spell },
	{ 10,  10, 40, _identify_spell },
	{ 12,  10, 30, _remove_obstacles_spell },
	{ 13,  20, 30, _double_crack_spell },
	{ 15,  15, 30, _magic_blueprint_spell },
	{ 18,  10, 30, _excavation_spell },
	{ 22,  20, 30, _fetch_spell },
	{ 25,  20, 50, _remove_curse_spell },
	{ 32,  30, 70, recharging_spell },
	{ 35,  80, 70, _ancient_protection_spell },
	{ 40, 150, 80, polish_shield_spell },
	{ 42,  30, 50, _evacuation_spell },
	{ 45,  50, 80, _pharaohs_curse_spell }, /* No wizardstaff.  No spell skills! So, 8% best possible fail.*/
};

static int _get_spells(spell_info* spells, int max)
{
	int i;
	int ct = 0;
	int stat_idx = (p_ptr->stat_ind[A_INT] + p_ptr->stat_ind[A_WIS]) / 2;
	
	for (i = 0; i < MAX_ARCHAEOLOGIST_SPELLS; i++)
	{
		spell_info *base = &_spells[i];
		if (ct >= max) break;
		if (base->level <= p_ptr->lev)
		{
			spell_info* current = &spells[ct];
			current->fn = base->fn;
			current->level = base->level;
			current->cost = base->cost;

			current->fail = calculate_fail_rate(base->level, base->fail, stat_idx);			
			ct++;
		}
	}
	return ct;
}

static bool _is_favored_weapon(object_type *o_ptr)
{
	if (o_ptr->tval == TV_DIGGING)
		return TRUE;

	if (o_ptr->tval == TV_HAFTED && o_ptr->sval == SV_WHIP)
		return TRUE;

	return FALSE;
}

static void _process_player(void)
{
	bool sense = _sense_great_discovery(3 + p_ptr->lev/10);
	if (sense && !p_ptr->sense_artifact)
	{
		msg_print("You feel close to a great discovery!");
		p_ptr->sense_artifact = TRUE;
		p_ptr->redraw |= PR_STATUS;
	}
	else if (!sense && p_ptr->sense_artifact)
	{
		msg_print("You feel you are leaving something special behind...");
		p_ptr->sense_artifact = FALSE;
		p_ptr->redraw |= PR_STATUS;
	}
}

static void _calc_bonuses(void)
{
	p_ptr->see_infra += p_ptr->lev/10;
	p_ptr->skill_dig += 2*p_ptr->lev;
	if (p_ptr->lev >= 20)
		p_ptr->see_inv = TRUE;
	if (p_ptr->lev >= 38)
		p_ptr->resist_dark = TRUE;
}

static void _calc_weapon_bonuses(object_type *o_ptr, weapon_info_t *info_ptr)
{
	if (_is_favored_weapon(o_ptr))
	{
		info_ptr->to_h += 10 * p_ptr->lev/50;
		info_ptr->dis_to_h += 10 * p_ptr->lev/50;
		info_ptr->to_d += 10 * p_ptr->lev/50;
		info_ptr->dis_to_d += 10 * p_ptr->lev/50;

		if (p_ptr->lev >= 40)
			info_ptr->num_blow += 2;
		else if (p_ptr->lev >= 20)
			info_ptr->num_blow += 1;
	}
}

static caster_info * _caster_info(void)
{
	static caster_info me = {0};
	static bool init = FALSE;
	if (!init)
	{
		me.magic_desc = "spell";
		me.use_sp = TRUE;
		init = TRUE;
	}
	return &me;
}

static void _spoiler_dump(FILE* fff)
{
	int i;
	variant vn, vd;
	var_init(&vn);
	var_init(&vd);

	fprintf(fff, "\n== Spells ==\n");
	fprintf(fff, "|| *Power* || *Lvl* || *Mana* || *Fail* || *Description* ||\n");
	for (i = 0; i < MAX_ARCHAEOLOGIST_SPELLS; i++)
	{
		spell_info *base = &_spells[i];

		base->fn(SPELL_SPOIL_NAME, &vn);
		if (var_is_null(&vn)) base->fn(SPELL_NAME, &vn);
		
		base->fn(SPELL_SPOIL_DESC, &vd);
		if (var_is_null(&vd)) base->fn(SPELL_DESC, &vd);

		fprintf(fff, "||%s||%d||%d||%d||`%s`||\n", 
			var_get_string(&vn), base->level, base->cost, base->fail, var_get_string(&vd));
	}

	var_clear(&vn);
	var_clear(&vd);
}

class_t *archaeologist_get_class_t(void)
{
	static class_t me = {0};
	static bool init = FALSE;

	/* static info never changes */
	if (!init)
	{           /* dis, dev, sav, stl, srh, fos, thn, thb */
	skills_t bs = { 45,  50,  36,   4,  50,  32,  40,  20};
	skills_t xs = { 15,  12,  10,   0,   0,   0,  11,  11};

		me.name = "Archaeologist";
		me.desc = "The Archaeologist is an erudite treasure hunter, seeking out the most valuable "
		          "prizes that the dungeon has to offer. At home in subterranean caverns and vaults, "
				  "he is rarely lost or snared in traps. His powers of perception and detection are "
				  "very great, as is his skill with arcane devices. At high levels he can use the "
				  "dark magic of the entombed Pharaohs.";
		me.stats[A_STR] = -1;
		me.stats[A_INT] =  1;
		me.stats[A_WIS] =  1;
		me.stats[A_DEX] =  1;
		me.stats[A_CON] = -1;
		me.stats[A_CHR] =  1;
		me.base_skills = bs;
		me.extra_skills = xs;
		me.calc_bonuses = _calc_bonuses;
		me.calc_weapon_bonuses = _calc_weapon_bonuses;
		me.process_player = _process_player;
		me.caster_info = _caster_info;
		me.get_spells = _get_spells;
		me.spoiler_dump = _spoiler_dump;
		init = TRUE;
	}

	return &me;
}