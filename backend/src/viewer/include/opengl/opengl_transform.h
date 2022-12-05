#pragma once
//#include "../../../stdafx.h"
//#include <Eigen/Core>
//#include <Eigen/Dense>

// using RowMatrixXd = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;
// using RowMatrixXi = Eigen::Matrix<int, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

namespace opengl_transform
{
	// inline void plane_to_plane_eigen(
	//	const float& O0_x, const float& O0_y, const float& O0_z,
	//	const float& X0_x, const float& X0_y, const float& X0_z,
	//	const float& Y0_x, const float& Y0_y, const float& Y0_z,
	//	const float& Z0_x, const float& Z0_y, const float& Z0_z,

	//	const float& O1_x, const float& O1_y, const float& O1_z,
	//	const float& X1_x, const float& X1_y, const float& X1_z,
	//	const float& Y1_x, const float& Y1_y, const float& Y1_z,
	//	const float& Z1_x, const float& Z1_y, const float& Z1_z,

	//	std::vector<float>& points
	//	//Eigen::Vector3d O0, Eigen::Vector3d X0, Eigen::Vector3d Y0, Eigen::Vector3d Z0,
	//	//Eigen::Vector3d O1, Eigen::Vector3d X1, Eigen::Vector3d Y1, Eigen::Vector3d Z1
	//) {
	//	Eigen::Vector3d T0(-O0_x, -O0_y, -O0_z);

	//	Eigen::Matrix3d F0;
	//	F0 << X0_x, X0_y, X0_z,
	//		Y0_x, Y0_y, Y0_z,
	//		Z0_x, Z0_y, Z0_z;

	//	Eigen::Matrix3d F1;
	//	F1 << X1_x, Y1_x, Z1_x,
	//		X1_y, Y1_y, Z1_y,
	//		X1_z, Y1_z, Z1_z;

	//	Eigen::Matrix3d F = F1 * F0;

	//	Eigen::Vector3d T1(O1_x, O1_y, O1_z);

	//	//Combine translation and rotation
	//	Eigen::Matrix4d A;
	//	A.setIdentity();
	//	A.block<3, 1>(0, 3) = T0;

	//	Eigen::Matrix4d B;
	//	B.setIdentity();
	//	B.block<3, 3>(0, 0) = F;

	//	Eigen::Matrix4d C;
	//	C.setIdentity();
	//	C.block<3, 1>(0, 3) = T1;

	//	auto matrix = C * B * A;

	//	for (int i = 0; i < points.size(); i += 3) {
	//		Eigen::Vector4d v(points[i], points[i + 1], points[i + 2], 1);
	//		v = matrix * v;
	//		points[i + 0] = (float)v.x();
	//		points[i + 1] = (float)v.y();
	//		points[i + 2] = (float)v.z();
	//	}
	//}

	inline glm::mat4 xy_to_plane(

		const float &O1_x, const float &O1_y, const float &O1_z,
		const float &X1_x, const float &X1_y, const float &X1_z,
		const float &Y1_x, const float &Y1_y, const float &Y1_z,
		const float &Z1_x, const float &Z1_y, const float &Z1_z
		// std::vector<float>& points
		// IK::Vector_3 O1, IK::Vector_3 X1, IK::Vector_3 Y1, IK::Vector_3 Z1
	)
	{
		// transformation maps P0 to P1, P0+X0 to P1+X1, ...

		// Move to origin -> T0 translates point P0 to (0,0,0)
		// glm::mat4 T0 = glm::translate(glm::vec3(0, 0, 0));

		// Rotate ->
		// glm has inverted matrix comparing to EGEIN or CGAL (i,j -> j,i):
		glm::mat3 F0(
			1, 0, 0,
			0, 1, 0,
			0, 0, 1);

		// glm has inverted matrix comparing to EGEIN or CGAL (i,j -> j,i):
		glm::mat3 F1(
			X1_x, X1_y, X1_z,
			Y1_x, Y1_y, Y1_z,
			Z1_x, Z1_y, Z1_z);

		glm::mat3 R = F1 * F0;
		// glm::mat3 R = F0_inv * F1_inv;

		glm::mat4 R_ = glm::mat4(R);

		// Move to 3d -> T1 translates (0,0,0) to point P1
		glm::mat4 T1 = glm::translate(glm::vec3(O1_x, O1_y, O1_z));

		// auto matrix = T1 * R_ * T0;

		// for (int i = 0; i < points.size(); i += 3) {
		//	glm::vec4 v(points[i], points[i + 1], points[i + 2], 1);
		//	v = matrix * v;
		//	points[i + 0] = v.x;
		//	points[i + 1] = v.y;
		//	points[i + 2] = v.z;
		//	print(v.x);
		//	print(v.y);
		//	print(v.z);
		// }

		return T1 * R_;
	}

