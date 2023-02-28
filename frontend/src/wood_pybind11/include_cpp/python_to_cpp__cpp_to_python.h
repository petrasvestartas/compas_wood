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
// the converter contains:
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
#include "wood_cut.h"
namespace python_to_cpp__cpp_to_python
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // EIGEN for Pybind11 used only at small part for meshing
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PYTHON TO CPP CONVERSIONS
    // These conversion are used in CPP
    // Mesh
    // Polyline
    // std::vector<std::vector<IK::Vector_3>>
    // std::vector<std::vector<int>> and <std::vector<int>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP Mesh
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * create a cgal mesh from vertices and faces
     * use - c++
     *
     * @param [in] vertices - matrix of coordinates
     * @param [in] faces - matrix of indices
     * @return a Mesh data structure -> typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
     */
    Mesh mesh_from_vertices_and_faces(const RowMatrixXd &vertices, const RowMatrixXi &faces);

    /**
     * create a polygoanl mesh from vertices and faces
     * use - c++
     *
     * @param [in] vertices - matrix of coordinates
     * @param [in] faces - matrix of indices
     * @return a Mesh data structure -> typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
     */
    Mesh ngon_from_vertices_and_faces(const RowMatrixXd &vertices, const std::vector<std::vector<int>> &faces);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP Polyline
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * create a vector of polylines
     * use - c++ | in wood: input_polyline_pairs
     *
     * @param [in] vertices - matrix of coordinates
     * @return a vector of polylines
     */
    std::vector<CGAL_Polyline> matrix_to_polylines(const RowMatrixXd &vertices);

    /**
     * create a vector of polylines
     * use - c++ | in wood: input_polyline_pairs
     *
     * @param [in] nested_vector_of_numbers - container of coordinates
     * @param [out] vector_of_polylines - container of polylines
     * @return a vector of polylines
     */
    void coord_to_vector_of_polylines(std::vector<std::vector<double>> &nested_vector_of_numbers, std::vector<std::vector<IK::Point_3>> &vector_of_polylines);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP std::vector<std::vector<IK::Vector_3>>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * create a nested vector of IK::Vector_3
     * use - c++ | in wood: input_insertion_vectors for elements: top, bottom and sides
     *
     * @param [in] vertices - matrix of coordinates
     * @return a vector of vectors of IK::Vector_3
     */
    std::vector<std::vector<IK::Vector_3>> matrix_to_nested_vector(const RowMatrixXd &vertices);

    /**
     * create a nested vector of IK::Vector_3
     * use - c++ | in wood: input_insertion_vectors for elements: top, bottom and sides
     *
     * @param [in] nested_vector_of_numbers - container of coordinates
     * @param [out] nested_vector_of_vectors - container of vectors
     * @return a nested vector of vectors of IK::Vector_3
     */
    void coord_to_vector_of_vectors(std::vector<std::vector<double>> &nested_vector_of_numbers, std::vector<std::vector<IK::Vector_3>> &nested_vector_of_vectors);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP std::vector<std::vector<int>> and <std::vector<int>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * create a vector of vectors of int
     * use - c++ | in wood: input_JOINTS_TYPES and input_three_valence_element_indices_and_instruction
     *
     * @param [in] indices - matrix of indices
     * @return a nested vector of int
     */
    std::vector<std::vector<int>> matrix_to_nested_vector(const RowMatrixXi &indices);

    /**
     * convert matrix a vector integers
     * use - c++ | in wood: input_adjacency
     *
     * @param [in] indices - matrix of indices
     * @return a vector of int
     */
    std::vector<int> matrix_to_vector(const RowMatrixXi &indices);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CPP TO PYTHON CONVERSIONS
    // These conversion are used in Python
    // Mesh
    // Polyline
    // XML
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mesh to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * container to store the vertices and faces, by faces we mean also polyline indices
     * use - python
     */
    struct ResultMesh
    {
        RowMatrixXd vertices;
        RowMatrixXi faces;
    };

    /**
     * convert mesh to coordinates and indices
     * use - python
     *
     * @param [in] mesh - a Mesh data structure -> typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
     * @return a tuple of matrices for coordinates and indices
     */
    std::tuple<RowMatrixXd, RowMatrixXi> mesh_to_vertices_and_faces(const Mesh &mesh);

    /**
     * convert quad mesh to coordinates and indices
     * use - python
     *
     * @param [in] mesh - a Mesh data structure -> typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
     * @return a tuple of matrices for coordinates and indices
     */
    std::tuple<RowMatrixXd, RowMatrixXi> quadmesh_to_vertices_and_faces(const Mesh &mesh);

    /**
     * convert mesh to a struct of vertices and faces
     * use - python
     *
     * @param [in] mesh - a Mesh data structure -> typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
     * @return struct that consists of two matrices for coordinates and indices (Xd and Xi)
     */
    ResultMesh result_from_mesh(const Mesh &mesh);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Polyline to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * convert polylines to a matrix of cordinates
     * use - python
     *
     * @param [in] polylines - nested container of points std::vector<std::vector<IK::Point3d>>
     * @return a matrix of coordinates
     */
    std::vector<RowMatrixXd> result_from_polylines(const std::vector<CGAL_Polyline> &polylines);

    /**
     * convert nested vector of polylines to coordinates
     * use - python
     *
     * @param [in] nested_vector_of_polylines - nested container of points std::vector<std::vector<std::vector<IK::Point3d>>>
     * @param [out] nested_nested_vector_of_coords - nested nested container of points std::vector<std::vector<std::vector<std::vector<double>>>>
     * @return a matrix of coordinates
     */
    void nested_vector_of_polylines_to_coord(std::vector<std::vector<CGAL_Polyline>> &nested_vector_of_polylines, std::vector<std::vector<std::vector<double>>> &nested_nested_vector_of_coords);

    /**
     * convert nested vector of wood_cut::cut_type to nested container of int
     * use - python
     *
     * @param [in] output_types - nested container of wood_cut::cut_type
     * @param [out] output_types_int - nested container of int
     * @return a matrix of coordinates
     */
    void nested_vector_of_cut_type_to_nested_vector_of_int(std::vector<std::vector<wood_cut::cut_type>> &output_types, std::vector<std::vector<int>> &output_types_int);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // XML to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * read a data-set from the folder
     * use - python
     *
     * @param [in] filename -  the name of the file in \compas_wood\backend\src\wood\dataset
     * @return a matrix of coordinates that represents pair of polylines
     */
    std::vector<RowMatrixXd> read_xml_as_polyline_pairs(std::string &filename);
}