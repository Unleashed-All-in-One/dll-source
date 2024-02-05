#pragma once
using namespace hh::math;
static const int* pColID_Common = reinterpret_cast<int*>(0x01E0AF30);
class ConversionUtility
{
public:
    static Hedgehog::Math::CVector MoveAroundPivot(Hedgehog::Math::CVector& player, const Hedgehog::Math::CVector& pivot, const Hedgehog::Math::CVector& rotationAngles) {

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

    static CQuaternion QuaternionFaceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, CQuaternion rotation) {
        Eigen::Vector3f currentForward = rotation * Eigen::Vector3f::UnitZ();
        Eigen::Vector3f targetDirection = (targetPoint - position).normalized();

        Eigen::Quaternionf rotationQuaternion;
        rotationQuaternion.setFromTwoVectors(currentForward, targetDirection);

        return rotation = rotationQuaternion * rotation;
    }
   
};
static const int* pColID_PlayerEvent = reinterpret_cast<int*>(0x01E0AFD8);
class WerehogPole :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    // This macro initializes the "make" function, takes the XML's object name.
    BB_SET_OBJECT_MAKE("TestObjectNextino")

        /* Make your fields */
        boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;

    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;    bool inCollision;
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
                    inCollision = true;
                }
                return true;
            }
            if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
            {
                const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
                if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                {
                    inCollision = false;
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

    // You'll do update logic here for the most part.
    // Again don't add this if you don't need to, but you usually will for interactables.
    bool inPole;
    float add;
    float add2;
    float add3;
    float cooldownTimer;
    float side = 0;
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
        if (inCollision && inputPtr->IsTapped(Sonic::eKeyState_B))
            inPole = true;
        if (inPole)
        {
            if (inputPtr->IsTapped(Sonic::eKeyState_A))
            {
                playerContext->m_Velocity = CVector(0, 0, 0);
                AddImpulse(playerContext->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector(0, 5, -130), true);
                playerContext->ChangeState(Sonic::Player::StateAction::Jump);
                playerContext->SetStateFlag(Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput, 1);

                playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
                inPole = false;
            }
            add += inputPtr->LeftStickVertical / 30;
            add2 += inputPtr->RightStickVertical;

            m_spMatrixNodeTransform->m_Transform.m_Rotation = ConversionUtility::QuaternionFromAngleAxisUtil(add2, CVector(0, 1, 0));
            add3 += inputPtr->RightStickHorizontal / 10;
            if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
            {
                side++;
                add3 += 1.5f;
            }
            if (add3 > 4.5f)
                add3 = 0;
            if (add3 < 0)
                add3 = 4.5f;
            playerContext->ChangeAnimation("Evilsonic_pillar_idle");
            playerContext->SetStateFlag(Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput, 1 );
            auto playerPos = playerContext->m_spMatrixNode->m_Transform.m_Position;
            auto targetPos = m_spMatrixNodeTransform->m_Transform.m_Position;

           targetPos.y() = playerPos.y();
            CVector veca = targetPos - playerPos;
            CVector vecb = CVector(1, 0, 1); // this is always the direction the particle is initially created in.
            veca.normalize();
            CVector axis = veca.cross(vecb);
            axis.normalize();
            DebugDrawText::log(std::format("Add3: {0} | Side: {1}", add3, side).c_str(), 0);
            playerContext->m_spMatrixNode->m_Transform.m_Rotation = ConversionUtility::QuaternionFaceTowards(targetPos, playerPos, playerContext->m_spMatrixNode->m_Transform.m_Rotation);
            //rotateQuaternionTowardsVector(playerContext->m_spMatrixNode->m_Transform.m_Rotation, m_spMatrixNodeTransform->m_Transform.m_Position);
            playerContext->m_spMatrixNode->m_Transform.m_Position = Hedgehog::Math::CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x() + 1, m_spMatrixNodeTransform->m_Transform.m_Position.y() + add, m_spMatrixNodeTransform->m_Transform.m_Position.z());
            playerContext->m_spMatrixNode->m_Transform.m_Position = ConversionUtility::MoveAroundPivot(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position, Hedgehog::math::CVector(add3, 0, add3));

            
        }
    }

    void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override // argument name/type might be wrong
    {
        auto test = in_rEditParam.m_ParamList[0];
        test->m_Name;
        // Use methods with in_pEditParam to initialize your fields, arg names should be self explanatory
    }

    static void Install();
    /* */
};