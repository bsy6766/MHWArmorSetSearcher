#include "stdafx.h"
#include "Database.h"
#include "SetSearcher.h"

#include "Charm.h"
#include "Skill.h"
#include "Armor.h"
#include "Decoration.h"
#include "Utility.h"
#include "Const.h"
#include "Logger.h"

MHW::SetSearcher::SetSearcher()
	: workerThread(nullptr)
	, state(State::IDLE)
	, running(false)
	, abort(false)
	, previouslyQueriedLastResultIndex(-1)
	, iterCount(0)
{}

MHW::SetSearcher::~SetSearcher()
{
	searchedArmorSets.clear();
}

void MHW::SetSearcher::init(Database * db)
{
	state = State::IDLE;
	running.store(true);

	OutputDebugString(L"Main thread creating worker thread.\n");
	workerThread = new std::thread(std::bind(&SetSearcher::work, this, db));

	std::wstringstream ss;
	ss << workerThread->get_id();
	OutputDebugString((L"Worker thread " + ss.str() + L" created\n").c_str());
}

void MHW::SetSearcher::stop()
{
	setState(State::STOP_REQUESTED);
	cv.notify_all();
}

void MHW::SetSearcher::search(Filter filter)
{
	// called by main thread
	this->filter = filter;
	this->iterCount = 0;

	setState(State::SEARCHING);

	{
		// scope lock
		std::unique_lock<std::mutex> qLock(resultMutex);

		// clear previously searched set
		searchedArmorSets.clear();
	}

	cv.notify_all();
}

void MHW::SetSearcher::work(Database * db)
{
	if (db == nullptr)
	{
		OutputDebugString(L"DB is nullptr.\n");
		state = State::IDLE;
		return;
	}
	else
	{
		while (running)
		{
			// scope lock
			std::unique_lock<std::mutex> wLock(workMutex);

			State curState = getState();

			if (curState == State::IDLE)
			{
				// idle. wait
				OutputDebugString(L"Running and IDLE. waiting for notification\n");
				cv.wait(wLock);
				OutputDebugString(L"CV Notified\n");

				if (getState() == State::STOP_REQUESTED)
				{
					setState(State::STOPPED);
					break;
				}
			}

			// Check skill
			if (filter.reqSkills.empty())
			{
				OutputDebugString(L"Skill is empty. Should not happen.\n");
				setState(State::ERR);
				return;
			}

			// search 
			OutputDebugString(L"Search started\n");
			
			// search!
			auto start = Utility::Time::now();
			searchArmorSet(db);
			auto end = Utility::Time::now();

			OutputDebugString((L"t: " + Utility::Time::toMilliSecondString(start, end)).c_str());

			// CV notified
			OutputDebugString(L"Search finished\n");

			if (abort.load())
			{
				abort.store(false);

				setState(State::ABORTED);

				if (mainHWND)
				{
					SendMessage(mainHWND, WM_FROM_WORKER_THREAD, 0, 0);
				}

				OutputDebugString(L"Search aborted\n");
			}
			else
			{
				setState(State::FINISHED);

				sendMsg(true);
			}

			auto state = getState();

			while (state == State::FINISHED || state == State::ABORTED)
			{
				cv.wait(wLock);
			}

			OutputDebugString(L"Main thread copied all search result. back to idle\n");
		}
	}
}

void MHW::SetSearcher::searchArmorSet(Database * db)
{
	MHW::ArmorSet::idCounter = 1;
	previouslyQueriedLastResultIndex = -1;

	MHW::ArmorSet* dummy = new MHW::ArmorSet();
	searchArmorSet(db, SearchState::LF_HEAD, dummy);
	delete dummy;
}

