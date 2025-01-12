#include "HudStatus.h"

boost::shared_ptr<Sonic::CGameObject> status;


void HudStatus::Start(Sonic::CGameObject* in_GameObject)
{
	if (!statusNew)
		statusNew = StatusNew::Generate(in_GameObject, false, 0);
	statusNew->Show();
}
float statusTimer;
void HudStatus::End()
{
}

bool HudStatus::CanEnd()
{
	return false;
}

bool HudStatus::EndDelayFinish()
{
	return false;
}

void HudStatus::Kill()
{
	statusNew->Kill();
}
extern "C" __declspec(dllexport) void API_StartStatsUI()
{
	HudStatus::Start(Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer);
}
HOOK(void, __fastcall, Status_CHudSonicStage_Update, 0x1098A50, void* This, void* Edx, float* dt)
{
	if (HudStatus::statusNew)
		HudStatus::statusNew->Update(*dt);
	if(HudStatus::CanEnd())
	{
		static boost::shared_ptr<Hedgehog::Sound::CSoundHandle> soundHandle;
		Sonic::SPadState const* padState = &Sonic::CInputState::GetInstance()->GetPadState();
		if (padState->IsTapped(Sonic::EKeyState::eKeyState_A) && statusTimer >= 2.5f)
		{
			statusTimer = 0.0f;
			//HudStatus::End();
			Common::PlaySoundStatic(soundHandle, 1000027);
			UnleashedHUD_API::StartFadeout();
			
		}		
	}	
	originalStatus_CHudSonicStage_Update(This, Edx, dt);
}

void HudStatus::RegisterHooks()
{
	INSTALL_HOOK(Status_CHudSonicStage_Update);
}