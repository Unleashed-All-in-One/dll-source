#include "SubtitleUI.h"
namespace SUC::UI::Event
{
	SubtitleUI::CaptionData SubtitleUI::s_CaptionData;
	void __declspec(naked) addCaption_COmochaoFollow()
	{
		static uint32_t sub_6B02B0 = 0x6B02B0;
		static uint32_t returnAddress = 0x461407;
		__asm
		{
			call[sub_6B02B0]

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

			call    SUC::UI::Event::SubtitleUI::AddCaptionImpl
			add     esp, 0x10

			pop     edx
			pop     eax

			jmp[returnAddress]
		}
	}

	void __declspec(naked) addCaption_MsgOmochao_PlayVoice()
	{
		static uint32_t sub_6B02B0 = 0x6B02B0;
		static uint32_t returnAddress = 0x1155E86;
		__asm
		{
			call[sub_6B02B0]

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

			call    SUC::UI::Event::SubtitleUI::AddCaptionImpl
			add     esp, 0x10

			pop     edx
			pop     eax

			jmp[returnAddress]
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
			movss   xmm0, dword ptr[edx + 4h]
			movss[eax], xmm0

			mov     byte ptr[ecx + 44h], 0 // force display immediately
			call[sub_6AE910]
			mov     cutsceneCaptionDuration, 0
			jmp[returnAddress]
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
			call    SUC::UI::Event::SubtitleUI::AddCaptionImpl
			add     esp, 0x10

			pop     ebp
			pop     eax

			// original function
			jump :
			mov     edx, [eax + 8]
				sub     edx, [eax + 4]
				jmp[returnAddress]
		}
	}

	HOOK(bool, __stdcall, SubtitleUI_CEventSceneStart, 0xB238C0, void* a1)
	{
		// Reset when cutscene starts
		SubtitleUI::s_CaptionData.Clear();
		return originalSubtitleUI_CEventSceneStart(a1);
	}

	HOOK(bool, __fastcall, SubtitleUI_CEventSceneAdvance, 0xB24A40, uint32_t* This, void* Edx, int a2)
	{
		// Reset when cutscene ends
		if (This[73] == 3)
		{
			SubtitleUI::s_CaptionData.Clear();
		}
		return originalSubtitleUI_CEventSceneAdvance(This, Edx, a2);
	}

	HOOK(int, __fastcall, SubtitleUI_CLastBossCaptionKill, 0xB16490, uint32_t This, void* Edx, int a2)
	{
		if (!SubtitleUI::s_CaptionData.m_Captions.empty())
		{
			if (SubtitleUI::s_CaptionData.m_Timer * 60.0f > 5.0f)
			{
				SubtitleUI::CaptionData::Caption& caption = SubtitleUI::s_CaptionData.m_Captions.front();
				SubtitleUI::s_CaptionData.m_Timer = caption.m_Duration - 5.0f / 60.0f;
			}
		}

		return originalSubtitleUI_CLastBossCaptionKill(This, Edx, a2);
	}

	bool SubtitleUI::s_InitSuccess = false;
	void SubtitleUI::RegisterHooks()
	{
		s_InitSuccess = InitializeFontDatabase();
		s_CaptionData.Initialize();
		// hooks have moved to CaptionData::init()
	}

	std::map<uint32_t, wchar_t> SubtitleUI::s_FontDatabase;
	bool SubtitleUI::InitializeFontDatabase()
	{
		std::ifstream database(SUC::Project::s_ModPath + "\\disk\\Fonts\\FontDatabase.txt");
		if (database.is_open())
		{
			std::stringstream ss;
			ss << database.rdbuf();
			database.close();

			uint32_t key = 0x82;
			std::wstring str = Common::multiByteToWideChar(ss.str().c_str());
			for (wchar_t const& c : str)
			{
				s_FontDatabase[key] = c;
				key++;
			}

			return true;
		}

		MessageBox(nullptr, TEXT("Failed to load font database, reverting to in-game textbox."), TEXT("Sonic 06 HUD"), MB_ICONWARNING);
		return false;
	}

