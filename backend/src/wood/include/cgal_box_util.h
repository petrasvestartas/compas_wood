
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
// Separating axis theorem:
// https://stackoverflow.com/questions/47866571/simple-oriented-bounding-box-obb-collision-detection-explaining
// https://github.com/alxschwrz/Separating-Axis-Theorem
// https://github.com/inzombiak/SATTest
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_BOX_UTIL_H
#define CGAL_BOX_UTIL_H

namespace cgal_box_util
{

    namespace internal
    {
        /**
         * Get a point on a plane,
         *
         * @param [in] plane Plane made as a C-style array of length 4: origin and x,y,z vectors
         * @param [in] x_axis The first coordinate of the vector
         * @param [in] y_axis The second coordinate of the vector
         * @param [in] z_axis The third coordinate of the vector
         * @return Vector that is a point on a plane
         */
        IK::Vector_3 vector_at(const IK::Vector_3 (&plane)[4], const double &x_axis, const double &y_axis, const double &z_axis);

        /**
         * Get a point on a box
         * The same as "Get a point on a plane"
         *
         * @param [in] box Plane made as a C-style array of length 5: origin and x,y,z vectors, and one vector for box size
         * @param [in] x_axis The first coordinate of the vector
         * @param [in] y_axis The second coordinate of the vector
         * @param [in] z_axis The third coordinate of the vector
         * @return Vector that is a point on a plane
         */
        IK::Point_3 point_at(const IK::Vector_3 (&box)[5], const double &x_axis, const double &y_axis, const double &z_axis);

        /**
         * Get box corners
         *
         * @param [in] box Plane made as a C-style array of length 5: origin and x,y,z vectors, and one vector for box size, each vector is the length of the box edge lengths
         * @param [out] corners Eight box corners
         */
        void get_corners(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners);

        /**
         * Get two rectangles of the box
         * Each rectangle has 5 points, total 10 points as closed polylines
         *
         * @param [in] box Plane made as a C-style array of length 5: origin and x,y,z vectors, and one vector for box size, each vector is the length of the box edge lengths
         * @param [out] corners Ten box corners
         */
        void get_two_box_rectangles(const IK::Vector_3 (&box)[5], CGAL_Polyline &corners);

        /**
         * Get corners of the CGAL box
         *
         * @param [in] box CGAL box
         * @param [out] corners Eight box corners
         */
        void get_corners_box(const CGAL::Bbox_3& box, CGAL_Polyline &corners);

        /**
         * Get two rectangles of the CGAL box
         *
         * @param [in] box CGAL box
         * @param [out] corners Ten box corners
         */
        void get_two_box_rectangles(const CGAL::Bbox_3& box, CGAL_Polyline &corners);

        /**
         * Check if there's a separating plane in between the selected axes
         * https://stackoverflow.com/questions/47866571/simple-oriented-bounding-box-obb-collision-detection-explaining
         * https://github.com/alxschwrz/Separating-Axis-Theorem
         * https://github.com/inzombiak/SATTest
         *
         * @param [in] relative_position difference between box origins
         * @param [in] axis The axis to check against
         * @param [in] box1 First box as a 5 vector C array
         * @param [in] box2 Second box as a 5 vector C array
         * @return return true If we find an axis that separates the both polygons, then the polygons cannot overlap! This works only for convex polygons!
         */
        bool get_separating_plane(const IK::Vector_3 &relative_position, const IK::Vector_3 &axis, const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5]);

        ///////////////////////////////////////////////////////////////////////////////////////////////
        // Native Box Collision
        ///////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * Minimal vector implementation with operators
         *
         */
        struct vec3
        {
            double x, y, z;
            vec3 operator-(const vec3 &rhs) const { return {x - rhs.x, y - rhs.y, z - rhs.z}; }
            double operator*(const vec3 &rhs) const { return (x * rhs.x + y * rhs.y + z * rhs.z); }                                 // DOT PRODUCT
            vec3 operator^(const vec3 &rhs) const { return {y * rhs.z - z * rhs.y, z * rhs.x - x * rhs.z, x * rhs.y - y * rhs.x}; } // CROSS PRODUCT
            vec3 operator*(const double &rhs) const { return {x * rhs, y * rhs, z * rhs}; }
        };

        /**
         * Oriented bounding box
         */
        struct obb
        {
            vec3 Pos, AxisX, AxisY, AxisZ, Half_size;
        };

        /**
         * Check if there's a separating plane in between the selected axes
         * https://stackoverflow.com/questions/47866571/simple-oriented-bounding-box-obb-collision-detection-explaining
         * https://github.com/alxschwrz/Separating-Axis-Theorem
         * https://github.com/inzombiak/SATTest
         *
         * @param [in] relative_position difference between box origins
         * @param [in] axis The axis to check against
         * @param [in] box1 First box as a 5 vector custom struct "vec3"
         * @param [in] box2 Second box as a 5 vector custom struct "vec3"
         * @return return true If we find an axis that separates the both polygons, then the polygons cannot overlap! This works only for convex polygons!
         */
        bool get_separating_plane(const vec3 &relative_position, const vec3 &axis, const obb &box1, const obb &box2);

        /**
         * Test for separating planes in all 15 axes
         * https://stackoverflow.com/questions/47866571/simple-oriented-bounding-box-obb-collision-detection-explaining
         * https://github.com/alxschwrz/Separating-Axis-Theorem
         * https://github.com/inzombiak/SATTest
         *
         * @param [in] box1 First box as a 5 vector C array
         * @param [in] box2 Second box as a 5 vector C array
         * @return return true if there is a collision in at least one of the projections
         */
        bool get_collision(const obb &box1, const obb &box2);

        /**
         * Test for separating planes in all 15 axes
         * https://stackoverflow.com/questions/47866571/simple-oriented-bounding-box-obb-collision-detection-explaining
         * https://github.com/alxschwrz/Separating-Axis-Theorem
         * https://github.com/inzombiak/SATTest
         *
         * @param [in] box1 First box as a 5 vector C array
         * @param [in] box2 Second box as a 5 vector C array
         * @return return true if there is a collision in at least one of the projections
         */
        bool get_collision_native(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5]);
    }

    /**
     * transform a plane as a vector array
     *
     * @param plane c array that is composed of 4 IK::Vector_3
     * @param xform CGAL 4_4 transformation matrix
     */
    void transform_plane_as_vector_array(IK::Vector_3 *plane, const CGAL::Aff_transformation_3<IK> &xform);

    /**
     * assign c style array for boxes
     * 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     *
     * @param source 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     * @param target 5 vectors that represents a box: origin, x, y, z, size (x,y,z)
     */
    void assign(const IK::Vector_3 *source, IK::Vector_3 *target, const int &n);

    /**
     * check if two boxes are colliding
     *
     * @param box1 5 vectors that represents a box: origin, x, y, z, half size (x,y,z)
     * @param box2 5 vectors that represents a box: origin, x, y, z, half size (x,y,z)
     * @return bool value, if boxes are colliding
     */
    bool get_collision(const IK::Vector_3 (&box1)[5], const IK::Vector_3 (&box2)[5]);
}

#endif
