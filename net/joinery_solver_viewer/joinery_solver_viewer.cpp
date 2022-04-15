#include <iostream>
#include "easy3d_polyline_viewer.h"
#include "xml_parser.h"
#include "joinery_solver_main.h"

int main(int argc, char** argv) {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Read Polylines from XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
    bool result = xml_parser::read_xml_polylines(input_polyline_pairs);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Joinery Solver
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector<std::vector<int>> input_joint_types;
    std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
    std::vector<int> input_adjacency;

    //output
    std::vector<std::vector<CGAL_Polyline>> output_polyline_pairs;
    std::vector<std::vector<int>> top_face_triangulation;

    //Global Parameters
    std::vector<double> default_parameters_for_joint_types = {
        1000,
        0.5,
        1,
        1000,
        0.5,
        10,
        1000,
        0.5,
        20,
        1000,
        0.6,
        30,
        1000,
        0.5,
        40,
        1000,
        0.5,
        50,
    };
    
    std::vector<double> scale = {1,1,1};
    int search_type = 0;
    int output_type = 1;
    int triangulate = 0;

    get_connection_zones(
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
        output_type = 4,
        triangulate = 0
    );


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Write Polylines to XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    xml_parser::write_xml_polylines(output_polyline_pairs);


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Preview poylylines from xml
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    auto viewer = viewer_init();
    viewer_disply_polylines_tree(viewer, output_polyline_pairs);
}
