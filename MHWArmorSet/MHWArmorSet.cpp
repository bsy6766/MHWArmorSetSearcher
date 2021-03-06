// MHWArmorSet.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MHWArmorSet.h"
#include "Settings.h"
#include "Database.h"
#include "Const.h"
#include "Utility.h"
#include "Logger.h"

//#pragma comment( lib, "Shcore.lib" )

using namespace MHW::CONSTS;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR    lpCmdLine, _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//SetProcessDpiAwareness(PROCESS_DPI_AWARENESS::PROCESS_PER_MONITOR_DPI_AWARE);

	// Initialize global strings

	//LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MHWARMORSET, szWindowClass, MAX_LOADSTRING);

	windowTitle = L"Monster Hunter World Armor Set Searcher";

	windowTitle += L" v" + std::to_wstring(MHW::VERSION::major) + L"." + std::to_wstring(MHW::VERSION::minor);

	MyRegisterClass(hInstance);

	LoadStringW(hInstance, IDC_MHWARMORSET_SEARCH_RESULT, szSearchResultWindowClass, MAX_LOADSTRING);
	MyRegisterSearchResultClass(hInstance);

	// create font
	defaultFont = CreateFont(
		16,		// height
		0,		// width 
		0,	// escapement (angle)
		0,	// orientation
		FW_NORMAL, // weight
		FALSE, // italic
		FALSE, // underline
		FALSE, // straight out
		DEFAULT_CHARSET, // default (english)
		OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS,
		CLEARTYPE_QUALITY,
		DEFAULT_PITCH,
		TEXT("Segoe UI"));
	//TEXT("Batang Regular"));

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow, windowTitle))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MHWARMORSET));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MHWARMORSET));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MHWARMORSET);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

ATOM MyRegisterSearchResultClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MHWARMORSET));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = 0;
	wcex.lpszClassName = szSearchResultWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow, const std::wstring title)
{
	// Store instance handle in our global variable
	hInst = hInstance;
	/*
	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC = ICC_LINK_CLASS;
	InitCommonControlsEx(&InitCtrlEx);

	*/

	// logger
	auto& logger = MHW::Logger::getInstance();

	logger.info("Initializing...");

	mainHWND = CreateWindowW(szWindowClass, title.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, MIN_WINDOW_W, MIN_WINDOW_H, nullptr, nullptr, hInstance, nullptr);

	if (!mainHWND)
	{
		return FALSE;
	}

	logger.info("Created window");

	// create setting instance to keep track all settings
	setting = nullptr;
	setting = new Settings();

	if (!setting)
	{
		logger.errorCode(MHW::ERROR_CODE::FAILED_TO_CREATE_SETTING_INSTANCE);
		showErrorCodeMsgBox(MHW::ERROR_CODE::FAILED_TO_CREATE_SETTING_INSTANCE, false);
		return FALSE;
	}

	// init.
	int settingResult = setting->init();
	if (settingResult != 0)
	{
		logger.errorCode(settingResult);
		showErrorCodeMsgBox(static_cast<MHW::ERROR_CODE>(settingResult), false);
		return FALSE;
	}

	// Create database. This will initialize all the data (armor, charm, decoration, skill, set skill)
	db = nullptr;
	db = new Database();

	if (!db)
	{
		logger.errorCode(MHW::ERROR_CODE::FAILED_TO_CREATE_DATABASE_INSTANCE);
		showErrorCodeMsgBox(MHW::ERROR_CODE::FAILED_TO_CREATE_DATABASE_INSTANCE, false);
		return FALSE;
	}

	// Initialize
	int dbResult = db->init(setting);
	if (dbResult != 0)
	{
		logger.errorCode(dbResult);
		showErrorCodeMsgBox(static_cast<MHW::ERROR_CODE>(dbResult), true);
	}

	logger.info("Succesfully initialized data");

	int loadResult = setting->load(db);
	if (loadResult != 0)
	{
		logger.errorCode(loadResult);
		showErrorCodeMsgBox(static_cast<MHW::ERROR_CODE>(loadResult), true);
	}

	// Create set searcher. This runs thread to find armor set
	setSearcher = nullptr;
	setSearcher = new MHW::SetSearcher();

	if (!setSearcher)
	{
		logger.errorCode(MHW::ERROR_CODE::FAILED_TO_CREATE_SET_SEARCHER_INSTANCE);
		showErrorCodeMsgBox(MHW::ERROR_CODE::FAILED_TO_CREATE_SET_SEARCHER_INSTANCE, false);
		return FALSE;
	}

	// initialzie
	setSearcher->init(db);
	setSearcher->mainHWND = mainHWND;

	logger.info("Succesfully initialized armor set searcher");

	// Initialize rich edit
	hMod = LoadLibrary(L"riched20.dll");

	// index for hovering added skill
	lastHoveringAddedSkillBBIndex = -1;
	// index for hovering added set skill
	lastHoveringAddedSetSkillBBIndex = -1;

	lastRightclickedSkillBBIndex = -1;
	lastRightclickedSetSkillBBIndex = -1;
	lastrightclickedDecoIndex = -1;

	// flag for skill dropdown list. true if skill drop down list is opened. If opened, it doesn't update mouse hover.
	blockMouseHoverUpdate = false;

	LVN_supreesed = false;
	armorRankRadioButtonSupressed = false;

	// Some hieghts for group box
	skillGBHeight = 0;
	addedSkillGBHeight = 0;

	initGroupboxes();
	initWeaponCombobox();
	initCharmControls();
	initSkillControls();
	initSetSkillControls();
	initArmorControls();
	initDecorationControls();
	initButtons();
	initSearchOptions();
	initPopUpMenu();

	refreshMenu(true);

	ShowWindow(mainHWND, SW_SHOWDEFAULT);
	UpdateWindow(mainHWND);

	// Then, create search result window as hidden
	initSearchResultWindow();
	initAbout();

	initialized = true;

	logger.info("Initializing finished");
	logger.flush();

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		// called when main window is created
		OutputDebugString(L"WM_CREATE\n");
		break;
	case WM_SETFONT:
		//OutputDebugString(L"WM_SETFONT\n");
		break;
	case WM_ERASEBKGND:
		//return (LRESULT)1;
		break;
	case WM_SIZE:
		//OutputDebugString(L"change\n");
		//InvalidateRect(hWnd, NULL, false);
		if (hWnd == searchResultHWND)
		{
			// Search result window resized. Resize rich edit to fit window size.
			resizeSearchResultRichEdit();
		}
		break;
	case WM_LBUTTONDOWN:
	{
		//updateLeftMouseButtonClick();
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (hWnd == mainHWND)
		{
			updateMouseRightClick();
		}
	}
	break;
	case WM_GETMINMAXINFO:
	{
		updateWindowSize(hWnd, lParam);
	}
	break;
	case WM_NOTIFY:
	{
		//OutputDebugString(L"notify\n");
		switch (LOWORD(wParam))
		{
		case ID_ALL_DECO_LISTBOX:
		{
			//OutputDebugString(L"Wallla!");
			updateListViewNotification(lParam);
		}
		break;
		default:
			break;
		}
	}
	break;
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_SYSLINK1:
		{
			OutputDebugString(L"walla\n");
		}
		break;
		case ID_WEAPON_TOTAL_SLOTS_COMBOBOX:
		{
			updateWeaponControls(wParam, lParam);
		}
		break;
		case ID_WEAPON_SLOT_1_SIZE_COMBOBOX:
		{
			updateWeaponSlot1(wParam);
		}
		break;
		case ID_WEAPON_SLOT_2_SIZE_COMBOBOX:
		{
			updateWeaponSlot2(wParam);
		}
		break;
		case ID_WEAPON_SLOT_3_SIZE_COMBOBOX:
		{
			updateWeaponSlot3(wParam);
		}
		break;
		case ID_CHARM_COMBOBOX:
		{
			updateCharmDropdownList(wParam);
		}
		break;
		case ID_CHARM_DISPLAY_BY_NAME_RADIO_BUTTON:
		{
			updateCharmDisplayByName();
		}
		break;
		case ID_CHARM_DISPLAY_BY_SKILL_NAME_RADIO_BUTTON:
		{
			updateCharmDisplayBySkillName();
		}
		break;
		case ID_SET_SKILL_LR_RADIO_BUTTON:
		{
			updateSetSkillLowRankRadioButton();
		}
		break;
		case ID_SET_SKILL_HR_RADIO_BUTTON:
		{
			updateSetSkillHighRankRadioButton();
		}
		break;
		case ID_SKILL_COMBOBOX:
		{
			updateSkillDropdownList(wParam);
		}
		break;
		case ID_SET_SKILL_COMBOBOX:
		{
			updateSetSkillDropdownList(wParam);
		}
		break;
		case ID_ADD_SKILL_BUTTON:
		{
			updateAddSkillButon();
		}
		break;
		case ID_CLEAR_SKILL_BUTTON:
		{
			updateClearSkillsButton();
		}
		break;
		case ID_SKILL_1_CHECKBOX:
		case ID_SKILL_2_CHECKBOX:
		case ID_SKILL_3_CHECKBOX:
		case ID_SKILL_4_CHECKBOX:
		case ID_SKILL_5_CHECKBOX:
		case ID_SKILL_6_CHECKBOX:
		case ID_SKILL_7_CHECKBOX:
		case ID_SKILL_8_CHECKBOX:
		case ID_SKILL_9_CHECKBOX:
		case ID_SKILL_10_CHECKBOX:
		case ID_SKILL_11_CHECKBOX:
		case ID_SKILL_12_CHECKBOX:
		case ID_SKILL_13_CHECKBOX:
		case ID_SKILL_14_CHECKBOX:
		case ID_SKILL_15_CHECKBOX:
		case ID_SKILL_16_CHECKBOX:
		case ID_SKILL_17_CHECKBOX:
		case ID_SKILL_18_CHECKBOX:
		case ID_SKILL_19_CHECKBOX:
		case ID_SKILL_20_CHECKBOX:
		{
			updateSkillCheckbox(wmId);
		}
		break;
		case ID_ADD_SET_SKILL_BUTTON:
		{
			updateAddSetSkillButton();
		}
		break;
		case ID_CLEAR_SET_SKILL_BUTTON:
		{
			updateClearSetSkillsButton();
		}
		break;
		case ID_SET_SKILL_1_CHECKBOX:
		case ID_SET_SKILL_2_CHECKBOX:
		{
			updateSetSkillCheckbox(wmId);
		}
		break;
		case ID_ARMOR_HEAD_LOW_RANK_RADIO_BUTTON:
		{
			updateHeadArmorLowRankRadioButton();
		}
		break;
		case ID_ARMOR_HEAD_HIGH_RANK_RADIO_BUTTON:
		{
			updateHeadArmorHighRankRadioButton();
		}
		break;
		case ID_ARMOR_CHEST_LOW_RANK_RADIO_BUTTON:
		{
			updateChestArmorLowRankRadioButton();
		}
		break;
		case ID_ARMOR_CHEST_HIGH_RANK_RADIO_BUTTON:
		{
			updateChestArmorHighRankRadioButton();
		}
		break;
		case ID_ARMOR_ARM_LOW_RANK_RADIO_BUTTON:
		{
			updateArmArmorLowRankRadioButton();
		}
		break;
		case ID_ARMOR_ARM_HIGH_RANK_RADIO_BUTTON:
		{
			updateArmArmorHighRankRadioButton();
		}
		break;
		case ID_ARMOR_WAIST_LOW_RANK_RADIO_BUTTON:
		{
			updateWaistArmorLowRankRadioButton();
		}
		break;
		case ID_ARMOR_WAIST_HIGH_RANK_RADIO_BUTTON:
		{
			updateWaistArmorHighRankRadioButton();
		}
		break;
		case ID_ARMOR_LEG_LOW_RANK_RADIO_BUTTON:
		{
			updateLegArmorLowRankRadioButton();
		}
		break;
		case ID_ARMOR_LEG_HIGH_RANK_RADIO_BUTTON:
		{
			updateLegArmorHighRankRadioButton();
		}
		break;
		case ID_ARMOR_HEAD_COMBOBOX:
		{
			updateHeadArmorDropdownList(wParam);
		}
		break;
		case ID_ARMOR_CHEST_COMBOBOX:
		{
			updateChestArmorDropdownList(wParam);
		}
		break;
		case ID_ARMOR_ARM_COMBOBOX:
		{
			updateArmArmorDropdownList(wParam);
		}
		break;
		case ID_ARMOR_WAIST_COMBOBOX:
		{
			updateWaistArmorDropdownList(wParam);
		}
		break;
		case ID_ARMOR_LEG_COMBOBOX:
		{
			updateLegArmorDropdownList(wParam);
		}
		break;
		case ID_SEARCH_BUTTON:
		{
			int result = updateSearchButton();

			auto errCode = static_cast<MHW::ERROR_CODE>(result);
			if (errCode == MHW::ERROR_CODE::CFS_SKILL_IS_EMPTY || errCode == MHW::ERROR_CODE::CF_SKILL_IS_EMPTY)
			{
				showErrorMsgBox(setting->getString(MHW::StringLiteral::EMPTY_SKILL_ERR), false);
			}
			else if (errCode == MHW::ERROR_CODE::CFS_NO_CHECKED_SKILLS)
			{
				showErrorMsgBox(setting->getString(MHW::StringLiteral::EMPTY_CHECKED_SKILL_ERR), false);
			}
			else if (errCode == MHW::ERROR_CODE::CF_TOO_MANY_REQ_ARMOR_PIECES)
			{
				auto str = setting->getString(MHW::StringLiteral::TOO_MANY_REQ_ARMOR_PIECES);

				size_t f = str.find(L"%TOTAL");
				if (f != std::string::npos)
				{
					std::wstring n = std::to_wstring(setting->getTotalReqArmorPieces());
					str.replace(f, 6, n);
				}

				showErrorMsgBox(str, false);
			}
		}
		break;
		case ID_POP_UP_MENU_REMOVE:
		{
			if (hWnd == mainHWND)
			{
				removeAddedSkillFromPopupMenu();
			}
		}
		break;
		case ID_POP_UP_MENU_1:
		case ID_POP_UP_MENU_2:
		case ID_POP_UP_MENU_3:
		case ID_POP_UP_MENU_4:
		case ID_POP_UP_MENU_5:
		case ID_POP_UP_MENU_6:
		case ID_POP_UP_MENU_7:
		case ID_POP_UP_MENU_8:
		case ID_POP_UP_MENU_9:
		case ID_POP_UP_MENU_10:
		{
			switchAddedSkillLevel(wmId);
		}
		break;
		case ID_POP_UP_MENU_SS_REMOVE:
		{
			if (hWnd == mainHWND)
			{
				removeAddedSetSkillFromPopupMenu();
			}
		}
		break;
		case ID_POP_UP_MENU_SS_1:
		{
			switchAddedSetSkill();
		}
		break;
		case ID_POP_UP_MENU_DECO_COUNT_0:
		case ID_POP_UP_MENU_DECO_COUNT_1:
		case ID_POP_UP_MENU_DECO_COUNT_2:
		case ID_POP_UP_MENU_DECO_COUNT_3:
		case ID_POP_UP_MENU_DECO_COUNT_4:
		case ID_POP_UP_MENU_DECO_COUNT_5:
		case ID_POP_UP_MENU_DECO_COUNT_6:
		case ID_POP_UP_MENU_DECO_COUNT_7:
		{
			int index = wmId - ID_POP_UP_MENU_DECO_COUNT_0;
			OutputDebugString((std::to_wstring(index) + L"\n").c_str());

			bool result = setting->setDecoCount(lastrightclickedDecoIndex, index);
			if (result)
			{
				std::wstring countStr = std::to_wstring(index);
				ListView_SetItemText(allDecoList, lastrightclickedDecoIndex, 3, (LPWSTR)countStr.c_str());
			}
			else
			{
				MHW::Logger::getInstance().error("Failed to set deco count on index: " + std::to_string(lastrightclickedDecoIndex) + ", c: " + std::to_string(index));
			}

			setting->saveTemp();
			lastrightclickedDecoIndex = -1;
		}
		break;
		case ID_SELECT_ALL_DECO_BUTTON:
		{
			selectAllDecorations();
		}
		break;
		case ID_DESELECT_ALL_DECO_BUTTON:
		{
			deselectAllDecorations();
		}
		break;
		case ID_RESET_ALL_DECO_COUNT:
		{
			resetAllDecoCount();
		}
		break;
		case ID_OPTION_ALLOWLOWRANKARMORS:
		{
			toggleAllowLowRankArmorOption();
		}
		break;
		case ID_OPTION_ALLOWARENAARMOR:
		{
			toggleAllowArenaArmorOption();
		}
		break;
		case ID_OPTION_ALLOWEVENTARMOR:
		{
			toggleAllowEventArmorOption();
		}
		break;
		case ID_GENDER_MALE:
		{
			switchGender(MHW::Gender::MALE);
		}
		break;
		case ID_GENDER_FEMALE:
		{
			switchGender(MHW::Gender::FEMALE);
		}
		break;
		case ID_SEARCH_OPTION_ALLOW_EXTRA_SKILL:
		{
			setting->allowExtraSkills = !setting->allowExtraSkills;
			setting->saveTemp();
		}
		break;
		case ID_SEARCH_OPTION_ALLOW_OVERLEVELED_SKILL:
		{
			setting->allowOverleveledSkills = !setting->allowOverleveledSkills;
			setting->saveTemp();
		}
		break;
		case ID_SEARCH_OPTION_SEARCH_FROM_HIGHER_RARIRTY:
		{
			setting->searchFromHigherArmorRarity = !setting->searchFromHigherArmorRarity;
			setting->saveTemp();
		}
		break;
		/*
	case ID_SEARCH_OPTION_USE_MAX_LEVEL_CHARM_ONLY:
	{
		setting->useOnlyMaxLevelCharm = !setting->useOnlyMaxLevelCharm;
		setting->saveTemp();
		//SendMessage(searchOptionUseMaxLevelCharmOnly, BM_SETCHECK, (setting->useOnlyMaxLevelCharm ? BST_CHECKED : BST_UNCHECKED), 0);
	}
		break;
		*/
		case ID_SEARCH_OPTION_SIMPLIFY_SEARCH_RESULT:
		{
			setting->simplifySearchResult = !setting->simplifySearchResult;
			setting->saveTemp();
		}
		break;
		case ID_SEARCH_OPTION_SHOW_MAX_LEVEL:
		{
			setting->showMaxLevel = !setting->showMaxLevel;
			setting->saveTemp();
		}
		break;
		case ID_SEARCH_OPTION_MIN_RARITY_DROPDOWNLIST:
		{
			updateMinRarity(wParam);
		}
		break;
		case ID_LANGUAGE_ENGLISH:
		{
			// Set language to english
			if (setSearcher->getState() == MHW::SetSearcher::State::IDLE)
			{
				switchLanguage(MHW::Language::ENGLISH, false);
			}
		}
		break;
		case ID_LANGUAGE_KOREAN:
		{
			// Set language to korean
			if (setSearcher->getState() == MHW::SetSearcher::State::IDLE)
			{
				switchLanguage(MHW::Language::KOREAN, false);
			}
		}
		break;
		case ID_OPTION_CLEAR:
		{
			setting->reset(db);
			setting->saveTemp();

			reloadUI();
			redrawRect(mainHWND);
		}
		break;
		case ID_FILE_SAVE:
		{
			save();
		}
		break;
		case ID_FILE_OPEN:
		{
			open();
		}
		break;
		case IDM_ABOUT:
		{
			/*
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			auto ret = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			auto rect = getControlRect(ret);
			ShowWindow(ret, SW_SHOWDEFAULT);
			auto sl2 = CreateWindow(WC_LINK, L"TEST", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 20, 20, 100, 30, ret, (HMENU)IDC_SYSLINK1, hInst, 0);
			auto err = GetLastError();
			*/

			ShowWindow(aboutHWND, SW_SHOW);
			break;
		}
		case ID_HELP_REPORTERROR:
		{
			ShellExecuteW(NULL, L"open", L"https://github.com/bsy6766/MHWASS/issues", NULL, NULL, SW_SHOWNORMAL);
		}
		break;
		case IDM_EXIT:
			clear();
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam)
		{
		case 80:
			// p key
			//setting->print(db->charms);
			break;
		case 84:
			// t key
		{
			//ScrollWindow(mainHWND, 0, -50, NULL, NULL);
			//showErrorMsgBox("Test", true);
			//ShowWindow(searchResultHWND, SW_SHOW);
		}
		break;
		case 85:
			// u key
			//ShowWindow(searchResultHWND, SW_HIDE);
			break;
		default:
			break;
		}
	}
	break;
	case WM_PAINT:
	{
		if (hWnd == mainHWND)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			drawMainWindow(hWnd, hdc);

			EndPaint(hWnd, &ps);
		}
		else if (hWnd == searchResultHWND)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			drawSearchResult(hdc);

			EndPaint(hWnd, &ps);
		}
		else
		{
			break;
		}
	}
	break;
	case WM_CLOSE:
	{
		if (hWnd == mainHWND)
		{
			DestroyWindow(hWnd);
			clear();
			return 0;
		}
		else if (hWnd == searchResultHWND)
		{
			//DestroyWindow(hWnd);
			//searchResultHWND = nullptr;
			ShowWindow(searchResultHWND, SW_HIDE);
		}
	}
	break;
	case WM_DESTROY:
		if (hWnd == mainHWND)
		{
			auto& logger = MHW::Logger::getInstance();
			logger.info("Destroying wiondow...");
			logger.flush();
			PostQuitMessage(0);
		}
		break;
	case WM_QUIT:
	{
		OutputDebugString(L"Wm_quit");
	}
	break;
	case WM_FROM_WORKER_THREAD:
	{
		checkWorkerThread();
	}
	break;
	default:
		if (hWnd == mainHWND)
		{
			updateMouseMove(false);
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

// draw 
void drawMainWindow(HWND& hWnd, HDC& hdc)
{
	//if (initialized == false) return;

	RECT clientRect;
	GetClientRect(hWnd, &clientRect);

	FillRect(hdc, &clientRect, NULL);

	SelectObject(hdc, defaultFont);

	// weapon group box texts
	auto totalWeaponDecoStr = setting->getString(MHW::StringLiteral::WEAPON_TOTAL_DECO);
	TextOut(hdc, weaponGBPos.x + X_OFFSET, weaponGBPos.y + TEXT_OUT_Y_OFFSET, totalWeaponDecoStr.c_str(), totalWeaponDecoStr.size());
	auto weaponDecoLevelStr = setting->getString(MHW::StringLiteral::WEAPON_DECO_LEVEL);
	TextOut(hdc, weaponGBPos.x + X_OFFSET + 100, weaponGBPos.y + TEXT_OUT_Y_OFFSET, weaponDecoLevelStr.c_str(), weaponDecoLevelStr.size());

	//std::wstring setSkillLabel = L"Set Skills";
	//TextOut(hdc, 10, 360, setSkillLabel.c_str(), setSkillLabel.size());

	if (setting)
	{
		// draw skill texts
		int startX = addedSkillsGBPos.x + 10;
		int startY = addedSkillsGBPos.y + 20;

		std::wstring wipeStr = L"                                                    ";

		// check skill
		if (setting->skills.empty())
		{
			// empty. Fill with (empty) text
			COLORREF gray = RGB(128, 128, 128);
			SetTextColor(hdc, gray);

			// draw text and rect to overwrite remove icon image
			for (int i = 0; i < MAX_SKILL_COUNT; i++)
			{
				std::wstring str = (setting->getString(MHW::StringLiteral::EMPTY) + L"                                         ");

				// draw text
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// overwrite rect area
				drawRect(hdc, startX - 16, startY + 2, 12, 12);

				// incremtne y
				startY += 20;
			}

			// set text color back to black
			COLORREF black = RGB(0, 0, 0);
			SetTextColor(hdc, black);
		}
		else
		{
			// Not empty
			COLORREF black = RGB(0, 0, 0);
			SetTextColor(hdc, black);

			for (auto skill : setting->skills)
			{
				// Dispaly skill name and level
				std::wstring str = skill->name + L" " + std::to_wstring(skill->level);

				// draw text
				TextOut(hdc, startX, startY, wipeStr.c_str(), (int)wipeStr.size());
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// draw icon
				//drawImage(hdc, removeSkillImage, startX - 16, startY + 2, 12, 12);

				// increment y
				startY += 20;
			}

			// draw remaining texts. 
			COLORREF gray = RGB(128, 128, 128);
			SetTextColor(hdc, gray);

			const int size = (int)setting->skills.size();
			for (int i = size; i < MAX_SKILL_COUNT; i++)
			{
				std::wstring str = (setting->getString(MHW::StringLiteral::EMPTY) + L"                                         ");

				// draw text
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// overwrite rect
				drawRect(hdc, startX - 16, startY + 2, 12, 12);

				// increment y
				startY += 20;
			}

			// Set text color back to black
			SetTextColor(hdc, black);
		}

		// draw set skill text

		startX = addedSetSkillsGBPos.x + 10;
		startY = addedSetSkillsGBPos.y + 20;

		if (setting->setSkills.empty())
		{
			COLORREF gray = RGB(128, 128, 128);
			SetTextColor(hdc, gray);

			for (int i = 0; i < MAX_SET_SKILL_COUNT; i++)
			{
				std::wstring str = (setting->getString(MHW::StringLiteral::EMPTY) + L"                                         ");

				// draw text
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// overwrite rect area
				drawRect(hdc, startX - 16, startY + 2, 12, 12);

				startY += 20;
			}

			COLORREF black = RGB(0, 0, 0);
			SetTextColor(hdc, black);
		}
		else
		{
			// Not empty
			COLORREF black = RGB(0, 0, 0);
			SetTextColor(hdc, black);

			for (auto setSkill : setting->setSkills)
			{
				// Dispaly skill name and level
				std::wstring str;

				if (setSkill->highRank)
				{
					str = L"(HR)";
				}
				else
				{
					str = L"(LR)";
				}

				str += setSkill->name + L" " + std::to_wstring(setSkill->reqArmorPieces);

				// draw text
				TextOut(hdc, startX, startY, wipeStr.c_str(), (int)wipeStr.size());
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// increment y
				startY += 20;
			}

			// draw remaining texts. 
			COLORREF gray = RGB(128, 128, 128);
			SetTextColor(hdc, gray);

			const int size = (int)setting->setSkills.size();
			for (int i = size; i < MAX_SET_SKILL_COUNT; i++)
			{
				std::wstring str = (setting->getString(MHW::StringLiteral::EMPTY) + L"                                         ");

				// draw text
				TextOut(hdc, startX, startY, str.c_str(), (int)str.size());

				// overwrite rect
				drawRect(hdc, startX - 16, startY + 2, 12, 12);

				// increment y
				startY += 20;
			}

			// Set text color back to black
			SetTextColor(hdc, black);
		}
	}

	/*
	// Count total selected skill and set skill
	int totalSkillApplied = 0;
	int totalSetSkillApplied = 0;

	if (setting)
	{
	if (!setting->skills.empty())
	{
	for (auto& skill : setting->skills)
	{
	if (skill.applied)
	{
	totalSkillApplied++;
	}
	}
	}

	if (!setting->setSkills.empty())
	{
	for (auto& ss : setting->setSkills)
	{
	if (ss.applied)
	{
	totalSetSkillApplied++;
	}
	}
	}
	}

	std::wstring str = std::to_wstring(totalSkillApplied) + L" skill" + ((totalSkillApplied > 1) ? L"s" : L"") + L" selected.      ";
	TextOut(hdc, skillGBPos.x + X_OFFSET, addedSkillsGBPos.y + addedSkillGBHeight, str.c_str(), (int)str.size());

	str = std::to_wstring(totalSetSkillApplied) + L" set skill" + ((totalSetSkillApplied > 1) ? L"s" : L"") + L" selected.      ";
	TextOut(hdc, setSkillGBPos.x + X_OFFSET, addedSetSkillsGBPos.y + 60, str.c_str(), (int)str.size());

	*/
	/*
	int totalReqArmorPiece = setting->getTotalReqArmorPieces();
	{
	// Show warning
	const int warningX = X_OFFSET * 2;
	const int warningY = addedSetSkillsGBPos.y + 82;

	if (totalReqArmorPiece > 5)
	{
	COLORREF red = RGB(255, 0, 0);
	SetTextColor(hdc, red);

	//std::wstring warning = L"Warning: Currently selected set skills require more armor pieces (" + std::to_wstring(totalReqArmorPiece) + L") than it can be equipped (5).";
	TextOut(hdc, warningX, warningY, L"Currently selected set skills require more armor", 48);
	TextOut(hdc, warningX, warningY + 16, (L"pieces (" + std::to_wstring(totalReqArmorPiece) + L") than it can be equipped (5).").c_str(), 39);

	COLORREF black = RGB(0, 0, 0);
	SetTextColor(hdc, black);
	}
	else
	{
	std::wstring emptyStr = L"                                                                                             ";
	TextOut(hdc, warningX, warningY, emptyStr.c_str(), (int)emptyStr.size());
	TextOut(hdc, warningX, warningY + 16, emptyStr.c_str(), (int)emptyStr.size());
	}
	}
	*/

	std::wstring wipeDescriptionStr = L"                                                                                                                                ";

	{
		// Skill description
		DrawText(hdc, wipeDescriptionStr.c_str(), (int)wipeDescriptionStr.size(), &skillDescriptionRect, DT_WORDBREAK);

		std::wstring skillDescription = setting->getString(MHW::StringLiteral::SKILL_EFFECT) + L": ";

		int skillIndex = -1;

		if (lastHoveringAddedSkillBBIndex >= 0)
		{
			//OutputDebugString((L"- " + std::to_wstring(lastHoveringAddedSkillBBIndex) + L"\n").c_str() );
			skillIndex = setting->getSkillOriginalIndexByPos(lastHoveringAddedSkillBBIndex);

			if (skillIndex == -1)
			{
				const int skillCBRow = (int)SendMessage(skillDropdownList, CB_GETCURSEL, 0, 0);

				skillIndex = db->getSkillIdByDropdownIndex(skillCBRow);
			}
		}
		else
		{
			const int skillCBRow = (int)SendMessage(skillDropdownList, CB_GETCURSEL, 0, 0);

			skillIndex = db->getSkillIdByDropdownIndex(skillCBRow);
		}

		skillDescription += db->getSkillDescriptionById(skillIndex);
		skillDescription += (L"\n" + setting->getString(MHW::StringLiteral::SKILL_LEVEL) + L" " + std::to_wstring(db->getSkillLevelById(skillIndex)) + L": " + db->getSkillLevelDescriptionById(skillIndex));

		DrawText(hdc, skillDescription.c_str(), (int)skillDescription.size(), &skillDescriptionRect, DT_WORDBREAK);
	}

	{
		// Set skill description.
		DrawText(hdc, wipeDescriptionStr.c_str(), (int)wipeDescriptionStr.size(), &setSkillDescriptionRect, DT_WORDBREAK);

		std::wstring setSkillDescription = setting->getString(MHW::StringLiteral::SKILL_EFFECT) + L": ";

		int setSkillIndex = -1;
		bool highRank = false;

		if (lastHoveringAddedSetSkillBBIndex >= 0)
		{
			setSkillIndex = setting->getSetSkillOriginalIndexByPos(lastHoveringAddedSetSkillBBIndex);
			highRank = setting->getSetSkillRankByPos(lastHoveringAddedSetSkillBBIndex);

			setSkillDescription += db->getSetSkillDescriptionById(setSkillIndex, highRank);
		}
		else
		{
			const int ssCBRow = (int)SendMessage(setSkillDropdownList, CB_GETCURSEL, 0, 0);

			setSkillIndex = db->getSetSkillIdByDropdownIndex(ssCBRow, setting->highRankSetSkill);

			setSkillDescription += db->getSetSkillDescriptionById(setSkillIndex, setting->highRankSetSkill);
		}

		DrawText(hdc, setSkillDescription.c_str(), (int)setSkillDescription.size(), &setSkillDescriptionRect, DT_WORDBREAK);
	}

	// 2nd column
	int armorLabelX = armorGBPos.x + 10;
	int armorLabelY = armorGBPos.y + 20;

	auto headStr = setting->getString(MHW::StringLiteral::HEAD);
	TextOut(hdc, armorLabelX, armorLabelY, headStr.c_str(), headStr.size());

	armorLabelY += ARMOR_TEXT_Y_OFFSET;
	auto chestStr = setting->getString(MHW::StringLiteral::CHEST);
	TextOut(hdc, armorLabelX, armorLabelY, chestStr.c_str(), chestStr.size());

	armorLabelY += ARMOR_TEXT_Y_OFFSET;
	auto armStr = setting->getString(MHW::StringLiteral::ARM);
	TextOut(hdc, armorLabelX, armorLabelY, armStr.c_str(), armStr.size());

	armorLabelY += ARMOR_TEXT_Y_OFFSET;
	auto waistStr = setting->getString(MHW::StringLiteral::WAIST);
	TextOut(hdc, armorLabelX, armorLabelY, waistStr.c_str(), waistStr.size());

	armorLabelY += ARMOR_TEXT_Y_OFFSET;
	auto legStr = setting->getString(MHW::StringLiteral::LEG);
	TextOut(hdc, armorLabelX, armorLabelY, legStr.c_str(), legStr.size());

	const int x = 602;
	const int y = 655;
	auto minRarityStr = setting->getString(MHW::StringLiteral::MIN_RARITY);
	TextOut(hdc, x, y, minRarityStr.c_str(), minRarityStr.size());
}

void drawRect(HDC & hdc, const int x, const int y, const int w, const int h)
{
	RECT rect;
	rect.left = x;
	rect.top = y;
	rect.right = x + w;
	rect.bottom = y + h;

	FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOW + 1));
}

