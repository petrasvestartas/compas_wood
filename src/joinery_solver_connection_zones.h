#pragma once
#include "cgal.h"

#include "cgal_xform_util.h"
#include "cgal_vector_util.h"
#include "cgal_polyline_util.h"
#include "cgal_intersection_util.h"
#include "cgal_box_util.h"
#include "cgal_plane_util.h"
#include "cgal_print.h"
#include "cgal_math_util.h"
#include "cgal_mesh_util.h"

#include "clipper.h"
#include "rtree.h"

#include <algorithm>
#include <thread>
#include <functional>
#include <vector>
#include <execution>

#include "joinery_solver_element.h"
#include "joinery_solver_joint.h"
#include "joinery_solver_joint_library.h"

inline void get_elements(
	std::vector<CGAL_Polyline>& pp,
	std::vector<std::vector<IK::Vector_3>>& insertion_vectors,
	std::vector<std::vector<int>>& joint_types,
	std::vector<element>& elements) {

	int n = pp.size() * 0.5;
	//elements = std::vector<element>(n);
	elements.reserve(n);

	int count = 0;
	for (int i = 0; i < pp.size(); i += 2) {




		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Safety Check
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (pp[i].size() != pp[i + 1].size()) continue;
		if (CGAL::squared_distance(pp[i][0], pp[i + 1][0]) < GlobalToleranceSquare) continue;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create Empty Element
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//elements[id].id = i * 0.5;
		elements.emplace_back(i * 0.5);


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get BoundingBox - AABB
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create copy of a polyline and transform points
		//CGAL_Polyline twoPolylines;
		//twoPolylines.resize(pp[i].size() + pp[i + 1].size());
		//std::copy(pp[i].begin(), pp[i].end(), twoPolylines.begin());
		//std::copy(pp[i + 1].begin(), pp[i + 1].end(), twoPolylines.begin() + pp[i].size());

		CGAL_Polyline twoPolylines;
		twoPolylines.reserve(pp[i].size() + pp[i + 1].size());

		for (auto& p : pp[i])
			twoPolylines.emplace_back(p);

		for (auto& p : pp[i + 1])
			twoPolylines.emplace_back(p);



		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());

		CGAL_Polyline AABB_Min_Max = {
			IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
			IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
		};

		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());

		elements[count].aabb = AABB;

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Object Oriented BoundingBox - OOB -> transform to XY and back to 3D
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Create Planes
		IK::Vector_3 planeAxes[4];
		cgal_polyline_util::AveragePlane(pp[i], planeAxes);

		//Create Transformation
		CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::VectorsToXY(planeAxes[0], planeAxes[1], planeAxes[2], planeAxes[3]);
		CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

		//Transform the merged polyline to xy and compute xyBounding Box

		for (auto it = twoPolylines.begin(); it != twoPolylines.end(); ++it) {
			*it = it->transform(xform_toXY);
			//printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
		}

		//Compute bounding box, get center point, and contruct 5 size vector, where 5th dimension is halfsite,  then transform box back to 3D by an inverse matrix
		CGAL::Bbox_3 AABBXY = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
		double scale = 10;
		IK::Vector_3 box[5] = {
			IK::Vector_3(
				(AABBXY.xmin() + AABBXY.xmax()) * 0.5,
				(AABBXY.ymin() + AABBXY.ymax()) * 0.5,
				(AABBXY.zmin() + AABBXY.zmax()) * 0.5),
			IK::Vector_3(1,0,0),
			IK::Vector_3(0,1,0),
			IK::Vector_3(0,0,1),
			IK::Vector_3(
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.xmax() - AABBXY.xmin()) * 0.5),//0.00001
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.ymax() - AABBXY.ymin()) * 0.5),
			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.zmax() - AABBXY.zmin()) * 0.5))
		};
		//CGAL_Debug(1.0 + GlobalTolerance * 0.00001);

		cgal_box_util::TransformPlaneOrPlane(box, xform_toXY_Inv);
		cgal_box_util::Assign(box, elements[i * 0.5].oob, 5);

		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Check orientation of polylines and reverse if needed
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		bool reverse_poylines = twoPolylines.back().z() > 0;
		if (reverse_poylines) {
			std::reverse(pp[i].begin(), pp[i].end());
			std::reverse(pp[i + 1].begin(), pp[i + 1].end());
		}


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Get Side Polylines and Planes
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		elements[count].polylines = std::vector<CGAL_Polyline>(1 + pp[i].size());
		elements[count].polylines[0] = pp[i];
		elements[count].polylines[1] = pp[i + 1];

		elements[count].planes = std::vector<IK::Plane_3>(1 + pp[i].size());


		//IK::Point_3 origin = cgal_polyline_util::Center(pp[i]);
		IK::Vector_3 normal;
		cgal_vector_util::AverageNormal(pp[i], normal, true, false);
		elements[count].planes[0] = IK::Plane_3(cgal_polyline_util::Center(pp[i]), normal);
		elements[count].planes[1] = IK::Plane_3(cgal_polyline_util::Center(pp[i + 1]), -normal);
		elements[count].thickness = std::sqrt(CGAL::squared_distance(pp[i][0], elements[count].planes[1].projection(pp[i][0])));

		for (int j = 0; j < pp[i].size() - 1; j++) {
			elements[count].planes[2 + j] = IK::Plane_3(pp[i][j + 1], pp[i][j], pp[i + 1][j + 1]);
			elements[count].polylines[2 + j] = { pp[i][j],  pp[i][j + 1], pp[i + 1][j + 1], pp[i + 1][j], pp[i][j] };
		}

		//Edge initialization, total number of edge top,bottom + all sides + undefined not lying on face
		elements[count].j_mf = std::vector< std::vector<std::tuple<int, bool, double>>>((pp[i].size() - 1) + 2 + 1);//(side id, false, parameter on edge)


		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//User given properties, Other prperties such as insertion vectors or joint tapes
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (insertion_vectors.size() > 0)
			if (insertion_vectors[count].size() > 0) {
				elements[count].edge_vectors = insertion_vectors[count];
				//This was very nasty bug, because polylines are reverse based on orientation fix | also only shift +2 must be reversed (edges, skip top and bottom)
				if (reverse_poylines)
					std::reverse(elements[count].edge_vectors.begin() + 2, elements[count].edge_vectors.end());
			}

		if (joint_types.size() > 0)
			if (joint_types[count].size() > 0) {
				elements[count].joint_types = joint_types[count];
				//This was very nasty bug, because polylines are reverse based on orientation fix | also only shift +2 must be reversed (edges, skip top and bottom)
				if (reverse_poylines)
					std::reverse(elements[count].joint_types.begin() + 2, elements[count].joint_types.end());
			}

		count++;
	}



}







