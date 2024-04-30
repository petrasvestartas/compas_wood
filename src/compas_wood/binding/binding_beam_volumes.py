from compas.geometry import Polyline


from wood_nano import beam_volumes as wood_nano_beam_volumes
from compas_wood.conversions_compas import to_point2
from compas_wood.conversions_compas import to_vector2
from wood_nano.conversions_python import to_double2
from wood_nano.conversions_python import to_int1
from wood_nano import point2
from wood_nano import int1
from wood_nano import GLOBALS


def beam_volumes(
    input_polyline_axes,
    input_polylines_segment_radii,
    input_polylines_segment_direction,
    input_allowed_types=[1],
    input_min_distance=20,
    input_volume_length=500,
    input_cross_or_side_to_end=0.91,
    input_flip_male=1,
    input_compute_joints=True,
    input_division_distance=150,
    input_shift=0.5,
    input_output_geometry_type=0,
    input_use_eccentricities_to_scale_joints=True,
    
    ):

        
        polyline0_id_segment0_id_polyline1_id_segment1_id = None
        point_pairs = None
        volume_pairs = None
        joints_areas = point2()
        joints_types = int1()
        w_output_plines = wood_nano_point3()
        w_output_types = wood_nano_cut_type2()
        

        wood_nano_beam_volumes(
            to_point2(input_polyline_axes),
            to_double2(input_polylines_segment_radii),
            to_vector2(input_polylines_segment_direction),
            to_int1(input_allowed_types),
            float(input_min_distance),
            float(input_volume_length),
            float(input_cross_or_side_to_end),
            int(input_flip_male),
            polyline0_id_segment0_id_polyline1_id_segment1_id, #out
            point_pairs,  #out
            volume_pairs,  #out
            joints_areas,  #out
            joints_types, #out
            GLOBALS,
            w_output_plines,
            w_output_types,
            input_compute_joints,
            input_division_distance,
            input_shift,
            input_output_geometry_type,
            input_use_eccentricities_to_scale_joints
            )



# bool
# type_beams_name_phanomema_node()
# {

#     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     // The filename of the xml file and the screenshot directory
#     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     std::vector<std::vector<IK::Point_3>> input_polyline_axes;
#     internal::set_file_path_for_input_xml_and_screenshot(input_polyline_axes, "type_beams_name_phanomema_node");

#     wood::GLOBALS::JOINT_VOLUME_EXTENSION[2] = 0;
#     wood::GLOBALS::JOINTS_PARAMETERS_AND_TYPES[1 * 3 + 0] = 150; // division_length
#     int search_type = 0;
#     int output_type = wood::GLOBALS::OUTPUT_GEOMETRY_TYPE; // 0 - Plate outlines 1 - joint lines 2 - joint volumes 3 - joint geometry 4 - merge
#     // std::cout << "\n output_type " << output_type << "\n";
#     std::vector<double> scale = {1, 1, 1};
#     std::vector<std::vector<IK::Vector_3>> input_insertion_vectors{};
#     std::vector<std::vector<int>> input_JOINTS_TYPES{};
#     // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {{1}, {16, 10, 11, 17}};
#     std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = {};

#     std::vector<int> input_adjacency = {};
#     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     // Main Method of Wood
#     ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     std::vector<std::vector<double>> polylines_segment_radii;
#     std::vector<std::vector<IK::Vector_3>> polylines_segment_direction;
#     for (int i = 0; i < input_polyline_axes.size(); i++)
#     {
#         std::vector<double> polyline_segment_radii;
#         // std::vector<IK::Vector_3> polyline_segment_direction;
#         for (int j = 0; j < input_polyline_axes[i].size(); j++)
#         {
#             polyline_segment_radii.emplace_back(150);
#             // polyline_segment_direction.emplace_back(0.037013, 0.909575, 0.413887);
#         }
#         polylines_segment_radii.emplace_back(polyline_segment_radii);
#         // polylines_segment_direction.emplace_back(polyline_segment_direction);
#     }

#     std::vector<int> allowed_types{1};

#     double min_distance = 20;
#     double volume_length = 500;
#     double cross_or_side_to_end = 0.91;
#     int flip_male = 1;

#     // output of wood::joint areas
#     std::vector<std::array<int, 4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
#     std::vector<std::array<IK::Point_3, 2>> point_pairs;
#     std::vector<std::array<CGAL_Polyline, 4>> volume_pairs;

#     std::vector<CGAL_Polyline> joints_areas;
#     std::vector<int> joints_types;

#     // Global Parameters and output wood::joint selection and orientation
#     std::vector<std::vector<CGAL_Polyline>> output_plines;
#     std::vector<std::vector<wood::cut::cut_type>> output_types;

#     bool compute_joints = true;                     // WARNING values are in unknown state
#     double division_distance = 150;                 // WARNING values are in unknown state
#     double shift = 0.5;                             // WARNING values are in unknown state
#     bool use_eccentricities_to_scale_joints = true; // WARNING values are in unknown state

#     wood::main::beam_volumes(
#         input_polyline_axes,
#         polylines_segment_radii,
#         polylines_segment_direction,
#         allowed_types,
#         min_distance,
#         volume_length,
#         cross_or_side_to_end,
#         flip_male,
#         // output of wood::joint areas
#         polyline0_id_segment0_id_polyline1_id_segment1_id,
#         point_pairs,
#         volume_pairs,
#         joints_areas,
#         joints_types,
#         // Global Parameters and output wood::joint selection and orientation
#         wood::GLOBALS::JOINTS_PARAMETERS_AND_TYPES,
#         output_plines,
#         output_types,
#         compute_joints,
#         division_distance,
#         shift,
#         wood::GLOBALS::OUTPUT_GEOMETRY_TYPE,
#         use_eccentricities_to_scale_joints);

#     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     // Export
#     //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#     wood::xml::path_and_file_for_output_polylines = wood::GLOBALS::DATA_SET_OUTPUT_FILE;
#     output_plines.clear();
#     for (auto &pline : volume_pairs)
#         for (int j = 0; j < 4; j++)
#             output_plines.emplace_back(std::vector<CGAL_Polyline>{pline[j]});

#     wood::xml::write_xml_polylines_and_types(output_plines, output_types);

#     return true;
# }