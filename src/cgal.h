#pragma once
//#define DEBUG

//filepath
//#ifdef _WIN32
//#include "direct.h"
//#define PATH_SEP '\\'
//#define GETCWD _getcwd
//#define CHDIR _chdir
//#else
//#include "unistd.h"
//#define PATH_SEP '/'
//#define GETCWD getcwd
//#define CHDIR chdir
//#endif
//
//std::string GetExecutableDirectory(const char* argv0) {
//    std::string path = argv0;
//    int path_directory_index = path.find_last_of(PATH_SEP);
//    return path.substr(0, path_directory_index + 1);
//}
//
//bool ChangeDirectory(const char* dir) { return CHDIR(dir) == 0; }
//
//std::string GetCurrentWorkingDirectory() {
//    const int BUFSIZE = 4096;
//    char buf[BUFSIZE];
//    memset(buf, 0, BUFSIZE);
//    GETCWD(buf, BUFSIZE - 1);
//    return buf;
//}

//Debugging
//#define DEBUG_MEASURE_TIME
//#define DEBUG_JOINERY_LIBRARY
//#define DEBUG_JOINERY_SOLVER_MAIN
#define DEBUG_JOINERY_SOLVER_ELEMENT
//#define DEBUG_JOINERY_SOLVER_MAIN_LOCAL_SEARCH
#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <boost/exception/diagnostic_information.hpp>
//#include <boost/filesystem/operations.hpp>
//#include <boost/filesystem/path.hpp>

//CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Intersections.h>
#include <CGAL/Bbox_2.h>
#include <CGAL/Bbox_3.h>
#include <CGAL/Plane_3.h>
#include <iostream>
#include <fstream>
#include <CGAL/Boolean_set_operations_2.h>
#include <list>

//meshing 2D
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

//closest polylines
#include <CGAL/box_intersection_d.h>
#include <CGAL/Iterator_range.h>
#include <CGAL/tuple.h>

#include <CGAL/boost/iterator/counting_iterator.hpp>
#include <boost/range/const_iterator.hpp>
#include <boost/range/value_type.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/graph/properties.hpp>

#include <cmath>
#include <string>
#include <chrono>

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