inline bool intersection_2D(CGAL_Polyline& p0, CGAL_Polyline& p1, IK::Plane_3& plane, CGAL_Polyline& c, double scale = 100000.0) {

	/////////////////////////////////////////////////////////////////////////////////////
	//Orient from 3D to 2D
	/////////////////////////////////////////////////////////////////////////////////////
	CGAL_Polyline a;
	CGAL_Polyline b;
	cgal_polyline_util::Duplicate(p0, a);
	cgal_polyline_util::Duplicate(p1, b);

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
	std::vector< ClipperLib::IntPoint > pathA(a.size() - 1);
	std::vector< ClipperLib::IntPoint > pathB(b.size() - 1);
	for (int i = 0; i < a.size() - 1; i++) {
		pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
		//printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
	}
	//printf("\n");
	for (int i = 0; i < b.size() - 1; i++) {
		pathB[i] = ClipperLib::IntPoint(b[i].x() * scale, b[i].y() * scale);
		//printf("%f,%f,%f \n", b[i].x(), b[i].y(), b[i].z());
	}

	//printf("\n");
	//printf("\n");

	ClipperLib::Clipper clipper;
	clipper.AddPath(pathA, ClipperLib::ptSubject, true);
	clipper.AddPath(pathB, ClipperLib::ptClip, true);
	ClipperLib::Paths C;
	clipper.Execute(ClipperLib::ctIntersection, C, ClipperLib::pftNonZero, ClipperLib::pftNonZero);



	if (C.size() > 0) {

		//CGAL_Debug(C[0].size());

		//CleanPolygon(C[0], GlobalTolerance * scale);//clean polygons

		//if (C[0].size() > 3 && Area(C[0]) > GlobalClipperAreaTolerance * scale * scale) {//skip triangles and very small polygons
		if (C[0].size() > 3 && std::abs(Area(C[0])) > std::abs(Area(pathA) * GlobalClipperAreaTolerance)) {//skip triangles and very small polygons

		/*	CGAL_Debug(Area(pathA));*/
			//CGAL_Debug(Area(C[0])/ (scale* scale));
			c.resize(C[0].size() + 1);

			for (int i = 0; i < C[0].size(); i++) {
				IK::Point_3 p(C[0][i].X / scale, C[0][i].Y / scale, 0);
				p = p.transform(xform_toXY_Inv);//Rotate back to 3D
				c[i] = p;
				//CGAL_Debug(c[i],true);
			}
			//CGAL_Debug(999999);
			c[C[0].size()] = c[0];//Close

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


inline bool plane_to_face(std::vector<
	CGAL_Polyline>& Polyline0,
	std::vector<CGAL_Polyline>& Polyline1,
	std::vector<IK::Plane_3>& Plane0,
	std::vector<IK::Plane_3>& Plane1,
	std::vector<IK::Vector_3>& insertion_vectors0,
	std::vector<IK::Vector_3>& insertion_vectors1,
	int& e0_0, int& e1_0,
	int& e0_1, int& e1_1,
	CGAL_Polyline& joint_area,
	//CGAL_Polyline(&joint_quads)[2],
	CGAL_Polyline(&joint_lines)[2],
	CGAL_Polyline(&joint_volumes_pairA_pairB)[4],

	int& type,

	double angleTol = 16,
	bool checkOverlap = false) {
	e0_0 = -1;
	e1_1 = -1;
	type = 30;

	//////////////////////////////////////////////////////////////////////////////
	//Check
	//1. Vector angles if they are not close to parallel
	//2. Are polylines overlapping (usually skipped within first except if stucture is planar) 3. Check overlay between center lines
	//////////////////////////////////////////////////////////////////////////////

	double angle = 90.0 - fabs(CGAL::approximate_angle(Plane0[0].orthogonal_vector(), Plane1[0].orthogonal_vector()) - 90);
	if (angle < angleTol) return false;


	//if (checkOverlap) if (Polylines0Center.plines[0].ToNurbsCurve().CurvesOverlap(Polylines1Center.plines[0].ToNurbsCurve())) return; //Happens very rarely if elements are planar






	//Line centerIntersectionLine = Line.Unset;
	//Line centerIntersectionLineMax = Line.Unset;
	IK::Segment_3 centerIntersectionLine;
	IK::Segment_3 centerIntersectionLineMax;

	//
	//    if (!cgal_polyline_util::PlanePolyline(Polyline0[0], Polyline1[0], Plane0[0], Plane1[0], centerIntersectionLine, centerIntersectionLineMax)) {
	///*        CGAL_Debug(centerIntersectionLine[0],true);
	//        CGAL_Debug(centerIntersectionLine[1],true)*/;
	//        //jointLine = { centerIntersectionLine[0],centerIntersectionLine[1] };
	//        //jointArea = { centerIntersectionLine[0],centerIntersectionLine[1] };
	//        return false;
	//    }

		//return true;

		//////////////////////////////////////////////////////////////////////////////
		//Inputs for intersection
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





	//IK::Segment_3 cx0_py0__cy0_px0_Max;
	//IK::Segment_3 cx0_py1__cy1_px0_Max;
	//IK::Segment_3 cx1_py0__cy0_px1_Max;
	//IK::Segment_3 cx1_py1__cy1_px1_Max;

	//////////////////////////////////////////////////////////////////////////////
	//Perform intersection
	//////////////////////////////////////////////////////////////////////////////
	//printf("\n_");
	IK::Segment_3 cx0_py0__cy0_px0;
	std::pair<int, int> edge_pair_e0_0__e1_0;//

	if (!cgal_polyline_util::PlanePolyline(*cx0, *cy0, *px0, *py0, cx0_py0__cy0_px0, edge_pair_e0_0__e1_0)) return false;//, cx0_py0__cy0_px0_Max
	//printf("A");

	IK::Segment_3 cx0_py1__cy1_px0;
	std::pair<int, int> edge_pair_e0_0__e1_1;
	if (!cgal_polyline_util::PlanePolyline(*cx0, *cy1, *px0, *py1, cx0_py1__cy1_px0, edge_pair_e0_0__e1_1)) return false;//, cx0_py1__cy1_px0_Max
	//printf("B");

	IK::Segment_3 cx1_py0__cy0_px1;
	std::pair<int, int> edge_pair_e0_1__e1_0;
	if (!cgal_polyline_util::PlanePolyline(*cx1, *cy0, *px1, *py0, cx1_py0__cy0_px1, edge_pair_e0_1__e1_0)) return false;//, cx1_py0__cy0_px1_Max
	//printf("C");

	IK::Segment_3 cx1_py1__cy1_px1;
	std::pair<int, int> edge_pair_e0_1__e1_1;
	if (!cgal_polyline_util::PlanePolyline(*cx1, *cy1, *px1, *py1, cx1_py1__cy1_px1, edge_pair_e0_1__e1_1)) return false;//,cx1_py1__cy1_px1_Max


	e0_0 = edge_pair_e0_0__e1_0.first + 2;
	e1_0 = edge_pair_e0_0__e1_0.second + 2;
	e0_1 = edge_pair_e0_1__e1_1.first + 2;
	e1_1 = edge_pair_e0_1__e1_1.second + 2;
	//	std::ofstream myfile;
	//	myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output3.txt");
	//	myfile << e0_0;
	//	myfile << e1_0;
	//	myfile << e0_1;
	//	myfile << e1_1;
	//
	//
	//myfile.close();
		//e0_0 = -1;
		//e1_0 = -1;
		//e0_1 = -1;
		//e1_1 = -1;
		//CGAL_Debug(e0_0, e0_1, e1_0, e1_1);


		//e0_0 = edge_pair_e0_0__e1_1.first;
		//e1_0 = edge_pair_e0_1__e1_0.second;
		//e0_1 = edge_pair_e0_1__e1_0.first;
		//e1_1 = edge_pair_e0_0__e1_1.second;
		//CGAL_Debug(e0_0, e0_1, e1_0, e1_1);
		//printf("D");
		//printf("_\n");

		//////////////////////////////////////////////////////////////////////////////
		//Sort Lines
		//////////////////////////////////////////////////////////////////////////////
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx0_py1__cy1_px0.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx0_py1__cy1_px0.to_vector()))
		cx0_py1__cy1_px0 = cx0_py1__cy1_px0.opposite();
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py0__cy0_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py0__cy0_px1.to_vector()))
		cx1_py0__cy0_px1 = cx1_py0__cy0_px1.opposite();
	if (CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), cx1_py1__cy1_px1.to_vector()) > CGAL::approximate_angle(cx0_py0__cy0_px0.to_vector(), -cx1_py1__cy1_px1.to_vector()))
		cx1_py1__cy1_px1 = cx1_py1__cy1_px1.opposite();



	//////////////////////////////////////////////////////////////////////////////
	//Define joint area from lines
	//////////////////////////////////////////////////////////////////////////////
	joint_volumes_pairA_pairB[0] = { cx0_py0__cy0_px0[0], cx0_py1__cy1_px0[0], cx1_py1__cy1_px1[0], cx1_py0__cy0_px1[0], cx0_py0__cy0_px0[0] };
	joint_volumes_pairA_pairB[1] = { cx0_py0__cy0_px0[1], cx0_py1__cy1_px0[1], cx1_py1__cy1_px1[1], cx1_py0__cy0_px1[1], cx0_py0__cy0_px0[1] };

	//////////////////////////////////////////////////////////////////////////////
	//Find shortest and longest curve based on closest point
	//-----*-----*--
	//*--------------*
	//////////////////////////////////////////////////////////////////////////////
	IK::Segment_3 c;
	cgal_polyline_util::MidLine(cx0_py1__cy1_px0, cx1_py0__cy0_px1, c);
	cgal_polyline_util::ScaleLine(c, 10);

	//Find Max gap
	double cpt0[4];
	cgal_polyline_util::ClosestPointTo(cx0_py0__cy0_px0[0], c, cpt0[0]);
	cgal_polyline_util::ClosestPointTo(cx0_py1__cy1_px0[0], c, cpt0[1]);
	cgal_polyline_util::ClosestPointTo(cx1_py0__cy0_px1[0], c, cpt0[2]);
	cgal_polyline_util::ClosestPointTo(cx1_py1__cy1_px1[0], c, cpt0[3]);
	std::sort(cpt0, cpt0 + 4);


	double cpt1[4];
	cgal_polyline_util::ClosestPointTo(cx0_py0__cy0_px0[1], c, cpt1[0]);
	cgal_polyline_util::ClosestPointTo(cx0_py1__cy1_px0[1], c, cpt1[1]);
	cgal_polyline_util::ClosestPointTo(cx1_py0__cy0_px1[1], c, cpt1[2]);
	cgal_polyline_util::ClosestPointTo(cx1_py1__cy1_px1[1], c, cpt1[3]);
	std::sort(cpt1, cpt1 + 4);

	double cpt[8] = { cpt0[0] ,cpt0[1] ,cpt0[2] ,cpt0[3],  cpt1[0] ,cpt1[1] ,cpt1[2] ,cpt1[3] };
	std::sort(cpt, cpt + 8);

	IK::Segment_3 lMin(cgal_polyline_util::PointAt(c, cpt0[3]), cgal_polyline_util::PointAt(c, cpt1[0]));
	IK::Segment_3 lMax(cgal_polyline_util::PointAt(c, cpt[0]), cgal_polyline_util::PointAt(c, cpt[7]));


	//////////////////////////////////////////////////////////////////////////////
	//When elements are not planar there are two options: 1) Rectangular beams are 3D overaly 2) Plate is connected to sharp corner in an angle
	//////////////////////////////////////////////////////////////////////////////
		//IK::Plane_3 jointAreaPlane0(jointArea0[0], jointArea0[1], jointArea0[2]);
		//IK::Plane_3 jointAreaPlane1(jointArea1[0], jointArea1[1], jointArea1[2]);


	//if (CGAL::squared_distance(jointAreaPlane0.projection(jointArea0[2]), jointArea0[2]) > GlobalToleranceSquare * 2 ||
	//    CGAL::squared_distance(jointAreaPlane1.projection(jointArea1[2]), jointArea1[2]) > GlobalToleranceSquare * 2 ) {

	//    //IK::Point_3 allPts [8] = {
	//    //    cx0_py0__cy0_px0_Max[0], cx0_py0__cy0_px0_Max[1],
	//    //    cx0_py1__cy1_px0_Max[0], cx0_py1__cy1_px0_Max[1],
	//    //    cx1_py0__cy0_px1_Max[0], cx1_py0__cy0_px1_Max[1],
	//    //    cx1_py1__cy1_px1_Max[0], cx1_py1__cy1_px1_Max[1]
	//    //};

	//    IK::Plane_3 lMaxPlane0(lMax[0], lMax.direction());
	//    IK::Plane_3 lMaxPlane1(lMax[1], lMax.direction());

	//    //LineTwoPlanes()

	//    ////Compute oriented bbox from allPts
	//    //Box box = new Box(lMax.ToPlane(true), allPts);

	//    ////Intersect line with box
	//    //Rhino.Geometry.Intersect.Intersection.LineBox(lMax, box, 0.01, out Interval lineParams);

	//    ////Remake maxLine
	//    //lMax = new Line(lMax.PointAt(lineParams.T0), lMax.PointAt(lineParams.T1));
	//}


	//////////////////////////////////////////////////////////////////////////////
	//RE-Define joint area from lines
	//////////////////////////////////////////////////////////////////////////////
	//lMin mid plane
	IK::Point_3 lMinMid;
	cgal_vector_util::MidPointConst(lMin.min(), lMin.max(), lMinMid);
	IK::Plane_3 midPlane(lMinMid, lMin.to_vector());//Min line mid-plane


	//Intersection lMax with midPlane
	IK::Point_3 midPlane_lMax;
	cgal_intersection_util::PlaneLineIntersection(midPlane, lMax, midPlane_lMax);


	//Get max distance from middle point of min line
	int maxID = CGAL::squared_distance(lMax[0], midPlane_lMax) > CGAL::squared_distance(lMax[1], midPlane_lMax) ? 0 : 1;
	IK::Vector_3 v = maxID == 1 ? lMax[1] - midPlane_lMax : -(lMax[0] - midPlane_lMax);
	v *= (1 + GlobalExtend[2]);


	//Align v direction in comparison to orignal 4 lines if possible
	//IK::Point_3 origin(0,0,0);
	//IK::Vector_3 v_copy = v;
	//IK::Vector_3 v_align = cx0_py0__cy0_px0.to_vector();
	//cgal_vector_util::Unitize(v_copy);
	//cgal_vector_util::Unitize(v_align);

	//if (CGAL::squared_distance(origin + v_align, origin + v_copy) > CGAL::squared_distance(origin - v_align, origin + v_copy))
		//v *= -1;

	//intersection mid plane with four lines and move it in both directions
	//CGAL_Polyline joint_area;
	cgal_intersection_util::Plane4LinesIntersection(midPlane, cx0_py0__cy0_px0, cx0_py1__cy1_px0, cx1_py1__cy1_px1, cx1_py0__cy0_px1, joint_area);



	//////////////////////////////////////////////////////////////////////////////
	//Move rectangles in opposite direction
	//////////////////////////////////////////////////////////////////////////////
	//std::reverse(joint_area.begin(), joint_area.end());
	//std::rotate(joint_area.begin(), joint_area.begin() + 1, joint_area.end());
	//std::vector<IK::Point_3> pts0 = { joint_area[0] ,joint_area[1] , joint_area[2] , joint_area[3]   };

	////IK::Point_3 pts1[4] = { joint_area[0] - v, joint_area[1] - v, joint_area[2] - v, joint_area[3] - v };
	//joint_area = pts0;
	//joint_area.push_back(joint_area[0]);







	//for (int i = 0; i < 4; i++) {
	//	if (CGAL::squared_distance(joint_area_oriented[0], px0->projection(joint_area_oriented[0])) > GlobalToleranceSquare)
	//		std::rotate(joint_area_oriented.begin(), joint_area_oriented.begin() + 1, joint_area_oriented.end());
	//	else {
	//		CGAL_Debug(9999);
	//		break;

	//	}
	//}

	//if (CGAL::squared_distance(joint_area[1], px1->projection(joint_area_oriented[1])) > GlobalToleranceSquare)
	//	std::reverse(joint_area_oriented.begin(), joint_area_oriented.end());
	//joint_area_oriented.emplace_back(joint_area_oriented[0]);
	//joint_area = joint_area_oriented;

	////std::rotate(joint_area_oriented.begin(), joint_area_oriented.begin() + 1, joint_area_oriented.end());

	//IK::Point_3 center = cgal_polyline_util::Center(*cx0);//not center but closest point, check all and tak closest
	//if (CGAL::squared_distance(IK::Point_3(joint_area_oriented[0]) + v, center) > CGAL::squared_distance(IK::Point_3(joint_area_oriented[0]) - v, center))
	//	v *= -1;


	//v *= -1;

	joint_volumes_pairA_pairB[0] = { IK::Point_3(joint_area[0]) + v, IK::Point_3(joint_area[1]) + v, IK::Point_3(joint_area[2]) + v, IK::Point_3(joint_area[3]) + v, IK::Point_3(joint_area[4]) + v };
	joint_volumes_pairA_pairB[1] = { IK::Point_3(joint_area[0]) - v, IK::Point_3(joint_area[1]) - v, IK::Point_3(joint_area[2]) - v, IK::Point_3(joint_area[3]) - v, IK::Point_3(joint_area[4]) - v };
	//std::reverse(joint_area.begin(), joint_area.end());
	//std::reverse(joint_volumes_pairA_pairB[0].begin(), joint_volumes_pairA_pairB[0].end());
	//std::reverse(joint_volumes_pairA_pairB[1].begin(), joint_volumes_pairA_pairB[1].end());

	//joint_lines[0] = { joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[1][0] + v };
	//joint_lines[1] = { joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[0][3] };

	 //jointArea0.insert(jointArea0.end(), jointArea1.begin(), jointArea1.end());



	 //jointArea0 = rm.Translate(lMax.direction().Unit() * (-(1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5
	 //jointArea1 = rm.Translate(lMax.direction().Unit() * ((1 + this.extend[2]) + 0.00) * maxLength);//For some reason extend by 1.5

	//does not work
	if (GlobalExtend[0] + GlobalExtend[1] > 0) {
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[0], 0, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[0], 2, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[0], 1, 0, 0, GlobalExtend[1], GlobalExtend[1]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[0], 3, 0, 0, GlobalExtend[1], GlobalExtend[1]);

		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[1], 0, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[1], 2, 0, 0, GlobalExtend[0], GlobalExtend[0]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[1], 1, 0, 0, GlobalExtend[1], GlobalExtend[1]);
		cgal_polyline_util::Extend(joint_volumes_pairA_pairB[1], 3, 0, 0, GlobalExtend[1], GlobalExtend[1]);
	}

	//For the sake of consistency
	//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
	//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];


	////////////////////////////////////////////////////////////////////////////////
	////return plate joints
	////////////////////////////////////////////////////////////////////////////////
	//Joint pj = new Joint(this.joints.Count, x.key, y.key, -1, -1, new List<Polyline>{ jointArea0, jointArea1 }, jointLines, new OutlineType[]{ OutlineType.Side, OutlineType.Side }, CollisionType.PlaneFace);//OutlineType
	//this.joints.Add(pj);

	return true;


}


