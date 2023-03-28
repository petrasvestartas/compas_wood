from wood_pybind11 import WoodVectorInt
from wood_pybind11 import WoodNestedVectorInt
from wood_pybind11 import WoodVectorDouble
from wood_pybind11 import WoodNestedVectorDouble
from compas.geometry import Polyline
from compas.datastructures import Mesh
from compas.geometry import Point
from compas.geometry import Frame
from compas.geometry import Box


def WoodVectorInt_to_list(wood_vector_int):
    my_list = []

    for i in range(0, len(wood_vector_int)):
        my_list.append(wood_vector_int[i])

    return my_list


def list_to_WoodVectorInt(list):
    my_list = WoodVectorInt(list)

    return my_list


def WoodNestedVectorInt_to_lists(wood_nested_vector_int):
    my_lists = []

    for i in range(len(wood_nested_vector_int)):
        my_list = []
        for j in range(0, len(wood_nested_vector_int[i])):
            my_list.append(wood_nested_vector_int[i][j])
        my_lists.append(my_list)

    return my_lists


def lists_to_WoodNestedVectorInt(lists):
    my_lists = WoodNestedVectorInt()

    for i in lists:
        my_lists.append(WoodVectorInt(i))

    return my_lists


def WoodVectorDouble_to_list(wood_vector_double):
    my_list = []

    for i in range(0, len(wood_vector_double)):
        my_list.append(wood_vector_double[i])

    return my_list


def list_to_WoodVectorDouble(list):
    my_list = WoodVectorDouble(list)

    return my_list


def WoodNestedVectorDouble_to_lists(wood_nested_vector_double):
    my_lists = []

    for i in range(len(wood_nested_vector_double)):
        my_list = []
        for j in range(0, len(wood_nested_vector_double[i])):
            my_list.append(wood_nested_vector_double[i][j])
        my_lists.append(my_list)

    return my_lists


def lists_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble([])

    for i in lists:
        my_lists.append(WoodVectorDouble(i))

    return my_lists


def lists_of_vectors_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble()

    for i in lists:
        list_coord = []
        for vector in i:
            list_coord.append(vector[0])
            list_coord.append(vector[1])
            list_coord.append(vector[2])
        my_lists.append(WoodVectorDouble(list_coord))

    return my_lists


def WoodNestedVectorDouble_to_polylines(wood_nested_vector_double):
    # polylines
    output_polylines = []

    for i in range(len(wood_nested_vector_double)):
        points = []
        for j in range(0, len(wood_nested_vector_double[i]), 3):
            points.append(
                [
                    wood_nested_vector_double[i][j + 0],
                    wood_nested_vector_double[i][j + 1],
                    wood_nested_vector_double[i][j + 2],
                ]
            )
        polyline = Polyline(points)
        output_polylines.append(polyline)

    return output_polylines


def WoodVectorFloat_and_WoodVectorInt_to_mesh(wood_vector_float, wood_vector_int):
    # >>> vertices_1 = [[0, 0, 0], [0, 500, 0], [500, 500, 0], [500, 0, 0]]
    # >>> vertices_2 = [[500, 0, 0], [500, 500, 0], [1000, 500, 0], [1000, 0, 0]]
    # >>> faces = [[0, 1, 2, 3]]

    vertices = []
    faces = []

    for i in range(0, len(wood_vector_float), 3):
        vertices.append(
            [
                wood_vector_float[i + 0],
                wood_vector_float[i + 1],
                wood_vector_float[i + 2],
            ]
        )

    for i in range(0, len(wood_vector_int), 3):
        faces.append(
            [
                wood_vector_int[i + 0],
                wood_vector_int[i + 1],
                wood_vector_int[i + 2],
            ]
        )

    return Mesh.from_vertices_and_faces(vertices, faces)


def polylines_to_WoodNestedVectorDouble(lists):
    my_lists = WoodNestedVectorDouble()

    for i in range(len(lists)):
        my_list = []
        for j in lists[i]:
            my_list.append(j[0])
            my_list.append(j[1])
            my_list.append(j[2])
        my_lists.append(WoodVectorDouble(my_list))

    return my_lists


def WoodNestedVectorDouble_to_vectorslist(wood_nested_vector_double):
    # vectors
    output_vectors = []
    for i in range(len(wood_nested_vector_double)):
        vectors = []
        for j in range(0, len(wood_nested_vector_double[i]), 3):
            vectors.append(
                [
                    wood_nested_vector_double[i][j + 0],
                    wood_nested_vector_double[i][j + 1],
                    wood_nested_vector_double[i][j + 2],
                ]
            )
        output_vectors.append(vectors)

    return output_vectors


def WoodNestedNestedVectorDouble_to_polylineslists(wood_nested_nested_vector_double):
    # lists of polylines
    output_polylines = []

    for i in wood_nested_nested_vector_double:
        polylines = []

        for j in i:
            points = []
            for k in range(0, len(j), 3):
                points.append([j[k + 0], j[k + 1], j[k + 2]])
            polylines.append(Polyline(points))

        output_polylines.append(polylines)

    return output_polylines


def WoodNestedNestedVectorDouble_to_polylineslist(wood_nested_nested_vector_double):
    # lists of polylines
    output_polylines = []

    for i in wood_nested_nested_vector_double:
        for j in i:
            points = []
            for k in range(0, len(j), 3):
                points.append([j[k + 0], j[k + 1], j[k + 2]])
            output_polylines.append(Polyline(points))

    return output_polylines


def WoodVectorDouble_to_box(wood_vector_double):
    points = []

    for i in range(0, len(wood_vector_double), 3):
        p = Point(wood_vector_double[i], wood_vector_double[i + 1], wood_vector_double[i + 2])
        points.append(p)

    point_center = (points[0] + points[6]) * 0.5

    xaxis = points[0] - points[4]
    yaxis = points[0] - points[3]
    zaxis = points[0] - points[1]
    frame = Frame(point_center, xaxis, yaxis)

    box = Box(frame, xaxis.length, yaxis.length, zaxis.length)
    vertices, faces = box.to_vertices_and_faces()
    mesh_box = Mesh.from_vertices_and_faces(vertices, faces)

    return mesh_box


def WoodNestedVectorDouble_to_boxeslist(wood_nested_vector_double):
    mesh_boxes = []
    for i in wood_nested_vector_double:
        mesh_boxes.append(WoodVectorDouble_to_box(i))

    return mesh_boxes