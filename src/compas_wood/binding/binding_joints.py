from wood_nano import joints as wood_nano_joints
from wood_nano import int2
from wood_nano import point2
from wood_nano import int1
from compas_wood.conversions_compas import to_point2
from compas_wood.conversions_compas import from_point2
from wood_nano.conversions_python import from_int2
from wood_nano.conversions_python import from_int1
from compas.geometry import Polyline


def joints(
    input_polyline_pairs,
    search_type=0,
):
    """
    Get joints for the given input parameters.
    This is the very first step in the process of creating a timber structure - detection of interfaces.

    Parameters
    ----------
    input_polyline_pairs : list[list[compas.geometry.Polyline]]
        List of polylines that represents the timber elements.
    search_type : int, optional
        Search type.

    Returns
    -------
    list[list[int]]
        List of element pairs.
    list[compas.geometry.Polyline]
        List of joint areas.
    list[int]
        List of joint types.
    """

    wood_nano_input_polyline_pairs = to_point2(input_polyline_pairs)
    element_pairs = int2()
    joint_areas = point2()
    joint_types = int1()

    wood_nano_joints(wood_nano_input_polyline_pairs, search_type, element_pairs, joint_areas, joint_types)

    return from_int2(element_pairs), from_point2(joint_areas), from_int1(joint_types)


if __name__ == "__main__":

    polyline0 = Polyline([[0, 0, 0], [1, 0, 0], [1, 1, 0], [0, 1, 0], [0, 0, 0]])
    polyline1 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
    polyline2 = Polyline([[0, 0, 1], [1, 0, 1], [1, 1, 1], [0, 1, 1], [0, 0, 1]])
    polyline3 = Polyline([[0, 0, 2], [1, 0, 2], [1, 1, 2], [0, 1, 2], [0, 0, 2]])

    from compas.geometry import Rotation

    xform = Rotation.from_axis_and_angle([0, 0, 1], 0.2)
    polyline2.transform(xform)
    polyline3.transform(xform)

    polylines = [polyline0, polyline1, polyline2, polyline3]

    element_pairs, joint_areas, joint_types = joints(polylines)
    print(element_pairs)
    print(joint_types)

    import sys

    if sys.version_info >= (3, 9):
        from compas_viewer import Viewer

        viewer = Viewer(show_grid=False)

        for polyline in polylines:
            viewer.scene.add(polyline, show_points=False)

        for polyline in joint_areas:
            viewer.scene.add(polyline, show_points=True, lineswidth=3)

        viewer.show()
