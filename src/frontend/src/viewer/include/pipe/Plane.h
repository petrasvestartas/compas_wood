///////////////////////////////////////////////////////////////////////////////
// Plane.h
// =======
// class for a 3D plane with normal vector (a,b,c) and a point (x0,y0,z0)
// ax + by + cz + d = 0,  where d = -(ax0 + by0 + cz0)
//
// NOTE:
// 1. The default plane is z = 0 (a plane on XY axis)
// 2. The distance is the length from the origin to the plane
//
// Dependencies: Vector3, Line
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-01-19
// UPDATED: 2016-04-14
///////////////////////////////////////////////////////////////////////////////

#pragma once
//#include "../../../stdafx.h"

#include "Vectors.h"
#include "Line.h"

class Plane
{
public:
	// ctor/dtor
	Plane();
	Plane(float a, float b, float c, float d);          // 4 coeff of plane equation
	Plane(const Vector3& normal, const Vector3& point); // a point on the plane and normal vector
	~Plane() {}

	// debug
	void printSelf() const;

	// setters/getters
	void set(float a, float b, float c, float d);
	void set(const Vector3& normal, const Vector3& point);  // set with  a point on the plane and normal
	const Vector3& getNormal() const { return normal; }
	float getD() const { return d; }                        // return 4th coefficient
	float getNormalLength() const { return normalLength; }  // return length of normal
	float getDistance() const { return distance; };         // return distance from the origin
	float getDistance(const Vector3& point);                // return distance from the point

	// convert plane equation with unit normal vector
	void normalize();

	// for intersection
	Vector3 intersect(const Line& line) const;              // intersect with a line
	Line intersect(const Plane& plane) const;               // intersect with another plane
	bool isIntersected(const Line& line) const;
	bool isIntersected(const Plane& plane) const;

protected:

private:
	Vector3 normal;     // normal vector of a plane
	float d;            // coefficient of constant term: d = -(a*x0 + b*y0 + c*z0)
	float normalLength; // length of normal vector
	float distance;     // distance from origin to plane
};

///////////////////////////////////////////////////////////////////////////////
// ctors
// default plane is z = 0 (a plane on XY-axis)
///////////////////////////////////////////////////////////////////////////////
inline Plane::Plane() : normal(Vector3(0, 0, 1)), d(0), normalLength(1), distance(0)
{
}

inline Plane::Plane(float a, float b, float c, float d)
{
	set(a, b, c, d);
}

inline Plane::Plane(const Vector3& normal, const Vector3& point)
{
	set(normal, point);
}

