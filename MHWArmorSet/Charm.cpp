#include "stdafx.h"

#include "Charm.h"

Charm::Charm() 
	: id(-1)
	, name(L"")
	, skillName(L"")
	, secondSkillName(L"")
	, skillId(-1)
	, secondSkillId(-1)
	, setSkill(false)
	, secondSetSkill(false)
	, level(-1)
	, maxLevel(-1)
	, dropdownIndex(-1)
{}

bool Charm::hasSecondSkill()
{
	return (secondSkillId != -1);
}

void Charm::print(const bool detail)
{
	std::wstring output = L"Name: " + name + L"\nSkill Name: " + skillName;

	if (!secondSkillName.empty())
	{
		output += L"\nSkill Name 2: " + secondSkillName;
	}

	if (detail)
	{
		output += L"\nLevel: " + std::to_wstring(level);
	}

	output += L"\nMax level: " + std::to_wstring(maxLevel) + L"\n\n";

	OutputDebugString(output.c_str());
}