#pragma once

// #include "../../stdafx.h"

// PlanePlanePlane
// PlanePlane
// LinePlane
//  LineLine not implemented
// https://github.com/mcneel/opennurbs/blob/c20e599d1ff8f08a55d3dddf5b39e37e8b5cac06/opennurbs_intersect.cpp
namespace cgal_intersection_util
{
    inline IK::Point_3 point_at(const IK::Segment_3 &l, double t)
    {
        const double s = 1.0 - t;

        return IK::Point_3(
            (l[0].x() == l[1].x()) ? l[0].x() : s * l[0].x() + t * l[1].x(),
            (l[0].y() == l[1].y()) ? l[0].y() : s * l[0].y() + t * l[1].y(),
            (l[0].z() == l[1].z()) ? l[0].z() : s * l[0].z() + t * l[1].z());
    }

    inline bool closest_point_to(const IK::Point_3 &point, const IK::Segment_3 &s, double &t)
    {
        bool rc = false;

        const IK::Vector_3 D = s.to_vector();
        const double DoD = D.squared_length();

        if (DoD > 0.0)
        {
            if ((point - s[0]).squared_length() <= (point - s[1]).squared_length())
                t = ((point - s[0]) * D) / DoD;
            else
                t = 1.0 + ((point - s[1]) * D) / DoD;

            rc = true;
        }
        else
        { // (GBA) Closest point to a degenerate line works as well
            t = 0.0;
            rc = true;
        }

        return rc;
    }

    inline CGAL::Aff_transformation_3<IK> PlaneToXY(
        IK::Point_3 O0, IK::Plane_3 plane)
    {
        auto X0 = plane.base1();
        auto Y0 = plane.base2();
        auto Z0 = plane.orthogonal_vector();
        cgal_vector_util::Unitize(X0);
        cgal_vector_util::Unitize(Y0);
        cgal_vector_util::Unitize(Z0);

        // transformation maps P0 to P1, P0+X0 to P1+X1, ...

        // Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

        // Rotate ->
        CGAL::Aff_transformation_3<IK> F0(
            X0.x(), X0.y(), X0.z(),
            Y0.x(), Y0.y(), Y0.z(),
            Z0.x(), Z0.y(), Z0.z());

        return F0 * T0;
    }

    // inline bool line_line_intersection(IK::Segment_3& s0, IK::Segment_3& s1, bool finite, IK::Point_3& p0, IK::Point_3& p1) {
    //     //Unlike lines, segments don’t extend beyond their start and end points.

    //     IK::Vector_3 v0 = s0.to_vector();
    //     IK::Vector_3 v1 = s1.to_vector();
    //     IK::Vector_3 normal = CGAL::cross_product(v0, v1);

    //     double angle = 90.0 - fabs(CGAL::approximate_angle(v0, v1) - 90);

    //     //The 2 intersection points of intersecting segments are identical.
    //     if (CGAL::squared_distance(s0[0], s1[0]) < wood_globals::DISTANCE || CGAL::squared_distance(s0[0], s1[1]) < wood_globals::DISTANCE) {
    //         p0 = s0[0];
    //         p1 = s0[0];
    //         return true;
    //     }
    //     else if (CGAL::squared_distance(s0[1], s1[0]) < wood_globals::DISTANCE || CGAL::squared_distance(s0[1], s1[1]) < wood_globals::DISTANCE) {
    //         p0 = s0[1];
    //         p1 = s0[1];
    //         return true;
    //     }

    //     // Collinear segments or //Parallel segments don’t intersect.

    //     if (normal == CGAL::NULL_VECTOR || angle < 1) {
    //         return false;

    //         IK::Line_3 l0 = s0.supporting_line();
    //         IK::Line_3 l1 = s1.supporting_line();
    //         IK::Vector_3 v0 = s0.to_vector();
    //         v0 = v0 / CGAL::approximate_sqrt(v0 * v0);
    //         IK::Vector_3 v1 = s1.to_vector();
    //         v1 = v1 / CGAL::approximate_sqrt(v1 * v1);

