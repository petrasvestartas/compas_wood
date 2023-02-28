#include "subdivision.h"
#include <CGAL/subdivision_method_3.h>


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
subd_catmullclark(
    compas::RowMatrixXd & V,
    std::vector< std::vector<int> > & faces,
    unsigned int k)
{
    Mesh mesh = compas::ngon_from_vertices_and_faces(V, faces);

    CGAL::Subdivision_method_3::CatmullClark_subdivision(mesh, CGAL::parameters::number_of_iterations(k));

    mesh.collect_garbage();

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> R = compas::quadmesh_to_vertices_and_faces(mesh);
    return R;
};


void init_subdivision(pybind11::module & m) {
    pybind11::module submodule = m.def_submodule("subdivision");

    submodule.def(
        "subd_catmullclark",
        &subd_catmullclark,
        pybind11::arg("V").noconvert(),
        pybind11::arg("faces").noconvert(),
        pybind11::arg("k")
    );
};
