#pragma once
#include "cgal.h"
#include "cgal_intersection_util.h"


namespace cgal_plane_util {

    inline IK::Plane_3 offset(IK::Plane_3& mainPlane, double d) {

        IK::Vector_3 normal = mainPlane.orthogonal_vector();
        cgal_vector_util::Unitize(normal);
        normal *= d;

        return IK::Plane_3(mainPlane.point() + normal, normal);
    }

    inline IK::Vector_3 PointAt2(IK::Vector_3(&input)[4], double s, double t) {

        return (input[0] + s * input[1] + t * input[2]);

    }

    inline IK::Vector_3 PointAt(IK::Vector_3(&input)[4], double s, double t, double c) {

        return (input[0] + s * input[1] + t * input[2] + c * input[3]);

    }

    inline CGAL_Polyline PlaneToLine(IK::Point_3& point, IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {

        CGAL_Polyline axes(2);
        IK::Point_3 center = point;
        IK::Vector_3 n = plane.orthogonal_vector();
        cgal_vector_util::Unitize(n);
        n *= x;
        IK::Point_3 normal = point + n;// +
        axes[0] = center;
        axes[1] = normal;

        return axes;

    }

    inline CGAL_Polyline PlaneToPolyline(IK::Plane_3& plane, double x = 1, double y = 1, double z = 1) {

        auto point = plane.point();
        auto X0 = plane.base1();
        auto Y0 = plane.base2();
        auto Z0 = plane.orthogonal_vector();
        cgal_vector_util::Unitize(X0);
        cgal_vector_util::Unitize(Y0);
        cgal_vector_util::Unitize(Z0);

        CGAL_Polyline axes(6);

        axes[0] = (point);
        axes[1] = (point + X0 * x * 1.25);
        axes[2] = (point);
        axes[3] = (point + Y0 * y);
        axes[4] = (point);
        axes[5] = (point + Z0 * z);

        return axes;

    }

    inline CGAL_Polyline PlaneToPolyline(IK::Vector_3(&input)[4], double x = 1, double y = 1, double z = 1) {

        CGAL_Polyline axes(6);

        axes[0] = (cgal_vector_util::ToPoint(input[0]));
        axes[1] = (cgal_vector_util::ToPoint(input[0]) + input[1] * x * 1.25);
        axes[2] = (cgal_vector_util::ToPoint(input[0]));
        axes[3] = (cgal_vector_util::ToPoint(input[0]) + input[2] * y);
        axes[4] = (cgal_vector_util::ToPoint(input[0]));
        axes[5] = (cgal_vector_util::ToPoint(input[0]) + input[3] * z);

        return axes;

    }

    inline void Assign(IK::Vector_3(&source)[4], IK::Vector_3(&target)[4], int n) {
        for (int i = 0; i < n; i++)
            target[i] = source[i];
    }

    inline void AveragePlaneOrigin(IK::Vector_3(&pl0)[4], IK::Vector_3(&pl1)[4], IK::Vector_3(&result)[4]) {

        IK::Vector_3 mid;
        cgal_vector_util::MidVector(pl0[0], pl0[1], mid);

        result[0] = mid;
        result[1] = pl0[1];
        result[2] = pl0[2];
        result[3] = pl0[3];

    }

    inline bool IsSameDirection(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.0001) {

        auto pl0_v = pl0.orthogonal_vector();
        auto pl1_v = pl1.orthogonal_vector();
        if (canBeFlipped) {
            return cgal_vector_util::IsParallelTo(pl0_v, pl1_v, tolerance) != 0;
        } else {
            return cgal_vector_util::IsParallelTo(pl0_v, pl1_v, tolerance) == -1;
        }

    }

    inline void ClosestPointTo(IK::Vector_3(&plane)[4], IK::Vector_3& p, IK::Vector_3& result) {

        const IK::Vector_3 v(p.x() - plane[0].x(), p.y() - plane[0].y(), p.z() - plane[0].z());
        double s = v * plane[1];
        double t = v * plane[2];
        result = PointAt2(plane, s, t);

    }

    inline bool  IsSamePosition(IK::Plane_3 pl0, IK::Plane_3 pl1, double tolerance = 0.0001) {

        IK::Point_3 p0 = pl0.point();
        IK::Point_3 p1 = pl1.point();

        IK::Point_3 cp0 = pl0.projection(p1);
        IK::Point_3 cp1 = pl1.projection(p0);

        return cgal_vector_util::DistanceSquare(cp0, p1) < tolerance && cgal_vector_util::DistanceSquare(cp1, p0) < tolerance;

    }

    inline bool IsCoplanar(IK::Plane_3 pl0, IK::Plane_3 pl1, bool canBeFlipped = true, double tolerance = 0.01) {

        return IsSameDirection(pl0, pl1, canBeFlipped, tolerance) && IsSamePosition(pl0, pl1, tolerance);

    }

}


