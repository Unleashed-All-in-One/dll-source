#pragma once
using namespace hh::math;
struct MsgChangePlayer : public hh::fnd::MessageTypeSet
{
public:
    HH_FND_MSG_MAKE_TYPE(0x0167F870);

    size_t m_PlayerType;

    MsgChangePlayer(size_t playerType) : m_PlayerType(playerType) {}
}; struct MsgChangeHudMode : public hh::fnd::MessageTypeSet
{
public:
    HH_FND_MSG_MAKE_TYPE(0x016803AC);

    size_t m_PlayerType;

    MsgChangeHudMode(size_t playerType) : m_PlayerType(playerType) {}
};

static const int* pColID_ObjectPhysics1 = reinterpret_cast<int*>(0x1E5E79C);
class ETFTimeSwitch :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
    // This macro initializes the "make" function, takes the XML's object name.
    BB_SET_OBJECT_MAKE("ETFTimeSwitch")

        /* Make your fields */
        boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;

    boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

    boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

    size_t playerIndex = 0;
    size_t playerCount = 0;
    
    /* Renderable methods */
    bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
    {
        /* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
        const char* assetName = "cmn_obj_km_etftimeswich_000";
        hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
        boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
        m_spExampleElement = boost::make_shared<hh::mr::CSingleElement>(spModelData);

        m_spExampleElement->BindMatrixNode(m_spMatrixNodeTransform);
        AddRenderable("Object", m_spExampleElement, true);
        static bool init = false;
        Initialize();
        
        return true;
    }
    bool classic = true;
    void ChangeSonic()
    {
        classic = !classic;
        Initialize();
        Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage("Player", boost::make_shared<MsgChangePlayer>(0));
        Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->SendMessage("Player", boost::make_shared<MsgChangeHudMode>(3));
    }
    bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
    {
        if (in_Flag)
        {
            
            if (std::strstr(in_rMsg.GetType(), "MsgSetPosition") != nullptr)
            {
                Sonic::Message::MsgSetPosition& msg = (Sonic::Message::MsgSetPosition&)(in_rMsg);
                m_spMatrixNodeTransform->m_Transform.m_Position = msg.m_Position;
                return true;
            } 
            if (std::strstr(in_rMsg.GetType(), "MsgDamage") != nullptr)
            {
                ChangeSonic();
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


        AddRigidBody(m_spRigidBody, shapeEventTrigger1, *pColID_Common, m_spNodeEventCollision);
        // You don't need to override this if you're not using it, but this would be for setting up event colliders & rigidbodies.
        // note you can do this in "SetAddRenderables" but sonic team *tends to* do collision stuff here.
        return true;
    }      
   

    void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
    {
        const size_t leaderID = Sonic::CGameDocument::GetInstance()->m_pMember->m_PlayerIDs[0];
        auto This = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer;
        if (This->m_ActorID != leaderID)
        {
            hh::math::CVector position;
            hh::math::CQuaternion rotation;
            This->SendMessageImm(leaderID, boost::make_shared<Sonic::Message::MsgGetPosition>(position));
            This->SendMessageImm(leaderID, boost::make_shared<Sonic::Message::MsgGetRotation>(rotation));

            const float angle = ((float)playerIndex + 0.5f) / (float)playerCount * 3.14159265359f;

            This->m_spContext->m_spMatrixNode->m_Transform.SetPosition(
                Eigen::Vector3f(position) + (rotation * Eigen::Vector3f(cosf(angle), 0, -sinf(angle))).normalized() * 1.5f);

            This->m_spContext->m_spMatrixNode->NotifyChanged();

            playerIndex++;
        }
        else
        {
            playerCount = max(playerIndex, playerCount);
            playerIndex = 0;
        }
    }

    
    static void Initialize();
    static void Install();
};