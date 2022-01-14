#pragma once
#include "cgal.h"

namespace cgal_box_search {

    inline double length(double x, double y, double z) {
        //double ON_DBL_MIN = 2.22507385850720200e-308;

        double len;
        x = fabs(x);
        y = fabs(y);
        z = fabs(z);
        if (y >= x && y >= z) {
            len = x;
            x = y;
            y = len;
        } else if (z >= x && z >= y) {
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
        if (x > ON_DBL_MIN) {
            y /= x;
            z /= x;
            len = x * sqrt(1.0 + y * y + z * z);
        } else if (x > 0.0 && ON_IS_FINITE(x))
            len = x;
        else
            len = 0.0;

        return len;
    }

    inline bool unitize(IK::Vector_3& v) {

        bool rc = false;
        // Since x,y,z are floats, d will not be denormalized and the
        // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

        double d = length(v.x(), v.y(), v.z());
        if (d > 0.0) {
            double dx = v.x();
            double dy = v.y();
            double dz = v.z();
            v = IK::Vector_3(
                (dx / d),
                (dy / d),
                (dz / d));
            rc = true;
        }
        return rc;
    }

    inline CGAL::Aff_transformation_3<IK> plane_to_xy( IK::Point_3 O0, IK::Plane_3 plane) {
        auto X0 = plane.base1();
        auto Y0 = plane.base2();
        auto Z0 = plane.orthogonal_vector();
        unitize(X0);
        unitize(Y0);
        unitize(Z0);

        // transformation maps P0 to P1, P0+X0 to P1+X1, ...

        //Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

        //Rotate ->
        CGAL::Aff_transformation_3<IK> F0(
            X0.x(), X0.y(), X0.z(),
            Y0.x(), Y0.y(), Y0.z(),
            Z0.x(), Z0.y(), Z0.z());

        return F0 * T0;
    }

    inline IK::Point_3 point_at(const IK::Segment_3& l, double t) {

        const double s = 1.0 - t;

        return IK::Point_3(
            (l[0].x() == l[1].x()) ? l[0].x() : s * l[0].x() + t * l[1].x(),
            (l[0].y() == l[1].y()) ? l[0].y() : s * l[0].y() + t * l[1].y(),
            (l[0].z() == l[1].z()) ? l[0].z() : s * l[0].z() + t * l[1].z());
    }

    inline bool closest_point_to(const IK::Point_3& point, const IK::Segment_3& s, double& t) {
        bool rc = false;

        const IK::Vector_3 D = s.to_vector();
        const double DoD = D.squared_length();

        if (DoD > 0.0) {

            if ((point - s[0]).squared_length() <= (point - s[1]).squared_length())
                t = ((point - s[0]) * D) / DoD;
            else
                t = 1.0 + ((point - s[1]) * D) / DoD;

            rc = true;
        } else { // (GBA) Closest point to a degenerate line works as well
            t = 0.0;
            rc = true;
        }

        return rc;
    }

    inline bool line_line_intersection(IK::Segment_3& s0, IK::Segment_3& s1, bool finite, IK::Point_3& p0, IK::Point_3& p1) {


        //Unlike lines, segments don’t extend beyond their start and end points.


        IK::Vector_3 v0 = s0.to_vector();
        IK::Vector_3 v1 = s1.to_vector();
        IK::Vector_3 normal = CGAL::cross_product(v0, v1);

        double angle = 90.0 - fabs(CGAL::approximate_angle(v0, v1) - 90);


        //The 2 intersection points of intersecting segments are identical.
        if (CGAL::squared_distance(s0[0], s1[0]) < GlobalTolerance || CGAL::squared_distance(s0[0], s1[1]) < GlobalTolerance) {
            p0 = s0[0];
            p1 = s0[0];
            return true;
        } else if (CGAL::squared_distance(s0[1], s1[0]) < GlobalTolerance || CGAL::squared_distance(s0[1], s1[1]) < GlobalTolerance) {
            p0 = s0[1];
            p1 = s0[1];
            return true;
        }

        // Collinear segments or //Parallel segments don’t intersect.

        if (normal == CGAL::NULL_VECTOR || angle < 1) {

            return false;


            IK::Line_3 l0 = s0.supporting_line();
            IK::Line_3 l1 = s1.supporting_line();
            IK::Vector_3 v0 = s0.to_vector();
            v0 = v0 / CGAL::approximate_sqrt(v0 * v0);
            IK::Vector_3 v1 = s1.to_vector();
            v1 = v1 / CGAL::approximate_sqrt(v1 * v1);

            std::vector<std::pair<IK::FT, IK::FT>> points_along_lines;

            auto create_projection = [&](const IK::Point_3& p)
            {
                IK::Point_3 p0 = l0.projection(p);
                IK::Vector_3 vec0(s0.source(), p0);
                IK::FT pos0 = vec0 * v0;

                IK::Point_3 p1 = l1.projection(p);
                IK::Vector_3 vec1(s1.source(), p1);
                IK::FT pos1 = vec1 * v1;

                points_along_lines.emplace_back(pos0, pos1);
            };
            create_projection(s0.source());
            create_projection(s0.target());
            create_projection(s1.source());
            create_projection(s1.target());

            std::sort(points_along_lines.begin(), points_along_lines.end(),
                [](const auto& a, const auto& b) -> bool
                {
                    return a.first < b.first;
                });

            IK::Segment_3 seg0(s0.source() + points_along_lines[1].first * v0,
                s0.source() + points_along_lines[2].first * v0);
            IK::Segment_3 seg1(s1.source() + points_along_lines[1].second * v1,
                s1.source() + points_along_lines[2].second * v1);


            //Not sure this is correct
            p0 = seg0[0];
            p1 = seg0[1];
            //return Result(seg0, seg1);
            return true;

        } else {//Skew segments have two different intersection points.

            //Co-linear - check ends


            //Works only if segments are not co-linear
            IK::Plane_3 plane(s0[0], normal);
            CGAL::Aff_transformation_3<IK> xform = plane_to_xy(s0[0], plane);
            CGAL::Aff_transformation_3<IK> xform_Inv = xform.inverse();

            IK::Point_3 p0_0 = xform.transform(s0[0]);
            IK::Point_3 p0_1 = xform.transform(s0[1]);

            IK::Point_3 p1_0 = xform.transform(s1[0]);
            IK::Point_3 p1_1 = xform.transform(s1[1]);

            IK::Line_2 l0(IK::Point_2(p0_0.hx(), p0_0.hy()), IK::Point_2(p0_1.hx(), p0_1.hy()));
            IK::Line_2 l1(IK::Point_2(p1_0.hx(), p1_0.hy()), IK::Point_2(p1_1.hx(), p1_1.hy()));
            IK::Line_3 l1_3d = s1.supporting_line();
            //CGAL_Debug(p0_0,true);
            //CGAL_Debug(p0_1, true);
            //CGAL_Debug(p1_0, true);
            //CGAL_Debug(p1_1, true);

            //Colinearity check!!!!!!!!

            const auto result = CGAL::intersection(l0, l1);


            if (result) {
                if (const IK::Point_2* p = boost::get<IK::Point_2 >(&*result)) {
                    p0 = IK::Point_3(p->hx(), p->hy(), 0);
                    p0 = xform_Inv.transform(p0);


                    if (finite) {
                        double t0, t1;
                        closest_point_to(p0, s0, t0);


                        bool outside0 = t0 > 1.0 || t0 < 0.0;

                        if (outside0) {
                            t0 = std::max(0.0, std::min(t0, 1.0));
                            p0 = point_at(s0, t0);
                        }


                        p1 = l1_3d.projection(p0);//is it ok?
                        closest_point_to(p1, s1, t1);


                        bool outside1 = t1 > 1.0 || t1 < 0.0;

                        if (outside1) {
                            t1 = std::max(0.0, std::min(t1, 1.0));
                            p1 = point_at(s1, t1);
                        }






                    }

                } else {
                    return false;
                }
            } else {
                return false;
            }
            return true;
        }
        return true;
    }



    //https://github.com/CGAL/cgal/issues/6089
//https://gist.github.com/sloriot/0943fec878401e04930b7bcbd73baf91
    inline void intersecting_sequences_of_dD_iso_oriented_boxes(std::vector<CGAL_Polyline>& polylines, std::vector<std::vector<double>>& polylines_segment_radii, double& min_distance,
        std::vector<std::array<int, 4>>& polyline0_id_segment0_id_polyline1_id_segment1_id,
        std::vector<std::array<IK::Point_3, 2>>& point_pairs
    ) {

#ifdef DEBUG
        CGAL_Debug(polylines.size(), polylines_segment_radii.size());
#endif

        //std::vector< std::vector<IK::Point_3> > polylines =    {
        //    { {0,0,0}, {1, 0, 0}, {3, 0, 0} },
        //    { {2, -1 , 0}, {2, 1 , 0} }
        //};




        //CGAL_Debug(polylines.size());
        /////////////////////////////////////////////////////////////////////
        //Segment callback
        /////////////////////////////////////////////////////////////////////

        auto segment = [&polylines](std::size_t pid, std::size_t sid)
        {
            return IK::Segment_3(polylines[pid][sid], polylines[pid][sid + 1]);
            //return EK::Segment_3(
            //    EK::Point_3(polylines[pid][sid][0], polylines[pid][sid][1], polylines[pid][sid][2]),
            //    EK::Point_3(polylines[pid + 1][sid][0], polylines[pid][sid + 1][1], polylines[pid][sid + 1][2])
            //);
        };

        auto segment_inflated = [&polylines, &polylines_segment_radii](std::size_t pid, std::size_t sid)
        {

            IK::Segment_3 segment(polylines[pid][sid], polylines[pid][sid + 1]);

            //IK::Vector_3 v0 = segment.to_vector();
            //segment = IK::Segment_3(segment[0] - v0*1, segment[1] + v0*1);


            double radius = polylines_segment_radii[pid][sid];

            IK::Vector_3 zaxis = segment.to_vector();
            IK::Plane_3 plane(segment[0], zaxis);
            IK::Vector_3 x_axis = plane.base1();



            IK::Vector_3 y_axis = plane.base2();//CGAL::cross_product(x_axis, zaxis);

            unitize(x_axis);
            unitize(y_axis);
            //x_axis = cgal_vector_util::Unit(x_axis);
            //y_axis = cgal_vector_util::Unit(y_axis);
            x_axis *= radius;
            y_axis *= radius;

            //CGAL_Debug(x_axis);
            //CGAL_Debug(y_axis);
            //CGAL_Debug(radius);

            std::vector<IK::Point_3> pts = {
                segment[0] + x_axis + y_axis,
                segment[0] - x_axis + y_axis,
                segment[0] - x_axis - y_axis,
                segment[0] + x_axis - y_axis,

                segment[1] + x_axis + y_axis,
                segment[1] - x_axis + y_axis,
                segment[1] - x_axis - y_axis,
                segment[1] + x_axis - y_axis,
            };


            CGAL::Bbox_3 box = CGAL::bbox_3(pts.begin(), pts.end(), IK());
            IK::Segment_3 segment_inflated(IK::Point_3(box.xmin(), box.ymin(), box.zmin()), IK::Point_3(box.xmax(), box.ymax(), box.zmax()));
            //CGAL_Debug(box.xmin(), box.ymin(), box.zmin());
            //CGAL_Debug(box.xmax(), box.ymax(), box.zmax());

            return segment_inflated;
        };

        /////////////////////////////////////////////////////////////////////
        // Create the corresponding vector of bounding boxes
        /////////////////////////////////////////////////////////////////////
        std::vector<Box> boxes;
        for (std::size_t pid = 0; pid < polylines.size(); ++pid)
            for (std::size_t sid = 0; sid < polylines[pid].size() - 1; ++sid)
                boxes.push_back(Box(segment_inflated(pid, sid).bbox(), std::make_pair(pid, sid)));




        /////////////////////////////////////////////////////////////////////
        //do_interesect call_back
        /////////////////////////////////////////////////////////////////////

        std::map<uint64_t, std::tuple<double, int, int, int, int >> pair_collisions;
        std::vector< std::tuple<double, int, int, int, int >>pair_collisionslist;
        auto callback = [&segment, &min_distance, &pair_collisions, &pair_collisionslist](const Box& b1, const Box& b2)
        {


            if (b1.info().first != b2.info().first) {


                IK::Segment_3 s0 = segment(b1.info().first, b1.info().second);
                IK::Segment_3 s1 = segment(b2.info().first, b2.info().second);
                //IK::Vector_3 v0 = s0.to_vector();
                // IK::Vector_3 v1 = s1.to_vector();
                // s0 = IK::Segment_3(s0[0] - v0, s0[1] + v0);
                // s1 = IK::Segment_3(s1[0] - v1, s1[1] + v1);
                double distance = CGAL::squared_distance(s0, s1);
#ifdef DEBUG
                CGAL_Debug(b1.info().first, b2.info().first, distance);
#endif

                if (distance < min_distance * min_distance) {

                    size_t first_0, first_1;
                    bool flipped = false;
                    uint64_t id;
                    if (b2.info().first > b1.info().first) {
                        flipped = true;
                        id = (uint64_t)b2.info().first << 32 | b1.info().first;
                    } else {
                        flipped = false;
                        id = (uint64_t)b1.info().first << 32 | b2.info().first;
                    }


                    auto dist_and_segment_ids = flipped ? std::make_tuple(distance, b1.info().first, b1.info().second, b2.info().first, b2.info().second) : std::make_tuple(distance, b2.info().first, b2.info().second, b1.info().first, b1.info().second);

                    //Add elements to std::map
                    if (pair_collisions.find(id) == pair_collisions.end()) {
                        //not found
                        pair_collisions.insert(std::make_pair(id, dist_and_segment_ids));
                        pair_collisionslist.push_back(dist_and_segment_ids);

                    } else if (distance < std::get<0>(pair_collisions[id])) {
                        pair_collisions.insert(std::make_pair(id, dist_and_segment_ids));
                        // found and distance is smaller that before found
                        pair_collisions[id] = dist_and_segment_ids;
                        pair_collisionslist.push_back(dist_and_segment_ids);
                    }
                    //}

                }//check if lines closer than the given distance

            }//check if boxes do not belong to the same group b.info().first

        };

        /////////////////////////////////////////////////////////////////////
        //self intersection
        /////////////////////////////////////////////////////////////////////
        CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), callback);

        /////////////////////////////////////////////////////////////////////
        //Iterate the result, get insertion points and parameter on the lines
        /////////////////////////////////////////////////////////////////////
        polyline0_id_segment0_id_polyline1_id_segment1_id.reserve(pair_collisions.size() * 4);
        point_pairs.reserve(point_pairs.size() * 2);

        for (auto const& x : pair_collisions) {

            auto& v = x.second;
            IK::Segment_3 s0 = segment(std::get<1>(v), std::get<2>(v));
            IK::Segment_3 s1 = segment(std::get<3>(v), std::get<4>(v));



            IK::Point_3 p0;
            IK::Point_3 p1;
            bool r = line_line_intersection(s0, s1, true, p0, p1);
            if (!r) continue;
            point_pairs.emplace_back(std::array<IK::Point_3, 2>{p0, p1});

#ifdef DEBUG
            CGAL_Debug(p0);
            CGAL_Debug(p1);
#endif


        }
    }


}
