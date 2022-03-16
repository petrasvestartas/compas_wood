#pragma once
#include "joinery_solver_main.h"
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic Method and Examples for PInvoke
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include<cstdio>

#define PINVOKE extern "C" __declspec(dllexport)
#define deletePtr(ptr, isArray) if (isArray) {delete[] arr;} else {delete arr;}

PINVOKE int test_get_square(int n);

PINVOKE void release_int(int* arr, bool isArray);

PINVOKE void release_double(double* arr, bool isArray);

PINVOKE void test_set_array(double* coord, int coord_size);

PINVOKE void test_get_array(double*& coord_out, int& coord_size_out);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Input Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void coord_to_list(int*& f, int& f_s, double*& v, int& v_s, std::vector<CGAL_Polyline>& plines);
void coord_to_list(int*& f, int& f_s, double*& v, int& v_s, std::vector<std::vector<IK::Vector_3>>& vectorlists);
void coord_to_list(int*& f, int& f_s, int*& v, int& v_s, std::vector<std::vector<int>>& vectorlists);
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

PINVOKE int pinvoke_get_connection_zones(
    //input
    int* f, int f_s, double* v, int v_s,
    int* vec_f, int vec_f_s, double* vec_v, int vec_v_s,
    int* jointtypes_f, int jointtypes_f_s, int* jointtypes_v, int jointtypes_v_s,
    int* threevalence_f, int threevalence_f_s, int* threevalence_v, int threevalence_v_s,
    int* adjacency_v, int adjacency_v_s,
    double* jointparams_v, int jointparams_v_s,
    double* scale_v, int scale_v_s,
    const char* file_and_folder_of_joint_library_xml,
    //output
    int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s,
    //optional inputs
    int search_type = 1, int output_type = 4, int triangulate = 0

);

PINVOKE int pinvoke_find_closest_plateside_to_line(
    //input - polylines flat list and a flat list of lines
    int* f, int f_s, double* v, int v_s,
    double* lines_v, int lines_v_s,
    //output - jagged array of vectors
    int*& out_f, int& out_f_s, double*& out_v, int& out_v_s
);

PINVOKE int pinvoke_find_closest_plateside_to_indexedpoint(
    //input - polylines flat list and a flat list of lines
    int* f, int f_s, double* v, int v_s,
    double* pts_v, int pts_v_s,
    int* ids_v, int ids_v_s,
    //output - jagged array of vectors
    int*& out_f, int& out_f_s, int*& out_v, int& out_v_s
);
