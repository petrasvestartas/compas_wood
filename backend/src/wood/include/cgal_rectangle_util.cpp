
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

 
#include "cgal_rectangle_util.h"

namespace cgal_rectangle_util
{

    namespace internal
    {

    double length(double x, double y, double z)
        {

            double len;
            x = fabs(x);
            y = fabs(y);
            z = fabs(z);
            if (y >= x && y >= z)
            {
                len = x;
                x = y;
                y = len;
            }
            else if (z >= x && z >= y)
            {
                len = x;
                x = z;
                z = len;
            }

            // 15 September 2003 Dale Lear
            //     For small denormalized doubles (positive but smaller
            //     than DBL_MIN), some compilers/FPUs set 1.0/x to +INF.
            //     Without the ON_DBL_MIN test we end up with
            //     microscopic vectors that have infinte length!
            //
            //     This code is absolutely necessary.  It is a critical
            //     part of the bug fix for RR 11217.
            if (x > ON_DBL_MIN)
            {
                y /= x;
                z /= x;
                len = x * sqrt(1.0 + y * y + z * z);
            }
            else if (x > 0.0 && ON_IS_FINITE(x))
                len = x;
            else
                len = 0.0;

            return len;
        }

        bool unitize(IK::Vector_3 &vector)
        {
            bool rc = false;
            // Since x,y,z are floats, d will not be denormalized and the
            // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

            double d = length(vector.hx(), vector.hy(), vector.hz());
            if (d > 0.0)
            {
                double dx = vector.hx();
                double dy = vector.hy();
                double dz = vector.hz();
                vector = IK::Vector_3(
                    (dx / d),
                    (dy / d),
                    (dz / d));
                rc = true;
            }
            return rc;
        }

        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane)
        {
            // Vectors must be unitized, else the result is wrong
            auto x_axis = plane.base1();
            auto y_axis = plane.base2();
            auto z_axis = plane.orthogonal_vector();
            internal::unitize(x_axis);
            internal::unitize(y_axis);
            internal::unitize(z_axis);

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f(
                x_axis.x(), x_axis.y(), x_axis.z(),
                y_axis.x(), y_axis.y(), y_axis.z(),
                z_axis.x(), z_axis.y(), z_axis.z());

            return f * t;
        }
                
        float ccw(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &c)
        {
            return (b.x() - a.x()) * (c.y() - a.y()) - (b.y() - a.y()) * (c.x() - a.x());
        }

        bool is_left_of(const IK::Point_3 &a, const IK::Point_3 &b)
        {
            return (a.x() < b.x() || (a.x() == b.x() && a.y() < b.y()));
        }

        float len(const IK::Point_3 &a, const IK::Point_3 &b)
        {
            return std::sqrt((b.x() - a.x()) * (b.x() - a.x()) + (b.y() - a.y()) * (b.y() - a.y()));
        }

        float dist(const IK::Point_3 &a, const IK::Point_3 &b, const IK::Point_3 &p)
        {
            return std::fabs((b.x() - a.x()) * (a.y() - p.y()) - (b.y() - a.y()) * (a.x() - p.x())) / len(a, b);
        }

        size_t get_farthest(const IK::Point_3 &a, const IK::Point_3 &b, const std::vector<IK::Point_3> &v)
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

        void quick_hull(const std::vector<IK::Point_3> &v, const IK::Point_3 &a, const IK::Point_3 &b, std::vector<IK::Point_3> &hull)
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

        double angle_to_xaxis(IK::Segment_3 &edge)
        {

            auto delta = edge[0] - edge[1];
            return -std::atan(delta.hy() / delta.hx());
        }

        IK::Point_3 rotate_to_xaxis(IK::Point_3 &point, const double &angle)
        {
            return IK::Point_3(point.hx() * std::cos(angle) - point.hy() * std::sin(angle), point.hx() * std::sin(angle) + point.hy() * std::cos(angle), 0);
        }




