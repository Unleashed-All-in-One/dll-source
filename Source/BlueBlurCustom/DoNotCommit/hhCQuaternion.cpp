#include "hhCVector.h"

namespace SUC::System
{
	CQuaternion CQuaternionHelper::FromAxes(const CVector& a1, const CVector& a2, const CVector& a3)
	{
		CQuaternion This = CQuaternion::Identity();

		const CVector AxisX = CVectorHelper::normalizedSafe(a1);
		const CVector AxisY = CVectorHelper::normalizedSafe(a2);
		CVector AxisZ = CVectorHelper::Cross(AxisX, AxisY);
		const float axisZ_length = (float)CVectorHelper::Length(AxisZ);

		AxisZ = CVectorHelper::normalizedSafe(AxisZ);
		if (fabs(axisZ_length) < 0.000199999994947575)
			AxisZ = CVectorHelper::normalizedSafe(a3);

		const float dot = std::clamp((float)CVectorHelper::Dot(AxisX, AxisY), -1.0f, 1.0f);
		const double halfDot = dot * 0.5;
		const float axisScale = sqrtf(0.5 - halfDot);

		This.x() = AxisZ.x() * axisScale;
		This.y() = AxisZ.y() * axisScale;
		This.z() = AxisZ.z() * axisScale;
		This.w() = sqrtf(0.5 + halfDot);
		return This;

		//BB_FUNCTION_PTR(CQuaternion*, __cdecl, func, 0x006F1950, CQuaternion* _This, const CVector& A1, const CVector& A2, const CVector& A3);
		//func(&This, a1, a2, a3);
		//return This;
	}

	CVector CQuaternionHelper::Up(const CQuaternion& a)
	{
		return a * CVectorHelper::Up();
	}

	CVector CQuaternionHelper::Down(const CQuaternion& a) 
	{
		return a * CVectorHelper::Down();
	}

	CVector CQuaternionHelper::Left(const CQuaternion& a) 
	{
		return a * CVectorHelper::Left();
	}

	CVector CQuaternionHelper::Right(const CQuaternion& a) 
	{
		return a * CVectorHelper::Right();
	}

	CVector CQuaternionHelper::Forward(const CQuaternion& a) 
	{
		return a * CVectorHelper::Forward();
	}

	CVector CQuaternionHelper::Back(const CQuaternion& a) 
	{
		return a * CVectorHelper::Backward();
	}

	CMatrix44 CQuaternionHelper::ToRotationMatrix(const CQuaternion& a)
	{
		CMatrix44 matrix = CMatrix44::Identity();
		BB_FUNCTION_PTR(void*, __cdecl, func, 0x009BEF20, const CMatrix44 & mat, const CQuaternion * quat);
		func(matrix, &a);
		return matrix;
	}

	float CQuaternionHelper::Angle(const CQuaternion& a, const CQuaternion& b)
	{
		float dot = Dot(a, b);
		return acosf(fminf(fabs(dot), 1)) * 2;
	}

	CQuaternion CQuaternionHelper::Conjugate(const CQuaternion& rotation)
	{
		return CQuaternion(-rotation.x(), -rotation.y(), -rotation.z(), rotation.w());
	}

	float CQuaternionHelper::Dot(const CQuaternion& lhs, const CQuaternion& rhs)
	{
		return lhs.x() * rhs.x() + lhs.y() * rhs.y() + lhs.z() * rhs.z() + lhs.w() * rhs.w();
	}

	CQuaternion CQuaternionHelper::FromAngleAxis(float angle, const CVector& axis)
	{
		float m = sqrt(axis.x() * axis.x() + axis.y() * axis.y() + axis.z() * axis.z());
		float s = sinf(angle / 2.0f) / m;
		const CQuaternion q(
			cosf(angle / 2.0f),
			axis.x() * s,
			axis.y() * s,
			axis.z() * s
		);
		return q;
	}

	CQuaternion CQuaternionHelper::FromEuler(const CVector& rotation)
	{
		return FromEuler(rotation.x(), rotation.y(), rotation.z());
	}

	CQuaternion CQuaternionHelper::FromEuler(float x, float y, float z)
	{
		float cx = cosf(x * 0.5f);
		float cy = cosf(y * 0.5f);
		float cz = cosf(z * 0.5f);
		float sx = sinf(x * 0.5f);
		float sy = sinf(y * 0.5f);
		float sz = sinf(z * 0.5f);
		CQuaternion q;
		q.x() = cx * sy * sz + cy * cz * sx;
		q.y() = cx * cz * sy - cy * sx * sz;
		q.z() = cx * cy * sz - cz * sx * sy;
		q.w() = sx * sy * sz + cx * cy * cz;
		return q;
	}

