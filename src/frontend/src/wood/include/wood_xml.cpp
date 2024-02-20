
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "wood_xml.h"

namespace wood_xml
{
    std::string path_and_file_for_input_numbers = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_numbers.xml";
    std::string path_and_file_for_input_polylines = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_polylines.xml";
    std::string path_and_file_for_input_polylines_simple_case = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_polylines_simple_case.xml";
    std::string path_and_file_for_output_polylines = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\output_polylines.xml";
    std::string path_and_file_for_output_polylines_simple_case = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\output_polylines_simple_case.xml";

    bool file_exists_0(const std::string &name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    }

    bool read_xml_numbers(std::vector<std::vector<double>> &numbers)
    {
        std::string file_path = path_and_file_for_input_numbers; // user input
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_numbers";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        printf("%s",file_path.c_str());
        if (!file_exists_0(file_path))
        {
            printf("\nread_wood_xml -> File does not exist");
            return false;
        }
        else
        {
            printf("\read_nwood_xml -> File exists");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            for (boost::property_tree::ptree::value_type &v : tree.get_child(property_to_read))
            {
                if (v.first == "numbers")
                {
                    std::vector<double> numbers_list;
                    for (boost::property_tree::ptree::value_type &number : v.second)
                    {
                        // std::cout << number.second.get_value<double>();
                        // double x = point.second.get<double>("double");//if "double" is written inside then elements inside this wood::element will be retrieved
                        numbers_list.emplace_back(number.second.get_value<double>());
                    }
                    numbers.emplace_back(numbers_list);
                }
            }
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("read_wood_xml -> CPP Wrong property, probaby wrong path \n");
            return false;
        }
        return true;
    }

