using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace joinery_solver_gh {

    public class joinery_solver_ghInfo : GH_AssemblyInfo {

        public override string Name {
            get {
                return "compas_wood";
            }
        }

        public override Bitmap Icon {
            get {
                return Properties.Resources.compas_wood;
            }
        }

        public override string Description {
            get {
                return "https://ibois-epfl.github.io/compas_wood/latest/";
            }
        }

        public override Guid Id {
            get {
                return new Guid("9bb16bc1-9a19-4a99-9f0b-1afee092287d");
            }
        }

        public override string AuthorName {
            get {
                return "petras_vestartas";
            }
        }

        public override string AuthorContact {
            get {
                return "petrasvestartas@gmail.com";
            }
        }
    }
}
