#include "stdafx.h"
#include "Database.h"
#include "Settings.h"
#include "Utility.h"
#include "Logger.h"
#include <assert.h>

using namespace Utility;

Database::Database()
	: totalLowRankHeadArmors(0)
	, totalHighRankHeadArmors(0)
	, totalLowRankChestArmors(0)
	, totalHighRankChestArmors(0)
	, totalLowRankArmArmors(0)
	, totalHighRankArmArmors(0)
	, totalLowRankWaistArmors(0)
	, totalHighRankWaistArmors(0)
	, totalLowRankLegArmors(0)
	, totalHighRankLegArmors(0)
	, bulwarkCharm(nullptr)
	, guardUpSkill(nullptr)
	, uragaanProtectionSetSkill(nullptr)
{}

int Database::init(Settings * setting)
{
	// Need to init skill data first.
	int result = 0;

	result = initSkillData(setting);
	if (result != 0) return result;

	result = initSetSkillData(setting);
	if (result != 0) return result;

	initDecoSetSkillData();

	result = initCharmData(setting);
	if (result != 0) return result;

	if ((guardUpSkill == nullptr) || (bulwarkCharm == nullptr) || (uragaanProtectionSetSkill == nullptr))
	{
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_INITIALIZE_GUARD_UP_PTR);
	}

	result = initArmorData(setting);
	if (result != 0) return result;

 	result = initDecorationData(setting);
	if (result != 0) return result;

	return 0;
}

int Database::initCharmData(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream charmDataFile("Data/" + setting->getLanguagePath() + "/CharmData");

	// Check if it's valid
	if (charmDataFile.is_open())
	{
		std::string line;

		// ID for each charms
		int idCounter = 0;		
		// 0 is None in dropdown list. 1 is any. Charm id starts from 2.
		int indexCounter = 2;	

		// iterate by each line
		while (std::getline(charmDataFile, line))
		{
			// check line
			if (!line.empty())
			{
				// not empty
#if _DEBUG
				// Debug output
				std::string str = "Processing charm data: " + line + "\n";
				OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
#endif

				// split line by comma
				std::vector<std::string> split = Utility::splitByComma(line);

				auto size = split.size();

				// Charm data has 3 or 4 entry on each line. 4 means that charm has 2 skills and all of these charms have max level of 1.
				if (size == 3 || size == 4)
				{
					int maxLv = 0;

					try
					{
						if (size == 3)
						{
							// get max level
							maxLv = std::stoi(split.at(2));
						}
						else
						{
							// Always 1.
							maxLv = std::stoi(split.at(3));
						}
					}
					catch (...)
					{
						// Charm's max level isn't number
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_CHARM_MAX_LEVEL_IS_NOT_NUM);
					}

					// Add charm item for each level
					for (int i = 0; i < maxLv; ++i)
					{
						Charm& curCharm = charms[indexCounter];
						curCharm.id = idCounter;
						curCharm.dropdownIndex = indexCounter;
						curCharm.level = i + 1;
						curCharm.maxLevel = maxLv;
						curCharm.name = Utility::utf8_decode(split.at(0));

						curCharm.skillName = Utility::utf8_decode(split.at(1));

						if (curCharm.name == setting->getString(MHW::StringLiteral::BULWARK_CHARM))
						{
							curCharm.setSkill = true;
							
							bulwarkCharm = &curCharm;
						}

						// get skill id
						auto find_it = skillNameToIdLUT.find(curCharm.skillName);
						if (find_it == skillNameToIdLUT.end())
						{
							// Skill name doens;t exists on lut
							return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_CHARMS_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT);
						}
						else
						{
							curCharm.skillId = find_it->second;
						}

						if (size == 4)
						{
							// has second skill
							// Note: For patch 1.06, second set skill doesn't exists on charm.

							curCharm.secondSkillName = utf8_decode(std::string(split.at(2).begin(), split.at(2).end()).c_str());

							// get skill id
							auto find_it = skillNameToIdLUT.find(curCharm.secondSkillName);
							if (find_it == skillNameToIdLUT.end())
							{
								// second skill name doesn't exists on lut
								return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_CHARMS_SECOND_SKILL_NAME_DOES_NOT_EXISTS_IN_LUT);
							}
							else
							{
								curCharm.secondSkillId = find_it->second;
							}
						}

						indexCounter++;
					}
				}
				else
				{
					// size of split is incorrect
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_CHARM_DATA_INCORRECT_SPLIT_SIZE);
				}
			}

			idCounter++;
		}
	}
	else
	{
		// Failed to read charm data file
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_CHARM_DATA_FILE);
	}

#if _DEBUG
	for (auto& e : charms)
	{
		(e.second).print(true);
	}
#endif

	return 0;
}

int Database::initSkillData(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream skillData("Data/" + setting->getLanguagePath() + "/SkillData");

	// Check if data is valid
	if (skillData.is_open())
	{
		std::string line;

		// ID for each charms
		int idCounter = 0;		
		
		// 0 is None in dropdown list.
		int indexCounter = 0;	

		// get each line
		while (std::getline(skillData, line))
		{
			// check str line
			if (!line.empty())
			{
#if _DEBUG
				// Debug
				auto wLine = utf8_decode(line);
				std::string str = "Processing skill data: " + line + "\n";
				OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
#endif

				// Split line by commna
				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				// All skill data has 4 entry per line. Name, max level, decoration flag and set skill flag
				if (size == 4)
				{
					int maxLv = 0;

					try
					{
						// get max level
						maxLv = std::stoi(split.at(1));
					}
					catch (...)
					{
						// Skill's max level isnot number
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_SKILL_MAX_LEVEL_IS_NOT_NUM);
					}

					bool onlyFromDeco = false;

					try
					{
						onlyFromDeco = (std::stoi(split.at(2)) == 1);
					}
					catch (...)
					{
						// Only from deco is not number
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_SKILL_ONLY_FROM_DECO_IS_NOT_NUM);
					}

					bool fromDecoSetSkill = false;

					try
					{
						fromDecoSetSkill = (std::stoi(split.at(3)) == 1);
					}
					catch (...)
					{
						// from deco set skill is not number
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_SKILL_DECO_SET_SKILL_IS_NOT_NUM);
					}

					// Add skill item for each level
					auto skillName = utf8_decode(split.at(0));

					bool cornerCase = false;

					// corner case
					if (skillName == setting->getString(MHW::StringLiteral::GUARD_UP))
					{
						cornerCase = true;
					}

					// Get skill description
					std::getline(skillData, line);
					std::wstring skillDescription = utf8_decode(line);

					skillIdToKeyLUT[idCounter] = indexCounter;

					for (int i = 0; i < maxLv; ++i)
					{
						Skill& curSkill = skills[indexCounter];
						curSkill.id = idCounter;

						if (cornerCase)
						{
							guardUpSkill = &curSkill;
						}

						curSkill.originalIndex = indexCounter;
						curSkill.dropdownIndex = indexCounter;
						skillDropdownListToKey[indexCounter] = indexCounter;

						curSkill.level = i + 1;
						curSkill.maxLevel = maxLv;
						curSkill.name = skillName;
						curSkill.onlyFromDeco = onlyFromDeco;
						curSkill.fromDecoSetSkill = fromDecoSetSkill;

						curSkill.description = skillDescription;

						// Get level description
						std::getline(skillData, line);
						std::wstring levelDescription = utf8_decode(line);

						/*
						auto pos = levelDescription.find_first_of(L'_');

						if (pos != std::string::npos)
						{
							levelDescription.replace(pos, 1, L"\n");
						}
						*/

						curSkill.levelDescription = levelDescription;

						indexCounter++;
					}

					skillNameToIdLUT[skillName] = idCounter;
					skillIdToNameLUT[idCounter] = skillName;
				}
				else
				{
					// split size is incorrect
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_SKILL_DATA_INCORRECT_SPLIT_SIZE);
				}
			}

			idCounter++;
		}
	}
	else
	{
		// Failed to read skill data file
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_SKILL_DATA_FILE);
	}

#if _DEBUG
	for (auto& e : skills)
	{
		(e.second).print(true);
	}
#endif

	return 0;
}

