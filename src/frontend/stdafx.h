
#pragma once

#define WOOD_WRAPPER


#ifndef STDAFX_H
#define STDAFX_H


// #define CGAL_NO_ASSERTIONS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STD
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <vector>
#include <array>
#include <map>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <numeric>
#include <limits>
#include <chrono>
#include <float.h>
#include <inttypes.h>
#include <cstring>
#include <set>
#include <unordered_set>
#include <list>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CLIPPER2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <clipper2/clipper.h>



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CGAL
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// https://github.com/CGAL/cgal/issues/7301 mac assertions error:

// #ifdef _LIBCPP_AVAILABILITY_UNCAUGHT_EXCEPTIONS 
// #include <CGAL/assertions.h>

// #ifdef CGAL_destructor_assertion
// #undef CGAL_destructor_assertion
// #define CGAL_destructor_assertion(EX) \
//             ((!CGAL::get_use_assertions() || CGAL::possibly(EX)||std::uncaught_exception())?(static_cast<void>(0)): ::CGAL::assertion_fail( # EX , __FILE__, __LINE__))
// #endif

// #ifdef CGAL_destructor_assertion_catch
// #undef CGAL_destructor_assertion_catch
// #define CGAL_destructor_assertion_catch(CODE) try{ CODE } catch(...) { if(!std::uncaught_exception()) throw; }
// #endif
// #endif

#undef __cpp_lib_uncaught_exceptions
#include <CGAL/assertions.h>

//#include <boost/exception/diagnostic_information.hpp>

//  CGAL predicates https://github.com/CGAL/cgal/discussions/6946
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>

// CGAL intersection, boxes, planes, boolean operations
#include <CGAL/intersections.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Plane_3.h>
#include <CGAL/Boolean_set_operations_2.h>

// CGAL meshing 2D
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

// CGAL closest polylines
#include <CGAL/box_intersection_d.h>
#include <CGAL/Iterator_range.h>
#include <CGAL/tuple.h>
#include <CGAL/boost/iterator/counting_iterator.hpp>

// CGAL mesh boolean
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/IO/polygon_mesh_io.h>
#include <boost/container/flat_map.hpp>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>

using IK = CGAL::Exact_predicates_inexact_constructions_kernel;
using EK = CGAL::Exact_predicates_exact_constructions_kernel;
typedef CGAL::Cartesian_converter<IK, EK> IK_to_EK;
typedef CGAL::Cartesian_converter<EK, IK> EK_to_IK;
using CGAL_Polyline = std::vector<IK::Point_3>;
using CGAL_Polylines = std::list<CGAL_Polyline>;
typedef typename CGAL::Box_intersection_d::Box_with_info_d<double, 3, std::pair<std::size_t, std::size_t>> Box;
using Mesh = CGAL::Surface_mesh<IK::Point_3>;
namespace PMP = CGAL::Polygon_mesh_processing;

struct FaceInfo2
{
    FaceInfo2() {}
    int nesting_level;
    bool in_domain()
    {
        return nesting_level % 2 == 1;
    }
};

typedef CGAL::Triangulation_vertex_base_2<IK> Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, IK> Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<IK, Fbb> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> TDS;
typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<IK, TDS, Itag> CGALCDT;
typedef CGALCDT::Point Point;
typedef CGAL::Polygon_2<IK> Polygon_2;
typedef CGALCDT::Face_handle Face_handle;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WOOD GLOBALS
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "wood_globals.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// BOOST
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/range/const_iterator.hpp>
#include <boost/range/value_type.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/graph/properties.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WOOD RTREE
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include "rtree.h"
#include "rtree_util.h" // here is duplicate symbols
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WOOD
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Order Matters
#include "cgal_box_search.h"
#include "cgal_inscribe_util.h"
#include "cgal_vector_util.h"
#include "cgal_intersection_util.h"
#include "cgal_xform_util.h"

// Order does not matter
#include "cgal_box_util.h"
#include "cgal_math_util.h"
#include "cgal_polyline_mesh_util.h"
#include "cgal_plane_util.h"
#include "clipper_util.h"
#include "cgal_polyline_util.h"
#include "cgal_rectangle_util.h"


#include "wood_cut.h"

#ifndef SKIP_PYBIND11_TO_STDAFX_H
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// compas_wood linkage
// PYBIND11
// Eigen
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <pybind11/pybind11.h> // minimal includes for pybind11
namespace py = pybind11;       // minimal includes for pybind11
#include <pybind11/eigen.h>    //this is needed to enable faster data transfer
#include <pybind11/stl.h>      //for faster binding https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html
#include <pybind11/stl_bind.h> //for opaque types https://pybind11.readthedocs.io/en/stable/advanced/cast/stl.html

using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
#endif

#ifdef __clang__
#define CATCH_CONFIG_NO_CPP17_UNCAUGHT_EXCEPTIONS
#endif

#endif
