using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class input_set_closest_indexed_points : GH_Component

    {
        private BoundingBox bbox_preview = BoundingBox.Unset;
        private List<Line> lines_preview = new List<Line>();
        private List<Rhino.Geometry.TextEntity> joint_per_face_current_preview = new List<Rhino.Geometry.TextEntity>();

        private List<Point3d> pts0 = new List<Point3d>();
        private List<Point3d> pts1 = new List<Point3d>();
        private List<Point3d> pts2 = new List<Point3d>();
        private List<Point3d> pts3 = new List<Point3d>();
        private List<Point3d> pts4 = new List<Point3d>();
        private List<Point3d> pts5 = new List<Point3d>();
        private List<Point3d> pts6 = new List<Point3d>();

        public override BoundingBox ClippingBox => bbox_preview;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            foreach (Line l in lines_preview)
                args.Display.DrawLineArrow(l, System.Drawing.Color.FromArgb(207, 0, 90), 2, 0.1);

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

            args.Display.DrawPoints(pts0, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Black);
            args.Display.DrawPoints(pts1, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Green);
            args.Display.DrawPoints(pts2, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Blue);
            args.Display.DrawPoints(pts3, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Red);
            args.Display.DrawPoints(pts4, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Violet);
            args.Display.DrawPoints(pts5, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Navy);
            args.Display.DrawPoints(pts6, Rhino.Display.PointStyle.RoundSimple, 3, System.Drawing.Color.Yellow);
        }

        protected override void BeforeSolveInstance()
        {
            bbox_preview = BoundingBox.Unset;
            lines_preview = new List<Line>();
            joint_per_face_current_preview = new List<Rhino.Geometry.TextEntity>();
            pts0.Clear();
            pts1.Clear();
            pts2.Clear();
            pts3.Clear();
            pts4.Clear();
            pts5.Clear();
            pts6.Clear();
        }

        public input_set_closest_indexed_points()
          : base("closest_indexed_points", "cp_lines",
              "searches closest edge of a polyline pair list from a list of indexed points",
              "compas_wood", "data")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Polylines", "P", "A list of polyline pairs", GH_ParamAccess.tree);
            pManager.AddPointParameter("Points", "P", "A list of points", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("Ints", "I", "Indices representing joint types", GH_ParamAccess.tree);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddCurveParameter("Polylines", "P", "A list of polyline pairs", GH_ParamAccess.tree);
            pManager.AddIntegerParameter("Ints", "I", "Indices representing joint types", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            //From GH
            GH_Structure<GH_Curve> gh_curves = new GH_Structure<GH_Curve>();
            GH_Structure<GH_Point> gh_pts = new GH_Structure<GH_Point>();
            GH_Structure<GH_Integer> gh_ids = new GH_Structure<GH_Integer>();
            DA.GetDataTree(0, out gh_curves);
            DA.GetDataTree(1, out gh_pts);
            DA.GetDataTree(2, out gh_ids);

            //Conversion
            Polyline[][] polylines = new Polyline[(int)(gh_curves.DataCount * 0.5)][];
            List<Point3d> pts = new List<Point3d>(gh_pts.DataCount);
            List<int> ids = new List<int>(gh_ids.DataCount);
            if (gh_pts.DataCount != gh_ids.DataCount) return;

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

            foreach (GH_Point gh_p in gh_pts.AllData(true))
            {
                pts.Add(gh_p.Value);
                this.bbox_preview.Union(gh_p.Value);//display
            }

            foreach (GH_Integer gh_i in gh_ids.AllData(true))
                ids.Add(gh_i.Value);

            //run
            int[][] joints_ids = null;
            joinery_solver_net.Test.pinvoke_find_closest_plateside_to_indexedpoint(polylines, pts, ids, ref joints_ids);

            //output
            DA.SetDataTree(0, rhino_util.GrasshopperUtil.IE2(polylines));
            DA.SetDataTree(1, rhino_util.GrasshopperUtil.IE2(joints_ids));

            //display points
            for (int i = 0; i < joints_ids.Length; i++)
            {
                var pts_display = new Point3d[joints_ids[i].Length];
                pts_display[0] = polylines[i][0].CenterPoint();
                pts_display[1] = polylines[i][1].CenterPoint();
                for (int j = 0; j < polylines[i][0].SegmentCount; j++)
                {
                    Polyline rec = new Polyline(new Point3d[] { polylines[i][0][j + 0], polylines[i][0][j + 1], polylines[i][1][j + 1], polylines[i][1][j + 0], polylines[i][0][j + 0], });
                    pts_display[2 + j] = rec.CenterPoint();
                }

                for (int j = 0; j < joints_ids[i].Length; j++)
                {
                    var text_entity = new TextEntity
                    {
                        Plane = new Plane(pts_display[j], Vector3d.ZAxis),
                        PlainText = joints_ids[i][j].ToString(),
                        MaskColor = System.Drawing.Color.FromArgb(i * 30, i * 10, 255 - i * 30),
                        TextHeight = 1
                    };

                    if (joints_ids[i][j] < 1) pts0.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 1 && joints_ids[i][j] < 10) pts1.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 10 && joints_ids[i][j] < 20) pts2.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 20 && joints_ids[i][j] < 30) pts3.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 30 && joints_ids[i][j] < 40) pts4.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 50 && joints_ids[i][j] < 60) pts5.Add(pts_display[j]);
                    else if (joints_ids[i][j] >= 60 && joints_ids[i][j] < 70) pts6.Add(pts_display[j]);

                    this.joint_per_face_current_preview.Add(text_entity);
                }
            }
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.joint_ids;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("15a08b2b-6bab-4a04-b2cc-f6516eadff7c"); }
        }
    }
}
