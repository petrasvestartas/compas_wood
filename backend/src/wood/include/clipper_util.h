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
// Clipper2 library mostly used in clipper_util, more info: https://github.com/AngusJohnson/Clipper2
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CLIPPER_UTIL_H
#define CLIPPER_UTIL_H

namespace clipper_util
{
    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in clipper_util to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
         * The transformation matrix from the plane to XY plane
         *
         * @param [in] origin point for translation from origin
         * @param [in] plane three vectors for rotation around the origin
         * @return transformation matrix, that is equal the rotation multiplied by translation
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);

        /**
         * The average normal computed my the sum of the concentric cross-products
         *
         * @param [in] polyline Open or closed polygon
         * @param [out] average_normal The sum of cross-products, computed from the current and neighboring points, considering both open and closed polylines
         */
        void get_average_normal(const CGAL_Polyline &polyline, IK::Vector_3 &average_normal);

        /**
         * Get a plane and its origin point, often used for orienting polylines from 3D to 2D
         *
         * @param [in] polyline Open or closed polygon
         * @param [out] center The first point of the polyline is taken as the output
         * @param [out] plane The plane is computed using the get_average_normal method
         */
        void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &center, IK::Plane_3 &plane);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polylines from 3D to 2D, overloads includes no copy and copy
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Copy and orient a list of polygons from 3D to 2D and ouput the inverse transformation matrix
         *
         * @param [in] input cgal list of polyline
         * @param [out] output cgal list of polyline
         * @param [out] xform_to_xy_inv inverse transformation matrix, because orientation to 2D is often followed by orienting geometry back to 3D
         */
        void orient_3d_to_2d(const std::vector<CGAL_Polyline> &input, std::vector<CGAL_Polyline> &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv);

        /**
         * Copy and orient a polygon from 3D to 2D and ouput the inverse transformation matrix
         *
         * @param [in] input a cgal polyline
         * @param [out] output a cgal polyline
         * @param [out] xform_to_xy_inv inverse transformation matrix, because orientation to 2D is often followed by orienting geometry back to 3D
         */
        void orient_3d_to_2d(const CGAL_Polyline &input, CGAL_Polyline &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv);

        /**
         * Orient a list of polygons from 3D to 2D and ouput the inverse transformation matrix
         *
         * @param [in, out] input cgal list of polyline
         * @param [out] xform_to_xy_inv inverse transformation matrix, because orientation to 2D is often followed by orienting geometry back to 3D
         */
        void orient_3d_to_2d(std::vector<CGAL_Polyline> &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv);

        /**
         * Orient a polygon from 3D to 2D and ouput the inverse transformation matrix
         *
         * @param [in, out] input cgal list of polyline
         * @param [out] xform_to_xy_inv inverse transformation matrix, because orientation to 2D is often followed by orienting geometry back to 3D
         */
        void orient_3d_to_2d(CGAL_Polyline &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Conversions of CGAL_Polyline and Clipper in 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Convert cgal polylines to clipper polylines
         *
         * @param [in] input cgal list of polyline
         * @param [out] output clipper list of polyline
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolylines_to_clipper_2d(const std::vector<CGAL_Polyline> &input, Clipper2Lib::Paths64 &output, bool closed_polyline = true);

        /**
         * Convert clipper polylines to cgal polylines
         *
         * @param [in] input clipper list of polyline
         * @param [out] output cgal list of polyline
         * @param [in] closed_polyline adds the first point from the clipper polyline to the end
         */
        void clipper_to_cgalpolylines_2d(const Clipper2Lib::Paths64 &input, std::vector<CGAL_Polyline> &output, bool closed_polyline = true);

        /**
         * Convert cgal polyline to clipper polyline
         *
         * @param [in] input cgal polyline
         * @param [out] output clipper polyline
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolyline_to_clipper_2d(const CGAL_Polyline &input, Clipper2Lib::Path64 &output, bool closed_polyline = true);

        /**
         * Convert clipper polyline to cgal polyline
         *
         * @param [in] input clipper polyline
         * @param [out] output cgal polyline
         * @param [in] closed_polyline adds the first point from the clipper polyline to the end
         */
        void clipper_to_cgalpolyline_2d(const Clipper2Lib::Path64 &input, CGAL_Polyline &output, bool closed_polyline = true);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Conversions of CGAL_Polyline and Clipper in 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * Convert cgal polylines to clipper polylines
         *
         * @param [in] input cgal list of polyline
         * @param [out] output clipper list of polyline
         * @param [out] xform_to_xy_inv orientation matrix to 3D
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolylines_to_clipper_3d(const std::vector<CGAL_Polyline> &input, Clipper2Lib::Paths64 &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline = true);

        /**
         * Convert clipper polylines to cgal polylines
         *
         * @param [in] input clipper list of polyline
         * @param [in] xform_to_xy_inv orientation matrix to 3D
         * @param [out] output cgal list of polyline
         * @param [in] closed_polyline adds the first point from the clipper polyline to the end
         */
        void clipper_to_cgalpolylines_3d(const Clipper2Lib::Paths64 &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, std::vector<CGAL_Polyline> &output, bool closed_polyline = true);

        /**
         * Convert cgal polyline to clipper polyline
         *
         * @param [in] input cgal polyline
         * @param [out] output clipper polyline
         * @param [out] xform_to_xy_inv orientation matrix to 3D
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolyline_to_clipper_3d(const CGAL_Polyline &input, Clipper2Lib::Path64 &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline = true);

        /**
         * Convert clipper polyline to cgal polyline
         *
         * @param [in] input clipper polyline
         * @param [in] xform_to_xy_inv orientation matrix to 3D
         * @param [out] output cgal polyline
         * @param [in] closed_polyline adds the first point from the clipper polyline to the end
         */
        void clipper_to_cgalpolyline_3d(const Clipper2Lib::Path64 &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, CGAL_Polyline &output, bool closed_polyline = true);

        /**
         * Convert cgal polylines to clipper polylines
         *
         * @param [in] input cgal list of polyline
         * @param [in] plane plane whose origin is the input the first of the first outline's points
         * @param [out] output clipper list of polyline
         * @param [out] xform_to_xy_inv orientation matrix to 3D
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolylines_to_clipper_3d(const std::vector<CGAL_Polyline> &input, const IK::Plane_3 &plane, Clipper2Lib::Paths64 &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline = true);

        /**
         * Convert cgal polyline to clipper polyline
         *
         * @param [in] input cgal polyline
         * @param [in] plane the plane of the polyline, the first point of the polyline is taken as the origin
         * @param [out] output clipper polyline
         * @param [out] xform_to_xy_inv orientation matrix to 3D
         * @param [in] closed_polyline skips the last point from the cgal polyline
         */
        void cgalpolyline_to_clipper_3d(const CGAL_Polyline &input, const IK::Plane_3 &plane, Clipper2Lib::Path64 &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline = true);
    }

    /**
     * Check if the point is inside the polyline or touching
     *
     * @param [in] polyline closed polyline
     * @param [in] plane the plane of the polyline
     * @param [in] point point to check the inclusion
     * @return return true if the point is inside the polyline or touching
     */
    bool is_point_inside(CGAL_Polyline &polyline, IK::Plane_3 &plane, IK::Point_3 &point);

    /**
     * Orient two polylines using the first polyline's plane, computed the boolean intersection, and returns the result back to 3D
     * The result igonores very small polygons within the tolerance: wood_globals::CLIPPER_AREA)
     * The user can choose to include the triangles in the result
     *
     * @param [in] polyline0 the first polyline
     * @param [in] polyline1 the second touching polyline
     * @param [in] plane the plane of the first polyline
     * @param [out] intersection_result the output of the clipper algorithm, if successful
     * @param [in] intersection_type the default parameter: 0 - Intersect, 1 - Union, 2 - Difference, 3 - Xor
     * @param [in] include_triangles the default paramter to skip polygons consisting of only 3 points
     * @return return true if the point is inside the polyline or touching
     */
    bool get_intersection_between_two_polylines(CGAL_Polyline &polyline0, CGAL_Polyline &polyline1, IK::Plane_3 &plane, CGAL_Polyline &intersection_result, int intersection_type = 0, bool include_triangles = false);

    /**
     * Offset polyline without holes in 2D
     * WARNING: the offset return several polylines, but only the first one is returned
     * WARNING: the polyline must be position on XY plane
     *
     * @param [in] polyline the closed polyline
     * @param [in] offset the distance
     * @return true if the offset the polylgon is not super small
     */
    bool offset_in_2d(CGAL_Polyline &polyline, const double &offset);

    /**
     * Offset polyline without holes in 3D
     * WARNING: the offset return several polylines, but only the first one is returned
     *
     * @param [in] polyline the closed polyline
     * @param [in] offset the distance
     * @return true if the offset the polylgon is not super small
     */
    bool offset_in_3d(CGAL_Polyline &polyline, IK::Plane_3 &plane, const double &offset);

    /**
     * Check if the points are inside the polyline or touching
     *
     * @param [in] polyline the closed polyline
     * @param [in] plane the plane of the first polyline
     * @param [in] test_points points to check the inclusion
     * @param [out] test_points_ids return the ids of the points that are inside the polyline
     * @return number of ponts that are inside the polyline
     */
    int are_points_inside(CGAL_Polyline &polyline, IK::Plane_3 &plane, std::vector<IK::Point_3> &test_points, std::vector<int> &test_points_ids);

    /**
     * bounding rectangle of a planar polyline
     * the polyline is oriented to XY plane using the input plane
     * then oriented back to 3D
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * @param polyline planar polyline rotated in space
     * @param plane polyline's plane
     * @param result bounding rectangle as CGAl polyline
     * @return bool if the result is successful, e.g. box is valid or the input polyline has less than 3 points
     */
    bool bounding_rectangle(CGAL_Polyline &polyline, IK::Plane_3 &plane, CGAL_Polyline &result);

    /**
     * offset and divide a planar polyline to points
     * WARNING: the offset is running the clipper offset algorithm, but only one polyline is outputed
     * the polyline takes into account 3d outlines
     * the division method adds all corner points and interpolates corners by a division distance
     * the method is able to divide open and closed polylines
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * @param [out] result division points
     * @param [in] polygon polyline
     * @param [in] offset_distance distance must be negative to offset the polyline inwards
     * @param [in] division_distance distance between two points
     * @return bool if the result is successful, e.g. box is valid or the input polyline has less than 3 points
     */
    bool offset_and_divide_to_points(std::vector<IK::Point_3> &result, std::vector<IK::Point_3> &polygon, double offset_distance = -2.5, double division_distance = 10);
}

#endif