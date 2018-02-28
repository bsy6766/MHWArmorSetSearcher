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

// foward declaration
class Settings;
class Database;

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;																		// current instance
WCHAR szTitle[MAX_LOADSTRING] = L"Monster Hunter World Armor Set Searcher";				// The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];														// the main window class name
WCHAR szSearchResultWindowClass[MAX_LOADSTRING];

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
ATOM				MyRegisterSearchResultClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// default font for all windows
HFONT defaultFont;	

// main window
HWND mainHWND;
HWND searchResultHWND = nullptr;

// instances
static Settings* setting = nullptr;
static Database* db = nullptr;
static MHW::SetSearcher* setSearcher = nullptr;

// flag
bool initialized;;

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
//HWND myDecoList;
//HWND excludedDecoList;
//HWND addDecoButton;
//HWND removeMyDecoButton;
//HWND removeExcludedDecoButton;

// Group boxes
HWND weaponGroupbox;
HWND charmGroupbox;
HWND skillGroupbox;
HWND addedSkillsGroupBox;
HWND setSkillGroupbox;
HWND addedSetSkillsGroupBox;
HWND armorGroupbox;
HWND decorationGroupbox;
//HWND searchResultGroupbox;
HWND searchOptionGroupBox;

// Image
HBITMAP removeSkillImage;
//HBITMAP decorationDirImage;

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

// Search button
HWND searchButton;

// Clear search result only
HWND clearSearchResultButton;

// Clear all button. Clears all setting
HWND clearAllButton;

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
HWND searchOptionMaleRadioButton;
HWND searchOptionFemaleRadioButton;

// Draw 
void drawMainWindow(HWND& hWnd, HDC& hdc);
void drawImage(HDC& hdc, HBITMAP& bitmapImage, const int x, const int y, const int w, const int h);
void drawRect(HDC& hdc, const int x, const int y, const int w, const int h);
void drawSearchResult(HDC& hdc);

// Initialize all group boxes
void initGroupboxes();

// Init image
//void initImageIcons();
// Init image bb
void initRemoveIconBBs();

// Initialize weapon comboboxes
void initWeaponCombobox();

// Initialize charm comboboxes
void initCharmControls();
// Refresh charm dropdown list by display setting
void refreshCharmDropdownList();

// Initialize skill button
void initSkillControls();
// Refresh skill lists based on currently added skills
void refreshSkillCombobox();
// Refresh added skill list based on order
void refreshAddedSkillList();

// Initialize set skill controls
void initSetSkillControls();
// Refresh set skill lists based on currently added skills
void refreshSetSkillCombobox();
// Refresh added set skill list based on order
void refreshAddedSetSkillList();

// Init armor dropdown list
void initArmorControls();
// refresh armor dropdown list
void refreshArmorDropdownList(HWND& armorComboboxHWND, std::map<int, Armor>& armors, const bool HR, const int showIndex);

// Init deco lists
void initDecorationControls();

void initSearchResultWindow();

// resize research result
void resizeSearchResultRichEdit();

// Init pop up menu
void initPopUpMenu();

// init search and clear all buttons
void initButtons();

// split string line by comma
std::vector<std::string> splitByComma(const std::string str);

// check if pos is in rect
bool isPointInRect(const RECT& rect, const POINT& point);

// update mouse move every frame
void updateMouseMove(const bool force);
void updateMouseRightClick();

// check worker thread state
void checkWorkerThread();

// build search result text
std::wstring buildSearchResultText(std::vector<MHW::ArmorSet>& newArmorSets);

// create search filter
int createSearchFilter(MHW::Filter& filter);
int checkFilter(MHW::Filter& filter);

POINT getCursorPos(const bool screenToClient);

RECT getControlRect(HWND& controlHWND);
void redrawRect(HWND& targetHWND);
void redrawRect(RECT targetRect, const bool convert);

// Update left mouse button click
void updateLeftMouseButtonClick();

// Prevent resizing window less than minimum size
void updateWindowSize(HWND hwnd, LPARAM lParam);

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
void updateSearchButton();

// Update list view notification
void updateListViewNotification(LPARAM lParam);

// Selects all decorations
void selectAllDecorations();

// Deselects all decorations
void deselectAllDecorations();

// Remove right clicked skill
void removeAddedSkillFromPopupMenu();
void removeAddedSetSkillFromPopupMenu();

// Switch level of added skill
void switchAddedSkillLevel(const int wmId);
void switchAddedSetSkill();

// gender
void switchGender();

// toggle
void toggleAllowLowRankArmorOption();

// Show error message box
void showErrorMsgBox(const std::string& message, const bool exit);
void showErrorCodeMsgBox(const MHW::ERROR_CODE errcode, const bool exit);

// clear and release
void clear();

#endif