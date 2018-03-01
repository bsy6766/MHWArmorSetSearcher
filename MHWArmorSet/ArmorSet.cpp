#include "stdafx.h"
#include "ArmorSet.h"
#include "Database.h"

int MHW::ArmorSet::idCounter = 1;

MHW::ArmorSet::ArmorSet()
	: id(-1)
	/*
	, headArmorIndex(-1)
	, chestArmorIndex(-1)
	, armArmorIndex(-1)
	, waistArmorIndex(-1)
	, legArmorIndex(-1)
	, charmIndex(-1)
	*/
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

std::wstring MHW::ArmorSet::toResultStr(Database* db)
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

	std::wstring dbr = L"==================================================\n";
	std::wstring br = L"------------------------------------------------------------------------------------------\n";
	
	std::wstring str = L"Set " + std::to_wstring(id) + L"\n";

	// double br
	str += dbr;

	// add skills
	str += getArmorSkillsStr(db);
	str += getExtraRmorSkillsStr(db);
	str += getDecoSkillsStr(db);
	str += getTotalSkillsStr(db);

	str += br;

	// set skill
	auto setSkillStr = getSetSkillStr(db);

	if (!setSkillStr.empty())
	{
		str += setSkillStr;
	}

	str += br;

	// head 
	if (headArmor)
	{
		str += L"Head: ";

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
			str += (headArmor->name + L" (" + headArmor->setName + L" / " + getItemDecoSizeAsStr(headArmorDecoSlots) + L")\n");
		}
	}
	else
	{
		// error
	}

	// chest
	if (chestArmor)
	{
		str += L"Chest: ";

		str += (chestArmor->name + L" (" + chestArmor->setName + L" / " + getItemDecoSizeAsStr(chestArmorDecoSlots) + L")\n");
	}
	else
	{
		// error
	}

	// arm
	if (armArmor)
	{
		str += L"Arm: ";

		str += (armArmor->name + L" (" + armArmor->setName + L" / " + getItemDecoSizeAsStr(armArmorDecoSlots) + L")\n");
	}
	else
	{
		// error
	}

	// waist
	if (waistArmor)
	{
		str += L"Waist: ";

		str += (waistArmor->name + L" (" + waistArmor->setName + L" / " + getItemDecoSizeAsStr(waistArmorDecoSlots) + L")\n");
	}
	else
	{
		// error
	}

	// leg
	if (legArmor)
	{
		str += L"Leg: ";

		str += (legArmor->name + L" (" + legArmor->setName + L" / " + getItemDecoSizeAsStr(legArmorDecoSlots) + L")\n");
	}
	else
	{
		// error
	}

	// charm
	if (charm == nullptr)
	{
		// Not using charm
		str += L"Charm: None\n";
	}
	else
	{
		if (charm)
		{
			str += L"Charm: ";

			str += charm->name + L" " + std::to_wstring(charm->level) + L"\n";
		}
		else
		{
			// error
		}
	}

	str += br;

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

	str += dbr;

	str += L"\n";

	return str;
}

