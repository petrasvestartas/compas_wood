// nothing = '0',
//
////plates
// edge_insertion = '1',
// hole = '2',
// insert_between_multiple_edges = '3',
//
////beams
// slice = '4', //project and make rectangle
// mill_project = '5', //
// mill = '6', //project
// cut = '7',
// conic_reverse = '8',		//project
////binary_slice_mill = '9', //project and make rectangle
// conic = '9'

//#include "../../stdafx.h"
#include "wood_joint.h"

#ifndef WOOD_ELEMENT_H
#define WOOD_ELEMENT_H
namespace wood{
class element
{
public:
    /////////////////////////////////////////////////////////////////////////////////////////
    // Parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    int id = -1;
    double thickness = 0;

    /////////////////////////////////////////////////////////////////////////////////////////
    // Constructors
    /////////////////////////////////////////////////////////////////////////////////////////
    element();
    element(int _id);

    /////////////////////////////////////////////////////////////////////////////////////////
    // Basic Geometry Parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    CGAL::Bbox_3 aabb;
    IK::Vector_3 oob[5];
    std::vector<CGAL_Polyline> polylines; // pairs of polylines + each side face
    std::vector<IK::Plane_3> planes;      // top and bottom planes + each side face

    std::vector<IK::Vector_3> edge_vectors; // if set

    std::vector<int> joint_types; // if set - used in joint_library | method construct_joint_by_index | if set negative is female

    IK::Segment_3 axis;

    // For beams only
    CGAL_Polyline central_polyline;

    /////////////////////////////////////////////////////////////////////////////////////////
    // Joinery Geometry Parameters
    // pointers to joints, wood::joint geometry will be modified inside wood::joint to avoid copy
    // wood::joint must be sorted according to edge id to be merged
    /////////////////////////////////////////////////////////////////////////////////////////
    // std::map<int, std::pair<bool, int>>edgeID_mf_jointID; //e0,true,jointID
    std::vector<std::vector<std::tuple<int, bool, double>>> j_mf; //(wood::joint id, male/female, parameter on edge) elements[result[i + 0]].j_mf[e0].push_back(std::tuple<int, bool, double>(jointID, true,0));
    // std::vector<CGAL_Polyline> modified_polylines;
    // public Mesh mesh;

    /////////////////////////////////////////////////////////////////////////////////////////
    // Component parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    std::string key;
    IK::Plane_3 component_plane;

    void get_joints_geometry(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output, int what_to_expose, std::vector<std::vector<wood_cut::cut_type>> &output_cut_types);

    // bool sort_by_third(const std::tuple<int, bool, double> &a, const std::tuple<int, bool, double> &b);

    void get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output);
    bool intersection_closed_and_open_paths_2D(
        CGAL_Polyline &closed_pline_cutter, CGAL_Polyline &pline_to_cut, IK::Plane_3 &plane, CGAL_Polyline &c,
        int (&edge_pair)[2], std::pair<double, double> &cp_pair);
    void merge_joints(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output);
};
}
#endif