	CQuaternion CQuaternionHelper::FromToRotation(const CVector& fromVector, const CVector& toVector)
	{
		float dot = CVectorHelper::Dot(fromVector, toVector);
		float k = sqrt(CVectorHelper::SqrMagnitude(fromVector) *
			CVectorHelper::SqrMagnitude(toVector));
		if (fabs(dot / k + 1) < 0.00001)
		{
			CVector ortho = CVectorHelper::Orthogonal(fromVector);
			auto vNorm = (ortho).normalized();
			return CQuaternion(vNorm.x(), vNorm.y(), vNorm.z(), 0);
		}
		CVector cross = CVectorHelper::Cross(fromVector, toVector);
		return CQuaternion(cross.x(), cross.y(), cross.z(), dot + k).normalized();
	}

	CQuaternion CQuaternionHelper::Inverse(const CQuaternion& rotation)
	{
		//float n = Norm(rotation);
		//return Conjugate(rotation) / (n * n);
		return rotation.inverse();
	}

	CQuaternion CQuaternionHelper::Lerp(const CQuaternion& a, const CQuaternion& b, float t, bool normalize)
	{
		if (t < 0)
			return a.normalized();
		else
			if (t > 1)
				return b.normalized();
		return LerpUnclamped(a, b, t, normalize);
	}

	CQuaternion CQuaternionHelper::LerpUnclamped(const CQuaternion& a, const CQuaternion& b, float t, bool normalize)
	{
		CVector4 tempVec;
		if (Dot(a, b) >= 0)
			tempVec = (*(CVector4*)&a * (1 - t)) + (*(CVector4*)&b * t);
		else
			tempVec = (*(CVector4*)&a * (1 - t)) - (*(CVector4*)&b * t);
		if (!normalize)
			return *(CQuaternion*)&tempVec;
		return (*(CQuaternion*)&tempVec).normalized();
		//return *(CQuaternion*)&tempVec;
	}

	CQuaternion CQuaternionHelper::LookRotation(const CVector& forward)
	{
		return LookRotation(forward, CVector(0, 1, 0));
	}

	CQuaternion CQuaternionHelper::LookRotation(const CVector& forward_, const CVector& upwards_)
	{
		// Normalize inputs
		CVector forward = CVectorHelper::normalizedSafe(forward_);
		CVector upwards = CVectorHelper::normalizedSafe(upwards_);
		// Don't allow zero vectors
		if (CVectorHelper::SqrMagnitude(forward) < FLT_EPSILON || CVectorHelper::SqrMagnitude(upwards) < FLT_EPSILON)
			return CQuaternion::Identity();
		// Handle alignment with up direction
		if (1 - fabs(CVectorHelper::Dot(forward, upwards)) < FLT_EPSILON)
			return FromToRotation(CVectorHelper::Forward(), forward);
		// Get orthogonal vectors
		CVector right = CVectorHelper::normalizedSafe(CVectorHelper::Cross(upwards, forward));
		upwards = CVectorHelper::Cross(forward, right);
		// Calculate rotation
		CQuaternion CQuaternion;
		float radicand = right.x() + upwards.y() + forward.z();
		if (radicand > 0)
		{
			CQuaternion.w() = sqrt(1.0f + radicand) * 0.5f;
			float recip = 1.0f / (4.0f * CQuaternion.w());
			CQuaternion.x() = (upwards.z() - forward.y()) * recip;
			CQuaternion.y() = (forward.x() - right.z()) * recip;
			CQuaternion.z() = (right.y() - upwards.x()) * recip;
		}
		else if (right.x() >= upwards.y() && right.x() >= forward.z())
		{
			CQuaternion.x() = sqrt(1.0f + right.x() - upwards.y() - forward.z()) * 0.5f;
			float recip = 1.0f / (4.0f * CQuaternion.x());
			CQuaternion.w() = (upwards.z() - forward.y()) * recip;
			CQuaternion.z() = (forward.x() + right.z()) * recip;
			CQuaternion.y() = (right.y() + upwards.x()) * recip;
		}
		else if (upwards.y() > forward.z())
		{
			CQuaternion.y() = sqrt(1.0f - right.x() + upwards.y() - forward.z()) * 0.5f;
			float recip = 1.0f / (4.0f * CQuaternion.y());
			CQuaternion.z() = (upwards.z() + forward.y()) * recip;
			CQuaternion.w() = (forward.x() - right.z()) * recip;
			CQuaternion.x() = (right.y() + upwards.x()) * recip;
		}
		else
		{
			CQuaternion.z() = sqrt(1.0f - right.x() - upwards.y() + forward.z()) * 0.5f;
			float recip = 1.0f / (4.0f * CQuaternion.z());
			CQuaternion.y() = (upwards.z() + forward.y()) * recip;
			CQuaternion.x() = (forward.x() + right.z()) * recip;
			CQuaternion.w() = (right.y() - upwards.x()) * recip;
		}
		return CQuaternion;
	}

