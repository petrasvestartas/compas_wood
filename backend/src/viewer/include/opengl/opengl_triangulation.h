#pragma once
//#include "../../../stdafx.h"
//#include <cdt/CDT.h>
// CDT

namespace opengl_triangulation
{
	inline void triangulate_flat_polyline_with_holes(
		const std::vector<std::vector<glm::vec3>> &in_polylines,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec3> &out_normals,
		std::vector<glm::ivec3> &out_triangles,
		bool convex_only = false, double closed_tolerance = 0.001)
	{

		// v.insert(v.end(), v_prime.begin(), v_prime.end());
		// std::vector<std::vector<glm::vec3>> input_normals;
		// input_normals.reserve(in_polylines.size());

		glm::vec3 v(0, 0, 0);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// is closed and number of vertices in the last iteration
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		std::vector<bool> closed_ends;
		closed_ends.reserve(in_polylines.size());
		int all_v_count = 0;
		int all_v_count_start = out_vertices.size();
		for (int i = 0; i < in_polylines.size(); i++)
		{
			closed_ends.emplace_back(glm::distance(in_polylines[i].front(), in_polylines[i].back()) < closed_tolerance);

			size_t len = closed_ends.back() ? in_polylines[i].size() - 1 : in_polylines[i].size();
			all_v_count += len;
			// print(glm::distance(in_polylines[i].front(), in_polylines[i].back()));
		}

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// get transformation matrices
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		glm::mat4 to_xy;
		glm::mat4 to_3d;

		for (int i = 0; i < 1; i++)
		{
			size_t len = closed_ends[i] ? in_polylines[i].size() - 1 : in_polylines[i].size();

			for (int j = 0; j < len; j++)
			{
				auto num = ((j - 1) + len) % len;
				auto item1 = ((j + 1) + len) % len;
				v = v + glm::cross(in_polylines[i][j] - in_polylines[i][num], in_polylines[i][item1] - in_polylines[i][j]);
			}
			v = glm::normalize(v);

			// x-axis
			float length = hypotf(v.x, hypotf(v.y, v.z));
			float dir_scalar = (v.x > 0.0) ? length : -length;
			float xt = v.x + dir_scalar;
			float dot = -v.y / (dir_scalar * xt);

			glm::vec3 x = glm::vec3(dot * xt, 1.0f + dot * v.y, dot * v.z);
			x = glm::normalize(x);
			// y-axis
			glm::vec3 y = glm::cross(v, x);

			glm::vec3 o = glm::vec3(in_polylines[i][0].x, in_polylines[i][0].y, in_polylines[i][0].z);
			// print(o.x, o.y, o.z);
			// print(x.x, x.y, x.z);
			// print(y.x, y.y, y.z);
			// print(v.x, v.y, v.z);
			// print("next");

			to_xy = (opengl_transform::plane_to_plane(
				o.x, o.y, o.z,
				x.x, x.y, x.z,
				y.x, y.y, y.z,
				v.x, v.y, v.z,
				0, 0, 0,
				1, 0, 0,
				0, 1, 0,
				0, 0, 1));
			to_3d = (opengl_transform::plane_to_plane(
				0, 0, 0,
				1, 0, 0,
				0, 1, 0,
				0, 0, 1,
				o.x, o.y, o.z,
				x.x, x.y, x.z,
				y.x, y.y, y.z,
				v.x, v.y, v.z

				));
		}

		out_normals.reserve(out_normals.size() + all_v_count);
		out_vertices.reserve(out_vertices.size() + all_v_count);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		// cdt https://github.com/artem-ogre/CDT/issues/73
		// rules: order and orientation of curves does not matter
		// polylines are treated as open
		// code orients polylines to 2d and then orients back to 3D
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		CDT::Triangulation<float> cdt;

		// vertices
		std::vector<CDT::V2d<float>> cdt_vertices;
		int total_count_of_v = 0;
		for (int i = 0; i < in_polylines.size(); i++)
		{
			int is_closed = (glm::distance(in_polylines[i].front(), in_polylines[i].back()) < 0.01) ? -1 : 0;
			total_count_of_v += (int)in_polylines.size() + is_closed;
		}
		cdt_vertices.reserve(total_count_of_v);

		// print("normal");
		// print(v.x, v.y, v.z);
		for (int i = 0; i < in_polylines.size(); i++)
		{
			size_t is_closed = (glm::distance(in_polylines[i].front(), in_polylines[i].back()) < 0.01) ? -1 : 0;
			// std::vector<glm::vec3> norm;
			// norm.reserve(in_polylines[i].size() + is_closed);
			for (int j = 0; j < in_polylines[i].size() + is_closed; j++)
			{
				// transformation to xy
				out_vertices.emplace_back(in_polylines[i][j]);
				out_normals.emplace_back(v);
				auto p4 = to_xy * glm::vec4(in_polylines[i][j].x, in_polylines[i][j].y, in_polylines[i][j].z, 1);
				auto p = glm::vec3(p4.x, p4.y, p4.z);
				// print(p.x, p.y, p.z);
				// auto p = glm::vec3(in_polylines[i][j].x, in_polylines[i][j].y, in_polylines[i][j].z);
				cdt_vertices.emplace_back(CDT::V2d<float>::make(p.x, p.y));
			}
			// input_normals.emplace_back(norm);
		}

		//{ CDT::V2d<float>::make(15,-7),CDT::V2d<float>::make(9.62180016727275f,4.57060284363674f),CDT::V2d<float>::make(12,4.43070873582926f),CDT::V2d<float>::make(19,0),CDT::V2d<float>::make(16,-5),CDT::V2d<float>::make(6,-16),CDT::V2d<float>::make(15,-9),CDT::V2d<float>::make(22,-22),CDT::V2d<float>::make(22,2),CDT::V2d<float>::make(7,11),CDT::V2d<float>::make(6,-6),CDT::V2d<float>::make(-7,-2),CDT::V2d<float>::make(-15,16),CDT::V2d<float>::make(-17,23),CDT::V2d<float>::make(-43,28),CDT::V2d<float>::make(-24,-10),CDT::V2d<float>::make(-17,-11),CDT::V2d<float>::make(-23,8),CDT::V2d<float>::make(-14,2),CDT::V2d<float>::make(7,-33),CDT::V2d<float>::make(-3,-11),CDT::V2d<float>::make(-15,7),CDT::V2d<float>::make(-19,16),CDT::V2d<float>::make(-23,18),CDT::V2d<float>::make(-31,21),CDT::V2d<float>::make(-28,8),CDT::V2d<float>::make(-27,6),CDT::V2d<float>::make(-25,10),CDT::V2d<float>::make(-25,17) }
		//;
		cdt.insertVertices(cdt_vertices);

		// edges
		if (!convex_only)
		{
			std::vector<CDT::Edge> edges; // CDT::EdgeVec edges;
			std::vector<int> edge_per_polyline;
			edge_per_polyline.reserve(in_polylines.size());
			for (int i = 0; i < in_polylines.size(); i++)
			{
				int is_closed = (glm::distance(in_polylines[i].front(), in_polylines[i].back()) < 0.01) ? -1 : 0;
				edge_per_polyline.emplace_back(in_polylines[i].size() + is_closed);
			}

			int total_count_of_e = 0;
			for (int i = 0; i < edge_per_polyline.size(); i++)
				total_count_of_e += edge_per_polyline[i];
			edges.reserve(total_count_of_e);

			int v_count = 0;
			int group_count = 0;
			for (int i = 0; i < edge_per_polyline.size(); i++)
			{
				group_count += edge_per_polyline[i];

				int start = v_count;
				for (int j = 0; j < edge_per_polyline[i] - 1; j++)
				{
					edges.emplace_back(CDT::Edge(v_count, v_count + 1));
					// print(v_count, v_count + 1);
					v_count++;
				}
				edges.emplace_back(CDT::Edge(v_count, start));
				// print(v_count, start);
				// print("next");
				v_count++;
			}
			cdt.insertEdges(edges);

			cdt.eraseOuterTrianglesAndHoles();
		}
		else
		{
			cdt.eraseSuperTriangle();
		}

		auto out_triangles_size = out_triangles.size() + cdt.triangles.size() * 3;
		out_triangles.reserve(out_triangles_size);
		// print("next");
		// print(all_v_count_start);
		// print(all_v_count);
		for (int i = 0; i < cdt.triangles.size(); i++)
		{
			// print(all_v_count_start + cdt.triangles[i].vertices[0], all_v_count_start + cdt.triangles[i].vertices[1], all_v_count_start + cdt.triangles[i].vertices[2]);
			out_triangles.emplace_back(
				all_v_count_start + cdt.triangles[i].vertices[0],
				all_v_count_start + cdt.triangles[i].vertices[1],
				all_v_count_start + cdt.triangles[i].vertices[2]);
		}
	}

