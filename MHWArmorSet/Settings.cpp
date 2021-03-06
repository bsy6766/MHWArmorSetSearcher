#include "stdafx.h"
#include "Settings.h"
#include "Const.h"
#include "Utility.h"
#include "Logger.h"
#include "Database.h"

using namespace MHW::CONSTS;

Settings::Settings()
	: majorVersion(0)
	, minorVersion(0)
	, language(MHW::Language::ENGLISH)
	, totalWeaponSlots(0)
	, weaponSlot1Size(1)
	, weaponSlot2Size(1)
	, weaponSlot3Size(1)
	, charmIndex(0)
	, charmDisplaySetting(MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME)
	, skillIndex(0)
	, setSkillIndex(0)
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
	, allowArenaArmor(false)
	, allowEventArmor(false)
	, minArmorRarity(5)
	, allowExtraSkills(true)
	, searchFromHigherArmorRarity(true)
	, allowOverleveledSkills(true)
	//, useOnlyMaxLevelCharm(false)
	, simplifySearchResult(true)
	, showMaxLevel(false)
{}

int Settings::init()
{
	auto& logger = MHW::Logger::getInstance();

	int result = readTemp();
	if (result != 0)
	{
		logger.errorCode(result);
		
		// init to default
		clear();
		saveTemp();
	}
		
	result = initStringLiterals();
	if (result != 0)
	{
		return result;
	}

	return 0;
}

int Settings::initStringLiterals()
{
	stringLiterals.clear();

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
				return static_cast<int>(MHW::ERROR_CODE::STRING_DATA_EMPTY_LINE);
			}
		}

		return 0;
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_STRING_DATA);
	}
}

int Settings::readTemp()
{
	auto& logger = MHW::Logger::getInstance();

	auto wd = logger.getWorkingDirectory();
	auto tempPath = wd + L"/Data/temp";

	if (!PathFileExistsW((LPWSTR)tempPath.c_str()))
	{
		// init with default
		logger.info("Previous settings don't exist. Creating new.");
		clear();
		saveTemp();

		return 0;
	}
	else
	{
		logger.info("Found previsouly used settings.");

		std::wifstream tempFile(tempPath);

		if (tempFile.is_open())
		{
			// string to read line
			std::wstring line;

			std::getline(tempFile, line);

			if (line != L"MHWASS_SAVE")
			{
				return static_cast<int>(MHW::ERROR_CODE::INVALID_SAVE_FILE);
			}

			std::getline(tempFile, line);

			// language
			int majorVersion = 0;
			if (!readValue(line, majorVersion))
			{
				return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_GET_MAJOR_VERSION);
			}

			std::getline(tempFile, line);

			// language
			int minorVersion = 0;
			if (!readValue(line, minorVersion))
			{
				return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_GET_MINOR_VERISON);
			}

			this->majorVersion = majorVersion;
			this->minorVersion = minorVersion;

			logger.info("Version: " + std::to_string(majorVersion) + "." + std::to_string(minorVersion));

			int loadResult = 0;

			// version 1.0
			loadResult = loadTemp(tempFile);

			if (loadResult != 0) return loadResult;

			// update major version
			if (majorVersion != MHW::VERSION::major)
			{
				majorVersion = MHW::VERSION::major;
				logger.info("Updating major version to: " + std::to_string(majorVersion));
			}
			
			// update minor version
			if (minorVersion != MHW::VERSION::minor)
			{
				minorVersion = MHW::VERSION::minor;
				logger.info("Updating minor version to: " + std::to_string(minorVersion));
			}

			return 0;
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_READ_TEMP);
		}

	}

	return 0;
}

bool Settings::readValue(const std::wstring str, int & dest)
{
	if (str.empty()) return false;

	// convert to number
	try
	{
		int result = std::stoi(str);

		dest = result;
		return true;
	}
	catch (...)
	{
		return false;
	}
}

int Settings::readLanguage(const std::wstring & line, MHW::Language & language, const std::string & log, const MHW::ERROR_CODE errCode)
{
	int val = 0;
	if (readValue(line, val))
	{
		if (val >= static_cast<int>(MHW::Language::MAX))
		{
			language = MHW::Language::ENGLISH;
		}
		else
		{
			language = static_cast<MHW::Language>(val);
		}

		MHW::Logger::getInstance().info(log + getLanguagePath(language));
	}
	else
	{
		return static_cast<int>(errCode);
	}

	return 0;
}

