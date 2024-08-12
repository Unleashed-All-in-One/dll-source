#include "HudStatus.h"

//class CHudStatus : public Sonic::CGameObject
//{
//    Chao::CSD::RCPtr<Chao::CSD::CProject> m_rcStatus;
//    boost::shared_ptr<Sonic::CGameObjectCSD> m_spStatus;
//
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_status_title;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_logo;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_info;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_s_gauge;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_medal_m_gauge;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_bg_1;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_tag_txt_1;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bg_1;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_bar_1;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_prgs_num_1;
//
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_decide_bg;
//    Chao::CSD::RCPtr<Chao::CSD::CScene> m_status_footer;
//
//    
//
//public:
//
//    void HudRemove()
//	{
//        RemoveStats();
//
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_status_title);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_logo);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_medal_info);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_medal_s_gauge);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_medal_m_gauge);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_tag_bg_1);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_tag_txt_1);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_prgs_bg_1);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_prgs_bar_1);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_prgs_num_1);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_status_footer);
//        Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), m_decide_bg);
//
//        if (m_spStatus) 
//        {
//            m_spStatus->SendMessage(m_spStatus->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
//            m_spStatus = nullptr;
//        }
//
//        m_rcStatus = nullptr;
//    }
//
//    void RemoveStats()
//	{
//        for (Stat& s : statsDay.stats)
//        {
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_tag_bg_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_tag_txt_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_bg_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_bar_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_num_2);
//        }
//
//        for (Stat& s : statsNight.stats)
//        {
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_tag_bg_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_tag_txt_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_bg_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_bar_2);
//            Chao::CSD::CProject::DestroyScene(m_rcStatus.Get(), s.m_prgs_num_2);
//        }
//    }
//
//    void ToggleStats()
//	{
//        isWerehog = !isWerehog;
//
//        //Exp Bar
//        m_tag_bg_1->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_tag_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        m_tag_txt_1->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_tag_txt_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        m_prgs_bg_1->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_prgs_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        m_prgs_bar_1->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_prgs_bar_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        m_prgs_num_1->SetHideFlag(false);
//        m_prgs_num_1->GetNode("num")->SetText(std::to_string(CurrentStatManager().expLevel).c_str());
//        CellSpriteUtil::PlayAnimation(m_prgs_num_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        m_decide_bg->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_decide_bg, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 0.0, 100.0, 100.0);
//
//        m_status_title->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_status_title, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 0.0, 0, 0, false, true);
//
//        m_logo->SetHideFlag(false);
//        CellSpriteUtil::PlayAnimation(m_logo, "Switch_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0, 20.0, false, !isWerehog);
//
//        currentStatIndex = 0;
//        statusIndex = 0;
//        y = 0.0f;
//
//        RemoveStats();
//        AddAllStats();
//    }
//
//    void Select(int index, bool up, bool reverse, bool quit = false) {
//
//        if (quit) {
//            CellSpriteUtil::PlayAnimation(m_decide_bg, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index - 1].m_tag_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index - 1].m_prgs_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index - 1].m_prgs_num_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index - 1].m_prgs_bar_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//        }
//        else {
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index].m_tag_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index].m_prgs_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index].m_prgs_num_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[index].m_prgs_bar_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
//        }
//
//        if (!quit && reverse && up && index != CurrentStatSizeMinusOne()) {
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index + 1, 0, CurrentStatSizeMinusOne())].m_tag_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index + 1, 0, CurrentStatSizeMinusOne())].m_prgs_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index + 1, 0, CurrentStatSizeMinusOne())].m_prgs_num_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index + 1, 0, CurrentStatSizeMinusOne())].m_prgs_bar_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//        }
//        else if (!quit && reverse && up && index == CurrentStatSizeMinusOne())
//            CellSpriteUtil::PlayAnimation(m_decide_bg, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//        else if (!quit && reverse && !up) {
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index - 1, 0, CurrentStatSizeMinusOne())].m_tag_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index - 1, 0, CurrentStatSizeMinusOne())].m_prgs_bg_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index - 1, 0, CurrentStatSizeMinusOne())].m_prgs_num_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//            CellSpriteUtil::PlayAnimation(CurrentStats()[std::clamp(index - 1, 0, CurrentStatSizeMinusOne())].m_prgs_bar_2, select(), Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0, 0, false, true);
//        }
//    }
//
//    void AddStatCommon()
//	{
//        
//
//        
//        
//        
//        
//
//        
//        
//        
//
//        
//        
//        
//        
//
//        
//        
//        
//
//        
//        
//        
//        
//        
//        
//        
//
//        
//        
//        
//        
//        
//        
//
//        
//
//        
//        
//        
//        
//        
//    }
//
//    void AddStat(bool woosh = true) {
//        if (currentStatIndex >= CurrentStats().size())
//            return;
//
//        AddStatCommon();
//
//        if (woosh) 
//            Common::PlaySoundStatic(wooshHandle, 1000029);
//    }
//
//    void AddAllStats() {
//        selectDelay = 0.65f;
//        for (int i = 0; i < CurrentStats().size(); i++)
//        {
//            currentStatIndex = i;
//            AddStatCommon();
//        }
//    }
//
//    void Outro() {
//        // Quit Button
//        CellSpriteUtil::PlayAnimation(m_decide_bg, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        // Header
//        CellSpriteUtil::PlayAnimation(m_status_title, outro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//        // Character Portrait
//        CellSpriteUtil::PlayAnimation(m_logo, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        // Bottom Buttons
//        m_status_footer->SetHideFlag(true);
//
//        //Medals (Top Right)
//        CellSpriteUtil::PlayAnimation(m_medal_info, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_medal_s_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_medal_m_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        currentStatIndex = std::clamp(currentStatIndex, 0,  CurrentStatSizeMinusOne());
//
//        for (int i = 0; i < CurrentStats().size(); i++)
//        {
//            Stat& s = CurrentStats()[i];
//            CellSpriteUtil::PlayAnimation(s.m_tag_bg_2, intro2(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//            CellSpriteUtil::PlayAnimation(s.m_tag_txt_2, (CurrentStat().txt_num < 3) ? intro2() : "Intro_ev_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//            CellSpriteUtil::PlayAnimation(s.m_prgs_num_2, intro2(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//            CellSpriteUtil::PlayAnimation(s.m_prgs_bg_2, intro2(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//            CellSpriteUtil::PlayAnimation(s.m_prgs_bar_2, intro2(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//        }
//
//        // Exp Bar
//        CellSpriteUtil::PlayAnimation(m_tag_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_tag_txt_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_prgs_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_prgs_bar_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        CellSpriteUtil::PlayAnimation(m_prgs_num_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0, 0.0, false, true);
//
//        EndDelay = 1.5f;
//        exit = true;
//    }
//
//    void Start()
//	{
//       m_tag_bg_1->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_tag_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_tag_txt_1->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_tag_txt_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_prgs_bg_1->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_prgs_bg_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_prgs_bar_1->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_prgs_bar_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_prgs_num_1->SetHideFlag(false);
//       m_prgs_num_1->GetNode("num")->SetText(std::to_string(CurrentStatManager().expLevel).c_str());
//       CellSpriteUtil::PlayAnimation(m_prgs_num_1, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_decide_bg->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_decide_bg, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_status_footer->SetHideFlag(true);
//       CellSpriteUtil::PlayAnimation(m_status_footer, "Usual_Anim_2", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_status_title->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_status_title, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_logo->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_logo, intro(), Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_medal_info->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_medal_info, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_medal_s_gauge->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_medal_s_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       m_medal_m_gauge->SetHideFlag(false);
//       CellSpriteUtil::PlayAnimation(m_medal_m_gauge, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1.0, 0.0);
//
//       medalSubImage = 0.0f;
//
//       timeFooter = 0.0f;
//       footerVisible = false;
//
//       statDelayStart = 0.75f;
//
//       currentStatIndex = 0;
//       statusIndex = 0;
//       y = 0.0f;
//
//       quitSoundTimer = 0.0f;
//
//       exit = false;
//    }
//    bool needToStart;
//    void AddCallback
//    (
//        const Hedgehog::Base::THolder<Sonic::CWorld>& worldHolder,
//        Sonic::CGameDocument* pGameDocument,
//        const boost::shared_ptr<Hedgehog::Database::CDatabase>& spDatabase
//    ) override
//    {
//        isWerehog = false;
//        Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
//        pGameDocument->AddUpdateUnit("1", this);
//
//        // initialize ui
//        Sonic::CCsdDatabaseWrapper wrapper(m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
//
//        auto spCsdProject = wrapper.GetCsdProject("ui_status");
//        m_rcStatus = spCsdProject->m_rcProject;
//
//        // Exp Bar
//        m_tag_bg_1 = m_rcStatus->CreateScene("tag_bg_1");
//
//        m_tag_txt_1 = m_rcStatus->CreateScene("tag_txt_1");
//
//        m_prgs_bg_1 = m_rcStatus->CreateScene("prgs_bg_1");
//
//        m_prgs_bar_1 = m_rcStatus->CreateScene("prgs_bar_1");
//
//        m_prgs_num_1 = m_rcStatus->CreateScene("prgs_num_1");
//
//        // Quit Button
//        m_decide_bg = m_rcStatus->CreateScene("decide_bg");
//
//        // Bottom Buttons
//        m_status_footer = m_rcStatus->CreateScene("status_footer");
//
//        // Header
//        m_status_title = m_rcStatus->CreateScene("status_title");
//
//        // Character Portrait
//        m_logo = m_rcStatus->CreateScene("logo");
//
//        //Medals (Top Right)
//        m_medal_info = m_rcStatus->CreateScene("medal_info");
//
//        m_medal_s_gauge = m_rcStatus->CreateScene("medal_s_gauge");
//
//        m_medal_m_gauge = m_rcStatus->CreateScene("medal_m_gauge");
//
//        m_spStatus = boost::make_shared<Sonic::CGameObjectCSD>(m_rcStatus, 0.5f, "HUD_B2", false);
//        Sonic::CGameDocument::GetInstance()->AddGameObject(m_spStatus, "main", this);
//
//    }
//
//    void MedalUpdate(const Hedgehog::Universe::SUpdateInfo& updateInfo) {
//
//        medalSubImage += updateInfo.DeltaTime * 25.0f;
//
//        if (medalSubImage >= 30)
//            medalSubImage = 0;
//
//        m_medal_info->GetNode("sun")->SetPatternIndex(std::clamp(medalSubImage, 0.0f, 29.0f));
//        m_medal_info->GetNode("sun_shade")->SetPatternIndex(std::clamp(medalSubImage, 0.0f, 29.0f));
//
//        m_medal_info->GetNode("moon")->SetPatternIndex(std::clamp(medalSubImage, 0.0f, 29.0f));
//        m_medal_info->GetNode("moon_shade")->SetPatternIndex(std::clamp(medalSubImage, 0.0f, 29.0f));
//    }
//
//    void ControlCharacterSwitch(const Hedgehog::Universe::SUpdateInfo& updateInfo) {
//        if (selectDelay != -1 || statDelayStart != -1) 
//            return;
//
//        if (switchCooldown > 0) {
//            switchCooldown -= updateInfo.DeltaTime;
//            return;
//        }
//
//        Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
//
//        if (padState->IsTapped(Sonic::eKeyState_LeftBumper) || padState->IsTapped(Sonic::eKeyState_RightBumper)) {
//            ToggleStats();
//            switchCooldown = 0.5f;
//            Common::PlaySoundStatic(switchHandle, 1000028);
//        }
//    }
//
//    void ControlStatSelection() {
//        if (selectDelay != -1 || statDelayStart != -1)
//            return;
//
//        Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
//
//        bool isUp = padState->IsTapped(Sonic::eKeyState_LeftStickUp) || padState->IsTapped(Sonic::eKeyState_DpadUp);
//
//        if (isUp && statusIndex == CurrentStats().size()) {
//            statusIndex = CurrentStatSizeMinusOne();
//            Select(statusIndex, true, true);
//            Common::PlaySoundStatic(selectHandle, 1000039);
//        }
//        else if (isUp && statusIndex != CurrentStats().size() && statusIndex != 0) {
//            statusIndex = (statusIndex - 1 >= 0) ? statusIndex - 1 : statusIndex;
//            Select(statusIndex, true, true);
//            Common::PlaySoundStatic(selectHandle, 1000039);
//        }
//
//        bool isDown = padState->IsTapped(Sonic::eKeyState_LeftStickDown) || padState->IsTapped(Sonic::eKeyState_DpadDown);
//
//        if (isDown && statusIndex + 1 <= CurrentStatSizeMinusOne()) {
//            statusIndex++;
//            Select(statusIndex, false, true);
//            Common::PlaySoundStatic(selectHandle, 1000039);
//        }
//        else if (isDown && statusIndex + 1 == CurrentStats().size()) {
//            statusIndex = CurrentStats().size();
//            Select(statusIndex, false, true, true);
//            Common::PlaySoundStatic(selectHandle, 1000039);
//        }
//    }
//
//    void FooterTimer(const Hedgehog::Universe::SUpdateInfo& updateInfo) {
//        if (footerVisible)
//            return;
//
//        bool finished = timeFooter >= 1.06f;
//
//        m_status_footer->SetHideFlag(!finished);
//        footerVisible = finished;
//
//        if (!finished)
//            timeFooter += updateInfo.DeltaTime;
//    }
//
//    void StatInitialization(const Hedgehog::Universe::SUpdateInfo& updateInfo) {
//        if (statDelayStart != -1) 
//            statDelayStart -= updateInfo.DeltaTime;
//
//        if (statDelayStart != -1 && statDelayStart <= 0) {
//            AddStat();
//            selectDelay = 0.65f;
//            statDelayStart = -1;
//            statDelay = 0.17f;
//        }
//
//        if(statDelayStart == -1) 
//            statDelay -= updateInfo.DeltaTime;
//
//        if (statDelayStart == -1 && selectDelay != -1 && selectDelay <= 0) {
//            Select(statusIndex, false, false);
//            selectDelay = -1;
//        }
//        else if (statDelayStart == -1 && selectDelay != -1 && selectDelay > 0)
//            selectDelay -= updateInfo.DeltaTime;
//
//        if (statDelayStart == -1 && statDelay <= 0) {
//            currentStatIndex++;
//            AddStat();
//            statDelay = 0.17f;
//            currentStatIndex = std::clamp(currentStatIndex, 0, CurrentStatSizeMinusOne());
//        }
//    }
//
//    void UpdateParallel
//    (
//        const Hedgehog::Universe::SUpdateInfo& updateInfo
//    ) override
//    {
//        if (exit) {
//            EndDelay -= updateInfo.DeltaTime;
//            return;
//        }
//        if(needToStart)
//            Start();
//
//        //CurrentStat().Update(updateInfo.DeltaTime);
//
//        StatInitialization(updateInfo);
//
//        if (quitSoundTimer >= 1.06f) {
//            Common::PlaySoundStatic(wooshHandle, 1000029);
//            quitSoundTimer = -1;
//        }
//        else if (quitSoundTimer != -1)
//            quitSoundTimer += updateInfo.DeltaTime;
//
//        MedalUpdate(updateInfo);
//
//        ControlCharacterSwitch(updateInfo);
//
//        ControlStatSelection();
//
//        FooterTimer(updateInfo);
//    }
//
//    void Kill() {
//        SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
//    }
//
//    bool IsQuitFinish() {
//        return EndDelay <= 0.0f;
//    }
//
//    bool IsQuit() {
//        return statusIndex == CurrentStats().size();
//    }
//
//    ~CHudStatus() {
//        HudRemove();
//    }
//};
boost::shared_ptr<Sonic::CGameObject> status;
// const char* _name, int _level, int _maxLevel, int _txt_num, int _count


