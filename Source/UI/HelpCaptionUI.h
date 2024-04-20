#pragma once


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
class HelpCaptionUIContainer
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

	static HelpCaptionUIContainer* Generate(Sonic::CGameObject* Parent)
	{
		HelpCaptionUIContainer* dataOut = new HelpCaptionUIContainer();
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
	void Step()
	{

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
		const std::string input = std::string(in);
		std::stringstream ss(input);
		std::string line;
		std::vector<std::string> lines;
		for (std::string line; std::getline(ss, line);)
		{
			lines.push_back(line);
		}
		rcText->GetNode("text_0")->SetText(lines.at(0).c_str());
		if (lines.size() >= 2)
			rcText1->GetNode("text_0")->SetText(lines.at(1).c_str());
		if (lines.size() >= 3)
			rcText2->GetNode("text_0")->SetText(lines.at(2).c_str());
	}
	void SetText(int index)
	{
		if (strings == nullptr)
			SetUIText("HINTDATA DOES NOT EXIST");
		else if (strings->hintScreens.size() <= index)
			SetUIText("HINTSCREENS DOES NOT EXIST");
		else
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