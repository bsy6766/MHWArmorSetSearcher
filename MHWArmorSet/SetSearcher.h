#ifndef SET_SEARCHER_H
#define SET_SEARCHER_H

#include <thread>
#include <vector>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <CommCtrl.h>

#include "Filter.h"
#include "ArmorSet.h"

class Database;
class Decoration;

#define WM_FROM_WORKER_THREAD (WM_USER+0)
#define MW_SEARCH_ABORTED (WM_USER+1)

namespace MHW
{
	/**
	*	@class SetSearcher
	*	@brief Simple class creates worker thread that searches desired armor set.
	*/
	class SetSearcher
	{
	public:
		enum class State
		{
			IDLE = 0,	// not searching
			SEARCHING,	// Searching
			FINISHED,
			ABORTED,
			STOP_REQUESTED,		// Stop working.
			STOPPED,
			ERR,		// Error from worker thread
		};

		enum class SearchState
		{
			READY = 0,
			LF_ARMOR_COMBINATION,
			LF_HEAD,
			LF_CHEST,
			LF_ARM,
			LF_WAIST,
			LF_LEG,
			LF_CHARM,
			LF_DECORATION,
		};
	private:
		// filter to use on search
		Filter filter;

		// state
		State state;

		// iter counter.
		int iterCount;

		// counter
		static float searchCounter;
		static float totalCounter;
		static int prevPrecentage;

		// flag
		std::atomic<bool> running;
		std::atomic<bool> abort;

		// state mutex
		std::mutex stateMutex;

		// work mutex
		std::mutex workMutex;

		// cb
		std::condition_variable cv;

		// result mutex
		std::mutex resultMutex;

		// armor counter
		int curHeadArmorCounter;
		int curChestArmorCounter;
		int curArmArmorCounter;
		int curWaistArmorCounter;
		int curLegArmorCounter;

		// armor size
		int headArmorSize;
		int chestArmorSize;
		int armArmorSize;
		int waistArmorSize;
		int legArmorSize;

		// Currently searched armor sets
		std::vector<ArmorSet> searchedArmorSets;

		// search armor set
		void searchArmorSet(Database* db);

		// serach helpder
		void searchArmorSet(Database* db, SearchState searchState, MHW::ArmorSet* curArmorSet);
		
		// add search result
		void addNewArmorSet(MHW::ArmorSet* newArmorSet);

		// check armor set
		bool checkNewArmorSet(MHW::ArmorSet* newArmorSet);

		// Check skill sum
		bool checkSkillLevelSums(MHW::ArmorSet* newArmorSet);

		// check if skill has overleveled skill
		bool hasOverLeveledSkill(MHW::ArmorSet* newArmorSet);

		// check set skill 
		bool checkSetSkill(MHW::ArmorSet* newArmorSet);
		bool checkSetSkillWithOutGuardUp(MHW::ArmorSet* newArmorSet);

		// Check if filtered set skill is guard up (Uragaan protection 3)
		bool isSetSkillGuardUp(Database* db);
		
		void initAndCountSums(MHW::ArmorSet* newArmorSet);

		void getNewArmorCombination(MHW::ArmorSet* newArmorSet);

		void step();
	public:
		// Constructor
		SetSearcher();

		// Default destructor
		~SetSearcher();

		// main window 
		HWND mainHWND;
		// pb
		HWND progressBarHWND;

		// initialize work
		void init(Database* db);
		
		// Joins thread
		bool join();

		// stop all work
		void stop();

		// workder thread
		std::thread* workerThread;

		// search armor
		void search(Filter filter);

		// work
		void work(Database* db);

		// get state. uses stateLock
		State getState();

		// set state. uses stateLock
		void setState(const State state);

		// notify thread
		void notifyDoneCopyingResult();

		// get result size
		int getResultSize();

		// query results
		void queryResults(std::vector<MHW::ArmorSet>& armorSets);

		// send message to main thread
		void sendMsg(const bool finished);

		void abortSearching();
	};
}

#endif