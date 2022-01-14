# from _typeshed import NoneType
from compas.geometry.primitives import polyline
import numpy as np
from compas.plugins import plugin
from compas.geometry import Point
from compas.geometry import Vector
from compas.geometry import Polyline
from compas.datastructures import Mesh
from compas.geometry import Box
from compas.geometry import Frame
from compas.datastructures import meshes_join_and_weld
import pybind11_joinery_solver
import time


@plugin(category="compas_wood_cpp", pluggable_name="compas_wood_cpp_test")
def test():
    """Test if C++ code works

    Parameters
    ----------


    Returns
    -------
    Prints
        Hello from CPP Wood

    """

    pybind11_joinery_solver.pybind11_test()


@plugin(
    category="compas_wood_cpp", pluggable_name="compas_wood_cpp_test_joinery_solver"
)
def get_connection_zones(
    polylines_vertices_XYZ,
    face_vectors_XYZ=None,
    face_joints_types_int=None,
    three_valance_element_indices_and_instruction=None,
    default_joint_parameters=None,
):
    """Compute joinery

    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines
    face_vectors_XYZ : vectors to orient a joint
    face_joints_types_int : joint id, existing in the code
    three_valance_element_indices_and_instruction : special case e.g. alignment of rectangles

    Returns
    -------
    List of Polylines
        with added joints

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    vertex_count_per_polyline = []
    flat_list_of_face_vectors_XYZ = []
    flat_list_of_face_joints_types_int = []
    flat_list_of_three_valance_element_indices_and_instruction = []
    flat_list_default_joint_paramters = [
        1000,
        0.5,
        1,
        1000,
        0.5,
        10,
        1000,
        0.5,
        20,
        1000,
        0.5,
        30,
        1000,
        0.5,
        40,
        1000,
        0.5,
        50,
    ]

    flag0 = face_vectors_XYZ != None
    if flag0:
        flag0 = len(face_vectors_XYZ) == len(polylines_vertices_XYZ) * 0.5
    print(flag0)

    flag1 = face_joints_types_int != None
    if flag1:
        flag1 = len(face_joints_types_int) == len(polylines_vertices_XYZ) * 0.5

    # print(flag1)
    # print(len(face_vectors_XYZ))
    # print(len(face_joints_types_int))
    # print(len(polylines_vertices_XYZ))

    flag2 = three_valance_element_indices_and_instruction != None

    if default_joint_parameters != None:
        if len(default_joint_parameters) == 6 * 3:
            flat_list_default_joint_paramters = default_joint_parameters

    for i in range(0, (int)(len(polylines_vertices_XYZ) * 0.5)):
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2])
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2 + 1])
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2]))
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2 + 1]))

        if flag0 and (
            len(face_vectors_XYZ[i]) == len(polylines_vertices_XYZ[i * 2]) + 1
        ):
            flat_list_of_face_vectors_XYZ.extend(face_vectors_XYZ[i])

        if flag1 and (
            len(face_joints_types_int[i]) == (len(polylines_vertices_XYZ[i * 2]) + 1)
        ):
            flat_list_of_face_joints_types_int.extend(face_joints_types_int[i])

    if flag2:
        for i in range(0, len(three_valance_element_indices_and_instruction)):
            flat_list_of_three_valance_element_indices_and_instruction.extend(
                three_valance_element_indices_and_instruction[i]
            )

    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)
    D = np.asarray(flat_list_of_face_vectors_XYZ, dtype=np.float64)
    J = np.asarray(flat_list_of_face_joints_types_int, dtype=np.int32)
    X = np.asarray(
        flat_list_of_three_valance_element_indices_and_instruction, dtype=np.int32
    )
    P = np.asarray(flat_list_default_joint_paramters, dtype=np.float64)

    search_type = 2
    show_plane_normals = True
    division_distance = 300
    shift = 0.6
    output_type = 4
    triangulate = 0

    # ==============================================================================
    # Test CPP module
    # ==============================================================================

    print("Input")
    print("flat_list_of_points " + str(V.size))
    print("vertex_count_per_polyline " + str(F.size))
    print("flat_list_of_face_vectors_XYZ " + str(D.size))
    print("flat_list_of_face_joints_types_int " + str(J.size))
    print("flat_list_of_three_valance_element_indices_and_instruction " + str(X.size))
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    (
        pointsets,
        pointsets_group_ids,
    ) = pybind11_joinery_solver.pybind11_get_connection_zones(
        V,
        F,
        D,
        J,
        X,
        P,
        search_type,
        division_distance,
        shift,
        output_type,
        triangulate,
    )
    print(
        "==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0)
    )
    # print(type(pointsets))
    # ==============================================================================
    # Process output
    # ==============================================================================

    polylines = []

    temp_collection = []
    last_id = pointsets_group_ids[0]

    for i in range(len(pointsets)):
        points = [Point(*point) for point in pointsets[i]]
        polyline = Polyline(points)

        """
        print(" ")
        for j in polyline:
            print( (str)(j[0]) +" "+ (str)(j[1])+" " + (str)(j[2]) )
        """

        if last_id != pointsets_group_ids[i]:
            polylines.append(temp_collection)
            temp_collection = []
            # print("Hi")

        temp_collection.append(polyline)
        last_id = pointsets_group_ids[i]
        # print(pointsets_group_ids[i])

    polylines.append(temp_collection)
    print(
        "============================================================================== CPP -"
    )
    print("Output")
    print("Number of Polylines ", len(polylines))

    return polylines


@plugin(
    category="compas_wood_cpp",
    pluggable_name="compas_wood_cpp_closed_mesh_from_polylines",
)
def closed_mesh_from_polylines(
    polylines_vertices_XYZ,
):
    """Create a mesh from a top and bottom outlines

    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines

    Returns
    -------
    Mesh
        from the pairs of polylines

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    vertex_count_per_polyline = []

    for i in range(0, (int)(len(polylines_vertices_XYZ) * 0.5)):
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2])
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2 + 1])
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2]))
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2 + 1]))

    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================
    """
    print("Input")
    print("flat_list_of_points " + str(V.size))
    print("vertex_count_per_polyline " + str(F.size))
    print("============================================================================== CPP +")
    """
    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    output_V, output_F = pybind11_joinery_solver.pybind11_closed_mesh_from_polylines(
        V, F
    )
    """
    print("F")
    output_F_ = []
    for i in output_F:
        if( abs(i[0]) < len(output_V) and abs(i[1]) < len(output_V) and abs(i[2]) < len(output_V)  ):
            output_F_.append(i)
        #print(i)
    
    
    print("V")
    for i in output_V:
        print(i)
    
    

  
   
    print("==================================== %s ms ====================================" %  round((time.time() - start_time)*1000.0) )
    # ==============================================================================
    # Process output
    # ==============================================================================
    print("Number of Mesh Vertices " , len(output_V))
    print("Number of Mesh Faces " , len(output_F))
      """

    mesh = Mesh.from_vertices_and_faces(output_V, output_F)
    # mesh = meshes_join_and_weld([mesh],0.001)
    # Mesh.cull_vertices(mesh)
    # Mesh.remove_unused_vertices(mesh)
    # Mesh.unify_cycles(mesh)
    print("Is Mesh Valid " + (str)(mesh.is_valid()))
    # Mesh.to_obj(mesh,"C:\IBOIS57\_Code\Software\Python\compas_wood\docs\examples\mesh.obj")

    print(
        "============================================================================== CPP -"
    )
    # print("Output")
    return mesh


