from ghpythonlib.componentbase import executingcomponent as component
import Rhino
import System
from wood_nano import get_connection_zones as wood_nano_get_connection_zones
from wood_nano import cut_type2 as wood_nano_cut_type2
from wood_nano import point3 as wood_nano_point3
from wood_nano.conversions_python import to_int2
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import to_double1
from wood_nano.conversions_python import from_cut_type2
from compas_wood.conversions_rhino import to_point1
from compas_wood.conversions_rhino import to_point2
from compas_wood.conversions_rhino import to_vector2
from compas_wood.conversions_rhino import from_point3
from compas_wood.binding import wood_globals
from compas_wood.datastructures import WoodData
from math import floor


class connections_zones(component):
    def __init__(self):
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        self.lines = []
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.polylines = []

    def DrawViewportWires(self, args):
        col = args.WireColour
        line_weight = args.DefaultCurveThickness
        for polylines_list in self.polylines:
            for polyline in polylines_list:
                args.Display.DrawPolyline(polyline, col, line_weight)

    def get_ClippingBox(self):
        return self.bbox

    def RunScript(
        self,
        _data,
        _joint_p: System.Collections.Generic.IList[float],
        _scale: System.Collections.Generic.IList[float],
        _extension: System.Collections.Generic.IList[float],
        _find: int,
        _get: int,
        _custom_joints: System.Collections.Generic.IList[Rhino.Geometry.Polyline],
        _custom_types: System.Collections.Generic.IList[int],
    ):

        # ==============================================================================
        # clear input
        # ==============================================================================
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        self.lines = []
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.polylines = []

        # ==============================================================================
        # input
        # ==============================================================================
        if _data is None:
            return

        input_polyline_pairs = _data.plines
        if input_polyline_pairs:
            if len(input_polyline_pairs) > 0:
                self.bbox = input_polyline_pairs[0].BoundingBox
                for pline in input_polyline_pairs:
                    self.bbox.Union(pline.BoundingBox)

        input_insertion_vectors = _data.insertion_vectors

        input_joint_types = _data.joints_per_face
        input_three_valence_element_indices_and_instruction = _data.three_valence

        input_search_type = max(min(2, _find), 0) if _find else 0

        input_scale = [1, 1, 1]
        if _scale:
            for i in range(0, len(_scale)):
                input_scale[i] = _scale[i]

        input_output_type = _get if _get is not None else 4

        input_joint_volume_parameters = [0, 0, 0]

        if _extension:
            if len(_extension) > 2:
                input_joint_volume_parameters = []
                for i in _extension:
                    input_joint_volume_parameters.append(i * 10)

        input_adjacency = _data.adjacency

        joint_parameters = wood_globals.joints_parameters_and_types

        if _joint_p:
            for i in range(0, len(_joint_p), 3):
                insertion_id = floor(_joint_p[i + 2] / 10)
                joint_parameters[insertion_id * 3 + 0] = _joint_p[i + 0]
                joint_parameters[insertion_id * 3 + 1] = _joint_p[i + 1]
                joint_parameters[insertion_id * 3 + 2] = _joint_p[i + 2]

        w_output_plines = wood_nano_point3()
        w_output_types = wood_nano_cut_type2()

        if _custom_joints and _custom_types:
            if len(_custom_joints) == len(_custom_types):
                custom_joints_dict = {
                    9: [],
                    -9: [],
                    -19: [],
                    19: [],
                    29: [],
                    -29: [],
                    39: [],
                    -39: [],
                    49: [],
                    -49: [],
                    59: [],
                    -59: [],
                    69: [],
                    -69: [],
                }
                for i in range(len(_custom_joints)):
                    polyline = to_point1(_custom_joints[i])
                    custom_joints_dict[_custom_types[i]].append(polyline)

                for key, value in custom_joints_dict.items():
                    if key == 9:
                        wood_globals.custom_joints_ss_e_ip_male = value
                    elif key == -9:
                        wood_globals.custom_joints_ss_e_ip_female = value
                    elif key == 19:
                        wood_globals.custom_joints_ss_e_op_male = value
                    elif key == -19:
                        wood_globals.custom_joints_ss_e_op_female = value
                    elif key == 29:
                        wood_globals.custom_joints_ts_e_p_male = value
                    elif key == -29:
                        wood_globals.custom_joints_ts_e_p_female = value
                    elif key == 39:
                        wood_globals.custom_joints_cr_c_ip_male = value
                    elif key == -39:
                        wood_globals.custom_joints_cr_c_ip_female = value
                    elif key == 49:
                        wood_globals.custom_joints_tt_e_p_male = value
                    elif key == -49:
                        wood_globals.custom_joints_tt_e_p_female = value
                    elif key == 59:
                        wood_globals.custom_joints_ss_e_r_male = value
                    elif key == -59:
                        wood_globals.custom_joints_ss_e_r_female = value
                    elif key == 69:
                        wood_globals.custom_joints_b_male = value
                    elif key == -69:
                        wood_globals.custom_joints_b_female = value

        wood_nano_get_connection_zones(
            to_point2(input_polyline_pairs),
            to_vector2(input_insertion_vectors),
            to_int2(input_joint_types),
            to_int2(input_three_valence_element_indices_and_instruction),
            to_int1(input_adjacency),
            to_double1(joint_parameters),
            int(input_search_type),
            to_double1(input_scale),
            int(input_output_type),
            w_output_plines,
            w_output_types,
            to_double1(input_joint_volume_parameters),
        )

        self.polylines = from_point3(w_output_plines)

        return WoodData(
            plines=_data.plines,
            insertion_vectors=_data.insertion_vectors,
            joints_per_face=_data.joints_per_face,
            three_valence=_data.three_valence,
            adjacency=_data.adjacency,
            planes=_data.planes,
            plines_with_joints=self.polylines,
            cut_types=from_cut_type2(w_output_types),
        )
