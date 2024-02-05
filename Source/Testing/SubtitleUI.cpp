
CaptionData SubtitleUI::m_captionData;
void __declspec(naked) addCaption_COmochaoFollow()
{
    static uint32_t sub_6B02B0 = 0x6B02B0;
    static uint32_t returnAddress = 0x461407;
    __asm
    {
        call    [sub_6B02B0]

        push    eax
        push    edx

        // not cutscene
        push    0

        // duration
        mov     ecx, [esp + 0x1C + 0xC]
        push    ecx

        // caption
        mov     ecx, [eax]
        push    ecx

        // this (COmochao)
        mov     ecx, [esi + 8]
        push    ecx

        call    SubtitleUI::addCaptionImpl
        add     esp, 0x10

        pop     edx
        pop     eax

        jmp     [returnAddress]
    }
}

void __declspec(naked) addCaption_MsgOmochao_PlayVoice()
{
    static uint32_t sub_6B02B0 = 0x6B02B0;
    static uint32_t returnAddress = 0x1155E86;
    __asm
    {
        call    [sub_6B02B0]

        push    eax
        push    edx

        // not cutscene
        push    0

        // duration
        mov     ecx, [esp + 0x1C + 0xC]
        push    ecx

        // caption
        mov     ecx, [eax]
        push    ecx

        // this (COmochao)
        push    ebx

        call    SubtitleUI::addCaptionImpl
        add     esp, 0x10

        pop     edx
        pop     eax

        jmp     [returnAddress]
    }
}

float cutsceneCaptionDuration = 0.0f;
void __declspec(naked) addCaption_GetCutsceneDuration()
{
    static uint32_t sub_6AE910 = 0x6AE910;
    static uint32_t returnAddress = 0xB16E71;
    __asm
    {
        lea     eax, [cutsceneCaptionDuration]
        mov     edx, [esp + 20h]
        movss   xmm0, dword ptr [edx + 4h]
        movss   [eax], xmm0

        mov     byte ptr[ecx + 44h], 0 // force display immediately
        call    [sub_6AE910]
        mov     cutsceneCaptionDuration, 0
        jmp     [returnAddress]
    }
}

void __declspec(naked) addCaption_Cutscene()
{
    static uint32_t returnAddress = 0x6AE0A3;
    __asm
    {
        cmp     cutsceneCaptionDuration, 0
        jz      jump

        push    eax
        push    ebp

        push    1
        push    cutsceneCaptionDuration
        push    eax
        push    0
        call    SubtitleUI::addCaptionImpl
        add     esp, 0x10

        pop     ebp
        pop     eax

        // original function
        jump:
        mov     edx, [eax + 8]
        sub     edx, [eax + 4]
        jmp     [returnAddress]
    }
}

HOOK(bool, __stdcall, SubtitleUI_CEventSceneStart, 0xB238C0, void* a1)
{
    // Reset when cutscene starts
    SubtitleUI::m_captionData.clear();
    return originalSubtitleUI_CEventSceneStart(a1);
}
HOOK(bool, __fastcall, SubtitleUI_CEventSceneAdvance, 0xB24A40, uint32_t* This, void* Edx, int a2)
{
    // Reset when cutscene ends
    if (This[73] == 3)
    {
        SubtitleUI::m_captionData.clear();
    }
    return originalSubtitleUI_CEventSceneAdvance(This, Edx, a2);
}

HOOK(int, __fastcall, SubtitleUI_CLastBossCaptionKill, 0xB16490, uint32_t This, void* Edx, int a2)
{
    if (!SubtitleUI::m_captionData.m_captions.empty())
    {
        if (SubtitleUI::m_captionData.m_timer * 60.0f > 5.0f)
        {
            Caption& caption = SubtitleUI::m_captionData.m_captions.front();
            SubtitleUI::m_captionData.m_timer = caption.m_duration - 5.0f / 60.0f;
        }
    }

    return originalSubtitleUI_CLastBossCaptionKill(This, Edx, a2);
}

bool SubtitleUI::m_initSuccess = false;

