#pragma once
#include "cgal.h"
#include "cgal_vector_util.h"
#include "cgal_print.h"
#include "clipper.h"

namespace cgal_polyline_util
{

	inline double polyline_length(CGAL_Polyline &pline)
	{

		double l = 0;
		for (int i = 0; i < pline.size(); i++)
		{
			l += std::sqrt(CGAL::squared_distance(pline[i + 0], pline[i + 1]));
		}

		return l;
	}

	inline IK::Point_3 PointAt(const IK::Segment_3 &l, double t)
	{

		//s[0].z()
		// 26 Feb 2003 Dale Lear
		//     Changed
		//          return (1-t)*from + t*to;
		//     to the following so that axis aligned lines will
		//     return exact answers for large values of t.
		//     See RR 9683.

		const double s = 1.0 - t;

		return IK::Point_3(
			(l[0].x() == l[1].x()) ? l[0].x() : s * l[0].x() + t * l[1].x(),
			(l[0].y() == l[1].y()) ? l[0].y() : s * l[0].y() + t * l[1].y(),
			(l[0].z() == l[1].z()) ? l[0].z() : s * l[0].z() + t * l[1].z());
	}

	inline bool ClosestPointTo(const IK::Point_3 &point, const IK::Segment_3 &s, double &t)
	{
		bool rc = false;

		const IK::Vector_3 D = s.to_vector();
		const double DoD = D.squared_length();

		if (DoD > 0.0)
		{

			if ((point - s[0]).squared_length() <= (point - s[1]).squared_length())
				t = ((point - s[0]) * D) / DoD;
			else
				t = 1.0 + ((point - s[1]) * D) / DoD;

			rc = true;
		}
		else
		{ // (GBA) Closest point to a degenerate line works as well
			t = 0.0;
			rc = true;
		}

		return rc;
	}

	inline void LineLineOverlap(const IK::Segment_3 &l0, const IK::Segment_3 &l1, IK::Segment_3 &result)
	{

		double t[4];
		t[0] = 0;
		t[1] = 1;
		ClosestPointTo(l0[0], l1, t[2]);
		ClosestPointTo(l0[1], l1, t[3]);

		std::sort(t, t + 4);

		result = IK::Segment_3(PointAt(l0, t[1]), PointAt(l0, t[2]));
	}

	inline void LineLineOverlapAverage(const IK::Segment_3 &l0, const IK::Segment_3 &l1, IK::Segment_3 &result)
	{
		IK::Segment_3 lA;
		LineLineOverlap(l0, l1, lA);
		IK::Segment_3 lB;
		LineLineOverlap(l1, l0, lB);

		IK::Segment_3 a(cgal_vector_util::MidPoint_(lA[0], lB[0]), cgal_vector_util::MidPoint_(lA[1], lB[1]));
		IK::Segment_3 b(cgal_vector_util::MidPoint_(lA[0], lB[1]), cgal_vector_util::MidPoint_(lA[1], lB[0]));

		result = (a.squared_length() > b.squared_length()) ? a : b;
	}

	inline void LineLineOverlapAverage(const CGAL_Polyline &l0_, const CGAL_Polyline &l1_, IK::Segment_3 &result)
	{

		IK::Segment_3 l0(l0_[0], l0_[1]);
		IK::Segment_3 l1(l1_[0], l1_[1]);

		IK::Segment_3 lA;
		LineLineOverlap(l0, l1, lA);
		IK::Segment_3 lB;
		LineLineOverlap(l1, l0, lB);

		IK::Segment_3 a(cgal_vector_util::MidPoint_(lA[0], lB[0]), cgal_vector_util::MidPoint_(lA[1], lB[1]));
		IK::Segment_3 b(cgal_vector_util::MidPoint_(lA[0], lB[1]), cgal_vector_util::MidPoint_(lA[1], lB[0]));

		result = (a.squared_length() > b.squared_length()) ? a : b;
	}

	inline void line_line_overlap_average_segments(const IK::Segment_3 &l0, const IK::Segment_3 &l1, IK::Segment_3 &result)
	{

		IK::Segment_3 lA;
		LineLineOverlap(l0, l1, lA);
		IK::Segment_3 lB;
		LineLineOverlap(l1, l0, lB);

		IK::Segment_3 a(cgal_vector_util::MidPoint_(lA[0], lB[0]), cgal_vector_util::MidPoint_(lA[1], lB[1]));
		IK::Segment_3 b(cgal_vector_util::MidPoint_(lA[0], lB[1]), cgal_vector_util::MidPoint_(lA[1], lB[0]));

		result = (a.squared_length() > b.squared_length()) ? a : b;
	}

