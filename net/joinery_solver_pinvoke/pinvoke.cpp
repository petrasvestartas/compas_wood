#include "pinvoke.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Basic Method and Examples for PInvoke
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PINVOKE int test_get_square(int n) {
    return n * n;
}

PINVOKE void release_int(int* arr, bool isArray) {
    deletePtr(arr, isArray);
}

PINVOKE void release_double(double* arr, bool isArray) {
    deletePtr(arr, isArray);
}

PINVOKE void test_set_array(double* coord, int coord_size) {
    for (int i = 0; i < coord_size; i++)
        printf("%f \n", coord[i]);
}

PINVOKE void test_get_array(double*& coord_out, int& coord_size_out) {
    coord_out = new double[3]{ 147.123,258.456,369.789 };
    coord_size_out = 3;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Input Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void coord_to_list(int*& f, int& f_s, double*& v, int& v_s, std::vector<CGAL_Polyline>& plines) {
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
            plines.back().reserve(f[plines.size()]);
        }
        //Add point to the last polyline
        plines.back().emplace_back(v[i + 0], v[i + 1], v[i + 2]);
    }
}

void coord_to_list(int*& f, int& f_s, double*& v, int& v_s, std::vector<std::vector<IK::Vector_3>>& vectorlists) {
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
            vectorlists.back().reserve(f[vectorlists.size()]);
        }
        //Add point to the last polyline
        vectorlists.back().emplace_back(v[i + 0], v[i + 1], v[i + 2]);
    }
}

void coord_to_list(int*& f, int& f_s, int*& v, int& v_s, std::vector<std::vector<int>>& vectorlists) {
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
            vectorlists.back().reserve(f[vectorlists.size()]);
        }
        //Add point to the last polyline
        vectorlists.back().emplace_back(v[i]);
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

void coord_to_list(int*& v, int& v_s, std::vector<int>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve(v_s);

    for (size_t i = 0; i < v_s; i++) {
        vectorlist.emplace_back(v[i]);
    }
}

void coord_to_list(double*& v, int& v_s, std::vector<double>& vectorlist) {
    //Sanity check
    if (v_s == 0) return;

    //Convert Coordinates to vector list
    vectorlist.reserve(v_s);

    for (size_t i = 0; i < v_s; i++) {
        vectorlist.emplace_back(v[i]);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Output Conversion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    //output
    int*& groups_f, int& groups_f_s, int*& out_f, int& out_f_s, double*& out_v, int& out_v_s,
    //optional inputs
    int search_type, double division_distance, double shift, int output_type, int triangulate
) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //input_convertion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector<std::vector<int>> input_joint_types;
    std::vector<std::vector<int>> input_three_valence_element_indices_and_instruction;
    std::vector<int> input_adjacency;
    std::vector<double> default_parameters_for_joint_types;

    coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
    //if (v_s == vec_v_s && f_s == vec_f_s)
    coord_to_list(vec_f, vec_f_s, vec_v, vec_v_s, input_insertion_vectors);
    //if (f_s == vec_f_s)
    coord_to_list(jointtypes_f, jointtypes_f_s, jointtypes_v, jointtypes_v_s, input_joint_types);
    //if (threevalence_v_s % 4 == 0)
    coord_to_list(threevalence_f, threevalence_f_s, threevalence_v, threevalence_v_s, input_three_valence_element_indices_and_instruction);
    //if (adjacency_v_s % 2 == 0)
    coord_to_list(adjacency_v, adjacency_v_s, input_adjacency);
    //if (jointparams_v_s % 3 == 0)
    coord_to_list(jointparams_v, jointparams_v_s, default_parameters_for_joint_types);
    //for (auto& o : default_parameters_for_joint_types)
        //CGAL_Debug(o);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //run
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    groups_f_s = 0;
    out_f_s = 0;
    out_v_s = 0;
    //CGAL_Debug(search_type);
    //CGAL_Debug(f_s);
    //CGAL_Debug(v_s);
    std::vector<std::vector<CGAL_Polyline>> output_plines;
    std::vector<std::vector<int>> output_top_face_triangulation;

    get_connection_zones(
        input_polyline_pairs, input_insertion_vectors, input_joint_types, input_three_valence_element_indices_and_instruction, input_adjacency,
        output_plines, output_top_face_triangulation,
        default_parameters_for_joint_types,
        search_type, division_distance, shift, output_type, triangulate
    );

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //output_conversion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //CGAL_Debug(output_plines.size());
    list_to_coord(output_plines, groups_f, groups_f_s, out_f, out_f_s, out_v, out_v_s);

    return 1;
}

PINVOKE int pinvoke_find_closest_plateside_to_line(
    //input - polylines flat list and a flat list of lines
    int* f, int f_s, double* v, int v_s,
    double* lines_v, int lines_v_s,
    //output - jagged array of vectors
    int*& out_f, int& out_f_s, double*& out_v, int& out_v_s
) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //input_convertion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    std::vector<IK::Segment_3> input_insertion_segments;

    coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
    coord_to_list(lines_v, lines_v_s, input_insertion_segments);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //run
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<IK::Vector_3>> output_insertion_vectors;

    rtree_util::find_closest_plateside_to_line(input_polyline_pairs, input_insertion_segments, output_insertion_vectors);
    //for (int i = 0; i < output_insertion_vectors.size(); i++) {
    //    CGAL_Debug(0);
    //    for (int j = 0; j < output_insertion_vectors[i].size(); j++) {
    //        CGAL_Debug(output_insertion_vectors[i][j]);
    //    }
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //output_conversion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //CGAL_Debug(output_plines.size());
    list_to_coord(output_insertion_vectors, out_f, out_f_s, out_v, out_v_s);

    return 1;
}

PINVOKE int pinvoke_find_closest_plateside_to_indexedpoint(
    //input - polylines flat list and a flat list of lines
    int* f, int f_s, double* v, int v_s,
    double* pts_v, int pts_v_s,
    int* ids_v, int ids_v_s,
    //output - jagged array of integers, representing joint types
    int*& out_f, int& out_f_s, int*& out_v, int& out_v_s
) {
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//input_convertion
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    std::vector<IK::Point_3> input_joint_pts;
    std::vector<int> input_joint_ids;

    coord_to_list(f, f_s, v, v_s, input_polyline_pairs);
    coord_to_list(pts_v, pts_v_s, input_joint_pts);
    coord_to_list(ids_v, ids_v_s, input_joint_ids);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //run
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    std::vector<std::vector<int>> output_joint_types;

    rtree_util::find_closest_plateside_to_indexedpoint(input_polyline_pairs, input_joint_pts, input_joint_ids, output_joint_types);
    //for (int i = 0; i < output_joint_types.size(); i++) {
    //    CGAL_Debug(0);
    //    for (int j = 0; j < output_joint_types[i].size(); j++) {
    //        CGAL_Debug(output_joint_types[i][j]);
    //    }
    //}

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //output_conversion
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //CGAL_Debug(output_plines.size());
    list_to_coord(output_joint_types, out_f, out_f_s, out_v, out_v_s);

    return 1;
}
