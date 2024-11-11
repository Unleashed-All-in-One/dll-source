#pragma once
#include "../../../../System/DebugMenu/WerehogMenu.h"
#include "../Posture/EvilStateAttackPosture_byList.h"
#include "../../GameObjects/AttackHitbox.h"
namespace SUC::Player::Evil
{
	class MsgChangePlayerAnimation : public hh::fnd::MessageTypeSet
	{
	public:
		HH_FND_MSG_MAKE_TYPE(0x016802A8);
		Hedgehog::Base::CSharedString m_AnimationName;
		uint32_t m_Field14;
		uint32_t m_Field18;
		uint32_t m_Field1C;
		uint32_t m_Field20;
		uint32_t m_Field24;
		uint32_t m_Field28;

		MsgChangePlayerAnimation(const Hedgehog::Base::CSharedString& in_AnimationName, uint32_t in_Field14 = 1,
			uint32_t in_Field18 = 0, uint32_t in_Field1C = 0, uint32_t in_Field20 = 0,
			uint32_t in_Field24 = 1, uint32_t in_Field28 = 0) : m_AnimationName(in_AnimationName),
			                                                    m_Field14(in_Field14), m_Field18(in_Field18),
			                                                    m_Field1C(in_Field1C), m_Field20(in_Field20),
			                                                    m_Field24(in_Field24), m_Field28(in_Field28)
		{
		}
		
	};
	class CStateAttackAction_byList : public Sonic::Player::CPlayerSpeedContext::CStateSpeedBase
	{
		int m_LastActionIndex;
		float m_LastFrame;
		float m_LastTriggerIndex;
		Motion m_CurrentMotion;
		//something i like to call a little hack
		CVector ms_InitialVelocity;
		CVector ms_AlteredVelocity;
		std::string m_Posture;
		static constexpr float ms_DecelerationForce = 0.65f;
		static constexpr float ms_AccelerationForce = 1.4285715f; //pulled from unleashed
		std::vector<boost::shared_ptr<CAttackHitbox>> collision;
		SharedPtrTypeless sound;
		const char* prevAnim;
		SharedPtrTypeless genericEffect;
		float m_MotionMoveSpeedRatioNew;
	public:
		static constexpr const char* ms_pStateName = "Evil_AttackAction_byList";
		void ChangeAnimation(const char* in_Name)
		{
			const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
			const auto spAnimInfo = boost::make_shared<MsgChangePlayerAnimation>(in_Name);
			playerContext->m_pPlayer->m_spAnimationStateMachine->ChangeState(in_Name);
		}
		CVector GetForward()
		{
			auto context = GetContext();
			return (context->m_spMatrixNode->m_Transform.m_Rotation * Eigen::Vector3f::UnitZ());
		}
		void GenerateHitbox(CollisionParam param, int index)
		{
			auto newHitbox = boost::make_shared<CAttackHitbox>();
			collision.push_back(newHitbox);
			Sonic::CGameDocument::GetInstance()->AddGameObject(newHitbox);
			switch (param.BoneType)
			{
			case CollisionBoneType::RHand:
			{
				newHitbox->BindToBone("Hand_R");
				break;
			}
			case CollisionBoneType::LHand:
			{
				newHitbox->BindToBone("Hand_L");
				break;
			}
			case CollisionBoneType::RLeg:
			{
				newHitbox->BindToBone("Toe_R");
				break;
			}
			case CollisionBoneType::LLeg:
			{
				newHitbox->BindToBone("Toe_L");
				break;
			}
			case CollisionBoneType::Hips:
			{
				newHitbox->BindToBone("Hips");
				break;
			}
			case CollisionBoneType::Head:
			{
				newHitbox->BindToBone("Head");
				break;
			}
			case CollisionBoneType::MiddleLeg:
			{
				newHitbox->BindToBone("Toe_R");
				break;
			}
			}
			newHitbox->m_TimerMax = (param.EndFrame - param.StartFrame) / 30;
			newHitbox->hitboxName = std::format("{0}{1}", EvilAttackConfiguration::GetBoneNameFromCollisionParam((int)param.BoneType), index + 1);
		}
		void KillHitbox(std::string name)
		{
			for (size_t i = 0; i < collision.size(); i++)
			{
				if (collision[i]->hitboxName == name)
				{
					auto& col = collision[i];
					col->SendMessage(col->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
					collision.erase(collision.begin() + i);
					return;
				}
			}
		}
		void EnterState() override
		{
			auto context = GetContext();
			m_LastFrame = -1;
			m_LastActionIndex = 0;
			m_LastTriggerIndex = 0;
			ms_InitialVelocity = CVector(0, 0, 0);
			ms_AlteredVelocity = ms_InitialVelocity;
			SONIC_CLASSIC_CONTEXT->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			ChangeAnimation(EvilGlobal::GetStateNameFromTable(EvilGlobal::s_LatestAttackName).c_str());
			SONIC_CLASSIC_CONTEXT->m_Field168 = true;
		}
		bool HasHitboxBeenSpawned(std::string name)
		{
			for (size_t i = 0; i < collision.size(); i++)
			{
				if (collision[i]->hitboxName == name)
					return true;
			}
			return false;
		}
		void Reset()
		{
			SONIC_CLASSIC_CONTEXT->m_Field168 = false;
			for (size_t i = 0; i < collision.size(); i++)
			{
				collision.at(i)->SendMessage(collision.at(i)->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
			}

			DebugDrawText::log("RESET", 5);
			collision.clear();
			LeaveState();
		}
		void LeaveState() override
		{
			auto context = GetContext();
			context->m_Velocity = CVector::Identity();

		}
		void HitboxRoutine(float in_CurrentAnimFrame)
		{
			for (size_t i = 0; i < m_CurrentMotion.Collision.BoneInfo.size(); i++)
			{
				std::string hitboxName = std::format("{0}{1}", EvilAttackConfiguration::GetBoneNameFromCollisionParam((int)m_CurrentMotion.Collision.BoneInfo[i].BoneType), i + 1);

				if (in_CurrentAnimFrame >= m_CurrentMotion.Collision.BoneInfo[i].StartFrame && !HasHitboxBeenSpawned(hitboxName))
				{
					GenerateHitbox(m_CurrentMotion.Collision.BoneInfo[i], i);
				}
				if (in_CurrentAnimFrame >= m_CurrentMotion.Collision.BoneInfo[i].EndFrame && HasHitboxBeenSpawned(hitboxName))
				{
					KillHitbox(hitboxName);
				}

			}
		}
		void FXRoutine(float in_CurrentAnimFrame, const Motion& in_Motion)
		{
			bool m_EndEarly = false;
			for (size_t i = m_LastTriggerIndex; i < in_Motion.TriggerInfos.Resources.size(); i++)
			{
				if (m_EndEarly)
					break;
				if (in_CurrentAnimFrame >= in_Motion.TriggerInfos.Resources[i].Frame.Start)
				{
					for (size_t x = 0; x < in_Motion.ResourceInfos.Resources.size(); x++)
					{
						if (in_Motion.ResourceInfos.Resources[x].ID == in_Motion.TriggerInfos.Resources[i].ResourceID)
						{
							m_LastTriggerIndex = i + 1;
							if (in_Motion.ResourceInfos.Resources[x].Type == ResourceType::CSB)
							{
								if (!in_Motion.ResourceInfos.Resources[x].Params.Cue.empty())
								{
									Common::PlaySoundStaticCueName(sound, in_Motion.ResourceInfos.Resources[x].Params.Cue.c_str());
									m_EndEarly = true;
									break;
								}
							}
							if (in_Motion.ResourceInfos.Resources[x].Type == ResourceType::Effect)
							{
								//genericEffect
								auto bone = SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spCharacterModel->GetNode(in_Motion.TriggerInfos.Resources[x].NodeName.c_str());
								if (!genericEffect)
									Common::fCGlitterCreate(SONIC_CLASSIC_CONTEXT->m_pPlayer->m_spContext.get(), genericEffect, &bone, in_Motion.ResourceInfos.Resources[x].Params.FileName.c_str(), 1);
							}
						}
					}
				}
			}
		}
		void UpdateState() override
		{
			auto triggers = m_CurrentMotion.TriggerInfos;
			auto resources = m_CurrentMotion.ResourceInfos;
			MoveRatioHelper* m_CurrentMoveRatioXZ = nullptr;
			MoveRatioHelper* m_CurrentMoveRatioY = nullptr;
			bool skip = false;
			auto context = GetContext();

			m_CurrentMotion = EvilGlobal::GetMotionFromName(EvilGlobal::s_LatestAttackName);
			//if(m_CurrentMotion.IsGravity)
			//{
			//	context->m_pPlayer->m_PostureStateMachine.ChangeState("Standard");
			//}
			//else
			//{
			//}
			boost::shared_ptr<Sonic::Message::MsgGetAnimationInfo> spAnimInfo = boost::make_shared<Sonic::Message::MsgGetAnimationInfo>();
			context->m_pPlayer->SendMessageImm(context->m_pPlayer->m_ActorID, spAnimInfo);
			DebugDrawText::log(std::format("CSTATEATTACKACTION_AnimFrame = {0}", spAnimInfo->m_Frame).c_str(), 0);
			if (spAnimInfo->m_Frame <= m_LastFrame)
			{
			}
			//if it isnt playing the anim for some reason, force it to play now
			

			//if the current anim is playing and the frame of it is still the same after 2 frames, it means the animation has ended, change states.
			if (std::strstr(spAnimInfo->m_Name.c_str(), EvilGlobal::GetStateNameFromTable(EvilGlobal::s_LatestAttackName).c_str()) != nullptr && spAnimInfo->m_Frame == m_LastFrame)
			{
				SONIC_CLASSIC_CONTEXT->ChangeState("Stand");
				Reset();

				//EvilGlobal::shockwaveGameObject->SendMessage(EvilGlobal::shockwaveGameObject->m_ActorID, boost::make_shared<Sonic::Message::MsgKill>());
				return;
			}


			//this is probably super overkill and could just be done using a for loop with no checks
			FXRoutine(spAnimInfo->m_Frame, m_CurrentMotion);
			HitboxRoutine(spAnimInfo->m_Frame);

			for (auto moveSpeedRatioH : m_CurrentMotion.MotionMoveSpeedRatio_H)
			{
				if(spAnimInfo->m_Frame >= moveSpeedRatioH.FrameStart)
				{
					m_CurrentMoveRatioXZ = &moveSpeedRatioH;
					break;
				}
			}
			for (auto moveSpeedRatioH : m_CurrentMotion.MotionMoveSpeedRatio_H_Y)
			{
				if (spAnimInfo->m_Frame >= moveSpeedRatioH.FrameStart)
				{
					m_CurrentMoveRatioY = &moveSpeedRatioH;
					break;
				}
			}
			//Apply basic velocity from attack
			if(m_CurrentMotion.MoveType == Motion::EMoveType::Default || m_CurrentMotion.MoveType == Motion::EMoveType::IngAutoTarget)
			{
				if (spAnimInfo->m_Frame >= m_CurrentMotion.MotionSpeed_FirstFrame && spAnimInfo->m_Frame < m_CurrentMotion.MotionSpeed_MiddleFrame)
				{
					if (m_CurrentMoveRatioXZ != nullptr)
					{
						float velocity = m_CurrentMoveRatioXZ->FrameValue;
						//Prevent division or multiplication by 0
						if (abs(velocity) != 0 && !std::isnan(velocity))
						{
							ms_AlteredVelocity = SONIC_CLASSIC_CONTEXT->GetFrontDirection() * (velocity);
							ImGuiMenu::WerehogMenu::s_MoveSpeedVelocity = velocity;
							context->m_Velocity = ms_AlteredVelocity;
						}
					}

					if (m_CurrentMoveRatioY != nullptr)	ms_AlteredVelocity.y() = m_CurrentMoveRatioY->FrameValue;
				}
				else
					context->m_Velocity = ms_InitialVelocity;
			}
			//WIP Auto target attacks
			if(m_CurrentMotion.MoveType == Motion::EMoveType::IngAutoTarget)
			{
				if (spAnimInfo->m_Frame >= m_CurrentMotion.AutoTargetEndFrame )
				{
					//30.0f is AutoTargetLength
					ms_AlteredVelocity += SONIC_CLASSIC_CONTEXT->GetFrontDirection() * (m_CurrentMotion.AutoTargetPower * 2.0f);
				}				
			}

			ImGuiMenu::WerehogMenu::s_AppliedVelocity = ms_AlteredVelocity;
			if (spAnimInfo->m_Frame >= m_CurrentMotion.MotionSpeed_MiddleFrame)
			{
				context->m_Velocity = ms_AlteredVelocity;
			}
			ms_AlteredVelocity = CVector(ms_AlteredVelocity.x() * ms_DecelerationForce, ms_AlteredVelocity.y(), ms_AlteredVelocity.z() * ms_DecelerationForce);
			context->m_Velocity = ms_AlteredVelocity;
			//if (m_CurrentMotion.MotionMoveSpeedRatio_H.size() > m_LastActionIndex)
			//{
			//	if ((spAnimInfo->m_Frame == m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameStart	)
			//		/*|| m_CurrentMotion.MotionMoveSpeedRatioFrameStart.at(m_LastActionIndex) == -1*/)
			//	{
			//		DebugDrawText::log(std::format("CSTATEATTACKACTION_MotionMoveSpeedRatioFrameStart = {0} ind:{1}", spAnimInfo->m_Frame, m_LastActionIndex).c_str(), 5);
			//		ms_AlteredVelocity = ms_InitialVelocity + (GetForward() * m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameValue);
			//		ms_AlteredVelocity = CVector(ms_AlteredVelocity.x(), ms_AlteredVelocity.y() + m_CurrentMotion.MotionMoveSpeedRatio_H.at(m_LastActionIndex).FrameValue_Y, ms_AlteredVelocity.z());
			//		m_LastActionIndex++;
			//	}
			//}

			m_LastFrame = spAnimInfo->m_Frame;

		}
	};
}