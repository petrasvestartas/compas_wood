from compas.geometry import Polyline
from compas.geometry import Point
from compas.geometry import Line
from compas.geometry import Plane
from compas.geometry import Frame
from compas.geometry import Transformation
from compas.geometry import intersection_segment_plane
from compas.geometry import Polygon
from compas.geometry import Scale
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import read_xml_polylines
from compas_wood.binding import wood_globals
from pathlib import Path
from compas import json_dump
from compas_wood.binding import closed_mesh_from_polylines


def cut_polygon_with_plane(polygon, plane, tolerance=1e-3):
    """ Cut a polygon with a plane.
    Originally implemented from NGon library: https://github.com/petrasvestartas/NGon/blob/8a52b39728732a0b20f9bc80879cd7de692b5d7a/Ngon/NGonsCore/NGonsCore/NGonGeometryUtil.cs#L12"""
  
    # Orient the polygon from the cutting frame to the XY frame
    
    # for p in polygon:
    #     print(p)
        
    frame = Frame.from_plane(plane) if isinstance(plane, Plane) else plane

    
    xform = Transformation.from_frame_to_frame(frame, Frame.worldXY())
    xformI = Transformation.from_frame_to_frame(Frame.worldXY(), frame)
   
    
    
    polygon_xformed = polygon.transformed(xform)
    
    points = polygon_xformed.points
    
    flag = True
    below = []
    
    # Check if the points are below the plane
    counter = 0
    for i in range(len(points)):
        if (points[i][2] < 0):
            
            below.append(True)
            flag = False
            counter = counter + 1
        else:
            below.append(False)
            
    if (flag):
        return [polygon]

                
    # For faces that coincide with plane
    polygons_culled = []

    if (counter != 0 and counter != len(points)):
        
        # Get split parameters
        polygon_points = []
        polygon_points_bool = []
        

        counter = 0
        for i in range(len(points)):
            polygon_points.append(points[i])
            polygon_points_bool.append(False)

            line = Line(points[i], points[(i + 1)%len(points)])
            result = intersection_segment_plane(line, Plane.worldXY())
     
            
            if result:
                intersection_point = Point(*result)
                polygon_points.append(intersection_point)
                polygon_points_bool.append(True)
                
                if counter == 0:
                    counter = len(polygon_points)-1

                
        def shift_left(lst, n):
            n = n % len(lst)  # To handle cases where n > len(lst)
            return lst[n:] + lst[:n]
        


        polygon_points = shift_left(polygon_points, counter)
        polygon_points_bool = shift_left(polygon_points_bool, counter)
        


        points_lists = []
        for i in range(len(polygon_points)):

            if polygon_points_bool[i]:
                points_lists.append([])
                
            if len(points_lists) > 1 and polygon_points_bool[i]:
                points_lists[-2].append(polygon_points[i])

            points_lists[-1].append(polygon_points[i])

            
        points_lists[-1].append(polygon_points[0])

        for i in range(len(points_lists)):
            cut_polyline_part = Polyline(points_lists[i])
            if (cut_polyline_part.length<tolerance):
                continue
            if (cut_polyline_part.point_at(0.5, True)[2] > tolerance):
                cut_polyline_part.transform(xformI)

                polygons_culled.append(Polygon(cut_polyline_part.points))
                
        print(polygons_culled)
        for p in polygons_culled[0]:
            print(*p)

    return polygons_culled

# Read polylines from an exrernal file, in this case compas_wood xml dataset.
xml_polylines = read_xml_polylines(
    foldername= str(Path.cwd() / "src/compas_wood/datasets/") + '/',
    filename='type_plates_name_side_to_side_edge_inplane_hilti_square_shell',
    # scale=0.25e1
)


xml_polylines.reverse()

# Create joints.
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = False
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = False
new_polyline_lists = []

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[1500, 1.0, 6],
    input_output_type=4)


# Remove duplicate polylines - bug in the code.

for polylines in polylines_lists:
    new_polylines = []
    for polyline in polylines:
        if polyline not in new_polylines:
            new_polylines.append(polyline)
    new_polyline_lists.append(new_polylines)
polylines_lists = new_polyline_lists

wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[1500, 1.0, 53],
    input_output_type=3)


# Remove duplicate polylines - bug in the code.

for polylines in polylines_lists:
    new_polylines = []
    for polyline in polylines:
        if polyline not in new_polylines:
            new_polylines.append(polyline)
            if len(new_polylines) == 2:
                new_polyline_lists.append(list(new_polylines))
                new_polylines.clear()
                
    # split new_polylines into two polylines

    
polylines_lists = new_polyline_lists


meshes = []


for plines in polylines_lists:
    mesh = closed_mesh_from_polylines(plines)
    meshes.append(mesh)


# Create meshes.
# meshes = mesh_boolean_difference_from_polylines(polylines_lists)

# # Serialize meshes.
# json_dump(meshes, 'data/json_dump/type_plates_name_side_to_side_edge_inplane_hilti_part6.json')

# Vizualize.

try:

    from compas_viewer import Viewer
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False, rendermode='ghosted')
    scale = 1e-2

    for i, polylines in enumerate(polylines_lists):
        # if(i == 1):
        for polyline in polylines:
            polyline.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(polyline, show_points=False, line_width=2, linecolor=(255, 0, 100))
            # break

    # print(meshes[0])
    for i, mesh in enumerate(meshes):
            mesh.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(mesh, show_points=False, hide_coplanaredges=True, lineswidth=2, linecolor=(0, 0, 0))
    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
    
    