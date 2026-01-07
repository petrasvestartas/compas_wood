from compas.geometry import Point
from compas.geometry import Vector
from compas.geometry import Frame
from compas.geometry import Transformation

frame = Frame(
    Point(0, 0, 0), 
    [0, 0, 1], 
    [0, 1, 0])

points = [
    Point(0, 0, 0), 
    Point(0, 1, 0), 
    Point(0, 1, 1), 
    Point(0, 0, 1) ]

t = Transformation.from_frame_to_frame(frame, Frame.worldXY())

for point in points:
    print(point.transformed(t))
    



# points = [
#     Point(0, 0, 0), 
#     Point(1, 0, 0), 
#     Point(0, 1, 0), 
#     Point(0, 0, 1) ]
# t = Transformation.from_frame_to_frame(Frame.worldXY(), frame)

# for point in points:
#     print(point.transformed(t))


# v0 = Vector(0, 1, 0)
# v1 = Vector(1, 0, 0)
# print(v0.cross(v1))


# frame = Frame(
#     Point(0, 0, 0), 
#     [0, 1, 0], 
#     [0, 0, 1])


# points = [
#     Point(0, 0, 0), 
#     Point(1, 0, 0), 
#     Point(0, 1, 0), 
#     Point(0, 0, 1) ]
# t = Transformation.from_frame_to_frame(Frame.worldXY(), frame)

# for point in points:
#     print(point.transformed(t))
