#pragma once
using namespace hh::math;
class Paraloop :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    BB_SET_OBJECT_MAKE("Paraloop")
    boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;   
    bool m_playerInsideCollider;
    SharedPtrTypeless particle = nullptr;
    SharedPtrTypeless sound = nullptr;
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
                    if (particle == nullptr && abs(playerContext->m_Velocity.norm()) >= playerContext->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_ParaloopMinSpeed)) {
                        void* matrixNode = (void*)((uint32_t)*PLAYER_CONTEXT + 0x30);
                        Common::fCGlitterCreate(playerContext, particle, matrixNode, *pSuperSonicContext ? "ef_ch_sps_lms_paraloop02" : "ef_ch_sng_lms_paraloop02", 0);
                        Common::PlaySoundStaticCueName(sound, "suc_paraloop");
                    }         
                    m_playerInsideCollider = true;
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
            DebugDrawText::log(SUC::Format("[Paraloop] Speed: %.3f | Param: %.3f", abs(playerContext->m_Velocity.norm()), playerContext->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_ParaloopMinSpeed)), 0);
            if (abs(playerContext->m_Velocity.norm()) < playerContext->m_spParameter->Get<float>(Sonic::Player::ePlayerSpeedParameter_ParaloopMinSpeed)) {
                StopParaloop();
            }
            if (ObjectUtility::Distance(m_spMatrixNodeTransform->m_Transform.m_Position, playerContext->m_spMatrixNode->m_Transform.m_Position) > 50)
            {
                StopParaloop();
            }
        }
        
    }
    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
    {
        in_rEditParam.CreateParamFloat(&m_CollisionHeight, "Collision_Height");
        in_rEditParam.CreateParamFloat(&m_CollisionLength, "Collision_Length");
        in_rEditParam.CreateParamFloat(&m_CollisionWidth, "Collision_Width");
    }
    static void registerObject();
};