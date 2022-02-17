using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace joinery_solver_gh
{
    public class input_connection_params_component : GH_Component
    {
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            double division_length = 300;
            List<double> jointparams = new List<double>{
        division_length, 0.5, 9,
        division_length * 1.5,0.65,10,
        division_length * 1.5, 0.5,21,
        division_length, 0.5,30,
        division_length, 0.5,40,
        division_length, 0.5,50
        };

            //Add sliders
            double[] sliderValue = new double[] {
        division_length, 0.5, 9,
        division_length * 1.5,0.65,10,
        division_length * 1.5, 0.5,21,
        division_length, 0.5,30,
        division_length, 0.5,40,
        division_length, 0.5,50
            };
            double[] sliderMinValue = new double[] {
        division_length*0.1, 0, 1,
        division_length*0.1,0,10,
        division_length*0.1, 0,20,
        division_length*0.1,0,30,
        division_length*0.1,0,40,
        division_length*0.1, 0,50
            };

            double[] sliderMaxValue = new double[] {
        division_length*10, 1, 9,
        division_length*10,1,19,
        division_length*10, 1,29,
        division_length*10, 1,39,
        division_length*10, 1,49,
        division_length*10, 1,59
            };

            int[] sliderID = new int[] {
                0, 1, 2,
                3,4,5,
                6,7,8,
                9,10,11,
                 12,13,14,
                 15,16,17,
            };

            for (int i = 0; i < sliderValue.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Number ni = Params.Input[sliderID[i]] as Grasshopper.Kernel.Parameters.Param_Number;
                if (ni == null || ni.SourceCount > 0 || ni.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ni.Attributes.Pivot.X - 250;
                int y = (int)ni.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_NumberSlider slider = new Grasshopper.Kernel.Special.GH_NumberSlider();
                slider.Slider.DrawControlBackground = true;
                slider.Slider.DrawControlBorder = true;
                slider.Slider.ControlEdgeColour = System.Drawing.Color.Blue;
                slider.Slider.ControlBackColour = System.Drawing.Color.Aquamarine;
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

        public input_connection_params_component()
          : base("joints_params", "joints_params",
              "joints_params",
              "compas_wood", "data")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddNumberParameter("ss_e_ip_len", "ss_e_ip_len", "ss_e_ip_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "ss_e_ip_shift", "ss_e_ip_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "ss_e_ip_type", "ss_e_ip_type", GH_ParamAccess.item);

            pManager.AddNumberParameter("ss_e_op_len", "ss_e_op_len", "ss_e_op_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "ss_e_op_shift", "ss_e_op_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "ss_e_op_type", "ss_e_op_type", GH_ParamAccess.item);

            pManager.AddNumberParameter("ts_e_p_len", "ts_e_p_len", "ts_e_p_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "ts_e_p_shift", "ts_e_p_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "ts_e_p_type", "ts_e_p_type", GH_ParamAccess.item);

            pManager.AddNumberParameter("cr_c_ip_len", "cr_c_ip_len", "cr_c_ip_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "cr_c_ip_shift", "cr_c_ip_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "cr_c_ip_type", "cr_c_ip_type", GH_ParamAccess.item);

            pManager.AddNumberParameter("tt_e_p_len", "tt_e_p_len", "tt_e_p_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "tt_e_p_shift", "tt_e_p_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "tt_e_p_type", "tt_e_p_type", GH_ParamAccess.item);

            pManager.AddNumberParameter("ss_e_r_len", "ss_e_r_len", "ss_e_r_len", GH_ParamAccess.item);
            pManager.AddNumberParameter("shift", "ss_e_r_shift", "ss_e_r_shift", GH_ParamAccess.item);
            pManager.AddNumberParameter("type", "ss_e_r_type", "ss_e_r_type", GH_ParamAccess.item);

            for (int i = 0; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddGenericParameter("p", "p", "p", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            List<double> jointparams = new List<double>();
            for (int i = 0; i < 18; i++)
            {
                double number = i;
                DA.GetData(i, ref number);
                jointparams.Add(number);
            }
            DA.SetDataList(0, jointparams);
            //    var ss_e_ip = new List<double>();
            //    var ss_e_op = new List<double>();
            //    var ts_e_p = new List<double>();
            //    var cr_c_ip = new List<double>();
            //    var tt_e_p = new List<double>();
            //    var ss_e_r = new List<double>();

            //    DA.GetDataList(0, ss_e_ip);
            //    DA.GetDataList(1, ss_e_op);
            //    DA.GetDataList(2, ts_e_p);
            //    DA.GetDataList(3, cr_c_ip);
            //    DA.GetDataList(4, tt_e_p);
            //    DA.GetDataList(5, ss_e_r);

            //    double division_length = 300;
            ////    List<double> jointparams = new List<double>{
            ////division_length, 0.5, 9,
            ////division_length * 1.5,0.65,10,
            ////division_length * 1.5, 0.5,21,
            ////division_length, 0.5,30,
            ////division_length, 0.5,40,
            ////division_length, 0.5,50
            ////};

            //    if (ss_e_ip.Count == 3)
            //    {
            //        jointparams[0 * 3 + 0] = ss_e_ip[0];
            //        jointparams[0 * 3 + 1] = ss_e_ip[1];
            //        jointparams[0 * 3 + 2] = ss_e_ip[2];
            //    }

            //    if (ss_e_op.Count == 3)
            //    {
            //        jointparams[1 * 3 + 0] = ss_e_op[0];
            //        jointparams[1 * 3 + 1] = ss_e_op[1];
            //        jointparams[1 * 3 + 2] = ss_e_op[2];
            //    }

            //    if (ts_e_p.Count == 3)
            //    {
            //        jointparams[2 * 3 + 0] = ts_e_p[0];
            //        jointparams[2 * 3 + 1] = ts_e_p[1];
            //        jointparams[2 * 3 + 2] = ts_e_p[2];
            //    }

            //    if (cr_c_ip.Count == 3)
            //    {
            //        jointparams[3 * 3 + 0] = cr_c_ip[0];
            //        jointparams[3 * 3 + 1] = cr_c_ip[1];
            //        jointparams[3 * 3 + 2] = cr_c_ip[2];
            //    }

            //    if (tt_e_p.Count == 3)
            //    {
            //        jointparams[4 * 3 + 0] = tt_e_p[0];
            //        jointparams[4 * 3 + 1] = tt_e_p[1];
            //        jointparams[4 * 3 + 2] = tt_e_p[2];
            //    }

            //    if (ss_e_r.Count == 3)
            //    {
            //        jointparams[5 * 3 + 0] = ss_e_r[0];
            //        jointparams[5 * 3 + 1] = ss_e_r[1];
            //        jointparams[5 * 3 + 2] = ss_e_r[2];
            //    }
            //    DA.SetDataList(0, jointparams);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.joint_params;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("9deb91eb-6e97-4234-974a-7b0926604f11"); }
        }
    }
}
