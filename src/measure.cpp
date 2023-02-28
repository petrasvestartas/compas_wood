#include "measure.h"
#include <CGAL/Polygon_mesh_processing/measure.h>


namespace PMP = CGAL::Polygon_mesh_processing;


double
pmp_volume(
    Eigen::Ref<const compas::RowMatrixXd> & V,
    Eigen::Ref<const compas::RowMatrixXi> & F)
{
    Mesh mesh = compas::mesh_from_vertices_and_faces(V, F);

    double volume = PMP::volume(mesh);

    return volume;
};


void init_measure(pybind11::module & m) {
    pybind11::module submodule = m.def_submodule("measure");

    submodule.def(
        "volume",
        &pmp_volume,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert()
    );
};
