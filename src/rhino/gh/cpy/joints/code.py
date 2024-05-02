# r: compas_wood, compas
from ghpythonlib.componentbase import executingcomponent as component
import System
from ghpythonlib import treehelpers
import Grasshopper
import Rhino
import os
from wood_nano import read_xml_polylines as wood_nano_read_xml_polylines
from wood_nano import double2
from wood_nano import joints as wood_nano_joints
from wood_nano import int2
from wood_nano import point2
from wood_nano import int1
import compas_wood
from compas_wood.conversions_rhino import to_point2
from compas_wood.conversions_rhino import from_point2
from wood_nano.conversions_python import from_int2
from wood_nano.conversions_python import from_int1


class joints(component):
    def RunScript(self, _polylines: System.Collections.Generic.List[Rhino.Geometry.Polyline], _id: int, _scale: float):

        # ==============================================================================
        # input
        # ==============================================================================
        dataset_index = _id if _id else 0
        module_path = os.path.dirname(compas_wood.__file__)
        foldername = os.path.join(module_path, "datasets")
        foldername = os.path.join(module_path, "datasets") + os.sep
        files = os.listdir(foldername)

        file_names_without_extensions = [os.path.splitext(file)[0] for file in files]
        filename = file_names_without_extensions[dataset_index % len(file_names_without_extensions)]

        scale = _scale if _scale else 1
        xform = Rhino.Geometry.Transform.Scale(Rhino.Geometry.Point3d(0, 0, 0), scale)

        # ==============================================================================
        # xml
        # ==============================================================================

        input_polyline_pairs = []

        if _polylines:
            for polyline in _polylines:
                copy = Rhino.Geometry.Polyline(polyline)
                copy.Transform(xform)
                input_polyline_pairs.append(copy)
        else:
            polylines_coordinates = double2()
            wood_nano_read_xml_polylines(
                foldername,
                filename,
                polylines_coordinates,
            )

            for polyline_coord in polylines_coordinates:
                polyline = Rhino.Geometry.Polyline()
                for i in range(0, len(polyline_coord), 3):
                    polyline.Add(
                        Rhino.Geometry.Point3d(polyline_coord[i], polyline_coord[i + 1], polyline_coord[i + 2])
                    )
                polyline.Transform(xform)
                input_polyline_pairs.append(polyline)

        # ==============================================================================
        # interface detection
        # ==============================================================================
        wood_nano_input_polyline_pairs = to_point2(input_polyline_pairs)
        element_pairs = int2()
        joint_areas = point2()
        joint_types = int1()

        wood_nano_joints(wood_nano_input_polyline_pairs, 2, element_pairs, joint_areas, joint_types)

        tree_element_pairs = treehelpers.list_to_tree(from_int2(element_pairs))
        tree_joint_areas = treehelpers.list_to_tree(from_point2(joint_areas))
        tree_joint_types = treehelpers.list_to_tree(from_int1(joint_types))

        # iterate joints add neighbors to individual elements element
        tree_element_neighbors = Grasshopper.DataTree[int]()
        for i in element_pairs:
            tree_element_neighbors.Add(i[1], Grasshopper.Kernel.Data.GH_Path(i[0]))
            tree_element_neighbors.Add(i[0], Grasshopper.Kernel.Data.GH_Path(i[1]))

        list_joint_areas_mesh = []
        for polyline in tree_joint_areas.AllData():
            polyline.MergeColinearSegments(0.02, True)
            polyline.CollapseShortSegments(0.02)
            list_joint_areas_mesh.append(Rhino.Geometry.Mesh.CreateFromClosedPolyline(polyline))

        return (
            input_polyline_pairs,
            tree_element_pairs,
            tree_joint_areas,
            list_joint_areas_mesh,
            tree_joint_types,
            tree_element_neighbors,
        )
