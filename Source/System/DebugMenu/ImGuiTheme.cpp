

void ImGuiTheme::ImGuiSetTheme(ImThemeColor color_for_text, ImThemeColor color_for_head, ImThemeColor color_for_area, ImThemeColor color_for_body, ImThemeColor color_for_pops)
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Colors[ImGuiCol_Text] = color_for_text.To01Vec4(1.00f);
	style.Colors[ImGuiCol_TextDisabled] = color_for_text.To01Vec4(0.58f);
	style.Colors[ImGuiCol_WindowBg] = color_for_body.To01Vec4(0.95f);
	style.Colors[ImGuiCol_Border] = color_for_body.To01Vec4(0.00f);
	style.Colors[ImGuiCol_BorderShadow] = color_for_body.To01Vec4(0.00f);
	style.Colors[ImGuiCol_FrameBg] = color_for_area.To01Vec4(1.00f);
	style.Colors[ImGuiCol_FrameBgHovered] = color_for_head.To01Vec4(0.78f);
	style.Colors[ImGuiCol_FrameBgActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_TitleBg] = color_for_area.To01Vec4(1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = color_for_area.To01Vec4(0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = color_for_area.To01Vec4(0.47f);
	style.Colors[ImGuiCol_ScrollbarBg] = color_for_area.To01Vec4(1.00f);
	style.Colors[ImGuiCol_ScrollbarGrab] = color_for_head.To01Vec4(0.21f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = color_for_head.To01Vec4(0.78f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_CheckMark] = color_for_head.To01Vec4(0.80f);
	style.Colors[ImGuiCol_SliderGrab] = color_for_head.To01Vec4(0.50f);
	style.Colors[ImGuiCol_SliderGrabActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_Button] = color_for_head.To01Vec4(0.50f);
	style.Colors[ImGuiCol_ButtonHovered] = color_for_head.To01Vec4(0.86f);
	style.Colors[ImGuiCol_ButtonActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_Header] = color_for_head.To01Vec4(0.76f);
	style.Colors[ImGuiCol_HeaderHovered] = color_for_head.To01Vec4(0.86f);
	style.Colors[ImGuiCol_HeaderActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = color_for_head.To01Vec4(0.15f);
	style.Colors[ImGuiCol_ResizeGripHovered] = color_for_head.To01Vec4(0.78f);
	style.Colors[ImGuiCol_ResizeGripActive] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_PlotLines] = color_for_text.To01Vec4(0.63f);
	style.Colors[ImGuiCol_PlotLinesHovered] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = color_for_text.To01Vec4(0.63f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = color_for_head.To01Vec4(1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = color_for_head.To01Vec4(0.43f);
	style.Colors[ImGuiCol_PopupBg] = color_for_pops.To01Vec4(0.92f);
}