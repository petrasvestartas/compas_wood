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

#ifndef CGAL_VECTOR_UTIL_H
#define CGAL_VECTOR_UTIL_H

namespace cgal_vector_util
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
     * Get the normal of a plate polyline
     *
     * @param [in] p polyline
     * @param [out] average_normal vector of the average normal of the polyline
     */
    void average_normal(const CGAL_Polyline &p, IK::Vector_3 &average_normal);

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

    /**
     * Interpolate two points
     *
     * @param [in] from start point
     * @param [in] to end point
     * @param [in] steps divisions
     * @param [in] include_ends end points
     * @param [out] interpolated_points divided points
     */
    void interpolate_points(const IK::Point_3 &from, const IK::Point_3 &to, const int &steps, std::vector<IK::Point_3> &points, int type_0_noends_1_end_2_oneend = 0);

    /**
     * Scale the vector to the given vertical height
     * Often used for CNC machining
     *
     * @param [in] vector vector to scale
     * @param [in] vertical_height vertical height of the vector
     * @return scaled vector, whose vertical component length is equal to the vertical_height
     */
    IK::Vector_3 get_leveled_vector(const IK::Vector_3 &vector, const double &vertical_height);
}

#endif