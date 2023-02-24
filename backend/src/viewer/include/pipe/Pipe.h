///////////////////////////////////////////////////////////////////////////////
// Pipe.h
// ======
// base contour following a path
// The contour is a 2D shape on XY plane.
//
// Dependencies: Vector3, Plane, Line, Matrix4
//
//  AUTHOR: Song ho Ahn (song.ahn@gmail.com)
// CREATED: 2016-04-16
// UPDATED: 2016-04-29
///////////////////////////////////////////////////////////////////////////////
#pragma once
//#include "../../../stdafx.h"
#include "Matrices.h"
#include "Line.h"
#include "Plane.h"
#include "Vectors.h"

class Pipe
{
public:

	std::vector<glm::vec3> v;
	std::vector<glm::vec3> n;
	std::vector<glm::uvec3> f;

	// ctor/dtor
	std::vector<Vector3>  buildCircle(float radius, int steps);
	Pipe();
	Pipe(const std::vector<Vector3>& pathPoints, int ring_count, const std::vector<float>& radii);//const std::vector<Vector3>& contourPoints,
	~Pipe() {}

	// setters/getters
	void set(const std::vector<Vector3>& pathPoints, int ring_count, const std::vector<float>& radii);//const std::vector<Vector3>& contourPoints,
	void setPath(const std::vector<Vector3>& pathPoints);
	void setContour(const std::vector<Vector3>& contourPoints);
	void addPathPoint(const Vector3& point);

	int getPathCount() const { return (int)path.size(); }
	const std::vector<Vector3>& getPathPoints() const { return path; }
	const Vector3& getPathPoint(int index) const { return path.at(index); }
	int getContourCount() const { return (int)contours.size(); }
	const std::vector< std::vector<Vector3> >& getContours() const { return contours; }
	const std::vector<Vector3>& getContour(int index) const { return contours.at(index); }
	const std::vector< std::vector<Vector3> >& getNormals() const { return normals; }
	const std::vector<Vector3>& getNormal(int index) const { return normals.at(index); }

protected:

private:
	// member functions
	void generateContours();
	void scale();
	void transformFirstContour();
	void get_mesh_data();
	std::vector<Vector3> projectContour(int fromIndex, int toIndex);
	std::vector<Vector3> computeContourNormal(int pathIndex);

	std::vector<Vector3> path;
	std::vector<Vector3> contour;
	std::vector<float> radii;
	std::vector< std::vector<Vector3> > contours;
	std::vector< std::vector<Vector3> > normals;
};

inline void Pipe::setPath(const std::vector<Vector3>& pathPoints)
{
	this->path = pathPoints;
	generateContours();
}

inline void Pipe::setContour(const std::vector<Vector3>& contourPoints)
{
	this->contour = contourPoints;
	generateContours();
}

///////////////////////////////////////////////////////////////////////////////
// add a new path point at the end of the path list
///////////////////////////////////////////////////////////////////////////////
inline void Pipe::addPathPoint(const Vector3& point)
{
	// add it to path first
	path.push_back(point);

	auto count = path.size();
	if (count == 1)
	{
		transformFirstContour();
		normals.push_back(computeContourNormal(0));
	}
	else if (count == 2)
	{
		contours.push_back(projectContour(0, 1));
		normals.push_back(computeContourNormal(1));
	}
	else
	{
		// add dummy to match same # of contours/normals and path
		std::vector<Vector3> dummy;
		contours.push_back(dummy);
		normals.push_back(dummy);

		// re-project the previous contour
		contours[count - (size_t)2] = projectContour((int)count - 3, (int)count - 2);
		normals[count - (size_t)2] = computeContourNormal((int)count - 2);

		// compute for new contour
		contours[count - (size_t)1] = projectContour((int)count - 2, (int)count - 1);
		normals[count - (size_t)1] = computeContourNormal((int)count - 1);
	}
}

///////////////////////////////////////////////////////////////////////////////
// build countour vertex and normal list on each path point
///////////////////////////////////////////////////////////////////////////////
inline void Pipe::generateContours()
{
	// reset
	contours.clear();
	normals.clear();

	// path must have at least a point
	if (path.size() < 1)
		return;

	// rotate and translate the contour to the first path point
	transformFirstContour();
	contours.push_back(this->contour);
	normals.push_back(computeContourNormal(0));

	// project contour to the plane at the next path point
	int count = (int)path.size();
	for (int i = 1; i < count; ++i)
	{
		contours.push_back(projectContour(i - 1, i));
		normals.push_back(computeContourNormal(i));
	}
}

