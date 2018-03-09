#include "stdafx.h"
#include "Decoration.h"
#include "Database.h"
#include "Utility.h"
#include "Logger.h"

Decoration::Decoration()
	: name(L"")
	, id(-1)
	, skillId(-1)
	, setSkillId(-1)
	, setSkill(false)
	, highRankSetSkill(false)
	, size(-1)
	, rarity(-1)
{}

std::wstring Decoration::getItemWStr(Database* db)
{
	std::wstring itemStr = name + L" Jewel " + std::to_wstring(size) + L" (R" + std::to_wstring(rarity);

	if (this->setSkill)
	{
		SetSkill* setSkill = db->getSetSkillByID(setSkillId, highRankSetSkill);

		if (setSkill)
		{
			itemStr += (L", " + setSkill->skillName + L", " + setSkill->name + L" " + std::to_wstring(setSkill->reqArmorPieces));
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::DECO_FAILED_TO_GET_SET_SKILL);
		}
	}
	else
	{
		itemStr += L", " + db->getSkillNameById(skillId);
	}

	itemStr += L")";

	return itemStr;
}

std::vector<std::wstring> Decoration::getStr(Database * db)
{
	std::vector<std::wstring> ret;

	if (this->setSkill)
	{
		SetSkill* setSkill = db->getSetSkillByID(setSkillId, highRankSetSkill);

		if (setSkill)
		{
			//itemStr += (L", " + setSkill->skillName + L", " + setSkill->name + L" " + std::to_wstring(setSkill->reqArmorPieces));
			ret.push_back(setSkill->skillName);
			ret.push_back(setSkill->name);
			ret.push_back(std::to_wstring(setSkill->reqArmorPieces));
		}
		else
		{
			MHW::Logger::getInstance().error("Failed to get set skill from decoration");
		}
	}
	else
	{
		Skill* skill = db->getSkillByIDAndLevel(skillId, 1);
		if (skill)
		{
			ret.push_back(skill->name);
		}
		else
		{
			MHW::Logger::getInstance().error("Failed to get skill from decoration");
		}
	}

	return ret;
}

void Decoration::print()
{
	OutputDebugString((L"Decoration: " + name + L"\n").c_str());
	OutputDebugString((L"id: " + std::to_wstring(id) + L"\n").c_str());
	OutputDebugString((L"rarity: " + std::to_wstring(rarity) + L"\n").c_str());
	OutputDebugString((L"size: " + std::to_wstring(size) + L"\n").c_str());
	if (setSkill)
	{
		OutputDebugString((L"Set skill: true\nSet skill id: " + std::to_wstring(setSkillId) + L"\n").c_str());
		if (highRankSetSkill)
		{
			OutputDebugString(L"Set skill rank: High Rank\n");
		}
		else
		{
			OutputDebugString(L"Set skill rank: Low Rank\n");
		}
	}
	else
	{
		OutputDebugString(L"Set skill: false\n");
	}
	OutputDebugString(L"\n");
}