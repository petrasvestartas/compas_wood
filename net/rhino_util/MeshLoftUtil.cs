using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace rhino_util
{
    public static class MeshLoftUtil
    {
        public static int[] sort_set_of_points(Point3d[] xArray, Point3d[] yArray)
        {
            Point3d[] xArray_ = xArray.Duplicate();
            int[] xIndex = new int[xArray_.Length];
            Point3d[] yArray_ = yArray.Duplicate();
            int[] yIndex = new int[yArray_.Length];
            for (int i = 0; i < xArray_.Length; i++) xIndex[i] = yIndex[i] = i;
            Array.Sort(xArray_, xIndex);
            Array.Sort(yArray_, yIndex);
            Array.Sort(yIndex, xIndex);
            return xIndex;
        }

        public static Mesh loft_polylines_with_holes(List<Curve> curves0, List<Curve> curves1)
        {
            if (curves0.Count == 0 && curves1.Count == 0) return new Mesh();

            List<Curve> curves0_ = new List<Curve>(curves0.Count);
            List<Curve> curves1_ = new List<Curve>(curves0.Count);
            if (curves1.Count == 0)
            {
                for (int i = 0; i < curves0.Count; i++)
                {//+=2
                    if (i < curves0.Count * 0.5)
                    {
                        curves0_.Add(curves0[i]);//+ 0
                    }
                    else
                    {
                        curves1_.Add(curves0[i]);//+ 1
                    }
                }
            }
            else if (curves0.Count == 0 && curves1.Count != 0)
            {
                //Rhino.RhinoApp.WriteLine("Hi");
                for (int i = 0; i < curves1.Count; i += 2)
                {//+=2
                    curves0_.Add(curves1[i + 0]);//+ 0

                    curves1_.Add(curves1[i + 1]);//+ 1
                }
            }
            else if ((curves0.Count != 0 && curves1.Count != 0) && (curves0.Count == curves1.Count))
            {
                curves0_ = curves0;
                curves1_ = curves1;
            }
            curves0 = curves0_;
            curves1 = curves1_;

            double len = -1;
            int id = -1;

            int counter = 0;
            double[] lens = new double[curves0.Count];
            foreach (Curve c in curves0)
            {
                double tempLen = c.GetBoundingBox(true).Diagonal.Length;

                if (tempLen > len)
                {
                    len = tempLen;
                    id = counter;
                }
                counter++;
            }

            Polyline pL0 = curves0[id].ToPolylineFromCP();
            Polyline pL1 = curves1[id].ToPolylineFromCP();
            //pL0.Bake();
            List<Curve> crv0 = new List<Curve>();
            List<Curve> crv1 = new List<Curve>();
            for (int i = 0; i < curves0.Count; i++)
            {
                if (i != id)
                {
                    crv0.Add(curves0[i]);
                    crv1.Add(curves1[i]);
                }
            }

            Mesh mesh0 = Rhino.Geometry.Mesh.CreatePatch(pL0, Rhino.RhinoDoc.ActiveDoc.ModelAngleToleranceRadians, null, crv0, null, null, true, 1);

            //Math mesh vertics to polylines points
            Point3d[] ptsMesh = mesh0.Vertices.ToPoint3dArray();

            Point3d[] ptsPlines0 = new Point3d[ptsMesh.Length];
            Point3d[] ptsPlines1 = new Point3d[ptsMesh.Length];
            int count = 0;
            for (int i = 0; i < pL0.Count - 1; i++)
            {
                ptsPlines0[count] = pL0[i];
                ptsPlines1[count++] = pL1[i];
            }

            for (int i = 0; i < crv0.Count; i++)
            {
                Polyline crv0Plines = crv0[i].ToPolyline();
                Polyline crv1Plines = crv1[i].ToPolyline();
                for (int j = 0; j < crv0Plines.Count - 1; j++)
                {
                    ptsPlines0[count] = crv0Plines[j];
                    ptsPlines1[count++] = crv1Plines[j];
                }
            }

            //Search ids to match mesh v to crv v
            int[] cpID = ClosestPointUtil.RTreeSearchIDOnlyOne(ptsPlines0, ptsMesh, Rhino.RhinoDoc.ActiveDoc.ModelAngleToleranceRadians); //use dictionary search istead

            Mesh mesh1 = mesh0.DuplicateMesh();
            for (int i = 0; i < cpID.Length; i++)
            {
                mesh1.Vertices.SetVertex(i, ptsPlines1[cpID[i]]);
            }

            //Create walls
            Mesh mesh = new Mesh();
            mesh.Append(mesh0);
            mesh.Append(mesh1);
            Mesh meshWalls = mesh.DuplicateMesh();
            meshWalls.Faces.Clear();

            int n = mesh0.Vertices.Count;
            for (int i = 0; i < mesh0.TopologyEdges.Count; i++)
            {//(mesh0.TopologyEdges.Count * 0.5)
                if (mesh0.TopologyEdges.GetConnectedFaces(i).Length == 1)
                {
                    Rhino.IndexPair pair = mesh0.TopologyEdges.GetTopologyVertices(i);
                    mesh.Faces.AddFace(pair.I, pair.J, pair.J + n, pair.I + n);
                    meshWalls.Faces.AddFace(pair.I, pair.J, pair.J + n, pair.I + n);
                }
            }

            mesh.UnifyNormals();
            meshWalls.RebuildNormals();
            mesh.Ngons.AddPlanarNgons(Rhino.RhinoDoc.ActiveDoc.ModelAbsoluteTolerance * 10, 3, 3, true);

            Mesh explodedMesh = new Mesh();
            IEnumerable<MeshNgon> ngonFaces = mesh.GetNgonAndFacesEnumerable();

            foreach (MeshNgon ngon in ngonFaces)
            {
                int[] meshFaceList = System.Linq.Enumerable.ToArray(System.Linq.Enumerable.Select(ngon.FaceIndexList(), j => unchecked((int)j)));
                Mesh NGonFaceMesh = mesh.DuplicateMesh().Faces.ExtractFaces(meshFaceList);
                explodedMesh.Append(NGonFaceMesh);
            }
            explodedMesh.RebuildNormals();

            //DA.SetData(1, explodedMesh);
            //DA.SetData(0, meshWalls);
            //explodedMesh.VertexColors.Clear();
            //this.PreparePreview(meshWalls, DA.Iteration, new List<Polyline> { pL0, pL1 });

            return explodedMesh;
        }
    }
}