std::map<uint32_t, wchar_t> SubtitleUI::m_fontDatabase;
void SubtitleUI::closeCaptionWindow()
{
    if (!m_captionData.m_captions.empty())
    {
        Caption& caption = m_captionData.m_captions.front();

        float constexpr fadeOutTime = 5.0f / 60.0f;
        if (caption.m_duration - m_captionData.m_timer > fadeOutTime)
        {
            m_captionData.m_timer = caption.m_duration - fadeOutTime;
        }
    }
}

void __cdecl SubtitleUI::addCaptionImpl(uint32_t* owner, uint32_t* caption, float duration, bool isCutscene)
{
    // Caption disabled
    if ((*(uint8_t*)Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x18 }) & 0x10) == 0)
    {
        return;
    }

    // Read caption and convert to string
    uint32_t const length = (caption[2] - caption[1]) / 4;
    uint32_t* captionList = (uint32_t*)caption[1];

    if (m_captionData.m_owner != owner || isCutscene || m_captionData.m_captionStart == captionList)
    {
        m_captionData.clear();
    }
    m_captionData.m_owner = owner;
    m_captionData.m_captionStart = captionList;
    m_captionData.m_isCutscene = isCutscene && (Common::GetCurrentStageID() & 0xFF) != SMT_blb;

    Caption newCaption;
    newCaption.m_duration = duration;

    bool isJapanese = Common::GetUILanguageType() == LT_Japanese;
    bool adjustLineBreak = !m_captionData.m_isCutscene;

    std::wstring str;
    int rowLength = 0;
    int linebreakCount = 0;
    for (uint32_t i = 0; i < length; i++)
    {
        uint32_t const key = captionList[i];
        if (key >= 0x64 && key <= 0x6F || key == 0x78)
        {
            // Button at the beginning of a line, add dummy text
            if (str.empty())
            {
                newCaption.m_captions.push_back("");
            }
            else
            {
                newCaption.m_captions.push_back(Common::wideCharToMultiByte(str.c_str()));
                str.clear();
            }
            newCaption.m_buttons[newCaption.m_captions.size() - 1] = (key == 0x78) ? CaptionButtonType::CBT_DPad : (CaptionButtonType)(key - 0x64);
            rowLength += 2;
        }
        else if (m_fontDatabase.count(key))
        {
            if (key == 0x82)
            {
                str += L"  ";
                rowLength += 2;
            }
            else
            {
                if (adjustLineBreak && linebreakCount < 2 && rowLength > 56)
                {
                    int space = str.find_last_of(L' ');
                    if (space != std::string::npos)
                    {
                        // Go back to the previous space to split from there
                        std::wstring const leftStr = str.substr(0, space + 1) + L'\n';
                        newCaption.m_captions.push_back(Common::wideCharToMultiByte(leftStr.c_str()));

                        str = str.substr(space + 1);
                        rowLength = str.size() * (isJapanese ? 2 : 1);
                        linebreakCount++;
                    }
                }

                str += m_fontDatabase[key];
                rowLength += isJapanese ? 2 : 1;
            }
        }
        else if (key == 0)
        {
            if (adjustLineBreak && !isJapanese)
            {
                // Don't do line break here
                if (!str.empty() && str.back() != L' ')
                {
                    str += L"  ";
                    rowLength += 2;
                }
            }
            else
            {
                str += L'\n';
                rowLength = 0;

                newCaption.m_captions.push_back(Common::wideCharToMultiByte(str.c_str()));
                str.clear();
            }
        }
        else if (key != 0x4)
        {
            str += L'?';
            rowLength++;
        }
    }

    // Push remaining text
    if (!str.empty())
    {
        newCaption.m_captions.push_back(Common::wideCharToMultiByte(str.c_str()));
    }

    m_captionData.m_captions.push_back(newCaption);
    m_captionData.m_bypassLoading = Common::IsAtLoadingScreen();
}