    //         std::vector<std::pair<IK::FT, IK::FT>> points_along_lines;

    //         auto create_projection = [&](const IK::Point_3& p)
    //         {
    //             IK::Point_3 p0 = l0.projection(p);
    //             IK::Vector_3 vec0(s0.source(), p0);
    //             IK::FT pos0 = vec0 * v0;

    //             IK::Point_3 p1 = l1.projection(p);
    //             IK::Vector_3 vec1(s1.source(), p1);
    //             IK::FT pos1 = vec1 * v1;

    //             points_along_lines.emplace_back(pos0, pos1);
    //         };
    //         create_projection(s0.source());
    //         create_projection(s0.target());
    //         create_projection(s1.source());
    //         create_projection(s1.target());

    //         std::sort(points_along_lines.begin(), points_along_lines.end(),
    //             [](const auto& a, const auto& b) -> bool
    //             {
    //                 return a.first < b.first;
    //             });

    //         IK::Segment_3 seg0(s0.source() + points_along_lines[1].first * v0,
    //             s0.source() + points_along_lines[2].first * v0);
    //         IK::Segment_3 seg1(s1.source() + points_along_lines[1].second * v1,
    //             s1.source() + points_along_lines[2].second * v1);

    //         //Not sure this is correct
    //         p0 = seg0[0];
    //         p1 = seg0[1];
    //         //return Result(seg0, seg1);
    //         return true;
    //     }
    //     else {//Skew segments have two different intersection points.
    //      //Co-linear - check ends

    //      //Works only if segments are not co-linear
    //         IK::Plane_3 plane(s0[0], normal);
    //         CGAL::Aff_transformation_3<IK> xform = PlaneToXY(s0[0], plane);
    //         CGAL::Aff_transformation_3<IK> xform_Inv = xform.inverse();

    //         IK::Point_3 p0_0 = xform.transform(s0[0]);
    //         IK::Point_3 p0_1 = xform.transform(s0[1]);

    //         IK::Point_3 p1_0 = xform.transform(s1[0]);
    //         IK::Point_3 p1_1 = xform.transform(s1[1]);

    //         IK::Line_2 l0(IK::Point_2(p0_0.hx(), p0_0.hy()), IK::Point_2(p0_1.hx(), p0_1.hy()));
    //         IK::Line_2 l1(IK::Point_2(p1_0.hx(), p1_0.hy()), IK::Point_2(p1_1.hx(), p1_1.hy()));
    //         IK::Line_3 l1_3d = s1.supporting_line();
    //         //CGAL_Debug(p0_0,true);
    //         //CGAL_Debug(p0_1, true);
    //         //CGAL_Debug(p1_0, true);
    //         //CGAL_Debug(p1_1, true);

    //         //Colinearity check!!!!!!!!

    //         const auto result = CGAL::intersection(l0, l1);

    //         if (result) {
    //             if (const IK::Point_2* p = boost::get<IK::Point_2 >(&*result)) {
    //                 p0 = IK::Point_3(p->hx(), p->hy(), 0);
    //                 p0 = xform_Inv.transform(p0);

    //                 if (finite) {
    //                     double t0, t1;
    //                     closest_point_to(p0, s0, t0);

    //                     bool outside0 = t0 > 1.0 || t0 < 0.0;

    //                     if (outside0) {
    //                         t0 = std::max(0.0, std::min(t0, 1.0));
    //                         p0 = point_at(s0, t0);
    //                     }

    //                     p1 = l1_3d.projection(p0);//is it ok?
    //                     closest_point_to(p1, s1, t1);

    //                     bool outside1 = t1 > 1.0 || t1 < 0.0;

    //                     if (outside1) {
    //                         t1 = std::max(0.0, std::min(t1, 1.0));
    //                         p1 = point_at(s1, t1);
    //                     }
    //                 }
    //             }
    //             else {
    //                 return false;
    //             }
    //         }
    //         else {
    //             return false;
    //         }
    //         return true;
    //     }
    //     return true;
    // }

   

