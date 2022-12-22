#pragma once

namespace cgal_rectangle_util
{

    namespace internal
    {
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

        inline float ccw(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &c)
        {
            return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
        }

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Returns true if a is lexicographically before b.
         *
         * @param a point
         * @param b point
         * @return bool value
         */
        inline bool is_left_of(const IK::Point_3 &a, const IK::Point_3 &b)
        {
            return (a.x() < b.x() || (a.x() == b.x() && a.y() < b.y()));
        }

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
        inline float len(const IK::Point_3 &a, const IK::Point_3 &b)
        {
            return std::sqrt((b.x() - a.x()) * (b.x() - a.x()) + (b.y() - a.y()) * (b.y() - a.y()));
        }

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * The unsigned distance of p from segment (a, b).
         * @param a point
         * @param b point
         * @return float value represented the distance
         */
        inline float dist(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &p)
        {
            return std::fabs((b.x() - a.x()) * (a.y() - p.y()) - (b.y() - a.y()) * (a.x() - p.x())) / len(a, b);
        }

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Returns the index of the farthest point from segment (a, b).
         * @param a point
         * @param b point
         * @param v points to search from
         * @return size_t index of the farthest point
         */
        inline size_t get_farthest(const IK::Point_3 &a, const IK::Point_3 &b, const std::vector<IK::Point_3> &v)
        {
            size_t idxMax = 0;
            float distMax = dist(a, b, v[idxMax]);

            for (size_t i = 1; i < v.size(); ++i)
            {
                float distCurr = dist(a, b, v[i]);
                if (distCurr > distMax)
                {
                    idxMax = i;
                    distMax = distCurr;
                }
            }

            return idxMax;
        }

        /**
         * WARNING user must not use this method - this method is used by quick_hull algorithm
         * Recursive call of the quick_hull algorithm.
         * This function is used by the main function below
         * @param [in] v point
         * @param [in] a point
         * @param [in] b point
         * @param [out] hull points
         */
        inline void quick_hull(const std::vector<IK::Point_3> &v, const IK::Point_3 &a, const IK::Point_3 &b, std::vector<IK::Point_3> &hull)
        {
            if (v.empty())
                return;

            IK::Point_3 f = v[get_farthest(a, b, v)];

            // Collect points to the left of segment (a, f)
            std::vector<IK::Point_3> left;
            for (auto p : v)
            {
                if (ccw(a, f, p) > 0)
                {
                    left.emplace_back(p);
                }
            }
            quick_hull(left, a, f, hull);

            // Add f to the hull
            hull.emplace_back(f);

            // Collect points to the left of segment (f, b)
            std::vector<IK::Point_3> right;
            for (auto p : v)
            {
                if (ccw(f, b, p) > 0)
                {
                    right.emplace_back(p);
                }
            }
            quick_hull(right, f, b, hull);
        }

        /**
         * WARNING user must not use this method - this method is used by bounding_rectangle algorithm
         * get angle of an edge to its projects to the x-axis, this is the helper fuction of bounding_rectangle
         *
         * @param edge polyline segment
         * @return angle
         */
        inline double angle_to_xaxis(IK::Segment_3 &edge)
        {

            auto delta = edge[0] - edge[1];
            return -std::atan(delta.hy() / delta.hx());
        }

        /**
         * WARNING user must not use this method - this method is used by bounding_rectangle algorithm
         * get edge after rotation to x axis, this is the helper fuction of bounding_rectangle
         *
         * @param vector segment direction
         * @param angle angle in radians
         * @return rotated vector
         */
        inline IK::Point_3 rotate_to_xaxis(IK::Point_3 &point, const double &angle)
        {
            return IK::Point_3(point.hx() * std::cos(angle) - point.hy() * std::sin(angle), point.hx() * std::sin(angle) + point.hy() * std::cos(angle), 0);
        }
    }

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
    inline bool orient_polyline_to_xy_and_back(CGAL_Polyline &polyline, CGAL::Aff_transformation_3<IK> &xform_to_xy, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
    {

        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline.size() < 3)
            return false;

        /////////////////////////////////////////////////////////////////////////////////////
        // Get center polyline plane
        /////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 center;
        IK::Plane_3 plane;
        cgal_polyline_util::get_fast_plane(polyline, center, plane);

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation and assign outputs
        /////////////////////////////////////////////////////////////////////////////////////
        xform_to_xy = cgal_xform_util::PlaneToXY(polyline[0], plane);
        xform_to_xy_inv = xform_to_xy.inverse();
        return true;
    }

