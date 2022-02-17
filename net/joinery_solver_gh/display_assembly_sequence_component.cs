using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class display_assembly_sequence_component : GH_Component
    {
        private BoundingBox bbox = BoundingBox.Unset;
        private List<Line> lines = new List<Line>();
        private List<Line> insertion_vectors_current = new List<Line>();
        private List<Rhino.Geometry.TextEntity> joint_per_face_current = new List<Rhino.Geometry.TextEntity>();
        private List<Polyline> polylines = new List<Polyline>();

        public override BoundingBox ClippingBox => base.ClippingBox;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            foreach (Line l in lines)
                args.Display.DrawLineArrow(l, System.Drawing.Color.FromArgb(207, 0, 90), 2, 100);

            foreach (var o in polylines)
                args.Display.DrawPolyline(o, System.Drawing.Color.FromArgb(207, 0, 90), 2);

            foreach (Line l in insertion_vectors_current)
                args.Display.DrawLine(l, System.Drawing.Color.FromArgb(0, 0, 0), 5);

            foreach (TextEntity o in joint_per_face_current)
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

        public override void DrawViewportMeshes(IGH_PreviewArgs args)
        {
            base.DrawViewportMeshes(args);
        }

        public display_assembly_sequence_component()
          : base("assembly", "assembly",
              "assembly",
              "compas_wood", "view")
        {
        }

        // DataTree<Polyline> plines, List<Line> box_insertion_lines, DataTree<Vector3d> insertion_vectors, DataTree<int> joint_per_face, double slider, ref object A) {
        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.item);
            pManager.AddLineParameter("dir", "dir", "directions as lines", GH_ParamAccess.list);
            pManager.AddIntegerParameter("n", "n", "polylines per element", GH_ParamAccess.item, 4);
            pManager.AddNumberParameter("t", "t", "t", GH_ParamAccess.item);
            pManager[2].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            //Params
            bbox = BoundingBox.Unset;
            lines = new List<Line>();
            insertion_vectors_current = new List<Line>();
            joint_per_face_current = new List<Rhino.Geometry.TextEntity>();
            polylines = new List<Polyline>();

            //Input
            var data = new joinery_solver_net.Data();
            DA.GetData(0, ref data);
            DA.GetDataList(1, lines);
            int group_count = 8;
            DA.GetData(2, ref group_count);
            double slider = 0;
            DA.GetData(3, ref slider);

            bbox = lines[0].BoundingBox;
            for (int i = 1; i < lines.Count; i++)
                bbox.Union(lines[i].BoundingBox);

            //add previous boxes
            for (int i = 0; i < Math.Floor(slider); i++)
            {
                for (int j = 0; j < group_count; j++)
                    polylines.AddRange(data.polylines[i * group_count + j]);
            }

            //add current box
            if (slider % 1 > 0)
            {
                int id = (int)Math.Floor(slider);
                var temp_plines = new List<Polyline>(8);

                for (int j = 0; j < group_count; j++)
                    temp_plines.AddRange(data.polylines[id * group_count + j]);

                //Print(id.ToString());
                for (int i = 0; i < temp_plines.Count; i++)
                {
                    Vector3d v = lines[id].From - lines[id].To;
                    v.ToString();
                    Polyline pli = temp_plines[i].Duplicate();
                    pli.Transform(Transform.Translation(v * (1 - (slider % 1))));
                    temp_plines[i] = pli;
                }
                polylines.AddRange(temp_plines);

                //Iterate plate
                for (int i = 0; i < temp_plines.Count; i += 2)
                {
                    var pts = new List<Point3d>();
                    pts.Add(temp_plines[i + 0].CenterPoint());
                    pts.Add(temp_plines[i + 1].CenterPoint());

                    for (int j = 0; j < temp_plines[i + 0].Count - 1; j++)
                    {
                        pts.Add((((temp_plines[i + 0][j] + temp_plines[i + 0][j + 1]) * 0.5) + ((temp_plines[i + 1][j] + temp_plines[i + 1][j + 1]) * 0.5)) * 0.5);
                    }

                    //Iterate center of a plate sides
                    for (int j = 0; j < pts.Count; j++)
                    {
                        Vector3d v = data.face_vectors[(id * 4) + (int)(i * 0.5)][j];
                        //Print(pts.Count.ToString());
                        //Print((id * 4).ToString());
                        //Print(((int)(i*0.5)).ToString());

                        if (v == Vector3d.Zero)
                            v = new Vector3d(0, 0, 0.0001);
                        insertion_vectors_current.Add(new Line(pts[j], pts[j] + v * 300));

                        //text
                        string text = data.joints_types[(int)(i * 0.5)][j].ToString();
                        //Print(text.ToString());
                        var text_entity = new TextEntity
                        {
                            Plane = new Plane(pts[j], Vector3d.ZAxis),
                            PlainText = text,
                            MaskColor = System.Drawing.Color.FromArgb(i * 30, i * 10, 255 - i * 30),
                            TextHeight = 30 + i * 5
                        };

                        joint_per_face_current.Add(text_entity);
                    }
                }
            }
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.insertion_sequence;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("ed02439c-2577-4599-b6e3-806dd3813ca9"); }
        }
    }
}