void MHW::SetSearcher::searchArmorSet(Database * db, SearchState searchState, MHW::ArmorSet * curArmorSet)
{
	if (searchState == SearchState::LF_HEAD)
	{
		if (abort.load()) return;

		// Armor search starts from the head armor.
		
		if (filter.headArmors.empty())
		{
			// there is no head armor
			curArmorSet->setHeadrmor(nullptr);

			// Look for chest
			searchArmorSet(db, SearchState::LF_CHEST, curArmorSet);
		}
		else
		{
			// There is at 
			for (auto headArmor : filter.headArmors)
			{
				if (abort.load()) return;

				// Set head armor
				curArmorSet->setHeadrmor(headArmor);

				// Look for chest
				searchArmorSet(db, SearchState::LF_CHEST, curArmorSet);
			}
		}

		// Finished
	}
	else if (searchState == SearchState::LF_CHEST)
	{
		if (abort.load()) return;

		// Looking for chest.
		
		if (filter.chestArmors.empty())
		{
			// there is no head armor
			curArmorSet->setChestArmor(nullptr);

			// Look for arm
			searchArmorSet(db, SearchState::LF_ARM, curArmorSet);
		}
		else
		{
			// There is multiple chest armors
			for (auto chestArmor : filter.chestArmors)
			{
				if (abort.load()) return;

				// Set chest armor
				curArmorSet->setChestArmor(chestArmor);
				
				// Look for arm
				searchArmorSet(db, SearchState::LF_ARM, curArmorSet);
			}
		}
	}
	else if (searchState == SearchState::LF_ARM)
	{
		if (abort.load()) return;

		// Looking for arm
		
		if (filter.armArmors.empty())
		{
			// there is no waist armor
			curArmorSet->setArmArmor(nullptr);

			// Look for waist
			searchArmorSet(db, SearchState::LF_WAIST, curArmorSet);
		}
		else
		{
			// There is multiple arm armors
			for (auto armArmor : filter.armArmors)
			{
				if (abort.load()) return;

				// Set arm armor
				curArmorSet->setArmArmor(armArmor);

				// Look for waist
				searchArmorSet(db, SearchState::LF_WAIST, curArmorSet);
			}
		}
	}
	else if (searchState == SearchState::LF_WAIST)
	{
		if (abort.load()) return;

		// Looking for waist
		
		if (filter.waistArmors.empty())
		{
			// there is no waist armor
			curArmorSet->setWaistArmor(nullptr);

			// Look for leg
			searchArmorSet(db, SearchState::LF_LEG, curArmorSet);
		}
		else
		{
			// There is multiple waist armors
			for (auto waistArmor : filter.waistArmors)
			{
				if (abort.load()) return;

				// set waist armor
				curArmorSet->setWaistArmor(waistArmor);

				// Look for leg
				searchArmorSet(db, SearchState::LF_LEG, curArmorSet);
			}
		}
	}
	else if (searchState == SearchState::LF_LEG)
	{
		if (abort.load()) return;

		// Looking for leg

		if (filter.legArmors.empty())
		{
			// there is no waist armor
			curArmorSet->setLegArmor(nullptr);

			// Look for leg
			searchArmorSet(db, SearchState::LF_CHARM, curArmorSet);
		}
		else
		{
			// Check all leg armor
			for (auto legArmor : filter.legArmors)
			{
				if (abort.load()) return;

				// set index
				curArmorSet->setLegArmor(legArmor);

				// clear sum
				curArmorSet->clearSums();

				// init sum (Not extra skills) to 0
				curArmorSet->initSums(filter.reqSkills, filter.reqLRSetSkills, filter.reqHRSetSkill);

				// Count skill sums and set skill's req armor pieces
				curArmorSet->countSums(db);

				// clear flags
				curArmorSet->skillPassed = false;
				curArmorSet->setSkillPassed = false;

				// Check if armor set meets set skill requirement
				bool setSkillResult = checkSetSkill(curArmorSet);

				if (setSkillResult)
				{
					// Passed set skill check
					curArmorSet->setSkillPassed = true;

					// Check if armor set can be added with only armors
					bool result = checkNewArmorSet(curArmorSet);

					if (result)
					{
						// Armor set found with only 5 piece armors and fulfills set skill requirements
						curArmorSet->skillPassed = true;

						// Set charm index as None
						curArmorSet->charm = nullptr;

						//  Add to result
						addNewArmorSet(curArmorSet);
						// Don't check charm because if armor set need charm, it would be failed.
	
						sendMsg(false);
					}
					else
					{
						// Failed to pass armor skill test. However, yet this armor set passed set skill check, so we only need to check charm and deco
						curArmorSet->skillPassed = false;

						//  Try with charm
						searchArmorSet(db, SearchState::LF_CHARM, curArmorSet);
					}

					// End of successed set skill check.
				}
				else
				{
					/*
					// set skill check failed.
					// User selected at least 1 set skill.
					// This usually means that new armor set is not qualified to fulfill set skill requirements. However, there is corner case.
					// Case 1: Set skill is Guard up. In this case, we need to handle corner case. Guard up can be activated by charm or decoration.
					// Case 2: Set skill is something else than guard up. In this case, it's failed because user asked for specific set skill
					curArmorSet->setSkillPassed = false;

					// Corner case: If set skill is Uragaan Protection 3, check if there is a charm that it gives this set skill
					bool u3 = isSetSkillGuardUp(db);

					if (u3)
					{
						// One of set skill is Guard Up (Uragaan Protection 3)
						// This mean that user selected Uragaan protection 3 on set skill section.
						// Which means only want to filter armor set that has Uragaan 3 pieces.
						// Case 1: Has guard up charm. In this case, select guard up charm and check skills.
						// Case 2: Doesn't have guard up charm. In this case, user selected none or didn't add guard up on skill section.

						// Check if there is a charm that has guard up
						bool hasBulwarkCharm = filter.hasGuardUpCharm(db);

						if (hasBulwarkCharm)
						{
							// Case 1

							// This armor set doesn't have enough pieces for Guard Up, but has charm that can activate set skill.

							// Check set skill without guard up
							if (checkSetSkillWithOutGuardUp(curArmorSet))
							{
								// pass without guard up. Since we have guard up charm, check skill
								curArmorSet->setSkillPassed = true;
							}
							else
							{
								// Can't pass even with guard up charm.
								curArmorSet->setSkillPassed = false;
								// Check decoration
								searchArmorSet(db, SearchState::LF_DECORATION, curArmorSet);
							}
						}
						else
						{
							// Case 2. Doesn't have guard up charm. It's either None or didn't select Guard up skill.
							// Because set skill check failed, this armor set can't get guard up.
							curArmorSet->setSkillPassed = false;

							return;
						}
					}
					else
					{
						// There is no guard up set skill. 
						// This mean that there is no charm to cover up set skill like guard up.
						// So it's fail
						curArmorSet->setSkillPassed = false;

						return;
					}
					*/
					// There was some set skill added and current armor pieces didn't meet the requirement.
					return;
				}
			}
		}
	}
	else if (searchState == SearchState::LF_CHARM)
	{
		if (abort.load()) return;

		// Looking for charm.

		// At this point, current armor set failed to be added because of lacking skills

		// if charm is none
		//		Nothing to add. Check decoration
		// if charm is user picked
		//		if there's 1 charm
		//			Use this charm only. Check with this charm.
		//				if success, add
		//				else, Check decoration
		//		else, there can't be more than 1 charm on use rpicked
		// else charm is not user picked
		//		iter all charms(min level)
		//			if success, add.
		//			else
		//				check decoration
		//				Then, try with higher level of charm if exists.
		//				if success, add
		//				

		if (filter.charms.empty())
		{
			// Charm is 'None'. 
			if (filter.hasDecorationToUse)
			{
				// There is decoration to use.

				// Set decoration to None
				curArmorSet->charm = nullptr;

				// There is nothing to add and armor check failed. Check decoartriotn
				searchArmorSet(db, SearchState::LF_DECORATION, curArmorSet);
			}
			else
			{
				// There is no decoration that can be used.
				return;
			}
		}
		else
		{
			// Charm is not 'none'
			if (filter.userPickedCharm)
			{
				// user picked specific charm
				if (filter.charms.size() == 1)
				{
					// valid. There can be only 1 user picked charm
					curArmorSet->charm = filter.charms.front();

					// add charm skill to skill level sum
					curArmorSet->addCharmSkillLevelSums(db);

					// Check skill sum
					bool result = checkNewArmorSet(curArmorSet);

					if (result)
					{
						// Armor set found with 5 piece armors and charm!
						curArmorSet->skillPassed = true;
						
						//Add to result.
						addNewArmorSet(curArmorSet);

						sendMsg(false);
						// Doesn't have to roll back skill sums and skill level sum because new leg armor will clear it
					}
					else
					{
						// Failed. 
						curArmorSet->skillPassed = false; 

						if (filter.hasDecorationToUse)
						{
							// Has decoration to use. Try with decorations
							searchArmorSet(db, SearchState::LF_DECORATION, curArmorSet);
						}
						else
						{
							// There is no decoration to use.
							return;
						}
					}
				}
				else
				{
					MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SS_USER_PICKED_CHARM_IS_NOT_SIZE_1);
					return;
				}
			}
			else
			{
				// User didn't picked charm. So it's 'Any'. All charms are queired with min level
				for (auto curCharm : filter.charms)
				{
					if (abort.load()) return;

					// set index
					curArmorSet->charm = curCharm;

					// copy current skill sum
					auto skillLevelSumsCopy = curArmorSet->skillLevelSums;
					auto extraSkillLevelSumsCopy = curArmorSet->extraSkillLevelSums;
					
					if (curCharm)
					{
						// valid charm
						int i = curCharm->level;
						int len = curCharm->maxLevel;

						// try all levels
						for (; i <= len; ++i)
						{
							// Add charm skill level to sum by 1
							curArmorSet->addCharmSkillLevelSumByOne(curArmorSet->charm);

							// Check skill sum
							bool result = checkNewArmorSet(curArmorSet);

							if (result)
							{
								// Armor set found with 5 piece armors and charm! 
								curArmorSet->skillPassed = true;

								// Add to result.
								addNewArmorSet(curArmorSet);

								sendMsg(false);

								// Next level charm might have a chance. keep going
							}
							else
							{
								// Failed. 
								curArmorSet->skillPassed = false;

								if (filter.hasDecorationToUse)
								{
									// Has decoration to use. Try with decorations
									searchArmorSet(db, SearchState::LF_DECORATION, curArmorSet);
								}
								// Else, there is no decoration to use

								// Next level charm might have a chance. keep going	
							}

							// Fix charm index with correct level
							Charm* nextLevelCharm = db->getNextLevelCharm(curArmorSet->charm);
							curArmorSet->charm = nextLevelCharm;
						}
					}
					else
					{
						// error: Charm deata deoesn't exists
						MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SS_CHARM_IS_NULLPTR);
						continue;
					}


					// roll back skill level to point where it was only counted with 5 armor pieces
					curArmorSet->skillLevelSums = skillLevelSumsCopy;
					curArmorSet->extraSkillLevelSums = extraSkillLevelSumsCopy;
					curArmorSet->charm = nullptr;
				}
			}
		}
	}
	else if (searchState == SearchState::LF_DECORATION)
	{
		if (abort.load()) return;

		if (filter.hasDecorationToUse == false)
		{
			// No decoration to use. Fail safe.
			return;
		}

		// sum all decoration slots from armors
		curArmorSet->countTotalDecoSizeBySizeFromArmors();

		// sum all decoration slots from weapon
		curArmorSet->addWeaponDecoSlots(filter.weaponDecoSizes, filter.totalWeaponDecorations);

		// Get diff btw current skill level sum and required skill level .
		const int skillFilterSize = (int)filter.reqSkills.size();

		std::unordered_map<int/*skill id*/, int/*remaining skill level*/> remainingSkillLevels;

		auto& logger = MHW::Logger::getInstance();

		for (auto reqSkill : filter.reqSkills)
		{
			auto find_it = curArmorSet->skillLevelSums.find(reqSkill->id);

			if (find_it == curArmorSet->skillLevelSums.end())
			{
				logger.errorCode(MHW::ERROR_CODE::SS_SKILL_LEVEL_SUM_DOES_NOT_EXIST);
			}
			else
			{
				// Before we check skill level, see if current skill is from deco. 
				// If so, see if it can be activated by set skill.
				// If so, don't add to remaining skill
				// Else, add to remaining skill
				if (reqSkill->fromDecoSetSkill)
				{
					// get deco
					Decoration* deco = db->getDecorationBySkillID(reqSkill->id);
					if (deco)
					{
						// valid deco
						SetSkill* setSkill = db->getSetSkillByID(deco->setSkillId, true);
						if (setSkill)
						{
							// valid set skill

							// Check if this set skill from deco can be replaced by armor set skill
							if (curArmorSet->hasEnoughArmorPieces(setSkill->groupId, setSkill->reqArmorPieces))
							{
								// yes it can be replaced. Consider this decoration is filled.
								continue;
							}
							else
							{
								// No this can't be replaced. However, if set skill was Uragaan Protection 3, there might be a chance that
								// Bulwark charm can replace it instead.
								
								// Check if deco is Guar dup
								if (deco->skillId == db->guardUpSkill->id)
								{
									// it's guard up.

									// Check if current armor set has bulwark charm
									if (curArmorSet->charm)
									{
										// has charm
										if (curArmorSet->charm->id == db->bulwarkCharm->id)
										{
											// it's using bulwark charm
											continue;
										}
										// Else, it's not bulwark charm Add this deco to remaiing skills.
									}
									// Else, doesn't have charm. Can't replace it. Add this deco to remainig skills.
								}
								// Else, it's not guard up. This means that we need to add this deco to remaining skills
							}
						}
						else
						{
							logger.errorCode(MHW::ERROR_CODE::SS_CANT_GET_DECO_SET_SKILL_BY_SET_SKILL_ID);
						}
					}
					else
					{
						// error
						logger.errorCode(MHW::ERROR_CODE::SS_CANT_GET_DECO_BY_SKILL_ID);
					}
				}
				// Else, required skill isn't from deco.

				// get current skill level sum
				const int curSkillLevelSum = find_it->second;

				if (curSkillLevelSum < reqSkill->level)
				{
					// current skill level sum is less than required.
					remainingSkillLevels[find_it->first] = ((reqSkill->level) - curSkillLevelSum);
				}
				// else, current armor set already has enough skill
			}
		}

		if (remainingSkillLevels.empty())
		{
			// Case: When we are checking decorations, it's usually when 5 armor pieces and charm wasn't enough to 
			//		meet requirements. So it usually expects to have remaining skill levels. However, there is 1 case
			//		where this is empty.
			//		It's when skill is from decoration (i.e. Bow Charge Plus (Legiana 4 piece bonus)) and it's activated
			//		by the set skill. With this case, we simple consider this as valid new armor set

			curArmorSet->updateUsedDecoCount(0, 0, 0);
			curArmorSet->decoSkillLevelSums = remainingSkillLevels;

			// Add
			addNewArmorSet(curArmorSet);

			sendMsg(false);

			// Clear deco data
			curArmorSet->clearDecoData();
		}
		else
		{
			// Count total decoration slots to fill remaining skills

			// Count
			int totalSize1DecoSlotReq = 0;
			int totalSize2DecoSlotReq = 0;
			int totalSize3DecoSlotReq = 0;

			for (auto& e : remainingSkillLevels)
			{
				//get deco
				Decoration* deco = db->getDecorationBySkillID(e.first);
				
				if (deco)
				{
					// valid deco. Check if this deco is usable
					if (filter.usableDecorations.at(deco->id))
					{
						// can use this decoration
						if (deco->size == 1)
						{
							totalSize1DecoSlotReq += remainingSkillLevels[e.first];
						}
						else if (deco->size == 2)
						{
							totalSize2DecoSlotReq += remainingSkillLevels[e.first];
						}
						else if (deco->size == 3)
						{
							totalSize3DecoSlotReq += remainingSkillLevels[e.first];
						}
					}
					else
					{
						// Can't use this decoration. Can't fill remaining skill with this decoration. Failed.
						return;
					}
				}
				else
				{
					// Deco not found. Maybe not implemented in game yet. Can be added later.
					// Whatever the case is, this armor set is fail.
					// patch(1.06)
					return;
				}
			}
			
			if (totalSize1DecoSlotReq == 0 && totalSize2DecoSlotReq == 0 && totalSize3DecoSlotReq == 0)
			{
				// huh? this shouldn't happen.
				OutputDebugString(L"Total required deco size slots are all 0\n");
			}

			bool result = curArmorSet->canDecorationsFit(totalSize1DecoSlotReq, totalSize2DecoSlotReq, totalSize3DecoSlotReq);

			if (result)
			{
				// Success!
				curArmorSet->updateUsedDecoCount(totalSize1DecoSlotReq, totalSize2DecoSlotReq, totalSize3DecoSlotReq);
				curArmorSet->decoSkillLevelSums = remainingSkillLevels;

				// Add
				addNewArmorSet(curArmorSet);

				sendMsg(false);
				
				// Clear deco data
				curArmorSet->clearDecoData();
			}
		}
	}
}

