/* File: init1.c */

/*
 * Copyright (c) 1997 Ben Harrison
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */ 

#include "angband.h"

/*
 * This file is used to initialize various variables and arrays for the
 * Angband game.  Note the use of "fd_read()" and "fd_write()" to bypass
 * the common limitation of "read()" and "write()" to only 32767 bytes
 * at a time.
 *
 * Several of the arrays for Angband are built from "template" files in
 * the "lib/file" directory, from which quick-load binary "image" files
 * are constructed whenever they are not present in the "lib/data"
 * directory, or if those files become obsolete, if we are allowed.
 *
 * Warning -- the "ascii" file parsers use a minor hack to collect the
 * name and text information in a single pass.  Thus, the game will not
 * be able to load any template file with more than 20K of names or 60K
 * of text, even though technically, up to 64K should be legal.
 *
 * Note that if "ALLOW_TEMPLATES" is not defined, then a lot of the code
 * in this file is compiled out, and the game will not run unless valid
 * "binary template files" already exist in "lib/data".  Thus, one can
 * compile Angband with ALLOW_TEMPLATES defined, run once to create the
 * "*.raw" files in "lib/data", and then quit, and recompile without
 * defining ALLOW_TEMPLATES, which will both save 20K and prevent people
 * from changing the ascii template files in potentially dangerous ways.
 *
 * The code could actually be removed and placed into a "stand-alone"
 * program, but that feels a little silly, especially considering some
 * of the platforms that we currently support.
 */

#ifdef ALLOW_TEMPLATES

#include "init.h"

/*** Helper arrays for parsing ascii template files ***/

/*
 * Monster Blow Methods
 */
static cptr r_info_blow_method[RBM_MAX] =
{
	"",
	"HIT",
	"TOUCH",
	"PUNCH",
	"KICK",
	"GRAB",
	"CLAW",
	"BITE",
	"STING",
	"KISS",
	"BUTT",
	"CRUSH",
	"ENGULF",
	"CRAWL",
	"DROOL",
	"SPIT",
	"GAZE",
	"WAIL",
	"SPORE",
	"BEG",
	"INSULT",
	"MOAN"
};


/*
 * Monster Blow Effects
 */
static cptr r_info_blow_effect[RBE_MAX] =
{
	"",
	"HURT",
	"POISON",
	"UN_BONUS",
	"CUT",
	"UN_POWER",
	"EAT_GOLD",
	"EAT_ITEM",
	"EAT_FOOD",
	"EAT_LITE",
	"ACID",
	"ELEC",
	"FIRE",
	"COLD",
	"BLIND",
	"CONFUSE",
	"TERRIFY",
	"PARALYZE",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_ALL",
	"SHATTER",
	"EXP_1",
	"EXP_2",
	"EXP_3",
	"EXP_4",
	"DISEASE",
	"RUST",
	"ROT",
	"HALLU",
	"TAINT"
};


/*
 * Monster race flags
 */
static cptr r_info_flags1[32] =
{
	"UNIQUE",
	"EGO",
	"ATTR_MIMIC",
	"ATTR_CLEAR",
	"ATTR_MULTI",
	"CHAR_CLEAR",
	"STUPID",
	"SMART",	
	"XXX1",
	"XXX2",	
	"MULTIPLY",
	"XXX3",
	"COMPANION",
	"GRP_6",	
	"GRP_12",
	"GRP_18",
	"GRP_ESCORT",
	"GRP_PEER",
	"RAND_25",		
	"RAND_50",		
	"ONLY_GOLD",	
	"ONLY_ITEM",
	"DROP_35",	
	"DROP_80",		
	"DROP_1",	
	"DROP_2",	
	"DROP_3",	
	"DROP_4",	
	"DROP_GOOD",
	"DROP_GREAT",
	"DROP_MIMIC",
	"DROP_CHOSEN"
};

/*
 * Monster race flags
 */
static cptr r_info_flags2[32] =
{
	"NEVER_BLOW",
	"NEVER_MOVE",
	"NEVER_FAIL",
	"OPEN_DOOR",
	"BASH_DOOR",
	"PICK_LOCK",
	"PASS_WALL",
	"KILL_WALL",
	"MOVE_BODY",
	"KILL_BODY",
	"TAKE_ITEM",
	"KILL_ITEM",
	"SEE_INVIS",
	"REGENERATE",
	"WIDE_BREATH",
	"FLYING",	
	"INVISIBLE",
	"COLD_BLOOD",
	"PASS_VEGETATION",
	"EMPTY_MIND",
	"WEIRD_MIND",
	"EVASIVE",
	"HAS_LITE",
	"HURT_LITE",
	"HURT_ROCK",
	"HURT_DARK",
	"HURT_ACID",
	"HURT_ELEC",
	"HURT_FIRE",
	"HURT_COLD",
	"BAD_JUMPER",
	"NO_TRAP"
};

/*
 * Monster race flags
 */
static cptr r_info_flags3[32] =
{
	"XXX1",
	"XXX2",
	"XXX3",
	"XXX4",
	"NO_BLIND",
	"NO_FEAR",
	"NO_STUN",
	"NO_SLEEP",
	"NO_CUT",
	"NO_CALM",
	"NO_CONF",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_POIS",
	"RES_NETH",
	"RES_LITE",
	"RES_DARK",
	"RES_SOUN",
	"RES_CHAO",
	"RES_DISE",
	"RES_NEXU",
	"RES_TIME",
	"RES_INER",
	"RES_GRAV",
	"RES_SHAR",
	"RES_PLAS",
	"RES_FORCE",
	"RES_MANA",
	"RES_WATER",
	"RES_DISEASE"
};

/*
 * Monster race flags
 */
static cptr r_info_flags4[32] =
{
	"EVIL",
	"CHAOS",
	"LIVING",
	"PERSON",
	"HUMANOID",
	"FAERY",
	"DRAGON",
	"DEMON",
	"UNDEAD",
	"ANIMAL",
	"PLANT",
	"LYCANTHROPE",
	"AETHER",
	"PRIEST",
	"WARRIOR",
	"ROGUE",
	"MAGE",
	"THORNWILD",
	"SKULTGARD",
	"SET_TRAPS",
	"XXX6",
	"XXX7",
	"XXX8",
	"XXX9",
	"XXX10",
	"XXX11",
	"XXX12",
	"XXX13",
	"XXX14",
	"XXX15",
	"MALE",
	"FEMALE",
};

/*
 * Monster race flags
 */
static cptr r_info_s_flags1[32] =
{
	"SHRIEK",
	"JAVELIN_1",
	"JAVELIN_2",
	"JAVELIN_3",
	"ARROW_1",
	"ARROW_2",
	"ARROW_3",
	"ARROW_4",
	"BR_ACID",
	"BR_ELEC",
	"BR_FIRE",
	"BR_COLD",
	"BR_POIS",
	"BR_NETH",
	"BR_LITE",
	"BR_DARK",
	"BR_SOUN",
	"BR_CHAO",
	"BR_DISE",
	"BR_NEXU",
	"BR_TIME",
	"BR_INER",
	"BR_GRAV",
	"BR_SHAR",
	"BR_PLAS",
	"BR_FORCE",
	"BR_MANA",
	"BR_WATER",
	"BR_DISEASE",
	"XXX4",
	"XXX5",
	"XXX6"
};

/*
 * Monster race flags
 */
static cptr r_info_s_flags2[32] =
{
	"BA_ACID",
	"BA_ELEC",
	"BA_FIRE",
	"BA_COLD",
	"BA_POIS",
	"BA_NETH",
	"BA_WATER",
	"BA_MANA",
	"BA_DARK",
	"DRAIN_MANA",
	"MIND_BLAST",
	"BRAIN_SMASH",
	"CAUSE_1",
	"CAUSE_2",
	"CAUSE_3",
	"CAUSE_4",
	"BO_ACID",
	"BO_ELEC",
	"BO_FIRE",
	"BO_COLD",
	"XXX1",
	"BO_NETH",
	"BO_WATER",
	"BO_MANA",
	"BO_PLAS",
	"BO_ICEE",
	"MISSILE",
	"SCARE",
	"BLIND",
	"CONF",
	"SLOW",
	"HOLD"
};

/*
 * Monster race flags
 */
static cptr r_info_s_flags3[32] =
{
	"HASTE",
	"XXX1",
	"HEAL",
	"XXX2",
	"BLINK",
	"TPORT",
	"BLINK_TO",
	"XXX3",
	"TELE_TO",
	"TELE_AWAY",
	"TELE_LEVEL",
	"DARKNESS",
	"FORGET",
	"TRAPS1",
	"TRAPS2",
	"TRAPS3",
	"S_KIN",
	"S_HI_DEMON",
	"S_MONSTER",
	"S_MONSTERS",
	"S_ANIMALS",
	"S_SPIDER",
	"S_HOUND",
	"S_FAERY",
	"S_HYDRA",
	"S_HORROR",
	"S_DEMON",
	"S_UNDEAD",
	"S_DRAGON",
	"S_HI_UNDEAD",
	"S_HI_DRAGON",
	"S_UNIQUE"
};

/*
 * Class flags
 */
static cptr c_info_flags[32] =
{
	"EXTRA_SHOT",
	"BRAVERY_30",
	"BLESS_WEAPON",
	"NO_GLOVE",
	"POWER",
	"EXTRA_MANA",
	"ZERO_FAIL",
	"BEAM",
	"MYSTIC_CAST",
	"EXTRA_SPELL",
	"FENCING",
	"CHOOSE_SPELLS",
	"SUB_SPELLS",
	"RITUAL_EXPERT",
	"BETTER_CRITICAL",
	"BETTER_SHOT",
	"BETTER_THROW",
	"INFLUENCE",
	"TRAP_KNOW",
	"TRAP_PLACE",
	"APPRAISE",
	"POVERTY",
	"BETTER_BLOWS",
	"WORSE_BLOWS",
	"WEAPON_GOOD",
	"WEAPON_NONE",
	"SHROOM_MAGIC",
	"AMBUSH",
	"RELIGION_EXPERT",
	"BREAK_2",
	"BREAK_3",
	"BREAK_4"
};

/*
 * Widget flags
 */
static cptr w_info_flags[32] =
{
	"PLAYER",
	"MONSTER",
	"DECORATE_FLOOR",	
	"DECORATE_RUBBLE",
	"FLOOR",	
	"GLYPH",
	"CHEST",	
	"LOCK",
	"DECORATION",	
	"BORING",
	"TRAP_DOOR",
	"PIT",			
	"RUNE",	
	"SPOT",	
	"DART",			
	"GAS",			
	"SLOTS",	
	"ROCKS",	
	"DECORATE_SECRET_DOOR",
	"NO_HACK",	
	"TRAPS1",	
	"TRAPS2",
	"TRAPS3",
	"DECORATE_STAIRS_UP",
	"DECORATED_WALL",	
	"DECORATE_WALL_OUTER",	
	"DECORATE_WALL_INNER",	
	"ATTR_CLEAR",
	"DECORATE_OPEN_DOOR",
	"DECORATE_CLOSED_DOOR",
	"DETECT",
	"DISARM"
};

/*
 * Prefix flags
 */
static cptr px_info_flags[8] =
{
	"SHOP",
	"GOOD",
	"XXX1",			
	"XXX2",			
	"XXX3",			
	"XXX4",			
	"XXX5",			
	"XXX6"
};

/*
 * Object flags
 */
