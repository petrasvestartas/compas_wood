///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEVELOPER:
// Petras Vestartas, petasvestartas@gmail.com
// Funding: NCCR Digital Fabrication and EPFL
//
// HISTORY:
// 1) The first version was written during the PhD 8928 thesis of Petras Vestartas called:
// Design-to-Fabrication Workflow for Raw-Sawn-Timber using Joinery Solver, 2017-2021
// 2) The translation from C# to C++ was started during the funding of NCCR in two steps
// A - standalone C++ version of the joinery solver and B - integration to COMPAS framework (Python Pybind11)
//
// RESTRICTIONS:
// The code cannot be used for commercial reasons
// If you would like to use or change the code for research or educational reasons,
// please contact the developer first
//
// CURRENT FILE NOTES:
// there custom tool-paths that are different from pc to pc
// please change the path before running the code in
// wood_globals.cpp -> data_set_input_folder
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../../stdafx.h"
#include "wood_test.h"

namespace wood_test
{

    void test_cgal_mesh_boolean_0()
    {
        cgal_mesh_boolean::mesh_boolean_test();
    }

    void test_side_to_top()
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool simple_case = false;
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs; //= wood_data_set::ss_21;

        // Global Parameters and output joint selection and orientation
        double division_length = 300;
        std::vector<double> default_parameters_for_joint_types{
            300,
            0.5,
            8,
            450,
            0.64,
            10,
            450, // top-to-side
            0.5,
            20,
            300,
            0.5,
            30,
            300,
            0.5,
            40,
            300,
            0.5,
            58,
            300,
            1.0,
            60

        };

        bool compute_joints = true;

        int search_type = 0;
        int output_type = 4;
        std::vector<double> scale = {1, 1, 1};

        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
        std::vector<std::vector<int>> input_joint_types;
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
        std::vector<int> input_adjacency;

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // output
        std::vector<std::vector<CGAL_Polyline>> output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(

            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,

            // output
            output_plines,
            output_types,
            top_face_triangulation,

            // Global Parameters
            default_parameters_for_joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        viewer_wood::add(output_plines); // grey
        viewer_wood::add_loft(output_plines);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Render
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // opengl_render::render();
    }

    void test_F_get_connection_zones_D_in_top_to_top_corner()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = -10;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_top_to_top_corner.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        wood_xml::read_xml_polylines(input_polyline_pairs, simple_case);

        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 150; // division_length
        std::cout
            << "\nwood_test -> joint_types\n";
        for (auto &joint_type : joint_types)
            std::cout << joint_type << "\n";
        std::cout << "\n";
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{};
        std::vector<std::vector<int>> input_joint_types{};
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {};

        std::vector<int> input_adjacency = {};
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>>
            output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        // xml_parser::write_xml_polylines(output_plines);
        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 1;

