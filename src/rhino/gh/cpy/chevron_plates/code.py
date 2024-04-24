from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs
import Rhino
import Rhino.Geometry
from Rhino.Geometry import Surface, Mesh, MeshFace, Point3d, Point3f, Vector3d, Plane, Line, BoundingBox
import math
import System
from collections import OrderedDict
import Grasshopper
from System.Drawing import Color


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
    plines = [] # pair of outlines
    insertion_vectors = [] # v0,v1,v2,v3,v4,v5
    adjacency = [] # v0,v1, f0, f1
    joints_per_face = [] # 0,0,1,1,2,2
    three_valence = [] # 43, 41, 45, 41

    # Display
    box_insertion_vectors = [] # v0,v1,v2,v3,v4,v5
    box_insertion_lines = [] # v0,v1,v2,v3,v4,v5
    
    def __init__(self, mesh, edge_rotation=1, edge_offset=0.5, box_height=760, top_plate_inlet=80, plate_thickness=40, ortho=True):

        if mesh == None:
            return
        self.mesh = mesh # assuming DuplicateMesh() is a method in Rhino or some other external library
        self.mesh.Flip(True, True, True)
        self.edge_rotation =  max(min(edge_rotation, 30), -30) if edge_rotation is not None else 1 # constrain self.edge_rotation to -30 to 
        self.edge_offset = max(min(edge_offset, 2), -2) if edge_offset is not None else 0.5 # constrain self.edge_offset to -2 to 2

        self.box_height = max(min(box_height, 100000000), 1) if box_height is not None else 760  # constrain box_height to 1 to infinity
        
        self.top_plate_inlet = max(min(top_plate_inlet, self.box_height * 0.333), 1) if top_plate_inlet is not None else 80 # constrain top_plate_inlet to 1 to box_height * 0.333
        #print(self.top_plate_inlet)
        self.plate_thickness = max(min(plate_thickness, self.box_height * 0.333), 1) if plate_thickness is not None else 40 # constrain self.plate_thickness to 1 to box_height * 0.333
        #print(self.plate_thickness)
        self.ortho = ortho if ortho is not None else True

        # output
        self.plines = [] # list of Polyline objects
        self.insertion_vectors = [[] for _ in range(self.mesh.Faces.Count * 4)] # list of Vector3d arrays
        self.adjacency = [] # list of int arrays
        self.joints_per_face = [[] for _ in range(self.mesh.Faces.Count * 4)] # list of int arrays
        self.three_valence = [] # list of int arrays
        self.f_v = [[] for _ in range(self.mesh.Faces.Count)] # list of int arrays
        self.f_me = [[] for _ in range(self.mesh.Faces.Count)] # list of int arrays

        # display
        self.box_insertion_vectors = [None] * self.mesh.Faces.Count # list of Vector3d objects
        self.box_insertion_lines = [None] * self.mesh.Faces.Count # list of Line objects
    
    def chevron_grid(self, s, u_divisions=4, v_division_dist=900, shift=0.5, scale=0.05799):
        
        # Python code
        # Inputs to construct self.mesh
        f = []
        v = []
        
        # Get Surface Interval, divisionStep, baseStep, maxStep, and Delta parameters
        DomU = s.Domain(0)
        DomV = s.Domain(1)
        half_v = DomV.T1 * 0.5
        StepU = (DomU.T1 - DomU.T0) / u_divisions
        baseStepV = v_division_dist
        maxStepV = 2500
        Delta = maxStepV - baseStepV
        
        totalV = DomV[1] - DomV[0]
        rem = (totalV / 2) - ((maxStepV / 2) + baseStepV + Delta)
        stepV = rem / baseStepV
        
        # Iterate number strips of the NURBS
        ctU = 0
        for j in range(u_divisions):
            ctV = 0
            thresh = totalV / 2
            StepV1 = baseStepV
            b = True
            ListV = []
        
            p0, p1, p2, p6, p7, p8 = None,None,None,None,None,None
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
        
                # Add self.mesh faces
                f.append(MeshFace(len(v), len(v)+ 1, len(v) + 2, len(v) + 3))
                v.extend([p0, p6, p7, p1])
                f.append(MeshFace(len(v), len(v) + 1, len(v)+ 2, len(v) + 3))
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
                            #print(ctU, revCt - ListV[i + 1] * shift / 2)
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
        #print(v)
        self.mesh = Mesh()
        for point in v:
            self.mesh.Vertices.Add(point)
        self.mesh.Faces.AddFaces(f)
        self.mesh.Weld(100)
        self.mesh.RebuildNormals()

        return self.mesh
        
    def get_mesh_face_edges(self, f, flag):
        edge_id = self.mesh.TopologyEdges.GetEdgesForFace(f)
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
            if(num>=self.mesh.Faces.Count):
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
    
            f_e_temp[face_1] = self.get_mesh_face_edges( face_1, i % 2 == 0)
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
                                if (a == b1 and b == a1) or (c == d1 and d == c1) or (c == b1 and d == a1) or (b == c1 and a == d1):
                                    f_e_temp[f_n[k]] = self.get_mesh_face_edges( f_n[k], i % 2 == 0)
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
            
            #print(len(f_e_temp_keys))
            for j in range(len(f_e_temp_keys)):
                
                self.f_e[f_e_temp_keys[j]] = f_e_temp[f_e_temp_keys[j]]
                
                self.f_rotation_flip[f_e_temp_keys[j]] = f_rotation_flip_temp[f_e_temp_keys[j]]
                #print(self.f_rotation_flip)
                
                #if(f_e_temp_keys == 10):
                    
                    #print(f_rotation_flip_temp[f_e_temp_keys[j]])
    
            f_e_temp = OrderedDict()
            f_rotation_flip_temp = OrderedDict()
        
        #print(self.f_rotation_flip )
        #print(self.f_rotation_flip)
