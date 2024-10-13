import Rhino
import typing
from wood_rui import wood_rui_globals, add_mesh, add_polylines
from typing import List, Dict, Tuple, Union
from Rhino.Geometry import (
    Mesh,
    Vector3d,
    Vector3f,
    Plane,
    Polyline,
    Line,
    Point3d,
    Transform,
    Rectangle3d,
    Interval,
    MeshFace,
)
import math


def default_dataset():
    return [
        Polyline(
            [
                Point3d(56.777139, 431.670746, -52.226174),
                Point3d(69.959354, 463.713542, -35.935164),
                Point3d(89.60588, 462.511875, -53.589404),
                Point3d(80.211012, 442.274112, -62.97383),
            ]
        ),
        Polyline(
            [
                Point3d(8.608377, 419.269539, -39.886569),
                Point3d(-8.663903, 443.952584, -15.463778),
                Point3d(13.787643, 480.589531, -0.031694),
                Point3d(61.181275, 481.789138, -19.725794),
                Point3d(69.959354, 463.713542, -35.935164),
                Point3d(56.777139, 431.670746, -52.226174),
            ]
        ),
        Polyline(
            [
                Point3d(61.181275, 481.789138, -19.725794),
                Point3d(84.480109, 521.354825, -21.213415),
                Point3d(104.59611, 521.7907, -38.498946),
                Point3d(89.60588, 462.511875, -53.589404),
                Point3d(69.959354, 463.713542, -35.935164),
            ]
        ),
        Polyline(
            [
                Point3d(13.787643, 480.589531, -0.031694),
                Point3d(-3.736258, 515.545854, 14.692483),
                Point3d(16.413291, 555.883057, 14.692483),
                Point3d(67.923253, 555.883057, -6.986232),
                Point3d(84.480109, 521.354825, -21.213415),
                Point3d(61.181275, 481.789138, -19.725794),
            ]
        ),
        Polyline(
            [
                Point3d(-135.351666, 555.883057, -7.519537),
                Point3d(-151.170269, 521.352276, -21.112346),
                Point3d(-171.359806, 521.789745, -38.461065),
                Point3d(-171.359806, 589.97637, -38.461065),
                Point3d(-151.170269, 590.413839, -21.112346),
            ]
        ),
        Polyline(
            [
                Point3d(-62.990813, 515.545854, 14.692483),
                Point3d(-82.379744, 555.883057, 14.692483),
                Point3d(-62.990813, 596.220261, 14.692483),
                Point3d(-3.736258, 596.220261, 14.692483),
                Point3d(16.413291, 555.883057, 14.692483),
                Point3d(-3.736258, 515.545854, 14.692483),
            ]
        ),
        Polyline(
            [
                Point3d(67.923253, 555.883057, -6.986232),
                Point3d(84.480109, 590.41129, -21.213415),
                Point3d(104.59611, 589.975415, -38.498946),
                Point3d(104.59611, 521.7907, -38.498946),
                Point3d(84.480109, 521.354825, -21.213415),
            ]
        ),
        Polyline(
            [
                Point3d(-135.351666, 555.883057, -7.519537),
                Point3d(-151.170269, 590.413839, -21.112346),
                Point3d(-128.658774, 627.554268, -19.158681),
                Point3d(-79.733844, 628.181215, 1.230004),
                Point3d(-62.990813, 596.220261, 14.692483),
                Point3d(-82.379744, 555.883057, 14.692483),
            ]
        ),
        Polyline(
            [
                Point3d(16.413291, 555.883057, 14.692483),
                Point3d(-3.736258, 596.220261, 14.692483),
                Point3d(13.787643, 631.176584, -0.031694),
                Point3d(61.181275, 629.976976, -19.725794),
                Point3d(84.480109, 590.41129, -21.213415),
                Point3d(67.923253, 555.883057, -6.986232),
            ]
        ),
        Polyline(
            [
                Point3d(-128.658774, 627.554268, -19.158681),
                Point3d(-151.170269, 590.413839, -21.112346),
                Point3d(-171.359806, 589.97637, -38.461065),
                Point3d(-156.359503, 649.250927, -53.540739),
                Point3d(-136.873829, 648.059099, -36.031041),
            ]
        ),
        Polyline(
            [
                Point3d(-62.990813, 596.220261, 14.692483),
                Point3d(-79.733844, 628.181215, 1.230004),
                Point3d(-58.167438, 667.813531, -15.463778),
                Point3d(-8.663903, 667.813531, -15.463778),
                Point3d(13.787643, 631.176584, -0.031694),
                Point3d(-3.736258, 596.220261, 14.692483),
            ]
        ),
        Polyline(
            [
                Point3d(61.181275, 629.976976, -19.725794),
                Point3d(70.912718, 650.015716, -37.695621),
                Point3d(89.206901, 650.836628, -53.993246),
                Point3d(104.59611, 589.975415, -38.498946),
                Point3d(84.480109, 590.41129, -21.213415),
            ]
        ),
        Polyline(
            [
                Point3d(-128.658774, 627.554268, -19.158681),
                Point3d(-136.873829, 648.059099, -36.031041),
                Point3d(-120.635147, 681.57991, -51.00163),
                Point3d(-77.640259, 692.382727, -39.77392),
                Point3d(-58.167438, 667.813531, -15.463778),
                Point3d(-79.733844, 628.181215, 1.230004),
            ]
        ),
        Polyline(
            [
                Point3d(13.787643, 631.176584, -0.031694),
                Point3d(-8.663903, 667.813531, -15.463778),
                Point3d(8.608377, 692.496576, -39.886569),
                Point3d(54.893818, 680.603319, -51.759562),
                Point3d(70.912718, 650.015716, -37.695621),
                Point3d(61.181275, 629.976976, -19.725794),
            ]
        ),
        Polyline(
            [
                Point3d(-156.359503, 649.250927, -53.540739),
                Point3d(-147.018792, 669.492003, -62.97383),
                Point3d(-120.635147, 681.57991, -51.00163),
                Point3d(-136.873829, 648.059099, -36.031041),
            ]
        ),
        Polyline(
            [
                Point3d(-58.167438, 667.813531, -15.463778),
                Point3d(-77.640259, 692.382727, -39.77392),
                Point3d(8.608377, 692.496576, -39.886569),
                Point3d(-8.663903, 667.813531, -15.463778),
            ]
        ),
        Polyline(
            [
                Point3d(70.912718, 650.015716, -37.695621),
                Point3d(54.893818, 680.603319, -51.759562),
                Point3d(80.132476, 669.382911, -63.063455),
                Point3d(89.206901, 650.836628, -53.993246),
            ]
        ),
        Polyline(
            [
                Point3d(-128.658774, 484.211847, -19.158681),
                Point3d(-151.170269, 521.352276, -21.112346),
                Point3d(-135.351666, 555.883057, -7.519537),
                Point3d(-82.379744, 555.883057, 14.692483),
                Point3d(-62.990813, 515.545854, 14.692483),
                Point3d(-79.733844, 483.5849, 1.230004),
            ]
        ),
        Polyline(
            [
                Point3d(-128.658774, 484.211847, -19.158681),
                Point3d(-137.65658, 461.753267, -37.638679),
                Point3d(-155.941453, 460.932773, -53.92801),
                Point3d(-171.359806, 521.789745, -38.461065),
                Point3d(-151.170269, 521.352276, -21.112346),
            ]
        ),
        Polyline(
            [
                Point3d(-146.940256, 442.383204, -63.063455),
                Point3d(-155.941453, 460.932773, -53.92801),
                Point3d(-137.65658, 461.753267, -37.638679),
                Point3d(-118.602906, 429.675588, -50.47093),
            ]
        ),
        Polyline(
            [
                Point3d(-58.167438, 443.952584, -15.463778),
                Point3d(-79.733844, 483.5849, 1.230004),
                Point3d(-62.990813, 515.545854, 14.692483),
                Point3d(-3.736258, 515.545854, 14.692483),
                Point3d(13.787643, 480.589531, -0.031694),
                Point3d(-8.663903, 443.952584, -15.463778),
            ]
        ),
        Polyline(
            [
                Point3d(-118.602906, 429.675588, -50.47093),
                Point3d(-137.65658, 461.753267, -37.638679),
                Point3d(-128.658774, 484.211847, -19.158681),
                Point3d(-79.733844, 483.5849, 1.230004),
                Point3d(-58.167438, 443.952584, -15.463778),
                Point3d(-77.640259, 419.383387, -39.77392),
            ]
        ),
        Polyline(
            [
                Point3d(-77.640259, 419.383387, -39.77392),
                Point3d(-58.167438, 443.952584, -15.463778),
                Point3d(-8.663903, 443.952584, -15.463778),
                Point3d(8.608377, 419.269539, -39.886569),
            ]
        ),
    ]


