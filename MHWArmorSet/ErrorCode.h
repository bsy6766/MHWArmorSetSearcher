#ifndef ERROR_CODE_H
#define ERROR_CODE_H

namespace MHW
{
	enum class ERROR_CODE
	{
		FAILED_TO_CREATE_SETTING_INSTANCE = 0,
		FAILED_TO_CREATE_DATABASE_INSTANCE,
		FAILED_TO_CREATE_SET_SEARCHER_INSTANCE,
		// initialize skill data
		FAILED_TO_READ_SKILL_DATA_FILE,
		BAD_FILE_SKILL_MAX_LEVEL_IS_NOT_NUM,
		BAD_FILE_SKILL_ONLY_FROM_DECO_IS_NOT_NUM,
		BAD_FILE_SKILL_DECO_SET_SKILL_IS_NOT_NUM,
		BAD_FILE_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		RELOAD_NAME_CANT_FIND_SKILL,
		RELOAD_NAME_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		// initialize set skilld ata
		FAILED_TO_READ_SET_SKILL_DATA_FILE,
		BAD_FILE_TOTAL_LOW_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		BAD_FILE_LOW_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_LOW_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		BAD_FILE_TOTAL_HIGH_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		BAD_FILE_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_SECOND_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_HIGH_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		RELOAD_NAME_FAILED_TO_READ_SET_SKILL_DATA_FILE,
		RELOAD_NAME_LOW_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		RELOAD_NAME_LOW_RANK_SET_SKILL_INCORECT_SPLIT_SIZE,
		RELOAD_NAME_HIGH_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		RELOAD_NAME_HIGH_RANK_SET_SKILL_INCORECT_SPLIT_SIZE,
		// initialize charm data
		FAILED_TO_READ_CHARM_DATA_FILE,
		BAD_FILE_CHARM_MAX_LEVEL_IS_NOT_NUM,
		BAD_FILE_CHARMS_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT,
		BAD_FILE_CHARMS_SECOND_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT,
		BAD_FILE_CHARM_DATA_INCORRECT_SPLIT_SIZE,
		REALOD_NAME_CANT_FIND_CHARM,
		RELOAD_CHARM_NAME_INCORRECT_SPLIT_SIZE,
		RELOAD_CHARM_NAME_EMPTY_LINE,
		// initialize guard ups
		FAILED_TO_INITIALIZE_GUARD_UP_PTR,
		// initialize armor data
		FAILED_TO_READ_ARMOR_DATA_FILE,
		BAD_FILE_ARMOR_DATA_ARE_NOT_NUM,
		BAD_FILE_ARMOR_DATA_INCORRECT_SPLIT_SIZE,
		BAD_FILE_ARMOR_DATA_SKILL_COUNT_IS_NOT_NUM,
		ARMOR_DATA_CANT_FIND_SKILL_ID_BY_NAME,
		ARMOR_DATA_CANT_FIND_SET_SKILL_BY_ID,
		BAD_FILE_ARMOR_DATA_SKILL_LEVEL_IS_NOT_NUM,
		BAD_FILE_ARMOR_DATA_DECO_COUNT_IS_NOT_NUM,
		BAD_FILE_ARMOR_DATA_DECO_SIZE_IS_NOT_NUM,
		RELOAD_NAME_ARMOR_DATA_INCORRECT_SPLIT_SIZE,
		// initialize deco data
		FAILED_TO_READ_DECO_DATA_FILE,
		BAD_FILE_DECO_SIZE_IS_NOT_NUM,
		BAD_FILE_DECO_RARITY_IS_NOT_NUM,
		DECO_SKILL_NAME_DOES_NOT_EXIST,
		BAD_FILE_DECO_SET_SKILL_REQ_ARMOR_PIECE_IS_NOT_NUM,
		DECO_SET_SKILL_NAME_DOES_NOT_EXIST,
		BAD_FILE_DECO_DATA_INCORRECT_SPLIT_SIZE,
		// Update search button
		USB_SETSEARCHER_IS_NULLPTR,
		USB_DATABASE_IS_NULLPTR,
		USB_SETTING_IS_NULLPTR,
		// creating search filter
		CFS_SKILL_IS_EMPTY,
		CFS_NO_CHECKED_SKILLS,
		CSF_CHARM_INDEX_IS_NEG_ONE,
		CSF_FAILED_TO_GET_DECO_DATA,
		CSF_DATABASE_IS_NULLPTR,
		CSF_SETTING_IS_NULLPTR,
		// Check filter
		CF_CHARM_IS_NULLPTR,
		CF_TOTAL_WEAPON_DECO_IS_NEG,
		CF_WEAPON_DECO_SIZE_IS_NEG,
		CF_SKILL_IS_EMPTY,
		CF_TOO_MANY_SET_SKILL,
		CF_TOO_MANY_HR_SET_SKILL,
		CF_TOO_MANY_LR_SET_SKILL,
		CF_TOO_MANY_REQ_ARMOR_PIECES,
		// Armor set
		AS_ADDED_WRONG_DECO_SKILLS,
		AS_CANT_FIND_SKILL_DATA,
		AS_CANT_FIND_SET_SKILL_DATA,
		AS_DIFF_SKILL_AND_SKILL_LEVEL_SIZE,
		AS_SECOND_CHARM_SKILL_SHOULD_NOT_BE_SET_SKILL,
		AS_FAILED_TO_GET_CHARM_DATA,
		// Decoration
		DECO_FAILED_TO_GET_SET_SKILL,
		// search window
		FAILED_TO_INIT_SEARCH_WINDOW,
		// weapon control total combobox
		WEAPON_DDL_ROW_INVALID,
		WEAPON_SLOT_1_DDL_ROW_INVALID,
		WEAPON_SLOT_2_DDL_ROW_INVALID,
		WEAPON_SLOT_3_DDL_ROW_INVALID,
		// charm control
		CHARM_DDL_ROW_INVALID,
		// skill control
		SKILL_DDL_ROW_INVALUD,
		ADD_SKILL_DDL_ROW_INVALID,
		SKILL_CHECKBOX_INVALID_INDEX,
		// set skil control
		SET_SKILL_DDL_ROW_INVALUD,
		ADD_SET_SKILL_DDL_ROW_INVALID,
		SET_SKILL_CHECKBOX_INVALID_INDEX,
		// set searcher
		SS_USER_PICKED_CHARM_IS_NOT_SIZE_1,
		SS_CHARM_IS_NULLPTR,
		SS_SKILL_LEVEL_SUM_DOES_NOT_EXIST,
		SS_CANT_GET_DECO_BY_SKILL_ID,
		SS_CANT_GET_DECO_SET_SKILL_BY_SET_SKILL_ID,
		SS_DB_IS_NULLPTR,
		// settings
		FAILED_TO_READ_TEMP,
		FAILED_TO_READ_STRING_DATA,
		FAILED_TO_GET_MAJOR_VERSION,
		FAILED_TO_GET_MINOR_VERISON,
		FAILED_TO_CONVERT_LANGUAGE_VAL_TO_NUM,
		FAILED_TO_CONVERT_TOTAL_WEAPON_DECO_SLOTS_TO_NUM,
		FAILED_TO_CONVERT_WEAPON_SLOT_1_LEVEL_TO_NUM,
		FAILED_TO_CONVERT_WEAPON_SLOT_2_LEVEL_TO_NUM,
		FAILED_TO_CONVERT_WEAPON_SLOT_3_LEVEL_TO_NUM,
		FAILED_TO_CONVERT_CHARM_INDEX_TO_NUM,
		FAILED_TO_CONVERT_CHARM_DISPLAY_TO_NUM,
		FAILED_TO_CONVERT_SKILL_INDEX_TO_NUM,
		FAILED_TO_CONVERT_SKILL_SIZE_TO_NUM,
		FAILED_TO_CONVERT_SKILL_ID_TO_NUM,
		FAILED_TO_CONVERT_SKILL_LEVEL_TO_NUM,
		FAILED_TO_CONVERT_SKILL_APPLIED_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_RANK_TO_BOOL,
		FAILED_TO_CONVERT_SET_SKILL_INDEX_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_SIZE_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_ID_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_REQ_ARMOR_PIECE_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_APPLIED_TO_NUM,
		FAILED_TO_CONVERT_SET_SKILL_RANK_TO_NUM,
		FAILED_TO_CONVERT_HEAD_ARMOR_RANK_TO_NUM,
		FAILED_TO_CONVERT_CHEST_ARMOR_RANK_TO_NUM,
		FAILED_TO_CONVERT_ARM_ARMOR_RANK_TO_NUM,
		FAILED_TO_CONVERT_WAIST_ARMOR_RANK_TO_NUM,
		FAILED_TO_CONVERT_LEG_ARMOR_RANK_TO_NUM,
		FAILED_TO_CONVERT_HEAD_ARMOR_INDEX_TO_NUM,
		FAILED_TO_CONVERT_CHEST_ARMOR_INDEX_TO_NUM,
		FAILED_TO_CONVERT_ARM_ARMOR_INDEX_TO_NUM,
		FAILED_TO_CONVERT_WAIST_ARMOR_INDEX_TO_NUM,
		FAILED_TO_CONVERT_LEG_ARMOR_INDEX_TO_NUM,
		FAILED_TO_CONVERT_DECO_COUNT_TO_NUM,
		FAILED_TO_CONVERT_DECO_BOOL_TO_NUM,
		FAILED_TO_CONVERT_GENDER_VAL_TO_NUM,
		FAILED_TO_CONVERT_ALLOW_LR_ARMOR_TO_NUM,
		FAILED_TO_CONVERT_ALLOW_ARENA_ARMOR_TO_NUM,
			FAILED_TO_CONVERT_MIN_ARMOR_RARITY_TO_NUM,
			FAILED_TO_CONVERT_ALLOW_EXTRA_SKILL_TO_BOOL,
			FAILED_TO_CONVERT_SEARCH_FROM_HIGHER_RARIRTY_TO_BOOL,
			FAILED_TO_CONVERT_ALLOW_OVERLEVELED_SKILL_TO_BOOL,
			FAILED_TO_CONVERT_ALLOW_ONLY_USE_MAX_LEVEL_CHARM_TO_BOOL,
		SETTING_FAILED_TO_GET_SKILL_BY_ID_AND_LEVEL,
		SETTING_FAILED_TO_GET_SET_SKILL_BY_ID_AND_RANK,
		STRING_DATA_EMPTY_LINE,
		ADD_SKILL_AT_INDEX_OUT_OF_RANGE,
		REMOVE_SKILL_AT_INDEX_OUT_OF_RANGE,
		REMOVE_SET_SKILL_AT_INDEX_OUT_OF_RANGE,
		GET_SKILL_AT_INDEX_OUT_OR_RANGE,
		GET_SET_SKILL_AT_INDEX_OUT_OR_RANGE,
	};
}

#endif