	inline double ClosestDistance(const IK::Point_3 point, CGAL_Polyline &s)
	{

		IK::Segment_3 segment(s[0], s[1]);
		double t;
		ClosestPointTo(point, segment, t);
		double closestDistance = CGAL::squared_distance(point, PointAt(segment, t));

		for (int i = 1; i < s.size() - 1; i++)
		{

			IK::Segment_3 segment_(s[i], s[i + 1]);
			ClosestPointTo(point, segment_, t);
			double closestDistanceTemp = CGAL::squared_distance(point, PointAt(segment_, t));
			if (closestDistanceTemp < closestDistance)
				closestDistance = closestDistanceTemp;
			if (closestDistance < GlobalToleranceSquare)
				break;
		}

		return closestDistance;
	}

	inline double closest_distance(const IK::Point_3 &point, CGAL_Polyline &s, int &edge)
	{

		edge = 0;
		IK::Segment_3 segment(s[0], s[1]);
		double closestDistance = 999999;

		for (int i = 0; i < s.size() - 1; i++)
		{

			IK::Segment_3 segment_(s[i], s[i + 1]);

			double t;
			ClosestPointTo(point, segment_, t);

			double closestDistanceTemp = std::abs(CGAL::squared_distance(point, PointAt(segment_, t)));
			if (closestDistanceTemp < closestDistance)
			{
				closestDistance = closestDistanceTemp;
				edge = i;
			}

			if (closestDistance < GlobalToleranceSquare)
				break;
		}

		return closestDistance;
	}

	inline double closest_distance_and_point(const IK::Point_3 &point, CGAL_Polyline &s, int &edge, IK::Point_3 &closest_point)
	{

		edge = 0;
		IK::Segment_3 segment(s[0], s[1]);
		double closestDistance = 999999;

		for (int i = 0; i < s.size() - 1; i++)
		{

			IK::Segment_3 segment_(s[i], s[i + 1]);

			double t;
			ClosestPointTo(point, segment_, t);

			closest_point = PointAt(segment_, t);
			double closestDistanceTemp = CGAL::squared_distance(point, closest_point);
			if (closestDistanceTemp < closestDistance)
			{
				closestDistance = closestDistanceTemp;
				edge = i;
			}

			if (closestDistance < GlobalToleranceSquare)
				break;
		}

		return closestDistance;
	}

	inline IK::Segment_3 LineLineOverlap(IK::Segment_3 &l0, IK::Segment_3 &l1)
	{

		double t[4];
		t[0] = 0;
		t[1] = 1;
		ClosestPointTo(l1[0], l0, t[2]);
		ClosestPointTo(l1[1], l0, t[3]);
		int n = sizeof(t) / sizeof(t[0]);
		std::sort(t, t + n);

		IK::Segment_3 l(PointAt(l0, t[1]), PointAt(l0, t[2]));

		return l;
	}

	inline bool IsClosed(CGAL_Polyline &input, double tolerance = 0.0001)
	{
		return cgal_vector_util::DistanceSquare(input.front(), input.back()) < tolerance;
		// IK::Point_3 p0(0, 0, 0);
		// IK::Point_3 p1(0, 0, 0);
		//double result =  cgal_vector_util::DistanceSquare(p0, p1);
		// return result < tolerance;
	}

	inline IK::Point_3 Center(CGAL_Polyline &input)
	{

		double x = 0, y = 0, z = 0;
		int n = input.size() - 1;

		for (int i = 0; i < n; i++)
		{
			x += input[i].x();
			y += input[i].y();
			z += input[i].z();
		}
		x /= n;
		y /= n;
		z /= n;

		IK::Point_3 p(x, y, z);
		return p;
	}

	inline IK::Vector_3 CenterVec(CGAL_Polyline &input)
	{

		double x = 0, y = 0, z = 0;
		int n = input.size() - 1;

		for (int i = 0; i < n; i++)
		{
			x += input[i].x();
			y += input[i].y();
			z += input[i].z();
		}
		x /= n;
		y /= n;
		z /= n;

		IK::Vector_3 p(x, y, z);
		return p;
	}

	inline void Duplicate(CGAL_Polyline &input, CGAL_Polyline &output)
	{

		//std::vector<int> newVec;
		//output.reserve(input.size());
		//std::copy(input.begin(), input.end(), std::back_inserter(output));

		output.clear();
		output.reserve(input.size());
		for (int i = 0; i < input.size(); i++)
		{
			output.push_back(IK::Point_3(input[i].x(), input[i].y(), input[i].z()));
		}
	}

