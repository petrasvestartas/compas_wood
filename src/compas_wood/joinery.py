# from _typeshed import NoneType
import numpy as np
from compas.plugins import plugin
from compas.geometry import Point
from compas.geometry import Polyline
from compas.datastructures import Mesh
from compas.geometry import Box
from compas.geometry import Frame
import pybind11_joinery_solver
import time
import os


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
    adjacency=None,
    default_joint_parameters=None,
    search_type=2,
    output_type=4,
    triangulate=0,
    scale=None,
):
    """Compute joinery

    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines
    face_vectors_XYZ : vectors to orient a joint
    face_joints_types_int : joint id, existing in the code
    three_valance_element_indices_and_instruction : special case e.g. alignment of rectangles
    adjacency : a list of plate id integer pairs and face pairs e.g. v0, v1, f0, f1
    search_type : 0 - face-to-face, 1 - plane-to-face (cross), 2 - all
    output_type : 0 - top outlines and one joint_area, 1 - top outlines and two joint lines,
    2 - top outlines and joint volumes first and third, 3 - top outlines and joint polylines,     4 - get_joints_geometry_as_closed_polylines_performing_intersection
    triangulate : 0 - not output as mesh, 1 - output as mesh, currently not supported in python
    scale : xyz once or six times per joint type

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
    flat_list_adjacency = []
    flat_list_default_joint_paramters = [
        1000,
        0.5,
        1,
        1000,
        0.5,
        10,
        1000,
        0.6,
        20,
        1000,
        0.6,
        30,
        1000,
        0.5,
        40,
        1000,
        0.5,
        50,
    ]

    flat_list_scale = [
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
    ]

    flag0 = face_vectors_XYZ is not None
    if flag0:
        flag0 = len(face_vectors_XYZ) == len(polylines_vertices_XYZ) * 0.5
    print(flag0)

    flag1 = face_joints_types_int is not None
    if flag1:
        flag1 = len(face_joints_types_int) == len(polylines_vertices_XYZ) * 0.5

    # print(flag1)
    # print(len(face_vectors_XYZ))
    # print(len(face_joints_types_int))
    # print(len(polylines_vertices_XYZ))

    if default_joint_parameters is not None:
        print("COMPAS_WOOD Number of params: " + (str)((len)(default_joint_parameters)))
        if len(default_joint_parameters) == 6 * 3:
            flat_list_default_joint_paramters = default_joint_parameters
            print("COMPAS_WOOD default_parameters_changed")
    else:
        print("COMPAS_WOOD Default Parameters")

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

    if three_valance_element_indices_and_instruction is not None:
        for i in range(0, len(three_valance_element_indices_and_instruction)):
            flat_list_of_three_valance_element_indices_and_instruction.extend(
                three_valance_element_indices_and_instruction[i]
            )

    if adjacency is not None:
        for i in range(0, len(adjacency)):
            flat_list_adjacency.extend(adjacency[i])

    if scale is not None:
        if len(scale) > 2:
            flat_list_scale = scale

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
    A = np.asarray(flat_list_adjacency, dtype=np.int32)
    P = np.asarray(flat_list_default_joint_paramters, dtype=np.float64)
    S = np.asarray(flat_list_scale, dtype=np.float64)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================

    print("COMPAS_WOOD Input")
    print("COMPAS_WOOD flat_list_of_points " + str(V.size))
    print("COMPAS_WOOD vertex_count_per_polyline " + str(F.size))
    print("COMPAS_WOOD flat_list_of_face_vectors_XYZ " + str(D.size))
    print("COMPAS_WOOD flat_list_of_face_joints_types_int " + str(J.size))
    print(
        "COMPAS_WOOD flat_list_of_three_valance_element_indices_and_instruction "
        + str(X.size)
    )
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================
    folder = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "joinery_library.xml"
    )
    # print(folder)
    start_time = time.time()
    (
        pointsets,
        pointsets_group_ids,
    ) = pybind11_joinery_solver.pybind11_get_connection_zones(
        folder,
        V,
        F,
        D,
        J,
        X,
        A,
        P,
        S,
        search_type,
        output_type,
        triangulate,
    )
    print(
        "\nCOMPAS_WOOD ==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0, 2)
    )
    # print(type(pointsets))
    # ==============================================================================
    # Process output
    # ==============================================================================
    polylines = []

    if len(pointsets) == 0 or len(pointsets_group_ids) == 0:
        return polylines
    # print(len(pointsets))
    # print(len(pointsets_group_ids))

    temp_collection = []
    last_id = pointsets_group_ids[0]

    for i in range(len(pointsets)):
        points = [Point(*point) for point in pointsets[i]]
        polyline = Polyline(points)

        if last_id != pointsets_group_ids[i]:
            polylines.append(temp_collection)
            temp_collection = []
            # print("Hi")

        temp_collection.append(polyline)
        last_id = pointsets_group_ids[i]
        # print(pointsets_group_ids[i])

    polylines.append(temp_collection)
    print(
        "\nCOMPAS_WOOD ============================================================================== CPP -"
    )
    print("COMPAS_WOOD Output")
    print("COMPAS_WOOD Number of Polylines ", len(polylines))

    return polylines


@plugin(
    category="compas_wood_cpp",
    pluggable_name="compas_wood_cpp_closed_mesh_from_polylines",
)
def closed_mesh_from_polylines(
    polylines_vertices_XYZ,
):
    """Create a mesh from a top and bottom outlines, may return empty or invalid mesh when no mesh is create or input is bad

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
    """
    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================
    # print("============================================================================== CPP +"   )
    # start_time = time.time()
    output_V, output_F = pybind11_joinery_solver.pybind11_closed_mesh_from_polylines(
        V, F
    )
    # print("==================================== %s ms ====================================" % round((time.time() - start_time) * 1000.0) )
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
    # ==============================================================================
    # Process output
    # ==============================================================================
    print("Number of Mesh Vertices " , len(output_V))
    print("Number of Mesh Faces " , len(output_F))
      """

    mesh = Mesh.from_vertices_and_faces(output_V, output_F)
    # print("Is Mesh Valid " + (str)(mesh.is_valid()))

    # Mesh.to_obj(mesh,"C:\IBOIS57\_Code\Software\Python\compas_wood\docs\examples\mesh.obj")

    # print("============================================================================== CPP -")
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
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    neighbours, AABB_corners, OOBB_corners = pybind11_joinery_solver.pybind11_rtree(
        V, F
    )

    print(
        "==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0)
    )
    print(
        "============================================================================== CPP -"
    )
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
    JOINTS_TYPES
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
    element_pairs, joint_areas, JOINTS_TYPES = pybind11_joinery_solver.pybind11_joints(
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

    JOINTS_TYPES_list = []
    for id in JOINTS_TYPES:
        JOINTS_TYPES_list.append(id[0])
    # print("Number of polylines " + str(len(joint_areas_polylines)))

    print(
        "============================================================================== CPP -"
    )
    # print("Output")
    return element_pairs_list, joint_areas_polylines, JOINTS_TYPES_list


@plugin(
    category="compas_wood_cpp",
    pluggable_name="compas_wood_cpp_intersecting_sequences_of_dD_iso_oriented_boxes",
)
def closest_polylines(polylines, segment_radii, min_distance):
    """Find closest polylines within given distance

    Parameters
    ----------
    polylines : list of polylines
    segment_radii : radius per each polyline segment, used to construct boxes around segments
    min_distance : distance until which search is valid

    Returns
    -------
    neighbours_list
        List of element indices of polyline0_id_segment0_id_polyline1_id_segment1_id
    pair_points_list
        a list of 2 points

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    flat_list_radii = []
    vertex_count_per_polyline = []

    for i in range(len(polylines)):
        flat_list_of_points.extend(polylines[i])
        vertex_count_per_polyline.append(len(polylines[i]))
        flat_list_radii.extend(segment_radii[i])

    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    S_R = np.asarray(flat_list_radii, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    start_time = time.time()
    (
        neighbours,
        point_pairs,
    ) = pybind11_joinery_solver.pybind11_intersecting_sequences_of_dD_iso_oriented_boxes(
        V, S_R, F, min_distance
    )
    # print(point_pairs)
    print(
        "==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0)
    )

    print(
        "============================================================================== CPP -"
    )

    # ==============================================================================
    # Process output
    # ==============================================================================
    neighbours_list = []
    for i in neighbours:
        neighbours_list.extend([i[0], i[1], i[2], i[3]])

    pair_points_list = []
    for i in point_pairs:
        p0 = Point(i[0], i[1], i[2])
        p1 = Point(i[3], i[4], i[5])
        pair_points_list.append(Polyline([p0, p1]))

    # print("Output")
    return neighbours_list, pair_points_list


@plugin(
    category="compas_wood_cpp",
    pluggable_name="compas_wood_cpp_beam_volumes",
)
def beam_volumes(
    polylines,
    segment_radii,
    segment_normals,
    allowed_types,
    min_distance,
    volume_length,
    cross_or_side_to_end,
    flip_male,
    joint_parameters,
    compute_joints=True,
    division_distance=300,
    shift=0.6,
    output_type=3,
):
    """Draw box volumes around polylines segments

    Parameters
    ----------
    polylines : list of polylines
    segment_radii : radius per each polyline segment, used to construct boxes around segments
    segment_vectors : radius per each polyline segment, used to construct boxes around segments
    allowed_types : -1 - all possible types, 0 - end-to-end, 1 - cross or side-to-end, this option is needed because multiple joint can be made in one intersection
    min_distance : double distance until which search is valid
    volume_length : double length of beam volumes
    cross_or_side_to_end : double type0_type1_parameter_00_05
    flip_male : property for side-to-end volumes, 0 - no shift, -1 shift to right, 1 shift to left
    joint_parameters_list : division distance, shift, type - 1000, 0.5, 1, 1000, 0.5, 10, 1000, 0.5, 20, 1000, 0.5, 30, 1000, 0.5, 40, 1000, 0.5, 50,
    compute_joints : true or false
    shift : parameter of a joint
    output type: 0 - 1 - 2 -3

    Returns
    -------
    neighbours_list
        List of element indices of polyline0_id_segment0_id_polyline1_id_segment1_id
    pair_points_list
        a list of 2 points
    pair_volumes_list
        a list of 4 polyline
    joint_areas_list
        a list of closed polylines within local connection detection
    JOINTS_TYPES_list
        a list of integers of joint area types
    joint_geometry
        oriented joint geometry if joints are computed
    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    flat_list_radii = []
    flat_list_normals = []
    flat_list_allowed_types = []

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

    if joint_parameters is not None:
        if len(joint_parameters) == len(flat_list_default_joint_paramters):
            flat_list_default_joint_paramters = joint_parameters

    if allowed_types is not None:
        flat_list_allowed_types.extend(allowed_types)

    vertex_count_per_polyline = []

    for i in range(len(polylines)):
        flat_list_of_points.extend(polylines[i])
        vertex_count_per_polyline.append(len(polylines[i]))
        flat_list_radii.extend(segment_radii[i])
        if segment_normals is not None:
            flat_list_normals.extend(segment_normals[i])

    # ==============================================================================
    # Convert to Numpy, points are lists, therefore, some of these will be nested
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    S_R = np.asarray(flat_list_radii, dtype=np.float64)
    S_N = np.asarray(flat_list_normals, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)
    F_T = np.asarray(flat_list_allowed_types, dtype=np.int32)
    P = np.asarray(flat_list_default_joint_paramters, dtype=np.float64)
    # print(V)
    # print(S_N)

    # ==============================================================================
    # Test CPP module
    # ==============================================================================
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================
    folder = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "joinery_library.xml"
    )
    # folder = ""
    start_time = time.time()
    (
        neighbours,
        point_pairs,
        volume_pairs,
        joint_areas,
        JOINTS_TYPES,
        joint_geometry,
    ) = pybind11_joinery_solver.pybind11_beam_volumes(
        folder,
        V,
        S_R,
        S_N,
        F,
        F_T,
        min_distance,
        volume_length,
        cross_or_side_to_end,
        flip_male,
        P,
        compute_joints,
        division_distance,
        shift,
        output_type,
    )
    # print(point_pairs)
    print(
        "==================================== %s ms ===================================="
        % round((time.time() - start_time) * 1000.0)
    )

    print(
        "============================================================================== CPP -"
    )

    # ==============================================================================
    # Process output
    # ==============================================================================
    neighbours_list = []
    for i in neighbours:
        neighbours_list.extend([i[0], i[1], i[2], i[3]])

    pair_points_list = []
    for i in point_pairs:
        p0 = Point(i[0], i[1], i[2])
        p1 = Point(i[3], i[4], i[5])
        pair_points_list.append(Polyline([p0, p1]))

    pair_volumes_list = []
    for points_coord in volume_pairs:
        points = [Point(*point) for point in points_coord]
        pline = Polyline(points)
        pair_volumes_list.append(pline)
        # pair_volumes_list.append(Polyline([pline[0], pline[1]]))

    joint_areas_list = []
    for points_coord in joint_areas:
        points = [Point(*point) for point in points_coord]
        pline = Polyline(points)
        joint_areas_list.append(pline)

    JOINTS_TYPES_list = []
    for points_coord in JOINTS_TYPES:
        JOINTS_TYPES_list.append(points_coord[0])

    joint_geometry_list = []
    for points_coord in joint_geometry:
        points = [Point(*point) for point in points_coord]
        pline = Polyline(points)
        joint_geometry_list.append(pline)

    print(os.getcwd())
    # print("Output")
    return (
        neighbours_list,
        pair_points_list,
        pair_volumes_list,
        joint_areas_list,
        JOINTS_TYPES_list,
        joint_geometry_list,
    )


