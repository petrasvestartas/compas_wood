__author__ = "petras vestartas"
__version__ = "2023.03.22"

"""
Subdivide surface into a diamond pattern
"""

from ghpythonlib.componentbase import executingcomponent as component
import Rhino.Geometry as RhinoGeometry
import math
from ghpythonlib import treehelpers
import Grasshopper

# nested_list = treehelpers.tree_to_list(tree)
# plates = treehelpers.list_to_tree(mtsj._f_polylines)


class case_3_folded_plates:
    # Diamond Subdivision
    _mesh = RhinoGeometry.Mesh()  # type Mesh

    _flags = []  # List<bool>
    _edge_joints = []  # new bool[][]
    _adjacency = []  # List<int[]>
    _surface = None  # Surface
    _u_divisions = 1  # int
    _v_divisions = 1  # int
    _base_planes = []  # List<Plane>
    _insertion_lines = []  # List<Line>

    # Properties for plates
    _has_ngons = False  # bool

    _face_thickness = 1  # double
    _chamfer = 0.01  # double
    _extend = 1  # double
    _face_positions = []  # List<double>

    _f = -1  # int
    _e = -1  # int
    _f_v = []  # int[][]
    _f_e = []  # int[][]
    _e_f = []  # int[][]
    _e_ngon_e = {}  # Dictionary<int, int>
    _f_planes = []  # Plane[]
    _fe_planes = []  # Plane[][]
    _bi_planes = []  # Plane[][]
    _e_planes = []  # Plane[]
    _e_lines = []  # Line[]
    _insertion_vectors = []  # Vector3d[]

    # Output
    _f_polylines_index = []  # string[][]

    _f_polylines_planes = []  # Plane[][]
    _f_polylines = []  # Polyline[][]

    def __init__(
        self,
        surface,
        u_divisions,
        v_divisions,
        base_planes,
        thickness,
        chamfer,
        face_positions,
    ):
        self._surface = surface

        if self._surface == None:
            crv = RhinoGeometry.Arc(
                RhinoGeometry.Point3d(-100, 0, 0),
                RhinoGeometry.Point3d(0, 0, 100),
                RhinoGeometry.Point3d(100, 0, 0),
            ).ToNurbsCurve()
            self._surface = RhinoGeometry.Extrusion.Create(
                crv, 300, False
            ).ToNurbsSurface()
            self._surface = self._surface.Transpose()

        self._u_divisions = u_divisions if u_divisions != None else 5
        self._v_divisions = v_divisions if v_divisions != None else 2
        self._base_planes = base_planes

        # Run
        self.diamond_subdivision()
        self._has_ngons = self._mesh.Ngons.Count > 0
        self._face_thickness = thickness if thickness != None else 1.4
        self._chamfer = chamfer
        self._face_positions = [0] if len(face_positions) == 0 else face_positions

        # Properties for plates
        self.get_face_vertices()
        self.get_faces_planes()
        self.get_face_edge_planes()
        self.get_bisector_planes()
        self.get_edges()
        self.get_edge_faces()
        self.get_face_polylines()
        self.get_insertion_vectors()

    def diamond_subdivision(self):
        #####################################################################################
        # Main Parameters
        #####################################################################################
        m = RhinoGeometry.Mesh()
        flags = []

        if self._u_divisions < 1:
            self._u_divisions = 1

        if self._v_divisions < 1:
            self._v_divisions = 1

        v = 0
        face = []  # List<MeshFace>
        pts = []  # List<Point3d>
        interval = self._surface.Domain(0)  # Interval
        interval1 = self._surface.Domain(1)  # Interval
        step_u = (interval.T1 - interval.T0) / (float)(self._u_divisions)  # double
        step_v = (interval1.T1 - interval1.T0) / (float)(self._v_divisions)  # double

        sum_step_u = 0  # double

        #####################################################################################
        # Subdivision
        #####################################################################################
        for i in range(self._u_divisions):
            a = 1 if i % 2 == 0 else 0  # int variable =  1 if not True else 2
            b = 2 if i % 2 == 0 else 3  # int

            sum_step_v = 0  # double
            for j in range(self._v_divisions):
                p0 = self._surface.PointAt(sum_step_u, sum_step_v)
                p1 = self._surface.PointAt(sum_step_u + step_u, sum_step_v)

                self._surface.PointAt(sum_step_u, sum_step_v + step_v * 0.5)
                self._surface.NormalAt(sum_step_u, sum_step_v + step_v * 0.5)
                self._surface.PointAt(sum_step_u + step_u, sum_step_v + step_v * 0.5)
                self._surface.NormalAt(sum_step_u + step_u, sum_step_v + step_v * 0.5)

                p2 = self._surface.PointAt(sum_step_u, sum_step_v + step_v)
                p3 = self._surface.PointAt(sum_step_u + step_u, sum_step_v + step_v)
                p4 = self._surface.PointAt(
                    sum_step_u + step_u * 0.5, sum_step_v + step_v * 1.5
                )
                p5 = self._surface.PointAt(
                    sum_step_u + step_u * 0.5, sum_step_v + step_v * 0.5
                )
                p6 = self._surface.PointAt(
                    sum_step_u + step_u * 0.5, sum_step_v - step_v * 0.5
                )
                p7 = (p5 + p6) / 2
                p8 = (p4 + p5) / 2

                if j == 0:
                    #####################################################################################
                    ## start triangles
                    #####################################################################################
                    p9 = self._surface.PointAt(sum_step_u + step_u * 0.5, interval1.T0)
                    line = RhinoGeometry.Line(p5, p6)
                    cp = line.ClosestPoint(p9, False)

                    if len(self._base_planes) > 0:
                        if (
                            Math.Abs(self._base_planes[0].ZAxis.X)
                            + Math.Abs(self._base_planes[0].ZAxis.Y)
                            + Math.Abs(self._base_planes[0].ZAxis.Z)
                            > 0.01
                        ):
                            t = Rhino.Geometry.Intersect.Intersection.LinePlane(
                                line, self._base_planes[0]
                            )
                            cp = line.PointAt(t)

                    pts.extend([p5, cp, p1])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                    pts.extend([cp, p5, p0])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                pts.extend([p1, p3, p5])
                self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                v += 3
                pts.extend([p2, p0, p5])
                self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                v += 3

                if j != self._v_divisions - 1:
                    pts.extend([p4, p5, p3])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                    pts.extend([p5, p4, p2])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                else:
                    #####################################################################################
                    # end triangles
                    #####################################################################################
                    p9 = self._surface.PointAt(
                        sum_step_u + step_u * 0.5, interval1.T1
                    )  # Point3d
                    line = RhinoGeometry.Line(p4, p5)  # Line
                    cp = line.ClosestPoint(p9, False)  # Point3d

                    if len(self._base_planes) > 0:
                        if (
                            Math.Abs(self._base_planes[-1].ZAxis.X)
                            + Math.Abs(self._base_planes[-1].ZAxis.Y)
                            + Math.Abs(self._base_planes[-1].ZAxis.Z)
                            > 0.01
                        ):
                            t = Rhino.Geometry.Intersect.Intersection.LinePlane(
                                line, self._base_planes[-1]
                            )
                            cp = line.PointAt(t)
                    pts.extend([cp, p5, p3])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                    pts.extend([p5, cp, p2])
                    self._flags.append(face.Count % 4 == a or face.Count % 4 == b)
                    face.append(RhinoGeometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                sum_step_v += step_v
            sum_step_u += step_u

        for i in pts:
            self._mesh.Vertices.Add(i)
        # self._mesh.Vertices.AddVertices(pts)

        self._mesh.Faces.AddFaces(face)
        self._mesh.Weld(3.14159)
        self._mesh.RebuildNormals()

        edge_joints = []  # new bool[face.Count][]
        self._adjacency = []  # = new List<int[]>()

        for i in range(len(self._flags)):
            if self._flags[i]:
                nei = self._mesh.Faces.AdjacentFaces(i)  # int[]

                for j in nei:
                    self._adjacency.append([i, j, -1, -1])
            edge_joints.append(
                [self._flags[i] for i in range(9)]
            )  # Enumerable.Repeat(self._flags[i], 9).ToArray()

    def center_point(self, polyline):
        Count = len(polyline)

        if Count == 0:
            return RhinoGeometry.Point3d.Unset

        if Count == 1:
            return polyline[0]

        center = RhinoGeometry.Point3d.Origin
        weight = 0.0

        stop = Count - 1
        if polyline[0].DistanceToSquared(polyline[-1]) > 0.001:
            stop = stop + 1

        for i in range(stop):
            A = RhinoGeometry.Point3d(polyline[i])
            B = RhinoGeometry.Point3d(polyline[(i + 1) % Count])
            d = A.DistanceTo(B)
            center = center + d * 0.5 * (A + B)
            weight = weight + d

        center /= weight

        return center

    def mesh_face_center(self, mesh, i):
        center = RhinoGeometry.Point3d(0, 0, 0)

        if mesh.Faces[i].IsQuad:
            pts = [
                mesh.Vertices[mesh.Faces[i].A],
                mesh.Vertices[mesh.Faces[i].B],
                mesh.Vertices[mesh.Faces[i].C],
                mesh.Vertices[mesh.Faces[i].D],
            ]
            center = self.center_point(pts)
        else:
            pts = [
                mesh.Vertices[mesh.Faces[i].A],
                mesh.Vertices[mesh.Faces[i].B],
                mesh.Vertices[mesh.Faces[i].C],
            ]
            center = self.center_point(pts)

        return center

    def get_face_vertices(self):
        if self._has_ngons:
            self._f = self._mesh.Ngons.Count
            # f_v = self._mesh.GetNGonsTopoBoundaries()
        else:
            self._f = self._mesh.Faces.Count

            empty_list = []
            self._f_v = []
            for i in range(self._mesh.Faces.Count):
                self._f_v.append([])

            # f_v = [] #new int[this.m.Faces.Count][]
            for i in range(self._mesh.Faces.Count):
                self._f_v[i] = (
                    [
                        self._mesh.Faces[i][0],
                        self._mesh.Faces[i][1],
                        self._mesh.Faces[i][2],
                        self._mesh.Faces[i][3],
                    ]
                    if self._mesh.Faces[i].IsQuad
                    else [
                        self._mesh.Faces[i][0],
                        self._mesh.Faces[i][1],
                        self._mesh.Faces[i][2],
                    ]
                )

    def get_faces_planes(self):
        if self._has_ngons:
            pass
            # f_planes = self._mesh._FPlanes()
        else:
            self._f_planes = []  # new Plane[m.Faces.Count]
            for i in range(self._mesh.Faces.Count):
                self._f_planes.append(
                    RhinoGeometry.Plane(
                        self.mesh_face_center(self._mesh, i), self._mesh.FaceNormals[i]
                    )
                )

    def set_edge_normals(self):
        topologyEdges = self._mesh.TopologyEdges
        faceNormals = self._mesh.FaceNormals

        self.e_planes = []  # new Plane[topologyEdges.Count]

        for i in range(topologyEdges.Count):
            line = topologyEdges.EdgeLine(i)
            connectedFaces = topologyEdges.GetConnectedFaces(i)  # int[]

            xaxis = line.To - line.From
            xaxis.Unitize()
            zaxis = RhinoGeometry.Vector3d(0, 0, 1)

            if connectedFaces.Length != 2:  # naked
                zaxis = RhinoGeometry.Vector3d.CrossProduct(
                    faceNormals[connectedFaces[0]], xaxis
                )
            else:  # inner
                zaxis_avg = (
                    faceNormals[connectedFaces[0]] + faceNormals[connectedFaces[1]]
                ) / 2
                zaxis_avg.Unitize()
                zaxis = RhinoGeometry.Vector3d.CrossProduct(xaxis, zaxis_avg)
            self._e_planes.append(RhinoGeometry.Plane(line.PointAt(0.5), zaxis))

    def dihedral_plane(self, plane0, plane1):
        ####################################################################################################
        # Plane between two Planes
        ####################################################################################################
        result, l = RhinoGeometry.Intersect.Intersection.PlanePlane(plane0, plane1)
        result, t = RhinoGeometry.Intersect.Intersection.LinePlane(l, plane0)
        centerDihedral = l.PointAt(t)

        ####################################################################################################
        # Convert ZAxis to lines, cant it be done on origin
        ####################################################################################################
        angleLine0 = RhinoGeometry.Line(plane0.Origin, plane0.Origin + plane0.ZAxis)
        angleLine1 = RhinoGeometry.Line(plane1.Origin, plane1.Origin + plane1.ZAxis)

        ####################################################################################################
        # Plane cannot be anti/parallel and they must be apart from each other
        ####################################################################################################
        if (
            plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) == 0
            and plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001
        ):
            ##########################################################################################3##########
            # Intersect two lines to get center
            ####################################################################################################
            result, t0, t1 = RhinoGeometry.Intersect.Intersection.LineLine(
                angleLine0, angleLine1
            )

            ####################################################################################################
            # Construct plane from LineLine center and plane origins
            ####################################################################################################
            center = angleLine0.PointAt(t0)
            v0 = plane0.Origin - center
            v1 = plane1.Origin - center
            v0.Unitize()
            v1.Unitize()

            ####################################################################################################
            # Compute Plane
            ####################################################################################################
            dihedralPlane = RhinoGeometry.Plane(centerDihedral, l.Direction, v0 + v1)

            return dihedralPlane
        else:
            return (
                RhinoGeometry.Plane.Unset
            )  # Axes are paralell or anti-parallel |.| or _._ or planes are at the same location

    def get_face_edge_planes(self):
        self.set_edge_normals()
        self._fe_planes = []  # new Plane[f][]

        for i in range(self._f):
            n = len(self._f_v[i])
            self._fe_planes.append([])

            for j in range(len(self._f_v[i])):
                v1 = self._f_v[i][(j + 0) % n]
                v2 = self._f_v[i][(j + 1) % n]

                e = self._mesh.TopologyEdges.GetEdgeIndex(v1, v2)  # int e =
                cf = self._mesh.TopologyEdges.GetConnectedFaces(e)  # int[]

                sum = RhinoGeometry.Vector3d.Zero
                for k in range(len(cf)):
                    sum += RhinoGeometry.Vector3d(self._mesh.FaceNormals[cf[k]])
                sum = sum / cf.Length

                edgePlane = RhinoGeometry.Plane.Unset

                if cf.Length == 2:
                    edgePlane = self.dihedral_plane(
                        RhinoGeometry.Plane(
                            self.mesh_face_center(self._mesh, cf[0]),
                            self._mesh.FaceNormals[cf[0]],
                        ),
                        RhinoGeometry.Plane(
                            self.mesh_face_center(self._mesh, cf[1]),
                            self._mesh.FaceNormals[cf[1]],
                        ),
                    )
                    edgePlane = self._e_planes[e]
                else:
                    edgePlane = RhinoGeometry.Plane(
                        (RhinoGeometry.Point3d)(
                            self._mesh.Vertices[v1] + self._mesh.Vertices[v2]
                        )
                        * 0.5,
                        self._mesh.Vertices[v1] - self._mesh.Vertices[v2],
                        sum,
                    )

                self._fe_planes[i].append(edgePlane)

    def get_bisector_planes(self):
        self._bi_planes = []  # new Plane[f][]

        for i in range(self._f):
            n = len(self._f_v[i])
            self._bi_planes.append([])
            for j in range(n):
                pl1 = self._fe_planes[i][j]
                pl0 = self._fe_planes[i][(j + 1) % n]
                self._bi_planes[-1].append(self.dihedral_plane(pl0, pl1))

    def get_edges(self):
        if self._has_ngons:
            pass
            # e_ngon_e = self._mesh._EAll()
            # this.e = e_ngon_e.Count
        else:
            self._e_ngon_e = {}
            for i in range(self._mesh.TopologyEdges.Count):
                # {'Germany' : 49}
                self._e_ngon_e.update({i: i})
        self._e = self._e_ngon_e.Count

    def get_edge_faces(self):
        if self._has_ngons:
            pass
            # this.e_f = self._mesh._EF()
        else:
            self._e_f = []
            for i in range(self._mesh.TopologyEdges.Count):
                self._e_f.append(self._mesh.TopologyEdges.GetConnectedFaces(i))

    def polyline_from_planes(self, basePlane, sidePlanes, close=True):
        polyline = RhinoGeometry.Polyline()
        for i in range(sidePlanes.Count - 1):
            result, pt = RhinoGeometry.Intersect.Intersection.PlanePlanePlane(
                basePlane, sidePlanes[i], sidePlanes[i + 1]
            )
            polyline.Add(pt)

        result, pt1 = RhinoGeometry.Intersect.Intersection.PlanePlanePlane(
            basePlane, sidePlanes[sidePlanes.Count - 1], sidePlanes[0]
        )
        polyline.Add(pt1)

        if close:
            polyline.Add(polyline[0])

        return polyline

    def move_plane_by_axis(self, plane, distance):
        return RhinoGeometry.Plane(
            plane.Origin + plane.ZAxis * distance, plane.XAxis, plane.YAxis
        )

    def get_face_polylines(self):
        zero_layer = self._face_positions.Count == 0

        self._f_polylines_index = []  # new string[f][]
        self._f_polylines_planes = []  # new Plane[f][]
        self._f_polylines = []  # new Polyline[f][]

        for i in range(self._f):  # (int i = 0  i < f  i++)
            if self._face_positions.Count == 0:
                self._f_polylines.append([])  # new Polyline[2]
                pline0 = self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self._f_planes[i], self._face_thickness * -0.0
                    ),
                    self._fe_planes[i],
                    True,
                )
                pline1 = self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self._f_planes[i], self._face_thickness * 1.0
                    ),
                    self._fe_planes[i],
                    True,
                )
                self._f_polylines_planes.append(
                    [
                        self.move_plane_by_axis(
                            self._f_planes[i], self._face_thickness * 1.0
                        )
                    ]
                )
                self._f_polylines_index.append([str(i)])
            else:
                self._f_polylines.append(
                    []
                )  # new Polyline[self._face_positions.Count * 2]
                self._f_polylines_planes.append(
                    []
                )  # new Plane[self._face_positions.Count]
                self._f_polylines_index.append(
                    []
                )  # new string[self._face_positions.Count]

                for j in range(
                    self._face_positions.Count
                ):  # (int j = 0  j < self._face_positions.Count  j++):
                    pline0 = self.polyline_from_planes(
                        self.move_plane_by_axis(
                            self._f_planes[i],
                            self._face_positions[j] + self._face_thickness * -0.0,
                        ),
                        self._fe_planes[i],
                        True,
                    )
                    pline1 = self.polyline_from_planes(
                        self.move_plane_by_axis(
                            self._f_planes[i],
                            self._face_positions[j] + self._face_thickness * 1.0,
                        ),
                        self._fe_planes[i],
                        True,
                    )
                    self._f_polylines[i].append(pline0)
                    self._f_polylines[i].append(pline1)
                    self._f_polylines_planes[i].append(
                        self.move_plane_by_axis(
                            self._f_planes[i],
                            self._face_positions[j] + self._face_thickness * 1.0,
                        )
                    )

                    temp_flag = (
                        str(i)
                        if len(self._face_positions) == 1
                        else str(i) + " " + str(j)
                    )
                    self._f_polylines_index[i].append(temp_flag)

    def chamfer_polyline(self, polyline, value=0.001):
        lines = polyline.GetSegments()

        if value == 0:
            return polyline

        p = RhinoGeometry.Polyline()

        if value < 0:
            for l in lines:
                l_ = RhinoGeometry.Line(l.From, l.To)

                l_.Extend(-abs(value), -abs(value))
                lShorter = l_
                p.Add(lShorter.From)
                p.Add(lShorter.To)
        else:
            for l in lines:
                p.Add(l.PointAt(value))
                p.Add(l.PointAt(1 - value))

        p.Add(p[0])

        return p

    def get_insertion_vectors(self):
        for i in range(self._f):
            if self._flags[i] == False:
                vec = self._f_polylines[i][0][2] - self._f_polylines[i][0][0]
                vec.Unitize()
                vec *= 10

                for j in range(3):
                    mid = (
                        self._f_polylines[i][0][j]
                        + self._f_polylines[i][0][(j + 1) % 3]
                    ) * 0.5
                    l = RhinoGeometry.Line(mid, vec)
                    self._insertion_lines.append(l)

        # chamfer
        if self._chamfer > 0.000001:
            for i in range(self._f):
                for j in range(2):
                    self._f_polylines[i][j] = self.chamfer_polyline(
                        self._f_polylines[i][j], -self._chamfer
                    )


class case_3_folded_plates_component(component):
    def RunScript(
        self,
        srf,
        u_divisions,
        v_divisions,
        base_planes,
        thickness,
        chamfer,
        face_positions,
    ):
        mtsj = case_3_folded_plates(
            srf,
            u_divisions,
            v_divisions,
            base_planes,
            thickness,
            chamfer,
            face_positions,
        )
        self.mesh = mtsj._mesh
        polylines_tree = Grasshopper.DataTree[RhinoGeometry.Polyline]()
        for i in range(0, len(mtsj._f_polylines)):
            for j in range(0, len(mtsj._f_polylines[i])):
                polylines_tree.Add(
                    mtsj._f_polylines[i][j], Grasshopper.Kernel.Data.GH_Path(i)
                )
        self.plates = polylines_tree
        self.insertion = mtsj._insertion_lines
        self.adjacency = treehelpers.list_to_tree(mtsj._adjacency)
        self.flags = mtsj._flags
        return self.mesh, self.plates, self.insertion, self.adjacency, self.flags
