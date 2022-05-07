#pragma once

#include "stdafx.h"

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
    return easy3d::Viewer("viewer_joinery_solver", 4, 3, 2, false, true, 24, 8, (int)(3840 * 0.495), (int)(2160 * 0.9));
}

void viewer_display_polylines(easy3d::Viewer& viewer, std::vector<std::vector<IK::Point_3>>& polylines,int display_specific_id=-1, float line_width = 2.0f) {//, std::vector<std::vector<IK::Point_3>>& polylines

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Convert geometry to easy3d lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //count the number of vertices
    size_t count_v = 0;
    size_t count_e = 0;
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
        //Output specific output
        if (display_specific_id > -1) {
            int panel_id = (int)((i - (i % 2)) * 0.5);
            if (panel_id != display_specific_id) continue;
        }

        for (int j = 0; j < polylines[i].size(); j++) {
            pts.emplace_back(easy3d::vec3((float)polylines[i][j].x(), (float)polylines[i][j].y(), (float)polylines[i][j].z()));
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
    drawable->set_line_width(line_width);
    viewer.add_drawable(drawable);
}


void viewer_display_polylines_tree(easy3d::Viewer& viewer, std::vector<std::vector<std::vector<IK::Point_3>>>& polylines_tree, int display_specific_id = -1) {//, std::vector<std::vector<IK::Point_3>>& polylines

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Convert geometry to easy3d lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //count the number of vertices
    size_t count_v = 0;
    size_t count_e = 0;
    for (auto& polylines : polylines_tree) {
        for (auto& polyline : polylines) {
            count_v += polyline.size();
            count_e += polyline.size() - 1;
            //std::cout << "\n Number of Polylines" << polyline.size();
        }
    }

    std::vector<easy3d::vec3> pts0;
    std::vector<uint32_t> lineIndices0;
    std::vector<easy3d::vec3> pts1;
    std::vector<uint32_t> lineIndices1;
    pts0.reserve((size_t)(count_v * 0.5));
    pts0.reserve((size_t)(count_e * 0.5));
    pts1.reserve((size_t)(count_v*0.5));
    pts1.reserve((size_t)(count_e * 0.5));

    //add vertices
    //add indices
    int id_count0 = 0;
    int id_count1 = 0;
    int tree_count = 0;
    bool flip = true;
    for (auto& polylines : polylines_tree) {

     
            //Output specific output
            if (display_specific_id > -1) {
                tree_count++;
                if (tree_count - 1 != display_specific_id) continue;
            }

            for (size_t i = 0; i < polylines.size(); i++)
            {

                if (flip) {
                    
                    for (int j = 0; j < polylines[i].size(); j++) {
                        pts0.emplace_back(easy3d::vec3((float)polylines[i][j].x(), (float)polylines[i][j].y(), (float)polylines[i][j].z()));
                    }

                    for (int j = 0; j < polylines[i].size() - 1; j++) {
                        lineIndices0.emplace_back(id_count0);
                        lineIndices0.emplace_back(id_count0 + 1);
                        id_count0++;
                    }
                    id_count0++;//go to next polyline
                    
                } else {
                    
                    for (int j = 0; j < polylines[i].size(); j++) {
                        pts1.emplace_back(easy3d::vec3((float)polylines[i][j].x(), (float)polylines[i][j].y(), (float)polylines[i][j].z()));
                    }

                    for (int j = 0; j < polylines[i].size() - 1; j++) {
                        lineIndices1.emplace_back(id_count1);
                        lineIndices1.emplace_back(id_count1 + 1);
                        id_count1++;
                    }
                    id_count1++;//go to next polyline
                }

                flip = !flip;
            }
       
    }


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //add vertices with indicies of lines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto drawable0 = new easy3d::LinesDrawable("lines0");
    drawable0->update_element_buffer(lineIndices0);
    drawable0->update_vertex_buffer(pts0);

    auto drawable1 = new easy3d::LinesDrawable("lines1");
    drawable1->update_element_buffer(lineIndices1);
    drawable1->update_vertex_buffer(pts1);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //update easy3d settings
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    drawable0->set_uniform_coloring(easy3d::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    drawable0->set_line_width(5.0f);
    viewer.add_drawable(drawable0);

    drawable1->set_uniform_coloring(easy3d::vec4(1.0f, 0.75f, 0.0f, 1.0f));
    drawable1->set_line_width(5.0f);
    viewer.add_drawable(drawable1);


}



void viewer_run(easy3d::Viewer& viewer) {
    viewer.set_background_color(easy3d::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    viewer.fit_screen();
    viewer.run();
}
