#include "TestingCode.h"
#include "..\UI\TitleWorldMap.h"

class CBasicSphere : public Sonic::CGameObject3D
{
public:
	boost::shared_ptr<hh::mr::CSingleElement> m_spRenderable;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

	virtual void AddCallback(const hh::base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<hh::db::CDatabase>& spDatabase) override
	{
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("0", this); // Worth looking into which updateuints do what, by the way.

		hh::mr::CMirageDatabaseWrapper wrapper(spDatabase.get());
		// This is a debug asset that has a broken material, so it will be pure red--but that's ok, cuz we can see it.
		const char* assetName = "BasicSphere";
		boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);

		m_spRenderable = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		// Safeguard from crashing (last I checked this works, could crash if it doesn't exist anyway.)
		if (!spModelData)
			return;

		Sonic::CGameObject::AddRenderable("Object", m_spRenderable, true);
	}
};
DWORD* GetServiceScriptMan(Hedgehog::Base::TSynchronizedPtr<Sonic::CApplicationDocument> doc)
{
	uint32_t func = 0x0041E6F0;
	DWORD* result;
	__asm
	{		
		mov     edi, doc
		add     edi, 34h
		call func
		mov     result, eax
	};
};
DWORD* GetServiceGameplay(Hedgehog::Base::TSynchronizedPtr<Sonic::CApplicationDocument> doc)
{
	uint32_t func = 0x0040EBD0;
	DWORD* result;
	__asm
	{		
		mov     edi, doc
		add     edi, 34h
		call func
		mov     result, eax
	};
};
DWORD* serviceScriptman;
DWORD* serviceGameplay;
__int64 mystery;
//
struct MsgRequestChangeModule : public hh::fnd::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x0168120C);

	size_t m_PlayerType;

	MsgRequestChangeModule(size_t playerType) : m_PlayerType(playerType) {}
};
struct MsgRequestEndModule : public hh::fnd::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01681224);

};
struct MsgRequestStartLoading : public hh::fnd::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x016813F0);

	size_t m_PlayerType;

	MsgRequestStartLoading(size_t playerType) : m_PlayerType(playerType) {}
};
DWORD LODWORD(__int64 x) 
{
	return x & 0xFFFFFFFF;
}
DWORD* storyseq;
//int sonic;
//bool were = false;
//void OpenStage(DWORD stageID)
//{
//	FUNCTION_PTR(void, __thiscall, Hedgehog_Base_CHolderBase___ct, 0x0065FBE0, Hedgehog::Base::CHolderBase * This, void* synchronizedObject, char forceSync);
//	//void __thiscall (void *this)
//	FUNCTION_PTR(void, __thiscall, Hedgehog_Base_CHolderBase___dt, 0x0065FC40, void* This);
//	//int  sub_552130( a1)
//	FUNCTION_PTR(int, __fastcall, sub_552130, 0x552130, __int16 a1);
//	Hedgehog::base::CHolderBase v6 = Hedgehog::base::CHolderBase();
//	Hedgehog_Base_CHolderBase___ct(&v6, (void*)0x01E66B34, 0);
//	int v4 = *((DWORD*)v6.get() + 1) + 436;
//	Hedgehog_Base_CHolderBase___dt(&v6);
//	unsigned int result = (unsigned __int8)stageID;
//	if ((unsigned __int8)stageID <= 0x1Au)
//	{
//		result = sub_552130(stageID);
//		if (result < 0x8E)
//		{
//			result = *(DWORD*)(*(DWORD*)v4 + 124) + 288 * result + 28;
//			if (result)
//				*(DWORD*)(result + 284) |= 1u;
//		}
//	}
//}
//HOOK(void*, __fastcall, TestingCode_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
//{
//	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
//	if (serviceGameplay)
//	{
//		DebugDrawText::log(std::to_string(serviceGameplay[1]).c_str(), 0);
//		serviceGameplay[1] = sonic;
//		if (inputPtr->IsTapped(Sonic::eKeyState_DpadLeft))
//			sonic -= 1;
//		else if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
//			sonic += 1;
//		//this'll be the final boss sonics
//	}
//
//	if (inputPtr->IsTapped(Sonic::eKeyState_DpadDown) && !were)
//	{
//		auto g = boost::make_shared<CBasicSphere>();
//		g->SendMessage("Player", boost::make_shared<MsgRequestChangeModule>(5));
//		g->SendMessage("Player", boost::make_shared<MsgRequestStartLoading>(5));
//		serviceScriptman = GetServiceScriptMan(Sonic::CApplicationDocument::GetInstance());
//		serviceGameplay = GetServiceGameplay(Sonic::CApplicationDocument::GetInstance());
//		//(void, __fastcall, sub_D71A90, 0x00D71A90, BYTE* This, void*Edx,int a2, Hedgehog::Base::CSharedString* stage)
//
//		/*auto p = Hedgehog::Base::CSharedString("pam000");
//		Hedgehog::Base::CSharedString* pam = &p;
//		FUNCTION_PTR(void, __thiscall, SetupStage, 0x00D71A90, DWORD* storysequence, int dummy, const Hedgehog::base::CSharedString & stage);
//		SetupStage(storyseq, 0, "pam000");*/
//		//unsigned int, __stdcall, sub_D71950, 0xD71950, int a1, DWORD* a2)
//		/*FUNCTION_PTR(unsigned int, __stdcall, sub_D71950, 0xD71950, int a1, DWORD * a2);
//		sub_D71950(0, storyseq);*/
//
//		OpenStage(0);
//
//
//		/*sonic->m_pPlayer->m_spCharacterModel = nullptr;
//		sonic->m_pPlayer->m_spCharacterModel =
//			boost::make_shared<hh::mr::CSingleElement>(hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("chr_Sonic_HD_werehog"));
//		sonic->m_pPlayer->Sonic::CGameObject::AddRenderable("Object", sonic->m_pPlayer->m_spCharacterModel, false);*/
//		/*FUNCTION_PTR(void, __fastcall, RequestChange, 0x00548290, Hedgehog::Universe::CMessageActor * This);
//		RequestChange(This);*/
//		/*auto luaread = (*(int(__thiscall**)(DWORD*))(*serviceScriptman + 16))(serviceScriptman);
//		__int64 holder;
//		DWORD* v92[2];
//		(*(void(__thiscall**)(int, const char*, __int64*, DWORD**))(*(DWORD*)luaread + 12))(
//			luaread,
//			"PlayEvent",
//			&holder,
//			&v92[1]);
//		if ((DWORD)holder)
//		{
//			if ((holder & 1) == 0)
//			{
//				auto v6 = *(void(__cdecl**)(__int64*, __int64*, int))(holder & 0xFFFFFFFE);
//				if (v6)
//					v6(&storySequence, &v88, 2);
//			}
//		}*/
//	}
//	return originalTestingCode_UpdateApplication(This, Edx, elapsedTime, a3);
//}
//
////__thiscall sub_D77020()
//HOOK(int*, __fastcall, sub_D77020, 0xD77020, void* This,void* Edx, const char* a2, const char* a3)
//{
//	return originalsub_D77020(This, Edx, a2, a3);
//}
////Sonic::Sequence::CStoryImpl *__stdcall Sonic::Sequence::CStoryImpl(Sonic::Sequence::CStoryImpl *a1
//HOOK(DWORD*, __stdcall, StoryImpl, 0x00D74560, DWORD* storySeq)
//{
//	auto retur = originalStoryImpl(storySeq);
//
//	storyseq = storySeq;
//	return retur;
//}
//HOOK(void, __fastcall, sub_D71A90, 0x00D71A90, BYTE* This, void*Edx,int a2, const Hedgehog::base::CSharedString& stage)
//{
//	originalsub_D71A90(This, Edx, a2, stage);
//}
//// __stdcall 
//HOOK(unsigned int, __stdcall, sub_D71950, 0xD71950, int a1, DWORD* a2)
//{
//	auto v2 = (DWORD*)a2[1];
//	auto v3 = v2[1];
//	return originalsub_D71950(a1,a2);
//}
//HOOK(void*, __cdecl, LoadLuanneData, 0x6D93E0, const char*& name, const uint8_t* data, const size_t length, void* a4)
//{
//	
//
//	return originalLoadLuanneData(name, data, length, a4);
//}
//void __thiscall sub_64B930(Hedgehog::Base::CSharedString **this, int a2, int a3, int a4, int a5, int a6, void *a7, int a8, int a9, char a10)