void drawSearchResult(HDC & hdc)
{
	RECT clientRect;
	GetClientRect(searchResultHWND, &clientRect);

	FillRect(hdc, &clientRect, NULL);
}

void initGroupboxes()
{
	// Weapon group box
	weaponGBPos.x = X_OFFSET;
	weaponGBPos.y = 5;

	weaponGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::WEAPON_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, weaponGBPos.x, weaponGBPos.y, LEFT_COLUMN_WIDTH, 45, mainHWND, NULL, hInst, NULL);
	SendMessage(weaponGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	// Charm group box
	charmGBPos.x = X_OFFSET;
	charmGBPos.y = 55;

	charmGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::CHARM_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, charmGBPos.x, charmGBPos.y, LEFT_COLUMN_WIDTH, 70, mainHWND, NULL, hInst, NULL);
	SendMessage(charmGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	// Skill group box
	skillGBPos.x = X_OFFSET;
	skillGBPos.y = 130;

	skillGBHeight = (MAX_SKILL_COUNT * 20) + 165;

	skillGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SKILLS_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, skillGBPos.x, skillGBPos.y, LEFT_COLUMN_WIDTH, skillGBHeight, mainHWND, NULL, hInst, NULL);
	SendMessage(skillGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	// Group box where added skills are listed
	addedSkillsGBPos.x = skillGBPos.x + X_OFFSET;
	addedSkillsGBPos.y = skillGBPos.y + 50;

	addedSkillGBHeight = skillGBHeight - 145;

	addedSkillsGroupBox = CreateWindow(L"button", (setting->getString(MHW::StringLiteral::ADDED_SKILLS) + L" (" + std::to_wstring(setting->skills.size()) + L"/" + std::to_wstring(MAX_SKILL_COUNT) + L")").c_str(), WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		addedSkillsGBPos.x, addedSkillsGBPos.y, 200, addedSkillGBHeight, mainHWND, 0, hInst, NULL);

	SendMessage(addedSkillsGroupBox, WM_SETFONT, (WPARAM)defaultFont, true);

	addedSkillGBRect = getControlRect(addedSkillsGroupBox);

	// set skill group box
	setSkillGBPos.x = X_OFFSET;
	setSkillGBPos.y = skillGBPos.y + skillGBHeight + 5;

	setSkillGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SET_SKILL_GB).c_str(), WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		setSkillGBPos.x, setSkillGBPos.y, LEFT_COLUMN_WIDTH, 185, mainHWND, 0, hInst, NULL);

	SendMessage(setSkillGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	// Group box where added skills are listed
	addedSetSkillsGBPos.x = setSkillGBPos.x + X_OFFSET;
	addedSetSkillsGBPos.y = setSkillGBPos.y + 70;

	addedSetSkillsGroupBox = CreateWindow(L"button", (setting->getString(MHW::StringLiteral::ADDED_SET_SKILLS) + L" (" + std::to_wstring(setting->getTotalAddedSetSkills()) + L"/" + std::to_wstring(MAX_SET_SKILL_COUNT) + L")").c_str(), WS_CHILD | BS_GROUPBOX | WS_VISIBLE,
		addedSetSkillsGBPos.x, addedSetSkillsGBPos.y, 200, 60, mainHWND, 0, hInst, NULL);

	SendMessage(addedSetSkillsGroupBox, WM_SETFONT, (WPARAM)defaultFont, true);

	addedSetSkillGBRect = getControlRect(addedSetSkillsGroupBox);

	// armor groupbox
	armorGBPos.x = RIGHT_COLUMN_X;
	armorGBPos.y = 5;

	int armorGBHeight = 270;

	armorGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::ARMOR_FILTER_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, armorGBPos.x, armorGBPos.y, RIGHT_COLUMN_WIDTH, armorGBHeight, mainHWND, NULL, hInst, NULL);
	SendMessage(armorGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	// decorations
	decorationGBPos.x = RIGHT_COLUMN_X;
	decorationGBPos.y = armorGBHeight + armorGBPos.y + 5;

	int decorationGBHeight = 350;

	decorationGroupbox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::DECORATION_FILTER_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, decorationGBPos.x, decorationGBPos.y, RIGHT_COLUMN_WIDTH, decorationGBHeight, mainHWND, NULL, hInst, NULL);
	SendMessage(decorationGroupbox, WM_SETFONT, (WPARAM)defaultFont, true);

	searchOptionsGBPos.x = RIGHT_COLUMN_X;
	searchOptionsGBPos.y = decorationGBPos.y + 355;

	const int searchOptionGBWidth = 430;
	const int searchOptionGBHeight = 150;

	searchOptionGroupBox = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SEARCH_OPTION_GB).c_str(), WS_CHILD | WS_VISIBLE | BS_GROUPBOX, searchOptionsGBPos.x, searchOptionsGBPos.y, searchOptionGBWidth, searchOptionGBHeight, mainHWND, NULL, hInst, NULL);
	SendMessage(searchOptionGroupBox, WM_SETFONT, (WPARAM)defaultFont, true);
}

