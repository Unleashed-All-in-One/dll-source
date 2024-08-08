#pragma once
#include "../../System/CollisionID.h"
using namespace hh::math;
namespace SUC::SetObject
{
	struct MsgStartHangOn : public hh::fnd::MessageTypeSet
	{
		HH_FND_MSG_MAKE_TYPE(0X0167FBA8);
	public:
		int m_Field10;
		void* m_Field14;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_Field18;
		bool m_Field1C;
		bool m_Field1D;
		Hedgehog::math::CVector4* m_Field20;
		Hedgehog::math::CVector m_Field30;
		int m_Field40;
	};
	BB_ASSERT_OFFSETOF(MsgStartHangOn, m_Field40, 0x40);
	class CObjHangOn :public Sonic::CObjectBase, public Sonic::CSetObjectListener
	{
	public:
		BB_SET_OBJECT_MAKE("HangOn")

			hk2010_2_0::hkpBoxShape* shapeEventTrigger1;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		float m_setWidth;
		bool m_inCollider;
		bool hangOn;
		bool preventRight, preventLeft;

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
						//if (playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->GetStateName() == "EvilArmSwing")
						//{
						m_inCollider = true;
						hangOn = true;
						//}
						//m_playerInsideCollider = true;
					}
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_inCollider = false;
						//m_playerInsideCollider = false;
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
			m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeEventCollision->NotifyChanged();
			m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());

			shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(m_setWidth, 1, 1);

			AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
			return true;
		}
		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			if (m_inCollider)
			{
				auto one = playerContext->m_spMatrixNode->m_Transform.m_Position;
				one.y() = 0;
				float distanceRight = abs(ObjectUtility::Distance(one, m_spMatrixNodeTransform->m_Transform.m_Position + (m_spMatrixNodeTransform->m_Transform.m_Rotation * shapeEventTrigger1->m_HalfExtents)));
				float distanceLeft = abs(ObjectUtility::Distance(one, m_spMatrixNodeTransform->m_Transform.m_Position + (m_spMatrixNodeTransform->m_Transform.m_Rotation * -shapeEventTrigger1->m_HalfExtents)));
				DebugDrawText::log(std::format("DISTANCE_L: {0}", distanceLeft).c_str(), 0);
				DebugDrawText::log(std::format("DISTANCE_R: {0}", distanceRight).c_str(), 0);
				preventLeft = distanceLeft < 0.6f;
				preventRight = distanceRight < 0.6f;
				playerContext->m_spMatrixNode->m_Transform.SetRotation(m_spMatrixNodeTransform->m_Transform.m_Rotation);
				//playerContext->m_spMatrixNode->m_Transform.m_Position = Hedgehog::Math::CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x() + 0.4f, m_spMatrixNodeTransform->m_Transform.m_Position.y() + m_playerVerticalProgress, m_spMatrixNodeTransform->m_Transform.m_Position.z());
				//playerContext->m_spMatrixNode->m_Transform.m_Position = ObjectUtility::MoveAroundPivot(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position, Hedgehog::math::CVector(m_playerPoleRotation, 0, m_playerPoleRotation));

			}
			else
			{
				ObjectUtility::DoWerehogArmHomingIfClose(m_spMatrixNodeTransform->m_Transform.m_Position, 10, in_rUpdateInfo.DeltaTime);
			}
		}

		void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
		{
			in_rEditParam.CreateParamFloat(&m_setWidth, "Width");
		}
		static void RegisterObject();
	};
}