    /**
     * quick_hull algorithm
     * https://en.wikipedia.org/wiki/quick_hull
     * https://github.com/MiguelVieira/ConvexHull2D/blob/master/ConvexHull.cpp
     *
     * @param [in, out] v input polylines or a list of points
     * @return points that re
     */
    inline CGAL_Polyline quick_hull(const CGAL_Polyline &v)
    {
        std::vector<IK::Point_3> hull;

        // Start with the leftmost and rightmost points.
        IK::Point_3 a = *min_element(v.begin(), v.end(), internal::is_left_of);
        IK::Point_3 b = *max_element(v.begin(), v.end(), internal::is_left_of);

        // Split the points on either side of segment (a, b)
        std::vector<IK::Point_3> left, right;
        for (auto p : v)
        {
            internal::ccw(a, b, p) > 0 ? left.emplace_back(p) : right.emplace_back(p);
        }

        // Be careful to add points to the hull
        // in the correct order. Add our leftmost point.
        hull.emplace_back(a);

        // Add hull points from the left (top)
        internal::quick_hull(left, a, b, hull);

        // Add our rightmost point
        hull.emplace_back(b);

        // Add hull points from the right (bottom)
        internal::quick_hull(right, b, a, hull);

        return hull;
    }

    /**
     * get bounding rectangle of a polygon
     * the polygon can be rotated in 3d, because the function orients the polygon to XY plan
     * port from the C# version: https://github.com/cansik/LongLiveTheSquare/blob/master/U4LongLiveTheSquare/MinimalBoundingBox.cs
     *
     * EXAMPLE:
     * 	CGAL_Polyline polygon{
     *        IK::Point_3(0.766806731645467, -18.3800901708968, 0),
     *        IK::Point_3(-31.4979699097944, 1.40597494719874, 0),
     *        IK::Point_3(-20.255887456331, 43.1141008495478, 0),
     *        IK::Point_3(-13.9603212823915, 2.53018319254508, 0),
     *        IK::Point_3(13.3579390795244, -1.17970401709784, 0),
     *        IK::Point_3(37.0787330563321, 34.457697360381, 0),
     *        IK::Point_3(53.4921734383886, -14.33294048765, 0),
     *        IK::Point_3(0.766806731645467, -18.3800901708968, 0),
     *    };
     *
     *    CGAL_Polyline rectangle;
     *    cgal_rectangle_util::bounding_rectangle(polygon, rectangle);
     *    for (auto &point : rectangle)
     *        std::cout << point.hx() << " " << point.hy() << " " << point.hz() << "\n";
     *
     *
     * @param [in] polygon input polyline
     * @param [out] result output rectangle
     * @return bool flag if the result is valid
     */
    inline bool bounding_rectangle(CGAL_Polyline &polygon, CGAL_Polyline &result)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polygon to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy, xform_to_xy_inv;
        orient_polyline_to_xy_and_back(polygon, xform_to_xy, xform_to_xy_inv);
        CGAL_Polyline polygon_copy = polygon;
        cgal_polyline_util::Transform(polygon_copy, xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // calculate the convex hull
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto hull_points = quick_hull(polygon_copy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // check if no bounding box available
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (hull_points.size() <= 2)
            return false;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // foreach edge of the convex hull
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_2 p0(0, 0);
        IK::Point_2 p1(0, 0);
        CGAL::Iso_rectangle_2<IK> rectangle(p0, p1);
        double min_angle = 0;

        for (int i = 0; i < hull_points.size(); i++)
        {

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get polyline index
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto next_index = i + 1;
            auto current = hull_points[i];
            auto next = hull_points[next_index % hull_points.size()];
            auto segment = IK::Segment_3(current, next);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // min / max points
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            double top = -std::numeric_limits<double>::max();
            double bottom = std::numeric_limits<double>::max();
            double left = std::numeric_limits<double>::max();
            double right = -std::numeric_limits<double>::max();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get angle of segment to x axis
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto angle = internal::angle_to_xaxis(segment);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // rotate every point and get min and max values for each direction
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            for (auto &p : hull_points)
            {
                auto rotated_point = internal::rotate_to_xaxis(p, angle);

                top = std::max(top, rotated_point.hy());
                bottom = std::min(bottom, rotated_point.hy());
                left = std::min(left, rotated_point.hx());
                right = std::max(right, rotated_point.hx());
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // create a rectangle with the min and max values
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto rectangle_temp = CGAL::Iso_rectangle_2<IK>(IK::Point_2(left, bottom), IK::Point_2(right, top));

            // check if this is the first rectangle or if the current one is smaller
            if (rectangle.is_degenerate() || rectangle_temp.area() < rectangle.area())
            {
                rectangle = rectangle_temp;
                min_angle = angle;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert to CGAL polyline and rotate to axis
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        result = {
            IK::Point_3(rectangle.min().hx(), rectangle.min().hy(), 0),
            IK::Point_3(rectangle.min().hx(), rectangle.max().hy(), 0),
            IK::Point_3(rectangle.max().hx(), rectangle.max().hy(), 0),
            IK::Point_3(rectangle.max().hx(), rectangle.min().hy(), 0),
        };

        // rotate axis algined box back
        for (auto &p : result)
        {
            p = internal::rotate_to_xaxis(p, -min_angle);
            p = p.transform(xform_to_xy_inv);
        }
        result.emplace_back(result.front());

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient back to 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        return true;
    }

    /**
     * @brief Calculate a grid of points inside a polygon by a) convex hull, b) its bounding box, c) grid of points inside the polygon
     *
     * @param [in] polygon input polyline
     * @param [in] division_distance distance between points, if 0 the distance is calculated automatically
     * @param [in] max_points number of points
     * @param [out] points output points
     * @return bool flag if the result is valid
     */
    inline bool grid_of_points_in_a_polygon(CGAL_Polyline &polygon, const double &division_distance, const int &max_points, std::vector<IK::Point_3> &points) // const double &divisions,
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polygon to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy, xform_to_xy_inv;
        orient_polyline_to_xy_and_back(polygon, xform_to_xy, xform_to_xy_inv);
        CGAL_Polyline polygon_copy = polygon;
        cgal_polyline_util::Transform(polygon_copy, xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // calculate the convex hull
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto hull_points = quick_hull(polygon_copy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // check if no bounding box available
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (hull_points.size() <= 2)
            return false;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // foreach edge of the convex hull
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_2 p0(0, 0);
        IK::Point_2 p1(0, 0);
        CGAL::Iso_rectangle_2<IK> rectangle(p0, p1);
        double min_angle = 0;

        for (int i = 0; i < hull_points.size(); i++)
        {

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get polyline index
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto next_index = i + 1;
            auto current = hull_points[i];
            auto next = hull_points[next_index % hull_points.size()];
            auto segment = IK::Segment_3(current, next);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // min / max points
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            double top = -std::numeric_limits<double>::max();
            double bottom = std::numeric_limits<double>::max();
            double left = std::numeric_limits<double>::max();
            double right = -std::numeric_limits<double>::max();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get angle of segment to x axis
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto angle = internal::angle_to_xaxis(segment);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // rotate every point and get min and max values for each direction
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            for (auto &p : hull_points)
            {
                auto rotated_point = internal::rotate_to_xaxis(p, angle);

                top = std::max(top, rotated_point.hy());
                bottom = std::min(bottom, rotated_point.hy());
                left = std::min(left, rotated_point.hx());
                right = std::max(right, rotated_point.hx());
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // create a rectangle with the min and max values
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            auto rectangle_temp = CGAL::Iso_rectangle_2<IK>(IK::Point_2(left, bottom), IK::Point_2(right, top));

            // check if this is the first rectangle or if the current one is smaller
            if (rectangle.is_degenerate() || rectangle_temp.area() < rectangle.area())
            {
                rectangle = rectangle_temp;
                min_angle = angle;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get two rectangle segments for interpolation, rectangle is place on XY axis
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // std::array<IK::Segment_3, 2> two_sides = u_or_v ? std::array<IK::Segment_3, 2>{
        //                                                       IK::Segment_3(
        //                                                           IK::Point_3(rectangle.min().hx(), rectangle.min().hy(), 0),
        //                                                           IK::Point_3(rectangle.min().hx(), rectangle.max().hy(), 0)),
        //                                                       IK::Segment_3(
        //                                                           IK::Point_3(rectangle.max().hx(), rectangle.min().hy(), 0),
        //                                                           IK::Point_3(rectangle.max().hx(), rectangle.max().hy(), 0))}
        //                                                 : std::array<IK::Segment_3, 2>{IK::Segment_3(IK::Point_3(rectangle.min().hx(), rectangle.max().hy(), 0), IK::Point_3(rectangle.max().hx(), rectangle.max().hy(), 0)), IK::Segment_3(IK::Point_3(rectangle.min().hx(), rectangle.min().hy(), 0), IK::Point_3(rectangle.max().hx(), rectangle.min().hy(), 0))};

        IK::Point_2 center_of_rect_2 = CGAL::midpoint(rectangle.min(), rectangle.max());
        IK::Point_3 center_of_rect(center_of_rect_2.hx(), center_of_rect_2.hy(), 0);
        IK::Vector_3 half_dir_u((rectangle.max().hx() - rectangle.min().hx()) * 0.5, 0, 0);
        IK::Vector_3 half_dir_v(0, (rectangle.max().hy() - rectangle.min().hy()) * 0.5, 0);

        // std::cout << rectangle.min().hx() << " " << rectangle.min().hy() << " " << 0 << std::endl;
        // std::cout << rectangle.max().hx() << " " << rectangle.max().hy() << " " << 0 << std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // number of divisions
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int divisions_u, divisions_v;
        // if (division_distance != 0)
        //{
        divisions_u = std::min(std::min(20.0, max_points*0.5), std::floor(std::sqrt(half_dir_u.squared_length()) / division_distance));
        divisions_v = std::min(std::min(20.0, max_points*0.5), std::floor(std::sqrt(half_dir_v.squared_length()) / division_distance));
        //}
        // else
        // {

        //     // when divisions are given instead of division distance
        //     double smaller_edge_dist = std::min(std::sqrt(half_dir_u.squared_length()), std::sqrt(half_dir_v.squared_length()));
        //     double division_distance = smaller_edge_dist / (divisions * 2);
        //     divisions_u = divisions_v = divisions;
        //     std::cout << smaller_edge_dist << " " << division_distance << " " << divisions << " " << divisions_u << " " << divisions_v << std::endl;
        // }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // change magnitue of u and v vector to the division_sitance
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        half_dir_u = IK::Vector_3(division_distance, 0, 0);
        half_dir_v = IK::Vector_3(0, division_distance, 0);
        // std::cout << divisions_u << " " << divisions_v << std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // move the middle point bottom left corner
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        center_of_rect -= (half_dir_u * divisions_u + half_dir_v * divisions_v);
        // std::cout << center_of_rect.hx() << " " << center_of_rect.hy() << " " << center_of_rect.hz() << std::endl;

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert polygon to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::Path64 polyline_clipper(polygon_copy.size() - 1);

        for (int i = 0; i < polygon_copy.size() - 1; i++)
        {
            polyline_clipper[i] = Clipper2Lib::Point64((int)(polygon_copy[i].x() * 1e6), (int)(polygon_copy[i].y() * 1e6));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // add points and cull them
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // std::vector<IK::Point_3> result;
        points.reserve((divisions_u * 2 + 1) * (divisions_v * 2 + 1));
        for (int i = 0; i < divisions_u * 2 + 1; i++)
        {
            for (int j = 0; j < divisions_v * 2 + 1; j++)
            {
                IK::Point_3 p = center_of_rect + half_dir_u * i + half_dir_v * j;

                // Rotate and convert to clipper and check point inclusion
                double x_rotated = p.hx() * std::cos(-min_angle) - p.hy() * std::sin(-min_angle);
                double y_rotated = p.hx() * std::sin(-min_angle) + p.hy() * std::cos(-min_angle);
                Clipper2Lib::Point64 point_clipper((int)(x_rotated * 1e6), (int)(y_rotated * 1e6));

                if (Clipper2Lib::PointInPolygonResult::IsInside == Clipper2Lib::PointInPolygon(point_clipper, polyline_clipper))
                {
                    // rotate and orient to 3D
                    p = internal::rotate_to_xaxis(p, -min_angle);
                    p = p.transform(xform_to_xy_inv);
                    points.emplace_back(p);
                    // std::cout << p.x() << " " << p.y() << " " << p.z() << std::endl;
                }
            }
        }

        return true;
    }
}
