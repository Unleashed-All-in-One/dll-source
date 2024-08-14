#include "..\..\UI\SubtitleUI.h"
const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace SUC::ImGuiMenu
{
	
	std::string DebugMenuRenderer::ms_ModDirectoryPath;
	std::string DebugMenuRenderer::ms_ImGuiIniPath = "ImGui.ini";

	HWND DebugMenuRenderer::ms_HwndWindow;
	IUnknown* DebugMenuRenderer::ms_Device;
	ImFont* DebugMenuRenderer::ms_PrimaryFont;
	ImFont* DebugMenuRenderer::ms_SubtitleFont;
	Backend DebugMenuRenderer::ms_Backend;


	std::string DebugMenuRenderer::GetModDirectoryPath()
	{
		return ms_ModDirectoryPath;
	}
	void DebugMenuRenderer::SetModDirectoryPath(const std::string& value)
	{
		ms_ModDirectoryPath = value;
		ms_ImGuiIniPath = ms_ModDirectoryPath + "/ImGui.ini";
	}
	bool DebugMenuRenderer::IsInitialized()
	{
		return ms_HwndWindow && ms_Device;
	}
	void DebugMenuRenderer::Initialize(HWND window, IUnknown* device)
	{
		ms_HwndWindow = window;
		ms_Device = device;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(window);

		ImGuiIO& io = ImGui::GetIO();
		io.MouseDrawCursor = true;
		io.IniFilename = ms_ImGuiIniPath.c_str();
		io.DisplaySize = { (float)*WIDTH, (float)*HEIGHT };
		ImGuiTheme::ImGuiSetTheme(ImThemeColor(0, 0, 0), ImThemeColor(155, 155, 155), ImThemeColor(200, 200, 200), ImThemeColor(212, 212, 212), ImThemeColor(155, 155, 155));
		const float fontSize = max(16, *HEIGHT / 1080.0f * 16);
		ImFontConfig font_cfg;
		font_cfg.GlyphExtraSpacing.x = -2;

		ms_SubtitleFont = io.Fonts->AddFontFromFileTTF((ms_ModDirectoryPath + "/disk/Fonts/SeuratProM.otf").c_str(), max(42, *HEIGHT / 1080.0f * 42), &font_cfg, io.Fonts->GetGlyphRangesDefault());
		ms_PrimaryFont = io.Fonts->AddFontFromFileTTF((ms_ModDirectoryPath + "/disk/Fonts/InterRegular.ttf").c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesDefault());

		if (ms_PrimaryFont == nullptr)
		{
			DebugDrawText::log("Failed to load InterRegular.ttf", 1);
			ms_PrimaryFont = io.Fonts->AddFontDefault();
		}
		if (ms_SubtitleFont == nullptr)
		{
			DebugDrawText::log("Failed to load SeuratProM.otf", 1);
			ms_SubtitleFont = io.Fonts->AddFontDefault();
		}

		ImFontConfig fontConfig;
		fontConfig.MergeMode = true;

		if (!io.Fonts->AddFontFromFileTTF((ms_ModDirectoryPath + "/disk/Fonts/DroidSansJapanese.ttf").c_str(), fontSize * 1.25f, &fontConfig, io.Fonts->GetGlyphRangesJapanese()))
			DebugDrawText::log("Failed to load DroidSansJapanese.ttf", 1);

		io.Fonts->Build();

		DebugDrawText::log(SUC::Format("UnleashedConversion init success", __DATE__, __TIME__));
		DebugDrawText::log(std::format("UnleashedConversion {0} {1}", __DATE__, __TIME__).c_str(), 10);
	}
	bool DebugMenuRenderer::LoadTextureFromFile(const wchar_t* filename, IUnknown** out_texture, int* out_width, int* out_height)
	{
		HRESULT hr = S_FALSE;

		// Load texture from disk.
		switch (ms_Backend)
		{
		case Backend::DX9:
		{
			IDirect3DDevice9* d3d9Device = nullptr;
			if (SUCCEEDED(ms_Device->QueryInterface(&d3d9Device)))
			{
				IDirect3DTexture9* texture = nullptr;
				hr = DirectX::CreateDDSTextureFromFile(d3d9Device, filename, &texture);
				d3d9Device->Release();

				if (hr == S_OK)
				{
					// Retrieve description of the texture surface so we can access its size.
					D3DSURFACE_DESC my_image_desc;
					texture->GetLevelDesc(0, &my_image_desc);
					*out_texture = texture;

					if (out_width)
					{
						*out_width = (int)my_image_desc.Width;
					}

					if (out_height)
					{
						*out_height = (int)my_image_desc.Height;
					}

					return true;
				}
			}
			break;
		}
		case Backend::DX11:
		{
			ID3D11Device* d3d11Device = nullptr;
			if (SUCCEEDED(ms_Device->QueryInterface(&d3d11Device)))
			{
				ID3D11Resource* texture = nullptr;
				ID3D11ShaderResourceView* textureView = nullptr;

				hr = DirectX::CreateDDSTextureFromFile(d3d11Device, filename, &texture, &textureView);
				d3d11Device->Release();

				if (hr == S_OK)
				{
					ID3D11Texture2D* texture2D = (ID3D11Texture2D*)texture;
					D3D11_TEXTURE2D_DESC my_image_desc;
					texture2D->GetDesc(&my_image_desc);
					*out_texture = textureView;

					if (out_width)
					{
						*out_width = (int)my_image_desc.Width;
					}

					if (out_height)
					{
						*out_height = (int)my_image_desc.Height;
					}

					texture2D->Release();
					return true;
				}
			}
			break;
		}
		}
		SUC::LogMessage(SUC::Format("Error reading texture! (0x%08x)", hr));
		return false;
	}
	void DebugMenuRenderer::Update()
	{
		switch (ms_Backend)
		{
		case Backend::DX9:
			ImGui_ImplDX9_NewFrame();
			break;

		case Backend::DX11:
			ImGui_ImplDX11_NewFrame();
			break;
		}

		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();

		if (GetAsyncKeyState(VK_F1) & 1)
			DebugMenu::s_Visible ^= true;


		//Draw all elements until the pop with SeuratPro (dialogue font)
		ImGui::PushFont(ms_SubtitleFont);
		SUC::UI::Event::SubtitleUI::Draw();
		ImGui::PopFont();

		//Draw all elements until the pop with DroidSans
		ImGui::PushFont(ms_PrimaryFont);
		DebugMenu::Draw();
		ImGui::PopFont();



		ImGui::EndFrame();
		ImGui::Render();

		CsdImGui::render(ImGui::GetDrawData());
	}	
	LRESULT DebugMenuRenderer::WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
	{
		return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
	}
}
