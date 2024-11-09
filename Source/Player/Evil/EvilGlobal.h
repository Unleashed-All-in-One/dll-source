#pragma once
#include "EvilAttackConfiguration.h"
namespace mod { namespace ev = SUC::Player::Evil; }
namespace Sonic
{
	class CMotionCameraController;
}
namespace SUC::Player::Evil
{
	class EvilGlobal
	{
	public:
		struct EvilParameters
		{
			float attackVelocityDivider;
			float lifeMaxAmount;
			float lifeCurrentAmount;
			float movementSpeed;
			float turnRate;
			float timerComboMax = 0.75f;
			float timerDamageMax = 0.3f;
			float timerAttackMax = 0.35f;
		};
		static inline float s_AnimationSpeed = 1;
		static inline std::string s_LatestAttackName;
		static inline EvilParameters* s_Param;
		static inline bool s_CanAttack;
		static inline bool s_AllowArmMovL, s_AllowArmMovR;
		static inline Hedgehog::Math::CVector s_ArmPosL;
		static inline Hedgehog::Math::CVector s_ArmPosR;
		static inline boost::shared_ptr<Sonic::CMotionCameraController> s_spMotionCameraController;

		static SUC::Player::Evil::Motion GetMotionFromName(std::string in_Name);
		static WerehogAttackNew GetAttackFromName();
		static std::string GetStateNameFromTable(std::string in);
		static void Initialize();
	};
}