// #include "../../stdafx.h"

// #include "cgal_box_search.h"

// #include "cgal_intersection_util.h"

// #include "cgal_print.h"
// #include "cgal_vector_util.h"
// #include "cgal_xform_util.h"

// #include "cgal_box_util.h"
// #include "cgal_data_set.h"
// #include "cgal_math_util.h"
// #include "cgal_mesh_util.h"
// #include "cgal_plane_util.h"
// #include "cgal_polyline_util.h"
//
// #include "clipper_util.h"
// #include "rtree_util.h"

#include "wood_cut.h"
#include "wood_joint.h"
#include "wood_element.h" //wood eleement has: #include "wood_joint.h"
#include "wood_joint_lib.h"

#ifndef WOOD_MAIN_H
#define WOOD_MAIN_H

namespace wood_main
{

    void get_elements(
        std::vector<CGAL_Polyline> &pp,
        std::vector<std::vector<IK::Vector_3>> &insertion_vectors,
        std::vector<std::vector<int>> &JOINTS_TYPES,
        std::vector<wood::element> &elements);

    bool border_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Vector_3> &insertion_vectors0,
        int f,
        double &thickness,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB);

    bool plane_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<CGAL_Polyline> &Polyline1,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Plane_3> &Plane1,
        std::vector<IK::Vector_3> &insertion_vectors0,
        std::vector<IK::Vector_3> &insertion_vectors1,
        std::pair<int, int> &el_ids,
        std::pair<std::array<int, 2>, std::array<int, 2>> &face_ids,
        int &type,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB,

        double angleTol = 30,
        bool checkOverlap = false

    );

    // ToDo: Currently one connection can be made with another object, but one multiple shared edges can be possible e.g. |_>-<_|
    bool face_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<CGAL_Polyline> &Polyline1,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Plane_3> &Plane1,
        std::vector<IK::Vector_3> &insertion_vectors0,
        std::vector<IK::Vector_3> &insertion_vectors1,
        std::pair<int, int> &el_ids,
        std::pair<std::array<int, 2>, std::array<int, 2>> &face_ids,
        int &type,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB);

    bool pair_search(

        // Input
        std::vector<wood::element> &elements,       // real wood::element
        std::array<CGAL_Polyline, 4> &beam_volumes, // wood::joint volumes
        const int &polyline_id_0,
        const int &polyline_id_1,

        int search_type,

        // Output
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map

    );

    void rtree_search(

        // Input
        std::vector<wood::element> &elements,
        int search_type,

        // Output
        std::vector<int> &result);

    void adjacency_search(
        // Input
        std::vector<wood::element> &elements,
        int search_type,
        std::vector<int> &input_adjacency,
        // Output
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map);

    void three_valence_joint_addition_vidy(
        std::vector<std::vector<int>> &in_s0_s1_e20_e31,
        std::vector<wood::element> &elements,
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map,
        bool align_joints = true);

    void three_valence_joint_alignment_annen(
        std::vector<std::vector<int>> &out_three_valence_element_indices_and_instruction,
        std::vector<wood::element> &elements,
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map);

    void get_connection_zones(
        std::vector<CGAL_Polyline> &input_polyline_pairs,
        std::vector<std::vector<IK::Vector_3>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,

        // output
        std::vector<std::vector<CGAL_Polyline>> &output_plines,
        std::vector<std::vector<wood_cut::cut_type>> &output_types,
        std::vector<std::vector<int>> &top_face_triangulation,

        // Global Parameters
        std::vector<double> &default_parameters_for_JOINTS_TYPES,
        std::vector<double> &scale,
        int search_type = 0,
        // double division_distance = 300,
        // double shift = 0.6,
        int output_type = 4,
        int triangulate = 0

    );

    void beam_volumes(
        std::vector<CGAL_Polyline> &polylines,
        std::vector<std::vector<double>> &polylines_segment_radii,
        std::vector<std::vector<IK::Vector_3>> &polylines_segment_direction,
        std::vector<int> &allowed_types,
        double &min_distance,
        double &volume_length,
        double &cross_or_side_to_end,
        int &flip_male,

        // output of wood::joint areas
        std::vector<std::array<int, 4>> &polyline0_id_segment0_id_polyline1_id_segment1_id,
        std::vector<std::array<IK::Point_3, 2>> &point_pairs,
        std::vector<std::array<CGAL_Polyline, 4>> &volume_pairs,
        std::vector<CGAL_Polyline> &joints_areas,
        std::vector<int> &joints_types,

        // Global Parameters and output wood::joint selection and orientation
        std::vector<double> &default_parameters_for_JOINTS_TYPES,
        std::vector<std::vector<CGAL_Polyline>> &output_plines,
        std::vector<std::vector<wood_cut::cut_type>> &output_types,
        bool compute_joints = false,
        double division_distance = 300,
        double shift = 0.6,
        int output_type = 3,
        bool use_eccentricities_to_scale_joints = true

    );
}
#endif