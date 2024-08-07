#pragma once


enum ItemboxType
{
	SpeedDown,
	L_45,
	L_90,
	L_pin,
	R_45,
	R_90,
	R_pin,
	Reverse,
	Bomb
};
class ItemboxUI
{
public:
	Chao::CSD::RCPtr<Chao::CSD::CProject> rcSubtitleScreen;
	boost::shared_ptr<Sonic::CGameObjectCSD> spSubtitleScreen;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcBomb_sign_position;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcBomb_sign;
	Chao::CSD::RCPtr<Chao::CSD::CScene> rcSign_box;

	static ItemboxUI* Generate(Sonic::CGameObject* Parent)
	{
		ItemboxUI* dataOut = new ItemboxUI();
		Sonic::CCsdDatabaseWrapper wrapper(Parent->m_pMember->m_pGameDocument->m_pMember->m_spDatabase.get());
		auto spCsdProject = wrapper.GetCsdProject("ui_itembox");
		dataOut->rcSubtitleScreen = spCsdProject->m_rcProject;
		dataOut->rcBomb_sign_position = dataOut->rcSubtitleScreen->CreateScene("bomb_sign_position");
		dataOut->rcBomb_sign = dataOut->rcSubtitleScreen->CreateScene("bomb_sign");
		dataOut->rcSign_box = dataOut->rcSubtitleScreen->CreateScene("sign_box");
		dataOut->Hide();
		Parent->m_pMember->m_pGameDocument->AddGameObject(dataOut->spSubtitleScreen = boost::make_shared<Sonic::CGameObjectCSD>(dataOut->rcSubtitleScreen, 0.5f, "HUD", false), "main", Parent);
		return dataOut;
	}
	void Hide()
	{
		rcBomb_sign_position->SetHideFlag(true);
		rcBomb_sign->SetHideFlag(true);
		rcSign_box->SetHideFlag(true);

	}
	void Show(ItemboxType type)
	{
		switch (type)
		{
		case SpeedDown:
		{
			rcSign_box->SetHideFlag(false);
			CSDCommon::PlayAnimation(rcSign_box, "speed_down", Chao::CSD::eMotionRepeatType_Loop, 1, 0);
			break;
		}
		}
	}
};