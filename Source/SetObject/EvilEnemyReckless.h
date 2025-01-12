#pragma once
#include <hk2010_2_0/hk2010_2_0.h>
#include <Sonic/Havok/PhysicsWorld.h>

#include "../hk2010_2_0_2.h"
#include "../BlueBlurCustom/Sonic/System/CharacterProxy.h"
#include "../hkTest.h"
#include "../BlueBlurCustom/Sonic/System/CharacterProxy.h"
#include "../Player/Evil/AnimationExtension/CeramicSA_hk2010.h"
#include "../System/CollisionID.h"
using namespace hh::math;

//class RigidbodyNode : public Hedgehog::Mirage::CMatrixNode
//{
//public:
//	BB_INSERT_PADDING(0xF0 - sizeof(Hedgehog::Mirage::CMatrixNode));
//	RigidbodyNode* Ctor(RigidbodyNode* in_Rb, hk2010_2_0::hkpRigidBody* a2)
//	{
//		static uint32_t pAddr = 0x010BD9E0;
//		__asm
//		{
//			push a2
//			mov esi, in_Rb
//			call[pAddr]
//		}
//
//	}
//	RigidbodyNode()
//	{
//		
//	}
//	RigidbodyNode(Sonic::CRigidBody* a2)
//	{
//		Ctor(this, a2->m_pHkpRigidBody);
//	}
//};

namespace SUC::SetObject
{


	inline void __declspec(naked) SetMotionType(Sonic::CRigidBody* in_Rb, int a2)
	{
		static uint32_t pAddr = 0x11806E0;
		__asm
		{
			mov esi, in_Rb
			push a2
			call[pAddr]
		}

	};
	inline void __declspec(naked) rigidbodyctor(hk2010_2_0_Conv::hkpRigidBody* in_Rb, hk2010_2_0_Conv::hkpRigidBodyCinfo* a2)
	{
		static uint32_t pAddr = 0x009245F0;
		__asm
		{
			mov edx, a2
			push    edx
			mov     ecx, in_Rb
			call[pAddr]
		}

	};
	inline BBExt::CCharacterProxy* CtorProxy(void* a1, BBExt::CCharacterProxy* a2, Hedgehog::Base::THolder<Sonic::CWorld> a3, hk2010_2_0::hkpShape* a4, hh::math::CVector& a5, hh::math::CQuaternion& a6, int a7, hh::math::CVector& a8)
	{
		static uint32_t pAddr = 0x010E3EF0;
		//a1 = eax
		//a2 = ecx
		__asm
		{
			push a8
			push a7
			push a6
			push a5
			push a4
			push a3
			mov eax, a1
			mov ecx, a2
			call[pAddr]
		}

	};


	class EvilEnemyReckless :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
	{
	public:
		// This macro initializes the "make" function, takes the XML's object name.
		BB_SET_OBJECT_MAKE("EvilEnemyRecklessR")

			boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeNextPos;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
		boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
		boost::shared_ptr<Sonic::CRayCastCollision> m_spRaycastCollision;
		boost::shared_ptr<BBExt::CCharacterProxy> m_spCharacterProxy;

		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> sound;
		std::vector<SUC::NewAnimationData> animations;
		bool m_IsGrounded;
		hh::math::CVector m_Velocity;

		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
			const char* assetName = "em_dark_recl_hi";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
			m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
			animations = std::vector<SUC::NewAnimationData>();
			animations.push_back(SUC::NewAnimationData("SleepIdle", "recl_sleep_l", 1, true, nullptr));
			animations.push_back(SUC::NewAnimationData("Seek", "recl_seek", 1, true, nullptr));
			animations.push_back(SUC::NewAnimationData("RunL", "recl_run_l", 1, true, nullptr));
			/*animations.push_back(SUC::NewAnimationData("Appear", "cmn_obj_sk2_hintring_appear", 1, false, nullptr));
			animations.push_back(SUC::NewAnimationData("Touch", "cmn_obj_sk2_hintring_touch", 1, false, "Idle"));*/

