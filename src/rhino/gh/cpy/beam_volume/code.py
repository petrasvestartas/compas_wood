import Rhino
import Grasshopper
import ghpythonlib.treehelpers as th
from ghpythonlib.componentbase import executingcomponent as component
from wood_nano import beam_volumes as wood_nano_beam_volumes
from compas_wood.conversions_rhino import to_point2
from compas_wood.conversions_rhino import to_vector2
from compas_wood.conversions_rhino import from_point2
from compas_wood.conversions_rhino import from_point3
from wood_nano.conversions_python import to_double2
from wood_nano.conversions_python import to_int1
from wood_nano.conversions_python import from_int1
from wood_nano.conversions_python import from_int2
from wood_nano.conversions_python import from_double2
from wood_nano import int1
from wood_nano import int2
from wood_nano import double2
from wood_nano import point2
from wood_nano import point3
from wood_nano import cut_type2
import System

class BeamVolume(component):
    def beam_volumes(self,
        input_polyline_axes,
        input_polylines_segment_radii,
        input_polylines_segment_direction,
        input_allowed_types_per_polyline=[1],
        input_min_distance=50,
        input_volume_length=500,
        input_cross_or_side_to_end=0.91,
        input_flip_male=1,
        input_compute_joints=False,
        input_division_distance=150,
        input_shift=0.5,
        input_output_geometry_type=0,
        input_use_eccentricities_to_scale_joints=True,
    ):

        index_polylines = int2()
        index_polylines_segment = int2()
        distance = double2()
        point_pairs = point2()
        volume_pairs = point3()
        joints_areas = point2()
        joints_types = int1()
        w_output_plines = point3()
        w_output_types = cut_type2()

        wood_nano_beam_volumes(
            to_point2(input_polyline_axes),
            to_double2(input_polylines_segment_radii),
            to_vector2(input_polylines_segment_direction),
            to_int1(input_allowed_types_per_polyline),
            float(input_min_distance),
            float(input_volume_length),
            float(input_cross_or_side_to_end),
            int(input_flip_male),
            index_polylines,  # out
            index_polylines_segment,  # out
            distance,  # out
            point_pairs,  # out
            volume_pairs,  # out
            joints_areas,  # out
            joints_types,  # out
            w_output_plines,  # from_point3(w_output_plines)
            w_output_types,  # from_cut_type2(w_output_types))
            input_compute_joints,
            input_division_distance,
            input_shift,
            input_output_geometry_type,
            input_use_eccentricities_to_scale_joints,
        )

        return (
            from_int2(index_polylines),
            from_int2(index_polylines_segment),
            from_double2(distance),
            from_point2(point_pairs),
            from_point3(volume_pairs),
            from_point2(joints_areas),
            from_int1(joints_types),
        )

    def RunScript(self,
            _circles: Grasshopper.DataTree[Rhino.Geometry.Circle],
            _length_of_box: float,
            _distance_tolerance: float,
            _cross_or_toptoside: float,
            _create_pipes: bool):

        if _circles is None: return
        length_of_box = _length_of_box if _length_of_box else 1
        distance_tolerance = _distance_tolerance if _distance_tolerance else 10
        cross_or_top_to_side= _cross_or_toptoside if _cross_or_toptoside else 0.91
        create_pipes = _create_pipes if _create_pipes else False
        
        # Initialize outputs
        input_polylines = []
        input_polylines_segment_radii = []
        input_polylines_segment_direction = []
        _pipes = []

        # Processing each branch which represents a polyline
        for i in range(_circles.BranchCount):
            points = []
            radii= []
            normals = []
            parameters = []
            for index, circle in enumerate(_circles.Branch(i)):
                points.append(circle.Center)
                radii.append(circle.Radius)
                normals.append(circle.Plane.YAxis)
                parameters.append(index)
            # radii.pop()
            # normals.pop()

            input_polylines.append(Rhino.Geometry.Polyline(points))
            input_polylines_segment_radii.append(radii)
            # input_polylines_segment_direction.append(normals)

            if(create_pipes):
                curves = []
                for j in range (_circles.Branch(i).Count):
                    curves.append(_circles.Branch(i)[j].ToNurbsCurve())

                brep = Rhino.Geometry.Brep.CreateFromLoft(curves, Rhino.Geometry.Point3d.Unset, Rhino.Geometry.Point3d.Unset, Rhino.Geometry.LoftType.Loose, False)[0]
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
                _pipes.append(brep)

        
        _index_polylines, _index_polylines_segment, _distance, _point_pairs, _volume_pairs, _joints_areas, _joints_types = self.beam_volumes(
                input_polylines,
                input_polylines_segment_radii,
                input_polylines_segment_direction,
                input_allowed_types_per_polyline=[1],
                input_min_distance=distance_tolerance,
                input_volume_length=length_of_box,
                input_cross_or_side_to_end=cross_or_top_to_side,
        )

        return  th.list_to_tree(_index_polylines), th.list_to_tree( _index_polylines_segment), th.list_to_tree(_distance), th.list_to_tree(_point_pairs), th.list_to_tree( _volume_pairs), th.list_to_tree( _joints_areas), th.list_to_tree( _joints_types), th.list_to_tree( _pipes)