	inline glm::mat4 plane_to_plane(
		const float &O0_x, const float &O0_y, const float &O0_z,
		const float &X0_x, const float &X0_y, const float &X0_z,
		const float &Y0_x, const float &Y0_y, const float &Y0_z,
		const float &Z0_x, const float &Z0_y, const float &Z0_z,

		const float &O1_x, const float &O1_y, const float &O1_z,
		const float &X1_x, const float &X1_y, const float &X1_z,
		const float &Y1_x, const float &Y1_y, const float &Y1_z,
		const float &Z1_x, const float &Z1_y, const float &Z1_z
		// std::vector<float>& points
		// IK::Vector_3 O1, IK::Vector_3 X1, IK::Vector_3 Y1, IK::Vector_3 Z1
	)
	{
		// transformation maps P0 to P1, P0+X0 to P1+X1, ...

		// Move to origin -> T0 translates point P0 to (0,0,0)
		glm::mat4 T0 = glm::translate(glm::vec3(-O0_x, -O0_y, -O0_z));

		// Rotate ->
		// glm has inverted matrix comparing to EGEIN or CGAL (i,j -> j,i):
		glm::mat3 F0(
			X0_x, Y0_x, Z0_x,
			X0_y, Y0_y, Z0_y,
			X0_z, Y0_z, Z0_z);

		// glm has inverted matrix comparing to EGEIN or CGAL (i,j -> j,i):
		glm::mat3 F1(
			X1_x, X1_y, X1_z,
			Y1_x, Y1_y, Y1_z,
			Z1_x, Z1_y, Z1_z);

		glm::mat3 R = F1 * F0;
		// glm::mat3 R = F0_inv * F1_inv;

		glm::mat4 R_ = glm::mat4(R);

		// Move to 3d -> T1 translates (0,0,0) to point P1
		glm::mat4 T1 = glm::translate(glm::vec3(O1_x, O1_y, O1_z));

		// auto matrix = T1 * R_ * T0;

		// for (int i = 0; i < points.size(); i += 3) {
		//	glm::vec4 v(points[i], points[i + 1], points[i + 2], 1);
		//	v = matrix * v;
		//	points[i + 0] = v.x;
		//	points[i + 1] = v.y;
		//	points[i + 2] = v.z;
		//	print(v.x);
		//	print(v.y);
		//	print(v.z);
		// }

		return T1 * R_ * T0;
	}

	// plane to plane
	inline glm::mat4 plane_to_plane(
		const glm::vec3 &O0,
		const glm::vec3 &X0,
		const glm::vec3 &Y0,
		const glm::vec3 &Z0,

		const glm::vec3 &O1,
		const glm::vec3 &X1,
		const glm::vec3 &Y1,
		const glm::vec3 &Z1

	)
	{
		return plane_to_plane(
			O0.x, O0.y, O0.z,
			X0.x, X0.y, X0.z,
			Y0.x, Y0.y, Y0.z,
			Z0.x, Z0.y, Z0.z,

			O1.x, O1.y, O1.z,
			X1.x, X1.y, X1.z,
			Y1.x, Y1.y, Y1.z,
			Z1.x, Z1.y, Z1.z

		);
	}

