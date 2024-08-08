#pragma once
using namespace hh::math;
namespace SUC::SetObject
{
	class PoleStyle
	{
	public:
		const char* modelNameTop;
		const char* modelNameMid;
		const char* modelNameBtm;
		PoleStyle(const char* top, const char* mid, const char* btm)
		{
			modelNameTop = top;
			modelNameMid = mid;
			modelNameBtm = btm;
		}
	};
	class WerehogPole :public Sonic::CObjectBase, public Sonic::CSetObjectListener
	{
	public:
		BB_SET_OBJECT_MAKE("EvilColumn")
			boost::shared_ptr<hh::mr::CSingleElement> m_spModelPoleMid, m_spModelPoleTop, m_spModelPoleBtm;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeEventCollision;
		boost::shared_ptr<Sonic::CMatrixNodeTransform> m_spNodeColumnTop;
		boost::shared_ptr<Sonic::CRigidBody> m_spRigidBody;

		//## Set params
		int m_setColliderLength;
		bool m_setRotation90Snap;
		int m_setModelType;
		uint32_t m_Test;

		//## Pole Variables
		bool m_playerInsideCollider;
		bool m_playerOnPole;

		float m_playerVerticalProgressPrevious;
		float m_playerVerticalProgress;
		float m_playerVerticalProgressTarget;

		float m_playerPoleRotation;
		float m_playerPoleRotationPrevTarget;
		float m_playerPoleRotationTarget;
		float m_rotationSide = 0;
		float m_rotationAnimProgress;

		bool m_isLeaping;
		float m_timerLeap;
		float timerSound;
		CVector swingPosition;

		bool isLeftStickUpPressed = false;
		bool isAPressed = false;
		SharedPtrTypeless soundClimb;

