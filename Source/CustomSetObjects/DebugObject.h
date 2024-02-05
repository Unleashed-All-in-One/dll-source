#pragma once

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
        return      Hedgehog::Math::CVector(x, player.y(), y);
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
        const char* assetName = "chr_Sonic_HD";
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
                auto quat = Hedgehog::math::CQuaternion(1, -playerContext->m_spMatrixNode->m_Transform.m_Rotation.x(), playerContext->m_spMatrixNode->m_Transform.m_Rotation.y(), -playerContext->m_spMatrixNode->m_Transform.m_Rotation.z());
                AddImpulse(quat * Hedgehog::Math::CVector(100, 100, 100), true);
                inPole = false;
            }
            add += inputPtr->LeftStickVertical / 10;
            if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
            {
                side++;
                add2 = 90 * side;
            }
            Common::ClampFloat(add, 0, 10);
            playerContext->m_spMatrixNode->m_Transform.m_Position = Hedgehog::Math::CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x() + 1, m_spMatrixNodeTransform->m_Transform.m_Position.y() + add, m_spMatrixNodeTransform->m_Transform.m_Position.z());
            playerContext->m_spMatrixNode->m_Transform.m_Position = ConversionUtility::MoveAroundPivot(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position, Hedgehog::math::CVector(add2, 0, 0));

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