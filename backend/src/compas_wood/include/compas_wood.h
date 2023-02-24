

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
// "stdafx_pybind11.h" file contains all the header files and type shortcuts, without any viewer, just algorithm
//
// the wrapper methods contains:
// converter methods (CGAl->Eigen and Eigen->CGAL),
// primary methods plates methods
// secondary geometry methods for Wood library
//
// all the test files needs to go to test folder e.g. compas_wood_test.py
//
// The development strategy is following:
// 1) Create backend in C++ with a Viewer - Finished
// 2) Create a wrapper in C++ with Pybind11 - In Process
// 3) Setup the Compas Python Environment and that replicated the 2nd step workflow
// 4) Produce documentation - Finished
// 5) Write Grasshopper components
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <stdafx_pybind11.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Converter methods, where np.asarray are mapped to Eigen matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * container to store the vertices and faces, by faces we mean also polyline indices
 */
struct Result
{
    RowMatrixXd vertices;
    RowMatrixXi faces;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Primary methods
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * test functions that has to be run before running the rest of the code to check if the library is working
 */
void test();

/**
 * read_data_set from the xml file, they will be numbered from 0 to n
 */
void get_data_set(int data_set_index);

/**
 * the main method to detect the joinery
 */
void get_connection_zones();

/**
 * get closed mesh from a pair of polylines
 */
void closed_mesh_from_polylines();

/**
 * get closest objects indices by using rtree
 */
void rtree();

/**
 * get joints from the get_connection_zones function without geometry
 */
void joints();

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
