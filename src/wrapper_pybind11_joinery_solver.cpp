#include "wrapper_pybind11_joinery_solver.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Converter methods, where np.asarray are mapped to Eiger matrix
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma region converter

RowMatrixXd result_from_polyline(CGAL_Polyline poly) {
    RowMatrixXd points(poly.size(), 3);

    for (int i = 0; i < poly.size(); i++) {
        points(i, 0) = poly[i].hx();
        points(i, 1) = poly[i].hy();
        points(i, 2) = poly[i].hz();
    }

    return points;
}

std::vector<RowMatrixXd> result_from_polylines(CGAL_Polylines polylines) {
    std::vector<RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {
        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        RowMatrixXd points(n, 3);

        for (int j = 0; j < n; j++) {
            points(j, 0) = (double)poly[j].x();
            points(j, 1) = (double)poly[j].y();
            points(j, 2) = (double)poly[j].z();
        }

        pointsets.emplace_back(points);
    }

    return pointsets;
}

std::vector<RowMatrixXd> result_from_polylines(std::vector<CGAL_Polyline> polylines) {
    std::vector<RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {
        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        RowMatrixXd points(n, 3);

        for (int j = 0; j < n; j++) {
            points(j, 0) = (double)poly[j].x();
            points(j, 1) = (double)poly[j].y();
            points(j, 2) = (double)poly[j].z();
        }

        pointsets.emplace_back(points);
    }

    return pointsets;
}

std::vector<RowMatrixXd> result_from_vector(IK::Vector_3* v) {
    std::vector<RowMatrixXd> pointsets;

    //for (auto i = 0; i < AABBs.size(); i++) {
    int n = 2;
    RowMatrixXd points(4, 3);

    points(0, 0) = (double)v[0].x();
    points(0, 1) = (double)v[0].y();
    points(0, 2) = (double)v[0].z();
    points(1, 0) = (double)v[1].x();
    points(1, 1) = (double)v[1].y();
    points(1, 2) = (double)v[1].z();
    points(2, 0) = (double)v[2].x();
    points(2, 1) = (double)v[2].y();
    points(2, 2) = (double)v[2].z();
    points(3, 0) = (double)v[3].x();
    points(3, 1) = (double)v[3].y();
    points(3, 2) = (double)v[3].z();

    pointsets.push_back(points);
    // }
    return pointsets;
}

std::vector<RowMatrixXd> result_from_bbox(std::vector<CGAL::Bbox_3> AABBs) {
    std::vector<RowMatrixXd> pointsets;

    for (auto i = 0; i < AABBs.size(); i++) {
        int n = 2;
        RowMatrixXd points(n, 3);

        points(0, 0) = (double)AABBs[i].xmin();
        points(0, 1) = (double)AABBs[i].ymin();
        points(0, 2) = (double)AABBs[i].zmin();
        points(1, 0) = (double)AABBs[i].xmax();
        points(1, 1) = (double)AABBs[i].ymax();
        points(1, 2) = (double)AABBs[i].zmax();

        pointsets.push_back(points);
    }
    return pointsets;
}

std::vector<CGAL_Polyline> polylines_from_vertices_and_faces(const RowMatrixXd& V, const RowMatrixXi& F) {
    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> polylinePairs(F.size());
    CGAL_Polyline pline;

    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {
        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            polylinePairs[counter] = pline;
            pline.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }
    return polylinePairs;
}

