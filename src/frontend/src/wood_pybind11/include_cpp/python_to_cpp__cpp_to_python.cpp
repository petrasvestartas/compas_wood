#include "../../../stdafx.h"
#include "python_to_cpp__cpp_to_python.h"

namespace python_to_cpp__cpp_to_python
{
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // PYTHON TO CPP CONVERSIONS
    // These conversion are used in CPP
    // Mesh
    // Polyline
    // std::vector<std::vector<IK::Vector_3>>
    // std::vector<std::vector<int>> and <std::vector<int>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP Mesh
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    Mesh mesh_from_vertices_and_faces(const RowMatrixXd &vertices, const RowMatrixXi &faces)
    {
        // get count of rows in both matrices
        // each row consists of 3 doubles or 3 integers
        // row 0 -------------------- 0.6856,0.9856,0.7856 | or 7,5,1
        // row 1 -------------------- 0.5853,0.7856,0.3551 | or 0,3,1
        // row 2 -------------------- 0.9856,0.3551,0.7856 | or 5,2,7
        // ...
        // row n -------------------- 0.7851,0.3856,0.9853 | or 1,8,9
        int vertices_count = vertices.rows();
        int faces_count = faces.rows();

        // create a cgal mesh data-structure and vertex id container
        Mesh mesh;
        std::vector<Mesh::Vertex_index> vertex_indices(vertices_count);

        // add vertices to the mesh and the index descriptor
        for (size_t i = 0; i < vertices_count; i++)
        {
            // index descriptor is used to store indices of vertices, indices are retrieved while adding points to the mesh
            vertex_indices[i] = mesh.add_vertex(IK::Point_3(vertices(i, 0), vertices(i, 1), vertices(i, 2)));
        }

        // add faces, see how vertex indices are added to the mesh, this only work when rows have the same amount of vertices per face
        Mesh::Vertex_index a;
        Mesh::Vertex_index b;
        Mesh::Vertex_index c;

        for (size_t i = 0; i < faces_count; i++)
        {
            a = vertex_indices[faces(i, 0)];
            b = vertex_indices[faces(i, 1)];
            c = vertex_indices[faces(i, 2)];
            mesh.add_face(a, b, c);
        }

        // output
        return mesh;
    }

