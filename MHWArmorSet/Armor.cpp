#include "stdafx.h"
#include "Armor.h"
#include "Database.h"

Armor::Armor()
	: setId(-1)
	, id(-1)
	, setName(L"")
	, name(L"")
	, defense(-1)
	, rarity(0)
	, highRank(false)
	, setSkillId(-1)
	, secondSetSkillId(-1)
	, setGroupId(-1)
	, gender(MHW::Gender::NONE)
	, arena(false)
	, dropdownListIndex(-1)
{}

std::wstring Armor::getDropdownListItemStr(Database * db)
{
	std::wstring itemStr = name + L" (" + setName;

	if (!skills.empty() && !skillLevels.empty())
	{
		// has skill
		int size = (int)skills.size();

		itemStr += L" / ";

		for (int i = 0; i < size; ++i)
		{
			itemStr += (db->skillIdToNameLUT[skills.at(i)] + L" " + std::to_wstring(skillLevels.at(i)));

			if (i < (size - 1))
			{
				itemStr += L",  ";
			}
		}
	}

	// decoration
	itemStr += (L" / " + getItemDecorationSizeAsStr() + L")");
	
	return itemStr;
}

std::wstring Armor::getItemDecorationSizeAsStr()
{
	std::wstring decoStr = L"";

	int size = (int)decorationSlots.size();

	for (int i = 0; i < 3; ++i)
	{
		if (i < size)
		{
			decoStr += std::to_wstring(decorationSlots.at(i));
		}
		else
		{
			//decoStr += L"¡ª";
			//decoStr += L"-";
			decoStr += L"0";
		}

		if (i < 2)
		{
			decoStr += L",  ";
		}
	}

	return decoStr;
}

bool Armor::isAnyArmor()
{
	return id >= ANY_ARMOR_ID_START;
}

void Armor::print(Database* db)
{
	OutputDebugString((L"Name: " + name + L"\n").c_str());
	OutputDebugString((L"Armor set name: " + setName + L"\n").c_str());
	OutputDebugString((L"ID: " + std::to_wstring(id) + L"\n").c_str());
	OutputDebugString((L"Set ID: " + std::to_wstring(setId) + L"\n").c_str());
	OutputDebugString((L"Defense: " + std::to_wstring(defense) + L"\n").c_str());
	OutputDebugString((L"Rarity: " + std::to_wstring(rarity) + L"\n").c_str());
	OutputDebugString((L"HR: " + std::wstring(highRank ? L"true" : L"false") + L"\n").c_str());

	const int skillSize = (int)skills.size();
	OutputDebugString((L"Skills: " + std::to_wstring(skillSize) + L"\n").c_str());

	for (int i = 0; i < skillSize; ++i)
	{
		OutputDebugString((db->getSkillNameById(skills.at(i)) + L" " + std::to_wstring(skillLevels.at(i)) + L"\n").c_str());
	}

	if (decorationSlots.empty())
	{
		OutputDebugString(L"Deco: None\n");
	}
	else
	{
		for (auto v : decorationSlots)
		{
			OutputDebugString((std::to_wstring(v) + L" ").c_str() );
		}
		OutputDebugString(L"\n");
	}

	OutputDebugString(L"\n");
}