static cptr k_info_flags1[32] =
{
	"STR",
	"INT",
	"WIS",
	"DEX",
	"CON",
	"CHR",
	"MELEE",
	"MISSILE_SKILL",
	"STEALTH",
	"PERCEPTION",
	"INFRA",
	"ESCAPES",
	"SPEED",
	"BLOWS",
	"SHOTS",
	"MANA",
	"HEALTH",
	"POWDER_RADIUS",
	"SP_DUR",
	"SP_DAM",
	"SP_INF",
	"JUMPING",
	"BOW_THROWN_RANGE",
	"MIGHT",
	"RANGE",
	"AMBUSH",
	"SUST_STR",
	"SUST_INT",
	"SUST_WIS",
	"SUST_DEX",
	"SUST_CON",
	"SUST_CHR"
};
	
/*
 * Object flags
 */
static cptr k_info_flags2[32] =
{
	"FREE_ACT",
	"HOLD_LIFE",
	"BRAVERY",
	"NO_BLIND",
	"NO_DISEASE",
	"NO_STUN",
	"NO_POISON",
	"NO_CUT",
	"NO_CONF",
	"XXX1",
	"XXX2",
	"XXX3",
	"XXX4",
	"XXX5",
	"XXX6",
	"XXX7",
	"XXX8",
	"XXX9",
	"THROWING",
	"XXX10",
	"XXX11",
	"XXX12",
	"XXX13",
	"XXX14",
	"XXX15",
	"XXX16",
	"KILL_MIST",
	"DEADLY_CRIT",
	"BLESSED",
	"WOUNDING",
	"TERROR",
	"IMPACT"
};

/*
 * Object flags
 */
static cptr k_info_flags3[32] =
{
	"PRO_CHAOS",
	"FEATHER",
	"REGEN",
	"TELEPATHY",
	"SEE_INVIS",
	"INVIS",
	"GLOW",
	"LUCK",
	"LITE1",
	"LITE2",
	"LITE3",
	"LITE4",
	"IGNORE_ELEM",
	"IGNORE_NON_ELEM",
	"IGNORE_DISEN",
	"FAERY",
	"PRO_THORNWILD",
	"EASY_KNOW",
	"HIDE_TYPE",
	"PRO_SKULTGARD",
	"PRO_AETHER",
	"DISSOLVE_MIST",
	"MIGHTY_THROW",
	"TAINT",
	"DRAIN_ITEM",
	"DISRUPT",
	"TELEPORT",
	"AGGRAVATE",
	"DRAIN_EXP",
	"LIGHT_CURSE",
	"HEAVY_CURSE",
	"PERMA_CURSE"
};

/*
 * Activation type
 */
static cptr k_info_act[POW_MAX] =
{
	"XXXXXX",
	"HEAL_1",
	"HEAL_2",
	"HEAL_3",
	"HEAL_4",
	"HEAL_5",
	"HEAL_CURE_1",
	"HEAL_CURE_2",
	"HEAL_CURE_3",
	"HEAL_CURE_4",
	"LIFE",
	"RESTORE_MANA",
	"RESTORE_MANA_INT",
	"RESTORE_STR",
	"RESTORE_INT",
	"RESTORE_WIS",
	"RESTORE_DEX",
	"RESTORE_CON",
	"RESTORE_CHR",
	"RESTORE_STATS",
	"RESTORE_LEVEL",
	"RESTORE_ALL",
	"GAIN_STR",
	"GAIN_INT",
	"GAIN_WIS",
	"GAIN_DEX",
	"GAIN_CON",
	"GAIN_CHR",
	"GAIN_ALL",
	"GAIN_EXP",
	"CURE_CONFUSION",
	"CURE_DISEASE",
	"CURE_POISON",
	"CURE_POIS_DISE",
	"CURE_FEAR_POIS",
	"CURE_TAINT",
	"CURE_ALL",
	"CURE_BODY",
	"CLEAR_MIND",
	"TELE_10",
	"TELE_MINOR",
	"TELE_MAJOR",
	"TELE_OTHER",
	"TELE_OTHER_BEAM",
	"TELE_LEVEL",
	"TELE_CONTROL",
	"WORD_RECALL",
	"ALTER_REALITY",
	"ARROW",
	"BOLT_MISSILE",
	"BOLT_MISSILE_X",
	"BOLT_ACID_1",
	"BOLT_ACID_2",
	"BOLT_ACID_X",
	"BOLT_ELEC",
	"BOLT_ELEC_X",
	"BOLT_FIRE_1",
	"BOLT_FIRE_2",
	"BOLT_FIRE_3",
	"BOLT_FIRE_X",
	"BOLT_COLD_1",
	"BOLT_COLD_2",
	"BOLT_COLD_X",
	"BOLT_POISON_X",
	"BOLT_SOUND",
	"BOLT_FORCE_1",
	"BOLT_FORCE_2",
	"BOLT_LITE",
	"BOLT_DARK",
	"BOLT_WATER",
	"BOLT_MANA",
	"BOLT_MANA_X",
	"BOLT_NEXUS_X",
	"BEAM_WEAK_LITE",
	"BEAM_NETHER",
	"BALL_ACID",
	"BALL_ACID_X",
	"BALL_ELEC_1",
	"BALL_ELEC_2",
	"BALL_ELEC_X",
	"BALL_FIRE_1",
	"BALL_FIRE_2",
	"BALL_FIRE_3",
	"BALL_FIRE_X",
	"BALL_COLD_1",
	"BALL_COLD_2",
	"BALL_COLD_3",
	"BALL_COLD_X",
	"BALL_POISON",
	"BALL_POISON_X",
	"BALL_SOUND",
	"BALL_PLASMA",
	"BALL_MANA",
	"BALL_HOLY_1",
	"BALL_HOLY_2",
	"BALL_COLD_ELEC_X",
	"BALL_FIRE_ACID_X",
	"BALL_ELEM_X",
	"BALL_ANNIHILATION",
	"STAR_BEAM_W_LITE",
	"STAR_BALL_ELEC",
	"BANISH",
	"BLIGHT",
	"BURST_ASTRAL",
	"DRAIN_LIFE_1",
	"DRAIN_LIFE_2",
	"DRAIN_LIFE_3",
	"DISPEL_ALL",
	"DISPEL_UNDEAD_1",
	"DISPEL_UNDEAD_2",
	"DISPEL_DEMON",
	"DISPEL_NON_EVIL",
	"DISPEL_EVIL_3",
	"DISPEL_EVIL_4",
	"DISPEL_EVIL_5",
	"HOLY_1",
	"HOLY_2",
	"GENOCIDE",
	"MASS_GENOCIDE",
	"EARTHQUAKE",
	"DESTRUCTION",
	"LIGHT_AREA_1",
	"LIGHT_AREA_2",
	"DARK_AREA",
	"DETECT_MONSTERS",
	"DETECT_EVIL",
	"DETECT_INVIS",
	"DETECT_TRAP",
	"DETECT_TREASURE",
	"DETECT_DOOR_STAIR",
	"DETECT_TRAP_DOOR",
	"DETECT_ITEM",
	"DETECT_ENCHANT",
	"DETECT_ALL",
	"ABSORB_HIT",
	"BLESS_1",
	"BLESS_2",
	"BLESS_3",
	"HEROISM",
	"BOLDNESS",
	"STABILITY",
	"SAFETY",
	"RAGE_1",
	"RAGE_2",
	"RAGE_BLESS_RESIST",
	"SHIELD",
	"INVIS_1",
	"INVIS_2",
	"RESILIENCE",
	"INFRAVISION",
	"STEALTH",
	"SEE_INVIS",
	"PROT_EVIL_1",
	"PROT_EVIL_2",
	"HASTE_SELF_1",
	"HASTE_SELF_2",
	"HASTE_SELF_3",
	"DISARM",
	"DEST_TRAP_DOOR_1",
	"DEST_TRAP_DOOR_2",
	"STONE_TO_MUD",
	"CREATE_WALL",
	"CREATE_DOOR",
	"CREATE_STAIR",
	"CREATE_TRAP",
	"MAGIC_LOCK",
	"ACQUIRE_1",
	"ACQUIRE_2",
	"AGGRAVATE",
	"AGGRAVATE_SAFE",
	"CONFUSE_MONSTER",
	"CONFUSE_ALL",
	"SLEEP_MONSTER",
	"SLEEP_ADJACENT",
	"SLEEP_ALL",
	"SLOW_MONSTER",
	"SLOW_ALL",
	"CALM_MONSTER",
	"CALM_ANIMALS",
	"CALM_NON_EVIL",
	"CALM_NON_CHAOS",
	"CALM_ALL",
	"BLIND_MONSTER",
	"SCARE_MONSTER",
	"SCARE_ALL",
	"HARPOON",
	"POLY_MONSTER",
	"HEAL_MONSTER",
	"HASTE_MONSTER",
	"CLONE_MONSTER",
	"SATISFY_HUNGER",
	"RECHARGE_1",
	"RECHARGE_2",
	"RECHARGE_3",
	"HYPERCHARGE",
	"IDENTIFY",
	"IDENTIFY_PACK",
	"IDENTIFY_FULL",
	"RES_ACID",
	"RES_ELEC",
	"RES_FIRE",
	"RES_COLD",
	"RES_ACID_ELEC",
	"RES_FIRE_COLD",
	"RES_POISON",
	"RES_DISEASE",
	"RES_SOUND",
	"RES_LITE_DARK",
	"RES_CHAOS_NEXUS",
	"RES_ELEMENTS",
	"RES_GREATER",
	"RESISTANCE",
	"GLYPH_WARDING",
	"GLYPH_LESSER",
	"GLYPH_HOLY",
	"REMOVE_CURSE_1",
	"REMOVE_CURSE_2",
	"MAP_1",
	"MAP_2",
	"MAP_3",
	"PROBE_MONSTER",
	"PROBE_ALL",
	"KNOW_ALL",
	"ENCHANT_WEAPON_1",
	"ENCHANT_WEAPON_2",
	"ENCHANT_ARMOR_1",
	"ENCHANT_ARMOR_2",
	"BRAND_WEAPON_ELMNT",
	"BRAND_AMMO_ANML",
	"BRAND_AMMO_WOUND",
	"BRAND_AMMO_ELMNT",
	"BRAND_AMMO_HOLY",
	"BIZZARE",
	"CURSE_EQUIP_1",
	"CURSE_EQUIP_2",
	"SUM_MONSTER",
	"SUM_UNDEAD",
	"SUM_DRAGON",
	"NAUSEA",
	"POISON_SELF",
	"BLIND_SELF",
	"CONFUSE_SELF",
	"SCARE_SELF",
	"SLOW_SELF",
	"PARALYZE",
	"HALLUCINATE",
	"DISEASE",
	"DEFORM",
	"TAINT",
	"AMNESIA",
	"LOSE_STR",
	"LOSE_INT",
	"LOSE_WIS",
	"LOSE_DEX",
	"LOSE_CON",
	"LOSE_CHR",
	"LOSE_EXP",
	"RUINATION",
	"DETONATE",
	"KILL_SELF",
	"SPELL_DURATION",
	"SPELL_DAMAGE",
	"SPELL_INFLUENCE",
	"DRAGON_BLACK",
	"DRAGON_BLUE",
	"DRAGON_WHITE",
	"DRAGON_RED",
	"DRAGON_GREEN",
	"DRAGON_GOLD",
	"DRAGON_SILVER",
	"DRAGON_MH",
	"DRAGON_SPIRIT",
	"DRAGON_SHADOW",
	"DRAGON_ETHER",
	"DRAGON_CHAOS",
	"DRAGON_TIME",
	"DRAGON_POWER",
	"RISK_HACK",
	"WONDER_HACK",
	"MUSIC_LYRE",
	"MUSIC_HORN",
	"MUSIC_FLUTE",	
	"MUSIC_LUTE",
	"MUSIC_DRUM",
	"MUSIC_HARP",
	"BRAWN",
	"INTELLECT",
	"CONTEMPLATION",
	"NIMBLENESS",
	"TOUGHNESS",
	"PLEASING",
	"SHRPOISON",
	"SHRBLIND",
	"SHRSCARE",
	"SHRCONFUSE",
	"SHRHALLUCINATE",
	"SHRPARALYZE",
	"SHRNAIVITY",
	"SHRSTUPIDITY",
	"SHRAMNESIA",
	"SHRDISEASE",
	"SHRANTIDOTE",
	"SHRFORTIFICATION",
	"SHRCURE_CONFUSION",
	"SHRHEAL_1",
	"SHRHEAL_2",
	"SHRSHIELD",
	"SHRCLEAR_MIND",
	"SHRRESTORE_STR",
	"SHRRESTORE_CON",
	"SHRRESTORE_DEX",
	"SHRRESTORE_STATS",
	"PHLOGISTON",
	"RESTORE_VIGOR",
	"RESTORE_WIT",
	"RESTORE_GRACE",
	"POWDER_HASTE",
	"POWDER_HEAL",
	"POWDER_CONFUSING",
	"POWDER_SLEEPING",
	"POWDER_CALMING",
	"POWDER_TRANSFORMING",
	"POWDER_FLASH",
	"POWDER_DARKNESS",
	"POWDER_POISONING",
	"POWDER_BURNING",
	"POWDER_FREEZING",
	"POWDER_INCINERATION",
	"POWDER_ICE_BLAST",
	"POWDER_ENERGY",
	"OIL_LANTERN",
	"OIL_BURNING",
	"SHIFT",
	"ALERTNESS",
	"CAVE_LORE",
	"DUNGEON_LORE",
	"ANALYSE_ITEM",
	"RITUAL_STR",
	"RITUAL_INT",
	"RITUAL_WIS",
	"RITUAL_DEX",
	"RITUAL_CON",
	"RITUAL_CHR",
	"RITUAL_AUGMENT_BODY",
	"RITUAL_MIND_OVER_BODY",
	"RITUAL_FORTIFICATION",
	"RITUAL_NIGHT_SIGHT",
	"RITUAL_ACQUIRE_ARMOR",
	"RITUAL_CREATE_POWDER",
	"RITUAL_CURE_WOUND",
	"RITUAL_ACQUIRE_WEAPON",
	"RITUAL_FORBIDDEN_LORE",
	"EDEN",
	"STORM_SHIELD",
	"DETECT_FORCE",
	"LORE",
	"GROWTH",
	"DRYAD",
	"CURSE_MONSTER",
	"SWAP_PLACES",
	"BALL_STUN",
	"HARPY_FORM",
	"ANGEL_FORM",
	"APE_FORM",
	"NAGA_FORM",
	"STATUE_FORM",
	"FAUN_FORM",
	"GOBLIN_FORM",
	"GHOUL_FORM",
	"IDENTIFY_SKILL",
	"ARCHERY",
	"FLAMING_HANDS",
	"ICY_HANDS",
	"RAY_MANA",
	"EARTHBIND",
	"DRAGONSLAYER",
	"CURSE_ALL",
	"TURN_UNLIFE",
	"PROT_CHAOS_2",
	"RAT_THING",
};

