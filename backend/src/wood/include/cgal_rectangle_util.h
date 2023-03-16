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

#ifndef CGAL_RECTANGLE_UTIL_H
#define CGAL_RECTANGLE_UTIL_H

namespace cgal_rectangle_util
{

    namespace internal
    {

        /**
         * Get length of a 3D vector, taken from the OpenNURBS library: https://github.com/mcneel/opennurbs
         *
         * @param [in] x The first coordinate of the vector
         * @param [in] y The second coordinate of the vector
         * @param [in] z The third coordinate of the vector
         * @return The length of the vector
         */
        double length(double x, double y, double z);

        /**
         * Unitize the vector by dividing its coordinates by its length
         *
         * @param [in, out] vector 3D vector
         * @return true if the vector length is not equal to zero
         */
        bool unitize(IK::Vector_3 &vector);

        /**
         * transformation matrix, orient from plane to xy plane
         *
         * @param [in] origin plane origin
         * @param [in] plane plane, from which x-axis and y-axis and z-axis are extracted
         * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * it checks if points are clockwise or counter clockwise
         * The z-value of the cross product of segments | (a, b) and (a, c). Positive means c is ccw | from (a, b), negative cw. Zero means its collinear.
         *
         * @param a point
         * @param b point
         * @param c point
         * @return float value
         */
        float ccw(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &c);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Returns true if a is lexicographically before b.
         *
         * @param a point
         * @param b point
         * @return bool value
         */
        bool is_left_of(const IK::Point_3 &a, const IK::Point_3 &b);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * struct for sorting points
         */
        struct ccw_sorter
        {
            const IK::Point_3 &pivot;

            ccw_sorter(const IK::Point_3 &in_pivot) : pivot(in_pivot) {}

            bool operator()(const IK::Point_3 &a, const IK::Point_3 &b)
            {
                return ccw(pivot, a, b) < 0;
            }
        };

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * the length of segment (a, b).
         * @param a point
         * @param b point
         * @return float value that represent the length of a line
         */
        float len(const IK::Point_3 &a, const IK::Point_3 &b);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * The unsigned distance of p from segment (a, b).
         * @param a point
         * @param b point
         * @return float value represented the distance
         */
        float dist(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &p);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Returns the index of the farthest point from segment (a, b).
         * @param a point
         * @param b point
         * @param v points to search from
         * @return size_t index of the farthest point
         */
        size_t get_farthest(const IK::Point_3 &a, const IK::Point_3 &b, const std::vector<IK::Point_3> &v);

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Recursive call of the quick_hull algorithm.
         * This function is used by the main function below
         * @param [in] v point
         * @param [in] a point
         * @param [in] b point
         * @param [out] hull points
         */
        void quick_hull(const std::vector<IK::Point_3> &v, const IK::Point_3 &a, const IK::Point_3 &b, std::vector<IK::Point_3> &hull);

        /**
         * WARNING user must not use this method - this method is used by bounding_rectangle algorithm
         * get angle of an edge to its projects to the x-axis, this is the helper fuction of bounding_rectangle
         *
         * @param edge polyline segment
         * @return angle
         */
        double angle_to_xaxis(IK::Segment_3 &edge);

        /**
         * WARNING user must not use this method - this method is used by bounding_rectangle algorithm
         * get edge after rotation to x axis, this is the helper fuction of bounding_rectangle
         *
         * @param vector segment direction
         * @param angle angle in radians
         * @return rotated vector
         */
        IK::Point_3 rotate_to_xaxis(IK::Point_3 &point, const double &angle);




        /**
         * get transformation from 3D to 2D and invcerse from a polyline
         * the 3D plane is computed from the start point of the polyline
         * and an the sum of the consecutive edges cross products
         *
         * @param [in] polyline input CGAL polyline
         * @param [out] xform_to_xy transformation from 3D to XY
         * @param [out] xform_to_xy_inv transformation from XY to 3D
         * @return returns true if the polyline has more than 3 points, else not valid
         */
        bool orient_polyline_to_xy_and_back(CGAL_Polyline &polyline, CGAL::Aff_transformation_3<IK> &xform_to_xy, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv);
    }

    /**
     * quick_hull algorithm
     * https://en.wikipedia.org/wiki/quick_hull
     * https://github.com/MiguelVieira/ConvexHull2D/blob/master/ConvexHull.cpp
     *
     * @param [in, out] v input polylines or a list of points
     * @return points that re
     */
    CGAL_Polyline quick_hull(const CGAL_Polyline &v);

    /**
     * get bounding rectangle of a polygon
     * the polygon can be rotated in 3d, because the function orients the polygon to XY plane
     * port from the C# version: https://github.com/cansik/LongLiveTheSquare/blob/master/U4LongLiveTheSquare/MinimalBoundingBox.cs
     *
     * @param [in] polygon input polyline
     * @param [out] result output rectangle
     * @return bool flag if the result is valid
     */
    bool bounding_rectangle(CGAL_Polyline &polygon, CGAL_Polyline &result);

    /**
     * @brief Calculate a grid of points inside a polygon by a) convex hull, b) its bounding box, c) grid of points inside the polygon
     *
     * @param [in] polygon input polyline
     * @param [in] offset_distance negative value is inwards offset, positive outwards, 0 is ignored. If the offset distance is too big, the initial polygon will be used without offset
     * @param [in] division_distance distance between points, if 0 the distance is calculated automatically
     * @param [in] max_points number of points
     * @param [out] points output points
     * @return bool flag if the result is valid
     */
    bool grid_of_points_in_a_polygon(CGAL_Polyline &polygon, const double &offset_distance, const double &division_distance, const int &max_points, std::vector<IK::Point_3> &points);

}

#endif