#pragma once
#include "..\GameObjects\TitleWorldMapCamera.h"
#include "..\GameObjects\TitleWorldMapGlobe.h"
#include "..\GameObjects\TitleWorldMapSky.h"
#include "..\GameObjects\TitleWorldMapSun.h"

#include "Title.h"
#include "TitleWorldMapPause.h"
#include "../System/AspectRatioHelper.h"

namespace SUC::Utility
{
	inline hh::math::CVector WorldToUIPosition(const hh::math::CVector& input, const Sonic::CCamera* in_Camera, const hh::math::CVector2& in_AspectRatioOffset)
	{
		if (!in_Camera) return Hedgehog::Math::CVector(0, 0,0);
		auto screenPosition = in_Camera->m_MyCamera.m_View * hh::math::CVector4(input.x(), input.y(), input.z(), 1.0f);
		screenPosition = in_Camera->m_MyCamera.m_Projection * screenPosition;
		screenPosition.head<2>() /= screenPosition.w();
		screenPosition.x() = ((screenPosition.x() * 0.5f + 0.5f) * (SUC::System::AspectRatioHelper::OriginalResolution->x() + in_AspectRatioOffset
			.x()));
		screenPosition.y() = (screenPosition.y() * -0.5f + 0.5f) * (SUC::System::AspectRatioHelper::OriginalResolution->y() + in_AspectRatioOffset
			.y());
		return hh::math::CVector(screenPosition.x(), screenPosition.y(), 0);
	}
}
namespace SUC::UI::TitleScreen
{
	//From GensDebug
	//============================
	// NonCopyable Interface Impl
	//============================
	class INonCopyable
	{
	protected:
		INonCopyable() { }

		~INonCopyable() = default;

	private:
		INonCopyable(const INonCopyable& other);
		const INonCopyable& operator= (const INonCopyable& other);
	};


	//=====================
	// CSingleton Impl
	//=====================
	template <typename T>
	class Singleton : INonCopyable
	{
	public:
		static T& ms_Instance()
		{
			static T _ms_Instance;
			return _ms_Instance;
		}

	protected:
		explicit Singleton<T>() { }
	};
	
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
		//static void CreateScreen(Sonic::CGameObject* pParentGameObject);
		//static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
		//static void KillScreen();
		//static void SetHideEverything(const bool visible);
		//static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
		static void PlayPanningAnim();
		static void EnableInput();
		static hh::math::CQuaternion QuaternionFromAngleAxis(float angle, const hh::math::CVector& axis);


		// Earth's position is in the center of the screen.
		// Later, we'll want to be able to move both the camera target & position in synch so the camera can smoothly move downward.
		static const Hedgehog::Math::CVector s_PivotPosition;
		static const Sonic::CCamera* s_WorldMapCamera;
		static bool s_IsActive;
		static bool s_ForceTitleFlow;
		static bool s_WorldmapCursorDisabled;
		static bool s_TargetDisabled;
		static bool s_IsCapitalWindowOpened;
		static bool s_IsWorldMapCameraInitialized;
		static int s_LastFlagSelected;
		static int s_StageSelectWindowSelection;
	};
	class CWorldCountry
	{
		SharedPtrTypeless m_SoundHandle;
		float m_TimerContinueDaylightIndicator;
	public:
		int m_ID;
		hh::math::CVector m_Position;
		hh::math::CVector2 m_ScreenPosition;
		bool m_IsInShade, m_LastState, m_IsChangingDaylight;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcFlag;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcDaylightIndicator;
		//bool night, playingMedalTransition;
		float visibility;
		bool m_CanFade;
		bool m_IsSelected = true;
		CWorldCountry(const hh::math::CVector& in_Position) : m_Position(in_Position) {};
		void Update(const hh::math::CVector& in_LightPos, const hh::math::CQuaternion& in_WorldRotation, const Sonic::CCamera* in_Cam)
		{
			visibility = fmax(
				0.0f, -(in_Cam->m_MyCamera.m_Direction.dot(
					(m_Position - SUC::UI::TitleScreen::TitleWorldMap::s_PivotPosition).normalized()))) * 100;
			//CHECK FOR REMOVAL (missing offset aspect)
			auto pos = Utility::WorldToUIPosition(in_WorldRotation * m_Position, in_Cam, Hedgehog::Math::CVector2(0, 0));
			m_ScreenPosition = hh::math::CVector2(pos.x(), pos.y());
			float shadeAmount = fmax(
				0.0f, -(in_LightPos.
					dot((m_Position).normalized()))) *
				100;
			bool isDark = shadeAmount > 50;
			if (m_IsChangingDaylight)
			{
				if (m_rcDaylightIndicator->m_MotionFrame == 0 
					|| m_rcDaylightIndicator->m_MotionDisableFlag)
					m_IsChangingDaylight = false;
			}
				//			&& 
				//			)
				//			m_Flag->m_IsChangingDaylight = false;


			m_IsInShade = !isDark;
			
			if (!m_IsChangingDaylight)
			{
				if (m_IsInShade == m_LastState)
				{

					CSDCommon::PlayAnimation(m_rcDaylightIndicator, "Fade_Anim",
						Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);
				}

				if (m_IsInShade != m_LastState)
				{
					m_IsChangingDaylight = true;
					CSDCommon::PlayAnimation(m_rcDaylightIndicator, "Switch_Anim",
						Chao::CSD::eMotionRepeatType_PlayOnce, 0, 0, 0, !m_IsInShade, !m_IsInShade);
				}
			}
			CSDCommon::PlayAnimation(m_rcFlag, "Fade_Anim",
				Chao::CSD::eMotionRepeatType_PlayOnce, 0, visibility, visibility);

			m_rcFlag->SetPosition(m_ScreenPosition.x(), m_ScreenPosition.y());
			m_rcDaylightIndicator->SetPosition(m_ScreenPosition.x() + 36, m_ScreenPosition.y() - 23);

			DebugDrawText::log(SUC::Format("[FLAG]\n LAST: %s | CURRENT: %s", m_LastState ? "true" : "false", m_IsInShade ? "true" : "false"), 0);
			m_LastState = m_IsInShade;

			
			if (m_IsSelected)
			{
				if (m_IsInShade && !m_LastState)
					Common::PlaySoundStaticCueName(m_SoundHandle, "sys_worldmap_sunset");
				else if (!m_IsInShade && m_LastState)
					Common::PlaySoundStaticCueName(m_SoundHandle, "sys_worldmap_sunrise");
			}
		}
	};
