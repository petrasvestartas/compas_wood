# compas_wood
from compas_wood.joinery import get_connection_zones
import data_set_plates
from compas_wood.joinery import closed_mesh_from_polylines


# compas_view2
from compas_view2.app import App
from compas_view2.collections import Collection

viewer = App(viewmode="shaded", enable_sidebar=True, width=3840, height=2160 - 250)
viewer.view.camera.distance = 2000
viewer.view.camera.near = 10
viewer.view.camera.far = 100000


# rotation_delta = 1
# pan_delta = 0.05
# viewer.view.camera.tx = 0
# viewer.view.camera.ty = 0
# viewer.view.camera.tz = -10
# viewer.view.camera.rz = 0
# viewer.view.camera.rx = -20
type_id = 43
shift = 0.5
divisions_distance = 1000
result = []

"""
@viewer.slider(text="S", minval=0, maxval=100, value=50, step=1)
def slide1(value):
    listOfGlobals = globals()
    listOfGlobals["shift"] = value / 100.0
    viewer.view.objects.clear()
    global type_id
    global divisions_distance
    global shift
    add_joint_geometry(type_id, divisions_distance, shift)
    viewer.view.update()


@viewer.slider(text="D", minval=100, maxval=1000, value=500, step=1)
def slide0(value):
    viewer.view.objects.clear()
    listOfGlobals = globals()
    listOfGlobals["divisions_distance"] = value
    viewer.view.objects.clear()
    global type_id
    global divisions_distance
    global shift
    add_joint_geometry(type_id, divisions_distance, shift)
    viewer.view.update()
"""


@viewer.button(text="mesh")
def click_mesh():

    count = 0
    meshes = []
    global result

    for i in range(len(result)):
        mesh_result = closed_mesh_from_polylines(result[i])

        # print("Mesh" + (str)(count))
        count += 1
        if mesh_result.is_valid() and len(list(mesh_result.vertices())) > 0:
            meshes.append(mesh_result)

    if len(meshes) > 0:
        viewer.add(
            Collection(meshes),
            hide_coplanaredges=False,
            opacity=0.9,
            # color=(104 / 255, 219 / 255, 3),
            color=(0.95, 0.95, 0.95),
        )

    viewer.view.update()


# global_id = 1
available_joints = {
    0: "c_0",
    1: "c_1",
    2: "c_2",
    3: "c_3",
    4: "c_4",
    5: "c_5",
    6: "c_6",
    7: "c_7",
    8: "c_8",
    9: "c_9",
    10: "c_10",
    11: "c_11",
    12: "c_12",
    13: "c_13",
    14: "c_14",
    15: "c_15",
    16: "c_16",
    17: "c_17",
    18: "ss_0",
    19: "ss_1",
    20: "ss_2",
    21: "ss_3",
    22: "ss_4",
    23: "ss_5",
    24: "ss_6",
    25: "ss_7",
    26: "ss_8",
    27: "ss_9",
    28: "ss_10",
    29: "ss_11",
    30: "ss_12",
    31: "ss_13",
    32: "ss_14",
    33: "ss_15",
    34: "ss_16",
    35: "ss_17",
    36: "ss_18",
    37: "ss_19",
    38: "ss_20",
    39: "ss_21",
    40: "ss_22",
    41: "ss_23",
    42: "ss_24",
    43: "annen_0",
    44: "ss_25",
    45: "ss_26",
    46: "ts_0",
    47: "ts_1",
    48: "ts_2",
    49: "ts_3",
    50: "ts_4",
    51: "ts_5",
    52: "ts_6",
    53: "vda_0",
    54: "vda_1",
    55: "vda_2",
    56: "bad_input_0",
    57: "annen_three_boxes",
}
for i, text in available_joints.items():

    @viewer.button(text=text, number=i)
    def click(id=i, local_number=type_id):
        viewer.view.objects.clear()
        global type_id
        listOfGlobals = globals()
        listOfGlobals["type_id"] = id

        # print(type_id)
        add_joint_geometry(id)
        viewer.view.update()


def select(value):
    viewer.view.mode = value
    viewer.view.update()