Chao::CSD::RCPtr<Chao::CSD::CProject> rcSubtitleScreen;
boost::shared_ptr<Sonic::CGameObjectCSD> spSubtitleScreen;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcWindow;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcWindowArea;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcHelpChara1;
Chao::CSD::RCPtr<Chao::CSD::CScene> rcHelpNametag;
void createScreen(Sonic::CGameObject* pParentGameObject)
{
    if (rcSubtitleScreen && !spSubtitleScreen)
    {
        pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spSubtitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(rcSubtitleScreen, 0.5f, "HUD", false), "main", pParentGameObject);
    }
}
void killScreen()
{
    if (spSubtitleScreen)
    {
        spSubtitleScreen->SendMessage(spSubtitleScreen->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
        spSubtitleScreen = nullptr;
    }
}
void __fastcall CSubtitleRemoveCallback(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
    //rcTitleLogo_1, rcTitlebg, rcTitleMenu, rcTitleMenuScroll, rcTitleMenuTXT, black_bg, bg_window, fg_window, txt_window, footer_window, bg_transition
    killScreen();
    Chao::CSD::CProject::DestroyScene(rcSubtitleScreen.Get(), rcWindow);
    Chao::CSD::CProject::DestroyScene(rcSubtitleScreen.Get(), rcHelpChara1);
    Chao::CSD::CProject::DestroyScene(rcSubtitleScreen.Get(), rcWindowArea);
    rcSubtitleScreen = nullptr;
   
}
//int __thiscall sub_11F8990(int parentGameObject, int worldHolder, int gameDocument, void **a4)
HOOK(int, __fastcall, sub_11F8990, 0x11F8990, Sonic::CGameObject* Parent, void* Edx, int worldHolder, Sonic::CGameDocument* gameDoc, void** a4)
{
    CSubtitleRemoveCallback(Parent, nullptr, nullptr);
    Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
    auto spCsdProject = wrapper.GetCsdProject("ui_help");
    rcSubtitleScreen = spCsdProject->m_rcProject;
    rcWindow = rcSubtitleScreen->CreateScene("help_window");
    rcWindowArea = rcSubtitleScreen->CreateScene("help_text_area");
    rcHelpChara1 = rcSubtitleScreen->CreateScene("help_chara_1");
    rcHelpNametag = rcSubtitleScreen->CreateScene("help_nametag");

    CSDCommon::PlayAnimation(*rcWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
    CSDCommon::PlayAnimation(*rcHelpNametag, "Intro_chip_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
    CSDCommon::PlayAnimation(*rcHelpChara1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
    createScreen(Parent);
    return originalsub_11F8990(Parent, Edx, worldHolder, gameDoc, a4);
}
bool wait = false;
bool skipped = false;
//sub_11F86B0(int this, const struct Hedgehog::Universe::SUpdateInfo *a2)
HOOK(void, __fastcall, sub_11F86B0, 0x11F86B0, void* This, void* Edx, const struct Hedgehog::Universe::SUpdateInfo* a2)
{   
    if (*((byte*)This + 253))
    {
        if (*((byte*)This + 288))
        {
           

            if (rcWindow && rcWindow->m_MotionDisableFlag)
            {
                rcWindowArea->SetHideFlag(true);
                CSDCommon::PlayAnimation(*rcHelpNametag, "Intro_chip_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
                CSDCommon::PlayAnimation(*rcHelpChara1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
                CSDCommon::PlayAnimation(*rcWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
            }
        }
    }
    if (!wait)
        originalsub_11F86B0(This, Edx, a2);
}
//int __thiscall sub_4609E0(CTempState *this)
HOOK(int, __fastcall, sub_4609E0, 0x4609E0, void* This)
{
    return 0;
}
HOOK(int, __fastcall, sub_4606A0, 0x4606A0, void* This)
{
    return 0;
}
void SubtitleUI::applyPatches()
{
	//// Omochao subtitle
	//WRITE_JUMP(0x461402, addCaption_COmochaoFollow);
	//WRITE_JUMP(0x1155E81, addCaption_MsgOmochao_PlayVoice);
	//WRITE_JUMP(0x11F8813, (void*)0x11F8979);
    WRITE_JUMP(0x011F8AC9, 0x011F8AE3);
    WRITE_JUMP(0x004606CF, 0x00460967);
    WRITE_MEMORY(0x016E11F4, void*, CSubtitleRemoveCallback);
	// Cutscene subtitle
	//WRITE_MEMORY(0xB16D7A, uint8_t, 0); // disable original textbox
	WRITE_JUMP(0xB16E6C, addCaption_GetCutsceneDuration);
	WRITE_JUMP(0x6AE09D, addCaption_Cutscene);
	INSTALL_HOOK(sub_11F8990);
	INSTALL_HOOK(sub_4609E0);
	//INSTALL_HOOK(sub_4606A0);
	INSTALL_HOOK(sub_11F86B0);
	INSTALL_HOOK(SubtitleUI_CEventSceneStart);
	INSTALL_HOOK(SubtitleUI_CEventSceneAdvance);
	INSTALL_HOOK(SubtitleUI_CLastBossCaptionKill);

}




//float SubtitleUI::drawCaptions(Caption const& caption, float alpha, bool isShadow, bool isCutscene)
//{
//    float maxWidth = 0.0f;
//    float currentWidth = 0.0f;
//
//    float const offset = 10.0f;
//    float const shadowPosX = *BACKBUFFER_WIDTH * 0.002f;
//    if (isShadow)
//    {
//        ImGui::SetCursorPos(ImVec2(shadowPosX + offset, *BACKBUFFER_HEIGHT * 0.0036f + offset));
//    }
//    else
//    {
//        ImGui::SetCursorPos(ImVec2(offset, offset));
//    }
//
//    for (uint32_t i = 0; i < caption.m_captions.size(); i++)
//    {
//        std::string const& str = caption.m_captions[i];
//        float color = isShadow ? 0.0f : 1.0f;
//
//        ImGui::TextColored(ImVec4(color, color, color, alpha * 0.9f), str.c_str());
//        currentWidth += ImGui::CalcTextSize(str.c_str()).x;
//
//        if (caption.m_buttons.count(i))
//        {
//            float buttonSizeX = 28.0f;
//            switch (caption.m_buttons.at(i))
//            {
//            case CBT_LB:
//            case CBT_RB:
//            case CBT_LT:
//            case CBT_RT:
//                buttonSizeX = 56.0f;
//                break;
//            }
//
//            IUnknown** texture = nullptr;
//            switch (caption.m_buttons.at(i))
//            {
//            case CBT_A:     texture = &m_captionData.m_buttonA;      break;
//            case CBT_B:     texture = &m_captionData.m_buttonB;      break;
//            case CBT_X:     texture = &m_captionData.m_buttonX;      break;
//            case CBT_Y:     texture = &m_captionData.m_buttonY;      break;
//            case CBT_LB:    texture = &m_captionData.m_buttonLB;     break;
//            case CBT_RB:    texture = &m_captionData.m_buttonRB;     break;
//            case CBT_LT:    texture = &m_captionData.m_buttonLT;     break;
//            case CBT_RT:    texture = &m_captionData.m_buttonRT;     break;
//            case CBT_Start: texture = &m_captionData.m_buttonStart;  break;
//            case CBT_Back:  texture = &m_captionData.m_buttonBack;   break;
//            case CBT_LStick:  texture = &m_captionData.m_buttonLStick;   break;
//            case CBT_RStick:  texture = &m_captionData.m_buttonRStick;   break;
//            case CBT_DPad:  texture = &m_captionData.m_buttonDPad;   break;
//            }
//
//            ImGui::SameLine();
//            if (texture)
//            {
//                ImGui::Image(*texture, ImVec2(*BACKBUFFER_WIDTH * buttonSizeX / 1280.0f, *BACKBUFFER_HEIGHT * 28.0f / 720.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, isShadow ? 0.0f : alpha));
//                currentWidth += buttonSizeX;
//            }
//        }
//
//        if (!str.empty() && str.back() == '\n')
//        {
//            ImGui::SetCursorPosX(isShadow ? shadowPosX + offset : offset);
//            if (!isCutscene)
//            {
//                // y-spacing is slightly larger for gameplay dialog
//                ImGui::SetCursorPosY(ImGui::GetCursorPosY() + *BACKBUFFER_HEIGHT * 0.01f);
//            }
//
//            maxWidth = max(maxWidth, currentWidth);
//            currentWidth = 0.0f;
//        }
//        else
//        {
//            ImGui::SameLine();
//        }
//    }
//
//    maxWidth = max(maxWidth, currentWidth);
//    return maxWidth;
//}
