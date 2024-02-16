#pragma once

class EXPCollect : public Sonic::CObjectBase
{
public:

    boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    Hedgehog::Math::CVector m_StartPosition;
    Hedgehog::Math::CVector m_Position;

    Hedgehog::Math::CVector m_Scale;

    float m_Radius = 1.0f;
    float m_LifeTime = 0.0f;
    float m_FollowSpeed = 5.0f;

    bool m_playerInsideCollider;

    EXPCollect(Hedgehog::Math::CVector _pos) {
        m_StartPosition = _pos;
        m_Position = _pos;
        m_LifeTime = 0.0f;
        m_Scale = Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
    }

    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        const char* assetName = "cmn_hintring_suc";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
        AddRenderable("Object", m_spModel, true);
        DebugDrawText::log("I EXIST!!", 10);
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
                    m_playerInsideCollider = in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID;
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

    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
        hk2010_2_0::hkpSphereShape* shapeEventTrigger1 = new hk2010_2_0::hkpSphereShape(1.0f);
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

        DebugDrawText::log(std::format("EXPCollide? : {0}", m_playerInsideCollider).c_str(), 0.0f);
        
        if (m_LifeTime >= 1.0f)
            m_Position = Common::LerpVector(m_Position, context->m_spMatrixNode->m_Transform.m_Position + (Eigen::Vector3f::UnitY() * 0.5f), updateInfo.DeltaTime * m_FollowSpeed);

        if (m_playerInsideCollider && m_LifeTime >= 2.0f)
            m_Scale = Common::LerpVector(m_Scale, Hedgehog::Math::CVector(0.0f, 0.0f, 0.0f), updateInfo.DeltaTime);

        auto& rTransform = m_spModel->m_spInstanceInfo->m_Transform;
        auto& rMatrix = rTransform.matrix();

        const float scale = max(rMatrix.col(0).head<3>().norm(),
            max(rMatrix.col(1).head<3>().norm(), rMatrix.col(2).head<3>().norm()));

        rMatrix.col(0) /= (scale / 0.2f);
        rMatrix.col(1) /= (scale / 0.2f);
        rMatrix.col(2) /= (scale / 0.2f);

        (&rTransform)[1] = rTransform;

        SetPosition(m_Position);
    }

    static void applyPatches();
};