	void SubtitleUI::CloseCaptionWindow()
	{
		if (!s_CaptionData.m_Captions.empty())
		{
			SubtitleUI::CaptionData::Caption& caption = s_CaptionData.m_Captions.front();

			float constexpr fadeOutTime = 5.0f / 60.0f;
			if (caption.m_Duration - s_CaptionData.m_Timer > fadeOutTime)
			{
				s_CaptionData.m_Timer = caption.m_Duration - fadeOutTime;
			}
		}
	}

	void __cdecl SubtitleUI::AddCaptionImpl(uint32_t* owner, uint32_t* caption, float duration, bool isCutscene)
	{
		// Caption disabled
		if ((*(uint8_t*)Common::ComposeAddressFromOffsets(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x18 }) & 0x10) == 0)
		{
			return;
		}

		// Read caption and convert to string
		uint32_t const length = (caption[2] - caption[1]) / 4;
		uint32_t* captionList = (uint32_t*)caption[1];

		if (s_CaptionData.m_Owner != owner || isCutscene || s_CaptionData.m_CaptionStart == captionList)
		{
			s_CaptionData.Clear();
		}
		s_CaptionData.m_Owner = owner;
		s_CaptionData.m_CaptionStart = captionList;
		s_CaptionData.m_IsCutscene = isCutscene;

		SubtitleUI::CaptionData::Caption newCaption;
		newCaption.m_Duration = duration;

