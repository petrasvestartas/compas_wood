#include "../../../stdafx.h"
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
                        std::cout << this->polylines.size() << std::endl;
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
                    // if (this->polylines.size() > 1 && i == 0) {
                    //     output[this->id].emplace_back(this->polylines[0]); //cut
                    //     output[this->id].emplace_back(this->polylines[1]); //cut
                    // }

                    output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[0]);
                    output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[1]);
                    break;
                case (2): // wood::joint volumes
                    // std::cout << joints[std::get<0>(j_mf[i][j])].id << std::endl;
                    //  if (this->polylines.size() > 1 && i == 0) {
                    //      output[this->id].emplace_back(this->polylines[0]); //cut
                    //      output[this->id].emplace_back(this->polylines[1]); //cut
                    //  }

                    if (joints[std::get<0>(j_mf[i][j])].joint_volumes[0].size() > 0)
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[0]);
                    if (joints[std::get<0>(j_mf[i][j])].joint_volumes[1].size() > 0)
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[1]);
                    if (joints[std::get<0>(j_mf[i][j])].joint_volumes[2].size() > 0)
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[2]);
                    if (joints[std::get<0>(j_mf[i][j])].joint_volumes[3].size() > 0)
                        output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_volumes[3]);
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
        /////////////////////////////////////////////////////////////////////////////////////
        // Orient from 3D to 2D
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline a = pline_to_cut;
        CGAL_Polyline b = closed_pline_cutter;

        /////////////////////////////////////////////////////////////////////////////////////
        // Create Transformation
        /////////////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::plane_to_xy(b[0], plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
        cgal_polyline_util::transform(a, xform_toXY);
        cgal_polyline_util::transform(b, xform_toXY);

        /////////////////////////////////////////////////////////////////////////////////////
        // Polylines will not be exactly on the origin due to rounding errors, so make z = 0
        /////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < a.size(); i++)
            a[i] = IK::Point_3(a[i].hx(), a[i].hy(), 0);

        for (int i = 0; i < b.size(); i++)
            b[i] = IK::Point_3(b[i].hx(), b[i].hy(), 0);

        /////////////////////////////////////////////////////////////////////////////////////
        // Find Max Coordinate to get Scale factor
        /////////////////////////////////////////////////////////////////////////////////////

        double max_coordinate = 0;
        for (int i = 0; i < a.size() - 1; i++)
        {
            if (max_coordinate < std::abs(a[i].hx()))
                max_coordinate = std::abs(a[i].hx());

            if (max_coordinate < std::abs(a[i].hy()))
                max_coordinate = std::abs(a[i].hy());
        }

        for (int i = 0; i < b.size() - 1; i++)
        {
            if (max_coordinate < std::abs(b[i].hx()))
                max_coordinate = std::abs(b[i].hx());

            if (max_coordinate < std::abs(a[i].hy()))
                max_coordinate = std::abs(b[i].hy());
        }

        double scale = std::pow(10, 16 - cgal_math_util::count_digits(max_coordinate));
        // CGAL_Debug(scale);
        /////////////////////////////////////////////////////////////////////////////////////
        // Convert to Clipper
        /////////////////////////////////////////////////////////////////////////////////////
        std::vector<Clipper2Lib::Point64> pathA(a.size());
        std::vector<Clipper2Lib::Point64> pathB(b.size() - 1);

        for (int i = 0; i < a.size(); i++)
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

        try
        {
            // elements[i].get_joints_geometry_as_closed_polylines_replacing_edges(joints, plines);

            // ClipperLib::Clipper clipper;

            // clipper.AddPath(pathA, ClipperLib::ptSubject, false);
            // clipper.AddPath(pathB, ClipperLib::ptClip, true);
            // //ClipperLib::Paths C;
            // ClipperLib::PolyTree C;
            // clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

            Clipper2Lib::Paths64 pathA_;
            Clipper2Lib::Paths64 pathB_;
            pathA_.emplace_back(pathA);
            pathB_.emplace_back(pathB);
            Clipper2Lib::Paths64 C = Clipper2Lib::Intersect(pathA_, pathB_, Clipper2Lib::FillRule::EvenOdd);

            // CGAL_Debug(C.ChildCount());
            if (C.size() > 0)
            {
                // Calculate number of points
                c.reserve(C[0].size());
                // CGAL_Debug(count);

                // polynode = C.GetFirst();
                int count = 0;
                for (auto &polynode : C)
                //    while (polynode)
                {
                    // do stuff with polynode here
                    if (polynode.size() <= 1)
                        continue;

                    // Check if seam is correctly placed
                    if (count == 0)
                    {
                        for (size_t j = 0; j < polynode.size(); j++)
                            c.emplace_back(polynode[j].x / scale, polynode[j].y / scale, 0);
                    }
                    else
                    { // if there are multiple segments
                        std::vector<IK::Point_3> pts;
                        pts.reserve(polynode.size());
                        for (size_t j = 0; j < polynode.size(); j++)
                            pts.emplace_back(polynode[j].x / scale, polynode[j].y / scale, 0);

                        // Check if curve is closest to new pline if not reverse
                        if (CGAL::squared_distance(c.back(), pts.front()) > wood_globals::DISTANCE_SQUARED && CGAL::squared_distance(c.back(), pts.back()) > wood_globals::DISTANCE_SQUARED)
                            std::reverse(c.begin(), c.end());

                        // Check if insert able curve end is closest to the main curve end, if not reverse
                        if (CGAL::squared_distance(c.back(), pts.front()) > CGAL::squared_distance(c.back(), pts.back()))
                            std::reverse(pts.begin(), pts.end());

                        for (size_t j = 1; j < pts.size(); j++)
                            c.emplace_back(pts[j]);
                    }
                    // if (count >0) {
                    //		for (size_t j = 0; j < polynode->Contour.size(); j++)
                    //			c.emplace_back(polynode->Contour[j].X / scale, polynode->Contour[j].Y / scale, 0);
                    // }

                    // polynode = polynode->GetNext();
                    count++;
                    // break;
                }

                ///////////////////////////////////////////////////////////////////////////////
                // Get closest parameters (Find closest parameters to edges) and add to pairs
                ///////////////////////////////////////////////////////////////////////////////
                double t0_, t1_;
                bool assigned_t0_ = false;
                bool assigned_t1_ = false;
                // for (int k = edge_pair[0]; k <= edge_pair[1]; k++) {
                for (int k = 0; k <= b.size() - 1; k++)
                {
                    IK::Segment_3 s(b[k], b[k + 1]);
                    double t0;
                    cgal_polyline_util::closest_point_to(c.front(), s, t0);
                    // CGAL_Debug(t0);
                    if (t0 < 0 || t0 > 1)
                        continue;
                    if (CGAL::squared_distance(cgal_polyline_util::point_at(s, t0), c.front()) < wood_globals::DISTANCE_SQUARED)
                    {
                        t0_ = k + t0;
                        assigned_t0_ = true;
                        break;
                    }
                }

                for (int k = 0; k <= b.size() - 1; k++)
                {
                    IK::Segment_3 s(b[k], b[k + 1]);
                    double t1;
                    cgal_polyline_util::closest_point_to(c.back(), s, t1);
                    // CGAL_Debug(t1);
                    if (t1 < 0 || t1 > 1)
                        continue;
                    if (CGAL::squared_distance(cgal_polyline_util::point_at(s, t1), c.back()) < wood_globals::DISTANCE_SQUARED)
                    {
                        assigned_t1_ = true;
                        t1_ = k + t1;
                        break;
                    }
                }

                // CGAL_Debug(assigned_t0_ && assigned_t1_);
                cp_pair = std::pair<double, double>(t0_, t1_);
                if (t0_ > t1_)
                {
                    std::swap(cp_pair.first, cp_pair.second);
                    std::reverse(c.begin(), c.end());
                }
                // transform to 3D space
                for (int i = 0; i < c.size(); i++)
                    c[i] = c[i].transform(xform_toXY_Inv);

                return assigned_t0_ && assigned_t1_;
            }
            else
            {
                return false;
            }
        }
        catch (const std::exception &ex)
        {
            std::cout << "wood_element.cpp -> parameters: scale " << scale << "max_coordinate " << max_coordinate << "\n";
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s   ", __FILE__, __FUNCTION__, __LINE__, ex.what());
        }

        /////////////////////////////////////////////////////////////////////////////////////
        // Output
        /////////////////////////////////////////////////////////////////////////////////////
        return true;
    }

    void element::merge_joints(std::vector<wood::joint> &joints, std::vector<std::vector<CGAL_Polyline>> &output)
    {

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

        std::map<double, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_segments_or_points_0;
        std::map<double, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_segments_or_points_1;

        std::vector<bool> flags0(pline0.size() - 1);
        std::vector<bool> flags1(pline1.size() - 1);
        auto point_count = pline0.size();

        IK::Segment_3 last_segment0_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
        IK::Segment_3 last_segment1_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
        IK::Segment_3 last_segment0;
        IK::Segment_3 last_segment1;
        int last_id = -1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Iterate edge joints ( skip the top and bottom autlines that cannot be merged)
        // "i" refers to the element edge
        // "j" refers to number of joints on one edge, currently "j" is always equals to 0
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

        // std::cout << "New Polygon" << std::endl;
        for (int i = 2; i < this->j_mf.size(); i++)
        {
            for (int j = 0; j < this->j_mf[i].size(); j++)
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
                    // ░░░░░░░░░░░░┌────┐░░░░┌────┐░░░░░░░░░░░
                    // ░░░░░░░░░░░░│░░░░│░░░░│░░░░│░░░░░░░░░░░
                    // ░░░░░░░░░░░░│░░░░│░░░░│░░░░│░░░░░░░░░░░
                    // ░█─────┐████│████│████│████│████┌────█░
                    // ░░░░░░░│░░░░│░░░░│░░░░│░░░░│░░░░│░░░░░░
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
                    // Get the joint line plane
                    //
                    // ░░┌───┐░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    // ░█│ * │██████████████████████████████░
                    // ░░└───┘░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    IK::Plane_3 joint_line_plane_0(joint_line_0[0], planes[i].orthogonal_vector());
                    IK::Plane_3 joint_line_plane_1(joint_line_1[0], planes[i].orthogonal_vector());

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Properties of the polyline
                    // n - number of points in the outline
                    // id - index of the current polyline point
                    //
                    // ░░░░░░░░░░prev░░░░░░░░░░░░░░░░░░░░id░░░
                    // ░░░░░░░░░░░░░░██████████████████╔═░░░░░
                    // ░░░░░░░░░░░┌──┘░░░░░░░░░░░░░░╔══╝░░░░░░
                    // ░░░░░░░░░┌─┘░░░░░░░░░░░░░░╔══╝░░░░░░░░░
                    // ░░░░░░┌──┘░░░░░░░░░░░░░░╔═╝░░░░░░░░░░░░
                    // ░░░░░█████████████████══╝░░░░░░░░░░░░░░
                    // nextnext░░░░░░░░░░░░next░░░░░░░░░░░░░░░
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    auto n = pline0.size() - 1;
                    int id = i - 2;

                    auto prev = (((id - 1) % n) + n) % n;
                    auto next = (((id + 1) % n) + n) % n;
                    auto nextnext = (((id + 2) % n) + n) % n;

                    IK::Segment_3 next_segment_0(pline0[prev], pline0[id]);
                    IK::Segment_3 prev_segment_0(pline0[next], pline0[nextnext]);
                    IK::Segment_3 next_segment_1(pline1[prev], pline1[id]);
                    IK::Segment_3 prev_segment_1(pline1[next], pline1[nextnext]);

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Intersect:
                    // Top or bottom plane
                    // Line plane
                    // Previous or next polyline segment plane
                    //
                    // WARNING: When two side planes are paralel the interseciton is not valid
                    // TODO: this method can be replace by line-line intersection, currently it is done by internsecting 3 planes, which is also working
                    //
                    // ░░░░░░░░░░░┌─────────────────────┬┐░░░░░░░░░░
                    // ░░░░░░░░░┌─┘░░░░░planes[0]░░░░┌──┘│░░░░░░░░░░
                    // ░░░░░░░┌─┘░░░░░██████░░░░░░┌──┘░░░│░░░░░░░░░░
                    // ░░░░░┌─┘░░░░░██████░░░░░┌──┘██░░░░│░░░░░░░░░░
                    // ░░░┌─┘░░░░░░░░░░░░░░░┌──┘░████░░░┌┘░░░░░░░░░░
                    // ░░░├────────────────┬┘░░░░███░┌──┘░░░░░░░░░░░
                    // ░░░│░░░░░████░░░░░░░│░░░░░┌───┘░░░░░░░░░░░░░░
                    // ░░░│░░░░░████░░░░░░░│░┌───┘░░░░░░░░░░░░░░░░░░
                    // ░░░└────────────────┴─┘░░░░joint_line_plane_0
                    // ░░░░░planes[2 + prev]░░░░░░░░░░░░░░░░░░░░░░░░
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    IK::Point_3 p0_int, p1_int, p2_int, p3_int;
                    bool is_intersected_0 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + prev], joint_line_plane_0, planes[0], p0_int);
                    bool is_intersected_1 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + next], joint_line_plane_0, planes[0], p1_int);

                    bool is_intersected_2 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + prev], joint_line_plane_1, planes[1], p2_int);
                    bool is_intersected_3 = cgal_intersection_util::plane_plane_plane_with_parallel_check(planes[2 + next], joint_line_plane_1, planes[1], p3_int);

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

                    IK::Line_3 temp_segment_0(this->polylines[0][i - 2], this->polylines[0][i - 1]);
                    IK::Line_3 temp_segment_1(this->polylines[1][i - 2], this->polylines[1][i - 1]);
                    bool geometry_distance_to_edge_0 = CGAL::squared_distance(joint_line_0[0], temp_segment_0.projection(joint_line_0[0])) > wood_globals::DISTANCE_SQUARED;
                    bool geometry_distance_to_edge_1 = CGAL::squared_distance(joint_line_1[0], temp_segment_1.projection(joint_line_1[0])) > wood_globals::DISTANCE_SQUARED;

                    if (last_id == i - 1 && (geometry_distance_to_edge_0 || geometry_distance_to_edge_1))
                    {

                        IK::Point_3 p0;
                        IK::Point_3 p1;

                        IK::Plane_3 plane_joint_line_0(joint_line_0[0], planes[i - 2].orthogonal_vector());    // current
                        IK::Plane_3 plane_last_segment_0(last_segment0[0], planes[i - 1].orthogonal_vector()); // previous

                        IK::Plane_3 plane_joint_line_1(joint_line_1[0], planes[i - 2].orthogonal_vector());    // current
                        IK::Plane_3 plane_last_segment_1(last_segment1[0], planes[i - 1].orthogonal_vector()); // previous

                        bool is_joint_line_interesected_0 = cgal_intersection_util::plane_plane_plane_with_parallel_check(plane_joint_line_0, plane_last_segment_0, planes[0], p0);
                        bool is_joint_line_interesected_1 = cgal_intersection_util::plane_plane_plane_with_parallel_check(plane_joint_line_1, planes[i - 1], plane_last_segment_1, p1);

                        if (is_joint_line_interesected_0 && is_joint_line_interesected_1)
                        {
                            p0_int = p0;
                            p2_int = p1;
                        }

                        // IF ERROR USE THE FOLLOWING
                        //      // bool do_previous_with_joint_line_intersect_0 = cgal_intersection_util::line_line_3d(last_segment0, joint_line_0, p0);
                        //      // bool do_previous_with_joint_line_intersect_1 = cgal_intersection_util::line_line_3d(last_segment1, joint_line_1, p1);

                        //     p0 = this->polylines[0][i - 2];
                        //     p1 = this->polylines[1][i - 2];

                        //     // if (do_previous_with_joint_line_intersect_0 && do_previous_with_joint_line_intersect_0)
                        //     // {
                        //     //     p0_int = p0;
                        //     //     p2_int = p1;
                        //     // }
                    }

                    ///////////////////////////////////////////////////////////////////////////////
                    // Relocate side segments points to intersection points
                    // If intersecting relocate wood::joint line points --|*---------*|--,
                    // Else overlaping do not change |  *-----*  |.
                    ///////////////////////////////////////////////////////////////////////////////

                    if (is_intersected_0)
                        pline0[id] = p0_int;
                    if (is_intersected_1)
                        pline0[next] = p1_int;
                    if (is_intersected_2)
                        pline1[id] = p2_int;
                    if (is_intersected_3)
                        pline1[next] = p3_int;

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
                    std::cout << "ERROR wood_element Line 879 " << std::endl;

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
                    std::pair<double, double> cp_pair((double)(id + 0.1), (double)(id + 0.9));

                    sorted_segments_or_points_0.insert(std::make_pair(cp_pair.first, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair, joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0]}));
                    sorted_segments_or_points_1.insert(std::make_pair(cp_pair.first, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair, joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), false)[0]}));
                    point_count += joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0].size();
                    break;
                }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    // Rectangle line joint geometry
                    //
                    // ░│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│░
                    // ░│███████████████████████████████████│░
                    // ░│░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░│░
                    // ░└───────────────────────────────────┘░
                    // ░█░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░█░
                    // ░█████████████████████████████████████░
                    // ░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░░
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                case (5):
                {

                    // two edges
                    int edge_pair[2];
                    joints[joint_id].get_edge_ids(male_or_female, edge_pair[0], edge_pair[1]);
                    if (edge_pair[0] > edge_pair[1])
                        std::swap(edge_pair[0], edge_pair[1]);

#ifdef DEBUG_wood_ELEMENT
                    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Intersect rectangle or line CASE 5 ");
#endif
                    if (false)
                    { // split by line, in this case you need to know which side is inside
                    }
                    else
                    { // split by full polygon
                        ///////////////////////////////////////////////////////////////////////////////
                        // 1) Cut polygons and 2) Get closest parameters (Find closest parameters to edges) and add to pairs
                        ///////////////////////////////////////////////////////////////////////////////
                        std::pair<double, double> cp_pair_0(0.0, 0.0);
                        CGAL_Polyline joint_pline_0;
                        bool result0 = intersection_closed_and_open_paths_2D(pline0, joints[joint_id](male_or_female, true).front(), this->planes[0], joint_pline_0, edge_pair, cp_pair_0);
                        if (!result0)
                            continue;

#ifdef DEBUG_wood_ELEMENT
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "First Intersection ");
#endif

                        std::pair<double, double> cp_pair_1(0.0, 0.0);
                        CGAL_Polyline joint_pline_1;
                        bool result1 = intersection_closed_and_open_paths_2D(pline1, joints[joint_id](male_or_female, false).front(), this->planes[1], joint_pline_1, edge_pair, cp_pair_1);
                        if (!result1)
                            continue;
#ifdef DEBUG_wood_ELEMENT
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Second Intersection ");
#endif

                        sorted_segments_or_points_0.insert(
                            std::make_pair(
                                (cp_pair_0.first + cp_pair_0.first) * 0.5,
                                std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_0, joint_pline_0}));
                        sorted_segments_or_points_1.insert(std::make_pair((cp_pair_1.first + cp_pair_1.first) * 0.5, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_1, joint_pline_1}));

                        point_count += joint_pline_1.size();
