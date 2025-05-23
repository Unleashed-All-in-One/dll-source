#pragma once

namespace SUC
{
	struct NewAnimationData;
}

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
	static void RegisterAnimations(const boost::shared_ptr<Hedgehog::Animation::CAnimationPose>& animPose, std::vector<SUC::NewAnimationData> anims, const boost::shared_ptr<hh::mr::CSingleElement>& model, Sonic::CAnimationStateMachine* stateMachine);
	static void SetTransformScale(Hedgehog::Mirage::CTransform& in_Transform, Hedgehog::math::CVector scale);
	static void AssignUVAnimation(const char* in_animationName, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, const boost::shared_ptr<Hedgehog::Mirage::CModelData>& in_spModelData, const boost::shared_ptr<hh::mot::CSingleElementEffectMotionAll>& in_spEffectMotionAll);
	static bool IsResourceAvailableHKX(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName);
	static bool IsResourceAvailableMODEL(const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, std::string in_assetName);
	static void CreateModel(const std::string& in_ModelName, const boost::shared_ptr<Hedgehog::Database::CDatabase>& in_spDatabase, boost::shared_ptr<hh::mr::CSingleElement>& in_spModelResult);
	static float GetAnimTime(Sonic::CAnimationStateMachine* in_AnimStateMachine);
	static bool SendCategoryMessage(const char* in_Category, const boost::shared_ptr<Hedgehog::Universe::Message>& in_spMessage);
};
