#pragma once

class EXPCollect : public Sonic::CObjectBase
{
public:

    boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    Hedgehog::Math::CVector m_StartPosition;
    Hedgehog::Math::CVector m_Position;
    Hedgehog::Math::CVector m_PositionAdd;

    Hedgehog::Math::CVector m_Scale;

    float m_Radius = 1.0f;
    float m_LifeTime = 0.0f;

    static inline SharedPtrTypeless particle;
    static inline SharedPtrTypeless sound;

    bool m_playerInsideCollider = false;
    Hedgehog::Math::CVector m_Velocity;
    Hedgehog::Math::CVector m_TargetPosition;

    Hedgehog::Math::CVector GetRandOffset(float radius) {
        return Hedgehog::Math::CVector(Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius));
    }

    EXPCollect(Hedgehog::Math::CVector _pos) {
        m_StartPosition = _pos + (Eigen::Vector3f::UnitY() * 1.5f) + GetRandOffset(Common::RandomFloat(0.25f, 1.0f));
        m_Position = _pos + GetRandOffset(Common::RandomFloat(0.1f, 0.5f));
        m_TargetPosition = m_StartPosition;
        m_LifeTime = 0.0f;
        m_Scale = Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
    }

    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        const char* assetName = "cmn_obj_exp_orb";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
        AddRenderable("Object", m_spModel, true);

        Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();
        return true;
    }

    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            //OnCollisionEnter
            if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                    m_playerInsideCollider = true;
                return true;
            }

            // get rid of exp
            if (std::strstr(in_rMsg.GetType(), "MsgRestartStage") != nullptr
                || std::strstr(in_rMsg.GetType(), "MsgStageClear") != nullptr)
            {
                Kill();
                return true;
            }

            //OnCollisionExit
            if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                    m_playerInsideCollider = false;
            }
        }
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };

    void Collect() {        
        Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

        if (!context)
            return;

        void* matrixNode = (void*)((uint32_t)*PLAYER_CONTEXT + 0x30);
        Common::fCGlitterEnd(context, particle, true);
        Common::fCGlitterCreate(context, particle, matrixNode, "ef_ch_sng_lms_expcol", 0);
        Common::PlaySoundStatic(sound, 4002073);

        context->m_ChaosEnergy += Common::RandomFloat(0.25f, 0.75f);
        Common::ClampFloat(context->m_ChaosEnergy, 0.0f, 100.0f);

        Kill();
    }

    void Kill() {
        removeEXPCollect(this);
        SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
    }

    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
        hk2010_2_0::hkpSphereShape* shapeEventTrigger1 = new hk2010_2_0::hkpSphereShape(0.5f);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        return true;
    }

    void SetUpdateParallel
    (
        const Hedgehog::Universe::SUpdateInfo& updateInfo
    ) override
    {

        m_LifeTime += updateInfo.DeltaTime;

        Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

        if (!context)
            return;

        float distance = abs((m_Position - context->m_spMatrixNode->m_Transform.m_Position).norm());
        
        if (m_LifeTime >= 0.25f) {
            m_TargetPosition = Common::Lerp(m_TargetPosition, context->m_spMatrixNode->m_Transform.m_Position + (Eigen::Vector3f::UnitY() * 0.5f), updateInfo.DeltaTime * 4.0f);
        }
        else {
            m_PositionAdd += Eigen::Vector3f::UnitY() * updateInfo.DeltaTime * 0.5f;
            m_TargetPosition = Common::Lerp(m_TargetPosition, m_StartPosition + m_PositionAdd, updateInfo.DeltaTime * 4.0f);
        }
        m_Position = Common::MoveTowards(m_Position, m_TargetPosition, updateInfo.DeltaTime * 8.0f);

        if (m_playerInsideCollider && m_LifeTime >= 0.5f)
            Collect();

        SetPosition(m_Position);
    }

    static void applyPatches();
    static void removeEXPCollect(Sonic::CGameObject* exp);
};