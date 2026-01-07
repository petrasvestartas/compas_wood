from compas.geometry import Polyline
from compas.geometry import Polygon
from compas.geometry import Box
from compas_wood.binding import get_connection_zones
from compas_wood.binding import mesh_boolean_difference_from_polylines
from compas_wood.binding import wood_globals
from compas.datastructures import Mesh
try:
    from compas_cgal.booleans import split_mesh_mesh
except ImportError:
    print("Error: The package 'compas_cgal.booleans' or the function 'split_mesh_mesh' does not exist.")
    exit(1)

# Create two polylines.
polyline00 = Polyline([[0, 150, 0], [-300, 150, 0], [-300, -150, 0], [0, -150, 0], [0, 150, 0]])
polyline01 = Polyline([[0, 150, 120], [-300, 150, 120], [-300, -150, 120], [0, -150, 120], [0, 150, 120]])
polyline10 = Polyline([[0,150,0], [0, -150, 0], [300, -150, 0], [300, 150, 0], [0, 150, 0]])
polyline11 = Polyline([[0,150,120], [0, -150, 120], [300, -150, 120], [300, 150, 120], [0, 150, 120]])
polylines = [polyline00, polyline01, polyline10, polyline11]

# Create joints.
wood_globals.face_to_face_side_to_side_joints_rotated_joint_as_average = True
wood_globals.face_to_face_side_to_side_joints_all_treated_as_rotated = True

polylines_lists, output_types = get_connection_zones(
    input_polyline_pairs=polylines,
    input_joint_parameters_and_types=[500, 1.0, 54],
    input_output_type=5)

# Create meshes.
meshes = mesh_boolean_difference_from_polylines(polylines_lists)

# Split mesh
# plane_mesh = Mesh.from_points([
#     [-10,0, -10],
#     [-10,0, 10],
#     [10,0, 10],
#     [10,0, -10],
#     ])


polygon = Polygon(   [ 
    [-4,0, -4],
    [-4,0, 4],
    [4,0, 4],
    [4,0, -4],
    ])
box_mesh = Mesh.from_shape(Box(2))


def split_mesh(mesh, splitter_mesh):
    # ==============================================================================
    # Make a box and a sphere
    # ==============================================================================

    A = mesh.to_vertices_and_faces(triangulated=True)
    print(mesh.is_valid)
    B = splitter_mesh.to_vertices_and_faces(triangulated=True)
    print(splitter_mesh.is_valid)

    # ==============================================================================
    # Compute the mesh split
    # ==============================================================================

    V, F = split_mesh_mesh(A, B)

    mesh = Mesh.from_vertices_and_faces(V, F)
    

    # ==============================================================================
    # Seperate disjoint faces and visualize
    # ==============================================================================
    from compas_viewer import Viewer
    from compas.colors import Color
    viewer = Viewer()

    viewer.renderer.camera.target = [0, 0, 0]
    viewer.renderer.camera.position = [4, -6, 3]

    for color, vertices in zip([Color.blue(), Color.pink()], mesh.connected_vertices()):
        faces = []
        for indices in F:
            if all(index in vertices for index in indices):
                faces.append(indices)
        mesh = Mesh.from_vertices_and_faces(V, faces)
        mesh.remove_unused_vertices()

        viewer.scene.add(mesh, facecolor=color, show_points=False)

    viewer.show()
    return mesh

split_meshes = split_mesh(box_mesh, Mesh.from_polygons([polygon]))

print(split_meshes)

# Vizualize.

try:
    
    from compas.geometry import Scale

    viewer = Viewer(show_grid=False, show_gridz=False, rendermode='ghosted')
    viewer.config.renderer.show_grid=True
    scale = 1e-2

    for polylines in polylines_lists:
        for polyline in polylines:
            polyline.transform(Scale.from_factors([scale, scale, scale]))
            viewer.scene.add(polyline, show_points=False)
            
    for mesh in meshes:
        mesh.transform(Scale.from_factors([scale, scale, scale]))
        viewer.scene.add(mesh, show_points=False, hide_coplanaredges=True, lineswidth=2, linecolor=(0, 0, 0))
        
    viewer.scene.add(box_mesh)
    viewer.scene.add(Mesh.from_polygons([polygon]))
    for mesh in split_meshes:
        viewer.scene.add(mesh)
    


    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
