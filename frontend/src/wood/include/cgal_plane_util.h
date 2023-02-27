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

#ifndef CGAL_PLANE_UTIL_H
#define CGAL_PLANE_UTIL_H

namespace cgal_plane_util
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
    }

    /**
     * Move a plane by a distance along its normal
     *
     * @param [in] plane plane whose origin is move by the unitized normal
     * @param [in] distance distance to move the plane origin
     * @return moved plane
     */
    IK::Plane_3 translate_by_normal(const IK::Plane_3 &plane, const double &distance);

    /**
     * Check if two planes have the same or flipped normal
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [in] can_be_flipped bool flag to indicate if the normal can be flipped
     * @return true if two planes ponting to the same or flipped normal
     */
    bool is_same_direction(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, bool can_be_flipped = true);

    /**
     * Check if two planes are in the same position
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @return true origins are very close to each other
     */
    bool is_same_position(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1);

    /**
     * Check if two planes have the same or flipped normal and share the same origin
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [in] can_be_flipped bool flag to indicate if the normal can be flipped
     * @return true if two planes are pointing to the same or flipped normal and share the same origin
     */
    bool is_coplanar(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, bool can_be_flipped = true);

    /**
     * Check if two vectors are parallel or anti-parallel or not-paralel
     * tolerance wood_globals::ANGLE
     *
     * @param [in] v0 first vector
     * @param [in] v1 second vector
     * @return 1: this and other vectors are and parallel
     * -1: this and other vectors are anti-parallel
     * 0: this and other vectors are not parallel or at least one of the vectors is zero
     */
    int is_parallel_to(const IK::Vector_3 &v0, const IK::Vector_3 &v1);

}

#endif // CGAL_PLANE_UTIL_H