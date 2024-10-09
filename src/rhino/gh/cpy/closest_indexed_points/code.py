from ghpythonlib.componentbase import executingcomponent as component
import Grasshopper
import Rhino
import Rhino.Geometry
from Rhino.Geometry import Sphere
from Rhino.Geometry import Polyline
import ghpythonlib.treehelpers as th
import System


class MyComponent(component):
    def RunScript(
        self,
        _polylines: Grasshopper.DataTree[Rhino.Geometry.Curve],
        _points: Grasshopper.DataTree[Rhino.Geometry.Point3d],
        _joint_types: Grasshopper.DataTree[int],
    ):
        ###############################################################################
        # Input
        ###############################################################################
        polylines = []
        polylines_out = Grasshopper.DataTree[Polyline]()
        pts = []
        ids = []

        ###############################################################################
        # Conversion
        ###############################################################################
        count = 0
        for curve in _polylines.AllData():
            result, polyline = curve.TryGetPolyline()
            polylines_out.Add(polyline, Grasshopper.Kernel.Data.GH_Path(int(count * 0.5)))
            if count % 2 == 0:
                polylines.append([polyline])
            else:
                polylines[-1].append(polyline)
            count = count + 1

        if _points.DataCount > 0 and _joint_types.DataCount > 0:
            for i in range(_points.BranchCount):
                for j in range(_points.Branch(i).Count):
                    pts.append(_points.Branch(i)[j])
                    ids.append(_joint_types.Branch(i)[j % _joint_types.Branch(i).Count])
        # pts = _points.AllData()
        # ids = _joint_types.AllData()

        ###############################################################################
        # Run the closest point search
        ###############################################################################

        # convert element polylines to a flattened dictionary:
        segments_dictionary = {}
        # stores ids e.g. int , [element_int,bottom_or_top_polyline_int,edge,int, bounding_box, hit_count, point_id]
        joint_types = []
        count = 0
        for i in range(len(polylines)):
            joint_types.append([-1] * (polylines[i][0].SegmentCount + 2))
            for j in range(len(polylines[i])):
                for k in range(polylines[i][j].SegmentCount):
                    bbox = polylines[i][j].SegmentAt(k).BoundingBox
                    bbox.Inflate(0.02)
                    segments_dictionary[count] = [i, j, k, bbox, polylines[i][j].SegmentAt(k)]
                    count = count + 1

        # create a tree
        rtree = Rhino.Geometry.RTree()

        # fill the tree
        for i in segments_dictionary:
            rtree.Insert(segments_dictionary[i][3], i)

        # call_backs of rtree
        def search_callback(sender, rtree_event_args):
            data_by_reference.append(rtree_event_args.Id)

        # perform search
        for i in range(len(pts)):
            data_by_reference = []
            if rtree.Search(Sphere(pts[i], 0), search_callback, data_by_reference):
                for j in data_by_reference:
                    if pts[i].DistanceToSquared(segments_dictionary[j][4].ClosestPoint(pts[i], True)) < 0.001:
                        joint_types[segments_dictionary[j][0]][segments_dictionary[j][2] + 2] = ids[i]

        ###############################################################################
        # Output
        ###############################################################################
        _polylines_out = polylines_out
        _joints_types_out = th.list_to_tree(joint_types)

        # return outputs if you have them; here I try it for you:
        return (_polylines_out, _joints_types_out)
