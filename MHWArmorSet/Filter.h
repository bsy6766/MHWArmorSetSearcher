#ifndef FILTER_H
#define FILTER_H

class Armor;
class Database;
class Charm;
class SetSkill;
class Skill;

namespace MHW
{
	/**
	*	Simple filter contains multiple informations for armor set search.
	*/
	struct Filter
	{
	public:
		// Constructor to initialize values
		Filter();

		// default detructor
		~Filter() = default;

		// Total number of decorations in weapon. Max 3.
		int totalWeaponDecorations;

		// Size of each decoration in weapon. Max 3.
		std::vector<int> weaponDecoSizes;

		// Charm
		//std::vector<int> charmIndices;
		std::vector<Charm*> charms;
		bool userPickedCharm;

		// Skills. Max 15.
		std::vector<Skill*> reqSkills;

		// Set skills. Contains group id. Max 2 for each rank. Total 2 for both.
		std::vector<SetSkill*> reqLRSetSkills;
		std::vector<SetSkill*> reqHRSetSkill;

		// Armors. These armors are filtered by skills.
		std::vector<Armor*> headArmors;
		std::vector<Armor*> chestArmors;
		std::vector<Armor*> armArmors;
		std::vector<Armor*> waistArmors;
		std::vector<Armor*> legArmors;

		// Decorations. Index means decoration id.
		bool hasDecorationToUse;
		std::vector<bool> usableDecorations;

		// Check if it has guard up charm
		bool hasGuardUpCharm(Database* db);

		// debug
		void print(Database* db);
		void print(Database* db, const std::vector<Armor*>& armors, const std::wstring v);
	};
}

#endif