HOOK(void, __fastcall, sub_64B930, 0x64B930, Hedgehog::Base::CSharedString** This,void* Edx, int a2, int a3, int a4, int a5, int a6, void* a7, int a8, int a9, char a10)
{
	printf("Loading PFD");
	originalsub_64B930(This,Edx,a2,a3,a4,a5,a6,a7,a8,a9,a10);
	printf("Done PFD");
}
HOOK(void, __fastcall, sub_D51A10, 0xD51A10, void* obj)
{
	printf("Loading PFD");
	originalsub_D51A10(obj);
	printf("Done PFD");
}
//int __cdecl sub_6D9B20(int a1)
//unsigned int __stdcall OpenStage(int a1, DWORD *a2)
HOOK(unsigned int, __stdcall, OpenStage, 0x00D71950, int a1, const char* a2)
{
	printf("Print string?");
	return originalOpenStage(a1, a2);
}
HOOK(void, __cdecl, A51CD0, 0xA51CD0, const char* text)
{
	std::string newText = (std::string("[BB_DEBUG] ") + std::string(text) + std::string("\n"));
	printf(newText.c_str());
}
HOOK(void, __fastcall, sub_6A1D40, 0x6A1D40, void* obj)
{
	printf("Loading PFD");
	try
	{

		originalsub_6A1D40(obj);
	}
	catch (const std::exception& ex)
	{

		printf("Couldnt load PFD");
	}
	printf("Done PFD");
}
//int __thiscall sub_E8F330(void *this, int a2, int a3, int a4)

