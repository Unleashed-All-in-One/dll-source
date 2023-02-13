Hedgehog::Math::CVector2* LetterboxHelper::Resolution;
Hedgehog::Math::CVector2* LetterboxHelper::OriginalResolution;
Hedgehog::Math::CVector2* LetterboxHelper::ScreenHalfPoint;
Hedgehog::Math::CVector2* LetterboxHelper::NativeResolution;

HOOK(double, __fastcall, LetterboxHelper_GetWidth, 0x675420)
{
	LetterboxHelper::OriginalResolution = new Hedgehog::Math::CVector2(originalLetterboxHelper_GetWidth(), LetterboxHelper::OriginalResolution->y());
	return LetterboxHelper::Resolution->x();
}
HOOK(double, __fastcall, LetterboxHelper_GetHeight, 0x675430)
{
	LetterboxHelper::OriginalResolution = new Hedgehog::Math::CVector2(LetterboxHelper::OriginalResolution->x(), originalLetterboxHelper_GetHeight());
	return LetterboxHelper::Resolution->y();
}
HOOK(int, __cdecl, TestSpec, 0x682770, DWORD a1, int type, int a3, byte* a4, float a5, float offset)
{
	return originalTestSpec(a1, 1, a3, a4, a5, offset);
}
void LetterboxHelper::setResolution(float resW = 1280, float resH = 720)
{
	Resolution = new Hedgehog::Math::CVector2(resW, resH);
}
void LetterboxHelper::scaleScene(Chao::CSD::CScene* scene)
{
	/*printf("\n\n%f", 1.0f / (4.0f / 3.0f));
	scene->SetScale(1.0f / (4.0f / 3.0f), 1);*/
}
void LetterboxHelper::initialize(float resW = 1280, float resH = 720)
{
	RECT rc;
	GetWindowRect(GetDesktopWindow(), &rc);
	NativeResolution = new Hedgehog::Math::CVector2(rc.right, rc.bottom);
	Resolution = new Hedgehog::Math::CVector2(resW, resH);
	OriginalResolution = new Hedgehog::Math::CVector2(0, 0);
	ScreenHalfPoint = new Hedgehog::Math::CVector2(Resolution->x() / 2, Resolution->y() / 2);
	INSTALL_HOOK(LetterboxHelper_GetWidth);
	INSTALL_HOOK(LetterboxHelper_GetHeight);
}