from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import rhinoscriptsyntax as rs
import Rhino
import Rhino.Geometry
from Rhino.Geometry import Surface, Mesh, MeshFace, Point3d, Point3f, Vector3d

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
    f_e = {}
    f_rotation_flip = {}
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
        self.mesh = mesh.DuplicateMesh() # assuming DuplicateMesh() is a method in Rhino or some other external library
        self.mesh.Flip(True, True, True)

        self.edge_rotation = max(min(edge_rotation, 30), -30) # constrain edge_rotation to -30 to 30
        self.edge_offset = max(min(edge_offset, 2), -2) # constrain edge_offset to -2 to 2

        self.box_height = max(min(box_height, math.inf), 1) # constrain box_height to 1 to infinity
        self.top_plate_inlet = max(min(top_plate_inlet, box_height * 0.333), 1) # constrain top_plate_inlet to 1 to box_height * 0.333
        self.plate_thickness = max(min(plate_thickness, box_height * 0.333), 1) # constrain plate_thickness to 1 to box_height * 0.333
        self.ortho = ortho

        # output
        self.plines = [] # list of Polyline objects
        self.insertion_vectors = [[] for _ in range(mesh.Faces.Count * 4)] # list of Vector3d arrays
        self.adjacency = [] # list of int arrays
        self.joints_per_face = [[] for _ in range(mesh.Faces.Count * 4)] # list of int arrays
        self.three_valence = [] # list of int arrays
        self.f_v = [[] for _ in range(mesh.Faces.Count)] # list of int arrays
        self.f_me = [[] for _ in range(mesh.Faces.Count)] # list of int arrays

        # display
        self.box_insertion_vectors = [None] * mesh.Faces.Count # list of Vector3d objects
        self.box_insertion_lines = [None] * mesh.Faces.Count # list of Line objects
    
    def chevron_grid(self, s, u_divisions=4, v_division_dist=900, shift=0.5, scale=0.05799):
        if(s is None):
            return
        # Python code
        # Inputs to construct mesh
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
#        print(s.Domain(0))
#        print(s.Domain(1))
#        print(u_divisions)
        for j in range(u_divisions):
#            print(j)
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
        
                # Add mesh faces
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
        mesh = Mesh()
        for point in v:
            mesh.Vertices.Add(point)
        mesh.Faces.AddFaces(f)
        mesh.Weld(100)
        mesh.RebuildNormals()

        return mesh

class MyComponent(component):
    
    def RunScript(self, _surface, _u_divisions, _v_division_dist, _shift, _scale):
        
        
        
        
        
        chevron = Chevron(None)
        _mesh = chevron.chevron_grid(_surface)
        
        # return outputs if you have them; here I try it for you:
        return _mesh
