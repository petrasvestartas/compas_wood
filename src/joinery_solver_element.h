#pragma once
#include <string>

#include "cgal.h"
#include "joinery_solver_joint.h"

#include "cgal_intersection_util.h"
#include "cgal_xform_util.h"
#include "cgal_polyline_util.h"
#include "cgal_print.h"

class element {
public:
    /////////////////////////////////////////////////////////////////////////////////////////
    //Parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    int id = -1;
    double thickness = 0;

    /////////////////////////////////////////////////////////////////////////////////////////
    //Basic Geometry Parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    CGAL::Bbox_3 aabb;
    IK::Vector_3 oob[5];
    std::vector<CGAL_Polyline> polylines; //pairs of polylines + each side face
    std::vector<IK::Plane_3> planes;	  //top and bottom planes + each side face

    std::vector<IK::Vector_3> edge_vectors; //if set
    std::vector<int> joint_types;			//if set - used in joint_library | method construct_joint_by_index
    IK::Segment_3 axis;

    //For beams only
    CGAL_Polyline central_polyline;

    /////////////////////////////////////////////////////////////////////////////////////////
    //Joinery Geometry Parameters
    //pointers to joints, joint geometry will be modified inside joint to avoid copy
    //joint must be sorted according to edge id to be merged
    /////////////////////////////////////////////////////////////////////////////////////////
    //std::map<int, std::pair<bool, int>>edgeID_mf_jointID; //e0,true,jointID
    std::vector < std::vector<std::tuple<int, bool, double>>> j_mf; //(joint id, male/female, parameter on edge) elements[result[i + 0]].j_mf[e0].push_back(std::tuple<int, bool, double>(jointID, true,0));
    //std::vector<CGAL_Polyline> modified_polylines;
    //public Mesh mesh;

    /////////////////////////////////////////////////////////////////////////////////////////
    //Component parameters
    /////////////////////////////////////////////////////////////////////////////////////////
    std::string key;
    IK::Plane_3 component_plane;

    /////////////////////////////////////////////////////////////////////////////////////////
    //Constructors
    /////////////////////////////////////////////////////////////////////////////////////////
    element();
    element(int);

    void get_joints_geometry(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output, int what_to_expose);
    void get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output);
    bool intersection_closed_and_open_paths_2D(
        CGAL_Polyline& closed_pline_cutter, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c,
        int(&edge_pair)[2], std::pair<double, double>& cp_pair);
    void merge_joints(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output);
};

inline element::element() {}

inline element::element(int _id) : id(_id) {
}

inline void element::get_joints_geometry(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output, int what_to_expose) {
    //you are in a loop
    for (int i = 0; i < j_mf.size(); i++) { //loop joint id
        for (size_t j = 0; j < j_mf[i].size(); j++) { //loop joints per each face + 1 undefined
            switch (what_to_expose) {
            case (0)://Plate outlines
                if (this->polylines.size() > 1) {
                    output[this->id].emplace_back(this->polylines[0]); //cut
                    output[this->id].emplace_back(this->polylines[1]); //cut
                }

                output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_area);
                break;
            case (1)://joint lines
                if (this->polylines.size() > 1) {
                    output[this->id].emplace_back(this->polylines[0]); //cut
                    output[this->id].emplace_back(this->polylines[1]); //cut
                }

                output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[0]);
                output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])].joint_lines[1]);
                break;
            case (2)://joint volumes

                if (this->polylines.size() > 1) {
                    output[this->id].emplace_back(this->polylines[0]); //cut
                    output[this->id].emplace_back(this->polylines[1]); //cut
                }

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

                if (this->polylines.size() > 1) {
                    output[this->id].emplace_back(this->polylines[0]); //cut
                    output[this->id].emplace_back(this->polylines[1]); //cut
                }

                for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size(); k++) {
                    output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);  //cut
                    output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]); //direction
                }

                break;
            default:
                break;
            }
        }
    }
}

inline bool sort_by_third(const std::tuple<int, bool, double>& a, const std::tuple<int, bool, double>& b) {
    return (std::get<2>(a) < std::get<2>(b));
}

