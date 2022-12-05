#ifndef WOOD_GLOBALS_H
#define WOOD_GLOBALS_H

namespace wood_globals
{
    extern int output_geometry_type;
    extern bool force_side_to_side_joints_to_be_rotated; // when all joints are computed as if sides are rotated, it was implemented in the Touch Wood ZAZ
    extern bool run;
    extern double joint_line_extension;   // useful for plates to avoid collision with corner - negative, while on beam the value has to be positive
    extern double limit_min_joint_length; // minimum edge length on which the joint can be generated
    extern std::array<std::string, 7> joint_names;
    extern std::vector<std::string> existing_types;
    extern std::vector<double> joint_types;

    
    extern std::string data_set_input_folder;
    extern std::string data_set_output_file;
}

#endif