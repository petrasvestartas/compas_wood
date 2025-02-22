#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import add_sub_layer
from wood_rui import Element
from System.Drawing import Color
from compas_wood.binding import wood_globals

def duplicate(data : list[Rhino.Geometry.Polyline]):
    duplicated_data = []
    for d in data:
        if d.Count == 2:
            duplicated_data.append(d)
            duplicated_data.append(d)
        else:
            duplicated_data.append(d)

    return(duplicated_data)



def callback(selection: dict[str, any], name: str):

    for key, item in selection.items():
        
        key_to_attr = {
            "ss_e_ip_positive": "custom_joints_ss_e_ip_male",
            "ss_e_ip_negative": "custom_joints_ss_e_ip_female",
            "ss_e_op_positive": "custom_joints_ss_e_op_male",
            "ss_e_op_negative": "custom_joints_ss_e_op_female",
            "ts_e_p_positive": "custom_joints_ts_e_p_male",
            "ts_e_p_negative": "custom_joints_ts_e_p_female",
            "cr_c_ip_positive": "custom_joints_cr_c_ip_male",
            "cr_c_ip_negative": "custom_joints_cr_c_ip_female",
            "tt_e_p_positive": "custom_joints_tt_e_p_male",
            "tt_e_p_negative": "custom_joints_tt_e_p_female",
            "ss_e_r_positive": "custom_joints_ss_e_r_male",
            "ss_e_r_negative": "custom_joints_ss_e_r_female",
            "b_positive": "custom_joints_b_male",
            "b_negative": "custom_joints_b_female",
        }

        for key, item in selection.items():
            if key in key_to_attr:
                attr_name = key_to_attr[key]
                duplicated_item = duplicate(item)
                setattr(wood_globals, attr_name, duplicated_item)
                print(f"{attr_name}: {duplicated_item}")

    return


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "ss_e_ip_positive": ([], list[Rhino.Geometry.Polyline]),
        "ss_e_ip_negative": ([], list[Rhino.Geometry.Polyline]),
        "ss_e_op_positive": ([], list[Rhino.Geometry.Polyline]),
        "ss_e_op_negative": ([], list[Rhino.Geometry.Polyline]),
        "ts_e_p_positive": ([], list[Rhino.Geometry.Polyline]),
        "ts_e_p_negative": ([], list[Rhino.Geometry.Polyline]),
        "cr_c_ip_positive": ([], list[Rhino.Geometry.Polyline]),
        "cr_c_ip_negative": ([], list[Rhino.Geometry.Polyline]),
        "tt_e_p_positive": ([], list[Rhino.Geometry.Polyline]),
        "tt_e_p_negative": ([], list[Rhino.Geometry.Polyline]),
        "ss_e_r_positive": ([], list[Rhino.Geometry.Polyline]),
        "ss_e_r_negative": ([], list[Rhino.Geometry.Polyline]),
        "b_positive": ([], list[Rhino.Geometry.Polyline]),
        "b_negative": ([], list[Rhino.Geometry.Polyline]),
    }

    process_input(selection_types, callback)