int Database::initSetSkillData(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream setSkillData("Data/" + setting->getLanguagePath() + "/SetSkillData");

	if (setSkillData.is_open())
	{
		int result = initLowRankSetSkillData(setSkillData);

		if (result != 0)
		{
			return result;
		}

		result = initHighRankSetSkillData(setSkillData, setting);

		if (result != 0)
		{
			return result;
		}
	}
	else
	{
		// failed to read set skill data
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_SET_SKILL_DATA_FILE);
	}

#if _DEBUG
	for (auto& e : lowRankSetSkills)
	{
		(e.second).print();
	}

	for (auto& e : highRankSetSkills)
	{
		(e.second).print();
	}
#endif

	return 0;
}

void Database::initDecoSetSkillData()
{
	for (auto& skill : skills)
	{
		if ((skill.second).fromDecoSetSkill)
		{
			for (auto& setSkill : highRankSetSkills)
			{
				if (setSkill.second.skillName == skill.second.name)
				{
					decoSetSkillIDToSetSkillIDLUT[skill.second.id] = setSkill.second.id;
					setSkillIDToDecoSetSkillIDLUT[setSkill.second.id] = skill.second.id;

					decoSetSkillIDToSetSkillGroupIDLUT[skill.second.id] = setSkill.second.groupId;
					setSkillGroupIdToDecoSetSkillIDLUT[setSkill.second.groupId] = skill.second.id;
				}
			}
		}
	}
}

int Database::initLowRankSetSkillData(std::ifstream & setSkillData)
{
	std::string line;

	std::getline(setSkillData, line);

	int lowRankSetSkillCount = 0;

	try
	{
		lowRankSetSkillCount = std::stoi(line);
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_TOTAL_LOW_RANK_SET_SKILL_COUNT_IS_NOT_NUM);
	}

	int idCounter = 0;		// ID for each charms
	int groupIdCounter = 0;
	int indexCounter = 0;	// 0 is None in dropdown list.

	for (int i = 0; i < lowRankSetSkillCount; ++i)
	{
		std::getline(setSkillData, line);

		// check if line is empty
		if (!line.empty())
		{
			std::vector<std::string> split = splitByComma(line);

			auto size = split.size();

			// Low rank doesn't have multiple set skills
			if (size == 3)
			{
				SetSkill& curSetSkill = lowRankSetSkills[indexCounter];

				curSetSkill.name = utf8_decode(split.at(0));
				curSetSkill.skillName = utf8_decode(split.at(1));
				curSetSkill.id = idCounter;
				curSetSkill.groupId = groupIdCounter;
				curSetSkill.hasSibling = false;

				curSetSkill.originalIndex = indexCounter;
				curSetSkill.dropdownIndex = indexCounter;

				lowRankSetSkillDropdownListToKey[indexCounter] = indexCounter;

				setSkillGroupNameToIdLUT[curSetSkill.name] = indexCounter;

				curSetSkill.highRank = false;

				int reqArmorPiece = 0;

				try
				{
					reqArmorPiece = std::stoi(split.at(2));
				}
				catch (...)
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_LOW_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM);
				}

				curSetSkill.reqArmorPieces = reqArmorPiece;
				curSetSkill.applied = false;

				// get description
				std::getline(setSkillData, line);
				curSetSkill.description = utf8_decode(line);

				std::wstring setSkillName = curSetSkill.name + L" " + utf8_decode(split.at(2));
#if _DEBUG
				auto find_it = setSkillNameToIdLUT.find(setSkillName);
				if (find_it != setSkillNameToIdLUT.end())
				{
					assert(false);
				}
#endif
				setSkillNameToIdLUT[setSkillName] = idCounter;
				setSkillIdToNameLUT[idCounter] = setSkillName;

				idCounter++;
				groupIdCounter++;
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_LOW_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE);
			}
		}
		// else, line is empty.

		indexCounter++;
	}

	return 0;
}

int Database::initHighRankSetSkillData(std::ifstream& setSkillData, Settings* setting)
{
	std::string line;

	std::getline(setSkillData, line);

	int highRankSetSkillCount = 0;

	try
	{
		highRankSetSkillCount = std::stoi(line);
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_TOTAL_HIGH_RANK_SET_SKILL_COUNT_IS_NOT_NUM);
	}

	int idCounter = 0;
	int indexCounter = 0;	// 0 is None in dropdown list.
	int groupIdCounter = 0;

	// get each line
	while (std::getline(setSkillData, line))
	{
		// check if line is empty
		if (!line.empty())
		{
#if _DEBUG
			auto wLine = utf8_decode(line);
			std::string str = "Processing set skill data: " + line + "\n";
			OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
#endif
			std::vector<std::string> split = splitByComma(line);

			auto size = split.size();

			// Splitted strings must be either 3 or 5
			if (size == 3 || size == 5)
			{
				SetSkill& curSetSkill = highRankSetSkills[indexCounter];

				curSetSkill.name = utf8_decode(split.at(0));
				curSetSkill.skillName = utf8_decode(split.at(1));

				curSetSkill.id = idCounter;
				curSetSkill.groupId = groupIdCounter;

				curSetSkill.originalIndex = indexCounter;
				curSetSkill.dropdownIndex = indexCounter;
				highRankSetSkillDropdownListToKey[indexCounter] = indexCounter;

				setSkillGroupNameToIdLUT[curSetSkill.name] = indexCounter;

				curSetSkill.highRank = true;

				// corner case
				if (curSetSkill.name == setting->getString(MHW::StringLiteral::URAGAAN_PROTECTION))
				{
					uragaanProtectionSetSkill = &curSetSkill;
				}

				int reqArmorPiece = 0;

				try
				{
					reqArmorPiece = std::stoi(split.at(2));
				}
				catch (...)
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM);
				}

				curSetSkill.reqArmorPieces = reqArmorPiece;
				curSetSkill.applied = false;

				// get description
				std::getline(setSkillData, line);
				curSetSkill.description = utf8_decode(line);

				std::wstring setSkillName = curSetSkill.name + L" " + utf8_decode(split.at(2));
#if _DEBUG
				auto find_it = setSkillNameToIdLUT.find(setSkillName);
				if (find_it != setSkillNameToIdLUT.end())
				{
					assert(false);
				}
#endif
				setSkillNameToIdLUT[setSkillName] = idCounter;
				setSkillIdToNameLUT[idCounter] = setSkillName;

				indexCounter++;
				idCounter++;

				if (size == 5)
				{
					secondSetSkillCheckLUT[idCounter - 1] = true;

					// add second skill set
					SetSkill& secondSetSkill = highRankSetSkills[indexCounter];
					secondSetSkill.name = utf8_decode(split.at(0));
					secondSetSkill.skillName = utf8_decode(split.at(3));
					secondSetSkill.id = idCounter;
					secondSetSkill.originalIndex = indexCounter;
					secondSetSkill.dropdownIndex = indexCounter;
					secondSetSkill.highRank = true;
					secondSetSkill.groupId = groupIdCounter;

					secondSetSkill.hasSibling = true;
					curSetSkill.hasSibling = true;

					secondSetSkill.sibling = &curSetSkill;
					curSetSkill.sibling = &secondSetSkill;

					int reqArmorPiece = 0;

					try
					{
						reqArmorPiece = std::stoi(split.at(4));
					}
					catch (...)
					{
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_SECOND_HIGH_RANK_REQ_ARMOR_PIECES_IS_NOT_NUM);
					}

					secondSetSkill.reqArmorPieces = reqArmorPiece;
					secondSetSkill.applied = false;

					// get description
					std::getline(setSkillData, line);
					secondSetSkill.description = utf8_decode(line);

					std::wstring setSkillName = secondSetSkill.name + L" " + utf8_decode(split.at(4));
#if _DEBUG
					auto find_it = setSkillNameToIdLUT.find(setSkillName);
					if (find_it != setSkillNameToIdLUT.end())
					{
						assert(false);
					}
#endif
					setSkillNameToIdLUT[setSkillName] = idCounter;
					setSkillIdToNameLUT[idCounter] = setSkillName;

					indexCounter++;
					idCounter++;
				}
				else
				{
					curSetSkill.hasSibling = false;
				}

				groupIdCounter++;
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_HIGH_RANK_SET_SKILL_DATA_INCORRECT_SPLIT_SIZE);
			}
		}
	}

	return 0;
}

