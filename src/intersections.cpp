#include "intersections.h"
#include <CGAL/Polygon_mesh_processing/intersection.h>


namespace PMP = CGAL::Polygon_mesh_processing;


std::vector<compas::RowMatrixXd>
pmp_intersection_mesh_mesh(
    Eigen::Ref<const compas::RowMatrixXd> & VA,
    Eigen::Ref<const compas::RowMatrixXi> & FA,
    Eigen::Ref<const compas::RowMatrixXd> & VB,
    Eigen::Ref<const compas::RowMatrixXi> & FB)
{
    Mesh A = compas::mesh_from_vertices_and_faces(VA, FA);
    Mesh B = compas::mesh_from_vertices_and_faces(VB, FB);

    Polylines polylines;

    PMP::surface_intersection(A, B, std::back_inserter(polylines));

    std::vector<compas::RowMatrixXd> result = compas::result_from_polylines(polylines);

    return result;
};


void init_intersections(pybind11::module & m) {
    pybind11::module submodule = m.def_submodule("intersections");

    submodule.def(
        "intersection_mesh_mesh",
        &pmp_intersection_mesh_mesh,
        pybind11::arg("VA").noconvert(),
        pybind11::arg("FA").noconvert(),
        pybind11::arg("VB").noconvert(),
        pybind11::arg("FB").noconvert()
    );
}