void polylines_from_vertices_and_faces_and_properties(
    const RowMatrixXd& polylines_vertices_XYZ,
    const RowMatrixXi& polylines_vertices_count_int,
    const RowMatrixXd& face_vectors_XYZ,
    const RowMatrixXi& face_joints_types_int,
    const RowMatrixXi& three_valence_element_indices_and_instruction,
    const RowMatrixXi& adjacency,
    const RowMatrixXd& default_parameters_for_joint_types_matrix,
    const RowMatrixXd& scale,

    std::vector<CGAL_Polyline>& out_polyline_pairs,
    std::vector<std::vector<IK::Vector_3>>& out_insertion_vectors,
    std::vector<std::vector<int>>& out_joint_types,
    std::vector<std::vector<int>>& out_three_valence_element_indices_and_instruction,
    std::vector<int>& out_adjacency,
    std::vector<double>& out_default_parameters_for_joint_types,
    std::vector<double>& out_scale

) {
    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    //std::vector<CGAL_Polyline> out_polyline_pairs;
    out_polyline_pairs.reserve(polylines_vertices_count_int.size());

    //std::vector< std::vector<IK::Vector_3>> insertion_vectors;
    out_insertion_vectors.reserve(polylines_vertices_count_int.size());

    // std::vector< std::vector<int>> joint_types;
    out_joint_types.reserve(polylines_vertices_count_int.size());

    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = polylines_vertices_count_int(counter, 0);
    for (int i = 0; i < polylines_vertices_XYZ.size() / 3; i++) {
        CGAL::Epick::Point_3 p(polylines_vertices_XYZ(i, 0), polylines_vertices_XYZ(i, 1), polylines_vertices_XYZ(i, 2));
        pline.emplace_back(p);

        if (pline.size() == lastCount) {
            out_polyline_pairs.emplace_back(pline);
            pline.clear();                                          //Clear points from the polyline
            lastCount = polylines_vertices_count_int(++counter, 0); //Take next polyline Count
        }
    }

    if (face_vectors_XYZ.size() > 0) {
        //printf("999");
        std::vector<IK::Vector_3> vectors;
        counter = 0;
        int lastCount = polylines_vertices_count_int(counter, 0) + 1;
        for (int i = 0; i < face_vectors_XYZ.size() / 3; i++) {
            CGAL::Epick::Vector_3 v(face_vectors_XYZ(i, 0), face_vectors_XYZ(i, 1), face_vectors_XYZ(i, 2));
            vectors.emplace_back(v);

            if (vectors.size() == (lastCount)) {
                out_insertion_vectors.emplace_back(vectors);
                vectors.clear();                                            //Clear points from the polyline
                lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
            }
        }
    }

    if (face_joints_types_int.size() > 0) {
        //printf("888");
        std::vector<int> types;
        counter = 0;
        int lastCount = polylines_vertices_count_int(counter, 0) + 1;
        for (int i = 0; i < face_joints_types_int.size(); i++) {
            int id(face_joints_types_int(i, 0));
            types.emplace_back(id);

            if (types.size() == (lastCount)) {
                out_joint_types.emplace_back(types);
                types.clear();                                              //Clear points from the polyline
                lastCount = polylines_vertices_count_int(++counter, 0) + 1; //Take next polyline Count
            }
        }
    }

    if (three_valence_element_indices_and_instruction.size() > 0) {
        out_three_valence_element_indices_and_instruction.reserve(three_valence_element_indices_and_instruction.size());

        for (int i = 0; i < three_valence_element_indices_and_instruction.size(); i += 4) {
            std::vector<int> ids{
                three_valence_element_indices_and_instruction(i + 0, 0),
                three_valence_element_indices_and_instruction(i + 1, 0),
                three_valence_element_indices_and_instruction(i + 2, 0),
                three_valence_element_indices_and_instruction(i + 3, 0),
            };

            out_three_valence_element_indices_and_instruction.emplace_back(ids);
        }
    }

    if (adjacency.size() > 0) {
        out_adjacency.reserve(adjacency.size());
        for (int i = 0; i < adjacency.size(); i++)
            out_adjacency.emplace_back(adjacency(i, 0));
    }
    //for (auto& e : out_adjacency)
    //    CGAL_Debug(e);

    if (default_parameters_for_joint_types_matrix.size() > 0) {
        out_default_parameters_for_joint_types.reserve(default_parameters_for_joint_types_matrix.size());
        for (int i = 0; i < default_parameters_for_joint_types_matrix.size(); i++)
            out_default_parameters_for_joint_types.emplace_back(default_parameters_for_joint_types_matrix(i, 0));
    }

    if (scale.size() > 0) {
        out_scale.reserve(scale.size());
        for (int i = 0; i < scale.size(); i++)
            out_scale.emplace_back(scale(i, 0));
    }
}

std::vector<RowMatrixXd> result_from_polylinesVectorVector(std::vector<std::vector<CGAL_Polyline>> polylines) {
    std::vector<RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {
        for (auto j = i->begin(); j != i->end(); j++) {
            const CGAL_Polyline& poly = *j;
            int n = poly.size();
            RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].x();
                points(k, 1) = (double)poly[k].y();
                points(k, 2) = (double)poly[k].z();
            }

            pointsets.push_back(points);
        }
    }

    //printf("CPP number of points: %i ", pointsets.size());

    return pointsets;
}

std::vector<RowMatrixXd> result_from_polylinesVector(std::vector<CGAL_Polyline>& polylines, bool debug) {
    std::vector<RowMatrixXd> pointsets;

    for (auto i = polylines.begin(); i != polylines.end(); i++) {
        const CGAL_Polyline& poly = *i;
        int n = poly.size();
        RowMatrixXd points(n, 3);

        for (int k = 0; k < n; k++) {
            points(k, 0) = (double)poly[k].hx();
            points(k, 1) = (double)poly[k].hy();
            points(k, 2) = (double)poly[k].hz();
        }

        pointsets.push_back(points);
    }

    //  printf("CPP number of points: %i ", pointsets.size());
    /*
    if (debug)
    {
        std::ofstream myfile;
        myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");

        for (auto i = polylines.begin(); i != polylines.end(); i++)
        {
            const CGAL_Polyline &poly = *i;
            int n = poly.size();
             RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++)
            {
                points(k, 0) = (double)poly[k].hx();
                points(k, 1) = (double)poly[k].hy();
                points(k, 2) = (double)poly[k].hz();
            }

            for (int k = 0; k < n; k++)
                myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " "
                       << "\n";
            myfile << ("\n");
        }
        myfile.close();
    }
    */
    return pointsets;
}