///////////////////////////////////////////////////////////////////////////////
// project a contour to a plane at the path point
///////////////////////////////////////////////////////////////////////////////
inline std::vector<Vector3> Pipe::projectContour(int fromIndex, int toIndex)
{
	//std::cout << fromIndex << " " << toIndex << std::endl;
	Vector3 dir1, dir2, normal;
	Line line;

	dir1 = path[toIndex] - path[fromIndex];
	if (toIndex == (int)path.size() - 1)
		dir2 = dir1;
	else
		dir2 = path[static_cast<int64_t>(toIndex) + static_cast<int64_t>(1)] - path[static_cast<int64_t>(toIndex)];

	normal = dir1 + dir2;               // normal vector of plane at toIndex
	Plane plane(normal, path[toIndex]);

	// project each vertex of contour to the plane
	std::vector<Vector3>& fromContour = contours[fromIndex];
	std::vector<Vector3> toContour;
	int count = (int)fromContour.size();

	for (int i = 0; i < count; ++i)
	{
		line.set(dir1, fromContour[i]);
		Vector3 intersection_point = plane.intersect(line);
		//intersection_point = intersection_point * (matrix_translate_a * matrix_scale * matrix_translate_b);

		toContour.emplace_back(intersection_point);
	}

	return toContour;
}