	inline void triangulate_flat_polylines(
		const std::vector<std::vector<glm::vec3>> &in_polylines,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec3> &out_normals,
		std::vector<glm::ivec3> &out_triangles,
		bool convex_only = false, double closed_tolerance = 0.001

	)
	{
		// std::cout << ("\ntriangulaiton\n");
		for (auto &in_polyline : in_polylines)
		{
			triangulate_flat_polyline_with_holes(
				{in_polyline},
				out_vertices,
				out_normals,
				out_triangles, convex_only, closed_tolerance);
		}
	}

	inline void triangulate_flat_polylines_with_holes(
		const std::vector<std::vector<std::vector<glm::vec3>>> &in_polylines_groups,
		std::vector<glm::vec3> &out_vertices,
		std::vector<glm::vec3> &out_normals,
		std::vector<glm::ivec3> &out_triangles,
		bool convex_only = false, double closed_tolerance = 0.001

	)
	{
		for (auto &in_polylines : in_polylines_groups)
			triangulate_flat_polyline_with_holes(
				in_polylines,
				out_vertices,
				out_normals,
				out_triangles, convex_only, closed_tolerance);
	}

	// inline void triangulate_flat_polylines_with_holes_pair_loft(
	// 	const std::vector<std::vector<std::vector<glm::vec3>>> &in_polylines_groups,
	// 	std::vector<glm::vec3> &out_vertices,
	// 	std::vector<glm::vec3> &out_normals,
	// 	std::vector<glm::ivec3> &out_triangles,
	// 	bool convex_only = false, double closed_tolerance = 0.001

