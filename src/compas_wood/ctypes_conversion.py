from ctypes import *

# # Convert the nested list to a flat list
# flat_list = [item for sublist in nested_list for item in sublist]

# # Convert the flat list to a ctypes array
# c_array = (ctypes.c_int * len(flat_list))(*flat_list)


def list_polylines_coord(polylines):
    

    v_s = 0
    f_s = len(polylines)
    f =  [None]*f_s
    for i in range(f_s):
        f[i] = len(polylines[i])
        v_s += len(polylines[i]) * 3

    _f = (c_size_t * f_s)(*f)  # polyline sizes
    _f_s = c_size_t(f_s)  # number of polylines
    _v_s = c_size_t(v_s)  # number of coordinates

    count = 0
    v =  [None]*v_s
    for i in range(f_s):
        for j in range(len(polylines[i])):
            v[count * 3 + 0] = polylines[i][j][0]
            v[count * 3 + 1] = polylines[i][j][1]
            v[count * 3 + 2] = polylines[i][j][2]
            count += 1
    _v = (c_float * v_s)(*v)  # polyline coordinates

    return _f, _f_s, _v, _v_s  # polyline sizes, number of polylines, polyline coordinates, number of coordinates


def lists_vectors_coord(vectors):
    f_s = len(vectors)
    f = [len(v) for v in vectors]
    v_s = sum(len(v) * 3 for v in vectors)
    v = [0.0] * v_s

    count = 0
    for i in range(len(vectors)):
        for j in range(len(vectors[i])):
            v[count * 3] = vectors[i][j].X
            v[count * 3 + 1] = vectors[i][j].Y
            v[count * 3 + 2] = vectors[i][j].Z
            count += 1

    return f, f_s, v, v_s


def lists_numbers_coord(numbers):
    v_s = 0
    f_s = len(numbers)
    f = [0] * len(numbers)

    for i in range(len(numbers)):
        f[i] = len(numbers[i])
        v_s += len(numbers[i]) * 1

    v = [0] * v_s

    count = 0
    for i in range(len(numbers)):
        for j in range(len(numbers[i])):
            v[count * 1 + 0] = numbers[i][j]
            count += 1

    return f, f_s, v, v_s


def list_numbers_coord(numbers):
    f_s = len(numbers)
    f = [0.0] * f_s
    for i in range(f_s):
        f[i] = numbers[i]
    return f, f_s


def coord_polylines_lists():
    pass


def coord_numbers_lists():
    pass



