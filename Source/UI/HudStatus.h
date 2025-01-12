





#pragma once
class HudStatus
{
public:

	class StatusNew
	{
	public:
		struct StatManager
		{
			struct Stat
			{
				enum EStatType
				{
					SPEED,
					RING_ENAGY,
					COMBAT,
					STRENGTH,
					LIFE,
					UNLEASH,
					SHIELD

				};
				EStatType m_Type;
				int m_StatLevel = 0;
				int m_MaximumLevel = 0;
				int txt_num = 0;
				int m_EXPLevelsNeeded = 0;
				float m_StatProgress = 0.0f;

				Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcBackgroundSlide;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcStatName;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcProgressBarBackground;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcProgressBar;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcLevelRequired;

				void Toggle(bool in_IsVisible)
				{
					m_rcBackgroundSlide->SetHideFlag(!in_IsVisible);
					m_rcStatName->SetHideFlag(!in_IsVisible);
					m_rcProgressBarBackground->SetHideFlag(!in_IsVisible);
					m_rcLevelRequired->SetHideFlag(!in_IsVisible);
					m_rcProgressBar->SetHideFlag(!in_IsVisible);
				}
				void GenerateScenes(const Chao::CSD::RCPtr<Chao::CSD::CProject>& in_Project, int index)
				{
					char m_Scene[100];
					sprintf(m_Scene, "tag_txt_%d", index);
					m_rcBackgroundSlide = in_Project->CreateScene("tag_bg_2");
					m_rcStatName = in_Project->CreateScene(m_Scene);
					m_rcProgressBarBackground = in_Project->CreateScene("prgs_bg_2");
					m_rcLevelRequired = in_Project->CreateScene("prgs_num_2");
					m_rcProgressBar = in_Project->CreateScene("prgs_bar_2");

					Toggle(false);
				}
			};
			std::vector<Stat*> stats;
			float expProgress = 0;
			int expLevel = 0;
		};
		enum EStatusState
		{
			Start,
			StartAdd,
			Idle,
			Exit,
		};

		EStatusState m_CurrentState;

		StatManager* statsDay;
		StatManager* statsNight;

		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> m_WooshSound;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> m_SelectHandle;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> m_SwitchHandle;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> m_EndHandle;

		boost::shared_ptr<Sonic::CGameObjectCSD> m_spStatus;
		Chao::CSD::RCPtr<Chao::CSD::CProject> m_rcStatus;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcStatusTitle;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcCharacterBanner;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcMedalInfo;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcSunMedalGauge;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcMoonMedalGauge;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcTagBg1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcTagTxt1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcPrgsBg1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcPrgsBar1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcPrgsNum1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcQuit;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_rcFooter;

		float m_TimerFooter = 0.0f;
		float m_TimerStatCreationDelay = 0.17f;
		float m_TimerCharSwitchCooldown = 0.0f;
		float m_TimerQuitSoundDelay = 0.0f;
		float m_TimerDelayFirstSelect = -1;
		float m_TimerDelayForStart = 0.0f;
		float m_TimerEndDelay;

		bool m_IsFooterActive = false;
		bool m_IsGoingToNextStage = false;
		bool m_IsWerehog = false;
		bool m_IsSwitchingChara;

		float m_YOffset = 0.0f;
		float m_SwitchCooldownMax = 0.5f;

		int medalSubImage = 0;
		int m_CreatedStatIndex = 0;
		int m_PreviousStatSelection = 0;
		int m_CurrentStatSelection = 0;
		bool m_PlayedFadeout;

		const char* m_AnimIntro;

