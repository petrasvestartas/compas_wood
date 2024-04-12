from compas_wood.joinery import get_connection_zones
from compas.colors import Color
from compas_viewer import Viewer
from compas.geometry import Polyline, Polygon
from compas.geometry import Scale

input_polylines = [
    [[0, -100, -60], [175, -100, -60], [175, 100, -60], [0, 100, -60], [0, -100, -60]],
    [[0, -100, 60], [175, -100, 60], [175, 100, 60], [0, 100, 60], [0, -100, 60]],
    [[-175, -100, -60], [0, -100, -60], [0, 100, -60], [-175, 100, -60], [-175, -100, -60]],
    [[-175, -100, 60], [0, -100, 60],  [0, 100, 60], [-175, 100, 60], [-175, -100, 60]],
]

division_length = 300
joint_parameters = [
    2, 0.5, 2,  # 1-9 ss_e_ip (side-to-side edge in-plane)
    450, 0.65, 10,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
    450, 0.5, 21,  # 20-29 ts_e_p (top-to-side edge plane)
    300, 0.95, 30,  # 30-39 cr_c_ip (cross cutting in-plane)
    300, 0.95, 40,  # 40-49 tt_e_p  (top-to-top edge plane)
    300, 0.95, 59,  # 50-59 ss_e_r (side-to-side edge rotated)
    300, 1.0, 60,   # 60-69 b (boundary)
]

# input custom joints, polyliines must be female and male

custom_joints = []
custom_types = []
custom_joint_type = None  # 59 if custom_joint_type is None else custom_joint_type

# for i in range(len(custom_joint_polylines_0)):
#     coordinate_list = []
#     for point in custom_joint_polylines_0[i]:
#         coordinate_list.append([point.X, point.Y, point.Z])
#     custom_joints.append(coordinate_list)
#     custom_types.append(custom_joint_type)

# for i in range(len(custom_joint_polylines_1)):
#     coordinate_list = []
#     for point in custom_joint_polylines_1[i]:
#         coordinate_list.append([point.X, point.Y, point.Z])
#     custom_joints.append(coordinate_list)
#     custom_types.append(-custom_joint_type)

# call to c++ 
adjacency_flat = []
# for o in adjacency:
#     adjacency_flat.extend(o)
custom_joints = []
custom_types = []
result = get_connection_zones(
    input_polyline_pairs=input_polylines,  # output_polylines,
    input_insertion_vectors=None,  # output_vectors,
    input_joints_types=None,  # output_joints_types,
    input_three_valence_element_indices_and_instruction=None,  # output_three_valence_element_indices_and_instruction,
    input_adjacency=adjacency_flat,  # output_adjacency,
    joint_parameters_and_types=joint_parameters,
    search_type=0,
    scale=[1, 1, 1],
    output_type=4,  # 0 - Plate outlines, 1 - wood::joint lines, 2 - wood::joint areas, 3 - wood::joint areas with bounding box, 4 - wood::joint areas with bounding box and cut types
    flatten_output=False,
    joint_volume_extension=[0, 0, 0],
    face_to_face_side_to_side_joints_all_treated_as_rotated=False,
    custom_joints=custom_joints,
    custom_joints_types=custom_types
)

print(result)

viewer = Viewer()
scale = Scale.from_factors([0.05, 0.05, 0.05])
for idx, polylines in enumerate(result):
    for polyline in polylines:
        polyline.points.pop()
        polyline.transform(scale)
        viewer.add(Polygon(polyline.points))
viewer.show()

# display
# rhino_polylines_list = []
# rhino_meshes_list = []
# for idx, polylines in enumerate(result):
#     rhino_polylines = []
#     for polyline in polylines:
#         rhino_polyline = Polyline()
#         for point in polyline:
#             rhino_polyline.Add(Point3d(point[0], point[1], point[2]))
#         rhino_polylines.append(rhino_polyline)

#     rhino_polylines.append(top_polylines[idx])
#     rhino_polylines.append(bot_polylines[idx])
#     rhino_polylines_list.append(rhino_polylines)
    
#     rhino_meshes = loft_polylines_list(rhino_polylines)
#     rhino_meshes_list.append(rhino_meshes)
#     #rhino_meshes.append(loft_polylines_with_holes([top_polylines[idx]], [bot_polylines[idx]]))

# # result_flat_list = [item for sublist in result for item in sublist]
# # return result_flat_list
# return rhino_polylines_list, rhino_meshes_list # 