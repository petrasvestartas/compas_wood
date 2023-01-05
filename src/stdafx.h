#pragma once

#define easy3d_precompiled
//#define DEBUG_MEASURE_TIME
//#define DEBUG_JOINERY_LIBRARY
//#define DEBUG_JOINERY_SOLVER_MAIN
//#define DEBUG_JOINERY_SOLVER_ELEMENT
//#define DEBUG_JOINERY_SOLVER_MAIN_LOCAL_SEARCH

#define NOMINMAX
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif

//Timer
#include <chrono>

//STD and Other
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <iostream>
#include <ostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <assert.h>
//#include <execution>
#include <algorithm>
#include <thread>
#include <functional>
#include <math.h>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <cstdlib>
#include <cstdio>

//Data structures
#include <float.h>
#include <inttypes.h>
#include <string>
#include <cstring>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <list>

#include <boost/exception/diagnostic_information.hpp>

//CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Intersections.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Plane_3.h>
#include <CGAL/Boolean_set_operations_2.h>

//CGAL meshing 2D
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

//CGAL closest polylines
#include <CGAL/box_intersection_d.h>
#include <CGAL/Iterator_range.h>
#include <CGAL/tuple.h>

#include <CGAL/boost/iterator/counting_iterator.hpp>

//BOOST
#include <boost/range/const_iterator.hpp>
#include <boost/range/value_type.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/graph/properties.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

//CLIPPER
#include "clipper.h"

//RTREE
#include "rtree.h"

using IK = CGAL::Exact_predicates_inexact_constructions_kernel;
using EK = CGAL::Exact_predicates_exact_constructions_kernel;
typedef CGAL::Cartesian_converter<IK, EK> IK_to_EK;
typedef CGAL::Cartesian_converter<EK, IK> EK_to_IK;
using CGAL_Polyline = std::vector<IK::Point_3>;
using CGAL_Polylines = std::list<CGAL_Polyline>;
typedef typename CGAL::Box_intersection_d::Box_with_info_d<double, 3, std::pair<std::size_t, std::size_t> > Box;

static double GlobalTolerance = 0.01;
static double GlobalToleranceSquare = 0.0001;
static double GlobalClipperScale = 1000000.0;
static double GlobalClipperAreaTolerance = 0.0001;
static double GlobalExtend[5] = { 0.0, 0.0, 0, 0, 0 };
static std::string path_and_file_for_joints = "";

#define ON_IS_FINITE(x) (0x7FF0 != (*((unsigned short *)(&x) + 3) & 0x7FF0))
#define ON_DBL_MIN 2.22507385850720200e-308
#define ON_EPSILON 2.2204460492503131e-16
#define ON_SQRT_EPSILON 1.490116119385000000e-8
#define ON_ZERO_TOLERANCE 2.3283064365386962890625e-10
#define ON_DBL_MAX 1.7976931348623158e+308

//Display
static std::vector<CGAL_Polyline> viewer_polylines;

struct FaceInfo2 {
    FaceInfo2() {}
    int nesting_level;
    bool in_domain() {
        return nesting_level % 2 == 1;
    }
};

typedef CGAL::Triangulation_vertex_base_2<IK> Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, IK> Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<IK, Fbb> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> TDS;
typedef CGAL::Exact_predicates_tag Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<IK, TDS, Itag> CDT;
typedef CDT::Point Point;
typedef CGAL::Polygon_2<IK> Polygon_2;
typedef CDT::Face_handle Face_handle;

using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

//EASY3D
#ifdef easy3d_precompiled
#include <easy3d/viewer/viewer.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/renderer/drawable_lines.h>
#include <easy3d/renderer/renderer.h>
#include <easy3d/fileio/resources.h>
#include <easy3d/util/logging.h>
#endif // easy3d_precompiled

//MY LIBRARY UTILITIES

//Order Matters
#include "cgal_print.h"
#include "cgal_box_search.h"
#include "cgal_vector_util.h"
#include "cgal_intersection_util.h"
#include "cgal_xform_util.h"

//Order does not matter
#include "cgal_box_util.h"
#include "cgal_data_set.h"

#include "cgal_math_util.h"
#include "cgal_mesh_util.h"
#include "cgal_plane_util.h"
#include "cgal_polyline_util.h"

//
#include "clipper_util.h"
#include "rtree_util.h"
