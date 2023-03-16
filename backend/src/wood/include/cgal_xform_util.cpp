
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_xform_util.h"

namespace cgal_xform_util
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

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Change of basis transformation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CGAL::Aff_transformation_3<IK> change_basis(
        const IK::Vector_3 &origin_1, const IK::Vector_3 &x_axis_1, const IK::Vector_3 &y_axis_1, const IK::Vector_3 &z_axis_1,
        const IK::Vector_3 &origin_0, const IK::Vector_3 &x_axis_0, const IK::Vector_3 &y_axis_0, const IK::Vector_3 &z_axis_0)
    {
        // Q = a0*x_axis_0 + b0*y_axis_0 + c0*z_axis_0 = a1*x_axis_1 + b1*y_axis_1 + c1*z_axis_1
        // then this transform will map the point (a0,b0,c0) to (a1,b1,c1)

        //*this = ON_Xform::ZeroTransformation;

        double a, b, c, d;
        a = x_axis_1 * y_axis_1;
        b = x_axis_1 * z_axis_1;
        c = y_axis_1 * z_axis_1;
        double R[3][6] = {{x_axis_1 * x_axis_1, a, b, x_axis_1 * x_axis_0, x_axis_1 * y_axis_0, x_axis_1 * z_axis_0},
                          {a, y_axis_1 * y_axis_1, c, y_axis_1 * x_axis_0, y_axis_1 * y_axis_0, y_axis_1 * z_axis_0},
                          {b, c, z_axis_1 * z_axis_1, z_axis_1 * x_axis_0, z_axis_1 * y_axis_0, z_axis_1 * z_axis_0}};

        // row reduce R
        int i0 = (R[0][0] >= R[1][1]) ? 0 : 1;
        if (R[2][2] > R[i0][i0])
            i0 = 2;
        int i1 = (i0 + 1) % 3;
        int i2 = (i1 + 1) % 3;

        // or return false
        if (R[i0][i0] == 0.0)
            return CGAL::Aff_transformation_3<IK>(1, 0, 0, 0, 1, 0, 0, 0, 1);

        d = 1.0 / R[i0][i0];
        R[i0][0] *= d;
        R[i0][1] *= d;
        R[i0][2] *= d;
        R[i0][3] *= d;
        R[i0][4] *= d;
        R[i0][5] *= d;
        R[i0][i0] = 1.0;
        if (R[i1][i0] != 0.0)
        {
            d = -R[i1][i0];
            R[i1][0] += d * R[i0][0];
            R[i1][1] += d * R[i0][1];
            R[i1][2] += d * R[i0][2];
            R[i1][3] += d * R[i0][3];
            R[i1][4] += d * R[i0][4];
            R[i1][5] += d * R[i0][5];
            R[i1][i0] = 0.0;
        }
        if (R[i2][i0] != 0.0)
        {
            d = -R[i2][i0];
            R[i2][0] += d * R[i0][0];
            R[i2][1] += d * R[i0][1];
            R[i2][2] += d * R[i0][2];
            R[i2][3] += d * R[i0][3];
            R[i2][4] += d * R[i0][4];
            R[i2][5] += d * R[i0][5];
            R[i2][i0] = 0.0;
        }

        if (fabs(R[i1][i1]) < fabs(R[i2][i2]))
        {
            int i = i1;
            i1 = i2;
            i2 = i;
        }
        if (R[i1][i1] == 0.0)
            return CGAL::Aff_transformation_3<IK>(1, 0, 0, 0, 1, 0, 0, 0, 1);

        d = 1.0 / R[i1][i1];
        R[i1][0] *= d;
        R[i1][1] *= d;
        R[i1][2] *= d;
        R[i1][3] *= d;
        R[i1][4] *= d;
        R[i1][5] *= d;
        R[i1][i1] = 1.0;
        if (R[i0][i1] != 0.0)
        {
            d = -R[i0][i1];
            R[i0][0] += d * R[i1][0];
            R[i0][1] += d * R[i1][1];
            R[i0][2] += d * R[i1][2];
            R[i0][3] += d * R[i1][3];
            R[i0][4] += d * R[i1][4];
            R[i0][5] += d * R[i1][5];
            R[i0][i1] = 0.0;
        }
        if (R[i2][i1] != 0.0)
        {
            d = -R[i2][i1];
            R[i2][0] += d * R[i1][0];
            R[i2][1] += d * R[i1][1];
            R[i2][2] += d * R[i1][2];
            R[i2][3] += d * R[i1][3];
            R[i2][4] += d * R[i1][4];
            R[i2][5] += d * R[i1][5];
            R[i2][i1] = 0.0;
        }

        if (R[i2][i2] == 0.0)
            return CGAL::Aff_transformation_3<IK>(1, 0, 0, 0, 1, 0, 0, 0, 1);

        d = 1.0 / R[i2][i2];
        R[i2][0] *= d;
        R[i2][1] *= d;
        R[i2][2] *= d;
        R[i2][3] *= d;
        R[i2][4] *= d;
        R[i2][5] *= d;
        R[i2][i2] = 1.0;
        if (R[i0][i2] != 0.0)
        {
            d = -R[i0][i2];
            R[i0][0] += d * R[i2][0];
            R[i0][1] += d * R[i2][1];
            R[i0][2] += d * R[i2][2];
            R[i0][3] += d * R[i2][3];
            R[i0][4] += d * R[i2][4];
            R[i0][5] += d * R[i2][5];
            R[i0][i2] = 0.0;
        }
        if (R[i1][i2] != 0.0)
        {
            d = -R[i1][i2];
            R[i1][0] += d * R[i2][0];
            R[i1][1] += d * R[i2][1];
            R[i1][2] += d * R[i2][2];
            R[i1][3] += d * R[i2][3];
            R[i1][4] += d * R[i2][4];
            R[i1][5] += d * R[i2][5];
            R[i1][i2] = 0.0;
        }

        CGAL::Aff_transformation_3<IK> m_xform(
            R[0][3], R[0][4], R[0][5],
            R[1][3], R[1][4], R[1][5],
            R[2][3], R[2][4], R[2][5]);

        // m_xform[0][0] = R[0][3];
        // m_xform[0][1] = R[0][4];
        // m_xform[0][2] = R[0][5];

        // m_xform[1][0] = R[1][3];
        // m_xform[1][1] = R[1][4];
        // m_xform[1][2] = R[1][5];

        // m_xform[2][0] = R[2][3];
        // m_xform[2][1] = R[2][4];
        // m_xform[2][2] = R[2][5];

        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(-origin_1.x(), -origin_1.y(), -origin_1.z()));
        CGAL::Aff_transformation_3<IK> T2(CGAL::TRANSLATION, IK::Vector_3(origin_0.x(), origin_0.y(), origin_0.z()));
        return T2 * m_xform * T0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Plane transformation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CGAL::Aff_transformation_3<IK> plane_to_plane(
        const IK::Vector_3 &origin_0, const IK::Vector_3 &x_axis_0, const IK::Vector_3 &y_axis_0, const IK::Vector_3 &z_axis_0,
        const IK::Vector_3 &origin_1, const IK::Vector_3 &x_axis_1, const IK::Vector_3 &y_axis_1, const IK::Vector_3 &z_axis_1)
    {

        // Vectors must be unitized, else the result is wrong
        auto _x_axis_0 = x_axis_0;
        auto _y_axis_0 = y_axis_0;
        auto _z_axis_0 = z_axis_0;
        internal::unitize(_x_axis_0);
        internal::unitize(_y_axis_0);
        internal::unitize(_z_axis_0);

        auto _x_axis_1 = x_axis_1;
        auto _y_axis_1 = y_axis_1;
        auto _z_axis_1 = z_axis_1;
        internal::unitize(_x_axis_1);
        internal::unitize(_y_axis_1);
        internal::unitize(_z_axis_1);

        // transformation maps P0 to P1, P0+x_axis_0 to P1+x_axis_1, ...
        // Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin_0.x(), -origin_0.y(), -origin_0.z()));

        // Rotate ->
        CGAL::Aff_transformation_3<IK> f0(
            _x_axis_0.x(), _x_axis_0.y(), _x_axis_0.z(),
            _y_axis_0.x(), _y_axis_0.y(), _y_axis_0.z(),
            _z_axis_0.x(), _z_axis_0.y(), _z_axis_0.z());

        CGAL::Aff_transformation_3<IK> f1(
            _x_axis_1.x(), _y_axis_1.x(), _z_axis_1.x(),
            _x_axis_1.y(), _y_axis_1.y(), _z_axis_1.y(),
            _x_axis_1.z(), _y_axis_1.z(), _z_axis_1.z());
        CGAL::Aff_transformation_3<IK> r = f1 * f0;

        // Move to 3d -> T1 translates (0,0,0) to point P1
        CGAL::Aff_transformation_3<IK> t1(CGAL::TRANSLATION, IK::Vector_3(origin_1.x(), origin_1.y(), origin_1.z()));

        return t1 * r * t0;
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

    CGAL::Aff_transformation_3<IK> plane_to_xy(
        const IK::Vector_3 &origin, const IK::Vector_3 &x_axis, const IK::Vector_3 &y_axis, const IK::Vector_3 &z_axis)
    {

        // Vectors must be unitized, else the result is wrong
        IK::Vector_3 _x_axis = x_axis;
        IK::Vector_3 _y_axis = y_axis;
        IK::Vector_3 _z_axis = z_axis;

        internal::unitize(_x_axis);
        internal::unitize(_y_axis);
        internal::unitize(_z_axis);

        // Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> t(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

        // Rotate ->
        CGAL::Aff_transformation_3<IK> f(
            _x_axis.x(), _x_axis.y(), _x_axis.z(),
            _y_axis.x(), _y_axis.y(), _y_axis.z(),
            _z_axis.x(), _z_axis.y(), _z_axis.z());

        return f * t;
    }

    CGAL::Aff_transformation_3<IK> xy_to_plane(const IK::Vector_3 &origin, const IK::Plane_3 &plane)
    {
        // Vectors must be unitized, else the result is wrong
        auto x_axis = plane.base1();
        auto y_axis = plane.base2();
        auto z_axis = plane.orthogonal_vector();
        internal::unitize(x_axis);
        internal::unitize(y_axis);
        internal::unitize(z_axis);

        // transformation maps P0 to P1, P0+x_axis_0 to P1+x_axis_1, ...
        CGAL::Aff_transformation_3<IK> f(
            x_axis.x(), y_axis.x(), z_axis.x(),
            x_axis.y(), y_axis.y(), z_axis.y(),
            x_axis.z(), y_axis.z(), z_axis.z());

        // Move to 3d -> T1 translates (0,0,0) to point P1
        CGAL::Aff_transformation_3<IK> t(CGAL::TRANSLATION, IK::Vector_3(origin.x(), origin.y(), origin.z()));

        return t * f;
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

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Scale
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    CGAL::Aff_transformation_3<IK> scale(const double &scale_x, const double &scale_y, const double &scale_z)
    {
        CGAL::Aff_transformation_3<IK> t(
            scale_x, 0, 0,
            0, scale_y, 0,
            0, 0, scale_z);
        return t;
    }

    CGAL::Aff_transformation_3<IK> scale(const IK::Point_3 &origin, const double &scale_value)
    {
        // Move to origin -> T0 translates point P0 to (0,0,0), scale, translate back
        CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));
        CGAL::Aff_transformation_3<IK> t1(CGAL::SCALING, scale_value);
        CGAL::Aff_transformation_3<IK> t2(CGAL::TRANSLATION, IK::Vector_3(origin.x(), origin.y(), origin.z()));

        return t2 * t1 * t0;
    }

    CGAL::Aff_transformation_3<IK> scale(const IK::Point_3 &origin, const double &scale_x, const double &scale_y, const double &scale_z)
    {
        // Move to origin -> T0 translates point P0 to (0,0,0), scale, translate back
        CGAL::Aff_transformation_3<IK> t0(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));
        CGAL::Aff_transformation_3<IK> t1 = scale(scale_x, scale_y, scale_z);
        CGAL::Aff_transformation_3<IK> t2(CGAL::TRANSLATION, IK::Vector_3(origin.x(), origin.y(), origin.z()));

        return t2 * t1 * t0;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Axis rotation
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
