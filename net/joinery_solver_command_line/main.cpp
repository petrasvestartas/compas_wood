// joinery_solver_command_line.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#pragma once
#define DEBUG

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
static std::string path_and_file_for_joints = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\src\\compas_wood\\joinery_library.xml";

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

//XML Reader
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <array>

bool exists_test3(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

bool read_xml(int type = 0) {
	std::string name = "";//custom

	switch (type) {
		case(12):
		case(0):
			name = "ss_e_ip_9";
		case(11):
		case(1):
			name = "ss_e_op_9";
			break;
		case(20):
		case(2):
			name = "ts_e_p_9";
			break;
		case(30):
		case(3):
			name = "cr_c_ip_9";
			break;
		case(40):
		case(4):
			name = "tt_e_p_9";
			break;
		case(50):
		case(5):
			name = "ss_e_r_9";
			break;
	}

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Check if XML file exists, path_and_file_for_joints is a global path
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG
	printf("\nCPP File path %s ", path_and_file_for_joints.c_str());
#endif
	if (!exists_test3(path_and_file_for_joints)) {
#ifdef DEBUG
		printf("\nCPP File does not exist %s ", path_and_file_for_joints.c_str());
#endif
		return false;
	}
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Read XML
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	boost::property_tree::ptree tree;
	boost::property_tree::xml_parser::read_xml(path_and_file_for_joints, tree);

#ifdef DEBUG
	printf("\nCPP tree size %zi ", tree.size());
#endif

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Get Property to find the right joint parameters and keys of XML file properties
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Traverse property tree example
	std::string xml_joint_name = "custom_joints." + name;
	std::array<std::string, 6> keys = { "m0", "m1","f0","f1", "m_boolean_type","f_boolean_type" };
	std::array<int, 6> array_id = { 0,1,0,1,0,1 };

	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Get properties from XML and add to joint
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	try {
		for (boost::property_tree::ptree::value_type& v : tree.get_child(xml_joint_name)) {
			for (int i = 0; i < 4; i++) {
				if (v.first == keys[i]) {
					std::cout << v.first << "\n";

					CGAL_Polyline polyline;
					for (boost::property_tree::ptree::value_type& point : v.second) {
						double x = point.second.get<double>("x");
						double y = point.second.get<double>("y");
						double z = point.second.get<double>("z");
						polyline.emplace_back(x, y, z);

#ifdef DEBUG
						printf("\n CPP x %f y %f z %f ", x, y, z);
#endif
					}

					//Assign to array
					switch (i) {
						case(0):
							//joint.m[0].emplace_back(polyline);
							break;

						case(1):
							//joint.m[1].emplace_back(polyline);
							break;

						case(2):
							//joint.f[0].emplace_back(polyline);
							break;

						case(3):
							//jointf[1].emplace_back(polyline);
							break;
					}
				}
			}

			for (int i = 4; i < 6; i++) {
				std::vector<char> boolean_type;
				if (v.first == keys[i]) {
					std::cout << v.first << "\n";

					//CGAL_Polyline polyline;

					for (boost::property_tree::ptree::value_type& index : v.second) {
						char id = index.second.get_value<char>();
						std::cout << "id" << id << "\n";
#ifdef DEBUG
						printf("\n CPP id %c ", id);
#endif
						if (i == 5) {
							//emplace to female joint.m_boolean_type
						} else {
							//emplace to female joint.f_boolean_type
						}
					}

					//Assign to array
				}
			}
		}
	} catch (std::exception& e) {
#ifdef DEBUG
		printf("\nCPP Wrong property ");
#endif
		return false;
	}
	return true;
}

int main() {
	read_xml(2);
	std::cout << "\n" << "End" << "\n";
}