inline void element::get_joints_geometry_as_closed_polylines_replacing_edges(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output) {
    //you are in a loop

    ///////////////////////////////////////////////////////////////////////////////
    //Copy top and bottom polylines
    ///////////////////////////////////////////////////////////////////////////////
    CGAL_Polyline polyline0;
    CGAL_Polyline polyline1;
    cgal_polyline_util::Duplicate(polylines[0], polyline0);
    cgal_polyline_util::Duplicate(polylines[1], polyline1);
    int n = polyline0.size() - 1;
    bool debug = false;
    if (debug)
        CGAL_Debug(-4);

    ///////////////////////////////////////////////////////////////////////////////
    //Sorts joints by edges
    //Get closest parameter to edge and sort by this values
    ///////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < n; i++) { //iterate edge count
        for (int j = 0; j < this->j_mf[i + 2].size(); j++) { //iterate edges, skipping first two
            if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                continue;

            IK::Segment_3 element_edge(polyline0[i], polyline0[i + 1]);
            IK::Point_3 joint_point = joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0];
            double t;
            cgal_intersection_util::ClosestPointTo(joint_point, element_edge, t);
            std::get<2>(j_mf[i + 2][j]) = t;
        }

        std::sort(j_mf[i + 2].begin(), j_mf[i + 2].end(), sort_by_third);
    }

    //CGAL_Debug(polylines[0][0], true);
    //CGAL_Debug(polylines[0][1], true);

    ///////////////////////////////////////////////////////////////////////////////
    //Perform Intersection, skip first two elements because they are top and bottom
    ///////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < n; i++) { //iterate sides only
        for (int j = 0; j < this->j_mf[i + 2].size(); j++) { //
            if (j_mf[i + 2].size() > 0) { //only if one line can be inserted
                ///////////////////////////////////////////////////////////////////////////////
                //Skip undefined names and if tiles has more than 1 polyline to insert
                ///////////////////////////////////////////////////////////////////////////////
                if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                    continue;
                if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() != 2)
                    continue;

                ///////////////////////////////////////////////////////////////////////////////
                //Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
                ///////////////////////////////////////////////////////////////////////////////
                bool flag =
                    CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0]) >
                    CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0]), joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0]);

                if (flag)
                    joints[std::get<0>(j_mf[i + 2][j])].reverse(std::get<1>(j_mf[i + 2][j]));

                IK::Segment_3 joint_line_0(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).back()[1]);
                IK::Segment_3 joint_line_1(joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[0], joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false).back()[1]);

                ///////////////////////////////////////////////////////////////////////////////
                //Take last lines
                ///////////////////////////////////////////////////////////////////////////////
                int prev = (((i - 1) % n) + n) % n;
                int next = (((i + 1) % n) + n) % n;
                int nextnext = (((i + 2) % n) + n) % n;

                IK::Segment_3 next_segment_0(polyline0[prev], polyline0[i]);
                IK::Segment_3 prev_segment_0(polyline0[next], polyline0[nextnext]);
                IK::Segment_3 next_segment_1(polyline1[prev], polyline1[i]);
                IK::Segment_3 prev_segment_1(polyline1[next], polyline1[nextnext]);
                if (debug)
                    CGAL_Debug(4);

                ///////////////////////////////////////////////////////////////////////////////
                //Intersect them with side lines, same principle must work on both polygons
                ///////////////////////////////////////////////////////////////////////////////
                //1 Perform intersection line-line (needs implementation from rhino)
                //3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
                IK::Point_3 p0_int, p1_int, p2_int, p3_int;
                double t0_int, t1_int, t2_int, t3_int;

                IK::Plane_3 joint_line_plane_0_prev(joint_line_0[0], planes[2 + i].orthogonal_vector());
                IK::Plane_3 joint_line_plane_0_next(joint_line_0[0], planes[2 + i].orthogonal_vector());
                IK::Plane_3 joint_line_plane_1_prev(joint_line_1[0], planes[2 + i].orthogonal_vector());
                IK::Plane_3 joint_line_plane_1_next(joint_line_1[0], planes[2 + i].orthogonal_vector());

                IK::Vector_3 v(0, 0, 2);

                bool flag0 = cgal_intersection_util::plane_plane_plane(planes[2 + prev], joint_line_plane_0_prev, planes[0], p0_int, joint_line_0, t0_int);

                //output.push_back({ p0_int + v,joint_line_0[0]+ v,cgal_polyline_util::Center(polylines[2 + prev]) + v,cgal_polyline_util::Center(polylines[0]) + v });

                bool flag1 = cgal_intersection_util::plane_plane_plane(planes[2 + next], joint_line_plane_0_next, planes[0], p1_int, joint_line_0, t1_int);
                //output.push_back({ p1_int + v ,joint_line_0[0] + v,cgal_polyline_util::Center(polylines[2 + next]) + v,cgal_polyline_util::Center(polylines[0]) + v });

                bool flag2 = cgal_intersection_util::plane_plane_plane(planes[2 + prev], joint_line_plane_1_prev, planes[1], p2_int, joint_line_1, t2_int);
                //output.push_back({ p2_int + v,joint_line_1[0] + v,cgal_polyline_util::Center(polylines[2 + prev]) + v,cgal_polyline_util::Center(polylines[1]) + v });

                bool flag3 = cgal_intersection_util::plane_plane_plane(planes[2 + next], joint_line_plane_1_next, planes[1], p3_int, joint_line_1, t3_int);
                //output.push_back({ p3_int + v,joint_line_1[0] + v,cgal_polyline_util::Center(polylines[2 + next]) + v,cgal_polyline_util::Center(polylines[1]) + v });

                ///////////////////////////////////////////////////////////////////////////////
                //2 Relocate side segments points to intersection points
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
                //3 Check orientation of joint outlines, if needed flip
                ///////////////////////////////////////////////////////////////////////////////
                bool flip = CGAL::has_smaller_distance_to_point(
                    polyline0[i],
                    joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].front(),
                    joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].back());
                ;
                if (!flip) {
                    //bottom
                    std::reverse(
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].begin(),
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), !flag)[0].end());

                    //top
                    std::reverse(
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), flag)[0].begin(),
                        joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), flag)[0].end());
                }
            } //if joint edge
        }	  //for j
    }		  //for i

    //Update the end
    polyline0[polyline0.size() - 1] = polyline0[0];
    polyline1[polyline1.size() - 1] = polyline1[0];

    ///////////////////////////////////////////////////////////////////////////////
    //After intersection is ready merger all polylines
    ///////////////////////////////////////////////////////////////////////////////
    CGAL_Polyline newPolyline0;
    CGAL_Polyline newPolyline1;

    for (int i = 0; i < n; i++) { //iterate sides only
        newPolyline0.push_back(polyline0[i]);
        newPolyline1.push_back(polyline1[i]);

        for (int j = 0; j < this->j_mf[i + 2].size(); j++) { //
            if (j_mf[i + 2].size() > 0) {
                if (joints[std::get<0>(j_mf[i + 2][j])].name == "" || joints[std::get<0>(j_mf[i + 2][j])].name == "cr_c_ip_0")
                    continue;
                if (joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true).size() == 2) {
                    for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), true)[0])
                        newPolyline0.push_back(pp);
                    for (auto pp : joints[std::get<0>(j_mf[i + 2][j])](std::get<1>(j_mf[i + 2][j]), false)[0])
                        newPolyline1.push_back(pp);
                }
            }
        }

        if (i == n - 1) {
            newPolyline0.push_back(newPolyline0[0]);
            newPolyline1.push_back(newPolyline1[0]);
            output[this->id].push_back(newPolyline0);
            output[this->id].push_back(newPolyline1);
        }
    }

    for (int i = 0; i < 2; i++) { //iterate top only
        for (size_t j = 0; j < j_mf[i].size(); j++) {
            if (joints[std::get<0>(j_mf[i][j])].name == "")
                continue;

            //Check hole position
            bool flag =
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]) >
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]);

            if (flag)
                joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));

            for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() - 1; k++) {
                output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);
                output[this->id].push_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);
            }
        }
    }

    //close

    ///////////////////////////////////////////////////////////////////////////////
    //output
    ///////////////////////////////////////////////////////////////////////////////

    return;
}

