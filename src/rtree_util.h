#pragma once
#include "stdafx.h"
//#include "rtree.h"

namespace rtree_util {
    //std::vector<std::vector<int>>& input_JOINTS_TYPES
    // std::vector<int>& input_polyline_pairs_indices

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
        }
        else { // (GBA) Closest point to a degenerate line works as well
            t = 0.0;
            rc = true;
        }

        return rc;
    }

    inline double get_closest_distance(const IK::Point_3& point, CGAL_Polyline& s, int& edge) {
        edge = 0;
        IK::Segment_3 segment(s[0], s[1]);
        double closestDistance = 999999;

        for (int i = 0; i < s.size() - 1; i++) {
            IK::Segment_3 segment_(s[i], s[i + 1]);

            double t;
            closest_point_to(point, segment_, t);

            double closestDistanceTemp = std::abs(CGAL::squared_distance(point, point_at(segment_, t)));
            if (closestDistanceTemp < closestDistance) {
                closestDistance = closestDistanceTemp;
                edge = i;
            }

            if (closestDistance < GlobalToleranceSquare)
                break;
        }

        return closestDistance;
    }

    inline void find_closest_plateside_to_line(
        std::vector<CGAL_Polyline>& input_polyline_pairs,
        std::vector<IK::Segment_3>& input_insertion_lines,
        std::vector<std::vector<IK::Vector_3>>& output_insertion_vectors,
        double tol = 0.01) {
        //Create a container to store full empty insertion vectors
        int n = (int)(input_polyline_pairs.size() * 0.5);

        //Init empty array
        output_insertion_vectors = std::vector<std::vector<IK::Vector_3>>(n);
        for (int i = 0; i < n; i++)
            output_insertion_vectors[i] = std::vector<IK::Vector_3>(input_polyline_pairs[i * 2].size() + 1, IK::Vector_3(0, 0, 0));

        //for (auto& l : input_insertion_lines) {
        //    CGAL_Debug(l[0]);
        //    CGAL_Debug(l[1]);
        //}
        //CGAL_Debug(false);
        //CGAL_Debug(false);
        //CGAL_Debug(false);

        if (input_insertion_lines.size() == 0) return;

        //Create RTree
        RTree<int, double, 3> tree;

        //Insert AABB
        for (int i = 0; i < n; i++) {
            //Create copy of a polyline and transform points
            CGAL_Polyline twoPolylines;
            twoPolylines.reserve(input_polyline_pairs[i * 2].size() + input_polyline_pairs[i * 2 + 1].size());
            //CGAL_Debug(false);

            for (auto& p : input_polyline_pairs[i * 2]) {
                //CGAL_Debug(p);
                twoPolylines.emplace_back(p);
            }

            for (auto& p : input_polyline_pairs[i * 2 + 1]) {
                //CGAL_Debug(p);
                twoPolylines.emplace_back(p);
            }

            CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

            CGAL_Polyline AABB_Min_Max = {
                IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
                IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
            };

            AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

            double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
            double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
            tree.Insert(min, max, i);
        }
        //CGAL_Debug(false);
        //CGAL_Debug(false);
        //CGAL_Debug(false);
        int collision_count = 0;

        for (int i = 0; i < input_insertion_lines.size(); i++) {//AABB.size()
            //Get lines points
            IK::Point_3 p0(input_insertion_lines[i][0].hx(), input_insertion_lines[i][0].hy(), input_insertion_lines[i][0].hz());
            IK::Point_3 p1(input_insertion_lines[i][1].hx(), input_insertion_lines[i][1].hy(), input_insertion_lines[i][1].hz());
            IK::Vector_3 v = p1 - p0;
            //CGAL_Debug(p0);
            // CGAL_Debug(p1);
             //CGAL_Debug(v);
            //printf("\n");

            //std::vector<int> result;
            auto callback = [i, &input_insertion_lines, &input_polyline_pairs, &output_insertion_vectors, &collision_count, &p0, &p1, &v](int foundValue) -> bool
            {
                int edge = 0;
                double closest_distance = std::abs(get_closest_distance(p0, input_polyline_pairs[foundValue * 2 + 0], edge));//first polyline - one starting point

                //RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
                double flag = closest_distance < GlobalToleranceSquare * 100;
                //CGAL_Debug(closest_distance);
                if (flag) {
                    //CGAL_Debug(foundValue, edge);
                    output_insertion_vectors[foundValue][edge + 2] = v;
                    //RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge+2);
                    collision_count++;
                    return true;
                }

                edge = 0;
                closest_distance = std::abs(get_closest_distance(p0, input_polyline_pairs[foundValue * 2 + 1], edge));//second polyline - one starting point
                //CGAL_Debug(closest_distance);
                //RhinoApp().Print(L"Element %i Closest Distance 1: %f \n", foundValue,  closest_distance);
                flag = closest_distance < GlobalToleranceSquare * 100;
                if (flag) {
                    //CGAL_Debug(foundValue, edge);
                    output_insertion_vectors[foundValue][edge + 2] = v;
                    //RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge+2);
                    collision_count++;
                    return true;
                }

                return true;
                //return false;
            };

            CGAL_Polyline pline{ input_insertion_lines[i][0] ,input_insertion_lines[i][1] };
            CGAL::Bbox_3 AABB = CGAL::bbox_3(pline.begin(), pline.end(), IK());

            double min[3] = { AABB.xmin() - tol, AABB.ymin() - tol, AABB.zmin() - tol };
            double max[3] = { AABB.xmax() + tol, AABB.ymax() + tol, AABB.zmax() + tol };
            int nhits = tree.Search(min, max, callback);//callback in this method call callback above
        }

        //CGAL_Debug(false);
        //CGAL_Debug(false);
        //CGAL_Debug(collision_count);
        //CGAL_Debug(false);
        //RhinoApp().Print(L" found insertion vectors: %i \n", collision_count);
        if (collision_count == 0) output_insertion_vectors.clear();
    }

    inline void find_closest_plateside_to_indexedpoint(
        std::vector<CGAL_Polyline>& input_polyline_pairs,
        std::vector<IK::Point_3>& input_points,
        std::vector<int>& input_id,
        std::vector<std::vector<int>>& output_JOINTS_TYPES,
        double tol = 0.01) {
        //Create a container to store full empty insertion vectors
        int n = (int)(input_polyline_pairs.size() * 0.5);

        output_JOINTS_TYPES = std::vector<std::vector<int>>(n);

        for (int i = 0; i < n; i++)
            output_JOINTS_TYPES[i] = std::vector<int>(input_polyline_pairs[i * 2].size() + 1, -1);

        if (input_points.size() == 0) return;
        if (input_points.size() != input_id.size()) return;

        //Create RTree
        RTree<int, double, 3> tree;

        //Insert AABB
        for (int i = 0; i < n; i++) {
            //Create copy of a polyline and transform points
            CGAL_Polyline twoPolylines;
            twoPolylines.reserve(input_polyline_pairs[i * 2].size() + input_polyline_pairs[i * 2 + 1].size());

            for (auto& p : input_polyline_pairs[i * 2])
                twoPolylines.emplace_back(p);

            for (auto& p : input_polyline_pairs[i * 2 + 1])
                twoPolylines.emplace_back(p);

            CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

            CGAL_Polyline AABB_Min_Max = {
                IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
                IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
            };

            AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

            double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
            double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
            tree.Insert(min, max, i);
        }

        int collision_count = 0;

        for (int i = 0; i < input_points.size(); i++) {//AABB.size()
          //RhinoApp().Print(L" x %f y %f z %f \n", text_dots[i].first.x, text_dots[i].first.y, text_dots[i].first.z);
          //std::vector<int> result;
            auto callback = [i, &input_polyline_pairs, &input_points, &input_id, &output_JOINTS_TYPES, &collision_count](int foundValue) -> bool
            {
                //RhinoApp().Print(L" __________________ \n");
                //Get lines points
                IK::Point_3 p = input_points[i];
                //RhinoApp().Print(L" x %f y %f z %f \n", p.hx(), p.hy(), p.hz());

                //Check the distance between top and bottom outlines edge
                int edge = 0;
                double closest_distance = std::abs(cgal_polyline_util::closest_distance(p, input_polyline_pairs[foundValue * 2 + 0], edge));
                //RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
                double flag = closest_distance < GlobalToleranceSquare * 100;
                if (flag) {
                    int face_or_edge = input_id[i] < 0 ? 0 : 2 + edge;
                    int type = input_id[i] < -100 ? 0 : std::abs(input_id[i]);
                    output_JOINTS_TYPES[foundValue][face_or_edge] = type;
                    //RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge);
                    //RhinoApp().Print(L" Closest Distance 0: %f \n", closest_distance);
                    collision_count++;
                    return true;
                }

                closest_distance = std::abs(cgal_polyline_util::closest_distance(p, input_polyline_pairs[foundValue * 2 + 1], edge));
                //RhinoApp().Print(L" Element %i Closest Distance 0: %f \n", foundValue, closest_distance);
                flag = closest_distance < GlobalToleranceSquare * 100;
                if (flag) {
                    int face_or_edge = input_id[i] < 0 ? 1 : 2 + edge;
                    int type = input_id[i] < -100 ? 0 : std::abs(input_id[i]);
                    output_JOINTS_TYPES[foundValue][face_or_edge] = std::abs(type);
                    //RhinoApp().Print(L" Element %i edge %i \n", foundValue, edge);
                    //RhinoApp().Print(L" Closest Distance 1: %f \n", closest_distance);
                    collision_count++;
                    return true;
                }

                return true;
            };

            double min[3] = { input_points[i].hx() - tol, input_points[i].hy() - tol, input_points[i].hz() - tol };
            double max[3] = { input_points[i].hx() + tol, input_points[i].hy() + tol, input_points[i].hz() + tol };
            int nhits = tree.Search(min, max, callback);//callback in this method call callback above
            //RhinoApp().Print(L" ________%i__________ \n", nhits);
        }
        if (collision_count == 0) output_JOINTS_TYPES.clear();
    }
}
