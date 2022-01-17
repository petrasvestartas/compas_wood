#pragma once
#include <pybind11/eigen.h>
#include <pybind11/stl.h>
#include "joinery_solver_connection_zones.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Converter methods, where np.asarray are mapped to Eigner matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma region converter

struct Result {
    RowMatrixXd vertices;
    RowMatrixXi faces;
};

//CGAL_Mesh mesh_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

//std::tuple<compas::RowMatrixXd, compas::RowMatrixXi> mesh_to_vertices_and_faces(const CGAL_Mesh& mesh);

//Result result_from_mesh(const CGAL_Mesh& mesh);

RowMatrixXd result_from_polyline(CGAL_Polyline poly);

std::vector<RowMatrixXd> result_from_polylines(CGAL_Polylines polylines);

std::vector<RowMatrixXd> result_from_vector(IK::Vector_3* v);

std::vector<RowMatrixXd> result_from_bbox(std::vector<CGAL::Bbox_3> boxes);

std::vector<CGAL_Polyline> polylines_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F);

void polylines_from_vertices_and_faces_and_properties(
    const RowMatrixXd& polylines_vertices_XYZ,
    const RowMatrixXi& polylines_vertices_count_int,
    const RowMatrixXd& face_vectors_XYZ,
    const RowMatrixXi& face_joints_types_int,
    const RowMatrixXi& three_valence_element_indices_and_instruction,
    const RowMatrixXd& default_parameters_for_joint_types_matrix,

    std::vector<CGAL_Polyline>& out_polyline_pairs,
    std::vector<std::vector<IK::Vector_3>>& out_insertion_vectors,
    std::vector<std::vector<int>>& out_joint_types,
    std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
    std::vector<double>& out_default_parameters_for_joint_types);

std::vector<RowMatrixXd> result_from_polylinesVectorVector(std::vector<std::vector<CGAL_Polyline>> polylines);

std::vector<RowMatrixXd> result_from_polylinesVector(std::vector<CGAL_Polyline>& polylines, bool debug);

std::vector<RowMatrixXd> result_from_polylinesVector(std::vector<std::vector<CGAL_Polyline>>& polylines, bool debug);

std::tuple<std::vector<RowMatrixXd>, std::vector<int>> result_tuple_from_polylinesVector(std::vector<std::vector<CGAL_Polyline>>& polylines_vector, bool debug);

#pragma endregion converter

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wrapped methods with inputs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void pybind11_test();

std::tuple<std::vector<RowMatrixXd>, std::vector<int>> pybind11_get_connection_zones(
    Eigen::Ref<const RowMatrixXd>& polylines_vertices_XYZ,
    Eigen::Ref<const RowMatrixXi>& polylines_vertices_count_int,
    Eigen::Ref<const RowMatrixXd>& face_vectors_XYZ,
    Eigen::Ref<const RowMatrixXi>& face_joints_types_int,
    Eigen::Ref<const RowMatrixXi>& three_valence_element_indices_and_instruction,
    Eigen::Ref<const RowMatrixXd>& default_parameters_for_joint_types_matrix,

    int search_type,
    double division_distance,
    double shift,
    int output_type,
    int triangulate

);

std::tuple<RowMatrixXd, RowMatrixXi> pybind11_closed_mesh_from_polylines(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F);

std::tuple<std::vector<RowMatrixXi>, RowMatrixXd, RowMatrixXd> pybind11_rtree(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F); //indices, aabb, oobb

std::tuple<RowMatrixXi, std::vector<RowMatrixXd>, RowMatrixXi> pybind11_joints(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F, int& search_type);

std::tuple<RowMatrixXi, RowMatrixXd> pybind11_intersecting_sequences_of_dD_iso_oriented_boxes(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXd>& E_R, Eigen::Ref<const RowMatrixXi>& F, double& min_distance);

std::tuple<RowMatrixXi, RowMatrixXd, std::vector<RowMatrixXd>> pybind11_beam_volumes(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXd>& E_R, Eigen::Ref<const RowMatrixXd>& E_N, Eigen::Ref<const RowMatrixXi>& F, Eigen::Ref<const RowMatrixXi>& F_T, double& min_distance, double& volume_length, double& cross_or_side_to_end, int& flip_male);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wrapped package joinery_solver
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PYBIND11_MODULE(pybind11_joinery_solver, m) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //pybind11 module "pybind11_joinery_solver" description
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    m.doc() = R"pbdoc(
        Pybind11 example plugin
        -----------------------
        .. currentmodule:: pybind11_joinery_solver
        .. autosummary::
           :toctree: _generate
           pybind11_test
           pybind11_connectionzones
    )pbdoc";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Exposed structs
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pybind11::class_<Result>(m, "Result").def_readonly("vertices", &Result::vertices).def_readonly("faces", &Result::faces);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Exposed methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    m.def("pybind11_test", &pybind11_test); //Function reference

    m.def(
        "pybind11_get_connection_zones",
        &pybind11_get_connection_zones,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("D").noconvert(),
        pybind11::arg("J").noconvert(),
        pybind11::arg("X").noconvert(),
        pybind11::arg("P").noconvert(),

        pybind11::arg("search_type"),
        pybind11::arg("division_distance"),
        pybind11::arg("shift"),
        pybind11::arg("output_type"),
        pybind11::arg("triangulation"));

    m.def(
        "pybind11_closed_mesh_from_polylines",
        &pybind11_closed_mesh_from_polylines,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert());

    m.def(
        "pybind11_rtree",
        &pybind11_rtree,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert());

    m.def(
        "pybind11_joints",
        &pybind11_joints,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("search_type"));

    m.def(
        "pybind11_intersecting_sequences_of_dD_iso_oriented_boxes",
        &pybind11_intersecting_sequences_of_dD_iso_oriented_boxes,
        pybind11::arg("V").noconvert(),
        pybind11::arg("E_R").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("min_distance"));

    m.def(
        "pybind11_beam_volumes",
        &pybind11_beam_volumes,
        pybind11::arg("V").noconvert(),
        pybind11::arg("E_R").noconvert(),
        pybind11::arg("E_N").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("F_T").noconvert(),
        pybind11::arg("min_distance"),
        pybind11::arg("volume_length"),
        pybind11::arg("cross_or_side_to_end"),
        pybind11::arg("flip_male"));
}
