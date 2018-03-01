#ifndef SETTINGS_H
#define SETTINGS_H

#include "Charm.h"
#include "Skill.h"
#include "Armor.h"

#include <map>
#include <list>
#include <unordered_map>

/**
*	@class Settings
*	@brief Simple class that keep tracks current setting for armor set search, such as total weapon decoration slots.
*/
class Settings
{
public:
	// const
	static const int CHARM_DISPLAY_BY_NAME;
	static const int CHARM_DISPLAY_BY_SKILL_NAME;

public:
	// constructor
	Settings();

	// Default destructor
	~Settings() = default;

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

	// Added skills
	std::list<Skill*> skills;

	// ============== 

	// set skill rank setting
	bool highRankSetSkill;
	// Added set skills
	std::list<SetSkill*> setSkills;

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

	// ============== 

	// string literals
	std::unordered_map<MHW::StringLiteral, std::wstring> stringLiterals;
	// ============== 

	bool init();
	bool initStringLiterals();
	// read temporary file
	bool readTemp();
	// read and store
	bool readValue(const std::string str, int& dest);

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