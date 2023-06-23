
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "wood_main.h"

#pragma region GET ELEMENTS

namespace wood_main
{

    void get_elements(
        std::vector<CGAL_Polyline> &pp,
        std::vector<std::vector<IK::Vector_3>> &insertion_vectors,
        std::vector<std::vector<int>> &JOINTS_TYPES,
        std::vector<wood::element> &elements)
    {
        int n = (int)(pp.size() * 0.5);
        // elements = std::vector<wood::element>(n);
        elements.reserve(n);

        int count = 0;
        for (int i = 0; i < pp.size(); i += 2)
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Safety Check
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (pp[i].size() != pp[i + 1].size())
                continue;
            if (CGAL::squared_distance(pp[i][0], pp[i + 1][0]) < wood_globals::DISTANCE_SQUARED)
                continue;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Empty Element
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // elements[id].id = i * 0.5;
            elements.emplace_back((int)(i * 0.5)); // Create empty wood::element just with id

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Get BoundingBox - AABB
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create copy of a polyline and transform points
            // CGAL_Polyline twoPolylines;
            // twoPolylines.resize(pp[i].size() + pp[i + 1].size());
            // std::copy(pp[i].begin(), pp[i].end(), twoPolylines.begin());
            // std::copy(pp[i + 1].begin(), pp[i + 1].end(), twoPolylines.begin() + pp[i].size());

            CGAL_Polyline twoPolylines;
            twoPolylines.reserve(pp[i].size() + pp[i + 1].size());

            for (auto &p : pp[i])
                twoPolylines.emplace_back(p);

            for (auto &p : pp[i + 1])
                twoPolylines.emplace_back(p);

            CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

            CGAL_Polyline AABB_Min_Max = {
                IK::Point_3(AABB.xmin() - 1 * wood_globals::DISTANCE, AABB.ymin() - 1 * wood_globals::DISTANCE, AABB.zmin() - 1 * wood_globals::DISTANCE),
                IK::Point_3(AABB.xmax() + 1 * wood_globals::DISTANCE, AABB.ymax() + 1 * wood_globals::DISTANCE, AABB.zmax() + 1 * wood_globals::DISTANCE),
            };

            AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

            elements[count].aabb = AABB;

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Get Object Oriented BoundingBox - OOB -> transform to XY and back to 3D
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Create Planes
            IK::Vector_3 planeAxes[4];
            cgal_polyline_util::get_average_plane(pp[i], planeAxes);

            // Create Transformation
            CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::plane_to_xy(planeAxes[0], planeAxes[1], planeAxes[2], planeAxes[3]);
            CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

            // transform the merged polyline to xy and compute xyBounding Box

            for (auto it = twoPolylines.begin(); it != twoPolylines.end(); ++it)
            {
                *it = it->transform(xform_toXY);
                // printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
            }

            // Compute bounding box, get center point, and construct 5 size vector, where 5th dimension is half site,  then transform box back to 3D by an inverse matrix
            CGAL::Bbox_3 AABBXY = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
            double scale = 10;
            IK::Vector_3 box[5] = {
                IK::Vector_3(
                    (AABBXY.xmin() + AABBXY.xmax()) * 0.5,
                    (AABBXY.ymin() + AABBXY.ymax()) * 0.5,
                    (AABBXY.zmin() + AABBXY.zmax()) * 0.5),
                IK::Vector_3(1, 0, 0),
                IK::Vector_3(0, 1, 0),
                IK::Vector_3(0, 0, 1),
                IK::Vector_3(
                    fabs((1.0 + wood_globals::DISTANCE * 1) * (AABBXY.xmax() - AABBXY.xmin()) * 0.5), // 0.00001
                    fabs((1.0 + wood_globals::DISTANCE * 1) * (AABBXY.ymax() - AABBXY.ymin()) * 0.5),
                    fabs((1.0 + wood_globals::DISTANCE * 1) * (AABBXY.zmax() - AABBXY.zmin()) * 0.5))};
            // CGAL_Debug(1.0 + wood_globals::DISTANCE * 0.00001);

            cgal_box_util::transform_plane_as_vector_array(box, xform_toXY_Inv);
            cgal_box_util::assign(box, elements[(int)(i * 0.5)].oob, 5);

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Check orientation of polylines and reverse if needed
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            bool reverse_poylines = twoPolylines.back().z() > 0;
            if (reverse_poylines)
            {
                std::reverse(pp[i].begin(), pp[i].end());
                std::reverse(pp[i + 1].begin(), pp[i + 1].end());
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Get Side Polylines and Planes
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            elements[count].polylines = std::vector<CGAL_Polyline>(1 + pp[i].size());
            elements[count].polylines[0] = pp[i];
            elements[count].polylines[1] = pp[i + 1];

            elements[count].planes = std::vector<IK::Plane_3>(1 + pp[i].size());

            // IK::Point_3 origin = cgal_polyline_util::center(pp[i]);
            IK::Vector_3 normal;
            cgal_vector_util::average_normal(pp[i], normal);
            elements[count].planes[0] = IK::Plane_3(cgal_polyline_util::center(pp[i]), normal);
            elements[count].planes[1] = IK::Plane_3(cgal_polyline_util::center(pp[i + 1]), -normal);
            elements[count].thickness = std::sqrt(CGAL::squared_distance(pp[i][0], elements[count].planes[1].projection(pp[i][0])));
            // CGAL_Debug(elements[count].thickness);

            for (int j = 0; j < pp[i].size() - 1; j++)
            {
                elements[count].planes[2 + j] = IK::Plane_3(pp[i][j + 1], pp[i][j], pp[i + 1][j + 1]);
                elements[count].polylines[2 + j] = {pp[i][j], pp[i][j + 1], pp[i + 1][j + 1], pp[i + 1][j], pp[i][j]};
            }

            // for (auto& k : elements[count].polylines) {
            //     CGAL_Debug(false);
            //     for (auto& kk : k)
            //         CGAL_Debug(kk);
            // }

            // Edge initialization, total number of edge all sides + top,bottom  + undefined not lying on face for beams
            elements[count].j_mf = std::vector<std::vector<std::tuple<int, bool, double>>>((pp[i].size() - 1) + 2 + 1); //(side id, false, parameter on edge)

            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // User given properties, Other properties such as insertion vectors or wood::joint tapes
            // be aware of "-1" in " elements[count].j_mf.size() - 1" | custom wood::joint type is skipped
            //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (insertion_vectors.size() != 0)
            {
                if (insertion_vectors.size() == pp.size() * 0.5)
                    if (insertion_vectors[count].size() == elements[count].j_mf.size() - 1)
                    {
                        elements[count].edge_vectors = insertion_vectors[count];
                        // This was very nasty bug, because polylines are reverse based on orientation fix | also only shift +2 must be reversed (edges, skip top and bottom)
                        if (reverse_poylines)
                            std::reverse(elements[count].edge_vectors.begin() + 2, elements[count].edge_vectors.end());
                    }
                    else
                        std::cout << "ERROR - wood_main.cpp -> get_elements -> insertion_vectors[count].size() == elements[count].j_mf.size()-1, currently is: " << insertion_vectors[count].size() << " must be: " << elements[count].j_mf.size() - 1 << "\n";
                else
                    std::cout << "ERROR - wood_main.cpp -> get_elements -> insertion_vectors.size() != pp.size() * 0.5, currently is: " << insertion_vectors.size() << " must be: " << 0.5 * pp.size() << "\n";
            }

            if (JOINTS_TYPES.size() != 0)
            {
                if (JOINTS_TYPES.size() == pp.size() * 0.5)
                    if (JOINTS_TYPES[count].size() == elements[count].j_mf.size() - 1)
                    {
                        elements[count].JOINTS_TYPES = JOINTS_TYPES[count];
                        // This was very nasty bug, because polylines are reverse based on orientation fix | also only shift +2 must be reversed (edges, skip top and bottom)
                        if (reverse_poylines)
                            std::reverse(elements[count].JOINTS_TYPES.begin() + 2, elements[count].JOINTS_TYPES.end());
                    }
                    else
                        std::cout << "ERROR - wood_main.cpp -> get_elements -> JOINTS_TYPES[count].size() == pp.size() * 0.5, currently is: " << JOINTS_TYPES[count].size() << " must be: " << 0.5 * pp.size() << "\n";
                else
                    std::cout << "ERROR - wood_main.cpp -> get_elements -> JOINTS_TYPES.size() == pp.size() * 0.5, currently is: " << JOINTS_TYPES.size() << " must be: " << 0.5 * pp.size() << "\n";
            }
            count++;
        }
    }

#pragma endregion

#pragma region SEARCH_LOCAL_METHODS