	inline glm::mat4 rotate_around_axis(const glm::vec3 &point, const glm::vec3 direction, const float &angle)
	{
		glm::mat4 T0 = glm::translate(-point);
		glm::mat4 R = glm::rotate(angle, direction);
		glm::mat4 T1 = glm::translate(point);
		return T1 * R * T0;
	}

	// when rotation line is transformed by another matrix
	inline glm::mat4 rotate_around_axis(const glm::mat4 &axis_matrix, const glm::vec3 &point_, const glm::vec3 direction_, const float &angle)
	{
		auto point = axis_matrix * glm::vec4(point_, 1);
		auto direction = axis_matrix * glm::vec4(direction_, 1);
		return rotate_around_axis(glm::vec3(point), glm::vec3(direction), angle);
	}

	// when rotation line is transformed by another matrix
	inline glm::mat4 rotate_around_line(const glm::mat4 &axis_matrix, const glm::vec3 &point0, const glm::vec3 point1, const float &angle)
	{
		auto point = axis_matrix * glm::vec4(point0, 1);
		auto direction = axis_matrix * glm::vec4(point1 - point0, 1);
		return rotate_around_axis(glm::vec3(point), glm::vec3(direction), angle);
	}

	////https://stackoverflow.com/questions/23270292/a-point-cgal-transformation-between-two-coordinate-systems
	// //https://github.com/mcneel/opennurbs/blob/7.x/opennurbs_xform.cpp Line1960
	// inline CGAL::Aff_transformation_3<IK> PlaneToPlane(
	//	IK::Vector_3 O0, IK::Vector_3 X0, IK::Vector_3 Y0, IK::Vector_3 Z0,
	//	IK::Vector_3 O1, IK::Vector_3 X1, IK::Vector_3 Y1, IK::Vector_3 Z1) {
	//	// transformation maps P0 to P1, P0+X0 to P1+X1, ...

	//	//Move to origin -> T0 translates point P0 to (0,0,0)
	//	CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

	//	//Rotate ->
	//	CGAL::Aff_transformation_3<IK> F0(
	//		X0.x(), X0.y(), X0.z(),
	//		Y0.x(), Y0.y(), Y0.z(),
	//		Z0.x(), Z0.y(), Z0.z());

	//	CGAL::Aff_transformation_3<IK> F1(
	//		X1.x(), Y1.x(), Z1.x(),
	//		X1.y(), Y1.y(), Z1.y(),
	//		X1.z(), Y1.z(), Z1.z());
	//	CGAL::Aff_transformation_3<IK> R = F1 * F0;

	//	//Move to 3d -> T1 translates (0,0,0) to point P1
	//	CGAL::Aff_transformation_3<IK> T1(CGAL::TRANSLATION, IK::Vector_3(O1.x() - 0, O1.y() - 0, O1.z() - 0));

	//	return T1 * R * T0;
	//}

	// inline Eigen::Matrix4d plane_to_plane(
	//	Eigen::Vector3d O0, Eigen::Vector3d X0, Eigen::Vector3d Y0, Eigen::Vector3d Z0,
	//	Eigen::Vector3d O1, Eigen::Vector3d X1, Eigen::Vector3d Y1, Eigen::Vector3d Z1
	//	​
	//) {
	//	Eigen::Vector3d T0(0 - O0.x(), 0 - O0.y(), 0 - O0.z());
	//	//Move to origin -> T0 translates point P0 to (0,0,0)
	//	​
	//		//Rotate A ->
	//		Eigen::Matrix3d F0;
	//	F0 << X0.x(), X0.y(), X0.z(),
	//		Y0.x(), Y0.y(), Y0.z(),
	//		Z0.x(), Z0.y(), Z0.z();
	//	​
	//		//Rotate B ->
	//		Eigen::Matrix3d F1;
	//	F1 << X1.x(), Y1.x(), Z1.x(),
	//		X1.y(), Y1.y(), Z1.y(),
	//		X1.z(), Y1.z(), Z1.z();
	//	​
	//		//Move to 3d -> T1 translates (0,0,0) to point P1
	//		Eigen::Vector3d T1(O1.x() - 0, O1.y() - 0, O1.z() - 0);
	//	​
	//		​
	//		Eigen::Matrix3d F = F1 * F0;
	//	​
	//		​
	//		​
	//		//Combine translation and rotation
	//		Eigen::Matrix4d A;
	//	A.setIdentity();
	//	//A.block<3, 3>(0, 0) = F0;
	//	A.block<3, 1>(0, 3) = T0;
	//	​
	//		Eigen::Matrix4d B;
	//	B.setIdentity();
	//	B.block<3, 3>(0, 0) = F;
	//	//B.block<3, 1>(0, 3) = T1;
	//	​
	//		Eigen::Matrix4d C;
	//	C.setIdentity();
	//	//C.block<3, 3>(0, 0) = F1;
	//	C.block<3, 1>(0, 3) = T1;
	//	​
	//		​
	//		​
	//		//Output
	//		return C * B * A;
	// }

