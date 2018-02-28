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
		// initialize set skilld ata
		FAILED_TO_READ_SET_SKILL_DATA_FILE,
		BAD_FILE_TOTAL_LOW_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		BAD_FILE_LOW_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_LOW_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		BAD_FILE_TOTAL_HIGH_RANK_SET_SKILL_COUNT_IS_NOT_NUM,
		BAD_FILE_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_SECOND_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM,
		BAD_FILE_HIGH_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE,
		// initialize charm data
		FAILED_TO_READ_CHARM_DATA_FILE,
		BAD_FILE_CHARM_MAX_LEVEL_IS_NOT_NUM,
		BAD_FILE_CHARMS_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT,
		BAD_FILE_CHARMS_SECOND_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT,
		BAD_FILE_CHARM_DATA_INCORRECT_SPLIT_SIZE,
		// initialize guard ups
		FAILED_TO_INITIALIZE_GUARD_UP_PTR,
		// initialize armor data
		FAILED_TO_READ_ARMOR_DATA_FILE,
		BAD_FILE_ARMOR_DATA_ARE_NOT_NUM,
		BAD_FILE_ARMOR_DATA_INCORRECT_SPLIT_SIZE,
		BAD_FILE_ARMOR_DATA_SKILL_COUNT_IS_NOT_NUM,
		ARMOR_DATA_CANT_FIND_SKILL_ID,
		BAD_FILE_ARMOR_DATA_SKILL_LEVEL_IS_NOT_NUM,
		BAD_FILE_ARMOR_DATA_DECO_COUNT_IS_NOT_NUM,
		BAD_FILE_ARMOR_DATA_DECO_SIZE_IS_NOT_NUM,
		// initialize deco data
		FAILED_TO_READ_DECO_DATA_FILE,
		BAD_FILE_DECO_SIZE_IS_NOT_NUM,
		BAD_FILE_DECO_RARITY_IS_NOT_NUM,
		DECO_SKILL_NAME_DOES_NOT_EXIST,
		BAD_FILE_DECO_SET_SKILL_REQ_ARMOR_PIECE_IS_NOT_NUM,
		DECO_SET_SKILL_NAME_DOES_NOT_EXIST,
		BAD_FILE_DECO_DATA_INCORRECT_SPLIT_SIZE,
		/*
		FAILED_TO_INITIALIZE_DATABASE,
		FAILED_TO_INITIALIZE_SKILL_DATA,
		FAILED_TO_INITIALIZE_SET_SKILL_DATA,
		FAILED_TO_INITIALIZE_DECO_SET_SKILL_DATA,
		FAILED_TO_INITIALIZE_CHARM_DATA,
		FAILED_TO_INITIALIZE_ARMOR_DATA,
		FAILED_TO_INITIALIZE_DECORATION_DATA,
		*/
	};
}

#endif