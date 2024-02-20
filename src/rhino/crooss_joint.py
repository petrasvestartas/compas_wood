"""
NOTE:

- Reference to RhinoCommmon.dll is added by default

- You can specify your script requirements like:

    # r: <package-specifier> [, <package-specifier>]
    # requirements: <package-specifier> [, <package-specifier>]

    For example this line will ask the runtime to install
    the listed packages before running the script:

    # requirements: pytoml, keras

    You can install specific versions of a package
    using pip-like package specifiers:

    # r: pytoml==0.10.2, keras>=2.6.0

- Use env directive to add an environment path to sys.path automatically
    # env: C:/Users/petrasv/AppData/Local/anaconda3/envs/wood_pip/Lib/site-packages/
"""

import System
from ctypes import *
import Rhino
from Rhino.Geometry import Line, Brep, Curve, Polyline, Plane, Vector3d, Point3d, Rectangle3d, Interval, LoftType, BrepSolidOrientation
from Rhino.Geometry.Intersect import Intersection
import math

#################################################################################################
# Load libraries and declare function signature
#################################################################################################

#! python3
# env: C:\Users\petrasv\AppData\Local\anaconda3\envs\wood_pip\Lib\site-packages
# env: C:\brg\2_code\compas_wood\src
# env: C:\brg\2_code\compas_wood\src\compas_wood

if System.Environment.OSVersion.Platform == System.PlatformID.Win32NT:
    print("Windows")
    lib_ = cdll.LoadLibrary("C:/compas_wood/libgmp-10.dll")
    lib = cdll.LoadLibrary("C:/compas_wood/pinvoke_wood.dll")
elif System.Environment.OSVersion.Platform == System.PlatformID.Unix:
    print("macOS")
    lib = cdll.LoadLibrary("/Users/compas_wood/libpinvoke_wood.dylib")
else:
    print("Unknown operating system")

#################################################################################################
# Selection
#################################################################################################
def select_multiple_geometries(message):
    input_geometry = []
    result, obj_refs = Rhino.Input.RhinoGet.GetMultipleObjects(message, False, Rhino.DocObjects.ObjectType.AnyObject)
    for obj_ref in obj_refs:
        input_geometry.append(obj_ref.Object().Geometry)
    return input_geometry

def to_lines(curves):
    lines = []
    for curveobject in curves:
        curve = curveobject.ToNurbsCurve()
        line = Line(curve.PointAtStart, curve.PointAtEnd)
        lines.append(line)
        
    return lines


#################################################################################################
# Convert Lines to Pipes
#################################################################################################

def match_lines(_set0, _set1):

    set0_sorted = _set0
    set1_sorted = []
    for i in range(0, len(_set0)):
        set1_line = Line.Unset
        for j in range(0, len(_set1)):
            if (_set0[i].DistanceTo(_set1[j].From, True) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance or _set0[i].DistanceTo(_set1[j].To, True) < Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance):
                set1_line=_set1[j]
                break
        set1_sorted.append(set1_line)
    return set0_sorted, set1_sorted


