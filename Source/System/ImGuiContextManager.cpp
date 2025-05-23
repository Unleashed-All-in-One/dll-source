SynchronizedObject** const APPLICATION_DOCUMENT = (SynchronizedObject**)0x1E66B34;

namespace SUC::ImGuiMenu
{
	HOOK(LRESULT, __stdcall, WndProc, 0xE7B6C0, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		if (DebugMenuRenderer::WndProc(hWnd, Msg, wParam, lParam))
			return true;

		return originalWndProc(hWnd, Msg, wParam, lParam);
	}
	void ImGuiContextManager::Update()
	{
		//CSDCommon::update();
		const SynchronizedObject::Lock lock(*APPLICATION_DOCUMENT);

		if (!lock.getSuccess())
			return;

		const uint32_t applicationDocument = (uint32_t)lock.getObject();
		if (!applicationDocument)
			return;

		if (!DebugMenuRenderer::IsInitialized())
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

			DebugMenuRenderer::Initialize(window, device);
		}

		const uint32_t ImguiManager = *(uint32_t*)(applicationDocument + 516);
		if (!ImguiManager)
			return;

		const uint32_t globalParameterManager = *(uint32_t*)(ImguiManager + 172);
		if (!globalParameterManager)
			return;


		DebugMenuRenderer::Update();
	}
	void SUC::ImGuiMenu::ImGuiContextManager::Initialize()
	{
		INSTALL_HOOK(WndProc);
	}
}