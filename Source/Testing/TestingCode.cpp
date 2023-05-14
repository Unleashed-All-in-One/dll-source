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
DWORD* storySequence;
__int64 mystery;

DWORD LODWORD(__int64 x) 
{
	return x & 0xFFFFFFFF;
}
int sonic;
bool were = false;
HOOK(void*, __fastcall, TestingCode_UpdateApplication, 0xE7BED0, Sonic::CGameObject* This, void* Edx, float elapsedTime, uint8_t a3)
{
	auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
	if (serviceGameplay)
	{
		DebugDrawText::log(std::to_string(serviceGameplay[1]).c_str(), 0);
		serviceGameplay[1] = sonic;
		if (inputPtr->IsTapped(Sonic::eKeyState_DpadLeft))
			sonic -= 1;
		else if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
			sonic += 1;
		//this'll be the final boss sonics
	}

	if (inputPtr->IsTapped(Sonic::eKeyState_DpadDown) && !were)
	{
		serviceScriptman = GetServiceScriptMan(Sonic::CApplicationDocument::GetInstance());
		serviceGameplay = GetServiceGameplay(Sonic::CApplicationDocument::GetInstance());

		/*sonic->m_pPlayer->m_spCharacterModel = nullptr;
		sonic->m_pPlayer->m_spCharacterModel =
			boost::make_shared<hh::mr::CSingleElement>(hh::mr::CMirageDatabaseWrapper(spDatabase.get()).GetModelData("chr_Sonic_HD_werehog"));
		sonic->m_pPlayer->AddRenderable("Object", sonic->m_pPlayer->m_spCharacterModel, false);*/
		/*FUNCTION_PTR(void, __fastcall, RequestChange, 0x00548290, Hedgehog::Universe::CMessageActor * This);
		RequestChange(This);*/
		/*auto luaread = (*(int(__thiscall**)(DWORD*))(*serviceScriptman + 16))(serviceScriptman);
		__int64 holder;
		DWORD* v92[2];
		(*(void(__thiscall**)(int, const char*, __int64*, DWORD**))(*(DWORD*)luaread + 12))(
			luaread,
			"PlayEvent",
			&holder,
			&v92[1]);
		if ((DWORD)holder)
		{
			if ((holder & 1) == 0)
			{
				auto v6 = *(void(__cdecl**)(__int64*, __int64*, int))(holder & 0xFFFFFFFE);
				if (v6)
					v6(&storySequence, &v88, 2);
			}
		}*/
	}
	return originalTestingCode_UpdateApplication(This, Edx, elapsedTime, a3);
}
HOOK(void*, __cdecl, LoadLuanneData, 0x6D93E0, const char*& name, const uint8_t* data, const size_t length, void* a4)
{
	

	return originalLoadLuanneData(name, data, length, a4);
}
void TestingCode::applyPatches()
{	
	INSTALL_HOOK(TestingCode_UpdateApplication);
}
