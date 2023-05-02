#include "pinvoke.h"
#include "../../../stdafx.h"              //includes
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic Method and Examples for PInvoke
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PINVOKE int test_get_square(int n) {
    return n * n;
}
PINVOKE int test_get_square2(int n) {
    return n * n * n;
}

PINVOKE int process_and_return_sum(int* array, int size) {
    int sum = 0;
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    return sum;
}

PINVOKE void release_int(int*& arr, bool isArray) {
    deletePtr(arr, isArray);
    //arr = NULL;
}

PINVOKE void release_double(double*& arr, bool isArray) {
    deletePtr(arr, isArray);
    //arr = NULL;
}

PINVOKE void release_float(float*& arr, bool isArray) {
    deletePtr(arr, isArray);
    //arr = NULL;
}

PINVOKE void release_size_t(size_t*& arr, bool isArray) {
    deletePtr(arr, isArray);
    //arr = NULL;
}

PINVOKE void test_set_array(double*& coord, int coord_size) {
    for (int i = 0; i < coord_size; i++)
        printf("%f \n", coord[i]);
}

PINVOKE void test_get_array(double*& coord_out, int& coord_size_out) {
    coord_out = new double[3] { 147.123, 258.456, 369.789 };
    coord_size_out = 3;
}

PINVOKE void test_get_array_floats(float*& coord_out, size_t& coord_size_out) {
    size_t size = 3;
    coord_out = new float[size] { 147.123, 258.456, 369.789 };
    coord_size_out = size;
}

struct Array_Floats {
    float* data;
    size_t size;
};

PINVOKE void test_get_nested_array_floats(Array_Floats*& array_out, size_t& array_size_out) {
    size_t outer_size = 2;

    // Allocate memory for the outer array
    array_out = new Array_Floats[outer_size];

    // Allocate memory for each inner array and populate it with data
    array_out[0].data = new float[3] { 1.0f, 2.0f, 3.0f };
    array_out[0].size = 3;

    array_out[1].data = new float[2] { 4.0f, 5.0f };
    array_out[1].size = 2;

    array_size_out = outer_size;
}



PINVOKE void release_nested_array(Array_Floats*& array_out, size_t& array_size_out) {
    // Free the memory for the inner arrays
    for (size_t i = 0; i < array_size_out; ++i) {
        delete[] array_out[i].data;
    }

    // Free the memory for the outer array
    delete[] array_out;


}





