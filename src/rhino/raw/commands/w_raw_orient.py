#! python3
# venv: timber_connections

import Rhino
from wood_rui import process_input
from wood_rui import Element
from wood_rui import add_sub_layer
from System.Drawing import Color


def callback(selection: dict[str, any], name: str):
    if not selection["elements"]:
        return

    # Check if there are elements to process
    if not selection["elements"]:
        return

    elements: list[Element] = selection["elements"]
    is_center: bool = selection["center"]  # Determines whether to use center alignment
    target_planes = []  # List to store target planes

    # Generate target planes from rectangles
    for r in selection["rectangles"]:
        origin = r.CenterPoint() if is_center else r[0]  # Use center or first point
        x_axis = r[1] - r[0]  # X direction vector
        y_axis = r[r.Count - 2] - r[0]  # Y direction vector
        target_plane = Rhino.Geometry.Plane(origin, x_axis, y_axis)
        target_planes.append(target_plane)

    # Generate target planes from lines
    for l in selection["lines"]:
        if is_center:
            origin = (l.From + l.To) * 0.5  # Midpoint of line
        else:
            origin = l.From  # Start point of line
        target_plane = Rhino.Geometry.Plane(origin, l.Direction)
        target_planes.append(target_plane)

    # Generate target planes from points
    for p in selection["points"]:
        target_plane = Rhino.Geometry.Plane(
            p, Rhino.Geometry.Vector3d.ZAxis
        )  # Align with Z axis
        target_planes.append(target_plane)

    # Align elements to target planes
    for idx, target_plane in enumerate(target_planes):
        element = elements[idx % len(elements)]  # Ensure elements wrap around if needed
        plane: Rhino.Geometry.Plane = element.plane  # Element's original plane

        # If center alignment is enabled and the element has axes, compute perpendicular frame
        if is_center and len(element.axes) > 0:
            curve = element.axes[0].ToNurbsCurve()
            result, plane = curve.PerpendicularFrameAt(curve.Domain.Mid)

        # Compute transformation and apply it to the element
        xform = Rhino.Geometry.Transform.PlaneToPlane(plane, target_plane)
        element.transformed(xform)


if __name__ == "__main__":
    selection_types: dict[str, tuple] = {
        "elements": ([], list[Element]),
        "rectangles": ([], list[Rhino.Geometry.Polyline]),
        "lines": ([], list[Rhino.Geometry.Line]),
        "points": ([], list[Rhino.Geometry.Point3d]),
        "center": (False, bool),
    }

    process_input(selection_types, callback)
