#pragma once

#include <d3d9.h>
namespace SUC::ImGuiMenu
{
	enum class Backend
	{
		Unknown,
		DX9,
		DX11
	};

	class DebugMenuRenderer
	{
		static std::string ms_ModDirectoryPath;
		static std::string ms_ImGuiIniPath;
		static HWND ms_HwndWindow;
		static IUnknown* ms_Device;
		static Backend ms_Backend;
		static ImFont* ms_PrimaryFont;
		static ImFont* ms_SubtitleFont;
	public:
		static bool LoadTextureFromFile(const wchar_t* filename, IUnknown** out_texture, int* out_width = nullptr, int* out_height = nullptr);
		static std::string GetModDirectoryPath();
		static void SetModDirectoryPath(const std::string& value);
		static bool IsInitialized();
		static void Initialize(HWND window, IUnknown* device);
		static void Update();
		static LRESULT WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
	};
}
