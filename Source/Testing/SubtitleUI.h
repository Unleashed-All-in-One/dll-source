#pragma once

enum CaptionButtonType : uint32_t
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
struct HintScreen
{
public:
	int emotionIndex;
	std::string text;
	int characterIndex;
	int imageIndex;
};
struct HintData
{
public:
	std::vector<HintScreen*> hintScreens;
};
struct HintDataList
{
public:
	std::vector<HintData*> hintData;
};
class SubtitleUIContainer
{
public:
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcSubtitleScreen;
	boost::shared_ptr<Sonic::CGameObjectCSD> spSubtitleScreen;
	HintData* strings;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcWindow;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcWindowArea;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcHelpChara1;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcText;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcText1;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcText2;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcHelpNametag;
	int textIndex;
	bool isComplete;

	static SubtitleUIContainer* Generate(Sonic::CGameObject* Parent)
	{
		SubtitleUIContainer* dataOut = new SubtitleUIContainer();
		dataOut->strings = new HintData();
		Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_help_suc");
		dataOut->rcSubtitleScreen = spCsdProject->m_rcProject;
		dataOut->rcWindow = dataOut->rcSubtitleScreen->CreateScene("help_window");
		dataOut->rcWindowArea = dataOut->rcSubtitleScreen->CreateScene("help_text_area");
		dataOut->rcHelpChara1 = dataOut->rcSubtitleScreen->CreateScene("help_chara_1");
		dataOut->rcHelpNametag = dataOut->rcSubtitleScreen->CreateScene("help_nametag");
		dataOut->rcText = dataOut->rcSubtitleScreen->CreateScene("ui_textbox_0");
		dataOut->rcText1 = dataOut->rcSubtitleScreen->CreateScene("ui_textbox_1");
		dataOut->rcText2 = dataOut->rcSubtitleScreen->CreateScene("ui_textbox_2");
		dataOut->rcText->GetNode("Text_sdw_0")->SetHideFlag(true);
		dataOut->rcText1->GetNode("Text_sdw_0")->SetHideFlag(true);
		dataOut->rcText2->GetNode("Text_sdw_0")->SetHideFlag(true);
		Parent->m_pMember->m_pGameDocument->AddGameObject(dataOut->spSubtitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(dataOut->rcSubtitleScreen, 0.5f, "HUD", false), "main", Parent);
		return dataOut;
	}
	void AddText(std::vector<std::string> in_String)
	{
	}
	void Reset()
	{
		textIndex = 0;
	}
	void SetUIText(std::string in)
	{
		rcText->GetNode("text_0")->SetText("");
		rcText1->GetNode("text_0")->SetText(" ");
		rcText2->GetNode("text_0")->SetText(" ");
		const string input = string(in);
		std::stringstream ss(input);
		string line;
		vector<string> lines;
		for (std::string line; std::getline(ss, line);)
		{
			lines.push_back(line);
		}
		rcText->GetNode("text_0")->SetText(lines.at(0).c_str());
		if(lines.size() >= 2)
		rcText1->GetNode("text_0")->SetText(lines.at(1).c_str());
		if (lines.size() >= 3)
		rcText2->GetNode("text_0")->SetText(lines.at(2).c_str());
	}
	void SetText(int index)
	{
		SetUIText(strings->hintScreens[index]->text);
		if (strings->hintScreens[index]->imageIndex == -1)
		{
			rcWindowArea->GetNode("imgbox")->SetHideFlag(true);
		}
		else
		{
			rcWindowArea->GetNode("imgbox")->SetHideFlag(false);
			rcWindowArea->GetNode("imgbox")->SetPatternIndex(strings->hintScreens[index]->imageIndex);
		}
	}
	void Show()
	{
		isComplete = false;
		textIndex = 0;
		rcText->SetHideFlag(false);
		rcText1->SetHideFlag(false);
		rcText2->SetHideFlag(false);
		rcWindowArea->SetHideFlag(false);
		rcText->GetNode("text_0")->SetHideFlag(false);
		rcText1->GetNode("text_0")->SetHideFlag(false);
		rcText2->GetNode("text_0")->SetHideFlag(false);
		CSDCommon::PlayAnimation(*rcWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*rcHelpNametag, "Intro_chip_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(*rcHelpChara1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		SetText(0);
	}
	void Progress()
	{
		textIndex++;
		if (textIndex >= strings->hintScreens.size())
		{
			Hide();
		}
		else
			SetText(textIndex);
	}
	void Hide()
	{
		isComplete = true;
		rcWindowArea->SetHideFlag(true);

		rcText->GetNode("text_0")->SetHideFlag(true);
			rcText1->GetNode("text_0")->SetHideFlag(true);
			rcText2->GetNode("text_0")->SetHideFlag(true);
		CSDCommon::PlayAnimation(*rcHelpNametag, "Intro_chip_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
		CSDCommon::PlayAnimation(*rcHelpChara1, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
		CSDCommon::PlayAnimation(*rcWindow, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 100, 0, false, true);
	}
	void Kill()
	{
		spSubtitleScreen->SendMessage(spSubtitleScreen->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spSubtitleScreen = nullptr;
	}
};
struct Caption
{
	std::vector<std::string> m_captions;
	std::map<uint32_t, CaptionButtonType> m_buttons;
	float m_duration;
};

struct CaptionData
{
	uint32_t* m_owner;
	uint32_t* m_captionStart;
	bool m_isCutscene;
	float m_timer;
	bool m_bypassLoading;
	std::deque<Caption> m_captions;

	IUnknown* m_textbox;
	IUnknown* m_buttonA;
	IUnknown* m_buttonB;
	IUnknown* m_buttonX;
	IUnknown* m_buttonY;
	IUnknown* m_buttonLB;
	IUnknown* m_buttonLT;
	IUnknown* m_buttonRB;
	IUnknown* m_buttonRT;
	IUnknown* m_buttonStart;
	IUnknown* m_buttonBack;
	IUnknown* m_buttonLStick;
	IUnknown* m_buttonRStick;
	IUnknown* m_buttonDPad;

	bool init();
	void clear()
	{
		m_owner = nullptr;
		m_captionStart = nullptr;
		m_isCutscene = false;
		m_timer = 0.0f;
		m_bypassLoading = false;
		m_captions.clear();
	}

	~CaptionData()
	{
		if (m_textbox) m_textbox->Release();
		if (m_buttonA) m_buttonA->Release();
		if (m_buttonB) m_buttonB->Release();
		if (m_buttonX) m_buttonX->Release();
		if (m_buttonY) m_buttonY->Release();
		if (m_buttonLB) m_buttonLB->Release();
		if (m_buttonLT) m_buttonLT->Release();
		if (m_buttonRB) m_buttonRB->Release();
		if (m_buttonRT) m_buttonRT->Release();
		if (m_buttonStart) m_buttonStart->Release();
		if (m_buttonBack) m_buttonBack->Release();
		if (m_buttonLStick) m_buttonLStick->Release();
		if (m_buttonRStick) m_buttonRStick->Release();
		if (m_buttonDPad) m_buttonDPad->Release();
	}
};

class SubtitleUI
{
public:
	static bool m_initSuccess;
	static void applyPatches();

	static std::map<uint32_t, wchar_t> m_fontDatabase;
	static bool initFontDatabase();

	static CaptionData m_captionData;
	static void closeCaptionWindow();

	static void __cdecl addCaptionImpl(uint32_t* owner, uint32_t* caption, float duration, bool isCutscene);
	static void draw();
	static SubtitleUIContainer GenerateSubtitle(Sonic::CGameObject* Parent);
	static void clearDraw();
	static float drawCaptions(Caption const& caption, float alpha, bool isShadow = false, bool isCutscene = false);
};