std::tuple<std::vector<RowMatrixXd>, std::vector<int>> result_tuple_from_polylinesVector(std::vector<std::vector<CGAL_Polyline>>& polylines_vector, bool debug) {
    std::vector<RowMatrixXd> pointsets;
    std::vector<int> pointsets_groups_ids;

    int i = 0;
    for (auto& polylines : polylines_vector) {
        for (auto j = polylines.begin(); j != polylines.end(); j++) {
            const CGAL_Polyline& poly = *j;
            int n = poly.size();
            RowMatrixXd points(n, 3);

            for (int k = 0; k < n; k++) {
                points(k, 0) = (double)poly[k].hx();
                points(k, 1) = (double)poly[k].hy();
                points(k, 2) = (double)poly[k].hz();
            }

            pointsets.push_back(points);
            pointsets_groups_ids.push_back(i);
        }

        i++;
    }
    //  printf("CPP number of points: %i ", pointsets.size());
    /*
    if (debug) {
        std::ofstream myfile;
        myfile.open("C:\\IBOIS57\\_Code\\Software\\Python\\Pybind11Example\\vsstudio\\Release\\output.txt");
        for (auto& polylines : polylines_vector) {
            for (auto i = polylines.begin(); i != polylines.end(); i++) {
                const CGAL_Polyline& poly = *i;
                int n = poly.size();
                 RowMatrixXd points(n, 3);

                for (int k = 0; k < n; k++) {
                    points(k, 0) = (double)poly[k].hx();
                    points(k, 1) = (double)poly[k].hy();
                    points(k, 2) = (double)poly[k].hz();
                }

                for (int k = 0; k < n; k++)
                    myfile << poly[k].hx() << " " << poly[k].hy() << " " << poly[k].hz() << " " << "\n";
                myfile << ("\n");
            }
        }
        myfile.close();
    }
    */
    return std::make_tuple(pointsets, pointsets_groups_ids);
}

#pragma endregion

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Wrapped methods with inputs
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void pybind11_test() {
    printf("Hello from CPP Wood \n");
}

std::tuple<std::vector<RowMatrixXd>, std::vector<int>> pybind11_get_connection_zones(
    std::string& file_path,
    Eigen::Ref<const RowMatrixXd>& polylines_vertices_XYZ,
    Eigen::Ref<const RowMatrixXi>& polylines_vertices_count_int,
    Eigen::Ref<const RowMatrixXd>& face_vectors_XYZ,
    Eigen::Ref<const RowMatrixXi>& face_joints_types_int,
    Eigen::Ref<const RowMatrixXi>& three_valence_element_indices_and_instruction,
    Eigen::Ref<const RowMatrixXi>& adjacency,
    Eigen::Ref<const RowMatrixXd>& default_parameters_for_joint_types_matrix,
    Eigen::Ref<const RowMatrixXd>& scale,

    int search_type = 1,
    int output_type = 4,
    int triangulate = 0

) {
    //////////////////////////////////////////////////////////////////////////////
    //Set File Path for XML joints 9 19 29 39 49 59
    //////////////////////////////////////////////////////////////////////////////
    path_and_file_for_joints = file_path;

    //////////////////////////////////////////////////////////////////////////////
    //Convert Raw data to list of Polyline
    //////////////////////////////////////////////////////////////////////////////
    //std::vector<element> elements;
    //std::vector<CGAL_Polyline> polyline_pairs =  polylines_from_vertices_and_faces(polylines_vertices_XYZ, polylines_vertices_count_int);
    std::vector<CGAL_Polyline> out_polyline_pairs;
    std::vector<std::vector<IK::Vector_3>> out_insertion_vectors;
    std::vector<std::vector<int>> out_joint_types;
    std::vector<std::vector<int>> out_three_valence_element_indices_and_instruction;
    std::vector<int> out_adjacency;
    std::vector<double> out_default_parameters_for_joint_types;
    std::vector<double> out_scale;
    polylines_from_vertices_and_faces_and_properties(

        polylines_vertices_XYZ,
        polylines_vertices_count_int,
        face_vectors_XYZ,
        face_joints_types_int,
        three_valence_element_indices_and_instruction,
        adjacency,
        default_parameters_for_joint_types_matrix,
        scale,

        out_polyline_pairs,
        out_insertion_vectors,
        out_joint_types,
        out_three_valence_element_indices_and_instruction,
        out_adjacency,
        out_default_parameters_for_joint_types,
        out_scale);

    std::vector<std::vector<CGAL_Polyline>> output;
    std::vector<std::vector<int>> top_face_triangulation;


    std::vector<std::vector<char>> output_types;
    get_connection_zones(
        out_polyline_pairs,
        out_insertion_vectors,
        out_joint_types,
        out_three_valence_element_indices_and_instruction,
        out_adjacency,

        //output
        output,
        output_types,//new
        top_face_triangulation,

        //Global Parameters
        out_default_parameters_for_joint_types,
        out_scale,
        search_type,

        output_type,
        triangulate

    );//        division_distance,    shift,

        //////////////////////////////////////////////////////////////////////////////
        //Convert Output to Raw Data
        //////////////////////////////////////////////////////////////////////////////

    return result_tuple_from_polylinesVector(output, true);
}

std::tuple<RowMatrixXd, RowMatrixXi> pybind11_closed_mesh_from_polylines(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F) {
    std::vector<CGAL_Polyline> polylines = polylines_from_vertices_and_faces(V, F);
    return cgal_mesh_util::closed_mesh_from_polylines(polylines);
}

