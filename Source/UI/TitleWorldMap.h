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
#define WM_STAGELIST_MAXLISTVISIBLE (size_t)6
	class CSUCTitleCompanion;
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
	class CWorldCountry
	{
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> m_SoundHandle;
		float m_TimerContinueDaylightIndicator;
	public:
		int m_ID;
		hh::math::CVector m_Position;
		hh::math::CQuaternion m_Rotation;
		hh::math::CVector2 m_ScreenPosition;
		bool m_IsInShade, m_LastState, m_IsChangingDaylight;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcFlag;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcDaylightIndicator;
		//bool night, playingMedalTransition;
		float visibility;
		bool m_CanFade;
		bool m_IsSelected = false;
		CWorldCountry(const hh::math::CVector& in_Position) : m_TimerContinueDaylightIndicator(0), m_ID(0),
		                                                      m_Position(in_Position), m_IsInShade(false),
		                                                      m_LastState(false),
		                                                      m_IsChangingDaylight(false),
		                                                      visibility(0), m_CanFade(false)
		{
		};
		void Update(const hh::math::CVector& in_LightPos, const hh::math::CQuaternion& in_WorldRotation, const Sonic::CCamera* in_Cam);
		hh::math::CVector GetPositionAdjusted() const;
		
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
			void ShowTextAct(bool in_Visible) const;

			void Configure(const boost::shared_ptr<Sonic::CCsdProject>& in_CsdProj);

			void SetInactive(bool in_Hide) const;
		};
		CWorldMapCanvas* WorldMapUI;
#pragma endregion

		Hedgehog::Math::CVector2 posCursorCenter;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> cursorMoveHandle, cursorSelectHandle, stageSelectHandle, worldMapMusicHandle;
		bool playingPointerMove;
		bool m_PanPlayed = false;
		int currentFlagSelected;
		int flagSelected;
		int lastFlagIndex = 0;
		int stageSelectedWindowMax = 6;
		int selectedCapital = 0;

		int flagSelectionAmount = 0;
		float lastflagSelectionAmount = 0;
		float timeStageSelectDelay = 0;


		bool m_StageWindowActive = false;

		bool m_IsCursorOverFlag = false;
		float m_CursorSpeed = 24;
		int m_StageListOverflow;
		int m_StageListSelection;
		std::vector<CWorldCountry*> Countries;
		boost::shared_ptr<SaveLoadTestStruct> m_spSave;
		boost::shared_ptr<Sonic::CLightManager> light;

		boost::shared_ptr<CTitleWorldMapGlobe> m_spEarth;
		boost::shared_ptr<CTitleWorldMapSky> m_spSkySpace;
		boost::shared_ptr<CTitleWorldMapSun> m_spSun;
		void DestroyCSDScene(Chao::CSD::RCPtr<Chao::CSD::CScene>& in_CsdScene) const;

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

		void CheckCursorAnimStatus() const;

		void SetCursorPos(const Hedgehog::Math::CVector2& pos);

		bool IsInsideCursorRange(const Hedgehog::Math::CVector2& input, float visibility, int flagID);

		void SetLevelTextCast(const char* text) const;

		void PopulateStageSelect(int id, int offset = 0);

		SaveStageInfo GetInfoForStage(std::string id);

		void PopulateCountryPreviewInfo(int flag);

		void PopulateStageWindowInfo(std::string id);

		void SetVisibilityPlayerInfo(bool visible);

		void SetVisibilityCapitalWindow(bool visible);

		void SetVisibilityStageWindow(bool visible);

		void CreateScreen();

		void KillScreen();

		void ToggleScreen(const bool visible);

		void CapitalWindow_Update();

		void SetStageSelectionScreenshot() const;

		void StageWindow_Update(Sonic::CGameObject* This);
	};


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

		static inline boost::shared_ptr<CSUCTitleCompanion> m_spTitle;
		static void RegisterHooks();
		static void Start();
		//static void CreateScreen(Sonic::CGameObject* pParentGameObject);
		//static void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject);
		//static void KillScreen();
		//static void SetHideEverything(const bool visible);
		//static void IntroAnim(Chao::CSD::RCPtr<Chao::CSD::CScene> scene);
		static void PlayPanningAnim();
		static void EnableInput();

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
	};
}