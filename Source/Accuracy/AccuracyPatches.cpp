#include "AccuracyPatches.h"
namespace SUC::Accuracy
{
	const char* m_LastFloorName; //This simply exists to make the string stay alive for long enough to get used for the model


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
	HOOK(void, __fastcall, CGameplayFlowStage_StateGoalFadeBefore_Update, 0xCFA5D0, uint32_t* This, void* Edx)
	{
		STAGEACT_FADEWAIT = 0.2f;
		originalCGameplayFlowStage_StateGoalFadeBefore_Update(This, Edx);
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
		m_LastFloorName = _strdup(modelNameString.c_str());
		return m_LastFloorName;
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
	HOOK(void*, __fastcall, hhSoundHandleCtor, 0x762030, hh::Sound::CSoundHandle* This, void* Edx, float a2)
	{
		auto ret = originalhhSoundHandleCtor(This, Edx, a2);
		//Add 15% more echo
		This->SetEcho(This->GetEcho() + 0.05f);
		return ret;
	}
	HOOK(bool, __stdcall, IsStageCompleted, 0x107ADC0, uint32_t stageID)
	{
		//TODO: Implement checking the save file.
		return true;
	}
	//Used by HUDs
	//Returns a ratio of how big the boost bar is. 0 is equal to the beginning of the game, 1 is the full Generations boost bar.
	extern "C" __declspec(dllexport) float API_GetBoostSize()
	{
		//if(System::StageManager::s_InStoryMode)		
			return System::SaveManager::GetCurrentSave(true)->GetSaveFloatKeyValue("RingEnergyRatio");		

		return 1;
	}
	void AccuracyPatches::RegisterHooks()
	{
		//Make IronPole swingable for the werehog
		INSTALL_HOOK(Sonic_CObjIronPole_UpdateSerial);

		//Set all ObjectPhysics to be the same type for the Werehog
		INSTALL_HOOK(ObjectPhysics_SetDamageType);

		//Make Result screen's white fade in the same way as Unleashed
		INSTALL_HOOK(CGameplayFlowStage_StateGoalFadeBefore_Update);

		//Patch calls so that stages always appear as completed according to the SUC save file
		INSTALL_HOOK(IsStageCompleted);

		//Restores Unleashed's reverb/echo for sounds.
		INSTALL_HOOK(hhSoundHandleCtor);

		//Makes Sonic's result animations work again for modern
		WRITE_JUMP(0X00CFDBD5, 0x00CFD840);

		//Patch out a weird check for the results music
		WRITE_JUMP(0x00CFD472, 0x00CFD492);

		//Rename GeneralFloor models to use Conversion stage IDs
		WRITE_JUMP(0x01008C91, ASM_ReplaceGeneralFloorModelName);

		//Get rid of mission icons and pamSettings permanently (in theory)
		WRITE_JUMP(0x0107EAF8, 0x0107EB8F);
	}
}