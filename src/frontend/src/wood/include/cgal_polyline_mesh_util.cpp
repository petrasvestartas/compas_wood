
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "cgal_polyline_mesh_util.h"

namespace cgal_polyline_mesh_util
{

    namespace internal
    {

        double length(double x, double y, double z)
        {

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

        bool unitize(IK::Vector_3 &vector)
        {
            bool rc = false;
            // Since x,y,z are floats, d will not be denormalized and the
            // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

            double d = length(vector.hx(), vector.hy(), vector.hz());
            if (d > 0.0)
            {
                double dx = vector.hx();
                double dy = vector.hy();
                double dz = vector.hz();
                vector = IK::Vector_3(
                    (dx / d),
                    (dy / d),
                    (dz / d));
                rc = true;
            }
            return rc;
        }

        void mark_domains(CGALCDT &ct, Face_handle start, int index, std::list<CGALCDT::Edge> &border)
        {
            if (start->info().nesting_level != -1)
                return;

            std::list<Face_handle> queue;
            queue.push_back(start);
            while (!queue.empty())
            {
                Face_handle fh = queue.front();
                queue.pop_front();
                if (fh->info().nesting_level == -1)
                {
                    fh->info().nesting_level = index;
                    for (int i = 0; i < 3; i++)
                    {
                        CGALCDT::Edge e(fh, i);
                        Face_handle n = fh->neighbor(i);
                        if (n->info().nesting_level == -1)
                        {
                            if (ct.is_constrained(e))
                                border.push_back(e);
                            else
                                queue.push_back(n);
                        }
                    }
                }
            }
        }

        void mark_domains(CGALCDT &CGALCDT)
        {
            for (CGALCDT::Face_handle f : CGALCDT.all_face_handles())
            {
                f->info().nesting_level = -1;
            }
            std::list<CGALCDT::Edge> border;
            mark_domains(CGALCDT, CGALCDT.infinite_face(), 0, border);
            while (!border.empty())
            {
                CGALCDT::Edge e = border.front();
                border.pop_front();
                Face_handle n = e.first->neighbor(e.second);
                if (n->info().nesting_level == -1)
                {
                    mark_domains(CGALCDT, n, e.first->info().nesting_level + 1, border);
                }
            }
        }

        CGAL::Aff_transformation_3<IK> plane_to_xy(const IK::Point_3 &origin, const IK::Plane_3 &plane)
        {
            auto x_axis = plane.base1();
            auto y_axis = plane.base2();
            auto z_axis = plane.orthogonal_vector();
            internal::unitize(x_axis);
            internal::unitize(y_axis);
            internal::unitize(z_axis);

            // transformation maps P0 to P1, P0+X0 to P1+X1, ...

            // Move to origin -> T0 translates point P0 to (0,0,0)
            CGAL::Aff_transformation_3<IK> t(CGAL::TRANSLATION, IK::Vector_3(-origin.x(), -origin.y(), -origin.z()));

            // Rotate ->
            CGAL::Aff_transformation_3<IK> f(
                x_axis.x(), x_axis.y(), x_axis.z(),
                y_axis.x(), y_axis.y(), y_axis.z(),
                z_axis.x(), z_axis.y(), z_axis.z());

            return f * t;
        }
    }

