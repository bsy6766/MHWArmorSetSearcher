#ifndef MHW_ARMOR_SET_H
#define MHW_ARMOR_SET_H

#include "resource.h"

#include "Charm.h"
#include "Skill.h"
#include "Armor.h"
#include "Decoration.h"
#include "SetSearcher.h"
#include "ErrorCode.h"
#include "GlobalEnums.h"

#include <unordered_map>
#include <CommCtrl.h>
#include <ShellScalingAPI.h>
#include <Commdlg.h>

// foward declaration
class Settings;
class Database;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;																		// current instance
WCHAR szWindowClass[MAX_LOADSTRING];														// the main window class name
WCHAR szSearchResultWindowClass[MAX_LOADSTRING];

std::wstring windowTitle;

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM				MyRegisterSearchResultClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int, const std::wstring);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// default font for all windows
HFONT defaultFont;	

// main window
HWND mainHWND;
HWND searchResultHWND = nullptr;
HWND aboutHWND = nullptr;

// instances
static Settings* setting = nullptr;
static Database* db = nullptr;
static MHW::SetSearcher* setSearcher = nullptr;

// flag
bool initialized;
bool LVN_supreesed;
bool armorRankRadioButtonSupressed;

// weapon
HWND totalWeaponSlotsDropdownList;
HWND weaponSlot1SizeDropdownList;
HWND weaponSlot2SizeDropdownList;
HWND weaponSlot3SizeDropdownList;

// charm
HWND charmDropdownList;
HWND charmDisplayTypeNameRadioButton;
HWND charmDisplayTypeSkillNameRadioButton;

// skill
HWND skillDropdownList;
HWND addSkillButton;
HWND clearAllSkillsButton;

// set skill
HWND setSkillDropdownList;
HWND addSetSkillButton;
HWND clearAllSetSkillsButton;
HWND setSkillLowRankRadioButton;
HWND setSkillHighRankRadioButton;

// Rects
RECT totalSelectedSkillRect;
RECT skillDescriptionRect;
RECT addedSkillGBRect;
RECT totalSelectedSetSkillRect;
RECT setSkillDescriptionRect;
RECT addedSetSkillGBRect;

// skill bb
std::vector<RECT> addedSkillBBs;
// last hovering bb
int lastHoveringAddedSkillBBIndex;
// flag
bool blockMouseHoverUpdate;
// last right clicked added skill bb index
int lastRightclickedSkillBBIndex;
// last right clicked deco
int lastrightclickedDecoIndex;

// Set skill bb
std::vector<RECT> addedSetSkillBBs;
// last hovering bb
int lastHoveringAddedSetSkillBBIndex;
// last right clicked added skill bb index
int lastRightclickedSetSkillBBIndex;

// checkboxes
std::vector<HWND> skillCheckboxes;
std::vector<HWND> setSkillCheckboxes;

// image bbs
std::vector<RECT> removeSkillButtonBBs;
std::vector<RECT> removeSetSkillButtonBBs;

// Armors
HWND headArmorDropdownList;
HWND headLowRankRadioButton;
HWND headHighRankRadioButton;
HWND chestArmorDropdownList;
HWND chestLowRankRadioButton;
HWND chestHighRankRadioButton;
HWND armArmorDropdownList;
HWND armLowRankRadioButton;
HWND armHighRankRadioButton;
HWND waistArmorDropdownList;
HWND waistLowRankRadioButton;
HWND waistHighRankRadioButton;
HWND legArmorDropdownList;
HWND legLowRankRadioButton;
HWND legHighRankRadioButton;

// deco lists
HWND allDecoList;
HWND selectAllDecoButton;
HWND deselectAllDecoButton;
HWND resetAllDecoCountButton;

// Group boxes
HWND weaponGroupbox;
HWND charmGroupbox;
HWND skillGroupbox;
HWND addedSkillsGroupBox;
HWND setSkillGroupbox;
HWND addedSetSkillsGroupBox;
HWND armorGroupbox;
HWND decorationGroupbox;
HWND searchOptionGroupBox;

// positions
POINT weaponGBPos;
POINT charmGBPos;
POINT skillGBPos;
POINT addedSkillsGBPos;
POINT setSkillGBPos;
POINT addedSetSkillsGBPos;
POINT armorGBPos;
POINT searchResultGBPos;
POINT searchButtonPos;
POINT clearSearchResultButtonPos;
POINT clearAllButtonPos;
POINT decorationGBPos;
POINT allDecoPos;
POINT myDecoPos;
POINT excludedDecoPos;
//POINT decoDirIconPos;
POINT searchOptionsGBPos;

// richedit
HMODULE hMod;
HWND resultRichEdit;

// pop up menu
HMENU addedSkillPopUpMenu;
HMENU addedSetSkillPopUpMenu;
HMENU decorationPopUpMenu;

// Search button
HWND searchButton;

HWND searchProgressBar;

// Clear search result only
//HWND clearSearchResultButton;

// Clear all button. Clears all setting
//HWND clearAllButton;

// cursor pos
POINT cursorPos;

// size data
int minClientAreaWidth;
int minClientAreaHeight;
int skillGBHeight;
int addedSkillGBHeight;

// Currently searched armor sets
std::vector<MHW::ArmorSet> searchedArmorSets;
// Search result string
std::wstring searchResultHeaderStr;
std::wstring searchResultStr;

