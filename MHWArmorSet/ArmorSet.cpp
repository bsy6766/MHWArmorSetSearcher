#include "stdafx.h"
#include "ArmorSet.h"
#include "Database.h"
#include "Settings.h"
#include "Logger.h"

int MHW::ArmorSet::idCounter = 1;

MHW::ArmorSet::ArmorSet()
	: id(-1)
	, perfectSkillMatch(false)
	, totalSize1DecorationSlots(0)
	, totalSize2DecorationSlots(0)
	, totalSize3DecorationSlots(0)
	, totalUsedSize1DecorationSlot(0)
	, totalUsedSize2DecorationSlot(0)
	, totalUsedSize3DecorationSlot(0)
	, setSkillPassed(false)
	, skillPassed(false)
	, headArmor(nullptr)
	, chestArmor(nullptr)
	, armArmor(nullptr)
	, waistArmor(nullptr)
	, legArmor(nullptr)
	, charm(nullptr)
{}

std::wstring MHW::ArmorSet::toResultStr(Database* db, Settings* setting)
{
	/*
	Set 1
	==================================================
	Active skills: Skill 1, Skill 2, skill 3, skill4, skill 5
	Extra skills: skill 6, skill 7, skill 8, skill 9, skill 10
	------------------------------------------------------------------------------------------
	head - deco - deco / skill1 skill2 skill 3
	chest
	arm
	waist
	leg
	charm
	------------------------------------------------------------------------------------------
	Total decorations: deco #, deco #, deco #, deco #
	Remaining slots (size): (3)
	==================================================
	*/

	//std::wstring dbr = L"==================================================\n";
	//std::wstring br = L"------------------------------------------------------------------------------------------\n";

	std::wstring dbr = L"=========================\n";
	std::wstring br = L"----------------------\n";
	
	std::wstring str = setting->getString(MHW::StringLiteral::SET) + L" " + std::to_wstring(id) + L"\n";

	// double br
	str += dbr;

	// add skills
	if (!setting->simplifySearchResult)
	{
		str += getArmorSkillsStr(db, setting);
		str += getExtraRmorSkillsStr(db, setting);
		str += getDecoSkillsStr(db, setting);
	}
	str += getTotalSkillsStr(db, setting);

	str += br;

	// set skill
	bool isNone = false;
	auto setSkillStr = getSetSkillStr(db, setting, isNone);

	if (isNone)
	{
		if (!setting->simplifySearchResult)
		{
			str += setSkillStr;
			str += br;
		}
	}
	else
	{
		str += setSkillStr;
		str += br;
	}

	// head 
	str += (setting->getString(MHW::StringLiteral::HEAD) + L": ");

	if (headArmor)
	{
		if (headArmor->id >= ANY_ARMOR_ID_START)
		{
			if (headArmor->decorationSlots.empty())
			{
				str += (L"Any armor\n");
			}
			else
			{
				str += (L"Any armor with deco slots (" + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
		else
		{
			str += headArmor->name;

			if (setting->simplifySearchResult)
			{
				str += L"\n";
			}
			else
			{
				str += (L" (" + headArmor->setName + L" / " + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
	}
	else
	{
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	// chest
	str += (setting->getString(MHW::StringLiteral::CHEST) + L": ");

	if (chestArmor)
	{
		if (chestArmor->id >= ANY_ARMOR_ID_START)
		{
			if (chestArmor->decorationSlots.empty())
			{
				str += (L"Any armor\n");
			}
			else
			{
				str += (L"Any armor with deco slots (" + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
		else
		{
			str += chestArmor->name;

			if (setting->simplifySearchResult)
			{
				str += L"\n";
			}
			else
			{
				str += (L" (" + chestArmor->setName + L" / " + getItemDecoSizeAsStr(chestArmorDecoSlots) + L")\n");
			}
		}
	}
	else
	{
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	// arm
	str += (setting->getString(MHW::StringLiteral::ARM) + L": ");

	if (armArmor)
	{
		if (armArmor->id >= ANY_ARMOR_ID_START)
		{
			if (armArmor->decorationSlots.empty())
			{
				str += (L"Any armor\n");
			}
			else
			{
				str += (L"Any armor with deco slots (" + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
		else
		{
			str += armArmor->name;

			if (setting->simplifySearchResult)
			{
				str += L"\n";
			}
			else
			{
				str += (L" (" + armArmor->setName + L" / " + getItemDecoSizeAsStr(armArmorDecoSlots) + L")\n");
			}
		}
	}
	else
	{
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	// waist
	str += (setting->getString(MHW::StringLiteral::WAIST) + L": ");

	if (waistArmor)
	{
		if (waistArmor->id >= ANY_ARMOR_ID_START)
		{
			if (waistArmor->decorationSlots.empty())
			{
				str += (L"Any armor\n");
			}
			else
			{
				str += (L"Any armor with deco slots (" + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
		else
		{
			str += waistArmor->name;

			if (setting->simplifySearchResult)
			{
				str += L"\n";
			}
			else
			{
				str += (L" (" + waistArmor->setName + L" / " + getItemDecoSizeAsStr(waistArmorDecoSlots) + L")\n");
			}
		}
	}
	else
	{
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	// leg
	str += (setting->getString(MHW::StringLiteral::LEG) + L": ");

	if (legArmor)
	{
		if (legArmor->id >= ANY_ARMOR_ID_START)
		{
			if (legArmor->decorationSlots.empty())
			{
				str += (L"Any armor\n");
			}
			else
			{
				str += (L"Any armor with deco slots (" + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
			}
		}
		else
		{
			str += legArmor->name;

			if (setting->simplifySearchResult)
			{
				str += L"\n";
			}
			else
			{
				str += (L" (" + legArmor->setName + L" / " + getItemDecoSizeAsStr(legArmorDecoSlots) + L")\n");
			}
		}
	}
	else
	{
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	// charm
	str += (setting->getString(MHW::StringLiteral::CHARM_GB) + L": ");

	if (charm)
	{
		str += charm->name + L" " + std::to_wstring(charm->level);

		if (setting->showMaxLevel)
		{
			str += L" (" + std::to_wstring(charm->maxLevel) + L")\n";
		}
		else
		{
			str += L"\n";
		}
	}
	else
	{
		// Not using charm
		str += (setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}

	str += br;

	str += setting->getString(MHW::StringLiteral::SEARCH_RESULT_DECORATIONS);

	if (usedDecorations.empty())
	{
		str += (L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n");
	}
	else
	{
		auto& logger = MHW::Logger::getInstance();

		if (setting->language == MHW::Language::ENGLISH)
		{
			if (usedDecorations.size() > 1)
			{
				str += L"s";
			}
		}

		str += L": ";

		for (auto iter = usedDecorations.begin(); iter != usedDecorations.end();)
		{
			auto find_deco = db->decorations.find(iter->first);
			if (find_deco == db->decorations.end())
			{
				logger.error("Decoration doesn't exists. id:" + std::to_string(iter->first));
				iter++;
				continue;
			}
			else
			{
				Decoration* deco = &(find_deco->second);

				str += deco->name;

				if (setting->language == MHW::Language::ENGLISH)
				{
					str += L" ";
				}

				str += (setting->getString(MHW::StringLiteral::JEWEL_SUFFIX) + L" x " + std::to_wstring(iter->second));
			}

			iter++;

			if (iter != usedDecorations.end())
			{
				str += L", ";
			}
		}

		str += L"\n";
	}

	// required decorations
	

	/*
	str += L"Total decoration slots\n";
	str += (L"Size 1: " + std::to_wstring(totalSize1DecorationSlots) + L"\n");
	str += (L"Size 2: " + std::to_wstring(totalSize2DecorationSlots) + L"\n");
	str += (L"Size 3: " + std::to_wstring(totalSize3DecorationSlots) + L"\n");
	str += L"Total used decoration slots\n";
	str += (L"Size 1: " + std::to_wstring(totalUsedSize1DecorationSlot) + L"\n");
	str += (L"Size 2: " + std::to_wstring(totalUsedSize2DecorationSlot) + L"\n");
	str += (L"Size 3: " + std::to_wstring(totalUsedSize3DecorationSlot) + L"\n");
	str += L"Total remaining decoration slots\n";
	str += (L"Size 1: " + std::to_wstring(totalSize1DecorationSlots - totalUsedSize1DecorationSlot) + L"\n");
	str += (L"Size 2: " + std::to_wstring(totalSize2DecorationSlots - totalUsedSize2DecorationSlot) + L"\n");
	str += (L"Size 3: " + std::to_wstring(totalSize3DecorationSlots - totalUsedSize3DecorationSlot) + L"\n");
	str += br;
	*/

	int level1DecoSpare = totalSize1DecorationSlots - totalUsedSize1DecorationSlot;
	int level2DecoSpare = totalSize2DecorationSlots - totalUsedSize2DecorationSlot;
	int level3DecoSpare = totalSize3DecorationSlots - totalUsedSize3DecorationSlot;

	if (level1DecoSpare < 0)
	{
		level2DecoSpare += level1DecoSpare;
		level1DecoSpare = 0;
	}

	if (level2DecoSpare < 0)
	{
		level3DecoSpare += level2DecoSpare;
		level2DecoSpare = 0;
	}

	str += setting->getString(MHW::StringLiteral::SEARCH_RESULT_REMAINING_DECOS);

	if (level1DecoSpare == 0 && level2DecoSpare == 0 && level3DecoSpare == 0)
	{
		str += L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n";
	}
	else
	{
		str += L"\n";

		if (level1DecoSpare != 0)
		{
			str += L"Level 1: " + std::to_wstring(level1DecoSpare) + L"\n";
		}

		if (level2DecoSpare != 0)
		{
			str += L"Level 2: " + std::to_wstring(level2DecoSpare) + L"\n";
		}

		if (level3DecoSpare != 0)
		{
			str += L"Level 3: " + std::to_wstring(level3DecoSpare) + L"\n";
		}
	}

	str += dbr;

	str += L"\n";

	return str;
}

std::wstring MHW::ArmorSet::getArmorSkillsStr(Database * db, Settings* setting)
{
	if (skillLevelSums.empty())
	{
		return setting->getString(MHW::StringLiteral::SEARCH_RESULT_ARMOR_SKILL) + L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n";
	}
	else
	{
		std::wstring str = setting->getString(MHW::StringLiteral::SEARCH_RESULT_ARMOR_SKILL);

		if (setting->language == MHW::Language::ENGLISH)
		{
			if (skillLevelSums.size() > 1)
			{
				str += L"s";
			}
		}

		str += L": ";

		auto iter = skillLevelSums.begin();
		for (; iter != skillLevelSums.end();)
		{
			str += (db->getSkillNameById(iter->first) + L" " + std::to_wstring(iter->second));

			iter++;

			if (iter != skillLevelSums.end())
			{
				str += L", ";
			}
		}

		str += L"\n";

		return str;
	}
}

std::wstring MHW::ArmorSet::getExtraRmorSkillsStr(Database * db, Settings* setting)
{
	if (extraSkillLevelSums.empty())
	{
		return setting->getString(MHW::StringLiteral::SEARCH_RESULT_EXTRA_ARMOR_SKILL) + L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n";
	}
	else
	{
		std::wstring str = setting->getString(MHW::StringLiteral::SEARCH_RESULT_EXTRA_ARMOR_SKILL);

		if (setting->language == MHW::Language::ENGLISH)
		{
			if (extraSkillLevelSums.size() > 1)
			{
				str += L"s";
			}
		}

		str += L": ";

		auto iter = extraSkillLevelSums.begin();
		for (; iter != extraSkillLevelSums.end();)
		{
			str += (db->getSkillNameById(iter->first) + L" " + std::to_wstring(iter->second));

			iter++;

			if (iter != extraSkillLevelSums.end())
			{
				str += L", ";
			}
		}

		str += L"\n";

		return str;
	}
}

std::wstring MHW::ArmorSet::getDecoSkillsStr(Database * db, Settings* setting)
{
	if (decoSkillLevelSums.empty())
	{
		return setting->getString(MHW::StringLiteral::SEARCH_RESULT_DECORATION_SKILL) + L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n";
	}
	else
	{
		std::wstring str = setting->getString(MHW::StringLiteral::SEARCH_RESULT_DECORATION_SKILL);

		if (setting->language == MHW::Language::ENGLISH)
		{
			if (decoSkillLevelSums.size() > 1)
			{
				str += L"s";
			}
		}

		str += L": ";

		auto iter = decoSkillLevelSums.begin();
		for (; iter != decoSkillLevelSums.end();)
		{
			str += (db->getSkillNameById(iter->first) + L" " + std::to_wstring(iter->second));

			iter++;

			if (iter != decoSkillLevelSums.end())
			{
				str += L", ";
			}
		}

		str += L"\n";

		return str;
	}
}

std::wstring MHW::ArmorSet::getTotalSkillsStr(Database * db, Settings * setting)
{
	auto copy = skillLevelSums;

	for (auto& e : decoSkillLevelSums)
	{
		copy[e.first] += e.second;
	}

	auto& logger = MHW::Logger::getInstance();

	if (copy.size() != skillLevelSums.size())
	{
		logger.errorCode(MHW::ERROR_CODE::AS_ADDED_WRONG_DECO_SKILLS);
	}

	if (copy.empty())
	{
		return setting->getString(MHW::StringLiteral::SEARCH_RESULT_TOTAL_SKILL) + L": " + setting->getString(MHW::StringLiteral::NONE) + L"\n";
	}
	else
	{
		std::wstring str = setting->getString(MHW::StringLiteral::SEARCH_RESULT_TOTAL_SKILL);

		if (setting->language == MHW::Language::ENGLISH)
		{
			if (copy.size() > 1)
			{
				str += L"s";
			}
		}

		str += L": ";

		auto iter = copy.begin();
		for (; iter != copy.end();)
		{
			Skill* skill = db->getSkillByIDAndLevel(iter->first, 1);

			if (skill)
			{
				//str += (db->getSkillNameById(iter->first) + L" " + std::to_wstring(iter->second));
				str += (skill->name + L" " + std::to_wstring(iter->second));

				if (setting->showMaxLevel)
				{
					str += (L" (" + std::to_wstring(skill->maxLevel) + L")");
				}

				iter++;

				if (iter != copy.end())
				{
					str += L", ";
				}
			}
			else
			{
				logger.errorCode(MHW::ERROR_CODE::AS_CANT_FIND_SKILL_DATA);
			}
		}

		str += L"\n";

		return str;
	}
}

std::wstring MHW::ArmorSet::getSetSkillStr(Database * db, Settings* setting, bool& isNone)
{
	std::wstring str = L"";
	
	str += setting->getString(MHW::StringLiteral::SEARCH_RESULT_SET_SKILL);

	if (setting->language == MHW::Language::ENGLISH)
	{
		if (activatedHighRankSetSkills.size() > 1)
		{
			str += L"s ";
		}
	}

	str += L": ";

	if (lowRankSetSkillArmorPieceSums.empty() && highRankSetSkillArmorPieceSums.empty())
	{
		str += setting->getString(MHW::StringLiteral::NONE);
		isNone = true;
	}
	else
	{
		bool empty = true;

		if (!lowRankSetSkillArmorPieceSums.empty())
		{
			// Have some low rank set skill
		}

		if (!highRankSetSkillArmorPieceSums.empty())
		{
			// Have some high rank set skill
			for (auto iter = highRankSetSkillArmorPieceSums.begin(); iter != highRankSetSkillArmorPieceSums.end();)
			{
				const int ssgId = iter->first;
				const int armorPices = iter->second;

				bool found = false;

				for (auto& e : db->highRankSetSkills)
				{
					SetSkill& setSkill = (e.second);
					if (setSkill.groupId == ssgId)
					{
						if (setSkill.reqArmorPieces <= armorPices)
						{
							// found
							str += (setSkill.name + L" " + std::to_wstring(setSkill.reqArmorPieces));

							if (setSkill.hasSibling)
							{
								if (setSkill.sibling)
								{
									if (setSkill.sibling->reqArmorPieces <= armorPices)
									{
										// found
										found = true;
										empty = false;
										str += L", " + (setSkill.sibling->name + L" " + std::to_wstring(setSkill.sibling->reqArmorPieces));
										break;
									}
								}
							}

							found = true;
							empty = false;
							break;
						}
					}
				}

				iter++;

				if (found)
				{
					if (iter != highRankSetSkillArmorPieceSums.end())
					{
						str += L", ";
					}
				}
			}
		}

		if (empty)
		{
			str += setting->getString(MHW::StringLiteral::NONE);
			isNone = true;
		}
	}


	str += L"\n";

	str += setting->getString(MHW::StringLiteral::SEARCH_RESULT_EXTRA_SET_SKILL);

	if (setting->language == MHW::Language::ENGLISH)
	{
		if (activatedHighRankSetSkills.size() > 1)
		{
			str += L"s ";
		}
	}

	str += L": ";

	if (extraLowRankSetSkillArmorPieceSums.empty() && extraHighRankSetSkillArmorPieceSums.empty())
	{
		str += setting->getString(MHW::StringLiteral::NONE);
		isNone = true;
	}
	else
	{
		bool empty = true;

		if (!extraLowRankSetSkillArmorPieceSums.empty())
		{
			// Have some extra low rank set skill
		}

		if (!extraHighRankSetSkillArmorPieceSums.empty())
		{
			// Have some high rank extra set skill
			bool first = true;

			for (auto iter = extraHighRankSetSkillArmorPieceSums.begin(); iter != extraHighRankSetSkillArmorPieceSums.end(); iter++)
			{
				const int ssgId = iter->first;
				const int armorPices = iter->second;

				bool found = false;

				for (auto& e : db->highRankSetSkills)
				{
					SetSkill& setSkill = (e.second);
					if (setSkill.groupId == ssgId)
					{
						if (setSkill.reqArmorPieces <= armorPices)
						{
							// found
							if (first)
							{
								first = false;
								empty = false;
								str += (setSkill.name + L" " + std::to_wstring(setSkill.reqArmorPieces));
							}
							else
							{

								str += L", " + (setSkill.name + L" " + std::to_wstring(setSkill.reqArmorPieces));
							}


							if (setSkill.hasSibling)
							{
								if (setSkill.sibling)
								{
									if (setSkill.sibling->reqArmorPieces <= armorPices)
									{
										// found
										found = true;
										empty = false;
										str += L", " + (setSkill.sibling->name + L" " + std::to_wstring(setSkill.sibling->reqArmorPieces));
										break;
									}
								}
							}

							found = true;
							break;
						}
					}
				}
			}
		}

		if (empty)
		{
			str += setting->getString(MHW::StringLiteral::NONE);
			isNone = true;
		}
	}

	str += L"\n";

	return str;
}

void MHW::ArmorSet::clearSums()
{
	skillLevelSums.clear();
	extraSkillLevelSums.clear();

	lowRankSetSkillArmorPieceSums.clear();
	extraLowRankSetSkillArmorPieceSums.clear();
	highRankSetSkillArmorPieceSums.clear();
	extraHighRankSetSkillArmorPieceSums.clear();

	activatedLowRankSetSkills.clear();
	activatedHighRankSetSkills.clear();
}

void MHW::ArmorSet::initSums(std::vector<Skill*>& filterSkills, const std::vector<SetSkill*>& reqLRSetSkills, const std::vector<SetSkill*>& reqHRSetSkills)
{
	for (auto skill : filterSkills)
	{
		skillLevelSums[skill->id] = 0;
	}

	for (auto setSkill : reqLRSetSkills)
	{
		lowRankSetSkillArmorPieceSums[setSkill->groupId] = 0;
	}

	for (auto setSkill : reqHRSetSkills)
	{
		highRankSetSkillArmorPieceSums[setSkill->groupId] = 0;
	}
}

void MHW::ArmorSet::countSums()
{
	// head
	countSums(headArmor);

	// chest
	countSums(chestArmor);

	// arm
	countSums(armArmor);

	// waist
	countSums(waistArmor);

	// leg
	countSums(legArmor);
}

void MHW::ArmorSet::countSums(Armor * armor)
{
	if (armor == nullptr)
	{
		return;
	}
	else
	{
		// Check skill
		if (!armor->skills.empty() && !armor->skillLevels.empty())
		{
			// has skill
			const int skillSize = armor->skills.size();
			// check size
			if (skillSize == armor->skillLevels.size())
			{
				// valid
				int i = 0;
				// iterate head skills
				for (; i < skillSize; ++i)
				{
					// add skill level
					addSkillLevelToSum(armor->skills.at(i), armor->skillLevels.at(i));
				}
			}
			else
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::AS_DIFF_SKILL_AND_SKILL_LEVEL_SIZE);
			}
		}

		// Check set skill
		if (armor->setSkillId != -1)
		{
			// has set skill. Don't need to check second set skill here.
			addArmorPieceToSum(armor->setGroupId, 1, armor->highRank);
		}
	}
}

/*
void MHW::ArmorSet::checkActviatedSetSkills(Database* db, const std::vector<SetSkill*>& reqLRSetSkills, const std::vector<SetSkill*>& reqHRSetSkills)
{
	if (!lowRankSetSkillArmorPieceSums.empty())
	{

	}

	if (!extraLowRankSetSkillArmorPieceSums.empty())
	{

	}

	if (!highRankSetSkillArmorPieceSums.empty())
	{
		for (auto& e : highRankSetSkillArmorPieceSums)
		{
			const int setSkillGroupId = e.first;
			const int setSkillArmorPieces = e.second;

			if (setSkillArmorPieces < 2)
			{
				// At least need 2
				continue;
			}
			else
			{
				//auto setSkill = db->getSetSkillByGroupIDAndReqArmorPieces(setSkillGroupId, setSkillArmorPieces);
			}
		}
	}

	if (!extraHighRankSetSkillArmorPieceSums.empty())
	{

	}
}
*/

void MHW::ArmorSet::addSkillLevelToSum(const int skillID, const int skillLevel)
{
	auto find_it = skillLevelSums.find(skillID);

	if (find_it == skillLevelSums.end())
	{
		auto find_extra = extraSkillLevelSums.find(skillID);

		if (find_extra == extraSkillLevelSums.end())
		{
			extraSkillLevelSums[skillID] = skillLevel;
		}
		else
		{
			extraSkillLevelSums[skillID] += skillLevel;
		}
	}
	else
	{
		skillLevelSums[skillID] += skillLevel;
	}
}

void MHW::ArmorSet::addCharmSkillLevelSums()
{
	if (charm == nullptr)
	{
		// there is no charm
		return;
	}
	else
	{
		if (charm)
		{
			// valid charm
			if (charm->setSkill)
			{
				// first skill is set skill
			}
			else
			{
				// first skill is not set skill
				addSkillLevelToSum(charm->skillId, charm->level);
			}

			// check second skill
			if (charm->hasSecondSkill())
			{
				// has second skill
				if (charm->secondSetSkill)
				{
					// second skill is set skill
					MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::AS_SECOND_CHARM_SKILL_SHOULD_NOT_BE_SET_SKILL);
				}
				else
				{
					// second skill is not set skill
					addSkillLevelToSum(charm->secondSkillId, charm->level);
				}
			}
			// Else, doesn't have second skill
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::AS_FAILED_TO_GET_CHARM_DATA);
		}
	}
}

void MHW::ArmorSet::addCharmSkillLevelSumByOne(Charm* charm)
{
	if (charm)
	{
		// valid charm
		if (charm->setSkill)
		{
			// first skill is set skill
		}
		else
		{
			// first skill is not set skill
			addSkillLevelToSum(charm->skillId, 1);
		}

		// check second skill
		if (charm->hasSecondSkill())
		{
			// has second skill
			if (charm->secondSetSkill)
			{
				// second skill is set skill
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::AS_SECOND_CHARM_SKILL_SHOULD_NOT_BE_SET_SKILL);
			}
			else
			{
				// second skill is not set skill
				addSkillLevelToSum(charm->secondSkillId, 1);
			}
		}
		// Else, doesn't have second skill
	}
	else
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::AS_FAILED_TO_GET_CHARM_DATA);
	}
}

void MHW::ArmorSet::addArmorPieceToSum(const int setSkillID, const int reqArmorPieces, const bool HR)
{
	if (HR)
	{
		auto find_it = highRankSetSkillArmorPieceSums.find(setSkillID);

		if (find_it == highRankSetSkillArmorPieceSums.end())
		{
			auto find_extra = extraHighRankSetSkillArmorPieceSums.find(setSkillID);

			if (find_extra == extraHighRankSetSkillArmorPieceSums.end())
			{
				extraHighRankSetSkillArmorPieceSums[setSkillID] = reqArmorPieces;
			}
			else
			{
				extraHighRankSetSkillArmorPieceSums[setSkillID] += reqArmorPieces;
			}
		}
		else
		{
			highRankSetSkillArmorPieceSums[setSkillID] += reqArmorPieces;
		}
	}
	else
	{
		auto find_it = lowRankSetSkillArmorPieceSums.find(setSkillID);

		if (find_it == lowRankSetSkillArmorPieceSums.end())
		{
			auto find_extra = extraLowRankSetSkillArmorPieceSums.find(setSkillID);

			if (find_extra == extraLowRankSetSkillArmorPieceSums.end())
			{
				extraLowRankSetSkillArmorPieceSums[setSkillID] = reqArmorPieces;
			}
			else
			{
				extraLowRankSetSkillArmorPieceSums[setSkillID] += reqArmorPieces;
			}
		}
		else
		{
			lowRankSetSkillArmorPieceSums[setSkillID] += reqArmorPieces;
		}
	}
}

void MHW::ArmorSet::setHeadrmor(Armor * armor)
{
	//this->headArmorIndex = armor->id;

	if (armor)
	{
		headArmor = armor;
		copyDecoSlots(headArmor->decorationSlots, headArmorDecoSlots);
	}
	else
	{
		headArmor = nullptr;
		headArmorDecoSlots = { 0 };
	}
}

void MHW::ArmorSet::setChestArmor(Armor * armor)
{
	if (armor)
	{
		chestArmor = armor;
		copyDecoSlots(chestArmor->decorationSlots, chestArmorDecoSlots);
	}
	else
	{
		chestArmor = nullptr;
		chestArmorDecoSlots = { 0 };
	}
}

void MHW::ArmorSet::setArmArmor(Armor * armor)
{
	if (armor)
	{
		armArmor = armor;
		copyDecoSlots(armArmor->decorationSlots, armArmorDecoSlots);
	}
	else
	{
		armArmor = nullptr;
		armArmorDecoSlots = { 0 };
	}
}

void MHW::ArmorSet::setWaistArmor(Armor * armor)
{
	if (armor)
	{
		waistArmor = armor;
		copyDecoSlots(waistArmor->decorationSlots, waistArmorDecoSlots);
	}
	else
	{
		waistArmor = nullptr;
		waistArmorDecoSlots = { 0 };
	}
}

void MHW::ArmorSet::setLegArmor(Armor * armor)
{
	if (armor)
	{
		legArmor = armor;
		copyDecoSlots(legArmor->decorationSlots, legArmorDecoSlots);
	}
	else
	{
		legArmor = nullptr;
		legArmorDecoSlots = { 0 };
	}
}

void MHW::ArmorSet::copyDecoSlots(const std::vector<int>& src, std::array<int, 3>& dest)
{
	// clear
	dest.at(0) = 0;
	dest.at(1) = 0;
	dest.at(2) = 0;

	const int size = (int)src.size();

	for (int i = 0; i < size; ++i)
	{
		if (i == 0)
		{
			dest.at(0) = src.at(0);
		}
		else if (i == 1)
		{
			dest.at(1) = src.at(1);
		}
		else if (i == 2)
		{
			dest.at(2) = src.at(2);
		}
	}
}

void MHW::ArmorSet::addWeaponDecoSlots(const std::vector<int>& weaponDecoSlots, const int count)
{
	if (count == 0)
	{
		return;
	}

	for (int i = 0; i < count; i++)
	{
		if (weaponDecoSlots.at(i) == 1)
		{
			totalSize1DecorationSlots++;
		}
		else if (weaponDecoSlots.at(i) == 2)
		{
			totalSize2DecorationSlots++;
		}
		else if (weaponDecoSlots.at(i) == 3)
		{
			totalSize3DecorationSlots++;
		}
	}
}

void MHW::ArmorSet::countTotalDecoSizeBySizeFromArmors()
{
	// clear
	totalSize1DecorationSlots = 0;
	totalSize2DecorationSlots = 0;
	totalSize3DecorationSlots = 0;

	// from armors
	countDecoSizeBySize(headArmorDecoSlots);
	countDecoSizeBySize(chestArmorDecoSlots);
	countDecoSizeBySize(armArmorDecoSlots);
	countDecoSizeBySize(waistArmorDecoSlots);
	countDecoSizeBySize(legArmorDecoSlots);
}

void MHW::ArmorSet::countDecoSizeBySize(const std::array<int, 3>& decoSlots)
{
	for (auto decoSize : decoSlots)
	{
		if (decoSize == 1)
		{
			totalSize1DecorationSlots++;
		}
		else if (decoSize == 2)
		{
			totalSize2DecorationSlots++;
		}
		else if (decoSize == 3)
		{
			totalSize3DecorationSlots++;
		}
	}
}

bool MHW::ArmorSet::canDecorationsFit(const int size1, const int size2, const int size3)
{
	int remainingSize3Slots = 0;
	int remainingSize2Slots = 0;

	// First, try to fit size 3 slots
	if (totalSize3DecorationSlots >= size3)
	{
		// can fit!

		// Get remaining slot size 3
		remainingSize3Slots = totalSize3DecorationSlots - size3;
	}
	else
	{
		// Can't fit all size 3 slots
		return false;
	}

	// then check size 2 slots.
	if (totalSize2DecorationSlots >= size2)
	{
		// can fit.
		remainingSize2Slots = totalSize2DecorationSlots - size2;
	}
	else
	{
		// can't fit. Try to use size 3 slots if have any remained.
		const int needMoreSize2Slots = size2 - totalSize2DecorationSlots;

		if (needMoreSize2Slots > remainingSize3Slots)
		{
			// Can't fit.
			return false;
		}
		else
		{
			// can fit! update slot size 3 remainings
			remainingSize3Slots -= needMoreSize2Slots;
			// used all size 2
			remainingSize2Slots = 0;
		}
	}

	// then check size 1 slots
	if (totalSize1DecorationSlots >= size1)
	{
		// can fit.
	}
	else
	{
		// can't fit. Try to use size 3 slot or size 2 slot
		const int needMoreSize1Slots = size1 - totalSize1DecorationSlots;

		const int totalRemainingSlots = remainingSize2Slots + remainingSize3Slots;

		if (needMoreSize1Slots > totalRemainingSlots)
		{
			// can't fit
			return false;
		}
	}

	// success!
	return true;
}

void MHW::ArmorSet::updateUsedDecoCount(const int size1, const int size2, const int size3)
{
	totalUsedSize1DecorationSlot = size1;
	totalUsedSize2DecorationSlot = size2;
	totalUsedSize3DecorationSlot = size3;
}

void MHW::ArmorSet::clearDecoData()
{
	totalSize1DecorationSlots = 0;
	totalSize2DecorationSlots = 0;
	totalSize3DecorationSlots = 0;

	totalUsedSize1DecorationSlot = 0;
	totalUsedSize2DecorationSlot = 0;
	totalUsedSize3DecorationSlot = 0;

	decoSkillLevelSums.clear();

	/*
	std::fill(headArmorDecoSlots.begin(), headArmorDecoSlots.end(), 0);
	std::fill(chestArmorDecoSlots.begin(), chestArmorDecoSlots.end(), 0);
	std::fill(armArmorDecoSlots.begin(), armArmorDecoSlots.end(), 0);
	std::fill(waistArmorDecoSlots.begin(), waistArmorDecoSlots.end(), 0);
	std::fill(legArmorDecoSlots.begin(), legArmorDecoSlots.end(), 0);
	*/

	usedDecorations.clear();
}

bool MHW::ArmorSet::hasEnoughArmorPieces(const int setSkillGroupID, const int reqArmorPieces)
{
	auto find_it = highRankSetSkillArmorPieceSums.find(setSkillGroupID);

	if (find_it != highRankSetSkillArmorPieceSums.end())
	{
		if ((find_it->second) >= reqArmorPieces)
		{
			return true;
		}
	}

	auto find_it_extra = extraHighRankSetSkillArmorPieceSums.find(setSkillGroupID);

	if (find_it_extra != extraHighRankSetSkillArmorPieceSums.end())
	{
		if ((find_it_extra->second) >= reqArmorPieces)
		{
			return true;
		}
	}

	return false;
}

std::wstring MHW::ArmorSet::getItemDecoSizeAsStr(const std::array<int, 3>& decoSlots)
{
	std::wstring decoStr = L"";

	if (decoSlots.at(0) == 0)
	{
		decoStr += L"0";
	}
	else
	{
		decoStr += std::to_wstring(decoSlots.at(0));
	}

	if (decoSlots.at(1) == 0)
	{
		decoStr += L", 0";
	}
	else
	{
		decoStr += (L", " + std::to_wstring(decoSlots.at(1)));
	}

	if (decoSlots.at(2) == 0)
	{
		decoStr += L", 0";
	}
	else
	{
		decoStr += (L", " + std::to_wstring(decoSlots.at(2)));
	}
	
	return decoStr;
}