
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
// PROTOCOL FOR IMPLEMENTING NEW TESTS
// STEP 1 - Example method, with Doxygen type comments in the header files and naming: "type_joinery_plates_name_hexbox_and_corner"
// STEP 2 - GoogleTest method
// STEP 3 - Screenshot , click "GLFW_KEY_KP_0" to save the screenshot wood_globals.cpp -> data_set_input_folder (C:\IBOIS57\_Code\Software\Python\compas_wood\backend\src\wood\dataset)
// STEP 4 - Run all the tests in the main.cpp like this:  wood_test::run_all_tests();
// NOTE - file-paths that are different from pc to pc, please change the path before running the code in wood_globals.cpp -> data_set_input_folder
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WOOD_TEST_H
#define WOOD_TEST_H

// data structure
#include "wood_cut.h"
#include "wood_main.h"

// viewer
#include "viewer_wood.h"
#include "wood_xml.h"

// joinery
#include "wood_joint_lib.h"
#include "wood_joint.h"

// geometry methods
#include "cgal_mesh_boolean.h"
#include <polylabel/cgal_polylabel.h>
#include "cgal_rectangle_util.h"

// 3rd party libraries
#include <gtest/gtest.h>

namespace wood_test
{

    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joinery - helper methods (display, etc.)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         *  this function is run first
         * set properties for saving the screenshot files e.g. "C:\IBOIS57\_Code\Software\Python\compas_wood\backend\src\wood\dataset"
         */
        void set_file_path_for_input_xml_and_screenshot(const std::string &function_name);

        /**
         *  this function is run first
         * set properties for saving the screenshot and input xml files e.g. "C:\IBOIS57\_Code\Software\Python\compas_wood\backend\src\wood\dataset"
         */
        void set_file_path_for_input_xml_and_screenshot(std::vector<std::vector<IK::Point_3>> &input_polyline_pairs, const std::string &function_name);

        /**
         * this function is run at the end of the example
         * define OpenGL vizualizer parameters
         */
        void set_plate_display(std::vector<std::vector<IK::Point_3>> &input_polyline_pairs, std::vector<std::vector<CGAL_Polyline>> &output_plines);

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // plate methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * example for plates
     * hex-box from the Rossiniere project development and a simple rectangle corner seen in Annen project
     */
    bool type_plates_name_hexbox_and_corner();

    /**
     * example for plates
     * example for developing screw connection when plates are touching top-to-top
     */
    bool type_plates_name_top_to_top_pairs();

    /**
     * example for plates
     * 4 types of connections for a) simple tenon mortise, b) four linked joints, c) two linked and one not linked joint but touching the wall, existing in vidy vertical walls only, d) two linked and one not linked joint
     */
    bool type_plates_name_joint_linking_vidychapel_corner();

    /**
     * example for plates
     * st loup chapel exaple using tenon-mortise joints
     */
    bool type_plates_name_joint_linking_vidychapel_one_layer();

    /**
     * example for plates
     * two layer st loup chapel in one axis, since the rest of the axis have the same geometrical joinery generation, just angles are different
     */
    bool type_plates_name_joint_linking_vidychapel_one_axis_two_layers();

    /**
     * example for plates
     * st loup chapel in two layers as if it would be done as the vidy theater
     */
    bool type_plates_name_joint_linking_vidychapel_full();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // beam methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * example for beams
     * multiple beams connected in one node, taken from the Phanome project near Zurich, Switzerland
     */
    bool type_beams_name_phanomema_node();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // library of joints
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * example for the joinery library
     * tenon and mortise joint used in vidy theater
     */
    bool type_library_name_ss_e_op_4();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // geometry methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * example for geometry
     * cgal mesh boolean difference using colors to show the cutouts
     */
    bool type_geometry_name_cgal_mesh_boolean_0();

    /**
     * example for geometry
     * get polygon center by an average of points
     */
    bool type_geometry_name_polygon_center();

    /**
     * example for geometry
     * get polygon center using the polylabel algorithm
     */
    bool type_geometry_name_polygon_center_polylabel();

    /**
     * example for geometry
     * get polylabel circle division points
     */
    bool type_geometry_name_circle_ponts_inscribed_in_a_polygon();

    /**
     * example for geometry
     * create minimum bounding box for a set of points using convex hull and cull them using a polygon
     */
    bool type_geometry_name_grid_of_points_in_a_polygon();

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // GoogleTest
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * googletest method that must be run in the main.cpp to check all the functions above
     */
    void run_all_tests();
}

#endif // WOOD_TEST_H