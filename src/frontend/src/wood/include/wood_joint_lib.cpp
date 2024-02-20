
#include "../../../stdafx.h" //go up to the folder where the CMakeLists.txt is

#include "wood_joint_lib.h"

namespace wood_joint_lib
{
    namespace internal
    {
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // xml file reading methods for creating joints
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        bool exists_file(const std::string& name)
        {
            std::ifstream f(name.c_str());
            return f.good();
        }

        bool read_xml(wood::joint& joint, int type)
        {
            try
            {

                std::string name = ""; // custom

                switch (type)
                {
                case (12):
                case (0):
                    name = "ss_e_ip_9";
                    break;
                case (11):
                case (1):
                    name = "ss_e_op_9";
                    break;
                case (20):
                case (2):
                    name = "ts_e_p_9";
                    break;
                case (30):
                case (3):
                    name = "cr_c_ip_9";
                    break;
                case (40):
                case (4):
                    name = "tt_e_p_9";
                    break;
                case (13):
                case (5):
                    name = "ss_e_r_9";
                    break;
                case (60):
                case (6):
                    name = "b_9";
                    break;
                }

                joint.name = name;
                // printf("XML \n");
                // printf(type);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Check if XML file exists, wood_globals::PATH_AND_FILE_FOR_JOINTS is a global path
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_JOINERY_LIBRARY
                printf("\nCPP File path %s ", wood_globals::PATH_AND_FILE_FOR_JOINTS.c_str());
                printf("\nCPP Joint Type %i %s ", type, name.c_str());
#endif
                if (!exists_file(wood_globals::PATH_AND_FILE_FOR_JOINTS))
                {
#ifdef DEBUG_JOINERY_LIBRARY
                    printf("\nCPP File does not exist %s ", wood_globals::PATH_AND_FILE_FOR_JOINTS.c_str());
#endif

                    std::ofstream myfile;
                    myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
                    myfile << "Do not Exist\n";
                    myfile << wood_globals::PATH_AND_FILE_FOR_JOINTS;
                    myfile.close();

                    return false;
                }
                else
                {
                    // std::ofstream myfile;
                    // myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");
                    // myfile << "Exists\n";
                    // myfile << wood_globals::PATH_AND_FILE_FOR_JOINTS;
                    // myfile.close();
                }
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Read XML
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                boost::property_tree::ptree tree;
                boost::property_tree::xml_parser::read_xml(wood_globals::PATH_AND_FILE_FOR_JOINTS, tree);

#ifdef DEBUG_JOINERY_LIBRARY
                printf("\nCPP tree size %zi ", tree.size());
#endif

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Get Property to find the right wood::joint parameters and keys of XML file properties
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Traverse property tree example
                std::string xml_joint_name = "custom_joints." + name;
                std::array<std::string, 7> keys = { "f0", "f1", "m0", "m1", "f_boolean_type", "m_boolean_type", "properties" };

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Get properties from XML and add to joint
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                try
                {
                    for (boost::property_tree::ptree::value_type& v : tree.get_child(xml_joint_name))
                    {
                        // printf("\nCPP %s", v.first.c_str());
#ifdef DEBUG_JOINERY_LIBRARY
                        printf("\nCPP %s", v.first.c_str());
#endif
                        for (int i = 0; i < 4; i++)
                        {
                            if (v.first == keys[i])
                            {
                                // std::cout << v.first << "\n";

                                CGAL_Polyline polyline;
                                for (boost::property_tree::ptree::value_type& point : v.second)
                                {
                                    double x = point.second.get<double>("x");
                                    double y = point.second.get<double>("y");
                                    double z = point.second.get<double>("z");
                                    polyline.emplace_back(x, y, z);

#ifdef DEBUG_JOINERY_LIBRARY
                                    printf("\nCPP Vector %.17g %.17g %.17g", x, y, z);
#endif
                                }

                                // printf("\n");
                                // CGAL_Debug(polyline.size());

                                // Assign to array
                                switch (i)
                                {
                                case (0):
                                    joint.m[0].emplace_back(polyline);
                                    // printf("\nCPP joint.m[0].emplace_back(polyline)");
                                    break;

                                case (1):
                                    joint.m[1].emplace_back(polyline);
                                    // printf("\nCPP joint.m[1].emplace_back(polyline)");
                                    break;

                                case (2):
                                    joint.f[0].emplace_back(polyline);
                                    // printf("\nCPP joint.f[0].emplace_back(polyline)");
                                    break;

                                case (3):
                                    joint.f[1].emplace_back(polyline);
                                    // printf("\nCPP joint.f[1].emplace_back(polyline)");
                                    break;
                                }
                            }
                        }

                        for (int i = 4; i < 6; i++)
                        {
                            std::vector<wood_cut::cut_type> boolean_type;
                            if (v.first == keys[i])
                            {
                                // std::cout << v.first << "\n";

                                for (boost::property_tree::ptree::value_type& index : v.second)
                                {

                                    auto txt = index.second.get_value<std::string>();
                                    wood_cut::cut_type type = wood_cut::string_to_cut_type[txt];

#ifdef DEBUG_JOINERY_LIBRARY
                                    printf("\nCPP id %c ", id);
#endif
                                    if (i == 4)
                                    {
                                        joint.m_boolean_type.emplace_back(type);
                                        // emplace to female joint.m_boolean_type
                                    }
                                    else
                                    {
                                        joint.f_boolean_type.emplace_back(type);
                                        // emplace to female joint.f_boolean_type
                                    }
                                }

                                // Assign to array
                            }
                        }

                        if (v.first == keys[6])
                        {
                            std::vector<double> properties;
                            for (boost::property_tree::ptree::value_type& index : v.second)
                            {
                                double parameter = index.second.get_value<double>();

#ifdef DEBUG_JOINERY_LIBRARY
                                printf("\nCPP id %f ", parameter);
#endif
                                properties.emplace_back(parameter);
                            }

                            if (properties.size() > 0)
                            {
                                joint.unit_scale = properties[0] > 0;
                            }

                            // Assign to array
                        }
                    }
                }
                catch (std::exception& e)
                {
                    (void)e;
#ifdef DEBUG_JOINERY_LIBRARY
                    printf("\nCPP Wrong property ");

#endif
                    std::ofstream myfile;
                    myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
                    myfile << "Wrong Property\n";
                    myfile.close();
                    // printf("Wrong Property \n");
                    return false;
                }
                // std::ofstream myfile;
                // myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\example.txt");
                // myfile << "Good Result\n";
                // myfile.close();
                // CGAL_Debug(joint.f[0].size(), joint.f[1].size(), joint.f_boolean_type.size());
                // CGAL_Debug(joint.m[0].size(), joint.m[1].size(), joint.m_boolean_type.size());
                // printf("Exists \n");

                return true;
            }
            catch (...)
            {
                std::cout << "Error in internal -> read_xml \n";
                return false;
            }
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Main geometry methods used in cgal_inscribe_util to be used as self-contained header and source file
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        double length(double x, double y, double z)
        {
            // double ON_DBL_MIN = 2.22507385850720200e-308;

            double len;
            x = fabs(x);
            y = fabs(y);
            z = fabs(z);
            if (y >= x && y >= z)
            {
                len = x;
                x = y;
                y = len;
            }
            else if (z >= x && z >= y)
            {
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
            if (x > ON_DBL_MIN)
            {
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

        bool unitize(IK::Vector_3& vector)
        {
            bool rc = false;
            // Since x,y,z are floats, d will not be denormalized and the
            // ON_DBL_MIN tests in ON_2dVector::Unitize() are not needed.

            double d = length(vector.hx(), vector.hy(), vector.hz());
            if (d > 0.0)
            {
                double dx = vector.x();
                double dy = vector.y();
                double dz = vector.z();
                vector = IK::Vector_3(
                    (dx / d),
                    (dy / d),
                    (dz / d));
                rc = true;
            }
            return rc;
        }

        double remap_numbers(const double& value_to_remap, const double& low0, const double& high0, const double& low1, const double& high1)
        {
            return low1 + (value_to_remap - low0) * (high1 - low1) / (high0 - low0);
        }

        double lerp(const double& value0, const double& value1, const double& t)
        {
            return value0 + (value1 - value0) * t;
        }

        void interpolate_points(const IK::Point_3& from, const IK::Point_3& to, const int& steps, const bool& include_ends, std::vector<IK::Point_3>& interpolated_points)
        {
            if (include_ends)
            {
                interpolated_points.reserve(steps + 2);
                interpolated_points.emplace_back(from);

                for (int i = 1; i < steps + 1; i++)
                {
                    double num = i / (double)(1 + steps);
                    interpolated_points.emplace_back(lerp(from.hx(), to.hx(), num), lerp(from.hy(), to.hy(), num), lerp(from.hz(), to.hz(), num));
                }

                interpolated_points.emplace_back(to);
            }
            else
            {
                interpolated_points.reserve(steps);

                for (int i = 1; i < steps + 1; i++)
                {
                    double num = i / (double)(1 + steps);
                    interpolated_points.emplace_back(lerp(from.hx(), to.hx(), num), lerp(from.hy(), to.hy(), num), lerp(from.hz(), to.hz(), num));
                }
            }
        }

        CGAL::Aff_transformation_3<IK> rotation_in_xy_plane(const IK::Vector_3& x_axis, const IK::Vector_3& y_axis, const IK::Vector_3& z_axis)
        {
            return CGAL::Aff_transformation_3<IK>(
                x_axis.x(), y_axis.x(), z_axis.x(),
                x_axis.y(), y_axis.y(), z_axis.y(),
                x_axis.z(), y_axis.z(), z_axis.z());
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Custom joints
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void side_removal(wood::joint& jo, std::vector<wood::element>& elements, bool merge_with_joint)
    {
        jo.name = __func__;
        jo.orient = false;
        std::swap(jo.v0, jo.v1);
        std::swap(jo.f0_0, jo.f1_0);
        std::swap(jo.f0_1, jo.f1_1);
        std::swap(jo.joint_lines[1], jo.joint_lines[0]);
        std::swap(jo.joint_volumes[0], jo.joint_lines[2]);
        std::swap(jo.joint_volumes[1], jo.joint_lines[3]);

        /////////////////////////////////////////////////////////////////////////////////
        // offset vector
        /////////////////////////////////////////////////////////////////////////////////
        IK::Vector_3 f0_0_normal = elements[jo.v0].planes[jo.f0_0].orthogonal_vector();
        cgal_vector_util::unitize(f0_0_normal);
        // v0 *= (jo.scale[2] + jo.shift);
        f0_0_normal *= (jo.scale[2]);

        IK::Vector_3 f1_0_normal = elements[jo.v1].planes[jo.f1_0].orthogonal_vector();
        cgal_vector_util::unitize(f1_0_normal);
        f1_0_normal *= (jo.scale[2] + 2); // Forced for safety

        /////////////////////////////////////////////////////////////////////////////////
        // copy side rectangles
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0 = elements[jo.v0].polylines[jo.f0_0];
        CGAL_Polyline pline1 = elements[jo.v1].polylines[jo.f1_0];

        /////////////////////////////////////////////////////////////////////////////////
        // extend only convex angles and side polygons | only rectangles
        /////////////////////////////////////////////////////////////////////////////////
        // CGAL_Debug(pline0.size(), pline1.size());
        // CGAL_Debug(joint.scale[0]);
        if (pline0.size() == 5 && pline1.size() == 5)
        {
            // get convex_concave corners
            std::vector<bool> convex_corner0;

            cgal_polyline_util::get_convex_corners(elements[jo.v0].polylines[0], convex_corner0);

            int id = 15;

            double scale0_0 = convex_corner0[jo.f0_0 - 2] ? jo.scale[0] : 0;
            double scale0_1 = convex_corner0[(jo.f0_0 - 2 + 1) % convex_corner0.size()] ? jo.scale[0] : 0;

            std::vector<bool> convex_corner1;
            cgal_polyline_util::get_convex_corners(elements[jo.v1].polylines[0], convex_corner1);
            double scale1_0 = convex_corner1[jo.f1_0 - 2] ? jo.scale[0] : 0;
            double scale1_1 = convex_corner1[(jo.f1_0 - 2 + 1) % convex_corner1.size()] ? jo.scale[0] : 0;

            // currrent
            cgal_polyline_util::extend(pline0, 0, scale0_0, scale0_1);
            cgal_polyline_util::extend(pline0, 2, scale0_1, scale0_0);

            // neighbor
            cgal_polyline_util::extend(pline1, 0, scale1_0, scale1_1);
            cgal_polyline_util::extend(pline1, 2, scale1_1, scale1_0);

            // extend vertical
            cgal_polyline_util::extend(pline0, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline0, 3, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline1, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline1, 3, jo.scale[1], jo.scale[1]);
        }

        /////////////////////////////////////////////////////////////////////////////////
        // move outlines by vector
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0_moved0 = pline0; // side 0
        CGAL_Polyline pline0_moved1 = pline0; // side 0
        CGAL_Polyline pline1_moved = pline1;  // side 1

        IK::Vector_3 f0_1_normal = f0_0_normal;
        cgal_vector_util::unitize(f0_1_normal);
        f0_1_normal *= (jo.scale[2] + 2) + jo.shift; // Forced offset for safety

        // Move twice to remove one side and the cut surface around
        cgal_polyline_util::move(pline0_moved0, f0_0_normal);
        cgal_polyline_util::move(pline0_moved1, f0_1_normal);

        // Move once to remove the side and the cut the female joint
        cgal_polyline_util::move(pline1_moved, f1_0_normal);

        /////////////////////////////////////////////////////////////////////////////////
        // orient a tile
        // 1) Create rectangle between two edge of the side
        // 2) Create wood::joint in XY plane and orient it to the two rectangles
        // 3) Clipper boolean difference, cut wood::joint polygon form the outline
        /////////////////////////////////////////////////////////////////////////////////

        // 1) Create rectangle between two edge of the side
        IK::Point_3 edge_mid_0 = CGAL::midpoint(CGAL::midpoint(pline0[0], pline1[0]), CGAL::midpoint(pline0[1], pline1[1]));
        IK::Point_3 edge_mid_1 = CGAL::midpoint(CGAL::midpoint(pline0[3], pline1[3]), CGAL::midpoint(pline0[2], pline1[2]));
        double half_dist = std::sqrt(CGAL::squared_distance(edge_mid_0, edge_mid_1)) * 0.5;
        half_dist = 10; // Change to scale

        IK::Vector_3 z_axis = f0_0_normal;
        cgal_vector_util::unitize(z_axis);

        z_axis *= jo.scale[2] / half_dist;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get average line
        IK::Segment_3 average_line;
        cgal_polyline_util::line_line_overlap_average(jo.joint_lines[0], jo.joint_lines[1], average_line);
        // viewer_polylines.emplace_back(CGAL_Polyline({average_line[0], average_line[1]}));

        // Get average thickness
        double half_thickness = (elements[jo.v0].thickness + elements[jo.v1].thickness) / 4.0;

        // Move points up and down using cross product
        auto x_axis = CGAL::cross_product(z_axis, average_line.to_vector());
        cgal_vector_util::unitize(x_axis);

        IK::Point_3 p0 = CGAL::midpoint(average_line[0], average_line[1]) + x_axis * half_thickness;
        IK::Point_3 p1 = CGAL::midpoint(average_line[0], average_line[1]) - x_axis * half_thickness;
        if (CGAL::has_smaller_distance_to_point(CGAL::midpoint(pline0[0], pline0[1]), p0, p1))
            std::swap(p0, p1);

        // set y-axis
        auto y_axis = average_line.to_vector();
        cgal_vector_util::unitize(y_axis);
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // y_axis = result.to_vector();
        // y_axis = (IK::Segment_3(jo.joint_lines[1][0], jo.joint_lines[1][1])).to_vector();
        // cgal_vector_util::unitize(y_axis);

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
        // viewer_polylines.emplace_back(rect_mid_0);
        // viewer_polylines.emplace_back(rect_mid_1);
        // rect0 = rect_mid_0;
        // rect1 = rect_mid_1;

        /////////////////////////////////////////////////////////////////////////////////
        // output, no need to merge if already cut
        //  vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
        /////////////////////////////////////////////////////////////////////////////////

        CGAL_Polyline pline0_moved0_surfacing_tolerance_male = pline0_moved0;
        // cgal_polyline_util::move(pline0_moved0_surfacing_tolerance_male, z_axis * 0.20);

        // viewer_polylines.emplace_back(copypline);
        if (jo.shift > 0 && merge_with_joint)
        {
            jo.m[0] = {
                // rect0,
                // rect0,

                pline0_moved0_surfacing_tolerance_male,
                pline0_moved0_surfacing_tolerance_male,
                pline0,
                pline0,
            };

            jo.m[1] = {
                // rect1,
                // rect1,

                pline0_moved1,
                pline0_moved1,
                pline0_moved0,
                pline0_moved0,
            };
        }
        else
        {
            jo.m[0] = {
                // rect0,
                // rect0
                // pline0_moved0,
                // pline0_moved0
                pline0,
                pline0 };

            jo.m[1] = {
                // rect1,
                // rect1
                // pline0_moved1,
                // pline0_moved1
                pline0_moved0,
                pline0_moved0 };
        }

        jo.f[0] = {
            // rect0,
            // rect0
            pline1,
            pline1 };

        jo.f[1] = {
            //  rect1,
            //  rect1
            pline1_moved,
            pline1_moved
            // pline1_moved,
            // pline1_moved
        };

        if (jo.shift > 0 && merge_with_joint)
            jo.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project, wood_cut::mill_project, wood_cut::mill_project };
        else
            jo.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project };

        jo.f_boolean_type = { wood_cut::mill_project, wood_cut::mill_project };
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge in-plane joints 1-9
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ss_e_ip_0(wood::joint& joint)
    {
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
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

        joint.f_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ss_e_ip_1(wood::joint& joint)
    {
        // CGAL_Debug(0);
        joint.name = __func__;

        // Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input wood::joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////
        // CGAL_Debug(1);
        // CGAL_Debug(joint.divisions);
        int divisions = (int)std::max(2, std::min(100, joint.divisions));
        divisions += divisions % 2;

        //////////////////////////////////////////////////////////////////////////////////////////
        // Interpolate points
        //////////////////////////////////////////////////////////////////////////////////////////
        // CGAL_Debug(2);
        std::vector<IK::Point_3> pts0;

        internal::interpolate_points(IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5), divisions, false, pts0);
        IK::Vector_3 v(0.5, 0, 0);
        double shift_ = internal::remap_numbers(joint.shift, 0, 1.0, -0.5, 0.5);
        IK::Vector_3 v_d(0, 0, -(1.0 / ((divisions + 1) * 2)) * shift_);

        size_t count = pts0.size() * 2;

        // CGAL_Debug(count);
        // CGAL_Debug(3);
        // 1st polyline
        std::vector<IK::Point_3> pline0;
        pline0.reserve(count);
        pline0.emplace_back(pts0[0]);
        pline0.emplace_back(pts0[0] - v - v_d);

        for (int i = 1; i < pts0.size() - 1; i++)
        {
            if (i % 2 == 1)
            {
                pline0.emplace_back(pts0[i] - v + v_d);
                pline0.emplace_back(pts0[i] + v - v_d);
            }
            else
            {
                pline0.emplace_back(pts0[i] + v + v_d);
                pline0.emplace_back(pts0[i] - v - v_d);
            }
        }

        pline0.emplace_back(pts0[pts0.size() - 1] - v + v_d);
        pline0.emplace_back(pts0[pts0.size() - 1]);

        // CGAL_Debug(4);
        // 2nd polyline
        IK::Vector_3 v_o(0, 1, 0);
        std::vector<IK::Point_3> pline1;
        pline1.reserve(pline0.size());

        for (int i = 0; i < pline0.size(); i++)
        {
            pline1.emplace_back(pline0[i] + v_o);
        }

        // CGAL_Debug(5);
        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
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

        joint.f_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ss_e_ip_2(wood::joint& joint, std::vector<wood::element>& elements)
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joint parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        // name
        joint.name = __func__;
        joint.unit_scale_distance = elements[joint.v0].thickness;

        // parameters that comes from the joint
        bool default_values = false;
        double edge_length = !default_values ? std::sqrt(CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1])) : 1000; // std::sqrt(CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1])) : 1000;
        int divisions = !default_values ? joint.divisions : 5;
        double joint_volume_edge_length = !default_values ? joint.unit_scale_distance : 40; // std::sqrt(CGAL::squared_distance(joint.joint_volumes[0][1], joint.joint_volumes[0][2])) : 40;

        // scale down the edge, since wood_joint ->  bool joint::orient_to_connection_area() make the distance between joint volumes equal to 2nd joint volume edge
        edge_length *= joint.scale[2];

        // normalization to the unit space, joint_volume_edge_length is used for parametrization
        double move_length_scaled = edge_length / (divisions * joint_volume_edge_length);
        double total_length_scaled = edge_length / joint_volume_edge_length;

        // movement vectors to translate the unit joint to the end of the edge and then to its middle
        IK::Vector_3 dir(0, 0, 1);
        IK::Vector_3 move_from_center_to_the_end = dir * ((total_length_scaled * 0.5) - (move_length_scaled * 0.5));
        IK::Vector_3 move_length_dir = -dir * move_length_scaled;

        // std::cout << "divisions" << divisions << "\n";
        // std::cout << "edge_length" << edge_length << "\n";
        // std::cout << "joint_volume_edge_length" << joint_volume_edge_length << "\n";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Male default shape
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<CGAL_Polyline>
            male_0 = {
                {

                    IK::Point_3(0, -0.5, 0.1166666667),
                    IK::Point_3(-0.5, -0.5, 0.4),
                    IK::Point_3(-0.5, -0.5, -0.4),
                    IK::Point_3(0, -0.5, -0.1166666667),
                },
                {

                    IK::Point_3(0, -0.5, 0.1166666667),
                    IK::Point_3(0, -0.5, -0.1166666667),
                } };

        std::vector<CGAL_Polyline> male_1 = { {

                                                 IK::Point_3(0, 0.5, 0.1166666667),
                                                 IK::Point_3(-0.5, 0.5, 0.4),
                                                 IK::Point_3(-0.5, 0.5, -0.4),
                                                 IK::Point_3(0, 0.5, -0.1166666667),
                                             },

                                             {

                                                 IK::Point_3(0, 0.5, 0.1166666667),
                                                 IK::Point_3(0, 0.5, -0.1166666667),
                                             } };

        std::vector<wood_cut::cut_type> male_types{ wood_cut::edge_insertion, wood_cut::edge_insertion };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // female default shape
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<CGAL_Polyline> female_0 = { {

                                                   IK::Point_3(0, -0.5, 0.1166666667),
                                                   IK::Point_3(0.5, -0.5, 0.4),
                                                   IK::Point_3(0.5, -0.5, -0.4),
                                                   IK::Point_3(0, -0.5, -0.1166666667),
                                               },
                                               {

                                                   IK::Point_3(0, -0.5, 0.1166666667),
                                                   IK::Point_3(0, -0.5, -0.1166666667),
                                               } };

        std::vector<CGAL_Polyline> female_1 = { {

                                                   IK::Point_3(0, 0.5, 0.1166666667),
                                                   IK::Point_3(0.5, 0.5, 0.4),
                                                   IK::Point_3(0.5, 0.5, -0.4),
                                                   IK::Point_3(0, 0.5, -0.1166666667),
                                               },
                                               {

                                                   IK::Point_3(0, 0.5, 0.1166666667),
                                                   IK::Point_3(0, 0.5, -0.1166666667),
                                               } };

        std::vector<wood_cut::cut_type> female_types{ wood_cut::edge_insertion, wood_cut::edge_insertion };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Reserve memory for multiple copies
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);
        joint.m_boolean_type.reserve(2);
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.f_boolean_type.reserve(2);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy the default shapes and move them by merging
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        auto a = joint.m[0];
        joint.m[0].emplace_back(CGAL_Polyline());
        joint.m[1].emplace_back(CGAL_Polyline());
        joint.f[0].emplace_back(CGAL_Polyline());
        joint.f[1].emplace_back(CGAL_Polyline());

        joint.m[0].back().reserve(male_0[0].size() * divisions);
        joint.m[1].back().reserve(male_1[0].size() * divisions);
        joint.f[0].back().reserve(female_0[0].size() * divisions);
        joint.f[1].back().reserve(female_1[0].size() * divisions);

        for (auto i = 0; i < divisions; i++)
        {
            // copy the first outline, be sure that the point order is correct, so that the non-internsecting polyline can be created, else reverse it
            CGAL_Polyline male_moved_0 = male_0[0];
            CGAL_Polyline male_moved_1 = male_1[0];
            CGAL_Polyline female_moved_0 = female_0[0];
            CGAL_Polyline female_moved_1 = female_1[0];

            // move joints that are positioned at the center to the end of the segment and then back by half of the division length
            for (auto& p : male_moved_0)
                p += move_from_center_to_the_end + move_length_dir * i;

            for (auto& p : male_moved_1)
                p += move_from_center_to_the_end + move_length_dir * i;

            for (auto& p : female_moved_0)
                p += move_from_center_to_the_end + move_length_dir * i;

            for (auto& p : female_moved_1)
                p += move_from_center_to_the_end + move_length_dir * i;

            // merge with the main outline
            joint.m[0].back().insert(joint.m[0].back().end(), male_moved_0.begin(), male_moved_0.end());
            joint.m[1].back().insert(joint.m[1].back().end(), male_moved_1.begin(), male_moved_1.end());
            joint.f[0].back().insert(joint.f[0].back().end(), female_moved_0.begin(), female_moved_0.end());
            joint.f[1].back().insert(joint.f[1].back().end(), female_moved_1.begin(), female_moved_1.end());
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Add the insertion lines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.m[0]
            .emplace_back(CGAL_Polyline{ joint.m[0].front().front(), joint.m[0].front().back() });
        joint.m[1].emplace_back(CGAL_Polyline{ joint.m[1].front().front(), joint.m[1].front().back() });
        joint.f[0].emplace_back(CGAL_Polyline{ joint.f[0].front().front(), joint.f[0].front().back() });
        joint.f[1].emplace_back(CGAL_Polyline{ joint.f[1].front().front(), joint.f[1].front().back() });

        joint.f_boolean_type = female_types;
        joint.m_boolean_type = male_types;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // is unit scale
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.unit_scale = true;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // joint for preview
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // viewer_wood::scale = 1.0;
        // std::vector<std::vector<CGAL_Polyline>> input_polyline_pairs0;
        // input_polyline_pairs0.emplace_back(joint.m[0]);
        // input_polyline_pairs0.emplace_back(joint.m[1]);
        // viewer_wood::add(input_polyline_pairs0, 0); // grey
        // std::vector<std::vector<CGAL_Polyline>> input_polyline_pairs1;
        // input_polyline_pairs1.emplace_back(joint.f[0]);
        // input_polyline_pairs1.emplace_back(joint.f[1]);
        // CGAL_Polyline default_segment = {IK::Point_3(0, 0, -total_length_scaled * 0.5), IK::Point_3(0, 0, total_length_scaled * 0.5)};
        // input_polyline_pairs1.push_back({default_segment});
        // viewer_wood::add(input_polyline_pairs1, 2); // grey
        // // reset
        // viewer_wood::scale = 1000;
        // return true;
    }

    void ss_e_ip_3(wood::joint& joint)
    {
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element


        joint.f[0] = {
            {
IK::Point_3(-1.25, -0.5, -0.5),
IK::Point_3(1, -0.5,  -0.5),
IK::Point_3(1, -0.2, -0.5),
IK::Point_3(-1, 0.2, -0.5),
IK::Point_3(-1, 0.5,  -0.5),
IK::Point_3(-1.25, 0.5,  -0.5),
IK::Point_3(-1.25, -0.5,  -0.5),
            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},
            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };


        joint.f[1] = {
            {
IK::Point_3(-1.25, -0.5,  0.5),
IK::Point_3(1, -0.5,  0.5),
IK::Point_3(1, -0.2, 0.5),
IK::Point_3(-1, 0.2, 0.5),
IK::Point_3(-1, 0.5, 0.5),
IK::Point_3(-1.25, 0.5, 0.5),
IK::Point_3(-1.25, -0.5, 0.5),
            },
            {IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)},
            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };








        joint.m[0] = {
            {
IK::Point_3(1.25, 0.5, -0.5),
IK::Point_3(-1, 0.5, -0.5),
IK::Point_3(-1, 0.2, -0.5),
IK::Point_3(1, -0.2, -0.5),
IK::Point_3(1, -0.5, -0.5),
IK::Point_3(1.25, -0.5, -0.5),
IK::Point_3(1.25, 0.5, -0.5),

            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},
            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };

        joint.m[1] = {
            {

IK::Point_3(1.25, 0.5, 0.5),
IK::Point_3(-1, 0.5, 0.5),
IK::Point_3(-1, 0.2, 0.5),
IK::Point_3(1, -0.2, 0.5),
IK::Point_3(1, -0.5, 0.5),
IK::Point_3(1.25, -0.5, 0.5),
IK::Point_3(1.25, 0.5, 0.5),


            },
            {IK::Point_3(0, 0.5, 0.5), IK::Point_3(0, 0.5, -0.5)},

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };


        joint.f_boolean_type = { wood_cut::mill_project, wood_cut::mill_project, wood_cut::drill, wood_cut::drill, wood_cut::drill, wood_cut::drill };
        joint.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project, wood_cut::drill, wood_cut::drill, wood_cut::drill, wood_cut::drill };
    }

