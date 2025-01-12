#pragma once

#define PI 3.141592
#define PI_F 3.141592f
#include "../Source/Project.h"
uint32_t* const BACKBUFFER_WIDTH = (uint32_t*)0x1DFDDDC;
uint32_t* const BACKBUFFER_HEIGHT = (uint32_t*)0x1DFDDE0;

/// <summary>
/// Used to get the world position of UI elements
/// </summary>
class MsgGetHudPosition : public Hedgehog::Universe::MessageTypeGet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x016804EC);
	Hedgehog::Math::CVector m_Position;
	uint32_t m_Field20;
	float m_Field24;
	uint32_t m_Field28;
};
/// <summary>
/// Sets the pinball hud's score text and ui type
///	m_flag does something with enabling /disabling ui nodes, score is score
/// </summary>
class MsgSetPinballHud : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01680500);
	uint32_t m_Flag;
	uint32_t m_Score;
	INSERT_PADDING(0x8);
};
/// <summary>
/// Gives Sonic the status effects from the item capsules (i think)
/// </summary>
class MsgGetItemType : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01680E68);
	uint32_t m_Type;
	bool m_Field14;
	INSERT_PADDING(0xB);
	Hedgehog::Math::CVector m_Field20;
	INSERT_PADDING(0x4);
};
/// <summary>
/// Spawns or kills the Homing Attack trail
///	if m_enabled is true, it spawns, if 0, it kills
/// </summary>
class MsgRequestLocusEffect : public Hedgehog::Universe::MessageTypeSet
{
public:
	HH_FND_MSG_MAKE_TYPE(0x01680360);
	uint32_t m_Enabled;
};
/// <summary>
/// Used to give an impulse (velocity vector) to objects
/// </summary>
class MsgApplyImpulse : public Hedgehog::Universe::MessageTypeSet
{
public:
	enum EImpulseType : uint32_t
	{
		eImpulseType_None = 0,
		eImpulseType_DashPanel = 1,
		eImpulseType_Case2 = 2,
		eImpulseType_Case3 = 3,
		eImpulseType_Case4 = 4,
		eImpulseType_Case5 = 5,
		eImpulseType_JumpBoard = 6,
		eImpulseType_JumpBoardSpecial = 7,
		eImpulseType_DashRing = 8,
		eImpulseType_DashRingR = 9,
		eImpulseType_LookBack = 10,
		eImpulseType_HomingAttackAfter = 11,
		eImpulseType_BoardJumpBoard = 12,
		eImpulseType_Case13 = 13,
		eImpulseType_BoardJumpAdlibTrickA = 14,
		eImpulseType_BoardJumpAdlibTrickB = 15,
		eImpulseType_BoardJumpAdlibTrickC = 16
	};
	HH_FND_MSG_MAKE_TYPE(0x01680D84);
	Hedgehog::Math::CVector m_Position; // 0x10
	float m_Field1C;
	Hedgehog::Math::CVector m_Impulse; // 0x20
	float m_Field24;
	float m_OutOfControlTime; // 0x30
	float m_Field34;
	EImpulseType m_ImpulseType; // 0x38
	float m_KeepVelocityTime; // 0x3C
	bool m_RelativeToObject; // 0x40 if false, add impulse direction relative to Sonic
	bool m_SnapToPosition; // 0x41 snap Sonic to m_position
	bool m_Field42;
	bool m_Field43;
	bool m_Field44;
	bool m_InterpolatePath; // 0x45 linked to 80
	INSERT_PADDING(0xA + sizeof(Eigen::Vector3f) + 0x14);
};



enum StageMissionType : uint32_t
{
	SMT_ghz100 = 0x00,
	SMT_ghz200 = 0x01,
	SMT_cpz100 = 0x02,
	SMT_cpz200 = 0x03,
	SMT_ssz100 = 0x04,
	SMT_ssz200 = 0x05,
	SMT_sph100 = 0x06,
	SMT_sph200 = 0x07,
	SMT_cte100 = 0x08,
	SMT_cte200 = 0x09,
	SMT_ssh100 = 0x0A,
	SMT_ssh200 = 0x0B,
	SMT_csc100 = 0x0C,
	SMT_csc200 = 0x0D,
	SMT_euc100 = 0x0E,
	SMT_euc200 = 0x0F,
	SMT_pla100 = 0x10,
	SMT_pla200 = 0x11,
	SMT_cnz100 = 0x12,
	SMT_emu = 0x13,
	SMT_bms = 0x14,
	SMT_bsd = 0x15,
	SMT_bsl = 0x16,
	SMT_bde = 0x17,
	SMT_bpc = 0x18,
	SMT_bne = 0x19,
	SMT_blb = 0x1A,
	SMT_pam000 = 0x1B,
	SMT_fig000 = 0x1C,
	SMT_evt041 = 0x1D,
	SMT_evt105 = 0x1E,
	SMT_evt121 = 0x1F,