int Database::initArmorData(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream armorDataFile("Data/" + setting->getLanguagePath() + "/ArmorData");

	// Check if it's valid
	if (armorDataFile.is_open())
	{
		std::string line;

		int setArmorIdCounter = 0;
		int headArmorIdCounter = 0;
		int chestArmorIdCounter = 0;
		int armArmorIdCounter = 0;
		int waistArmorIdCounter = 0;
		int legArmorIdCounter = 0;

		int anyHeadArmorIdCounter = 2;
		int anyChestArmorIdCounter = 2;
		int anyArmArmorIdCounter = 2;
		int anyWaistArmorIdCounter = 2;
		int anyLegArmorIdCounter = 2;

		initDefaultAnyArmors();

		// get first line
		std::getline(armorDataFile, line);

		std::vector<std::string> firstSplit = splitByComma(line);

		auto firstSize = firstSplit.size();

		// Defense of armor
		int defense = 0;
		// Rarity of armor
		int rarity = 0;
		// Armor set name (i.e. Leather, Chainmail, etc)
		std::string setName = "";
		// rank flag
		bool highRank = false;
		// Group name of set skill if have one
		std::string setSkillGroupName = "";
		// Set skill id if have one (i.e. Odogaron Master - Punishing draw)
		int setSkillId = -1;
		// Second set skill id have one(i.e. Odoragon Mastery - Protective Polish)
		int secondSetSkillId = -1;
		// Set skill group id if have one
		int setGroupId = -1;
		// Counter for type (head, chest, etc)
		int typeCounter = 0;
		// gender flag.
		bool hasGender = false;
		// gender.
		MHW::Gender gender = MHW::Gender::NONE;

		auto& logger = MHW::Logger::getInstance();

		// Each first line for each armor sets have 3 or 4 entries per line, but first one has only 4
		// Armor set name,rarity,armor,gender
		if (firstSize == 4)
		{
			// Set name, rarity, armor
			setName = firstSplit.at(0);

			try
			{
				rarity = std::stoi(firstSplit.at(1));
				defense = std::stoi(firstSplit.at(2));
				int genderCode = std::stoi(firstSplit.at(3));

				if (genderCode == 0)
				{
					hasGender = false;

					gender = MHW::Gender::NONE;
				}
				else
				{
					hasGender = true;
					if (genderCode == 1)
					{
						gender = MHW::Gender::FEMALE;
					}
					else if (genderCode == 2)
					{
						gender = MHW::Gender::MALE;
					}
					else
					{
						logger.error("Gender code is wrong: " + std::to_string(genderCode));
					}
				}
			}
			catch (...)
			{
				return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_ARE_NOT_NUM);
			}
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_INCORRECT_SPLIT_SIZE);
		}

		// iterate by each line
		while (std::getline(armorDataFile, line))
		{
			// check line
			if (line.empty())
			{
				// current set armor finished
				setArmorIdCounter++;

				// reset type counter
				typeCounter = 0;

				// get line
				std::getline(armorDataFile, line);

				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				// 
				if (size >= 4 && size <= 5)
				{
					// Set name, rarity, armor
					setName = split.at(0);

					try
					{
						rarity = std::stoi(split.at(1));
						defense = std::stoi(split.at(2));
						int genderCode = std::stoi(split.at(3));

						if (genderCode == 0)
						{
							hasGender = false;

							gender = MHW::Gender::NONE;
						}
						else
						{
							hasGender = true;
							if (genderCode == 1)
							{
								gender = MHW::Gender::FEMALE;
							}
							else if (genderCode == 2)
							{
								gender = MHW::Gender::MALE;
							}
							else
							{
								logger.error("Gender code is wrong: " + std::to_string(genderCode));
							}
						}
					}
					catch (...)
					{
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_ARE_NOT_NUM);
					}

					if (size == 5)
					{
						// this armor has set skill
						setSkillGroupName = split.at(4);

						auto find_setSkillId = setSkillGroupNameToIdLUT.find(utf8_decode(setSkillGroupName));
						if (find_setSkillId == setSkillGroupNameToIdLUT.end())
						{
							setSkillId = -1;
							secondSetSkillId = -1;
							setGroupId = -1;
						}
						else
						{
							setSkillId = find_setSkillId->second;
							
							auto find_setSkill = highRankSetSkills.find(setSkillId);
							if (find_setSkill == highRankSetSkills.end())
							{
								return static_cast<int>(MHW::ERROR_CODE::ARMOR_DATA_CANT_FIND_SET_SKILL_BY_ID);
							}
							else
							{
								setGroupId = (find_setSkill->second).groupId;
							}

							auto find_second = secondSetSkillCheckLUT.find(setSkillId);
							if (find_second == secondSetSkillCheckLUT.end())
							{
								secondSetSkillId = -1;
							}
							else
							{
								secondSetSkillId = setSkillId + 1;
							}
						}
					}
					else
					{
						setSkillGroupName.clear();
						setSkillId = -1;
					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_INCORRECT_SPLIT_SIZE);
				}
			}
			else
			{
				// not empty
#if _DEBUG
				// Debug output
				std::string str = "Processing armor data: " + line + "\n";
				OutputDebugString(utf8_decode(std::string(str.begin(), str.end())).c_str());
#endif

				// split line by comma
				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				if (line.size() == 1 && line == "-")
				{
					// Armor doesn't exists.
					typeCounter++;
					continue;
				}
				else
				{
					// armor exists
					switch (typeCounter)
					{
					case 0:
					{
						// Head
						Armor& head = headArmors[headArmorIdCounter];

						int result = initArmor(head, split, defense, rarity, headArmorIdCounter, setArmorIdCounter, setSkillId, secondSetSkillId, setGroupId, setName, gender);

						if (result != 0) return result;

						if (head.rarity >= 5)
						{
							head.highRank = true;
							totalHighRankHeadArmors++;
						}
						else
						{
							head.highRank = false;
							totalLowRankHeadArmors++;
						}

						// inc head id counter
						headArmorIdCounter++;

						if (!head.decorationSlots.empty())
						{
							// has decorations
							if (!hasSameTypeOfAnyArmor(head, anyHeadArmors))
							{
								// Doesn't have same type of any armor
								Armor& anyHead = anyHeadArmors[ANY_ARMOR_ID_START + anyHeadArmorIdCounter];

								anyHead.id = (ANY_ARMOR_ID_START + anyHeadArmorIdCounter);
								anyHead.highRank = head.highRank;
								anyHead.decorationSlots = head.decorationSlots;

								anyHeadArmorIdCounter++;
							}
						}
					}
					break;
					case 1:
					{
						// Chest
						Armor& chest = chestArmors[chestArmorIdCounter];

						int result = initArmor(chest, split, defense, rarity, chestArmorIdCounter, setArmorIdCounter, setSkillId, secondSetSkillId, setGroupId, setName, gender);

						if (result != 0) return result;

						if (chest.rarity >= 5)
						{
							chest.highRank = true;
							totalHighRankChestArmors++;
						}
						else
						{
							chest.highRank = false;
							totalLowRankChestArmors++;
						}

						// inc chest id counter
						chestArmorIdCounter++;

						if (!chest.decorationSlots.empty())
						{
							// has decorations
							if (!hasSameTypeOfAnyArmor(chest, anyChestArmors))
							{
								// Doesn't have same type of any armor
								Armor& anyChest = anyChestArmors[ANY_ARMOR_ID_START + anyChestArmorIdCounter];

								anyChest.id = (ANY_ARMOR_ID_START + anyChestArmorIdCounter);
								anyChest.highRank = chest.highRank;
								anyChest.decorationSlots = chest.decorationSlots;

								anyChestArmorIdCounter++;
							}
						}
					}
					break;
					case 2:
					{
						// Arm
						Armor& arm = armArmors[armArmorIdCounter];

						initArmor(arm, split, defense, rarity, armArmorIdCounter, setArmorIdCounter, setSkillId, secondSetSkillId, setGroupId, setName, gender);

						if (arm.rarity >= 5)
						{
							arm.highRank = true;
							totalHighRankArmArmors++;
						}
						else
						{
							arm.highRank = false;
							totalLowRankArmArmors++;
						}

						// inc chest id counter
						armArmorIdCounter++;

						if (!arm.decorationSlots.empty())
						{
							// has decorations
							if (!hasSameTypeOfAnyArmor(arm, anyArmArmors))
							{
								// Doesn't have same type of any armor
								Armor& anyArm = anyArmArmors[ANY_ARMOR_ID_START + anyArmArmorIdCounter];

								anyArm.id = (ANY_ARMOR_ID_START + anyArmArmorIdCounter);
								anyArm.highRank = arm.highRank;
								anyArm.decorationSlots = arm.decorationSlots;

								anyArmArmorIdCounter++;
							}
						}
					}
					break;
					case 3:
					{
						// Waist
						Armor& waist = waistArmors[waistArmorIdCounter];

						initArmor(waist, split, defense, rarity, waistArmorIdCounter, setArmorIdCounter, setSkillId, secondSetSkillId, setGroupId, setName, gender);

						if (waist.rarity >= 5)
						{
							waist.highRank = true;
							totalHighRankWaistArmors++;
						}
						else
						{
							waist.highRank = false;
							totalLowRankWaistArmors++;
						}

						// inc chest id counter
						waistArmorIdCounter++;

						if (!waist.decorationSlots.empty())
						{
							// has decorations
							if (!hasSameTypeOfAnyArmor(waist, anyWaistArmors))
							{
								// Doesn't have same type of any armor
								Armor& anyWaist = anyWaistArmors[ANY_ARMOR_ID_START + anyWaistArmorIdCounter];

								anyWaist.id = (ANY_ARMOR_ID_START + anyWaistArmorIdCounter);
								anyWaist.highRank = waist.highRank;
								anyWaist.decorationSlots = waist.decorationSlots;

								anyWaistArmorIdCounter++;
							}
						}
					}
					break;
					case 4:
					{
						// Leg
						Armor& leg = legArmors[legArmorIdCounter];

						initArmor(leg, split, defense, rarity, legArmorIdCounter, setArmorIdCounter, setSkillId, secondSetSkillId, setGroupId, setName, gender);

						if (leg.rarity >= 5)
						{
							leg.highRank = true;
							totalHighRankLegArmors++;
						}
						else
						{
							leg.highRank = false;
							totalLowRankLegArmors++;
						}

						// inc chest id counter
						legArmorIdCounter++;

						if (!leg.decorationSlots.empty())
						{
							// has decorations
							if (!hasSameTypeOfAnyArmor(leg, anyLegArmors))
							{
								// Doesn't have same type of any armor
								Armor& anyLeg = anyLegArmors[ANY_ARMOR_ID_START + anyLegArmorIdCounter];

								anyLeg.id = (ANY_ARMOR_ID_START + anyLegArmorIdCounter);
								anyLeg.highRank = leg.highRank;
								anyLeg.decorationSlots = leg.decorationSlots;

								anyLegArmorIdCounter++;
							}
						}
					}
					break;
					default:
						break;
					}

					typeCounter++;
				}
			}
		}
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_ARMOR_DATA_FILE);
	}

	return 0;
}

