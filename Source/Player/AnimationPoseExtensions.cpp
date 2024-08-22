#include "AnimationPoseExtensions.h"

#include "Evil/EvilGlobal.h"
#include "Evil/AnimationExtension/CeramicSA_AnimatorPose.h"
namespace BBExt
{
	//TY ceramic
	void ProceduralData::SetUpdateFunction(void* functionPointer)
	{
		UpdateProcedural = (FPtrUpdateProcedural)functionPointer;
	}
	void CAnimationPoseInit_Ctor(BBExt::Animation::CAnimationPose* pose)
	{
		ProceduralData* procData = ProceduralData::Get(pose);
		procData->m_pObject = nullptr;
		procData->UpdateProcedural = nullptr;

	}
	ProceduralData* ProceduralData::Get(BBExt::Animation::CAnimationPose* pose)
	{
		return &reinterpret_cast<CAnimationPose_Alternate*>(pose)->m_pMap->procData;
	}
}
namespace SUC::Player
{
	//You can loop through the skeleton using blender's console to get bone indices
	enum EEvilBone
	{
		Reference = 1,
		Hips = 2,
		Spine = 3,
		Spine1 = 4,
		Neck = 5,
		Head = 6,
		Ear_L = 7,
		Ear_R = 8,
		Needle_U_L = 9,
		Needle1_U_L = 10,
		Needle_B_L = 11,
		Needle1_B_L = 12,
		Needle_U_C = 13,
		Needle1_U_C = 14,
		Needle_B_C = 15,
		Needle1_B_C = 16,
		Needle_U_R = 17,
		Needle1_U_R = 18,
		Needle_B_R = 19,
		Needle1_B_R = 20,
		Brow_Reference = 21,
		Brow_C = 22,
		Brow1_L = 23,
		Brow2_L = 24,
		Brow1_R = 25,
		Brow2_R = 26,
		EyeLid_Reference = 27,
		EyeLidUp_C = 28,
		EyeLidUp1_L = 29,
		EyeLidUp2_L = 30,
		EyeLidUp3_L = 31,
		EyeLidUp1_R = 32,
		EyeLidUp2_R = 33,
		EyeLidUp3_R = 34,
		EyeLidLow_C = 35,
		EyeLidLow1_L = 36,
		EyeLidLow1_R = 37,
		Mouth_Reference = 38,
		JawUp = 39,
		LipUp_C = 40,
		Lip1_L = 41,
		Lip2_L = 42,
		Lip3_L = 43,
		TeethUp = 44,
		Lip1_R = 45,
		Lip2_R = 46,
		Lip3_R = 47,
		JawLow = 48,
		LipLow_C = 49,
		Lip4_L = 50,
		Lip5_L = 51,
		TeethLow = 52,
		Lip4_R = 53,
		Lip5_R = 54,
		Cheek_L = 55,
		Cheek_R = 56,
		Nose1 = 57,
		Nose2 = 58,
		Model_EyeBoad = 59,
		Shoulder_L = 60,
		Arm01_L = 61,
		Arm02_L = 62,
		Arm03_L = 63,
		Arm04_L = 64,
		Arm05_L = 65,
		Arm06_L = 66,
		Arm07_L = 67,
		Arm08_L = 68,
		Arm09_L = 69,
		Arm10_L = 70,
		Arm11_L = 71,
		Arm12_L = 72,
		Hand_L_Reference = 73,
		Hand_L = 74,
		Thumb1_L = 75,
		Thumb2_L = 76,
		Thumb3_L = 77,
		Index1_L = 78,
		Index2_L = 79,
		Index3_L = 80,
		Middle1_L = 81,
		Middle2_L = 82,
		Middle3_L = 83,
		Ring1_L = 84,
		Ring2_L = 85,
		Ring3_L = 86,
		Pinky1_L = 87,
		Pinky2_L = 88,
		Pinky3_L = 89,
		Arm12Sub_L = 90,
		Arm11Sub_L = 91,
		Arm10Sub_L = 92,
		Arm09Sub_L = 93,
		Arm08Sub_L = 94,
		Arm07Sub_L = 95,
		Arm06Sub_L = 96,
		Arm05Sub_L = 97,
		Arm04Sub_L = 98,
		Arm03Sub_L = 99,
		Arm02Sub_L = 100,
		Arm01Sub_L = 101,
		Shoulder_R = 102,
		Arm01_R = 103,
		Arm02_R = 104,
		Arm03_R = 105,
		Arm04_R = 106,
		Arm05_R = 107,
		Arm06_R = 108,
		Arm07_R = 109,
		Arm08_R = 110,
		Arm09_R = 111,
		Arm10_R = 112,
		Arm11_R = 113,
		Arm12_R = 114,
		Hand_R_Reference = 115,
		Hand_R = 116,
		Thumb1_R = 117,
		Thumb2_R = 118,
		Thumb3_R = 119,
		Index1_R = 120,
		Index2_R = 121,
		Index3_R = 122,
		Middle1_R = 123,
		Middle2_R = 124,
		Middle3_R = 125,
		Ring1_R = 126,
		Ring2_R = 127,
		Ring3_R = 128,
		Pinky1_R = 129,
		Pinky2_R = 130,
		Pinky3_R = 131,
		Arm12Sub_R = 132,
		Arm11Sub_R = 133,
		Arm10Sub_R = 134,
		Arm09Sub_R = 135,
		Arm08Sub_R = 136,
		Arm07Sub_R = 137,
		Arm06Sub_R = 138,
		Arm05Sub_R = 139,
		Arm04Sub_R = 140,
		Arm03Sub_R = 141,
		Arm02Sub_R = 142,
		Arm01Sub_R = 143,
		WingL = 144,
		WingR = 145,
		Chest = 146,
		Thigh_L = 147,
		ThighSub_L = 148,
		Calf_L = 149,
		CalfSub_L = 150,
		Foot_L = 151,
		Toe_L = 152,
		Socks_L = 153,
		Knee_L = 154,
		Thigh_R = 155,
		ThighSub_R = 156,
		Calf_R = 157,
		CalfSub_R = 158,
		Foot_R = 159,
		Toe_R = 160,
		Socks_R = 161,
		Knee_R = 162,
		Tail = 163,
		Help01 = 164,
		Help02 = 165,
		Help03 = 166,
		Model_Body = 167,
		Model_Socks = 168,
		Fur_Point = 169,
		Fur_All = 170,
	};
	
