#pragma once
using namespace hh::math;
namespace SUC::SetObject
{
	class Pelican :public Sonic::CObjectBase, public Sonic::CSetObjectListener, public Sonic::IAnimationContext, public Sonic::CAnimationStateMachine
	{
	public:
		BB_SET_OBJECT_MAKE("Pelican")

			boost::shared_ptr<hh::mr::CSingleElement> m_spSpawnedModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;
		boost::shared_ptr<Hedgehog::Animation::CAnimationPose> m_AnimatorPose;
		boost::shared_ptr<Hedgehog::Sound::CSoundHandle> sound;
		std::vector<SUC::NewAnimationData> animations;
		float timer;
		bool isRight = true;
		bool hasScared = false;
		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			/* returning false if your renderable doesn't exist is recommended, your object simply wont render / will cease construction or w/e. */
			const char* assetName = "myk_obj_soc_pelican";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			m_spSpawnedModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spSpawnedModel->BindMatrixNode(m_spMatrixNodeTransform);
			m_AnimatorPose = boost::make_shared<Hedgehog::Animation::CAnimationPose>(in_spDatabase, assetName);
			animations = std::vector<SUC::NewAnimationData>();
			animations.push_back(SUC::NewAnimationData("Surprise", "myk_obj_soc_pelican_surprise", 1, true, nullptr));
			animations.push_back(SUC::NewAnimationData("Stay", "myk_obj_soc_pelican_stay", 1, true, nullptr));
			animations.push_back(SUC::NewAnimationData("TurnL", "myk_obj_soc_pelican_pose_L", 1, false, nullptr));
			animations.push_back(SUC::NewAnimationData("TurnR", "myk_obj_soc_pelican_pose_R", 1, false, nullptr));
			this->SetContext(this); //set Context of AnimatorStateMachine to IAnimatorContext
			ObjectUtility::RegisterAnimations(m_AnimatorPose, animations, m_spSpawnedModel, this);

			Sonic::CGameObject::AddRenderable("Object", m_spSpawnedModel, true);

			this->ChangeState("Stay");
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
			return Hedgehog::Math::CVector(0.2f, 0.2f, 0.2f);
		}
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{
				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID && !hasScared)
					{
						hasScared = true;
						this->ChangeState("Surprise");
						Common::PlaySoundStaticCueName(sound, "objsn_myk_pelicanflaps");
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
			//void __thiscall sub_10C0E00(_DWORD *this, int a2)
			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, 2, 1);
			hk2010_2_0::hkpBoxShape* shapeEventTrigger2 = new hk2010_2_0::hkpBoxShape(3, 3, 3);


			AddRigidBody(m_spRigidBody, shapeEventTrigger1, CollisionLayerID::Common, m_spNodeEventCollision);
			AddEventCollision("Object", shapeEventTrigger2, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
			// You don't need to override this if you're not using it, but this would be for setting up event colliders & rigidbodies.
			// note you can do this in "SetAddRenderables" but sonic team *tends to* do collision stuff here.
			return true;
		}
		void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
		{
			in_rEditParam.CreateParamBool(&isRight, "IsRight");
		}

		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			m_AnimatorPose->Update(in_rUpdateInfo.DeltaTime);
			if (GetCurrentState()->m_Name == "Surprise")
			{
				timer += in_rUpdateInfo.DeltaTime;
				if (timer > 3)
				{
					sound.reset();
					if (isRight)
						this->ChangeState("TurnR");
					else
						this->ChangeState("TurnL");
				}
			}
		}
		static void RegisterObject();
	};
}