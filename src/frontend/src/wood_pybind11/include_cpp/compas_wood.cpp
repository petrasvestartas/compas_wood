#include "../../../stdafx.h"              //includes
#include "python_to_cpp__cpp_to_python.h" //converters
#include "compas_wood.h"                  //headers
#include "wood_xml.h"                     //read xml file of the datasets
namespace compas_wood
{

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Test methods - mainly to chekc if the library is loading and whether the types can be passed between C++ and Python
    // https://github.com/tdegeus/pybind11_examples
    // https://github.com/pybind/pybind11/discussions/4340
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void test()
    {
        printf("\n________________________________________________________________________\n");
        printf("\n_______________________Hello from CPP Wood______________________________\n");
        printf("\n___If you see this message, say hi to the developer Petras Vestartas ___\n");
        printf("\n____________________petrasvestartas@gmail.com___________________________\n");
        printf("\n________________________________________________________________________\n");
    }

    void test_opaque_types_wood_vector_int(std::vector<int> &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = vec[i] * vec[i];
        }
    }

    void test_opaque_types_wood_nested_vector_int(std::vector<std::vector<int>> &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < vec[i].size(); j++)
            {
                vec[i][j] = vec[i][j] * vec[i][j];
            }
        }
    }

    void test_opaque_types_wood_vector_double(std::vector<double> &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = vec[i] * vec[i];
        }
    }

    void test_opaque_types_wood_nested_vector_double(std::vector<std::vector<double>> &vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < vec[i].size(); j++)
            {
                vec[i][j] = vec[i][j] * vec[i][j];
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Primary methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void get_data_set(std::string &foldername, std::string &filename_of_dataset, std::vector<std::vector<double>> &polylines_coordinates)
    {
        // set file paths
        wood_globals::DATA_SET_INPUT_FOLDER = foldername; // = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\frontend\\src\\wood\\dataset\\";
        wood_xml::path_and_file_for_input_polylines = wood_globals::DATA_SET_INPUT_FOLDER + filename_of_dataset + ".xml";

        // print the user given values
        printf("\n___________________________________start_of_CPP_____________________________________\n");
        printf("User given values \n");
        printf(foldername.c_str());
        printf("\n");
        printf(filename_of_dataset.c_str());
        printf("\n");
        printf(wood_xml::path_and_file_for_input_polylines.c_str());
        printf("\n____________________________________end_of_CPP______________________________________\n");

        // read the xml file
        wood_xml::read_xml_polylines(polylines_coordinates, false, true);
        printf(std::to_string(polylines_coordinates.size()).c_str());
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Secondary Methods
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pybind11 Opaque types, be sure that the these types are equavivalent to the ones in the "PYBIND11_MODULE(wood_pybind11, m)""
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PYBIND11_MAKE_OPAQUE(std::vector<int>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<int>>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<double>>);
PYBIND11_MAKE_OPAQUE(std::map<std::string, double>);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pybind11 module and the wrapped functions
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

    // CPP collections visible in Python
    py::bind_vector<std::vector<int>>(m, "WoodVectorInt");
    py::bind_vector<std::vector<std::vector<int>>>(m, "WoodNestedVectorInt");
    py::bind_vector<std::vector<double>>(m, "WoodVectorDouble");
    py::bind_vector<std::vector<std::vector<double>>>(m, "WoodNestedVectorDouble");

    py::bind_map<std::map<std::string, double>>(m, "WoodMapStringDouble");

    // mesh structure
    pybind11::class_<python_to_cpp__cpp_to_python::ResultMesh>(m, "ResultMesh")
        .def_readonly("vertices", &python_to_cpp__cpp_to_python::ResultMesh::vertices)
        .def_readonly("faces", &python_to_cpp__cpp_to_python::ResultMesh::faces);

    // test methods
    m.def("test", &compas_wood::test); // Function reference
    m.def("test_opaque_types_wood_vector_int", &compas_wood::test_opaque_types_wood_vector_int);
    m.def("test_opaque_types_wood_nested_vector_int", &compas_wood::test_opaque_types_wood_nested_vector_int);
    m.def("test_opaque_types_wood_vector_double", &compas_wood::test_opaque_types_wood_vector_double);
    m.def("test_opaque_types_wood_nested_vector_double", &compas_wood::test_opaque_types_wood_nested_vector_double);

    // primary methods
    m.def("get_data_set", &compas_wood::get_data_set,
          // inputs
          pybind11::arg("foldername").noconvert(),
          pybind11::arg("filename_of_dataset").noconvert(),
          // outputs
          pybind11::arg("polylines_coordinates").noconvert());

    // secondary methods
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