    void mesh_from_polylines(const std::vector<CGAL_Polyline> &polylines_with_holes, const IK::Plane_3 &base_plane, std::vector<int> &top_outline_face_vertex_indices, int &v_count, int &f_count)
    {
        //////////////////////////////////////////////////////////////////////////////
        // Create Transformation | Orient to 2D
        //////////////////////////////////////////////////////////////////////////////
        CGAL::Aff_transformation_3<IK> xform_toXY = internal::plane_to_xy(polylines_with_holes[0][0], base_plane);
        CGAL::Aff_transformation_3<IK> xform_toXY_Inv = xform_toXY.inverse();

        CGALCDT CGALCDT;
        for (int i = 0; i < polylines_with_holes.size(); i += 2)
        {
            Polygon_2 polygon_2d;
            for (int j = 0; j < polylines_with_holes[i].size() - 1; j++)
            {
                IK::Point_3 p = polylines_with_holes[i][j].transform(xform_toXY);
                auto pt_2d = Point(p.hx(), p.hy());

                CGALCDT::Locate_type l_t;
                int l_i;
                CGALCDT.locate(pt_2d, l_t, l_i);

                if (l_t == CGALCDT::VERTEX)
                {
                    printf("CPP CGALCDT::VERTEX \n");

                    return;
                }

                polygon_2d.push_back(pt_2d);
            }

            if (!polygon_2d.is_simple())
            {

                printf("CPP Not simple \n");
                return;
            }

            //////////////////////////////////////////////////////////////////////////////
            // Insert the polygons into a constrained triangulation
            //////////////////////////////////////////////////////////////////////////////
            //try
            //{
                CGALCDT.insert_constraint(polygon_2d.vertices_begin(), polygon_2d.vertices_end(), true);
            //}
            // catch (...)//const CGALCDT::Intersection_of_constraints_exception &
            // {
            //     printf("CPP CGALCDT::Intersection_of_constraints_exception \n");

            //     return;
            // }
        }

        //////////////////////////////////////////////////////////////////////////////
        // Mark facets that are inside the domain bounded by the polygon
        //////////////////////////////////////////////////////////////////////////////
        internal::mark_domains(CGALCDT);

        int count = 0;
        for (Face_handle f : CGALCDT.finite_face_handles())
        {
            if (f->info().in_domain())
            {
                ++count;
            }
        }

        std::map<CGALCDT::Vertex_handle, int> vertex_index;
        int k = 0;
        for (auto it = CGALCDT.vertices_begin(); it != CGALCDT.vertices_end(); ++it)
        {
            auto p = it->point();
            vertex_index[it] = k;
            k++;
        }
        v_count = k;

        // count vertices to check if there are same number of points as in polyline
        size_t vertex_count = 0;
        for (int i = 0; i < polylines_with_holes.size(); i += 2)
            vertex_count += polylines_with_holes[i].size() - 1;

        if (v_count != vertex_count)
        {
            top_outline_face_vertex_indices = std::vector<int>(0);
            return;
        }

        int number_of_faces = 0;
        for (Face_handle f : CGALCDT.finite_face_handles())
            if (f->info().in_domain())
                number_of_faces += 3;

        f_count = number_of_faces / 3;

        top_outline_face_vertex_indices.reserve(number_of_faces);
        for (Face_handle f : CGALCDT.finite_face_handles())
        {
            if (f->info().in_domain())
            {
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(0)]);
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(1)]);
                top_outline_face_vertex_indices.emplace_back(vertex_index[f->vertex(2)]);
            }
        }
    }

    // std::tuple<RowMatrixXd, RowMatrixXi> closed_mesh_from_polylines(const std::vector<CGAL_Polyline> &polylines_with_holes)
    // {
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Sanity Check
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     RowMatrixXd v_empty(0, 0);
    //     RowMatrixXi f_empty(0, 0);
    //     auto empty_tuple = std::make_tuple(v_empty, f_empty);

    //     if (polylines_with_holes.size() % 2 == 1)
    //         return empty_tuple;

    //     for (auto i = 0; i < polylines_with_holes.size(); i += 2)
    //     {
    //         auto a = polylines_with_holes[i].size();
    //         auto b = polylines_with_holes[i + 1].size();
    //         if (a != b)
    //             return empty_tuple;
    //         if (a < 2 || b < 2)
    //             return empty_tuple;
    //     }

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Compute average normal and create a plane
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     auto lastID = polylines_with_holes.size() - 2;
    //     auto len = polylines_with_holes[lastID].size() - 1;
    //     IK::Vector_3 average_normal = IK::Vector_3(0, 0, 0);

    //     for (int i = 0; i < len; i++)
    //     {
    //         auto prev = ((i - 1) + len) % len;
    //         auto next = ((i + 1) + len) % len;
    //         average_normal = average_normal + CGAL::cross_product(polylines_with_holes[lastID][i] - polylines_with_holes[lastID][prev], polylines_with_holes[lastID][next] - polylines_with_holes[lastID][i]);
    //     }

    //     IK::Plane_3 base_plane(polylines_with_holes[lastID][0], average_normal);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Create a mesh for top outlines
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     std::vector<int> top_outline_face_vertex_indices;
    //     int v, f;

    //     mesh_from_polylines(polylines_with_holes, base_plane, top_outline_face_vertex_indices, v, f);

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // Create mesh for the full plate
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Count vertices and faces
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     size_t vertex_count = 0;
    //     for (auto i = 0; i < polylines_with_holes.size(); i += 2)
    //         vertex_count += polylines_with_holes[i].size() - 1;

    //     if (v != vertex_count)
    //     {
    //         // CGAL_Debug(v);
    //         // CGAL_Debug(vertex_count);
    //         RowMatrixXd vv(0, 3);
    //         RowMatrixXi ff(0, 3);
    //         return std::make_tuple(vv, ff);
    //     }

    //     auto face_count = top_outline_face_vertex_indices.size() / 3;

    //     RowMatrixXd vertices(vertex_count * 2, 3);
    //     RowMatrixXi faces(face_count * 2 + vertex_count * 2, 3); //

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Top vertices
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     // Set vertex coordinates from all polylines a-b a-b
    //     int vid = 0;
    //     std::vector<std::array<int, 4>> sides;
    //     sides.reserve(vertex_count);

    //     bool holes = polylines_with_holes.size() > 2;

    //     for (auto i = 0; i < polylines_with_holes.size(); i += 2)
    //     {
    //         bool last = i == (polylines_with_holes.size() - 2);

    //         for (auto j = 0; j < polylines_with_holes[i].size() - 1; j++)
    //         {
    //             // vertices
    //             vertices(vid, 0) = polylines_with_holes[i][j].hx();
    //             vertices(vid, 1) = polylines_with_holes[i][j].hy();
    //             vertices(vid, 2) = polylines_with_holes[i][j].hz();

    //             // last faces
    //             if (j == polylines_with_holes[i].size() - 2)
    //             { // take vertices from beggining
    //                 auto n = polylines_with_holes[i].size() - 2;
    //                 std::array<int, 4> side{vid, vid - (int)n, vid - (int)n + (int)vertex_count, vid + 0 + (int)vertex_count};

    //                 if (holes)
    //                 {
    //                     sides.emplace_back(side);
    //                 }
    //                 else
    //                 {
    //                     sides.emplace_back(side);
    //                 }

    //                 // iterated faces
    //             }
    //             else
    //             { // take next vertices
    //                 std::array<int, 4> side = {
    //                     vid + 0 + (int)vertex_count,
    //                     vid + 1 + (int)vertex_count,
    //                     vid + 1,
    //                     vid,
    //                 };

    //                 if (holes)
    //                 {
    //                     // if (last) {
    //                     side = {
    //                         vid,
    //                         vid + 1,
    //                         vid + 1 + (int)vertex_count,
    //                         vid + 0 + (int)vertex_count,
    //                     };
    //                     sides.emplace_back(side);
    //                 }
    //                 else
    //                 {
    //                     side = {
    //                         vid,
    //                         vid + 1,
    //                         vid + 1 + (int)vertex_count,
    //                         vid + 0 + (int)vertex_count,
    //                     };
    //                     sides.emplace_back(side);
    //                 }
    //             }
    //             vid++;
    //         }
    //     }

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Bottom vertices
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     vid = 0;
    //     for (auto i = 0; i < polylines_with_holes.size(); i += 2)
    //     {
    //         for (auto j = 0; j < polylines_with_holes[i].size() - 1; j++)
    //         {
    //             // vertices
    //             vertices(vertex_count + vid, 0) = polylines_with_holes[i + 1][j].hx();
    //             vertices(vertex_count + vid, 1) = polylines_with_holes[i + 1][j].hy();
    //             vertices(vertex_count + vid, 2) = polylines_with_holes[i + 1][j].hz();
    //             vid++;
    //         }
    //     }

    //     // RhinoApp().Print("face_count %i vertex_count %i \n", output.FaceCount(), output.VertexCount());

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Top face indives
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     for (auto i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
    //     {
    //         int fid = i / 3;
    //         faces(fid, 0) = top_outline_face_vertex_indices[i + 0];
    //         faces(fid, 1) = top_outline_face_vertex_indices[i + 1];
    //         faces(fid, 2) = top_outline_face_vertex_indices[i + 2];
    //         // RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n", flag0, fid, top_face_triangulation[i + 0], top_face_triangulation[i + 1], top_face_triangulation[i + 2]);
    //     }

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Bottom face indices
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     for (auto i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
    //     {
    //         int fid = i / 3;
    //         faces(face_count + fid, 0) = (int)vertex_count + top_outline_face_vertex_indices[i + 2];
    //         faces(face_count + fid, 1) = (int)vertex_count + top_outline_face_vertex_indices[i + 1];
    //         faces(face_count + fid, 2) = (int)vertex_count + top_outline_face_vertex_indices[i + 0];

    //         // RhinoApp().Print("Triangulation flag %i faceID %i vertexIds %i %i %i \n\n", flag1, face_count + fid, top_face_triangulation[i + 0] + vertex_count, top_face_triangulation[i + 1] + vertex_count, top_face_triangulation[i + 2] + vertex_count);
    //     }

    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //     // -> Side face indices
    //     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //     // CGAL_Debug(sides.size());
    //     for (int i = 0; i < sides.size(); i++)
    //     {
    //         faces(face_count * 2 + 2 * i + 0, 0) = sides[i][3];
    //         faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
    //         faces(face_count * 2 + 2 * i + 0, 2) = sides[i][1];

    //         faces(face_count * 2 + 2 * i + 1, 0) = sides[i][3];
    //         faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
    //         faces(face_count * 2 + 2 * i + 1, 2) = sides[i][0];

    //         // if (polylines.size() == 2) {
    //         //      faces(face_count * 2 + 2 * i + 0, 0) = sides[i][3];
    //         //      faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
    //         //      faces(face_count * 2 + 2 * i + 0, 2) = sides[i][1];

    //         //     faces(face_count * 2 + 2 * i + 1, 0) = sides[i][3];
    //         //     faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
    //         //     faces(face_count * 2 + 2 * i + 1, 2) = sides[i][0];
    //         // } else {
    //         //     faces(face_count * 2 + 2 * i + 0, 0) = sides[i][1];
    //         //     faces(face_count * 2 + 2 * i + 0, 1) = sides[i][2];
    //         //     faces(face_count * 2 + 2 * i + 0, 2) = sides[i][3];

    //         //     faces(face_count * 2 + 2 * i + 1, 0) = sides[i][0];
    //         //     faces(face_count * 2 + 2 * i + 1, 1) = sides[i][1];
    //         //     faces(face_count * 2 + 2 * i + 1, 2) = sides[i][3];
    //         // }

    //         // bool flag = output.SetQuad(face_count * 2 + i, sides[i][3], sides[i][2], sides[i][1], sides[i][0]);
    //     }

    //     return std::make_tuple(vertices, faces);
    // }

    void closed_mesh_from_polylines_vnf(const std::vector<CGAL_Polyline> &polylines_with_holes_not_clean, std::vector<float> &out_vertices, std::vector<float> &out_normals, std::vector<int> &out_triangles, const double &scale)
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Sanity Check
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // RowMatrixXd v_empty(0, 0);
        // RowMatrixXi f_empty(0, 0);
        // auto empty_tuple = std::make_tuple(v_empty, f_empty);

        std::vector<CGAL_Polyline> polylines_with_holes;
        polylines_with_holes.reserve(polylines_with_holes_not_clean.size());

        // ignore line segments with less than 2 points
        for (auto &polyline : polylines_with_holes_not_clean)
            if (polyline.size() > 2)
                polylines_with_holes.emplace_back(polyline);

        // if there polyline pairs are not even, return empty mesh
        if (polylines_with_holes_not_clean.size() % 2 == 1)
            return;

        for (auto i = 0; i < polylines_with_holes.size(); i += 2)
        {
            auto a = polylines_with_holes[i].size();
            auto b = polylines_with_holes[i + 1].size();
            if (a != b)
                return;
            if (a < 2 || b < 2)
                return;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Clean duplicate points
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> polylines(polylines_with_holes.size());

        for (auto i = 0; i < polylines_with_holes.size(); i += 2)
        {
            polylines[i + 0].reserve(polylines_with_holes[i + 0].size());
            polylines[i + 1].reserve(polylines_with_holes[i + 1].size());
            polylines[i + 0].emplace_back(polylines_with_holes[i + 0][0]);
            polylines[i + 1].emplace_back(polylines_with_holes[i + 1][0]);
            for (auto j = 1; j < polylines_with_holes[i + 0].size(); j++)
            {
                if (CGAL::squared_distance(polylines_with_holes[i + 0][j - 1], polylines_with_holes[i + 0][j]) > wood_globals::DISTANCE_SQUARED)
                {
                    polylines[i + 0].emplace_back(polylines_with_holes[i + 0][j]);
                    polylines[i + 1].emplace_back(polylines_with_holes[i + 1][j]);
                }
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Compute average normal and create a plane
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto lastID = polylines.size() - 2;
        auto len = polylines[lastID].size() - 1;
        IK::Vector_3 average_normal = IK::Vector_3(0, 0, 0);

        for (auto i = 0; i < len; i++)
        {
            auto prev = ((i - 1) + len) % len;
            auto next = ((i + 1) + len) % len;
            average_normal = average_normal + CGAL::cross_product(polylines[lastID][i] - polylines[lastID][prev], polylines[lastID][next] - polylines[lastID][i]);
        }
        internal::unitize(average_normal);

        // flip if needed
        IK::Plane_3 base_plane(polylines[lastID][0], average_normal);
        if (base_plane.has_on_positive_side(polylines[polylines.size() - 1][0]))
            average_normal *= -1;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create a mesh for top outlines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> top_outline_face_vertex_indices;
        int v, f;
        mesh_from_polylines(polylines, base_plane, top_outline_face_vertex_indices, v, f);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Create mesh for the full plate
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Count vertices and faces
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        size_t vertex_count = 0;
        for (int i = 0; i < polylines.size(); i += 2)
            vertex_count += polylines[i].size() - 1;

        if (v != vertex_count)
        {
            return;
        }

        auto face_count = top_outline_face_vertex_indices.size() / 3;

        std::vector<float> out_vertices_temp;
        out_vertices_temp.reserve(vertex_count * 2 * 3);
        out_vertices.reserve(face_count * 2 * 3);
        out_normals.reserve(face_count * 2 * 3);
        out_triangles.reserve(face_count * 2 * 3);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Top vertices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // Set vertex coordinates from all polylines a-b a-b
        int vid = 0;
        std::vector<std::array<int, 4>> sides;
        sides.reserve(vertex_count);

        bool holes = polylines.size() > 2;

        for (auto i = 0; i < polylines.size(); i += 2)
        {
            bool last = i == (polylines.size() - 2);

            for (auto j = 0; j < polylines[i].size() - 1; j++)
            {
                // vertices
                out_vertices_temp.emplace_back((float)polylines[i][j].hx() / scale);
                out_vertices_temp.emplace_back((float)polylines[i][j].hy() / scale);
                out_vertices_temp.emplace_back((float)polylines[i][j].hz() / scale);

                // last faces
                if (j == polylines[i].size() - 2)
                { // take vertices from beggining
                    auto n = polylines[i].size() - 2;
                    std::array<int, 4> side{vid, vid - (int)n, vid - (int)n + (int)vertex_count, vid + 0 + (int)vertex_count};

                    if (holes)
                    {
                        sides.emplace_back(side);
                    }
                    else
                    {
                        sides.emplace_back(side);
                    }
                }
                else
                { // take next vertices
                    std::array<int, 4> side = {
                        vid + 0 + (int)vertex_count,
                        vid + 1 + (int)vertex_count,
                        vid + 1,
                        vid,
                    };

                    if (holes)
                    {

                        side = {
                            vid,
                            vid + 1,
                            vid + 1 + (int)vertex_count,
                            vid + 0 + (int)vertex_count,
                        };
                        sides.emplace_back(side);
                    }
                    else
                    {
                        side = {
                            vid,
                            vid + 1,
                            vid + 1 + (int)vertex_count,
                            vid + 0 + (int)vertex_count,
                        };
                        sides.emplace_back(side);
                    }
                }

                vid++;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Bottom vertices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        vid = 0;
        for (auto i = 0; i < polylines.size(); i += 2)
        {
            for (auto j = 0; j < polylines[i].size() - 1; j++)
            {
                // vertices
                out_vertices_temp.emplace_back((float)polylines[i + 1][j].hx() / scale);
                out_vertices_temp.emplace_back((float)polylines[i + 1][j].hy() / scale);
                out_vertices_temp.emplace_back((float)polylines[i + 1][j].hz() / scale);
                vid++;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Top face indices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
        {
            int fid = i / 3;
            int a = top_outline_face_vertex_indices[i + 0];
            int b = top_outline_face_vertex_indices[i + 1];
            int c = top_outline_face_vertex_indices[i + 2];

            int v_count = out_triangles.size();
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());

            out_vertices.emplace_back(out_vertices_temp[a * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[a * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[a * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[b * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[b * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[b * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[c * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[c * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[c * 3 + 2]);

            // std::cout << out_vertices_temp[a * 3 + 0] << " " << out_vertices_temp[a * 3 + 1] << " " << out_vertices_temp[a * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[b * 3 + 0] << " " << out_vertices_temp[b * 3 + 1] << " " << out_vertices_temp[b * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[c * 3 + 0] << " " << out_vertices_temp[c * 3 + 1] << " " << out_vertices_temp[c * 3 + 2] << "\n";

            out_normals.emplace_back(average_normal.hx());
            out_normals.emplace_back(average_normal.hy());
            out_normals.emplace_back(average_normal.hz());

            out_normals.emplace_back(average_normal.hx());
            out_normals.emplace_back(average_normal.hy());
            out_normals.emplace_back(average_normal.hz());

            out_normals.emplace_back(average_normal.hx());
            out_normals.emplace_back(average_normal.hy());
            out_normals.emplace_back(average_normal.hz());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Bottom face indices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < top_outline_face_vertex_indices.size(); i += 3)
        {
            int fid = i / 3;
            int a = (int)vertex_count + top_outline_face_vertex_indices[i + 2];
            int b = (int)vertex_count + top_outline_face_vertex_indices[i + 1];
            int c = (int)vertex_count + top_outline_face_vertex_indices[i + 0];

            int v_count = out_triangles.size();
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());

            out_vertices.emplace_back(out_vertices_temp[a * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[a * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[a * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[b * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[b * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[b * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[c * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[c * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[c * 3 + 2]);

            // std::cout << out_vertices_temp[a * 3 + 0] << " " << out_vertices_temp[a * 3 + 1] << " " << out_vertices_temp[a * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[b * 3 + 0] << " " << out_vertices_temp[b * 3 + 1] << " " << out_vertices_temp[b * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[c * 3 + 0] << " " << out_vertices_temp[c * 3 + 1] << " " << out_vertices_temp[c * 3 + 2] << "\n";

            out_normals.emplace_back(-average_normal.hx());
            out_normals.emplace_back(-average_normal.hy());
            out_normals.emplace_back(-average_normal.hz());

            out_normals.emplace_back(-average_normal.hx());
            out_normals.emplace_back(-average_normal.hy());
            out_normals.emplace_back(-average_normal.hz());

            out_normals.emplace_back(-average_normal.hx());
            out_normals.emplace_back(-average_normal.hy());
            out_normals.emplace_back(-average_normal.hz());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // -> Side face indices
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // CGAL_Debug(sides.size());
        for (int i = 0; i < sides.size(); i++)
        {
            int a0 = sides[i][3];
            int b0 = sides[i][2];
            int c0 = sides[i][1];

            int a1 = sides[i][3];
            int b1 = sides[i][1];
            int c1 = sides[i][0];

            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());

            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());
            out_triangles.emplace_back(out_triangles.size());

            out_vertices.emplace_back(out_vertices_temp[a0 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[a0 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[a0 * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[b0 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[b0 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[b0 * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[c0 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[c0 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[c0 * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[a1 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[a1 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[a1 * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[b1 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[b1 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[b1 * 3 + 2]);

            out_vertices.emplace_back(out_vertices_temp[c1 * 3 + 0]);
            out_vertices.emplace_back(out_vertices_temp[c1 * 3 + 1]);
            out_vertices.emplace_back(out_vertices_temp[c1 * 3 + 2]);

            // std::cout << out_vertices_temp[a0 * 3 + 0] << " " << out_vertices_temp[a0 * 3 + 1] << " " << out_vertices_temp[a0 * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[b0 * 3 + 0] << " " << out_vertices_temp[b0 * 3 + 1] << " " << out_vertices_temp[b0 * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[c0 * 3 + 0] << " " << out_vertices_temp[c0 * 3 + 1] << " " << out_vertices_temp[c0 * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[a1 * 3 + 0] << " " << out_vertices_temp[a1 * 3 + 1] << " " << out_vertices_temp[a1 * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[b1 * 3 + 0] << " " << out_vertices_temp[b1 * 3 + 1] << " " << out_vertices_temp[b1 * 3 + 2] << "\n";
            // std::cout << out_vertices_temp[c1 * 3 + 0] << " " << out_vertices_temp[c1 * 3 + 1] << " " << out_vertices_temp[c1 * 3 + 2] << "\n";

            IK::Point_3 coord_0(out_vertices_temp[a0 * 3 + 0], out_vertices_temp[a0 * 3 + 1], out_vertices_temp[a0 * 3 + 2]);
            IK::Point_3 coord_1(out_vertices_temp[b0 * 3 + 0], out_vertices_temp[b0 * 3 + 1], out_vertices_temp[b0 * 3 + 2]);
            IK::Point_3 coord_2(out_vertices_temp[c0 * 3 + 0], out_vertices_temp[c0 * 3 + 1], out_vertices_temp[c0 * 3 + 2]);
            IK::Vector_3 normal_0 = -CGAL::cross_product(coord_0 - coord_1, coord_2 - coord_1);
            internal::unitize(normal_0);
            // std::cout << normal_0.hx() << " " << normal_0.hy() << " " << normal_0.hz() << "\n";
            // std::cout << coord_1.hx() << " " << coord_1.hy() << " " << coord_1.hz() << "\n";

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());

            IK::Point_3 coord_3(out_vertices_temp[a1 * 3 + 0], out_vertices_temp[a1 * 3 + 1], out_vertices_temp[a1 * 3 + 2]);
            IK::Point_3 coord_4(out_vertices_temp[b1 * 3 + 0], out_vertices_temp[b1 * 3 + 1], out_vertices_temp[b1 * 3 + 2]);
            IK::Point_3 coord_5(out_vertices_temp[c1 * 3 + 0], out_vertices_temp[c1 * 3 + 1], out_vertices_temp[c1 * 3 + 2]);

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());

            out_normals.emplace_back(normal_0.hx());
            out_normals.emplace_back(normal_0.hy());
            out_normals.emplace_back(normal_0.hz());
        }

        return;
    }
}
