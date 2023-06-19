"""get joints ids and connections areas between elements"""

__author__ = "petras vestartas"
__version__ = "2023.03.31"

from ghpythonlib.componentbase import executingcomponent as component
import Rhino.RhinoDoc
import Rhino.Geometry
import System
from System.Drawing import Color

class connections_zones(component):


    def to_polyline_from_cp(self, curve):
        
        polyline = Rhino.Geometry.Polyline() 
        result, polyline1 = curve.TryGetPolyline()
        
        if (result == False):
            
            nurbsCurve = curve.ToNurbsCurve()  
            point3dArray = []
            
            for i in range(nurbsCurve.Points.Count):
                result, pt = nurbsCurve.Points.GetPoint(i)
                point3dArray.append(pt)
            
            polyline1 = Rhino.Geometry.Polyline(point3dArray)  
            nurbsCurve = polyline1.ToNurbsCurve()  
            result, polyline1 = nurbsCurve.TryGetPolyline()  
            
            polyline1.CollapseShortSegments(0.01) 
            
            polyline1 = Rhino.Geometry.Polyline(polyline1)  
            polyline = polyline1  
            
        else:
            polyline1.CollapseShortSegments(0.01)  
            polyline = polyline1  
        
        return polyline  

    def loft_polylines_with_holes(self, curves0, curves1):
        """Loft polylines with holes
        
        Parameters
        ----------
        curves0 : list[rg.Polyline]
        curves1 : list[rg.Polyline]
        
        Returns
        -------
        rg.Mesh
            mesh as lofted polylines and cap them
        """
        
        ###############################################################################
        # user input
        ###############################################################################
        
        flag = len(curves0) is not 0 if True else len(curves1) != 0
        if (flag):
            
            curves = []
            curves2 = []
            
            flag0 = len(curves1) == 0
            flag1 = len(curves0) == 0 and len(curves1) != 0
            flag2 = len(curves0) and len(curves1)
            
            if (flag0):
                for i in range(len(curves0)):
                    if (float(i) >= float(len(curves0)) * 0.5):
                        curves2.Add(curves0[i])
                    else:
                        curves.Add(curves0[i])
            elif (flag1):
                for i in range(0, len(curves1), 2):
                    curves.Add(curves1[i])
                    curves2.Add(curves1[i + 1])
            elif (flag2):
                curves = curves0
                curves2 = curves1
            
            curves0 = curves
            curves1 = curves2
        
        ###############################################################################
        # sort curves which one is border and which ones are holes
        ###############################################################################
        length = -1.0
        border_id = -1
        count = 0
        numArray = []
        for curve in curves0:
            diagonal = curve.GetBoundingBox(True).Diagonal
            temp_length = diagonal.Length
            if temp_length > length:
                length = temp_length
                border_id = count
            count=count+1
        
        
        border0 = self.to_polyline_from_cp(curves0[border_id])
        border1 = self.to_polyline_from_cp(curves1[border_id])
        holes0 = []
        holes1 = []
        for i in range(curves0.Count):
            if (i != border_id):
                holes0.Add(curves0[i])
                holes1.Add(curves1[i])
        
        ###############################################################################
        # Create mesh of the bottom face
        ###############################################################################
        mesh_bottom = Rhino.Geometry.Mesh.CreatePatch(border0, Rhino.RhinoDoc.ActiveDoc.ModelAngleToleranceRadians, None, holes0, None, None, True, 1)
        
        ###############################################################################
        # Convert closed polylines to open ones
        ###############################################################################
        
        point_3d_array = []
        for i in range(len(mesh_bottom.Vertices)):
            point_3d_array.append(mesh_bottom.Vertices.Point3dAt(i))
        
        open_curve_points_0 = []
        open_curve_points_1 = []
        
        for i in range(border0.Count-1):
            open_curve_points_0.append( border0[i])
            open_curve_points_1.append( border1[i])
        
        for i in range(holes0.Count):
            holes0_polyline = self.to_polyline_from_cp(holes0[i]) 
            holes1_polyline = self.to_polyline_from_cp(holes1[i]) 
            for j in range(len(holes0_polyline)-1):
                open_curve_points_0.append(holes0_polyline[j])
                open_curve_points_1.append(holes1_polyline[j])
        
        ###############################################################################
        # Using the bottom mesh vertices create the top mesh f
        ###############################################################################
        points_to_sort_new = self.sort_set_of_points(open_curve_points_0, point_3d_array)
        mesh_top = mesh_bottom.DuplicateMesh()
        
        for i in range(len(points_to_sort_new)):
            mesh_top.Vertices.SetVertex(i, open_curve_points_1[points_to_sort_new[i]])
        
        ###############################################################################
        # Merge the top and bottom mesh into one and add sides
        ###############################################################################
        mesh = Rhino.Geometry.Mesh()
        mesh.Append(mesh_bottom)
        mesh.Append(mesh_top)
        
        count = mesh_bottom.Vertices.Count
        for p in range (mesh_bottom.TopologyEdges.Count):
            
            if (mesh_bottom.TopologyEdges.GetConnectedFaces(p).Length == 1):
                topologyVertices = mesh_bottom.TopologyEdges.GetTopologyVertices(p)
                mesh.Faces.AddFace(topologyVertices.I, topologyVertices.J, topologyVertices.J + count, topologyVertices.I + count)
        
        mesh.UnifyNormals()
        mesh.Ngons.AddPlanarNgons(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 1, 3, 1, True)
        
        ###############################################################################
        # Unweld ngons meshes
        ###############################################################################
        unwelded_ngon_mesh = Rhino.Geometry.Mesh()
        count = 0
        for ngonAndFacesEnumerable in mesh.GetNgonAndFacesEnumerable():
            
            faces = []
            for j in ngonAndFacesEnumerable.FaceIndexList():
                faces.append(System.Int32(j))
            
            temp_ngon_mesh = mesh.DuplicateMesh().Faces.ExtractFaces(faces)
            
            #add colors
            if(count<2):
                temp_ngon_mesh.VertexColors.CreateMonotoneMesh(Color.LightGray)
            else:
                temp_ngon_mesh.VertexColors.CreateMonotoneMesh(Color.DeepPink)

            unwelded_ngon_mesh.Append(temp_ngon_mesh)
            count = count + 1
        
        ###############################################################################
        # Output
        ###############################################################################
        unwelded_ngon_mesh.RebuildNormals()
        return unwelded_ngon_mesh




    def sort_set_of_points(self, points_to_sort, guide_points):
        """Sort one array of point by an another point array
        
        use it like this: 
        #points_to_sort_new = []
        #for i in points_to_sort_ids:
        #    points_to_sort_new.append(points_to_sort[i])
        
        Parameters
        ----------
        points_to_sort : list[rg.Point3d]
        guide_points : list[rg.Point3d]
        
        Returns
        -------
        list[int]
            the list of indices of the sorted array
        """
        
        # create a copy
        points_to_sort_copy = []
        for i in points_to_sort:
            points_to_sort_copy.append(Rhino.Geometry.Point3d(i))
        
        guide_points_copy = []
        for i in guide_points:
            guide_points_copy.append(Rhino.Geometry.Point3d(i))
        
        # make indices of points
        points_to_sort_ids = list(range(0,len(points_to_sort_copy)))
        guide_points_ids = list(range(0,len(guide_points_copy)))
        
        # sort both lists by xyz coordinates together with indices
        points_to_sort_copy, points_to_sort_ids = zip(*sorted(zip(points_to_sort_copy, points_to_sort_ids)))
        guide_points_copy, guide_points_ids = zip(*sorted(zip(guide_points_copy, guide_points_ids)))
        
        # sort guide points indices and order the points to sort indec
        guide_points_ids, points_to_sort_ids = zip(*sorted(zip(guide_points_ids, points_to_sort_ids)))
        
        # can be used like this:
        #points_to_sort_new = []
        #for i in points_to_sort_ids:
        #    points_to_sort_new.append(points_to_sort[i])
        
        return points_to_sort_ids

    def RunScript(self, _p0, _p1):
        if _p0 and _p1:
            return self.loft_polylines_with_holes(_p0, _p1)