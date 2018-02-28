#ifndef DATABASE_H
#define DATABASE_H

#include <map>
#include <unordered_map>
#include <unordered_set>

#include "Charm.h"
#include "Skill.h"
#include "Armor.h"
#include "Decoration.h"
#include "ErrorCode.h"

// Foward delcaration
class Settings;

#define ANY_ARMOR_ID_START 9000

/**
*	@class Database
*	@brief Simple class that manages all the data for charm, skill, set skill and decoration
*/
class Database
{
public:
	// Constructor
	Database();

	~Database() = default;

	// Data

	/**
	*	Charms
	*	Key represent the unique ID for each charm.
	*/
	std::map<int, Charm> charms;

	/**
	*	Skills
	*	Key represent the unique ID for each skill
	*/
	std::map<int, Skill> skills;

	/**
	*	Low rank set skill
	*	Key represent the unique ID for each low rank set skill
	*/
	std::map<int, SetSkill> lowRankSetSkills;
	
	/**
	*	High rank set skill
	*	Key represent the unique ID for each high rank set skill
	*/
	std::map<int, SetSkill> highRankSetSkills;

	/**
	*	For corner cases.
	*	Cache Bulwark charm, Uragaan Protection set skill, Guard up skill
	*/
	Charm* bulwarkCharm;
	Skill* guardUpSkill;
	SetSkill* uragaanProtectionSetSkill;

	/**
	*	Head armors
	*	Key represent the unique ID for each head armor
	*/
	std::map<int, Armor> headArmors;
	int totalLowRankHeadArmors;
	int totalHighRankHeadArmors;
	std::map<int, Armor> anyHeadArmors;

	/**
	*	Chest armors
	*	Key represent the unique ID for each chest armor
	*/
	std::map<int, Armor> chestArmors;
	int totalLowRankChestArmors;
	int totalHighRankChestArmors;
	std::map<int, Armor> anyChestArmors;

	/**
	*	Arm armors
	*	Key represent the unique ID for each arm armor
	*/
	std::map<int, Armor> armArmors;
	int totalLowRankArmArmors;
	int totalHighRankArmArmors;
	std::map<int, Armor> anyArmArmors;

	/**
	*	Waist armors
	*	Key represent the unique ID for each waist armor
	*/
	std::map<int, Armor> waistArmors;
	int totalLowRankWaistArmors;
	int totalHighRankWaistArmors;
	std::map<int, Armor> anyWaistArmors;

	/**
	*	Leg armors
	*	Key represent the unique ID for each leg armor
	*/
	std::map<int, Armor> legArmors;
	int totalLowRankLegArmors;
	int totalHighRankLegArmors;
	std::map<int, Armor> anyLegArmors;

	/**
	*	Decorations
	*	Key represent the unique ID for each decoration
	*/
	std::map<int, Decoration> decorations;

	// user decoration
	std::map<int, MyDecoration> userDecoration;

	// Look up tables

	// LUT to convert skill id to string skill name.
	std::unordered_map<int, std::wstring> skillIdToNameLUT;	
	// LUT to convert string skill name to skill id.
	std::unordered_map<std::wstring, int> skillNameToIdLUT;
	// LUT to convert skill id to skill key
	std::unordered_map<int, int> skillIdToKeyLUT;
	// LUT to convert set skill id to string set name. (i.e. id -> Odogaron Mastert 2)
	std::unordered_map<int, std::wstring> setSkillIdToNameLUT;	
	// LUT to convert string set name with req armor pieces to skill id. (i.e. Odogaron Mastery 2 -> id)
	std::unordered_map<std::wstring, int> setSkillNameToIdLUT;	
	// LUT to convert string set skill name (group name not each set pieces) to setSkillId (i.ie. gId <- Odogaron Mastery)
	std::unordered_map<std::wstring, int> setSkillGroupNameToIdLUT;
	// LUt to check if set skill id has another set skill in same group
	std::unordered_map<int, bool> secondSetSkillCheckLUT;
	// LUT to convert skill group id to set skill decoration id
	std::unordered_map<int, int> setSkillGroupIdToDecoSetSkillIDLUT;
	// LUT to convert skill id to decorations
	std::unordered_map<int, int> skillIdToDecorationIdLUT;
	std::unordered_map<int, int> setSkillIdToDecorationIdLUT;

	