    void ss_e_ip_4(wood::joint& joint)
    {
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element













        joint.f[0] = {
            {
IK::Point_3(-1.25, -0.5, 0),
IK::Point_3(1, -0.5, 0),
IK::Point_3(1, -0.2, 0),
IK::Point_3(-1, 0.2, 0),
IK::Point_3(-1, 0.5, 0),
IK::Point_3(-1.25, 0.5, 0),
IK::Point_3(-1.25, -0.5, 0),
            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},

                        {
IK::Point_3(-1.25, 0.5, 0),
IK::Point_3(1, 0.5, 0),
IK::Point_3(1, 0.2, 0),
IK::Point_3(-1, -0.2, 0),
IK::Point_3(-1, -0.5, 0),
IK::Point_3(-1.25, -0.5, 0),
IK::Point_3(-1.25, 0.5, 0),

            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},






            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };






        joint.f[1] = {
            {
IK::Point_3(-1.25, -0.5, -0.5),
IK::Point_3(1, -0.5, -0.5),
IK::Point_3(1, -0.2, -0.5),
IK::Point_3(-1, 0.2, -0.5),
IK::Point_3(-1, 0.5, -0.5),
IK::Point_3(-1.25, 0.5, -0.5),
IK::Point_3(-1.25, -0.5, -0.5),
            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},

                        {
IK::Point_3(-1.25, 0.5, 0.5),
IK::Point_3(1, 0.5, 0.5),
IK::Point_3(1, 0.2, 0.5),
IK::Point_3(-1, -0.2, 0.5),
IK::Point_3(-1, -0.5, 0.5),
IK::Point_3(-1.25, -0.5, 0.5),
IK::Point_3(-1.25, 0.5, 0.5),


            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},



            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };





















        joint.m[0] = {
            {
IK::Point_3(1.25, 0.5, 0),
IK::Point_3(-1, 0.5, 0),
IK::Point_3(-1, 0.2, 0),
IK::Point_3(1, -0.2, 0),
IK::Point_3(1, -0.5, 0),
IK::Point_3(1.25, -0.5, 0),
IK::Point_3(1.25, 0.5, 0),

            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},

                        {

IK::Point_3(1.25, -0.5, 0),
IK::Point_3(-1, -0.5, 0),
IK::Point_3(-1, -0.2, 0),
IK::Point_3(1, 0.2, 0),
IK::Point_3(1, 0.5, 0),
IK::Point_3(1.25, 0.5, 0),
IK::Point_3(1.25, -0.5, 0),

            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},




            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };

        joint.m[1] = {
            {
IK::Point_3(1.25, 0.5, -0.5),
IK::Point_3(-1, 0.5, -0.5),
IK::Point_3(-1, 0.2, -0.5),
IK::Point_3(1, -0.2, -0.5),
IK::Point_3(1, -0.5, -0.5),
IK::Point_3(1.25, -0.5, -0.5),
IK::Point_3(1.25, 0.5, -0.5),

            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},

                        {

IK::Point_3(1.25, -0.5, 0.5),
IK::Point_3(-1, -0.5, 0.5),
IK::Point_3(-1, -0.2, 0.5),
IK::Point_3(1, 0.2, 0.5),
IK::Point_3(1, 0.5, 0.5),
IK::Point_3(1.25, 0.5, 0.5),
IK::Point_3(1.25, -0.5, 0.5),


            },
            {IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5)},





            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(-0.333333, -0.6, 0),
            IK::Point_3(-0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

            {
            IK::Point_3(0.333333, -0.6, 0),
            IK::Point_3(0.333333, 0.6, 0),
            },

        };


        joint.f_boolean_type = { wood_cut::mill_project, wood_cut::mill_project,wood_cut::mill_project, wood_cut::mill_project, wood_cut::drill, wood_cut::drill, wood_cut::drill, wood_cut::drill };
        joint.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project,wood_cut::mill_project, wood_cut::mill_project, wood_cut::drill, wood_cut::drill, wood_cut::drill, wood_cut::drill };
    }

