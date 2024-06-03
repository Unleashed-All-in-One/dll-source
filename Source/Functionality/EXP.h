#pragma once
using namespace hh::math;
class EXPCollect : public Sonic::CObjectBase
{
public:

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

	static inline SharedPtrTypeless particle;
	static inline SharedPtrTypeless sound;
	bool m_IsGoingTowardsPlayer;


	bool m_playerInsideCollider = false;
	CVector m_Velocity;
	CVector m_TargetPosition;

	CVector GetRandOffset(float radius) {
		return CVector(Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius), Common::RandomFloat(-radius, radius));
	}

	EXPCollect(CVector _pos) {
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

	void Collect() {
		Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!context)
			return;

		void* matrixNode = (void*)((uint32_t)*PLAYER_CONTEXT + 0x30);
		Common::fCGlitterEnd(context, particle, true);
		Common::fCGlitterCreate(context, particle, matrixNode, "ef_ch_sng_lms_expcol", 0);
		Common::PlaySoundStatic(sound, 4002073);

		context->m_ChaosEnergy += Common::RandomFloat(0.25f, 0.75f);
		Common::ClampFloat(context->m_ChaosEnergy, 0.0f, 100.0f);
		addProgress(10);
		displayHUD();
		Kill();
	}

	void Kill() {
		removeEXPCollect(this);
		SendMessage(m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
	}

	bool SetAddColliders(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
	{
		m_spNodeEventCollision = boost::make_shared<Sonic::CMatrixNodeTransform>();
		m_spNodeEventCollision->m_Transform.SetPosition(CVector(0, 0, 0));
		m_spNodeEventCollision->NotifyChanged();
		m_spNodeEventCollision->SetParent(m_spMatrixNodeTransform.get());
		hk2010_2_0::hkpSphereShape* shapeEventTrigger1 = new hk2010_2_0::hkpSphereShape(0.5f);
		AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
		return true;
	}

	void SetUpdateParallel
	(
		const Hedgehog::Universe::SUpdateInfo& updateInfo
	) override
	{

		m_LifeTime += updateInfo.DeltaTime;

		Sonic::Player::CPlayerSpeedContext* context = Sonic::Player::CPlayerSpeedContext::GetInstance();

		if (!context)
			return;


		float distance = abs((m_Position - context->m_spMatrixNode->m_Transform.m_Position).norm());
		m_Angle += 0.15f * updateInfo.DeltaTime;
		m_Rotation = ObjectUtility::QuaternionFromAngleAxisUtil(m_Angle, CVector(0, 1, 0));
		if (m_LifeTime >= 0.75f)
		{
			//lerp towards player
			if (!m_IsGoingTowardsPlayer)
			{
				m_IsGoingTowardsPlayer = true;
				m_TimerLerp = 0;
				m_PositionAtStartHoming = m_TargetPosition;
			}
			m_TimerLerp += updateInfo.DeltaTime / 0.5f;
			m_Position = Common::Lerp(m_PositionAtStartHoming, context->m_spMatrixNode->m_Transform.m_Position + (Eigen::Vector3f::UnitY() * 0.5f), m_TimerLerp);
			DebugDrawText::log(std::format("TIMERLERP: {0}\nLIFETIME: {1}\nANGLE: {2}", m_TimerLerp, m_LifeTime, m_Angle).c_str(),0);
		}
		else
		{
			//go up anim
			m_TimerLerp += updateInfo.DeltaTime;
			m_PositionAdd += Eigen::Vector3f::UnitY() * updateInfo.DeltaTime * 0.5f;
			m_TargetPosition = Common::Lerp(m_StartPosition, m_StartPosition + m_PositionAdd, m_TimerLerp / 1);
			m_Position = Common::MoveTowards(m_Position, m_TargetPosition, m_TimerLerp / 1);
			m_Scale = Common::Lerp(0.5f, 1.75f, m_TimerLerp / 1);
		}
		//

		if ((m_playerInsideCollider && m_LifeTime >= 0.5f) || m_LifeTime > 3)
			Collect();
		SetPosition(m_Position);
		m_spMatrixNodeTransform->m_Transform.SetRotation(m_Rotation);
		ObjectUtility::SetTransformScale(m_spMatrixNodeTransform->m_Transform, CVector(m_Scale, m_Scale, m_Scale));
	}

	static void applyPatches();
	static void addProgress(float progress);
	static void displayHUD();
	static void removeEXPCollect(CGameObject* exp);
};