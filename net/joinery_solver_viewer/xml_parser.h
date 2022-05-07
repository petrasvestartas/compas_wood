#pragma once

#include "stdafx.h"
#include "joinery_solver_main.h"

static std::string path_and_file_for_input_polylines = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\input_polylines.xml";
static std::string path_and_file_for_output_polylines = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\net\\data\\output_polylines.xml";

namespace xml_parser {
    inline bool file_exists_0(const  std::string& name  ) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    //inline bool file_exists_1(const std::string& name) {
    //    struct stat buffer;
    //    return (stat(name.c_str(), &buffer) == 0);
    //}

    inline bool read_xml_polylines(std::vector<std::vector<IK::Point_3>>& polylines) {
        std::string file_path = path_and_file_for_input_polylines;
        std::string property_to_read = "input_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
         //Check if XML file exists, path_and_file_for_joints is a global path
         /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        printf(file_path.c_str());
        if (!file_exists_0(file_path)) {
            printf("\n File does not exist");
            return false;
        }
        else {
            printf("\n File exists");
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Get properties from XML and create polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try {
            boost::property_tree::ptree tree;
            boost::property_tree::xml_parser::read_xml(file_path, tree);

            for (boost::property_tree::ptree::value_type& v : tree.get_child(property_to_read)) {
                if (v.first == "Polyline") {
                    std::vector<IK::Point_3> polyline;
                    for (boost::property_tree::ptree::value_type& point : v.second) {
                        double x = point.second.get<double>("x");
                        double y = point.second.get<double>("y");
                        double z = point.second.get<double>("z") ;
                        polyline.emplace_back(x, y, z);
                    }
                    polylines.emplace_back(polyline);
                }
            }
        }
        catch (std::exception& e) {
            (void)e;
            printf("\nCPP Wrong property ");
            return false;
        }
        return true;
    }


    inline bool write_xml_polylines(std::vector<std::vector<IK::Point_3>>& polylines) {
        std::string  file_path = path_and_file_for_output_polylines ;
        std::string property_to_write = "output_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Set properties to XML from a polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try {
            boost::property_tree::ptree tree;


            boost::property_tree::ptree main_node;
            for (auto& polyline : polylines) {
                
                boost::property_tree::ptree polyline_node;
                
                for (auto& point : polyline) {
                    
                    //Add coordinates to point_node
                    boost::property_tree::ptree point_node;
                    point_node.put("x", point.x());
                    point_node.put("y", point.y());
                    point_node.put("z", point.z());
                    
                    //Add point_node to polyline_node
                    polyline_node.push_back(std::make_pair("point", point_node));
                }
                main_node.push_back(std::make_pair("Polyline", polyline_node));

            }
            tree.add_child(property_to_write, main_node);//without this xml is invalid
            
            // Write property tree to XML file
            boost::property_tree::write_xml(file_path, tree);

        }
        catch (std::exception& e) {
            (void)e;
            printf("\nCPP Something went wrong ");
            return false;
        }
        return true;
    }


    inline bool write_xml_polylines(std::vector<std::vector<std::vector<IK::Point_3>>>& polylines_tree) {
        std::string  file_path = path_and_file_for_output_polylines;
        std::string property_to_write = "output_polylines";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Set properties to XML from a polylines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try {
            boost::property_tree::ptree tree;


            boost::property_tree::ptree main_node;
            for (auto& polylines : polylines_tree) {

                boost::property_tree::ptree polyline_group_node;
                for (auto& polyline : polylines) {

                    boost::property_tree::ptree polyline_node;

                    for (auto& point : polyline) {

                        //Add coordinates to point_node
                        boost::property_tree::ptree point_node;
                        point_node.put("x", point.x());
                        point_node.put("y", point.y());
                        point_node.put("z", point.z());

                        //Add point_node to polyline_node
                        polyline_node.push_back(std::make_pair("point", point_node));
                    }
                    //Add to polyline group
                    polyline_group_node.push_back(std::make_pair("Polyline", polyline_node));

                }
                main_node.push_back(std::make_pair("Polyline_Group", polyline_group_node));
            }
            tree.add_child(property_to_write, main_node);//without this xml is invalid

            // Write property tree to XML file
            boost::property_tree::write_xml(file_path, tree);

        }
        catch (std::exception& e) {
            (void)e;
            printf("\nCPP Something went wrong ");
            return false;
        }
        return true;
    }

}
