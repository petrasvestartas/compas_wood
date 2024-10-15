import Rhino
from wood_rui import (
    wood_rui_globals,
    ensure_layer_exists,
    add_polylines,
    add_mesh,
    add_adjacency,
    add_insertion_vectors,
    add_three_valence,
    add_joint_type,
)
from Rhino.Geometry import Mesh, MeshFace, Point3f, Plane, Vector3d, Point3d, Line
import System
import math
import Rhino.Geometry as rg
import Rhino.Input.Custom as ric
import Rhino.Commands as rc
from typing import *


def import_step_file(step_file_path, new_layer_name):

    # Step 5: Ensure the new layer exists
    layer_index = ensure_layer_exists("compas_wood", "annen", "surface")
    objects_in_layer = [obj for obj in Rhino.RhinoDoc.ActiveDoc.Objects if obj.Attributes.LayerIndex == layer_index]
    if objects_in_layer:
        print(
            f"Layer '{new_layer_name}' is not empty. Skipping import. \nUse surfaces from layer: compas_wood > annen > surface."
        )
        return None

    # Step 1: Get the current object count before importing
    before_import_objects = [obj.Id for obj in Rhino.RhinoDoc.ActiveDoc.Objects]

    # Step 2: Use RhinoCommon to import the .STEP file via command
    import_command = '-_Import "{}" _Enter _Pause'.format(step_file_path)
    Rhino.RhinoApp.RunScript(import_command, False)

    # Step 3: Get the new object count after importing
    after_import_objects = [obj.Id for obj in Rhino.RhinoDoc.ActiveDoc.Objects]

    # Step 4: Identify newly imported objects by comparing the IDs
    new_object_ids = set(after_import_objects) - set(before_import_objects)
    if new_object_ids:
        last_imported_objects = [Rhino.RhinoDoc.ActiveDoc.Objects.FindId(obj_id) for obj_id in new_object_ids]

        # Step 6: Change the layer of each imported object
        for obj in last_imported_objects:
            obj.Attributes.LayerIndex = layer_index
            obj.CommitChanges()
            # Unselect the object explicitly
            obj.Select(False)

        # Ensure that no objects remain selected
        Rhino.RhinoDoc.ActiveDoc.Objects.UnselectAll()
        Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

        print(f"Successfully imported {len(last_imported_objects)} object(s) to layer '{new_layer_name}':")
        for obj in last_imported_objects:
            print(f"Object ID: {obj.Id}, Type: {obj.ObjectType}")

        return last_imported_objects

    else:
        print("No objects were imported.")
        return None


