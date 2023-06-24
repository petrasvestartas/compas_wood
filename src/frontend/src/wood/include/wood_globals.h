///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// DEVELOPER:
// Petras Vestartas, petasvestartas@gmail.com
// Funding: NCCR Digital Fabrication and EPFL
//
// HISTORY:
// 1) The first version was written during the PhD 8928 thesis of Petras Vestartas called:
// Design-to-Fabrication Workflow for Raw-Sawn-Timber using Joinery Solver, 2017-2021
// 2) The translation from C# to C++ was started during the funding of NCCR in two steps
// A - standalone C++ version of the joinery solver and B - integration to COMPAS framework (Python Pybind11)
//
// RESTRICTIONS:
// The code cannot be used for commercial reasons
// If you would like to use or change the code for research or educational reasons,
// please contact the developer first
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef WOOD_GLOBALS_H
#define WOOD_GLOBALS_H

// #ifdef WOOD_WRAPPER
// #include "../../../src/compas_wood/include/stdafx_pybind11.h" //go up to the folder where the CMakeLists.txt is
// #endif

// Preprocessor stage
#define ON_IS_FINITE(x) (0x7FF0 != (*((unsigned short *)(&x) + 3) & 0x7FF0))
#define ON_DBL_MIN 2.22507385850720200e-308
#define ON_EPSILON 2.2204460492503131e-16
#define ON_SQRT_EPSILON 1.490116119385000000e-8
#define ON_ZERO_TOLERANCE 2.3283064365386962890625e-10
#define ON_DBL_MAX 1.7976931348623158e+308

// Compilation stage
namespace wood_globals
{

    // Clipper2 library mostly used in clipper_util
    extern int64_t CLIPPER_SCALE;
    extern double CLIPPER_AREA;

    // Tolerances for distance search
    extern double DISTANCE;         // GlobalTolerance
    extern double DISTANCE_SQUARED; // GlobalToleranceSquare
    extern double ANGLE;            // GlobalAngleTolerance

    // File names
    extern std::string PATH_AND_FILE_FOR_JOINTS; // path_and_file_for_joints
    extern std::string DATA_SET_INPUT_FOLDER;
    extern std::string DATA_SET_OUTPUT_FILE;

    // Wood library
    extern std::vector<double> JOINT_VOLUME_EXTENSION; // useful for plates to avoid collision with corner - negative, while on beam the value has to be positive
    extern int OUTPUT_GEOMETRY_TYPE;
    extern bool FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ALL_TREATED_AS_ROTATED; // when all joints are computed as if sides are rotated, it was implemented in the Touch Wood ZAZ
    extern double FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_DIHEDRAL_ANGLE; // value to decide when a joint is in-plane or out-of-plane
    extern double LIMIT_MIN_JOINT_LENGTH;                // minimum edge length on which the joint can be generated
    extern std::array<std::string, 7> JOINT_NAMES;
    extern std::vector<std::string> EXISTING_TYPES;
    extern std::vector<double> JOINTS_PARAMETERS_AND_TYPES;

    // Wood viewer
    extern size_t RUN_COUNT;

    // custom joint types
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_ip_male;
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_ip_female;
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_op_male;
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_op_female;
    extern std::vector<CGAL_Polyline> custom_joints_ts_e_p_male;
    extern std::vector<CGAL_Polyline> custom_joints_ts_e_p_female;
    extern std::vector<CGAL_Polyline> custom_joints_cr_c_ip_male;
    extern std::vector<CGAL_Polyline> custom_joints_cr_c_ip_female;
    extern std::vector<CGAL_Polyline> custom_joints_tt_e_p_male;
    extern std::vector<CGAL_Polyline> custom_joints_tt_e_p_female;
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_r_male;
    extern std::vector<CGAL_Polyline> custom_joints_ss_e_r_female;
    extern std::vector<CGAL_Polyline> custom_joints_b_male;
    extern std::vector<CGAL_Polyline> custom_joints_b_female;
}

#endif