    bool border_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Vector_3> &insertion_vectors0,
        int f,
        double &thickness,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB)
    {

        size_t extension_variables_count = size_t(std::floor ( wood_globals::JOINT_VOLUME_EXTENSION.size() / 3.0 )-1);
        size_t extension_id = extension_variables_count == 0 ? 0 : std::min(joint_id,extension_variables_count)*3;
        

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // The wifth and offset bretween two rectangles must be changed by user given scale values
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        joint_area = Polyline0[f];

        if (f > 1)
        {

            /////////////////////////////////////////////////////////////////////////////////////////////////////
            // Get average line
            /////////////////////////////////////////////////////////////////////////////////////////////////////

            IK::Segment_3 edge_0(Polyline0[0][f - 2], Polyline0[1][f - 2]);
            IK::Segment_3 edge_1(Polyline0[0][f - 1], Polyline0[1][f - 1]);
            CGAL_Polyline projection_points = {Polyline0[0][f - 2], Polyline0[1][f - 2], Polyline0[0][f - 1], Polyline0[1][f - 1]};

            IK::Segment_3 average_line;
            cgal_polyline_util::line_line_average(edge_0, edge_1, average_line);
            // IK::Segment_3 average_line = cgal_polyline_util::LineLineMaximumOverlap(edge_0, edge_1);
            cgal_polyline_util::line_from_projected_points(average_line, projection_points, average_line);
            // viewer_polylines.emplace_back(CGAL_Polyline({ average_line[0], average_line[1] }));
            joint_lines[0] = {average_line[0], average_line[1]};
            joint_lines[1] = joint_lines[0];
            double line_length = cgal_polyline_util::polyline_length(joint_lines[0]);

            // Get average thickness
            double y_offset = 0.5; // thickness / 2.0;
            double x_offset = 0.5; // thickness / 2.0;
            double half_dist = thickness / 2.0;

            // Move points up and down using cross product
            IK::Vector_3 z_axis = Plane0[f].orthogonal_vector();
            cgal_vector_util::unitize(z_axis);

            // set x-axis
            auto x_axis = average_line.to_vector();
            cgal_vector_util::unitize(x_axis);

            // set y-axis
            auto y_axis = CGAL::cross_product(z_axis, x_axis);
            cgal_vector_util::unitize(y_axis);

            // IK::Point_3 p0 = CGAL::midpoint(average_line[0], average_line[1]) + x_axis * y_offset *0.5;
            // IK::Point_3 p1 = CGAL::midpoint(average_line[0], average_line[1]) - x_axis * y_offset *0.5;
            IK::Point_3 p0 = average_line[0];
            IK::Point_3 p1 = average_line[1];
            if (CGAL::has_smaller_distance_to_point(CGAL::midpoint(Polyline0[f][0], Polyline0[f][1]), p0, p1))
                std::swap(p0, p1);

            /////////////////////////////////////////////////////////////////////////////////////////////////////
            // wood::joint valumes
            /////////////////////////////////////////////////////////////////////////////////////////////////////
            CGAL_Polyline rect0 = {
                p0 - y_axis * half_dist * 1 - z_axis * half_dist * 0.25,
                p0 - y_axis * half_dist * 1 + z_axis * half_dist * 0.25,
                p1 - y_axis * half_dist * 1 + z_axis * half_dist * 0.25,
                p1 - y_axis * half_dist * 1 - z_axis * half_dist * 0.25,
                p0 - y_axis * half_dist * 1 - z_axis * half_dist * 0.25,
            };

            CGAL_Polyline rect1 = {
                p0 - y_axis * half_dist * -1 - z_axis * half_dist * 0.25,
                p0 - y_axis * half_dist * -1 + z_axis * half_dist * 0.25,
                p1 - y_axis * half_dist * -1 + z_axis * half_dist * 0.25,
                p1 - y_axis * half_dist * -1 - z_axis * half_dist * 0.25,
                p0 - y_axis * half_dist * -1 - z_axis * half_dist * 0.25,
            };
            joint_volumes_pairA_pairB = {rect0, rect1, rect0, rect1};

            // viewer_polylines.emplace_back(joint_lines[0]);
            // viewer_polylines.emplace_back(rect0);
            // viewer_polylines.emplace_back(rect1);

            return true;
        }

        return false;
    }

    bool plane_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<CGAL_Polyline> &Polyline1,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Plane_3> &Plane1,
        std::vector<IK::Vector_3> &insertion_vectors0,
        std::vector<IK::Vector_3> &insertion_vectors1,
        std::pair<int, int> &el_ids,
        std::pair<std::array<int, 2>, std::array<int, 2>> &face_ids,
        int &type,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB,

        double angleTol,
        bool checkOverlap

    )
    {
        face_ids.first[0] = -1;
        face_ids.first[1] = -1;
        face_ids.second[0] = -1;
        face_ids.second[1] = -1;
        type = 30;
        size_t extension_variables_count = size_t(std::floor ( wood_globals::JOINT_VOLUME_EXTENSION.size() / 3.0 )-1);
        size_t extension_id = extension_variables_count == 0 ? 0 : std::min(joint_id,extension_variables_count)*3;


        //////////////////////////////////////////////////////////////////////////////
        // Check
        // 1. Vector angles if they are not close to parallel
        // 2. Are polylines overlapping (usually skipped within first except if structure is planar) 3. Check overlay between center lines
        //////////////////////////////////////////////////////////////////////////////

        double angle = 90.0 - fabs(CGAL::approximate_angle(Plane0[0].orthogonal_vector(), Plane1[0].orthogonal_vector()) - 90);

        if (angle < angleTol)
            return false;

        // CGAL_Debug(angle);

        // if (checkOverlap) if (Polylines0Center.plines[0].ToNurbsCurve().CurvesOverlap(Polylines1Center.plines[0].ToNurbsCurve())) return; //Happens very rarely if elements are planar

        // Line centerIntersectionLine = Line.Unset;
        // Line centerIntersectionLineMax = Line.Unset;
        IK::Segment_3 centerIntersectionLine;
        IK::Segment_3 centerIntersectionLineMax;

        //
        //    if (!cgal_polyline_util::plane_polyline(Polyline0[0], Polyline1[0], Plane0[0], Plane1[0], centerIntersectionLine, centerIntersectionLineMax)) {
        ///*        CGAL_Debug(centerIntersectionLine[0],true);
        //        CGAL_Debug(centerIntersectionLine[1],true)*/;
        //        //jointLine = { centerIntersectionLine[0],centerIntersectionLine[1] };
        //        //jointArea = { centerIntersectionLine[0],centerIntersectionLine[1] };
        //        return false;
        //    }

        // return true;

        //////////////////////////////////////////////////////////////////////////////
        // Inputs for intersection
        //////////////////////////////////////////////////////////////////////////////
        auto cx0 = &Polyline0[0];
        auto cx1 = &Polyline0[1];
        auto cy0 = &Polyline1[0];
        auto cy1 = &Polyline1[1];

        auto px0 = &Plane0[0];
        auto px1 = &Plane0[1];
        auto py0 = &Plane1[0];
        auto py1 = &Plane1[1];

        ////////////////////////////////////////////////////////////////////////////////
        ////Intersection results -> 8 points -> 4 lines
        ////////////////////////////////////////////////////////////////////////////////

        // IK::Segment_3 cx0_py0__cy0_px0_Max;
        // IK::Segment_3 cx0_py1__cy1_px0_Max;
        // IK::Segment_3 cx1_py0__cy0_px1_Max;
        // IK::Segment_3 cx1_py1__cy1_px1_Max;

        //////////////////////////////////////////////////////////////////////////////
        // Perform intersection
        //////////////////////////////////////////////////////////////////////////////
        // printf("\n_");
        IK::Segment_3 cx0_py0__cy0_px0;
        std::pair<int, int> edge_pair_e0_0__e1_0; //
        // std::cout << "a \n";
        if (!cgal_intersection_util::polyline_plane_cross_joint(*cx0, *cy0, *px0, *py0, cx0_py0__cy0_px0, edge_pair_e0_0__e1_0))
            return false; //, cx0_py0__cy0_px0_Max
        // printf("A");
        // std::cout << "b \n";
        IK::Segment_3 cx0_py1__cy1_px0;
        std::pair<int, int> edge_pair_e0_0__e1_1;
        if (!cgal_intersection_util::polyline_plane_cross_joint(*cx0, *cy1, *px0, *py1, cx0_py1__cy1_px0, edge_pair_e0_0__e1_1))
            return false; //, cx0_py1__cy1_px0_Max

        // std::cout << "c \n";
        IK::Segment_3 cx1_py0__cy0_px1;
        std::pair<int, int> edge_pair_e0_1__e1_0;
        if (!cgal_intersection_util::polyline_plane_cross_joint(*cx1, *cy0, *px1, *py0, cx1_py0__cy0_px1, edge_pair_e0_1__e1_0))
            return false; //, cx1_py0__cy0_px1_Max

        // std::cout << "d \n";
        IK::Segment_3 cx1_py1__cy1_px1;
        std::pair<int, int> edge_pair_e0_1__e1_1;
        if (!cgal_intersection_util::polyline_plane_cross_joint(*cx1, *cy1, *px1, *py1, cx1_py1__cy1_px1, edge_pair_e0_1__e1_1))
            return false; //,cx1_py1__cy1_px1_Max
        // std::cout << "e \n";
        face_ids.first[0] = edge_pair_e0_0__e1_0.first + 2;
        face_ids.second[0] = edge_pair_e0_0__e1_0.second + 2;
        face_ids.first[1] = edge_pair_e0_1__e1_1.first + 2;
        face_ids.second[1] = edge_pair_e0_1__e1_1.second + 2;
        //	std::ofstream myfile;
        //	myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output3.txt");
        //	myfile << e0_0;
        //	myfile << e1_0;
        //	myfile << e0_1;
        //	myfile << e1_1;
        //
        //
        // myfile.close();
        // e0_0 = -1;
        // e1_0 = -1;
        // e0_1 = -1;
        // e1_1 = -1;
        // CGAL_Debug(e0_0, e0_1, e1_0, e1_1);

        // e0_0 = edge_pair_e0_0__e1_1.first;
        // e1_0 = edge_pair_e0_1__e1_0.second;
        // e0_1 = edge_pair_e0_1__e1_0.first;
        // e1_1 = edge_pair_e0_0__e1_1.second;
        // CGAL_Debug(e0_0, e0_1, e1_0, e1_1);
        // printf("D");
        // printf("_\n");

        //////////////////////////////////////////////////////////////////////////////
        // Sort Lines
        //////////////////////////////////////////////////////////////////////////////
        if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx0_py1__cy1_px0.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx0_py1__cy1_px0.to_vector()))
            cx0_py1__cy1_px0 = cx0_py1__cy1_px0.opposite();
        if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py0__cy0_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py0__cy0_px1.to_vector()))
            cx1_py0__cy0_px1 = cx1_py0__cy0_px1.opposite();
        if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py1__cy1_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py1__cy1_px1.to_vector()))
            cx1_py1__cy1_px1 = cx1_py1__cy1_px1.opposite();

        //////////////////////////////////////////////////////////////////////////////
        // Define wood::joint area from lines
        //////////////////////////////////////////////////////////////////////////////
        joint_volumes_pairA_pairB[0] = {cx0_py0__cy0_px0[0], cx0_py1__cy1_px0[0], cx1_py1__cy1_px1[0], cx1_py0__cy0_px1[0], cx0_py0__cy0_px0[0]};
        joint_volumes_pairA_pairB[1] = {cx0_py0__cy0_px0[1], cx0_py1__cy1_px0[1], cx1_py1__cy1_px1[1], cx1_py0__cy0_px1[1], cx0_py0__cy0_px0[1]};

        //////////////////////////////////////////////////////////////////////////////
        // Find shortest and longest curve based on closest point
        //-----*-----*--
        //*--------------*
        //////////////////////////////////////////////////////////////////////////////
        IK::Segment_3 c;
        cgal_polyline_util::get_middle_line(cx0_py1__cy1_px0, cx1_py0__cy0_px1, c);
        cgal_polyline_util::scale_line(c, 10);

        // Find Max gap
        double cpt0[4];
        cgal_polyline_util::closest_point_to(cx0_py0__cy0_px0[0], c, cpt0[0]);
        cgal_polyline_util::closest_point_to(cx0_py1__cy1_px0[0], c, cpt0[1]);
        cgal_polyline_util::closest_point_to(cx1_py0__cy0_px1[0], c, cpt0[2]);
        cgal_polyline_util::closest_point_to(cx1_py1__cy1_px1[0], c, cpt0[3]);
        std::sort(cpt0, cpt0 + 4);

        double cpt1[4];
        cgal_polyline_util::closest_point_to(cx0_py0__cy0_px0[1], c, cpt1[0]);
        cgal_polyline_util::closest_point_to(cx0_py1__cy1_px0[1], c, cpt1[1]);
        cgal_polyline_util::closest_point_to(cx1_py0__cy0_px1[1], c, cpt1[2]);
        cgal_polyline_util::closest_point_to(cx1_py1__cy1_px1[1], c, cpt1[3]);
        std::sort(cpt1, cpt1 + 4);

        double cpt[8] = {cpt0[0], cpt0[1], cpt0[2], cpt0[3], cpt1[0], cpt1[1], cpt1[2], cpt1[3]};
        std::sort(cpt, cpt + 8);

        IK::Segment_3 lMin(cgal_polyline_util::point_at(c, cpt0[3]), cgal_polyline_util::point_at(c, cpt1[0]));
        IK::Segment_3 lMax(cgal_polyline_util::point_at(c, cpt[0]), cgal_polyline_util::point_at(c, cpt[7]));

        //////////////////////////////////////////////////////////////////////////////
        // When elements are not planar there are two options: 1) Rectangular beams are 3D overaly 2) Plate is connected to sharp corner in an angle
        //////////////////////////////////////////////////////////////////////////////
        // IK::Plane_3 jointAreaPlane0(jointArea0[0], jointArea0[1], jointArea0[2]);
        // IK::Plane_3 jointAreaPlane1(jointArea1[0], jointArea1[1], jointArea1[2]);

        // if (CGAL::squared_distance(jointAreaPlane0.projection(jointArea0[2]), jointArea0[2]) > wood_globals::DISTANCE_SQUARED * 2 ||
        //     CGAL::squared_distance(jointAreaPlane1.projection(jointArea1[2]), jointArea1[2]) > wood_globals::DISTANCE_SQUARED * 2 ) {
        //     //IK::Point_3 allPts [8] = {
        //     //    cx0_py0__cy0_px0_Max[0], cx0_py0__cy0_px0_Max[1],
        //     //    cx0_py1__cy1_px0_Max[0], cx0_py1__cy1_px0_Max[1],
        //     //    cx1_py0__cy0_px1_Max[0], cx1_py0__cy0_px1_Max[1],
        //     //    cx1_py1__cy1_px1_Max[0], cx1_py1__cy1_px1_Max[1]
        //     //};

        //    IK::Plane_3 lMaxPlane0(lMax[0], lMax.direction());
        //    IK::Plane_3 lMaxPlane1(lMax[1], lMax.direction());

        //    //LineTwoPlanes()

        //    ////Compute oriented bbox from allPts
        //    //Box box = new Box(lMax.ToPlane(true), allPts);

        //    ////Intersect line with box
        //    //Rhino.Geometry.Intersect.Intersection.LineBox(lMax, box, 0.01, out Interval lineParams);

        //    ////Remake maxLine
        //    //lMax = new Line(lMax.point_at(lineParams.T0), lMax.point_at(lineParams.T1));
        //}

        //////////////////////////////////////////////////////////////////////////////
        // RE-Define wood::joint area from lines
        //////////////////////////////////////////////////////////////////////////////
        // lMin mid plane
        IK::Point_3 lMinMid = CGAL::midpoint(lMin.min(), lMin.max());
        IK::Plane_3 midPlane(lMinMid, lMin.to_vector()); // Min line mid-plane

        // Intersection lMax with midPlane
        IK::Point_3 midPlane_lMax;
        cgal_intersection_util::line_plane(lMax, midPlane, midPlane_lMax);
        // cgal_intersection_util::PlaneLineIntersection(midPlane, lMax, midPlane_lMax);

        // Get max distance from middle point of min line
        int maxID = CGAL::squared_distance(lMax[0], midPlane_lMax) > CGAL::squared_distance(lMax[1], midPlane_lMax) ? 0 : 1;
        IK::Vector_3 v = maxID == 1 ? lMax[1] - midPlane_lMax : -(lMax[0] - midPlane_lMax);

        // exten only when user gives positive values
        
        if (wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id] > 0)
        {
            double length = cgal_vector_util::length(v.hx(), v.hy(), v.hz());
            double target_length = length + wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id];
            v *= (target_length / length);
        }

        // Align v direction in comparison to orignal 4 lines if possible
        // IK::Point_3 origin(0,0,0);
        // IK::Vector_3 v_copy = v;
        // IK::Vector_3 v_align = cx0_py0__cy0_px0.to_vector();
        // cgal_vector_util::unitize(v_copy);
        // cgal_vector_util::unitize(v_align);

        // if (CGAL::squared_distance(origin + v_align, origin + v_copy) > CGAL::squared_distance(origin - v_align, origin + v_copy))
        // v *= -1;

        // intersection mid plane with four lines and move it in both directions
        // CGAL_Polyline joint_area;
        cgal_intersection_util::plane_4lines(midPlane, cx0_py0__cy0_px0, cx0_py1__cy1_px0, cx1_py1__cy1_px1, cx1_py0__cy0_px1, joint_area);

        //////////////////////////////////////////////////////////////////////////////
        // Move rectangles in opposite direction
        //////////////////////////////////////////////////////////////////////////////
        // std::reverse(joint_area.begin(), joint_area.end());
        // std::rotate(joint_area.begin(), joint_area.begin() + 1, joint_area.end());
        // std::vector<IK::Point_3> pts0 = { joint_area[0] ,joint_area[1] , joint_area[2] , joint_area[3]   };

        ////IK::Point_3 pts1[4] = { joint_area[0] - v, joint_area[1] - v, joint_area[2] - v, joint_area[3] - v };
        // joint_area = pts0;
        // joint_area.push_back(joint_area[0]);

        // for (int i = 0; i < 4; i++) {
        //	if (CGAL::squared_distance(joint_area_oriented[0], px0->projection(joint_area_oriented[0])) > wood_globals::DISTANCE_SQUARED)
        //		std::rotate(joint_area_oriented.begin(), joint_area_oriented.begin() + 1, joint_area_oriented.end());
        //	else {
        //		CGAL_Debug(9999);
        //		break;

        //	}
        //}

        // if (CGAL::squared_distance(joint_area[1], px1->projection(joint_area_oriented[1])) > wood_globals::DISTANCE_SQUARED)
        //	std::reverse(joint_area_oriented.begin(), joint_area_oriented.end());
        // joint_area_oriented.emplace_back(joint_area_oriented[0]);
        // joint_area = joint_area_oriented;

        ////std::rotate(joint_area_oriented.begin(), joint_area_oriented.begin() + 1, joint_area_oriented.end());

        // IK::Point_3 center = cgal_polyline_util::center(*cx0);//not center but closest point, check all and tak closest
        // if (CGAL::squared_distance(IK::Point_3(joint_area_oriented[0]) + v, center) > CGAL::squared_distance(IK::Point_3(joint_area_oriented[0]) - v, center))
        //	v *= -1;

        // v *= -1;

        joint_volumes_pairA_pairB[0] = {IK::Point_3(joint_area[0]) + v, IK::Point_3(joint_area[1]) + v, IK::Point_3(joint_area[2]) + v, IK::Point_3(joint_area[3]) + v, IK::Point_3(joint_area[4]) + v};
        joint_volumes_pairA_pairB[1] = {IK::Point_3(joint_area[0]) - v, IK::Point_3(joint_area[1]) - v, IK::Point_3(joint_area[2]) - v, IK::Point_3(joint_area[3]) - v, IK::Point_3(joint_area[4]) - v};
        // std::reverse(joint_area.begin(), joint_area.end());
        // std::reverse(joint_volumes_pairA_pairB[0].begin(), joint_volumes_pairA_pairB[0].end());
        // std::reverse(joint_volumes_pairA_pairB[1].begin(), joint_volumes_pairA_pairB[1].end());

        // joint_lines[0] = { joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[1][0] + v };
        // joint_lines[1] = { joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[0][3] };

        // jointArea0.insert(jointArea0.end(), jointArea1.begin(), jointArea1.end());

        // jointArea0 = rm.Translate(lMax.direction().Unit() * (-(1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5
        // jointArea1 = rm.Translate(lMax.direction().Unit() * ((1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5

        // does not work
        if (wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id] + wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id] > 0)
        {
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[0], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[0], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[0], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[0], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

            cgal_polyline_util::extend(joint_volumes_pairA_pairB[1], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[1], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[1], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
            cgal_polyline_util::extend(joint_volumes_pairA_pairB[1], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id], wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
        }

        // For the sake of consistency
        // joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
        // joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];

        ////////////////////////////////////////////////////////////////////////////////
        ////return plate joints
        ////////////////////////////////////////////////////////////////////////////////
        // Joint pj = new Joint(this.joints.Count, x.key, y.key, -1, -1, new List<Polyline>{ jointArea0, jointArea1 }, jointLines, new OutlineType[]{ OutlineType.Side, OutlineType.Side }, CollisionType.PlaneFace);//OutlineType
        // this.joints.Add(pj);
        // std::cout << "true\n";
        return true;
    }

    // ToDo: Currently one connection can be made with another object, but one multiple shared edges can be possible e.g. |_>-<_|
    bool face_to_face(
        const size_t& joint_id,
        std::vector<CGAL_Polyline> &Polyline0,
        std::vector<CGAL_Polyline> &Polyline1,
        std::vector<IK::Plane_3> &Plane0,
        std::vector<IK::Plane_3> &Plane1,
        std::vector<IK::Vector_3> &insertion_vectors0,
        std::vector<IK::Vector_3> &insertion_vectors1,
        std::pair<int, int> &el_ids,
        std::pair<std::array<int, 2>, std::array<int, 2>> &face_ids,
        int &type,
        CGAL_Polyline &joint_area,
        std::array<CGAL_Polyline, 2> &joint_lines,
        std::array<CGAL_Polyline, 4> &joint_volumes_pairA_pairB)
    {
        size_t extension_variables_count = size_t(std::floor ( wood_globals::JOINT_VOLUME_EXTENSION.size() / 3.0 )-1);
        size_t extension_id = extension_variables_count == 0 ? 0 : std::min(joint_id,extension_variables_count)*3;
      

#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
        printf("\nCPP face_to_face \nCPP planes %zi, %zi  \n", Plane0.size(), Plane1.size());
#endif

        for (int i = 0; i < Plane0.size(); i++)
        {
            for (int j = 0; j < Plane1.size(); j++)
            {
                // Check if polygons are co-planar
                bool coplanar = cgal_plane_util::is_coplanar(Plane0[i], Plane1[j], false); // O(n*n) +10 ms

#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                if (coplanar)
                    printf("CPP Coplanar %i\n", coplanar);
#endif

                if (coplanar)
                {

                    // Perform 2D Intersection 20 ms
                    bool include_triangles = i < 2 && j < 2;
                    bool hasIntersection = clipper_util::get_intersection_between_two_polylines(Polyline0[i], Polyline1[j], Plane0[i], joint_area, 0, include_triangles); // +20 ms 10000.0; GlobalClipperScale

#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                    printf("CPP hasIntersection %i\n", hasIntersection);
#endif

                    //////////////////////////////////////////////////////////////////////////////////////////////////
                    // Intersection lines and rectangles
                    //////////////////////////////////////////////////////////////////////////////////////////////////
                    if (hasIntersection)
                    {
                        // std::cout << "t\n";
                        face_ids.first[0] = i;  // Do not add +2, because planes are iterated
                        face_ids.second[0] = j; // Do not add +2, because planes are iterated
                        face_ids.first[1] = i;  // Do not add +2, because planes are iterated
                        face_ids.second[1] = j; // Do not add +2, because planes are iterated

                        int type0 = i > 1 ? 0 : 1;
                        int type1 = j > 1 ? 0 : 1;
                        type = type0 + type1;

                        // Get Line: Option A(0) - Side-Side i>2 && i > 2, Option B(1) - Side-Top i>2, C(2) Top-Top

                        // jointArea1 = jointArea0;//default case (Top-top)
                        IK::Segment_3 joint_line0(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
                        IK::Plane_3 averagePlane0(CGAL::midpoint(Polyline0[0][0], Polyline0[1][0]), Plane0[0].orthogonal_vector()); // center Plane
                        CGAL_Polyline joint_quads0;
                        if (i > 1)
                        { // Side-Top  or Side-Side
                          // Middle line for alignment
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                            printf("CPP i>1 \n");
#endif

                            IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline0[0][i - 2], Polyline0[1][i - 2]), CGAL::midpoint(Polyline0[0][i - 1], Polyline0[1][i - 1]));

                            // Intersect: a) clipper region, b) center plane

                            bool isLine = cgal_intersection_util::polyline_plane_to_line(joint_area, averagePlane0, alignmentSegment, joint_line0);
                            // std::cout << alignmentSegment[0];
                            // std::cout << alignmentSegment[1];

                            // std::cout << alignmentSegment[0];
                            // std::cout << alignmentSegment[1];

                            // Planes to get a quad
                            if (isLine && joint_line0.squared_length() > wood_globals::DISTANCE)
                            { //
                                bool isQuad = cgal_intersection_util::get_quad_from_line_topbottomplanes(Plane0[i], joint_line0, Plane0[0], Plane0[1], joint_quads0);
                            }
                            else
                            {
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                printf("CPP   IsLine %i joint_line0.squared_length() %f \n", isLine, joint_line0.squared_length());
#endif

                                return false;
                            }
                        }

                        // Intersect second time for the side-side connection

                        IK::Segment_3 joint_line1(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
                        IK::Plane_3 averagePlane1(CGAL::midpoint(Polyline1[0][0], Polyline1[1][0]), Plane1[0].orthogonal_vector()); // center Plane
                        CGAL_Polyline joint_quads1;

                     

                        if (j > 1)
                        { // Side-Side
                          // Middle line for alignment

#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                            printf("CPP j>1 \n");
#endif

                            IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline1[0][j - 2], Polyline1[1][j - 2]), CGAL::midpoint(Polyline1[0][j - 1], Polyline1[1][j - 1]));

                            bool isLine = cgal_intersection_util::polyline_plane_to_line(joint_area, averagePlane1, alignmentSegment, joint_line1);

                            // Planes to get a quad
                            if (isLine && joint_line1.squared_length() > wood_globals::DISTANCE)
                            { //
                                bool isQuad = cgal_intersection_util::get_quad_from_line_topbottomplanes(Plane1[j], joint_line1, Plane1[0], Plane1[1], joint_quads1);
                            }
                            else
                            {
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                printf("CPP   IsLine %i joint_line1.squared_length() %f \n", isLine, joint_line1.squared_length());
#endif
                                return false;
                                continue;
                            }
                        }

                        ////////////////////////////////////////////////////////////////////////////////
                        // extend wood::joint line, for plates it is negative, for beam positive e.g. wood_globals::JOINT_VOLUME_EXTENSION[2] = -20;
                        // check the limit so that the lines would not be 0 or inverse
                        ////////////////////////////////////////////////////////////////////////////////

                        double JOINT_LINE_EXTENSION_limit = (wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id] * 2) * (wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id] * 2);

                        double joint_line0_squared_length = joint_line0.squared_length();
                        double joint_line1_squared_length = joint_line1.squared_length();
                        double LIMIT_MIN_JOINT_LENGTH_squared = std::pow(wood_globals::LIMIT_MIN_JOINT_LENGTH, 2);
                        if (joint_line0.squared_length() > 0.001)
                            if (JOINT_LINE_EXTENSION_limit > joint_line0.squared_length() - LIMIT_MIN_JOINT_LENGTH_squared)
                                return false;

                        if (joint_line1.squared_length() > 0.001)
                            if (JOINT_LINE_EXTENSION_limit > joint_line1.squared_length() - LIMIT_MIN_JOINT_LENGTH_squared)
                                return false;

                        cgal_polyline_util::extend_equally(joint_line0, wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id]);
                        cgal_polyline_util::extend_equally(joint_line1, wood_globals::JOINT_VOLUME_EXTENSION[2+extension_id]);

                        ////////////////////////////////////////////////////////////////////////////////
                        // ToDo set edge direction - Check Insertion angle if edge axis is assigned
                        // Applies for both elements
                        ////////////////////////////////////////////////////////////////////////////////
                        IK::Vector_3 dir(0, 0, 0);
                        bool dirSet = false;
                        if (insertion_vectors0.size() > 0)
                        {
                            //////Take priority for male
                            dir = i > j ? insertion_vectors0[i] : insertion_vectors1[j];
                            dirSet = (std::abs(dir.hx()) + std::abs(dir.hy()) + std::abs(dir.hz())) > 0.01;
                        }

                        //////////////////////////////////////////////////////////////////////////////////////////////////
                        // Identify connection volumes
                        //////////////////////////////////////////////////////////////////////////////////////////////////
                        // CGAL_Debug(type);
                        if (type == 0)
                        { // side-side
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                            printf("Type0");
#endif
                            //printf("Type0");
                            joint_lines[0] = {joint_line0[0], joint_line0[1]};
                            joint_lines[1] = {joint_line1[0], joint_line1[1]};

                            // return true;

                            ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            // Elements are rotated
                            ///////////////////////////////////////////////////§/////////////////////////////////////////////////////////////////
                            // CGAL_Debug(cgal_vector_util::Distance(jointLine0[0], jointLine0[1]));
                            // CGAL_Debug(cgal_vector_util::Distance(jointLine1[0], jointLine1[1]));

                            auto v0 = joint_line0[0] - joint_line0[1];
                            auto v1 = joint_line1[0] - joint_line1[1];

                            //the rotation is checked between the line vectors
                            //std::cout<< "Side-to-side \n";
                            //std::cout<<(cgal_vector_util::is_parallel_to(v0, v1) ) << "\n";
                            if ( (cgal_vector_util::is_parallel_to(v0, v1) == 0 || wood_globals::FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ALL_TREATED_AS_ROTATED) )
                            { 
                                //std::cout<<cgal_vector_util::is_parallel_to(v0, v1) <<"\n" << v0 << "\n" << v1 << "\n";
                                
                                // wood_globals::DISTANCE
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                printf("Elements Are rotated");
#endif                          
                                //printf("Elements Are rotated \n");
                                // joint_lines[0] = { joint_line0[0], joint_line0[1] };
                                // joint_lines[1] = { joint_line1[0], joint_line1[1] };

                                ////////////////////////////////////////////////////////////////////////////////
                                // Get average intersection line
                                ////////////////////////////////////////////////////////////////////////////////
                                IK::Segment_3 average_segment = CGAL::has_smaller_distance_to_point(joint_line0[0], joint_line1[0], joint_line1[1]) ? IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[0]), CGAL::midpoint(joint_line0[1], joint_line1[1])) : IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[1]), CGAL::midpoint(joint_line0[1], joint_line1[0]));

                                ////////////////////////////////////////////////////////////////////////////////
                                // Create Plane to XY transformation matrix
                                ////////////////////////////////////////////////////////////////////////////////
                                IK::Vector_3 o(average_segment[0].hx(), average_segment[0].hy(), average_segment[0].hz());
                                IK::Vector_3 x = average_segment.to_vector();   // cgal_vector_util::unitize(x);
                                IK::Vector_3 z = Plane0[i].orthogonal_vector(); // cgal_vector_util::unitize(z);
                                IK::Vector_3 y = CGAL::cross_product(x, z);     // cgal_vector_util::unitize(y);
                                cgal_vector_util::unitize(y);

                                // Reorient axis using first wood::element orientation - Plane0 and Plane1
                                IK::Point_3 center = cgal_polyline_util::center(Polyline0[i]);
                                double thickness = std::max(
                                    std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point()))),
                                    std::sqrt(CGAL::squared_distance(Plane1[0].point(), Plane1[1].projection(Plane1[0].point()))));
                                y *= (thickness)*2;
                                IK::Segment_3 y_line(center + y, center - y);
                                cgal_intersection_util::line_two_planes(y_line, Plane0[0], Plane1[1]);
                                y = y_line[1] - y_line[0];
                                x = CGAL::cross_product(y, z);
                                // viewer_polylines.emplace_back(CGAL_Polyline{y_line[0], y_line[1]});

                                CGAL::Aff_transformation_3<IK> xform = cgal_xform_util::plane_to_xy(o, x, y, z);

                                ////////////////////////////////////////////////////////////////////////////////
                                // Decide min or max rectangle
                                ////////////////////////////////////////////////////////////////////////////////
                                CGAL_Polyline joint_area_copy;
                                bool min = true;
                                if (min)
                                    joint_area_copy = joint_area;
                                else
                                {
                                    joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline0[i]), std::end(Polyline0[i]));
                                    joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline1[j]), std::end(Polyline1[j]));
                                }

                                ////////////////////////////////////////////////////////////////////////////////
                                // 2D - Get boundary ractangle
                                ////////////////////////////////////////////////////////////////////////////////
                                cgal_polyline_util::transform(joint_area_copy, xform);
                                auto AABB = CGAL::bbox_3(joint_area_copy.begin(), joint_area_copy.end(), IK());
                                IK::Segment_3 segmentX(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmax(), AABB.ymin(), AABB.zmin()));
                                IK::Segment_3 segmentY(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmin(), AABB.ymax(), AABB.zmin()));
                                CGAL_Polyline average_rectangle = {segmentX[0], segmentX[1], segmentX[0] + segmentX.to_vector() + segmentY.to_vector(), segmentY[1]};

                                ////////////////////////////////////////////////////////////////////////////////
                                // 2D Orient to 3D
                                ////////////////////////////////////////////////////////////////////////////////
                                CGAL::Aff_transformation_3<IK> xformInv = xform.inverse();
                                cgal_polyline_util::transform(average_rectangle, xformInv);

                                ////////////////////////////////////////////////////////////////////////////////
                                // Create Joint rectangles by an offset of wood::element thickness
                                ////////////////////////////////////////////////////////////////////////////////
                                IK::Vector_3 offset_vector = z;

                                if (dirSet)
                                {
                                    offset_vector = dir;
                                    // CGAL_Debug((dir.x() + dir.y() + dir.z()));
                                }

                                cgal_vector_util::unitize(offset_vector);
                                double d0 = 0.5 * std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
                                offset_vector *= d0;

                                joint_volumes_pairA_pairB[0] = {average_rectangle[3] + offset_vector, average_rectangle[3] - offset_vector, average_rectangle[0] - offset_vector, average_rectangle[0] + offset_vector, average_rectangle[3] + offset_vector};
                                joint_volumes_pairA_pairB[1] = {average_rectangle[2] + offset_vector, average_rectangle[2] - offset_vector, average_rectangle[1] - offset_vector, average_rectangle[1] + offset_vector, average_rectangle[2] + offset_vector};

                                // extend the polygons to scale the joint if the values are not set to zero
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                                // set type for the wood_joint_lib
                                type = 13;

                                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                // Elements are parallel
                                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            }
                            else
                            {
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                printf("Elements are parallel");
#endif
                                ////////////////////////////////////////////////////////////////////////////////
                                // Get Overlap-Line // scale it down ?
                                ////////////////////////////////////////////////////////////////////////////////
                                IK::Segment_3 lJ;
                                cgal_polyline_util::line_line_overlap_average(joint_line0, joint_line1, lJ);
                                // CGAL_Debug(std::sqrt(joint_line0.squared_length()), std::sqrt(joint_line1.squared_length()), std::sqrt(lJ.squared_length()));
                                joint_lines[0] = {lJ[0], lJ[1]};
                                joint_lines[1] = joint_lines[0];

                                ////////////////////////////////////////////////////////////////////////////////
                                // Get Plane perpendicular to overlap-axis //with or without insertion vector |-----------|
                                ////////////////////////////////////////////////////////////////////////////////

                                IK::Plane_3 plEnd0 = IK::Plane_3(lJ[0], lJ.to_vector()); // averagePlane0.orthogonal_vector(), CGAL::cross_product(lJ.to_vector(), averagePlane0.orthogonal_vector()));

                                if (dirSet)
                                    // plEnd0=IK::Plane_3(lJ[0], CGAL::cross_product(dir, CGAL::cross_product(lJ.to_vector(), dir)));
                                    plEnd0 = IK::Plane_3(lJ[0], dir);
                                // CGAL_Debug(dir);

                                IK::Plane_3 plEnd1(lJ[1], plEnd0.orthogonal_vector());
                                IK::Plane_3 pl_mid(CGAL::midpoint(lJ[0], lJ[1]), plEnd0.orthogonal_vector());

                                ////////////////////////////////////////////////////////////////////////////////
                                // Get dihedral angle to a) in-plane b) out-of-plane c) not-valid sharper than 20 deg
                                // returns an approximation of the signed dihedral angle in the tetrahedron pqrs of edge pq.
                                // The sign is negative if orientation(p, q, r, s) is CGAL::NEGATIVEand positive otherwise.The angle is given in degrees.
                                ////////////////////////////////////////////////////////////////////////////////
                                IK::Point_3 centerPoint0 = averagePlane0.projection(cgal_polyline_util::center(Polyline0[0]));
                                IK::Point_3 centerPoint1 = averagePlane1.projection(cgal_polyline_util::center(Polyline1[0]));
                                auto dihedralAngle = std::abs(CGAL::approximate_dihedral_angle(lJ[0], lJ[1], centerPoint0, centerPoint1));

                                if (dihedralAngle < 20)
                                { // 160
                                    // CGAL_Debug(20);
                                    return false;
                                }
                                else if (dihedralAngle <= wood_globals::FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_DIHEDRAL_ANGLE)
                                { // OUT-OF-PLANE // && jointArea0.size()>0
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                    printf("Out-of-plane");
#endif
                                    // CGAL_Debug(150);
                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Rotate line-wood::joint 90 degrees and intersect with adjacent wood::element top and bottom planes
                                    // This is needed to check if adjacent wood::element top and bottom planes are in the same or opposite order
                                    ////////////////////////////////////////////////////////////////////////////////
                                    IK::Vector_3 connectionNormal = Plane0[i].orthogonal_vector();
                                    IK::Vector_3 lJ_normal = lJ.to_vector();
                                    IK::Vector_3 lJ_v_90 = CGAL::cross_product(lJ_normal, connectionNormal) * 0.5;
                                    IK::Line_3 lj_l_90(lJ[0], lJ_v_90);

                                    IK::Point_3 pl0_0_p;
                                    cgal_intersection_util::line_plane(lj_l_90, Plane0[0], pl0_0_p);
                                    IK::Point_3 pl1_0_p;
                                    cgal_intersection_util::line_plane(lj_l_90, Plane1[0], pl1_0_p);
                                    IK::Point_3 pl1_1_p;
                                    cgal_intersection_util::line_plane(lj_l_90, Plane1[1], pl1_1_p);

                                    IK::Plane_3 planes[4];
                                    planes[1] = Plane0[0];

                                    if (CGAL::has_larger_distance_to_point(pl0_0_p, pl1_0_p, pl1_1_p))
                                    {
                                        planes[2] = Plane1[0];
                                        planes[3] = Plane0[1];
                                        planes[0] = Plane1[1];
                                    }
                                    else
                                    {
                                        planes[2] = Plane1[1];
                                        planes[3] = Plane0[1];
                                        planes[0] = Plane1[0];
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Intersect End plane |-----------------------| with top and bottom planes
                                    ////////////////////////////////////////////////////////////////////////////////
                                    cgal_intersection_util::plane_4planes_open(plEnd0, planes, joint_volumes_pairA_pairB[0]);
                                    cgal_intersection_util::plane_4planes_open(plEnd1, planes, joint_volumes_pairA_pairB[1]);

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Check the orientation of the volume, must consistent for non-simetrical joints
                                    ////////////////////////////////////////////////////////////////////////////////
                                    if (!Plane0[i].has_on_negative_side(joint_volumes_pairA_pairB[0][1]))
                                    {
                                        std::rotate(joint_volumes_pairA_pairB[0].begin(), joint_volumes_pairA_pairB[0].begin() + 2, joint_volumes_pairA_pairB[0].end());
                                        std::rotate(joint_volumes_pairA_pairB[1].begin(), joint_volumes_pairA_pairB[1].begin() + 2, joint_volumes_pairA_pairB[1].end());
                                    }

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Reverse if female and male order
                                    // ERROR CHECK OTHER DATA SETS BEOFRE DELETING THIS: you need to find other solution than reversing the order, because use might need to preserve the order
                                    ////////////////////////////////////////////////////////////////////////////////
                                    std::reverse(joint_volumes_pairA_pairB[0].begin(), joint_volumes_pairA_pairB[0].end());
                                    std::reverse(joint_volumes_pairA_pairB[1].begin(), joint_volumes_pairA_pairB[1].end());
                                    std::rotate(joint_volumes_pairA_pairB[0].begin(), joint_volumes_pairA_pairB[0].begin() + 3, joint_volumes_pairA_pairB[0].end());
                                    std::rotate(joint_volumes_pairA_pairB[1].begin(), joint_volumes_pairA_pairB[1].begin() + 3, joint_volumes_pairA_pairB[1].end());
                                    el_ids = std::pair<int, int>(el_ids.second, el_ids.first);
                                    face_ids = std::pair<std::array<int, 2>, std::array<int, 2>>(face_ids.second, face_ids.first);
                                    std::reverse(joint_lines.begin(), joint_lines.end());

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Close rectangle
                                    ////////////////////////////////////////////////////////////////////////////////
                                    joint_volumes_pairA_pairB[0].emplace_back(joint_volumes_pairA_pairB[0][0]);
                                    joint_volumes_pairA_pairB[1].emplace_back(joint_volumes_pairA_pairB[1][0]);
                                    // extend the polygons to scale the joint if the values are not set to zero
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                                    // set type for the wood_joint_lib
                                    type = 11;
                                }
                                else
                                { // IN-PLANE
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                                    printf("In-Plane");
#endif

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Intersect current top and bottom wood::element planes, including the offseted wood::joint face planes with |......................| end planes
                                    ////////////////////////////////////////////////////////////////////////////////
                                    double d0 = 0.5 * std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
                                    IK::Plane_3 offset_plane_0 = cgal_plane_util::translate_by_normal(Plane0[i], -d0);
                                    IK::Plane_3 offset_plane_1 = cgal_plane_util::translate_by_normal(Plane0[i], d0);

                                    IK::Plane_3 loopOfPlanes0[4] = {
                                        offset_plane_0,
                                        Plane0[0],
                                        offset_plane_1,
                                        Plane0[1],
                                    };
                                    IK::Plane_3 loopOfPlanes1[4] = {
                                        offset_plane_0,
                                        Plane1[0],
                                        offset_plane_1,
                                        Plane1[1],
                                    };

                                    ////////////////////////////////////////////////////////////////////////////////
                                    // Intersect End plane |-----------------------| with top and bottom planes
                                    ////////////////////////////////////////////////////////////////////////////////
                                    cgal_intersection_util::plane_4planes(plEnd0, loopOfPlanes0, joint_volumes_pairA_pairB[0]);
                                    cgal_intersection_util::plane_4planes(plEnd1, loopOfPlanes0, joint_volumes_pairA_pairB[1]);
                                    cgal_intersection_util::plane_4planes(plEnd0, loopOfPlanes1, joint_volumes_pairA_pairB[2]);
                                    cgal_intersection_util::plane_4planes(plEnd1, loopOfPlanes1, joint_volumes_pairA_pairB[3]);

                                    // extend the polygons to scale the joint if the values are not set to zero
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[2], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[2], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[3], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[3], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[2], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[2], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[3], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                                    cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[3], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                                    // set type for the wood_joint_lib
                                    type = 12;
                                }
                            }
                        }
                        else if (type == 1)
                        { // top-side
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                            printf("CPP Top-side \n");
#endif
                            //////////////////////////////////////////////////////////////////////////////////
                            // Which wood::element is male or female?
                            //////////////////////////////////////////////////////////////////////////////////
                            bool male_or_female = i > j;

                            joint_lines[0] = male_or_female ? CGAL_Polyline({joint_line0[0], joint_line0[1]}) : CGAL_Polyline({joint_line1[0], joint_line1[1]});
                            joint_lines[1] = joint_lines[0];

                            IK::Plane_3 *plane0_0 = male_or_female ? &Plane0[0] : &Plane1[0];
                            IK::Plane_3 *plane1_0 = !male_or_female ? &Plane0[i] : &Plane1[j]; // female collision plane
                            IK::Plane_3 *plane1_1 = !male_or_female ? &Plane0[std::abs(i - 1)] : &Plane1[std::abs(j - 1)];

                            CGAL_Polyline *quad_0 = male_or_female ? &joint_quads0 : &joint_quads1; // male, female does not exist int top-side

                            //////////////////////////////////////////////////////////////////////////////////////
                            // Two possibilities: if dir is unset move it perpendicularly else move by wood::element direction
                            // This case will only work for top-side connection when elements are parallel
                            // For other cases you need to find a way to get opposite plane i.e. mesh intersection
                            //////////////////////////////////////////////////////////////////////////////////////
                            IK::Vector_3 offset_vector;
                            cgal_intersection_util::get_orthogonal_vector_between_two_plane_pairs(*plane0_0, *plane1_0, *plane1_1, offset_vector);

                            // dir = i > j ? insertion_vectors0[i]: insertion_vectors1[j];
                            // dirSet = true;
                            if (dirSet)
                            {
                                IK::Vector_3 offset_vector_;
                                // CGAL::cross_product(dir, plane0_0->orthogonal_vector())
                                bool flag = cgal_intersection_util::scale_vector_to_distance_of_2planes(dir, *plane1_0, *plane1_1, offset_vector_);
                                if (flag)
                                    offset_vector = offset_vector_;
                            }

                            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            // OUTPUT: IMPORTANT Switch if first one is not side
                            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                            if (!male_or_female)
                            {
                                // el_ids = std::pair(0, 0);

                                el_ids = std::make_pair(el_ids.second, el_ids.first);
                                face_ids = std::make_pair(std::array<int, 2>{face_ids.second[0], face_ids.second[1]}, std::array<int, 2>{face_ids.first[0], face_ids.first[1]});
                            }

                            //////////////////////////////////////////////////////////////////////////////////
                            // OUTPUT: Create Connection rectangles, IMPORTANT based on order
                            //////////////////////////////////////////////////////////////////////////////////
                            int m_id = male_or_female ? 0 : 1;
                            int f_id = male_or_female ? 1 : 0;
                            joint_volumes_pairA_pairB[m_id] = {(*quad_0)[0], (*quad_0)[1], (*quad_0)[1] + offset_vector, (*quad_0)[0] + offset_vector, (*quad_0)[0]};
                            joint_volumes_pairA_pairB[f_id] = {(*quad_0)[3], (*quad_0)[2], (*quad_0)[2] + offset_vector, (*quad_0)[3] + offset_vector, (*quad_0)[3]};

                            // extend the polygons to scale the joint if the values are not set to zero
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[m_id], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[m_id], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[f_id], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[f_id], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[m_id], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[m_id], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[f_id], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[f_id], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                            // set type for the wood_joint_lib
                            type = 20;

                            return true;
                        }
                        else
                        {
#ifdef DEBUG_wood_MAIN_LOCAL_SEARCH
                            printf("CPP Type40\n");
#endif
                            // top-to-top:
                            // compute bounding-rectangle around the "joint_area"
                            // move the bounding-rectangle up and down by the element thickness
                            // contruct joint_volumes_pairA_pairB from the rectangles and assign them
                            // std::cout << "wood_main -> top-to-top not implemented \n";

                            // this has to become a function
                            CGAL_Polyline result;
                            clipper_util::bounding_rectangle(joint_area, Plane0[i], result);

                            // assign output that are in the same position but will be moved in the next step
                            joint_volumes_pairA_pairB[0] = result;
                            joint_volumes_pairA_pairB[1] = result;

                            // get movement direction. WARNING this will fail if the insertion direction is pointing to the opposite direction from the element's plane
                            IK::Vector_3 dir0 = dirSet ? insertion_vectors0[i] : Plane0[i].orthogonal_vector(); // Plane0[i].orthogonal_vector();
                            cgal_vector_util::unitize(dir0);
                            IK::Vector_3 dir1 = -dir0;
                            dir0 *= -1;
                            dir1 *= -1;

                            // get thickness
                            int next_plane_0 = i == 0 ? 1 : 0;
                            int next_plane_1 = j == 0 ? 1 : 0;
                            double distance_0 = std::sqrt(CGAL::squared_distance(Plane0[i], Plane0[next_plane_0]));
                            double distance_1 = std::sqrt(CGAL::squared_distance(Plane1[j], Plane1[next_plane_1]));
                            dir0 *= distance_0;
                            dir1 *= distance_1;

                            // move the bounding boxes rectangles
                            for (int k = 0; k < 5; k++)
                            {
                                joint_volumes_pairA_pairB[0][k] += dir0;
                                joint_volumes_pairA_pairB[1][k] += dir1;
                            }

                            // to follow the same notation as other types, take two edge and reconstruct two rectangles
                            CGAL_Polyline joint_volumes_pairA_pairB_temp0 = {
                                joint_volumes_pairA_pairB[0][0],
                                joint_volumes_pairA_pairB[0][1],
                                joint_volumes_pairA_pairB[1][1],
                                joint_volumes_pairA_pairB[1][0],
                                joint_volumes_pairA_pairB[0][0],
                            };

                            CGAL_Polyline joint_volumes_pairA_pairB_temp1 = {
                                joint_volumes_pairA_pairB[0][3],
                                joint_volumes_pairA_pairB[0][2],
                                joint_volumes_pairA_pairB[1][2],
                                joint_volumes_pairA_pairB[1][3],
                                joint_volumes_pairA_pairB[0][3],
                            };

                            // reassign
                            joint_volumes_pairA_pairB[0] = joint_volumes_pairA_pairB_temp0;
                            joint_volumes_pairA_pairB[1] = joint_volumes_pairA_pairB_temp1;

                            // extend the polygons to scale the joint if the values are not set to zero
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 0, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 2, wood_globals::JOINT_VOLUME_EXTENSION[0+extension_id]);

                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[0], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 1, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);
                            cgal_polyline_util::extend_equally(joint_volumes_pairA_pairB[1], 3, wood_globals::JOINT_VOLUME_EXTENSION[1+extension_id]);

                            // set type for the wood_joint_lib
                            type = 40;

                            return true;
                        }
                        return true;
                    } // Has Intersection
                }     // Co-planar
            }         // for j
        }             // for i

        return false;
    }

