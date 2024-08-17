#pragma once
#include "EvilAttackConfiguration.h"
namespace mod { namespace ev = SUC::Player::Evil; }
namespace Sonic
{
	class CMotionCameraController;
}
namespace SUC::Player::Evil
{

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
	class EvilGlobal
	{
	public:
		static inline std::string lastAttackName;
		static inline boost::shared_ptr<Sonic::CGameObject3D> shockwaveGameObject;
		static inline EvilParameters* parameters;
		static inline bool canExecuteAttacks;
		static inline bool disableAnimations;
		static inline bool allowFreemoveArmLeft, allowFreemoveArmRight;
		static inline Hedgehog::Math::CVector freemovePositionLeft;
		static inline Hedgehog::Math::CVector freemovePositionRight;

		static SUC::Player::Evil::Motion GetMotionFromName(std::string in_Name);
		static WerehogAttackNew GetAttackFromName();
		static std::string GetStateNameFromTable(std::string in);
		static void initializeValues();

		static inline boost::shared_ptr<Sonic::CMotionCameraController> m_spMotionCameraController;
	};
}