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
// NOTE:
// "compas_wood.h" and "compas_wood.cpp" are the only two files that contains pybind11 code, do not create any other files
// "stdafx.h" file contains all the header files and type shortcuts, without any viewer, just algorithm
// "compas.h" and "compas.cpp" contains converter methods (CGAl->Eigen and Eigen->CGAL),
//
// the wrapper methods contains:
// primary methods plates methods
// secondary geometry methods for Wood library
//
// all the test files needs to go to test folder e.g. compas_wood_test.py
//
// The development strategy is following:
// 1) Create backend in C++ with a Viewer - Finished
// 2) Create a wrapper in C++ with Pybind11 - In Process - https://pybind11.readthedocs.io/en/stable/ https://github.com/petrasvestartas/sp4e/tree/main/lectures/week14
// 3) Setup the Compas Python Environment that replicates the 2nd step workflow - Not done
// 4) Produce documentation - Finished
// 5) Write Grasshopper components - Not done
// 6) Write if the code works on MAC and Linux - Not done
//
// COMMENT STYLE
/**
 * test functions that has to be run before running the rest of the code to check if the library is working
 *
 * @param [in] x The first coordinate of the vector
 * @param [in] y The second coordinate of the vector
 * @param [in] z The third coordinate of the vector
 * @return The length of the vector
 */
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

namespace compas_wood
{

    namespace internal
    {

        /**
         * get point on a plane using 3 coordinate
         *
         * @param [in] box 0 - origin, 1 - xaxis, 2 - yaxis, 3 - zaxis, 4 - size, but the function does not use the size, so it can be used for planes too
         * @param [in] s coordinate on the x axis
         * @param [in] t coordinate on the y axis
         * @param [in] c coordinate on the z axis
         * @return point on the plane
         */
        IK::Point_3 point_at(IK::Vector_3 (&box)[5], const double &s, const double &t, const double &c);

        /**
         * get eight corners of a box
         *
         * @param [in] box 0 - origin, 1 - xaxis, 2 - yaxis, 3 - zaxis, 4 - size
         * @param [out] corners corners of a box, bottom rectangle and top rectangle
         */
        void get_corners(IK::Vector_3 (&box)[5], CGAL_Polyline &corners);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Test methods - mainly to chekc if the library is loading and whether the types can be passed between C++ and Python
    // https://github.com/tdegeus/pybind11_examples
    // https://github.com/pybind/pybind11/discussions/4340
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * test functions that has to be run before running the rest of the code to check if the library is working
     */
    void test();

    /**
     * share a vector of int between C++ and Python without copying
     *
     * @param [in] vec a list of integers
     */
    void test_opaque_types_wood_vector_int(std::vector<int> &vec);

    /**
     * share a nested vector of int between C++ and Python without copying
     *
     * @param [in] vec a nested list of integers
     */
    void test_opaque_types_wood_nested_vector_int(std::vector<std::vector<int>> &vec);

    /**
     * share a vector of int between C++ and Python without copying
     *
     * @param [in] vec a list of doubles
     */
    void test_opaque_types_wood_vector_double(std::vector<double> &vec);

    /**
     * share a nested vector of double between C++ and Python without copying
     *
     * @param [in] vec a nested list of doubles
     */
    void test_opaque_types_wood_nested_vector_double(std::vector<std::vector<double>> &vec);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Primary methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * read_data_set from the xml file, they will be numbered from 0 to n
     *
     * @param [in] foldername folder name where all the data sets are stored
     * @param [in] filename_of_dataset file name of the xml file, without the ending
     * @param [out] polylines_coordinates a nest vectors of coordinates, that represents pairs of polylines
     */
    void read_xml_polylines(std::string &foldername, std::string &filename_of_dataset, std::vector<std::vector<double>> &polylines_coordinates);

    /**
     * read_data_set from the xml file, they will be numbered from 0 to n
     *
     * @param [in] foldername folder name where all the data sets are stored
     * @param [in] filename_of_dataset file name of the xml file, without the ending
     * @param [out] input_polyline_pairs_coord vector of polylines in a flat list
     * @param [out] input_insertion_vectors_coord vectors per element face (top,bottom and sides)
     * @param [out] input_JOINTS_TYPES vector of adjacency
     * @param [out] input_three_valence_element_indices_and_instruction vector of adjacency
     * @param [out] input_adjacency vector of adjacency
     *
     */
    void read_xml_polylines_and_properties(
        std::string &foldername,
        std::string &filename_of_dataset,
        std::vector<std::vector<double>> &input_polyline_pairs_coord,
        std::vector<std::vector<double>> &input_insertion_vectors_coord,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency);

