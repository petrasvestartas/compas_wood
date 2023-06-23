
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

 
#include "clipper_util.h"

namespace clipper_util
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
         * The average normal computed my the sum of the concentric cross-products
         *
         * @param [in] polyline Open or closed polygon
         * @param [out] average_normal The sum of cross-products, computed from the current and neighboring points, considering both open and closed polylines
         */
        void get_average_normal(const CGAL_Polyline &polyline, IK::Vector_3 &average_normal)
        {
            size_t len = CGAL::squared_distance(polyline.front(), polyline.back()) < wood_globals::DISTANCE_SQUARED ? polyline.size() - 1 : polyline.size();
            average_normal = IK::Vector_3(0, 0, 0);

            for (int i = 0; i < len; i++)
            {
                auto prev = ((i - 1) + len) % len;
                auto next = ((i + 1) + len) % len;
                average_normal = average_normal + CGAL::cross_product(polyline[i] - polyline[prev], polyline[next] - polyline[i]);
            }
        }

        void get_fast_plane(const CGAL_Polyline &polyline, IK::Point_3 &center, IK::Plane_3 &plane)
        {
            // origin
            center = polyline[0];

            // plane
            IK::Vector_3 average_normal;
            get_average_normal(polyline, average_normal);
            plane = IK::Plane_3(center, average_normal);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Orient polylines from 3D to 2D, overloads includes no copy and copy
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void orient_3d_to_2d(const std::vector<CGAL_Polyline> &input, std::vector<CGAL_Polyline> &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
        {
            // copy
            output = input;

            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input[0], origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // transform the points of the input polyline, be aware that the initial points are modified
            for (auto &polyline : output)
                for (auto it = polyline.begin(); it != polyline.end(); ++it)
                    *it = it->transform(xform_to_xy);
        }

        void orient_3d_to_2d(const CGAL_Polyline &input, CGAL_Polyline &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
        {
            // copy
            output = input;

            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input, origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // transform the points of the input polyline, be aware that the initial points are modified
            for (auto it = output.begin(); it != output.end(); ++it)
                *it = it->transform(xform_to_xy);
        }

        void orient_3d_to_2d(std::vector<CGAL_Polyline> &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
        {

            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input[0], origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // transform the points of the input polyline, be aware that the initial points are modified
            for (auto &polyline : input)
                for (auto it = polyline.begin(); it != polyline.end(); ++it)
                    *it = it->transform(xform_to_xy);
        }

        void orient_3d_to_2d(CGAL_Polyline &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv)
        {

            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input, origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // transform the points of the input polyline, be aware that the initial points are modified
            for (auto it = input.begin(); it != input.end(); ++it)
                *it = it->transform(xform_to_xy);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Conversions of CGAL_Polyline and Clipper in 2D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        void cgalpolylines_to_clipper_2d(const std::vector<CGAL_Polyline> &input, Clipper2Lib::PathsD &output, bool closed_polyline)
        {

            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the cgal polylines
            for (auto &polyline : input)
            {
                // add clipper polyline and reserve its memory
                output.emplace_back(Clipper2Lib::PathD());
                size_t size = closed_polyline ? polyline.size() : polyline.size() - 1;
                output.back().reserve(size);

                // add points to clipper outline
                if (closed_polyline)
                    for (auto iter = polyline.begin(); iter != std::prev(polyline.end()); ++iter)
                        output.back().emplace_back(
                            (int64_t)((*iter).hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)((*iter).hy() * wood_globals::CLIPPER_SCALE));

                else
                    for (auto &point : polyline)
                        output.back().emplace_back(
                            (int64_t)(point.hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)(point.hy() * wood_globals::CLIPPER_SCALE));
            }
        }

        void clipper_to_cgalpolylines_2d(const Clipper2Lib::PathsD &input, std::vector<CGAL_Polyline> &output, bool closed_polyline)
        {
            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the clipper polylines
            for (auto &polyline : input)
            {
                // add cgal polyline and reserve its memory
                output.emplace_back(CGAL_Polyline());
                size_t size = closed_polyline ? polyline.size() : polyline.size() + 1;
                output.back().reserve(polyline.size());

                // add points to cgal outline
                for (auto &point : polyline)
                    output.back().emplace_back(
                        point.x / (double)wood_globals::CLIPPER_SCALE,
                        point.y / (double)wood_globals::CLIPPER_SCALE,
                        0);

                if (closed_polyline)
                    output.back().emplace_back(output.back()[0]);
            }
        }

        void cgalpolyline_to_clipper_2d(const CGAL_Polyline &input, Clipper2Lib::PathD &output, bool closed_polyline)
        {
            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // add points to clipper outline
            if (closed_polyline)
                for (auto iter = input.begin(); iter != std::prev(input.end()); ++iter)
                    output.emplace_back(
                        (int64_t)((*iter).hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)((*iter).hy() * wood_globals::CLIPPER_SCALE));

            else
                for (auto &point : input)
                    output.emplace_back(
                        (int64_t)(point.hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)(point.hy() * wood_globals::CLIPPER_SCALE));
        }

        void clipper_to_cgalpolyline_2d(const Clipper2Lib::PathD &input, CGAL_Polyline &output, bool closed_polyline)
        {
            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the clipper points
            for (auto &point : input)
                output.emplace_back(point.x / (double)wood_globals::CLIPPER_SCALE, point.y / (double)wood_globals::CLIPPER_SCALE, 0);
            if (closed_polyline)
                output.emplace_back(output[0]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Conversions of CGAL_Polyline and Clipper in 3D
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        void cgalpolylines_to_clipper_3d(const std::vector<CGAL_Polyline> &input, Clipper2Lib::PathsD &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline)
        {
            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input[0], origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the cgal polylines
            for (auto &polyline : input)
            {
                // add clipper polyline and reserve its memory
                output.emplace_back(Clipper2Lib::PathD());
                size_t size = closed_polyline ? polyline.size() : polyline.size() - 1;
                output.back().reserve(size);

                // add points to clipper outline while performing transformation to 2d
                if (closed_polyline)
                    for (auto iter = polyline.begin(); iter != std::prev(polyline.end()); ++iter)
                    {
                        auto point_oriented = iter->transform(xform_to_xy);
                        output.back().emplace_back(
                            (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                    }

                else
                    for (auto &point : polyline)
                    {
                        auto point_oriented = point.transform(xform_to_xy);
                        output.back().emplace_back(
                            (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                    }
            }
        }

        void clipper_to_cgalpolylines_3d(const Clipper2Lib::PathsD &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, std::vector<CGAL_Polyline> &output, bool closed_polyline)
        {
            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the clipper polylines
            for (auto &polyline : input)
            {
                // add cgal polyline and reserve its memory
                output.emplace_back(CGAL_Polyline());
                size_t size = closed_polyline ? polyline.size() : polyline.size() + 1;
                output.back().reserve(polyline.size());

                // add points to cgal outline while performing transformation to 3d
                for (auto &point : polyline)
                {
                    auto point_oriented = IK::Point_3(point.x / (double)wood_globals::CLIPPER_SCALE, point.y / (double)wood_globals::CLIPPER_SCALE, 0);
                    point_oriented = point_oriented.transform(xform_to_xy_inv);
                    output.back().emplace_back(point_oriented);
                }

                if (closed_polyline)
                    output.back().emplace_back(output.back()[0]);
            }
        }

        void cgalpolyline_to_clipper_3d(const CGAL_Polyline &input, Clipper2Lib::PathD &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline)
        {
            // get plane from the first polyline
            IK::Point_3 origin;
            IK::Plane_3 plane;
            get_fast_plane(input, origin, plane);

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(origin, plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // add points to clipper outline
            if (closed_polyline)
                for (auto iter = input.begin(); iter != std::prev(input.end()); ++iter)
                {
                    auto point_oriented = iter->transform(xform_to_xy);
                    output.emplace_back(
                        (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                }

            else
                for (auto &point : input)
                {
                    auto point_oriented = point.transform(xform_to_xy);
                    output.emplace_back(
                        (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                }
        }

        void clipper_to_cgalpolyline_3d(const Clipper2Lib::PathD &input, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, CGAL_Polyline &output, bool closed_polyline)
        {
            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the clipper points
            for (auto &point : input)
            {
                auto point_oriented = IK::Point_3(point.x / (double)wood_globals::CLIPPER_SCALE, point.y / (double)wood_globals::CLIPPER_SCALE, 0);
                point_oriented = point_oriented.transform(xform_to_xy_inv);
                output.emplace_back(point_oriented);
            }
            if (closed_polyline)
                output.emplace_back(output[0]);
        }

        void cgalpolylines_to_clipper_3d(const std::vector<CGAL_Polyline> &input, const IK::Plane_3 &plane, Clipper2Lib::PathsD &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline)
        {

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(input[0][0], plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // interate through the cgal polylines
            for (auto &polyline : input)
            {
                // add clipper polyline and reserve its memory
                output.emplace_back(Clipper2Lib::PathD());
                size_t size = closed_polyline ? polyline.size() : polyline.size() - 1;
                output.back().reserve(size);

                // add points to clipper outline while performing transformation to 2d
                if (closed_polyline)
                    for (auto iter = polyline.begin(); iter != std::prev(polyline.end()); ++iter)
                    {
                        auto point_oriented = iter->transform(xform_to_xy);
                        output.back().emplace_back(
                            (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                    }

                else
                    for (auto &point : polyline)
                    {
                        auto point_oriented = point.transform(xform_to_xy);
                        output.back().emplace_back(
                            (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                            (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                    }
            }
        }

        void cgalpolyline_to_clipper_3d(const CGAL_Polyline &input, const IK::Plane_3 &plane, Clipper2Lib::PathD &output, CGAL::Aff_transformation_3<IK> &xform_to_xy_inv, bool closed_polyline)
        {

            // get transformation from the 3D to xy plane and output its inverse
            CGAL::Aff_transformation_3<IK> xform_to_xy = plane_to_xy(input[0], plane);
            xform_to_xy_inv = xform_to_xy.inverse();

            // clean the containter and reserve memory
            output.clear();
            output.reserve(input.size());

            // add points to clipper outline
            if (closed_polyline)
                for (auto iter = input.begin(); iter != std::prev(input.end()); ++iter)
                {
                    auto point_oriented = iter->transform(xform_to_xy);
                    output.emplace_back(
                        (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                }

            else
                for (auto &point : input)
                {
                    auto point_oriented = point.transform(xform_to_xy);
                    output.emplace_back(
                        (int64_t)(point_oriented.hx() * wood_globals::CLIPPER_SCALE),
                        (int64_t)(point_oriented.hy() * wood_globals::CLIPPER_SCALE));
                }
        }
    }

    bool is_point_inside(CGAL_Polyline &polyline, IK::Plane_3 &plane, IK::Point_3 &point)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline.size() > polyline.max_size())
            return false;

        CGAL_Polyline a = polyline;
        IK::Point_3 b = point;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy = internal::plane_to_xy(polyline[0], plane);

        for (auto it = a.begin(); it != a.end(); ++it)
            *it = it->transform(xform_to_xy);

        b = b.transform(xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::PointD> pathA(a.size() - 1);
        for (int i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::PointD((int)(a[i].x() * wood_globals::CLIPPER_SCALE), (int)(a[i].y() * wood_globals::CLIPPER_SCALE));
        }

        Clipper2Lib::PointD point_clipper((int)(b.x() * wood_globals::CLIPPER_SCALE), (int)(b.y() * wood_globals::CLIPPER_SCALE));

        /////////////////////////////////////////////////////////////////////////////////////
        // Check if point is inside
        // returns 0 if false, +1 if true, -1 if pt ON polygon boundary
        /////////////////////////////////////////////////////////////////////////////////////
        return Clipper2Lib::PointInPolygon(point_clipper, pathA) != Clipper2Lib::PointInPolygonResult::IsOutside;
    }

    bool get_intersection_between_two_polylines(CGAL_Polyline &polyline0, CGAL_Polyline &polyline1, IK::Plane_3 &plane, CGAL_Polyline &intersection_result, int intersection_type, bool include_triangles)
    {

        //printf("get_intersection_between_two_polylines\n");

        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline0.size() > polyline0.max_size())
            return false;
        if (polyline1.size() > polyline1.max_size())
            return false;

        CGAL_Polyline a = polyline0;
        CGAL_Polyline b = polyline1;


        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy = internal::plane_to_xy(polyline0[0], plane);
        CGAL::Aff_transformation_3<IK> xform_to_xy_Inv = xform_to_xy.inverse();

        for (auto it = a.begin(); it != a.end(); ++it)
            *it = it->transform(xform_to_xy);

        for (auto it = b.begin(); it != b.end(); ++it)
            *it = it->transform(xform_to_xy);
        //  Transform(a, xform_to_xy);
        //  Transform(b, xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::PointD> pathA(a.size() - 1);
        std::vector<Clipper2Lib::PointD> pathB(b.size() - 1);
        for (size_t i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::PointD((a[i].x() ), (a[i].y() ));
        }

        for (size_t i = 0; i < b.size() - 1; i++)
        {
            pathB[i] = Clipper2Lib::PointD((b[i].x()), (b[i].y()));
        }

        Clipper2Lib::PathsD pathA_;
        Clipper2Lib::PathsD pathB_;
        pathA_.emplace_back(pathA);
        pathB_.emplace_back(pathB);

        Clipper2Lib::PathsD C;


        int precision = 2;
        if (intersection_type == 0)
            C = Clipper2Lib::Intersect(pathA_, pathB_, Clipper2Lib::FillRule::NonZero,precision);
        else if (intersection_type == 1)
            C = Clipper2Lib::Union(pathA_, pathB_, Clipper2Lib::FillRule::NonZero,precision);
        else if (intersection_type == 2)
            C = Clipper2Lib::Difference(pathA_, pathB_, Clipper2Lib::FillRule::NonZero,precision);
        else if (intersection_type == 3)
            C = Clipper2Lib::Xor(pathA_, pathB_, Clipper2Lib::FillRule::NonZero,precision);


        if (C.size() > 0)
        {
         
           // printf("Area_0 %f Area_0 %f\n", std::abs(Area(C[0])),Area(pathA) );
            // include triangles based on user input
            bool is_not_triangle = C[0].size() != 3;
            if (!is_not_triangle)
                is_not_triangle = include_triangles;

            //&& std::abs(Area(C[0])) > std::abs(Area(pathA)*0.05
            if (C[0].size() > 2 && is_not_triangle && std::abs(Area(C[0])) > 0.1  )//* the result is not a line, not a triangle, and not a very small polygon
            { 
                
                intersection_result.resize(C[0].size() + 1);

                for (size_t i = 0; i < C[0].size(); i++)
                {
                    IK::Point_3 p(C[0][i].x  , C[0][i].y , 0);
                    p = p.transform(xform_to_xy_Inv); // Rotate back to 3D

                    intersection_result[i] = p;
                }

                intersection_result[C[0].size()] = intersection_result[0]; // Close
               
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
        //intersection_result = polyline0;
        //printf("intersection_result.size() %d\n", intersection_result.size());
        return true;
    }

    bool offset_in_2d(CGAL_Polyline &polyline, const double &offset)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline.size() > polyline.max_size())
            return false;

        CGAL_Polyline a = polyline;

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::PathD pathA(a.size() - 1);

        for (size_t i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::PointD((a[i].x()  ), (a[i].y()  ));
        }

        double offset_ = offset;
        Clipper2Lib::PathsD C;
        C.emplace_back(pathA);
        C = Clipper2Lib::InflatePaths(C, offset_, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon, 2);

        if (C.size() > 0)
        {

             if (std::abs(Area(C[0])) > 0.0001) // C[0].size() > 3 && std::abs(Area(pathA) * 0.9)
            {                                                                              // skip triangles and very small polygons

                // Not Sure if rotation is correct, I doubt that cp_id+1 is needed,
                size_t cp_id = 0;
                double cp_dist = std::pow(pathA[0].x - C[0][0].x, 2) + std::pow(pathA[0].y - C[0][0].y, 2);
                for (size_t i = 1; i < C[0].size(); i++)
                {
                    double dist = std::pow(pathA[0].x - C[0][i].x, 2) + std::pow(pathA[0].y - C[0][i].y, 2);
                    if (dist < cp_dist)
                    {
                        cp_dist = dist;
                        cp_id = i;
                    }
                }
                std::rotate(C[0].begin(), C[0].begin() + cp_id, C[0].end());

                polyline.clear();
                polyline.resize(C[0].size() + 1);

                for (size_t i = 0; i < C[0].size(); i++)
                {
                    IK::Point_3 p(C[0][i].x  , C[0][i].y , 0);
                    polyline[i] = p;
                }

                polyline[C[0].size()] = polyline[0]; // Close
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

    bool offset_in_3d(CGAL_Polyline &polyline, IK::Plane_3 &plane, const double &offset)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        if (polyline.size() > polyline.max_size())
            return false;

        CGAL_Polyline a = polyline;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy = internal::plane_to_xy(polyline[0], plane);
        CGAL::Aff_transformation_3<IK> xform_to_xy_Inv = xform_to_xy.inverse();
        for (auto it = a.begin(); it != a.end(); ++it)
            *it = it->transform(xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::PathD pathA(a.size() - 1);

        for (size_t i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::PointD(a[i].x() , a[i].y() );
        }

        double offset_ = offset ;
        Clipper2Lib::PathsD C;
        C.emplace_back(pathA);
        C = Clipper2Lib::InflatePaths(C, offset_, Clipper2Lib::JoinType::Miter, Clipper2Lib::EndType::Polygon);

        if (C.size() > 0)
        {

             if (std::abs(Area(C[0])) > 0.0001) // C[0].size() > 3 && std::abs(Area(pathA) * 0.9)
            {                                                                              // skip triangles and very small polygons

                // Not Sure if rotation is correct, I doubt that cp_id+1 is needed,
                size_t cp_id = 0;
                double cp_dist = std::pow(pathA[0].x - C[0][0].x, 2) + std::pow(pathA[0].y - C[0][0].y, 2);
                for (size_t i = 1; i < C[0].size(); i++)
                {
                    double dist = std::pow(pathA[0].x - C[0][i].x, 2) + std::pow(pathA[0].y - C[0][i].y, 2);
                    if (dist < cp_dist)
                    {
                        cp_dist = dist;
                        cp_id = i;
                    }
                }

                std::rotate(C[0].begin(), C[0].begin() + cp_id, C[0].end());

                polyline.clear();
                polyline.resize(C[0].size() + 1);

                for (size_t i = 0; i < C[0].size(); i++)
                {
                    IK::Point_3 p(C[0][i].x, C[0][i].y, 0);
                    p = p.transform(xform_to_xy_Inv); // Rotate back to 3D
                    polyline[i] = p;
                }

                polyline[C[0].size()] = polyline[0]; // Close
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

    int are_points_inside(CGAL_Polyline &polyline, IK::Plane_3 &plane, std::vector<IK::Point_3> &test_points, std::vector<int> &test_points_ids)
    {
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline a = polyline;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation - Orient to XY
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_to_xy = internal::plane_to_xy(polyline[0], plane);

        for (auto it = a.begin(); it != a.end(); ++it)
            *it = it->transform(xform_to_xy);

        std::vector<IK::Point_3> pts(test_points.size());
        for (size_t i = 0; i < test_points.size(); i++)
        {
            pts[i] = test_points[i];
            pts[i] = xform_to_xy.transform(pts[i]);
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::PointD> pathA(a.size() - 1);
        for (size_t i = 0; i < a.size() - 1; i++)
        {
            pathA[i] = Clipper2Lib::PointD(a[i].x(), a[i].y() );
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // Check if point is inside polyline
        /////////////////////////////////////////////////////////////////////////////////////

        size_t count = 0;

        for (size_t i = 0; i < test_points.size(); i++)
        {
            auto p = Clipper2Lib::PointD(pts[i].x() , pts[i].y() );

            Clipper2Lib::PointInPolygonResult result = Clipper2Lib::PointInPolygon(p, pathA);

            if (result != Clipper2Lib::PointInPolygonResult::IsOutside)
            {
                test_points_ids.emplace_back(i);
                count++;
            }
        }

        return count;
    }

    bool bounding_rectangle(CGAL_Polyline &polyline, IK::Plane_3 &plane, CGAL_Polyline &result)
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
        CGAL::Aff_transformation_3<IK> xform_to_xy = internal::plane_to_xy(polyline_copy[0], plane);
        CGAL::Aff_transformation_3<IK> xform_to_xy_Inv = xform_to_xy.inverse();
        for (auto it = polyline_copy.begin(); it != polyline_copy.end(); ++it)
            *it = it->transform(xform_to_xy);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::PathD polyline_clipper(polyline_copy.size() - 1);

        for (size_t i = 0; i < polyline_copy.size() - 1; i++)
            polyline_clipper[i] = Clipper2Lib::PointD(polyline_copy[i].x() , polyline_copy[i].y());

        /////////////////////////////////////////////////////////////////////////////////////
        // Get Bounding Box
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::RectD br = Clipper2Lib::GetBounds(polyline_clipper);

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to CGAL and orient back to 3D
        /////////////////////////////////////////////////////////////////////////////////////
        result.reserve(5);
        result = CGAL_Polyline{
            IK::Point_3(br.left , br.bottom , 0),
            IK::Point_3(br.right , br.bottom , 0),
            IK::Point_3(br.right , br.top, 0),
            IK::Point_3(br.left, br.top, 0)};
        for (size_t i = 0; i < 4; i++)
            result[i] = result[i].transform(xform_to_xy_Inv);
        result.emplace_back(result[0]);

        return true;
    }

    bool offset_and_divide_to_points(std::vector<IK::Point_3> &result, std::vector<IK::Point_3> &polygon, double offset_distance, double division_distance)
    {
        // offset polygon
        IK::Point_3 center;
        IK::Plane_3 plane;
        CGAL_Polyline polygon_copy = polygon;
        internal::get_fast_plane(polygon_copy, center, plane);
        clipper_util::offset_in_3d(polygon_copy, plane, offset_distance);

        // interpolate two points in steps
        for (size_t i = 0; i < polygon_copy.size() - 1; i++)
        {
            std::vector<IK::Point_3> division_points;
            int divisions = (int)std::min(100.0, std::sqrt(CGAL::squared_distance(polygon_copy[i], polygon_copy[i + 1])) / division_distance);
            cgal_vector_util::interpolate_points(polygon_copy[i], polygon_copy[i + 1], divisions, division_points, 2);
            result.insert(result.end(), division_points.begin(), division_points.end());
        }

        // if the polyline is open add the end of the polyline to the points
        if (CGAL::squared_distance(polygon_copy.front(), polygon_copy.back()) > wood_globals::CLIPPER_AREA)
            result.emplace_back(polygon_copy.back());

        return true;
    }
}
