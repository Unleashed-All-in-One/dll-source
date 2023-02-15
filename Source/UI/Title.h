#pragma once
struct SaveLoadTestStruct
{
	INSERT_PADDING(0x34);
	bool m_saveCompleted;
};
class Title
{
	
public:	
	static void applyPatches();
	static void createScreen(Sonic::CGameObject* pParentGameObject);
	static void toggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void killScreen();
	static void setHideEverything(const bool visible, const bool logoVisible = false);
	static void setSubmenu(bool enabled);
	static void setScrollDirection(bool horizontal);
	static void introAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
	static void ShowWindow(const char* text);
	static void HideWindow();
	static bool canLoad;
	static bool inWorldMap;
	static bool inInstall;

	static enum TitleIndexState {
		New_Game,
		Continue,
		Options,
		Quit
	};
	
};