#ifndef OPENGL_GLOBALS_H
#define OPENGL_GLOBALS_H
//#include "../../../stdafx.h"
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

namespace opengl_globals
{
	// debug
	extern bool state_changed;
	static bool debug = true;

	// camera type
	//  = opengl_cameras::CAMERA_WALK_PERSPECTIVE;//ARCBALL_PERSPECTIVE

	// mouse position
	static float xoffset = 0;
	static float yoffset = 0;

	// window
	static glm::vec4 background(1.0f, 1.0f, 1.0f, 0.5f);
	static unsigned int SCR_FULL_WIDTH = 3840;
	static unsigned int SCR_FULL_HEIGHT = 2160;

	// static unsigned int SCR_WIDTH = SCR_FULL_WIDTH / 2 + 500;
	// static unsigned int SCR_HEIGHT = SCR_FULL_HEIGHT - 175;
	static unsigned int SCR_WIDTH = SCR_FULL_WIDTH;
	static unsigned int SCR_HEIGHT = SCR_FULL_HEIGHT;

	// static  unsigned int SCR_CORNER_X = SCR_FULL_WIDTH / 2 - 500;
	// static  unsigned int SCR_CORNER_Y = 70;
	static unsigned int SCR_CORNER_X = 0;
	static unsigned int SCR_CORNER_Y = 0;

	static float lastX = SCR_WIDTH / 2.0f;
	static float lastY = SCR_HEIGHT / 2.0f;

	// lights
	static int shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
	static glm::vec3 light_pos(-10.2f, -10.0f, 10.0f);
	static glm::vec3 light_color(0.601f, 0.6f, 0.6f);

	// camera
	static glm::vec3 bbox_min;
	static glm::vec3 bbox_max;
	static glm::vec3 camera_position(0.0f, 0.0f, 0.0f);
	static glm::mat4 projection;
	static glm::mat4 view;
	static glm::mat4 model;
	static glm::mat4 projection_view;

	// shaders
	static float time = 0.0f;
	static glm::vec4 white(1.0f, 1.0f, 1.0f, 0.5f);
	static glm::vec4 light_grey(0.95f, 0.95f, 0.95f, 0.5f);
	static glm::vec4 black(0.0f, 0.0f, 0.0f, 0.1f);
	// glm::vec3 object_color(1.0f, 0.5f, 0.31f);
	static glm::vec4 object_color(0.95f, 0.95f, 0.95f, 0.5f);
	static std::string shaders_folder;

	// directories of shaders in "Release" folder
	static std::string filename_basic_light_vertex_shader = "\\2.2.basic_lighting.vert";
	static std::string filename_basic_light_fragment_shader = "\\2.2.basic_lighting.frag";
	static std::string filename_basic_cube_vertex_shader = "\\2.2.light_cube.vert";
	static std::string filename_basic_cube_fragment_shader = "\\2.2.light_cube.frag";

	static std::string filename_line_vertex_shader = "\\9.1.line_shader.vert";
	static std::string filename_line_fragment_shader = "\\9.1.line_shader.frag";
	static std::string filename_line_geometry_shader = "\\9.1.line_shader.geom";

	static std::string filename_pointcloud_vertex_shader = "\\pointcloud_shader.vert";
	static std::string filename_pointcloud_fragment_shader = "\\pointcloud_shader.frag";
	static std::string filename_pointcloud_geometry_shader = "\\pointcloud_shader.geom";

	static std::string filename_explode_vertex_shader = "\\9.2.explode_shader.vert";
	static std::string filename_explode_fragment_shader = "\\9.2.explode_shader.frag";
	static std::string filename_explode_geometry_shader = "\\9.2.explode_shader.geom";

	static std::string filename_vertex_shader = "\\9.3.default.vert";	//"\\3.3.shader.vert";
	static std::string filename_fragment_shader = "\\9.3.default.frag"; //"\\3.3.shader.frag";
	static std::string filename_geometry_shader = "\\9.3.default.geom"; //"\\3.3.shader.frag";

	static std::string filename_normal_vertex_shader = "\\9.3.normal.vert";
	static std::string filename_normal_fragment_shader = "\\9.3.normal.frag";
	static std::string filename_normal_geometry_shader = "\\9.3.normal.geom";

	// folder of 3d models of meshes (just directory ending)
	static std::string filename_model_folder_and_file_name = "\\model\\backpack.obj";

	// preview
	// transformation matrix
	static float default_line_thickness = 1.0;

	// keyboard and mouse
	static bool enable_mouse = false;

	// this function needs to go to shader
	inline void xyz(float &x, float &y, float &z)
	{
		float x_ = x;
		float y_ = y;
		float z_ = z;
		x = x_;
		y = y_;
		z = z_;
	}


	static std::vector<float> geo_triangle_vertices_and_colors = {
		// positions         // colors
		0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
		0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f	  // top
	};

	static std::vector<float> geo_triangle_vertices_and_colors_1 = {
		// positions         // colors
		0.5f + 0.5f, -0.5f + 0.5f, 0.0f + 0.5f, 1.0f, 0.0f, 0.0f,  // bottom right
		-0.5f + 0.5f, -0.5f + 0.5f, 0.0f + 0.5f, 0.0f, 1.0f, 0.0f, // bottom left
		0.0f + 0.5f, 0.5f + 0.5f, 0.0f + 0.5f, 0.0f, 0.0f, 1.0f	   // top
	};

	static std::vector<unsigned int> geo_triangle_indices = {
		0, 1, 2};

	static std::vector<float> geo_rect_vertices_and_colors_and_texture = {
		// positions          // colors           // texture coords
		0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,	  // top right
		0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
		-0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f	  // top left
	};

	static std::vector<unsigned int> geo_rect_indices = {
		0, 1, 3, // first triangle
		1, 2, 3	 // second triangle
	};

	static std::vector<float> geo_cube_vertices = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f};

	static std::vector<float> geo_cube_v_n = {
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
		-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
		-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f};
}
// resources

// file formats
//
// ply
// https://github.com/nmwsharp/happly
//
//
// opengl
//
// lines render GL_LINES
//^https://stackoverflow.com/questions/58494179/how-to-create-a-grid-in-opengl-and-drawing-it-with-lines
//
// pointclouds
// https://stackoverflow.com/questions/44687061/how-to-draw-points-efficiently
// https://stackoverflow.com/questions/58348670/trying-to-draw-a-point-cloud-just-a-single-point-in-the-center-in-opengl
// https://learnopengl.com/Getting-started/Shaders
//
// camera
// https://sjbaker.org/steve/omniv/projection_abuse.html
//
//
//

#endif