@plugin(category="compas_wood_cpp", pluggable_name="compas_wood_cpp_rtee")
def rtree(
    polylines_vertices_XYZ,
):
    """Find neighbors of polyline pairs using the R-Tree search

    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines

    Returns
    -------
    Neighbours
        List of element indices
    AABB
        List of element axis aligned bounding-boxes corners
    OOBB
        List of element object oriented boxes corners

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    vertex_count_per_polyline = []

    for i in range(0, (int)(len(polylines_vertices_XYZ) * 0.5)):
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2])
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2 + 1])
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2]))
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2 + 1]))

    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================

    # print("Input")
    # print("flat_list_of_points " + str(V.size))
    # print("vertex_count_per_polyline " + str(F.size))
    # print("============================================================================== CPP +")

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    neighbours, AABB_corners, OOBB_corners = pybind11_joinery_solver.pybind11_rtree(
        V, F
    )

    # print("==================================== %s ms ====================================" %  round((time.time() - start_time)*1000.0) )
    # ==============================================================================
    # Process output
    # ==============================================================================
    # print("Number of Neighbours " , len(neighbours))
    # print("Number of AABB Corners " , len(AABB_corners))
    # print("Number of OOBB Corners " , len(OOBB_corners))

    """
    for i in range (len(pointsets)) :
        points = [Point(*point) for point in pointsets[i]]
        polyline = Polyline(points)
    
    
    for i in range (len(neighbours)) :
        print("New")
        for j in neighbours[i]:
            print(*j)
        #points = [Point(*point) for point in pointsets[i]]
        #polyline = Polyline(points)
    """

    boxes_AABB = []
    for i in range(len(AABB_corners)):

        points = []
        for j in range(0, len(AABB_corners[i]), 3):
            p = Point(
                AABB_corners[i][j], AABB_corners[i][j + 1], AABB_corners[i][j + 2]
            )
            points.append(p)

        point_center = (points[0] + points[6]) * 0.5

        xaxis = points[0] - points[4]
        yaxis = points[0] - points[3]
        zaxis = points[0] - points[1]
        frame = Frame(point_center, xaxis, yaxis)

        box = Box(frame, xaxis.length, yaxis.length, zaxis.length)
        V, F = box.to_vertices_and_faces()
        mesh_box = Mesh.from_vertices_and_faces(V, F)
        boxes_AABB.append(mesh_box)

    boxes_OOBB = []
    for i in range(len(OOBB_corners)):

        points = []
        for j in range(0, len(OOBB_corners[i]), 3):
            p = Point(
                OOBB_corners[i][j], OOBB_corners[i][j + 1], OOBB_corners[i][j + 2]
            )
            points.append(p)

        point_center = (points[0] + points[6]) * 0.5

        xaxis = points[0] - points[4]
        yaxis = points[0] - points[3]
        zaxis = points[0] - points[1]
        frame = Frame(point_center, xaxis, yaxis)

        box = Box(frame, xaxis.length, yaxis.length, zaxis.length)
        V, F = box.to_vertices_and_faces()
        mesh_box = Mesh.from_vertices_and_faces(V, F)
        boxes_OOBB.append(mesh_box)

    # print("============================================================================== CPP -")
    # print("Output")
    return neighbours, boxes_AABB, boxes_OOBB


@plugin(category="compas_wood_cpp", pluggable_name="compas_wood_cpp_joints")
def joints(polylines_vertices_XYZ, search_type):
    """Find joints using face_to_face or plane_to_face search

    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines
    search_type : int 0 - face_to_face, 1 - plane_to_face, 2 - all

    Returns
    -------
    element_pairs
        List of element indices
    joint_areas
        List of joint areas as polylines
    joint_types
        List of joint types as int

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    vertex_count_per_polyline = []

    for i in range(0, (int)(len(polylines_vertices_XYZ) * 0.5)):
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2])
        flat_list_of_points.extend(polylines_vertices_XYZ[i * 2 + 1])
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2]))
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i * 2 + 1]))

    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================

    # print("Input")
    # print("flat_list_of_points " + str(V.size))
    # print("vertex_count_per_polyline " + str(F.size))
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    element_pairs, joint_areas, joint_types = pybind11_joinery_solver.pybind11_joints(
        V, F, search_type
    )

    print(
        "==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0)
    )
    # ==============================================================================
    # Process output
    # ==============================================================================
    # print("Number of Neighbours " , len(neighbours))
    # print("Number of AABB Corners " , len(AABB_corners))
    # print("Number of OOBB Corners " , len(OOBB_corners))

    """
    for i in element_pairs :
        print(i[0])
        print(i[1])
    """

    element_pairs_list = []
    for id in element_pairs:
        element_pairs_list.append([id[0], id[1]])

    joint_areas_polylines = []
    for points_coord in joint_areas:
        points = [Point(*point) for point in points_coord]
        polyline = Polyline(points)
        joint_areas_polylines.append(polyline)

    joint_types_list = []
    for id in joint_types:
        joint_types_list.append(id[0])
    # print("Number of polylines " + str(len(joint_areas_polylines)))

    print(
        "============================================================================== CPP -"
    )
    # print("Output")
    return element_pairs_list, joint_areas_polylines, joint_types_list
