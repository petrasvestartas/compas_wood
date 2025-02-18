#! python3
# venv: timber_connections
from wood_nano import beam_volumes as wood_nano_beam_volumes
import wood_nano as m

from wood_nano.conversions_python import to_double2
from wood_nano.conversions_python import to_int1
from wood_nano import int1
from wood_nano import int2
from wood_nano import double2
from wood_nano import point2
from wood_nano import point3
from wood_nano import cut_type2
import Rhino
from typing import *
from wood_rui import add_polylines, wood_rui_globals, generalized_input_method, Element

def my_callback(
    name_value_type,
    dataset_name,
    input_flip_male=0,
    input_compute_joints=False,
    input_division_distance=150,
    input_shift=0.5,
    input_output_geometry_type=0,
    input_use_eccentricities_to_scale_joints=True,
):


    input_elements = name_value_type["elements"]

    if len(input_elements) == 0:
        print("Attention: no elements selected.")
        return

    input_polyline_axes = Element.get_first_axes(input_elements)
    input_polylines_segment_radii = Element.get_first_radii(input_elements)
    input_polylines_segment_direction = Element.get_first_insertion(input_elements) if name_value_type["find_insertion"] else []

    input_allowed_types_per_polyline = name_value_type["allowed_types_per_polyline"]
    input_min_distance = name_value_type["min_distance"]
    input_volume_length = name_value_type["volume_length"]
    input_cross_or_side_to_end = name_value_type["cross_or_side_to_end"]  

    index_polylines = int2()
    index_polylines_segment = int2()
    distance = double2()
    point_pairs = point2()
    volume_pairs = point3()
    joints_areas = point2()
    joints_types = int1()
    w_output_plines = point3()
    w_output_types = cut_type2()
   

    def to_point(vectors, depth):
        if depth == 1:  # Single polyline
            polyline = m.point1()
            for point3d in vectors:
                polyline.append(m.point(point3d.X, point3d.Y, point3d.Z))
            return polyline
        else:  # Nested polyline
            polyline = getattr(m, f"point{depth}")()
            for vec in vectors:
                polyline.append(to_point(vec, depth - 1))
            return polyline

    def to_point2(polylines):
        return to_point(polylines, 2)

    def to_vector(vectors, depth):
        if depth == 1:  # Single vector_of_vectors
            vector_of_vectors = m.vector1()
            for vector3d in vectors:
                vector_of_vectors.append(m.vector(vector3d.X, vector3d.Y, vector3d.Z))
            return vector_of_vectors
        else:  # Nested vector_of_vectors
            vector = getattr(m, f"vector{depth}")()
            for vec in vectors:
                vector.append(to_vector(vec, depth - 1))
            return vector

    def to_vector2(vectors):
        return to_vector(vectors, 2)

    if len(input_polyline_axes) == 0:
        print("Attention: No axes found.")
        return

    if len(input_polylines_segment_radii) == 0:
        print("Attention: No radii found.")
        return

    wood_nano_beam_volumes(
        to_point2(input_polyline_axes),  # input_polylines,
        to_double2(input_polylines_segment_radii),
        to_vector2(input_polylines_segment_direction),  # input_polylines_segment_direction,
        to_int1(input_allowed_types_per_polyline),
        float(input_min_distance),
        float(input_volume_length),
        float(input_cross_or_side_to_end),
        int(input_flip_male),
        index_polylines,  # out
        index_polylines_segment,  # out
        distance,  # out
        point_pairs,  # out
        volume_pairs,  # out
        joints_areas,  # out
        joints_types,  # out
        w_output_plines,  # from_point3(w_output_plines)
        w_output_types,  # from_cut_type2(w_output_types))
        input_compute_joints,
        input_division_distance,
        input_shift,
        input_output_geometry_type,
        input_use_eccentricities_to_scale_joints,
    )

    ######################################################################
    # Convert Solver output: beam volumes and indices.
    ######################################################################
    def from_point1(point1):
        points = []
        for i in range(len(point1)):
            point = point1[i]
            points.append(Rhino.Geometry.Point3d(point[0], point[1], point[2]))
        return Rhino.Geometry.Polyline(points)

    def from_point2(point2):
        return [from_point1(p) for p in point2]

    def from_point3(point3):
        return [from_point2(p) for p in point3]

    output_volume_pairs = from_point3(volume_pairs)

    output_volume_pairs_flat = []

    for idx, o in enumerate(output_volume_pairs):
        output_volume_pairs_flat.extend(o)

    group_indices = []
    for polylines_indices in index_polylines:
        for index in polylines_indices:
            group_indices.append(index)
            group_indices.append(index)
    
    ######################################################################
    # Split Volumes by index.
    # Store the 2 polylines in the separates lists.
    # The volumes are written as user strings on the rhino geometry object.
    ######################################################################
    elements_volumes = [[] for _ in input_elements]

    for i in range(0, len(group_indices), 4):
        elements_volumes[group_indices[i]].extend(output_volume_pairs_flat[i:i+2])
        elements_volumes[group_indices[i+2]].extend(output_volume_pairs_flat[i+2:i+4])

    for element, volumes in zip(input_elements, elements_volumes):
        element.volumes = volumes

    ######################################################################
    # Set element indices:
    # a) current element index.
    # b) neighbors of the element.
    ######################################################################

    neighbours = {}
    for idx, element in enumerate(input_elements):
        element.index = idx
        neighbours[idx] = []

    for polylines_indices in index_polylines:
        neighbours[polylines_indices[0]].append([polylines_indices[1],-1])
        neighbours[polylines_indices[1]].append([polylines_indices[0],-1])
    
    for key, item in neighbours.items():
        input_elements[key].neighbours = item


    ######################################################################
    # Add element volumes:
    # [[[Rhino.Geometry.Polyline, Rhino.Geometry.Polyline], ...]] to Rhino.
    ######################################################################
    polylines = [e.volumes for e in input_elements]
    add_polylines(polylines, "model", "joint_volumes")
        

if __name__ == "__main__":

    # Select Beams

    # Define the input dictionary based on your initial dataset
    input_dict = {
        "elements" : ([], List[Element]),
        "allowed_types_per_polyline": ([-1], List[int]),  # Default value for types (list of ints)
        "min_distance": (100, float),  # Default value for min distance (float)
        "volume_length": (500, float),  # Default value for volume length (float)
        "cross_or_side_to_end": (0.91, float),  # Default value for cross or side to end (float)
        "find_insertion" : (False, bool)  # if the find direction is used the beam volumes are computed orthogonal to one another, else beam directions are used
        # "flip_male": (1, int),  # Default value for flip
        # "compute_joints": (False, bool),  # Default value for compute joints
        # "division_distance": (150, float),  # Default value for division distance
        # "shift": (0.5, float),  # Default value for shift
        # "output_geometry_type": (0, int),  # Default value for output geometry type
        # "use_eccentricities_to_scale_joints": (True, bool),  # Default value for use eccentricities to scale joints
    }

    # Call the generalized input method with the dataset name and input dictionary
    dataset_name = "beams"
    wood_rui_globals.init_data(dataset_name)
    generalized_input_method(dataset_name, input_dict, my_callback)