#ifdef DEBUG_wood_ELEMENT
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Output of Case 5 ");
#endif
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

        ///////////////////////////////////////////////////////////////////////////////
        // Iterate pairs and mark skipped points ids
        // first is key, second - value (pair of cpt (first) and polyline (second))
        ///////////////////////////////////////////////////////////////////////////////
        std::vector<bool> point_flags_0(pline0.size(), true); // point flags to keep corners
        for (auto &pair : sorted_segments_or_points_0)
            for (size_t j = (size_t)std::ceil(pair.second.first.first); j <= (size_t)std::floor(pair.second.first.second); j++)
            { // are corners in between insertable polylines
                point_flags_0[j] = false;
                point_count--;
            }
        point_flags_0[point_flags_0.size() - 1] = false; // ignore last

#ifdef DEBUG_wood_ELEMENT
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Iterate pairs and mark skipped points ids ");
#endif

        std::vector<bool> point_flags_1(pline0.size(), true); // point flags to keep corners
        for (auto &pair : sorted_segments_or_points_1)
            for (size_t j = (size_t)std::ceil(pair.second.first.first); j <= (size_t)std::floor(pair.second.first.second); j++) // are corners in between insertable polylines
                point_flags_1[j] = false;
        point_flags_1[point_flags_1.size() - 1] = false; // ignore last

        // Skip first in case there is a cut on the corner
        if (std::floor(sorted_segments_or_points_0.begin()->second.first.first) < 1 && std::ceil(sorted_segments_or_points_0.begin()->second.first.second) == (pline0.size() - 1))
        {
            std::reverse(sorted_segments_or_points_0.begin()->second.second.begin(), sorted_segments_or_points_0.begin()->second.second.end());
            point_flags_0[0] = false;
            point_count--;
        }

        if (std::floor(sorted_segments_or_points_1.begin()->second.first.first) < 1 && std::ceil(sorted_segments_or_points_1.begin()->second.first.second) == (pline1.size() - 1))
        {
            std::reverse(sorted_segments_or_points_1.begin()->second.second.begin(), sorted_segments_or_points_1.begin()->second.second.end());
            point_flags_1[0] = false;
        }

        ///////////////////////////////////////////////////////////////////////////////
        // Add polygons including points to sorted map and merge
        ///////////////////////////////////////////////////////////////////////////////

        for (size_t i = 0; i < point_flags_0.size(); i++)
            if (point_flags_0[i])
                sorted_segments_or_points_0.insert(std::make_pair((double)i, std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>((double)i, (double)i), CGAL_Polyline{pline0[i]}}));

        for (size_t i = 0; i < point_flags_1.size(); i++)
            if (point_flags_1[i])
                sorted_segments_or_points_1.insert(std::make_pair((double)i, std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>((double)i, (double)i), CGAL_Polyline{pline1[i]}}));

        CGAL_Polyline pline0_new; // reserve optimize
        CGAL_Polyline pline1_new; // reserve optimize
        pline0_new.reserve(point_count);
        pline1_new.reserve(point_count);

        for (auto const &x : sorted_segments_or_points_0)
        {
            for (size_t j = 0; j < x.second.second.size(); j++)
                pline0_new.emplace_back(x.second.second[j]);
        }

        for (auto const &x : sorted_segments_or_points_1)
        {
            for (size_t j = 0; j < x.second.second.size(); j++)
                pline1_new.emplace_back(x.second.second[j]);
        }