class cross_connectors:
    def __init__(
        self,
        m,
        s,
        face_thickness,
        face_positions,
        edge_divisions,
        edge_division_len,
        insertion_vectors,
        rect_width,
        rect_height,
        rect_thickness,
        chamfer,
        projection_breps = [],
    ):
        self.m = m.DuplicateMesh()

        if self.m is None:
            # Create vertices
            vertices = []
            vertices.append(Point3d(0, 0, 0))
            vertices.append(Point3d(100, 0, 0))
            vertices.append(Point3d(100, 100, 0))
            vertices.append(Point3d(0, 100, 0))

            # Create faces
            faces = []
            faces.append(MeshFace(0, 1, 2))
            faces.append(MeshFace(0, 2, 3))

            # Create mesh
            self.m = Mesh()

            # Add vertices to the mesh
            for vertex in vertices:
                self.m.Vertices.Add(vertex)

            # Add faces to the mesh
            for face in faces:
                self.m.Faces.AddFace(face)

            # Optional: Compute vertex normals
            self.m.Normals.ComputeNormals()

            # Optional: Compact the mesh to remove unused vertices
            self.m.Compact()

            # Optional: Update the mesh to display changes
            self.m.FaceNormals.ComputeFaceNormals()
            self.m.Normals.ComputeNormals()
            self.m.Normals.UnitizeNormals()
            Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(self.m)

        self.s = None
        self.face_thickness = face_thickness if face_thickness is not None else 1
        self.face_positions = list(face_positions) if face_positions is not None else [0]
        self.face_positions.sort()

        if len(self.face_positions) == 0:
            self.face_positions = [0]

        self.projection_breps = projection_breps
        self.edge_divisions = edge_divisions if edge_divisions is not None else [2]
        self.edge_division_len = edge_division_len if edge_division_len is not None else []
        if len(self.edge_division_len) > 0:
            if self.edge_division_len[0] < 0.01:
                self.edge_division_len = []
        self.has_ngons = len(self.m.Ngons) > 0
        self.rect_width = rect_width if rect_width is not None else 10
        self.rect_height = rect_height if rect_height is not None else 10
        self.rect_thickness = rect_thickness if rect_thickness is not None else 1
        self.f = None
        self.e = None
        self.f_v = None
        self.f_e = None
        self.e_f = None
        self.e_ngon_e = {}
        self.f_planes = []
        self.fe_planes = []
        self.bi_planes = []
        self.e90_planes = []
        self.e90_multiple_planes = []
        self.e_lines = []
        self.f_polylines_index = []
        self.f_polylines_planes = []
        self.f_polylines = []
        self.e_polylines_index = []
        self.e_polylines_planes = []
        self.e_polylines = []

        if self.m is not None:
            if self.m.IsValid:
                self.m.Weld(0.01)
                self.m.RebuildNormals()
            else:
                return
        else:
            return
        self.s = s
        # run methods in constructor
        self.get_face_vertices()
        self.get_faces_planes()
        self.get_face_edge_planes()
        self.get_bisector_planes()
        self.get_face_polylines(chamfer)
        self.get_edges()
        self.get_edge_faces()
        self.get_edge_vectors(insertion_vectors)
        self.get_edge_planes()
        self.get_connectors()

    def vertex_topo_vertex(self, mesh, vertex_ids):
        topo_vertices = []
        for i in range(len(vertex_ids)):
            topo_vertices.append(mesh.TopologyVertices.TopologyVertexIndex(vertex_ids[i]))

        return topo_vertices

    def get_ngons_topo_boundaries(self, mesh):
        boundaries = []
        for i in range(mesh.Ngons.Count):
            boundaries.append(self.vertex_topo_vertex(mesh, mesh.Ngons[i].BoundaryVertexIndexList()))

        return boundaries

    def get_face_vertices(self):
        if self.has_ngons:
            self.f = self.m.Ngons.Count
            self.f_v = self.get_ngons_topo_boundaries(self.m)
        else:
            self.f = self.m.Faces.Count
            self.f_v = []
            for i in range(self.m.Faces.Count):
                self.f_v.append(
                    [
                        self.m.Faces[i][0],
                        self.m.Faces[i][1],
                        self.m.Faces[i][2],
                        self.m.Faces[i][3],
                    ]
                    if self.m.Faces[i].IsQuad
                    else [
                        self.m.Faces[i][0],
                        self.m.Faces[i][1],
                        self.m.Faces[i][2],
                    ]
                )

    def get_normal(self, mesh, f):
        vector3d = Vector3f(0, 0, 0)

        faces = mesh.Ngons[f].FaceIndexList()

        for i in range(len(faces)):
            vector3d += mesh.FaceNormals[faces[i]]

        vector3d.Unitize()

        return Vector3d(vector3d)

    def center_point(self, polyline):
        count = len(polyline)

        if count == 0:
            return Point3d.Unset

        if count == 1:
            return polyline[0]

        center = Point3d.Origin
        weight = 0.0

        stop = count - 1
        if polyline[0].DistanceToSquared(polyline[polyline.Count - 1]) > 0.001:
            stop += 1

        for i in range(stop):
            A = polyline[i]
            B = polyline[(i + 1) % count]
            d = A.DistanceTo(B)
            center += d * 0.5 * (A + B)
            weight += d

        center /= weight

        return center

    def face_center(self, mesh, f):
        vertices = mesh.Ngons.GetNgon(f).BoundaryVertexIndexList()
        outline = Polyline()
        for v in vertices:
            outline.Add(mesh.Vertices[v])
        return self.center_point(outline)

    def get_plane(self, mesh, f):
        return Plane(self.face_center(mesh, f), self.get_normal(mesh, f))

    def get_f_planes(self, mesh):
        planes = []
        for i in range(mesh.Ngons.Count):
            plane = self.get_plane(mesh, i)
            planes.append(plane)
        return planes

    def get_faces_planes(self):
        if self.has_ngons:
            self.f_planes = self.get_f_planes(self.m)
        else:
            self.f_planes = [None] * self.m.Faces.Count
            for i in range(self.m.Faces.Count):
                center = self.m.Faces.GetFaceCenter(i)
                self.f_planes[i] = Plane(center, self.m.FaceNormals[i])

        # Adjust to surface
        if self.s is not None:
            for i in range(len(self.f_planes)):
                u, v = None, None
                self.s.ClosestPoint(self.f_planes[i].Origin, u, v)
                normal = self.s.NormalAt(u, v)
                xaxis = Vector3d.CrossProduct(normal, self.f_planes[i].YAxis)
                yaxis = Vector3d.CrossProduct(normal, xaxis)
                self.f_planes[i] = Plane(self.f_planes[i].Origin, xaxis, yaxis)

    def get_face_edge_planes(self):
        self.fe_planes = []

        for i in range(self.f):
            n = len(self.f_v[i])
            self.fe_planes.append([None] * n)

            for j in range(len(self.f_v[i])):
                v1 = self.f_v[i][(j + 0) % n]
                v2 = self.f_v[i][(j + 1) % n]

                e = self.m.TopologyEdges.GetEdgeIndex(v1, v2)
                cf = self.m.TopologyEdges.GetConnectedFaces(e)

                sum = Vector3f.Zero
                for k in range(len(cf)):
                    sum += self.m.FaceNormals[cf[k]]
                sum /= len(cf)

                edgePlane = Plane(
                    Point3d(self.m.Vertices[v1] + self.m.Vertices[v2]) * 0.5,
                    self.m.Vertices[v1] - self.m.Vertices[v2],
                    sum,
                )
                self.fe_planes[i][j] = edgePlane

                # Adjust to surface
                if self.s is not None:
                    u, v = None, None
                    self.s.ClosestPoint(self.fe_planes[i][j].Origin, u, v)
                    normal = self.s.NormalAt(u, v)
                    self.fe_planes[i][j] = Plane(
                        self.fe_planes[i][j].Origin,
                        Vector3d.CrossProduct(normal, self.fe_planes[i][j].ZAxis),
                        normal,
                    )

                for brep in self.projection_breps:
                    if brep.IsPointInside(self.fe_planes[i][j].Origin, 0.01, False):
                        xform = Rhino.Geometry.Transform.PlanarProjection(Plane.WorldXY)
                        self.fe_planes[i][j].Transform(xform)

    def to_line(self, v, p, scale=1):
        return Line(p, p + v * scale)

    def dihedral_plane(self, plane0, plane1):
        # Plane between two Planes
        result, line = Rhino.Geometry.Intersect.Intersection.PlanePlane(plane0, plane1)
        result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(line, plane0)
        centerDihedral = line.PointAt(t)

        # Convert ZAxis to lines, cant it be done on origin
        angleLine0 = self.to_line(plane0.ZAxis, plane0.Origin)
        angleLine1 = self.to_line(plane1.ZAxis, plane1.Origin)

        # Plane cannot be anti/parallel and they must be apart from each other
        if not plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) and plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001:
            # Intersect two lines to get center
            result, t0, t1 = Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1, 0.01, False)

            # Construct plane from LineLine center and plane origins
            center = angleLine0.PointAt(t0)
            v0 = plane0.Origin - center
            v1 = plane1.Origin - center
            v0.Unitize()
            v1.Unitize()

            # Compute Plane
            dihedralPlane = Rhino.Geometry.Plane(centerDihedral, line.Direction, v0 + v1)

            return dihedralPlane
        else:
            return Rhino.Geometry.Plane.Unset  # Axes are parallel or anti-parallel, or planes are at the same location

    def get_bisector_planes(self):
        self.bi_planes = []

        for i in range(self.f):
            n = len(self.f_v[i])
            self.bi_planes.append([None] * n)

            for j in range(n):
                pl1 = self.fe_planes[i][j]
                pl0 = self.fe_planes[i][(j + 1) % n]
                self.bi_planes[i][j] = self.dihedral_plane(pl0, pl1)

    def face_vertices(self, mesh, F, topo):
        uintv = mesh.Ngons[F].BoundaryVertexIndexList()
        v = []
        for i in range(len(uintv)):
            if topo:
                v.append(int(uintv[i]))
            else:
                v.append(mesh.TopologyVertices.TopologyVertexIndex(int(uintv[i])))

        return v

    def e_all(self, mesh):
        all_e = {}
        hash_set = set()
        counter = 0

        try:
            for i in range(mesh.Ngons.Count):
                n = mesh.Ngons[i].BoundaryVertexCount
                fv = self.face_vertices(mesh, i, True)

                for j in range(n - 1):
                    et = mesh.TopologyEdges.GetEdgeIndex(fv[j], fv[j + 1])
                    if et not in hash_set:
                        hash_set.add(et)
                        all_e[et] = counter
                        counter += 1

                e = mesh.TopologyEdges.GetEdgeIndex(fv[0], fv[n - 1])
                if e not in hash_set:
                    hash_set.add(e)
                    all_e[e] = counter
                    counter += 1
        except Exception as e:
            Rhino.RhinoApp.WriteLine(str(e))
        return all_e

    def get_edges(self):
        if self.has_ngons:
            self.e_ngon_e = self.e_all(self.m)
            self.e = len(self.e_ngon_e)
        else:
            self.e_ngon_e = {}

            for i in range(self.m.TopologyEdges.Count):
                self.e_ngon_e[i] = i
            self.e = len(self.e_ngon_e)

    def _ef(self, mesh, mesh_edge):
        # Get connected mesh faces
        connected_faces = mesh.TopologyEdges.GetConnectedFaces(mesh_edge)
        connected_ngons = []

        for face_index in connected_faces:
            connected_ngons.append(mesh.Ngons.NgonIndexFromFaceIndex(face_index))

        return connected_ngons

    def _EF(self, mesh):
        e_all = self.e_all(mesh)
        ef = [None] * len(e_all)
        for mesh_edge_id in e_all:
            ef[e_all[mesh_edge_id]] = self._ef(mesh, mesh_edge_id)
        return ef

    def get_edge_faces(self):
        if self.has_ngons:
            self.e_f = self._EF(self.m)
        else:
            self.e_f = []
            for i in range(self.m.TopologyEdges.Count):
                self.e_f.append(self.m.TopologyEdges.GetConnectedFaces(i))

    found_ids = []

    def get_edge_vectors(self, lines):

        self.found_ids.clear()
        # Init empty vectors
        self.insertion_vectors = [Vector3d.Zero] * self.e

        # Get edge lines including naked
        self.e_lines = [Line.Unset] * self.e
        for pair in self.e_ngon_e:
            # print(self.e_ngon_e[pair])
            self.e_lines[self.e_ngon_e[pair]] = self.m.TopologyEdges.EdgeLine(pair)
        if len(lines) == 0:
            return
        # Create rtree and add lines
        rTree = Rhino.Geometry.RTree()
        for i in range(len(self.e_lines)):
            bbox = self.e_lines[i].BoundingBox
            bbox.Inflate(1)
            rTree.Insert(bbox, i)

        # Search tree
        for i in range(len(lines)):
            bbox = lines[i].BoundingBox
            bbox.Inflate(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
            bbox.Inflate(1)
            # print(bbox.IsValid)
            rTree.Search(lines[i].BoundingBox, self.BoundingBoxCallback, self.found_ids)

            # Iterate through found ids and check if end point lies on a mesh edge
            for id in self.found_ids:

                if (
                    
                    self.e_lines[id].ClosestPoint(lines[i].From, True).DistanceToSquared(lines[i].From)
                    < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance
                ):
                    self.insertion_vectors[id] = lines[i].Direction
                    self.insertion_vectors[id].Unitize()
                elif (
                    self.e_lines[id].ClosestPoint(lines[i].To, True).DistanceToSquared(lines[i].To)
                    < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance
                ):
                    self.insertion_vectors[id] = lines[i].Direction
                    self.insertion_vectors[id].Unitize()

            self.found_ids = []

    def BoundingBoxCallback(self, sender, e):
        self.found_ids.append(e.Id)

    def constrain(self, x, min_val, max_val):
        return max(min_val, min(x, max_val))

    def interpolate_points(self, from_pt, to_pt, steps, include_ends=True):
        if include_ends:
            point3d_array = [from_pt]
            for i in range(1, steps + 1):
                num = i / (1.0 + steps)
                interpolated_pt = Point3d(
                    from_pt.X + num * (to_pt.X - from_pt.X),
                    from_pt.Y + num * (to_pt.Y - from_pt.Y),
                    from_pt.Z + num * (to_pt.Z - from_pt.Z),
                )
                point3d_array.append(interpolated_pt)
            point3d_array.append(to_pt)
        else:
            point3d_array = []
            for i in range(1, steps + 1):
                num = i / (1.0 + steps)
                interpolated_pt = Point3d(
                    from_pt.X + num * (to_pt.X - from_pt.X),
                    from_pt.Y + num * (to_pt.Y - from_pt.Y),
                    from_pt.Z + num * (to_pt.Z - from_pt.Z),
                )
                point3d_array.append(interpolated_pt)
        return point3d_array

    def change_origin(self, pl, p):
        return Rhino.Geometry.Plane(p, pl.XAxis, pl.YAxis)

    def get_edge_planes(self):
        self.e90_planes = [None] * len(self.e_ngon_e)
        self.e90_multiple_planes = [[] for _ in range(len(self.e_ngon_e))]

        count = 0
        # print(self.e_ngon_e)
        for id in self.e_ngon_e:  # id mesh edge - i ngon edge
            # for i in range(len(self.e_ngon_e)):

            # print(self.e_ngon_e)
            i = self.e_ngon_e[id]

            # print(i)
            # print(id)
            # print(len(self.f_planes))
            if len(self.m.TopologyEdges.GetConnectedFaces(id)) == 1:
                self.e90_planes[i] = Plane.Unset
                self.e90_multiple_planes[i] = [Plane.Unset]
                continue
            # print(id)
            # print(i)
            # print(self.e_f)
            edge_line = self.m.TopologyEdges.EdgeLine(id)
            origin = edge_line.PointAt(0.5)
            zaxis = edge_line.Direction
            zaxis.Unitize()
            pair = self.m.TopologyEdges.GetTopologyVertices(id)
            # edge vertex normals
            yaxis = ((Vector3d(self.m.Normals[pair.I]) + Vector3d(self.m.Normals[pair.J]))) * 0.5
            # or face normals
            # print(len(self.f_planes))
            # print(i)
            # print(self.e_f[i])

            # print(self.e_f[i][0])
            # print(self.e_f[i][1])

            yaxis = (self.f_planes[self.e_f[i][0]].ZAxis + self.f_planes[self.e_f[i][1]].ZAxis) * 0.5

            # self.f_planes[]

            if self.s is not None:
                u, v = self.s.ClosestPoint(origin)
                yaxis = self.s.NormalAt(u, v)

            xaxis = Vector3d.CrossProduct(zaxis, yaxis)

            # orient x-axis towards first edge index
            if (origin + xaxis).DistanceToSquared(self.f_planes[self.e_f[i][0]].Origin) < (
                origin - xaxis
            ).DistanceToSquared(self.f_planes[self.e_f[i][0]].Origin):
                xaxis *= -1

            # Incase insertion vectors are given
            # print(self.insertion_vectors[i])
            if (
                abs(self.insertion_vectors[i].X) + abs(self.insertion_vectors[i].Y) + abs(self.insertion_vectors[i].Z)
                > 0.01
            ):
                # Project given vector to a plane
                edge_plane = Plane(self.e_lines[i].PointAt(0.5), yaxis)
                xform = Transform.PlanarProjection(edge_plane)
                xaxis_ = self.insertion_vectors[i]
                xaxis_.Transform(xform)
                # print (xaxis_)
                if (Point3d.Origin + xaxis_).DistanceToSquared(Point3d.Origin + xaxis) > (
                    Point3d.Origin - xaxis_
                ).DistanceToSquared(Point3d.Origin + xaxis):
                    xaxis_ *= -1
                xaxis = xaxis_

            # set plane
            self.e90_planes[i] = Plane(origin, xaxis, yaxis)
            # 1) By N 2) by division length 3) in both cases consider lists
            # Skip emppty

            self.divisions = 1

            if len(self.edge_division_len) > 0:
                if len(self.edge_division_len) == self.e:
                    self.divisions = int(
                        (
                            self.constrain(
                                edge_line.Length / self.edge_division_len[i],
                                1,
                                10,
                            )
                        )
                    )
                else:
                    self.divisions = int(
                        (
                            self.constrain(
                                edge_line.Length / self.edge_division_len[0],
                                1,
                                10,
                            )
                        )
                    )
            elif len(self.edge_divisions) > 0:
                if len(self.edge_divisions) != self.e:
                    self.divisions = int(self.edge_divisions[0])
                else:
                    self.divisions = int(self.edge_divisions[i])
            pts = self.interpolate_points(edge_line.From, edge_line.To, self.divisions, False)
            #            print(self.divisions)
            #            print(edge_line.Length)

            self.e90_multiple_planes[i] = [self.change_origin(self.e90_planes[i], pt) for pt in pts]
            count = count + 1

    def polyline_from_planes(self, basePlane, sidePlanes, close=True):
        polyline = Rhino.Geometry.Polyline()

        for i in range(len(sidePlanes) - 1):
            result, pt = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(
                basePlane, sidePlanes[i], sidePlanes[i + 1]
            )
            polyline.Add(pt)

        result, pt1 = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(
            basePlane, sidePlanes[len(sidePlanes) - 1], sidePlanes[0]
        )
        polyline.Add(pt1)

        if close:
            polyline.Add(polyline[0])

        return polyline

    def get_connectors(self):
        self.e_polylines = []
        self.e_polylines_planes = []
        self.e_polylines_index = []

        for i in range(len(self.e90_multiple_planes)):
            self.e_polylines.append([None] * (len(self.e90_multiple_planes[i]) * 2))
            self.e_polylines_planes.append([None] * len(self.e90_multiple_planes[i]))
            self.e_polylines_index.append([None] * len(self.e90_multiple_planes[i]))

            for j in range(len(self.e90_multiple_planes[i])):
                if self.e90_multiple_planes[i][j] == Plane.Unset:
                    self.e_polylines[i] = []
                    self.e_polylines_planes[i][j] = None
                    self.e_polylines_index[i][j] = None
                else:
                    if self.rect_height > 0 and self.rect_width > 0:
                        # type0
                        rect0 = Rectangle3d(
                            self.move_plane_by_axis(
                                self.e90_multiple_planes[i][j],
                                self.rect_thickness * 0.5,
                            ),
                            Interval(-self.rect_width * 0.5, self.rect_width * 0.5),
                            Interval(-self.rect_height * 0.5, self.rect_height * 0.5),
                        )
                        rect1 = Rectangle3d(
                            self.move_plane_by_axis(
                                self.e90_multiple_planes[i][j],
                                self.rect_thickness * -0.5,
                            ),
                            Interval(-self.rect_width * 0.5, self.rect_width * 0.5),
                            Interval(-self.rect_height * 0.5, self.rect_height * 0.5),
                        )
                        self.e_polylines[i][j * 2 + 0] = rect0.ToPolyline()
                        self.e_polylines[i][j * 2 + 1] = rect1.ToPolyline()

                    elif self.rect_height <= 0 and self.rect_width <= 0:
                        w = abs(self.rect_height)
                        h = abs(self.rect_width)

                        # type1
                        e_plane = Plane(self.e90_multiple_planes[i][j])
                        e_plane.Rotate(math.pi * 0.5, e_plane.YAxis)
                        top_plane_0 = self.move_plane_by_axis(
                            self.move_plane_by_axis(
                                self.f_planes[self.e_f[i][0]],
                                self.face_positions[-1],
                            ),
                            self.face_thickness * 0.5 + h,
                        )
                        top_plane_1 = self.move_plane_by_axis(
                            self.move_plane_by_axis(
                                self.f_planes[self.e_f[i][1]],
                                self.face_positions[-1],
                            ),
                            self.face_thickness * 0.5 + h,
                        )
                        bot_plane_0 = self.move_plane_by_axis(
                            self.move_plane_by_axis(
                                self.f_planes[self.e_f[i][0]],
                                self.face_positions[0],
                            ),
                            self.face_thickness * -0.5 - h,
                        )
                        bot_plane_1 = self.move_plane_by_axis(
                            self.move_plane_by_axis(
                                self.f_planes[self.e_f[i][1]],
                                self.face_positions[0],
                            ),
                            self.face_thickness * -0.5 - h,
                        )

                        e_plane_ = Plane(self.e90_planes[i])
                        e_plane_.Rotate(math.pi * 0.5, e_plane_.YAxis)

                        side_planes = [
                            top_plane_0,
                            e_plane,
                            top_plane_1,
                            self.move_plane_by_axis(e_plane, w * 0.5),
                            bot_plane_1,
                            e_plane,
                            bot_plane_0,
                            self.move_plane_by_axis(e_plane, w * -0.5),
                        ]
                        # Step 1: Create polyline type1_0
                        type1_0 = self.polyline_from_planes(
                            self.move_plane_by_axis(
                                self.e90_multiple_planes[i][j],
                                -self.rect_thickness * 0.5,
                            ),
                            side_planes,
                        )

                        # Step 2: Translate polyline type1_0 by ZAxis * rect_thickness to create type1_1
                        type1_1 = Polyline(type1_0)
                        type1_1.Transform(
                            Transform.Translation(self.e90_multiple_planes[i][j].ZAxis * self.rect_thickness)
                        )

                        # Step 3: Assign type1_0 and type1_1 to corresponding variables in the e_polylines array
                        self.e_polylines[i][j * 2 + 0] = type1_0
                        self.e_polylines[i][j * 2 + 1] = type1_1
                    # Assigning values to e_polylines_planes and e_polylines_index
                    self.e_polylines_planes[i][j] = self.move_plane_by_axis(
                        self.e90_multiple_planes[i][j],
                        self.rect_thickness * 0.5,
                    )
                    self.e_polylines_index[i][j] = "{}-{}_{}".format(self.e_f[i][0], self.e_f[i][1], j)

    def outline_from_face_edge_corner(self, face_plane, edge_planes, bise_planes, T=1, tolerance=0.1):
        polyline = Rhino.Geometry.Polyline()

        if T == 2:
            for i in range(len(edge_planes)):
                plane = edge_planes[i]
                plane1 = edge_planes[(i + 1) % len(edge_planes)]
                if (
                    Rhino.Geometry.Vector3d.VectorAngle(plane.ZAxis, plane1.ZAxis) > -0.01
                    and Rhino.Geometry.Vector3d.VectorAngle(plane.ZAxis, plane1.ZAxis) < 0.01
                ):
                    edge_planes[(i + 1) % len(edge_planes)] = plane
                    continue

                if Rhino.Geometry.Vector3d.VectorAngle(plane.XAxis, plane1.XAxis) < tolerance:
                    vector3d = Rhino.Geometry.Vector3d(plane.XAxis)
                    vector3d.Rotate(math.pi / 2, plane.YAxis)
                    plane1 = Rhino.Geometry.Plane(bise_planes[i].Origin, vector3d, plane.YAxis)

                    # edge_planes[(i + 1) % len(edge_planes)] = plane

                result, line = Rhino.Geometry.Intersect.Intersection.PlanePlane(plane, plane1)
                result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(line, face_plane)
                polyline.Add(line.PointAt(t))

            polyline.Add(polyline[0])
            # Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(polyline)
        else:
            for j in range(len(bise_planes)):
                (
                    result,
                    p,
                ) = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(face_plane, bise_planes[j], edge_planes[j])
                polyline.Add(p)

            polyline.Add(polyline[0])

        return polyline

    def move_plane_by_axis(self, plane, dist, axis=2):
        plane1 = Rhino.Geometry.Plane(plane)
        if axis == 0:
            plane1.Translate(plane1.XAxis * dist)
        elif axis == 1:
            plane1.Translate(plane1.YAxis * dist)
        else:
            plane1.Translate(plane1.Normal * dist)
        return plane1

    def remap_numbers(self, speed=50, low1=0, high1=2000, low2=0, high2=1):
        return low2 + (speed - low1) * (high2 - low2) / (high1 - low1)

    def chamfer(self, polyline, flags, value=0.001):

        if value <= 0:
            return polyline

        p = Rhino.Geometry.Polyline()
        points = Rhino.Geometry.Polyline(polyline.GetRange(0, polyline.Count - 1))
        n = len(p)

        for i in range(n):
            curr = i
            next = (((i + 1) % n) + n) % n
            prev = (((i - 1) % n) + n) % n

            v0 = points[prev] - points[curr]
            v1 = points[next] - points[curr]
            angle = Rhino.Geometry.Vector3d.VectorAngle(v0, v1, Rhino.Geometry.Vector3d.CrossProduct(v0, v1))

            if flags[i] and angle < math.pi * 0.5:
                value_ = self.remap_numbers(angle, 0, math.pi * 0.5, value, value * 0.1)
                v0.Unitize()
                v1.Unitize()
                v0 *= value_
                v1 *= value_
                p.Add(points[curr] + v0)
                p.Add(points[curr] + v1)
            else:
                p.Add(points[curr])
                p.Add(points[curr])

        p.Add(p[0])

        return p

    def get_face_polylines(self, chamfer_dist=0.0):

        self.f_polylines_index = []
        self.f_polylines_planes = []
        self.f_polylines = []

        for i in range(self.f):
            if len(self.face_positions) == 0:
                self.f_polylines.append([None, None])

                self.f_polylines[i][0] = self.outline_from_face_edge_corner(
                    self.move_plane_by_axis(self.f_planes[i], self.face_thickness * -0.5),
                    self.fe_planes[i],
                    self.bi_planes[i],
                    2,
                )
                self.f_polylines[i][1] = self.outline_from_face_edge_corner(
                    self.move_plane_by_axis(self.f_planes[i], self.face_thickness * 0.5),
                    self.fe_planes[i],
                    self.bi_planes[i],
                    2,
                )

                self.f_polylines_planes.append([self.move_plane_by_axis(self.f_planes[i], self.face_thickness * 0.5)])

                self.f_polylines_index.append([str(i)])
            else:
                self.f_polylines.append([None] * (len(self.face_positions) * 2))
                self.f_polylines_planes.append([None] * len(self.face_positions))
                self.f_polylines_index.append([None] * len(self.face_positions))

                for j in range(len(self.face_positions)):
                    pline0 = self.outline_from_face_edge_corner(
                        self.move_plane_by_axis(
                            self.f_planes[i],
                            self.face_positions[j] + self.face_thickness * -0.5,
                        ),
                        self.fe_planes[i],
                        self.bi_planes[i],
                        2,
                    )
                    pline1 = self.outline_from_face_edge_corner(
                        self.move_plane_by_axis(
                            self.f_planes[i],
                            self.face_positions[j] + self.face_thickness * 0.5,
                        ),
                        self.fe_planes[i],
                        self.bi_planes[i],
                        2,
                    )

                    self.f_polylines[i][j * 2 + 0] = pline0
                    self.f_polylines[i][j * 2 + 1] = pline1

                    self.f_polylines_planes[i][j] = self.move_plane_by_axis(
                        self.f_planes[i],
                        self.face_positions[j] + self.face_thickness * 0.5,
                    )
                    self.f_polylines_index[i][j] = str(i) if len(self.face_positions) == 1 else "{0}-{1}".format(i, j)

        return self.f_polylines_index, self.f_polylines_planes, self.f_polylines


def MeshFromPolylines(polylines, weld=1, triangulation=0):

    # Create an empty mesh
    mesh = Mesh()
    v = 0  # Vertex index
    f = 0  # Face index

    for polyline in polylines:
        # Check for valid polylines
        if not polyline.IsValid or polyline is None or polyline.Count < 3:
            continue

        # Duplicate the polyline to work on

        p = Polyline(polyline)

        # Ensure the polyline is closed
        if p[0].DistanceToSquared(p[polyline.Count - 1]) >= 0.001:
            p.Add(p[0])

        # Determine the triangulation mode
        switchVar = triangulation if triangulation < 0 else (polyline.Count - 1) * triangulation

        # Subdivide cases based on triangulation
        if switchVar == 0:  # Fan subdivision
            center = p.CenterPoint()
            p.RemoveAt(p.Count - 1)
            mesh.Vertices.AddVertices(p)
            mesh.Vertices.Add(center)

            for i in range(p.Count):
                mesh.Faces.AddFace(v + i, v + (i + 1) % p.Count, v + p.Count)

            tempV_ = [v + i for i in range(p.Count)]
            tempF_ = [f + i for i in range(p.Count)]

            mesh.Ngons.AddNgon(Rhino.Geometry.MeshNgon.Create(tempV_, tempF_))

            v += p.Count + 1
            f += p.Count

        elif switchVar == 4:  # Quad subdivision
            p.RemoveAt(p.Count - 1)
            mesh.Vertices.AddVertices(p)
            mesh.Faces.AddFace(v + 3, v + 2, v + 1, v + 0)
            mesh.Ngons.AddNgon(Rhino.Geometry.MeshNgon.Create([v, v + 1, v + 2, v + 3], [f]))
            v += 4
            f += 1

        elif switchVar == 3:  # Triangle subdivision
            p.RemoveAt(p.Count - 1)
            mesh.Vertices.AddVertices(p)
            mesh.Faces.AddFace(v + 2, v + 1, v + 0)
            mesh.Ngons.AddNgon(Rhino.Geometry.MeshNgon.Create([v, v + 1, v + 2], [f]))
            v += 3
            f += 1

        else:
            # General case: Mesh creation from closed polyline
            temp_mesh = Rhino.Geometry.Mesh.CreateFromClosedPolyline(p)

            tempF = [f + i for i in range(temp_mesh.Faces.Count)]
            naked_vertices = temp_mesh.GetNakedEdges()

            if naked_vertices:
                tempV = [v + i for i in naked_vertices[0]]

                mesh.Append(temp_mesh)
                mesh.Ngons.AddNgon(Rhino.Geometry.MeshNgon.Create(tempV, tempF))

                v += temp_mesh.Vertices.Count
                f += temp_mesh.Faces.Count

    # Post-process the mesh
    mesh.Compact()
    mesh.Vertices.CombineIdentical(True, True)
    mesh.Vertices.CullUnused()

    # Unify normals and fix orientation
    if mesh.Ngons.Count > 0:

        # NGon boundaries
        boundaries = []
        for i in range(mesh.Ngons.Count):
            boundary = mesh.Ngons[i].BoundaryVertexIndexList()
            boundaries.append(list(boundary))

        # Unify the normals of the mesh
        mesh.UnifyNormals()

        # Get NGon boundaries after unifying normals
        # NGon boundaries
        boundaries2 = []
        for i in range(mesh.Ngons.Count):
            boundary = mesh.Ngons[i].BoundaryVertexIndexList()
            boundaries2.append(list(boundary))

        # Iterate through each NGon boundary
        for i in range(len(boundaries)):
            a = boundaries[i][1]
            b = boundaries2[i][1]

            # If the boundary IDs differ after unifying normals, update the NGon
            if a != b:
                # Convert boundaries and faces to integer lists
                cast_boundaries = [int(x) for x in boundaries[i]]
                cast_faces = [int(x) for x in mesh.Ngons[i].FaceIndexList()]

                # Replace the NGon with the updated version
                ngon = Rhino.Geometry.MeshNgon.Create(cast_boundaries, cast_faces)
                mesh.Ngons[i] = ngon
    else:
        mesh.UnifyNormals()

    mesh.Weld(3.14159265358979)  # Welding tolerance (radians for pi)
    mesh.FaceNormals.ComputeFaceNormals()
    mesh.Normals.ComputeNormals()

    if mesh.SolidOrientation() == -1:
        mesh.Flip(True, True, True)

    if weld > 0:
        aligned_vertex_count = mesh.Vertices.Align(weld, None)
        mesh.Weld(3.14159265358979)  # Welding tolerance (radians for pi)
        # This is where you would weld using RTree, but we are skipping this as per the request.

    return mesh


# General handler for string input
def handle_string_input(option_name: str) -> str:
    go = Rhino.Input.Custom.GetString()
    go.SetCommandPrompt(f"Enter value for {option_name}")
    
    if go.Get() != Rhino.Input.GetResult.String:
        Rhino.RhinoApp.WriteLine("Nothing is selected, returning to main menu.")
        return None

    return go.StringResult()

# General handler for list of numbers input
def handle_numbers_input(option_name: str) -> List[float]:
    go = Rhino.Input.Custom.GetString()
    go.SetCommandPrompt(f"Enter {option_name} as comma-separated values (e.g., 0.0, 1.0, 2.5)")
    
    if go.Get() != Rhino.Input.GetResult.String:
        Rhino.RhinoApp.WriteLine("Nothing is selected, returning to main menu.")
        return []

    input_str = go.StringResult()
    try:
        return [float(val.strip()) for val in input_str.split(",")]
    except ValueError:
        Rhino.RhinoApp.WriteLine("Invalid input. Please enter valid numbers separated by commas.")
        return []

# General handler for list of integers input
def handle_integers_input(option_name: str) -> List[int]:
    go = Rhino.Input.Custom.GetString()
    go.SetCommandPrompt(f"Enter {option_name} as comma-separated integers (e.g., 1, 2, 3)")
    
    if go.Get() != Rhino.Input.GetResult.String:
        Rhino.RhinoApp.WriteLine("Nothing is selected, returning to main menu.")
        return []

    input_str = go.StringResult()
    try:
        return [int(val.strip()) for val in input_str.split(",")]
    except ValueError:
        Rhino.RhinoApp.WriteLine("Invalid input. Please enter valid integers separated by commas.")
        return []

# General handler for polylines input
def handle_polylines_input(option_name: str) -> List[Rhino.Geometry.Polyline]:
    go = Rhino.Input.Custom.GetObject()
    go.SetCommandPrompt(f"Select {option_name}")
    go.GeometryFilter = Rhino.DocObjects.ObjectType.Curve  # Filter to curves
    go.EnablePreSelect(True, True)
    go.SubObjectSelect = False
    go.DeselectAllBeforePostSelect = False
    res = go.GetMultiple(1, 0)

    if go.CommandResult() == Rhino.Commands.Result.Success:
        selected_curves = [go.Object(i).Curve() for i in range(go.ObjectCount) if go.Object(i).Curve()]
        polylines = []
        for curve in selected_curves:
            result, polyline = curve.TryGetPolyline()
            if result:
                polylines.append(polyline)
            else:
                Rhino.RhinoApp.WriteLine("One of the selected curves could not be converted to a polyline.")
        return polylines
    return []

# General handler for lines input
def handle_lines_input(option_name: str) -> List[Rhino.Geometry.Line]:
    go = Rhino.Input.Custom.GetObject()
    go.SetCommandPrompt(f"Select {option_name}")
    go.GeometryFilter = Rhino.DocObjects.ObjectType.Curve  # Filter to curves
    go.EnablePreSelect(True, True)
    go.SubObjectSelect = False
    go.DeselectAllBeforePostSelect = False
    res = go.GetMultiple(1, 0)

    if go.CommandResult() == Rhino.Commands.Result.Success:
        selected_curves = [go.Object(i).Curve() for i in range(go.ObjectCount) if go.Object(i).Curve()]
        lines = [Rhino.Geometry.Line(c.PointAtStart, c.PointAtEnd) for c in selected_curves]
        return lines
    return []

# Main method that processes input types based on the input dictionary
def generalized_input_method(dataset_name: str, input_dict: Dict[str, Tuple[Union[float, int, List[float], List[int], List[Rhino.Geometry.Line], List[Rhino.Geometry.Polyline]], type]], callback = None) -> None:
    get_options = Rhino.Input.Custom.GetOption()
    

    # Dynamically add options based on the input dictionary
    dict_options = {}
    dict_values = {}
    for option_name, (default_value, value_type) in input_dict.items():

        # print(option_name, value_type)
        if value_type is float:
            print(option_name, "float")
            dict_options[option_name] = Rhino.Input.Custom.OptionDouble(default_value)  # float
            dict_values[option_name] = dict_options[option_name].CurrentValue
            get_options.AddOptionDouble(option_name, dict_options[option_name])
        elif value_type is int:
            print(option_name, "int")
            dict_options[option_name] = Rhino.Input.Custom.OptionInteger(default_value)  # int
            dict_values[option_name] = dict_options[option_name].CurrentValue
            get_options.AddOptionInteger(option_name, dict_options[option_name])
        elif value_type is typing.List[float]:  # List of floats
            print(option_name, "list float")
            get_options.AddOption(option_name)
        elif value_type is typing.List[int]:  # List of ints
            print(option_name, "list int")
            get_options.AddOption(option_name)
        elif value_type is typing.List[Rhino.Geometry.Line]:  # List of lines
            print(option_name, "list line")
            get_options.AddOption(option_name)
        elif value_type is typing.List[Rhino.Geometry.Polyline]:  # List of polylines
            print(option_name, "list polyline")
            get_options.AddOption(option_name)

    # Run external method to update geometry each time the input is changed.
    callback(input_dict, dataset_name)


    # Command prompt
    get_options.SetCommandPrompt("Select input method and options.")

    done = False
    while not done:
        # Get the result from the option dialog
        res = get_options.Get()

        # If an option is selected
        if res == Rhino.Input.GetResult.Option:
            option_name = get_options.Option().EnglishName
            input_type = input_dict[option_name][1]

            if input_type is float or input_type is int:
                input_dict[option_name] = (dict_options[option_name].CurrentValue, input_type)
            elif input_type is typing.List[float]:
                result = handle_numbers_input(option_name)
                if result:
                    input_dict[option_name] = (result, input_type)
                    Rhino.RhinoApp.WriteLine(f"Selected numbers for {option_name}: {result}")
            elif input_type is typing.List[int]:
                result = handle_numbers_input(option_name)
                if result:
                    input_dict[option_name] = (result, input_type)
                    Rhino.RhinoApp.WriteLine(f"Selected integers for {option_name}: {result}")
            elif input_type is typing.List[Rhino.Geometry.Line]:  # List of Line
                result = handle_lines_input(option_name)
                if result:
                    input_dict[option_name] = (result, input_type)
                    Rhino.RhinoApp.WriteLine(f"Selected lines for {option_name}: {len(result)} lines selected.")
            elif input_type is typing.List[Rhino.Geometry.Polyline]:  # List of Polyline
                result = handle_polylines_input(option_name)
                if result:
                    input_dict[option_name] = (result, input_type)
                    Rhino.RhinoApp.WriteLine(f"Selected lines for {option_name}: {len(result)} polylines selected.")

            # Run external method to update geometry each time the input is changed.
            callback(input_dict, dataset_name)

        elif res == Rhino.Input.GetResult.Nothing or res == Rhino.Input.GetResult.Cancel:
            Rhino.RhinoApp.WriteLine("No option selected or operation canceled. Exiting...")
            done = True  # Exit the loop by setting done to True

    # Final output and return success
    return Rhino.Commands.Result.Success


def my_callback(name_value_type, dataset_name):


    mesh = wood_rui_globals[dataset_name]["mesh"]
    print()

    if mesh is None or len(name_value_type["polylines"][0]) == 0:
        print("Mesh default dataset is created.")
        mesh = MeshFromPolylines(polylines=default_dataset(), weld=name_value_type["weld_radius"][0])
        add_mesh(mesh, dataset_name)
    else:
        print("Number of polylines: ", len(name_value_type["polylines"][0]))
        mesh = MeshFromPolylines(polylines=name_value_type["polylines"][0], weld=name_value_type["weld_radius"][0])
        add_mesh(mesh, dataset_name)

    if mesh.IsValid is not True:
        print("Mesh is not valid, geometry will not be created.")
        return
    


    

    ###############################################################################
    # user input
    ###############################################################################
    m = wood_rui_globals[dataset_name]["mesh"] # mesh
    face_positions = name_value_type["face_positions"][0] if name_value_type["face_positions"][0] else [0] # offset of panels as a list
    face_thickness = name_value_type["face_thickness"][0] if name_value_type["face_thickness"][0] is not None else 1 # thickness of a panels
    divisions = [name_value_type["divisions"][0]] if name_value_type["divisions"][0] else [2] # number of connector per mesh edge
    print(name_value_type["division_length"][0] )
    division_length = [name_value_type["division_length"][0]] # if the value is 0, the division are used, elsewise this parameter is used for edge length division
    edge_vectors = name_value_type["edge_vectors"][0] if name_value_type["edge_vectors"][0] else [] # list of selected lines
    connector_width = name_value_type["connector_width"][0] if name_value_type["connector_width"][0]  else 10 # width of a connector
    connector_height = name_value_type["connector_height"][0] if name_value_type["connector_height"][0] else 10 # height of a connector
    connector_thickness = name_value_type["connector_thickness"][0] if name_value_type["connector_thickness"][0] else 11 # thickness of a connector
    # chamfer = name_value_type["chamfer"][0] if name_value_type["chamfer"][0] else 0 # thickness of a connector

    ###############################################################################
    # run code
    ###############################################################################

    vda = cross_connectors(
        m,
        None,
        face_thickness,
        face_positions,
        divisions,
        division_length,
        edge_vectors,
        connector_width,
        connector_height,
        connector_thickness,
        chamfer = 0
        # chamfer,
    )

    ###############################################################################
    # user output
    ###############################################################################

    polylines = []
    for i in range(len(vda.f_polylines)):
        polylines.extend(vda.f_polylines[i])

    for i in range(len(vda.e_polylines)):
        if len(vda.e_polylines[i]) > 0:
            polylines.extend(vda.e_polylines[i])
    
    add_polylines(polylines, dataset_name)

    # for i in range(len(vda.e_polylines_planes)):
    #     if vda.e_polylines_planes[i][0] is not None:
    #         data_tree_e_pl.AddRange(vda.e_polylines_planes[i], Grasshopper.Kernel.Data.GH_Path(i))
    # for i in range(len(vda.e_polylines_index)):
    #     if vda.e_polylines_index[i][0] is not None:
    #         data_tree_e_id.AddRange(vda.e_polylines_index[i], Grasshopper.Kernel.Data.GH_Path(i))
    # _f_pl = th.list_to_tree(vda.f_polylines_planes)
    # _f_id = th.list_to_tree(vda.f_polylines_index)


if __name__ == "__main__":
    dataset_name = "cross_connections"
    wood_rui_globals.init_data(dataset_name)

    # Define the input dictionary based on your initial dataset
    input_dict = {
        "polylines": ([], List[Rhino.Geometry.Polyline]),  # Default value for polylines (list of polylines)
        "weld_radius": (1.0, float),                       # Default value for weld radius (float)
        "edge_vectors": ([], List[Rhino.Geometry.Line]),   # Default value for edge vectors (list of lines)
        "face_positions": ([], List[float]),               # Default value for face positions (list of floats)
        "face_thickness": (2.0, float),                    # Default value for face thickness (float)
        "divisions": (2, int),                     # Default value for divisions (list of integers)
        "division_length": (15.0, float),             # Default value for division length (list of floats)
        "connector_width": (10.0, float),                       # Default value for rectangle width (float)
        "connector_height": (10.0, float),                      # Default value for rectangle height (float)
        "connector_thickness": (2.0, float),                   # Default value for rectangle thickness (float)
        # "chamfer": (0, int),                               # Default value for chamfer (int)
    }

    # Call the generalized input method with the dataset name and input dictionary
    generalized_input_method(dataset_name, input_dict, my_callback)