	inline void Transform(CGAL_Polyline &input, CGAL::Aff_transformation_3<IK> &transform)
	{

		for (auto it = input.begin(); it != input.end(); ++it)
		{
			*it = it->transform(transform);
			//printf("CPP Transformed Point %d %d %d \n", it->x(), it->y(), it->z());
		}
	}

	inline void AveragePlane(CGAL_Polyline &polyline, IK::Vector_3 (&planeAxes)[4], bool closed = true)
	{

		//Origin
		planeAxes[0] = CenterVec(polyline); // IK::Vector_3(polyline[0].x(), polyline[0].y(), polyline[0].z());

		//XAxis
		planeAxes[1] = polyline[1] - polyline[0];
		cgal_vector_util::Unitize(planeAxes[1]);

		//ZAxis
		cgal_vector_util::AverageNormal(polyline, planeAxes[3], closed);

		//YAxis
		planeAxes[2] = CGAL::cross_product(planeAxes[3], planeAxes[1]);

		//printf("%f", planeAxes[1].squared_length());
		//printf("%f", planeAxes[2].squared_length());
		//printf("%f", planeAxes[3].squared_length());
		//printf("%f", CGAL::approximate_angle(planeAxes[1], planeAxes[2]));
		//printf("%f", CGAL::approximate_angle(planeAxes[1], planeAxes[3]));
		//printf("%f", CGAL::approximate_angle(planeAxes[2], planeAxes[3]));

		//GetOrthonormalVectors(planeAxes[3], planeAxes[1], planeAxes[2]);
	}

	inline bool LineTwoPlanes(IK::Segment_3 &line, IK::Plane_3 &plane0, IK::Plane_3 &plane1)
	{

		auto result0 = CGAL::intersection(line, plane0);
		auto result1 = CGAL::intersection(line, plane1);
		line = IK::Segment_3(*boost::get<IK::Point_3>(&*result0), *boost::get<IK::Point_3>(&*result1));

		//if (result) {
		//    if (const IK::Point_3* p = boost::get<IK::Point_3>(&*result)) {
		//        pts[count] = *p;
		//        //    CGAL_Debug(*p, true);
		//        count++;

		//        if (count == 2)
		//            break;
		//    }//if point type
		//}//result exists
	}

	inline bool PlanePolylineIntersection(CGAL_Polyline &polyline, IK::Plane_3 &plane, std::vector<IK::Point_3> &points, std::vector<int> &edge_ids)
	{

		//CGAL_Debug(polyline.size() - 1);
		for (int i = 0; i < polyline.size() - 1; i++)
		{
			//CGAL_Debug(1);
			IK::Segment_3 segment(polyline[i], polyline[i + 1]);
			//CGAL_Debug(2);
			//CGAL_Debug(segment.squared_length());
			//CGAL_Debug(plane.is_degenerate());
			//CGAL_Debug(CGAL::squared_distance(plane.projection(segment[0]), segment[0]));
			//CGAL_Debug(CGAL::squared_distance(plane.projection(segment[1]), segment[1]));

			if (CGAL::squared_distance(plane.projection(segment[0]), segment[0]) < GlobalToleranceSquare)
			{
				//CGAL_Debug(9999);
				return false;
			}

			if (CGAL::squared_distance(plane.projection(segment[1]), segment[1]) < GlobalToleranceSquare)
			{
				//CGAL_Debug(9999);
				return false;
			}

			//if(  CGAL::squared_distance(plane.projection(segment[0]), segment[0])  )

			const auto result = CGAL::intersection(segment, plane);

			//CGAL_Debug(3);
			if (result)
			{
				if (const IK::Point_3 *p = boost::get<IK::Point_3>(&*result))
				{
					points.emplace_back(*p);
					edge_ids.emplace_back(i);
				} //if point type
			}
			//CGAL_Debug(4);
		}
		//CGAL_Debug(5);
		return true;
	}