std::wstring MHW::ArmorSet::getArmorSkillsStr(Database * db)
{
	if (skillLevelSums.empty())
	{
		return L"Armor skill: None\n";
	}
	else
	{
		std::wstring str = L"Armor skill";

		if (skillLevelSums.size() > 1)
		{
			str += L"s";
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

std::wstring MHW::ArmorSet::getExtraRmorSkillsStr(Database * db)
{
	if (extraSkillLevelSums.empty())
	{
		return L"Extra armor skill: None\n";
	}
	else
	{
		std::wstring str = L"Extra armor skill";

		if (extraSkillLevelSums.size() > 1)
		{
			str += L"s";
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

std::wstring MHW::ArmorSet::getDecoSkillsStr(Database * db)
{
	if (decoSkillLevelSums.empty())
	{
		return L"Decoration skill: None\n";
	}
	else
	{
		std::wstring str = L"Decoration skill";

		if (decoSkillLevelSums.size() > 1)
		{
			str += L"s";
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

std::wstring MHW::ArmorSet::getTotalSkillsStr(Database * db)
{
	auto copy = skillLevelSums;

	for (auto& e : decoSkillLevelSums)
	{
		copy[e.first] += e.second;
	}

	if (copy.size() != skillLevelSums.size())
	{
		// error
		OutputDebugString(L"Added some wrong skills\n");
	}

	if (copy.empty())
	{
		return L"Total skill: None\n";
	}
	else
	{
		std::wstring str = L"Total skill";

		if (copy.size() > 1)
		{
			str += L"s";
		}

		str += L": ";

		auto iter = copy.begin();
		for (; iter != copy.end();)
		{
			Skill* skill = db->getSkillByIDAndLevel(iter->first, 1);

			if (skill)
			{
				//str += (db->getSkillNameById(iter->first) + L" " + std::to_wstring(iter->second));
				str += (skill->name + L" " + std::to_wstring(iter->second) + L" (" + std::to_wstring(skill->maxLevel) + L")");

				iter++;

				if (iter != copy.end())
				{
					str += L", ";
				}
			}
			else
			{
				// error
				OutputDebugString(L"Can't find skill data\n");
			}
		}

		str += L"\n";

		return str;
	}
}

std::wstring MHW::ArmorSet::getSetSkillStr(Database * db)
{
	std::wstring str = L"";

	if (activatedHighRankSetSkills.empty())
	{
		str = L"Set skill: None\n";
	}
	else
	{
		str = L"Set skill";

		if (activatedHighRankSetSkills.size() > 1)
		{
			str += L"s: ";
		}
		else
		{
			str += L": ";
		}

		const int size = activatedHighRankSetSkills.size();

		for (int i = 0; i < size; i++)
		{
			auto setSkill = db->getSetSkillByID(activatedHighRankSetSkills.at(i), true);

			if (setSkill)
			{
				str += (setSkill->name + L" " + std::to_wstring(setSkill->reqArmorPieces));
			}
			else
			{
				OutputDebugString(L"Failed to get set skill data\n");
			}

			if (i < (size - 1))
			{
				str += L", ";
			}
		}

		str += L"\n";
	}

	return str;
}

std::wstring MHW::ArmorSet::getExtraSetSkillStr(Database * db)
{
	return std::wstring();
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

void MHW::ArmorSet::countSums(Database* db)
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
		// error
		OutputDebugString(L"Error: Can't find head armor data.\n");
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
				// Error. skill and skill level size is different.
				OutputDebugString(L"Error: skill and skill level size is different\n");
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

void MHW::ArmorSet::addCharmSkillLevelSums(Database * db)
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
			// Error: failed to get charm data.
			return;
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
		// Error: failed to get charm data.
		return;
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

/*
void MHW::ArmorSet::clearSetSkillReqArmorPiecesSums()
{
	lowRankSetSkillArmorPieceSums.clear();
	highRankSetSkillArmorPieceSums.clear();
}

void MHW::ArmorSet::initSetSkillLevelSums(const std::vector<int>& lowRankSetSkills, const std::vector<int>& highRankSetSkills)
{
	for (auto lrSetSkillId : lowRankSetSkills)
	{
		lowRankSetSkillArmorPieceSums[lrSetSkillId] = 0;
	}

	for (auto hrSetSkillId : highRankSetSkills)
	{
		highRankSetSkillArmorPieceSums[hrSetSkillId] = 0;
	}
}

void MHW::ArmorSet::countSetSkillReqArmorPiecesSums(Database * db)
{
	// head
	if (headArmorIndex != -1)
	{
		// has head armor. Find head
		auto find_head = db->headArmors.find(headArmorIndex);
		if (find_head == db->headArmors.end())
		{
			// error
			OutputDebugString(L"Error: Can't find head armor data.\n");
		}
		else
		{
			int i = 0;
			int len = (find_head->second).skills.size();
			// iterate head skills
			for (; i < len; ++i)
			{
				int skillID = (find_head->second).skills.at(i);
				int skillLevel = (find_head->second).skillLevels.at(i);
				addSkillLevelToSum(skillID, skillLevel);
				//addSkillLevelToSkillSums(skillLevelSums, extraSkillLevelSums, skillID, skillLevel);
			}
		}
	}
}
*/

/*
void MHW::ArmorSet::setHeadrmor(Database * db, const int headArmorIndex)
{
	this->headArmorIndex = headArmorIndex;

	if (headArmorIndex != -1)
	{
		headArmor = db->getHeadArmorByID(headArmorIndex);

		if (headArmor)
		{
			copyDecoSlots(headArmor->decorationSlots, headArmorDecoSlots);
		}
		else
		{
			// error
			OutputDebugString(L"armor doesn't exists\n");
		}
	}
	else
	{
		headArmor = nullptr;
	}
}

void MHW::ArmorSet::setChestArmor(Database * db, const int chestArmorIndex)
{
	this->chestArmorIndex = chestArmorIndex;

	if (chestArmorIndex != -1)
	{
		chestArmor = db->getChestArmorByID(chestArmorIndex);

		if (chestArmor)
		{
			copyDecoSlots(chestArmor->decorationSlots, chestArmorDecoSlots);
		}
		else
		{
			// error
			OutputDebugString(L"armor doesn't exists\n");
		}
	}
	else
	{
		chestArmor = nullptr;
	}
}

void MHW::ArmorSet::setArmArmor(Database * db, const int armArmorIndex)
{
	this->armArmorIndex = armArmorIndex;

	if (armArmorIndex != -1)
	{
		armArmor = db->getArmArmorByID(armArmorIndex);

		if (armArmor)
		{
			copyDecoSlots(armArmor->decorationSlots, armArmorDecoSlots);
		}
		else
		{
			// error
			OutputDebugString(L"armor doesn't exists\n");
		}
	}
	else
	{
		armArmor = nullptr;
	}
}

void MHW::ArmorSet::setWaistArmor(Database * db, const int waistArmorIndex)
{
	this->waistArmorIndex = waistArmorIndex;

	if (waistArmorIndex != -1)
	{
		waistArmor = db->getWaistArmorByID(waistArmorIndex);

		if (waistArmor)
		{
			copyDecoSlots(waistArmor->decorationSlots, waistArmorDecoSlots);
		}
		else
		{
			// error
			OutputDebugString(L"armor doesn't exists\n");
		}
	}
	else
	{
		waistArmor = nullptr;
	}
}

void MHW::ArmorSet::setLegArmor(Database * db, const int legArmorIndex)
{
	this->legArmorIndex = legArmorIndex;

	if (legArmorIndex != -1)
	{
		legArmor = db->getLegArmorByID(legArmorIndex);

		if (legArmor)
		{
			copyDecoSlots(legArmor->decorationSlots, legArmorDecoSlots);
		}
		else
		{
			// error
			OutputDebugString(L"armor doesn't exists\n");
		}
	}
	else
	{
		legArmor = nullptr;
	}
}
*/

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
		OutputDebugString(L"armor doesn't exists\n");
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
		OutputDebugString(L"armor doesn't exists\n");
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
		OutputDebugString(L"armor doesn't exists\n");
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
		OutputDebugString(L"armor doesn't exists\n");
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
		OutputDebugString(L"armor doesn't exists\n");
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

	usedDecorations.clear();
	*/
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

/*
void MHW::ArmorSet::setArmor(Armor * armor)
{
	if (armor)
	{
		// Update skill sum

		// Update set skill armor piece sum

		// Update total deco losts
		for (auto decoSize : armor->decorationSlots)
		{
			addDecoSlot(decoSize);
		}
	}
	else
	{
		// Error
		OutputDebugString(L"Armor data doesn't exists\n");
	}
}

void MHW::ArmorSet::addDecoSlot(const int decoSize)
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
	else
	{
#if _DEBUG
		// error. Deco size is wrong
		OutputDebugString(L"Decoration size is wrong.\n");
#endif
	}
}

void MHW::ArmorSet::removeDecoSlot(const int decoSize)
{
	if (decoSize == 1)
	{
		totalSize1DecorationSlots--;

		if (totalSize1DecorationSlots < 0)
		{
			// fail safe
#if _DEBUG
			OutputDebugString(L"Total size 1 decoration slots are below 0");
#endif
			totalSize1DecorationSlots = 0;
		}
	}
	else if (decoSize == 2)
	{
		totalSize2DecorationSlots--;

		if (totalSize2DecorationSlots < 0)
		{
			// fail safe
#if _DEBUG
			OutputDebugString(L"Total size 2 decoration slots are below 0");
#endif
			totalSize2DecorationSlots = 0;
		}
	}
	else if (decoSize == 3)
	{
		totalSize3DecorationSlots--;

		if (totalSize3DecorationSlots < 0)
		{
			// fail safe
#if _DEBUG
			OutputDebugString(L"Total size 3 decoration slots are below 0");
#endif
			totalSize3DecorationSlots = 0;
	}
	}
	else
	{
#if _DEBUG
		// error. Deco size is wrong
		OutputDebugString(L"Decoration size is wrong.\n");
#endif
	}
}

*/
