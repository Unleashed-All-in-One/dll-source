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
	ObjectUtility::DoWerehogArmHomingIfClose(This->m_spMatrixNodeTransform->m_Transform.m_Position, 5, a2->DeltaTime);
	originalSonic_CObjIronPole_UpdateSerial(This, Edx, a2);
}
std::string replace(const std::string& original, const std::string& from, const std::string& to) {
	if (from.empty()) {
		return original; // Avoid infinite loop when `from` is empty.
	}

	std::string result = original;
	size_t start_pos = 0;

	// Iterate through the string and replace all occurrences of `from` with `to`.
	while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
		result.replace(start_pos, from.length(), to);
		start_pos += to.length(); // Move past the last replaced substring.
	}

	return result;
}
std::string splitStr2(std::string const& s, char separator)
{
	std::string::size_type pos = s.find(separator);
	if (pos != std::string::npos)
	{
		return s.substr(0, pos);
	}
	else
	{
		return s;
	}
}
const char* GetNewName(const char* existing)
{
	uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
	std::string stageIDName = *(char**)stageTerrainAddress;
	std::string name = std::string(existing);
	stageIDName = splitStr2(stageIDName, '_');

	name = replace(name, "ghz", stageIDName);
	name = replace(name, "cpz", stageIDName);
	name = replace(name, "ssz", stageIDName);
	name = replace(name, "sph", stageIDName);
	name = replace(name, "cte", stageIDName);
	name = replace(name, "ssh", stageIDName);
	name = replace(name, "csc", stageIDName);
	name = replace(name, "euc", stageIDName);
	name = replace(name, "pla", stageIDName);
	printf((name + "\n").c_str());
	return _strdup(name.c_str());
}
void __declspec(naked) ASM_ReplaceGeneralFloorModelName()
{
	static uint32_t pAddr = 0x01008C9D;
	static uint32_t Csharedstring = 0x006621A0;
	__asm {
		push[edi]
		call GetNewName
		mov[edi], eax
		lea     ecx, [esp + 544h - 530h]; Load Effective Address
		call[Csharedstring]
		jmp[pAddr]
	}
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

	//Rename GeneralFloor models to use Conversion stage IDs
	WRITE_JUMP(0x01008C91, ASM_ReplaceGeneralFloorModelName);

	//Get rid of mission icons and pamSettings permanently (in theory)
	WRITE_JUMP(0x0107EAF8, 0x0107EB8F);
}