
#include "stdafx.h"
#include "easy3d_polyline_viewer.h"
#include "xml_parser.h"
#include "joinery_solver_main.h"

//Generate cross connections from lines
//Input: vector of lines
//Output: rectangle volumes
//Output: cross joints
int main(int argc, char** argv) {

    //std::vector<CGAL_Polyline>& polylines,
    //std::vector<std::vector<double>>& polylines_segment_radii,
    //std::vector<std::vector<IK::Vector_3>>& polylines_segment_direction,
    //std::vector<int>& allowed_types,
    //double& min_distance,
    //double& volume_length,
    //double& cross_or_side_to_end,
    //int& flip_male,

    ////output of joint areas
    //std::vector<std::array<int, 4>>& polyline0_id_segment0_id_polyline1_id_segment1_id,
    //std::vector<std::array<IK::Point_3, 2>>& point_pairs,
    //std::vector<std::array<CGAL_Polyline, 4>>& volume_pairs,
    //std::vector<CGAL_Polyline>& joints_areas,
    //std::vector<int>& joints_types,

    ////Global Parameters and output joint selection and orientation
    //std::vector<double>& default_parameters_for_joint_types,
    //std::vector<std::vector<CGAL_Polyline>>& joints_oriented_polylines,
    //bool compute_joints = false,
    //double division_distance = 300,
    //double shift = 0.6,
    //int output_type = 3



    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Read Polylines from XML
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //pollylines axes
    bool simple_case = false;
    std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
    path_and_file_for_input_polylines = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_polylines.xml";
    bool result = xml_parser::read_xml_polylines(input_polyline_pairs, simple_case);
    if (!result) {
        std::cout << "Error input_polylines.xml";
        return 0;
    }

    //radii
    std::vector<std::vector<double>> input_radii;
    path_and_file_for_input_numbers = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_numbers.xml";
    result = xml_parser::read_xml_numbers(input_radii);
    if (!result) {
        std::cout << "Error input_numbers.xml";
        return 0;
    }

    //direction
    std::vector<std::vector<IK::Vector_3>> polylines_segment_direction;


    //allowed types
    //allowed_types: -1 - all possible types, 0 - end - to - end, 1 - cross or side - to - end, this option is needed because multiple joint can be made in one intersection,
    std::vector<int> allowed_types;
    allowed_types.reserve(input_polyline_pairs.size());
    for (int i = 0; i < input_polyline_pairs.size(); i++)
        allowed_types.emplace_back(-1);



    //min_distance : double distance until which search is valid
    //Parameter t explanation
    // x < 0 - all cross
    //x > 1.0 all side - end
    // how close the point is to the middle from
    //(closest to the middle) 0 <= x <= 1 (closest to the edge)
    //possible output : one is above another below - side - to - end joint  e.g. 0.9999
    //possible output : when both are above, the smaller is female, other is male 0.75
    //possible output : when both are below, then cross, e.g. 0.0001
    double min_distance = 50.0;


    // volume_length : double length of beam volumes
    double volume_length = 100;

    //cross_or_side_to_end : double type0_type1_parameter
    double cross_or_side_to_end = -1; 1 - 0.91;

    //flip_male : property for side-to-end volumes 0 - no shift, -1 shift to right, 1 shift to left
    int flip_male = 0;




    //Output
    std::vector<std::array<int, 4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
    std::vector<std::array<IK::Point_3, 2>> point_pairs;
    std::vector<std::array<CGAL_Polyline, 4>> volume_pairs;
    std::vector<CGAL_Polyline> joints_areas;
    std::vector<int> joints_types;

    //Global Parameters and output joint selection and orientation
    double division_length = 300;
    std::vector<double> default_parameters_for_joint_types{
        300,
        0.5,
        8,
        450,
        0.64,
        10,
        450,
        0.7,
        22,
        300,//cross joints
        0.1,//cross joints
        32, //cross joints
        300,
        0.5,
        40,
        300,
        0.5,
        58,
        300,
        1.0,
        60
    };

    std::vector<std::vector<CGAL_Polyline>> output_plines;
    std::vector<std::vector<char>> output_types;
    bool compute_joints = true;
    double division_distance = 500;
    double shift = 0.6;
    int output_type = 3;


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Joinery Solver
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   // std::vector < double> 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,


    beam_volumes(

        //input
        input_polyline_pairs,
        input_radii,
        polylines_segment_direction,
        allowed_types,
        min_distance,
        volume_length,
        cross_or_side_to_end,
        flip_male,

        //ouput
        polyline0_id_segment0_id_polyline1_id_segment1_id,
        point_pairs,
        volume_pairs,
        joints_areas,
        joints_types,

        //Global Parameters and output joint selection and orientation
        default_parameters_for_joint_types,
        output_plines,
        output_types,
        compute_joints,
        division_distance,
        shift,
        output_type

    );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //WritePolylines
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    xml_parser::write_xml_polylines_and_types(output_plines, output_types);

    std::vector<CGAL_Polyline> output_plines2;
    for (auto& volume : volume_pairs) {
        output_plines2.emplace_back(volume[0]);
        output_plines2.emplace_back(volume[1]);
        output_plines2.emplace_back(volume[2]);
        output_plines2.emplace_back(volume[3]);
    }

    //xml_parser::write_xml_polylines(joints_areas);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Preview polylines from xml, take 9-th element
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto viewer = viewer_init();
    viewer_display_polylines(
        viewer,
        input_polyline_pairs
    );

    //viewer_display_polylines_tree(viewer, output_polyline_pairs, 6);


    for (auto& point_pair : point_pairs) {
        std::vector<CGAL_Polyline> polylines{ CGAL_Polyline{point_pair[0],point_pair[1]} };
        viewer_display_polylines(
            viewer,
            polylines, -1, 10

        );
    }


    for (auto& volume : volume_pairs) {
        std::vector<CGAL_Polyline> polylines{ volume[0] ,volume[1] ,volume[2] ,volume[3] };
        viewer_display_polylines(
            viewer,
            polylines
        );
    }


    viewer_display_polylines(
        viewer,
        joints_areas, -1, 5
    );

    viewer_display_polylines_tree(viewer, output_plines);


    //viewer_display_polylines()
    //    //std::vector<std::array<CGAL_Polyline, 4>> volume_pairs;
    //    //volume_pairs(viewer, input_polyline_pairs, 6);
    //    viewer_run(viewer);


    //    auto viewer = viewer_init();
    //    viewer_display_polylines(viewer, viewer_polylines, -1, 20);

    //    viewer_display_polylines_tree(viewer, output_polyline_pairs, 6);
    viewer_run(viewer);

}
