
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_polyline_util.h"

namespace cgal_polyline_util
{

    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in clipper_util to be used as self-contained header and source file
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

        void average_normal(const CGAL_Polyline &p, IK::Vector_3 &average_normal)
        {
            size_t len = CGAL::squared_distance(p.front(), p.back()) < wood_globals::DISTANCE_SQUARED ? p.size() - 1 : p.size();

            average_normal = IK::Vector_3(0, 0, 0);

            for (int i = 0; i < len; i++)
            {
                auto num = ((i - 1) + len) % len;
                auto item1 = ((i + 1) + len) % len;
                average_normal = average_normal + CGAL::cross_product(p[i] - p[num], p[item1] - p[i]);
            }
            unitize(average_normal);
        }

        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane)
        {
            auto x0 = plane.base1();
            auto y0 = plane.base2();
            auto z0 = plane.orthogonal_vector();
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
    }

    void shift(CGAL_Polyline &pline, const int &times)
    {
        pline.pop_back();
        std::rotate(pline.begin(), pline.begin() + times, pline.end());
        pline.emplace_back(pline[0]);
    }

    double polyline_length(CGAL_Polyline &pline)
    {
        double length = 0;
        for (auto i = 0; i < pline.size(); i++)
            length += std::sqrt(CGAL::squared_distance(pline[i + 0], pline[i + 1]));
        return length;
    }

    double polyline_length_squared(CGAL_Polyline &pline)
    {
        double length = 0;
        for (auto i = 0; i < pline.size(); i++)
            length += CGAL::squared_distance(pline[i + 0], pline[i + 1]);
        return length;
    }

    IK::Point_3 point_at(const IK::Segment_3 &line, const double &t)
    {
        // s[0].z()
        //  26 Feb 2003 Dale Lear
        //      Changed
        //           return (1-t)*from + t*to;
        //      to the following so that axis aligned lines will
        //      return exact answers for large values of t.
        //      See RR 9683.

        const double s = 1.0 - t;

        return IK::Point_3(
            (line[0].x() == line[1].x()) ? line[0].x() : s * line[0].x() + t * line[1].x(),
            (line[0].y() == line[1].y()) ? line[0].y() : s * line[0].y() + t * line[1].y(),
            (line[0].z() == line[1].z()) ? line[0].z() : s * line[0].z() + t * line[1].z());
    }

    void closest_point_to(const IK::Point_3 &point, const IK::Segment_3 &line, double &t)
    {

        const IK::Vector_3 D = line.to_vector();
        const double DoD = D.squared_length();

        if (DoD > 0.0)
        {
            if ((point - line[0]).squared_length() <= (point - line[1]).squared_length())
                t = ((point - line[0]) * D) / DoD;
            else
                t = 1.0 + ((point - line[1]) * D) / DoD;
        }
        else
        { // (GBA) Closest point to a degenerate line works as well
            t = 0.0;
        }
    }

    bool line_line_overlap(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output)
    {
        // four parameters that represents the two lines
        double t[4];
        t[0] = 0;
        t[1] = 1;

        // IF ERROR CHANGE TO THIS:
        // closest_point_to(line0[0], line1, t[2]);
        // closest_point_to(line0[1], line1, t[3]);
        closest_point_to(line1[0], line0, t[2]);
        closest_point_to(line1[1], line0, t[3]);

        // check if there is an overlap
        bool do_overlap = !((t[2] < 0 && t[3] < 0) || (t[2] > 1 && t[3] > 1));

        // sort line0 parameters
        std::sort(t, t + 4);

        // check if the overalp is not a point
        do_overlap = std::abs(t[2] - t[1]) > wood_globals::DISTANCE_SQUARED;

        // get line points as the overlap
        output = IK::Segment_3(point_at(line0, t[1]), point_at(line0, t[2]));

        // return if the lines are actually overallaping
        return do_overlap;
    }

    bool line_from_projected_points(const IK::Segment_3 &line, const CGAL_Polyline &points, IK::Segment_3 &output)
    {

        // reserve memory
        std::vector<double> t(points.size());
        t.reserve(points.size());

        // project all polyline points to the line
        for (size_t i = 0; i < points.size(); i++)
            closest_point_to(points[i], line, t[i]);

        // sort the parameters
        std::sort(t.begin(), t.end());

        // output the first and the last points as a segment
        output = IK::Segment_3(point_at(line, t.front()), point_at(line, t.back()));

        // check if not the points
        return std::abs(t.front() - t.back()) > wood_globals::DISTANCE_SQUARED;
    }

    void line_line_average(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output)
    {
        output = IK::Segment_3(CGAL::midpoint(line0[0], line1[0]), CGAL::midpoint(line0[1], line1[1]));
    }

    void line_line_overlap_average(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output)
    {
        // get two overalps
        IK::Segment_3 lineA;
        line_line_overlap(line0, line1, lineA);
        IK::Segment_3 lineB;
        line_line_overlap(line1, line0, lineB);

        // construct middle lines, incase the first one is flipped
        IK::Segment_3 mid_line0(CGAL::midpoint(lineA[0], lineB[0]), CGAL::midpoint(lineA[1], lineB[1]));
        IK::Segment_3 mid_line1(CGAL::midpoint(lineA[0], lineB[1]), CGAL::midpoint(lineA[1], lineB[0]));

        // the diagonal is always longer, so return the longer
        output = (mid_line0.squared_length() > mid_line1.squared_length()) ? mid_line0 : mid_line1;
    }

    void line_line_overlap_average(const CGAL_Polyline &line0, const CGAL_Polyline &line1, IK::Segment_3 &output)
    {
        IK::Segment_3 l0(line0[0], line0[1]);
        IK::Segment_3 l1(line1[0], line1[1]);
        line_line_overlap_average(l0, l1, output);
    }

    double closest_distance_and_point(const IK::Point_3 &point, const CGAL_Polyline &polyline, size_t &edge_id, IK::Point_3 &closest_point)
    {
        edge_id = 0;
        double closestDistance = DBL_MAX;
        double t = 0;

        for (auto i = 0; i < polyline.size() - 1; i++)
        {
            IK::Segment_3 segment_(polyline[i], polyline[i + 1]);

            double t_temp = 0;
            closest_point_to(point, segment_, t_temp);

            double closestDistanceTemp = CGAL::squared_distance(point, closest_point);

            if (closestDistanceTemp < closestDistance)
            {
                closestDistance = closestDistanceTemp;
                edge_id = i;
                t = t_temp;
            }

            if (closestDistance < wood_globals::DISTANCE_SQUARED)
                break;
        }

        closest_point = point_at(IK::Segment_3(polyline[edge_id], polyline[edge_id + 1]), t);

        return closestDistance;
    }

    bool is_closed(const CGAL_Polyline &polyline)
    {
        return CGAL::squared_distance(polyline.front(), polyline.back()) <  wood_globals::DISTANCE_SQUARED;
    }

    IK::Point_3 center(const CGAL_Polyline &polyline)
    {
        double x = 0, y = 0, z = 0;
        auto n = is_closed(polyline) ? polyline.size() - 1 : polyline.size();

        for (auto i = 0; i < n; i++)
        {
            x += polyline[i].x();
            y += polyline[i].y();
            z += polyline[i].z();
        }
        x /= n;
        y /= n;
        z /= n;

        return IK::Point_3(x, y, z);
    }

    IK::Vector_3 center_vec(const CGAL_Polyline &polyline)
    {
        double x = 0, y = 0, z = 0;
        auto n = is_closed(polyline) ? polyline.size() - 1 : polyline.size();

        for (auto i = 0; i < n; i++)
        {
            x += polyline[i].x();
            y += polyline[i].y();
            z += polyline[i].z();
        }
        x /= n;
        y /= n;
        z /= n;

        return IK::Vector_3(x, y, z);
    }

    void transform(CGAL_Polyline &polyline, const CGAL::Aff_transformation_3<IK> &transform)
    {
        for (auto it = polyline.begin(); it != polyline.end(); ++it)
            *it = it->transform(transform);
    }

    void get_average_plane(CGAL_Polyline &polyline, IK::Vector_3 (&plane_axes)[4])
    {
        // Origin
        plane_axes[0] = center_vec(polyline);

        // XAxis
        plane_axes[1] = polyline[1] - polyline[0];
        internal::unitize(plane_axes[1]);

        // ZAxis
        internal::average_normal(polyline, plane_axes[3]);

        // YAxis
        plane_axes[2] = CGAL::cross_product(plane_axes[3], plane_axes[1]);
    }

    void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &origin, IK::Plane_3 &plane)
    {
        // origin
        origin = polyline[0];

        // plane
        IK::Vector_3 average_normal;
        internal::average_normal(polyline, average_normal);
        plane = IK::Plane_3(origin, average_normal);
    }

    void get_middle_line(const IK::Segment_3 &line0, const IK::Segment_3 &line1, IK::Segment_3 &output)
    {
        IK::Point_3 p0 = IK::Point_3(
            (line0[0].x() + line1[0].x()) * 0.5,
            (line0[0].y() + line1[0].y()) * 0.5,
            (line0[0].z() + line1[0].z()) * 0.5);

        IK::Point_3 p1 = IK::Point_3(
            (line0[1].x() + line1[1].x()) * 0.5,
            (line0[1].y() + line1[1].y()) * 0.5,
            (line0[1].z() + line1[1].z()) * 0.5);

        output = IK::Segment_3(p0, p1);
    }

    void extend_line(IK::Segment_3 &line, const double &distance0, const double &distance1)
    {
        IK::Point_3 p0 = line[0];
        IK::Point_3 p1 = line[1];

        auto v = line.to_vector();
        internal::unitize(v);

        line = IK::Segment_3(p0 - v * distance0, p1 + v * distance1);
    }

    void scale_line(IK::Segment_3 &line, const double &distance)
    {
        IK::Point_3 p0 = line[0];
        IK::Point_3 p1 = line[1];
        IK::Vector_3 v = p1 - p0;
        p0 += v * distance;
        p1 -= v * distance;
        line = IK::Segment_3(p0, p1);
    }

    void extend(CGAL_Polyline &pline, int sID, double dist0, double dist1, double proportion0, double proportion1)
    {
        if (dist0 == 0 && dist1 == 0 && proportion0 == 0 && proportion1 == 0)
            return;

        IK::Point_3 p0 = pline[sID];
        IK::Point_3 p1 = pline[sID + 1];
        IK::Vector_3 v = p1 - p0;

        // Either scale or extend polyline segments
        if (proportion0 != 0 || proportion1 != 0)
        {
            p0 -= v * proportion0;
            p1 += v * proportion1;
        }
        else
        {
            internal::unitize(v);
            p0 -= v * dist0;
            p1 += v * dist1;
        }

        pline[sID] = p0;
        pline[sID + 1] = p1;

        if (sID == 0)
            pline[pline.size() - 1] = pline[0];
        else if ((sID + 1) == pline.size() - 1)
            pline[0] = pline[pline.size() - 1];
    }

    void extend_equally(IK::Segment_3 &segment, double dist, double proportion)
    {
        if (dist == 0 && proportion == 0)
            return;

        IK::Point_3 p0 = segment[0];
        IK::Point_3 p1 = segment[1];
        IK::Vector_3 v = p1 - p0;

        // Either scale or extend polyline segments
        if (proportion != 0)
        {
            p0 -= v * proportion;
            p1 += v * proportion;
        }
        else
        {
            internal::unitize(v);
            p0 -= v * dist;
            p1 += v * dist;
        }

        segment = IK::Segment_3(p0, p1);
    }

    void extend_equally(CGAL_Polyline &pline, int segment_id, double dist, double proportion)
    {


        if (dist == 0 && proportion == 0)
            return;

        bool is_pline_closed = is_closed(pline);

        IK::Point_3 p0 = pline[segment_id];
        IK::Point_3 p1 = pline[segment_id + 1];
        IK::Vector_3 v = p1 - p0;

        // Either scale or extend polyline segments
        if (proportion != 0)
        {
            p0 -= v * proportion;
            p1 += v * proportion;
        }
        else
        {
            internal::unitize(v);
            p0 -= v * dist;
            p1 += v * dist;
        }

        pline[segment_id] = p0;
        pline[segment_id + 1] = p1;

        if (pline.size() > 2)
        { // not a line
            if (is_pline_closed)
            { // user given, else you can compute the square distance
                if (segment_id == 0){
                    pline[pline.size() - 1] = pline[0];
                }
                else if ((segment_id+1) == pline.size() - 1) {
                    pline[0] = pline[pline.size() - 1];
                }
            }
        }
    }

    void move(CGAL_Polyline &polyline, const IK::Vector_3 &direction)
    {
        CGAL::Aff_transformation_3<IK> T(CGAL::TRANSLATION, direction);
        transform(polyline, T);
    }

    bool is_clockwise(CGAL_Polyline &polyline, IK::Plane_3 plane)
    {
        // copy
        CGAL_Polyline cp = polyline;

        // transform
        CGAL::Aff_transformation_3<IK> xform_toXY = internal::plane_to_xy(polyline[0], plane);
        transform(cp, xform_toXY);

        // check if closed
        if (!is_closed(polyline))
        {
            cp.emplace_back(cp[0]);
        }

        // check winding, negative anti-clockwise
        double num = 0;
        for (auto i = 0; i < cp.size() - 1; i++)
        {
            auto item = cp[i + 1];
            double x = item.hx();
            item = cp[i];
            double x1 = x - item.hx();
            item = cp[i + 1];
            double y = item.hy();
            item = cp[i];
            double y1 = y + item.hy();
            num = num + x1 * y1;
        }
        return num > 0;
    }

    void flip(CGAL_Polyline &polyline)
    {
        std::reverse(polyline.begin(), polyline.end());
    }

    void get_convex_corners(const CGAL_Polyline &polyline, std::vector<bool> &convex_or_concave)
    {
        // check if it is closed
        bool closed = true;
        if (CGAL::squared_distance(polyline[0], polyline[polyline.size() - 1]) > wood_globals::DISTANCE_SQUARED)
            closed = false;

        // get average normal
        IK::Vector_3 normal(0, 0, 1);
        internal::average_normal(polyline, normal);

        // reserve boolean flags
        auto n = closed ? polyline.size() - 1 : polyline.size();
        convex_or_concave.reserve(n);

        // find the convex corner corners
        for (auto current = 0; current < n; current++)
        {
            auto prev = current == 0 ? n - 1 : current - 1;
            auto next = current == n - 1 ? 0 : current + 1;

            auto dir0 = polyline[current] - polyline[prev];
            internal::unitize(dir0);

            auto dir1 = polyline[next] - polyline[current];
            internal::unitize(dir1);

            auto dir2 = CGAL::cross_product(dir0, dir1);
            internal::unitize(dir2);

            auto dot = dir2 * normal; // dot product

            bool is_convex = !(dot < 0.0);
            convex_or_concave.emplace_back(is_convex);
        }
    }

    CGAL_Polyline tween_two_polylines(const CGAL_Polyline &polyline0, const CGAL_Polyline &polyline1, const double &weight)
    {

        CGAL_Polyline result;
        result.reserve(polyline0.size());

        for (size_t i = 0; i < polyline0.size(); i++)
        {
            auto p = polyline0[i] + (polyline1[i] - polyline0[i]) * weight;
            result.emplace_back(p);
        }
        return result;
    }

}