void HudStatus::Start(Sonic::CGameObject* in_GameObject)
{
	if (!statusNew)
		statusNew = StatusNew::Generate(in_GameObject, false, 0);
	statusNew->Show();
	//if (status)
	//    HudStatus::Kill();
	//
	//status = boost::make_shared<CHudStatus>();
	//Sonic::CGameDocument::GetInstance()->AddGameObject(status);
}
float statusTimer;
void HudStatus::End()
{
	
	
	// ((CHudStatus*)status.get())->Outro();
}

bool HudStatus::CanEnd() {
	//if (!status)
	//    return false;
	//
	//return ((CHudStatus*)status.get())->IsQuit();
	return false;
}

bool HudStatus::EndDelayFinish() {
	//if (!status)
	//    return false;

	//return ((CHudStatus*)status.get())->IsQuitFinish();
	return false;
}

void HudStatus::Kill()
{
	statusNew->Kill();
}
extern "C" __declspec(dllexport) void API_StartStatsUI()
{
	HudStatus::Start(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer);
}
HOOK(void, __fastcall, Status_CHudSonicStage_Update, 0x1098A50, void* This, void* Edx, float* dt)
{
	if (HudStatus::statusNew)
		HudStatus::statusNew->Update(*dt);
	if(HudStatus::CanEnd())
	{
		static SharedPtrTypeless soundHandle;
		Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
		if (padState->IsTapped(Sonic::EKeyState::eKeyState_A) && statusTimer >= 2.5f)
		{
			statusTimer = 0.0f;
			//HudStatus::End();
			Common::PlaySoundStatic(soundHandle, 1000027);
			UnleashedHUD_API::StartFadeout();
		}		
	}	
	originalStatus_CHudSonicStage_Update(This, Edx, dt);
}

void HudStatus::RegisterHooks()
{
	INSTALL_HOOK(Status_CHudSonicStage_Update);
}