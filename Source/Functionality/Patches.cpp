HOOK(void, __fastcall, ObjectPhysics_SetDamageType, 0xE9FD10, char* This, void* Edx, int a2, int a3, int a4)
{
	originalObjectPhysics_SetDamageType(This, Edx, a2, a3, a4);
	if (!Sonic::Player::CSonicClassicContext::GetInstance())
	{
		int second = (int)(This + 244);
		auto v5 = *(void**)(second + 4);
		auto v6 = (void*)*((DWORD*)v5 + 1);
		Hedgehog::Base::CSharedString* damageType = (Hedgehog::Base::CSharedString*)v6 + 3;
		*damageType = "Contact";
	}
}
HOOK(void, __fastcall, Sonic_CObjIronPole_UpdateSerial, 0xFFF100, Sonic::CGameObject3D* This, void* Edx, const struct Hedgehog::Universe::SUpdateInfo* a2)
{
	if (Sonic::Player::CSonicClassicContext::GetInstance() == nullptr)
	{
		originalSonic_CObjIronPole_UpdateSerial(This, Edx, a2);
		return;
	}
	TransformUtilities::DoWerehogArmHomingIfClose(This->m_spMatrixNodeTransform->m_Transform.m_Position, 5, a2->DeltaTime);
	originalSonic_CObjIronPole_UpdateSerial(This, Edx, a2);
}
void Patches::applyPatches()
{
	//Make IronPole swingable for the werehog
	INSTALL_HOOK(Sonic_CObjIronPole_UpdateSerial);

	//Set all ObjectPhysics to be the same type for the Werehog
	INSTALL_HOOK(ObjectPhysics_SetDamageType);

    //Makes Sonic's result animations work again for modern
    WRITE_JUMP(0X00CFDBD5, 0x00CFD840);

	//Patch out a weird check for the results music
	WRITE_JUMP(0x00CFD472, 0x00CFD492);

}