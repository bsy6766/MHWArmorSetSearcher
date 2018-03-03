#ifndef ARMOR_SET_H
#define ARMOR_SET_H

#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "Decoration.h"

class Database;
class Charm;
class Armor;
class SetSkill;
class Skill;
class Settings;

namespace MHW
{
	/**
	*	@class ArmorSet
	*	@brief Simple struct contains information of single armor set.
	*/
	class ArmorSet
	{
	public:
		// flag that keep tracks armor set inspection
		bool hasAnyArmor;		// true if this armor has 'any with deco' armor
		//bool setSkillPassedWithArmor;	// true if this armor set passed set skill check only with 5 piece armors.
		//bool setSkillPassedWithCharm;	// true if this armor set passed set skill with charm.
		bool setSkillPassed;
		bool skillPassed;
	public:
		// Constructor
		ArmorSet();

		// default destructor
		~ArmorSet() = default;

		// id counter
		static int idCounter;
	public:
		// id of armorset. 
		int id;

		// Armor ptr
		Armor* headArmor;
		Armor* chestArmor;
		Armor* armArmor;
		Armor* waistArmor;
		Armor* legArmor;
		
		// Charm index
		//int charmIndex;
		Charm* charm;
		
		// Sum of skills
		std::unordered_map<int/*skill id*/, int/*total level*/> skillLevelSums;
		// Sum of extra skills
		std::unordered_map<int/*skill id*/, int/*total level*/> extraSkillLevelSums;
		// Sum of decoration skills
		std::unordered_map<int/*skill id*/, int/*total level*/> decoSkillLevelSums;

		// sum of set skill armor pieces
		std::unordered_map<int/*set skill group id*/, int/*total armor pieces*/> lowRankSetSkillArmorPieceSums;
		std::unordered_map<int/*set skill group id*/, int/*total armor pieces*/> extraLowRankSetSkillArmorPieceSums;
		std::vector<int/*set skil id*/> activatedLowRankSetSkills;
		std::unordered_map<int/*set skill group id*/, int/*total armor pieces*/> highRankSetSkillArmorPieceSums;
		std::unordered_map<int/*set skill group id*/, int/*total armor pieces*/> extraHighRankSetSkillArmorPieceSums;
		std::vector<int/*set skill id*/> activatedHighRankSetSkills;

		// true if skill level sum exactly matches the skill level that user queried. Flase if any is greater. Lower means failed so doesn't used in this case.
		bool perfectSkillMatch;

		// Total decoration slot by size. 
		std::array<int/*deco size. 0 means none.*/, 3/*total 1 ~ 3 decos*/> headArmorDecoSlots;
		std::array<int/*deco size. 0 means none.*/, 3/*total 1 ~ 3 decos*/> chestArmorDecoSlots;
		std::array<int/*deco size. 0 means none.*/, 3/*total 1 ~ 3 decos*/> armArmorDecoSlots;
		std::array<int/*deco size. 0 means none.*/, 3/*total 1 ~ 3 decos*/> waistArmorDecoSlots;
		std::array<int/*deco size. 0 means none.*/, 3/*total 1 ~ 3 decos*/> legArmorDecoSlots;

		// Total decoration slots by size (from armor + weapon)
		int totalSize1DecorationSlots;
		int totalSize2DecorationSlots;
		int totalSize3DecorationSlots;

		// Total decoration slots used by size
		int totalUsedSize1DecorationSlot;
		int totalUsedSize2DecorationSlot;
		int totalUsedSize3DecorationSlot;

		// Sum of used decorations
		std::unordered_map<int/*deco id*/, int/*total decos used*/> usedDecorations;

		void clearSums();
		void initSums(std::vector<Skill*>& filterSkills, const std::vector<SetSkill*>& reqLRSetSkills, const std::vector<SetSkill*>& reqHRSetSkills);
		void countSums();
		void countSums(Armor* armor);
		// Check if there is any set skill activated and stores if there's any.
		//void checkActviatedSetSkills(Database* db, const std::vector<SetSkill*>& reqLRSetSkills, const std::vector<SetSkill*>& reqHRSetSkills);
		void addSkillLevelToSum(const int skillID, const int skillLevel);
		void addCharmSkillLevelSums();
		void addCharmSkillLevelSumByOne(Charm* charm);
		void addArmorPieceToSum(const int setSkillID, const int reqArmorPieces, const bool HR);
		
		// set armor
		void setHeadrmor(Armor* armor);
		void setChestArmor(Armor* armor);
		void setArmArmor(Armor* armor);
		void setWaistArmor(Armor* armor);
		void setLegArmor(Armor* armor);

		// copy deco slot form armor
		void copyDecoSlots(const std::vector<int>& src, std::array<int, 3>& dest);

		// Count total deco slots by size
		void addWeaponDecoSlots(const std::vector<int>& weaponDecoSlots, const int count);
		void countTotalDecoSizeBySizeFromArmors();
		void countDecoSizeBySize(const std::array<int, 3>& decoSlots);
		bool canDecorationsFit(const int size1, const int size2, const int size3);
		void updateUsedDecoCount(const int size1, const int size2, const int size3);
		void clearDecoData();

		bool hasEnoughArmorPieces(const int setSkillGroupID, const int reqArmorPieces);

		// get deco string
		std::wstring getItemDecoSizeAsStr(const std::array<int, 3>& decoSlots);

		// Generate string output of armor set
		std::wstring toResultStr(Database* db, Settings* setting);
		std::wstring getArmorSkillsStr(Database* db, Settings* setting);
		std::wstring getExtraRmorSkillsStr(Database* db, Settings* setting);
		std::wstring getDecoSkillsStr(Database* db, Settings* setting);
		std::wstring getTotalSkillsStr(Database* db, Settings* setting);
		std::wstring getSetSkillStr(Database* db, Settings* setting);
	};
}

#endif