int Settings::readInt(const std::wstring & line, int & dest, const std::string & log, const MHW::ERROR_CODE errCode)
{
	if (readValue(line, dest))
	{
		MHW::Logger::getInstance().info(log + std::to_string(dest));
	}
	else
	{
		return static_cast<int>(errCode);
	}

	return 0;
}

int Settings::readBool(const std::wstring & line, bool & dest, const std::string & log, const MHW::ERROR_CODE errCode)
{
	int appliedVal = 0;
	if (readValue(line, appliedVal))
	{
		dest = (appliedVal == 1);
		MHW::Logger::getInstance().info(log + std::string(dest ? "true" : "false"));
	}
	else
	{
		return static_cast<int>(errCode);
	}

	return 0;
}

void Settings::saveTemp()
{
	auto& logger = MHW::Logger::getInstance();
	
	auto wd = logger.getWorkingDirectory();
	auto tempPath = wd + L"/Data/temp";

	if (PathFileExistsW((LPWSTR)tempPath.c_str()))
	{
		// file path exstis
		DeleteFileW((LPWSTR)tempPath.c_str());
	}

	std::wofstream ofs(tempPath);

	save(ofs);

	ofs.close();
}

void Settings::saveTemp(const std::wstring & path)
{
	if (PathFileExistsW((LPWSTR)path.c_str()))
	{
		// file path exstis
		DeleteFileW((LPWSTR)path.c_str());
	}

	std::wofstream ofs(path);

	save(ofs);

	ofs.close();
}

