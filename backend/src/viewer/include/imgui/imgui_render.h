#pragma once
// #include "../../../stdafx.h"

// #include "imgui_my_style.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

namespace imgui_params
{
	static std::array<glm::vec3, 2> robot_translation{
		glm::vec3(-6.7 * 0.5, 0, 0),
		glm::vec3(6.7 * 0.5, 0, 0)};

	static std::vector<std::vector<float>> robot_axes_angles{
		{90.0f,
		 0.0f,
		 0.0f,
		 0.0f,
		 0.0f,
		 0.0f},
		{-90.0f,
		 0.0f,
		 0.0f,
		 0.0f,
		 0.0f,
		 0.0f}};

	static std::vector<std::vector<const char *>> robot_axes_angles_names{
		{
			"IBOIS_axis0",
			"IBOIS_axis1",
			"IBOIS_axis2",
			"IBOIS_axis3",
			"IBOIS_axis4",
			"IBOIS_axis5",
		},
		{
			"CRCL_axis0",
			"CRCL_axis1",
			"CRCL_axis2",
			"CRCL_axis3",
			"CRCL_axis4",
			"CRCL_axis5",
		}};
}

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

inline void StyleColorsCustom()
{
	ImGuiStyle *style = &ImGui::GetStyle();
	ImVec4 *colors = style->Colors;

	colors[ImGuiCol_Text] = ImVec4(0.0f, 0.0f, 0.0f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.85f);
	colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.43f, 0.43f, 0.43f, 0.39f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.47f, 0.47f, 0.47f, 0.40f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.42f, 0.42f, 0.42f, 0.69f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.9f, 0.9f, 0.9f, 0.83f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.9f, 0.9f, 0.87f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.9f, 0.9f, 0.9f, 0.20f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.9f, 0.9f, 0.9f, 0.80f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.60f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.40f, 0.40f, 0.40f, 0.30f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.60f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
	colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.30f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.41f, 0.41f, 0.41f, 0.60f);
	colors[ImGuiCol_Button] = ImVec4(0.7f, 0.7f, 0.7f, 0.62f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.79f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.5f, 0.5f, 0.5f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.90f, 0.90f, 0.90f, 0.45f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.6f, 0.6f, 0.6f, 0.80f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.53f, 0.53f, 0.53f, 0.80f);
	colors[ImGuiCol_Separator] = ImVec4(0.50f, 0.50f, 0.50f, 0.60f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.70f, 0.70f, 0.70f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.78f, 0.78f, 0.78f, 0.60f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.78f, 0.78f, 0.78f, 0.90f);
	// colors[ImGuiCol_Tab] = ImLerp(colors[ImGuiCol_Header], colors[ImGuiCol_TitleBgActive], 0.80f);
	colors[ImGuiCol_Tab] = colors[ImGuiCol_Header];
	colors[ImGuiCol_TabHovered] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_TabActive] = colors[ImGuiCol_TitleBgActive];	  // ImLerp(colors[ImGuiCol_HeaderActive], colors[ImGuiCol_TitleBgActive], 0.60f);
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_TitleBg];		  // ImLerp(colors[ImGuiCol_Tab], colors[ImGuiCol_TitleBg], 0.80f);
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive]; // ImLerp(colors[ImGuiCol_TabActive], colors[ImGuiCol_TitleBg], 0.40f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f); // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);	 // Prefer using Alpha=1.0 here
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.35f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight] = colors[ImGuiCol_HeaderHovered];
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}

static bool show_demo_window = true;
static bool show_another_window = false;
static ImVec4 clear_color = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
static std::array<std::string, 3> line_thickness_names = {"volumes", "joint_area", "joint_geometry"};
static std::array<int, 3> line_thickness = {1, 10, 10};
static float line_colors_0[4] = {0, 0, 0, 0};
static float line_colors_1[4] = {1.0f, 0.2f, 0.5f, 0.0f};
static float line_colors_2[4] = {0, 0.5, 1, 0};
static bool lock = 0, wf_mode = 0;
static int render_type = 0;

inline void start_imgui(const char *&glsl_version, GLFWwindow *&window);
inline void loop_start_imgui();
inline void loop_middle_imgui();
inline void loop_end_imgui(GLFWwindow *&window);
inline void end_imgui();

inline void start_imgui(const char *&glsl_version, GLFWwindow *&window)
{
	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void)io;
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	// io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	StyleColorsCustom();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

#ifdef _WIN32
	ImFont *font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\CascadiaMono.ttf", 25.0f, NULL, NULL);
	IM_ASSERT(font != NULL);
