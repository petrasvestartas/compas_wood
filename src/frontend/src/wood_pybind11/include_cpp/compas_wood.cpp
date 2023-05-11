#include "../../../stdafx.h"              //includes
#include "python_to_cpp__cpp_to_python.h" //converters
#include "compas_wood.h"                  //headers
#include "wood_xml.h"                     //read xml file of the datasets

// data structure
#include "wood_cut.h"
#include "wood_main.h"
#include "wood_element.h"

// joinery
#include "wood_joint_lib.h"
#include "wood_joint.h"

// geometry methods
#include "cgal_mesh_boolean.h"
#include "cgal_inscribe_util.h"
#include "cgal_rectangle_util.h"

namespace compas_wood
{

    namespace internal
    {
        IK::Point_3 point_at(IK::Vector_3(&box)[5], const double& s, const  double& t, const double& c)
        {
            return IK::Point_3(
                box[0].x() + s * box[1].x() + t * box[2].x() + c * box[3].x(),
                box[0].y() + s * box[1].y() + t * box[2].y() + c * box[3].y(),
                box[0].z() + s * box[1].z() + t * box[2].z() + c * box[3].z()

            );
        }

        void get_corners(IK::Vector_3(&box)[5], CGAL_Polyline& corners)
        {
            corners = CGAL_Polyline(8);

            corners[0] = point_at(box, box[4].x(), box[4].y(), -box[4].z());
            corners[1] = point_at(box, -box[4].x(), box[4].y(), -box[4].z());
            corners[3] = point_at(box, box[4].x(), -box[4].y(), -box[4].z());
            corners[2] = point_at(box, -box[4].x(), -box[4].y(), -box[4].z());

            corners[4] = point_at(box, box[4].x(), box[4].y(), box[4].z());
            corners[5] = point_at(box, -box[4].x(), box[4].y(), box[4].z());
            corners[7] = point_at(box, box[4].x(), -box[4].y(), box[4].z());
            corners[6] = point_at(box, -box[4].x(), -box[4].y(), box[4].z());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Test methods - mainly to check if the library is loading and whether the types can be passed between C++ and Python
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

    void test_opaque_types_wood_vector_int(std::vector<int>& vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = vec[i] * vec[i];
        }
    }

    void test_opaque_types_wood_nested_vector_int(std::vector<std::vector<int>>& vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            for (int j = 0; j < vec[i].size(); j++)
            {
                vec[i][j] = vec[i][j] * vec[i][j];
            }
        }
    }

    void test_opaque_types_wood_vector_double(std::vector<double>& vec)
    {
        for (int i = 0; i < vec.size(); i++)
        {
            vec[i] = vec[i] * vec[i];
        }
    }

    void test_opaque_types_wood_nested_vector_double(std::vector<std::vector<double>>& vec)
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

    void read_xml_polylines(std::string& foldername, std::string& filename_of_dataset, std::vector<std::vector<double>>& polylines_coordinates)
    {
        // set file paths
        wood_globals::DATA_SET_INPUT_FOLDER = foldername; // = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\frontend\\src\\wood\\dataset\\";
        wood_xml::path_and_file_for_input_polylines = wood_globals::DATA_SET_INPUT_FOLDER + filename_of_dataset + ".xml";

        // print the user given values
        printf("User given values \n");
        printf(foldername.c_str());
        printf("\n");
        printf(filename_of_dataset.c_str());
        printf("\n");
        printf(wood_xml::path_and_file_for_input_polylines.c_str());
        printf("\n");

        // read the xml file
        wood_xml::read_xml_polylines(polylines_coordinates, false, true);
    }

