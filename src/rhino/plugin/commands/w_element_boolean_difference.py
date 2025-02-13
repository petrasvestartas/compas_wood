#! python3
# venv: timber_connections

import Rhino
import System

from wood_rui import (
    select_and_find_valid_groups, polyline_obj_to_plane
)


def prepare_for_boolean_difference(geometry_planes):
    shapes = []
    cuts = []

    for geometry_plane in geometry_planes:
        obj = geometry_plane[0]
        geometry = obj.Geometry
        string_dictionary = obj.Attributes.GetUserStrings()

        T = Rhino.Geometry.Transform.PlaneToPlane(Rhino.Geometry.Plane.WorldXY, polyline_obj_to_plane(geometry_plane[1]))
        shapes.append(geometry)

        # Determine type-specific processing
        obj_type = type(geometry)
        valid_types = {
            Rhino.Geometry.Brep: Rhino.DocObjects.ObjectType.Brep,
            Rhino.Geometry.Mesh: Rhino.DocObjects.ObjectType.Mesh
        }

        if obj_type in valid_types:
            cut_list = []
            expected_type = valid_types[obj_type]

            for key in string_dictionary:
                value = string_dictionary[key]
                if "feature" in key:
                    feature_geometry = Rhino.Geometry.GeometryBase.FromJSON(value)
                    feature_geometry.Transform(T)
                    if feature_geometry.ObjectType == expected_type:
                        cut_list.append(feature_geometry)

            cuts.append(cut_list)

        # Redraw the document to reflect changes
        Rhino.RhinoDoc.ActiveDoc.Views.Redraw()

    return shapes, cuts

def clean_brep(brep : Rhino.Geometry.Brep):
    copy = brep.DuplicateBrep()
    copy.Faces.SplitKinkyFaces()
    if Rhino.Geometry.BrepSolidOrientation.Inward == copy.SolidOrientation:
        copy.Flip()
    return copy


def clean_mesh(mesh : Rhino.Geometry.Mesh):
    copy = mesh.DuplicateMesh()
    copy.Compact();
    copy.Vertices.CombineIdentical(True, True);
    copy.Vertices.CullUnused();
    copy.UnifyNormals();
    copy.Weld(3.14159265358979);
    copy.FaceNormals.ComputeFaceNormals();
    copy.Normals.ComputeNormals();
    if (copy.SolidOrientation() == -1):
        copy.Flip(True, True, True);
    return copy



def brep_boolean_difference(shape : Rhino.Geometry.Brep, cutters : list[Rhino.Geometry.Brep]):

    for i in range(len(shapes)):

        shape_cleaned = clean_brep(shape)

        for brep in cutters:
            cutter_cleaned = clean_brep(brep)
            out_breps = Rhino.Geometry.Brep.CreateBooleanDifference(shape_cleaned, cutter_cleaned, Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance)
            
            volume = 0
            
            if out_breps:
                for o in out_breps:
                    o_cleaned = clean_brep(o)
                    current_volume = Rhino.Geometry.VolumeMassProperties.Compute(o_cleaned, True, False, False, False).Volume;
                    if current_volume > volume:
                        volume = current_volume
                        shape_cleaned = o_cleaned

    return shape_cleaned

def mesh_boolean_difference(shape : Rhino.Geometry.Mesh, cutters : list[Rhino.Geometry.Mesh]):
    print(shape, cutters)
    for i in range(len(shapes)):

        shape_cleaned = clean_mesh(shape)

        for mesh in cutters:

            cutter_cleaned = clean_mesh(mesh)
            out_meshes = Rhino.Geometry.Mesh.CreateBooleanDifference([shape_cleaned], [cutter_cleaned])
            
            volume = 0
            
            if out_meshes:
                for o in out_meshes:
                    o_cleaned = clean_mesh(o)
                    current_volume = Rhino.Geometry.VolumeMassProperties.Compute(o_cleaned, True, False, False, False).Volume;
                    if current_volume > volume:
                        volume = current_volume
                        shape_cleaned = o_cleaned

    return shape_cleaned

def boolean_difference(shapes : list[Rhino.Geometry.GeometryBase], cuts : list[list[Rhino.Geometry.GeometryBase]]):

    for i in range(len(shapes)):
        if isinstance(shapes[i], Rhino.Geometry.Brep):
            shapes[i] = brep_boolean_difference(shapes[i], cuts[i])
            Rhino.RhinoDoc.ActiveDoc.Objects.AddBrep(shapes[i])
        elif isinstance(shapes[i], Rhino.Geometry.Mesh):
            shapes[i] = mesh_boolean_difference(shapes[i], cuts[i])
            Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(shapes[i])
            
    


if __name__ == "__main__":
    geometry_planes = select_and_find_valid_groups("Elements")
    shapes, cuts = prepare_for_boolean_difference(geometry_planes)
    boolean_difference(shapes, cuts)


