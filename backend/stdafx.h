#pragma once

#include <stdio.h>
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

// clipper2
#include <clipper2/clipper.h>

// debug
#include "debug.h"

// glad and glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif

#include <GLFW/glfw3.h> // Will drag system OpenGL headers

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/euler_angles.hpp>

// imgui
// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
// #include "imgui_my_style.h"

// assimp
#include <stb_image.h>
#include <stb_image_write.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// pipe
#include "src/viewer/include/pipe/Pipe.h"

// my lib
#include "src/viewer/include/opengl/opengl_transform.h"
#include "src/viewer/include/opengl/opengl_globals.h"
#include "src/viewer/include/opengl/opengl_debug.h"

// imgui
// #include "imgui.h"
#include "wood_globals.h"
#include "imgui_impl_glfw.h"
#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#include "imgui_impl_opengl3.h"

// #include "imgui.h"
// #include "imgui_impl_opengl3.h"

// my lib goemetry
#include "src/viewer/include/opengl/opengl_shaders.h"

#include "src/viewer/include/opengl/opengl_polylines.h"
#include "src/viewer/include/opengl/opengl_pointclouds.h"
// #include <CDT.h>
//  temp
// #include <CDT.h>
// #include "src/viewer/include/opengl/opengl_triangulation.h"
#include <CDT.h>                                            //used in opengl_meshes
#include "src/viewer/include/opengl/opengl_triangulation.h" //used in opengl_meshes

#include "src/viewer/include/opengl/opengl_meshes.h"
#include "src/viewer/include/opengl/opengl_cameras.h"
#define gui
// #ifdef(gui) //only declare this if the following header is outside of the precompiled header
#include "src/viewer/include/imgui/imgui_render.h"
// #endif//only declare this if the following header is outside of the precompiled header
#include "src/viewer/include/opengl/opengl_globals_geometry.h"
#include "src/viewer/include/opengl/opengl_render.h"

// temp
#include "src/viewer/include/opengl/opengl_meshes.h"
#include "src/viewer/include/opengl/opengl_cameras.h"
#define gui
// #ifdef(gui) //only declare this if the following header is outside of the precompiled header
#include "src/viewer/include/imgui/imgui_render.h"
// #endif//only declare this if the following header is outside of the precompiled header
#include "src/viewer/include/opengl/opengl_globals_geometry.h"
#include "src/viewer/include/opengl/opengl_render.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// WOOD include
// BOOST
// CGAL
// EIGEN
// PYBIND11
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/exception/diagnostic_information.hpp>
// https://github.com/CGAL/cgal/discussions/6946
//  CGAL
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Exact_predicates_exact_constructions_kernel.h>
#include <CGAL/Intersections.h>
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

// BOOST
#include <boost/range/const_iterator.hpp>
#include <boost/range/value_type.hpp>
#include <boost/foreach.hpp>
#include <boost/iterator/function_output_iterator.hpp>
#include <boost/graph/properties.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

// RTREE
#include "src/wood/include/rtree.h"

using IK = CGAL::Exact_predicates_inexact_constructions_kernel;
using EK = CGAL::Exact_predicates_exact_constructions_kernel;
typedef CGAL::Cartesian_converter<IK, EK> IK_to_EK;
typedef CGAL::Cartesian_converter<EK, IK> EK_to_IK;
using CGAL_Polyline = std::vector<IK::Point_3>;
using CGAL_Polylines = std::list<CGAL_Polyline>;
typedef typename CGAL::Box_intersection_d::Box_with_info_d<double, 3, std::pair<std::size_t, std::size_t>> Box;
typedef CGAL::Surface_mesh<IK::Point_3> Mesh;
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

// Eigen for Pybind11
using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

// Wood Library Utilities

// Order Matters
// #include "cgal_print.h"
#include "cgal_box_search.h"
#include "cgal_inscribe_util.h"
#include "cgal_vector_util.h"
#include "cgal_intersection_util.h"
#include "cgal_xform_util.h"

// Order does not matter
#include "cgal_box_util.h"
// #include "cgal_data_set.h"
#include "cgal_math_util.h"
#include "cgal_polyline_mesh_util.h"
#include "cgal_plane_util.h"
#include "clipper_util.h"
#include "cgal_polyline_util.h"
#include "cgal_rectangle_util.h"
#include "rtree_util.h"

// Display
static std::vector<CGAL_Polyline> viewer_polylines;

// pybind11
#include <pybind11/pybind11.h>