//inline bool element::intersection_closed_and_open_paths_2D(CGAL_Polyline& closed_pline_cutter, std::pair<int, int>& edge_pair, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c, CGAL_Polyline& output, std::pair<double, double>& cp_pair) {
inline bool element::intersection_closed_and_open_paths_2D(
    CGAL_Polyline& closed_pline_cutter, CGAL_Polyline& pline_to_cut, IK::Plane_3& plane, CGAL_Polyline& c,
    int(&edge_pair)[2], std::pair<double, double>& cp_pair) {
    /////////////////////////////////////////////////////////////////////////////////////
    //Orient from 3D to 2D
    /////////////////////////////////////////////////////////////////////////////////////
    CGAL_Polyline a;
    CGAL_Polyline b;
    cgal_polyline_util::Duplicate(pline_to_cut, a);
    cgal_polyline_util::Duplicate(closed_pline_cutter, b);

    /////////////////////////////////////////////////////////////////////////////////////
    //Create Transformation
    /////////////////////////////////////////////////////////////////////////////////////
    CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(b[0], plane);
    CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
    cgal_polyline_util::Transform(a, xform_toXY);
    cgal_polyline_util::Transform(b, xform_toXY);

    /////////////////////////////////////////////////////////////////////////////////////
    //Polylines will not be exactly on the origin due to rounding errors, so make z = 0
    /////////////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < a.size(); i++)
        a[i] = IK::Point_3(a[i].hx(), a[i].hy(), 0);

    for (int i = 0; i < b.size(); i++)
        b[i] = IK::Point_3(b[i].hx(), b[i].hy(), 0);

    /////////////////////////////////////////////////////////////////////////////////////
    //Find Max Coordinate to get Scale factor
    /////////////////////////////////////////////////////////////////////////////////////

    double max_coordinate = 0;
    for (int i = 0; i < a.size() - 1; i++) {
        if (max_coordinate < std::abs(a[i].hx()))
            max_coordinate = std::abs(a[i].hx());

        if (max_coordinate < std::abs(a[i].hy()))
            max_coordinate = std::abs(a[i].hy());
    }

    for (int i = 0; i < b.size() - 1; i++) {
        if (max_coordinate < std::abs(b[i].hx()))
            max_coordinate = std::abs(b[i].hx());

        if (max_coordinate < std::abs(a[i].hy()))
            max_coordinate = std::abs(b[i].hy());
    }

    double scale = std::pow(10, 16 - cgal_math_util::count_digits(max_coordinate));
    //CGAL_Debug(scale);
    /////////////////////////////////////////////////////////////////////////////////////
    //Convert to Clipper
    /////////////////////////////////////////////////////////////////////////////////////
    std::vector<ClipperLib::IntPoint> pathA(a.size());
    std::vector<ClipperLib::IntPoint> pathB(b.size() - 1);

    for (int i = 0; i < a.size(); i++) {
        pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
        //printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
    }
    //printf("\n");
    for (int i = 0; i < b.size() - 1; i++) {
        pathB[i] = ClipperLib::IntPoint(b[i].x() * scale, b[i].y() * scale);
        //printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
    }

    try {
        //elements[i].get_joints_geometry_as_closed_polylines_replacing_edges(joints, plines);

        ClipperLib::Clipper clipper;

        clipper.AddPath(pathA, ClipperLib::ptSubject, false);
        clipper.AddPath(pathB, ClipperLib::ptClip, true);
        //ClipperLib::Paths C;
        ClipperLib::PolyTree C;
        clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftEvenOdd, ClipperLib::pftEvenOdd);

        //CGAL_Debug(C.ChildCount());
        if (C.ChildCount() > 0) {
            //Calculate number of points
            int count = 0;
            ClipperLib::PolyNode* polynode = C.GetFirst();
            while (polynode) {
                //do stuff with polynode here
                if (polynode->Contour.size() <= 1)
                    continue;
                count += polynode->Contour.size();
                polynode = polynode->GetNext();

                break;
            }
            c.reserve(count);
            //CGAL_Debug(count);

            polynode = C.GetFirst();
            count = 0;
            while (polynode) {
                //do stuff with polynode here
                if (polynode->Contour.size() <= 1)
                    continue;

                //Check if seam is correctly placed
                if (count == 0) {
                    for (size_t j = 0; j < polynode->Contour.size(); j++)
                        c.emplace_back(polynode->Contour[j].X / scale, polynode->Contour[j].Y / scale, 0);
                }
                else { //if there are multiple segments
                    std::vector<IK::Point_3> pts;
                    pts.reserve(polynode->Contour.size());
                    for (size_t j = 0; j < polynode->Contour.size(); j++)
                        pts.emplace_back(polynode->Contour[j].X / scale, polynode->Contour[j].Y / scale, 0);

                    //Check if curve is closest to new pline if not reverse
                    if (CGAL::squared_distance(c.back(), pts.front()) > GlobalToleranceSquare && CGAL::squared_distance(c.back(), pts.back()) > GlobalToleranceSquare)
                        std::reverse(c.begin(), c.end());

                    //Check if insert able curve end is closest to the main curve end, if not reverse
                    if (CGAL::squared_distance(c.back(), pts.front()) > CGAL::squared_distance(c.back(), pts.back()))
                        std::reverse(pts.begin(), pts.end());

                    for (size_t j = 1; j < pts.size(); j++)
                        c.emplace_back(pts[j]);
                }
                //if (count >0) {
                //		for (size_t j = 0; j < polynode->Contour.size(); j++)
                //			c.emplace_back(polynode->Contour[j].X / scale, polynode->Contour[j].Y / scale, 0);
                //}

                polynode = polynode->GetNext();
                count++;
                //break;
            }

            ///////////////////////////////////////////////////////////////////////////////
            //Get closest parameters (Find closest parameters to edges) and add to pairs
            ///////////////////////////////////////////////////////////////////////////////
            double t0_, t1_;
            bool assigned_t0_ = false;
            bool assigned_t1_ = false;
            //for (int k = edge_pair[0]; k <= edge_pair[1]; k++) {
            for (int k = 0; k <= b.size() - 1; k++) {
                IK::Segment_3 s(b[k], b[k + 1]);
                double t0;
                cgal_polyline_util::ClosestPointTo(c.front(), s, t0);
                //CGAL_Debug(t0);
                if (t0 < 0 || t0 > 1)
                    continue;
                if (CGAL::squared_distance(cgal_polyline_util::PointAt(s, t0), c.front()) < GlobalToleranceSquare) {
                    t0_ = k + t0;
                    assigned_t0_ = true;
                    break;
                }
            }

            for (int k = 0; k <= b.size() - 1; k++) {
                IK::Segment_3 s(b[k], b[k + 1]);
                double t1;
                cgal_polyline_util::ClosestPointTo(c.back(), s, t1);
                //CGAL_Debug(t1);
                if (t1 < 0 || t1 > 1)
                    continue;
                if (CGAL::squared_distance(cgal_polyline_util::PointAt(s, t1), c.back()) < GlobalToleranceSquare) {
                    assigned_t1_ = true;
                    t1_ = k + t1;
                    break;
                }
            }

            //CGAL_Debug(assigned_t0_ && assigned_t1_);
            cp_pair = std::pair<double, double>(t0_, t1_);
            if (t0_ > t1_) {
                std::swap(cp_pair.first, cp_pair.second);
                std::reverse(c.begin(), c.end());
            }
            //Transform to 3D space
            for (int i = 0; i < c.size(); i++)
                c[i] = c[i].transform(xform_toXY_Inv);

            return assigned_t0_ && assigned_t1_;
        }
        else {
            return false;
        }
    }
    catch (const  std::exception& ex) {
        CGAL_Debug(scale);
        CGAL_Debug(max_coordinate);
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s   ", __FILE__, __FUNCTION__, __LINE__, ex.what());
    }

    /////////////////////////////////////////////////////////////////////////////////////
    //Output
    /////////////////////////////////////////////////////////////////////////////////////
    return true;
}

