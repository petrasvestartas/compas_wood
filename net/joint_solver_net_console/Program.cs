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

namespace joint_solver_net_console
{
    internal class Program
    {
        #region Program static constructor

        static Program()
        {
            Resolver.Initialize();
        }

        #endregion Program static constructor

        // Use STAThread for this app as we are starting Rhino in a mode that does actually
        // include user interface (we just never show the main window). This allows for things
        // like RhinoApp().RunScript to properly run.
        [System.STAThread]
        private static void Main(string[] args)
        {
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
new Polyline{new Point3d(79.3722772723983,116.178055434243,0),new Point3d(44.1296460666944,133.883376279235,0),new Point3d(11.1750728299249,112.215022781386,0),new Point3d(13.4631307988593,72.8413484385437,0),new Point3d(48.7057620045632,55.1360275935513,0),new Point3d(81.6603352413327,76.8043810914008,0),new Point3d(79.3722772723983,116.178055434243,0),},
new Polyline{new Point3d(79.3722772723983,116.178055434243,5.91851628154928),new Point3d(44.1296460666944,133.883376279235,5.91851628154928),new Point3d(11.1750728299249,112.215022781386,5.91851628154928),new Point3d(13.4631307988593,72.8413484385437,5.91851628154928),new Point3d(48.7057620045632,55.1360275935513,5.91851628154928),new Point3d(81.6603352413327,76.8043810914008,5.91851628154928),new Point3d(79.3722772723983,116.178055434243,5.91851628154928),},
new Polyline{new Point3d(48.5932410001005,-17.1204299826823,0),new Point3d(105.256703853712,-17.1204299826823,0),new Point3d(48.5932410001005,31.318938635028,0),new Point3d(48.5932410001005,-17.1204299826823,0),},
new Polyline{new Point3d(48.5932410001005,-17.1204299826823,3.89780846412175),new Point3d(105.256703853712,-17.1204299826823,3.89780846412175),new Point3d(48.5932410001005,31.318938635028,3.89780846412175),new Point3d(48.5932410001005,-17.1204299826823,3.89780846412175),},
new Polyline{new Point3d(-46.0627043905459,-17.1204299826823,0),new Point3d(10.6007584630656,-17.1204299826823,0),new Point3d(10.6007584630656,31.318938635028,0),new Point3d(-46.0627043905459,31.318938635028,0),new Point3d(-46.0627043905459,-17.1204299826823,0),},
new Polyline{new Point3d(-46.0627043905459,-17.1204299826823,3.89780846412175),new Point3d(10.6007584630656,-17.1204299826823,3.89780846412175),new Point3d(10.6007584630656,31.318938635028,3.89780846412175),new Point3d(-46.0627043905459,31.318938635028,3.89780846412175),new Point3d(-46.0627043905459,-17.1204299826823,3.89780846412175),},
};

            var polylines_array = new Polyline[(int)(polylines.Count * 0.5)][];
            for (int i = 0; i < polylines_array.Length; i++)
            {
                polylines_array[i] = new Polyline[] { polylines[i * 2 + 0], polylines[i * 2 + 1] };
            }

            var lines = new List<Line>() {
new Line(new Point3d(65.1830486229479,65.9702043424761,5.91851628154928),new Point3d(80.5602903217781,54.6720408086573,5.91851628154928)),
new Line(new Point3d(48.5932410001005,1.85339312857982,0),new Point3d(41.5795635671774,2.84205148477829,0)),
new Line(new Point3d(79.3052666614427,-17.1204299826823,3.89780846412175),new Point3d(84.0932196303112,-26.1752384449411,3.89780846412175)),
new Line(new Point3d(10.6007584630656,4.1522990375585,0),new Point3d(17.8046559730008,5.2014741506303,0)),
new Line(new Point3d(-12.2775187233455,31.318938635028,3.89780846412175),new Point3d(-11.4948856916736,46.0245545911547,3.89780846412175)),
new Line(new Point3d(-46.0627043905459,10.0358113328213,0),new Point3d(-55.6164678402938,10.059307646704,0)),
new Line(new Point3d(-16.6269479562328,-17.1204299826823,3.89780846412175),new Point3d(-16.6269479562328,-27.657211435445,3.89780846412175)),
};

            var points = new List<Point3d>() {
new Point3d(26.6631135952929,66.2098932218454,5.91851628154928),
new Point3d(30.3731679287786,124.838190682803,0),
new Point3d(61.7509616695463,125.030715856739,5.91851628154928),
new Point3d(80.5163062568655,96.4912182628218,5.91851628154928),
new Point3d(76.9249724269063,7.09925432617284,3.89780846412175),
new Point3d(89.4716903982837,-17.1204299826823,3.89780846412175),
};

            var ids = new List<int>() {
0,
1,
2,
3,
4,
5,
};
            Vector3d[][] insertion_vectors = null;
            joinery_solver_net.Test.pinvoke_find_closest_plateside_to_line(polylines_array, lines, ref insertion_vectors);
            int[][] joints_ids = null;
            joinery_solver_net.Test.pinvoke_find_closest_plateside_to_indexedpoint(polylines_array, points, ids, ref joints_ids);

            //if (insertion_vectors != null)
            //{
            //    Console.WriteLine(insertion_vectors.Length.ToString());
            //    foreach (var vecs in insertion_vectors)
            //    {
            //        Console.WriteLine("New");
            //        foreach (var vec in vecs)
            //            Console.WriteLine(vec.ToString());
            //    }
            //}

            if (joints_ids != null)
            {
                Console.WriteLine(joints_ids.Length.ToString());
                foreach (var vecs in joints_ids)
                {
                    Console.WriteLine("New");
                    foreach (var vec in vecs)
                        Console.WriteLine(vec.ToString());
                }
            }

            //var face_vectors = new Vector3d[2][] {
            //    new Vector3d[]{ new Vector3d(90, 91.1, 93) , new Vector3d(94.1, 91, 98.1) , new Vector3d(90.1, 99.4, 92.1) },
            //    new Vector3d[]{ new Vector3d(910.8, 910.1, 930.5) , new Vector3d(940.1, 910, 980.1) , new Vector3d(952.1, 990.4, 920.1) }
            //};

            //var joints_types = new int[][] {
            //    new int[]{  93 , 98, 92 },
            //    new int[]{ 930, 980, 920 }
            //};

            //var three_valence = new List<int[]> {
            //    new int[]{  93 , 98, 92, 98 },
            //    new int[]{ 930, 980, 920, 980 }
            //};

            //var adjacency = new List<int[]>
            //{
            //    //new int[]{ 0,1 }
            //};

            //Data data = new Data { polylines = polylines, face_vectors = face_vectors, joints_types = joints_types, three_valence = three_valence, adjacency = adjacency };

            //double division_length = 300;
            //var jointparams = new List<double>{
            //    division_length, 0.5, 9,
            //    division_length * 1.5,0.65,10,
            //    division_length * 1.5, 0.5,21,
            //    division_length, 0.5,30,
            //    division_length, 0.5,40,
            //    division_length, 0.5,50
            //};

            //int search_type = 0;
            //double division_distance = 300;
            //double shift = 0.6;
            //int output_type = 4;
            //int triangulate = 0;
            //Polyline[][] out_polylines = null;
            //joinery_solver_net.Test.pinvoke_get_connection_zones(ref out_polylines, data, jointparams, search_type, division_distance, shift, output_type, triangulate);

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
