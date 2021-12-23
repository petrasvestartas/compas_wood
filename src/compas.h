#pragma once
//#include <pybind11/eigen.h>
//#include <pybind11/stl.h>

#include <Eigen/StdVector>
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





//#include "connection_zones.h"

using IK = CGAL::Exact_predicates_inexact_constructions_kernel;
using EK = CGAL::Exact_predicates_exact_constructions_kernel;
typedef CGAL::Cartesian_converter<IK, EK> IK_to_EK;
typedef CGAL::Cartesian_converter<EK, IK> EK_to_IK;
using CGAL_Polyline = std::vector<IK::Point_3>;
using CGAL_Polylines = std::list<CGAL_Polyline>;


static  double GlobalTolerance = 0.01;
static  double GlobalToleranceSquare = 0.0001;
static  double GlobalClipperScale = 1000000.0;
static  double GlobalClipperAreaTolerance = 0.0001;
static  double GlobalExtend[5] = { 0.0,0.0,0,0,0 };

#define ON_IS_FINITE(x) (0x7FF0 != (*((unsigned short*)(&x) + 3) & 0x7FF0))
#define ON_DBL_MIN 2.22507385850720200e-308
#define ON_EPSILON 2.2204460492503131e-16
#define ON_SQRT_EPSILON 1.490116119385000000e-8
#define ON_ZERO_TOLERANCE 2.3283064365386962890625e-10
#define ON_DBL_MAX 1.7976931348623158e+308

//meshing 2D
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_face_base_with_info_2.h>
#include <CGAL/Polygon_2.h>

struct FaceInfo2 {
	FaceInfo2() {}
	int nesting_level;
	bool in_domain() {
		return nesting_level % 2 == 1;
	}
};

typedef CGAL::Triangulation_vertex_base_2<IK>                      Vb;
typedef CGAL::Triangulation_face_base_with_info_2<FaceInfo2, IK>    Fbb;
typedef CGAL::Constrained_triangulation_face_base_2<IK, Fbb>        Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb>               TDS;
typedef CGAL::Exact_predicates_tag                                Itag;
typedef CGAL::Constrained_Delaunay_triangulation_2<IK, TDS, Itag>  CDT;
typedef CDT::Point                                                Point;
typedef CGAL::Polygon_2<IK>                                        Polygon_2;
typedef CDT::Face_handle                                          Face_handle;



namespace compas {

	using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
	using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

	struct Result {
		RowMatrixXd vertices;
		RowMatrixXi faces;
	};

	//CGAL_Mesh mesh_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

	//std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> mesh_to_vertices_and_faces(const CGAL_Mesh& mesh);

	std::vector<compas::RowMatrixXd> result_from_polylines(CGAL_Polylines polylines);



	//Result result_from_mesh(const CGAL_Mesh& mesh);



	//New Methods
	std::vector<compas::RowMatrixXd> result_from_vector(IK::Vector_3* v);

	std::vector<compas::RowMatrixXd> result_from_bbox(std::vector<CGAL::Bbox_3> boxes);

	std::vector<CGAL_Polyline> polylines_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

	void polylines_from_vertices_and_faces_and_properties(
		const compas::RowMatrixXd& polylines_vertices_XYZ,
		const compas::RowMatrixXi& polylines_vertices_count_int,
		const compas::RowMatrixXd& face_vectors_XYZ,
		const compas::RowMatrixXi& face_joints_types_int,
		const compas::RowMatrixXi& three_valence_element_indices_and_instruction,
		const compas::RowMatrixXd& default_parameters_for_joint_types_matrix,

		std::vector<CGAL_Polyline>& out_polyline_pairs,
		std::vector<std::vector<IK::Vector_3>>& out_insertion_vectors,
		std::vector<std::vector<int>>& out_joint_types,
		std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
		std::vector<double>& out_default_parameters_for_joint_types
	);


	std::vector<compas::RowMatrixXd> result_from_polylinesVectorVector(std::vector < std::vector < CGAL_Polyline>> polylines);

