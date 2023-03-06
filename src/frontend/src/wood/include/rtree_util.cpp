

#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "rtree_util.h"

namespace rtree_util
{

    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in rtree to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        IK::Point_3 point_at(const IK::Segment_3& line, const double& t)
        {
            const double s = 1.0 - t;

            return IK::Point_3(
                (line[0].x() == line[1].x()) ? line[0].x() : s * line[0].x() + t * line[1].x(),
                (line[0].y() == line[1].y()) ? line[0].y() : s * line[0].y() + t * line[1].y(),
                (line[0].z() == line[1].z()) ? line[0].z() : s * line[0].z() + t * line[1].z());
        }

        bool closest_point_to(const IK::Point_3& point, const IK::Segment_3& line, double& t)
        {
            bool rc = false;

            const IK::Vector_3 D = line.to_vector();
            const double DoD = D.squared_length();

            if (DoD > 0.0)
            {
                if ((point - line[0]).squared_length() <= (point - line[1]).squared_length())
                    t = ((point - line[0]) * D) / DoD;
                else
                    t = 1.0 + ((point - line[1]) * D) / DoD;

                rc = true;
            }
            else
            { // (GBA) Closest point to a degenerate line works as well
                t = 0.0;
                rc = true;
            }

            return rc;
        }

        double get_closest_distance(const IK::Point_3& point, CGAL_Polyline& polyline, size_t& edge_id)
        {
            edge_id = 0;
            IK::Segment_3 segment(polyline[0], polyline[1]);
            double closest_distance = DBL_MAX;

            for (size_t i = 0; i < polyline.size() - 1; i++)
            {
                IK::Segment_3 segment_(polyline[i], polyline[i + 1]);

                double t;
                closest_point_to(point, segment_, t);

                double closest_distance_temp = std::abs(CGAL::squared_distance(point, point_at(segment_, t)));
                if (closest_distance_temp < closest_distance_temp)
                {
                    closest_distance = closest_distance_temp;
                    edge_id = i;
                }

                if (closest_distance < wood_globals::DISTANCE_SQUARED)
                    break;
            }

            return closest_distance;
        }
    }

    void find_closest_plateside_to_line(
        std::vector<CGAL_Polyline>& input_polyline_pairs,
        std::vector<IK::Segment_3>& input_insertion_lines,
        std::vector<std::vector<IK::Vector_3>>& output_insertion_vectors)
    {
        // Create a container to store full empty insertion vectors
        size_t n = (size_t)(input_polyline_pairs.size() * 0.5);

        // Init empty array
        output_insertion_vectors = std::vector<std::vector<IK::Vector_3>>(n);
        for (auto i = 0; i < n; i++)
            output_insertion_vectors[i] = std::vector<IK::Vector_3>(input_polyline_pairs[i * 2].size() + 1, IK::Vector_3(0, 0, 0));

        if (input_insertion_lines.size() == 0)
            return;

        // Create RTree
        RTree<size_t, double, 3> tree;

        // Insert AABB
        for (auto i = 0; i < n; i++)
        {
            // Create copy of a polyline and transform points
            CGAL_Polyline twoPolylines;
            twoPolylines.reserve(input_polyline_pairs[i * 2].size() + input_polyline_pairs[i * 2 + 1].size());

            for (auto& p : input_polyline_pairs[i * 2])
            {
                twoPolylines.emplace_back(p);
            }

            for (auto& p : input_polyline_pairs[i * 2 + 1])
            {
                twoPolylines.emplace_back(p);
            }

            CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

            CGAL_Polyline AABB_Min_Max = {
                IK::Point_3(AABB.xmin() - 1 * wood_globals::DISTANCE, AABB.ymin() - 1 * wood_globals::DISTANCE, AABB.zmin() - 1 * wood_globals::DISTANCE),
                IK::Point_3(AABB.xmax() + 1 * wood_globals::DISTANCE, AABB.ymax() + 1 * wood_globals::DISTANCE, AABB.zmax() + 1 * wood_globals::DISTANCE),
            };

            AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

            double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
            double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
            tree.Insert(min, max, i);
        }

        int collision_count = 0;

        for (auto i = 0; i < input_insertion_lines.size(); i++)
        {
            // Get lines points
            IK::Point_3 p0(input_insertion_lines[i][0].hx(), input_insertion_lines[i][0].hy(), input_insertion_lines[i][0].hz());
            IK::Point_3 p1(input_insertion_lines[i][1].hx(), input_insertion_lines[i][1].hy(), input_insertion_lines[i][1].hz());
            IK::Vector_3 v = p1 - p0;

            auto callback = [i, &input_insertion_lines, &input_polyline_pairs, &output_insertion_vectors, &collision_count, &p0, &p1, &v](int foundValue) -> bool
            {
                size_t edge = 0;
                double closest_distance = std::abs(internal::get_closest_distance(p0, input_polyline_pairs[foundValue * 2 + 0], edge)); // first polyline - one starting point

                // RhinoApp().Print(L"Element %i Closest Distance 0: %f \n", foundValue,  closest_distance);
                double flag = closest_distance < wood_globals::DISTANCE_SQUARED * 100;
                // CGAL_Debug(closest_distance);
                if (flag)
                {
                    // CGAL_Debug(foundValue, edge);
                    output_insertion_vectors[foundValue][edge + 2] = v;

                    collision_count++;
                    return true;
                }

                edge = 0;
                closest_distance = std::abs(internal::get_closest_distance(p0, input_polyline_pairs[foundValue * 2 + 1], edge)); // second polyline - one starting point
                // CGAL_Debug(closest_distance);

                flag = closest_distance < wood_globals::DISTANCE_SQUARED * 100;
                if (flag)
                {
                    // CGAL_Debug(foundValue, edge);
                    output_insertion_vectors[foundValue][edge + 2] = v;

                    collision_count++;
                    return true;
                }

                return true;
            };

            CGAL_Polyline pline{ input_insertion_lines[i][0], input_insertion_lines[i][1] };
            CGAL::Bbox_3 AABB = CGAL::bbox_3(pline.begin(), pline.end(), IK());

            double min[3] = { AABB.xmin() - wood_globals::DISTANCE, AABB.ymin() - wood_globals::DISTANCE, AABB.zmin() - wood_globals::DISTANCE };
            double max[3] = { AABB.xmax() + wood_globals::DISTANCE, AABB.ymax() + wood_globals::DISTANCE, AABB.zmax() + wood_globals::DISTANCE };
            auto nhits = tree.Search(min, max, callback); // callback in this method call callback above
        }

        if (collision_count == 0)
            output_insertion_vectors.clear();
    }

    void find_closest_plateside_to_indexedpoint(
        std::vector<CGAL_Polyline>& input_polyline_pairs,
        std::vector<IK::Point_3>& input_points,
        std::vector<int>& input_points_types,
        std::vector<std::vector<int>>& output_joint_types)
    {
        // Create a container to store full empty insertion vectors
        size_t n = (size_t)(input_polyline_pairs.size() * 0.5);

        output_joint_types = std::vector<std::vector<int>>(n);

        for (size_t i = 0; i < n; i++)
            output_joint_types[i] = std::vector<int>(input_polyline_pairs[i * 2].size() + 1, -1);

        if (input_points.size() == 0)
            return;

        if (input_points.size() != input_points_types.size())
            return;

        // Create RTree
        RTree<size_t, double, 3> tree;

        // Insert AABB
        for (size_t i = 0; i < n; i++)
        {
            // Create copy of a polyline and transform points
            CGAL_Polyline twoPolylines;
            twoPolylines.reserve(input_polyline_pairs[i * 2].size() + input_polyline_pairs[i * 2 + 1].size());

            for (auto& p : input_polyline_pairs[i * 2])
                twoPolylines.emplace_back(p);

            for (auto& p : input_polyline_pairs[i * 2 + 1])
                twoPolylines.emplace_back(p);

            CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

            CGAL_Polyline AABB_Min_Max = {
                IK::Point_3(AABB.xmin() - 1 * wood_globals::DISTANCE, AABB.ymin() - 1 * wood_globals::DISTANCE, AABB.zmin() - 1 * wood_globals::DISTANCE),
                IK::Point_3(AABB.xmax() + 1 * wood_globals::DISTANCE, AABB.ymax() + 1 * wood_globals::DISTANCE, AABB.zmax() + 1 * wood_globals::DISTANCE),
            };

            AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

            double min[3] = { AABB.xmin(), AABB.ymin(), AABB.zmin() };
            double max[3] = { AABB.xmax(), AABB.ymax(), AABB.zmax() };
            tree.Insert(min, max, i);
        }

        size_t collision_count = 0;

        for (size_t i = 0; i < input_points.size(); i++)
        {

            auto callback = [i, &input_polyline_pairs, &input_points, &input_points_types, &output_joint_types, &collision_count](size_t foundValue) -> bool
            {
                // Get lines points
                IK::Point_3 p = input_points[i];

                // Check the distance between top and bottom outlines edge
                size_t edge = 0;
                double closest_distance = std::abs(internal::get_closest_distance(p, input_polyline_pairs[foundValue * 2 + 0], edge));

                double flag = closest_distance < wood_globals::DISTANCE_SQUARED * 100;
                if (flag)
                {
                    size_t face_or_edge = input_points_types[i] < 0 ? 0 : 2 + edge;
                    int type = input_points_types[i] < -100 ? 0 : std::abs(input_points_types[i]);
                    output_joint_types[foundValue][face_or_edge] = type;

                    collision_count++;
                    return true;
                }

                closest_distance = std::abs(internal::get_closest_distance(p, input_polyline_pairs[foundValue * 2 + 1], edge));

                flag = closest_distance < wood_globals::DISTANCE_SQUARED * 100;
                if (flag)
                {
                    auto face_or_edge = input_points_types[i] < 0 ? 1 : 2 + edge;
                    auto type = input_points_types[i] < -100 ? 0 : std::abs(input_points_types[i]);
                    output_joint_types[foundValue][face_or_edge] = std::abs(type);

                    collision_count++;
                    return true;
                }

                return true;
            };

            double min[3] = { input_points[i].hx() - wood_globals::DISTANCE, input_points[i].hy() - wood_globals::DISTANCE, input_points[i].hz() - wood_globals::DISTANCE };
            double max[3] = { input_points[i].hx() + wood_globals::DISTANCE, input_points[i].hy() + wood_globals::DISTANCE, input_points[i].hz() + wood_globals::DISTANCE };
            auto nhits = tree.Search(min, max, callback); // callback in this method call callback above
        }

        if (collision_count == 0)
            output_joint_types.clear();
    }
}
