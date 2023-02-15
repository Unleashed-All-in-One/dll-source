#pragma once

class TitleWorldMapPause
{

public:

	
	static void applyPatches();
	static void Start();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
};
