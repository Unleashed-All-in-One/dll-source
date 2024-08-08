#pragma once
namespace SUC::Accuracy
{
	class FallCam
	{
	public:
		static void RegisterHooks();

		static float AngleBetween(const Hedgehog::Math::CVector& a, const Hedgehog::Math::CVector& b) {
			float cosTheta = a.dot(b) / (a.norm() * b.norm());

			return acos(cosTheta);
		}
	};
}