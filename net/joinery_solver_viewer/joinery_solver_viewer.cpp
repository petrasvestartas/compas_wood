
#include "stdafx.h"
#include "easy3d_polyline_viewer.h"
#include "xml_parser.h"
#include "joinery_solver_main.h"


int main(int argc, char** argv  ) {
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Read Polylines from XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    bool simple_case = true;
    std::vector<std::vector<IK::Point_3>> input_polyline_pairs  ;
    bool result = xml_parser::read_xml_polylines(input_polyline_pairs, simple_case)  ;
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Joinery Solver
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //xml joint path
    path_and_file_for_joints = "C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\joinery_library.xml ";

    auto start = std::chrono::high_resolution_clock::now();
    // unsync the I/O of C and C++.
    std::ios::sync_with_stdio(false);

    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
    //std::vector<std::vector<int>> input_joint_types;
    std::vector<std::vector<int>> input_joint_types;
    input_joint_types.reserve(input_polyline_pairs.size()) ;



    //The adjacency will work because none of the joints can be found on boundaries
    std::vector<int> input_adjacency = {
        0,0,2 + 0,2 + 0 ,
        1,1,2 + 0,2 + 0,
        2,2,2 + 0,2 + 0,
        13,13,2 + 3,2 + 3,
        14,14,2 + 3,2 + 3,
        15,15,2 + 2,2 + 2
    };
    if (simple_case)
        input_adjacency.clear();


    //Joint types are given to each element, no rtree search will be used here
    for (int i = 0; i < input_polyline_pairs.size(); i+=2) {

        int id = (int)(i*0.5);

        auto input_joint_types_ = std::vector<int>();
    
        //Give first two faces empty joint type
        input_joint_types_.reserve(input_polyline_pairs[i].size()+1);
        input_joint_types_.emplace_back(-1);
        input_joint_types_.emplace_back(-1);
    
       //Give the rest of the faces the faces joint type based on distance
        for (int j = 0; j < input_polyline_pairs[i].size()-1; j++) {
            int type = CGAL::squared_distance(input_polyline_pairs[i+1][j], input_polyline_pairs[i+1][j + 1]) < 800 ? 56 : 58;
            input_joint_types_.emplace_back(type);
        }
        
        input_joint_types.emplace_back(input_joint_types_);       
    }

    for (size_t i = 0; i < input_adjacency.size(); i += 4)
       input_joint_types[input_adjacency[i]][input_adjacency[i + 2]] = 60;
    

    //Three valence
    std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;

    //output
    std::vector<std::vector<CGAL_Polyline>> output_polyline_pairs;
    std::vector<std::vector<char>> output_types;
    std::vector<std::vector<int>> top_face_triangulation;

    //Global Parameters
    std::vector<double> default_parameters_for_joint_types = {
300,
0.5,
8,
450,
0.64,
10,
450,
0.7,
22,
300,
0.5,
30,
300,
0.5,
40,
300,
0.5,
58,
300,
0.25,
60
    };

    std::vector<double> scale = { 1,1,2 };
    int search_type = 0;
    int output_type = 3;


    get_connection_zones(

        //input
        input_polyline_pairs,
        input_insertion_vectors,
        input_joint_types,
        input_three_valence_element_indices_and_instruction,
        input_adjacency,

        //output
        output_polyline_pairs,
        output_types,
        top_face_triangulation,

        //Global Parameters
        default_parameters_for_joint_types,
        scale,
        search_type ,
        output_type,
        0
    );

    
    auto end = std::chrono::high_resolution_clock::now();
    // Calculating total time taken by the program.
    double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    time_taken *= 1e-6;

    std::cout << "\nTime taken by program is : " << std::fixed << time_taken << std::setprecision(9);
    std::cout << " ms" << std::endl;
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Write Polylines to XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //output_polyline_pairs.emplace_back(viewer_polylines);
    xml_parser::write_xml_polylines_and_types(output_polyline_pairs, output_types);
    //printf("\n Loops Starts");
    //for (auto& types : output_types) {
    //    printf("\n Iteration \n");
    //    for (auto& type : types)
    //        printf("%c \n", type);
    //    break;
    //}
    //printf("\n");

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Preview poylylines from xml, take 9-th element
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto viewer = viewer_init();
    viewer_display_polylines(viewer, viewer_polylines,-1,20) ;    
    viewer_display_polylines(viewer, input_polyline_pairs);
    viewer_display_polylines_tree(viewer, output_polyline_pairs);
    viewer_run(viewer);
}