// Function to process a nested list and return a flattened list
PINVOKE void process_and_return_nested_list(
    const std::vector<std::vector<int>>& nested_list,
    int* flattened_list,
    int* flattened_list_size) {
    // Access and process the nested list in C++
    // ...

    // Flatten the nested list and store it in the provided flattened_list buffer
    int index = 0;
    for (const auto& sublist : nested_list) {
        for (const auto& item : sublist) {
            flattened_list[index++] = item + 1000;
        }
    }

    // Store the size of the flattened list in flattened_list_size
    *flattened_list_size = index;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Input Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void coord_to_list(size_t* f, size_t& f_s, float* v, size_t& v_s, std::vector<CGAL_Polyline>& plines) {

    //Sanity check     
    if (v_s == 0 || f_s == 0) return;

    //Convert Coordinates to CGAL Polylines
    plines.reserve(f_s);
    plines.emplace_back(CGAL_Polyline());
    plines.back().reserve(f[0]);

    for (size_t i = 0; i < v_s; i += 3) {

        //New polyline
        if (plines.back().size() == f[plines.size() - 1]) {
            plines.emplace_back(CGAL_Polyline());
            plines.back().reserve(f[plines.size() - 1]);
        }
        //Add point to the last polyline
        plines.back().emplace_back((double)v[i + 0], (double)v[i + 1], (double)v[i + 2]);

    }

}

void coord_to_list(size_t* f, size_t& f_s, float* v, size_t& v_s, std::vector<std::vector<IK::Vector_3>>& vectorlists) {
    //Sanity check
    if (v_s == 0 || f_s == 0) return;

    //Convert Coordinates to vector_3 lists
    vectorlists.reserve(f_s);
    vectorlists.emplace_back(std::vector<IK::Vector_3>());
    vectorlists.back().reserve(f[0]);

    for (size_t i = 0; i < v_s; i += 3) {
        //New polyline
        if (vectorlists.back().size() == f[vectorlists.size() - 1]) {
            vectorlists.emplace_back(std::vector<IK::Vector_3>());
            vectorlists.back().reserve(f[vectorlists.size() - 1]);
        }
        //Add point to the last polyline
        vectorlists.back().emplace_back((double)v[i + 0], (double)v[i + 1], (double)v[i + 2]);
    }

}

void coord_to_list(size_t* f, size_t& f_s, int* v, size_t& v_s, std::vector<std::vector<int>>& vectorlists) {
    //Sanity check
    if (v_s == 0 || f_s == 0) return;

    //Convert Coordinates to vector of vectors
    vectorlists.reserve(f_s);
    vectorlists.emplace_back(std::vector<int>());
    vectorlists.back().reserve(f[0]);

    for (size_t i = 0; i < v_s; i++) {
        //New list
        if (vectorlists.back().size() == f[vectorlists.size() - 1]) {
            vectorlists.emplace_back(std::vector<int>());
            vectorlists.back().reserve(f[vectorlists.size() - 1]);
        }
        //Add point to the last polyline
        vectorlists.back().emplace_back(v[i]);
    }
}


void coord_to_list(int* v, size_t& v_s, std::vector<int>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve(v_s);

    for (size_t i = 0; i < v_s; i++) {
        vectorlist.emplace_back(v[i]);
    }
}

void coord_to_list(double* v, size_t& v_s, std::vector<double>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve(v_s);

    for (size_t i = 0; i < v_s; i++) {
        vectorlist.emplace_back(v[i]);
    }
}

void coord_to_list(float* v, size_t& v_s, std::vector<double>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve(v_s);

    for (size_t i = 0; i < v_s; i++) {
        vectorlist.emplace_back((double)v[i]);
    }
}



void coord_to_list(double*& v, int& v_s, std::vector<IK::Segment_3>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve((int)(v_s / 6));

    //CGAL_Debug(v_s);
    //for (size_t i = 0; i < v_s; i++) {
    //    if (i % 6 == 0) {
    //        CGAL_Debug(999);
    //    }
    //    CGAL_Debug(v[i]);
    //}

    //CGAL_Debug(999);

    for (size_t i = 0; i < v_s; i += 6) {
        //CGAL_Debug(v[i * 6 + 0], v[i * 6 + 1], v[i * 6 + 2]);
        //CGAL_Debug(v[i * 6 + 3], v[i * 6 + 4], v[i * 6 + 5]);
        vectorlist.emplace_back(
            IK::Point_3(v[i + 0], v[i + 1], v[i + 2]),
            IK::Point_3(v[i + 3], v[i + 4], v[i + 5])
        );
    }
}

void coord_to_list(double*& v, int& v_s, std::vector<IK::Point_3>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve((int)(v_s / 3));

    for (size_t i = 0; i < v_s; i += 3) {
        vectorlist.emplace_back(
            IK::Point_3(v[i + 0], v[i + 1], v[i + 2])
        );
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Output Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void list_to_coord(std::vector<std::vector<CGAL_Polyline>>& output_plines, size_t*& groups_f, size_t& groups_f_s, size_t*& out_f, size_t& out_f_s, float*& out_v, size_t& out_v_s) {
    out_f_s = 0;
    out_v_s = 0;
    groups_f_s = output_plines.size();

    for (auto& plines : output_plines) {//plates
        out_f_s += plines.size();
        for (auto& pline : plines)//plate polylines
            out_v_s += pline.size() * 3;
    }

    groups_f = new size_t[groups_f_s];
    out_f = new size_t[out_f_s];
    out_v = new float[out_v_s];

    size_t face_count = 0;
    size_t vertex_count = 0;
    for (size_t i = 0; i < output_plines.size(); i++) {
        groups_f[i] = output_plines[i].size();


        for (size_t j = 0; j < output_plines[i].size(); j++) {
            out_f[face_count] = output_plines[i][j].size();

            for (size_t k = 0; k < output_plines[i][j].size(); k++) {
                // CGAL_Debug(output_plines[i][j][k]);
                out_v[vertex_count + 0] = output_plines[i][j][k].hx();
                out_v[vertex_count + 1] = output_plines[i][j][k].hy();
                out_v[vertex_count + 2] = output_plines[i][j][k].hz();
                vertex_count += 3;
            }
            face_count++;
        }
    }

    //coord_out = new double[3]{ 147.123,258.456,369.789 };
    //coord_size_out = 3;
}


void list_to_coord(std::vector<std::vector<CGAL_Polyline>>& output_plines, int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s) {
    out_f_s = 0;
    out_v_s = 0;
    groups_f_s = output_plines.size();

    for (auto& plines : output_plines) {//plates
        out_f_s += plines.size();
        for (auto& pline : plines)//plate polylines
            out_v_s += pline.size() * 3;
    }

    groups_f = new int[groups_f_s];
    out_f = new int[out_f_s];
    out_v = new double[out_v_s];

    int face_count = 0;
    int vertex_count = 0;
    for (int i = 0; i < output_plines.size(); i++) {
        groups_f[i] = output_plines[i].size();
        // CGAL_Debug(groups_f[i]);

        for (int j = 0; j < output_plines[i].size(); j++) {
            out_f[face_count] = output_plines[i][j].size();

            for (int k = 0; k < output_plines[i][j].size(); k++) {
                // CGAL_Debug(output_plines[i][j][k]);
                out_v[vertex_count + 0] = output_plines[i][j][k].hx();
                out_v[vertex_count + 1] = output_plines[i][j][k].hy();
                out_v[vertex_count + 2] = output_plines[i][j][k].hz();
                vertex_count += 3;
            }
            face_count++;
        }
    }

    //coord_out = new double[3]{ 147.123,258.456,369.789 };
    //coord_size_out = 3;
}

void list_to_coord(std::vector<std::vector<IK::Vector_3>>& output_vectors, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s) {
    out_f_s = output_vectors.size();
    out_v_s = 0;

    for (auto& list : output_vectors) {//vectors
        out_v_s += list.size() * 3;
    }

    out_f = new int[output_vectors.size()];//number of arrays
    out_v = new double[out_v_s];//number of coordinates

    int vertex_count = 0;
    for (int i = 0; i < output_vectors.size(); i++) {
        out_f[i] = output_vectors[i].size();//number of sub-arrays

        for (int j = 0; j < output_vectors[i].size(); j++) {
            out_v[vertex_count + 0] = output_vectors[i][j].hx();
            out_v[vertex_count + 1] = output_vectors[i][j].hy();
            out_v[vertex_count + 2] = output_vectors[i][j].hz();
            vertex_count += 3;
        }
    }
}

void list_to_coord(std::vector<std::vector<int>>& output_vectors, int*& out_f, int& out_f_s, int*& out_v, int& out_v_s) {
    out_f_s = output_vectors.size();
    out_v_s = 0;

    for (auto& list : output_vectors) {//vectors
        out_v_s += list.size();
    }

    out_f = new int[output_vectors.size()];//number of arrays
    out_v = new int[out_v_s];//number of coordinates

    int vertex_count = 0;
    for (int i = 0; i < output_vectors.size(); i++) {
        out_f[i] = output_vectors[i].size();//number of sub-arrays

        for (int j = 0; j < output_vectors[i].size(); j++) {
            out_v[vertex_count] = output_vectors[i][j];
            vertex_count++;
        }
    }
}

void list_to_coord(std::vector<std::vector<wood_cut::cut_type>>& output_vectors, size_t*& out_f, size_t& out_f_s, int*& out_v, size_t& out_v_s) {
    out_f_s = output_vectors.size();
    out_v_s = 0;

    for (auto& list : output_vectors) {//vectors
        out_v_s += list.size();
    }

    out_f = new size_t[output_vectors.size()];//number of arrays
    out_v = new int[out_v_s];//number of coordinates


    size_t vertex_count = 0;
    for (size_t i = 0; i < output_vectors.size(); i++) {
        out_f[i] = output_vectors[i].size();//number of sub-arrays

        for (size_t j = 0; j < output_vectors[i].size(); j++) {
            out_v[vertex_count] = (int)output_vectors[i][j];
            vertex_count++;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PINVOKE void ctypes_get_connection_zones(

    // input geometry
    //size_t* in_polyline_pairs_f, size_t& in_polyline_pairs_f_s
    size_t* in_polyline_pairs_f, size_t& in_polyline_pairs_f_s, float* in_polyline_pairs_v, size_t& in_polyline_pairs_v_s,
    size_t* in_vectors_f, size_t& in_vectors_f_s, float* in_vectors_v, size_t& in_vectors_v_s,
    size_t* in_joints_types_f, size_t& in_joints_types_f_s, int* in_joints_types_v, size_t& in_joints_types_v_s,
    size_t* in_three_valence_f, size_t& in_three_valence_f_s, int* in_three_valence_v, size_t& in_three_valence_v_s,
    int* in_adjancency_v, size_t& in_adjancency_v_s,

    //input parameters
    float* in_joint_parameters_v, size_t& in_joint_parameters_v_s,
    int& in_search_type,
    float* in_scale_v, size_t& in_scale_v_s,
    int& in_output_type,

    // output
    size_t*& out_plines_groups_f, size_t& out_plines_groups_f_s, size_t*& out_plines_out_f, size_t& out_plines_out_f_s, float*& out_plines_out_v, size_t& out_plines_out_v_s,
    size_t*& out_types_f, size_t& out_types_f_s, int*& out_types_v, size_t& out_types_v_s,

    // global_parameters
    float* in_joint_volume_parameters_v, size_t& in_joint_volume_parameters_v_s
) {

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Output
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    // input geometry
    std::vector<CGAL_Polyline> in_polyline_pairs;
    coord_to_list(in_polyline_pairs_f, in_polyline_pairs_f_s, in_polyline_pairs_v, in_polyline_pairs_v_s, in_polyline_pairs);

    std::vector<std::vector<IK::Vector_3>> in_vectors;
    coord_to_list(in_vectors_f, in_vectors_f_s, in_vectors_v, in_vectors_v_s, in_vectors);

    std::vector<std::vector<int>> in_joints_types;
    coord_to_list(in_joints_types_f, in_joints_types_f_s, in_joints_types_v, in_joints_types_v_s, in_joints_types);

    std::vector<std::vector<int>> in_three_valence;
    coord_to_list(in_three_valence_f, in_three_valence_f_s, in_three_valence_v, in_three_valence_v_s, in_three_valence);

    std::vector<int> in_adjancency;
    coord_to_list(in_adjancency_v, in_adjancency_v_s, in_adjancency);

    // input parameters
    std::vector<double> in_joint_parameters;
    coord_to_list(in_joint_parameters_v, in_joint_parameters_v_s, in_joint_parameters);

    // for (auto& i : in_joint_parameters) {
    //     printf("in_joint_parameters: %f\n", i);
    // }

    std::vector<double> in_scale;
    coord_to_list(in_scale_v, in_scale_v_s, in_scale);


    // for (auto& i : in_scale) {
    //     printf("in_scale: %f\n", i);
    // }

    // global_parameters
    std::vector<double> in_joint_volume_parameters;
    coord_to_list(in_joint_volume_parameters_v, in_joint_volume_parameters_v_s, in_joint_volume_parameters);

    // for (auto& i : in_joint_volume_parameters) {
    //     printf("in_joint_volume_parameters: %f\n", i);
    // }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Main Method of Wood
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<CGAL_Polyline>> out_plines;
    std::vector<std::vector<wood_cut::cut_type>> out_types;
    std::vector<std::vector<int>> top_face_triangulation;

    wood_globals::JOINTS_PARAMETERS_AND_TYPES = in_joint_parameters;

    for (size_t i = 0; i < in_joint_volume_parameters.size(); i++)
        wood_globals::JOINT_VOLUME_EXTENSION[i] = (in_joint_volume_parameters[i]);

    wood_globals::OUTPUT_GEOMETRY_TYPE = in_output_type;


    wood_main::get_connection_zones(
        // input
        in_polyline_pairs,
        in_vectors,
        in_joints_types,
        in_three_valence,
        in_adjancency,
        // output
        out_plines,
        out_types,
        top_face_triangulation,
        // global parameters
        wood_globals::JOINTS_PARAMETERS_AND_TYPES,
        in_scale,
        in_search_type,
        wood_globals::OUTPUT_GEOMETRY_TYPE,
        0
    );


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Convert polylines to double vector and cut types to int vector
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // size_t* out_plines_groups_f;
    // size_t out_plines_groups_f_s = 0;
    // size_t* out_plines_out_f;
    // size_t out_plines_out_f_s = 0;
    // float* out_plines_out_v;
    // size_t out_plines_out_v_s = 0;
    list_to_coord(out_plines, out_plines_groups_f, out_plines_groups_f_s, out_plines_out_f, out_plines_out_f_s, out_plines_out_v, out_plines_out_v_s);
    list_to_coord(out_types, out_types_f, out_types_f_s, out_types_v, out_types_v_s);
    // printf("out_plines_groups_f: %f\n", out_plines[0][0][0].x());
    // printf("out_plines_groups_f: %zu\n", out_plines_groups_f[0]);

}

// PINVOKE void pinvoke_get_connection_zones(
//     // _____________________________________________________ input
//     float** input_polyline_pairs_coords, size_t input_polyline_pairs_rows, size_t* input_polyline_pairs_cols,
//     // _____________________________________________________ output
//     float* arr, size_t& size
//     // _____________________________________________________ global parameters
//     // float** input_polyline_pairs_coords, int input_polyline_pairs_rows, int* input_polyline_pairs_cols
//     // // input
//     // std::vector<std::vector<double>>& pybind11_input_polyline_pairs,
//     // std::vector<std::vector<double>>& pybind11_input_insertion_vectors,
//     // std::vector<std::vector<int>>& pybind11_input_JOINTS_TYPES,
//     // std::vector<std::vector<int>>& pybind11_input_three_valence_element_indices_and_instruction,
//     // std::vector<int>& pybind11_input_adjacency,
//     // std::vector<double>& pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES,
//     // int& _search_type,
//     // std::vector<double>& _scale,
//     // int& _output_type,
//     // // output
//     // std::vector<std::vector<std::vector<double>>>& pybind11_output_plines,
//     // std::vector<std::vector<int>>& pybind11_output_types,
//     // // global_parameters
//     // std::vector<double>& pybind11_joint_volume_parameters
// )
// {

//     std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
//     // Process each row of the nested array
//     for (size_t i = 0; i < input_polyline_pairs_rows; i++) {
//         float* row = input_polyline_pairs_coords[i];
//         size_t num_cols_in_row = input_polyline_pairs_cols[i];
//         // Process each element in the row
//         input_polyline_pairs.emplace_back(std::vector<IK::Point_3>());
//         for (size_t j = 0; j < num_cols_in_row; j += 3) {
//             input_polyline_pairs.back().emplace_back(IK::Point_3(row[j], row[j + 1], row[j + 2]));
//         }
//     }

//     // Populate the array with some values
//     arr[0] = 1.0;
//     arr[1] = 2.0;
//     arr[2] = 3.0;
//     arr[3] = 4.0;
//     arr[4] = 5.0;

//     // Set the size variable to the length of the array
//     size = 5;
//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // // The filename of the xml file and the screenshot directory
//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//     // std::vector<std::vector<IK::Point_3>> input_polyline_pairs;
//     // python_to_cpp__cpp_to_python::coord_to_vector_of_polylines(pybind11_input_polyline_pairs, input_polyline_pairs);

//     // std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
//     // python_to_cpp__cpp_to_python::coord_to_vector_of_vectors(pybind11_input_insertion_vectors, input_insertion_vectors);


//     // std::vector<std::vector<int>> input_JOINTS_TYPES = pybind11_input_JOINTS_TYPES = pybind11_input_JOINTS_TYPES;
//     // std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction = pybind11_input_three_valence_element_indices_and_instruction;
//     // std::vector<int> input_adjacency = pybind11_input_adjacency;

//     // int search_type = _search_type;
//     // std::vector<double> scale = _scale;
//     // int output_type = _output_type;

//     // wood_globals::JOINTS_PARAMETERS_AND_TYPES = pybind11_wood_globals_JOINTS_PARAMETERS_AND_TYPES;
//     // wood_globals::OUTPUT_GEOMETRY_TYPE = output_type;
//     // for (int i = 0; i < pybind11_joint_volume_parameters.size(); i++)
//     //     wood_globals::JOINT_VOLUME_EXTENSION[i] = pybind11_joint_volume_parameters[i];


//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // // Main Method of Wood
//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // std::vector<std::vector<CGAL_Polyline>> output_plines;
//     // std::vector<std::vector<wood_cut::cut_type>> output_types;
//     // std::vector<std::vector<int>> top_face_triangulation;


//     // wood_main::get_connection_zones(
//     //     // input
//     //     input_polyline_pairs,
//     //     input_insertion_vectors,
//     //     input_JOINTS_TYPES,
//     //     input_three_valence_element_indices_and_instruction,
//     //     input_adjacency,
//     //     // output
//     //     output_plines,
//     //     output_types,
//     //     top_face_triangulation,
//     //     // Global Parameters
//     //     wood_globals::JOINTS_PARAMETERS_AND_TYPES,
//     //     scale,
//     //     search_type,
//     //     wood_globals::OUTPUT_GEOMETRY_TYPE,
//     //     0);

//     // //printf("CPP 6");
//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // // Convert polylines to double vector and cut types to int vector
//     // ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     // python_to_cpp__cpp_to_python::nested_vector_of_polylines_to_coord(output_plines, pybind11_output_plines);
//     // python_to_cpp__cpp_to_python::nested_vector_of_cut_type_to_nested_vector_of_int(output_types, pybind11_output_types); // only filled when 3rd output type is uesd
//     // //printf("CPP 7");
// }


// PINVOKE int pinvoke_get_connection_zones(
//     //input
//     int* f, int f_s, double* v, int v_s,
//     int* vec_f, int vec_f_s, double* vec_v, int vec_v_s,
//     int* jointtypes_f, int jointtypes_f_s, int* jointtypes_v, int jointtypes_v_s,
//     int* threevalence_f, int threevalence_f_s, int* threevalence_v, int threevalence_v_s,
//     int* adjacency_v, int adjacency_v_s,
//     double* jointparams_v, int jointparams_v_s,
//     double* scale_v, int scale_v_s,
//     const char* file_and_folder_of_joint_library_xml,
//     //output
//     int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s,
//     //optional inputs
//     int search_type, int output_type, int triangulate
// ) {
//     path_and_file_for_joints = std::string(file_and_folder_of_joint_library_xml) + "\\joinery_library.xml";

//     //std::ofstream myfile;
//     //myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");
//     //myfile << "Dir\n";
//     //myfile << path_and_file_for_joints;
//     //myfile.close();

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //input_convertion
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     std::vector<CGAL_Polyline> input_polyline_pairs;
//     std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
//     std::vector<std::vector<int>> input_JOINTS_TYPES;
//     std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
//     std::vector<int> input_adjacency;
//     std::vector<double> default_parameters_for_JOINTS_TYPES;
//     std::vector<double> scale;

//     coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
//     //if (v_s == vec_v_s && f_s == vec_f_s)
//     coord_to_list(vec_f, vec_f_s, vec_v, vec_v_s, input_insertion_vectors);
//     //if (f_s == vec_f_s)
//     coord_to_list(jointtypes_f, jointtypes_f_s, jointtypes_v, jointtypes_v_s, input_JOINTS_TYPES);
//     //if (threevalence_v_s % 4 == 0)
//     coord_to_list(threevalence_f, threevalence_f_s, threevalence_v, threevalence_v_s, input_three_valence_element_indices_and_instruction);
//     //if (adjacency_v_s % 2 == 0)
//     coord_to_list(adjacency_v, adjacency_v_s, input_adjacency);
//     //if (jointparams_v_s % 3 == 0)
//     coord_to_list(jointparams_v, jointparams_v_s, default_parameters_for_JOINTS_TYPES);

//     coord_to_list(scale_v, scale_v_s, scale);
//     //for (auto& o : default_parameters_for_JOINTS_TYPES)
//         //CGAL_Debug(o);

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //run
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     groups_f_s = 0;
//     out_f_s = 0;
//     out_v_s = 0;
//     //CGAL_Debug(search_type);
//     //CGAL_Debug(f_s);
//     //CGAL_Debug(v_s);
//     std::vector<std::vector<CGAL_Polyline>> output_plines;
//     std::vector<std::vector<int>> output_top_face_triangulation;

//     get_connection_zones(
//         input_polyline_pairs, input_insertion_vectors, input_JOINTS_TYPES, input_three_valence_element_indices_and_instruction, input_adjacency,
//         output_plines, output_top_face_triangulation,
//         default_parameters_for_JOINTS_TYPES, scale,
//         search_type, output_type, triangulate
//     );//division_distance, shift,

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //output_conversion
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //std::ofstream myfile;
//     //myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");

//     //for (auto& p : output_plines[0][0]) {
//     //    myfile << p[0];
//     //    myfile << "\n";
//     //    myfile << p[1];
//     //    myfile << "\n";
//     //    myfile << p[2];
//     //    myfile << "\n";
//     //    myfile << "\n";
//     //}

//     //myfile.close();

//     list_to_coord(output_plines, groups_f, groups_f_s, out_f, out_f_s, out_v, out_v_s);

//     return 1;
// }

// PINVOKE int pinvoke_find_closest_plateside_to_line(
//     //input - polylines flat list and a flat list of lines
//     int* f, int f_s, double* v, int v_s,
//     double* lines_v, int lines_v_s,
//     //output - jagged array of vectors
//     int*& out_f, int& out_f_s, double*& out_v, int& out_v_s
// ) {
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //input_convertion
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     std::vector<CGAL_Polyline> input_polyline_pairs;
//     std::vector<IK::Segment_3> input_insertion_segments;

//     coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
//     coord_to_list(lines_v, lines_v_s, input_insertion_segments);

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //run
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     std::vector<std::vector<IK::Vector_3>> output_insertion_vectors;

//     rtree_util::find_closest_plateside_to_line(input_polyline_pairs, input_insertion_segments, output_insertion_vectors);
//     //for (int i = 0; i < output_insertion_vectors.size(); i++) {
//     //    CGAL_Debug(0);
//     //    for (int j = 0; j < output_insertion_vectors[i].size(); j++) {
//     //        CGAL_Debug(output_insertion_vectors[i][j]);
//     //    }
//     //}

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //output_conversion
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //CGAL_Debug(output_plines.size());
//     list_to_coord(output_insertion_vectors, out_f, out_f_s, out_v, out_v_s);

//     return 1;
// }

// PINVOKE int pinvoke_find_closest_plateside_to_indexedpoint(
//     //input - polylines flat list and a flat list of lines
//     int* f, int f_s, double* v, int v_s,
//     double* pts_v, int pts_v_s,
//     int* ids_v, int ids_v_s,
//     //output - jagged array of integers, representing joint types
//     int*& out_f, int& out_f_s, int*& out_v, int& out_v_s
// ) {
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// //input_convertion
// /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     std::vector<CGAL_Polyline> input_polyline_pairs;
//     std::vector<IK::Point_3> input_joint_pts;
//     std::vector<int> input_joint_ids;

//     coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
//     coord_to_list(pts_v, pts_v_s, input_joint_pts);
//     coord_to_list(ids_v, ids_v_s, input_joint_ids);

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //run
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     std::vector<std::vector<int>> output_JOINTS_TYPES;

//     rtree_util::find_closest_plateside_to_indexedpoint(input_polyline_pairs, input_joint_pts, input_joint_ids, output_JOINTS_TYPES);
//     //for (int i = 0; i < output_JOINTS_TYPES.size(); i++) {
//     //    CGAL_Debug(0);
//     //    for (int j = 0; j < output_JOINTS_TYPES[i].size(); j++) {
//     //        CGAL_Debug(output_JOINTS_TYPES[i][j]);
//     //    }
//     //}

//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //output_conversion
//     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//     //CGAL_Debug(output_plines.size());
//     list_to_coord(output_JOINTS_TYPES, out_f, out_f_s, out_v, out_v_s);

//     return 1;
// }
