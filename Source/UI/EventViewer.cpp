Chao::CSD::RCPtr<Chao::CSD::CProject> rcNewUi;
Chao::CSD::RCPtr<Chao::CSD::CScene> basicSceneTemplate;
boost::shared_ptr<Sonic::CGameObjectCSD> spNewUi;
DWORD* objecttt;
bool isGoingToSkip;
bool isInEventViewer;
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
HOOK(volatile signed int*, __stdcall, CreateBTNSKip, 0x00B21A30, DWORD* This)
{

	auto returnee = originalCreateBTNSKip(This);
	objecttt = This;
	isGoingToSkip = true;
	UnleashedHUD_API::StartFadeout();	
	return returnee;
}

void EventViewer::update()
{
	DebugDrawText::log(std::to_string(LevelLoadingManager::LastSavedQueueIndex).c_str(), 0);
	if (isInEventViewer)
	{
		if (isGoingToSkip)
		{
			if (UnleashedHUD_API::IsLoadingFadeoutCompleted())
			{
				LevelLoadingManager::setCorrectStage();
				isGoingToSkip = false;
				objecttt[77] = 2;
			}
		}
	}
}
//Hedgehog::Base::CSharedString *__thiscall sub_10FE500(DWORD *this, Hedgehog::Base::CSharedString *a2)
HOOK(Hedgehog::Base::CSharedString*, __fastcall, sub_10fe500, 0x10FE500, DWORD* EventQueue, void* Edx, Hedgehog::Base::CSharedString* a2)
{
	return originalsub_10fe500(EventQueue, Edx, a2);
}
//_DWORD *__thiscall sub_645C40(_DWORD *this, char a2)
HOOK(char, __fastcall, sub_10FE9E0, 0x10FE9E0, void* This, void* Edx, DWORD* a1, int a2)
{
	isInEventViewer = true;
	return originalsub_10FE9E0(This, Edx, a1,a2);
}
//char __thiscall sub_10FE9E0(void *this, DWORD *a1, int a2)
void EventViewer::applyPatches()
{
	INSTALL_HOOK(CreateBTNSKip);
	INSTALL_HOOK(sub_10fe500);
	INSTALL_HOOK(sub_10FE9E0);
	//WRITE_JUMP(0x00B21AD0, InterceptProject);
	//WRITE_JUMP(0x00B21A33, InterceptGameObject);

	WRITE_JUMP(0x00B21D38, 0x00B21CF3);
	//IMPORTANT!!!!!!!!!
	WRITE_MEMORY(0x16A467C, void*, CHudSonicStageRemoveCallback2);

}