#        for val in self.f_e:
#            print(val, self.f_e[val])
    
    def update_neighbour_plane(self, update_plane, curr_face, curr_edge):
        
        
        face_neighbors = self.mesh.TopologyEdges.GetConnectedFaces(curr_edge)
        if len(face_neighbors) == 1:
            return
        
        
        if face_neighbors[0] == curr_face:
            face_neighbors = [face_neighbors[1],face_neighbors[0]]
    
        neighbor_edges = [0, 0, 0, 0]
        for i in range(4):
            neighbor_edges[i] = self.mesh.TopologyEdges.GetEdgeIndex(
                self.f_v[face_neighbors[0]][i],
                self.f_v[face_neighbors[0]][(i + 1) % 4]
            )
    
        neighbor_edge = neighbor_edges.index( curr_edge )
        """
        if(curr_face == 139):
            print("Hi", face_neighbors, curr_edge, neighbor_edges)
            print(face_neighbors[0])
            print(curr_face)
            print(self.e_planes[face_neighbors[0]][neighbor_edge])
            Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(self.mesh.TopologyEdges.EdgeLine(curr_edge))
        """
        self.e_planes[face_neighbors[0]][neighbor_edge] = Plane(
            update_plane.Origin,
            -update_plane.XAxis,
            update_plane.YAxis
        )
        
    
    def get_edge_planes(self):
        try:
            for i in range(len(self.mesh.Faces)):
                self.f_v[i] = [self.mesh.Faces[i][0], self.mesh.Faces[i][1], self.mesh.Faces[i][2], self.mesh.Faces[i][3]]
    
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
                        self.mesh.Vertices[v1] - self.mesh.Vertices[v2], sum)
    
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
        
        #print(self.f_e)
        
        for i in range(self.mesh.Faces.Count):
            for j in range(4):
                e = self.mesh.TopologyEdges.GetEdgeIndex(self.f_v[i][j], self.f_v[i][(j + 1) % 4])
                #print("___")

                if len(self.mesh.TopologyEdges.GetConnectedFaces(e)) == 2:
                    sign = -1 if self.f_rotation_flip[i] else 1
                    sign = sign * 1 if j % 2 == 0 else sign * -1
                    sign = 1
                    
                    #if(i == 162):
                        #print(j)
                    
                    if (j in self.f_e[i]):
                        
                        
                        if j % 2 == 0 and self.f_rotation_flip[i]:
                            self.e_planes[i][j] = self.move_plane_by_axis(self.e_planes[i][j], self.plate_thickness * self.edge_offset)
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                        if j % 2 == 0 and not self.f_rotation_flip[i]: # this failing
                            
                            self.e_planes[i][j] = self.move_plane_by_axis(self.e_planes[i][j], self.plate_thickness * self.edge_offset)
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                        if j % 2 == 1 and self.f_rotation_flip[i]:
                            self.e_planes[i][j].Rotate(math.radians(self.edge_rotation * sign * 1), self.e_planes[i][j].YAxis)
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                            
                        if j % 2 == 1 and not self.f_rotation_flip[i]:
                            sign *= -1
                            self.e_planes[i][j].Rotate(math.radians(self.edge_rotation * sign * 1), self.e_planes[i][j].YAxis)
                            self.update_neighbour_plane(self.e_planes[i][j], i, e)
                            
                elif self.ortho:
                    self.e_planes[i][j] = self.orthogonalize_planes(self.e_planes[i][j])
    
    def to_line(self,  v, p, scale = 1):
        return Line(p, p + v * scale);
    
    def dihedral_plane(self, plane0, plane1):
         # Plane between two Planes
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(plane0, plane1)
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l, plane0)
            centerDihedral = l.PointAt(t)
        
            # Convert ZAxis to lines, cant it be done on origin
            angleLine0 = self.to_line(plane0.ZAxis,plane0.Origin) 
            angleLine1 = self.to_line(plane1.ZAxis,plane1.Origin) 
        
            # Plane cannot be anti/parallel and they must be apart from each other
            if not plane0.ZAxis.IsParallelTo(plane1.ZAxis, 0.01) and plane0.Origin.DistanceToSquared(plane1.Origin) > 0.001:
                # Intersect two lines to get center
                result, t0, t1 = Rhino.Geometry.Intersect.Intersection.LineLine(angleLine0, angleLine1,0.01,False)
                
                # Construct plane from LineLine center and plane origins
                center = angleLine0.PointAt(t0)
                v0 = (plane0.Origin - center)
                v1 = (plane1.Origin - center)
                v0.Unitize()
                v1.Unitize()
                
                # Compute Plane
                dihedralPlane = Rhino.Geometry.Plane(centerDihedral, l.Direction, v0 + v1)
        
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
            result, pt = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(basePlane, sidePlanes[i], sidePlanes[i + 1])
            polyline.Add(pt)
    
        result, pt1 = Rhino.Geometry.Intersect.Intersection.PlanePlanePlane(basePlane, sidePlanes[len(sidePlanes) - 1], sidePlanes[0])
        polyline.Add(pt1)
    
        if close:
            polyline.Add(polyline[0])
    
        return polyline
    
    def get_plates(self):
        self.plines = []
        #print("________")
        for f, e in self.f_e.items():
            #print(f)
            # Offset Planes
            self.e_planes_local = []
            for j in range(4):
                pl = self.e_planes[f][j] if j not in e else self.move_plane_by_axis(self.e_planes[f][j], self.plate_thickness*1.0)
                self.e_planes_local.append(pl)
                #Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(self.e_planes[f][j].Origin)
    
            # Get Top Plates
            self.plines.append(self.polyline_from_planes(self.move_plane_by_axis(self.f_planes[f], self.box_height * 0.5 - self.top_plate_inlet - self.plate_thickness * 0.5), self.e_planes_local))
            self.plines.append(self.polyline_from_planes(self.move_plane_by_axis(self.f_planes[f], self.box_height * 0.5 - self.top_plate_inlet + self.plate_thickness * 0.5), self.e_planes_local))
            self.plines.append(self.polyline_from_planes(self.move_plane_by_axis(self.f_planes[f], -self.box_height * 0.5 + self.top_plate_inlet - self.plate_thickness * 0.5), self.e_planes_local))
            self.plines.append(self.polyline_from_planes(self.move_plane_by_axis(self.f_planes[f], -self.box_height * 0.5 + self.top_plate_inlet + self.plate_thickness * 0.5), self.e_planes_local))
    
            # Get Side Plates
            for j in range(len(e)):
                curr = e[j]
                prev = ((e[j] - 1)+4)%4# MathUtil.Wrap(e[j] - 1, 4)
                next = (e[j] + 1)%4
                # Side planes
                side_plane0 = self.move_plane_by_axis(self.f_planes[f], self.box_height * 0.5) # top plane
                side_plane1 = self.e_planes[f][prev] if j == 0 else self.bi_planes[f][prev]
                side_plane2 = self.move_plane_by_axis(self.f_planes[f], -self.box_height * 0.5) # bottom outline
                side_plane3 = self.e_planes[f][next] if j != 0 else self.bi_planes[f][curr]
                side_planes  = [side_plane0, side_plane1, side_plane2, side_plane3]
    
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
            v1_0 = v[e[1]]
            v1_1 = v[(e[1] + 1) % 4]
        
            dir0 = self.mesh.Vertices[v0_1] - self.mesh.Vertices[v0_0]
            dir1 = self.mesh.Vertices[v0_1] - self.mesh.Vertices[v1_1]
            dir0.Unitize()
            dir1.Unitize()
            
            result, l0 = Rhino.Geometry.Intersect.Intersection.PlanePlane(self.f_planes[f], self.bi_planes[f][e[0]])
            bisector_dir0 = l0.To - l0.From
            bisector_dir0.Unitize()
            
            result, l1 = Rhino.Geometry.Intersect.Intersection.PlanePlane(self.f_planes[f], self.bi_planes[f][(e[1] + 1) % 4])
            bisector_dir1 = l1.To - l1.From
            bisector_dir1.Unitize()
        
            # Assign vectors per box
            self.box_insertion_vectors[counter] = bisector_dir1
            #print(self.box_insertion_lines)
            #print(counter)
            #print(self.plines)
            self.box_insertion_lines[counter] = Line(
                self.center_point(self.plines[counter * 8 + 1]),
                self.center_point(self.plines[counter * 8 + 1]) + bisector_dir1 * 300)
        
            # Assign vectors per face
        
            # Top and bottom plate
            insertions = [Vector3d.Zero, Vector3d.Zero, bisector_dir1, bisector_dir1, bisector_dir1, bisector_dir1]
            insertions[2 + ((e[1] + 2) % 4)] = bisector_dir0
            insertions[2 + ((e[1] + 3) % 4)] = bisector_dir0
            self.insertion_vectors[counter * 4 + 0] = insertions
            self.insertion_vectors[counter * 4 + 1] = insertions
        
            # Side plates
            self.insertion_vectors[counter * 4 + 2] = [Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero]
            self.insertion_vectors[counter * 4 + 3] = [Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero, Vector3d.Zero]
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
        unique_key = set()
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
                
                f_0 = e_f_0[abs(System.Array.IndexOf(e_f_0, f) - 1)];
                
                
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
                f_1 = e_f_1[abs(System.Array.IndexOf(e_f_1, f) - 1)];
                
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
        
    def run(self):
        if self.mesh == None:
            print("Mesh is not given")
            return
        self.stripper()
        self.get_edge_planes()
        self.rotate_planes()
        self.get_bisector_planes()
        self.get_plates()
        self.get_joinery_solver_output()