	//   inline CGAL::Aff_transformation_3<IK> PlaneToXY(
	//       IK::Point_3 O0, IK::Plane_3 plane) {
	//       auto X0 = plane.base1();
	//       auto Y0 = plane.base2();
	//       auto Z0 = plane.orthogonal_vector();
	//       cgal_vector_util::Unitize(X0);
	//       cgal_vector_util::Unitize(Y0);
	//       cgal_vector_util::Unitize(Z0);

	//       // transformation maps P0 to P1, P0+X0 to P1+X1, ...

	//       //Move to origin -> T0 translates point P0 to (0,0,0)
	//       CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

	//       //Rotate ->
	//       CGAL::Aff_transformation_3<IK> F0(
	//           X0.x(), X0.y(), X0.z(),
	//           Y0.x(), Y0.y(), Y0.z(),
	//           Z0.x(), Z0.y(), Z0.z());

	//       return F0 * T0;
	//   }

	//   inline CGAL::Aff_transformation_3<IK> Scale(double x, double y, double z) {
	//       CGAL::Aff_transformation_3<IK> scale_matrix(
	//           x, 0, 0,
	//           0, y, 0,
	//           0, 0, z);
	//       return scale_matrix;

	//   }

	////rotate in plane
	//   inline void RotationAroundAxis(
	//       double sin_angle,
	//       double cos_angle,
	//       IK::Vector_3 axis,
	//       IK::Point_3 center,
	//       CGAL::Aff_transformation_3<IK>& xform) {
	//       xform = CGAL::Aff_transformation_3<IK>(CGAL::Identity_transformation());

	//       // *this = ON_Xform::IdentityTransformation;

	//       for (;;) {
	//           // 29 June 2005 Dale Lear
	//           //     Kill noise in input
	//           if (fabs(sin_angle) >= 1.0 - ON_SQRT_EPSILON && fabs(cos_angle) <= ON_SQRT_EPSILON) {
	//               cos_angle = 0.0;
	//               sin_angle = (sin_angle < 0.0) ? -1.0 : 1.0;
	//               break;
	//           }

	//           if (fabs(cos_angle) >= 1.0 - ON_SQRT_EPSILON && fabs(sin_angle) <= ON_SQRT_EPSILON) {
	//               cos_angle = (cos_angle < 0.0) ? -1.0 : 1.0;
	//               sin_angle = 0.0;
	//               break;
	//           }

	//           if (fabs(cos_angle * cos_angle + sin_angle * sin_angle - 1.0) > ON_SQRT_EPSILON) {
	//               IK::Vector_2 cs(cos_angle, sin_angle);
	//               if (cgal_vector_util::Unitize2(cs)) {
	//                   cos_angle = cs.x();
	//                   sin_angle = cs.y();
	//                   // no break here
	//               }
	//               else {
	//                   //ON_ERROR("sin_angle and cos_angle are both zero.");
	//                   cos_angle = 1.0;
	//                   sin_angle = 0.0;
	//                   break;
	//               }
	//           }

