using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using rhino_util;

namespace joinery_solver_net
{

    public enum cut_type
    {
        nothing = 0,

        //plates
        edge_insertion = 1,
        hole = 2,
        insert_between_multiple_edges = 3,

        //beams
        slice = 4, //project and make rectangle
        mill_project = 5, //
        mill = 6, //project
        cut = 7,
        conic_reverse = 8,        //project
                                  //binary_slice_mill = '9', //project and make rectangle
        conic = 9,

        //drill
        drill = 10,
        cut_reverse = 11
    }
    public class cut
    {

        //    nothing = '0',
        //
        //    //plates
        //    edge_insertion = '1',
        //    hole = '2',
        //    insert_between_multiple_edges = '3',
        //
        //    //beams
        //    slice = '4', //project and make rectangle
        //    mill_project = '5', //
        //    mill = '6', //project
        //    cut = '7',
        //    conic_reverse = '8',		//project
        //    //binary_slice_mill = '9', //project and make rectangle
        //    conic = '9'

        public cut_type type = 0;
        public List<Polyline> plines_copy = new List<Polyline>();
        public Brep geometry = null;
        public double radius = double.NaN;

        public cut()
        {

        }

        public cut(cut c)
        {
            this.type = c.type;
            this.plines_copy = c.plines_copy.Duplicate();
            this.radius = c.radius;
            //Rhino.RhinoApp.WriteLine("CutCopy");
            if (c.geometry != null)
                this.geometry = c.geometry.DuplicateBrep();

        }

        public cut(Line line, string txt)
        {
            /////////////////////////////////////////////////////////////////////////////////
            //convert string values
            /////////////////////////////////////////////////////////////////////////////////
            string[] words = txt.Split('|');
            string type = words[0].Split(':')[1];
            this.radius = Convert.ToDouble(words[1].Split(':')[1]);


            this.type = (cut_type)Enum.Parse(typeof(cut_type), type);
            this.plines_copy.Add(line.ToPolyline());
            Rhino.Geometry.Cylinder cylinder = new Rhino.Geometry.Cylinder(new Circle(new Plane(line.From, line.Direction), radius), line.Length);
            this.geometry = cylinder.ToBrep(true, true);
        }