inline void element::merge_joints(std::vector<joint>& joints, std::vector<std::vector<CGAL_Polyline>>& output) {
    //OPTIMIZE CASE(5) BECAUSE EDGE ARE KNOWN, BUT CHECK ALSO CROSS JOINT ENSURE THAT YOU TAKE CROSSING EDGES
    //CHANGE TO 2D METHOD, TO AVOID MULTIPLE THE SAME MATRIX CREATION FOR ORIENTATION TO 2D I.E. CLIPPER AND LINELINE3D
    //you are in a loop
    //only for objects that has one element as a joint and edges to insert are known
    //List order: top0 bottom0 top1 bottom1 ... topN bottomN

    CGAL_Polyline pline0 = this->polylines[0];
    CGAL_Polyline pline1 = this->polylines[1];

    std::map<double, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_segments_or_points_0;
    std::map<double, std::pair<std::pair<double, double>, CGAL_Polyline>> sorted_segments_or_points_1;

    std::vector<bool> flags0(pline0.size() - 1);
    std::vector<bool> flags1(pline1.size() - 1);
    int point_count = pline0.size();

    IK::Segment_3 last_segment0_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
    IK::Segment_3 last_segment1_start(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
    IK::Segment_3 last_segment0;
    IK::Segment_3 last_segment1;
    int lastID = -1;

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Loops starts");
#endif

    for (int i = 2; i < this->j_mf.size(); i++) {
        for (int j = 0; j < this->j_mf[i].size(); j++) { //
            ///////////////////////////////////////////////////////////////////////////////
            //Skip undefined names and if tiles has more than 1 polyline to insert
            ///////////////////////////////////////////////////////////////////////////////
            bool male_or_female = std::get<1>(j_mf[i][j]);
            if (joints[std::get<0>(j_mf[i][j])].name == "")
                continue;

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s num of elements %zi ", __FILE__, __FUNCTION__, __LINE__, "Good Name", joints[std::get<0>(j_mf[i][j])](male_or_female, true).size());
#endif

            if (joints[std::get<0>(j_mf[i][j])](male_or_female, true).size() != 2)
                continue;
            if (joints[std::get<0>(j_mf[i][j])](male_or_female, false).size() == 0)
                continue;
            ///////////////////////////////////////////////////////////////////////////////
            //Take last lines Element outline can be not in the same order as joint outlines Take joint segment and measure its point to the plane
            ///////////////////////////////////////////////////////////////////////////////

            bool flag =
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](male_or_female, true).back()[0]), joints[std::get<0>(j_mf[i][j])](male_or_female, true).back()[0]) >
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](male_or_female, false).back()[0]), joints[std::get<0>(j_mf[i][j])](male_or_female, false).back()[0]);

            if (flag)
                joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %zi ", __FILE__, __FUNCTION__, __LINE__, "Intersect rectangle or line", joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back().size());