    void read_xml_polylines_and_properties(
        std::string& foldername,
        std::string& filename_of_dataset,
        std::vector<std::vector<double>>& input_polyline_pairs_coord,
        std::vector<std::vector<double>>& input_insertion_vectors_coord,
        std::vector<std::vector<int>>& input_JOINTS_TYPES,
        std::vector<std::vector<int>>& input_three_valence_element_indices_and_instruction,
        std::vector<int>& input_adjacency)
    {

        // set file paths
        wood_globals::DATA_SET_INPUT_FOLDER = foldername; // = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\frontend\\src\\wood\\dataset\\";
        wood_xml::path_and_file_for_input_polylines = wood_globals::DATA_SET_INPUT_FOLDER + filename_of_dataset + ".xml";

        // print the user given values
        printf("User given values \n");
        printf(foldername.c_str());
        printf("\n");
        printf(filename_of_dataset.c_str());
        printf("\n");
        printf(wood_xml::path_and_file_for_input_polylines.c_str());
        // read the xml file
        wood_xml::read_xml_polylines_and_properties(input_polyline_pairs_coord, input_insertion_vectors_coord, input_JOINTS_TYPES, input_three_valence_element_indices_and_instruction, input_adjacency, false, true);

        printf("\n input_polyline_pairs_coord ");
        printf(std::to_string(input_polyline_pairs_coord.size()).c_str());
        printf("\n input_insertion_vectors_coord ");
        printf(std::to_string(input_insertion_vectors_coord.size()).c_str());
        printf("\n input_JOINTS_TYPES ");
        printf(std::to_string(input_JOINTS_TYPES.size()).c_str());
        printf("\n input_three_valence_element_indices_and_instruction ");
        printf(std::to_string(input_three_valence_element_indices_and_instruction.size()).c_str());
        printf("\n input_adjacency ");
        printf(std::to_string(input_adjacency.size()).c_str());
        printf("\n");

    }

    void get_connection_zones(
        // input
        std::vector<std::vector<double>>& pybind11_input_polyline_pairs,
        std::vector<std::vector<double>>& pybind11_input_insertion_vectors,
        std::vector<std::vector<int>>& pybind11_input_JOINTS_TYPES,
        std::vector<std::vector<int>>& pybind11_input_three_valence_element_indices_and_instruction,
        std::vector<int>& pybind11_input_adjacency,
        std::vector<double>& pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES,
        int& _search_type,
        std::vector<double>& _scale,
        int& _output_type,
        // output
        std::vector<std::vector<std::vector<double>>>& pybind11_output_plines,
        std::vector<std::vector<int>>& pybind11_output_types,
        // global_parameters
        std::vector<double>& pybind11_joint_volume_parameters
    )
    {
        // internal::set_file_path_for_input_xml_and_screenshot(
        //     input_polyline_pairs,
        //     input_insertion_vectors,
        //     input_JOINTS_TYPES,
        //     input_three_valence_element_indices_and_instruction,
        //     input_adjacency,
        //     "type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small",
        //     false);

        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // The filename of the xml file and the screenshot directory
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
        python_to_cpp__cpp_to_python::coord_to_vector_of_polylines(pybind11_input_polyline_pairs, input_polyline_pairs);

        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
        python_to_cpp__cpp_to_python::coord_to_vector_of_vectors(pybind11_input_insertion_vectors, input_insertion_vectors);


        std::vector<std::vector<int>> input_JOINTS_TYPES = pybind11_input_JOINTS_TYPES = pybind11_input_JOINTS_TYPES;
        std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = pybind11_input_three_valence_element_indices_and_instruction;
        std::vector<int> input_adjacency = pybind11_input_adjacency;

        int search_type = _search_type;
        std::vector<double> scale = _scale;
        int output_type = _output_type;

        wood_globals::JOINTS_PARAMETERS_AND_TYPES = pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES;
        wood_globals::OUTPUT_GEOMETRY_TYPE = output_type;

        if( pybind11_joint_volume_parameters.size()>2)
            wood_globals::JOINT_VOLUME_EXTENSION  = pybind11_joint_volume_parameters; 



        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main Method of Wood
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<std::vector<CGAL_Polyline>> output_plines;
        std::vector<std::vector<wood_cut::cut_type>> output_types;
        std::vector<std::vector<int>> top_face_triangulation;


        wood_main::get_connection_zones(
            // input
            input_polyline_pairs,
            input_insertion_vectors,
            input_JOINTS_TYPES,
            input_three_valence_element_indices_and_instruction,
            input_adjacency,
            // output
            output_plines,
            output_types,
            top_face_triangulation,
            // Global Parameters
            wood_globals::JOINTS_PARAMETERS_AND_TYPES,
            scale,
            search_type,
            wood_globals::OUTPUT_GEOMETRY_TYPE,
            0);

        //printf("CPP 6");
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Convert polylines to double vector and cut types to int vector
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        python_to_cpp__cpp_to_python::nested_vector_of_polylines_to_coord(output_plines, pybind11_output_plines);
        python_to_cpp__cpp_to_python::nested_vector_of_cut_type_to_nested_vector_of_int(output_types, pybind11_output_types); // only filled when 3rd output type is uesd
        //printf("CPP 7");
    }

