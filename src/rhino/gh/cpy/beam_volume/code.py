# from ghpythonlib.componentbase import executingcomponent as component
# import Grasshopper

# import Rhino
# from Rhino.Geometry import (
#     Point3d,
#     Vector3d,
#     Polyline,
#     Curve,
#     Line,
#     Plane,
#     Brep,
#     BoundingBox,
#     Interval,
#     RTree,
#     Transform,
#     Rectangle3d,
# )
# from collections import OrderedDict
# from Grasshopper import DataTree
# from Grasshopper.Kernel.Data import GH_Path
# import math


# class MyComponent(component):
#     def RunScript(
#         self,
#         _circles: Grasshopper.DataTree[Rhino.Geometry.Circle],
#         _length_of_box: float,
#         _distance_tolerance: float,
#         _parallel_tolerance: float,
#         _cross_or_toptoside: bool,
#         _create_pipes: bool,
#     ):


#         input_polylines_segment_radii = []

#         for i in range(len(input_polylines)):
#             input_polyline_segment_radii = []
#             for j in range(len(input_polylines[i].points)):
#                 input_polyline_segment_radii.append(10)
#             input_polylines_segment_radii.append(input_polyline_segment_radii)

#         input_polylines_segment_direction = []
#         for i in range(len(input_polylines)):
#             input_polyline_segment_direction = []
#             for j in range(len(input_polylines[i].points)-1):
#                 input_polyline_segment_direction.append([0, 1, 0])
#             input_polylines_segment_direction.append(input_polyline_segment_direction)


#         i_polylines, i_polylines_segment, distance, points, volume_pairs, joints_areas, joints_types = beam_volumes(
#                 input_polylines,
#                 input_polylines_segment_radii,
#                 input_polylines_segment_direction,
#                 input_allowed_types_per_polyline=[-1],
#                 input_min_distance=20,
#                 input_volume_length=75,
#                 input_cross_or_side_to_end=0.91,
#     )
