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

#ifndef CGAL_POLYLINE_UTIL_H
#define CGAL_POLYLINE_UTIL_H

namespace cgal_polyline_util
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
         * Get the normal of a plate polyline
         *
         * @param [in] p polyline
         * @param [out] average_normal vector of the average normal of the polyline
         */
        void average_normal(const CGAL_Polyline &p, IK::Vector_3 &average_normal);

        /**
         * The transformation matrix from the plane to XY plane
         *
         * @param [in] origin point for translation from origin
         * @param [in] plane three vectors for rotation around the origin
         * @return transformation matrix, that is equal the rotation multiplied by translation
         */
        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane);
    }

    /**
     * Shift closed polyline n-times to the right
     *
     * @param [in, out] pline The polyline to modify
     * @param [in] times How many times the closed polylines is rotated
     */
    void shift(CGAL_Polyline &pline, const int &times);

    /**
     * Get length of a polyline
     *
     * @param [in] pline The polyline to measure
     * @return length of the polyline
     */
    double polyline_length(CGAL_Polyline &pline);

    /**
     * Get square length of a polyline
     *
     * @param [in] pline The polyline to measure
     * @return square length of the polyline
     */
    double polyline_length_squared(CGAL_Polyline &pline);

    /**
     * Get point on a segment using parameter "t"
     *
     * @param [in] line segment to process
     * @param [in] t parameter on the line, when line interval is considered from 0 to 1
     * @return point on the segment
     */
    IK::Point_3 point_at(const IK::Segment_3 &line, const double &t);

    /**
     * Get closest parameter on the line segment by point
     * Works with degenerate lines too
     *
     * @param [in] point point to search from
     * @param [in] line line segment
     * @param [out] t parameter on the line, when line interval is considered from 0 to 1
     */
    void closest_point_to(const IK::Point_3 &point, const IK::Segment_3 &line, double &t);

    /**
     * Get closest parameter on the line segment by point
     * |||||0------------1||||||||||||||||||||||||
     * ||||||||||||2-----------3||||||||||||||||||
     * RESULT||||||-------||||||||||||||||||||||||
     * ERROR: refactoring change, be sure to check the comment "IF ERROR CHANGE TO THIS"
     * WARNING: this method work if the lines are overlapping
     *
     * @param [in] line0 segment to project to
     * @param [in] line1 segment that is projected
     * @param [out] output overlap between segments
     * @return true, if there is an overlap by doing the projection
     */
    bool line_line_overlap(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output);
    /**
     * Get projection of polyline onto the line
     *
     * @param [in] line segment to project to
     * @param [in] points CGAL_Polyline (a vector of points)
     * @param [out] output the projected segment
     * @return true if the projection is not a point, but a line
     */
    bool line_from_projected_points(const IK::Segment_3 &line, const CGAL_Polyline &points, IK::Segment_3 &output);

    /**
     * Get average line by averaging the lines' end points
     *
     * @param [in] line0 first line
     * @param [in] line1 second line
     * @param [out] output average line
     */
    void line_line_average(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output);

    /**
     * Get average line of two line projections
     *
     * @param [in] line0 first line
     * @param [in] line1 second line
     * @param [out] output projected average line
     */
    void line_line_overlap_average(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output);

    /**
     * Get average line of two line projections
     *
     * @param [in] line0 first polyline of two points
     * @param [in] line1 second polyline of two points
     * @param [out] output projected average line
     */
    void line_line_overlap_average(const CGAL_Polyline &line0, const CGAL_Polyline &line1, IK::Segment_3 &output);

    /**
     * Get get closest distance to a polyline and return the closest edge index
     *
     * @param [in] point point point to search from
     * @param [in] polyline polyline to check the segments against
     * @param [out] edge_id closest edge
     * @param [out] closest_point closest point
     * @return closest distance
     */
    double closest_distance_and_point(const IK::Point_3 &point, const CGAL_Polyline &polyline, size_t &edge_id, IK::Point_3 &closest_point);

    /**
     * Check if the polyline is closed by measuring its end points
     *
     * @param [in] polyline polylines whose start and end points' distance is measured
     * @return true if the points are far from each other within wood_globals::DISTANCE_SQUARED
     */
    bool is_closed(const CGAL_Polyline &polyline);

    /**
     * Get the center of the polyline by averaging the points
     *
     * @param [in] polyline vector of points
     * @return the average point
     */
    IK::Point_3 center(const CGAL_Polyline &polyline);

    /**
     * Get the center of the polyline by averaging the points
     *
     * @param [in] polyline vector of points
     * @return the average point as a vector type
     */
    IK::Vector_3 center_vec(const CGAL_Polyline &polyline);

    /**
     * Transform polyline, by applying the transformation matrix to each point
     *
     * @param [in] polyline vector of points
     * @param [in] transform CGAl 4_4 transformation matrix
     */
    void transform(CGAL_Polyline &polyline, const CGAL::Aff_transformation_3<IK> &transform);

    /**
     * Get the average plane of the polyline, by averaging the normals and vertices of the segments
     *
     * @param [in] polyline vector of points
     * @param [in] plane_axes center point and axes of the plane
     */
    void get_average_plane(CGAL_Polyline &polyline, IK::Vector_3 (&plane_axes)[4]);

    /**
     * Get a plane of the polyline, by averaging the normals of the segments
     *
     * @param [in] polyline planar polyline
     * @param [out] origin plane origin, as the first point of the polyline
     * @param [out] plane plane whose normal is the average of the cross-products of the segments
     */
    void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Plane_3 &plane);

    /**
     * Construct line by averaging the end points of the two lines
     * WARNING: be aware that the lines might be flipped
     *
     * @param [in] line0 vector of points
     * @param [in] line1 plane that is intersecting the polyline
     * @param [in] output middle line, the cross might occur
     */
    void get_middle_line(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output);

    /**
     * Extend a line by a distance
     *
     * @param [in, out] line segment to extend
     * @param [in] distance0 start of the line
     * @param [in] distance1 end of the line
     */
    void extend_line(IK::Segment_3 &line, const double &distance0, const double &distance1);

    /**
     * Scale a line by distance in both sides
     *
     * @param [in, out] line segment to extend
     * @param [in] distance start of the line
     */
    void scale_line(IK::Segment_3 &line, const double &distance);

    /**
     * Extend a polyline's segment ends by distance [0.0 to ...] or scale [0 to 1]
     *
     * @param [in, out] pline polyline to extend
     * @param [in] sID segment ID to extend
     * @param [in] dist0 start extension by distance
     * @param [in] dist1 end extension by distance
     * @param [in] proportion0 start extension by scale
     * @param [in] proportion1 end extension by scale
     */
    void extend(CGAL_Polyline &pline, int sID, double dist0, double dist1, double proportion0 = 0, double proportion1 = 0);

    /**
     * Extend a segment both ends by distance or scale
     *
     * @param [in] segment polyline to extend
     * @param [in] dist extension by distance
     * @param [in] proportion extension by scale
     */
    void extend_equally(IK::Segment_3 &segment, double dist = 0, double proportion = 0);

    /**
     * Extend a polyline's segment both ends by distance or scale
     *
     * @param [in, out] pline polyline either closed or open
     * @param [in] segment_id polyline edge id
     * @param [in] dist extension by distance
     * @param [in] proportion extension by scale
     */
    void extend_equally(CGAL_Polyline &pline, int segment_id, double dist = 0, double proportion = 0);

    /**
     * Move the polyline by direction
     *
     * @param [in, out] polyline polygon to move
     * @param [in] direction vector to move the polyline
     */
    void move(CGAL_Polyline &polyline, const IK::Vector_3 &direction);

    /**
     * Get the orientation of the 3D polyline
     * The polyline is oriented from 3D to 2D and then the winding is checked
     *
     * @param [in] polyline polygon to move
     * @param [in] plane orientation plane from 3D to 2D using plane_to_xy() method
     * @return true if clockwise
     */
    bool is_clockwise(CGAL_Polyline &polyline, IK::Plane_3 plane);

    /**
     * Reverse the polyline
     *
     * @param [in] polyline polygon to reverse
     */
    void flip(CGAL_Polyline &polyline);

    /**
     * Check which points of the polyline are convex (true) or concave (false)
     *
     * @param [in] polyline polygon to to check the courers
     * @param [out] convex_or_concave vector of boolean flags, that are true if the conrer is convex, for closed polyline the last point
     */
    void get_convex_corners(const CGAL_Polyline &polyline, std::vector<bool> &convex_or_concave);

    /**
     * Interpolate two polylines
     *
     * @param [in] polyline0 first polyline to interpolate
     * @param [in] polyline1 second polyline to interpolate
     * @param [out] weight value for the linear interpolation
     */
    CGAL_Polyline tween_two_polylines(const CGAL_Polyline &polyline0, const CGAL_Polyline &polyline1, const double &weight);

}

#endif