    void closed_mesh_from_polylines_vnf(
        std::vector<std::vector<double>>& polylines_coordinates,
        std::vector<float>& out_vertices,
        std::vector<float>& out_normals,
        std::vector<int>& out_triangles)
    {

        std::vector<std::vector<IK::Point_3>> vector_of_polyline;
        python_to_cpp__cpp_to_python::coord_to_vector_of_polylines(polylines_coordinates, vector_of_polyline);
        cgal_polyline_mesh_util::closed_mesh_from_polylines_vnf(vector_of_polyline, out_vertices, out_normals, out_triangles, 1);
    }

    void rtree(
        std::vector<std::vector<double>>& polylines_coordinates,
        std::vector<std::vector<int>>& elements_neighbours,
        std::vector<std::vector<double>>& elements_AABB,
        std::vector<std::vector<double>>& elements_OOBB)
    {

        //////////////////////////////////////////////////////////////////////////////
        // Convert raw-data to list of Polylines
        //////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> input_polyline_pairs;
        python_to_cpp__cpp_to_python::coord_to_vector_of_polylines(polylines_coordinates, input_polyline_pairs);

        //////////////////////////////////////////////////////////////////////////////
        // Create elements, AABB, OBB, P, Pls, thickness
        //////////////////////////////////////////////////////////////////////////////
        std::vector<wood::element> e;
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
        std::vector<std::vector<int>> input_joint_types;
        wood_main::get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, e);

        //////////////////////////////////////////////////////////////////////////////
        // Create joints, Perform Joint Area Search
        //////////////////////////////////////////////////////////////////////////////

        //////////////////////////////////////////////////////////////////////////////
        // Create RTree
        //////////////////////////////////////////////////////////////////////////////

        RTree<int, double, 3> tree;

        //////////////////////////////////////////////////////////////////////////////
        // Insert AABB
        //////////////////////////////////////////////////////////////////////////////

        for (size_t i = 0; i < e.size(); i++)
        {
            double min[3] = { e[i].aabb.xmin(), e[i].aabb.ymin(), e[i].aabb.zmin() };
            double max[3] = { e[i].aabb.xmax(), e[i].aabb.ymax(), e[i].aabb.zmax() };
            tree.Insert(min, max, i);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
        //////////////////////////////////////////////////////////////////////////////
        elements_neighbours.reserve(e.size());

        for (size_t i = 0; i < e.size(); i++)
        {

            std::vector<int> element_neigbhours;

            auto callback = [&element_neigbhours, i, &e](int foundValue) -> bool
            {
                if (cgal_box_util::get_collision(e[i].oob, e[foundValue].oob))
                {
                    element_neigbhours.push_back(foundValue);
                }
                return true;
            };

            double min[3] = { e[i].aabb.xmin(), e[i].aabb.ymin(), e[i].aabb.zmin() };
            double max[3] = { e[i].aabb.xmax(), e[i].aabb.ymax(), e[i].aabb.zmax() };
            int nhits = tree.Search(min, max, callback); // callback in this method call callback above

            // Add elements to the vector
            elements_neighbours.emplace_back(std::vector<int>());

            for (int j = 0; j < element_neigbhours.size(); j++)
                elements_neighbours.back().emplace_back(element_neigbhours[j]);
        }

        //////////////////////////////////////////////////////////////////////////////
        // Output AABB
        //////////////////////////////////////////////////////////////////////////////
        elements_AABB.reserve(e.size());

        for (size_t i = 0; i < e.size(); i++)
        {

            elements_AABB.emplace_back(std::vector<double>());
            elements_AABB.back().reserve(24);

            elements_AABB.back().emplace_back(e[i].aabb.xmin());
            elements_AABB.back().emplace_back(e[i].aabb.ymin());
            elements_AABB.back().emplace_back(e[i].aabb.zmin());

            elements_AABB.back().emplace_back(e[i].aabb.xmin());
            elements_AABB.back().emplace_back(e[i].aabb.ymin());
            elements_AABB.back().emplace_back(e[i].aabb.zmax());

            elements_AABB.back().emplace_back(e[i].aabb.xmin());
            elements_AABB.back().emplace_back(e[i].aabb.ymax());
            elements_AABB.back().emplace_back(e[i].aabb.zmax());

            elements_AABB.back().emplace_back(e[i].aabb.xmin());
            elements_AABB.back().emplace_back(e[i].aabb.ymax());
            elements_AABB.back().emplace_back(e[i].aabb.zmin());

            elements_AABB.back().emplace_back(e[i].aabb.xmax());
            elements_AABB.back().emplace_back(e[i].aabb.ymin());
            elements_AABB.back().emplace_back(e[i].aabb.zmin());

            elements_AABB.back().emplace_back(e[i].aabb.xmax());
            elements_AABB.back().emplace_back(e[i].aabb.ymin());
            elements_AABB.back().emplace_back(e[i].aabb.zmax());

            elements_AABB.back().emplace_back(e[i].aabb.xmax());
            elements_AABB.back().emplace_back(e[i].aabb.ymax());
            elements_AABB.back().emplace_back(e[i].aabb.zmax());

            elements_AABB.back().emplace_back(e[i].aabb.xmax());
            elements_AABB.back().emplace_back(e[i].aabb.ymax());
            elements_AABB.back().emplace_back(e[i].aabb.zmin());
        }

        //////////////////////////////////////////////////////////////////////////////
        // Output OOBB
        //////////////////////////////////////////////////////////////////////////////

        elements_OOBB.reserve(e.size());

        for (size_t i = 0; i < e.size(); i++)
        {
            elements_OOBB.emplace_back(std::vector<double>());
            elements_OOBB.back().reserve(24);

            CGAL_Polyline corners;
            internal::get_corners(e[i].oob, corners);

            for (int j = 0; j < 8; j++)
            {
                elements_OOBB.back().emplace_back(corners[j].hx());
                elements_OOBB.back().emplace_back(corners[j].hy());
                elements_OOBB.back().emplace_back(corners[j].hz());
            }
        }
    }

