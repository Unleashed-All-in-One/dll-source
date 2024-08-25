#pragma once
#include "..\..\hk2010_2_0_2.h"
using namespace hh::math;
namespace SUC::SetObject
{
	class MsgHit : public Hedgehog::Universe::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01682064);

		int test1;
		int test2;
	};

	inline void DoSomethingRB(boost::shared_ptr<Sonic::CRigidBody>* rigidbody, int a1)
	{
		static uint32_t func = 0x011806E0;
		__asm
		{
			mov esi, rigidbody
			push a1
			call[func]
		}
	}

	class Icicle :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
	{
	public:
		// This macro initializes the "make" function, takes the XML's object name.
		BB_SET_OBJECT_MAKE("Icicle")

			boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeRigidbody;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody2;
		boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
		SharedPtrTypeless sound;
		std::vector<SUC::NewAnimationData> animations;
		void AddImpulse(Sonic::CRigidBody* f, Hedgehog::math::CVector a1)
		{
			static uint32_t add = 0x01180870;
			_asm
			{
				push a1
				mov eax, f
				call[add]
			}
		}

		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
			const char* assetName = "snw_obj_icicleB";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());


			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
			//animations.push_back(SUC::NewAnimationData("Shake", "snw_obj_icicleBmotion01", 1, false, nullptr));

			//ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);

			Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
			//this->ChangeState("Shake");

			return true;
		}
		Hedgehog::Animation::CAnimationPose* GetAnimationPose()
		{
			return m_AnimatorPose.get();
		}
		Hedgehog::Math::CVector GetVelocityForAnimationSpeed()
		{
			return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
		}
		Hedgehog::Math::CVector GetVelocityForAnimationChange()
		{
			return Hedgehog::Math::CVector(1.0f, 1.0f, 1.0f);
		}
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{

				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgHitNotifyRigidBody") != nullptr)
				{
					DebugDrawText::log("HIT!", 10);
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgSetPosition") != nullptr)
				{
					Sonic::Message::MsgSetPosition& msg = (Sonic::Message::MsgSetPosition&)(in_rMsg);
					m_spMatrixNodeTransform->m_Transform.m_Position = msg.m_Position;
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgDamage") != nullptr)
				{
					Common::PlaySoundStaticCueName(sound, "es_damage_norm");
					return true;
				}
			}
			return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
		};
		bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			m_spNodeRigidbody = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeRigidbody->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeRigidbody->NotifyChanged();
			m_spNodeRigidbody->SetParent(m_spMatrixNodeTransform.get());

			m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, -20, 0));
			m_spNodeEventCollision->NotifyChanged();
			m_spNodeEventCollision->SetParent(m_spNodeRigidbody.get());
			//void __thiscall sub_10C0E00(_DWORD *this, int a2)
			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(20, 40, 20);
			AddRigidBody(m_spRigidBody, "snw_obj_icicle", "snw_obj_km_icicleA_000", CollisionLayerID::Common, m_spNodeRigidbody, in_spDatabase);
			AddRigidBody(m_spRigidBody2, "snw_obj_icicle", "snw_obj_km_icicleB_000", CollisionLayerID::Common, m_spNodeRigidbody, in_spDatabase);
			AddEventCollision("Normal", shapeEventTrigger1, CollisionLayerID::PlayerEvent, false, m_spNodeEventCollision);
			m_spRigidBody->m_CollisionCategory = 51;
			m_spRigidBody2->m_CollisionCategory = 51;
			auto test = (uint32_t*)m_spRigidBody->m_pHkpRigidBody;

			auto test2 = (hk2010_2_0_Conv::hkpMotion::MotionType) * (test + 232);
			//to test56 = (hk2010_2_0::hkpMotion::MotionType)test2->m_type;
			auto gggg = ((hk2010_2_0_Conv::hkpRigidBody*)m_spRigidBody2->m_pHkpRigidBody)->m_Motion;
			((hk2010_2_0_Conv::hkpRigidBody*)m_spRigidBody->m_pHkpRigidBody)->m_Motion.m_type.m_storage = 1;
			((hk2010_2_0_Conv::hkpRigidBody*)m_spRigidBody2->m_pHkpRigidBody)->m_Motion.m_type.m_storage = 1;
			//DoSomethingRB(&m_spRigidBody, 0);
			AddImpulse(m_spRigidBody.get(), CVector(5, 5, 5));
			//m_spRigidBody2->m_pHkpRigidBody->m_Collideable.m_Motion->m_type = hk2010_2_0::hkEnum<hk2010_2_0::hkpMotion::MotionType, hk2010_2_0::hkUint8>(hk2010_2_0::hkpMotion::MOTION_DYNAMIC);
			// You don't need to override this if you're not using it, but this would be for setting up event colliders & rigidbodies.
			// note you can do this in "SetAddRenderables" but sonic team *tends to* do collision stuff here.
			return true;
		}
		int numbTest1 = 0;
		int numbTest2 = 0;

		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			//m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
			Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			float distance = abs(ObjectUtility::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position));

			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];

			//auto gggg = m_spRigidBody2->m_pHkpRigidBody->m_Collideable.m_Motion;
		}
		static void RegisterObject();
	};
}