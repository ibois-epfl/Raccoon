using NGonsCore;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RhinoJoint2 {
    public class CutDrill : Cut
    {

        public CutDrill(int id, CutType cutType, Plane refPlane, List<Plane> planes) : base( id, cutType, refPlane, planes)
        {
        }

        public CutDrill(Cut cut) : base(cut.id,cut.cutType, cut.refPlane, cut.planes)
        {
            this.plines = cut.plines;
        }
        public CutDrill(int id, IEnumerable<Polyline> geo, CutType cutType, Plane refPlane, List<Plane> planes) : base(id, cutType, refPlane, planes)
        {
            this.plines = geo.ToList();
        }


        public List<List<Plane>> CreateToolPath(double radius, double scale, double turns, double retreate, double retreateZ) {


            var nestedPlanes = new List<List<Plane>>();

            double R = radius;
            double S = scale;
            double T = turns;

            foreach (Polyline pline in this.plines) {

                Line L = new Line(pline[0], pline[1]);
                Vector3d dir = pline[0] - pline[1];
                dir.Unitize();
                //Rhino.RhinoApp.WriteLine(L.Length.ToString());
                L = new Line(pline[0] + dir * 5, pline[1] - dir * 5);
                //Rhino.RhinoApp.WriteLine(L.Length.ToString());
                R = Math.Max(1, R);


                if (L != Line.Unset) {
                    Vector3d v = L.Direction;
                    Plane plane = new Plane(L.From, v);




                    Polyline p0 = GeometryProcessing.Polygon(12, (Math.Max(1, R)), plane, 0, false);//(int)(Math.Max(3, R * 1.00 / Math.Max(0.1, S)))
                    Polyline p1 = new Polyline(p0);


                    p1.Transform(Rhino.Geometry.Transform.Translation(v));

                    //DA.SetData(0, p1);
                    //DA.SetData(1, p0);


                    //SPiral
                    Polyline[] p = (T != 0) ? GeometryProcessing.InterpolatePolylines(p0, p1, (int)T) : GeometryProcessing.InterpolatePolylines(p0, p1, (int)Math.Max(0, (L.Length / R) - 1));


                    //Rhino.RhinoApp.WriteLine(p.Length.ToString());

                    Rhino.Geometry.Interval interval = new Rhino.Geometry.Interval(0, 1);
                    int n = p0.Count;
                    double[] tInterval = new double[n];
                    for (int i = 0; i < n; i++) {
                        tInterval[i] = interval.ParameterAt((double)i / (double)(n - 1));
                    }


                    //Spiral
                    Polyline spiral = new Polyline();


                    for (int i = 0; i < p.Length - 1; i++) {
                        for (int j = 0; j < n; j++) {
                            if (i == 1 && j == 0)
                                continue;
                            spiral.Add(GeometryProcessing.Lerp(p[i][j], p[i + 1][j], tInterval[j]));
                        }
                    }


                    for (int j = 0; j < n; j++) {
                        if (j == 0 || j == n - 1)
                            continue;
                        spiral.Add(p[p.Length - 1][j]);
                    }

                    spiral.Add(p[0][n - 2]);

                    Polyline spiral1 = new Polyline(spiral);
                    v.Unitize();
                    spiral1.Transform(Rhino.Geometry.Transform.Translation(-v * R * 0.1));

                    var planes = new List<Plane>();
                    plane = plane.FlipAndRotate();
                    for (int i = 0; i < spiral.Count; i++) {
                        planes.Add(plane.ChangeOrigin(spiral[i]));
                    }

                    Plane planeFirst = planes.First();
                    planeFirst = planeFirst.ChangeOrigin(planeFirst.Origin + planeFirst.ZAxis * retreate);

                    Plane planeRetreate = planes.First();
                    planeRetreate = planeFirst.ChangeOriginCoord(retreateZ);

                    Plane planeLast = planes.Last();
                    planes.Insert(0, planeFirst);
                    planes.Insert(0, planeRetreate);

                    planes.Add(planeFirst);
                    planes.Add(planeRetreate);

                    Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(0.00, 0.00, 1.00));
                    homePlane.Rotate(Math.PI * 0.5, homePlane.ZAxis);
                    planes.Add(homePlane);
                    planes.Insert(0, homePlane);

                    nestedPlanes.Add(planes);

                    //DA.SetData(2, spiral);
                    //DA.SetData(3, spiral1);


                }
            }

            return nestedPlanes;

        }



        public Tuple<List<Plane>, List<string>, List<string>, List<Arc>> CreateToolPathSpiral(Vector3d xAxis, Plane refPlane, double angle = 0, double speed = 200, double speedMin = 200, int smooth = 10, string WObj = "RotatingTool default", int turns = 4,
  double radius = 15, double retreate = 100, double retreateZ = 1200) {

            //Before this we need some absolute movement + home position
            //Example of arc movement
            //MoveAbs RotatingTool default 300 10 932 1474 1546 Quaternion 0 0 1 0
            //MoveC RotatingTool default 300 10 732 1574 1546 Quaternion 0 0 1 0 932 1674 1546 0 0 1 0


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Base lists
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            var planes = new List<Plane>();
            var commands = new List<string>() { "Mill 20000" };
            var moveTypes = new List<string>() ;
            var arcsAll = new List<Arc>();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Home
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(0.00, 0.00, -1.00));

            double[] posRot = InitPlane(refPlane, homePlane);//Plane to Quaternion
            string outputHomePlane = string.Format("MoveAbs " + WObj + " {0} {1} {2} {3} {4}", (speed), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);
            planes.Add(homePlane);
            commands.Add(outputHomePlane);
            moveTypes.Add("1");


            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Both Directions
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            var drillLines = new List<Polyline>(plines.Count*2);
            
            foreach (Polyline pline in plines) {
                //if (this.DrillTwoSides) {
                //    Line l0 = new Line(pline.First(), (pline.First() + pline.Last()) * 0.5);
                //    Line l1 = new Line(pline.Last(), (pline.First() + pline.Last()) * 0.5);
                //    drillLines.Add(l0.ToP());
                //    drillLines.Add(l1.ToP());
                //} else {
                    drillLines.Add(pline);

                //}
            }
        

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Iterate Lines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            

            foreach (Polyline pline in drillLines) {


                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Convert polylines to lines 
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Line line = new Line(pline[0], pline[1]);
               
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Create spiral arcs
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                List<Arc> arcs = DrillLineToArcs(line, turns, radius);
                arcsAll.AddRange(arcs);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Base Plane
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Vector3d XAxis = xAxis == Vector3d.Unset ? (new Plane(line.From, line.Direction)).XAxis : xAxis;
                Plane plane = xAxis == Vector3d.Unset ? new Plane(Point3d.Origin, XAxis, Vector3d.CrossProduct(XAxis, -line.Direction)) : new Plane(Point3d.Origin,line.Direction) ;
                plane.Rotate(angle, plane.ZAxis);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Retreate
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Plane planeRetreateLinear = new Plane(arcs[0].StartPoint, plane.XAxis, plane.YAxis);
                planeRetreateLinear.Translate(planeRetreateLinear.ZAxis * -retreate);
                posRot = InitPlane(refPlane, planeRetreateLinear);//Plane to Quaternion
                string outputRetreateLinear = string.Format("MoveL " + WObj + " {0} {1} {2} {3} {4}", (speed), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //RetreateZ
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Plane planeRetreateLinearZ = new Plane(new Point3d(planeRetreateLinear.Origin.X, planeRetreateLinear.Origin.Y, retreateZ), plane.XAxis, plane.YAxis);
                posRot = InitPlane(refPlane, planeRetreateLinearZ);//Plane to Quaternion
                string outputRetreateLinearZ = string.Format("MoveAbs " + WObj + " {0} {1} {2} {3} {4}", (speed), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);
                string outputRetreateLinearZ_ = string.Format("MoveL " + WObj + " {0} {1} {2} {3} {4}", (speed), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);

                planes.Add(planeRetreateLinearZ);
                commands.Add(outputRetreateLinearZ);
                moveTypes.Add("0");

                planes.Add(planeRetreateLinear);
                commands.Add(outputRetreateLinear);
                moveTypes.Add("0");




                for (int i = 0; i < arcs.Count; i++) {

                    //Command MoveAbs
                    Plane planeMoveAbs = new Plane(arcs[i].StartPoint, plane.XAxis, plane.YAxis);
                    posRot = InitPlane(refPlane, planeMoveAbs);//Plane to Quaternion

                    if(i == 0) {
                        string outputMoveAbsFast = string.Format("MoveL " + WObj + " {0} {1} {2} {3} {4}", ((int)(25)), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);
                        planes.Add(planeMoveAbs);
                        commands.Add(outputMoveAbsFast);
                        moveTypes.Add("0");

                    }

                    //planeMoveAbs.BakeAxes(10);
                    string outputMoveAbs = string.Format("MoveL " + WObj + " {0} {1} {2} {3} {4}", (speedMin), smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);
                    planes.Add(planeMoveAbs);
                    commands.Add(outputMoveAbs);
                    moveTypes.Add("0");

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Command MoveC
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Plane planeMoveC0 = new Plane(arcs[i].MidPoint, plane.XAxis, plane.YAxis);
                    Plane planeMoveC1 = new Plane(arcs[i].EndPoint, plane.XAxis, plane.YAxis);
                    double[] posRotC0 = InitPlane(refPlane, planeMoveC0);//Plane to Quaternion
                    double[] posRotC1 = InitPlane(refPlane, planeMoveC1);//Plane to Quaternion
                    string outputMoveC = string.Format("MoveC " + WObj + " {0} {1} {2} {3} {4}", (speedMin), smooth, posRotC0[0], posRotC0[1], posRotC0[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRotC0[3], posRotC0[4], posRotC0[5], posRotC0[6]) + string.Format(" {0} {1} {2} {3} {4} {5} {6}", posRotC1[0], posRotC1[1], posRotC1[2], posRotC1[3], posRotC1[4], posRotC1[5], posRotC1[6]);
                    //planes.Add(planeMoveC0);
                    planes.Add(planeMoveC1);
                    //moveTypes.Add("0");
                    moveTypes.Add("2");
                    commands.Add(outputMoveC);
                }

                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Retreate
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                planes.Add(planeRetreateLinear);
                commands.Add(outputRetreateLinear);
                moveTypes.Add("0");

                planes.Add(planeRetreateLinearZ);
                commands.Add(outputRetreateLinearZ_);
                moveTypes.Add("0");


            }

            planes.Add(homePlane);
            commands.Add(outputHomePlane);
            moveTypes.Add("0");
            commands.Add("Mill 0");
         
            return Tuple.Create(planes, commands, moveTypes,arcsAll);
        }

        public double[] InitPlane(Plane refPlane, Plane p) {

            Rhino.Geometry.Quaternion quaternion = new Quaternion();
            quaternion.SetRotation(refPlane, p);

            double[] transformation = new double[]{
      p.OriginX,
      p.OriginY,
      p.OriginZ,
      quaternion.A,quaternion.B,quaternion.C,quaternion.D
      };


            for (int i = 0; i < transformation.Length; i++) {
                if (Math.Abs(transformation[i]) < 0.0001)
                    transformation[i] = 0;
            }
            return transformation;
        }

        public List<Arc> DrillLineToArcs(Line line, int Turns = 4, double radius = 15) {


            //Parameters

            Line x = new Line(line.To, line.From);
            double r = radius;
            double realTurns = Turns;
            double pitch = x.Length / realTurns;//x.Length/(toolR*t);
            double turns = 1 * realTurns;//1*toolR*


            //Create Spiral
            Curve spiral = NurbsCurve.CreateSpiral(x.PointAt(0), x.Direction, x.PointAt(0) + (new Plane(x.PointAt(0), x.Direction)).XAxis * r, pitch, turns, r, r);


            //Reverse spiral for the return
            PolylineCurve pCurve = new PolylineCurve();
            Curve spiralReversed = spiral.DuplicateCurve();
            spiral.Reverse();


            //Create Arc for the Circle
            Point3d pt_l1 = x.PointAt(1);
            Point3d pt_s1 = spiral.PointAtEnd;
            Plane plane = new Plane(pt_l1, x.Direction);
            Point3d pt_s2 = new Point3d(pt_s1);
            pt_s2.Transform(Rhino.Geometry.Transform.Rotation(-Math.PI * 0.5, plane.ZAxis, plane.Origin));
            Point3d pt_s3 = new Point3d(pt_s2);
            pt_s3.Transform(Rhino.Geometry.Transform.Rotation(-Math.PI * 0.5, plane.ZAxis, plane.Origin));
            Arc arc0 = new Arc(pt_s1, pt_s2, pt_s3);
            Arc arc1 = new Arc(pt_s1, pt_s2, pt_s3);
            arc1.Transform(Rhino.Geometry.Transform.Rotation(-Math.PI, plane.ZAxis, plane.Origin));



            //Convert To Arcs
            Point3d[] pts;
            spiral.DivideByCount((int)realTurns * 2 * 2*2, true, out pts);
            List<Arc> arcs = new List<Arc>();
            List<Arc> arcsReversed = new List<Arc>();
            for (int i = 0; i < pts.Length - 1; i += 2) {
                arcs.Add(new Arc(pts[i], pts[i + 1], pts[i + 2]));
                Arc arcReversed = new Arc(pts[i + 2], pts[i + 1], pts[i]);
                arcReversed.Transform(Rhino.Geometry.Transform.Rotation(-Math.PI, plane.ZAxis, plane.Origin));
                arcsReversed.Add(arcReversed);
            }
            arcsReversed.Reverse();
            arcs.Add(arc0);
            arcs.Add(arc1);
            arcs.Add(arc0);
            arcs.AddRange(arcsReversed);





            return arcs;
        }

    }
}