		const char* GetStringWithPrefix(const char* format)
		{
			char buffer[100];
			if (m_IsWerehog)
				sprintf(buffer, format, "ev");
			else
				sprintf(buffer, format, "so");
			return buffer;
		}
		void Intro(bool in_isReset = true)
		{
			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			m_AnimIntro = GetStringWithPrefix("Intro_%s_Anim");
			m_rcPrgsNum1->GetNode("num")->SetText(std::to_string(statsManager->expLevel).c_str());
			CSDCommon::PlayAnimation(m_rcTagBg1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcTagTxt1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcPrgsBg1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10);
			CSDCommon::PlayAnimation(m_rcPrgsBar1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcPrgsNum1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10);
			CSDCommon::PlayAnimation(m_rcQuit, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_rcFooter, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_rcStatusTitle, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 2, 0.0);
			CSDCommon::PlayAnimation(m_rcCharacterBanner, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_rcMedalInfo, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_rcSunMedalGauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_rcMoonMedalGauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);

			if (in_isReset)
			{
				medalSubImage = 0.0f;
				m_TimerFooter = 0.0f;
				m_IsFooterActive = false;
				m_TimerDelayForStart = 0.75f;
				m_CreatedStatIndex = 0;
				m_CurrentStatSelection = 0;
				m_YOffset = 0.0f;
				m_TimerQuitSoundDelay = 0.0f;
				m_IsGoingToNextStage = false;
			}
		}
		static StatusNew* Generate(Sonic::CGameObject* Parent, bool isChainPr, int btnty)
		{
			StatusNew* dataOut = new StatusNew();
			Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());

			auto spCsdProject = wrapper.GetCsdProject("ui_status");
			dataOut->m_rcStatus = spCsdProject->m_rcProject;
			dataOut->Hide();
			Parent->m_pMember->m_pGameDocument->AddGameObject(dataOut->m_spStatus = boost::make_shared<Sonic::CGameObjectCSD>(dataOut->m_rcStatus, 0.5f, "HUD", false), "main", Parent);
			// Exp Bar
			dataOut->m_rcTagBg1 = dataOut->m_rcStatus->CreateScene("tag_bg_1");
			dataOut->m_rcTagTxt1 = dataOut->m_rcStatus->CreateScene("tag_txt_1");
			dataOut->m_rcPrgsBg1 = dataOut->m_rcStatus->CreateScene("prgs_bg_1");
			dataOut->m_rcPrgsBar1 = dataOut->m_rcStatus->CreateScene("prgs_bar_1");
			dataOut->m_rcPrgsNum1 = dataOut->m_rcStatus->CreateScene("prgs_num_1");
			// Quit Button               
			dataOut->m_rcQuit = dataOut->m_rcStatus->CreateScene("decide_bg");
			// Bottom Buttons            
			dataOut->m_rcFooter = dataOut->m_rcStatus->CreateScene("status_footer");
			// Header                    
			dataOut->m_rcStatusTitle = dataOut->m_rcStatus->CreateScene("status_title");
			// Character Portrait       
			dataOut->m_rcCharacterBanner = dataOut->m_rcStatus->CreateScene("logo");
			//Medals (Top Right)         
			dataOut->m_rcMedalInfo = dataOut->m_rcStatus->CreateScene("medal_info");
			dataOut->m_rcSunMedalGauge = dataOut->m_rcStatus->CreateScene("medal_s_gauge");
			dataOut->m_rcMoonMedalGauge = dataOut->m_rcStatus->CreateScene("medal_m_gauge");
			