void initWeaponCombobox()
{
	const int totalWeaponSlotCount = 4;
	WCHAR totalWeaponSlotChars[totalWeaponSlotCount][2] = { L"0",L"1",L"2",L"3" };

	int totalWeaponSlotDropdownListX = weaponGBPos.x + X_OFFSET + 40;
	int dropdownListY = weaponGBPos.y + COMBOBOX_Y_OFFSET;

	// create weapon slot dropdown list
	{
		totalWeaponSlotsDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			totalWeaponSlotDropdownListX, dropdownListY, 40, 100, mainHWND, (HMENU)ID_WEAPON_TOTAL_SLOTS_COMBOBOX, hInst, NULL);

		SendMessage(totalWeaponSlotsDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		for (int i = 0; i < totalWeaponSlotCount; i++)
			SendMessage(totalWeaponSlotsDropdownList, CB_ADDSTRING, 0, (LPARAM)totalWeaponSlotChars[i]);

		SendMessage(totalWeaponSlotsDropdownList, CB_SETCURSEL, setting->totalWeaponSlots, 0);
	}

	int weaponSlotSizeDropdownListX = totalWeaponSlotDropdownListX + 100;

	{
		// weapon slot 1 drop down list
		weaponSlot1SizeDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			weaponSlotSizeDropdownListX, dropdownListY, 40, 100, mainHWND, (HMENU)ID_WEAPON_SLOT_1_SIZE_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(weaponSlot1SizeDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// Fill combobox with number of range 1 ~ 3.
		for (int i = 1; i < totalWeaponSlotCount; i++)
		{
			SendMessage(weaponSlot1SizeDropdownList, CB_ADDSTRING, 0, (LPARAM)totalWeaponSlotChars[i]);
		}

		// Disable window by default
		EnableWindow(weaponSlot1SizeDropdownList, ((setting->totalWeaponSlots >= 1) ? true : false));

		SendMessage(weaponSlot1SizeDropdownList, CB_SETCURSEL, setting->weaponSlot1Size - 1, 0);

		// increment x pos
		weaponSlotSizeDropdownListX += WEAPON_SLOT_SIZE_X_OFFSET;
	}

	{
		// weapon slot 2
		weaponSlot2SizeDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			weaponSlotSizeDropdownListX, dropdownListY, 40, 100, mainHWND, (HMENU)ID_WEAPON_SLOT_2_SIZE_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(weaponSlot2SizeDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// Fill combobox with number of range 1 ~ 3.
		for (int i = 1; i < totalWeaponSlotCount; i++)
		{
			SendMessage(weaponSlot2SizeDropdownList, CB_ADDSTRING, 0, (LPARAM)totalWeaponSlotChars[i]);
		}

		// Disable window by default
		EnableWindow(weaponSlot2SizeDropdownList, ((setting->totalWeaponSlots >= 2) ? true : false));

		SendMessage(weaponSlot2SizeDropdownList, CB_SETCURSEL, setting->weaponSlot2Size - 1, 0);

		// increment x pos
		weaponSlotSizeDropdownListX += WEAPON_SLOT_SIZE_X_OFFSET;
	}

	{
		// weapon slot 3
		weaponSlot3SizeDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,
			weaponSlotSizeDropdownListX, dropdownListY, 40, 100, mainHWND, (HMENU)ID_WEAPON_SLOT_3_SIZE_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(weaponSlot3SizeDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// Fill combobox with number of range 1 ~ 3.
		for (int i = 1; i < totalWeaponSlotCount; i++)
		{
			SendMessage(weaponSlot3SizeDropdownList, CB_ADDSTRING, 0, (LPARAM)totalWeaponSlotChars[i]);
		}

		// Disable window by default
		EnableWindow(weaponSlot3SizeDropdownList, ((setting->totalWeaponSlots >= 3) ? true : false));

		SendMessage(weaponSlot3SizeDropdownList, CB_SETCURSEL, setting->weaponSlot3Size - 1, 0);

		// increment x pos
		weaponSlotSizeDropdownListX += WEAPON_SLOT_SIZE_X_OFFSET;
	}
}

void initCharmControls()
{
	// Create radio button that decides how to display charm, either by name or skill name
	charmDisplayTypeNameRadioButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::CHARM_DISPLAY_BY_NAME).c_str(), WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, X_OFFSET * 2, charmGBPos.y + TEXT_OUT_Y_OFFSET, 50, 12, mainHWND, (HMENU)ID_CHARM_DISPLAY_BY_NAME_RADIO_BUTTON, hInst, NULL);
	SendMessage(charmDisplayTypeNameRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

	charmDisplayTypeSkillNameRadioButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::CHARM_DISPLAY_BY_SKILL_NAME).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, (X_OFFSET * 2) + 60, charmGBPos.y + TEXT_OUT_Y_OFFSET, 80, 12, mainHWND, (HMENU)ID_CHARM_DISPLAY_BY_SKILL_NAME_RADIO_BUTTON, hInst, NULL);
	SendMessage(charmDisplayTypeSkillNameRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
	// Note: Above radio buttons are grouped. 

	refreshCharmDisplaySetting();

	// List if charms
	charmDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWN,
		TEXT_OUT_X_OFFSET, charmGBPos.y + 15 + TEXT_OUT_Y_OFFSET + 3, DROPDOWN_LIST_W, 400, mainHWND, (HMENU)ID_CHARM_COMBOBOX, hInst, NULL);

	// Set font
	SendMessage(charmDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

	// This function will populate dropdown list
	refreshCharmDropdownList();
}

void refreshCharmDropdownList()
{
	// reset dropdown list
	SendMessage(charmDropdownList, CB_RESETCONTENT, 0, 0);

	// Add None item. If None is selected, charm will not be used in search
	SendMessage(charmDropdownList, CB_ADDSTRING, 0, (LPARAM)(setting->getString(MHW::StringLiteral::NONE).c_str()));
	// Add Any item. If Any is selected, best fitting charm will be use in search.
	SendMessage(charmDropdownList, CB_ADDSTRING, 0, (LPARAM)(setting->getString(MHW::StringLiteral::ANY).c_str()));

	// Iterate charm and fill dropdown list
	for (auto& e : db->charms)
	{
		std::wstring str;

		// Change display based on dispaly setting
		if (setting->charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_NAME)
		{
			// Display charm name (i.e. OOOOO Charm) with level.
			str = (e.second).name + L" " + std::to_wstring((e.second).level);
		}
		else
		{
			// Display charm's skill name
			// Note: Charm's first skill can be set skill
			if ((e.second).setSkill)
			{
				// set skill. 
				// Note: in patch 1.06, it's only highrank setskill (Guard up)
				//str = db->getSetSkillNameById((e.second).skillId, true) + L" " + std::to_wstring((e.second).level);
				str = db->getSkillNameById((e.second).skillId) + L" " + std::to_wstring((e.second).level);
			}
			else
			{
				// normal skill
				str = db->getSkillNameById((e.second).skillId) + L" " + std::to_wstring((e.second).level);
			}

			if ((e.second).hasSecondSkill())
			{
				// Some charm has 2 skills
				str += (L", " + db->getSkillNameById((e.second).secondSkillId) + L" " + std::to_wstring((e.second).level));
			}
		}

		// add item
		SendMessage(charmDropdownList, CB_ADDSTRING, 0, (LPARAM)str.c_str());
	}

	// Display charm item that was selected before.
	SendMessage(charmDropdownList, CB_SETCURSEL, setting->charmIndex, 0);
}

void initSkillControls()
{
	skillDescriptionRect.left = skillGBPos.x + X_OFFSET;
	skillDescriptionRect.top = addedSkillsGBPos.y + addedSkillGBHeight;
	skillDescriptionRect.right = skillDescriptionRect.left + 280;
	skillDescriptionRect.bottom = skillDescriptionRect.top + (LINESPACE * 6);

	// init bbs for skill description
	int bbStartX = 24;
	int bbStartY = addedSkillsGBPos.y + 21;
	int bbYOffset = 20;
	const int bbWidth = 190;
	const int bbHeight = 20;

	for (int i = 0; i < MAX_SKILL_COUNT; ++i)
	{
		RECT bb;
		bb.left = bbStartX;
		bb.right = bbStartX + bbWidth;
		bb.top = bbStartY;
		bb.bottom = bbStartY + bbHeight;

		bbStartY += bbYOffset;

		addedSkillBBs.push_back(bb);
	}

	// skill drop down list
	int dropdownListY = skillGBPos.y + TEXT_OUT_Y_OFFSET;

	skillDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWN,
		skillGBPos.x + X_OFFSET, dropdownListY, DROPDOWN_LIST_W, 400, mainHWND, (HMENU)ID_SKILL_COMBOBOX, hInst, NULL);

	// set font
	SendMessage(skillDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

	// Fill skill items in dropdown list
	refreshSkillCombobox();

	// add buttons
	{
		int buttonX = skillGBPos.x + X_OFFSET + 205;
		int buttonY = skillGBPos.y + TEXT_OUT_Y_OFFSET + 35;

		// Add skill button. Adds selected skill on skill dropdown menu
		addSkillButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::ADD_SKILLS).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonX, buttonY, 70, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_ADD_SKILL_BUTTON, hInst, NULL);

		// set font
		SendMessage(addSkillButton, WM_SETFONT, (WPARAM)defaultFont, true);

		// move down
		buttonY += 30;

		// Clear added skills button. Clears all added skills on the list
		clearAllSkillsButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::CLEAR).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonX, buttonY, 50, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_CLEAR_SKILL_BUTTON, hInst, NULL);

		// set font
		SendMessage(clearAllSkillsButton, WM_SETFONT, (WPARAM)defaultFont, true);
	}

	// Add check boxes
	int startX = addedSkillsGBPos.x + 180;
	int startY = addedSkillsGBPos.y + 20;

	int id = (int)ID_SKILL_1_CHECKBOX;
	const int addedSkillSize = setting->skills.size();
	int index = 0;
	auto iter = setting->skills.begin();

	for (int i = 0; i < MAX_SKILL_COUNT; ++i)
	{
		auto cbHwnd = CreateWindow(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, startX, startY, 12, 12, mainHWND, (HMENU)(id + i), hInst, NULL);
		startY += 20;
		skillCheckboxes.push_back(cbHwnd);
		EnableWindow(cbHwnd, false);

		if (index < addedSkillSize)
		{
			EnableWindow(cbHwnd, true);

			if ((*iter)->applied)
			{
				SendMessage(cbHwnd, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				SendMessage(cbHwnd, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			iter++;
		}

		index++;
	}
}

void refreshSkillCombobox()
{
	SendMessage(skillDropdownList, CB_RESETCONTENT, 0, 0);

	int indexCounter = 0;
	int newIndexCounter = 0;

	for (auto& e : db->skills)
	{
		if (setting->isSkillAdded((e.second).id))
		{
			db->skills[indexCounter].dropdownIndex = -1;
			indexCounter++;
			continue;
		}

		db->skills[indexCounter].dropdownIndex = newIndexCounter;
		indexCounter++;
		newIndexCounter++;

		auto skillStr = (e.second).name + L" " + std::to_wstring((e.second).level);

		if ((e.second).fromDecoSetSkill)
		{
			skillStr += L" (s)";
		}
		else if ((e.second).onlyFromDeco)
		{
			skillStr += L" (d)";
		}

		SendMessage(skillDropdownList, CB_ADDSTRING, 0, (LPARAM)skillStr.c_str());
	}

	SendMessage(skillDropdownList, CB_SETCURSEL, setting->skillIndex, 0);
}

void refreshAddedSkillList()
{
	SetWindowText(addedSkillsGroupBox, (setting->getString(MHW::StringLiteral::ADDED_SKILLS) + L" (" + std::to_wstring(setting->skills.size()) + L"/" + std::to_wstring(MAX_SKILL_COUNT) + L")").c_str());

	redrawRect(skillGroupbox);

	if (setting)
	{
		if (setting->skills.empty())
		{
			for (int i = 0; i < MAX_SKILL_COUNT; ++i)
			{
				SendMessage(skillCheckboxes.at(i), BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(skillCheckboxes.at(i), false);
			}
		}
		else
		{
			int index = 0;
			int curskillSize = (int)setting->skills.size();

			auto iter = setting->skills.begin();

			for (int i = 0; i < MAX_SKILL_COUNT; ++i)
			{
				if (index < curskillSize)
				{
					if ((*iter)->applied)
					{
						SendMessage(skillCheckboxes.at(index), BM_SETCHECK, BST_CHECKED, 0);
					}
					else
					{
						SendMessage(skillCheckboxes.at(index), BM_SETCHECK, BST_UNCHECKED, 0);
					}
					EnableWindow(skillCheckboxes.at(index), true);

					iter++;
				}
				else
				{
					SendMessage(skillCheckboxes.at(index), BM_SETCHECK, BST_UNCHECKED, 0);
					EnableWindow(skillCheckboxes.at(index), false);
				}

				index++;
			}
		}
	}
}

void initSetSkillControls()
{
	totalSelectedSetSkillRect.left = setSkillGBPos.x + X_OFFSET;
	totalSelectedSetSkillRect.top = addedSetSkillsGBPos.y + 65/*added set skills gb height*/ - 5;
	totalSelectedSetSkillRect.right = totalSelectedSetSkillRect.left + 110;
	totalSelectedSetSkillRect.bottom = totalSelectedSetSkillRect.top + 15;

	setSkillDescriptionRect.left = setSkillGBPos.x + X_OFFSET;
	setSkillDescriptionRect.top = addedSetSkillsGBPos.y + 65/*added set skills gb height*/;
	setSkillDescriptionRect.right = setSkillDescriptionRect.left + 285;
	setSkillDescriptionRect.bottom = setSkillDescriptionRect.top + (LINESPACE * 6) + 5;

	// init bbs for skill description
	int bbStartX = 24;
	int bbStartY = addedSetSkillsGBPos.y + 19;
	int bbYOffset = 20;
	const int bbWidth = 190;
	const int bbHeight = 20;

	for (int i = 0; i < MAX_SET_SKILL_COUNT; ++i)
	{
		RECT bb;
		bb.left = bbStartX;
		bb.right = bbStartX + bbWidth;
		bb.top = bbStartY;
		bb.bottom = bbStartY + bbHeight;

		bbStartY += bbYOffset;

		addedSetSkillBBs.push_back(bb);
	}

	// Create radio button that decides how to display charm, either by name or skill name
	setSkillLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, X_OFFSET * 2, setSkillGBPos.y + TEXT_OUT_Y_OFFSET, 35, 12, mainHWND, (HMENU)ID_SET_SKILL_LR_RADIO_BUTTON, hInst, NULL);
	SendMessage(setSkillLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

	setSkillHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, (X_OFFSET * 2) + 40, setSkillGBPos.y + TEXT_OUT_Y_OFFSET, 35, 12, mainHWND, (HMENU)ID_SET_SKILL_HR_RADIO_BUTTON, hInst, NULL);
	SendMessage(setSkillHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
	// Note: Above radio buttons are grouped. 

	SendMessage(setSkillHighRankRadioButton, BM_SETCHECK, BST_CHECKED, 0);

	if (setting->highRankSetSkill)
	{
		SendMessage(setSkillLowRankRadioButton, BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(setSkillHighRankRadioButton, BM_SETCHECK, BST_CHECKED, 0);
	}
	else
	{
		SendMessage(setSkillLowRankRadioButton, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(setSkillHighRankRadioButton, BM_SETCHECK, BST_UNCHECKED, 0);
	}


	// set skill drop down list
	int dropdownListY = setSkillGBPos.y + TEXT_OUT_Y_OFFSET + 20;

	setSkillDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
		setSkillGBPos.x + X_OFFSET, dropdownListY, DROPDOWN_LIST_W, 400, mainHWND, (HMENU)ID_SET_SKILL_COMBOBOX, hInst, NULL);

	// set font
	SendMessage(setSkillDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

	refreshSetSkillCombobox();

	// add buttons
	{
		int buttonX = setSkillGBPos.x + X_OFFSET + 205;
		int buttonY = dropdownListY + 35;

		// Add skill button. Adds selected skill on skill dropdown menu
		addSetSkillButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::ADD_SKILLS).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonX, buttonY, 70, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_ADD_SET_SKILL_BUTTON, hInst, NULL);

		// set font
		SendMessage(addSetSkillButton, WM_SETFONT, (WPARAM)defaultFont, true);

		// move down
		buttonY += 30;

		// Clear added skills button. Clears all added skills on the list
		clearAllSetSkillsButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::CLEAR).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonX, buttonY, 50, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_CLEAR_SET_SKILL_BUTTON, hInst, NULL);

		// set font
		SendMessage(clearAllSetSkillsButton, WM_SETFONT, (WPARAM)defaultFont, true);
	}

	// Add check boxes
	int startX = addedSetSkillsGBPos.x + 180;
	int startY = addedSetSkillsGBPos.y + 20;

	int id = (int)ID_SET_SKILL_1_CHECKBOX;
	const int addedSetSkillSize = setting->setSkills.size();
	int index = 0;
	auto iter = setting->setSkills.begin();

	for (int i = 0; i < MAX_SET_SKILL_COUNT; ++i)
	{
		auto cbHwnd = CreateWindow(L"button", NULL, WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, startX, startY, 12, 12, mainHWND, (HMENU)(id + i), hInst, NULL);
		EnableWindow(cbHwnd, false);
		startY += 20;
		setSkillCheckboxes.push_back(cbHwnd);

		if (index < addedSetSkillSize)
		{
			EnableWindow(cbHwnd, true);

			if ((*iter)->applied)
			{
				SendMessage(cbHwnd, BM_SETCHECK, BST_CHECKED, 0);
			}
			else
			{
				SendMessage(cbHwnd, BM_SETCHECK, BST_UNCHECKED, 0);
			}
			iter++;
		}

		index++;
	}
}

void refreshSetSkillCombobox()
{
	// Reset dropdown list
	SendMessage(setSkillDropdownList, CB_RESETCONTENT, 0, 0);

	int indexCounter = 0;
	int newTempIndexCounter = 0;

	if (setting->highRankSetSkill)
	{
		// Refresh with high rank set skills
		for (auto& e : db->highRankSetSkills)
		{
			// Check if set skill is already added
			if (setting->isSetSkillAdded((e.second).id, (e.second).groupId, true))
			{
				// Already added. remove temp index
				db->highRankSetSkills[indexCounter].dropdownIndex = -1;
				// increment index counter
				indexCounter++;
				// next.
				continue;
			}

			// set skill is not added yet. Update temp index.
			db->highRankSetSkills[indexCounter].dropdownIndex = newTempIndexCounter;
			// increment index counter
			indexCounter++;
			// inc new temp index counter
			newTempIndexCounter++;

			// Show set skill name and required armor piece
			auto skillStr = (e.second).name + L" " + std::to_wstring((e.second).reqArmorPieces) + L" (" + (e.second).skillName + L")";

			// Add item
			SendMessage(setSkillDropdownList, CB_ADDSTRING, 0, (LPARAM)skillStr.c_str());
		}
	}
	else
	{
		// Refresh with low rank set skills
		for (auto& e : db->lowRankSetSkills)
		{
			// Check if set skill is already added
			if (setting->isSetSkillAdded((e.second).id, (e.second).groupId, false))
			{
				// Already added. remove temp index
				db->lowRankSetSkills[indexCounter].dropdownIndex = -1;
				// increment index counter
				indexCounter++;
				// next.
				continue;
			}

			// set skill is not added yet. Update temp index.
			db->lowRankSetSkills[indexCounter].dropdownIndex = newTempIndexCounter;
			// increment index counter
			indexCounter++;
			// inc new temp index counter
			newTempIndexCounter++;

			// Show set skill name and required armor piece
			auto skillStr = (e.second).name + L" " + std::to_wstring((e.second).reqArmorPieces) + L" (" + (e.second).skillName + L")";

			// Add item
			SendMessage(setSkillDropdownList, CB_ADDSTRING, 0, (LPARAM)skillStr.c_str());
		}
	}

	// Select first one if dropdown list is refreshed
	SendMessage(setSkillDropdownList, CB_SETCURSEL, setting->setSkillIndex, 0);
}

void refreshAddedSetSkillList()
{
	SetWindowText(addedSetSkillsGroupBox, (setting->getString(MHW::StringLiteral::ADDED_SET_SKILLS) + L" (" + std::to_wstring(setting->getTotalAddedSetSkills()) + L"/" + std::to_wstring(MAX_SET_SKILL_COUNT) + L")").c_str());

	redrawRect(addedSetSkillsGroupBox);
	redrawRect(setSkillDescriptionRect, false);
	redrawRect(totalSelectedSetSkillRect, false);

	if (setting)
	{
		if (setting->setSkills.empty())
		{
			for (int i = 0; i < MAX_SET_SKILL_COUNT; ++i)
			{
				SendMessage(setSkillCheckboxes.at(i), BM_SETCHECK, BST_UNCHECKED, 0);
				EnableWindow(setSkillCheckboxes.at(i), false);
			}
		}
		else
		{
			int index = 0;
			int curSetSkillSize = (int)setting->setSkills.size();

			auto iter = setting->setSkills.begin();

			for (int i = 0; i < MAX_SET_SKILL_COUNT; ++i)
			{
				if (index < curSetSkillSize)
				{
					if ((*iter)->applied)
					{
						SendMessage(setSkillCheckboxes.at(index), BM_SETCHECK, BST_CHECKED, 0);
					}
					else
					{
						SendMessage(setSkillCheckboxes.at(index), BM_SETCHECK, BST_UNCHECKED, 0);
					}
					EnableWindow(setSkillCheckboxes.at(index), true);

					iter++;
				}
				else
				{
					SendMessage(setSkillCheckboxes.at(index), BM_SETCHECK, BST_UNCHECKED, 0);
					EnableWindow(setSkillCheckboxes.at(index), false);
				}

				index++;
			}
		}
	}
}

void initArmorControls()
{
	const int lrRadioButtonX = armorGBPos.x + (X_OFFSET * 2) + 30;
	int armorRadioButtonY = armorGBPos.y + TEXT_OUT_Y_OFFSET + 2;
	const int hrRadioButtonX = lrRadioButtonX + 40;

	int dropdownListX = armorGBPos.x + 10;
	int dropdownListY = armorGBPos.y + 40;

	const int armorDropdownListWidth = 500;
	const int armorDropdownListHeight = 400;

	armorRankRadioButtonSupressed = true;

	{
		// Low rank head armor and high rank head armor
		headLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, lrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_HEAD_LOW_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(headLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		headHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, hrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_HEAD_HIGH_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(headHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		// Note: Above radio buttons are grouped. 

		refreshArmorRankRadioButton(headLowRankRadioButton, headHighRankRadioButton, setting->highRankHeadArmor);

		// add dropdown list
		headArmorDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
			dropdownListX, dropdownListY, armorDropdownListWidth, armorDropdownListHeight, mainHWND, (HMENU)ID_ARMOR_HEAD_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(headArmorDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// add items
		refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	}

	{
		armorRadioButtonY += ARMOR_TEXT_Y_OFFSET;
		dropdownListY += ARMOR_TEXT_Y_OFFSET;

		// Low rank chest armor and high rank chest armor
		chestLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, lrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_CHEST_LOW_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(chestLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		chestHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, hrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_CHEST_HIGH_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(chestHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

		refreshArmorRankRadioButton(chestLowRankRadioButton, chestHighRankRadioButton, setting->highRankChestArmor);

		// add dropdown list
		chestArmorDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
			dropdownListX, dropdownListY, armorDropdownListWidth, armorDropdownListHeight, mainHWND, (HMENU)ID_ARMOR_CHEST_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(chestArmorDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// add items
		refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	}

	{
		armorRadioButtonY += ARMOR_TEXT_Y_OFFSET;
		dropdownListY += ARMOR_TEXT_Y_OFFSET;

		// Low rank arm armor and high rank arm armor
		armLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, lrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_ARM_LOW_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(armLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		armHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, hrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_ARM_HIGH_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(armHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

		refreshArmorRankRadioButton(armLowRankRadioButton, armHighRankRadioButton, setting->highRankArmArmor);

		// add dropdown list
		armArmorDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
			dropdownListX, dropdownListY, armorDropdownListWidth, armorDropdownListHeight, mainHWND, (HMENU)ID_ARMOR_ARM_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(armArmorDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// add items
		refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	}

	{
		armorRadioButtonY += ARMOR_TEXT_Y_OFFSET;
		dropdownListY += ARMOR_TEXT_Y_OFFSET;

		// Low rank arm armor and high rank arm armor
		waistLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, lrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_WAIST_LOW_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(waistLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		waistHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, hrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_WAIST_HIGH_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(waistHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

		refreshArmorRankRadioButton(waistLowRankRadioButton, waistHighRankRadioButton, setting->highRankWaistArmor);

		// add dropdown list
		waistArmorDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
			dropdownListX, dropdownListY, armorDropdownListWidth, armorDropdownListHeight, mainHWND, (HMENU)ID_ARMOR_WAIST_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(waistArmorDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// add items
		refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	}

	{
		armorRadioButtonY += ARMOR_TEXT_Y_OFFSET;
		dropdownListY += ARMOR_TEXT_Y_OFFSET;

		// Low rank chest armor and high rank chest armor
		legLowRankRadioButton = CreateWindow(L"button", L"LR", WS_CHILD | WS_VISIBLE | WS_GROUP | BS_AUTORADIOBUTTON, lrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_LEG_LOW_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(legLowRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);
		legHighRankRadioButton = CreateWindow(L"button", L"HR", WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON, hrRadioButtonX, armorRadioButtonY, 35, 12, mainHWND, (HMENU)ID_ARMOR_LEG_HIGH_RANK_RADIO_BUTTON, hInst, NULL);
		SendMessage(legHighRankRadioButton, WM_SETFONT, (WPARAM)defaultFont, true);

		refreshArmorRankRadioButton(legLowRankRadioButton, legHighRankRadioButton, setting->highRankLegArmor);

		// add dropdown list
		legArmorDropdownList = CreateWindow(L"combobox", NULL, WS_CHILD | WS_VSCROLL | WS_VISIBLE | CBS_DROPDOWNLIST,
			dropdownListX, dropdownListY, armorDropdownListWidth, armorDropdownListHeight, mainHWND, (HMENU)ID_ARMOR_LEG_COMBOBOX, hInst, NULL);

		// set font
		SendMessage(legArmorDropdownList, WM_SETFONT, (WPARAM)defaultFont, true);

		// add items
		refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);
	}

	armorRankRadioButtonSupressed = false;
}

void refreshArmorDropdownList(HWND & armorComboboxHWND, std::map<int, Armor>& armors, const bool HR, int& dropdownIndex)
{
	SendMessage(armorComboboxHWND, CB_RESETCONTENT, 0, 0);
	SendMessage(armorComboboxHWND, CB_ADDSTRING, 0, (LPARAM)(setting->getString(MHW::StringLiteral::NONE)).c_str());

	int index = 0;

	for (auto& e : armors)
	{
		// reset dropdown list index
		(e.second).dropdownListIndex = -1;

		if (!setting->allowArenaArmor && (e.second).arenaArmor)
		{
			continue;
		}

		if (!setting->allowEventArmor && (e.second).eventArmor)
		{
			continue;
		}

		if ((HR && (e.second).highRank) || (!HR && !(e.second).highRank))
		{
			if ((e.second).gender == MHW::Gender::NONE)
			{
				// This armor doesn't ahve gender. Add.
				SendMessage(armorComboboxHWND, CB_ADDSTRING, 0, (LPARAM)(e.second).getDropdownListItemStr(db).c_str());

				// set index
				(e.second).dropdownListIndex = index;
				index++;
			}
			else
			{
				if ((e.second).gender == MHW::Gender::MALE && setting->gender == MHW::Gender::MALE)
				{
					// Gender is set to male and this armor is male armor
					SendMessage(armorComboboxHWND, CB_ADDSTRING, 0, (LPARAM)(e.second).getDropdownListItemStr(db).c_str());

					// set index
					(e.second).dropdownListIndex = index;
					index++;
				}
				else if ((e.second).gender == MHW::Gender::FEMALE && setting->gender == MHW::Gender::FEMALE)
				{
					// Gender is set to female and this armor is female armor
					SendMessage(armorComboboxHWND, CB_ADDSTRING, 0, (LPARAM)(e.second).getDropdownListItemStr(db).c_str());

					// set index
					(e.second).dropdownListIndex = index;
					index++;
				}
				// Else, wrong gender value
			}
		}
	}

	// Set to first item by default
	int ret = SendMessage(armorComboboxHWND, CB_SETCURSEL, dropdownIndex, 0);
	if (ret != dropdownIndex)
	{
		dropdownIndex = 0;
		SendMessage(armorComboboxHWND, CB_SETCURSEL, dropdownIndex, 0);
		setting->saveTemp();
	}
}

void refreshAllArmorDropdownList()
{
	refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);
}

void refreshAllDecoList()
{
	LVCOLUMN lvC = { 0 };
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.iSubItem = 0;

	ListView_GetColumn(allDecoList, 0, &lvC);

	std::wstring jewelNameStr = setting->getString(MHW::StringLiteral::DECO_COL_JEWEL_NAME);
	lvC.pszText = (LPWSTR)jewelNameStr.c_str();

	ListView_SetColumn(allDecoList, 0, &lvC);

	ListView_GetColumn(allDecoList, 1, &lvC);

	std::wstring jewelLevelStr = setting->getString(MHW::StringLiteral::DECO_COL_LEVEL);
	lvC.pszText = (LPWSTR)jewelLevelStr.c_str();

	ListView_SetColumn(allDecoList, 1, &lvC);

	/*
	ListView_GetColumn(allDecoList, 2, &lvC);

	std::wstring jewelRarityStr = setting->getString(MHW::StringLiteral::DECO_COL_RARITY);
	lvC.pszText = (LPWSTR)jewelRarityStr.c_str();

	ListView_SetColumn(allDecoList, 2, &lvC);
	*/

	ListView_GetColumn(allDecoList, 2, &lvC);

	std::wstring jewelSkillStr = setting->getString(MHW::StringLiteral::DECO_COL_SKILL);
	lvC.pszText = (LPWSTR)jewelSkillStr.c_str();

	ListView_SetColumn(allDecoList, 2, &lvC);

	ListView_GetColumn(allDecoList, 3, &lvC);

	std::wstring countStr = setting->getString(MHW::StringLiteral::DECO_COL_COUNT);
	lvC.pszText = (LPWSTR)countStr.c_str();

	ListView_SetColumn(allDecoList, 3, &lvC);

	LVN_supreesed = true;
	int row = 0;
	for (auto& e : db->decorations)
	{

		auto strList = (e.second).getStr(db);
		auto decoName = (e.second).name;

		if (setting->language == MHW::Language::ENGLISH)
		{
			decoName += L" ";
		}

		decoName += setting->getString(MHW::StringLiteral::JEWEL_SUFFIX);

		ListView_SetItemText(allDecoList, row, 0, (LPWSTR)(decoName.c_str()));

		if (!strList.empty())
		{
			if (strList.size() == 1)
			{
				ListView_SetItemText(allDecoList, row, 2, (LPWSTR)(strList.front().c_str()));
			}
			else
			{
				std::wstring setSkillStr = strList.at(0) + L" (" + strList.at(1) + L" " + strList.at(2) + L")";
				ListView_SetItemText(allDecoList, row, 2, (LPWSTR)((setSkillStr).c_str()));
			}
		}

		std::wstring countStr = std::to_wstring(setting->decorationCountList.at(row));
		ListView_SetItemText(allDecoList, row, 3, (LPWSTR)((countStr).c_str()));

		ListView_SetCheckState(allDecoList, (e.second).id, setting->decorationCheckList.at(row));

		row++;
	}
	LVN_supreesed = false;
}

void refreshMenu(const bool init)
{
	HMENU menu = GetMenu(mainHWND);
	auto option = GetSubMenu(menu, 1);

	// hard coded
	std::wstring genderStr = setting->getString(MHW::StringLiteral::MENU_GENDER);
	ModifyMenuW(option, 6, MF_BYPOSITION | MF_STRING, 0, (LPWSTR)genderStr.c_str());

	refreshMenuLanguage(menu, ID_FILE_SAVE, MHW::StringLiteral::MENU_SAVE);
	refreshMenuLanguage(menu, ID_FILE_OPEN, MHW::StringLiteral::MENU_OPEN);
	refreshMenuLanguage(menu, IDM_EXIT, MHW::StringLiteral::MENU_EXIT);
	refreshMenuLanguage(menu, ID_OPTION_CLEAR, MHW::StringLiteral::MENU_CLEAR);
	refreshMenuLanguage(menu, ID_OPTION_ALLOWLOWRANKARMORS, MHW::StringLiteral::MENU_ALLOW_LOW_RANK_ARMOR);
	refreshMenuCheck(menu, ID_OPTION_ALLOWLOWRANKARMORS, setting->allowLowRankArmor);
	refreshMenuLanguage(menu, ID_OPTION_ALLOWARENAARMOR, MHW::StringLiteral::MENU_ALLOW_ARENA_ARMOR);
	refreshMenuCheck(menu, ID_OPTION_ALLOWARENAARMOR, setting->allowArenaArmor);
	refreshMenuLanguage(menu, ID_OPTION_ALLOWEVENTARMOR, MHW::StringLiteral::MENU_ALLOW_EVENT_ARMOR);
	refreshMenuCheck(menu, ID_OPTION_ALLOWEVENTARMOR, setting->allowEventArmor);
	refreshMenuLanguage(menu, ID_GENDER_FEMALE, MHW::StringLiteral::MENU_GENDER_FEMALE);
	refreshMenuLanguage(menu, ID_GENDER_MALE, MHW::StringLiteral::MENU_GENDER_MALE);
	refreshMenuLanguage(menu, ID_HELP_REPORTERROR, MHW::StringLiteral::MENNU_REPORT_ERROR);
	toggleGenderMenu();
}

void refreshMenuLanguage(HMENU menu, const int id, const MHW::StringLiteral strEnum)
{
	MENUITEMINFOW mii = { sizeof(MENUITEMINFOW) };

	mii.fMask = MIIM_STATE | MIIM_TYPE;
	GetMenuItemInfoW(menu, id, FALSE, &mii);

	std::wstring text = setting->getString(strEnum);
	mii.dwTypeData = (LPWSTR)text.c_str();
	SetMenuItemInfo(menu, id, FALSE, &mii);
}

void refreshMenuCheck(HMENU menu, const int id, const bool flag)
{
	MENUITEMINFOW mii = { sizeof(MENUITEMINFOW) };

	mii.fMask = MIIM_STATE;
	GetMenuItemInfoW(menu, id, FALSE, &mii);

	if (mii.fState & MFS_CHECKED)
	{
		// checked
		if (!flag)
		{
			mii.fState ^= MFS_CHECKED;
		}
	}
	else
	{
		// unchecked
		if (flag)
		{
			mii.fState ^= MFS_CHECKED;
		}
	}

	SetMenuItemInfo(menu, id, FALSE, &mii);
}

void refreshCharmDisplaySetting()
{
	if (setting->charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_NAME)
	{
		SendMessage(charmDisplayTypeNameRadioButton, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(charmDisplayTypeSkillNameRadioButton, BM_SETCHECK, BST_UNCHECKED, 0);
	}
	else
	{
		SendMessage(charmDisplayTypeNameRadioButton, BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(charmDisplayTypeSkillNameRadioButton, BM_SETCHECK, BST_CHECKED, 0);
	}
}

void refreshArmorRankRadioButton(HWND hwnLR, HWND hwndHR, const bool flag)
{
	if (flag)
	{
		SendMessage(hwnLR, BM_SETCHECK, BST_UNCHECKED, 0);
		SendMessage(hwndHR, BM_SETCHECK, BST_CHECKED, 0);
	}
	else
	{
		SendMessage(hwnLR, BM_SETCHECK, BST_CHECKED, 0);
		SendMessage(hwndHR, BM_SETCHECK, BST_UNCHECKED, 0);
	}
}

void refreshWeaponDropdownlistEnable()
{
	if (setting->totalWeaponSlots == 0)
	{
		// 0 weapon slot
		EnableWindow(weaponSlot1SizeDropdownList, false);
		EnableWindow(weaponSlot2SizeDropdownList, false);
		EnableWindow(weaponSlot3SizeDropdownList, false);
	}
	else if (setting->totalWeaponSlots == 1)
	{
		// 1 weapon slot
		EnableWindow(weaponSlot1SizeDropdownList, true);
		EnableWindow(weaponSlot2SizeDropdownList, false);
		EnableWindow(weaponSlot3SizeDropdownList, false);
	}
	else if (setting->totalWeaponSlots == 2)
	{
		// 2 weapon slot
		EnableWindow(weaponSlot1SizeDropdownList, true);
		EnableWindow(weaponSlot2SizeDropdownList, true);
		EnableWindow(weaponSlot3SizeDropdownList, false);
	}
	else if (setting->totalWeaponSlots == 3)
	{
		// 3 weapon slot
		EnableWindow(weaponSlot1SizeDropdownList, true);
		EnableWindow(weaponSlot2SizeDropdownList, true);
		EnableWindow(weaponSlot3SizeDropdownList, true);
	}
}

void refreshSearchOptionCheckbox()
{
	SendMessage(searchOptionAllowExtraSkill, BM_SETCHECK, (setting->allowExtraSkills ? BST_CHECKED : BST_UNCHECKED), 0);
	SendMessage(searchOptionAllowOverleveledSkill, BM_SETCHECK, (setting->allowOverleveledSkills ? BST_CHECKED : BST_UNCHECKED), 0);
	SendMessage(searchOptionSearchFromHigherRarity, BM_SETCHECK, (setting->searchFromHigherArmorRarity ? BST_CHECKED : BST_UNCHECKED), 0);
	SendMessage(searchOptionSimplifyResult, BM_SETCHECK, (setting->simplifySearchResult ? BST_CHECKED : BST_UNCHECKED), 0);
	SendMessage(searchOptionShowMaxLevel, BM_SETCHECK, (setting->showMaxLevel ? BST_CHECKED : BST_UNCHECKED), 0);
}

void initDecorationControls()
{
	allDecoPos.x = decorationGBPos.x + 10;
	allDecoPos.y = decorationGBPos.y + 20;

	allDecoList = CreateWindow(WC_LISTVIEW, 0, WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, allDecoPos.x, allDecoPos.y, DECO_LISTBOX_W, DECO_LISTBOX_H, mainHWND, (HMENU)ID_ALL_DECO_LISTBOX, hInst, 0);
	//SendMessage(allDecoList, LVM_SETCOLUMNWIDTH, 0, 500);
	ListView_SetColumnWidth(allDecoList, 0, 500);
	//ListView_SetExtendedListViewStyle(allDecoList, LVS_EX_CHECKBOXES);
	ListView_SetExtendedListViewStyle(allDecoList, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);
	//ListView_SetExtendedListViewStyle(allDecoList, LVS_EX_FULLROWSELECT);
	SendMessage(allDecoList, WM_SETFONT, (WPARAM)defaultFont, true);

	/*
	//myDecoPos.x = decorationGBPos.x + 270;
	myDecoPos.x = allDecoPos.x;
	myDecoPos.y = allDecoPos.y + 150;

	myDecoList = CreateWindow(L"listbox", 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER, myDecoPos.x, myDecoPos.y, DECO_LISTBOX_W, DECO_LISTBOX_H - 30, mainHWND, (HMENU)ID_MY_DECO_LISTBOC, hInst, 0);
	SendMessage(myDecoList, WM_SETFONT, (WPARAM)defaultFont, true);

	excludedDecoPos.x = myDecoPos.x;
	excludedDecoPos.y = myDecoPos.y + 120;

	excludedDecoList = CreateWindow(L"listbox", 0, WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | WS_BORDER, excludedDecoPos.x, excludedDecoPos.y, DECO_LISTBOX_W, DECO_LISTBOX_H - 30, mainHWND, (HMENU)ID_MY_DECO_LISTBOC, hInst, 0);
	SendMessage(excludedDecoList, WM_SETFONT, (WPARAM)defaultFont, true);

	*/
	//decoDirIconPos.x = decorationGBPos.x + 181;
	//decoDirIconPos.y = decorationGBPos.y + 220;

	LVCOLUMN lvC;
	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	std::wstring jewelNameStr = setting->getString(MHW::StringLiteral::DECO_COL_JEWEL_NAME);
	lvC.pszText = (LPWSTR)(jewelNameStr.c_str());
	lvC.cx = 150;
	lvC.fmt = LVCFMT_LEFT;
	lvC.iSubItem = 0;
	ListView_InsertColumn(allDecoList, 0, &lvC);

	std::wstring jewelLevelStr = setting->getString(MHW::StringLiteral::DECO_COL_LEVEL);
	lvC.pszText = (LPWSTR)(jewelLevelStr.c_str());
	lvC.cx = 50;
	lvC.iSubItem = 1;
	ListView_InsertColumn(allDecoList, 1, &lvC);

	/*
	std::wstring jewelRarityStr = setting->getString(MHW::StringLiteral::DECO_COL_RARITY);
	lvC.pszText = (LPWSTR)(jewelRarityStr.c_str());
	lvC.cx = 50;
	lvC.iSubItem = 2;
	ListView_InsertColumn(allDecoList, 2, &lvC);
	*/

	std::wstring jewelSkillStr = setting->getString(MHW::StringLiteral::DECO_COL_SKILL);
	lvC.pszText = (LPWSTR)(jewelSkillStr.c_str());
	lvC.cx = 200;
	lvC.iSubItem = 2;
	ListView_InsertColumn(allDecoList, 2, &lvC);

	/*
	std::wstring countStr = L"Count";
	lvC.pszText = (LPWSTR)(countStr.c_str());
	lvC.cx = 60;
	lvC.iSubItem = 3;
	ListView_InsertColumn(allDecoList, 3, &lvC);
	*/

	std::wstring jewelCountStr = setting->getString(MHW::StringLiteral::DECO_COL_COUNT);
	lvC.pszText = (LPWSTR)(jewelCountStr.c_str());
	lvC.cx = 60;
	lvC.iSubItem = 3;
	ListView_InsertColumn(allDecoList, 3, &lvC);

	// fill list
	LVITEM lvI;

	// Initialize LVITEM members that are common to all items.
	lvI.mask = LVIF_TEXT;
	lvI.stateMask = 0;
	lvI.iSubItem = 0;

	int indexCheck = 0;

	LVN_supreesed = true;

	for (auto& deco : db->decorations)
	{
		auto strList = (deco.second).getStr(db);
		auto decoName = deco.second.name;

		if (setting->language == MHW::Language::ENGLISH)
		{
			decoName += L" ";
		}

		decoName += setting->getString(MHW::StringLiteral::JEWEL_SUFFIX);

		lvI.pszText = (LPWSTR)(decoName).c_str();
		lvI.iItem = (deco.second).id;

		assert(indexCheck == lvI.iItem);

#if _DEBUG
		OutputDebugString((L"Inserting decoration to list. Id(index): " + std::to_wstring(deco.second.id) + L", name: " + deco.second.name + L"\n").c_str());
#endif

		ListView_InsertItem(allDecoList, &lvI);

		std::wstring sizeStr = std::to_wstring(deco.second.size);
		ListView_SetItemText(allDecoList, indexCheck, 1, (LPWSTR)(sizeStr.c_str()));

		/*
		std::wstring rarityStr = std::to_wstring(deco.second.rarity);
		ListView_SetItemText(allDecoList, indexCheck, 2, (LPWSTR)(rarityStr.c_str()));
		*/

		if (!strList.empty())
		{
			if (strList.size() == 1)
			{
				ListView_SetItemText(allDecoList, indexCheck, 2, (LPWSTR)(strList.front().c_str()));
			}
			else
			{
				std::wstring setSkillStr = strList.at(0) + L" (" + strList.at(1) + L" " + strList.at(2) + L")";
				ListView_SetItemText(allDecoList, indexCheck, 2, (LPWSTR)((setSkillStr).c_str()));
			}
		}

		std::wstring countStr = std::to_wstring(setting->decorationCountList.at(indexCheck));
		ListView_SetItemText(allDecoList, indexCheck, 3, (LPWSTR)((countStr).c_str()));

		ListView_SetCheckState(allDecoList, (deco.second).id, setting->decorationCheckList.at(indexCheck));

		indexCheck++;
	}

	LVN_supreesed = false;
}

int initSearchResultWindow()
{
	if (searchResultHWND == nullptr)
	{
		OutputDebugString(L"Creating search window\n");
		searchResultHWND = CreateWindow(szSearchResultWindowClass, L"Search results", WS_POPUPWINDOW | WS_CAPTION | WS_SIZEBOX | WS_MAXIMIZEBOX | WS_MINIMIZEBOX, 100, 100, MIN_SEARCH_RESULT_WINDOW_W, MIN_SEARCH_RESULT_WINDOW_H, 0, 0, hInst, NULL);

		if (searchResultHWND)
		{
			ShowWindow(searchResultHWND, SW_HIDE);
			//UpdateWindow(searchResultHWND);

			RECT clientRect;
			GetClientRect(searchResultHWND, &clientRect);

			minClientAreaWidth = (clientRect.right - clientRect.left);
			minClientAreaHeight = (clientRect.bottom - clientRect.top);

			resultRichEdit = CreateWindow(RICHEDIT_CLASS, NULL, WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_BORDER | ES_READONLY, RICH_EDIT_X, RICH_EDIT_Y, MIN_SEARCH_RESULT_WINDOW_W - 40, MIN_SEARCH_RESULT_WINDOW_H - 100, searchResultHWND, NULL, hInst, NULL);
			SendMessage(resultRichEdit, WM_SETFONT, (WPARAM)defaultFont, true);
			SendMessage(resultRichEdit, EM_SETLIMITTEXT, -1, 0);

			searchProgressBar = CreateWindowEx(0, PROGRESS_CLASS, NULL,
				WS_CHILD | WS_VISIBLE, 10,
				20,
				490, 15,
				searchResultHWND, (HMENU)0, hInst, NULL);

			setSearcher->progressBarHWND = searchProgressBar;
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::FAILED_TO_INIT_SEARCH_WINDOW);
		}
	}

	return 0;
}

int initAbout()
{
	if (aboutHWND == nullptr)
	{
		aboutHWND = CreateDialog(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), mainHWND, About);

		ShowWindow(aboutHWND, SW_HIDE);

		//auto rect = getControlRect(ret);
		//ShowWindow(ret, SW_SHOWDEFAULT);
		//auto sl2 = CreateWindow(WC_LINK, L"Comments or error report to <a href=\"http://www.microsoft.com\">here</a>.", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 50, 60, 200, 30, aboutHWND, (HMENU)IDC_SYSLINK1, hInst, 0);
		//SendMessage(sl2, WM_SETFONT, (WPARAM)defaultFont, true);
		//auto err = GetLastError();
	}

	return 0;
}

void resizeSearchResultRichEdit()
{
	/*
	RECT clientRect;
	GetClientRect(mainHWND, &clientRect);

	int diff = (clientRect.bottom - clientRect.top) - minClientAreaHeight;

	SetWindowPos(searchResultGroupbox, NULL, searchResultGBPos.x, searchResultGBPos.y, RIGHT_COLUMN_WIDTH, searchResultGBHeight + diff, 0);
	SetWindowPos(resultRichEdit, NULL, searchResultGBPos.x + 10, searchResultGBPos.y + 20, RIGHT_COLUMN_WIDTH - 15, searchResultREHeight + diff, 0);
	*/

	RECT clientRect;
	GetClientRect(searchResultHWND, &clientRect);

	int diffWidth = (clientRect.right - clientRect.left) - minClientAreaWidth;
	int diffHeight = (clientRect.bottom - clientRect.top) - minClientAreaHeight;

	SetWindowPos(resultRichEdit, NULL, RICH_EDIT_X, RICH_EDIT_Y, MIN_SEARCH_RESULT_WINDOW_W - 40 + diffWidth, MIN_SEARCH_RESULT_WINDOW_H - 100 + diffHeight, 0);
	SetWindowPos(searchProgressBar, NULL, 10, 20, MIN_SEARCH_RESULT_WINDOW_W - 40 + diffWidth, 15, 0);
}

void initPopUpMenu()
{
	addedSkillPopUpMenu = nullptr;
	addedSetSkillPopUpMenu = nullptr;
}

void initButtons()
{
	//searchButtonPos.x = 780;
	//searchButtonPos.y = SEARCH_BUTTON_Y;
	searchButtonPos.x = 780;
	searchButtonPos.y = SEARCH_BUTTON_Y;

	searchButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SEARCH_BUTTON).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, searchButtonPos.x, searchButtonPos.y, 60, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_SEARCH_BUTTON, hInst, NULL);
	SendMessage(searchButton, WM_SETFONT, (WPARAM)defaultFont, true);

	/*
	clearSearchResultButtonPos.x = searchButtonPos.x - 130;
	clearSearchResultButtonPos.y = SEARCH_BUTTON_Y;

	// Doesn't really need. maybe put on menu bar

	clearSearchResultButton = CreateWindow(L"button", L"Clear Search Result", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, clearSearchResultButtonPos.x, clearSearchResultButtonPos.y, 120, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_CLEAR_SEARCH_RESULT_BUTTON, hInst, NULL);
	SendMessage(clearSearchResultButton, WM_SETFONT, (WPARAM)defaultFont, true);

	clearAllButtonPos.x = RIGHT_COLUMN_X;
	clearAllButtonPos.y = SEARCH_BUTTON_Y;

	clearAllButton = CreateWindow(L"button", L"Clear All", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, clearAllButtonPos.x, clearAllButtonPos.y, 80, BUTTON_SIZE_Y, hWnd, (HMENU)ID_CLEAR_ALL_BUTTON, hInst, NULL);
	SendMessage(clearAllButton, WM_SETFONT, (WPARAM)defaultFont, true);
	*/

	selectAllDecoButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SELECT_ALL).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, allDecoPos.x, allDecoPos.y + 300, 80, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_SELECT_ALL_DECO_BUTTON, hInst, NULL);
	SendMessage(selectAllDecoButton, WM_SETFONT, (WPARAM)defaultFont, true);

	deselectAllDecoButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::DESELECT_ALL).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, allDecoPos.x + 90, allDecoPos.y + 300, 80, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_DESELECT_ALL_DECO_BUTTON, hInst, NULL);
	SendMessage(deselectAllDecoButton, WM_SETFONT, (WPARAM)defaultFont, true);

	resetAllDecoCountButton = CreateWindow(L"button", setting->getString(MHW::StringLiteral::RESET_ALL_COUNT).c_str(), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, allDecoPos.x + 180, allDecoPos.y + 300, 100, BUTTON_SIZE_Y, mainHWND, (HMENU)ID_RESET_ALL_DECO_COUNT, hInst, NULL);
	SendMessage(resetAllDecoCountButton, WM_SETFONT, (WPARAM)defaultFont, true);
}

void initSearchOptions()
{
	const int x = RIGHT_COLUMN_X + 7;
	int y = 655;
	searchOptionAllowExtraSkill = CreateWindow(L"button", setting->getString(MHW::StringLiteral::ALLOW_EXTRA_SKILL).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_ALLOW_EXTRA_SKILL), hInst, NULL);
	SendMessage(searchOptionAllowExtraSkill, WM_SETFONT, (WPARAM)defaultFont, true);

	y += 22;
	searchOptionAllowOverleveledSkill = CreateWindow(L"button", setting->getString(MHW::StringLiteral::ALLOW_OVERLEVELED_SKILL).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_ALLOW_OVERLEVELED_SKILL), hInst, NULL);
	SendMessage(searchOptionAllowOverleveledSkill, WM_SETFONT, (WPARAM)defaultFont, true);

	y += 22;
	searchOptionSearchFromHigherRarity = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SEARCH_FROM_HIGHER_RARIRTY).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_SEARCH_FROM_HIGHER_RARIRTY), hInst, NULL);
	SendMessage(searchOptionSearchFromHigherRarity, WM_SETFONT, (WPARAM)defaultFont, true);

	/*
	y += 22;
	searchOptionUseMaxLevelCharmOnly = CreateWindow(L"button", L"Use max level charm only", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_USE_MAX_LEVEL_CHARM_ONLY), hInst, NULL);
	SendMessage(searchOptionUseMaxLevelCharmOnly, WM_SETFONT, (WPARAM)defaultFont, true);
	SendMessage(searchOptionUseMaxLevelCharmOnly, BM_SETCHECK, (setting->useOnlyMaxLevelCharm ? BST_CHECKED : BST_UNCHECKED), 0);
	*/

	y += 22;
	searchOptionSimplifyResult = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SIMPLIFY_SEARCH_RESULT).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_SIMPLIFY_SEARCH_RESULT), hInst, NULL);
	SendMessage(searchOptionSimplifyResult, WM_SETFONT, (WPARAM)defaultFont, true);

	y += 22;
	searchOptionShowMaxLevel = CreateWindow(L"button", setting->getString(MHW::StringLiteral::SHOW_MAX_LEVEL).c_str(), WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX, x, y, 200, 15, mainHWND, (HMENU)(ID_SEARCH_OPTION_SHOW_MAX_LEVEL), hInst, NULL);
	SendMessage(searchOptionShowMaxLevel, WM_SETFONT, (WPARAM)defaultFont, true);

	y = 651;
	const int x2 = x + 230;

	searchOptionMinArmorRarityCombobox = CreateWindow(L"combobox", L"Min rarity", WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST, x2, y, 40, 15, mainHWND, (HMENU)ID_SEARCH_OPTION_MIN_RARITY_DROPDOWNLIST, hInst, NULL);
	SendMessage(searchOptionMinArmorRarityCombobox, WM_SETFONT, (WPARAM)defaultFont, true);

	for (int i = 1; i <= MHW::CONSTS::MAX_ARMOR_RARITY; i++)
	{
		SendMessage(searchOptionMinArmorRarityCombobox, CB_ADDSTRING, 0, (LPARAM)std::to_wstring(i).c_str());
	}

	SendMessage(searchOptionMinArmorRarityCombobox, CB_SETCURSEL, setting->minArmorRarity - 1, 0);

	refreshSearchOptionCheckbox();
}

