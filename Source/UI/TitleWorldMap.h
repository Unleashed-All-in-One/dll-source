#pragma once
#include "..\GameObjects\TitleWorldMapCamera.h"
#include "..\GameObjects\TitleWorldMapGlobe.h"
#include "..\GameObjects\TitleWorldMapSky.h"

class TitleWorldMap
{
public:
	struct SFlagUIInformation
	{
		Chao::CSD::RCPtr<Chao::CSD::CScene> flag;
		Chao::CSD::RCPtr<Chao::CSD::CScene> sun_moon;
		int id;
		bool night, playingMedalTransition;
		float visibility;
	};
	struct SaveStageInfo
	{
		std::string stageID_string;
		uint32_t stageIDForGens;
		bool isStageCompleted;
		uint32_t bestScore;
		float bestTime;
		float bestTime2;
		float bestTime3;
		uint32_t bestRank;
		uint32_t bestRing;
		uint32_t redRingCount;
	};

	// Earth's position is in the center of the screen.
	// Later, we'll want to be able to move both the camera target & position in synch so the camera can smoothly move downward.
	static const Hedgehog::Math::CVector m_emblemPosition;
	static const TitleWorldMapCamera* m_worldMapCamera;
	static void applyPatches();
	static void Start();
	static void CreateScreen(Sonic::CGameObject* pParentGameObject);
	static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
	static void KillScreen();
	static void SetHideEverything(const bool visible);
	static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
	static hh::math::CQuaternion QuaternionFromAngleAxis(float angle, const hh::math::CVector& axis);
	static void PlayPanningAnim();
	static void EnableInput();

	static bool m_isActive;
	static bool m_ForceLoadToTitle;
	static bool m_isStickMovementDisabled;
	static bool m_isTargetDisabled;
	static bool m_isCapitalWindowOpened;
	static bool m_isWorldMapCameraInit;
	static int m_lastFlagSelected;
	static int m_stageSelectWindowSelection;
	static SFlagUIInformation m_flags[9];
};
