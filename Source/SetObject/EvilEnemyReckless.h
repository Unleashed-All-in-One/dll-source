#pragma once
using namespace hh::math;

class EvilEnemyReckless :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
{
public:
	// This macro initializes the "make" function, takes the XML's object name.
	BB_SET_OBJECT_MAKE("EvilEnemyRecklessR")

		boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
	boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
	boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
	SharedPtrTypeless sound;
	std::vector<NewAnimationData> animations;

	bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
	{
		/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
		const char* assetName = "em_dark_recl_hi";
		hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
		boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
		m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
		m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
		animations = std::vector<NewAnimationData>();
		animations.push_back(NewAnimationData("SleepIdle", "recl_sleep_l", 1, true, nullptr));
		animations.push_back(NewAnimationData("Seek", "recl_seek", 1, true, nullptr));
		animations.push_back(NewAnimationData("RunL", "recl_run_l", 1, true, nullptr));
		/*animations.push_back(NewAnimationData("Appear", "cmn_obj_sk2_hintring_appear", 1, false, nullptr));
		animations.push_back(NewAnimationData("Touch", "cmn_obj_sk2_hintring_touch", 1, false, "Idle"));*/

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
		}
		return Sonic::CObjectBase::ProcessMessage(in_rMsg, in_Flag);
	};
	bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
	{
		m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
		m_spNodeEventCollision->m_Transform.SetPosition(Hedgehog::Math::CVector(0, 0, 0));
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
		m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
		Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		float distance = abs(ObjectUtility::Distance(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position));
		
		if (distance < 5)
		{
			if(this->GetCurrentState()->m_Name != "Seek")
			this->ChangeState("Seek");
		}
		else
		{
			if (this->GetCurrentState()->m_Name != "RunL")
			this->ChangeState("RunL");
		}
	}
	static void registerObject();
};