        bool orient_polyline_to_xy_and_back(CGAL_Polyline &polyline, CGAL::Aff_transformation_3<IK> &xform_to_xy, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
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
            xform_to_xy = internal::plane_to_xy(polyline[0], plane);
            xform_to_xy_inv = xform_to_xy.inverse();
            return true;
        }

        
   }

    CGAL_Polyline quick_hull(const CGAL_Polyline &v)
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

    bool bounding_rectangle(CGAL_Polyline &polygon, CGAL_Polyline &result)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polygon to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy, xform_to_xy_inv;
        internal::orient_polyline_to_xy_and_back(polygon, xform_to_xy, xform_to_xy_inv);
        CGAL_Polyline polygon_copy = polygon;
        cgal_polyline_util::transform(polygon_copy, xform_to_xy);

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

        for (size_t i = 0; i < hull_points.size(); i++)
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
        // Convert to CGAL polyline and rotate to axis and orient to 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        result = {
            IK::Point_3(rectangle.min().hx(), rectangle.min().hy(), 0),
            IK::Point_3(rectangle.min().hx(), rectangle.max().hy(), 0),
            IK::Point_3(rectangle.max().hx(), rectangle.max().hy(), 0),
            IK::Point_3(rectangle.max().hx(), rectangle.min().hy(), 0),
        };

        // rotate axis aligned box back
        for (auto &p : result)
        {
            p = internal::rotate_to_xaxis(p, -min_angle);
            p = p.transform(xform_to_xy_inv);
        }
        result.emplace_back(result.front());

        return true;
    }

    bool grid_of_points_in_a_polygon(CGAL_Polyline &polygon, const double &offset_distance, const double &division_distance, const int &max_points, std::vector<IK::Point_3> &points) // const double &divisions,
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polygon to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy, xform_to_xy_inv;
        internal::orient_polyline_to_xy_and_back(polygon, xform_to_xy, xform_to_xy_inv);
        CGAL_Polyline polygon_copy = polygon;
        cgal_polyline_util::transform(polygon_copy, xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // offset polygon if the input is below 0
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (offset_distance != 0)
            clipper_util::offset_in_2d(polygon_copy, offset_distance);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // input is rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (polygon_copy.size() == 5)
        {

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get divisions of each edge and take the smallest division
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            double edge_dist_00 = std::sqrt(CGAL::squared_distance(polygon_copy[0], polygon_copy[1]));
            double edge_dist_01 = std::sqrt(CGAL::squared_distance(polygon_copy[3], polygon_copy[2]));
            double edge_dist_10 = std::sqrt(CGAL::squared_distance(polygon_copy[1], polygon_copy[2]));
            double edge_dist_11 = std::sqrt(CGAL::squared_distance(polygon_copy[0], polygon_copy[3]));

            int divisions_u = (int)(std::min((int)(edge_dist_00 / division_distance), (int)(edge_dist_01 / division_distance)));
            int divisions_v = (int)(std::min((int)(edge_dist_10 / division_distance), (int)(edge_dist_11 / division_distance)));

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // divide two edges into divisions points and then interpolate these points
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::vector<IK::Point_3> division_points_edge_0, division_points_edge_2;
            division_points_edge_0.reserve(divisions_u + 2);
            division_points_edge_2.reserve(divisions_v + 2);
            cgal_vector_util::interpolate_points(polygon_copy[0], polygon_copy[1], divisions_u, division_points_edge_0, 1);
            cgal_vector_util::interpolate_points(polygon_copy[3], polygon_copy[2], divisions_u, division_points_edge_2, 1);

            points.reserve((divisions_u + 2) * (divisions_v + 2));
            for (int i = 0; i < (divisions_u + 2); i++)
            {
                std::vector<IK::Point_3> temp_divisions;
                cgal_vector_util::interpolate_points(division_points_edge_0[i], division_points_edge_2[i], divisions_v, temp_divisions, 1);
                for (auto &p : temp_divisions)
                    p = p.transform(xform_to_xy_inv);
                points.insert(points.end(), temp_divisions.begin(), temp_divisions.end());
            }
        }
        else
        {

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

            for (size_t i = 0; i < hull_points.size(); i++)
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
            divisions_u = std::min(std::min(20.0, max_points * 0.5), (2 * std::floor(std::sqrt(half_dir_u.squared_length())) / division_distance));
            divisions_v = std::min(std::min(20.0, max_points * 0.5), (2 * std::floor(std::sqrt(half_dir_v.squared_length())) / division_distance));

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

            for (size_t i = 0; i < polygon_copy.size() - 1; i++)
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
        }

        return true;
    }

}
