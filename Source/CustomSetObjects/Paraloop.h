#pragma once
using namespace hh::math;
class Paraloop :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    BB_SET_OBJECT_MAKE("Paraloop")
    boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;   
    bool m_playerInsideCollider;
    SharedPtrTypeless particle;
    SharedPtrTypeless sound;
    float m_CollisionWidth = 1;
    float m_CollisionHeight = 1;
    float m_CollisionLength = 1;
    float m_Range = 10000;
    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {        
        return true;
    }
    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    auto bone = playerContext->m_pPlayer->m_spCharacterModel->GetNode("Reference");
                    if(!particle)
                    Common::fCGlitterCreate(playerContext, particle, &bone, "ef_ch_sng_lms_paraloop02", 0);
                    m_playerInsideCollider = true;
                    Common::PlaySoundStatic(sound,2002501);
                }
                return true;
            }
            if (std::strstr(in_rMsg.GetType(), "MsgSetPosition") != nullptr)
            {
                Sonic::Message::MsgSetPosition& msg = (Sonic::Message::MsgSetPosition&)(in_rMsg);
                m_spMatrixNodeTransform->m_Transform.m_Position = msg.m_Position;
                return true;
            }
        }
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };
    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0,0,0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
        hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(m_CollisionWidth, m_CollisionHeight, m_CollisionLength);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        return true;
    }        
    void StopParaloop()
    {
        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
        Common::fCGlitterEnd(playerContext, particle, true);
        particle = nullptr;
        m_playerInsideCollider = false;
    }
    void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
    {
        auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
        if (m_playerInsideCollider)
        {
            DebugDrawText::log(std::format("Paraloop. Speed: {0} | Param: {1}", abs(playerContext->m_Velocity.norm()), playerContext->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_ParaloopMinSpeed)).c_str(), 0);
            if (abs(playerContext->m_Velocity.norm()) < playerContext->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_ParaloopMinSpeed)) {
                StopParaloop();
            }
            if (TransformUtilities::Distance(m_spMatrixNodeTransform->m_Transform.m_Position, playerContext->m_spMatrixNode->m_Transform.m_Position) > 50)
            {
                StopParaloop();
            }
        }
        
    }
    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
    {
        in_rEditParam.SetFloat(&m_CollisionHeight, "Collision_Height");
        in_rEditParam.SetFloat(&m_CollisionLength, "Collision_Length");
        in_rEditParam.SetFloat(&m_CollisionWidth, "Collision_Width");
    }
    static void Install();
};