std::tuple<std::vector<RowMatrixXi>, RowMatrixXd, RowMatrixXd> pybind11_rtree(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F) //neihgbours per element, aabb, oobb
{
    //Do you need to delete the vector?
    //[0] [5, 6, 7]
    //[1] [4]
    //
    //////////////////////////////////////////////////////////////////////////////
    //Convert raw-data to list of Polylines
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    input_polyline_pairs.reserve(F.size());

    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {
        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            input_polyline_pairs.push_back(pline);
            pline.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }

    const int n = input_polyline_pairs.size() * 0.5;

    //////////////////////////////////////////////////////////////////////////////
    //Create elements, AABB, OBB, P, Pls, thickness
    //////////////////////////////////////////////////////////////////////////////
    std::vector<element> elements;
    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector<std::vector<int>> input_joint_types;
    get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);

    //////////////////////////////////////////////////////////////////////////////
    //Create joints, Perform Joint Area Search
    //////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////
    // Create RTree
    //////////////////////////////////////////////////////////////////////////////

    RTree<int, double, 3> tree;

    //////////////////////////////////////////////////////////////////////////////
    // Insert AABB
    //////////////////////////////////////////////////////////////////////////////

    for (int i = 0; i < elements.size(); i++) {
        double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
        double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
        tree.Insert(min, max, i);
    }

    //////////////////////////////////////////////////////////////////////////////
    // Search Closest Boxes | Skip duplicates pairs | Perform callback with OBB
    //////////////////////////////////////////////////////////////////////////////
    std::vector<RowMatrixXi> elements_neigbhours;
    elements_neigbhours.reserve(elements.size());

    for (int i = 0; i < elements.size(); i++) {
        //CGAL_Debug("_____");

        std::vector<int> element_neigbhours;

        //std::vector<int> result;
        auto callback = [&element_neigbhours, i, &elements](int foundValue) -> bool
        {
            if (cgal_box_util::GetCollision(elements[i].oob, elements[foundValue].oob)) {
                //if (i < foundValue && cgal_box_util::GetCollision(elements[i].oob, elements[foundValue].oob)) {
                //element_neigbhours.push_back(i);
                element_neigbhours.push_back(foundValue);
            }
            return true;
        };

        double min[3] = { elements[i].aabb.xmin(), elements[i].aabb.ymin(), elements[i].aabb.zmin() };
        double max[3] = { elements[i].aabb.xmax(), elements[i].aabb.ymax(), elements[i].aabb.zmax() };
        int nhits = tree.Search(min, max, callback); //callback in this method call callback above

        //elements_neigbhours.push_back(element_neigbhours);

        //Convert vector to matrix
        RowMatrixXi element_neigbhours_matrix(1, element_neigbhours.size());

        for (int j = 0; j < element_neigbhours.size(); j++) {
            //CGAL_Debug(element_neigbhours[j]);
            element_neigbhours_matrix(0, j) = element_neigbhours[j];
        }

        elements_neigbhours.push_back(element_neigbhours_matrix);
    }

    //////////////////////////////////////////////////////////////////////////////////
    ////// Display Rtree hierarchy
    //////////////////////////////////////////////////////////////////////////////////

    //auto list = tree.ListTree();
    //RowMatrixXd elements_AABB(list.size(), 24);

    //int count = 0;
    //for (auto aabb : list) {
    //    //IK::Point_3 p0(aabb.m_min[0], aabb.m_min[1], aabb.m_min[2]);
    //    //IK::Point_3 p1(aabb.m_max[0], aabb.m_max[1], aabb.m_max[2]);
    //    //std::vector<IK::Point_3> pts{ p0 ,p1};
    //    CGAL::Bbox_3 aabb(aabb.m_min[0], aabb.m_min[1], aabb.m_min[2], aabb.m_max[0], aabb.m_max[1], aabb.m_max[2]);
    //    //CGAL_Debug(aabb.m_min[0], aabb.m_min[1], aabb.m_min[2]);
    //    //CGAL_Debug(aabb.m_max[0], aabb.m_max[1], aabb.m_max[2]);

    //    elements_AABB(count, 0) = aabb.xmin();
    //    elements_AABB(count, 1) = aabb.ymin();
    //    elements_AABB(count, 2) = aabb.zmin();

    //    elements_AABB(count, 3) = aabb.xmin();
    //    elements_AABB(count, 4) = aabb.ymin();
    //    elements_AABB(count, 5) = aabb.zmax();

    //    elements_AABB(count, 6) = aabb.xmin();
    //    elements_AABB(count, 7) = aabb.ymax();
    //    elements_AABB(count, 8) = aabb.zmax();

    //    elements_AABB(count, 9) = aabb.xmin();
    //    elements_AABB(count, 10) = aabb.ymax();
    //    elements_AABB(count, 11) = aabb.zmin();

    //    elements_AABB(count, 12) = aabb.xmax();
    //    elements_AABB(count, 13) = aabb.ymin();
    //    elements_AABB(count, 14) = aabb.zmin();

    //    elements_AABB(count, 15) = aabb.xmax();
    //    elements_AABB(count, 16) = aabb.ymin();
    //    elements_AABB(count, 17) = aabb.zmax();

    //    elements_AABB(count, 18) = aabb.xmax();
    //    elements_AABB(count, 19) = aabb.ymax();
    //    elements_AABB(count, 20) = aabb.zmax();

    //    elements_AABB(count, 21) = aabb.xmax();
    //    elements_AABB(count, 22) = aabb.ymax();
    //    elements_AABB(count, 23) = aabb.zmin();

    //    count++;
    //}

    //////////////////////////////////////////////////////////////////////////////
    // Output AABB
    //////////////////////////////////////////////////////////////////////////////
    RowMatrixXd elements_AABB(elements.size(), 24);

    for (int i = 0; i < elements.size(); i++) {
        elements_AABB(i, 0) = elements[i].aabb.xmin();
        elements_AABB(i, 1) = elements[i].aabb.ymin();
        elements_AABB(i, 2) = elements[i].aabb.zmin();

        elements_AABB(i, 3) = elements[i].aabb.xmin();
        elements_AABB(i, 4) = elements[i].aabb.ymin();
        elements_AABB(i, 5) = elements[i].aabb.zmax();

        elements_AABB(i, 6) = elements[i].aabb.xmin();
        elements_AABB(i, 7) = elements[i].aabb.ymax();
        elements_AABB(i, 8) = elements[i].aabb.zmax();

        elements_AABB(i, 9) = elements[i].aabb.xmin();
        elements_AABB(i, 10) = elements[i].aabb.ymax();
        elements_AABB(i, 11) = elements[i].aabb.zmin();

        elements_AABB(i, 12) = elements[i].aabb.xmax();
        elements_AABB(i, 13) = elements[i].aabb.ymin();
        elements_AABB(i, 14) = elements[i].aabb.zmin();

        elements_AABB(i, 15) = elements[i].aabb.xmax();
        elements_AABB(i, 16) = elements[i].aabb.ymin();
        elements_AABB(i, 17) = elements[i].aabb.zmax();

        elements_AABB(i, 18) = elements[i].aabb.xmax();
        elements_AABB(i, 19) = elements[i].aabb.ymax();
        elements_AABB(i, 20) = elements[i].aabb.zmax();

        elements_AABB(i, 21) = elements[i].aabb.xmax();
        elements_AABB(i, 22) = elements[i].aabb.ymax();
        elements_AABB(i, 23) = elements[i].aabb.zmin();
    }

    ////////////////////////////////////////////////////////////////////////////////
    //// Output OOBB
    ////////////////////////////////////////////////////////////////////////////////

    RowMatrixXd elements_OOBB(elements.size(), 24);

    for (int i = 0; i < elements.size(); i++) {
        CGAL_Polyline corners;
        cgal_box_util::GetCorners(elements[i].oob, corners);

        for (int j = 0; j < 8; j++) {
            //int a = j * 3 + 0;
            //int b = j * 3 + 1;
            //int c = j * 3 + 2;
            //CGAL_Debug(j * 3 + 0);
            //CGAL_Debug(j * 3 + 1);
            //CGAL_Debug(j * 3 + 2);
            //CGAL_Debug(corners.size());
            elements_OOBB(i, j * 3 + 0) = corners[j].hx();
            elements_OOBB(i, j * 3 + 1) = corners[j].hy();
            elements_OOBB(i, j * 3 + 2) = corners[j].hz();
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    // Output Tuple
    //////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(elements_neigbhours, elements_AABB, elements_OOBB);
}

//element_pairs, joint areas and joint types
std::tuple<RowMatrixXi, std::vector<RowMatrixXd>, RowMatrixXi> pybind11_joints(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXi>& F, int& search_type) {
    //////////////////////////////////////////////////////////////////////////////
    //Convert raw-data to list of Polylines
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> input_polyline_pairs;
    input_polyline_pairs.reserve(F.size());

    CGAL_Polyline pline;
    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {
        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.push_back(p);

        if (pline.size() == lastCount) {
            input_polyline_pairs.push_back(pline);
            pline.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }

    const int n = input_polyline_pairs.size() * 0.5;

    //////////////////////////////////////////////////////////////////////////////
    //Create elements, AABB, OBB, P, Pls, thickness
    //////////////////////////////////////////////////////////////////////////////
    std::vector<element> elements;
    std::vector<std::vector<IK::Vector_3>> input_insertion_vectors;
    std::vector<std::vector<int>> input_joint_types;
    get_elements(input_polyline_pairs, input_insertion_vectors, input_joint_types, elements);

    //////////////////////////////////////////////////////////////////////////////
    //Create joints, Perform Joint Area Search
    //////////////////////////////////////////////////////////////////////////////

    auto joints = std::vector<joint>();
    auto joints_map = std::unordered_map<uint64_t, int>();
    std::vector<int> neighbors;
    adjacency_search(elements, search_type, neighbors, joints, joints_map);

    //////////////////////////////////////////////////////////////////////////////
    //Get element pairs, joint areas, join types
    //////////////////////////////////////////////////////////////////////////////
    RowMatrixXi element_pairs(joints.size(), 2);
    std::vector<RowMatrixXd> joint_areas;
    joint_areas.reserve(joints.size());
    RowMatrixXi joint_types(joints.size(), 1);

    for (int i = 0; i < joints.size(); i++) {
        //element pairs
        element_pairs(i, 0) = joints[i].v0;
        element_pairs(i, 1) = joints[i].v1;

        //CGAL_Debug(" ");
        //for (auto& pp : joints[i].joint_area)
        //    CGAL_Debug(pp.hx(), pp.hy(), pp.hz());

        //joint areas
        RowMatrixXd current_joint_areas(joints[i].joint_area.size(), 3);

        for (size_t j = 0; j < joints[i].joint_area.size(); j++) {
            current_joint_areas(j, 0) = joints[i].joint_area[j].hx();
            current_joint_areas(j, 1) = joints[i].joint_area[j].hy();
            current_joint_areas(j, 2) = joints[i].joint_area[j].hz();
        }
        joint_areas.emplace_back(current_joint_areas);

        //joint types
        joint_types(i, 0) = joints[i].type;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Output Tuple
    //////////////////////////////////////////////////////////////////////////////
    return std::make_tuple(element_pairs, joint_areas, joint_types);
}

std::tuple<RowMatrixXi, RowMatrixXd> pybind11_intersecting_sequences_of_dD_iso_oriented_boxes(Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXd>& E_R, Eigen::Ref<const RowMatrixXi>& F, double& min_distance) {
    //////////////////////////////////////////////////////////////////////////////
    //Convert raw-data to list of Polylines
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> polylines;
    polylines.reserve(F.size());
    CGAL_Polyline pline;

    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {
        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.emplace_back(p);

        if (pline.size() == lastCount) {
            polylines.emplace_back(pline);
            pline.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }

    std::vector<std::vector<double>> segment_radii;
    segment_radii.reserve(F.size());
    std::vector<double> one_pline_radii;

    counter = 0;
    lastCount = F(counter, 0) - 1;
    for (int i = 0; i < E_R.size(); i++) {
        one_pline_radii.emplace_back(E_R(i, 0));

        if (one_pline_radii.size() == lastCount) {
            segment_radii.emplace_back(one_pline_radii);
            one_pline_radii.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0) - 1; //Take next polyline Count
        }
    }

    //////////////////////////////////////////////////////////////////////////////
    //Perform search
    //////////////////////////////////////////////////////////////////////////////
    std::vector<std::array<int, 4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
    std::vector<std::array<IK::Point_3, 2>> point_pairs;
    cgal_box_search::intersecting_sequences_of_dD_iso_oriented_boxes(polylines, segment_radii, min_distance, polyline0_id_segment0_id_polyline1_id_segment1_id, point_pairs);

    //////////////////////////////////////////////////////////////////////////////
    //Convert to output
    //////////////////////////////////////////////////////////////////////////////
    RowMatrixXi neighbours(polyline0_id_segment0_id_polyline1_id_segment1_id.size(), 4);
    RowMatrixXd points(point_pairs.size(), 6);

    for (size_t i = 0; i < polyline0_id_segment0_id_polyline1_id_segment1_id.size(); i++) {
        neighbours(i, 0) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][0];
        neighbours(i, 1) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][1];
        neighbours(i, 2) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][2];
        neighbours(i, 3) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][3];
    }

    for (size_t i = 0; i < point_pairs.size(); i++) {
        points(i, 0) = point_pairs[i][0][0];
        points(i, 1) = point_pairs[i][0][1];
        points(i, 2) = point_pairs[i][0][2];
        points(i, 3) = point_pairs[i][1][0];
        points(i, 4) = point_pairs[i][1][1];
        points(i, 5) = point_pairs[i][1][2];
    }
    return std::make_tuple(neighbours, points);
}