int Database::initArmor(Armor & armor, std::vector<std::string>& split, const int defense, const int rarity, const int id, const int setId, const int setSkiiId, const int secondSetSkillId, const int setGroupId, const std::string& setName, const MHW::Gender gender)
{
	armor.name = utf8_decode(split.at(0));			// Name of armor (leather head, etc...)
	armor.setId = setId;							// unique set id for each armor set(leather, chainmail, etc...)
	armor.id = id;									// unique id for each type(head, chest, ...) armor
	armor.setName = utf8_decode(setName);			// Set name of armor (leather a, leather b, etc..)
	armor.defense = defense;						// base defense state
	armor.rarity = rarity;							// rarity
	armor.setSkillId = setSkiiId;					// set skill id.
	armor.secondSetSkillId = secondSetSkillId;
	armor.setGroupId = setGroupId;
	armor.gender = gender;

	int skillCount = 0;
	int result = initArmorSkillData(armor, split, skillCount);

	if (result != 0) return result;

	const int decorationCountIndex = (skillCount * 2) + 2;

	result = initArmorDecorationData(armor, decorationCountIndex, split);

	if (result != 0) return result;

	return 0;
}

int Database::initArmorSkillData(Armor & armor, std::vector<std::string>& split, int& skillCount)
{
	try
	{
		skillCount = std::stoi(split.at(1));
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_SKILL_COUNT_IS_NOT_NUM);
	}

	if (skillCount > 0)
	{
		// has skill.
		for (int i = 1; i <= skillCount; ++i)
		{
			auto skillName = utf8_decode(split.at(2 * i));
			auto find_it = skillNameToIdLUT.find(skillName);
			if (find_it == skillNameToIdLUT.end())
			{
				return static_cast<int>(MHW::ERROR_CODE::ARMOR_DATA_CANT_FIND_SKILL_ID_BY_NAME);
			}
			else
			{
				int skillLevel = 0;

				try
				{
					skillLevel = std::stoi(split.at((2 * i) + 1));
				}
				catch (...)
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_SKILL_LEVEL_IS_NOT_NUM);
				}

				// skill id
				armor.skills.push_back(find_it->second);
				// skill level
				armor.skillLevels.push_back(skillLevel);
			}
		}
	}

	return 0;
}

int Database::initArmorDecorationData(Armor & armor, const int decoCountIndex, std::vector<std::string>& split)
{
	int decorationCount = 0;

	try
	{
		decorationCount = std::stoi(split.at(decoCountIndex));
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_DECO_COUNT_IS_NOT_NUM);
	}

	if (decorationCount == 0)
	{
		// no decoration
		armor.decorationSlots.clear();
	}
	else
	{
		for (int i = 0; i < decorationCount; ++i)
		{
			int decoSize = 0;

			try
			{
				decoSize = std::stoi(split.at(decoCountIndex + 1 + i));
				armor.decorationSlots.push_back(decoSize);
			}
			catch (...)
			{
				return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_DECO_SIZE_IS_NOT_NUM);
			}
		}
	}

	return 0;
}

int Database::initDecorationData(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream decoDataFile("Data/" + setting->getLanguagePath() + "/DecorationData");

	// Check if it's valid
	if (decoDataFile.is_open())
	{
		std::string line;

		// ID for each charms
		int idCounter = 0;

		// iterate by each line
		while (std::getline(decoDataFile, line))
		{
			// check line
			if (!line.empty())
			{
#if _DEBUG
				// Debug
				auto wLine = utf8_decode(line);
				std::string str = "Processing deco data: " + line + "\n";
				OutputDebugString(std::wstring(str.begin(), str.end()).c_str());
#endif

				// Split line by commna
				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				if (size == 4 || size == 6)
				{
					Decoration& deco = decorations[idCounter];
					deco.id = idCounter;

					// Name of decoration. Name + Jewel + size
					deco.name = utf8_decode(split.at(3));

					if (deco.name == L"?")
					{
						deco.name = L"[Missing Name]";
					}

					int decoSize = 0;

					try
					{
						// get max level
						decoSize = std::stoi(split.at(1));
					}
					catch (...)
					{
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_DECO_SIZE_IS_NOT_NUM);
					}

					deco.size = decoSize;

					int rarity = 0;

					try
					{
						// get max level
						rarity = std::stoi(split.at(2));
					}
					catch (...)
					{
						return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_DECO_RARITY_IS_NOT_NUM);
					}

					deco.rarity = rarity;

					std::wstring skillName = utf8_decode(split.at(0));

					auto find_it = skillNameToIdLUT.find(skillName);
					if (find_it == skillNameToIdLUT.end())
					{
						return static_cast<int>(MHW::ERROR_CODE::DECO_SKILL_NAME_DOES_NOT_EXIST);
					}
					else
					{
						deco.skillId = find_it->second;
						deco.setSkill = false;
						deco.highRankSetSkill = false;

						skillIdToDecorationIdLUT[deco.skillId] = deco.id;
					}

					if (size == 6)
					{
						// This decoration give set skill
						std::wstring setSkillName = utf8_decode(split.at(4));

						int reqArmorPieces = 0;

						try
						{
							// get max level
							reqArmorPieces = std::stoi(split.at(5));
						}
						catch (...)
						{
							return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_DECO_SET_SKILL_REQ_ARMOR_PIECE_IS_NOT_NUM);
						}

						// Set skill name key is Set Skill Name +  whitespace + Required Armor Pieces
						std::wstring setSkillNameKey = setSkillName + L" " + utf8_decode(split.at(5));

						auto find_it = setSkillNameToIdLUT.find(setSkillNameKey);
						if (find_it == setSkillNameToIdLUT.end())
						{
							return static_cast<int>(MHW::ERROR_CODE::DECO_SET_SKILL_NAME_DOES_NOT_EXIST);
						}
						else
						{
							deco.setSkillId = find_it->second;
							deco.setSkill = true;
							// For now, all deocrations give high rank set 
							deco.highRankSetSkill = true;

							setSkillIdToDecorationIdLUT[deco.setSkillId] = deco.id;
							//skillIdToDecorationIdLUT[deco.skillId] = deco.id;
						}
					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_DECO_DATA_INCORRECT_SPLIT_SIZE);
				}
			}

			setting->decorationCheckList.push_back(false);

			idCounter++;
		}
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_DECO_SIZE_IS_NOT_NUM);
	}

	return 0;
}

