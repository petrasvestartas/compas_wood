#pragma once
#include "stdafx.h"
#include "cgal.h"

namespace cgal_mesh_util {
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
                        if (ct.is_constrained(e))
                            border.push_back(e);
                        else
                            queue.push_back(n);
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
        IK::Point_3 O0, IK::Plane_3 plane) {
        auto X0 = plane.base1();
        auto Y0 = plane.base2();
        auto Z0 = plane.orthogonal_vector();
        cgal_vector_util::Unitize(X0);
        cgal_vector_util::Unitize(Y0);
        cgal_vector_util::Unitize(Z0);

        // transformation maps P0 to P1, P0+X0 to P1+X1, ...

        //Move to origin -> T0 translates point P0 to (0,0,0)
        CGAL::Aff_transformation_3<IK> T0(CGAL::TRANSLATION, IK::Vector_3(0 - O0.x(), 0 - O0.y(), 0 - O0.z()));

        //Rotate ->
        CGAL::Aff_transformation_3<IK> F0(
            X0.x(), X0.y(), X0.z(),
            Y0.x(), Y0.y(), Y0.z(),
            Z0.x(), Z0.y(), Z0.z());

        return F0 * T0;
    }

    inline void mesh_from_polylines(std::vector<CGAL_Polyline>& polylines_with_holes, IK::Plane_3& base_plane, std::vector<int>& top_outline_face_vertex_indices, int& v, int& f) {
        //////////////////////////////////////////////////////////////////////////////
        //Create Transformation | Orient to 2D
        //////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = plane_to_XY_mesh(polylines_with_holes[0][0], base_plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

        //for (int i = 0; i < polylines_with_holes.size(); i += 2) {
        //    for (int j = 0; j < polylines_with_holes[i+1].size() - 1; j++) {
        //        IK::Point_3 p = polylines_with_holes[i+1][j].transform(xform_toXY);
        //        CGAL_Debug(p.hz());
        //   }
        //}

        CDT cdt;
        for (int i = 0; i < polylines_with_holes.size(); i += 2) {
            Polygon_2 polygon_2d;
            for (int j = 0; j < polylines_with_holes[i].size() - 1; j++) {
                IK::Point_3 p = polylines_with_holes[i][j].transform(xform_toXY);
                auto pt_2d = Point(p.hx(), p.hy());

                CDT::Locate_type l_t;
                int l_i;
                cdt.locate(pt_2d, l_t, l_i);

                if (l_t == CDT::VERTEX) {
                    printf("CPP CDT::VERTEX \n");

                    return;
                }

                polygon_2d.push_back(pt_2d);
            }

            if (!polygon_2d.is_simple()) {
                printf("CPP Not simple \n");
                return;
            }

            //////////////////////////////////////////////////////////////////////////////
            //Insert the polygons into a constrained triangulation
            //////////////////////////////////////////////////////////////////////////////
            try {
                cdt.insert_constraint(polygon_2d.vertices_begin(), polygon_2d.vertices_end(), true);
            } catch (const CDT::Intersection_of_constraints_exception&) {
                printf("CPP CDT::Intersection_of_constraints_exception \n");

                return;
            }
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
        }
        v = k;

        //count vertices to check if there are same number of points as in polyline
        int vertex_count = 0;
        for (int i = 0; i < polylines_with_holes.size(); i += 2)
            vertex_count += polylines_with_holes[i].size() - 1;

        if (v != vertex_count) {
            top_outline_face_vertex_indices = std::vector<int>(0);
            return;
        }

        int number_of_faces = 0;
        for (Face_handle f : cdt.finite_face_handles()) {
            if (f->info().in_domain()) {
                number_of_faces += 3;
            }
        }
        f = number_of_faces / 3;

        top_outline_face_vertex_indices.reserve(number_of_faces);
        for (Face_handle f : cdt.finite_face_handles()) {
            if (f->info().in_domain()) {
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(0)]);
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(1)]);
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(2)]);
            }
        }
    }

    inline std::tuple<RowMatrixXd, RowMatrixXi> closed_mesh_from_polylines(std::vector<CGAL_Polyline>& polylines) {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Compute average normal and create a plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //CGAL_Debug("Polyline Length");
        //for (int i = 0; i < polylines.size(); i += 2)
        //    CGAL_Debug(cgal_polyline_util::polyline_length(polylines[i]));

        int lastID = polylines.size() - 2;
        int len = polylines[lastID].size() - 1;
        IK::Vector_3 average_normal = IK::Vector_3(0, 0, 0);

        for (int i = 0; i < len; i++) {
            int prev = ((i - 1) + len) % len;
            int next = ((i + 1) + len) % len;
            average_normal = average_normal + CGAL::cross_product(polylines[lastID][i] - polylines[lastID][prev], polylines[lastID][next] - polylines[lastID][i]);
        }

        IK::Plane_3 base_plane(polylines[lastID][0], average_normal);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Create a mesh for top outlines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> top_outline_face_vertex_indices;
        int v, f;
        cgal_mesh_util::mesh_from_polylines(polylines, base_plane, top_outline_face_vertex_indices, v, f);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Create mesh for the full plate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Count vertices and faces
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        int vertex_count = 0;
        for (int i = 0; i < polylines.size(); i += 2)
            vertex_count += polylines[i].size() - 1;

        if (v != vertex_count) {
            //CGAL_Debug(v);
            //CGAL_Debug(vertex_count);
            RowMatrixXd vv(0, 3);
            RowMatrixXi ff(0, 3);
            return std::make_tuple(vv, ff);
        }

        int face_count = top_outline_face_vertex_indices.size() / 3;

        RowMatrixXd vertices(vertex_count * 2, 3);
        RowMatrixXi faces(face_count * 2 + vertex_count * 2, 3); //

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Top vertices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //Set vertex coordinates from all polylines a-b a-b
        int vid = 0;
        std::vector<std::array<int, 4>> sides;
        sides.reserve(vertex_count);

        bool holes = polylines.size() > 2;

        for (int i = 0; i < polylines.size(); i += 2) {
            bool last = i == (polylines.size() - 2);

            //CGAL_Debug(last);
            //CGAL_Debug(i );
            //CGAL_Debug(polylines.size() - 2);

            for (int j = 0; j < polylines[i].size() - 1; j++) {
                //vertices
                vertices(vid, 0) = polylines[i][j].hx();
                vertices(vid, 1) = polylines[i][j].hy();
                vertices(vid, 2) = polylines[i][j].hz();

                //last faces
                if (j == polylines[i].size() - 2) { //take vertices from beggining
                    int n = polylines[i].size() - 2;
                    std::array<int, 4> side{ vid, vid - n, vid - n + vertex_count, vid + 0 + vertex_count };

                    if (holes) {
                        sides.emplace_back(side);
                        //if (last) {
                        //    sides.emplace_back(side);
                        //} else {
                        //    side = {
                        //        vid + 0 + vertex_count,
                        //        vid - n + vertex_count,
                        //        vid - n,
                        //        vid
                        //    };
                        //    sides.emplace_back(side);
                        //}
                    } else {
                        sides.emplace_back(side);
                    }

                    //iterated faces
                } else { //take next vertices
                    std::array<int, 4> side = {
                        vid + 0 + vertex_count,
                        vid + 1 + vertex_count,
                        vid + 1,
                        vid,
                    };

                    if (holes) {
                        //if (last) {
                        side = {
                            vid,
                            vid + 1,
                            vid + 1 + vertex_count,
                            vid + 0 + vertex_count,
                        };
                        sides.emplace_back(side);
                        //}
                        // else {
                        //    sides.emplace_back(side);
                        //}
                    } else {
                        side = {
                            vid,
                            vid + 1,
                            vid + 1 + vertex_count,
                            vid + 0 + vertex_count,
                        };
                        sides.emplace_back(side);
                    }
                }

                //RhinoApp().Print("Vertices flag %i index %i vertices %f %f %f \n", flag0, vid, polylines[i][j].hx(), polylines[i][j].hy(), polylines[i][j].hz());

                vid++;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Bottom vertices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        vid = 0;
        for (int i = 0; i < polylines.size(); i += 2) {
            for (int j = 0; j < polylines[i].size() - 1; j++) {
                //vertices
                vertices(vertex_count + vid, 0) = polylines[i + 1][j].hx();
                vertices(vertex_count + vid, 1) = polylines[i + 1][j].hy();
                vertices(vertex_count + vid, 2) = polylines[i + 1][j].hz();

                //RhinoApp().Print("Vertices flag %i index %i vertices %f %f %f \n\n", flag1, vid + vertex_count, polylines[i + 1][j].hx(), polylines[i + 1][j].hy(), polylines[i + 1][j].hz());
                vid++;
            }
        }

        //RhinoApp().Print("face_count %i vertex_count %i \n", output.FaceCount(), output.VertexCount());

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Top face indives
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3) {
            int fid = i / 3;
            faces(fid, 0) = top_outline_face_vertex_indices[i + 0];
            faces(fid, 1) = top_outline_face_vertex_indices[i + 1];
            faces(fid, 2) = top_outline_face_vertex_indices[i + 2];
            //RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n", flag0, fid, top_face_triangulation[i + 0], top_face_triangulation[i + 1], top_face_triangulation[i + 2]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Bottom face indices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3) {
            int fid = i / 3;
            faces(face_count + fid, 0) = vertex_count + top_outline_face_vertex_indices[i + 2];
            faces(face_count + fid, 1) = vertex_count + top_outline_face_vertex_indices[i + 1];
            faces(face_count + fid, 2) = vertex_count + top_outline_face_vertex_indices[i + 0];

            //RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n\n", flag1, face_count + fid, top_face_triangulation[i + 0] + vertex_count, top_face_triangulation[i + 1] + vertex_count, top_face_triangulation[i + 2] + vertex_count);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Side face indices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        //CGAL_Debug(sides.size());
        for (int i = 0; i < sides.size(); i++) {
            faces(face_count * 2 + 2 * i + 0, 0) = sides[i][3];
            faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
            faces(face_count * 2 + 2 * i + 0, 2) = sides[i][1];

            faces(face_count * 2 + 2 * i + 1, 0) = sides[i][3];
            faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
            faces(face_count * 2 + 2 * i + 1, 2) = sides[i][0];

            //if (polylines.size() == 2) {
            //     faces(face_count * 2 + 2 * i + 0, 0) = sides[i][3];
            //     faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
            //     faces(face_count * 2 + 2 * i + 0, 2) = sides[i][1];

            //     faces(face_count * 2 + 2 * i + 1, 0) = sides[i][3];
            //     faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
            //     faces(face_count * 2 + 2 * i + 1, 2) = sides[i][0];
            // } else {
            //     faces(face_count * 2 + 2 * i + 0, 0) = sides[i][1];
            //     faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
            //     faces(face_count * 2 + 2 * i + 0, 2) = sides[i][3];

            //     faces(face_count * 2 + 2 * i + 1, 0) = sides[i][0];
            //     faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
            //     faces(face_count * 2 + 2 * i + 1, 2) = sides[i][3];
            // }

            //bool flag = output.SetQuad(face_count * 2 + i, sides[i][3], sides[i][2], sides[i][1], sides[i][0]);
            //	RhinoApp().Print("Triangulation flag %i",flag);
        }

        return std::make_tuple(vertices, faces);
    }
}