//ToDo: Currently one connection can be made with another object, but one multiple shared edges can be possible e.g. |_>-<_|
inline bool face_to_face(
	std::vector<CGAL_Polyline>& Polyline0,
	std::vector<CGAL_Polyline>& Polyline1,
	std::vector<IK::Plane_3>& Plane0,
	std::vector<IK::Plane_3>& Plane1,
	std::vector<IK::Vector_3>& insertion_vectors0,
	std::vector<IK::Vector_3>& insertion_vectors1,
	int& f0_0, int& f1_0,
	int& f0_1, int& f1_1,
	CGAL_Polyline& joint_area,
	CGAL_Polyline(&joint_lines)[2],
	CGAL_Polyline(&joint_volumes_pairA_pairB)[4],
	int& type) {


	//printf("CPP StartIndersection \n");

	for (int i = 0; i < Plane0.size(); i++) {
		for (int j = 0; j < Plane1.size(); j++) {

			//Check if polygons are co-planar
			bool coplanar = cgal_plane_util::IsCoplanar(Plane0[i], Plane1[j], false, GlobalTolerance);//O(n*n) +10 ms

			//CGAL_Debug(coplanar);

			if (coplanar) {

				//Perform 2D Intersection 20 ms
				bool hasIntersection = intersection_2D(Polyline0[i], Polyline1[j], Plane0[i], joint_area, GlobalClipperScale);// +20 ms 10000.0; GlobalClipperScale

			   //////////////////////////////////////////////////////////////////////////////////////////////////
			   //Intersection lines and rectangles
			   //////////////////////////////////////////////////////////////////////////////////////////////////
				if (hasIntersection) {


					f0_0 = i;//Do not add +2, because planes are iterated
					f1_0 = j;//Do not add +2, because planes are iterated
					f0_1 = i;//Do not add +2, because planes are iterated
					f1_1 = j;//Do not add +2, because planes are iterated

					int type0 = i > 1 ? 0 : 1;
					int type1 = j > 1 ? 0 : 1;
					type = type0 + type1;

					//Get Line: Option A(0) - Side-Side i>2 && i > 2, Option B(1) - Side-Top i>2, C(2) Top-Top

					//jointArea1 = jointArea0;//default case (Top-top)
					IK::Segment_3 joint_line0(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
					IK::Plane_3 averagePlane0(CGAL::midpoint(Polyline0[0][0], Polyline0[1][0]), Plane0[0].orthogonal_vector());//Center Plane
					CGAL_Polyline joint_quads0;
					if (i > 1) {//Side-Top  or Side-Side

						//Middle line for alignment
						IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline0[0][i - 2], Polyline0[1][i - 2]), CGAL::midpoint(Polyline0[0][i - 1], Polyline0[1][i - 1]));

						//Intersect: a) clipper region, b) center plane

						bool isLine = cgal_intersection_util::PolylinePlane(joint_area, averagePlane0, alignmentSegment, joint_line0);

						//Planes to get a quad
						if (isLine && joint_line0.squared_length() > GlobalTolerance) {//

							bool isQuad = cgal_intersection_util::QuadFromLineAndTopBottomPlanes(Plane0[i], joint_line0, Plane0[0], Plane0[1], joint_quads0);
							//joint_quads0.push_back(Polyline0[0][0]);
							//joint_quads0.push_back(Polyline0[1][0]);
							//joint_quads0.push_back(Plane0[i].point());
						} else {
							CGAL_Debug(joint_quads0.size());
							return false;
						}
					}

					//Intersect second time for the side-side connection

					IK::Segment_3 joint_line1(IK::Point_3(0, 0, 0), IK::Point_3(0, 0, 0));
					IK::Plane_3 averagePlane1(CGAL::midpoint(Polyline1[0][0], Polyline1[1][0]), Plane1[0].orthogonal_vector());//Center Plane
					CGAL_Polyline joint_quads1;

					if (j > 1) {//Side-Side

						//Middle line for alignment
						IK::Segment_3 alignmentSegment(CGAL::midpoint(Polyline1[0][i - 2], Polyline1[1][i - 2]), CGAL::midpoint(Polyline1[0][i - 1], Polyline1[1][i - 1]));

						bool isLine = cgal_intersection_util::PolylinePlane(joint_area, averagePlane1, alignmentSegment, joint_line1);


						//Planes to get a quad
						if (isLine && joint_line1.squared_length() > GlobalTolerance) {//
							bool isQuad = cgal_intersection_util::QuadFromLineAndTopBottomPlanes(Plane1[j], joint_line1, Plane1[0], Plane1[1], joint_quads1);
							//joint_volumes_pairA_pairB[0] = joint_quads1;
							//joint_volumes_pairA_pairB[1] = joint_quads1;
						} else {
							CGAL_Debug(joint_quads1.size());
							return false;
							continue;
						}

					}

					//CGAL_Debug(type);
					//if (type == 1)
						//return false;
					//Top-top

					//return true;
					////////////////////////////////////////////////////////////////////////////////
					//ToDo set edge direction - Check Insertion angle if �dge axis is assigned
					//Applies for both elements
					////////////////////////////////////////////////////////////////////////////////
					IK::Vector_3 dir(0, 0, 0);
					bool dirSet = false;
					if (insertion_vectors0.size() > 0) {
						//////Take priority for male
						dir = i > j ? insertion_vectors0[i] : insertion_vectors1[j];
						dirSet = (std::abs(dir.hx()) + std::abs(dir.hy()) + std::abs(dir.hz())) > 0.01;
					}



					//////////////////////////////////////////////////////////////////////////////////////////////////
					//Indentify connection volumes
					//////////////////////////////////////////////////////////////////////////////////////////////////
					//CGAL_Debug(type);
					if (type == 0) {//side-side

						joint_lines[0] = { joint_line0[0], joint_line0[1] };
						joint_lines[1] = { joint_line1[0], joint_line1[1] };

						//return true;

						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//Elements are rotated
						////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						//CGAL_Debug(cgal_vector_util::Distance(jointLine0[0], jointLine0[1]));
						//CGAL_Debug(cgal_vector_util::Distance(jointLine1[0], jointLine1[1]));

						auto v0 = joint_line0[0] - joint_line0[1];
						auto v1 = joint_line1[0] - joint_line1[1];

						if (cgal_vector_util::IsParallelTo(v0, v1, GlobalTolerance) == 0) {


							//joint_lines[0] = { joint_line0[0], joint_line0[1] };
							//joint_lines[1] = { joint_line1[0], joint_line1[1] };

							////////////////////////////////////////////////////////////////////////////////
							//Get average intersection line
							////////////////////////////////////////////////////////////////////////////////
							IK::Segment_3 average_segment = CGAL::has_smaller_distance_to_point(joint_line0[0], joint_line1[0], joint_line1[1]) ? IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[0]), CGAL::midpoint(joint_line0[1], joint_line1[1])) : IK::Segment_3(CGAL::midpoint(joint_line0[0], joint_line1[1]), CGAL::midpoint(joint_line0[1], joint_line1[0]));

							////////////////////////////////////////////////////////////////////////////////
							//Create Plane to XY transformation matrix
							////////////////////////////////////////////////////////////////////////////////
							IK::Vector_3 o(average_segment[0].hx(), average_segment[0].hy(), average_segment[0].hz());
							IK::Vector_3 x = average_segment.to_vector();//cgal_vector_util::Unitize(x);
							IK::Vector_3 z = Plane0[i].orthogonal_vector();//cgal_vector_util::Unitize(z);
							IK::Vector_3 y = CGAL::cross_product(x, z);//cgal_vector_util::Unitize(y);
							CGAL::Aff_transformation_3<IK> xform = cgal_xform_util::VectorsToXY(o, x, y, z);

							////////////////////////////////////////////////////////////////////////////////
							//Decide min or max rectangle
							////////////////////////////////////////////////////////////////////////////////
							CGAL_Polyline joint_area_copy;
							bool min = true;
							if (min)
								cgal_polyline_util::Duplicate(joint_area, joint_area_copy);
							else {
								joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline0[i]), std::end(Polyline0[i]));
								joint_area_copy.insert(joint_area_copy.end(), std::begin(Polyline1[j]), std::end(Polyline1[j]));
							}

							////////////////////////////////////////////////////////////////////////////////
							//2D - Get boundary ractangle
							////////////////////////////////////////////////////////////////////////////////
							cgal_polyline_util::Transform(joint_area_copy, xform);
							auto AABB = CGAL::bbox_3(joint_area_copy.begin(), joint_area_copy.end(), IK());
							IK::Segment_3 segmentX(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmax(), AABB.ymin(), AABB.zmin()));
							IK::Segment_3 segmentY(IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin()), IK::Point_3(AABB.xmin(), AABB.ymax(), AABB.zmin()));
							CGAL_Polyline average_rectangle = { segmentX[0],segmentX[1],segmentX[0] + segmentX.to_vector() + segmentY.to_vector(),segmentY[1] };

							////////////////////////////////////////////////////////////////////////////////
							//3D Orient to 3D
							////////////////////////////////////////////////////////////////////////////////
							CGAL::Aff_transformation_3<IK> xformInv = xform.inverse();
							cgal_polyline_util::Transform(average_rectangle, xformInv);

							////////////////////////////////////////////////////////////////////////////////
							//Create Joint rectangles by an offset of element thickness
							////////////////////////////////////////////////////////////////////////////////
							IK::Vector_3 offset_vector = z;


							if (dirSet) {
								offset_vector = dir;
								//CGAL_Debug((dir.x() + dir.y() + dir.z()));
							}

							cgal_vector_util::Unitize(offset_vector);
							double d0 = 0.5 * std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
							offset_vector *= d0;

							joint_volumes_pairA_pairB[0] = { average_rectangle[3] + offset_vector, average_rectangle[3] - offset_vector, average_rectangle[0] - offset_vector, average_rectangle[0] + offset_vector, average_rectangle[0] + offset_vector };
							joint_volumes_pairA_pairB[1] = { average_rectangle[2] + offset_vector,average_rectangle[2] - offset_vector,average_rectangle[1] - offset_vector,average_rectangle[1] + offset_vector,  average_rectangle[1] + offset_vector };
							//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
							//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];
							type = 10;

							////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							//Elements are parallel
							////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						} else {




							////////////////////////////////////////////////////////////////////////////////
							//Get Overlap-Line // scale it down ?
							////////////////////////////////////////////////////////////////////////////////
							IK::Segment_3 lJ;
							cgal_polyline_util::LineLineOverlapAverage(joint_line0, joint_line1, lJ);
							//CGAL_Debug(std::sqrt(joint_line0.squared_length()), std::sqrt(joint_line1.squared_length()), std::sqrt(lJ.squared_length()));
							joint_lines[0] = { lJ[0],lJ[1] };
							joint_lines[1] = joint_lines[0];



							////////////////////////////////////////////////////////////////////////////////
							//Get Plane perpedicular to overlap-axis //with or without insertion vector |-----------|
							////////////////////////////////////////////////////////////////////////////////

							IK::Plane_3 plEnd0 = IK::Plane_3(lJ[0], lJ.to_vector());// averagePlane0.orthogonal_vector(), CGAL::cross_product(lJ.to_vector(), averagePlane0.orthogonal_vector()));

							if (dirSet)
								//plEnd0=IK::Plane_3(lJ[0], CGAL::cross_product(dir, CGAL::cross_product(lJ.to_vector(), dir)));
								plEnd0 = IK::Plane_3(lJ[0], dir);
							//CGAL_Debug(dir);

							IK::Plane_3 plEnd1(lJ[1], plEnd0.orthogonal_vector());
							IK::Plane_3 pl_mid(CGAL::midpoint(lJ[0], lJ[1]), plEnd0.orthogonal_vector());

							////////////////////////////////////////////////////////////////////////////////
							//Get dihedral angle to a) in-plane b) out-of-plane c) not-valid sharper than 20 deg
							//returns an approximation of the signed dihedral angle in the tetrahedron pqrs of edge pq.
							//The sign is negative if orientation(p, q, r, s) is CGAL::NEGATIVEand positive otherwise.The angle is given in degrees.
							////////////////////////////////////////////////////////////////////////////////
							IK::Point_3 centerPoint0 = averagePlane0.projection(cgal_polyline_util::Center(Polyline0[0]));
							IK::Point_3 centerPoint1 = averagePlane1.projection(cgal_polyline_util::Center(Polyline1[0]));
							auto dihedralAngle = std::abs(CGAL::approximate_dihedral_angle(lJ[0], lJ[1], centerPoint0, centerPoint1));

							if (dihedralAngle < 20) {//160
								//CGAL_Debug(20);
								return false;


							} else if (dihedralAngle <= 150) {//OUT-OF-PLANE // && jointArea0.size()>0
								//CGAL_Debug(150);
							 ////////////////////////////////////////////////////////////////////////////////
							 //Rotate line-joint 90 degrees and intersect with adjacent element top and bottom planes
							 //This is needed to check if adjacent element top and bottom planes are in the same or opposite order
							 ////////////////////////////////////////////////////////////////////////////////
								IK::Vector_3 connectionNormal = Plane0[i].orthogonal_vector();
								IK::Vector_3 lJ_normal = lJ.to_vector();
								IK::Vector_3 lJ_v_90 = CGAL::cross_product(lJ_normal, connectionNormal) * 0.5;
								IK::Line_3 lj_l_90(lJ[0], lJ_v_90);

								IK::Point_3 pl0_0_p;
								cgal_intersection_util::LinePlane(lj_l_90, Plane0[0], pl0_0_p);
								IK::Point_3  pl1_0_p;
								cgal_intersection_util::LinePlane(lj_l_90, Plane1[0], pl1_0_p);
								IK::Point_3 pl1_1_p;
								cgal_intersection_util::LinePlane(lj_l_90, Plane1[1], pl1_1_p);

								IK::Plane_3 planes[4];
								planes[1] = Plane0[0];

								if (CGAL::has_larger_distance_to_point(pl0_0_p, pl1_0_p, pl1_1_p)) {
									planes[2] = Plane1[0];
									planes[3] = Plane0[1];
									planes[0] = Plane1[1];
								} else {
									planes[2] = Plane1[1];
									planes[3] = Plane0[1];
									planes[0] = Plane1[0];
								}

								////////////////////////////////////////////////////////////////////////////////
								//Intersect End plane |-----------------------| with top and bottom planes
								////////////////////////////////////////////////////////////////////////////////
								cgal_intersection_util::plane_4_planes(plEnd0, planes, joint_volumes_pairA_pairB[0]);
								cgal_intersection_util::plane_4_planes(plEnd1, planes, joint_volumes_pairA_pairB[1]);
								//joint_volumes_pairA_pairB[2] = { joint_volumes_pairA_pairB[0][3],joint_volumes_pairA_pairB[0][0],joint_volumes_pairA_pairB[0][1],joint_volumes_pairA_pairB[0][2] };
								//joint_volumes_pairA_pairB[3] = { joint_volumes_pairA_pairB[1][3],joint_volumes_pairA_pairB[1][0],joint_volumes_pairA_pairB[1][1],joint_volumes_pairA_pairB[1][2] };

								type = 11;

							} else {//IN-PLANE
								//CGAL_Debug(-150);
							//joint_lines[0] = { joint_line0[0], joint_line0[1] };
							//joint_lines[1] = { joint_line1[0], joint_line1[1] };

							 ////////////////////////////////////////////////////////////////////////////////
							 //Intersect current top and bottom element planes, including the offseted joint face planes with |......................| end planes
							 ////////////////////////////////////////////////////////////////////////////////
								double d0 = 0.5 * std::sqrt(CGAL::squared_distance(Plane0[0].point(), Plane0[1].projection(Plane0[0].point())));
								IK::Plane_3 offset_plane_0 = cgal_plane_util::offset(Plane0[i], -d0);
								IK::Plane_3 offset_plane_1 = cgal_plane_util::offset(Plane0[i], d0);

								IK::Plane_3 loopOfPlanes0[4] = { offset_plane_0, Plane0[0], offset_plane_1, Plane0[1], };
								IK::Plane_3 loopOfPlanes1[4] = { offset_plane_0, Plane1[0], offset_plane_1, Plane1[1], };

								////////////////////////////////////////////////////////////////////////////////
								//Intersect End plane |-----------------------| with top and bottom planes
								////////////////////////////////////////////////////////////////////////////////
								cgal_intersection_util::plane_4_planes(plEnd0, loopOfPlanes0, joint_volumes_pairA_pairB[0]);
								cgal_intersection_util::plane_4_planes(plEnd1, loopOfPlanes0, joint_volumes_pairA_pairB[1]);
								cgal_intersection_util::plane_4_planes(plEnd0, loopOfPlanes1, joint_volumes_pairA_pairB[2]);
								cgal_intersection_util::plane_4_planes(plEnd1, loopOfPlanes1, joint_volumes_pairA_pairB[3]);
								type = 12;
							}


						}


					} else if (type == 1) {//top-side



					//double len0 = 0;
					//auto a = joint_line0[0];
					//auto b = joint_line0[1];
					//auto c = joint_line1[0];
					//auto d = joint_line1[1];

					//double len1 = cgal_vector_util::Distance(a, b);
					//double len2 = cgal_vector_util::Distance(c, d);
					//for (int k = 0; k < joint_area.size() - 1; k++) {
					//	len0+=cgal_vector_util::Distance(joint_area[k], joint_area[k+1]);
					//}
					//CGAL_Debug(len0, len1, len2);
					////if (len1 > 0.001 && len2 > 0.001)
					//	return false;





						//////////////////////////////////////////////////////////////////////////////////
						//Which element is male or female?
						//////////////////////////////////////////////////////////////////////////////////
						bool male_or_female = i > j;

						//if (!male_or_female) {
						//	f0_0 = j;
						//	f1_0 = i;
						//	f0_1 = f0_0;
						//	f1_1 = f1_0;
						//}

						joint_lines[0] = male_or_female ? CGAL_Polyline({ joint_line0[0], joint_line0[1] }) : CGAL_Polyline({ joint_line1[0], joint_line1[1] });
						joint_lines[1] = joint_lines[0];

						IK::Plane_3* plane0_0 = male_or_female ? &Plane0[0] : &Plane1[0];
						//IK::Plane_3* plane0_1 = male_or_female ? &Plane0[1] : &Plane1[1];
						IK::Plane_3* plane1_0 = !male_or_female ? &Plane0[i] : &Plane1[j];//female collision plane
						IK::Plane_3* plane1_1 = !male_or_female ? &Plane0[std::abs(i - 1)] : &Plane1[std::abs(j - 1)];

						//CGAL_Polyline* pline0 = male_or_female ? &Polyline0[0] : &Polyline1[0];
						//CGAL_Polyline* pline1 = !male_or_female ? &Polyline0[1] : &Polyline1[1];

						//IK::Segment_3* line_0 = male_or_female ? &joint_line0 : &joint_line1;//male, female does not exist int top-side
						CGAL_Polyline* quad_0 = male_or_female ? &joint_quads0 : &joint_quads1;//male, female does not exist int top-side


						//////////////////////////////////////////////////////////////////////////////////////
						//Two possibilities: if dir is unset move it perpendicularly else move by element direction
						//This case will only work for top-side connection when elements are parallell
						//For other cases you need to find a way to get opposite plane i.e. mesh intersection
						//////////////////////////////////////////////////////////////////////////////////////
						IK::Vector_3 offset_vector;
						cgal_intersection_util::orthogonal_vector_between_two_plane_pairs(*plane0_0, *plane1_0, *plane1_1, offset_vector);

						//dir = i > j ? insertion_vectors0[i]: insertion_vectors1[j];
						//dirSet = true;
						if (dirSet) {
							IK::Vector_3 offset_vector_;
							//CGAL::cross_product(dir, plane0_0->orthogonal_vector())
							bool flag = cgal_intersection_util::vector_two_planes(dir, *plane1_0, *plane1_1, offset_vector_);
							if (flag)
								offset_vector = offset_vector_;
						}






						//////////////////////////////////////////////////////////////////////////////////
						//Create Connection rectangles
						//////////////////////////////////////////////////////////////////////////////////
						joint_volumes_pairA_pairB[0] = { (*quad_0)[0], (*quad_0)[1], (*quad_0)[1] + offset_vector, (*quad_0)[0] + offset_vector, (*quad_0)[0] };
						joint_volumes_pairA_pairB[1] = { (*quad_0)[3], (*quad_0)[2], (*quad_0)[2] + offset_vector, (*quad_0)[3] + offset_vector, (*quad_0)[3] };
						//joint_area = joint_volumes_pairA_pairB[0];
						//joint_volumes_pairA_pairB[2] = joint_volumes_pairA_pairB[0];
						//joint_volumes_pairA_pairB[3] = joint_volumes_pairA_pairB[1];

						type = 20;
						return true;
					} else {
						type = 40;
						joint_volumes_pairA_pairB[0] = joint_area;
						joint_volumes_pairA_pairB[1] = joint_area;

						return true;

					}
					return true;



				}//Has Intersection
			}//Co-planar

		}//for j
	}//for i

	return false;

}