std::vector<std::string> splitByComma(const std::string str)
{
	std::vector<std::string> split;

	std::stringstream ss(str);
	std::string token;

	while (std::getline(ss, token, ','))
	{
		split.push_back(token);
	}

	return split;
}

bool isPointInRect(const RECT & rect, const POINT & point)
{
	return (rect.left <= point.x && point.x <= rect.right && rect.top <= point.y && point.y <= rect.bottom);
}

void updateMouseMove(const bool force)
{
	if (initialized == false) return;

	if (setting)
	{
		POINT curCursorPos = getCursorPos(true);

		if (force)
		{
			cursorPos = curCursorPos;
		}
		else
		{
			if ((cursorPos.x != curCursorPos.x) || (cursorPos.y != curCursorPos.y))
			{
				cursorPos = curCursorPos;
			}
			else
			{
				return;
			}
		}

		if (!blockMouseHoverUpdate && isPointInRect(addedSkillGBRect, curCursorPos))
		{
			// check added skill bbs
			if (setting->skills.empty())
			{
				// There is no skill added
				if (lastHoveringAddedSkillBBIndex != -1)
				{
					// was hovering something. Reset index.
					lastHoveringAddedSkillBBIndex = -1;

					// redraw skill description with the skill selected in dropdown list
					redrawRect(skillDescriptionRect, false);
				}
			}

			else
			{
				// index of added skill 
				int index = 0;
				// hover flag
				bool hovering = false;

				// get size of currently added skills
				int size = setting->skills.size();

				// iterate BBs
				for (auto& bb : addedSkillBBs)
				{
					if (isPointInRect(bb, curCursorPos))
					{
						// Cursor is hovering one of skill label.
						if (lastHoveringAddedSkillBBIndex == -1)
						{
							// Was hovering nothing. Mark as hovering
							hovering = true;

							// check index
							if (index < size)
							{
								// Valid index. Save.
								lastHoveringAddedSkillBBIndex = index;

								// redraw skill description with hovered skill
								redrawRect(skillDescriptionRect, false);

								// end of iteration
								break;
							}
						}
						else if (lastHoveringAddedSkillBBIndex != index)
						{
							// check index
							if (index < size)
							{
								// Was hovering nothing. Mark as hovering
								hovering = true;

								// Valid index. Save.
								lastHoveringAddedSkillBBIndex = index;

								// redraw skill description with hovered skill
								redrawRect(skillDescriptionRect, false);

								// end of iteration
								break;
							}
						}
						else if (lastHoveringAddedSkillBBIndex == index)
						{
							// hovering same skill
							hovering = true;

							// end of iteration
							break;
						}
					}

					// inc index
					index++;
				}

				if (!hovering)
				{
					// didn't hover any.
					if (lastHoveringAddedSkillBBIndex != -1)
					{
						// was hovering something. Reset index
						lastHoveringAddedSkillBBIndex = -1;

						// redraw skill description with the skill selected in dropdown list
						redrawRect(skillDescriptionRect, false);
					}
				}
			}
		}
		else if (isPointInRect(addedSetSkillGBRect, curCursorPos))
		{
			// Check added set skill bbs
			if (setting->setSkills.empty())
			{
				// There is no set skill added
				if (lastHoveringAddedSetSkillBBIndex != -1)
				{
					// was hovering something. Reset index.
					lastHoveringAddedSetSkillBBIndex = -1;

					// redraw set skill description with the skill selected in dropdown list
					redrawRect(setSkillDescriptionRect, false);
				}
			}
			else
			{
				// index of added set skill 
				int index = 0;
				// hover flag
				bool hovering = false;

				// get size of currently added set skills
				int size = setting->setSkills.size();

				// iterate BBs
				for (auto& bb : addedSetSkillBBs)
				{
					if (isPointInRect(bb, curCursorPos))
					{
						// Cursor is hovering one of set skill label.
						if (lastHoveringAddedSetSkillBBIndex == -1)
						{
							// Was hovering nothing. Mark as hovering
							hovering = true;

							// check index
							if (index < size)
							{
								// Valid index. Save.
								lastHoveringAddedSetSkillBBIndex = index;

								// redraw set skill description with hovered set skill
								redrawRect(setSkillDescriptionRect, false);

								// end of iteration
								break;
							}
						}
						else if (lastHoveringAddedSetSkillBBIndex != index)
						{
							// check index
							if (index < size)
							{
								// Was hovering nothing. Mark as hovering
								hovering = true;

								// Valid index. Save.
								lastHoveringAddedSetSkillBBIndex = index;

								// redraw set skill description with hovered set skill
								redrawRect(setSkillDescriptionRect, false);

								// end of iteration
								break;
							}
						}
						else if (lastHoveringAddedSetSkillBBIndex == index)
						{
							// hovering same set skill
							hovering = true;

							// end of iteration
							break;
						}
					}

					// inc index
					index++;
				}

				if (!hovering)
				{
					// didn't hover any.
					if (lastHoveringAddedSetSkillBBIndex != -1)
					{
						// was hovering something. Reset index
						lastHoveringAddedSetSkillBBIndex = -1;

						// redraw set skill description with the skill selected in dropdown list
						redrawRect(setSkillDescriptionRect, false);
					}
				}
			}
		}
	}
}

