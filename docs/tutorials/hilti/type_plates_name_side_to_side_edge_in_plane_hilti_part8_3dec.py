from compas.geometry import Polyline
from compas.geometry import Point
from compas.geometry import Line
from compas.geometry import Plane
from compas.geometry import Frame
from compas.geometry import Box
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
from compas_snippets.viewer_iterative import run, add_geometry


def cut_polygon_with_plane(_polygon, plane, tolerance=1e-3):
    """ Cut a polygon with a plane.
    Originally implemented from NGon library: https://github.com/petrasvestartas/NGon/blob/8a52b39728732a0b20f9bc80879cd7de692b5d7a/Ngon/NGonsCore/NGonsCore/NGonGeometryUtil.cs#L12"""
  
    # Orient the polygon from the cutting frame to the XY frame
    
    # for p in polygon:
    #     print(p)
    
    polygon = _polygon.copy()
        
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
                
        # print(polygons_culled)
        # for p in polygons_culled[0]:
        #     print(*p)

    points = polygons_culled[0].points
    points.append(points[0])
    return Polyline(points)

######################################################################################
# Read polylines from an exrernal file, in this case compas_wood xml dataset.
######################################################################################
xml_polylines = read_xml_polylines(
    foldername= str(Path.cwd() / "src/compas_wood/datasets/") + '/',
    filename='type_plates_name_side_to_side_edge_inplane_hilti_square_shell',
    # scale=0.25e1
)


xml_polylines.reverse()

######################################################################################
# Create joints.
######################################################################################
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = False
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = False
new_polyline_lists = []

polylines_lists, output_types, polylines_lists_cleaned = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[1500, 1.0, 6],
    input_output_type=4,
    remove_duplicate_polylines = True)


wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists_connectors, output_types_connectors, polylines_lists_connector_cleaned = get_connection_zones(
    input_polyline_pairs=xml_polylines,
    input_joint_parameters_and_types=[1500, 1.0, 53],
    input_output_type=3,
    remove_duplicate_polylines = True)

######################################################################################
# Cut Polylines with planes.
######################################################################################

geometry_serialization = {}
geometry = []

def cut_polygons_with_planes(polylines_lists, indices, planes_lists, geometry):
    cut_polygons = []
    for index in range(len(indices)):
        polygons = [polylines_lists[indices[index]][0],polylines_lists[indices[index]][1]]
        planes = planes_lists[index]

        for polygon in polygons:
            cut_polygon = polygon.copy()
            for plane in planes:
                cut_polygon = cut_polygon_with_plane(cut_polygon, plane)
            if cut_polygon:
                if len(cut_polygon) > 2:
                    geometry.append(cut_polygon)
    return cut_polygons



main_planes = [
    
    
    Plane(Point(-3510+990+1010+990+1040+990+1010+990,4000,10), [1, 0, 0]), # 0
    Plane(Point(-3510+990+1010+990+1040+990+1010,4000,10), [1, 0, 0]), # 1
    Plane(Point(-3510+990+1010+990+1040+990,4000,10), [1, 0, 0]), # 2
    Plane(Point(-3510+990+1010+990+1040,4000,10), [1, 0, 0]), # 3
    Plane(Point(-3510+990+1010+990,4000,10), [1, 0, 0]), # 4
    Plane(Point(-3510+990+1010,4000,10), [1, 0, 0]), # 5
    Plane(Point(-3510+990,4000,10), [1, 0, 0]), # 6
    Plane(Point(-3510,4000,10), [1, 0, 0]), # 7

    Plane(Point(4000, -3510+990+1010+990+1040+990+1010+990,10), [0, 1, 0]), # 8
    Plane(Point(4000, -3510+990+1010+990+1040+990+1010,10),[0, 1, 0]), # 9
    Plane(Point(4000, -3510+990+1010+990+1040+990,10),[0, 1, 0]), # 10
    Plane(Point(4000, -3510+990+1010+990+1040,10), [0, 1, 0]), # 11
    Plane(Point(4000, -3510+990+1010+990,10), [0, 1, 0]), # 12
    Plane(Point(4000, -3510+990+1010,10),[0, 1, 0]), # 13
    Plane(Point(4000, -3510+990,10), [0, 1, 0]), # 14
    Plane(Point(4000, -3510,10), [0, 1, 0]), # 15
    

]

