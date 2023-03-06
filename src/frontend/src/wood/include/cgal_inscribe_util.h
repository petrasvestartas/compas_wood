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
// Get center with radius in a polygon, the polylabel folder contains all the data structures from: https://github.com/mapbox/polylabel
// CGAL for largest empty area: https://doc.cgal.org/latest/Inscribed_areas/index.html
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_INSCRIBE_UTIL_H
#define CGAL_INSCRIBE_UTIL_H

#include <polylabel/polylabel.h>
#include <CGAL/Largest_empty_iso_rectangle_2.h>

namespace cgal_inscribe_util
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
         * @param [in] x0 x-axis of the plane
         * @param [in] y0 y-axis of the plane
         * @param [in] z0 z-axis of the plane
         * @param [in] plane three vectors for rotation around the origin
         * @return transformation matrix, that is equal the rotation multiplied by translation
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, IK::Vector_3 &x0, IK::Vector_3 &y0, IK::Vector_3 &z0);

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

        /**
         * Get a plane and its origin point, often used for orienting polylines from 3D to 2D
         *
         * @param [in] polyline Open or closed polygon
         * @param [out] origin origin of the plane
         * @param [out] x_axis x-axis of the plane computed from the plane: plane.base1()
         * @param [out] y_axis y-axis of the plane computed from the plane: plane.base2()
         * @param [out] z_axis z-axis of the plane computed from the plane: plane.orthogonal_vector()
         */
        void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis);

        /**
         * Get a plane and its origin point, often used for orienting polylines from 3D to 2D
         *
         * @param [in] polyline Open or closed polygon
         * @param [out] origin origin of the plane that corresponds to the longest edge start point
         * @param [out] x_axis x-axis of the plane that corresponds to the longest edge direction
         * @param [out] y_axis y-axis of the plane that corresponds to CGAL::cross_product(x_axis, z_axis)
         * @param [out] z_axis z-axis of the plane that corresponds to the average normal
         */
        void get_average_plane_axes_oriented_to_longest_edge(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis);

        /**
         * create point in a circle
         * the first four inputs are needed to orient the points in 3d
         * WARNING user must not use this method - this method is used by get_polylabel_circle_division_points method
         * @param [in] origin plane origin
         * @param [in] x_axis plane x axis
         * @param [in] y_axis plane y axis
         * @param [in] z_axis plane z axis
         * @param [out] points point divisions
         * @param [in] number_of_chunks circle divisions
         * @param [in] radius circle radius
         */
        void circle_points(const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis, std::vector<IK::Point_3> &points, int number_of_chunks = 4, double radius = 150);

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

        /**
         * transformation matrix, orient from xy plane to plane
         *
         * @param [in] origin plane oint
         * @param [in] x_axis x_axis, that is non-unitized
         * @param [in] y_axis y_axis, that is non-unitized
         * @param [in] z_axis z_axis, that is non-unitized
         * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
         */
        CGAL::Aff_transformation_3<IK> xy_to_plane(const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis);



        /**
         * transformation matrix, rotate geometry around a vector, the vector start at the origin (0,0,0)
         *
         * @param [in] angle angle in radians
         * @param [in] axis direction of rotation
         * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
         */
        CGAL::Aff_transformation_3<IK> axis_rotation(const double &angle, const IK::Vector_3 &axis);
    }

    /**
     * inscribe circle in a polygon (with or without holes)
     * the polyline is oriented to XY plane using the input plane
     * then oriented back to 3D
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * @param polylines planar polylines rotated in space
     * @param precision precision, the smaller the number the less precise the result, default 1.0
     * @return a tuple that represents the circle (center, plane, radius)
     */
    std::tuple<IK::Point_3, IK::Plane_3, double> get_polylabel(const std::vector<CGAL_Polyline> &polylines, double precision = 1.0);

    /**
     * get points inscribed in a polylable circle
     * @param [in] division_direction_in_3d division direction
     * @param [in] polylines list of polylines that representes the input polylines for the polylabel algorithm
     * @param [out] points division points
     * @param [in] division number of points
     * @param [in] scale scale of the circle
     * @param [in] precision tolerance for the polylabel algorithm
     * @param [in] orient_to_closest_edge if true, the points are oriented to the closest edge
     */
    void get_polylabel_circle_division_points(const IK::Vector_3 &division_direction_in_3d, const std::vector<CGAL_Polyline> &polylines, std::vector<IK::Point_3> &points, int division = 4, double scale = 0.75, double precision = 1.0, bool orient_to_closest_edge = true);

    /**
     * iscribe ractangle in a polygon and divide its edges into points or create rectangle grid
     * https://doc.cgal.org/latest/Inscribed_areas/index.html
     * also a good reference that could be implemented in a separate method: https://github.com/mo42/ler
     *
     * @param [in] polylines input polylines (with holes)
     * @param [out] result output rectangle inscribed between division points of the polygon
     * @param [out] points output division points hwne
     * @param [in] direction segment in 3D that represents the orientation of the rectangle
     * @param [in] scale the rectangle scale, since the algorithm works with points, it never returns the exact touching with the edges
     * @param [in] precision numeric parameters that subdivide more or less points of the polygon
     * @param [in] rectangle_division_distance when division distance is below 0, the rectangle grid is cretead, if above, the rectangle's edges are subdivided
     * @return bool flag if the result is valid
     */
    bool inscribe_rectangle_in_convex_polygon(const std::vector<CGAL_Polyline> &polylines, CGAL_Polyline &result, std::vector<IK::Point_3> &points, IK::Segment_3 &direction, double scale = 0, double precision = 1.0, double rectangle_division_distance = 10);

}

#endif