	SMT_Mission1 = 0x100,
	SMT_Mission2 = 0x200,
	SMT_Mission3 = 0x300,
	SMT_Mission4 = 0x400,
	SMT_Mission5 = 0x500,

	SMT_BossHard = 0x100,
};

using SharedPtrTypeless = boost::anonymous_shared_ptr;

namespace Common
{
	static bool IsPlayerDead()
	{
		return SONIC_GENERAL_CONTEXT->GetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_Dead);
	}
	static void ChangePlayerAnimation(const Hedgehog::Base::CSharedString& in_Name)
	{
		const auto playerContext = Sonic::Player::CPlayerSpeedContext::GetInstance();
		playerContext->m_pPlayer->m_spAnimationStateMachine->ChangeState(in_Name);
	}
	inline float WrapFloat(float number, float bounds)
	{
		if (number > bounds) number -= bounds;
		if (number < 0) number += bounds;
		return number;
	}

	inline void ClampFloat(float& number, float min, float max)
	{
		if (number < min) number = min;
		if (number > max) number = max;
	}

	inline float RandomFloat(float min, float max) {
		return min + rand() / (RAND_MAX / (max - min + 1) + 1);
	}

	inline float Lerp(float a, float b, float t) {
		return a + t * (b - a);
	}

	inline bool IsStringEndsWith(std::string const& value, std::string const& ending)
	{
		if (ending.size() > value.size()) return false;
		return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
	}

	inline uint32_t ComposeAddressFromOffsets(uint32_t initAddress, std::vector<uint32_t> offsets)
	{
		uint32_t address = *(uint32_t*)initAddress;
		for (uint32_t i = 0; i < offsets.size(); i++)
		{
			uint32_t const& offset = offsets[i];
			address += offset;

			if (i < offsets.size() - 1)
			{
				address = *(uint32_t*)address;
			}
		}
		return address;
	}

	static void* SpawnParticle
	(
		void* pContext,
		SharedPtrTypeless& handle,
		void* pMatrixTransformNode,
		Hedgehog::Base::CSharedString const& name,
		uint32_t flag
	)
	{
		static void* const pCGlitterCreate = (void*)0xE73890;
		__asm
		{
			push    flag
			push    name
			push    pMatrixTransformNode
			mov     eax, pContext
			mov     esi, handle
			call[pCGlitterCreate]
		}
	}

	static void KillParticle
	(
		void* pContext,
		SharedPtrTypeless& handle,
		bool instantStop
	)
	{
		static void* const pCGlitterEnd = (void*)0xE72650;
		static void* const pCGlitterKill = (void*)0xE72570;
		__asm
		{
			mov     eax, [handle]
			mov     ebx, [eax + 4]
			push    ebx
			test	ebx, ebx
			jz		noIncrement
			mov		edx, 1
			add		ebx, 4
			lock xadd[ebx], edx

			noIncrement :
			mov     ebx, [eax]
				push    ebx
				mov     eax, pContext
				cmp     instantStop, 0
				jnz     jump
				call[pCGlitterEnd]
				jmp     end

				jump :
			call[pCGlitterKill]

				end :
		}
	}
	static bool Raycast
	(
		Eigen::Vector4f const& rayStartPos,
		Eigen::Vector4f const& rayEndPos,
		Eigen::Vector4f& outPos,
		Eigen::Vector4f& outNormal,
		uint32_t flag
	)
	{
		// sub_10BE3B0(&rayStartPos, flag, **(_DWORD ***)(*PLAYER_CONTEXT+ 1516), &outPos, &outNormal, &rayEndPos)
		static void* const pRaycast = (void*)0x10BE3B0;

		void* PLAYER_CONTEXT = SONIC_GENERAL_CONTEXT;
		__asm
		{
			mov		ecx, rayEndPos
			push	ecx
			mov		edx, outNormal
			push	edx
			mov		eax, outPos
			push	eax
			mov		ebx, PLAYER_CONTEXT
			mov		ebx, [ebx]
			mov     eax, [ebx + 5ECh]
			mov     edi, [eax]
			mov		edx, flag
			mov		eax, rayStartPos
			call[pRaycast]
		}
	}

	static void* WorldToScreenPosition
	(
		Eigen::Vector4f const& pos3D,
		Eigen::Vector4f& pos2D
	)
	{
		static void* const pWorldToScreenPosition = (void*)0xD61C40;
		__asm
		{
			mov		ecx, 0x1E0BE5C
			mov		ecx, [ecx]
			mov		eax, pos3D
			mov		ebx, pos2D
			push	ebx
			call[pWorldToScreenPosition]
		}
	}

	inline void fEventTrigger(void* This, int Event)
	{
		FUNCTION_PTR(void*, __stdcall, fpEventTrigger, 0xD5ED00, void* This, int Event);
		fpEventTrigger(This, Event);
	}
	inline void ObjectCGlitterPlayerOneShot(void* pObject, Hedgehog::Base::CSharedString const& name)
	{
		uint32_t* CGlitterPlayer = *(uint32_t**)((uint32_t)pObject + 0xF8);
		void* matrixNode = (void*)((uint32_t)pObject + 0xB8);
		if (CGlitterPlayer && *CGlitterPlayer == 0x16D0514)
		{
			FUNCTION_PTR(void, __thiscall, CGlitterPlayerOneShot, 0xE85F00, void* This, void* pMatrixTransformNode, Hedgehog::Base::CSharedString const& name, float a4, int a5);
			CGlitterPlayerOneShot(CGlitterPlayer, matrixNode, name, 1.0, 1);
		}
		else
		{
			MessageBox(NULL, L"Object does not contain CGlitterPlayer!", NULL, MB_ICONERROR);
		}
	}	

	inline bool IsPlayerIn2D()
	{
		if (SONIC_GENERAL_CONTEXT == nullptr) return false;
		return SONIC_GENERAL_CONTEXT->m_Is2DMode;
	}

	inline bool IsPlayerExtendedBoost()
	{
		if (SONIC_GENERAL_CONTEXT == nullptr) return false;
		return SONIC_GENERAL_CONTEXT->m_ChaosEnergyGaugeSize == 2;
	}

	inline bool IsPlayerHangOn()
	{
		if (SONIC_GENERAL_CONTEXT == nullptr) return false;
		return SONIC_GENERAL_CONTEXT->m_CameraOnlyUseGroundTarget != 0;
	}

	inline bool IsAtLoadingScreen()
	{
		uint32_t** hudCount = (uint32_t**)0x1E66B40;
		if (!*hudCount) return false;
		return (*hudCount)[2] > 0;
	}

	inline bool DoesNodeExistOnSonic(const Hedgehog::Base::CSharedString& name)
	{
		auto context = SONIC_GENERAL_CONTEXT;
		if (context)
		{
			return context->m_pPlayer->m_spCharacterModel->GetNode(name) != nullptr;
		}
		else
		{
			SUC::LogMessage("Could not get node because Sonic doesn't exist.");
		}

		return false;
	}

	inline bool IsStagePlayed(uint32_t stageID)
	{
		// Note: Only works for regular stages not mission
		// fake it as CHudGateMenuMain object
		uint32_t dummyAddress = (uint32_t)&stageID - 0x208;
		FUNCTION_PTR(bool, __stdcall, CHudGateMenuMain_IsStagePlayed, 0x107AE80, uint32_t pCHudGateMenuMain);

		return CHudGateMenuMain_IsStagePlayed(dummyAddress);
	}

	inline bool IsStageCompleted(uint32_t stageID)
	{
		FUNCTION_PTR(bool, __stdcall, fpIsStageCompleted, 0x107ADC0, uint32_t stageID);
		return fpIsStageCompleted(stageID);
	}

	inline bool GetStageData
	(
		uint32_t stageID,
		uint32_t& bestScore,
		float& bestTime,
		float& bestTime2,
		float& bestTime3,
		uint32_t& bestRank,
		uint32_t& bestRing,
		uint32_t& redRingCount
	)
	{
		FUNCTION_PTR(int, __fastcall, fpGetSaveSlotID, 0x552130, uint32_t stageID);
		int saveSlotID = fpGetSaveSlotID(stageID);

		uint32_t CGameParameterAddress = *(uint32_t*)ComposeAddressFromOffsets(0x1E66B34, { 0x4, 0x1B4 });
		uint32_t unknownOffset = *(uint32_t*)(CGameParameterAddress + 0x7C) + 0x1C;
		uint32_t saveSlotAddress = 0x120 * saveSlotID + unknownOffset;
		if (saveSlotID < 0x8E && saveSlotAddress != 0)
		{
			bestScore = *(uint32_t*)(saveSlotAddress);
			if (IsStageCompleted(stageID))
			{
				bestTime = *(float*)(saveSlotAddress + 0x4);
				bestTime2 = *(float*)(saveSlotAddress + 0x8);
				bestTime3 = *(float*)(saveSlotAddress + 0xC);
			}
			else
			{
				bestTime = -1.0f;
				bestTime2 = -1.0f;
				bestTime3 = -1.0f;
			}
			bestRank = 4 - *(uint32_t*)(saveSlotAddress + 0x10);
			bestRing = *(uint32_t*)(saveSlotAddress + 0x14);

			if ((stageID & 0xFF00) > 0 && (stageID & 0xFF) <= 0x11)
			{
				redRingCount = 0;
			}
			else
			{
				FUNCTION_PTR(int, __fastcall, fpGetRedRingCount, 0xD591B0, uint32_t stageID, int dummy, uint32_t CGameParameter);
				redRingCount = fpGetRedRingCount(stageID, 0, CGameParameterAddress);
			}

			return true;
		}
		else
		{
			return false;
		}
	}

	inline void PlayStageMusic(char const* cueName, float fadeInTime)
	{	
		FUNCTION_PTR(bool, __stdcall, PlayStageMusicFromCueName, 0xD63070, void* gameDocument, Hedgehog::Base::CSharedString & cueName, float fadeInTime);
		Hedgehog::Base::CSharedString name = Hedgehog::Base::CSharedString(cueName);
		PlayStageMusicFromCueName(Sonic::CGameDocument::GetInstance().get().get(), name, fadeInTime);
	}

	inline void PlayBGM(char const* cueName, float fadeInTime)
	{
		FUNCTION_PTR(unsigned int, __stdcall, PlayAudioFromCueName, 0xD62440, void* gameDocument, Hedgehog::Base::CSharedString & cueName, float fadeInTime);
		Hedgehog::Base::CSharedString name = Hedgehog::Base::CSharedString(cueName);
		PlayAudioFromCueName(Sonic::CGameDocument::GetInstance().get().get(), name, fadeInTime);
	}

	inline void StopBGM(char const* cueName, float fadeOutTime)
	{
		FUNCTION_PTR(int, __stdcall, StopAudioFromCueName, 0xD61E40, void* gameDocument, Hedgehog::Base::CSharedString & cueName, float fadeOutTime);
		Hedgehog::Base::CSharedString name = Hedgehog::Base::CSharedString(cueName);
		StopAudioFromCueName(Sonic::CGameDocument::GetInstance().get().get(), name, fadeOutTime);
	}

	inline float GetPlayerBoost()
	{
		if (SONIC_GENERAL_CONTEXT == nullptr) return 0;
		
		return SONIC_GENERAL_CONTEXT->m_ChaosEnergy;
	}

	inline float GetPlayerMaxBoost()
	{
		return IsPlayerExtendedBoost() ? 200.0f : 100.0f;
	}

	inline float* GetPlayerMaxSpeed()
	{
		if (SONIC_GENERAL_CONTEXT == nullptr) return nullptr;
		return &SONIC_GENERAL_CONTEXT->m_MaxVelocity;
	}

	inline void SetPlayerAutoBoost(bool enabled)
	{
		SONIC_GENERAL_CONTEXT->SetStateFlag(Sonic::Player::CPlayerSpeedContext::eStateFlag_AutoBoost, enabled);
	}

	inline bool GetPlayerWorldDirection(Eigen::Vector3f& direction, bool normalize)
	{
		direction = SONIC_GENERAL_CONTEXT->m_spMatrixNode->m_Transform.m_Rotation * SONIC_GENERAL_CONTEXT->m_WorldInput;
		return true;
	}

	inline float GetPlayerModelScale()
	{
		return SONIC_GENERAL_CONTEXT->m_pPlayer->m_spAnimationPose->m_Scale;
	}

	inline void SetPlayerModelScale(float scale)
	{
		SONIC_GENERAL_CONTEXT->m_pPlayer->m_spAnimationPose->m_Scale = scale;
	}
	

	

	inline void SonicContextPlayVoice(SharedPtrTypeless& soundHandle, uint32_t cueID, uint32_t priority)
	{
		// Note: This doesn't work at result screen, use PlaySoundStatic instead
		void* pSonicContext = SONIC_GENERAL_CONTEXT;
		if (!pSonicContext) return;

		// Original code by Skyth: https://github.com/blueskythlikesclouds
		//CSonicSpeedContextPlaySound* playSoundFunc = *(CSonicSpeedContextPlaySound**)(*(uint32_t*)pSonicContext + 0xA0);
		//playSoundFunc(pSonicContext, nullptr, soundHandle, cueID, priority);
	}
	inline void SonicContextRequestLocusEffect()
	{

		// 1 seems to not stop? Force it to be 0
		WRITE_MEMORY(0xE178E5, uint32_t, 0);

		struct MsgRequestLocusEffect
		{
			BB_INSERT_PADDING(0x10);
			uint32_t flag;
		};
		MsgRequestLocusEffect message{};
		message.flag = 0;

		FUNCTION_PTR(int, __thiscall, processMsgRequestLocusEffect, 0xE178D0, void* This, void* pMessage);
		
		processMsgRequestLocusEffect(SONIC_GENERAL_CONTEXT->m_pPlayer, &message);
	}

	inline void SonicContextAddPlugin(Hedgehog::Base::CSharedString const& plugin)
	{
		FUNCTION_PTR(void, __thiscall, addPlayerPlugin, 0xE77D80, Hedgehog::Base::CSharedString const* plugin, void* player);
		addPlayerPlugin(&plugin, SONIC_GENERAL_CONTEXT->m_pPlayer);
	}

	//static void* SonicContextSetCollision(SonicCollision collisionType, bool enabled)
	//{
	//	static void* const pEnableFunc = (void*)0xE65610;
	//	static void* const pDisableFunc = (void*)0xE655C0;
	//	__asm
	//	{
	//		mov		edi, PLAYER_CONTEXT
	//		mov		edi, [edi]
	//
	//		mov		ecx, collisionType
	//		mov		ecx, [ecx]
	//		push	ecx
	//
	//		cmp		enabled, 0
	//		je		jump
	//
	//		call[pEnableFunc]
	//		jmp		end
	//
	//		jump :
	//		call[pDisableFunc]
	//
	//			end :
	//	}
	//}

	inline void PlaySoundStatic(boost::shared_ptr<Hedgehog::Sound::CSoundHandle>& soundHandle, uint32_t cueID)
	{
		uint32_t* syncObject = *(uint32_t**)0x1E79044;
		if (syncObject)
		{
			FUNCTION_PTR(void*, __thiscall, sub_75FA60, 0x75FA60, void* This, boost::shared_ptr<Hedgehog::Sound::CSoundHandle>&, uint32_t cueId);
			sub_75FA60((void*)syncObject[8], soundHandle, cueID);
		}
	}
	inline void PlaySoundStaticCueName(boost::shared_ptr<Hedgehog::Sound::CSoundHandle>& soundHandle, Hedgehog::base::CSharedString cueID)
	{
		uint32_t* syncObject = *(uint32_t**)0x1E79044;
		if (syncObject)
		{
			FUNCTION_PTR(void*, __thiscall, sub_75FA60, 0x75FA90, void* This, boost::shared_ptr<Hedgehog::Sound::CSoundHandle>&, const Hedgehog::base::CSharedString & cueId);
			sub_75FA60((void*)syncObject[8], soundHandle, cueID);
		}
	}
	inline void SpawnBoostParticle(uint32_t** This, Eigen::Vector3f const& pos, int amount)
	{
		if (amount <= 0) return;

		struct BoostParticleData
		{
			Eigen::Vector3f m_pos;
			float m_unknown0xC = 1.0f;
			int m_amount = 0;
			float m_unknown0x14 = 1.0f;
			float m_unknown0x18 = 0.0f;
		};

		FUNCTION_PTR(void, __cdecl, fpSpawnBoostParticle, 0x1125210, uint32_t * pCGameService, uint32_t * pCWorld, BoostParticleData * pData);
		alignas(16) BoostParticleData data = {};
		data.m_pos = pos;
		data.m_amount = amount;
		uint32_t CWorld = (This[41][1] + 0x7C);
		uint32_t* pCWorld = &CWorld;
		uint32_t* pCGameService = (uint32_t*)This[41][0];
		fpSpawnBoostParticle(pCGameService, pCWorld, &data);
	}

	inline void ApplyPlayerApplyImpulse(MsgApplyImpulse const& message)
	{
		SONIC_GENERAL_CONTEXT->m_pPlayer->SendMessageSelfImm(message);
	}

	/*inline void ApplyObjectPhysicsPosition(void* pObject, Eigen::Vector3f const& pos)
	{
		FUNCTION_PTR(void*, __thiscall, processObjectMsgSetPosition, 0xEA2130, void* This, void* message);
		alignas(16) MsgSetPosition msgSetPosition {};
		msgSetPosition.m_position = pos;
		processObjectMsgSetPosition(pObject, &msgSetPosition);
	}*/
	inline float lerpUnclampedf(const float a, const float b, const float t)
	{
		return a + (b - a) * t;
	}
	inline float LerpFloat(float start, float end, float time, bool inEnabled = true, bool outEnabled = true)
	{
		const float min = fmin(start, end);
		const float max = fmax(start, end);
		return fmin(min, fmax(max, lerpUnclampedf(start, end, time)));
	}
	

	inline hh::math::CVector Lerp(hh::math::CVector a, hh::math::CVector b, float t)
	{
		return hh::math::CVector(a.x() + t * (b.x() - a.x()), a.y() + t * (b.y() - a.y()), a.z() + t * (b.z() - a.z()));
	}

	inline hh::math::CVector SmoothDamp(hh::math::CVector current, hh::math::CVector target, hh::math::CVector currentVelocity, float smoothTime, float maxSpeed, float deltaTime)
	{
		float output_x = 0.0f;
		float output_y = 0.0f;
		float output_z = 0.0f;

		// Based on Game Programming Gems 4 Chapter 1.10
		smoothTime = max(0.0001F, smoothTime);
		float omega = 2.0F / smoothTime;

		float x = omega * deltaTime;
		float exp = 1.0F / (1.0F + x + 0.48F * x * x + 0.235F * x * x * x);

		float change_x = current.x() - target.x();
		float change_y = current.y() - target.y();
		float change_z = current.z() - target.z();
		hh::math::CVector originalTo = target;

		// Clamp maximum speed
		float maxChange = maxSpeed * smoothTime;

		float maxChangeSq = maxChange * maxChange;
		float sqrmag = change_x * change_x + change_y * change_y + change_z * change_z;
		if (sqrmag > maxChangeSq)
		{
			float mag = (float)sqrt(sqrmag);
			change_x = change_x / mag * maxChange;
			change_y = change_y / mag * maxChange;
			change_z = change_z / mag * maxChange;
		}

		target.x() = current.x() - change_x;
		target.y() = current.y() - change_y;
		target.z() = current.z() - change_z;

		float temp_x = (currentVelocity.x() + omega * change_x) * deltaTime;
		float temp_y = (currentVelocity.y() + omega * change_y) * deltaTime;
		float temp_z = (currentVelocity.z() + omega * change_z) * deltaTime;

		currentVelocity.x() = (currentVelocity.x() - omega * temp_x) * exp;
		currentVelocity.y() = (currentVelocity.y() - omega * temp_y) * exp;
		currentVelocity.z() = (currentVelocity.z() - omega * temp_z) * exp;

		output_x = target.x() + (change_x + temp_x) * exp;
		output_y = target.y() + (change_y + temp_y) * exp;
		output_z = target.z() + (change_z + temp_z) * exp;

		// Prevent overshooting
		float origMinusCurrent_x = originalTo.x() - current.x();
		float origMinusCurrent_y = originalTo.y() - current.y();
		float origMinusCurrent_z = originalTo.z() - current.z();
		float outMinusOrig_x = output_x - originalTo.x();
		float outMinusOrig_y = output_y - originalTo.y();
		float outMinusOrig_z = output_z - originalTo.z();

		if (origMinusCurrent_x * outMinusOrig_x + origMinusCurrent_y * outMinusOrig_y + origMinusCurrent_z * outMinusOrig_z > 0)
		{
			output_x = originalTo.x();
			output_y = originalTo.y();
			output_z = originalTo.z();

			currentVelocity.x() = (output_x - originalTo.x()) / deltaTime;
			currentVelocity.y() = (output_y - originalTo.y()) / deltaTime;
			currentVelocity.z() = (output_z - originalTo.z()) / deltaTime;
		}

		return hh::math::CVector(output_x, output_y, output_z);
	}


	inline hh::math::CVector MoveTowards(hh::math::CVector current, hh::math::CVector target, float maxDistanceDelta)
	{
		hh::math::CVector a = target - current;
		float magnitude = abs(a.norm());

		if (magnitude <= maxDistanceDelta || magnitude == 0.0f)
			return target;
		
		return current + a / magnitude * maxDistanceDelta;
	}

	inline void CreatePlayerSupportShockWave(hh::math::CVector const& pos, float height, float radius, float duration)
	{
		struct ShockWaveParam
		{
			float m_height;
			float m_radius;
			float m_duration;
			size_t m_actorID;
		};
		FUNCTION_PTR(void*, __cdecl, fCreatePlayerSupportShockWave, 0x123D090, boost::shared_ptr<Sonic::CGameObject>&spObject, hh::math::CVector const* pos, ShockWaveParam const& param);
		boost::shared_ptr<Sonic::CGameObject> spObject;
		fCreatePlayerSupportShockWave(spObject, &pos, { height, radius, duration, Sonic::Player::CPlayerSpeedContext::GetInstance()->m_pPlayer->m_ActorID });
		Sonic::CGameDocument::GetInstance()->AddGameObject(spObject);
	}
	inline bool SolveBallisticArc
	(
		const Eigen::Vector3f& start,
		const Eigen::Vector3f& end,
		const float speed,
		const float gravity,
		const bool bUseHighArc,
		Eigen::Vector3f& outTrajectory,
		float& outTime
	)
	{
		Eigen::Vector3f diff = end - start;
		Eigen::Vector3f diffXZ(diff.x(), 0.0f, diff.z());
		float groundDist = diffXZ.norm();

		float speed2 = speed * speed;
		float speed4 = speed2 * speed2;
		float y = diff.y();
		float x = groundDist;
		float gx = gravity * x;

		float root = speed4 - gravity * (gravity * x * x + 2 * y * speed2);

		// No solution
		if (root < 0) return false;

		root = std::sqrt(root);
		float lowAng = std::atan2f(speed2 - root, gx);
		float highAng = std::atan2f(speed2 + root, gx);

		Eigen::Vector3f groundDir = diffXZ.normalized();
		if (bUseHighArc)
		{
			outTrajectory = groundDir * std::cos(highAng) * speed + Eigen::Vector3f::UnitY() * std::sin(highAng) * speed;
		}
		else
		{
			outTrajectory = groundDir * std::cos(lowAng) * speed + Eigen::Vector3f::UnitY() * std::sin(lowAng) * speed;
		}

		float speedXZ = Eigen::Vector3f(outTrajectory.x(), 0.0f, outTrajectory.z()).norm();
		outTime = groundDist / speedXZ;

		return true;
	}

	inline bool IsFileExist(std::string const& file)
	{
		struct stat buffer;
		return stat(file.c_str(), &buffer) == 0;
	}

	inline void GetModIniList(std::vector<std::string>& modIniList)
	{
		char buffer[MAX_PATH];
		GetModuleFileNameA(NULL, buffer, MAX_PATH);
		std::string exePath(buffer);
		std::string cpkRedirConfig = exePath.substr(0, exePath.find_last_of("\\")) + "\\cpkredir.ini";

		if (!Common::IsFileExist(cpkRedirConfig))
		{
			printf("%s not exist.\n", cpkRedirConfig.c_str());
			return;
		}

		INIReader reader(cpkRedirConfig);
		std::string modsDatabase = reader.Get("CPKREDIR", "ModsDbIni", "mods\\ModsDB.ini");

		if (!Common::IsFileExist(modsDatabase))
		{
			printf("%s not exist.\n", modsDatabase.c_str());
			return;
		}

		INIReader modsDatabaseReader(modsDatabase);
		int count = modsDatabaseReader.GetInteger("Main", "ActiveModCount", 0);
		for (int i = 0; i < count; i++)
		{
			std::string guid = modsDatabaseReader.Get("Main", "ActiveMod" + std::to_string(i), "");
			std::string config = modsDatabaseReader.Get("Mods", guid, "");
			if (!config.empty() && Common::IsFileExist(config))
			{
				modIniList.push_back(config);
			}
		}
	}

	inline bool IsModEnabled(std::string const& testModName, std::string* o_iniPath = nullptr)
	{
		std::vector<std::string> modIniList;
		GetModIniList(modIniList);
		for (size_t i = 0; i < modIniList.size(); i++)
		{
			std::string const& config = modIniList[i];
			INIReader configReader(config);
			std::string name = configReader.Get("Desc", "Title", "");
			if (name == testModName)
			{
				if (o_iniPath)
				{
					*o_iniPath = config;
				}

				return true;
			}
		}

		return false;
	}

	inline bool IsModEnabled(std::string const& section, std::string const& name, std::string const& str, std::string* o_iniPath = nullptr)
	{
		std::vector<std::string> modIniList;
		GetModIniList(modIniList);
		for (size_t i = 0; i < modIniList.size(); i++)
		{
			std::string const& config = modIniList[i];
			INIReader configReader(config);
			std::string value = configReader.Get(section, name, "");
			if (value == str)
			{
				if (o_iniPath)
				{
					*o_iniPath = config;
				}

				return true;
			}
		}

		return false;
	}

	inline bool TestModPriority(std::string const& currentModName, std::string const& testModName, bool higherPriority)
	{
		printf("currentModName = %s, testModName = %s\n", currentModName.c_str(), testModName.c_str());

		int currentModIndex = -1;
		int testModIndex = -1;

		std::vector<std::string> modIniList;
		GetModIniList(modIniList);
		for (size_t i = 0; i < modIniList.size(); i++)
		{
			std::string const& config = modIniList[i];
			INIReader configReader(config);
			std::string name = configReader.Get("Desc", "Title", "");
			if (name == currentModName)
			{
				currentModIndex = i;
			}
			else if (name == testModName)
			{
				testModIndex = i;
			}
		}

		if (currentModIndex != -1 && testModIndex != -1)
		{
			bool success = true;
			if (higherPriority)
			{
				success = (testModIndex < currentModIndex);
			}
			else
			{
				success = (testModIndex > currentModIndex);
			}

			if (!success)
			{
				std::string errorMsg = testModName + " detected, please put it " + (higherPriority ? "higher" : "lower") + " priority than (" + (higherPriority ? "above" : "below") + ") this mod.";
				std::wstring stemp = std::wstring(errorMsg.begin(), errorMsg.end());
				std::wstring stemp2 = std::wstring(currentModName.begin(), currentModName.end());
				MessageBox(nullptr, stemp.c_str(), stemp2.c_str(), MB_ICONERROR);
				exit(-1);
			}

			return success;
		}

		// Mod not found
		return false;
	}

	inline std::string wideCharToMultiByte(LPCWSTR value)
	{
		char multiByte[0x1000];
		WideCharToMultiByte(CP_UTF8, 0, value, -1, multiByte, _countof(multiByte), 0, 0);
		return std::string(multiByte);
	}

	inline std::wstring multiByteToWideChar(const char* value)
	{
		WCHAR wideChar[0x1000];
		MultiByteToWideChar(CP_UTF8, 0, value, -1, wideChar, _countof(wideChar));
		return std::wstring(wideChar);
	}

	inline bool DoesArchiveExist(std::string const& archiveName, std::set<std::string> ignoreModList = {})
	{
		std::vector<std::string> modIniList;
		GetModIniList(modIniList);
		for (std::string const& modIni : modIniList)
		{
			bool ignore = false;
			INIReader configReader(modIni);
			std::string modName = configReader.Get("Desc", "Title", "");
			for (std::string const& ignoreMod : ignoreModList)
			{
				if (modName.find(ignoreMod) != std::string::npos)
				{
					ignore = true;
					break;
				}
			}

			if (ignore)
			{
				continue;
			}

			std::string folder = modIni.substr(0, modIni.length() - 7);
			for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(folder))
			{
				if (dirEntry.path().filename() == archiveName)
				{
					return true;
				}
			}
		}
		return false;
	}
	inline uint32_t GetLivesCount()
	{
		const size_t liveCountAddr = Common::ComposeAddressFromOffsets(0x1E66B34, { 0x4, 0x1B4, 0x7C, 0x9FDC });
		return *(size_t*)liveCountAddr;
	}
	static const float GetVector2Distance(Hedgehog::Math::CVector2 vec1, Hedgehog::Math::CVector2 vec2)
	{
		auto vec = vec1 - vec2;
		float mag1 = sqrt(vec.x() * vec.x() + vec.y() * vec.y());
		return mag1;
	}
	inline void ClampInt(int& number, int min, int max)
	{
		if (number < min) number = min;
		if (number > max) number = max;
	}inline void ClampUInt(uint8_t& number, int min, int max)
	{
		if (number < min) number = min;
		if (number > max) number = max;
	}

	static char* IntToString(int num, const char* format)
	{
		char returnable[16];
		sprintf(returnable, format, num);
		return returnable;
	}
} // namespace Common
