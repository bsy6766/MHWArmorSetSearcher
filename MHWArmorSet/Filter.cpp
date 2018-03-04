#include "stdafx.h"
#include "Filter.h"
#include "Database.h"
#include "Skill.h"


MHW::Filter::Filter()
	: totalWeaponDecorations(0)
	, userPickedCharm(false)
	, hasDecorationToUse(false)
	, allowOverleveledSkill(false)
{}

bool MHW::Filter::hasGuardUpCharm(Database* db)
{
	if (charms.empty())
	{
		return false;
	}
	else
	{
		for (auto charm : charms)
		{
			if (charm->setSkill)
			{
				if (charm->skillId == db->setSkillGroupNameToIdLUT[L"Uragaan Protection"])
				{
					return true;
				}
			}
		}

		return false;
	}
}

void MHW::Filter::print(Database* db)
{
	OutputDebugString(L"Filter detials...\n");
	OutputDebugString((L"Total weapon decorations: " + std::to_wstring(totalWeaponDecorations) + L"\n").c_str());

	for (int i = 0; i < totalWeaponDecorations; ++i)
	{
		OutputDebugString((L"Weapon decoration #" + std::to_wstring(i) + L" size: " + std::to_wstring(weaponDecoSizes.at(i)) + L"\n").c_str());
	}

	int skillSize = (int)reqSkills.size();
	OutputDebugString((L"Total skill selected: " + std::to_wstring(skillSize) + L"\n").c_str());

	for (auto skill : reqSkills)
	{
		if (skill)
		{
			skill->print(true);
		}
	}

	int lrSetSkillSize = (int)reqLRSetSkills.size();
	OutputDebugString((L"Total low rank set skill selected: " + std::to_wstring(lrSetSkillSize) + L"\n").c_str());

	for (auto setSkill : reqLRSetSkills)
	{
		if (setSkill)
		{
			setSkill->print();
		}
	}
	
	int hrSetSkillSize = (int)reqHRSetSkill.size();
	OutputDebugString((L"Total high rank set skill selected: " + std::to_wstring(hrSetSkillSize) + L"\n").c_str());

	for (auto setSkill : reqHRSetSkill)
	{
		if (setSkill)
		{
			setSkill->print();
		}
	}

	const int charmSize = charms.size();
	if (charmSize == 0)
	{
		OutputDebugString(L"No charm selected\n");
	}
	else if (charmSize == 1)
	{
		if (userPickedCharm)
		{
			OutputDebugString(L"Specific charm selected.\n");
		}
		else
		{
			OutputDebugString(L"Charm queried.\n");
		}

		charms.front()->print(true);
	}
	else
	{
		OutputDebugString(L"Mutiple charm queired\n");
		for(auto charm : charms)
		{
			charm->print(true);
		}
	}

	OutputDebugString(L"Filtered armors...\n");

	print(db, headArmors, L"head");
	print(db, chestArmors, L"chest");
	print(db, armArmors, L"arm");
	print(db, waistArmors, L"waist");
	print(db, legArmors, L"leg");
	/*
	const int headArmorSize = (int)headArmors.size();
	OutputDebugString((L"Total head armors: " + std::to_wstring(headArmorSize) + L"\n").c_str());
	for (int i = 0; i < headArmorSize; ++i)
	{
		OutputDebugString((L"Head armor id: " + std::to_wstring(headArmors.at(i)->id) + L"\n").c_str());
		
		if (headArmors.at(i) >= ANY_ARMOR_ID_START)
		{
			auto find_it = db->anyHeadArmors.find(headArmors.at(i));
			if (find_it == db->anyHeadArmors.end())
			{
				OutputDebugString(L"Error: Failed to get any head armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
		else
		{
			auto find_it = db->headArmors.find(headArmors.at(i));
			if (find_it == db->headArmors.end())
			{
				OutputDebugString(L"Error: Failed to get head armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
	}

	const int chestArmorSize = (int)chestArmors.size();
	OutputDebugString((L"Total chest armors: " + std::to_wstring(chestArmorSize) + L"\n").c_str());
	for (int i = 0; i < chestArmorSize; ++i)
	{
		OutputDebugString((L"chest armor id: " + std::to_wstring(chestArmors.at(i)->id) + L"\n").c_str());

		if (chestArmors.at(i) >= ANY_ARMOR_ID_START)
		{
			auto find_it = db->anyChestArmors.find(chestArmors.at(i));
			if (find_it == db->anyChestArmors.end())
			{
				OutputDebugString(L"Error: Failed to get any head armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
		else
		{
			auto find_it = db->chestArmors.find(chestArmors.at(i));
			if (find_it == db->chestArmors.end())
			{
				OutputDebugString(L"Error: Failed to get chest armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
	}

	const int armArmorSize = (int)armArmors.size();
	OutputDebugString((L"Total arm armors: " + std::to_wstring(armArmorSize) + L"\n").c_str());
	for (int i = 0; i < armArmorSize; ++i)
	{
		OutputDebugString((L"arm armor id: " + std::to_wstring(armArmors.at(i)->id) + L"\n").c_str());

		if (armArmors.at(i) >= ANY_ARMOR_ID_START)
		{
			auto find_it = db->anyArmArmors.find(armArmors.at(i));
			if (find_it == db->anyArmArmors.end())
			{
				OutputDebugString(L"Error: Failed to get any head armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
		else
		{
			auto find_it = db->armArmors.find(armArmors.at(i));
			if (find_it == db->armArmors.end())
			{
				OutputDebugString(L"Error: Failed to get arm armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
	}

	const int waistArmorSize = (int)waistArmors.size();
	OutputDebugString((L"Total waist armors: " + std::to_wstring(waistArmorSize) + L"\n").c_str());
	for (int i = 0; i < waistArmorSize; ++i)
	{
		OutputDebugString((L"waist armor id: " + std::to_wstring(waistArmors.at(i)->id) + L"\n").c_str());

		if (waistArmors.at(i) >= ANY_ARMOR_ID_START)
		{
			auto find_it = db->anyWaistArmors.find(waistArmors.at(i));
			if (find_it == db->anyWaistArmors.end())
			{
				OutputDebugString(L"Error: Failed to get any waist armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
		else
		{
			auto find_it = db->waistArmors.find(waistArmors.at(i));
			if (find_it == db->waistArmors.end())
			{
				OutputDebugString(L"Error: Failed to get waist armor data\n");
			}
			else
			{
				(find_it->second).print(db);
			}
		}
	}
	*/
}

void MHW::Filter::print(Database* db, const std::vector<Armor*>& armors, const std::wstring v)
{

	const int size = (int)armors.size();
	OutputDebugString((L"Total " + v + L" armors: " + std::to_wstring(size) + L"\n").c_str());
	for (int i = 0; i < size; ++i)
	{
		OutputDebugString((v + L" armor id: " + std::to_wstring(armors.at(i)->id) + L"\n").c_str());

		armors.at(i)->print(db);
	}
}