		PoleStyle poleTypes[1] = { PoleStyle("evl_myk_obj_soc_dialopleAaTop_000", "evl_myk_obj_soc_dialopleAaMid5m_000", "evl_myk_obj_soc_dialopleAaBtm_000") };
		bool SetAddRenderables(Sonic::CGameDocument* in_pGameDocument, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase) override
		{
			hh::mr::CMirageDatabaseWrapper wrapper(in_spDatabase.get());
			boost::shared_ptr<hh::mr::CModelData> spModelData = wrapper.GetModelData(poleTypes[m_setModelType].modelNameMid, 0);
			boost::shared_ptr<hh::mr::CModelData> spModelDataT = wrapper.GetModelData(poleTypes[m_setModelType].modelNameTop, 0);
			boost::shared_ptr<hh::mr::CModelData> spModelDataB = wrapper.GetModelData(poleTypes[m_setModelType].modelNameBtm, 0);
			if (spModelData == nullptr || spModelDataT == nullptr || spModelDataB == nullptr)
			{
				throw std::runtime_error("The model data for this object could not be loaded. An archive may have not been loaded properly.");
			}
			m_spModelPoleMid = boost::make_shared<hh::mr::CSingleElement>(spModelData);
			m_spModelPoleTop = boost::make_shared<hh::mr::CSingleElement>(spModelDataT);
			m_spModelPoleBtm = boost::make_shared<hh::mr::CSingleElement>(spModelDataB);

			m_spNodeColumnTop = boost::make_shared<Sonic::CMatrixNodeTransform>();
			m_spNodeColumnTop->m_Transform.SetPosition(Hedgehog::Math::CVector(0, m_setColliderLength, 0));
			m_spNodeColumnTop->NotifyChanged();
			m_spNodeColumnTop->SetParent(m_spMatrixNodeTransform.get());

			m_spModelPoleMid->BindMatrixNode(m_spMatrixNodeTransform);
			m_spModelPoleTop->BindMatrixNode(m_spNodeColumnTop);
			m_spModelPoleBtm->BindMatrixNode(m_spMatrixNodeTransform);

			swingPosition = CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x(), m_spMatrixNodeTransform->m_Transform.m_Position.y(), m_spMatrixNodeTransform->m_Transform.m_Position.z());
			Sonic::CGameObject::AddRenderable("Object", m_spModelPoleMid, true);
			Sonic::CGameObject::AddRenderable("Object", m_spModelPoleTop, true);
			Sonic::CGameObject::AddRenderable("Object", m_spModelPoleBtm, true);
			return true;
		}
		bool ProcessMessage(Hedgehog::Universe::Message& in_rMsg, bool in_Flag) override
		{
			if (in_Flag)
			{
				if (std::strstr(in_rMsg.GetType(), "MsgHitEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_playerInsideCollider = true;
					}
					return true;
				}
				if (std::strstr(in_rMsg.GetType(), "MsgLeaveEventCollision") != nullptr)
				{
					const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
					if (in_rMsg.m_SenderActorID == playerContext->m_pPlayer->m_ActorID)
					{
						m_playerInsideCollider = false;
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

			hk2010_2_0::hkpBoxShape* shapeEventTrigger1 = new hk2010_2_0::hkpBoxShape(1, m_setColliderLength, 1);
			AddEventCollision("Object", shapeEventTrigger1, *pColID_PlayerEvent, true, m_spNodeEventCollision);
			return true;
		}
		void AddImpulse(Hedgehog::Math::CVector impulse, bool relative)
		{
			auto context = Sonic::Player::CPlayerSpeedContext::GetInstance();
			if (!relative)
			{
				context->m_Velocity = impulse;
				return;
			}
			else
			{
				context->m_Velocity += impulse;
				return;
			}
		}
		void rotateQuaternionTowardsVector(Eigen::Quaternionf& quat, const Eigen::Vector3f& targetVector) {
			Eigen::Vector3f currentVector = quat * Eigen::Vector3f::UnitZ();
			Eigen::Quaternionf rotation;

			rotation.setFromTwoVectors(currentVector, targetVector);

			quat = rotation * quat;
		}
		CQuaternion faceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, CQuaternion rotation) {
			Eigen::Vector3f currentForward = rotation * Eigen::Vector3f::UnitZ();
			Eigen::Vector3f targetDirection = (targetPoint - position).normalized();

			Eigen::Quaternionf rotationQuaternion;
			rotationQuaternion.setFromTwoVectors(currentForward, targetDirection);

			return rotation = rotationQuaternion * rotation;
		}
		void MoveVerticalA(Sonic::Player::CPlayerSpeedContext* playerContext, int increase)
		{
			m_playerVerticalProgressTarget = m_spMatrixNodeTransform->m_Transform.m_Position.y() + m_playerVerticalProgress + increase;
			m_timerLeap = 0;
			m_isLeaping = true;
			playerContext->ChangeAnimation("Evilsonic_pillar_upH");
		}
		float easeInOutCirc(float x)
		{
			return x < 0.5
				? (1 - sqrt(1 - pow(2 * x, 2))) / 2
				: (sqrt(1 - pow(-2 * x + 2, 2)) + 1) / 2;
		}
		void VerticalMovementRegular(Sonic::Player::CPlayerSpeedContext* playerContext, Sonic::SPadState* inputPtr, float deltaTime)
		{
			if (!m_isLeaping && m_rotationAnimProgress >= 1)
			{
				float stickVerticalValue = inputPtr->LeftStickVertical / 40;

				if (m_playerVerticalProgress > m_setColliderLength && stickVerticalValue > 0)
					stickVerticalValue = 0;

				if (stickVerticalValue < 0)
					stickVerticalValue *= 2;
				m_playerVerticalProgress += stickVerticalValue;
				const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
				playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
				if (stickVerticalValue > 0 && spAnimInfo->m_Name != "Evilsonic_pillar_up")
				{
					playerContext->ChangeAnimation("Evilsonic_pillar_up");
					timerSound += deltaTime;
					if (timerSound > 1)
					{
						timerSound = 0;
						Common::PlaySoundStaticCueName(soundClimb, "es_climbpillar");
					}
				}
				if (stickVerticalValue == 0 && spAnimInfo->m_Name != "Evilsonic_pillar_idle")
				{
					/*isMovingDown = false;
					soundClimb.reset();*/
					playerContext->ChangeAnimation("Evilsonic_pillar_idle");
				}
				if (stickVerticalValue < 0 && spAnimInfo->m_Name != "Evilsonic_pillar_fall")
				{
					/*if (!isMovingDown)
					{
						isMovingDown = true;
						Common::PlaySoundStaticCueName(soundClimb, "es_slide_down2");
					}*/
					playerContext->ChangeAnimation("Evilsonic_pillar_fall");
				}
			}
		}
		void Rotation(Sonic::Player::CPlayerSpeedContext* playerContext, Sonic::SPadState* inputPtr, float deltaTime)
		{
			if (inputPtr->IsTapped(Sonic::eKeyState_DpadRight))
			{
				m_rotationSide++;
				m_playerPoleRotationPrevTarget = m_playerPoleRotationTarget;
				playerContext->ChangeAnimation("Evilsonic_pillar_turnL");
				m_playerPoleRotationTarget += 1.5f;
				m_rotationAnimProgress = 0;
			}
			if (inputPtr->IsTapped(Sonic::eKeyState_DpadLeft))
			{
				m_rotationSide--;
				m_playerPoleRotationPrevTarget = m_playerPoleRotationTarget;
				playerContext->ChangeAnimation("Evilsonic_pillar_turnR");
				m_playerPoleRotationTarget -= 1.5f;
				m_rotationAnimProgress = 0;
			}
			//
			const auto spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			playerContext->m_pPlayer->SendMessageImm(playerContext->m_pPlayer->m_ActorID, spAnimInfo);
			if (spAnimInfo->m_Frame > 10 && (spAnimInfo->m_Name == "Evilsonic_pillar_turnR" || spAnimInfo->m_Name == "Evilsonic_pillar_turnL"))
			{
				m_playerPoleRotation = m_playerPoleRotationTarget;
			}
			if (m_rotationAnimProgress < 1)
			{
				m_rotationAnimProgress += deltaTime * 2;
			}
			//Clamp
			if (m_playerPoleRotationTarget > 6)
				m_playerPoleRotationTarget = 0;

			if (m_playerPoleRotationTarget < -1.5f)
				m_playerPoleRotationTarget = 4.5f;

			if (m_playerPoleRotation >= 6.0f)
			{
				m_playerPoleRotationTarget = 0;
				m_playerPoleRotationPrevTarget = 0;
				m_playerPoleRotation = 0;
			}
			if (m_playerPoleRotation <= -1.5f)
			{
				m_playerPoleRotationTarget = 4.5f;
				m_playerPoleRotationPrevTarget = 4.5f;
				m_playerPoleRotation = 4.5f;
			}

		}
		void SetUpdateParallel(const hh::fnd::SUpdateInfo& in_rUpdateInfo) override
		{
			Sonic::SPadState* inputPtr = &Sonic::CInputState::GetInstance()->m_PadStates[Sonic::CInputState::GetInstance()->m_CurrentPadStateIndex];
			Sonic::Player::CPlayerSpeedContext* playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			isAPressed = inputPtr->IsTapped(Sonic::eKeyState_A);
			isLeftStickUpPressed = inputPtr->IsDown(Sonic::eKeyState_LeftStickUp);
			swingPosition.y() = playerContext->m_spMatrixNode->m_Transform.m_Position.y();
			if (!m_playerInsideCollider)
				ObjectUtility::DoWerehogArmHomingIfClose(swingPosition, 3, in_rUpdateInfo.DeltaTime);
			if (m_playerInsideCollider && (inputPtr->IsTapped(Sonic::eKeyState_B) || playerContext->m_pPlayer->m_StateMachine.GetCurrentState()->m_Name == "EvilArmSwing"))
			{
				playerContext->ChangeState(Sonic::Player::ePlayerSpeedState_JumpHurdle);
				m_playerOnPole = true;
			}
			if (m_playerOnPole)
			{

				DebugDrawText::log("###PLAYER CURRENTLY ON POLE###", 0);
				DebugDrawText::log(std::format("PoleVerticalProgress: {0}", m_playerVerticalProgress).c_str(), 0);
				DebugDrawText::log(std::format("PoleRotation: {0}", m_playerPoleRotation).c_str(), 0);
				DebugDrawText::log(std::format("PoleRotationPrevTarget: {0}", m_playerPoleRotationPrevTarget).c_str(), 0);
				DebugDrawText::log(std::format("PoleRotationTarget: {0}", m_playerPoleRotationTarget).c_str(), 0);
				if (m_isLeaping)
				{
					m_playerVerticalProgress = Common::lerpUnclampedf(m_playerVerticalProgressPrevious, m_playerVerticalProgressTarget, m_timerLeap);
					if (m_timerLeap < 1)
						m_timerLeap += in_rUpdateInfo.DeltaTime * 2;
					else
						m_isLeaping = false;
				}
				else
				{
					m_playerVerticalProgressPrevious = m_playerVerticalProgress;
				}
				if (isLeftStickUpPressed && isAPressed && !m_isLeaping)
				{
					MoveVerticalA(playerContext, 2);
				}
				if (isAPressed && !isLeftStickUpPressed)
				{
					playerContext->m_Velocity = CVector(0, 0, 0);
					AddImpulse(playerContext->m_spMatrixNode->m_Transform.m_Rotation * Hedgehog::Math::CVector(0, 5, -130), true);
					playerContext->ChangeState(Sonic::Player::EPlayerSpeedState::ePlayerSpeedState_Jump);
					playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = false;

					playerContext->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
					m_playerOnPole = false;
					return;
				}

				VerticalMovementRegular(playerContext, inputPtr, in_rUpdateInfo.DeltaTime);
				Rotation(playerContext, inputPtr, in_rUpdateInfo.DeltaTime);
				auto playerPos = playerContext->m_spMatrixNode->m_Transform.m_Position;
				auto targetPos = m_spMatrixNodeTransform->m_Transform.m_Position;
				targetPos.y() = playerPos.y();
				playerContext->m_pStateFlag->m_Flags[Sonic::Player::CPlayerSpeedContext::EStateFlag::eStateFlag_IgnorePadInput] = true;
				auto quat = ObjectUtility::QuaternionFaceTowards(targetPos, playerPos, playerContext->m_spMatrixNode->m_Transform.m_Rotation);

				DebugDrawText::log(std::format("Quat: {0}, {1}, {2}", quat.x(), quat.y(), quat.z()).c_str(), 0);
				playerContext->m_spMatrixNode->m_Transform.SetRotation(quat);
				playerContext->m_spMatrixNode->m_Transform.m_Position = Hedgehog::Math::CVector(m_spMatrixNodeTransform->m_Transform.m_Position.x() + 0.4f, m_spMatrixNodeTransform->m_Transform.m_Position.y() + m_playerVerticalProgress, m_spMatrixNodeTransform->m_Transform.m_Position.z());
				playerContext->m_spMatrixNode->m_Transform.m_Position = ObjectUtility::MoveAroundPivot(playerContext->m_spMatrixNode->m_Transform.m_Position, m_spMatrixNodeTransform->m_Transform.m_Position, Hedgehog::math::CVector(m_playerPoleRotation, 0, m_playerPoleRotation));

			}
		}

		void InitializeEditParam(Sonic::CEditParam& in_rEditParam) override
		{
			in_rEditParam.CreateParamInt(&m_setColliderLength, "Length");
			in_rEditParam.CreateParamBool(&m_setRotation90Snap, "Rotation90Snap");
			in_rEditParam.CreateParamInt(&m_setModelType, "ModelTypeIndex");
		}
		static void RegisterObject();
	};
}