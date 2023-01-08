#include "../../../stdafx.h"
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

        const auto result = CGAL::intersection(l0, l1);

        if (result)
        {
            if (const IK::Point_2 *p = boost::get<IK::Point_2>(&*result))
            {
                output = IK::Point_3(p->hx(), p->hy(), 0);
                output = xform_Inv.transform(output);
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
    bool plane_plane(const IK::Plane_3 &plane_0, const IK::Plane_3 &plane_1, IK::Line_3 &output)
    {
        // The third plane is computed by the cross product of the first two
        IK::Vector_3 d = CGAL::cross_product(plane_1.orthogonal_vector(), plane_0.orthogonal_vector());

        // Since the line is infinite, the origin of the third plane is irrelevant
        IK::Point_3 p = CGAL::midpoint(plane_0.point(), plane_1.point());
        IK::Plane_3 plane_2(p, d);

        // 3 plane intersection
        IK::Point_3 output_p;
        bool rc = plane_plane_plane(plane_0, plane_1, plane_2, output_p);

        // infinite line by give the direction of the 3-plane intersection as the cross product of the first two planes
        output = IK::Line_3(output_p, output_p + d);

        return rc;
    }

    /**
     * Get the intersection of a line and a plane
     *
     * @param [in] line segment
     * @param [in] plane plane
     * @param [out] output the point output
     * @param [out] is_finite true if the line is finite, still the point will be outputed
     * @return true the intersection is sucsessful or point is outside the line, incase the finite search is used
     */
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

    /**
     * Get the intersection of a line and a plane
     *
     * @param [in] line infinite line
     * @param [in] plane plane
     * @param [out] output the point output
     * @param [out] is_finite true if the line is finite, still the point will be outputed
     * @return true the intersection is sucsessful or point is outside the line, incase the finite search is used
     */
    void line_plane(const IK::Line_3 &line, const IK::Plane_3 &plane, IK::Point_3 &output, bool is_finite)
    {
        auto segment = IK::Segment_3(line.point(), line.point() + line.to_vector());
        line_plane(segment, plane, output, is_finite);
    }

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
    bool get_quad_from_line_topbottomplanes(const IK::Plane_3 &collision_face, const IK::Segment_3 &line, const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, CGAL_Polyline &output)
    {
        IK::Vector_3 dir = line.to_vector();
        IK::Plane_3 line_pl0(line[0], dir);
        IK::Plane_3 line_pl1(line[1], dir);

        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane(line_pl0, plane0, collision_face, p0);
        bool result1 = plane_plane_plane(line_pl0, plane1, collision_face, p1);
        bool result2 = plane_plane_plane(line_pl1, plane1, collision_face, p2);
        bool result3 = plane_plane_plane(line_pl1, plane0, collision_face, p3);
        output = {p0, p1, p2, p3, p0};

        return result0 && result1 && result2 && result3;
    }

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
    bool get_orthogonal_vector_between_two_plane_pairs(const IK::Plane_3 &plane_pair0_0, const IK::Plane_3 &plane_pair1_0, const IK::Plane_3 &plane_pair1_1, IK::Vector_3 &output)
    {
        IK::Line_3 l0;
        IK::Line_3 l1;
        bool result0 = plane_plane(plane_pair0_0, plane_pair1_0, l0);
        bool result1 = plane_plane(plane_pair0_0, plane_pair1_1, l1);

        output = l1.point() - l0.projection(l1.point());
        return result0 && result1;
    }

    /**
     * Get a closed quad from four planes and the main_plane intersection
     *
     * @param [in] main_plane the plane that intersects the four planes
     * @param [in] sequence_of_planes C-type array of four planes
     * @param [out] output quad CGAL polyline
     * @return true, if the four plane intersections are successful
     */
    bool plane_4planes(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output)
    {
        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane(sequence_of_planes[0], sequence_of_planes[1], main_plane, p0);
        bool result1 = plane_plane_plane(sequence_of_planes[1], sequence_of_planes[2], main_plane, p1);
        bool result2 = plane_plane_plane(sequence_of_planes[2], sequence_of_planes[3], main_plane, p2);
        bool result3 = plane_plane_plane(sequence_of_planes[3], sequence_of_planes[0], main_plane, p3);
        output = {p0, p1, p2, p3, p0};
        return result0 && result1 && result2 && result3;
    }

    /**
     * Get a closed quad from four planes and the main_plane intersection, the outline is open, the last point is not the first point
     *
     * @param [in] main_plane
     * @param [in] sequence_of_planes C-type array of four planes
     * @param [out] output quad CGAL polyline
     * @return true, if the four plane intersections are successful
     */
    bool plane_4planes_open(const IK::Plane_3 &main_plane, const IK::Plane_3 (&sequence_of_planes)[4], CGAL_Polyline &output)
    {
        IK::Point_3 p0, p1, p2, p3;
        bool result0 = plane_plane_plane(sequence_of_planes[0], sequence_of_planes[1], main_plane, p0);
        bool result1 = plane_plane_plane(sequence_of_planes[1], sequence_of_planes[2], main_plane, p1);
        bool result2 = plane_plane_plane(sequence_of_planes[2], sequence_of_planes[3], main_plane, p2);
        bool result3 = plane_plane_plane(sequence_of_planes[3], sequence_of_planes[0], main_plane, p3);
        output = {p0, p1, p2, p3};
        return result0 && result1 && result2 && result3;
    }

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

    /**
     * Get the intersection of polyline and plane
     * WARNING: this method is used imprecise CGAL - boost method, it is not recommended to use it, it is probably causing non-planarity in the merge function
     *
     * @param [in] polyline CGAl_Polyline
     * @param [in] plane The plane that intersect the polylin
     * @param [in] alignment_segment Align intersection points according to an average polygon edge direction
     * @param [out] output the segment output
     * @return true if two points are returned from the intersection
     */
    bool polyline_plane(const CGAL_Polyline &polyline, const IK::Plane_3 &plane, IK::Segment_3 &alignment_segment, IK::Segment_3 &output)
    {
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Intersect polyline segments one by one, until two points are found, if any
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 pts[2];
        int count = 0;

        for (int i = 0; i < polyline.size() - 1; i++)
        {
            IK::Segment_3 segment(polyline[i], polyline[i + 1]);
            auto output = CGAL::intersection(segment, plane);

            if (output)
            {
                if (const IK::Point_3 *p = boost::get<IK::Point_3>(&*output))
                {
                    pts[count] = *p;
                    count++;
                    if (count == 2)
                        break;
                } // if point type
            }     // output exists
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
}