std::tuple<RowMatrixXi, RowMatrixXd, std::vector<RowMatrixXd>, std::vector<RowMatrixXd>, RowMatrixXi, std::vector<RowMatrixXd> > pybind11_beam_volumes(std::string& file_path, Eigen::Ref<const RowMatrixXd>& V, Eigen::Ref<const RowMatrixXd>& E_R, Eigen::Ref<const RowMatrixXd>& E_N, Eigen::Ref<const RowMatrixXi>& F, Eigen::Ref<const RowMatrixXi>& F_T, double& min_distance, double& volume_length, double& cross_or_side_to_end, int& flip_male,
    Eigen::Ref <const RowMatrixXd>& input_default_parameters_for_joint_types, bool compute_joints, double division_distance, double shift, int output_type) {
    //////////////////////////////////////////////////////////////////////////////
    //Set directory for joint file path .XML
    //////////////////////////////////////////////////////////////////////////////
    path_and_file_for_joints = file_path;

    //default_parameters_for_joint_types, joints_oriented_polylines, true, 300, 0.6, 3
    //////////////////////////////////////////////////////////////////////////////
    //Convert raw-data to list of Polylines
    //////////////////////////////////////////////////////////////////////////////
    std::vector<CGAL_Polyline> polylines;
    polylines.reserve(F.size());
    CGAL_Polyline pline;

    int counter = 0;
    int lastCount = F(counter, 0);
    for (int i = 0; i < V.size() / 3; i++) {
        CGAL::Epick::Point_3 p(V(i, 0), V(i, 1), V(i, 2));
        pline.emplace_back(p);

        if (pline.size() == lastCount) {
            polylines.emplace_back(pline);
            pline.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0); //Take next polyline Count
        }
    }

    std::vector<std::vector<double>> segment_radii;
    segment_radii.reserve(F.size());
    std::vector<double> one_pline_radii;

    counter = 0;
    lastCount = F(counter, 0) - 1;
    for (int i = 0; i < E_R.size(); i++) {
        one_pline_radii.emplace_back(E_R(i, 0));

        if (one_pline_radii.size() == lastCount) {
            segment_radii.emplace_back(one_pline_radii);
            one_pline_radii.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0) - 1; //Take next polyline Count
        }
    }

    std::vector<std::vector<IK::Vector_3>> segment_normals;
    segment_normals.reserve(F.size());
    std::vector<IK::Vector_3> one_segment_normals;

    counter = 0;
    lastCount = F(counter, 0) - 1;
    for (int i = 0; i < E_N.size(); i++) {
        CGAL::Epick::Vector_3 n(E_N(i, 0), E_N(i, 1), E_N(i, 2));
        one_segment_normals.emplace_back(n);
        //CGAL_Debug(n);

        if (one_segment_normals.size() == lastCount) {
            segment_normals.emplace_back(one_segment_normals);
            one_segment_normals.clear();               //Clear points from the polyline
            lastCount = F(++counter, 0) - 1; //Take next polyline Count
        }
    }

    //CGAL_Debug(false);
    //for (size_t i = 0; i < segment_normals.size(); i++) {
    //    for (size_t j = 0; j < segment_normals[i].size(); j++) {
    //        CGAL_Debug(segment_normals[i][j]);
    //    }
    //}
    //CGAL_Debug(false);
    //for (size_t i = 0; i < polylines.size(); i++) {
    //    CGAL_Debug(false);
    //    for (size_t j = 0; j < polylines[i].size(); j++) {
    //        CGAL_Debug(polylines[i][j]);
    //    }
    //}

    std::vector<int> face_types;
    face_types.reserve(F_T.size());

    for (int i = 0; i < F_T.size(); i++) {
        face_types.emplace_back(F_T(i, 0));
        //CGAL_Debug(F_T(i, 0));
    }

    std::vector<double> default_parameters_for_joint_types;

    if (input_default_parameters_for_joint_types.size() > 0) {
        default_parameters_for_joint_types.reserve(input_default_parameters_for_joint_types.size());

        for (int i = 0; i < input_default_parameters_for_joint_types.size(); i++)
            default_parameters_for_joint_types.emplace_back(input_default_parameters_for_joint_types(i, 0));
    }

    //////////////////////////////////////////////////////////////////////////////
    //Perform search
    //////////////////////////////////////////////////////////////////////////////

    //joint volume and area
    std::vector<std::array<int, 4>> polyline0_id_segment0_id_polyline1_id_segment1_id;
    std::vector<std::array<IK::Point_3, 2>> point_pairs;
    std::vector<std::array<CGAL_Polyline, 4>> volume_pairs;
    std::vector<CGAL_Polyline> joints_areas;
    std::vector<int> joints_types;

    //joint selection
    std::vector<std::vector<CGAL_Polyline>> joints_oriented_polylines;
    beam_volumes(polylines, segment_radii, segment_normals, face_types, min_distance, volume_length, cross_or_side_to_end, flip_male,
        polyline0_id_segment0_id_polyline1_id_segment1_id, point_pairs, volume_pairs, joints_areas, joints_types,
        default_parameters_for_joint_types, joints_oriented_polylines, compute_joints, division_distance, shift, output_type);

    //////////////////////////////////////////////////////////////////////////////
    //Convert to output
    //////////////////////////////////////////////////////////////////////////////
    RowMatrixXi neighbours(polyline0_id_segment0_id_polyline1_id_segment1_id.size(), 4);
    RowMatrixXd points(point_pairs.size(), 6);
    std::vector<RowMatrixXd> volumes;
    std::vector<RowMatrixXd> output_joint_areas;
    volumes.reserve(point_pairs.size() * 4);
    output_joint_areas.reserve(joints_areas.size());
    RowMatrixXi output_joint_types(joints_types.size(), 1);

    for (size_t i = 0; i < polyline0_id_segment0_id_polyline1_id_segment1_id.size(); i++) {
        neighbours(i, 0) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][0];
        neighbours(i, 1) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][1];
        neighbours(i, 2) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][2];
        neighbours(i, 3) = polyline0_id_segment0_id_polyline1_id_segment1_id[i][3];
    }

    for (size_t i = 0; i < point_pairs.size(); i++) {
        points(i, 0) = point_pairs[i][0][0];
        points(i, 1) = point_pairs[i][0][1];
        points(i, 2) = point_pairs[i][0][2];
        points(i, 3) = point_pairs[i][1][0];
        points(i, 4) = point_pairs[i][1][1];
        points(i, 5) = point_pairs[i][1][2];

        for (size_t j = 0; j < 4; j++) {
            RowMatrixXd rect = result_from_polyline(volume_pairs[i][j]);
            volumes.emplace_back(rect);
        }
    }

    for (size_t i = 0; i < joints_areas.size(); i++) {
        RowMatrixXd rect = result_from_polyline(joints_areas[i]);
        output_joint_areas.push_back(rect);
    }

    for (size_t i = 0; i < joints_types.size(); i++) {
        output_joint_types(i, 0) = joints_types[i];
    }

    std::tuple<std::vector<RowMatrixXd>, std::vector<int>> joint_geometry_and_index = result_tuple_from_polylinesVector(joints_oriented_polylines, true);

    return std::make_tuple(neighbours, points, volumes, output_joint_areas, output_joint_types, std::get<0>(joint_geometry_and_index));
}

