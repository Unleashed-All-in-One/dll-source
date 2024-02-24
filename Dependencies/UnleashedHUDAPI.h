#pragma once
#include "APIHelper.h"
enum UnleashedHUD_ResultRankType : int
{
	HUD_E = -1,
	HUD_D,
	HUD_C,
	HUD_B,
	HUD_A,
	HUD_S
};
struct UnleashedHUD_ResultData
{
	int m_score;
	UnleashedHUD_ResultRankType m_rank;
	UnleashedHUD_ResultRankType m_perfectRank;
	int m_nextRankTime;
	float m_totalProp;	// result progress bar (time prop + ring prop) 
	float m_timeProp;	// result progress bar (time prop)
};
class UnleashedHUD_API
{
public:

private:
	LIB_FUNCTION(void, "UnleashedHUD.dll", API_AddRingScore, int score);
	LIB_FUNCTION(void, "UnleashedHUD.dll", API_AddSpeedScore, int score);
	LIB_FUNCTION(void, "UnleashedHUD.dll", API_AddEnemyScore, int score);
	LIB_FUNCTION(void, "UnleashedHUD.dll", API_AddTrickScore, int score);
	LIB_FUNCTION(bool, "UnleashedHUD.dll", API_IsLoadingFadeoutCompleted);
	LIB_FUNCTION(UnleashedHUD_ResultData, "UnleashedHUD.dll", API_GetStageResultData);
	LIB_FUNCTION(void, "UnleashedHUD.dll", API_StartFadeOut);

public:
    static UnleashedHUD_API* GetInstance()
    {
        static UnleashedHUD_API* instance = nullptr;
        return instance != nullptr ? instance : instance = new UnleashedHUD_API();
    }

    static void AddRingScore(int score)
	{
		VOID_EXPORT(API_AddRingScore, score);
	}
	static bool IsLoadingFadeoutCompleted()
	{
		BOOL_EXPORT(API_IsLoadingFadeoutCompleted);
	}
	static void StartFadeout()
	{
		VOID_EXPORT(API_StartFadeOut);
	}

    static void AddSpeedScore(int score)
	{
		VOID_EXPORT(API_AddSpeedScore, score);
	}

    static void AddEnemyScore(int score)
	{
		VOID_EXPORT(API_AddEnemyScore, score);
	}

    static void AddTrickScore(int score)
	{
		VOID_EXPORT(API_AddTrickScore, score);
	}
	static UnleashedHUD_ResultData GetStageResultData()
	{
		GENERIC_EXPORT(UnleashedHUD_ResultData, API_GetStageResultData);
	}
};