void updateMouseRightClick()
{
	// Get current cursor pos in client
	auto curCursorPos = getCursorPos(true);

	// check bb
	if (isPointInRect(addedSkillGBRect, curCursorPos))
	{
		// User right clicked in added skill group box
		int index = 0;

		// iterate through bounding boxes.
		for (auto& bb : addedSkillBBs)
		{
			if (isPointInRect(bb, curCursorPos))
			{
				// User clicked on one of bounding boxes. Get skill.
				Skill* skill = setting->getAddedSkillAt(index);

				if (skill == nullptr)
				{
					// There is no skill on this row.
					return;
				}

				// Got skill. Check if there was popup menu before
				if (addedSkillPopUpMenu)
				{
					// Destroy it.
					// Note: I do not know how to reset popup menu items.
					DestroyMenu(addedSkillPopUpMenu);
				}

				// Create new one. I don't think this is a heavy job since user won't use these feature gazillion times.
				addedSkillPopUpMenu = CreatePopupMenu();

				// Insert last item first.
				InsertMenu(addedSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, ID_POP_UP_MENU_REMOVE, setting->getString(MHW::StringLiteral::REMOVE_SKILL).c_str());

				int id = ID_POP_UP_MENU_REMOVE + skill->maxLevel;

				// Insert items.
				for (int i = skill->maxLevel; i >= 1; --i)
					//for (int i = 1; i <= skill->maxLevel; i++)
				{
					if (i != skill->level)
					{
						InsertMenu(addedSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, id, (LPCWSTR)((skill->name + L" " + std::to_wstring(i)).c_str()));
					}

					id--;
				}

				// Get cursor pos on screen
				auto newCursorPos = getCursorPos(false);
				// reposition the popup menu
				TrackPopupMenu(addedSkillPopUpMenu, TPM_TOPALIGN | TPM_LEFTALIGN, newCursorPos.x, newCursorPos.y, 0, mainHWND, NULL);

				// save index
				lastRightclickedSkillBBIndex = index;

				//blockMouseHoverUpdate = true;

				return;
			}

			// increment boudning box index
			index++;
		}

		// If it reaches here, user didn't clicked one of bounding boxes.
		lastRightclickedSkillBBIndex = -1;
		return;
	}
	else
	{
		// User didn't click added skill group box, no need to check
		lastRightclickedSkillBBIndex = -1;
	}

	// If it reaches here, user didn't clicked the added skill group box
	if (isPointInRect(addedSetSkillGBRect, curCursorPos))
	{
		// user clicked added set skillg roup box
		int index = 0;

		// iterate added set skills
		for (auto& bb : addedSetSkillBBs)
		{
			if (isPointInRect(bb, curCursorPos))
			{
				// Get set skill
				SetSkill* setSkill = setting->getAddedSetSkillAt(index);

				if (setSkill == nullptr)
				{
					// Set skill doesn't exists on this row.
					return;
				}

				// Same as added skill popup menu
				if (addedSetSkillPopUpMenu)
				{
					DestroyMenu(addedSetSkillPopUpMenu);
				}

				addedSetSkillPopUpMenu = CreatePopupMenu();

				// Insert last item
				InsertMenu(addedSetSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, ID_POP_UP_MENU_SS_REMOVE, setting->getString(MHW::StringLiteral::REMOVE_SET_SKILL).c_str());

				// save index
				lastRightclickedSetSkillBBIndex = index;

				if (setSkill->highRank == false)
				{
					// it's low rank. Only add remove item. Done.
				}
				else
				{
					// it's high rank

					// Check if this set skill has sibling
					if (setSkill->hasSibling && (setSkill->sibling != nullptr))
					{
						// display 2 set skill
						if (setSkill->reqArmorPieces < setSkill->sibling->reqArmorPieces)
						{
							InsertMenu(addedSetSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, ID_POP_UP_MENU_SS_1, (LPCWSTR)((setSkill->name + L" " + std::to_wstring(setSkill->sibling->reqArmorPieces)).c_str()));
						}
						else
						{
							InsertMenu(addedSetSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, ID_POP_UP_MENU_SS_1, (LPCWSTR)((setSkill->sibling->name + L" " + std::to_wstring(setSkill->sibling->reqArmorPieces)).c_str()));
						}
					}
					else
					{
						// Only add remove item. Done.
					}

				}

				auto newCursorPos = getCursorPos(false);
				TrackPopupMenu(addedSetSkillPopUpMenu, TPM_TOPALIGN | TPM_LEFTALIGN, newCursorPos.x, newCursorPos.y, 0, mainHWND, NULL);

				//blockMouseHoverUpdate = true;

				return;
			}

			index++;
		}

		lastRightclickedSetSkillBBIndex = -1;
		return;
	}
	else
	{
		lastRightclickedSetSkillBBIndex = -1;
	}
}

