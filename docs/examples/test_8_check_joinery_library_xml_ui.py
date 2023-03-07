# compas_wood

from compas_wood.joinery import check_joinery_library_xml

# compas_view2
from compas.geometry import Point, Polyline
import compas.geometry as cg
from compas_view2.app import App


viewer = App(viewmode="shaded", enable_sidebar=True, width=3840, height=2160 - 250)
viewer.view.camera.distance = 4
# rotation_delta = 1
# pan_delta = 0.05
# viewer.view.camera.tx = 0
# viewer.view.camera.ty = 0
# viewer.view.camera.tz = -10
# viewer.view.camera.rz = 0
# viewer.view.camera.rx = -20
type_id = 1
shift = 0.5
divisions_distance = 1000


@viewer.slider(text="S", minval=0, maxval=100, value=50, step=1)
def slide1(value):
    listOfGlobals = globals()
    listOfGlobals["shift"] = value / 100.0
    viewer.view.objects.clear()
    global type_id
    global divisions_distance
    global shift
    add_joint_geometry(type_id, divisions_distance, shift)
    viewer.view.update()


@viewer.slider(text="D", minval=100, maxval=1000, value=500, step=1)
def slide0(value):
    viewer.view.objects.clear()
    listOfGlobals = globals()
    listOfGlobals["divisions_distance"] = value
    viewer.view.objects.clear()
    global type_id
    global divisions_distance
    global shift
    add_joint_geometry(type_id, divisions_distance, shift)
    viewer.view.update()


# global_id = 1
available_joints = [1, 2, 9, 10, 11, 12, 19, 20, 21, 22, 23, 29, 30, 31, 39, 49, 59]
for i in available_joints:

    @viewer.button(text="Joint" + str(i), number=i)
    def click(id=i, local_number=type_id):
        viewer.view.objects.clear()
        global type_id
        listOfGlobals = globals()
        listOfGlobals["type_id"] = id

        # print(type_id)
        add_joint_geometry(id)
        viewer.view.update()


def select(value):
    viewer.view.mode = value
    viewer.view.update()


def add_joint_geometry(type, division_dist=1000, shift=0.6):

    (
        male_plines_polylines,
        female_plines_polylines,
        male_boolean_types,
        female_boolean_types,
    ) = check_joinery_library_xml(type, division_dist, shift)

    if male_plines_polylines is not None:
        for i in range(0, len(male_plines_polylines)):

            if i % 2 == 0:
                display_polyline(viewer, male_plines_polylines[i], 1, 1, 0, 0, 3)
            else:
                display_polyline(viewer, male_plines_polylines[i], 1, 1, 0, 0, 1)

            xform = cg.transformations.Translation.from_vector([2, 0, 0])
            transformed_pline = male_plines_polylines[i].transformed(xform)

            if i % 2 == 0:
                display_polyline(viewer, transformed_pline, 1, 1, 0, 0, 3)
            else:
                display_polyline(viewer, transformed_pline, 1, 1, 0, 0, 1)

    if female_plines_polylines is not None:
        for i in range(0, len(female_plines_polylines)):
            if i % 2 == 0:
                display_polyline(viewer, female_plines_polylines[i], 1, 0, 0, 1, 3)
            else:
                display_polyline(viewer, female_plines_polylines[i], 1, 0, 0, 1, 1)

            xform = cg.transformations.Translation.from_vector([-2, 0, 0])
            transformed_pline = female_plines_polylines[i].transformed(xform)

            if i % 2 == 0:
                display_polyline(viewer, transformed_pline, 1, 0, 0, 1, 3)
            else:
                display_polyline(viewer, transformed_pline, 1, 0, 0, 1, 1)

    display_corners = [
        Polyline([Point(-0.55, 0.55, 0.55), Point(-0.44, 0.55, 0.55)]),
        Polyline([Point(-0.55, 0.55, 0.55), Point(-0.55, 0.55, 0.44)]),
        Polyline([Point(-0.55, 0.55, 0.55), Point(-0.55, 0.44, 0.55)]),
        Polyline([Point(-0.55, -0.55, 0.55), Point(-0.55, -0.44, 0.55)]),
        Polyline([Point(-0.55, -0.55, 0.55), Point(-0.55, -0.55, 0.44)]),
        Polyline([Point(-0.55, -0.55, 0.55), Point(-0.44, -0.55, 0.55)]),
        Polyline([Point(0.55, -0.55, 0.55), Point(0.44, -0.55, 0.55)]),
        Polyline([Point(0.55, -0.55, 0.55), Point(0.55, -0.55, 0.44)]),
        Polyline([Point(0.55, -0.55, 0.55), Point(0.55, -0.44, 0.55)]),
        Polyline([Point(0.55, 0.55, 0.55), Point(0.55, 0.44, 0.55)]),
        Polyline([Point(0.55, 0.55, 0.55), Point(0.55, 0.55, 0.44)]),
        Polyline([Point(0.55, 0.55, 0.55), Point(0.44, 0.55, 0.55)]),
        Polyline([Point(-0.55, 0.55, -0.55), Point(-0.44, 0.55, -0.55)]),
        Polyline([Point(-0.55, 0.55, -0.55), Point(-0.55, 0.55, -0.44)]),
        Polyline([Point(-0.55, 0.55, -0.55), Point(-0.55, 0.44, -0.55)]),
        Polyline([Point(-0.55, -0.55, -0.55), Point(-0.55, -0.44, -0.55)]),
        Polyline([Point(-0.55, -0.55, -0.55), Point(-0.55, -0.55, -0.44)]),
        Polyline([Point(-0.55, -0.55, -0.55), Point(-0.44, -0.55, -0.55)]),
        Polyline([Point(0.55, -0.55, -0.55), Point(0.44, -0.55, -0.55)]),
        Polyline([Point(0.55, -0.55, -0.55), Point(0.55, -0.55, -0.44)]),
        Polyline([Point(0.55, -0.55, -0.55), Point(0.55, -0.44, -0.55)]),
        Polyline([Point(0.55, 0.55, -0.55), Point(0.55, 0.44, -0.55)]),
        Polyline([Point(0.55, 0.55, -0.55), Point(0.55, 0.55, -0.44)]),
        Polyline([Point(0.55, 0.55, -0.55), Point(0.44, 0.55, -0.55)]),
    ]
    for line in display_corners:
        display_polyline(viewer, line, 1, 0, 0, 0, 3)

    # 1 2 9   10 11 12 19   20 21 22 23 29   30 31 39    49    59
    # 1 - weird intersecting segment low distances issues - 22 23


def display_polyline(viewer, polyline, scale=0.01, r=0.0, g=0.0, b=0.0, t=1):
    if len(polyline) > 1:
        viewer.add(polyline, color=(r, g, b), linewidth=t)


viewer.view.objects.clear()
add_joint_geometry(1)

viewer.view.update()
viewer.run()
