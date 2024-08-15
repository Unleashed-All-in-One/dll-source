#pragma once
#include "../AnimationExtension/CameraMotion.h"

namespace Sonic::Message
{
	class MsgDamage : public hh::fnd::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x01681E80);

		//Hedgehog::Base::SSymbolNode* m_DamageType {};
		uint32_t m_DamageType{};
		int m_DamageEffectID{};
		Hedgehog::Math::CVector m_DamagePosition{};
		Hedgehog::Math::CVector m_PositionB{};
		Hedgehog::Math::CVector m_Velocity{};
		int m_SuccessActorIDOverride{};

		MsgDamage(uint32_t in_DamageType, const Hedgehog::Math::CVector& position, const Hedgehog::Math::CVector& velocity)
			: m_DamageType(in_DamageType), m_DamageEffectID(0), m_DamagePosition(position), m_PositionB(position), m_Velocity(velocity), m_SuccessActorIDOverride(0)
		{}

		MsgDamage(uint32_t in_DamageType, const Hedgehog::Math::CVector& position)
			: m_DamageType(in_DamageType), m_DamageEffectID(0), m_DamagePosition(position), m_PositionB(position), m_Velocity(Hedgehog::Math::CVector::Zero()), m_SuccessActorIDOverride(0)
		{}
	};
	BB_ASSERT_SIZEOF(MsgDamage, 0x60);
}
struct MsgIsPlayerDamaging : hh::fnd::MessageTypeGet
{
	HH_FND_MSG_MAKE_TYPE(0x0167FBDC);
public:
	bool m_Field10;
};
using namespace hh::math;
class CAttackHitbox : public Sonic::CGameObject3D
{
public:
	std::string hitboxName;
	boost::shared_ptr<hh::mr::CSingleElement> m_spRenderable;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
	boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
	float m_Timer;
	float m_TimerMax = 20;
	hk2010_2_0::hkpSphereShape* shapeEventTrigger = new hk2010_2_0::hkpSphereShape(1);

	void BindToBone(const char* boneName)
	{
		hitboxName = boneName;
		if (m_spMatrixNodeTransform)
		{
			m_spMatrixNodeTransform->NotifyChanged();
			auto node = Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_spCharacterModel->GetNode(boneName);
			m_spMatrixNodeTransform->SetParent(node.get());
			m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(0, 0, 0));
		}
		else
		{
			printf("\nTried to bind to bone, but the matrixnode doesn't exist!");
		}
	}
	void Kill()
	{

	}
	virtual void AddCallback(const hh::base::THolder<Sonic::CWorld>& worldHolder,
		Sonic::CGameDocument* pGameDocument, const boost::shared_ptr<hh::db::CDatabase>& spDatabase) override
	{
		Sonic::CApplicationDocument::GetInstance()->AddMessageActor("GameObject", this);
		pGameDocument->AddUpdateUnit("0", this);
		

		m_spMatrixNodeTransform->NotifyChanged();
		m_spMatrixNodeTransform->m_Transform.SetPosition(CVector(0, 0, 0));

		m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
		m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 0, 0));
		m_spNodeEventCollision->NotifyChanged();
		m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
		//27
		int col = Sonic::CollisionID({ Sonic::CollisionID::TypePlayerAttack }, { Sonic::CollisionID::TypeEnemy, Sonic::CollisionID::TypeLockon, Sonic::CollisionID::TypeEvent, Sonic::CollisionID::TypeEventTrigger, Sonic::CollisionID::TypeBreakable }).m_Data;
		AddEventCollision("Normal", shapeEventTrigger,col , true, m_spNodeEventCollision);
	}
	void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& updateInfo) override
	{
		m_Timer += updateInfo.DeltaTime;
		if (m_Timer > m_TimerMax)
		{
			SendMessage(this->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		}
	}
	bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
	{
		if (in_Flag)
		{
			if (std::strstr(in_rMsg.GetType(), "MsgIsPlayerDamaging") != nullptr)
			{
				((MsgIsPlayerDamaging&)in_rMsg).m_Field10 = true;
				return true;
			}
			if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
			{
				auto vector1 = CVector(100, 100, 100);
				auto out_msgDamage = boost::make_shared<Sonic::Message::MsgDamage>(*(int*)0x01E0BE28, m_spMatrixNodeTransform->m_Transform.m_Position, Sonic::Player::CPlayerSpeedContext::GetInstance()->m_Velocity);
				
				SendMessage(in_rMsg.m_SenderActorID, out_msgDamage);
				return true;
			}
		}
		return Sonic::CGameObject::ProcessMessage(in_rMsg, in_Flag);
	};
	
};