#endif

            ///////////////////////////////////////////////////////////////////////////////
            //Intersect rectangle or line
            ///////////////////////////////////////////////////////////////////////////////

            switch (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back().size()) {
            case (2):
            { //Reposition end points
//#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
//                    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Intersect rectangle or line CASE 2 ");
//#endif
                int id = i - 2;
                int n = pline0.size() - 1;

                IK::Segment_3 joint_line_0(joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true).back()[0], joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true).back()[1]);
                IK::Segment_3 joint_line_1(joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), false).back()[0], joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), false).back()[1]);

                ///////////////////////////////////////////////////////////////////////////////
                //Take last lines
                ///////////////////////////////////////////////////////////////////////////////
                int prev = (((id - 1) % n) + n) % n;
                int next = (((id + 1) % n) + n) % n;
                int nextnext = (((id + 2) % n) + n) % n;

                IK::Segment_3 next_segment_0(pline0[prev], pline0[id]);
                IK::Segment_3 prev_segment_0(pline0[next], pline0[nextnext]);
                IK::Segment_3 next_segment_1(pline1[prev], pline1[id]);
                IK::Segment_3 prev_segment_1(pline1[next], pline1[nextnext]);

                ///////////////////////////////////////////////////////////////////////////////
                //Intersect them with side lines, same principle must work on both polygons
                ///////////////////////////////////////////////////////////////////////////////
                //1 Perform intersection line-line (needs implementation from rhino)
                //3 If intersecting relocate joint line points --|*---------*|--, if not overlaping do not change |  *-----*  |.
                IK::Point_3 p0_int, p1_int, p2_int, p3_int;
                double t0_int, t1_int, t2_int, t3_int;

                IK::Plane_3 joint_line_plane_0_prev = IK::Plane_3(joint_line_0[0], planes[2 + id].orthogonal_vector()); // IK::Plane_3(prev_segment_0[0], planes[2 + id].orthogonal_vector());
                IK::Plane_3 joint_line_plane_0_next(joint_line_0[0], planes[2 + id].orthogonal_vector());
                IK::Plane_3 joint_line_plane_1_prev = IK::Plane_3(joint_line_1[0], planes[2 + id].orthogonal_vector()); // IK::Plane_3(prev_segment_1[0], planes[2 + id].orthogonal_vector());
                IK::Plane_3 joint_line_plane_1_next(joint_line_1[0], planes[2 + id].orthogonal_vector());

                bool flag0 = cgal_intersection_util::plane_plane_plane(planes[2 + prev], joint_line_plane_0_prev, planes[0], p0_int, joint_line_0, t0_int);
                bool flag1 = cgal_intersection_util::plane_plane_plane(planes[2 + next], joint_line_plane_0_next, planes[0], p1_int, joint_line_0, t1_int);
                bool flag2 = cgal_intersection_util::plane_plane_plane(planes[2 + prev], joint_line_plane_1_prev, planes[1], p2_int, joint_line_1, t2_int);
                bool flag3 = cgal_intersection_util::plane_plane_plane(planes[2 + next], joint_line_plane_1_next, planes[1], p3_int, joint_line_1, t3_int);

                ///////////////////////////////////////////////////////////////////////////////
                //2 Relocate side segments points to intersection points
                ///////////////////////////////////////////////////////////////////////////////

                if (lastID == i - 1) {
                    IK::Point_3 p0;
                    IK::Point_3 p1;
                    if (cgal_intersection_util::LineLine3D(last_segment0, joint_line_0, p0) && cgal_intersection_util::LineLine3D(last_segment1, joint_line_1, p1)) {
                        p0_int = p0;
                        p2_int = p1;
                    }
                }

                if (flag0)
                    pline0[id] = p0_int;
                if (flag1)
                    pline0[next] = p1_int;
                if (flag2)
                    pline1[id] = p2_int;
                if (flag3)
                    pline1[next] = p3_int;

                last_segment0 = joint_line_0;
                last_segment1 = joint_line_1;
                if (i == 2) {
                    last_segment0_start = joint_line_0;
                    last_segment1_start = joint_line_1;
                }

                lastID = i;
                ///////////////////////////////////////////////////////////////////////////////
                //3 Check orientation of joint outlines, if needed flip
                ///////////////////////////////////////////////////////////////////////////////
                bool flip = CGAL::has_smaller_distance_to_point(
                    pline0[id],
                    joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), !flag)[0].front(),
                    joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), !flag)[0].back());

                if (!flip) {
                    //bottom
                    std::reverse(
                        joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), !flag)[0].begin(),
                        joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), !flag)[0].end());

                    //top
                    std::reverse(
                        joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), flag)[0].begin(),
                        joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), flag)[0].end());
                }

                ///////////////////////////////////////////////////////////////////////////////
                //Get closest parameters (edge start, start+1) and add to pairs
                ///////////////////////////////////////////////////////////////////////////////
                std::pair<double, double> cp_pair(id + 0.1, id + 0.9);

                sorted_segments_or_points_0.insert(std::make_pair(cp_pair.first, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair, joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0]}));
                sorted_segments_or_points_1.insert(std::make_pair(cp_pair.first, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair, joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), false)[0]}));
                point_count += joints[std::get<0>(j_mf[id + 2][j])](std::get<1>(j_mf[id + 2][j]), true)[0].size();
                break;
            }
            case (5):
            { //two edges
                int edge_pair[2];
                joints[std::get<0>(j_mf[i][j])].get_edge_ids(std::get<1>(j_mf[i][j]), edge_pair[0], edge_pair[1]);
                if (edge_pair[0] > edge_pair[1])
                    std::swap(edge_pair[0], edge_pair[1]);

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
                printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Intersect rectangle or line CASE 5 ");
#endif
                if (false) { //split by line, in this case you need to know which side is inside
                }
                else { //split by full polygon
                 ///////////////////////////////////////////////////////////////////////////////
                 //1) Cut polygons and 2) Get closest parameters (Find closest parameters to edges) and add to pairs
                 ///////////////////////////////////////////////////////////////////////////////
                    std::pair<double, double> cp_pair_0(0, 0);
                    CGAL_Polyline joint_pline_0;
                    bool result0 = intersection_closed_and_open_paths_2D(pline0, joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).front(), this->planes[0], joint_pline_0, edge_pair, cp_pair_0);
                    if (!result0) continue;

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
                    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "First Intersection ");