    /**
     * the main method to detect the joinery
     * @param [in] pybind11_input_polyline_pairs vector of polylines in a flat list
     * @param [in] pybind11_input_insertion_vectors vectors per element face (top,bottom and sides)
     * @param [in] pybind11_input_JOINTS_TYPES vector of joints types per element face (top,bottom and sides)
     * @param [in] pybind11_input_three_valence_element_indices_and_instruction vector of three valence element indices and instruction
     * @param [in] pybind11_input_adjacency vector of adjacency
     * @param [in] pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES vector of joints parameters and types | (side-to-side edge in-plane), (side-to-side edge out-of-plane), (top-to-side edge plane), (cross cutting in-plane), (top-to-top edge plane), (side-to-side edge rotated), (boundary) | (6*3) | 300, 0.5, 3, 450, 0.64, 15, 450, 0.5, 20, 300, 0.5, 30, 6, 0.95, 40, 300, 1.0, 60
     * @param [in] search_type search type 0 - side-to-side, 1 - cross, 2 - both
     * @param [in] scale scale in x,y,z directions
     * @param [in] output_type 0 - Plate outlines | 1 - wood::joint lines | 2 - wood::joint areas | 3 - wood::joint areas with bounding box | 4 - wood::joint areas with bounding box and cut types
     * @param [out] pybind11_output_plines nested vector of polylines in a flat list
     * @param [out] pybind11_output_types vector of joints types per element face (top,bottom and sides)
     */
    void get_connection_zones(
        // input
        std::vector<std::vector<double>> &pybind11_input_polyline_pairs,
        std::vector<std::vector<double>> &pybind11_input_insertion_vectors,
        std::vector<std::vector<int>> &pybind11_input_JOINTS_TYPES,
        std::vector<std::vector<int>> &pybind11_input_three_valence_element_indices_and_instruction,
        std::vector<int> &pybind11_input_adjacency,
        std::vector<double> &pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES,
        int &search_type,
        std::vector<double> &scale,
        int &output_type,
        // output
        std::vector<std::vector<std::vector<double>>> &pybind11_output_plines,
        std::vector<std::vector<int>> &pybind11_output_types,
        // global_parameters
        std::vector<double>& pybind11_joint_volume_parameters
        );

    /**
     * mesh a list of polylines with holes
     *
     * @param [in] polylines_coordinates a nested vector of doubles, representing a list of polylines
     * @param [out] out_vertices a vector of vertices
     * @param [out] out_normals a vector of normals
     * @param [out] out_triangles a vector of triangles
     */
    void closed_mesh_from_polylines_vnf(
        std::vector<std::vector<double>> &polylines_coordinates,
        std::vector<float> &out_vertices,
        std::vector<float> &out_normals,
        std::vector<int> &out_triangles);

    /**
     * get indices colliding the current elements
     * also output axisaligned bounding box and oriented bounding box
     *
     * @param [in] polylines_coordinates a nested vector of doubles, representing a list of polylines
     * @param [out] out_vertices a vector of vertices
     * @param [out] out_normals a vector of normals
     * @param [out] out_triangles a vector of triangles
     */
    void rtree(
        std::vector<std::vector<double>> &polylines_coordinates,
        std::vector<std::vector<int>> &elements_neighbours,
        std::vector<std::vector<double>> &elements_AABB,
        std::vector<std::vector<double>> &elements_OOBB);

    /**
     * get joints from the get_connection_zones function without geometry
     *
     * @param [in] polylines_coordinates a nested vector of doubles, representing a list of polylines
     * @param [in] search_type search type e.g. 0 - side-to-side, 1 - cross, 2 - both
     * @param [out] element_pairs indices of the elements that are connected
     * @param [out] joint_areas a vector of polyline coordinates
     * @param [out] joint_types type of joint e.g. side-to-side, top-to-side, cross ...
     */
    void joints(
        std::vector<std::vector<double>> &polylines_coordinates,
        int &search_type,
        std::vector<std::vector<int>> &element_pairs,
        std::vector<std::vector<double>> &joint_areas,
        std::vector<int> &joint_types);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Secondary Methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * get proximity lines from two polylines
     */
    void intersecting_sequences_of_dD_iso_oriented_boxes();

    /**
     * get volumes of the beams that represent plate like elements for joinery definition
     */
    void beam_volumes();
}