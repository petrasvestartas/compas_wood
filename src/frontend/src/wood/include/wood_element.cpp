
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "wood_element.h"
namespace wood
{
    element::element()
    {
        // ...
    }

    element::element(int _id) : id(_id)
    {
        // ...
    }

    void element::get_joints_geometry(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output, int what_to_expose, std::vector<std::vector<wood_cut::cut_type>> &output_cut_types)
    {
        // you are in a loop
        // printf("/n %i", id);
        for (size_t i = 0; i < j_mf.size(); i++)
        { // loop wood::joint id
            for (size_t j = 0; j < j_mf[i].size(); j++)
            { // loop joints per each face + 1 undefined

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // correct drilling lines
                // k+=2 means skipping bounding lines or rectangles that are used in other method when joints have to be merged with polygons
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                // output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);  //cut
                // output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]); //direction
                for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size(); k += 2)
                {
                    continue;
                    if (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]))[k] != wood_cut::drill)
                        continue;

                    auto segment = IK::Segment_3(
                        joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k].front(),
                        joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k].back());

                    std::vector<IK::Point_3> intersection_points;
                    intersection_points.reserve(2);

                    if (this->central_polyline.size() > 0)
                    {
                        size_t e = -1;
                        IK::Point_3 cp;
                        double dist = cgal_polyline_util::closest_distance_and_point(segment[0], this->central_polyline, e, cp);
                    }
                    else
                    {

                        for (size_t l = 0; l < this->planes.size(); l++)
                        {

                            IK::Point_3 output;
                            if (cgal_intersection_util::line_plane(segment, this->planes[l], output))
                            {
                                if (clipper_util::is_point_inside(this->polylines[l], this->planes[l], output))
                                {
                                    intersection_points.emplace_back(output);
                                    if (intersection_points.size() == 2)
                                        break;
                                }
                            }
                        }

                        if (intersection_points.size() == 2)
                        {
                            double t0;
                            cgal_polyline_util::closest_point_to(intersection_points[0], segment, t0);
                            double t1;
                            cgal_polyline_util::closest_point_to(intersection_points[1], segment, t1);
                            if (t0 > t1)
                                std::reverse(intersection_points.begin(), intersection_points.end());
                            joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k] = intersection_points;
                            joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k] = intersection_points;
                        }
                    }
                }

                size_t male_or_female = joints[std::get<0>(j_mf[i][j])].v0 == this->id ? 0 : 1;

                bool four_volumes = joints[std::get<0>(j_mf[i][j])].joint_volumes[0].size() > 0 &&
                                    joints[std::get<0>(j_mf[i][j])].joint_volumes[1].size() > 0 &&
                                    joints[std::get<0>(j_mf[i][j])].joint_volumes[2].size() > 0 &&
                                    joints[std::get<0>(j_mf[i][j])].joint_volumes[3].size() > 0;

                std::pair<size_t, size_t> male_or_female_four_volumes = std::make_pair(0, 1);
                if (four_volumes)
                    male_or_female_four_volumes = male_or_female ? std::make_pair(0, 1) : std::make_pair(2, 3);

                switch (what_to_expose)
                {
                case (0): // Plate outlines
                          // if (this->polylines.size() > 1 && i == 0) {
                          //     output[this->id].emplace_back(this->polylines[0]); //cut
                          //     output[this->id].emplace_back(this->polylines[1]); //cut
                          // }
                {

                    CGAL_Polyline joint_area(joints[std::get<0>(j_mf[i][j])].joint_area);
                    if (this->polylines.size() > 0)
                    {
                        IK::Vector_3 plane[4];
                        cgal_polyline_util::get_average_plane(joint_area, plane);
                        IK::Point_3 origin(plane[0].hx(), plane[0].hy(), plane[0].hz());

                        IK::Point_3 center = cgal_polyline_util::center(this->polylines[0]);
                        // std::cout << this->polylines.size() << std::endl;
                        IK::Point_3 p0 = origin + plane[3];
                        IK::Point_3 p1 = origin - plane[3];

                        if (CGAL::has_smaller_distance_to_point(center, p0, p1))
                        {
                            std::reverse(joint_area.begin(), joint_area.end());
                        }
                    }
                    output[this->id].emplace_back(joint_area);
                    break;
                }

                case (1): // wood::joint lines
                    output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[male_or_female]);
                    break;

                case (2): // wood::joint volumes

                    if (four_volumes)
                    {
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[male_or_female_four_volumes.first].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[male_or_female_four_volumes.first]);
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[male_or_female_four_volumes.second].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[male_or_female_four_volumes.second]);
                    }
                    else
                    {
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[0].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[0]);
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[1].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[1]);
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[2].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[2]);
                        if (joints[std::get<0>(j_mf[i][j])].joint_volumes[3].size() > 0)
                            output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[3]);
                    }

                    break;
                case (3):

                    // k+=2 means skipping bounding lines or rectangles that are used in other method when joints have to be merged with polygons
                    // std::cout << " new line \n";
                    for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size(); k += 2)
                    {
                        // std::cout << k << " ";
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);  // cut
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]); // direction
                        if (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j])).size() > k)
                            output_cut_types[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]))[k]); // type
                        else
                            std::cout << "\n ERROR in wood_element.cpp -> get_joints_geometry -> case3 cut_types are not equal to the number of the joint outlines ";
                    }
                    // std::cout << " outlines count: " << output[this->id].size() << " ";
                    // std::cout << " new line \n";

                    break;
                default:
                    break;
                }
            }
        }
    }

    inline bool sort_by_third(const std::tuple<int, bool, double> &a, const std::tuple<int, bool, double> &b)
    {
        return (std::get<2>(a) < std::get<2>(b));
    }

    void element::get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output)
    {
        // you are in a loop

        ///////////////////////////////////////////////////////////////////////////////
        // Copy top and bottom polylines
        ///////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline polyline0 = polylines[0];
        CGAL_Polyline polyline1 = polylines[1];
        auto n = polyline0.size() - 1;
        bool debug = false;

        ///////////////////////////////////////////////////////////////////////////////
        // Sorts joints by edges
        // Get closest parameter to edge and sort by this values
        ///////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < n; i++)
        { // iterate edge count
            for (int j = 0; j < this->j_mf[i + 2].size(); j++)
            { // iterate edges, skipping first two
                if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                    continue;

                IK::Segment_3 element_edge(polyline0[i], polyline0[i + 1]);
                IK::Point_3 joint_point = joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0];
                double t;
                cgal_polyline_util::closest_point_to(joint_point, element_edge, t);
                std::get<2>(j_mf[i + 2][j]) = t;
            }

            std::sort(j_mf[i + 2].begin(), j_mf[i + 2].end(), sort_by_third);
        }

        // CGAL_Debug(polylines[0][0], true);
        // CGAL_Debug(polylines[0][1], true);

        ///////////////////////////////////////////////////////////////////////////////
        // Perform Intersection, skip first two elements because they are top and bottom
        ///////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < n; i++)
        { // iterate sides only
            for (int j = 0; j < this->j_mf[i + 2].size(); j++)
            { //
                if (j_mf[i + 2].size() > 0)
                { // only if one line can be inserted
                    ///////////////////////////////////////////////////////////////////////////////
                    // Skip undefined names and if tiles has more than 1 polyline to insert
                    ///////////////////////////////////////////////////////////////////////////////
                    if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                        continue;
                    if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() != 2)
                        continue;

                    ///////////////////////////////////////////////////////////////////////////////
                    // Take last lines Element outline can be not in the same order as wood::joint outlines Take wood::joint segment and measure its point to the plane
                    ///////////////////////////////////////////////////////////////////////////////
                    bool is_geo_reversed =
                        CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]) >
                        CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0]);

                    if (is_geo_reversed)
                        joints[std::get<0>(j_mf[i + 2][j])].reverse(std::get<1>(j_mf[i + 2][j]));

                    IK::Segment_3 joint_line_0(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[1]);
                    IK::Segment_3 joint_line_1(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[1]);

                    ///////////////////////////////////////////////////////////////////////////////
                    // Take last lines
                    ///////////////////////////////////////////////////////////////////////////////
                    auto prev = (((i - 1) % n) + n) % n;
                    auto next = (((i + 1) % n) + n) % n;
                    auto nextnext = (((i + 2) % n) + n) % n;

                    IK::Segment_3 next_segment_0(polyline0[prev], polyline0[i]);
                    IK::Segment_3 prev_segment_0(polyline0[next], polyline0[nextnext]);
                    IK::Segment_3 next_segment_1(polyline1[prev], polyline1[i]);
                    IK::Segment_3 prev_segment_1(polyline1[next], polyline1[nextnext]);

                    ///////////////////////////////////////////////////////////////////////////////
                    // Intersect them with side lines, same principle must work on both polygons
                    ///////////////////////////////////////////////////////////////////////////////
                    // 1 Perform intersection line-line (needs implementation from rhino)
                    // 3 If intersecting relocate wood::joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
                    IK::Point_3 p0_int, p1_int, p2_int, p3_int;
                    double t0_int, t1_int, t2_int, t3_int;

                    IK::Plane_3 joint_line_plane_0_prev(joint_line_0[0], planes[2 + i].orthogonal_vector());
                    IK::Plane_3 joint_line_plane_0_next(joint_line_0[0], planes[2 + i].orthogonal_vector());
                    IK::Plane_3 joint_line_plane_1_prev(joint_line_1[0], planes[2 + i].orthogonal_vector());
                    IK::Plane_3 joint_line_plane_1_next(joint_line_1[0], planes[2 + i].orthogonal_vector());

                    IK::Vector_3 v(0, 0, 2);

                    bool flag0 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + prev], joint_line_plane_0_prev, planes[0], p0_int); //, joint_line_0, t0_int

                    // output.push_back({ p0_int + v,joint_line_0[0]+ v,cgal_polyline_util::Center(polylines[2 + prev]) + v,cgal_polyline_util::Center(polylines[0]) + v });

                    bool flag1 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + next], joint_line_plane_0_next, planes[0], p1_int); //, joint_line_0, t1_int
                    // output.push_back({ p1_int + v ,joint_line_0[0] + v,cgal_polyline_util::Center(polylines[2 + next]) + v,cgal_polyline_util::Center(polylines[0]) + v });

                    bool flag2 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + prev], joint_line_plane_1_prev, planes[1], p2_int); //, joint_line_1, t2_int
                    // output.push_back({ p2_int + v,joint_line_1[0] + v,cgal_polyline_util::Center(polylines[2 + prev]) + v,cgal_polyline_util::Center(polylines[1]) + v });

                    bool flag3 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + next], joint_line_plane_1_next, planes[1], p3_int); //, joint_line_1, t3_int
                    // output.push_back({ p3_int + v,joint_line_1[0] + v,cgal_polyline_util::Center(polylines[2 + next]) + v,cgal_polyline_util::Center(polylines[1]) + v });

                    ///////////////////////////////////////////////////////////////////////////////
                    // 2 Relocate side segments points to intersection points
                    ///////////////////////////////////////////////////////////////////////////////

                    if (flag0)
                        polyline0[i] = p0_int;
                    if (flag1)
                        polyline0[next] = p1_int;

                    if (flag2)
                        polyline1[i] = p2_int;
                    if (flag3)
                        polyline1[next] = p3_int;

                    ///////////////////////////////////////////////////////////////////////////////
                    // 3 Check orientation of wood::joint outlines, if needed flip
                    ///////////////////////////////////////////////////////////////////////////////
                    bool flip = CGAL::has_smaller_distance_to_point(
                        polyline0[i],
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !is_geo_reversed)[0].front(),
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !is_geo_reversed)[0].back());
                    ;
                    if (!flip)
                    {
                        // bottom
                        std::reverse(
                            joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !is_geo_reversed)[0].begin(),
                            joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !is_geo_reversed)[0].end());

                        // top
                        std::reverse(
                            joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), is_geo_reversed)[0].begin(),
                            joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), is_geo_reversed)[0].end());
                    }
                } // if wood::joint edge
            }     // for j
        }         // for i

        // Update the end
        polyline0[polyline0.size() - 1] = polyline0[0];
        polyline1[polyline1.size() - 1] = polyline1[0];

        ///////////////////////////////////////////////////////////////////////////////
        // After intersection is ready merger all polylines
        ///////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline newPolyline0;
        CGAL_Polyline newPolyline1;

        for (int i = 0; i < n; i++)
        { // iterate sides only
            newPolyline0.push_back(polyline0[i]);
            newPolyline1.push_back(polyline1[i]);

            for (int j = 0; j < this->j_mf[i + 2].size(); j++)
            { //
                if (j_mf[i + 2].size() > 0)
                {
                    if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                        continue;
                    if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() == 2)
                    {
                        for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true)[0])
                            newPolyline0.push_back(pp);
                        for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false)[0])
                            newPolyline1.push_back(pp);
                    }
                }
            }

            if (i == n - 1)
            {
                newPolyline0.push_back(newPolyline0[0]);
                newPolyline1.push_back(newPolyline1[0]);
                output[this->id].push_back(newPolyline0);
                output[this->id].push_back(newPolyline1);
            }
        }

        for (int i = 0; i < 2; i++)
        { // iterate top only
            for (size_t j = 0; j < j_mf[i].size(); j++)
            {
                if (joints[std::get<0>(j_mf[i][j])].name == "")
                    continue;

                // Check hole position
                bool is_geo_reversed =
                    CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]) >
                    CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]);

                if (is_geo_reversed)
                    joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));

                for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() - 1; k++)
                {
                    output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);
                    output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);
                }
            }
        }

        // close

        ///////////////////////////////////////////////////////////////////////////////
        // output
        ///////////////////////////////////////////////////////////////////////////////

        return;
    }

    // inline bool element::intersection_closed_and_open_paths_2D(CGAL_Polyline& closed_pline_cutter, std::pair<int, int>& edge_pair, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c, CGAL_Polyline& output, std::pair<double, double>& cp_pair) {
    bool element::intersection_closed_and_open_paths_2D(CGAL_Polyline &closed_pline_cutter, CGAL_Polyline &pline_to_cut, IK::Plane_3 &plane, CGAL_Polyline &c, int (&edge_pair)[2], std::pair<double, double> &cp_pair)
    {

        // ATTENTION THIS IS A SPECIFIC CLIPPER FUNCTION TO CLIP USING OPEN PATHS TO OBTAIN CLEAN END POINTS

        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline plate_outline = closed_pline_cutter;
        CGAL_Polyline joint_outline = pline_to_cut;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation, to orient the the 3D polylines to the XY plane
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::plane_to_xy(plate_outline[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::transform(plate_outline, xform_toXY);
        cgal_polyline_util::transform(joint_outline, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Find Max Coordinate to get Scale factor
        /////////////////////////////////////////////////////////////////////////////////////
        IK::Vector_3 translation_vector;
        size_t n = joint_outline.size() - 1 + plate_outline.size() - 1;

        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        Clipper2Lib::PathsD clipper_plate_outlines;
        clipper_plate_outlines.emplace_back(std::vector<Clipper2Lib::PointD>());
        clipper_plate_outlines.back().reserve(plate_outline.size() - 1);

        Clipper2Lib::PathsD clipper_joint_outlines;
        clipper_joint_outlines.emplace_back(std::vector<Clipper2Lib::PointD>());
        clipper_joint_outlines.back().reserve(joint_outline.size());

        // be aware that the scaling miust be done using int64_t, if int will be used the result will be wrong due to rounding
        for (int i = 0; i < plate_outline.size() - 1; i++) // this outline has no duplicate end point because it is "closed path"
        {
            clipper_plate_outlines.back().emplace_back((plate_outline[i].x()), (plate_outline[i].y()));
        }

        for (int i = 0; i < joint_outline.size(); i++) // this outline has duplicate end points because it is "open path"
        {
            clipper_joint_outlines.back().emplace_back((joint_outline[i].x()), (joint_outline[i].y()));
        }

        try
        {

            // https://github.com/AngusJohnson/Clipper2/blob/main/CPP/Clipper2Lib/include/clipper2/clipper.h
            Clipper2Lib::PathsD clipper_result_closed;
            Clipper2Lib::PathsD clipper_result_open;
            Clipper2Lib::ClipperD clipper;
            clipper.AddOpenSubject(clipper_joint_outlines);
            clipper.AddClip(clipper_plate_outlines);
            clipper.Execute(Clipper2Lib::ClipType::Intersection, Clipper2Lib::FillRule::NonZero, clipper_result_closed, clipper_result_open);

            if (clipper_result_open.size() > 0)
            {

                // Calculate number of points
                c.reserve(clipper_result_open[0].size());
                // CGAL_Debug(count);

                int count = 0;
                for (auto &polynode : clipper_result_open)

                {
                    // do stuff with polynode here
                    if (polynode.size() <= 1)
                        continue;

                    // scale back to original coordinates
                    if (count == 0)
                    {

                        // std::cout << "\n";
                        for (size_t j = 0; j < polynode.size(); j++)
                        {
                            c.emplace_back(polynode[j].x, polynode[j].y, 0);
                            // std::cout << polynode[j].x / scale << " " << polynode[j].y / scale << " " << 0 << std::endl;
                        }
                        // std::cout << "\n";
                    }
                    else
                    { // if there are multiple segments
                        std::vector<IK::Point_3> pts;
                        pts.reserve(polynode.size());
                        for (size_t j = 0; j < polynode.size(); j++)
                            pts.emplace_back(polynode[j].x, polynode[j].y, 0);
                        // pts.emplace_back(polynode[j].x / scale, polynode[j].y / scale, 0);

                        // Check if curve is closest to new pline if not reverse
                        if (CGAL::squared_distance(c.back(), pts.front()) > wood_globals::DISTANCE_SQUARED && CGAL::squared_distance(c.back(), pts.back()) > wood_globals::DISTANCE_SQUARED)
                            std::reverse(c.begin(), c.end());

                        // Check if insert able curve end is closest to the main curve end, if not reverse
                        if (CGAL::squared_distance(c.back(), pts.front()) > CGAL::squared_distance(c.back(), pts.back()))
                            std::reverse(pts.begin(), pts.end());

                        for (size_t j = 1; j < pts.size(); j++)
                            c.emplace_back(pts[j]);
                    }

                    count++;
                }

                ///////////////////////////////////////////////////////////////////////////////
                // Get closest parameters (Find closest parameters to edges) and add to pairs
                ///////////////////////////////////////////////////////////////////////////////
                double t0_, t1_;
                bool assigned_t0_ = false;
                bool assigned_t1_ = false;

                // orient the intersection result to the "b" polyline that represents the plate
                // iterate through the plate outline
                // itarate through segments,
                // the two points of the resulting polyline segment that touches the plate segment is the point for orientation
                // the shift the vector so that the first and the end points are actually touching
                //
                // ░░░░s0░░░░░░░░░░░░░░░░░░░░░░
                // ░░░┌───█═══█───┐░░░░░░░░░░░░
                // ░░░│░p0║░░░░p1░│░░s3░░░░░░░░
                // s1░│░░░█═══█░░░│░░░░░░░░░░░░
                // ░░░│░p1░░░░░p2░│░░b░░░░░░░░░
                // ░░░│░░░░░░░░░░░│░░░░░░░░░░░░
                // ░░░└───────────┘░░░░░░░░░░░░
                // ░░░░░░░░░c░░░s2░░░░░░░░░░░░░

                // find the closest point to the plate segment, output point indices and paramters on the plate edge
                // bool nearest_id_0 = -1;
                // bool nearest_id_1 = -1;
                double t0 = -1;
                double t1 = -1;

                // std::cout << "\n";
                // this has to be rewritten to get to smallest ids to an edge or below the error message to be scale prone
                for (size_t i = 0; i < plate_outline.size() - 1; i++)
                {
                    IK::Segment_3 s(plate_outline[i], plate_outline[(i + 1)]);

                    for (size_t j = 0; j < 2; j++)
                    {
                        size_t id = j == 0 ? 0 : c.size() - 1; // take only the first and last points
                        double dist_to_plate_segment = CGAL::squared_distance(s, c[id]);

                        // std::cout << "dist_to_plate_segment: " << dist_to_plate_segment << std::endl;

                        if (j == 0 && dist_to_plate_segment < 1) // wood_globals::DISTANCE_SQUARED * 1000)
                        {

                            cgal_polyline_util::closest_point_to(c[0], s, t0);

                            // std::cout << "t0: " << t0 << std::endl;
                            t0 += i;
                        }
                        else if (j == 1 && dist_to_plate_segment < 1) //< wood_globals::DISTANCE_SQUARED * 1000)
                        {

                            cgal_polyline_util::closest_point_to(c[c.size() - 1], s, t1);

                            // std::cout << "t1: " << t1 << std::endl;
                            t1 += i;
                        }
                    }

                    if (t0 != -1 && t1 != -1)
                        break;
                }

                // for (int i = 0; i < plate_outline.size(); i++)
                //     plate_outline[i] = plate_outline[i].transform(xform_toXY_Inv);

                // reverse if the clipper result is flipped
                // sort intersection polyling so that the two points are in the correct order
                bool reverse_flag = (t0 > t1);
                reverse_flag = std::floor(t0) == 0 && std::floor(t1) == c.size() - 1 ? !reverse_flag : reverse_flag;
                if (reverse_flag)
                {
                    std::swap(t0, t1);
                    std::reverse(c.begin(), c.end());
                }
                else
                {
                }


                if (t0 != -1 && t1 != -1)
                {
                    // transform to 3D space and give parameters for insertion
                    cgal_polyline_util::transform(c, xform_toXY_Inv);
                    cp_pair = std::pair<double, double>(t0, t1);
                    return true;
                }
                else
                {

                    return false;
                }
            }
            else
            {
                // std::cout << "intersection_closed_and_open_paths_2D -> No intersection found\n";
                return false;
            }
        }
        catch (const std::exception &ex)
        {
            // std::cout << "wood_element.cpp -> parameters: scale " << scale << "max_coordinate " << max_coordinate << "\n";
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s   ", __FILE__, __FUNCTION__, __LINE__, ex.what());
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }

    void element::merge_joints(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output)
    {

        // printf( "merge function \n" );

        // OPTIMIZE CASE(5) BECAUSE EDGE ARE KNOWN, BUT CHECK ALSO CROSS JOINT ENSURE THAT YOU TAKE CROSSING EDGES
        // CHANGE TO 2D METHOD, TO AVOID MULTIPLE THE SAME MATRIX CREATION FOR ORIENTATION TO 2D I.E. CLIPPER AND line_line_3d
        // you are in a loop
        // only for objects that has one element as a wood::joint and edges to insert are known
        // List order: top0 bottom0 top1 bottom1 ... topN bottomN

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy the top and bottom polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0 = this->polylines[0];
        CGAL_Polyline pline1 = this->polylines[1];
        std::vector<IK::Plane_3> joint_planes = planes;

        // scale is used to convert the t0 and t1 parameter of the curve to size_t key value,
        // since double are not allowed as key in map
        // "scale_0" is used for segment id and "scale_1" is parameter on the line
        double scale_0 = 1000000;
        double scale_1 = 1000;
        std::map<size_t, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_by_id_plines_0;
        std::map<size_t, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_by_id_plines_1;

        std::vector<bool> flags0(pline0.size() - 1);
        std::vector<bool> flags1(pline1.size() - 1);
        auto point_count = pline0.size();

        IK::Segment_3 last_segment0_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
        IK::Segment_3 last_segment1_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
        IK::Segment_3 last_segment0;
        IK::Segment_3 last_segment1;
        int last_id = -1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // STEP 1
        // Iterate edge joints ( skip the top and bottom autlines that cannot be merged)
        // "i" refers to the element edge
        // "j" refers to number of joints on one edge, currently "j" is always equals to 0
        //
        // STEP 2
        // Perform checks: 1. If joint name is undefined 2. If the joint outlines are empty 3. If the joint top and bottom outlines matches the element top and bottom outlines
        //
        // STEP 3
        // Then there are two posisbilities: Joint 2D volume is a line segment or a rectangle
        //
        // STEP 3.1 - LINE
        // Get joint line
        // Get update the joint_planes (copy of planes) with the plane positioned at the joint line
        // Get new points of the edge, by interesecting: a) next-current-top, b) prev-current-top, c) next-current-bottom, d) prev-current-bottom
        //
        // STEP 3.2 - RECTANGLE
        //
        // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
        // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
        // ░░░░░░░░░░░░░░░░░░░░░┌─────────────────────────┬───────────────────────────┬┐░░░░
        // ░░░░░░░░░░░░░░░░░░┌──┘░░░░░░░░░░░░░░░░░░░░░░┌──┘░░│░░░░░░░░░░░░░░░░░░░░░░┌─┘│░░░░
        // ░░░░░░░░░░░░░░░░┌─┘░░░░░░░░░░░░░░░░░░░░░░░┌─┘░░░░░│░░░░░░░░░░░░░░░░░░░┌──┘░░│░░░░
        // ░░░░░░░░░░░░░┌──┘░░░░░░░i=0░░░░░░░░░░░░┌──┘░░i=5░░│░░░░░░i=4░░░░░░░░┌─┘░░░░░│░░░░
        // ░░░░░░░░░░┌──┘░░░░░░(skipped)░░░░░░░┌──┘iter edges│░j_mf.size()=0░┌─┘░░░░░░░│░░░░
        // ░░░░░░░░┌─┘░░░░░░░░░░░░░░░░░░░░░░┌──┘j_mf.size()=6│░░░░░░░░░░░░░┌─┘░░░░░░░░░│░░░░
        // ░░░░░┌──┘░░░░░░░░░░░░░░░░░░░░░┌──┘joints_per_edge:│░░░░░░░░░░┌──┘░░░░i=3░░░░│░░░░
        // ░░░░─┴────────────────────────┴──────────────────┬───────────┤j_mf.size()=0┌┘░░░░
        // ░░░░│░░░░░░░░░░░░░░░░░░░░░░░░│j_mf[i].size()=1┌──┘░░░░░░░░░░░│░░░░░░░░░░░┌─┘░░░░░
        // ░░░░│░░░░░░░░░░░░░░░░░░░░░░░░│░░░░░░░░░░░░░┌──┘░░░░░░░░░░░░░░│░░░░░░░░┌──┘░░░░░░░
        // ░░░░│░░░░░░░░░░░░░░░░░░░░░░░░│░░░░░░░░░░┌──┘░░░░░░░░░░░░░░░░░│░░░░░░┌─┘░░░░░░░░░░
        // ░░░░│░░░░░░░░░░i=2░░░░░░░░░░░│░░░░░░░┌──┘░░░░░░░i=2░░░░░░░░░░│░░░░┌─┘░░░░░░░░░░░░
        // ░░░░│░░░░░j_mf.size()=0░░░░░░│░░░░┌──┘░░░░░░j_mf.size()=0░░░░│░░┌─┘░░░░░░░░░░░░░░
        // ░░░░│░░░░░░░░░░░░░░░░░░░░░░░░│░░┌─┘░░░░░░░░░░░░░░░░░░░░░░░░░░├──┘░░░░░░░░░░░░░░░░
        // ░░░░────────────────────────────┴────────────────────────────┘░░░░░░░░░░░░░░░░░░░
        // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
        //
        //
        // j_mf contains 3 properties:
        // 1. int - joint id
        // 2. bool - male/female
        // 3. std::pair<double, double> - parameters on edge
        //
        // https://fsymbols.com/draw/
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // std::cout << "New Polygon, Planes Count: " << this->planes.size() << std::endl;
        for (size_t i = 2; i < this->j_mf.size(); i++)
        {
            for (size_t j = 0; j < this->j_mf[i].size(); j++)
            {

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Get properties for the better readability, because tuple indexing is not so readable
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                int joint_id = std::get<0>(j_mf[i][j]);
                bool male_or_female = std::get<1>(j_mf[i][j]);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Sanity checks:
                // Check undefined names, if yes continue
                // Check empty outlines, if yes continue
                // Check the if top and bottom joint oulines are different from the plane order, if yes reverse the order
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                if (joints[joint_id].name == "")
                    continue;

                if (joints[joint_id](male_or_female, false).size() == 0)
                    continue;

                bool is_geo_reversed =
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, true)[1][0]), joints[joint_id](male_or_female, true)[1][0]) >
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, false)[1][0]), joints[joint_id](male_or_female, false)[1][0]);

                if (is_geo_reversed)
                    joints[joint_id].reverse(male_or_female);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Intersect line (2 points) or rectangle (5 points)
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                switch (joints[joint_id](male_or_female, true)[1].size())
                {
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Single line joint geometry
                    //
                    // // The side-to-side connection is always applied one specific edge, so we are talking about always about 2 points
                    // ╬ - removed points
                    // █ - added or replaced intersection points
                    // ═ - joint line
                    // ─ - polygon edge
                    //
                    // ░1░░░░░░░░░░0░░░░░1░░░░░░░░░░0░░░░░1░░░░░░░░░░0░░░░░1░░░░░░░░░░0░░░░░1░░░░░░░░░0░░░░░
                    // ░░░░░░░░░░░░░░░░░░█══════════█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░█══════════█░░░░░X──────────X░░░░░┌──█════█──┐░░░░░X──────────X░░░══█═════════█══░░░
                    // ░│t0░░░░░░t1│░░░░░│t0░░░░░░t1│░░░░░│░t0░░░░░t1│░░░░░│░░░░░░░░░░│░░░░░│t0░░░░░t1│░░░░░
                    // ░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░█░════════░█░░░░░│░░░░░░░░░│░░░░░
                    // ░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│t0░░░░░░t1│░░░░░│░░░░░░░░░│░░░░░
                    // ░└──────────┘░░░░░└──────────┘░░░░░└──────────┘░░░░░└──────────┘░░░░░└─────────┘░░░░░
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░3░░░░░
                    //
                    // option 0 - input - line is the same as corner points, output - the two points are kept as is
                    // option 1 - input - line is above the edge, output - the two points are replaced by the intersection point
                    // option 2 - input - line is inside the edge, output, output - the two points are kept as is, polygon is inserted between the two points
                    // option 3 - input - line is below the edge, output - the two points are replaced by the intersection point
                    // option 4 - input - line is outside the edge, output - the two points are kept as is
                    //
                    // in all cases plane-to-plane intersection is used to relocated the points
                    // in the case of option 2, the polygon is inserted between the two points, no points are deleted
                    //
                    // // Goal is to insert a joint polyline between the two points:
                    //
                    // ░░░░░░░░░░░░┌────┐░░░░┌────┐░░░░░░░░░░░
                    // ░░░░░░░░░░░░│░░░░│░░░░│░░░░│░░░░░░░░░░░
                    // ░░░░░░░░░░░░│░░░░│░░░░│░░░░│░░░░░░░░░░░
                    // ░█─────┐████│████│████│████│████┌────█░
                    // ░t0░░░░│░░░░│░░░░│░░░░│░░░░│░░░░│░░░t1░
                    // ░░░░░░░│░░░░│░░░░│░░░░│░░░░│░░░░│░░░░░░
                    // ░░░░░░░└────┘░░░░└────┘░░░░└────┘░░░░░░
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                case (2):
                {

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Get top and bottom line of the joint
                    //
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░█████████████████████████████████████░
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    IK::Segment_3 joint_line_0(joints[joint_id](male_or_female, true)[1][0], joints[joint_id](male_or_female, true)[1][1]);
                    IK::Segment_3 joint_line_1(joints[joint_id](male_or_female, false)[1][0], joints[joint_id](male_or_female, false)[1][1]);

                    ///////////////////////////////////////////////////////////////////////////////
                    // Update the joint line plane, the plane orientation is may not be the same, so reconstruct it from the joint-lines
                    //
                    // ░░┌───┐░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░█│ * │██████████████████████████████░
                    // ░░└───┘░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    //
                    // Old version:
                    // IK::Plane_3 joint_line_plane_0(joint_line_0[0], planes[i].orthogonal_vector());
                    // IK::Plane_3 joint_line_plane_1(joint_line_1[0], planes[i].orthogonal_vector());
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    IK::Vector_3 x_axis = joint_line_0[1] - joint_line_0[0];
                    IK::Vector_3 y_axis = joint_line_0[0] - joint_line_1[0];
                    IK::Vector_3 z_axis = CGAL::cross_product(x_axis, y_axis);
                    joint_planes[i] = IK::Plane_3(joint_line_0[0], z_axis);

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Properties of the polyline
                    // n - number of points in the outline
                    // id - index of the current polyline point
                    //
                    // ░░░░░░░░░░prev░░░░░░░░░e_prev░░░░░id░░░
                    // ░░░░░░░░░░░░░░██████████████████╔═░░░░░
                    // ░░░░░░░░░░░┌──┘░░░░░░░░░░░░░░╔══╝░░░░░░
                    // ░░░░░░░░░┌─┘░░░░░░░░░░░░░░╔══╝░░░░░░░░░
                    // ░░░░░░┌──┘░░░░░░░░░░░░░░╔═╝░░░░e_id░░░░
                    // ░░░░░█████████████████══╝░░░░░░░░░░░░░░
                    // nextnext░░░e_next░░░next░░░░░░░░░░░░░░░
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    auto n = pline0.size() - 1;

                    int id = i - 2;

                    auto prev = (n + id - 1) % n; // ( ((id - 1) % n) + n) % n;
                    auto next = (id + 1) % n;
                    auto nextnext = (id + 2) % n;

                    // std::cout << " n " << n << " id " << id << " prev " << prev << " next " << next << std::endl;

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Intersect:
                    // Top or bottom plane
                    // Line plane
                    // Previous or next polyline segment plane
                    //
                    // WARNING: When two side planes are paralel the interseciton is not valid
                    // TODO: this method can be replaced by line-line intersection, currently it is done by internsecting 3 planes, which is also working
                    //
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░░░░░░░░░░░░████░░0 and 1░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░░░░░░░░░░░░████░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░──────┐░░░░░░░░p0_int░░░░░░░░p1_int░░░┌────────░░
                    // ░░░░░████└──────────█──────────────█─────┘░░░████░░░
                    // ░░░░░████░░░░2+prev░│░░░░████░2+i░░│░░2+next░████░░░
                    // ░░░────────┐░░░░░░░░│░░░░████░░░░░░│░░░░┌─────────░░
                    // ░░░░░░░░░░░└────────█──────────────█────┘░░░░░░░░░░░
                    // ░░░░░░░░░░░░░░░░░░p0_int░░░░░░░░p1_int░░░░░░░░░░░░░░

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    IK::Point_3 p0_int, p1_int, p2_int, p3_int;
                    // std::cout << "2 + prev " << (2 + prev) << " " << (prev) << " i " << i << std::endl;
                    // std::cout << "2 + next " << (2 + next) << " i " << i << std::endl;
                    bool is_intersected_0 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[2 + prev], joint_planes[i], joint_planes[0], p0_int);
                    bool is_intersected_1 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[2 + next], joint_planes[i], joint_planes[0], p1_int);

                    bool is_intersected_2 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[2 + prev], joint_planes[i], joint_planes[1], p2_int);
                    bool is_intersected_3 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[2 + next], joint_planes[i], joint_planes[1], p3_int);

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Relocate end-point, on the 2d iteration
                    // The check asks if the previously looped edge had joint, because it can only interesect with neigbor edge (they must not be parallel too)
                    // 0 - 0 1 -
                    // 0 - 0 1 1 -
                    // 0 1 1 1 1 1 -
                    // - 0 1 1 -
                    // 0 1 1 1 1 -
                    // where "0" means that the previous edge was did not contain a joint, "1" - vice-versa, "-" - no joint is in the joint joint lsit
                    // the lines are always in the same plane
                    // WARNING: Replace line_line_3d with a plane method because it is using boost
                    //
                    // ░░░░░░░░░░░░░░░░░░joint_line_prev)░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░░░░░░████████████████████░░░█░intersection point ░░
                    // ░░░░░░░░░░░   ░░░░░░░░░░░░░░░   ░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░   ░░░░░░░░░░░░░░   ░░███░░░░joint_line░░░░░░░░░░░░
                    // ░░░░░░░   ░░░░░░░░░░░░░░   ░░███░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░                    ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    //
                    // perform this intersection only if the two lines are not one the edge
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    // get distance between a joint-line and element edge
                    IK::Line_3 temp_segment_0(this->polylines[0][i - 2], this->polylines[0][i - 1]);
                    IK::Line_3 temp_segment_1(this->polylines[1][i - 2], this->polylines[1][i - 1]);
                    bool geometry_distance_to_edge_0 = CGAL::squared_distance(joint_line_0[0], temp_segment_0.projection(joint_line_0[0])) > wood_globals::DISTANCE_SQUARED;
                    bool geometry_distance_to_edge_1 = CGAL::squared_distance(joint_line_1[0], temp_segment_1.projection(joint_line_1[0])) > wood_globals::DISTANCE_SQUARED;

                    if (last_id == i - 1 && (geometry_distance_to_edge_0 || geometry_distance_to_edge_1))
                    {

                        IK::Point_3 p0;
                        IK::Point_3 p1;

                        bool is_joint_line_interesected_0 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[i], joint_planes[i - 1], joint_planes[0], p0);
                        bool is_joint_line_interesected_1 = cgal_intersection_util::plane_plane_plane_with_parallel_check(joint_planes[i], joint_planes[i - 1], joint_planes[1], p1);

                        if (is_joint_line_interesected_0 && is_joint_line_interesected_1)
                        {
                            p0_int = p0;
                            p2_int = p1;
                        }
                    }

                    ///////////////////////////////////////////////////////////////////////////////
                    // Relocate side points to intersection points
                    // If intersecting relocate wood::joint line points --|*---------*|--,
                    // Else overlaping do not change |  *-----*  |.
                    ///////////////////////////////////////////////////////////////////////////////

                    if (is_intersected_0)
                    {
                        pline0[id] = p0_int;
                        // std::cout << "p0_int " << p0_int << std::endl;
                    }

                    if (is_intersected_1)
                    {
                        pline0[next] = p1_int;
                        // std::cout << "p1_int " << p1_int << std::endl;
                    }

                    if (is_intersected_2)
                    {
                        pline1[id] = p2_int;
                        // std::cout << "p2_int " << p2_int << std::endl;
                    }

                    if (is_intersected_3)
                    {
                        pline1[next] = p3_int;
                        // std::cout << "p3_int " << p3_int << std::endl;
                    }

                    last_segment0 = joint_line_0;
                    last_segment1 = joint_line_1;

                    if (i == 2)
                    {
                        last_segment0_start = joint_line_0;
                        last_segment1_start = joint_line_1;
                    }

                    last_id = i;

                    ///////////////////////////////////////////////////////////////////////////////
                    // Check orientation of wood::joint outlines, if needed flip
                    // The check is made by measuring the distance between edge outline first and last points
                    // And the edge end point
                    //
                    // ░░░░░░░░░░░░░3░░░░░░░░░░░░░░░░4░░░░0, ignore ░
                    // ░░░░░░░░░░░░░███░░░░░░░░░░░░░███│││░░due to░░░
                    // ░░░░░░░░░░░░░░░░░░░█░█░░░░░█░░░░░░░arbitrary░░
                    // ░░░░░░░░░░░░░█░░░░░░░░░░░█░░░░░░░░░░first ░░░░
                    // ░░░░░░░░░░░█░░░░█░█░░░░░░░░░░░░░░░░░point░░░░░
                    // ░░░░░███░░░░░░░░░░░░░░███░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░2░░░░░░░░░░░░░░░░1░░░░░░edge 0 to 1st pt░░
                    ///////////////////////////////////////////////////////////////////////////////
                    // std::cout << "ERROR wood_element Line 879 " << std::endl;

                    bool is_geo_flipped = CGAL::has_smaller_distance_to_point(
                        pline0[id + 1],
                        joints[joint_id](male_or_female, !is_geo_reversed)[0].front(),
                        joints[joint_id](male_or_female, !is_geo_reversed)[0].back());

                    if (is_geo_flipped)
                    {
                        std::reverse(joints[joint_id](male_or_female, !is_geo_reversed)[0].begin(), joints[joint_id](male_or_female, !is_geo_reversed)[0].end());
                        std::reverse(joints[joint_id](male_or_female, is_geo_reversed)[0].begin(), joints[joint_id](male_or_female, is_geo_reversed)[0].end());
                    }

                    ///////////////////////////////////////////////////////////////////////////////
                    // Get closest parameters (edge start, start+1) and add to pairs
                    ///////////////////////////////////////////////////////////////////////////////
                    std::pair<double, double> cp_pair((double)(id + 0.1), (double)(id + 0.9)); // these pairs are not functional
                    double average = id + 0.5;

                    sorted_by_id_plines_0.insert(
                        std::make_pair(
                            (size_t)(scale_0 * std::floor(cp_pair.first)) + (size_t)(scale_1 * std::fmod(cp_pair.first, 1)), // id + 0.1
                            std::pair<std::pair<double, double>, CGAL_Polyline>{
                                cp_pair, // id + 0.1, id + 0.9
                                joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0]}));

                    sorted_by_id_plines_1.insert(
                        std::make_pair(
                            (size_t)(scale_0 * std::floor(cp_pair.first)) + (size_t)(scale_1 * std::fmod(cp_pair.first, 1)), // id + 0.1
                            std::pair<std::pair<double, double>, CGAL_Polyline>{
                                cp_pair, // id + 0.1, id + 0.9
                                joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), false)[0]}));

                    point_count += joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0].size();
                    break;
                }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Rectangle line joint geometry
                    // X - removed points
                    // █ - added or replaced intersection points
                    // ═ - joint polygon rectangle
                    // ─ - polygon edge
                    //
                    // ░░░░░░░░░╔═╗░░░░░░░░0░░░░░║1░░░░░░░░0║░░░║░░1░░░░░░░░0░░║░░░░1░║░░░░║░0░░░░░░║1░░░░░░░░0║░░░║░░1░░░░░░░░0░║░░░
                    // ░░░░░░╔══╝░╚═╗░░░░░░░░░░░░║░░░░░░░░░░║░░░║░░░░░░░░░░░░░░║░░░░░░║░░░░║░░░░░░░░║░░░░░░░░░░║░░░║░░░░░░░░░░░░░║░░░
                    // ░░░╔══╝1░X───╚═█────┐░░░░░█══════════█░░░║░X──────────X░║░░░┌──█════█──┐░░░░░X──────────X░░░╚═█═════════█═╝░░░
                    // ░░░╚╗░░░░│░░╔══╝t1░░│░░░░░│t0░░░░░░t1│░░░╚═█══════════█═╝░░░│t0║░░░░║t1│░░░░░║░░░░░░░░░░║░░░░░│t0░░░░░t1│░░░░░
                    // ░░░░╚═╗░░█══╝░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░╚════╝░░│░░░░░█══════════█░░░░░│░░░░░░░░░│░░░░░
                    // ░░░░░░╚══╝t0░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│░░░░░░░░░░│░░░░░│t0░░░░░░t1│░░░░░│░░░░░░░░░│░░░░░
                    // ░░░░░░░░░└──────────┘░░░░░└──────────┘░░░░░└──────────┘░░░░░└──────────┘░░░░░└──────────┘░░░░░└─────────┘░░░░░
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░░░░░░░░░░2░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░░3░░░░░2░░░░░░░░░3░░░░░
                    //
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                case (5):
                {

                    // std::cout << "Intersect rectangle or line CASE 5 \n";

                    // two edges
                    int edge_pair[2];
                    joints[joint_id].get_edge_ids(male_or_female, edge_pair[0], edge_pair[1]);
                    if (edge_pair[0] > edge_pair[1])
                        std::swap(edge_pair[0], edge_pair[1]);

                    if (false)
                    { // split by line, in this case you need to know which side is inside
                    }
                    else
                    { // split by full polygon
                        ///////////////////////////////////////////////////////////////////////////////
                        // 1) Cut polygons and 2) Get closest parameters (Find closest parameters to edges) and add to pairs
                        ///////////////////////////////////////////////////////////////////////////////

                        // First Intersection
                        std::pair<double, double> cp_pair_0(0.0, 0.0);
                        CGAL_Polyline joint_pline_0;
                        bool result0 = intersection_closed_and_open_paths_2D(
                            pline0, joints[joint_id](male_or_female, true).front(), this->planes[0],
                            joint_pline_0, edge_pair, cp_pair_0);
                        if (!result0)
                            continue;

                        // Second Intersection
                        std::pair<double, double> cp_pair_1(0.0, 0.0);
                        CGAL_Polyline joint_pline_1;
                        bool result1 = intersection_closed_and_open_paths_2D(
                            pline1, joints[joint_id](male_or_female, false).front(), this->planes[1],
                            joint_pline_1, edge_pair, cp_pair_1);
                        if (!result1)
                            continue;

                        // Output
                        // std::cout << "__________cp_pair_0: " << cp_pair_0.first << " " << cp_pair_0.second << std::endl;
                        // std::cout << "__________cp_pair_1: " << cp_pair_1.first << " " << cp_pair_1.second << std::endl;

                        sorted_by_id_plines_0.insert(
                            std::make_pair(
                                (size_t)(scale_0 * std::floor(cp_pair_0.first)) + (size_t)(scale_1 * std::fmod(cp_pair_0.first, 1)), //(cp_pair_0.first + cp_pair_0.second) * 0.5,
                                std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_0, joint_pline_0}));

                        sorted_by_id_plines_1.insert(
                            std::make_pair(
                                (size_t)(scale_0 * std::floor(cp_pair_1.first)) + (size_t)(scale_1 * std::fmod(cp_pair_1.first, 1)), //(cp_pair_1.first + cp_pair_1.second) * 0.5,
                                std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_1, joint_pline_1}));

                        point_count += joint_pline_1.size();
                    }

                    break;
                }
                default:
                    continue;
                    break;
                }
            }

            // Unify windings of polylines
        }

        // printf("Merge all polygons in one closed polyline \n");

        // auto j_mf_count_ = this->j_mf.size();
        // printf("The value of a : %zu", this->j_mf.size());
        // // test segmentation fault 1
        // return;

        ///////////////////////////////////////////////////////////////////////////////
        // Iterate pairs and mark skipped points ids
        // first is key, second - value (pair of cpt (first) and polyline (second))
        ///////////////////////////////////////////////////////////////////////////////

        // printf("First polyline \n");

        //////////////////////////////////////// first polyline ////////////////////////////////////////

        // Point flags, if the flag is set to true, the original points is used, otherwise the point is skipped
        std::vector<bool> point_flags_0(pline0.size(), true);

        // iterate sides faces of the element
        for (auto &pair : sorted_by_id_plines_0)
            // iterate // id + 0.1, id + 0.9, this makes no sence
            for (size_t j = (size_t)std::ceil(pair.second.first.first); j <= (size_t)std::floor(pair.second.first.second); j++)
            { // are corners in between insertable polylines
                // std::cout << "remove points \n";
                point_flags_0[j] = false;
                point_count--;
            }

        point_flags_0.back() = false; // ignore last

        // corner case
        // when a cross joint is position on the last vertex of the polyline
        // Skip the first point, when the joint is located on the last segment of the polyline
        // pline0.size() - 2, because the polyline is closed
        // [ . first ] [ . last ]

        // std::cout << "__________dict: " << (sorted_by_id_plines_0.begin()->second.first.first) << " " << (sorted_by_id_plines_0.begin()->second.first.second) << "\n";
        // std::cout << "__________dict: " << (sorted_by_id_plines_0.rbegin()->second.first.first) << " " << (sorted_by_id_plines_0.rbegin()->second.first.second) << "\n";
        if (sorted_by_id_plines_0.size())
        {
            if (
                sorted_by_id_plines_0.rbegin()->second.first.first > (pline0.size() - 2) &&
                sorted_by_id_plines_0.rbegin()->second.first.second < 1)
            {
                // std::cout << "__________corner case detected\n";
                //  No need to reverse, it is handled inside intersection_closed_and_open_paths_2D method -> // std::reverse(sorted_by_id_plines_0.begin()->second.second.begin(), sorted_by_id_plines_0.begin()->second.second.end());
                point_flags_0[0] = false; // skip the first points
                point_count--;
            }
        }

        // Add single point polygons to the sorted map
        // [ . ] [ . ] [ . ]
        for (size_t i = 0; i < point_flags_0.size(); i++)
            if (point_flags_0[i])
                sorted_by_id_plines_0.insert(
                    std::make_pair((size_t)(i * scale_0),
                                   std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>((double)i, (double)i), CGAL_Polyline{pline0[i]}}));

        // Merge all polygons in one closed polyline
        // [ . ] [ . : : . ] [ . ] [ . : : . ] [ . ]
        CGAL_Polyline pline0_new;
        pline0_new.reserve(point_count);
        for (auto const &x : sorted_by_id_plines_0)
            pline0_new.insert(pline0_new.end(), x.second.second.begin(), x.second.second.end());

        // // test segmentation fault 2
        // return;

        //////////////////////////////////////// second polyline ////////////////////////////////////////

        // printf("Second polyline \n");
        // printf("point flags to keep corners \n");

        std::vector<bool> point_flags_1(pline0.size(), true); // point flags to keep corners
        // printf("0 \n");

        for (auto &pair : sorted_by_id_plines_1)
        {
            // printf("1 \n");
            for (size_t j = (size_t)std::ceil(pair.second.first.first); j <= (size_t)std::floor(pair.second.first.second); j++)
            { // are corners in between insertable polylines§§§
                // printf("2 \n");
                point_flags_1[j] = false;
            }
        }

        point_flags_1.back() = false; // ignore last

        // printf("Corner Case \n");

        if (sorted_by_id_plines_1.size() > 0)
        {
            if (
                sorted_by_id_plines_1.rbegin()->second.first.first > pline1.size() - 2 &&
                sorted_by_id_plines_1.rbegin()->second.first.second < 1)
            {
                // printf("0 \n");
                if (point_flags_1.size() > 0)
                {
                    // No need to reverse, it is handled inside intersection_closed_and_open_paths_2D method -> // std::reverse(sorted_by_id_plines_1.begin()->second.second.begin(), sorted_by_id_plines_1.begin()->second.second.end());
                    point_flags_1[0] = false;
                    // printf("1 \n");
                }
            }
        }

        // printf("Add single point polygons \n");
        for (size_t i = 0; i < point_flags_1.size(); i++)
            if (point_flags_1[i])
                sorted_by_id_plines_1.insert(std::make_pair((size_t)scale_0 * i, std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>((double)i, (double)i), CGAL_Polyline{pline1[i]}}));

        // printf("Merge all polygons in one closed polyline \n");
        CGAL_Polyline pline1_new; // reserve optimize
        pline1_new.reserve(point_count);

        for (auto const &x : sorted_by_id_plines_1)
            pline1_new.insert(pline1_new.end(), x.second.second.begin(), x.second.second.end());

        // printf("Close \n");

        ///////////////////////////////////////////////////////////////////////////////
        // WARNING Close - Does this part make sense?
        ///////////////////////////////////////////////////////////////////////////////
        if (last_id == this->polylines[0].size() && last_segment0_start.squared_length() > wood_globals::DISTANCE_SQUARED)
        {
            IK::Point_3 p0, p1;
            if (cgal_intersection_util::line_line_3d(last_segment0_start, last_segment0, p0) && cgal_intersection_util::line_line_3d(last_segment1_start, last_segment1, p1))
            {
                pline0_new[0] = p0;
                pline1_new[0] = p1;
            }
        }

        pline0_new.emplace_back(pline0_new.front());
        pline1_new.emplace_back(pline1_new.front());

        // // test segmentation fault 3
        // return;

        // printf("Add holes to the top and bottop outlines \n");

        ///////////////////////////////////////////////////////////////////////////////
        // Only used for tenon-mortise joinery holes
        // Add loose elements from top and bottom outlines also
        // Also check the winding
        //
        // ░░░for multiple holes there is one big hole░░░░for one outline there is one line or one rectangle ░
        // ░░░░░░░░░not a copy of every hole░░░░░░░░░░░░░░░░░░░░██████████████████████████████████░░░░░░░░░░░░
        // ░░░████████████████████████████████████████░░░░░░░░░░│░░░░░░░░░░┌──────────┐░░░░░░░░░░│░░░░░░░░░░░░
        // ░░░█┌────────────────┐░┌─────────────────┐█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
        // ░░░█│░░░░░░░░░░░░░░░░│░│░░░░░░░░░░░░░░░░░│█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
        // ░░░█└────────────────┘░└─────────────────┘█░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
        // ░░░████████████████████████████████████████░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░│░░░░░░░░░░░░
        // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░└──────────┘░░░░░░░░░░└──────────┘░░░░░░░░░░░░
        // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
        //
        ///////////////////////////////////////////////////////////////////////////////

        // std::cout << "iterate holes \n";
        // test segmentation fault 3.0 - error
        // return;
        // auto j_mf_count__ = this->j_mf.size();
        // printf("The value of a : %zu \n", j_mf_count__);
        // printf("The value of a : %zu", j_mf_count);

        for (size_t i = 0; i < this->j_mf.size(); i++)
        { // iterate top outlines only, as i is always 0 and 1
            if (i == 2)
                break;
            // test segmentation fault 3.01 - error
            for (size_t j = 0; j < this->j_mf[i].size(); j++)
            {

                // std::cout << "iterate holes \n";
                // test segmentation fault 3.1 - error
                // return;

                int joint_id = std::get<0>(j_mf[i][j]);
                bool male_or_female = std::get<1>(j_mf[i][j]);

                if (joints[joint_id].name == "")
                    continue;

                if (joints[joint_id](male_or_female, true).size() == 0)
                    continue;
                if (joints[joint_id](male_or_female, false).size() == 0)
                    continue;

                // test segmentation fault 3.2 - error
                // return;

                ///////////////////////////////////////////////////////////////////////////////
                // Check hole position
                ///////////////////////////////////////////////////////////////////////////////
                bool is_geo_reversed =
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, true).back()[0]), joints[joint_id](male_or_female, true).back()[0]) >
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, false).back()[0]), joints[joint_id](male_or_female, false).back()[0]);

                if (is_geo_reversed)
                    joints[joint_id].reverse(male_or_female);

                // test segmentation fault 3.3 - error
                // return;

                ///////////////////////////////////////////////////////////////////////////////
                // Check Winding | we skip the last outline which is the biggest rectangle, if there are multiple holes, it will still have one boundary rectangle
                ///////////////////////////////////////////////////////////////////////////////
                for (int k = 0; k < joints[joint_id](male_or_female, true).size() - 1; k++) // we skip the last outline, because it marks the boundary of all holes
                {
                    // Orient to 2D and check the winding
                    bool is_clockwise = cgal_polyline_util::is_clockwise(joints[joint_id](male_or_female, true)[k], planes[0]);

                    if (!is_clockwise)
                    {
                        std::reverse(
                            std::begin(joints[joint_id](male_or_female, true)[k]),
                            std::end(joints[joint_id](male_or_female, true)[k]));

                        std::reverse(
                            std::begin(joints[joint_id](male_or_female, false)[k]),
                            std::end(joints[joint_id](male_or_female, false)[k]));
                    }

                    // Add holes
                    output[this->id].emplace_back(joints[joint_id](male_or_female, true)[k]);
                    output[this->id].emplace_back(joints[joint_id](male_or_female, false)[k]);
                }
            }
        }

        // test segmentation fault 4 - error
        // return;

        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Collect holes for sides
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // printf("Collect holes for sides \n");
        for (int i = 2; i < this->j_mf.size(); i++)
        {
            for (int j = 0; j < this->j_mf[i].size(); j++)
            {

                int joint_id = std::get<0>(j_mf[i][j]);
                bool male_or_female = std::get<1>(j_mf[i][j]);

                if (joints[joint_id].name == "")
                    continue;

                if (joints[joint_id](male_or_female, true).size() == 0)
                    continue;
                if (joints[joint_id](male_or_female, false).size() == 0)
                    continue;

                ///////////////////////////////////////////////////////////////////////////////
                // Check if there are any holes, if yes continue loop
                ///////////////////////////////////////////////////////////////////////////////
                std::vector<int> id_of_holes;

                for (int k = 0; k < joints[joint_id](male_or_female).size(); k += 2)
                {

                    if (joints[joint_id](male_or_female)[k] == wood_cut::hole) //
                        id_of_holes.emplace_back((int)(k));

                    std::string s(1, joints[joint_id](male_or_female)[k]);
                }

                if (id_of_holes.size() == 0)
                    continue;

                ///////////////////////////////////////////////////////////////////////////////
                // Check hole position
                ///////////////////////////////////////////////////////////////////////////////
                bool is_geo_reversed =
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, true).back()[0]), joints[joint_id](male_or_female, true).back()[0]) >
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, false).back()[0]), joints[joint_id](male_or_female, false).back()[0]);

                if (is_geo_reversed)
                    joints[joint_id].reverse(male_or_female);

                ///////////////////////////////////////////////////////////////////////////////
                // Check Winding and add holes
                ///////////////////////////////////////////////////////////////////////////////
                for (auto &k : id_of_holes)
                {

                    // Orient to 2D and check the winding using top outline
                    bool is_clockwise = cgal_polyline_util::is_clockwise(joints[std::get<0>(j_mf[i][j])](male_or_female, true)[k], planes[0]);

                    if (!is_clockwise)
                    {
                        std::reverse(
                            std::begin(joints[joint_id](male_or_female, true)[k]),
                            std::end(joints[joint_id](male_or_female, true)[k]));

                        std::reverse(
                            std::begin(joints[joint_id](male_or_female, false)[k]),
                            std::end(joints[joint_id](male_or_female, false)[k]));
                    }

                    // std::cout << "i, j, k : " << i << " " << j << " " << k << std::endl;

                    output[this->id].emplace_back(joints[joint_id](male_or_female, true)[k]);
                    output[this->id].emplace_back(joints[joint_id](male_or_female, false)[k]);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////
        // printf("Output \n");
        if (output.size() > this->id)
        { // else the input is bad
            output[this->id].emplace_back(pline0_new);
            output[this->id].emplace_back(pline1_new);
        }
    }

    void element::update_lines_by_intersection(std::vector<wood::joint> &joints, std::vector<wood::element> &elements)
    {
    }
}