#pragma endregion

#pragma region SEARCH_GLOBAL_METHODS

    bool pair_search(

        // Input
        std::vector<wood::element> &elements,       // real wood::element
        std::array<CGAL_Polyline, 4> &beam_volumes, // wood::joint volumes
        const int &polyline_id_0,
        const int &polyline_id_1,

        int search_type,

        // Output
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map

    )
    {
        //////////////////////////////////////////////////////////////////////////////
        // construct elements, with planes and side polylines
        //////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> input_polyline_pairs(std::begin(beam_volumes), std::end(beam_volumes));
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
        std::vector<std::vector<int>> input_JOINTS_TYPES;
        std::vector<wood::element> beam_volumes_elements;

        get_elements(input_polyline_pairs, input_insertion_vectors, input_JOINTS_TYPES, beam_volumes_elements);
        // elements = beam_volumes_elements;
        //////////////////////////////////////////////////////////////////////////////
        // search
        //////////////////////////////////////////////////////////////////////////////

        CGAL_Polyline joint_area;
        std::array<CGAL_Polyline, 2> joint_quads;
        std::array<CGAL_Polyline, 2> joint_lines;
        std::array<CGAL_Polyline, 4> joint_volumes_pairA_pairB;

        std::pair<int, int> el_ids(polyline_id_0, polyline_id_1);
        std::pair<std::array<int, 2>, std::array<int, 2>> face_ids;
        int type;

        int found_type = 0;
        switch (search_type)
        {
        case (0):

            found_type = face_to_face(
                             joints.size(),
                             beam_volumes_elements[0].polylines, beam_volumes_elements[1].polylines,
                             beam_volumes_elements[0].planes, beam_volumes_elements[1].planes,
                             beam_volumes_elements[0].edge_vectors, beam_volumes_elements[1].edge_vectors,
                             el_ids, face_ids, type,
                             joint_area, joint_lines, joint_volumes_pairA_pairB)
                             ? 1
                             : 0;

#ifdef DEBUG
            printf("CPP Found_Type %i\n", found_type);
#endif

            break;

        case (1):

            found_type = plane_to_face(
                             joints.size(),
                             beam_volumes_elements[0].polylines, beam_volumes_elements[1].polylines,
                             beam_volumes_elements[0].planes, beam_volumes_elements[1].planes,
                             beam_volumes_elements[0].edge_vectors, beam_volumes_elements[1].edge_vectors,
                             el_ids, face_ids, type,
                             joint_area, joint_lines, joint_volumes_pairA_pairB)
                             ? 2
                             : 0;

#ifdef DEBUG

            printf("CPP Found_Type %i\n", found_type);
#endif
            break;
        }

        if (found_type == 0)
            return false;

        //////////////////////////////////////////////////////////////////////////////
        // create and joints
        //////////////////////////////////////////////////////////////////////////////
        int joint_id = (int)joints.size();
        if (joint_area.size() > 0)
        {
            joints.emplace_back(
                joint_id,
                el_ids.first,
                el_ids.second,
                face_ids.first[0], face_ids.second[0], face_ids.first[1], face_ids.second[1],
                joint_area,
                joint_lines,
                joint_volumes_pairA_pairB,
                type);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Map element0-element1 to joint_id
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            joints_map.emplace(cgal_math_util::unique_from_two_int(el_ids.first, el_ids.second), joint_id);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Place wood::joint ids and male or female flags to wood::joint list
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // elements[el_ids.first].j_mf[face_ids.first[0]].push_back(std::tuple<int, bool, double>(joint_id, true, 0));
            // elements[el_ids.second].j_mf[face_ids.second[0]].push_back(std::tuple<int, bool, double>(joint_id, false, 0));
#ifdef DEBUG
            printf("CPP Number of elements %zi polyline id_0 %i polyline id_1  %i \n", elements.size(), el_ids.first, el_ids.second);
#endif
            // auto a = elements[el_ids.first];
            // auto b = elements[el_ids.second];
            elements[el_ids.first].j_mf.back().push_back(std::tuple<int, bool, double>(joint_id, true, 0));
            elements[el_ids.second].j_mf.back().push_back(std::tuple<int, bool, double>(joint_id, false, 0));
            return true;
        }

        return false;
    }

    void rtree_search(

        // Input
        std::vector<wood::element> &elements,
        int search_type,

        // Output
        std::vector<int> &result)
    {
        // std::vector<wood::joint>& joints,
        // std::unordered_map<uint64_t, int>& joints_map) {
        //////////////////////////////////////////////////////////////////////////////
        // Create RTree
        //////////////////////////////////////////////////////////////////////////////

        RTree<int, double, 3> tree;

        //////////////////////////////////////////////////////////////////////////////
        // Insert AABB
        //////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < elements.size(); i++)
        {
            double min[3] = {elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin()};
            double max[3] = {elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax()};
            tree.Insert(min, max, i);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
        //////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < elements.size(); i++)
        { // AABB.size()
            // std::vector<int> result;
            auto callback = [&result, i, &elements](int foundValue) -> bool
            {
                if (i < foundValue && cgal_box_util::get_collision(elements[i].oob, elements[foundValue].oob))
                {
                    result.emplace_back(i);
                    result.emplace_back(foundValue);
                    result.emplace_back(-1);
                    result.emplace_back(-1);
                }
                return true;
            };

            double min[3] = {elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin()};
            double max[3] = {elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax()};
            int nhits = tree.Search(min, max, callback); // callback in this method call callback above
        }
    }

    void adjacency_search(
        // Input
        std::vector<wood::element> &elements,
        int search_type,
        std::vector<int> &input_adjacency,
        // Output
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map)
    {

        //////////////////////////////////////////////////////////////////////////////
        // Split the adjacency list into two lists:
        // a) one that does not have v0=v1 and f0 = f1 (border conditions usually)
        // b) one that has valid adjacency
        //////////////////////////////////////////////////////////////////////////////
        std::vector<int> adjacency_border;
        std::vector<int> adjacency_valid;
        adjacency_border.reserve(input_adjacency.size());
        adjacency_valid.reserve(input_adjacency.size());

        int adjacency_item_count = 4; // v0, v1, f0, f1
        for (int i = 0; i < input_adjacency.size(); i += adjacency_item_count)
        {

            // Sanity check
            if (input_adjacency[i] > (elements.size() - 1) || input_adjacency[i + 1] > (elements.size() - 1))
                continue;

            if (input_adjacency[i + 0] == input_adjacency[i + 1])
            {
                for (int j = 0; j < adjacency_item_count; j++)
                {
                    adjacency_border.emplace_back(input_adjacency[i + j]);
                }
            }
            else
            {
                for (int j = 0; j < adjacency_item_count; j++)
                {
                    adjacency_valid.emplace_back(input_adjacency[i + j]);
                }
            }
        }
        // printf("\n %zi", adjacency_border.size());
        // printf("\n %zi", adjacency_valid.size());

        //////////////////////////////////////////////////////////////////////////////
        // Perform Adjacency Search in result is empty
        //////////////////////////////////////////////////////////////////////////////
        if (adjacency_valid.size() == 0)
            rtree_search(elements, search_type, adjacency_valid);

        joints.reserve(adjacency_valid.size());
        joints_map.reserve(adjacency_valid.size());

        //////////////////////////////////////////////////////////////////////////////
        // Search Contact zones
        //////////////////////////////////////////////////////////////////////////////
        int joint_id = 0;
        for (int i = 0; i < adjacency_valid.size(); i += adjacency_item_count)
        { // because v0, f0 and v1,f1 are adjacent
            // CGAL_Debug(result[i], result[i + 1]);

            // CGAL_Debug(99999999);

            CGAL_Polyline joint_area;
            // CGAL_Polyline joint_quads[2];
            std::array<CGAL_Polyline, 2> joint_lines;
            std::array<CGAL_Polyline, 4> joint_volumes_pairA_pairB;

            std::pair<int, int> el_ids(adjacency_valid[i], adjacency_valid[i + 1]);

            std::pair<std::array<int, 2>, std::array<int, 2>> face_ids;
            int type;
            // std::cout << el_ids.first << " " << el_ids.second << std::endl;
            int found_type = 0;
            switch (search_type)
            {
            case (0):
                found_type = face_to_face(
                                 joints.size(),
                                 elements[el_ids.first].polylines, elements[el_ids.second].polylines,
                                 elements[el_ids.first].planes, elements[el_ids.second].planes,
                                 elements[el_ids.first].edge_vectors, elements[el_ids.second].edge_vectors,
                                 el_ids, face_ids, type,
                                 joint_area,
                                 joint_lines,
                                 joint_volumes_pairA_pairB

                                 )
                                 ? 1
                                 : 0;

                break;

            case (1):

                found_type = plane_to_face(
                                 joints.size(),
                                 elements[adjacency_valid[i]].polylines, elements[adjacency_valid[i + 1]].polylines,
                                 elements[adjacency_valid[i]].planes, elements[adjacency_valid[i + 1]].planes,
                                 elements[adjacency_valid[i]].edge_vectors, elements[adjacency_valid[i + 1]].edge_vectors,
                                 el_ids, face_ids, type,
                                 joint_area, joint_lines, joint_volumes_pairA_pairB)
                                 ? 2
                                 : 0;

                break;

            case (2):

                bool found_type_temp = face_to_face(
                    joints.size(),
                    elements[el_ids.first].polylines, elements[el_ids.second].polylines,
                    elements[el_ids.first].planes, elements[el_ids.second].planes,
                    elements[el_ids.first].edge_vectors, elements[el_ids.second].edge_vectors,
                    el_ids, face_ids, type,
                    joint_area,
                    joint_lines,
                    joint_volumes_pairA_pairB);

                if (found_type_temp)
                {
                    found_type = 3;
                    break;
                }

                found_type = plane_to_face(
                                 joints.size(),
                                 elements[el_ids.first].polylines, elements[el_ids.second].polylines,
                                 elements[el_ids.first].planes, elements[el_ids.second].planes,
                                 elements[el_ids.first].edge_vectors, elements[el_ids.second].edge_vectors,
                                 el_ids, face_ids, type,
                                 joint_area, joint_lines, joint_volumes_pairA_pairB)
                                 ? 3
                                 : 0;
                break;
            }
            // std::cout << el_ids.first << " " << el_ids.second << std::endl;
            if (!found_type)
                continue;

            if (joint_area.size() > 0)
            {

                // int joint_id = joints.size();

                joints.emplace_back(
                    joint_id,
                    el_ids.first, el_ids.second,
                    face_ids.first[0], face_ids.second[0], face_ids.first[1], face_ids.second[1],
                    joint_area,
                    joint_lines,
                    joint_volumes_pairA_pairB,
                    type);

                joints_map.emplace(cgal_math_util::unique_from_two_int(el_ids.first, el_ids.second), joint_id);
                // std::cout << "\n joint id "
                //           << joint_id << " el_ids.first "
                //           << el_ids.first << " el_ids.second "
                //           << el_ids.second << std::endl;

                //////////////////////////////////////////////////////////////////////////////////////////////////////
                // Place wood::joint ids and male or female flags to wood::joint list
                //////////////////////////////////////////////////////////////////////////////////////////////////////
                elements[el_ids.first].j_mf[face_ids.first[0]].emplace_back(std::tuple<int, bool, double>(joint_id, true, 0));
                elements[el_ids.second].j_mf[face_ids.second[0]].emplace_back(std::tuple<int, bool, double>(joint_id, false, 0));
                joint_id++;
            }
        }

        //////////////////////////////////////////////////////////////////////////////
        // Define border if adjacency was given
        //////////////////////////////////////////////////////////////////////////////
        for (int i = 0; i < adjacency_border.size(); i += adjacency_item_count)
        {

            CGAL_Polyline joint_area;
            std::array<CGAL_Polyline, 2> joint_lines;
            std::array<CGAL_Polyline, 4> joint_volumes_pairA_pairB;

            border_to_face(
                joints.size(),
                elements[adjacency_border[i]].polylines,
                elements[adjacency_border[i]].planes,
                elements[adjacency_border[i]].edge_vectors,
                adjacency_border[i + 2],
                elements[adjacency_border[i]].thickness,
                joint_area,
                joint_lines,
                joint_volumes_pairA_pairB

            );

            joints.emplace_back(
                joint_id,
                adjacency_border[i], adjacency_border[i],
                adjacency_border[i + 2], adjacency_border[i + 2], adjacency_border[i + 2], adjacency_border[i + 2],
                joint_area,
                joint_lines,
                joint_volumes_pairA_pairB,
                60);

            // printf("\n %i %i %i %i", adjacency_border[i], adjacency_border[i], adjacency_border[i + 2], adjacency_border[i + 2]);

            joints_map.emplace(cgal_math_util::unique_from_two_int(adjacency_border[i], adjacency_border[i]), joint_id);
            // printf("\n border %d %ds ", adjacency_border[i], joint_id);

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Place wood::joint ids and male or female flags to wood::joint list
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            elements[adjacency_border[i]].j_mf[adjacency_border[i + 2]].emplace_back(std::tuple<int, bool, double>(joint_id, true, 0));
            // elements[adjacency_border[i]].j_mf[adjacency_border[i + 2]].emplace_back(std::tuple<int, bool, double>(joint_id, false,99990));

            joint_id++;
        }
        // printf("\n \n");
    }

#pragma endregion

#pragma region CUSTOM IMPLEMENTATIONS : 3 - VALENCE VIDY
    // 0. create a parametric joints whose number of tenons and chamfers could be controlled, this wood::joint must be adapted to the length
    // 1. compute two additional connection volumes, meaning creating two additional wood::joint with index(id0, id_tenon) and (id1, id_tenon)
    // 2. assign wood::joint type
    // 2.1 orient it to the connection volumes(this is probably done in wood::joint computatio step)
    // 2.2. remove male geometry and merge it with the id_tenon,
    // this has to be done in the preprocessing step before merge joints in the wood_main.cpp in the switch statements
    void three_valence_joint_addition_vidy(std::vector<std::vector<int>> &in_s0_s1_e20_e31, // 0 - side wood::element, 1 - side wood::element, 2 - connecting wood::element for 0, 3 - connecting wood::element for 1
                                           std::vector<wood::element> &elements,
                                           std::vector<wood::joint> &joints,
                                           std::unordered_map<uint64_t, int> &joints_map,
                                           bool align_joints)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // check if the user gave the input
        // check if the 3 valence number is within the range
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if (in_s0_s1_e20_e31.size() < 2)
            return;
        
        for(int i = 1; i < in_s0_s1_e20_e31.size(); i++)
        {
            auto size = elements.size()-1;

            if( in_s0_s1_e20_e31[i].size() != 4){
                std::cout << "wood_main -> three_valence_joint_addition_vidy -> four elements must be present \n";
                std::cout<< in_s0_s1_e20_e31[i].size() << "\n";
                return;
            }
            
            if(in_s0_s1_e20_e31[i][0] < 0 || in_s0_s1_e20_e31[i][1] < 0 || in_s0_s1_e20_e31[i][2] < 0 || in_s0_s1_e20_e31[i][3] < 0 ){
                std::cout<< in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << "\n";
                  std::cout << "wood_main -> three_valence_joint_addition_vidy -> id is negative \n";
                 return;

            }

            if(in_s0_s1_e20_e31[i][0] > size || in_s0_s1_e20_e31[i][1] > size|| in_s0_s1_e20_e31[i][2] > size|| in_s0_s1_e20_e31[i][3] > size ){
                std::cout << in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << " " << size << "\n";
                std::cout << "wood_main -> three_valence_joint_addition_vidy -> id is bigger than the total number of elements \n";
                return;

            }

        }
        

        



        

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // 1. compute two additional connection volumes, meaning creating two additional wood::joint with index(id0, id_tenon) and (id1, id_tenon)
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 1; i < in_s0_s1_e20_e31.size(); i++) // skip first item because it is indicator which custom method to run
        {

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // check if the two plates in the double layer system are parallel
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (in_s0_s1_e20_e31[i][2] != in_s0_s1_e20_e31[i][3])
            {
                if (
                    !cgal_plane_util::is_same_direction(elements[in_s0_s1_e20_e31[i][0]].planes[0],
                                                        elements[in_s0_s1_e20_e31[i][3]].planes[0]) ||
                    !cgal_plane_util::is_same_direction(elements[in_s0_s1_e20_e31[i][1]].planes[0],
                                                        elements[in_s0_s1_e20_e31[i][2]].planes[0]))
                {
                    std::cout << "wood_main.cpp -> three_valence_joint_addition_vidy -> planes are not parallel \n";
                    std::cout << "wood_main.cpp -> three_valence_joint_addition_vidy ->"
                              << in_s0_s1_e20_e31[i][0] << " "
                              << in_s0_s1_e20_e31[i][1] << " "
                              << in_s0_s1_e20_e31[i][2] << " "
                              << in_s0_s1_e20_e31[i][3] << "\n";
                    continue;
                }
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // get unique key, if key does not exist throw out of range exception
            //////////////////////////////////////////////////////////////////////////////////////////////////
            int id = -1;
            // int id_alignment_joint = -1;
            try
            {
                id = joints_map.at(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][0], in_s0_s1_e20_e31[i][1]));

                // id_alignment_joint = joints_map.at(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][2], in_s0_s1_e20_e31[i][3]));
                // std::cout << id << " three_valence_joint_addition_vidy \n";
            }
            catch (const std::out_of_range &oor)
            {
                printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  %s ", __FILE__, __FUNCTION__, __LINE__, "wood_main -> vidy -> out-of-range error:", oor.what());
                continue;
            }


            //////////////////////////////////////////////////////////////////////////////////////////////////
            // if plates are parallel, then it would be enough to move the wood::joint volume, without performing intersection
            //////////////////////////////////////////////////////////////////////////////////////////////////

            // find two nearst planes
            double d00 = CGAL::squared_distance(
                elements[in_s0_s1_e20_e31[i][0]].planes[0].point(),
                elements[in_s0_s1_e20_e31[i][3]].planes[0]);
            double d01 = CGAL::squared_distance(
                elements[in_s0_s1_e20_e31[i][0]].planes[0].point(),
                elements[in_s0_s1_e20_e31[i][3]].planes[1]);

            IK::Plane_3 plane00_far = d00 < d01 ? elements[in_s0_s1_e20_e31[i][3]].planes[0] : elements[in_s0_s1_e20_e31[i][3]].planes[1];
            // std::cout << "dist: " << std::sqrt(d00) << " " << std::sqrt(d01) << "\n";

            d00 = CGAL::squared_distance(
                plane00_far.point(),
                elements[in_s0_s1_e20_e31[i][0]].planes[0]);
            d01 = CGAL::squared_distance(
                plane00_far.point(),
                elements[in_s0_s1_e20_e31[i][0]].planes[1]);

            IK::Plane_3 plane01_near = d00 < d01 ? elements[in_s0_s1_e20_e31[i][0]].planes[1] : elements[in_s0_s1_e20_e31[i][0]].planes[0];
            // std::cout << "dist: " << std::sqrt(d00) << " " << std::sqrt(d01) << "\n";

            double d10 = CGAL::squared_distance(
                elements[in_s0_s1_e20_e31[i][1]].planes[0].point(),
                elements[in_s0_s1_e20_e31[i][2]].planes[0]);
            double d11 = CGAL::squared_distance(
                elements[in_s0_s1_e20_e31[i][1]].planes[0].point(),
                elements[in_s0_s1_e20_e31[i][2]].planes[1]);

            IK::Plane_3 plane10_far = d10 < d11 ? elements[in_s0_s1_e20_e31[i][2]].planes[0] : elements[in_s0_s1_e20_e31[i][2]].planes[1];
            // std::cout << "dist: " << std::sqrt(d10) << " " << std::sqrt(d11) << "\n";

            d10 = CGAL::squared_distance(
                plane10_far.point(),
                elements[in_s0_s1_e20_e31[i][1]].planes[0]);
            d11 = CGAL::squared_distance(
                plane10_far.point(),
                elements[in_s0_s1_e20_e31[i][1]].planes[1]);

            IK::Plane_3 plane11_near = d10 < d11 ? elements[in_s0_s1_e20_e31[i][1]].planes[1] : elements[in_s0_s1_e20_e31[i][1]].planes[0];
            // std::cout << "dist: " << std::sqrt(d10) << " " << std::sqrt(d11) << "\n";

            // find movement direction

            IK::Line_3 l0(joints[id].joint_volumes[0][0], joints[id].joint_volumes[0][1]);
            IK::Line_3 l1(joints[id].joint_volumes[0][1], joints[id].joint_volumes[0][2]);
            // std::cout << "dist of lines: " << std::sqrt(CGAL::squared_distance(joints[id].joint_volumes[0][0], joints[id].joint_volumes[0][1])) << "\n";
            // std::cout << "dist of lines: " << std::sqrt(CGAL::squared_distance(joints[id].joint_volumes[0][1], joints[id].joint_volumes[0][2])) << "\n";

            // double d_plane0_near = std::abs(CGAL::squared_distance(joints[id].joint_volumes[0][0], plane01_near) - CGAL::squared_distance(joints[id].joint_volumes[0][1], plane01_near));
            // double d_plane1_near = std::abs(CGAL::squared_distance(joints[id].joint_volumes[0][1], plane11_near) - CGAL::squared_distance(joints[id].joint_volumes[0][2], plane11_near));
            IK::Line_3 projection_line_0(plane01_near.projection(joints[id].joint_volumes[0][0]), plane01_near.projection(joints[id].joint_volumes[0][1]));
            IK::Line_3 projection_line_1(plane01_near.projection(joints[id].joint_volumes[0][1]), plane01_near.projection(joints[id].joint_volumes[0][2]));

            bool is_parallel_00 = cgal_vector_util::is_parallel_to(projection_line_0.to_vector(), l0.to_vector()) == 0;
            bool is_parallel_01 = cgal_vector_util::is_parallel_to(projection_line_1.to_vector(), l1.to_vector()) == 0;

            // std::cout << "is parallel: " << is_parallel_00 << " " << is_parallel_01 << "\n";

            IK::Point_3 midpoint_0 = CGAL::midpoint(joints[id].joint_volumes[0][0], joints[id].joint_volumes[0][1]);
            IK::Point_3 midpoint_1 = CGAL::midpoint(joints[id].joint_volumes[0][1], joints[id].joint_volumes[0][2]);
            IK::Point_3 midpoint_2 = CGAL::midpoint(joints[id].joint_volumes[0][2], joints[id].joint_volumes[0][3]);
            IK::Point_3 midpoint_3 = CGAL::midpoint(joints[id].joint_volumes[0][3], joints[id].joint_volumes[0][0]);

            // double d_plane0_near = std::abs(CGAL::squared_distance(joints[id].joint_volumes[0][0], plane01_near) - CGAL::squared_distance(joints[id].joint_volumes[0][1], plane01_near));
            // double d_plane1_near = std::abs(CGAL::squared_distance(joints[id].joint_volumes[0][1], plane11_near) - CGAL::squared_distance(joints[id].joint_volumes[0][2], plane11_near));

            // std::array<IK::Line_3, 2> l = d_plane0_near < d_plane1_near ? std::array<IK::Line_3, 2>{l1, l0} : std::array<IK::Line_3, 2>{l0, l1};
            std::array<IK::Line_3, 2> l = is_parallel_01 ? std::array<IK::Line_3, 2>{l1, l0} : std::array<IK::Line_3, 2>{l0, l1};

            IK::Point_3 p00;
            cgal_intersection_util::line_plane(l[0], plane00_far, p00);

            IK::Point_3 p01;
            cgal_intersection_util::line_plane(l[0], plane01_near, p01);

            IK::Point_3 p10;
            cgal_intersection_util::line_plane(l[1], plane10_far, p10);

            IK::Point_3 p11;
            cgal_intersection_util::line_plane(l[1], plane11_near, p11);

            // when only one neighbor is given
            if (in_s0_s1_e20_e31[i][2] == in_s0_s1_e20_e31[i][3])
            {
                p10 = p00;
                p11 = p01;
            }

            // std::cout << p00.hx() << " " << p00.hy() << " " << p00.hz() << "\n";
            // std::cout << p01.hx() << " " << p01.hy() << " " << p01.hz() << "\n";
            // std::cout << p10.hx() << " " << p10.hy() << " " << p10.hz() << "\n";
            // std::cout << p11.hx() << " " << p11.hy() << " " << p11.hz() << "\n";

            std::array<CGAL::Aff_transformation_3<IK>, 2> translation = std::array<CGAL::Aff_transformation_3<IK>, 2>{
                CGAL::Aff_transformation_3<IK>(CGAL::TRANSLATION, p00 - p01),
                CGAL::Aff_transformation_3<IK>(CGAL::TRANSLATION, p10 - p11)};

            std::array<IK::Vector_3, 2> translation_vectors = std::array<IK::Vector_3, 2>{
                p00 - p01,
                p10 - p11};

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Translates the wood::joint volume and lines | change the orientation of the second volume to rotate the same wood::joint
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            std::array<CGAL_Polyline, 4> joint_volumes_0 = {joints[id].joint_volumes[0], joints[id].joint_volumes[1], joints[id].joint_volumes[2], joints[id].joint_volumes[3]};
            std::array<CGAL_Polyline, 4> joint_volumes_1 = {joints[id].joint_volumes[0], joints[id].joint_volumes[1], joints[id].joint_volumes[2], joints[id].joint_volumes[3]};

            // for (int j = 0; j < 4; j++)
            // {
            //     std::cout << joint_volumes_0[j].size() << "\n";
            //     if (joint_volumes_0[j].size() != 0)
            //     {
            //         std::cout << joint_volumes_1[j].size() << "\n";
            //         for (int k = 0; k < joint_volumes_1[j].size(); k++)
            //         {
            //             std::cout << joint_volumes_1[j][k] << "\n";
            //         }
            //     }
            // }

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            // Check if translation vectors are valid
            //////////////////////////////////////////////////////////////////////////////////////////////////////
            double vector_coord_sum = translation_vectors[0].hx() + translation_vectors[0].hy() + translation_vectors[0].hz() + translation_vectors[1].hx() + translation_vectors[1].hy() + translation_vectors[1].hz();
            bool a = vector_coord_sum < -1.0e8;
            bool b = vector_coord_sum > 1.0e8;
            // std::cout << a << "\n";
            // std::cout << b << "\n";
            // std::cout << vector_coord_sum << "\n";

            if (vector_coord_sum < -1.0e8 || vector_coord_sum > 1.0e8)
            {
                std::cout << "ERROR -> wood_main.cpp -> three_valence_joint_addition_vidy " << vector_coord_sum
                          << " translation vector is huge or super small \n";
                return;
            }

            // orient volumes to translation direction for correct wood::joint orientation
            int shift = 0;
            for (int j = 0; j < 4; j++)
            {
                IK::Vector_3 v = joint_volumes_1[0][j] - joint_volumes_1[0][j + 1];
                if (cgal_vector_util::is_parallel_to(translation_vectors[1], v) == 1)
                {
                    shift = j;
                    break;
                }
            }

            // std::cout << "shift: " << shift << std::endl;
            for (int j = 0; j < joint_volumes_1.size(); j++)
                if (joint_volumes_1[j].size() == 5)
                    cgal_polyline_util::shift(joint_volumes_1[j], shift);

            std::array<CGAL_Polyline, 2> joint_lines_0 = {joints[id].joint_lines[0], joints[id].joint_lines[1]};
            std::array<CGAL_Polyline, 2> joint_lines_1 = {joints[id].joint_lines[0], joints[id].joint_lines[1]};
            for (int j = 0; j < 2; j++)
            {
                cgal_polyline_util::transform(joint_volumes_0[j], translation[0]);
                cgal_polyline_util::transform(joint_volumes_1[j], translation[1]);
                cgal_polyline_util::transform(joint_lines_0[j], translation[0]);
                cgal_polyline_util::transform(joint_lines_1[j], translation[1]);
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // if wood::joint order was reversed, reverse the neighbors here:
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // std::cout << "wood_main " <<in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << "\n";
            // std::cout << "wood_main " <<joints[id].v0 << " " << joints[id].v1 << "\n";
            if (joints[id].v0 == in_s0_s1_e20_e31[i][1])
            {
                std::swap(in_s0_s1_e20_e31[i][2], in_s0_s1_e20_e31[i][3]);
                std::swap(in_s0_s1_e20_e31[i][0], in_s0_s1_e20_e31[i][1]);
            }
            // std::cout << "wood_main " << in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << "\n";

            //////////////////////////////////////////////////////////////////////////////////////////////////////
            //  2. Add joints | Map element0-element1 to joint_id | Add wood::element indexing for display of volumes
            //////////////////////////////////////////////////////////////////////////////////////////////////////

            joints.emplace_back(
                (int)joints.size(),
                in_s0_s1_e20_e31[i][0],
                in_s0_s1_e20_e31[i][2],
                -1, -1, -1, -1,
                joints[id].joint_area,
                joint_lines_0,
                joint_volumes_0,
                joints[id].type);

            joints_map.emplace(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][0], in_s0_s1_e20_e31[i][2]), joints[joints.size() - 1].id);
            elements[in_s0_s1_e20_e31[i][0]].j_mf.back().push_back(std::tuple<int, bool, double>(joints[joints.size() - 1].id, true, 0));
            elements[in_s0_s1_e20_e31[i][2]].j_mf.back().push_back(std::tuple<int, bool, double>(joints[joints.size() - 1].id, false, 0));
            joints[joints.size() - 1].link = true;

            if (in_s0_s1_e20_e31[i][2] != in_s0_s1_e20_e31[i][3])
            {
                joints.emplace_back(
                    (int)joints.size(),
                    in_s0_s1_e20_e31[i][1],
                    in_s0_s1_e20_e31[i][3],
                    -1, -1, -1, -1,
                    joints[id].joint_area,
                    joint_lines_1,
                    joint_volumes_1,
                    joints[id].type);

                joints_map.emplace(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][1], in_s0_s1_e20_e31[i][3]), joints[joints.size() - 1].id);
                elements[in_s0_s1_e20_e31[i][1]].j_mf.back().push_back(std::tuple<int, bool, double>(joints[joints.size() - 1].id, true, 0));
                elements[in_s0_s1_e20_e31[i][3]].j_mf.back().push_back(std::tuple<int, bool, double>(joints[joints.size() - 1].id, false, 0));
                joints[joints.size() - 1].link = true;
            }

            joints[id].linked_joints = (in_s0_s1_e20_e31[i][2] != in_s0_s1_e20_e31[i][3]) ? std::vector<int>{(int)joints.size() - 2, (int)joints.size() - 1} : std::vector<int>{(int)joints.size() - 1};
            // std::cout << "linked_joints " << joints[id].linked_joints.size() << "\n";
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // perepare for linking
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        }
    }