bool MHW::SetSearcher::checkNewArmorSet(MHW::ArmorSet* newArmorSet)
{
	// Either set skill wasn't on search filter or condition met.
	if (checkSkillLevelSums(newArmorSet))
	{
		// Skills are met as well
		return true;
	}
	else
	{
		// Failed
		return false;
	}
}

bool MHW::SetSearcher::checkSkillLevelSums(MHW::ArmorSet * newArmorSet)
{
	const int skillSize = filter.reqSkills.size();

	// Sum of skill levels perfectly fits as user queried. 
	bool perfectSkillMatch = true;

	for(auto skill : filter.reqSkills)
	{
		const int reqLevel = skill->level;
		const int curLevel = newArmorSet->skillLevelSums[skill->id];

		// todo: check if skillLevelSums have index (filter.skills.at(i)). Should have it though.
		if (curLevel < reqLevel)
		{
			// doesn't qualify.
			newArmorSet->perfectSkillMatch = false;
			return false;
		}
		else if (curLevel == reqLevel)
		{
			// perfect match
			continue;
		}
		else if (curLevel > reqLevel)
		{
			// Current armor set's skill level sum is higher than queried level.
			perfectSkillMatch = false;
		}
		// Else, nothing
	}

	if (perfectSkillMatch)
	{
		newArmorSet->perfectSkillMatch = true;
	}

	return true;
}

