#from _typeshed import NoneType
import numpy as np
from compas.plugins import plugin
from compas.geometry import Point
from compas.geometry import Vector
from compas.geometry import Plane
from compas.geometry import Polyline
from compas.geometry import Box
from compas.geometry import bounding_box
import joinery_solver
import time


@plugin(category='compas_wood_cpp', pluggable_name='compas_wood_cpp_test')
def wrapper_test():
    joinery_solver.test()



@plugin(category='compas_wood_cpp', pluggable_name='compas_wood_cpp_test_joinery_solver')
def get_connection_zones(
    polylines_vertices_XYZ,
    face_vectors_XYZ=None,
    face_joints_types_int=None,
    three_valance_element_indices_and_instruction=None,
    default_joint_parameters=None
    ):
    """Compute joinery.
    Parameters
    ----------
    polylines_vertices_XYZ : pairs of polylines
    face_vectors_XYZ : vectors to orient a joint
    face_joints_types_int : joint id, existing in the code
    three_valance_element_indices_and_instruction : special case e.g. alignment of rectangles

    Returns
    -------
    nothing for now
    """

    # ==============================================================================
    # Prepare input
    # ==============================================================================
    flat_list_of_points = []
    vertex_count_per_polyline = []
    flat_list_of_face_vectors_XYZ = []
    flat_list_of_face_joints_types_int = []
    flat_list_of_three_valance_element_indices_and_instruction = []
    flat_list_default_joint_paramters = [1000, 0.5, 1,  1000, 0.5, 10 ,  1000, 0.5, 20 ,  1000, 0.5, 30 ,  1000, 0.5, 40 ,  1000, 0.5, 50 ]

    flag0 = face_vectors_XYZ!=None;
    if(flag0):
        flag0 = len(face_vectors_XYZ) == len(polylines_vertices_XYZ)*0.5
    print(flag0)


    flag1 = face_joints_types_int!=None;
    if(flag1):
        flag1 = len(face_joints_types_int) == len(polylines_vertices_XYZ)*0.5

    #print(flag1)
    #print(len(face_vectors_XYZ))
    #print(len(face_joints_types_int))
    #print(len(polylines_vertices_XYZ))

    flag2 = three_valance_element_indices_and_instruction!=None;

    if(default_joint_parameters != None):
        if(len(default_joint_parameters)==6*3):
            flat_list_default_joint_paramters = default_joint_parameters



    for i in range(0, (int)(len(polylines_vertices_XYZ)*0.5)):
        flat_list_of_points.extend(polylines_vertices_XYZ[i*2])
        flat_list_of_points.extend(polylines_vertices_XYZ[i*2+1])
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i*2]))
        vertex_count_per_polyline.append(len(polylines_vertices_XYZ[i*2+1]))

        if(flag0 and (len(face_vectors_XYZ[i]) == len(polylines_vertices_XYZ[i*2])+1) ):
            flat_list_of_face_vectors_XYZ.extend(face_vectors_XYZ[i])



        if(flag1 and ( len(face_joints_types_int[i])  == (len(polylines_vertices_XYZ[i*2])+1) ) ):
            flat_list_of_face_joints_types_int.extend(face_joints_types_int[i])

    if(flag2):
        for i in range(0, len(three_valance_element_indices_and_instruction)):
            flat_list_of_three_valance_element_indices_and_instruction.extend(three_valance_element_indices_and_instruction[i])


    # ==============================================================================
    # Convert to Numpy
    # ==============================================================================
    V = np.asarray(flat_list_of_points, dtype=np.float64)
    F = np.asarray(vertex_count_per_polyline, dtype=np.int32)
    D = np.asarray(flat_list_of_face_vectors_XYZ, dtype=np.float64)
    J = np.asarray(flat_list_of_face_joints_types_int, dtype=np.int32)
    X = np.asarray(flat_list_of_three_valance_element_indices_and_instruction, dtype=np.int32)
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
    print("============================================================================== CPP +")

    # ==============================================================================
    # Execute main CPP method
    # ==============================================================================


    start_time = time.time()
    pointsets = joinery_solver.connectionzonesCGAL.get_connection_zones_compas( V, F, D, J, X, P, search_type,division_distance,shift,output_type,triangulate )
    print("==================================== %s ms ====================================" %  round((time.time() - start_time)*1000.0) )

    # ==============================================================================
    # Process output
    # ==============================================================================

    polylines = []
    for points in pointsets:
        points = [Point(*point) for point in points]
        polyline = Polyline(points)
        polylines.append(polyline)

    print("============================================================================== CPP -")
    print("Output")
    print("Number of Polylines " , len(polylines))


    return polylines