HOOK(int, __fastcall, sub_E8F330, 0xE8F330, void* This, void* Edx, int a2, int a3, int a4)
{
	return originalsub_E8F330(This, Edx, a2, a3, a4);
}
//int __thiscall sub_42A710(void *this)
//0042ABA0
HOOK(void, __fastcall, sub_42A710, 0x0042ABA0, void* This, void* Edx)
{

	//int *__thiscall Sonic::Sequence::CSequenceMainImpl::LuanneFunctions::StartModule(DWORD *this, int a2, Luanne_IntegerMessageContainer *a3)
	/*FUNCTION_PTR(int*, __thiscall, StartModule, 0x00D77020, Hedgehog::Universe::CMessageActor* This, int a2, LuaStringEntryContainer * a3);
	StartModule(Sonic::Sequence::Main::GetInstance(), 0, new LuaStringEntryContainer("Title"));*/
	//SequenceHelpers::ChangeModule(ModuleFlow::Title);
	DWORD* test = (DWORD*)Sonic::CApplicationDocument::GetInstance()->m_pMember;
	//DWORD *__thiscall ProcMsgRequestChangeModule(Hedgehog::Universe::MessageTypeSet *this, DWORD *a2)
	FUNCTION_PTR(DWORD*, __thiscall, ProcMsg, 0x00D0B2E0, DWORD* T5his, boost::shared_ptr<MsgRequestChangeModule> a2);

	DWORD* test2 = (DWORD*)test[13];
	//ProcMsg(test2, boost::make_shared<MsgRequestChangeModule>(1));
	//void __thiscall sub_D0AE90(Sonic::CAnimationStateMachine *this, int a2
	FUNCTION_PTR(DWORD*, __thiscall, ProcMsg2, 0xD0AE90, DWORD * T5his, MsgRequestEndModule * a2);
	//ProcMsg2(test2, new MsgRequestEndModule());
	SUC::System::SequenceHelpers::ChangeModule(ModuleFlow::Genesis);
	originalsub_42A710(This, Edx);
}
HOOK(int, __fastcall, CHudGateMenuMainIntroInfo, 0x1080110, hh::fnd::CStateMachineBase::CStateBase* This, void* Edx)
{
	////Run function first, otherwise scenes won't be gettable
	auto returne = originalCHudGateMenuMainIntroInfo(This, Edx);
	auto contextBase = static_cast<int*>(This->GetContextBase());
	auto stageID = (*(uint8_t*)((int)contextBase + 336));
	int index = stageID == 0 ? stageID : stageID + 1;
	auto idArchive = (const char*)((int*)0x015BBA34)[stageID];

	Common::ClampUInt(stageID, 0, SUC::Project::s_WorldData.data[SUC::UI::TitleScreen::TitleWorldMap::s_LastFlagSelected].data.size() - 1);
	SUC::System::StageManager::s_NextStage = SUC::Project::s_WorldData.data[SUC::UI::TitleScreen::TitleWorldMap::s_LastFlagSelected].data[stageID].levelID.
	                                                                                                                                                 c_str();
	return returne;
}

HOOK(char, *_stdcall, StageGate_MoveToOtherStage, 0x107FBC0, int a1)
{
	char* returned = originalStageGate_MoveToOtherStage(a1);
	uint8_t stageID = (*(uint8_t*)(a1 + 336));
	SUC::System::StageManager::s_NextStage = SUC::Project::s_WorldData.data[SUC::UI::TitleScreen::TitleWorldMap::s_LastFlagSelected].data[stageID].levelID.
	                                                                                                                                                 c_str();
	return returned;
}

HOOK(int, __fastcall, CHudGateMenuMainOutro, 0x107B770, hh::fnd::CStateMachineBase::CStateBase* This)
{
	return originalCHudGateMenuMainOutro(This);
}
HOOK(int, __cdecl, sub_7C931F, 0x7C931F, int a1, const char* a2)
{
	printf("[CRIWARE Audio 1] %s\n", a2);
	return originalsub_7C931F(a1, a2);
}

HOOK(int, __fastcall, sub_7E27B0, 0x7E27B0, char* a1, ...)
{
	printf("[CRIWARE Audio 2] %s\n", a1);
	return originalsub_7E27B0(a1);
}

void TestingCode::applyPatches()
{		
	//INSTALL_HOOK(sub_7C931F);
	INSTALL_HOOK(sub_7E27B0);
	INSTALL_HOOK(sub_E8F330);
	INSTALL_HOOK(A51CD0);
	//
	//INSTALL_HOOK(sub_42A710);
	//INSTALL_HOOK(OpenStage);
	//INSTALL_HOOK(sub_64B930);
	////INSTALL_HOOK(sub_D51A10);
	//INSTALL_HOOK(sub_6A1D40);
	/*INSTALL_HOOK(TestingCode_UpdateApplication);
	INSTALL_HOOK(sub_D77020);
	INSTALL_HOOK(sub_D71950);
	INSTALL_HOOK(sub_D71A90);
	INSTALL_HOOK(StoryImpl);*/
}