        public cut(Brep brep, string txt)
        {
            this.geometry = brep;

            /////////////////////////////////////////////////////////////////////////////////
            //convert string values
            /////////////////////////////////////////////////////////////////////////////////
            string[] words = txt.Split('|');
            string type = words[0].Split(':')[1];
            int face_index = Convert.ToInt32(words[1].Split(':')[1]);
            int edge_index = Convert.ToInt32(words[2].Split(':')[1]);

            /////////////////////////////////////////////////////////////////////////////////
            //edge points
            /////////////////////////////////////////////////////////////////////////////////
            Point3d p0 = brep.Edges[edge_index].PointAtStart;
            Point3d p1 = brep.Edges[edge_index].PointAtEnd;

            /////////////////////////////////////////////////////////////////////////////////
            //top and bottom outlines
            /////////////////////////////////////////////////////////////////////////////////
            Curve curve = brep.Faces[face_index].OuterLoop.To3dCurve();
            Polyline top_polyline = new Polyline();
            curve.TryGetPolyline(out top_polyline);
            top_polyline = rhino_util.PolylineUtil.Flip(top_polyline);
            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(top_polyline);

            //get opposite face
            int[] nf = brep.Faces[face_index].AdjacentFaces();
            int[] all_f = Enumerable.Range(0, brep.Faces.Count).ToArray();
            int[] except = all_f.Except(nf).ToArray();
            int opposite_face = except[0] == face_index ? except[1] : except[0];

            curve = brep.Faces[opposite_face].OuterLoop.To3dCurve();
            Polyline bot_polyline = new Polyline();
            curve.TryGetPolyline(out bot_polyline);
            //bot_polyline = rhino_util.PolylineUtil.Flip(bot_polyline);
            //  rhino_util.PolylineUtil.Flip();
            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(bot_polyline);

            Polyline[] sortedPlines = { top_polyline, bot_polyline };
            /////////////////////////////////////////////////////////////////////////////////
            //align to two points
            /////////////////////////////////////////////////////////////////////////////////
            int id0 = top_polyline.ClosestIndex(p0);

            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(top_polyline);

            sortedPlines[0].ShiftPolyline(sortedPlines[0].Count - id0 - 1);

            int id1 = sortedPlines[0].ClosestIndex(p1);
            if (id1 == sortedPlines[0].Count - 2)
                top_polyline.ShiftPolyline(1);
            // Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(p0);
            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(p1);

            /////////////////////////////////////////////////////////////////////////////////
            //align two outlines
            /////////////////////////////////////////////////////////////////////////////////

            int[] sortValues = new int[sortedPlines[0].Count];
            int[] sortValuesID = new int[sortedPlines[0].Count];
            for (int j = 0; j < sortedPlines[0].Count; j++)
            {
                int cp = sortedPlines[1].ClosestIndex(sortedPlines[0].PointAt(j));
                int shiftValue = MathUtil.Wrap(j - cp, sortedPlines[0].Count - 1);
                sortValues[shiftValue]++;
                sortValuesID[j] = j;
            }

            Array.Sort(sortValues, sortValuesID);
            Array.Reverse(sortValues);
            Array.Reverse(sortValuesID);


            int cp0 = sortedPlines[1].ClosestIndex(sortedPlines[0].PointAt(0));
            int shiftN = MathUtil.Wrap(0 - cp0, sortedPlines[0].Count - 1);
            shiftN = sortValuesID[0];
            sortedPlines[1].ShiftPolyline(shiftN);


            /////////////////////////////////////////////////////////////////////////////////
            //reverse for outside cutting
            /////////////////////////////////////////////////////////////////////////////////
            if (type == "cut_reverse")
            {
                sortedPlines[0] = sortedPlines[0].Flip();
                sortedPlines[0].ShiftPolyline(1);

                sortedPlines[1] = sortedPlines[1].Flip();
                sortedPlines[1].ShiftPolyline(1);
            }




            /////////////////////////////////////////////////////////////////////////////////
            //add a cut
            /////////////////////////////////////////////////////////////////////////////////
            this.type = (cut_type)Enum.Parse(typeof(cut_type), type);

            this.plines_copy = new List<Polyline>() { sortedPlines[0], sortedPlines[1] };

            /////////////////////////////////////////////////////////////////////////////////
            //post-processing based on type
            /////////////////////////////////////////////////////////////////////////////////

            //if (this.type == cut_type.slice)
            //    for (int i = 0; i < plines_copy.Count; i++)
            //        plines_copy[i] = plines_copy[i].Flip().ShiftPline(-1);

            if (this.type == cut_type.mill_project)
            {
                Polyline plines_copy_ = plines_copy[0].Duplicate();
                var xform = Transform.PlanarProjection(plines_copy[1].plane());
                plines_copy_.Transform(xform);

            }

            //joinery_solver_net.cut cut = new cut(sortedPlines.ToList(),types[type]);

            //Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(sortedPlines[0]);
            //  Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(sortedPlines[1]);
        }



        public cut(List<Polyline> plines, cut_type type)
        {

            this.type = type;


            Polyline polyline = new Polyline(plines[0]);
            Plane plane = plines[0].plane();

            bool flag = rhino_util.PolylineUtil.IsClockwiseClosedPolyline(plines[0], plines[0].plane());

            plines_copy = new List<Polyline>(plines);

            if (plane.DistanceTo(plines[1][0]) > 0)
                for (int i = 0; i < plines_copy.Count; i++)
                    plines_copy[i] = rhino_util.PolylineUtil.Flip(plines_copy[i]).ShiftPline(-1);

            if (type == cut_type.slice)
                for (int i = 0; i < plines_copy.Count; i++)
                    plines_copy[i] = plines_copy[i].Flip().ShiftPline(-1);

            if (type == cut_type.mill_project)
            {
                plines_copy[1] = plines_copy[0].Duplicate();
                var xform = Transform.PlanarProjection(plines[1].plane());
                plines_copy[1].Transform(xform);

            }

        }


        public cut duplicate()
        {
            return new cut(this);
        }

        public void transform(Transform xform)
        {
            plines_copy.Transform(xform);
            if(geometry!=null)
                geometry.Transform(xform);
        }

        public override string ToString()
        {
            return "type_" + type.ToString();
        }

    }


}