#define WM_STAGELIST_MAXLISTVISIBLE (size_t)6
	class CSUCTitleCompanion : public Sonic::CGameObject, public Hedgehog::Universe::CStateMachineBase,
		public Singleton<CSUCTitleCompanion>
	{
	public:
		

		static inline const auto lightPosition = Hedgehog::Math::CVector(-79.8565f, 0, 4.78983f);
		
#pragma region UI
		class CWorldMapCanvas
		{
		public:
			Chao::CSD::RCPtr<Chao::CSD::CProject> rcWorldMap;
			boost::shared_ptr<Sonic::CGameObjectCSD> spWorldMap;
			boost::shared_ptr<Sonic::CCsdProject> spCsdProject;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcInfoBg1, rcWorldmapFooterImgA, rcInfoImg1, rcInfoImg2, rcInfoImg3, rcInfoImg4,
				rcHeaderBgW, rcHeaderImgW, rcFooterBgW, rcFooterImgW;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcCursorLeft, rcCursorTop, rcCursorBottom, rcCursorRight;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsName, rcCtsCursorEffect, rcCtsGuideWindow, rcCtsGuideScreenshot, rcCtsGuideText
				, rcCtsGuide4Mission, rcCtsGuide5Medal;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsStageWindow, rcCtsStageSelect, rcCtsName_2, rcStageSelectFlag,
				rcCtsStageInfoWindow, rcCtsGuide1Hiscore, rcCtsGuide2BestTime, rcCtsGuide3Rank,
				rcCtsStage4Mission, rcCtsStage5Medal, rcCtsStageScreenshot;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcCtsChoicesBg, rcCtsChoicesWindow, rcCtsChoicesSelect;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcTextWorldDescription, rcTextStageDescription;
			Chao::CSD::RCPtr<Chao::CSD::CScene> rcTextElement[9];
			void ShowTextAct(bool visible);

			void Configure(const boost::shared_ptr<Sonic::CCsdProject>& in_CsdProj);

			void SetInactive(bool in_Hide);
		};
		CWorldMapCanvas* UI;
#pragma endregion

		Hedgehog::Math::CVector2 posCursorCenter;
		SharedPtrTypeless cursorMoveHandle, cursorSelectHandle, stageSelectHandle, worldMapMusicHandle;
		bool playingPointerMove;
		bool introPlayed = false;
		int currentFlagSelected;
		int lastFlagIndex = 0;
		int stageSelectedWindowMax = 6;
		int selectedCapital = 0;

		int flagSelectionAmount = 0;
		float lastflagSelectionAmount = 0;
		float timeStageSelectDelay = 0;


		bool isStageWindowOpen = false;

		bool cursorSelected = false;
		float cursorMultiplier = 24;
		int m_StageListOverflow;
		int m_StageListSelection;
		std::vector<CWorldCountry*> m_Country;
		boost::shared_ptr<SaveLoadTestStruct> m_spSave;
		boost::shared_ptr<Sonic::CLightManager> light;

		boost::shared_ptr<CTitleWorldMapGlobe> m_GlobeModel;
		boost::shared_ptr<CTitleWorldMapSky> m_spSkySpace;
		boost::shared_ptr<CTitleWorldMapSun> m_spSun;
		void DestroyCSDScene(Chao::CSD::RCPtr<Chao::CSD::CScene>& in_CsdScene);

		void KillCallback() override;

		void ShowSaveIcon();

		void AddCallback
		(
			const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
			Sonic::CGameDocument* pGameDocument,
			const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase
		) override;

		void UpdateSerial(const Hedgehog::Universe::SUpdateInfo& in_rUpdateInfo) override;

		void SetUIVisibility(bool in_Hide);

		void PlayCursorAnim(const char* name, bool reverse = false);

		void CheckCursorAnimStatus();

		void SetCursorPos(const Hedgehog::Math::CVector2& pos);

		bool IsInsideCursorRange(const Hedgehog::Math::CVector2& input, float visibility, int flagID);

		void SetLevelTextCast(const char* text);

		void PopulateStageSelect(int id, int offset = 0);

		TitleWorldMap::SaveStageInfo GetInfoForStage(std::string id);

		void PopulateCountryPreviewInfo(int flag);

		void PopulateStageWindowInfo(std::string id);

		void SetVisibilityPlayerInfo(bool visible);

		void SetVisibilityCapitalWindow(bool visible);

		void SetVisibilityStageWindow(bool visible);

		void CreateScreen();

		void KillScreen();

		void ToggleScreen(const bool visible);

		void CapitalWindow_Update();

		void SetStageSelectionScreenshot();
		///Stage selection highlight & stage launch
		void StageWindow_Update(Sonic::CGameObject* This);
	};
}