class Chevron:
    # User inputs
    mesh = None
    edge_rotation = 0.0
    edge_offset = 0.0
    box_height = 0.0
    top_plate_inlet = 0.0
    plate_thickness = 0.0
    ortho = False

    # Class generated properties
    f_e = OrderedDict()
    f_rotation_flip = OrderedDict()
    bi_planes = []
    e_planes = []
    f_planes = []

    # Output
    f_v = []
    f_me = []
    plines = []  # pair of outlines
    insertion_vectors = []  # v0,v1,v2,v3,v4,v5
    adjacency = []  # v0,v1, f0, f1
    joints_per_face = []  # 0,0,1,1,2,2
    three_valence = []  # 43, 41, 45, 41

    # Display
    box_insertion_vectors = []  # v0,v1,v2,v3,v4,v5
    box_insertion_lines = []  # v0,v1,v2,v3,v4,v5

    def __init__(
        self, mesh, edge_rotation=1, edge_offset=0.5, box_height=760, top_plate_inlet=80, plate_thickness=40, ortho=True
    ):

        if mesh is None:
            return
        self.mesh = mesh  # assuming DuplicateMesh() is a method in Rhino or some other external library
        self.mesh.Flip(True, True, True)
        self.mesh.RebuildNormals()
        print(mesh.Vertices.Count)
        self.edge_rotation = (
            max(min(edge_rotation, 30), -30) if edge_rotation is not None else 1
        )  # constrain self.edge_rotation to -30 to
        self.edge_offset = (
            max(min(edge_offset, 2), -2) if edge_offset is not None else 0.5
        )  # constrain self.edge_offset to -2 to 2

        self.box_height = (
            max(min(box_height, 100000000), 1) if box_height is not None else 760
        )  # constrain box_height to 1 to infinity

        self.top_plate_inlet = (
            max(min(top_plate_inlet, self.box_height * 0.333), 1) if top_plate_inlet is not None else 80
        )  # constrain top_plate_inlet to 1 to box_height * 0.333
        # print(self.top_plate_inlet)
        self.plate_thickness = (
            max(min(plate_thickness, self.box_height * 0.333), 1) if plate_thickness is not None else 40
        )  # constrain self.plate_thickness to 1 to box_height * 0.333
        # print(self.plate_thickness)
        self.ortho = ortho if ortho is not None else True

        # output
        self.plines = []  # list of Polyline objects
        self.insertion_vectors = [[] for _ in range(self.mesh.Faces.Count * 4)]  # list of Vector3d arrays
        self.adjacency = []  # list of int arrays
        self.joints_per_face = [[] for _ in range(self.mesh.Faces.Count * 4)]  # list of int arrays
        self.three_valence = []  # list of int arrays
        self.f_v = [[] for _ in range(self.mesh.Faces.Count)]  # list of int arrays
        self.f_me = [[] for _ in range(self.mesh.Faces.Count)]  # list of int arrays

        # display
        self.box_insertion_vectors = [None] * self.mesh.Faces.Count  # list of Vector3d objects
        self.box_insertion_lines = [None] * self.mesh.Faces.Count  # list of Line objects

    def get_mesh_face_edges(self, f, flag):
        # edge_id = self.mesh.TopologyEdges.GetEdgesForFace(f)
        ids = [0, 1] if flag else [1, 2]
        return ids

    def stripper(self):
        self.f_e = OrderedDict()
        self.f_rotation_flip = OrderedDict()
        f_e_temp = OrderedDict()
        f_rotation_flip_temp = OrderedDict()
        flagArray = [False] * self.mesh.Faces.Count
        num = 0

        for i in range(self.mesh.Faces.Count):
            if num >= self.mesh.Faces.Count:
                break

            face_1 = 0
            num2 = 0
            while num2 < self.mesh.Faces.Count:
                if flagArray[num2]:
                    num2 += 1
                else:
                    face_1 = num2
                    break

            self.mesh1 = Rhino.Geometry.Mesh()
            self.mesh1.Vertices.AddVertices(self.mesh.Vertices)
            self.mesh1.Faces.AddFace(self.mesh.Faces[face_1])

            f_e_temp[face_1] = self.get_mesh_face_edges(face_1, i % 2 == 0)
            f_rotation_flip_temp[face_1] = i % 2 == 0

            nums = [face_1]
            flagArray[face_1] = True
            num += 1
            flags = [False]
            flag = False

            while not flag:
                flag1 = False
                for j in range(self.mesh1.Faces.Count):
                    if not flags[j]:
                        f_n = self.mesh.Faces.AdjacentFaces(nums[j])
                        for k in range(len(f_n)):
                            if not flagArray[f_n[k]]:
                                f = self.mesh1.Faces[j]
                                a = f.A
                                b = f.B
                                c = f.C
                                d = f.D
                                f = self.mesh.Faces[f_n[k]]
                                a1 = f.A
                                b1 = f.B
                                c1 = f.C
                                d1 = f.D
                                if (
                                    (a == b1 and b == a1)
                                    or (c == d1 and d == c1)
                                    or (c == b1 and d == a1)
                                    or (b == c1 and a == d1)
                                ):
                                    f_e_temp[f_n[k]] = self.get_mesh_face_edges(f_n[k], i % 2 == 0)
                                    f_rotation_flip_temp[f_n[k]] = i % 2 == 0

                                    self.mesh1.Faces.AddFace(self.mesh.Faces[f_n[k]])
                                    nums.append(f_n[k])
                                    flags.append(False)
                                    flagArray[f_n[k]] = True
                                    num += 1
                                    flag1 = True
                        flags[j] = True

                if flag1:
                    continue

                flag = True
                self.mesh1.Vertices.CullUnused()
                # Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(self.mesh1)

            f_e_temp_keys = list(f_e_temp.keys())
            if i % 2 == 0:
                f_e_temp_keys.reverse()

            # print(len(f_e_temp_keys))
            for j in range(len(f_e_temp_keys)):

                self.f_e[f_e_temp_keys[j]] = f_e_temp[f_e_temp_keys[j]]

                self.f_rotation_flip[f_e_temp_keys[j]] = f_rotation_flip_temp[f_e_temp_keys[j]]
                # print(self.f_rotation_flip)

                # if(f_e_temp_keys == 10):

                # print(f_rotation_flip_temp[f_e_temp_keys[j]])

            f_e_temp = OrderedDict()
            f_rotation_flip_temp = OrderedDict()

        # print(self.f_rotation_flip )
        # print(self.f_rotation_flip)

    #        for val in self.f_e:
    #            print(val, self.f_e[val])

    def update_neighbour_plane(self, update_plane, curr_face, curr_edge):

        face_neighbors = self.mesh.TopologyEdges.GetConnectedFaces(curr_edge)
        if len(face_neighbors) == 1:
            return

        if face_neighbors[0] == curr_face:
            face_neighbors = [face_neighbors[1], face_neighbors[0]]

        neighbor_edges = [0, 0, 0, 0]
        for i in range(4):
            neighbor_edges[i] = self.mesh.TopologyEdges.GetEdgeIndex(
                self.f_v[face_neighbors[0]][i], self.f_v[face_neighbors[0]][(i + 1) % 4]
            )

        neighbor_edge = neighbor_edges.index(curr_edge)
        """
        if(curr_face == 139):
            print("Hi", face_neighbors, curr_edge, neighbor_edges)
            print(face_neighbors[0])
            print(curr_face)
            print(self.e_planes[face_neighbors[0]][neighbor_edge])
            Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(self.mesh.TopologyEdges.EdgeLine(curr_edge))
        """
        self.e_planes[face_neighbors[0]][neighbor_edge] = Plane(
            update_plane.Origin, -update_plane.XAxis, update_plane.YAxis
        )

    def get_edge_planes(self):
        try:
            for i in range(len(self.mesh.Faces)):
                self.f_v[i] = [
                    self.mesh.Faces[i][0],
                    self.mesh.Faces[i][1],
                    self.mesh.Faces[i][2],
                    self.mesh.Faces[i][3],
                ]

            self.e_planes = [None] * len(self.f_v)
            self.f_planes = [None] * len(self.f_v)
            self.f_me = [None] * len(self.f_v)

            for i in range(len(self.f_v)):
                n = len(self.f_v[i])
                self.e_planes[i] = [None] * n
                self.f_planes[i] = Plane(self.mesh.Faces.GetFaceCenter(i), Vector3d(self.mesh.FaceNormals[i]))
                self.f_me[i] = [None] * len(self.f_v[i])

                for j in range(len(self.f_v[i])):
                    v1 = self.f_v[i][(j + 0) % n]
                    v2 = self.f_v[i][(j + 1) % n]

                    e = self.mesh.TopologyEdges.GetEdgeIndex(v1, v2)
                    self.f_me[i][j] = e
                    cf = self.mesh.TopologyEdges.GetConnectedFaces(e)

                    sum = Vector3d.Zero
                    for k in range(len(cf)):
                        sum = sum + Vector3d(self.mesh.FaceNormals[cf[k]])
                    sum /= len(cf)

                    edgePlane = Plane(
                        (Point3d)(self.mesh.Vertices[v1] + self.mesh.Vertices[v2]) * 0.5,
                        self.mesh.Vertices[v1] - self.mesh.Vertices[v2],
                        sum,
                    )

                    self.e_planes[i][j] = edgePlane

        except Exception as e:
            print(str(e))

    def orthogonalize_planes(self, plane):

        zaxis_coord_abs = [abs(plane.ZAxis.X), abs(plane.ZAxis.Y), abs(plane.ZAxis.Z)]
        zaxis_coord = [plane.ZAxis.X, plane.ZAxis.Y, plane.ZAxis.Z]
        id = [0, 1, 2]

        zaxis_coord_abs, id = (list(t) for t in zip(*sorted(zip(zaxis_coord_abs, id), key=lambda pair: pair[0])))

        coord = math.copysign(math.ceil(abs(zaxis_coord[id[2]])), zaxis_coord[id[2]])

        if id[2] == 0:
            plane0 = Plane(plane.Origin, Vector3d(coord, 0, 0))
            return plane0
        elif id[2] == 1:
            plane0 = Plane(plane.Origin, Vector3d(0, coord, 0))
            return plane0
        else:
            plane0 = Plane(plane.Origin, Vector3d(0, 0, coord))
            return plane0

    def move_plane_by_axis(self, plane, dist, axis=2):
        plane1 = Rhino.Geometry.Plane(plane)
        if axis == 0:
            plane1.Translate(plane1.XAxis * dist)
        elif axis == 1:
            plane1.Translate(plane1.YAxis * dist)
        else:
            plane1.Translate(plane1.Normal * dist)
        return plane1

    def rotate_planes(self):

        # print(self.f_e)

        for i in range(self.mesh.Faces.Count):
            for j in range(4):
                e = self.mesh.TopologyEdges.GetEdgeIndex(self.f_v[i][j], self.f_v[i][(j + 1) % 4])
                # print("___")

                if len(self.mesh.TopologyEdges.GetConnectedFaces(e)) == 2:
                    sign = -1 if self.f_rotation_flip[i] else 1
                    sign = sign * 1 if j % 2 == 0 else sign * -1
                    sign = 1

                    # if(i == 162):
                    # print(j)

                    if j in self.f_e[i]:

                        if j % 2 == 0 and self.f_rotation_flip[i]:
                            self.e_planes[i][j] = self.move_plane_by_axis(
                                self.e_planes[i][j], self.plate_thickness * self.edge_offset
                            )
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                        if j % 2 == 0 and not self.f_rotation_flip[i]:  # this failing

                            self.e_planes[i][j] = self.move_plane_by_axis(
                                self.e_planes[i][j], self.plate_thickness * self.edge_offset
                            )
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                        if j % 2 == 1 and self.f_rotation_flip[i]:
                            self.e_planes[i][j].Rotate(
                                math.radians(self.edge_rotation * sign * 1), self.e_planes[i][j].YAxis
                            )
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)

                        if j % 2 == 1 and not self.f_rotation_flip[i]:
                            sign *= -1
                            self.e_planes[i][j].Rotate(
                                math.radians(self.edge_rotation * sign * 1), self.e_planes[i][j].YAxis
                            )
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)

                elif self.ortho:
                    self.e_planes[i][j] = self.orthogonalize_planes(self.e_planes[i][j])

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

        for i in range(self.mesh.Faces.Count):
            bi_planes_i = []
            for j in range(4):
                pl1 = self.e_planes[i][j]
                pl0 = self.e_planes[i][(j + 1) % 4]
                bi_planes_i.append(self.dihedral_plane(pl0, pl1))
            self.bi_planes.append(bi_planes_i)

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

    def get_plates(self):
        self.plines = []
        # print("________")
        for f, e in self.f_e.items():
            # print(f)
            # Offset Planes
            self.e_planes_local = []
            for j in range(4):
                pl = (
                    self.e_planes[f][j]
                    if j not in e
                    else self.move_plane_by_axis(self.e_planes[f][j], self.plate_thickness * 1.0)
                )
                self.e_planes_local.append(pl)
                # Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(self.e_planes[f][j].Origin)

            # Get Top Plates
            self.plines.append(
                self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self.f_planes[f], self.box_height * 0.5 - self.top_plate_inlet - self.plate_thickness * 0.5
                    ),
                    self.e_planes_local,
                )
            )
            self.plines.append(
                self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self.f_planes[f], self.box_height * 0.5 - self.top_plate_inlet + self.plate_thickness * 0.5
                    ),
                    self.e_planes_local,
                )
            )
            self.plines.append(
                self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self.f_planes[f], -self.box_height * 0.5 + self.top_plate_inlet - self.plate_thickness * 0.5
                    ),
                    self.e_planes_local,
                )
            )
            self.plines.append(
                self.polyline_from_planes(
                    self.move_plane_by_axis(
                        self.f_planes[f], -self.box_height * 0.5 + self.top_plate_inlet + self.plate_thickness * 0.5
                    ),
                    self.e_planes_local,
                )
            )

            # Get Side Plates
            for j in range(len(e)):
                curr = e[j]
                prev = ((e[j] - 1) + 4) % 4  # MathUtil.Wrap(e[j] - 1, 4)
                next = (e[j] + 1) % 4
                # Side planes
                side_plane0 = self.move_plane_by_axis(self.f_planes[f], self.box_height * 0.5)  # top plane
                side_plane1 = self.e_planes[f][prev] if j == 0 else self.bi_planes[f][prev]
                side_plane2 = self.move_plane_by_axis(self.f_planes[f], -self.box_height * 0.5)  # bottom outline
                side_plane3 = self.e_planes[f][next] if j != 0 else self.bi_planes[f][curr]
                side_planes = [side_plane0, side_plane1, side_plane2, side_plane3]

                # Top and bottom plane plane
                base_plane0 = self.e_planes[f][curr]
                base_plane1 = self.move_plane_by_axis(self.e_planes[f][curr], self.plate_thickness)

                self.plines.append(self.polyline_from_planes(base_plane0, side_planes))
                self.plines.append(self.polyline_from_planes(base_plane1, side_planes))

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

    # output
    #    public List<Polyline> plines;//pair of outlines
    #    public Vector3d[][] insertion_vectors;//v0,v1,v2,v3,v4,v5
    #    public int[][] adjacency;//0,1
    #    public int[][] joints_per_face;//0,0,1,1,2,2
    #    public List<int[]> three_valence;//43, 41, 45, 41
    def get_joinery_solver_output(self):
        # Insertion vectors
        counter = 0

        for o in self.f_e.items():
            f = o[0]
            e = o[1]
            e.sort()
            if e[0] == 0 and e[1] == 3:
                e.reverse()

            # Get vertices to construct vectors
            v = [self.mesh.Faces[f].A, self.mesh.Faces[f].B, self.mesh.Faces[f].C, self.mesh.Faces[f].D]
            v0_0 = v[e[0]]
            v0_1 = v[(e[0] + 1) % 4]
            v1_1 = v[(e[1] + 1) % 4]

            dir0 = self.mesh.Vertices[v0_1] - self.mesh.Vertices[v0_0]
            dir1 = self.mesh.Vertices[v0_1] - self.mesh.Vertices[v1_1]
            dir0.Unitize()
            dir1.Unitize()

            result, l0 = Rhino.Geometry.Intersect.Intersection.PlanePlane(self.f_planes[f], self.bi_planes[f][e[0]])
            bisector_dir0 = l0.To - l0.From
            bisector_dir0.Unitize()

            result, l1 = Rhino.Geometry.Intersect.Intersection.PlanePlane(
                self.f_planes[f], self.bi_planes[f][(e[1] + 1) % 4]
            )
            bisector_dir1 = l1.To - l1.From
            bisector_dir1.Unitize()

            # Assign vectors per box
            self.box_insertion_vectors[counter] = bisector_dir1
            # print(self.box_insertion_lines)
            # print(counter)
            # print(self.plines)
            self.box_insertion_lines[counter] = Line(
                self.center_point(self.plines[counter * 8 + 1]),
                self.center_point(self.plines[counter * 8 + 1]) + bisector_dir1 * 300,
            )

            # Assign vectors per face

            # Top and bottom plate
            insertions = [Vector3d.Zero, Vector3d.Zero, bisector_dir1, bisector_dir1, bisector_dir1, bisector_dir1]
            insertions[2 + ((e[1] + 2) % 4)] = bisector_dir0
            insertions[2 + ((e[1] + 3) % 4)] = bisector_dir0
            self.insertion_vectors[counter * 4 + 0] = insertions
            self.insertion_vectors[counter * 4 + 1] = insertions

            # Side plates
            self.insertion_vectors[counter * 4 + 2] = [
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
            ]
            self.insertion_vectors[counter * 4 + 3] = [
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
                Vector3d.Zero,
            ]
            counter += 1

        # Flatten ID Mapping
        counter = 0
        dict_plate_id_meshface_id = OrderedDict()
        for o in self.f_e.items():
            dict_plate_id_meshface_id[o[0]] = counter
            counter += 1

        ##############################################################################
        # Adjacency
        ##############################################################################

        # Box adjacency
        counter = 0
        for o in self.f_e.items():
            f = o[0]
            e = o[1]

            # Current box
            self.adjacency.append([counter * 4 + 0, counter * 4 + 2, -1, -1])
            self.adjacency.append([counter * 4 + 0, counter * 4 + 3, -1, -1])
            self.adjacency.append([counter * 4 + 1, counter * 4 + 2, -1, -1])
            self.adjacency.append([counter * 4 + 1, counter * 4 + 3, -1, -1])
            self.adjacency.append([counter * 4 + 2, counter * 4 + 3, -1, -1])
            # Neighbors
            m_e_0 = self.f_me[f][e[0]]  # self.mesh.TopologyEdges.GetEdgeIndex(v[e[0]], v[(e[0] + 1) % 4])
            m_e_1 = self.f_me[f][e[1]]  # self.mesh.TopologyEdges.GetEdgeIndex(v[e[1]], v[(e[1] + 1) % 4])

            f0 = self.mesh.TopologyEdges.GetConnectedFaces(m_e_0)
            if len(f0) == 2:
                next_face = f0[0] if f0[0] != f else f0[1]
                self.adjacency.append([dict_plate_id_meshface_id[next_face] * 4 + 0, counter * 4 + 2, -1, -1])
                self.adjacency.append([dict_plate_id_meshface_id[next_face] * 4 + 1, counter * 4 + 2, -1, -1])

            f1 = self.mesh.TopologyEdges.GetConnectedFaces(m_e_1)
            if len(f1) == 2:
                next_face = f1[0] if f1[0] != f else f1[1]
                self.adjacency.append([dict_plate_id_meshface_id[next_face] * 4 + 0, counter * 4 + 3, -1, -1])
                self.adjacency.append([dict_plate_id_meshface_id[next_face] * 4 + 1, counter * 4 + 3, -1, -1])

            counter += 1

        ##############################################################################
        # Three valence
        ##############################################################################
        # Initialize counter variable
        counter = 0

        # Loop through key-value pairs
        for f, e in self.f_e.items():
            # Retrieve topology vertices of current face
            v = [self.mesh.Faces[f].A, self.mesh.Faces[f].B, self.mesh.Faces[f].C, self.mesh.Faces[f].D]

            # Retrieve topology edge indexes
            m_e_0 = self.f_me[f][e[0]]
            m_e_1 = self.f_me[f][e[1]]

            # Retrieve connected faces of topology edges
            e_f_0 = self.mesh.TopologyEdges.GetConnectedFaces(m_e_0)

            # Check if edge m_e_0 is shared by two faces
            if len(e_f_0) == 2:
                # Retrieve index of the other face connected to m_e_0

                f_0 = e_f_0[abs(System.Array.IndexOf(e_f_0, f) - 1)]

                # Retrieve index of neighboring face from dictionary
                nei0 = dict_plate_id_meshface_id[f_0]

                # Add connectivity information to three_valence list
                self.three_valence.append([counter * 4 + 0, counter * 4 + 2, nei0 * 4 + 0, counter * 4 + 2])
                self.three_valence.append([counter * 4 + 1, counter * 4 + 2, nei0 * 4 + 1, counter * 4 + 2])

            # Retrieve connected faces of topology edges
            e_f_1 = self.mesh.TopologyEdges.GetConnectedFaces(m_e_1)

            # Check if edge m_e_1 is shared by two faces
            if len(e_f_1) == 2:
                # Retrieve index of the other face connected to m_e_1
                f_1 = e_f_1[abs(System.Array.IndexOf(e_f_1, f) - 1)]

                # Retrieve index of neighboring face from dictionary
                nei1 = dict_plate_id_meshface_id[f_1]

                # Add connectivity information to three_valence list
                self.three_valence.append([counter * 4 + 0, counter * 4 + 3, nei1 * 4 + 0, counter * 4 + 3])
                self.three_valence.append([counter * 4 + 1, counter * 4 + 3, nei1 * 4 + 1, counter * 4 + 3])

            # Increment counter
            counter += 1

        ##############################################################################
        # Joints per face
        ##############################################################################
        # Initialize counter variable
        counter = 0

        # Loop through key-value pairs
        for f, e in self.f_e.items():
            # top-bottom
            self.joints_per_face[counter * 4 + 0] = [0, 0, 20, 20, 20, 20]
            self.joints_per_face[counter * 4 + 1] = [0, 0, 20, 20, 20, 20]

            # sides
            # set all to 0 except the dovetail
            a = 1 if e[0] == 3 and e[1] == 0 else 0
            b = 0 if e[0] == 3 and e[1] == 0 else 1
            self.joints_per_face[counter * 4 + 2 + a] = [0, 0, 0, 0, 10, 0]
            self.joints_per_face[counter * 4 + 2 + b] = [0, 0, 10, 0, 0, 0]
            counter += 1

    def run(self, dataset_name):
        if self.mesh is None:
            print("Mesh is not given")
            return
        self.stripper()
        self.get_edge_planes()
        self.rotate_planes()
        self.get_bisector_planes()
        self.get_plates()
        self.get_joinery_solver_output()

        add_polylines(self.plines, dataset_name)
        add_insertion_vectors(self.insertion_vectors, dataset_name)
        add_joint_type(self.joints_per_face, dataset_name)
        add_three_valence(self.three_valence, dataset_name)
        add_adjacency(self.adjacency, dataset_name)

        # _data = WoodData(
        #     chevron.plines,
        #     chevron.insertion_vectors,
        #     chevron.joints_per_face,
        #     chevron.three_valence,
        #     self.flatten_list(chevron.adjacency),
        #     planes,
        # )
        # _dir = chevron.box_insertion_lines

    def chevron_grid(self, dataset_name, s, u_divisions=4, v_division_dist=900, shift=0.5, scale=0.05799):
        if s is None:
            return
        # Python code
        # Inputs to construct mesh
        f = []
        v = []

        # Get Surface Interval, divisionStep, baseStep, maxStep, and Delta parameters
        s = s.Transpose()
        DomU = s.Domain(0)
        DomV = s.Domain(1)
        half_v = DomV.T1 * 0.5
        StepU = (DomU.T1 - DomU.T0) / u_divisions
        baseStepV = v_division_dist

        totalV = DomV[1] - DomV[0]

        # Iterate number strips of the NURBS
        ctU = 0
        for j in range(u_divisions):

            ctV = 0
            thresh = totalV / 2
            StepV1 = baseStepV
            b = True
            ListV = []

            p0, p1, p2, p6, p7, p8 = None, None, None, None, None, None
            savept6 = Point3f.Unset
            savept7 = Point3f.Unset
            savept8 = Point3f.Unset
            iterations = 0

            while b and iterations < 1000:
                iterations += 1
                ListV.append(StepV1)

                # Get First Half of a Quads
                if ctV == 0:

                    p0 = s.PointAt(ctU, ctV)

                    p1 = s.PointAt(ctU + StepU * 0.5, ctV)
                    p2 = s.PointAt(ctU + StepU, ctV)
                    p6 = s.PointAt(ctU, ctV + StepV1 * (1 - shift / 2))
                    p7 = s.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + shift / 2))
                    p8 = s.PointAt(ctU + StepU, ctV + StepV1 * (1 - shift / 2))
                    savept6 = p6
                    savept7 = p7
                    savept8 = p8
                else:
                    p0 = savept6
                    p1 = savept7
                    p2 = savept8
                    p6 = s.PointAt(ctU, ctV + StepV1 * (1 - shift / 2))
                    p7 = s.PointAt(ctU + StepU * 0.5, ctV + StepV1 * (1 + shift / 2))
                    p8 = s.PointAt(ctU + StepU, ctV + StepV1 * (1 - shift / 2))
                    savept6 = p6
                    savept7 = p7
                    savept8 = p8

                # Add mesh faces
                f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                v.extend([p0, p6, p7, p1])
                f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                v.extend([p1, p7, p8, p2])

                # Increment V Parameter, and multiply by scale due to curvature change
                ctV += StepV1
                thresh -= StepV1
                StepV1 += StepV1 * scale

                # mirror side
                if ctV + StepV1 > half_v:
                    ListV.append(thresh)
                    ListV.reverse()
                    revCt = totalV / 2  # rev counter

                    for i in range(len(ListV) - 1):
                        revCt += ListV[i]

                        if i == 0:
                            p0 = s.PointAt(ctU, revCt - ListV[i + 1] * shift / 2)
                            # print(ctU, revCt - ListV[i + 1] * shift / 2)
                            p1 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * shift / 2)
                            p2 = s.PointAt(ctU + StepU, revCt - ListV[i + 1] * shift / 2)

                            f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                            v.extend([p6, p0, p1, p7])

                            f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                            v.extend([p7, p1, p2, p8])

                            p6 = s.PointAt(ctU, revCt + ListV[i + 1] * (1 - shift / 2))
                            p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + shift / 2))
                            p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - shift / 2))
                            savept6 = p6
                            savept7 = p7
                            savept8 = p8
                        elif i == (len(ListV) - 2):
                            p0 = savept6
                            p1 = savept7
                            p2 = savept8
                            p6 = s.PointAt(ctU, revCt + ListV[i + 1])
                            p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1])
                            p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1])
                        else:
                            p0 = savept6
                            p1 = savept7
                            p2 = savept8
                            p6 = s.PointAt(ctU, revCt + ListV[i + 1] * (1 - shift / 2))
                            p7 = s.PointAt(ctU + StepU * 0.5, revCt + ListV[i + 1] * (1 + shift / 2))
                            p8 = s.PointAt(ctU + StepU, revCt + ListV[i + 1] * (1 - shift / 2))
                            savept6 = p6
                            savept7 = p7
                            savept8 = p8

                        f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                        v.extend([p1, p7, p8, p2])
                        f.append(MeshFace(len(v), len(v) + 1, len(v) + 2, len(v) + 3))
                        v.extend([p0, p6, p7, p1])

                    b = False

            ctU += StepU

        # Create Mesh
        # print(v)
        mesh = Mesh()
        for point in v:
            mesh.Vertices.Add(point)
        mesh.Faces.AddFaces(f)
        mesh.Weld(100)
        mesh.RebuildNormals()

        add_mesh(mesh, dataset_name)
        # flat_list_of_polylines = []
        # for i in range(0, len(self._f_polylines)):
        #     for j in range(0, len(self._f_polylines[i])):
        #         flat_list_of_polylines.append(self._f_polylines[i][j])
        # add_polylines(flat_list_of_polylines, self._dataset_name)
        # add_insertion_lines(self._insertion_lines, self._dataset_name)
        # add_adjacency(self._adjacency, self._dataset_name)
        # add_flags(self._flags, self._dataset_name)
        # # Rhino.RhinoDoc.ActiveDoc.Views.Redraw()  # 10-12ms
        # Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.Redraw()  # 0 ms
        return mesh


