"""get joints ids and connections areas between elements"""

__author__ = "petras vestartas"
__version__ = "2023.04.01"


from ghpythonlib.componentbase import executingcomponent as component

import System
import Rhino
from Rhino.Geometry import Polyline
from Rhino.Geometry import Plane
from Rhino.Geometry import Point3d
from Rhino.Geometry import Vector3d
from Rhino.Geometry import Line
from Rhino.Geometry import Transform
from Rhino.Geometry import Arc
from Rhino.Geometry import Brep
from Rhino.Geometry import Curve
import math
import Grasshopper
from ghpythonlib import treehelpers



class Ears(component):
    def to_polyline_from_control_points(self, curve):
        """
        Convert a given curve to a polyline. If the curve cannot be directly converted,
        it uses the NURBS curve points to approximate the polyline.

        Args:
        curve: A curve object that supports TryGetPolyline and ToNurbsCurve methods.

        Returns:
        Polyline object approximating the input curve.
        """
        result, polyline = curve.TryGetPolyline()

        if not result:
            nurbs_curve = curve.ToNurbsCurve()
            point3d_array = []

            for i in range(len(nurbs_curve.Points)):
                result, pt = nurbs_curve.Points.GetPoint(i)
                if result:
                    point3d_array.append(pt)

            polyline = Polyline(point3d_array)
            nurbs_curve = polyline.ToNurbsCurve()
            result, polyline = nurbs_curve.TryGetPolyline()

            if not result:
                raise ValueError("Failed to convert NURBS curve to polyline.")

            polyline.CollapseShortSegments(0.01)

        else:
            polyline.CollapseShortSegments(0.01)

        return polyline



    def offset_parallelograms(self, x, y, radius) :
        """offset outlines by intersecting side planes
                
        Parameters
        ----------
        x : first closed outline
        y : second closed outline
        radius : offset distance that normally corresponds to the tool radius
                
        Returns
        -------
        tuple(polyline0, polyline1)
            two outlines
        bool
            if polylines are closed
        """

                
        if(len(x) < 2 or len(y)<2):
            print("WARNING empty polylines")
            return
        
        
        ###############################################################################
        # Get edge planes that are offseted
        ###############################################################################
        
        offset_planes = []
        for i in range(len(x)-1):
            c = (x[i] + x[i + 1]) * 0.5
            XAxis = y[i] - y[i + 1] if x[i].DistanceTo(x[i + 1]) < 0.01 else x[i] - x[i + 1]
            YAxis = x[i] - y[i]
            ZAxis = Vector3d.CrossProduct(XAxis, YAxis)
            ZAxis.Unitize()
            plane =  Plane(c + ZAxis * radius, XAxis, YAxis)
            offset_planes.append(plane)
        
        
        ###############################################################################
        # Intersect planes one by one
        ###############################################################################
        
        result, fit_plane0 = Plane.FitPlaneToPoints(x)
        result, fit_plane1 = Plane.FitPlaneToPoints(y)
        offset_polyline0 = Polyline()
        offset_polyline1 = Polyline()
        
        
        closed = x[0].DistanceToSquared( x[len(x)-1]) < 0.0001
        
        for i in range(len(offset_planes)) :
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(offset_planes[(len(offset_planes)+(i-1))% len(offset_planes)], offset_planes[i])
            result, types_of_notches_per_point = Rhino.Geometry.Intersect.Intersection.LinePlane(l, fit_plane0)
            offset_polyline0.Add(l.PointAt(types_of_notches_per_point))
            result, types_of_notches_per_point = Rhino.Geometry.Intersect.Intersection.LinePlane(l, fit_plane1)
            offset_polyline1.Add(l.PointAt(types_of_notches_per_point))
        
        
        
        
        if (closed == False) :
            
            l = Line.Unset
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(fit_plane0, offset_planes[0])
            offset_polyline0.Insert(0, l.ClosestPoint(x[0], False))
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(fit_plane0, offset_planes[len(offset_planes) - 1])
            offset_polyline0.Add(l.ClosestPoint(x[len(x) - 1], False))
            
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(fit_plane1, offset_planes[0])
            offset_polyline1.Insert(0, l.ClosestPoint(y[0], False))
            result, l = Rhino.Geometry.Intersect.Intersection.PlanePlane(fit_plane1, offset_planes[len(offset_planes) - 1])
            offset_polyline1.Add(l.ClosestPoint(y[len(y) - 1], False))
        else :
            offset_polyline0.Add(offset_polyline0[0])
            offset_polyline1.Add(offset_polyline1[0])
        
        
        ###############################################################################
        # Output
        ###############################################################################
        return offset_polyline0, offset_polyline1, closed

    def align_plane(self, plane, v):
        
        p = Plane(plane)
        
        result, parameterAlongXDirection, parameterAlongYDirection = p.ClosestParameter(p.Origin + v)
        angle = math.atan2(parameterAlongXDirection, parameterAlongYDirection)
        p.Rotate(-angle + 1.5707963267949, p.ZAxis, p.Origin)
        
        return p

    def vector_projection(self,v0, v1, extendV1 = False) :
        
        v0L = v0.Length
        v1L = v1.Length
        v0.Unitize()
        v1.Unitize()
        dotProduct = v0 * v1
        v0ProjectionLen = v0L * dotProduct
        if (extendV1):
            return v1 * (v0ProjectionLen + v1L)
        return v1 * v0ProjectionLen

    def get_leveled_vector(self, vector, vertical_height):
        
        dir = Vector3d(vector)
        dir.Unitize()
        angle = Vector3d.VectorAngle(dir, Vector3d(0, 0, 1))
        inclined_offset_by_vertical_distance = vertical_height * math.tan(angle)
        dir = dir*inclined_offset_by_vertical_distance

        return dir;


    def notches_lines(self, plines0, plines1, plines0_offset, plines1_offset, _radius, types_of_notches_per_point, offset_distances, is_closed, border):
        """create drilling lines that represent notches
                
        Parameters
        ----------
        plines0_offset[i], plines1_offset[i], _radius, notch_types, offset_distances
        plines0 : bottom offseted outlines by plane to plane inersection
        plines1 : top offseted outlines by plane to plane inersection
        radius : offset distance
        types_of_notches_per_point : type of notch: bisector, translation u, trasnlation v, rounding, none
        offset_distances : perpendicular distance between corner lines and offset lines
        is_closed : check if the offseted outline is closed  
        border : bool if border or hole
        Returns
        -------
        list(Line)
            drilling lines
        list(Line)
            drilling lines of corners that notches belongs to
        """
        
        ###############################################################################
        # Copy the offset polylines
        # Remove end point if the polyline is closed
        ###############################################################################
        xA_ = Polyline(plines0)
        xB_ = Polyline(plines1)
        xA = Polyline(plines0_offset)
        xB = Polyline(plines1_offset)
        
        if(is_closed):
            xA_.RemoveAt(len(xA_) - 1)
            xB_.RemoveAt(len(xB_) - 1)
            xA.RemoveAt(len(xA) - 1)
            xB.RemoveAt(len(xB) - 1)
        
        ###############################################################################
        # Output lists
        ###############################################################################
        l0_corners =  [] # fillet
        l0 =  []
        l1 =  [] # fillet
        l2 =  [] # fillet
        plines = []
        planes = []
        
        ###############################################################################
        # create plane to plane transformation for geometry rotated in 3D
        ###############################################################################
        result, base_plane = Plane.FitPlaneToPoints(xA)
        if (base_plane.DistanceTo(xB[0]) < 0.0):
            base_plane.Flip()
        to_xy = Transform.PlaneToPlane(base_plane, Plane.WorldXY)
        
        ###############################################################################
        # For notches or fillet, if fillet twice bigger radius is needed
        # also see this if statement if (types_of_notches_per_point[0] == 4 && angle_ > 0) : because corner is  inversed
        ###############################################################################
        fillet = False
        R_ = _radius * 2 if types_of_notches_per_point[0] == 4 else _radius
        notchStart = True
        
        for i in range(0, len(xA)) :
            
            ###############################################################################
            # Get vertical lines
            ###############################################################################
            normal = Line(xA[i], xB[i])
            n = len(xA)
            next = (i + 1) % n
            nextNext = (i + 2) % n
            prev = (((i - 1) % n) + n) % n
            
            p0a = xA[i]
            p1a = xA[next]
            p2a = xA[prev]
            p3a = xA[nextNext]
            
            p0b = xB[i]
            p1b = xB[next]
            p2b = xB[prev]
            p3b = xB[nextNext]
            
            normal1 = Line(xA[next], xB[next])
            normal2 = Line(xA[prev], xB[prev])

            ###############################################################################
            # Measure Concavity
            ###############################################################################
            cornera =  Polyline( [ p2a, p0a, p1a ] )
            cornerb =  Polyline( [ p2b, p0b, p1b ] )
            
            cornera.Transform(Transform.PlaneToPlane( Plane(p0a, normal.Direction), Plane.WorldXY))
            nextEdge = cornera.SegmentAt(0).Direction
            previousEdge = cornera.SegmentAt(1).Direction
            
            angle_ = ((math.atan2(nextEdge.X, nextEdge.Y) - math.atan2(previousEdge.X, previousEdge.Y) + math.pi * 2) % (math.pi * 2)) -math.pi
            
            if (types_of_notches_per_point[0] == 4 and angle_ > 0):
                l0_corners.append(Line.Unset)
                l0.append(Line.Unset)
                l1.append(Line.Unset)
                l2.append(Line.Unset)
                continue
            
            elif (types_of_notches_per_point[0] != 4 and angle_ < 0) :
                l0_corners.append(Line.Unset)
                l0.append(Line.Unset)
                l1.append(Line.Unset)
                l2.append(Line.Unset)
                continue
            
            ###############################################################################
            # Get Perpependicular points
            # Intersect current and neighbor lines
            ###############################################################################
            
            result, fit_plane1 = Plane.FitPlaneToPoints( [normal.To, normal1.To, normal2.To] )
            normalPl =  Plane(normal.From, fit_plane1.ClosestPoint(normal.From) - normal.From)
            f0, t = Rhino.Geometry.Intersect.Intersection.LinePlane(normal, normalPl)
            p0a = normal.PointAt(t)
            f1, t = Rhino.Geometry.Intersect.Intersection.LinePlane(normal1, normalPl)
            p1a = normal1.PointAt(t)
            f2, t = Rhino.Geometry.Intersect.Intersection.LinePlane(normal2, normalPl)
            p2a = normal2.PointAt(t)
            
            ###############################################################################
            # Compute offset vectors on perpedicular points
            # Error here, with v0 and v1 orientaton, must be rotated  90 deg
            ###############################################################################
            v0 = p0a - p1a
            v1 = p0a - p2a
            
            v0.Unitize()
            v1.Unitize()
            
            ###############################################################################
            # Depending on angle the offset changes, sin gives this distance value
            ###############################################################################
            angle = Vector3d.VectorAngle(v0, v1,  Plane(p0a, v0, v1))
            radius = R_ * ((1 / math.atan(angle)) )
            radius = -offset_distances[i].Length
            # translation
            radius *= -1
            
            ###############################################################################
            # TYPE A Notch - Line Directions A-B
            ###############################################################################
            local_cornera_ =  Polyline( [ xA_[prev], xA_[i], xA_[next] ] )
            local_cornerb_ =  Polyline( [ xB_[prev], xB_[i], xB_[next] ] )
            local_cornera =  Polyline( [ xA[prev], xA[i], xA[next]  ] )
            local_cornerb =  Polyline( [ xB[prev], xB[i], xB[next] ] )
            pl0 = self.align_plane(normalPl, v0)
            pl1 = self.align_plane(normalPl, v1)
            extension0 = 0
            extension1 = 0 
            normalXAxis0 = Line(local_cornera.SegmentAt(0).ClosestPoint(local_cornera_[1],False),local_cornerb.SegmentAt(0).ClosestPoint(local_cornerb_[1],False))
            normalXAxis1 = Line(local_cornera.SegmentAt(1).ClosestPoint(local_cornera_[1],False),local_cornerb.SegmentAt(1).ClosestPoint(local_cornerb_[1],False))
            dist0 = xA.ClosestPoint(normalXAxis0.From).DistanceTo(normalXAxis0.From)
            dist1 = xA.ClosestPoint(normalXAxis1.From).DistanceTo(normalXAxis1.From)
            border_or_hole = dist0<dist1 if border else dist0>dist1 
            if(border_or_hole):
                normalXAxis0 = normalXAxis1
            else:
                normalXAxis1 = normalXAxis0

            ###############################################################################
            # TYPE B Notch -  Diaognal
            ###############################################################################
            bisectorDir = offset_distances[i]
            
            pl2normalXAxis2Fillet = self.align_plane(normalPl, bisectorDir)
            length = bisectorDir.Length
            normalXAxis2 = Line(xA[i] + bisectorDir, xB[i] + bisectorDir)
            
            ###############################################################################
            # TYPE C Notch -  Fillet
            ###############################################################################
            bisectorDirFillet = (pl0.XAxis * radius + pl1.XAxis * radius)
            pl2 = self.align_plane(normalPl, bisectorDirFillet)
            lengthFillet = bisectorDir.Length
            bisectorDirFillet *= -(length) / length
            
            normalXAxis2Fillet = Line(xA[i] + bisectorDirFillet * ((length - R_) / length), xB[i] + bisectorDirFillet * ((length - R_) / length))
            normalXAxis2FilletCenter = Line(xA[i] + bisectorDirFillet, xB[i] + bisectorDirFillet)
            normalXAxis2Fillet0 = Line(normalXAxis2FilletCenter.From + pl0.XAxis * (radius + extension0), normalXAxis2FilletCenter.To + pl0.XAxis * (radius + extension0))
            normalXAxis2Fillet1 = Line(normalXAxis2FilletCenter.From + pl1.XAxis * (radius + extension0), normalXAxis2FilletCenter.To + pl1.XAxis * (radius + extension0))
            
            ###############################################################################
            # Output
            ###############################################################################
            planes.append(pl0)
            planes.append(pl1)
            planes.append(pl2)
            
            lastFlag = False
            
            l0_corners.append(Line(xA[i],xB[i]))
            if (len(types_of_notches_per_point) == 1):
                if (types_of_notches_per_point[0]==2 or types_of_notches_per_point[0]==3) :
                    flag = p0a.DistanceToSquared(p1a) < p0a.DistanceToSquared(p2a) if types_of_notches_per_point[0] == 3 else  p0a.DistanceToSquared(p1a) > p0a.DistanceToSquared(p2a)
                    if (flag):
                        l0.append(normalXAxis0)
                    else:
                        l0.append(normalXAxis1)
                    
                elif (types_of_notches_per_point[0]==1) :
                    l0.append(normalXAxis2)
                    
                elif (types_of_notches_per_point[0]==4) :
                    fillet = True
                    l0.append(normalXAxis2Fillet0)
                    l1.append(normalXAxis2Fillet)
                    l2.append(normalXAxis2Fillet1)
                    
                elif (types_of_notches_per_point[0]==5 or types_of_notches_per_point[0]==6) :
                    if (i % 2 == 1):
                        lastFlag = not lastFlag
                    if (types_of_notches_per_point[0] == 6):
                        lastFlag = not lastFlag
                    
                    if (lastFlag):
                        l0.append(normalXAxis0)
                    else:
                        l0.append(normalXAxis1)
                    
                else:
                    l0.append(Line.Unset)
                
            else:
                
                if (types_of_notches_per_point[i % len(types_of_notches_per_point)] == 2):
                    l0.append(normalXAxis0)
                
                elif (types_of_notches_per_point[i % len(types_of_notches_per_point)] == 3):
                    l0.append(normalXAxis1)
                
                elif (types_of_notches_per_point[i % len(types_of_notches_per_point)] == 4):
                    fillet = True
                    l0.append(normalXAxis2Fillet0)
                    l1.append(normalXAxis2Fillet)
                    l2.append(normalXAxis2Fillet1)
                
                elif (types_of_notches_per_point[i % len(types_of_notches_per_point)] == 1):
                    l0.append(normalXAxis2)
                
                else:
                    l0.append(Line.Unset)
        
        ###############################################################################
        # append Notches to a Pair of Polylines
        ###############################################################################
        lines = []
        lines_corners = []
        lines_horizontal = []
        result, fitPlane = Plane.FitPlaneToPoints(xA)
        result, fitPlaneB = Plane.FitPlaneToPoints(xB)
        
        xA_Notch = Polyline()
        xB_Notch = Polyline()
        
        extensions = []
        is_below = []
        
        for i in range(len(xA)):
            
            # Extend bottom line to compensate drilling bit thickness and angle
            extensionV = xA[i] - xB[i]
            extension = abs(math.tan(Vector3d.VectorAngle(extensionV, fitPlane.ZAxis)) * R_)
            extensionV.Unitize()
            
            # Move tool-path below surface
            extensionV = extensionV * extension
            extensions.append( extensionV )
            
            next = (i+1)%len(xA)
            prev = (len(xA)+(i-1))%len(xA)
            
            # Check direction of inclined cut, the check is made according to XY plane
            side_plane = Plane(xA[i], xB[i], xA[next])
            
            mid_0 = 0.5 * (xA[i] + xA[next])
            mid_1 = 0.5 * (xB[i] + xB[next])
            segment = Line(xB[i], xB[next])
            
            tool_direction =  Line(mid_0, segment.ClosestPoint(mid_0, False))
            side_plane.Transform(to_xy)
            
            is_below.append(0)
            if (side_plane.Normal.Z < -0.01):
                is_below[i] = -1
            elif (side_plane.Normal.Z > 0.01):
                is_below[i] = 1
                extensions[i] *= -1
            else:
                extensions[i] *= 0
        
        count = 0

        for i in range (len(xA)) :
            
            # Extend bottom line to compensate drilling bit thickness and angle
            extensionV = extensions[i]
            next = (i+1)%len(xA)
            prev = (len(xA)+(i-1))%len(xA)
            # Fillet corner
            if (fillet):
                if (l0[count] != Line.Unset) :

                    result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l0[count], fitPlane)
                    xA_Notch.append(l0[count].PointAt(t) + extensionV*0)
                    result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l2[count], fitPlane)
                    xA_Notch.append(l2[count].PointAt(t) + extensionV*0)
                    
                    result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l0[count], fitPlaneB)
                    xB_Notch.append(l0[count].PointAt(t))
                    result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(l2[count], fitPlaneB)
                    xB_Notch.append(l2[count].PointAt(t))
                
                else: 
                    xA_Notch.append(xA[i] + extensionV*0)
                    xB_Notch.append(xB[i])
            
            
            # Notch corner
            else :
                if(l0[count] != Line.Unset):
                    lines.append(l0[count])
                    lines_corners.append(l0_corners[count])
            
            
            
            count = count + 1 
            
        ###############################################################################
        # Cut lines by bottom and top planes and extend by the radius
        ###############################################################################
            
        for i in range(len(lines)):
            #cut the lines by top and bottom planes
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(lines[i], fitPlane)
            new_point_0 = lines[i].PointAt(t)
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(lines[i], fitPlaneB)
            new_point_1 = lines[i].PointAt(t)
            
            # get extension direction
            l_temp = Line(new_point_0,new_point_1)
            extension_dir = new_point_1-new_point_0
            extension_dir.Unitize()
            l_temp.Transform(to_xy)
            extension_vector = self.get_leveled_vector(l_temp.Direction,_radius)
            extension_dir = extension_dir * extension_vector.Length
            
            # extend the line and update the line
            lines[i] = Line(new_point_0-extension_dir*1,new_point_1+extension_dir*1)
            
        for i in range(len(lines_corners)):
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(lines_corners[i], fitPlane)
            new_point_0 = lines_corners[i].PointAt(t)
            result, t = Rhino.Geometry.Intersect.Intersection.LinePlane(lines_corners[i], fitPlaneB)
            new_point_1 = lines_corners[i].PointAt(t)
            
            # get extension direction
            l_temp = Line(new_point_0,new_point_1)
            extension_dir = new_point_1-new_point_0
            extension_dir.Unitize()
            l_temp.Transform(to_xy)
            extension_vector = self.get_leveled_vector(l_temp.Direction,_radius)
            extension_dir = extension_dir * extension_vector.Length
            
            # extend the line and update the line
            lines_corners[i] = Line(new_point_0-extension_dir*1,new_point_1+extension_dir*1) 
            
        for i in range(len(lines_corners)):
            lines_horizontal.append(Line(lines[i].From,lines_corners[i].From))
            lines_horizontal.append(Line(lines[i].To,lines_corners[i].To))
        
        ###############################################################################
        # Output
        ###############################################################################
        
        return lines, lines_corners,lines_horizontal, fitPlane, fitPlaneB


    def sort_by_size(self, _p0, _p1):
        # This function assumes _p0 and _p1 are lists of objects that have a method GetBoundingBox.
        # It sorts these objects by the diagonal length of their bounding boxes in descending order.

        try:
            # Creating a list of tuples, each containing the diagonal length and the corresponding objects from _p0 and _p1
            items_with_sizes = [(obj.GetBoundingBox(True).Diagonal.Length, obj, _p1[i]) for i, obj in enumerate(_p0)]
        except Exception as e:
            print(f"Error retrieving bounding box sizes: {str(e)}")
            return None

        # Sorting the list of tuples by the diagonal length, which is the first element of each tuple
        items_with_sizes.sort(reverse=True, key=lambda x: x[0])

        # Unzipping the sorted tuples into separate lists
        sizes, sorted_p0, sorted_p1 = zip(*items_with_sizes)

        return sorted_p0, sorted_p1


    def get_ears(self, _c0, _c1, _radius, _notch_type, _union):
        
        ###############################################################################
        # User interface, convert curves toi polylines
        ###############################################################################
        plines0 = []
        plines1 = []
        plines0_offset = []
        plines1_offset = []
        planes0 = []
        planes1 = []
        are_closed = []
        for i in range(len(_c0)):
            plines0.append(self.to_polyline_from_control_points(_c0[i]))
            plines1.append(self.to_polyline_from_control_points( _c1[i]))
            plines0_offset.append(plines0[-1])
            plines1_offset.append(plines1[-1])
            are_closed.append(True)
        
        ###############################################################################
        # Offset otlines, faces must be planar
        ###############################################################################
        for i in range(len(_c0)):
            plines0_offset[i], plines1_offset[i], are_closed[i] = self.offset_parallelograms(plines0[i], plines1[i], _radius)
       
        
        ###############################################################################
        # Create notches
        ###############################################################################
        
        notch_lines_nested = []
        notch_lines_nested_corners = []
        notch_lines_horizontal_nested = []
        for i in range(len(plines0)):
            
            
            pline_length =  plines0[i].Length
            notch_types = [0]*(len(plines0[i])-1)
            offset_distances = [Vector3d(0,0,0)]*(len(plines0[i])-1)
            
            n = len(plines0[i]) - 1
            for j in range( len(plines0[i])-1):
                
                l1 = Line( plines0_offset[i][j], plines1_offset[i][j])
                l2 = Line( plines0[i][j],  plines1[i][j])
                
                dist = l1.ClosestPoint( l2.From, False ).DistanceTo( l2.From ) - _radius
                bisector_dir =  l2.From - ( l1.ClosestPoint( l2.From, False))
                bisector_dir.Unitize()
                bisector_dir *= dist
                offset_distances[j] = bisector_dir
                

                if(_notch_type > 1):

                    shift = 0 if j % 2 == 1 else 0
                    
                    if(pline_length < 300):
                        
                        shift = 0 if j % 2 == 0 else 1
                    
                    notch_types[j] = (3 - shift)
                else:
                    notch_types[j] = _notch_type

            lines, lines_corners,lines_horizontal, plane0, plane1 = self.notches_lines(plines0[i], plines1[i],plines0_offset[i], plines1_offset[i], _radius, notch_types, offset_distances, are_closed[i],i==0)
            notch_lines_nested.append(lines)
            notch_lines_nested_corners.append(lines_corners)
            notch_lines_horizontal_nested.append(lines_horizontal)
            planes0.append(plane0)
            planes1.append(plane1)

        
        ###############################################################################
        # Output
        ###############################################################################
        return plines0_offset, plines1_offset, planes0, planes1, are_closed, notch_lines_nested, notch_lines_nested_corners, notch_lines_horizontal_nested

    def get_notch_geometry(self, _c0, _c1, notch_lines_nested, notch_lines_nested_corner, planes0, planes1, radius):
        
        
        #geo = []
        cut_curves0 = []
        cut_curves1 = []
        cut_extrusions = []
        for i in range(len(notch_lines_nested)):
            
            cutter_curves0 = []
            cutter_curves1 = []
            for j in range(len(notch_lines_nested[i])):
                
                # get plane
                p0 = notch_lines_nested[i][j].PointAt(0.5)
                p1 = notch_lines_nested_corner[i][j].ClosestPoint(p0,False)
                xaxis = p0-p1
                zaxis = notch_lines_nested[i][j].Direction
                yaxis = Vector3d.CrossProduct(xaxis,zaxis)
                plane = Plane(p0,xaxis,yaxis)
                
                #construction 5 points for an arc and rectangle profile
                p0_profile = p0 + plane.YAxis*radius
                p1_profile = p1 + plane.YAxis*radius
                p2_profile = p1 - plane.YAxis*radius
                p3_profile = p0 - plane.YAxis*radius
                p4_profile = p0 + plane.XAxis*radius
                p5_profile = p1 - plane.XAxis*radius
                
                # construct PolyCurve
                arc0 = Arc(p0_profile,p4_profile,p3_profile)
                arc1 = Arc(p1_profile,p5_profile,p2_profile)
                polyline0 = Polyline([p0_profile,p1_profile]).ToNurbsCurve()
                polyline1 = Polyline([p2_profile,p3_profile]).ToNurbsCurve()

                joined_curve = Rhino.Geometry.Curve.JoinCurves([arc0.ToNurbsCurve(),polyline0.ToNurbsCurve(),arc1.ToNurbsCurve(),polyline1.ToNurbsCurve()],0.01)[0]
                joined_curve.Transform(Transform.Translation(notch_lines_nested[i][j].Direction*1))
                
                # extrude
                extrusion = Rhino.Geometry.Extrusion.Create(joined_curve.ToNurbsCurve(),notch_lines_nested[i][j].Length*2, True).ToBrep()

                # brep plane
                result, curves0, points = Rhino.Geometry.Intersect.Intersection.BrepPlane(extrusion, planes0[i], 0.01)
                result, curves1, points = Rhino.Geometry.Intersect.Intersection.BrepPlane(extrusion, planes1[i], 0.01)
                cutter_curves0.append(curves0[0])
                cutter_curves1.append(curves1[0])
                
                cut_extrusions.append(extrusion)            
        
        return cut_curves0, cut_curves1,cut_extrusions
    
    def RunScript(self,
            _p0: Grasshopper.DataTree[Rhino.Geometry.Curve],
            _p1: Grasshopper.DataTree[Rhino.Geometry.Curve],
            _radius: float,
            _notch_type: int,
            _union: bool):
        
        ###############################################################################
        # Run
        ###############################################################################
        radius = _radius if _radius is not None else 1

        
        if _p0 and _p1:

            if _p0 == None:
                return
            
            if _p1 == None:
                return

            if _notch_type == None:
                return
            
            if _p0.DataCount is not _p1.DataCount:
                return
            
            lines =Grasshopper.DataTree[Line]()
            lines_corners = Grasshopper.DataTree[Line]()
            lines_horizontal = Grasshopper.DataTree[Line]()
            offset0 = Grasshopper.DataTree[Polyline]()
            offset1 = Grasshopper.DataTree[Polyline]()
            planes = Grasshopper.DataTree[Plane]()

            cut_extrusions = Grasshopper.DataTree[Brep]()
            for i in range(_p0.BranchCount):
                _c0, _c1 = self.sort_by_size(_p0.Branch(i), _p1.Branch(i))
                self.get_ears(_c0, _c1,radius,_notch_type, _union)
                plines0, plines1, planes0, planes1, are_closed, notch_lines_nested, notch_lines_nested_corners, notch_lines_horizontal_nested = self.get_ears(_c0, _c1,radius,_notch_type, _union)
                print(notch_lines_nested)
                for notch_lines in notch_lines_nested:
                    lines.AddRange(notch_lines, Grasshopper.Kernel.Data.GH_Path(i))

                for notch_lines in notch_lines_nested:
                    lines_corners.AddRange(notch_lines, Grasshopper.Kernel.Data.GH_Path(i))
                    

                for j in range(len(notch_lines_horizontal_nested)):
                    for k in range(0,len(notch_lines_horizontal_nested[j]),2):
                        lines_horizontal.AddRange([notch_lines_horizontal_nested[j][k],notch_lines_horizontal_nested[j][k+1]],Grasshopper.Kernel.Data.GH_Path(i,j,k))
                
                offset0.AddRange(plines0,Grasshopper.Kernel.Data.GH_Path(i))
                offset1.AddRange(plines1,Grasshopper.Kernel.Data.GH_Path(i))
                planes.Add(Plane(plines0[0][0],planes0[0].ZAxis),Grasshopper.Kernel.Data.GH_Path(i))
                notch_geo = []
                if(_union):
                    cut_curves0_temp, cut_curves1_temp, cut_extrusions_temp = self.get_notch_geometry(_c0, _c1, notch_lines_nested,notch_lines_nested_corners, planes0, planes1, radius+0.0000)
                    cut_extrusions.AddRange(cut_extrusions_temp,Grasshopper.Kernel.Data.GH_Path(i))
        return lines, lines_corners, lines_horizontal, offset0, offset1, planes, cut_extrusions

     