///////////////////////////////////////////////////////////////////////////////
//scale contours by radii
///////////////////////////////////////////////////////////////////////////////
inline void Pipe::scale()
{
	for (int i = 0; i < this->contours.size(); i++) {
		for (int j = 0; j < this->contours[i].size(); j++) {
			//print(this->contours[i][j]);
			this->contours[i][j] = this->contours[i][j] - this->path[i];
			this->contours[i][j] = this->contours[i][j] * this->radii[i];
			this->contours[i][j] = this->contours[i][j] + this->path[i];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// transform the contour at the first path point
///////////////////////////////////////////////////////////////////////////////
inline void Pipe::transformFirstContour()
{
	int pathCount = (int)path.size();
	int vertexCount = (int)contour.size();
	Matrix4 matrix;

	if (pathCount > 0)
	{
		// transform matrix
		if (pathCount > 1)
			matrix.lookAt(path[1] - path[0]);

		matrix.translate(path[0]);

		// multiply matrix to the contour
		// NOTE: the contour vertices are transformed here
		//       MUST resubmit contour data if the path is resset to 0
		for (int i = 0; i < vertexCount; ++i)
		{
			contour[i] = matrix * contour[i];
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
// return normal vectors at the current path point
///////////////////////////////////////////////////////////////////////////////
inline std::vector<Vector3> Pipe::computeContourNormal(int pathIndex)
{
	// get current contour and center point
	std::vector<Vector3>& contour = contours[pathIndex];
	Vector3 center = path[pathIndex];

	std::vector<Vector3> contourNormal;
	Vector3 normal;
	for (int i = 0; i < (int)contour.size(); ++i)
	{
		normal = (contour[i] - center).normalize();
		contourNormal.push_back(normal);
	}

	return contourNormal;
}

inline void Pipe::get_mesh_data() {
	auto point_count = contours.size();
	auto ring_count = contours[0].size();

	v.reserve(ring_count * contours.size() + 2);
	n.reserve(ring_count * contours.size() + 2);
	f.reserve(ring_count * contours.size() * 2 + ring_count + ring_count);

	int a = 1;
	for (int i = 0; i < point_count; ++i)
	{
		std::vector<Vector3> contour = getContour(i);
		std::vector<Vector3> normal = getNormal(i);

		a = (int)contour.size() - 1;
		for (int j = 0; j < a; j++)
		{
			//print(contour[j].x, contour[j].y, contour[j].z);
			v.emplace_back(contour[j].x, contour[j].y, contour[j].z);
			n.emplace_back(normal[j].x, normal[j].y, normal[j].z);

			if (i < point_count - 1) {
				f.emplace_back(
					(i + 0) * a + (j + 0),
					(i + 0) * a + (j + 1) % a,
					(i + 1) * a + (j + 1) % a
				);
				f.emplace_back(
					(i + 0) * a + (j + 0),
					(i + 1) * a + (j + 1) % a,
					(i + 1) * a + (j + 0) % a
				);
			}
		}
	}

	//cap ends
	Vector3 c0(0, 0, 0);
	Vector3 c1(0, 0, 0);
	std::vector<Vector3> contour0 = Pipe::getContour(0);
	std::vector<Vector3> contour1 = getContour((int)point_count - 1);
	for (int i = 0; i < a; i++) {
		c0 += contour0[i];
		c1 += contour1[i];
	}

	c0 /= (a * 1.0f);
	v.emplace_back(c0.x, c0.y, c0.z);
	glm::vec3 n0(contour0[0].x - contour0[1].x, contour0[0].y - contour0[1].y, contour0[0].z - contour0[1].z);
	glm::normalize(n0);

	c1 /= (a * 1.0f);
	v.emplace_back(c1.x, c1.y, c1.z);
	glm::vec3 n1(contour1[contour1.size() - 1].x - contour1[contour1.size() - 2].x, contour1[contour1.size() - 1].y - contour1[contour1.size() - 2].y, contour1[contour1.size() - 1].z - contour1[contour1.size() - 2].z);
	glm::normalize(n1);

	n.emplace_back(n0);
	n.emplace_back(n1);
	for (int i = 0; i < a; i++) {
		f.emplace_back(
			(i + 0),
			(i + 1) % (a),
			v.size() - 2
		);
	}

	for (int i = 0; i < a; i++) {
		f.emplace_back(
			a * (point_count - 1) + (i + 0),
			a * (point_count - 1) + (i + 1) % (a),
			v.size() - 1
		);
	}
}

///////////////////////////////////////////////////////////////////////////////
// generate points of a circle on xy plane
///////////////////////////////////////////////////////////////////////////////
inline std::vector<Vector3> Pipe::buildCircle(float radius, int steps)
{
	std::vector<Vector3> points;
	if (steps < 2) return points;

	const float PI2 = (float)acos(-1) * 2.0f;
	float x, y, a;
	for (int i = 0; i <= steps; ++i)
	{
		a = PI2 / steps * i;
		x = radius * cosf(a);
		y = radius * sinf(a);
		points.push_back(Vector3(x, y, 0));
	}
	return points;
}

///////////////////////////////////////////////////////////////////////////////
// ctors
///////////////////////////////////////////////////////////////////////////////
inline Pipe::Pipe()
{
}

inline Pipe::Pipe(const std::vector<Vector3>& pathPoints, int ring_count, const std::vector<float>& radii)//const std::vector<Vector3>& contourPoints,
{
	set(pathPoints, ring_count, radii);
}

///////////////////////////////////////////////////////////////////////////////
// setters
///////////////////////////////////////////////////////////////////////////////
inline void Pipe::set(const std::vector<Vector3>& pathPoints, int ring_count, const std::vector<float>& radii)
{
	std::vector<Vector3> circle = buildCircle(1.00f, ring_count); 	// sectional contour of pipe // radius, segments
	//std::vector<Vector3> p(1, path[0]);

	this->path = pathPoints;
	this->contour = circle;
	this->radii = radii;
	generateContours();
	scale();
	get_mesh_data();
}

///////////////////////////////////////////////////////////////////////////////
// generate a spiral path along y-axis
// r1: starting radius
// r2: ending radius
// h1: starting height
// h2: ending height
// turns: # of revolutions
// points: # of points
inline std::vector<Vector3> buildSpiralPath(float r1, float r2, float h1, float h2,
	float turns, int points)
{
	const float PI = (float)acos(-1);
	std::vector<Vector3> vertices;
	Vector3 vertex;
	float r = r1;
	float rStep = (r2 - r1) / (points - 1);
	float y = h1;
	float yStep = (h2 - h1) / (points - 1);
	float a = 0;
	float aStep = (turns * 2 * PI) / (points - 1);
	for (int i = 0; i < points; ++i)
	{
		vertex.x = r * cos(a);
		vertex.z = r * sin(a);
		vertex.y = y;
		vertices.push_back(vertex);
		// next
		r += rStep;
		y += yStep;
		a += aStep;
	}
	return vertices;
}