void checkWorkerThread()
{
	if (setSearcher)
	{
		auto curState = setSearcher->getState();
		if (curState == MHW::SetSearcher::State::ERR)
		{
			setSearcher->stop();
			setSearcher->join();
		}
		else if (curState == MHW::SetSearcher::State::STOPPED)
		{
			setSearcher->join();
		}
		else if (curState == MHW::SetSearcher::State::ABORTED)
		{
			setSearcher->notifyDoneCopyingResult();

			searchResultHeaderStr = setting->getString(MHW::StringLiteral::SEARCH_ABORTED) + L" " + setting->getString(MHW::StringLiteral::SEARCH_TOTAL) + std::to_wstring(searchedArmorSets.size()) + L"\n" + setting->getString(MHW::StringLiteral::FIRST_500_RESULT) + L"\n\n";

			SendMessage(resultRichEdit, WM_SETTEXT, 0, (LPARAM)(searchResultHeaderStr + searchResultStr).c_str());

			SendMessage(searchButton, WM_SETTEXT, 0, (LPARAM)(setting->getString(MHW::StringLiteral::SEARCH_BUTTON).c_str()));
		}
		else if (curState == MHW::SetSearcher::State::SEARCHING)
		{
			int resultSize = setSearcher->getResultSize();

			//if (resultSize > (int)searchedArmorSets.size())
			if (resultSize > 0)
			{
				std::vector<MHW::ArmorSet> newArmorSets;
				setSearcher->queryResults(newArmorSets);

				OutputDebugString((L"new search result query: " + std::to_wstring(resultSize) + L"\n").c_str());

				auto str = buildSearchResultText(newArmorSets);

				searchedArmorSets.insert(searchedArmorSets.end(), newArmorSets.begin(), newArmorSets.end());

				if (!str.empty())
				{
					const int lastPos = searchResultHeaderStr.length() + searchResultStr.length();
					SendMessage(resultRichEdit, EM_SETSEL, lastPos, lastPos);
					SendMessage(resultRichEdit, EM_REPLACESEL, 0, (LPARAM)(str).c_str());

					searchResultStr += str;
				}
			}
		}
		else if (curState == MHW::SetSearcher::State::FINISHED)
		{
			int resultSize = setSearcher->getResultSize();

			//if (resultSize > (int)searchedArmorSets.size())
			if (resultSize > 0)
			{
				std::vector<MHW::ArmorSet> newArmorSets;
				setSearcher->queryResults(newArmorSets);

				OutputDebugString((L"!!new search result query: " + std::to_wstring(resultSize) + L"\n").c_str());

				auto str = buildSearchResultText(newArmorSets);

				searchedArmorSets.insert(searchedArmorSets.end(), newArmorSets.begin(), newArmorSets.end());

				searchResultStr += str;
			}

			searchResultHeaderStr = setting->getString(MHW::StringLiteral::SEARCH_FINISHED) + L" " + setting->getString(MHW::StringLiteral::SEARCH_TOTAL) + std::to_wstring(searchedArmorSets.size()) + L"\n" + setting->getString(MHW::StringLiteral::FIRST_500_RESULT) + L"\n\n";

			SendMessage(resultRichEdit, WM_SETTEXT, 0, (LPARAM)(searchResultHeaderStr + searchResultStr).c_str());
			/*
			else
			{
				SendMessage(resultRichEdit, WM_SETTEXT, 0, (LPARAM)L"Search finished. Total armor set searched: 0");
			}
			*/

			setSearcher->notifyDoneCopyingResult();

			SendMessage(searchButton, WM_SETTEXT, 0, (LPARAM)(setting->getString(MHW::StringLiteral::SEARCH_BUTTON).c_str()));
		}
	}
}

std::wstring buildSearchResultText(std::vector<MHW::ArmorSet>& newArmorSets)
{
	std::wstring str = L"";

	if (searchedArmorSets.size() > 500)
	{
		return str;
	}

	for (auto& as : newArmorSets)
	{
		//searchResultStr += as.toResultStr(db);
		str += as.toResultStr(db, setting);

		if (as.id >= 500)
		{
			return str;
		}
	}

	return str;
}

int createSearchFilter(MHW::Filter & filter)
{
	if (setting)
	{
		if (db)
		{
			// Only check setting and db.

			filter.allowOverleveledSkill = setting->allowOverleveledSkills;

			// Get weapon slots
			filter.totalWeaponDecorations = setting->totalWeaponSlots;
			filter.weaponDecoSizes.push_back(setting->weaponSlot1Size);
			filter.weaponDecoSizes.push_back(setting->weaponSlot2Size);
			filter.weaponDecoSizes.push_back(setting->weaponSlot3Size);

			// Create skill filters. 

			// This filter will contain skill ID from all the selected skills that comes from armor
			std::unordered_set<int> skillFilters;
			// This filter will contain skill ID from all the selected skills that comes from decoration and effect is set skill.
			std::unordered_set<int> decoSetSkillFilters;
			// This filter will contains set skill ID from all the selected high rank set skills.
			std::unordered_set<int> highRankSetSkillFilter;
			// This filter will contains set skill ID from all the selected low rank set skills.
			std::unordered_set<int> lowRankSetSkillFilter;

			if (setting->skills.empty())
			{
				return static_cast<int>(MHW::ERROR_CODE::CFS_SKILL_IS_EMPTY);
			}
			else
			{
				for (auto skill : setting->skills)
				{
					if (skill->applied)
					{
						// only add that is applied
						if (skill->onlyFromDeco)
						{
							// These skills are only from decos (i.e. Paracoat, Sleepcoat, etc).
							// Don't add these to filter because there are no armor that gives these skills.
						}
						else if (skill->fromDecoSetSkill)
						{
							// From set skill. 
							decoSetSkillFilters.emplace(skill->id);
						}
						else
						{
							// From armor
							skillFilters.emplace(skill->id);
						}

						// Add skill id to require skill list
						filter.reqSkills.push_back(skill);
					}
				}

				if (filter.reqSkills.empty())
				{
					return static_cast<int>(MHW::ERROR_CODE::CFS_NO_CHECKED_SKILLS);
				}
			}

			if (!setting->setSkills.empty())
			{
				// Have selected set skills
				for (auto setSkill : setting->setSkills)
				{
					// only add that is applied
					if (setSkill->applied)
					{
						// check rank
						if (setSkill->highRank)
						{
							// set skill is high rank. Add group id
							highRankSetSkillFilter.emplace(setSkill->groupId);

							// Note: add group id, not unique set skill id.
							filter.reqHRSetSkill.push_back(setSkill);
						}
						else
						{
							// set skill is low rank
							lowRankSetSkillFilter.emplace(setSkill->groupId);

							// Note: add group id, not unique set skill id.
							filter.reqLRSetSkills.push_back(setSkill);
						}
					}
				}
			}

			// charm
			if (setting->charmIndex == 0/*None*/)
			{
				// make empty
				filter.charms.clear();
				filter.userPickedCharm = false;
			}
			else if (setting->charmIndex == 1/*Any*/)
			{
				// charm is any. Get all charms by skills, but only with min level. 
				db->getAllMinLevelCharmsBySkill(filter.charms, skillFilters, decoSetSkillFilters);
				filter.userPickedCharm = false;
			}
			else if (setting->charmIndex < 0)
			{
				return static_cast<int>(MHW::ERROR_CODE::CSF_CHARM_INDEX_IS_NEG_ONE);
			}
			else
			{
				// Add specific charm
				filter.charms.clear();
				Charm* charm = db->getCharmByID(setting->charmIndex);
				if (charm)
				{
					filter.charms.push_back(charm);
					filter.userPickedCharm = true;
				}
				else
				{
					// failed to get charm data
					return static_cast<int>(MHW::ERROR_CODE::CSF_FAILED_TO_GET_DECO_DATA);
				}
			}

			// armors

			// head
			if (setting->headArmorIndex == 0)
			{
				db->getArmorBySkill(db->headArmors, filter.headArmors, skillFilters, decoSetSkillFilters, lowRankSetSkillFilter, highRankSetSkillFilter, setting);
			}
			else
			{
				int dropdownListIndex = setting->headArmorIndex - 1;

				Armor* armor = db->getArmorByDropdownListIndex(db->headArmors, dropdownListIndex);

				if (armor)
				{
					filter.headArmors.push_back(armor);
				}
			}

			//db->getAnyArmor(filter.headArmors, db->anyHeadArmors, false, true);

			// chest
			if (setting->chestArmorIndex == 0)
			{
				db->getArmorBySkill(db->chestArmors, filter.chestArmors, skillFilters, decoSetSkillFilters, lowRankSetSkillFilter, highRankSetSkillFilter, setting);
			}
			else
			{
				int dropdownListIndex = setting->chestArmorIndex - 1;

				Armor* armor = db->getArmorByDropdownListIndex(db->chestArmors, dropdownListIndex);

				if (armor)
				{
					filter.chestArmors.push_back(armor);
				}
			}

			//db->getAnyArmor(filter.chestArmors, db->anyChestArmors, false, true);

			// arm
			if (setting->armArmorIndex == 0)
			{
				db->getArmorBySkill(db->armArmors, filter.armArmors, skillFilters, decoSetSkillFilters, lowRankSetSkillFilter, highRankSetSkillFilter, setting);
			}
			else
			{
				int dropdownListIndex = setting->armArmorIndex - 1;

				Armor* armor = db->getArmorByDropdownListIndex(db->armArmors, dropdownListIndex);

				if (armor)
				{
					filter.armArmors.push_back(armor);
				}
			}

			//db->getAnyArmor(filter.armArmors, db->anyArmArmors, false, true);

			// waist
			if (setting->waistArmorIndex == 0)
			{
				db->getArmorBySkill(db->waistArmors, filter.waistArmors, skillFilters, decoSetSkillFilters, lowRankSetSkillFilter, highRankSetSkillFilter, setting);
			}
			else
			{
				int dropdownListIndex = setting->waistArmorIndex - 1;

				Armor* armor = db->getArmorByDropdownListIndex(db->waistArmors, dropdownListIndex);

				if (armor)
				{
					filter.waistArmors.push_back(armor);
				}
			}

			//db->getAnyArmor(filter.waistArmors, db->anyWaistArmors, false, true);

			// leg
			if (setting->legArmorIndex == 0)
			{
				db->getArmorBySkill(db->legArmors, filter.legArmors, skillFilters, decoSetSkillFilters, lowRankSetSkillFilter, highRankSetSkillFilter, setting);
			}
			else
			{
				int dropdownListIndex = setting->legArmorIndex - 1;

				Armor* armor = db->getArmorByDropdownListIndex(db->legArmors, dropdownListIndex);

				if (armor)
				{
					filter.legArmors.push_back(armor);
				}
			}

			//db->getAnyArmor(filter.legArmors, db->anyLegArmors, false, true);

			bool hasDecorationToUse = false;

			for (auto b : setting->decorationCheckList)
			{
				if (b)
				{
					hasDecorationToUse = true;
				}
			}

			filter.hasDecorationToUse = hasDecorationToUse;
			filter.usableDecorations = setting->decorationCheckList;
			filter.decorationCount = setting->decorationCountList;

			return 0;
		}
		else
		{
			// db is nullptr
			return static_cast<int>(MHW::ERROR_CODE::CSF_DATABASE_IS_NULLPTR);
		}
	}
	else
	{
		// setting is nullptr
		return static_cast<int>(MHW::ERROR_CODE::CSF_SETTING_IS_NULLPTR);
	}
}

int checkFilter(MHW::Filter & filter)
{
	for (auto charm : filter.charms)
	{
		if (charm == nullptr)
		{
#if _DEBUG
			OutputDebugString(L"Charm is invalid\n");
#endif
			return static_cast<int>(MHW::ERROR_CODE::CF_CHARM_IS_NULLPTR);
		}
	}

	if (filter.totalWeaponDecorations < 0)
	{
		// wrong total weapon deco 
#if _DEBUG
		OutputDebugString(L"Invalid total decorations in weapon.\n");
#endif
		return static_cast<int>(MHW::ERROR_CODE::CF_TOTAL_WEAPON_DECO_IS_NEG);
	}

	for (auto val : filter.weaponDecoSizes)
	{
		if (val < 0)
		{
			// wrong total weapon deco size 
#if _DEBUG
			OutputDebugString(L"Invalid decoration size in weapon.\n");
#endif
			return static_cast<int>(MHW::ERROR_CODE::CF_WEAPON_DECO_SIZE_IS_NEG);
		}
	}

	if (filter.reqSkills.empty())
	{
#if _DEBUG
		OutputDebugString(L"Skill is empty.\n");
#endif
		return static_cast<int>(MHW::ERROR_CODE::CF_SKILL_IS_EMPTY);
	}

	if (!filter.reqHRSetSkill.empty() && !filter.reqLRSetSkills.empty())
	{
		if (filter.reqHRSetSkill.size() + filter.reqLRSetSkills.size() > 2)
		{
#if _DEBUG
			OutputDebugString(L"There are too many set skils\n");
#endif
			return static_cast<int>(MHW::ERROR_CODE::CF_TOO_MANY_SET_SKILL);
		}
	}
	else if (!filter.reqHRSetSkill.empty())
	{
		if (filter.reqHRSetSkill.size() > 2)
		{
#if _DEBUG
			OutputDebugString(L"There are too many high rank set skils\n");
#endif
			return static_cast<int>(MHW::ERROR_CODE::CF_TOO_MANY_HR_SET_SKILL);
		}
	}
	else if (!filter.reqLRSetSkills.empty())
	{
		if (filter.reqLRSetSkills.size() > 2)
		{
#if _DEBUG
			OutputDebugString(L"There are too many low rank set skils\n");
#endif
			return static_cast<int>(MHW::ERROR_CODE::CF_TOO_MANY_LR_SET_SKILL);
		}
	}

	const int totalSetSkillReqArmorPieces = setting->getTotalReqArmorPieces();

	if (totalSetSkillReqArmorPieces > 5)
	{
		return static_cast<int>(MHW::ERROR_CODE::CF_TOO_MANY_REQ_ARMOR_PIECES);
	}

	return 0;
}

void createDecoPopUpMenu(const int row)
{
	// Got skill. Check if there was popup menu before
	if (decorationPopUpMenu)
	{
		// Destroy it.
		// Note: I do not know how to reset popup menu items.
		DestroyMenu(decorationPopUpMenu);
	}

	// Create new one. I don't think this is a heavy job since user won't use these feature gazillion times.
	decorationPopUpMenu = CreatePopupMenu();

	// Insert last item first.
	auto find_deco = db->decorations.find(row);
	if (find_deco == db->decorations.end())
	{
		MHW::Logger::getInstance().error("Failed to get deco by id: " + std::to_string(row));
	}

	Decoration* deco = &(find_deco->second);

	Skill* skill = db->getSkillByIDAndLevel(deco->skillId, 1);

	const int max = skill->maxLevel;

	HMENU skillMenu = CreatePopupMenu();
	InsertMenuW(skillMenu, 0, MF_STRING, ID_POP_UP_MENU_DECO_STRING, (skill->description).c_str());

	for (int i = 1; i <= max; ++i)
	{
		Skill* skill = db->getSkillByIDAndLevel(deco->skillId, 1);
		if (skill)
		{
			InsertMenuW(skillMenu, 0, MF_STRING, ID_POP_UP_MENU_DECO_STRING, (setting->getString(MHW::StringLiteral::SKILL_LEVEL) + L" " + std::to_wstring(i) + L": " + skill->levelDescription).c_str());
		}
	}


	AppendMenuW(decorationPopUpMenu, MF_STRING, ID_POP_UP_MENU_DECO_STRING, (deco->name + L" " + setting->getString(MHW::StringLiteral::JEWEL_SUFFIX)).c_str());
	AppendMenuW(decorationPopUpMenu, MF_SEPARATOR, ID_POP_UP_MENU_DECO_SEPERATOR, NULL);
	AppendMenuW(decorationPopUpMenu, MF_POPUP, (UINT)skillMenu, (skill->name).c_str());
	AppendMenuW(decorationPopUpMenu, MF_SEPARATOR, ID_POP_UP_MENU_DECO_SEPERATOR, NULL);
	std::wstring rarityStr = setting->getString(MHW::StringLiteral::DECO_COL_RARITY) + L": " + std::to_wstring(deco->rarity);
	AppendMenuW(decorationPopUpMenu, MF_STRING, ID_POP_UP_MENU_DECO_STRING, (rarityStr).c_str());
	AppendMenuW(decorationPopUpMenu, MF_SEPARATOR, ID_POP_UP_MENU_DECO_SEPERATOR, NULL);

	int id = (int)ID_POP_UP_MENU_DECO_COUNT_START;

	for (int i = 0; i <= max; ++i)
	{
		std::wstring countStr = setting->getString(MHW::StringLiteral::DECO_COL_COUNT) + L": " + std::to_wstring(i);
		id++;
		AppendMenuW(decorationPopUpMenu, MF_STRING, id, countStr.c_str());
	}



	//InsertMenu(decorationPopUpMenu, 0, MF_BYPOSITION | MF_SEPARATOR, ID_POP_UP_MENU_REMOVE, NULL);

	//InsertMenu(decorationPopUpMenu, 0, MF_BYPOSITION | MF_STRING, ID_POP_UP_MENU_REMOVE, (deco->name + L" " + setting->getString(MHW::StringLiteral::JEWEL_SUFFIX)).c_str());
	/*
	int id = ID_POP_UP_MENU_REMOVE + skill->maxLevel;

	// Insert items.
	for (int i = skill->maxLevel; i >= 1; --i)
		//for (int i = 1; i <= skill->maxLevel; i++)
	{
		if (i != skill->level)
		{
			InsertMenu(addedSkillPopUpMenu, 0, MF_BYPOSITION | MF_STRING, id, (LPCWSTR)((skill->name + L" " + std::to_wstring(i)).c_str()));
		}

		id--;
	}
	*/

	// Get cursor pos on screen
	auto newCursorPos = getCursorPos(false);
	// reposition the popup menu
	TrackPopupMenu(decorationPopUpMenu, TPM_TOPALIGN | TPM_LEFTALIGN, newCursorPos.x, newCursorPos.y, 0, mainHWND, NULL);
}

POINT getCursorPos(const bool screenToClient)
{
	POINT cursorPos;
	bool success = GetCursorPos(&cursorPos);
	if (screenToClient)
	{
		ScreenToClient(mainHWND, &cursorPos);
	}
	return cursorPos;
}

RECT getControlRect(HWND & controlHWND)
{
	RECT rect;
	GetWindowRect(controlHWND, &rect);

	POINT pos;
	pos.x = rect.left;
	pos.y = rect.top;
	ScreenToClient(mainHWND, &pos);

	int w = rect.right - rect.left;
	int h = rect.bottom - rect.top;

	rect.left = pos.x;
	rect.top = pos.y;
	rect.right = pos.x + w;
	rect.bottom = pos.y + h;

	return rect;
}

void redrawRect(HWND & targetHWND)
{
	InvalidateRect(mainHWND, &getControlRect(targetHWND), NULL);
}

void redrawRect(RECT targetRect, const bool convert)
{
	if (convert)
	{
		POINT pos;
		pos.x = targetRect.left;
		pos.y = targetRect.top;
		ScreenToClient(mainHWND, &pos);

		int w = targetRect.right - targetRect.left;
		int h = targetRect.bottom - targetRect.top;

		targetRect.left = pos.x;
		targetRect.top = pos.y;
		targetRect.right = pos.x + w;
		targetRect.bottom = pos.y + h;
	}

	InvalidateRect(mainHWND, &targetRect, NULL);
}

