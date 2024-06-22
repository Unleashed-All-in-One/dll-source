DWORD* SetManager;

inline FUNCTION_PTR(char, __thiscall, SetObjectManager_ProcessMessageF, 0x00EB3BE0, DWORD* This, Hedgehog::Universe::Message&, int flag);
//uint32_t __thiscall sub_EB2A80(DWORD *this, int a2, int a3, int a4)
HOOK(char, __fastcall, SetObjectManager_ProcessMessage, 0x00EB3BE0, DWORD* This, void* Edx, Hedgehog::Universe::Message& message, int flag)
{
	SetManager = This;
	return originalSetObjectManager_ProcessMessage(This, Edx, message, flag);
}
void SetEditorTest::remakeAllObjects()
{	
	Sonic::Message::MsgRemakeAllSetObject msg = Sonic::Message::MsgRemakeAllSetObject();
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::activateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgActivateLayer msg = Sonic::Message::MsgActivateLayer("base", true);
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
void SetEditorTest::deactivateLayer(const char* m_SetLayerName)
{
	Sonic::Message::MsgDeactivateLayer msg = Sonic::Message::MsgDeactivateLayer("base");
	SetObjectManager_ProcessMessageF(SetManager, msg, 0);
}
bool showSetTest = false;
std::string nameLayer;
void SetEditorTest::draw()
{
	ImGui::SetNextWindowBgAlpha(1);
	ImGuiWindowFlags flags = ImGuiWindowFlags_MenuBar;

	if (GetAsyncKeyState(VK_F2) & 1)
		showSetTest ^= true;
	if (SetManager == nullptr)
		showSetTest = false;
	if(showSetTest)
	{
		if (ImGui::Begin("SetEditorTest", &showSetTest, flags))
		{
			if (ImGui::Button("MsgRemakeAllSetObject"))
			{
				SetEditorTest::remakeAllObjects();
			}
			if (ImGui::Button("MsgActivateLayer (base)"))
			{
				SetEditorTest::activateLayer("");
			}
			if (ImGui::Button("MsgDeactivateLayer (base)"))
			{
				SetEditorTest::deactivateLayer("");
			}
		}
		ImGui::End();
	}
}
int numb;
struct MsgStartSetEditor : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0X0167EEDC);
};
struct MsgDebugDrawPath : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0X0167EF04);
};
struct struct_dword4_1
{
	boost::shared_ptr<void> dword0;
	boost::shared_ptr<void> dword4;
	boost::shared_ptr<void> dword8;
};
struct SSetLayerInfo
{
	Hedgehog::Base::CSharedString m_LayerName;
	Hedgehog::Base::CSharedString m_FileName;
	Hedgehog::Base::CSharedString m_SaveLocation;
	float dwordC;
	void* dword10;
	void* byte14;
	void* byte15;
	void* dword18;
};
struct SetLayerManagerTest
{
	BYTE gap0[4];
	hh::map <uint8_t, boost::shared_ptr<SSetLayerInfo>>* m_mSetLayers;
	DWORD* dword8;
	DWORD* dwordC;
	BYTE gap10[4];
	void* dword14;
	void* dword18;
	void* dword1C;
	void* dword20;
	void* dword24;
};

