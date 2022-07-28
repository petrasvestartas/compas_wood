//nothing = '0',
//
////plates
//edge_insertion = '1',
//hole = '2',
//insert_between_multiple_edges = '3',
//
////beams
//slice = '4', //project and make rectangle
//mill_project = '5', //
//mill = '6', //project
//cut = '7',
//conic_reverse = '8',		//project
////binary_slice_mill = '9', //project and make rectangle
//conic = '9'
using Rhino;
using Rhino.Commands;
using Rhino.Geometry;
using Rhino.Input;
using Rhino.Input.Custom;
using System;
using System.Collections.Generic;

namespace joinery_solver_net_rhino_command_line
{
    public class joinery_solver_conic_reverse : Command
    {
        public joinery_solver_conic_reverse()
        {
            Instance = this;
        }

        public static joinery_solver_conic_reverse Instance { get; private set; }
        public override string EnglishName => "joinery_solver_toolpath_conic_reverse";

        protected override Result RunCommand(RhinoDoc doc, RunMode mode)
        {

            RhinoApp.WriteLine("Select closed brep", EnglishName);

            //Select face
            Rhino.DocObjects.ObjRef objref;
            var rc = Rhino.Input.RhinoGet.GetOneObject("Select Face", false, Rhino.DocObjects.ObjectType.Surface, out objref);
            if (rc != Rhino.Commands.Result.Success)
                return rc;

            var face = objref.Face();

            //Select  Edge
            Rhino.DocObjects.ObjRef objref_edge;
            var rc_edge = Rhino.Input.RhinoGet.GetOneObject("Select Edge", false, Rhino.DocObjects.ObjectType.EdgeFilter, out objref_edge);
            if (rc_edge != Rhino.Commands.Result.Success)
                return rc_edge;
            var edge = objref_edge.Edge();

            //Point3d p0 = edge.PointAtStart;
            //Point3d p1 = edge.PointAtEnd;



            string cut = String.Format("type:{0}|faceindex:{1}|edgeindex:{2}",
                "conic_reverse",
                face.FaceIndex,
                edge.EdgeIndex
                );

            RhinoApp.WriteLine("The {0} command added one line to the document.", EnglishName);
            //https://discourse.mcneel.com/t/is-userdictionary-data-from-commonobject-saved-with-the-file/134612/3
            Rhino.DocObjects.RhinoObject rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(objref.ObjectId);
            Rhino.DocObjects.ObjectAttributes object_attributes = rhino_object.Attributes.Duplicate();
            //object_attributes.UserDictionary.Set("cut", cut);
            object_attributes.SetUserString("cut", cut);
            //RhinoApp.WriteLine(object_attributes.UserDictionary.GetString("cut"));
            bool result = Rhino.RhinoDoc.ActiveDoc.Objects.ModifyAttributes(objref.ObjectId, object_attributes, false);


            doc.Views.Redraw();


            return Rhino.Commands.Result.Success;



            // ---
            return Result.Success;
        }
    }
}
