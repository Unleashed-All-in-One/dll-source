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
				const char* name = "";
				int level = 0;
				int maxLevel = 0;
				int txt_num = 0;
				int count = 0;
				float levelProgress = 0.0f;

				Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_bg_2;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_txt_2;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bg_2;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bar_2;
				Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_num_2;

				void GenerateScenes(const Chao::CSD::RCPtr<Chao::CSD::CProject>& in_Project)
				{
					m_tag_bg_2	=		in_Project->CreateScene("tag_bg_2");
					m_tag_txt_2 =		in_Project->CreateScene("tag_txt_2");
					m_prgs_bg_2 =		in_Project->CreateScene("prgs_bg_2");
					m_prgs_num_2=		in_Project->CreateScene("prgs_num_2");
					m_prgs_bar_2=		in_Project->CreateScene("prgs_bar_2");

					m_tag_bg_2->SetHideFlag(true);
					m_tag_txt_2->SetHideFlag(true);
					m_prgs_bg_2->SetHideFlag(true);
					m_prgs_num_2->SetHideFlag(true);
					m_prgs_bar_2->SetHideFlag(true);
				}

				const char* TextNumIntToString()
				{
					switch (txt_num)
					{
					case 1:
						return "tag_txt_2";
						break;
					case 2:
						return "tag_txt_3";
						break;
					case 3:
						return "tag_txt_4";
						break;
					case 4:
						return "tag_txt_5";
						break;
					case 5:
						return "tag_txt_6";
						break;
					default:
						return "tag_txt_2";
						break;
					}
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
		SharedPtrTypeless wooshHandle;
		SharedPtrTypeless selectHandle;
		SharedPtrTypeless switchHandle;
		Chao::CSD::RCPtr<Chao::CSD::CProject> m_rcStatus;
		boost::shared_ptr<Sonic::CGameObjectCSD> m_spStatus;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_status_title;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_logo;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_info;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_s_gauge;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_m_gauge;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_bg_1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_txt_1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bg_1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bar_1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_num_1;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_decide_bg;
		Chao::CSD::RCPtr<Chao::CSD::CScene> m_status_footer;
		float timeFooter = 0.0f;
		bool footerVisible = false;
		float m_StatCreationDelay = 0.17f;
		float y = 0.0f;
		float m_DelayForStart = 0.0f;
		float switchCooldown = 0.0f;
		float EndDelay;
		bool m_IsGoingToNextStage = false;
		float m_DelayFirstSelect = -1;
		float quitSoundTimer = 0.0f;
		float medalSubImage = 0.0f;
		SharedPtrTypeless soundEffect;
		bool isWerehog = false;
		int currentStatIndex = 0;
		int previousStatIndex = 0;
		int statusIndex = 0;
		StatManager* statsDay;
		StatManager* statsNight;
		const char* m_AnimIntro;
		const char* GetStringWithPrefix(const char* format)
		{
			char buffer[100];
			if (isWerehog)
				sprintf(buffer, format, "ev");
			else
				sprintf(buffer, format, "so");
			return buffer;
		}
		void Intro()
		{
			auto statsManager = isWerehog ? statsNight : statsDay;
			m_AnimIntro = GetStringWithPrefix("Intro_%s_Anim");
			m_prgs_num_1->GetNode("num")->SetText(std::to_string(statsManager->expLevel).c_str());
			CSDCommon::PlayAnimation(m_tag_bg_1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_tag_txt_1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_prgs_bg_1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_prgs_bar_1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_prgs_num_1, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_decide_bg, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_status_footer, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_status_title, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_logo, m_AnimIntro, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_medal_info, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_medal_s_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
			CSDCommon::PlayAnimation(m_medal_m_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);

			medalSubImage = 0.0f;
			timeFooter = 0.0f;
			footerVisible = false;
			m_DelayForStart = 0.75f;
			currentStatIndex = 0;
			statusIndex = 0;
			y = 0.0f;
			quitSoundTimer = 0.0f;
			m_IsGoingToNextStage = false;
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
			dataOut->m_tag_bg_1 = dataOut->m_rcStatus->CreateScene("tag_bg_1");
			dataOut->m_tag_txt_1 = dataOut->m_rcStatus->CreateScene("tag_txt_1");
			dataOut->m_prgs_bg_1 = dataOut->m_rcStatus->CreateScene("prgs_bg_1");
			dataOut->m_prgs_bar_1 = dataOut->m_rcStatus->CreateScene("prgs_bar_1");
			dataOut->m_prgs_num_1 = dataOut->m_rcStatus->CreateScene("prgs_num_1");
			// Quit Button               
			dataOut->m_decide_bg = dataOut->m_rcStatus->CreateScene("decide_bg");
			// Bottom Buttons            
			dataOut->m_status_footer = dataOut->m_rcStatus->CreateScene("status_footer");
			// Header                    
			dataOut->m_status_title = dataOut->m_rcStatus->CreateScene("status_title");
			// Character Portrait       
			dataOut->m_logo = dataOut->m_rcStatus->CreateScene("logo");
			//Medals (Top Right)         
			dataOut->m_medal_info = dataOut->m_rcStatus->CreateScene("medal_info");
			dataOut->m_medal_s_gauge = dataOut->m_rcStatus->CreateScene("medal_s_gauge");
			dataOut->m_medal_m_gauge = dataOut->m_rcStatus->CreateScene("medal_m_gauge");

			StatManager::Stat* speed = new StatManager::Stat{ "speed", 9, 11, 1, 10 };
			StatManager::Stat* ring_energy = new StatManager::Stat{ "ring_energy", 6, 6, 2, 0 };
			StatManager::Stat* combat = new StatManager::Stat{ "combat", 0, 31, 1, 0 };
			StatManager::Stat* strength = new StatManager::Stat{ "strength", 0, 11, 2, 0 };
			StatManager::Stat* life = new StatManager::Stat{ "life", 0, 11, 3, 0 };
			StatManager::Stat* unleash = new StatManager::Stat{ "unleash", 0, 11, 4, 0 };
			StatManager::Stat* shield = new StatManager::Stat{ "shield", 0, 11, 5, 0 };
			dataOut->statsDay = new StatManager();
			dataOut->statsNight = new StatManager();
			dataOut->statsDay->stats = { speed, ring_energy };
			dataOut->statsNight->stats = { combat, strength, life, unleash, shield };
			for (auto stat : dataOut->statsDay->stats)
			{
				stat->GenerateScenes(dataOut->m_rcStatus);
			}
			for (auto stat : dataOut->statsNight->stats)
			{
				stat->GenerateScenes(dataOut->m_rcStatus);
			}
			dataOut->statsNight->expLevel = 1;
			return dataOut;
		}
		void Hide()
		{

		}
		void Dissapear()
		{
		}
		void CreateAndAnimateScene(Chao::CSD::RCPtr<Chao::CSD::CScene> scene, const char* animation, float positionY)
		{
			scene->SetHideFlag(false);
			scene->SetPosition(0.0f, positionY);
			CSDCommon::PlayAnimation(scene, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0f, 0.0f);
		}
		void AddStat(bool woosh = true)
		{
			auto statsManager = isWerehog ? statsNight : statsDay;
			auto m_AnimIntro2 = GetStringWithPrefix("Intro_%s_Anim_2");

			// Create and animate scenes
			CreateAndAnimateScene(statsManager->stats[currentStatIndex]->m_prgs_bar_2, m_AnimIntro2, y);
			CreateAndAnimateScene(statsManager->stats[currentStatIndex]->m_prgs_bg_2, m_AnimIntro2, y);
			CreateAndAnimateScene(statsManager->stats[currentStatIndex]->m_prgs_num_2, m_AnimIntro2, y);
			CreateAndAnimateScene(statsManager->stats[currentStatIndex]->m_tag_bg_2, m_AnimIntro2, y);
			CreateAndAnimateScene(statsManager->stats[currentStatIndex]->m_tag_txt_2, m_AnimIntro2, y);
			// Set text for tag
			statsManager->stats[currentStatIndex]->m_tag_txt_2->SetHideFlag(false);
			auto txtAnim = (statsManager->stats[currentStatIndex]->txt_num < 3) ? m_AnimIntro2 : "Intro_ev_Anim_2";
			CSDCommon::PlayAnimation(statsManager->stats[currentStatIndex]->m_tag_txt_2, txtAnim, Chao::CSD::eMotionRepeatType_PlayOnce, 1.0f, 0.0f);

			// Handle progress number visibility
			if (statsManager->stats[currentStatIndex]->level >= statsManager->stats[currentStatIndex]->maxLevel)
			{										
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("num")->SetHideFlag(true);
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("img")->SetHideFlag(true);
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("txt")->SetHideFlag(false);
			}
			else
			{
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("num")->SetText(std::to_string(statsManager->stats[currentStatIndex]->level).c_str());
			}

			if (statsManager->stats[currentStatIndex]->count == 0)
			{
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("num_2")->SetHideFlag(true);
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("img_2")->SetHideFlag(true);
			}
			else
			{
				statsManager->stats[currentStatIndex]->m_prgs_num_2->GetNode("num_2")->SetText(std::to_string(statsManager->stats[currentStatIndex]->count).c_str());
			}

			// Update position for next stat
			y += (isWerehog ? 64.0f : 73.0f);
			currentStatIndex++;

			if (woosh)			
				Common::PlaySoundStatic(wooshHandle, 1000029);
			
		}
		void PlayAnimationForStat(StatManager::Stat* stat, const char* animation, bool isReverse = true)
		{
			CSDCommon::PlayAnimation(stat->m_tag_bg_2, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_prgs_bg_2, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_prgs_num_2, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
			CSDCommon::PlayAnimation(stat->m_prgs_bar_2, animation, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, isReverse);
		}
		void Select(int index, bool up, bool reverse, bool quit = false)
		{
			auto statsManager = isWerehog ? statsNight : statsDay;
			auto m_SelectAnim = GetStringWithPrefix("select_%s_Anim");
			int m_MaxSize = statsManager->stats.size() - 1;
			for (auto stat : statsManager->stats)
			{
				PlayAnimationForStat(stat, m_SelectAnim, true);
			}
			if (index == statsManager->stats.size())
			{
				PlayAnimationForStat(statsManager->stats[index - 1], m_SelectAnim, true);
				CSDCommon::PlayAnimation(m_decide_bg, m_SelectAnim, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0);
				return;
			}
			//PlayAnimationForStat(statsManager->stats[index], m_SelectAnim, false);

			PlayAnimationForStat(statsManager->stats[index], m_SelectAnim, reverse);

			//if (reverse)
			//{
			//	if (up)
			//	{
			//		if (index == m_MaxSize)
			//		{
			//			CSDCommon::PlayAnimation(m_decide_bg, m_SelectAnim, Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
			//		}
			//		else
			//		{
			//			PlayAnimationForStat(statsManager->stats[std::clamp(index + 1, 0, m_MaxSize)], m_SelectAnim, false);
			//		}
			//	}
			//	else
			//	{
			//		PlayAnimationForStat(statsManager->stats[std::clamp(index - 1, 0, m_MaxSize)], m_SelectAnim, true);
			//	}
			//}
		}
		void FooterTimer(const Hedgehog::Universe::SUpdateInfo& updateInfo)
		{
			if (footerVisible)
				return;

			
		}
		void Update(float deltaTime)
		{
			DebugDrawText::log(SUC::Format("STATE: %d\nFOOTER: %d\nSTATUSINDEX: %d", m_CurrentState, footerVisible, statusIndex),0, 100, TEXT_CYAN);
			auto statsManager = isWerehog ? statsNight : statsDay;
			switch (m_CurrentState)
			{
			case StartAdd:
			{
				m_StatCreationDelay -= deltaTime;

				//Add all stats
				if (m_StatCreationDelay <= 0 && currentStatIndex != statsManager->stats.size())
				{					
					AddStat();
					m_StatCreationDelay = 0.17f;
				}
				//Wait half a second before selecting first element
				if (m_DelayFirstSelect != -1)
				{
					m_DelayFirstSelect -= deltaTime;
					if (m_DelayFirstSelect <= 0)
					{
						Select(statusIndex, true, false);
						m_CurrentState = Idle;
					}
				}
				break;
			}
			case Start:
			{
				m_DelayForStart -= deltaTime;
				if(m_DelayForStart <= 0)
				{
					m_CurrentState = StartAdd;
					m_DelayFirstSelect = 0.65f;
					m_DelayForStart = -1;
					m_StatCreationDelay = 0.17f;
				}
				break;
			}
			case Idle:
				{
					//Show footer
					if(!footerVisible)
					{
						bool finished = timeFooter >= 1.06f;

						m_status_footer->SetHideFlag(!finished);
						footerVisible = finished;

						if (!finished)
							timeFooter += deltaTime;
					}

					Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();

					bool isUp = padState->IsTapped(Sonic::eKeyState_LeftStickUp) || padState->IsTapped(Sonic::eKeyState_DpadUp);
					bool isDown = padState->IsTapped(Sonic::eKeyState_LeftStickDown) || padState->IsTapped(Sonic::eKeyState_DpadDown);
					previousStatIndex = statusIndex;
					if (isUp)
					{
						statusIndex--;						
					}
					if (isDown)
					{
						statusIndex++;
					}
					if(isUp || isDown)
					{
						statusIndex = std::clamp(statusIndex, 0, (int)statsManager->stats.size());
						if(previousStatIndex != statusIndex)
						Select(statusIndex, isUp, false);
						Common::PlaySoundStatic(selectHandle, 1000039);
					}

				break;
				}
			case Exit:
			{
				EndDelay -= deltaTime;
				return;
			}
			}


			if (quitSoundTimer >= 1.06f) {
				Common::PlaySoundStatic(wooshHandle, 1000029);
				quitSoundTimer = -1;
			}
			else if (quitSoundTimer != -1)
				quitSoundTimer += deltaTime;

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