//inline void search(
//
//	//Input
//	std::vector<CGAL::Bbox_3>& AABB,
//	std::vector<IK::Vector_3[5]>& OOBs,
//	std::vector<std::vector<CGAL_Polyline>>& P,
//	std::vector<std::vector<IK::Plane_3>>& Pl,
//	std::vector<int>& pairs,
//	int searchType,
//
//	//Output
//	std::vector<int>& outputPairs, //Contact Element ID and Local ID
//	std::vector<CGAL_Polyline>& outputContactPolylines //Contact Areas + Lines
//
//
//
//
//) {
//
//
//
//	//////////////////////////////////////////////////////////////////////////////
//	// Search Contact zones
//	//////////////////////////////////////////////////////////////////////////////
//
//	for (int i = 0; i < pairs.size(); i += 2) {
//
//		CGAL_Polyline joint_area;
//		CGAL_Polyline joint_quads[2];
//		CGAL_Polyline joint_lines[2];
//		CGAL_Polyline joint_volumes_pairA_pairB[4];
//		int e0, e1, type;
//
//		bool flag = true;
//		switch (searchType) {
//		case(0):
//			flag = face_to_face(P[pairs[i]], P[pairs[i + 1]], Pl[pairs[i]], Pl[pairs[i + 1]], e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
//			break;
//		case(1):
//			flag = plane_to_face(P[pairs[i]], P[pairs[i + 1]], Pl[pairs[i]], Pl[pairs[i + 1]], e0, e1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
//			break;
//		}
//
//		if (!flag) continue;
//
//		if (e0 != e1) {
//
//			outputPairs.push_back(pairs[i]);
//			outputPairs.push_back(pairs[i + 1]);
//			outputPairs.push_back(e0);
//			outputPairs.push_back(e1);
//
//			outputContactPolylines.push_back(joint_area);
//			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_quads), std::end(joint_quads));
//			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_lines), std::end(joint_lines));
//			outputContactPolylines.insert(outputContactPolylines.end(), std::begin(joint_volumes_pairA_pairB), std::end(joint_volumes_pairA_pairB));
//
//		}
//	}
//
//
//
//
//}
//



