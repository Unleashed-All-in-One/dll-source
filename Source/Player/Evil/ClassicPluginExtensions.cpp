#include "ClassicPluginExtensions.h"

#include "../Evil/StateMachine/Posture/EvilPostureIntro.h"
#include "../Evil/StateMachine/Posture/EvilStateAttackPosture_byList.h"
#include "../Evil/StateMachine/State/EvilStateAttackAction_byList.h"
#include "../Evil/StateMachine/State/EvilStateArmSwing.h"
#include "../Evil/StateMachine/State/EvilStateRunningJump.h"
#include "../Evil/StateMachine/State/EvilStateDamageNormal.h"
#include "../Evil/StateMachine/State/EvilStateWalkSlowE.h"
#include "StateMachine/State/EvilStateRunE.h"
#include "StateMachine/State/EvilStateWalkE.h"

namespace SUC::Player::Evil
{
	//REPLACE WITH PARAMS FROM EVIL.PRM.XML
	

		float CEvilBasicStateBase::GetPadIntensity()
		{
			Sonic::SPadState* input = InputSingleton;
			CVector m_InputVec = CVector(input->LeftStickHorizontal, input->LeftStickVertical, 0);
			return abs(System::CVectorHelper::Length(m_InputVec));
		}

		
		void CEvilBasicStateBase::PadMovement() const
		{
			auto context = SONIC_CLASSIC_CONTEXT;
			auto input = InputSingleton;
			auto camera = Sonic::CGameDocument::GetInstance()->GetWorld()->m_pMember->m_spCamera;
			CVector cameraForward = camera->m_MyCamera.m_Direction.normalized();
			CVector cameraUp = camera->m_UpVector;
			CVector cameraRight = cameraUp.cross(cameraForward).normalized();
			CVector2 m_InputVec = CVector2(input->LeftStickHorizontal, input->LeftStickVertical);

			CVector velocity = (cameraForward * (m_InputVec.y())) + (cameraRight * (-m_InputVec.x()));
			velocity *= (ms_MovementSpeed * 2);
			velocity *= 30;
			velocity *= GetDeltaTime();

			EvilGlobal::s_AnimationSpeed =1000;
			SUC::Player::Evil::EvilGlobal::s_AnimationSpeed = m_InputVec.squaredNorm();
			if (m_InputVec.norm() != 0)
			{
				CQuaternion newrot = System::CQuaternionHelper::LookRotation(System::CVectorHelper::normalizedSafe(velocity));

				context->m_HorizontalRotation = newrot;
			}
			velocity.y() = 0;
			context->SetHorizontalVelocity(velocity);
			context->SetHorizontalVelocityClearChanged(velocity);
		};
		void CEvilBasicStateBase::UpdateState()
		{
			PadMovement();
		};
	
