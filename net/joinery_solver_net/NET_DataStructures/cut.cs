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
        nothing,

        //plates
        edge_insertion,
        hole,
        insert_between_multiple_edges,

        //beams
        slice, //always projected
        slice_projectsheer,

        mill,//always inside volume
        mill_project,
        mill_projectsheer,

        cut,
        cut_project,
        cut_projectsheer,
        cut_reverse,

        conic, //always projected
        conic_reverse,

        drill,
    }
    public class cut
    {

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

        public cut(Line line, double radius, bool loft = false)
        {
            /////////////////////////////////////////////////////////////////////////////////
            //convert string values
            /////////////////////////////////////////////////////////////////////////////////
            this.radius = radius;


            this.type = cut_type.drill;
            this.plines_copy.Add(line.ToPolyline());
            if (loft)
            {
                Rhino.Geometry.Cylinder cylinder = new Rhino.Geometry.Cylinder(new Circle(new Plane(line.From, line.Direction), radius), line.Length);
                this.geometry = cylinder.ToBrep(true, true);
            }
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
            //Check orientation
            /////////////////////////////////////////////////////////////////////////////////
            Plane plane = rhino_util.PolylineUtil.plane(sortedPlines[0]);
            Point3d center = rhino_util.PolylineUtil.CenterPoint(sortedPlines[1]);
            if (plane.DistanceTo(center) < 0)
            {
                sortedPlines[0] = sortedPlines[0].Flip();
                sortedPlines[0].ShiftPolyline(1);

                sortedPlines[1] = sortedPlines[1].Flip();
                sortedPlines[1].ShiftPolyline(1);
            }




            /////////////////////////////////////////////////////////////////////////////////
            //reverse for inside cutting
            /////////////////////////////////////////////////////////////////////////////////
            if (type != "cut_reverse")
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

            //this.plines_copy = new List<Polyline>() { sortedPlines[0].Flip().ShiftPline(-1), sortedPlines[1].Flip().ShiftPline(-1) };
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



        public cut(List<Polyline> plines, cut_type type, bool loft = false)
        {

            this.type = type;
            this.plines_copy = plines.Duplicate();

            project();


            if (loft)
                this.geometry = BrepUtil.Loft(plines_copy[0], plines_copy[1], true);

        }

        public void project()
        {

            // List<Polyline> plines_copy = x.Duplicate();



            Polyline polyline = new Polyline(plines_copy[0]);
            Plane pline_plane0 = plines_copy[0].plane();
            Plane pline_plane1 = plines_copy[1].plane();

            if (pline_plane0.DistanceTo(plines_copy[1][0]) > 0)
            { //if wrong orientation, this will force polylines to be inwards always
                for (int i = 0; i < plines_copy.Count; i++)
                    plines_copy[i] = rhino_util.PolylineUtil.Flip(plines_copy[i]).ShiftPline(1);
                // Print("Hi");
            }

            //B = plane;

            if (type == cut_type.mill_project || type == cut_type.slice || type == cut_type.cut_project || type == cut_type.conic || type == cut_type.conic_reverse)
            {
                plines_copy[1] = plines_copy[0].Duplicate();
                var xform = Transform.PlanarProjection(pline_plane1);
                plines_copy[1].Transform(xform);

            }
            else if (type == cut_type.slice_projectsheer || type == cut_type.mill_projectsheer || type == cut_type.cut_projectsheer)
            {
                //Get segments of the two polylines and intersect them with perperdicular to first line segment



                Line plane_line = new Line(plines_copy[0][0], plines_copy[1].SegmentAt(0).ClosestPoint(plines_copy[0][0], false));
                Plane plane0 = new Plane(plane_line.From, plane_line.Direction);
                Plane plane1 = new Plane(plane_line.To, plane_line.Direction);
                plines_copy[1] = plines_copy[0].Duplicate();
                plines_copy[1].Transform(Transform.Translation(plane_line.Direction));

                Line[] lines = new Line[plines_copy[0].Count - 1];// (plines_copy[0][0], plines_copy[1][0]);
                for (int i = 0; i < lines.Length; i++)
                    lines[i] = new Line(plines_copy[0][i], plines_copy[1][i]);




                Polyline pline0 = new Polyline();
                Polyline pline1 = new Polyline();

                for (int i = 0; i < lines.Length; i++)
                {
                    pline0.Add(rhino_util.PlaneUtil.LinePlane(lines[i], plane0));
                    pline1.Add(rhino_util.PlaneUtil.LinePlane(lines[i], plane1));
                }
                pline0.Close();
                pline1.Close();
                plines_copy[0] = pline0;
                plines_copy[1] = pline1;
            }

            if (type == cut_type.cut_reverse || type == cut_type.conic_reverse)
                for (int i = 0; i < plines_copy.Count; i++)
                    plines_copy[i] = rhino_util.PolylineUtil.Flip(plines_copy[i]).ShiftPline(-1);
        }


        public cut duplicate()
        {
            return new cut(this);
        }

        public void transform(Transform xform)
        {
            plines_copy.Transform(xform);
            if (geometry != null)
                geometry.Transform(xform);
        }

        public override string ToString()
        {
            return "type_" + type.ToString();
        }

    }


}
