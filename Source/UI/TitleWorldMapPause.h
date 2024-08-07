#pragma once
namespace SUC::UI::TitleScreen
{
	class TitleWorldMapPause
	{
	public:
		static void RegisterHooks();
		static void Start();
		static void CreateScreen(Sonic::CGameObject* pParentGameObject);
		static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
		static void KillScreen();

		static bool s_IsPaused;
	};
}