			m_spRaycastCollision = boost::make_shared<Sonic::CRayCastCollision>(Sonic::CGameDocument::GetInstance()->GetWorld().get());
			this->SetContext(this); //set Context of AnimatorStateMachine to IAnimatorContext
			ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);
			Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);

			this->ChangeState("SleepIdle");
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
		bool isColliding;
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
					Common::PlaySoundStaticCueName(sound, "es_damage_norm");
					return true;
				}
				if(std::strstr(in_rMsg.GetType(), "MsgHitEventCollision"))
				{
					m_Velocity -= (m_spMatrixNodeTransform->m_Transform.m_Position - m_spNodeNextPos->m_Transform.m_Position).normalized();
					isColliding = true;
					return true;
				}
			}
			return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
		};
		bool AddRigidBody2(const boost::shared_ptr<Sonic::CRigidBody>& in_spRigidBody,
			hk2010_2_0::hkpShape* in_pShape, const boost::shared_ptr<Hedgehog::Mirage::CMatrixNode>& in_spMatrixNode)
		{
			BB_FUNCTION_PTR(bool, __stdcall, fpCGameObject3DAddRigidBody2, 0xE98BD0,
				const boost::shared_ptr<Sonic::CRigidBody>&, Sonic::CGameObject3D * This, hk2010_2_0::hkpShape * in_pShape,
				Hedgehog::Mirage::CMatrixNode* in_pMatrixNode,boost::shared_ptr<Hedgehog::Mirage::CMatrixNode> in_spMatrixNode);
			return fpCGameObject3DAddRigidBody2(in_spRigidBody, this, in_pShape, in_spMatrixNode.get(), in_spMatrixNode);
		}
		hk2010_2_0_Conv::hkpRigidBody* rb;
		bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeEventCollision->NotifyChanged();
			m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
			m_spNodeNextPos = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeNextPos->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
			m_spNodeNextPos->NotifyChanged();
			m_spNodeNextPos->SetParent(m_spMatrixNodeTransform.get());
			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 1, 1);

			BBExt::CCharacterProxy* characterproxy = new BBExt::CCharacterProxy();
			//auto a7 = Sonic::CollisionID({ Sonic::CollisionID::TypeTerrain }, { Sonic::CollisionID::TypeTerrain, Sonic::CollisionID::TypeKeepOffEnemy });
			auto a8 = hh::math::CVector(0, 0, 0);
			//characterproxy = CtorProxy(this, characterproxy, Sonic::CGameDocument::GetInstance()->GetWorld().get(), shapeEventTrigger1, m_spNodeEventCollision->m_Transform.m_Position, m_spNodeEventCollision->m_Transform.m_Rotation, a7, a8);

			//m_spCharacterProxy = boost::make_shared<BBExt::CCharacterProxy>(this, Sonic::CGameDocument::GetInstance()->GetWorld().get(), shapeEventTrigger1, CVector::Zero(), m_spMatrixNodeTransform->m_Transform.m_Rotation, Sonic::CollisionID({Sonic::CollisionID::TypeTerrain }, {Sonic::CollisionID::TypeTerrain, Sonic::CollisionID::TypeKeepOffEnemy }));


			FUNCTION_PTR(void, __thiscall, setMotionType2, 0x00923D50, void* hkpRigidbody, int motionType, int entityActivation, int colFilter);
			//_WORD *__thiscall hkpWorld::addEntity(int world, _WORD *entity, int initialActivationState)
			FUNCTION_PTR(void*, __thiscall, addEntity, 0x0090B5D0, hk2010_2_0::hkpWorld* world, void* entity, int activstate);
			
			AddRigidBody(m_spRigidBody, shapeEventTrigger1, CollisionLayerID::BasicTerrain, m_spNodeNextPos);
			setMotionType2(m_spRigidBody->m_pHkpRigidBody, 1, 1, 0);
			//void __thiscall sub_10C0E00(_DWORD *this, int a2)
			//
			//hk2010_2_0_Conv::hkpRigidBodyCinfo info = hk2010_2_0_Conv::hkpRigidBodyCinfo();
			//info.m_position = hh::math::CVector4(m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_spMatrixNodeTransform->m_Transform.m_Position.y() + 20, m_spMatrixNodeTransform->m_Transform.m_Position.z(), 1);
			//info.m_mass = 1000.0;
			//info.m_shape = shapeEventTrigger1;
			//info.m_motionType = hk2010_2_0_Conv::hkpMotion::MOTION_DYNAMIC;
			//info.m_gravityFactor = 1;
			//rigidbodyctor(rb, &info);
			//addEntity(Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spPhysicsWorld->m_pHkpWorld, rb, 1);
			//m_spMatrixNodeTransform->NotifyChanged();
			////m_spRigidBody->m_pPhysicsWorld->m_pHkpWorld->addEntity(m_spRigidBody->m_pHkpRigidBody);
			//rb->removeReference();
			//m_spRigidBody->m_pHkpRigidBody = (hk2010_2_0::hkpRigidBody*)rb;

			//SetMotionType(&m_spRigidBody, 0);
			//m_spRigidBody->m_pHkpRigidBody->addReference();
			// You don't need to override this if you're not using it, but this would be for setting up event colliders & rigidbodies.
			// note you can do this in "SetAddRenderables" but sonic team *tends to* do collision stuff here.
			return true;
		}

		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{

			return;
			m_spMatrixNodeTransform->m_Transform.m_Position += m_Velocity;
			m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
			DebugDrawText::log(SUC::Format("%.3f %.3f %.3f", m_spNodeNextPos->m_Transform.m_Position.x(), m_spNodeNextPos->m_Transform.m_Position.y(), m_spNodeNextPos->m_Transform.m_Position.z()), 0);
			
			
			//do physics here
			m_Velocity.y() -= 0.001f;
			m_spNodeNextPos->m_Transform.SetPosition(m_Velocity);
			m_spNodeNextPos->m_Transform.m_Rotation = m_spMatrixNodeTransform->m_Transform.m_Rotation;
			m_spNodeNextPos->NotifyChanged();

			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			if (inputPtr->IsTapped(Sonic::eKeyState_RightBumper))
			{
				m_spMatrixNodeTransform->m_Transform.m_Position.y() += 10;
				m_spMatrixNodeTransform->NotifyChanged();
				//rb->m_dynamicMotion.m_linearVelocity.y() += 10;
			}

			m_spMatrixNodeTransform->NotifyChanged();
			return;
			Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			float distance = abs(ObjectUtility::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position));

			if (distance < 5)
			{
				if (this->GetCurrentState()->m_Name != "Seek")
					this->ChangeState("Seek");
			}
			else
			{
				if (this->GetCurrentState()->m_Name != "RunL")
					this->ChangeState("RunL");
			}
			Hedgehog::Math::CVector newPos = m_spMatrixNodeTransform->m_Transform.m_Position;

			// Apply fake gravity via constant raycast LOL
			Hedgehog::Math::CVector rayEnd = newPos - Hedgehog::Math::CVector(0, 1, 0);

			// Variable to store the hit result
			Sonic::SCollisionHitPointInfo result;

			// Cast a ray from the object root to the given end position & store the result
			m_spRaycastCollision->m_pPhysicsWorld->CheckLineCollisionClosest(result, newPos, rayEnd, CollisionLayerID::BasicTerrain);
			m_IsGrounded = result.Valid;
			if (!m_IsGrounded)
			{
				m_Velocity += Hedgehog::Math::CVector(0, -0.25f, 0);				
			}
			else
			{
				m_Velocity.y() = 0;
			}
			
			DebugDrawText::log(SUC::Format("PosEnemyY: %.3f", m_spMatrixNodeTransform->m_Transform.m_Position.y()), 0);
			newPos = m_spMatrixNodeTransform->m_Transform.m_Position + (m_Velocity * in_rUpdateInfo.DeltaTime);
			// Update Matrix Node
			m_spMatrixNodeTransform->m_Transform.SetPosition(newPos);
			m_spMatrixNodeTransform->NotifyChanged();
		}
		static void RegisterObject();
	};
}