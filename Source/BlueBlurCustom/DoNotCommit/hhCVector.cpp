namespace SUC::System
{
#define NANVEC(vec)
#define NANFLOAT(vec)

	CVector CVectorHelper::One()
	{
		return CVector(1, 1, 1);
	}

	CVector CVectorHelper::Right()
	{
		return CVector(-1, 0, 0);
	}

	CVector CVectorHelper::Left()
	{
		return CVector(1, 0, 0);
	}

	CVector CVectorHelper::Up()
	{
		return CVector(0, 1, 0);
	}

	CVector CVectorHelper::Down()
	{
		return CVector(0, -1, 0);
	}

	CVector CVectorHelper::Forward()
	{
		return CVector(0, 0, 1);
	}

	CVector CVectorHelper::Backward()
	{
		return CVector(0, 0, -1);
	}

	CVector* CVectorHelper::Lerp(CVector* out, CVector* VectorA, CVector* VectorB, float tValue)
	{
#ifdef _USE_INGAME_MATH
		NANVEC(out);
		NANVEC(VectorA);
		NANVEC(VectorB);

		BB_FUNCTION_PTR(CVector*, __thiscall, func, 0x006F0330, CVector * _out, CVector * _VectorA, CVector * _VectorB, float _tValue);
		CVector* result = func(out, VectorA, VectorB, tValue);

		NANVEC(out);
		NANVEC(VectorA);
		NANVEC(VectorB);

		return result;
#else
		*out = CVectorHelper::Lerp(*VectorA, *VectorB, tValue);
		return out;
#endif
	}

	CVector CVectorHelper::Orthogonal(const CVector& v)
	{
		const CVector out = v.z() < v.x() ? CVector(v.y(), -v.x(), 0) : CVector(0, -v.z(), v.y());

		NANVEC(out);
		NANVEC(v);

		return out;
	}

	double CVectorHelper::Length(CVector* This)
	{
		NANVEC(This);

		BB_FUNCTION_PTR(double, __thiscall, func, 0x009BF710, CVector * _This);
		double out = func(This);

		NANFLOAT(out);

		return out;
	}

	float CVectorHelper::Length(const CVector& This)
	{
		NANVEC(This);
		return This.norm();
	}

	// Two-point length
	float CVectorHelper::Length(const CVector& A, const CVector& B)
	{
		return (A - B).norm();
	}
	float CVectorHelper::Distance(const CVector& A, const CVector& B)
	{
		return Length(A, B);
	}

	CVector CVectorHelper::Project(const CVector& a, const CVector& b)
	{
		NANVEC(a);
		NANVEC(b);
		float m = Magnitude(b);
		NANFLOAT(m);
		return b * (Dot(a, b) / (m * m));
	}

	CVector CVectorHelper::Reject(const CVector& a, const CVector& b)
	{
		return a - Project(a, b);
	}

	CVector CVectorHelper::ProjectOnPlane(const CVector& vector, const CVector& planeNormal)
	{
		return Reject(vector, planeNormal);
	}

	double CVectorHelper::Magnitude(const CVector& This)
	{
		return This.norm();
	}

	CVector* CVectorHelper::SetZero(CVector* This)
	{
		*This = CVector::Zero();
		return This;
		//BB_FUNCTION_PTR(CVector, __thiscall, func, 0x009BFB10, CVector* _This);
		//return func(This);
	}

	

	

	CVector* CVectorHelper::Add(CVector* result, CVector* value)
	{
		*result += *value;
		return result;
	}

	CVector* CVectorHelper::Divide(CVector* result, CVector* value, float scalar)
	{
		//BB_FUNCTION_PTR(CVector*, __cdecl, func, 0x004030E0, CVector* _result, CVector* _value, float _scalar);
		//return func(result, value, scalar);

		if (scalar < FLT_EPSILON)
		{
			*result = CVector::Zero();
			return result;
		}

		NANVEC(result);
		NANVEC(value);
		NANFLOAT(scalar);

		*result = *value / scalar;

		NANVEC(result);
		NANVEC(value);

		return result;
	}

	CVector CVectorHelper::Divide(const CVector& value, float scalar)
	{
		if (scalar < FLT_EPSILON)
		{
			return CVector::Zero();
		}

		NANVEC(value);
		NANFLOAT(scalar);

		const CVector out = value / scalar;
		NANVEC(out);
		return out;
	}

	CVector* CVectorHelper::Multiply(CVector* result, CVector* value, float scalar)
	{
		//BB_FUNCTION_PTR(CVector*, __cdecl, func, 0x009BFB90, CVector* _result, CVector* _value, float _scalar);
		//return func(result, value, scalar);

		*result = *value * scalar;
		return result;
	}

	CVector* CVectorHelper::Multiply(CVector* result, float scalar, CVector* value)
	{
		//BB_FUNCTION_PTR(CVector*, __cdecl, func, 0x00404A70, CVector* _result, float _scalar, CVector* _value);
		//return func(result, scalar, value);
		*result = *value * scalar;
		return result;
	}

	CVector* CVectorHelper::Multiply(CVector* vector, float scalar)
	{
		//BB_FUNCTION_PTR(CVector*, __thiscall, func, 0x009BFB40, CVector* _vector, float _scalar);
		//return func(vector, scalar);
		*vector *= scalar;
		return vector;
	}

	double CVectorHelper::Dot(const CVector* This, const CVector* value)
	{
#if _USE_INGAME_MATH
		NANVEC(This);
		NANVEC(value);
		BB_FUNCTION_PTR(double, __thiscall, func, 0x009BF650, const CVector * _This, const CVector * _value);
		double out = func(This, value);
		NANVEC(This);
		NANVEC(value);
		NANFLOAT(out);
		return out;
#else
		return This->dot(*value);
#endif
	}

	double CVectorHelper::Dot(const CVector& This, const CVector& value)
	{
		/*
		BB_FUNCTION_PTR(double, __thiscall, func, 0x009BF650, const CVector & _This, const CVector & _value);
		double out = func(This, value);
		NANVEC(This);
		NANVEC(value);
		NANFLOAT(out);
		return out;
		*/

		return This.dot(value);
	}

	

	double CVectorHelper::LengthSqr(CVector* This)
	{
#if _USE_INGAME_MATH
		BB_FUNCTION_PTR(double, __thiscall, func, 0x004030A0, CVector * _This);
		return func(This);
#else
		return This->squaredNorm();
#endif
	}
	double CVectorHelper::LengthSqr(const CVector& This)
	{
		return This.squaredNorm();
	}

	

	double CVectorHelper::SqrMagnitude(const CVector& This)
	{
		return This.squaredNorm();
	}

	

	CVector* CVectorHelper::Cross(const CVector* This, CVector* result, const CVector* value)
	{
#ifdef _USE_INGAME_MATH
		NANVEC(This);
		NANVEC(result);
		NANVEC(value);
		BB_FUNCTION_PTR(CVector*, __thiscall, func, 0x009BF550, const CVector * _This, CVector * _result, const CVector * _value);
		CVector* out = func(This, result, value);
		NANVEC(This);
		NANVEC(result);
		NANVEC(value);
		NANVEC(out);
		return out;
#else
		CVector a = *This;
		CVector b = *value;
		*result = Cross(a, b);
		return result;
#endif
	}

	CVector CVectorHelper::Cross(const CVector& lhs, const CVector& rhs)
	{
		float x = lhs.y() * rhs.z() - lhs.z() * rhs.y();
		float y = lhs.z() * rhs.x() - lhs.x() * rhs.z();
		float z = lhs.x() * rhs.y() - lhs.y() * rhs.x();
		return CVector(x, y, z);
	}

	
	CVector CVectorHelper::SlerpUnclamped(const CVector& a, const CVector& b, const float t)
	{
		float magA = Magnitude(a);
		float magB = Magnitude(b);

		// HACK: Fix potential NAN stuff.
		if (magA < FLT_EPSILON)
			return b;
		if (magB < FLT_EPSILON)
			return a;

		CVector _a = a / magA;
		CVector _b = b / magB;
		float dot = Dot(_a, _b);
		dot = fmaxf(dot, -1.0);
		dot = fminf(dot, 1.0);
		float theta = acosf(dot) * t;
		CVector relativeVec = (_b - _a * dot).normalized();
		CVector newVec = _a * cosf(theta) + relativeVec * sinf(theta);
		return newVec * (magA + (magB - magA) * t);
	}

	CVector CVectorHelper::Slerp(const CVector& a, const CVector& b, const float t)
	{
		if (t < 0)
			return a;
		else
			if (t > 1)
				return b;
		return SlerpUnclamped(a, b, t);
	}

	float CVectorHelper::Angle(const CVector& a, const CVector& b)
	{
		float v = Dot(a, b) / (Magnitude(a) * Magnitude(b));
		v = fmaxf(v, -1.0);
		v = fminf(v, 1.0);
		return acosf(v);
	}

	float CVectorHelper::SignedAngle(const CVector& a, const CVector& b, const CVector& axis)
	{
		auto cross = CVectorHelper::Cross(a, b);
		auto x = CVectorHelper::Dot(axis, cross);
		auto y = CVectorHelper::Dot(a, b);

		return atan2(x, y);
	}

	CVector CVectorHelper::RotateTowards(const CVector& from, const CVector& to, float maxRadiansDelta)
	{
		float angle = CVectorHelper::Angle(from, to);
		if (angle == 0)
			return to;
		maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
		float t = fminf(1, maxRadiansDelta / angle);
		return CVectorHelper::SlerpUnclamped(from, to, t);
	}

	CVector CVectorHelper::LerpTowards(const CVector& from, const CVector& to, float maxRadiansDelta)
	{
		float angle = CVectorHelper::Angle(from, to);
		if (angle == 0)
			return to;
		maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
		float t = fminf(1, maxRadiansDelta / angle);
		return CVectorHelper::LerpUnclamped(from, to, t);
	}

	CVector CVectorHelper::SlerpTowards(const CVector& from, const CVector& to, float maxRadiansDelta)
	{
		float angle = CVectorHelper::Angle(from, to);
		if (angle == 0)
			return to;
		maxRadiansDelta = fmaxf(maxRadiansDelta, angle - (float)M_PI);
		float t = fminf(1, maxRadiansDelta / angle);
		return CVectorHelper::LerpUnclamped(from, to, t);
	}

	CVector CVectorHelper::Scale(const CVector& a, const CVector& b)
	{
		return CVector(a.x() * b.x(), a.y() * b.y(), a.z() * b.z());
	}

	
	CVector CVectorHelper::LerpUnclamped(const CVector& a, const CVector& b, const float t)
	{
		CVector result = (b - a) * t + a;
		return result;
	}

	CVector CVectorHelper::Lerp(const CVector& a, const CVector& b, const float t)
	{
		if (t < 0)
			return a;
		if (t > 1)
			return b;
		return LerpUnclamped(a, b, t);
	}

	CVector CVectorHelper::Reflect(const CVector& vector, const CVector& planeNormal)
	{
		return vector - Project(vector, planeNormal) * 2.0f;
	}
	CVector CVectorHelper::normalizedSafe(const CVector& a)
	{
		CVector This = a;
		CVector result = CVectorHelper::LengthSqr(a) > (DBL_EPSILON + DBL_EPSILON + DBL_EPSILON)
			? This.normalized()
			: CVector(0, 0, 0);
		return result;
	}



	

}
