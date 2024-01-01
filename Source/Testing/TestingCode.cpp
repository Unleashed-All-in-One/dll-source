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

		AddRenderable("Object", m_spRenderable, true);
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
//		sonic->m_pPlayer->AddRenderable("Object", sonic->m_pPlayer->m_spCharacterModel, false);*/
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
void TestingCode::applyPatches()
{	

	INSTALL_HOOK(A51CD0);
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
