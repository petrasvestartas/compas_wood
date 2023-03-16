
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_plane_util.h"

namespace cgal_plane_util
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
    }

    /**
     * Move a plane by a distance along its normal
     *
     * @param [in] plane plane whose origin is move by the unitized normal
     * @param [in] distance distance to move the plane origin
     * @return moved plane
     */
    IK::Plane_3 translate_by_normal(const IK::Plane_3 &plane, const double &distance)
    {
        IK::Vector_3 normal = plane.orthogonal_vector();
        internal::unitize(normal);
        normal *= distance;

        return IK::Plane_3(plane.point() + normal, normal);
    }

    /**
     * Check if two planes have the same or flipped normal
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [in] can_be_flipped bool flag to indicate if the normal can be flipped
     * @return true if two planes ponting to the same or flipped normal
     */
    bool is_same_direction(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, bool can_be_flipped)
    {
        IK::Vector_3 plane0_v = plane0.orthogonal_vector();
        IK::Vector_3 plane1_v = plane1.orthogonal_vector();

        if (can_be_flipped)
            return internal::is_parallel_to(plane0_v, plane1_v) != 0;
        else
            return internal::is_parallel_to(plane0_v, plane1_v) == -1;
    }

    /**
     * Check if two planes are in the same position
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @return true origins are very close to each other
     */
    bool is_same_position(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1)
    {
        IK::Point_3 p0 = plane0.point();
        IK::Point_3 p1 = plane1.point();

        IK::Point_3 cp0 = plane0.projection(p1);
        IK::Point_3 cp1 = plane1.projection(p0);

        return CGAL::squared_distance(cp0, p1) < wood_globals::DISTANCE_SQUARED && CGAL::squared_distance(cp1, p0) < wood_globals::DISTANCE_SQUARED;
    }

    /**
     * Check if two planes have the same or flipped normal and share the same origin
     *
     * @param [in] plane0 first plane
     * @param [in] plane1 second plane
     * @param [in] can_be_flipped bool flag to indicate if the normal can be flipped
     * @return true if two planes are pointing to the same or flipped normal and share the same origin
     */
    bool is_coplanar(const IK::Plane_3 &plane0, const IK::Plane_3 &plane1, bool can_be_flipped)
    {
        return is_same_direction(plane0, plane1, can_be_flipped) && is_same_position(plane0, plane1);
    }

}
