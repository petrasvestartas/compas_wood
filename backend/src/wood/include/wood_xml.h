#include "wood_cut.h"

#ifndef WOOD_XML_H
#define WOOD_XML_H

namespace wood_xml
{
//#include "../../stdafx.h"
//#include "wood_main.h"
extern std::string path_and_file_for_input_numbers ;
extern std::string path_and_file_for_input_polylines ;
extern std::string path_and_file_for_input_polylines_simple_case ;
extern std::string path_and_file_for_output_polylines ;
extern std::string path_and_file_for_output_polylines_simple_case ;


    bool file_exists_0(const std::string &name);

    bool read_xml_numbers(std::vector<std::vector<double>> &numbers);

    bool read_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool& simple_case = false, bool remove_duplicates = false);

    bool write_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool&  simple_case = false);

    bool write_xml_polylines(std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree, int id = -1);

    bool write_xml_polylines_and_types(std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree, std::vector<std::vector<wood_cut::cut_type>> &types_tree, int id = -1, bool simple_case = false);

}

#endif