#endif

	float scale_factor = 2.00f;
	ImFontConfig config;
	config.SizePixels = roundf(13.0f * scale_factor);
	io.Fonts->AddFontDefault(&config);
	ImGuiStyle &style = ImGui::GetStyle();
	style.ScaleAllSizes(scale_factor);
	// io.Fonts->AddFontDefault();
	// ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
}
inline void loop_start_imgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	// if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	//		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	//{
	//	static float f = 0.0f;
	//	static int counter = 0;

	//	ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

	//	ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
	//	ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
	//	ImGui::Checkbox("Another Window", &show_another_window);

	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
	//	ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

	//	if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
	//		counter++;
	//	ImGui::SameLine();
	//	ImGui::Text("counter = %d", counter);

	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	//	ImGui::End();
	//}

	//// 3. Show another simple window.
	// if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	// }
}
static float f1 = 0.123f, f2 = 0.0f;
// inline void loop_middle_imgui() {
//	// Create a window called "My First Tool", with a menu bar.
//	bool my_tool_active;
//	ImGui::SetNextWindowPos(ImVec2((float)opengl_globals::SCR_WIDTH - 600.0f, 0));
//	ImGui::SetNextWindowSize(ImVec2(600, (float)opengl_globals::SCR_HEIGHT));
//	//ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar
//	ImGui::Begin("UI", &my_tool_active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar);//ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse ImGuiWindowFlags_NoTitleBar
//
//	//ImGui::SetWindowFontScale(1.5);
//
//	//if (ImGui::BeginMenuBar())
//	//{
//	//	if (ImGui::BeginMenu("File"))
//	//	{
//	//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
//	//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
//	//		if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
//	//		ImGui::EndMenu();
//	//	}
//	//	ImGui::EndMenuBar();
//	//}
//	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//
//	//Slider Robot0
//	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nIRB6700_MH3_175_305_IRC5 AXES IBOIS");
//
//	for (int i = 0; i < 6; i++)
//		ImGui::SliderFloat(imgui_params::robot_axes_angles_names[0][i], &imgui_params::robot_axes_angles[0][i], -180.0f, 180.0f, "ratio = %.3f");
//
//	ImGui::SliderFloat("track_0", &imgui_params::robot_translation[0].x, -6.7f * 0.5f, 6.7f * 0.5f, "ratio = %.01f");
//
//	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nIRB6700_MH3_175_305_IRC5 AXES CRCL");
//	for (int i = 0; i < 6; i++)
//		ImGui::SliderFloat(imgui_params::robot_axes_angles_names[1][i], &imgui_params::robot_axes_angles[1][i], -180.0f, 180.0f, "ratio = %.3f");
//
//	ImGui::SliderFloat("track_1", &imgui_params::robot_translation[1].x, -6.7f * 0.5f, 6.7f * 0.5f, "ratio = %.01f");
//
//	//ImGui::SliderFloat("slider float", &imgui_params::robot_axes_angles[0], 0.0f, 1.0f, "ratio = %.3f");
//	//ImGui::SliderFloat("slider float", &imgui_params::robot_axes_angles[1], 0.0f, 1.0f, "ratio = %.3f");
//	//for (int i = 0; i < 6; i++) {
//	//ImGui::SliderFloat("slider float", &(imgui_params::robot_axes_angles[i]), -180.0f, 180.0f, "ratio = %.3f");
//	//}
//	//ImGui::SliderFloat("slider float (log)", &f2, -10.0f, 10.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
//	////for (int i = 0; i < 6; i++) {
//	//auto slider_name_0 = ("Axis" + std::to_string(0)).c_str();
//	//ImGui::SliderFloat(slider_name_0, &(imgui_params::robot_axes_angles[0]), -180.0f, 180.0f);
//
//	//auto slider_name_1 = ("BAxis" + std::to_string(1)).c_str();
//	//ImGui::SliderFloat(slider_name_1, &(imgui_params::robot_axes_angles[1]), -180.0f, 180.0f);
//
//	//}
//
//	//ImGui::BeginChild("Scrolling");
//	//for (int i = 0; i <  wood_globals::EXISTING_TYPES.size(); i++)
//	//	ImGui::Text( wood_globals::EXISTING_TYPES[i].c_str(), i);
//
//	//ImGui::EndChild();
//	ImGui::End();
// }

