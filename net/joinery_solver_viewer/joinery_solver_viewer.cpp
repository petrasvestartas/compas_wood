
#include "stdafx.h"
#include "easy3d_polyline_viewer.h"
#include "xml_parser.h"
#include "joinery_solver_main.h"

int main(int argc, char** argv) {
        
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Read Polylines from XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<IK::Point_3>> input_polyline_pairs     ;
    bool result = xml_parser::read_xml_polylines(input_polyline_pairs)  ;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Joinery Solver
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //xml joint path
    path_and_file_for_joints = "C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\joinery_library.xml ";

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

    //Joint types are give to each element, no rtree search will be used here
    for (int i = 0; i < input_polyline_pairs.size(); i+=2) {

        int id = (int)(i*0.5);

        auto input_joint_types_ = std::vector<int>();
        input_joint_types_.reserve(input_polyline_pairs[i].size()+1);
        input_joint_types_.emplace_back(-1);
        input_joint_types_.emplace_back(-1);
        
        for (int j = 0; j < input_polyline_pairs[i].size()-1; j++) {
            int type = CGAL::squared_distance(input_polyline_pairs[i+1][j], input_polyline_pairs[i+1][j + 1]) < 800 ? 57 : 58;
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
        top_face_triangulation,

        //Global Parameters
        default_parameters_for_joint_types,
        scale,
        search_type,
        output_type,
        0
    );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Write Polylines to XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    xml_parser::write_xml_polylines(output_polyline_pairs);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Preview poylylines from xml, take 9-th element
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto viewer = viewer_init();
    viewer_display_polylines(viewer, viewer_polylines,-1,20) ;    
    viewer_display_polylines(viewer, input_polyline_pairs);
    viewer_display_polylines_tree(viewer, output_polyline_pairs);
    viewer_run(viewer);
}
