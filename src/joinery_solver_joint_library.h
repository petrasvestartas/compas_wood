#pragma once
#include "cgal.h"
#include "joinery_solver_joint.h"

//XML Reader
#include <iostream>
#include <fstream>
//#include <cstdint>

//
#include <vector>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace joint_library_xml_parser {
    inline bool exists_test0(const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    inline bool exists_test3(const std::string& name) {
        struct stat buffer;
        return (stat(name.c_str(), &buffer) == 0);
    }

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
            case(50):
            case(5):
                name = "ss_e_r_9";
                break;
        }

        joint.name = name;

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Check if XML file exists, path_and_file_for_joints is a global path
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP File path %s ", path_and_file_for_joints.c_str());
        printf("\nCPP Joint Type %i %s ", type, name.c_str());
#endif
        if (!exists_test3(path_and_file_for_joints)) {
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP File does not exist %s ", path_and_file_for_joints.c_str());
#endif
            return false;
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

                        //Assign to array
                        switch (i) {
                            case(0):
                                joint.m[0].emplace_back(polyline);
                                break;

                            case(1):
                                joint.m[1].emplace_back(polyline);
                                break;

                            case(2):
                                joint.f[0].emplace_back(polyline);
                                break;

                            case(3):
                                joint.f[1].emplace_back(polyline);
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
                            if (i == 5) {
                                joint.m_boolean_type.emplace_back(id);
                                //emplace to female joint.m_boolean_type
                            } else {
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
        } catch (std::exception& e) {
#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP Wrong property ");
#endif
            return false;
        }
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
        } else if (z >= x && z >= y) {
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
        } else if (x > 0.0 && ON_IS_FINITE(x))
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
        } else {
            interpolated_points.reserve(Steps);

            for (int i = 1; i < Steps + 1; i++) {
                double num = i / (double)(1 + Steps);
                interpolated_points.emplace_back(Lerp(from.hx(), to.hx(), num), Lerp(from.hy(), to.hy(), num), Lerp(from.hz(), to.hz(), num));
            }
        }
    }

    //type_typeidedge_subtypeieinplane_id
    //0 - do not merge, 1 - edge insertion, 2 - hole 3 - insert between multiple edges hole

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
        int divisions = (int)std::max(2, std::min(100, joint.divisions));
        divisions += divisions % 2;

        //////////////////////////////////////////////////////////////////////////////////////////
        //Interpolate points
        //////////////////////////////////////////////////////////////////////////////////////////
        std::vector<IK::Point_3> pts0;

        interpolate_points(IK::Point_3(0, -0.5, 0.5), IK::Point_3(0, -0.5, -0.5), divisions, false, pts0);
        IK::Vector_3 v(0.5, 0, 0);
        double shift_ = RemapNumbers(joint.shift, 0, 1.0, -0.5, 0.5);
        IK::Vector_3 v_d(0, 0, -(1.0 / ((divisions + 1) * 2)) * shift_);

        int count = pts0.size() * 2;

        //1st polyline
        std::vector<IK::Point_3> pline0;
        pline0.reserve(count);
        pline0.emplace_back(pts0[0]);
        pline0.emplace_back(pts0[0] - v - v_d);

        for (int i = 1; i < pts0.size() - 1; i++) {
            if (i % 2 == 1) {
                pline0.emplace_back(pts0[i] - v + v_d);
                pline0.emplace_back(pts0[i] + v - v_d);
            } else {
                pline0.emplace_back(pts0[i] + v + v_d);
                pline0.emplace_back(pts0[i] - v - v_d);
            }
        }

        pline0.emplace_back(pts0[pts0.size() - 1] - v + v_d);
        pline0.emplace_back(pts0[pts0.size() - 1]);

        //2nd polyline
        IK::Vector_3 v_o(0, 1, 0);
        std::vector<IK::Point_3> pline1;
        pline1.reserve(pline0.size());

        for (int i = 0; i < pline0.size(); i++) {
            pline1.emplace_back(pline0[i] + v_o);
        }

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

        //if (orient_to_connection_zone)
          //  joint.orient_to_connection_area();
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
                } else if (i == 0 || i == 2) {
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
                } else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            } else {
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
                } else {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            } else {
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
                } else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            } else {
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
                } else {
                    pline.push_back(arrays[(i + 1) % 4][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.f[0] = {
                    pline,
                    //{ pline[0],pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            } else {
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
                } else {
                    pline.push_back(arrays[i + 1][j]);
                    pline.push_back(arrays[i + 0][j]);
                }
            }

            if (i < 2) {
                joint.m[1] = {
                    pline,
                    //{ pline[0], pline[pline.size() - 1] },
                    {pline[0], pline[pline.size() - 1]} };
            } else {
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
                } else {
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
            } else {
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
            {IK::Point_3(0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, -scale), IK::Point_3(-0.5, 0.5, 0), IK::Point_3(0.5, 0.5, 0), IK::Point_3(0.5, 0.5, -scale)} };

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

    inline void construct_joint_by_index(std::vector<element>& elements, std::vector<joint>& joints, const double& division_distance_, const double& shift_, std::vector<double>& default_parameters_for_four_types) {
        std::array<std::string, 59> joint_names;

        for (size_t i = 0; i < joint_names.size(); i++)
            joint_names[i] = "undefined";

        joint_names[1] = "ss_e_ip_0";
        joint_names[2] = "ss_e_ip_1";
        joint_names[9] = "ss_e_ip_9";
        joint_names[10] = "ss_e_op_0";
        joint_names[11] = "ss_e_op_1";
        joint_names[12] = "ss_e_op_2";
        joint_names[19] = "ss_e_op_9";
        joint_names[20] = "ts_e_p_0";
        joint_names[21] = "ts_e_p_1";
        joint_names[22] = "ts_e_p_2";
        joint_names[22] = "ts_e_p_3";
        joint_names[29] = "ts_e_p_9";
        joint_names[30] = "cr_c_ip_0";
        joint_names[31] = "cr_c_ip_1";
        joint_names[39] = "cr_c_ip_9";

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

        double division_distance = division_distance_;
        double shift = shift_;

#ifdef DEBUG_JOINERY_LIBRARY
        printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s   ", __FILE__, __FUNCTION__, __LINE__, "Before Joint Iteration");
#endif
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Iterate joints constructed during search methods
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        std::vector<int> ids_to_remove;
        ids_to_remove.reserve(joints.size());
        int counter = 0;
        for (auto& jo : joints) {
            counter++;

            //Select user given type
            //types0+265
            int id_representing_joint_name = -1;
            if (elements[jo.v0].joint_types.size() && elements[jo.v1].joint_types.size()) {
                int a = elements[jo.v0].joint_types[jo.f0_0];
                int b = elements[jo.v1].joint_types[jo.f1_0];
                id_representing_joint_name = (a > b) ? a : b;
                //CGAL_Debug(a, b, a);
            } else if (elements[jo.v0].joint_types.size()) {
                id_representing_joint_name = elements[jo.v0].joint_types[jo.f0_0];
            } else if (elements[jo.v1].joint_types.size()) {
                id_representing_joint_name = elements[jo.v1].joint_types[jo.f1_0];
            }

            //When users gives an input -> default_parameters_for_four_types
            int number_of_types = 6;	  //side-side in-plane | side-side out-of-plane | top-side | cross | top-top | side-side rotated
            int number_of_parameters = 3; //division_dist | shift | type
            bool default_parameters_given = default_parameters_for_four_types.size() == (long long)(number_of_types * number_of_parameters);

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            // Select first by local search, only then by user given index, or by default
            // This setting produces joints without geometry, do you need to delete these joints?
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            int group = -1;
            if (id_representing_joint_name == 0) {
                continue;
            } else if (jo.type == 12 && ((id_representing_joint_name > 0 && id_representing_joint_name < 10) || id_representing_joint_name == -1)) {
                group = 0;
            } else if (jo.type == 11 && ((id_representing_joint_name > 9 && id_representing_joint_name < 20) || id_representing_joint_name == -1)) {
                group = 1;
            } else if (jo.type == 20 && ((id_representing_joint_name > 19 && id_representing_joint_name < 30) || id_representing_joint_name == -1)) {
                group = 2;
            } else if (jo.type == 30 && ((id_representing_joint_name > 29 && id_representing_joint_name < 40) || id_representing_joint_name == -1)) {
                group = 3;
            } else if (jo.type == 40 && ((id_representing_joint_name > 39 && id_representing_joint_name < 50) || id_representing_joint_name == -1)) { //top-top
                group = 4;
            } else if (jo.type == 50 && ((id_representing_joint_name > 49 && id_representing_joint_name < 60) || id_representing_joint_name == -1)) { //side-side rotated
                group = 5;
            }

            if (default_parameters_given) {
                division_distance = default_parameters_for_four_types[(long long)(number_of_parameters * group + 0)];
                jo.shift = default_parameters_for_four_types[(long long)(number_of_parameters * group + 1)];
                if (id_representing_joint_name == -1) //for cases when joint types per each edge are not defined
                    id_representing_joint_name = default_parameters_for_four_types[(long long)(number_of_parameters * group + 2)];
            }

            if (id_representing_joint_name < 1 || group == -1) {
                ids_to_remove.emplace_back(counter - 1);
                continue;
            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Set unit distance, this value is a distance between joint volume rectangles, in case this property is set -> unit_scale = true
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            jo.name = joint_names[id_representing_joint_name];
            jo.unit_scale_distance = elements[jo.v0].thickness;
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

            //CGAL_Debug(id_representing_joint_name);
            //is_similar_joint = false;
            if (is_similar_joint) {
                auto u_j = unique_joints.at(key);
                jo.transfer_geometry(u_j);
                jo.orient_to_connection_area();
                continue;
            }

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
                        case (12):
                            ss_e_op_0(jo);
                            break;
                        case (19):
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
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
                case(5):
                    joint_library_xml_parser::read_xml(jo, jo.type);//is it 0 or 12 ?
                    break;
            }

#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "after unique joint create");
#endif
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Add to joint map because this joint was not present
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            if (jo.m[0].size() == 0 && jo.f[0].size() == 0) {
                CGAL_Debug(100000000);
                CGAL_Debug(group);
                CGAL_Debug(id_representing_joint_name);
                continue;
            }
            //continue;
            joint temp_joint;
            jo.duplicate_geometry(temp_joint);

            unique_joints.insert(std::pair<std::string, joint>(key, temp_joint));
            jo.orient_to_connection_area();//and orient to connection volume

#ifdef DEBUG_JOINERY_LIBRARY
            printf("\nCPP   FILE %s    METHOD %s   LINE %i     WHAT %s ", __FILE__, __FUNCTION__, __LINE__, "last");
#endif
        }

        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //Remove empty joints
        /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //std::reverse(ids_to_remove.begin(), ids_to_remove.end());
        //for (auto& id : ids_to_remove)
        //    joints.erase(joints.begin() + id);
        //for (auto& j : joints)
        //    j.id -= ids_to_remove.size();
        //joints.

        //myfile.close();
    }
}