void Settings::save(std::wofstream & file)
{
	std::wstring data = L"MHWASS_SAVE\n";

	// version
	data += std::to_wstring(static_cast<int>(majorVersion));
	data += (L"\n" + std::to_wstring(static_cast<int>(minorVersion)));

	// language
	data += (L"\n" + std::to_wstring(static_cast<int>(language)));

	// weapon
	data += (L"\n" + std::to_wstring(static_cast<int>(totalWeaponSlots)));
	data += (L"\n" + std::to_wstring(static_cast<int>(weaponSlot1Size)));
	data += (L"\n" + std::to_wstring(static_cast<int>(weaponSlot2Size)));
	data += (L"\n" + std::to_wstring(static_cast<int>(weaponSlot3Size)));

	// charm
	data += (L"\n" + std::to_wstring(static_cast<int>(charmDisplaySetting)));
	data += (L"\n" + std::to_wstring(static_cast<int>(charmIndex)));

	// skill
	data += (L"\n" + std::to_wstring(static_cast<int>(skillIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(skills.size())));
	for (auto skill : skills)
	{
		data += (L"\n" + std::to_wstring(static_cast<int>(skill->id)));
		data += (L"\n" + std::to_wstring(static_cast<int>(skill->level)));
		data += (L"\n" + std::to_wstring(static_cast<int>((skill->applied ? 1 : 0))));
	}

	// set skill
	data += (L"\n" + std::to_wstring(static_cast<int>((highRankSetSkill ? 1 : 0))));
	data += (L"\n" + std::to_wstring(static_cast<int>(setSkillIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(setSkills.size())));
	for (auto setSkill : setSkills)
	{
		data += (L"\n" + std::to_wstring(static_cast<int>(setSkill->id)));
		data += (L"\n" + std::to_wstring(static_cast<int>(setSkill->reqArmorPieces)));
		data += (L"\n" + std::to_wstring(static_cast<int>((setSkill->applied ? 1 : 0))));
		data += (L"\n" + std::to_wstring(static_cast<int>((setSkill->highRank ? 1 : 0))));
	}

	// armor
	data += (L"\n" + std::to_wstring(static_cast<int>(highRankHeadArmor ? 1 : 0)));
	data += (L"\n" + std::to_wstring(static_cast<int>(highRankChestArmor ? 1 : 0)));
	data += (L"\n" + std::to_wstring(static_cast<int>(highRankArmArmor ? 1 : 0)));
	data += (L"\n" + std::to_wstring(static_cast<int>(highRankWaistArmor ? 1 : 0)));
	data += (L"\n" + std::to_wstring(static_cast<int>(highRankLegArmor ? 1 : 0)));

	data += (L"\n" + std::to_wstring(static_cast<int>(headArmorIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(chestArmorIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(armArmorIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(waistArmorIndex)));
	data += (L"\n" + std::to_wstring(static_cast<int>(legArmorIndex)));

	// deco
	data += (L"\n" + std::to_wstring(static_cast<int>(decorationCheckList.size())));
	const int size = decorationCheckList.size();
	for (int i = 0; i < size; ++i)
	{
		if (decorationCheckList.at(i))
		{
			data += (L"\n1");
		}
		else
		{
			data += (L"\n0");
		}

		data += ((L"\n") + std::to_wstring(decorationCountList.at(i)));
	}

	// options
	if (gender == MHW::Gender::MALE || gender == MHW::Gender::NONE)
	{
		data += (L"\n2");
	}
	else if (gender == MHW::Gender::FEMALE)
	{
		data += (L"\n1");
	}

	// allow low rank armor
	data += (L"\n" + std::wstring(allowLowRankArmor ? L"1" : L"0"));
	// allow arena armor
	data += (L"\n" + std::wstring(allowArenaArmor ? L"1" : L"0"));
	// allow event armor
	data += (L"\n" + std::wstring(allowEventArmor ? L"1" : L"0"));

	// min rarirty
	data += (L"\n" + std::to_wstring(static_cast<int>(minArmorRarity)));

	// allow extra level skill
	data += (L"\n" + std::wstring(allowExtraSkills ? L"1" : L"0"));
	// search from higher rank
	data += (L"\n" + std::wstring(searchFromHigherArmorRarity ? L"1" : L"0"));
	// allow over leveled skills
	data += (L"\n" + std::wstring(allowOverleveledSkills ? L"1" : L"0"));

	// use only max level charm
	//data += (L"\n" + std::wstring(useOnlyMaxLevelCharm ? L"1" : L"0"));

	// simplify search result
	data += (L"\n" + std::wstring(simplifySearchResult ? L"1" : L"0"));

	// show max level
	data += (L"\n" + std::wstring(showMaxLevel ? L"1" : L"0"));

	file.write(data.c_str(), data.size());
}

int Settings::openTemp(const std::wstring & path)
{
	auto& logger = MHW::Logger::getInstance();
	logger.info("Trying to open file...");

	std::wifstream file(path);

	if (file.is_open())
	{
		std::wstring line;

		std::getline(file, line);

		if (line != L"MHWASS_SAVE")
		{
			return static_cast<int>(MHW::ERROR_CODE::TRIED_TO_OPEN_INVALID_FILE);
		}

		std::getline(file, line);

		// language
		int majorVersion = 0;
		if (!readValue(line, majorVersion))
		{
			return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_GET_MAJOR_VERSION);
		}

		std::getline(file, line);

		// language
		int minorVersion = 0;
		if (!readValue(line, minorVersion))
		{
			return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_GET_MINOR_VERISON);
		}

		this->majorVersion = majorVersion;
		this->minorVersion = minorVersion;

		int result = loadTemp(file);
		if (result != 0)
		{
			logger.errorCode(result);
		}

		return 0;
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_OPNE_SAVE_FILE);
	}
}

int Settings::load(Database * db)
{
	auto& logger = MHW::Logger::getInstance();
	logger.info("Loading data to settings...");

	int result = loadSkills(db);
	if (result != 0) return result;

	result = loadSetSkills(db);
	if (result != 0) return result;
	
	tempSkillData.clear();
	tempSetSkillData.clear();

	if (decorationCheckList.empty())
	{
		logger.info("Deco check list is empty. Init all to true");
		decorationCheckList.resize(db->decorations.size(), true);
	}
	
	result = loadDecoCounts(db);
	if (result != 0) return result;
	
	return 0;
}

int Settings::loadSkills(Database * db)
{
	const int size = tempSkillData.size();

	std::list<Skill*> skills;

	for (int i = 0; i < size; ++i)
	{
		Skill* skill = db->getSkillByIDAndLevel(tempSkillData.at(i).skillId, tempSkillData.at(i).skillLevel);

		if (skill)
		{
			skill->applied = tempSkillData.at(i).applied;
			skills.push_back(skill);
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::SETTING_FAILED_TO_GET_SKILL_BY_ID_AND_LEVEL);
		}
	}

	this->skills = skills;

	return 0;
}

int Settings::loadSetSkills(Database * db)
{
	const int size = tempSetSkillData.size();

	std::list<SetSkill*> setSkills;

	for (int i = 0; i < size; ++i)
	{
		SetSkill* setSkill = db->getSetSkillByID(tempSetSkillData.at(i).setSkillId, tempSetSkillData.at(i).highRank);
		
		if (setSkill)
		{
			setSkill->applied = tempSetSkillData.at(i).applied;
			setSkills.push_back(setSkill);
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::SETTING_FAILED_TO_GET_SET_SKILL_BY_ID_AND_RANK);
		}
	}

	this->setSkills = setSkills;

	return 0;
}