    void joints(
        std::vector<std::vector<double>>& polylines_coordinates,
        int& search_type,
        std::vector<std::vector<int>>& element_pairs,
        std::vector<std::vector<double>>& joint_areas,
        std::vector<int>& joint_types)
    {
        //////////////////////////////////////////////////////////////////////////////
        // Convert raw-data to list of Polylines
        //////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline> input_polyline_pairs;
        python_to_cpp__cpp_to_python::coord_to_vector_of_polylines(polylines_coordinates, input_polyline_pairs);

        //////////////////////////////////////////////////////////////////////////////
        // Create elements, AABB, OBB, P, Pls, thickness
        //////////////////////////////////////////////////////////////////////////////
        const int n = input_polyline_pairs.size() * 0.5;
        std::vector<wood::element> elements;
        std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
        std::vector<std::vector<int>> input_joint_types;
        wood_main::get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);

        //////////////////////////////////////////////////////////////////////////////
        // Create joints, Perform Joint Area Search
        //////////////////////////////////////////////////////////////////////////////
        auto joints = std::vector<wood::joint>();
        auto joints_map = std::unordered_map<uint64_t, int>();
        std::vector<int> neighbors;
        wood_main::adjacency_search(elements, search_type, neighbors, joints, joints_map);

        //////////////////////////////////////////////////////////////////////////////
        // Get element pairs, joint areas, joint types
        //////////////////////////////////////////////////////////////////////////////
        element_pairs.reserve(joints.size() * 3);
        joint_areas.reserve(joints.size());
        joint_types.reserve(joints.size());

