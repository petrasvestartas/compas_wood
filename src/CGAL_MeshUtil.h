#pragma once
#include "stdafx.h"
#include "compas.h"



namespace CGAL_MeshUtil {

	inline void
		mark_domains(CDT& ct,
			Face_handle start,
			int index,
			std::list<CDT::Edge>& border) {
		if (start->info().nesting_level != -1) {
			return;
		}
		std::list<Face_handle> queue;
		queue.push_back(start);
		while (!queue.empty()) {
			Face_handle fh = queue.front();
			queue.pop_front();
			if (fh->info().nesting_level == -1) {
				fh->info().nesting_level = index;
				for (int i = 0; i < 3; i++) {
					CDT::Edge e(fh, i);
					Face_handle n = fh->neighbor(i);
					if (n->info().nesting_level == -1) {
						if (ct.is_constrained(e)) border.push_back(e);
						else queue.push_back(n);
					}
				}
			}
		}
	}
	//explore set of facets connected with non constrained edges,
	//and attribute to each such set a nesting level.
	//We start from facets incident to the infinite vertex, with a nesting
	//level of 0. Then we recursively consider the non-explored facets incident
	//to constrained edges bounding the former set and increase the nesting level by 1.
	//Facets in the domain are those with an odd nesting level.
	inline void
		mark_domains(CDT& cdt) {
		for (CDT::Face_handle f : cdt.all_face_handles()) {
			f->info().nesting_level = -1;
		}
		std::list<CDT::Edge> border;
		mark_domains(cdt, cdt.infinite_face(), 0, border);
		while (!border.empty()) {
			CDT::Edge e = border.front();
			border.pop_front();
			Face_handle n = e.first->neighbor(e.second);
			if (n->info().nesting_level == -1) {
				mark_domains(cdt, n, e.first->info().nesting_level + 1, border);
			}
		}
	}

	inline CGAL::Aff_transformation_3<IK> plane_to_XY_mesh(
		IK::Point_3 O0, IK::Plane_3 plane
	) {
		auto X0 = plane.base1();
		auto Y0 = plane.base2();
		auto Z0 = plane.orthogonal_vector();
		CGAL_VectorUtil::Unitize(X0);
		CGAL_VectorUtil::Unitize(Y0);
		CGAL_VectorUtil::Unitize(Z0);

		// transformation maps P0 to P1, P0+X0 to P1+X1, ...

		//Move to origin -> T0 translates point P0 to (0,0,0)
		CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));


		//Rotate ->
		CGAL::Aff_transformation_3<IK> F0(
			X0.x(), X0.y(), X0.z(),
			Y0.x(), Y0.y(), Y0.z(),
			Z0.x(), Z0.y(), Z0.z()
		);


		return  F0 * T0;

	}



	inline void mesh_from_polylines(std::vector<CGAL_Polyline>& polylines_with_holes, IK::Plane_3& base_plane, std::vector<int>& top_face_vertices) {

		//////////////////////////////////////////////////////////////////////////////
		//Create Transformation | Orient to 2D 
		//////////////////////////////////////////////////////////////////////////////
		CGAL::Aff_transformation_3<IK> xform_toXY = plane_to_XY_mesh(polylines_with_holes[0][0], base_plane);
		CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

		CDT cdt;
		for (int i = 0; i < polylines_with_holes.size(); i += 2) {

			Polygon_2 polygon_2d;
			for (int j = 0; j < polylines_with_holes[i].size() - 1; j++) {
				IK::Point_3 p = polylines_with_holes[i][j].transform(xform_toXY);
				polygon_2d.push_back(Point(p.hx(), p.hy()));
				//myfile << polylines_with_holes[i][j][0] << " " << polylines_with_holes[i][j][1] << " " << polylines_with_holes[i][j][2] << "\n";
			}

			//////////////////////////////////////////////////////////////////////////////
			//Insert the polygons into a constrained triangulation
			//////////////////////////////////////////////////////////////////////////////
			cdt.insert_constraint(polygon_2d.vertices_begin(), polygon_2d.vertices_end(), true);
		}

		//////////////////////////////////////////////////////////////////////////////
		//Mark facets that are inside the domain bounded by the polygon
		//////////////////////////////////////////////////////////////////////////////
		mark_domains(cdt);

		int count = 0;
		for (Face_handle f : cdt.finite_face_handles()) {
			if (f->info().in_domain()) {

				++count;

			}
		}


		std::map<CDT::Vertex_handle, int> vertex_index;
		int k = 0;
		for (auto it = cdt.vertices_begin(); it != cdt.vertices_end(); ++it) {
			auto p = it->point();
			vertex_index[it] = k;
			k++;
			//myfile << p[0] << " " << p[1] << "\n";
		}


		int number_of_faces = 0;
		for (Face_handle f : cdt.finite_face_handles()) {
			if (f->info().in_domain()) {
				number_of_faces += 3;
			}
		}

		top_face_vertices.reserve(number_of_faces);
		for (Face_handle f : cdt.finite_face_handles()) {
			if (f->info().in_domain()) {
				top_face_vertices.emplace_back(vertex_index[f->vertex(0)]);
				top_face_vertices.emplace_back(vertex_index[f->vertex(1)]);
				top_face_vertices.emplace_back(vertex_index[f->vertex(2)]);

			}
		}

	}



}
