

using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon
{
    //Sawblade cut must contain 4 point polylines
    //It consists of one segment as a path and other segment as normal
    public class CutSawBladeBisector : Cut
    {

     

        public CutSawBladeBisector(int id, CutType cutType, Plane refPlane, List<Plane> planes) : base( id, cutType, refPlane, planes)
        {//, List<double> speeds, int smooth , speeds, smooth


        }

        public CutSawBladeBisector(Cut cut) : base(cut.id, cut.cutType, cut.refPlane, cut.planes)
        {//, cut.speeds, cut.smooth

            this.plines = cut.plines;
        }
        public CutSawBladeBisector(int id, IEnumerable<Polyline> geo, CutType cutType, Plane refPlane, List<Plane> planes) : base(id,cutType, refPlane, planes)
        {//, List<double> speeds, int smooth // , speeds, smooth

            this.plines = geo.ToList();
        }

 

        public List<List<Plane>> CreatePyramidCut(double r, double step, double retreate = 10, double retreateZ = -1, double stepPyramid=50) {

            var pyramids = new List<List<Plane>>((int)(this.plines.Count * 0.5));

            List<Polyline> quads = new List<Polyline>();

            //  1/---------------\2
            //  0-----------------3

            //Interpolate Quads


            List<List<Polyline>> interpolatedQuadsNested = new List<List<Polyline>>();

            for (int i = 0; i < this.plines.Count; i += 2) {
                List<Polyline> interpolatedQuadsLocal = new List<Polyline>();
                int divisions = (int)Math.Round(this.plines[i][0].DistanceTo(this.plines[i][1]) / stepPyramid, 0);
                Point3d[] p0A = PointUtil.InterpolatePoints(this.plines[i][0], this.plines[i][1], divisions, true);
                Point3d[] p0B = PointUtil.InterpolatePoints(this.plines[i][3], this.plines[i][2], divisions, true);

                Point3d[] p1A = PointUtil.InterpolatePoints(this.plines[i + 1][0], this.plines[i + 1][1], divisions, true);
                Point3d[] p1B = PointUtil.InterpolatePoints(this.plines[i + 1][3], this.plines[i + 1][2], divisions, true);

                for(int j = 0; j < p0A.Length-1; j++) {
                    interpolatedQuadsLocal.Add(new Polyline() { p0A[0], p0A[j + 1], p0B[j + 1], p0B[0], p0A[0] });
                    interpolatedQuadsLocal.Add(new Polyline() { p1A[0], p1A[j + 1], p1B[j + 1], p1B[0], p1A[0] });
              
                }
                interpolatedQuadsNested.Add(interpolatedQuadsLocal);
                //interpolatedQuadsLocal.Bake();
            }


            foreach (var interpolatedQuads in interpolatedQuadsNested) {
                List<Plane> pyramidsLocal = new List<Plane>();
                for (int i = 0; i < interpolatedQuads.Count; i += 2) {


                    double distanceBetweenCuts = interpolatedQuads[i][1].DistanceTo(interpolatedQuads[i][2]);
                    int divisions = (int)Math.Round(distanceBetweenCuts / stepPyramid, 0);

                    Point3d[] p0 = PointUtil.InterpolatePoints(interpolatedQuads[i][1], interpolatedQuads[i][2], divisions, true);
                    Point3d[] p1 = PointUtil.InterpolatePoints(interpolatedQuads[i + 1][1], interpolatedQuads[i + 1][2], divisions, true);

                    //Have only one cut if you want to cut triangles
                    if (distanceBetweenCuts < 0.01) {
                        p0 = new Point3d[] { p0[0] };
                        p1 = new Point3d[] { p1[1] };
                    }

                    Vector3d dir0 = interpolatedQuads[i][1] - interpolatedQuads[i][0];
                    Vector3d dir1 = interpolatedQuads[i][2] - interpolatedQuads[i][3];



                    var quads0 = new List<Polyline>();
                    for (int j = 0; j < p0.Length; j++) {

                        Polyline pline = new Polyline() { p0[j] - dir0, p0[j], p1[j], p1[j] - dir0, p0[j] - dir0 };

                        //Create rect start
                        Line l1 = pline.SegmentAt(1);
                        Line l2 = pline.SegmentAt(3);
                        l2 = new Line(l2.ClosestPoint(l1.From, false), l2.ClosestPoint(l1.To, false));
                        pline = new Polyline() { l2.From,l1.From,l1.To,l2.To,l2.From };
                        //pline.Bake();
                        //Create rect end

                        quads0.Add(pline);
                        quads0.Add(pline.Translate(10 * Vector3d.CrossProduct(-dir0, p1[j] - p0[j])));

                        //pline.Bake();


                    }

                    //First side
                    List<Plane> toolPathPlanes = CreateToolPath(quads0, r, 100000, retreate, retreateZ)[0];//.Flatten();
                    pyramids.Add(toolPathPlanes);




                    var quads1 = new List<Polyline>();
                    for (int j = 0; j < p0.Length; j++) {
                    
                        Polyline pline = new Polyline() { p0[j] - dir1, p0[j], p1[j], p1[j] - dir1, p0[j] - dir1 };

                        //Create rect start
                        Line l1 = pline.SegmentAt(1);
                        Line l2 = pline.SegmentAt(3);
                        l2 = new Line(l2.ClosestPoint(l1.From, false), l2.ClosestPoint(l1.To, false));
                        pline = new Polyline() { l2.From, l1.From, l1.To, l2.To, l2.From };
                        //pline.Bake();
                        //Create rect end

                        quads1.Add(pline);
                        quads1.Add(pline.Translate(10 * Vector3d.CrossProduct(dir1, p1[j] - p0[j])));
                        //pline.Bake();
                    }

                    //Second side
                    toolPathPlanes = CreateToolPath(quads1, r, 100000, retreate, retreateZ)[1];//.Flatten();
                    pyramids.Add(toolPathPlanes);


                }
          
            }
            return pyramids;
  

        }

            public List<List<Plane>> CreateToolPath(List<Polyline> quads, double r, double step, double retreateAll = 10, double retreateZ = -1, bool retreatFirst = true) {



            var toolPath = new List<List<Plane>>((int)(quads.Count * 0.5));

            //Scale polyline by bladesaw size
            for (int i = 0; i < quads.Count; i += 2) {

          

                double retreate = retreateAll;
                double retreateZCoord = retreateZ;
                //if (retreatFirst && i!= 0 && i!= quads.Count-2)
                    //retreateZCoord = 0;



                    //1.0 Get vector for step divisions
                    Vector3d v1 = quads[i][0] - quads[i][1];
                double len = v1.Length;
                Vector3d v1Unit = new Vector3d(v1);
                v1Unit.Unitize();

                //1.1 Divisions
                double stepConstrained = MathUtil.Constrain(step, 0.005, 100000);
                int d = (int)Math.Ceiling(len / stepConstrained);

                //2.0 Get planes of sawblade outline
                var pScaled = ScalePolyline(quads[i], quads[i+1], r);

                //2.1 Interpolate planes
                pScaled = pScaled.ZigZag(d);

                //2.2 Move start and end segment to back + retreat
                pScaled.Insert(0, (pScaled.First().XForm(Rhino.Geometry.Transform.Translation( v1Unit * retreate))));
                pScaled.Add(pScaled.Last().XForm(Rhino.Geometry.Transform.Translation(v1+ v1Unit * retreate)));

                //2.3 Add first point to start and end tool path from the same point
                if (pScaled.Last().Origin.DistanceToSquared(pScaled[0].Origin) > 0.01)
                    pScaled.Add(pScaled[0]);

                //2.3 Safety Z
                if (retreateZCoord > 0) {
                    //Rhino.RhinoApp.WriteLine("Hi");

                    pScaled.Insert(0, pScaled.First().ChangeOriginCoord(retreateZCoord));
                    pScaled.Add(pScaled.Last().ChangeOriginCoord(retreateZCoord));

                    //if (!retreatFirst) {
                    //    pScaled.Insert(0, pScaled.First().ChangeOriginCoord(retreateZCoord));
                    //    pScaled.Add(pScaled.Last().ChangeOriginCoord(retreateZCoord));
                    //} else {
                    //    if (i == 0)
                    //        pScaled.Insert(0, pScaled.First().ChangeOriginCoord(retreateZCoord));
                    //    if (i == quads.Count - 2)
                    //        pScaled.Add(pScaled.Last().ChangeOriginCoord(retreateZCoord));
                    //}


                }

            

                //2.3 Add to list
                //pScaled.Insert(0, pScaled[pScaled.Count - 1]);
               //pScaled.Insert(pScaled.Count - 2, pScaled[1]);

                Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(0.00, 0.00, 1.00));
                homePlane.Rotate(Math.PI * 0.5, homePlane.ZAxis);
                pScaled.Add(homePlane);
                pScaled.Insert(0,homePlane);
         


                toolPath.Add(pScaled);
             

                //Rhino.RhinoApp.WriteLine(i.ToString());
            }


            //Rhino.RhinoApp.WriteLine("");
            return toolPath;

        }

        //P0*************p3
        //|               *
        //|               *
        //V               *
        //P1------------>P2
        public List<Plane> ScalePolyline(Polyline p, Polyline n, double r) {

            Polyline pScaled = new Polyline(p);

            Vector3d v0 = p[2] - p[1];
            v0.Unitize();

            Vector3d XAxis = p[1] - p[0];
            XAxis.Unitize();

            Vector3d vNormal = n[0] - p[0];
            Vector3d YAxis = Vector3d.CrossProduct(-XAxis, vNormal);

            //Move left or right for cutting part
            double extend = 0.5;
            pScaled[0] -= v0 * r* extend;
            pScaled[1] -= v0 * r * extend;
            pScaled[2] += v0 * r * extend;
            pScaled[3] += v0 * r * extend;

            //Move up
            for (int i = 0; i < pScaled.Count; i++) {
                pScaled[i] -= XAxis * r;
            }

            //Convert polyline to planes
            List<Plane> planes = new List<Plane>(pScaled.Count);
            
            for(int i= 0; i< pScaled.Count-1; i++) {
                planes.Add(new Plane(pScaled[i], XAxis, YAxis));
            }

            return planes;

        }

        
        
    }
}