bool MHW::SetSearcher::checkSetSkill(MHW::ArmorSet * newArmorSet)
{
	bool result = true;

	// Check if there is set skill
	if (!filter.reqLRSetSkills.empty())
	{
		// Has low rank
		for (auto setSkill : filter.reqLRSetSkills)
		{
			const int curArmorPiece = newArmorSet->lowRankSetSkillArmorPieceSums[setSkill->groupId];

			if (curArmorPiece < setSkill->reqArmorPieces)
			{
				// doesn't qualify
				result = false;
			}
			else
			{
				newArmorSet->activatedLowRankSetSkills.push_back(setSkill->id);
			}
		}
	}

	if (!filter.reqHRSetSkill.empty())
	{
		// Has high rank
		for (auto setSkill : filter.reqHRSetSkill)
		{
			const int curArmorPiece = newArmorSet->highRankSetSkillArmorPieceSums[setSkill->groupId];

			if (curArmorPiece < setSkill->reqArmorPieces)
			{
				// doesn't qualify
				result = false;
			}
			else
			{
				newArmorSet->activatedHighRankSetSkills.push_back(setSkill->id);
			}
		}
	}

	return result;
}

bool MHW::SetSearcher::checkSetSkillWithOutGuardUp(MHW::ArmorSet * newArmorSet)
{
	return false;
}

