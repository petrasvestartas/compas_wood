using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util {

    public static class ColorUtil {

        public static Grasshopper.GUI.Gradient.GH_Gradient Gh2ColorsGradient(Color c0, Color c1, double t0 = 0, double t1 = 1) {
            Grasshopper.GUI.Gradient.GH_Gradient gradient = new Grasshopper.GUI.Gradient.GH_Gradient();
            gradient.AddGrip(t0, c0);
            gradient.AddGrip(t1, c1);
            return gradient;
        }

        public static Grasshopper.GUI.Gradient.GH_Gradient Gh3ColorsGradient(Color c0, Color c1, Color c2, double t0 = 0, double t1 = 0.5, double t2 = 1) {
            Grasshopper.GUI.Gradient.GH_Gradient gradient = new Grasshopper.GUI.Gradient.GH_Gradient();
            gradient.AddGrip(t0, c0);
            gradient.AddGrip(t1, c1);
            gradient.AddGrip(t2, c2);
            return gradient;
        }
    }
}