#ifdef DEBUG_wood_ELEMENT
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Add polygons including points to sorted map and merge ");
#endif
        ///////////////////////////////////////////////////////////////////////////////
        // Close
        ///////////////////////////////////////////////////////////////////////////////
        if (last_id == this->polylines[0].size() && last_segment0_start.squared_length() > wood_globals::DISTANCE_SQUARED)
        {
            IK::Point_3 p0;
            IK::Point_3 p1;
            if (cgal_intersection_util::line_line_3d(last_segment0_start, last_segment0, p0) && cgal_intersection_util::line_line_3d(last_segment1_start, last_segment1, p1))
            {
                pline0_new[0] = p0;
                pline1_new[0] = p1;
            }
            // else
            // {
            //     return;
            // }
        }

        pline0_new.emplace_back(pline0_new.front());
        pline1_new.emplace_back(pline1_new.front());

        ///////////////////////////////////////////////////////////////////////////////
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
        for (int i = 0; i < 2; i++)
        { // iterate top outlines only, as i is always 0 and 1
            for (size_t j = 0; j < j_mf[i].size(); j++)
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
                // Check hole position
                ///////////////////////////////////////////////////////////////////////////////
                bool is_geo_reversed =
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, true).back()[0]), joints[joint_id](male_or_female, true).back()[0]) >
                    CGAL::squared_distance(planes[0].projection(joints[joint_id](male_or_female, false).back()[0]), joints[joint_id](male_or_female, false).back()[0]);

                if (is_geo_reversed)
                    joints[joint_id].reverse(male_or_female);

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

        // Collect holes for sides, this was first implemented for miter joints

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

#ifdef DEBUG_wood_ELEMENT
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %i %i %i %i ", __FILE__, __FUNCTION__, __LINE__, "Add elements  ", pline0_new.size(), pline1_new.size(), this->id, output.size());
#endif

        ///////////////////////////////////////////////////////////////////////////////
        // Output
        ///////////////////////////////////////////////////////////////////////////////
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