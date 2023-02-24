#include "../../../../stdafx.h"
#include "opengl_globals_geometry.h"

namespace opengl_globals_geometry
{
    opengl_geometry::polylines polylines;
    opengl_geometry::meshes meshes;
    opengl_geometry::pointclouds pointclouds;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// pointclouds samples
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void opengl_globals_geometry::add_random_pointcloud(int sizex, int sizey)
{
    std::vector<std::vector<float>> points;
    std::vector<std::vector<float>> points_c;

    std::vector<float> points_coord;
    std::vector<float> points_colors;

    points_coord.reserve(3 * sizex * sizey);
    points_colors.reserve(3 * sizex * sizey);

    float scale = 0.055f;
    float offset = (1.0f + std::rand() / ((RAND_MAX + 1u) / 100.0f)) * 10.0f;
    // std::cout << offset;
    int random_variable = std::rand();
    for (int i = 0; i < sizex; i++)
    {
        for (int j = 0; j < sizey; j++)
        {
            for (int k = 0; k < 1; k++)
            {
                points_coord.emplace_back((float)i * 0.01f - 0);

                // int z = 1 + std::rand() / ((RAND_MAX + 1u) / 100);  // Note: 1+rand()%6 is biased
                // points_coord.emplace_back((float)z * 0.1f - 0);
                points_coord.emplace_back((float)j * 0.01f + 0);
                points_coord.emplace_back(((float)k * 0.001f + (float)std::sin(i * scale + offset) * (float)std::sin(i * scale) * (float)std::cos(j * scale)) * 0.2f);

                points_colors.emplace_back((float)std::fmod((float)i * 1.0f / sizex, 1.0f));
                points_colors.emplace_back((float)std::fmod((float)j * 1.0f / sizey, 1.0f));
                points_colors.emplace_back((float)std::fmod((float)j * 1.0f / sizex, 1.0f));
            }
        }
    }

    points.emplace_back(points_coord);
    points_c.emplace_back(points_colors);

    opengl_globals_geometry::pointclouds.add(points_coord, points_colors, 3);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// meshes samples
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void opengl_globals_geometry::add_random_pipes()
{
    // std::vector<Vector3> path = buildSpiralPath(0.5, 0.25, -0.25, 0.25, 2, point_count);  // generate path for pipe
    std::vector<Vector3> path_0 = {Vector3{0.0f, 0, 0}, Vector3{0.1f, 0.0f, 0}, Vector3{0.2f, 0.0f, 0.0}, Vector3{0.3f, 0.0f, 0.05f}, Vector3{0.4f, 0.0f, 0.05f}}; // generate path for pipe
    std::vector<float> radii_0 = {0.05f, 0.06f, 0.07f, 0.08f, 0.05f};

    std::vector<Vector3> path_1 = {Vector3{0.0f, 0, 0.25f}, Vector3{0.1f, 0.0f, 0.25f}, Vector3{0.2f, 0.0f, 0.25f}, Vector3{0.3f, 0.0f, 0.25f}, Vector3{0.4f, 0.0f, 0.25f}}; // generate path for pipe
    std::vector<float> radii_1 = {0.05f, 0.06f, 0.07f, 0.08f, 0.05f};
    Pipe pipe_0(path_0, 8, radii_0);
    Pipe pipe_1(path_1, 8, radii_1);

    // add
    glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 0.5f);
    opengl_globals_geometry::meshes.add(pipe_0.v, pipe_0.n, pipe_0.f, color);
    opengl_globals_geometry::meshes.add(pipe_1.v, pipe_1.n, pipe_1.f, color);
}

void opengl_globals_geometry::add_obj(const std::string &file_name)
{
    std::string model_dir = opengl_globals::shaders_folder + file_name; // backpack.obj
    // std::cout << model_dir << std::endl;
    opengl_globals_geometry::meshes.add(model_dir.c_str(), opengl_globals::object_color);
    // std::cout << opengl_render::meshes.mesh_list.size();
}

void opengl_globals_geometry::add_planar_polylines_with_holes_as_one_mesh()
{
    std::vector<std::vector<std::vector<glm::vec3>>> input_polylines_groups = {
        {{glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0)}, {glm::vec3(0.29937882553208f, 0.520343246235039f, 0), glm::vec3(0.633334102338264f, 0.205456671187076f, 0), glm::vec3(0.545869300673597f, 0.781760535439682f, 0)}},
        {{glm::vec3(0, 0, 1), glm::vec3(1, 0, 1), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1)}, {glm::vec3(0.29937882553208f, 0.520343246235039f, 1), glm::vec3(0.633334102338264f, 0.205456671187076f, 1), glm::vec3(0.545869300673597f, 0.781760535439682f, 1)}},
        {{glm::vec3(-2.07893421271085E-16f, 0.520343246235039f, 0.70062117446792f), glm::vec3(2.84484281128421E-16f, 0.205456671187075f, 0.366665897661737f), glm::vec3(-6.73526351575482E-16f, 0.781760535439681f, 0.454130699326402f)}, {glm::vec3(5.55111512312577E-16f, -1.11022302462514E-16f, 1), glm::vec3(6.66133814775094E-16f, -1.16573417585641E-15f, 5.55111512312578E-16f), glm::vec3(-7.7715611723761E-16f, 0.999999999999999f, -4.44089209850063E-16f), glm::vec3(-7.77156117237611E-16f, 1, 1)}},
        {{glm::vec3(0.29937882553208f, 1, 0.520343246235039f), glm::vec3(0.633334102338264f, 1, 0.205456671187076f), glm::vec3(0.545869300673597f, 1, 0.781760535439682f)}, {glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1)}},
        {{glm::vec3(1, 0.52034324623504f, 0.70062117446792f), glm::vec3(1, 0.205456671187076f, 0.366665897661737f), glm::vec3(0.999999999999999f, 0.781760535439682f, 0.454130699326402f)}, {glm::vec3(1, 1.33226762955019E-15f, 1), glm::vec3(1, 2.77555756156291E-16f, 5.55111512312578E-16f), glm::vec3(0.999999999999999f, 1, -6.66133814775094E-16f), glm::vec3(0.999999999999999f, 1, 1)}},
        {{glm::vec3(0.29937882553208f, 1.11022302462516E-16f, 0.520343246235039f), glm::vec3(0.633334102338264f, 0, 0.205456671187076f), glm::vec3(0.545869300673597f, 1.11022302462516E-16f, 0.781760535439682f)}, {glm::vec3(0, 0, 1.11022302462516E-16f), glm::vec3(1, 0, 1.11022302462516E-16f), glm::vec3(1, 1.11022302462516E-16f, 1), glm::vec3(0, 1.11022302462516E-16f, 1)}},
    };
    opengl_globals_geometry::polylines.add(input_polylines_groups, 5, glm::vec4(0.2f, 0.2f, 0.2f, 0.5f), true, "mesh_outlines");

    opengl_globals_geometry::meshes.add(input_polylines_groups, glm::vec4(1, 0.0f, 0.50f, 0.1f), false, 0.001, 1, "polylne_with_holes");
}

