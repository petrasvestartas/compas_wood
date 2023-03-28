# compas
import compas.geometry as cg
from compas.datastructures import Mesh

# viewer
from compas_view2.app import App
from compas_view2.objects import Collection


def disply_joint(
    male_plines_polylines,
    female_plines_polylines,
    male_boolean_types,
    female_boolean_types,
):
    viewer = App(
        # width=3840,
        # height=2160 - 250,
    )

    if male_plines_polylines is not None:
        for i in range(0, len(male_plines_polylines)):
            if i % 2 == 0:
                display_polyline(viewer, male_plines_polylines[i], 1, 1, 0, 0, 3)
            else:
                display_polyline(viewer, male_plines_polylines[i], 1, 1, 0, 0, 1)

    if female_plines_polylines is not None:
        for i in range(0, len(female_plines_polylines)):
            if i % 2 == 0:
                display_polyline(viewer, female_plines_polylines[i], 1, 0, 0, 1, 3)
            else:
                display_polyline(viewer, female_plines_polylines[i], 1, 0, 0, 1, 1)

    viewer.run()


def display(
    input=None,
    result=None,
    meshes=None,
    scale=0.01,
    movex_0=1.25,
    movex_1=0,
    movex_2=0,
    color_first=False,
    display_polylines_as_mesh_types=None,
):
    viewer = App(
        # width=3840,
        # height=2160 - 250,
    )

    viewer.view.camera.fov = 10

    # preview
    if input is not None:
        display_polylines(
            viewer, input, scale, 0.25, 0.25, 0.25, 3, True, movex_0
        )  # polylines without joints

    if result is not None:
        flag = False
        if display_polylines_as_mesh_types is not None:
            if len(display_polylines_as_mesh_types) == len(result):
                flag = True

        if flag:
            display_polylines_as_mesh(
                viewer, result, display_polylines_as_mesh_types, scale
            )
        else:
            if len(result) > 0:
                display_polylines(
                    viewer, result, scale, 206 / 255, 0, 88 / 255.0, 3, False, movex_1
                )  # polylines with joints
                # display_polylines(viewer,result[i],scale,randrange(0,206)/255.0,0,randrange(0,88)/255.0, 3,False,movex_1) #polylines with joints

    if meshes is not None:
        for i in range(len(meshes)):
            if i == 0 and color_first:
                display_mesh(viewer, meshes[i], scale, 206 / 255, 0, 88 / 255.0, 3)
            else:
                display_mesh(viewer, meshes[i], scale, 0.9, 0.9, 0.9, 3)

    viewer.run()


def display_lines(viewer, lines, scale=0.01):
    linesScaled = []
    for i in lines:
        line = cg.Line(i[0] * scale, i[1] * scale)
        linesScaled.append(line)
    viewer.add(Collection(linesScaled), color=(0, 0, 0), linewidth=2)


def display_polylines_as_mesh(viewer, polylines, types, scale=0.01, t=3):
    # 10 - SS Rotate 11 - SS OUT OF PLANE 12 - SS IN Plane,  20 Top-Side, 30 - Cross
    my_dict = {10: 0, 11: 1, 12: 2, 20: 3, 30: 4, 13: 5}
    out_mesh = [[], [], [], [], [], []]
    out_mesh_colors = [
        (255 / 255.0, 0 / 255.0, 0 / 255.0),
        (255 / 255.0, 0 / 255.0, 100 / 255.0),
        (206 / 255, 0, 88 / 255.0),
        (0 / 255.0, 146 / 255.0, 210 / 255.0),
        (200 / 255.0, 174 / 255.0, 102 / 255.0),
        (255 / 255.0, 0 / 255.0, 25 / 255.0),
    ]
    # 0 Red Side-Side 1 Yellow Top-Side 2 Grey Top-Top 3 Blue Cross

    for i in range(len(polylines)):
        polyline = polylines[i]
        polyline.transform(
            cg.transformations.scale.matrix_from_scale_factors([scale, scale, scale])
        )
        mesh = Mesh.from_polygons([polyline])
        # print(types[i])
        out_mesh[my_dict[types[i]]].append(mesh)

    for i in range(len(out_mesh)):
        # print(len(outPolylines[i]))
        if len(out_mesh[i]) > 0:
            # for j in outPolylines[i]:
            #    viewer.add(j,color = outPolylinesColors[i],opacity=0.75)
            colors_mesh = []
            for j in range(len(out_mesh[i])):
                colors_mesh.append(out_mesh_colors[i])
                viewer.add(
                    out_mesh[i][j],
                    color=out_mesh_colors[i],
                    opacity=0.5,
                )

            # viewer.add(Collection(out_mesh[i]),colors =colors_mesh)
    viewer.add(Collection(polylines), linewidth=t)


def display_polylines(
    viewer, polylines, scale=0.00, r=0.0, g=0.0, b=0.0, t=1, collection=True, movex=1.25
):
    polylinesScaled = []
    colors = []

    for i in polylines:
        # print(i)
        if len(i) > 1:
            a = i.transformed(
                cg.transformations.scale.matrix_from_scale_factors(
                    [scale, scale, scale]
                )
            )
            polylinesScaled.append(a)
            if collection is False:
                if len(i) == 4:
                    viewer.add(a, color=(0, 0, 1), linewidth=1)
                elif len(i) == 2:
                    if a[0].distance_to_point(a[1]) < 0.001:
                        viewer.add(a[0], color=(206 / 255, 0, 88 / 255.0), size=15)
                    else:
                        viewer.add(a, color=(206 / 255, 0, 88 / 255.0), linewidth=10)
                else:
                    viewer.add(a, linewidth=t)

            if collection is True:
                colors.append((r, g, b))
                # colors.append((1,0,0))

    # Translation within function, because there is scaling
    # translate input data
    if movex > 1:
        points = []
        for i in range(len(polylinesScaled)):
            points.extend(polylinesScaled[i])

        bbox = cg.bounding_box(points)
        xmin = bbox[0][0]
        xmax = bbox[1][0]
        offset_dist = abs(xmax - xmin)

        T = cg.transformations.Translation.from_vector([-offset_dist * movex, 0, 0])
        for i in range(len(polylinesScaled)):
            polylinesScaled[i].transform(T)

    if collection:
        if len(polylinesScaled):
            viewer.add(
                Collection(polylinesScaled), linewidth = t
            )

        else:
            print("Nothing is displayed")


def display_polyline(viewer, polyline, scale=0.01, r=0.0, g=0.0, b=0.0, t=1):
    if len(polyline) > 1:
        y = polyline.transformed(
            cg.transformations.scale.matrix_from_scale_factors([scale, scale, scale])
        )
        viewer.add(y, color=(r, g, b), linewidth=t)


def display_mesh(viewer, input, scale=0.01, r=0.0, g=0.0, b=0.0, t=1):
    y = input.transformed(
        cg.transformations.scale.matrix_from_scale_factors([scale, scale, scale])
    )
    # viewer.add(y, hide_coplanaredges=False)

    viewer.add(
        y,
        facecolor=(r, g, b),
        linecolor=(0, 0, 0),
        opacity=0.5,
        linewidth=1,
        show_lines=False,
        hide_coplanaredges=True,
        show_faces=True,

    )