def command_line_input_mesh_subdivision(dataset_name) -> rc.Result:
    # Create an instance of GetOption to define options
    get_options: ric.GetOption = ric.GetOption()

    # Define options with custom default values
    surface_ref: Optional[Rhino.DocObjects.ObjRef] = None
    surface: Optional[rg.Surface] = None
    u_divisions: ric.OptionInteger = ric.OptionInteger(4)  # Default value 4
    v_division_dist: ric.OptionDouble = ric.OptionDouble(900)  # Default value 900
    shift: ric.OptionDouble = ric.OptionDouble(0.5)  # Default value 0.5
    scale: ric.OptionDouble = ric.OptionDouble(0.05799)  # Default value 0.05799

    # Add options for surface and numerical inputs
    get_options.AddOption("select_surface")  # New option to select surface

    Rhino.RhinoApp.WriteLine("\n─ Select a surface before entering other parameters for the mesh. ─")
    get_options.SetCommandPrompt("Sub-menu 1 : mesh.")

    def update():
        chevron = Chevron(None)
        _mesh = chevron.chevron_grid(
            dataset_name,
            surface,
            u_divisions.CurrentValue,
            v_division_dist.CurrentValue,
            shift.CurrentValue,
            scale.CurrentValue,
        )

    while True:
        # Ensure the surface is selected first
        if surface is None:
            Rhino.RhinoApp.WriteLine("Please select a surface before proceeding.")

            # Step A: Select a surface
            go: ric.GetObject = ric.GetObject()
            go.SetCommandPrompt("Select a surface")
            go.GeometryFilter = Rhino.DocObjects.ObjectType.Surface
            go.EnablePreSelect(True, True)
            go.DeselectAllBeforePostSelect = False
            go.SubObjectSelect = False

            if go.Get() != Rhino.Input.GetResult.Object:
                return go.CommandResult()

            surface_ref = go.Object(0)
            surface: Optional[rg.Surface] = surface_ref.Surface()

            if not surface:
                Rhino.RhinoApp.WriteLine("No surface selected.")
                return rc.Result.Failure

            Rhino.RhinoApp.WriteLine("Surface selected. Now input parameters.")
            get_options.ClearCommandOptions()  # Clear surface option

            # Add options for numeric inputs after surface selection
            get_options.AddOptionInteger("u_divisions", u_divisions)
            get_options.AddOptionDouble("v_division_dist", v_division_dist)
            get_options.AddOptionDouble("shift", shift)
            get_options.AddOptionDouble("scale", scale)

            update()  # Run update after surface selection
            continue  # Continue to allow further option adjustments

        # Step B: Handle the remaining numeric input options
        res = get_options.Get()

        # Check if the operation was cancelled
        if res == Rhino.Input.GetResult.Cancel:
            return rc.Result.Cancel  # Exit the loop

        if res == Rhino.Input.GetResult.Option:
            update()  # Update the chevron mesh after any option is adjusted
            continue

        if res == Rhino.Input.GetResult.Nothing:
            return

    # Final output for confirmation and processing
    return rc.Result.Success


