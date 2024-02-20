#pragma once
struct ImThemeColor
{
    float x, y, z;
    ImThemeColor(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; }
    ImVec4 To01Vec4(float alpha)
    {
        return ImVec4(x / 255.0f, y / 255.0f, z / 255.0f, alpha);
    }
};
class ImGuiTheme
{
public:
    static void ImGuiSetTheme(ImThemeColor color_for_text, ImThemeColor color_for_head, ImThemeColor color_for_area, ImThemeColor color_for_body, ImThemeColor color_for_pops);
};