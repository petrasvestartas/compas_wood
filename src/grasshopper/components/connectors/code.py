from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import Rhino.Geometry
import Rhino
import Grasshopper
import ghpythonlib.treehelpers as th
from Rhino.Geometry import (
    Mesh,
    Surface,
    Brep,
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


class MyComponent(component):
    def RunScript(
        self,
        _mesh,
        _face_positions,
        _face_thickness,
        _divisions,
        _division_len,
        _edge_vectors,
        _rect_width,
        _rect_height,
        _rect_thickness,
        _projection,
    ):
        class case_2_vda:
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
                projection_breps,
            ):
                self.m = m

                if self.m == None:
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
                self.face_thickness = (
                    face_thickness if face_thickness is not None else 1
                )
                self.face_positions = (
                    face_positions if face_positions is not None else [0]
                )
                self.face_positions.sort()

                if len(self.face_positions) == 0:
                    self.face_positions = [0]

                self.projection_breps = projection_breps
                self.edge_divisions = (
                    edge_divisions if edge_divisions is not None else [2]
                )
                self.edge_division_len = (
                    edge_division_len if edge_division_len is not None else []
                )
                if len(self.edge_division_len) > 0:
                    if self.edge_division_len[0] < 0.01:
                        self.edge_division_len = []
                self.has_ngons = len(self.m.Ngons) > 0
                self.rect_width = rect_width if rect_width is not None else 10
                self.rect_height = rect_height if rect_height is not None else 10
                self.rect_thickness = (
                    rect_thickness if rect_thickness is not None else 1
                )
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
                    topo_vertices.append(
                        mesh.TopologyVertices.TopologyVertexIndex(vertex_ids[i])
                    )

                return topo_vertices

            def get_ngons_topo_boundaries(self, mesh):
                boundaries = []
                for i in range(mesh.Ngons.Count):
                    boundaries.append(
                        self.vertex_topo_vertex(
                            mesh, mesh.Ngons[i].BoundaryVertexIndexList()
                        )
                    )

                return boundaries

            def get_face_vertices(self):
                if self.has_ngons:
                    self.f = self.m.Ngons.Count
                    self.f_v = self.get_ngons_topo_boundaries(m)
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

            #        print(self.f)
            #        print(self.f_v)

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
                    for i in range(len(f_planes)):
                        u, v = None, None
                        s.ClosestPoint(f_planes[i].Origin, u, v)
                        normal = s.NormalAt(u, v)
                        xaxis = Vector3d.CrossProduct(normal, f_planes[i].YAxis)
                        yaxis = Vector3d.CrossProduct(normal, xaxis)
                        self.f_planes[i] = Plane(f_planes[i].Origin, xaxis, yaxis)

            #        print(self.f_planes)

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
                            s.ClosestPoint(self.fe_planes[i][j].Origin, u, v)
                            normal = s.NormalAt(u, v)
                            self.fe_planes[i][j] = Plane(
                                self.fe_planes[i][j].Origin,
                                Vector3d.CrossProduct(
                                    normal, self.fe_planes[i][j].ZAxis
                                ),
                                normal,
                            )

                        for brep in self.projection_breps:
                            if brep.IsPointInside(
                                self.fe_planes[i][j].Origin, 0.01, False
                            ):
                                xform = Rhino.Geometry.Transform.PlanarProjection(
                                    Plane.WorldXY
                                )
                                self.fe_planes[i][j].Transform(xform)

            #        print(self.fe_planes)

            def to_line(self, v, p, scale=1):
                return Line(p, p + v * scale)

            def dihedral_plane(self, plane0, plane1):
                # Plane between two Planes
                result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(
                    plane0, plane1
                )
                result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l, plane0)
                centerDihedral = l.PointAt(t)

                # Convert ZAxis to lines, cant it be done on origin
                angleLine0 = self.to_line(plane0.ZAxis, plane0.Origin)
                angleLine1 = self.to_line(plane1.ZAxis, plane1.Origin)

                # Plane cannot be anti/parallel and they must be apart from each other
                if (
                    not plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01)
                    and plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001
                ):
                    # Intersect two lines to get center
                    result, t0, t1 = Rhino.Geometry.Intersect.Intersection.LineLine(
                        angleLine0, angleLine1, 0.01, False
                    )

                    # Construct plane from LineLine center and plane origins
                    center = angleLine0.PointAt(t0)
                    v0 = plane0.Origin - center
                    v1 = plane1.Origin - center
                    v0.Unitize()
                    v1.Unitize()

                    # Compute Plane
                    dihedralPlane = Rhino.Geometry.Plane(
                        centerDihedral, l.Direction, v0 + v1
                    )

                    return dihedralPlane
                else:
                    return (
                        Rhino.Geometry.Plane.Unset
                    )  # Axes are parallel or anti-parallel, or planes are at the same location

            def get_bisector_planes(self):
                self.bi_planes = []

                for i in range(self.f):
                    n = len(self.f_v[i])
                    self.bi_planes.append([None] * n)

                    for j in range(n):
                        pl1 = self.fe_planes[i][j]
                        pl0 = self.fe_planes[i][(j + 1) % n]
                        self.bi_planes[i][j] = self.dihedral_plane(pl0, pl1)

            #        print(self.bi_planes)

            def face_vertices(self, mesh, F, topo):
                uintv = mesh.Ngons[F].BoundaryVertexIndexList()
                v = []
                for i in range(len(uintv)):
                    if topo:
                        v.append(int(uintv[i]))
                    else:
                        v.append(
                            mesh.TopologyVertices.TopologyVertexIndex(int(uintv[i]))
                        )

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
                    connected_ngons.append(
                        mesh.Ngons.NgonIndexFromFaceIndex(face_index)
                    )

                return connected_ngons

            def _EF(self, mesh):
                e_all = self.e_all(mesh)
                ef = [None] * len(e_all)
                # print(e_all)
                # count = 0
                for mesh_edge_id in e_all:
                    # print(edge_id)
                    ef[e_all[mesh_edge_id]] = self._ef(mesh, mesh_edge_id)
                    """
                    ef.append(self._ef(mesh, mesh_edge_id))
                    if(count == 198):
                        print(self._ef(mesh, mesh_edge_id))
                    """
                    # count = count + 1
                # print(ef[198])
                return ef

            def get_edge_faces(self):
                if self.has_ngons:
                    self.e_f = self._EF(self.m)
                #            print(len(self.e_f))
                #            print(self.e_f[0])
                else:
                    self.e_f = []
                    for i in range(self.m.TopologyEdges.Count):
                        self.e_f.append(self.m.TopologyEdges.GetConnectedFaces(i))

            found_ids = []

            def get_edge_vectors(self, lines):
                # Init empty vectors
                self.insertion_vectors = [Vector3d.Zero] * self.e

                # Get edge lines including naked
                self.e_lines = [Line.Unset] * self.e
                for pair in self.e_ngon_e:
                    # print(self.e_ngon_e[pair])
                    self.e_lines[self.e_ngon_e[pair]] = self.m.TopologyEdges.EdgeLine(
                        pair
                    )
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
                    rTree.Search(
                        lines[i].BoundingBox, self.BoundingBoxCallback, self.found_ids
                    )

                    # Iterate through found ids and check if end point lies on a mesh edge
                    for id in self.found_ids:
                        # print(id)
                        if (
                            self.e_lines[id]
                            .ClosestPoint(lines[i].From, True)
                            .DistanceToSquared(lines[i].From)
                            < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance
                        ):
                            self.insertion_vectors[id] = lines[i].Direction
                            self.insertion_vectors[id].Unitize()
                        elif (
                            self.e_lines[id]
                            .ClosestPoint(lines[i].To, True)
                            .DistanceToSquared(lines[i].To)
                            < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance
                        ):
                            self.insertion_vectors[id] = lines[i].Direction
                            self.insertion_vectors[id].Unitize()

                    self.found_ids = []

            def BoundingBoxCallback(self, sender, e):
                # print("Hi")
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
                    yaxis = (
                        (
                            Vector3d(self.m.Normals[pair.I])
                            + Vector3d(self.m.Normals[pair.J])
                        )
                    ) * 0.5
                    # or face normals
                    # print(len(self.f_planes))
                    # print(i)
                    # print(self.e_f[i])

                    # print(self.e_f[i][0])
                    # print(self.e_f[i][1])

                    yaxis = (
                        self.f_planes[self.e_f[i][0]].ZAxis
                        + self.f_planes[self.e_f[i][1]].ZAxis
                    ) * 0.5

                    # self.f_planes[]

                    if self.s is not None:
                        u, v = self.s.ClosestPoint(origin)
                        yaxis = self.s.NormalAt(u, v)

                    xaxis = Vector3d.CrossProduct(zaxis, yaxis)

                    # orient x-axis towards first edge index
                    if (origin + xaxis).DistanceToSquared(
                        self.f_planes[self.e_f[i][0]].Origin
                    ) < (origin - xaxis).DistanceToSquared(
                        self.f_planes[self.e_f[i][0]].Origin
                    ):
                        xaxis *= -1

                    # Incase insertion vectors are given
                    # print(self.insertion_vectors[i])
                    if (
                        abs(self.insertion_vectors[i].X)
                        + abs(self.insertion_vectors[i].Y)
                        + abs(self.insertion_vectors[i].Z)
                        > 0.01
                    ):
                        # Project given vector to a plane
                        edge_plane = Plane(self.e_lines[i].PointAt(0.5), yaxis)
                        xform = Transform.PlanarProjection(edge_plane)
                        xaxis_ = self.insertion_vectors[i]
                        xaxis_.Transform(xform)
                        # print (xaxis_)
                        if (Point3d.Origin + xaxis_).DistanceToSquared(
                            Point3d.Origin + xaxis
                        ) > (Point3d.Origin - xaxis_).DistanceToSquared(
                            Point3d.Origin + xaxis
                        ):
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
                    pts = self.interpolate_points(
                        edge_line.From, edge_line.To, self.divisions, False
                    )
                    #            print(self.divisions)
                    #            print(edge_line.Length)

                    self.e90_multiple_planes[i] = [
                        self.change_origin(self.e90_planes[i], pt) for pt in pts
                    ]
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
                    self.e_polylines.append(
                        [None] * (len(self.e90_multiple_planes[i]) * 2)
                    )
                    self.e_polylines_planes.append(
                        [None] * len(self.e90_multiple_planes[i])
                    )
                    self.e_polylines_index.append(
                        [None] * len(self.e90_multiple_planes[i])
                    )

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
                                    Interval(
                                        -self.rect_width * 0.5, self.rect_width * 0.5
                                    ),
                                    Interval(
                                        -self.rect_height * 0.5, self.rect_height * 0.5
                                    ),
                                )
                                rect1 = Rectangle3d(
                                    self.move_plane_by_axis(
                                        self.e90_multiple_planes[i][j],
                                        self.rect_thickness * -0.5,
                                    ),
                                    Interval(
                                        -self.rect_width * 0.5, self.rect_width * 0.5
                                    ),
                                    Interval(
                                        -self.rect_height * 0.5, self.rect_height * 0.5
                                    ),
                                )
                                self.e_polylines[i][j * 2 + 0] = rect0.ToPolyline()
                                self.e_polylines[i][j * 2 + 1] = rect1.ToPolyline()
                            #                        if(len(self.e_f[i])==2):
                            #                            if (self.e_f[i][0] == 99 or self.e_f[i][0] == 89  ) and(self.e_f[i][1] == 99 or self.e_f[i][1] == 89  ):
                            #                                print (self.e90_multiple_planes[i][j])
                            #                                Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(rect0.ToPolyline()[0] )
                            #                                Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(rect0.ToPolyline()[1] )
                            #                                Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(rect0.ToPolyline()[2] )
                            #                                Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(rect0.ToPolyline()[3] )
                            #                                print(rect0.ToPolyline()[0] )
                            #                                print(rect0.IsValid)
                            #                                print(rect0.Width)
                            #                                print(rect0.Height)
                            #                                print(self.move_plane_by_axis(self.e90_multiple_planes[i][j],self.rect_thickness * -0.5))
                            #                                print(self.move_plane_by_axis(self.e90_multiple_planes[i][j],self.rect_thickness * 0.5))
                            #                                print(Interval(-self.rect_width * 0.5, self.rect_width * 0.5))
                            #                                print(Interval(-self.rect_height * 0.5, self.rect_height * 0.5))
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
                                    Transform.Translation(
                                        self.e90_multiple_planes[i][j].ZAxis
                                        * self.rect_thickness
                                    )
                                )

                                # Step 3: Assign type1_0 and type1_1 to corresponding variables in the e_polylines array
                                self.e_polylines[i][j * 2 + 0] = type1_0
                                self.e_polylines[i][j * 2 + 1] = type1_1
                            # Assigning values to e_polylines_planes and e_polylines_index
                            self.e_polylines_planes[i][j] = self.move_plane_by_axis(
                                self.e90_multiple_planes[i][j],
                                self.rect_thickness * 0.5,
                            )
                            self.e_polylines_index[i][j] = "{}-{}_{}".format(
                                self.e_f[i][0], self.e_f[i][1], j
                            )

            def outline_from_face_edge_corner(
                self, face_plane, edge_planes, bise_planes, T=1, tolerance=0.1
            ):
                polyline = Rhino.Geometry.Polyline()

                if T == 2:
                    for i in range(len(edge_planes)):
                        plane = edge_planes[i]
                        plane1 = edge_planes[(i + 1) % len(edge_planes)]
                        if (
                            Rhino.Geometry.Vector3d.VectorAngle(
                                plane.ZAxis, plane1.ZAxis
                            )
                            > -0.01
                            and Rhino.Geometry.Vector3d.VectorAngle(
                                plane.ZAxis, plane1.ZAxis
                            )
                            < 0.01
                        ):
                            edge_planes[(i + 1) % len(edge_planes)] = plane
                            continue

                        if (
                            Rhino.Geometry.Vector3d.VectorAngle(
                                plane.XAxis, plane1.XAxis
                            )
                            < tolerance
                        ):
                            vector3d = Rhino.Geometry.Vector3d(plane.XAxis)
                            vector3d.Rotate(math.pi / 2, plane.YAxis)
                            plane1 = Rhino.Geometry.Plane(
                                bise_planes[i].Origin, vector3d, plane.YAxis
                            )

                            # edge_planes[(i + 1) % len(edge_planes)] = plane

                        result, line = Rhino.Geometry.Intersect.Intersection.PlanePlane(
                            plane, plane1
                        )
                        result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(
                            line, face_plane
                        )
                        polyline.Add(line.PointAt(t))

                    polyline.Add(polyline[0])
                    # Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(polyline)
                else:
                    for j in range(len(bise_planes)):
                        (
                            result,
                            p,
                        ) = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(
                            face_plane, bise_planes[j], edge_planes[j]
                        )
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

            def chamfer(self, polyline, flags, value=0.001):
                lines = polyline.GetSegments()

                if value <= 0:
                    return polyline

                p = Rhino.Geometry.Polyline()
                points = Rhino.Geometry.Polyline(
                    polyline.GetRange(0, polyline.Count - 1)
                )

                for i in range(points.Count):
                    curr = i
                    next = rhino_util.MathUtil.Wrap((i + 1), points.Count)
                    prev = rhino_util.MathUtil.Wrap((i - 1), points.Count)

                    v0 = points[prev] - points[curr]
                    v1 = points[next] - points[curr]
                    angle = Rhino.Geometry.Vector3d.VectorAngle(
                        v0, v1, Rhino.Geometry.Vector3d.CrossProduct(v0, v1)
                    )

                    if flags[i] and angle < math.pi * 0.5:
                        value_ = rhino_util.MathUtil.RemapNumbers(
                            angle, 0, math.pi * 0.5, value, value * 0.1
                        )
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
                zero_layer = len(face_positions) == 0

                self.f_polylines_index = []
                self.f_polylines_planes = []
                self.f_polylines = []

                for i in range(self.f):
                    if len(self.face_positions) == 0:
                        self.f_polylines.append([None, None])

                        self.f_polylines[i][0] = self.outline_from_face_edge_corner(
                            self.move_plane_by_axis(
                                self.f_planes[i], face_thickness * -0.5
                            ),
                            self.fe_planes[i],
                            self.bi_planes[i],
                            2,
                        )
                        self.f_polylines[i][1] = self.outline_from_face_edge_corner(
                            self.move_plane_by_axis(
                                self.f_planes[i], face_thickness * 0.5
                            ),
                            self.fe_planes[i],
                            self.bi_planes[i],
                            2,
                        )

                        self.f_polylines_planes.append(
                            [
                                self.move_plane_by_axis(
                                    self.f_planes[i], self.face_thickness * 0.5
                                )
                            ]
                        )

                        self.f_polylines_index.append([str(i)])
                    else:
                        self.f_polylines.append([None] * (len(face_positions) * 2))
                        self.f_polylines_planes.append([None] * len(face_positions))
                        self.f_polylines_index.append([None] * len(face_positions))

                        for j in range(len(face_positions)):
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
                            # Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(pline0)
                            self.f_polylines_planes[i][j] = self.move_plane_by_axis(
                                self.f_planes[i],
                                self.face_positions[j] + self.face_thickness * 0.5,
                            )
                            self.f_polylines_index[i][j] = (
                                str(i)
                                if len(self.face_positions) == 1
                                else "{0}-{1}".format(i, j)
                            )

                # Chamfer
                """
                for i in range(self.m.Faces.Count):
                    v = [self.m.Faces[i].A, self.m.Faces[i].B, self.m.Faces[i].C, self.m.Faces[i].D] if self.m.Faces[i].IsQuad else [self.m.Faces[i].A, self.m.Faces[i].B, self.m.Faces[i].C]
                    do_chamfer = [False] * len(v)
                    
                    for j in range(len(v)):
                        tv = self.m.TopologyVertices.TopologyVertexIndex(v[j])
                        connected_faces = self.m.TopologyVertices.ConnectedFaces(tv)
                        if len(connected_faces) > 3:
                            do_chamfer[(j - 1) % len(v)] = True
                    print(self.m.Faces.Count)
                    for j in range(len(f_polylines[i])):
                        f_polylines[i][j] = self.chamfer(f_polylines[i][j], do_chamfer, chamfer_dist)
                """
                return self.f_polylines_index, self.f_polylines_planes, self.f_polylines

        ###############################################################################
        # user input
        ###############################################################################

        m = _mesh
        face_positions = _face_positions if len(_face_positions) is not 0 else [0]
        face_thickness = _face_thickness if _face_thickness is not None else 1
        divisions = _divisions if len(_divisions) is not 0 else [2]
        division_len = _division_len if len(_division_len) is not 0 else [15]
        edge_vectors = _edge_vectors if len(_edge_vectors) is not 0 else []
        rect_width = _rect_width if _rect_width is not 0 else 10
        rect_height = _rect_height if _rect_height is not 0 else 10
        rect_thickness = _rect_thickness if _rect_thickness is not 0 else 11
        projection = _projection if len(_projection) else []

        chamfer = 0
        #            DA.GetData(10, ref chamfer);
        #
        #            List<Plane> cut_planes = new List<Plane>();
        #            DA.GetDataList(11, cut_planes);

        ###############################################################################
        # run code
        ###############################################################################
        vda = case_2_vda(
            m,
            None,
            face_thickness,
            face_positions,
            divisions,
            division_len,
            edge_vectors,
            rect_width,
            rect_height,
            rect_thickness,
            chamfer,
            projection,
        )

        """
        if (cut_planes.Count > 0)
        {
            rhino_util.PolylineUtil.CutMesh_FoldedPlates(ref vda.f_polylines, cut_planes);
        }
        """
        ###############################################################################
        # user output
        ###############################################################################

        data_tree_f = Grasshopper.DataTree[Polyline]()
        data_tree_e = Grasshopper.DataTree[Polyline]()
        data_tree_e_pl = Grasshopper.DataTree[Plane]()
        data_tree_e_id = Grasshopper.DataTree[str]()
        for i in range(len(vda.f_polylines)):
            data_tree_f.AddRange(vda.f_polylines[i], Grasshopper.Kernel.Data.GH_Path(i))

        for i in range(len(vda.e_polylines)):
            if len(vda.e_polylines[i]) > 0:
                data_tree_e.AddRange(
                    vda.e_polylines[i], Grasshopper.Kernel.Data.GH_Path(i)
                )
        for i in range(len(vda.e_polylines_planes)):
            if vda.e_polylines_planes[i][0] is not None:
                data_tree_e_pl.AddRange(
                    vda.e_polylines_planes[i], Grasshopper.Kernel.Data.GH_Path(i)
                )
        for i in range(len(vda.e_polylines_index)):
            if vda.e_polylines_index[i][0] is not None:
                data_tree_e_id.AddRange(
                    vda.e_polylines_index[i], Grasshopper.Kernel.Data.GH_Path(i)
                )

        _f = data_tree_f
        _f_pl = th.list_to_tree(vda.f_polylines_planes)
        _f_id = th.list_to_tree(vda.f_polylines_index)
        _e = data_tree_e
        _e_pl = data_tree_e_pl
        _e_id = data_tree_e_id

        # return outputs if you have them; here I try it for you:
        return (_f, _f_pl, _f_id, _e, _e_pl, _e_id)
