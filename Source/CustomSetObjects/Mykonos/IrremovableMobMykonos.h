#pragma once
using namespace hh::math;
class IrremovableMobMykonos :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
{
	int m_characterType;
	int m_motionType;
	bool m_isBeingHit;
	const char* m_prevAnimState;
	float m_timer;
public:
	//name of the set object for the XMLs
	BB_SET_OBJECT_MAKE("IrremovableMobMykonos")

	boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
	boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

	boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
	std::string mobNames[9] = { "man01a", "","","", "man02b", "wom01a","", "wom02a", "wom02c"};
	std::string animNames[18] = {"idle01L","walk01L", "clash01", "plant01L", "talk01L", "talk02L", "audiance01L","audiance02L","working01L", "window01L", "cafe01L", "sitL01L","newspaper01L", "fishing01L", "spieler01L", "book01L", "legs01L", "wash01L"};
	std::vector<NewAnimationData> animations;

	bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
	{
		const char* assetName;
		const char* animName;
		std::string mobName;

		assetName = _strdup(std::format("mob_enm_{0}",mobNames[m_characterType]).c_str());
		switch (m_characterType) {
			case 0: {
				mobName = "manA";
				break;
			}
			case 4: {
				mobName = "manB";
				break;
			}
			case 5: {
				mobName = "womA";
				break;
			}
			case 7:
			case 8: {
				mobName = "womB";
				break;
			}
			default: {
				mobName = "manA";
				break;
			}
		}
		animName = animNames[m_motionType].c_str();
		if (m_motionType == 11)
		{
			if (m_characterType == 5) 
			{
				animName = "sitR01L";
			}
		}

		hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
		boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);

		m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
		m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);

		std::string animPrefix = std::format("mob_{0}_", mobName);
		std::string motionFilename = animPrefix + animName;
		std::string motionFilenameClash = animPrefix + "clash01";

		m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
		animations = std::vector<NewAnimationData>();

		animations.push_back(NewAnimationData(animName, _strdup(motionFilename.c_str()), 1, true, nullptr));
		animations.push_back(NewAnimationData("Clash01", _strdup(motionFilenameClash.c_str()), 1, false, nullptr));
		
		this->SetContext(this); //set Context of AnimatorStateMachine to IAnimatorContext
		ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);
		Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);
		this->ChangeState(animName);
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
		return Hedgehog::Math::CVector(1, 1, 1);
	}

	bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
	{
		if (in_Flag)
		{
			if (std::strstr(in_rMsg.GetType(), "MsgDamage") != nullptr && !m_isBeingHit)
			{
				const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
				if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
				{
					m_isBeingHit = true;
					m_prevAnimState = this->GetCurrentState()->m_Name.c_str();
					this->ChangeState("Clash01");
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

		hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(0.75f, 3.25f, 0.5f);
		hk2010_2_0::hkpBoxShape* shapeEventTrigger2 = new hk2010_2_0::hkpBoxShape(1, 3.5f, 1);

		AddRigidBody(m_spRigidBody, shapeEventTrigger1, *pColID_Common, m_spNodeEventCollision);
		AddEventCollision("Object", shapeEventTrigger2, *pColID_PlayerEvent, true, m_spNodeEventCollision);
		return true;
	}
	void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
	{
		in_rEditParam.CreateParamInt(&m_characterType, "CharaType");
		in_rEditParam.CreateParamInt(&m_motionType, "Motion");
	}
	void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
	{
		m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);

		if (m_isBeingHit == true) 
		{
			m_timer += in_rUpdateInfo.DeltaTime;
			if (m_timer > 2.0935f) //todo: find way to detect anim has finished
			{				
				m_isBeingHit = false;
				m_timer = 0;
				this->ChangeState(m_prevAnimState);
			}
		}
	}
	static void registerObject();
};