@plugin(
    category="compas_wood_cpp",
    pluggable_name="compas_wood_cpp_check_joinery_library_xml",
)
def check_joinery_library_xml(type=1, division_dist=1000, shift=0.6):
    """Find closest polylines within given distance

    Parameters
    ----------
    type of a joint : list of polylines

    Returns
    -------
    just prints a meesage
        nothing

    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    folder = os.path.join(
        os.path.dirname(os.path.abspath(__file__)), "joinery_library.xml"
    )

    # ==============================================================================
    # Test CPP module
    # ==============================================================================
    print(
        "============================================================================== CPP +"
    )

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================

    (
        male_plines,
        female_plines,
        m_boolean_type,
        f_boolean_type,
    ) = pybind11_joinery_solver.pybind11_check_joinery_library_xml(
        folder, type, division_dist, shift
    )

    print(
        "\n ============================================================================== CPP -"
    )

    male_plines_polylines = []
    for points_coord in male_plines:
        points = [Point(*point) for point in points_coord]
        polyline = Polyline(points)
        male_plines_polylines.append(polyline)

    female_plines_polylines = []
    for points_coord in female_plines:
        points = [Point(*point) for point in points_coord]
        polyline = Polyline(points)
        female_plines_polylines.append(polyline)

    male_boolean_types = []
    for i in m_boolean_type[0]:
        male_boolean_types.append(i)

    female_boolean_types = []
    for i in f_boolean_type[0]:
        female_boolean_types.append(i)

    return (
        male_plines_polylines,
        female_plines_polylines,
        male_boolean_types,
        female_boolean_types,
    )
