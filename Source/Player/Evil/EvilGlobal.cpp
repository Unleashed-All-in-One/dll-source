#include "EvilGlobal.h"

#include "EvilAttackConfiguration.h"
EvilParameters* EvilGlobal::parameters;
std::string EvilGlobal::lastAttackName;
boost::shared_ptr<Sonic::CGameObject3D> EvilGlobal::shockwaveGameObject;
boost::shared_ptr<Sonic::CMotionCameraController> EvilGlobal::m_spMotionCameraController;
bool EvilGlobal::canExecuteAttacks;
bool EvilGlobal::disableAnimations;
bool EvilGlobal::allowFreemoveArmLeft, EvilGlobal::allowFreemoveArmRight;
Hedgehog::Math::CVector EvilGlobal::freemovePositionLeft;
Hedgehog::Math::CVector EvilGlobal::freemovePositionRight;

Motion EvilGlobal::GetMotionFromName(std::string in_Name)
{
	for (size_t i = 0; i < EvilAttackConfiguration::animationTable.size(); i++)
	{
		if (EvilAttackConfiguration::animationTable[i].MotionName == in_Name)
			return EvilAttackConfiguration::animationTable[i];
	}
}
std::string EvilGlobal::GetStateNameFromTable(std::string in)
{
	for (size_t i = 0; i < EvilAttackConfiguration::animationTable.size(); i++)
	{
		if (EvilAttackConfiguration::animationTable[i].MotionName == in)
			return EvilAttackConfiguration::animationTable[i].FileName;
	}
}
WerehogAttackNew EvilGlobal::GetAttackFromName()
{
	auto motion = GetMotionFromName(EvilGlobal::lastAttackName);
	for (size_t i = 0; i < EvilAttackConfiguration::attacks.size(); i++)
	{
		if (EvilAttackConfiguration::attacks[i].MotionName == motion.MotionName)
			return EvilAttackConfiguration::attacks[i];
	}
}
void EvilGlobal::initializeValues()
{
	EvilParameters* params = new EvilParameters();
	params->attackVelocityDivider = 1.5f;
	params->lifeMaxAmount = 15;
	params->timerDamageMax = 2;
	parameters = params;
}