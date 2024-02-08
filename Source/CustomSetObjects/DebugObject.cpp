namespace Sonic::Player
{

    class IPosture : public Hedgehog::Universe::CStateMachineBase::CStateBase
    {
    public:
        Hedgehog::Base::CSharedString m_ClassName;

        IPosture(const bb_null_ctor&) : CStateBase(bb_null_ctor{}) {}
        IPosture(const Hedgehog::Base::CSharedString& name) : m_ClassName(name) {}
        //IPosture() : IPosture(bb_null_ctor{}) {}
    };

    class CPlayerSpeedPostureCommon : public IPosture
    {
    public:
        float m_Unk1 = 0.0f;
        float m_Unk2 = 0.0f;
        float m_Unk3 = 0.0f;
        Hedgehog::Math::CVector m_Vector = Hedgehog::Math::CVector(0, 0, 0);

        CPlayerSpeedPostureCommon(const bb_null_ctor&) : IPosture(bb_null_ctor{}) {}
        //CPlayerSpeedPostureCommon() : CPlayerSpeedPostureCommon(bb_null_ctor{})

        CPlayerSpeedPostureCommon() : IPosture("CPlayerSpeedPostureCommon")
        {
            *(int*)this = 0x016D3B6C;
        }

        CPlayerSpeedPostureCommon(const Hedgehog::Base::CSharedString& name) : IPosture(name)
        {
            *(int*)this = 0x016D3B6C;
        }
    };

    class CPlayerSpeedPosture3DCommon : public CPlayerSpeedPostureCommon
    {
    public:
        CPlayerSpeedPosture3DCommon(const bb_null_ctor&) : CPlayerSpeedPostureCommon(bb_null_ctor{}) {}

        CPlayerSpeedPosture3DCommon() : CPlayerSpeedPostureCommon("CPlayerSpeedPosture3DCommon")
        {
            *(int*)this = 0x016D392C;
        }

        CPlayerSpeedPosture3DCommon(const Hedgehog::Base::CSharedString& name) : CPlayerSpeedPostureCommon(name)
        {
            *(int*)this = 0x016D392C;
        }

        void UpdateState() override
        {
            BB_FUNCTION_PTR(void, __thiscall, MovementRoutine, 0x00E37FD0, void* This);
            MovementRoutine(this);
        }

    };
}
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

// Whatever your entrypoint is, you'll need this:
void WerehogPole::Install()
{
    WRITE_JUMP(0x00EA54E0, 0x00EA5506);
	INSTALL_HOOK(_InitializePlayer);
    BB_INSTALL_SET_OBJECT_MAKE_HOOK(WerehogPole)
}