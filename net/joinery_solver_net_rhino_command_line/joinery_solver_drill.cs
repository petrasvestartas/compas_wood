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
    public class joinery_solver_drill : Command
    {
        public joinery_solver_drill()
        {
            Instance = this;
        }

        public static joinery_solver_drill Instance { get; private set; }
        public override string EnglishName => "joinery_solver_toolpath_drill";

        protected override Result RunCommand(RhinoDoc doc, RunMode mode)
        {

            RhinoApp.WriteLine("Select closed lines", EnglishName);

            //Select lines
            Rhino.DocObjects.ObjRef[] objrefs;
            var rc = Rhino.Input.RhinoGet.GetMultipleObjects("Select Lines", false, Rhino.DocObjects.ObjectType.Curve, out objrefs);
            if (rc != Rhino.Commands.Result.Success)
                return rc;

            //give lines radius
            double radius = -1;
            Rhino.Input.RhinoGet.GetNumber("drill radius", true, ref radius);

            string cut = String.Format("type:{0}|radius:{1}",
    "drill",
    radius
    );
            for (int i = 0; i < objrefs.Length; i++)
            {

                RhinoApp.WriteLine("The {0} command added one line to the document.", EnglishName);
                //https://discourse.mcneel.com/t/is-userdictionary-data-from-commonobject-saved-with-the-file/134612/3
                Rhino.DocObjects.RhinoObject rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(objrefs[i].ObjectId);
                Rhino.DocObjects.ObjectAttributes object_attributes = rhino_object.Attributes.Duplicate();
                //object_attributes.UserDictionary.Set("cut", cut);
                object_attributes.SetUserString("cut", cut);
                //RhinoApp.WriteLine(object_attributes.UserDictionary.GetString("cut"));
                bool result = Rhino.RhinoDoc.ActiveDoc.Objects.ModifyAttributes(objrefs[i].ObjectId, object_attributes, false);
            }








            doc.Views.Redraw();


            return Rhino.Commands.Result.Success;



            // ---
            return Result.Success;
        }
    }
}