    bool read_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool &simple_case, const bool &remove_duplicates)
    {
        std::string file_path = simple_case ? path_and_file_for_input_polylines_simple_case : path_and_file_for_input_polylines;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_polylines"; // this is the main enclosing tag for the full data-set in the xml file

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        printf("read_wood_xml ->  read_xml_polylines -> ");
        printf("%s",file_path.c_str());
        printf("\n");
        if (!file_exists_0(file_path))
        {
            printf("read_wood_xml -> read_wood_xml|read_xml_polylines|File does not exist \n");
            return false;
        }
        else
        {
            printf("read_wood_xml -> read_xml_polylines|file exists \n");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            for (boost::property_tree::ptree::value_type &v : tree.get_child(property_to_read))
            {
                if (v.first == "Polyline")
                {
                    std::vector<IK::Point_3> polyline;
                    for (boost::property_tree::ptree::value_type &point : v.second)
                    {
                        double x = point.second.get<double>("x");
                        double y = point.second.get<double>("y");
                        double z = point.second.get<double>("z");
                        IK::Point_3 p(x, y, z);

                        // skip duplicate points
                        if (remove_duplicates)
                            if (polyline.size() > 0)
                                if (CGAL::squared_distance(polyline.back(), p) < wood_globals::DISTANCE_SQUARED)
                                    continue;

                        polyline.emplace_back(p);
                    }
                    polylines.emplace_back(polyline);
                }
            }
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("nread_wood_xml -> |read_xml_polylines|CPP Wrong property \n");
            return false;
        }
        return true;
    }

    bool read_xml_polylines(std::vector<std::vector<double>> &polylines, const bool &simple_case, const bool &remove_duplicates)
    {
        std::string file_path = simple_case ? path_and_file_for_input_polylines_simple_case : path_and_file_for_input_polylines;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_polylines"; // this is the main enclosing tag for the full data-set in the xml file

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        printf("read_wood_xml ->  read_xml_polylines -> ");
        printf("%s",file_path.c_str());
        printf("\n");
        if (!file_exists_0(file_path))
        {
            printf("read_wood_xml -> read_wood_xml|read_xml_polylines|File does not exist \n");
            return false;
        }
        else
        {
            printf("read_wood_xml -> read_xml_polylines|file exists \n");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            for (boost::property_tree::ptree::value_type &v : tree.get_child(property_to_read))
            {
                if (v.first == "Polyline")
                {
                    std::vector<double> polyline;
                    for (boost::property_tree::ptree::value_type &point : v.second)
                    {
                        double x = point.second.get<double>("x");
                        double y = point.second.get<double>("y");
                        double z = point.second.get<double>("z");

                        polyline.emplace_back(x);
                        polyline.emplace_back(y);
                        polyline.emplace_back(z);
                    }
                    polylines.emplace_back(polyline);
                }
            }
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("nread_wood_xml -> |read_xml_polylines|CPP Wrong property \n");
            return false;
        }
        return true;
    }

    bool read_xml_polylines_and_properties(
        std::vector<std::vector<IK::Point_3>> &input_polyline_pairs,
        std::vector<std::vector<IK::Vector_3>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,
        const bool &simple_case,
        const bool &remove_duplicates)
    {
        std::string file_path = simple_case ? path_and_file_for_input_polylines_simple_case : path_and_file_for_input_polylines;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_polylines"; // this is the main enclosing tag for the full data-set in the xml file

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        printf("read_wood_xml ->  read_xml_polylines -> ");
        printf("%s",file_path.c_str());
        printf("\n");
        if (!file_exists_0(file_path))
        {
            printf("read_wood_xml -> read_wood_xml|read_xml_polylines|File does not exist \n");
            return false;
        }
        else
        {
            printf("read_wood_xml -> read_xml_polylines|file exists \n");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            const static std::unordered_map<std::string, int> string_to_case{
                {"polyline", 0},
                {"insertion_vectors", 1},
                {"joints_types", 2},
                {"three_valence_element_indices_and_instruction", 3},
                {"adjacency", 4}};

            for (boost::property_tree::ptree::value_type &v : tree.get_child(property_to_read))
            {
                if (v.first == "polyline")
                {

                    // iterate through <point> array and add points to the polyline
                    std::vector<IK::Point_3> polyline;
                    for (boost::property_tree::ptree::value_type &point : v.second)
                    {
                        // convert get corrdinates of the points, by directly accessing <x>, <y>, <z> tags
                        double x = point.second.get<double>("x");
                        double y = point.second.get<double>("y");
                        double z = point.second.get<double>("z");
                        IK::Point_3 p(x, y, z);

                        // skip duplicate points
                        if (remove_duplicates && polyline.size() > 0)
                            if (CGAL::squared_distance(polyline.back(), p) < wood_globals::DISTANCE_SQUARED)
                                continue;

                        // add point to polyline
                        polyline.emplace_back(x, y, z);
                    }
                    input_polyline_pairs.emplace_back(polyline);
                }
                else if (v.first == "insertion_vectors")
                {
                    // iterate through <vector> array and add vectors to the insertion vector
                    std::vector<IK::Vector_3> vectors;
                    for (boost::property_tree::ptree::value_type &vector : v.second)
                    {
                        // convert get coordinates of the points, by directly accessing <x>, <y>, <z> tags
                        double x = vector.second.get<double>("x");
                        double y = vector.second.get<double>("y");
                        double z = vector.second.get<double>("z");

                        // add point to polyline
                        vectors.emplace_back(x, y, z);
                    }
                    input_insertion_vectors.emplace_back(vectors);
                }
                else if (v.first == "joints_types")
                {
                    // iterate through integers representing joint types
                    std::vector<int> joint_types;

                    // this is different from the previous because it is a flat list of integers with the same identifiers
                    // std::cout << "\n";
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {
                        // std::cout << id.second.get_value<int>() << "\n";
                        joint_types.emplace_back(id.second.get_value<int>());
                    }
                    input_JOINTS_TYPES.emplace_back(joint_types);
                }
                else if (v.first == "three_valence")
                {
                    // WARNING the three_valence must start with 0 or 1 depending whether it is Annen or Vidy corner case
                    // iterate through integers representing joint types
                    std::vector<int> three_valence;

                    // this is different from the previous because it is a flat list of integers with the same identifiers
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {

                        three_valence.emplace_back(id.second.get_value<int>());
                    }
                    input_three_valence_element_indices_and_instruction.emplace_back(three_valence);
                }
                else if (v.first == "adjacency")
                {
                    // flat list of pair integers
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {
                        input_adjacency.emplace_back(id.second.get_value<int>());
                    }
                }
            } // for
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("nread_wood_xml -> |read_xml_polylines|CPP Wrong property \n");
            return false;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // perform checks of insertion vectors and joint types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // check if the number of polylines is equal to the number of insertion vectors
        // polyline input is twice bigger because the list is flattened
        if (input_insertion_vectors.size() > 0)
            if (input_insertion_vectors.size() != input_polyline_pairs.size() * 0.5)
            {
                std::cout << "\n"
                          << input_polyline_pairs.size() * 0.5 << " " << input_insertion_vectors.size() << "\n";
                printf("nread_wood_xml -> |read_xml_polylines|CPP insertion vectors are given, but count of them is not equal to polyline count \n");
                return false;
            }
        // check if the number of polyliens is equal to the number of joints types
        if (input_JOINTS_TYPES.size() > 0)
            if (input_JOINTS_TYPES.size() != input_polyline_pairs.size() * 0.5)
            {
                printf("nread_wood_xml -> |read_xml_polylines|CPP joint types are given, but count of them is not equal to polyline count \n");
                std::cout << "\n"
                          << input_polyline_pairs.size() << " " << input_JOINTS_TYPES.size() << "\n";
                return false;
            }
        return true;
    }

    bool read_xml_polylines_and_properties(
        std::vector<std::vector<double>> &input_polyline_pairs,
        std::vector<std::vector<double>> &input_insertion_vectors,
        std::vector<std::vector<int>> &input_JOINTS_TYPES,
        std::vector<std::vector<int>> &input_three_valence_element_indices_and_instruction,
        std::vector<int> &input_adjacency,
        const bool &simple_case,
        const bool &remove_duplicates)
    {
        std::string file_path = simple_case ? path_and_file_for_input_polylines_simple_case : path_and_file_for_input_polylines;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_polylines"; // this is the main enclosing tag for the full data-set in the xml file

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        printf("read_wood_xml ->  read_xml_polylines -> ");
        printf("%s",file_path.c_str());
        printf("\n");
        if (!file_exists_0(file_path))
        {
            printf("read_wood_xml -> wood_xml|read_xml_polylines | File does not exist \n");
            return false;
        }
        else
        {
            printf("read_wood_xml -> read_xml_polylines | file exists \n");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        printf("start reading \n");
        try
        {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            const static std::unordered_map<std::string, int> string_to_case{
                {"polyline", 0},
                {"insertion_vectors", 1},
                {"joints_types", 2},
                {"three_valence_element_indices_and_instruction", 3},
                {"adjacency", 4}};


            for (boost::property_tree::ptree::value_type &v : tree.get_child(property_to_read))
            {
                //printf(v.first.c_str());
                if (v.first == "polyline" || v.first == "Polyline")
                {

                    // iterate through <point> array and add points to the polyline
                    std::vector<double> polyline;
                    for (boost::property_tree::ptree::value_type &point : v.second)
                    {
                        // convert get corrdinates of the points, by directly accessing <x>, <y>, <z> tags
                        double x = point.second.get<double>("x");
                        double y = point.second.get<double>("y");
                        double z = point.second.get<double>("z");
                        //printf("read_wood_xml -> read_xml_polylines | x: %f y: %f z: %f \n", x, y, z);
                        // IK::Point_3 p(x, y, z);

                        // skip duplicate points
                        // if (remove_duplicates && polyline.size() > 0)
                        //     if (CGAL::squared_distance(polyline.back(), p) < wood_globals::DISTANCE_SQUARED)
                        //         continue;

                        // add point to polyline
                        polyline.emplace_back(x);
                        polyline.emplace_back(y);
                        polyline.emplace_back(z);
                    }
                    input_polyline_pairs.emplace_back(polyline);
                }
                else if (v.first == "insertion_vectors")
                {
                    // iterate through <vector> array and add vectors to the insertion vector
                    std::vector<double> vectors;
                    for (boost::property_tree::ptree::value_type &vector : v.second)
                    {
                        // convert get coordinates of the points, by directly accessing <x>, <y>, <z> tags
                        double x = vector.second.get<double>("x");
                        double y = vector.second.get<double>("y");
                        double z = vector.second.get<double>("z");

                        // add point to polyline
                        // vectors.emplace_back(x, y, z);
                        vectors.emplace_back(x);
                        vectors.emplace_back(y);
                        vectors.emplace_back(z);
                    }
                    input_insertion_vectors.emplace_back(vectors);
                }
                else if (v.first == "joints_types")
                {
                    // iterate through integers representing joint types
                    std::vector<int> joint_types;

                    // this is different from the previous because it is a flat list of integers with the same identifiers
                    // std::cout << "\n";
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {
                        // std::cout << id.second.get_value<int>() << "\n";
                        joint_types.emplace_back(id.second.get_value<int>());
                    }
                    input_JOINTS_TYPES.emplace_back(joint_types);
                }
                else if (v.first == "three_valence")
                {
                    // WARNING the three_valence must start with 0 or 1 depending whether it is Annen or Vidy corner case
                    // iterate through integers representing joint types
                    std::vector<int> three_valence;

                    // this is different from the previous because it is a flat list of integers with the same identifiers
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {

                        three_valence.emplace_back(id.second.get_value<int>());
                    }
                    input_three_valence_element_indices_and_instruction.emplace_back(three_valence);
                }
                else if (v.first == "adjacency")
                {
                    // flat list of pair integers
                    for (boost::property_tree::ptree::value_type &id : v.second)
                    {
                        input_adjacency.emplace_back(id.second.get_value<int>());
                    }
                }
            } // for
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("nread_wood_xml -> | read_xml_polylines | CPP Wrong property \n");
            return false;
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // perform checks of insertion vectors and joint types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // check if the number of polylines is equal to the number of insertion vectors
        // polyline input is twice bigger because the list is flattened
        if (input_insertion_vectors.size() > 0)
            if (input_insertion_vectors.size() != input_polyline_pairs.size() * 0.5)
            {
                std::cout << "\n"
                          << input_polyline_pairs.size() * 0.5 << " " << input_insertion_vectors.size() << "\n";
                printf("nread_wood_xml -> |read_xml_polylines|CPP insertion vectors are given, but count of them is not equal to polyline count \n");
                return false;
            }
        // check if the number of polylines is equal to the number of joints types
        if (input_JOINTS_TYPES.size() > 0)
            if (input_JOINTS_TYPES.size() != input_polyline_pairs.size() * 0.5)
            {
                printf("nread_wood_xml -> |read_xml_polylines|CPP joint types are given, but count of them is not equal to polyline count \n");
                std::cout << "\n"
                          << input_polyline_pairs.size() << " " << input_JOINTS_TYPES.size() << "\n";
                return false;
            }
        return true;
    }

    bool write_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, const bool &simple_case)
    {

        std::string file_path = simple_case ? path_and_file_for_output_polylines_simple_case : path_and_file_for_output_polylines;
        std::string property_to_write = "output_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Set properties to XML from a polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;

            boost::property_tree::ptree main_node;
            for (auto &polyline : polylines)
            {

                boost::property_tree::ptree polyline_node;

                for (auto &point : polyline)
                {

                    // Add coordinates to point_node
                    boost::property_tree::ptree point_node;
                    point_node.put("x", point.x());
                    point_node.put("y", point.y());
                    point_node.put("z", point.z());

                    // Add point_node to polyline_node
                    polyline_node.push_back(std::make_pair("point", point_node));
                }
                main_node.push_back(std::make_pair("Polyline", polyline_node));
            }
            tree.add_child(property_to_write, main_node); // without this xml is invalid

            // Write property tree to XML file
            boost::property_tree::write_xml(file_path, tree);
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("write_wood_xml -> CPP Something went wrong, probaby wrong path \n");
            return false;
        }
        return true;
    }

    bool write_xml_polylines(std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree, int id)
    {
        std::string file_path = path_and_file_for_output_polylines;
        std::string property_to_write = "output_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Set properties to XML from a polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {
            boost::property_tree::ptree tree;

            boost::property_tree::ptree main_node;
            int count = 0;
            for (auto &polylines : polylines_tree)
            {

                if (id != -1)
                    if (count != id)
                        continue;
                boost::property_tree::ptree polyline_group_node;
                for (auto &polyline : polylines)
                {

                    boost::property_tree::ptree polyline_node;

                    for (auto &point : polyline)
                    {

                        // Add coordinates to point_node
                        boost::property_tree::ptree point_node;
                        point_node.put("x", point.x());
                        point_node.put("y", point.y());
                        point_node.put("z", point.z());

                        // Add point_node to polyline_node
                        polyline_node.push_back(std::make_pair("point", point_node));
                    }
                    // Add to polyline group
                    polyline_group_node.push_back(std::make_pair("Polyline", polyline_node));
                }
                main_node.push_back(std::make_pair("Polyline_Group", polyline_group_node));
                count++;
            }
            tree.add_child(property_to_write, main_node); // without this xml is invalid

            // Write property tree to XML file
            boost::property_tree::write_xml(file_path, tree);
        }

        catch (std::exception &e)
        {
            (void)e;
            printf("write_wood_xml -> CPP Something went wrong, probaby wrong path \n");
            return false;
        }
        return true;
    }

    bool write_xml_polylines_and_types(std::vector<std::vector<std::vector<IK::Point_3>>> &polylines_tree, std::vector<std::vector<wood_cut::cut_type>> &types_tree, int id, bool simple_case)
    {

        std::string property_to_write = "output_polylines";
        std::string file_path = simple_case ? path_and_file_for_output_polylines_simple_case : path_and_file_for_output_polylines;
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Set properties to XML from a polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try
        {

            ////////////////////////////////////////////////////////////////////////////////
            // start
            ////////////////////////////////////////////////////////////////////////////////

            boost::property_tree::ptree tree;
            boost::property_tree::ptree main_node;
            int count = 0;

            ////////////////////////////////////////////////////////////////////////////////
            // Polylines
            ////////////////////////////////////////////////////////////////////////////////
            for (auto &polylines : polylines_tree)
            {

                if (id != -1)
                    if (count != id)
                        continue;

                boost::property_tree::ptree polyline_group_node;
                for (auto &polyline : polylines)
                {

                    boost::property_tree::ptree polyline_node;

                    for (auto &point : polyline)
                    {

                        // Add coordinates to point_node
                        boost::property_tree::ptree point_node;
                        point_node.put("x", point.x());
                        point_node.put("y", point.y());
                        point_node.put("z", point.z());

                        // Add point_node to polyline_node
                        polyline_node.add_child("point", point_node);

                        // polyline_node.push_back(std::make_pair("point", point_node));
                    }
                    // Add to polyline group
                    polyline_group_node.add_child("polyline", polyline_node);
                }
                main_node.add_child("polyline_group", polyline_group_node);
                count++;

                // break;
            }

            ////////////////////////////////////////////////////////////////////////////////
            // types
            ////////////////////////////////////////////////////////////////////////////////

            count = 0;
            // printf("\n Loop of types ");
            for (auto &types : types_tree)
            {

                if (id != -1)
                    if (count != id)
                        continue;

                boost::property_tree::ptree type_group;
                for (wood_cut::cut_type &type : types)
                {
                    type_group.add("type", wood_cut::cut_type_to_string[type]);
                }

                main_node.add_child("type_group", type_group);
                count++;
                // break;
            }

            ////////////////////////////////////////////////////////////////////////////////
            // end
            ////////////////////////////////////////////////////////////////////////////////

            // add to the main code
            // tree.add(property_to_write, main_node);//without this xml is invalid

            // without this xml is invalid
            tree.add_child(property_to_write, main_node);

            // Write property tree to XML file
            boost::property_tree::write_xml(file_path, tree);
        }

        catch (std::exception &e)
        {
            (void)e;
            printf("write_wood_xml -> CPP Something went wrong, probaby wrong path \n");
            return false;
        }
        return true;
    }
}
