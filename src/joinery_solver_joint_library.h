#pragma once
#include "cgal.h"
#include "joinery_solver_joint.h"

namespace joint_library
{

	inline CGAL::Aff_transformation_3<IK> to_plane(IK::Vector_3 X1, IK::Vector_3 Y1, IK::Vector_3 Z1)
	{

		CGAL::Aff_transformation_3<IK> F1(
			X1.x(), Y1.x(), Z1.x(),
			X1.y(), Y1.y(), Z1.y(),
			X1.z(), Y1.z(), Z1.z());
		return F1;
	}

	inline double get_length(double x, double y, double z)
	{
		//double ON_DBL_MIN = 2.22507385850720200e-308;

		double len;
		x = fabs(x);
		y = fabs(y);
		z = fabs(z);
		if (y >= x && y >= z)
		{
			len = x;
			x = y;
			y = len;
		}
		else if (z >= x && z >= y)
		{
			len = x;
			x = z;
			z = len;
		}

		// 15 September 2003 Dale Lear
		//     For small denormalized doubles (positive but smaller
		//     than DBL_MIN), some compilers/FPUs set 1.0/x to +INF.
		//     Without the ON_DBL_MIN test we end up with
		//     microscopic vectors that have infinte length!
		//
		//     This code is absolutely necessary.  It is a critical
		//     part of the bug fix for RR 11217.
		if (x > ON_DBL_MIN)
		{
			y /= x;
			z /= x;
			len = x * sqrt(1.0 + y * y + z * z);
		}
		else if (x > 0.0 && ON_IS_FINITE(x))
			len = x;
		else
			len = 0.0;

		return len;
	}

	inline bool unitize(IK::Vector_3 &v)
	{

		bool rc = false;
		// Since x,y,z are floats, d will not be denormalized and the
		// ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

		double d = get_length(v.x(), v.y(), v.z());
		if (d > 0.0)
		{
			double dx = v.x();
			double dy = v.y();
			double dz = v.z();
			v = IK::Vector_3(
				(dx / d),
				(dy / d),
				(dz / d));
			rc = true;
		}
		return rc;
	}

	inline double RemapNumbers(const double &speed = 50, const double &low1 = 0, const double &high1 = 2000, const double &low2 = 0, const double &high2 = 1)
	{
		return low2 + (speed - low1) * (high2 - low2) / (high1 - low1);
	}

	inline double Lerp(const double &value1, const double &value2, const double &amount)
	{
		return value1 + (value2 - value1) * amount;
	}

	inline void interpolate_points(const IK::Point_3 &from, const IK::Point_3 &to, const int Steps, const bool includeEnds, std::vector<IK::Point_3> &interpolated_points)
	{

		if (includeEnds)
		{

			interpolated_points.reserve(Steps + 2);
			interpolated_points.emplace_back(from);

			for (int i = 1; i < Steps + 1; i++)
			{
				double num = i / (double)(1 + Steps);
				interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
			}

			interpolated_points.emplace_back(to);
		}
		else
		{

			interpolated_points.reserve(Steps);

			for (int i = 1; i < Steps + 1; i++)
			{
				double num = i / (double)(1 + Steps);
				interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
			}
		}
	}

	//type_typeidedge_subtypeieinplane_id
	//0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

	//1-9
	inline void ss_e_ip_0(joint &joint)
	{

		joint.name = "ss_e_ip_0";

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.f[0] = {
			{IK::Point_3(0, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0, -0.5, -0.357142857142857)},
			{IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)}};

