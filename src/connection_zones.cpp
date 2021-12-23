#include "stdafx.h"
#include "connection_zones.h"

void get_connection_zones_test() {
	//fsdfsdfsd
}

void get_connection_zones(
	std::vector<CGAL_Polyline>& input_polyline_pairs,
	std::vector<std::vector<IK::Vector_3>>& input_insertion_vectors,
	std::vector<std::vector<int>>& input_joint_types,
	std::vector<std::vector<int>>& input_three_valence_element_indices_and_instruction,

	//output
	std::vector<std::vector<CGAL_Polyline>>& plines,
	std::vector<std::vector<int>>& top_face_triangulation,

	std::vector<double>& default_parameters_for_joint_types,
	int search_type,
	double division_distance,
	double shift,
	int output_type,
	int triangulate

) {


	const int n = input_polyline_pairs.size() * 0.5;

	//////////////////////////////////////////////////////////////////////////////
	//Create elements, AABB, OBB, P, Pls, thickness
	//////////////////////////////////////////////////////////////////////////////
	std::vector<element> elements;
	get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);

	//IK::Point_3 p0(0, 0, 0);
	//IK::Point_3 p1(0, -94.581, 0);

	//std::ofstream myfile;
	//myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output4.txt");
	//for (int i = 0; i < n; i++) {
	//	int e = 0;

	///*	for(auto& p : elements[i].polylines[0])
	//		myfile << p.hx() << " " << p.hy() << " " << p.hz() << "\n";*/

	//	IK::Point_3 cp0;
	//	double dist0 = CGAL_PolylineUtil::closest_distance_and_point(p0, elements[i].polylines[0], e, cp0);
	//	if (dist0 < GlobalToleranceSquare)
	//		myfile << " Element " << i << " Edge " << e << " " << dist0 << " " << cp0.hx() << " " << cp0.hy() << " " << cp0.hz() << "\n";


	//	//for (auto& p : elements[i].polylines[1])
	//	//	myfile << p.hx() << " " << p.hy() << " " << p.hz() << "\n";

	//	e = 0;
	//	IK::Point_3 cp1;
	//	double dist1 = CGAL_PolylineUtil::closest_distance_and_point(p0, elements[i].polylines[1], e, cp1);
	//	if (dist1 < GlobalToleranceSquare)
	//		myfile << i << " Element " << e << " Edge " << dist1 << " " << cp1.hx() << " " << cp1.hy() << " " << cp1.hz() << "\n";

	//	myfile << "\n" << "_______" << "\n";
	//}
	//myfile << "\n" << "_______________________________________________________________" << "\n";
	//for (int i = 0; i < n; i++) {
	//	int e = 0;
	//	double dist = CGAL_PolylineUtil::closest_distance(p1, elements[i].polylines[0], e);
	//	if (dist < GlobalToleranceSquare)
	//		myfile << i << " " << e << " " << dist << "\n";

	//	e = 0;
	//	dist = CGAL_PolylineUtil::closest_distance(p1, elements[i].polylines[1], e);
	//	if (dist < GlobalToleranceSquare)
	//		myfile << i << " " << e << " " << dist << "\n";

	//	myfile << "\n" << "_______" << "\n";
	//}
	//myfile.close();
	//////////////////////////////////////////////////////////////////////////////
	//Create joints, Perform Joint Area Search
	//////////////////////////////////////////////////////////////////////////////
	auto joints = std::vector<joint>();
	auto joints_map = std::unordered_map<uint64_t, int>();
	rtree_search(elements, search_type, joints, joints_map);
	//return;
	//if (true) {
	//	std::ofstream myfile;
	//	myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");
	//	myfile << (elements.size()) << "\n";
	//	myfile << (joints.size()) << "\n";
	//	//myfile << (input_three_valence_element_indices_and_instruction.size()) << "\n";
	//	myfile.close();
	//}
	//return;

	//////////////////////////////////////////////////////////////////////////////
	//3-valence joints
	//////////////////////////////////////////////////////////////////////////////
	if (input_three_valence_element_indices_and_instruction.size() > 0)
		three_valence_joint_alignment(input_three_valence_element_indices_and_instruction, elements, joints, joints_map, division_distance);//plines, 
	
	////////////////////////////////////////////////////////////////////////////////
	////Create and Align Joints 1. Iterate type 2. Select joint based on not/given user joint_type
	////////////////////////////////////////////////////////////////////////////////
	//std::vector<double> default_parameters_for_four_types = default_parameters_for_four_types_;
	joint_library::construct_joint_by_index(elements, joints, division_distance, shift, default_parameters_for_joint_types);
	//std::ofstream myfile;
	//myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output3.txt");
	//myfile << joints.size();
	//for (int i = 0; i < default_parameters_for_joint_types.size(); i++) {
	//	for (int j = 0; j < default_parameters_for_joint_types[i].size(); j++) {

	//	}
	//}
	//myfile.close();
	//////////////////////////////////////////////////////////////////////////////
	//Iterate joint address
	//////////////////////////////////////////////////////////////////////////////

	plines = std::vector<std::vector<CGAL_Polyline>>(elements.size());
	//CGAL_Debug(joints.size());
	for (int i = 0; i < elements.size(); i++) {//takes 30-50 ms just to copy past polyline geometry

		switch (output_type) {
			case(0):
				elements[i].get_joints_geometry(joints, plines, 0);
				break;
			case(1):
				elements[i].get_joints_geometry(joints, plines, 1);
				break;
			case(2):
				elements[i].get_joints_geometry(joints, plines, 2);
				break;
			default:
			case(3):
				elements[i].get_joints_geometry(joints, plines, 3);
				break;
			case(4):
				//elements[i].get_joints_geometry_as_closed_polylines_replacing_edges(joints, plines);
				elements[i].get_joints_geometry_as_closed_polylines_performing_intersection(joints, plines);
				break;
		}

	}

	//////////////////////////////////////////////////////////////////////////////
	//Create Mesh Triangulation for top face
	//////////////////////////////////////////////////////////////////////////////
	if (triangulate) {
		top_face_triangulation = std::vector<std::vector<int>>(elements.size());
		for (int i = 0; i < elements.size(); i++) {
			CGAL_MeshUtil::mesh_from_polylines(plines[i], elements[i].planes[0], top_face_triangulation[i]);

		}
	}

	//if (false) {
	//	for (int i = 1; i < elements.size(); i++) {//Pls.size()
	//		for (int j = 0; j < elements[i].planes.size(); j++) {//
	//			auto center = CGAL_PolylineUtil::Center(elements[i].polylines[j]);

	//			if (elements[i].edge_vectors.size() > 0)
	//				plines.push_back({ center,center + elements[i].edge_vectors[j] });
	//			// auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(Pls[i][j].point(), Pls[i][j], 10, 10, 10);
	//			//auto planeDisplay = CGAL_PlaneUtil::PlaneToLine(CGAL_PolylineUtil::Center(elements[i].polylines[j]), elements[i].planes[j], 10, 10, 10);
	//			//plines.push_back(planeDisplay);
	//		}
	//	}
	//}



}

