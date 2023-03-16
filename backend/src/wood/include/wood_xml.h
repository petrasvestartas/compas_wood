#ifndef WOOD_XML_H
#define WOOD_XML_H

#include "wood_cut.h"



namespace wood_xml
{
    // #include "../../stdafx.h"
    // #include "wood_main.h"
    extern std::string path_and_file_for_input_numbers;
    extern std::string path_and_file_for_input_polylines;
    extern std::string path_and_file_for_input_polylines_simple_case;
    extern std::string path_and_file_for_output_polylines;
    extern std::string path_and_file_for_output_polylines_simple_case;

    bool file_exists_0(const std::string &name);

    bool read_xml_numbers(std::vector<std::vector<double>> &numbers);

    /**
     * xml reader - <?xml version="1.0" encoding="UTF-8"?>
     * extract polylines for wood algorithm
     *
     * @param [out] polylines pair of polyline
     * @param [in] simple_case choose between two file paths "path_and_file_for_input_polylines" and "path_and_file_for_input_polylines_simple_case"
     * @param [in] remove_duplicates remove duplicate points in polylines
     * @return true if the file reading was successful
     */
    bool read_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool &simple_case = false, const bool &remove_duplicates = false);

    /**
     * xml reader - <?xml version="1.0" encoding="UTF-8"?>
     * extract polylines for wood algorithm
     *
     * @param [out] polylines pair of polyline coordinates
     * @param [in] simple_case choose between two file paths "path_and_file_for_input_polylines" and "path_and_file_for_input_polylines_simple_case"
     * @param [in] remove_duplicates remove duplicate points in polylines
     * @return true if the file reading was successful
     */
    bool read_xml_polylines(std::vector<std::vector<double>> &polylines, const bool &simple_case, const bool &remove_duplicates);

    /**
     * xml reader - <?xml version="1.0" encoding="UTF-8"?>
     * extract polylines for wood algorithm
     * and properties: insertion_vectors, joints_per_face, three_valence, adjacency
     *
     * @param [out] input_polyline_pairs pair of polyline
     * @param [out] input_insertion_vectors insertion vectors per face, e.g. a box has 6 faces, 2 top and bottom, 4 sides
     * @param [out] input_JOINTS_TYPES joint types per face e.g. {{0,0,1,1,1,1}{0,0,1,1,1,1}}
     * @param [out] input_three_valence_element_indices_and_instruction three valence element indices e.g. {{0},{0,7,1,7}}, for corner cases like 0 - Annen or 1 - Vidy, be aware that this vector starts by a list consisting of one index point to a correct corner case
     * @param [out] input_adjacency a list of consisting of element pairs, e.g. id00, id01, id10, id11 ...
     * @param [in] simple_case choose between two file paths "path_and_file_for_input_polylines" and "path_and_file_for_input_polylines_simple_case"
     * @param [in] remove_duplicates remove duplicate points in polylines
     * @return true if the file reading was successful
     */
    bool read_xml_polylines_and_properties(
        std::vector<std::vector<IK::Point_3>> &input_polyline_pairs,
        std::vector<std::vector<IK::Vector_3>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,
        const bool &simple_case = false,
        const bool &remove_duplicates = false);

    /**
     * xml reader - <?xml version="1.0" encoding="UTF-8"?>
     * extract polylines for wood algorithm
     * and properties: insertion_vectors, joints_per_face, three_valence, adjacency
     *
     * @param [out] input_polyline_pairs pair of polyline
     * @param [out] input_insertion_vectors insertion vectors per face, e.g. a box has 6 faces, 2 top and bottom, 4 sides
     * @param [out] input_JOINTS_TYPES joint types per face e.g. {{0,0,1,1,1,1}{0,0,1,1,1,1}}
     * @param [out] input_three_valence_element_indices_and_instruction three valence element indices e.g. {{0},{0,7,1,7}}, for corner cases like 0 - Annen or 1 - Vidy, be aware that this vector starts by a list consisting of one index point to a correct corner case
     * @param [out] input_adjacency a list of consisting of element pairs, e.g. id00, id01, id10, id11 ...
     * @param [in] simple_case choose between two file paths "path_and_file_for_input_polylines" and "path_and_file_for_input_polylines_simple_case"
     * @param [in] remove_duplicates remove duplicate points in polylines
     * @return true if the file reading was successful
     */
    bool read_xml_polylines_and_properties(
        std::vector<std::vector<double>> &input_polyline_pairs,
        std::vector<std::vector<double>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,
        const bool &simple_case = false,
        const bool &remove_duplicates = false);

    bool write_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool &simple_case = false);

    bool write_xml_polylines(std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree, int id = -1);

    /**
     * xml writer - <?xml version="1.0" encoding="UTF-8"?>
     * extract polylines for wood algorithm
     * and properties: insertion_vectors, joints_per_face, three_valence, adjacency
     *
     * @param [in] polylines_tree list of polylines lists, where the lists contains the following order: hole00, hole01, hole10, hole11 ... outline0, outline1
     * @param [in] types_tree list of cut types per joint, if given, else this output is empty
     * @param [in] id joint types per face e.g. {{0,0,1,1,1,1}{0,0,1,1,1,1}}
     * @param [in] simple_case choose between two file paths "path_and_file_for_input_polylines" and "path_and_file_for_input_polylines_simple_case"
     * @return true if the file reading was successful
     */
    bool write_xml_polylines_and_types(
        std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree,
        std::vector<std::vector<wood_cut::cut_type>> &types_tree,
        int id = -1,
        bool simple_case = false);
}

#endif
