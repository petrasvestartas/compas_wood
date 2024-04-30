from wood_nano import GLOBALS


class Globals(object):
    """Class to access global variables of the wood_nano library using class instance wood_globals:

    >>> from compas_wood.binding import wood_globals
    >>> wood_globals.distance_squared = 10
    >>> wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
    >>> wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

    Attributes
    ----------
    clipper_scale : int, default 1e6
        The clipper scale.
    clipper_area : float, default 1e-2
        The clipper area.
    distance : float, default 1e-1
        The distance.
    distance_squared : float, default 1e-2
        The distance squared.
    angle : float, default 0.11
        The angle.
    path_and_file_for_joints : str, default ""
        The path and file for joints.
    data_set_input_folder : str
        The data set input folder.
    data_set_output_file : str,  default data_set_input_folder + "out.xml"
        The data set output file.
    data_set_output_database : str, default data_set_input_folder + "out.db"
        The data set output database.
    joint_volume_extension : list[float], default [0.0, 0.0, 0.0, 0.0, 0.0]
        The joint volume extension, first 3 values are for xyz of the joint volume.
    output_geometry_type : int, default 4
        The output geometry type.
    face_to_face_side_to_side_joints_all_treated_as_rotated : bool, default False
        The face to face side to side joints all treated as rotated.
    face_to_face_side_to_side_joints_rotated_joint_as_average : bool, default False
        The face to face side to side joints rotated joint as average.
    face_to_face_side_to_side_joints_dihedral_angle : float, default 150.0
        The face to face side to side joints dihedral angle.
    limit_min_joint_length : float, default 0.0
        The limit min joint length.
    existing_types : list[str], read-only
        The existing types of joints.
    joints_parameters_and_types : list[float], default:

        300, 0.5, 3,   // 1-9 ss_e_ip (side-to-side edge in-plane)

        450, 0.64, 15, // 10-19 ss_e_op (side-to-side edge out-of-plane)

        450, 0.5, 20,  // 20-29 ts_e_p (top-to-side edge plane)

        300, 0.5, 30,  // 30-39 cr_c_ip (cross cutting in-plane)

        6, 0.95, 40,   // 40-49 tt_e_p  (top-to-top edge plane)

        300, 0.5, 58,  // 50-59 ss_e_r (side-to-side edge rotated)

        300, 1.0, 60   // 60-69 b (boundary)

    custom_joints_ss_e_ip_male : list[Polyline], default []
        Joinery polylines for custom side-to-side edge in-plane male joints.
    custom_joints_ss_e_ip_female : list[Polyline], default []
        Joinery polylines for custom side-to-side edge in-plane female joints.
    custom_joints_ss_e_op_male : list[Polyline], default []
        Joinery polylines for custom side-to-side edge out-of-plane male joints.
    custom_joints_ss_e_op_female : list[Polyline], default []
        Joinery polylines for custom side-to-side edge out-of-plane female joints.
    custom_joints_ts_e_p_male : list[Polyline], default []
        Joinery polylines for custom top-to-side edge in-plane male joints.
    custom_joints_ts_e_p_female : list[Polyline], default []
        Joinery polylines for custom top-to-side edge in-plane female joints.
    custom_joints_cr_c_ip_male : list[Polyline], default []
        Joinery polylines for custom corner-to-corner in-plane male joints.
    custom_joints_cr_c_ip_female : list[Polyline], default []
        Joinery polylines for custom corner-to-corner in-plane female joints.
    custom_joints_tt_e_p_male : list[Polyline], default []
        Joinery polylines for custom top-to-top edge in-plane male joints.
    custom_joints_tt_e_p_female : list[Polyline], default []
        Joinery polylines for custom top-to-top edge in-plane female joints.
    custom_joints_ss_e_r_male : list[Polyline], default []
        Joinery polylines for custom side-to-side edge rounded male joints.
    custom_joints_ss_e_r_female : list[Polyline], default []
        Joinery polylines for custom side-to-side edge rounded female joints.
    custom_joints_b_male : list[Polyline], default []
        Joinery polylines for custom base male joints.
    custom_joints_b_female : list[Polyline], default []
        Joinery polylines for custom base female joints.

    """

    def __new__(cls):

        if not hasattr(cls, "instance"):
            cls.instance = super(Globals, cls).__new__(cls)

        return cls.instance

    @property
    def clipper_scale(cls):
        return GLOBALS.CLIPPER_SCALE

    @clipper_scale.setter
    def clipper_scale(cls, value):
        GLOBALS.CLIPPER_SCALE = value

    @property
    def clipper_area(cls):
        return GLOBALS.CLIPPER_AREA

    @clipper_area.setter
    def clipper_area(cls, value):
        GLOBALS.CLIPPER_AREA = value

    @property
    def distance(cls):
        return GLOBALS.DISTANCE

    @distance.setter
    def distance(cls, value):
        GLOBALS.DISTANCE = value

    @property
    def distance_squared(cls):
        return GLOBALS.DISTANCE_SQUARED

    @distance_squared.setter
    def distance_squared(cls, value):
        GLOBALS.DISTANCE_SQUARED = value

    @property
    def angle(cls):
        return GLOBALS.ANGLE

    @angle.setter
    def angle(cls, value):
        GLOBALS.ANGLE = value

    @property
    def path_and_file_for_joints(cls):
        return GLOBALS.PATH_AND_FILE_FOR_JOINTS

    @path_and_file_for_joints.setter
    def path_and_file_for_joints(cls, value):
        GLOBALS.PATH_AND_FILE_FOR_JOINTS = value

    @property
    def data_set_input_folder(cls):
        return GLOBALS.DATA_SET_INPUT_FOLDER

    @data_set_input_folder.setter
    def data_set_input_folder(cls, value):
        GLOBALS.DATA_SET_INPUT_FOLDER = value

    @property
    def data_set_output_file(cls):
        return GLOBALS.DATA_SET_OUTPUT_FILE

    @data_set_output_file.setter
    def data_set_output_file(cls, value):
        GLOBALS.DATA_SET_OUTPUT_FILE = value

    @property
    def data_set_output_database(cls):
        return GLOBALS.DATA_SET_OUTPUT_DATABASE

    @data_set_output_database.setter
    def data_set_output_database(cls, value):
        GLOBALS.DATA_SET_OUTPUT_DATABASE = value

    @property
    def joint_volume_extension(cls):
        from wood_nano.conversions_python import from_int1

        return from_int1(GLOBALS.JOINT_VOLUME_EXTENSION)

    @joint_volume_extension.setter
    def joint_volume_extension(cls, value):
        from wood_nano.conversions_python import to_int1

        GLOBALS.JOINT_VOLUME_EXTENSION = to_int1(value)

    @property
    def output_geometry_type(cls):
        return GLOBALS.OUTPUT_GEOMETRY_TYPE

    @output_geometry_type.setter
    def output_geometry_type(cls, value):
        GLOBALS.OUTPUT_GEOMETRY_TYPE = value

    @property
    def face_to_face_side_to_side_joints_all_treated_as_rotated(cls):
        return GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ALL_TREATED_AS_ROTATED

    @face_to_face_side_to_side_joints_all_treated_as_rotated.setter
    def face_to_face_side_to_side_joints_all_treated_as_rotated(cls, value):
        GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ALL_TREATED_AS_ROTATED = value

    @property
    def face_to_face_side_to_side_joints_rotated_joint_as_average(cls):
        return GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ROTATED_JOINT_AS_AVERAGE

    @face_to_face_side_to_side_joints_rotated_joint_as_average.setter
    def face_to_face_side_to_side_joints_rotated_joint_as_average(cls, value):
        GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_ROTATED_JOINT_AS_AVERAGE = value

    @property
    def face_to_face_side_to_side_joints_dihedral_angle(cls):
        return GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_DIHEDRAL_ANGLE

    @face_to_face_side_to_side_joints_dihedral_angle.setter
    def face_to_face_side_to_side_joints_dihedral_angle(cls, value):
        GLOBALS.FACE_TO_FACE_SIDE_TO_SIDE_JOINTS_DIHEDRAL_ANGLE = value

    @property
    def limit_min_joint_length(cls):
        return GLOBALS.LIMIT_MIN_JOINT_LENGTH

    @limit_min_joint_length.setter
    def limit_min_joint_length(cls, value):
        GLOBALS.LIMIT_MIN_JOINT_LENGTH = value

    @property
    def existing_types(cls):
        from wood_nano.conversions_python import from_string1

        return from_string1(GLOBALS.EXISTING_TYPES)

    @property
    def joints_parameters_and_types(cls):
        from wood_nano.conversions_python import from_double1

        return from_double1(GLOBALS.JOINTS_PARAMETERS_AND_TYPES)

    @joints_parameters_and_types.setter
    def joints_parameters_and_types(cls, value):
        from wood_nano.conversions_python import to_double1

        GLOBALS.JOINTS_PARAMETERS_AND_TYPES = to_double1(value)

    @property
    def custom_joints_ss_e_ip_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_IP_MALE)

    @custom_joints_ss_e_ip_male.setter
    def custom_joints_ss_e_ip_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_IP_MALE = to_point2(value)

    @property
    def custom_joints_ss_e_ip_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_IP_FEMALE)

    @custom_joints_ss_e_ip_female.setter
    def custom_joints_ss_e_ip_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_IP_FEMALE = to_point2(value)

    @property
    def custom_joints_ss_e_op_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_OP_MALE)

    @custom_joints_ss_e_op_male.setter
    def custom_joints_ss_e_op_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_OP_MALE = to_point2(value)

    @property
    def custom_joints_ss_e_op_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_OP_FEMALE)

    @custom_joints_ss_e_op_female.setter
    def custom_joints_ss_e_op_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_OP_FEMALE = to_point2(value)

    @property
    def custom_joints_ts_e_p_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_TS_E_P_MALE)

    @custom_joints_ts_e_p_male.setter
    def custom_joints_ts_e_p_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_TS_E_P_MALE = to_point2(value)

    @property
    def custom_joints_ts_e_p_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_TS_E_P_FEMALE)

    @custom_joints_ts_e_p_female.setter
    def custom_joints_ts_e_p_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_TS_E_P_FEMALE = to_point2(value)

    @property
    def custom_joints_cr_c_ip_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_CR_C_IP_MALE)

    @custom_joints_cr_c_ip_male.setter
    def custom_joints_cr_c_ip_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_CR_C_IP_MALE = to_point2(value)

    @property
    def custom_joints_cr_c_ip_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_CR_C_IP_FEMALE)

    @custom_joints_cr_c_ip_female.setter
    def custom_joints_cr_c_ip_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_CR_C_IP_FEMALE = to_point2(value)

    @property
    def custom_joints_tt_e_p_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_TT_E_P_MALE)

    @custom_joints_tt_e_p_male.setter
    def custom_joints_tt_e_p_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_TT_E_P_MALE = to_point2(value)

    @property
    def custom_joints_tt_e_p_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_TT_E_P_FEMALE)

    @custom_joints_tt_e_p_female.setter
    def custom_joints_tt_e_p_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_TT_E_P_FEMALE = to_point2(value)

    @property
    def custom_joints_ss_e_r_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_R_MALE)

    @custom_joints_ss_e_r_male.setter
    def custom_joints_ss_e_r_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_R_MALE = to_point2(value)

    @property
    def custom_joints_ss_e_r_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_SS_E_R_FEMALE)

    @custom_joints_ss_e_r_female.setter
    def custom_joints_ss_e_r_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_SS_E_R_FEMALE = to_point2(value)

    @property
    def custom_joints_b_male(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_B_MALE)

    @custom_joints_b_male.setter
    def custom_joints_b_male(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_B_MALE = to_point2(value)

    @property
    def custom_joints_b_female(cls):
        from compas_wood.conversions_compas import from_point2

        return from_point2(GLOBALS.CUSTOM_JOINTS_B_FEMALE)

    @custom_joints_b_female.setter
    def custom_joints_b_female(cls, value):
        from compas_wood.conversions_compas import to_point2

        GLOBALS.CUSTOM_JOINTS_B_FEMALE = to_point2(value)


wood_globals = Globals()
