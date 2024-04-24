"""Provides a scripting component.
    Inputs:
        x: The x script variable
        y: The y script variable
    Output:
        a: The a output variable"""

__author__ = "petras"
__version__ = "2023.04.02"

from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import Rhino
from Rhino.Geometry import Polyline, Point3d
#from compas_rhino import conversions
import System
import time
from ctypes import *
#from compas_wood.ctypes_conversion import *

import os

app_data = os.getenv('APPDATA')
vers = Rhino.RhinoApp.Version.Major
plugin_name = 'compas_wood'
plugin_path = "{0}\McNeel\Rhinoceros\packages\{1}.0\{2}".format(app_data, vers, plugin_name)

for _path in os.walk(plugin_path):
    if os.path.isdir(_path[0]):
        plugin_path =  _path[0]




def list_polylines_coord(polylines):
    f = []
    v = []
    for polyline in polylines:
        f.append(len(polyline))
        for point in polyline:
            v.append(point[0]*10)
            v.append(point[1]*10)
            v.append(point[2]*10)
            #v.extend(point)
    _f = (c_size_t * len(f))(*f)
    _v = (c_float * len(v))(*v)
    return _f, c_size_t(len(f)), _v, c_size_t(len(v))


def lists_vectors_coord(vectors):
    faces = [len(v) for v in vectors]
    f = (c_size_t * len(vectors))(*faces)
    v = [coord for vec in vectors for point in vec for coord in point]

    v_s = c_size_t(len(v))  # * sizeof(c_float)
    v = (c_float * len(v))(*v)
    return f, c_size_t(len(vectors)), v, v_s


def lists_numbers_coord(numbers):
    v_s = 0
    f_s = len(numbers)
    f = (c_size_t * len(numbers))()

    for i in range(len(numbers)):
        f[i] = len(numbers[i])
        v_s += len(numbers[i]) * 1

    v = (c_int * v_s)()

    count = 0
    for i in range(len(numbers)):
        for j in range(len(numbers[i])):
            v[count] = numbers[i][j]
            count += 1

    return f, c_size_t(f_s), v, c_size_t(v_s)


def list_ints_coord(numbers):
    f_s = len(numbers)
    f = [0.0] * f_s
    for i in range(f_s):
        f[i] = numbers[i]
    return (c_int * len(f))(*f), c_size_t(f_s)


def list_numbers_coord(numbers):
    f_s = len(numbers)
    f = [0.0] * f_s
    for i in range(f_s):
        f[i] = numbers[i]
    return (c_float * len(f))(*f), c_size_t(f_s)



def coord_polylines_lists(groups_f, groups_f_s, out_f, out_f_s, out_v, out_v_s):
    polylines = [
        [Polyline([]) for j in range(groups_f[i])] for i in range(groups_f_s.value)
    ]

    a = 0
    b = 0
    pline_count = 0
    num_points = 3
    num_groups = groups_f_s.value

    points = []
    for i in range(0, out_v_s.value, num_points):
        #points.append([out_v[i]*0.1, out_v[i + 1]*0.1, out_v[i + 2]*0.1])
        points.append(Point3d(out_v[i]*0.1, out_v[i + 1]*0.1, out_v[i + 2]*0.1))
        num_points_per_polyline = out_f[pline_count]

        if len(points) == num_points_per_polyline:
            for j in range(a, groups_f_s.value):
                if groups_f[j] > 0:
                    a = j
                    break

            polylines[a][b] = Polyline(points)
            points = []
            num_polylines_per_group = groups_f[a]
            if b == (num_polylines_per_group - 1):
                a += 1
                b = 0
            else:
                b += 1
            pline_count += 1

    return polylines