	inline int IsPointPairInside(
		CGAL_Polyline &polyline, IK::Plane_3 &plane,
		std::vector<IK::Point_3> &testPoints, std::vector<int> &testPointsID, double scale = 100000.0)
	{

		/////////////////////////////////////////////////////////////////////////////////////
		//Orient from 3D to 2D
		/////////////////////////////////////////////////////////////////////////////////////
		CGAL_Polyline a;
		cgal_polyline_util::Duplicate(polyline, a);

		/////////////////////////////////////////////////////////////////////////////////////
		//Create Transformation - Orient to XY
		/////////////////////////////////////////////////////////////////////////////////////
		CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(polyline[0], plane);
		Transform(a, xform_toXY);

		std::vector<IK::Point_3> pts(testPoints.size());
		for (int i = 0; i < testPoints.size(); i++)
		{
			pts[i] = IK::Point_3(testPoints[i].x(), testPoints[i].y(), testPoints[i].z());
			pts[i] = xform_toXY.transform(pts[i]);
		}

		/////////////////////////////////////////////////////////////////////////////////////
		//Convert to Clipper
		/////////////////////////////////////////////////////////////////////////////////////
		std::vector<ClipperLib::IntPoint> pathA(a.size() - 1);
		for (int i = 0; i < a.size() - 1; i++)
		{
			pathA[i] = ClipperLib::IntPoint(a[i].x() * scale, a[i].y() * scale);
			//printf("%f,%f,%f \n", a[i].x(), a[i].y(), a[i].z());
		}

		/////////////////////////////////////////////////////////////////////////////////////
		//Check if point is inside polyline
		/////////////////////////////////////////////////////////////////////////////////////

		//testPointsFlag.clear();
		//testPointsFlag.resize(testPoints.size());

		int count = 0;

		//printf("-\n");
		for (int i = 0; i < testPoints.size(); i++)
		{
			auto p = ClipperLib::IntPoint(pts[i].x() * scale, pts[i].y() * scale);

			int result = ClipperLib::PointInPolygon(p, pathA);
			//testPointsFlag[i] = result != 0;//0 - false, 1 true, -1 on boundary

			//if (result != 0) {
			//     printf("CPP Distance %f \n", ClosestDistance(pts[i], a));
			//    CGAL_Debug(pts[i],true);
			//}

			if (result != 0)
			{
				testPointsID.push_back(i);
				count++;
			}
		}
		//printf("-\n");

		return count;
	}

