from ctypes import *

# # Convert the nested list to a flat list
# flat_list = [item for sublist in nested_list for item in sublist]

# # Convert the flat list to a ctypes array
# c_array = (ctypes.c_int * len(flat_list))(*flat_list)


def list_polylines_coord(polylines):
    f = []
    v = []
    for polyline in polylines:
        f.append(len(polyline))
        for point in polyline:
            v.extend(point)
    _f = (c_size_t * len(f))(*f)
    _v = (c_float * len(v))(*v)
    return _f, c_size_t(len(f)), _v, c_size_t(len(v))


def lists_vectors_coord(vectors):
    faces = [len(v) for v in vectors]
    f = (c_size_t * len(vectors))(*faces)
    v = [coord for vec in vectors for point in vec for coord in point]
    v_s = c_size_t(len(v) * sizeof(c_float))
    v = (c_float * len(v))(*v)
    return f, c_size_t(len(vectors)), v, v_s




def lists_numbers_coord(numbers):
    v_s = 0
    f_s = len(numbers)
    f = (c_size_t * len(numbers))()

    for i in range(len(numbers)):
        f[i] = len(numbers[i])
        v_s += len(numbers[i]) * 1

    v = (c_int * v_s)()

    count = 0
    for i in range(len(numbers)):
        for j in range(len(numbers[i])):
            v[count] = numbers[i][j]
            count += 1

    return f, c_size_t(f_s), v, c_size_t(v_s)


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
