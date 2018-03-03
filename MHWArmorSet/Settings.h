#ifndef SETTINGS_H
#define SETTINGS_H

#include "Charm.h"
#include "Skill.h"
#include "Armor.h"
#include "GlobalEnums.h"
#include "ErrorCode.h"

#include <map>
#include <list>
#include <unordered_map>

class Database;

/**
*	@class Settings
*	@brief Simple class that keep tracks current setting for armor set search, such as total weapon decoration slots.
*/
class Settings
{
public:
	struct SkillData
	{
	public:
		int skillId;
		int skillLevel;
		bool applied;

		SkillData() : skillId(-1), skillLevel(-1), applied(false) { }
		SkillData(int skillId, int skillLevel, bool applied) : skillId(skillId), skillLevel(skillLevel), applied(applied) { }
	};
	
	struct SetSkillData
	{
	public:
		int setSkillId;
		int reqArmorPieces;
		bool applied;
		bool highRank;

		SetSkillData() : setSkillId(-1), reqArmorPieces(-1), applied(false), highRank(false) { }
		SetSkillData(int setSkillId, int reqArmorPieces, bool applied, bool highRank) : setSkillId(setSkillId), reqArmorPieces(reqArmorPieces), applied(applied), highRank(highRank) { }
	};
public:
	// constructor
	Settings();

	// Default destructor
	~Settings() = default;

	// ============== 

	int majorVersion;
	int minorVersion;

	// ============== 

	// language
	MHW::Language language;

	// ============== 

	// Total number of decoration that can be equipped to weapon
	int totalWeaponSlots;
	// Size of first decoration in weapon
	int weaponSlot1Size;
	// Size of second decoration in weapon
	int weaponSlot2Size;
	// size of third decoration in weapon
	int weaponSlot3Size;

	// ============== 

	// Charm display setting
	int charmDisplaySetting;
	// Charm
	int charmIndex;

	// ============== 

	// skill index
	int skillIndex;
	// Added skills
	std::list<Skill*> skills;
	// For initialization. Temporary storage
	std::vector<SkillData> tempSkillData;
	// ============== 

	// set skill rank setting
	bool highRankSetSkill;
	// set skill index
	int setSkillIndex;
	// Added set skills
	std::list<SetSkill*> setSkills;
	// For initialization. Temporary storage
	std::vector<SetSkillData> tempSetSkillData;
	// ============== 

	// armor rank
	bool highRankHeadArmor;
	bool highRankChestArmor;
	bool highRankArmArmor;
	bool highRankWaistArmor;
	bool highRankLegArmor;

	// armor index
	int headArmorIndex;
	int chestArmorIndex;
	int armArmorIndex;
	int waistArmorIndex;
	int legArmorIndex;

	// ============== 

	// decoration checked list. Index means the id of decoration
	std::vector<bool> decorationCheckList;

	// ============== 

	// current gender
	MHW::Gender gender;
	// option that allows low rank armor
	bool allowLowRankArmor;
	// option that allows arena armr
	bool allowArenaArmor;
	// option that sets minor armor rarity. 5 (highrank) by default.
	int minArmorRarity;
	// if true, rejects armor sets that has extra skills
	bool allowExtraSkills;
	// if true, searches from higher armor rank. If false, searches from lower armor ran.
	bool searchFromHigherArmorRarity;
	// allow over leveled skills in armorset
	bool allowOverleveledSkills;
	// use only max level charm
	bool useOnlyMaxLevelCharm;

	// ============== 

	// string literals
	std::unordered_map<MHW::StringLiteral, std::wstring> stringLiterals;
	// ============== 

	int init();
	int initStringLiterals();
	int readTemp();
	bool readValue(const std::wstring str, int& dest);
	int readLanguage(const std::wstring& line, MHW::Language& language, const std::string& log, const MHW::ERROR_CODE errCode);
	int readInt(const std::wstring& line, int& dest, const std::string& log, const MHW::ERROR_CODE errCode);
	int readBool(const std::wstring& line, bool& dest, const std::string& log, const MHW::ERROR_CODE errCode);
	void saveTemp();
	int load(Database* db);
	int loadSkills(Database* db);
	int loadSetSkills(Database* db);

	// version 1.0
	int loadTemp(std::wifstream& tempFile);

	// str to num
	int stoi(const std::string str);

	// Check if unique skill is added
	bool isSkillAdded(const int skillId);

	// Check if unique set skill is added
	bool isSetSkillAdded(const int setSkillId, const int groupId, const bool HR);

	// get set drop down index by set skill pos
	int getSkillDropdownIndexByPos(const int listPos);
	int getSkillOriginalIndexByPos(const int listPos);

	// get set skill drop down index by set skill pos
	int getSetSkillDropdownIndexByPos(const int listPos);
	int getSetSkillOriginalIndexByPos(const int listPos);
	bool getSetSkillRankByPos(const int listPos);

	// Get number of armor pieces to fulfill desired set skills
	int getTotalReqArmorPieces();

	// Get language path
	std::string getLanguagePath();
	std::string getLanguagePath(const MHW::Language language);

	void addSkillAt(const int index, Skill* skill);
	// Remove skill at position
	void removeAddedSkillAt(const int index);

	// remove set skill at position
	void removeAddedSetSkillAt(const int index);

	// Get added skill at index
	Skill* getAddedSkillAt(const int index);
	SetSkill* getAddedSetSkillAt(const int index);

	int getTotalAddedSkills();
	int getTotalAddedSetSkills();

	std::wstring getString(const MHW::StringLiteral e);

	// Clear all setting to default
	void clear();

	// for debug
	void print(std::map<int, Charm>& charms);
	void printPrefs();
};

#endif