///////////////////////////////////////////////////////////////////////////////
// print itself with 4 coefficients of normalized plane equation
// aX + bY + cZ + d = 0
// where (a, b, c) is unit normal vector, d = -(ax0 + by0 + cz0)
///////////////////////////////////////////////////////////////////////////////
inline void Plane::printSelf() const
{
	std::cout << "Plane(" << normal.x << ", " << normal.y << ", " << normal.z
		<< ", " << d << ")" << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
inline void Plane::set(float a, float b, float c, float d)
{
	normal.set(a, b, c);
	this->d = d;

	// compute distance
	normalLength = sqrtf(a * a + b * b + c * c);
	distance = -d / normalLength;
}

inline void Plane::set(const Vector3& normal, const Vector3& point)
{
	this->normal = normal;
	normalLength = normal.length();
	d = -normal.dot(point);         // -(a*x0 + b*y0 + c*z0)
	distance = -d / normalLength;
}

///////////////////////////////////////////////////////////////////////////////
// compute the shortest distance from a given point P to the plane
// Note: The distance is signed. If the distance is negative, the point is in
// opposite side of the plane.
//
// D = (a * Px + b * Py + c * Pz + d) / sqrt(a*a + b*b + c*c)
// reference: www.songho.ca/math/plane.html
///////////////////////////////////////////////////////////////////////////////
inline float Plane::getDistance(const Vector3& point)
{
	float dot = normal.dot(point);
	return (dot + d) / normalLength;
}

///////////////////////////////////////////////////////////////////////////////
// normalize
// divide each coefficient by the length of normal
///////////////////////////////////////////////////////////////////////////////
inline void Plane::normalize()
{
	float lengthInv = 1.0f / normalLength;
	normal *= lengthInv;
	normalLength = 1.0f;
	d *= lengthInv;
	distance = -d;
}

///////////////////////////////////////////////////////////////////////////////
// find the intersect point
// substitute a point on the line to the plane equation, then solve for alpha
// a point on a line: (x0 + x*t, y0 + y*t, z0 + z*t)
// plane: a*X + b*Y + c*Z + d = 0
//
// a*(x0 + x*t) + b*(y0 + y*t) + c*(z0 + z*t) + d = 0
// a*x0 + a*x*t + b*y0 + b*y*t + c*z0 + c*z*t + d = 0
// (a*x + b*x + c*x)*t = -(a*x0 + b*y0 + c*z0 + d)
//
// t = -(a*x0 + b*y0 + c*z0 + d) / (a*x + b*x + c*x)
///////////////////////////////////////////////////////////////////////////////
inline Vector3 Plane::intersect(const Line& line) const
{
	// from line = p + t * v
	Vector3 p = line.getPoint();        // (x0, y0, z0)
	Vector3 v = line.getDirection();    // (x,  y,  z)

	// dot products
	float dot1 = normal.dot(p);         // a*x0 + b*y0 + c*z0
	float dot2 = normal.dot(v);         // a*x + b*y + c*z

	// if denominator=0, no intersect
	if (dot2 == 0)
		return Vector3(NAN, NAN, NAN);

	// find t = -(a*x0 + b*y0 + c*z0 + d) / (a*x + b*y + c*z)
	float t = -(dot1 + d) / dot2;

	// find intersection point
	return p + (v * t);
}

///////////////////////////////////////////////////////////////////////////////
// find the intersection line of 2 planes
// P1: N1 dot p + d1 = 0 (a1*X + b1*Y + c1*Z + d1 = 0)
// P2: N2 dot p + d2 = 0 (a2*X + b2*Y + c2*Z + d2 = 0)
//
// L: p0 + a*V where
// V is the direction vector of intersection line = (a1,b1,c1) x (a2,b2,c2)
// p0 is a point, which is on the L and both P1 and P2 as well
//
// p0 can be found by solving a linear system of 3 planes
// P1: N1 dot p + d1 = 0     (given)
// P2: N2 dot p + d2 = 0     (given)
// P3: V dot p = 0           (chosen where d3=0)
//
// Use the formula for intersecting 3 planes to find p0;
// p0 = ((-d1*N2 + d2*N1) x V) / V dot V
///////////////////////////////////////////////////////////////////////////////
inline Line Plane::intersect(const Plane& rhs) const
{
	// find direction vector of the intersection line
	Vector3 v = normal.cross(rhs.getNormal());

	// if |direction| = 0, 2 planes are parallel (no intersect)
	// return a line with NaN
	if (v.x == 0 && v.y == 0 && v.z == 0)
		return Line(Vector3(NAN, NAN, NAN), Vector3(NAN, NAN, NAN));

	// find a point on the line, which is also on both planes
	// choose simple plane where d=0: ax + by + cz = 0
	float dot = v.dot(v);                       // V dot V
	Vector3 n1 = rhs.getD() * normal;           // d2 * N1
	Vector3 n2 = -d * rhs.getNormal();          //-d1 * N2
	Vector3 p = (n1 + n2).cross(v) / dot;       // (d2*N1-d1*N2) X V / V dot V

	return Line(v, p);
}

///////////////////////////////////////////////////////////////////////////////
// determine if it intersects with the line
///////////////////////////////////////////////////////////////////////////////
inline bool Plane::isIntersected(const Line& line) const
{
	// direction vector of line
	Vector3 v = line.getDirection();

	// dot product with normal of the plane
	float dot = normal.dot(v);  // a*Vx + b*Vy + c*Vz

	if (dot == 0)
		return false;
	else
		return true;
}

///////////////////////////////////////////////////////////////////////////////
// determine if it intersects with the other plane
///////////////////////////////////////////////////////////////////////////////
inline bool Plane::isIntersected(const Plane& plane) const
{
	// check if 2 plane normals are same direction
	Vector3 cross = normal.cross(plane.getNormal());
	if (cross.x == 0 && cross.y == 0 && cross.z == 0)
		return false;
	else
		return true;
}