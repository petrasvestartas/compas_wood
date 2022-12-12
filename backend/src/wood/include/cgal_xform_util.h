#pragma once
// #include "../../stdafx.h"
// #include "cgal_vector_util.h"

namespace cgal_xform_util
{
    // https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp //Line 2153
    inline bool ChangeBasis(

        IK::Point_3 &O1,
        IK::Vector_3 &X1, // final frame X (X,Y,Z = arbitrary basis)
        IK::Vector_3 &Y1, // final frame Y
        IK::Vector_3 &Z1, // final frame Z

        IK::Point_3 &O0,
        IK::Vector_3 &X0, // initial frame X (X,Y,Z = arbitrary basis)
        IK::Vector_3 &Y0, // initial frame Y
        IK::Vector_3 &Z0, // initial frame Z

        CGAL::Aff_transformation_3<IK> &output)
    {
        // Q = a0*X0 + b0*Y0 + c0*Z0 = a1*X1 + b1*Y1 + c1*Z1
        // then this transform will map the point (a0,b0,c0) to (a1,b1,c1)

        //*this = ON_Xform::ZeroTransformation;

        double a, b, c, d;
        a = X1 * Y1;
        b = X1 * Z1;
        c = Y1 * Z1;
        double R[3][6] = {{X1 * X1, a, b, X1 * X0, X1 * Y0, X1 * Z0},
                          {a, Y1 * Y1, c, Y1 * X0, Y1 * Y0, Y1 * Z0},
                          {b, c, Z1 * Z1, Z1 * X0, Z1 * Y0, Z1 * Z0}};

        // row reduce R
        int i0 = (R[0][0] >= R[1][1]) ? 0 : 1;
        if (R[2][2] > R[i0][i0])
            i0 = 2;
        int i1 = (i0 + 1) % 3;
        int i2 = (i1 + 1) % 3;
        if (R[i0][i0] == 0.0)
            return false;
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
            return false;
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
            return false;
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
        // m_xform.cartesian(0,0) = R[0][3];
        // m_xform[0][1] = R[0][4];
        // m_xform[0][2] = R[0][5];

        // m_xform[1][0] = R[1][3];
        // m_xform[1][1] = R[1][4];
        // m_xform[1][2] = R[1][5];

        // m_xform[2][0] = R[2][3];
        // m_xform[2][1] = R[2][4];
        // m_xform[2][2] = R[2][5];

        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O1.x(), 0 - O1.y(), 0 - O1.z()));
        CGAL::Aff_transformation_3<IK> T2(CGAL::TRANSLATION, IK::Vector_3(O0.x(), O0.y(), O0.z()));
        output = T2 * m_xform * T0;

        return true;
    }

    // https://stackoverflow.com/questions/23270292/a-point-cgal-transformation-between-two-coordinate-systems
    // https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp Line1960
    inline CGAL::Aff_transformation_3<IK> PlaneToPlane(
        const IK::Vector_3 &O0, const IK::Vector_3 &X0, const IK::Vector_3 &Y0, const IK::Vector_3 &Z0,
        const IK::Vector_3 &O1, const IK::Vector_3 &X1, const IK::Vector_3 &Y1, const IK::Vector_3 &Z1)
    {
        // transformation maps P0 to P1, P0+X0 to P1+X1, ...

        // Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

        // Rotate ->
        CGAL::Aff_transformation_3<IK> F0(
            X0.x(), X0.y(), X0.z(),
            Y0.x(), Y0.y(), Y0.z(),
            Z0.x(), Z0.y(), Z0.z());

        CGAL::Aff_transformation_3<IK> F1(
            X1.x(), Y1.x(), Z1.x(),
            X1.y(), Y1.y(), Z1.y(),
            X1.z(), Y1.z(), Z1.z());
        CGAL::Aff_transformation_3<IK> R = F1 * F0;

        // Move to 3d -> T1 translates (0,0,0) to point P1
        CGAL::Aff_transformation_3<IK> T1(CGAL::TRANSLATION, IK::Vector_3(O1.x() - 0, O1.y() - 0, O1.z() - 0));

        return T1 * R * T0;
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

    inline CGAL::Aff_transformation_3<IK> XYToPlane(const IK::Vector_3 &O1, const IK::Vector_3 &X1, const IK::Vector_3 &Y1, const IK::Vector_3 &Z1)
    {
        return PlaneToPlane(IK::Vector_3(0, 0, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 1, 0), IK::Vector_3(0, 0, 1), O1, X1, Y1, Z1);
    }

    inline CGAL::Aff_transformation_3<IK> Scale(double x, double y, double z)
    {
        CGAL::Aff_transformation_3<IK> scale_matrix(
            x, 0, 0,
            0, y, 0,
            0, 0, z);
        return scale_matrix;
    }

    inline CGAL::Aff_transformation_3<IK> VectorsToXY(
        IK::Vector_3 O0, IK::Vector_3 X0, IK::Vector_3 Y0, IK::Vector_3 Z0)
    {
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

    inline CGAL::Aff_transformation_3<IK> Scale(
        IK::Point_3 O0, double s)
    {
        // transformation maps P0 to P1, P0+X0 to P1+X1, ...

        // Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));
        CGAL::Aff_transformation_3<IK> T1(CGAL::SCALING, s);
        CGAL::Aff_transformation_3<IK> T2(CGAL::TRANSLATION, IK::Vector_3(O0.x(), O0.y(), O0.z()));

        return T2 * T1 * T0;
    }

    inline void RotationAroundAxis(
        double sin_angle,
        double cos_angle,
        IK::Vector_3 axis,
        IK::Point_3 center,
        CGAL::Aff_transformation_3<IK> &xform)
    {
        xform = CGAL::Aff_transformation_3<IK>(CGAL::Identity_transformation());

        // *this = ON_Xform::IdentityTransformation;

        for (;;)
        {
            // 29 June 2005 Dale Lear
            //     Kill noise in input
            if (fabs(sin_angle) >= 1.0 - ON_SQRT_EPSILON && fabs(cos_angle) <= ON_SQRT_EPSILON)
            {
                cos_angle = 0.0;
                sin_angle = (sin_angle < 0.0) ? -1.0 : 1.0;
                break;
            }

            if (fabs(cos_angle) >= 1.0 - ON_SQRT_EPSILON && fabs(sin_angle) <= ON_SQRT_EPSILON)
            {
                cos_angle = (cos_angle < 0.0) ? -1.0 : 1.0;
                sin_angle = 0.0;
                break;
            }

            if (fabs(cos_angle * cos_angle + sin_angle * sin_angle - 1.0) > ON_SQRT_EPSILON)
            {
                IK::Vector_2 cs(cos_angle, sin_angle);
                if (cgal_vector_util::Unitize2(cs))
                {
                    cos_angle = cs.x();
                    sin_angle = cs.y();
                    // no break here
                }
                else
                {
                    // ON_ERROR("sin_angle and cos_angle are both zero.");
                    cos_angle = 1.0;
                    sin_angle = 0.0;
                    break;
                }
            }

            if (fabs(cos_angle) > 1.0 - ON_EPSILON || fabs(sin_angle) < ON_EPSILON)
            {
                cos_angle = (cos_angle < 0.0) ? -1.0 : 1.0;
                sin_angle = 0.0;
                break;
            }

            if (fabs(sin_angle) > 1.0 - ON_EPSILON || fabs(cos_angle) < ON_EPSILON)
            {
                cos_angle = 0.0;
                sin_angle = (sin_angle < 0.0) ? -1.0 : 1.0;
                break;
            }

            break;
        }

        if (sin_angle != 0.0 || cos_angle != 1.0)
        {
            const double one_minus_cos_angle = 1.0 - cos_angle;
            IK::Vector_3 a = axis;
            if (fabs(cgal_vector_util::LengthSquared(a) - 1.0) > ON_EPSILON)
                cgal_vector_util::Unitize(a);

            auto x00 = a.x() * a.x() * one_minus_cos_angle + cos_angle;
            auto x01 = a.x() * a.y() * one_minus_cos_angle - a.z() * sin_angle;
            auto x02 = a.x() * a.z() * one_minus_cos_angle + a.y() * sin_angle;

            auto x10 = a.y() * a.x() * one_minus_cos_angle + a.z() * sin_angle;
            auto x11 = a.y() * a.y() * one_minus_cos_angle + cos_angle;
            auto x12 = a.y() * a.z() * one_minus_cos_angle - a.x() * sin_angle;

            auto x20 = a.z() * a.x() * one_minus_cos_angle - a.y() * sin_angle;
            auto x21 = a.z() * a.y() * one_minus_cos_angle + a.x() * sin_angle;
            auto x22 = a.z() * a.z() * one_minus_cos_angle + cos_angle;

            auto x30 = 0.0;
            auto x31 = 0.0;
            auto x32 = 0.0;
            auto x33 = 1.0;

            if (center.x() != 0.0 || center.y() != 0.0 || center.z() != 0.0)
            {
                auto x03 = -((x00 - 1.0) * center.x() + x01 * center.y() + x02 * center.z());
                auto x13 = -(x10 * center.x() + (x11 - 1.0) * center.y() + x12 * center.z());
                auto x23 = -(x20 * center.x() + x21 * center.y() + (x22 - 1.0) * center.z());

                xform = CGAL::Aff_transformation_3<IK>(
                    x00, x01, x02, x03,
                    x10, x11, x12, x13,
                    x20, x21, x22, x23
                    // x30, x31, x32, x33
                );
            }
            else
            {
                xform = CGAL::Aff_transformation_3<IK>(
                    x00, x01, x02,
                    x10, x11, x12,
                    x20, x21, x22);
            }
        }
    }

    inline void AxisRotation(double angle, IK::Vector_3 &axis, CGAL::Aff_transformation_3<IK> &rot)
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

        rot = CGAL::Aff_transformation_3<IK>(matrix[0], matrix[1], matrix[2],
                                             matrix[3], matrix[4], matrix[5],
                                             matrix[6], matrix[7], matrix[8],
                                             matrix[9], matrix[10], matrix[11]);
    }
}
