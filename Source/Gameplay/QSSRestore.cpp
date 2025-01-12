#include "QSSRestore.h"
#include "../Project.h"

namespace SUC::Gameplay
{
	void QSSRestore_IncreaseSpeed(char const* targetState)
	{
		auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();
		auto stateName = context->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName();
		if (std::string(stateName.c_str()) == std::string(targetState))
		{			
			context->SetVelocity((context->m_Velocity.norm() + 12.0f) * context->m_Velocity.normalized());			
		}
	}
	HOOK(void, __fastcall, QSSRestore_CSonicStateSlidingAdvance, 0x11D69A0, int This)
	{
		originalQSSRestore_CSonicStateSlidingAdvance(This);
		QSSRestore_IncreaseSpeed("RunQuickStep");
	}
	HOOK(void, __fastcall, QSSRestore_CSonicStateStompingAdvance, 0x12548C0, int This)
	{
		originalQSSRestore_CSonicStateStompingAdvance(This);
		QSSRestore_IncreaseSpeed("Sliding");
	}
	void QSSRestore::RegisterHooks()
	{
		if (!SUC::Project::m_DoQSS)
		{
			return;
		}
		INSTALL_HOOK(QSSRestore_CSonicStateSlidingAdvance);
		INSTALL_HOOK(QSSRestore_CSonicStateStompingAdvance);
	}
}
