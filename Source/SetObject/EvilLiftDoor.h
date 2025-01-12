#pragma once
#include "../UI/EvilQTEUI.h"
using namespace hh::math;
namespace SUC::SetObject
{
	class EvilLiftDoor :public Sonic::CObjectBase, public Sonic::CSetObjectListener
	{
	public:
		// This macro initializes the "make" function, takes the XML's object name.
		BB_SET_OBJECT_MAKE("EvilLiftDoor")

			boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollisionSolid;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
		bool m_isPlayerInside;
		float timerForCompletion;
		float timerForOff;
		bool isDoingIt;
		float yNormal;
		float counterB;
		bool complete;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> soundGrunt;
		static EvilQTEUI* m_ItemBoxUI;
		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
			const char* assetName = "evl_myk_obj_soc_doorA_000";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			if (spModelData == nullptr)
			{
				throw std::runtime_error("The model data for this object could not be loaded. An archive may have not been loaded properly.");
			}
			m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
			Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
			yNormal = m_spMatrixNodeTransform->m_Transform.m_Position.y();

			return true;
		}
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{
				if (std::strstr(in_rMsg.GetType(), "MsgSetPosition") != nullptr)
				{
					Sonic::Message::MsgSetPosition& msg = (Sonic::Message::MsgSetPosition&)(in_rMsg);
					m_spMatrixNodeTransform->m_Transform.m_Position = msg.m_Position;
					yNormal = m_spMatrixNodeTransform->m_Transform.m_Position.y();
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_isPlayerInside = true;
						if (m_ItemBoxUI == nullptr)
						{
							m_ItemBoxUI = EvilQTEUI::Generate(playerContext->m_pPlayer, true, 1);
						}
						m_ItemBoxUI->Show();
						m_ItemBoxUI->rcBtn->GetNode("img")->SetPatternIndex(1);
					}
					return true;
				}

				//OnCollisionExit
				if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_isPlayerInside = false;
					}
				}
				if (std::strstr(in_rMsg.GetType(), "MsgGetHomingAttackPosition") != nullptr)
				{

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
			m_spNodeEventCollisionSolid = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeEventCollisionSolid->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeEventCollisionSolid->NotifyChanged();
			m_spNodeEventCollisionSolid->SetParent(m_spMatrixNodeTransform.get());
			//void __thiscall sub_10C0E00(_DWORD *this, int a2)
			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(5, 6, 2);
			hk2010_2_0::hkpBoxShape* shapeEventTrigger2 = new hk2010_2_0::hkpBoxShape(8, 2, 4);

			AddEventCollision("Object", shapeEventTrigger2, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollisionSolid);
			AddRigidBody(m_spRigidBody, shapeEventTrigger1, CollisionLayerID::Common, m_spNodeEventCollision);
			return true;
		}
		void SetDoorPos(CVector pos)
		{
			m_spMatrixNodeTransform->m_Transform.m_Position = pos;
			m_spMatrixNodeTransform->m_Transform.UpdateMatrix();
			m_spMatrixNodeTransform->NotifyChanged();
		}
		void SetMotionSpeed(uint32_t* anim, float speed)
		{
			static uint32_t pAddrCall = 0xCDEF60;
			__asm
			{
				push speed
				mov     eax, anim
				call[pAddrCall]
			}
		}
		void MotionTest()
		{
			Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			uint32_t* p = (uint32_t*)playerContext->m_pPlayer;
			SetMotionSpeed((p + 244), 0);
		}
		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			auto handVector = (CVector)playerContext->m_pPlayer->m_spCharacterModel->GetNode("Hand_R")->m_WorldMatrix.matrix().col(3);
			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			if (m_isPlayerInside)
			{
				if (inputPtr->IsTapped(Sonic::eKeyState_B))
				{
					if (!isDoingIt)
					{
						timerForCompletion = 0;
						isDoingIt = true;
						playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = true;
						playerContext->ChangeAnimation("Evilsonic_gate");
						Common::PlaySoundStaticCueName(soundGrunt, "V_WHG_023");
					}
					timerForOff = 5;
				}
			}
			if (isDoingIt && !complete)
			{
				DebugDrawText::log(SUC::Format("[Door] TimerOff %.3f | TimerComp %.3f", timerForOff, timerForCompletion), 0);
				if (isDoingIt)
				{
					timerForOff -= in_rUpdateInfo.DeltaTime;
					timerForCompletion += in_rUpdateInfo.DeltaTime;
					if (timerForOff > 0 && timerForCompletion > 1.9f)
					{
						complete = true;
						SetDoorPos(CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_spMatrixNodeTransform->m_Transform.m_Position.y() + 2, m_spMatrixNodeTransform->m_Transform.m_Position.z()));
						m_spNodeEventCollision->NotifyChanged();
						m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 1000, 0));
						m_spNodeEventCollision->m_Transform.UpdateMatrix();
						m_spNodeEventCollision->NotifyChanged();
						m_ItemBoxUI->Dissapear();
					}
					else if (!complete)
					{
						SetDoorPos(CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x(), handVector.y(), m_spMatrixNodeTransform->m_Transform.m_Position.z()));
						playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = false;
					}
					if (timerForOff <= 0)
					{
						playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = false;
						isDoingIt = false;
					}
				}
			}
		}
		static void RegisterObject();
	};
}