// Search option
HWND searchOptionAllowExtraSkill;
HWND searchOptionAllowOverleveledSkill;
HWND searchOptionSearchFromHigherRarity;
//HWND searchOptionUseMaxLevelCharmOnly;
HWND searchOptionMinArmorRarityCombobox;
HWND searchOptionSimplifyResult;
HWND searchOptionShowMaxLevel;

// Draw 
void drawMainWindow(HWND& hWnd, HDC& hdc);
void drawRect(HDC& hdc, const int x, const int y, const int w, const int h);
void drawSearchResult(HDC& hdc);

// init
int initSearchResultWindow();
int initAbout();
void initGroupboxes();
void initWeaponCombobox();
void initCharmControls();
void initSkillControls();
void initSetSkillControls();
void initArmorControls();
void initDecorationControls();
void initPopUpMenu();
void initButtons();
void initSearchOptions();

// refresh
void refreshCharmDropdownList();
void refreshSkillCombobox();
void refreshAddedSkillList();
void refreshSetSkillCombobox();
void refreshAddedSetSkillList();
void refreshArmorDropdownList(HWND& armorComboboxHND, std::map<int, Armor>& armors, const bool HR, int& dropdownIndex);
void refreshAllArmorDropdownList();
void refreshAllDecoList();
void refreshMenu(const bool init);
void refreshMenuLanguage(HMENU menu, const int id, const MHW::StringLiteral strEnum);
void refreshMenuCheck(HMENU menu, const int id, const bool flag);
void refreshCharmDisplaySetting();
void refreshArmorRankRadioButton(HWND hwnLR, HWND hwndHR, const bool flag);
void refreshWeaponDropdownlistEnable();
void refreshSearchOptionCheckbox();

// resize
void resizeSearchResultRichEdit();
void updateWindowSize(HWND hwnd, LPARAM lParam);

// draw
void redrawRect(HWND& targetHWND);
void redrawRect(RECT targetRect, const bool convert);

// Utility
std::vector<std::string> splitByComma(const std::string str);
bool isPointInRect(const RECT& rect, const POINT& point);
std::wstring buildSearchResultText(std::vector<MHW::ArmorSet>& newArmorSets);
POINT getCursorPos(const bool screenToClient);
RECT getControlRect(HWND& controlHWND);

// update mouse
void updateMouseMove(const bool force);
void updateMouseRightClick();
void updateLeftMouseButtonClick();

// check worker thread state
void checkWorkerThread();

// create search filter
int createSearchFilter(MHW::Filter& filter);
int checkFilter(MHW::Filter& filter);

// create popup menu
void createDecoPopUpMenu(const int row);

// Update weapon controls
void updateWeaponControls(WPARAM wParam, LPARAM lParam);
void updateWeaponSlot1(WPARAM wParam);
void updateWeaponSlot2(WPARAM wParam);
void updateWeaponSlot3(WPARAM wParam);

// Update charm dropdown list
void updateCharmDropdownList(WPARAM wParam);
void updateCharmDisplayByName();
void updateCharmDisplayBySkillName();

// Update skill dropdown list
void updateSkillDropdownList(WPARAM wParam);
void updateAddSkillButon();
void updateClearSkillsButton();
void updateSkillCheckbox(const int wmId);

// Update set skill dropdown list
void updateSetSkillDropdownList(WPARAM wParam);
void updateSetSkillLowRankRadioButton();
void updateSetSkillHighRankRadioButton();
void updateAddSetSkillButton();
void updateClearSetSkillsButton();
void updateSetSkillCheckbox(const int wmId);

// Update armor
void updateHeadArmorDropdownList(WPARAM wParam);
void updateHeadArmorLowRankRadioButton();
void updateHeadArmorHighRankRadioButton();
void updateChestArmorDropdownList(WPARAM wParam);
void updateChestArmorLowRankRadioButton();
void updateChestArmorHighRankRadioButton();
void updateArmArmorDropdownList(WPARAM wParam);
void updateArmArmorLowRankRadioButton();
void updateArmArmorHighRankRadioButton();
void updateWaistArmorDropdownList(WPARAM wParam);
void updateWaistArmorLowRankRadioButton();
void updateWaistArmorHighRankRadioButton();
void updateLegArmorDropdownList(WPARAM wParam);
void updateLegArmorLowRankRadioButton();
void updateLegArmorHighRankRadioButton();

// Update search button 
int updateSearchButton();

// Update list view notification
void updateListViewNotification(LPARAM lParam);

// Selects all decorations
void selectAllDecorations();

// Deselects all decorations
void deselectAllDecorations();

// rest all deco count
void resetAllDecoCount();

// Remove right clicked skill
void removeAddedSkillFromPopupMenu();
void removeAddedSetSkillFromPopupMenu();

// Switch level of added skill
void switchAddedSkillLevel(const int wmId);
void switchAddedSetSkill();

// gender
void switchGender(const MHW::Gender gender);

// update search option
void updateMinRarity(WPARAM wParam);

// toggle
void toggleAllowLowRankArmorOption();
void toggleAllowArenaArmorOption();
void toggleAllowEventArmorOption();
void toggleGenderMenu();

// language
void switchLanguage(const MHW::Language language, const bool force);
// reload ui
void reloadUI();

// Show error message box
void showErrorMsgBox(const std::wstring& message, const bool exit);
void showErrorCodeMsgBox(const MHW::ERROR_CODE errcode, const bool exit);

// file
void save();
void open();

// clear and release
void clear();

#endif