		bool isJapanese = Sonic::CApplicationDocument::GetInstance()->m_UILanguage == Sonic::eLanguage_Japanese;
		bool adjustLineBreak = !s_CaptionData.m_IsCutscene;

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
					newCaption.m_Captions.push_back("");
				}
				else
				{
					newCaption.m_Captions.push_back(Common::wideCharToMultiByte(str.c_str()));
					str.clear();
				}
				newCaption.m_Buttons[newCaption.m_Captions.size() - 1] = (key == 0x78) ? CaptionData::Caption::ECaptionButtonType::CBT_DPad : (CaptionData::Caption::ECaptionButtonType)(key - 0x64);
				rowLength += 2;
			}
			else if (s_FontDatabase.count(key))
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
							newCaption.m_Captions.push_back(Common::wideCharToMultiByte(leftStr.c_str()));

							str = str.substr(space + 1);
							rowLength = str.size() * (isJapanese ? 2 : 1);
							linebreakCount++;
						}
					}

					str += s_FontDatabase[key];
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

					newCaption.m_Captions.push_back(Common::wideCharToMultiByte(str.c_str()));
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
			newCaption.m_Captions.push_back(Common::wideCharToMultiByte(str.c_str()));
		}

		s_CaptionData.m_Captions.push_back(newCaption);
		s_CaptionData.m_BypassLoading = Common::IsAtLoadingScreen();
	}

	bool SubtitleUI::CaptionData::Initialize()
	{
		if (!SubtitleUI::s_InitSuccess) return false;

		//std::wstring const dir = Application::getModDirWString();
		bool success = true;
		/*success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Textbox.dds").c_str(), &m_textbox);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_A.dds").c_str(), &m_buttonA);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_B.dds").c_str(), &m_buttonB);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_X.dds").c_str(), &m_buttonX);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_Y.dds").c_str(), &m_buttonY);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_LB.dds").c_str(), &m_buttonLB);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_LT.dds").c_str(), &m_buttonLT);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_RB.dds").c_str(), &m_buttonRB);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_RT.dds").c_str(), &m_buttonRT);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_Start.dds").c_str(), &m_buttonStart);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_Back.dds").c_str(), &m_buttonBack);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_LStick.dds").c_str(), &m_buttonLStick);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_RStick.dds").c_str(), &m_buttonRStick);
		success &= UIContext::LoadTextureFromFile((dir + L"Assets\\Textbox\\Button_DPad.dds").c_str(), &m_buttonDPad);*/

		if (!success)
		{
			m_Captions.clear();
			MessageBox(nullptr, TEXT("Failed to load assets for custom 06 textbox, reverting to in-game textbox."), TEXT("Sonic 06 HUD"), MB_ICONWARNING);
		}
		else
		{
			// Omochao subtitle
			WRITE_JUMP(0x461402, addCaption_COmochaoFollow);
			WRITE_JUMP(0x1155E81, addCaption_MsgOmochao_PlayVoice);
			WRITE_JUMP(0x11F8813, (void*)0x11F8979);

			// Cutscene subtitle
			WRITE_MEMORY(0xB16D7A, uint8_t, 0); // disable original textbox
			WRITE_JUMP(0xB16E6C, addCaption_GetCutsceneDuration);
			WRITE_JUMP(0x6AE09D, addCaption_Cutscene);
			INSTALL_HOOK(SubtitleUI_CEventSceneStart);
			INSTALL_HOOK(SubtitleUI_CEventSceneAdvance);
			INSTALL_HOOK(SubtitleUI_CLastBossCaptionKill);
		}

		return success;
	}

	void SubtitleUI::Draw()
	{
		if (s_CaptionData.m_BypassLoading)
		{
			// No longer at loading screen
			if (!Common::IsAtLoadingScreen())
			{
				s_CaptionData.m_BypassLoading = false;
			}
		}
		else if (Common::IsAtLoadingScreen())
		{
			// At loading screen, clear all
			ClearDraw();
			return;
		}

		if (!s_CaptionData.m_Captions.empty())
		{
			SubtitleUI::CaptionData::Caption& caption = s_CaptionData.m_Captions.front();
			float sizeX = *BACKBUFFER_WIDTH * 890.0f / 1280.0f;
			float sizeY = *BACKBUFFER_HEIGHT * 170.0f / 720.0f;
			float posX = 0.143f;
			float posY = 0.6958f;
			float alpha = 1.0f;

			static bool visible = true;
			if (s_CaptionData.m_IsCutscene)
			{
				// Don't draw first frame since position is stale
				alpha = (s_CaptionData.m_Timer == 0.0f) ? 0.0f : 1.0f;
			}
			else
			{
				ImGui::Begin("Textbox", &visible, SubtitleUI::s_ImguiFlags);
				{
					// Fade in and out
					float frame1 = s_CaptionData.m_Timer * 60.0f;
					float frame2 = (caption.m_Duration - s_CaptionData.m_Timer) * 60.0f;
					if (frame1 < 5.0f)
					{
						posY += 0.03476f * (5.0f - frame1);
						alpha = 0.2f * frame1;
					}
					else if (frame2 < 5.0f)
					{
						posY += 0.03476f * (5.0f - frame2);
						alpha = 0.2f * frame2;
					}

					ImGui::SetWindowFocus();
					ImGui::SetWindowSize(ImVec2(sizeX, sizeY));
					//ImGui::Image(m_captionData.m_textbox, ImVec2(sizeX, sizeY), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, alpha * 0.9f));
					ImGui::SetWindowPos(ImVec2(*BACKBUFFER_WIDTH * posX, *BACKBUFFER_HEIGHT * posY));
				}
				ImGui::End();
			}

			ImGui::Begin("Caption", &visible, SubtitleUI::s_ImguiFlags);
			{
				ImGui::SetWindowFocus();
				ImGui::SetWindowSize(ImVec2(sizeX, sizeY));

				if (s_CaptionData.m_IsCutscene)
				{
					DrawCaptions(caption, 1, true, true);
					float textWidth = DrawCaptions(caption, 1, false, true);
					ImGui::SetWindowPos(ImVec2((*BACKBUFFER_WIDTH - textWidth) * 0.5f - 10.0f, *BACKBUFFER_HEIGHT * 0.8502f - 10.0f));
				}
				else
				{
					DrawCaptions(caption, 1);
					ImGui::SetWindowPos(ImVec2(*BACKBUFFER_WIDTH * 0.2023f, *BACKBUFFER_HEIGHT * (posY + 0.047f)));
				}
			}
			ImGui::End();

			s_CaptionData.m_Timer += s_CaptionData.m_IsCutscene ? SUC::Project::GetDeltaTime() : SUC::Project::GetHudDeltaTime();
			if (s_CaptionData.m_Timer > caption.m_Duration)
			{
				s_CaptionData.m_Captions.pop_front();
				s_CaptionData.m_Timer = 0.0f;

				// Finished
				if (s_CaptionData.m_Captions.empty())
				{
					s_CaptionData.Clear();
				}
			}
		}
	}

	void SubtitleUI::ClearDraw()
	{
		s_CaptionData.Clear();
		cutsceneCaptionDuration = 0.0f;
	}

	float SubtitleUI::DrawCaptions(SubtitleUI::CaptionData::Caption const& caption, float alpha, bool isShadow, bool isCutscene)
	{
		float maxWidth = 0.0f;
		float currentWidth = 0.0f;

		float const offset = 4.0f;
		float const shadowPosX = *BACKBUFFER_WIDTH * 0.002f;
		if (isShadow)
		{
			ImGui::SetCursorPos(ImVec2(shadowPosX + offset, *BACKBUFFER_HEIGHT * 0.0036f + offset));
		}
		else
		{
			ImGui::SetCursorPos(ImVec2(offset, offset));
		}

		for (uint32_t i = 0; i < caption.m_Captions.size(); i++)
		{
			std::string const& str = caption.m_Captions[i];
			float color = isShadow ? 0.0f : 1.0f;

			ImGui::TextColored(ImVec4(color, color, color, alpha * 0.9f), str.c_str());
			currentWidth += ImGui::CalcTextSize(str.c_str()).x;

			if (caption.m_Buttons.count(i))
			{
				float buttonSizeX = 28.0f;
				switch (caption.m_Buttons.at(i))
				{
				case CaptionData::Caption::CBT_LB:
				case CaptionData::Caption::CBT_RB:
				case CaptionData::Caption::CBT_LT:
				case CaptionData::Caption::CBT_RT:
					buttonSizeX = 56.0f;
					break;
				}

				IUnknown** texture = nullptr;
				switch (caption.m_Buttons.at(i))
				{
					case CaptionData::Caption::CBT_A:		texture = &s_CaptionData.m_ButtonA;      break;
					case CaptionData::Caption::CBT_B:		texture = &s_CaptionData.m_ButtonB;      break;
					case CaptionData::Caption::CBT_X:		texture = &s_CaptionData.m_ButtonX;      break;
					case CaptionData::Caption::CBT_Y:		texture = &s_CaptionData.m_ButtonY;      break;
					case CaptionData::Caption::CBT_LB:		texture = &s_CaptionData.m_ButtonLB;     break;
					case CaptionData::Caption::CBT_RB:		texture = &s_CaptionData.m_ButtonRB;     break;
					case CaptionData::Caption::CBT_LT:		texture = &s_CaptionData.m_ButtonLT;     break;
					case CaptionData::Caption::CBT_RT:		texture = &s_CaptionData.m_ButtonRT;     break;
					case CaptionData::Caption::CBT_Start:	texture = &s_CaptionData.m_ButtonStart;  break;
					case CaptionData::Caption::CBT_Back:	texture = &s_CaptionData.m_ButtonBack;   break;
					case CaptionData::Caption::CBT_LStick:  texture = &s_CaptionData.m_ButtonLStick; break;
					case CaptionData::Caption::CBT_RStick:  texture = &s_CaptionData.m_ButtonRStick; break;
					case CaptionData::Caption::CBT_DPad:	texture = &s_CaptionData.m_ButtonDPad;   break;
				}

				ImGui::SameLine();
				if (texture)
				{
					//ImGui::Image(*texture, ImVec2(*BACKBUFFER_WIDTH * buttonSizeX / 1280.0f, *BACKBUFFER_HEIGHT * 28.0f / 720.0f), ImVec2(0, 0), ImVec2(1, 1), ImVec4(1, 1, 1, isShadow ? 0.0f : alpha));
					currentWidth += buttonSizeX;
				}
			}

			if (!str.empty() && str.back() == '\n')
			{
				ImGui::SetCursorPosX(isShadow ? shadowPosX + offset : offset);
				if (!isCutscene)
				{
					// y-spacing is slightly larger for gameplay dialog
					ImGui::SetCursorPosY(ImGui::GetCursorPosY() + *BACKBUFFER_HEIGHT * 0.01f);
				}

				maxWidth = max(maxWidth, currentWidth);
				currentWidth = 0.0f;
			}
			else
			{
				ImGui::SameLine();
			}
		}

		maxWidth = max(maxWidth, currentWidth);
		return maxWidth;
	}
}
