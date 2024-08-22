#include "EvilGlobal.h"
#include "EvilAttackConfiguration.h"

namespace SUC::Player::Evil
{
	/*EvilParameters* EvilGlobal::parameters;
	std::string EvilGlobal::lastAttackName;
	boost::shared_ptr<Sonic::CGameObject3D> EvilGlobal::shockwaveGameObject;
	boost::shared_ptr<Sonic::CMotionCameraController> EvilGlobal::m_spMotionCameraController;
	bool EvilGlobal::canExecuteAttacks;
	bool EvilGlobal::disableAnimations;
	bool EvilGlobal::allowFreemoveArmLeft, EvilGlobal::allowFreemoveArmRight;
	Hedgehog::Math::CVector EvilGlobal::freemovePositionLeft;
	Hedgehog::Math::CVector EvilGlobal::freemovePositionRight;*/

	Motion EvilGlobal::GetMotionFromName(std::string in_Name)
	{
		for (size_t i = 0; i < EvilAttackConfiguration::s_AnimationTable.size(); i++)
		{
			if (EvilAttackConfiguration::s_AnimationTable[i].MotionName == in_Name)
				return EvilAttackConfiguration::s_AnimationTable[i];
		}
	}
	std::string EvilGlobal::GetStateNameFromTable(std::string in)
	{
		for (size_t i = 0; i < EvilAttackConfiguration::s_AnimationTable.size(); i++)
		{
			if (EvilAttackConfiguration::s_AnimationTable[i].MotionName == in)
				return EvilAttackConfiguration::s_AnimationTable[i].FileName;
		}
	}
	WerehogAttackNew EvilGlobal::GetAttackFromName()
	{
		auto motion = GetMotionFromName(EvilGlobal::s_LatestAttackName);
		for (size_t i = 0; i < EvilAttackConfiguration::s_Attacks.size(); i++)
		{
			if (EvilAttackConfiguration::s_Attacks[i].MotionName == motion.MotionName)
				return EvilAttackConfiguration::s_Attacks[i];
		}
	}
	void EvilGlobal::Initialize()
	{
		EvilParameters* params = new EvilParameters();
		params->attackVelocityDivider = 1.5f;
		params->lifeMaxAmount = 15;
		params->timerDamageMax = 2;
		s_Param = params;
	}
}