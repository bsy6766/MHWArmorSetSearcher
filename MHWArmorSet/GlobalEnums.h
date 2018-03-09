#ifndef GLOBAL_ENUMS_H
#define GLOBAL_ENUMS_H

namespace MHW
{
	enum class ArmorType
	{
		HEAD = 0,
		CHEST,
		ARM,
		WAIST,
		LEG
	};

	enum class Gender
	{
		NONE = 0,
		FEMALE,
		MALE
	};

	enum class Language
	{
		ENGLISH = 0,
		KOREAN,
		MAX,
	};

	enum class StringLiteral
	{
		WEAPON_GB = 0,
		WEAPON_TOTAL_DECO,
		WEAPON_DECO_LEVEL,
		CHARM_GB,
		CHARM_DISPLAY_BY_NAME,
		CHARM_DISPLAY_BY_SKILL_NAME,
		NONE,
		ANY,
		SKILLS_GB,
		ADD_SKILLS,
		CLEAR,
		ADDED_SKILLS,
		SKILL_EFFECT,
		SKILL_LEVEL,
		EMPTY,
		SET_SKILL_GB,
		ADDED_SET_SKILLS,
		ARMOR_FILTER_GB,
		HEAD,
		CHEST,
		ARM,
		WAIST,
		LEG,
		SET,
		DECORATION_FILTER_GB,
		DECO_COL_JEWEL_NAME,
		DECO_COL_LEVEL,
		DECO_COL_RARITY,
		DECO_COL_SKILL,
		DECO_COL_COUNT,
		JEWEL_SUFFIX,
		SELECT_ALL,
		DESELECT_ALL,
		RESET_ALL_COUNT,
		SEARCH_OPTION_GB,
		SEARCH_BUTTON,
		STOP_BUTTON,
		SEARCH_RESULT_ARMOR_SKILL,
		SEARCH_RESULT_EXTRA_ARMOR_SKILL,
		SEARCH_RESULT_DECORATION_SKILL,
		SEARCH_RESULT_TOTAL_SKILL,
		SEARCH_RESULT_SET_SKILL,
		SEARCH_RESULT_EXTRA_SET_SKILL,
		SEARCH_RESULT_DECORATIONS,
		SEARCH_RESULT_REMAINING_DECOS,
		SEARCHING,
		SEARCH_FINISHED,
		SEARCH_TOTAL,
		SEARCH_ABORTED,
		FIRST_500_RESULT,
		REMOVE_SKILL,
		REMOVE_SET_SKILL,
		BULWARK_CHARM,
		GUARD_UP,
		URAGAAN_PROTECTION,
		EMPTY_SKILL_ERR,
		EMPTY_CHECKED_SKILL_ERR,
		TOO_MANY_REQ_ARMOR_PIECES,
		ALLOW_EXTRA_SKILL,
		ALLOW_OVERLEVELED_SKILL,
		SEARCH_FROM_HIGHER_RARIRTY,
		SIMPLIFY_SEARCH_RESULT,
		SHOW_MAX_LEVEL,
		MENU_SAVE,
		MENU_OPEN,
		MENU_EXIT,
		MENU_CLEAR,
		MENU_ALLOW_LOW_RANK_ARMOR,
		MENU_ALLOW_ARENA_ARMOR,
		MENU_ALLOW_EVENT_ARMOR,
		MENU_GENDER,
		MENU_GENDER_MALE,
		MENU_GENDER_FEMALE,
		MENNU_REPORT_ERROR,
		FAILED_TO_OPEN_FILE,
	};

	enum class TempFileIndex
	{
		TOTAL_WEAPON_DECORATIONS,
		WEAPON_DECO_LEVEL_1,
		WEAPON_DECO_LEVEL_2,
		WEAPON_DECO_LEVEL_3,
		CHARM_DISPLAY_SETTING,

	};
}

#endif