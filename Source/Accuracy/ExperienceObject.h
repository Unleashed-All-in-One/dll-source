#pragma once
namespace SUC::Accuracy
{
	using namespace hh::math;

	class ExperienceObject : public Sonic::CObjectBase
	{
		static inline SharedPtrTypeless particle;
		static inline boost::shared_ptr<Hedgehog::Sound::CSoundHandle> sound;

	public:
		enum EEnemyVtableEntries
		{
			EFIGHTER = 0x016F593C,
			SPINNER = 0x016F70BC,
			EFIGHTERMISSILE = 0x016FB1FC,
			AIRCANNON = 0x016FB62C,
			MOLE = 0x016F912C
		};
		boost::shared_ptr<hh::mr::CSingleElement> m_spModel;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;

		CVector m_StartPosition;
		CVector m_Position;
		CVector m_PositionAtStartHoming;
		CVector m_PositionAdd;
		CQuaternion m_Rotation;
		float m_Scale;
		float m_Angle;
		float m_Radius = 1.0f;
		float m_LifeTime = 0.0f;
		float m_TimerLerp = 0.0f;
		bool m_IsGoingTowardsPlayer;
		bool m_playerInsideCollider = false;
		CVector m_Velocity;
		CVector m_TargetPosition;

		CVector GetRandOffset(float radius)
		{
			return CVector(Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius));
		}

		ExperienceObject(CVector _pos)
		{
			m_StartPosition = _pos + (Eigen::Vector3f::UnitY() * 1.5f) + GetRandOffset(Common::RandomFloat(0.25f, 1.0f));
			m_Position = _pos + GetRandOffset(Common::RandomFloat(0.1f, 0.5f));
			m_TargetPosition = m_StartPosition;
			m_LifeTime = 0.0f;
		}

		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			const char* assetName = "cmn_obj_exp_orb";
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(assetName, 0);
			m_spModel = boost::make_shared<hh::mr::CSingleElement>(spModelData);

			m_spModel->BindMatrixNode(m_spMatrixNodeTransform);
			CGameObject::AddRenderable("Object", m_spModel, true);
			Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();
			return true;
		}

		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{
				//OnCollisionEnter
				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
						m_playerInsideCollider = true;
					return true;
				}

				// get rid of exp
				if (std::strstr(in_rMsg.GetType(), "MsgRestartStage") != nullptr
					|| std::strstr(in_rMsg.GetType(), "MsgStageClear") != nullptr)
				{
					Kill();
					return true;
				}

				//OnCollisionExit
				if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
						m_playerInsideCollider = false;
				}
			}
			return CObjectBase::ProcessMessage(in_rMsg, in_Flag);
		};

		void Collect()
		{
			Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

			if (!context)
				return;

			void* matrixNode = SONIC_GENERAL_CONTEXT->m_spField30.get();
			//Common::KillParticle(context, particle, true);
			//Common::SpawnParticle(context, particle, matrixNode, "ef_ch_sng_lms_expcol", 0);
			Common::PlaySoundStatic(sound, 4002073);

			context->m_ChaosEnergy += Common::RandomFloat(0.25f, 0.75f);
			Common::ClampFloat(context->m_ChaosEnergy, 0.0f, 100.0f);
			AddProgress(10);
			DisplayHUD();
			Kill();
		}

		void Kill()
		{
			RemoveEXPCollect(this);
			SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
		}

		bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 0, 0));
			m_spNodeEventCollision->NotifyChanged();
			m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
			hk2010_2_0::hkpSphereShape* shapeEventTrigger1 = new hk2010_2_0::hkpSphereShape(0.5f);
			AddEventCollision("Object", shapeEventTrigger1, CollisionLayerID::PlayerEvent, true, m_spNodeEventCollision);
			return true;
		}
		float easeInCirc(float x )
		{
			return 1 - cos((x * PI) / 2);
		}
		float easeOutCubic(float x)
		{
			return 1 - pow(1 - x, 3);
		}
		void SetUpdateParallel
		(
			const Hedgehog::Universe::SUpdateInfo& updateInfo
		) override
		{
			float deltaTimeFixed = updateInfo.DeltaTime * Project::s_DeltaTo60;
			m_LifeTime += deltaTimeFixed;

			Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

			if (!context)
				return;

			float durationOfStep1 = 1;
			float duration1 = 2;
			float duration2 = 2.1f;
			float distance = abs((m_Position - context->m_spMatrixNode->m_Transform.m_Position).norm());
			m_Angle += 0.15f * deltaTimeFixed;
			m_Rotation = ObjectUtility::QuaternionFromAngleAxisUtil(m_Angle, CVector(0, 1, 0));
			m_TimerLerp += deltaTimeFixed;
			if(m_LifeTime < durationOfStep1)
			{
				m_PositionAdd += (Eigen::Vector3f::UnitY() * deltaTimeFixed * 2);
				m_TargetPosition = Common::Lerp(m_StartPosition, m_StartPosition + m_PositionAdd, easeOutCubic(m_TimerLerp / durationOfStep1));
				m_Position = Common::MoveTowards(m_Position, m_TargetPosition, easeOutCubic(m_TimerLerp / durationOfStep1));
				m_Scale = Common::Lerp(0.5f, 1.75f, m_TimerLerp / durationOfStep1);
			}
			if (m_LifeTime >= durationOfStep1 + 0.15f)
			{
				//lerp towards player
				if (!m_IsGoingTowardsPlayer)
				{
					m_IsGoingTowardsPlayer = true;
					m_TimerLerp = 0;
					m_PositionAtStartHoming = m_TargetPosition;
				}
				//So that time is doubled
				m_TimerLerp += deltaTimeFixed;
				m_Position = Common::Lerp(m_PositionAtStartHoming, context->m_spMatrixNode->m_Transform.m_Position + (Eigen::Vector3f::UnitY() * 0.5f), easeInCirc(m_TimerLerp / duration2));
				DebugDrawText::log(SUC::Format("[EXP] TIMERLERP: %.3f\nLIFETIME: %.3f\nANGLE: %.3f", m_TimerLerp, m_LifeTime, m_Angle), 0);
			}
			
			//

			if ((m_playerInsideCollider && m_LifeTime >= 0.5f) || m_LifeTime > 3)
				Collect();
			SetPosition(m_Position);
			m_spMatrixNodeTransform->m_Transform.SetRotation(m_Rotation);
			ObjectUtility::SetTransformScale(m_spMatrixNodeTransform->m_Transform, CVector(m_Scale, m_Scale, m_Scale));
		}

		static void RegisterHooks();
		static void AddProgress(float progress);
		static void DisplayHUD();
		static void RemoveEXPCollect(CGameObject* exp);
	};
}