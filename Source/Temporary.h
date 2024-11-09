

//This is in the latest blueblur, but we have to reconfigure the project because of big changes to blueblur, so for now this is staying here.
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
    BB_ASSERT_OFFSETOF(IPosture, m_ClassName, 0x60);

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

       //void UpdateState() override
       //{
       //   //BB_FUNCTION_PTR(void, __thiscall, MovementRoutine, 0x00E37FD0, void* This);
       //   //MovementRoutine(this);
       //}

    };
    class CPlayerSpeedPosture3DStandard : public IPosture
    {
    public:
        CPlayerSpeedPosture3DStandard() : IPosture("Revcerse")
        {
            *(int*)this = 0x016D3C68;
        }
        BB_FUNCTION_PTR(void, __thiscall, fEnterState, 0x119DA50, void* This);
        void EnterState() override
        {
        }
        BB_FUNCTION_PTR(void, __thiscall, fUpdateState, 0x119DB00, void* This);
        void UpdateState() override
        {
            fUpdateState(this);
           //BB_FUNCTION_PTR(void, __thiscall, MovementRoutine, 0x00E37FD0, void* This);
           //MovementRoutine(this);
        }

    };
}