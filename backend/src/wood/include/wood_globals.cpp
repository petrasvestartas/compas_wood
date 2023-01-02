#include "wood_globals.h"
#include "../../../stdafx.h"

namespace wood_globals
{
    // Clipper2 library mostly used in clipper_util
    int64_t CLIPPER_SCALE = 1000000;
    double CLIPPER_AREA = 0.0001;

    // Tolerances for distance search
    double DISTANCE = 0.01;
    double DISTANCE_SQUARED = 0.0001;

    // File names
    std::string PATH_AND_FILE_FOR_JOINTS = "";
    std::string DATA_SET_INPUT_FOLDER = "C:\\IBOIS57\\_Code\\Software\\Python\\compas_wood\\backend\\src\\wood\\dataset\\";
    std::string DATA_SET_OUTPUT_FILE = DATA_SET_INPUT_FOLDER + "out.xml";

    // Wood library
    double EXTEND[5] = {0.0, 0.0, 0.0, 0, 0};
    int OUTPUT_GEOMETRY_TYPE = 3;
    bool FORCE_SIDE_TO_SIDE_JOINTS_TO_BE_ROTATED = false;
    double JOINT_LINE_EXTENSION = 0;
    double LIMIT_MIN_JOINT_LENGTH = 0;

    std::array<std::string, 7> JOINT_NAMES = {
        "side-to-side\nin-plane",
        "side-to-side\nout-of-plane",
        "top-to-side\n ",
        "cross\n ",
        "top-to-top\n ",
        "side-to-side\nrotated",
        "border\n "};

    std::vector<std::string> EXISTING_TYPES = {
        "JOINT_NAMES[1] = ss_e_ip_0;",
        "JOINT_NAMES[2] = ss_e_ip_1;",
        "JOINT_NAMES[3] = ss_e_ip_2;",
        "JOINT_NAMES[8] = side_removal;",
        "JOINT_NAMES[9] = ss_e_ip_9;",
        "JOINT_NAMES[10] = ss_e_op_0;",
        "JOINT_NAMES[11] = ss_e_op_1;",
        "JOINT_NAMES[12] = ss_e_op_2;",
        "JOINT_NAMES[13] = ss_e_op_3;",
        "JOINT_NAMES[14] = ss_e_op_4;",
        "JOINT_NAMES[15] = ss_e_op_5;",
        "JOINT_NAMES[18] = side_removal;",
        "JOINT_NAMES[19] = ss_e_op_9;",
        "JOINT_NAMES[20] = ts_e_p_0;",
        "JOINT_NAMES[21] = ts_e_p_1;",
        "JOINT_NAMES[22] = ts_e_p_2;",
        "JOINT_NAMES[23] = ts_e_p_3;",
        "JOINT_NAMES[24] = ts_e_p_4;",
        "JOINT_NAMES[28] = side_removal;",
        "JOINT_NAMES[29] = ts_e_p_9;",
        "JOINT_NAMES[30] = cr_c_ip_0;",
        "JOINT_NAMES[31] = cr_c_ip_1;",
        "JOINT_NAMES[32] = cr_c_ip_2;",
        "JOINT_NAMES[38] = side_removal;",
        "JOINT_NAMES[39] = cr_c_ip_9;",
        "JOINT_NAMES[48] = side_removal;",
        "JOINT_NAMES[58] = side_removal_ss_e_r_1;",
        "JOINT_NAMES[59] = ss_e_r_9;",
        "JOINT_NAMES[60] = b_0;",
    };

    std::vector<double> JOINTS_TYPES{300, 0.5, 8, 450, 0.64, 15, 450, 0.5, 20, 300, 0.5, 30, 300, 0.5, 40, 300, 0.5, 58, 300, 1.0, 60};

}