    void ss_e_ip_custom(wood::joint& joint)
    {

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_ip_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_ip_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_ip_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_ip_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_ip_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_ip_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_ip_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_ip_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_ip_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_ip_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge out-of-plane joints 10-19
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ss_e_op_0(wood::joint& joint)
    {
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
        joint.f[0] = { {IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5)} };

        joint.m[0] = { {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, 0.5), IK::Point_3(-0.5, 0.5, -0.5)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.0714285714285713), IK::Point_3(0.5, -0.5, 0.0714285714285712), IK::Point_3(0.5, -0.5, -0.0714285714285716), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)} };

        joint.f_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ss_e_op_1(wood::joint& joint)
    {
        joint.name = __func__;

        // Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input wood::joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////

        int divisions = (int)std::max(2, std::min(20, joint.divisions));
        divisions += divisions % 2 + 2 * 0;

        ////////////////////////////////////////////////////////////////////
        // Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        internal::interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[2]);
        internal::interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[3]);
        internal::interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[0]);
        internal::interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[1]);

        ////////////////////////////////////////////////////////////////////
        // Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;
        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, internal::remap_numbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++)
        {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++)
            {
                // int flip = (j % 2 == 0) ? 1 : -1;
                // flip = i < 2 ? flip : flip * -1;
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                arrays[i][j] += v * flip;
            }
        }

        ////////////////////////////////////////////////////////////////////
        // Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                /*bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;*/

                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else
                {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else
            {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        for (int i = 1; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[(i + 1) % 4][j]);
                }
                else
                {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else
            {
                joint.f[1] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        joint.f_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };

        // if (orient_to_connection_zone)
        // joint.orient_to_connection_area();
    }

    void ss_e_op_2(wood::joint& joint)
    {
        joint.name = __func__;

        // Resize arrays
        joint.f[0].reserve(2);
        joint.f[1].reserve(2);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input wood::joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////

        int divisions = (int)std::max(4, std::min(20, joint.divisions));
        divisions += divisions % 2;
        ////////////////////////////////////////////////////////////////////
        // Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        internal::interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[0]);
        internal::interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[1]);
        internal::interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[2]);
        internal::interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[3]);

        ////////////////////////////////////////////////////////////////////
        // Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, internal::remap_numbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));

        for (int i = start; i < 4; i += 1)
        {
            int mid = (int)(arrays[i].size() * 0.5);
            for (int j = 0; j < arrays[i].size(); j++)
            {
                // int flip = (j % 2 < 0) ? 1 : -1;

                int flip = (j < mid) ? 1 : -1;

                if (i == 1)
                {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 4 * v * flip;
                }
                else if (i == 0 || i == 2)
                {
                    if (j < (mid - 1) || j > mid)
                        arrays[i][j] -= 2 * v * flip;
                }
            }
        }

        ////////////////////////////////////////////////////////////////////
        // Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else
                {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else
            {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        for (int i = 1; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[(i + 1) % 4][j]);
                }
                else
                {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else
            {
                joint.f[1] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        joint.f_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ss_e_op_3(wood::joint& joint)
    {
        // Miter tenon-mortise
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
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
            } };

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
            } };

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
            } };

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
            } };

        joint.f_boolean_type = { wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges, wood_cut::hole, wood_cut::hole };
        joint.m_boolean_type = { wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges };
    }

    void ss_e_op_4(wood::joint& joint, double t, bool chamfer, bool female_modify_outline, double x0, double x1, double y0, double y1, double z_ext0, double z_ext1) //
    {

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // wood::joint parameters
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.name = __func__;
        int number_of_tenons = joint.divisions;

        std::array<double, 2>
            x = { x0, x1 };
        std::array<double, 2> y = { y0, y1 };
        std::array<double, 2> z_ext = { z_ext0, z_ext1 };

        number_of_tenons = std::min(50, std::max(2, number_of_tenons)) * 2;
        double step = 1 / ((double)number_of_tenons - 1);
        std::array<double, 2> z = { z_ext[0] + step, z_ext[1] - step };
        number_of_tenons -= 2;
        step = 1 / ((double)number_of_tenons - 1);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Male
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int j = 0; j < 2; j++)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // memory and variables
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j].resize(2);
            joint.m[j][0].reserve(4 + number_of_tenons * 2);
            int sign = j == 0 ? -1 : 1;

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // tenon interpolation
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j][0].emplace_back(sign * x[1], y[j], z_ext[1]);
            joint.m[j][0].emplace_back(x[1] + j * 0.01, y[j], z_ext[1]);

            if (joint.divisions > 0)
            {
                for (int i = 0; i < number_of_tenons; i++)
                {
                    double z_ = z[1] + (z[0] - z[1]) * step * i;
                    double z_mid = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * t * 0.05 : z_ - (z[0] - z[1]) * step * t * 0.05;                                                             // female_modify_outline
                    double z_chamfer = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * ((t * 0.5) + ((1 - t) * 0.5 * 0.25)) : z_ - (z[0] - z[1]) * step * ((t * 0.5) + ((1 - t) * 0.5 * 0.25)); // chamfer
                    z_ = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5;

                    if (i % 2 == 0)
                    {
                        if (!female_modify_outline)
                            joint.m[j][0].emplace_back(x[1], y[j], z_mid);
                        joint.m[j][0].emplace_back(x[1], y[j], z_);
                        joint.m[j][0].emplace_back(x[0], y[j], z_);
                        if (chamfer)
                            joint.m[j][0].emplace_back(-0.75 + x[0], y[j], z_chamfer);
                    }
                    else
                    {
                        if (chamfer)
                            joint.m[j][0].emplace_back(-0.75 + x[0], y[j], z_chamfer);
                        joint.m[j][0].emplace_back(x[0], y[j], z_);
                        joint.m[j][0].emplace_back(x[1], y[j], z_);
                        if (!female_modify_outline)
                            joint.m[j][0].emplace_back(x[1], y[j], z_mid);

                    } // for scaling down the tenon
                }
            }

            joint.m[j][0].emplace_back(x[1] + j * 0.01, y[j], z_ext[0]);
            joint.m[j][0].emplace_back(sign * x[1], y[j], z_ext[0]);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // cut outlines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            joint.m[j][1] = {
                IK::Point_3(sign * x[1], y[j], z_ext[1]),
                IK::Point_3(sign * x[1], y[j], z_ext[0]),
            };
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Female
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        int female_modify_outline_count = 2 * female_modify_outline;
        for (int j = 0; j < 2; j++)
        {
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // memory and variables
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (joint.divisions == 0)
                joint.f[j].resize(female_modify_outline_count);
            else
                joint.f[j].resize(female_modify_outline_count + number_of_tenons);
            int sign = j == 0 ? 1 : -1;
            int j_inv = j == 0 ? 1 : 0;
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // main outlines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            if (female_modify_outline)
            {
                joint.f[j][0] =
                {
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                    IK::Point_3(y[j_inv], 3 * y[0], z_ext[1]),
                    IK::Point_3(y[j_inv], 3 * y[0], z_ext[0]),
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[0]),
                };

                joint.f[j][1] =
                {
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                    IK::Point_3(y[j_inv], sign * y[1], z_ext[1]),
                };
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // wood_cut::holes
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (joint.divisions > 0)
            {
                for (int i = 0; i < number_of_tenons; i += 2)
                {
                    double z_ = z[1] + (z[0] - z[1]) * step * i;
                    z_ = i % 2 == 0 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5;
                    joint.f[j][female_modify_outline_count + i].reserve(5);
                    joint.f[j][female_modify_outline_count + i + 1].reserve(5);
                    joint.f[j][female_modify_outline_count + i].emplace_back(y[j_inv], y[0], z_);
                    joint.f[j][female_modify_outline_count + i].emplace_back(y[j_inv], y[1], z_);

                    z_ = z[1] + (z[0] - z[1]) * step * (i + 1);
                    z_ = i % 2 == 1 ? z_ + (z[0] - z[1]) * step * t * 0.5 : z_ - (z[0] - z[1]) * step * t * 0.5;
                    joint.f[j][female_modify_outline_count + i].emplace_back(y[j_inv], y[1], z_);
                    joint.f[j][female_modify_outline_count + i].emplace_back(y[j_inv], y[0], z_);

                    joint.f[j][female_modify_outline_count + i].emplace_back(joint.f[j][female_modify_outline_count + i].front());
                    // copy
                    joint.f[j][female_modify_outline_count + i + 1] = joint.f[j][female_modify_outline_count + i];
                }
            }
        }

        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // // scale
        // /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //  IK::Point_3 mid (0,0,0);
        //     CGAL::Aff_transformation_3<IK> scale_xform(
        //     1, 0, 0,
        //     0, 1, 0,
        //     0, 0, joint.shift);

        //     for (int i = 0; i < 2; i++)
        //         for (int j = 0; j < joint.f[i].size(); j++)
        //             for (int k = 0; k < joint.f[i][j].size(); k++)
        //                 joint.f[i][j][k] = joint.f[i][j][k].transform(scale_xform);

        //     for (int i = 0; i < 2; i++)
        //         for (int j = 0; j < joint.m[i].size(); j++)
        //             for (int k = 0; k < joint.m[i][j].size(); k++)
        //                 joint.m[i][j][k] = joint.m[i][j][k].transform(scale_xform);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // boolean
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m_boolean_type = { wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges };

        if (joint.divisions > 0)
            joint.f_boolean_type.resize(female_modify_outline_count + number_of_tenons);
        else
            joint.f_boolean_type.resize(2);

        joint.f_boolean_type[0] = wood_cut::insert_between_multiple_edges;
        joint.f_boolean_type[1] = wood_cut::insert_between_multiple_edges;

        if (joint.divisions > 0)
            for (int i = 0; i < number_of_tenons; i += 2)
            {
                joint.f_boolean_type[female_modify_outline_count + i] = wood_cut::hole;
                joint.f_boolean_type[female_modify_outline_count + i + 1] = wood_cut::hole;
            }
    }

    void ss_e_op_5(wood::joint& jo, std::vector<wood::joint>& all_joints, bool disable_joint_divisions)
    {

        // return;
        //  get geometry from ss_e_op_4

        if (jo.linked_joints.size() == 0 || jo.linked_joints.size() > 2)

        { // wood::joint can have no links
            ss_e_op_4(jo, 0.00, true, true, -0.75, 0.5, -0.5, 0.5, -0.5, 0.5);
            jo.name = __func__;
            // std::cout << "ss_e_op_5" << std::endl;
            return;
        }
        else
        {
            ss_e_op_4(jo, 0.00, false, true, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);
            jo.name = __func__;
        }

        // std::cout << "ss_e_op_5" << std::endl;
        //  jo.linked_joints.pop_back();
        //  jo.linked_joints.pop_back();
        //  return;
        //  create geometry for linked wood::joint 0
        int a = 0;
        int b = 1;
        all_joints[jo.linked_joints[a]].divisions = jo.divisions; // division must be the same to merge two joints correctly
        ss_e_op_4(all_joints[jo.linked_joints[a]], 0.5, true, false, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);

        // set wood::joint sequences for wood::joint 0 and wood::joint 1
        // std::vector<std::vector<std::array<int, 4>>> linked_joints_seq; // assigned on wood_joint_library | it is nested because there can be umber of polylines | example {start_curr,step_curr} means that "start_curr+step_curr*i" and {start_link,step_link} -> "start_link+step_link*i"
        // wood::joint 0 - there is only one polyline
        std::vector<std::array<int, 4>> linked_joints_seq_0;
        linked_joints_seq_0.emplace_back(std::array<int, 4>{2, 4, 2, 8});
        jo.linked_joints_seq.emplace_back(linked_joints_seq_0);

        if (jo.linked_joints.size() == 2)
        {
            // create geometry for linked wood::joint 1
            all_joints[jo.linked_joints[b]].divisions = disable_joint_divisions ? jo.divisions * 0 : jo.divisions; // division must be the same to merge two joints correctly
            ss_e_op_4(all_joints[jo.linked_joints[b]], 0.00, true, false, -0.5, 0.5, -0.5, 0.5, -0.5, 0.5);

            std::vector<std::array<int, 4>> linked_joints_seq_1;
            // std::cout << "jo.f[0].size()" << jo.f[0].size() << "\n";
            for (int i = 0; i < jo.f[0].size(); i += 2)
            {
                if (i == 0)
                    linked_joints_seq_1.emplace_back(std::array<int, 4>{1, (int)jo.f[0][0].size() - 2, 1, (int)all_joints[jo.linked_joints[1]].m[0][0].size() - 2}); // std::array<std::vector<CGAL_Polyline>, 2> f;
                else
                    linked_joints_seq_1.emplace_back(std::array<int, 4>{0, 0, 0, 0});
            }
            jo.linked_joints_seq.emplace_back(linked_joints_seq_1);
        }
        // std::cout << "linked_joints " << jo.linked_joints_seq.size() << "\n";
    }

    void ss_e_op_6(wood::joint& jo, std::vector<wood::joint>& all_joints)
    {
        ss_e_op_5(jo, all_joints, true);
        jo.name = __func__;
    }

    void ss_e_op_custom(wood::joint& joint)
    {

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_op_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_op_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_op_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_op_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_op_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_op_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_op_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_op_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_op_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_op_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }


    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Side-to-side edge rotated joints 20-29
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ss_e_r_0(wood::joint& jo, std::vector<wood::element>& elements)
    {

        jo.name = __func__;
        jo.orient = false;

        CGAL_Polyline rect_2 = cgal_polyline_util::tween_two_polylines(jo.joint_volumes[0], jo.joint_volumes[1], 0.5);

        IK::Point_3 p_rect_2_mid_0 = CGAL::midpoint(rect_2[0], rect_2[1]);
        IK::Point_3 p_rect_2_mid_1 = CGAL::midpoint(rect_2[3], rect_2[2]);
        IK::Vector_3 z_scaled = (p_rect_2_mid_0 - rect_2[1]) * 0.75; // this might be scaled by user
        IK::Vector_3 x = (jo.joint_volumes[1][0] - jo.joint_volumes[0][0]) * 0.5;
        double x_len = std::sqrt(x.squared_length());

        // viewer_polylines.emplace_back(CGAL_Polyline{ jo.joint_volumes[0][1] ,jo.joint_volumes[0][2] });

        CGAL_Polyline rect_half_0 = {
            p_rect_2_mid_0, // + z_scaled,
            rect_2[0] - z_scaled,
            rect_2[3] - z_scaled,
            p_rect_2_mid_1, // + z_scaled,
            p_rect_2_mid_0  // + z_scaled
        };

        CGAL_Polyline rect_half_1 = {
            p_rect_2_mid_0,
            rect_2[1] + z_scaled,
            rect_2[2] + z_scaled,
            p_rect_2_mid_1,
            p_rect_2_mid_0 };

        // extend
        double y_extend_len = jo.scale[1];
        cgal_polyline_util::extend_equally(rect_half_0, 1, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_0, 3, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_1, 1, y_extend_len);
        cgal_polyline_util::extend_equally(rect_half_1, 3, y_extend_len);

        double x_target_len = jo.scale[0];
        IK::Vector_3 x_offset = x * ((5 + x_len + x_target_len) / x_len);
        IK::Vector_3 x_tween = x * ((0.25) / x_len); // jo.shift is the length //x_offset * jo.shift;

        std::array<IK::Vector_3, 4> offset_vectors = {
            x_tween,
            x_offset,
            -x_tween,
            -x_offset,
        };

        std::array<CGAL_Polyline, 4> m_rectangles{
            rect_half_0, rect_half_0, rect_half_0, rect_half_0 };

        std::array<CGAL_Polyline, 4> f_rectangles{
            rect_half_1, rect_half_1, rect_half_1, rect_half_1 };

        for (int j = 0; j < 4; j++)
        {
            cgal_polyline_util::move(m_rectangles[j], offset_vectors[j]);
            cgal_polyline_util::move(f_rectangles[j], offset_vectors[j]);
            // viewer_polylines.emplace_back(m_rectangles[j]);
        }

        // Check sides
        // Add polylines
        // Add wood_cut::slices

        jo.m[0] = { m_rectangles[0], m_rectangles[0], m_rectangles[2], m_rectangles[2] };
        jo.m[1] = { m_rectangles[1], m_rectangles[1], m_rectangles[3], m_rectangles[3] };

        jo.f[0] = { f_rectangles[0], f_rectangles[0], f_rectangles[2], f_rectangles[2] };
        jo.f[1] = { f_rectangles[1], f_rectangles[1], f_rectangles[3], f_rectangles[3] };

        jo.m_boolean_type = { wood_cut::slice, wood_cut::slice, wood_cut::slice, wood_cut::slice };
        jo.f_boolean_type = { wood_cut::slice, wood_cut::slice, wood_cut::slice, wood_cut::slice };
    }

    void ss_e_r_1(wood::joint& joint, int type)
    {
        // Miter tenon-mortise
        joint.name = __func__;

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
        switch (type)
        {

        case (1):
            joint.f[0] = {
                {
                    IK::Point_3(0.5, -0.625, -0.2),
                    IK::Point_3(0.5, -0.625, 0.2),
                    IK::Point_3(0.5, 0.125798405, 0.062633245),
                    IK::Point_3(0.5, 0.141579173, 0.057404662),
                    IK::Point_3(0.5, 0.155319467, 0.04804651),
                    IK::Point_3(0.5, 0.16596445, 0.035277208),
                    IK::Point_3(0.5, 0.172696911, 0.020077054),
                    IK::Point_3(0.5, 0.175, 0.003612957),
                    IK::Point_3(0.5, 0.175, -0.003612957),
                    IK::Point_3(0.5, 0.172696911, -0.020077054),
                    IK::Point_3(0.5, 0.16596445, -0.035277208),
                    IK::Point_3(0.5, 0.155319467, -0.04804651),
                    IK::Point_3(0.5, 0.141579173, -0.057404662),
                    IK::Point_3(0.5, 0.125798405, -0.062633245),
                    IK::Point_3(0.5, -0.625, -0.2),
                },
                {
                    IK::Point_3(0.5, -0.625, -0.2),
                    IK::Point_3(0.5, -0.625, 0.2),
                    IK::Point_3(0.5, 0.175, 0.2),
                    IK::Point_3(0.5, 0.175, -0.2),
                    IK::Point_3(0.5, -0.625, -0.2),
                },
            };
            joint.f[1] = {
                {
                    IK::Point_3(0, -0.625, -0.2),
                    IK::Point_3(0, -0.625, 0.2),
                    IK::Point_3(0, 0.125798405, 0.062633245),
                    IK::Point_3(0, 0.141579173, 0.057404662),
                    IK::Point_3(0, 0.155319467, 0.04804651),
                    IK::Point_3(0, 0.16596445, 0.035277208),
                    IK::Point_3(0, 0.172696911, 0.020077054),
                    IK::Point_3(0, 0.175, 0.003612957),
                    IK::Point_3(0, 0.175, -0.003612957),
                    IK::Point_3(0, 0.172696911, -0.020077054),
                    IK::Point_3(0, 0.16596445, -0.035277208),
                    IK::Point_3(0, 0.155319467, -0.04804651),
                    IK::Point_3(0, 0.141579173, -0.057404662),
                    IK::Point_3(0, 0.125798405, -0.062633245),
                    IK::Point_3(0, -0.625, -0.2),
                },
                {
                    IK::Point_3(0, -0.625, -0.2),
                    IK::Point_3(0, -0.625, 0.2),
                    IK::Point_3(0, 0.175, 0.2),
                    IK::Point_3(0, 0.175, -0.2),
                    IK::Point_3(0, -0.625, -0.2),
                },
            };
            joint.m[0] = {
                {
                    IK::Point_3(0, -0.625, -0.2),
                    IK::Point_3(0, -0.625, 0.2),
                    IK::Point_3(0, 0.125798405, 0.062633245),
                    IK::Point_3(0, 0.141579173, 0.057404662),
                    IK::Point_3(0, 0.155319467, 0.04804651),
                    IK::Point_3(0, 0.16596445, 0.035277208),
                    IK::Point_3(0, 0.172696911, 0.020077054),
                    IK::Point_3(0, 0.175, 0.003612957),
                    IK::Point_3(0, 0.175, -0.003612957),
                    IK::Point_3(0, 0.172696911, -0.020077054),
                    IK::Point_3(0, 0.16596445, -0.035277208),
                    IK::Point_3(0, 0.155319467, -0.04804651),
                    IK::Point_3(0, 0.141579173, -0.057404662),
                    IK::Point_3(0, 0.125798405, -0.062633245),
                    IK::Point_3(0, -0.625, -0.2),
                },
                {
                    IK::Point_3(0, -0.625, -0.2),
                    IK::Point_3(0, -0.625, 0.2),
                    IK::Point_3(0, 0.175, 0.2),
                    IK::Point_3(0, 0.175, -0.2),
                    IK::Point_3(0, -0.625, -0.2),
                },
            };
            joint.m[1] = {
                {
                    IK::Point_3(0.5, -0.625, -0.2),
                    IK::Point_3(0.5, -0.625, 0.2),
                    IK::Point_3(0.5, 0.125798405, 0.062633245),
                    IK::Point_3(0.5, 0.141579173, 0.057404662),
                    IK::Point_3(0.5, 0.155319467, 0.04804651),
                    IK::Point_3(0.5, 0.16596445, 0.035277208),
                    IK::Point_3(0.5, 0.172696911, 0.020077054),
                    IK::Point_3(0.5, 0.175, 0.003612957),
                    IK::Point_3(0.5, 0.175, -0.003612957),
                    IK::Point_3(0.5, 0.172696911, -0.020077054),
                    IK::Point_3(0.5, 0.16596445, -0.035277208),
                    IK::Point_3(0.5, 0.155319467, -0.04804651),
                    IK::Point_3(0.5, 0.141579173, -0.057404662),
                    IK::Point_3(0.5, 0.125798405, -0.062633245),
                    IK::Point_3(0.5, -0.625, -0.2),
                },
                {
                    IK::Point_3(0.5, -0.625, -0.2),
                    IK::Point_3(0.5, -0.625, 0.2),
                    IK::Point_3(0.5, 0.175, 0.2),
                    IK::Point_3(0.5, 0.175, -0.2),
                    IK::Point_3(0.5, -0.625, -0.2),
                },
            };
            joint.f_boolean_type = {
                wood_cut::conic,
                wood_cut::conic,
            };
            joint.m_boolean_type = {
                wood_cut::conic_reverse,
                wood_cut::conic_reverse,
            };

            break;
        default:
            joint.f[0] = {
                {
                    IK::Point_3(0.5, -0.825, 0),
                    IK::Point_3(0.5, -0.825, -0.151041813),
                    IK::Point_3(0.5, -0.825, -0.302083626),
                    IK::Point_3(0.5, -0.825, -0.39066965),
                    IK::Point_3(0.5, -0.764910275, -0.37364172),
                    IK::Point_3(0.5, -0.619590501, -0.332461718),
                    IK::Point_3(0.5, -0.474270727, -0.291281717),
                    IK::Point_3(0.5, -0.328950953, -0.250101715),
                    IK::Point_3(0.5, -0.183631179, -0.208921714),
                    IK::Point_3(0.5, -0.038311405, -0.167741712),
                    IK::Point_3(0.5, 0.078145959, -0.134740598),
                    IK::Point_3(0.5, 0.097349939, -0.129031066),
                    IK::Point_3(0.5, 0.106158874, -0.124374202),
                    IK::Point_3(0.5, 0.11914747, -0.117507751),
                    IK::Point_3(0.5, 0.138265279, -0.101937742),
                    IK::Point_3(0.5, 0.153962352, -0.082924124),
                    IK::Point_3(0.5, 0.165630347, -0.061203771),
                    IK::Point_3(0.5, 0.172817069, -0.037618462),
                    IK::Point_3(0.5, 0.175, -0.01554903),
                    IK::Point_3(0.5, 0.175, -3.4E-08),
                    IK::Point_3(0.5, 0.175, 0.01554903),
                    IK::Point_3(0.5, 0.172817069, 0.037618462),
                    IK::Point_3(0.5, 0.165630347, 0.061203771),
                    IK::Point_3(0.5, 0.153962352, 0.082924124),
                    IK::Point_3(0.5, 0.138265279, 0.101937742),
                    IK::Point_3(0.5, 0.11914747, 0.117507751),
                    IK::Point_3(0.5, 0.106158839, 0.12437399),
                    IK::Point_3(0.5, 0.09734984, 0.129030732),
                    IK::Point_3(0.5, 0.078145959, 0.134740598),
                    IK::Point_3(0.5, -0.038311405, 0.167741712),
                    IK::Point_3(0.5, -0.183631179, 0.208921714),
                    IK::Point_3(0.5, -0.328950953, 0.250101715),
                    IK::Point_3(0.5, -0.474270727, 0.291281717),
                    IK::Point_3(0.5, -0.619590501, 0.332461718),
                    IK::Point_3(0.5, -0.764910275, 0.37364172),
                    IK::Point_3(0.5, -0.825, 0.39066965),
                    IK::Point_3(0.5, -0.825, 0.302083626),
                    IK::Point_3(0.5, -0.825, 0.151041813),
                    IK::Point_3(0.5, -0.825, 0),
                },
                {
                    IK::Point_3(0.5, -0.825, 0.39066965),
                    IK::Point_3(0.5, 0.175, 0.39066965),
                    IK::Point_3(0.5, 0.175, -0.39066965),
                    IK::Point_3(0.5, -0.825, -0.39066965),
                    IK::Point_3(0.5, -0.825, 0.39066965),
                },
            };
            joint.f[1] = {
                {
                    IK::Point_3(0, -0.825, 0),
                    IK::Point_3(0, -0.825, -0.151041813),
                    IK::Point_3(0, -0.825, -0.302083626),
                    IK::Point_3(0, -0.825, -0.39066965),
                    IK::Point_3(0, -0.764910275, -0.37364172),
                    IK::Point_3(0, -0.619590501, -0.332461718),
                    IK::Point_3(0, -0.474270727, -0.291281717),
                    IK::Point_3(0, -0.328950953, -0.250101715),
                    IK::Point_3(0, -0.183631179, -0.208921714),
                    IK::Point_3(0, -0.038311405, -0.167741712),
                    IK::Point_3(0, 0.078145959, -0.134740598),
                    IK::Point_3(0, 0.097349939, -0.129031066),
                    IK::Point_3(0, 0.106158874, -0.124374202),
                    IK::Point_3(0, 0.11914747, -0.117507751),
                    IK::Point_3(0, 0.138265279, -0.101937742),
                    IK::Point_3(0, 0.153962352, -0.082924124),
                    IK::Point_3(0, 0.165630347, -0.061203771),
                    IK::Point_3(0, 0.172817069, -0.037618462),
                    IK::Point_3(0, 0.175, -0.01554903),
                    IK::Point_3(0, 0.175, -3.4E-08),
                    IK::Point_3(0, 0.175, 0.01554903),
                    IK::Point_3(0, 0.172817069, 0.037618462),
                    IK::Point_3(0, 0.165630347, 0.061203771),
                    IK::Point_3(0, 0.153962352, 0.082924124),
                    IK::Point_3(0, 0.138265279, 0.101937742),
                    IK::Point_3(0, 0.11914747, 0.117507751),
                    IK::Point_3(0, 0.106158839, 0.12437399),
                    IK::Point_3(0, 0.09734984, 0.129030732),
                    IK::Point_3(0, 0.078145959, 0.134740598),
                    IK::Point_3(0, -0.038311405, 0.167741712),
                    IK::Point_3(0, -0.183631179, 0.208921714),
                    IK::Point_3(0, -0.328950953, 0.250101715),
                    IK::Point_3(0, -0.474270727, 0.291281717),
                    IK::Point_3(0, -0.619590501, 0.332461718),
                    IK::Point_3(0, -0.764910275, 0.37364172),
                    IK::Point_3(0, -0.825, 0.39066965),
                    IK::Point_3(0, -0.825, 0.302083626),
                    IK::Point_3(0, -0.825, 0.151041813),
                    IK::Point_3(0, -0.825, 0),
                },
                {
                    IK::Point_3(0, -0.825, 0.39066965),
                    IK::Point_3(0, 0.175, 0.39066965),
                    IK::Point_3(0, 0.175, -0.39066965),
                    IK::Point_3(0, -0.825, -0.39066965),
                    IK::Point_3(0, -0.825, 0.39066965),
                },
            };
            joint.m[0] = {
                {
                    IK::Point_3(0, -0.825, 0),
                    IK::Point_3(0, -0.825, -0.151041813),
                    IK::Point_3(0, -0.825, -0.302083626),
                    IK::Point_3(0, -0.825, -0.39066965),
                    IK::Point_3(0, -0.764910275, -0.37364172),
                    IK::Point_3(0, -0.619590501, -0.332461718),
                    IK::Point_3(0, -0.474270727, -0.291281717),
                    IK::Point_3(0, -0.328950953, -0.250101715),
                    IK::Point_3(0, -0.183631179, -0.208921714),
                    IK::Point_3(0, -0.038311405, -0.167741712),
                    IK::Point_3(0, 0.078145959, -0.134740598),
                    IK::Point_3(0, 0.097349939, -0.129031066),
                    IK::Point_3(0, 0.106158874, -0.124374202),
                    IK::Point_3(0, 0.11914747, -0.117507751),
                    IK::Point_3(0, 0.138265279, -0.101937742),
                    IK::Point_3(0, 0.153962352, -0.082924124),
                    IK::Point_3(0, 0.165630347, -0.061203771),
                    IK::Point_3(0, 0.172817069, -0.037618462),
                    IK::Point_3(0, 0.175, -0.01554903),
                    IK::Point_3(0, 0.175, -3.4E-08),
                    IK::Point_3(0, 0.175, 0.01554903),
                    IK::Point_3(0, 0.172817069, 0.037618462),
                    IK::Point_3(0, 0.165630347, 0.061203771),
                    IK::Point_3(0, 0.153962352, 0.082924124),
                    IK::Point_3(0, 0.138265279, 0.101937742),
                    IK::Point_3(0, 0.11914747, 0.117507751),
                    IK::Point_3(0, 0.106158839, 0.12437399),
                    IK::Point_3(0, 0.09734984, 0.129030732),
                    IK::Point_3(0, 0.078145959, 0.134740598),
                    IK::Point_3(0, -0.038311405, 0.167741712),
                    IK::Point_3(0, -0.183631179, 0.208921714),
                    IK::Point_3(0, -0.328950953, 0.250101715),
                    IK::Point_3(0, -0.474270727, 0.291281717),
                    IK::Point_3(0, -0.619590501, 0.332461718),
                    IK::Point_3(0, -0.764910275, 0.37364172),
                    IK::Point_3(0, -0.825, 0.39066965),
                    IK::Point_3(0, -0.825, 0.302083626),
                    IK::Point_3(0, -0.825, 0.151041813),
                    IK::Point_3(0, -0.825, 0),
                },
                {
                    IK::Point_3(0, -0.825, 0.39066965),
                    IK::Point_3(0, 0.175, 0.39066965),
                    IK::Point_3(0, 0.175, -0.39066965),
                    IK::Point_3(0, -0.825, -0.39066965),
                    IK::Point_3(0, -0.825, 0.39066965),
                },
            };
            joint.m[1] = {
                {
                    IK::Point_3(0.5, -0.825, 0),
                    IK::Point_3(0.5, -0.825, -0.151041813),
                    IK::Point_3(0.5, -0.825, -0.302083626),
                    IK::Point_3(0.5, -0.825, -0.39066965),
                    IK::Point_3(0.5, -0.764910275, -0.37364172),
                    IK::Point_3(0.5, -0.619590501, -0.332461718),
                    IK::Point_3(0.5, -0.474270727, -0.291281717),
                    IK::Point_3(0.5, -0.328950953, -0.250101715),
                    IK::Point_3(0.5, -0.183631179, -0.208921714),
                    IK::Point_3(0.5, -0.038311405, -0.167741712),
                    IK::Point_3(0.5, 0.078145959, -0.134740598),
                    IK::Point_3(0.5, 0.097349939, -0.129031066),
                    IK::Point_3(0.5, 0.106158874, -0.124374202),
                    IK::Point_3(0.5, 0.11914747, -0.117507751),
                    IK::Point_3(0.5, 0.138265279, -0.101937742),
                    IK::Point_3(0.5, 0.153962352, -0.082924124),
                    IK::Point_3(0.5, 0.165630347, -0.061203771),
                    IK::Point_3(0.5, 0.172817069, -0.037618462),
                    IK::Point_3(0.5, 0.175, -0.01554903),
                    IK::Point_3(0.5, 0.175, -3.4E-08),
                    IK::Point_3(0.5, 0.175, 0.01554903),
                    IK::Point_3(0.5, 0.172817069, 0.037618462),
                    IK::Point_3(0.5, 0.165630347, 0.061203771),
                    IK::Point_3(0.5, 0.153962352, 0.082924124),
                    IK::Point_3(0.5, 0.138265279, 0.101937742),
                    IK::Point_3(0.5, 0.11914747, 0.117507751),
                    IK::Point_3(0.5, 0.106158839, 0.12437399),
                    IK::Point_3(0.5, 0.09734984, 0.129030732),
                    IK::Point_3(0.5, 0.078145959, 0.134740598),
                    IK::Point_3(0.5, -0.038311405, 0.167741712),
                    IK::Point_3(0.5, -0.183631179, 0.208921714),
                    IK::Point_3(0.5, -0.328950953, 0.250101715),
                    IK::Point_3(0.5, -0.474270727, 0.291281717),
                    IK::Point_3(0.5, -0.619590501, 0.332461718),
                    IK::Point_3(0.5, -0.764910275, 0.37364172),
                    IK::Point_3(0.5, -0.825, 0.39066965),
                    IK::Point_3(0.5, -0.825, 0.302083626),
                    IK::Point_3(0.5, -0.825, 0.151041813),
                    IK::Point_3(0.5, -0.825, 0),
                },
                {
                    IK::Point_3(0.5, -0.825, 0.39066965),
                    IK::Point_3(0.5, 0.175, 0.39066965),
                    IK::Point_3(0.5, 0.175, -0.39066965),
                    IK::Point_3(0.5, -0.825, -0.39066965),
                    IK::Point_3(0.5, -0.825, 0.39066965),
                },
            };

            break;
        }

        joint.f_boolean_type = {
            wood_cut::conic,
            wood_cut::conic,
        };
        joint.m_boolean_type = {
            wood_cut::conic_reverse,
            wood_cut::conic_reverse,
        };
    }

    void side_removal_ss_e_r_1(wood::joint& jo, std::vector<wood::element>& elements, bool merge_with_joint)
    {
        jo.name = __func__;
        jo.orient = false;
        std::swap(jo.v0, jo.v1);
        std::swap(jo.f0_0, jo.f1_0);
        std::swap(jo.f0_1, jo.f1_1);
        std::swap(jo.joint_lines[1], jo.joint_lines[0]);
        std::swap(jo.joint_volumes[0], jo.joint_lines[2]);
        std::swap(jo.joint_volumes[1], jo.joint_lines[3]);

        // printf("Side_Removal");

        /////////////////////////////////////////////////////////////////////////////////
        // offset vector
        /////////////////////////////////////////////////////////////////////////////////
        IK::Vector_3 f0_0_normal = elements[jo.v0].planes[jo.f0_0].orthogonal_vector();
        cgal_vector_util::unitize(f0_0_normal);
        // v0 *= (jo.scale[2] + jo.shift);
        f0_0_normal *= (jo.scale[2]);

        IK::Vector_3 f1_0_normal = elements[jo.v1].planes[jo.f1_0].orthogonal_vector();
        cgal_vector_util::unitize(f1_0_normal);
        f1_0_normal *= (jo.scale[2] + 2); // Forced for safety

        /////////////////////////////////////////////////////////////////////////////////
        // copy side rectangles
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0 = elements[jo.v0].polylines[jo.f0_0];
        CGAL_Polyline pline1 = elements[jo.v1].polylines[jo.f1_0];

        /////////////////////////////////////////////////////////////////////////////////
        // extend only convex angles and side polygons | only rectangles
        /////////////////////////////////////////////////////////////////////////////////
        // CGAL_Debug(pline0.size(), pline1.size());
        // CGAL_Debug(joint.scale[0]);
        if (pline0.size() == 5 && pline1.size() == 5)
        {
            // get convex_concave corners
            std::vector<bool> convex_corner0;

            cgal_polyline_util::get_convex_corners(elements[jo.v0].polylines[0], convex_corner0);

            int id = 15;

            double scale0_0 = convex_corner0[jo.f0_0 - 2] ? jo.scale[0] : 0;
            double scale0_1 = convex_corner0[(jo.f0_0 - 2 + 1) % convex_corner0.size()] ? jo.scale[0] : 0;

            std::vector<bool> convex_corner1;
            cgal_polyline_util::get_convex_corners(elements[jo.v1].polylines[0], convex_corner1);
            double scale1_0 = convex_corner1[jo.f1_0 - 2] ? jo.scale[0] : 0;
            double scale1_1 = convex_corner1[(jo.f1_0 - 2 + 1) % convex_corner1.size()] ? jo.scale[0] : 0;

            // currrent
            cgal_polyline_util::extend(pline0, 0, scale0_0, scale0_1);
            cgal_polyline_util::extend(pline0, 2, scale0_1, scale0_0);

            // neighbor
            cgal_polyline_util::extend(pline1, 0, scale1_0, scale1_1);
            cgal_polyline_util::extend(pline1, 2, scale1_1, scale1_0);

            // extend vertical
            cgal_polyline_util::extend(pline0, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline0, 3, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline1, 1, jo.scale[1], jo.scale[1]);
            cgal_polyline_util::extend(pline1, 3, jo.scale[1], jo.scale[1]);
        }

        /////////////////////////////////////////////////////////////////////////////////
        // move outlines by vector
        /////////////////////////////////////////////////////////////////////////////////
        CGAL_Polyline pline0_moved0 = pline0; // side 0
        CGAL_Polyline pline0_moved1 = pline0; // side 0
        CGAL_Polyline pline1_moved = pline1;  // side 1

        IK::Vector_3 f0_1_normal = f0_0_normal;
        cgal_vector_util::unitize(f0_1_normal);
        f0_1_normal *= (jo.scale[2] + 2) + jo.shift; // Forced offset for safety

        // Move twice to remove one side and the cut surface around
        cgal_polyline_util::move(pline0_moved0, f0_0_normal);
        cgal_polyline_util::move(pline0_moved1, f0_1_normal);

        // Move once to remove the side and the cut the female joint
        cgal_polyline_util::move(pline1_moved, f1_0_normal);

        /////////////////////////////////////////////////////////////////////////////////
        // orient a tile
        // 1) Create rectangle between two edge of the side
        // 2) Create wood::joint in XY plane and orient it to the two rectangles
        // 3) Clipper boolean difference, cut wood::joint polygon form the outline
        /////////////////////////////////////////////////////////////////////////////////

        // 1) Create rectangle between two edge of the side
        IK::Point_3 edge_mid_0 = CGAL::midpoint(CGAL::midpoint(pline0[0], pline1[0]), CGAL::midpoint(pline0[1], pline1[1]));
        IK::Point_3 edge_mid_1 = CGAL::midpoint(CGAL::midpoint(pline0[3], pline1[3]), CGAL::midpoint(pline0[2], pline1[2]));
        double half_dist = std::sqrt(CGAL::squared_distance(edge_mid_0, edge_mid_1)) * 0.5;
        half_dist = 10; // Change to scale

        IK::Vector_3 z_axis = f0_0_normal;
        cgal_vector_util::unitize(z_axis);

        z_axis *= jo.scale[2] / half_dist;

        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // Get average line
        IK::Segment_3 average_line;
        cgal_polyline_util::line_line_overlap_average(jo.joint_lines[0], jo.joint_lines[1], average_line);
        // viewer_polylines.emplace_back(CGAL_Polyline({average_line[0], average_line[1]}));

        // Get average thickness
        double half_thickness = (elements[jo.v0].thickness + elements[jo.v1].thickness) / 4.0;

        // Move points up and down using cross product
        auto x_axis = CGAL::cross_product(z_axis, average_line.to_vector());
        cgal_vector_util::unitize(x_axis);

        IK::Point_3 p0 = CGAL::midpoint(average_line[0], average_line[1]) + x_axis * half_thickness;
        IK::Point_3 p1 = CGAL::midpoint(average_line[0], average_line[1]) - x_axis * half_thickness;
        if (CGAL::has_smaller_distance_to_point(CGAL::midpoint(pline0[0], pline0[1]), p0, p1))
            std::swap(p0, p1);

        // set y-axis
        auto y_axis = average_line.to_vector();
        cgal_vector_util::unitize(y_axis);
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        //
        // y_axis = result.to_vector();
        // y_axis = (IK::Segment_3(jo.joint_lines[1][0], jo.joint_lines[1][1])).to_vector();
        // cgal_vector_util::unitize(y_axis);

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
        // viewer_polylines.emplace_back(rect_mid_0);
        // viewer_polylines.emplace_back(rect_mid_1);
        // rect0 = rect_mid_0;
        // rect1 = rect_mid_1;

        /////////////////////////////////////////////////////////////////////////////////
        // output, no need to merge if already cut
        //  vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
        /////////////////////////////////////////////////////////////////////////////////

        CGAL_Polyline pline0_moved0_surfacing_tolerance_male = pline0_moved0;
        // cgal_polyline_util::move(pline0_moved0_surfacing_tolerance_male, z_axis * 0.20);

        // viewer_polylines.emplace_back(copypline);
        if (jo.shift > 0 && merge_with_joint)
        {
            jo.m[0] = {
                // rect0,
                // rect0,

                pline0_moved0_surfacing_tolerance_male,
                pline0_moved0_surfacing_tolerance_male,
                pline0,
                pline0,
            };

            jo.m[1] = {
                // rect1,
                // rect1,

                pline0_moved1,
                pline0_moved1,
                pline0_moved0,
                pline0_moved0,
            };
        }
        else
        {
            jo.m[0] = {
                // rect0,
                // rect0
                // pline0_moved0,
                // pline0_moved0
                pline0,
                pline0 };

            jo.m[1] = {
                // rect1,
                // rect1
                // pline0_moved1,
                // pline0_moved1
                pline0_moved0,
                pline0_moved0 };
        }

        jo.f[0] = {
            // rect0,
            // rect0
            pline1,
            pline1 };

        jo.f[1] = {
            //  rect1,
            //  rect1
            pline1_moved,
            pline1_moved
            // pline1_moved,
            // pline1_moved
        };

        if (jo.shift > 0 && merge_with_joint)
            jo.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project, wood_cut::mill_project, wood_cut::mill_project };
        else
            jo.m_boolean_type = { wood_cut::mill_project, wood_cut::mill_project };

        jo.f_boolean_type = { wood_cut::mill_project, wood_cut::mill_project };

        /////////////////////////////////////////////////////////////////////////////////
        // if merge is needed
        //  vector1.insert( vector1.end(), vector2.begin(), vector2.end() );
        /////////////////////////////////////////////////////////////////////////////////
        if (merge_with_joint)
        {

            // 2) Create wood::joint in XY plane and orient it to the two rectangles
            wood::joint joint_2;
            // bool read_successful = internal::read_xml(joint_2, jo.type);
            ss_e_r_1(joint_2);
            bool read_successful = true;

            joint_2.unit_scale = true;
            // joint_2.unit_scale_distance = 10;
            // printf("xml tile reading number of polyines %iz", joint_2.m[0].size());
            if (read_successful)
            {
                joint_2.joint_volumes[0] = rect0;
                joint_2.joint_volumes[1] = rect1;
                joint_2.joint_volumes[2] = rect0;
                joint_2.joint_volumes[3] = rect1;
                joint_2.orient_to_connection_area(); // and orient to connection volume

                IK::Plane_3 plane_0_0(jo.m[0][0][0], elements[jo.v0].planes[jo.f0_0].orthogonal_vector());
                IK::Plane_3 plane_0_1(jo.m[0][2][0], elements[jo.v0].planes[jo.f0_0].orthogonal_vector());
                // wood_cut::conical offset
                double dist_two_outlines = std::sqrt(CGAL::squared_distance(jo.m[0][0][0], plane_0_1.projection(jo.m[0][0][0])));
                double conic_offset = -cgal_math_util::triangle_edge_by_angle(dist_two_outlines, 15.0);
                double conic_offset_opposite = -(0.8440 + conic_offset);
                // wood_cut::conic_offset = 0.8440;
                // printf("\n %f", wood_cut::conic_offset);
                // printf("\n %f", wood_cut::conic_offset_opposite);

                double offset_value = -(1.0 * conic_offset_opposite) - conic_offset; // was 1.5 ERROR possible here, check in real prototype

                for (int i = 0; i < joint_2.m[0].size(); i++)
                {
                    // cgal_polyline_util::reverse_if_clockwise(joint_2.f[0][i], plane_0_0);
                    clipper_util::offset_in_3d(joint_2.m[0][i], plane_0_0, offset_value); // 0.1 means more tighter
                    // double value = -(2 * wood_cut::conic_offset_opposite) - wood_cut::conic_offset;
                    // printf("reult %f ", offset_value);
                    //  printf("reult %f ", wood_cut::conic_offset);
                    // printf("reult %f ", wood_cut::conic_offset_opposite);
                    // printf("reult %f ", wood_cut::conic_offset +wood_cut::conic_offset_opposite);//shOULD BE THIS
                }

                for (int i = 0; i < joint_2.m[1].size(); i++)
                {
                    // cgal_polyline_util::reverse_if_clockwise(joint_2.f[1][i], plane_0_0);
                    clipper_util::offset_in_3d(joint_2.m[1][i], plane_0_0, offset_value);
                }

                // printf("xml tile reading number of polyines %iz" , joint_2.m[0].size());

                // 3) Clipper boolean difference, cut wood::joint polygon form the outline

                // Merge male, by performing boolean union

                clipper_util::get_intersection_between_two_polylines(jo.m[0][2], joint_2.m[0][0], plane_0_0, jo.m[0][2], 2);
                clipper_util::get_intersection_between_two_polylines(jo.m[1][2], joint_2.m[1][0], plane_0_1, jo.m[1][2], 2);

                jo.m[0].insert(jo.m[0].end(), joint_2.m[0].begin(), joint_2.m[0].end());
                jo.m[1].insert(jo.m[1].end(), joint_2.m[1].begin(), joint_2.m[1].end());
                for (auto& m : joint_2.m_boolean_type)
                    jo.m_boolean_type.emplace_back(wood_cut::conic);

                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // offset curve due to wood_cut::conic tool
                ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                for (int i = 0; i < joint_2.f[0].size(); i++)
                {
                    // cgal_polyline_util::reverse_if_clockwise(joint_2.f[0][i], plane_0_0);
                    // clipper_util::offset_in_3d(joint_2.f[0][i], plane_0_0, wood_cut::conic_offset_opposite);//- wood_cut::conic_offset
                }

                for (int i = 0; i < joint_2.f[1].size(); i++)
                {
                    // clipper_util::offset_in_3d(joint_2.f[1][i], plane_0_0, wood_cut::conic_offset_opposite);// - wood_cut::conic_offset
                }

                // Add once for milling
                jo.f[0].insert(jo.f[0].end(), joint_2.f[0].begin(), joint_2.f[0].end());
                jo.f[1].insert(jo.f[1].end(), joint_2.f[1].begin(), joint_2.f[1].end());

                for (auto& f : joint_2.f_boolean_type)
                    jo.f_boolean_type.emplace_back(wood_cut::mill);

                jo.f[0].insert(jo.f[0].end(), joint_2.f[0].begin(), joint_2.f[0].end());
                jo.f[1].insert(jo.f[1].end(), joint_2.f[1].begin(), joint_2.f[1].end());
                for (auto& f : joint_2.f_boolean_type)
                    jo.f_boolean_type.emplace_back(wood_cut::conic_reverse);
            }

            std::swap(jo.m[0], jo.f[0]);
            std::swap(jo.m[1], jo.f[1]);
            std::swap(jo.m_boolean_type[1], jo.f_boolean_type[1]);
        }
    }


    void ss_e_r_custom(wood::joint& joint)
    {

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_r_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_r_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_ss_e_r_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_r_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ss_e_r_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_ss_e_r_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_r_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_ss_e_r_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_r_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ss_e_r_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Top-to-side edge plane joints 20-29
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void ts_e_p_0(wood::joint& joint)
    {
        joint.name = __func__;

        joint.f[0] = { {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, -0.0714285714285713), IK::Point_3(-0.5, -0.5, 0.0714285714285715)},
                      {IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, -0.214285714285714)},
                      {IK::Point_3(-0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285713), IK::Point_3(0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, 0.0714285714285713)},
                      {IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714)},
                      {IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, 0.5, 0.357142857142857)} };

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
        joint.m[0] = { {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285715), IK::Point_3(0.5, 0.5, -0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
                      {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)} };

        joint.f_boolean_type = { wood_cut::hole, wood_cut::hole, wood_cut::hole, wood_cut::hole };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ts_e_p_1(wood::joint& joint)
    {
        joint.name = __func__; // Annen

        joint.f[0] = { {IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.277777777777778)},
                      {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.0555555555555556), IK::Point_3(-0.5, -0.5, 0.166666666666667)},
                      {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, -0.5, 0.166666666666667)} };

        joint.f[1] = { {IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, 0.5, -0.277777777777778)},
                      {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.0555555555555556), IK::Point_3(-0.5, 0.5, 0.166666666666667)},
                      {IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667)} };

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
        joint.m[0] = { {IK::Point_3(0.5, -0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.166666666666667), IK::Point_3(0.5, 0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, 0.0555555555555556), IK::Point_3(0.5, -0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.277777777777778), IK::Point_3(0.5, 0.5, -0.388888888888889), IK::Point_3(0.5, -0.5, -0.388888888888889)},
                      {IK::Point_3(0.5, -0.5, 0.5), IK::Point_3(0.5, -0.5, -0.5)} };

        joint.m[1] = { {IK::Point_3(-0.5, -0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.166666666666667), IK::Point_3(-0.5, 0.5, 0.0555555555555558), IK::Point_3(-0.5, -0.5, 0.0555555555555557), IK::Point_3(-0.5, -0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.277777777777778), IK::Point_3(-0.5, 0.5, -0.388888888888889), IK::Point_3(-0.5, -0.5, -0.388888888888889)},
                      {IK::Point_3(-0.5, -0.5, 0.5), IK::Point_3(-0.5, -0.5, -0.5)} };

        joint.f_boolean_type = { wood_cut::hole, wood_cut::hole, wood_cut::hole, wood_cut::hole };
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };

        // if (orient_to_connection_zone)
        // joint.orient_to_connection_area();
    }

    void ts_e_p_2(wood::joint& joint)
    {
        joint.name = __func__;

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input wood::joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////
        // printf("\n JOINT DIVISIONS \n");
        // CGAL_Debug(joint.divisions);
        // printf("\n JOINT DIVISIONS \n");
        int divisions = (int)std::max(2, std::min(20, joint.divisions));
        divisions += divisions % 2;

        // Resize arrays
        int size = (int)(divisions * 0.5) + 1;

        joint.f[0].reserve(size);
        joint.f[1].reserve(size);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

        ////////////////////////////////////////////////////////////////////
        // Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];

        internal::interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
        internal::interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
        internal::interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
        internal::interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

        ////////////////////////////////////////////////////////////////////
        // Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, internal::remap_numbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++)
        {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++)
            {
                int flip = (j % 2 == 0) ? 1 : -1;
                flip = i < 2 ? flip : flip * -1;

                arrays[i][j] += v * flip;
            }
        }

        ////////////////////////////////////////////////////////////////////
        // Create Polylines
        ////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else
                {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2)
            {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
            else
            {
                joint.m[0] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            }
        }

        for (int i = 0; i < joint.m[0][0].size(); i += 4)
        {
            joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
            joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
        }
        joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
        joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

        joint.f_boolean_type = std::vector<wood_cut::cut_type>(size, wood_cut::hole);
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };
    }

    void ts_e_p_3(wood::joint& joint)
    {
        joint.name = __func__;

        ////////////////////////////////////////////////////////////////////
        // Number of divisions
        // Input wood::joint line (its lengths)
        // Input distance for division
        ////////////////////////////////////////////////////////////////////
        int divisions = (int)std::max(8, std::min(100, joint.divisions));
        // CGAL_Debug(divisions);
        // if (joint.tile_parameters.size() > 0)
        //    divisions = joint.tile_parameters[0];
        // CGAL_Debug(divisions);
        // CGAL_Debug(divisions);
        divisions -= divisions % 4;
        // CGAL_Debug(divisions);

        // if ((divisions / 4) % 2 == 1)
        //  divisions += 4;

        // Resize arrays
        int size = (int)(divisions * 0.25) + 1;
        // CGAL_Debug(divisions);

        // divisions += divisions % 4;

        joint.f[0].reserve(size);
        joint.f[1].reserve(size);
        joint.m[0].reserve(2);
        joint.m[1].reserve(2);

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> wood_joint_library.h <<Method>> ts_e_p_3 <<Description>> Joint Main Parameters");
#endif

        ////////////////////////////////////////////////////////////////////
        // Interpolate points
        ////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> arrays[4];
        if (divisions == 0)
            return;
        internal::interpolate_points(IK::Point_3(0.5, -0.5, -0.5), IK::Point_3(0.5, -0.5, 0.5), divisions, false, arrays[3]);
        internal::interpolate_points(IK::Point_3(-0.5, -0.5, -0.5), IK::Point_3(-0.5, -0.5, 0.5), divisions, false, arrays[0]);
        internal::interpolate_points(IK::Point_3(-0.5, 0.5, -0.5), IK::Point_3(-0.5, 0.5, 0.5), divisions, false, arrays[1]);
        internal::interpolate_points(IK::Point_3(0.5, 0.5, -0.5), IK::Point_3(0.5, 0.5, 0.5), divisions, false, arrays[2]);

        // CGAL_Debug(divisions);
        // CGAL_Debug(arrays[0].size());
        // CGAL_Debug(arrays[1].size());
        // CGAL_Debug(arrays[2].size());
        // CGAL_Debug(arrays[3].size());

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> wood_joint_library.h <<Method>> ts_e_p_3 <<Description> Interpolate Points");
#endif

        ////////////////////////////////////////////////////////////////////
        // Move segments
        ////////////////////////////////////////////////////////////////////
        int start = 0;

        IK::Vector_3 v = joint.shift == 0 ? IK::Vector_3(0, 0, 0) : IK::Vector_3(0, 0, internal::remap_numbers(joint.shift, 0, 1.0, -0.5, 0.5) / (divisions + 1));
        for (int i = start; i < 4; i++)
        {
            int mid = (int)(arrays[i].size() * 0.5);

            for (int j = 0; j < arrays[i].size(); j++)
            {
                int flip = (j % 2 == 0) ? 1 : -1;
                flip = i < 2 ? flip : flip * -1;

                arrays[i][j] += v * flip;
            }
        }

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> wood_joint_library.h <<Method>> ts_e_p_3 <<Description> Move Segments");
#endif

        ////////////////////////////////////////////////////////////////////
        // Create Polylines
        ////////////////////////////////////////////////////////////////////

        // CGAL_Debug(0);

        for (int i = 0; i < 4; i += 2)
        {
            CGAL_Polyline pline;
            pline.reserve(arrays[0].size() * 2);

            // CGAL_Debug(1);

            for (int j = 0; j < arrays[0].size(); j++)
            {
                if (j % 4 > 1)
                    continue;

                bool flip = j % 2 == 0;
                flip = i < 2 ? flip : !flip;

                if (flip)
                {
                    pline.push_back(arrays[i + 0][j]);
                    pline.push_back(arrays[i + 1][j]);
                }
                else
                {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            // CGAL_Debug(2);

            if (arrays->size() == 0)
            {
                printf("\n empty");
                return;
            }

            if (i < 2)
            {
                // CGAL_Debug(3);
                // CGAL_Debug(arrays->size());
                // CGAL_Debug(arrays[0].size());
                pline.push_back(arrays[i + 0][arrays[0].size() - 1]);
                // CGAL_Debug(4);
                joint.m[1] = { pline, {pline[0], pline[pline.size() - 1]} };
                // CGAL_Debug(5);
            }
            else
            {
                // CGAL_Debug(6);
                pline.push_back(arrays[i + 1][arrays[0].size() - 1]);
                // CGAL_Debug(7);
                joint.m[0] = { pline, {pline[0], pline[pline.size() - 1]} };
                // CGAL_Debug(8);
            }
            // CGAL_Debug(9);
        }
        // CGAL_Debug(10);

        for (int i = 0; i < joint.m[0][0].size() - joint.m[0][0].size() % 4; i += 4)
        {
            joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 0], joint.m[0][0][i + 3], joint.m[1][0][i + 3], joint.m[1][0][i + 0], joint.m[0][0][i + 0]});
            joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.m[0][0][i + 1], joint.m[0][0][i + 2], joint.m[1][0][i + 2], joint.m[1][0][i + 1], joint.m[0][0][i + 1]});
        }
        joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0][0][0], joint.f[0][0][3], joint.f[0][size - 2][3], joint.f[0][size - 2][0], joint.f[0][0][0]});
        joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1][0][0], joint.f[1][0][3], joint.f[1][size - 2][3], joint.f[1][size - 2][0], joint.f[1][0][0]});

        // CGAL_Debug(30);
        joint.f_boolean_type = std::vector<wood_cut::cut_type>(size, wood_cut::hole);
        joint.m_boolean_type = { wood_cut::edge_insertion, wood_cut::edge_insertion };

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP <<File>> wood_joint_library.h <<Method>> ts_e_p_3 <<Description> Create Polylines");
#endif
    }

    void ts_e_p_4(wood::joint& joint)
    {
        joint.name = __func__;

        joint.f[0] = {

            {
                IK::Point_3(0.64333, -0.78666, -0.01),
                IK::Point_3(0.375, -0.25, -0.01),
                IK::Point_3(-0.375, -0.25, -0.01),
                IK::Point_3(-0.64333, -0.78666, -0.01),
                IK::Point_3(0.64333, -0.78666, -0.01),
            },

            {
                IK::Point_3(0.64333, -0.78666, -0.01),
                IK::Point_3(0.375, -0.25, -0.01),
                IK::Point_3(-0.375, -0.25, -0.01),
                IK::Point_3(-0.64333, -0.78666, -0.01),
                IK::Point_3(0.64333, -0.78666, -0.01),
            },

            {
                IK::Point_3(0.64333, -0.78666, 0.01),
                IK::Point_3(0.375, -0.25, 0.01),
                IK::Point_3(-0.375, -0.25, 0.01),
                IK::Point_3(-0.64333, -0.78666, 0.01),
                IK::Point_3(0.64333, -0.78666, 0.01),
            },

            {
                IK::Point_3(0.64333, -0.78666, 0.01),
                IK::Point_3(0.375, -0.25, 0.01),
                IK::Point_3(-0.375, -0.25, 0.01),
                IK::Point_3(-0.64333, -0.78666, 0.01),
                IK::Point_3(0.64333, -0.78666, 0.01),
            },

            {
                IK::Point_3(-0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, 0.15),
            },

            {
                IK::Point_3(-0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, 0.15),
            },

            {
                IK::Point_3(-0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, 0.15),
            },

            {
                IK::Point_3(-0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, 0.15),
                IK::Point_3(0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, -0.15),
                IK::Point_3(-0.375, 0.075, 0.15),
            },

        };

        joint.f[1] = {

            {
                IK::Point_3(0.64333, -0.78666, 0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.64333, -0.78666, 0.5),
                IK::Point_3(0.64333, -0.78666, 0.5),
            },

            {
                IK::Point_3(0.64333, -0.78666, 0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.64333, -0.78666, 0.5),
                IK::Point_3(0.64333, -0.78666, 0.5),
            },

            {
                IK::Point_3(0.64333, -0.78666, -0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.64333, -0.78666, -0.5),
                IK::Point_3(0.64333, -0.78666, -0.5),
            },

            {
                IK::Point_3(0.64333, -0.78666, -0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.64333, -0.78666, -0.5),
                IK::Point_3(0.64333, -0.78666, -0.5),
            },

            {
                IK::Point_3(-0.375, -0.35, 0.15),
                IK::Point_3(0.375, -0.35, 0.15),
                IK::Point_3(0.375, -0.35, -0.15),
                IK::Point_3(-0.375, -0.35, -0.15),
                IK::Point_3(-0.375, -0.35, 0.15),
            },

            {
                IK::Point_3(-0.375, -0.35, 0.15),
                IK::Point_3(0.375, -0.35, 0.15),
                IK::Point_3(0.375, -0.35, -0.15),
                IK::Point_3(-0.375, -0.35, -0.15),
                IK::Point_3(-0.375, -0.35, 0.15),
            },

            {
                IK::Point_3(-0.375, 0.5, 0.15),
                IK::Point_3(0.375, 0.5, 0.15),
                IK::Point_3(0.375, 0.5, -0.15),
                IK::Point_3(-0.375, 0.5, -0.15),
                IK::Point_3(-0.375, 0.5, 0.15),
            },

            {
                IK::Point_3(-0.375, 0.5, 0.15),
                IK::Point_3(0.375, 0.5, 0.15),
                IK::Point_3(0.375, 0.5, -0.15),
                IK::Point_3(-0.375, 0.5, -0.15),
                IK::Point_3(-0.375, 0.5, 0.15),
            },

        };

        joint.m[0] = {

            {
                IK::Point_3(-0.59861, -0.69721, 0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.59861, -0.69721, -0.5),
                IK::Point_3(-0.59861, -0.69721, 0.5),
            },

            {
                IK::Point_3(-0.59861, -0.69721, 0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.59861, -0.69721, -0.5),
                IK::Point_3(-0.59861, -0.69721, 0.5),
            },

            {
                IK::Point_3(0.59861, -0.69721, 0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(0.59861, -0.69721, -0.5),
                IK::Point_3(0.59861, -0.69721, 0.5),
            },

            {
                IK::Point_3(0.59861, -0.69721, 0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(0.59861, -0.69721, -0.5),
                IK::Point_3(0.59861, -0.69721, 0.5),
            },

            {
                IK::Point_3(-0.375, 0.7, 0.5),
                IK::Point_3(-0.375, 0.7, -0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.375, 0.7, 0.5),
            },

            {
                IK::Point_3(-0.375, 0.7, 0.5),
                IK::Point_3(-0.375, 0.7, -0.5),
                IK::Point_3(-0.375, -0.25, -0.5),
                IK::Point_3(-0.375, -0.25, 0.5),
                IK::Point_3(-0.375, 0.7, 0.5),
            },

            {
                IK::Point_3(0.375, 0.7, 0.5),
                IK::Point_3(0.375, 0.7, -0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(0.375, 0.7, 0.5),
            },

            {
                IK::Point_3(0.375, 0.7, 0.5),
                IK::Point_3(0.375, 0.7, -0.5),
                IK::Point_3(0.375, -0.25, -0.5),
                IK::Point_3(0.375, -0.25, 0.5),
                IK::Point_3(0.375, 0.7, 0.5),
            },

            {
                IK::Point_3(-0.5, 0.7, 0.15),
                IK::Point_3(0.5, 0.7, 0.15),
                IK::Point_3(0.5, -0.25, 0.15),
                IK::Point_3(-0.5, -0.25, 0.15),
                IK::Point_3(-0.5, 0.7, 0.15),
            },

            {
                IK::Point_3(-0.5, 0.7, 0.15),
                IK::Point_3(0.5, 0.7, 0.15),
                IK::Point_3(0.5, -0.25, 0.15),
                IK::Point_3(-0.5, -0.25, 0.15),
                IK::Point_3(-0.5, 0.7, 0.15),
            },

            {
                IK::Point_3(-0.5, 0.7, -0.15),
                IK::Point_3(0.5, 0.7, -0.15),
                IK::Point_3(0.5, -0.25, -0.15),
                IK::Point_3(-0.5, -0.25, -0.15),
                IK::Point_3(-0.5, 0.7, -0.15),
            },

            {
                IK::Point_3(-0.5, 0.7, -0.15),
                IK::Point_3(0.5, 0.7, -0.15),
                IK::Point_3(0.5, -0.25, -0.15),
                IK::Point_3(-0.5, -0.25, -0.15),
                IK::Point_3(-0.5, 0.7, -0.15),
            },

            {
                IK::Point_3(0.05, 0.7, -0.6),
                IK::Point_3(0.05, 0.8, -0.6),
                IK::Point_3(0.05, 0.8, 0.6),
                IK::Point_3(0.05, 0.7, 0.6),
                IK::Point_3(0.05, 0.7, -0.6),
            },

            {
                IK::Point_3(0.05, 0.7, -0.6),
                IK::Point_3(0.05, 0.8, -0.6),
                IK::Point_3(0.05, 0.8, 0.6),
                IK::Point_3(0.05, 0.7, 0.6),
                IK::Point_3(0.05, 0.7, -0.6),
            },

            {
                IK::Point_3(-0.05, 0.7, -0.6),
                IK::Point_3(-0.05, 0.8, -0.6),
                IK::Point_3(-0.05, 0.8, 0.6),
                IK::Point_3(-0.05, 0.7, 0.6),
                IK::Point_3(-0.05, 0.7, -0.6),
            },

            {
                IK::Point_3(-0.05, 0.7, -0.6),
                IK::Point_3(-0.05, 0.8, -0.6),
                IK::Point_3(-0.05, 0.8, 0.6),
                IK::Point_3(-0.05, 0.7, 0.6),
                IK::Point_3(-0.05, 0.7, -0.6),
            },

        };

        joint.m[1] = {

            {
                IK::Point_3(-0.95638, -0.51833, 0.5),
                IK::Point_3(-0.73277, -0.07111, 0.5),
                IK::Point_3(-0.73277, -0.07111, -0.5),
                IK::Point_3(-0.95638, -0.51833, -0.5),
                IK::Point_3(-0.95638, -0.51833, 0.5),
            },

            {
                IK::Point_3(-0.95638, -0.51833, 0.5),
                IK::Point_3(-0.73277, -0.07111, 0.5),
                IK::Point_3(-0.73277, -0.07111, -0.5),
                IK::Point_3(-0.95638, -0.51833, -0.5),
                IK::Point_3(-0.95638, -0.51833, 0.5),
            },

            {
                IK::Point_3(0.95638, -0.51833, 0.5),
                IK::Point_3(0.73277, -0.07111, 0.5),
                IK::Point_3(0.73277, -0.07111, -0.5),
                IK::Point_3(0.95638, -0.51833, -0.5),
                IK::Point_3(0.95638, -0.51833, 0.5),
            },

            {
                IK::Point_3(0.95638, -0.51833, 0.5),
                IK::Point_3(0.73277, -0.07111, 0.5),
                IK::Point_3(0.73277, -0.07111, -0.5),
                IK::Point_3(0.95638, -0.51833, -0.5),
                IK::Point_3(0.95638, -0.51833, 0.5),
            },

            {
                IK::Point_3(-0.675, 0.7, 0.5),
                IK::Point_3(-0.675, 0.7, -0.5),
                IK::Point_3(-0.675, -0.25, -0.5),
                IK::Point_3(-0.675, -0.25, 0.5),
                IK::Point_3(-0.675, 0.7, 0.5),
            },

            {
                IK::Point_3(-0.675, 0.7, 0.5),
                IK::Point_3(-0.675, 0.7, -0.5),
                IK::Point_3(-0.675, -0.25, -0.5),
                IK::Point_3(-0.675, -0.25, 0.5),
                IK::Point_3(-0.675, 0.7, 0.5),
            },

            {
                IK::Point_3(0.675, 0.7, 0.5),
                IK::Point_3(0.675, 0.7, -0.5),
                IK::Point_3(0.675, -0.25, -0.5),
                IK::Point_3(0.675, -0.25, 0.5),
                IK::Point_3(0.675, 0.7, 0.5),
            },

            {
                IK::Point_3(0.675, 0.7, 0.5),
                IK::Point_3(0.675, 0.7, -0.5),
                IK::Point_3(0.675, -0.25, -0.5),
                IK::Point_3(0.675, -0.25, 0.5),
                IK::Point_3(0.675, 0.7, 0.5),
            },

            {
                IK::Point_3(-0.5, 0.7, 0.55),
                IK::Point_3(0.5, 0.7, 0.55),
                IK::Point_3(0.5, -0.25, 0.55),
                IK::Point_3(-0.5, -0.25, 0.55),
                IK::Point_3(-0.5, 0.7, 0.55),
            },

            {
                IK::Point_3(-0.5, 0.7, 0.55),
                IK::Point_3(0.5, 0.7, 0.55),
                IK::Point_3(0.5, -0.25, 0.55),
                IK::Point_3(-0.5, -0.25, 0.55),
                IK::Point_3(-0.5, 0.7, 0.55),
            },

            {
                IK::Point_3(-0.5, 0.7, -0.55),
                IK::Point_3(0.5, 0.7, -0.55),
                IK::Point_3(0.5, -0.25, -0.55),
                IK::Point_3(-0.5, -0.25, -0.55),
                IK::Point_3(-0.5, 0.7, -0.55),
            },

            {
                IK::Point_3(-0.5, 0.7, -0.55),
                IK::Point_3(0.5, 0.7, -0.55),
                IK::Point_3(0.5, -0.25, -0.55),
                IK::Point_3(-0.5, -0.25, -0.55),
                IK::Point_3(-0.5, 0.7, -0.55),
            },

            {
                IK::Point_3(0.7, 0.7, -0.6),
                IK::Point_3(0.7, 0.8, -0.6),
                IK::Point_3(0.7, 0.8, 0.6),
                IK::Point_3(0.7, 0.7, 0.6),
                IK::Point_3(0.7, 0.7, -0.6),
            },

            {
                IK::Point_3(0.7, 0.7, -0.6),
                IK::Point_3(0.7, 0.8, -0.6),
                IK::Point_3(0.7, 0.8, 0.6),
                IK::Point_3(0.7, 0.7, 0.6),
                IK::Point_3(0.7, 0.7, -0.6),
            },

            {
                IK::Point_3(-0.7, 0.7, -0.6),
                IK::Point_3(-0.7, 0.8, -0.6),
                IK::Point_3(-0.7, 0.8, 0.6),
                IK::Point_3(-0.7, 0.7, 0.6),
                IK::Point_3(-0.7, 0.7, -0.6),
            },

            {
                IK::Point_3(-0.7, 0.7, -0.6),
                IK::Point_3(-0.7, 0.8, -0.6),
                IK::Point_3(-0.7, 0.8, 0.6),
                IK::Point_3(-0.7, 0.7, 0.6),
                IK::Point_3(-0.7, 0.7, -0.6),
            },

        };

        joint.f_boolean_type = { wood_cut::mill, wood_cut::mill, wood_cut::mill, wood_cut::mill, wood_cut::mill, wood_cut::mill, wood_cut::mill, wood_cut::mill };
        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
        };

        // Joint lines, always the last line or rectangle is not a wood::joint but an cutting wood::element
        // joint.m[0] = { {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.357142857142857), IK::Point_3(0.5, 0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.214285714285714), IK::Point_3(0.5, -0.5, -0.0714285714285715), IK::Point_3(0.5, 0.5, -0.0714285714285713), IK::Point_3(0.5, 0.5, 0.0714285714285715), IK::Point_3(0.5, -0.5, 0.0714285714285714), IK::Point_3(0.5, -0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.214285714285714), IK::Point_3(0.5, 0.5, 0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)},
        //         {IK::Point_3(0.5, -0.5, -0.357142857142857), IK::Point_3(0.5, -0.5, 0.357142857142857)} };

        // joint.m[1] = { {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.357142857142857), IK::Point_3(-0.5, 0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.214285714285714), IK::Point_3(-0.5, -0.5, -0.0714285714285715), IK::Point_3(-0.5, 0.5, -0.0714285714285713), IK::Point_3(-0.5, 0.5, 0.0714285714285715), IK::Point_3(-0.5, -0.5, 0.0714285714285714), IK::Point_3(-0.5, -0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.214285714285714), IK::Point_3(-0.5, 0.5, 0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)},
        // {IK::Point_3(-0.5, -0.5, -0.357142857142857), IK::Point_3(-0.5, -0.5, 0.357142857142857)} };

        // joint.f_boolean_type = { '6', '6', '6', '6', '6', '6', '6', '6' , '6', '6' };
        // joint.m_boolean_type = { '6', '6', '6', '6', '6', '6', '6', '6' , '6', '6'  , '4', '4' , '4', '4' };
    }

    void ts_e_p_5(wood::joint& joint)
    {

        // std::cout << "ts_e_p_5" << std::endl;

        joint.name = __func__;

        // name

        // parameters that comes from the joint
        bool default_values = !true;

        double edge_length = !default_values ? std::sqrt(CGAL::squared_distance(joint.joint_lines[0][0], joint.joint_lines[0][1])) : 1000;
        int divisions = !default_values ? joint.divisions : 5;
        double joint_volume_edge_length = !default_values ? std::sqrt(CGAL::squared_distance(joint.joint_volumes[0][1], joint.joint_volumes[0][2])) : 40;
        //  scale down the edge, since wood_joint ->  bool joint::orient_to_connection_area() make the distance between joint volumes equal to 2nd joint volume edge
        edge_length *= joint.scale[2];

        // normalization to the unit space, joint_volume_edge_length is used for parametrization
        double move_length_scaled = edge_length / (divisions * joint_volume_edge_length);
        double total_length_scaled = edge_length / joint_volume_edge_length;

        // movement vectors to translate the unit joint to the end of the edge and then to its middle
        IK::Vector_3 dir(0, 0, 1);
        IK::Vector_3 move_from_center_to_the_end = dir * ((total_length_scaled * 0.5) - (move_length_scaled * 0.5));
        IK::Vector_3 move_length_dir = -dir * move_length_scaled;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Male default shape
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<CGAL_Polyline>
            male_0 = {
                {

                    IK::Point_3(-0.499996349848395, -0.499996349847159, 1.62789509252326),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, 1.62789509252326),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, 1.10464309849792),
                    IK::Point_3(-0.499996349848395, -0.0232556441796868, 1.10464309849801),
                    IK::Point_3(-0.499996349848395, -0.0232556441796339, 0.843017101485296),
                    IK::Point_3(-0.499996349848395, 0.49999634984571, 0.843017101485296),
                    IK::Point_3(-0.499996349848395, 0.49999634984571, 1.19185176416881),
                    IK::Point_3(-0.499996349848395, 0.6038871791861, 1.19185176416881),
                    IK::Point_3(-0.499996349848395, 1.01079104575736, 1.04650398805065),
                    IK::Point_3(-0.499996349848395, 1.01079104575736, 0.261625997012692),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, 0.261625997012692),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, -0.261625997012652),
                    IK::Point_3(-0.499996349848395, 1.01079104575736, -0.261625997012652),
                    IK::Point_3(-0.499996349848395, 1.01079104575736, -1.04650398805062),
                    IK::Point_3(-0.499996349848395, 0.6038871791861, -1.19185176416877),
                    IK::Point_3(-0.499996349848395, 0.49999634984571, -1.19185176416877),
                    IK::Point_3(-0.499996349848395, 0.49999634984571, -0.843017101485257),
                    IK::Point_3(-0.499996349848395, -0.0232556441796339, -0.843017101485257),
                    IK::Point_3(-0.499996349848395, -0.0232556441796868, -1.10464309849797),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, -1.10464309849788),
                    IK::Point_3(-0.499996349848395, -3.40695187221018, -1.62789509252322),
                    IK::Point_3(-0.499996349848395, -0.499996349847159, -1.62789509252322),
                },
                {

                    IK::Point_3(-0.499996349848395, -0.499996349847159, 1.62789509252326),
                    IK::Point_3(-0.499996349848395, -0.499996349847159, -1.62789509252322),
                } };

        std::vector<CGAL_Polyline> male_1 = {
            {

                IK::Point_3(0.499996349844421, -0.49999634984737, 1.62789509252334),
                IK::Point_3(0.499996349844421, -3.40695187221039, 1.62789509252334),
                IK::Point_3(0.499996349844421, -3.40695187221039, 1.10464309849799),
                IK::Point_3(0.499996349844421, -0.0232556441798983, 1.10464309849809),
                IK::Point_3(0.499996349844421, -0.0232556441798454, 0.843017101485375),
                IK::Point_3(0.499996349844421, 0.499996349845499, 0.843017101485375),
                IK::Point_3(0.499996349844421, 0.499996349845499, 1.19185176416889),
                IK::Point_3(0.499996349844421, 0.603887179185889, 1.19185176416889),
                IK::Point_3(0.499996349844421, 1.01079104575715, 1.04650398805073),
                IK::Point_3(0.499996349844421, 1.01079104575715, 0.261625997012771),
                IK::Point_3(0.499996349844421, -3.40695187221039, 0.261625997012771),
                IK::Point_3(0.499996349844421, -3.40695187221039, -0.261625997012573),
                IK::Point_3(0.499996349844421, 1.01079104575715, -0.261625997012573),
                IK::Point_3(0.499996349844421, 1.01079104575715, -1.04650398805054),
                IK::Point_3(0.499996349844421, 0.603887179185889, -1.19185176416869),
                IK::Point_3(0.499996349844421, 0.499996349845499, -1.19185176416869),
                IK::Point_3(0.499996349844421, 0.499996349845499, -0.843017101485177),
                IK::Point_3(0.499996349844421, -0.0232556441798454, -0.843017101485177),
                IK::Point_3(0.499996349844421, -0.0232556441798983, -1.1046430984979),
                IK::Point_3(0.499996349844421, -3.40695187221039, -1.1046430984978),
                IK::Point_3(0.499996349844421, -3.40695187221039, -1.62789509252314),
                IK::Point_3(0.499996349844421, -0.49999634984737, -1.62789509252314),
            },

            {
                IK::Point_3(0.499996349844421, -0.49999634984737, 1.62789509252334),
                IK::Point_3(0.499996349844421, -0.49999634984737, -1.62789509252314),
            } };

        std::vector<wood_cut::cut_type> male_types{
            wood_cut::edge_insertion,
            wood_cut::edge_insertion };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // female default shape
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        std::vector<CGAL_Polyline> female_0 = {
            {

                IK::Point_3(-0.499996349848395, -0.499996349847212, 1.10464309849788),
                IK::Point_3(-0.499996349848395, -0.499996349847212, -1.104643098498),
                IK::Point_3(0.499996349844421, -0.499996349847317, -1.104643098498),
                IK::Point_3(0.499996349844421, -0.499996349847317, 1.10464309849788),
                IK::Point_3(-0.499996349848395, -0.499996349847212, 1.10464309849788),
            },
            {

                IK::Point_3(-0.499996349848395, -0.499996349847212, 1.10464309849788),
                IK::Point_3(-0.499996349848395, -0.499996349847212, -1.104643098498),
                IK::Point_3(0.499996349844421, -0.499996349847317, -1.104643098498),
                IK::Point_3(0.499996349844421, -0.499996349847317, 1.10464309849788),
                IK::Point_3(-0.499996349848395, -0.499996349847212, 1.10464309849788),
            } };

        std::vector<CGAL_Polyline> female_1 = {
            {
                IK::Point_3(-0.499996349848395, 0.499996349845604, 1.104643098498),
                IK::Point_3(-0.499996349848395, 0.499996349845604, -1.10464309849788),
                IK::Point_3(0.499996349844421, 0.499996349845499, -1.10464309849788),
                IK::Point_3(0.499996349844421, 0.499996349845499, 1.104643098498),
                IK::Point_3(-0.499996349848395, 0.499996349845604, 1.104643098498),
            },
            {

                IK::Point_3(-0.499996349848395, 0.499996349845604, 1.104643098498),
                IK::Point_3(-0.499996349848395, 0.499996349845604, -1.10464309849788),
                IK::Point_3(0.499996349844421, 0.499996349845499, -1.10464309849788),
                IK::Point_3(0.499996349844421, 0.499996349845499, 1.104643098498),
                IK::Point_3(-0.499996349848395, 0.499996349845604, 1.104643098498),
            } };

        std::vector<wood_cut::cut_type> female_types{
            wood_cut::hole,
            wood_cut::hole };

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Reserve memory for multiple copies
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m[0].reserve(2);
        joint.m[1].reserve(2);
        joint.m_boolean_type.reserve(2);
        joint.f[0].reserve(2 * divisions);
        joint.f[1].reserve(2 * divisions);
        joint.f_boolean_type.reserve(2 * divisions);

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Copy the default shapes and move them
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.m[0].emplace_back(CGAL_Polyline());
        joint.m[1].emplace_back(CGAL_Polyline());

        joint.m[0].back().reserve(male_0[0].size() * divisions);
        joint.m[1].back().reserve(male_1[0].size() * divisions);

        for (auto i = 0; i < divisions; i++)
        {

            // copy the first outline, be sure that the point order is correct, so that the non-internsecting polyline can be created, else reverse it
            CGAL_Polyline male_moved_0 = male_0[0];
            CGAL_Polyline male_moved_1 = male_1[0];

            // move joints that are positioned at the center to the end of the segment and then back by half of the division length
            for (auto& p : male_moved_0)
                p += move_from_center_to_the_end + move_length_dir * i;

            for (auto& p : male_moved_1)
                p += move_from_center_to_the_end + move_length_dir * i;

            // merge with the main outline
            joint.m[0].back().insert(joint.m[0].back().end(), male_moved_0.begin(), male_moved_0.end());
            joint.m[1].back().insert(joint.m[1].back().end(), male_moved_1.begin(), male_moved_1.end());
        }

        for (auto i = 0; i < divisions; i++)
        {

            // copy the first outline, be sure that the point order is correct, so that the non-internsecting polyline can be created, else reverse it
            CGAL_Polyline female_moved_0 = female_0[0];
            CGAL_Polyline female_moved_1 = female_1[0];

            // move joints that are positioned at the center to the end of the segment and then back by half of the division length
            for (auto& p : female_moved_0)
                p += move_from_center_to_the_end + move_length_dir * i;

            for (auto& p : female_moved_1)
                p += move_from_center_to_the_end + move_length_dir * i;

            // merge with the main outline
            joint.f[0].emplace_back(female_moved_0);
            joint.f[1].emplace_back(female_moved_1);
        }
        joint.f[0].emplace_back(std::initializer_list<IK::Point_3>{joint.f[0].front()[0], joint.f[0].front()[3], joint.f[0].back()[2], joint.f[0].back()[1], joint.f[0].front()[0]});
        joint.f[1].emplace_back(std::initializer_list<IK::Point_3>{joint.f[1].front()[0], joint.f[1].front()[3], joint.f[1].back()[2], joint.f[1].back()[1], joint.f[1].front()[0]});
        joint.f_boolean_type = std::vector<wood_cut::cut_type>(joint.f[0].size(), wood_cut::hole);
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Add the insertion lines
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.m[0].emplace_back(CGAL_Polyline{ joint.m[0].front().front(), joint.m[0].front().back() });
        joint.m[1].emplace_back(CGAL_Polyline{ joint.m[1].front().front(), joint.m[1].front().back() });

        joint.m_boolean_type = male_types;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // is unit scale
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        joint.unit_scale = true;
    }

    void ts_e_p_custom(wood::joint& joint)
    {
        // std::ofstream outputFile("C:/compas_wood/output.txt"); 
        // outputFile << wood_globals::custom_joints_ts_e_p_male.size() << std::endl;
        // outputFile << wood_globals::custom_joints_ts_e_p_female.size() << std::endl;
        // outputFile.close();

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_ts_e_p_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_ts_e_p_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_ts_e_p_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ts_e_p_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_ts_e_p_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_ts_e_p_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_ts_e_p_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_ts_e_p_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ts_e_p_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_ts_e_p_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Cross cutting in-plane joints 30-39
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void cr_c_ip_0(wood::joint& joint)
    {
        joint.name = __func__;

        double scale = 1;
        joint.f[0] = {
            {IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)},
            {IK::Point_3(-0.5, 0.5, scale), IK::Point_3(-0.5, -0.5, scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(-0.5, 0.5, scale)} };

        joint.f[1] = {
            {IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)},
            {IK::Point_3(0.5, 0.5, scale), IK::Point_3(0.5, -0.5, scale), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, scale)} };

        joint.m[0] = {
            {IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)},
            {IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)} };

        joint.m[1] = {
            {IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)},
            {IK::Point_3(0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, -scale), IK::Point_3(-0.5, -0.5, 0), IK::Point_3(0.5, -0.5, 0), IK::Point_3(0.5, -0.5, -scale)} };

        joint.m_boolean_type = { wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges };
        joint.f_boolean_type = { wood_cut::insert_between_multiple_edges, wood_cut::insert_between_multiple_edges };
    }

    void cr_c_ip_1(wood::joint& joint)
    {
        joint.name = __func__;
        // double shift = 0.5;

        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),                                 // center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), // CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),                                 // Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)                              // Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        joint.f[0].reserve(9 * 2);
        // Construct polylines from points
        joint.f[0] = {
            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, // center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, // TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, // TopSide1

            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]}, // BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]}, // BotSide1

            {p[0], p[0 + 12], p[0 + 4], p[0 + 8], p[0]},      // Corner0
            {p[1 + 12], p[1], p[1 + 8], p[1 + 4], p[1 + 12]}, // Corner1
            {p[2], p[2 + 12], p[2 + 4], p[2 + 8], p[2]},      // Corner2
            {p[3 + 12], p[3], p[3 + 8], p[3 + 4], p[3 + 12]}  // Corner3
        };

        // for (int i = 0; i < 9; i++) {
        //     for (auto it = joint.f[0][i].begin(); it != joint.f[0][i].end(); ++it)
        //         *it = it->transform(xform_scale);
        // }

        // Offset and
        // flip polylines
        joint.f[1].reserve(9 * 2);
        joint.m[0].reserve(9 * 2);
        joint.m[1].reserve(9 * 2);

        auto xform = internal::rotation_in_xy_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[9] = { 0.5, 0.4, 0.4, 0.4, 0.4, 0.1, 0.1, 0.1, 0.1 };
        for (int i = 0; i < 9; i++)
        {
            joint.f[1].emplace_back(joint.f[0][i]);

            // offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            internal::unitize(cross);

            // offset
            for (int j = 0; j < joint.f[1][i].size(); j++)
                joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].emplace_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].emplace_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        // duplicate two times
        auto f0 = joint.f[0];
        auto f1 = joint.f[1];
        auto m0 = joint.m[0];
        auto m1 = joint.m[1];
        joint.f[0].clear();
        joint.f[1].clear();
        joint.m[0].clear();
        joint.m[1].clear();
        for (int i = 0; i < 9; i++)
        {
            joint.f[0].emplace_back(f0[i]);
            joint.f[0].emplace_back(f0[i]);

            joint.f[1].emplace_back(f1[i]);
            joint.f[1].emplace_back(f1[i]);

            joint.m[0].emplace_back(m0[i]);
            joint.m[0].emplace_back(m0[i]);

            joint.m[1].emplace_back(m1[i]);
            joint.m[1].emplace_back(m1[i]);
        }

        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,

            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
            wood_cut::slice,
        };
        joint.f_boolean_type = {
            wood_cut::mill_project, wood_cut::mill_project,
            wood_cut::mill_project, wood_cut::mill_project,
            wood_cut::mill_project, wood_cut::mill_project,
            wood_cut::slice, wood_cut::slice,
            wood_cut::slice, wood_cut::slice,
            wood_cut::slice, wood_cut::slice,
            wood_cut::slice, wood_cut::slice,

            wood_cut::slice, wood_cut::slice,
            wood_cut::slice, wood_cut::slice

        };
    }

    void cr_c_ip_2(wood::joint& joint)
    {
        joint.name = __func__;
        // double shift = 0.5;

        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),                                 // center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), // CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),                                 // Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)                              // Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        int n = 5;

        joint.f[0].reserve(n * 2);
        // Construct polylines from points
        joint.f[0] = {

            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, // center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, // wood_cut::slice TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, // wood_cut::slice TopSide1

            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]}, // wood_cut::mill BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]}, // wood_cut::mill BotSide1

            // {IK::Point_3(0.3, 0.041421, -0.928477), IK::Point_3(0.041421, 0.3, 0.928477)},     // wood_cut::drill line
            // {IK::Point_3(-0.3, -0.041421, -0.928477), IK::Point_3(-0.041421, -0.3, 0.928477)}, // wood_cut::drill line

        };

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // extend rectangles to both sides to compensate for irregularities
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

        // to sides

        cgal_polyline_util::extend_equally(joint.f[0][3], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][3], 2, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 2, 0.15);

        // vertically
        cgal_polyline_util::extend_equally(joint.f[0][3], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][3], 3, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 3, 0.6);


        // Offset and
        // flip polylines
        joint.f[1].reserve(n * 2);
        joint.m[0].reserve(n * 2);
        joint.m[1].reserve(n * 2);

        auto xform = internal::rotation_in_xy_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[5] = { 0.5, 0.4, 0.4, 0.4, 0.4 };
        for (int i = 0; i < n; i++)
        {
            joint.f[1].emplace_back(joint.f[0][i]);

            // offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            internal::unitize(cross);

            // offset| skip wood_cut::drill lines
            if (joint.f[1][i].size())//> 2
                for (int j = 0; j < joint.f[1][i].size(); j++)
                    joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].emplace_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].emplace_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // duplicate two times
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto f0 = joint.f[0];
        auto f1 = joint.f[1];
        auto m0 = joint.m[0];
        auto m1 = joint.m[1];
        joint.f[0].clear();
        joint.f[1].clear();
        joint.m[0].clear();
        joint.m[1].clear();

        for (int i = 0; i < n; i++)
        {
            joint.f[0].emplace_back(f0[i]);
            joint.f[0].emplace_back(f0[i]);

            joint.f[1].emplace_back(f1[i]);
            joint.f[1].emplace_back(f1[i]);

            joint.m[0].emplace_back(m0[i]);
            joint.m[0].emplace_back(m0[i]);

            joint.m[1].emplace_back(m1[i]);
            joint.m[1].emplace_back(m1[i]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // take sides of id = 1*2 and id = 2*2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 2; i++)
        {
            int id = (i + 1) * 2;
            CGAL_Polyline side00 = { joint.f[0][id][0], joint.f[0][id][1], joint.f[1][id][1], joint.f[1][id][0], joint.f[0][id][0] };
            CGAL_Polyline side01 = { joint.f[0][id][3], joint.f[0][id][2], joint.f[1][id][2], joint.f[1][id][3], joint.f[0][id][3] };
            joint.f[0][id] = side00;
            joint.f[1][id] = side01;
            joint.f[0][id + 1] = side00;
            joint.f[1][id + 1] = side01;

            side00 = { joint.m[0][id][0], joint.m[0][id][1], joint.m[1][id][1], joint.m[1][id][0], joint.m[0][id][0] };
            side01 = { joint.m[0][id][3], joint.m[0][id][2], joint.m[1][id][2], joint.m[1][id][3], joint.m[0][id][3] };
            joint.m[0][id] = side00;
            joint.m[1][id] = side01;
            joint.m[0][id + 1] = side00;
            joint.m[1][id + 1] = side01;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // cut types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            // wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,
        };
        joint.f_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            // wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,

        };

        // joint.m_boolean_type = {};
        // joint.m[0]= {};
        // joint.m[1]= {};

        // joint.f_boolean_type = {};
        // joint.f[0]= {};
        // joint.f[1]= {};
    }


    void cr_c_ip_3(wood::joint& joint)
    {
        joint.name = __func__;
        // double shift = 0.5;

        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),                                 // center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), // CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),                                 // Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)                              // Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        int n = 7;

        joint.f[0].reserve(n * 2);
        // Construct polylines from points
        joint.f[0] = {

            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, // center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, // wood_cut::slice TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, // wood_cut::slice TopSide1

            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]}, // wood_cut::mill BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]}, // wood_cut::mill BotSide1

            {IK::Point_3(0.3, 0.041421, -0.928477), IK::Point_3(0.041421, 0.3, 0.928477)},     // wood_cut::drill line
            {IK::Point_3(-0.3, -0.041421, -0.928477), IK::Point_3(-0.041421, -0.3, 0.928477)}, // wood_cut::drill line

        };

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // extend rectangles to both sides to compensate for irregularities
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

        // to sides

        cgal_polyline_util::extend_equally(joint.f[0][3], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][3], 2, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 2, 0.15);

        // vertically
        cgal_polyline_util::extend_equally(joint.f[0][3], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][3], 3, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 3, 0.6);


        // Offset and
        // flip polylines
        joint.f[1].reserve(n * 2);
        joint.m[0].reserve(n * 2);
        joint.m[1].reserve(n * 2);

        auto xform = internal::rotation_in_xy_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[5] = { 0.5, 0.4, 0.4, 0.4, 0.4 };
        for (int i = 0; i < n; i++)
        {
            joint.f[1].emplace_back(joint.f[0][i]);

            // offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            internal::unitize(cross);

            // offset| skip wood_cut::drill lines
            if (joint.f[1][i].size() > 2)
                for (int j = 0; j < joint.f[1][i].size(); j++)
                    joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].emplace_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].emplace_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // duplicate two times
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto f0 = joint.f[0];
        auto f1 = joint.f[1];
        auto m0 = joint.m[0];
        auto m1 = joint.m[1];
        joint.f[0].clear();
        joint.f[1].clear();
        joint.m[0].clear();
        joint.m[1].clear();

        for (int i = 0; i < n; i++)
        {
            joint.f[0].emplace_back(f0[i]);
            joint.f[0].emplace_back(f0[i]);

            joint.f[1].emplace_back(f1[i]);
            joint.f[1].emplace_back(f1[i]);

            joint.m[0].emplace_back(m0[i]);
            joint.m[0].emplace_back(m0[i]);

            joint.m[1].emplace_back(m1[i]);
            joint.m[1].emplace_back(m1[i]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // take sides of id = 1*2 and id = 2*2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 2; i++)
        {
            int id = (i + 1) * 2;
            CGAL_Polyline side00 = { joint.f[0][id][0], joint.f[0][id][1], joint.f[1][id][1], joint.f[1][id][0], joint.f[0][id][0] };
            CGAL_Polyline side01 = { joint.f[0][id][3], joint.f[0][id][2], joint.f[1][id][2], joint.f[1][id][3], joint.f[0][id][3] };
            joint.f[0][id] = side00;
            joint.f[1][id] = side01;
            joint.f[0][id + 1] = side00;
            joint.f[1][id + 1] = side01;

            side00 = { joint.m[0][id][0], joint.m[0][id][1], joint.m[1][id][1], joint.m[1][id][0], joint.m[0][id][0] };
            side01 = { joint.m[0][id][3], joint.m[0][id][2], joint.m[1][id][2], joint.m[1][id][3], joint.m[0][id][3] };
            joint.m[0][id] = side00;
            joint.m[1][id] = side01;
            joint.m[0][id + 1] = side00;
            joint.m[1][id + 1] = side01;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // cut types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill,
            wood_cut::drill,
            wood_cut::drill,
            wood_cut::drill,
        };
        joint.f_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill,
            wood_cut::drill,
            wood_cut::drill,
            wood_cut::drill,

        };
    }


    void cr_c_ip_4(wood::joint& joint)
    {
        joint.name = __func__;
        // double shift = 0.5;

        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),                                 // center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), // CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),                                 // Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)                              // Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        int n = 6;

        joint.f[0].reserve(n * 2);
        // Construct polylines from points
        joint.f[0] = {

            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, // center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, // wood_cut::slice TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, // wood_cut::slice TopSide1

            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]}, // wood_cut::mill BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]}, // wood_cut::mill BotSide1

           {IK::Point_3(0.0, 0.0, -1.0), IK::Point_3(0.0, 0.0, 1.0)},     // wood_cut::drill line


           // {IK::Point_3(0.3, 0.041421, -0.928477), IK::Point_3(0.041421, 0.3, 0.928477)},     // wood_cut::drill line
           // {IK::Point_3(-0.3, -0.041421, -0.928477), IK::Point_3(-0.041421, -0.3, 0.928477)}, // wood_cut::drill line

        };

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // extend rectangles to both sides to compensate for irregularities
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

        // to sides

        cgal_polyline_util::extend_equally(joint.f[0][3], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][3], 2, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 0, 0.15);
        cgal_polyline_util::extend_equally(joint.f[0][4], 2, 0.15);

        // vertically
        cgal_polyline_util::extend_equally(joint.f[0][3], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][3], 3, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 3, 0.6);


        // Offset and
        // flip polylines
        joint.f[1].reserve(n * 2);
        joint.m[0].reserve(n * 2);
        joint.m[1].reserve(n * 2);

        auto xform = internal::rotation_in_xy_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[5] = { 0.5, 0.4, 0.4, 0.4, 0.4 };
        for (int i = 0; i < n; i++)
        {
            joint.f[1].emplace_back(joint.f[0][i]);

            // offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            internal::unitize(cross);

            // offset| skip wood_cut::drill lines
            if (joint.f[1][i].size() > 1)//
                for (int j = 0; j < joint.f[1][i].size(); j++)
                    joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].emplace_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].emplace_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // duplicate two times
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto f0 = joint.f[0];
        auto f1 = joint.f[1];
        auto m0 = joint.m[0];
        auto m1 = joint.m[1];
        joint.f[0].clear();
        joint.f[1].clear();
        joint.m[0].clear();
        joint.m[1].clear();

        for (int i = 0; i < n; i++)
        {
            joint.f[0].emplace_back(f0[i]);
            joint.f[0].emplace_back(f0[i]);

            joint.f[1].emplace_back(f1[i]);
            joint.f[1].emplace_back(f1[i]);

            joint.m[0].emplace_back(m0[i]);
            joint.m[0].emplace_back(m0[i]);

            joint.m[1].emplace_back(m1[i]);
            joint.m[1].emplace_back(m1[i]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // take sides of id = 1*2 and id = 2*2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 2; i++)
        {
            int id = (i + 1) * 2;
            CGAL_Polyline side00 = { joint.f[0][id][0], joint.f[0][id][1], joint.f[1][id][1], joint.f[1][id][0], joint.f[0][id][0] };
            CGAL_Polyline side01 = { joint.f[0][id][3], joint.f[0][id][2], joint.f[1][id][2], joint.f[1][id][3], joint.f[0][id][3] };
            joint.f[0][id] = side00;
            joint.f[1][id] = side01;
            joint.f[0][id + 1] = side00;
            joint.f[1][id + 1] = side01;

            side00 = { joint.m[0][id][0], joint.m[0][id][1], joint.m[1][id][1], joint.m[1][id][0], joint.m[0][id][0] };
            side01 = { joint.m[0][id][3], joint.m[0][id][2], joint.m[1][id][2], joint.m[1][id][3], joint.m[0][id][3] };
            joint.m[0][id] = side00;
            joint.m[1][id] = side01;
            joint.m[0][id + 1] = side00;
            joint.m[1][id + 1] = side01;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // cut types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill,
            wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,
        };
        joint.f_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill,
            wood_cut::drill,
            // wood_cut::drill,
            // wood_cut::drill,

        };

        // joint.m_boolean_type = {};
        // joint.m[0]= {};
        // joint.m[1]= {};

        // joint.f_boolean_type = {};
        // joint.f[0]= {};
        // joint.f[1]= {};
    }


    void cr_c_ip_5(wood::joint& joint)
    {
        joint.name = __func__;
        // double shift = 0.5;

        double s = std::max(std::min(joint.shift, 1.0), 0.0);
        s = 0.05 + (s - 0.0) * (0.4 - 0.05) / (1.0 - 0.0);

        double a = 0.5 - s;
        double b = 0.5;
        double c = 2 * (b - a);
        double z = 0.5;

        IK::Point_3 p[16] = {
            IK::Point_3(a, -a, 0), IK::Point_3(-a, -a, 0), IK::Point_3(-a, a, 0), IK::Point_3(a, a, 0),                                 // center
            IK::Point_3(a + c, -a - c, 0), IK::Point_3(-a - c, -a - c, 0), IK::Point_3(-a - c, a + c, 0), IK::Point_3(a + c, a + c, 0), // CenterOffset
            IK::Point_3(b, -b, z), IK::Point_3(-b, -b, z), IK::Point_3(-b, b, z), IK::Point_3(b, b, z),                                 // Top
            IK::Point_3(b, -b, -z), IK::Point_3(-b, -b, -z), IK::Point_3(-b, b, -z), IK::Point_3(b, b, -z)                              // Bottom
        };

        IK::Vector_3 v0 = ((p[0] - p[1]) * (1 / (a * 2))) * (0.5 - a);

        int n = 7;

        joint.f[0].reserve(n * 2);
        // Construct polylines from points
        joint.f[0] = {

            {p[0] + v0, p[1] - v0, p[2] - v0, p[3] + v0, p[0] + v0}, // center

            {p[1] - v0, p[0] + v0, p[0 + 8] + v0, p[1 + 8] - v0, p[1] - v0}, // wood_cut::slice TopSide0
            {p[3] + v0, p[2] - v0, p[2 + 8] - v0, p[3 + 8] + v0, p[3] + v0}, // wood_cut::slice TopSide1

            {p[2], p[1], p[1 + 12], p[2 + 12], p[2]}, // wood_cut::mill BotSide0
            {p[0], p[3], p[3 + 12], p[0 + 12], p[0]}, // wood_cut::mill BotSide1

           {IK::Point_3(0.0, 0.0, -1.0), IK::Point_3(0.0, 0.0, 1.0)},     // wood_cut::drill line
           {IK::Point_3(-0.5, 0, -0.55), IK::Point_3(0.5, 0, -0.55)},
           //{IK::Point_3(0, 1, 1), IK::Point_3(0, -1, 1)},     // wood_cut::drill line (smaller element)

        };

        ///////////////////////////////////////////////////////////////////////////////////////////////////////
        // extend rectangles to both sides to compensate for irregularities
        ///////////////////////////////////////////////////////////////////////////////////////////////////////

        // to sides

        cgal_polyline_util::extend_equally(joint.f[0][3], 0, 0.15 * 1.8);
        cgal_polyline_util::extend_equally(joint.f[0][3], 2, -0.15 * 0.5);
        cgal_polyline_util::extend_equally(joint.f[0][4], 0, 0.15 * 1.8);
        cgal_polyline_util::extend_equally(joint.f[0][4], 2, -0.15 * 0.5);

        // vertically
        cgal_polyline_util::extend_equally(joint.f[0][3], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][3], 3, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 1, 0.6);
        cgal_polyline_util::extend_equally(joint.f[0][4], 3, 0.6);


        // Offset and
        // flip polylines
        joint.f[1].reserve(n * 2);
        joint.m[0].reserve(n * 2);
        joint.m[1].reserve(n * 2);

        auto xform = internal::rotation_in_xy_plane(IK::Vector_3(0, 1, 0), IK::Vector_3(1, 0, 0), IK::Vector_3(0, 0, -1));

        double lenghts[5] = { 0.5, 0.4, 0.4, 0.4, 0.4 };
        for (int i = 0; i < n; i++)
        {
            joint.f[1].emplace_back(joint.f[0][i]);

            // offset distance
            IK::Vector_3 cross = CGAL::cross_product(joint.f[1][i][1] - joint.f[1][i][0], joint.f[1][i][1] - joint.f[1][i][2]);
            internal::unitize(cross);

            // offset| skip wood_cut::drill lines
            if (joint.f[1][i].size() > 2)//
                for (int j = 0; j < joint.f[1][i].size(); j++)
                    joint.f[1][i][j] += cross * lenghts[i];

            joint.m[0].emplace_back(joint.f[0][i]);
            for (auto it = joint.m[0][i].begin(); it != joint.m[0][i].end(); ++it)
                *it = it->transform(xform);

            joint.m[1].emplace_back(joint.f[1][i]);
            for (auto it = joint.m[1][i].begin(); it != joint.m[1][i].end(); ++it)
                *it = it->transform(xform);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // duplicate two times
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        auto f0 = joint.f[0];
        auto f1 = joint.f[1];
        auto m0 = joint.m[0];
        auto m1 = joint.m[1];
        joint.f[0].clear();
        joint.f[1].clear();
        joint.m[0].clear();
        joint.m[1].clear();

        for (int i = 0; i < n; i++)
        {
            joint.f[0].emplace_back(f0[i]);
            joint.f[0].emplace_back(f0[i]);

            joint.f[1].emplace_back(f1[i]);
            joint.f[1].emplace_back(f1[i]);

            joint.m[0].emplace_back(m0[i]);
            joint.m[0].emplace_back(m0[i]);

            joint.m[1].emplace_back(m1[i]);
            joint.m[1].emplace_back(m1[i]);
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // take sides of id = 1*2 and id = 2*2
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        for (int i = 0; i < 2; i++)
        {
            int id = (i + 1) * 2;
            CGAL_Polyline side00 = { joint.f[0][id][0], joint.f[0][id][1], joint.f[1][id][1], joint.f[1][id][0], joint.f[0][id][0] };
            CGAL_Polyline side01 = { joint.f[0][id][3], joint.f[0][id][2], joint.f[1][id][2], joint.f[1][id][3], joint.f[0][id][3] };
            joint.f[0][id] = side00;
            joint.f[1][id] = side01;
            joint.f[0][id + 1] = side00;
            joint.f[1][id + 1] = side01;

            side00 = { joint.m[0][id][0], joint.m[0][id][1], joint.m[1][id][1], joint.m[1][id][0], joint.m[0][id][0] };
            side01 = { joint.m[0][id][3], joint.m[0][id][2], joint.m[1][id][2], joint.m[1][id][3], joint.m[0][id][3] };
            joint.m[0][id] = side00;
            joint.m[1][id] = side01;
            joint.m[0][id + 1] = side00;
            joint.m[1][id + 1] = side01;
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // cut types
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

        joint.m_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill_50,
            wood_cut::drill_50,
            wood_cut::drill_10,
            wood_cut::drill_10,
        };
        joint.f_boolean_type = {
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,
            wood_cut::slice_projectsheer,

            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,
            wood_cut::mill_project,

            wood_cut::drill_50,
            wood_cut::drill_50,
            wood_cut::drill_10,
            wood_cut::drill_10,

        };

        // joint.m_boolean_type = {};
        // joint.m[0]= {};
        // joint.m[1]= {};

        // joint.f_boolean_type = {};
        // joint.f[0]= {};
        // joint.f[1]= {};
    }

    void cr_c_ip_custom(wood::joint& joint)
    {

        // std::ofstream outputFile("C:/compas_wood/output.txt"); 

        joint.name = __func__;
        // outputFile << wood_globals::custom_joints_cr_c_ip_male.size() << std::endl;
        // outputFile << wood_globals::custom_joints_cr_c_ip_female.size() << std::endl;
        // outputFile.close();  // Close the file
        for (size_t i = 0; i < wood_globals::custom_joints_cr_c_ip_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_cr_c_ip_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_cr_c_ip_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_cr_c_ip_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_cr_c_ip_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_cr_c_ip_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_cr_c_ip_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_cr_c_ip_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_cr_c_ip_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_cr_c_ip_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Top-to-top edge plane joints 40-49
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void tt_e_p_0(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the center of the joint_area
        IK::Point_3 center = cgal_polyline_util::center(joint.joint_area);

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // create polyline segments that represents the drilling holes
        CGAL_Polyline line0 = { center, center + dir0 };
        CGAL_Polyline line1 = { center, center + dir1 };

        // output
        joint.f[0] = { line0, line0 };
        joint.f[1] = { line0, line0 };
        joint.m[0] = { line1, line1 };
        joint.m[1] = { line1, line1 };

        joint.m_boolean_type = { wood_cut::drill, wood_cut::drill };
        joint.f_boolean_type = { wood_cut::drill, wood_cut::drill };

        // WARNING: set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }

    void tt_e_p_1(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the inscribed rectangle and subdivide it into points
        // WARNING the offset distance and division has to come from user
        std::tuple<IK::Point_3, IK::Plane_3, double> result = cgal_inscribe_util::get_polylabel({ joint.joint_area }, 1.0);
        IK::Point_3 center = std::get<0>(result);

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // create polyline segments that represents the drilling holes
        CGAL_Polyline line0 = { center, center + dir0 };
        CGAL_Polyline line1 = { center, center + dir1 };

        // output
        joint.f[0] = { line0, line0 };
        joint.f[1] = { line0, line0 };
        joint.m[0] = { line1, line1 };
        joint.m[1] = { line1, line1 };

        joint.m_boolean_type = { wood_cut::drill, wood_cut::drill };
        joint.f_boolean_type = { wood_cut::drill, wood_cut::drill };

        // WARNING: set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }

    void tt_e_p_2(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the inscribed rectangle and subdivide it into points
        // WARNING the offset distance and division has to come from user
        std::vector<IK::Point_3> points;
        double scale = joint.shift;
        double number_of_points = joint.division_length;
        cgal_inscribe_util::get_polylabel_circle_division_points(IK::Vector_3(0, 0, 0), { joint.joint_area }, points, number_of_points, scale, 1.0, true); //  joint.divisions, joint.shift 6, 0.75, 1.0, true

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // output
        joint.f[0].reserve(points.size() * 2);
        joint.f[1].reserve(points.size() * 2);
        joint.m[0].reserve(points.size() * 2);
        joint.m[1].reserve(points.size() * 2);
        joint.m_boolean_type.reserve(points.size() * 2);
        joint.f_boolean_type.reserve(points.size() * 2);
        for (auto& point : points)
        {
            // create polyline segments that represents the drilling holes
            CGAL_Polyline line0 = { point, point + dir0 };
            CGAL_Polyline line1 = { point, point + dir1 };
            joint.f[0].emplace_back(line0);
            joint.f[0].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.m[0].emplace_back(line1);
            joint.m[0].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
        }

        // WARNING set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }

    void tt_e_p_3(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the inscribed rectangle and subdivide it into points
        // WARNING the offset distance and division has to come from user
        std::vector<IK::Point_3> points;
        double offset_distance = -joint.shift;
        double division_distance = joint.division_length; // negative value = grid, positive = edge division
        clipper_util::offset_and_divide_to_points(points, joint.joint_area, offset_distance, division_distance);

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // output
        joint.f[0].reserve(points.size() * 2);
        joint.f[1].reserve(points.size() * 2);
        joint.m[0].reserve(points.size() * 2);
        joint.m[1].reserve(points.size() * 2);
        joint.m_boolean_type.reserve(points.size() * 2);
        joint.f_boolean_type.reserve(points.size() * 2);
        for (auto& point : points)
        {
            // create polyline segments that represents the drilling holes
            CGAL_Polyline line0 = { point, point + dir0 };
            CGAL_Polyline line1 = { point, point + dir1 };
            double offset_distance = -joint.shift; // negative means offset inwards
            double division_distance = joint.division_length;
            joint.f[0].emplace_back(line0);
            joint.f[0].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.m[0].emplace_back(line1);
            joint.m[0].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
        }

        // WARNING set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }

    void tt_e_p_4(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the center of the joint_area
        std::vector<IK::Point_3> points;
        double offset_distance = -joint.shift; // negative means offset inwards
        double division_distance = joint.division_length;
        cgal_rectangle_util::grid_of_points_in_a_polygon(joint.joint_area, offset_distance, division_distance, 100, points);

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // output
        joint.f[0].reserve(points.size() * 2);
        joint.f[1].reserve(points.size() * 2);
        joint.m[0].reserve(points.size() * 2);
        joint.m[1].reserve(points.size() * 2);
        joint.m_boolean_type.reserve(points.size() * 2);
        joint.f_boolean_type.reserve(points.size() * 2);
        for (auto& point : points)
        {
            // create polyline segments that represents the drilling holes
            CGAL_Polyline line0 = { point, point + dir0 };
            CGAL_Polyline line1 = { point, point + dir1 };
            joint.f[0].emplace_back(line0);
            joint.f[0].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.m[0].emplace_back(line1);
            joint.m[0].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
        }

        // WARNING set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }

    void tt_e_p_5(wood::joint& joint, std::vector<wood::element>& elements) //
    {
        joint.name = __func__;

        // compute the inscribed rectangle and subdivide it into points
        // WARNING the division has to come from user
        std::vector<IK::Point_3> points;
        CGAL_Polyline polygon_inscribed_rectangle;
        IK::Segment_3 segment;
        double scale = joint.shift;
        double division_length = joint.division_length; // negative value = grid, positive = edge division
        double precision = 1;
        cgal_inscribe_util::inscribe_rectangle_in_convex_polygon({ joint.joint_area }, polygon_inscribed_rectangle, points, segment, scale, precision, division_length);

        // move the center by the direction of rectangle 2nd edge, because it is vertical
        IK::Vector_3 dir0 = joint.joint_volumes[0][1] - joint.joint_volumes[0][2];
        cgal_vector_util::unitize(dir0);
        IK::Vector_3 dir1 = -dir0;
        dir0 *= elements[joint.v0].thickness;
        dir1 *= elements[joint.v1].thickness;

        // output

        joint.f[0].reserve(points.size() * 2);
        joint.f[1].reserve(points.size() * 2);
        joint.m[0].reserve(points.size() * 2);
        joint.m[1].reserve(points.size() * 2);
        joint.m_boolean_type.reserve(points.size() * 2);
        joint.f_boolean_type.reserve(points.size() * 2);
        // joint.f[0].emplace_back(polygon_inscribed_rectangle);
        // joint.f[0].emplace_back(polygon_inscribed_rectangle);
        // joint.f[1].emplace_back(polygon_inscribed_rectangle);
        // joint.f[1].emplace_back(polygon_inscribed_rectangle);
        // joint.f_boolean_type.emplace_back(wood_cut::mill);
        // joint.f_boolean_type.emplace_back(wood_cut::mill);
        for (auto& point : points)
        {
            // create polyline segments that represents the drilling holes
            CGAL_Polyline line0 = { point, point + dir0 };
            CGAL_Polyline line1 = { point, point + dir1 };
            joint.f[0].emplace_back(line0);
            joint.f[0].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.f[1].emplace_back(line0);
            joint.m[0].emplace_back(line1);
            joint.m[0].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m[1].emplace_back(line1);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.m_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
            joint.f_boolean_type.emplace_back(wood_cut::drill);
        }

        // WARNING set variable so that it wont be recomputed, because it is run only once | also it must not be orientable
        joint.orient = false;
        joint.key += std::to_string(joint.id);
    }


    void tt_e_p_custom(wood::joint& joint)
    {

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_tt_e_p_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_tt_e_p_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_tt_e_p_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_tt_e_p_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_tt_e_p_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_tt_e_p_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_tt_e_p_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_tt_e_p_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_tt_e_p_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_tt_e_p_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Boundary joint 60-69
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void b_0(wood::joint& joint)
    {
        joint.name = __func__;
        joint.orient = false; // prevents wood::joint from copying

        double temp_scale_y = 5;
        double temp_scale_z = 15;
        double offset_from_center = 0.25;
        // printf("\nb_0\n");

        // Get center rectangle
        CGAL_Polyline mid_rectangle = cgal_polyline_util::tween_two_polylines(joint.joint_volumes[0], joint.joint_volumes[1], 0.5);

        // X-Axis extend polyline in scale[0]
        cgal_polyline_util::extend_equally(mid_rectangle, 1, joint.scale[0] + 0);
        cgal_polyline_util::extend_equally(mid_rectangle, 3, joint.scale[0] + 0);

        // Y-Axis Move rectangle down and give it a length of scale[1]
        // move to center
        IK::Vector_3 v = mid_rectangle[1] - mid_rectangle[0];
        v *= 0.5;

        mid_rectangle[0] += v;
        mid_rectangle[1] -= v;
        mid_rectangle[2] -= v;
        mid_rectangle[3] += v;
        mid_rectangle[4] += v;

        cgal_vector_util::unitize(v);
        v *= joint.scale[1] + temp_scale_y;
        mid_rectangle[0] += v;
        mid_rectangle[3] += v;
        mid_rectangle[4] += v;

        // Z-AxisOffset by normal, scale value gives the offset from the center
        IK::Vector_3 z_axis;
        cgal_vector_util::average_normal(mid_rectangle, z_axis);
        IK::Vector_3 z_axis_offset_from_center = z_axis * offset_from_center;
        double len = cgal_vector_util::length(z_axis.x(), z_axis.y(), z_axis.z());

        z_axis *= (joint.scale[2] + temp_scale_z);

        cgal_polyline_util::shift(mid_rectangle, 2);

        CGAL_Polyline rect0 = mid_rectangle;
        CGAL_Polyline rect1 = mid_rectangle;
        CGAL_Polyline rect2 = mid_rectangle;
        CGAL_Polyline rect3 = mid_rectangle;
        cgal_polyline_util::move(rect1, z_axis);
        cgal_polyline_util::move(rect3, -z_axis);
        cgal_polyline_util::move(rect0, z_axis_offset_from_center);
        cgal_polyline_util::move(rect2, -z_axis_offset_from_center);
        // printf("\n length %f %f %f", len, joint.scale[2], temp_scale_z);
        // rect0=cgal_polyline_util::tween_two_polylines(rect0, rect1, 0.25);//joint.shift
        // rect2=cgal_polyline_util::tween_two_polylines(rect2, rect3, 0.25);
        // viewer_polylines.emplace_back(rect0);
        // viewer_polylines.emplace_back(rect1);
        // Create cutting geometry - two rectangles that represents single line cut

        // Add geometry and boolean types
        // viewer_polylines.emplace_back(rect0);
        // viewer_polylines.emplace_back(rect1);
        // viewer_polylines.emplace_back(rect2);
        // viewer_polylines.emplace_back(rect3);
        joint.m[0] = { rect0, rect0, rect2, rect2 };
        joint.m[1] = { rect1, rect1, rect3, rect3 };

        // joint.f[0] = { rect0, rect1 };
        // joint.f[1] = { rect2, rect3 };
        // printf("Hi");
        joint.m_boolean_type = { wood_cut::slice, wood_cut::slice, wood_cut::slice, wood_cut::slice };
        // joint.f_boolean_type = { '1', '1' };
    }

    void b_custom(wood::joint& joint)
    {

        joint.name = __func__;

        for (size_t i = 0; i < wood_globals::custom_joints_b_male.size(); i += 2)
        {
            joint.m[0].emplace_back(wood_globals::custom_joints_b_male[i]);
            joint.m[0].emplace_back(wood_globals::custom_joints_b_male[i]);
            joint.m[1].emplace_back(wood_globals::custom_joints_b_male[i + 1]);
            joint.m[1].emplace_back(wood_globals::custom_joints_b_male[i + 1]);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
            joint.m_boolean_type.emplace_back(wood_cut::nothing);
        }

        for (size_t i = 0; i < wood_globals::custom_joints_b_female.size(); i += 2)
        {
            joint.f[0].emplace_back(wood_globals::custom_joints_b_female[i]);
            joint.f[0].emplace_back(wood_globals::custom_joints_b_female[i]);
            joint.f[1].emplace_back(wood_globals::custom_joints_b_female[i + 1]);
            joint.f[1].emplace_back(wood_globals::custom_joints_b_female[i + 1]);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
            joint.f_boolean_type.emplace_back(wood_cut::nothing);
        }

    }
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Key function that iterates through all joints and calls the function that constructs the joint geometry
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    void construct_joint_by_index(std::vector<wood::element>& elements, std::vector<wood::joint>& joints, std::vector<double>& default_parameters_for_four_types, std::vector<double>& scale)
    {
        /////////////////////////////////////////////////////////////////////
        // You must define new wood::joint each time you internalize it
        /////////////////////////////////////////////////////////////////////
        std::array<std::string, 70> joint_names;
        for (size_t i = 0; i < joint_names.size(); i++)

            joint_names[i] = "undefined";

        joint_names[1] = "ss_e_ip_0";
        joint_names[2] = "ss_e_ip_1";
        joint_names[3] = "ss_e_ip_2";
        joint_names[4] = "ss_e_ip_3";
        joint_names[8] = "side_removal";
        joint_names[9] = "ss_e_ip_custom";
        joint_names[10] = "ss_e_op_0";
        joint_names[11] = "ss_e_op_1";
        joint_names[12] = "ss_e_op_2";
        joint_names[13] = "ss_e_op_3";
        joint_names[14] = "ss_e_op_4";
        joint_names[15] = "ss_e_op_5";
        joint_names[16] = "ss_e_op_6";
        joint_names[18] = "side_removal";
        joint_names[19] = "ss_e_op_custom";
        joint_names[20] = "ts_e_p_0";
        joint_names[21] = "ts_e_p_1";
        joint_names[22] = "ts_e_p_2";
        joint_names[23] = "ts_e_p_3";
        joint_names[24] = "ts_e_p_4";
        joint_names[25] = "ts_e_p_5";
        joint_names[28] = "side_removal";
        joint_names[29] = "ts_e_p_custom";
        joint_names[30] = "cr_c_ip_0";
        joint_names[31] = "cr_c_ip_1";
        joint_names[32] = "cr_c_ip_2";
        joint_names[33] = "cr_c_ip_3";
        joint_names[34] = "cr_c_ip_4";
        joint_names[35] = "cr_c_ip_5";
        joint_names[39] = "cr_c_ip_custom";
        joint_names[38] = "side_removal";
        joint_names[40] = "tt_e_p_0";
        joint_names[41] = "tt_e_p_1";
        joint_names[42] = "tt_e_p_2";
        joint_names[43] = "tt_e_p_3";
        joint_names[44] = "tt_e_p_4";
        joint_names[45] = "tt_e_p_5";
        joint_names[48] = "side_removal";
        joint_names[49] = "tt_e_p_custom";
        joint_names[58] = "side_removal_ss_e_r_1";
        joint_names[59] = "ss_e_r_custom";
        joint_names[60] = "b_0";
        joint_names[69] = "b_custom";

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Most joints are often the same
        // Store parameters as a string: "joint_type ; divisions, not dist ; shift"
        // If this dictionary already has joints, fill parameters: f, m, booleans, with existing ones.
        //  This wood::joint is not oriented, orientation happens at the end of the loop
        //  if unique_joints.contains(string)
        //   replace parameters
        //  else wood::joint = createnewjoint()
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::map<std::string, wood::joint> unique_joints;

        double division_length = 1000;
        double shift = 0.5;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Iterate joints constructed during search methods
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> ids_to_remove;
        ids_to_remove.reserve(joints.size());
        int counter = 0;

        for (auto& jo : joints)
        {

            counter++;
            if (jo.link) // skip link jont because they are generated inside main wood::joint and then orient below
                continue;

            // Select user given type
            int id_representing_joint_name = -1;
            if (elements[jo.v0].JOINTS_TYPES.size() && elements[jo.v1].JOINTS_TYPES.size())
            {
                int a = std::abs(elements[jo.v0].JOINTS_TYPES[jo.f0_0]);
                int b = std::abs(elements[jo.v1].JOINTS_TYPES[jo.f1_0]);
                id_representing_joint_name = (a > b) ? a : b;
            }
            else if (elements[jo.v0].JOINTS_TYPES.size())
            {
                id_representing_joint_name = std::abs(elements[jo.v0].JOINTS_TYPES[jo.f0_0]);
            }
            else if (elements[jo.v1].JOINTS_TYPES.size())
            {
                id_representing_joint_name = std::abs(elements[jo.v1].JOINTS_TYPES[jo.f1_0]);
            }

            // When users gives an input -> default_parameters_for_four_types
            int number_of_types = 7;      // side-side in-plane | side-side out-of-plane | top-side | cross | top-top | side-side rotated
            int number_of_parameters = 3; // division_dist | shift | type
            bool default_parameters_given = default_parameters_for_four_types.size() == (long long)(number_of_types * number_of_parameters);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Select first by local search, only then by user given index, or by default
            // This setting produces joints without geometry, do you need to delete these joints?
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            int group = -1;
            if (id_representing_joint_name == 0)
            {
                continue;
            }
            else if (jo.type == 12 && ((id_representing_joint_name > 0 && id_representing_joint_name < 10) || id_representing_joint_name == -1))
            {
                group = 0;
            }
            else if (jo.type == 11 && ((id_representing_joint_name > 9 && id_representing_joint_name < 20) || id_representing_joint_name == -1))
            {
                group = 1;
            }
            else if (jo.type == 20 && ((id_representing_joint_name > 19 && id_representing_joint_name < 30) || id_representing_joint_name == -1))
            {
                group = 2;
            }
            else if (jo.type == 30 && ((id_representing_joint_name > 29 && id_representing_joint_name < 40) || id_representing_joint_name == -1))
            {
                group = 3;
            }
            else if (jo.type == 40 && ((id_representing_joint_name > 39 && id_representing_joint_name < 50) || id_representing_joint_name == -1))
            { // top-top
                group = 4;
            }
            else if (jo.type == 13 && ((id_representing_joint_name > 49 && id_representing_joint_name < 60) || id_representing_joint_name == -1))
            { // side-side rotated
                group = 5;
            }
            else if (jo.type == 60 && ((id_representing_joint_name > 59 && id_representing_joint_name < 70) || id_representing_joint_name == -1))
            { // top-top
                group = 6;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Joint scale that can be defined
            // a) by wood_globals, when the scale input is empty
            // b) by user
            // TODO: think if this this property can be replaced by wood_global::JOINT_VOLUME_EXTENSION property only
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (scale.size() > 0)
            {
                if (scale.size() < 3)
                    if (scale.size() == 0)
                    {
                        // std::cout << "Scale is not defined, using default scale: " << wood_globals::JOINT_VOLUME_EXTENSION[0] << " " << wood_globals::JOINT_VOLUME_EXTENSION[1] << " " << wood_globals::JOINT_VOLUME_EXTENSION[2] << std::endl;
                        jo.scale = { wood_globals::JOINT_VOLUME_EXTENSION[0], wood_globals::JOINT_VOLUME_EXTENSION[1], wood_globals::JOINT_VOLUME_EXTENSION[2] };
                    }
                    else
                        jo.scale = { scale[0], scale[0], scale[0] };
                else if (scale.size() == 3)
                    jo.scale = { scale[0], scale[1], scale[2] };
                else
                    jo.scale = { scale[group * 3 + 0], scale[group * 3 + 1], scale[group * 3 + 2] };
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Set the properties of the joint from the global parameters to local joint parameters
            // jo.shift
            // jo.divisions
            // Set unit distance, this value is a distance between wood::joint volume rectangles, in case this property is set->unit_scale = true
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (default_parameters_given)
            {
                division_length = default_parameters_for_four_types[(long long)(number_of_parameters * group + 0)];
                jo.shift = default_parameters_for_four_types[(long long)(number_of_parameters * group + 1)];
                if (id_representing_joint_name == -1) // for cases when wood::joint types per each edge are not defined
                    id_representing_joint_name = (int)default_parameters_for_four_types[(number_of_parameters * group + 2)];
            }
            // std::cout << id_representing_joint_name << " " << group << "\n";
            //  printf("\n  %i, %i, %i", jo.type, jo.v0, jo.v1);
            //  printf("\n Hi %i %i", id_representing_joint_name, group);
            if (id_representing_joint_name < 1 || group == -1)
            {
                ids_to_remove.emplace_back(counter - 1);
                // printf("%i %i %i ", jo.type, jo.v0, jo.v1);
                // std::cout << "Joint is skipped \n";
                continue;
            }
            else
            {
                // printf("%i %i %i \n ", jo.type, jo.v0, jo.v1);
                // std::cout << "Joint is not skipped \n";
            }
            jo.name = jo.linked_joints.size() > 0 ? joint_names[id_representing_joint_name] + "_linked" : joint_names[id_representing_joint_name];
            jo.unit_scale_distance = elements[jo.v0].thickness * jo.scale[2];
            jo.get_divisions(division_length); // the division_length is assigne to the division_length property, and divisions are computed only if there is a line, for top-to-top connections this line does not exist, so the division is set to 1

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Assign geometry to joints that currently contains only adjacency
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            // if there is already such joint
            // std::cout << key << "\n";
            bool is_similar_joint = unique_joints.count(jo.get_key()) == 1;

            // CGAL_Debug(0);
            // CGAL_Debug(id_representing_joint_name);
            // is_similar_joint = false;
            // std::cout << "_joint id: " << jo.id << " " << unique_joints.count(key) << " " << key << std::endl;
            // std::cout << "key " << jo.get_key() << "links " << jo.linked_joints.size() << std::endl;
            if (is_similar_joint && jo.linked_joints.size() == 0) // skip linked joints, that must be regenerated each time, currently there is no solution to optimize this further due to 4 valence joints
            {
                // std::cout << "wood_joint_lib -> skipping joint \n";
                auto u_j = unique_joints.at(jo.get_key());
                // CGAL_Debug(1);
                jo.transfer_geometry(u_j);
                // CGAL_Debug(2);
                jo.orient_to_connection_area();
                // CGAL_Debug(3);
                continue;
            }

            // else create a new joint
            // std::cout << group << "\n";
            // continue;
            // bool is_joint_implemented = true;
            // std::cout << group << "\n"                      << id_representing_joint_name<<"\n ";
            switch (group)
            {
            case (0):
                switch (id_representing_joint_name)
                {
                case (1):
                    ss_e_ip_1(jo);
                    break;
                case (2):
                    ss_e_ip_0(jo);
                    break;

                case (3):
                    ss_e_ip_2(jo, elements);
                    break;
                case (4):
                    ss_e_ip_3(jo);
                    break;
                case (5):
                    ss_e_ip_4(jo);
                    break;
                case (8):
                    side_removal(jo, elements);
                    break;
                case (9):
                    ss_e_ip_custom(jo);
                    // wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    //internal::read_xml(jo, jo.type); // is it 0 or 12 ?
                    // CGAL_Debug(elements[joint.v0].thickness);
                    break;
                default:
                    ss_e_ip_1(jo);
                    // ss_e_ip_0(joint);
                    break;
                }
                break;
            case (1):
                switch (id_representing_joint_name)
                {
                    // printf("\nss_e_op_1");
                case (10):

                    ss_e_op_1(jo);
                    break;
                case (11):
                    ss_e_op_2(jo);
                    break;
                case (13):
                    ss_e_op_3(jo);
                    break;
                case (14):
                    ss_e_op_4(jo);
                    break;
                case (15):
                    // std::cout << ("ss_e_op_5\n");
                    ss_e_op_5(jo, joints);
                    break;
                case (16):
                    // std::cout << ("ss_e_op_6\n");
                    ss_e_op_6(jo, joints);
                    break;
                case (12):
                    ss_e_op_0(jo);
                    break;
                case (18):
                    side_removal(jo, elements);
                    break;
                case (19):
                    ss_e_op_custom(jo);
                    // if (true) {
                    //     std::ofstream myfile2;
                    //     myfile2.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\19.txt");
                    //     myfile2 << "Exists\n";
                    //     myfile2 << wood_globals::PATH_AND_FILE_FOR_JOINTS;
                    //     myfile2.close();
                    // }
                    // wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    //internal::read_xml(jo, jo.type); // is it 0 or 12 ?
                    break;
                default:
                    ss_e_op_1(jo);
                    // ss_e_op_0(joint);
                    break;
                }
                break;
            case (2):
                switch (id_representing_joint_name)
                {
                case (20):
                    // printf("\nts_e_p_1");
                    // printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ID %i SHIFT %f KEY %s DIVISIONS %f   ", __FILE__, __FUNCTION__, __LINE__, "Assigned groups", id_representing_joint_name, jo.shift, key.c_str(), jo.divisions);

                    ts_e_p_3(jo);
                    // ts_e_p_1(jo);
                    break;
                case (21):
                    ts_e_p_2(jo);
                    break;
                case (22):
                    ts_e_p_3(jo);
                    break;
                case (23):
                    ts_e_p_0(jo);
                case (24):
                    ts_e_p_4(jo);
                    break;
                case (25):
                    ts_e_p_5(jo);
                    break;
                case (28):
                    side_removal(jo, elements);
                    break;
                case (29):
                    ts_e_p_custom(jo);
                    // wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    //internal::read_xml(jo, jo.type); // is it 0 or 12 ?
                    break;
                default:
                    ts_e_p_3(jo);
                    break;
                }
                break;
            case (3):
                switch (id_representing_joint_name)
                {
                case (30):
                    cr_c_ip_0(jo);
                    break;
                case (31):
                    cr_c_ip_1(jo);
                    break;
                case (32):
                    cr_c_ip_2(jo);
                    break;
                case (33):
                    cr_c_ip_3(jo);
                    break;
                case (34):
                    cr_c_ip_4(jo);
                    break;
                case (35):
                    cr_c_ip_5(jo);
                    break;
                case (38):
                    side_removal(jo, elements);
                    break;
                case (39):
                    cr_c_ip_custom(jo);
                    // wont work, because not oriented to connection zones, need additional layer e.g. std::map of all joints
                    //internal::read_xml(jo, jo.type); // is it 0 or 12 ?
                    break;
                default:
                    cr_c_ip_0(jo);
                    // cr_c_ip_0(joint);
                    // printf(joint.name.c_str());
                    break;
                }
                break;
            case (4):
                switch (id_representing_joint_name)
                {
                case (40):
                    tt_e_p_0(jo, elements);
                    break;
                case (41):
                    tt_e_p_1(jo, elements);
                    break;
                case (42):
                    tt_e_p_2(jo, elements);
                    break;
                case (43):
                    tt_e_p_3(jo, elements);
                    break;
                case (44):
                    tt_e_p_4(jo, elements);
                    break;
                case (45):
                    tt_e_p_5(jo, elements);
                    break;
                case (48):
                    side_removal(jo, elements);
                    break;
                case (49):
                    tt_e_p_custom(jo);
                    break;
                default:
                    tt_e_p_0(jo, elements);
                    break;
                }
                break;
            case (5):
                switch (id_representing_joint_name)
                {
                    // CGAL_Debug(id_representing_joint_name);

                case (56):
                    ss_e_r_0(jo, elements);
                    break;
                case (57):
                    side_removal(jo, elements);
                    break;
                case (58):
                    side_removal(jo, elements, true);
                    break;
                case (59):
                    ss_e_r_custom(jo);
                    // CGAL_Debug(99999);
                    // ss_e_ip_1(jo);
                    //internal::read_xml(jo, jo.type); // is it 0 or 12 ?
                    break;
                default:

                    // default case remove polygon from each

                    side_removal(jo, elements);
                    // CGAL_Debug(jo.orient);
                    // internal::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                }
                break;
            case (6):
                switch (id_representing_joint_name)
                {
                case (60):
                    // printf("\nhi %i", id_representing_joint_name);
                    b_0(jo);
                    break;
                case (69):
                    // printf("\nhi %i", id_representing_joint_name);
                    b_custom(jo);
                    break;
                default:
                    b_0(jo);
                    break;
                }
                break;
            }
            // std::cout << "b\n";
            // if (!is_joint_implemented)
            //     continue;
            // std::cout << "joint_name  " << jo.name << std::endl;
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Add to wood::joint map because this wood::joint was not present
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (jo.m[0].size() == 0 && jo.f[0].size() == 0)
            {
                // std::ofstream myfile;
                // myfile.open("C:\\Users\\petra\\AppData\\Roaming\\Grasshopper\\Libraries\\compas_wood\\xml_error.txt");
                // myfile << "Empty joint\n";
                // myfile << wood_globals::PATH_AND_FILE_FOR_JOINTS;
                // myfile.close();
                printf("\nwood_joint_lib -> joint name %s between elements %i and %i is empty", jo.name.c_str(), jo.v0, jo.v1);
                // CGAL_Debug(group);
                // CGAL_Debug(id_representing_joint_name);
                continue;
            }

            if (jo.orient)
            {
                wood::joint temp_joint;
                jo.duplicate_geometry(temp_joint);
                unique_joints.insert(std::pair<std::string, wood::joint>(jo.get_key(), temp_joint));
                jo.orient_to_connection_area(); // and orient to connection volume

                // special case for vidy only when joints must be merged
                if (jo.linked_joints.size() > 0 && (id_representing_joint_name == 15 || id_representing_joint_name == 16))
                {
                    for (auto& linked_joint_id : jo.linked_joints)
                        joints[linked_joint_id].orient_to_connection_area();
                    jo.remove_geo_from_linked_joint_and_merge_with_current_joint(joints);
                }
            }
        }
    }
}
