Chao::CSD::RCPtr<Chao::CSD::CProject> rcNewUi;
Chao::CSD::RCPtr<Chao::CSD::CScene> basicSceneTemplate;
boost::shared_ptr<Sonic::CGameObjectCSD> spNewUi;

void CreateScreen(Sonic::CGameObject* pParentGameObject)
{
	if (rcNewUi && !spNewUi)
		pParentGameObject->m_pMember->m_pGameDocument->AddGameObject(spNewUi = boost::make_shared<Sonic::CGameObjectCSD>(rcNewUi, 0.5f, "HUD_B1", false), "main", pParentGameObject);
}
void KillScreen()
{
	if (spNewUi)
	{
		spNewUi->SendMessage(spNewUi->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		spNewUi = nullptr;
	}
}
void ToggleScreen(const bool visible, Sonic::CGameObject* pParentGameObject)
{
	if (visible)
		CreateScreen(pParentGameObject);
	else
		KillScreen();
}
void __fastcall CHudSonicStageRemoveCallback2(Sonic::CGameObject* This, void*, Sonic::CGameDocument* pGameDocument)
{
	KillScreen();

	Chao::CSD::CProject::DestroyScene(rcNewUi.Get(), basicSceneTemplate);

	rcNewUi = nullptr;
}
HOOK(volatile signed int*, __stdcall, CreateBTNSKip, 0x00B21A30, Sonic::CGameObject* This)
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	auto returnee = originalCreateBTNSKip(This);
	CHudSonicStageRemoveCallback2(This, nullptr, nullptr);
	Sonic::CCsdDatabaseWrapper wrapper(Sonic::CGameDocument::GetInstance()->m_pMember->m_spDatabase.get());

	auto spCsdProject = wrapper.GetCsdProject("ui_btn_guide");
	rcNewUi = spCsdProject->m_rcProject;
	basicSceneTemplate = rcNewUi->CreateScene("btn_guide_skip");
	CreateScreen(This);
	return returnee;
}
void EventViewer::applyPatches()
{
	INSTALL_HOOK(CreateBTNSKip);


	//IMPORTANT!!!!!!!!!
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback2);

}
