from wood_nano import read_xml_polylines_and_properties as wood_nano_read_xml_polylines_and_properties
from wood_nano import double2
from wood_nano import int2
from wood_nano import int1
from wood_nano.conversions_python import from_int1
from wood_nano.conversions_python import from_int2
from compas.geometry import Polyline
from compas.geometry import Scale
from compas.geometry import Line
from compas.geometry import Vector


def read_xml_polylines_and_properties(
    foldername="/home/petras/brg/2_code/wood_nano/src/wood/cmake/src/wood/dataset/",
    filename_of_dataset="type_plates_name_top_to_side_and_side_to_side_outofplane_annen_grid_small",
    scale=1e-2,
):
    """Read polylines from XML file with attributes:

    -   insertion vectors per face

    -   input_joints_types per face

    -   input_three_valence_element_indices_and_instruction

    -   input_adjacency per curren and next elements and their faces.


    Parameters
    ----------
    foldername : str, optional
        Foldername.
    filename_of_dataset : str, optional
        Filename of dataset.
    scale : float, optional
        Scale.

    Returns
    -------
    list[compas.geometry.Polyline]
        List of polylines.
    list[list[compas.geometry.Vector]]
        List of vectors.
    list[list[int]]
        List of joint types.
    list[list[int]]
        List of three valence element indices and instruction.
    list[int]
        List of adjacency.

    """
    input_polyline_pairs_coord = double2()
    input_insertion_vectors_coord = double2()
    input_joints_types = int2()
    input_three_valence_element_indices_and_instruction = int2()
    input_adjacency = int1()

    wood_nano_read_xml_polylines_and_properties(
        foldername,
        filename_of_dataset,
        input_polyline_pairs_coord,
        input_insertion_vectors_coord,
        input_joints_types,
        input_three_valence_element_indices_and_instruction,
        input_adjacency,
    )

    polylines = []
    xform = Scale.from_factors([scale, scale, scale])

    for polyline in input_polyline_pairs_coord:
        points = []
        for i in range(0, len(polyline), 3):
            points.append([polyline[i], polyline[i + 1], polyline[i + 2]])
        polyline = Polyline(points)
        polyline.transform(xform)
        polylines.append(polyline)

    vectors_lists = []
    for vector in input_insertion_vectors_coord:
        vector_list = []
        for i in range(0, len(vector), 3):
            vector_list.append(Vector(vector[i], vector[i + 1], vector[i + 2]))
        vectors_lists.append(vector_list)

    return (
        polylines,
        vectors_lists,
        from_int2(input_joints_types),
        from_int2(input_three_valence_element_indices_and_instruction),
        from_int1(input_adjacency),
    )


if __name__ == "__main__":
    (
        polylines,
        vectors_lists,
        input_joints_types,
        input_three_valence_element_indices_and_instruction,
        input_adjacency,
    ) = read_xml_polylines_and_properties()

    import sys

    if sys.version_info >= (3, 9):
        from compas_viewer import Viewer

        viewer = Viewer()

        for i in range(len(vectors_lists)):
            viewer.scene.add(polylines[i * 2 + 0], show_points=False)
            viewer.scene.add(polylines[i * 2 + 1], show_points=False)
            for segment, vector in zip(polylines[i * 2 + 0].lines, vectors_lists[i]):
                midpoint = segment.midpoint
                line = Line(midpoint, midpoint + vector)
                if line.length > 0:
                    viewer.scene.add(line, show_points=False, lineswidth=3)
                else:
                    viewer.scene.add(line.start, show_points=True, points_size=0.02)

        # for i in input_joints_types:
        #     print(i)

        # for i in input_three_valence_element_indices_and_instruction:
        #     print(i)

        # for i in range(0, len(input_adjacency), 4):
        #     print(input_adjacency[i:i + 4])

        viewer.show()
