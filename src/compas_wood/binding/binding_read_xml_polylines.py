from wood_nano import read_xml_polylines as wood_nano_read_xml_polylines
from wood_nano import double2
from compas.geometry import Polyline
from compas.geometry import Scale


def read_xml_polylines(
    foldername="/home/petras/brg/2_code/wood_nano/src/wood/cmake/src/wood/dataset/",
    filename="type_plates_name_cross_vda_hexshell_reciprocal",
    scale=1,
):
    """Read polylines from XML file.

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
    """
    polylines_coordinates = double2()
    wood_nano_read_xml_polylines(
        foldername,
        filename,
        polylines_coordinates,
    )

    polylines = []
    xform = Scale.from_factors([scale, scale, scale])

    for polyline in polylines_coordinates:
        points = []
        for i in range(0, len(polyline), 3):
            points.append([polyline[i], polyline[i + 1], polyline[i + 2]])
        polyline = Polyline(points)
        polyline.transform(xform)
        polylines.append(polyline)

    return polylines


if __name__ == "__main__":
    polylines = read_xml_polylines()

    import sys

    if sys.version_info >= (3, 9):
        from compas_viewer import Viewer

        viewer = Viewer()

        for polyline in polylines:
            viewer.scene.add(polyline, show_points=False)

        viewer.show()
