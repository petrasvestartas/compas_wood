#pragma once
#include "../../../stdafx.h"              //includes
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic Method and Examples for PInvoke
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<cstdio>


#define PINVOKE extern "C" __declspec(dllexport)
#define deletePtr(ptr, isArray) if (isArray) {delete[] arr;} else {delete arr;}

PINVOKE int test_get_square(int n);
PINVOKE int test_get_square2(int n);

// Function to process an integer array and return its sum
PINVOKE int process_and_return_sum(int* array, int size);

PINVOKE void release_int(int* arr, bool isArray);

PINVOKE void release_double(double* arr, bool isArray);

PINVOKE void release_float(float* arr, bool isArray);

PINVOKE void test_set_array(double* coord, int coord_size);

PINVOKE void test_get_array(double*& coord_out, int& coord_size_out);

PINVOKE void test_get_array_floats(float*& coord_out, size_t& coord_size_out);


typedef struct Array_Floats Array_Floats_;

PINVOKE void test_get_nested_array_floats(Array_Floats_*& array_out, size_t& array_size_out);

PINVOKE void release_nested_array(Array_Floats*& array_out, size_t& array_size_out);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Input Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void coord_to_list(size_t* f, size_t& f_s, float* v, size_t& v_s, std::vector<CGAL_Polyline>& plines);
void coord_to_list(size_t* f, size_t& f_s, float* v, size_t& v_s, std::vector<std::vector<IK::Vector_3>>& vectorlists);
void coord_to_list(size_t* f, size_t& f_s, int* v, size_t& v_s, std::vector<std::vector<int>>& vectorlists);

void coord_to_list(double*& v, int& v_s, std::vector<IK::Segment_3>& vectorlist);
void coord_to_list(double*& v, int& v_s, std::vector<IK::Point_3>& vectorlist);
void coord_to_list(int*& v, int& v_s, std::vector<int>& vectorlist);
void coord_to_list(double*& v, int& v_s, std::vector<double>& vectorlist);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Output Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void list_to_coord(std::vector<std::vector<CGAL_Polyline>>& output_plines, int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s);
void list_to_coord(std::vector<std::vector<IK::Vector_3>>& output_vectors, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s);
void list_to_coord(std::vector<std::vector<int>>& output_vectors, int*& out_f, int& out_f_s, int*& out_v, int& out_v_s);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Implementations
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PINVOKE void pinvoke_get_connection_zones(
    float** input_polyline_pairs_coords, size_t input_polyline_pairs_rows, size_t* input_polyline_pairs_cols,
    float* arr, size_t& size
);




PINVOKE void ctypes_get_connection_zones(

    // input
    //size_t* in_polyline_pairs_f, size_t& in_polyline_pairs_f_s
    size_t* in_polyline_pairs_f, size_t& in_polyline_pairs_f_s, float* in_polyline_pairs_v, size_t& in_polyline_pairs_v_s,
    size_t* in_vectors_f, size_t& in_vectors_f_s, float* in_vectors_v, size_t& in_vectors_v_s,
    size_t* in_joints_types_f, size_t& in_joints_types_f_s, int* in_joints_types_v, size_t& in_joints_types_v_s


    // int*& in_joints_types_f, int& in_joints_types_f_s, int*& in_joints_types_v, int& in_joints_types_v_s,
    // int*& in_three_valence_f, int& in_three_valence_f_s, int*& in_three_valence_v, int& in_three_valence_v_s,
    // int& in_search_type,
    // double*& in_scale_v, int& in_scale_v_s
    // int& in_output_type,

    // // output
    // int*& out_plines_groups_f, int& out_plines_groups_f_s, int*& out_plines_out_f, int& out_plines_out_f_s, double*& out_plines_out_v, int& out_plines_out_v_s,//std::vector<std::vector<CGAL_Polyline>> &output_plines,
    // int*& out_types_f, int& out_types_f_s, int*& out_types_v, int& out_types_v_s,//std::vector<std::vector<wood_cut::cut_type>> &output_types,

    // // global_parameters
    // double*& in_joint_volume_parameters_v, int& in_joint_volume_parameters_v_s
);

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
//     int search_type = 1, int output_type = 4, int triangulate = 0

// );

// PINVOKE int pinvoke_find_closest_plateside_to_line(
//     //input - polylines flat list and a flat list of lines
//     int* f, int f_s, double* v, int v_s,
//     double* lines_v, int lines_v_s,
//     //output - jagged array of vectors
//     int*& out_f, int& out_f_s, double*& out_v, int& out_v_s
// );

// PINVOKE int pinvoke_find_closest_plateside_to_indexedpoint(
//     //input - polylines flat list and a flat list of lines
//     int* f, int f_s, double* v, int v_s,
//     double* pts_v, int pts_v_s,
//     int* ids_v, int ids_v_s,
//     //output - jagged array of vectors
//     int*& out_f, int& out_f_s, int*& out_v, int& out_v_s
// );
