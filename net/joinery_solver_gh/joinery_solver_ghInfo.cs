using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace joinery_solver_gh {
    public class joinery_solver_ghInfo : GH_AssemblyInfo {
        public override string Name {
            get {
                return "joinerysolvergh";
            }
        }
        public override Bitmap Icon {
            get {
                //Return a 24x24 pixel bitmap to represent this GHA library.
                return null;
            }
        }
        public override string Description {
            get {
                //Return a short string describing the purpose of this GHA library.
                return "";
            }
        }
        public override Guid Id {
            get {
                return new Guid("9bb16bc1-9a19-4a99-9f0b-1afee092287d");
            }
        }

        public override string AuthorName {
            get {
                //Return a string identifying you or your company.
                return "";
            }
        }
        public override string AuthorContact {
            get {
                //Return a string representing your preferred contact details.
                return "";
            }
        }
    }
}
