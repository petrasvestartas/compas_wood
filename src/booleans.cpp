#include "booleans.h"
#include <CGAL/Polygon_mesh_processing/corefinement.h>
#include <CGAL/Polygon_mesh_processing/clip.h>


namespace PMP = CGAL::Polygon_mesh_processing;


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_boolean_union(
    Eigen::Ref<const compas::RowMatrixXd> &VA,
    Eigen::Ref<const compas::RowMatrixXi> &FA,
    Eigen::Ref<const compas::RowMatrixXd> &VB,
    Eigen::Ref<const compas::RowMatrixXi> &FB)
{
    Mesh A = compas::mesh_from_vertices_and_faces(VA, FA);
    Mesh B = compas::mesh_from_vertices_and_faces(VB, FB);
    Mesh C;

    PMP::corefine_and_compute_union(A, B, C);

    // Result

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> R = compas::mesh_to_vertices_and_faces(C);

    return R;
};


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_boolean_difference(
    Eigen::Ref<const compas::RowMatrixXd> &VA,
    Eigen::Ref<const compas::RowMatrixXi> &FA,
    Eigen::Ref<const compas::RowMatrixXd> &VB,
    Eigen::Ref<const compas::RowMatrixXi> &FB)
{
    Mesh A = compas::mesh_from_vertices_and_faces(VA, FA);
    Mesh B = compas::mesh_from_vertices_and_faces(VB, FB);
    Mesh C;

    PMP::corefine_and_compute_difference(A, B, C);

    // Result

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> R = compas::mesh_to_vertices_and_faces(C);

    return R;
};


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_boolean_intersection(
    Eigen::Ref<const compas::RowMatrixXd> &VA,
    Eigen::Ref<const compas::RowMatrixXi> &FA,
    Eigen::Ref<const compas::RowMatrixXd> &VB,
    Eigen::Ref<const compas::RowMatrixXi> &FB)
{
    Mesh A = compas::mesh_from_vertices_and_faces(VA, FA);
    Mesh B = compas::mesh_from_vertices_and_faces(VB, FB);
    Mesh C;

    PMP::corefine_and_compute_intersection(A, B, C);

    // Result

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> R = compas::mesh_to_vertices_and_faces(C);

    return R;
};


std::tuple<compas::RowMatrixXd, compas::RowMatrixXi>
pmp_split(
    Eigen::Ref<const compas::RowMatrixXd> &VA,
    Eigen::Ref<const compas::RowMatrixXi> &FA,
    Eigen::Ref<const compas::RowMatrixXd> &VB,
    Eigen::Ref<const compas::RowMatrixXi> &FB)
{
    Mesh A = compas::mesh_from_vertices_and_faces(VA, FA);
    Mesh B = compas::mesh_from_vertices_and_faces(VB, FB);
    // Mesh C;

    // PMP::clip(A, B, CGAL::parameters::clip_volume(false), CGAL::parameters::clip_volume(false));
    PMP::split(A, B);

    // Result

    std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> R = compas::mesh_to_vertices_and_faces(A);

    return R;
};


void init_booleans(pybind11::module &m)
{
    pybind11::module submodule = m.def_submodule("booleans");

    submodule.def(
        "boolean_union",
        &pmp_boolean_union,
        pybind11::arg("VA").noconvert(),
        pybind11::arg("FA").noconvert(),
        pybind11::arg("VB").noconvert(),
        pybind11::arg("FB").noconvert());

    submodule.def(
        "boolean_difference",
        &pmp_boolean_difference,
        pybind11::arg("VA").noconvert(),
        pybind11::arg("FA").noconvert(),
        pybind11::arg("VB").noconvert(),
        pybind11::arg("FB").noconvert());

    submodule.def(
        "boolean_intersection",
        &pmp_boolean_intersection,
        pybind11::arg("VA").noconvert(),
        pybind11::arg("FA").noconvert(),
        pybind11::arg("VB").noconvert(),
        pybind11::arg("FB").noconvert());
    
    submodule.def(
        "split",
        &pmp_split,
        pybind11::arg("VA").noconvert(),
        pybind11::arg("FA").noconvert(),
        pybind11::arg("VB").noconvert(),
        pybind11::arg("FB").noconvert());
};