bool MHW::SetSearcher::isSetSkillGuardUp(Database* db)
{
	// Only high rank
	if (filter.reqHRSetSkill.empty())
	{
		return false;
	}
	else
	{
		const int size = (int)filter.reqHRSetSkill.size();

		if (size >= 1 && filter.reqHRSetSkill.at(0)->groupId == db->uragaanProtectionSetSkill->groupId)
		{
			return true;
		}
		
		if (size == 2 && filter.reqHRSetSkill.at(1)->groupId == db->uragaanProtectionSetSkill->groupId)
		{
			return true;
		}

		return false;
	}
}

void MHW::SetSearcher::addNewArmorSet(MHW::ArmorSet * newArmorSet)
{
	// scope lock
	std::unique_lock<std::mutex> qLock(resultMutex);

	searchedArmorSets.push_back(MHW::ArmorSet());
	ArmorSet& asRef = searchedArmorSets.back();

	asRef.id = MHW::ArmorSet::idCounter;
	MHW::ArmorSet::idCounter++;

	asRef.headArmor = newArmorSet->headArmor;
	asRef.chestArmor = newArmorSet->chestArmor;
	asRef.armArmor = newArmorSet->armArmor;
	asRef.waistArmor = newArmorSet->waistArmor;
	asRef.legArmor = newArmorSet->legArmor;

	asRef.charm = newArmorSet->charm;

	asRef.headArmorDecoSlots = newArmorSet->headArmorDecoSlots;
	asRef.chestArmorDecoSlots = newArmorSet->chestArmorDecoSlots;
	asRef.armArmorDecoSlots = newArmorSet->armArmorDecoSlots;
	asRef.waistArmorDecoSlots = newArmorSet->waistArmorDecoSlots;
	asRef.legArmorDecoSlots = newArmorSet->legArmorDecoSlots;

	asRef.totalSize1DecorationSlots = newArmorSet->totalSize1DecorationSlots;
	asRef.totalSize2DecorationSlots = newArmorSet->totalSize2DecorationSlots;
	asRef.totalSize3DecorationSlots = newArmorSet->totalSize3DecorationSlots;

	asRef.totalUsedSize1DecorationSlot = newArmorSet->totalUsedSize1DecorationSlot;
	asRef.totalUsedSize2DecorationSlot = newArmorSet->totalUsedSize2DecorationSlot;
	asRef.totalUsedSize3DecorationSlot = newArmorSet->totalUsedSize3DecorationSlot;

	asRef.skillLevelSums = newArmorSet->skillLevelSums;
	asRef.extraSkillLevelSums = newArmorSet->extraSkillLevelSums;
	asRef.decoSkillLevelSums = newArmorSet->decoSkillLevelSums;

	asRef.perfectSkillMatch = newArmorSet->perfectSkillMatch;

	asRef.lowRankSetSkillArmorPieceSums = newArmorSet->lowRankSetSkillArmorPieceSums;
	asRef.extraLowRankSetSkillArmorPieceSums = newArmorSet->extraLowRankSetSkillArmorPieceSums;
	asRef.activatedLowRankSetSkills = newArmorSet->activatedLowRankSetSkills;

	asRef.highRankSetSkillArmorPieceSums = newArmorSet->highRankSetSkillArmorPieceSums;
	asRef.extraHighRankSetSkillArmorPieceSums = newArmorSet->extraHighRankSetSkillArmorPieceSums;
	asRef.activatedHighRankSetSkills = newArmorSet->activatedHighRankSetSkills;
}