    inline IK::Point_3 PointAt(IK::Segment_3 &l, double t)
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
            (l[0].x() == l[1].x()) ? l[0].x() : s * l[0].x() + t * l[1].x(),
            (l[0].y() == l[1].y()) ? l[0].y() : s * l[0].y() + t * l[1].y(),
            (l[0].z() == l[1].z()) ? l[0].z() : s * l[0].z() + t * l[1].z());
    }

    inline bool ClosestPointTo(const IK::Point_3 &point, IK::Segment_3 &s, double &t)
    {
        bool rc = false;
        // if (t) {
        const IK::Vector_3 D = s.to_vector();

        const double DoD = D.squared_length();
        // CGAL_Debug(DoD);

        if (DoD > 0.0)
        {
            if ((point - s[0]).squared_length() <= (point - s[1]).squared_length())
            {
                t = ((point - s[0]) * D) / DoD;
            }
            else
            {
                t = 1.0 + ((point - s[1]) * D) / DoD;
            }

            rc = true;
        }
        else
        {
            t = 0.0;
            rc = true; // (GBA) Closest point to a degenerate line works as well
        }
        // }
        return rc;
    }

    inline bool ClosestPointTo(const IK::Point_2 &point, const IK::Segment_2 &s, double &t)
    {
        bool rc = false;
        // if (t) {
        const IK::Vector_2 D = s.to_vector();

        const double DoD = D.squared_length();
        // CGAL_Debug(DoD);

        if (DoD > 0.0)
        {
            if ((point - s[0]).squared_length() <= (point - s[1]).squared_length())
            {
                t = ((point - s[0]) * D) / DoD;
            }
            else
            {
                t = 1.0 + ((point - s[1]) * D) / DoD;
            }

            rc = true;
        }
        else
        {
            t = 0.0;
            rc = true; // (GBA) Closest point to a degenerate line works as well
        }
        // }
        return rc;
    }

    inline bool LineLine3D(IK::Segment_3 &cutter_line, IK::Segment_3 &segment, IK::Point_3 &output)
    {
        IK::Plane_3 plane(cutter_line[0], CGAL::cross_product(cutter_line.to_vector(), segment.to_vector()));
        CGAL::Aff_transformation_3<IK> xform = PlaneToXY(cutter_line[0], plane);
        CGAL::Aff_transformation_3<IK> xform_Inv = xform.inverse();

        IK::Point_3 p0_0 = xform.transform(cutter_line[0]);
        IK::Point_3 p0_1 = xform.transform(cutter_line[1]);

        IK::Point_3 p1_0 = xform.transform(segment[0]);
        IK::Point_3 p1_1 = xform.transform(segment[1]);

        IK::Line_2 l0(IK::Point_2(p0_0.hx(), p0_0.hy()), IK::Point_2(p0_1.hx(), p0_1.hy()));
        IK::Line_2 l1(IK::Point_2(p1_0.hx(), p1_0.hy()), IK::Point_2(p1_1.hx(), p1_1.hy()));

        // Colinearity check!!!!!!!!

        const auto result = CGAL::intersection(l0, l1);

        if (result)
        {
            if (const IK::Point_2 *p = boost::get<IK::Point_2>(&*result))
            {
                output = IK::Point_3(p->hx(), p->hy(), 0);
                output = xform_Inv.transform(output);
                // cgal_intersection_util::ClosestPointTo(*p, cutter_line, t);
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        return true;
    }

    inline bool LineLine2D(IK::Segment_2 &cutter_line, IK::Segment_2 &segment, IK::Point_2 &output, double &t)
    {
        const auto result = CGAL::intersection(cutter_line, segment);

        if (result)
        {
            if (const IK::Point_2 *p = boost::get<IK::Point_2>(&*result))
            {
                output = *p;
                // cgal_intersection_util::ClosestPointTo(*p, cutter_line, t);
            }
        }

        return false;
    }

    inline void line_line_line(IK::Segment_3 &l0, IK::Segment_3 &middle, IK::Segment_3 &l1,
                               IK::Point_3 &p0, IK::Point_3 &p1,
                               double &middle_t_0, double &middle_t_1)
    {
        // IK::Line_3 l0_(IK::Point_3(-1242.9339929999999, -680.04568599999982, 711.26441699999987), IK::Point_3(-1740.1076069999997, -680.04568599999982, 711.26441699999987));
        // IK::Line_3 l1_(IK::Point_3(-1242.933992, -680.04568600000005, 711.26441699999998), IK::Point_3(-1242.933992, -196.286, 535.190291));
        //

        auto result0 = CGAL::intersection((l0.supporting_line()), (middle.supporting_line()));
        // auto result1 = CGAL::intersection(ie(l1), ie(middle));

        if (result0)
        {
            p0 = (*boost::get<IK::Point_3>(&*result0));
            // printf("Good\n");
            // CGAL_Debug(p0, true);
        }
        else
        {
            printf("Bad\n");
        }
    }

    inline int Solve3x3(

        const double (&row0)[3], const double (&row1)[3], const double (&row2)[3],
        double d0, double d1, double d2,
        double *x_addr, double *y_addr, double *z_addr,
        double *pivot_ratio)
    {
        /* Solve a 3x3 linear system using Gauss-Jordan elimination
         * with full pivoting.
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

    inline bool Intersect(IK::Plane_3 &R, IK::Plane_3 &S, IK::Plane_3 &T,
                          IK::Point_3 &P)
    {
        double pr = 0.0;
        double x, y, z;

        const double R_[3] = {R.a(), R.b(), R.c()};
        const double S_[3] = {S.a(), S.b(), S.c()};
        const double T_[3] = {T.a(), T.b(), T.c()};

        const int rank = Solve3x3(
            //&R.plane_equation.x, &S.plane_equation.x, &T.plane_equation.x,
            R_, S_, T_,
            -R.d(), -S.d(), -T.d(),
            &x, &y, &z, &pr);
        P = IK::Point_3(x, y, z);
        return (rank == 3) ? true : false;
    }

    inline bool Intersect(IK::Plane_3 &R, IK::Plane_3 &S, IK::Line_3 &L)
    {
        IK::Vector_3 d = CGAL::cross_product(S.orthogonal_vector(), R.orthogonal_vector());
        IK::Point_3 p = CGAL::midpoint(R.point(), S.point());
        IK::Plane_3 T(p, d);

        IK::Point_3 l0;
        bool rc = Intersect(R, S, T, l0);
        L = IK::Line_3(l0, l0 + d);

        return rc;
    }

    inline double ValueAt(IK::Plane_3 &Pl, IK::Point_3 &P)
    {
        return (Pl.a() * P.hx() + Pl.b() * P.hy() + Pl.c() * P.hz() + Pl.d());
    }

    inline bool Intersect(
        IK::Segment_3 &L,
        IK::Plane_3 &Pl,
        IK::Point_3 &P

        // double* line_parameter
    )
    {
        bool rc = false;
        double a, b, d, fd, t;

        auto pt0 = L[0];
        auto pt1 = L[1];
        a = ValueAt(Pl, pt0);
        b = ValueAt(Pl, pt1);
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

        P = IK::Point_3(
            (L[0].x() == L[1].x()) ? L[0].x() : s * L[0].x() + t * L[1].x(),
            (L[0].y() == L[1].y()) ? L[0].y() : s * L[0].y() + t * L[1].y(),
            (L[0].z() == L[1].z()) ? L[0].z() : s * L[0].z() + t * L[1].z());

        // if (finite && (t < 0 || t>1))
        //     return false;

        return rc;
    }

    inline bool QuadFromLineAndTopBottomPlanes(IK::Plane_3 &collision_face, IK::Segment_3 &l, IK::Plane_3 &pl0, IK::Plane_3 &pl1, CGAL_Polyline &quad)
    {
        IK::Vector_3 dir = l.to_vector();
        IK::Plane_3 line_pl0_EK(l[0], dir);
        IK::Plane_3 line_pl1_EK(l[1], dir);

        // IK_to_EK ie; EK_to_IK ei;
        // auto result0 = CGAL::intersection(ie(line_pl0_EK), ie(pl0), ie(collision_face));
        // auto result1 = CGAL::intersection(ie(line_pl0_EK), ie(pl1), ie(collision_face));
        // auto result2 = CGAL::intersection(ie(line_pl1_EK), ie(pl1), ie(collision_face));
        // auto result3 = CGAL::intersection(ie(line_pl1_EK), ie(pl0), ie(collision_face));
        // IK::Point_3 p0 = ei(*boost::get<EK::Point_3>(&*result0));
        // IK::Point_3 p1 = ei(*boost::get<EK::Point_3>(&*result1));
        // IK::Point_3 p2 = ei(*boost::get<EK::Point_3>(&*result2));
        // IK::Point_3 p3 = ei(*boost::get<EK::Point_3>(&*result3));

        IK::Point_3 p0;
        cgal_intersection_util::Intersect(line_pl0_EK, pl0, collision_face, p0);
        IK::Point_3 p1;
        cgal_intersection_util::Intersect(line_pl0_EK, pl1, collision_face, p1);
        IK::Point_3 p2;
        cgal_intersection_util::Intersect(line_pl1_EK, pl1, collision_face, p2);
        IK::Point_3 p3;
        cgal_intersection_util::Intersect(line_pl1_EK, pl0, collision_face, p3);
        quad = {p0, p1, p2, p3, p0};

        return true;
    }

    inline bool vector_two_planes(IK::Vector_3 &dir, IK::Plane_3 &plane0, IK::Plane_3 &plane1, IK::Vector_3 &output)
    {
        // printf("vector_two_planes \n");
        // IK::Line_3 line(IK::Point_3(0, 0, 0), v);

        // auto result0 = CGAL::intersection(plane0, line);
        // auto result1 = CGAL::intersection(plane1, line);

        // auto p0 = *boost::get<IK::Point_3>(&*result0);
        // auto p1 = *boost::get<IK::Point_3>(&*result1);

        // return p1 - p0;

        // check validity
        bool dirSet = (std::abs(dir.hx()) + std::abs(dir.hy()) + std::abs(dir.hz())) > wood_globals::DISTANCE; // == CGAL::NULL_VECTOR;
        if (!dirSet)
            return dirSet;

        IK::Segment_3 line(IK::Point_3(0, 0, 0), IK::Point_3(dir.hx(), dir.hy(), dir.hz()));
        IK::Point_3 p0;
        IK::Point_3 p1;
        cgal_intersection_util::Intersect(line, plane0, p0);
        cgal_intersection_util::Intersect(line, plane1, p1);
        output = p1 - p0;

        // Check scale
        double dist_ortho = (plane1.projection(plane0.point()) - plane0.point()).squared_length();
        double dist = output.squared_length();
        dirSet = dist / dist_ortho < 10;
        return dirSet;
        // return true;
    }

    inline void orthogonal_vector_between_two_plane_pairs(IK::Plane_3 &plane_pair0_0, IK::Plane_3 &plane_pair1_0, IK::Plane_3 &plane_pair1_1, IK::Vector_3 &output)
    {
        IK::Line_3 l0;
        IK::Line_3 l1;
        cgal_intersection_util::Intersect(plane_pair0_0, plane_pair1_0, l0);
        cgal_intersection_util::Intersect(plane_pair0_0, plane_pair1_1, l1);

        output = l1.point() - l0.projection(l1.point());
    }

    inline void plane_4_planes(IK::Plane_3 &mainPlane, IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &quad)
    {
        // auto result0 = CGAL::intersection(sequence_of_planes[0], sequence_of_planes[1], mainPlane);
        // auto result1 = CGAL::intersection(sequence_of_planes[1], sequence_of_planes[2], mainPlane);
        // auto result2 = CGAL::intersection(sequence_of_planes[2], sequence_of_planes[3], mainPlane);
        // auto result3 = CGAL::intersection(sequence_of_planes[3], sequence_of_planes[0], mainPlane);

        // return {
        //  *boost::get<IK::Point_3>(&*result0),
        //*boost::get<IK::Point_3>(&*result1),
        //*boost::get<IK::Point_3>(&*result2),
        //*boost::get<IK::Point_3>(&*result3),
        //*boost::get<IK::Point_3>(&*result0)
        // };

        IK::Point_3 p0;
        cgal_intersection_util::Intersect(sequence_of_planes[0], sequence_of_planes[1], mainPlane, p0);
        IK::Point_3 p1;
        cgal_intersection_util::Intersect(sequence_of_planes[1], sequence_of_planes[2], mainPlane, p1);
        IK::Point_3 p2;
        cgal_intersection_util::Intersect(sequence_of_planes[2], sequence_of_planes[3], mainPlane, p2);
        IK::Point_3 p3;
        cgal_intersection_util::Intersect(sequence_of_planes[3], sequence_of_planes[0], mainPlane, p3);
        quad = {p0, p1, p2, p3, p0};
    }

    inline void plane_4_planes_open(IK::Plane_3 &mainPlane, IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &quad)
    {
        // auto result0 = CGAL::intersection(sequence_of_planes[0], sequence_of_planes[1], mainPlane);
        // auto result1 = CGAL::intersection(sequence_of_planes[1], sequence_of_planes[2], mainPlane);
        // auto result2 = CGAL::intersection(sequence_of_planes[2], sequence_of_planes[3], mainPlane);
        // auto result3 = CGAL::intersection(sequence_of_planes[3], sequence_of_planes[0], mainPlane);

        // return {
        //  *boost::get<IK::Point_3>(&*result0),
        //*boost::get<IK::Point_3>(&*result1),
        //*boost::get<IK::Point_3>(&*result2),
        //*boost::get<IK::Point_3>(&*result3),
        //*boost::get<IK::Point_3>(&*result0)
        // };

        IK::Point_3 p0;
        cgal_intersection_util::Intersect(sequence_of_planes[0], sequence_of_planes[1], mainPlane, p0);
        IK::Point_3 p1;
        cgal_intersection_util::Intersect(sequence_of_planes[1], sequence_of_planes[2], mainPlane, p1);
        IK::Point_3 p2;
        cgal_intersection_util::Intersect(sequence_of_planes[2], sequence_of_planes[3], mainPlane, p2);
        IK::Point_3 p3;
        cgal_intersection_util::Intersect(sequence_of_planes[3], sequence_of_planes[0], mainPlane, p3);
        quad = {p0, p1, p2, p3};
    }

    inline void LinePlane(IK::Line_3 &line, IK::Plane_3 &plane, IK::Point_3 &output)
    {
        auto segment = IK::Segment_3(line.point(), line.point() + line.to_vector());
        cgal_intersection_util::Intersect(segment, plane, output);

        // auto result = CGAL::intersection(line, plane);
        ///*    if (!result)
        //        CGAL_Debug(0);*/
        // auto point = *boost::get<IK::Point_3>(&*result);
        // return point;
    }

    inline void SegmentPlane(IK::Segment_3 &segment, IK::Plane_3 &plane, IK::Point_3 &output)
    {
        cgal_intersection_util::Intersect(segment, plane, output);
        /* if (!result)
             CGAL_Debug(0);*/
        // auto point = *boost::get<IK::Point_3>(&*result);
    }

    inline bool Plane4LinesIntersection(IK::Plane_3 &plane, IK::Segment_3 &l0, IK::Segment_3 &l1, IK::Segment_3 &l2, IK::Segment_3 &l3, CGAL_Polyline &output)
    {
        // printf("CPP %f \n", l0.squared_length());
        // printf("CPP %f  %f %f %f \n", plane.a(), plane.b(), plane.c(), plane.d() );

        IK::Point_3 p0;
        cgal_intersection_util::Intersect(l0, plane, p0);
        IK::Point_3 p1;
        cgal_intersection_util::Intersect(l1, plane, p1);
        IK::Point_3 p2;
        cgal_intersection_util::Intersect(l2, plane, p2);
        IK::Point_3 p3;
        cgal_intersection_util::Intersect(l3, plane, p3);
        output = {p0, p1, p2, p3, p0};
        // auto result0 = CGAL::intersection(l0.supporting_line(), plane);
        ////if (!result0) printf("CPP result0");

        // auto result1 = CGAL::intersection(l1.supporting_line(), plane);
        ////if (!result1)printf("CPP result1");

        // auto result2 = CGAL::intersection(l2.supporting_line(), plane);
        ////if (!result2) printf("CPP result2");

        // auto result3 = CGAL::intersection(l3.supporting_line(), plane);
        ////if (!result3)  printf("CPP result3");

        // output = {
        //*boost::get<IK::Point_3>(&*result0),
        //*boost::get<IK::Point_3>(&*result1),
        //*boost::get<IK::Point_3>(&*result2),
        //*boost::get<IK::Point_3>(&*result3),
        //*boost::get<IK::Point_3>(&*result0)
        // };

        return true;
    }

    inline bool PlaneLineIntersection(IK::Plane_3 &plane, IK::Segment_3 &l0, IK::Point_3 &output)
    {
        return cgal_intersection_util::Intersect(l0, plane, output);
        // auto result0 = CGAL::intersection(l0, plane);
        // output = *boost::get<IK::Point_3>(&*result0);
        // return true;
    }

    inline bool plane_plane_plane(IK::Plane_3 &plane0, IK::Plane_3 &plane1, IK::Plane_3 &plane2, IK::Point_3 &p0, IK::Segment_3 &segment, double &t
                                  /*IK::Point_3& p0, IK::Point_3& p1,
                          double& middle_t_0, double& middle_t_1*/
    )
    {
        // CGAL_Debug(cgal_vector_util::IsParallelTo(plane0.orthogonal_vector(), plane1.orthogonal_vector(),0.11));

        auto plane0_v = plane0.orthogonal_vector();
        auto plane1_v = plane1.orthogonal_vector();
        auto plane2_v = plane2.orthogonal_vector();
        bool flag = cgal_vector_util::IsParallelTo(plane0_v, plane1_v, 0.11) == 0 &&
                    cgal_vector_util::IsParallelTo(plane0_v, plane2_v, 0.11) == 0 &&
                    cgal_vector_util::IsParallelTo(plane1_v, plane2_v, 0.11) == 0;

        if (flag)
            flag = cgal_intersection_util::Intersect(plane0, plane1, plane2, p0);

        return flag;
    }

    inline bool PolylinePlane(CGAL_Polyline &polyline, IK::Plane_3 &plane, IK::Segment_3 &alignmentSegment, IK::Segment_3 &result)
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Intersect polyline segments one by one, until two points are found, if any
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 pts[2];
        int count = 0;

        for (int i = 0; i < polyline.size() - 1; i++)
        {
            IK::Segment_3 segment(polyline[i], polyline[i + 1]);

            // IK::Point_3 p0;
            // bool flag = cgal_intersection_util::Intersect(segment, plane, p0);

            // if (flag) {
            //     pts[count] = p0;
            //     count++;
            //     if (count == 2)
            //         break;
            // }

            auto result = CGAL::intersection(segment, plane);

            if (result)
            {
                if (const IK::Point_3 *p = boost::get<IK::Point_3>(&*result))
                {
                    pts[count] = *p;
                    count++;
                    if (count == 2)
                        break;
                } // if point type
            }     // result exists
        }

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Align intersection points according to an average polygon edge direction
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (count == 2)
        {
            if (CGAL::squared_distance(alignmentSegment[0], pts[0]) < CGAL::squared_distance(alignmentSegment[0], pts[1]))
                result = IK::Segment_3(pts[0], pts[1]);
            else
                result = IK::Segment_3(pts[1], pts[0]);

            return true;
        }
        else
        {
            return false;
        }
    }
}