#endif

                    std::pair<double, double> cp_pair_1(0, 0);
                    CGAL_Polyline joint_pline_1;
                    bool result1 = intersection_closed_and_open_paths_2D(pline1, joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).front(), this->planes[1], joint_pline_1, edge_pair, cp_pair_1);
                    //CGAL_Debug(pline1);
                    //CGAL_Debug(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).front());
                    if (!result1) continue;
#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
                    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Second Intersection ");
#endif

                    sorted_segments_or_points_0.insert(
                        std::make_pair(
                            (cp_pair_0.first + cp_pair_0.first) * 0.5,
                            std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_0, joint_pline_0}
                    )
                    );
                    sorted_segments_or_points_1.insert(std::make_pair((cp_pair_1.first + cp_pair_1.first) * 0.5, std::pair<std::pair<double, double>, CGAL_Polyline>{cp_pair_1, joint_pline_1}));

                    point_count += joint_pline_1.size();
#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
                    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Output of Case 5 ");
#endif

                    //CGAL_Debug((cp_pair_0.first + cp_pair_0.first) * 0.5, cp_pair_0.first, cp_pair_0.second);
                    //CGAL_Debug((cp_pair_1.first + cp_pair_1.first) * 0.5, cp_pair_1.first, cp_pair_1.second);

                    //CGAL_Debug(joint_pline_1.size());
                    //for (size_t i = 0; i < joint_pline_1.size(); i++) {
                    //    CGAL_Debug(joint_pline_1[i]);
                    //}
                }

                break;
            }
            default:
                continue;
                break;
            }
        }

        //Unify windings of polylines
    }
    //return;
    ///////////////////////////////////////////////////////////////////////////////
    //Iterate pairs and mark skipped points ids
    //first is key, second - value (pair of cpt (first) and polyline (second))
    ///////////////////////////////////////////////////////////////////////////////
    std::vector<bool> point_flags_0(pline0.size(), true); //point flags to keep corners
    for (auto& pair : sorted_segments_or_points_0)
        for (size_t j = std::ceil(pair.second.first.first); j <= std::floor(pair.second.first.second); j++) { //are corners in between insertable polylines
            point_flags_0[j] = false;
            point_count--;
        }
    point_flags_0[point_flags_0.size() - 1] = false; //ignore last

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Iterate pairs and mark skipped points ids ");
#endif

    //CGAL_Debug(std::floor(sorted_segments_or_points_0.begin()->second.first.first), sorted_segments_or_points_0.begin()->second.first.second);

    std::vector<bool> point_flags_1(pline0.size(), true); //point flags to keep corners
    for (auto& pair : sorted_segments_or_points_1)
        for (size_t j = std::ceil(pair.second.first.first); j <= std::floor(pair.second.first.second); j++) //are corners in between insertable polylines
            point_flags_1[j] = false;
    point_flags_1[point_flags_1.size() - 1] = false; //ignore last

    //Skip first in case there is a cut on the corner
    if (std::floor(sorted_segments_or_points_0.begin()->second.first.first) < 1 && std::ceil(sorted_segments_or_points_0.begin()->second.first.second) == (pline0.size() - 1)) {
        std::reverse(sorted_segments_or_points_0.begin()->second.second.begin(), sorted_segments_or_points_0.begin()->second.second.end());
        point_flags_0[0] = false;
        point_count--;
    }

    if (std::floor(sorted_segments_or_points_1.begin()->second.first.first) < 1 && std::ceil(sorted_segments_or_points_1.begin()->second.first.second) == (pline1.size() - 1)) {
        std::reverse(sorted_segments_or_points_1.begin()->second.second.begin(), sorted_segments_or_points_1.begin()->second.second.end());
        point_flags_1[0] = false;
    }

    ///////////////////////////////////////////////////////////////////////////////
    //Add polygons including points to sorted map and merge
    ///////////////////////////////////////////////////////////////////////////////

    for (size_t i = 0; i < point_flags_0.size(); i++)
        if (point_flags_0[i])
            sorted_segments_or_points_0.insert(std::make_pair(i, std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>(i, i), CGAL_Polyline{ pline0[i] }}));

    //

    for (size_t i = 0; i < point_flags_1.size(); i++)
        if (point_flags_1[i])
            sorted_segments_or_points_1.insert(std::make_pair(i, std::pair<std::pair<double, double>, CGAL_Polyline>{std::pair<double, double>(i, i), CGAL_Polyline{ pline1[i] }}));

    CGAL_Polyline pline0_new; //reserve optimize
    CGAL_Polyline pline1_new; //reserve optimize
    pline0_new.reserve(point_count);
    pline1_new.reserve(point_count);

    //int counter = 0;
    for (auto const& x : sorted_segments_or_points_0) {
        for (size_t j = 0; j < x.second.second.size(); j++)
            pline0_new.emplace_back(x.second.second[j]);
        //if(counter==1)
        //break;
        //counter++;
    }

    for (auto const& x : sorted_segments_or_points_1) {
        for (size_t j = 0; j < x.second.second.size(); j++)
            pline1_new.emplace_back(x.second.second[j]);
    }

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Add polygons including points to sorted map and merge ");
#endif
    ///////////////////////////////////////////////////////////////////////////////
    //Close
    ///////////////////////////////////////////////////////////////////////////////
    if (lastID == this->polylines[0].size() && last_segment0_start.squared_length() > GlobalToleranceSquare) {
        IK::Point_3 p0;
        IK::Point_3 p1;
        if (cgal_intersection_util::LineLine3D(last_segment0_start, last_segment0, p0) && cgal_intersection_util::LineLine3D(last_segment1_start, last_segment1, p1)) {
            pline0_new[0] = p0;
            pline1_new[0] = p1;
        }
    }

    pline0_new.emplace_back(pline0_new.front());
    pline1_new.emplace_back(pline1_new.front());
    CGAL_Debug(pline0_new);
    CGAL_Debug(pline1_new);

    //#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
    //    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Close ");
    //#endif

        ///////////////////////////////////////////////////////////////////////////////
        //Add loose elements from top and bottom outlines also
        //Also check the winding
        ///////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < 2; i++) { //iterate top only
        for (size_t j = 0; j < j_mf[i].size(); j++) {
            if (joints[std::get<0>(j_mf[i][j])].name == "")
                continue;

            if (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() == 0) continue;
            if (joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).size() == 0)continue;

            //Check hole position
            bool flag =
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).back()[0]) >
                CGAL::squared_distance(planes[0].projection(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]), joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false).back()[0]);

            if (flag)
                joints[std::get<0>(j_mf[i][j])].reverse(std::get<1>(j_mf[i][j]));

            for (int k = 0; k < joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true).size() - 1; k++) {
                //Orient to 2D and check the winding
                bool is_clockwise = cgal_polyline_util::is_clockwise(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k], planes[0]);

                if (!is_clockwise) {
                    std::reverse(
                        std::begin(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]),
                        std::end(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]));

                    std::reverse(
                        std::begin(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]),
                        std::end(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]));
                }

                output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), true)[k]);
                output[this->id].emplace_back(joints[std::get<0>(j_mf[i][j])](std::get<1>(j_mf[i][j]), false)[k]);
            }
        }
    }

#ifdef DEBUG_JOINERY_SOLVER_ELEMENT
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %i %i %i %i ", __FILE__, __FUNCTION__, __LINE__, "Add elements  ", pline0_new.size(), pline1_new.size(), this->id, output.size());
#endif
    ///////////////////////////////////////////////////////////////////////////////
    //Output
    ///////////////////////////////////////////////////////////////////////////////
    if (output.size() > this->id) { //else the input is bad
        output[this->id].emplace_back(pline0_new);
        output[this->id].emplace_back(pline1_new);
    }
}