        switch (output_type)
        {
        case (0):
        case (2):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::add_areas(output_plines);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

    void test_F_three_valence_joint_addition_vidy_D_chapel_corner()
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = -10;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_chapel_corner.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        wood_xml::read_xml_polylines(input_polyline_pairs, simple_case);

        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 150; // division_length
        std::cout
            << "\nwood_test -> joint_types\n";
        for (auto &joint_type : joint_types)
            std::cout << joint_type << "\n";
        std::cout << "\n";
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{};
        std::vector<std::vector<int>> input_joint_types{
            {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16}, {-1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16}, {-1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16}, {-1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}, {-1, -1, -1, -1, 15, -1}, {-1, -1, 15, -1, -1, -1}, {-1, -1, -1, -1, -1, 15}, {-1, -1, -1, 15, -1, -1}

        };
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {
            {1},
            // column1 - joint 15
            {6, 7, 9, 8},
            {10, 11, 13, 12},
            {14, 15, 17, 16},
            // column2 - joint 16
            {18, 19, 21, 20},
            {22, 23, 25, 24},
            {26, 27, 29, 28},
            // column3 - joint one direction
            {30, 31, 32, 32}, // 33,32
            {34, 35, 36, 36}, // 37,36
            {38, 39, 40, 40}, // 41,40

        };

        std::vector<int> input_adjacency = {
            // column 0
            0,
            1,
            -1,
            -1,
            2,
            3,
            -1,
            -1,
            4,
            5,
            -1,
            -1,
            // column 1
            6,
            7,
            -1,
            -1,
            8,
            9,
            -1,
            -1,
            10,
            11,
            -1,
            -1,
            12,
            13,
            -1,
            -1,
            14,
            15,
            -1,
            -1,
            16,
            17,
            -1,
            -1,
            // column 2
            18,
            19,
            -1,
            -1,
            20,
            21,
            -1,
            -1,
            22,
            23,
            -1,
            -1,
            24,
            25,
            -1,
            -1,
            26,
            27,
            -1,
            -1,
            28,
            29,
            -1,
            -1,
            // column 3
            30,
            31,
            -1,
            -1,
            32,
            33,
            -1,
            -1,
            34,
            35,
            -1,
            -1,
            36,
            37,
            -1,
            -1,
            38,
            39,
            -1,
            -1,
            40,
            41,
            -1,
            -1,

        };
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>>
            output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        // xml_parser::write_xml_polylines(output_plines);
        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 1;

        switch (output_type)
        {
        case (0):
        case (2):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::add_areas(output_plines);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

    void test_F_three_valence_joint_addition_vidy_D_chapel_one_layer()
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = -10;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_chapel_one_layer.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        wood_xml::read_xml_polylines(input_polyline_pairs, simple_case);

        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 50; // division_length
        std::cout
            << "\nwood_test -> joint_types\n";
        for (auto &joint_type : joint_types)
            std::cout << joint_type << "\n";
        std::cout << "\n";
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{};
        std::vector<std::vector<int>> input_joint_types{};
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {};

        std::vector<int> input_adjacency = {};
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>>
            output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        // xml_parser::write_xml_polylines(output_plines);
        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 1;

        switch (output_type)
        {
        case (0):
        case (2):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::add_areas(output_plines);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

    // Minor correction Joint side-to-side 13, in get_joints_geometry - 4 - merge joints produces outlines with duplicated points
    void test_F_three_valence_joint_addition_vidy_D_chapel_simple()
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = -10;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_chapel_simple.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        wood_xml::read_xml_polylines(input_polyline_pairs, simple_case);
        // double division_length = 300;
        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 50;
        std::cout
            << "\nwood_test -> joint_types\n";
        for (auto &joint_type : joint_types)
            std::cout << joint_type << "\n";
        std::cout << "\n";
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

        };
        std::vector<std::vector<int>> input_joint_types{
            {-1, -1, -1, 16, -1, -1, -1}, {-1, -1, -1, 16, -1, -1, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}

        };
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {
            {1},
            {9, 15, 14, 8},   // long edge
            {16, 10, 11, 17}, // long edge
            {6, 12, 13, 7},   // long edge
            {15, 17, 16, 14}, // wall
            {9, 11, 10, 8},   // wall
            {15, 13, 12, 14}, // wall
            {9, 7, 6, 8},     // wall
            {3, 5, 4, 2},     // wall
            {3, 1, 0, 2}      // wall
        };

        std::vector<int> input_adjacency = {
            // long edge
            9,
            15,
            -1,
            -1,
            14,
            8,
            -1,
            -1,
            // long edge
            16,
            10,
            -1,
            -1,
            11,
            17,
            -1,
            -1,
            // long edge
            6,
            12,
            -1,
            -1,
            13,
            7,
            -1,
            -1,
            // wall
            15,
            17,
            -1,
            -1,
            14,
            16,
            -1,
            -1,
            // wall
            9,
            11,
            -1,
            -1,
            8,
            10,
            -1,
            -1,
            // wall
            15,
            13,
            -1,
            -1,
            14,
            12,
            -1,
            -1,
            // wall
            9,
            7,
            -1,
            -1,
            8,
            6,
            -1,
            -1,
            // wall
            3,
            5,
            -1,
            -1,
            2,
            4,
            -1,
            -1,
            // wall
            3,
            1,
            -1,
            -1,
            2,
            0,
            -1,
            -1

        };
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>>
            output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        // xml_parser::write_xml_polylines(output_plines);
        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 1;

        switch (output_type)
        {
        case (0):
        case (2):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::add_areas(output_plines);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

    void test_F_three_valence_joint_addition_vidy_D_chapel()
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = -10;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_chapel.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        wood_xml::read_xml_polylines(input_polyline_pairs, simple_case);
        // double division_length = 300;
        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 50;
        std::cout
            << "\nwood_test -> joint_types\n";
        for (auto &joint_type : joint_types)
            std::cout << joint_type << "\n";
        std::cout << "\n";
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{
            {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 133.378166, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}}

        };
        std::vector<std::vector<int>> input_joint_types{
            {-1, -1, -1, 16, -1, -1, -1}, {-1, -1, -1, 16, -1, -1, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, -1, 15, -1, 16, -1, -1, -1, 16}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, 15, -1, 16, -1, -1, -1}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, -1, 16, -1, 15, -1, 16}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, 15, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, -1, -1, 16, -1, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, 16, -1, -1, -1, 16, -1}, {-1, -1, -1, 16, -1, -1, -1}, {-1, -1, -1, 16, -1, -1, -1}

        };
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {
            {1},
            {3, 1, 0, 2},
            {3, 5, 4, 2},
            {9, 7, 6, 8},
            {9, 11, 10, 8},
            {15, 13, 12, 14},
            {15, 17, 16, 14},
            {21, 19, 18, 20},
            {21, 23, 22, 20},
            {27, 25, 24, 26},
            {27, 29, 28, 26},
            {33, 31, 30, 32},
            {33, 35, 34, 32},
            {39, 37, 36, 38},
            {39, 41, 40, 38},
            {45, 43, 42, 44},
            {45, 47, 46, 44},
            {51, 49, 48, 50},
            {51, 53, 52, 50},
            {57, 55, 54, 56},
            {57, 59, 58, 56},
            {63, 61, 60, 62},
            {63, 65, 64, 62},
            {69, 67, 66, 68},
            {69, 71, 70, 68},
            {75, 73, 72, 74},
            {75, 77, 76, 74},
            {81, 79, 78, 80},
            {81, 83, 82, 80},
            {6, 12, 13, 7},
            {18, 24, 25, 19},
            {30, 36, 37, 31},
            {42, 48, 49, 43},
            {54, 60, 61, 55},
            {66, 72, 73, 67},
            {9, 15, 14, 8},
            {21, 27, 26, 20},
            {33, 39, 38, 32},
            {45, 51, 50, 44},
            {57, 63, 62, 56},
            {69, 75, 74, 68},
            {10, 16, 17, 11},
            {22, 28, 29, 23},
            {34, 40, 41, 35},
            {46, 52, 53, 47},
            {58, 64, 65, 59},
            {70, 76, 77, 71}

        };

        std::vector<int> input_adjacency = {3, 1, -1, -1, 2, 0, -1, -1, 3, 5, -1, -1, 2, 4, -1, -1, 9, 7, -1, -1, 8, 6, -1, -1, 9, 11, -1, -1, 8, 10, -1, -1, 15, 13, -1, -1, 14, 12, -1, -1, 15, 17, -1, -1, 14, 16, -1, -1, 21, 19, -1, -1, 20, 18, -1, -1, 21, 23, -1, -1, 20, 22, -1, -1, 27, 25, -1, -1, 26, 24, -1, -1, 27, 29, -1, -1, 26, 28, -1, -1, 33, 31, -1, -1, 32, 30, -1, -1, 33, 35, -1, -1, 32, 34, -1, -1, 39, 37, -1, -1, 38, 36, -1, -1, 39, 41, -1, -1, 38, 40, -1, -1, 45, 43, -1, -1, 44, 42, -1, -1, 45, 47, -1, -1, 44, 46, -1, -1, 51, 49, -1, -1, 50, 48, -1, -1, 51, 53, -1, -1, 50, 52, -1, -1, 57, 55, -1, -1, 56, 54, -1, -1, 57, 59, -1, -1, 56, 58, -1, -1, 63, 61, -1, -1, 62, 60, -1, -1, 63, 65, -1, -1, 62, 64, -1, -1, 69, 67, -1, -1, 68, 66, -1, -1, 69, 71, -1, -1, 68, 70, -1, -1, 75, 73, -1, -1, 74, 72, -1, -1, 75, 77, -1, -1, 74, 76, -1, -1, 81, 79, -1, -1, 80, 78, -1, -1, 81, 83, -1, -1, 80, 82, -1, -1, 6, 12, -1, -1, 7, 13, -1, -1, 18, 24, -1, -1, 19, 25, -1, -1, 30, 36, -1, -1, 31, 37, -1, -1, 42, 48, -1, -1, 43, 49, -1, -1, 54, 60, -1, -1, 55, 61, -1, -1, 66, 72, -1, -1, 67, 73, -1, -1, 9, 15, -1, -1, 8, 14, -1, -1, 21, 27, -1, -1, 20, 26, -1, -1, 33, 39, -1, -1, 32, 38, -1, -1, 45, 51, -1, -1, 44, 50, -1, -1, 57, 63, -1, -1, 56, 62, -1, -1, 69, 75, -1, -1, 68, 74, -1, -1, 10, 16, -1, -1, 11, 17, -1, -1, 22, 28, -1, -1, 23, 29, -1, -1, 34, 40, -1, -1, 35, 41, -1, -1, 46, 52, -1, -1, 47, 53, -1, -1, 58, 64, -1, -1, 59, 65, -1, -1, 70, 76, -1, -1, 71, 77, -1, -1};
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>> output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;

        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_joint_types,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            joint_types,
            scale,
            search_type,
            output_type,
            0);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        // xml_parser::write_xml_polylines(output_plines);
        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 1;

        switch (output_type)
        {
        case (0):
        case (2):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::add_areas(output_plines);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_pairs); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

    void ss_e_op_4()
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // viewer type and shader location
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals::shader_type_0default_1transparent_2shaded_3shadedwireframe_4wireframe_5normals_6explode = 3;
        opengl_globals::shaders_folder = "C:\\IBOIS57\\_Code\\Software\\CPP\\CMAKE\\super_build\\wood\\src\\viewer\\shaders\\";
        opengl_globals_geometry::add_grid();

        wood::joint joint;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joint parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.name = "ss_e_op_4";
        int number_of_tenons = 4;
        std::array<double, 2> x = {-1.5, 0.5};
        // std::array<double, 2> x_chamfer = {-2.0, 0.5};
        std::array<double, 2> y = {-0.50, 0.50};

        // double z_extension = 0.01;
        std::array<double, 2> z_ext = {-0.5, 0.5};
        number_of_tenons = std::min(50, std::max(2, number_of_tenons)) * 2;
        double step = 1 / ((double)number_of_tenons - 1);
        std::array<double, 2> z = {z_ext[0] + step, z_ext[1] - step};
        number_of_tenons -= 2;
        step = 1 / ((double)number_of_tenons - 1);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Male
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        double t = 0.5;
        for (int j = 0; j < 2; j++)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // memory and variables
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j].resize(2);
            joint.m[j][0].reserve(4 + number_of_tenons * 2);
            int sign = j == 0 ? -1 : 1;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // tenon interpolation
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j][0].emplace_back(sign * x[1], y[j], z_ext[1]);
            joint.m[j][0].emplace_back(x[1] + j * 0.01, y[j], z_ext[1]);

            for (int i = 0; i < number_of_tenons; i++)
            {
                double z_ = z[1] + (z[0] - z[1]) * step * i;

                double z_chamfer = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * ((t * 0.5) + ((1 - t) * 0.2)) : z_ - (z[0] - z[1]) * step * ((t * 0.5) + ((1 - t) * 0.2)); // chamfer
                z_ = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5;                                                           // for scaling down the tenon

                if (i % 2 == 1)                                               // chamfer
                    joint.m[j][0].emplace_back(-0.1 + x[0], y[j], z_chamfer); // chamfer

                joint.m[j][0].emplace_back(x[(i + 1) % 2], y[j], z_);
                joint.m[j][0].emplace_back(x[(i + 0) % 2], y[j], z_);

                if (i % 2 == 0)                                               // chamfer
                    joint.m[j][0].emplace_back(-0.1 + x[0], y[j], z_chamfer); // chamfer
            }

            joint.m[j][0].emplace_back(x[1] + j * 0.01, y[j], z_ext[0]);
            joint.m[j][0].emplace_back(sign * x[1], y[j], z_ext[0]);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // cut outlines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j][1] = {
                IK::Point_3(sign * x[1], y[j], z_ext[1]),
                IK::Point_3(sign * x[1], y[j], z_ext[0]),
            };
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Female
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (int j = 0; j < 2; j++)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // memory and variables
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.f[j].resize(2 + number_of_tenons);
            int sign = j == 0 ? 1 : -1;
            int j_inv = j == 0 ? 1 : 0;
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // main outlines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            joint.f[j][0] =
                {
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                    IK::Point_3(y[j_inv], 3 * y[0], z_ext[1]),
                    IK::Point_3(y[j_inv], 3 * y[0], z_ext[0]),
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[0]),
                };

            joint.f[j][1] =
                {
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                };

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // holes
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            for (int i = 0; i < number_of_tenons; i += 2)
            {
                double z_ = z[1] + (z[0] - z[1]) * step * i;
                z_ = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5; // for scaling down the tenon

                joint.f[j][2 + i].reserve(5);
                joint.f[j][2 + i + 1].reserve(5);
                joint.f[j][2 + i].emplace_back(y[j_inv], y[0], z_);
                joint.f[j][2 + i].emplace_back(y[j_inv], y[1], z_);

                z_ = z[1] + (z[0] - z[1]) * step * (i + 1);
                z_ = i % 2 == 1 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5; // for scaling down the tenon

                joint.f[j][2 + i].emplace_back(y[j_inv], y[1], z_);
                joint.f[j][2 + i].emplace_back(y[j_inv], y[0], z_);

                joint.f[j][2 + i].emplace_back(joint.f[j][2 + i].front());
                // copy
                joint.f[j][2 + i + 1] = joint.f[j][2 + i];
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // boolean
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.m_boolean_type = {wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges};
        joint.f_boolean_type.resize(2 + number_of_tenons);
        joint.f_boolean_type[0] = wood_cut::insert_between_multiple_edges;
        joint.f_boolean_type[1] = wood_cut::insert_between_multiple_edges;
        for (int i = 0; i < number_of_tenons; i += 2)
        {
            joint.f_boolean_type[2 + i] = wood_cut::hole;
            joint.f_boolean_type[2 + i + 1] = wood_cut::hole;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joint for preview
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        viewer_wood::scale = 1.0;
        std::vector<std::vector<CGAL_Polyline>> input_polyline_pairs0;
        input_polyline_pairs0.emplace_back(joint.m[0]);
        input_polyline_pairs0.emplace_back(joint.m[1]);
        viewer_wood::add(input_polyline_pairs0, 0); // grey
        std::vector<std::vector<CGAL_Polyline>> input_polyline_pairs1;
        input_polyline_pairs1.emplace_back(joint.f[0]);
        input_polyline_pairs1.emplace_back(joint.f[1]);
        viewer_wood::add(input_polyline_pairs1, 2); // grey
    }

    void beam_node_0()
    {

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Input for the Main Method
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // global inputs
        wood_globals::joint_line_extension = 0;
        // wood_globals::limit_min_joint_length = 140;
        //  function inputs
        bool simple_case = false;
        wood_xml::path_and_file_for_input_polylines = wood_globals::data_set_input_folder + "in_beam_node_0.xml";
        std::vector<std::vector<IK::Point_3>> input_polyline_axes;
        wood_xml::read_xml_polylines(input_polyline_axes, simple_case);

        std::vector<double> joint_types = wood_globals::joint_types;
        joint_types[1 * 3 + 0] = 150; // division_length
        int search_type = 0;
        int output_type = wood_globals::output_geometry_type; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
        // std::cout << "\n output_type " << output_type << "\n";
        std::vector<double> scale = {1, 1, 1};
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{};
        std::vector<std::vector<int>> input_joint_types{};
        // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {};

        std::vector<int> input_adjacency = {};
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<double>> polylines_segment_radii;
        std::vector<std::vector<IK::Vector_3>> polylines_segment_direction;
        for (int i = 0; i < input_polyline_axes.size(); i++)
        {
            std::vector<double> polyline_segment_radii;
            // std::vector<IK::Vector_3> polyline_segment_direction;
            for (int j = 0; j < input_polyline_axes[i].size(); j++)
            {
                polyline_segment_radii.emplace_back(150);
                // polyline_segment_direction.emplace_back(0.037013, 0.909575, 0.413887);
            }
            polylines_segment_radii.emplace_back(polyline_segment_radii);
            // polylines_segment_direction.emplace_back(polyline_segment_direction);
        }

        std::vector<int> allowed_types{1};

        double min_distance = 20;
        double volume_length = 500;
        double cross_or_side_to_end = 0.91;
        int flip_male = 1;

        // output of wood::joint areas
        std::vector<std::array<int, 4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
        std::vector<std::array<IK::Point_3, 2>> point_pairs;
        std::vector<std::array<CGAL_Polyline, 4>> volume_pairs;
        std::vector<CGAL_Polyline> joints_areas;
        std::vector<int> joints_types;

        // Global Parameters and output wood::joint selection and orientation
        std::vector<std::vector<CGAL_Polyline>> output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        bool compute_joints;
        double division_distance;
        double shift;
        bool use_eccentricities_to_scale_joints;

        wood_main::beam_volumes(
            input_polyline_axes,
            polylines_segment_radii,
            polylines_segment_direction,
            allowed_types,
            min_distance,
            volume_length,
            cross_or_side_to_end,
            flip_male,
            // output of wood::joint areas
            polyline0_id_segment0_id_polyline1_id_segment1_id,
            point_pairs,
            volume_pairs,
            joints_areas,
            joints_types,
            // Global Parameters and output wood::joint selection and orientation
            joint_types,
            output_plines,
            output_types,
            compute_joints,
            division_distance,
            shift,
            output_type,
            use_eccentricities_to_scale_joints);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Export
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        wood_xml::path_and_file_for_output_polylines = wood_globals::data_set_output_file;
        output_plines.clear();
        for (auto &pline : volume_pairs)
            for (int j = 0; j < 4; j++)
                output_plines.emplace_back(std::vector<CGAL_Polyline>{pline[j]});

        wood_xml::write_xml_polylines_and_types(output_plines, output_types);

        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Display
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        opengl_globals_geometry::add_grid();
        viewer_wood::line_thickness = 2;

        switch (output_type)
        {
        case (0):
            viewer_wood::add(input_polyline_axes); // grey
            viewer_wood::add_areas(joints_areas);
            break;
        case (2):
            viewer_wood::add(input_polyline_axes); // grey
            viewer_wood::add(volume_pairs);
            break;
        case (1):
        case (3):
            viewer_wood::add(input_polyline_axes); // grey
            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 0); // grey
            break;

        case (4):

            viewer_wood::line_thickness = 3;
            viewer_wood::add(output_plines, 3);   // grey
            viewer_wood::add_loft(output_plines); // grey
            break;
        }
    }

}