	std::vector<compas::RowMatrixXd> result_from_polylinesVector(std::vector < CGAL_Polyline>& polylines, bool debug);
	std::vector<compas::RowMatrixXd> result_from_polylinesVector(std::vector < std::vector < CGAL_Polyline>>& polylines, bool debug);
}


inline std::vector<compas::RowMatrixXd> compas::result_from_polylines(CGAL_Polylines polylines) {

	std::vector<compas::RowMatrixXd> pointsets;

	for (auto i = polylines.begin(); i != polylines.end(); i++) {

		const CGAL_Polyline& poly = *i;
		int n = poly.size();
		compas::RowMatrixXd points(n, 3);

		for (int j = 0; j < n; j++) {
			points(j, 0) = (double)poly[j].x();
			points(j, 1) = (double)poly[j].y();
			points(j, 2) = (double)poly[j].z();
		}

		pointsets.push_back(points);
	}

	return pointsets;
}


inline std::vector<compas::RowMatrixXd> compas::result_from_vector(IK::Vector_3* v) {
	std::vector<compas::RowMatrixXd> pointsets;

	//for (auto i = 0; i < AABBs.size(); i++) {


	int n = 2;
	compas::RowMatrixXd points(4, 3);

	points(0, 0) = (double)v[0].x();
	points(0, 1) = (double)v[0].y();
	points(0, 2) = (double)v[0].z();
	points(1, 0) = (double)v[1].x();
	points(1, 1) = (double)v[1].y();
	points(1, 2) = (double)v[1].z();
	points(2, 0) = (double)v[2].x();
	points(2, 1) = (double)v[2].y();
	points(2, 2) = (double)v[2].z();
	points(3, 0) = (double)v[3].x();
	points(3, 1) = (double)v[3].y();
	points(3, 2) = (double)v[3].z();

	pointsets.push_back(points);
	// }
	return pointsets;
}


inline std::vector<compas::RowMatrixXd> compas::result_from_bbox(std::vector<CGAL::Bbox_3> AABBs) {
	std::vector<compas::RowMatrixXd> pointsets;

	for (auto i = 0; i < AABBs.size(); i++) {


		int n = 2;
		compas::RowMatrixXd points(n, 3);

		points(0, 0) = (double)AABBs[i].xmin();
		points(0, 1) = (double)AABBs[i].ymin();
		points(0, 2) = (double)AABBs[i].zmin();
		points(1, 0) = (double)AABBs[i].xmax();
		points(1, 1) = (double)AABBs[i].ymax();
		points(1, 2) = (double)AABBs[i].zmax();

		pointsets.push_back(points);
	}
	return pointsets;
}

inline std::vector<CGAL_Polyline> compas::polylines_from_vertices_and_faces(const compas::RowMatrixXd& V, const compas::RowMatrixXi& F) {
	//////////////////////////////////////////////////////////////////////////////
	//Convert Raw data to list of Polyline
	//////////////////////////////////////////////////////////////////////////////
	std::vector<CGAL_Polyline> polylinePairs(F.size());
	CGAL_Polyline pline;

	int counter = 0;
	int lastCount = F(counter, 0);
	for (int i = 0; i < V.size() / 3; i++) {

		CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
		pline.push_back(p);

		if (pline.size() == lastCount) {
			polylinePairs[counter] = pline;
			pline.clear();//Clear points from the polyline
			lastCount = F(++counter, 0); //Take next polyline Count
		}

	}
	return polylinePairs;
}

