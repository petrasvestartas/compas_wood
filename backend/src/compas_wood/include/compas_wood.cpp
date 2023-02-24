#include "compas_wood.h"

void test()
{
    printf("\n________________________________________________________________________\n");
    printf("\n_______________________Hello from CPP Wood______________________________\n");
    printf("\n___If you see this message, say hi to the developer Petras Vestartas ___\n");
    printf("\n____________________petrasvestartas@gmail.com___________________________\n");
    printf("\n________________________________________________________________________\n");
}

PYBIND11_MODULE(wood_pybind11, m)
{
    m.doc() = R"pbdoc(
        Pybind11 wood plugin
        -----------------------
        .. currentmodule:: pybind11_joinery_solver
        .. autosummary::
           :toctree: _generate
           test
           get_connection_zones
           closed_mesh_from_polylines
           rtree
           joints
           intersecting_sequences_of_dD_iso_oriented_boxes
           beam_volumes
           check_joinery_library_xml
    )pbdoc";

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // This will be the main container to transfer data
    // Wood is written mainly using CGAL::IK:Point_3 type, which will be converted to Eigen Matrix
    // Then Eigen Matrix will be converted back to compas data structure
    // This means that there will be a small overhead for the data-transfer
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pybind11::class_<Result>(m, "Result").def_readonly("vertices", &Result::vertices).def_readonly("faces", &Result::faces);

    m.def("test", &test); // Function reference
}

// PYBIND11_MODULE(pybind11_joinery_solver, m)
// {
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // pybind11 module "pybind11_joinery_solver" description
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//     m.doc() = R"pbdoc(
//         Pybind11 example plugin
//         -----------------------
//         .. currentmodule:: pybind11_joinery_solver
//         .. autosummary::
//            :toctree: _generate
//            pybind11_test
//            pybind11_connectionzones

//     )pbdoc";



//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // Exposed methods
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     m.def(
//         "pybind11_test",
//         &pybind11_test); // Function reference

//     m.def(
//         "pybind11_get_connection_zones",
//         &pybind11_get_connection_zones,
//         pybind11::arg("file_path").noconvert(),
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("F").noconvert(),
//         pybind11::arg("D").noconvert(),
//         pybind11::arg("J").noconvert(),
//         pybind11::arg("X").noconvert(),
//         pybind11::arg("A").noconvert(),
//         pybind11::arg("P").noconvert(),
//         pybind11::arg("S").noconvert(),

//         pybind11::arg("search_type"),
//         pybind11::arg("output_type"),
//         pybind11::arg("triangulation"));

//     m.def(
//         "pybind11_closed_mesh_from_polylines",
//         &pybind11_closed_mesh_from_polylines,
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("F").noconvert());

//     m.def(
//         "pybind11_rtree",
//         &pybind11_rtree,
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("F").noconvert());

//     m.def(
//         "pybind11_joints",
//         &pybind11_joints,
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("F").noconvert(),
//         pybind11::arg("search_type"));

//     m.def(
//         "pybind11_intersecting_sequences_of_dD_iso_oriented_boxes",
//         &pybind11_intersecting_sequences_of_dD_iso_oriented_boxes,
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("E_R").noconvert(),
//         pybind11::arg("F").noconvert(),
//         pybind11::arg("min_distance"));

//     m.def(
//         "pybind11_beam_volumes",
//         &pybind11_beam_volumes,
//         pybind11::arg("file_path"),
//         pybind11::arg("V").noconvert(),
//         pybind11::arg("E_R").noconvert(),
//         pybind11::arg("E_N").noconvert(),
//         pybind11::arg("F").noconvert(),
//         pybind11::arg("F_T").noconvert(),
//         pybind11::arg("min_distance"),
//         pybind11::arg("volume_length"),
//         pybind11::arg("cross_or_side_to_end"),
//         pybind11::arg("flip_male"),
//         pybind11::arg("input_default_parameters_for_JOINTS_TYPES"),
//         pybind11::arg("compute_joints"),
//         pybind11::arg("division_distance"),
//         pybind11::arg("shift"),
//         pybind11::arg("output_type")

//     );

//     m.def(
//         "pybind11_check_joinery_library_xml",
//         &pybind11_check_joinery_library_xml,
//         pybind11::arg("file_path"),
//         pybind11::arg("type"),
//         pybind11::arg("division_dist"),
//         pybind11::arg("shift"));
// }
