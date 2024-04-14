import wood_nano
from wood_nano.conversions_python import from_int2
from compas_wood.conversions_compas import to_point2, from_point2


def rtree(polylines):
    """Given a list of polylines-pairs find neighbors between them via aabb and obb collisions.
    Polyline pairs means the top and bottom polylines of plates.

    Parameters
    ----------
    list_of_polylines : list[point2]
        list of polylines that represents the timber elements

    Returnsto_point
        point2 obb
    """

    w_polylines = to_point2(polylines)
    w_neighbors = wood_nano.int2()
    w_aabb = wood_nano.point2()
    w_obb = wood_nano.point2()

    wood_nano.rtree(w_polylines, w_neighbors, w_aabb, w_obb)

    return from_int2(w_neighbors), from_point2(w_aabb, "Box"), from_point2(w_obb, "Box")


if __name__ == "__main__":

    from compas.geometry import Polyline, Point

    # Example
    polyline0 = Polyline([Point(0, 0, 0), Point(1, 0, 0), Point(1, 1, 0), Point(0, 1, 0), Point(0, 0, 0)])
    polyline1 = Polyline([Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1), Point(0, 0, 1)])

    polyline2 = Polyline([Point(0, 0, 1), Point(1, 0, 1), Point(1, 1, 1), Point(0, 1, 1), Point(0, 0, 1)])
    polyline3 = Polyline([Point(0, 0, 2), Point(1, 0, 2), Point(1, 1, 2), Point(0, 1, 2), Point(0, 0, 2)])
    polylines = [polyline0, polyline1, polyline2, polyline3]

    neighbors, aabb, obb = rtree(polylines)

    print(neighbors)
    print(aabb)
    print(obb)

    import sys

    if sys.version_info >= (3, 9):
        from compas_viewer import Viewer

        viewer = Viewer()

        for polyline in polylines:
            viewer.scene.add(polyline, show_points=False)

        for box in aabb:
            viewer.scene.add(box, show_points=False)

        for box in obb:
            viewer.scene.add(box, show_points=False)

        viewer.show()
