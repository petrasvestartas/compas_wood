#ifndef CGAL_MESH_BOOLEAN_H
#define CGAL_MESH_BOOLEAN_H

namespace cgal_mesh_boolean
{
    void mesh_boolean_create_array_track_colors(

        // double *coord_mesh, int *n_coord_meshArray, // Flat array of coordinates / 0 256 512 flat array of vertices array / number of meshes
        // int *faces_mesh, int *n_faces_meshArray,
        // size_t n_mesh,

        std::vector<Mesh> mesh_list,
        size_t difference_union_intersection,

        std::vector<float> &coord_out, int &n_coord_out,
        std::vector<float> &normals_out,
        std::vector<int> &faces_out, int &n_faces_out,
        std::vector<int> &facesColors_out, int &n_facesColors_out,
        int &n_valid_meshes

    );

    void mesh_boolean_test();

}

#endif // CGAL_MESH_BOOLEAN_H