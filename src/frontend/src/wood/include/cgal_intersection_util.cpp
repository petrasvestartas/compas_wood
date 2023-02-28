
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

 
#include "cgal_intersection_util.h"

namespace cgal_intersection_util
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

        int is_parallel_to(const IK::Vector_3 &v0, const IK::Vector_3 &v1)
        {
            int rc = 0;

            double ll = length(v0.x(), v0.y(), v0.z()) * length(v1.x(), v1.y(), v1.z());
            if (ll > 0.0)
            {
                const double cos_angle = (v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z()) / ll;
                const double cos_tol = cos(wood_globals::ANGLE);
                if (cos_angle >= cos_tol)
                    rc = 1;
                else if (cos_angle <= -cos_tol)
                    rc = -1;
            }
            return rc;
        }

        int solve3x3(const double (&row0)[3], const double (&row1)[3], const double (&row2)[3],
                     double d0, double d1, double d2,
                     double *x_addr, double *y_addr, double *z_addr,
                     double *pivot_ratio)
        {
            /* Solve a 3x3 linear system using Gauss-Jordan elimination
             *
             */
            int i, j;
            double *p0;
            double *p1;
            double *p2;
            double x, y, workarray[12], maxpiv, minpiv;

            const int sizeof_row = 3 * sizeof(row0[0]);

            *pivot_ratio = *x_addr = *y_addr = *z_addr = 0.0;
            x = fabs(row0[0]);
            i = j = 0;
            y = fabs(row0[1]);
            if (y > x)
            {
                x = y;
                j = 1;
            }
            y = fabs(row0[2]);
            if (y > x)
            {
                x = y;
                j = 2;
            }
            y = fabs(row1[0]);
            if (y > x)
            {
                x = y;
                i = 1;
                j = 0;
            }
            y = fabs(row1[1]);
            if (y > x)
            {
                x = y;
                i = 1;
                j = 1;
            }
            y = fabs(row1[2]);
            if (y > x)
            {
                x = y;
                i = 1;
                j = 2;
            }
            y = fabs(row2[0]);
            if (y > x)
            {
                x = y;
                i = 2;
                j = 0;
            }
            y = fabs(row2[1]);
            if (y > x)
            {
                x = y;
                i = 2;
                j = 1;
            }
            y = fabs(row2[2]);
            if (y > x)
            {
                x = y;
                i = 2;
                j = 2;
            }
            if (x == 0.0)
                return 0;
            maxpiv = minpiv = fabs(x);
            p0 = workarray;
            switch (i)
            {
            case 1: /* swap rows 0 and 1 */
                memcpy(p0, row1, sizeof_row);
                p0[3] = d1;
                p0 += 4;
                memcpy(p0, row0, sizeof_row);
                p0[3] = d0;
                p0 += 4;
                memcpy(p0, row2, sizeof_row);
                p0[3] = d2;
                break;
            case 2: /* swap rows 0 and 2 */
                memcpy(p0, row2, sizeof_row);
                p0[3] = d2;
                p0 += 4;
                memcpy(p0, row1, sizeof_row);
                p0[3] = d1;
                p0 += 4;
                memcpy(p0, row0, sizeof_row);
                p0[3] = d0;
                break;
            default:
                memcpy(p0, row0, sizeof_row);
                p0[3] = d0;
                p0 += 4;
                memcpy(p0, row1, sizeof_row);
                p0[3] = d1;
                p0 += 4;
                memcpy(p0, row2, sizeof_row);
                p0[3] = d2;
                break;
            }
            switch (j)
            {
            case 1: /* swap columns 0 and 1 */
                p0 = x_addr;
                x_addr = y_addr;
                y_addr = p0;
                p0 = &workarray[0];
                x = p0[0];
                p0[0] = p0[1];
                p0[1] = x;
                p0 += 4;
                x = p0[0];
                p0[0] = p0[1];
                p0[1] = x;
                p0 += 4;
                x = p0[0];
                p0[0] = p0[1];
                p0[1] = x;
                break;
            case 2: /* swap columns 0 and 2 */
                p0 = x_addr;
                x_addr = z_addr;
                z_addr = p0;
                p0 = &workarray[0];
                x = p0[0];
                p0[0] = p0[2];
                p0[2] = x;
                p0 += 4;
                x = p0[0];
                p0[0] = p0[2];
                p0[2] = x;
                p0 += 4;
                x = p0[0];
                p0[0] = p0[2];
                p0[2] = x;
                break;
            }

            x = 1.0 / workarray[0];
            /* debugger set workarray[0] = 1 */
            p0 = p1 = workarray + 1;
            *p1++ *= x;
            *p1++ *= x;
            *p1++ *= x;
            x = -(*p1++);
            /* debugger set workarray[4] = 0 */
            if (x == 0.0)
                p1 += 3;
            else
            {
                *p1++ += x * (*p0++);
                *p1++ += x * (*p0++);
                *p1++ += x * (*p0);
                p0 -= 2;
            }
            x = -(*p1++);
            /* debugger set workarray[8] = 0 */
            if (x != 0.0)
            {
                *p1++ += x * (*p0++);
                *p1++ += x * (*p0++);
                *p1++ += x * (*p0);
                p0 -= 2;
            }

            x = fabs(workarray[5]);
            i = j = 0;
            y = fabs(workarray[6]);
            if (y > x)
            {
                x = y;
                j = 1;
            }
            y = fabs(workarray[9]);
            if (y > x)
            {
                x = y;
                i = 1;
                j = 0;
            }
            y = fabs(workarray[10]);
            if (y > x)
            {
                x = y;
                i = j = 1;
            }
            if (x == 0.0)
                return 1; // rank = 1;
            y = fabs(x);
            if (y > maxpiv)
                maxpiv = y;
            else if (y < minpiv)
                minpiv = y;
            if (j)
            {
                /* swap columns 1 and 2 */
                p0 = workarray + 1;
                p1 = p0 + 1;
                x = *p0;
                *p0 = *p1;
                *p1 = x;
                p0 += 4;
                p1 += 4;
                x = *p0;
                *p0 = *p1;
                *p1 = x;
                p0 += 4;
                p1 += 4;
                x = *p0;
                *p0 = *p1;
                *p1 = x;
                p0 += 4;
                p1 += 4;
                p0 = y_addr;
                y_addr = z_addr;
                z_addr = p0;
            }

            if (i)
            {
                /* pivot is in row 2 */
                p0 = workarray + 1;
                p1 = p0 + 8;
                p2 = p0 + 4;
            }
            else
            {
                /* pivot is in row 1 */
                p0 = workarray + 1;
                p1 = p0 + 4;
                p2 = p0 + 8;
            }

            /* debugger set workarray[5+4*i] = 1 */
            x = 1.0 / (*p1++);
            *p1++ *= x;
            *p1 *= x;
            p1--;
            x = -(*p0++);
            /* debugger set p0[-1] = 0 */
            if (x != 0.0)
            {
                *p0++ += x * (*p1++);
                *p0 += x * (*p1);
                p0--;
                p1--;
            }
            x = -(*p2++);
            /* debugger set p2[-1] = 0 */
            if (x != 0.0)
            {
                *p2++ += x * (*p1++);
                *p2 += x * (*p1);
                p2--;
                p1--;
            }
            x = *p2++;
            if (x == 0.0)
                return 2; // rank = 2;
            y = fabs(x);
            if (y > maxpiv)
                maxpiv = y;
            else if (y < minpiv)
                minpiv = y;
            /* debugger set p2[-1] = 1 */
            *p2 /= x;
            x = -(*p1++);
            if (x != 0.0)
                *p1 += x * (*p2);
            /* debugger set p1[-1] = 0 */
            x = -(*p0++);
            if (x != 0.0)
                *p0 += x * (*p2);
            /* debugger set p0[-1] = 0 */
            *x_addr = workarray[3];
            if (i)
            {
                *y_addr = workarray[11];
                *z_addr = workarray[7];
            }
            else
            {
                *y_addr = workarray[7];
                *z_addr = workarray[11];
            }
            *pivot_ratio = minpiv / maxpiv;
            return 3;
        }

        double value_at(const IK::Plane_3 &Pl, const IK::Point_3 &P)
        {
            return (Pl.a() * P.hx() + Pl.b() * P.hy() + Pl.c() * P.hz() + Pl.d());
        }
    }

    bool line_line_3d(const IK::Segment_3 &cutter_line, const IK::Segment_3 &segment, IK::Point_3 &output)
    {
        IK::Plane_3 plane(cutter_line[0], CGAL::cross_product(cutter_line.to_vector(), segment.to_vector()));
        CGAL::Aff_transformation_3<IK> xform = internal::plane_to_xy(cutter_line[0], plane);
        CGAL::Aff_transformation_3<IK> xform_Inv = xform.inverse();

        IK::Point_3 p0_0 = xform.transform(cutter_line[0]);
        IK::Point_3 p0_1 = xform.transform(cutter_line[1]);

        IK::Point_3 p1_0 = xform.transform(segment[0]);
        IK::Point_3 p1_1 = xform.transform(segment[1]);

        IK::Line_2 l0(IK::Point_2(p0_0.hx(), p0_0.hy()), IK::Point_2(p0_1.hx(), p0_1.hy()));
        IK::Line_2 l1(IK::Point_2(p1_0.hx(), p1_0.hy()), IK::Point_2(p1_1.hx(), p1_1.hy()));

        // Colinearity check!!!!!!!!

        // ERRROR THIS PLANE-PLANE-PLANE IS NOT WORKING
        IK::Plane_3 plane_2d_0(p0_0, CGAL::cross_product(IK::Vector_3(0, 0, 1), p0_1 - p0_0));
        IK::Plane_3 plane_2d_1(p1_0, CGAL::cross_product(IK::Vector_3(0, 0, 1), p1_1 - p1_0));
        IK::Plane_3 plane_2d_2(p0_0, IK::Vector_3(0, 0, 1));

        IK::Point_3 output_point;
        bool result = plane_plane_plane_with_parallel_check(plane_2d_0, plane_2d_1, plane_2d_2, output_point);
        if (result)
        {
            output = output_point;
            output = xform_Inv.transform(output);
            return true;
        }
        else
        {
            return false;
        }

        // const auto result = CGAL::intersection(l0, l1);

        // if (result)
        // {
        //     if (const IK::Point_2 *p = boost::get<IK::Point_2>(&*result))
        //     {
        //         output = IK::Point_3(p->hx(), p->hy(), 0);
        //         output = xform_Inv.transform(output);
        //     }
        //     else
        //     {
        //         return false;
        //     }
        // }
        // else
        // {
        //     return false;
        // }

        return true;
    }

    bool plane_plane_plane(const IK::Plane_3 &plane_0, const IK::Plane_3 &plane_1, const IK::Plane_3 &plane_2, IK::Point_3 &output)
    {
        double pr = 0.0;
        double x, y, z;

        const double plane_0_[3] = {plane_0.a(), plane_0.b(), plane_0.c()};
        const double plane_1_[3] = {plane_1.a(), plane_1.b(), plane_1.c()};
        const double plane_2_[3] = {plane_2.a(), plane_2.b(), plane_2.c()};

        const int rank = internal::solve3x3(
            //&plane_0.plane_equation.x, &plane_1.plane_equation.x, &plane_2.plane_equation.x,
            plane_0_, plane_1_, plane_2_,
            -plane_0.d(), -plane_1.d(), -plane_2.d(),
            &x, &y, &z, &pr);
        output = IK::Point_3(x, y, z);
        return (rank == 3) ? true : false;
    }

    bool plane_plane(const IK::Plane_3 &plane_0, const IK::Plane_3 &plane_1, IK::Line_3 &output)
    {
        // The third plane is computed by the cross product of the first two
        IK::Vector_3 d = CGAL::cross_product(plane_1.orthogonal_vector(), plane_0.orthogonal_vector());

        // Since the line is infinite, the origin of the third plane is irrelevant
        IK::Point_3 p = CGAL::midpoint(plane_0.point(), plane_1.point());
        IK::Plane_3 plane_2(p, d);

        // 3 plane intersection
        IK::Point_3 output_p;
        bool rc = plane_plane_plane_with_parallel_check(plane_0, plane_1, plane_2, output_p);

        // infinite line by give the direction of the 3-plane intersection as the cross product of the first two planes
        output = IK::Line_3(output_p, output_p + d);

        return rc;
    }

    bool line_plane(const IK::Segment_3 &line, const IK::Plane_3 &plane, IK::Point_3 &output, bool is_finite)
    {
        bool rc = false;
        double a, b, d, fd, t;

        auto pt0 = line[0];
        auto pt1 = line[1];
        a = internal::value_at(plane, pt0);
        b = internal::value_at(plane, pt1);
        d = a - b;
        if (d == 0.0)
        {
            if (fabs(a) < fabs(b))
                t = 0.0;
            else if (fabs(b) < fabs(a))
                t = 1.0;
            else
                t = 0.5;
        }
        else
        {
            d = 1.0 / d;
            fd = fabs(d);
            if (fd > 1.0 && (fabs(a) >= ON_DBL_MAX / fd || fabs(b) >= ON_DBL_MAX / fd))
            {
                // double overflow - line may be (nearly) parallel to plane
                t = 0.5;
            }
            else
            {
                t = a / (a - b); // = a*d;  a/(a-b) has more precision than a*d
                rc = true;
            }
        }

        // if (line_parameter)
        //     *line_parameter = t;

        // s[0].z()
        //  26 Feb 2003 Dale Lear
        //      Changed
        //           return (1-t)*from + t*to;
        //      to the following so that axis aligned lines will
        //      return exact answers for large values of t.
        //      See RR 9683.

        const double s = 1.0 - t;

        output = IK::Point_3(
            (line[0].x() == line[1].x()) ? line[0].x() : s * line[0].x() + t * line[1].x(),
            (line[0].y() == line[1].y()) ? line[0].y() : s * line[0].y() + t * line[1].y(),
            (line[0].z() == line[1].z()) ? line[0].z() : s * line[0].z() + t * line[1].z());

        if (is_finite && (t < 0 || t > 1))
            return false;

        return rc;
    }

    void line_plane(const IK::Line_3 &line, const IK::Plane_3 &plane, IK::Point_3 &output, bool is_finite)
    {
        auto segment = IK::Segment_3(line.point(), line.point() + line.to_vector());
        line_plane(segment, plane, output, is_finite);
    }

    bool get_quad_from_line_topbottomplanes(const IK::Plane_3 &collision_face, const IK::Segment_3 &line, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, CGAL_Polyline &output)
    {
        IK::Vector_3 dir = line.to_vector();
        IK::Plane_3 line_pl0(line[0], dir);
        IK::Plane_3 line_pl1(line[1], dir);

        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane_with_parallel_check(line_pl0, plane0, collision_face, p0);
        bool result1 = plane_plane_plane_with_parallel_check(line_pl0, plane1, collision_face, p1);
        bool result2 = plane_plane_plane_with_parallel_check(line_pl1, plane1, collision_face, p2);
        bool result3 = plane_plane_plane_with_parallel_check(line_pl1, plane0, collision_face, p3);
        output = {p0, p1, p2, p3, p0};

        return result0 && result1 && result2 && result3;
    }

    bool scale_vector_to_distance_of_2planes(const IK::Vector_3 &dir, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, IK::Vector_3 &output)
    {

        // check validity
        bool dirSet = (std::abs(dir.hx()) + std::abs(dir.hy()) + std::abs(dir.hz())) > wood_globals::DISTANCE; // == CGAL::NULL_VECTOR;
        if (!dirSet)
            return dirSet;

        // scale a vector to the length of the distance between the two planes
        IK::Segment_3 line(IK::Point_3(0, 0, 0), IK::Point_3(dir.hx(), dir.hy(), dir.hz()));
        IK::Point_3 p0;
        IK::Point_3 p1;
        line_plane(line, plane0, p0);
        line_plane(line, plane1, p1);
        output = p1 - p0;

        // Check scale, if the output_vector_length / plane_ortho_length < 10, then the vector is valid
        double dist_ortho = (plane1.projection(plane0.point()) - plane0.point()).squared_length();
        double dist = output.squared_length();
        dirSet = dist / dist_ortho < 10;
        return dirSet;
    }

    bool get_orthogonal_vector_between_two_plane_pairs(const IK::Plane_3 &plane_pair0_0, const IK::Plane_3 &plane_pair1_0, const IK::Plane_3 &plane_pair1_1, IK::Vector_3 &output)
    {
        IK::Line_3 l0;
        IK::Line_3 l1;
        bool result0 = plane_plane(plane_pair0_0, plane_pair1_0, l0);
        bool result1 = plane_plane(plane_pair0_0, plane_pair1_1, l1);

        output = l1.point() - l0.projection(l1.point());
        return result0 && result1;
    }

    bool plane_4planes(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output)
    {
        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane_with_parallel_check(sequence_of_planes[0], sequence_of_planes[1], main_plane, p0);
        bool result1 = plane_plane_plane_with_parallel_check(sequence_of_planes[1], sequence_of_planes[2], main_plane, p1);
        bool result2 = plane_plane_plane_with_parallel_check(sequence_of_planes[2], sequence_of_planes[3], main_plane, p2);
        bool result3 = plane_plane_plane_with_parallel_check(sequence_of_planes[3], sequence_of_planes[0], main_plane, p3);
        output = {p0, p1, p2, p3, p0};
        return result0 && result1 && result2 && result3;
    }

    bool plane_4planes_open(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output)
    {
        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane_with_parallel_check(sequence_of_planes[0], sequence_of_planes[1], main_plane, p0);
        bool result1 = plane_plane_plane_with_parallel_check(sequence_of_planes[1], sequence_of_planes[2], main_plane, p1);
        bool result2 = plane_plane_plane_with_parallel_check(sequence_of_planes[2], sequence_of_planes[3], main_plane, p2);
        bool result3 = plane_plane_plane_with_parallel_check(sequence_of_planes[3], sequence_of_planes[0], main_plane, p3);
        output = {p0, p1, p2, p3};
        return result0 && result1 && result2 && result3;
    }

    bool plane_4lines(IK::Plane_3 &plane, IK::Segment_3 &l0, IK::Segment_3 &l1, IK::Segment_3 &l2, IK::Segment_3 &l3, CGAL_Polyline &output)
    {
        IK::Point_3 p0, p1, p2, p3;
        bool result0 = line_plane(l0, plane, p0);
        bool result1 = line_plane(l1, plane, p1);
        bool result2 = line_plane(l2, plane, p2);
        bool result3 = line_plane(l3, plane, p3);
        output = {p0, p1, p2, p3, p0};
        return result0 && result1 && result2 && result3;
    }

    bool plane_plane_plane_with_parallel_check(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, const IK::Plane_3 &plane2, IK::Point_3 &output)
    {

        auto plane0_v = plane0.orthogonal_vector();
        auto plane1_v = plane1.orthogonal_vector();
        auto plane2_v = plane2.orthogonal_vector();

        // check if the plane normals are not parallel
        bool flag = internal::is_parallel_to(plane0_v, plane1_v) == 0 &&
                    internal::is_parallel_to(plane0_v, plane2_v) == 0 &&
                    internal::is_parallel_to(plane1_v, plane2_v) == 0;

        if (flag)
            flag = plane_plane_plane(plane0, plane1, plane2, output);

        return flag;
    }

    bool line_two_planes(IK::Segment_3 &line, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1)
    {

        IK::Point_3 output_point_0;
        bool result_0 = line_plane(line, plane0, output_point_0, true);

        IK::Point_3 output_point_1;
        bool result_1 = line_plane(line, plane1, output_point_1, true);

        if (result_0 && result_1)
        {
            line = IK::Segment_3(output_point_0, output_point_1);
            return true;
        }
        else
            return false;

        // line = IK::Segment_3(*boost::get<IK::Point_3>(&*result0), *boost::get<IK::Point_3>(&*result1));

        // if (result)
        // {
        //     // std::cout << output_point.hx() << " " << output_point.hy() << " " << output_point.hz() << std::endl;
        //     points.emplace_back(output_point);
        //     edge_ids.emplace_back(i);
        // }

        // auto result0 = CGAL::intersection(line, plane0);
        // auto result1 = CGAL::intersection(line, plane1);
        // line = IK::Segment_3(*boost::get<IK::Point_3>(&*result0), *boost::get<IK::Point_3>(&*result1));
        // return true;
    }

    bool polyline_plane_to_line(const CGAL_Polyline &polyline, const IK::Plane_3 &plane, IK::Segment_3 &alignment_segment, IK::Segment_3 &output)
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Intersect polyline segments one by one, until two points are found, if any
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 pts[2];
        int count = 0;

        for (int i = 0; i < polyline.size() - 1; i++)
        {

            IK::Segment_3 segment(polyline[i], polyline[i + 1]);

            IK::Point_3 output_point_0;
            bool result_0 = line_plane(segment, plane, output_point_0, true);

            if (result_0)
            {
                pts[count] = output_point_0;
                count++;
                if (count == 2)
                    break;
            }

            // auto output = CGAL::intersection(segment, plane);

            // if (output)
            // {
            //     if (const IK::Point_3 *p = boost::get<IK::Point_3>(&*output))
            //     {
            //         pts[count] = *p;
            //         count++;
            //         if (count == 2)
            //             break;
            //     } // if point type
            // }     // output exists
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Align intersection points according to an average polygon edge direction
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (count == 2)
        {
            if (CGAL::squared_distance(alignment_segment[0], pts[0]) < CGAL::squared_distance(alignment_segment[0], pts[1]))
                output = IK::Segment_3(pts[0], pts[1]);
            else
                output = IK::Segment_3(pts[1], pts[0]);

            return true;
        }
        else
        {
            return false;
        }
    }

    bool polyline_plane(const CGAL_Polyline &polyline, const IK::Plane_3 &plane, std::vector<IK::Point_3> &points, std::vector<int> &edge_ids)
    {

        for (auto i = 0; i < polyline.size() - 1; i++)
        {
            IK::Segment_3 segment(polyline[i], polyline[i + 1]);

            if (CGAL::squared_distance(plane.projection(segment[0]), segment[0]) < wood_globals::DISTANCE_SQUARED)
            {
                // std::cout << "polylines are coincident \n";
                return false;
            }

            if (CGAL::squared_distance(plane.projection(segment[1]), segment[1]) < wood_globals::DISTANCE_SQUARED)
            {
                // std::cout << "polylines are coincident \n";
                return false;
            }
            IK::Point_3 output_point;
            bool result = line_plane(segment, plane, output_point, true);

            if (result)
            {
                // std::cout << output_point.hx() << " " << output_point.hy() << " " << output_point.hz() << std::endl;
                points.emplace_back(output_point);
                edge_ids.emplace_back(i);
            }

        } // if point type

        // std::cout << "points.size() " << points.size() << std::endl;
        return points.size() == 2;
    }

    bool polyline_plane_cross_joint(CGAL_Polyline &c0, CGAL_Polyline &c1, IK::Plane_3 &p0, IK::Plane_3 &p1, IK::Segment_3 &line, std::pair<int, int> &pair)
    {
        bool debug = false;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Perform both events
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> pts0;
        std::vector<int> edge_ids_0;
        if (!polyline_plane(c0, p1, pts0, edge_ids_0))
        {
            if (debug)
            {
                for (auto &p_temp : c0)
                    std::cout << (p_temp) << std::endl;
                std::cout << " " << std::endl;
                for (auto &p_temp : c1)
                    std::cout << (p_temp) << std::endl;
                printf("CPP cgal_intersection_util -> polyline_plane(c0, p1, pts0, edge_ids_0) \n");
            }

            return false;
        }

        std::vector<IK::Point_3> pts1;
        std::vector<int> edge_ids_1;
        if (!polyline_plane(c1, p0, pts1, edge_ids_1))
        {
            if (debug)
                printf("CPP polyline_plane(c1, p0, pts1, edge_ids_1) \n");
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check1: if there are 2 intersections
        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (pts0.size() < 2)
        {
            if (debug)
                printf("CPP PlanePolylineIntersection 0 Not2 \n");
            return false;
        }

        if (pts1.size() < 2)
        {
            if (debug)
                printf("CPP PlanePolylineIntersection 1 Not2 \n");
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check2: if any of points are in the other curve,
        // If one point is inside, perform second intersection else curve are inside curves
        // What if count is not 1 but 2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<int> ID1;
        int count0 = clipper_util::are_points_inside(c0, p0, pts1, ID1);

        std::vector<int> ID0;
        int count1 = clipper_util::are_points_inside(c1, p1, pts0, ID0);

        if (count0 == 0 && count1 == 0)
        {
            return false;
        }
        else if (std::abs(count0 - count1) == 2)
        { // rectangle curve inside a rectangle curve
            line = count0 == 2 ? IK::Segment_3(pts0[0], pts0[1]) : IK::Segment_3(pts1[0], pts1[1]);
            pair = count0 == 2 ? std::pair<int, int>(edge_ids_0[0], edge_ids_0[1]) : std::pair<int, int>(edge_ids_1[0], edge_ids_1[1]);

            return true;
        }
        else if (count0 == 1 && count1 == 1)
        {
            line = IK::Segment_3(pts0[ID0[0]], pts1[ID1[0]]);
            pair = std::pair<int, int>(edge_ids_0[ID0[0]], edge_ids_1[ID1[0]]);
            return true;
        }
        else if (count0 > 1 || count1 > 1)
        {
            std::vector<IK::Point_3> pts(ID0.size() + ID1.size());
            for (int i = 0; i < ID0.size(); i++)
                pts[i] = pts0[ID0[i]];

            for (int i = 0; i < ID1.size(); i++)
                pts[i + ID0.size()] = pts1[ID1[i]];

            CGAL::Bbox_3 AABB = CGAL::bbox_3(pts.begin(), pts.end(), IK());
            auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
            auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
            line = IK::Segment_3(p0, p1);
            int e0, e1;

            // Find edge ID - not optimized...
            for (int i = 0; i < ID0.size(); i++)
            {
                if (CGAL::squared_distance(p0, pts0[ID0[i]]) < 0.001 || CGAL::squared_distance(p1, pts0[ID0[i]]) < 0.001)
                {
                    e0 = edge_ids_0[ID0[i]];
                    break;
                }
            }

            for (int i = 0; i < ID1.size(); i++)
            {
                if (CGAL::squared_distance(p0, pts1[ID1[i]]) < 0.001 || CGAL::squared_distance(p1, pts1[ID1[i]]) < 0.001)
                {
                    e1 = edge_ids_1[ID1[i]];
                    break;
                }
            }
            pair = std::pair<int, int>(e0, e1);

            return true;
        }

        return false;
    }
}
