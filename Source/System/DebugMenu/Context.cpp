

const uint32_t* const WIDTH = (uint32_t*)0x1DFDDDC;
const uint32_t* const HEIGHT = (uint32_t*)0x1DFDDE0;

std::string Context::modDirectoryPath;
std::string Context::imGuiIniPath = "ImGui.ini";

HWND Context::window;
IUnknown* Context::device;
ImFont* Context::fontDroidSans;
ImFont* Context::fontSeuratProM;
Backend Context::backend;
std::string Context::getModDirectoryPath()
{
    return modDirectoryPath;
}

void Context::setModDirectoryPath(const std::string& value)
{
    modDirectoryPath = value;
    imGuiIniPath = modDirectoryPath + "/ImGui.ini";
}

bool Context::isInitialized()
{
    return window && device;
}

void Context::initialize(HWND window, IUnknown* device)
{
    Context::window = window;
    Context::device = device;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplWin32_Init(window);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseDrawCursor = true;
    io.IniFilename = imGuiIniPath.c_str();
    io.DisplaySize = { (float)*WIDTH, (float)*HEIGHT };
    ImGuiTheme::ImGuiSetTheme(ImThemeColor(0, 0, 0), ImThemeColor(155, 155, 155), ImThemeColor(200, 200, 200), ImThemeColor(212, 212, 212), ImThemeColor(155, 155, 155));
    const float fontSize = max(16, *HEIGHT / 1080.0f * 16);
    ImFontConfig font_cfg;
    font_cfg.GlyphExtraSpacing.x = -2;
    fontSeuratProM = io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/disk/Fonts/SeuratProM.otf").c_str(), max(42, *HEIGHT / 1080.0f * 42), &font_cfg, io.Fonts->GetGlyphRangesDefault());
    fontDroidSans = io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/disk/Fonts/DroidSans.ttf").c_str(), fontSize, nullptr, io.Fonts->GetGlyphRangesDefault());

    if (fontDroidSans == nullptr)
    {
        DebugDrawText::log("Failed to load DroidSans.ttf", 1);
        fontDroidSans = io.Fonts->AddFontDefault();
    }
    if (fontSeuratProM == nullptr)
    {
        DebugDrawText::log("Failed to load SeuratProM.otf", 1);
        fontSeuratProM = io.Fonts->AddFontDefault();
    }

    ImFontConfig fontConfig;
    fontConfig.MergeMode = true;

    if (!io.Fonts->AddFontFromFileTTF((modDirectoryPath + "/disk/Fonts/DroidSansJapanese.ttf").c_str(), fontSize * 1.25f, &fontConfig, io.Fonts->GetGlyphRangesJapanese()))
        DebugDrawText::log("Failed to load DroidSansJapanese.ttf", 1);

    io.Fonts->Build();

    DebugDrawText::log("UnleashedConversion init success", 1);
    DebugDrawText::log(std::format("UnleashedConversion {0} {1}", __DATE__, __TIME__).c_str(), 10);
}
bool Context::loadTextureFromFile(const wchar_t* filename, IUnknown** out_texture, int* out_width, int* out_height)
{
    HRESULT hr = S_FALSE;

    // Load texture from disk.
    switch (backend)
    {
    case Backend::DX9:
    {
        IDirect3DDevice9* d3d9Device = nullptr;
        if (SUCCEEDED(device->QueryInterface(&d3d9Device)))
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
        if (SUCCEEDED(device->QueryInterface(&d3d11Device)))
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

    printf("[SonicUnleashedConversion] (UIContext) Error reading texture! (0x%08x)\n", hr);
    return false;
}
void Context::update()
{
    switch (backend)
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

    if (GetAsyncKeyState(VK_TAB) & 1)
        ImguiManager::visible ^= true;

    //Draw all elements until the pop with DroidSans
    ImGui::PushFont(fontDroidSans);
    ImguiManager::update();
    ImGui::PopFont();

    //Draw all elements until the pop with SeuratPro (dialogue font)
    ImGui::PushFont(fontSeuratProM);
    SubtitleUI::draw();
    ImGui::PopFont();

    ImGui::EndFrame();
    ImGui::Render();

    CsdImGui::render(ImGui::GetDrawData());
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Context::wndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, Msg, wParam, lParam);
}