std::tuple < std::vector<RowMatrixXd>, std::vector<RowMatrixXd>, RowMatrixXi, RowMatrixXi> pybind11_check_joinery_library_xml(std::string& file_path, int type, double division_dist, double shift) {
    //#define DEBUG
    path_and_file_for_joints = file_path;
    joint empty_joint;
    empty_joint.get_divisions(division_dist);
    empty_joint.shift = shift;

    switch (type) {
    case(1):
        joint_library::ss_e_ip_0(empty_joint);
        break;
    case(2):
        joint_library::ss_e_ip_1(empty_joint);
        break;
    case(9):
        joint_library_xml_parser::read_xml(empty_joint, 0);
        break;

    case(10):
        joint_library::ss_e_op_0(empty_joint);
        break;
    case(11):
        joint_library::ss_e_op_1(empty_joint);
        break;
    case(12):
        joint_library::ss_e_op_2(empty_joint);
        break;
    case(19):
        joint_library_xml_parser::read_xml(empty_joint, 1);
        break;

    case(20):
        joint_library::ts_e_p_0(empty_joint);
        break;
    case(21):
        joint_library::ts_e_p_1(empty_joint);
        break;
    case(22):
        joint_library::ts_e_p_2(empty_joint);
        break;
    case(23):
        joint_library::ts_e_p_3(empty_joint);
        break;
    case(29):
        joint_library_xml_parser::read_xml(empty_joint, 2);
        break;

    case(30):
        joint_library::cr_c_ip_0(empty_joint);
        break;
    case(31):
        joint_library::cr_c_ip_1(empty_joint);
        break;
    case(39):
        joint_library_xml_parser::read_xml(empty_joint, 3);
        break;

    case(49):
        joint_library_xml_parser::read_xml(empty_joint, 4);
        break;

    case(59):
        joint_library_xml_parser::read_xml(empty_joint, 5);
        break;

    default:
        joint_library::ss_e_ip_0(empty_joint);
    }

    /* CGAL_Debug(empty_joint.m[0].size());
     for (auto& pline : empty_joint.m[0]) {
         printf("CPP Polyline\n");
         for (auto& p : pline)
             CGAL_Debug(p);
     }
     CGAL_Debug(empty_joint.m[1].size());
     for (auto& pline : empty_joint.m[1]) {
         printf("CPP Polyline\n");
         for (auto& p : pline)
             CGAL_Debug(p);
     }
     CGAL_Debug(empty_joint.f[0].size());
     for (auto& pline : empty_joint.f[0]) {
         printf("CPP Polyline\n");
         for (auto& p : pline)
             CGAL_Debug(p);
     }
     CGAL_Debug(empty_joint.f[1].size());
     for (auto& pline : empty_joint.f[1]) {
         printf("CPP Polyline\n");
         for (auto& p : pline)
             CGAL_Debug(p);
     }
     CGAL_Debug(empty_joint.m_boolean_type.size());
     for (auto& c : empty_joint.m_boolean_type) {
         printf("CPP char %c\n", c);
     }
     CGAL_Debug(empty_joint.f_boolean_type.size());
     for (auto& c : empty_joint.f_boolean_type) {
         printf("CPP char %c\n", c);
     }*/

    std::vector<CGAL_Polyline> m;
    m.reserve(empty_joint.m[0].size() * 2);
    std::vector<CGAL_Polyline> f;
    f.reserve(empty_joint.f[0].size() * 2);
    for (int i = 0; i < empty_joint.m[0].size(); i++) {
        m.emplace_back(empty_joint.m[0][i]);
        m.emplace_back(empty_joint.m[1][i]);
    }

    for (int i = 0; i < empty_joint.f[0].size(); i++) {
        f.emplace_back(empty_joint.f[0][i]);
        f.emplace_back(empty_joint.f[1][i]);
    }

    auto output0 = result_from_polylines(m);
    auto output1 = result_from_polylines(f);

#ifdef DEBUG
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Output Polylines");
    CGAL_Debug(m.size());
    CGAL_Debug(f.size());
#endif

    RowMatrixXi output2(1, empty_joint.m_boolean_type.size());
    for (int i = 0; i < empty_joint.m_boolean_type.size(); i++)
        output2(0, i) = empty_joint.m_boolean_type[i] - 48;

    RowMatrixXi output3(1, empty_joint.f_boolean_type.size());
    for (int i = 0; i < empty_joint.f_boolean_type.size(); i++)
        output3(0, i) = empty_joint.f_boolean_type[i] - 48;

#ifdef DEBUG
    printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "Output Boolean Types");
    CGAL_Debug(empty_joint.m_boolean_type.size());
    CGAL_Debug(empty_joint.f_boolean_type.size());
#endif

    return std::make_tuple(output0, output1, output2, output3);
}
