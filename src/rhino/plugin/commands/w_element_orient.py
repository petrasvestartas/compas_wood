#! python3
# venv: timber_connections

import Rhino
import Rhino.Input
import Rhino.DocObjects
from typing import *
from wood_rui import wood_rui_globals, generalized_input_method, Element, add_sub_layer
import System

def my_callback(input_dict, dataset_name):
    """
    Processes input elements and aligns them to calculated target planes.
    
    Args:
        input_dict (dict): Dictionary containing elements, rectangles, lines, and points.
        dataset_name (str): Name of the dataset (currently unused).
    """
    
    # Check if there are elements to process
    if not input_dict["elements"]:
        return
    
    elements: list[Element] = input_dict["elements"]
    is_center: bool = input_dict["center"]  # Determines whether to use center alignment
    target_planes = []  # List to store target planes
    
    # Generate target planes from rectangles
    for r in input_dict["rectangles"]:
        origin = r.CenterPoint() if is_center else r[0]  # Use center or first point
        x_axis = r[1] - r[0]  # X direction vector
        y_axis = r[r.Count - 2] - r[0]  # Y direction vector
        target_plane = Rhino.Geometry.Plane(origin, x_axis, y_axis)
        target_planes.append(target_plane)
    
    # Generate target planes from lines
    for l in input_dict["lines"]:
        if is_center:
            origin = (l.From + l.To) * 0.5  # Midpoint of line
        else:
            origin = l.From  # Start point of line
        target_plane = Rhino.Geometry.Plane(origin, l.Direction)
        target_planes.append(target_plane)
    
    # Generate target planes from points
    for p in input_dict["points"]:
        target_plane = Rhino.Geometry.Plane(p, Rhino.Geometry.Vector3d.ZAxis)  # Align with Z axis
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

    dataset_name = "beam"
    wood_rui_globals.init_data(dataset_name)

    input_dict = {
        "elements" : ([], List[Element]),
        "rectangles" : ([], List[Rhino.Geometry.Polyline]),
        "lines" : ([], List[Rhino.Geometry.Line]),
        "points" : ([], List[Rhino.Geometry.Point3d]),
        "center" : (False, bool)
    }

   # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback, False, True)
