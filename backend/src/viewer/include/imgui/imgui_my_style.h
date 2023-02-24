//#pragma once
//#include "stdafx.h"
////#include "imgui.h"
////#include "imgui_internal.h"
//
//namespace ImGui {
//	inline void StyleColorsCustom()
//	{
//		ImGuiStyle* style = &ImGui::GetStyle();
//		ImVec4* colors = style->Colors;
//
//		colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
//		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
//		colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.85f);
//		colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
//		colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.92f);
//		colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
//		colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//		colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
//		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.40f);
//		colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.69f);
//		colors[ImGuiCol_TitleBg] = ImVec4(0.9f, 0.9f, 0.9f, 0.83f);
//		colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.9f, 0.9f, 0.87f);
//		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.9f, 0.9f, 0.9f, 0.20f);
//		colors[ImGuiCol_MenuBarBg] = ImVec4(0.9f, 0.9f, 0.9f, 0.80f);
//		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
//		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.40f, 0.30f);
//		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
//		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.60f);
//		colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
//		colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
//		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.60f);
//		colors[ImGuiCol_Button] = ImVec4(0.7f, 0.7f, 0.7f, 0.62f);
//		colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.79f);
//		colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
//		colors[ImGuiCol_Header] = ImVec4(0.90f, 0.90f, 0.90f, 0.45f);
//		colors[ImGuiCol_HeaderHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.80f);
//		colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.53f, 0.80f);
//		colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
//		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
//		colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
//		colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
//		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.78f, 0.78f, 0.60f);
//		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);
//		colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
//		colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
//		colors[ImGuiCol_TabActive] = ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
//		colors[ImGuiCol_TabUnfocused] = ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
//		colors[ImGuiCol_TabUnfocusedActive] = ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
//		colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//		colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
//		colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
//		colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
//		colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
//		colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);   // Prefer using Alpha=1.0 here
//		colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);   // Prefer using Alpha=1.0 here
//		colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
//		colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
//		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
//		colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
//		colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
//		colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
//		colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
//		colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
//	}
//}