/*** Initialize from ascii template files ***/

/*
 * Initialize an "*_info" array, by parsing an ascii "template" file
 */
errr init_info_txt(FILE *fp, char *buf, header *head,
                   parse_info_txt_func parse_info_txt_line)
{
	errr err;

	/* Not ready yet */
	bool okay = FALSE;

	/* Just before the first record */
	error_idx = -1;

	/* Just before the first line */
	error_line = 0;

	/* Prepare the "fake" stuff */
	head->name_size = 0;
	head->text_size = 0;

	/* Parse */
	while (0 == my_fgets(fp, buf, 1024))
	{
		/* Advance the line number */
		error_line++;

		/* Skip comments and blank lines */
		if (!buf[0] || (buf[0] == '#')) continue;

		/* Verify correct "colon" format */
		if (buf[1] != ':') return (PARSE_ERROR_GENERIC);

		/* Hack -- Process 'V' for "Version" */
		if (buf[0] == 'V')
		{
			int v1, v2, v3;

			/* Scan for the values */
			if ((3 != sscanf(buf+2, "%d.%d.%d", &v1, &v2, &v3)) ||
				(v1 != head->v_major) ||
				(v2 != head->v_minor) ||
				(v3 != head->v_patch))
			{
				return (PARSE_ERROR_OBSOLETE_FILE);
			}

			/* Okay to proceed */
			okay = TRUE;

			/* Continue */
			continue;
		}

		/* No version yet */
		if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Parse the line */
		err = (*parse_info_txt_line)(buf, head);

		if (err != 0) return (err);
	}

	/* Complete the "name" and "text" sizes */
	if (head->name_size) head->name_size++;
	if (head->text_size) head->text_size++;

	/* No version yet */
	if (!okay) return (PARSE_ERROR_OBSOLETE_FILE);

	/* Success */
	return (0);
}

/*
 * Add a text to the text-storage and store offset to it.
 *
 * Returns FALSE when there isn't enough space available to store
 * the text.
 */
static bool add_text(u32b *offset, header *head, cptr buf)
  {
  	/* Hack -- Verify space */
  	if (head->text_size + strlen(buf) + 8 > z_info->fake_text_size)
 		return (FALSE);
 
 	/* New text? */
 	if (*offset == 0)
 	{
 		/* Advance and save the text index */
 		*offset = ++head->text_size;	
 	}
  
  	/* Append chars to the text */
  	strcpy(head->text_ptr + head->text_size, buf);
  
  	/* Advance the index */
  	head->text_size += strlen(buf);
 
 	/* Success */
 	return (TRUE);
}

/*
 * Add a name to the name-storage and return an offset to it.
 *
 * Returns 0 when there isn't enough space available to store
 * the name.
 */
static u32b add_name(header *head, cptr buf)
{
	u32b index;

	/* Hack -- Verify space */
	if (head->name_size + strlen(buf) + 8 > z_info->fake_name_size)
		return (0);

	/* Advance and save the name index */
	index = ++head->name_size;

	/* Append chars to the names */
	strcpy(head->name_ptr + head->name_size, buf);

	/* Advance the index */
	head->name_size += strlen(buf);
	
	/* Return the name index */
	return (index);
}

/*
 * Initialize the "z_info" structure, by parsing an ascii "template" file
 */
