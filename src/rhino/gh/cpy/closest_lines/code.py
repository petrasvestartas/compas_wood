from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper, GhPython
import System
import Rhino
import rhinoscriptsyntax as rs

__author__ = "petra"
__version__ = "2023.04.04"

import Rhino.Geometry
from Rhino.Geometry import RTree
from Rhino.Geometry import BoundingBox
from Rhino.Geometry import Sphere
from Rhino.Geometry import Vector3d
from Rhino.Geometry import Point3d
from Rhino.Geometry import Polyline
from Rhino.Geometry import Line
import Grasshopper
import ghpythonlib.treehelpers as th


class MyComponent(component):
    def RunScript(self, _polylines, _lines):
        """Provides a scripting component.
        Inputs:
            x: The x script variable
            y: The y script variable
        Output:
            a: The a output variable"""

        ###############################################################################
        # Input
        ###############################################################################
        polylines = []
        polylines_out = Grasshopper.DataTree[Polyline]()
        lines = []

        ###############################################################################
        # Conversion
        ###############################################################################
        count = 0
        for curve in _polylines.AllData():
            result, polyline = curve.TryGetPolyline()
            polylines_out.Add(
                polyline, Grasshopper.Kernel.Data.GH_Path(int(count * 0.5))
            )
            if count % 2 == 0:
                polylines.append([polyline])
            else:
                polylines[-1].append(polyline)
            count = count + 1

        lines = _lines.AllData()

        ###############################################################################
        # Run the closest point search
        ###############################################################################

        # convert element polylines to a flattened dictionary:
        segments_dictionary = (
            {}
        )  # stores ids e.g. int , [element_int,bottom_or_top_polyline_int,edge,int, bounding_box, hit_count, point_id]
        vectors = []
        count = 0
        for i in range(len(polylines)):
            vectors.append([Vector3d.Zero] * (polylines[i][0].SegmentCount + 2))
            for j in range(len(polylines[i])):
                for k in range(polylines[i][j].SegmentCount):
                    bbox = polylines[i][j].SegmentAt(k).BoundingBox
                    bbox.Inflate(0.02)
                    segments_dictionary.Add(
                        count, [i, j, k, bbox, polylines[i][j].SegmentAt(k)]
                    )
                    count = count + 1

        # create a tree
        rtree = Rhino.Geometry.RTree()

        # fill the tree
        for i in segments_dictionary:
            rtree.Insert(segments_dictionary[i][3], i)

        # call_backs of rtree
        hit_count = 0

        def search_callback(sender, rtree_event_args):
            data_by_reference.append(rtree_event_args.Id)

        # perform search, two times first for start points of a polyline, second for the end point
        for i in range(len(lines)):
            data_by_reference = []
            if rtree.Search(
                Sphere(lines[i].From, 0), search_callback, data_by_reference
            ):
                for j in data_by_reference:
                    if (
                        lines[i].From.DistanceToSquared(
                            segments_dictionary[j][4].ClosestPoint(lines[i].From, True)
                        )
                        < 0.001
                    ):
                        vectors[segments_dictionary[j][0]][
                            segments_dictionary[j][2] + 2
                        ] = lines[i].Direction
                        # vectors[segments_dictionary[j][0]][segments_dictionary[j][2]+2].Unitize()

        for i in range(len(lines)):
            data_by_reference = []
            if rtree.Search(Sphere(lines[i].To, 0), search_callback, data_by_reference):
                for j in data_by_reference:
                    if (
                        lines[i].To.DistanceToSquared(
                            segments_dictionary[j][4].ClosestPoint(lines[i].To, True)
                        )
                        < 0.001
                    ):
                        vectors[segments_dictionary[j][0]][
                            segments_dictionary[j][2] + 2
                        ] = -lines[i].Direction
                        # vectors[segments_dictionary[j][0]][segments_dictionary[j][2]+2].Unitize()

        ###############################################################################
        # Output
        ###############################################################################
        _polylines_out = polylines_out
        _vectors = th.list_to_tree(vectors)

        # return outputs if you have them; here I try it for you:
        return (_polylines_out, _vectors)