			StatManager::Stat* speed = new StatManager::Stat{ StatManager::Stat::SPEED, 9, 11, 1, 10 };
			StatManager::Stat* ring_energy = new StatManager::Stat{ StatManager::Stat::RING_ENAGY, 6, 6, 2, 0 };
			StatManager::Stat* combat = new StatManager::Stat{ StatManager::Stat::COMBAT, 0, 31, 1, 0 };
			StatManager::Stat* strength = new StatManager::Stat{ StatManager::Stat::STRENGTH, 0, 11, 2, 0 };
			StatManager::Stat* life = new StatManager::Stat{ StatManager::Stat::LIFE, 0, 11, 3, 0 };
			StatManager::Stat* unleash = new StatManager::Stat{ StatManager::Stat::UNLEASH, 0, 11, 4, 0 };
			StatManager::Stat* shield = new StatManager::Stat{ StatManager::Stat::SHIELD, 0, 11, 5, 0 };
			dataOut->statsDay = new StatManager();
			dataOut->statsNight = new StatManager();
			dataOut->statsDay->stats = { speed, ring_energy };
			dataOut->statsNight->stats = { combat, strength, life, unleash, shield };
			for (int i = 0; i < dataOut->statsDay->stats.size(); ++i)
			{
				dataOut->statsDay->stats[i]->GenerateScenes(dataOut->m_rcStatus, i + 2);
			}
			for (int i = 0; i < dataOut->statsNight->stats.size(); ++i)
			{
				dataOut->statsNight->stats[i]->GenerateScenes(dataOut->m_rcStatus, i + 2);
			}
			dataOut->statsNight->expLevel = 1;
			dataOut->m_rcFooter->SetHideFlag(true);
			return dataOut;
		}
		void Hide()
		{

		}
		void Dissapear()
		{
		}
		void ShowAndAnimate(Chao::CSD::RCPtr<Chao::CSD::CScene> scene, const char* animation, float positionY)
		{
			scene->SetHideFlag(false);
			scene->SetPosition(0.0f, positionY);
			CSDCommon::PlayAnimation(scene, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0f, 0.0f);
		}
		void AddStatGauge(bool woosh = true)
		{
			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			auto m_AnimIntro2 = GetStringWithPrefix("Intro_%s_Anim_2");

			const auto& currentStat = statsManager->stats[m_CreatedStatIndex];
			// Create and animate scenes
			ShowAndAnimate(currentStat->m_rcProgressBar, m_AnimIntro2, m_YOffset);
			ShowAndAnimate(currentStat->m_rcProgressBarBackground, m_AnimIntro2, m_YOffset);
			ShowAndAnimate(currentStat->m_rcLevelRequired, m_AnimIntro2, m_YOffset);
			ShowAndAnimate(currentStat->m_rcBackgroundSlide, m_AnimIntro2, m_YOffset);
			ShowAndAnimate(currentStat->m_rcStatName, m_AnimIntro2, 0);
			// Set text for tag
			currentStat->m_rcStatName->SetHideFlag(false);
			CSDCommon::PlayAnimation(currentStat->m_rcStatName, m_AnimIntro2, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0f, 0.0f);

			// Handle progress number visibility
			if (currentStat->m_StatLevel >= currentStat->m_MaximumLevel)
			{
				currentStat->m_rcLevelRequired->GetNode("num")->SetHideFlag(true);
				currentStat->m_rcLevelRequired->GetNode("img")->SetHideFlag(true);
				currentStat->m_rcLevelRequired->GetNode("txt")->SetHideFlag(false);
			}
			else
			{
				currentStat->m_rcLevelRequired->GetNode("num")->SetText(std::to_string(currentStat->m_StatLevel).c_str());
			}

			if (currentStat->m_EXPLevelsNeeded == 0)
			{
				currentStat->m_rcLevelRequired->GetNode("num_2")->SetHideFlag(true);
				currentStat->m_rcLevelRequired->GetNode("img_2")->SetHideFlag(true);
			}
			else
			{
				currentStat->m_rcLevelRequired->GetNode("num_2")->SetText(std::to_string(currentStat->m_EXPLevelsNeeded).c_str());
			}

			// Update position for next stat
			m_YOffset += (m_IsWerehog ? 64.0f : 73.0f);
			m_CreatedStatIndex++;

			if (woosh)

				Common::PlaySoundStaticCueName(m_WooshSound, "sys_actstg_stateslrslide");

		}
		void PlayAnimationForStat(StatManager::Stat* stat, const char* animation, bool isReverse = true)
		{
			CSDCommon::PlayAnimation(stat->m_rcBackgroundSlide, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_rcProgressBarBackground, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_rcLevelRequired, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_rcProgressBar, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
		}
		void Select(int index, bool up, bool reverse, int previousIndex)
		{
			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			auto m_SelectAnim = GetStringWithPrefix("select_%s_Anim");
			int m_MaxSize = statsManager->stats.size() - 1;

			if (previousIndex != -1)
			{
				if (previousIndex == statsManager->stats.size())
					CSDCommon::PlayAnimation(m_rcQuit, m_SelectAnim, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
				else
					PlayAnimationForStat(statsManager->stats[previousIndex], m_SelectAnim, true);
			}
			if (index == statsManager->stats.size())
			{
				PlayAnimationForStat(statsManager->stats[index - 1], m_SelectAnim, true);
				CSDCommon::PlayAnimation(m_rcQuit, m_SelectAnim, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
				return;
			}

			//for (auto stat : statsManager->stats)
			//{
			//	PlayAnimationForStat(stat, m_SelectAnim, true);
			//}
			//PlayAnimationForStat(statsManager->stats[index], m_SelectAnim, false);

			PlayAnimationForStat(statsManager->stats[index], m_SelectAnim, reverse);


		}
		void FooterTimer(const Hedgehog::Universe::SUpdateInfo& updateInfo)
		{
			if (m_IsFooterActive)
				return;
		}
		void HideStats()
		{
			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			auto m_AnimIntro2 = GetStringWithPrefix("Intro_%s_Anim_2");
			m_AnimIntro = GetStringWithPrefix("Intro_%s_Anim");
			CSDCommon::PlayAnimation(m_rcTagBg1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, 0, false, true);
			CSDCommon::PlayAnimation(m_rcTagTxt1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, 0, false, true);
			CSDCommon::PlayAnimation(m_rcPrgsBg1, m_AnimIntro2, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, 0, false, true);
			CSDCommon::PlayAnimation(m_rcPrgsBar1, m_AnimIntro2, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, false, true);
			CSDCommon::PlayAnimation(m_rcPrgsNum1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 100, false, true);
			CSDCommon::PlayAnimation(m_rcQuit, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 0.0, 0, 0, false, true);
			CSDCommon::PlayAnimation(m_rcStatusTitle, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 0.0, 100.0f, 101.0f, false, true);
			for (auto stat : statsManager->stats)
			{
				PlayAnimationForStat(stat, m_AnimIntro, true);
				stat->m_rcStatName->SetHideFlag(true);
			}
			
		}
		void ToggleStats()
		{
			char prevAnim[100];
			sprintf(prevAnim, "Intro_%s_Anim", m_IsWerehog ? "ev" : "so");
			char prevAnimQuit[100];
			sprintf(prevAnimQuit, "select_%s_Anim", m_IsWerehog ? "ev" : "so");
			m_IsWerehog = !m_IsWerehog;

			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			m_AnimIntro = GetStringWithPrefix("Intro_%s_Anim");
			auto m_AnimIntro2 = GetStringWithPrefix("Intro_%s_Anim_2");

			m_rcPrgsNum1->GetNode("num")->SetText(SUC::Format("%d", statsManager->expLevel));
			//Exp Bar
			m_rcTagBg1->SetHideFlag(false);
			m_rcTagTxt1->SetHideFlag(false);
			m_rcPrgsBg1->SetHideFlag(false);
			m_rcPrgsBar1->SetHideFlag(false);
			m_rcPrgsNum1->SetHideFlag(false);
			m_rcQuit->SetHideFlag(false);
			m_rcStatusTitle->SetHideFlag(false);
			m_rcCharacterBanner->SetHideFlag(false);
			CSDCommon::PlayAnimation(m_rcTagBg1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcTagTxt1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcPrgsBg1, m_AnimIntro2, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10);
			CSDCommon::PlayAnimation(m_rcPrgsBar1, m_AnimIntro2, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10.0);
			CSDCommon::PlayAnimation(m_rcPrgsNum1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 10);
			CSDCommon::PlayAnimation(m_rcQuit, prevAnimQuit, Chao::CSD::eMotionRepeatType_PlayOnce, 0.0, 100.0f, 101.0f);
			CSDCommon::PlayAnimation(m_rcCharacterBanner, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, 20.0, false, !m_IsWerehog);

			m_CreatedStatIndex = 0;
			m_CurrentStatSelection = 0;
			m_YOffset = 0.0f;

			auto previousStats = !m_IsWerehog ? statsNight : statsDay;
			for (auto stats : previousStats->stats)
			{
				stats->Toggle(false);
			}

			m_CreatedStatIndex = 0;
			for (auto stats : statsManager->stats)
			{
				stats->Toggle(true);
				AddStatGauge(false);
			}
		}
		void Update(float deltaTime)
		{
			DebugDrawText::log(SUC::Format("STATE: %d\nFOOTER: %d\nSTATUSINDEX: %d", m_CurrentState, m_IsFooterActive, m_CurrentStatSelection), 0, 100, TEXT_CYAN);
			auto statsManager = m_IsWerehog ? statsNight : statsDay;
			switch (m_CurrentState)
			{
			case StartAdd:
			{
				m_TimerStatCreationDelay -= deltaTime;

				//Add all stats
				if (m_TimerStatCreationDelay <= 0 && m_CreatedStatIndex != statsManager->stats.size())
				{
					AddStatGauge();
					m_TimerStatCreationDelay = 0.17f;
				}
				//Wait half a second before selecting first element
				if (m_TimerDelayFirstSelect != -1)
				{
					m_TimerDelayFirstSelect -= deltaTime;
					if (m_TimerDelayFirstSelect <= 0)
					{
						Select(m_CurrentStatSelection, true, false, -1);
						m_CurrentState = Idle;
					}
				}
				break;
			}
			case Start:
			{
				m_TimerDelayForStart -= deltaTime;
				if (m_TimerDelayForStart <= 0)
				{
					m_CurrentState = StartAdd;
					m_TimerDelayFirstSelect = 0.65f;
					m_TimerDelayForStart = -1;
					m_TimerStatCreationDelay = 0.17f;
				}
				break;
			}
			case Idle:
			{
				//Show footer with a delay
				if (!m_IsFooterActive)
				{
					bool finished = m_TimerFooter >= 1.06f;

					m_rcFooter->SetHideFlag(!finished);
					m_IsFooterActive = finished;

					if (!finished)
						m_TimerFooter += deltaTime;
				}
				//Stat selection
				Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

				bool isUp = padState->IsTapped(Sonic::eKeyState_LeftStickUp) || padState->IsTapped(Sonic::eKeyState_DpadUp);
				bool isDown = padState->IsTapped(Sonic::eKeyState_LeftStickDown) || padState->IsTapped(Sonic::eKeyState_DpadDown);
				m_PreviousStatSelection = m_CurrentStatSelection;
				if (isUp)
				{
					m_CurrentStatSelection--;
				}
				if (isDown)
				{
					m_CurrentStatSelection++;
				}
				if (isUp || isDown)
				{
					m_CurrentStatSelection = std::clamp(m_CurrentStatSelection, 0, (int)statsManager->stats.size());
					if (m_PreviousStatSelection != m_CurrentStatSelection)
						Select(m_CurrentStatSelection, isUp, false, m_PreviousStatSelection);
					Common::PlaySoundStaticCueName(m_SelectHandle, "sys_actstg_statescursor");
				}
				//Character switching
				if (m_TimerDelayFirstSelect <= 0)
				{
					if (m_TimerCharSwitchCooldown >= 0)
					{
						m_TimerCharSwitchCooldown -= deltaTime;
					}
					else
					{
						if (padState->IsTapped(Sonic::eKeyState_LeftBumper) || padState->IsTapped(Sonic::eKeyState_RightBumper))
						{
							m_IsSwitchingChara = true;
							m_TimerCharSwitchCooldown = m_SwitchCooldownMax;
							ToggleStats();
							Common::PlaySoundStaticCueName(m_SwitchHandle, "sys_actstg_statescharachange");
						}
					}
				}
				if (m_IsSwitchingChara)
				{
					if (m_TimerCharSwitchCooldown <= m_SwitchCooldownMax / 2)
					{
						Select(0, isUp, false, 0);
						m_IsSwitchingChara = false;
						CSDCommon::PlayAnimation(m_rcStatusTitle, GetStringWithPrefix("Intro_%s_Anim"), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100);
					}
				}
				if (padState->IsTapped(Sonic::eKeyState_B))
				{
					m_CurrentStatSelection = statsManager->stats.size();
					Select(m_CurrentStatSelection, false, false, m_PreviousStatSelection);					
				}
				if(m_CurrentStatSelection == statsManager->stats.size() && padState->IsTapped(Sonic::eKeyState_A))
				{
					m_CurrentState = Exit;
					HideStats();
				}
				break;
			}
			case Exit:
			{
				m_TimerEndDelay -= deltaTime;
				m_TimerQuitSoundDelay += deltaTime;
				SoundController::SetBGMVolume(Common::LerpFloat(0.63f, 0.0f, m_TimerQuitSoundDelay / 2));

				if(!m_PlayedFadeout)
				{
					m_PlayedFadeout = true;
					Common::PlaySoundStaticCueName(m_EndHandle, "sys_actstg_finaldecide");
					UnleashedHUD_API::StartFadeout();
					if (SUC::System::StageManager::s_InStoryMode)
						SUC::System::SaveManager::SaveToDisk();
				}
				
				if (m_TimerQuitSoundDelay >= 1.06f)
				{
					WRITE_JUMP(0x10B96E6, (void*)0x10B974B);
					WRITE_NOP(0x10B9976, 5);
				}
				break;
			}
			}

			//MedalUpdate(updateInfo);
			//
			//ControlCharacterSwitch(updateInfo);
			//
			//ControlStatSelection();
			//
			//FooterTimer(updateInfo);
		}
		void Kill()
		{
			//using Chao::CSD::CProject;
			//
			//CProject::DestroyScene(m_rcStatus.Get(), m_status_title);
			//CProject::DestroyScene(m_rcStatus.Get(), m_logo);
			//CProject::DestroyScene(m_rcStatus.Get(), m_medal_info);
			//CProject::DestroyScene(m_rcStatus.Get(), m_medal_s_gauge);
			//CProject::DestroyScene(m_rcStatus.Get(), m_medal_m_gauge);
			//CProject::DestroyScene(m_rcStatus.Get(), m_tag_bg_1);
			//CProject::DestroyScene(m_rcStatus.Get(), m_tag_txt_1);
			//CProject::DestroyScene(m_rcStatus.Get(), m_prgs_bg_1);
			//CProject::DestroyScene(m_rcStatus.Get(), m_prgs_bar_1);
			//CProject::DestroyScene(m_rcStatus.Get(), m_prgs_num_1);
			//CProject::DestroyScene(m_rcStatus.Get(), m_status_footer);
			//CProject::DestroyScene(m_rcStatus.Get(), m_decide_bg);
			//
			//if (m_spStatus)
			//{
			//	m_spStatus->SendMessage(m_spStatus->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			//	m_spStatus = nullptr;
			//}
			//
			//m_rcStatus = nullptr;
		}
		void Show()
		{
			Intro();
		}
	};
	static inline StatusNew* statusNew;
	static void RegisterHooks();
	static void Start(Sonic::CGameObject* in_GameObject);
	static void End();
	static void Kill();
	static bool CanEnd();
	static bool EndDelayFinish();
};

