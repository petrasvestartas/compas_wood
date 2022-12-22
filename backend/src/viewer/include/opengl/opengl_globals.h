#ifndef OPENGL_GLOBALS_H
#define OPENGL_GLOBALS_H
// #include "../../../stdafx.h"
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>

namespace opengl_globals
{

	// debug
	extern bool state_changed;
	extern bool debug;

	// camera type
	//  = opengl_cameras::CAMERA_WALK_PERSPECTIVE;//ARCBALL_PERSPECTIVE

	// mouse position
	extern float xoffset;
	extern float yoffset;

	// window
	extern glm::vec4 background;
	extern unsigned int SCR_FULL_WIDTH;
	extern unsigned int SCR_FULL_HEIGHT;

	//  extern unsigned int SCR_WIDTH = SCR_FULL_WIDTH / 2 + 500;
	//  extern unsigned int SCR_HEIGHT = SCR_FULL_HEIGHT - 175;
	extern unsigned int SCR_WIDTH;
	extern unsigned int SCR_HEIGHT;

	//  extern  unsigned int SCR_CORNER_X = SCR_FULL_WIDTH / 2 - 500;
	//  extern  unsigned int SCR_CORNER_Y = 70;
	extern unsigned int SCR_CORNER_X;
	extern unsigned int SCR_CORNER_Y;

	extern float lastX;
	extern float lastY;

	// lights
	extern int shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode;
	extern glm::vec3 light_pos;
	extern glm::vec3 light_color;

	// camera
	extern glm::vec3 bbox_min;
	extern glm::vec3 bbox_max;
	extern glm::vec3 camera_position;
	extern glm::mat4 projection;
	extern glm::mat4 view;
	extern glm::mat4 model;
	extern glm::mat4 projection_view;

	// shaders
	extern float time;
	extern glm::vec4 white;
	extern glm::vec4 light_grey;
	extern glm::vec4 black;
	// glm::vec3 object_color(1.0f, 0.5f, 0.31f);
	extern glm::vec4 object_color;
	extern std::string shaders_folder;

	// directories of shaders in "Release" folder
	extern std::string filename_basic_light_vertex_shader;
	extern std::string filename_basic_light_fragment_shader;
	extern std::string filename_basic_cube_vertex_shader;
	extern std::string filename_basic_cube_fragment_shader;

	extern std::string filename_line_vertex_shader;
	extern std::string filename_line_fragment_shader;
	extern std::string filename_line_geometry_shader;

	extern std::string filename_pointcloud_vertex_shader;
	extern std::string filename_pointcloud_fragment_shader;
	extern std::string filename_pointcloud_geometry_shader;

	extern std::string filename_explode_vertex_shader;
	extern std::string filename_explode_fragment_shader;
	extern std::string filename_explode_geometry_shader;

	extern std::string filename_vertex_shader;
	extern std::string filename_fragment_shader;
	extern std::string filename_geometry_shader;

	extern std::string filename_normal_vertex_shader;
	extern std::string filename_normal_fragment_shader;
	extern std::string filename_normal_geometry_shader;

	// folder of 3d models of meshes (just directory ending)
	extern std::string filename_model_folder_and_file_name;
	extern std::string filename_and_folder_screenshot;

	// preview
	// transformation matrix
	extern float default_line_thickness;

	// keyboard and mouse
	extern bool enable_mouse;

	// this function needs to go to shader
	void xyz(float &x, float &y, float &z);

	extern std::vector<float> geo_triangle_vertices_and_colors;

	extern std::vector<float> geo_triangle_vertices_and_colors_1;

	extern std::vector<unsigned int> geo_triangle_indices;

	extern std::vector<float> geo_rect_vertices_and_colors_and_texture;

	extern std::vector<unsigned int> geo_rect_indices;

	extern std::vector<float> geo_cube_vertices;

	extern std::vector<float> geo_cube_v_n;
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