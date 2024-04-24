
//void __thiscall sub_E9FD10(char *this, int a2, int a3, int a4)
HOOK(void, __fastcall, sub_E9FD10, 0xE9FD10, char* This, void* Edx, int a2, int a3, int a4)
{
	originalsub_E9FD10(This, Edx, a2, a3, a4);
	if (!Sonic::Player::CSonicClassicContext::GetInstance())
	{
		int second = (int)(This + 244);
		auto v5 = *(void**)(second + 4);
		auto v6 = (void*)*((DWORD*)v5 + 1);
		Hedgehog::Base::CSharedString* string = (Hedgehog::Base::CSharedString*)v6 + 3;
		*string = "Contact";
	}
}
void Patches::applyPatches()
{
	INSTALL_HOOK(sub_E9FD10);
    //Makes Sonic's result animations work again for modern
    WRITE_JUMP(0X00CFDBD5, 0x00CFD840);

	//Patch out a weird check for the results music
	WRITE_JUMP(0x00CFD472, 0x00CFD492);
}