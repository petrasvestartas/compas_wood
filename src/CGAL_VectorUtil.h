#pragma once
#include <math.h>
#include "compas.h"
//#include "AxisPlane.h"
//#include "CGAL_PolylineUtil.h"




namespace CGAL_VectorUtil {






	inline double DotProduct(const IK::Vector_3& a, const IK::Vector_3& b) {
		// inner (dot) product between 3d vectors
		return (a.x() * b.x() + a.y() * b.y() + a.z() * b.z());
	}


	inline double DistanceSquareVec(IK::Vector_3& p0, IK::Vector_3& p1) {

		IK::Vector_3  p(p0.x() - p1.x(), p0.y() - p1.y(), p0.z() - p1.z());
		return p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
	}


	inline double DistanceSquare(IK::Point_3& p0, IK::Point_3& p1) {

		IK::Point_3  p(p0.x() - p1.x(), p0.y() - p1.y(), p0.z() - p1.z());
		return p.x() * p.x() + p.y() * p.y() + p.z() * p.z();
	}

	inline double Distance(IK::Point_3& p0, IK::Point_3& p1) {

		IK::Point_3  p(p0.x() - p1.x(), p0.y() - p1.y(), p0.z() - p1.z());
		return std::sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
	}


	inline double DistanceSquare2D(IK::Point_3& p0, IK::Point_3& p1) {

		IK::Point_3  p(p0.x() - p1.x(), p0.y() - p1.y(), p0.z() - p1.z());
		return p.x() * p.x() + p.y() * p.y();
	}


