#ifndef SKILL_H
#define SKILL_H

#include <string>

class Skill
{
public:
	// Constructor
	Skill();

	// Default destructor
	~Skill() = default;

	// name
	std::wstring name;

	// unique id for each skill
	int id;

	// index on skill amp
	int originalIndex;
	// temporary index on skill map
	int dropdownIndex;

	// level
	int level;
	// max level
	int maxLevel;

	// true if this skill is only from deocration (i.e. Poisoncoat)
	bool onlyFromDeco;
	// true if this skill is from set skill. (i.e. Sharp (Odogaron 4)
	bool fromDecoSetSkill;

	// true if this skill is applied and being used in search. false to exclude from search
	bool applied;

	// description
	std::wstring description;
	// description for each level
	std::wstring levelDescription;

	// debug
	void print(const bool detail);
};

class SetSkill
{
public:
	// Constructor
	SetSkill();

	// Default destructor
	~SetSkill() = default;

	// name
	std::wstring name;
	// set skill name
	std::wstring skillName;

	// unique id for each set skill (parent)
	int id;
	// unique id for set group
	int groupId;

	// index on skill amp
	int originalIndex;
	// temporary index on skill map
	int dropdownIndex;

	// required armor piece count
	int reqArmorPieces;

	// true if this set skill is applied and being used in search. false to exclude from search
	bool applied;
	// rank
	bool highRank;
	// Has sibling set skill(i.e. Odogaron's punish draw and protective polish)
	bool hasSibling;
	// Ptr to sibling
	SetSkill* sibling;

	// skilld escription
	std::wstring description;

	// debug
	void print();
};

#endif