def add_joint_geometry(type, division_dist=1000, shift=0.6):

    # Input pairs of polylines
    input = []
    result = []
    if type == 0:
        input = data_set_plates.c_0()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 1:
        input = data_set_plates.c_1()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 2:
        input = data_set_plates.c_2()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 3:
        input = data_set_plates.c_3()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 4:
        input = data_set_plates.c_4()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 5:
        input = data_set_plates.c_5()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 6:
        input = data_set_plates.c_6()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 7:
        input = data_set_plates.c_7()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 8:
        input = data_set_plates.c_8()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 9:
        input = data_set_plates.c_9()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 10:
        input = data_set_plates.c_10()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 11:
        input = data_set_plates.c_11()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 12:
        input = data_set_plates.c_12()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 13:
        input = data_set_plates.c_13()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 14:
        input = data_set_plates.c_14()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 15:
        input = data_set_plates.c_15()
        result = get_connection_zones(input, None, None, None, None, 2)
    elif type == 16:
        input = data_set_plates.c_16()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 17:
        input = data_set_plates.c_17()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 18:
        input = data_set_plates.ss_0()
        joint_paramters = [
            15,
            0.5,
            1,
            50,
            0.5,
            10,
            50,
            0.5,
            20,
            50,
            0.5,
            30,
            50,
            0.5,
            40,
            50,
            0.5,
            50,
        ]
        result = get_connection_zones(input, None, None, None, joint_paramters, 0)
    elif type == 19:
        input = data_set_plates.ss_1()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 20:
        input = data_set_plates.ss_2()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 21:
        input = data_set_plates.ss_3()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 22:
        input = data_set_plates.ss_4()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 23:
        input = data_set_plates.ss_5()
        joint_paramters = [
            15,
            0.5,
            9,
            50,
            0.5,
            10,
            50,
            0.5,
            20,
            50,
            0.5,
            30,
            50,
            0.5,
            40,
            50,
            0.5,
            50,
        ]
        result = get_connection_zones(input, None, None, None, joint_paramters)
    elif type == 24:
        input = data_set_plates.ss_6()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 25:

        joint_parameters = [
            15,
            0.5,
            9,
            50,
            0.5,
            10,
            50,
            0.5,
            20,
            50,
            0.5,
            30,
            50,
            0.5,
            40,
            50,
            0.5,
            50,
        ]
        input = data_set_plates.ss_7()
        result = get_connection_zones(input, None, None, None, joint_parameters, 0)
    elif type == 26:

        joint_parameters = [
            15,
            0.5,
            9,
            50,
            0.5,
            10,
            50,
            0.5,
            20,
            50,
            0.5,
            30,
            50,
            0.5,
            40,
            50,
            0.5,
            50,
        ]
        input = data_set_plates.ss_8()
        result = get_connection_zones(
            input, None, None, None, joint_parameters, 0, 300, 0.6, 2
        )
    elif type == 27:
        input = data_set_plates.ss_9()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 28:
        input = data_set_plates.ss_10()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 29:
        input = data_set_plates.ss_11()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 30:
        input = data_set_plates.ss_12()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 31:
        input = data_set_plates.ss_13()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 32:
        input = data_set_plates.ss_14()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 33:
        input = data_set_plates.ss_15()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 34:
        input = data_set_plates.ss_16()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 35:
        input = data_set_plates.ss_17()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 36:
        input = data_set_plates.ss_18()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 37:
        input = data_set_plates.ss_19()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 38:
        input = data_set_plates.ss_20()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 39:
        input = data_set_plates.ss_21()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 40:
        input = data_set_plates.ss_22()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 41:
        input = data_set_plates.ss_23()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 42:
        input = data_set_plates.ss_24()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 43:

        division_length = 300
        joint_parameters = [
            division_length,
            0.5,
            9,
            division_length * 1.5,
            0.65,
            10,
            division_length,
            0.5,
            21,
            division_length,
            0.5,
            30,
            division_length,
            0.5,
            40,
            division_length,
            0.5,
            15,
        ]

        result = get_connection_zones(
            data_set_plates.annen_small_polylines(),
            data_set_plates.annen_small_edge_directions(),
            data_set_plates.annen_small_edge_joints(),
            data_set_plates.annen_small_three_valance_element_indices_and_instruction(),
            joint_parameters,
            0,
            300,
            0.6,
            4,
        )
    elif type == 44:
        input = data_set_plates.ss_25()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 45:
        input = data_set_plates.ss_26()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 46:
        input = data_set_plates.ts_0()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 47:
        input = data_set_plates.ts_1()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 48:
        input = data_set_plates.ts_2()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 49:
        input = data_set_plates.ts_3()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 50:
        input = data_set_plates.ts_4()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 51:
        input = data_set_plates.ts_5()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 52:
        input = data_set_plates.ts_6()
        result = get_connection_zones(input, None, None, None, None, 0)
    elif type == 53:
        input = data_set_plates.vda_0()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 54:
        input = data_set_plates.vda_1()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 55:
        input = data_set_plates.vda_2()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 56:
        input = data_set_plates.bad_input_0()
        result = get_connection_zones(input, None, None, None, None, 1)
    elif type == 57:
        input = data_set_plates.annen_three_boxes()

        division_length = 300
        joint_parameters = [
            division_length,
            0.5,
            9,
            division_length * 1.5,
            0.65,
            10,
            division_length,
            0.5,
            21,
            division_length,
            0.5,
            30,
            division_length,
            0.5,
            40,
            division_length,
            0.5,
            15,
        ]
        result = get_connection_zones(
            input, None, None, None, joint_parameters, 0, 20, 0.6, 4
        )
    result_flat_list = [item for sublist in result for item in sublist]

    # Generate connections

    # Display via Compas_View2
    # display(input, result_flat_list, None, 0.01, 1.25)

    """
    if input is not None:
        for i in range(0, len(input)):

            if i % 2 == 0:
                display_polyline(viewer, input[i], 1, 1, 0, 0, 3)
            else:
                display_polyline(viewer, input[i], 1, 1, 0, 0, 1)
    """
    listOfGlobals = globals()
    listOfGlobals["result"] = result

    if len(result_flat_list) == 0:
        return

    plines0 = []
    plines1 = []

    if result_flat_list is not None:
        for i in range(0, len(result_flat_list)):
            if i % 2 == 0:
                plines0.append(result_flat_list[i])
                # display_polyline(        viewer, result_flat_list[i], 1, 0, 104 / 255, 219 / 255, 3)

            else:
                plines1.append(result_flat_list[i])
                # display_polyline(    viewer, result_flat_list[i], 1, 0, 104 / 255, 219 / 255, 1   )

    plines0_collection = Collection(plines0)
    plines1_collection = Collection(plines1)

    viewer.add(plines0_collection, linewidth=3, color=[0, 104 / 255, 219 / 255])
    viewer.add(plines1_collection, linewidth=2, color=[0, 104 / 255, 219 / 255])

    # 1 2 9   10 11 12 19   20 21 22 23 29   30 31 39    49    59
    # 1 - weird intersecting segment low distances issues - 22 23


def display_polyline(viewer, polyline, scale=0.01, r=0.0, g=0.0, b=0.0, t=1):
    if len(polyline) > 1:
        viewer.add(polyline, color=(r, g, b), linewidth=t)


viewer.view.objects.clear()
add_joint_geometry(43)

viewer.view.update()
viewer.run()
