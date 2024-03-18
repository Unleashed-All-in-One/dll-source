#pragma once
using namespace hh::math;
class PhysicsObject
{
    CVector prevPos;
public:
	CVector m_Velocity;
	boost::shared_ptr<Sonic::CMatrixNodeTransform> transform;
	bool m_DoGravity = true;
	float m_Drag = 0.8f;
	void SetTransformReference(boost::shared_ptr<Sonic::CMatrixNodeTransform> tf)
	{
		transform = tf;
	}
	void ApplyGravity(float deltaTime, float gravity = -9.8f)
	{
		transform->m_Transform.SetPosition(transform->m_Transform.m_Position + (Hedgehog::Math::CVector::UnitY()* gravity * deltaTime));
	}
	void OnUpdate(float deltaTime)
	{
		if (m_DoGravity)
		{
			ApplyGravity(deltaTime);
		}
		//transform->m_Transform.SetPosition(transform->m_Transform.m_Position += m_Velocity * (Hedgehog::Math::CVector(m_Drag, m_Drag, m_Drag)));
        Eigen::Vector4f const rayStartPos(prevPos.x(), prevPos.y(), prevPos.z(), 1.0f);
        Eigen::Vector4f const rayEndPos(transform->m_Transform.m_Position.x(), transform->m_Transform.m_Position.y(), transform->m_Transform.m_Position.z(), 1.0f);
        Eigen::Vector4f outPos;
        Eigen::Vector4f outNormal;
        if (Common::fRaycast(rayStartPos, rayEndPos, outPos, outNormal, *(uint32_t*)0x1E0AFB4))
        {
			printf("collision!");
        }
	}
	void CollisionDetection()
	{
		/*Eigen::Vector4f const rayStartPos(posPrev.x(), posPrev.y(), posPrev.z(), 1.0f);
		Eigen::Vector4f const rayEndPos(m_Position.x(), m_Position.y(), m_Position.z(), 1.0f);
		Eigen::Vector4f outPos;
		Eigen::Vector4f outNormal;
		if (Common::fRaycast(rayStartPos, rayEndPos, outPos, outNormal, *(uint32_t*)0x1E0AFB4))
		{
		}*/
	}
	void ApplyForce(boost::shared_ptr<Sonic::CMatrixNodeTransform> transform, Hedgehog::math::CVector velocity)
	{
		transform->m_Transform.SetPosition(transform->m_Transform.m_Position + velocity);
	}
};