inline void loop_middle_imgui()
{
	// Create a window called "My First Tool", with a menu bar.
	bool my_tool_active;
	ImGui::SetNextWindowPos(ImVec2((float)opengl_globals::SCR_WIDTH - (float)opengl_globals::SCR_FULL_WIDTH / 6, 0));
	ImGui::SetNextWindowSize(ImVec2((float)opengl_globals::SCR_FULL_WIDTH / 6, (float)opengl_globals::SCR_HEIGHT));
	// ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar
	ImGui::Begin("UI", &my_tool_active, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar); // ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse ImGuiWindowFlags_NoTitleBar

	// ImGui::SetWindowFontScale(1.5);

	// if (ImGui::BeginMenuBar())
	//{
	//	if (ImGui::BeginMenu("File"))
	//	{
	//		if (ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
	//		if (ImGui::MenuItem("Close", "Ctrl+W")) { my_tool_active = false; }
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMenuBar();
	// }

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CAMERA
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::Button("pe", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_PERSPECTIVE;
	}
	ImGui::SameLine();
	if (ImGui::Button("or", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO;
		ImGui::SameLine();
	}
	ImGui::SameLine();
	if (ImGui::Button("to", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_TOP;
		opengl_cameras::set_camera();
	}
	ImGui::SameLine();
	if (ImGui::Button("bo", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_BOTTOM;
		opengl_cameras::set_camera();
	}
	ImGui::SameLine();
	if (ImGui::Button("le", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_LEFT;
		opengl_cameras::set_camera();
	}
	ImGui::SameLine();
	if (ImGui::Button("ri", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_RIGHT;
		opengl_cameras::set_camera();
	}
	ImGui::SameLine();
	if (ImGui::Button("fr", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_FRONT;
		opengl_cameras::set_camera();
	}
	ImGui::SameLine();
	if (ImGui::Button("ba", ImVec2(40, 40)))
	{
		opengl_cameras::cam_type = opengl_cameras::CAMERA_ARCBALL_ORTHO_BACK;
		opengl_cameras::set_camera();
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// JOINTS_PARAMETERS_AND_TYPES
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (ImGui::CollapsingHeader("DIVISION LENGTH VALUES"))
	{

		// Set shift value
		ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nSELECT DIVISION LENGTH VALUE");
		for (int i = 0; i < wood_globals::JOINT_NAMES.size(); i++)
		{
			float joint_type = (float)wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 0];
			ImGui::SliderFloat((wood_globals::JOINT_NAMES[i] + " division_length").c_str(), &joint_type, 0.1, 400);
			wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 0] = (double)joint_type;
		}
	}

	// IMGUI_DEMO_MARKER("Help");
	if (ImGui::CollapsingHeader("SHIFT VALUES"))
	{

		// Set shift value
		ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nSELECT SHIFT VALUE");
		for (int i = 0; i < wood_globals::JOINT_NAMES.size(); i++)
		{
			float joint_type = (float)wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 1];
			ImGui::SliderFloat((wood_globals::JOINT_NAMES[i] + " shift").c_str(), &joint_type, 0.0, 1.0);
			wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 1] = (double)joint_type;
		}
	}
	// Select joint Types - Sliders
	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nSELECT JOINT TYPE");
	for (int i = 0; i < wood_globals::JOINT_NAMES.size(); i++)
	{
		int joint_type = (int)wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 2];
		ImGui::SliderInt(wood_globals::JOINT_NAMES[i].c_str(), &joint_type, i * 10, (i * 10 + 10) - 1);
		wood_globals::JOINTS_PARAMETERS_AND_TYPES[i * 3 + 2] = (double)joint_type;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// OUTPUT_GEOMETRY_TYPE
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Run, the code below set the state_chaged bool so that the code is rerun inside render pipeline, by calling the function pointer
	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nSOLVER \n0 - joint area \n1 - joint lines \n2 - joint volumes \n3 - joint geometry \n4 - merge joints");
	// ImGui::Checkbox("joint_area_detection", &joint_area_detection);

	// Search type
	// int state_0 =  wood_globals::OUTPUT_GEOMETRY_TYPE;
	ImGui::SliderInt("get_joints_geometry", &wood_globals::OUTPUT_GEOMETRY_TYPE, 0, 4);

	// wood_globals::RUN = ImGui::Button("run", ImVec2(100, 100)); // Buttons return true when clicked (most widgets return true when edited/activated)
	if (ImGui::Button("run", ImVec2(100, 100)))
	{
		opengl_globals::state_changed = true;
		std::cout << "imgui_render -> restart \n";
	}

	// Line thickness and Line Colors
	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nSELECT DISPLAY OPTIONS");

	for (int i = 0; i < line_thickness.size(); i++)
		ImGui::SliderInt(line_thickness_names[i].c_str(), &line_thickness[i], 0, 10);

	ImGui::ColorEdit4(line_thickness_names[0].c_str(), line_colors_0);
	ImGui::ColorEdit4(line_thickness_names[1].c_str(), line_colors_1);
	ImGui::ColorEdit4(line_thickness_names[2].c_str(), line_colors_2);

	// Display contents in a scrolling region
	ImGui::TextColored(ImVec4(0, 0, 0, 1), "\nEXISTING JOINT TYPES");

	ImGui::BeginChild("Scrolling");
	for (int i = 0; i < wood_globals::EXISTING_TYPES.size(); i++)
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1), wood_globals::EXISTING_TYPES[i].c_str(), i);
	ImGui::EndChild();

	ImGui::End();
}
inline void loop_end_imgui(GLFWwindow *&window)
{
	// Rendering
	ImGui::Render();
	int display_w, display_h;
	glfwGetFramebufferSize(window, &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
inline void end_imgui()
{
	// cleanup imgui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}