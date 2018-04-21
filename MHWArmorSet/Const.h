#ifndef CONST_H
#define CONST_H

#include <string>

namespace MHW
{
	namespace VERSION
	{
		static const int major = 1;
		static const int minor = 4;
	}

	namespace CONSTS
	{
		static const int MIN_WINDOW_W = 860;
		static const int MIN_WINDOW_H = 850;

		static const int MIN_SEARCH_RESULT_WINDOW_W = 530;
		static const int MIN_SEARCH_RESULT_WINDOW_H = 485;

		static const int MAX_SKILL_COUNT = 15;
		static const int MAX_SET_SKILL_COUNT = 2;

		static const int MAX_ARMOR_RARITY = 8;

		// const
		static const int LEFT_COLUMN_WIDTH = 300;
		static const int RIGHT_COLUMN_X = 320;
		static const int RIGHT_COLUMN_WIDTH = 520;
		static const int X_OFFSET = 10;
		static const int TEXT_OUT_X_OFFSET = 20;
		static const int TEXT_OUT_Y_OFFSET = 20;
		static const int COMBOBOX_Y_OFFSET = 17;
		static const int BUTTON_SIZE_Y = 22;
		static const int ARMOR_TEXT_Y_OFFSET = 50;
		static const int WEAPON_SLOT_SIZE_X_OFFSET = 50;
		static const int SEARCH_RESULT_GB_HEIGHT = 510;
		static const int SEARCH_BUTTON_Y = 759;
		static const int DROPDOWN_LIST_W = 285;

		static const int RICH_EDIT_X = 10;
		static const int RICH_EDIT_Y = 50;

		static const int LINESPACE = 15;

		static const int DECO_LISTBOX_W = 500;
		static const int DECO_LISTBOX_H = 295;

		static const int SETSEARCHER_ITER_TRESHOLD = 2;
		static const long long SET_SEARCHER_TIME_TRESHOLD = 1LL;

		static const int CHARM_DISPLAY_BY_NAME = 0;
		static const int CHARM_DISPLAY_BY_SKILL_NAME = 1;
	}
}

#endif