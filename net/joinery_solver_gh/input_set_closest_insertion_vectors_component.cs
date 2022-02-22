using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class input_set_closest_insertion_vectors_component : GH_Component
    {
        private BoundingBox bbox_preview = BoundingBox.Unset;
        private List<Line> lines_preview = new List<Line>();
        private List<Rhino.Geometry.TextEntity> joint_per_face_current_preview = new List<Rhino.Geometry.TextEntity>();

        public override BoundingBox ClippingBox => bbox_preview;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            foreach (Line l in lines_preview)
                args.Display.DrawLineArrow(l, System.Drawing.Color.FromArgb(207, 0, 90), 2, 1);

            foreach (TextEntity o in joint_per_face_current_preview)
            {
                Plane plane;
                Rhino.RhinoDoc.ActiveDoc.Views.ActiveView.ActiveViewport.GetCameraFrame(out plane);
                plane.Origin = o.Plane.Origin;
                args.Display.Draw3dText(o.PlainText, o.MaskColor,
                  plane, o.TextHeight, "Arial", false, false,
                  Rhino.DocObjects.TextHorizontalAlignment.Center,
                  Rhino.DocObjects.TextVerticalAlignment.Middle);
            }
        }

        protected override void BeforeSolveInstance()
        {
            bbox_preview = BoundingBox.Unset;
            lines_preview = new List<Line>();
            joint_per_face_current_preview = new List<Rhino.Geometry.TextEntity>();
        }

        public input_set_closest_insertion_vectors_component()
          : base("closest_lines", "cp_lines",
              "searches closest edge of a polyline pair list from a list of lines \n lines start point must be on a polyline within 0.01 tolerance",
              "compas_wood", "data")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Polylines", "P", "A list of polyline pairs", GH_ParamAccess.tree);
            pManager.AddLineParameter("Lines", "L", "A list of lines", GH_ParamAccess.tree);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddCurveParameter("Polylines", "P", "A list of polyline pairs", GH_ParamAccess.tree);
            pManager.AddVectorParameter("Vectors", "V", "Insertion Vectors per each face of plates \n indexing: top, bottom faces and side faces", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            //From GH
            GH_Structure<GH_Curve> gh_curves = new GH_Structure<GH_Curve>();
            GH_Structure<GH_Line> gh_lines = new GH_Structure<GH_Line>();
            DA.GetDataTree(0, out gh_curves);
            DA.GetDataTree(1, out gh_lines);

            //Conversion
            Polyline[][] polylines = new Polyline[(int)(gh_curves.DataCount * 0.5)][];
            List<Line> lines = new List<Line>(gh_lines.DataCount);

            int count = 0;
            foreach (GH_Curve gh_curve in gh_curves.AllData(true))
            {
                gh_curve.Value.TryGetPolyline(out Polyline pline);
                if (count % 2 == 0)
                {
                    polylines[(int)(count * 0.5)] = new Polyline[2];
                    polylines[(int)(count * 0.5)][0] = pline;
                }
                else
                {
                    polylines[(int)(count * 0.5)][1] = pline;
                }
                count++;
            }

            foreach (GH_Line gh_line in gh_lines.AllData(true))
            {
                lines.Add(gh_line.Value);
                this.bbox_preview.Union(gh_line.Value.BoundingBox);//display
            }

            //run
            Vector3d[][] insertion_vectors = null;
            joinery_solver_net.Test.pinvoke_find_closest_plateside_to_line(polylines, lines, ref insertion_vectors);

            //output
            DA.SetDataTree(0, rhino_util.GrasshopperUtil.IE2(polylines));
            DA.SetDataTree(1, rhino_util.GrasshopperUtil.IE2(insertion_vectors));

            //display insertion vectors per face
            //Rhino.RhinoApp.WriteLine(lines_preview.Count.ToString());
            for (int i = 0; i < insertion_vectors.Length; i++)
            {
                var pts = new Point3d[insertion_vectors[i].Length];
                pts[0] = polylines[i][0].CenterPoint();
                pts[1] = polylines[i][1].CenterPoint();
                for (int j = 0; j < polylines[i][0].SegmentCount; j++)
                {
                    Polyline rec = new Polyline(new Point3d[] { polylines[i][0][j + 0], polylines[i][0][j + 1], polylines[i][1][j + 1], polylines[i][1][j + 0], polylines[i][0][j + 0], });
                    pts[2 + j] = rec.CenterPoint();
                }

                for (int j = 0; j < insertion_vectors[i].Length; j++)
                {
                    Vector3d v = insertion_vectors[i][j];
                    if ((Math.Abs(v.X) + Math.Abs(v.Y) + Math.Abs(v.Z)) < 0.01) continue;
                    lines_preview.Add(new Line(pts[j], pts[j] + v));
                }
            }
            //Rhino.RhinoApp.WriteLine(lines_preview.Count.ToString());
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.insertion_direction_edge;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("6f698509-a60e-4366-bb0b-a2cf2d2ca564"); }
        }
    }
}