inline void rtree_search(

	//Input
	std::vector<element>& elements,
	int search_type,

	//Output
	std::vector<joint>& joints,
	std::unordered_map<uint64_t, int>& joints_map
) {

	//////////////////////////////////////////////////////////////////////////////
	// Create RTree
	//////////////////////////////////////////////////////////////////////////////

	RTree<int, double, 3> tree;

	//////////////////////////////////////////////////////////////////////////////
	// Insert AABB
	//////////////////////////////////////////////////////////////////////////////

	for (int i = 0; i < elements.size(); i++) {
		double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
		double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
		tree.Insert(min, max, i);
	}

	//////////////////////////////////////////////////////////////////////////////
	// Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
	//////////////////////////////////////////////////////////////////////////////
	std::vector<int> result;
	for (int i = 0; i < elements.size(); i++) {//AABB.size()

		//std::vector<int> result;
		auto callback = [&result, i, &elements](int foundValue) -> bool
		{

			if (i < foundValue && cgal_box_util::GetCollision(elements[i].oob, elements[foundValue].oob)) {
				result.push_back(i);
				result.push_back(foundValue);
			}
			return true;
		};

		double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
		double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
		int nhits = tree.Search(min, max, callback);//callback in this method call callback above

	}
	joints.reserve(result.size());
	joints_map.reserve(result.size());


	//////////////////////////////////////////////////////////////////////////////
	// Search Contact zones
	//////////////////////////////////////////////////////////////////////////////
	int jointID = 0;
	for (int i = 0; i < result.size(); i += 2) {

		CGAL_Polyline joint_area;
		CGAL_Polyline joint_quads[2];
		CGAL_Polyline joint_lines[2];
		CGAL_Polyline joint_volumes_pairA_pairB[4];
		int f0_0, f1_0, f0_1, f1_1, type;

		int found_type = 0;
		switch (search_type) {
			case(0):
				found_type = face_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, elements[result[i]].edge_vectors, elements[result[i + 1]].edge_vectors, f0_0, f1_0, f0_1, f1_1, joint_area, joint_lines, joint_volumes_pairA_pairB, type) ? 1 : 0;
				break;
			case(1):
				found_type = plane_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, elements[result[i]].edge_vectors, elements[result[i + 1]].edge_vectors, f0_0, f1_0, f0_1, f1_1, joint_area, joint_lines, joint_volumes_pairA_pairB, type) ? 2 : 0;
				break;
			case(2):
				bool flag0 = face_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, elements[result[i]].edge_vectors, elements[result[i + 1]].edge_vectors, f0_0, f1_0, f0_1, f1_1, joint_area, joint_lines, joint_volumes_pairA_pairB, type);
				if (flag0) {
					found_type = 3;
					break;
				}

				found_type = plane_to_face(elements[result[i]].polylines, elements[result[i + 1]].polylines, elements[result[i]].planes, elements[result[i + 1]].planes, elements[result[i]].edge_vectors, elements[result[i + 1]].edge_vectors, f0_0, f1_0, f0_1, f1_1, joint_area, joint_lines, joint_volumes_pairA_pairB, type) ? 3 : 0;


				break;
		}

		if (!found_type) continue;


		//CGAL_Debug(joint_area.size());
		if (joint_area.size() > 0) {


			//


			joints.emplace_back(
				jointID,
				result[i], result[i + 1],
				f0_0, f1_0, f0_1, f1_1,
				joint_area,
				joint_lines,
				joint_volumes_pairA_pairB,
				type
			);




			//CGAL_Debug(1);
			joints_map.emplace(cgal_math_util::unique_from_two_int(result[i], result[i + 1]), jointID);

			//CGAL_Debug(e1);
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			//Place joint ids and male or female flags to joint list
			////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (f1_0 == -1) {
				//CGAL_Debug(3);
				elements[result[i + 0]].j_mf.back().push_back(std::tuple<int, bool, double>(jointID, true, 0));
				elements[result[i + 1]].j_mf.back().push_back(std::tuple<int, bool, double>(jointID, false, 0));
				//CGAL_Debug(4);
			} else {



				//CGAL_Debug((double)f0_0, (double)f1_0);
				if ((f1_0 < 2 || f0_0 < 2) && type != 30 && type != 40) {//side-top connection weirdo, clean this up
					if ((f1_0 < 2 && f0_0>1)) {
						elements[result[i + 0]].j_mf[f0_0].push_back(std::tuple<int, bool, double>(jointID, true, 0));
						elements[result[i + 1]].j_mf[f1_0].push_back(std::tuple<int, bool, double>(jointID, false, 0));
					} else {
						elements[result[i + 0]].j_mf[f0_0].push_back(std::tuple<int, bool, double>(jointID, false, 0));
						elements[result[i + 1]].j_mf[f1_0].push_back(std::tuple<int, bool, double>(jointID, true, 0));
					}
				} else {
					elements[result[i + 0]].j_mf[f0_0].push_back(std::tuple<int, bool, double>(jointID, true, 0));
					elements[result[i + 1]].j_mf[f1_0].push_back(std::tuple<int, bool, double>(jointID, false, 0));

				}
			}




			jointID++;
		}



	}

	//CGAL_Debug(9999999);
	//CGAL_Debug(joints.size());

}


