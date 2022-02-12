using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using joinery_solver_net;
using Rhino.Runtime.InProcess;
using Rhino.Geometry;
using Rhino;
using System.IO;

//namespace joint_solver_net_console {
//    internal class Program {
//        private static void Main(string[] args) {
//            Console.WriteLine(joinery_solver_net.Test.get_square(4));
//            joinery_solver_net.Test.test_set_coord();
//            joinery_solver_net.Test.test_get_coord();
//            joinery_solver_net.Test.pinvoke_get_connection_zones();
//        }
//    }
//}

namespace joint_solver_net_console {

    internal class Program {

        #region Program static constructor

        static Program() {
            Resolver.Initialize();
        }

        #endregion Program static constructor

        // Use STAThread for this app as we are starting Rhino in a mode that does actually
        // include user interface (we just never show the main window). This allows for things
        // like RhinoApp().RunScript to properly run.
        [System.STAThread]
        private static void Main(string[] args) {
            //Console.WriteLine(joinery_solver_net.Test.get_square(4));
            //joinery_solver_net.Test.test_set_coord();
            //joinery_solver_net.Test.test_get_coord();

            //    var polylines = new List<Polyline>() {
            //        new Polyline{ new Point3d(0, 0, 0) , new Point3d(0, 10, 0) , new Point3d(10, 10, 0), new Point3d(10, 0, 0),new Point3d(0, 0, 0) },
            //        new Polyline{ new Point3d(0, 0, 1), new Point3d(0, 10, 1), new Point3d(10, 10, 1), new Point3d(10, 0, 1), new Point3d(0, 0, 1) },
            //new Polyline{ new Point3d(10, 0, 0) , new Point3d(10, 10, 0) , new Point3d(20, 10, 0), new Point3d(20, 0, 0),new Point3d(10, 0, 0) },
            //new Polyline{ new Point3d(10, 0, 1) , new Point3d(10, 10, 1) , new Point3d(20, 10, 1), new Point3d(20, 0, 1),new Point3d(10, 0, 1) }
            //    };

            var polylines = new List<Polyline>() {
new Polyline{new Point3d(17032.7530566426,2041.83227154561,-4835.73063596581),new Point3d(17094.9673755385,2046.03452535024,-5525.46630859375),new Point3d(17513.3924786418,2879.22900390625,-5525.46630859375),new Point3d(17401.6367556032,2879.22900390625,-4242.99021175908),new Point3d(17032.7530566426,2041.83227154561,-4835.73063596581),},
new Polyline{new Point3d(17067.7501393799,2023.13252353802,-4829.898636052),new Point3d(17130.4905060312,2027.37030913374,-5525.46630859375),new Point3d(17558.2886628402,2879.22900390625,-5525.46630859375),new Point3d(17444.8713104347,2879.22900390625,-4223.9218375698),new Point3d(17067.7501393799,2023.13252353802,-4829.898636052),},
new Polyline{new Point3d(16507.7968155834,2322.32849165945,-4923.21063467291),new Point3d(16562.1204181476,2325.99776859774,-5525.46630859375),new Point3d(16839.9497156666,2879.22900390625,-5525.46630859375),new Point3d(16753.1184331296,2879.22900390625,-4529.01582459816),new Point3d(16507.7968155834,2322.32849165945,-4923.21063467291),},
new Polyline{new Point3d(16542.7938983207,2303.62874365186,-4917.3786347591),new Point3d(16597.6435486403,2307.33355238124,-5525.46630859375),new Point3d(16884.845899865,2879.22900390625,-5525.46630859375),new Point3d(16796.3529879612,2879.22900390625,-4509.94745040889),new Point3d(16542.7938983207,2303.62874365186,-4917.3786347591),},
new Polyline{new Point3d(17505.6697484159,2879.22900390625,-4148.80370539581),new Point3d(16684.2132066161,2879.22900390625,-4511.10281499199),new Point3d(16435.4244846789,2314.45787580948,-4910.8687384917),new Point3d(17100.3690566873,1959.16266366556,-4800.06074012918),new Point3d(17505.6697484159,2879.22900390625,-4148.80370539581),},
new Polyline{new Point3d(17509.7231426821,2879.22900390625,-4195.3192762859),new Point3d(16688.2666008823,2879.22900390625,-4557.61838588207),new Point3d(16455.3070643454,2350.39144557014,-4931.94919773148),new Point3d(17120.2516363538,1995.09623342623,-4821.14119936895),new Point3d(17509.7231426821,2879.22900390625,-4195.3192762859),},
new Polyline{new Point3d(17100.3690566872,1959.16266366542,-4800.0607401291),new Point3d(16435.4244846789,2314.45787580961,-4910.86873849178),new Point3d(16490.8613291635,2318.20234645217,-5525.46630859375),new Point3d(17165.8008085253,1963.58223833866,-5525.46630859375),new Point3d(17100.3690566872,1959.16266366542,-4800.0607401291),},
new Polyline{new Point3d(17120.2442012279,1995.08279600424,-4821.13331629392),new Point3d(16455.2996292197,2350.37800814843,-4931.94131465661),new Point3d(16508.8357224085,2353.99409292249,-5525.46630859375),new Point3d(17183.7752017703,1999.37398480899,-5525.46630859375),new Point3d(17120.2442012279,1995.08279600424,-4821.13331629392),},
};

            var face_vectors = new Vector3d[2][] {
                new Vector3d[]{ new Vector3d(90, 91.1, 93) , new Vector3d(94.1, 91, 98.1) , new Vector3d(90.1, 99.4, 92.1) },
                new Vector3d[]{ new Vector3d(910.8, 910.1, 930.5) , new Vector3d(940.1, 910, 980.1) , new Vector3d(952.1, 990.4, 920.1) }
            };

            var joints_types = new int[][] {
                new int[]{  93 , 98, 92 },
                new int[]{ 930, 980, 920 }
            };

            var three_valence = new List<int[]> {
                new int[]{  93 , 98, 92, 98 },
                new int[]{ 930, 980, 920, 980 }
            };

            var adjacency = new List<int[]> {
                //new int[]{ 0,1 }
            };

            Data data = new Data { polylines = polylines, face_vectors = face_vectors, joints_types = joints_types, three_valence = three_valence, adjacency = adjacency };

            double division_length = 300;
            var jointparams = new List<double>{
                division_length, 0.5, 9,
                division_length * 1.5,0.65,10,
                division_length * 1.5, 0.5,21,
                division_length, 0.5,30,
                division_length, 0.5,40,
                division_length, 0.5,50
            };

            int search_type = 0;
            double division_distance = 300;
            double shift = 0.6;
            int output_type = 4;
            int triangulate = 0;
            Polyline[][] out_polylines = null;
            joinery_solver_net.Test.pinvoke_get_connection_zones(ref out_polylines, data, jointparams, search_type, division_distance, shift, output_type, triangulate);

            //Console.WriteLine("Please wait while Rhino is starting...");
            //try {
            //    using (new RhinoCore(new string[] { "/NOSPLASH" }, WindowStyle.Hidden)) {
            //        Console.WriteLine("Enter path to directory which contains files to convert and press ENTER:");
            //        string path = Console.ReadLine();

            //        string[] filePaths = Directory.GetFiles(path, "*.3dm");
            //        if (filePaths == null || filePaths.Length == 0) {
            //            Console.WriteLine("Directory is empty. No files to process. Press any key to exit.");
            //            Console.ReadKey();
            //            return;
            //        }

            //        foreach (string file in filePaths) {
            //            var doc = RhinoDoc.Open(file, out bool wasOpen);

            //            Console.WriteLine("Nº of objects in file: {0}", doc.Objects.Count);

            //            // View capture to .png file
            //            var pngPath = Path.ChangeExtension(file, ".png");
            //            var imgScript = string.Format("_-ViewCaptureToFile \"{0}\" _Enter", pngPath);
            //            RhinoApp.RunScript(imgScript, false);

            //            // Save the .obj file
            //            var fowo = new Rhino.FileIO.FileObjWriteOptions(new Rhino.FileIO.FileWriteOptions { SuppressAllInput = true }) {
            //                ExportMaterialDefinitions = false,
            //                MapZtoY = true,
            //                MeshParameters = MeshingParameters.Default,
            //            };

            //            var objPath = Path.ChangeExtension(file, ".obj");
            //            var result = Rhino.FileIO.FileObj.Write(objPath, doc, fowo);

            //            if (result == Rhino.PlugIns.WriteFileResult.Success)
            //                Console.WriteLine("Converted file: {0}", objPath);
            //            else
            //                Console.WriteLine("File conversion failed.");
            //        }

            //        Console.WriteLine("Finished converting. Press any key to exit...");
            //        Console.ReadKey();
            //    }
            //} catch (Exception ex) {
            //    Console.Error.WriteLine(ex.Message);
            //    Console.WriteLine("press any key to exit");
            //    Console.ReadKey();
            //}
        }
    }
}
