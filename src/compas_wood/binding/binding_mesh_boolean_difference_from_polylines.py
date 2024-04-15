from wood_nano import mesh_boolean_difference_from_polylines as wood_nano_mesh_boolean_difference_from_polylines
from compas_wood.conversions_compas import to_point2
from compas_wood.conversions_compas import to_point3
from wood_nano import point2
from wood_nano import vector2
from wood_nano import int3
from compas.datastructures import Mesh


def mesh_boolean_difference_from_polylines(
    polylines_lists,
):
    """The polylines lists first items represnts elements outlines and the second items represents joinery outlines.

    Parameters
    ----------
    polylines_lists : list of list of compas.geometry.Polyline
        The polylines lists first items represnts elements outlines and the second items represents joinery outlines.

    Returns
    -------
    list of compas.datastructures.Mesh
        The meshes created by the boolean difference of the elements and joineries.

    """

    object_outlines = [polyline for polylines in polylines_lists for polyline in polylines[:2]]
    joinery_outlines = [polylines[2:] for polylines in polylines_lists]

    wood_nano_out_vertices = point2()
    wood_nano_out_normals = vector2()
    wood_nano_out_faces = int3()
    wood_nano_mesh_boolean_difference_from_polylines(
        to_point2(object_outlines),
        to_point3(joinery_outlines),
        wood_nano_out_vertices,
        wood_nano_out_normals,
        wood_nano_out_faces,
    )

    meshes = []
    for vertices, faces in zip(wood_nano_out_vertices, wood_nano_out_faces):
        mesh = Mesh.from_vertices_and_faces(vertices, faces)
        meshes.append(mesh)
    return meshes
