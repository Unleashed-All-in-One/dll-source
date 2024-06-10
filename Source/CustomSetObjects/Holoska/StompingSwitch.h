#pragma once

class StompingSwitch : public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    BB_SET_OBJECT_MAKE("StompingSwitch")

    boost::shared_ptr<hh::mr::CSingleElement> m_spModelBase;
    boost::shared_ptr<hh::mr::CSingleElement> m_spModelButton;
    boost::shared_ptr<Hedgehog::Mirage::CMaterialData> m_MaterialInactive;
    boost::shared_ptr<Hedgehog::Mirage::CMaterialData> m_MaterialActive;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spModelButtonTransform;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodePoleCollision;
    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeRigidbody;

    Hedgehog::Math::CVector m_ButtonPosition;
    float m_yOffset = 0.0f;
    float shake_timer = 0.0f;

    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

    Sonic::Player::CPlayerSpeedContext* sonic;

    float m_detectionCooldown = 0.0f;

    SharedPtrTypeless m_ButtonSoundHandle;
    SharedPtrTypeless m_ActivationSoundHandle;
    uint32_t m_Test;
    uint32_t extra;

    int m_phase = 0;

    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        m_MaterialInactive = wrapper.GetMaterialData("cmn_obj_km_switch02_2");
        m_MaterialActive = wrapper.GetMaterialData("cmn_obj_km_switch02_2_active");
        boost::shared_ptr<hh::mr::CModelData> spModelDataButton = wrapper.GetModelData("cmn_obj_km_switchbuttonM_000", 0);
        m_spModelButton = boost::make_shared<hh::mr::CSingleElement>(spModelDataButton);

        m_spModelButtonTransform = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spModelButtonTransform->m_Transform.SetPosition(m_ButtonPosition);
        m_spModelButtonTransform->NotifyChanged();
        m_spModelButtonTransform->SetParent(m_spMatrixNodeTransform.get());
        
        m_spModelButton->BindMatrixNode(m_spModelButtonTransform);
        Sonic::CGameObject::AddRenderable("Object", m_spModelButton, true);

        boost::shared_ptr<hh::mr::CModelData> spModelDataBase = wrapper.GetModelData("cmn_obj_km_switchpushM_000", 0);
        m_spModelBase = boost::make_shared<hh::mr::CSingleElement>(spModelDataBase);

        m_spModelBase->BindMatrixNode(m_spMatrixNodeTransform);
        Sonic::CGameObject::AddRenderable("Object", m_spModelBase, true);

        sonic = Sonic::Player::CPlayerSpeedContext::GetInstance();
        return true;
    }

    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            if (std::strstr(in_rMsg.GetType(), "MsgDamage") != nullptr) {
                if (in_rMsg.m_SenderActorID == sonic->m_pPlayer->m_ActorID && m_detectionCooldown <= 0 && sonic->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName() == "Stomping") {
                    m_detectionCooldown = 0.25f;
                    OnLanded();
                }
                return true;
            }
        }
        return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
    };

    void Kill() {
        SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
    }

    bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        m_spNodeRigidbody = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeRigidbody->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
        m_spNodeRigidbody->NotifyChanged();
        m_spNodeRigidbody->SetParent(m_spModelButtonTransform.get());

        m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 2.45, 0));
        m_spNodeEventCollision->NotifyChanged();
        m_spNodeEventCollision->SetParent(m_spNodeRigidbody.get());
        hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(3, 0.5, 3);
        AddEventCollision("Normal", shapeEventTrigger1, *pColID_PlayerEvent, false, m_spNodeEventCollision);
        AddRigidBody(m_spRigidBody, shapeEventTrigger1, *pColID_Common, m_spNodeEventCollision);

        m_spNodePoleCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
        m_spNodePoleCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 1.25, 0));
        m_spNodePoleCollision->NotifyChanged();
        m_spNodePoleCollision->SetParent(m_spNodeRigidbody.get());
        hk2010_2_0::hkpBoxShape* shapeEventTrigger2 = new hk2010_2_0::hkpBoxShape(0.5, 2.5, 0.5);
        AddRigidBody(m_spRigidBody, shapeEventTrigger2, *pColID_Common, m_spNodePoleCollision);
        
        AddRigidBody(m_spRigidBody, "cmn_obj_km_switchpushM_000", "cmn_obj_km_switchpushM_000", *pColID_Common, m_spMatrixNodeTransform, in_spDatabase);
        return true;
    }

    void SetPositions(float deltaTime) {
        if (shake_timer > 0) {
            shake_timer -= deltaTime;
            float newOffset = sinf(shake_timer * 3.0f) * 2.0f;
            Common::ClampFloat(newOffset, -0.5f, 0.5f);
            m_yOffset = Common::Lerp(m_yOffset, newOffset, deltaTime * 10.0f);
        }
        else {
            m_yOffset = Common::Lerp(m_yOffset, 0.0f, deltaTime * 10.0f);
        }
        m_spModelButtonTransform->m_Transform.SetPosition(Common::Lerp(m_spModelButtonTransform->m_Transform.m_Position, m_ButtonPosition + (Eigen::Vector3f::UnitY() * m_yOffset), deltaTime * 8.0f));
        m_spModelButtonTransform->NotifyChanged();
        m_spNodeRigidbody->NotifyChanged();
        m_spNodeEventCollision->NotifyChanged();
        m_spNodePoleCollision->NotifyChanged();
    }

    void OnLanded() {
        m_phase++;
        switch (m_phase) {
            case 1:
                m_ButtonPosition = Eigen::Vector3f(0.0, -0.833, 0.0);
                shake_timer = 0.25f;
                Common::PlaySoundStatic(m_ButtonSoundHandle, 4002063);
                break;
            case 2:
                m_ButtonPosition = Eigen::Vector3f(0.0, -1.666, 0.0);
                shake_timer = 0.25f;
                Common::PlaySoundStatic(m_ButtonSoundHandle, 4002063);
                break;
            case 3:
                m_spModelButton->m_MaterialMap.emplace(m_MaterialInactive.get(), m_MaterialActive);
                m_ButtonPosition = Eigen::Vector3f(0.0, -2.499, 0.0);
                shake_timer = 0.25f;
                Common::PlaySoundStatic(m_ButtonSoundHandle, 4002063);
                Common::PlaySoundStatic(m_ActivationSoundHandle, 4002043);
                break;
        }
    }

    void OnReset() {
        m_phase = 0;
        m_ButtonPosition = Eigen::Vector3f();
        shake_timer = 0.0f;
    }

    void SetUpdateParallel
    (
        const Hedgehog::Universe::SUpdateInfo& updateInfo
    ) override
    {
        SetPositions(updateInfo.DeltaTime);

        if (m_detectionCooldown > 0)
            m_detectionCooldown -= updateInfo.DeltaTime;

        //Sonic::SPadState input = Sonic::CInputState::GetInstance()->GetPadState();
        //if (input.IsTapped(Sonic::eKeyState_X)) {
        //    OnReset();
        //}
    }
    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
    {       
    }
    static void registerObject();
};