int Database::getSetSkillIdByDropdownIndex(const int dropdownIndex, const bool HR)
{
	int result = -1;

	int index = 0;

	if (HR)
	{
		// Add high rank set skill
		for (auto& e : highRankSetSkills)
		{
			if ((e.second).dropdownIndex == dropdownIndex)
			{
				result = highRankSetSkills[index].originalIndex;

				break;
			}

			index++;
		}
	}
	else
	{
		// Add low rank set skill
		for (auto& e : lowRankSetSkills)
		{
			if ((e.second).dropdownIndex == dropdownIndex)
			{
				result = lowRankSetSkills[index].originalIndex;

				break;
			}

			index++;
		}
	}

	return result;
}

std::wstring Database::getSetSkillDescriptionById(const int id, const bool HR)
{
	if (HR)
	{
		auto find_it = highRankSetSkills.find(id);
		if (find_it == highRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).description;
		}
	}
	else
	{
		auto find_it = lowRankSetSkills.find(id);
		if (find_it == lowRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).description;
		}
	}
}

int Database::getSkillIdByDropdownIndex(const int dropdownIndex)
{
	int result = -1;

	int index = 0;

	// Add high rank set skill
	for (auto& e : skills)
	{
		if ((e.second).dropdownIndex == dropdownIndex)
		{
			result = skills[index].originalIndex;

			break;
		}

		index++;
	}

	return result;
}

std::wstring Database::getSkillDescriptionById(const int id)
{
	auto find_it = skills.find(id);
	if (find_it == skills.end())
	{
		return L"";
	}
	else
	{
		return (find_it->second).description;
	}
}

std::wstring Database::getSkillLevelDescriptionById(const int id)
{
	auto find_it = skills.find(id);
	if (find_it == skills.end())
	{
		return L"";
	}
	else
	{
		return (find_it->second).levelDescription;
	}
}

int Database::getSkillLevelById(const int id)
{
	auto find_it = skills.find(id);
	if (find_it == skills.end())
	{
		return -1;
	}
	else
	{
		return (find_it->second).level;
	}
}

std::wstring Database::getSkillNameById(const int id)
{
	auto find_key = skillIdToKeyLUT.find(id);

	if (find_key == skillIdToKeyLUT.end())
	{
		return L"";
	}
	else
	{
		auto find_it = skills.find(find_key->second);

		if (find_it == skills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).name;
		}
	}
}

std::wstring Database::getSetSkillNameById(const int id, const bool HR)
{
	if (HR)
	{
		auto find_it = highRankSetSkills.find(id);

		if (find_it == highRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).skillName;
		}
	}
	else
	{
		auto find_it = lowRankSetSkills.find(id);

		if (find_it == lowRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).skillName;
		}
	}
}

std::wstring Database::getSetSkillGroupNameById(const int id, const bool HR)
{
	if (HR)
	{
		auto find_it = highRankSetSkills.find(id);

		if (find_it == highRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).name;
		}
	}
	else
	{
		auto find_it = lowRankSetSkills.find(id);

		if (find_it == lowRankSetSkills.end())
		{
			return L"";
		}
		else
		{
			return (find_it->second).name;
		}
	}
}

Skill * Database::getSkillByIDAndLevel(const int id, const int level)
{
	for (auto& skill : skills)
	{
		if (skill.second.id == id)
		{
			// matches id
			if (skill.second.maxLevel < level)
			{
				// Querying level is higher than skill's max level
				if (skill.second.level == skill.second.maxLevel)
				{
					// return the skill with max level
					return &(skill.second);
				}
				// Else, continue
			}
			else if (skill.second.maxLevel >= level)
			{
				// Querying level is equal or lower than max level
				if (skill.second.level == level)
				{
					// return the matching one
					return &(skill.second);
				}
			}
			// Else, continue
		}
		// Else, continue
	}

	return nullptr;
}

SetSkill * Database::getSetSkillByID(const int id, const bool HR)
{
	if (HR)
	{
		auto find_it = highRankSetSkills.find(id);
		if (find_it == highRankSetSkills.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_it->second);
		}
	}
	else
	{
		auto find_it = lowRankSetSkills.find(id);
		if (find_it == lowRankSetSkills.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_it->second);
		}
	}
}

Decoration * Database::getDecorationBySkillID(const int skillId)
{
	auto find_deco_id = skillIdToDecorationIdLUT.find(skillId);
	if (find_deco_id == skillIdToDecorationIdLUT.end())
	{
		// warning?
		return nullptr;
	}
	else
	{
		const int decoId = find_deco_id->second;

		auto find_deco = decorations.find(decoId);
		if (find_deco == decorations.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_deco->second);
		}
	}
}

Armor * Database::getHeadArmorByID(const int id)
{
	if (id == -1)
	{
		return nullptr;
	}
	else if (id < ANY_ARMOR_ID_START)
	{
		// normal head armor
		auto find_head = headArmors.find(id);
		if (find_head == headArmors.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_head->second);
		}
	}
	else
	{
		// any head armor
		const int size = anyHeadArmors.size();

		if ((id - ANY_ARMOR_ID_START) >= size)
		{
			return nullptr;
		}
		else
		{
			auto find_it = anyHeadArmors.find(id);
			if (find_it == anyHeadArmors.end())
			{
				return nullptr;
			}
			else
			{
				return &(find_it->second);
			}
		}
	}
}

Armor * Database::getChestArmorByID(const int id)
{
	if (id == -1)
	{
		return nullptr;
	}
	else if (id < ANY_ARMOR_ID_START)
	{
		// normal chest armor
		auto find_chest = chestArmors.find(id);
		if (find_chest == chestArmors.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_chest->second);
		}
	}
	else
	{
		// any chest armor
		const int size = anyChestArmors.size();

		if ((id - ANY_ARMOR_ID_START) >= size)
		{
			return nullptr;
		}
		else
		{
			auto find_it = anyChestArmors.find(id);
			if (find_it == anyChestArmors.end())
			{
				return nullptr;
			}
			else
			{
				return &(find_it->second);
			}
		}
	}
}

Armor * Database::getArmArmorByID(const int id)
{
	if (id == -1)
	{
		return nullptr;
	}
	else if (id < ANY_ARMOR_ID_START)
	{
		// normal arm armor
		auto find_arm = armArmors.find(id);
		if (find_arm == armArmors.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_arm->second);
		}
	}
	else
	{
		// any arm armor
		const int size = anyArmArmors.size();

		if ((id - ANY_ARMOR_ID_START) >= size)
		{
			return nullptr;
		}
		else
		{
			auto find_it = anyArmArmors.find(id);
			if (find_it == anyArmArmors.end())
			{
				return nullptr;
			}
			else
			{
				return &(find_it->second);
			}
		}
	}
}

Armor * Database::getWaistArmorByID(const int id)
{
	if (id == -1)
	{
		return nullptr;
	}
	else if (id < ANY_ARMOR_ID_START)
	{
		// normal waist armor
		auto find_waist = waistArmors.find(id);
		if (find_waist == waistArmors.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_waist->second);
		}
	}
	else
	{
		// any waist armor
		const int size = anyWaistArmors.size();

		if ((id - ANY_ARMOR_ID_START) >= size)
		{
			return nullptr;
		}
		else
		{
			auto find_it = anyWaistArmors.find(id);
			if (find_it == anyWaistArmors.end())
			{
				return nullptr;
			}
			else
			{
				return &(find_it->second);
			}
		}
	}
}