inline void compas::polylines_from_vertices_and_faces_and_properties(
	const compas::RowMatrixXd& polylines_vertices_XYZ,
	const compas::RowMatrixXi& polylines_vertices_count_int,
	const compas::RowMatrixXd& face_vectors_XYZ,
	const compas::RowMatrixXi& face_joints_types_int,
	const compas::RowMatrixXi& three_valence_element_indices_and_instruction,
	const compas::RowMatrixXd& default_parameters_for_joint_types_matrix,

	std::vector<CGAL_Polyline>& out_polyline_pairs,
	std::vector<std::vector<IK::Vector_3>>& out_insertion_vectors,
	std::vector<std::vector<int>>& out_joint_types,
	std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
	std::vector<double>& out_default_parameters_for_joint_types


) {

	//////////////////////////////////////////////////////////////////////////////
	//Convert Raw data to list of Polyline
	//////////////////////////////////////////////////////////////////////////////
	//std::vector<CGAL_Polyline> out_polyline_pairs; 
	out_polyline_pairs.reserve(polylines_vertices_count_int.size());

	//std::vector< std::vector<IK::Vector_3>> insertion_vectors;
	out_insertion_vectors.reserve(polylines_vertices_count_int.size());

	// std::vector< std::vector<int>> joint_types;
	out_joint_types.reserve(polylines_vertices_count_int.size());





	CGAL_Polyline pline;
	int counter = 0;
	int lastCount = polylines_vertices_count_int(counter, 0);
	for (int i = 0; i < polylines_vertices_XYZ.size() / 3; i++) {

		CGAL::Epick::Point_3 p(polylines_vertices_XYZ(i, 0), polylines_vertices_XYZ(i, 1), polylines_vertices_XYZ(i, 2));
		pline.push_back(p);

		if (pline.size() == lastCount) {
			out_polyline_pairs.push_back(pline);
			pline.clear();//Clear points from the polyline
			lastCount = polylines_vertices_count_int(++counter, 0); //Take next polyline Count
		}
	}

	if (face_vectors_XYZ.size() > 0) {
		//printf("999");
		std::vector<IK::Vector_3> vectors;
		counter = 0;
		int lastCount = polylines_vertices_count_int(counter, 0) + 1;
		for (int i = 0; i < face_vectors_XYZ.size() / 3; i++) {

			CGAL::Epick::Vector_3 v(face_vectors_XYZ(i, 0), face_vectors_XYZ(i, 1), face_vectors_XYZ(i, 2));
			vectors.push_back(v);

			if (vectors.size() == (lastCount)) {
				out_insertion_vectors.push_back(vectors);
				vectors.clear();//Clear points from the polyline
				lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
			}

		}

	}

	if (face_joints_types_int.size() > 0) {
		//printf("888");
		std::vector<int> types;
		counter = 0;
		int lastCount = polylines_vertices_count_int(counter, 0) + 1;
		for (int i = 0; i < face_joints_types_int.size(); i++) {

			int id(face_joints_types_int(i, 0));
			types.push_back(id);

			if (types.size() == (lastCount)) {
				out_joint_types.push_back(types);
				types.clear();//Clear points from the polyline
				lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
			}

		}

	}

	if (three_valence_element_indices_and_instruction.size() > 0) {
		out_three_valence_element_indices_and_instruction.reserve(three_valence_element_indices_and_instruction.size());



		for (int i = 0; i < three_valence_element_indices_and_instruction.size(); i += 4) {

			std::vector<int> ids
			{
			three_valence_element_indices_and_instruction(i + 0, 0),
			three_valence_element_indices_and_instruction(i + 1, 0),
			three_valence_element_indices_and_instruction(i + 2, 0),
			three_valence_element_indices_and_instruction(i + 3, 0),

			};

			out_three_valence_element_indices_and_instruction.push_back(ids);

		}
	}

	if (default_parameters_for_joint_types_matrix.size() > 0) {
		out_default_parameters_for_joint_types.reserve(default_parameters_for_joint_types_matrix.size());


		for (int i = 0; i < default_parameters_for_joint_types_matrix.size(); i ++) {
			out_default_parameters_for_joint_types.push_back(default_parameters_for_joint_types_matrix(i, 0));

		}

	}






}


