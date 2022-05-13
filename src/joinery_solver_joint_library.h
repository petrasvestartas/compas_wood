#pragma once
#include "stdafx.h"
#include "joinery_solver_joint.h"



namespace joint_library_xml_parser {
    inline bool exists_test0(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    //inline bool exists_test3(const std::string& name) {
      //  struct stat buffer;
     //   return (stat(name.c_str(), &buffer) == 0);
    //}

    //https://zipproth.de/cheat-sheets/cpp-boost/
    inline bool read_xml(joint& joint, int type = 0) {
        std::string name = "";//custom

        switch (type) {
        case(12):
        case(0):
            name = "ss_e_ip_9";
            break;
        case(11):
        case(1):
            name = "ss_e_op_9";
            break;
        case(20):
        case(2):
            name = "ts_e_p_9";
            break;
        case(30):
        case(3):
            name = "cr_c_ip_9";
            break;
        case(40):
        case(4):
            name = "tt_e_p_9";
            break;
        case(13):
        case(5):
            name = "ss_e_r_9";
            break;
        case(60):
        case(6):
            name = "b_9";
            break;            
        }

        joint.name = name;
        //printf("XML \n");
        //printf(type);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP File path %s ", path_and_file_for_joints.c_str());
        printf("\nCPP Joint Type %i %s ", type, name.c_str());
#endif
        if (!exists_test0(path_and_file_for_joints)) {
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP File does not exist %s ", path_and_file_for_joints.c_str());
#endif

            std::ofstream myfile;
            myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
            myfile << "Do not Exist\n";
            myfile << path_and_file_for_joints;
            myfile.close();

            return false;
        }
        else {
            //std::ofstream myfile;
            //myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");
            //myfile << "Exists\n";
            //myfile << path_and_file_for_joints;
            //myfile.close();
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Read XML
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        boost::property_tree::ptree tree;
        boost::property_tree::xml_parser::read_xml(path_and_file_for_joints, tree);

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP tree size %zi ", tree.size());
#endif

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Get Property to find the right joint parameters and keys of XML file properties
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Traverse property tree example
        std::string xml_joint_name = "custom_joints." + name;
        std::array<std::string, 7> keys = { "m0", "m1","f0","f1", "m_boolean_type","f_boolean_type","properties" };
        std::array<int, 6> array_id = { 0,1,0,1,0,1 };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Get properties from XML and add to joint
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        try {
            for (boost::property_tree::ptree::value_type& v : tree.get_child(xml_joint_name)) {
                //printf("\nCPP %s", v.first.c_str());
#ifdef DEBUG_JOINERY_LIBRARY
                printf("\nCPP %s", v.first.c_str());
#endif
                for (int i = 0; i < 4; i++) {
                    if (v.first == keys[i]) {
                        //std::cout << v.first << "\n";

                        CGAL_Polyline polyline;
                        for (boost::property_tree::ptree::value_type& point : v.second) {
                            double x = point.second.get<double>("x");
                            double y = point.second.get<double>("y");
                            double z = point.second.get<double>("z");
                            polyline.emplace_back(x, y, z);

#ifdef DEBUG_JOINERY_LIBRARY
                            printf("\nCPP Vector %.17g %.17g %.17g", x, y, z);
#endif
                        }

                        //printf("\n");
                        //CGAL_Debug(polyline.size());

                        //Assign to array
                        switch (i) {
                        case(0):
                            joint.m[0].emplace_back(polyline);
                            //printf("\nCPP joint.m[0].emplace_back(polyline)");
                            break;

                        case(1):
                            joint.m[1].emplace_back(polyline);
                            //printf("\nCPP joint.m[1].emplace_back(polyline)");
                            break;

                        case(2):
                            joint.f[0].emplace_back(polyline);
                            //printf("\nCPP joint.f[0].emplace_back(polyline)");
                            break;

                        case(3):
                            joint.f[1].emplace_back(polyline);
                            //printf("\nCPP joint.f[1].emplace_back(polyline)");
                            break;
                        }
                    }
                }

                for (int i = 4; i < 6; i++) {
                    std::vector<char> boolean_type;
                    if (v.first == keys[i]) {
                        //std::cout << v.first << "\n";

                        for (boost::property_tree::ptree::value_type& index : v.second) {
                            char id = index.second.get_value<char>();

#ifdef DEBUG_JOINERY_LIBRARY
                            printf("\nCPP id %c ", id);
#endif
                            if (i == 4) {
                                joint.m_boolean_type.emplace_back(id);
                                //emplace to female joint.m_boolean_type
                            }
                            else {
                                joint.f_boolean_type.emplace_back(id);
                                //emplace to female joint.f_boolean_type
                            }
                        }

                        //Assign to array
                    }
                }

                if (v.first == keys[6]) {
                    std::vector<double> properties;
                    for (boost::property_tree::ptree::value_type& index : v.second) {
                        double parameter = index.second.get_value<double>();

#ifdef DEBUG_JOINERY_LIBRARY
                        printf("\nCPP id %f ", parameter);
#endif
                        properties.emplace_back(parameter);
                    }

                    if (properties.size() > 0) {
                        joint.unit_scale = properties[0] > 0;
                    }

                    //Assign to array
                }
            }
        }
        catch (std::exception& e) {
            (void)e;
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP Wrong property ");

#endif
            std::ofstream myfile;
            myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
            myfile << "Wrong Property\n";
            myfile.close();
            //printf("Wrong Property \n");
            return false;
        }
        //std::ofstream myfile;
        //myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");
        //myfile << "Good Result\n";
        //myfile.close();
        //CGAL_Debug(joint.f[0].size(), joint.f[1].size(), joint.f_boolean_type.size());
        //CGAL_Debug(joint.m[0].size(), joint.m[1].size(), joint.m_boolean_type.size());
        //printf("Exists \n");

        return true;
    }
}

namespace joint_library {
    inline CGAL::Aff_transformation_3<IK> to_plane(IK::Vector_3 X1, IK::Vector_3 Y1, IK::Vector_3 Z1) {
        CGAL::Aff_transformation_3<IK> F1(
            X1.x(), Y1.x(), Z1.x(),
            X1.y(), Y1.y(), Z1.y(),
            X1.z(), Y1.z(), Z1.z());
        return F1;
    }

    inline double get_length(double x, double y, double z) {
        //double ON_DBL_MIN = 2.22507385850720200e-308;

        double len;
        x = fabs(x);
        y = fabs(y);
        z = fabs(z);
        if (y >= x && y >= z) {
            len = x;
            x = y;
            y = len;
        }
        else if (z >= x && z >= y) {
            len = x;
            x = z;
            z = len;
        }

        // 15 September 2003 Dale Lear
        //     For small denormalized doubles (positive but smaller
        //     than DBL_MIN), some compilers/FPUs set 1.0/x to +INF.
        //     Without the ON_DBL_MIN test we end up with
        //     microscopic vectors that have infinte length!
        //
        //     This code is absolutely necessary.  It is a critical
        //     part of the bug fix for RR 11217.
        if (x > ON_DBL_MIN) {
            y /= x;
            z /= x;
            len = x * sqrt(1.0 + y * y + z * z);
        }
        else if (x > 0.0 && ON_IS_FINITE(x))
            len = x;
        else
            len = 0.0;

        return len;
    }

    inline bool unitize(IK::Vector_3& v) {
        bool rc = false;
        // Since x,y,z are floats, d will not be denormalized and the
        // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

        double d = get_length(v.x(), v.y(), v.z());
        if (d > 0.0) {
            double dx = v.x();
            double dy = v.y();
            double dz = v.z();
            v = IK::Vector_3(
                (dx / d),
                (dy / d),
                (dz / d));
            rc = true;
        }
        return rc;
    }

    inline double RemapNumbers(const double& speed = 50, const double& low1 = 0, const double& high1 = 2000, const double& low2 = 0, const double& high2 = 1) {
        return low2 + (speed - low1) * (high2 - low2) / (high1 - low1);
    }

    inline double Lerp(const double& value1, const double& value2, const double& amount) {
        return value1 + (value2 - value1) * amount;
    }

    inline void interpolate_points(const IK::Point_3& from, const IK::Point_3& to, const int Steps, const bool includeEnds, std::vector<IK::Point_3>& interpolated_points) {
        if (includeEnds) {
            interpolated_points.reserve(Steps + 2);
            interpolated_points.emplace_back(from);

            for (int i = 1; i < Steps + 1; i++) {
                double num = i / (double)(1 + Steps);
                interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
            }

            interpolated_points.emplace_back(to);
        }
        else {
            interpolated_points.reserve(Steps);

            for (int i = 1; i < Steps + 1; i++) {
                double num = i / (double)(1 + Steps);
                interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
            }
        }
    }

    //type_typeidedge_subtypeieinplane_id
    //0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

    //custom

 

    
    inline void side_removal(joint& jo, std::vector<element>& elements, bool merge_with_joint = false) {
        jo.name = "side_removal";
        jo.orient = false;

        //printf("Side_Removal");

        /////////////////////////////////////////////////////////////////////////////////
        //offset vector
        /////////////////////////////////////////////////////////////////////////////////
        IK::Vector_3 f0_0_normal = elements[jo.v0].planes[jo.f0_0].orthogonal_vector();
        cgal_vector_util::Unitize(f0_0_normal);
        //v0 *= (jo.scale[2] + jo.shift);
        f0_0_normal *= jo.scale[2];

        IK::Vector_3 f1_0_normal = elements[jo.v1].planes[jo.f1_0].orthogonal_vector();
        cgal_vector_util::Unitize(f1_0_normal);
        f1_0_normal *= jo.scale[2];

        /////////////////////////////////////////////////////////////////////////////////
        //copy side rectangles
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0 = elements[jo.v0].polylines[jo.f0_0];
        CGAL_Polyline pline1 = elements[jo.v1].polylines[jo.f1_0];
       

        /////////////////////////////////////////////////////////////////////////////////
        //extend only convex angles and side polygons | only rectangles
        /////////////////////////////////////////////////////////////////////////////////
        //CGAL_Debug(pline0.size(), pline1.size());
        //CGAL_Debug(joint.scale[0]);
        if (pline0.size() == 5 && pline1.size() == 5) {
            //get convex_concave corners
            std::vector<bool>convex_corner0;

            cgal_polyline_util::convex_corner(elements[jo.v0].polylines[0], convex_corner0);

            int id = 15;

            double scale0_0 = convex_corner0[jo.f0_0 - 2] ? jo.scale[0] : 0;
            double scale0_1 = convex_corner0[(jo.f0_0 - 2 + 1) % convex_corner0.size()] ? jo.scale[0] : 0;

            //if (joint.v0 == id) {
            //    for (auto& flag : convex_corner0)
            //        CGAL_Debug(flag);
            //    CGAL_Debug(8888);
            //    CGAL_Debug(convex_corner0[joint.f0_0 - 2]);
            //    CGAL_Debug(convex_corner0[(joint.f0_0 - 2 + 1) % convex_corner0.size()]);
            //    CGAL_Debug(joint.f0_0 - 2);
            //    CGAL_Debug(9999);
            //}

            std::vector<bool>convex_corner1;
            cgal_polyline_util::convex_corner(elements[jo.v1].polylines[0], convex_corner1);
            double scale1_0 = convex_corner1[jo.f1_0 - 2] ? jo.scale[0] : 0;
            double scale1_1 = convex_corner1[(jo.f1_0 - 2 + 1) % convex_corner1.size()] ? jo.scale[0] : 0;

            //if (joint.v1 == id) {
            //    for (auto& flag : convex_corner1)
            //        CGAL_Debug(flag);
            //    CGAL_Debug(8888);
            //    CGAL_Debug(convex_corner1[joint.f1_0 - 2]);
            //    CGAL_Debug(convex_corner1[(joint.f1_0 - 2 + 1) % convex_corner1.size()]);
            //    CGAL_Debug(joint.f1_0 - 2);
            //    CGAL_Debug(9999);
            //}

            //currrent
            cgal_polyline_util::Extend(pline0, 0, scale0_0, scale0_1);
            cgal_polyline_util::Extend(pline0, 2, scale0_1, scale0_0);

            //neighbor
            cgal_polyline_util::Extend(pline1, 0, scale1_0, scale1_1);
            cgal_polyline_util::Extend(pline1, 2, scale1_1, scale1_0);

            //Extend vertical
            cgal_polyline_util::Extend(pline0, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::Extend(pline0, 3, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::Extend(pline1, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::Extend(pline1, 3, jo.scale[1], jo.scale[1]);
        }

        /////////////////////////////////////////////////////////////////////////////////
        //move outlines by vector
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0_moved0 = pline0;//side 0
        CGAL_Polyline pline0_moved1 = pline0;//side 0        
        CGAL_Polyline pline1_moved = pline1;//side 1

        IK::Vector_3 f0_1_normal = f0_0_normal;
        cgal_vector_util::Unitize(f0_1_normal);
        f0_1_normal *= jo.scale[2] +jo.shift;


        //Move twice to remove one side and the cut surface around
        cgal_polyline_util::move(pline0_moved0, f0_0_normal);
        cgal_polyline_util::move(pline0_moved1, f0_1_normal);

        //Move once to remove the side and the cut the female joint
        cgal_polyline_util::move(pline1_moved, f1_0_normal);

        /////////////////////////////////////////////////////////////////////////////////
        //orient a tile
        //1) Create rectangle between two edge of the side
        //2) Create joint in XY plane and orient it to the two rectangles
        //3) Clipper boolean difference, cut joint polygon form the outline
        /////////////////////////////////////////////////////////////////////////////////

        //1) Create rectangle between two edge of the side
        IK::Point_3 edge_mid_0 = CGAL::midpoint(CGAL::midpoint(pline0[0], pline1[0]), CGAL::midpoint(pline0[1], pline1[1]));
        IK::Point_3 edge_mid_1 = CGAL::midpoint(CGAL::midpoint(pline0[3], pline1[3]), CGAL::midpoint(pline0[2], pline1[2]));
        double half_dist = std::sqrt(CGAL::squared_distance(edge_mid_0, edge_mid_1)) * 0.5;
        half_dist = 10;//Change to scale

        //IK::Vector_3 x_axis = edge_mid_1 - edge_mid_0;
        //cgal_vector_util::Unitize(x_axis);
        IK::Vector_3 z_axis = f0_0_normal;
        cgal_vector_util::Unitize(z_axis);
        //IK::Vector_3 y_axis = CGAL::cross_product(z_axis, x_axis);
       // y_axis = CGAL::midpoint(pline0[1], pline0[2]) - CGAL::midpoint(pline0[0], pline0[3]);
        //cgal_vector_util::Unitize(y_axis);
 
        z_axis *= jo.scale[2] / half_dist;
        
        //CGAL_Polyline rect0 = {
        //    edge_mid_1 - y_axis * half_dist - z_axis * half_dist,
        //    edge_mid_1 - y_axis * half_dist + z_axis * half_dist,
        //    edge_mid_0 - y_axis * half_dist + z_axis * half_dist,
        //    edge_mid_0 - y_axis * half_dist - z_axis * half_dist,
        //    edge_mid_1 - y_axis * half_dist - z_axis * half_dist,
        //};

        //CGAL_Polyline rect1 = {
        //    edge_mid_1 + y_axis * half_dist - z_axis * half_dist,
        //    edge_mid_1 + y_axis * half_dist + z_axis * half_dist,
        //    edge_mid_0 + y_axis * half_dist + z_axis * half_dist,
        //    edge_mid_0 + y_axis * half_dist - z_axis * half_dist,
        //    edge_mid_1 + y_axis * half_dist - z_axis * half_dist,
        //};

        //Insert of creating two rectangles at the ends, create centre one and project to two ends
        //viewer_polylines.emplace_back(
        //    CGAL_Polyline({
        //        CGAL::midpoint(pline0[1], pline0[2]),
        //        CGAL::midpoint(pline0[0], pline0[3])
        //        }));


       //IK::Segment_3 result;
        //CGAL::midpoint(CGAL::midpoint(pline0[0], pline1[0]), CGAL::midpoint(pline0[1], pline1[1]))
        //auto s0 = IK::Segment_3(pline0[0], pline0[1]);
        //auto s1 = IK::Segment_3(pline0[3], pline0[2]);
        //viewer_polylines.emplace_back(CGAL_Polyline({ pline0[0], pline0[1] }));
        //viewer_polylines.emplace_back(CGAL_Polyline({ pline0[3], pline0[2] }));
        
       // auto smid = IK::Segment_3(CGAL::midpoint(s0[0], s1[0]), CGAL::midpoint(s0[1], s1[1]));
       // auto pmid = CGAL::midpoint(smid[0], smid[1]);
       //     
       //IK::Segment_3 result = cgal_polyline_util::LineLineMaximumOverlap( s0, s1 );
       
       //IK::Point_3 p0 = s1.supporting_line().projection(pmid);
       //IK::Point_3 p1 = s0.supporting_line().projection(pmid);
       //viewer_polylines.emplace_back(CGAL_Polyline({ result[0], result[1] }));
       //viewer_polylines.emplace_back(CGAL_Polyline({ p0,p1 }));
       //viewer_polylines.emplace_back(CGAL_Polyline({ CGAL::midpoint(s0[0], s1[0]), CGAL::midpoint(s0[1], s1[1]) }));

       //viewer_polylines.emplace_back(jo.joint_lines[0]);
       //viewer_polylines.emplace_back(jo.joint_lines[1]);

       /////////////////////////////////////////////////////////////////////////////////////////////////////
       //Get average line
       IK::Segment_3 average_line;
       cgal_polyline_util::LineLineOverlapAverage(jo.joint_lines[0], jo.joint_lines[1], average_line);
       viewer_polylines.emplace_back(CGAL_Polyline({ average_line[0], average_line[1] }));

       //Get average thickness
       double half_thickness = (elements[jo.v0].thickness + elements[jo.v1].thickness)/4.0;

       //Move points up and down using cross produc
       auto x_axis = CGAL::cross_product(z_axis, average_line.to_vector());
       cgal_vector_util::Unitize(x_axis);
       
       IK::Point_3 p0 = CGAL::midpoint(average_line[0], average_line[1]) + x_axis * half_thickness;
       IK::Point_3 p1 = CGAL::midpoint(average_line[0], average_line[1]) - x_axis * half_thickness;
       if (CGAL::has_smaller_distance_to_point(CGAL::midpoint(pline0[0], pline0[1]), p0, p1))
           std::swap(p0, p1);
           
       //set y-axis
       auto y_axis = average_line.to_vector();
       cgal_vector_util::Unitize(y_axis);
       /////////////////////////////////////////////////////////////////////////////////////////////////////
       //    
       //y_axis = result.to_vector();
       //y_axis = (IK::Segment_3(jo.joint_lines[1][0], jo.joint_lines[1][1])).to_vector();
       //cgal_vector_util::Unitize(y_axis);
           
       CGAL_Polyline rect0 = {
   p0 - y_axis * half_dist * 1 - z_axis * half_dist,
   p0 - y_axis * half_dist * 1 + z_axis * half_dist,
   p1 - y_axis * half_dist * 1 + z_axis * half_dist,
   p1 - y_axis * half_dist * 1 - z_axis * half_dist,
   p0 - y_axis * half_dist * 1 - z_axis * half_dist,
       };
       CGAL_Polyline rect1 = {
    p0 - y_axis * half_dist * -1 - z_axis * half_dist,
    p0 - y_axis * half_dist * -1 + z_axis * half_dist,
    p1 - y_axis * half_dist * -1 + z_axis * half_dist,
    p1 - y_axis * half_dist * -1 - z_axis * half_dist,
    p0 - y_axis * half_dist * -1 - z_axis * half_dist,
       };
       //viewer_polylines.emplace_back(rect_mid_0);
       //viewer_polylines.emplace_back(rect_mid_1);
       //rect0 = rect_mid_0;
       //rect1 = rect_mid_1;

            /////////////////////////////////////////////////////////////////////////////////
            //output, no need to merge if already cut
            // vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
            /////////////////////////////////////////////////////////////////////////////////
            if (jo.shift > 0 && merge_with_joint) {
                jo.m[0] = {
                    //rect0,
                    //rect0,
                 

                      
                   
                       pline0_moved0,
                      pline0_moved0,
                      pline0,
                     pline0,
                };

                jo.m[1] = {
                     //rect1,
                    //rect1,
              
  
                       
              
                       pline0_moved1,
                       pline0_moved1,
                       pline0_moved0,
                       pline0_moved0,
                };
            }
            else {
                jo.m[0] = {
                    //rect0,
                    //rect0
                    // pline0_moved0,
                    //pline0_moved0
                   pline0,
                    pline0
                };

                jo.m[1] = {
                    //rect1,
                    //rect1
                   // pline0_moved1,
                   // pline0_moved1
                     pline0_moved0,
                    pline0_moved0                    
                };
            }

            jo.f[0] = {
                  // rect0,
                  // rect0
               pline1,
               pline1
            };

            jo.f[1] = {
                 //  rect1,
                 //  rect1
                pline1_moved,
                pline1_moved
               //pline1_moved,
               //pline1_moved
            };

            if (jo.shift > 0 && merge_with_joint)
                jo.m_boolean_type = { mill_project,mill_project,mill_project,mill_project };
            else
                jo.m_boolean_type = { mill_project,mill_project };

            jo.f_boolean_type = { mill_project,mill_project };

            /////////////////////////////////////////////////////////////////////////////////
            //if merge is needed
            // vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
            /////////////////////////////////////////////////////////////////////////////////
            if (merge_with_joint) {

                //2) Create joint in XY plane and orient it to the two rectangles
                joint joint_2;

               
                bool read_successful = joint_library_xml_parser::read_xml(joint_2, jo.type);
                joint_2.unit_scale = true;
                //joint_2.unit_scale_distance = 10;
                //printf("xml tile reading number of polyines %iz", joint_2.m[0].size());
                if (read_successful) {
                    joint_2.joint_volumes[0] = rect0;
                    joint_2.joint_volumes[1] = rect1;
                    joint_2.joint_volumes[2] = rect0;
                    joint_2.joint_volumes[3] = rect1;
                    joint_2.orient_to_connection_area();//and orient to connection volume

                    IK::Plane_3 plane_0_0(jo.m[0][0][0], elements[jo.v0].planes[jo.f0_0].orthogonal_vector());
                    IK::Plane_3 plane_0_1(jo.m[0][2][0], elements[jo.v0].planes[jo.f0_0].orthogonal_vector());
                    //Conical offset
                    double dist_two_outlines = std::sqrt(CGAL::squared_distance(jo.m[0][0][0], plane_0_1.projection(jo.m[0][0][0])));
                    double conic_offset = -cgal_math_util::triangle_edge_by_angle(dist_two_outlines, 15.0);
                    double conic_offset_opposite = -(0.8440 + conic_offset);
                    //conic_offset = 0.8440;
                    printf("\n %f", conic_offset);
                    printf("\n %f", conic_offset_opposite);

                    for (int i = 0; i < joint_2.m[0].size(); i++) {
                        //cgal_polyline_util::reverse_if_clockwise(joint_2.f[0][i], plane_0_0);
                        clipper_util::offset_2D(joint_2.m[0][i], plane_0_0, -conic_offset_opposite-conic_offset);//0.1 means more tighter

                    }

                    for (int i = 0; i < joint_2.m[1].size(); i++) {
                        //cgal_polyline_util::reverse_if_clockwise(joint_2.f[1][i], plane_0_0);
                        clipper_util::offset_2D(joint_2.m[1][i], plane_0_0, -conic_offset_opposite - conic_offset );
                    }

                    //printf("xml tile reading number of polyines %iz" , joint_2.m[0].size());

                    //3) Clipper boolean difference, cut joint polygon form the outline

                //Merge male, by performing boolean union
                //cgal_polyline_util::AveragePlane(



                    clipper_util::intersection_2D(jo.m[0][2], joint_2.m[0][0], plane_0_0, jo.m[0][2], 100000, 2);
                    clipper_util::intersection_2D(jo.m[1][2], joint_2.m[1][0], plane_0_1, jo.m[1][2], 100000, 2);


                   jo.m[0].insert(jo.m[0].end(), joint_2.m[0].begin(), joint_2.m[0].end());
                    jo.m[1].insert(jo.m[1].end(), joint_2.m[1].begin(), joint_2.m[1].end());
                    for (auto& m : joint_2.m_boolean_type)
                        jo.m_boolean_type.emplace_back('9');
                    //jo.m_boolean_type.insert(jo.m_boolean_type.end(), joint_2.m_boolean_type.begin(), joint_2.m_boolean_type.end());

                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //offset curve due to conic tool
                    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                   
                    //viewer_polylines.emplace_back(jo.m[0][0]);
                    //viewer_polylines.emplace_back(jo.m[0][2]);
                    
                    //double conic_offset = dist_two_outlines * std::tan(15 * 3.14159265 / 180.0);


/*                    jo.f[0].insert(jo.f[0].end(), joint_2.f[0].begin(), joint_2.f[0].end());
                    jo.f[1].insert(jo.f[1].end(), joint_2.f[1].begin(), joint_2.f[1].end());
                    jo.f_boolean_type.insert(jo.f_boolean_type.end(), joint_2.f_boolean_type.begin(), joint_2.f_boolean_type.end());          */          

                    for (int i = 0; i < joint_2.f[0].size(); i++) {
                        //cgal_polyline_util::reverse_if_clockwise(joint_2.f[0][i], plane_0_0);
                        clipper_util::offset_2D(joint_2.f[0][i], plane_0_0, conic_offset_opposite);//- conic_offset

                    }

                    for (int i = 0; i < joint_2.f[1].size(); i++) {
                        //cgal_polyline_util::reverse_if_clockwise(joint_2.f[1][i], plane_0_0);
                        clipper_util::offset_2D(joint_2.f[1][i], plane_0_0, conic_offset_opposite);// - conic_offset
                    }

                    //Add once for milling
                    jo.f[0].insert(jo.f[0].end(), joint_2.f[0].begin(), joint_2.f[0].end());
                    jo.f[1].insert(jo.f[1].end(), joint_2.f[1].begin(), joint_2.f[1].end());
  
                    for (auto& f : joint_2.f_boolean_type)
                        jo.f_boolean_type.emplace_back('6');
                    
                    //Add second time for conical cut
                    printf("\n %i", jo.f[0].size());
                    
                    jo.f[0].insert(jo.f[0].end(), joint_2.f[0].begin(), joint_2.f[0].end());
                    jo.f[1].insert(jo.f[1].end(), joint_2.f[1].begin(), joint_2.f[1].end());
                    for (auto& f : joint_2.f_boolean_type)
                        jo.f_boolean_type.emplace_back('8');
                    //jo.f_boolean_type.insert(jo.f_boolean_type.end(), joint_2.f_boolean_type.begin(), joint_2.f_boolean_type.end());
                }
            
        }
    }

    //1-9

    inline void ss_e_ip_0(joint& joint) {
        joint.name = "ss_e_ip_0";

        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.f[0] = {
            {IK::Point_3(0, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0, -0.5, -0.357142857142857)},
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)} };

        joint.f[1] = {
            {IK::Point_3(0, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0, 0.5, -0.357142857142857)},
            {IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)} };

        joint.m[0] = {
            {IK::Point_3(0, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0, -0.5, -0.357142857142857)},
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)} };

        joint.m[1] = {
            {IK::Point_3(0, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0, 0.5, -0.357142857142857)},
            {IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)} };

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };
    }

    inline void ss_e_ip_1(joint& joint) {
        //CGAL_Debug(0);
        joint.name = "ss_e_ip_1";

        //Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        //Number of divisions
        //Input joint line (its lengths)
        //Input distance for division
        ////////////////////////////////////////////////////////////////////
        //CGAL_Debug(1);
        //CGAL_Debug(joint.divisions);
        int divisions = (int)std::max(2, std::min(100, joint.divisions));
        divisions += divisions % 2;

        //////////////////////////////////////////////////////////////////////////////////////////
        //Interpolate points
        //////////////////////////////////////////////////////////////////////////////////////////
        //CGAL_Debug(2);
        std::vector<IK::Point_3> pts0;

        interpolate_points(IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5), divisions, false, pts0);
        IK::Vector_3 v(0.5, 0, 0);
        double shift_ = RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5);
        IK::Vector_3 v_d(0, 0, -(1.0 / ((divisions + 1) * 2)) * shift_);

        size_t count = pts0.size() * 2;

        //CGAL_Debug(count);
        //CGAL_Debug(3);
        //1st polyline
        std::vector<IK::Point_3> pline0;
        pline0.reserve(count);
        pline0.emplace_back(pts0[0]);
        pline0.emplace_back(pts0[0] - v - v_d);

        for (int i = 1; i < pts0.size() - 1; i++) {
            if (i % 2 == 1) {
                pline0.emplace_back(pts0[i] - v + v_d);
                pline0.emplace_back(pts0[i] + v - v_d);
            }
            else {
                pline0.emplace_back(pts0[i] + v + v_d);
                pline0.emplace_back(pts0[i] - v - v_d);
            }
        }

        pline0.emplace_back(pts0[pts0.size() - 1] - v + v_d);
        pline0.emplace_back(pts0[pts0.size() - 1]);

        // CGAL_Debug(4);
         //2nd polyline
        IK::Vector_3 v_o(0, 1, 0);
        std::vector<IK::Point_3> pline1;
        pline1.reserve(pline0.size());

        for (int i = 0; i < pline0.size(); i++) {
            pline1.emplace_back(pline0[i] + v_o);
        }

        //CGAL_Debug(5);
        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.f[0] = {
            pline0,
            {pline0.front(), pline0.back()} };

        joint.f[1] = {
            pline1,
            {pline1.front(), pline1.back()} };

        joint.m[0] = {
            pline0,
            {pline0.front(), pline0.back()} };

        joint.m[1] = {
            pline1,
            {pline1.front(), pline1.back()} };

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };
    }

    inline void ss_e_ip_2(joint& joint) {
        joint.name = "ss_e_ip_2";//butterfly x-fix

        //Joint lines, always the last line or rectangle is not a joint but an cutting element

        joint.f[0] = {
        {
        IK::Point_3(0, -0.5, -0.1166666667),
        IK::Point_3(0.5, -0.5, -0.4),
        IK::Point_3(0.5, -0.5, 0.4),
        IK::Point_3(0, -0.5, 0.1166666667),
        },
        {
        IK::Point_3(0, -0.5, -0.1166666667),
        IK::Point_3(0, -0.5, 0.1166666667),
        }
        };

        joint.f[1] = {
        {
        IK::Point_3(0, 0.5, -0.1166666667),
        IK::Point_3(0.5, 0.5, -0.4),
        IK::Point_3(0.5, 0.5, 0.4),
        IK::Point_3(0, 0.5, 0.1166666667),
        },
        {
        IK::Point_3(0, 0.5, -0.1166666667),
        IK::Point_3(0, 0.5, 0.1166666667),
        }
        };

        joint.m[0] = {
        {
        IK::Point_3(0, -0.5, -0.1166666667),
        IK::Point_3(-0.5, -0.5, -0.4),
        IK::Point_3(-0.5, -0.5, 0.4),
        IK::Point_3(0, -0.5, 0.1166666667),
        },
        {
        IK::Point_3(0, -0.5, -0.1166666667),
        IK::Point_3(0, -0.5, 0.1166666667),
        }
        };

        joint.m[1] = {
       {
        IK::Point_3(0, 0.5, -0.1166666667),
        IK::Point_3(-0.5, 0.5, -0.4),
        IK::Point_3(-0.5, 0.5, 0.4),
        IK::Point_3(0, 0.5, 0.1166666667),
        },

        {
        IK::Point_3(0, 0.5, -0.1166666667),
        IK::Point_3(0, 0.5, 0.1166666667),
        }
        };

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };
        joint.unit_scale = true;
    }

    //10-19
    inline void ss_e_op_0(joint& joint) {
        joint.name = "ss_e_op_0";

        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.f[0] = { {IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5)} };

        joint.m[0] = { {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, 0.5), IK::Point_3(-0.5, 0.5, -0.5)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285712), IK::Point_3(0.5, -0.5, -0.0714285714285716), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)} };

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };
    }

    inline void ss_e_op_2(joint& joint) {
        joint.name = "ss_e_op_2";

        //Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        //Number of divisions
        //Input joint line (its lengths)
        //Input distance for division
        ////////////////////////////////////////////////////////////////////

        int divisions = (int)std::max(4, std::min(20, joint.divisions));
        divisions += divisions % 2;
        ////////////////////////////////////////////////////////////////////
        //Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[0]);
        interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[1]);
        interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[2]);
        interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[3]);

        ////////////////////////////////////////////////////////////////////
        //Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));

        for (int i = start; i < 4; i += 1) {
            int mid = (int)(arrays[i].size() * 0.5);
            for (int j = 0; j < arrays[i].size(); j++) {
                //int flip = (j % 2 < 0) ? 1 : -1;

                int flip = (j < mid) ? 1 : -1;

                if (i == 1) {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 4 * v * flip;
                }
                else if (i == 0 || i == 2) {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 2 * v * flip;
                }
            }
        }

        ////////////////////////////////////////////////////////////////////
        //Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++) {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}
                };
            }
        }

        for (int i = 1; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++) {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[(i + 1) % 4][j]);
                }
                else {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else {
                joint.f[1] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };
    }

    inline void ss_e_op_1(joint& joint) {
        joint.name = "ss_e_op_1";

        //Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        //Number of divisions
        //Input joint line (its lengths)
        //Input distance for division
        ////////////////////////////////////////////////////////////////////

        int divisions = (int)std::max(2, std::min(20, joint.divisions));
        divisions += divisions % 2 + 2;
        ////////////////////////////////////////////////////////////////////
        //Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[2]);
        interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[3]);
        interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[0]);
        interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[1]);

        ////////////////////////////////////////////////////////////////////
        //Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;
        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++) {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++) {
                //int flip = (j % 2 == 0) ? 1 : -1;
                //flip = i < 2 ? flip : flip * -1;
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                arrays[i][j] += v * flip;
            }
        }

        ////////////////////////////////////////////////////////////////////
        //Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++) {
                /*bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;*/

                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}
                };
            }
        }

        for (int i = 1; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++) {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[(i + 1) % 4][j]);
                }
                else {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else {
                joint.f[1] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        joint.f_boolean_type = { '1', '1' };
        joint.m_boolean_type = { '1', '1' };

        //if (orient_to_connection_zone)
            //joint.orient_to_connection_area();
    }

    inline void ss_e_op_3(joint& joint) {
        //Miter tenon-mortise
        joint.name = "ss_e_op_3";

        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.f[0] = {
        {
        IK::Point_3(0.5, 0.5, 0.3),
        IK::Point_3(0.5, -1.499975, 0.3),
        IK::Point_3(0.5, -1.499975, -0.3),
        IK::Point_3(0.5, 0.5, -0.3),
        },

        {
        IK::Point_3(0.5, 0.5, 0.3),
        IK::Point_3(0.5, 0.5, -0.3),
        },

        {
        IK::Point_3(0.5, -0.5, 0.25),
        IK::Point_3(0.5, 0.5, 0.25),
        IK::Point_3(0.5, 0.5, -0.25),
        IK::Point_3(0.5, -0.5, -0.25),
        IK::Point_3(0.5, -0.5, 0.25),
        },

        {
        IK::Point_3(0.5, -0.5, 0.25),
        IK::Point_3(0.5, 0.5, 0.25),
        IK::Point_3(0.5, 0.5, -0.25),
        IK::Point_3(0.5, -0.5, -0.25),
        IK::Point_3(0.5, -0.5, 0.25),
        }
        };

        joint.f[1] = {
        {
        IK::Point_3(-0.5, -0.499975, 0.3),
        IK::Point_3(-0.5, -1.49995, 0.3),
        IK::Point_3(-0.5, -1.49995, -0.3),
        IK::Point_3(-0.5, -0.5, -0.3),
        },

        {
        IK::Point_3(-0.5, -0.499975, 0.3),
        IK::Point_3(-0.5, -0.5, -0.3),
        },

        {
        IK::Point_3(-0.5, -0.499975, 0.25),
        IK::Point_3(-0.5, 0.500025, 0.25),
        IK::Point_3(-0.5, 0.500025, -0.25),
        IK::Point_3(-0.5, -0.499975, -0.25),
        IK::Point_3(-0.5, -0.499975, 0.25),
        },

        {
        IK::Point_3(-0.5, -0.499975, 0.25),
        IK::Point_3(-0.5, 0.500025, 0.25),
        IK::Point_3(-0.5, 0.500025, -0.25),
        IK::Point_3(-0.5, -0.499975, -0.25),
        IK::Point_3(-0.5, -0.499975, 0.25),
        }
        };

        joint.m[0] = {
        {
        IK::Point_3(-0.5, -0.5, 0.3),
        IK::Point_3(0.5, -0.5, 0.3),
        IK::Point_3(0.5, -0.5, 0.25),
        IK::Point_3(-1, -0.5, 0.25),
        IK::Point_3(-1, -0.5, -0.25),
        IK::Point_3(0.5, -0.5, -0.25),
        IK::Point_3(0.5, -0.5, -0.3),
        IK::Point_3(-0.5, -0.5, -0.3),
        },

        {
        IK::Point_3(-0.5, -0.5, 0.3),
        IK::Point_3(-0.5, -0.5, -0.3),
        }
        };

        joint.m[1] = {
        {
        IK::Point_3(0.5, 0.5, 0.3),
        IK::Point_3(0.510075, 0.5, 0.3),
        IK::Point_3(0.5, 0.5, 0.25),
        IK::Point_3(-1, 0.5, 0.25),
        IK::Point_3(-1, 0.5, -0.25),
        IK::Point_3(0.5, 0.5, -0.25),
        IK::Point_3(0.510075, 0.5, -0.3),
        IK::Point_3(0.5, 0.5, -0.3),
        },

        {
        IK::Point_3(0.5, 0.5, 0.3),
        IK::Point_3(0.5, 0.5, -0.3),
        }
        };

        joint.f_boolean_type = { '3', '3', '2', '2' };
        joint.m_boolean_type = { '3', '3' };
    }

    inline void ss_e_r_0(joint& jo, std::vector<element>& elements) {

        jo.name = __func__;
        jo.orient = false;

        CGAL_Polyline rect_2 = cgal_polyline_util::tween_two_polylines(jo.joint_volumes[0], jo.joint_volumes[1], 0.5);

        IK::Point_3 p_rect_2_mid_0 = CGAL::midpoint(rect_2[0], rect_2[1]);
        IK::Point_3 p_rect_2_mid_1 = CGAL::midpoint(rect_2[3], rect_2[2]);
        IK::Vector_3 z_scaled = (p_rect_2_mid_0 - rect_2[1])*0.75;//this might be scaled by user
        IK::Vector_3 x = (jo.joint_volumes[1][0] - jo.joint_volumes[0][0])*0.5;
        double x_len = std::sqrt(x.squared_length());


        //viewer_polylines.emplace_back(CGAL_Polyline{ jo.joint_volumes[0][1] ,jo.joint_volumes[0][2] });

        
        CGAL_Polyline rect_half_0 = {
            p_rect_2_mid_0,// + z_scaled,
            rect_2[0] - z_scaled,
            rect_2[3] - z_scaled,
            p_rect_2_mid_1,// + z_scaled,
            p_rect_2_mid_0// + z_scaled
        };

        CGAL_Polyline rect_half_1 = {
            p_rect_2_mid_0 ,
            rect_2[1] + z_scaled,
            rect_2[2] + z_scaled,
            p_rect_2_mid_1,
            p_rect_2_mid_0
        };

        //Extend
        double y_extend_len = jo.scale[1];
        cgal_polyline_util::extend_equally(rect_half_0, 1, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_0, 3, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_1, 1, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_1, 3, y_extend_len);

       // viewer_polylines.emplace_back(CGAL_Polyline{ p_rect_2_mid_0 ,p_rect_2_mid_1 });
       // viewer_polylines.emplace_back(rect_half_0);

        double x_target_len = jo.scale[0];
        IK::Vector_3 x_offset = x * ( (x_len+x_target_len) / x_len);
        IK::Vector_3 x_tween = x * ((jo.shift) / x_len);//x_offset * jo.shift;

        std::array<IK::Vector_3, 4> offset_vectors = {
   x_tween,
   x_offset ,
   -x_tween,
    -x_offset ,
        };
        

        std::array<CGAL_Polyline, 4> m_rectangles{
     rect_half_0,rect_half_0,rect_half_0,rect_half_0
        };

        std::array<CGAL_Polyline, 4> f_rectangles{
rect_half_1,rect_half_1,rect_half_1,rect_half_1
        };

        for (int j = 0; j < 4; j++) {
            cgal_polyline_util::move(m_rectangles[j], offset_vectors[j]);
            cgal_polyline_util::move(f_rectangles[j], offset_vectors[j]);
            //viewer_polylines.emplace_back(m_rectangles[j]);
        }
        

        // Check sides
        // Add polylines
        // Add slices

        jo.m[0] = { m_rectangles[0],m_rectangles[0] , m_rectangles[2] , m_rectangles[2] };
        jo.m[1] = { m_rectangles[1] ,m_rectangles[1] , m_rectangles[3], m_rectangles[3] };

        jo.f[0] = { f_rectangles[0] , f_rectangles[0] , f_rectangles[2], f_rectangles[2] };
        jo.f[1] = { f_rectangles[1] ,f_rectangles[1] , f_rectangles[3] ,f_rectangles[3] };
      
        jo.m_boolean_type = { slice,slice,slice,slice };
        jo.f_boolean_type = { slice,slice,slice,slice };

    }
    
    //20-29
    inline void ts_e_p_0(joint& joint) {
        joint.name = "ts_e_p_0";

        joint.f[0] = { {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715)},
                      {IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714)},
                      {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285713)},
                      {IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714)},
                      {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)} };

        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.m[0] = { {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285715), IK::Point_3(0.5, 0.5, -0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)} };

        joint.f_boolean_type = { '2', '2', '2', '2' };
        joint.m_boolean_type = { '1', '1' };

        //if (orient_to_connection_zone)
            //joint.orient_to_connection_area();
    }

    inline void ts_e_p_1(joint& joint) {
        joint.name = "ts_e_p_1"; //Annen

        joint.f[0] = { {IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.277777777777778)},
                      {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.166666666666667)},
                      {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, 0.166666666666667)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.277777777777778)},
                      {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.166666666666667)},
                      {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667)} };

        //Joint lines, always the last line or rectangle is not a joint but an cutting element
        joint.m[0] = { {IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889)},
                      {IK::Point_3(0.5, -0.5, 0.5), IK::Point_3(0.5, -0.5, -0.5)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.0555555555555558), IK::Point_3(-0.5, -0.5, 0.0555555555555557), IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889)},
                      {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)} };

        joint.f_boolean_type = { '2', '2', '2', '2' };
        joint.m_boolean_type = { '1', '1' };

        //if (orient_to_connection_zone)
            //joint.orient_to_connection_area();
    }

    inline void ts_e_p_2(joint& joint) {
        joint.name = "ts_e_p_2";

        ////////////////////////////////////////////////////////////////////
        //Number of divisions
        //Input joint line (its lengths)
        //Input distance for division
        ////////////////////////////////////////////////////////////////////
        //printf("\n JOINT DIVISIONS \n");
        //CGAL_Debug(joint.divisions);
        //printf("\n JOINT DIVISIONS \n");
        int divisions = (int)std::max(2, std::min(20, joint.divisions));
        divisions += divisions % 2;

        //Resize arrays
        int size = (int)(divisions * 0.5) + 1;

        joint.f[0].reserve(size);
        joint.f[1].reserve(size);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        //Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
        interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
        interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
        interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

        ////////////////////////////////////////////////////////////////////
        //Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++) {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++) {
                int flip = (j % 2 == 0) ? 1 : -1;
                flip = i < 2 ? flip : flip * -1;

                arrays[i][j] += v * flip;
            }
        }

        ////////////////////////////////////////////////////////////////////
        //Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++) {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]}
                };
            }
        }

        for (int i = 0; i < joint.m[0][0].size(); i += 4) {
            joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
            joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
        }
        joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
        joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

        joint.f_boolean_type = std::vector<char>(size, '2');
        joint.m_boolean_type = { '1', '1' };
    }

    inline void ts_e_p_3(joint& joint) {
        joint.name = "ts_e_p_3";

        ////////////////////////////////////////////////////////////////////
        //Number of divisions
        //Input joint line (its lengths)
        //Input distance for division
        ////////////////////////////////////////////////////////////////////
        int divisions = (int)std::max(8, std::min(100, joint.divisions));
        //CGAL_Debug(divisions);
        //if (joint.tile_parameters.size() > 0)
         //   divisions = joint.tile_parameters[0];
        //CGAL_Debug(divisions);
        //CGAL_Debug(divisions);
        divisions -= divisions % 4;
        //CGAL_Debug(divisions);

        //if ((divisions / 4) % 2 == 1)
          // divisions += 4;

        //Resize arrays
        int size = (int)(divisions * 0.25) + 1;
        //CGAL_Debug(divisions);

       // divisions += divisions % 4;

        joint.f[0].reserve(size);
        joint.f[1].reserve(size);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> joinery_solver_joint_library.h <<Method>> ts_e_p_3 <<Description>> Joint Main Parameters");
#endif

        ////////////////////////////////////////////////////////////////////
        //Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];
        if (divisions == 0)return;
        interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
        interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
        interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
        interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

        //CGAL_Debug(divisions);
        //CGAL_Debug(arrays[0].size());
        //CGAL_Debug(arrays[1].size());
        //CGAL_Debug(arrays[2].size());
        //CGAL_Debug(arrays[3].size());

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> joinery_solver_joint_library.h <<Method>> ts_e_p_3 <<Description> Interpolate Points");
#endif

        ////////////////////////////////////////////////////////////////////
        //Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++) {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++) {
                int flip = (j % 2 == 0) ? 1 : -1;
                flip = i < 2 ? flip : flip * -1;

                arrays[i][j] += v * flip;
            }
        }

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> joinery_solver_joint_library.h <<Method>> ts_e_p_3 <<Description> Move Segments");
#endif

        ////////////////////////////////////////////////////////////////////
        //Create Polylines
        ////////////////////////////////////////////////////////////////////

        //CGAL_Debug(0);

        for (int i = 0; i < 4; i += 2) {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            //CGAL_Debug(1);

            for (int j = 0; j < arrays[0].size(); j++) {
                if (j % 4 > 1)
                    continue;

                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip) {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            //CGAL_Debug(2);

            if (arrays->size() == 0) {
                printf("\n empty");
                return;
            }

            if (i < 2) {
                //CGAL_Debug(3);
                //CGAL_Debug(arrays->size());
                //CGAL_Debug(arrays[0].size());
                pline.push_back(arrays[i + 0][arrays[0].size() - 1]);
                //CGAL_Debug(4);
                joint.m[1] = { pline, {pline[0], pline[pline.size() - 1]} };
                //CGAL_Debug(5);
            }
            else {
                //CGAL_Debug(6);
                pline.push_back(arrays[i + 1][arrays[0].size() - 1]);
                //CGAL_Debug(7);
                joint.m[0] = { pline,   {pline[0], pline[pline.size() - 1]} };
                //CGAL_Debug(8);
            }
            //CGAL_Debug(9);
        }
        //CGAL_Debug(10);

        for (int i = 0; i < joint.m[0][0].size() - joint.m[0][0].size() % 4; i += 4) {
            joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
            joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
        }
        //CGAL_Debug(20);

        joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
        joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

        //CGAL_Debug(30);
        joint.f_boolean_type = std::vector<char>(size, '2');
        joint.m_boolean_type = { '1', '1' };

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> joinery_solver_joint_library.h <<Method>> ts_e_p_3 <<Description> Create Polylines");
#endif
    }

    //30-39
    inline void cr_c_ip_0(joint& joint) {
        //printf("cr_c_ip_0");

        joint.name = "cr_c_ip_0";

        double scale = 1;
        joint.f[0] = {
            {IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)},
            {IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)}
        };

        joint.f[1] = {
            {IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)},
            {IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)}
        };

        joint.m[0] = {
            {IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)},
            {IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)}
        };

        joint.m[1] = {
            {IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)},
            {IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)} };

        joint.m_boolean_type = { '3', '3' };
        joint.f_boolean_type = { '3', '3' };

        //Orient to 3D
        //if (orient_to_connection_zone)
           // joint.orient_to_connection_area();
    }

    inline void cr_c_ip_1(joint& joint) {
        joint.name = "cr_c_ip_1";
        //double shift = 0.5;
        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),									//Center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), //CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),									//Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)								//Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        //Construct polylines from points
        joint.f[0] = {
            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, //Center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, //TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, //TopSide1
            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]},						 //BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]},						 //BotSide1

            {p[0], p[0 + 12], p[0 + 4], p[0 + 8], p[0]},	  //Corner0
            {p[1 + 12], p[1], p[1 + 8], p[1 + 4], p[1 + 12]}, //Corner1
            {p[2], p[2 + 12], p[2 + 4], p[2 + 8], p[2]},	  //Corner2
            {p[3 + 12], p[3], p[3 + 8], p[3 + 4], p[3 + 12]}  //Corner3
        };

        //Offset and
        //flip polylines
        joint.f[1].reserve(9);
        joint.m[0].reserve(9);
        joint.f[1].reserve(9);

        auto xform = to_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[9] = { 0.5, 0.4, 0.4, 0.4, 0.4, 0.1, 0.1, 0.1, 0.1 };
        for (int i = 0; i < 9; i++) {
            joint.f[1].push_back(joint.f[0][i]);

            //offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            unitize(cross);

            //offset
            for (int j = 0; j < joint.f[1][i].size(); j++)
                joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].push_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].push_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        ////Construct closed mesh
        ////Construct Mesh
        //Mesh mesh0 = new Mesh();
        //mesh0.Vertices.AddVertices(p);

        //mesh0.Faces.AddFace(0, 1, 2);
        //mesh0.Faces.AddFace(2, 3, 0);

        //mesh0.Faces.AddFace(1, 0, 0 + 8);
        //mesh0.Faces.AddFace(0 + 8, 1 + 8, 1);

        //mesh0.Faces.AddFace(3, 2, 2 + 8);
        //mesh0.Faces.AddFace(2 + 8, 3 + 8, 3);

        //mesh0.Faces.AddFace(2, 1, 1 + 12);
        //mesh0.Faces.AddFace(1 + 12, 2 + 12, 2);

        //mesh0.Faces.AddFace(0, 3, 3 + 12);
        //mesh0.Faces.AddFace(3 + 12, 0 + 12, 0);

        //mesh0.Faces.AddFace(0, 0 + 12, 0 + 4);
        //mesh0.Faces.AddFace(0 + 4, 0 + 8, 0);

        //mesh0.Faces.AddFace(1 + 12, 1, 1 + 8);
        //mesh0.Faces.AddFace(1 + 8, 1 + 4, 1 + 12);

        //mesh0.Faces.AddFace(2, 2 + 12, 2 + 4);
        //mesh0.Faces.AddFace(2 + 4, 2 + 8, 2);

        //mesh0.Faces.AddFace(3 + 12, 3, 3 + 8);
        //mesh0.Faces.AddFace(3 + 8, 3 + 4, 3 + 12);

        //mesh0.Faces.AddFace(0 + 12, 3 + 12, 3 + 4);
        //mesh0.Faces.AddFace(3 + 4, 0 + 4, 0 + 12);

        //mesh0.Faces.AddFace(0 + 4, 3 + 4, 3 + 8);
        //mesh0.Faces.AddFace(3 + 8, 4 + 4, 0 + 4);

        //mesh0.Faces.AddFace(4 + 4, 3 + 8, 2 + 8);
        //mesh0.Faces.AddFace(2 + 8, 1 + 8, 4 + 4);

        //mesh0.Faces.AddFace(1 + 8, 2 + 8, 2 + 4);
        //mesh0.Faces.AddFace(2 + 4, 1 + 4, 1 + 8);

        //mesh0.Faces.AddFace(1 + 4, 2 + 4, 2 + 12);
        //mesh0.Faces.AddFace(2 + 12, 1 + 12, 1 + 4);

        //Mesh mesh1 = mesh0.DuplicateMesh();
        //mesh1.Transform(xform);

        joint.m_boolean_type = { '6', '6', '6', '4', '4', '4', '4', '9', '9' };
        joint.f_boolean_type = { '6', '6', '6', '4', '4', '4', '4', '9', '9' };

        //Orient to 3D
        //if (orient_to_connection_zone)
            //joint.orient_to_connection_area();
    }

    //60-69
    inline void b_0(joint& joint) {
        joint.name = "b_0";
        joint.orient = false;//prevents joint from copying

        double temp_scale_y = 5;
        double temp_scale_z = 1;
        
        //printf("\nb_0\n");

        //Get center rectangle
        CGAL_Polyline mid_rectangle = cgal_polyline_util::tween_two_polylines(joint.joint_volumes[0], joint.joint_volumes[1],0.5);
        

        //X-Axis Extend polyline in scale[0]
        cgal_polyline_util::extend_equally(mid_rectangle, 1, joint.scale[0] + 0);
        cgal_polyline_util::extend_equally(mid_rectangle, 3, joint.scale[0] + 0);
        
        //Y-Axis Move rectangle down and give it a length of scale[1]
        //move to center
        IK::Vector_3 v = mid_rectangle[1] - mid_rectangle[0];
        v *= 0.5;

        mid_rectangle[0] += v;
        mid_rectangle[1] -= v;
        mid_rectangle[2] -= v;
        mid_rectangle[3] += v;
        mid_rectangle[4] += v;

        cgal_vector_util::Unitize(v);
        v *= joint.scale[1]+ temp_scale_y;
        mid_rectangle[0] += v;
        mid_rectangle[3] += v;
        mid_rectangle[4] += v;
    
        //Z-AxisOffset by normal, scale value gives the offset from the center
        IK::Vector_3 z_axis;
        cgal_vector_util::AverageNormal(mid_rectangle, z_axis, true, true);
        double len = cgal_vector_util::LengthVec(z_axis);
        z_axis *= (joint.scale[2] + temp_scale_z);
        

        cgal_polyline_util::shift(mid_rectangle, 2);
        CGAL_Polyline rect0 = mid_rectangle;
        CGAL_Polyline rect1 = mid_rectangle;
        CGAL_Polyline rect2 = mid_rectangle;
        CGAL_Polyline rect3 = mid_rectangle;
        cgal_polyline_util::move(rect1, z_axis );
        cgal_polyline_util::move(rect3, -z_axis );
        //printf("\n Length %f %f %f", len, joint.scale[2], temp_scale_z);
        rect0=cgal_polyline_util::tween_two_polylines(rect0, rect1, joint.shift);
        rect2=cgal_polyline_util::tween_two_polylines(rect2, rect3, joint.shift);

        //Create cutting geometry - two rectangles that represents single line cut

        //Add geometry and boolean types
        //viewer_polylines.emplace_back(rect0);
        //viewer_polylines.emplace_back(rect1);
        //viewer_polylines.emplace_back(rect2);
        //viewer_polylines.emplace_back(rect3);
        joint.m[0] = { rect0,rect0, rect2,rect2 };
        joint.m[1] = { rect1,rect1, rect3,rect3 };

        //joint.f[0] = { rect0, rect1 };
        //joint.f[1] = { rect2, rect3 };

        joint.m_boolean_type = { slice,slice,slice,slice };
        //joint.f_boolean_type = { '1', '1' };
        
    }
    
    inline void construct_joint_by_index(std::vector<element>& elements, std::vector<joint>& joints, std::vector<double>& default_parameters_for_four_types, std::vector<double>& scale) {// const double& division_distance_, const double& shift_,
        /////////////////////////////////////////////////////////////////////
        //You must define new joint each time you internalize it
        /////////////////////////////////////////////////////////////////////
        std::array<std::string, 70> joint_names;
        for (size_t i = 0; i < joint_names.size(); i++)
            joint_names[i] = "undefined";

        joint_names[1] = "ss_e_ip_0";
        joint_names[2] = "ss_e_ip_1";
        joint_names[3] = "ss_e_ip_2";
        joint_names[8] = "side_removal";
        joint_names[9] = "ss_e_ip_9";
        joint_names[10] = "ss_e_op_0";
        joint_names[11] = "ss_e_op_1";
        joint_names[12] = "ss_e_op_2";
        joint_names[18] = "side_removal";
        joint_names[19] = "ss_e_op_9";
        joint_names[20] = "ts_e_p_0";
        joint_names[21] = "ts_e_p_1";
        joint_names[22] = "ts_e_p_2";
        joint_names[22] = "ts_e_p_3";
        joint_names[28] = "side_removal";
        joint_names[29] = "ts_e_p_9";
        joint_names[30] = "cr_c_ip_0";
        joint_names[31] = "cr_c_ip_1";
        joint_names[38] = "side_removal";
        joint_names[39] = "cr_c_ip_9";
        joint_names[48] = "side_removal";
        joint_names[58] = "side_removal";
        joint_names[59] = "ss_e_r_9";
        joint_names[60] = "b_0";

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "construct_joint_by_index");
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Most joints are often the same
        //Store parameters as a string: "joint_type ; divisions, not dist ; shift"
        //If this dictionary already has joints, fill parameters: f, m, booleans, with existing ones.
        // This joint is not oriented, orientation happens at the end of the loop
        // if unique_joints.contains(string)
        //  replace parameters
        // else joint = createnewjoint()
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::map<std::string, joint> unique_joints;

        double division_distance = 1000;
        double shift = 0.5;

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s   ", __FILE__, __FUNCTION__, __LINE__, "Before Joint Iteration");
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Iterate joints constructed during search methods
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> ids_to_remove;
        ids_to_remove.reserve(joints.size());
        int counter = 0;
        //CGAL_Debug(joints.size());
        for (auto& jo : joints) {
            //CGAL_Debug(counter);
            counter++;

            //Select user given type
            //types0+265
            //CGAL_Debug(0);
            int id_representing_joint_name = -1;
            if (elements[jo.v0].joint_types.size() && elements[jo.v1].joint_types.size()) {
                int a = std::abs(elements[jo.v0].joint_types[jo.f0_0]);
                int b = std::abs(elements[jo.v1].joint_types[jo.f1_0]);
                id_representing_joint_name = (a > b) ? a : b;
               //CGAL_Debug(a, b, a);
            }
            else if (elements[jo.v0].joint_types.size()) {
                id_representing_joint_name = std::abs(elements[jo.v0].joint_types[jo.f0_0]);
            }
            else if (elements[jo.v1].joint_types.size()) {
                id_representing_joint_name = std::abs(elements[jo.v1].joint_types[jo.f1_0]);
            }

            //When users gives an input -> default_parameters_for_four_types
            int number_of_types = 7;	  //side-side in-plane | side-side out-of-plane | top-side | cross | top-top | side-side rotated
            int number_of_parameters = 3; //division_dist | shift | type
            bool default_parameters_given = default_parameters_for_four_types.size() == (long long)(number_of_types * number_of_parameters);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Select first by local search, only then by user given index, or by default
            // This setting produces joints without geometry, do you need to delete these joints?
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //CGAL_Debug(1);
            int group = -1;
            if (id_representing_joint_name == 0) {
                continue;
            }
            else if (jo.type == 12 && ((id_representing_joint_name > 0 && id_representing_joint_name < 10) || id_representing_joint_name == -1)) {
                group = 0;
            }
            else if (jo.type == 11 && ((id_representing_joint_name > 9 && id_representing_joint_name < 20) || id_representing_joint_name == -1)) {
                group = 1;
            }
            else if (jo.type == 20 && ((id_representing_joint_name > 19 && id_representing_joint_name < 30) || id_representing_joint_name == -1)) {
                group = 2;
            }
            else if (jo.type == 30 && ((id_representing_joint_name > 29 && id_representing_joint_name < 40) || id_representing_joint_name == -1)) {
                group = 3;
            }
            else if (jo.type == 40 && ((id_representing_joint_name > 39 && id_representing_joint_name < 50) || id_representing_joint_name == -1)) { //top-top
                group = 4;
            }
            else if (jo.type == 13 && ((id_representing_joint_name > 49 && id_representing_joint_name < 60) || id_representing_joint_name == -1)) { //side-side rotated
                group = 5;
            }
            else if (jo.type == 60 && ((id_representing_joint_name > 59 && id_representing_joint_name < 70) || id_representing_joint_name == -1)) { //top-top
                group = 6;
            }
            //printf("\n %i %i %i", group, id_representing_joint_name, jo.type);
            

            //define scale

            if (scale.size() > 0) {
                if (scale.size() < 3)
                    jo.scale = { scale[0] ,scale[0],scale[0] };
                else if (scale.size() == 3)
                    jo.scale = { scale[0] ,scale[1],scale[2] };
                else
                    jo.scale = { scale[group * 3 + 0] ,scale[group * 3 + 1],scale[group * 3 + 2] };
            }
            //CGAL_Debug(jo.scale[0], jo.scale[1], jo.scale[2]);

            //printf("\n");
            //CGAL_Debug(group);
            //CGAL_Debug(id_representing_joint_name);
            //CGAL_Debug(jo.type);
            //printf("\n");

            if (default_parameters_given) {
                division_distance = default_parameters_for_four_types[(long long)(number_of_parameters * group + 0)];
                jo.shift = default_parameters_for_four_types[(long long)(number_of_parameters * group + 1)];
                if (id_representing_joint_name == -1) //for cases when joint types per each edge are not defined
                    id_representing_joint_name = (int)default_parameters_for_four_types[(number_of_parameters * group + 2)];
            }

            if (id_representing_joint_name < 1 || group == -1) {
                ids_to_remove.emplace_back(counter - 1);
                continue;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Set unit distance, this value is a distance between joint volume rectangles, in case this property is set -> unit_scale = true
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //CGAL_Debug(2);
            //CGAL_Debug(division_distance);
            //CGAL_Debug(id_representing_joint_name);
            //std::ofstream f;
            //f.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
            //f << "construct_joint_by_index\n";
            //f << id_representing_joint_name;
            //f.close();
            jo.name = joint_names[id_representing_joint_name];
            jo.unit_scale_distance = elements[jo.v0].thickness * jo.scale[2];
            //CGAL_Debug(division_distance);

            jo.get_divisions(division_distance);
            //CGAL_Debug(2);
            std::string key = jo.get_key();

#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ID %i SHIFT %f KEY %s DIVISIONS %f   ", __FILE__, __FUNCTION__, __LINE__, "Assigned groups", id_representing_joint_name, jo.shift, key.c_str(), jo.divisions);
#endif

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Assign geometry to joints that currently contains only adjacency
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //if there is already such joint
            bool is_similar_joint = unique_joints.count(key) == 1;
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s %i", __FILE__, __FUNCTION__, __LINE__, "Is similar joint", is_similar_joint);
#endif
            //CGAL_Debug(0);
             //CGAL_Debug(id_representing_joint_name);
             //is_similar_joint = false;
            if (is_similar_joint) {
                //CGAL_Debug(0);
                auto u_j = unique_joints.at(key);
                //CGAL_Debug(1);
                jo.transfer_geometry(u_j);
                //CGAL_Debug(2);
                jo.orient_to_connection_area();
                //CGAL_Debug(3);
                continue;
            }
            //CGAL_Debug(4);
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "transfer_geometry");
#endif

            //else create a new joint
            switch (group) {
            case(0):
                switch (id_representing_joint_name) {
                case (1):
                    ss_e_ip_1(jo);
                    break;
                case (2):
                    ss_e_ip_0(jo);
                    break;

                case (3):
                    ss_e_ip_2(jo);
                    break;
                case (8):
                    side_removal(jo, elements);
                    break;
                case (9):
                    //wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    //CGAL_Debug(elements[joint.v0].thickness);
                    break;
                default:
                    ss_e_ip_1(jo);
                    //ss_e_ip_0(joint);
                    break;
                }
                break;
            case(1):
                switch (id_representing_joint_name) {
                    //printf("\nss_e_op_1");
                case (10):

                    ss_e_op_1(jo);
                    break;
                case (11):
                    ss_e_op_2(jo);
                    break;
                case (13):
                    ss_e_op_3(jo);
                    break;
                case (12):
                    ss_e_op_0(jo);
                    break;
                case (18):
                    side_removal(jo, elements);
                    break;
                case (19):
                    //if (true) {
                    //    std::ofstream myfile2;
                    //    myfile2.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\19.txt");
                    //    myfile2 << "Exists\n";
                    //    myfile2 << path_and_file_for_joints;
                    //    myfile2.close();
                    //}
                    //wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                default:
                    ss_e_op_1(jo);
                    //ss_e_op_0(joint);
                    break;
                }
                break;
            case(2):
                switch (id_representing_joint_name) {
                case (20):
                    //printf("\nts_e_p_1");
                    //printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ID %i SHIFT %f KEY %s DIVISIONS %f   ", __FILE__, __FUNCTION__, __LINE__, "Assigned groups", id_representing_joint_name, jo.shift, key.c_str(), jo.divisions);

                    ts_e_p_3(jo);
                    //ts_e_p_1(jo);
                    break;
                case (21):
                    ts_e_p_2(jo);
                    break;
                case (22):
                    ts_e_p_3(jo);
                    break;
                case (23):
                    ts_e_p_0(jo);
                    break;
                case (28):
                    side_removal(jo, elements);
                    break;
                case (29):
                    //wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                default:
                    ts_e_p_3(jo);
                    break;
                }
                break;
            case (3):

                switch (id_representing_joint_name) {
                case (30):
                    cr_c_ip_0(jo);
                    break;
                case (31):
                    //cr_c_ip_0(joint);
                    cr_c_ip_1(jo);
                    break;
                case (38):
                    side_removal(jo, elements);
                    break;
                case (39):
                    //wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                default:
                    cr_c_ip_0(jo);
                    //cr_c_ip_0(joint);
                    //printf(joint.name.c_str());
                    break;
                }
                break;
            case(4):
                switch (id_representing_joint_name) {
                case (48):
                    side_removal(jo, elements);
                    break;
                default:
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                }
            case(5):

                //CGAL_Debug(99999);
                //CGAL_Debug(id_representing_joint_name);
                switch (id_representing_joint_name) {
                    //CGAL_Debug(id_representing_joint_name);

                case (56):
                    ss_e_r_0(jo, elements);
                    break;
                case (57):
                    side_removal(jo, elements);
                    break;
                case (58):
                    side_removal(jo, elements,true);
                    break;
                case (59):
                    //CGAL_Debug(99999);
                    //ss_e_ip_1(jo);

                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?

                    break;
                default:

                    //default case remove polygon from each

                    side_removal(jo, elements);
                    //CGAL_Debug(jo.orient);
                    //joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                }

                break;

            case(6):
               
                switch (id_representing_joint_name) {
                case (60):
                    //printf("\nhi %i", id_representing_joint_name);                    
                    b_0(jo);
                    break;
                default:
                    b_0(jo);
                    break;
                }
                break;
            }
                
            //CGAL_Debug(5);
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "after unique joint create");
#endif
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Add to joint map because this joint was not present
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (jo.m[0].size() == 0 && jo.f[0].size() == 0) {
                //std::ofstream myfile;
                //myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
                //myfile << "Empty joint\n";
                //myfile << path_and_file_for_joints;
                //myfile.close();
                printf("\njoint name %s between elements %i and %i is empty",jo.name.c_str(), jo.v0, jo.v1);
                //CGAL_Debug(group);
                //CGAL_Debug(id_representing_joint_name);
                continue;
            }
            //continue;

            if (jo.orient) {
                joint temp_joint;
                jo.duplicate_geometry(temp_joint);
                unique_joints.insert(std::pair<std::string, joint>(key, temp_joint));
                jo.orient_to_connection_area();//and orient to connection volume
            }

#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "last");
#endif
        }

        //CGAL_Debug(111111);
    }
}
