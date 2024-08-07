#pragma once
struct SaveLoadTestStruct
{
	INSERT_PADDING(0x34);
	bool m_saveCompleted;
};

namespace SUC::UI::TitleScreen
{
	class Title
	{
	public:
		enum ETitleIndexState
		{
			New_Game,
			Continue,
			Options,
			Quit
		};
		static void RegisterHooks();
		static void CreateScreen(Sonic::CGameObject* pParentGameObject);
		static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
		static void KillScreen();
		static void ToggleUI(const bool visible, const bool logoVisible = false);
		static void SetSubMenu(bool enabled);
		static void SetScrollDirection(bool horizontal);
		static void IntroAnimation(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
		static void ShowWindow(const char* text);
		static void HideWindow();
		static void Update();
		static void ShowLoadingTransition(bool enableLoad);

		static bool ms_IsWorldMapActive;
	};
}