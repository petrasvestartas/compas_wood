#pragma once
// #include "../../../stdafx.h"

// #include <opengl_shaders.h>
// #include <opengl_cameras.h>
// #include <opengl_meshes.h>
// #include <opengl_polylines.h>
// #include <opengl_pointclouds.h>

namespace opengl_render
{
	////TEMP
	// unsigned int VBO, cubeVAO, lightCubeVAO;//LIGHT
	// opengl_shaders::shader lightingShader;

	////shaders
	// opengl_shaders::shader shader;//TEMP
	// opengl_shaders::shader normal_shader;//TEMP

	// methods
	// void start_glfw();
	// void init_geometry();
	// void loop_start_glfw();
	// void loop_middle_glfw();
	// void loop_end_glfw();
	// void end_glfw();

	// void glfw_error_callback(int error, const char *description);
	// void framebuffer_size_callback(GLFWwindow *window, int width, int height);
	// void mouse_callback(GLFWwindow *window, double xpos, double ypos);
	// void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
	// void processInput(GLFWwindow *window);

	// window
	// GLuint vao, vbo, ibo, my_len;
	static const char *glsl_version;
	static GLFWwindow *window = NULL;

	// mouse
	static float lastX = opengl_globals::SCR_WIDTH / 2.0f;
	static float lastY = opengl_globals::SCR_HEIGHT / 2.0f;
	static bool firstMouse = true;

	// timing
	static float deltaTime = 0.0f;
	static float lastFrame = 0.0f;

	// glfw: whenever the mouse scroll wheel scrolls, this callback is called
	inline void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
	{
		opengl_cameras::camera.ProcessMouseScroll(static_cast<float>(yoffset));

		// zoom
		opengl_cameras::zoom_value += (float)yoffset * 0.01f;
		if (opengl_cameras::cam_type != opengl_cameras::CAMERA_ARCBALL_ORTHO)
			opengl_cameras::camera_arc.zoom(static_cast<float>(yoffset * 0.01f));
	}

	// glfw: whenever the window size changed (by OS or user resize) this callback function executes
	inline void framebuffer_size_callback(GLFWwindow *window, int width, int height)
	{
		// make sure the viewport matches the new window dimensions; note that width and
		// height will be significantly larger than specified on retina displays.
		glViewport(0, 0, width, height);
		opengl_globals::SCR_WIDTH = width;
		opengl_globals::SCR_HEIGHT = height;
	}

	// glfw: whenever the mouse moves, this callback is called
	inline void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
	{
		float xpos = static_cast<float>(xposIn);
		float ypos = static_cast<float>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		opengl_globals::xoffset = xpos - lastX;
		opengl_globals::yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		// arc camera
		// pan
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
			opengl_cameras::camera_arc.pan(glm::vec2(opengl_globals::xoffset * 0.001, opengl_globals::yoffset * 0.001));

		// rotate
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			auto cur_pos = glm::vec2(xpos * 2.f / opengl_globals::SCR_WIDTH - 1.f, 1.f - 2.f * ypos / opengl_globals::SCR_HEIGHT);
			auto prev_pos = glm::vec2(lastX * 2.f / opengl_globals::SCR_WIDTH - 1.f, 1.f - 2.f * lastY / opengl_globals::SCR_HEIGHT);
			opengl_cameras::camera_arc.rotate(prev_pos, cur_pos);
		}

		lastX = xpos;
		lastY = ypos;

		// simple camera
		opengl_cameras::camera.ProcessMouseMovement(opengl_globals::xoffset, opengl_globals::yoffset);
	}

	// initialize opengl
	inline void start_glfw()
	{
		// std::cout << "start_glfw 0\n";
		//  glfw: initialize and configure
		//  ------------------------------
		//  glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			return;
			// std::cout << "start_glfw 1\n";
			//  Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
		// GL ES 2.0 + GLSL 100
		glsl_version = "#version 100";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
		// GL 3.2 + GLSL 150
		glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // Required on Mac
#else
		// GL 3.0 + GLSL 130
		glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
		if (opengl_globals::debug)
		{
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true); // 4.3+ only
		}
		// std::cout << "start_glfw 2\n";