		joint.f[1] = {
			{IK::Point_3(0, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0, 0.5, -0.357142857142857)},
			{IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)}};

		joint.m[0] = {
			{IK::Point_3(0, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0, -0.5, -0.357142857142857)},
			{IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)}};

		joint.m[1] = {
			{IK::Point_3(0, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0, 0.5, -0.357142857142857)},
			{IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)}};

		joint.f_boolean_type = {'1', '1'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ss_e_ip_1(joint &joint, const double &division_distance, const double &shift)
	{

		joint.name = "ss_e_ip_1";

		//Resize arrays
		joint.f[0].reserve(2);
		joint.f[1].reserve(2);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(2, std::min(100, divisions));
		divisions += divisions % 2;
		/*divisions = 10;
		CGAL_Debug(joint_length);
		CGAL_Debug(divisions);*/

		//////////////////////////////////////////////////////////////////////////////////////////
		//Interpolate points
		//////////////////////////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> pts0;
		interpolate_points(IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5), divisions, false, pts0);
		IK::Vector_3 v(0.5, 0, 0);
		double shift_ = RemapNumbers(shift, 0, 1.0, -0.5, 0.5);
		IK::Vector_3 v_d(0, 0, -(1.0 / ((divisions + 1) * 2)) * shift_);

		int count = pts0.size() * 2;

		//1st polyline
		std::vector<IK::Point_3> pline0;
		pline0.reserve(count);
		pline0.emplace_back(pts0[0]);
		pline0.emplace_back(pts0[0] - v - v_d);

		for (int i = 1; i < pts0.size() - 1; i++)
		{
			if (i % 2 == 1)
			{
				pline0.emplace_back(pts0[i] - v + v_d);
				pline0.emplace_back(pts0[i] + v - v_d);
			}
			else
			{
				pline0.emplace_back(pts0[i] + v + v_d);
				pline0.emplace_back(pts0[i] - v - v_d);
			}
		}

		pline0.emplace_back(pts0[pts0.size() - 1] - v + v_d);
		pline0.emplace_back(pts0[pts0.size() - 1]);

		//2nd polyline
		IK::Vector_3 v_o(0, 1, 0);
		std::vector<IK::Point_3> pline1;
		pline1.reserve(pline0.size());

		for (int i = 0; i < pline0.size(); i++)
		{
			pline1.emplace_back(pline0[i] + v_o);
		}

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.f[0] = {
			pline0,
			{pline0.front(), pline0.back()}};

		joint.f[1] = {
			pline1,
			{pline1.front(), pline1.back()}};

		joint.m[0] = {
			pline0,
			{pline0.front(), pline0.back()}};

		joint.m[1] = {
			pline1,
			{pline1.front(), pline1.back()}};

		joint.f_boolean_type = {'1', '1'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	//10-19
	inline void ss_e_op_0(joint &joint)
	{

		joint.name = "ss_e_op_0";

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.f[0] = {{IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857)},
					  {IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5)}};

		joint.f[1] = {{IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
					  {IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5)}};

		joint.m[0] = {{IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857)},
					  {IK::Point_3(-0.5, 0.5, 0.5), IK::Point_3(-0.5, 0.5, -0.5)}};

		joint.m[1] = {{IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285712), IK::Point_3(0.5, -0.5, -0.0714285714285716), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857)},
					  {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)}};

		joint.f_boolean_type = {'1', '1'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ss_e_op_2(joint &joint, const double &division_distance, const double &shift)
	{

		joint.name = "ss_e_op_2";

		//Resize arrays
		joint.f[0].reserve(2);
		joint.f[1].reserve(2);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(4, std::min(20, divisions));
		divisions += divisions % 2;
		////////////////////////////////////////////////////////////////////
		//Interpolate points
		////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> arrays[4];

		interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[0]);
		interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[1]);
		interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[2]);
		interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[3]);

		////////////////////////////////////////////////////////////////////
		//Move segments
		////////////////////////////////////////////////////////////////////
		int start = 0;

		IK::Vector_3 v = shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));

		for (int i = start; i < 4; i += 1)
		{

			int mid = (int)(arrays[i].size() * 0.5);
			for (int j = 0; j < arrays[i].size(); j++)
			{
				//int flip = (j % 2 < 0) ? 1 : -1;

				int flip = (j < mid) ? 1 : -1;

				if (i == 1)
				{
					if (j < (mid - 1) || j > mid)
						arrays[i][j] -= 4 * v * flip;
				}
				else if (i == 0 || i == 2)
				{
					if (j < (mid - 1) || j > mid)
						arrays[i][j] -= 2 * v * flip;
				}
			}
		}

		////////////////////////////////////////////////////////////////////
		//Create Polylines
		////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[i + 1][j]);
				}
				else
				{
					pline.push_back(arrays[i + 1][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				joint.m[1] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				joint.m[0] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}

				};
			}
		}

		for (int i = 1; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[(i + 1) % 4][j]);
				}
				else
				{
					pline.push_back(arrays[(i + 1) % 4][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				joint.f[0] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				joint.f[1] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
		}

		joint.f_boolean_type = {'1', '1'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ss_e_op_1(joint &joint, const double &division_distance, const double &shift)
	{

		joint.name = "ss_e_op_1";

		//Resize arrays
		joint.f[0].reserve(2);
		joint.f[1].reserve(2);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(2, std::min(20, divisions));
		if (joint.tile_parameters.size() > 0)
			divisions = joint.tile_parameters[0];

		divisions += divisions % 2 + 2;
		////////////////////////////////////////////////////////////////////
		//Interpolate points
		////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> arrays[4];

		interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[2]);
		interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[3]);
		interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[0]);
		interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[1]);

		////////////////////////////////////////////////////////////////////
		//Move segments
		////////////////////////////////////////////////////////////////////
		int start = 0;

		IK::Vector_3 v = shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
		for (int i = start; i < 4; i++)
		{

			int mid = (int)(arrays[i].size() * 0.5);

			for (int j = 0; j < arrays[i].size(); j++)
			{

				//int flip = (j % 2 == 0) ? 1 : -1;
				//flip = i < 2 ? flip : flip * -1;
				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				arrays[i][j] += v * flip;
			}
		}

		////////////////////////////////////////////////////////////////////
		//Create Polylines
		////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				/*bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;*/

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[i + 1][j]);
				}
				else
				{
					pline.push_back(arrays[i + 1][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				joint.m[1] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				joint.m[0] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}

				};
			}
		}

		for (int i = 1; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[(i + 1) % 4][j]);
				}
				else
				{
					pline.push_back(arrays[(i + 1) % 4][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				joint.f[0] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				joint.f[1] = {
					pline,
					//{ pline[0],pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
		}

		joint.f_boolean_type = {'1', '1'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	//20-29
	inline void ts_e_p_0(joint &joint)
	{

		joint.name = "ts_e_p_0";

		joint.f[0] = {{IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
					  {IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715)},
					  {IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714)},
					  {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)}};

		joint.f[1] = {{IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
					  {IK::Point_3(-0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285713)},
					  {IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714)},
					  {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)}};

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.m[0] = {{IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285715), IK::Point_3(0.5, 0.5, -0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)},
					  {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)}};

		joint.m[1] = {{IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
					  {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)}};

		joint.f_boolean_type = {'2', '2', '2', '2'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ts_e_p_1(joint &joint)
	{

		joint.name = "ts_e_p_1"; //Annen

		joint.f[0] = {{IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.277777777777778)},
					  {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.166666666666667)},
					  {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, 0.166666666666667)}};

		joint.f[1] = {{IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.277777777777778)},
					  {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.166666666666667)},
					  {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667)}};

		//Joint lines, always the last line or rectangle is not a joint but an cutting element
		joint.m[0] = {{IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889)},
					  {IK::Point_3(0.5, -0.5, 0.5), IK::Point_3(0.5, -0.5, -0.5)}};

		joint.m[1] = {{IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.0555555555555558), IK::Point_3(-0.5, -0.5, 0.0555555555555557), IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889)},
					  {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)}};

		joint.f_boolean_type = {'2', '2', '2', '2'};
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ts_e_p_2(joint &joint, const double &division_distance, const double &shift)
	{

		joint.name = "ts_e_p_2";

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(2, std::min(20, divisions));
		divisions += divisions % 2;

		//Resize arrays
		int size = (int)(divisions * 0.5) + 1;

		joint.f[0].reserve(size);
		joint.f[1].reserve(size);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Interpolate points
		////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> arrays[4];

		interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
		interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
		interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
		interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

		////////////////////////////////////////////////////////////////////
		//Move segments
		////////////////////////////////////////////////////////////////////
		int start = 0;

		IK::Vector_3 v = shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
		for (int i = start; i < 4; i++)
		{

			int mid = (int)(arrays[i].size() * 0.5);

			for (int j = 0; j < arrays[i].size(); j++)
			{

				int flip = (j % 2 == 0) ? 1 : -1;
				flip = i < 2 ? flip : flip * -1;

				arrays[i][j] += v * flip;
			}
		}

		////////////////////////////////////////////////////////////////////
		//Create Polylines
		////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[i + 1][j]);
				}
				else
				{
					pline.push_back(arrays[i + 1][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				joint.m[1] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				joint.m[0] = {
					pline,
					//{ pline[0], pline[pline.size() - 1] },
					{pline[0], pline[pline.size() - 1]}

				};
			}
		}

		for (int i = 0; i < joint.m[0][0].size(); i += 4)
		{

			joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
			joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
		}
		joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
		joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

		joint.f_boolean_type = std::vector<char>(size, '2');
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	inline void ts_e_p_3(joint &joint, const double &division_distance, const double &shift)
	{

		joint.name = "ts_e_p_3";

		////////////////////////////////////////////////////////////////////
		//Number of divisions
		//Input joint line (its lengths)
		//Input distance for division
		////////////////////////////////////////////////////////////////////
		double joint_length = CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1]); // Math.Abs(500);
		int divisions = (int)std::ceil(joint_length / (division_distance * division_distance));
		divisions = (int)std::max(4, std::min(100, divisions));
		if (joint.tile_parameters.size() > 0)
			divisions = joint.tile_parameters[0];
		divisions -= divisions % 4;

		//Resize arrays
		int size = (int)(divisions * 0.25) + 1;

		joint.f[0].reserve(size);
		joint.f[1].reserve(size);
		joint.m[0].reserve(2);
		joint.m[1].reserve(2);

		////////////////////////////////////////////////////////////////////
		//Interpolate points
		////////////////////////////////////////////////////////////////////
		std::vector<IK::Point_3> arrays[4];

		interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
		interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
		interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
		interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

		////////////////////////////////////////////////////////////////////
		//Move segments
		////////////////////////////////////////////////////////////////////
		int start = 0;

		IK::Vector_3 v = shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
		for (int i = start; i < 4; i++)
		{

			int mid = (int)(arrays[i].size() * 0.5);

			for (int j = 0; j < arrays[i].size(); j++)
			{

				int flip = (j % 2 == 0) ? 1 : -1;
				flip = i < 2 ? flip : flip * -1;

				arrays[i][j] += v * flip;
			}
		}

		////////////////////////////////////////////////////////////////////
		//Create Polylines
		////////////////////////////////////////////////////////////////////

		for (int i = 0; i < 4; i += 2)
		{

			CGAL_Polyline pline;
			pline.reserve(arrays[0].size() * 2);

			for (int j = 0; j < arrays[0].size(); j++)
			{

				if (j % 4 > 1)
					continue;

				bool flip = j % 2 == 0;
				flip = i < 2 ? flip : !flip;

				if (flip)
				{
					pline.push_back(arrays[i + 0][j]);
					pline.push_back(arrays[i + 1][j]);
				}
				else
				{
					pline.push_back(arrays[i + 1][j]);
					pline.push_back(arrays[i + 0][j]);
				}
			}

			if (i < 2)
			{
				pline.push_back(arrays[i + 0][arrays[0].size() - 1]);
				joint.m[1] = {
					pline,
					{pline[0], pline[pline.size() - 1]}};
			}
			else
			{
				pline.push_back(arrays[i + 1][arrays[0].size() - 1]);
				joint.m[0] = {
					pline,
					{pline[0], pline[pline.size() - 1]}

				};
			}
		}

		for (int i = 0; i < joint.m[0][0].size() - joint.m[0][0].size() % 4; i += 4)
		{

			joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
			joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
		}
		joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
		joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

		joint.f_boolean_type = std::vector<char>(size, '2');
		joint.m_boolean_type = {'1', '1'};

		joint.orient_to_connection_area();
	}

	//30-39
	inline void cr_c_ip_0(joint &joint)
	{

		//printf("cr_c_ip_0");

		joint.name = "cr_c_ip_0";

		double scale = 1;
		joint.f[0] = {

			{IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)},
			{IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)}

		};

		joint.f[1] = {
			{IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)},
			{IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)}

		};

		joint.m[0] = {
			{IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)},
			{IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)}};

		joint.m[1] = {
			{IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)},
			{IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)}};

		joint.m_boolean_type = {'3', '3'};
		joint.f_boolean_type = {'3', '3'};

		//Orient to 3D
		joint.orient_to_connection_area();
	}

	inline void cr_c_ip_1(joint &joint, const double &shift)
	{

		joint.name = "cr_c_ip_1";

		//double shift = 0.5;
		double s = std::max(std::min(shift, 1.0), 0.0);
		s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

		double a = 0.5 - s;
		double b = 0.5;
		double c = 2 * (b - a);
		double z = 0.5;

		IK::Point_3 p[16] = {
			IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),									//Center
			IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), //CenterOffset
			IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),									//Top
			IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)								//Bottom
		};

		IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

		//Construct polylines from points
		joint.f[0] = {

			{p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, //Center

			{p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, //TopSide0
			{p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, //TopSide1
			{p[2], p[1], p[1 + 12], p[2 + 12], p[2]},						 //BotSide0
			{p[0], p[3], p[3 + 12], p[0 + 12], p[0]},						 //BotSide1

			{p[0], p[0 + 12], p[0 + 4], p[0 + 8], p[0]},	  //Corner0
			{p[1 + 12], p[1], p[1 + 8], p[1 + 4], p[1 + 12]}, //Corner1
			{p[2], p[2 + 12], p[2 + 4], p[2 + 8], p[2]},	  //Corner2
			{p[3 + 12], p[3], p[3 + 8], p[3 + 4], p[3 + 12]}  //Corner3

		};

		//Offset and
		//flip polylines
		joint.f[1].reserve(9);
		joint.m[0].reserve(9);
		joint.f[1].reserve(9);

		auto xform = to_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

		double lenghts[9] = {0.5, 0.4, 0.4, 0.4, 0.4, 0.1, 0.1, 0.1, 0.1};
		for (int i = 0; i < 9; i++)
		{

			joint.f[1].push_back(joint.f[0][i]);

			//offset distance
			IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
			unitize(cross);

			//offset
			for (int j = 0; j < joint.f[1][i].size(); j++)
				joint.f[1][i][j] += cross * lenghts[i];

			joint.m[0].push_back(joint.f[0][i]);
			for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
				*it = it->transform(xform);

			joint.m[1].push_back(joint.f[1][i]);
			for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
				*it = it->transform(xform);
		}

		////Construct closed mesh
		////Construct Mesh
		//Mesh mesh0 = new Mesh();
		//mesh0.Vertices.AddVertices(p);

		//mesh0.Faces.AddFace(0, 1, 2);
		//mesh0.Faces.AddFace(2, 3, 0);

		//mesh0.Faces.AddFace(1, 0, 0 + 8);
		//mesh0.Faces.AddFace(0 + 8, 1 + 8, 1);

		//mesh0.Faces.AddFace(3, 2, 2 + 8);
		//mesh0.Faces.AddFace(2 + 8, 3 + 8, 3);

		//mesh0.Faces.AddFace(2, 1, 1 + 12);
		//mesh0.Faces.AddFace(1 + 12, 2 + 12, 2);

		//mesh0.Faces.AddFace(0, 3, 3 + 12);
		//mesh0.Faces.AddFace(3 + 12, 0 + 12, 0);

		//mesh0.Faces.AddFace(0, 0 + 12, 0 + 4);
		//mesh0.Faces.AddFace(0 + 4, 0 + 8, 0);

		//mesh0.Faces.AddFace(1 + 12, 1, 1 + 8);
		//mesh0.Faces.AddFace(1 + 8, 1 + 4, 1 + 12);

		//mesh0.Faces.AddFace(2, 2 + 12, 2 + 4);
		//mesh0.Faces.AddFace(2 + 4, 2 + 8, 2);

		//mesh0.Faces.AddFace(3 + 12, 3, 3 + 8);
		//mesh0.Faces.AddFace(3 + 8, 3 + 4, 3 + 12);

		//mesh0.Faces.AddFace(0 + 12, 3 + 12, 3 + 4);
		//mesh0.Faces.AddFace(3 + 4, 0 + 4, 0 + 12);

		//mesh0.Faces.AddFace(0 + 4, 3 + 4, 3 + 8);
		//mesh0.Faces.AddFace(3 + 8, 4 + 4, 0 + 4);

		//mesh0.Faces.AddFace(4 + 4, 3 + 8, 2 + 8);
		//mesh0.Faces.AddFace(2 + 8, 1 + 8, 4 + 4);

		//mesh0.Faces.AddFace(1 + 8, 2 + 8, 2 + 4);
		//mesh0.Faces.AddFace(2 + 4, 1 + 4, 1 + 8);

		//mesh0.Faces.AddFace(1 + 4, 2 + 4, 2 + 12);
		//mesh0.Faces.AddFace(2 + 12, 1 + 12, 1 + 4);

		//Mesh mesh1 = mesh0.DuplicateMesh();
		//mesh1.Transform(xform);

		joint.m_boolean_type = {'6', '6', '6', '4', '4', '4', '4', '9', '9'};
		joint.f_boolean_type = {'6', '6', '6', '4', '4', '4', '4', '9', '9'};

		//Orient to 3D
		joint.orient_to_connection_area();
	}

	inline void construct_joint_by_index(std::vector<element> &elements, std::vector<joint> &joints, const double &division_distance_, const double &shift_, std::vector<double> &default_parameters_for_four_types)
	{

		double division_distance = division_distance_;
		double shift = shift_;
		//std::ofstream myfile;
		//myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output2.txt");
		//myfile << joints.size()<< " \n";
		for (auto &joint : joints)
		{

			//Select user given type
			//types0+265
			int id_representing_joing_name = -1;
			if (elements[joint.v0].joint_types.size() && elements[joint.v1].joint_types.size())
			{
				int a = elements[joint.v0].joint_types[joint.f0_0];
				int b = elements[joint.v1].joint_types[joint.f1_0];
				id_representing_joing_name = (a > b) ? a : b;

				/*	for (auto& o : elements[joint.v0].joint_types)
						myfile << o << " ";
					myfile << "\n";

					for (auto& o : elements[joint.v1].joint_types)
						myfile << o << " ";
					myfile << "\n";

					myfile << joint.f0_0 << "\n";
					myfile << joint.f1_0 << "\n";
					myfile << a << "\n";
					myfile << b << "\n";
					myfile << id_representing_joing_name <<"\n";*/
			}
			else if (elements[joint.v0].joint_types.size())
				id_representing_joing_name = elements[joint.v0].joint_types[joint.f0_0];
			else if (elements[joint.v1].joint_types.size())
				id_representing_joing_name = elements[joint.v1].joint_types[joint.f1_0];

			//myfile << joint.f0_0 << " " << joint.f0_1 << " \n";
			//myfile << elements[joint.v0].joint_types[joint.f0_0] << " " << elements[joint.v1].joint_types[joint.f1_0] << " \n";

			//When users gives an input -> default_parameters_for_four_types
			int number_of_types = 6;	  //side-side in-plane | side-side out-of-plane | top-side | cross | top-top | side-side rotated
			int number_of_parameters = 3; //division_dist | shift | type
			bool default_parameters_given = default_parameters_for_four_types.size() == number_of_types * number_of_parameters;
			//for (int i = 0; i < default_parameters_for_four_types.size(); i+=3)
			//CGAL_Debug(default_parameters_for_four_types[i], default_parameters_for_four_types[i+1], default_parameters_for_four_types[i+2]);

			//CGAL_Debug(joint.type);
			//CGAL_Debug(id_representing_joing_name);
			//CGAL_Debug(joint.type == 30);
			//CGAL_Debug((id_representing_joing_name > 39 && id_representing_joing_name < 50));
			//CGAL_Debug(id_representing_joing_name == -1);
			//CGAL_Debug(joint.type == 30 && ((id_representing_joing_name > 39 && id_representing_joing_name < 50) || id_representing_joing_name == -1));

			//Select first by local search, only then by user given index, or by default
			if (id_representing_joing_name == 0)
			{
				//CGAL_Debug(55555);
				continue; //Nothing
			}
			else if (joint.type == 12 && ((id_representing_joing_name > 0 && id_representing_joing_name < 10) || id_representing_joing_name == -1))
			{
				//CGAL_Debug(66666);

				if (default_parameters_given)
				{
					int group = 0;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];
				}

				switch (id_representing_joing_name)
				{
				case (1):
					ss_e_ip_1(joint, division_distance, shift);
					break;
				case (2):
					ss_e_ip_0(joint);
					break;
				default:
					ss_e_ip_1(joint, division_distance, shift);
					//ss_e_ip_0(joint);
					break;
				}
			}
			else if (joint.type == 11 && ((id_representing_joing_name > 9 && id_representing_joing_name < 20) || id_representing_joing_name == -1))
			{

				if (default_parameters_given)
				{
					int group = 1;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];

					//CGAL_Debug(division_distance);
					//CGAL_Debug(shift);
					//CGAL_Debug(id_representing_joing_name);
				}

				//CGAL_Debug(77777);
				switch (id_representing_joing_name)
				{
				case (10):
					ss_e_op_1(joint, division_distance, shift);
					break;
				case (11):
					ss_e_op_2(joint, division_distance, shift);
					break;
				case (12):
					ss_e_op_0(joint);
					break;

				default:
					ss_e_op_1(joint, division_distance, shift);
					//ss_e_op_0(joint);
					break;
				}
			}
			else if (joint.type == 20 && ((id_representing_joing_name > 19 && id_representing_joing_name < 30) || id_representing_joing_name == -1))
			{

				if (default_parameters_given)
				{
					int group = 2;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];

					//CGAL_Debug(division_distance);
					//CGAL_Debug(shift);
					//CGAL_Debug(id_representing_joing_name);
				}

				//CGAL_Debug(88888);
				switch (id_representing_joing_name)
				{
				case (20):
					ts_e_p_1(joint);
					break;
				case (21):
					ts_e_p_2(joint, division_distance, 0);
					break;
				case (22):
					ts_e_p_3(joint, division_distance, 0);
					break;
				case (23):
					ts_e_p_0(joint);
					break;
				default:
					ts_e_p_3(joint, division_distance, 0);
					break;
				}
			}
			else if (joint.type == 30 && ((id_representing_joing_name > 39 && id_representing_joing_name < 50) || id_representing_joing_name == -1))
			{

				//CGAL_Debug(99999);

				if (default_parameters_given)
				{
					int group = 3;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];
				}

				switch (id_representing_joing_name)
				{

				case (30):
					cr_c_ip_0(joint);
					break;
				case (31):
					//cr_c_ip_0(joint);
					cr_c_ip_1(joint, shift);
					break;
				default:
					cr_c_ip_0(joint);
					//cr_c_ip_0(joint);
					//printf(joint.name.c_str());
					break;
				}
			}
			else if (false)
			{ //top-top

				if (default_parameters_given)
				{
					int group = 4;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];
				}
			}
			else if (false)
			{ //side-side rotated

				if (default_parameters_given)
				{
					int group = 5;
					division_distance = default_parameters_for_four_types[number_of_parameters * group + 0];
					shift = default_parameters_for_four_types[number_of_parameters * group + 1];
					if (id_representing_joing_name == -1) //for cases when joint types per each edge are not defined
						id_representing_joing_name = default_parameters_for_four_types[number_of_parameters * group + 2];
				}
			}
		}

		//myfile.close();
	}

}