std::vector<compas::RowMatrixXd> get_connection_zones_compas(
	Eigen::Ref<const compas::RowMatrixXd>& polylines_vertices_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& polylines_vertices_count_int,
	Eigen::Ref<const compas::RowMatrixXd>& face_vectors_XYZ,
	Eigen::Ref<const compas::RowMatrixXi>& face_joints_types_int,
	Eigen::Ref<const compas::RowMatrixXi>& three_valence_element_indices_and_instruction,

	//output
	Eigen::Ref<const compas::RowMatrixXd>& default_parameters_for_joint_types_matrix,
	//Eigen::Ref<const compas::RowMatrixXi>& top_face_triangulation_matrix,

	int search_type ,
	double division_distance ,
	double shift,
	int output_type ,
	int triangulate 
	
) {





	//////////////////////////////////////////////////////////////////////////////
	//Convert Raw data to list of Polyline
	//////////////////////////////////////////////////////////////////////////////
	//std::vector<element> elements;
	//std::vector<CGAL_Polyline> polyline_pairs = compas::polylines_from_vertices_and_faces(polylines_vertices_XYZ, polylines_vertices_count_int);
	std::vector<CGAL_Polyline> out_polyline_pairs;
	std::vector<std::vector<IK::Vector_3>> out_insertion_vectors;
	std::vector<std::vector<int>> out_joint_types;
	std::vector<std::vector<int>> out_three_valence_element_indices_and_instruction;
	std::vector<double> out_default_parameters_for_joint_types;


	compas::polylines_from_vertices_and_faces_and_properties(

		polylines_vertices_XYZ,
		polylines_vertices_count_int,
		face_vectors_XYZ,
		face_joints_types_int,
		three_valence_element_indices_and_instruction,
		default_parameters_for_joint_types_matrix,

		out_polyline_pairs,
		out_insertion_vectors,
		out_joint_types,
		out_three_valence_element_indices_and_instruction,
		out_default_parameters_for_joint_types
	);
	
	std::vector<std::vector<CGAL_Polyline>> output;
	std::vector<std::vector<int>> top_face_triangulation;
	get_connection_zones(
		out_polyline_pairs,
		out_insertion_vectors,
		out_joint_types,
		out_three_valence_element_indices_and_instruction,

		output,
		top_face_triangulation,

		out_default_parameters_for_joint_types,
		search_type,
		division_distance,
		shift,
		output_type,
		triangulate
		
	);

	//////////////////////////////////////////////////////////////////////////////
//Convert Output to Raw Data
//////////////////////////////////////////////////////////////////////////////
	return compas::result_from_polylinesVector(output, true);
}



//
//void init_connectionzones(pybind11::module& m) {
//    pybind11::module submodule = m.def_submodule("connectionzonesCGAL");
//
//    submodule.def(
//        "get_connection_zones",
//        &get_connection_zones,
//        pybind11::arg("V").noconvert(),
//        pybind11::arg("F").noconvert(),
//        pybind11::arg("D").noconvert(),
//        pybind11::arg("J").noconvert(),
//        pybind11::arg("X").noconvert()
//    );
//}