void updateLeftMouseButtonClick()
{
	/*
	// Left mouse down/
	POINT cursorPos;
	bool success = GetCursorPos(&cursorPos);
	ScreenToClient(hWnd, &cursorPos);

	if (success)
	{
	#if _DEBUG
	OutputDebugString(L"Left mouse down\n");
	#endif

	bool ret = false;
	int index = 0;

	for (auto& skill : setting->skills)
	{
	// Get bb
	RECT& BB = removeSkillButtonBBs.at(index);

	if(isPointInRect(BB, cursorPos))
	{
	setting->removeAddedSkillAt(index);

	lastHoveringAddedSkillBBIndex = -1;

	updateMouseMove(true);

	refreshSkillCombobox();
	refreshAddedSkillList();

	ret = true;

	break;
	}

	index++;
	}

	if (ret) break;

	index = 0;

	for (auto& ss : setting->setSkills)
	{
	RECT& BB = removeSetSkillButtonBBs.at(index);

	if (isPointInRect(BB, cursorPos))
	{
	setting->removeAddedSetSkillAt(index);

	lastHoveringAddedSetSkillBBIndex = -1;

	updateMouseMove(true);

	refreshSetSkillCombobox();
	refreshAddedSetSkillList();

	break;
	}

	index++;
	}
	}
	*/
}

void updateWindowSize(HWND hwnd, LPARAM lParam)
{
	if (hwnd == mainHWND)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = MIN_WINDOW_W;
		lpMMI->ptMinTrackSize.y = MIN_WINDOW_H;
	}
	else if (hwnd == searchResultHWND)
	{
		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;
		lpMMI->ptMinTrackSize.x = MIN_SEARCH_RESULT_WINDOW_W;
		lpMMI->ptMinTrackSize.y = MIN_SEARCH_RESULT_WINDOW_H;
	}
}

void updateWeaponControls(WPARAM wParam, LPARAM lParam)
{
	//OutputDebugString(L"Weapon total slots\n");

	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		int cbRow = -1;

		wchar_t  strText[255] = { 0 };

		cbRow = (int)SendMessage(totalWeaponSlotsDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow >= 0)
		{
			// save to settings
			setting->totalWeaponSlots = cbRow;

			setting->saveTemp();

			refreshWeaponDropdownlistEnable();
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::WEAPON_DDL_ROW_INVALID);
		}
	}
	break;
	default:
		break;
	}
}

void updateWeaponSlot1(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		const int cbRow = (int)SendMessage(weaponSlot1SizeDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow >= 0)
		{
			setting->weaponSlot1Size = cbRow + 1;
			setting->saveTemp();
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::WEAPON_SLOT_1_DDL_ROW_INVALID);
		}
	}
	break;
	default:
		break;
	}
}

void updateWeaponSlot2(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		const int cbRow = (int)SendMessage(weaponSlot2SizeDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow >= 0)
		{
			setting->weaponSlot2Size = cbRow + 1;
			setting->saveTemp();
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::WEAPON_SLOT_2_DDL_ROW_INVALID);
		}
	}
	break;
	default:
		break;
	}
}

void updateWeaponSlot3(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		const int cbRow = (int)SendMessage(weaponSlot3SizeDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow >= 0)
		{
			setting->weaponSlot3Size = cbRow + 1;
			setting->saveTemp();
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::WEAPON_SLOT_3_DDL_ROW_INVALID);
		}
	}
	break;
	default:
		break;
	}
}

void updateCharmDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_SELENDCANCEL:
	{
		PostMessage(charmDropdownList, CB_SETCURSEL, setting->charmIndex, 0);
		SetFocus(mainHWND);

#if _DEBUG
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_SELENDCANCEL \n");
#endif
	}
	break;
	case CBN_SELENDOK:
	{
		const int cbRow = (int)SendMessage(charmDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow < 0)
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::CHARM_DDL_ROW_INVALID);
		}
		else
		{
			setting->charmIndex = cbRow;
			setting->saveTemp();
		}

		SetFocus(mainHWND);

#if _DEBUG
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_SELENDOK \n");
#endif
	}
	break;
	case CBN_CLOSEUP:
	{
#if _DEBUG
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_CLOSEUP\n");
#endif
	}
	break;
	case CBN_SELCHANGE:
	{
#if _DEBUG
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_SELCHANGE\n");
#endif
	}
	break;
	case CBN_DROPDOWN:
	{
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_DROPDOWN\n");
	}
	break;
	case CBN_EDITCHANGE:
	{
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_EDITCHANGE\n");
	}
	break;
	case CBN_SETFOCUS:
	{
		auto ret = SendMessage(charmDropdownList, CB_GETDROPPEDSTATE, 0, 0);
		if (!ret)
		{
			PostMessage(charmDropdownList, CB_SHOWDROPDOWN, TRUE, 0);
			PostMessage(mainHWND, WM_SETCURSOR, 0, 0);
		}

#if _DEBUG
		OutputDebugString((L"cb = " + std::to_wstring(cb) + L"\n").c_str());
		OutputDebugString(L"CBN_SETFOCUS\n");
#endif
	}
	break;
	default:
		//OutputDebugString((L"d cb = " + std::to_wstring(cb) + L"\n").c_str());
		break;
	}
}

void updateCharmDisplayByName()
{
	if (setting->charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME)
	{
		setting->charmDisplaySetting = MHW::CONSTS::CHARM_DISPLAY_BY_NAME;
		refreshCharmDropdownList();
		setting->saveTemp();
	}

	SetFocus(mainHWND);
}

void updateCharmDisplayBySkillName()
{
	if (setting->charmDisplaySetting == MHW::CONSTS::CHARM_DISPLAY_BY_NAME)
	{
		setting->charmDisplaySetting = MHW::CONSTS::CHARM_DISPLAY_BY_SKILL_NAME;
		refreshCharmDropdownList();
		setting->saveTemp();
	}

	SetFocus(mainHWND);
}

void updateSkillDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_SELENDCANCEL:
	{
		PostMessage(skillDropdownList, CB_SETCURSEL, setting->skillIndex, 0);
		SetFocus(mainHWND);
		blockMouseHoverUpdate = false;
	}
	break;
	case CBN_SELENDOK:
	{
		const int cbRow = (int)SendMessage(skillDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow < 0)
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SKILL_DDL_ROW_INVALUD);
		}
		else
		{
			setting->skillIndex = cbRow;
			setting->saveTemp();
		}

		SetFocus(mainHWND);

		InvalidateRect(mainHWND, &skillDescriptionRect, NULL);
		UpdateWindow(mainHWND);
		blockMouseHoverUpdate = false;
	}
	break;
	case CBN_SETFOCUS:
	{
		auto ret = SendMessage(skillDropdownList, CB_GETDROPPEDSTATE, 0, 0);
		if (!ret)
		{
			PostMessage(skillDropdownList, CB_SHOWDROPDOWN, TRUE, 0);
			PostMessage(mainHWND, WM_SETCURSOR, 0, 0);
		}
		blockMouseHoverUpdate = true;
	}
	break;
	case CBN_CLOSEUP:
	case CBN_SELCHANGE:
	case CBN_DROPDOWN:
	case CBN_EDITCHANGE:
	default:
		break;
	}
}

void updateAddSkillButon()
{
	if (setting->skills.size() >= MAX_SKILL_COUNT)
	{
		return;
	}

	//const int cbRow = (int)SendMessage(skillDropdownList, CB_GETCURSEL, 0, 0);

	if (setting->skillIndex >= 0)
	{
		// Add skill to setting
		int index = 0;
		for (auto& e : db->skills)
		{
			if ((e.second).dropdownIndex == setting->skillIndex)
			{
				setting->skills.push_back(&db->skills[index]);
				setting->skills.back()->applied = true;

				break;
			}

			index++;
		}
	}
	else
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::ADD_SKILL_DDL_ROW_INVALID);
		return;
	}

	setting->skillIndex = 0;
	setting->saveTemp();

	refreshSkillCombobox();
	refreshAddedSkillList();

	SetFocus(mainHWND);
}

void updateClearSkillsButton()
{
	setting->skills.clear();
	setting->setSkillIndex = 0;
	setting->saveTemp();

	lastHoveringAddedSkillBBIndex = -1;

	refreshSkillCombobox();
	refreshAddedSkillList();

	SetFocus(mainHWND);
}

void updateSkillCheckbox(const int wmId)
{
	int index = wmId - (int)ID_SKILL_1_CHECKBOX;

	if (index >= (int)setting->skills.size())
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SKILL_CHECKBOX_INVALID_INDEX);
		return;
	}

	auto iter = setting->skills.begin();
	std::advance(iter, index);

	(*iter)->applied = !(*iter)->applied;
	setting->saveTemp();

	SetFocus(mainHWND);
}

void updateSetSkillDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_SELENDCANCEL:
	{
		PostMessage(setSkillDropdownList, CB_SETCURSEL, setting->setSkillIndex, 0);
		SetFocus(mainHWND);
	}
	break;
	case CBN_SELENDOK:
	{
		const int cbRow = (int)SendMessage(setSkillDropdownList, CB_GETCURSEL, 0, 0);

		if (cbRow < 0)
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SET_SKILL_DDL_ROW_INVALUD);
		}
		else
		{
			setting->setSkillIndex = cbRow;
			setting->saveTemp();
		}

		SetFocus(mainHWND);

		InvalidateRect(mainHWND, &setSkillDescriptionRect, NULL);
		UpdateWindow(mainHWND);
	}
	break;
	case CBN_SETFOCUS:
	{
		auto ret = SendMessage(setSkillDropdownList, CB_GETDROPPEDSTATE, 0, 0);
		if (!ret)
		{
			PostMessage(setSkillDropdownList, CB_SHOWDROPDOWN, TRUE, 0);
			PostMessage(mainHWND, WM_SETCURSOR, 0, 0);
		}
	}
	break;
	case CBN_CLOSEUP:
	case CBN_SELCHANGE:
	case CBN_DROPDOWN:
	case CBN_EDITCHANGE:
	default:
		break;
	}
}

void updateSetSkillLowRankRadioButton()
{
	if (setting->highRankSetSkill)
	{
		setting->highRankSetSkill = false;
		setting->setSkillIndex = 0;
		setting->saveTemp();
		refreshSetSkillCombobox();
		redrawRect(setSkillDescriptionRect, false);
	}

	SetFocus(mainHWND);
}

void updateSetSkillHighRankRadioButton()
{
	if (!setting->highRankSetSkill)
	{
		setting->highRankSetSkill = true;
		setting->setSkillIndex = 0;
		setting->saveTemp();
		refreshSetSkillCombobox();
		redrawRect(setSkillDescriptionRect, false);
	}

	SetFocus(mainHWND);
}

void updateAddSetSkillButton()
{
	if (setting->setSkills.size() >= MAX_SET_SKILL_COUNT)
	{
		return;
	}

	//const int cbRow = (int)SendMessage(setSkillDropdownList, CB_GETCURSEL, 0, 0);

	if (setting->setSkillIndex >= 0)
	{
		// Add skill to setting
		int index = 0;

		if (setting->highRankSetSkill)
		{
			// Add high rank set skill
			// todo, optional: Create LUT for dropdown index(cbrow) to high rank set skill index.
			for (auto& e : db->highRankSetSkills)
			{
				// Check if dropdownIndex matches
				if ((e.second).dropdownIndex == setting->setSkillIndex)
				{
					// match. add set skill
					setting->setSkills.push_back(&db->highRankSetSkills[index]);
					setting->setSkills.back()->applied = true;
					setting->setSkillIndex = 0;

					break;
				}

				index++;
			}
		}
		else
		{
			// Add low rank high skill
			for (auto& e : db->lowRankSetSkills)
			{
				if ((e.second).dropdownIndex == setting->setSkillIndex)
				{
					setting->setSkills.push_back(&db->lowRankSetSkills[index]);
					setting->setSkills.back()->applied = true;
					setting->setSkillIndex = 0;

					break;
				}

				index++;
			}
		}
	}
	else
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::ADD_SET_SKILL_DDL_ROW_INVALID);
		return;
	}

	refreshSetSkillCombobox();
	refreshAddedSetSkillList();
	setting->saveTemp();
	SetFocus(mainHWND);
}

void updateClearSetSkillsButton()
{
	setting->setSkills.clear();
	setting->saveTemp();

	lastHoveringAddedSetSkillBBIndex = -1;

	refreshSetSkillCombobox();
	refreshAddedSetSkillList();

	SetFocus(mainHWND);
}

void updateSetSkillCheckbox(const int wmId)
{
	int index = wmId - (int)ID_SET_SKILL_1_CHECKBOX;

	if (index >= (int)setting->skills.size())
	{
		MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SET_SKILL_CHECKBOX_INVALID_INDEX);
		return;
	}

	auto iter = setting->setSkills.begin();
	std::advance(iter, index);

	(*iter)->applied = !(*iter)->applied;

	refreshAddedSetSkillList();
	setting->saveTemp();
	SetFocus(mainHWND);
}

void updateHeadArmorDropdownList(WPARAM wParam)
{

	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		setting->headArmorIndex = (int)SendMessage(headArmorDropdownList, CB_GETCURSEL, 0, 0);
		setting->saveTemp();

		//SetFocus(hWnd);
	}
	break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	default:
		break;
	}
}

void updateHeadArmorLowRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (setting->highRankHeadArmor)
	{
		// set to low rank
		setting->highRankHeadArmor = false;
		setting->headArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateHeadArmorHighRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (!setting->highRankHeadArmor)
	{
		// set to high rank
		setting->highRankHeadArmor = true;
		setting->headArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateChestArmorDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		setting->chestArmorIndex = (int)SendMessage(chestArmorDropdownList, CB_GETCURSEL, 0, 0);
		setting->saveTemp();
	}
	break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	default:
		break;
	}
}

void updateChestArmorLowRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (setting->highRankChestArmor)
	{
		// set to low rank
		setting->highRankChestArmor = false;
		setting->chestArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateChestArmorHighRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (!setting->highRankChestArmor)
	{
		// set to high rank
		setting->highRankChestArmor = true;
		setting->chestArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateArmArmorDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		setting->armArmorIndex = (int)SendMessage(armArmorDropdownList, CB_GETCURSEL, 0, 0);
		setting->saveTemp();

		//SetFocus(hWnd);
	}
	break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	default:
		break;
	}
}

void updateArmArmorLowRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (setting->highRankArmArmor)
	{
		// set to low rank
		setting->highRankArmArmor = false;
		setting->armArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateArmArmorHighRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (!setting->highRankArmArmor)
	{
		// set to high rank
		setting->highRankArmArmor = true;
		setting->armArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateWaistArmorDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		setting->waistArmorIndex = (int)SendMessage(waistArmorDropdownList, CB_GETCURSEL, 0, 0);
		setting->saveTemp();

		//SetFocus(hWnd);
	}
	break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	default:
		break;
	}
}

void updateWaistArmorLowRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (setting->highRankWaistArmor)
	{
		// set to low rank
		setting->highRankWaistArmor = false;
		setting->waistArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateWaistArmorHighRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (!setting->highRankWaistArmor)
	{
		// set to high rank
		setting->highRankWaistArmor = true;
		setting->waistArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateLegArmorDropdownList(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		setting->legArmorIndex = (int)SendMessage(legArmorDropdownList, CB_GETCURSEL, 0, 0);
		setting->saveTemp();

		//SetFocus(hWnd);
	}
	break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	default:
		break;
	}
}

void updateLegArmorLowRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (setting->highRankLegArmor)
	{
		// set to low rank
		setting->highRankLegArmor = false;
		setting->legArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);
	}

	SetFocus(mainHWND);
}

void updateLegArmorHighRankRadioButton()
{
	if (armorRankRadioButtonSupressed) return;

	if (!setting->highRankLegArmor)
	{
		// set to high rank
		setting->highRankLegArmor = true;
		setting->legArmorIndex = 0;
		setting->saveTemp();
		refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);
	}

	SetFocus(mainHWND);
}

int updateSearchButton()
{
	// check if there are enough setting to find.
	// If there isn't enough information to search, show alert box
	// Else, search
	// todo: use popup to confirm search?
	if (setSearcher)
	{
		if (db)
		{
			if (setting)
			{
				const auto curState = setSearcher->getState();
				if (curState == MHW::SetSearcher::State::SEARCHING)
				{
					// still searching. abrot
					setSearcher->abortSearching(false);
				}
				else if (curState == MHW::SetSearcher::State::IDLE)
				{
					// start search
					MHW::Filter filter;
					// create search filter
					int result = createSearchFilter(filter);

					if (result != 0)
					{
						return result;
					}

					// Check if filter is valid
					result = checkFilter(filter);

					if (result != 0)
					{
						return result;
					}
#if _DEBUG
#endif
					filter.print(db);

					searchedArmorSets.clear();
					searchResultStr.clear();

					SendMessage(resultRichEdit, EM_REPLACESEL, 0, 0);

					searchResultHeaderStr = setting->getString(MHW::StringLiteral::SEARCHING) + L"\n" + setting->getString(MHW::StringLiteral::FIRST_500_RESULT) + L"\n\n";
					SendMessage(resultRichEdit, WM_SETTEXT, 0, (LPARAM)(searchResultHeaderStr).c_str());

					SendMessage(searchButton, WM_SETTEXT, 0, (LPARAM)(setting->getString(MHW::StringLiteral::STOP_BUTTON).c_str()));

					setSearcher->search(filter);

					ShowWindow(searchResultHWND, SW_SHOW);

					SetFocus(searchResultHWND);
				}
			}
			else
			{
				return static_cast<int>(MHW::ERROR_CODE::USB_SETTING_IS_NULLPTR);
			}
		}
		else
		{
			return static_cast<int>(MHW::ERROR_CODE::USB_DATABASE_IS_NULLPTR);
		}
	}
	else
	{
		return static_cast<int>(MHW::ERROR_CODE::USB_SETSEARCHER_IS_NULLPTR);
	}

	return 0;
}

void updateListViewNotification(LPARAM lParam)
{

	LPNMLISTVIEW pNMLV = (LPNMLISTVIEW)lParam;

	switch (pNMLV->hdr.code)
	{
	case NM_DBLCLK:
	{
		OutputDebugString(L"NM_DBCLICK\n");
		/*
		//SendMessage(allDecoList, LVM_EDITLABELW, 1, 0);

		long x, y;
		x = (long)LOWORD(lParam);
		y = (long)HIWORD(lParam);
		LVHITTESTINFO itemclicked;
		auto cursorPos = getCursorPos(true);
		itemclicked.pt.x = cursorPos.x;
		itemclicked.pt.y = cursorPos.y;
		int lResult = ListView_SubItemHitTest(allDecoList, &itemclicked);
		//If SubItemHitTest doesn't return any error (lResult!=-1), it gets the Rect
		//of the SubItem (or Item) clicked, and creates an EditBox (hEdit) with the
		//same size as the SubItem. Then, it sets focus on hEdit and sets a
		//callback function (EditProc) for it.
		if (lResult != -1) {
			RECT subitemrect;
			ListView_GetSubItemRect(allDecoList, itemclicked.iItem, itemclicked.iSubItem, LVIR_BOUNDS, &subitemrect);
			int altura = subitemrect.bottom - subitemrect.top;
			int largura = subitemrect.right - subitemrect.left;
			if (itemclicked.iSubItem == 0) { largura = largura / 3; };
			//NOTE: the ListView has 4 columns;
			//when iSubItem == 0 (an item is clicked),
			//the width (largura) is divided by 4,
			//because for items (not subitems) the
			//width returned is that of the whole row.
			auto hEdit = CreateWindowEx(WS_EX_CLIENTEDGE, L"EDIT", L"", WS_CHILD | WS_VISIBLE | ES_WANTRETURN,
				subitemrect.left, subitemrect.top, largura, 1.5*altura, mainHWND, 0, GetModuleHandle(NULL), NULL);
			if (hEdit == NULL) { MessageBox(allDecoList, L"Could not create edit box.", L"Error", MB_OK | MB_ICONERROR); };
			SetFocus(hEdit);
			//EOldProc = (WNDPROC)SetWindowLong(hEdit, GWL_WNDPROC, (LONG)EditProc);
		}
		*/
	}
	break;
	case NM_CLICK:
	{
		OutputDebugString(L"NM_CLICK\n");
	}
	break;
	case NM_RCLICK:
	{
		LVITEM lvI;
		lvI.iItem = ((NMITEMACTIVATE*)lParam)->iItem;
		lvI.iSubItem = ((NMITEMACTIVATE*)lParam)->iSubItem;
		OutputDebugString((L"NM_RCLICK. iItem: " + std::to_wstring(lvI.iItem) + L", iSubItem: " + std::to_wstring(lvI.iSubItem) + L"\n").c_str());

		createDecoPopUpMenu(lvI.iItem);

		lastrightclickedDecoIndex = lvI.iItem;
	}
	break;
	/*
case LVN_BEGINLABELEDIT:
{
	LVITEM lvI;
	lvI.iItem = ((NMITEMACTIVATE*)lParam)->iItem;
	lvI.iSubItem = ((NMITEMACTIVATE*)lParam)->iSubItem;
	OutputDebugString(L"LVN_BEGINLABELEDIT\n");
}
break;
case LVN_ENDLABELEDIT:
{
	OutputDebugString(L"LVN_ENDLABELEDIT\n");
}
break;
	*/
	case LVN_ITEMCHANGED:
	{
		if (initialized && !LVN_supreesed)
		{
			if (pNMLV->uChanged & LVIF_STATE) /* Item state has been changed */
			{
				switch (pNMLV->uNewState & LVIS_STATEIMAGEMASK)
				{
				case 0x2000: /* CHECKED */
				{
					//OutputDebugString(L"Checked\n");
					//db->decorations[pNMLV->iItem].print();
					if (setting && !setting->decorationCheckList.empty())
					{
						setting->decorationCheckList[pNMLV->iItem] = true;
						setting->saveTemp();
					}
				}
				break;
				case 0x1000: /* UNCHECKED */
				{
					//OutputDebugString(L"Unchecked\n");
					if (setting && !setting->decorationCheckList.empty())
					{
						setting->decorationCheckList[pNMLV->iItem] = false;
						setting->saveTemp();
					}
				}
				break;
				default:
					break;
				}
			}
		}
	}
	break;
	default:
		break;
	}
}

