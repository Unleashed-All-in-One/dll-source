namespace SUC::System
{
	Hedgehog::Math::CVector2* AspectRatioHelper::Resolution;
	Hedgehog::Math::CVector2* AspectRatioHelper::OriginalResolution;
	Hedgehog::Math::CVector2* AspectRatioHelper::ScreenHalfPoint;
	Hedgehog::Math::CVector2* AspectRatioHelper::NativeResolution;

	HOOK(double, __fastcall, AspectRatioHelper_GetWidth, 0x675420)
	{
		AspectRatioHelper::OriginalResolution = new Hedgehog::Math::CVector2(originalAspectRatioHelper_GetWidth(), AspectRatioHelper::OriginalResolution->y());
		return AspectRatioHelper::Resolution->x();
	}
	HOOK(double, __fastcall, AspectRatioHelper_GetHeight, 0x675430)
	{
		AspectRatioHelper::OriginalResolution = new Hedgehog::Math::CVector2(AspectRatioHelper::OriginalResolution->x(), originalAspectRatioHelper_GetHeight());
		return AspectRatioHelper::Resolution->y();
	}
	HOOK(int, __cdecl, TestSpec, 0x682770, DWORD a1, int type, int a3, byte* a4, float a5, float offset)
	{
		return originalTestSpec(a1, 1, a3, a4, a5, offset);
	}
	void AspectRatioHelper::SetReferenceResolution(float resW = 1280, float resH = 720)
	{
		Resolution = new Hedgehog::Math::CVector2(resW, resH);
	}
	void AspectRatioHelper::ScaleScene(Chao::CSD::CScene* scene)
	{
	}
	//I'll admit, I have kinda forgotten what this does, but it works, sooo
	void AspectRatioHelper::Initialize(float resW = 1280, float resH = 720)
	{
		RECT rc;
		GetWindowRect(GetDesktopWindow(), &rc);
		NativeResolution = new Hedgehog::Math::CVector2(rc.right, rc.bottom);
		Resolution = new Hedgehog::Math::CVector2(resW, resH);
		OriginalResolution = new Hedgehog::Math::CVector2(0, 0);
		ScreenHalfPoint = new Hedgehog::Math::CVector2(Resolution->x() / 2, Resolution->y() / 2);
		INSTALL_HOOK(AspectRatioHelper_GetWidth);
		INSTALL_HOOK(AspectRatioHelper_GetHeight);
	}
}