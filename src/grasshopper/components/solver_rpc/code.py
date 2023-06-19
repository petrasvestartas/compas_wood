"""get joints ids and connections areas between elements"""

__author__ = "petras vestartas"
__version__ = "2023.04.03"


from ghpythonlib.componentbase import executingcomponent as component
# STEP 0 - Installation, anaconda prompt:
# python -m compas_rhino.install
# python -m compas_rhino.install -p compas_wood
# STEP 1 - check the environment name
# import compas_bootstrapper
# print(compas_bootstrapper.ENVIRONMENT_NAME)
# STEP 2 - import proxy from compas.rpc
# https://compas.dev/compas/latest/tutorial/rpc.html#supported-data-types-1
from compas.rpc import Proxy # https://compas.dev/compas/latest/api/compas.rpc.html
proxy = Proxy('compas_wood.joinery') # import module
from compas_rhino import conversions
from ghpythonlib import treehelpers
import Rhino
import Grasshopper
from Rhino.Geometry import Polyline
# STEP 3 - start server
#proxy.stop_server()
#proxy.start_server()


class connections_zones(component):
    
    bbox = Rhino.Geometry.BoundingBox.Unset
    lines = []
    insertion_vectors_current = []
    joint_per_face_current_text_entity = []
    polylines = []
    
    def DrawViewportWires(self, args):
        #GrasshopperDocument = Grasshopper.Instances.ActiveCanvas.Document
        #is_selected = False
        """
        for _ in GrasshopperDocument.Objects:
            is_selected = _.Attributes.Selected
        """
        #col = args.WireColour_Selected if is_selected else args.WireColour;
        col =  args.WireColour
        line_weight = args.DefaultCurveThickness;
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
    
    def RunScript(self, _data, _join_p, _scale, _extension, _find, _get):
        # ==============================================================================
        # clear input
        # ==============================================================================
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        self.lines = []
        self.insertion_vectors_current = []
        self.joint_per_face_current_text_entity = []
        self.polylines = []
        # ==============================================================================
        # input
        # ==============================================================================
        if(_data is None):
            return
        
        output_polylines = []
        if( _data[0] != 0):
            #convert to compas polylines
            for i in _data[0]:
                for j in i:
                    output_polylines.append(conversions.polyline_to_compas(j))
        
        vectors = []
        if(_data[1] != None):
            for i in _data[1]:
                list = []
                for j in i:
                    list.append(conversions.vector_to_compas(j))
                vectors.append(list)
        
        joint_parameters = [
            300, 0.5, 3,   # 1-9 ss_e_ip (side-to-side edge in-plane)
            50, 0.64, 15,  # 10-19 ss_e_op (side-to-side edge out-of-plane)
            450, 0.5, 20,  # 20-29 ts_e_p (top-to-side edge plane)
            300, 0.5, 30,  # 30-39 cr_c_ip (cross cutting in-plane)
            6, 0.95, 40,   # 40-49 tt_e_p  (top-to-top edge plane)
            300, 0.5, 58,  # 50-59 ss_e_r (side-to-side edge rotated)
            300, 1.0, 60   # 60-69 b (boundary)
        ]
        
        for i in range(0, len(_join_p)):
            joint_parameters[i] = _join_p[i]
        
        search = id = max(min(2, _find), 0)
        
        scale = [1,1,1]
        for i in range(0, len(_scale)):
            scale[i] = _scale[i]
        
        output_type = _get if _get != None else 4
        
        extension = [0,0,0]
        for i in range(0, len(_extension)):
            extension[i] = _extension[i]
        
        
        adjacency = []
        if(len(_data[4])>0):
            for i in _data[4]:
                for j in i:
                    adjacency.append(j)
        
        # ==============================================================================
        # get joints
        # ==============================================================================
        result = proxy.get_connection_zones(
        output_polylines,
        vectors, # output_vectors,
        _data[2], # output_joints_types,
        _data[3], # output_three_valence_element_indices_and_instruction,
        adjacency, # output_adjacency,
        joint_parameters,
        search,
        scale,
        output_type,
        False,
        extension)
        
        
        
        # ==============================================================================
        # output
        # ==============================================================================
        _plines = []
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        # nest two times the result and add to the tree
        for i in range(0, len(result)):
            list = []
            for j in range(0, len(result[i])):
                    list.append(conversions.polyline_to_rhino(result[i][j]))
                    self.polylines.append(list[-1])
                    self.bbox.Union(list[-1].BoundingBox)
            _plines.append(list)
        _cuts = []
        _data = [[_plines,_data[1],_data[2],_data[3],_data[4],_data[5],_cuts]]
        
        return _data