Armor * Database::getLegArmorByID(const int id)
{
	if (id == -1)
	{
		return nullptr;
	}
	else if (id < ANY_ARMOR_ID_START)
	{
		// normal leg armor
		auto find_leg = legArmors.find(id);
		if (find_leg == legArmors.end())
		{
			return nullptr;
		}
		else
		{
			return &(find_leg->second);
		}
	}
	else
	{
		// any leg armor
		const int size = anyLegArmors.size();

		if ((id - ANY_ARMOR_ID_START) >= size)
		{
			return nullptr;
		}
		else
		{
			auto find_it = anyLegArmors.find(id);
			if (find_it == anyLegArmors.end())
			{
				return nullptr;
			}
			else
			{
				return &(find_it->second);
			}
		}
	}
}

Armor * Database::getArmorByDropdownListIndex(std::map<int, Armor>& armors, const int index)
{
	for (auto& e : armors)
	{
		if ((e.second).dropdownListIndex == index)
		{
			return &(e.second);
		}
	}

	return nullptr;
}

Charm * Database::getCharmByID(const int id)
{
	auto find_charm = charms.find(id);
	if (find_charm == charms.end())
	{
		return nullptr;
	}
	else
	{
		return &(find_charm->second);
	}
}

Charm * Database::getNextLevelCharm(Charm * charm)
{
	if (charm)
	{
		if (charm->level == charm->maxLevel)
		{
			return nullptr;
		}
		else
		{
			auto find = charms.find(charm->dropdownIndex + 1);
			if (find == charms.end())
			{
				return nullptr;
			}
			else
			{
				return &(find->second);
			}
		}
	}
	else
	{
		return nullptr;
	}
}

void Database::getArmorBySkill(std::map<int, Armor>& armors, std::vector<Armor*>& queriedArmors, std::unordered_set<int>& skillFilter, std::unordered_set<int>& decoSetSkillFilter, std::unordered_set<int>& lowRankSetSkillFilter, std::unordered_set<int>& highRankSetSkillFilter, const bool LR, const bool HR, const MHW::Gender genderSetting)
{
	// This function queires all the armor that has filtered skill, set skill and set skill from decoration

	for (auto iter = armors.rbegin(); iter != armors.rend(); ++iter)			// iterate all armors from backward because armors are in order of low to high rarity
	//for (auto iter = armors.begin(); iter != armors.end(); ++iter)			// iterate all  armors
	{
		Armor& armor = (iter->second);
		bool found = false;

		// armor rank
		bool highRank = armor.highRank;

		if (!skillFilter.empty())
		{
			// iterate all head skills
			for (auto& skillID : armor.skills)
			{
				// Check if this skill exist in armor skill filter
				auto find_it = skillFilter.find(skillID);
				if (find_it == skillFilter.end())
				{
					// This head armor doesn't have desired skill. skip.
					continue;
				}
				else
				{
					// This head armor has desired skill.

					// check for high rank
					if (HR && highRank)
					{
						// Add to head armor filter
						if (armor.gender == MHW::Gender::NONE)
						{
							// This armor doesn't have gender
							queriedArmors.push_back(&armor);
							found = true;
						}
						else
						{
							if (armor.gender == MHW::Gender::MALE && genderSetting == MHW::Gender::MALE)
							{
								// This armor is for male and gender is set to male
								queriedArmors.push_back(&armor);
								found = true;
							}
							else if (armor.gender == MHW::Gender::FEMALE && genderSetting == MHW::Gender::FEMALE)
							{
								// this armor is for female and gender is set to female
								queriedArmors.push_back(&armor);
								found = true;
							}
						}
					}

					// Check for low rank
					if (LR && !highRank)
					{
						// Add to head armor filter
						if (armor.gender == MHW::Gender::NONE)
						{
							// This armor doesn't have gender
							queriedArmors.push_back(&armor);
							found = true;
						}
						else
						{
							if (armor.gender == MHW::Gender::MALE && genderSetting == MHW::Gender::MALE)
							{
								// This armor is for male and gender is set to male
								queriedArmors.push_back(&armor);
								found = true;
							}
							else if (armor.gender == MHW::Gender::FEMALE && genderSetting == MHW::Gender::FEMALE)
							{
								// this armor is for female and gender is set to female
								queriedArmors.push_back(&armor);
								found = true;
							}
						}
					}

					if (found)
					{
						// Armor was found. Stop iterating armor skill
						break;
					}
				}
			}
			// head armor skill iter ends
		}

		// Check if this head armor was added to filter
		if (!found)
		{
			// It did not. Check if this armor is one of the selected set skill

			// Gender doesn't matter here

			// get set's group id (i.e. Odoragon set skill)
			const int ssgId = armor.setGroupId;

			if (ssgId != -1)
			{
				// has set skill
				if (highRank)
				{
					// armor is high rank
					if (!highRankSetSkillFilter.empty())
					{
						auto find_setSkill = highRankSetSkillFilter.find(ssgId);
						if (find_setSkill != highRankSetSkillFilter.end())
						{
							// add to head armor filter
							queriedArmors.push_back(&armor);
							found = true;
						}
						// else, nope.
					}
				}
				else
				{
					// armor is low rank
					if (!lowRankSetSkillFilter.empty())
					{
						auto find_setSkill = lowRankSetSkillFilter.find(ssgId);
						if (find_setSkill != lowRankSetSkillFilter.end())
						{
							// add to head armor filter
							queriedArmors.push_back(&armor);
							found = true;
						}
						// else, nope
					}
				}

				if (!found)
				{
					// Still hasn't found! check if this armor's set skill is also from deco set skill

					// Gender doesn't matter here.

					if (!decoSetSkillFilter.empty())
					{
						if (ssgId != -1)
						{
							auto find_deco_id = setSkillGroupIdToDecoSetSkillIDLUT.find(ssgId);
							if (find_deco_id == setSkillGroupIdToDecoSetSkillIDLUT.end())
							{
								// We don't need this set skill.
							}
							else
							{
								if (decoSetSkillFilter.find(find_deco_id->second) != decoSetSkillFilter.end())
								{
									// add to head armor filter
									queriedArmors.push_back(&armor);
									found = true;
								}
								// Else, nope
							}
						}
					}
					// else, no setskill from deco
				}
				// else, nope
			}
			// else, set skill id is -1
		}
		// else found
	}
}

void Database::getAllCharmsBySkill(std::vector<int>& filterCharmIndices, std::unordered_set<int>& filterSkills, std::unordered_set<int>& filterSetSkills)
{
	for (auto& charm : charms)
	{
		bool found = false;

		// First skill
		if ((charm.second).setSkill)
		{
			// this charm has set skill
			auto find_it = filterSetSkills.find((charm.second).skillId);
			if (find_it == filterSetSkills.end())
			{
				found = false;
			}
			else
			{
				found = true;
				filterCharmIndices.push_back((charm.second).dropdownIndex);
				continue;
			}
		}
		else
		{
			// this charm doesn't have set skill
			auto find_it = filterSkills.find((charm.second).skillId);
			if (find_it == filterSkills.end())
			{
				found = false;
			}
			else
			{
				found = true;
				filterCharmIndices.push_back((charm.second).dropdownIndex);
				continue;
			}
		}

		// here, it failed to find first skill
		if (!found && (charm.second).hasSecondSkill())
		{
			// second skill is always not set skill (patch 1.06)
			auto find_it = filterSkills.find((charm.second).secondSkillId);
			if (find_it == filterSkills.end())
			{
				found = false;
			}
			else
			{
				found = true;
				filterCharmIndices.push_back((charm.second).dropdownIndex);
				continue;
			}
		}
	}
}

