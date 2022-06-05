#pragma once
#include "stdafx.h"


namespace clipper_util {
    inline bool intersection_2D(
        CGAL_Polyline& p0,
        CGAL_Polyline& p1,
        IK::Plane_3& plane,
        CGAL_Polyline& c,
        double scale = 100000.0,
        int intersection_type = 0) {
        /////////////////////////////////////////////////////////////////////////////////////
        //Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (p0.size() > p0.max_size()) return false;
        if (p1.size() > p1.max_size()) return false;

        CGAL_Polyline a = p0;
        CGAL_Polyline b = p1;
        

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
            pathA[i] = ClipperLib::IntPoint((int)(a[i].x() * scale), (int)(a[i].y() * scale));
            //printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
        }
        //printf("\n");
        for (int i = 0; i < b.size() - 1; i++) {
            pathB[i] = ClipperLib::IntPoint((int)(b[i].x() * scale), (int)(b[i].y() * scale));
            //printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
        }

        //printf("\n");
        //printf("\n");

        ClipperLib::Clipper clipper;
        clipper.AddPath(pathA, ClipperLib::ptSubject, true);
        clipper.AddPath(pathB, ClipperLib::ptClip, true);
        ClipperLib::Paths C;
        
        ClipperLib::ClipType clip_type = static_cast<ClipperLib::ClipType>(intersection_type);
        clipper.Execute(clip_type, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);

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

    inline bool offset_2D(
        CGAL_Polyline& p0,
        //CGAL_Polyline& result,
        IK::Plane_3& plane,
        double offset,
        double scale = 100000.0
    ) {
        /////////////////////////////////////////////////////////////////////////////////////
        //Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (p0.size() > p0.max_size()) return false;

        CGAL_Polyline a = p0;

        /////////////////////////////////////////////////////////////////////////////////////
        //Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(p0[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::Transform(a, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        //Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<ClipperLib::IntPoint> pathA(a.size() - 1);

        for (int i = 0; i < a.size() - 1; i++) {
            pathA[i] = ClipperLib::IntPoint((int)(a[i].x() * scale), (int)(a[i].y() * scale));
        }


      
        ClipperLib::ClipperOffset clipper;
        clipper.AddPath(pathA, ClipperLib::JoinType::jtMiter, ClipperLib::EndType::etClosedPolygon);
        ClipperLib::Paths C;
        clipper.Execute(C, offset*scale);
        //printf("\n offset %f", offset);

        if (C.size() > 0) {

            if (C[0].size() > 3 && std::abs(Area(C[0])) > std::abs(Area(pathA) * GlobalClipperAreaTolerance)) { //skip triangles and very small polygons

             //Not Sure if rotation is correct, I doubt that cp_id+1 is needed, 
                int cp_id = 0;
                double cp_dist = std::pow(pathA[0].X - C[0][0].X, 2) + std::pow(pathA[0].Y - C[0][0].Y, 2);
                for (int i = 1; i < C[0].size(); i++) {
                    double dist = std::pow(pathA[0].X - C[0][i].X, 2) + std::pow(pathA[0].Y - C[0][i].Y, 2);
                    if (dist < cp_dist) {
                        cp_dist = dist;
                        cp_id = i;
                    }
                }
                std::rotate(C[0].begin(), C[0].begin() + cp_id, C[0].end());
                //std::rotate(C[0].begin(), C[0].begin()+ C[0].size()- cp_id, C[0].end());
                

                p0.clear();
                p0.resize(C[0].size() + 1);

                for (int i = 0; i < C[0].size(); i++) {
                    IK::Point_3 p(C[0][i].X / scale, C[0][i].Y / scale, 0);
                    p = p.transform(xform_toXY_Inv); //Rotate back to 3D
                    p0[i] = p;
                }

                p0[C[0].size()] = p0[0]; //Close
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }

        /////////////////////////////////////////////////////////////////////////////////////
        //Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }


}
