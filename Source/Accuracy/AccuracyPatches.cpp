#include "AccuracyPatches.h"
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
const char* latestNameFloor;
const char* GetNewName(const char* in_ModelName)
{
	auto gameParameter = Sonic::CApplicationDocument::GetInstance()->m_pMember->m_spGameParameter;
	std::string stageIDName = std::string(gameParameter->m_pStageParameter->TerrainArchiveName.c_str());
	std::string modelNameString = std::string(in_ModelName);

	//Get stage name with everything before the underscore
	stageIDName = splitStr2(stageIDName, '_');

	modelNameString = replace(modelNameString, "ghz", stageIDName);
	modelNameString = replace(modelNameString, "cpz", stageIDName);
	modelNameString = replace(modelNameString, "ssz", stageIDName);
	modelNameString = replace(modelNameString, "sph", stageIDName);
	modelNameString = replace(modelNameString, "cte", stageIDName);
	modelNameString = replace(modelNameString, "ssh", stageIDName);
	modelNameString = replace(modelNameString, "csc", stageIDName);
	modelNameString = replace(modelNameString, "euc", stageIDName);
	modelNameString = replace(modelNameString, "pla", stageIDName);
	printf((modelNameString + "\n").c_str());
	latestNameFloor = _strdup(modelNameString.c_str());
	return latestNameFloor;
}
void __declspec(naked) ASM_ReplaceGeneralFloorModelName()
{
	static uint32_t pAddr = 0x01008C9D;
	static uint32_t Csharedstring = 0x006621A0;
	__asm
	{
		push[edi]
		call GetNewName
		mov[edi], eax
		lea     ecx, [esp + 544h - 530h]; Load Effective Address
		call[Csharedstring]
		jmp[pAddr]
	}
}

//FUNCTION_PTR(int, __stdcall, AddEventColWrap,0x00D5E2A0,int a1, const char* symbol, int havokShape, int staticNum, Sonic::CMatrixNodeTransform* a5, volatile signed __int32* a6);
//void TestFloorFunc(Sonic::CGameObject3D *a1, const char* symbol, hk2010_2_0::hkpShape* havokShape, int staticNum, Sonic::CMatrixNodeTransform* a5, volatile signed __int32* a6)
//{
//	if(wrapper2)
//}
//void __declspec(naked) ASM_Test1()
//{
//	static uint32_t pAddr = 0x01008C9D;
//	static uint32_t Csharedstring = 0x006621A0;
//	__asm
//	{
//		call    hkpBoxShape; Call Procedure
//		mov     esi, eax
//		mov     eax, dword_1E0AFC0
//		push    eax
//		lea     ecx, [esp + 14Ch + var_130]; Load Effective Address
//		push    ecx
//		lea     edx, [esp + 150h + var_E0]; Load Effective Address
//		push    edx
//		push    ebx
//		mov[esp + 158h + var_130], esi
//		call    AddEventColWrap
//	}
//}
//
////int __thiscall sub_100A9B0(int this, _DWORD *a2)
//HOOK(int, __fastcall, GeneralFloor_InitializeEditParam, 0x100A9B0, int This, void* Edx, Sonic::CEditParam* in_Param)
//{
//	
//	return originalGeneralFloor_InitializeEditParam(This, Edx, in_Param);
//}
void AccuracyPatches::applyPatches()
{
	//WRITE_JUMP(0x0100DFC5, ASM_Test1);
	//INSTALL_HOOK(GeneralFloor_InitializeEditParam);
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