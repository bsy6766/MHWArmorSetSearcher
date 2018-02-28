#ifndef CHARMS_H
#define CHARMS_H

#include <string>

/*
enum class CHARM
{
	NONE = 0,			// Not using charm
	ANY,				// Use any charm
	POISON_RES_1,
	POISON_RES_2,
	POISON_RES_3,
	ATTACK_1,
	ATTACK_2,
	ATTACK_3,
	DEFENSE_BOOST_1,
	DEFENSE_BOOST_2,
	DEFENSE_BOOST_3,
	PARALYSIS_RES_1,
	PARALYSIS_RES_2,
	PARALYSIS_RES_3,
	SLEEP_RES_1,
	SLEEP_RES_2,
	SLEEP_RES_3,
	STUN_RES_1,
	STUN_RES_2,
	STUN_RES_3,
	WINDPROFF_1,
	WINDPROFF_2,
	WINDPROFF_3,
	HEALTH_BOOST_1,
	HEALTH_BOOST_2,
	HEALTH_BOOST_3,
	RECOVERY_UP_1,
	RECOVERY_UP_2,
	RECOVERY_UP_3,
	FIRE_RES_1,
	FIRE_RES_2,
	FIRE_RES_3,
	WATER_RES_1,
	WATER_RES_2,
	WATER_RES_3,

};

enum class CHARM_ID
{
	POISON_RES,
	ATTACK,
	DEFENSE_BOOST,
	PARALYSIS_RES,
	SLEEP,
	STUN,
	WINDPROOF,
	HEALTH_BOOST,
	RECOVERY_UP,
	FIRE_RES,
	WATER_RES,
	THUNDER_RES,
	FIRE_ATTACK,
	WATER_ATTACK,
	THUNDER_ATTACK,
	POISON_ATTACK,
	PARALYSIS_ATTACK,
	SLEEP_ATTACK,
	SLUGGER,
	STAMINA_THIEF,
	ARTILLERY,
	HUNGER_RES,
	GUARD,
	WIDE_RANGE,
	ITEM_PROLONGER,
	SPEED_EATING,
	DIVINE_BLESSING,
};
*/

class Charm
{
public:
	// Constructor
	Charm();

	// Default destructor
	~Charm() = default;

	// numeric id for each charm
	int id;

	// Charm's IGN
	std::wstring name;

	// Charm's skill name
	std::wstring skillName;

	// Additional skill (Some charms have 2 skills)
	std::wstring secondSkillName;

	// skill id.
	int skillId;

	// second skill id
	int secondSkillId;

	// set skill
	bool setSkill;

	// second set skill
	bool secondSetSkill;

	// Level of charm
	int level;

	// Max level of charm
	int maxLevel;

	// Index in combobox
	int dropdownIndex;

	// Check if it has second skill
	bool hasSecondSkill();

	// debug
	void print(const bool detail);
};

#endif