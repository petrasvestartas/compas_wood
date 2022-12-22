#include "../../../../stdafx.h"
#include "opengl_globals.h"

namespace opengl_globals
{
    // debug
    bool state_changed = true; // for rerunning the code in the viewer using imgui
    bool debug = true;

    // camera type
    //  = opengl_cameras::CAMERA_WALK_PERSPECTIVE;//ARCBALL_PERSPECTIVE

    // mouse position
    float xoffset = 0;
    float yoffset = 0;

    // window
    glm::vec4 background(1.0f, 1.0f, 1.0f, 0.5f);
    unsigned int SCR_FULL_WIDTH = 3840;
    unsigned int SCR_FULL_HEIGHT = 2160;

    //   unsigned int SCR_WIDTH = SCR_FULL_WIDTH / 2 + 500;
    //   unsigned int SCR_HEIGHT = SCR_FULL_HEIGHT - 175;
    unsigned int SCR_WIDTH = SCR_FULL_WIDTH;
    unsigned int SCR_HEIGHT = SCR_FULL_HEIGHT;

    //    unsigned int SCR_CORNER_X = SCR_FULL_WIDTH / 2 - 500;
    //    unsigned int SCR_CORNER_Y = 70;
    unsigned int SCR_CORNER_X = 0;
    unsigned int SCR_CORNER_Y = 0;

    float lastX = SCR_WIDTH / 2.0f;
    float lastY = SCR_HEIGHT / 2.0f;

    // lights
    int shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 2;
    glm::vec3 light_pos(-10.2f, -10.0f, 10.0f);
    glm::vec3 light_color(0.601f, 0.6f, 0.6f);

    // camera
    glm::vec3 bbox_min;
    glm::vec3 bbox_max;
    glm::vec3 camera_position(0.0f, 0.0f, 0.0f);
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 model;
    glm::mat4 projection_view;

    // shaders
    float time = 0.0f;
    glm::vec4 white(1.0f, 1.0f, 1.0f, 0.5f);
    glm::vec4 light_grey(0.95f, 0.95f, 0.95f, 0.5f);
    glm::vec4 black(0.0f, 0.0f, 0.0f, 0.1f);
    // glm::vec3 object_color(1.0f, 0.5f, 0.31f);
    glm::vec4 object_color(0.95f, 0.95f, 0.95f, 0.5f);
    std::string shaders_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\viewer\\shaders\\";

    // directories of shaders in "Release" folder
    std::string filename_basic_light_vertex_shader = "\\2.2.basic_lighting.vert";
    std::string filename_basic_light_fragment_shader = "\\2.2.basic_lighting.frag";
    std::string filename_basic_cube_vertex_shader = "\\2.2.light_cube.vert";
    std::string filename_basic_cube_fragment_shader = "\\2.2.light_cube.frag";

    std::string filename_line_vertex_shader = "\\9.1.line_shader.vert";
    std::string filename_line_fragment_shader = "\\9.1.line_shader.frag";
    std::string filename_line_geometry_shader = "\\9.1.line_shader.geom";

    std::string filename_pointcloud_vertex_shader = "\\pointcloud_shader.vert";
    std::string filename_pointcloud_fragment_shader = "\\pointcloud_shader.frag";
    std::string filename_pointcloud_geometry_shader = "\\pointcloud_shader.geom";

    std::string filename_explode_vertex_shader = "\\9.2.explode_shader.vert";
    std::string filename_explode_fragment_shader = "\\9.2.explode_shader.frag";
    std::string filename_explode_geometry_shader = "\\9.2.explode_shader.geom";

    std::string filename_vertex_shader = "\\9.3.default.vert";   //"\\3.3.shader.vert";
    std::string filename_fragment_shader = "\\9.3.default.frag"; //"\\3.3.shader.frag";
    std::string filename_geometry_shader = "\\9.3.default.geom"; //"\\3.3.shader.frag";

    std::string filename_normal_vertex_shader = "\\9.3.normal.vert";
    std::string filename_normal_fragment_shader = "\\9.3.normal.frag";
    std::string filename_normal_geometry_shader = "\\9.3.normal.geom";

    // folder of 3d models of meshes (just directory ending)
    std::string filename_model_folder_and_file_name = "\\model\\backpack.obj";
    std::string filename_and_folder_screenshot;

    // preview
    // transformation matrix
    float default_line_thickness = 1.0;

    // keyboard and mouse
    bool enable_mouse = false;

    // this function needs to go to shader
    void xyz(float &x, float &y, float &z)
    {
        float x_ = x;
        float y_ = y;
        float z_ = z;
        x = x_;
        y = y_;
        z = z_;
    }

    std::vector<float> geo_triangle_vertices_and_colors = {
        // positions         // colors
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f    // top
    };

    std::vector<float> geo_triangle_vertices_and_colors_1 = {
        // positions         // colors
        0.5f + 0.5f, -0.5f + 0.5f, 0.0f + 0.5f, 1.0f, 0.0f, 0.0f,  // bottom right
        -0.5f + 0.5f, -0.5f + 0.5f, 0.0f + 0.5f, 0.0f, 1.0f, 0.0f, // bottom left
        0.0f + 0.5f, 0.5f + 0.5f, 0.0f + 0.5f, 0.0f, 0.0f, 1.0f    // top
    };

    std::vector<unsigned int> geo_triangle_indices = {
        0, 1, 2};

    std::vector<float> geo_rect_vertices_and_colors_and_texture = {
        // positions          // colors           // texture coords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f   // top left
    };

    std::vector<unsigned int> geo_rect_indices = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    std::vector<float> geo_cube_vertices = {
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

    std::vector<float> geo_cube_v_n = {
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

namespace wood
{
    // int output_geometry_type = 0;

}