#pragma once

class EXPCollect : public Sonic::CObjectBase
{
public:

    boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    float m_Radius = 1.0f;

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
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };

    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
        hk2010_2_0::hkpSphereShape* shapeEventTrigger1 = new hk2010_2_0::hkpSphereShape(0.0f);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        return true;
    }

    void SetUpdateParallel
    (
        const Hedgehog::Universe::SUpdateInfo& updateInfo
    ) override
    {

        Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

        if (!context)
            return;

        DebugDrawText::log(std::format("{0}{1}{2}", context->m_spMatrixNode->m_Transform.m_Position.x(), context->m_spMatrixNode->m_Transform.m_Position.y(), context->m_spMatrixNode->m_Transform.m_Position.z()).c_str(), 0.0f);
        DebugDrawText::log(std::format("{0}{1}{2}", m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_spMatrixNodeTransform->m_Transform.m_Position.y(), m_spMatrixNodeTransform->m_Transform.m_Position.z()).c_str(), 0.0f);
        
        m_spMatrixNodeTransform->m_Transform.m_Position = context->m_spMatrixNode->m_Transform.m_Position;
        m_spMatrixNodeTransform->NotifyChanged();
    }

    static void applyPatches();
};