	//           if (fabs(cos_angle) > 1.0 - ON_EPSILON || fabs(sin_angle) < ON_EPSILON) {
	//               cos_angle = (cos_angle < 0.0) ? -1.0 : 1.0;
	//               sin_angle = 0.0;
	//               break;
	//           }

	//           if (fabs(sin_angle) > 1.0 - ON_EPSILON || fabs(cos_angle) < ON_EPSILON) {
	//               cos_angle = 0.0;
	//               sin_angle = (sin_angle < 0.0) ? -1.0 : 1.0;
	//               break;
	//           }

	//           break;
	//       }

	//       if (sin_angle != 0.0 || cos_angle != 1.0) {
	//           const double one_minus_cos_angle = 1.0 - cos_angle;
	//           IK::Vector_3 a = axis;
	//           if (fabs(cgal_vector_util::LengthSquared(a) - 1.0) > ON_EPSILON)
	//               cgal_vector_util::Unitize(a);

	//           auto x00 = a.x() * a.x() * one_minus_cos_angle + cos_angle;
	//           auto x01 = a.x() * a.y() * one_minus_cos_angle - a.z() * sin_angle;
	//           auto x02 = a.x() * a.z() * one_minus_cos_angle + a.y() * sin_angle;

	//           auto x10 = a.y() * a.x() * one_minus_cos_angle + a.z() * sin_angle;
	//           auto x11 = a.y() * a.y() * one_minus_cos_angle + cos_angle;
	//           auto x12 = a.y() * a.z() * one_minus_cos_angle - a.x() * sin_angle;

	//           auto x20 = a.z() * a.x() * one_minus_cos_angle - a.y() * sin_angle;
	//           auto x21 = a.z() * a.y() * one_minus_cos_angle + a.x() * sin_angle;
	//           auto x22 = a.z() * a.z() * one_minus_cos_angle + cos_angle;

	//           auto x30 = 0.0;
	//           auto x31 = 0.0;
	//           auto x32 = 0.0;
	//           auto x33 = 1.0;

	//           if (center.x() != 0.0 || center.y() != 0.0 || center.z() != 0.0) {
	//               auto x03 = -((x00 - 1.0) * center.x() + x01 * center.y() + x02 * center.z());
	//               auto x13 = -(x10 * center.x() + (x11 - 1.0) * center.y() + x12 * center.z());
	//               auto x23 = -(x20 * center.x() + x21 * center.y() + (x22 - 1.0) * center.z());

	//               xform = CGAL::Aff_transformation_3<IK>(
	//                   x00, x01, x02, x03,
	//                   x10, x11, x12, x13,
	//                   x20, x21, x22, x23
	//                   //x30, x31, x32, x33
	//                   );
	//           }
	//           else {
	//               xform = CGAL::Aff_transformation_3<IK>(
	//                   x00, x01, x02,
	//                   x10, x11, x12,
	//                   x20, x21, x22);
	//           }
	//       }
	//   }

	//   inline void AxisRotation(double angle, IK::Vector_3& axis, CGAL::Aff_transformation_3<IK>& rot) {
	//       //create matrix of the rotation
	//       IK::RT
	//           c = cos(angle),
	//           s = sin(angle),
	//           ux(axis.x()),
	//           uy(axis.y()),
	//           uz(axis.z());

	//       IK::RT matrix[12] =
	//       {
	//           ux * ux * (1 - c) + c, ux * uy * (1 - c) - uz * s, ux * uz * (1 - c) + uy * s, 0,
	//           ux * uy * (1 - c) + uz * s, uy * uy * (1 - c) + c, uy * uz * (1 - c) - ux * s, 0,
	//           ux * uz * (1 - c) - uy * s, uy * uz * (1 - c) + ux * s, uz * uz * (1 - c) + c, 0 };

	//       rot = CGAL::Aff_transformation_3<IK>(matrix[0], matrix[1], matrix[2],
	//           matrix[3], matrix[4], matrix[5],
	//           matrix[6], matrix[7], matrix[8],
	//           matrix[9], matrix[10], matrix[11]);
	//   }
}