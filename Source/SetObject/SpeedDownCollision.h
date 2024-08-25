#pragma once
#include "../UI/ItemboxUI.h"
using namespace hh::math;
namespace SUC::SetObject
{
    class SpeedDownCollision :public Sonic::CObjectBase, public Sonic::CSetObjectListener
    {
    public:
        static float m_PlayerOriginalSpeed;
        static ItemboxUI* m_ItemBoxUI;
        BB_SET_OBJECT_MAKE("SpeedDownCollision")
            boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
        boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
        boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
        bool m_playerInsideCollider;
        SharedPtrTypeless particle;
        SharedPtrTypeless sound;
        float m_CollisionWidth = 1;
        float m_CollisionHeight = 1;
        float m_CollisionLength = 1;
        float m_Speed = 45;
        float m_Range = 10000;
        bool m_hideUI;
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
                        if (!m_hideUI)
                        {
                            if (m_ItemBoxUI == nullptr)
                            {
                                m_ItemBoxUI = ItemboxUI::Generate(playerContext->m_pPlayer);
                            }
                            m_ItemBoxUI->Show(ItemboxType::SpeedDown);
                        }
                        m_playerInsideCollider = true;
                    }
                    return true;
                }
                if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
                {
                    const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

                    if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
                    {
                        if (!m_hideUI)
                            m_ItemBoxUI->Hide();
                    }
                    m_playerInsideCollider = false;
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
            m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
            m_spNodeEventCollision->NotifyChanged();
            m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
            hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(m_CollisionWidth, m_CollisionHeight, m_CollisionLength);
            AddEventCollision("Object", shapeEventTrigger1, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
            return true;
        }
        void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
        {
            auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
            const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();

            if (playerContext->m_Velocity.norm() > m_Speed && m_playerInsideCollider)
                playerContext->m_Velocity *= 1.0f - in_rUpdateInfo.DeltaTime * 0.85f;
        }
        void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
        {
            in_rEditParam.CreateParamFloat(&m_CollisionHeight, "Collision_Height");
            in_rEditParam.CreateParamFloat(&m_CollisionLength, "Collision_Length");
            in_rEditParam.CreateParamFloat(&m_CollisionWidth, "Collision_Width");
            in_rEditParam.CreateParamFloat(&m_Speed, "Speed");
            in_rEditParam.CreateParamBool(&m_hideUI, "IsHideUI");
        }
        static void RegisterObject();
    };
}