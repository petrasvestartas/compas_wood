#! python3
# venv: timber_connections
import Rhino
import Rhino.Geometry as rg
import Rhino.Input.Custom as ric
import Rhino.Commands as rc
import System
from typing import List, Optional, Tuple
from typing import *
from wood_rui import wood_rui_globals, add_polylines_dataset, add_mesh, add_insertion_lines, add_adjacency, add_flags


class dataset_folded_plates:
    def __init__(
        self,
        dataset_name: str,
        surface: Rhino.Geometry.Surface,
        u_divisions: int,
        v_divisions: int,
        base_planes: System.Collections.Generic.List[Rhino.Geometry.Plane],
        thickness: float,
        chamfer: float,
        face_positions: System.Collections.Generic.List[float],
    ):

        # Diamond Subdivision
        self._dataset_name = dataset_name  # type str
        self._mesh = Rhino.Geometry.Mesh()  # type Mesh

        self._flags = []  # List<bool>
        self._edge_joints = []  # new bool[][]
        self._adjacency = []  # List<int[]>
        self._surface = None  # Surface
        self._u_divisions = 1  # int
        self._v_divisions = 1  # int
        self._base_planes = []  # List<Plane>
        self._insertion_lines = []  # List<Line>

        # Properties for plates
        self._has_ngons = False  # bool

        self._face_thickness = 1  # double
        self._chamfer = 0.01  # double
        self._extend = 1  # double
        self._face_positions = []  # List<double>

        self._f = -1  # int
        self._e = -1  # int
        self._f_v = []  # int[][]
        self._f_e = []  # int[][]
        self._e_f = []  # int[][]
        self._e_ngon_e = {}  # Dictionary<int, int>
        self._f_planes = []  # Plane[]
        self._fe_planes = []  # Plane[][]
        self._bi_planes = []  # Plane[][]
        self._e_planes = []  # Plane[]
        self._e_lines = []  # Line[]
        self._insertion_vectors = []  # Vector3d[]

        # Output
        self._f_polylines_index = []  # string[][]

        self._f_polylines_planes = []  # Plane[][]
        self._f_polylines = []  # Polyline[][]

        self._surface = surface

        if self._surface is None:
            crv = Rhino.Geometry.Arc(
                Rhino.Geometry.Point3d(-100, 0, 0),
                Rhino.Geometry.Point3d(0, 0, 100),
                Rhino.Geometry.Point3d(100, 0, 0),
            ).ToNurbsCurve()
            self._surface = Rhino.Geometry.Extrusion.Create(crv, 300, False).ToNurbsSurface()
            self._surface = self._surface.Transpose()

        self._u_divisions = u_divisions if u_divisions is not None else 5
        self._v_divisions = v_divisions if v_divisions is not None else 2
        self._base_planes = list(base_planes) if base_planes else []

        # Run
        self.diamond_subdivision()

        self._has_ngons = self._mesh.Ngons.Count > 0
        self._face_thickness = thickness if thickness is not None else 1.4
        self._chamfer = chamfer if chamfer else 0
        self._face_positions = [0] if not face_positions else face_positions

        # Properties for plates
        self.get_face_vertices()
        self.get_faces_planes()
        self.get_face_edge_planes()
        self.get_bisector_planes()
        self.get_edges()
        self.get_edge_faces()
        self.get_face_polylines()
        self.get_insertion_vectors()

        # Transfer attributes from this class to global compas_wood attributes.
        self.create_dataset()

    def diamond_subdivision(self):
        #####################################################################################
        # Main Parameters
        #####################################################################################
        self._mesh = Rhino.Geometry.Mesh()

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
                p4 = self._surface.PointAt(sum_step_u + step_u * 0.5, sum_step_v + step_v * 1.5)
                p5 = self._surface.PointAt(sum_step_u + step_u * 0.5, sum_step_v + step_v * 0.5)
                p6 = self._surface.PointAt(sum_step_u + step_u * 0.5, sum_step_v - step_v * 0.5)

                if j == 0:
                    #####################################################################################
                    # start triangles
                    #####################################################################################
                    p9 = self._surface.PointAt(sum_step_u + step_u * 0.5, interval1.T0)
                    line = Rhino.Geometry.Line(p5, p6)
                    cp = line.ClosestPoint(p9, False)

                    if len(self._base_planes) > 0:
                        if (
                            abs(self._base_planes[0].ZAxis.X)
                            + abs(self._base_planes[0].ZAxis.Y)
                            + abs(self._base_planes[0].ZAxis.Z)
                            > 0.01
                        ):
                            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(line, self._base_planes[0])
                            cp = line.PointAt(t)

                    pts.extend([p5, cp, p1])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                    pts.extend([cp, p5, p0])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                pts.extend([p1, p3, p5])
                self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                v += 3
                pts.extend([p2, p0, p5])
                self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                v += 3

                if j != self._v_divisions - 1:
                    pts.extend([p4, p5, p3])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                    pts.extend([p5, p4, p2])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                else:
                    #####################################################################################
                    # end triangles
                    #####################################################################################
                    p9 = self._surface.PointAt(sum_step_u + step_u * 0.5, interval1.T1)  # Point3d
                    line = Rhino.Geometry.Line(p4, p5)  # Line
                    cp = line.ClosestPoint(p9, False)  # Point3d

                    if len(self._base_planes) > 0:
                        if (
                            abs(self._base_planes[-1].ZAxis.X)
                            + abs(self._base_planes[-1].ZAxis.Y)
                            + abs(self._base_planes[-1].ZAxis.Z)
                            > 0.01
                        ):
                            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(line, self._base_planes[-1])
                            cp = line.PointAt(t)
                    pts.extend([cp, p5, p3])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3

                    pts.extend([p5, cp, p2])
                    self._flags.append(len(face) % 4 == a or len(face) % 4 == b)
                    face.append(Rhino.Geometry.MeshFace(v, v + 1, v + 2))
                    v += 3
                sum_step_v += step_v
            sum_step_u += step_u

        for i in pts:
            self._mesh.Vertices.Add(i)

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
            edge_joints.append([self._flags[i] for i in range(9)])  # Enumerable.Repeat(self._flags[i], 9).ToArray()

    def center_point(self, polyline):
        Count = len(polyline)

        if Count == 0:
            return Rhino.Geometry.Point3d.Unset

        if Count == 1:
            return polyline[0]

        center = Rhino.Geometry.Point3d.Origin
        weight = 0.0

        stop = Count - 1
        if polyline[0].DistanceToSquared(polyline[-1]) > 0.001:
            stop = stop + 1

        for i in range(stop):
            A = Rhino.Geometry.Point3d(polyline[i])
            B = Rhino.Geometry.Point3d(polyline[(i + 1) % Count])
            d = A.DistanceTo(B)
            center = center + d * 0.5 * (A + B)
            weight = weight + d

        center /= weight

        return center

    def mesh_face_center(self, mesh, i):
        center = Rhino.Geometry.Point3d(0, 0, 0)

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
                    Rhino.Geometry.Plane(self.mesh_face_center(self._mesh, i), self._mesh.FaceNormals[i])
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
            zaxis = Rhino.Geometry.Vector3d(0, 0, 1)

            if connectedFaces.Length != 2:  # naked
                zaxis = Rhino.Geometry.Vector3d.CrossProduct(faceNormals[connectedFaces[0]], xaxis)
            else:  # inner
                zaxis_avg = (faceNormals[connectedFaces[0]] + faceNormals[connectedFaces[1]]) / 2
                zaxis_avg.Unitize()
                zaxis = Rhino.Geometry.Vector3d.CrossProduct(xaxis, zaxis_avg)
            self._e_planes.append(Rhino.Geometry.Plane(line.PointAt(0.5), zaxis))

    def dihedral_plane(self, plane0, plane1):
        ####################################################################################################
        # Plane between two Planes
        ####################################################################################################
        result, line = Rhino.Geometry.Intersect.Intersection.PlanePlane(plane0, plane1)
        result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(line, plane0)
        centerDihedral = line.PointAt(t)

        ####################################################################################################
        # Convert ZAxis to lines, cant it be done on origin
        ####################################################################################################
        angleLine0 = Rhino.Geometry.Line(plane0.Origin, plane0.Origin + plane0.ZAxis)
        angleLine1 = Rhino.Geometry.Line(plane1.Origin, plane1.Origin + plane1.ZAxis)

        ####################################################################################################
        # Plane cannot be anti/parallel and they must be apart from each other
        ####################################################################################################
        if (
            plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) == 0
            and plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001
        ):
            ####################################################################################################
            # Intersect two lines to get center
            ####################################################################################################
            result, t0, t1 = Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1)

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
            dihedralPlane = Rhino.Geometry.Plane(centerDihedral, line.Direction, v0 + v1)

            return dihedralPlane
        else:
            return (
                Rhino.Geometry.Plane.Unset
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

                sum = Rhino.Geometry.Vector3d.Zero
                for k in range(len(cf)):
                    sum += Rhino.Geometry.Vector3d(self._mesh.FaceNormals[cf[k]])
                sum = sum / cf.Length

                edgePlane = Rhino.Geometry.Plane.Unset

                if cf.Length == 2:
                    edgePlane = self.dihedral_plane(
                        Rhino.Geometry.Plane(
                            self.mesh_face_center(self._mesh, cf[0]),
                            self._mesh.FaceNormals[cf[0]],
                        ),
                        Rhino.Geometry.Plane(
                            self.mesh_face_center(self._mesh, cf[1]),
                            self._mesh.FaceNormals[cf[1]],
                        ),
                    )
                    edgePlane = self._e_planes[e]
                else:
                    edgePlane = Rhino.Geometry.Plane(
                        (Rhino.Geometry.Point3d)(self._mesh.Vertices[v1] + self._mesh.Vertices[v2]) * 0.5,
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
        self._e = len(self._e_ngon_e)

    def get_edge_faces(self):
        if self._has_ngons:
            pass
            # this.e_f = self._mesh._EF()
        else:
            self._e_f = []
            for i in range(self._mesh.TopologyEdges.Count):
                self._e_f.append(self._mesh.TopologyEdges.GetConnectedFaces(i))

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

    def move_plane_by_axis(self, plane, distance):
        return Rhino.Geometry.Plane(plane.Origin + plane.ZAxis * distance, plane.XAxis, plane.YAxis)

    def get_face_polylines(self):

        self._f_polylines_index = []  # new string[f][]
        self._f_polylines_planes = []  # new Plane[f][]
        self._f_polylines = []  # new Polyline[f][]

        for i in range(self._f):  # (int i = 0  i < f  i++)
            if len(self._face_positions) == 0:
                self._f_polylines.append([])  # new Polyline[2]
                pline0 = self.polyline_from_planes(
                    self.move_plane_by_axis(self._f_planes[i], self._face_thickness * -0.0),
                    self._fe_planes[i],
                    True,
                )
                pline1 = self.polyline_from_planes(
                    self.move_plane_by_axis(self._f_planes[i], self._face_thickness * 1.0),
                    self._fe_planes[i],
                    True,
                )
                self._f_polylines_planes.append(
                    [self.move_plane_by_axis(self._f_planes[i], self._face_thickness * 1.0)]
                )
                self._f_polylines_index.append([str(i)])
            else:
                self._f_polylines.append([])  # new Polyline[self._face_positions.Count * 2]
                self._f_polylines_planes.append([])  # new Plane[self._face_positions.Count]
                self._f_polylines_index.append([])  # new string[self._face_positions.Count]

                for j in range(len(self._face_positions)):  # (int j = 0  j < self._face_positions.Count  j++):
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

                    temp_flag = str(i) if len(self._face_positions) == 1 else str(i) + " " + str(j)
                    self._f_polylines_index[i].append(temp_flag)

    def chamfer_polyline(self, polyline, value=0.001):
        lines = polyline.GetSegments()

        if value == 0:
            return polyline

        p = Rhino.Geometry.Polyline()

        if value < 0:
            for line in lines:
                l_ = Rhino.Geometry.Line(line.From, line.To)

                l_.Extend(-abs(value), -abs(value))
                lShorter = l_
                p.Add(lShorter.From)
                p.Add(lShorter.To)
        else:
            for line in lines:
                p.Add(line.PointAt(value))
                p.Add(line.PointAt(1 - value))

        p.Add(p[0])

        return p

    def get_insertion_vectors(self):
        for i in range(self._f):
            if self._flags[i] is False:
                vec = self._f_polylines[i][0][2] - self._f_polylines[i][0][0]
                vec.Unitize()
                vec *= 10

                for j in range(3):
                    mid = (self._f_polylines[i][0][j] + self._f_polylines[i][0][(j + 1) % 3]) * 0.5
                    self._insertion_lines.append(Rhino.Geometry.Line(mid, vec))

        # chamfer
        if self._chamfer > 0.000001:
            for i in range(self._f):
                for j in range(2):
                    self._f_polylines[i][j] = self.chamfer_polyline(self._f_polylines[i][j], -self._chamfer)

    def create_dataset(self):
        add_mesh(self._mesh, self._dataset_name)
        flat_list_of_polylines = []
        for i in range(0, len(self._f_polylines)):
            for j in range(0, len(self._f_polylines[i])):
                flat_list_of_polylines.append(self._f_polylines[i][j])
        add_polylines_dataset(flat_list_of_polylines, self._dataset_name)
        add_insertion_lines(self._insertion_lines, self._dataset_name)
        add_adjacency(self._adjacency, self._dataset_name)
        add_flags(self._flags, self._dataset_name)
        # Rhino.RhinoDoc.ActiveDoc.Views.Redraw()  # 10-12ms
        Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms


def get_base_planes() -> Tuple[Optional[List[rg.Plane]], rc.Result]:
    base_planes: List[rg.Plane] = []

    # Create a GetObject instance to select multiple curves
    go: ric.GetObject = ric.GetObject()
    go.SetCommandPrompt("Select multiple rectangle curves")
    go.GeometryFilter = Rhino.DocObjects.ObjectType.Curve
    go.EnablePreSelect(False, True)  # Allow multiple selections
    go.SubObjectSelect = False  # Prevent selecting sub-objects
    go.GetMultiple(1, 0)  # Allow selecting multiple objects

    if go.CommandResult() != rc.Result.Success:
        return None, go.CommandResult()

    # Loop through the selected objects
    for i in range(go.ObjectCount):
        curve_ref = go.Object(i)
        curve: rg.Curve = curve_ref.Curve()

        if not curve:
            continue

        # Check if the curve is closed and rectangular-like
        if not curve.IsClosed or not curve.TryGetPlane()[0]:
            print(f"Curve {i} is not closed or planar. Skipping.")
            continue

        # Extract the plane from the rectangular curve
        plane: rg.Plane = curve.TryGetPlane()[1]
        base_planes.append(plane)

    if not base_planes:
        return None, rc.Result.Failure

    return base_planes, rc.Result.Success


def command_line_input(dataset_name) -> rc.Result:
    # Create an instance of GetOption to define options in one line
    get_options: ric.GetOption = ric.GetOption()

    # Define options using OptionInteger and OptionDouble with custom names
    surface_ref: Optional[Rhino.DocObjects.ObjRef] = None
    surface: Optional[rg.Surface] = None
    u_divisions: ric.OptionInteger = ric.OptionInteger(5)  # Default value 5
    v_divisions: ric.OptionInteger = ric.OptionInteger(2)  # Default value 2
    thickness: ric.OptionDouble = ric.OptionDouble(1.4)  # Default value 1.4
    chamfer: ric.OptionDouble = ric.OptionDouble(6)  # Default value 0.0
    face_positions: List[float] = [0.0]  # Default list with a single value 0.0
    base_planes: List[Rhino.Geometry.Plane] = []

    # Add options to the GetOption instance with custom names
    get_options.AddOption("select_surface")  # New option to select surface
    get_options.AddOptionInteger("u_divisions", u_divisions)
    get_options.AddOptionInteger("v_divisions", v_divisions)
    get_options.AddOption("get_planes")  # New option to get base planes
    get_options.AddOptionDouble("thickness", thickness)
    get_options.AddOptionDouble("chamfer", chamfer)

    # Here we add an option for entering face_positions as a comma-separated string
    get_options.AddOption("face_positions")

    Rhino.RhinoApp.WriteLine("\n─ Select a surface and options, if no surface is selected a default one is created. ─")
    get_options.SetCommandPrompt("Select surface and options.")

    def update():
        dataset_folded_plates(
            dataset_name,
            surface,
            u_divisions.CurrentValue,
            v_divisions.CurrentValue,
            base_planes,
            thickness.CurrentValue,
            chamfer.CurrentValue,
            face_positions,  # Pass face_positions to the UI
        )

    update()  # Run once to see the code input

    while True:
        res = get_options.Get()

        # Check if the operation was cancelled
        if res == Rhino.Input.GetResult.Cancel:
            return rc.Result.Cancel  # Exit the loop

        if res == Rhino.Input.GetResult.Option:

            if get_options.OptionIndex() == 1:  # 'select_surface'
                # Step A: Select a surface
                go: ric.GetObject = ric.GetObject()
                go.SetCommandPrompt("surface")
                go.GeometryFilter = Rhino.DocObjects.ObjectType.Surface
                go.EnablePreSelect(True, True)
                go.DeselectAllBeforePostSelect = False
                go.SubObjectSelect = False

                if go.Get() != Rhino.Input.GetResult.Object:
                    return go.CommandResult()

                surface_ref = go.Object(0)
                surface: Optional[rg.Surface] = surface_ref.Surface()

                if not surface:
                    print("No surface selected.")
                    return rc.Result.Failure

                update()
                continue  # Continue to allow further option adjustments

            elif get_options.OptionIndex() == 4:  # 'get_planes'
                print("You have chosen to get base planes.")
                # Call get_base_planes() immediately when option is selected
                base_planes, result = get_base_planes()

                if result != rc.Result.Success:
                    return result

                print("Base planes selected:", len(base_planes) if base_planes else 0)
                update()
                continue

            elif get_options.OptionIndex() == 6:  # 'face_positions'
                # Prompt the user to enter face positions as comma-separated values
                gp = Rhino.Input.Custom.GetString()
                gp.SetCommandPrompt("Enter face positions as comma-separated values (e.g. 0.0, 1.0, 2.5)")
                if gp.Get() != Rhino.Input.GetResult.String:
                    print("No input provided for face positions.")
                    return rc.Result.Failure

                input_str = gp.StringResult()

                try:
                    # Convert the input string to a list of floats
                    face_positions = [float(val.strip()) for val in input_str.split(",")]
                    print("Face positions:", face_positions)
                except ValueError:
                    print("Invalid input. Please enter valid numbers separated by commas.")
                    return rc.Result.Failure

                update()
                continue

            elif res == Rhino.Input.GetResult.Cancel:
                # Output the gathered inputs for verification
                print("└─ surface:", surface)
                print("└─ u_divisions:", u_divisions.CurrentValue)
                print("└─ v_divisions:", v_divisions.CurrentValue)
                print("└─ thickness:", thickness.CurrentValue)
                print("└─ chamfer:", chamfer.CurrentValue)
                print("└─ face_positions:", face_positions)
                print("└─ base_planes:", base_planes)
                return
            elif res == Rhino.Input.GetResult.Nothing:
                return
            else:
                update()

    # Do something with the inputs (you can apply further operations here)
    return rc.Result.Success


if __name__ == "__main__":
    dataset_name = "folded_plates"
    wood_rui_globals.init_data(dataset_name)
    command_line_input(dataset_name)
