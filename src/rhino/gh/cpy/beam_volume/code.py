from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import rhinoscriptsyntax as rs

import Rhino
from Rhino.Geometry import Point3d, Vector3d, Polyline, Curve, Line, Plane, Brep, BoundingBox, Interval, RTree, Transform, Rectangle3d
from collections import OrderedDict
from Grasshopper import DataTree
from Grasshopper.Kernel.Data import GH_Path
import math
from System.Collections.Generic import SortedDictionary

class MyComponent(component):
    
    def RunScript(self, _circles, _length_of_box, _distance_tolerance, _parallel_tolerance, _cross_or_toptoside, _create_pipes):
        
        
        class Cut:
            pass
        
        class OutlineType:
            Top = 1
            Side = 2
            Center = 9
            Unset = 777
        
        class Outline:
            
            
            def __init__(self, pline=None, name=None, outline=None):
                
                
                self.plines = []
                self.plane = Plane.Unset
                self.name = OutlineType.Unset
                self.bbox = BoundingBox.Unset
                self.tileNames = None
            
                if pline is not None and name is not None:
                    
                    if(self.is_list_of_polylines(pline)):
                        for current_pline in pline:
                            self.plines.append(Polyline(current_pline))
                    else:
                        self.plines = [Polyline(pline)]
                    
                    self.plane = self.GetPlane(self.plines[0])
                    self.name = name
                    self.bbox = self.plines[0].BoundingBox
                    self.bbox.Inflate(0.02)
                    
                elif outline is not None and name is not None:
                    
                    self.plines = outline.Duplicate()
                    self.plane = self.GetPlane(outline[0])
                    self.name = name
                    self.bbox = BoundingBox()
                    for polyline in outline:
                        self.bbox.Union(polyline.BoundingBox)
                    self.bbox.Inflate(0.02)
            
            def is_list_of_polylines(self, obj):
                if isinstance(obj, list):
                    if all(isinstance(item, Polyline) for item in obj):
                        return True
            
            def AverageNormal(self, p):
                
                count = p.Count - 1
                vector3d = Vector3d(0,0,0)
                num = count - 1
                
                for i in range(num + 1):
                    num1 = (i - 1 + count) % count
                    num2 = (i + 1 + count) % count
                    item = p[num1]
                    point3d = p[num2]
                    item1 = p[i]
                    vector3d += Vector3d.CrossProduct(Vector3d(item1 - item), Vector3d(point3d - item1))
                
                if vector3d.X == 0 and vector3d.Y == 0 and vector3d.Z == 0:
                    vector3d.Unitize()
                
                return vector3d
            
            def GetPlane(self, polyline, AveragePlane=True):
                plane = Plane.Unset
                if not AveragePlane:
                    Plane.FitPlaneToPoints(polyline, plane)
                    plane.Origin = polyline.CenterPoint()
                    return plane
                
                line = polyline.SegmentAt(0)
                vector3d = line.Direction
                vector3d.Unitize()
                vector3d1 = self.AverageNormal(polyline)
                vector3d1.Unitize()
                vector3d2 = Vector3d.CrossProduct(vector3d, vector3d1)
                return Plane(polyline.CenterPoint(), vector3d, vector3d2)
            
            def FlipAndRotate(self, p):
                plane = Plane(p)
                plane.Flip()
                plane.Rotate(1.5707963267949, plane.ZAxis)
                return plane
            
            def IsClockwiseClosedPolyline(self, p, plane):
                polyline = Polyline(p)
                world_xy = Rhino.Geometry.Plane.WorldXY
                plane_to_world_xy = Rhino.Geometry.Transform.PlaneToPlane(plane, world_xy)
                polyline.Transform(plane_to_world_xy)
                y = 0
                for i in range(len(polyline) - 1):
                    item = polyline[i + 1]
                    x = item.X
                    item = polyline[i]
                    num = x - item.X
                    item = polyline[i + 1]
                    y1 = item.Y
                    item = polyline[i]
                    y += num * (y1 + item.Y)
                return y > 0
            
            def CheckPolygonOrientation(self, optionalOutline):
                for i in range(len(self.plines)):
                    if i == 0:
                        if self.IsClockwiseClosedPolyline(self.plines[i], self.plane):
                            self.plines[i].Reverse()
                            optionalOutline.plines[i].Reverse()
                    else:
                        if not self.IsClockwiseClosedPolyline(self.plines[i], self.plane):
                            self.plines[i].Reverse()
                            optionalOutline.plines[i].Reverse()
            
            def Duplicate(self):
                outline = Outline()
                outline.bbox = self.bbox
                outline.plines = []
                for i in self.plines:
                    outline.plines.append(i.Duplicate())
                outline.plane = Plane(self.plane)
                outline.name = self.name
                if self.tileNames is None:
                    outline.tileNames = None
                else:
                    outline.tileNames = [name for name in self.tileNames]
                return outline
            
            def FlipPlane(self, optionalOutline):
                self.plane = self.FlipAndRotate(self.plane)
                optionalOutline.plane = self.FlipAndRotate(optionalOutline.plane)
            
            def Transform(self, xform):
                for i in range(len(self.plines)):
                    self.plines[i].Transform(xform)
                self.plane.Transform(xform)
                self.bbox.Transform(xform)
        
        class Beam:
            
            id = None
            id2 = -1
            centreCurve = None
            centrePline = None
            frames = []
            radii = []
            samples = 5
            circles = []
            polygons = []
            cuts = []
            jointsID = []
            endCuts = True
            debug = []
            
            @property
            def GetMidPt(self):
                t0 = curve.Domain.T0
                domain = curve.Domain
                return self.centreCurve.PointAt((t0 + domain.T1) * 0.5)
            
            def __getitem__(self, index):
                if index == 0:
                    return self.centreCurve.PointAtStart
                return self.centreCurve.PointAtEnd
            
            
            def __init__(self, id, centrePline, radii, adjustDirection_, planes=None, samples_=20, rebuild=False):
                plane = Plane.Unset
                num = 0
                point3dArray = None
                self.id = id
                self.centreCurve = Curve.CreateInterpolatedCurve(centrePline, 3, Rhino.Geometry.CurveKnotStyle.ChordPeriodic)
                self.centrePline = centrePline
                self.radii = []
                self.frames = []
                if planes is not None and len(planes) != 0:
                    for i in range(len(planes)):
                        self.centreCurve.ClosestPoint(planes[i].Origin, num)
                        self.frames.append((num, planes[i]))
                    self.radii = list(radii)
                elif not self.centreCurve.IsPlanar():
                    curve = self.centreCurve
                    domain = self.centreCurve.Domain
                    plane1 = curve.GetPlane(domain.T0, False)
                    planes = [plane1]
                    self.frames.append((self.centreCurve.Domain.T0, plane1))
                    self.radii = list(radii)
                else:
                    self.centreCurve.TryGetPlane(plane)
                    plane2 = Plane(self.centreCurve.PointAtStart(), plane.ZAxis, Vector3d.CrossProduct(self.centreCurve.TangentAtStart, plane.ZAxis))
                    plane3 = Plane(self.centreCurve.PointAtEnd(), plane.ZAxis, Vector3d.CrossProduct(self.centreCurve.TangentAtEnd, plane.ZAxis))
                    self.frames.append((self.centreCurve.Domain.T0, plane2))
                    self.frames.append((self.centreCurve.Domain.T1, plane3))
                    self.radii = list(radii)
                if len(radii) == len(centrePline):
                    self.radii.clear()
                    self.centreCurve.DivideByCount(self.samples - 1, True, point3dArray)
                    for j in range(len(point3dArray)):
                        num1 = centrePline.ClosestParameter(point3dArray[j])
                        num2 = MathUtil.RemapNumbers(num1 % 1, 0, 1, radii[int(Math.Floor(num1))], radii[int(Math.Ceiling(num1))])
                        self.radii.append(num2)
                self.SortFrames()
                self.frames = self.RecalculateFrames(self.centreCurve, self.frames)
                self.circles = []
                self.polygons = []
                self.cuts = []
                self.jointsID = []
                if adjustDirection_ != Line.Unset:
                    plane4 = self.centreCurve.GetPlane(self.frames, adjustDirection_.get_From())
                    num3 = Vector3d.VectorAngle(plane4.get_XAxis(), adjustDirection_.get_Direction(), plane4)
                    for k in range(len(self.frames)):
                        item2 = self.frames[k][1]
                        item2.Rotate(num3, plane4.get_ZAxis())
                        self.frames[k] = (self.frames[k][0], item2)
            
            def RecalculateFrames(self, centreCurve, frames):
                frames_updated = []
                for i in range(len(frames)):
                    point3d = centreCurve.PointAt(frames[i][0])
                    vector3d = centreCurve.TangentAt(frames[i][0])
                    item2 = frames[i][1]
                    vector3d1 = Vector3d.CrossProduct(item2.YAxis, vector3d)
                    vector3d2 = Vector3d.CrossProduct(vector3d, vector3d1)
                    frames_updated.append( (frames[i][0], Plane(point3d, vector3d1, vector3d2)) )
                return frames_updated
            
            def AdjustPlanes(self, l):
                    plane = self.GetPlane(l.From)
                    num = rg.Vector3d.VectorAngle(plane.XAxis, l.Direction, plane)
                    for i in range(len(self.frames)):
                        item2 = self.frames[i].Item2
                        item2.Rotate(num, plane.ZAxis)
                        self.frames[i] = rg.Tuple(self.frames[i].Item1, item2)
            
            def CreateBeamCircles(self, rebuild=False):
                return self.centreCurve.CreateBeamCircles(self.radii, self.frames, self.samples, rebuild, self.samples, 1, 1)
            
            def CreateBeamLoft(self):
                return rg.Brep.CreatePipe(self.centreCurve, self.frames, self.radii)
            
            def Duplicate(self):
                beam = Beam()
                beam.id = self.id
                beam.id2 = self.id2
                beam.centreCurve = self.centreCurve.DuplicateCurve()
                beam.centrePline = self.centrePline.Duplicate()
                beam.frames = [rg.Tuple(frame.Item1, rg.Plane(frame.Item2)) for frame in self.frames]
                beam.radii = self.radii[:]
                beam.samples = self.samples
                beam.circles = [rg.Circle(rg.Plane(circle.Plane), circle.Radius) for circle in self.circles]
                beam.polygons = [rg.Polyline(polygon) for polygon in self.polygons]
                beam.cuts = [cut.Duplicate() for cut in self.cuts]
                beam.jointsID = self.jointsID[:]
                beam.endCuts = self.endCuts
                return beam
            
            def BoundingBox(self, crvs, inflate=0.001):
                boundingBox = crvs[0].GetBoundingBox(False)
                for crv in crvs:
                    boundingBox.Union(crv.GetBoundingBox(True))
                boundingBox.Inflate(inflate)
                return boundingBox
            
            def GetBoundingBox(self, inflate=0.001):
                return self.BoundingBox(self.OffsetCurve4(False), inflate)
            
            def GetCP(self, p):
                return self.centreCurve.TCP(p)
            
            def GetEndPlanes(self):
                domain = self.centreCurve.Domain
                plane1 = self.GetPlane(domain.T0)
                domain = self.centreCurve.Domain
                plane2 = self.GetPlane(domain.T1)
                return rg.Tuple(plane1, plane2)
            
            
            def TCP(self, curve, p):
                result, num = curve.ClosestPoint(p)
                return num
            
            def TXAB(self, curve, valueFromAToB):
                min = curve.Domain.Min
                domain = curve.Domain
                return self.RemapNumbers(valueFromAToB, min, domain.Max, 0, 1)
            
            
            def GetInterpolatedRadius_by_parameter(self, t):
                return self.Interpolate(self.radii, self.TXAB(self.centreCurve, t))
            
            
            def GetInterpolatedRadius_by_point(self, p):
                
                return self.Interpolate(self.radii, self.TCP(self.centreCurve, p))
            
            def GetMidPlane(self):
                domain = self.centreCurve.Domain
                return self.GetPlane(domain.Mid)
            
            def GetP(self, t):
                return self.centreCurve.PointAt(t)
            
            def GetParameters(self):
                nums = []
                for frame in self.frames:
                    nums.append(frame.Item1)
                return nums
            
            def GetPlane_by_point(self, centerCurve, frames, p):
                
                
                result, num = centerCurve.ClosestPoint(p)
                tuple = self.FramesAround(centerCurve, frames, num)
                plane = self.InterpolatePlanes(tuple[0], tuple[1], tuple[2])
                plane.Origin = centerCurve.PointAt(num)
                plane.Transform(Transform.Rotation(plane.ZAxis, centerCurve.TangentAt(num), plane.Origin))
                return plane
            
            def GetPlane_by_parameter(self, centerCurve, frames, t):
                tuple = self.FramesAround(centerCurve,frames, t)
                plane = self.InterpolatePlanes(tuple[0], tuple[1], tuple[2])
                plane.Origin = centerCurve.PointAt(t)
                plane.Transform(Transform.Rotation(plane.ZAxis, centerCurve.TangentAt(t), plane.Origin))
                return plane
        
            
            def GetPlane_by_p(self, p):
                return self.GetPlane_by_point(self.centreCurve, self.frames, p)
            
            def GetPlane_by_t(self, t):
                return self.GetPlane_by_parameter(self.centreCurve, self.frames, t)
            
            def OrientPlaneTowardsCurveShorterEnd(self, curve, plane, t):
                
                #print(message)
                plane1 = Plane(plane)
                
                # which side is shorter
                p = curve.PointAt(t)
                copy_curve = curve.Duplicate()
                copy_curve = copy_curve.Extend(Rhino.Geometry.CurveEnd.Both, copy_curve.GetLength(0.01)*0.1, Rhino.Geometry.CurveExtensionStyle.Smooth )
                copy_curve.Domain = Interval(0,1)
                
                result, cpt = copy_curve.ClosestPoint(p)
                
                domain0 = Interval(copy_curve.Domain.T0, cpt)
                domain1 = Interval(cpt, copy_curve.Domain.T1)
                
                
                
                # get middle point of that side
                orientation_point = copy_curve.PointAt(domain0.Mid)
                if(domain1.Length>domain0.Length):
                    orientation_point = copy_curve.PointAt(domain1.Mid)
                    
                # orient the plane based on the distance search
                distance = plane.DistanceTo(orientation_point)  
                #print(distance) 
                
                #Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(orientation_point)
                if(distance>0):
                    plane1 = self.FlipAndRotate(plane1)
                
                return plane1
            
            
            def GetPlaneOrientedToShorterSide_by_t(self, t):
                plane = self.GetPlane_by_parameter(self.centreCurve, self.frames, t)
                return self.OrientPlaneTowardsCurveShorterEnd(self.centreCurve, plane, t)
            
            def GetPlaneOrientedToShorterSide_by_p(self, p):
                plane = self.GetPlane_by_point(self.centreCurve, self.frames, p)
                cp = self.GetCP(p)
                return self.OrientPlaneTowardsCurveShorterEnd(self.centreCurve, plane, cp)
            
            def GetPlanes(self):
                planes = []
                for frame in self.frames:
                    planes.append(frame.Item2)
                return planes
            
            def GetSidePlanes(self, t, XZ_YZ=True):
                return self.centreCurve.GetSidePlanes(self.frames, self.radii, t, XZ_YZ)
            
            def GetXAxis(self, p):
                plane = self.GetPlane_by_point(self.centreCurve, self.frames, p)
                return plane.XAxis
            
            def GetXAxis(self, t):
                plane = self.GetPlane_by_parameter(self.centreCurve, self.frames, t)
                return plane.XAxis
            
            def GetYAxis(self, p):
                plane = self.GetPlane_by_point(self.centreCurve, self.frames, p)
                return plane.YAxis
            
            def GetYAxis(self, t):
                plane = self.GetPlane_by_parameter(self.centreCurve, self.frames, t)
                return plane.YAxis
            
            def GetZAxis(self, p):
                plane = self.GetPlane_by_point(self.centreCurve, self.frames, p)
                return plane.ZAxis
            
            def GetZAxis(self, t):
                plane = self.GetPlane_by_parameter(self.centreCurve, self.frames, t)
                return plane.ZAxis
            
            def RemapNumbers(self, speed=50, low1=0, high1=2000, low2=0, high2=1):
                return low2 + (speed - low1) * (high2 - low2) / (high1 - low1)
            
            def Interpolate(self, values, parameter):
                def SampleLinear(v0, v1, t):
                    return v0 * (1 - t) + v1 * t
            
                count = len(values) - 1
                num2 = parameter * count
                num3 = int(math.floor(num2))
                if num3 < 0:
                    item = values[0]
                elif num3 >= count:
                    item = values[count]
                else:
                    item3 = values[num3]
                    item4 = values[num3 + 1]
                    item = SampleLinear(item3, item4, num2 - num3)
                return item
            
            def FramesAround(self, centerCurve, frames, t):
                if len(frames) < 1:
                    return None
                num = 0
                num1 = 1
                count = len(frames) - 1
                if t <= frames[0][0]:
                    return (frames[0][1], frames[0][1], 0)
                if t > frames[count][0]:
                    return (frames[count][1], frames[count][1], 1)
                num2 = 1
                while num2 < len(frames):
                    if frames[num2][0] < t:
                        num2 += 1
                    else:
                        num1 = num2
                        break
                num = (t - frames[num1 - 1][0]) / (frames[num1][0] - frames[num1 - 1][0])
                return (frames[num1 - 1][1], frames[num1][1], num)
            
            def InterpolatePlanes(self, A, B, t):
                origin = self.Lerp(A.Origin, B.Origin, t)
                xAxis = self.Lerp(A.XAxis, B.XAxis, t)
                yAxis = self.Lerp(A.YAxis, B.YAxis, t)
                return Plane(origin, xAxis, yAxis)
            
            def Lerp(self, vA, vB, t):
                return vA + (t * (vB - vA))
                
            
            def GetPlane_CreateOffsetCurve(self, centerCurve, frames, t):
                tuple = self.FramesAround(centerCurve, frames, t)
                plane = self.InterpolatePlanes(tuple[0], tuple[1], tuple[2])
                plane.Origin = centerCurve.PointAt(t)
                plane.Transform(Transform.Rotation(plane.ZAxis, centerCurve.TangentAt(t), plane.Origin))
                return plane
            
            def FlipAndRotate(self, p):
                plane = Plane(p)
                plane.Flip()
                plane.Rotate(1.5707963267949, plane.ZAxis)
                return plane
            
            def CreateOffsetCurve(self, centreCurve, radii, frames, samples, rebuild=False, rebuild_pts=20, signX=1, signY=1):
                point3ds = []
                numArray = centreCurve.DivideByCount(samples, True)
                for i in range(len(numArray)):
                    num = self.RemapNumbers(numArray[i], 0, numArray[-1], 0, 1)
                    num1 = self.Interpolate(radii, num)
                    plane = self.GetPlane_CreateOffsetCurve(centreCurve, frames, numArray[i])
                    point3ds.append((plane.Origin + (plane.XAxis * float(signX) * num1)) + (plane.YAxis * float(signY) * num1))
                curve = Curve.CreateInterpolatedCurve(point3ds, 3, 0, centreCurve.TangentAtStart, centreCurve.TangentAtEnd)
                if curve is None:
                    raise Exception("FreeformGlulam::CreateOffsetCurve:: Failed to create interpolated curve!")
                curve.Domain = Interval(0, curve.GetLength())
                if rebuild:
                    curve = curve.Rebuild(rebuild_pts, curve.Degree, True)
                return curve
            """
            def CreateOffsetCurve(self, centreCurve, frames, samples, x, y, rebuild=False, rebuild_pts=20):
                point3ds = []
                numArray = centreCurve.DivideByCount(samples, True)
                for i in range(len(numArray)):
                    plane = centreCurve.GetPlane(frames, numArray[i])
                    point3ds.append((plane.Origin + (plane.XAxis * x)) + (plane.YAxis * y))
                curve = rg.Curve.CreateInterpolatedCurve(point3ds, 3, 0, centreCurve.TangentAtStart, centreCurve.TangentAtEnd)
                if curve is None:
                    raise Exception("FreeformGlulam::CreateOffsetCurve:: Failed to create interpolated curve!")
                curve.Domain = rg.Interval(0, curve.GetLength())
                if rebuild:
                    curve = curve.Rebuild(rebuild_pts, curve.Degree, True)
                return curve
            """
            def CreateOffsetCurve4(self, centreCurve, radii, frames, samples, rebuild=False, rebuild_pts=20):
                curve1 = self.CreateOffsetCurve(self.centreCurve, radii, frames, samples, rebuild, rebuild_pts, 1, 1)
                curve2 = self.CreateOffsetCurve(self.centreCurve, radii, frames, samples, rebuild, rebuild_pts, -1, 1)
                curve3 = self.CreateOffsetCurve(self.centreCurve, radii, frames, samples, rebuild, rebuild_pts, -1, -1)
                curve4 = self.CreateOffsetCurve(self.centreCurve, radii, frames, samples, rebuild, rebuild_pts, 1, -1)
                return [curve1, curve2, curve3, curve4]
            
            def OffsetCurve(self, x, y, rebuild=False):
                return self.CreateOffsetCurve(self.centreCurve, self.frames, self.samples, x, y, rebuild, self.samples)
            
            def OffsetCurve(self, rebuild=False):
                return self.CreateOffsetCurve(self.centreCurve, self.radii, self.frames, self.samples, rebuild, self.samples, 1, 1)
            
            def OffsetCurve4(self, rebuild=False):
                return self.CreateOffsetCurve4(self.centreCurve, self.radii, self.frames, self.samples, rebuild, self.samples)
            
            def SortFrames(self):
                self.frames.sort(key=lambda x: x[0])
            
            def ToString(self):
                length = round(self.centreCurve.GetLength(), 3)
                num_cuts = len(self.cuts)
                return "Beam ID: {self.id} Length: {length} Cuts: {num_cuts}"
            
            def Transform(self, xform):
                self.centreCurve.Transform(xform)
                self.centrePline.Transform(xform)
                for i in range(len(self.frames)):
                    plane = rg.Plane(self.frames[i].Item2)
                    plane.Transform(xform)
                    self.frames[i] = (self.frames[i].Item1, rg.Plane(plane))
                for circle in self.circles:
                    circle.Transform(xform)
                for polygon in self.polygons:
                    polygon.Transform(xform)
                for cut in self.cuts:
                    cut.Transform(xform)
            
            
        class Element:
            
            key = None
            fID = None
            eID = []
            cuts = []
            outlinesCenter = Outline()
            outlines = []
            outlinesBackUp = []
            plGroup = Plane.Unset
            bbox = BoundingBox()
            bboxAligned = None
            mesh = None
            d = Line.Unset
            ev = []
            beamOrPlate = False
            w = 0
            
            
            def __init__(self, fID, v, p0_, p1_, plGroup, eID=None, key=""):
                self.key = None
                self.fID = None
                self.eID = []
                self.cuts = []
                self.outlinesCenter = Outline()
                self.outlines = []
                self.outlinesBackUp = []
                self.plGroup = Plane.Unset
                self.bbox = BoundingBox()
                self.bboxAligned = None
                self.mesh = None
                self.d = Line.Unset
                self.ev = []
                self.beamOrPlate = False
                self.w = 0
                
                if(isinstance(p0_, Polyline)):
                    p0 = [p0_]
                    p1 = [p1_]
                
                
                
                self.beamOrPlate = True if v.IsValid and v != Line.Unset else False
                self.beamOrPlate = False  # Overriding previous assignment
                self.fID = fID
                self.key = str(fID) if key == "" else key
                segmentCount = sum(len(polyline) for polyline in p0)
                self.eID = eID if eID is not None else [-1] * segmentCount
                self.d = Line(v.From, v.To)
                outline = Outline(p0, OutlineType.Top)
                outline1 = Outline(p1, OutlineType.Top)
                outline1.plane = self.FlipAndRotate(outline1.plane)
                
                
                
                plane = self.Translation(outline.plane,outline.plane.ZAxis * 0.01)
                plane1 = self.Translation(outline1.plane,outline1.plane.ZAxis * 0.01)
                if plane.Origin.DistanceToSquared(plane1.Origin) < outline.plane.Origin.DistanceToSquared(
                        outline1.plane.Origin):
                    outline.FlipPlane(outline1)
                outline.CheckPolygonOrientation(outline1)
                self.outlines.append(outline)
                self.outlines.append(outline1)
                polylines = []
                
                for j in range(len(p0)):
                    polylines.append(self.tweenPolylines(outline.plines[j], outline1.plines[j], 0.5))
                self.outlinesCenter = Outline(polylines, OutlineType.Center)
                self.outlinesCenter.plane = Plane(
                    (outline.plane.Origin + outline1.plane.Origin) * 0.5,
                    outline.plane.XAxis,
                    outline.plane.YAxis
                )
                
                
                
                for k in range(len(p0)):
                    numArray = [0] * (len(p0[k]) - 1)
                    array = list(range(len(p0[k]) - 1))
                    for l in range(len(p0[k]) - 1):
                        polyline = Polyline()
                        polyline.Add(outline.plines[k][l + 1])
                        polyline.Add(outline.plines[k][l])
                        polyline.Add(outline1.plines[k][l])
                        polyline.Add(outline1.plines[k][l + 1])
                        polyline.Add(outline.plines[k][l + 1])
                        outline2 = Outline(polyline, OutlineType.Side)
                        outline2.plane = Plane(
                            outline2.plane.Origin,
                            outline.plines[k][l + 1] - outline.plines[k][l],
                            outline.plines[k][l] - outline1.plines[k][l]
                        )
                        self.outlines.append(outline2)
                        self.ev.append(Vector3d.Unset)
                        if self.beamOrPlate:
                            numArray[l] = 999999
                            num = 0
                            if Intersection.LinePlane(self.d, outline2.plane, num) and -0.001 < num < 1.0001:
                                numArray[l] = 0
                
                    if self.beamOrPlate:
                        Array.Sort(numArray, array)
                        for m in range(len(p0[k]) - 1):
                            if m >= 2:
                                self.outlines[array[m] + 2].name = OutlineType.Top
                            else:
                                self.outlines[array[m] + 2].name = OutlineType.Side
                
                point3ds = []
                for polyline1 in p0:
                    point3ds.extend(polyline1)
                
                for polyline2 in p1:
                    point3ds.extend(polyline2)
                
                self.bbox = BoundingBox(point3ds)
                self.bbox.Inflate(0.01)
                self.bboxAligned = self.GetBoundingBoxAligned(point3ds, self.outlines[0].plane, 0.01)
                
                for outline3 in self.outlines:
                    self.outlinesBackUp.append(outline3.Duplicate())
                
                self.plGroup = self.outlines[0].plane if plGroup == Plane.Unset else Plane(plGroup)
                
                if self.d != Line.Unset:
                    self.plGroup = self.ToPlane(self.d,False)
                    self.plGroup = Plane(self.d.PointAt(0.5),self.plGroup.XAxis, self.plGroup.YAxis) 
                    self.plGroup = self.AlignPlane(self.plGroup, Vector3d.ZAxis)
                
                if self.d == Line.Unset:
                    origin = self.outlines[0].plane.Origin
                    self.w = origin.DistanceTo(self.outlines[1].plane.Origin)
                    return
                
                origin = self.outlines[0].plane.ClosestPoint(self.d.From)
                num1 = origin.DistanceTo(self.outlines[1].plane.ClosestPoint(self.d.From))
                num2 = num1
                self.w = num1
                num3 = num2
                origin = self.outlines[0].plane.ClosestPoint(self.d.To)
                num4 = origin.DistanceTo(self.outlines[1].plane.ClosestPoint(self.d.To))
                num2 = num4
                self.w = num4
                self.w = max(num3, num2)
            
            def ToPlane(self, line, start_or_end):
                if start_or_end:
                    return Plane(line.From, line.Direction)
                return Plane(line.To, line.Direction)
            
            def AlignPlane(self, plane, v):
                plane1 = Plane(plane)
                num = 0.0
                num1 = 0.0
                result, num, num1 = plane1.ClosestParameter(plane1.Origin + v)
                num2 = math.atan2(num, num1)
                plane1.Rotate(-num2 + 1.5707963267949, plane1.ZAxis, plane1.Origin)
                return plane1
            
            def GetBoundingBoxAligned(self, pts, plane, inflate=0):
                transform = Rhino.Geometry.Transform.ChangeBasis(Rhino.Geometry.Plane.WorldXY, plane)
                boundingBox = Rhino.Geometry.BoundingBox(pts, transform)
                if inflate > 0:
                    boundingBox.Inflate(inflate)
                min_point = boundingBox.Min
                x = min_point.X
                max_point = boundingBox.Max
                x_interval = Rhino.Geometry.Interval(x, max_point.X)
                y = min_point.Y
                y_interval = Rhino.Geometry.Interval(y, max_point.Y)
                z = min_point.Z
                z_interval = Rhino.Geometry.Interval(z, max_point.Z)
                return Rhino.Geometry.Box(plane, x_interval, y_interval, z_interval)
            
            def tweenPolylines(self, l0, l1, t=0.5):
                def Lerp(start, end, t):
                    return (1 - t) * start + t * end
                
                polyline = Polyline(l0)
                for i in range(l0.Count):
                    polyline[i] = Lerp(l0[i], l1[i], t)
                return polyline
            
            def FlipAndRotate(self, p):
                plane = Plane(p)
                plane.Flip()
                plane.Rotate(1.5707963267949, plane.ZAxis)
                return plane
            
            def Translation(self, plane, v):
                plane1 = Plane(plane)
                plane1.Translate(v)
                return plane1
            
            def Orient(self, targetPlane):
                self.Transform(Rhino.Geometry.Transform.PlaneToPlane(self.plGroup, targetPlane))
            
            def Transform(self, x):
                self.d.Transform(x)
                for i in range(len(self.outlines)):
                    self.outlines[i].Transform(x)
                for j in range(len(self.outlinesBackUp)):
                    self.outlinesBackUp[j].Transform(x)
                self.outlinesCenter.Transform(x)
                for k in range(len(self.cuts)):
                    self.cuts[k].Transform(x)
                for l in range(len(self.ev)):
                    if self.ev[l] != Rhino.Geometry.Vector3d.Unset:
                        self.ev[l].Transform(x)
                self.plGroup.Transform(x)
                self.bbox.Transform(x)
                self.bboxAligned.Transform(x)
                for m in range(len(self.ev)):
                    self.ev[m].Transform(x)
                if self.mesh is not None:
                    self.mesh.Transform(x)
            
            def KeyToIntArray(self):
                strArrays = self.key.split(';')
                numArray = [int(num) for num in strArrays]
                return numArray
            
        class Elements:
            
            eSL = OrderedDict()
            r = []
            
            def __init__(self, listElements=None):
                
                
                if listElements is not None:
                    for listElement in listElements:
                        self.Add(listElement)
            
            def __getitem__(self, index):
                return self.eSL.values()[index]
            
            def __setitem__(self, index, value):
                self.eSL.values()[index] = value
            
            def Add(self, item):
                key = item.key
                self.eSL[key] = item
            
            def Duplicate(self):
                elements = Elements()
                for key, value in self.eSL.items():
                    elements.Add(value.Duplicate())
                for geometryBase in self.r:
                    elements.r.append(geometryBase.Duplicate())
                return elements
            
            def GetEdgeDirections(self, lines):
                num = 0
                for direction in self.eSL.values():
                    value = direction.bbox
                    for i in range(len(lines)):
                        if value.Contains(lines[i].get_From()) or not value.Contains(lines[i].get_To()):
                            for j in range(2, len(direction.outlines)):
                                item = lines[i]
                                if not direction.outlines[j].bbox.Contains(item.get_From()):
                                    item = lines[i]
                                    if not direction.outlines[j].bbox.Contains(item.get_To()):
                                        continue
                                item = lines[i]
                                if abs(direction.outlines[j].plane.DistanceTo(item.get_From())) >= 0.0001:
                                    item = lines[i]
                                    if abs(direction.outlines[j].plane.DistanceTo(item.get_To())) >= 0.0001:
                                        continue
                                if direction.outlines[0].plines[0].ClosestPoint(lines[i].get_From()).DistanceTo(lines[i].get_From()) < 0.001 or direction.outlines[1].plines[0].ClosestPoint(lines[i].get_From()).DistanceTo(lines[i].get_From()) < 0.001:
                                    line = lines[i]
                                    direction.ev[j - 2] = line.get_Direction()
                    num += 1
            
            def GetElementDirections(self, lines):
                try:
                    num = 0
                    for keyValuePair in self.eSL.items():
                        value = keyValuePair.Value.bbox
                        for i in range(len(lines)):
                            if value.Contains(lines[i].get_From()) or not value.Contains(lines[i].get_To()):
                                num1 = 0
                                while True:
                                    if num1 >= 2:
                                        break
                                    item = lines[i]
                                    if not keyValuePair.Value.outlines[num1].bbox.Contains(item.get_From()):
                                        item = lines[i]
                                        if not keyValuePair.Value.outlines[num1].bbox.Contains(item.get_To()):
                                            continue
                                    item = lines[i]
                                    if abs(keyValuePair.Value.outlines[num1].plane.DistanceTo(item.get_From())) < 0.01:
                                        break
                                    item = lines[i]
                                    if abs(keyValuePair.Value.outlines[num1].plane.DistanceTo(item.get_To())) < 0.01:
                                        break
                                    num1 += 1
                                line = lines[i]
                                line.Transform(Transform.PlanarProjection(keyValuePair.Value.outlines[0].plane))
                                for j in range(2, len(keyValuePair.Value.outlines)):
                                    direction = line.get_Direction()
                                    origin = keyValuePair.Value.outlines[j].plane.get_Origin()
                                    if keyValuePair.Value.outlinesCenter.plane.get_Origin().DistanceToSquared(origin + (direction * 1)) < keyValuePair.Value.outlinesCenter.plane.get_Origin().DistanceToSquared(origin - (direction * 1)):
                                        direction.Reverse()
                                    keyValuePair.Value.ev[j - 2] = direction
                        num += 1
                except Exception as exception:
                    Debug.Log(str(exception), True)
            
            def GetEnumerator(self):
                return self.eSL.GetEnumerator()
            
            def GetList(self):
                return list(self.eSL.values())
            
            def GetOutlinePlane(self, f, e):
                return self.eSL[f].outlines[e].plane
            
            def GetOutlinePlaneX(self, f, e):
                return self.eSL[f].outlines[e].plane.get_XAxis()
            
            def GetOutlinePlaneY(self, f, e):
                return self.eSL[f].outlines[e].plane.get_YAxis()
            
            def GetOutlinePlaneZ(self, f, e):
                return self.eSL[f].outlines[e].plane.get_ZAxis()
            
            def GetOutlinePlines(self, f, e):
                return self.eSL[f].outlines[e].plines
            
            def GetOutlineType(self, f, e):
                return self.eSL[f].outlines[e].name
            
            def Partition(self, level=0, addRefGeometry=True):
                elements = []
                if len(self.eSL) == 0:
                    return elements
                elements1 = Elements()
                num = -10
                flag = True
                for key, value in self.eSL.items():
                    intArray = value.KeyToIntArray()
                    if num != intArray and not flag:
                        elements.append(elements1)
                        elements1 = Elements()
                    elements1.Add(value)
                    if len(self.r) > intArray and addRefGeometry:
                        elements1.r.append(self.r[intArray])
                    num = intArray
                    flag = False
                elements.append(elements1)
                return elements
            
            def GetEnumerator(self):
                return self.Partition()
            
            def ToString(self):
                str = str(len(self.eSL))
                count = len(self.r)
                return "Elements " + str + " Ref " + str(count)
            
            def Transform(self, xform):
                for keyValuePair in self.eSL:
                    keyValuePair.Value.Transform(xform)
                for geometryBase in self.r:
                    geometryBase.Transform(xform)
        
        
        class ElementsFromCCX:
            
            adjacency = []
            elements = Elements() # class for storage
            beams = None
            JointsPts = []
            CollisionPairs = set()
            id = []
            idParams = []
            closestDistanceTol = 400.0
            parallelTol = 0.2
            lengthOfBoxes = 300.0
            elementCount = 0
            halfLap = False    
            
            def __init__ (self, axis_, C_, rebuild, indices, componentPlanes, adjustLines, closestDistanceTol, parallelTol, lengthOfBoxes, jointsPoints, halfLap):
                polyline = None
                count = len(axis_)
                self.beams = [None] * count
                self.closestDistanceTol = closestDistanceTol
                self.parallelTol = parallelTol
                self.lengthOfBoxes = lengthOfBoxes
                self.elementCount = 0
                self.JointsPts = jointsPoints
                self.elements = Elements() # class for storage
                self.halfLap = halfLap
                
                for i in range(count):
                    result, polyline = axis_[i].TryGetPolyline()
                    if polyline[0].DistanceToSquared(C_[i][0].Center) > polyline[0].DistanceToSquared(C_[i][-1].Center):
                        C_[i].Reverse()
                    
                    line = adjustLines[i] if count == len(adjustLines) else Line.Unset
                    num = indices[i] if len(indices) == count else i
                    
                    nums = []
                    planes = []
                    for j in range(len(C_[i]) - 1):
                        item = C_[i][j]
                        nums.append(item.Radius)
                        item = C_[i][j]
                        planes.append(item.Plane)
                    
                    self.beams[i] = Beam(num, polyline, nums, line, planes, rebuild, rebuild > 0)
                
                self.GetNeighbours()
                
                num1 = 0
                num2 = -1
                for element in self.elements:
                    intArray = element.KeyToIntArray()[0]
                    if num2 == intArray:
                        continue
                    num1 += 1
                    num2 = intArray
                
                if len(indices) != len(componentPlanes) or len(indices) + len(componentPlanes) <= 0:
                    if num1 == len(componentPlanes):
                        for keyValuePair in self.elements:
                            intArray1 = keyValuePair.KeyToIntArray()[0]
                            keyValuePair.Value.plGroup = componentPlanes[intArray1]
                    return
                
                nums1 = set()
                nums2 = {}
                for k in range(len(indices)):
                    if indices[k] not in nums1:
                        nums1.add(indices[k])
                        nums2[indices[k]] = componentPlanes[k]
                
                for l in range(len(self.elements)):
                    numArray = self.elements[l].KeyToIntArray()
                    self.elements[l].plGroup = nums2[numArray[0]]
            
            
            def MovePlanebyAxis(self, plane, dist, axis=2):
                plane1 = Plane(plane)
                if axis == 0:
                    plane1.Translate(plane1.XAxis * dist)
                elif axis == 1:
                    plane1.Translate(plane1.YAxis * dist)
                else:
                    plane1.Translate(plane1.Normal * dist)
                return plane1
            
            def OrientPlaneTowardsCurveShorterEnd(self, curve, plane, t, message):
                
                
                
                plane1 = Plane(plane)
                
                # which side is shorter
                p = curve.PointAt(t)
                copy_curve = curve.Duplicate()
                copy_curve = copy_curve.Extend(Rhino.Geometry.CurveEnd.Both, copy_curve.GetLength(0.01)*0.1, Rhino.Geometry.CurveExtensionStyle.Smooth )
                copy_curve.Domain = Interval(0,1)
                
                result, cpt = copy_curve.ClosestPoint(p)
                
                domain0 = Interval(copy_curve.Domain.T0, cpt)
                domain1 = Interval(cpt, copy_curve.Domain.T1)
                
                
                
                # get middle point of that side
                orientation_point = copy_curve.PointAt(domain0.Mid)
                if(domain1.Length>domain0.Length):
                    orientation_point = copy_curve.PointAt(domain1.Mid)
                    
                # orient the plane based on the distance search
                distance = plane.DistanceTo(orientation_point)  
                #print(distance) 
                #Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(orientation_point)
                if(distance<0):
                    plane1 = self.FlipAndRotate(plane1)
                
                #Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(Rectangle3d(plane1,10,10).ToPolyline())
                return plane1
                """
                num = 0
                plane1 = Plane(plane)
                interval = [None] * 2
                domain = curve.Domain
                interval[0] = Interval(domain.T0 - 1000, t)
                domain = curve.Domain
                interval[1] = Interval(t, domain.T1 + 1000)
                intervalArray = interval
                num1 = 0 if intervalArray[0].Length < intervalArray[1].Length else 1
                curve.ClosestPoint(plane.Origin + plane.ZAxis, num)
                if not intervalArray[num1].IncludesParameter(num):
                    plane1 = self.FlipAndRotate(plane1)
                return plane1
                """
            
            def AlignPlane(self, A, B):
                origin = B.Origin
                xAxis = B.XAxis
                yAxis = B.YAxis
                zAxis = B.ZAxis
            
                switch_value = A.ZAxis.IsParallelTo(B.ZAxis, 1.539380400259)
                if switch_value == -1:
                    point3d = B.ClosestPoint(origin + A.XAxis)
                    xAxis = origin - point3d
                    xAxis.Unitize()
                elif switch_value == 0:
                    transform = Rhino.Geometry.Transform.Rotation(A.ZAxis, B.ZAxis, B.Origin)
                    xAxis = A.XAxis
                    xAxis.Transform(transform)
                elif switch_value == 1:
                    xAxis = B.ClosestPoint(origin + A.XAxis) - origin
                    xAxis.Unitize()
            
                return Plane(origin, xAxis, Vector3d.CrossProduct(zAxis, xAxis))
            
            """
            def AlignPlane(self, plane, v):
                plane1 = Plane(plane)
                num = 0.0
                num1 = 0.0
                result, num, num1 = plane1.ClosestParameter(plane1.Origin + v)
                num2 = math.atan2(num, num1)
                plane1.Rotate(-num2 + 1.5707963267949, plane1.ZAxis, plane1.Origin)
                return plane1
            """
            
            def GetMale(self, C0, C1, T0 = -1, T1 = -1):
                t0 = None
                t1 = None
            
                if T0 != -1 or T1 != -1:
                    t0 = T0
                    t1 = T1
                else:
                    curveIntersection = Intersection.CurveCurve(C0, C1, 1000, 0.01)
                    t0 = curveIntersection[0].ParameterA
                    t1 = curveIntersection[0].ParameterB
            
                domain = C0.Domain
                domain = Rhino.Geometry.Interval(domain.T0, t0)
                length = domain.Length
                domain = C0.Domain
                domain = Rhino.Geometry.Interval(t0, domain.T1)
                num = domain.Length
            
                domain = C1.Domain
                domain = Rhino.Geometry.Interval(domain.T0, t1)
                length1 = domain.Length
                domain = C1.Domain
                domain = Rhino.Geometry.Interval(t1, domain.T1)
                num1 = domain.Length
            
                if abs(length - num) / C0.Domain.Length <= abs(length1 - num1) / C1.Domain.Length:
                    return 1
                return 0
            
            def MovePlanebyAxis(self, plane, dist, axis=2):
                plane1 = Plane(plane)
                if axis == 0:
                    plane1.Translate(plane1.XAxis * dist)
                elif axis == 1:
                    plane1.Translate(plane1.YAxis * dist)
                else:
                    plane1.Translate(plane1.Normal * dist)
                return plane1
            
            def AverageNormal(self, p):
                count = p.Count - 1
                vector3d = Vector3d(0,0,0)
                num = count - 1
                for i in range(num + 1):
                    num1 = (i - 1 + count) % count
                    num2 = (i + 1 + count) % count
                    item = p[num1]
                    point3d = p[num2]
                    item1 = p[i]
                    vector3d += Vector3d.CrossProduct(Vector3d(item1 - item), Vector3d(point3d - item1))
                
                if vector3d.X == 0 and vector3d.Y == 0 and vector3d.Z == 0:
                    vector3d.Unitize()
                
                return vector3d
            
            def GetPlane(self, polyline, AveragePlane=True):
                plane = Plane.Unset
                if not AveragePlane:
                    Plane.FitPlaneToPoints(polyline, plane)
                    plane.Origin = polyline.CenterPoint()
                    return plane
                
                line = polyline.SegmentAt(0)
                vector3d = line.Direction
                vector3d.Unitize()
                vector3d1 = self.AverageNormal(polyline)
                vector3d1.Unitize()
                vector3d2 = Vector3d.CrossProduct(vector3d, vector3d1)
                return Plane(polyline.CenterPoint(), vector3d, vector3d2)
            
            def FlipAndRotate(self, p):
                plane = Plane(p)
                plane.Flip()
                plane.Rotate(1.5707963267949, plane.ZAxis)
                return plane
            
            def ShiftPline(self, a, n=1):
                def Wrap(index, n):
                    if n == 0:
                        return 0
                    return (index % n + n) % n
            
                polyline = Rhino.Geometry.Polyline(a)
                polyline.RemoveAt(polyline.Count - 1)
                num = Wrap(n, polyline.Count)
                for i in range(num):
                    count = polyline.Count
                    item = polyline[count - 1]
                    for j in range(count - 1, 0, -1):
                        polyline[j] = polyline[j - 1]
                    polyline[0] = item
                polyline.Add(polyline[0])
                return polyline
            
            def SplitCurvesByPlane(self, pline, plane):
                num = 0
                polyline = None
                polyline1 = Polyline(pline)
                nurbsCurve = polyline1.ToNurbsCurve()
                nums = []
                
                for i in range(polyline1.SegmentCount):
                    result, num = Rhino.Geometry.Intersect.Intersection.LinePlane(polyline1.SegmentAt(i), plane)
                    if result:
                        if num > -0.999999999 and num < 1.0000001:
                            nums.append(float(i) + num)
                
                array = sorted(nums)
                nurbsCurve = polyline1.ToNurbsCurve()
                curveArray = None
                if len(array) != 2:
                    curveArray = nurbsCurve.Split(array)
                else:
                    curveArray = [nurbsCurve.Trim(array[0], array[1]), nurbsCurve.Trim(array[1], array[0])]
                
                nums1 = SortedDictionary[int, Polyline]()
                nums2 = SortedDictionary[int, Polyline]()
                plane1 = Plane(plane)
                plane1.Translate(plane.ZAxis * 0.01)
                num1 = 0
                for curve in curveArray:
                    result, polyline = curve.TryGetPolyline()
                    if result:
                        
                        #Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(curve.PointAtNormalizedLength(0.5))
                        # check which polygon is on the plane
                        # if this assumtion will brake in the future, thake the point that is closest to the plane by measuring the absolute point to plane distance
                        if plane.DistanceTo(curve.PointAtNormalizedLength(0.5)) < 0.01 and plane.DistanceTo(curve.PointAtNormalizedLength(0.5)) > -0.01:
                            nums2.Add(num1, polyline)
                        else:
                            nums1.Add(num1, polyline)
                    num1 += 1
                
                return tuple([nums1, nums2])
            
            
            def GetNeighbours(self):
                bounding_boxes = {}
                for i in range(len(self.beams)):
                    bounding_boxes[i] = self.beams[i].GetBoundingBox(0.001)
                
                rTree = RTree()
                for j in range(len(bounding_boxes)):
                    rTree.Insert(bounding_boxes[j], j)
                
                def SearchCallback(sender, e):
                    # Indexing
                    index1 = int(e.Tag) if isinstance(e.Tag, int) else None
                    a = index1 if index1 is not None and index1 < e.Id else e.Id
                    b = e.Id if index1 is not None and index1 < e.Id else index1
                    
                    # Check1: equal elements Check2: belongs to the same component Check3: pair is visited
                    if a == b:
                        return
                    if self.beams[a].id == self.beams[b].id:
                        return
                    
                    longKey = (a << 32) + b  # long key for faceID
                    newPair = longKey not in self.CollisionPairs
                    if newPair:
                        self.CollisionPairs.add(longKey)
                    else:
                        return
                    
                    
                    
                    # Proximity Search
                    # Get Curves for Proximity Search and CurveClosestPoints Parameters
                    c0 = self.beams[a].centreCurve
                    c1 = self.beams[b].centreCurve
                    minDistance = -1
                    mint0 = -1
                    mint1 = -1
                    
                    if len(self.JointsPts) != 0:
                        # If user gives intersection points
                        for i in range(len(self.JointsPts)):
                            result, t0 = c0.ClosestPoint(self.JointsPts[i], self.closestDistanceTol)
                            p0 = c0.PointAt(t0)
                            result, t1 = c1.ClosestPoint(self.JointsPts[i], self.closestDistanceTol)
                            p1 = c1.PointAt(t1)
                            dist = p0.DistanceTo(p1)
                    
                            if dist < closestDistanceTol:
                                minDistance = p0.DistanceTo(p1)
                                mint0 = t0
                                mint1 = t1
                                break
                    
                        if minDistance == -1:
                            return
                    
                    elif len(self.JointsPts) == 0 or minDistance == -1:
                        # Else ClosestPoints method is called between two curves
                        result, p0, p1, = c0.ClosestPoints(c1)
                        result, mint0 = c0.ClosestPoint(p0)
                        result, mint1 = c1.ClosestPoint(p1)
                        minDistance = p0.DistanceTo(p1)
                    
                    if not (minDistance < self.closestDistanceTol):
                        return
                
                    mPoint = c0.PointAt(mint0)
                    fPoint = c1.PointAt(mint1)
                    mID = a
                    fID = b
                    mT = mint0
                    fT = mint1
                    plane0 = self.beams[a].GetPlane_by_t(mint0)
                    plane1 = self.beams[b].GetPlane_by_t(mint1)
                    self.id.append((a, b))
                    self.idParams.append((mint0, mint1))
                    
                    if plane0.ZAxis.IsParallelTo(plane1.ZAxis, self.parallelTol) == 0:
                        # Non-Parallel case for Side-to-End and Side-Side Joints
                    
                        # Firstly, it is necessary to identify which beam is male and female from BeamJoint curve parameters.
                        # The male beam will have an intersection point close to one of the line ends, and the female will be located vice-versa.
                    
                        m = self.GetMale(self.beams[mID].centreCurve, self.beams[fID].centreCurve, mT, fT)
                        f = abs(m - 1)  # female will have a connection close to the center
                    
                        mID = [a, b][m]  # male beam id 0 -> 58
                        fID = [a, b][f]  # female beam id 1 -> 102
                    
                        mT = [mint0, mint1][m]
                        fT = [mint0, mint1][f]
                    
                        mPoint = self.beams[mID].centreCurve.PointAt(mT)
                        fPoint = self.beams[fID].centreCurve.PointAt(fT)
                    
                        mRadius = self.beams[mID].GetInterpolatedRadius_by_parameter(mT)
                        fRadius = self.beams[fID].GetInterpolatedRadius_by_parameter(fT)
                    
                        mPlane = self.beams[mID].GetPlane_by_t(mT)
                        fPlane = self.beams[fID].GetPlane_by_t(fT)
                    
                        # Align plane orientation
                    
                        cross = Rhino.Geometry.Vector3d.CrossProduct(mPlane.ZAxis, fPlane.ZAxis)  # YAxis
                    
                        angle0 = Rhino.Geometry.Vector3d.VectorAngle(mPlane.ZAxis, fPlane.ZAxis, cross)
                        angle1 = Rhino.Geometry.Vector3d.VectorAngle(-mPlane.ZAxis, fPlane.ZAxis, cross)
                    
                        angleMin = min(angle0, angle1)
                    
                        angleLimit = Rhino.RhinoMath.ToRadians(30)  # Math.PI * 0.25 = 45
                    
                        if angleMin < angleLimit:  # 45
                            fixAngle = angleLimit - angleMin
                            fixAngle *= -1
                            if angle1 < angle0:
                                mPlane.Rotate(-fixAngle, cross, mPlane.Origin)
                            else:
                                mPlane.Rotate(fixAngle, cross, mPlane.Origin)
                    
                        ci = Rhino.Geometry.Intersect.Intersection.CurvePlane(self.beams[mID].centreCurve, mPlane, 0.1)
                        mPlane = Plane(ci[0].PointA,mPlane.XAxis, mPlane.YAxis)
                        cross = Rhino.Geometry.Vector3d.CrossProduct(mPlane.ZAxis, fPlane.ZAxis)
                    
                        fp = fPlane.Origin
                        fXAxis = fPlane.XAxis
                        mPlane = Rhino.Geometry.Plane(mPlane.Origin, Rhino.Geometry.Vector3d.CrossProduct(mPlane.ZAxis, cross), -cross)
                        fPlane = Rhino.Geometry.Plane(fPlane.Origin, Rhino.Geometry.Vector3d.CrossProduct(fPlane.ZAxis, cross), -cross)
                    
                        if self.halfLap:
                            mPlane.Origin = mPoint
                            fPlane.Origin = fPoint
                            mPlane.Rotate(math.pi * 0.5, mPlane.ZAxis)
                        else:
                            # Orient towards ends
                            #print(mT)
                            #Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(self.beams[mID].centreCurve.PointAt(mT))
                            
                            mPlane = self.beams[mID].GetPlaneOrientedToShorterSide_by_t(mT)
                            mPlane = self.FlipAndRotate(mPlane)
                            #Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(Rectangle3d(mPlane,1,1).ToPolyline())
                        
                        
                        
                        # Create Elements properties
                        scale = 1.0
                        extend0 = self.lengthOfBoxes * scale if self.halfLap else 0
                        extend1 = -self.lengthOfBoxes * scale if self.halfLap else self.lengthOfBoxes * 2 * scale
                        
                        mPlane0 = self.MovePlanebyAxis(mPlane, extend0)
                        mPlane1 = self.MovePlanebyAxis(mPlane, extend1)
                        r0 = Rectangle3d(mPlane0, Interval(-mRadius, mRadius), Interval(-mRadius, mRadius))
                        r1 = Rectangle3d(mPlane1, Interval(-mRadius, mRadius), Interval(-mRadius, mRadius))
                        axis0 = Line(r0.Center, r1.Center)
                        pline0m = Polyline([r0.Corner(0), r0.Corner(1), r1.Corner(1), r1.Corner(0), r0.Corner(0)])
                        pline1m = Polyline([r0.Corner(3), r0.Corner(2), r1.Corner(2), r1.Corner(3), r0.Corner(3)])
                        
                        fID0 = self.elementCount
                        key0 = str(self.beams[mID].id) + ";" + str(self.elementCount)
                        self.elementCount += 1
                        
                        fPlane0 = self.MovePlanebyAxis(fPlane, -self.lengthOfBoxes * scale)
                        fPlane1 = self.MovePlanebyAxis(fPlane, self.lengthOfBoxes * scale)
                        r0 = Rectangle3d(fPlane0, Interval(-fRadius, fRadius), Interval(-fRadius, fRadius))
                        r1 = Rectangle3d(fPlane1, Interval(-fRadius, fRadius), Interval(-fRadius, fRadius))
                        pline0f = Polyline([r0.Corner(0), r0.Corner(1), r1.Corner(1), r1.Corner(0), r0.Corner(0)])
                        pline1f = Polyline([r0.Corner(3), r0.Corner(2), r1.Corner(2), r1.Corner(3), r0.Corner(3)])
                        axis1 = Line(r0.Center, r1.Center)
                        pline2 = Polyline([r0.Corner(1), r0.Corner(2), r1.Corner(2), r1.Corner(1), r0.Corner(1)])
                        pline3 = Polyline([r0.Corner(0), r0.Corner(3), r1.Corner(3), r1.Corner(0), r0.Corner(0)])
                        fPlane2 = self.GetPlane(pline2)
                        fPlane3 = self.GetPlane(pline3)
                        
                        fID1 = self.elementCount
                        key1 = str(self.beams[fID].id) + ";" + str(self.elementCount)
                        self.elementCount += 1
                        
                        
                        
                        midP = (fPlane2.Origin + fPlane3.Origin) * 0.5
                        
                        # Orient Planes to outside of the center
                        if fPlane2.DistanceTo(midP)>0:
                            self.FlipAndRotate(fPlane2)
                        if fPlane3.DistanceTo(midP)>0:
                            self.FlipAndRotate(fPlane3)
                        
                        # Take nearest plane to center of male beam
                        fPlaneClosest = fPlane2 if mPlane1.Origin.DistanceToSquared(fPlane2.Origin) < mPlane1.Origin.DistanceToSquared(fPlane3.Origin) else fPlane3
                        
                        # Cut elements by nearest plane or do not cut if a cross-lap joint is needed
                        if not self.halfLap:
                            pline0m = next(iter( self.SplitCurvesByPlane(pline0m, fPlaneClosest)[0])).Value
                            pline1m = next(iter(self.SplitCurvesByPlane(pline1m, fPlaneClosest)[0])).Value
                            pline0m.Add(pline0m[0])
                            pline1m.Add(pline1m[0])
                            #Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(pline0m)
                            
                            
                        # Create Elements
                        e0 = Element(fID0, Line.Unset, pline0m, pline1m, self.GetPlane(pline0m), None, key0)  # new Line(r0.Center, r1.Center)
                        e1 = Element(fID1, Line.Unset, pline0f, pline1f, self.GetPlane(pline0f), None, key1)
                        
                        # Edge Vectors
                        e1.Transform(Rhino.Geometry.Transform.Rotation(math.pi * 0.5, r1.Center - r0.Center, r0.Center))
                        e0.d = axis0
                        e1.d = axis1
                        
                        
                        ###############################################################################################################################################
                        # Match Plane orientation and rebuild female element
                        ###############################################################################################################################################
                        pline0f = e1.outlines[0].plines[0]
                        pline1f = e1.outlines[1].plines[0]
                        orientationPoint = fp + fXAxis * pline0f.Length
                        line = Rhino.Geometry.Line(pline0f.ClosestPoint(orientationPoint), orientationPoint)
                        idf = int(math.floor(pline0f.ClosestParameter(orientationPoint)))
                        pline0f = self.ShiftPline(pline0f,-idf)
                        pline1f = self.ShiftPline(pline1f, -idf)
                        
                        e1 = Element(fID1, Rhino.Geometry.Line.Unset, pline0f, pline1f, self.GetPlane(pline0f), None, key1)
                        e1.beamOrPlate = True
                        e1.d = axis1
                        
                        # Get edgevectors
                        evM = []
                        for i in range(pline0m.SegmentCount):
                            if fPlaneClosest.DistanceTo(pline0m.SegmentAt(i).PointAt(0.5)) < 0.01:
                                evM.append(axis0.Direction)
                            else:
                                evM.append(Rhino.Geometry.Vector3d.Unset)
                        
                        e0.ev = evM
                        
                        ###############################################################################################################################################
                        # Output
                        ###############################################################################################################################################
                        self.elements.Add(e0)
                        self.elements.Add(e1)
                        #print("neighbors", e0.fID, e1.fID)
                        self.adjacency.append([e0.fID, e1.fID])
                    else:
                        
                        #region Parallel case for End-to-End Joints
                        
                        #region Get male and female plane
                        
                        midP = (mPoint + fPoint) * 0.5
                        mPlane = self.beams[mID].GetPlaneOrientedToShorterSide_by_t(mT)
                        fPlane = self.beams[fID].GetPlaneOrientedToShorterSide_by_t(fT)
                        mZAxis = mPlane.ZAxis
                        fZAxis = fPlane.ZAxis
                        mRadius = self.beams[mID].GetInterpolatedRadius_by_parameter(mT)
                        fRadius = self.beams[fID].GetInterpolatedRadius_by_parameter(fT)
                        
                        #endregion
                        
                        #region Get Bisector Plane
                        
                        isParallel = mZAxis.IsParallelTo(fZAxis, 0.01)
                        ZAxis = Vector3d.CrossProduct(fZAxis, mZAxis) if isParallel == 0 else Vector3d.ZAxis
                        XAxis = fZAxis + mZAxis if isParallel == 0 else Vector3d.CrossProduct(ZAxis, mZAxis)
                        YAxis = Vector3d.CrossProduct(ZAxis, XAxis)
                        bisector = Plane(midP, XAxis, ZAxis)
                        bisector = self.AlignPlane(mPlane, bisector)
                        
                        #endregion
                        
                        #region Orient Plane towards the longer ends
                        
                        fPlaneUser = self.OrientPlaneTowardsCurveShorterEnd(self.beams[mID].centreCurve.DuplicateCurve(), Plane(bisector), mT, "a")
                        mPlaneUser = self.OrientPlaneTowardsCurveShorterEnd(self.beams[fID].centreCurve.DuplicateCurve(), Plane(bisector), fT, "b")
                        #mPlaneUser = self.FlipAndRotate(mPlaneUser)
                        #Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(Rectangle3d(mPlaneUser,5,5).ToPolyline())
                        #Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(Rectangle3d(fPlaneUser,5,5).ToPolyline())
                        
                        #endregion
                        
                        #region Create Element
                        
                        r0 = Rectangle3d(mPlaneUser, Interval(-mRadius, mRadius), Interval(-mRadius, mRadius))
                        r1 = Rectangle3d(self.MovePlanebyAxis(mPlaneUser, self.lengthOfBoxes), Interval(-mRadius, mRadius), Interval(-mRadius, mRadius))
                        pline0 = Polyline([r0.Corner(0), r0.Corner(1), r1.Corner(1), r1.Corner(0), r0.Corner(0)])
                        pline1 = Polyline([r0.Corner(3), r0.Corner(2), r1.Corner(2), r1.Corner(3), r0.Corner(3)])
                        fID0 = self.elementCount
                        e0 = Element(fID0, Line(r0.Center, r1.Center), pline0, pline1, self.GetPlane(pline0), None, str(self.beams[a].id) + ";" + str(self.elementCount)) 
                        self.elementCount += 1
                        
                        r0 = Rectangle3d(fPlaneUser, Interval(-fRadius, fRadius), Interval(-fRadius, fRadius))
                        r1 = Rectangle3d(self.MovePlanebyAxis(fPlaneUser, self.lengthOfBoxes), Interval(-fRadius, fRadius), Interval(-fRadius, fRadius))
                        pline0 = Polyline([r0.Corner(0), r0.Corner(1), r1.Corner(1), r1.Corner(0), r0.Corner(0)])
                        pline1 = Polyline([r0.Corner(3), r0.Corner(2), r1.Corner(2), r1.Corner(3), r0.Corner(3)])
                        fID1 = self.elementCount
                        e1 = Element(fID1, Line(r0.Center, r1.Center), pline0, pline1, self.GetPlane(pline0), None, str(self.beams[b].id) + ";" + str(self.elementCount))
                        self.elementCount += 1
                        
                        #endregion
                        
                        #region Output
                        
                        self.elements.Add(e0)
                        self.elements.Add(e1)
                        self.adjacency.Add([e0.fID, e1.fID])
                        
                        #endregion
            
                        #endregion
        
                
                for k in range(rTree.Count - 1):
                    rTree.Search(bounding_boxes[k], SearchCallback, k)
            
        
        
        #####################################################################################
        # Create circles around the curves
        #####################################################################################
        
        #####################################################################################
        # Generate Elements
        #  (self, axis_, C_, rebuild, indices, componentPlanes, adjustLines, closestDistanceTol, parallelTol, lengthOfBoxes, jointsPoints, halfLap)
        #####################################################################################
        
        # create axes from circles
        list_axes_as_curves = []
        list_of_lists_circles = []
        
        for i in range(_circles.BranchCount):
            
            axis = Polyline()
            list_of_circles = []
            
            for j in range(_circles.Branch(i).Count):
                axis.Add(_circles.Branch(i)[j].Center)
                list_of_circles.append(_circles.Branch(i)[j])
            
            list_axes_as_curves.Add(axis.ToNurbsCurve())
            list_of_lists_circles.append(list_of_circles)
        
        # numeric parameters
        rebuild = 1
        indexing = [] # If none is added tree trunks will follow data tree order, for forks it repeat like this  0 0 0 1 1 ...
        distance_tolerance = _distance_tolerance if _distance_tolerance is not  None else 10
        parallel_tolerance =  _parallel_tolerance if _parallel_tolerance is not  None else 0.2
        length_of_box = _length_of_box if _length_of_box is not None else 1
        joint_points = [] # If points are given they are used for joint detection, else Curve.ClosestPoints method is used
        cross_lap = _cross_or_toptoside if  _cross_or_toptoside is not None else False # CrossLap - true, Side-End - false
        #print length_of_box
        # optional parameters
        planes = [] # Used only orientation, and most importantly for components such as forks or bent beams with multiple nodes
        lines = [] # adjust beam, reorient planes
        
        if(len(list_axes_as_curves)>0):
            
            elementsFromCCX = ElementsFromCCX(
                                                list_axes_as_curves, 
                                                list_of_lists_circles, 
                                                rebuild, 
                                                indexing, 
                                                planes, 
                                                lines, 
                                                distance_tolerance, 
                                                parallel_tolerance, 
                                                length_of_box, 
                                                joint_points, 
                                                cross_lap) 
            
            _pipes = DataTree[Brep]()
            if(_create_pipes):
                for i in range(_circles.BranchCount):
                    curves = []
                    for j in range (_circles.Branch(i).Count):
                        curves.append(_circles.Branch(i)[j].ToNurbsCurve())
                    
                    brep = Brep.CreateFromLoft(curves, Point3d.Unset, Point3d.Unset, Rhino.Geometry.LoftType.Loose, False)[0]
                    brep = brep.CapPlanarHoles(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
                    
                    faces = []
                    for j in range(brep.Faces.Count):
                        faces.append(brep.Faces[j].DuplicateFace(False))
                    
                    brep = Rhino.Geometry.Brep.JoinBreps(
                        faces, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance*10
                    )[0]
                    
                    if brep:
                        brep.Faces.SplitKinkyFaces(Rhino.RhinoMath.DefaultAngleTolerance, True)
                        
                        if (Rhino.Geometry.BrepSolidOrientation.Inward is  brep.SolidOrientation):
                          brep.Flip();
                    _pipes.Add(brep,GH_Path(i))
                
                
            #elements = elementsFromCCX.elements.Partition(0, True)
            
            _volumes = DataTree[Polyline]()
            _group_id = DataTree[int]()
            for key,item in elementsFromCCX.elements.eSL.items():
                ids = item.KeyToIntArray()
                #print(ids)
                path = GH_Path(ids[0],ids[1])
                _volumes.Add(item.outlines[0].plines[0],path)
                _volumes.Add(item.outlines[1].plines[0],path)
                _group_id.Add(ids[0],GH_Path(ids[0]));
            
            # renumerate
            total_count = 0
            for i in range(_group_id.BranchCount):
                for j in range(len(_group_id.Branch(i))):
                    _group_id.Branch(i)[j] = total_count
                    total_count = total_count+1
        
            # return outputs if you have them; here I try it for you:
            return (_volumes, _pipes, _group_id)