void opengl_globals_geometry::add_planar_polylines_without_holes_as_one_mesh()
{
    std::vector<std::vector<glm::vec3>> input_polylines_groups_no_holes = {
        {glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(1, 1, 0), glm::vec3(0, 1, 0)},
        {glm::vec3(0, 1, 0), glm::vec3(1, 1, 0), glm::vec3(1, 1, 1), glm::vec3(0, 1, 1)},
    };
    opengl_globals_geometry::meshes.add(input_polylines_groups_no_holes, glm::vec4(0.75, 0.75, 0.75, 0.5f), false, 0.001, 0.75, "polylne_without_holes");
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// polylines samples
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void opengl_globals_geometry::add_random_polylines()
{
    std::vector<std::vector<float>> plines_v = {
        {
            -1,
            -1,
            0,
            1,
            -1,
            0,
            1,
            1,
            0,
            -1,
            1,
            0,
            -1,
            -1,
            0,
        },
        {1.028334f, 0.945913f, 1.127935f, -0.002187f, -1.540394f, 1.127935f},
        {1.138932f, -1.488553f, 0.273899f, -0.112785f, 0.894072f, 0.273899f, -1.764648f, 0.438204f, 0.0f},
    };

    std::vector<float> plines_w = {20, 40, 60};

    std::vector<glm::vec4> plines_c = {glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.5f, 1.0f)};

    for (int i = 0; i < plines_w.size(); i++)
    {
        opengl_globals_geometry::polylines.add(plines_v[i], plines_w[i], plines_c[i]);
    }
}

void opengl_globals_geometry::add_line()
{
    std::vector<std::vector<float>> plines_v = {
        {-1, -1, 0, -1, -1, 1},
    };

    std::vector<float> plines_w = {20};

    std::vector<glm::vec4> plines_c = {glm::vec4(1.0f, 0.5f, 0.5f, 0.5f)};

    for (int i = 0; i < plines_w.size(); i++)
    {
        opengl_globals_geometry::polylines.add(plines_v[i], plines_w[i], plines_c[i]);
    }
}