inline std::vector<compas::RowMatrixXd> compas::result_from_polylinesVectorVector(std::vector < std::vector < CGAL_Polyline>> polylines) {

	std::vector<compas::RowMatrixXd> pointsets;

	for (auto i = polylines.begin(); i != polylines.end(); i++) {

		for (auto j = i->begin(); j != i->end(); j++) {

			const CGAL_Polyline& poly = *j;
			int n = poly.size();
			compas::RowMatrixXd points(n, 3);

			for (int k = 0; k < n; k++) {
				points(k, 0) = (double)poly[k].x();
				points(k, 1) = (double)poly[k].y();
				points(k, 2) = (double)poly[k].z();
			}

			pointsets.push_back(points);
		}
	}

	//printf("CPP number of points: %i ", pointsets.size());

	return pointsets;
}


inline std::vector<compas::RowMatrixXd> compas::result_from_polylinesVector(std::vector < CGAL_Polyline>& polylines, bool debug) {

	std::vector<compas::RowMatrixXd> pointsets;

	for (auto i = polylines.begin(); i != polylines.end(); i++) {



		const CGAL_Polyline& poly = *i;
		int n = poly.size();
		compas::RowMatrixXd points(n, 3);

		for (int k = 0; k < n; k++) {
			points(k, 0) = (double)poly[k].hx();
			points(k, 1) = (double)poly[k].hy();
			points(k, 2) = (double)poly[k].hz();


		}

		pointsets.push_back(points);

	}

	//  printf("CPP number of points: %i ", pointsets.size());

	if (debug) {
		std::ofstream myfile;
		myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");


		for (auto i = polylines.begin(); i != polylines.end(); i++) {

			const CGAL_Polyline& poly = *i;
			int n = poly.size();
			compas::RowMatrixXd points(n, 3);

			for (int k = 0; k < n; k++) {
				points(k, 0) = (double)poly[k].hx();
				points(k, 1) = (double)poly[k].hy();
				points(k, 2) = (double)poly[k].hz();
			}


			for (int k = 0; k < n; k++)
				myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " " << "\n";
			myfile << ("\n");
		}
		myfile.close();
	}

	return pointsets;
}

inline std::vector<compas::RowMatrixXd> compas::result_from_polylinesVector(std::vector < std::vector < CGAL_Polyline>>& polylines_vector, bool debug) {

	std::vector<compas::RowMatrixXd> pointsets;

	int kk = 0;
	for (auto& polylines : polylines_vector) {

		kk++;

		//if (kk % 2 == 1) {
		//	continue;
		//}

		for (auto i = polylines.begin(); i != polylines.end(); i++) {



			const CGAL_Polyline& poly = *i;
			int n = poly.size();
			compas::RowMatrixXd points(n, 3);

			for (int k = 0; k < n; k++) {
				points(k, 0) = (double)poly[k].hx();
				points(k, 1) = (double)poly[k].hy();
				points(k, 2) = (double)poly[k].hz();


			}

			pointsets.push_back(points);

		}
	}
	//  printf("CPP number of points: %i ", pointsets.size());

	if (debug) {
		std::ofstream myfile;
		myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");
		for (auto& polylines : polylines_vector) {

			for (auto i = polylines.begin(); i != polylines.end(); i++) {

				const CGAL_Polyline& poly = *i;
				int n = poly.size();
				compas::RowMatrixXd points(n, 3);

				for (int k = 0; k < n; k++) {
					points(k, 0) = (double)poly[k].hx();
					points(k, 1) = (double)poly[k].hy();
					points(k, 2) = (double)poly[k].hz();
				}


				for (int k = 0; k < n; k++)
					myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " " << "\n";
				myfile << ("\n");
			}
		
		}
		myfile.close();
	}

	return pointsets;
}


//void init_connectionzones(pybind11::module&);
//
//
//void say_hello() {
//    printf("CPP SHello World \n");
//}
//
//
////pybind11module, module
//PYBIND11_MODULE(pybind11module, m) {
//
//    m.doc() = "";
//    m.def("say_hello", &say_hello);//Function reference
//
//    pybind11::class_<compas::Result>(m, "Result")
//        .def_readonly("vertices", &compas::Result::vertices)
//        .def_readonly("faces", &compas::Result::faces);
//
//    init_connectionzones(m);
//}