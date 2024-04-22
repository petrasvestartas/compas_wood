from compas_wood.binding import read_xml_polylines_and_properties
from compas.geometry import Line


(
    polylines,
    vectors_lists,
    input_joints_types,
    input_three_valence_element_indices_and_instruction,
    input_adjacency,
) = read_xml_polylines_and_properties()



try:
    
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

    viewer.show()

except ImportError:
    print("compas_viewer is not installed.")
