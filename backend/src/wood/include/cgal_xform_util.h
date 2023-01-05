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
// OpenNurbs helped a lot to implement different transformation methods absent in CGAL, more info: https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_XFORM_UTIL_H
#define CGAL_XFORM_UTIL_H



namespace cgal_xform_util
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

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Change of basis transformation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /**
     * transformation matrix, orient from one coordinate system to another
     * imagine box-to-box morphing
     * https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp //Line 2153
     *
     * @param [in] origin_1 target plane origin
     * @param [in] x_axis_1 target plane x_axis
     * @param [in] y_axis_1 target plane y_axis
     * @param [in] z_axis_1 target plane z_axis
     * @param [in] origin_0 source plane origin
     * @param [in] x_axis_0 source plane x_axis
     * @param [in] y_axis_0 source plane y_axis
     * @param [in] z_axis_0 source plane z_axis
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> change_basis(
        const IK::Vector_3 &origin_1, const IK::Vector_3 &x_axis_1, const IK::Vector_3 &y_axis_1, const IK::Vector_3 &z_axis_1,
        const IK::Vector_3 &origin_0, const IK::Vector_3 &x_axis_0, const IK::Vector_3 &y_axis_0, const IK::Vector_3 &z_axis_0);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Plane transformation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * transformation matrix, orient from 3D plane to 3D plane
     * https://stackoverflow.com/questions/23270292/a-point-cgal-transformation-between-two-coordinate-systems
     * https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp Line1960
     *
     * @param [in] origin_0 source plane origin
     * @param [in] x_axis_0 source plane x_axis
     * @param [in] y_axis_0 source plane y_axis
     * @param [in] z_axis_0 source plane z_axis
     * @param [in] origin_1 target plane origin
     * @param [in] x_axis_1 target plane x_axis
     * @param [in] y_axis_1 target plane y_axis
     * @param [in] z_axis_1 target plane z_axis
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> plane_to_plane(
        const IK::Vector_3 &origin_0, const IK::Vector_3 &x_axis_0, const IK::Vector_3 &y_axis_0, const IK::Vector_3 &z_axis_0,
        const IK::Vector_3 &origin_1, const IK::Vector_3 &x_axis_1, const IK::Vector_3 &y_axis_1, const IK::Vector_3 &z_axis_1);

    /**
     * transformation matrix, orient from plane to xy plane
     *
     * @param [in] origin plane origin
     * @param [in] plane plane, from which x-axis and y-axis and z-axis are extracted
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);

    /**
     * transformation matrix, orient from plane to xy plane
     *
     * @param [in] origin plane oint
     * @param [in] x_axis x_axis, that is non-unitized
     * @param [in] y_axis y_axis, that is non-unitized
     * @param [in] z_axis z_axis, that is non-unitized
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> plane_to_xy(
        const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis);

    /**
     * transformation matrix, orient from xy plane to plane
     *
     * @param [in] origin plane origin
     * @param [in] plane plane, from which x-axis and y-axis and z-axis are extracted
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> xy_to_plane(const IK::Vector_3 &origin, const IK::Plane_3 &plane);

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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Scale
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * transformation matrix, scale geometry by around origin
     *
     * @param [in] origin center point
     * @param [in] scale_x scale in x_axis
     * @param [in] scale_y scale in y_axis
     * @param [in] scale_z scale in z_axis
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> scale(const double &scale_x, const double &scale_y, const double &scale_z);

    /**
     * transformation matrix, scale geometry by around a point
     *
     * @param [in] origin center point
     * @param [in] scale_value magnitude of scale from 0 to n
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> scale(const IK::Point_3 &origin, const double &scale_value);

    /**
     * transformation matrix, scale geometry by around a point
     *
     * @param [in] origin center point
     * @param [in] scale_x scale in x_axis
     * @param [in] scale_y scale in y_axis
     * @param [in] scale_z scale in z_axis
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> scale(const IK::Point_3 &origin, const double &scale_x, const double &scale_y, const double &scale_z);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Axis rotation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
     * transformation matrix, rotate geometry around a vector, the vector start at the origin (0,0,0)
     *
     * @param [in] angle angle in radians
     * @param [in] axis direction of rotation
     * @return CGAL 4x4 transformation matrix https://doc.cgal.org/latest/Kernel_23/classCGAL_1_1Aff__transformation__3.html
     */
    CGAL::Aff_transformation_3<IK> axis_rotation(const double &angle, const IK::Vector_3 &axis);
}

#endif // CGAL_AFF_TRANSFORMATION_3_H