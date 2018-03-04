#ifndef ARMOR_H
#define ARMOR_H

#include <vector>
#include <array>
#include <string>

#include "Decoration.h"
#include "GlobalEnums.h"

class Database;

class Armor
{
public:
	// Constructor
	Armor();

	// Default destructor
	~Armor() = default;

	// id for unique armor set
	int setId;

	// id for unique armor per type (head, chest, etc...).
	int id;

	// dropdown list index
	int dropdownListIndex;

	// name of armor set
	std::wstring setName;

	// name of armor
	std::wstring name;

	// armor point
	int defense;

	// rarity
	int rarity;

	// Rank
	bool highRank;

	// Gender
	MHW::Gender gender;

	// arena
	bool arenaArmor;
	// event
	bool eventArmor;

	// skills. Can have up to 2 without decoration
	std::vector<int/*skill id*/> skills;
	std::vector<int/*skill id*/> skillLevels;

	// Set skill. (i.e. Odogaron punish draw id)
	int setSkillId;
	// Incase armor can have 2 set skill (i.e. Odogaron protective polish id)
	int secondSetSkillId;
	// set skills's group id (i.e. Odogaron id)
	int setGroupId;

	// Decoration slots. Value means the size of slot.
	std::vector<int> decorationSlots;

	// get armor dropdown list str
	std::wstring getDropdownListItemStr(Database* db);
	std::wstring getItemDecorationSizeAsStr();

	bool isAnyArmor();
	//bool hasSkill(const bool allowExtra)

	// debug
	void print(Database* db);
};

#endif