	float CQuaternionHelper::Norm(const CQuaternion& rotation)
	{
		return sqrt(rotation.x() * rotation.x() +
			rotation.y() * rotation.y() +
			rotation.z() * rotation.z() +
			rotation.w() * rotation.w());
	}

	
	CQuaternion CQuaternionHelper::RotateTowards(const CQuaternion& from, const CQuaternion& to,
		float maxRadiansDelta)
	{
		float angle = CQuaternionHelper::Angle(from, to);
		if (angle == 0)
			return to;
		const float radiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
		float t = fminf(1, radiansDelta / angle);
		return CQuaternionHelper::SlerpUnclamped(from, to, t);
	}

	CQuaternion CQuaternionHelper::Slerp(const CQuaternion& a, const CQuaternion& b, float t, bool normalize)
	{
		if (t < 0)
			return a.normalized();
		else
			if (t > 1)
				return b.normalized();
		return SlerpUnclamped(a, b, t, normalize);
	}

	CQuaternion CQuaternionHelper::SlerpUnclamped(const CQuaternion& a, const CQuaternion& b, float t, bool normalize)
	{
		float n1;
		float n2;
		float n3 = Dot(a, b);
		bool flag = false;
		if (n3 < 0)
		{
			flag = true;
			n3 = -n3;
		}
		if (n3 > 0.999999)
		{
			n2 = 1 - t;
			n1 = flag ? -t : t;
		}
		else
		{
			float n4 = acosf(n3);
			float n5 = 1 / sinf(n4);
			n2 = sinf((1 - t) * n4) * n5;
			n1 = flag ? -sinf(t * n4) * n5 : sinf(t * n4) * n5;
		}
		CQuaternion CQuaternion;
		CQuaternion.x() = (n2 * a.x()) + (n1 * b.x());
		CQuaternion.y() = (n2 * a.y()) + (n1 * b.y());
		CQuaternion.z() = (n2 * a.z()) + (n1 * b.z());
		CQuaternion.w() = (n2 * a.w()) + (n1 * b.w());
		if (normalize)
			return CQuaternion.normalized();
		return CQuaternion;
	}

	CQuaternion CQuaternionHelper::SlerpTowards(const CQuaternion& from, const CQuaternion& to, float maxRadiansDelta)
	{
		float angle = Angle(from, to);
		if (angle == 0)
			return to;
		maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
		float t = fminf(1, maxRadiansDelta / angle);
		return Slerp(from, to, t);
	}

	void CQuaternionHelper::ToAngleAxis(const CQuaternion& rotation, float& angle, CVector& axis)
	{
		CQuaternion rot = rotation.w() > 1 ? rotation.normalized() : rotation;
		//if (rot.w() > 1)
		//    rot = Normalized(rot);
		angle = 2 * acosf(rot.w());
		float s = sqrt(1 - rot.w() * rot.w());
		if (s < 0.00001)
		{
			axis.x() = 1;
			axis.y() = 0;
			axis.z() = 0;
		}
		else
		{
			axis.x() = rot.x() / s;
			axis.y() = rot.y() / s;
			axis.z() = rot.z() / s;
		}
	}

	CVector CQuaternionHelper::ToEuler(const CQuaternion& rotation)
	{
		float sqw = rotation.w() * rotation.w();
		float sqx = rotation.x() * rotation.x();
		float sqy = rotation.y() * rotation.y();
		float sqz = rotation.z() * rotation.z();
		// If normalized is one, otherwise is correction factor
		float unit = sqx + sqy + sqz + sqw;
		float test = rotation.x() * rotation.w() - rotation.y() * rotation.z();
		CVector v;
		// sinfgularity at north pole
		if (test > 0.4995f * unit)
		{
			v.y() = 2 * atan2f(rotation.y(), rotation.x());
			v.x() = (float)M_PI_2;
			v.z() = 0;
			return v;
		}
		// sinfgularity at south pole
		if (test < -0.4995f * unit)
		{
			v.y() = -2 * atan2f(rotation.y(), rotation.x());
			v.x() = -(float)M_PI_2;
			v.z() = 0;
			return v;
		}
		// Yaw
		v.y() = atan2f(2 * rotation.w() * rotation.y() + 2 * rotation.z() * rotation.x(),
			1 - 2 * (rotation.x() * rotation.x() + rotation.y() * rotation.y()));
		// Pitch
		v.x() = asinf(2 * (rotation.w() * rotation.x() - rotation.y() * rotation.z()));
		// Roll
		v.z() = atan2f(2 * rotation.w() * rotation.z() + 2 * rotation.x() * rotation.y(),
			1 - 2 * (rotation.z() * rotation.z() + rotation.x() * rotation.x()));
		return v;
	}

}
