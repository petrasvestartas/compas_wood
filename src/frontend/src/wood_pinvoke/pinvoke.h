#pragma once
#include "../../../stdafx.h"              //includes
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic Method and Examples for PInvoke
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<cstdio>


#ifdef _WIN32 // Check if the target platform is Windows
    #define PINVOKE extern "C" __declspec(dllexport)
#else // Assuming the target platform is Mac
    #define PINVOKE extern "C" __attribute__((visibility("default")))
#endif

#define deletePtr(ptr, isArray) if (isArray) {delete[] arr;} else {delete arr;}

PINVOKE int test_get_square(int n);
PINVOKE int test_get_square2(int n);

// Function to process an integer array and return its sum
PINVOKE int process_and_return_sum(int* array, int size);

PINVOKE void release_int(int*& arr, bool isArray);

PINVOKE void release_double(double*& arr, bool isArray);

PINVOKE void release_float(float*& arr, bool isArray);

PINVOKE void release_size_t(size_t*& arr, bool isArray);

PINVOKE void test_set_array(double*& coord, int coord_size);

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

void coord_to_list(int* v, size_t& v_s, std::vector<int>& vectorlist);
void coord_to_list(double* v, size_t& v_s, std::vector<double>& vectorlist);
void coord_to_list(float* v, size_t& v_s, std::vector<double>& vectorlist);

void coord_to_list(double*& v, int& v_s, std::vector<IK::Segment_3>& vectorlist);
void coord_to_list(double*& v, int& v_s, std::vector<IK::Point_3>& vectorlist);


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Output Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void list_to_coord(std::vector<std::vector<CGAL_Polyline>>& output_plines, size_t*& groups_f, size_t& groups_f_s, size_t*& out_f, size_t& out_f_s, float*& out_v, size_t& out_v_s);
void list_to_coord(std::vector<std::vector<CGAL_Polyline>>& output_plines, int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s);
void list_to_coord(std::vector<std::vector<IK::Vector_3>>& output_vectors, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s);
void list_to_coord(std::vector<std::vector<wood_cut::cut_type>>& output_vectors, size_t*& out_f, size_t& out_f_s, int*& out_v, size_t& out_v_s);
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
    float* in_joint_volume_parameters_v, size_t& in_joint_volume_parameters_v_s,
    float& face_to_face_side_to_side_joints_dihedral_angle
);

PINVOKE void ctypes_get_connection_zones_with_custom_joints(

    // input geometry
    size_t* in_polyline_pairs_f, size_t& in_polyline_pairs_f_s, float* in_polyline_pairs_v, size_t& in_polyline_pairs_v_s,
    size_t* in_vectors_f, size_t& in_vectors_f_s, float* in_vectors_v, size_t& in_vectors_v_s,
    size_t* in_joints_types_f, size_t& in_joints_types_f_s, int* in_joints_types_v, size_t& in_joints_types_v_s,
    size_t* in_three_valence_f, size_t& in_three_valence_f_s, int* in_three_valence_v, size_t& in_three_valence_v_s,
    int* in_adjancency_v, size_t& in_adjancency_v_s,

    //custom joints
    size_t* in_joint_polyline_pairs_f, size_t& in_joint_polyline_pairs_f_s, float* in_joint_polyline_pairs_v, size_t& in_joint_polyline_pairs_v_s,
    int* in_joint_types_v, size_t& in_joint_types_v_s,

    //input parameters
    float* in_joint_parameters_v, size_t& in_joint_parameters_v_s,
    int& in_search_type,
    float* in_scale_v, size_t& in_scale_v_s,
    int& in_output_type,

    // output
    size_t*& out_plines_groups_f, size_t& out_plines_groups_f_s, size_t*& out_plines_out_f, size_t& out_plines_out_f_s, float*& out_plines_out_v, size_t& out_plines_out_v_s,
    size_t*& out_types_f, size_t& out_types_f_s, int*& out_types_v, size_t& out_types_v_s,

    // global_parameters
    float* in_joint_volume_parameters_v, size_t& in_joint_volume_parameters_v_s,
    float& face_to_face_side_to_side_joints_dihedral_angle
);