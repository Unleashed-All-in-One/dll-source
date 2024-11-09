#pragma once
namespace SUC::System
{
	using namespace hh::math;
	class CVectorHelper
	{
	public:

		static CVector One();
		static CVector Right();
		static CVector Left();
		static CVector Up();
		static CVector Down();
		static CVector Forward();
		static CVector Backward();

		static CVector LerpUnclamped(const CVector& a, const CVector& b, const float t);

		static CVector Lerp(const CVector& a, const CVector& b, const float t);

		static CVector* Lerp(CVector* out, CVector* VectorA, CVector* VectorB, float tValue);

		static CVector Orthogonal(const CVector& v);

		static double Length(CVector* This);

		static float Length(const CVector& This);

		static float Length(const CVector& A, const CVector& B);

		static float Distance(const CVector& A, const CVector& B);

		static CVector Project(const CVector& a, const CVector& b);

		static CVector Reject(const CVector& a, const CVector& b);

		/**
	 * Returns a vector reflected off the plane orthogonal to the normal.
	 * The input vector is pointed inward, at the plane, and the return vector
	 * is pointed outward from the plane, like a beam of light hitting and then
	 * reflecting off a mirror.
	 * @param vector: The vector traveling inward at the plane.
	 * @param planeNormal: The normal of the plane off of which to reflect.
	 * @return: A new vector pointing outward from the plane.
	 */
		static CVector Reflect(const CVector& vector, const CVector& planeNormal);

		static CVector ProjectOnPlane(const CVector& vector, const CVector& planeNormal);

		static double Magnitude(const CVector& This);

		static CVector* SetZero(CVector* This);


		static CVector* Add(CVector* result, CVector* value);

		static CVector* Divide(CVector* result, CVector* value, float scalar);

		static CVector Divide(const CVector& value, float scalar);

		static CVector* Multiply(CVector* result, CVector* value, float scalar);

		static CVector* Multiply(CVector* result, float scalar, CVector* value);

		static CVector* Multiply(CVector* vector, float scalar);

		static double Dot(const CVector* This, const CVector* value);

		static double Dot(const CVector& This, const CVector& value);

		static double LengthSqr(CVector* This);

		static double LengthSqr(const CVector& This);

		static double SqrMagnitude(const CVector& This);

		static CVector* Cross(const CVector* This, CVector* result, const CVector* value);

		static CVector Cross(const CVector& lhs, const CVector& rhs);


		static CVector SlerpUnclamped(const CVector& a, const CVector& b, const float t);

		static CVector Slerp(const CVector& a, const CVector& b, const float t);

		static float Angle(const CVector& a, const CVector& b);

		static float SignedAngle(const CVector& a, const CVector& b, const CVector& axis);

		static CVector RotateTowards(const CVector& from, const CVector& to,
			float maxRadiansDelta);

		static CVector LerpTowards(const CVector& from, const CVector& to,
			float maxRadiansDelta);

		static CVector SlerpTowards(const CVector& from, const CVector& to,
			float maxRadiansDelta);

		static CVector Scale(const CVector& a, const CVector& b);


		static CVector normalizedSafe(const CVector& a);
	};
}