        for (size_t i = 0; i < joints.size(); i++)
        {
            // element pairs and faces ids
            element_pairs.emplace_back(std::vector<int>{joints[i].v0, joints[i].v1, joints[i].f0_0, joints[i].f1_0, joints[i].f0_1, joints[i].f1_1});

            // joint areas
            joint_areas.emplace_back(std::vector<double>());
            joint_areas.back().reserve(joints[i].joint_area.size() * 3);

            for (size_t j = 0; j < joints[i].joint_area.size(); j++)
            {
                joint_areas.back().emplace_back(joints[i].joint_area[j].hx());
                joint_areas.back().emplace_back(joints[i].joint_area[j].hy());
                joint_areas.back().emplace_back(joints[i].joint_area[j].hz());
            }

            // joint types
            joint_types.emplace_back(joints[i].type);
        }
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
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<std::vector<int>>>);
PYBIND11_MAKE_OPAQUE(std::vector<double>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<double>>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<std::vector<double>>>);
PYBIND11_MAKE_OPAQUE(std::vector<float>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<float>>);
PYBIND11_MAKE_OPAQUE(std::vector<std::vector<std::vector<float>>>);
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
           read_xml_polylines
           read_xml_polylines_and_properties
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

    // CPP collections visible in Python | these binding must come together with the PYBIND11_MAKE_OPAQUE equivalent at the top
    py::bind_vector<std::vector<int>>(m, "WoodVectorInt");
    py::bind_vector<std::vector<std::vector<int>>>(m, "WoodNestedVectorInt");
    py::bind_vector<std::vector<std::vector<std::vector<int>>>>(m, "WoodNestedNestedVectorInt");
    py::bind_vector<std::vector<double>>(m, "WoodVectorDouble");
    py::bind_vector<std::vector<std::vector<double>>>(m, "WoodNestedVectorDouble");
    py::bind_vector<std::vector<std::vector<std::vector<double>>>>(m, "WoodNestedNestedVectorDouble");
    py::bind_vector<std::vector<float>>(m, "WoodVectorFloat");
    py::bind_vector<std::vector<std::vector<float>>>(m, "WoodNestedVectorFloat");
    py::bind_vector<std::vector<std::vector<std::vector<float>>>>(m, "WoodNestedNestedVectorFloat");

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
    m.def("read_xml_polylines", &compas_wood::read_xml_polylines,
        // inputs
        pybind11::arg("foldername").noconvert(),
        pybind11::arg("filename_of_dataset").noconvert(),
        // outputs
        pybind11::arg("polylines_coordinates").noconvert());

    m.def("read_xml_polylines_and_properties", &compas_wood::read_xml_polylines_and_properties,
        // inputs
        pybind11::arg("foldername").noconvert(),
        pybind11::arg("filename_of_dataset").noconvert(),
        // outputs
        pybind11::arg("input_polyline_pairs_coord").noconvert(),
        pybind11::arg("input_insertion_vectors_coord").noconvert(),
        pybind11::arg("input_JOINTS_TYPES").noconvert(),
        pybind11::arg("input_three_valence_element_indices_and_instruction").noconvert(),
        pybind11::arg("input_adjacency").noconvert());

    m.def("get_connection_zones", &compas_wood::get_connection_zones,
        // inputs
        pybind11::arg("pybind11_input_polyline_pairs").noconvert(),
        pybind11::arg("pybind11_input_insertion_vectors").noconvert(),
        pybind11::arg("pybind11_input_JOINTS_TYPES").noconvert(),
        pybind11::arg("pybind11_input_three_valence_element_indices_and_instruction").noconvert(),
        pybind11::arg("pybind11_input_adjacency").noconvert(),
        pybind11::arg("pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES").noconvert(),
        pybind11::arg("search_type"),
        pybind11::arg("scale").noconvert(),
        pybind11::arg("output_type").noconvert(),
        // outputs
        pybind11::arg("pybind11_output_plines").noconvert(),
        pybind11::arg("pybind11_output_types").noconvert(),
        pybind11::arg("pybind11_joint_volume_parameters").noconvert());

    m.def("closed_mesh_from_polylines_vnf", &compas_wood::closed_mesh_from_polylines_vnf,
        // inputs
        pybind11::arg("polylines_coordinates").noconvert(),
        // outputs
        pybind11::arg("out_vertices").noconvert(),
        pybind11::arg("out_normals").noconvert(),
        pybind11::arg("out_triangles").noconvert());

    m.def("rtree", &compas_wood::rtree,
        // inputs
        pybind11::arg("polylines_coordinates").noconvert(),
        // outputs
        pybind11::arg("elements_neighbours").noconvert(),
        pybind11::arg("elements_AABB").noconvert(),
        pybind11::arg("elements_OOBB").noconvert());

    m.def("joints", &compas_wood::joints,
        // inputs
        pybind11::arg("polylines_coordinates").noconvert(),
        pybind11::arg("search_type"),
        // outputs
        pybind11::arg("element_pairs").noconvert(),
        pybind11::arg("joint_areas").noconvert(),
        pybind11::arg("joint_types").noconvert());

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
