#! python3
from typing import *
from wood_rui import wood_rui_globals, NamedValuesForm  # Import the singleton instance
from compas_wood.binding import wood_globals

if __name__ == "__main__":

    # Define the parameters
    parameters: Tuple[str, Any] = [
        ("SEARCH TYPE", " "),  # 0
        (
            "(0 - side-to-side, 1 - cross, 2 - both)",
            wood_rui_globals.search_type,
        ),  # temporary parameter that must go from compas_wood.binding import wood_globals
        (" ", " "),  # 2
        ("OUTPUT TYPE", " "),  # 3
        (
            "(0 - areas, 1 - lines, 2 - volumes, 3 - joints, 4 - merged joints)     ",
            wood_globals.output_geometry_type,
        ),  # 4
        (" ", " "),  # 5
        ("SCALE", " "),  # 6
        (
            "x-axis",
            wood_rui_globals.scale[0],
        ),  # 7 temporary parameter that must go from compas_wood.binding import wood_globals
        (
            "y-axis",
            wood_rui_globals.scale[1],
        ),  # 8 temporary parameter that must go from compas_wood.binding import wood_globals
        (
            "z-axis",
            wood_rui_globals.scale[2],
        ),  # 9 temporary parameter that must go from compas_wood.binding import wood_globals
        (" ", " "),  # 10
        ("EXTENSION", " "),  # 11
        (
            "x-axis",
            wood_rui_globals.joint_volume_extension[0],
        ),  # 12 temporary parameter that must go from compas_wood.binding import wood_globals
        (
            "y-axis",
            wood_rui_globals.joint_volume_extension[1],
        ),  # 13 temporary parameter that must go from compas_wood.binding import wood_globals
        (
            "z-axis",
            wood_rui_globals.joint_volume_extension[2],
        ),  # 14 temporary parameter that must go from compas_wood.binding import wood_globals
        (" ", " "),  # 15
        ("SIDE-TO-SIDE EDGE IN-PLANE", " "),  # 16
        ("division length (x>0)", wood_globals.joints_parameters_and_types[0]),  # 17
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[1]),  # 18
        ("type (1-9)", wood_globals.joints_parameters_and_types[2]),  # 19
        (" ", " "),  # 20
        ("SIDE-TO-SIDE EDGE OUT-OF-PLANE", " "),  # 21
        ("division length (x>0)", wood_globals.joints_parameters_and_types[3]),  # 22
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[4]),  # 23
        ("type (10-19)", wood_globals.joints_parameters_and_types[5]),  # 24
        (" ", " "),  # 25
        ("TOP-TO-SIDE EDGE PLANE", " "),  # 26
        ("division length (x>0)", wood_globals.joints_parameters_and_types[6]),  # 27
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[7]),  # 28
        ("type (20-29)", wood_globals.joints_parameters_and_types[8]),  # 29
        (" ", " "),  # 30
        ("CROSS CUTTING IN-PLANE", " "),  # 31
        ("division length (x>0)", wood_globals.joints_parameters_and_types[9]),  # 32
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[10]),  # 33
        ("type (30-39)", wood_globals.joints_parameters_and_types[11]),  # 34
        (" ", " "),  # 35
        ("TOP-TO-TOP EDGE PLANE", " "),  # 36
        ("division length (x>0)", wood_globals.joints_parameters_and_types[12]),  # 37
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[13]),  # 38
        ("type (40-49)", wood_globals.joints_parameters_and_types[14]),  # 39
        (" ", " "),  # 40
        ("SIDE-TO-SIDE EDGE ROTATED", " "),  # 41
        ("division length (x>0)", wood_globals.joints_parameters_and_types[15]),  # 42
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[16]),  # 43
        ("type (50-59)", wood_globals.joints_parameters_and_types[17]),  # 44
        (" ", " "),  # 45
        ("BOUNDARY", " "),  # 46
        ("division length (x>0)", wood_globals.joints_parameters_and_types[18]),  # 47
        ("parameter (0-1)", wood_globals.joints_parameters_and_types[19]),  # 48
        ("type (60-69)", wood_globals.joints_parameters_and_types[20]),  # 49
        (" ", " "),  # 50
        ("DEBUG PARAMETERS", " "),  # 51
        ("clipper_scale", wood_globals.clipper_scale),  # 52
        ("clipper_area", wood_globals.clipper_area),  # 53
        ("distance", wood_globals.distance),  # 54
        ("distance_squared", wood_globals.distance_squared),  # 55
        ("angle", wood_globals.angle),  # 56
        ("path_and_file_for_joints", wood_globals.path_and_file_for_joints),  # 57
        ("data_set_input_folder", wood_globals.data_set_input_folder),  # 58
        ("data_set_output_file", wood_globals.data_set_output_file),  # 59
        ("data_set_output_database", wood_globals.data_set_output_database),  # 60
        (
            "face_to_face_side_to_side_joints_all_treated_as_rotated",
            wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated,
        ),  # 61
        (
            "face_to_face_side_to_side_joints_rotated_joint_as_average",
            wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average,
        ),  # 62
        (
            "face_to_face_side_to_side_joints_dihedral_angle",
            wood_globals.face_to_face_side_to_side_joints_dihedral_angle,
        ),  # 63
        ("limit_min_joint_length", wood_globals.limit_min_joint_length),  # 64
        ("three_valence", wood_rui_globals.three_valence),  # 65
        ("adjacency", wood_rui_globals.adjacency),  # 66
    ]

    # Extract names and values separately
    names: list[str] = [item[0] for item in parameters]  # First row: keys (names)
    values: list[str, float, int, tuple, list, dict, set, bool, None] = [
        item[1] for item in parameters
    ]  # Second row: values

    # Pass the my_method as the run_method parameter
    form: NamedValuesForm = NamedValuesForm(
        names=names, values=values, title="Joinery Parameters", width=408, height=600
    )

    if form.show():
        joint_parameters = [
            float(form.attributes[17][1]),
            float(form.attributes[18][1]),
            float(form.attributes[19][1]),
            float(form.attributes[22][1]),
            float(form.attributes[23][1]),
            float(form.attributes[24][1]),
            float(form.attributes[27][1]),
            float(form.attributes[28][1]),
            float(form.attributes[29][1]),
            float(form.attributes[32][1]),
            float(form.attributes[33][1]),
            float(form.attributes[34][1]),
            float(form.attributes[37][1]),
            float(form.attributes[38][1]),
            float(form.attributes[39][1]),
            float(form.attributes[42][1]),
            float(form.attributes[43][1]),
            float(form.attributes[44][1]),
            float(form.attributes[47][1]),
            float(form.attributes[48][1]),
            float(form.attributes[49][1]),
        ]

        wood_globals.joints_parameters_and_types = joint_parameters
        wood_globals.joint_volume_extension = [form.attributes[12][1], form.attributes[13][1], form.attributes[14][1]]
        wood_globals.output_geometry_type = form.attributes[4][1]
        wood_globals.clipper_scale = form.attributes[52][1]
        wood_globals.clipper_area = form.attributes[53][1]
        wood_globals.distance = form.attributes[54][1]
        wood_globals.distance_squared = form.attributes[55][1]
        wood_globals.angle = form.attributes[56][1]
        wood_globals.path_and_file_for_joints = form.attributes[57][1]
        wood_globals.data_set_input_folder = form.attributes[58][1]
        wood_globals.data_set_output_file = form.attributes[59][1]
        wood_globals.data_set_output_database = form.attributes[60][1]
        wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = form.attributes[61][1]
        wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = form.attributes[62][1]
        wood_globals.face_to_face_side_to_side_joints_dihedral_angle = form.attributes[63][1]
        wood_globals.limit_min_joint_length = form.attributes[64][1]

    wood_rui_globals.scale = [
        form.attributes[7][1],
        form.attributes[8][1],
        form.attributes[9][1],
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals
    wood_rui_globals.search_type = form.attributes[1][
        1
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals
    wood_rui_globals.joint_volume_extension = [
        form.attributes[12][1],
        form.attributes[13][1],
        form.attributes[14][1],
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals

    wood_rui_globals.scale = [
        form.attributes[7][1],
        form.attributes[8][1],
        form.attributes[9][1],
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals

    wood_rui_globals.three_valence = form.attributes[65][
        1
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals
    wood_rui_globals.adjacency = form.attributes[66][
        1
    ]  # temporary parameter that must go from compas_wood.binding import wood_globals