#endif

#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

		// glfw window creation
		// --------------------
		// std::cout << "start_glfw 2.1\n " << opengl_globals::SCR_WIDTH << " " << opengl_globals::SCR_HEIGHT << "\n";
		window = glfwCreateWindow(opengl_globals::SCR_WIDTH, opengl_globals::SCR_HEIGHT, "Viewer", NULL, NULL);
		// std::cout << "start_glfw 2.2\n";
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		// std::cout << "start_glfw 3\n";
		//  Set corner coordinate
		GLFWmonitor *monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode *mode = glfwGetVideoMode(monitor);

		opengl_globals::SCR_FULL_WIDTH = mode->width;
		opengl_globals::SCR_FULL_HEIGHT = mode->height;

		glfwSetWindowPos(window, opengl_globals::SCR_CORNER_X, opengl_globals::SCR_CORNER_Y);
		// std::cout << "start_glfw 4\n";
		if (window == NULL)
			return;

		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);

		// tell GLFW to capture our mouse
		if (opengl_globals::enable_mouse)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// glad: load all OpenGL function pointers
		// ---------------------------------------
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		// std::cout << "start_glfw 5\n";
		//  tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
		stbi_set_flip_vertically_on_load(true);

		glEnable(GL_PROGRAM_POINT_SIZE); // https://www.informit.com/articles/article.aspx?p=1616796&seqNum=4
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	// draw in wireframe

		if (opengl_globals::debug)
		{
			int flags;
			glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
			if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
			{
				std::cout << "\nOPENGL DEBUG MODE SLOW, REMOVE WHEN SHIPPING APPLICATION\n"
						  << std::endl;
				glEnable(GL_DEBUG_OUTPUT);
				glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
				glDebugMessageCallback(opengl_debug::glDebugOutput, nullptr);
				// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
				glDebugMessageControl(GL_DEBUG_SOURCE_API,
									  GL_DEBUG_TYPE_ERROR,
									  GL_DEBUG_SEVERITY_HIGH,
									  0, nullptr, GL_TRUE);
				// glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_ERROR, 0,
				// GL_DEBUG_SEVERITY_MEDIUM, -1, "error message here");
			}
		}
		// std::cout << "start_glfw 6\n";

		// glDisable(GL_DEPTH_TEST);
		// glEnable(GL_CULL_FACE);
		// glEnable(GL_BLEND);
		// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		// glCullFace(GL_FRONT);
		if (opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode == 1)
		{
			// https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Transparent_Textures
			glDisable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			glCullFace(GL_FRONT);
		}
		else
		{
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		// std::cout << "start_glfw 7\n";
	}

	// using sysclock_t = std::chrono::system_clock;

	inline std::string CurrentDate()
	{
		// time_t t = time(NULL);
		// struct tm *tm = localtime(&t);
		// char s[64];
		// strftime(s, sizeof(s), "%c", tm);
		// std::string ss(s);
		// ss.erase(remove(ss.begin(), ss.end(), ' '), ss.end());
		// ss.erase(remove(ss.begin(), ss.end(), ':'), ss.end());

		// std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
		// time_t t = std::chrono::system_clock::to_time_t(p);
		// char str[26];
		// ctime_s(str, sizeof str, &t);

		// return std::string(str);

		std::array<char, 64> buffer;
		buffer.fill(0);
		time_t rawtime;
		time(&rawtime);
		const auto timeinfo = localtime(&rawtime);
		strftime(buffer.data(), sizeof(buffer), "%d-%m-%Y_%H-%M-%S", timeinfo);
		std::string timeStr(buffer.data());
		return timeStr;
	}

	// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
	inline void processInput(GLFWwindow *window)
	{
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			opengl_cameras::camera.ProcessKeyboard(opengl_cameras::FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
			opengl_cameras::camera.ProcessKeyboard(opengl_cameras::BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
			opengl_cameras::camera.ProcessKeyboard(opengl_cameras::LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			opengl_cameras::camera.ProcessKeyboard(opengl_cameras::RIGHT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_KP_0) == GLFW_PRESS)
		{
			// save screenshot in this directory: opengl_globals::filename_and_folder_screenshot
			if (opengl_globals::filename_and_folder_screenshot.size() > 0)
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);
				GLsizei nrChannels = 3;
				GLsizei stride = nrChannels * width;
				stride += (stride % 4) ? (4 - stride % 4) : 0;
				GLsizei bufferSize = stride * height;
				std::vector<char> buffer(bufferSize);
				glPixelStorei(GL_PACK_ALIGNMENT, 4);
				glReadBuffer(GL_FRONT);
				glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer.data());
				stbi_flip_vertically_on_write(true);

				std::cout << "opengl_render.cpp -> processInput: the screenshot is saved here: " << opengl_globals::filename_and_folder_screenshot << "\n";
				stbi_write_png(opengl_globals::filename_and_folder_screenshot.c_str(), width, height, nrChannels, buffer.data(), stride);
			}
			else
			{
				std::cout << "opengl_render.cpp -> processInput: the screenshot path is wrong, check this path: " << opengl_globals::filename_and_folder_screenshot << "\n";
			}
		}

		// TEMP
		if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
			for (int i = 0; i < 1; i++)
			{ // pointclouds._geo.size()
				opengl_globals_geometry::pointclouds._geo[i].draw_dynamic = true;
				for (int j = 0; j < opengl_globals_geometry::pointclouds._geo[i].vertices.size(); j++)
					opengl_globals_geometry::pointclouds._geo[i].vertices[j].position.y += 0.01f;
			}
		else
			for (int i = 0; i < opengl_globals_geometry::pointclouds._geo.size(); i++)
				opengl_globals_geometry::pointclouds._geo[i].draw_dynamic = false;

		// zoom
		if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
		{
			opengl_cameras::camera_arc.zoom(-0.01f);
		}
		else if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
		{
			opengl_cameras::camera_arc.zoom(0.01f);
		}

		// pan
		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS)
			opengl_cameras::camera_arc.pan(glm::vec2(-opengl_cameras::pan_step, 0));
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS)
			opengl_cameras::camera_arc.pan(glm::vec2(opengl_cameras::pan_step, 0));
		if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS)
			opengl_cameras::camera_arc.pan(glm::vec2(0, -opengl_cameras::pan_step));
		if (glfwGetKey(window, GLFW_KEY_KP_8) == GLFW_PRESS)
			opengl_cameras::camera_arc.pan(glm::vec2(0, opengl_cameras::pan_step));

		// rotate
		glm::vec2 a(0.0f, 0.0f);
		glm::vec2 b(0.0f, 0.0f);
		if (glfwGetKey(window, GLFW_KEY_KP_4) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_0))
		{
			a[0] = opengl_cameras::pan_step;
			opengl_cameras::camera_arc.rotate(a, b);
		}
		if (glfwGetKey(window, GLFW_KEY_KP_6) == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_KP_0))
		{
			a[0] = -opengl_cameras::pan_step;
			opengl_cameras::camera_arc.rotate(a, b);
		}
	}

	// glfw: init error if it is
	inline void glfw_error_callback(int error, const char *description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	// initialization of buffers (transfer data from CPU to GPU)
	inline void init_geometry()
	{
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Meshes -> init shader and add vertices with indices to GPU
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		opengl_globals_geometry::meshes.init_geo_in_opengl();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Polylines -> init shader and add vertices with indices to GPU
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		opengl_globals_geometry::polylines.init_geo_in_opengl();

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// Pointclouds -> init shader and add vertices with indices to GPU
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		opengl_globals_geometry::pointclouds.init_geo_in_opengl();
	}

	// while loop  - main properties like clean background
	inline void loop_start_glfw(bool (*function)())
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		glfwPollEvents();

		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		processInput(window);

		// clear background
		glClearColor(opengl_globals::background.x, opengl_globals::background.y, opengl_globals::background.z, opengl_globals::background.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// time
		opengl_globals::time = static_cast<float>(glfwGetTime());

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// check if input is changed then update the geometry
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (opengl_globals::state_changed)
		{

			opengl_globals::state_changed = false;
			opengl_globals_geometry::meshes.clear();
			opengl_globals_geometry::polylines.clear();
			opengl_globals_geometry::pointclouds.clear();
			// opengl_globals_geometry::meshes.clear_geo_in_opengl();
			// opengl_globals_geometry::polylines.clear_geo_in_opengl();
			// opengl_globals_geometry::pointclouds.clear_geo_in_opengl();
			try
			{
				bool result = function(); // add geometry
			}
			catch (...)
			{
				std::cout << "opengl_render.h -> error in function that produces geometry \n";
			}

			init_geometry();
		}
	}

	// while loop - swap buffers and poll events
	inline void loop_end_glfw()
	{
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// clean up geometry <- TODO
	inline void end_glfw()
	{
		opengl_globals_geometry::meshes.clear_geo_in_opengl();
		opengl_globals_geometry::polylines.clear_geo_in_opengl();
		opengl_globals_geometry::pointclouds.clear_geo_in_opengl();
		// glfw: terminate, clearing all previously allocated GLFW resources.
		// ------------------------------------------------------------------
		glfwTerminate();
	}

	// while loop - draw geometry here
	inline void loop_middle_glfw()
	{

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// draw geometry
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// camera
		opengl_cameras::projection_matrix(); // ARCBALL_PERSPECTIVE
											 // print("a");
		// draw

		// opengl_render::meshes.mesh_list.front().xform = glm::translate(glm::vec3(0, 0, opengl_globals::time * 0.1));
		// print(opengl_globals::time);

		opengl_globals_geometry::meshes.draw();

		// print("b");
		// opengl_globals_geometry::polylines._geo.back().xform = opengl_transform::rotate_around_axis(
		//	glm::vec3(-1.190432f, -0.544969f, -0.08755f),
		//	glm::vec3(0.265586f, -0.365549f, 0.892097f),
		//	opengl_globals::time * 1
		//);
		// opengl_globals_geometry::draw_robots();
		//

		// opengl_render::polylines._geo.back().xform = glm::translate(glm::vec3(0, 0, opengl_globals::time * 0.1));
		// print(opengl_globals::time);
		opengl_globals_geometry::polylines.draw();

		// opengl_render::pointclouds._geo.back().xform = glm::translate(glm::vec3(0, 0, opengl_globals::time * 0.1));
		// print(opengl_globals::time);

		// auto axis_positions = opengl_globals_geometry::robot_abb_6700_origins();
		// glm::mat4 sixth_xform = opengl_transform::xy_to_plane(
		//	axis_positions.back().x + 0.0f, axis_positions.back().y - 0.6f, axis_positions.back().z - 0.75f,
		//	0, 0, 1,
		//	0, 1, 0,
		//	1, 0, 0

		//);

		// opengl_globals_geometry::pointclouds._geo.back().xform = opengl_globals_geometry::loaded_robots_last_xforms[0] * sixth_xform;
		opengl_globals_geometry::pointclouds.draw();
	}

	// all steps
	inline void render(bool (*function)())
	{
		// std::cout << "render 0\n";
		start_glfw();
		// std::cout << "render 1\n";
		init_geometry();
		// std::cout << "render 2\n";
#if defined(gui)
		start_imgui(glsl_version, window);
#endif
		// std::cout << "render 3\n";
		while (!glfwWindowShouldClose(window))
		{
			// std::cout << "render 4\n";
			loop_start_glfw(function);
#if defined(gui)
			loop_start_imgui();
			loop_middle_imgui();
#endif
			// std::cout << "render 5\n";
			loop_middle_glfw();
#if defined(gui)
			loop_end_imgui(window);
			// std::cout << "render 6\n";
			loop_end_glfw();
#endif
		}
#if defined(gui)
		end_imgui();
#endif
		end_glfw();
		// std::cout << "render 7\n";
	}
}