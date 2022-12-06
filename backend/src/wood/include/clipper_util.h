#pragma once
//#include "../../stdafx.h"

// https://github.com/AngusJohnson/Clipper2
namespace clipper_util
{

    inline bool point_inclusion(
        CGAL_Polyline &p0,
        IK::Plane_3 &plane,
        IK::Point_3 &point,
        double scale = 100000.0)
    {

        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (p0.size() > p0.max_size())
            return false;

        CGAL_Polyline a = p0;
        IK::Point_3 b = point;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(p0[0], plane);
        cgal_polyline_util::Transform(a, xform_toXY);
        b = b.transform(xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::Point64> pathA(a.size() - 1);
        for (int i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::Point64((int)(a[i].x() * scale), (int)(a[i].y() * scale));
            // printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
        }

        Clipper2Lib::Point64 point_clipper((int)(b.x() * scale), (int)(b.y() * scale));

        /////////////////////////////////////////////////////////////////////////////////////
        // Check if point is inside
        // returns 0 if false, +1 if true, -1 if pt ON polygon boundary
        /////////////////////////////////////////////////////////////////////////////////////
        return Clipper2Lib::PointInPolygon(point_clipper, pathA) != Clipper2Lib::PointInPolygonResult::IsOutside;
    }

    inline bool intersection_2D(
        CGAL_Polyline &p0,
        CGAL_Polyline &p1,
        IK::Plane_3 &plane,
        CGAL_Polyline &c,
        double scale = 100000.0,
        int intersection_type = 0,
        bool include_triangles = false)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (p0.size() > p0.max_size())
            return false;
        if (p1.size() > p1.max_size())
            return false;

        CGAL_Polyline a = p0;
        CGAL_Polyline b = p1;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(p0[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::Transform(a, xform_toXY);
        cgal_polyline_util::Transform(b, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::Point64> pathA(a.size() - 1);
        std::vector<Clipper2Lib::Point64> pathB(b.size() - 1);
        for (int i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::Point64((int)(a[i].x() * scale), (int)(a[i].y() * scale));
            // printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
        }
        // printf("\n");
        for (int i = 0; i < b.size() - 1; i++)
        {
            pathB[i] = Clipper2Lib::Point64((int)(b[i].x() * scale), (int)(b[i].y() * scale));
            // printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
        }

        // printf("\n");
        // printf("\n");

        // Clipper2Lib::Clipper clipper;
        // clipper.AddPath(pathA, ClipperLib::ptSubject, true);
        // clipper.AddPath(pathB, ClipperLib::ptClip, true);
        // Clipper2Lib::Paths C;

        // Clipper2Lib::ClipType clip_type = static_cast<Clipper2Lib::ClipType>(intersection_type);
        // clipper.Execute(clip_type, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);
        Clipper2Lib::Paths64 pathA_;
        Clipper2Lib::Paths64 pathB_;
        pathA_.emplace_back(pathA);
        pathB_.emplace_back(pathB);
        Clipper2Lib::Paths64 C = Clipper2Lib::Intersect(pathA_, pathB_, Clipper2Lib::FillRule::NonZero);

        if (C.size() > 0)
        {
            // CGAL_Debug(C[0].size());

            // CleanPolygon(C[0], GlobalTolerance * scale);//clean polygons

            // if (C[0].size() > 3 && Area(C[0]) > GlobalClipperAreaTolerance * scale * scale) {//skip triangles and very small polygons
            // include triangles based on user input
            bool is_not_triangle = C[0].size() != 3;
            if (!is_not_triangle)
                is_not_triangle = include_triangles;

            if (C[0].size() > 2 && is_not_triangle && std::abs(Area(C[0])) > std::abs(Area(pathA) * GlobalClipperAreaTolerance))
            { // skip triangles and very small polygons
                /*	CGAL_Debug(Area(pathA));*/
                // CGAL_Debug(Area(C[0])/ (scale* scale));
                c.resize(C[0].size() + 1);

                for (int i = 0; i < C[0].size(); i++)
                {
                    IK::Point_3 p(C[0][i].x / scale, C[0][i].y / scale, 0);
                    p = p.transform(xform_toXY_Inv); // Rotate back to 3D
                    c[i] = p;
                    // CGAL_Debug(c[i],true);
                }
                // CGAL_Debug(999999);
                c[C[0].size()] = c[0]; // Close
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

        /////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }

    inline bool offset_2D(
        CGAL_Polyline &p0,
        IK::Plane_3 &plane,
        double offset,
        double scale = 100000.0)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (p0.size() > p0.max_size())
            return false;

        CGAL_Polyline a = p0;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(p0[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::Transform(a, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::Path64 pathA(a.size() - 1);

        for (int i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::Point64((int)(a[i].x() * scale), (int)(a[i].y() * scale));
        }

        // Clipper2Lib::ClipperOffset clipper;
        // clipper.AddPath(pathA, ClipperLib::JoinType::jtMiter, ClipperLib::EndType::etClosedPolygon);
        // Clipper2Lib::Paths64 C;
        // clipper.Execute(C, offset * scale);

        double offset_ = offset * scale;
        Clipper2Lib::Paths64 C;
        C.emplace_back(pathA);
        C = Clipper2Lib::InflatePaths(C, offset_, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);

        // printf("\n offset %f", offset);

        if (C.size() > 0)
        {

            if (C[0].size() > 3 && std::abs(Area(C[0])) > std::abs(Area(pathA) * GlobalClipperAreaTolerance))
            { // skip triangles and very small polygons

                // Not Sure if rotation is correct, I doubt that cp_id+1 is needed,
                int cp_id = 0;
                double cp_dist = std::pow(pathA[0].x - C[0][0].x, 2) + std::pow(pathA[0].y - C[0][0].y, 2);
                for (int i = 1; i < C[0].size(); i++)
                {
                    double dist = std::pow(pathA[0].x - C[0][i].x, 2) + std::pow(pathA[0].y - C[0][i].y, 2);
                    if (dist < cp_dist)
                    {
                        cp_dist = dist;
                        cp_id = i;
                    }
                }
                std::rotate(C[0].begin(), C[0].begin() + cp_id, C[0].end());
                // std::rotate(C[0].begin(), C[0].begin()+ C[0].size()- cp_id, C[0].end());

                p0.clear();
                p0.resize(C[0].size() + 1);

                for (int i = 0; i < C[0].size(); i++)
                {
                    IK::Point_3 p(C[0][i].x / scale, C[0][i].y / scale, 0);
                    p = p.transform(xform_toXY_Inv); // Rotate back to 3D
                    p0[i] = p;
                }

                p0[C[0].size()] = p0[0]; // Close
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

        /////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }

    /**
     * bounding rectangle of a planar polyline
     * the polyline is oriented to XY plane using the input plane
     * then oriented back to 3D
     * a) only the height of the connection volume is taken, not the rectangle
     * b) joints have to be recomputed each time
     * c) intersection between line and elements have to be computed to get a correct line length
     *
     * @param polyline planar polyline rotated in space
     * @param plane polyline's plane
     * @param result bounding rectangle as CGAl polyline
     * @return bool if the result is successful, e.g. box is valid or the input polyline has less than 3 points
     */
    inline bool bounding_rectangle(CGAL_Polyline &polyline, IK::Plane_3 &plane, CGAL_Polyline &result, double scale = 100000.0)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline.size() < 3)
            return false;

        CGAL_Polyline polyline_copy = polyline;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(polyline_copy[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::Transform(polyline_copy, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::Path64 polyline_clipper(polyline_copy.size() - 1);

        for (int i = 0; i < polyline_copy.size() - 1; i++)
            polyline_clipper[i] = Clipper2Lib::Point64((int)(polyline_copy[i].x() * scale), (int)(polyline_copy[i].y() * scale));

        /////////////////////////////////////////////////////////////////////////////////////
        // Get Bounding Box
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::Rect64 br = Clipper2Lib::Bounds(polyline_clipper);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to CGAL and orient back to 3D
        /////////////////////////////////////////////////////////////////////////////////////
        result.reserve(5);
        result = CGAL_Polyline{
            IK::Point_3(br.left / scale, br.bottom / scale, 0),
            IK::Point_3(br.right / scale, br.bottom / scale, 0),
            IK::Point_3(br.right / scale, br.top / scale, 0),
            IK::Point_3(br.left / scale, br.top / scale, 0)};
        for (int i = 0; i < 4; i++)
            result[i] = result[i].transform(xform_toXY_Inv);
        result.emplace_back(result[0]);

        return true;
    }

    // /**
    //  * center of a planar polyline
    //  * the polyline is oriented to XY plane using the input plane
    //  * then oriented back to 3D
    //  * the plane is computed from the polyline
    //  *
    //  * @param polyline planar polyline rotated in space
    //  * @param result point as IK::Point_3
    //  * @return bool if the result is successful, e.g. box is valid or the input polyline has less than 3 points
    //  */
    // inline bool polyline_center(CGAL_Polyline &polyline, IK::Point_3 &result, double scale = 100000.0)
    // {
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Orient from 3D to 2D
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     if (polyline.size() < 3)
    //         return false;

    //     CGAL_Polyline polyline_copy = polyline;

    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Get center polyline plane
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     cgal_plane_util::average_plane(polyline_copy, result);

    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Create Transformation
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(polyline_copy[0], plane);
    //     CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
    //     cgal_polyline_util::Transform(polyline_copy, xform_toXY);

    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Convert to Clipper
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     Clipper2Lib::Path64 polyline_clipper(polyline_copy.size() - 1);

    //     for (int i = 0; i < polyline_copy.size() - 1; i++)
    //         polyline_clipper[i] = Clipper2Lib::Point64((int)(polyline_copy[i].x() * scale), (int)(polyline_copy[i].y() * scale));

    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Get Bounding Box
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     Clipper2Lib::Rect64 br = Clipper2Lib::Bounds(polyline_clipper);

    //     /////////////////////////////////////////////////////////////////////////////////////
    //     // Convert to CGAL and orient back to 3D
    //     /////////////////////////////////////////////////////////////////////////////////////
    //     result.reserve(5);
    //     result = CGAL_Polyline{
    //         IK::Point_3(br.left / scale, br.bottom / scale, 0),
    //         IK::Point_3(br.right / scale, br.bottom / scale, 0),
    //         IK::Point_3(br.right / scale, br.top / scale, 0),
    //         IK::Point_3(br.left / scale, br.top / scale, 0)};
    //     for (int i = 0; i < 4; i++)
    //         result[i] = result[i].transform(xform_toXY_Inv);
    //     result.emplace_back(result[0]);

    //     return true;
    // }
}