//inline void get_obb_and_planes(
//	//Input
//	std::vector<CGAL_Polyline>& pp,
//	//Output
//	std::vector< CGAL::Bbox_3>& AABBs,
//	std::vector<IK::Vector_3[5]>& OOBs,
//	std::vector< std::vector<CGAL_Polyline>>& P,
//	std::vector< std::vector<IK::Plane_3>>& Pls) {
//
//	int n = pp.size() * 0.5;
//
//	AABBs = std::vector<CGAL::Bbox_3>(n);
//	OOBs = std::vector<IK::Vector_3[5]>(n);
//	P = std::vector<std::vector<CGAL_Polyline>>(n);
//	Pls = std::vector<std::vector<IK::Plane_3>>(n);
//
//
//
//	for (int i = 0; i < pp.size(); i += 2) {
//
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Get BoundingBox - AABB
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Create copy of a polyline and transform points
//		CGAL_Polyline twoPolylines;
//		twoPolylines.resize(pp[i].size() + pp[i + 1].size());
//		std::copy(pp[i].begin(), pp[i].end(), twoPolylines.begin());
//		std::copy(pp[i + 1].begin(), pp[i + 1].end(), twoPolylines.begin() + pp[i].size());
//
//		//auto t = cgal_xform_util::Scale(cgal_polyline_util::Center(twoPolylines), 1+ GlobalTolerance);//change when dilate works
//		//cgal_polyline_util::Transform(twoPolylines, t);//change when dilate works
//
//		CGAL::Bbox_3 AABB = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
//
//		CGAL_Polyline AABB_Min_Max = {
//			IK::Point_3(AABB.xmin() - 1 * GlobalTolerance, AABB.ymin() - 1 * GlobalTolerance, AABB.zmin() - 1 * GlobalTolerance),
//			IK::Point_3(AABB.xmax() + 1 * GlobalTolerance, AABB.ymax() + 1 * GlobalTolerance, AABB.zmax() + 1 * GlobalTolerance),
//		};
//
//		AABB = CGAL::bbox_3(AABB_Min_Max.begin(), AABB_Min_Max.end(), IK());
//
//		//CGAL_Debug(AABB.zmax());
//		//AABB.dilate(1E34);
//		//CGAL_Debug(AABB.zmax());
//		//AABB = CGAL::bbox_3()
//		//AABB.dilate(GlobalTolerance);//Does not work
//		AABBs[i * 0.5] = AABB;
//
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Get Object Oriented BoundingBox - OOB -> transform to XY and back to 3D
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Create Planes
//		IK::Vector_3 planeAxes[4];
//		cgal_polyline_util::AveragePlane(pp[i], planeAxes);
//
//		//Create Transformation
//		CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::VectorsToXY(planeAxes[0], planeAxes[1], planeAxes[2], planeAxes[3]);
//		CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();
//
//		//Transform the merged polyline to xy and compute xyBounding Box
//
//		for (auto it = twoPolylines.begin(); it != twoPolylines.end(); ++it) {
//			*it = it->transform(xform_toXY);
//			//printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
//		}
//
//		//Compute bounding box, get center point, and contruct 5 size vector, where 5th dimension is halfsite,  then transform box back to 3D by an inverse matrix
//		CGAL::Bbox_3 AABBXY = CGAL::bbox_3(twoPolylines.begin(), twoPolylines.end(), IK());
//		double scale = 10;
//		IK::Vector_3 box[5] = {
//			IK::Vector_3(
//				(AABBXY.xmin() + AABBXY.xmax()) * 0.5,
//				(AABBXY.ymin() + AABBXY.ymax()) * 0.5,
//				(AABBXY.zmin() + AABBXY.zmax()) * 0.5),
//			IK::Vector_3(1,0,0),
//			IK::Vector_3(0,1,0),
//			IK::Vector_3(0,0,1),
//			IK::Vector_3(
//			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.xmax() - AABBXY.xmin()) * 0.5),//0.00001
//			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.ymax() - AABBXY.ymin()) * 0.5),
//			fabs((1.0 + GlobalTolerance * 1) * (AABBXY.zmax() - AABBXY.zmin()) * 0.5))
//		};
//		//CGAL_Debug(1.0 + GlobalTolerance * 0.00001);
//
//
//
//
//		cgal_box_util::TransformPlaneOrPlane(box, xform_toXY_Inv);
//		cgal_box_util::Assign(box, OOBs[i * 0.5], 5);
//
//		//CGAL_Debug(box[0]);
//		//CGAL_Debug(box[1]);
//		//CGAL_Debug(box[2]);
//		//CGAL_Debug(box[3]);
//		//CGAL_Debug(box[4]);
//
//
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Check orientation of polylines and reverse if needed
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		if (twoPolylines.back().z() > 0) {
//			std::reverse(pp[i].begin(), pp[i].end());
//			std::reverse(pp[i + 1].begin(), pp[i + 1].end());
//		}
//
//
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		//Get Side Polylines and Planes
//		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//		P[i * 0.5] = std::vector<CGAL_Polyline>(1 + pp[i].size());
//		P[i * 0.5][0] = pp[i];
//		P[i * 0.5][1] = pp[i + 1];
//
//		Pls[i * 0.5] = std::vector<IK::Plane_3>(1 + pp[i].size());
//
//
//		//IK::Point_3 origin = cgal_polyline_util::Center(pp[i]);
//		IK::Vector_3 normal;
//		cgal_vector_util::AverageNormal(pp[i], normal,true,false);
//		Pls[i * 0.5][0] = IK::Plane_3(cgal_polyline_util::Center(pp[i]), normal);
//		Pls[i * 0.5][1] = IK::Plane_3(cgal_polyline_util::Center(pp[i + 1]), -normal);
//
//		for (int j = 0; j < pp[i].size() - 1; j++) {
//			Pls[i * 0.5][2 + j] = IK::Plane_3(pp[i][j + 1], pp[i][j], pp[i + 1][j + 1]);
//			P[i * 0.5][2 + j] = { pp[i][j],  pp[i][j + 1], pp[i + 1][j + 1], pp[i + 1][j], pp[i][j] };
//		}
//
//
//	}
//
//
//
//}


inline void three_valence_joint_alignment(
	std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
	std::vector<element>& elements,
	std::vector<joint>& joints,
	std::unordered_map<uint64_t, int>& joints_map,
	//std::vector<std::vector<CGAL_Polyline>>& plines,
	double division_length
) {

	//CGAL_Debug(0);
	//////////////////////////////////////////////////////////////////////////////////////////////////
	 //For solving multiple valences (Specific case Annen), only works when only one joint is possible between two unique plates (wont work for plates with subdivided edges)
	 //////////////////////////////////////////////////////////////////////////////////////////////////
	if (out_three_valence_element_indices_and_instruction.size() == 0) return;
	//CGAL_Debug(1);

	//CGAL_Debug();
	//CGAL_Debug(joints_map.size());
	//for (auto a : joints_map) {
	//	CGAL_Debug(a.first);
	//	CGAL_Debug(a.second);
	//}
	//CGAL_Debug();

	for (int i = 0; i < out_three_valence_element_indices_and_instruction.size(); i++) {



		//////////////////////////////////////////////////////////////////////////////////////////////////
		//get unique key
		//////////////////////////////////////////////////////////////////////////////////////////////////
		int id_0 = joints_map[cgal_math_util::unique_from_two_int(out_three_valence_element_indices_and_instruction[i][0], out_three_valence_element_indices_and_instruction[i][1])];
		int id_1 = joints_map[cgal_math_util::unique_from_two_int(out_three_valence_element_indices_and_instruction[i][2], out_three_valence_element_indices_and_instruction[i][3])];

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//Get overlap segment and plane within its normal
		//////////////////////////////////////////////////////////////////////////////////////////////////
		IK::Segment_3 l0(joints[id_0].joint_lines[0][0], joints[id_0].joint_lines[0][1]);


		IK::Segment_3 l1 = CGAL::has_smaller_distance_to_point(joints[id_0].joint_lines[0][0], joints[id_1].joint_lines[0][0], joints[id_1].joint_lines[0][1])
			? IK::Segment_3(joints[id_1].joint_lines[0][0], joints[id_1].joint_lines[0][1])
			: IK::Segment_3(joints[id_1].joint_lines[0][1], joints[id_1].joint_lines[0][0]);

		IK::Segment_3 l;
		cgal_polyline_util::line_line_overlap_average_segments(l0, l1, l);

		//plines.push_back({ l[0] ,l[1] });

		double divisions = std::ceil(l.squared_length() / (division_length * division_length));
		joints[id_0].tile_parameters = { divisions };
		joints[id_1].tile_parameters = { divisions };

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//Construct plane from exisiting joint volume edges
		//////////////////////////////////////////////////////////////////////////////////////////////////

		//if (joints[id_0].joint_volumes[0].size() == 0) return;
		//if (joints[id_1].joint_volumes[0].size() == 0) return;

		IK::Vector_3 cross0 = CGAL::cross_product(
			joints[id_0].joint_volumes[0][2] - joints[id_0].joint_volumes[0][1],
			joints[id_0].joint_volumes[0][0] - joints[id_0].joint_volumes[0][1]);
		IK::Vector_3 cross1 = CGAL::cross_product(
			joints[id_1].joint_volumes[0][2] - joints[id_1].joint_volumes[0][1],
			joints[id_1].joint_volumes[0][0] - joints[id_1].joint_volumes[0][1]);

		//////////////////////////////////////////////////////////////////////////////////////////////////
		//Intersect lines with planes
		//////////////////////////////////////////////////////////////////////////////////////////////////

		//CGAL_Debug(cross0);
		//CGAL_Debug(cross1);

		//if (!CGAL::has_smaller_distance_to_point(IK::Point_3(cross0.hx(), cross0.hy(), cross0.hz()), IK::Point_3(cross1.hx(), cross1.hy(), cross1.hz()), IK::Point_3(-cross1.hx(), -cross1.hy(), -cross1.hz())))
		//	cross1 = IK::Vector_3(-cross1.hx(), -cross1.hy(), -cross1.hz());

		IK::Plane_3 plane0_0(l[0], cross0);
		IK::Plane_3 plane0_1(l[1], cross0);
		IK::Plane_3 plane1_0(l[1], cross1);
		IK::Plane_3 plane1_1(l[0], cross1);

		//plines.push_back({ l[0], l[0] + cross0 });
		//plines.push_back({ l[1], l[1] + cross0 });
		//plines.push_back({ l[0], l[0] + cross1 });
		//plines.push_back({ l[1], l[1] + cross1 });

		for (int j = 0; j < 4; j += 2) {
			if (joints[id_0].joint_volumes[j].size() == 0) continue;

			IK::Segment_3 s0(joints[id_0].joint_volumes[j + 0][0], joints[id_0].joint_volumes[j + 1][0]);
			IK::Segment_3 s1(joints[id_0].joint_volumes[j + 0][1], joints[id_0].joint_volumes[j + 1][1]);
			IK::Segment_3 s2(joints[id_0].joint_volumes[j + 0][2], joints[id_0].joint_volumes[j + 1][2]);
			IK::Segment_3 s3(joints[id_0].joint_volumes[j + 0][3], joints[id_0].joint_volumes[j + 1][3]);

			cgal_intersection_util::Plane4LinesIntersection(plane0_0, s0, s1, s2, s3, joints[id_0].joint_volumes[j]);
			cgal_intersection_util::Plane4LinesIntersection(plane0_1, s0, s1, s2, s3, joints[id_0].joint_volumes[j + 1]);
			//plines.push_back(joints[id_0].joint_volumes[j]);
			//plines.push_back(joints[id_0].joint_volumes[j+1]);
		}

		for (int j = 0; j < 4; j += 2) {
			if (joints[id_1].joint_volumes[j].size() == 0) continue;

			IK::Segment_3 s0(joints[id_1].joint_volumes[j + 0][0], joints[id_1].joint_volumes[j + 1][0]);
			IK::Segment_3 s1(joints[id_1].joint_volumes[j + 0][1], joints[id_1].joint_volumes[j + 1][1]);
			IK::Segment_3 s2(joints[id_1].joint_volumes[j + 0][2], joints[id_1].joint_volumes[j + 1][2]);
			IK::Segment_3 s3(joints[id_1].joint_volumes[j + 0][3], joints[id_1].joint_volumes[j + 1][3]);

			cgal_intersection_util::Plane4LinesIntersection(plane1_0, s0, s1, s2, s3, joints[id_1].joint_volumes[j]);
			cgal_intersection_util::Plane4LinesIntersection(plane1_1, s0, s1, s2, s3, joints[id_1].joint_volumes[j + 1]);

		}

	}







}

//inline bool OrientTile(CGAL_Polyline &rect0, CGAL_Polyline &rect1, std::vector<std::vector<CGAL_Polyline>> &orientedTiles, int tileType = 0) {
//
//    if (rect0.size() != 5 || rect1.size() != 5)
//        return false;
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Get a Tile
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    std::vector<CGAL_Polyline> tile = cgal_polyline_util::Cross_Simple();
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Transform a tile
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    for (int i = 0; i < tile.size(); i++) {
//        CGAL::Aff_transformation_3<IK> X;
//
//        cgal_xform_util::ChangeBasis(
//            IK::Point_3(-0.5, -0.5, -0.5), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 1, 0), IK::Vector_3(0, 0, 1),
//            rect0[0], (rect0[1] - rect0[0]), (rect0[3] - rect0[0]), (rect1[0] - rect0[0]),
//            X
//        );
//
//        cgal_polyline_util::Transform(tile[i], X);
//
//    }
//
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    //Output
//    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    orientedTiles.push_back(tile);
//    return true;
//}



inline void get_connection_zones(
	std::vector<CGAL_Polyline>& input_polyline_pairs,
	std::vector<std::vector<IK::Vector_3>>& input_insertion_vectors,
	std::vector<std::vector<int>>& input_joint_types,
	std::vector<std::vector<int>>& input_three_valence_element_indices_and_instruction,


	//output
	std::vector<std::vector<CGAL_Polyline>>& plines,
	std::vector<std::vector<int>>& top_face_triangulation,

	//Global Parameters
	std::vector<double>& default_parameters_for_joint_types,
	int search_type = 1,
	double division_distance = 300,
	double shift = 0.6,
	int output_type = 4,
	int triangulate = 0

) {

	const int n = input_polyline_pairs.size() * 0.5;

	//////////////////////////////////////////////////////////////////////////////
	//Create elements, AABB, OBB, P, Pls, thickness
	//////////////////////////////////////////////////////////////////////////////
	std::vector<element> elements;
	get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);


	//////////////////////////////////////////////////////////////////////////////
	//Create joints, Perform Joint Area Search
	//////////////////////////////////////////////////////////////////////////////
	auto joints = std::vector<joint>();
	auto joints_map = std::unordered_map<uint64_t, int>();
	rtree_search(elements, search_type, joints, joints_map);

	//////////////////////////////////////////////////////////////////////////////
	//3-valence joints
	//////////////////////////////////////////////////////////////////////////////
	if (input_three_valence_element_indices_and_instruction.size() > 0)
		three_valence_joint_alignment(input_three_valence_element_indices_and_instruction, elements, joints, joints_map, division_distance);//plines, 

	////////////////////////////////////////////////////////////////////////////////
	////Create and Align Joints 1. Iterate type 2. Select joint based on not/given user joint_type
	////////////////////////////////////////////////////////////////////////////////
	joint_library::construct_joint_by_index(elements, joints, division_distance, shift, default_parameters_for_joint_types);

	//////////////////////////////////////////////////////////////////////////////
	//Iterate joint address
	//////////////////////////////////////////////////////////////////////////////

	plines = std::vector<std::vector<CGAL_Polyline>>(elements.size());
	//CGAL_Debug(joints.size());
	for (int i = 0; i < elements.size(); i++) {//takes 30-50 ms just to copy past polyline geometry

		switch (output_type) {
			case(0):
				elements[i].get_joints_geometry(joints, plines, 0);
				break;
			case(1):
				elements[i].get_joints_geometry(joints, plines, 1);
				break;
			case(2):
				elements[i].get_joints_geometry(joints, plines, 2);
				break;
			default:
			case(3):
				elements[i].get_joints_geometry(joints, plines, 3);
				break;
			case(4):
				//elements[i].get_joints_geometry_as_closed_polylines_replacing_edges(joints, plines);
				elements[i].get_joints_geometry_as_closed_polylines_performing_intersection(joints, plines);
				break;
		}

	}

	//////////////////////////////////////////////////////////////////////////////
	//Create Mesh Triangulation for top face
	//////////////////////////////////////////////////////////////////////////////
	if (triangulate) {
		top_face_triangulation = std::vector<std::vector<int>>(elements.size());
		for (int i = 0; i < elements.size(); i++) {
            int v, f;
			cgal_mesh_util::mesh_from_polylines(plines[i], elements[i].planes[0], top_face_triangulation[i],v,f);

		}
	}


}


