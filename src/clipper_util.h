#pragma once
#include "stdafx.h"
#include "clipper.h"
#include "cgal.h"
#include "cgal_polyline_util.h"
#include "cgal_xform_util.h"

namespace clipper_util {
    inline bool intersection_2D(
        CGAL_Polyline& p0,
        CGAL_Polyline& p1,
        IK::Plane_3& plane,
        CGAL_Polyline& c,
        double scale = 100000.0) {
        /////////////////////////////////////////////////////////////////////////////////////
        //Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline a;
        CGAL_Polyline b;
        cgal_polyline_util::Duplicate(p0, a);
        cgal_polyline_util::Duplicate(p1, b);

        /////////////////////////////////////////////////////////////////////////////////////
        //Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(p0[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::Transform(a, xform_toXY);
        cgal_polyline_util::Transform(b, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        //Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<ClipperLib::IntPoint> pathA(a.size() - 1);
        std::vector<ClipperLib::IntPoint> pathB(b.size() - 1);
        for (int i = 0; i < a.size() - 1; i++) {
            pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
            //printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
        }
        //printf("\n");
        for (int i = 0; i < b.size() - 1; i++) {
            pathB[i] = ClipperLib::IntPoint(b[i].x() * scale, b[i].y() * scale);
            //printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
        }

        //printf("\n");
        //printf("\n");

        ClipperLib::Clipper clipper;
        clipper.AddPath(pathA, ClipperLib::ptSubject, true);
        clipper.AddPath(pathB, ClipperLib::ptClip, true);
        ClipperLib::Paths C;
        clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

        if (C.size() > 0) {
            //CGAL_Debug(C[0].size());

            //CleanPolygon(C[0], GlobalTolerance * scale);//clean polygons

            //if (C[0].size() > 3 && Area(C[0]) > GlobalClipperAreaTolerance * scale * scale) {//skip triangles and very small polygons
            if (C[0].size() > 3 && std::abs(Area(C[0])) > std::abs(Area(pathA) * GlobalClipperAreaTolerance)) { //skip triangles and very small polygons
                /*	CGAL_Debug(Area(pathA));*/
                //CGAL_Debug(Area(C[0])/ (scale* scale));
                c.resize(C[0].size() + 1);

                for (int i = 0; i < C[0].size(); i++) {
                    IK::Point_3 p(C[0][i].X / scale, C[0][i].Y / scale, 0);
                    p = p.transform(xform_toXY_Inv); //Rotate back to 3D
                    c[i] = p;
                    //CGAL_Debug(c[i],true);
                }
                //CGAL_Debug(999999);
                c[C[0].size()] = c[0]; //Close
            } else {
                return false;
            }
        } else {
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////
        //Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }
}