void selectAllDecorations()
{
	if (setting)
	{
		LVN_supreesed = true;

		const int size = (int)setting->decorationCheckList.size();

		for (int i = 0; i < size; i++)
		{
			ListView_SetCheckState(allDecoList, i, true);
			setting->decorationCheckList.at(i) = true;
		}

		setting->saveTemp();
		SetFocus(mainHWND);

		LVN_supreesed = false;
	}
}

void deselectAllDecorations()
{
	if (setting)
	{
		LVN_supreesed = true;

		const int size = (int)setting->decorationCheckList.size();

		for (int i = 0; i < size; i++)
		{
			ListView_SetCheckState(allDecoList, i, false);
			setting->decorationCheckList.at(i) = false;
		}

		setting->saveTemp();
		SetFocus(mainHWND);

		LVN_supreesed = false;
	}
}

void resetAllDecoCount()
{
	if (setting)
	{
		setting->decorationCountList.clear();
		setting->loadDecoCounts(db);
		setting->saveTemp();

		refreshAllDecoList();
	}
}

void removeAddedSkillFromPopupMenu()
{
	if (lastRightclickedSkillBBIndex != -1)
	{
		setting->removeAddedSkillAt(lastRightclickedSkillBBIndex);
		setting->saveTemp();

		updateMouseMove(true);

		refreshSkillCombobox();
		refreshAddedSkillList();
	}
}

void removeAddedSetSkillFromPopupMenu()
{
	if (lastRightclickedSetSkillBBIndex != -1)
	{
		setting->removeAddedSetSkillAt(lastRightclickedSetSkillBBIndex);
		setting->saveTemp();

		updateMouseMove(true);

		refreshSetSkillCombobox();
		refreshAddedSetSkillList();
	}
}

void switchAddedSkillLevel(const int wmId)
{
	int index = wmId - ID_POP_UP_MENU_REMOVE;

	if (lastRightclickedSkillBBIndex != -1)
	{
		Skill* skill = setting->getAddedSkillAt(lastRightclickedSkillBBIndex);
		Skill* newSkill = db->getSkillByIDAndLevel(skill->id, index);

		if (skill && newSkill)
		{
			setting->removeAddedSkillAt(lastRightclickedSkillBBIndex);
			newSkill->applied = true;
			setting->addSkillAt(lastRightclickedSkillBBIndex, newSkill);
			setting->saveTemp();
		}
		else
		{
			if (skill == nullptr)
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::FAILED_TO_CHANGE_SKILL_LEVEL_SKILL_IS_NULLPTR);
			}

			if (newSkill == nullptr)
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::FAILED_TO_CHANGE_SKILL_LEVEL_NEW_SKILL_IS_NULLPTR);
			}
		}

		updateMouseMove(true);

		//refreshSkillCombobox();
		refreshAddedSkillList();
	}
}

void switchAddedSetSkill()
{
	if (lastRightclickedSetSkillBBIndex != -1)
	{
		SetSkill* setSkill = setting->getAddedSetSkillAt(lastRightclickedSetSkillBBIndex);
		SetSkill* sibling = setSkill->sibling;

		if (setSkill && sibling)
		{
			// Remove set skill from setting. This was copy so it's ok to remove
			setting->removeAddedSetSkillAt(lastRightclickedSetSkillBBIndex);
			// Add sibling. This will make copy.
			if (lastRightclickedSetSkillBBIndex == 0)
			{
				setting->setSkills.push_front(sibling);
				setting->setSkills.front()->applied = true;
			}
			else if (lastRightclickedSetSkillBBIndex == 1)
			{
				setting->setSkills.push_back(sibling);
				setting->setSkills.back()->applied = true;
			}
			setting->saveTemp();
		}
		else
		{
			if (setSkill == nullptr)
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::FAILED_TO_CHANGE_SET_SKILL_LEVEL_SET_SKILL_IS_NULLPTR);
			}

			if (sibling == nullptr)
			{
				MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::FAILED_TO_CHANGE_SET_SKILL_SIBLING_SET_SKILL_IS_NULLPTR);
			}
		}
		updateMouseMove(true);

		//refreshSkillCombobox();
		refreshAddedSetSkillList();
	}
}

void switchGender(const MHW::Gender gender)
{
	if (setting->gender == gender)
	{
		return;
	}

	if (setting->gender == MHW::Gender::MALE)
	{
		setting->gender = MHW::Gender::FEMALE;
		setting->saveTemp();
	}
	else if (setting->gender == MHW::Gender::FEMALE)
	{
		setting->gender = MHW::Gender::MALE;
		setting->saveTemp();
	}
	else
	{
		return;
	}

	refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);

	toggleGenderMenu();
}

void updateMinRarity(WPARAM wParam)
{
	int cb = HIWORD(wParam);
	switch (cb)
	{
	case CBN_CLOSEUP:
		SetFocus(mainHWND);
		break;
	case CBN_SELENDCANCEL:
		SetFocus(mainHWND);
		break;
	case CBN_SELCHANGE:
	{
		const int cbRow = (int)SendMessage(searchOptionMinArmorRarityCombobox, CB_GETCURSEL, 0, 0);

		if (cbRow >= 0)
		{
			setting->minArmorRarity = cbRow + 1;
			setting->saveTemp();
			OutputDebugString((L"min rarity: " + std::to_wstring(setting->minArmorRarity)).c_str());
		}
		else
		{
			MHW::Logger::getInstance().errorCode(MHW::ERROR_CODE::SEARCH_OPTION_MIN_RARITY_DDL_ROW_INVALID);
		}
	}
	break;
	default:
		break;
	}
}

void toggleAllowLowRankArmorOption()
{
	setting->allowLowRankArmor = !setting->allowLowRankArmor;
	setting->saveTemp();

	refreshMenuCheck(GetMenu(mainHWND), ID_OPTION_ALLOWLOWRANKARMORS, setting->allowLowRankArmor);
}

void toggleAllowArenaArmorOption()
{
	setting->allowArenaArmor = !setting->allowArenaArmor;
	setting->saveTemp();

	refreshMenuCheck(GetMenu(mainHWND), ID_OPTION_ALLOWARENAARMOR, setting->allowArenaArmor);
	// There is only head arena armors
	refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
}

void toggleAllowEventArmorOption()
{
	setting->allowEventArmor = !setting->allowEventArmor;
	setting->saveTemp();

	refreshMenuCheck(GetMenu(mainHWND), ID_OPTION_ALLOWEVENTARMOR, setting->allowEventArmor);

	// Note: MHW v3.0 adds Kulve Taroth, which is event monster with full armor set
	refreshAllArmorDropdownList();
}

void toggleGenderMenu()
{
	HMENU menu = GetMenu(mainHWND);

	refreshMenuCheck(GetMenu(mainHWND), ID_GENDER_FEMALE, (setting->gender == MHW::Gender::FEMALE));
	refreshMenuCheck(GetMenu(mainHWND), ID_GENDER_MALE, (setting->gender == MHW::Gender::MALE));
}

void switchLanguage(const MHW::Language language, const bool force)
{
	auto& logger = MHW::Logger::getInstance();
	logger.info("Attempting to switch language from " + setting->getLanguagePath() + " to " + setting->getLanguagePath(language));

	if (!force)
	{
		if (setting->language == language)
		{
			logger.info("Language is already " + setting->getLanguagePath());
			return;
		}
	}

	setting->language = language;
	setting->initStringLiterals();

	int result = db->reloadNames(setting);

	if (result != 0)
	{
		logger.errorCode(result);
		logger.error("Failed to change language to " + setting->getLanguagePath() + ". Reverting to English (default).");
		showErrorMsgBox(L"Failed to change language to " + Utility::utf8_decode(setting->getLanguagePath()) + L". Reverting to English (default).", false);

		setting->language = MHW::Language::ENGLISH;
		setting->initStringLiterals();

		result = db->reloadNames(setting);

		if (result != 0)
		{
			logger.errorCode(result);
			logger.error("Failed to change language back to English.");
			showErrorMsgBox(L"Failed to change language back to English.", false);
		}
	}

	// group boxes
	SetWindowText(weaponGroupbox, (setting->getString(MHW::StringLiteral::WEAPON_GB).c_str()));
	SetWindowText(charmGroupbox, (setting->getString(MHW::StringLiteral::CHARM_GB).c_str()));
	SetWindowText(skillGroupbox, (setting->getString(MHW::StringLiteral::SKILLS_GB).c_str()));
	SetWindowText(addedSkillsGroupBox, (setting->getString(MHW::StringLiteral::ADDED_SKILLS) + L" (" + std::to_wstring(setting->getTotalAddedSkills()) + L"/" + std::to_wstring(MAX_SKILL_COUNT) + L")").c_str());
	SetWindowText(setSkillGroupbox, (setting->getString(MHW::StringLiteral::SET_SKILL_GB).c_str()));
	SetWindowText(addedSetSkillsGroupBox, (setting->getString(MHW::StringLiteral::ADDED_SET_SKILLS) + L" (" + std::to_wstring(setting->getTotalAddedSetSkills()) + L"/" + std::to_wstring(MAX_SET_SKILL_COUNT) + L")").c_str());
	SetWindowText(armorGroupbox, (setting->getString(MHW::StringLiteral::ARMOR_FILTER_GB).c_str()));
	SetWindowText(decorationGroupbox, (setting->getString(MHW::StringLiteral::DECORATION_FILTER_GB).c_str()));
	SetWindowText(searchOptionGroupBox, (setting->getString(MHW::StringLiteral::SEARCH_OPTION_GB).c_str()));

	// radio button
	SetWindowText(charmDisplayTypeNameRadioButton, (setting->getString(MHW::StringLiteral::CHARM_DISPLAY_BY_NAME).c_str()));
	SetWindowText(charmDisplayTypeSkillNameRadioButton, (setting->getString(MHW::StringLiteral::CHARM_DISPLAY_BY_SKILL_NAME).c_str()));

	// buttons
	SetWindowText(addSkillButton, (setting->getString(MHW::StringLiteral::ADD_SKILLS).c_str()));
	SetWindowText(clearAllSkillsButton, (setting->getString(MHW::StringLiteral::CLEAR).c_str()));
	SetWindowText(addSetSkillButton, (setting->getString(MHW::StringLiteral::ADD_SKILLS).c_str()));
	SetWindowText(clearAllSetSkillsButton, (setting->getString(MHW::StringLiteral::CLEAR).c_str()));
	SetWindowText(selectAllDecoButton, (setting->getString(MHW::StringLiteral::SELECT_ALL).c_str()));
	SetWindowText(deselectAllDecoButton, (setting->getString(MHW::StringLiteral::DESELECT_ALL).c_str()));
	SetWindowText(resetAllDecoCountButton, (setting->getString(MHW::StringLiteral::RESET_ALL_COUNT).c_str()));
	SetWindowText(searchButton, (setting->getString(MHW::StringLiteral::SEARCH_BUTTON).c_str()));

	refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);

	refreshCharmDropdownList();
	refreshSkillCombobox();
	refreshAddedSkillList();
	refreshSetSkillCombobox();
	refreshAddedSetSkillList();

	refreshAllDecoList();

	SetWindowText(searchOptionAllowExtraSkill, (setting->getString(MHW::StringLiteral::ALLOW_EXTRA_SKILL).c_str()));
	SetWindowText(searchOptionAllowOverleveledSkill, (setting->getString(MHW::StringLiteral::ALLOW_OVERLEVELED_SKILL).c_str()));
	SetWindowText(searchOptionSearchFromHigherRarity, (setting->getString(MHW::StringLiteral::SEARCH_FROM_HIGHER_RARIRTY).c_str()));
	SetWindowText(searchOptionSimplifyResult, (setting->getString(MHW::StringLiteral::SIMPLIFY_SEARCH_RESULT).c_str()));
	SetWindowText(searchOptionShowMaxLevel, (setting->getString(MHW::StringLiteral::SHOW_MAX_LEVEL).c_str()));

	refreshMenu(false);

	redrawRect(mainHWND);

	logger.info("Successfully changed language. Saving...");
	setting->saveTemp();
}

void reloadUI()
{
	// weapon decoration
	SendMessage(totalWeaponSlotsDropdownList, CB_SETCURSEL, setting->totalWeaponSlots, 0);
	SendMessage(weaponSlot1SizeDropdownList, CB_SETCURSEL, setting->weaponSlot1Size - 1, 0);
	SendMessage(weaponSlot2SizeDropdownList, CB_SETCURSEL, setting->weaponSlot2Size - 1, 0);
	SendMessage(weaponSlot3SizeDropdownList, CB_SETCURSEL, setting->weaponSlot3Size - 1, 0);
	refreshWeaponDropdownlistEnable();

	// charm
	refreshCharmDisplaySetting();
	refreshCharmDropdownList();

	// skill
	refreshSkillCombobox();
	refreshAddedSkillList();

	// set skill
	refreshArmorRankRadioButton(setSkillLowRankRadioButton, setSkillHighRankRadioButton, setting->highRankSetSkill);
	refreshSetSkillCombobox();
	refreshAddedSetSkillList();

	// armor
	refreshArmorRankRadioButton(headLowRankRadioButton, headHighRankRadioButton, setting->highRankHeadArmor);
	refreshArmorRankRadioButton(chestLowRankRadioButton, chestHighRankRadioButton, setting->highRankChestArmor);
	refreshArmorRankRadioButton(armLowRankRadioButton, armHighRankRadioButton, setting->highRankArmArmor);
	refreshArmorRankRadioButton(waistLowRankRadioButton, waistHighRankRadioButton, setting->highRankWaistArmor);
	refreshArmorRankRadioButton(legLowRankRadioButton, legHighRankRadioButton, setting->highRankLegArmor);
	refreshArmorDropdownList(headArmorDropdownList, db->headArmors, setting->highRankHeadArmor, setting->headArmorIndex);
	refreshArmorDropdownList(chestArmorDropdownList, db->chestArmors, setting->highRankChestArmor, setting->chestArmorIndex);
	refreshArmorDropdownList(armArmorDropdownList, db->armArmors, setting->highRankArmArmor, setting->armArmorIndex);
	refreshArmorDropdownList(waistArmorDropdownList, db->waistArmors, setting->highRankWaistArmor, setting->waistArmorIndex);
	refreshArmorDropdownList(legArmorDropdownList, db->legArmors, setting->highRankLegArmor, setting->legArmorIndex);

	// deco
	refreshAllDecoList();

	// search option
	refreshSearchOptionCheckbox();
	SendMessage(searchOptionMinArmorRarityCombobox, CB_SETCURSEL, setting->minArmorRarity - 1, 0);

	// menu
	HMENU menu = GetMenu(mainHWND);
	refreshMenuCheck(menu, ID_OPTION_ALLOWLOWRANKARMORS, setting->allowLowRankArmor);
	refreshMenuCheck(menu, ID_OPTION_ALLOWARENAARMOR, setting->allowArenaArmor);
	refreshMenuCheck(menu, ID_OPTION_ALLOWEVENTARMOR, setting->allowEventArmor);

	redrawRect(mainHWND);
}

void showErrorMsgBox(const std::wstring & message, const bool exit)
{
	auto ret = MessageBoxW(mainHWND, message.c_str(), L"Error", MB_OK | MB_ICONWARNING | MB_APPLMODAL);

	if (exit)
	{
		if (ret == IDOK)
		{
			clear();

			DestroyWindow(mainHWND);
		}
	}
}

void showErrorCodeMsgBox(const MHW::ERROR_CODE errcode, const bool exit)
{
	auto ret = MessageBoxA(mainHWND, ("Error: " + std::to_string(static_cast<int>(errcode))).c_str(), "Error", MB_OK | MB_ICONWARNING | MB_APPLMODAL);

	if (exit)
	{
		if (ret == IDOK)
		{
			clear();

			DestroyWindow(mainHWND);
		}
	}
}

void save()
{
	OPENFILENAMEW ofn;

	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = (LPCWSTR)L"All Files (*.*)\0*.*\0";
	ofn.lpstrFile = (LPWSTR)szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
	//ofn.lpstrDefExt = (LPCWSTR)L"txt";

	auto& logger = MHW::Logger::getInstance();

	if (GetSaveFileNameW(&ofn) != 0)
	{
		// Save confirmed
		std::wstring filePath(ofn.lpstrFile);
		std::wstring log = L"Saved setting to: ";
		logger.infoW((log + filePath).c_str());

		setting->saveTemp(filePath);
	}
	else
	{
		// save canceled
		logger.info("Save canceled");
	}

	logger.flush();
}

void open()
{
	OPENFILENAMEW ofn;

	wchar_t szFileName[MAX_PATH] = L"";

	ZeroMemory(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = (LPCWSTR)L"All Files (*.*)\0*.*\0";
	ofn.lpstrFile = (LPWSTR)szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;
	//ofn.lpstrDefExt = (LPCWSTR)L"txt";

	auto& logger = MHW::Logger::getInstance();

	auto curLang = setting->language;

	if (GetOpenFileNameW(&ofn) != 0)
	{
		// Save confirmed
		std::wstring filePath(ofn.lpstrFile);
		std::wstring log = L"Opening setting at: ";
		logger.infoW((log + filePath).c_str());

		int result = setting->openTemp(filePath);
		if (result != 0)
		{
			logger.errorCode(result);
			showErrorMsgBox(setting->getString(MHW::StringLiteral::FAILED_TO_OPEN_FILE) + L"\nError code: " + std::to_wstring(result), false);
		}
		else
		{
			result = setting->load(db);

			if (result != 0)
			{
				logger.errorCode(result);
				return;
			}
			else
			{
				reloadUI();
				if (curLang != setting->language)
				{
					switchLanguage(setting->language, true);
				}
				setting->saveTemp();
			}
		}
	}
	else
	{
		// save canceled
		logger.info("Open canceled");
	}

	logger.flush();
}

void clear()
{
	auto& logger = MHW::Logger::getInstance();

	logger.info("Clearing before window closes...");

	if (setSearcher->getState() == MHW::SetSearcher::State::SEARCHING)
	{
		// still searching. abrot
		setSearcher->abortSearching(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	if (initialized)
	{
		if (hMod)
		{
			FreeLibrary(hMod);
		}
	}

	if (setting)
	{
		setting->saveTemp();
		delete setting;
		setting = nullptr;
	}

	if (db)
	{
		delete db;
		db = nullptr;
	}

	if (setSearcher)
	{
		setSearcher->stop();
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		if (setSearcher->join())
		{
			logger.info("Thread successfully joined");
		}

		delete setSearcher;
		setSearcher = nullptr;
	}

	logger.info("Finished clearing");
}