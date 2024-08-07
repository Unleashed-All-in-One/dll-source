SynchronizedObject** const APPLICATION_DOCUMENT = (SynchronizedObject**)0x1E66B34;

HOOK(LRESULT, __stdcall, WndProc, 0xE7B6C0, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (Context::wndProc(hWnd, Msg, wParam, lParam))
		return true;

	return originalWndProc(hWnd, Msg, wParam, lParam);
}

void SUC::ImGuiMenu::ImguiInitializer::Update()
{
	//CSDCommon::update();
	const SynchronizedObject::Lock lock(*APPLICATION_DOCUMENT);

	if (!lock.getSuccess())
		return;

	const uint32_t applicationDocument = (uint32_t)lock.getObject();
	if (!applicationDocument)
		return;

	if (!Context::isInitialized())
	{
		const uint32_t application = *(uint32_t*)(applicationDocument + 20);
		if (!application)
			return;

		IUnknown* device = *(IUnknown**)(*(uint32_t*)(application + 80) + 8);
		if (!device)
			return;

		HWND window = *(HWND*)(application + 72);
		if (!window)
			return;

		Context::initialize(window, device);
	}

	const uint32_t ImguiManager = *(uint32_t*)(applicationDocument + 516);
	if (!ImguiManager)
		return;

	const uint32_t globalParameterManager = *(uint32_t*)(ImguiManager + 172);
	if (!globalParameterManager)
		return;


	Context::update();
}
void SUC::ImGuiMenu::ImguiInitializer::Initialize()
{
	INSTALL_HOOK(WndProc);
}