MHW::SetSearcher::State MHW::SetSearcher::getState()
{
	State result;
	{
		std::unique_lock<std::mutex> sLock(stateMutex);
		result = state;
	}
	return result;
}

void MHW::SetSearcher::setState(const State state)
{
	std::unique_lock<std::mutex> sLock(stateMutex);
	this->state = state;
}

void MHW::SetSearcher::notifyDoneCopyingResult()
{
	// back to idle
	setState(State::IDLE);

	cv.notify_all();
}

int MHW::SetSearcher::getResultSize()
{
	// scope lock
	std::unique_lock<std::mutex> qLock(resultMutex);

	return searchedArmorSets.size();
}

void MHW::SetSearcher::queryResults(std::vector<MHW::ArmorSet>& armorSets)
{
	// scope lock
	std::unique_lock<std::mutex> qLock(resultMutex);

	int size = armorSets.size();

	armorSets = std::move(searchedArmorSets);
}

void MHW::SetSearcher::sendMsg(const bool finished)
{
	if (finished)
	{
		if (mainHWND)
		{
			SendMessage(mainHWND, WM_FROM_WORKER_THREAD, 0, 0);
		}
	}
	else
	{
		iterCount++;

		if (iterCount > MHW::CONSTS::SETSEARCHER_ITER_TRESHOLD)
		{
			iterCount = 0;

			if (mainHWND)
			{
				SendMessage(mainHWND, WM_FROM_WORKER_THREAD, 0, 0);
			}
		}
	}
}

void MHW::SetSearcher::abortSearching()
{
	abort.store(true);
}

bool MHW::SetSearcher::join()
{
	if (workerThread)
	{
		if (workerThread->joinable())
		{
			std::wstringstream ss;
			ss << workerThread->get_id();
			OutputDebugString((L"Worker thread " + ss.str() + L" joins\n").c_str());

			workerThread->join();
			delete workerThread;
			workerThread = nullptr;

			return true;
		}
		else
		{
			OutputDebugString(L"Worker thread not joinable yet\n");

			return false;
		}
	}
	else
	{
		return true;
	}
}