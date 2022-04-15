#pragma once
#define NOMINMAX
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

#include <iostream>
#include <easy3d/viewer/viewer.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/fileio/resources.h>
#include <easy3d/util/logging.h>
#include "joinery_solver_main.h"

easy3d::Viewer viewer_init() {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize logging.
    // Create the default Easy3D viewer.
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    easy3d::logging::initialize();
    return easy3d::Viewer("viewer_joinery_solver", 4, 3, 2, false, true, 24, 8, 3840 * 0.495, 2160 * 0.9);
}

easy3d::Viewer viewer_disply_polylines(easy3d::Viewer& viewer, std::vector<std::vector<IK::Point_3>>& polylines) {//, std::vector<std::vector<IK::Point_3>>& polylines

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Convert geometry to easy3d lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //count the number of vertices
    int count_v = 0;
    int count_e = 0;
    for (auto& polyline : polylines) {
        count_v += polyline.size();
        count_e += polyline.size() - 1;
        //std::cout << "\n Number of Polylines" << polyline.size();
    }

    std::vector<easy3d::vec3> pts;
    std::vector<uint32_t> lineIndices;
    pts.reserve(count_v);
    pts.reserve(count_e);

    //add vertices
    //add indices
    int id_count = 0;
    for (size_t i = 0; i < polylines.size(); i++)
    {

        for (int j = 0; j < polylines[i].size(); j++) {
            pts.emplace_back(easy3d::vec3(polylines[i][j].x(), polylines[i][j].y(), polylines[i][j].z()));
        }

        for (int j = 0; j < polylines[i].size() - 1; j++) {
            lineIndices.emplace_back(id_count);
            lineIndices.emplace_back(id_count + 1);
            id_count++;
        }
        id_count++;//go to next polyline
    }



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //add vertices with indicies of lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto drawable = new easy3d::LinesDrawable("lines");
    drawable->update_element_buffer(lineIndices);
    drawable->update_vertex_buffer(pts);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //update easy3d settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    drawable->set_uniform_coloring(easy3d::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    drawable->set_line_width(10.0f);
    viewer.add_drawable(drawable);
    viewer.set_background_color(easy3d::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    viewer.fit_screen();
    viewer.run();
}


easy3d::Viewer viewer_disply_polylines_tree(easy3d::Viewer& viewer, std::vector<std::vector<std::vector<IK::Point_3>>>& polylines_tree) {//, std::vector<std::vector<IK::Point_3>>& polylines

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Convert geometry to easy3d lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //count the number of vertices
    int count_v = 0;
    int count_e = 0;
    for (auto& polylines : polylines_tree) {
        for (auto& polyline : polylines) {
            count_v += polyline.size();
            count_e += polyline.size() - 1;
            //std::cout << "\n Number of Polylines" << polyline.size();
        }
    }

    std::vector<easy3d::vec3> pts;
    std::vector<uint32_t> lineIndices;
    pts.reserve(count_v);
    pts.reserve(count_e);

    //add vertices
    //add indices
    int id_count = 0;
    for (auto& polylines : polylines_tree) {
        for (size_t i = 0; i < polylines.size(); i++)
        {

            for (int j = 0; j < polylines[i].size(); j++) {
                pts.emplace_back(easy3d::vec3(polylines[i][j].x(), polylines[i][j].y(), polylines[i][j].z()));
            }

            for (int j = 0; j < polylines[i].size() - 1; j++) {
                lineIndices.emplace_back(id_count);
                lineIndices.emplace_back(id_count + 1);
                id_count++;
            }
            id_count++;//go to next polyline
        }
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //add vertices with indicies of lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto drawable = new easy3d::LinesDrawable("lines");
    drawable->update_element_buffer(lineIndices);
    drawable->update_vertex_buffer(pts);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //update easy3d settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    drawable->set_uniform_coloring(easy3d::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    drawable->set_line_width(10.0f);
    viewer.add_drawable(drawable);
    viewer.set_background_color(easy3d::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    viewer.fit_screen();
    viewer.run();
}