def command_line_input_mesh_params(dataset_name) -> rc.Result:
    # Create an instance of GetOption to define options in one line
    get_options: ric.GetOption = ric.GetOption()

    # Define default values for the input parameters
    # mesh: Optional[rg.Mesh] = None
    edge_rotation: ric.OptionInteger = ric.OptionInteger(1)  # Default value 1
    edge_offset: ric.OptionDouble = ric.OptionDouble(0.5)  # Default value 0.5
    box_height: ric.OptionDouble = ric.OptionDouble(760)  # Default value 760
    top_plate_inlet: ric.OptionDouble = ric.OptionDouble(80)  # Default value 80
    plate_thickness: ric.OptionDouble = ric.OptionDouble(40)  # Default value 40
    ortho: ric.OptionToggle = ric.OptionToggle(True, "No", "Yes")  # Default value True

    # Add options to the GetOption instance with custom names
    get_options.AddOptionInteger("edge_rotation", edge_rotation)
    get_options.AddOptionDouble("edge_offset", edge_offset)
    get_options.AddOptionDouble("box_height", box_height)
    get_options.AddOptionDouble("top_plate_inlet", top_plate_inlet)
    get_options.AddOptionDouble("plate_thickness", plate_thickness)
    get_options.AddOptionToggle("ortho", ortho)

    Rhino.RhinoApp.WriteLine("\n─ Select a mesh and input parameters for edge and plate configuration. ─")
    get_options.SetCommandPrompt("Sub-menu 2 : plates.")

    def update():
        chevron_polylines = Chevron(
            wood_rui_globals[dataset_name]["mesh"],
            edge_rotation.CurrentValue,
            edge_offset.CurrentValue,
            box_height.CurrentValue,
            top_plate_inlet.CurrentValue,
            plate_thickness.CurrentValue,
            ortho.CurrentValue,
        )
        chevron_polylines.run(dataset_name)

    update()  # Run once to display the initial values

    while True:
        res = get_options.Get()

        # Check if the operation was cancelled
        if res == Rhino.Input.GetResult.Cancel:
            # Output the gathered inputs for verification
            print("└─ edge_rotation:", edge_rotation.CurrentValue)
            print("└─ edge_offset:", edge_offset.CurrentValue)
            print("└─ box_height:", box_height.CurrentValue)
            print("└─ top_plate_inlet:", top_plate_inlet.CurrentValue)
            print("└─ plate_thickness:", plate_thickness.CurrentValue)
            print("└─ ortho:", ortho.CurrentValue)
            return rc.Result.Cancel  # Exit the loop

        if res == Rhino.Input.GetResult.Option:

            option_index = get_options.Option().Index  # Get the selected option index

        elif res == Rhino.Input.GetResult.Nothing:
            return rc.Result.Nothing

        # Final processing with the inputs
        update()  # Display the final values
    return rc.Result.Success

    # def RunScript(self,
    #         _mesh: Rhino.Geometry.Mesh,
    #         _edge_rotation: float,
    #         _edge_offset: float,
    #         _box_height: float,
    #         _top_plate_inlet: float,
    #         _plate_thickness: float,
    #         _ortho: bool):

    #     ##############################################################################
    #     # Output
    #     ##############################################################################
    #     chevron = Chevron(_mesh, _edge_rotation, _edge_offset, _box_height, _top_plate_inlet, _plate_thickness, _ortho)
    #     chevron.run()

    #     _data = WoodData(
    #         chevron.plines,
    #         chevron.insertion_vectors,
    #         chevron.joints_per_face,
    #         chevron.three_valence,
    #         self.flatten_list(chevron.adjacency),
    #         planes,
    #     )
    #     _dir = chevron.box_insertion_lines


if __name__ == "__main__":

    dataset_name = "annen"
    wood_rui_globals.init_data(dataset_name)

    # ToDo: use uncomment and try the code below to access the shared folder:
    # Step 1: import geometry from step file if layer "surface" is empty
    guid = System.Guid("D0647BA8-EEE5-4C18-AB3E-03A95F119654")
    plugin_file: str = Rhino.PlugIns.PlugIn.PathFromId(guid)
    plugin_path: str = System.IO.Path.GetDirectoryName(plugin_file)
    data_sets: str = System.IO.Path.Combine(plugin_path, "shared")
    step_file_path = System.IO.Path.Combine(data_sets, "annen.stp")
    print(step_file_path)

    # step_file_path = r"C:\brg\2_code\compas_wood\src\rhino\plugin\shared\datasets\annen.stp"
    new_layer_name = "compas_wood::annen::surface"
    imported_objects = import_step_file(step_file_path, new_layer_name)

    # Step 2: Select the surface, provid u_division, v_division, shift and scale.
    # Default values: u_divisions=4, v_division_dist=900, shift=0.5, scale=0.05799
    command_line_input_mesh_subdivision(dataset_name)

    # Step 3: subdivide the geometry into a mesh
    command_line_input_mesh_params(dataset_name)