void Database::getAllMinLevelCharmsBySkill(std::vector<Charm*>& filterCharms, std::unordered_set<int>& skillFilters, std::unordered_set<int>& decoSetSkillFilters)
{
	// keep track of charm's skill id.
	std::unordered_set<int> lut;

	for (auto& charm : charms)
	{
		bool found = false;

		// First skill
		if ((charm.second).setSkill)
		{
			// this charm has set skill
			auto find_it = decoSetSkillFilters.find((charm.second).skillId);
			if (find_it == decoSetSkillFilters.end())
			{
				found = false;
			}
			else
			{
				found = true;
				filterCharms.push_back(&(charm.second));
				// no need to add to lut because there is only 1 charm with set skill and max level is 1 (patch 1.06)
				continue;
			}
		}
		else
		{
			// this charm doesn't have set skill
			auto find_it = skillFilters.find((charm.second).skillId);
			if (find_it == skillFilters.end())
			{
				found = false;
			}
			else
			{
				auto find_skill_id = lut.find((charm.second).skillId);
				if (find_skill_id == lut.end())
				{
					found = true;
					filterCharms.push_back(&(charm.second));
					lut.emplace((charm.second).skillId);
					continue;
				}
				else
				{
					// already added
					continue;
				}
			}
		}

		// here, it failed to find first skill
		if (!found && (charm.second).hasSecondSkill())
		{
			// second skill is always not set skill (patch 1.06)
			// charm with second skill is always have max level of 1. (patch 1.06)
			auto find_it = skillFilters.find((charm.second).secondSkillId);
			if (find_it == skillFilters.end())
			{
				found = false;
			}
			else
			{
				found = true;
				filterCharms.push_back(&(charm.second));
				continue;
			}
		}
	}
}

void Database::initDefaultAnyArmors()
{
	initDefaultAnyArmor(anyHeadArmors);
	initDefaultAnyArmor(anyChestArmors);
	initDefaultAnyArmor(anyArmArmors);
	initDefaultAnyArmor(anyWaistArmors);
	initDefaultAnyArmor(anyLegArmors);
}

void Database::initDefaultAnyArmor(std::map<int, Armor>& anyArmors)
{
	auto& lrDefaultAny = anyArmors[ANY_ARMOR_ID_START];
	lrDefaultAny.highRank = false;
	lrDefaultAny.id = ANY_ARMOR_ID_START;
	lrDefaultAny.decorationSlots.clear();

	auto& hrDefaultAny = anyArmors[ANY_ARMOR_ID_START + 1];
	hrDefaultAny.highRank = true;
	hrDefaultAny.id = ANY_ARMOR_ID_START + 1;
	hrDefaultAny.decorationSlots.clear();
}

void Database::getAnyArmor(std::vector<Armor*>& filterArmors, std::map<int, Armor>& anyArmors, const bool LR, const bool HR)
{
	for (auto& e : anyArmors)
	{
		if (e.second.highRank && HR)
		{
			filterArmors.push_back(&(e.second));
		}
		else if (!e.second.highRank && LR)
		{
			filterArmors.push_back(&(e.second));
		}
	}
}

bool Database::doesSkillCanComeFromDeco(const int skillId)
{
	Skill* skill = getSkillByIDAndLevel(skillId, 1);

	if (skill)
	{
		return skill->onlyFromDeco;
	}
	else
	{
		return false;
	}
}

bool Database::hasSameTypeOfAnyArmor(Armor & armor, const std::map<int, Armor>& anyArmors)
{
	if (armor.decorationSlots.size() == 0)
	{
		// Only check that has decoration slots
		return true;
	}
	else
	{
		//const int newTotalDecoSlots = armor.decorationSlots.size();

		for (auto& e : anyArmors)
		{
			/*
			const int totalDecoSlots = (e.second).decorationSlots.size();

			if (totalDecoSlots == newTotalDecoSlots)
			{
				for (int i = 0; i < totalDecoSlots; ++i)
				{
					if(armor.decorationSlots.at(i) != )
				}
			}
			*/
			if ((e.second).decorationSlots == armor.decorationSlots)
			{
				// same.
				return true;
			}
		}

		return false;
	}
}

int Database::reloadNames(Settings * setting)
{
	int result = 0;

	result = reloadSkillNames(setting);
	if (result != 0) return result;

	result = reloadSetSkillNames(setting);
	if (result != 0) return result;

	result = reloadCharmNames(setting);
	if (result != 0) return result;

	result = reloadArmorNames(setting);
	if (result != 0) return result;

	result = reloadDecorationNames(setting);
	if (result != 0) return result;

	return 0;
}

int Database::reloadSkillNames(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream skillData("Data/" + setting->getLanguagePath() + "/SkillData");

	skillNameToIdLUT.clear();
	skillIdToNameLUT.clear();

	// Check if data is valid
	if (skillData.is_open())
	{
		std::string line;

		// ID for each charms
		int idCounter = 0;

		// 0 is None in dropdown list.
		int indexCounter = 0;

		// get each line
		while (std::getline(skillData, line))
		{
			// check str line
			if (!line.empty())
			{
				// Split line by commna
				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				if (size == 4)
				{
					auto find_skill = skills.find(indexCounter);

					if (find_skill != skills.end())
					{
						Skill* skill = &(find_skill->second);
						std::wstring skillName = utf8_decode(split.at(0));

						const int maxLvl = skill->maxLevel;

						std::getline(skillData, line);
						std::wstring description = utf8_decode(line);

						for (int i = 1; i <= maxLvl; i++)
						{
							std::getline(skillData, line);
							skill->name = skillName;
							skill->description = description;
							skill->levelDescription = utf8_decode(line);

							find_skill++;
							if (find_skill != skills.end())
							{
								skill = &(find_skill->second);
							}
						}

						indexCounter += maxLvl;

						skillNameToIdLUT[skillName] = skill->id;
						skillIdToNameLUT[skill->id] = skillName;
					}
					else
					{
						return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_CANT_FIND_SKILL);
					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_SKILL_DATA_INCORRECT_SPLIT_SIZE);
				}
			}
		}
	}

	return 0;
}

int Database::reloadSetSkillNames(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream setSkillData("Data/" + setting->getLanguagePath() + "/SetSkillData");
	
	if (setSkillData.is_open())
	{
		reloadLowRankSetSkillNames(setSkillData);
		reloadHighRankSetSkillNames(setSkillData);
	}
	else
	{
		// failed to read set skill data
		return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_FAILED_TO_READ_SET_SKILL_DATA_FILE);
	}

	return 0;
}

int Database::reloadLowRankSetSkillNames(std::ifstream & file)
{
	setSkillNameToIdLUT.clear();
	setSkillIdToNameLUT.clear();
	setSkillGroupNameToIdLUT.clear();

	std::string line;

	std::getline(file, line);

	int lowRankSetSkillCount = 0;

	try
	{
		lowRankSetSkillCount = std::stoi(line);
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_LOW_RANK_SET_SKILL_COUNT_IS_NOT_NUM);
	}

	int idCounter = 0;		// ID for each charms
	int indexCounter = 0;	// 0 is None in dropdown list.

	for (int i = 0; i < lowRankSetSkillCount; ++i)
	{
		std::getline(file, line);

		// check if line is empty
		if (!line.empty())
		{
			std::vector<std::string> split = splitByComma(line);

			auto size = split.size();

			// Low rank doesn't have multiple set skills
			if (size == 3)
			{
				SetSkill& curSetSkill = lowRankSetSkills[indexCounter];

				curSetSkill.name = utf8_decode(split.at(0));
				curSetSkill.skillName = utf8_decode(split.at(1));

				// get description
				std::getline(file, line);
				curSetSkill.description = utf8_decode(line);

				std::wstring setSkillName = curSetSkill.name + L" " + utf8_decode(split.at(2));

				setSkillNameToIdLUT[setSkillName] = idCounter;
				setSkillIdToNameLUT[idCounter] = setSkillName;
				setSkillGroupNameToIdLUT[curSetSkill.name] = indexCounter;

				idCounter++;
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_LOW_RANK_SET_SKILL_INCORECT_SPLIT_SIZE);
			}
		}
		// else, line is empty.

		indexCounter++;
	}

	return 0;
}

