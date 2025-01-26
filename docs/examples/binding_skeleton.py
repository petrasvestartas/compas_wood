from compas_wood.binding import beam_skeleton
from compas.datastructures import Mesh
from pathlib import Path
from compas_viewer import Viewer
from compas.geometry import Frame, Plane
from compas.geometry import Polygon
from compas.geometry import Transformation

file_path = Path("data/beam_skeleton_sample_0.ply")
mesh = Mesh.from_ply(file_path)
polyline, distances = beam_skeleton(mesh, divisions=7, number_of_neighbours=10, extend_ends=True)


# Visualize the result
s = 0.01
viewer = Viewer()
viewer.scene.add(mesh.scaled(s), show_faces=False, show_lines=True, opacity=0.5)
viewer.scene.add(polyline.scaled(s), linewidth=5, linecolor=(255, 0, 0))
for i in range(0, len(polyline)):
    a = i
    b = i+1 if i+1 < len(polyline) else i-1
    direction = polyline[b] - polyline[a]
    polygon = Polygon.from_sides_and_radius_xy(25, distances[i])
    polygon.transform(Transformation.from_frame(Frame.from_plane(Plane(polyline[i], direction))))
    viewer.scene.add(polygon.scaled(s), facecolor=(255, 0, 0), linewidth=5, linecolor=(255, 0, 0))
viewer.show()