def coord_numbers_lists(out_f, out_f_s, out_v, out_v_s):

    # create Python objects
    ids = [[] for _ in range(out_f_s.value)]

    for i in range(out_f_s.value):
        ids[i] = [None] * out_f[i]
        if out_f[i] == 0:
            return

    # fill array with points
    a = 0
    b = 0
    for i in range(0, out_v_s.value):
        ids[a][b] = out_v[i]
        if b == (out_f[a] - 1):
            a += 1
            b = 0
        else:
            b += 1

    return ids



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
        
        input_polylines = []
        if( _data[0] != 0):
            #convert to compas polylines
            for i in _data[0]:
                for j in i:
                    input_polylines.append(j)#(conversions.polyline_to_compas(j))
        
        input_vectors = []
        if(_data[1] != None):
            for i in _data[1]:
                list = []
                for j in i:
                    list.append(j)#(conversions.vector_to_compas(j))
                input_vectors.append(list)
        
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
            if(i%3==0):
                joint_parameters[i] = _join_p[i]*10
            else:
                joint_parameters[i] = _join_p[i]*1
            
        
        search = id = max(min(2, _find), 0)
        
        scale = [1,1,1]
        for i in range(0, len(_scale)):
            scale[i] = _scale[i]
        
        output_type = _get if _get != None else 4
        
        extension = [0,0,0]
        
        if(len(_extension)>2):
            extension = []
            for i in _extension:
                extension.append(i*10)
        
        
        
        input_adjacency = []
        if(len(_data[4])>0):
            for i in _data[4]:
                for j in i:
                    input_adjacency.append(j)
        
        ############################################################################################################
        # Load inputs and perform conversion
        ############################################################################################################
    
        # input geometry
        # input_polylines = data_set_plates.annen_small_polylines()
        # print(len(input_polylines))
        (
            in_polyline_pairs_f,
            in_polyline_pairs_f_s,
            in_polyline_pairs_v,
            in_polyline_pairs_v_s,
        ) = list_polylines_coord(input_polylines)
        
    
        #input_vectors = data_set_plates.annen_small_edge_directions()
    
        (
            in_vectors_f,
            in_vectors_f_s,
            in_vectors_v,
            in_vectors_v_s,
        ) = lists_vectors_coord(input_vectors)
        
        
        
        input_joints = _data[2]#data_set_plates.annen_small_edge_joints()
        # print(len(input_joints))
        (
            in_joints_f,
            in_joints_f_s,
            in_joints_v,
            in_joints_v_s,
        ) = lists_numbers_coord(input_joints)
    
        input_three_valence =_data[3]#(data_set_plates.annen_small_three_valance_element_indices_and_instruction()   )
        
        # print(len(input_three_valence))
    
        (
            in_three_valence_f,
            in_three_valence_f_s,
            in_three_valence_v,
            in_three_valence_v_s,
        ) = lists_numbers_coord(input_three_valence)
    
        #input_adjacency = []
        #print(input_adjacency)
        (
            in_adjancency_v,
            in_adjancency_v_s,
        ) = list_ints_coord(input_adjacency)
    
        # input parameters
        (
            in_joint_parameters_v,
            in_joint_parameters_v_s,
        ) = list_numbers_coord(joint_parameters)
    
        in_search_type = c_int(search)
        (
            in_scale_v,
            in_scale_v_s,
        ) = list_numbers_coord(scale)
    
        in_output_type = c_int(output_type)
    
        # output
    
        # out polylines
        out_plines_groups_f = POINTER(c_size_t)()
        out_plines_groups_f_s = c_size_t()
        out_plines_out_f = POINTER(c_size_t)()
        out_plines_out_f_s = c_size_t()
        out_plines_out_v = POINTER(c_float)()
        out_plines_out_v_s = c_size_t()
    
        # out types
        out_types_f = POINTER(c_size_t)()
        out_types_f_s = c_size_t()
        out_types_v = POINTER(c_int)()
        out_types_v_s = c_size_t()
    
        # global_parameters
        (
            in_joint_volume_parameters_v,
            in_joint_volume_parameters_v_s,
        ) = list_numbers_coord(extension)
        
        face_to_face_side_to_side_joints_dihedral_angle = c_float(130.0)
        
        ############################################################################################################
        # Load library and declare function signature
        ############################################################################################################
        

        if System.Environment.OSVersion.Platform == System.PlatformID.Win32NT:
            #print("Windows")
            lib_ = cdll.LoadLibrary(plugin_path+"\libgmp-10.dll")
            lib = cdll.LoadLibrary(plugin_path+"\pinvoke_wood.dll")
        elif System.Environment.OSVersion.Platform == System.PlatformID.Unix:
            #print("macOS")
            lib = cdll.LoadLibrary("/Users/compas_wood/libpinvoke_wood.dylib")
        else:
            print("Unknown operating system")
                

        lib.ctypes_get_connection_zones.restype = None
        lib.ctypes_get_connection_zones.argtypes = [
            # input geometry
            # polylines
            POINTER(c_size_t),
            POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
            POINTER(c_float),
            POINTER(c_size_t),  # if a value is passed by reference, it must be a pointer
            # vectors
            POINTER(c_size_t),
            POINTER(c_size_t),
            POINTER(c_float),
            POINTER(c_size_t),
            # joints
            POINTER(c_size_t),
            POINTER(c_size_t),
            POINTER(c_int),
            POINTER(c_size_t),
            # three valence
            POINTER(c_size_t),
            POINTER(c_size_t),
            POINTER(c_int),
            POINTER(c_size_t),
            # adjacency
            POINTER(c_int),
            POINTER(c_size_t),
            # input parameters
            POINTER(c_float),
            POINTER(c_size_t),
            # search_type
            POINTER(c_int),
            # scale
            POINTER(c_float),
            POINTER(c_size_t),
            # output_type
            POINTER(c_int),
            # output
            # polylines
            POINTER(POINTER(c_size_t)),
            POINTER(c_size_t),
            POINTER(POINTER(c_size_t)),
            POINTER(c_size_t),
            POINTER(POINTER(c_float)),
            POINTER(c_size_t),
            # types
            POINTER(POINTER(c_size_t)),
            POINTER(c_size_t),
            POINTER(POINTER(c_int)),
            POINTER(c_size_t),
            # global_parameters
            POINTER(c_float), # joint volume
            POINTER(c_size_t), # joint volume
            POINTER(c_float) # dihedral angle to switch between in-plane and out-of-plane jointery default is 150 degrees
        ]
    
        # generate joints
        start_time = time.time()
        ############################################################################################################
        # Call the function
        # https://stackoverflow.com/questions/4145775/how-do-i-convert-a-python-list-into-a-c-array-by-using-ctypes
        # https://stackoverflow.com/questions/53265216/python-ctypes-type-conversion
        ############################################################################################################
        
        lib.ctypes_get_connection_zones(
            # input geometry
            # polylines
            in_polyline_pairs_f,
            byref(in_polyline_pairs_f_s),
            in_polyline_pairs_v,
            byref(in_polyline_pairs_v_s),
            # vectors
            in_vectors_f,
            byref(in_vectors_f_s),
            in_vectors_v,
            byref(in_vectors_v_s),
            # joints
            in_joints_f,
            byref(in_joints_f_s),
            in_joints_v,
            byref(in_joints_v_s),
            # three_valence
            in_three_valence_f,
            byref(in_three_valence_f_s),
            in_three_valence_v,
            byref(in_three_valence_v_s),
            # adjacency
            in_adjancency_v,
            byref(in_adjancency_v_s),
            # input parameters
            # joint_parameters
            in_joint_parameters_v,
            byref(in_joint_parameters_v_s),
            # search_type
            byref(in_search_type),
            # scale
            in_scale_v,
            byref(in_scale_v_s),
            # output_type
            byref(in_output_type),
            # output
            # polylines
            byref(out_plines_groups_f),
            byref(out_plines_groups_f_s),
            byref(out_plines_out_f),
            byref(out_plines_out_f_s),
            byref(out_plines_out_v),
            byref(out_plines_out_v_s),
            # types
            byref(out_types_f),
            byref(out_types_f_s),
            byref(out_types_v),
            byref(out_types_v_s),
            # global_parameters
            in_joint_volume_parameters_v,
            byref(in_joint_volume_parameters_v_s),
            byref(face_to_face_side_to_side_joints_dihedral_angle),
        )
    
        """
    
        print(out_plines_groups_f[0])
        out_polylines = []
        """
        out_polylines = coord_polylines_lists(
            out_plines_groups_f,
            out_plines_groups_f_s,
            out_plines_out_f,
            out_plines_out_f_s,
            out_plines_out_v,
            out_plines_out_v_s,
        )
        
        
        def temp_coord_numbers_lists(out_f, out_f_s, out_v, out_v_s):
        
            # create Python objects
            ids = [[] for _ in range(out_f_s.value)]
            
            
            
            for i in range(out_f_s.value):
                #print(i, out_f_s.value)
                ids[i] = [None] * out_f[i]
                if out_f[i] == 0:
                    break
            
            #print( ids)
            # fill array with points
            a = 0
            b = 0
            #for i in range(0, out_v_s.value):
            #    print( out_v[i])
            """
            ids[a][b] = out_v[i]
            if b == (out_f[a] - 1):
                a += 1
                b = 0
            else:
                b += 1
            """
        
            return ids
        
        _cuts = coord_numbers_lists(
            out_types_f, out_types_f_s, out_types_v, out_types_v_s
        )
        """
        print _cuts
        print  out_types_f, out_types_f_s, out_types_v, out_types_v_s
        print out_types_v[0]
        print out_types_f[0]
        
        print(
            "\n______________________________________ %s ms ______________________________________"
            % round((time.time() - start_time) * 1000.0, 2)
        )
        
        """
    
    

        
        
        # ==============================================================================
        # output
        # ==============================================================================
        _plines = []
        self.bbox = Rhino.Geometry.BoundingBox.Unset
        # nest two times the result and add to the tree
        for i in range(0, len(out_polylines)):
            list = []
            for j in range(0, len(out_polylines[i])):
                    list.append(out_polylines[i][j])#(conversions.polyline_to_rhino(out_polylines[i][j]))
                    self.polylines.append(list[-1])
                    self.bbox.Union(list[-1].BoundingBox)
            _plines.append(list)
        
        _data = [[_plines,_data[1],_data[2],_data[3],_data[4],_data[5],_cuts]]
        #print(_cuts)
        
        ############################################################################################################
        # release the memory
        ############################################################################################################
    
        # release output of polylines
        
        
        lib.release_size_t(byref(out_plines_groups_f), True)
        lib.release_size_t(byref(out_plines_out_f), True)
        lib.release_float(byref(out_plines_out_v), True)
    
        # release output of types
        lib.release_size_t(byref(out_types_f), True)
        lib.release_int(byref(out_types_v), True)
        
        
        return _data