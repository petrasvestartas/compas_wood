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
    public class joinery_solver_element_type : Command
    {
        public joinery_solver_element_type()
        {
            Instance = this;
        }

        public static joinery_solver_element_type Instance { get; private set; }
        public override string EnglishName => "joinery_solver_element_type";

        protected override Result RunCommand(RhinoDoc doc, RunMode mode)
        {



            string element_type = "";
            var rc_type = Rhino.Input.RhinoGet.GetString("0 - plate, 1 - rect_beam, 2 - round_beam, 3 - glulam", false, ref element_type);

            if (element_type == "0")
            {
                element_type = "plate";


                RhinoApp.WriteLine("Select Main Geometry", EnglishName);

                //Select face
                Rhino.DocObjects.ObjRef objref;
                var rc_ = Rhino.Input.RhinoGet.GetOneObject("Select Face", false, Rhino.DocObjects.ObjectType.Surface, out objref);
                if (rc_ != Rhino.Commands.Result.Success)
                    return rc_;

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
                    "cut_reverse",
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

            }
            else if (element_type == "1")
                element_type = "rect_beam";
            else if (element_type == "2")
                element_type = "round_beam";
            else if (element_type == "3")
                element_type = "glulam";

            Rhino.DocObjects.ObjRef[] objrefs = null;
            //var rc = Rhino.Input.RhinoGet.GetMultipleObjects("Select Objects", false, Rhino.DocObjects.ObjectType.AnyObject, out objrefs);
            if (Rhino.Input.RhinoGet.GetMultipleObjects("Select all objects that belongs to one group", false, Rhino.DocObjects.ObjectType.Surface | Rhino.DocObjects.ObjectType.Curve, out objrefs) != Rhino.Commands.Result.Success)
                return Result.Failure;


            string element = String.Format("type:{0}", element_type);
            for (int i = 0; i < objrefs.Length; i++)
            {

                RhinoApp.WriteLine("The {0} command added one line to the document.", EnglishName);
                //https://discourse.mcneel.com/t/is-userdictionary-data-from-commonobject-saved-with-the-file/134612/3
                Rhino.DocObjects.RhinoObject rhino_object = Rhino.RhinoDoc.ActiveDoc.Objects.Find(objrefs[i].ObjectId);
                Rhino.DocObjects.ObjectAttributes object_attributes = rhino_object.Attributes.Duplicate();
                //object_attributes.UserDictionary.Set("cut", cut);
                object_attributes.SetUserString("element", element);
                //RhinoApp.WriteLine(object_attributes.UserDictionary.GetString("cut"));
                bool result = Rhino.RhinoDoc.ActiveDoc.Objects.ModifyAttributes(objrefs[i].ObjectId, object_attributes, false);
            }



            doc.Views.Redraw();


            return Rhino.Commands.Result.Success;




        }
    }
}