	//====== not used? =======
	// LUT to convert skill from deco that gives set skills to set skill group id. only for high rank
	std::unordered_map<int, int> decoSetSkillIDToSetSkillGroupIDLUT;
	// LUT to convert skill from deco that gives set skill to actual set skill. Only for highrnk set skills for now (patch 1.06)
	std::unordered_map<int, int> decoSetSkillIDToSetSkillIDLUT;
	// vice versa
	std::unordered_map<int, int> setSkillIDToDecoSetSkillIDLUT;
	// Dropdown list row index to skill key. This will be handy since dropdown list changes as user add/remove skills or set skills
	std::unordered_map<int/*index*/, int/*skills key*/> skillDropdownListToKey;
	std::unordered_map<int/*index*/, int/*LR set skill key*/> lowRankSetSkillDropdownListToKey;
	std::unordered_map<int/*index*/, int/*HR set skill key*/> highRankSetSkillDropdownListToKey;
	// ========================

	// initialization
	int init(Settings* setting);
	int initCharmData(Settings * setting);
	int initSkillData(Settings * setting);
	int initSetSkillData(Settings * setting);
	void initDecoSetSkillData();
	int initLowRankSetSkillData(std::ifstream& setSkillData);
	int initHighRankSetSkillData(std::ifstream& setSkillData, Settings* setting);
	int initArmorData(Settings * setting);
	int initArmor(Armor& armor, std::vector<std::string>& split, const int defense, const int rarity, const int id, const int setId, const int setSkiiId, const int secondSetSkillId, const int setGroupId, const std::string& setName, const MHW::Gender gender);
	int initArmorSkillData(Armor& armor, std::vector<std::string>& split, int& skillCount);
	int initArmorDecorationData(Armor& armor, const int decoCountIndex, std::vector<std::string>& split);
	int initDecorationData(Settings * setting);

	// Get set skill ID by set skill dropdown index
	int getSetSkillIdByDropdownIndex(const int dropdownIndex, const bool HR);

	// Get set skill description by set skill id
	std::wstring getSetSkillDescriptionById(const int id, const bool HR);

	// Get skill id by skill dropdown index
	int getSkillIdByDropdownIndex(const int dropdownIndex);

	// Get skill description by skill id
	std::wstring getSkillDescriptionById(const int id);

	// Get skill level description by skill id
	std::wstring getSkillLevelDescriptionById(const int id);

	// Get skill level by id
	int getSkillLevelById(const int id);

	// Get skill name by id
	std::wstring getSkillNameById(const int id);

	// Get set skill name by id
	std::wstring getSetSkillNameById(const int id, const bool HR);
	std::wstring getSetSkillGroupNameById(const int id, const bool HR);

	// Get skill by skill id and level
	Skill* getSkillByIDAndLevel(const int id, const int level);

	// Get set skill by id
	SetSkill* getSetSkillByID(const int id, const bool HR);

	// Get decoration by skill id
	Decoration* getDecorationBySkillID(const int skillId);

	// Get head armor by id
	Armor* getHeadArmorByID(const int id);
	Armor* getChestArmorByID(const int id);
	Armor* getArmArmorByID(const int id);
	Armor* getWaistArmorByID(const int id);
	Armor* getLegArmorByID(const int id);
	Armor* getArmorByDropdownListIndex(std::map<int, Armor>& armors, const int index);

	// Get charm by id
	Charm* getCharmByID(const int id);
	Charm* getNextLevelCharm(Charm* charm);

	// Get 
	//void getArmorBySkill(std::map<int, Armor>& armors, std::vector<int>& queriedArmors, std::unordered_set<int>& skillFilter, std::unordered_set<int>& decoSetSkillFilter, std::unordered_set<int>& lowRankSetSkillFilter, std::unordered_set<int>& highRankSetSkillFilter, const bool LR, const bool HR);
	void getArmorBySkill(std::map<int, Armor>& armors, std::vector<Armor*>& queriedArmors, std::unordered_set<int>& skillFilter, std::unordered_set<int>& decoSetSkillFilter, std::unordered_set<int>& lowRankSetSkillFilter, std::unordered_set<int>& highRankSetSkillFilter, const bool LR, const bool HR, const MHW::Gender genderSetting);

	void getAllCharmsBySkill(std::vector<int>& filterCharmIndices, std::unordered_set<int>& filterSkills, std::unordered_set<int>& filterSetSkills);
	void getAllMinLevelCharmsBySkill(std::vector<Charm*>& filterCharms, std::unordered_set<int>& skillFilters, std::unordered_set<int>& decoSetSkillFilters);

	void initDefaultAnyArmors();
	void initDefaultAnyArmor(std::map<int, Armor>& anyArmors);
	//void getAnyArmor(std::vector<int>& filterArmors, std::map<int, Armor>& anyArmors, const bool LR, const bool HR);
	void getAnyArmor(std::vector<Armor*>& filterArmors, std::map<int, Armor>& anyArmors, const bool LR, const bool HR);

	bool doesSkillCanComeFromDeco(const int skillId);

	// Check if there is same kind of any armor
	bool hasSameTypeOfAnyArmor(Armor& armor, const std::map<int, Armor>& anyArmors);
};

#endif