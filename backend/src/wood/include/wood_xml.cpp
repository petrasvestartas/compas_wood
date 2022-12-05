#include "../../../stdafx.h"
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
        std::string file_path = path_and_file_for_input_numbers;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_numbers";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        printf(file_path.c_str());
        if (!file_exists_0(file_path))
        {
            printf("\nwood_xml -> File does not exist");
            return false;
        }
        else
        {
            printf("\nwood_xml -> File exists");
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
            printf("\nwood_xml -> CPP Wrong property, probaby wrong path ");
            return false;
        }
        return true;
    }

    bool read_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, bool simple_case)
    {
        std::string file_path = simple_case ? path_and_file_for_input_polylines_simple_case : path_and_file_for_input_polylines;
        // printf( " \n %s  \n", file_path.c_str());
        std::string property_to_read = "input_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        printf("\nwood_xml ->  read_xml_polylines -> ");
        printf(file_path.c_str());
        if (!file_exists_0(file_path))
        {
            printf("\nwood_xml -> wood_xml|read_xml_polylines|File does not exist");
            return false;
        }
        else
        {
            printf("\nwood_xml -> read_xml_polylines|file exists");
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
                        polyline.emplace_back(x, y, z);
                    }
                    polylines.emplace_back(polyline);
                }
            }
        }
        catch (std::exception &e)
        {
            (void)e;
            printf("\nnwood_xml -> |read_xml_polylines|CPP Wrong property ");
            return false;
        }
        return true;
    }

    bool write_xml_polylines(std::vector<std::vector<IK::Point_3>> &polylines, bool simple_case)
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
            printf("\nwood_xml -> CPP Something went wrong, probaby wrong path ");
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
            printf("\nwood_xml -> CPP Something went wrong, probaby wrong path ");
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
            printf("\nwood_xml -> CPP Something went wrong, probaby wrong path ");
            return false;
        }
        return true;
    }

}
