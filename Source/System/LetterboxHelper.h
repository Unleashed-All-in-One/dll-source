#pragma once

class LetterboxHelper
{
	
public:
	static Hedgehog::Math::CVector2* Resolution;
	static Hedgehog::Math::CVector2* OriginalResolution;
	static Hedgehog::Math::CVector2* ScreenHalfPoint;
	static Hedgehog::Math::CVector2* NativeResolution;
	static void initialize(float resW, float resH);
	static void setResolution(float resW, float resH);
	static void scaleScene(Chao::CSD::CScene* scene);
};
