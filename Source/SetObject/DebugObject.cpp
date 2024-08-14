
#include "DebugObject.h"
namespace SUC::SetObject
{
	// The other macros are your object's class name, not XML name.
	BB_SET_OBJECT_MAKE_HOOK(WerehogPole);

	class OnPolePosture : public Sonic::Player::CPlayerSpeedPosture3DCommon
	{
	public:
		static constexpr const char* ms_pStateName = "PoleGrip";


		Sonic::Player::CPlayerSpeedContext* GetContext() const
		{
			return static_cast<Sonic::Player::CPlayerSpeedContext*>(m_pContext);
		}
		void EnterState() override
		{
		}
		void UpdateState() override
		{

		}
		virtual void UnknownStateFunction(void* a1) {}
	};

	HOOK(void*, __fastcall, _InitializePlayer, 0x00D96110, void* This)
	{
		void* result = original_InitializePlayer(This);
		auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();    // Hack: there's a better way to do this but whatever. This writes to the singleton anyway.
		
		
		context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<OnPolePosture>();
		return result;
	}

	void WerehogPole::RegisterObject()
	{
		WRITE_JUMP(0x00EA54E0, 0x00EA5506);
		INSTALL_HOOK(_InitializePlayer);
		BB_INSTALL_SET_OBJECT_MAKE_HOOK(WerehogPole)
	}
}