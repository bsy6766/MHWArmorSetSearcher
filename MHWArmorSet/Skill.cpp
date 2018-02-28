#include "stdafx.h"
#include "Skill.h"

Skill::Skill()
	: name(L"")
	, id(-1)
	, originalIndex(-1)
	, dropdownIndex(-1)
	, level(-1)
	, maxLevel(-1)
	, applied(false)
	, onlyFromDeco(false)
	, fromDecoSetSkill(false)
	, description(L"")
	, levelDescription(L"")
{}

void Skill::print(const bool detail)
{
	std::wstring output = L"Name: " + name;
	
	if (detail)
	{
		output += L"\n" + description;
		output += L"\n" + levelDescription;

		output += L"\nLevel: " + std::to_wstring(level);
	}

	output += L"\nMax level: " + std::to_wstring(maxLevel) + L"\n\n";

	OutputDebugString(output.c_str());
}



SetSkill::SetSkill()
	: name(L"")
	, skillName(L"")
	, id(-1)
	, groupId(-1)
	, originalIndex(-1)
	, dropdownIndex(-1)
	, reqArmorPieces(-1)
	, applied(false)
	, highRank(false)
	, description(L"")
	, hasSibling(false)
	, sibling(nullptr)
{}

void SetSkill::print()
{
	std::wstring output = L"Name: " + name + L"\nSkill name: " + skillName + L"\nRequired armoer pieces: " + std::to_wstring(reqArmorPieces) + L"\nDescription: " + description + L"\n\n";

	OutputDebugString(output.c_str());
}
