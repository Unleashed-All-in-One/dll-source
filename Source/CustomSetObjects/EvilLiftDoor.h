#pragma once
using namespace hh::math;

static const int* pColID_BasicTerrain = reinterpret_cast<int*>(0x01E0AFAC);
class EvilLiftDoor :public Sonic::CObjectBase, public Sonic::CSetObjectListener
{
public:
	// This macro initializes the "make" function, takes the XML's object name.
	BB_SET_OBJECT_MAKE("EvilLiftDoor")

		boost::shared_ptr<hh::mr::CSingleElement> m_spExampleElement;
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

	bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
	{
		/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
		const char* assetName = "evl_myk_obj_soc_doorA_000";
		hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
		boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
		m_spExampleElement = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		m_spExampleElement->BindMatrixNode(m_spMatrixNodeTransform);				
		Sonic::CGameObject::AddRenderable("Object", m_spExampleElement, true);
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

		AddEventCollision("Object", shapeEventTrigger2, *pColID_PlayerEvent, true, m_spNodeEventCollisionSolid);
		AddRigidBody(m_spRigidBody, shapeEventTrigger1, *pColID_Common, m_spNodeEventCollision);
		return true;
	}


	void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
	{
		Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		if (m_isPlayerInside && !complete)
		{
			auto inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			if (inputPtr->IsTapped(Sonic::eKeyState_B))
			{
				if (!isDoingIt)
				{
					timerForCompletion = 0;
					isDoingIt = true;
					playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = true;
					playerContext->ChangeAnimation("Evilsonic_gate");
				}
				timerForOff = 5;				
			}
			DebugDrawText::log(std::format("TimerOff {0} | TimerComp {1}", timerForOff, timerForCompletion).c_str(), 0);
			if (isDoingIt)
			{
				timerForOff -= in_rUpdateInfo.DeltaTime;
				timerForCompletion += in_rUpdateInfo.DeltaTime;
				if (timerForOff > 0 && timerForCompletion >  5)
				{
					complete = true;
					m_spMatrixNodeTransform->m_Transform.m_Position = CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_spMatrixNodeTransform->m_Transform.m_Position.y() + 2, m_spMatrixNodeTransform->m_Transform.m_Position.z());
					m_spMatrixNodeTransform->m_Transform.UpdateMatrix();
					m_spMatrixNodeTransform->NotifyChanged();
					m_spNodeEventCollision->NotifyChanged();
					m_spNodeEventCollisionSolid->m_Transform.SetPosition(CVector(0, 1000, 0));
					m_spNodeEventCollisionSolid->m_Transform.UpdateMatrix();
					m_spNodeEventCollisionSolid->NotifyChanged();
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
	static void registerObject();
};