#pragma endregion

#pragma region CUSTOM IMPLEMENTATIONS : 3 - VALENCE ANNEN
    // the function aligns to joints so that tenons would no collide and be distributed in an equal manner
    void three_valence_joint_alignment_annen(
        std::vector<std::vector<int>> &in_s0_s1_e20_e31, // 0 - side wood::element, 1 - top wood::element, 2 - side wood::element, 3 - top wood::element
        std::vector<wood::element> &elements,
        std::vector<wood::joint> &joints,
        std::unordered_map<uint64_t, int> &joints_map
        // std::vector<double>& default_parameters_for_four_types,
        // std::vector<std::vector<CGAL_Polyline>>& plines,
        // double division_distance

    )
    {
        if (in_s0_s1_e20_e31.size() < 2)
            return;
        
        for(int i = 1; i < in_s0_s1_e20_e31.size(); i++)
        {
            auto size = elements.size()-1;

            if( in_s0_s1_e20_e31[i].size() != 4){
                std::cout << "wood_main -> three_valence_joint_addition_vidy -> four elements must be present \n";
                std::cout<< in_s0_s1_e20_e31[i].size() << "\n";
                return;
            }
            
            if(in_s0_s1_e20_e31[i][0] < 0 || in_s0_s1_e20_e31[i][1] < 0 || in_s0_s1_e20_e31[i][2] < 0 || in_s0_s1_e20_e31[i][3] < 0 ){
                std::cout<< in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << "\n";
                  std::cout << "wood_main -> three_valence_joint_addition_vidy -> id is negative \n";
                 return;

            }

            if(in_s0_s1_e20_e31[i][0] > size || in_s0_s1_e20_e31[i][1] > size|| in_s0_s1_e20_e31[i][2] > size|| in_s0_s1_e20_e31[i][3] > size ){
                std::cout << in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1] << " " << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3] << " " << size << "\n";
                std::cout << "wood_main -> three_valence_joint_addition_vidy -> id is bigger than the total number of elements \n";
                return;

            }

        }
        
        //////////////////////////////////////////////////////////////////////////////////////////////////
        // For solving multiple valences (Specific case Annen), only works when only one wood::joint is possible
        // between two unique plates (wont work for plates with subdivided edges)
        //////////////////////////////////////////////////////////////////////////////////////////////////
        if (in_s0_s1_e20_e31.size() < 2)
            return;

        for (int i = 1; i < in_s0_s1_e20_e31.size(); i++)
        {
            //////////////////////////////////////////////////////////////////////////////////////////////////
            // get unique key, if key does not exist throw out of range exception
            //////////////////////////////////////////////////////////////////////////////////////////////////
            int id_0, id_1;
            try
            {
                id_0 = joints_map.at(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][0], in_s0_s1_e20_e31[i][1]));
                id_1 = joints_map.at(cgal_math_util::unique_from_two_int(in_s0_s1_e20_e31[i][2], in_s0_s1_e20_e31[i][3]));
            }
            catch (const std::out_of_range &oor)
            {
                std::cout << "\n"
                          << in_s0_s1_e20_e31[i][0] << " " << in_s0_s1_e20_e31[i][1];
                std::cout << "\n"
                          << in_s0_s1_e20_e31[i][2] << " " << in_s0_s1_e20_e31[i][3];
                printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  %s ", __FILE__, __FUNCTION__, __LINE__, "Out of Range error:", oor.what());
                continue;
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // Get overlap segment and plane within its normal
            //////////////////////////////////////////////////////////////////////////////////////////////////
            IK::Segment_3 l0(joints[id_0].joint_lines[0][0], joints[id_0].joint_lines[0][1]);

            IK::Segment_3 l1 = CGAL::has_smaller_distance_to_point(joints[id_0].joint_lines[0][0], joints[id_1].joint_lines[0][0], joints[id_1].joint_lines[0][1])
                                   ? IK::Segment_3(joints[id_1].joint_lines[0][0], joints[id_1].joint_lines[0][1])
                                   : IK::Segment_3(joints[id_1].joint_lines[0][1], joints[id_1].joint_lines[0][0]);

            IK::Segment_3 l;
            cgal_polyline_util::line_line_overlap_average(l0, l1, l);
            double thickness = elements[joints[id_0].v0].thickness;
            cgal_polyline_util::extend_line(l, -thickness, -thickness);

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // Both sides of joints must have the same alignment else there will be a collision
            //////////////////////////////////////////////////////////////////////////////////////////////////
            if (joints[id_0].joint_lines->size() > 0)
            {
                joints[id_0].joint_lines[0] = CGAL_Polyline{l[0], l[1]};
                joints[id_1].joint_lines[0] = CGAL_Polyline{l[0], l[1]};
            }

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // Construct plane from exisiting wood::joint volume edges
            //////////////////////////////////////////////////////////////////////////////////////////////////

            IK::Vector_3 cross0 = CGAL::cross_product(
                joints[id_0].joint_volumes[0][2] - joints[id_0].joint_volumes[0][1],
                joints[id_0].joint_volumes[0][0] - joints[id_0].joint_volumes[0][1]);
            IK::Vector_3 cross1 = CGAL::cross_product(
                joints[id_1].joint_volumes[0][2] - joints[id_1].joint_volumes[0][1],
                joints[id_1].joint_volumes[0][0] - joints[id_1].joint_volumes[0][1]);

            //////////////////////////////////////////////////////////////////////////////////////////////////
            // Intersect lines with planes
            //////////////////////////////////////////////////////////////////////////////////////////////////

            IK::Plane_3 plane0_0(l[0], cross0);
            IK::Plane_3 plane0_1(l[1], cross0);
            IK::Plane_3 plane1_0(l[1], cross1);
            IK::Plane_3 plane1_1(l[0], cross1);

            for (int j = 0; j < 4; j += 2)
            {
                if (joints[id_0].joint_volumes[j].size() == 0)
                    continue;

                IK::Segment_3 s0(joints[id_0].joint_volumes[j + 0][0], joints[id_0].joint_volumes[j + 1][0]);
                IK::Segment_3 s1(joints[id_0].joint_volumes[j + 0][1], joints[id_0].joint_volumes[j + 1][1]);
                IK::Segment_3 s2(joints[id_0].joint_volumes[j + 0][2], joints[id_0].joint_volumes[j + 1][2]);
                IK::Segment_3 s3(joints[id_0].joint_volumes[j + 0][3], joints[id_0].joint_volumes[j + 1][3]);

                cgal_intersection_util::plane_4lines(plane0_0, s0, s1, s2, s3, joints[id_0].joint_volumes[j]);
                cgal_intersection_util::plane_4lines(plane0_1, s0, s1, s2, s3, joints[id_0].joint_volumes[j + 1]);
            }

            for (int j = 0; j < 4; j += 2)
            {
                if (joints[id_1].joint_volumes[j].size() == 0)
                    continue;

                IK::Segment_3 s0(joints[id_1].joint_volumes[j + 0][0], joints[id_1].joint_volumes[j + 1][0]);
                IK::Segment_3 s1(joints[id_1].joint_volumes[j + 0][1], joints[id_1].joint_volumes[j + 1][1]);
                IK::Segment_3 s2(joints[id_1].joint_volumes[j + 0][2], joints[id_1].joint_volumes[j + 1][2]);
                IK::Segment_3 s3(joints[id_1].joint_volumes[j + 0][3], joints[id_1].joint_volumes[j + 1][3]);

                cgal_intersection_util::plane_4lines(plane1_0, s0, s1, s2, s3, joints[id_1].joint_volumes[j]);
                cgal_intersection_util::plane_4lines(plane1_1, s0, s1, s2, s3, joints[id_1].joint_volumes[j + 1]);
            }
        }
    }