	// )
	// {

	// 	// refer to cgal_mesh_util.h
	// 	for (std::vector<CGAL_Polyline> &polylines : in_polylines_groups)
	// 	{

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// Sanity Check
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		if (polylines.size() % 2 == 1)
	// 			return;

	// 		for (int i = 0; i < polylines.size(); i += 2)
	// 		{
	// 			auto a = polylines[i].size();
	// 			auto b = polylines[i + 1].size();
	// 			if (a != b)
	// 				return;
	// 			if (a < 2 || b < 2)
	// 				return;
	// 		}

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// Compute average normal and create a plane
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// CGAL_Debug("Polyline Length");
	// 		// for (int i = 0; i < polylines.size(); i += 2)
	// 		//     CGAL_Debug(cgal_polyline_util::polyline_length(polylines[i]));

	// 		auto lastID = polylines.size() - 2;
	// 		auto len = polylines[lastID].size() - 1;
	// 		IK::Vector_3 average_normal = IK::Vector_3(0, 0, 0);

	// 		for (int i = 0; i < len; i++)
	// 		{
	// 			auto prev = ((i - 1) + len) % len;
	// 			auto next = ((i + 1) + len) % len;
	// 			average_normal = average_normal + CGAL::cross_product(polylines[lastID][i] - polylines[lastID][prev], polylines[lastID][next] - polylines[lastID][i]);
	// 		}

	// 		IK::Plane_3 base_plane(polylines[lastID][0], average_normal);

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// Create a mesh for top outlines
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		std::vector<int> top_outline_face_vertex_indices;
	// 		int v, f;
	// 		cgal_mesh_util::mesh_from_polylines(polylines, base_plane, top_outline_face_vertex_indices, v, f);

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// Create mesh for the full plate
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Count vertices and faces
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		size_t vertex_count = 0;
	// 		for (int i = 0; i < polylines.size(); i += 2)
	// 			vertex_count += polylines[i].size() - 1;

	// 		if (v != vertex_count)
	// 		{
	// 			// CGAL_Debug(v);
	// 			// CGAL_Debug(vertex_count);
	// 			RowMatrixXd vv(0, 3);
	// 			RowMatrixXi ff(0, 3);
	// 			return std::make_tuple(vv, ff);
	// 		}

	// 		auto face_count = top_outline_face_vertex_indices.size() / 3;

	// 		RowMatrixXd vertices(vertex_count * 2, 3);
	// 		RowMatrixXi faces(face_count * 2 + vertex_count * 2, 3); //

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Top vertices
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		// Set vertex coordinates from all polylines a-b a-b
	// 		int vid = 0;
	// 		std::vector<std::array<int, 4>> sides;
	// 		sides.reserve(vertex_count);

	// 		bool holes = polylines.size() > 2;

	// 		for (int i = 0; i < polylines.size(); i += 2)
	// 		{
	// 			bool last = i == (polylines.size() - 2);

	// 			// CGAL_Debug(last);
	// 			// CGAL_Debug(i );
	// 			// CGAL_Debug(polylines.size() - 2);

	// 			for (int j = 0; j < polylines[i].size() - 1; j++)
	// 			{
	// 				// vertices
	// 				vertices(vid, 0) = polylines[i][j].hx();
	// 				vertices(vid, 1) = polylines[i][j].hy();
	// 				vertices(vid, 2) = polylines[i][j].hz();

