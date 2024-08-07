#pragma once
#include "..\GameObjects\TitleWorldMapCamera.h"
#include "..\GameObjects\TitleWorldMapGlobe.h"
#include "..\GameObjects\TitleWorldMapSky.h"

#include "Title.h"
#include "TitleWorldMapPause.h"
namespace SUC::UI::TitleScreen
{
	class TitleWorldMap
	{
	public:
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
		struct SFlagUIInformation
		{
			Chao::CSD::RCPtr<Chao::CSD::CScene> flag;
			Chao::CSD::RCPtr<Chao::CSD::CScene> sun_moon;
			int id;
			bool night, playingMedalTransition;
			float visibility;
		};

		static void RegisterHooks();
		static void Start();
		static void CreateScreen(Sonic::CGameObject* pParentGameObject);
		static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
		static void KillScreen();
		static void SetHideEverything(const bool visible);
		static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
		static void PlayPanningAnim();
		static void EnableInput();
		static hh::math::CQuaternion QuaternionFromAngleAxis(float angle, const hh::math::CVector& axis);


		// Earth's position is in the center of the screen.
		// Later, we'll want to be able to move both the camera target & position in synch so the camera can smoothly move downward.
		static const Hedgehog::Math::CVector s_PivotPosition;
		static const TitleWorldMapCamera* s_WorldMapCamera;
		static bool s_IsActive;
		static bool s_ForceTitleFlow;
		static bool s_WorldmapCursorDisabled;
		static bool s_TargetDisabled;
		static bool s_IsCapitalWindowOpened;
		static bool s_IsWorldMapCameraInitialized;
		static int s_LastFlagSelected;
		static int s_StageSelectWindowSelection;
		static SFlagUIInformation s_Flags[9];
	};
}