	inline bool PlanePolyline(CGAL_Polyline &c0, CGAL_Polyline &c1,
							  IK::Plane_3 &p0, IK::Plane_3 &p1,
							  IK::Segment_3 &line, std::pair<int, int> &pair)
	{ //, IK::Segment_3& cornerMaxLine

		//printf("H");
		bool debug = false;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Perform both events
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> pts0;
		std::vector<int> edge_ids_0;
		if (!PlanePolylineIntersection(c0, p1, pts0, edge_ids_0))
			return false;

		std::vector<IK::Point_3> pts1;
		std::vector<int> edge_ids_1;
		if (!PlanePolylineIntersection(c1, p0, pts1, edge_ids_1))
			return false;
		//printf(" %zi ", pts0.size());
		//printf(" %zi ", pts1.size());

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Check1: if there are 2 intersections
		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		if (pts0.size() < 2)
		{
			if (debug)
				printf("CPP PlanePolylineIntersection 0 Not2 \n");
			return false;
		}

		if (pts1.size() < 2)
		{
			if (debug)
				printf("CPP PlanePolylineIntersection 1 Not2 \n");
			return false;
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Check2: if any of points are in the other curve,
		//If one point is inside, perform second intersection else curve are inside curves
		//What if count is not 1 but 2
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::vector<int> ID1;
		int count0 = IsPointPairInside(c0, p0, pts1, ID1, GlobalClipperScale);

		std::vector<int> ID0;
		int count1 = IsPointPairInside(c1, p1, pts0, ID0, GlobalClipperScale);

		if (count0 == 0 && count1 == 0)
		{
			return false;
		}
		else if (std::abs(count0 - count1) == 2)
		{ //rectangle curve inside a rectangle curve

			line = count0 == 2 ? IK::Segment_3(pts0[0], pts0[1]) : IK::Segment_3(pts1[0], pts1[1]);
			pair = count0 == 2 ? std::pair<int, int>(edge_ids_0[0], edge_ids_0[1]) : std::pair<int, int>(edge_ids_1[0], edge_ids_1[1]);

			return true;
		}
		else if (count0 == 1 && count1 == 1)
		{
			line = IK::Segment_3(pts0[ID0[0]], pts1[ID1[0]]);
			pair = std::pair<int, int>(edge_ids_0[ID0[0]], edge_ids_1[ID1[0]]);
			//CGAL_Debug(4444);
			return true;
		}
		else if (count0 > 1 || count1 > 1)
		{

			std::vector<IK::Point_3> pts(ID0.size() + ID1.size());
			for (int i = 0; i < ID0.size(); i++)
				pts[i] = pts0[ID0[i]];

			for (int i = 0; i < ID1.size(); i++)
				pts[i + ID0.size()] = pts1[ID1[i]];

			CGAL::Bbox_3 AABB = CGAL::bbox_3(pts.begin(), pts.end(), IK());
			auto p0 = IK::Point_3(AABB.xmin(), AABB.ymin(), AABB.zmin());
			auto p1 = IK::Point_3(AABB.xmax(), AABB.ymax(), AABB.zmax());
			line = IK::Segment_3(p0, p1);
			int e0, e1;

			//Find edge ID - not optimized...
			for (int i = 0; i < ID0.size(); i++)
			{
				if (CGAL::squared_distance(p0, pts0[ID0[i]]) < 0.001 || CGAL::squared_distance(p1, pts0[ID0[i]]) < 0.001)
				{
					e0 = edge_ids_0[ID0[i]];
					break;
				}
			}

			for (int i = 0; i < ID1.size(); i++)
			{
				if (CGAL::squared_distance(p0, pts1[ID1[i]]) < 0.001 || CGAL::squared_distance(p1, pts1[ID1[i]]) < 0.001)
				{
					e1 = edge_ids_1[ID1[i]];
					break;
				}
			}
			pair = std::pair<int, int>(e0, e1);

			return true;
		}

		return false;
	}
	inline void MidLine(IK::Segment_3 &a, IK::Segment_3 &b, IK::Segment_3 &c)
	{

		IK::Point_3 p0;
		cgal_vector_util::MidPointConst(a[0], b[0], p0);

		IK::Point_3 p1;
		cgal_vector_util::MidPointConst(a[1], b[1], p1);

		c = IK::Segment_3(p0, p1);
	}

	inline void ExtendLine(IK::Segment_3 &l, double d0, double d1)
	{

		IK::Point_3 p0 = l[0];
		IK::Point_3 p1 = l[1];

		auto v = l.to_vector();
		cgal_vector_util::Unitize(v);

		l = IK::Segment_3(p0 - v * d0, p1 + v * d1);
	}

	inline void ScaleLine(IK::Segment_3 &l, double d0)
	{

		IK::Point_3 p0 = l[0];
		IK::Point_3 p1 = l[1];
		IK::Vector_3 v = p1 - p0;
		p0 += v * d0;
		p1 -= v * d0;
		//cgal_vector_util::MidVectorFromPoints(p0, p1, mid);

		//CGAL::Aff_transformation_3<IK> T0 (CGAL::TRANSLATION, -mid);
		//CGAL::Aff_transformation_3<IK> S (CGAL::SCALING, d0);
		//CGAL::Aff_transformation_3<IK> T1(CGAL::TRANSLATION, mid);
		//CGAL::Aff_transformation_3<IK> X = T1 * S * T0;

		////auto v = l.to_vector();
		////cgal_vector_util::Unitize(v);
		////l.squared_length();

		l = IK::Segment_3(p0, p1);
	}

	inline void Extend(CGAL_Polyline &pline, int sID, double dist0, double dist1, double proportion0 = 0, double proportion1 = 0)
	{

		IK::Point_3 p0 = pline[sID];
		IK::Point_3 p1 = pline[sID + 1];
		IK::Vector_3 v = p1 - p0;

		//Either scale or extend polyline segments
		if (proportion0 != 0 || proportion1 != 0)
		{
			p0 += v * proportion0;
			p1 -= v * proportion1;
		}
		else
		{
			cgal_vector_util::Unitize(v);
			p0 += v * dist0;
			p1 -= v * dist1;
		}

		pline[sID] = p0;
		pline[sID + 1] = p1;

		if (sID == 0)
			pline[pline.size() - 1] = pline[0];
		else if (sID == pline.size() - 1)
			pline[0] = pline[pline.size() - 1];
	}

	inline void move(CGAL_Polyline &polyline, IK::Vector_3 &v)
	{
		CGAL::Aff_transformation_3<IK> T(CGAL::TRANSLATION, v);
		Transform(polyline, T);
	}

	inline bool is_clockwise(CGAL_Polyline &polyline, IK::Plane_3 plane)
	{

		//copy
		CGAL_Polyline cp;
		Duplicate(polyline, cp);

		//transform
		CGAL::Aff_transformation_3<IK> xform_toXY = cgal_xform_util::PlaneToXY(polyline[0], plane);
		Transform(cp, xform_toXY);

		//check if closed
		if (!IsClosed(polyline))
		{
			cp.emplace_back(cp[0]);
		}

		//check windind, negative anti-clockwise
		double num = 0;
		for (int i = 0; i < cp.size() - 1; i++)
		{
			auto item = cp[i + 1];
			double x = item.hx();
			item = cp[i];
			double x1 = x - item.hx();
			item = cp[i + 1];
			double y = item.hy();
			item = cp[i];
			double y1 = y + item.hy();
			num = num + x1 * y1;
		}
		return num > 0;
	}
}
