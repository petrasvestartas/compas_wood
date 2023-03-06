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
// Rtree library mostly used in rtree_util, more info: https://github.com/nushoin/RTree/issues/22
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef RTREE_UTIL_H
#define RTREE_UTIL_H

namespace rtree_util
{

    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in rtree to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * Get point on a line using a paramter
         *
         * @param [in] line segment
         * @param [in] t line paramter
         * @return point on the line
         */
         IK::Point_3 point_at(const IK::Segment_3 &line, const double &t);

        /**
         * Get closest line parameter to a point
         *
         * @param [in] point point to search from
         * @param [in] line segment
         * @param [out] t line paramter
         * @return bool true if the result is successful
         */

         bool closest_point_to(const IK::Point_3 &point, const IK::Segment_3 &line, double &t);

        /**
         * Get closest distance and closest edge to a polyline, by iterating each line segment and measuring the distance
         *
         * @param [in] point point to search from
         * @param [in] polyline polygon, the method considers closed polygons only
         * @param [out] edge_id polygon edge id
         * @return the closest distance
         */
         double get_closest_distance(const IK::Point_3 &point, CGAL_Polyline &polyline, size_t &edge_id);
    }

    /**
     * Assign edge vectors to the element, by iterating each line segment and measuring the distance
     *
     * @param [in] input_polyline_pairs point to search from
     * @param [in] input_insertion_lines polygon, the method considers closed polygons only
     * @param [out] output_insertion_vectors polygon edge vectors, default vectors are set to IK::Vector_3(0, 0, 0)
     */
     void find_closest_plateside_to_line(
        std::vector<CGAL_Polyline> &input_polyline_pairs,
        std::vector<IK::Segment_3> &input_insertion_lines,
        std::vector<std::vector<IK::Vector_3>> &output_insertion_vectors);

    /**
     * Assign edge vectors to the element, by iterating each line segment and measuring the distance
     *
     * @param [in] input_polyline_pairs elements represented by two polylines
     * @param [in] input_points points to search from
     * @param [in] input_points_types joint type indices assigned to each point, thus the number of points must be equal to the number of indices
     * @param [out] output_joint_types joint type per face (side+top+bottom)
     */
     void find_closest_plateside_to_indexedpoint(
        std::vector<CGAL_Polyline> &input_polyline_pairs,
        std::vector<IK::Point_3> &input_points,
        std::vector<int> &input_points_types,
        std::vector<std::vector<int>> &output_joint_types);
}

#endif // RTREE_UTIL_H
