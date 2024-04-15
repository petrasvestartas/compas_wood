from compas.geometry import Polyline, Point, Vector
from compas.datastructures import Mesh
import wood_nano as m


######################################################################################################
# Conversion functions for Polyline
######################################################################################################


def to_point(vectors, depth):
    if depth == 1:  # Single polyline
        polyline = m.point1()
        for coordinate_list in vectors:
            polyline.append(m.point(*coordinate_list))
        return polyline
    else:  # Nested polyline
        polyline = getattr(m, f"point{depth}")()
        for vec in vectors:
            polyline.append(to_point(vec, depth - 1))
        return polyline


def to_point1(polylines):
    return to_point(polylines, 1)


def to_point2(polylines):
    return to_point(polylines, 2)


def to_point3(polylines):
    return to_point(polylines, 3)


def to_point4(polylines):
    return to_point(polylines, 4)


def from_point1(point1, type="Polyline"):
    points = []
    for i in range(len(point1)):
        point = point1[i]
        points.append(Point(point[0], point[1], point[2]))
    if type == "Polyline":
        return Polyline(points)
    elif type == "Box" and len(points) == 8:
        faces = []
        faces.append([3, 2, 1, 0])
        faces.append([4, 5, 6, 7])
        faces.append([0, 1, 5, 4])
        faces.append([1, 2, 6, 5])
        faces.append([2, 3, 7, 6])
        faces.append([3, 0, 4, 7])
        return Mesh.from_vertices_and_faces(points, faces)


def from_point2(point2, type="Polyline"):
    return [from_point1(p, type=type) for p in point2]


def from_point3(point3, type="Polyline"):
    return [from_point2(p, type=type) for p in point3]


def from_point4(point4, type="Polyline"):
    return [from_point3(p, type=type) for p in point4]


######################################################################################################
# Conversion functions for Vectors
######################################################################################################


def to_vector(vectors, depth):
    if depth == 1:  # Single vector_of_vectors
        vector_of_vectors = m.vector1()
        for coordinate_list in vectors:
            vector_of_vectors.append(m.vector(*coordinate_list))
        return vector_of_vectors
    else:  # Nested vector_of_vectors
        vector = getattr(m, f"vector{depth}")()
        for vec in vectors:
            vector.append(to_vector(vec, depth - 1))
        return vector


def to_vector1(vectors):
    return to_vector(vectors, 1)


def to_vector2(vectors):
    return to_vector(vectors, 2)


def to_vector3(vectors):
    return to_vector(vectors, 3)


def to_vector4(vectors):
    return to_vector(vectors, 4)


def from_vector1(vector1):
    vectors = []
    for i in range(len(vector1)):
        vec = vector1[i]
        vectors.append(Vector(vec[0], vec[1], vec[2]))
    return vectors


def from_vector2(vector2):
    return [from_vector1(v) for v in vector2]


def from_vector3(vector3):
    return [from_vector2(v) for v in vector3]


def from_vector4(vector4):
    return [from_vector3(v) for v in vector4]


if __name__ == "__main__":

    # For polylines
    polylines = [
        Polyline([[0, 0, 0], [1, 1, 1], [2, 2, 2]]),
        Polyline([[0, 0, 0], [1, 1, 1]]),
    ]

    print("")
    vectors = [
        [[0, 0, 0], [1, 1, 1], [2, 2, 2]],
        [[0, 0, 0], [1, 1, 1]],
    ]

    print("")
    vector2 = to_point2(vectors)
    print(vector2)
    compas_vectors = from_point2(vector2)
    print(compas_vectors)