#pragma endregion

#pragma region MAIN METHODS

    void get_connection_zones(
        std::vector<CGAL_Polyline> &input_polyline_pairs,
        std::vector<std::vector<IK::Vector_3>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,

        // output
        std::vector<std::vector<CGAL_Polyline>> &output_plines,
        std::vector<std::vector<wood_cut::cut_type>> &output_types,
        std::vector<std::vector<int>> &top_face_triangulation,

        // Global Parameters
        std::vector<double> &default_parameters_for_JOINTS_TYPES,
        std::vector<double> &scale,
        int search_type,
        int output_type,
        int triangulate

    )
    {

#ifdef DEBUG_MEASURE_TIME
        auto begin = std::chrono::high_resolution_clock::now();
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Main Properties: elements, joints, joints_map
        //////////////////////////////////////////////////////////////////////////////
        std::vector<wood::element> elements;
        auto joints = std::vector<wood::joint>();
        auto joints_map = std::unordered_map<uint64_t, int>();

#ifdef DEBUG_wood_MAIN
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Create elements from pair of polylines");
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Create elements, AABB, OBB, P, Pls, thickness
        //////////////////////////////////////////////////////////////////////////////
        try
        {
            get_elements(input_polyline_pairs, input_insertion_vectors, input_JOINTS_TYPES, elements);
        }
        catch (...)
        {
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Error in get_elements");
            return;
        }

#ifdef DEBUG_MEASURE_TIME
        auto end = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Time measured get_elements : %.3f seconds.\n", elapsed.count() * 1e-6);
        begin = std::chrono::high_resolution_clock::now();
#endif

#ifdef DEBUG_wood_MAIN
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %zi ", __FILE__, __FUNCTION__, __LINE__, "Number of elements", elements.size());
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Create joints, Perform Joint Area Search
        //////////////////////////////////////////////////////////////////////////////
        // std::vector<int> neighbours;
        try
        {
            adjacency_search(elements, search_type, input_adjacency, joints, joints_map);
        }
        catch (...)
        {
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Error in adjacency_search");
            return;
        }

#ifdef DEBUG_MEASURE_TIME
        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Time measured adjacency_search : %.3f seconds.\n", elapsed.count() * 1e-6);
        begin = std::chrono::high_resolution_clock::now();
#endif

        // printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT     input_adjacency %zi Joints %zi \n", __FILE__, __FUNCTION__, __LINE__, input_adjacency.size(), joints.size());
        // CGAL_Debug(99999);
        // for (auto& id : input_adjacency)
        //     CGAL_Debug(id);
        // CGAL_Debug(99999);

#ifdef DEBUG_wood_MAIN
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %zi  %s output_type %i", __FILE__, __FUNCTION__, __LINE__, "Joints", joints.size(), "Output_type", output_type);
#endif

        //////////////////////////////////////////////////////////////////////////////
        // custom cases, where the two valence rules breaks
        // the first vector in the in the parameter "input_three_valence_element_indices_and_instruction" must be a single number e.g. {0}, that points to the right method needed to be applied
        // {0} - Annen - 3-valence joints cases
        // {1} - Vidy - extension of the tenon mortise wood::joint to another panel
        //////////////////////////////////////////////////////////////////////////////
        if (input_three_valence_element_indices_and_instruction.size() > 0)
        {
            // sanity check give default parameter of the annen project
            if (input_three_valence_element_indices_and_instruction[0].size() != 1)
                input_three_valence_element_indices_and_instruction.insert(input_three_valence_element_indices_and_instruction.begin(), {0});

            // run the correct method
            switch (input_three_valence_element_indices_and_instruction[0][0])
            {
            case (0):
                // Annen
                three_valence_joint_alignment_annen(input_three_valence_element_indices_and_instruction, elements, joints, joints_map);
                break;
            case (1):
                // Vidy
                three_valence_joint_addition_vidy(input_three_valence_element_indices_and_instruction, elements, joints, joints_map);
                break;
            }
        }

#ifdef DEBUG_MEASURE_TIME
        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Time measured three_valence_joint_alignment_annen: %.3f seconds.\n", elapsed.count() * 1e-6);
        begin = std::chrono::high_resolution_clock::now();
#endif
        // std::cout << elements.size() << " " << joints.size() << " " << default_parameters_for_JOINTS_TYPES.size() << " " << scale.size() << "\n";

        ////////////////////////////////////////////////////////////////////////////////
        // Create and Align Joints 1. Iterate type 2. Select wood::joint based on not/given user joint_type
        ////////////////////////////////////////////////////////////////////////////////

        try
        {
            wood_joint_lib::construct_joint_by_index(elements, joints, default_parameters_for_JOINTS_TYPES, scale); // division_distance, shift,  // CGAL_Debug(99999);
        }
        catch (...)
        {
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Error in construct_joint_by_index");
            return;
        }

#ifdef DEBUG_MEASURE_TIME
        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Time measured construct_joint_by_index: %.3f seconds.\n", elapsed.count() * 1e-6);
        begin = std::chrono::high_resolution_clock::now();
#endif

#ifdef DEBUG_wood_MAIN
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, "construct_joint_by_index");
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Iterate wood::joint address
        //////////////////////////////////////////////////////////////////////////////
        output_plines = std::vector<std::vector<CGAL_Polyline>>(elements.size());
        output_types = std::vector<std::vector<wood_cut::cut_type>>(elements.size());

        try
        {

            // size_t start = 35;
            for (auto i = 0; i < elements.size(); i++) // elements.size()
            {                                          // takes 30-50 ms just to copy-paste polyline geometry

                switch (output_type)
                {
                case (0):
                    elements[i].get_joints_geometry(joints, output_plines, 0, output_types);
                    break;
                case (1):
                    elements[i].get_joints_geometry(joints, output_plines, 1, output_types);
                    break;
                case (2):
                    elements[i].get_joints_geometry(joints, output_plines, 2, output_types); // Display error
                    break;
                default:
                case (3):
                    elements[i].get_joints_geometry(joints, output_plines, 3, output_types);
                    break;
                case (4):

                    try
                    {
                        elements[i].merge_joints(joints, output_plines);
                    }
                    catch (const std::overflow_error &e)
                    {
                        (void)e;
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, "this executes if f() throws std::overflow_error(same type rule)");
                    } // this executes if f() throws std::overflow_error (same type rule)
                    catch (const std::runtime_error &e)
                    {
                        (void)e;
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, "this executes if f() throws std::underflow_error (base class rule)");
                    } // this executes if f() throws std::underflow_error (base class rule)
                    catch (const std::exception &e)
                    {
                        (void)e;
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, " this executes if f() throws std::logic_error (base class rule)");
                    } // this executes if f() throws std::logic_error (base class rule)
                    catch (...)
                    {
                        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, " // this executes if f() throws std::string or int or any other unrelated type");
                    } // this executes if f() throws std::string or int or any other unrelated type

                    break;
                }
            }
        }
        catch (...)
        {
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Error in get_joints_geometry");
            return;
        }

