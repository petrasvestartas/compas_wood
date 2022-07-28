using Rhino;
using Rhino.Commands;
using Rhino.Geometry;
using Rhino.Input;
using Rhino.Input.Custom;
using System;
using System.Collections.Generic;

namespace joinery_solver_rhino
{
    public class joinery_solver_toolpath_mill : Command
    {
        public joinery_solver_toolpath_mill()
        {
            Instance = this;
        }

        public static joinery_solver_toolpath_mill Instance { get; private set; }

        public override string EnglishName => "joinery_solver_toolpath_mill";

        protected override Result RunCommand(RhinoDoc doc, RunMode mode)
        {

            RhinoApp.WriteLine("Select closed brep", EnglishName);

            //Select face
            Rhino.DocObjects.ObjRef objref;
            var rc = Rhino.Input.RhinoGet.GetOneObject("Select Face", false, Rhino.DocObjects.ObjectType.Surface, out objref);
            if (rc != Rhino.Commands.Result.Success)
                return rc;

            var face = objref.Face();

            //Select start vertex
            Rhino.DocObjects.ObjRef objref_vertex;
            var rc_vertex = Rhino.Input.RhinoGet.GetOneObject("Select Vertex", false, Rhino.DocObjects.ObjectType.BrepVertex, out objref_vertex);
            if (rc_vertex != Rhino.Commands.Result.Success)
                return rc_vertex;

            var v0 = objref.Point();

            //https://discourse.mcneel.com/t/is-userdictionary-data-from-commonobject-saved-with-the-file/134612/3
            Rhino.DocObjects.RhinoObject rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(objref.ObjectId);
            Rhino.DocObjects.ObjectAttributes object_attributes = rhino_object.Attributes;
            object_attributes.UserDictionary.Set("cut", String.Format("{0}_{1}_{2}", "mill", face.Id, -1));
            Rhino.RhinoDoc.ActiveDoc.Objects.ModifyAttributes(objref.ObjectId, object_attributes, true);



            return Rhino.Commands.Result.Success;

            doc.Views.Redraw();
            RhinoApp.WriteLine("The {0} command added one line to the document.", EnglishName);

            // ---
            return Result.Success;
        }
    }
}