	Eigen::Vector3f PointToLocalSpace(const Eigen::Vector3f& point,
		const Eigen::Quaternionf& rotation,
		const Eigen::Vector3f& position)
	{
		// Get the world space vector from the origin to the point.
		Eigen::Vector3f offset = point - position;

		// Rotate this offset by the inverse of the rotation to get it into local space.
		Eigen::Vector3f local = rotation.inverse() * offset;

		return local;
	}
	void WerehogArmSwing(BBExt::Animation::CAnimationPose* pose, Sonic::Player::CPlayerSpeed* player)
	{
		using namespace hh::math;
		auto context = player->GetContext();
		//////////////
		// Code start
		//////////////
		//-----------------------------------------

		// We don't want to do anything if we're not in the right state.
		const auto state = player->m_StateMachine.GetCurrentState().get();
		if (!state)
			return;
		if (Evil::EvilGlobal::s_AllowArmMovL)
		{
			BBExt::Animation::hkQsTransform* tBone = pose->m_pAnimData->m_TransformArray.GetIndex(EEvilBone::Shoulder_L);

			tBone->m_Position = context->m_spMatrixNode->m_Transform.m_Position - Evil::EvilGlobal::s_ArmPosL;
		}
		if (Evil::EvilGlobal::s_AllowArmMovR)
		{			
			auto increment = PointToLocalSpace(Evil::EvilGlobal::s_ArmPosR, context->m_spMatrixNode->m_Transform.m_Rotation, context->m_spMatrixNode->m_Transform.m_Position) / (11);

			pose->m_pAnimData->m_TransformArray.GetIndex((int)EEvilBone::Arm12Sub_R)->m_Rotation = CQuaternion::Identity();
			//WorldToLocalScale(context->m_pPlayer->m_spCharacterModel->GetNode("Shoulder_R")->m_WorldMatrix, context->m_pPlayer->m_spCharacterModel->GetNode("Shoulder_R")->m_LocalMatrix, CVector(1, 1, 1));
			for (int i = 0; i < 11; ++i)
			{
				BBExt::Animation::hkQsTransform* tBoneS = pose->m_pAnimData->m_TransformArray.GetIndex((int)EEvilBone::Arm12Sub_R + i);
				tBoneS->m_Rotation = CQuaternion::Identity();
				tBoneS->m_Position = increment;
				tBoneS->m_Scale = CVector(1, 1, 1);;
			}
			pose->m_pAnimData->m_TransformArray.GetIndex((int)EEvilBone::Shoulder_R)->m_Rotation = CQuaternion::Identity();
			pose->m_pAnimData->m_TransformArray.GetIndex((int)EEvilBone::Hand_R_Reference)->m_Position = PointToLocalSpace(Evil::EvilGlobal::s_ArmPosR, context->m_spMatrixNode->m_Transform.m_Rotation, context->m_spMatrixNode->m_Transform.m_Position);
			//WorldToLocalScale(context->m_pPlayer->m_spCharacterModel->GetNode("Shoulder_R")->m_WorldMatrix, context->m_pPlayer->m_spCharacterModel->GetNode("Shoulder_R")->m_LocalMatrix, CVector(1, 1, 1));
			for (int i = 0; i < 11; ++i)
			{
				BBExt::Animation::hkQsTransform* tBoneS = pose->m_pAnimData->m_TransformArray.GetIndex((int)EEvilBone::Shoulder_R + i);
				tBoneS->m_Rotation = CQuaternion::Identity();
				tBoneS->m_Scale = CVector(1, 1, 1);;
			}
		}
	}
	void CAnimationPoseInit_AddCallbackCustom(app::Player::CPlayerSpeed* player)
	{
		auto m_spAnimatorPoseCustom = (BBExt::Animation::CAnimationPose*)player->m_spAnimationPose.get();
		auto m_CustomPose = &reinterpret_cast<BBExt::CAnimationPose_Alternate*>(m_spAnimatorPoseCustom)->m_pMap->procData;
		m_CustomPose->m_pObject = player;
		m_CustomPose->SetUpdateFunction(WerehogArmSwing);
	}
	HOOK(void, __fastcall, ClassicSonic_InitializeRenderables, 0x00DE93A0, app::Player::CPlayerSpeed* This, void*, void* a2, int a3)
	{
		originalClassicSonic_InitializeRenderables(This, nullptr, a2, a3);
		CAnimationPoseInit_AddCallbackCustom(This);
	}
	HOOK(void*, __fastcall, CAnimationPoseCtor_Common, 0x006CB140, BBExt::Animation::CAnimationPose* This, void*, void* a2, void* a3)
	{
		void* result = originalCAnimationPoseCtor_Common(This, nullptr, a2, a3);
		BBExt::CAnimationPoseInit_Ctor(This);
		return result;
	}
	HOOK(void*, __fastcall, CAnimationPoseCtor_Msn, 0x006CA910, BBExt::Animation::CAnimationPose* This, void*, void* a2, void* a3, void* a4)
	{
		void* result = originalCAnimationPoseCtor_Msn(This, nullptr, a2, a3, a4);
		BBExt::CAnimationPoseInit_Ctor(This);
		return result;
	}
	HOOK(void, __fastcall, CustomAnimationPose_UpdateAnimation, 0x6CC950, BBExt::Animation::CAnimationPose* This)
	{
		originalCustomAnimationPose_UpdateAnimation(This);

		BBExt::ProceduralData* procData = BBExt::ProceduralData::Get(This);
		if (procData->UpdateProcedural == nullptr || procData->m_pObject == nullptr)
			return;

		procData->UpdateProcedural(This, procData->m_pObject);
	}
	void AnimationPoseExtensions::RegisterHooks()
	{
		WRITE_MEMORY(0x006CB29D, uint8_t, sizeof(BBExt::CAnimationPose_Alternate::Map));
		INSTALL_HOOK(CAnimationPoseCtor_Common);
		INSTALL_HOOK(CAnimationPoseCtor_Msn);
		INSTALL_HOOK(ClassicSonic_InitializeRenderables);
		INSTALL_HOOK(CustomAnimationPose_UpdateAnimation);
	}
}