class MyComponent(component):
    
    
    bbox = BoundingBox.Unset
    lines = []
    insertion_vectors_current = []
    joint_per_face_current_text_entity = []
    polylines = []
    
    def DrawViewportWires(self, args):
        
        is_selected = False
        """
        for _ in GrasshopperDocument.Objects:
            is_selected = _.Attributes.Selected
        """
        #col = args.WireColour_Selected if is_selected else args.WireColour;
        col = args.WireColour
        line_weight = args.DefaultCurveThickness
        try:
            for polyline in self.polylines:
                args.Display.DrawPolyline(polyline,  col, line_weight)
            """
            plane = Plane.WorldXY
            for line in self.lines:
                args.Display.DrawLineArrow(line, Color.FromArgb(207, 0, 90), 2, 100)
            for insertionVectorsCurrent in self.insertion_vectors_current:
                if(insertionVectorsCurrent.Length<0.05):
                    continue
                args.Display.DrawLine(insertionVectorsCurrent, Color.FromArgb(0, 0, 0), 5)
            for jointPerFaceCurrent in self.joint_per_face_current_text_entity:
                result, plane = Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.ActiveViewport.GetCameraFrame()
                plane.Origin=(jointPerFaceCurrent.Plane.Origin)
                args.Display.Draw3dText(jointPerFaceCurrent.PlainText, jointPerFaceCurrent.MaskColor, plane, jointPerFaceCurrent.TextHeight, "Arial", False, False,Rhino.DocObjects.TextHorizontalAlignment.Center, Rhino.DocObjects.TextVerticalAlignment.BottomOfTop)
            """
        except Exception, e:
            System.Windows.Forms.MessageBox.Show(str(e), "script error")
    
    def get_ClippingBox(self):
        return self.bbox
    
    
    def RunScript(self, _mesh, _edge_rotation, _edge_offset, _box_height, _top_plate_inlet, _plate_thickness, _ortho):
        
        # ==============================================================================
        # clear input
        # ==============================================================================
        bbox = Rhino.Geometry.BoundingBox.Unset
        lines = []
        insertion_vectors_current = []
        joint_per_face_current_text_entity = []
        polylines = []
        if(_mesh == None):
            return
        ##############################################################################
        # Output
        ##############################################################################
        chevron = Chevron(_mesh, _edge_rotation, _edge_offset, _box_height, _top_plate_inlet, _plate_thickness, _ortho)
        chevron.run()
        
        self.polylines = chevron.plines
        planes = []
        _plines_partitioned =  [[] for _ in range(int(chevron.plines.Count*0.5))] # list of int arrays
        
        self.bbox = chevron.plines[0].BoundingBox
        
        for i in range(int(chevron.plines.Count*0.5)):
            _plines_partitioned[i] = [chevron.plines[i*2+0], chevron.plines[i*2+1]]
            self.bbox.Union(chevron.plines[i*2+0].BoundingBox)
            plane = Plane(chevron.plines[i*2+0][0],chevron.plines[i*2+0][1]-chevron.plines[i*2+0][0],chevron.plines[i*2+0][1]-chevron.plines[i*2+0][2]);
            planes.append(plane)
        _data = [[_plines_partitioned, chevron.insertion_vectors, chevron.joints_per_face, chevron.three_valence, chevron.adjacency,planes]]
        _dir = chevron.box_insertion_lines
        
        # return outputs if you have them; here I try it for you:
        return (_data, _dir)
