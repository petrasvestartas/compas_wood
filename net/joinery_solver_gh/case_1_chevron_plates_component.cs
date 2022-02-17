using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class chevron_plates_component : GH_Component
    {
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add sliders
            double[] sliderValue = new double[] { 1, 0.5, 760, 80, 40 };
            double[] sliderMinValue = new double[] { 0, 0.0, 100, 0, 1 };
            double[] sliderMaxValue = new double[] { 10, 2.5, 1000, 100, 40 };
            int[] sliderID = new int[] { 1, 2, 3, 4, 5 };

            for (int i = 0; i < sliderValue.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Number ni = Params.Input[sliderID[i]] as Grasshopper.Kernel.Parameters.Param_Number;
                if (ni == null || ni.SourceCount > 0 || ni.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ni.Attributes.Pivot.X - 250;
                int y = (int)ni.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_NumberSlider slider = new Grasshopper.Kernel.Special.GH_NumberSlider();
                slider.SetInitCode(string.Format("{0}<{1}<{2}", sliderMinValue[i], sliderValue[i], sliderMaxValue[i]));
                slider.CreateAttributes();
                slider.Attributes.Pivot = new System.Drawing.PointF(x, y);
                slider.Attributes.ExpireLayout();
                document.AddObject(slider, false);
                ni.AddSource(slider);
            }
        }

        protected override void AfterSolveInstance()
        {
            GH_Document ghdoc = base.OnPingDocument();
            for (int i = 0; i < ghdoc.ObjectCount; i++)
            {
                IGH_DocumentObject obj = ghdoc.Objects[i];
                if (obj.Attributes.DocObject.ToString().Equals("Grasshopper.Kernel.Special.GH_Group"))
                {
                    Grasshopper.Kernel.Special.GH_Group groupp = (Grasshopper.Kernel.Special.GH_Group)obj;
                    if (groupp.ObjectIDs.Contains(this.InstanceGuid))
                        return;
                }
            }

            List<Guid> guids = new List<Guid>() { this.InstanceGuid };

            foreach (var param in base.Params.Input)
                foreach (IGH_Param source in param.Sources)
                    guids.Add(source.InstanceGuid);

            Grasshopper.Kernel.Special.GH_Group g = new Grasshopper.Kernel.Special.GH_Group();
            g.NickName = base.Name.ToString();

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 255);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }

        private List<Polyline> polylines = new List<Polyline>();
        private BoundingBox bbox = BoundingBox.Unset;

        protected override void BeforeSolveInstance()
        {
            polylines.Clear();
            bbox = BoundingBox.Unset;
        }

        public override BoundingBox ClippingBox => bbox;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            var col = Attributes.Selected ? args.WireColour_Selected : args.WireColour;
            var lineWeight = args.DefaultCurveThickness;

            if (!base.Hidden && !base.Locked)
            {
                foreach (Polyline pline in polylines)
                    args.Display.DrawPolyline(pline, col, lineWeight);
            }
        }

        public chevron_plates_component()
          : base("chevron_plates", "chevron_plates",
              "chevron_plates",
              "compas_wood", "cases")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddMeshParameter("mesh", "mesh", "mesh", GH_ParamAccess.item);
            pManager.AddNumberParameter("edge_rotation", "edge_rotation", "edge_rotation", GH_ParamAccess.item);
            pManager.AddNumberParameter("edge_offset", "edge_offset", "edge_offset", GH_ParamAccess.item);
            pManager.AddNumberParameter("box_height", "box_height", "box_height", GH_ParamAccess.item);
            pManager.AddNumberParameter("top_plate_inlet", "top_plate_inlet", "top_plate_inlet", GH_ParamAccess.item);
            pManager.AddNumberParameter("plate_thickness", "plate_thickness", "plate_thickness", GH_ParamAccess.item);
            pManager.AddBooleanParameter("ortho", "ortho", "ortho", GH_ParamAccess.item, true);
            pManager[6].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddGenericParameter("data", "data", "data", GH_ParamAccess.item);
            //pManager.AddCurveParameter("plines", "plines", "plines", GH_ParamAccess.tree);
            pManager.AddLineParameter("dir", "dir", "dir", GH_ParamAccess.list);
            //pManager.AddVectorParameter("insertion_vectors", "insertion_vectors", "insertion_vectors", GH_ParamAccess.tree);
            //pManager.AddIntegerParameter("joints_per_face", "joints_per_face", "joints_per_face", GH_ParamAccess.tree);
            //pManager.AddIntegerParameter("three_valence", "three_valence", "three_valence", GH_ParamAccess.tree);
            //pManager.AddIntegerParameter("adjacency", "adjacency", "adjacency", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            Mesh mesh = null;
            double edge_rotation = 1;
            double edge_offset = 0.5;
            double box_height = 760;
            double top_plate_inlet = 80;
            double plate_thickness = 40;
            bool ortho = true;

            DA.GetData(0, ref mesh);
            DA.GetData(1, ref edge_rotation);
            DA.GetData(2, ref edge_offset);
            DA.GetData(3, ref box_height);
            DA.GetData(4, ref top_plate_inlet);
            DA.GetData(5, ref plate_thickness);
            DA.GetData(6, ref ortho);

            chevron annen = new chevron(mesh, edge_rotation, edge_offset, box_height, top_plate_inlet, plate_thickness, ortho);
            annen.run();
            var data = new joinery_solver_net.Data
            {
                polylines = rhino_util.GrasshopperUtil.SplitArray(annen.plines, 2),
                face_vectors = annen.insertion_vectors,
                joints_types = annen.joints_per_face,
                three_valence = annen.three_valence,
                adjacency = annen.adjacency,
            };

            DA.SetData(0, data);
            DA.SetDataList(1, annen.box_insertion_lines);

            //Display
            this.polylines = annen.plines;
            this.bbox = annen.plines[0].BoundingBox;
            foreach (var pline in annen.plines)
                this.bbox.Union(pline.BoundingBox);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.annen_plates;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("9deb91eb-6e97-4234-974a-7b0926605f30"); }
        }
    }
}
