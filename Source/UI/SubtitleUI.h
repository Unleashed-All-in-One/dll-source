#pragma once

namespace SUC::UI::Event
{
	class SubtitleUI
	{
	public:
		struct CaptionData
		{
			struct Caption
			{
				enum ECaptionButtonType : uint32_t
				{
					CBT_A = 0,
					CBT_B,
					CBT_X,
					CBT_Y,
					CBT_LB,
					CBT_RB,
					CBT_LT,
					CBT_RT,
					CBT_Start,
					CBT_Back,
					CBT_LStick,
					CBT_RStick,
					CBT_DPad,
				};
				std::vector<std::string> m_Captions;
				std::map<uint32_t, ECaptionButtonType> m_Buttons;
				float m_Duration;
			};
			uint32_t* m_Owner;
			uint32_t* m_CaptionStart;
			bool m_IsCutscene;
			float m_Timer;
			bool m_BypassLoading;
			std::deque<Caption> m_Captions;

			IUnknown* m_Textbox;
			IUnknown* m_ButtonA;
			IUnknown* m_ButtonB;
			IUnknown* m_ButtonX;
			IUnknown* m_ButtonY;
			IUnknown* m_ButtonLB;
			IUnknown* m_ButtonLT;
			IUnknown* m_ButtonRB;
			IUnknown* m_ButtonRT;
			IUnknown* m_ButtonStart;
			IUnknown* m_ButtonBack;
			IUnknown* m_ButtonLStick;
			IUnknown* m_ButtonRStick;
			IUnknown* m_ButtonDPad;

			bool Initialize();
			void Clear()
			{
				m_Owner = nullptr;
				m_CaptionStart = nullptr;
				m_IsCutscene = false;
				m_Timer = 0.0f;
				m_BypassLoading = false;
				m_Captions.clear();
			}

			~CaptionData()
			{
				if (m_Textbox) m_Textbox->Release();
				if (m_ButtonA) m_ButtonA->Release();
				if (m_ButtonB) m_ButtonB->Release();
				if (m_ButtonX) m_ButtonX->Release();
				if (m_ButtonY) m_ButtonY->Release();
				if (m_ButtonLB) m_ButtonLB->Release();
				if (m_ButtonLT) m_ButtonLT->Release();
				if (m_ButtonRB) m_ButtonRB->Release();
				if (m_ButtonRT) m_ButtonRT->Release();
				if (m_ButtonStart) m_ButtonStart->Release();
				if (m_ButtonBack) m_ButtonBack->Release();
				if (m_ButtonLStick) m_ButtonLStick->Release();
				if (m_ButtonRStick) m_ButtonRStick->Release();
				if (m_ButtonDPad) m_ButtonDPad->Release();
			}
		};
		static bool s_InitSuccess;
		static void RegisterHooks();

		static std::map<uint32_t, wchar_t> s_FontDatabase;
		static bool InitializeFontDatabase();

		static CaptionData s_CaptionData;
		static void CloseCaptionWindow();

		static void __cdecl AddCaptionImpl(uint32_t* owner, uint32_t* caption, float duration, bool isCutscene);
		static void Draw();
		static void ClearDraw();
		static float DrawCaptions(CaptionData::Caption const& caption, float alpha, bool isShadow = false, bool isCutscene = false);
		static constexpr ImGuiWindowFlags s_ImguiFlags
			= ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_AlwaysAutoResize
			| ImGuiWindowFlags_NoFocusOnAppearing
			| ImGuiWindowFlags_NoDecoration
			| ImGuiWindowFlags_NoBackground;
	};
}