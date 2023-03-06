///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEVELOPER:
// Petras Vestartas, petasvestartas@gmail.com
// Funding: NCCR Digital Fabrication and EPFL
//
// HISTORY:
// 1) The first version was written during the PhD 8928 thesis of Petras Vestartas called:
// Design-to-Fabrication Workflow for Raw-Sawn-Timber using Joinery Solver, 2017-2021
// 2) The translation from C# to C++ was started during the funding of NCCR in two steps
// A - standalone C++ version of the joinery solver and B - integration to COMPAS framework (Python Pybind11)
//
// RESTRICTIONS:
// The code cannot be used for commercial reasons
// If you would like to use or change the code for research or educational reasons,
// please contact the developer first
//
// 3RD PARTY LIBRARIES:
// None
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

// #include "../../stdafx.h"

#ifndef WOOD_ELEMENT_H
#define WOOD_ELEMENT_H

#include "wood_joint.h"


namespace wood
{
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

        std::vector<int> JOINTS_TYPES; // if set - used in joint_library | method construct_joint_by_index | if set negative is female

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

        /**
         * ATTENTION THIS IS A SPECIFIC CLIPPER FUNCTION TO CLIP USING OPEN PATHS TO OBTAIN CLEAN END POINTS
         * // https://github.com/AngusJohnson/Clipper2/blob/main/CPP/Clipper2Lib/include/clipper2/clipper.h
         *
         * @param closed_pline_cutter plate outline
         * @param pline_to_cut joint outline
         * @param plane plane for orientation to XY axies
         * @param c output
         * @param edge_pair not used
         * @param cp_pair plate outline segment parameters to correctly insert joint between the outline points
         * @return true if intersection is found
         */

        bool intersection_closed_and_open_paths_2D(
            CGAL_Polyline &closed_pline_cutter, CGAL_Polyline &pline_to_cut, IK::Plane_3 &plane, CGAL_Polyline &c,
            int (&edge_pair)[2], std::pair<double, double> &cp_pair);

        void merge_joints(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output);

        /**
         * Intersect line segments with the neighbors of elements
         * Lines can be inclined
         * lines can be shorter or longer
         * so the length of them has to be updated
         *
         * @param joints a list of connections between two elements
         * @param elements a list of elements
         */
        void update_lines_by_intersection(std::vector<wood::joint> &joints, std::vector<wood::element> &elements);
    };
}
#endif
