from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs
from Rhino.Geometry import Plane, Mesh, Vector3d, Polyline
from Rhino.Geometry.Intersect.Intersection import PlanePlanePlane


class MyComponent(component):
    def RunScript(self, _mesh, _offset, _inlay_0, _inlay_1):
        if _mesh == None:
            return
        if _offset == None:
            _offset = 1

        def get_ngon_polylines(mesh):
            polylines = []

            for i in range(mesh.Ngons.Count):
                vertices = mesh.Ngons.GetNgon(i).BoundaryVertexIndexList()

                outline = Polyline()
                for v in vertices:
                    outline.Add(mesh.Vertices[v])
                outline.Add(outline[0])

                polylines.append(outline)

            return polylines

        def get_ngon_center(mesh, ngon):
            vertices = mesh.Ngons.GetNgon(ngon).BoundaryVertexIndexList()

            outline = Polyline()
            for v in vertices:
                outline.Add(mesh.Vertices[v])

            return outline.CenterPoint()

        def get_ngon_normal(mesh, n):
            vector3d = Vector3d.Zero

            faces = mesh.Ngons[n].FaceIndexList()

            for i in range(len(faces)):
                vector3d += Vector3d(mesh.FaceNormals[int(faces[i])])

            vector3d.Unitize()

            return vector3d

        def get_ngons_boundaries(mesh):
            boundaries = [[] for _ in range(mesh.Ngons.Count)]

            for i in range(mesh.Ngons.Count):
                boundaries[i] = mesh.Ngons[i].BoundaryVertexIndexList()

            return boundaries

        def get_ngons_boundaries_point3d(mesh, verticesIDs):
            pts = [[] for _ in range(len(verticesIDs))]

            for i in range(len(verticesIDs)):
                pts[i] = [mesh.Vertices[int(v)] for v in verticesIDs[i]]

            return pts

        def get_ngon_planes(mesh):
            p = [None] * mesh.Ngons.Count

            b = get_ngons_boundaries(mesh)
            pts = get_ngons_boundaries_point3d(mesh, b)

            for i in range(mesh.Ngons.Count):
                p[i] = Plane(get_ngon_center(mesh, i), get_ngon_normal(mesh, i))
            return p

        def get_ngon_face_adjacency(mesh):
            faceAdj = [None] * mesh.Ngons.Count

            for i in range(mesh.Ngons.Count):
                faceAdjTemp = []
                meshFaceIdInCurrentNGon = mesh.Ngons[i].FaceIndexList()

                for j in range(len(meshFaceIdInCurrentNGon)):
                    id = mesh.Faces.AdjacentFaces(meshFaceIdInCurrentNGon[j])

                    for k in range(len(id)):
                        ngonID = mesh.Ngons.NgonIndexFromFaceIndex(id[k])

                        if ngonID != i and ngonID not in faceAdjTemp:
                            faceAdjTemp.append(ngonID)

                faceAdj[i] = faceAdjTemp
            return faceAdj

        def move_plane_by_axis(plane, dist, axis=2):
            plane1 = Plane(plane)
            if axis == 0:
                plane1.Translate(plane1.XAxis * dist)
            elif axis == 1:
                plane1.Translate(plane1.YAxis * dist)
            else:
                plane1.Translate(plane1.Normal * dist)
            return plane1

        def polyline_from_planes(basePlane, sidePlanes, close=True):
            polyline = Polyline()

            for i in range(len(sidePlanes) - 1):
                result, pt = PlanePlanePlane(
                    basePlane, sidePlanes[i], sidePlanes[i + 1]
                )
                polyline.Add(pt)

            result, pt1 = PlanePlanePlane(
                basePlane, sidePlanes[len(sidePlanes) - 1], sidePlanes[0]
            )
            polyline.Add(pt1)

            if close:
                polyline.Add(polyline[0])

            return polyline

        def get_intersection_between_current_and_neihgbor_planes(
            planes, adjacent_faces, offset, inlay_0=False, inlay_1=False
        ):
            planes_offset = []

            for i in range(len(planes)):
                planes_offset.append(move_plane_by_axis(planes[i], -offset))

            polylines_no_offset = []
            polylines_offset = []

            for i in range(len(adjacent_faces)):  # iterate through nested list
                # base planes
                base_plane = planes[i]
                base_plane_offset = planes_offset[i]

                # get neighbor planes
                adjacent_planes_offset = []
                adjacent_planes_no_offset = []
                for j in range(len(adjacent_faces[i])):
                    adjacent_planes_offset.append(planes_offset[adjacent_faces[i][j]])
                    adjacent_planes_no_offset.append(planes[adjacent_faces[i][j]])

                    if inlay_0 and i == 0:
                        adjacent_planes_no_offset[j] = adjacent_planes_offset[j]

                    if inlay_1 and i == 1:
                        adjacent_planes_no_offset[j] = adjacent_planes_offset[j]

                if inlay_0 and i == 0:
                    base_plane = move_plane_by_axis(base_plane, -offset * 2)
                    base_plane_offset = move_plane_by_axis(
                        base_plane_offset, -offset * 2
                    )

                if inlay_1 and i == 1:
                    base_plane = move_plane_by_axis(base_plane, -offset * 2)
                    base_plane_offset = move_plane_by_axis(
                        base_plane_offset, -offset * 2
                    )

                # shift to right
                # adjacent_planes_offset.insert(0, adjacent_planes_offset.pop())#adjacent_planes_offset.append(adjacent_planes_offset.pop(0))
                # adjacent_planes_offset.insert(0, adjacent_planes_offset.pop())#adjacent_planes_offset.append(adjacent_planes_offset.pop(0))

                # perform intersection
                polylines_no_offset.append(
                    polyline_from_planes(base_plane, adjacent_planes_no_offset)
                )
                polylines_offset.append(
                    polyline_from_planes(base_plane_offset, adjacent_planes_offset)
                )

            return polylines_no_offset, polylines_offset

        # get planes from polylines
        polylines0 = get_ngon_polylines(_mesh)
        planes = get_ngon_planes(_mesh)
        adjacent_faces = get_ngon_face_adjacency(_mesh)
        _p0, _p1 = get_intersection_between_current_and_neihgbor_planes(
            planes, adjacent_faces, _offset, _inlay_0, _inlay_1
        )

        # offset all polylines inside to get bisector plates

        # create an inlay plate if user defines it

        # return outputs if you have them; here I try it for you:
        return (_p0, _p1)