    Mesh ngon_from_vertices_and_faces(const RowMatrixXd &vertices, const std::vector<std::vector<int>> &faces)
    {

        // get count of rows in both matrices
        // each row consists of 3 doubles
        // row 0 -------------------- 0.6856,0.9856,0.7856
        // row 1 -------------------- 0.5853,0.7856,0.3551
        // row 2 -------------------- 0.9856,0.3551,0.7856
        // ...
        // row n -------------------- 0.7851,0.3856,0.9853
        int vertices_count = vertices.rows();

        // create a cgal mesh data-structure and vertex id container
        Mesh mesh;
        std::vector<Mesh::Vertex_index> index_descriptor(vertices_count);

        // add vertices to the mesh and the index descriptor
        for (size_t i = 0; i < vertices_count; i++)
        {
            // index descriptor is used to store indices of vertices, indices are retrieved while adding points to the mesh
            index_descriptor[i] = mesh.add_vertex(IK::Point_3(vertices(i, 0), vertices(i, 1), vertices(i, 2)));
        }

        // add faces
        for (size_t i = 0; i < faces.size(); i++)
        {
            // create a face of the vector of integers, the number of vertices per face can change, so a vector is used, instead of matrix
            std::vector<Mesh::Vertex_index> face;
            for (size_t j = 0; j < faces[i].size(); j++)
                face.push_back(index_descriptor[faces[i][j]]);

            // add that face to the mesh
            mesh.add_face(face);
        }

        // output
        return mesh;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP Polyline
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<CGAL_Polyline> matrix_to_polylines(const RowMatrixXd &vertices)
    {
    }

    void coord_to_vector_of_polylines(std::vector<std::vector<double>> &nested_vector_of_numbers, std::vector<std::vector<IK::Point_3>> &vector_of_polylines)
    {
        if (nested_vector_of_numbers.size() == 0) return;

            vector_of_polylines.reserve(nested_vector_of_numbers.size());
        for (auto &polyline : nested_vector_of_numbers)
        {
            if (polyline.size() == 0)
                continue;

            vector_of_polylines.push_back(std::vector<IK::Point_3>());
            vector_of_polylines.back().reserve(polyline.size() / 3);

            for (size_t i = 0; i < polyline.size(); i += 3)
                vector_of_polylines.back().emplace_back(polyline[i], polyline[i + 1], polyline[i + 2]);
          
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP std::vector<std::vector<IK::Vector_3>>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<IK::Vector_3>> matrix_to_nested_vector(const RowMatrixXd &vertices)
    {
    }

    void coord_to_vector_of_vectors(std::vector<std::vector<double>> &nested_vector_of_numbers, std::vector<std::vector<IK::Vector_3>> &nested_vector_of_vectors)
    {
        nested_vector_of_vectors.reserve(nested_vector_of_numbers.size());
        for (auto &vector : nested_vector_of_numbers)
        {
            nested_vector_of_vectors.push_back(std::vector<IK::Vector_3>());
            nested_vector_of_vectors.back().reserve(vector.size() / 3);
            for (size_t i = 0; i < vector.size(); i += 3)
            {
                nested_vector_of_vectors.back().push_back(IK::Vector_3(vector[i], vector[i + 1], vector[i + 2]));
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Python to CPP std::vector<std::vector<int>> and <std::vector<int>
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<std::vector<int>> matrix_to_nested_vector(const RowMatrixXi &indices)
    {
    }

    std::vector<int> matrix_to_vector(const RowMatrixXi &indices)
    {
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // CPP TO PYTHON CONVERSIONS
    // These conversion are used in Python
    // Mesh
    // Polyline
    // XML
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Mesh to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    struct ResultMesh result_mesh;

    std::tuple<RowMatrixXd, RowMatrixXi> mesh_to_vertices_and_faces(const Mesh &mesh)
    {

        // create a vertex and face matrix
        int vertices_count = mesh.number_of_vertices();
        int faces_count = mesh.number_of_faces();

        RowMatrixXd vertex_matrix(vertices_count, 3);
        RowMatrixXi face_matrix(faces_count, 3);

        // assign vertices
        Mesh::Property_map<Mesh::Vertex_index, IK::Point_3> location = mesh.points();
        for (Mesh::Vertex_index &vd : mesh.vertices())
        {
            vertex_matrix(vd.idx(), 0) = (double)location[vd][0];
            vertex_matrix(vd.idx(), 1) = (double)location[vd][1];
            vertex_matrix(vd.idx(), 2) = (double)location[vd][2];
        }

        // assign faces
        for (Mesh::Face_index &fd : mesh.faces())
        {
            int i = 0;
            for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh))
            {
                face_matrix(fd.idx(), i) = (int)vd;
                i++;
            }
        }

        // output
        std::tuple<RowMatrixXd, RowMatrixXi> result = std::make_tuple(vertex_matrix, face_matrix);
        return result;
    }

    std::tuple<RowMatrixXd, RowMatrixXi> quadmesh_to_vertices_and_faces(const Mesh &mesh)
    {

        // create a vertex and face matrix

        int vertices_count = mesh.number_of_vertices();
        int faces_count = mesh.number_of_faces();

        RowMatrixXd vertex_matrix(vertices_count, 3);
        RowMatrixXi face_matrix(faces_count, 4);

        // assign vertices
        Mesh::Property_map<Mesh::Vertex_index, IK::Point_3> location = mesh.points();

        for (Mesh::Vertex_index &vd : mesh.vertices())
        {
            vertex_matrix(vd.idx(), 0) = (double)location[vd][0];
            vertex_matrix(vd.idx(), 1) = (double)location[vd][1];
            vertex_matrix(vd.idx(), 2) = (double)location[vd][2];
        }

        // assign faces
        for (Mesh::Face_index &fd : mesh.faces())
        {
            int i = 0;
            for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh))
            {
                face_matrix(fd.idx(), i) = (int)vd;
                i++;
            }
        }

        // output
        std::tuple<RowMatrixXd, RowMatrixXi> result = std::make_tuple(vertex_matrix, face_matrix);
        return result;
    }

    ResultMesh result_from_mesh(const Mesh &mesh)
    {

        // create a result mesh
        ResultMesh result_mesh;
        result_mesh.vertices = RowMatrixXd(mesh.number_of_vertices(), 3);
        result_mesh.faces = RowMatrixXi(mesh.number_of_faces(), 3);

        //  assign vertices
        Mesh::Property_map<Mesh::Vertex_index, IK::Point_3> location = mesh.points();

        for (Mesh::Vertex_index vd : mesh.vertices())
        {
            result_mesh.vertices(vd.idx(), 0) = (double)location[vd][0];
            result_mesh.vertices(vd.idx(), 1) = (double)location[vd][1];
            result_mesh.vertices(vd.idx(), 2) = (double)location[vd][2];
        }

        // assign faces
        for (Mesh::Face_index fd : mesh.faces())
        {
            int i = 0;
            for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh))
            {
                result_mesh.faces(fd.idx(), i) = (int)vd;
                i++;
            }
        }

        // output
        return result_mesh;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Polyline to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<RowMatrixXd> result_from_polylines(const std::vector<CGAL_Polyline> &polylines)
    {
        std::vector<RowMatrixXd> pointsets;

        for (auto &polyline : polylines)
        {
            // create a matrix of points of size 3
            RowMatrixXd points(polyline.size(), 3);

            // assign coordinates
            for (size_t j = 0; j < polyline.size(); j++)
            {
                points(j, 0) = (double)polyline[j].x();
                points(j, 1) = (double)polyline[j].y();
                points(j, 2) = (double)polyline[j].z();
            }

            pointsets.push_back(points);
        }

        return pointsets;
    }

    void nested_vector_of_polylines_to_coord(std::vector<std::vector<CGAL_Polyline>> &nested_vector_of_polylines, std::vector<std::vector<std::vector<double>>> &nested_nested_vector_of_coords)
    {

        nested_nested_vector_of_coords.reserve(nested_vector_of_polylines.size());
        for (size_t i = 0; i < nested_vector_of_polylines.size(); i++)
        {
            nested_nested_vector_of_coords.emplace_back(std::vector<std::vector<double>>());
            nested_nested_vector_of_coords.back().reserve(nested_vector_of_polylines[i].size());
            for (size_t j = 0; j < nested_vector_of_polylines[i].size(); j++)
            {
                nested_nested_vector_of_coords[i].emplace_back(std::vector<double>());
                nested_nested_vector_of_coords[i].back().reserve(nested_vector_of_polylines[i][j].size());
                for (size_t k = 0; k < nested_vector_of_polylines[i][j].size(); k++)
                {
                    nested_nested_vector_of_coords[i][j].emplace_back((double)nested_vector_of_polylines[i][j][k].x());
                    nested_nested_vector_of_coords[i][j].emplace_back((double)nested_vector_of_polylines[i][j][k].y());
                    nested_nested_vector_of_coords[i][j].emplace_back((double)nested_vector_of_polylines[i][j][k].z());
                }
            }
        }
    }

    void nested_vector_of_cut_type_to_nested_vector_of_int(std::vector<std::vector<wood_cut::cut_type>> &output_types, std::vector<std::vector<int>> &output_types_int)
    {

        output_types_int.reserve(output_types.size());
        for (size_t i = 0; i < output_types.size(); i++)
        {
            output_types_int.emplace_back(std::vector<int>());
            output_types_int.back().reserve(output_types[i].size());
            for (size_t j = 0; j < output_types[i].size(); j++)
            {
                output_types_int[i].emplace_back((int)output_types[i][j]);
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // XML to Python
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    std::vector<RowMatrixXd> read_xml_as_polyline_pairs(std::string &filename)
    {
    }
}