errr parse_z_info(char *buf, header *head)
{
	maxima *z_info = head->info_ptr;

	/* Hack - Verify 'M/S:x:' format */
	if (!buf[2]) return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	if (buf[3] != ':') return (PARSE_ERROR_UNDEFINED_DIRECTIVE);

	if (buf[0] == 'M')
	{
		/* Process 'F' for "Maximum f_info[] index" */
		if (buf[2] == 'F')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->f_max = max;
		}

		/* Process 'D' for "Maximum d_info[] index" */
		else if (buf[2] == 'D')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->d_max = max;
		}

		/* Process 'K' for "Maximum k_info[] index" */
		else if (buf[2] == 'K')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->k_max = max;
		}

		/* Process 'A' for "Maximum a_info[] index" */
		else if (buf[2] == 'A')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->a_max = max;
		}

		/* Process 'C' for "Maximum c_info[] index" */
		else if (buf[2] == 'C')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->c_max = max;
		}

		/* Process 'E' for "Maximum e_info[] index" */
		else if (buf[2] == 'E')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->e_max = max;
		}

		/* Process 'X' for "Maximum px_info[] index" */
		else if (buf[2] == 'X')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->wpx_max = max;
		}

		/* Process 'Y' for "Maximum apx_info[] index" */
		else if (buf[2] == 'Y')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->apx_max = max;
		}

		/* Process 'W' for "Maximum w_info[] index" */
		else if (buf[2] == 'W')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->w_max = max;
		}

		/* Process 'R' for "Maximum r_info[] index" */
		else if (buf[2] == 'R')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->r_max = max;
		}

		/* Process 'S' for "Maximum s_info[] index" */
		else if (buf[2] == 'S')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->s_max = max;
		}

		/* Process 'U' for "Maximum u_info[] index" */
		else if (buf[2] == 'U')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->u_max = max;
		}

		/* Process 'V' for "Maximum v_info[] index" */
		else if (buf[2] == 'V')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->v_max = max;
		}

		/* Process 'P' for "Maximum p_info[] index" */
		else if (buf[2] == 'P')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->p_max = max;
		}

		/* Process 'H' for "Maximum h_info[] index" */
		else if (buf[2] == 'H')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->h_max = max;
		}

		/* Process 'B' for "Maximum b_info[] subindex" */
		else if (buf[2] == 'B')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->b_max = max;
		}

		/* Process 'Q' for "Maximum q_info[] index" */
		else if (buf[2] == 'Q')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->q_max = max;
		}

		/* Process 'O' for "Maximum o_list[] index" */
		else if (buf[2] == 'O')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->o_max = max;
		}

		/* Process 'M' for "Maximum m_list[] index" */
		else if (buf[2] == 'M')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->m_max = max;
		}

		/* Process 'T' for "Maximum t_list[] index" */
		else if (buf[2] == 'T')
		{
			int max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->t_max = max;
		}

		/* Process 'N' for "Fake name size" */
		else if (buf[2] == 'N')
		{
			long max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->fake_name_size = max;
		}

		/* Process 'T' for "Fake text size" */
		else if (buf[2] == '?')
		{
			long max;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%ld", &max)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->fake_text_size = max;
		}
		else
		{
			/* Oops */
			return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
		}
	}

	else if (buf[0] == 'S')
	{
		/* Process 'A' for "Minimum normal artifact" index" */
		if (buf[2] == 'A')
		{
			int min_norm;

			/* Scan for the value */
			if (1 != sscanf(buf+4, "%d", &min_norm)) return (PARSE_ERROR_GENERIC);

			/* Save the value */
			z_info->a_min_normal = min_norm;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "v_info" array, by parsing an ascii "template" file
 */
errr parse_v_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static vault_type *v_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		v_ptr = (vault_type*)head->info_ptr + i;

		/* Store the name */
		if (!(v_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current v_ptr */
		if (!v_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&v_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'X' for "Extra info" (one line only) */
	else if (buf[0] == 'X')
	{
		int typ, rat, hgt, wid;

		/* There better be a current v_ptr */
		if (!v_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &typ, &rat, &hgt, &wid)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		v_ptr->typ = typ;
		v_ptr->rat = rat;
		v_ptr->hgt = hgt;
		v_ptr->wid = wid;

		/* Check for maximum vault sizes */
		if ((v_ptr->typ == 7) && ((v_ptr->wid > 33) || (v_ptr->hgt > 22)))
			return (PARSE_ERROR_VAULT_TOO_BIG);

		if ((v_ptr->typ == 8) && ((v_ptr->wid > 66) || (v_ptr->hgt > 44)))
			return (PARSE_ERROR_VAULT_TOO_BIG);

		if ((v_ptr->typ == 9) && ((v_ptr->wid > 33) || (v_ptr->hgt > 22)))
			return (PARSE_ERROR_VAULT_TOO_BIG);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "f_info" array, by parsing an ascii "template" file
 */
errr parse_f_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static feature_type *f_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		f_ptr = (feature_type*)head->info_ptr + i;

		/* Store the name */
		if (!(f_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Default "mimic" */
		f_ptr->mimic = i;
	}

	/* Process 'M' for "Mimic" (one line only) */
	else if (buf[0] == 'M')
	{
		int mimic;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d",
			            &mimic)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->mimic = mimic;
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		int tmp;

		/* There better be a current f_ptr */
		if (!f_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		f_ptr->d_attr = tmp;
		f_ptr->d_char = buf[2];
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag from a textual string
 */
static errr grab_one_flag(u32b *flags, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 32; i++)
	{
		if (streq(what, names[i]))
		{
			*flags |= (1L << i);
			return (0);
		}
	}

	return (-1);
}

/*
 * Grab one flag from a textual string
 */
static errr grab_one_short_flag(byte *flags, cptr names[], cptr what)
{
	int i;

	/* Check flags */
	for (i = 0; i < 8; i++)
	{
		if (streq(what, names[i]))
		{
			*flags |= (1L << i);
			return (0);
		}
	}

	return (-1);
}

/*
 * Initialize the "d_info" array, by parsing an ascii "template" file
 */
errr parse_d_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static desc_type *d_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		int prv, nxt, prc, min;

		/* Hack - get the index */
		i = error_idx + 1;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		d_ptr = (desc_type*)head->info_ptr + i;

		/* Scan for the values */
		if (4 != sscanf(buf, "N:%d:%d:%d:%d",
			&prv, &nxt, &prc, &min)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		d_ptr->chart = prv;
		d_ptr->next = nxt;
		d_ptr->roll = prc;
		d_ptr->level = min;

		/* Initialize other values */
		d_ptr->seen = FALSE;
		d_ptr->tval = 0;
		d_ptr->feat = 0;
		d_ptr->r_flag = 0;
		d_ptr->r_char = 0;
	}
	/* Process 'A' for "Name1" */
	else if (buf[0] == 'A')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;
			
		/* Store the name */
		if (!(d_ptr->name1 = add_name(head, s)))
		
		return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	/* Process 'B' for "Name2" */
	else if (buf[0] == 'B')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;
			
		/* Store the name */
		if (!(d_ptr->name2 = add_name(head, s)))
		
		return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&d_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	/* Process 'S' for "Seeable" (one line only) */
	else if (buf[0] == 'S')
	{
		char yes;

		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		yes = buf[2];

		/* Check visible */
		if (yes == 'Y')
		{
			d_ptr->seen = TRUE;
		}
		else if (yes == 'N')
		{
			d_ptr->seen = FALSE;
		}
		else
		{
			return (PARSE_ERROR_GENERIC);
		}
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		d_ptr->r_char = buf[2];
	}

	/* Process 'K' for "Kind" (one line only) */
	else if (buf[0] == 'K')
	{
		int kind;

		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d", &kind)) return (1);

		/* Save the values */
		d_ptr->tval = kind;
	}

	/* Process 'F' for "Feature" (one line only) */
	else if (buf[0] == 'F')
	{
		int feat;

		/* There better be a current d_ptr */
		if (!d_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%d", &feat)) return (1);

		/* Save the values */
		d_ptr->feat = feat;
	}

	/* Process 'R' for "Race flag" (once only) */
	else if (buf[0] == 'R')
	{
		int n1;

		/* Set to the first field */
		s=buf+2;

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_flags4[n1]; n1++)
		{
			if (streq(s, r_info_flags4[n1]))
			{
				d_ptr->r_flag = n1 + 1;
				break;
			}
		}

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_flags2[n1]; n1++)
		{
			if (streq(s, r_info_flags2[n1]))
			{
				d_ptr->r_flag = n1 + 33;
				break;
			}
		}

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_flags3[n1]; n1++)
		{
			if (streq(s, r_info_flags3[n1]))
			{
				d_ptr->r_flag = n1 + 65;
				break;
			}
		}

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_flags4[n1]; n1++)
		{
			if (streq(s, r_info_flags4[n1]))
			{
				d_ptr->r_flag = n1 + 97;
				break;
			}
		}
		
		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_s_flags1[n1]; n1++)
		{
			if (streq(s, r_info_s_flags1[n1]))
			{
				d_ptr->r_s_flag = n1 + 1;
				break;
			}
		}

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_s_flags2[n1]; n1++)
		{
			if (streq(s, r_info_s_flags2[n1]))
			{
				d_ptr->r_s_flag = n1 + 33;
				break;
			}
		}

		/* Analyze the race flag */
		if (!d_ptr->r_flag) for (n1 = 0; r_info_s_flags3[n1]; n1++)
		{
			if (streq(s, r_info_s_flags3[n1]))
			{
				d_ptr->r_s_flag = n1 + 65;
				break;
			}
		}

		/* Oops */
		if (!d_ptr->r_flag) 
		{
			message_format(MSG_GENERIC, 0, "Unknown room race flag '%s'.", s);

			/* Fail */
			return(PARSE_ERROR_GENERIC);
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in an object_kind from a textual string
 */
static errr grab_one_kind_flag(object_kind *k_ptr, cptr what)
{
	if (grab_one_flag(&k_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&k_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&k_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown object flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Grab one activation from a textual string
 */
static errr grab_one_activation(u16b *act, cptr what)
{
	int i;

	/* Scan activations */
	for (i = 1; i < POW_MAX; i++)
	{
		if (streq(what, k_info_act[i]))
		{
			*act = i;
			return (0);
		}
	}

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown activation '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "k_info" array, by parsing an ascii "template" file
 */
errr parse_k_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static object_kind *k_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		k_ptr = (object_kind*)head->info_ptr + i;

		/* Store the name */
		if (!(k_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		int tmp;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->d_attr = tmp;
		k_ptr->d_char = sym;
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pval;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			            &tval, &sval, &pval)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->tval = tval;
		k_ptr->sval = sval;
		k_ptr->pval = pval;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, extra, wgt;
		long cost;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			            &level, &extra, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->level = level;
		k_ptr->weight = wgt;
		k_ptr->cost = cost;
	}

	/* Process 'X' for "Even more Info" (one line only) */
	else if (buf[0] == 'X')
	{
		int material, qd, qs, breakage;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%dd%d:%d",
						&material, &qd, &qs, &breakage)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->material = material;
		k_ptr->qd = qd;
		k_ptr->qs = qs;
		k_ptr->breakage = breakage;
	}

	/* Process 'A' for "Allocation" (one line only) */
	else if (buf[0] == 'A')
	{
		int i;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* XXX XXX Simply read each number following a colon */
		for (i = 0, s = buf+1; s && (s[0] == ':') && s[1]; ++i)
		{
			if (i >= MAX_OBJ_ALLOC) return (PARSE_ERROR_TOO_MANY_ARGUMENTS);

			/* Default chance */
			k_ptr->chance[i] = 1;

			/* Store the locale */
			k_ptr->locale[i] = atoi(s + 1);

			/* Find the slash */
			t = strchr(s+1, '/');

			/* Find the next colon */
			s = strchr(s+1, ':');

			/* If the slash is "nearby", use it */
			if (t && (!s || t < s))
			{
				int chance = atoi(t+1);
				if (chance > 0) k_ptr->chance[i] = chance;
			}
		}
	}

	/* Process 'U' for "Activation" */
	else if (buf[0] == 'U')
	{
		u16b act = 0;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Get the activation */
			grab_one_activation(&act, s);

			k_ptr->activation = act;

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, ta;

		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (5 != sscanf(buf+2, "%d:%dd%d:%d:%d",
			            &ac, &hd1, &hd2, &th, &ta)) return (PARSE_ERROR_GENERIC);

		k_ptr->ac = ac;
		k_ptr->dd = hd1;
		k_ptr->ds = hd2;
		k_ptr->to_h = th;
		k_ptr->to_a =  ta;
	}

	/* Process 'K' for "Kill bonuses" (one line only) */
	else if (buf[0] == 'K')
	{
		int evl, myt, sku, tho, chs, anl, plt, udd, dmn, hmn, ppl, fry;
		int drg, lyc, acd, elc, fir, cld, psn, lit, drk;

		/* There better be a current pr_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (21 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &evl, &myt, &sku, &tho, &chs, &anl, &plt, &udd, &dmn,
						&hmn, &ppl, &fry, &drg, &lyc, &acd, 
						&elc, &fir, &cld, &psn, &lit, &drk)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->slays[SL_ANTI_EVIL] = evl;
		k_ptr->slays[SL_ANTI_AETHER] = myt;
		k_ptr->slays[SL_ANTI_SKULTGARD] = sku;
		k_ptr->slays[SL_ANTI_THORNWILD] = tho;
		k_ptr->slays[SL_ANTI_CHAOS] = chs;
		k_ptr->slays[SL_ANTI_ANIMAL] = anl;
		k_ptr->slays[SL_ANTI_PLANT] = plt;
		k_ptr->slays[SL_ANTI_UNDEAD] = udd;
		k_ptr->slays[SL_ANTI_DEMON] = dmn;
		k_ptr->slays[SL_ANTI_HUMANOID] = hmn;
		k_ptr->slays[SL_ANTI_PERSON] = ppl;
		k_ptr->slays[SL_ANTI_FAERY] = fry;
		k_ptr->slays[SL_ANTI_DRAGON] = drg;
		k_ptr->slays[SL_ANTI_LYCANTHROPE] = lyc;
		k_ptr->slays[SL_BRAND_ACID] = acd;
		k_ptr->slays[SL_BRAND_ELEC] = elc;
		k_ptr->slays[SL_BRAND_FIRE] = fir;
		k_ptr->slays[SL_BRAND_COLD] = cld;
		k_ptr->slays[SL_BRAND_POIS] = psn;
		k_ptr->slays[SL_BRAND_LITE] = lit;
		k_ptr->slays[SL_BRAND_DARK] = drk;
	}
	
	/* Process 'O' for "Oppositions" (one line only) */
	else if (buf[0] == 'O')
	{
		int acd, elc, fir, cld, wtr, psn, dis, lit, drk;
		int snd, shr, nex, nth, chs, dsn, tim, mna;

		/* There better be a current pr_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (17 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &acd, &elc, &fir, &cld, &wtr, &psn,
						&dis, &lit, &drk, &snd, &shr, &nex, 
						&nth, &chs, &dsn, &tim, &mna)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		k_ptr->res[RS_ACD] = acd;
		k_ptr->res[RS_ELC] = elc;
		k_ptr->res[RS_FIR] = fir;
		k_ptr->res[RS_CLD] = cld;
		k_ptr->res[RS_WTR] = wtr;
		k_ptr->res[RS_PSN] = psn;
		k_ptr->res[RS_DIS] = dis;
		k_ptr->res[RS_LIT] = lit;
		k_ptr->res[RS_DRK] = drk;
		k_ptr->res[RS_SND] = snd;
		k_ptr->res[RS_SHR] = shr;
		k_ptr->res[RS_NEX] = nex;
		k_ptr->res[RS_NTH] = nth;
		k_ptr->res[RS_CHS] = chs;
		k_ptr->res[RS_DSN] = dsn;
		k_ptr->res[RS_TIM] = tim;
		k_ptr->res[RS_MNA] = mna;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current k_ptr */
		if (!k_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_kind_flag(k_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in an artifact_type from a textual string
 */
static errr grab_one_artifact_flag(artifact_type *a_ptr, cptr what)
{
	if (grab_one_flag(&a_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&a_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&a_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown artifact flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "a_info" array, by parsing an ascii "template" file
 */
errr parse_a_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static artifact_type *a_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		a_ptr = (artifact_type*)head->info_ptr + i;

		/* Store the name */
		if (!(a_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Ignore everything */
		a_ptr->flags3 |= (TR3_IGNORE_ELEM);
		a_ptr->flags3 |= (TR3_IGNORE_NON_ELEM);
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int tval, sval, pfix, pval;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &tval, &sval, &pfix, &pval)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->tval = tval;
		a_ptr->sval = sval;
		a_ptr->pval = pval;

		a_ptr->prefix_idx = pfix;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, wgt;
		long cost;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			            &level, &rarity, &wgt, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->level = level;
		a_ptr->rarity = rarity;
		a_ptr->weight = wgt;
		a_ptr->cost = cost;
	}

	/* Process 'P' for "power" and such */
	else if (buf[0] == 'P')
	{
		int ac, hd1, hd2, th, ta;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (5 != sscanf(buf+2, "%d:%dd%d:%d:%d:",
			            &ac, &hd1, &hd2, &th, &ta)) return (PARSE_ERROR_GENERIC);

		a_ptr->ac = ac;
		a_ptr->dd = hd1;
		a_ptr->ds = hd2;
		a_ptr->to_h = th;
		a_ptr->to_a = ta;
	}

	/* Process 'K' for "Kill bonuses" (one line only) */
	else if (buf[0] == 'K')
	{
		int evl, myt, sku, tho, chs, anl, plt, udd, dmn, hmn, ppl, fry;
		int drg, lyc, acd, elc, fir, cld, psn, lit, drk;

		/* There better be a current pr_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (21 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &evl, &myt, &sku, &tho, &chs, &anl, &plt, &udd, &dmn,
						&hmn, &ppl, &fry, &drg, &lyc, &acd, 
						&elc, &fir, &cld, &psn, &lit, &drk)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->slays[SL_ANTI_EVIL] = evl;
		a_ptr->slays[SL_ANTI_AETHER] = myt;
		a_ptr->slays[SL_ANTI_SKULTGARD] = sku;
		a_ptr->slays[SL_ANTI_THORNWILD] = tho;
		a_ptr->slays[SL_ANTI_CHAOS] = chs;
		a_ptr->slays[SL_ANTI_ANIMAL] = anl;
		a_ptr->slays[SL_ANTI_PLANT] = plt;
		a_ptr->slays[SL_ANTI_UNDEAD] = udd;
		a_ptr->slays[SL_ANTI_DEMON] = dmn;
		a_ptr->slays[SL_ANTI_HUMANOID] = hmn;
		a_ptr->slays[SL_ANTI_PERSON] = ppl;
		a_ptr->slays[SL_ANTI_FAERY] = fry;
		a_ptr->slays[SL_ANTI_DRAGON] = drg;
		a_ptr->slays[SL_ANTI_LYCANTHROPE] = lyc;
		a_ptr->slays[SL_BRAND_ACID] = acd;
		a_ptr->slays[SL_BRAND_ELEC] = elc;
		a_ptr->slays[SL_BRAND_FIRE] = fir;
		a_ptr->slays[SL_BRAND_COLD] = cld;
		a_ptr->slays[SL_BRAND_POIS] = psn;
		a_ptr->slays[SL_BRAND_LITE] = lit;
		a_ptr->slays[SL_BRAND_DARK] = drk;
	}

	/* Process 'O' for "Oppositions" (one line only) */
	else if (buf[0] == 'O')
	{
		int acd, elc, fir, cld, wtr, psn, dis, lit, drk;
		int snd, shr, nex, nth, chs, dsn, tim, mna;

		/* There better be a current pr_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (17 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &acd, &elc, &fir, &cld, &wtr, &psn,
						&dis, &lit, &drk, &snd, &shr, &nex, 
						&nth, &chs, &dsn, &tim, &mna)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->res[RS_ACD] = acd;
		a_ptr->res[RS_ELC] = elc;
		a_ptr->res[RS_FIR] = fir;
		a_ptr->res[RS_CLD] = cld;
		a_ptr->res[RS_WTR] = wtr;
		a_ptr->res[RS_PSN] = psn;
		a_ptr->res[RS_DIS] = dis;
		a_ptr->res[RS_LIT] = lit;
		a_ptr->res[RS_DRK] = drk;
		a_ptr->res[RS_SND] = snd;
		a_ptr->res[RS_SHR] = shr;
		a_ptr->res[RS_NEX] = nex;
		a_ptr->res[RS_NTH] = nth;
		a_ptr->res[RS_CHS] = chs;
		a_ptr->res[RS_DSN] = dsn;
		a_ptr->res[RS_TIM] = tim;
		a_ptr->res[RS_MNA] = mna;
	}

	/* Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_artifact_flag(a_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	
	/* Process 'U' for "Activation & time" */
	else if (buf[0] == 'U')
	{
		int time, rand;
		u16b act = 0;

		/* There better be a current a_ptr */
		if (!a_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the colon before the name */
		s = strchr(buf + 2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the activation */
		grab_one_activation(&act, buf + 2);

		/* Scan for the values */
		if (2 != sscanf(s, "%d:%d",
		                &time, &rand)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		a_ptr->activation = act;
		a_ptr->time = time;
		a_ptr->randtime = rand;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a ego-item_type from a textual string
 */
static bool grab_one_ego_item_flag(ego_item_type *e_ptr, cptr what)
{
	if (grab_one_flag(&e_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&e_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&e_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown ego-item flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "e_info" array, by parsing an ascii "template" file
 */
errr parse_e_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static ego_item_type *e_ptr = NULL;

	static int cur_t = 0;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		e_ptr = (ego_item_type*)head->info_ptr + i;

		/* Store the name */
		if (!(e_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Start with the first of the tval indices */
		cur_t = 0;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int level, rarity, pad2;
		long cost;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%ld",
			            &level, &rarity, &pad2, &cost)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->level = level;
		e_ptr->rarity = rarity;
		/* e_ptr->weight = wgt; */
		e_ptr->cost = cost;
	}

	/* Process 'X' for "Xtra" (one line only) */
	else if (buf[0] == 'X')
	{
		int rate, xtra;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
			            &rate, &xtra)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->rating = rate;
		e_ptr->xtra = xtra;
	}

	/* Process 'T' for "Types allowed" (up to three lines) */
	else if (buf[0] == 'T')
	{
		int tval, sval1, sval2;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			            &tval, &sval1, &sval2)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->tval[cur_t] = (byte)tval;
		e_ptr->min_sval[cur_t] = (byte)sval1;
		e_ptr->max_sval[cur_t] = (byte)sval2;

		/* increase counter for 'possible tval' index */
		cur_t++;

		/* only three T: lines allowed */
		if (cur_t > EGO_TVALS_MAX) return (PARSE_ERROR_GENERIC);
	}

	/* Process 'M' (one line only) */
	else if (buf[0] == 'M')
	{
		int bas, min_w, max_w;

		/* There better be a current wpx_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			            &bas, &min_w, &max_w)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->base_material = bas;
		e_ptr->min_weight = min_w;
		e_ptr->max_weight = max_w;
	}

	/* Hack -- Process 'C' for "creation" */
	else if (buf[0] == 'C')
	{
		int th, perc, ta, pv;

		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &th, &perc, &ta, &pv)) return (PARSE_ERROR_GENERIC);

		e_ptr->max_to_h = th;
		e_ptr->d_perc = perc;
		e_ptr->max_to_a = ta;
		e_ptr->max_pval = pv;
	}

	/* Process 'K' for "Kill bonuses" (one line only) */
	else if (buf[0] == 'K')
	{
		int evl, myt, sku, tho, chs, anl, plt, udd, dmn, hmn, ppl, fry;
		int drg, lyc, acd, elc, fir, cld, psn, lit, drk;

		/* There better be a current pr_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (21 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &evl, &myt, &sku, &tho, &chs, &anl, &plt, &udd, &dmn,
						&hmn, &ppl, &fry, &drg, &lyc, &acd, 
						&elc, &fir, &cld, &psn, &lit, &drk)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->slays[SL_ANTI_EVIL] = evl;
		e_ptr->slays[SL_ANTI_AETHER] = myt;
		e_ptr->slays[SL_ANTI_SKULTGARD] = sku;
		e_ptr->slays[SL_ANTI_THORNWILD] = tho;
		e_ptr->slays[SL_ANTI_CHAOS] = chs;
		e_ptr->slays[SL_ANTI_ANIMAL] = anl;
		e_ptr->slays[SL_ANTI_PLANT] = plt;
		e_ptr->slays[SL_ANTI_UNDEAD] = udd;
		e_ptr->slays[SL_ANTI_DEMON] = dmn;
		e_ptr->slays[SL_ANTI_HUMANOID] = hmn;
		e_ptr->slays[SL_ANTI_PERSON] = ppl;
		e_ptr->slays[SL_ANTI_FAERY] = fry;
		e_ptr->slays[SL_ANTI_DRAGON] = drg;
		e_ptr->slays[SL_ANTI_LYCANTHROPE] = lyc;
		e_ptr->slays[SL_BRAND_ACID] = acd;
		e_ptr->slays[SL_BRAND_ELEC] = elc;
		e_ptr->slays[SL_BRAND_FIRE] = fir;
		e_ptr->slays[SL_BRAND_COLD] = cld;
		e_ptr->slays[SL_BRAND_POIS] = psn;
		e_ptr->slays[SL_BRAND_LITE] = lit;
		e_ptr->slays[SL_BRAND_DARK] = drk;
	}
	
	/* Process 'O' for "Oppositions" (one line only) */
	else if (buf[0] == 'O')
	{
		int acd, elc, fir, cld, wtr, psn, dis, lit, drk;
		int snd, shr, nex, nth, chs, dsn, tim, mna;

		/* There better be a current pr_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (17 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &acd, &elc, &fir, &cld, &wtr, &psn,
						&dis, &lit, &drk, &snd, &shr, &nex, 
						&nth, &chs, &dsn, &tim, &mna)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		e_ptr->res[RS_ACD] = acd;
		e_ptr->res[RS_ELC] = elc;
		e_ptr->res[RS_FIR] = fir;
		e_ptr->res[RS_CLD] = cld;
		e_ptr->res[RS_WTR] = wtr;
		e_ptr->res[RS_PSN] = psn;
		e_ptr->res[RS_DIS] = dis;
		e_ptr->res[RS_LIT] = lit;
		e_ptr->res[RS_DRK] = drk;
		e_ptr->res[RS_SND] = snd;
		e_ptr->res[RS_SHR] = shr;
		e_ptr->res[RS_NEX] = nex;
		e_ptr->res[RS_NTH] = nth;
		e_ptr->res[RS_CHS] = chs;
		e_ptr->res[RS_DSN] = dsn;
		e_ptr->res[RS_TIM] = tim;
		e_ptr->res[RS_MNA] = mna;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current e_ptr */
		if (!e_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_ego_item_flag(e_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a player class from a textual string
 */
static errr grab_one_w_prefix_flag(weapon_prefix_type *wpx_ptr, cptr what)
{
	if (grab_one_short_flag(&wpx_ptr->flags, px_info_flags, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown prefix flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "wpx_info" array, by parsing an ascii "template" file
 */
errr parse_wpx_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	static int cur_t = 0;

	/* Current entry */
	static weapon_prefix_type *wpx_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		wpx_ptr = (weapon_prefix_type*)head->info_ptr + i;

		/* Store the name */
		if (!(wpx_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Start with the first of the tval indices */
		cur_t = 0;
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		int tmp;
		byte clr;

		/* There better be a current d_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%c", &clr)) return (PARSE_ERROR_GENERIC);

		/* Extract the attr */
		tmp = color_char_to_attr(clr);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		wpx_ptr->d_attr = tmp;
	}

	/* Process 'W' (one line only) */
	else if (buf[0] == 'W')
	{
		int dep, rar, cst, wgt;

		/* There better be a current wpx_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &dep, &rar, &wgt, &cst)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		wpx_ptr->depth = dep;
		wpx_ptr->rarity = rar;
		wpx_ptr->cost = cst;
		wpx_ptr->weight = wgt;
	}

	/* Process 'M' (one line only) */
	else if (buf[0] == 'M')
	{
		int bas, act;

		/* There better be a current wpx_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (2 != sscanf(buf+2, "%d:%d",
			            &bas, &act)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		wpx_ptr->base_material = bas;
		wpx_ptr->actual_material = act;
	}

	/* Process 'P' (one line only) */
	else if (buf[0] == 'P')
	{
		int th, prc;

		/* There better be a current wpx_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (2 != sscanf(buf+2, "%d:%d",
			            &th, &prc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		wpx_ptr->to_h = th;
		wpx_ptr->d_perc = prc;
	}

	/* Process 'K' for "Kill bonuses" (one line only) */
	else if (buf[0] == 'K')
	{
		int evl, myt, sku, tho, chs, anl, plt, udd, dmn, hmn, ppl, fry;
		int drg, lyc, acd, elc, fir, cld, psn, lit, drk;

		/* There better be a current pr_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (21 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &evl, &myt, &sku, &tho, &chs, &anl, &plt, &udd, &dmn,
						&hmn, &ppl, &fry, &drg, &lyc, &acd, 
						&elc, &fir, &cld, &psn, &lit, &drk)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		wpx_ptr->slays[SL_ANTI_EVIL] = evl;
		wpx_ptr->slays[SL_ANTI_AETHER] = myt;
		wpx_ptr->slays[SL_ANTI_SKULTGARD] = sku;
		wpx_ptr->slays[SL_ANTI_THORNWILD] = tho;
		wpx_ptr->slays[SL_ANTI_CHAOS] = chs;
		wpx_ptr->slays[SL_ANTI_ANIMAL] = anl;
		wpx_ptr->slays[SL_ANTI_PLANT] = plt;
		wpx_ptr->slays[SL_ANTI_UNDEAD] = udd;
		wpx_ptr->slays[SL_ANTI_DEMON] = dmn;
		wpx_ptr->slays[SL_ANTI_HUMANOID] = hmn;
		wpx_ptr->slays[SL_ANTI_PERSON] = ppl;
		wpx_ptr->slays[SL_ANTI_FAERY] = fry;
		wpx_ptr->slays[SL_ANTI_DRAGON] = drg;
		wpx_ptr->slays[SL_ANTI_LYCANTHROPE] = lyc;
		wpx_ptr->slays[SL_BRAND_ACID] = acd;
		wpx_ptr->slays[SL_BRAND_ELEC] = elc;
		wpx_ptr->slays[SL_BRAND_FIRE] = fir;
		wpx_ptr->slays[SL_BRAND_COLD] = cld;
		wpx_ptr->slays[SL_BRAND_POIS] = psn;
		wpx_ptr->slays[SL_BRAND_LITE] = lit;
		wpx_ptr->slays[SL_BRAND_DARK] = drk;
	}

	/* Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current pc_ptr */
		if (!wpx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_w_prefix_flag(wpx_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a player class from a textual string
 */
static errr grab_one_a_prefix_flag(armor_prefix_type *apx_ptr, cptr what)
{
	if (grab_one_short_flag(&apx_ptr->flags, px_info_flags, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown prefix flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "apx_info" array, by parsing an ascii "template" file
 */
errr parse_apx_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	static int cur_t = 0;

	/* Current entry */
	static armor_prefix_type *apx_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		apx_ptr = (armor_prefix_type*)head->info_ptr + i;

		/* Store the name */
		if (!(apx_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Start with the first of the tval indices */
		cur_t = 0;
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		int tmp;
		byte clr;

		/* There better be a current d_ptr */
		if (!apx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (1 != sscanf(buf+2, "%c", &clr)) return (PARSE_ERROR_GENERIC);

		/* Extract the attr */
		tmp = color_char_to_attr(clr);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		apx_ptr->d_attr = tmp;
	}

	/* Process 'W' (one line only) */
	else if (buf[0] == 'W')
	{
		int dep, rar, cst, wgt;

		/* There better be a current wpx_ptr */
		if (!apx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &dep, &rar, &wgt, &cst)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		apx_ptr->depth = dep;
		apx_ptr->rarity = rar;
		apx_ptr->cost = cst;
		apx_ptr->weight = wgt;
	}

	/* Process 'M' (one line only) */
	else if (buf[0] == 'M')
	{
		int bas, act;

		/* There better be a current wpx_ptr */
		if (!apx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (2 != sscanf(buf+2, "%d:%d",
			            &bas, &act)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		apx_ptr->base_material = bas;
		apx_ptr->actual_material = act;
	}

	/* Process 'P' (one line only) */
	else if (buf[0] == 'P')
	{
		int ac;

		/* There better be a current wpx_ptr */
		if (!apx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (1 != sscanf(buf+2, "%d",
			            &ac)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		apx_ptr->ac = ac;
	}

	/* Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current pc_ptr */
		if (!apx_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_a_prefix_flag(apx_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a trap widget from a textual string
 */
static errr grab_one_widget_flag(trap_widget *w_ptr, cptr what)
{
	if (grab_one_flag(&w_ptr->flags, w_info_flags, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown trap_widget flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "w_info" array, by parsing an ascii "template" file
 */
errr parse_w_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	static int cur_t = 0;

	/* Current entry */
	static trap_widget *w_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		w_ptr = (trap_widget*)head->info_ptr + i;;

		/* Store the name */
		if (!(w_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* Start with the first of the tval indices */
		cur_t = 0;
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		int tmp;

		/* There better be a current r_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		w_ptr->t_attr = tmp;
		w_ptr->t_char = sym;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int lev, rar;

		/* There better be a current r_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
			            &lev, &rar)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		w_ptr->level = lev;
		w_ptr->rarity = rar;
	}

	/* Process 'X' for "More Info" (one line only) */
	else if (buf[0] == 'X')
	{
		int spot, dis, chr;

		/* There better be a current r_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			            &spot, &dis, &chr)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		w_ptr->spot_factor = spot;
		w_ptr->disarm_factor = dis;
		w_ptr->max_charges = chr;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current k_ptr */
		if (!w_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_widget_flag(w_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one (basic) flag in a monster_race from a textual string
 */
static errr grab_one_race_basic_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->flags1, r_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags2, r_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags3, r_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->flags4, r_info_flags4, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Grab one (spell) flag in a monster_race from a textual string
 */
static errr grab_one_race_spell_flag(monster_race *r_ptr, cptr what)
{
	if (grab_one_flag(&r_ptr->s_flags1, r_info_s_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->s_flags2, r_info_s_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&r_ptr->s_flags3, r_info_s_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "r_info" array, by parsing an ascii "template" file
 */
errr parse_r_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static monster_race *r_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		r_ptr = (monster_race*)head->info_ptr + i;

		/* Store the name */
		if (!(r_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&r_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'G' for "Graphics" (one line only) */
	else if (buf[0] == 'G')
	{
		char sym;
		int tmp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Paranoia */
		if (!buf[2]) return (PARSE_ERROR_GENERIC);
		if (!buf[3]) return (PARSE_ERROR_GENERIC);
		if (!buf[4]) return (PARSE_ERROR_GENERIC);

		/* Extract the char */
		sym = buf[2];

		/* Extract the attr */
		tmp = color_char_to_attr(buf[4]);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->d_attr = tmp;
		r_ptr->d_char = sym;
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int spd, lif, aaf, ac, slp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (5 != sscanf(buf+2, "%d:%d:%d:%d:%d",
			            &spd, &lif, &aaf, &ac, &slp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->speed = spd;
		r_ptr->life = lif;
		r_ptr->aaf = aaf;
		r_ptr->ac = ac;
		r_ptr->sleep = slp;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int lev, rar;
		long exp;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%ld",
			            &lev, &rar, &exp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		r_ptr->level = lev;
		r_ptr->rarity = rar;
		r_ptr->mexp = exp;
	}
	
	/* Process 'B' for "Blows" */
	else if (buf[0] == 'B')
	{
		int n1, n2;

		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < MONSTER_BLOW_MAX; i++) if (!r_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == MONSTER_BLOW_MAX) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; n1 < RBM_MAX; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);

		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; n2 < RBE_MAX; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		r_ptr->blow[i].method = n1;

		/* Save the effect */
		r_ptr->blow[i].effect = n2;

		/* Extract the damage dice and sides */
		r_ptr->blow[i].d_dice = atoi(s);
		r_ptr->blow[i].d_side = atoi(t);
	}

	/* Process 'F' for "Basic Flags" (multiple lines) */
	else if (buf[0] == 'F')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_race_basic_flag(r_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'S' for "Spell Flags" (multiple lines) */
	else if (buf[0] == 'S')
	{
		/* There better be a current r_ptr */
		if (!r_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* XXX Hack -- Read spell frequency */
			if (1 == sscanf(s, "1_IN_%d", &i))
			{
				/* Sanity check */
				if ((i < 1) || (i > 100))
					return (PARSE_ERROR_INVALID_SPELL_FREQ);

				/* Extract a "frequency" */
				r_ptr->freq_spell = 100 / i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_race_spell_flag(r_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one (basic) flag in a monster_unique from a textual string
 */
static errr grab_one_unique_basic_flag(monster_unique *u_ptr, cptr what)
{
	if (grab_one_flag(&u_ptr->flags1, r_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&u_ptr->flags2, r_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&u_ptr->flags3, r_info_flags3, what) == 0)
		return (0);

	if (grab_one_flag(&u_ptr->flags4, r_info_flags4, what) == 0)
		return (0);
	
	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Grab one (spell) flag in a monster_unique from a textual string
 */
static errr grab_one_unique_spell_flag(monster_unique *u_ptr, cptr what)
{
	if (grab_one_flag(&u_ptr->s_flags1, r_info_s_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&u_ptr->s_flags2, r_info_s_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&u_ptr->s_flags3, r_info_s_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown monster flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "u_info" array, by parsing an ascii "template" file
 */
errr parse_u_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static monster_unique *u_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		u_ptr = (monster_unique*)head->info_ptr + i;

		/* Store the name */
		if (!(u_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'R' for "Monster Race" (one line only) */
	else if (buf[0] == 'R')
	{
		int r_idx, tmp;
		byte clr;

		/* There better be a current u_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%c", &r_idx, &clr)) return (PARSE_ERROR_GENERIC);

		/* Extract the attr */
		tmp = color_char_to_attr(clr);

		/* Paranoia */
		if (tmp < 0) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		u_ptr->r_idx = r_idx;
		u_ptr->d_attr = tmp;
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current u_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&u_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'I' for "Info" (one line only) */
	else if (buf[0] == 'I')
	{
		int spd, lif, aaf, ac, slp;

		/* There better be a current u_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the other values */
		if (5 != sscanf(buf+2, "%d:%d:%d:%d:%d",
		                &spd, &lif, &aaf, &ac, &slp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		u_ptr->speed = spd;
		u_ptr->life = lif;
		u_ptr->aaf = aaf;
		u_ptr->ac = ac;
		u_ptr->sleep = slp;
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int lev, rar;
		long exp;

		/* There better be a current u_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%ld",
		                &lev, &rar, &exp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		u_ptr->level = lev;
		u_ptr->rarity = rar;
		u_ptr->mexp = exp;
	}

	/* Process 'B' for "Blows" (up to four lines) */
	else if (buf[0] == 'B')
	{
		int n1, n2;

		/* There better be a current u_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Find the next empty blow slot (if any) */
		for (i = 0; i < 4; i++) if (!u_ptr->blow[i].method) break;

		/* Oops, no more slots */
		if (i == 4) return (PARSE_ERROR_GENERIC);

		/* Analyze the first field */
		for (s = t = buf+2; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze the method */
		for (n1 = 0; n1 < RBM_MAX; n1++)
		{
			if (streq(s, r_info_blow_method[n1])) break;
		}

		/* Invalid method */
		if (!r_info_blow_method[n1]) return (PARSE_ERROR_GENERIC);

		/* Analyze the second field */
		for (s = t; *t && (*t != ':'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == ':') *t++ = '\0';

		/* Analyze effect */
		for (n2 = 0; n2 < RBE_MAX; n2++)
		{
			if (streq(s, r_info_blow_effect[n2])) break;
		}

		/* Invalid effect */
		if (!r_info_blow_effect[n2]) return (PARSE_ERROR_GENERIC);

		/* Analyze the third field */
		for (s = t; *t && (*t != 'd'); t++) /* loop */;

		/* Terminate the field (if necessary) */
		if (*t == 'd') *t++ = '\0';

		/* Save the method */
		u_ptr->blow[i].method = n1;

		/* Save the effect */
		u_ptr->blow[i].effect = n2;

		/* Extract the damage dice and sides */
		u_ptr->blow[i].d_dice = atoi(s);
		u_ptr->blow[i].d_side = atoi(t);
	}

	/* Process 'F' for "Basic Flags" (multiple lines) */
	else if (buf[0] == 'F')
	{
		/* There better be a current r_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;
			
			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_unique_basic_flag(u_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Process 'S' for "Spell Flags" (multiple lines) */
	else if (buf[0] == 'S')
	{
		/* There better be a current r_ptr */
		if (!u_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* XXX XXX XXX Hack -- Read spell frequency */
			if (1 == sscanf(s, "1_IN_%d", &i))
			{
				/* Extract a "frequency" */
				u_ptr->freq_spell = 100 / i;

				/* Start at next entry */
				s = t;

				/* Continue */
				continue;
			}

			/* Parse this entry */
			if (0 != grab_one_unique_spell_flag(u_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one (basic) flag in a monster_special from a textual string
 */
static errr grab_one_special_flag(u32b *flag, cptr what)
{
	if (grab_one_flag(flag, r_info_flags4, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Illegal negative flag '%s'.", what);

	/* Failure */
	return (PARSE_ERROR_GENERIC);
}
/*
 * Initialize the "s_info" array, by parsing an ascii "template" file
 */
errr parse_s_info(char *buf, header *head)
{
	int i;

	char *s, *t;

	/* Current entry */
	static monster_special *s_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		s_ptr = (monster_special*)head->info_ptr + i;

		/* Store the name */
		if (!(s_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'W' for "More Info" (one line only) */
	else if (buf[0] == 'W')
	{
		int rar;
		int lev;
		int exp;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
		                &lev, &rar, &exp)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		s_ptr->level = lev;
		s_ptr->rarity = rar;
		s_ptr->exp_perc = exp;
	}

	/* Process 'I' for ego type effects (one line only)*/
	else if (buf[0] == 'I')
	{
		int spd, hp_prc;

		/* There better be a current s_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d",
		                &spd, &hp_prc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		s_ptr->speed_mod = spd;
		s_ptr->hp_perc = hp_prc;
	}

	/* Process 'X' for "Negative Flags" (multiple lines) */
	else if (buf[0] == 'X')
	{
		/* There better be a current r_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;
			
			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_special_flag(&s_ptr->no_flag4, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	
	/* Process 'Y' for "Required Flags" (multiple lines) */
	else if (buf[0] == 'Y')
	{
		/* There better be a current r_ptr */
		if (!s_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;
			
			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while (*t == ' ' || *t == '|') t++;
			}

			/* Parse this entry */
			if (0 != grab_one_special_flag(&s_ptr->req_flag4, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a player_race from a textual string
 */
static errr grab_one_racial_flag(player_race *pr_ptr, cptr what)
{
	if (grab_one_flag(&pr_ptr->flags1, k_info_flags1, what) == 0)
		return (0);

	if (grab_one_flag(&pr_ptr->flags2, k_info_flags2, what) == 0)
		return (0);

	if (grab_one_flag(&pr_ptr->flags3, k_info_flags3, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown player flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "p_info" array, by parsing an ascii "template" file
 */
errr parse_p_info(char *buf, header *head)
{
	int i, j;

	char *s, *t;

	/* Current entry */
	static player_race *pr_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		pr_ptr = (player_race*)head->info_ptr + i;

		/* Store the name */
		if (!(pr_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'S' for "Stats" (one line only) */
	else if (buf[0] == 'S')
	{
		int adj;

		/* There better be a current pc_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Start the string */
		s = buf+1;

		/* For each stat */
		for (j = 0; j < A_MAX; j++)
		{
			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			pr_ptr->r_adj[j] = adj;

			/* Next... */
			continue;
		}
	}
	
	/* Process 'R' for "Racial Skills" (one line only) */
	else if (buf[0] == 'R')
	{
		int dis, dev, sav, stl, per, thn, thb, tht, mob, alc, map;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (SK_MAX != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &dis, &dev, &sav, &stl, &per,
						&thn, &thb, &tht, &mob, &alc, &map)) 
						return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pr_ptr->r_skill[SK_DIS] = dis;
		pr_ptr->r_skill[SK_DEV] = dev;
		pr_ptr->r_skill[SK_SAV] = sav;
		pr_ptr->r_skill[SK_STL] = stl;
		pr_ptr->r_skill[SK_PER] = per;
		pr_ptr->r_skill[SK_THN] = thn;
		pr_ptr->r_skill[SK_THB] = thb;
		pr_ptr->r_skill[SK_THT] = tht;
		pr_ptr->r_skill[SK_MOB] = mob;
		pr_ptr->r_skill[SK_ALC] = alc;
		pr_ptr->r_skill[SK_MAP] = map;
	}

	/* Process 'X' for "Extra Info" (one line only) */
	else if (buf[0] == 'X')
	{
		int mhp, exp, infra;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
			            &mhp, &exp, &infra)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pr_ptr->r_mhp = mhp;
		pr_ptr->r_exp = exp;
		pr_ptr->infra = infra;
	}

	/* Process 'O' for "Oppositions" (one line only) */
	else if (buf[0] == 'O')
	{
		int acd, elc, fir, cld, wtr, psn, dis, lit, drk;
		int snd, shr, nex, nth, chs, dsn, tim, mna;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (17 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &acd, &elc, &fir, &cld, &wtr, &psn,
						&dis, &lit, &drk, &snd, &shr, &nex, 
						&nth, &chs, &dsn, &tim, &mna)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pr_ptr->res[RS_ACD] = acd;
		pr_ptr->res[RS_ELC] = elc;
		pr_ptr->res[RS_FIR] = fir;
		pr_ptr->res[RS_CLD] = cld;
		pr_ptr->res[RS_WTR] = wtr;
		pr_ptr->res[RS_PSN] = psn;
		pr_ptr->res[RS_DIS] = dis;
		pr_ptr->res[RS_LIT] = lit;
		pr_ptr->res[RS_DRK] = drk;
		pr_ptr->res[RS_SND] = snd;
		pr_ptr->res[RS_SHR] = shr;
		pr_ptr->res[RS_NEX] = nex;
		pr_ptr->res[RS_NTH] = nth;
		pr_ptr->res[RS_CHS] = chs;
		pr_ptr->res[RS_DSN] = dsn;
		pr_ptr->res[RS_TIM] = tim;
		pr_ptr->res[RS_MNA] = mna;
	}

	/* Hack -- Process 'I' for "info" and such */
	else if (buf[0] == 'I')
	{
		int hist, prfx, b_age, m_age;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
		                &hist, &prfx, &b_age, &m_age)) return (PARSE_ERROR_GENERIC);

		pr_ptr->hist = hist;
		pr_ptr->prefix = prfx;
		pr_ptr->b_age = b_age;
		pr_ptr->m_age = m_age;
	}

	/* Hack -- Process 'H' for "Height" */
	else if (buf[0] == 'H')
	{
		int m_b_ht, m_m_ht, f_b_ht, f_m_ht;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &m_b_ht, &m_m_ht, &f_b_ht, &f_m_ht)) return (PARSE_ERROR_GENERIC);

		pr_ptr->m_b_ht = m_b_ht;
		pr_ptr->m_m_ht = m_m_ht;
		pr_ptr->f_b_ht = f_b_ht;
		pr_ptr->f_m_ht = f_m_ht;
	}

	/* Hack -- Process 'W' for "Weight" */
	else if (buf[0] == 'W')
	{
		int m_b_wt, m_m_wt, f_b_wt, f_m_wt;

		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
			            &m_b_wt, &m_m_wt, &f_b_wt, &f_m_wt)) return (PARSE_ERROR_GENERIC);

		pr_ptr->m_b_wt = m_b_wt;
		pr_ptr->m_m_wt = m_m_wt;
		pr_ptr->f_b_wt = f_b_wt;
		pr_ptr->f_m_wt = f_m_wt;
	}

	/* Hack -- Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_racial_flag(pr_ptr, s)) return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'U' for special abilties */
	else if (buf[0] == 'U')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (streq(s,"ANGEL")) pr_ptr->special = RACE_SPECIAL_ANGEL;
			else if (streq(s,"DEMON")) pr_ptr->special = RACE_SPECIAL_DEMON;
			else return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}

	/* Hack -- Process 'C' for class choices */
	else if (buf[0] == 'C')
	{
		/* There better be a current pr_ptr */
		if (!pr_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Hack - Parse this entry */
			pr_ptr->choice |= (1 << atoi(s));

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Grab one flag in a player class from a textual string
 */
static errr grab_one_class_flag(player_class *pc_ptr, cptr what)
{
	if (grab_one_flag(&pc_ptr->flags, c_info_flags, what) == 0)
		return (0);

	/* Oops */
	message_format(MSG_GENERIC, 0, "Unknown player class flag '%s'.", what);

	/* Error */
	return (PARSE_ERROR_GENERIC);
}

/*
 * Initialize the "c_info" array, by parsing an ascii "template" file
 */
errr parse_c_info(char *buf, header *head)
{
	int i, j;

	char *s, *t;

	/* Current entry */
	static player_class *pc_ptr = NULL;

	static int cur_title = 0;
	static int cur_equip = 0;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		pc_ptr = (player_class*)head->info_ptr + i;

		/* Store the name */
		if (!(pc_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);

		/* No titles and equipment yet */
		cur_title = 0;
		cur_equip = 0;
	}

	/* Process 'S' for "Stats" (one line only) */
	else if (buf[0] == 'S')
	{
		int adj;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Start the string */
		s = buf+1;


		/* For each stat */
		for (j = 0; j < A_MAX; j++)
		{
			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			pc_ptr->c_adj[j] = adj;

			/* Next... */
			continue;
		}
	}

	/* Process 'C' for "Class Skills" (one line only) */
	else if (buf[0] == 'C')
	{
		int dis, dev, sav, stl, per, thn, thb, tht, mob, alc, map;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (SK_MAX != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &dis, &dev, &sav, &stl, &per,
						&thn, &thb, &tht, &mob, &alc, &map)) 
						return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->c_skill[SK_DIS] = dis;
		pc_ptr->c_skill[SK_DEV] = dev;
		pc_ptr->c_skill[SK_SAV] = sav;
		pc_ptr->c_skill[SK_STL] = stl;
		pc_ptr->c_skill[SK_PER] = per;
		pc_ptr->c_skill[SK_THN] = thn;
		pc_ptr->c_skill[SK_THB] = thb;
		pc_ptr->c_skill[SK_THT] = tht;
		pc_ptr->c_skill[SK_MOB] = mob;
		pc_ptr->c_skill[SK_ALC] = alc;
		pc_ptr->c_skill[SK_MAP] = map;
	}

	/* Process 'X' for "Xtra Skills" (one line only) */
	else if (buf[0] == 'X')
	{
		int dis, dev, sav, stl, per, thn, thb, tht, mob, alc, map;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);
		
		/* Scan for the values */
		if (SK_MAX != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &dis, &dev, &sav, &stl, &per,
						&thn, &thb, &tht, &mob, &alc, &map)) 
						return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->x_skill[SK_DIS] = dis;
		pc_ptr->x_skill[SK_DEV] = dev;
		pc_ptr->x_skill[SK_SAV] = sav;
		pc_ptr->x_skill[SK_STL] = stl;
		pc_ptr->x_skill[SK_PER] = per;
		pc_ptr->x_skill[SK_THN] = thn;
		pc_ptr->x_skill[SK_THB] = thb;
		pc_ptr->x_skill[SK_THT] = tht;
		pc_ptr->x_skill[SK_MOB] = mob;
		pc_ptr->x_skill[SK_ALC] = alc;
		pc_ptr->x_skill[SK_MAP] = map;
	}

	/* Process 'O' for "Oppositions" (one line only) */
	else if (buf[0] == 'O')
	{
		int acd, elc, fir, cld, wtr, psn, dis, lit, drk;
		int snd, shr, nex, nth, chs, dsn, tim, mna;

		/* There better be a current pr_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (17 != sscanf(buf+2, "%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d",
		                &acd, &elc, &fir, &cld, &wtr, &psn,
						&dis, &lit, &drk, &snd, &shr, &nex, 
						&nth, &chs, &dsn, &tim, &mna)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->res[RS_ACD] = acd;
		pc_ptr->res[RS_ELC] = elc;
		pc_ptr->res[RS_FIR] = fir;
		pc_ptr->res[RS_CLD] = cld;
		pc_ptr->res[RS_WTR] = wtr;
		pc_ptr->res[RS_PSN] = psn;
		pc_ptr->res[RS_DIS] = dis;
		pc_ptr->res[RS_LIT] = lit;
		pc_ptr->res[RS_DRK] = drk;
		pc_ptr->res[RS_SND] = snd;
		pc_ptr->res[RS_SHR] = shr;
		pc_ptr->res[RS_NEX] = nex;
		pc_ptr->res[RS_NTH] = nth;
		pc_ptr->res[RS_CHS] = chs;
		pc_ptr->res[RS_DSN] = dsn;
		pc_ptr->res[RS_TIM] = tim;
		pc_ptr->res[RS_MNA] = mna;
	}

	/* Process 'I' for "info" - 1 line only */
	else if (buf[0] == 'I')
	{
		int mhp, exp, sstat1, sstat2, dist;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (5 != sscanf(buf+2, "%d:%d:%d:%d:%d",
		                &mhp, &exp, &sstat1, &sstat2, &dist))
						return (PARSE_ERROR_GENERIC);

		/* Save the values */
		pc_ptr->c_mhp = mhp;
		pc_ptr->c_exp = exp;
		pc_ptr->spell_stat1 = sstat1;
		pc_ptr->spell_stat2 = sstat2;
		pc_ptr->spell_weight = dist;
	}

	/* Hack -- Process 'B' for "Spellbooks" */
	else if (buf[0] == 'B')
	{
		int book_index, handicap;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);
	
		/* Scan for the values */
		if (2 != sscanf(buf+2, "%d:%d", &book_index, &handicap))
			return (PARSE_ERROR_GENERIC);

		if (book_index >= SV_BOOK_MAX) return (PARSE_ERROR_OUT_OF_BOUNDS);

		pc_ptr->spell_book[book_index] = TRUE;
		pc_ptr->spell_handicap[book_index] = handicap;

	}

	/* Process 'E' for "Starting Equipment" */
	else if (buf[0] == 'E')
	{
		int tval, sval, min, max;

		start_item *e_ptr;

		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Limit number of starting items */
		if (cur_equip == MAX_START_ITEMS) return (PARSE_ERROR_GENERIC);

		/* Access the item */
		e_ptr = &pc_ptr->start_items[cur_equip];

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",
		                &tval, &sval, &min, &max)) return (PARSE_ERROR_GENERIC);

		e_ptr->tval = tval;
		e_ptr->sval = sval;
		e_ptr->min = min;
		e_ptr->max = max;

		/* Next item */
		cur_equip++;
	}

	/* Process 'T' for "Titles" */
	else if (buf[0] == 'T')
	{
		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&pc_ptr->title[cur_title], head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
		
		/* Next title */
		cur_title++;

		/* Limit number of titles */
		if (cur_title > PY_MAX_LEVEL / 5)
			return (PARSE_ERROR_TOO_MANY_ARGUMENTS);
	}

	/* Process 'F' for flags */
	else if (buf[0] == 'F')
	{
		/* There better be a current pc_ptr */
		if (!pc_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Parse every entry textually */
		for (s = buf + 2; *s; )
		{
			/* Find the end of this entry */
			for (t = s; *t && (*t != ' ') && (*t != '|'); ++t) /* loop */;

			/* Nuke and skip any dividers */
			if (*t)
			{
				*t++ = '\0';
				while ((*t == ' ') || (*t == '|')) t++;
			}

			/* Parse this entry */
			if (0 != grab_one_class_flag(pc_ptr, s))
				return (PARSE_ERROR_INVALID_FLAG);

			/* Start the next entry */
			s = t;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "h_info" array, by parsing an ascii "template" file
 */
errr parse_h_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static hist_type *h_ptr = NULL;

	/* Process 'N' for "New/Number" */
	if (buf[0] == 'N')
	{
		int prv, nxt, prc, soc;

		/* Hack - get the index */
		i = error_idx + 1;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		h_ptr = (hist_type*)head->info_ptr + i;

		/* Scan for the values */
		if (4 != sscanf(buf, "N:%d:%d:%d:%d",
			            &prv, &nxt, &prc, &soc)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		h_ptr->chart = prv;
		h_ptr->next = nxt;
		h_ptr->roll = prc;
		h_ptr->bonus = soc;
	}

	/* Process 'D' for "Description" */
	else if (buf[0] == 'D')
	{
		/* There better be a current h_ptr */
		if (!h_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Get the text */
		s = buf+2;

		/* Store the text */
 		if (!add_text(&h_ptr->text, head, s))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "b_info" array, by parsing an ascii "template" file
 */
errr parse_b_info(char *buf, header *head)
{
	int i, j;

	char *s, *t;

	/* Current entry */
	static owner_type *ot_ptr = NULL;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the subindex */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the subindex */
		*s++ = '\0';

		/* Get the index */
		i = atoi(buf+2);

		/* Find the colon before the name */

		t = strchr(s, ':');

		/* Verify that colon */
		if (!t) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*t++ = '\0';

		/* Paranoia -- require a name */
		if (!*t) return (PARSE_ERROR_GENERIC);

		/* Get the subindex */
		j = atoi(s);

		/* Verify information */
		if (j >= z_info->b_max) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Get the *real* index */
		i = (i * z_info->b_max) + j;

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		ot_ptr = (owner_type*)head->info_ptr + i;

		/* Store the name */
		if (!(ot_ptr->owner_name = add_name(head, t)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}
		
	else /* Process 'I' for "Info" (one line only) */
	if (buf[0] == 'I')
	{
		int race, gld, inf;

		/* There better be a current ot_ptr */
		if (!ot_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (3 != sscanf(buf+2, "%d:%d:%d",
		                &race, &gld, &inf)) return (PARSE_ERROR_GENERIC);

		/* Save the values */
		ot_ptr->owner_race = race;
		ot_ptr->max_cost = gld;

		ot_ptr->inflate = inf;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "g_info" array, by parsing an ascii "template" file
 */
errr parse_g_info(char *buf, header *head)
{
	int i, j;

	char *s;

	/* Current entry */
	static byte *g_ptr;

	/* Process 'A' for "Adjustments" */
	if (buf[0] == 'A')
	{
		int adj;

		/* Start the string */
		s = buf+1;

		/* Initialize the counter to max races */
		j = z_info->p_max;

		/* Repeat */
		while (j-- > 0)
		{
			/* Hack - get the index */
			i = error_idx + 1;

			/* Verify information */
			if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

			/* Verify information */
			if (i >= head->info_num) return (PARSE_ERROR_TOO_MANY_ENTRIES);

			/* Save the index */
			error_idx = i;

			/* Point at the "info" */
			g_ptr = (byte*)head->info_ptr + i;

			/* Find the colon before the subindex */
			s = strchr(s, ':');

			/* Verify that colon */
			if (!s) return (PARSE_ERROR_GENERIC);

			/* Nuke the colon, advance to the subindex */
			*s++ = '\0';

			/* Get the value */
			adj = atoi(s);

			/* Save the value */
			*g_ptr = adj;
		}
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

/*
 * Initialize the "q_info" array, by parsing an ascii "template" file
 */
errr parse_q_info(char *buf, header *head)
{
	int i;

	char *s;

	/* Current entry */
	static quest_type *q_ptr = NULL;

	static int prev_lev = 0;

	/* Process 'N' for "New/Number/Name" */
	if (buf[0] == 'N')
	{
		/* Find the colon before the name */
		s = strchr(buf+2, ':');

		/* Verify that colon */
		if (!s) return (PARSE_ERROR_GENERIC);

		/* Nuke the colon, advance to the name */
		*s++ = '\0';

		/* Paranoia -- require a name */
		if (!*s) return (PARSE_ERROR_GENERIC);

		/* Get the index */
		i = atoi(buf+2);

		/* Verify information */
		if (i <= error_idx) return (PARSE_ERROR_NON_SEQUENTIAL_RECORDS);

		/* Verify information */
		if (i >= head->info_num) return (PARSE_ERROR_OBSOLETE_FILE);

		/* Save the index */
		error_idx = i;

		/* Point at the "info" */
		q_ptr = (quest_type*)head->info_ptr + i;

		/* Store the name */
		if (!(q_ptr->name = add_name(head, s)))
			return (PARSE_ERROR_OUT_OF_MEMORY);
	}

	/* Process 'W' for "Where/What" (one line only) */
	else if (buf[0] == 'W')
	{
		int lev, r_idx, u_idx, max;

		/* There better be a current q_ptr */
		if (!q_ptr) return (PARSE_ERROR_MISSING_RECORD_HEADER);

		/* Scan for the values */
		if (4 != sscanf(buf+2, "%d:%d:%d:%d",&lev, &r_idx, &u_idx, &max)) 
			return (PARSE_ERROR_GENERIC);

		/* Check quests */
		for (i = 0; i < error_idx; i++)
		{
			/* Check for quest */
			if (lev <= prev_lev) return (PARSE_ERROR_NON_SEQUENTIAL_QUESTS);
		}

		/* Save the values */
		prev_lev = q_ptr->base_level = q_ptr->active_level = lev;
		if (r_idx)
		{
			q_ptr->mon_idx = r_idx;
			q_ptr->type = QUEST_FIXED;
		}
		else 
		{
			q_ptr->mon_idx = u_idx;
			q_ptr->type = QUEST_FIXED_U;
		}

		q_ptr->max_num = max;
	}
	else
	{
		/* Oops */
		return (PARSE_ERROR_UNDEFINED_DIRECTIVE);
	}

	/* Success */
	return (0);
}

#else	/* ALLOW_TEMPLATES */

#ifdef MACINTOSH
static int i = 0;
#endif

#endif	/* ALLOW_TEMPLATES */
