#pragma once

class EvilQTEUI
{
public:
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcSubtitleScreen;
	boost::shared_ptr<Sonic::CGameObjectCSD> spSubtitleScreen;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcBtn;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcTypePress;
	bool isChainPress;
	int btnType;
	SharedPtrTypeless soundEffect;
	static EvilQTEUI* Generate(Sonic::CGameObject* Parent, bool isChainPr, int btnty)
	{
		EvilQTEUI* dataOut = new EvilQTEUI();
		dataOut->isChainPress = isChainPr;
		dataOut->btnType = btnty;
		Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_qte_ev");
		dataOut->rcSubtitleScreen = spCsdProject->m_rcProject;
		dataOut->rcTypePress = dataOut->rcSubtitleScreen->CreateScene(isChainPr ? "chain" : "single");
		dataOut->rcBtn = dataOut->rcSubtitleScreen->CreateScene(std::format("btn_{0}", btnty).c_str());
		dataOut->Hide();
		Parent->m_pMember->m_pGameDocument->AddGameObject(dataOut->spSubtitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(dataOut->rcSubtitleScreen, 0.5f, "HUD", false), "main", Parent);
		return dataOut;
	}
	void Hide()
	{
		rcTypePress->SetHideFlag(true);
		rcBtn->SetHideFlag(true);
	}
	void Dissapear()
	{
		CSDCommon::PlayAnimation(rcBtn, "Effect_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcTypePress, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
	}
	void Show()
	{
		Common::PlaySoundStaticCueName(soundEffect, "suc_ui_button_appear_sfx");
		rcTypePress->SetHideFlag(false);
		rcBtn->SetHideFlag(false);
		CSDCommon::PlayAnimation(rcBtn, "Intro_Anim", Chao::CSD::eMotionRepeatType_PlayOnce, 1, 0);
		CSDCommon::PlayAnimation(rcTypePress, "Intro_Anim", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
	}
};