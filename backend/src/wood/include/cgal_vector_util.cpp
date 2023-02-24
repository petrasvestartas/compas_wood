#include "../../../stdafx.h"
#include "cgal_vector_util.h"

namespace cgal_vector_util
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

    void interpolate_points(const IK::Point_3 &from, const IK::Point_3 &to, const int &steps, std::vector<IK::Point_3> &points, int type_0_noends_1_end_2_oneend)
    {

        switch (type_0_noends_1_end_2_oneend)
        {
        case (0):
        {
            points.reserve(steps);

            for (auto i = 1; i < steps + 1; i++)
            {
                double t = i / (double)(1 + steps);

                points.emplace_back(
                    from.hx() + t * (to.hx() - from.hx()),
                    from.hy() + t * (to.hy() - from.hy()),
                    from.hz() + t * (to.hz() - from.hz()));
            }
        }
        break;
        case (1):
        {
            points.reserve(steps + 2);
            points.emplace_back(from);

            for (auto i = 1; i < steps + 1; i++)
            {
                double t = i / (double)(1 + steps);

                points.emplace_back(
                    from.hx() + t * (to.hx() - from.hx()),
                    from.hy() + t * (to.hy() - from.hy()),
                    from.hz() + t * (to.hz() - from.hz()));
            }
            points.emplace_back(to);
        }
        break;
        case (2):
        {
            points.reserve(steps + 1);
            points.emplace_back(from);

            for (auto i = 1; i < steps + 1; i++)
            {
                double t = i / (double)(1 + steps);

                points.emplace_back(
                    from.hx() + t * (to.hx() - from.hx()),
                    from.hy() + t * (to.hy() - from.hy()),
                    from.hz() + t * (to.hz() - from.hz()));
            }
        }
        break;
        }
    }
}
