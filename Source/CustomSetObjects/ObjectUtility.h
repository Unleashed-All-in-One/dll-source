#pragma once

class ObjectUtility
{
public:
	static Hedgehog::Math::CVector MoveAroundPivot(Hedgehog::Math::CVector& player, const Hedgehog::Math::CVector& pivot, const Hedgehog::Math::CVector& rotationAngles);
	static Hedgehog::Math::CQuaternion QuaternionFromAngleAxisUtil(float angle, const Hedgehog::Math::CVector& axis);
	static float Distance(const Eigen::Vector3f& v1, const Eigen::Vector3f& v2);
	static Eigen::Vector3f ClampMagnitudeMax(const Eigen::Vector3f& vec, float minMagnitude, float maxMagnitude);
	static Hedgehog::Math::CQuaternion QuaternionFaceTowards(const Eigen::Vector3f& targetPoint, const Eigen::Vector3f& position, Hedgehog::Math::CQuaternion rotation);
	static float easeInOutQuart(float x);
	static bool DoWerehogArmHomingIfClose(Hedgehog::Math::CVector target, float maxDistance, float deltaTime);
	static void RegisterAnimations(const boost::shared_ptr<Hedgehog::Animation::CAnimationPose>& animPose, std::vector<NewAnimationData> anims, const boost::shared_ptr<hh::mr::CSingleElement>& model, Sonic::CAnimationStateMachine* stateMachine);
	static void SetTransformScale(Hedgehog::Mirage::CTransform& in_Transform, Hedgehog::math::CVector scale);
	static bool IsResourceAvailableHKX(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName);
	static bool IsResourceAvailableMODEL(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName);
};