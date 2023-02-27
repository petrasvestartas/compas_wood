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
// The implementations is following the OpenNURBS library:
// PlanePlanePlane
// PlanePlane
// LinePlane
// LineLine not implemented
// https://github.com/mcneel/opennurbs/blob/c20e599d1ff8f08a55d3dddf5b39e37e8b5cac06/opennurbs_intersect.cpp
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CGAL_INTERSECTION_UTIL_H
#define CGAL_INTERSECTION_UTIL_H

namespace cgal_intersection_util
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
         * The transformation matrix from the plane to XY plane
         *
         * @param [in] origin point for translation from origin
         * @param [in] plane three vectors for rotation around the origin
         * @return transformation matrix, that is equal the rotation multiplied by translation
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);

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
         * Solve a 3x3 linear system using Gauss-Jordan elimination with full pivoting.
         * https://github.com/hansec/opennurbs/blob/a149d1172e1564c677fd7729e9eb33628ba191bd/opennurbs_math.cpp 2732
         * @param [in] row0 first row of 3x3 matrix
         * @param [in] row1 second row of 3x3 matrix
         * @param [in] row2 third row of 3x3 matrix
         * @param [in] d0
         * @param [in] d1
         * @param [in] d2 (d0,d1,d2) right hand column of system
         * @param [in] x_addr first unknown
         * @param [in] y_addr second unknown
         * @param [in] z_addr third unknown
         * @param [out] pivot_ratio if not NULL, the pivot ration is
                returned here.  If the pivot ratio is "small",
                then the matrix may be singular or ill
                conditioned. You should test the results
                before you use them.  "Small" depends on the
                precision of the input coefficients and the
                use of the solution.  If you can't figure out
                what "small" means in your case, then you
                must check the solution before you use it.
         * @return    The rank of the 3x3 matrix (0,1,2, or 3)
         * If ON_Solve3x3() is successful (returns 3), then
         * the solution is returned in
         * (*x_addr, *y_addr, *z_addr) and *pivot_ratio = min(|pivots|)/max(|pivots|).
         * If the return code is < 3, then (0,0,0) is returned as the "solution".
         */
        int solve3x3(const double (&row0)[3], const double (&row1)[3], const double (&row2)[3],
                     double d0, double d1, double d2,
                     double *x_addr, double *y_addr, double *z_addr,
                     double *pivot_ratio);

        double value_at(const IK::Plane_3 &Pl, const IK::Point_3 &P);
    }

    /**
     * Get the intersection of two segments (internally line-line intersection)
     * The lines are oriented from 3D plane to 2D and the result is oriented back to 3d
     * WARNING: the lines must not be parallel, this case is not handled
     *
     * @param [in] cutter_line first plane
     * @param [in] segment second plane
     * @param [out] output point output
     * @return true if line and a segment interescts
     */
    bool line_line_3d(const IK::Segment_3 &cutter_line, const IK::Segment_3 &segment, IK::Point_3 &output);

    /**
     * Get the intersection of two segments (internally line-line intersection)
     * WARNING: the planes must not be parallel, this case is checked internal
     *
     * @param [in] plane_0 first plane
     * @param [in] plane_1 second plane
     * @param [in] plane_2 third plane
     * @param [out] output the point output
     * @return true if the solve3x3 reutrs the rank 3
     */
    bool plane_plane_plane(const IK::Plane_3 &plane_0, const IK::Plane_3 &plane_1, const IK::Plane_3 &plane_2, IK::Point_3 &output);

    /**
     * Get the intersection of two planes (internally plane-plane-plane intersection)
     * The third plane is computed by the cross product of the first two
     * Since the line is infinite, the origin of the third plane is irrelevant
     * WARNING: the planes must not be parallel, this case is checked internal
     *
     * @param [in] plane_0 first plane
     * @param [in] plane_1 second plane
     * @param [out] output the line output
     * @return true if the solve3x3 reutrs the rank 3
     */
    bool plane_plane(const IK::Plane_3 &plane_0, const IK::Plane_3 &plane_1, IK::Line_3 &output);

    /**
     * Get the intersection of a line and a plane
     *
     * @param [in] line segment
     * @param [in] plane plane
     * @param [out] output the point output
     * @param [out] is_finite true if the line is finite, still the point will be outputed
     * @return true the intersection is sucsessful or point is outside the line, incase the finite search is used
     */
    bool line_plane(const IK::Segment_3 &line, const IK::Plane_3 &plane, IK::Point_3 &output, bool is_finite = false);

    /**
     * Get the intersection of a line and a plane
     *
     * @param [in] line infinite line
     * @param [in] plane plane
     * @param [out] output the point output
     * @param [out] is_finite true if the line is finite, still the point will be outputed
     * @return true the intersection is sucsessful or point is outside the line, incase the finite search is used
     */
    void line_plane(const IK::Line_3 &line, const IK::Plane_3 &plane, IK::Point_3 &output, bool is_finite = false);

    /**
     * Get the intersection of plane with four other planes
     * Two planes are given
     * Other two are contructed from a line segment
     *
     * @param [in] collision_face plnae of the timber plate side often
     * @param [in] line segment, from which the top and bottom planes are computed
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [out] output CGAL_Polyline of size 5, meaning the rectangle end point is duplicated
     * @return true the intersection is sucsessful all the four plane_plane_plane intersections were successful
     */
    bool get_quad_from_line_topbottomplanes(const IK::Plane_3 &collision_face, const IK::Segment_3 &line, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, CGAL_Polyline &output);

    /**
     * Scale vector by line and two plane intersection
     * WARNING: this method is very specific to the use case of plane_to_face mehtod in the wood_main.cpp
     *
     * @param [in] dir vector to intersect with the planes
     * @param [in] line segment, from which the top and bottom planes are computed
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [out] output vector whose length is scaled to the distance between the two planes
     * @return true, if the output_vector_length / plane_ortho_length < 10, then the vector is valid
     */
    bool scale_vector_to_distance_of_2planes(const IK::Vector_3 &dir, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, IK::Vector_3 &output);

    /**
     * Intersect plane_pair0_0 with plane_pair1_0 and plane_pair1_1 and return the orthogonal vector between the two lines
     * Two planes are given
     * Other two are contructed from a line segment
     *
     * @param [in] plane_pair0_0 first plane
     * @param [in] plane_pair1_0 second plane
     * @param [in] plane_pair1_1 third plane
     * @param [out] output vector
     * @return true, if the two plane intersections are successful, but the line paralleism is not checked
     */
    bool get_orthogonal_vector_between_two_plane_pairs(const IK::Plane_3 &plane_pair0_0, const IK::Plane_3 &plane_pair1_0, const IK::Plane_3 &plane_pair1_1, IK::Vector_3 &output);

    /**
     * Get a closed quad from four planes and the main_plane intersection
     *
     * @param [in] main_plane the plane that intersects the four planes
     * @param [in] sequence_of_planes C-type array of four planes
     * @param [out] output quad CGAL polyline
     * @return true, if the four plane intersections are successful
     */
    bool plane_4planes(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output);

    /**
     * Get a closed quad from four planes and the main_plane intersection, the outline is open, the last point is not the first point
     *
     * @param [in] main_plane
     * @param [in] sequence_of_planes C-type array of four planes
     * @param [out] output quad CGAL polyline
     * @return true, if the four plane intersections are successful
     */
    bool plane_4planes_open(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output);

    /**
     * Get a closed quad from four planes and the main_plane intersection, the outline is open, the last point is not the first point
     *
     * @param [in] main_plane the plane that intersect the four lines
     * @param [in] l0 the first line segment
     * @param [in] l1 the second line segment
     * @param [in] l2 the third line segment
     * @param [in] l3 the fourth line segment
     * @param [out] output quad CGAL polyline
     * @return true, if the four plane and line intersections are successful
     */
    bool plane_4lines(IK::Plane_3 &plane, IK::Segment_3 &l0, IK::Segment_3 &l1, IK::Segment_3 &l2, IK::Segment_3 &l3, CGAL_Polyline &output);

    /**
     * Get the intersection of two segments (internally line-line intersection)
     * Plane normals are checked for parallelism tolerance 0.1
     *
     * @param [in] plane_0 first plane
     * @param [in] plane_1 second plane
     * @param [in] plane_2 third plane
     * @param [out] output the point output
     * @return true if the solve3x3 reutrs the rank 3 and planes are not parallel
     */
    bool plane_plane_plane_with_parallel_check(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, const IK::Plane_3 &plane2, IK::Point_3 &output);

    /**
     * Adjust the line segment by interescting with two planes
     *
     * @param [out] line planar polyline
     * @param [in] plane0 plane origin, as the first point of the polyline
     * @param [in] plane1 plane whose normal is the average of the cross-products of the segments
     */
    bool line_two_planes(IK::Segment_3 &line, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1);

    /**
     * Get the intersection of polyline and plane as a line segment
     * WARNING: this method is used imprecise CGAL - boost method, it is not recommended to use it, it is probably causing non-planarity in the merge function
     *
     * @param [in] polyline CGAl_Polyline
     * @param [in] plane The plane that intersect the polylin
     * @param [in] alignment_segment Align intersection points according to an average polygon edge direction
     * @param [out] output the segment output
     * @return true if two points are returned from the intersection
     */
    bool polyline_plane_to_line(const CGAL_Polyline &polyline, const IK::Plane_3 &plane, IK::Segment_3 &alignment_segment, IK::Segment_3 &output);

    /**
     * Intersect all polyline's segments with a plane
     *
     * @param [in] polyline vector of points
     * @param [in] plane plane that is intersecting the polyline
     * @param [out] points intersection points
     * @param [out] edge_ids intersection edge ids
     */
    bool polyline_plane(const CGAL_Polyline &polyline, const IK::Plane_3 &plane, std::vector<IK::Point_3> &points, std::vector<int> &edge_ids);

    /**
     * Intersect two polyline with two planes
     * This method is used for cross joint generation
     *
     * @param [in] c0 vector of points
     * @param [in] c1 plane that is intersecting the polyline
     * @param [in] p0 intersection points
     * @param [in] p1 intersection edge ids
     * @param [out] line intersection line ids common to the two intersection
     * @param [out] pair intersection edge ids
     */
    bool polyline_plane_cross_joint(CGAL_Polyline &c0, CGAL_Polyline &c1, IK::Plane_3 &p0, IK::Plane_3 &p1, IK::Segment_3 &line, std::pair<int, int> &pair);
}

#endif // CGAL_INTERSECTION_UTIL_H