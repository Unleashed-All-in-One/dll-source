SaveObject* SaveManager::save;

boost::shared_ptr<SaveLoadTestStruct> m_spSaveT;

FUNCTION_PTR(void, __thiscall, SaveManager_CTitleOptionCStateOutroSaving, 0xD22A70, boost::shared_ptr<SaveLoadTestStruct>& spSave, void* a2);

void SaveManager::GenerationsSave()
{
	SaveManager_CTitleOptionCStateOutroSaving(m_spSaveT, nullptr);
}
HOOK(bool, __fastcall, CSaveLoadTestPC_SaveContentsRead, 0x00E7A220, void* ecx, void* edx, void* buffer, size_t bufsize)
{
	DebugDrawText::log("CSaveLoadTestPC_SaveContentsRead", 5);
	return SaveManager::loadFromDisk();
}
HOOK(bool, __fastcall, CSaveLoadTestPC_SaveContentsDelete, 0x00E7A1F0, void* ecx)
{
	DebugDrawText::log("CSaveLoadTestPC_SaveContentsDelete", 5);
	return SaveManager::deleteSave();
}
HOOK(bool, __fastcall, CSaveLoadTestPC_SaveContentsSave, 0x00E7A320, void* ecx, void* edx, const void* buffer, size_t bufsize)
{
	DebugDrawText::log("CSaveLoadTestPC_SaveContentsSave", 5);
	return SaveManager::saveToDisk();
}
HOOK(bool, __fastcall, CSaveLoadTestPC_SaveContentsExist, 0x00E7A3D0, void* ecx)
{
	DebugDrawText::log("CSaveLoadTestPC_SaveContentsExist", 5);
	return SaveManager::getCurrentSave() != nullptr;
}
HOOK(void*, __fastcall, SaveManager_InitializePlayer, 0x00D96110, void* This)
{
	void* result = originalSaveManager_InitializePlayer(This);
	uint32_t* livesCount = (uint32_t*)Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
	*livesCount = SaveManager::getCurrentSave()->lives;
	return result;
}
HOOK(void, __fastcall, SaveManager_CGameplayFlowStage_CStateGoalBegin, 0xCFD550, void* This)
{
	try
	{
		uint32_t stageTerrainAddress = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x80, 0x20 });
		char** pointerTerrainID = (char**)stageTerrainAddress;
		std::string stageID = std::string(*pointerTerrainID);
		auto saveObj = SaveManager::getCurrentSave();
		DebugDrawText::log("Saving data for stage...", 10);
		const size_t liveCountAddr = Common::GetMultiLevelAddress(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
		saveObj->lives = *(int*)liveCountAddr;
		int stageDataIndex = saveObj->getStageDataIndexFromID(stageID);
		if (stageDataIndex == -1)
		{
			saveObj->stageData.push_back(new StageSaveData());
			stageDataIndex = saveObj->stageData.size() - 1;
			saveObj->stageData[stageDataIndex]->stageID = stageID;
		}
		float score = 0;
		if (GetModuleHandle(TEXT("UnleashedHUD.dll")))
		{
			auto stageDataUHUD = UnleashedHUD_API::GetInstance()->GetStageResultData();
			saveObj->stageData[stageDataIndex]->rank = (StageRank)(int)stageDataUHUD.m_rank;
			score = stageDataUHUD.m_score;
		}
		else if (GetModuleHandle(TEXT("ScoreGenerations.dll")))
		{
			score = ScoreGenerationsAPI::GetScore();
			saveObj->stageData[stageDataIndex]->rank = (StageRank)ScoreGenerationsAPI::GetRank();
		}
		if (ScoreGenerationsAPI::IsAttached())
			saveObj->stageData[stageDataIndex]->timeInSeconds = ScoreGenerationsAPI::GetStatistics().seconds;
		saveObj->stageData[stageDataIndex]->score = score;
		SaveManager::saveToDisk();
		// Force saving at result screen
		SaveManager_CTitleOptionCStateOutroSaving(m_spSaveT, nullptr);
	}
	catch (std::exception& e)
	{

	}

	originalSaveManager_CGameplayFlowStage_CStateGoalBegin(This);
}
//bool __cdecl CApplicationSettingImpl::SetEnableSaveLoad(_DWORD *a1)
HOOK(bool, __cdecl, CApplicationSettingImpl_SetEnableSaveLoad, 0x00D0D240, DWORD* a1)
{
	bool* saveLoadEnabled = (bool*)0x01A430D5;
	*saveLoadEnabled = false;
	return false;
}
void SaveManager::applyPatches()
{
	////patch out saving in title options
	//WRITE_JUMP(0x00D22BB5, 0x00D22CF1);
	////patch out common function for loading saves
	//WRITE_JUMP(0x00D5572E, 0x00D5581D);
	//INSTALL_HOOK(CApplicationSettingImpl_SetEnableSaveLoad);
	INSTALL_HOOK(SaveManager_InitializePlayer);
	INSTALL_HOOK(SaveManager_CGameplayFlowStage_CStateGoalBegin);
	INSTALL_HOOK(CSaveLoadTestPC_SaveContentsExist);
	INSTALL_HOOK(CSaveLoadTestPC_SaveContentsSave);
	INSTALL_HOOK(CSaveLoadTestPC_SaveContentsDelete);
	INSTALL_HOOK(CSaveLoadTestPC_SaveContentsRead);
}