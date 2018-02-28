#include "stdafx.h"
#include "Settings.h"
#include "Const.h"
#include "Utility.h"

using namespace MHWASS::CONSTS;

const int Settings::CHARM_DISPLAY_BY_NAME = 0;
const int Settings::CHARM_DISPLAY_BY_SKILL_NAME = 1;

Settings::Settings()
	: language(MHW::Language::ENGLISH)
	, totalWeaponSlots(0)
	, weaponSlot1Size(1)
	, weaponSlot2Size(1)
	, weaponSlot3Size(1)
	, charmIndex(0)
	, charmDisplaySetting(Settings::CHARM_DISPLAY_BY_SKILL_NAME)
	, highRankSetSkill(true)
	, highRankHeadArmor(true)
	, highRankChestArmor(true)
	, highRankArmArmor(true)
	, highRankWaistArmor(true)
	, highRankLegArmor(true)
	, headArmorIndex(0)
	, chestArmorIndex(0)
	, armArmorIndex(0)
	, waistArmorIndex(0)
	, legArmorIndex(0)
	, gender(MHW::Gender::MALE)
	, allowLowRankArmor(false)
{}

bool Settings::init()
{
	if (!initStringLiterals())
	{
		return false;
	}

	return true;
}

bool Settings::initStringLiterals()
{
	// Read charm data file and init to map
	std::ifstream strLitFile("Data/" + getLanguagePath() + "/str");

	// Check if it's valid
	if (strLitFile.is_open())
	{
		std::string line;
		
		int index = 0;

		// iterate by each line
		while (std::getline(strLitFile, line))
		{
			// check line
			if (!line.empty())
			{
				stringLiterals[static_cast<MHW::StringLiteral>(index)] = Utility::utf8_decode(line);

				index++;
			}
			else
			{
				// bad file
				return false;
			}
		}

		return true;
	}
	else
	{
		// error. can't find file
		return false;
	}
}

bool Settings::readTemp()
{
	std::ifstream tempFile("temp");

	if (tempFile.is_open())
	{
		// string to read line
		std::string line;

		// result. -1 means failed to read.
		int result = -1;

		std::getline(tempFile, line);

		// total weapon decoration slots
		if (!readValue(line, weaponSlot1Size))
		{
			return false;
		}

		std::getline(tempFile, line);

		// First decoration size in weapons
		if (!readValue(line, weaponSlot1Size))
		{
			return false;
		}

		std::getline(tempFile, line);

		// Second decoration size in weapons
		if (!readValue(line, weaponSlot2Size))
		{
			return false;
		}

		std::getline(tempFile, line);

		// Third decoration size in weapons
		if (!readValue(line, weaponSlot3Size))
		{
			return false;
		}

		std::getline(tempFile, line);

		// charm index
		if (!readValue(line, charmIndex))
		{
			return false;
		}

		std::getline(tempFile, line);

		// used skill size
		int usedSkillCount = 0;
		if (!readValue(line, usedSkillCount))
		{
			return false;
		}

		if (usedSkillCount > 0)
		{
			std::vector<int> skillIds;
			std::vector<int> skillLevels;

			for (int i = 0; i < usedSkillCount; ++i)
			{
				int tempInt = 0;
				if (!readValue(line, tempInt))
				{
					return false;
				}

				skillIds.push_back(tempInt);

				if (!readValue(line, tempInt))
				{
					return false;
				}

				skillLevels.push_back(tempInt);
			}

			// Restore skills
		}


		return true;
	}
	else
	{
		return false;
	}

}

bool Settings::readValue(const std::string str, int & dest)
{
	if (str.empty()) return false;

	// convert to number
	int result = stoi(str);

	if (result)
	{
		dest = result;
		return true;
	}
	else
	{
		// failed
		clear();
		return false;
	}
}

int Settings::stoi(const std::string str)
{
	try
	{
		return std::stoi(str);
	}
	catch (...)
	{
		return -1;
	}
}

bool Settings::isSkillAdded(const int skillId)
{
	for (auto& skill : skills)
	{
		if (skill.id == skillId)
		{
			return true;
		}
	}

	return false;
}

bool Settings::isSetSkillAdded(const int setSkillId, const int groupId, const bool HR)
{
	for (auto& setSkill : setSkills)
	{
		if (setSkill.id == setSkillId || setSkill.groupId == groupId)
		{
			if (HR)
			{
				if (setSkill.highRank == true)
				{
					return true;
				}
			}
			else
			{
				if (setSkill.highRank == false)
				{
					return true;
				}
			}
		}
	}

	return false;
}

int Settings::getSkillDropdownIndexByPos(const int listPos)
{
	// there are 15 skills can be added.
	if (listPos >= MAX_SKILL_COUNT)
	{
		return -1;
	}
	else
	{
		auto iter = skills.begin();
		std::advance(iter, listPos);

		return (iter->dropdownIndex);
	}
}

int Settings::getSkillOriginalIndexByPos(const int listPos)
{
	// there are 15 skills can be added.
	if (listPos >= MAX_SKILL_COUNT)
	{
		return -1;
	}
	else
	{
		auto iter = skills.begin();
		std::advance(iter, listPos);

		return (iter->originalIndex);
	}
}

int Settings::getSetSkillDropdownIndexByPos(const int listPos)
{
	// there is only 2 set skills at a time

	if (listPos == 0)
	{
		return setSkills.front().dropdownIndex;
	}
	else if (listPos == 1)
	{
		return setSkills.back().dropdownIndex;
	}
	else
	{
		return -1;
	}
}

int Settings::getSetSkillOriginalIndexByPos(const int listPos)
{
	// there is only 2 set skills at a time

	if (listPos == 0)
	{
		return setSkills.front().originalIndex;
	}
	else if (listPos == 1)
	{
		return setSkills.back().originalIndex;
	}
	else
	{
		return -1;
	}
}