int Settings::loadDecoCounts(Database * db)
{
	if (decorationCountList.empty())
	{
		MHW::Logger::getInstance().info("Deco count list is empty. Init to max");
		decorationCountList.resize(db->decorations.size(), 0);

		int i = 0;
		for (auto& e : db->decorations)
		{
			decorationCheckList.at(i) = true;

			Skill* skill = db->getSkillByIDAndLevel((e.second).skillId, 1);
			if (skill)
			{
				decorationCountList.at(i) = skill->maxLevel;
			}
			else
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::FAILED_TO_GET_SKILL_DURING_CLEAR);
				return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_GET_SKILL_DURING_CLEAR);
			}

			i++;
		}
	}

	return 0;
}

int Settings::loadTemp(std::wifstream & tempFile)
{
	std::wstring line;
	auto& logger = MHW::Logger::getInstance();

	int result = 0;


	// language
	std::getline(tempFile, line);
	MHW::Language language = MHW::Language::ENGLISH;
	result = readLanguage(line, language, "Language: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_LANGUAGE_VAL_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int totalWeaponSlots = 0;
	result = readInt(line, totalWeaponSlots, "Total weapon slots: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_TOTAL_WEAPON_DECO_SLOTS_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int weaponSlot1Size = 0;
	result = readInt(line, weaponSlot1Size, "Weapon slot 1 level: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_WEAPON_SLOT_1_LEVEL_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int weaponSlot2Size = 0;
	result = readInt(line, weaponSlot2Size, "Weapon slot 2 level: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_WEAPON_SLOT_2_LEVEL_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int weaponSlot3Size = 0;
	result = readInt(line, weaponSlot3Size, "Weapon slot 3 level: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_WEAPON_SLOT_3_LEVEL_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);

	// charm display 
	int charmDisplaySetting = MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME;
	if (readValue(line, charmDisplaySetting))
	{
		logger.info("Charm display: " + std::string((charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_NAME) ? "By name" : "By skill name"));
	}
	else
	{
		charmDisplaySetting = MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME;
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_CONVERT_CHARM_DISPLAY_TO_NUM);
	}

	// charm index
	std::getline(tempFile, line);
	int charmIndex = 0;
	result = readInt(line, charmIndex, "Charm index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_CHARM_INDEX_TO_NUM);
	if (result != 0) return result;

	// skill index
	std::getline(tempFile, line);
	int skillIndex = 0;
	result = readInt(line, skillIndex, "Skill index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SKILL_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int skillSize = 0;
	result = readInt(line, skillSize, "Added skills count: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SKILL_SIZE_TO_NUM);
	if (result != 0) return result;

	std::vector<SkillData> tempSkillData;

	if (skillSize)
	{
		for (int i = 0; i < skillSize; ++i)
		{
			std::getline(tempFile, line);
			int skillId = 0;
			result = readInt(line, skillId, std::to_string(i) + ") Skill ID: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SKILL_ID_TO_NUM);
			if (result != 0) return result;

			std::getline(tempFile, line);
			int skillLevel = 0;
			result = readInt(line, skillLevel, std::to_string(i) + ") Skill Level: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SKILL_LEVEL_TO_NUM);
			if (result != 0) return result;

			std::getline(tempFile, line);
			bool skillApplied = false;
			result = readBool(line, skillApplied, std::to_string(i) + ") Skill applied: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SKILL_APPLIED_TO_NUM);
			if (result != 0) return result;

			tempSkillData.push_back(std::move(SkillData(skillId, skillLevel, skillApplied)));
		}
	}

	std::getline(tempFile, line);
	bool highRankSetSkill = true;
	result = readBool(line, highRankSetSkill, "High rank set skill: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_RANK_TO_BOOL);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int setSkillIndex = 0;
	result = readInt(line, setSkillIndex, "Set skill index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int setSkillSize = 0;
	result = readInt(line, setSkillSize, "Added set skills count: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_SIZE_TO_NUM);
	if (result != 0) return result;

	std::vector<SetSkillData> tempSetSkillData;
	if (setSkillSize)
	{
		for (int i = 0; i < setSkillSize; ++i)
		{
			std::getline(tempFile, line);
			int setSkillId = 0;
			result = readInt(line, setSkillId, std::to_string(i) + ") Set skill ID: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_ID_TO_NUM);
			if (result != 0) return result;

			std::getline(tempFile, line);
			int setSkillReqArmorPice = 0;
			result = readInt(line, setSkillReqArmorPice, std::to_string(i) + ") Req armor pices: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_REQ_ARMOR_PIECE_TO_NUM);
			if (result != 0) return result;

			std::getline(tempFile, line);
			bool setSkillApplied = false;
			result = readBool(line, setSkillApplied, std::to_string(i) + ") Set skill applied: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_APPLIED_TO_NUM);
			if (result != 0) return result;

			std::getline(tempFile, line);
			bool highRankSetSkill = false;
			result = readBool(line, highRankSetSkill, std::to_string(i) + ") High rank: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SET_SKILL_RANK_TO_NUM);
			if (result != 0) return result;

			tempSetSkillData.push_back(std::move(SetSkillData(setSkillId, setSkillReqArmorPice, setSkillApplied, highRankSetSkill)));
		}
	}

	std::getline(tempFile, line);
	bool highRankHeadArmor = true;
	result = readBool(line, highRankHeadArmor, "High rank head armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_HEAD_ARMOR_RANK_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool highRankChestArmor = true;
	result = readBool(line, highRankChestArmor, "High rank chest armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_CHEST_ARMOR_RANK_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool highRankArmArmor = true;
	result = readBool(line, highRankArmArmor, "High rank arm armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ARM_ARMOR_RANK_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool highRankWaistArmor = true;
	result = readBool(line, highRankWaistArmor, "High rank waist armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_WAIST_ARMOR_RANK_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool highRankLegArmor = true;
	result = readBool(line, highRankLegArmor, "High rank leg armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_LEG_ARMOR_RANK_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int headArmorIndex = 0;
	result = readInt(line, headArmorIndex, "Head armor index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_HEAD_ARMOR_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int chestArmorIndex = 0;
	result = readInt(line, chestArmorIndex, "chest armor index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_CHEST_ARMOR_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int armArmorIndex = 0;
	result = readInt(line, armArmorIndex, "arm armor index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ARM_ARMOR_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int waistArmorIndex = 0;
	result = readInt(line, waistArmorIndex, "waist armor index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_WAIST_ARMOR_INDEX_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int legArmorIndex = 0;
	result = readInt(line, legArmorIndex, "leg armor index: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_LEG_ARMOR_INDEX_TO_NUM);
	if (result != 0) return result;

	int decoSize = 0;
	std::getline(tempFile, line);
	result = readInt(line, decoSize, "Deco count: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_DECO_TOTAL_SIZE_TO_NUM);
	if (result != 0) return result;

	std::vector<bool> decorationCheckList;
	std::vector<int> decorationCountList;

	decorationCheckList.resize(decoSize, false);
	decorationCountList.resize(decoSize, 0);

	for (int i = 0; i < decoSize; ++i)
	{
		std::getline(tempFile, line);
		bool checked = true;
		result = readBool(line, checked, std::to_string(i) + ") deco: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_DECO_BOOL_TO_NUM);
		if (result != 0) return result;
		decorationCheckList.at(i) = checked;

		std::getline(tempFile, line);
		int count = 0;
		result = readInt(line, count, std::to_string(i) + ") deco count: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_DECO_COUNT_TO_NUM);
		if (result != 0) return result;
		decorationCountList.at(i) = count;
	}

	std::getline(tempFile, line);
	MHW::Gender gender = MHW::Gender::NONE;
	int genderVal = 0;
	if (readValue(line, genderVal))
	{
		if (genderVal == 0 || genderVal == 2)
		{
			gender = MHW::Gender::MALE;
		}
		else
		{
			gender = MHW::Gender::FEMALE;
		}

		logger.info("Gender Val: " + std::to_string(genderVal));
	}
	else
	{
		gender = MHW::Gender::MALE;
		return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_CONVERT_DECO_COUNT_TO_NUM);
	}

	// lr armor
	std::getline(tempFile, line);
	bool allowLowRankArmor = false;
	result = readBool(line, allowLowRankArmor, "Allow low rank armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_LR_ARMOR_TO_NUM);
	if (result != 0) return result;

	// arena armor
	std::getline(tempFile, line);
	bool allowArenaArmor = false;
	result = readBool(line, allowArenaArmor, "Allow arena armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_ARENA_ARMOR_TO_NUM);
	if (result != 0) return result;

	// event armor
	std::getline(tempFile, line);
	bool allowEventArmor = false;
	result = readBool(line, allowEventArmor, "Allow event armor: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_EVENT_ARMOR_TO_BOOL);
	if (result != 0) return result;

	std::getline(tempFile, line);
	int minArmorRarity = 1;
	result = readInt(line, minArmorRarity, "Min armor rarity: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_MIN_ARMOR_RARITY_TO_NUM);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool allowExtraSkills = true;
	result = readBool(line, allowExtraSkills, "Allows extra skill: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_EXTRA_SKILL_TO_BOOL);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool searchFromHigherArmorRarity = true;
	result = readBool(line, searchFromHigherArmorRarity, "Search from higer rarity armors: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SEARCH_FROM_HIGHER_RARIRTY_TO_BOOL);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool allowOverleveledSkills = true;
	result = readBool(line, allowOverleveledSkills, "Allow overleveled skill: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_OVERLEVELED_SKILL_TO_BOOL);
	if (result != 0) return result;

	/*
	std::getline(tempFile, line);
	bool useOnlyMaxLevelCharm = true;
	result = readBool(line, useOnlyMaxLevelCharm, "Only use max level charm: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_ALLOW_ONLY_USE_MAX_LEVEL_CHARM_TO_BOOL);
	if (result != 0) return result;
	*/

	std::getline(tempFile, line);
	bool simplifySearchResult = true;
	result = readBool(line, simplifySearchResult, "Simplify search result: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SIMPLIFY_SEARCH_RESULT_TO_BOOL);
	if (result != 0) return result;

	std::getline(tempFile, line);
	bool showMaxLevel = true;
	result = readBool(line, showMaxLevel, "Shows max level: ", MHW::ERROR_CODE::FAILED_TO_CONVERT_SHOW_MAX_LEVEL_TO_BOOL);
	if (result != 0) return result;

	// copy
	this->language = language;
	this->totalWeaponSlots = totalWeaponSlots;
	this->weaponSlot1Size = weaponSlot1Size;
	this->weaponSlot2Size = weaponSlot2Size;
	this->weaponSlot3Size = weaponSlot3Size;
	this->charmDisplaySetting = charmDisplaySetting;
	this->charmIndex = charmIndex;
	this->skillIndex = skillIndex;
	this->tempSkillData = tempSkillData;
	this->highRankSetSkill = highRankSetSkill;
	this->setSkillIndex = setSkillIndex;
	this->tempSetSkillData = tempSetSkillData;
	this->highRankHeadArmor = highRankHeadArmor;
	this->highRankChestArmor = highRankChestArmor;
	this->highRankArmArmor = highRankArmArmor;
	this->highRankWaistArmor = highRankWaistArmor;
	this->highRankLegArmor = highRankLegArmor;
	this->headArmorIndex = headArmorIndex;
	this->chestArmorIndex = chestArmorIndex;
	this->armArmorIndex = armArmorIndex;
	this->waistArmorIndex = waistArmorIndex;
	this->legArmorIndex = legArmorIndex;
	this->decorationCheckList = decorationCheckList;
	this->decorationCountList = decorationCountList;
	this->gender = gender;
	this->allowLowRankArmor = allowLowRankArmor;
	this->allowArenaArmor = allowArenaArmor;
	this->allowEventArmor = allowEventArmor;
	this->minArmorRarity = minArmorRarity;
	this->allowExtraSkills = allowExtraSkills;
	this->searchFromHigherArmorRarity = searchFromHigherArmorRarity;
	this->allowOverleveledSkills = allowOverleveledSkills;
	//this->useOnlyMaxLevelCharm = useOnlyMaxLevelCharm;
	this->simplifySearchResult = simplifySearchResult;
	this->showMaxLevel = showMaxLevel;

	return 0;
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
		if (skill->id == skillId)
		{
			return true;
		}
	}

	return false;
}

bool Settings::isSetSkillAdded(const int setSkillId, const int groupId, const bool HR)
{
	for (auto setSkill : setSkills)
	{
		if (setSkill->id == setSkillId || setSkill->groupId == groupId)
		{
			if (HR)
			{
				if (setSkill->highRank == true)
				{
					return true;
				}
			}
			else
			{
				if (setSkill->highRank == false)
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

		return ((*iter)->dropdownIndex);
	}
}

int Settings::getSkillOriginalIndexByPos(const int listPos)
{
	//OutputDebugString((L"-- " + std::to_wstring(listPos) + L"\n").c_str());

	// there are 15 skills can be added.
	if (listPos >= MAX_SKILL_COUNT)
	{
		return -1;
	}
	else
	{
		if (listPos >= (int)skills.size())
		{
			return -1;
		}

		auto iter = skills.begin();

		std::advance(iter, listPos);

		return ((*iter)->originalIndex);
	}
}

int Settings::getSetSkillDropdownIndexByPos(const int listPos)
{
	// there is only 2 set skills at a time

	if (listPos == 0)
	{
		return setSkills.front()->dropdownIndex;
	}
	else if (listPos == 1)
	{
		return setSkills.back()->dropdownIndex;
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
		return setSkills.front()->originalIndex;
	}
	else if (listPos == 1)
	{
		return setSkills.back()->originalIndex;
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
		return setSkills.front()->highRank;
	}
	else if (listPos == 1)
	{
		return setSkills.back()->highRank;
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

		for (auto ss : setSkills)
		{
			if (ss->applied)
			{
				result += ss->reqArmorPieces;
			}
		}

		return result;
	}
}

std::string Settings::getLanguagePath()
{
	return getLanguagePath(language);
}

std::string Settings::getLanguagePath(const MHW::Language language)
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
		skills.push_back(skill);
	}
	else
	{
		if (index > (int)skills.size())
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::ADD_SKILL_AT_INDEX_OUT_OF_RANGE);
			return;
		}
		else if (index == (int)skills.size())
		{
			skills.push_back(skill);
		}
		else
		{
			auto iter = skills.begin();
			std::advance(iter, index);

			skills.insert(iter, skill);
		}
	}
}

void Settings::removeAddedSkillAt(const int index)
{
	if (index >= (int)skills.size())
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::REMOVE_SKILL_AT_INDEX_OUT_OF_RANGE);
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
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::REMOVE_SET_SKILL_AT_INDEX_OUT_OF_RANGE);
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
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::GET_SKILL_AT_INDEX_OUT_OR_RANGE);
		return nullptr;
	}
	else
	{
		auto iter = skills.begin();
		std::advance(iter, index);

		return (*iter);
	}
}

SetSkill * Settings::getAddedSetSkillAt(const int index)
{
	if (index == -1) return nullptr;

	if (index >= (int)setSkills.size())
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::GET_SET_SKILL_AT_INDEX_OUT_OR_RANGE);
		return nullptr;
	}
	else
	{
		auto iter = setSkills.begin();
		std::advance(iter, index);

		return (*iter);
	}
}

int Settings::getTotalAddedSkills()
{
	return skills.size();
}

int Settings::getTotalAddedSetSkills()
{
	return setSkills.size();
}

std::wstring Settings::getString(const MHW::StringLiteral e)
{
	return stringLiterals[e];
}

bool Settings::setDecoCount(const int index, const int count)
{
	const int size = (int)decorationCountList.size();

	if (index >= size)
	{
		return false;
	}
	else
	{
		decorationCountList.at(index) = count;
		return true;
	}
}

void Settings::clear()
{
	auto& logger = MHW::Logger::getInstance();

	logger.info("Reseting settings...");

	majorVersion = MHW::VERSION::major;
	minorVersion = MHW::VERSION::minor;
	logger.info("Version: " + std::to_string(majorVersion) + "." + std::to_string(minorVersion));

	//language = MHW::Language::ENGLISH;
	//logger.info("Language: English");

	totalWeaponSlots = 0;
	logger.info("Total weapon deco slots: 0");

	weaponSlot1Size = 1;
	weaponSlot2Size = 1;
	weaponSlot3Size = 1;
	logger.info("First weapon deco level: 1");
	logger.info("Second weapon deco level: 1");
	logger.info("Third weapon deco level: 1");

	charmDisplaySetting = MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME;
	charmIndex = 0;
	logger.info("Charm display setting: By skill name");
	logger.info("Charm index: 0");

	skills.clear();
	tempSkillData.clear();
	skillIndex = 0;
	logger.info("Added skills: None");
	logger.info("Skill index: 0");
	
	highRankSetSkill = true;
	setSkills.clear();
	tempSetSkillData.clear();
	setSkillIndex = 0;
	logger.info("High rank set skill: true");
	logger.info("Added set skills: None");
	logger.info("Set skill index: 0");

	highRankHeadArmor = true;
	highRankChestArmor = true;
	highRankArmArmor = true;
	highRankWaistArmor = true;
	highRankLegArmor = true;
	logger.info("All high rank armors: true");

	headArmorIndex = 0;
	chestArmorIndex = 0;
	armArmorIndex = 0;
	waistArmorIndex = 0;
	legArmorIndex = 0;
	logger.info("All armors index: 0");

	/*
	int i = 0;
	for (auto& e : db->decorations)
	{
		decorationCheckList.at(i) = true;
		
		Skill* skill = db->getSkillByIDAndLevel((e.second).skillId, 1);
		if (skill)
		{
			decorationCountList.at(i) = skill->maxLevel;
		}
		else
		{
			logger.errorCode(MHW::ERROR_CODE::FAILED_TO_GET_SKILL_DURING_CLEAR);
			decorationCountList.at(i) = 0;
		}
		i++;
	}
	*/

	/*
	for (int i = 0; i < (int)decorationCheckList.size(); ++i)
	{
		decorationCheckList.at(i) = true;
	}
	*/
	decorationCheckList.clear();
	decorationCountList.clear();
	logger.info("deco check list & count list empty");

	gender = MHW::Gender::MALE;
	logger.info("Gender: Male");
	allowLowRankArmor = false;
	logger.info("Disallow LR armor");
	allowArenaArmor = false;
	logger.info("Disallow arena armor");
	allowEventArmor = false;
	logger.info("Disallow event armor");

	minArmorRarity = 5;
	logger.info("Min armor rarity to search: 5");
	allowExtraSkills = true;
	logger.info("Allow extra skill");
	searchFromHigherArmorRarity = true;
	logger.info("Searches from higher armor rarity");
	allowOverleveledSkills = true;
	logger.info("Allows overleved skills");
	
	//useOnlyMaxLevelCharm = false;
	//logger.info("use only max level charm");

	simplifySearchResult = true;
	logger.info("Simplifies search result");

	showMaxLevel = true;
	logger.info("Show max level");

	//stringLiterals.clear();
}

void Settings::reset(Database * db)
{
	clear();
	load(db);
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
		for (auto skill : skills)
		{
			OutputDebugString((L"Skill #" + std::to_wstring(i) + L"\n").c_str());
			skill->print(true);
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
	OutputDebugString((L"Charm display type: " + std::wstring((charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_NAME) ? L"By name" : L"By skill name")).c_str());

	// set skill rank
	OutputDebugString((L"Set skill rank: " + std::wstring((highRankSetSkill) ? L"High Rank" : L"Low Rank")).c_str());

	// armor rank
	OutputDebugString((L"Head armor rank: " + std::wstring((highRankHeadArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Chest armor rank: " + std::wstring((highRankChestArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Arm armor rank: " + std::wstring((highRankArmArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Waist armor rank: " + std::wstring((highRankWaistArmor) ? L"High Rank" : L"Low Rank")).c_str());
	OutputDebugString((L"Leg armor rank: " + std::wstring((highRankLegArmor) ? L"High Rank" : L"Low Rank")).c_str());
}