	inline double Length(double x, double y, double z) {
		//double ON_DBL_MIN = 2.22507385850720200e-308;

		double len;
		x = fabs(x);
		y = fabs(y);
		z = fabs(z);
		if (y >= x && y >= z) {
			len = x; x = y; y = len;
		} else if (z >= x && z >= y) {
			len = x; x = z; z = len;
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

	inline double LengthVec(IK::Vector_3& v) {
		return Length(v.x(), v.y(), v.z());
	}

	inline double LengthVecConst(const IK::Vector_3& v) {
		return Length(v.x(), v.y(), v.z());
	}


	inline double Length2(IK::Vector_2 v) {

		double x = v.x();
		double y = v.y();

		double len;
		x = fabs(x);
		y = fabs(y);
		if (y > x) {
			len = x; x = y; y = len;
		}

		// 15 September 2003 Dale Lear
		//     For small denormalized doubles (positive but smaller
		//     than DBL_MIN), some compilers/FPUs set 1.0/fx to +INF.
		//     Without the ON_DBL_MIN test we end up with
		//     microscopic vectors that have infinte length!
		//
		//     This code is absolutely necessary.  It is a critical
		//     part of the bug fix for RR 11217.
		if (x > ON_DBL_MIN) {
			y /= x;
			len = x * sqrt(1.0 + y * y);
		} else if (x > 0.0 && ON_IS_FINITE(x))
			len = x;
		else
			len = 0.0;

		return len;

	}




	template <typename Vector>
	Vector normalize(Vector v) {
		auto length = CGAL::sqrt(v * v);
		if (length != 0) { // a null vector cannot be normalized
			v = v / length;
		}
		return v;
	};

	//inline void RotatePlane(
	//    double angle,           // angle in radians
	//    const ON_3dVector& axis // axis of rotation
	//)
	//{
	//    return Rotate(sin(angle), cos(angle), axis);
	//}

	inline IK::Point_3 ToPoint(IK::Vector_3& a) {
		return IK::Point_3(a.x(), a.y(), a.z());
	}

	inline void MidVector(IK::Vector_3& a, IK::Vector_3& b, IK::Vector_3& output) {
		output = IK::Vector_3(
			(a.x() + b.x()) * 0.5,
			(a.y() + b.y()) * 0.5,
			(a.z() + b.z()) * 0.5
		);
	}

	inline void MidVectorFromPoints(IK::Point_3& a, IK::Point_3& b, IK::Vector_3& output) {
		output = IK::Vector_3(
			(a.x() + b.x()) * 0.5,
			(a.y() + b.y()) * 0.5,
			(a.z() + b.z()) * 0.5
		);
	}

	inline void MidPoint(IK::Point_3& a, IK::Point_3& b, IK::Point_3& output) {
		output = IK::Point_3(
			(a.x() + b.x()) * 0.5,
			(a.y() + b.y()) * 0.5,
			(a.z() + b.z()) * 0.5
		);
	}

	inline IK::Point_3 MidPoint_(IK::Point_3 a, IK::Point_3 b) {
		return IK::Point_3(
			(a.x() + b.x()) * 0.5,
			(a.y() + b.y()) * 0.5,
			(a.z() + b.z()) * 0.5
		);
	}

	inline void MidPointConst(const IK::Point_3& a, const IK::Point_3& b, IK::Point_3& output) {
		output = IK::Point_3(
			(a.x() + b.x()) * 0.5,
			(a.y() + b.y()) * 0.5,
			(a.z() + b.z()) * 0.5
		);
	}



	inline void FlipPlane(IK::Vector_3(&planeAxes)[4]) {
		IK::Vector_3 v = planeAxes[1];
		planeAxes[1] = planeAxes[2];
		planeAxes[2] = v;
		planeAxes[3] *= -1;

	}

	inline IK::Vector_3 ToVectorFromDirection(IK::Direction_3& a) {
		return IK::Vector_3(a.dx(), a.dy(), a.dz());
	}

	inline IK::Vector_3 ToVector(IK::Point_3& a) {
		return IK::Vector_3(a.x(), a.y(), a.z());
	}

	inline void ChangePlane(IK::Vector_3& a, IK::Vector_3& b, IK::Vector_3& c, IK::Vector_3& d, IK::Vector_3(&planeAxes)[4]) {
		planeAxes[0] = a;
		planeAxes[1] = b;
		planeAxes[2] = c;
		planeAxes[3] = d;
	}

	inline void ChangeBox(IK::Vector_3& a, IK::Vector_3& b, IK::Vector_3& c, IK::Vector_3& d, IK::Vector_3& e, IK::Vector_3(&box)[5]) {
		box[0] = a;
		box[1] = b;
		box[2] = c;
		box[3] = d;
		box[4] = e;
	}




	inline bool Unitize(IK::Vector_3& v) {

		bool rc = false;
		// Since x,y,z are floats, d will not be denormalized and the
		// ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.


		double d = Length(v.x(), v.y(), v.z());
		if (d > 0.0) {
			double dx = v.x();
			double dy = v.y();
			double dz = v.z();
			v = IK::Vector_3(
				(dx / d),
				(dy / d),
				(dz / d)
			);
			rc = true;
		}
		return rc;
	}

	inline IK::Vector_3 Unit(IK::Vector_3& v) {

		bool rc = false;
		// Since x,y,z are floats, d will not be denormalized and the
		// ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.


		double d = Length(v.x(), v.y(), v.z());
		if (d > 0.0) {
			double dx = v.x();
			double dy = v.y();
			double dz = v.z();
			return IK::Vector_3(
				(dx / d),
				(dy / d),
				(dz / d)
			);
			// rc = true;
		}


		return v;
	}

	inline bool Unitize2(IK::Vector_2& v) {

		bool rc = false;
		// Since x,y,z are floats, d will not be denormalized and the
		// ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.


		double d = Length2(v);
		if (d > 0.0) {
			double dx = v.x();
			double dy = v.y();
			v = IK::Vector_2(
				(dx / d),
				(dy / d)
			);
			rc = true;
		}
		return rc;
	}




	//inline bool GetOrthonormalVectors(IK::Vector_3& ZAxis, IK::Vector_3& XAxis, IK::Vector_3& YAxis) {

	//    bool b = Unitize(ZAxis);

	//    //PerpendicularTo(XAxis, ZAxis);//Make XAxis perpendicular to ZAxis
	//    Unitize(XAxis);

	//    YAxis = CGAL::cross_product(ZAxis, XAxis);
	//    Unitize(YAxis);
	//    return b;
	//}

	inline void AverageNormal(CGAL_Polyline& p, IK::Vector_3& averageNormal, bool closed = true, bool unitize = false) {

		int len = closed ? p.size() - 1 : p.size();
		averageNormal = IK::Vector_3(0, 0, 0);


		for (int i = 0; i < len; i++) {
			int num = ((i - 1) + len) % len;
			int item1 = ((i + 1) + len) % len;
			averageNormal = averageNormal + CGAL::cross_product(p[i] - p[num], p[item1] - p[i]);
		}

		if (unitize)
			Unitize(averageNormal);

		//return &vector3d;
	}





	inline double LengthSquared(IK::Vector_3 v) {
		return (v.x() * v.x() + v.y() * v.y() + v.z() * v.z());
	}




	inline void PlaneFromFourPoints(IK::Point_3& a, IK::Point_3& b, IK::Point_3& c, IK::Point_3& d, IK::Vector_3(&planeAxes)[4]) {
		// a----b
		// c----d
		MidVectorFromPoints(a, d, planeAxes[0]);

		planeAxes[1] = b - a;
		Unitize(planeAxes[1]);

		planeAxes[2] = a - c;
		Unitize(planeAxes[2]);

		planeAxes[3] = CGAL::cross_product(planeAxes[1], planeAxes[2]);
		Unitize(planeAxes[3]);

		planeAxes[2] = CGAL::cross_product(planeAxes[3], planeAxes[1]);
		Unitize(planeAxes[2]);
	}



	inline int IsParallelTo(
		// returns  1: this and other vectors are and parallel
		//         -1: this and other vectors are anti-parallel
		//          0: this and other vectors are not parallel
		//             or at least one of the vectors is zero
		IK::Vector_3& v0,
		IK::Vector_3& v1,

		double angle_tolerance // (default=ON_DEFAULT_ANGLE_TOLERANCE) radians
	) //const
	{
		int rc = 0;

		double ll = LengthVec(v0) * LengthVec(v1);
		if (ll > 0.0) {
			const double cos_angle = (v0.x() * v1.x() + v0.y() * v1.y() + v0.z() * v1.z()) / ll;
			const double cos_tol = cos(angle_tolerance);
			if (cos_angle >= cos_tol)
				rc = 1;
			else if (cos_angle <= -cos_tol)
				rc = -1;
		}
		return rc;
	}




	inline bool PerpendicularTo(IK::Vector_3& v, IK::Vector_3& result) {
		//bool rc = false;
		int i, j, k;
		double a, b;
		k = 2;
		if (fabs(v.y()) > fabs(v.x())) {
			if (fabs(v.z()) > fabs(v.y())) {
				// |v.z| > |v.y| > |v.x|
				i = 2;
				j = 1;
				k = 0;
				a = v.z();
				b = -v.y();
			} else if (fabs(v.z()) >= fabs(v.x())) {
				// |v.y| >= |v.z| >= |v.x|
				i = 1;
				j = 2;
				k = 0;
				a = v.y();
				b = -v.z();
			} else {
				// |v.y| > |v.x| > |v.z|
				i = 1;
				j = 0;
				k = 2;
				a = v.y();
				b = -v.x();
			}
		} else if (fabs(v.z()) > fabs(v.x())) {
			// |v.z| > |v.x| >= |v.y|
			i = 2;
			j = 0;
			k = 1;
			a = v.z();
			b = -v.x();
		} else if (fabs(v.z()) > fabs(v.y())) {
			// |v.x| >= |v.z| > |v.y|
			i = 0;
			j = 2;
			k = 1;
			a = v.x();
			b = -v.z();
		} else {
			// |v.x| >= |v.y| >= |v.z|
			i = 0;
			j = 1;
			k = 2;
			a = v.x();
			b = -v.y();
		}

		//double* this_v = &x;
		double number[] = { 0.0,0.0,0.0 };
		number[i] = b;
		number[j] = a;
		number[k] = 0.0;
		result = IK::Vector_3(number[0], number[1], number[2]);

		return (a != 0.0) ? true : false;
	}


	inline bool PerpendicularToPoints(
		const IK::Point_3& P0, const IK::Point_3& P1, const IK::Point_3& P2, IK::Vector_3& result
	) {
		// Find a the unit normal to a triangle defined by 3 points
		result = IK::Vector_3(0, 0, 0);

		IK::Vector_3 V0 = P2 - P1;
		IK::Vector_3 V1 = P0 - P2;
		IK::Vector_3 V2 = P1 - P0;

		IK::Vector_3 N0 = CGAL::cross_product(V1, V2);
		if (!Unitize(N0))
			return false;
		IK::Vector_3 N1 = CGAL::cross_product(V2, V0);
		if (!Unitize(N1))
			return false;
		IK::Vector_3 N2 = CGAL::cross_product(V0, V1);
		if (!Unitize(N2))
			return false;

		const double s0 = 1.0 / LengthVec(V0);
		const double s1 = 1.0 / LengthVec(V1);
		const double s2 = 1.0 / LengthVec(V2);

		// choose normal with smallest total error
		const double e0 = s0 * fabs(DotProduct(N0, V0)) + s1 * fabs(DotProduct(N0, V1)) + s2 * fabs(DotProduct(N0, V2));
		const double e1 = s0 * fabs(DotProduct(N1, V0)) + s1 * fabs(DotProduct(N1, V1)) + s2 * fabs(DotProduct(N1, V2));
		const double e2 = s0 * fabs(DotProduct(N2, V0)) + s1 * fabs(DotProduct(N2, V1)) + s2 * fabs(DotProduct(N2, V2));

		if (e0 <= e1) {
			if (e0 <= e2) {
				result = N0;
			} else {
				result = N2;
			}
		} else if (e1 <= e2) {
			result = N1;
		} else {
			result = N2;
		}

		return true;
	}





	inline double GetAngle90(IK::Vector_3& x, IK::Vector_3& y) {

		if (IsParallelTo(x, y, 0.001) == 0) {
			double angle = CGAL::approximate_angle(x, y);//in degrees
			return 90 - std::abs(angle - 90);
		} else {
			return 0;
		}
	}




}