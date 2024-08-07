#pragma once

namespace SUC::System
{
	class AspectRatioHelper
	{
	public:
		static Hedgehog::Math::CVector2* Resolution;
		static Hedgehog::Math::CVector2* OriginalResolution;
		static Hedgehog::Math::CVector2* ScreenHalfPoint;
		static Hedgehog::Math::CVector2* NativeResolution;
		static void Initialize(float resW, float resH);
		static void SetReferenceResolution(float resW, float resH);
		static void ScaleScene(Chao::CSD::CScene* scene);
	};
}