	//MOVE TO SEPARATE FILES
	class PostureStandardReverse : public Hedgehog::Universe::CStateMachineBase::CStateBase
	{public:
		BB_INSERT_PADDING(0x10);
		static constexpr const char* ms_pStateName = "Revcerse";
		void SetGravity(void* context, float deltatime)
		{
			static uint32_t func = 0xE59C30;
			__asm
			{
				push 0.0f
				push deltatime
				mov eax, context
				call func
			}
		}
		void Clean(void* context)
		{
			static uint32_t func = 0x00E4F100;
			__asm
			{
				
				mov esi, context
				call func
			}
		}
		//void __stdcall SonicMovementRoutine(CTempState *This, Hedgehog::Math::CVector *Velocity)
		FUNCTION_PTR(void, __stdcall, SonicMovementRoutine, 0x00E32180, void* This, Hedgehog::Math::CVector* in_Velocity);
		//int __stdcall PostMovement(CSonicContext *sonicContext)
		FUNCTION_PTR(int, __stdcall, PostMovement, 0x00E63530, void* SonicContext);
		//sub_119DB00(int this)
		FUNCTION_PTR(int, __thiscall, StopUpdate, 0x119DB00, void* SonicContext);
		void UpdateState() override
		{
			
			




			////SonicMovementRoutine(this, &context->m_Velocity);
			////PostMovement(context);
			//context->m_Velocity -= CVector(0, 1, 0);
			//context->m_VelocityChanged = true;
			//SetGravity(context, Project::GetDeltaTime());
			//if(context->m_VelocityChanged)
			//Clean(context);
			//context->m_HorizontalOrVerticalVelocityChanged = true;
			//context->HandleHorizontalOrVerticalVelocityChanged();
			//context->m_spMatrixNode->NotifyChanged();
			//
			//auto v10 = context->m_HorizontalVelocity;
			//
			//if (context->m_HorizontalOrVerticalVelocityChanged)
			//{
			//	context->m_Velocity = context->GetVerticalVelocity() + context->GetHorizontalVelocity();
			//	context->m_HorizontalOrVerticalVelocityChanged = 0;
			//	context->m_VelocityChanged = 0;
			//}
			//float vec = *(float*)(*(uint32_t*)(context) + 4352);
			//context->m_Velocity = context->m_Velocity + v10;
			//context->m_VelocityChanged = 1;
			//context->m_HorizontalOrVerticalVelocityChanged = 0;
			//context->m_spMatrixNode->NotifyChanged();
			////StopUpdate(this);
		}
	};
	bool m_PlayedCameraIntro;
	HOOK(int, __fastcall, ProcMsgRestart, 0xE76810, int* This, void* Edx, int* a2)
	{
		m_PlayedCameraIntro = false;
		return originalProcMsgRestart(This, Edx, a2);
	}
	HOOK(char, __stdcall, SetupPlayerStageIntro, 0xDFCE30, void* StateCrouching)
	{
		if (IS_CLASSIC_SONIC)
		{
			//2 = readygo crouch
			//*((DWORD*)a2 + 1329) = 2;
			if (!m_PlayedCameraIntro)
			{
				Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_PostureStateMachine.ChangeState<Evil::CStartWerehogPosture>();
				m_PlayedCameraIntro = true;
			}
			return 1;
		}
		else
			return originalSetupPlayerStageIntro(StateCrouching);
	}
	void RegisterCustomStates(Sonic::Player::CPlayerSpeedContext* context)
	{
		static bool added = false;
		if (added) return;

		if (!added)
		{
			//Postures
			context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<Evil::CStartWerehogPosture>();
			context->m_pPlayer->m_PostureStateMachine.RegisterStateFactory<Evil::CStateAttackAction_byList_Posture>();
			//context->m_pPlayer->m_StateMachine.RegisterStateFactory<PostureStandardReverse>();

			//States
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateAttackAction_byList>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateArmSwing>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateRunningJump>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<Evil::CStateDamageNormal>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<EvilStateWalkSlowE>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<EvilStateWalkE>();
			context->m_pPlayer->m_StateMachine.RegisterStateFactory<EvilStateRunE>();
			//added = true;
		}
	}
	HOOK(void*, __fastcall, CPlayerCreator_CCreate_Execute, 0x00D96110, void* This)
	{
		void* result = originalCPlayerCreator_CCreate_Execute(This);
		RegisterCustomStates(Sonic::Player::CPlayerSpeedContext::GetInstance());
		return result;
	}
	void __declspec(naked) ASM_ClassicPlayStageIntro()
	{
		static uint32_t RedRingCollectedCheckReturnAddress = 0x00DC69B6;
		static uint32_t sub_E71A50 = 0xE71A50;
		static uint32_t sub_DFCE30 = 0xDFCE30;
		__asm
		{
			call[sub_E71A50]
			push    esi
			call[sub_DFCE30]
			jmp[RedRingCollectedCheckReturnAddress]
		}
	}
	
	void ClassicPluginExtensions::RegisterHooks()
	{
		INSTALL_HOOK(ProcMsgRestart);
		INSTALL_HOOK(SetupPlayerStageIntro);
		INSTALL_HOOK(CPlayerCreator_CCreate_Execute);

		//Classic doesn't use the function that Modern uses to play the intros
		//which is why Classic doesn't have any "READY GO" or anything of the sort
		WRITE_JUMP(0x00DC69B1, ASM_ClassicPlayStageIntro);
	}
}
