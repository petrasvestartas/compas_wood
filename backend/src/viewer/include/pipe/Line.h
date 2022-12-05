///////////////////////////////////////////////////////////////////////////////
// Line.h
// ======
// class to construct a line with parametric form
// Line = p + aV (a point and a direction vector on the line)
//
// Dependency: Vector2, Vector3
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2015-12-18
// UPDATED: 2020-07-17
///////////////////////////////////////////////////////////////////////////////

#pragma once
//#include "../../../stdafx.h"
#include <cmath>
#include "Vectors.h"

class Line
{
public:
	// ctor/dtor
	Line() : direction(Vector3(0, 0, 0)), point(Vector3(0, 0, 0)) {}
	Line(const Vector3& v, const Vector3& p) : direction(v), point(p) {}    // with 3D direction and a point
	Line(const Vector2& v, const Vector2& p);                               // with 2D direction and a point
	Line(float slope, float intercept);                                     // with 2D slope-intercept form
	~Line() {};

	// getters/setters
	void set(const Vector3& v, const Vector3& p);               // from 3D
	void set(const Vector2& v, const Vector2& p);               // from 2D
	void set(float slope, float intercept);                     // from slope-intercept form
	void setPoint(Vector3& p) { point = p; }
	void setDirection(const Vector3& v) { direction = v; }
	const Vector3& getPoint() const { return point; }
	const Vector3& getDirection() const { return direction; }
	void printSelf();

	// find intersect point with other line
	Vector3 intersect(const Line& line);
	bool isIntersected(const Line& line);

protected:

private:
	Vector3 direction;
	Vector3 point;
};

///////////////////////////////////////////////////////////////////////////////
// ctor
// convert 2D slope-intercept form to parametric form
///////////////////////////////////////////////////////////////////////////////
inline Line::Line(float slope, float intercept)
{
	set(slope, intercept);
}

///////////////////////////////////////////////////////////////////////////////
// ctor with 2D direction and point
///////////////////////////////////////////////////////////////////////////////
inline Line::Line(const Vector2& direction, const Vector2& point)
{
	set(direction, point);
}

///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
inline void Line::set(const Vector3& v, const Vector3& p)
{
	this->direction = v;
	this->point = p;
}

inline void Line::set(const Vector2& v, const Vector2& p)
{
	// convert 2D to 3D
	this->direction = Vector3(v.x, v.y, 0);
	this->point = Vector3(p.x, p.y, 0);
}

inline void Line::set(float slope, float intercept)
{
	// convert slope-intercept form (2D) to parametric form (3D)
	this->direction = Vector3(1, slope, 0);
	this->point = Vector3(0, intercept, 0);
}

///////////////////////////////////////////////////////////////////////////////
// debug
///////////////////////////////////////////////////////////////////////////////
inline void Line::printSelf()
{
	std::cout << "Line\n"
		<< "====\n"
		<< "Direction: " << this->direction << "\n"
		<< "    Point: " << this->point << std::endl;
}

///////////////////////////////////////////////////////////////////////////////
// find the intersection point with the other line.
// If no intersection, return a point with NaN in it.
//
// Line1 = p1 + aV1 (this)
// Line2 = p2 + bV2 (other)
//
// Intersect:
// p1 + aV1 = p2 + bV2
//      aV1 = (p2-p1) + bV2
//   aV1xV2 = (p2-p1)xV2
//        a = (p2-p1)xV2 / (V1xV2)
//        a = ((p2-p1)xV2).(V1xV2) / (V1xV2).(V1xV2)
///////////////////////////////////////////////////////////////////////////////
inline Vector3 Line::intersect(const Line& line)
{
	const Vector3 v2 = line.getDirection();
	const Vector3 p2 = line.getPoint();
	Vector3 result = Vector3(NAN, NAN, NAN);    // default with NaN

	// find v3 = (p2 - p1) x V2
	Vector3 v3 = (p2 - point).cross(v2);

	// find v4 = V1 x V2
	Vector3 v4 = direction.cross(v2);

	// find (V1xV2) . (V1xV2)
	float dot = v4.dot(v4);

	// if both V1 and V2 are same direction, return NaN point
	if (dot == 0)
		return result;

	// find a = ((p2-p1)xV2).(V1xV2) / (V1xV2).(V1xV2)
	float alpha = v3.dot(v4) / dot;

	/*
	// if both V1 and V2 are same direction, return NaN point
	if(v4.x == 0 && v4.y == 0 && v4.z == 0)
		return result;

	float alpha = 0;
	if(v4.x != 0)
		alpha = v3.x / v4.x;
	else if(v4.y != 0)
		alpha = v3.y / v4.y;
	else if(v4.z != 0)
		alpha = v3.z / v4.z;
	else
		return result;
	*/

	// find intersect point
	result = point + (alpha * direction);
	return result;
}

///////////////////////////////////////////////////////////////////////////////
// determine if it intersects with the other line
///////////////////////////////////////////////////////////////////////////////
inline bool Line::isIntersected(const Line& line)
{
	// if 2 lines are same direction, the magnitude of cross product is 0
	Vector3 v = this->direction.cross(line.getDirection());
	if (v.x == 0 && v.y == 0 && v.z == 0)
		return false;
	else
		return true;
}