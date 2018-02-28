#ifndef DECORATION_H
#define DECORATION_H

#include <string>
#include <vector>

class Database;

class Decoration
{
public:
	// Constructor
	Decoration();

	// Default destructor
	~Decoration() = default;

	// unique decoration id
	int id;

	// name of deco
	std::wstring name;

	// decoration size. (1~3)
	int size;

	// rarity
	int rarity;

	// skill id that this decoration gives
	int skillId;

	// true if this decoration gives set skill
	bool setSkill;
	// set skill id that this decoration gives. Only used when setSkill is true
	int setSkillId;
	// if setSkill is true, must have rank
	bool highRankSetSkill;

	std::wstring getItemWStr(Database* db);
	std::vector<std::wstring> getStr(Database* db);

	void print();
};

class MyDecoration
{
public:
	// Constructor
	MyDecoration();

	// Default destructor
	~MyDecoration() = default;

	// deco
	Decoration deco;

	// count
	int count;
};

#endif