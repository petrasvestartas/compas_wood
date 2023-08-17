//

#ifndef OPENGL_GLOBALS_GEOMETRY_H
#define OPENGL_GLOBALS_GEOMETRY_H

namespace weight
{
	const float hair = 1.0f;
	const float thin = 5.0f;
	const float thick = 10.0f;
}

namespace colors
{
	const glm::vec4 red(1.0, 0.0, 0.5, 1.0);
	const glm::vec4 green(0.5, 0.75, 0.0, 1.0);
	const glm::vec4 blue(0.0, 0.5, 1.0, 1.0);
	const glm::vec4 black(0.0, 0.0, 0.0, 1.0);
	const glm::vec4 grey(0.5, 0.5, 0.5, 1.0);
	const glm::vec4 white(1.0, 1.0, 1.0, 1.0);
}

namespace opengl_globals_geometry
{
	// display geometry
	extern opengl_geometry::polylines polylines;
	extern opengl_geometry::meshes meshes;
	extern opengl_geometry::pointclouds pointclouds;

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// pointclouds samples
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	void add_random_pointcloud(int sizex = 168, int sizey = 120);

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // meshes samples
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    void add_random_pipes();
	void add_obj(const std::string &file_name = "\\model\\abb_6700.obj");
	void add_planar_polylines_with_holes_as_one_mesh();
	void add_planar_polylines_without_holes_as_one_mesh();
	void add_random_polylines();
	void add_line();
	void add_square(bool orient = false);
	float lerp(float a, float b, float f);
	void x_axis(std::vector<std::vector<float>> &vertices, float width = 1.0f);
    void z_axis(std::vector<std::vector<float>> &vertices, float width = 1.0f);
    void y_axis(std::vector<std::vector<float>> &vertices, float width = 1.0f);
    void grid_of_lines(std::vector<std::vector<float>> &vertices, float width = 1.0f, int slices = 10);
    void add_grid(bool axes = true);
    

#if defined(gui)

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// robot abb 6700 3.05
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	extern std::vector<std::string> loaded_robots;
	extern std::vector<glm::mat4> loaded_robots_base;
	extern std::vector<glm::mat4> loaded_robots_last_xforms;
	void add_robot_abb_6700(const int &id, const glm::mat4 &loaded_robot_base, const std::string &file_name = "\\model\\abb_6700.obj");
	glm::vec3 robot_abb_6700_translation();
	std::vector<glm::vec3> robot_abb_6700_origins();
	std::vector<glm::vec3> robot_abb_6700_normals();
	void draw_robot_abb_6700(const int &id = 0);
	void draw_robots();

#endif
}

#endif