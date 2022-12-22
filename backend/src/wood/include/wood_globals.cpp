#include "wood_globals.h"
#include "../../../stdafx.h"

namespace wood_globals
{

    int output_geometry_type = 3;
    bool force_side_to_side_joints_to_be_rotated = false;
    bool run = 0;
    double joint_line_extension = 0;
    double limit_min_joint_length = 0;
    std::array<std::string, 7> joint_names = {
        "side-to-side\nin-plane",
        "side-to-side\nout-of-plane",
        "top-to-side\n ",
        "cross\n ",
        "top-to-top\n ",
        "side-to-side\nrotated",
        "border\n "};

    std::vector<std::string> existing_types = {
        "joint_names[1] = ss_e_ip_0;",
        "joint_names[2] = ss_e_ip_1;",
        "joint_names[3] = ss_e_ip_2;",
        "joint_names[8] = side_removal;",
        "joint_names[9] = ss_e_ip_9;",
        "joint_names[10] = ss_e_op_0;",
        "joint_names[11] = ss_e_op_1;",
        "joint_names[12] = ss_e_op_2;",
        "joint_names[13] = ss_e_op_3;",
        "joint_names[14] = ss_e_op_4;",
        "joint_names[15] = ss_e_op_5;",
        "joint_names[18] = side_removal;",
        "joint_names[19] = ss_e_op_9;",
        "joint_names[20] = ts_e_p_0;",
        "joint_names[21] = ts_e_p_1;",
        "joint_names[22] = ts_e_p_2;",
        "joint_names[23] = ts_e_p_3;",
        "joint_names[24] = ts_e_p_4;",
        "joint_names[28] = side_removal;",
        "joint_names[29] = ts_e_p_9;",
        "joint_names[30] = cr_c_ip_0;",
        "joint_names[31] = cr_c_ip_1;",
        "joint_names[32] = cr_c_ip_2;",
        "joint_names[38] = side_removal;",
        "joint_names[39] = cr_c_ip_9;",
        "joint_names[48] = side_removal;",
        "joint_names[58] = side_removal_ss_e_r_1;",
        "joint_names[59] = ss_e_r_9;",
        "joint_names[60] = b_0;",
    };

    std::vector<double> joint_types{300, 0.5, 8, 450, 0.64, 15, 450, 0.5, 20, 300, 0.5, 30, 300, 0.5, 40, 300, 0.5, 58, 300, 1.0, 60};

    //how to make the local path to the data set folder
    std::string data_set_input_folder = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\wood\\dataset\\";
    std::string data_set_output_file = data_set_input_folder + "out.xml";


    
}