void opengl_globals_geometry::add_square(bool orient)
{
    std::vector<float> plines_v = {
        -1.190432f, -0.544969f, -0.08755f, -0.23036f, -0.360331f, -0.297715f, -0.274305f, 0.095817f, -0.09772f, -0.503533f, 0.516331f, 0.142835f, -0.467288f, 0.466445f, 0.264581f, -0.23806f, 0.04593f, 0.024026f, -0.194115f, -0.410217f, -0.17597f, -1.154188f, -0.594856f, 0.034195f, -1.242077f, 0.317439f, 0.434186f, -1.278322f, 0.367326f, 0.312441f, -1.190432f, -0.544969f, -0.08755f};

    opengl_globals_geometry::polylines.add(plines_v, 10, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// grid
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float opengl_globals_geometry::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void opengl_globals_geometry::x_axis(std::vector<std::vector<float>> &vertices, float width)
{
    vertices.clear();
    vertices.emplace_back(std::vector<float>{0, 0, 0, width * 0.5f, 0, 0});
}
void opengl_globals_geometry::z_axis(std::vector<std::vector<float>> &vertices, float width)
{
    vertices.clear();
    vertices.emplace_back(std::vector<float>{0, 0, 0, 0, 0, width * 0.5f});
}

void opengl_globals_geometry::y_axis(std::vector<std::vector<float>> &vertices, float width)
{
    vertices.clear();
    vertices.emplace_back(std::vector<float>{0, 0, 0, 0, width * 0.5f, 0});
}

void opengl_globals_geometry::grid_of_lines(std::vector<std::vector<float>> &vertices, float width, int slices)
{
    vertices.clear();
    float start = -width * 0.5f;
    float end = width * 0.5f;
    float len = end - start;
    int divisions = slices;
    float step = len / divisions;

    std::vector<float> l00;
    l00.reserve(divisions + 1);
    std::vector<float> l01;
    l01.reserve(divisions + 1);
    std::vector<float> l10;
    l10.reserve(divisions + 1);
    std::vector<float> l11;
    l11.reserve(divisions + 1);
    for (float i = start; i <= end + 0.0001; i += step)
    {
        l00.emplace_back(i);
        l00.emplace_back(start);
        l00.push_back(0);

        l01.emplace_back(i);
        l01.emplace_back(end);
        l01.push_back(0);

        l10.emplace_back(start);
        l10.emplace_back(i);
        l10.push_back(0);

        l11.emplace_back(end);
        l11.emplace_back(i);
        l11.push_back(0);
    }

    for (int i = 0; i < l00.size(); i += 3)
    {
        vertices.emplace_back(
            std::vector<float>{
                l00[i + 0],
                l00[i + 1],
                l00[i + 2],
                l01[i + 0],
                l01[i + 1],
                l01[i + 2],
            });

        vertices.emplace_back(
            std::vector<float>{
                l10[i + 0],
                l10[i + 1],
                l10[i + 2],
                l11[i + 0],
                l11[i + 1],
                l11[i + 2],
            });
    }
}

void opengl_globals_geometry::add_grid(bool axes)
{
    std::vector<std::vector<float>> vertices;

    if (axes)
    {
        x_axis(vertices, 0.2f);
        opengl_globals_geometry::polylines.add(vertices, 5.0f, glm::vec4(1.0, 0.0, 0.0, 1.0));

        y_axis(vertices, 0.2f);
        opengl_globals_geometry::polylines.add(vertices, 5.0f, glm::vec4(0.0, 1.0, 0.72, 1.0));

        z_axis(vertices, 0.2f);
        polylines.add(vertices, 5.0f, glm::vec4(0.0, 0.0, 1.0, 1.0));
    }

    grid_of_lines(vertices, 1.0f, 10);
    polylines.add(vertices, 2.0f, glm::vec4(0.75, 0.75, 0.75, 1.0));
}

#if defined(gui)

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// robot abb 6700 3.05
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<std::string> opengl_globals_geometry::loaded_robots{};

std::vector<glm::mat4> opengl_globals_geometry::loaded_robots_base{};

std::vector<glm::mat4> opengl_globals_geometry::loaded_robots_last_xforms{};

void opengl_globals_geometry::add_robot_abb_6700(const int &id, const glm::mat4 &loaded_robot_base, const std::string &file_name)
{
    std::string model_dir = opengl_globals::shaders_folder + file_name; // backpack.obj
    // std::cout << model_dir << std::endl;
    std::string group_name = "abb_6700_" + std::to_string(id);
    opengl_globals_geometry::loaded_robots.emplace_back(group_name);
    opengl_globals_geometry::loaded_robots_base.emplace_back(loaded_robot_base);
    opengl_globals_geometry::loaded_robots_last_xforms.emplace_back(glm::mat4(1.0f));
    // print(group_name);
    // print(loaded_robots.size());
    opengl_globals_geometry::meshes.add(model_dir.c_str(), opengl_globals::object_color, false, group_name);

    // std::cout << opengl_render::meshes.mesh_list.size();
}

glm::vec3 opengl_globals_geometry::robot_abb_6700_translation()
{
    return glm::vec3(0, 0, 0);
}

std::vector<glm::vec3> opengl_globals_geometry::robot_abb_6700_origins()
{
    return {
        glm::vec3(0, 0, 0),
        glm::vec3(0.32, 0, 0.78),
        glm::vec3(0.32, 0, 1.915),
        glm::vec3(0.32, 0, 2.115),
        glm::vec3(1.9125, 0, 2.115),
        glm::vec3(2.1115, 0, 2.115),
    };
}

std::vector<glm::vec3> opengl_globals_geometry::robot_abb_6700_normals()
{
    return {
        glm::vec3(0, 0, 1),
        glm::vec3(0, 1, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(1, 0, 0),
        glm::vec3(0, 1, 0),
        glm::vec3(1, 0, 0),
    };
}

void opengl_globals_geometry::draw_robot_abb_6700(const int &id)
{
    std::vector<glm::vec3> robot_axes_origins = opengl_globals_geometry::robot_abb_6700_origins();
    std::vector<glm::vec3> robot_axes_normals = opengl_globals_geometry::robot_abb_6700_normals();

    // get ids
    std::vector<int> ids = opengl_globals_geometry::meshes._dict.find(loaded_robots[id])->second;

    // reset previous transformation
    for (int i = 0; i < 7; i++)
        opengl_globals_geometry::meshes._geo[ids[i]].xform = glm::mat4(1.0f);

    for (int i = 1; i < 7; i++)
    {
        // rotate geometry
        glm::mat4 xform_rot(1.0f);

        glm::vec3 curr_origin = robot_axes_origins[i - 1];
        glm::vec3 curr_normal = robot_axes_normals[i - 1];

        // float radians = i == 5 ? opengl_globals::time * 0.1f : imgui_params::robot_axes_angles[i] * (3.14159265359f / 180);
        float radians = imgui_params::robot_axes_angles[id][i - 1] * (3.14159265359f / 180);
        xform_rot = xform_rot * opengl_transform::rotate_around_axis(curr_origin, curr_normal, radians); // opengl_globals::time * 0.01f);

        for (int j = i; j < ids.size(); j++)
        {
            // mesh transformation
            opengl_globals_geometry::meshes._geo[ids[j]].xform = (i == 0) ? xform_rot : xform_rot * opengl_globals_geometry::meshes._geo[ids[j]].xform;
            // loaded_robots_last_xforms[id] = opengl_globals_geometry::meshes._geo[ids[j]].xform;

            // update rotation axis
            glm::vec4 o(robot_axes_origins[j - 1], 1);
            glm::vec4 n(robot_axes_origins[j - 1] + robot_axes_normals[j - 1], 1);
            o = xform_rot * o;
            n = xform_rot * n;
            robot_axes_origins[j - 1] = glm::vec3(o.x, o.y, o.z);
            robot_axes_normals[j - 1] = glm::vec3(n.x, n.y, n.z);
            robot_axes_normals[j - 1] -= robot_axes_origins[j - 1];
        }
    }

    // global transformation
    glm::mat4 track_translation = glm::translate(imgui_params::robot_translation[id]);
    for (int i = 0; i < 7; i++)
    {
        opengl_globals_geometry::meshes._geo[ids[i]].xform = track_translation * loaded_robots_base[id] * opengl_globals_geometry::meshes._geo[ids[i]].xform;
    }

    loaded_robots_last_xforms[id] = opengl_globals_geometry::meshes._geo[ids[6]].xform;
}

void opengl_globals_geometry::draw_robots()
{
    // print("draw_robots");
    for (int i = 0; i < opengl_globals_geometry::loaded_robots.size(); i++)
    {
        opengl_globals_geometry::draw_robot_abb_6700(i);
    }

    // print(group_name);
}
// print("draw_robots");
#endif