	// 				// last faces
	// 				if (j == polylines[i].size() - 2)
	// 				{ // take vertices from beggining
	// 					auto n = polylines[i].size() - 2;
	// 					std::array<int, 4> side{vid, vid - (int)n, vid - (int)n + (int)vertex_count, vid + 0 + (int)vertex_count};

	// 					if (holes)
	// 					{
	// 						sides.emplace_back(side);
	// 						// if (last) {
	// 						//     sides.emplace_back(side);
	// 						// } else {
	// 						//     side = {
	// 						//         vid + 0 + vertex_count,
	// 						//         vid - n + vertex_count,
	// 						//         vid - n,
	// 						//         vid
	// 						//     };
	// 						//     sides.emplace_back(side);
	// 						// }
	// 					}
	// 					else
	// 					{
	// 						sides.emplace_back(side);
	// 					}

	// 					// iterated faces
	// 				}
	// 				else
	// 				{ // take next vertices
	// 					std::array<int, 4> side = {
	// 						vid + 0 + (int)vertex_count,
	// 						vid + 1 + (int)vertex_count,
	// 						vid + 1,
	// 						vid,
	// 					};

	// 					if (holes)
	// 					{
	// 						// if (last) {
	// 						side = {
	// 							vid,
	// 							vid + 1,
	// 							vid + 1 + (int)vertex_count,
	// 							vid + 0 + (int)vertex_count,
	// 						};
	// 						sides.emplace_back(side);
	// 						//}
	// 						// else {
	// 						//    sides.emplace_back(side);
	// 						//}
	// 					}
	// 					else
	// 					{
	// 						side = {
	// 							vid,
	// 							vid + 1,
	// 							vid + 1 + (int)vertex_count,
	// 							vid + 0 + (int)vertex_count,
	// 						};
	// 						sides.emplace_back(side);
	// 					}
	// 				}

	// 				// RhinoApp().Print("Vertices flag %i index %i vertices %f %f %f \n", flag0, vid, polylines[i][j].hx(), polylines[i][j].hy(), polylines[i][j].hz());

	// 				vid++;
	// 			}
	// 		}

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Bottom vertices
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		vid = 0;
	// 		for (int i = 0; i < polylines.size(); i += 2)
	// 		{
	// 			for (int j = 0; j < polylines[i].size() - 1; j++)
	// 			{
	// 				// vertices
	// 				vertices(vertex_count + vid, 0) = polylines[i + 1][j].hx();
	// 				vertices(vertex_count + vid, 1) = polylines[i + 1][j].hy();
	// 				vertices(vertex_count + vid, 2) = polylines[i + 1][j].hz();

	// 				// RhinoApp().Print("Vertices flag %i index %i vertices %f %f %f \n\n", flag1, vid + vertex_count, polylines[i + 1][j].hx(), polylines[i + 1][j].hy(), polylines[i + 1][j].hz());
	// 				vid++;
	// 			}
	// 		}

	// 		// RhinoApp().Print("face_count %i vertex_count %i \n", output.FaceCount(), output.VertexCount());

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Top face indives
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
	// 		{
	// 			int fid = i / 3;
	// 			faces(fid, 0) = top_outline_face_vertex_indices[i + 0];
	// 			faces(fid, 1) = top_outline_face_vertex_indices[i + 1];
	// 			faces(fid, 2) = top_outline_face_vertex_indices[i + 2];
	// 			// RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n", flag0, fid, top_face_triangulation[i + 0], top_face_triangulation[i + 1], top_face_triangulation[i + 2]);
	// 		}

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Bottom face indices
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
	// 		{
	// 			int fid = i / 3;
	// 			faces(face_count + fid, 0) = (int)vertex_count + top_outline_face_vertex_indices[i + 2];
	// 			faces(face_count + fid, 1) = (int)vertex_count + top_outline_face_vertex_indices[i + 1];
	// 			faces(face_count + fid, 2) = (int)vertex_count + top_outline_face_vertex_indices[i + 0];

	// 			// RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n\n", flag1, face_count + fid, top_face_triangulation[i + 0] + vertex_count, top_face_triangulation[i + 1] + vertex_count, top_face_triangulation[i + 2] + vertex_count);
	// 		}

	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 		// -> Side face indices
	// 		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// 		// CGAL_Debug(sides.size());
	// 		for (int i = 0; i < sides.size(); i++)
	// 		{
	// 			faces(face_count * 2 + 2 * i + 0, 0) = sides[i][3];
	// 			faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
	// 			faces(face_count * 2 + 2 * i + 0, 2) = sides[i][1];

	// 			faces(face_count * 2 + 2 * i + 1, 0) = sides[i][3];
	// 			faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
	// 			faces(face_count * 2 + 2 * i + 1, 2) = sides[i][0];
	// 		}
	// 	}
	// }
}
