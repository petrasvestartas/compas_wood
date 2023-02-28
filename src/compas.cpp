#include <compas.h>
#include <pybind11/pybind11.h>


Mesh
compas::mesh_from_vertices_and_faces(
    const compas::RowMatrixXd & V,
    const compas::RowMatrixXi & F)
{
    int v = V.rows();
    int f = F.rows();

    Mesh mesh;
    std::vector<Mesh::Vertex_index> index_descriptor(v);

    for (int i=0; i < v; i++) {
        index_descriptor[i] = mesh.add_vertex(Kernel::Point_3(V(i, 0), V(i, 1), V(i, 2)));
    }

    Mesh::Vertex_index a;
    Mesh::Vertex_index b;
    Mesh::Vertex_index c;

    for (int i=0; i < f; i++) {
        a = index_descriptor[F(i, 0)];
        b = index_descriptor[F(i, 1)];
        c = index_descriptor[F(i, 2)];
        mesh.add_face(a, b, c);
    }

    return mesh;
}


Mesh
compas::ngon_from_vertices_and_faces(
    const compas::RowMatrixXd & V,
    const std::vector< std::vector<int> > & faces)
{
    int v = V.rows();

    Mesh mesh;
    std::vector<Mesh::Vertex_index> index_descriptor(v);

    for (int i=0; i < v; i++) {
        index_descriptor[i] = mesh.add_vertex(Kernel::Point_3(V(i, 0), V(i, 1), V(i, 2)));
    }

    for (int i=0; i < faces.size(); i++) {
        std::vector<Mesh::Vertex_index> face;

        for (int j=0; j < faces[i].size(); j++) {
            face.push_back(index_descriptor[faces[i][j]]);
        }

        mesh.add_face(face);
    }

    return mesh;
}


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
compas::mesh_to_vertices_and_faces(
    const Mesh & mesh)
{
    int v = mesh.number_of_vertices();
    int f = mesh.number_of_faces();

    compas::RowMatrixXd V(v, 3);
    compas::RowMatrixXi F(f, 3);

    Mesh::Property_map<Mesh::Vertex_index, Kernel::Point_3> location = mesh.points();

    for (Mesh::Vertex_index vd : mesh.vertices()) {
        V(vd, 0) = (double) location[vd][0];
        V(vd, 1) = (double) location[vd][1];
        V(vd, 2) = (double) location[vd][2];
    }

    for (Mesh::Face_index fd : mesh.faces()) {
        int i = 0;
        for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh)) {
            F(fd, i) = (int) vd;
            i++;
        }
    }

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> result = std::make_tuple(V, F);
    return result;
}


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
compas::quadmesh_to_vertices_and_faces(
    const Mesh & mesh)
{
    int v = mesh.number_of_vertices();
    int f = mesh.number_of_faces();

    compas::RowMatrixXd V(v, 3);
    compas::RowMatrixXi F(f, 4);

    Mesh::Property_map<Mesh::Vertex_index, Kernel::Point_3> location = mesh.points();

    for (Mesh::Vertex_index vd : mesh.vertices()) {
        V(vd, 0) = (double) location[vd][0];
        V(vd, 1) = (double) location[vd][1];
        V(vd, 2) = (double) location[vd][2];
    }

    for (Mesh::Face_index fd : mesh.faces()) {
        int i = 0;
        for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh)) {
            F(fd, i) = (int) vd;
            i++;
        }
    }

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> result = std::make_tuple(V, F);
    return result;
}


// TODO: rename to ResultMesh
// construct a result mesh
// from a CGAL surface mesh
compas::Result
compas::result_from_mesh(const Mesh & mesh)
{
    int v = mesh.number_of_vertices();
    int f = mesh.number_of_faces();

    compas::Result R;
    compas::RowMatrixXd V(v, 3);
    compas::RowMatrixXi F(f, 3);

    Mesh::Property_map<Mesh::Vertex_index, Kernel::Point_3> location = mesh.points();

    for (Mesh::Vertex_index vd : mesh.vertices()) {
        V(vd, 0) = (double) location[vd][0];
        V(vd, 1) = (double) location[vd][1];
        V(vd, 2) = (double) location[vd][2];
    }

    for (Mesh::Face_index fd : mesh.faces()) {
        int i = 0;
        for (Mesh::Vertex_index vd : vertices_around_face(mesh.halfedge(fd), mesh)) {
            F(fd, i) = (int) vd;
            i++;
        }
    }

    R.vertices = V;
    R.faces = F;

    return R;
}


// construct a set of result polylines
// from CGAL polylines
// the CGAL polylines are a list of vectors of points
std::vector<compas::RowMatrixXd>
compas::result_from_polylines(Polylines polylines)
{
    std::vector<compas::RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++){
        const Polyline& poly = *i;
        int n = poly.size();
        compas::RowMatrixXd points(n, 3);

        for(int j = 0; j < n; j++){
            points(j, 0) = (double) poly[j].x();
            points(j, 1) = (double) poly[j].y();
            points(j, 2) = (double) poly[j].z();
        }

        pointsets.push_back(points);
    }

    return pointsets;
}
