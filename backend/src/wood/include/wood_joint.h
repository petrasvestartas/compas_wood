//#pragma once
#include "wood_cut.h"

#ifndef WOOD_JOINT_H
#define WOOD_JOINT_H
namespace wood
{
    // Container for cuts
    class joint
    {
        // Methods

        bool change_basis(CGAL_Polyline &rect0, CGAL_Polyline &rect1, CGAL::Aff_transformation_3<IK> &xform); // first get 2x ChangeBasis transformation matrices

    public:
        /////////////////////////////////////////////////////////////////////////////////////////
        // Parameters from Search method v-volume f-face
        /////////////////////////////////////////////////////////////////////////////////////////
        int id = 0, v0, v1, f0_0, f1_0, f0_1, f1_1, type; // 10 - SS Rotate 11 - SS OUT OF PLANE 12 - SS IN Plane,  20 Top-Side, 30 - Cross
        std::string key = "";
        CGAL_Polyline joint_area;                         // delete
        CGAL_Polyline joint_lines[2];                     // delete
        // CGAL_Polyline joint_quads[2];//delete
        CGAL_Polyline joint_volumes[4]; // mostly 2, but can be 4 e.g. in-plane side-side

        /////////////////////////////////////////////////////////////////////////////////////////
        // Detailed parameters for geometry transfer from library or custom made
        /////////////////////////////////////////////////////////////////////////////////////////
        std::string name = "";
        int id_of_global_joint_list = -1;    // Directs which joint applies where, -1 all cases
        std::vector<double> tile_parameters; // For rebuilding

        std::array<std::vector<CGAL_Polyline>, 2> m;
        std::vector<wood_cut::cut_type> m_boolean_type; // 0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

        std::array<std::vector<CGAL_Polyline>, 2> f;
        std::vector<wood_cut::cut_type> f_boolean_type; // 0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

        // if this property is enable, joint volume rectangles are moved within unit_scale_distance, this property is equal to first element thickness
        bool unit_scale = false;
        double unit_scale_distance = 0;
        bool orient = true;
        std::array<double, 3> scale = {1, 1, 1};

        // Geometrical divisions
        double shift = 0.5;
        int divisions = 1;
        double length = 1;
        bool compute_geometrical_divisions = true;



        /////////////////////////////////////////////////////////////////////////////////////////
        // Constructors
        /////////////////////////////////////////////////////////////////////////////////////////
        joint();
        joint(int, int, int, int, int, int, int, std::array<CGAL_Polyline, 4> &);
        joint(int, int, int, int, int, int, int, CGAL_Polyline(&), std::array<CGAL_Polyline, 2> &, std::array<CGAL_Polyline, 4> &, int);

        // Operators
        //  IK::Vector_3(&input)[4]
        std::vector<CGAL_Polyline> &operator()(bool male_or_female, bool first_or_second);

        void get_edge_ids(bool male_or_female, int &fA, int &fB);

        wood_cut::cut_type &get_first_cutting_type(bool male_or_female);

        std::vector<wood_cut::cut_type> &operator()(bool male_or_female);

        void reverse(bool male_or_female);

        std::string get_key();

        void transform(CGAL::Aff_transformation_3<IK> &xform0, CGAL::Aff_transformation_3<IK> &xform1); // Custom user transformation

        bool orient_to_connection_area(); // Orient to connection area if rectangles are set

        void duplicate_geometry(joint &);

        void transfer_geometry(joint &);

        void get_divisions(double &division_distance);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joint linking with other joints
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        bool link = false;                                              // assigned in three_valence_joint_addition_vidy for the link joint
        std::vector<int> linked_joints;                                 // assigned in three_valence_joint_addition_vidy for the main joint
        std::vector<std::vector<std::array<int, 4>>> linked_joints_seq; // assigned on wood_joint_library | it is nested because there can be umber of polylines | example {start_curr,step_curr} means that "start_curr+step_curr*i" and {start_link,step_link} -> "start_link+step_link*i"

        void remove_geo_from_linked_joint_and_merge_with_current_joint(std::vector<joint> &all_joints); // it is called if linked_joints vector is not empty | also check wood_joint -> joint linking with other joints
    };
}
#endif
