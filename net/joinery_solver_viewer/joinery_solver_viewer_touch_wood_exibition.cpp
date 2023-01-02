//
#include "stdafx.h"
//#include "easy3d_polyline_viewer.h"
//#include "xml_parser.h"
//#include "joinery_solver_main.h"
////
////int main(int argc, char** argv) {
////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    //Read Polylines from XML
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    bool simple_case = true;
////    std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
////    bool result = xml_parser::read_xml_polylines(input_polyline_pairs, simple_case);
////
////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    //Joinery Solver
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    //xml joint path
////    path_and_file_for_joints = "C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\joinery_library.xml ";
////
////    auto start = std::chrono::high_resolution_clock::now();
////    // unsync the I/O of C and C++.
////    std::ios::sync_with_stdio(false);
////
////    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
////    //std::vector<std::vector<int>> input_JOINTS_TYPES;
////    std::vector<std::vector<int>> input_JOINTS_TYPES;
////    input_JOINTS_TYPES.reserve(input_polyline_pairs.size());
////
////
////
////
////
////    //The adjacency will work because none of the joints can be found on boundaries
////
////
////
////
////    //if (simple_case)
////    //    input_adjacency.clear();
////
////
////
////
////    //Three valence
////    std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
////
////    //output
////    std::vector<std::vector<CGAL_Polyline>> output_polyline_pairs;
////    std::vector<std::vector<char>> output_types;
////    std::vector<std::vector<int>> top_face_triangulation;
////
////    //Global Parameters
////    std::vector<double> default_parameters_for_JOINTS_TYPES = {
////300,
////0.5,
////8,
////450,
////0.64,
////10,
////450,
////0.7,
////24,
////300,
////0.5,
////30,
////300,
////0.5,
////40,
////300,
////0.5,
////58,
////300,
////1.0,
////60
////    };
////
////    std::vector<double> scale = { 1,1,1 };
////    int search_type = 0;
////    int output_type = 3;
////
////        //Joint types are given to each element, no rtree search will be used here
////    for (int i = 0; i < input_polyline_pairs.size(); i+=2) {
////
////        int id = (int)(i*0.5);
////
////        auto input_JOINTS_TYPES_ = std::vector<int>();
////    
////        //Give first two faces empty joint type
////        input_JOINTS_TYPES_.reserve(input_polyline_pairs[i].size()+1);
////        input_JOINTS_TYPES_.emplace_back(-1);
////        input_JOINTS_TYPES_.emplace_back(-1);
////    
////       //Give the rest of the faces the faces joint type based on distance
////        for (int j = 0; j < input_polyline_pairs[i].size()-1; j++) {
////                    input_JOINTS_TYPES_.emplace_back(24);
////        }
////
////
////        
////        input_JOINTS_TYPES.emplace_back(input_JOINTS_TYPES_);       
////    }    
////    std::vector<int> input_adjacency;
////
////    //!!!
////    //This would work only if polyline are oriented correctly, because during the element creation in the solver the joint types property is reversed
////
////    input_adjacency = {
////   1,1,2 + 1,2 + 1 ,
////   1,1,2 + 3,2 + 3,
////   0,0,2 +3,2 + 3
////
////    };
////    
////    for (size_t i = 0; i < input_adjacency.size(); i += 4) {
////        input_JOINTS_TYPES[input_adjacency[i]][input_adjacency[i + 2]] = 60;
////    }
////
////
////    get_connection_zones(
////
////        //input
////        input_polyline_pairs,
////        input_insertion_vectors,
////        input_JOINTS_TYPES,
////        input_three_valence_element_indices_and_instruction,
////        input_adjacency,
////
////        //output
////        output_polyline_pairs,
////        output_types,
////        top_face_triangulation,
////
////        //Global Parameters
////        default_parameters_for_JOINTS_TYPES,
////        scale,
////        search_type,
////        output_type,
////        0
////    );
////
////
////    auto end = std::chrono::high_resolution_clock::now();
////    // Calculating total time taken by the program.
////    double time_taken = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
////    time_taken *= 1e-6;
////
////    std::cout << "\nTime taken by program is : " << std::fixed << time_taken << std::setprecision(9);
////    std::cout << " ms" << std::endl;
////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    //Write Polylines to XML
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    //output_polyline_pairs.emplace_back(viewer_polylines);
////    xml_parser::write_xml_polylines_and_types(output_polyline_pairs, output_types,-1,true);
////    //printf("\n Loops Starts");
////    //for (auto& types : output_types) {
////    //    printf("\n Iteration \n");
////    //    for (auto& type : types)
////    //        printf("%c \n", type);
////    //    break;
////    //}
////    //printf("\n");
////
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    //Preview poylylines from xml, take 9-th element
////    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////    auto viewer = viewer_init();
////    viewer_display_polylines(viewer, viewer_polylines, -1, 20);
////    viewer_display_polylines(viewer, input_polyline_pairs);
////    viewer_display_polylines_tree(viewer, output_polyline_pairs);
////    viewer_run(viewer);
////}
////
//
//int main(int argc, char** argv) {
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Read Polylines from XML
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    bool simple_case = false;
//    std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
//    bool result = xml_parser::read_xml_polylines(input_polyline_pairs, simple_case);
//
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Joinery Solver
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    //xml joint path
//    path_and_file_for_joints = "C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\joinery_library.xml ";
//
//    auto start = std::chrono::high_resolution_clock::now();
//    // unsync the I/O of C and C++.
//    std::ios::sync_with_stdio(false);
//
//    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
//    //std::vector<std::vector<int>> input_JOINTS_TYPES;
//    std::vector<std::vector<int>> input_JOINTS_TYPES;
//    input_JOINTS_TYPES.reserve(input_polyline_pairs.size());
//
//
//
//    //The adjacency will work because none of the joints can be found on boundaries
//    std::vector<int> input_adjacency;
//    input_adjacency = {
//       5,5,2 + 0,2 + 0 ,
//       11,11,2 + 0,2 + 0,
//       6,6,2 + 0,2 + 0,
//       13,13,2 + 3,2 + 3,
//       1,1,2 + 3,2 + 3,
//       0,0,2 + 2,2 + 2
//    };
//    if (simple_case)
//        input_adjacency.clear();
//
//
//    //Joint types are given to each element, no rtree search will be used here
//    for (int i = 0; i < input_polyline_pairs.size(); i += 2) {
//
//        int id = (int)(i * 0.5);
//
//        auto input_JOINTS_TYPES_ = std::vector<int>();
//
//        //Give first two faces empty joint type
//        input_JOINTS_TYPES_.reserve(input_polyline_pairs[i].size() + 1);
//        input_JOINTS_TYPES_.emplace_back(-1);
//        input_JOINTS_TYPES_.emplace_back(-1);
//
//        //Give the rest of the faces the faces joint type based on distance
//        for (int j = 0; j < input_polyline_pairs[i].size() - 1; j++) {
//
//            if (input_polyline_pairs[i].size() - 1 == 6) {
//
//                if (j == 0 || j == 3) {
//                    input_JOINTS_TYPES_.emplace_back(56);
//
//                }
//                else {
//                    input_JOINTS_TYPES_.emplace_back(58);
//                }
//
//            }
//            else {
//                if (j == 0 || j == 2) {
//
//                    input_JOINTS_TYPES_.emplace_back(56);
//                }
//                else {
//                    input_JOINTS_TYPES_.emplace_back(58);
//                }
//            }
//
//
//
//            //int type = CGAL::squared_distance(input_polyline_pairs[i+1][j], input_polyline_pairs[i+1][j + 1]) < 800 ? 56 : 58;
//           // input_JOINTS_TYPES_.emplace_back(type);
//        }
//
//
//
//        input_JOINTS_TYPES.emplace_back(input_JOINTS_TYPES_);
//    }
//
//    for (size_t i = 0; i < input_adjacency.size(); i += 4)
//        input_JOINTS_TYPES[input_adjacency[i]][input_adjacency[i + 2]] = 60;
//
//
//    //Three valence
//    std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
//
//    //output
//    std::vector<std::vector<CGAL_Polyline>> output_polyline_pairs;
//    std::vector<std::vector<char>> output_types;
//    std::vector<std::vector<int>> top_face_triangulation;
//
//    //Global Parameters
//    std::vector<double> default_parameters_for_JOINTS_TYPES = {
//300,
//0.5,
//8,
//450,
//0.64,
//10,
//450,
//0.7,
//22,
//300,
//0.5,
//30,
//300,
//0.5,
//40,
//300,
//0.5,
//58,
//300,
//1.0,
//60
//    };
//
//    std::vector<double> scale = { 1,1,2.5 };
//    int search_type = 0;
//    int output_type = 3;
//
//
//    get_connection_zones(
//
//        //input
//        input_polyline_pairs,
//        input_insertion_vectors,
//        input_JOINTS_TYPES,
//        input_three_valence_element_indices_and_instruction,
//        input_adjacency,
//
//        //output
//        output_polyline_pairs,
//        output_types,
//        top_face_triangulation,
//
//        //Global Parameters
//        default_parameters_for_JOINTS_TYPES,
//        scale,
//        search_type,
//        output_type,
//        0
//    );
//
//
//    auto end = std::chrono::high_resolution_clock::now();
//    // Calculating total time taken by the program.
//    double time_taken = (double)std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
//    time_taken *= 1e-6;
//
//    std::cout << "\nTime taken by program is : " << std::fixed << time_taken << std::setprecision(9);
//    std::cout << " ms" << std::endl;
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Write Polylines to XML
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //output_polyline_pairs.emplace_back(viewer_polylines);
//    xml_parser::write_xml_polylines_and_types(output_polyline_pairs, output_types);
//    //printf("\n Loops Starts");
//    //for (auto& types : output_types) {
//    //    printf("\n Iteration \n");
//    //    for (auto& type : types)
//    //        printf("%c \n", type);
//    //    break;
//    //}
//    //printf("\n");
//
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Preview poylylines from xml, take 9-th element
//    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    auto viewer = viewer_init();
//    viewer_display_polylines(viewer, viewer_polylines, -1, 20);
//    viewer_display_polylines(viewer, input_polyline_pairs, 6);
//    viewer_display_polylines_tree(viewer, output_polyline_pairs, 6);
//    viewer_run(viewer);
//}
