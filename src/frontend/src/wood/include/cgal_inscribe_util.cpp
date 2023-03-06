
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

 
#include "cgal_inscribe_util.h"

namespace cgal_inscribe_util
{

    namespace internal
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in cgal_inscribe_util to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, IK::Vector_3 &x0, IK::Vector_3 &y0, IK::Vector_3 &z0)
        {
            unitize(x0);
            unitize(y0);
            unitize(z0);

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f0(
                x0.x(), x0.y(), x0.z(),
                y0.x(), y0.y(), y0.z(),
                z0.x(), z0.y(), z0.z());

            return f0 * t0;
        }

        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane)
        {
            auto x0 = plane.base1();
            auto y0 = plane.base2();
            auto z0 = plane.orthogonal_vector();
            internal::unitize(x0);
            internal::unitize(y0);
            internal::unitize(z0);

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f0(
                x0.x(), x0.y(), x0.z(),
                y0.x(), y0.y(), y0.z(),
                z0.x(), z0.y(), z0.z());

            return f0 * t0;
        }

        void get_average_normal(const CGAL_Polyline &polyline, IK::Vector_3 &average_normal)
        {
            size_t len = CGAL::squared_distance(polyline.front(), polyline.back()) < wood_globals::DISTANCE_SQUARED ? polyline.size() - 1 : polyline.size();
            average_normal = IK::Vector_3(0, 0, 0);

            for (size_t i = 0; i < len; i++)
            {
                auto prev = ((i - 1) + len) % len;
                auto next = ((i + 1) + len) % len;
                average_normal = average_normal + CGAL::cross_product(polyline[i] - polyline[prev], polyline[next] - polyline[i]);
            }
        }

        void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &center, IK::Plane_3 &plane)
        {
            // origin
            center = polyline[0];

            // plane
            IK::Vector_3 average_normal;
            get_average_normal(polyline, average_normal);
            plane = IK::Plane_3(center, average_normal);
        }

        void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis)
        {
            // origin
            origin = polyline[0];

            // plane
            IK::Vector_3 average_normal;
            get_average_normal(polyline, average_normal);
            IK::Plane_3 plane(origin, average_normal);

            x_axis = plane.base1();
            y_axis = plane.base2();
            z_axis = plane.orthogonal_vector();
        }

        void get_average_plane_axes_oriented_to_longest_edge(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Vector_3 &x_axis, IK::Vector_3 &y_axis, IK::Vector_3 &z_axis)
        {
            size_t len = CGAL::squared_distance(polyline.front(), polyline.back()) < wood_globals::DISTANCE_SQUARED ? polyline.size() - 1 : polyline.size();
            z_axis = IK::Vector_3(0, 0, 0);

            double max_length = 0;
            for (size_t i = 0; i < len; i++)
            {
                auto prev = ((i - 1) + len) % len;
                auto next = ((i + 1) + len) % len;
                z_axis = z_axis + CGAL::cross_product(polyline[i] - polyline[prev], polyline[next] - polyline[i]);

                double temp_max_length = CGAL::squared_distance(polyline[i], polyline[next]);
                if (temp_max_length > max_length)
                {
                    max_length = temp_max_length;
                    x_axis = polyline[next] - polyline[i];
                    origin = polyline[i];
                }
            }
            y_axis = CGAL::cross_product(x_axis, z_axis);
        }

        void circle_points(const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis, std::vector<IK::Point_3> &points, int number_of_chunks, double radius)
        {

            double pi_to_radians = 3.14159265358979323846 / 180.0;
            double degrees = 0; // <-- correction
            double chunk_angle = 360 / number_of_chunks;
            points.reserve(number_of_chunks);
            CGAL::Aff_transformation_3<IK> xy_to_plane = internal::xy_to_plane(origin, x_axis, y_axis, z_axis);

            for (int i = 0; i < number_of_chunks; i++)
            {
                degrees = i * chunk_angle;                       // <-- correction
                float radian = ((45 + degrees) * pi_to_radians); // <-- correction
                IK::Point_3 p(radius * cos(radian), radius * sin(radian), 0);
                points.emplace_back(p.transform(xy_to_plane));
            }
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

        CGAL::Aff_transformation_3<IK> xy_to_plane(const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis)
            {
                // Vectors must be unitized, else the result is wrong
                IK::Vector_3 _x_axis = x_axis;
                IK::Vector_3 _y_axis = y_axis;
                IK::Vector_3 _z_axis = z_axis;

                internal::unitize(_x_axis);
                internal::unitize(_y_axis);
                internal::unitize(_z_axis);

                // transformation maps P0 to P1, P0+x_axis_0 to P1+x_axis_1, ...
                CGAL::Aff_transformation_3<IK> f(
                    _x_axis.x(), _y_axis.x(), _z_axis.x(),
                    _x_axis.y(), _y_axis.y(), _z_axis.y(),
                    _x_axis.z(), _y_axis.z(), _z_axis.z());

                // Move to 3d -> T1 translates (0,0,0) to point P1
                CGAL::Aff_transformation_3<IK> t(CGAL::TRANSLATION, IK::Vector_3(origin.x(), origin.y(), origin.z()));

                return t * f;
            }


        CGAL::Aff_transformation_3<IK> axis_rotation(const double &angle, const IK::Vector_3 &axis)
    {
        // create matrix of the rotation
        IK::RT
            c = cos(angle),
            s = sin(angle),
            ux(axis.x()),
            uy(axis.y()),
            uz(axis.z());

        IK::RT matrix[12] =
            {
                ux * ux * (1 - c) + c, ux * uy * (1 - c) - uz * s, ux * uz * (1 - c) + uy * s, 0,
                ux * uy * (1 - c) + uz * s, uy * uy * (1 - c) + c, uy * uz * (1 - c) - ux * s, 0,
                ux * uz * (1 - c) - uy * s, uy * uz * (1 - c) + ux * s, uz * uz * (1 - c) + c, 0};

        return CGAL::Aff_transformation_3<IK>(matrix[0], matrix[1], matrix[2],
                                              matrix[3], matrix[4], matrix[5],
                                              matrix[6], matrix[7], matrix[8],
                                              matrix[9], matrix[10], matrix[11]);
    }

    }

    std::tuple<IK::Point_3, IK::Plane_3, double> get_polylabel(const std::vector<CGAL_Polyline> &polylines, double precision)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // copy polylines for orienting from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines_copy = polylines;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation to XY plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 origin;
        IK::Plane_3 plane;
        cgal_polyline_util::get_fast_plane(polylines_copy[0], origin, plane);

        CGAL::Aff_transformation_3<IK> xform_toXY = internal::plane_to_xy(origin, plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        for (auto &polyline : polylines_copy)
            cgal_polyline_util::transform(polyline, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sort polylines based on bounding-box to detect which polylines are holes, this only works if the polygon does not have holes within holes
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<double> len = std::vector<double>(polylines_copy.size());
        std::vector<int> ids(len.size());
        std::iota(begin(ids), end(ids), 0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get bounding box length of polylines_copy
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (len.size() > 0)
        {
            for (size_t i = 0; i < polylines_copy.size(); i++)
            {
                CGAL::Bbox_3 AABB = CGAL::bbox_3(polylines_copy[i].begin(), polylines_copy[i].end(), IK());
                auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
                auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
                len[i] = CGAL::squared_distance(p0, p1);
            }
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Sort polylines_copy indices by the bounding box length
            // WARNING orientation is not checked, which should be fine with the Polylabel algorithm
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            std::vector<int> ids(len.size());
            std::sort(begin(ids), end(ids), [&](int ia, int ib)
                      { return len[ia] < len[ib]; });
            std::reverse(begin(ids), end(ids));
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert polylines_copy to Polylabel datastructure, be aware that the polyline considers all polygons closed, so skip the last point
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        mapbox::geometry::polygon<double> polygon_with_holes(polylines_copy.size());

        for (size_t i = 0; i < polylines_copy.size(); i++)
        {
            for (size_t j = 0; j < polylines_copy[ids[i]].size() - 1; j++)
            {
                polygon_with_holes[i].emplace_back(polylines_copy[ids[i]][j].x(), polylines_copy[ids[i]][j].y());
                // std::cout << polylines_copy[ids[i]][j].x() << " " << polylines_copy[ids[i]][j].y() << std::endl;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Run the Polylabel algorithm, WARNING numbers are in 2D, first two doubles are center, the third is the radius
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::array<double, 3> center_and_radius = mapbox::polylabel(polygon_with_holes, 1.0);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert from the Polylabel datastructure to CGAL
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 center(center_and_radius[0], center_and_radius[1], 0);
        center = center.transform(xform_toXY_Inv);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        return std::tuple<IK::Point_3, IK::Plane_3, double>(center, plane, center_and_radius[2]);
    }

    void get_polylabel_circle_division_points(const IK::Vector_3 &division_direction_in_3d, const std::vector<CGAL_Polyline> &polylines, std::vector<IK::Point_3> &points, int division, double scale, double precision, bool orient_to_closest_edge)
    {
        // run the polylabel algorithm
        std::tuple<IK::Point_3, IK::Plane_3, double> circle = get_polylabel(polylines, precision);
        IK::Vector_3 center(std::get<0>(circle).hx(), std::get<0>(circle).hy(), std::get<0>(circle).hz());
        bool is_direction_valid = division_direction_in_3d.hx() != 0.0 && division_direction_in_3d.hy() != 0.0 && division_direction_in_3d.hz() != 0.0;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Find closest edge direction to the center
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        IK::Point_3 center_point(std::get<0>(circle).hx(), std::get<0>(circle).hy(), std::get<0>(circle).hz());
        double distance_from_center_to_polyline_edge = DBL_MAX;
        size_t edge_id0 = 0;
        size_t edge_id1 = 0;
        if (orient_to_closest_edge)
        {

            for (size_t i = 0; i < polylines.size(); i++)
            {
                for (size_t j = 0; j < polylines[i].size() - 1; j++)
                {

                    IK::Segment_3 polyline_segment(polylines[i][j], polylines[i][j + 1]);
                    IK::Point_3 projection = polyline_segment.supporting_line().projection(center_point);

                    // closest point paraneters
                    bool has_on = false;
                    const IK::Vector_3 polyline_segment_vector = polyline_segment.to_vector();
                    const double polyline_segment_vector_squared_len = polyline_segment_vector.squared_length();
                    double t = 0;

                    if (polyline_segment_vector.squared_length() > 0.0)
                    {
                        if ((projection - polyline_segment[0]).squared_length() <= (projection - polyline_segment[1]).squared_length())
                            t = ((projection - polyline_segment[0]) * polyline_segment_vector) / polyline_segment_vector_squared_len;
                        else
                            t = 1.0 + ((projection - polyline_segment[1]) * polyline_segment_vector) / polyline_segment_vector_squared_len;

                        has_on = t >= 0 && t <= 1;
                    }
                    else
                    { // (GBA) Closest point to a degenerate line works as well
                        t = 0.0;
                        has_on = false;
                    }

                    // check if the point is on the edge
                    if (has_on)
                    {
                        double temp_distance_from_center_to_polyline_edge = CGAL::squared_distance(center_point, projection);
                        if (temp_distance_from_center_to_polyline_edge < distance_from_center_to_polyline_edge)
                        {
                            distance_from_center_to_polyline_edge = temp_distance_from_center_to_polyline_edge;
                            edge_id0 = i;
                            edge_id1 = j;
                        }
                    }
                }
            }
        }

        // orientation direction
        IK::Vector_3 division_direction_in_3d_ = orient_to_closest_edge ? polylines[edge_id0][edge_id1 + 1] - polylines[edge_id0][edge_id1 + 0] : IK::Vector_3(division_direction_in_3d.hx(), division_direction_in_3d.hy(), division_direction_in_3d.hz());

        // plane vectors for orienting again to 2d for circle points dvisions
        IK::Vector_3 x_axis = is_direction_valid || orient_to_closest_edge ? division_direction_in_3d_ : std::get<1>(circle).base1();
        IK::Vector_3 y_axis = is_direction_valid || orient_to_closest_edge ? CGAL::cross_product(division_direction_in_3d_, std::get<1>(circle).orthogonal_vector()) : std::get<1>(circle).base2();
        IK::Vector_3 z_axis = std::get<1>(circle).orthogonal_vector();

        // run the division method which is the output -> points
        internal::circle_points(center, x_axis, y_axis, z_axis, points, division, std::get<2>(circle) * scale);
    }

    bool inscribe_rectangle_in_convex_polygon(const std::vector<CGAL_Polyline> &polylines, CGAL_Polyline &result, std::vector<IK::Point_3> &points, IK::Segment_3 &direction, double scale, double precision, double rectangle_division_distance)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // copy polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines_copy = polylines;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // user direction
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        IK::Point_3 o;
        IK::Vector_3 x, y, z;
        if (direction.squared_length() > wood_globals::DISTANCE_SQUARED)
        {

            internal::get_fast_plane(polylines_copy[0], o, x, y, z);
            x = direction.to_vector();
            y = CGAL::cross_product(x, z);
        }
        else
        {
            // this was never checked
            internal::get_average_plane_axes_oriented_to_longest_edge(polylines_copy[0], o, x, y, z);
        }

        // if (polylines_copy[0].size() != 5)
        //     return true;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation to XY plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK>
            xform_toXY = internal::plane_to_xy(o, x, y, z);

        // without rotation the algorithm does not work, is it a bug of CGAL?
        CGAL::Aff_transformation_3<IK> rot = internal::axis_rotation(0.0001, IK::Vector_3(0, 0, 1));
        xform_toXY = rot * xform_toXY;

        CGAL::Aff_transformation_3<IK>
            xform_toXY_Inv = xform_toXY.inverse();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Count how many points we have
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        size_t point_count = 0;

        for (auto &polyline : polylines_copy)
            point_count += polyline.size();

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Compute 2D bounding box to get a division step, the division step is equal to = diagonal_length/(20*precision)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_2> points_2d;
        points_2d.reserve(point_count);

        for (auto &polyline : polylines_copy)
        {
            cgal_polyline_util::transform(polyline, xform_toXY);
            for (auto &p : polyline)
                points_2d.push_back(IK::Point_2(p.hx(), p.hy()));
            points_2d.pop_back();
        }

        CGAL::Bbox_2 bbox = CGAL::bbox_2(points_2d.begin(), points_2d.end(), IK());
        auto diagonal = {IK::Point_3(bbox.xmin(), bbox.ymin(), 0), IK::Point_3(bbox.xmax(), bbox.ymax(), 0)};
        double squared_diagonal_length = CGAL::squared_distance(IK::Point_3(bbox.xmin(), bbox.ymin(), 0), IK::Point_3(bbox.xmax(), bbox.ymax(), 0));
        double step_division = std::sqrt(squared_diagonal_length) / (50.0 * precision);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get iso rectangle as the first input for the algorithm
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Iso_rectangle_2 bbox_rectangle(bbox);
        CGAL::Largest_empty_iso_rectangle_2<IK> empty_rect(bbox_rectangle);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Interpolate edge points in by the precomputed division step
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        CGAL_Polyline division_points;
        CGAL_Polyline division_points_temp;
        for (auto &polyline : polylines_copy)
            for (int i = 0; i < polyline.size() - 1; i++)
            {
                division_points.clear();
                int divisions = (int)std::min(100.0, std::sqrt(CGAL::squared_distance(polyline[i], polyline[i + 1])) / step_division);
                cgal_vector_util::interpolate_points(polyline[i], polyline[i + 1], divisions, division_points, 2);
                division_points_temp.insert(division_points_temp.end(), division_points.begin(), division_points.end());

                for (auto &point : division_points)
                {
                    empty_rect.push_back(IK::Point_2(point.hx(), point.hy()));
                }
            }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get the largest empty iso rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Iso_rectangle_2 rectangle = empty_rect.get_largest_empty_iso_rectangle();
        double x_dist = (1 - scale) * std::abs(rectangle.max().hx() - rectangle.min().hx());
        double y_dist = (1 - scale) * std::abs(rectangle.max().hy() - rectangle.min().hy());
        x_dist = std::min(x_dist, y_dist);
        y_dist = x_dist;

        CGAL_Polyline rect = {
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.min().hy() + y_dist, 0),
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.max().hy() - y_dist, 0),
            IK::Point_3(rectangle.max().hx() - x_dist, rectangle.max().hy() - y_dist, 0),
            IK::Point_3(rectangle.max().hx() - x_dist, rectangle.min().hy() + y_dist, 0),
            IK::Point_3(rectangle.min().hx() + x_dist, rectangle.min().hy() + y_dist, 0),
        };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient Back to 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        for (auto &point : rect)
            point = point.transform(xform_toXY_Inv);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // point on the edge of the rectangle or division of points inside the rectangle
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (rectangle_division_distance > wood_globals::DISTANCE)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // divide edges by distance
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            for (int i = 0; i < rect.size() - 1; i++)
            {
                division_points.clear();
                int divisions = (int)std::min(25.0, std::sqrt(CGAL::squared_distance(rect[i], rect[i + 1])) / rectangle_division_distance);
                cgal_vector_util::interpolate_points(rect[i], rect[i + 1], divisions, division_points, 2);
                points.insert(points.end(), division_points.begin(), division_points.end());
            }
            // cgal_polyline_util::transform(division_points_temp, xform_toXY_Inv);
            // points.clear();
            // points.insert(points.end(), division_points_temp.begin(), division_points_temp.end());
            //  auto p0_min = (IK::Point_3(bbox.xmin(), bbox.ymin(), 0)).transform(xform_toXY_Inv);
            //  auto p0_max = (IK::Point_3(bbox.xmax(), bbox.ymax(), 0)).transform(xform_toXY_Inv);
            //  points.emplace_back(p0_min);
            //  points.emplace_back(p0_max);
        }
        else if (rectangle_division_distance < -wood_globals::DISTANCE)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // get divisions of each edge and take the smallest division
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            double unsigned_rectangle_division_distance = std::abs(rectangle_division_distance);
            double edge_dist_00 = std::sqrt(CGAL::squared_distance(rect[0], rect[1]));
            double edge_dist_01 = std::sqrt(CGAL::squared_distance(rect[3], rect[2]));
            double edge_dist_10 = std::sqrt(CGAL::squared_distance(rect[1], rect[2]));
            double edge_dist_11 = std::sqrt(CGAL::squared_distance(rect[0], rect[3]));

            int divisions_u = (int)(std::min(std::ceil(edge_dist_00 / unsigned_rectangle_division_distance), std::ceil(edge_dist_01 / unsigned_rectangle_division_distance)));
            int divisions_v = (int)(std::min(std::ceil(edge_dist_10 / unsigned_rectangle_division_distance), std::ceil(edge_dist_11 / unsigned_rectangle_division_distance)));

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // divide two edges into divisions points and then interpolate these points
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            std::vector<IK::Point_3> division_points_edge_0, division_points_edge_2;
            division_points_edge_0.reserve(divisions_u);
            division_points_edge_2.reserve(divisions_v);
            cgal_vector_util::interpolate_points(rect[0], rect[1], divisions_u, division_points_edge_0, 1);
            cgal_vector_util::interpolate_points(rect[3], rect[2], divisions_u, division_points_edge_2, 1);

            points.reserve((divisions_u + 2) * (divisions_v + 2));
            for (int i = 0; i < (divisions_u + 2); i++)
            {
                std::vector<IK::Point_3> temp_divisions;
                cgal_vector_util::interpolate_points(division_points_edge_0[i], division_points_edge_2[i], divisions_v, temp_divisions, 1);
                points.insert(points.end(), temp_divisions.begin(), temp_divisions.end());
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        result = rect;
        return true;
    }

}