bool Settings::getSetSkillRankByPos(const int listPos)
{
	// there is only 2 set skills at a time

	if (listPos == 0)
	{
		return setSkills.front().highRank;
	}
	else if (listPos == 1)
	{
		return setSkills.back().highRank;
	}
	else
	{
		return false;
	}
}

int Settings::getTotalReqArmorPieces()
{
	if (setSkills.empty())
	{
		return 0;
	}
	else
	{
		int result = 0;

		for (auto& ss : setSkills)
		{
			if (ss.applied)
			{
				result += ss.reqArmorPieces;
			}
		}

		return result;
	}
}

std::string Settings::getLanguagePath()
{
	switch (language)
	{
	case MHW::Language::ENGLISH:
		return "English";
		break;
	case MHW::Language::KOREAN:
		return "Korean";
		break;
	default:
		break;
	}

	return "";
}

void Settings::addSkillAt(const int index, Skill* skill)
{
	if (skills.empty())
	{
		skills.push_back(*skill);
	}
	else
	{
		if (index > (int)skills.size())
		{
			// error
			return;
		}
		else if (index == (int)skills.size())
		{
			skills.push_back(*skill);
		}
		else
		{
			auto iter = skills.begin();
			std::advance(iter, index);

			skills.insert(iter, *skill);
		}
	}
}

void Settings::removeAddedSkillAt(const int index)
{
	if (index >= (int)skills.size())
	{
		// todo: add warning? error
		return;
	}
	else
	{
		auto iter = skills.begin();
		std::advance(iter, index);

		skills.erase(iter);
	}
}

void Settings::removeAddedSetSkillAt(const int index)
{
	if (index >= (int)setSkills.size())
	{
		// todo: add warning?
		return;
	}
	else
	{
		auto iter = setSkills.begin();
		std::advance(iter, index);

		setSkills.erase(iter);
	}
}

Skill * Settings::getAddedSkillAt(const int index)
{
	if (index == -1) return nullptr;

	if (index >= (int)skills.size())
	{
		// todo: add warning?
		return nullptr;
	}
	else
	{
		auto iter = skills.begin();
		std::advance(iter, index);

		return &(*iter);
	}
}

SetSkill * Settings::getAddedSetSkillAt(const int index)
{
	if (index == -1) return nullptr;

	if (index >= (int)setSkills.size())
	{
		// todo: add warning?
		return nullptr;
	}
	else
	{
		auto iter = setSkills.begin();
		std::advance(iter, index);

		return &(*iter);
	}
}

std::wstring Settings::getString(const MHW::StringLiteral e)
{
	return stringLiterals[e];
}

void Settings::clear()
{
	totalWeaponSlots = 0;

	weaponSlot1Size = 1;
	weaponSlot1Size = 1;
	weaponSlot1Size = 1;

	charmIndex = 0;

	skills.clear();


}

void Settings::print(std::map<int, Charm>& charms)
{
	{
		auto totalWeaponSlotStr = std::string("Total weapon decoration slots: " + std::to_string(totalWeaponSlots) + "\n");
		OutputDebugString(std::wstring(totalWeaponSlotStr.begin(), totalWeaponSlotStr.end()).c_str());
	}

	if (totalWeaponSlots >= 1)
	{
		auto str = std::string("First decoration size in weapon: " + std::to_string(weaponSlot1Size) + "\n");
		OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
	}

	if (totalWeaponSlots >= 2)
	{
		auto str = std::string("Second decoration size in weapon: " + std::to_string(weaponSlot2Size) + "\n");
		OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
	}

	if (totalWeaponSlots >= 3)
	{
		auto str = std::string("Third decoration size in weapon: " + std::to_string(weaponSlot3Size) + "\n");
		OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
	}

	if (charmIndex == 0)
	{
		OutputDebugString(L"Charm: None\n");
	}
	else if (charmIndex == 1)
	{
		OutputDebugString(L"Charm: Any\n");
	}
	else
	{
		OutputDebugString((L"Charm (" + std::to_wstring(charmIndex) + L")\n").c_str());
		charms[charmIndex].print(true);
	}

	if (skills.empty())
	{
		OutputDebugString(L"Skills: None\n");
	}
	else
	{
		int i = 0;
		for (auto& skill : skills)
		{
			OutputDebugString((L"Skill #" + std::to_wstring(i) + L"\n").c_str());
			skill.print(true);
			i++;
		}
	}
}

void Settings::printPrefs()
{
	OutputDebugString(L"Printing preferences...\n");
	
	// language
	std::wstring lanStr;
	
	switch (language)
	{
	case MHW::Language::ENGLISH:
		lanStr = L"English";
		break;
	case MHW::Language::KOREAN:
		lanStr = L"Korean";
		break;
	default:
		lanStr = L"Error";
		break;
	}

	OutputDebugString((L"Language: " + lanStr).c_str());

	// charm display
	OutputDebugString((L"Charm display type: " + std::wstring((charmDisplaySetting == Settings::CHARM_DISPLAY_BY_NAME) ? L"By name" : L"By skill name")).c_str());

	// set skill rank
	OutputDebugString((L"Set skill rank: " + std::wstring((highRankSetSkill) ? L"High Rank" : L"Low Rank")).c_str());

	// armor rank
	OutputDebugString((L"Head armor rank: " + std::wstring((highRankHeadArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Chest armor rank: " + std::wstring((highRankChestArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Arm armor rank: " + std::wstring((highRankArmArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Waist armor rank: " + std::wstring((highRankWaistArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Leg armor rank: " + std::wstring((highRankLegArmor) ? L"High Rank" : L"Low Rank")).c_str());
}