int Database::reloadHighRankSetSkillNames(std::ifstream & file)
{
	setSkillNameToIdLUT.clear();
	setSkillIdToNameLUT.clear();
	setSkillGroupNameToIdLUT.clear();

	std::string line;

	std::getline(file, line);

	int highRankSetSkillCount = 0;

	try
	{
		highRankSetSkillCount = std::stoi(line);
	}
	catch (...)
	{
		return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_HIGH_RANK_SET_SKILL_COUNT_IS_NOT_NUM);
	}

	int idCounter = 0;
	int indexCounter = 0;	// 0 is None in dropdown list.

	// get each line
	while (std::getline(file, line))
	{
		// check if line is empty
		if (!line.empty())
		{
			std::vector<std::string> split = splitByComma(line);

			auto size = split.size();

			// Splitted strings must be either 3 or 5
			if (size == 3 || size == 5)
			{
				SetSkill& curSetSkill = highRankSetSkills[indexCounter];

				curSetSkill.name = utf8_decode(split.at(0));
				curSetSkill.skillName = utf8_decode(split.at(1));

				// get description
				std::getline(file, line);
				curSetSkill.description = utf8_decode(line);

				std::wstring setSkillName = curSetSkill.name + L" " + utf8_decode(split.at(2));

				setSkillNameToIdLUT[setSkillName] = idCounter;
				setSkillIdToNameLUT[idCounter] = setSkillName;
				setSkillGroupNameToIdLUT[curSetSkill.name] = indexCounter;

				indexCounter++;
				idCounter++;

				if (size == 5)
				{
					secondSetSkillCheckLUT[idCounter - 1] = true;

					// add second skill set
					SetSkill& secondSetSkill = highRankSetSkills[indexCounter];
					secondSetSkill.name = utf8_decode(split.at(0));
					secondSetSkill.skillName = utf8_decode(split.at(3));

					// get description
					std::getline(file, line);
					secondSetSkill.description = utf8_decode(line);

					setSkillNameToIdLUT[setSkillName] = idCounter;
					setSkillIdToNameLUT[idCounter] = setSkillName;

					indexCounter++;
					idCounter++;
				}
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_HIGH_RANK_SET_SKILL_INCORECT_SPLIT_SIZE);
			}
		}
	}

	return 0;
}

int Database::reloadCharmNames(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream charmDataFile("Data/" + setting->getLanguagePath() + "/CharmData");

	// Check if it's valid
	if (charmDataFile.is_open())
	{
		std::string line;

		int index = 2;
		
		// iterate by each line
		while (std::getline(charmDataFile, line))
		{
			// check line
			if (!line.empty())
			{
				// not empty
				OutputDebugString((L"processing: " + utf8_decode(line) + L"\n").c_str());

				// split line by comma
				std::vector<std::string> split = Utility::splitByComma(line);

				auto size = split.size();

				if (3 <= size && size <= 4)
				{
					std::wstring charmName = utf8_decode(split.at(0));
					std::wstring skillName = utf8_decode(split.at(1));
					std::wstring secondSkillName = L"";

					if (size == 4)
					{
						secondSkillName = utf8_decode(split.at(2));
					}
					else
					{
						secondSkillName = L"";
					}

					auto find_charm = charms.find(index);

					if (find_charm != charms.end())
					{
						Charm* curCharm = &(find_charm->second);

						const int maxLvl = (find_charm->second).maxLevel;

						for (int i = 1; i <= maxLvl; ++i)
						{
							curCharm->name = charmName;
							curCharm->skillName = skillName;
							curCharm->secondSkillName = secondSkillName;

							find_charm++;
							if (find_charm != charms.end())
							{
								curCharm = &(find_charm->second);
							}
						}
						
						index += maxLvl;
					}
					else
					{
						return static_cast<int>(MHW::ERROR_CODE::REALOD_NAME_CANT_FIND_CHARM);
					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::RELOAD_CHARM_NAME_INCORRECT_SPLIT_SIZE);
				}
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::RELOAD_CHARM_NAME_EMPTY_LINE);
			}
		}

	}

	return 0;
}

int Database::reloadArmorNames(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream armorDataFile("Data/" + setting->getLanguagePath() + "/ArmorData");

	// Check if it's valid
	if (armorDataFile.is_open())
	{
		std::string line;

		int headIndex = 0;
		int chestIndex = 0;
		int armIndex = 0;
		int waistIndex = 0;
		int legIndex = 0;

		// iterate by each line
		while (std::getline(armorDataFile, line))
		{
			// check line
			if (!line.empty())
			{
				// not empty

				// split line by comma
				std::vector<std::string> split = Utility::splitByComma(line);

				auto size = split.size();

				if (size == 4 || size == 5)
				{
					// header
					std::wstring setName = utf8_decode(split.at(0));

					// armors.
					for (int i = 0; i < 5; ++i)
					{
						std::getline(armorDataFile, line);

						if (line == "-")
						{
							continue;
						}

						std::vector<std::string> armorSplit = Utility::splitByComma(line);

						std::wstring name = utf8_decode(armorSplit.at(0));

						switch (i)
						{
						case 0:
						{
							// head
							headArmors[headIndex].name = name;
							headArmors[headIndex].setName = setName;
							headIndex++;
						}
							break;
						case 1:
						{
							// chest
							chestArmors[chestIndex].name = name;
							chestArmors[chestIndex].setName = setName;
							chestIndex++;
						}
							break;
						case 2:
						{
							// arm
							armArmors[armIndex].name = name;
							armArmors[armIndex].setName = setName;
							armIndex++;
						}
							break;
						case 3:
						{
							// waist
							waistArmors[waistIndex].name = name;
							waistArmors[waistIndex].setName = setName;
							waistIndex++;
						}
							break;
						case 4:
						{
							// leg
							legArmors[legIndex].name = name;
							legArmors[legIndex].setName = setName;
							legIndex++;
						}
							break;
						default:
							break;
						}

					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::RELOAD_NAME_ARMOR_DATA_INCORRECT_SPLIT_SIZE);
				}
			}
		}
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_ARMOR_DATA_FILE);
	}

	return 0;
}

int Database::reloadDecorationNames(Settings * setting)
{
	// Read charm data file and init to map
	std::ifstream decoDataFile("Data/" + setting->getLanguagePath() + "/DecorationData");

	// Check if it's valid
	if (decoDataFile.is_open())
	{
		std::string line;

		// ID for each charms
		int idCounter = 0;

		// iterate by each line
		while (std::getline(decoDataFile, line))
		{
			// check line
			if (!line.empty())
			{
				// Split line by commna
				std::vector<std::string> split = splitByComma(line);

				auto size = split.size();

				if (size == 4 || size == 6)
				{
					Decoration& deco = decorations[idCounter];

					// Name of decoration. Name + Jewel + size
					deco.name = utf8_decode(split.at(3));

					if (deco.name == L"?")
					{
						deco.name = L"[Missing Name]";
					}
				}
				else
				{
					return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_DECO_DATA_INCORRECT_SPLIT_SIZE);
				}
			}

			idCounter++;
		}
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::BAD_FILE_ARMOR_DATA_DECO_SIZE_IS_NOT_NUM);
	}

	return 0;
}

void Database::clear()
{
	charms.clear();
	skills.clear();
	lowRankSetSkills.clear();
	highRankSetSkills.clear();

	bulwarkCharm = nullptr;
	guardUpSkill = nullptr;
	uragaanProtectionSetSkill = nullptr;

	headArmors.clear();
	anyHeadArmors.clear();
	totalLowRankHeadArmors = 0;
	totalHighRankHeadArmors = 0;

	chestArmors.clear();
	anyChestArmors.clear();
	totalLowRankChestArmors = 0;
	totalHighRankChestArmors = 0;

	armArmors.clear();
	anyArmArmors.clear();
	totalLowRankArmArmors = 0;
	totalHighRankArmArmors = 0;

	waistArmors.clear();
	anyWaistArmors.clear();
	totalLowRankWaistArmors = 0;
	totalHighRankWaistArmors = 0;

	legArmors.clear();
	anyLegArmors.clear();
	totalLowRankLegArmors = 0;
	totalHighRankArmArmors = 0;

	decorations.clear();

	skillIdToNameLUT.clear();
	skillNameToIdLUT.clear();
	skillIdToKeyLUT.clear();
	setSkillIdToNameLUT.clear();
	setSkillNameToIdLUT.clear();
	setSkillGroupNameToIdLUT.clear();
	secondSetSkillCheckLUT.clear();
	setSkillGroupIdToDecoSetSkillIDLUT.clear();
	skillIdToDecorationIdLUT.clear();
	setSkillIdToDecorationIdLUT.clear();
}
