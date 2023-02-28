#include "slicer.h"
#include <CGAL/Polygon_mesh_slicer.h>


std::vector<compas::RowMatrixXd>
pmp_slice_mesh(
    Eigen::Ref<const compas::RowMatrixXd> & V,
    Eigen::Ref<const compas::RowMatrixXi> & F,
    Eigen::Ref<const compas::RowMatrixXd> & P,
    Eigen::Ref<const compas::RowMatrixXd> & N)
{
    Mesh mesh = compas::mesh_from_vertices_and_faces(V, F);

    CGAL::Polygon_mesh_slicer<Mesh, Kernel> slicer(mesh);
    Polylines polylines;
    std::back_insert_iterator<Polylines> slices(polylines);

    int number_of_planes = P.rows();

    for (int i = 0; i < number_of_planes; i++){
        Kernel::Plane_3 plane = Kernel::Plane_3(
            Kernel::Point_3(P(i, 0), P(i, 1), P(i, 2)),
            Kernel::Vector_3(N(i, 0), N(i, 1), N(i, 2)));

        slicer(plane, slices);
    }

    std::vector<compas::RowMatrixXd> result = compas::result_from_polylines(polylines);
    return result;
};


void init_slicer(pybind11::module & m) {
    pybind11::module submodule = m.def_submodule("slicer");

    submodule.def(
        "slice_mesh",
        &pmp_slice_mesh,
        pybind11::arg("V").noconvert(),
        pybind11::arg("F").noconvert(),
        pybind11::arg("P").noconvert(),
        pybind11::arg("N").noconvert()
    );
}