#ifdef DEBUG_MEASURE_TIME
        end = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);
        printf("Time measured merge joints: %.3f seconds.\n", elapsed.count() * 1e-6);
        begin = std::chrono::high_resolution_clock::now();
#endif

#ifdef DEBUG_wood_MAIN
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s  ", __FILE__, __FUNCTION__, __LINE__, "merge_joints");
#endif

        //////////////////////////////////////////////////////////////////////////////
        // Create Mesh Triangulation for top face
        //////////////////////////////////////////////////////////////////////////////
        if (triangulate)
        {
            top_face_triangulation = std::vector<std::vector<int>>(elements.size());
            for (int i = 0; i < elements.size(); i++)
            {
                int v, f;
                cgal_polyline_mesh_util::mesh_from_polylines(output_plines[i], elements[i].planes[0], top_face_triangulation[i], v, f);
            }
        }
    }

    void beam_volumes(
        std::vector<CGAL_Polyline> &polylines,
        std::vector<std::vector<double>> &polylines_segment_radii,
        std::vector<std::vector<IK::Vector_3>> &polylines_segment_direction,
        std::vector<int> &allowed_types,
        double &min_distance,
        double &volume_length,
        double &cross_or_side_to_end,
        int &flip_male,

        // output of wood::joint areas
        std::vector<std::array<int, 4>> &polyline0_id_segment0_id_polyline1_id_segment1_id,
        std::vector<std::array<IK::Point_3, 2>> &point_pairs,
        std::vector<std::array<CGAL_Polyline, 4>> &volume_pairs,
        std::vector<CGAL_Polyline> &joints_areas,
        std::vector<int> &joints_types,

        // Global Parameters and output wood::joint selection and orientation
        std::vector<double> &default_parameters_for_JOINTS_TYPES,
        std::vector<std::vector<CGAL_Polyline>> &output_plines,
        std::vector<std::vector<wood_cut::cut_type>> &output_types,
        bool compute_joints,
        double division_distance,
        double shift,
        int output_type,
        bool use_eccentricities_to_scale_joints

    )
    {

        //////////////////////////////////////////////////////////////////////////////
        // Main Properties: elements, joints, joints_map
        //////////////////////////////////////////////////////////////////////////////
        // std::cos(std::asin(1-std::min(L/R/2,R)))
        std::vector<double> scale = {1, 1, 1};

        std::vector<wood::element> elements;
        elements.reserve(polylines.size());
        for (int i = 0; i < polylines.size(); i++)
        {
            elements.emplace_back(i);
            elements.back().central_polyline = polylines[i];
            elements.back().j_mf = std::vector<std::vector<std::tuple<int, bool, double>>>(1);
        }

#ifdef DEBUG
        printf("CPP Number of elements %zi   \n", elements.size());
#endif

        std::vector<wood::joint> joints;
        std::unordered_map<uint64_t, int> joints_map;

        /////////////////////////////////////////////////////////////////////
        // Segment callback
        /////////////////////////////////////////////////////////////////////

        auto segment = [&polylines](std::size_t pid, std::size_t sid)
        {
            return IK::Segment_3(polylines[pid][sid], polylines[pid][sid + 1]);
        };

        auto segment_inflated = [&polylines, &polylines_segment_radii](std::size_t pid, std::size_t sid)
        {
            IK::Segment_3 segment(polylines[pid][sid], polylines[pid][sid + 1]);

            // IK::Vector_3 v0 = segment.to_vector();
            // segment = IK::Segment_3(segment[0] - v0*1, segment[1] + v0*1);

            double radius = polylines_segment_radii[pid][sid];

            IK::Vector_3 zaxis = segment.to_vector();
            IK::Plane_3 plane(segment[0], zaxis);
            IK::Vector_3 x_axis = plane.base1();
            IK::Vector_3 y_axis = plane.base2(); // CGAL::cross_product(x_axis, zaxis);

            cgal_box_search::unitize(x_axis);
            cgal_box_search::unitize(y_axis);
            // x_axis = cgal_vector_util::Unit(x_axis);
            // y_axis = cgal_vector_util::Unit(y_axis);
            x_axis *= radius;
            y_axis *= radius;

            std::array<IK::Point_3, 8> pts = {
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
            // CGAL_Debug(box.xmin(), box.ymin(), box.zmin());
            // CGAL_Debug(box.xmax(), box.ymax(), box.zmax());

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
        // do_interesect call_back
        /////////////////////////////////////////////////////////////////////

        std::map<uint64_t, std::tuple<double, int, int, int, int>> pair_collisions;
        std::vector<std::tuple<double, int, int, int, int>> pair_collisionslist;
        auto callback = [&segment, &min_distance, &pair_collisions, &pair_collisionslist](const Box &b1, const Box &b2)
        {
            if (b1.info().first != b2.info().first)
            {
                IK::Segment_3 s0 = segment(b1.info().first, b1.info().second);
                IK::Segment_3 s1 = segment(b2.info().first, b2.info().second);
                double distance = CGAL::squared_distance(s0, s1);

                if (distance < min_distance * min_distance)
                {
                    size_t first_0 = 0, first_1 = 0;
                    bool flipped = false;
                    uint64_t id;
                    if (b2.info().first > b1.info().first)
                    {
                        flipped = true;
                        id = (uint64_t)b2.info().first << 32 | b1.info().first;
                    }
                    else
                    {
                        flipped = false;
                        id = (uint64_t)b1.info().first << 32 | b2.info().first;
                    }

                    std::tuple<double, int, int, int, int> dist_and_segment_ids = flipped ? std::make_tuple(distance, (int)b1.info().first, (int)b1.info().second, (int)b2.info().first, (int)b2.info().second) : std::make_tuple(distance, (int)b2.info().first, (int)b2.info().second, (int)b1.info().first, (int)b1.info().second);

                    // Add elements to std::map
                    if (pair_collisions.find(id) == pair_collisions.end())
                    {
                        // not found
                        pair_collisions.insert(std::make_pair(id, dist_and_segment_ids));
                        pair_collisionslist.emplace_back(dist_and_segment_ids);
                    }
                    else if (distance < std::get<0>(pair_collisions[id]))
                    {
                        pair_collisions.insert(std::make_pair(id, dist_and_segment_ids));
                        // found and distance is smaller that before found
                        pair_collisions[id] = dist_and_segment_ids;
                        pair_collisionslist.emplace_back(dist_and_segment_ids);
                    }
                    //}
                } // check if lines closer than the given distance
            }     // check if boxes do not belong to the same group b.info().first
        };

        /////////////////////////////////////////////////////////////////////
        // self intersection
        /////////////////////////////////////////////////////////////////////
        CGAL::box_self_intersection_d(boxes.begin(), boxes.end(), callback);

        /////////////////////////////////////////////////////////////////////
        // Iterate the result, get insertion points and parameter on the lines
        /////////////////////////////////////////////////////////////////////
        polyline0_id_segment0_id_polyline1_id_segment1_id.reserve(pair_collisions.size() * 4);
        point_pairs.reserve(point_pairs.size() * 2);

        for (auto const &x : pair_collisions)
        {
            auto &v = x.second;

            ///////////////////////////////////////////////////////////////////////
            // line line intersection and type detection 0-0 0-1 1-1
            ///////////////////////////////////////////////////////////////////////
            IK::Segment_3 s0 = segment(std::get<1>(v), std::get<2>(v));
            IK::Segment_3 s1 = segment(std::get<3>(v), std::get<4>(v));

            IK::Point_3 p0;
            IK::Point_3 p1;
            IK::Vector_3 v0;
            IK::Vector_3 v1;
            IK::Vector_3 normal;
            bool type0, type1;
            bool is_parallel = false;

            bool r = cgal_box_search::line_line_intersection_with_properties(
                s0, s1,
                (int)(polylines[std::get<1>(v)].size() - 1), (int)(polylines[std::get<3>(v)].size() - 1),
                std::get<2>(v), std::get<4>(v), cross_or_side_to_end,
                p0, p1, v0, v1, normal,
                type0, type1,
                is_parallel);

            if (!r)
                continue;

            ///////////////////////////////////////////////////////////////////////
            // Check the assigned types
            ///////////////////////////////////////////////////////////////////////

            auto is_valid_type = [](int sum_of_type0_type1, int allowed_type)
            {
                switch (allowed_type)
                {
                case (0):
                    return sum_of_type0_type1 == 0;
                case (1):
                    return sum_of_type0_type1 == 1 || sum_of_type0_type1 == 2;
                case (-1):
                    return true;
                default:
                    return false;
                }
            };

            if (allowed_types.size() > 0)
            {
                if (allowed_types.size() == 1)
                {
                    if (!(is_valid_type(type0 + type1, allowed_types[0])))
                        continue;
                }
                else if (allowed_types.size() == polylines.size())
                {
                    bool allowed_0 = is_valid_type(type0 + type1, allowed_types[std::get<1>(v)]);
                    bool allowed_1 = is_valid_type(type0 + type1, allowed_types[std::get<3>(v)]);
                    if (!(allowed_0 && allowed_1))
                        continue;
                }
            }

            point_pairs.emplace_back(std::array<IK::Point_3, 2>{p0, p1});

            ///////////////////////////////////////////////////////////////////////
            // draw rectangles volumes around intersection points
            ///////////////////////////////////////////////////////////////////////
            IK::Vector_3 segment_normal0 = polylines_segment_direction.size() == 0 ? normal : polylines_segment_direction[std::get<1>(v)][std::get<2>(v)];
            IK::Vector_3 segment_normal1 = polylines_segment_direction.size() == 0 ? normal : polylines_segment_direction[std::get<3>(v)][std::get<4>(v)];

            std::array<CGAL_Polyline, 4> beam_volume;
            cgal_box_search::two_rect_from_point_vector_and_zaxis(p0, v0, segment_normal0, type0, polylines_segment_radii[std::get<1>(v)][std::get<2>(v)], volume_length, flip_male, beam_volume[0], beam_volume[1]);
            cgal_box_search::two_rect_from_point_vector_and_zaxis(p1, v1, segment_normal1, type1, polylines_segment_radii[std::get<3>(v)][std::get<4>(v)], volume_length, flip_male, beam_volume[2], beam_volume[3]);

            ///////////////////////////////////////////////////////////////////////
            // Cut rectangles by planes, 0-0 bisector, 0-1 or 1-0 be closer female side
            // orient cross wood::joint closer to female
            ///////////////////////////////////////////////////////////////////////
            std::array<IK::Point_3, 4> intersection_points;
            if (type0 + type1 == 0)
            {
                // if v0,v1 are oriented from intersection point the plane normal is v0+v1

                // Get cutting planes

                IK::Point_3 p = CGAL::midpoint(p0, p1);
                IK::Point_3 p0_ = p + v0;
                IK::Point_3 p1_ = p + v1;
                IK::Plane_3 cut_plane = is_parallel ? IK::Plane_3(p, v0) : IK::Plane_3(p, v0 - v1);

                bool oriented_towards_v0 = cut_plane.has_on_positive_side(p0_);
                IK::Plane_3 cut_plane0 = oriented_towards_v0 ? IK::Plane_3(p, cut_plane.orthogonal_vector()) : IK::Plane_3(p, -cut_plane.orthogonal_vector());
                IK::Plane_3 cut_plane1 = !oriented_towards_v0 ? IK::Plane_3(p, cut_plane.orthogonal_vector()) : IK::Plane_3(p, -cut_plane.orthogonal_vector());

                for (size_t lid = 0; lid < 2; lid++)
                {
                    int shift = lid == 0 ? 0 : 2;
                    IK::Plane_3 cut_plane_ = lid == 0 ? cut_plane0 : cut_plane1;

                    // Intersect segments with plane
                    IK::Segment_3 s0(beam_volume[0 + shift][0], beam_volume[0 + shift][1]);
                    IK::Segment_3 s1(beam_volume[0 + shift][3], beam_volume[0 + shift][2]);
                    bool result = cgal_box_search::line_plane(s0, cut_plane_, intersection_points[0]);
                    result = cgal_box_search::line_plane(s1, cut_plane_, intersection_points[1]);
                    s0 = IK::Segment_3(beam_volume[1 + shift][0], beam_volume[1 + shift][1]);
                    s1 = IK::Segment_3(beam_volume[1 + shift][3], beam_volume[1 + shift][2]);
                    result = cgal_box_search::line_plane(s0, cut_plane_, intersection_points[2]);
                    result = cgal_box_search::line_plane(s1, cut_plane_, intersection_points[3]);

                    // points on positive side must be moved
                    if (cut_plane_.has_on_negative_side(beam_volume[0 + shift][0]))
                    {
                        beam_volume[0 + shift][0] = intersection_points[0];
                        beam_volume[0 + shift][3] = intersection_points[1];
                        beam_volume[0 + shift][4] = beam_volume[0 + shift][0];

                        beam_volume[1 + shift][0] = intersection_points[2];
                        beam_volume[1 + shift][3] = intersection_points[3];
                        beam_volume[1 + shift][4] = beam_volume[1 + shift][0];
                    }
                    else
                    {
                        beam_volume[0 + shift][1] = intersection_points[0];
                        beam_volume[0 + shift][2] = intersection_points[1];

                        beam_volume[1 + shift][1] = intersection_points[2];
                        beam_volume[1 + shift][2] = intersection_points[3];
                    }
                }

                // beam_volume[0] = CGAL_Polyline{ p + v0 ,p + 2 * v0,p + 3 * v0 ,p + 4 * v0,p + 4 * v0 + IK::Vector_3(0,0,10) };
                // beam_volume[2] = CGAL_Polyline{ p + v1 ,p + 2 * v1,p + 3 * v1,p + 4 * v1,p + 4 * v1 + IK::Vector_3(0,0,10) };
            }
            else if (type0 + type1 == 1)
            {
                // CGAL_Debug(1000);
                // if male vector is oriented from intersection point, the p+v closer rectangle is the cutting plane
                int closer_rect;
                int farrer_rect;
                if (type0 == 0)
                {
                    bool closer = CGAL::has_smaller_distance_to_point(p0 + v0, beam_volume[2][0], beam_volume[3][0]);
                    closer_rect = closer ? 2 : 3;
                    farrer_rect = closer ? 3 : 2;
                }
                else
                {
                    bool closer = CGAL::has_smaller_distance_to_point(p1 + v1, beam_volume[0][0], beam_volume[1][0]);
                    closer_rect = closer ? 0 : 1;
                    farrer_rect = closer ? 1 : 0;
                }

                IK::Vector_3 rect_v0 = beam_volume[closer_rect][1] - beam_volume[closer_rect][0];
                IK::Vector_3 rect_v1 = beam_volume[closer_rect][2] - beam_volume[closer_rect][0];
                IK::Vector_3 rect_normal = CGAL::cross_product(rect_v0, rect_v1);

                IK::Plane_3 cut_plane(beam_volume[closer_rect][0], rect_normal);
                if (cut_plane.has_on_positive_side(beam_volume[farrer_rect][0]))
                    cut_plane = IK::Plane_3(beam_volume[closer_rect][0], -rect_normal);

                // Get intersection points

                int shift = type0 == 0 ? 0 : 2;
                IK::Segment_3 s0(beam_volume[0 + shift][0], beam_volume[0 + shift][1]);
                IK::Segment_3 s1(beam_volume[0 + shift][3], beam_volume[0 + shift][2]);
                bool result = cgal_box_search::line_plane(s0, cut_plane, intersection_points[0]);
                result = cgal_box_search::line_plane(s1, cut_plane, intersection_points[1]);
                s0 = IK::Segment_3(beam_volume[1 + shift][0], beam_volume[1 + shift][1]);
                s1 = IK::Segment_3(beam_volume[1 + shift][3], beam_volume[1 + shift][2]);
                result = cgal_box_search::line_plane(s0, cut_plane, intersection_points[2]);
                result = cgal_box_search::line_plane(s1, cut_plane, intersection_points[3]);

                if (cut_plane.has_on_negative_side(beam_volume[0 + shift][0]))
                {
                    beam_volume[0 + shift][0] = intersection_points[0];
                    beam_volume[0 + shift][3] = intersection_points[1];
                    beam_volume[0 + shift][4] = beam_volume[0 + shift][0];

                    beam_volume[1 + shift][0] = intersection_points[2];
                    beam_volume[1 + shift][3] = intersection_points[3];
                    beam_volume[1 + shift][4] = beam_volume[1 + shift][0];
                }
                else
                {
                    beam_volume[0 + shift][1] = intersection_points[0];
                    beam_volume[0 + shift][2] = intersection_points[1];

                    beam_volume[1 + shift][1] = intersection_points[2];
                    beam_volume[1 + shift][2] = intersection_points[3];
                }
            }

            //////////////////////////////////////////////////////////////////////////////
            // Construct wood::element properties, side polylines and planes
            //////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
            printf("\nCPP -------------> beam_volumes Type %i %i \n", type0, type1);
#endif

            bool found_joint = pair_search(elements, beam_volume, std::get<1>(v), std::get<3>(v), (type0 + type1 == 2), joints, joints_map);

#ifdef DEBUG
            printf("CPP pair_search %i \n", found_joint);
#endif

            if (found_joint)
            {
                joints_areas.emplace_back(joints[joints.size() - 1].joint_area);
                joints_types.emplace_back(joints[joints.size() - 1].type);
            }

            //////////////////////////////////////////////////////////////////////////////
            // Search Contact zones
            //////////////////////////////////////////////////////////////////////////////

            // this is wrong because after joints must be distributed to elements that is why you need joints list
            volume_pairs.emplace_back(beam_volume);
        }

        if (!compute_joints)
            return;

            //////////////////////////////////////////////////////////////////////////////////
            ////Create and Align Joints 1. Iterate type 2. Select wood::joint based on not/given user joint_type
            //////////////////////////////////////////////////////////////////////////////////
#ifdef DEBUG
        printf("CPP finish pair search\n");
#endif

        // Experimental scale joints by value of intersection
        // This works only when two beams are the same radius
        if (use_eccentricities_to_scale_joints && joints.size() == point_pairs.size())
        {
            scale.clear();
            for (int i = 0; i < point_pairs.size(); i++)
            {
                double L = std::sqrt(CGAL::squared_distance(point_pairs[i][0], point_pairs[i][1]));
                double L_ = L;
                L *= 0.5;

                double scale_value = 1;
                if (L > 0.01)
                {

                    // double max_r = std::max(polylines_segment_radii[joints[i].v0][0], polylines_segment_radii[joints[i].v1][0]);
                    double max_r = (polylines_segment_radii[joints[i].v0][0] + polylines_segment_radii[joints[i].v1][0]) * 0.5;

                    // L == 0 will be scale 1
                    //
                    L = (max_r - L) / max_r;

                    // L /= (polylines_segment_radii[joints[i].v0][0] + polylines_segment_radii[joints[i].v1][0]) * 0.5;
                    scale_value = std::cos(std::asin(1 - std::min(L, 1.0))); // Math.Cos(Math.Asin(1 - x));
                    // scale_value = std::sin(std::acos(1 - std::min(L, 1.0)));//Math.Cos(Math.Asin(1 - x));
                }

                // std::cout << L_ << " " << L << " " << std::endl;

                joints[i].scale[0] = scale_value;
                joints[i].scale[1] = scale_value;
                joints[i].scale[2] = 1;
            }
            scale.clear();
        }

        wood_joint_lib::construct_joint_by_index(elements, joints, default_parameters_for_JOINTS_TYPES, scale); // division_distance, shift,
#ifdef DEBUG
        printf("CPP construct_joint_by_index\n");
#endif
        //////////////////////////////////////////////////////////////////////////////
        // Iterate wood::joint address
        //////////////////////////////////////////////////////////////////////////////
        // printf("\nCPP -------------> wood::joint count %i  \n", joints.size());

        output_plines = std::vector<std::vector<CGAL_Polyline>>(elements.size());
        output_types = std::vector<std::vector<wood_cut::cut_type>>(elements.size());
        for (int i = 0; i < elements.size(); i++)
        { // takes 30-50 ms just to copy past polyline geometry
            switch (output_type)
            {
            case (0):
                elements[i].get_joints_geometry(joints, output_plines, 0, output_types);
                break;
            case (1):
                elements[i].get_joints_geometry(joints, output_plines, 1, output_types);
                break;
            case (2):
                elements[i].get_joints_geometry(joints, output_plines, 2, output_types);
                break;
            default:
            case (3):
                elements[i].get_joints_geometry(joints, output_plines, 3, output_types);
                break;
            case (4):
                // This does not exist here because boolean cuts are made
                // elements[i].get_joints_geometry_as_closed_polylines_performing_intersection(joints, plines);
                break;
            }
        }
#ifdef DEBUG
        printf("CPP get_joints_geometry\n");
#endif
        // std::cout << output_plines.size() << std::endl;
        //  wood::joint jj;
        //  wood_joint_lib_xml::read_xml(jj, 0);
    }

#pragma endregion

}