def lines_to_pipes(_crv, _align_vec, _radius, _circ_rect):

    _crv = _crv.ToNurbsCurve()
    
    
    if(_crv is  None):
        return
    if(_radius is  None):
        _radius = 0.5
    if(_circ_rect is  None):
        _circ_rect = False
         
    
    def align_plane(plane, v):
        
        p = Plane(plane)
        
        result, parameterAlongXDirection, parameterAlongYDirection = p.ClosestParameter(p.Origin + v)
        angle = math.atan2(parameterAlongXDirection, parameterAlongYDirection)
        p.Rotate(-angle + 1.5707963267949, p.ZAxis, p.Origin)
        
        return p
    
    def create_rectangles(rail_curve, radius, vector, divisions = 10):
        rects = []
        divisions_refined = 1 if _crv.IsLinear(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance) else divisions
        for i in range(divisions_refined+1):
            t = (1.0/divisions_refined)*i
            result, plane = rail_curve.PerpendicularFrameAt(rail_curve.Domain.ParameterAt(t))
            scale = 1
            if(vector):
                plane = align_plane(plane,vector)
            rect = Rectangle3d(plane, Interval(-radius*scale,radius*scale), Interval(-radius*scale,radius*scale))
            rects.append(rect.ToPolyline())
        
        if(len(rects)==2):
            pline0 = Polyline([rects[0][0],rects[0][1],rects[1][1],rects[1][0]])
            pline0.Add(pline0[0])
            pline1 = Polyline([rects[0][3],rects[0][2],rects[1][2],rects[1][3]])
            pline1.Add(pline1[0])
            rects = [pline0,pline1]
        return rects
    
    def create_pipe_from_curve(rail_curve, radius, vector, divisions = 10):
        
        
        rects = create_rectangles(rail_curve, radius, vector, divisions)
        
        localblending = False
        cap = Rhino.Geometry.PipeCapMode.Flat
        fitrail = True
        abstol = 0.1
        angtol = math.radians(15)
        brep = list( Rhino.Geometry.Brep.CreatePipe(rail_curve,radius,localblending,cap,fitrail,abstol,angtol) )[0]
        
        return rects, brep
    
    def create_rect_pipe_from_curve(rail_curve, radius, vector, divisions = 10):
        
        # if the curve is not linear then divide the line into segments
        rects = create_rectangles(rail_curve, radius, vector, divisions)
        
        curves = []
        for c in rects:
            curves.append(c.ToNurbsCurve())
        brep = Brep.CreateFromLoft(curves, Point3d.Unset, Point3d.Unset, LoftType.Normal, False)[0]
        
        if brep:
            brep.Faces.SplitKinkyFaces()
            brep = brep.CapPlanarHoles(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
            if brep:
                if brep.SolidOrientation == BrepSolidOrientation.Inward:
                    brep.Flip()
            
        return rects , brep
    
    if(_circ_rect):
        rects, brep = create_pipe_from_curve(_crv,_radius,_align_vec)
        _brep = brep
        _plines = rects
    else:
        rects, brep = create_rect_pipe_from_curve(_crv,_radius,_align_vec)
        _brep = brep
        _plines = rects
    
    #_brep = Rhino.Geometry.Brep.CreatePipe(_crv, [_radius], False, Rhino.Geometry.PipeCapMode.Flat, False, 0, 0.01)
    #print(_brep)
    
    # return outputs if you have them; here I try it for you:
    return (_brep, _plines)

input_beam_element_geometry = []

# for input_beam_axis in input_beam_axes:
#     brep, two_rectangles = lines_to_pipes(input_beam_axis, Vector3d(0, 0, 1), 3, True)
#     input_beam_element_geometry.append([brep, two_rectangles])
#     Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(brep)
#     Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(two_rectangles[0])
#     Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(two_rectangles[1])

input_beam_axes = select_multiple_geometries("Select multiple lines - beam central axes")
input_beam_axes = to_lines(input_beam_axes)
input_beam_directions = select_multiple_geometries("Select multiple lines - beam directions")
input_beam_directions = to_lines(input_beam_directions)
input_beam_axes, input_beam_directions = match_lines(input_beam_axes, input_beam_directions)
# print(input_beam_axes)
# print(input_beam_directions)
for idx, input_beam_axis in enumerate(input_beam_axes):
    brep, two_rectangles = lines_to_pipes(input_beam_axis, input_beam_directions[idx].Direction, 3, True)
    input_beam_element_geometry.append([brep, two_rectangles])
    Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(brep)
    Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(two_rectangles[0])
    Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(two_rectangles[1])

#################################################################################################
# Joint Parameters
#################################################################################################

#################################################################################################
# Prepare Input
#################################################################################################

#################################################################################################
# Solver
#################################################################################################


#################################################################################################
# Boolean Difference
#################################################################################################

