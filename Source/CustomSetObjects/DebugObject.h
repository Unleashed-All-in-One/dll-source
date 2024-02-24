#pragma once
using namespace hh::math;

class TransformUtilities
{
public:

    static Hedgehog::Math::CVector MoveAroundPivot(Hedgehog::Math::CVector& player, const Hedgehog::Math::CVector& pivot, const Hedgehog::Math::CVector& rotationAngles) 
    {

        if (rotationAngles.x() == 0)
            return player;

        float x = player.x();
        float y = player.z();

        float px = pivot.x();
        float py = pivot.z();
        float s = sin(rotationAngles.x());
        float c = cos(rotationAngles.x());

        x -= px;
        y -= py;

        double nx = (x * c) - (y * s);
        double ny = (x * s) + (y * c);

        x = nx + px;
        y = ny + py;
        player.x() = x;
        player.z() = y;
        return Hedgehog::Math::CVector(x, player.y(), y);
    }
    static CQuaternion QuaternionFromAngleAxisUtil(float angle, const CVector& axis)
    {
        CQuaternion q;
        float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
        float s = sinf(angle / 2) / m;
        q.x() = axis.x() * s;
        q.y() = axis.y() * s;
        q.z() = axis.z() * s;
        q.w() = cosf(angle / 2);
        return q;
    }
    static float Distance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2) 
    {
        Eigen::Vector3f diff = v1 - v2;
        double distanceSquared = diff.dot(diff);
        return std::sqrt(distanceSquared);
    }
    static Eigen::Vector3f ClampMagnitudeMax(const Eigen::Vector3f& vec, float minMagnitude, float maxMagnitude) {
        float magnitude = vec.norm();
        Eigen::Vector3f clampedVec = vec;

        
        if (magnitude > maxMagnitude) 
        {
            clampedVec *= maxMagnitude / magnitude;
        }

        return clampedVec;
    }
    static CQuaternion QuaternionFaceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, CQuaternion rotation) {
        Eigen::Vector3f currentForward = rotation * Eigen::Vector3f::UnitZ();
        Eigen::Vector3f targetDirection = (targetPoint - position).normalized();

        Eigen::Quaternionf rotationQuaternion;
        rotationQuaternion.setFromTwoVectors(currentForward, targetDirection);

        return rotation = rotationQuaternion * rotation;
    }


};
class WerehogPole :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    // This macro initializes the "make" function, takes the XML's object name.
    BB_SET_OBJECT_MAKE("TestObjectNextino")

        /* Make your fields */
        boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;

    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
    boost::shared_ptr<Sonic::CAnimationStateMachine> animator;

    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;   
    bool m_playerInsideCollider;
    bool m_playerOnPole;
    float m_playerVerticalProgress;
    float add2;
    float m_playerPoleRotation;
    float m_playerPoleRotationTarget;
    float m_playerPoleRotationPrevTarget;
    float cooldownTimer;
    float side = 0;
    float progress;
    /* Renderable methods */
    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        /* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
        const char* assetName = "evl_myk_obj_soc_dialopleAaMid5m_000";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spExampleElement = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        m_spExampleElement->BindMatrixNode(m_spMatrixNodeTransform);
        AddRenderable("Object", m_spExampleElement, true);
        DebugDrawText::log("I EXIST!!", 10);
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
                    m_playerInsideCollider = true;
                }
                return true;
            }
            if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    m_playerInsideCollider = false;
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
        //void __thiscall sub_10C0E00(_DWORD *this, int a2)
        hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 6, 1);
        AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
        // You don't need to override this if you're not using it, but this would be for setting up event colliders & rigidbodies.
        // note you can do this in "SetAddRenderables" but sonic team *tends to* do collision stuff here.
        return true;
    }        
    void AddImpulse(Hedgehog::Math::CVector impulse, bool relative)
    {
        auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();
        if (!relative)
        {
            context->m_Velocity = impulse;
            return;
        }
        else
        {
            context->m_Velocity += impulse;
            return;
        }
    }
    void rotateQuaternionTowardsVector(Eigen::Quaternionf& quat, const Eigen::Vector3f& targetVector) {
        Eigen::Vector3f currentVector = quat * Eigen::Vector3f::UnitZ();
        Eigen::Quaternionf rotation;

        rotation.setFromTwoVectors(currentVector, targetVector);

        quat = rotation * quat;
    }
    CQuaternion faceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, CQuaternion rotation) {
        Eigen::Vector3f currentForward = rotation * Eigen::Vector3f::UnitZ();
        Eigen::Vector3f targetDirection = (targetPoint - position).normalized();

        Eigen::Quaternionf rotationQuaternion;
        rotationQuaternion.setFromTwoVectors(currentForward, targetDirection);

        return rotation = rotationQuaternion * rotation;
    }
    // You'll do update logic here for the most part.
    // Again don't add this if you don't need to, but you usually will for interactables.
    void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
    {
        auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
        const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
        if (m_playerInsideCollider && inputPtr->IsTapped(Sonic::eKeyState_B))
            m_playerOnPole = true;
        if (m_playerOnPole)
        {
            if (inputPtr->IsTapped(Sonic::eKeyState_A))
            {
                playerContext->m_Velocity = CVector(0, 0, 0);
                AddImpulse(playerContext->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector(0, 5, -130), true);
                playerContext->ChangeState(Sonic::Player::StateAction::Jump);
                playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = false;

                playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
                m_playerOnPole = false;
                return;
            }
            if(progress < 1)
            progress += in_rUpdateInfo.DeltaTime * 0.8f;
            m_playerVerticalProgress += inputPtr->LeftStickVertical / 30;
            if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
            {
                side++;
                m_playerPoleRotationPrevTarget = m_playerPoleRotationTarget;
                m_playerPoleRotationTarget += 1.5f;
                progress = 0;
            }
            if (m_playerPoleRotationTarget > 4.5f)
                m_playerPoleRotationTarget = 0;

            if (m_playerPoleRotationTarget < 0)
                m_playerPoleRotationTarget = 4.5f;
            m_playerPoleRotation = Common::lerpUnclampedf(m_playerPoleRotationPrevTarget, m_playerPoleRotationTarget, progress);
            
            //playerContext->ChangeAnimation("Evilsonic_pillar_idle");
            playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = true;
            auto playerPos = playerContext->m_spMatrixNode->m_Transform.m_Position;
            auto targetPos = m_spMatrixNodeTransform->m_Transform.m_Position;
            targetPos.y() = playerPos.y();

            playerContext->m_spMatrixNode->m_Transform.m_Rotation = TransformUtilities::QuaternionFaceTowards(targetPos, playerPos, playerContext->m_spMatrixNode->m_Transform.m_Rotation);
            playerContext->m_spMatrixNode->m_Transform.m_Position = Hedgehog::Math::CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x() + 1, m_spMatrixNodeTransform->m_Transform.m_Position.y() + m_playerVerticalProgress, m_spMatrixNodeTransform->m_Transform.m_Position.z());
            playerContext->m_spMatrixNode->m_Transform.m_Position = TransformUtilities::MoveAroundPivot(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position, Hedgehog::math::CVector(m_playerPoleRotation, 0, m_playerPoleRotation));

            
        }
    }

    static void registerObject();
};