base_instruction = [
    #    0   1   2   3   4   5   6   7
    [0,  1,  0,  0,  0,  0,  0,  0,  0,
         1,  0,  0,  0,  0,  0,  0,  0],
    [0, -1,  1,  0,  0,  0,  0,  0,  0,
         1,  0,  0,  0,  0,  0,  0,  0],
    [0,  0, -1,  0,  0,  0,  0,  0,  0,
         1,  0,  0,  0,  0,  0,  0,  0],

    [0,  1,  0,  0,  0,  0,  0,  0,  0,
         -1,  1,  0,  0,  0,  0,  0,  0],
    [0,  -1,  1,  0,  0,  0,  0,  0,  0,
         -1,  1,  0,  0,  0,  0,  0,  0],
    [0,  0,  -1,  0,  0,  0,  0,  0,  0,
         -1,  1,  0,  0,  0,  0,  0,  0],
    
    [0,   1,  0,  0,  0,  0,  0,  0,  0,
         0,  -1,  0,  0,  0,  0,  0,  0],
    [0,  -1,  1,  0,  0,  0,  0,  0,  0,
         0,  -1,  0,  0,  0,  0,  0,  0],
    [0,  0,  -1,  0,  0,  0,  0,  0,  0,
         0,  -1,  0,  0,  0,  0,  0,  0],
]

instructions = []

counter = 0
geometry_serialization["groups"] = []
for i in range(4):
    for j in range(4):
        
        id = i+(4*j)
        
        if i==0 and j==0:
            instructions.extend(base_instruction)
        else:
            for instruction in base_instruction:
                temp = list(instruction)
                temp[0] = id
                for k in range(i):
                    temp.insert(1, 0)
                    temp.insert(1, 0)
                    temp.pop(9)
                    temp.pop(9)
                for k in range(j):
                    temp.insert(1+8, 0)
                    temp.insert(1+8, 0)
                    temp.pop(9+8)
                    temp.pop(9+8)
                instructions.append(temp)
        group_id = id*9
        geometry_serialization["groups"].append([group_id, group_id+1, group_id+2, group_id+3, group_id+4, group_id+5, group_id+6, group_id+7, group_id+8])

geometry_serialization["corners"] = [0, 33, 110, 143]     
            

indices = []
planes = []


for instruction in instructions:
    
    
    mesh_id = []

    for i in range(len(instruction)):
        if i == 0:
            indices.append(instruction[0])
            planes.append([])
        else:
            id = i-1
            if instruction[i] == 1:
                planes[-1].append(main_planes[id])
            elif instruction[i] == -1:
                flipped_plane = Plane(main_planes[id].point, [-x for x in main_planes[id].normal])
                planes[-1].append(flipped_plane)
       

boxes = []
for p in main_planes:
    boxes.append(Box(200, 2, 2, Frame.from_plane(p)))
# geometry.extend(boxes)
cut_polylines = []
cut_polygons_with_planes(polylines_lists_cleaned, indices, planes, cut_polylines)
geometry.extend(cut_polylines)
geometry.append(polylines_lists_cleaned) # Add the original polylines to the geometry



######################################################################################
# Mesh Polylines.
######################################################################################


meshes_panels = []
counter = 0
for i in range(0, len(cut_polylines), 2):
    mesh = closed_mesh_from_polylines([cut_polylines[i], cut_polylines[i+1]])
    mesh.name = "mesh_panel_" + str(counter)
    counter = counter + 1
    meshes_panels.append(mesh)

geometry.extend(meshes_panels)
# meshes = mesh_boolean_difference_from_polylines(polylines_lists)
geometry_serialization["meshes_panels"] = meshes_panels
######################################################################################
# Mesh Connectors.
######################################################################################

meshes_connectors = []
for polylines in polylines_lists_connectors:
    for i in range(0, len(polylines), 2):
        pair = [polylines[i], polylines[i+1]]
        mesh = closed_mesh_from_polylines(pair)
        meshes_connectors.append(mesh)
geometry.extend(meshes_connectors)
geometry_serialization["meshes_connectors"] = meshes_connectors
######################################################################################
# Serialize
######################################################################################


json_dump(geometry_serialization, 'data/json_dump/timber_shell_connectors.json')

######################################################################################
# Vizualize
######################################################################################

add_geometry(geometry)
# run()


    