bool init = false;
HOOK(void*, __fastcall, SetUpdateApplication, 0xE7BED0, void* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (GetAsyncKeyState(VK_F10) && !init)
	{
		init = true;
		//auto ptr = (uint32_t*)(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember);
		//auto e = *(void**)((ptr + 16)+4);
		//int size = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList.size();
		//auto size2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_spSetObjectManager->m_pMember->test;
		////std::vector<int> t2 = std::vector(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->list.begin(), Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->list.end());
		//auto sub = (uint32_t*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_spSetObjectManager->m_pMember;
		//auto test1 = (uint32_t*)(sub + 6);
		//auto test2 = *(DWORD*)(test1 + 4);
		//auto test3 = *(DWORD*)(test2 + 4);
		////xstd::vector<uint32_t> vec1 = std::vector(size2->begin(), size2->end());
		////for (size_t i = 0; i < vec1.size(); i++)
		////{
		////	auto fff = vec1.at(i);
		////}
		////for (size_t i = 0; i < size; i++)
		////{
		////	
		////	DebugDrawText::log(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList[i].m_Name.c_str());
		////	Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetObjectFactory->m_MakeList[i].m_pMake->pFunc()->m_spSetObjectInfo->spSetObjectListener;
		////	printf("");
		////}
		auto t2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_Objects;
		t2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_Objects2.get();
		t2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_Objects4.get(); std::vector<boost::shared_ptr<Sonic::CGameObject>> vec1 = std::vector(Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin(), Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.end());

		SetLayerManagerTest* t = (SetLayerManagerTest*)Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager.get();
		auto aaf = t->m_mSetLayers->size();
		

		for (auto it = t->m_mSetLayers->begin(); it != t->m_mSetLayers->end(); ++it)
		{
			auto one = it->first;
			auto two = it->second;
			auto three = it->second.get();
			DebugDrawText::log(std::format("LAYER {0}\nName: {1}\nFilename: {2}\nSaving Location: {3}",one, three->m_LayerName.c_str(), three->m_FileName.c_str(), three->m_SaveLocation.c_str()).c_str());
		}
		//for (std::set<boost::shared_ptr<void*>>::iterator it = t->dword4.begin(); it != t->dword4.end(); it++) {
		//	//^^                   //^^
		//	SetLayer* fa = (SetLayer*)it->get();
		//	printf("");
		//}
		for (size_t i = 0; i < Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.size(); i++)
		{
			void* vptrA = nullptr;
			try
			{
				vptrA = *((void**)vec1.at(i).get());
			}
			catch(std::exception& e)
			{
				continue;
			}
			vec1.at(i)->SendMessageImm(vec1.at(i)->m_ActorID, boost::make_shared<MsgDebugDrawPath>());

			if (dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get()) != nullptr)
			{
				//init tool preview
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->CSetObjectListener20();
			}
		}
		//auto t = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test;
		/*int size2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.size();
		auto ssize2 = Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test;
		std::vector<Hedgehog::base::CSharedString> test = std::vector(Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.begin(), Sonic::CGameDocument::GetInstance()->m_pGameActParameter->m_pSetObjectManager->m_pMember->m_spSetLayerManager->test->vector.end());
		for (size_t i = 0; i < size2; i++)
		{
			
			DebugDrawText::log(test.at(i).c_str());
			printf("");
		}*/
		//auto t = ->Test;
		MsgStartSetEditor msg = MsgStartSetEditor();
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	if (GetAsyncKeyState(VK_F11) && init)
	{
		init = false;

		std::vector<boost::shared_ptr<Sonic::CGameObject>> vec1 = std::vector(Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.begin(), Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.end());

		for (size_t i = 0; i < Sonic::CGameDocument::GetInstance()->m_pMember->m_GameObjects.size(); i++)
		{
			void* vptrA = nullptr;
			try
			{
				vptrA = *((void**)vec1.at(i).get());
			}
			catch (std::exception& e)
			{
				continue;
			}
			vec1.at(i)->SendMessageImm(vec1.at(i)->m_ActorID, boost::make_shared<MsgDebugDrawPath>());

			if (dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get()) != nullptr)
			{
				//kill tool preview
				dynamic_cast<Sonic::CSetObjectListener*>(vec1.at(i).get())->CSetObjectListener24();
			}
		}
		
		MsgStartSetEditor msg = MsgStartSetEditor();
		SetObjectManager_ProcessMessageF(SetManager, msg, 0);
	}
	return originalSetUpdateApplication(This, Edx, elapsedTime, a3);
}

//int *__stdcall sub_EA67D0(int a1, int layerIndex, char in_Toggle)
HOOK(int*, __stdcall, sub_EA67D0, 0xEA67D0, void* a1, int layerIndex, char in_Toggle)
{
	void* vftable = *(void**)a1;
	return originalsub_EA67D0(a1, layerIndex, in_Toggle);
}
void SetEditorTest::applyPatches()
{
	WRITE_JUMP(0x00FD6AB3, 0x00FD6ABF);
	//WRITE_JUMP(0x00EB681D, ASMTEST);
	//INSTALL_HOOK(ADD_SET_OBJECTS_TO_WORLD);
	INSTALL_HOOK(sub_EA67D0);
	INSTALL_HOOK(SetUpdateApplication);
	INSTALL_HOOK(SetObjectManager_ProcessMessage);
}