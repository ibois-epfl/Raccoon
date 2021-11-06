using NGonsCore;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using RhinoJoint2;

namespace RhinoJoint2 {
    //Sawblade cut must contain 4 point polylines
    //It consists of one segment as a path and other segment as normal
    public class CutSawBlade : Cut
    {

     

        public CutSawBlade(int id, CutType cutType, Plane refPlane, List<Plane> planes, List<double> speeds, int smooth) : base(id, cutType, refPlane, planes) {

        }

        public CutSawBlade( Cut cut) : base() {


            //this.plines = cut.plines;
            //this.cutType = cut.cutType;

            this.id = cut.id;
            this.cutType = CutType.SawBlade;
            this.refPlane = cut.refPlane;
            this.planes = cut.planes;




            for(int i = 0; i < cut.plines.Count; i++) {
                var plineCopy = cut.plines[i].Duplicate();
                this.plines.Add(plineCopy);
            }

           

            foreach (var g in cut.breps)
                this.breps.Add(g.DuplicateBrep());

            foreach (var g in cut.meshes)
                this.meshes.Add(g.DuplicateMesh());



            this.toolR = cut.toolR;
            this.insertionVector = new Vector3d(cut.insertionVector);
            this.CutOrHole = cut.CutOrHole;
            this.PolylineMergeTakeOutside = cut.PolylineMergeTakeOutside;
            this.merge = cut.merge;
            this.project = cut.project;
            this.notches = cut.notches;
            this.notchesTypes = new List<byte[]>(cut.notchesTypes.Count);

            for (int i = 0; i < cut.notchesTypes.Count; i++) {
                var bytes = new byte[cut.notchesTypes[i].Length];
                for (int j = 0; j < cut.notchesTypes[i].Length; j++) {
                    bytes[j] = cut.notchesTypes[i][j];
                }
                this.notchesTypes.Add(bytes);
            }

            this.filletR = cut.filletR;
            this.CutOrHole = cut.CutOrHole;
            this.PolylineMergeTakeOutside = cut.PolylineMergeTakeOutside;
            this.merge = cut.merge;
            this.SawFlip90Cut = cut.SawFlip90Cut;

            this.plinesBinary = plinesBinary.Duplicate();
            this.plineReference = plineReference.Duplicate();
            this.weight = cut.weight;
            this.projectRotate = cut.projectRotate;
        }

     
        public CutSawBlade(int id, IEnumerable<Polyline> geo, CutType cutType, Plane refPlane, List<Plane> planes) : base(id, cutType, refPlane, planes) {

            this.plines = geo.ToList();
        }

        public List<List<Plane>> CreateToolPath(double r, double step, double retreate = 10, double retreateZ = -1, int ThicknessDivisions = 0, int cut90Degrees = 0, double extendSides = 1, bool flip90 = false, double rotation90Deg = 0) {

            double AngleFor90DegreeCut = -Rhino.RhinoMath.ToRadians(rotation90Deg);// -Math.PI * 0.10;

            var toolPath = new List<List<Plane>>((int)(this.plines.Count * 0.5));

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Interpolate polylines a series of cutting outlines
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            for (int i = 0; i < this.plines.Count; i += 2) {



                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                //Interpolate polylines a series of rectangles
                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                Polyline a = new Polyline(this.plines[i + 1]);
                Polyline b = new Polyline(this.plines[i]);

                //Rhino.RhinoApp.WriteLine("Rotable " + this.rotatable.ToString());
                //This breaks the orientation -> fix tile orientation instead
                
                if (this.rotatable && a.PointAt(0.25).Z < a.PointAt(2.25).Z) {

           

                    a = a.ShiftPline(2);
                    b = b.ShiftPline(2);
                }

                //double len0 = a[a.Count - 2].DistanceTo(a[0]);
                //double len1 = a[1].DistanceTo(a[0]);
                //a[0].Bake();




                        Polyline[] interpolatedPlines = PointUtil.InterpolatePolylines(a, b, ThicknessDivisions);

                
                interpolatedPlines = interpolatedPlines.Reverse().ToArray();


                for (int m = interpolatedPlines.Length-1; m >0; m -= 2) {

                    Polyline pline0 = interpolatedPlines[m-1];//cut
                    Polyline pline1 = interpolatedPlines[m ];//normal

                    //1.0 Get vector for step divisions
                    Vector3d v1 = pline0[0] - pline0[1];
                    double len = v1.Length;
                    Vector3d v1Unit = new Vector3d(v1);
                    v1Unit.Unitize();

                    //1.1 Divisions
                    double stepConstrained = MathUtil.Constrain(step, 0.005, 100000);
                    int d = (int)Math.Ceiling(len / stepConstrained);
                    d += d % 2;
                    Rhino.RhinoApp.WriteLine(d.ToString());

                    //2.0 Get planes of sawblade outline
                    var pScaled = ScalePolyline(pline0, pline1, r,true,extendSides);
                    var pScaledMoved = ScalePolyline(pline0, pline1, r,false, extendSides);


                    Vector3d v = pline1[0] - pline0[0];
                    v.Rotate(AngleFor90DegreeCut, pScaledMoved[0].YAxis);
                    var pScaledMovedN = pScaledMoved.Transform(Rhino.Geometry.Transform.Translation(v));
                   

                    /////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///cut90Degrees
                    /////////////////////////////////////////////////////////////////////////////////////////////////////
                    Rhino.RhinoApp.WriteLine("SawBladeGH " + cut90Degrees.ToString());
                    if (cut90Degrees==1 || cut90Degrees==2) {



                        //2.1. Interpolate Planes
                        Plane[] planesInterpolated0 = PointUtil.InterpolatePlanes(pScaledMoved[0], pScaledMoved[1], d - 2);
                        Plane[] planesInterpolated1 = PointUtil.InterpolatePlanes(pScaledMoved[3], pScaledMoved[2], d - 2);
                        Plane[] planesInterpolatedN0 = PointUtil.InterpolatePlanes(pScaledMovedN[0], pScaledMovedN[1], d - 2);
                        Plane[] planesInterpolatedN1 = PointUtil.InterpolatePlanes(pScaledMovedN[3], pScaledMovedN[2], d - 2);

                        List<Plane> planes = new List<Plane>();
                        for (int j = 0; j < planesInterpolated0.Length; j++) {

                            Vector3d XAxis = pline1[0] - pline0[0];
                            Vector3d YAxis = -(pline0[1] - pline0[2]);

                            //Rotate if axis is down
                            Vector3d ZAxis = Vector3d.CrossProduct(XAxis,YAxis);
                            if (ZAxis.Z>0) {
                                YAxis *= -1;

                            }


                            XAxis.Rotate(AngleFor90DegreeCut, YAxis);

                            Plane plane = new Plane(planesInterpolated0[j].Origin, XAxis, YAxis);
       
                            if (flip90)
                                plane = plane.FlipAndRotate();

                            Vector3d retreateV = Vector3d.CrossProduct(YAxis, Vector3d.CrossProduct(XAxis, YAxis)).UnitVector();
                            retreateV = plane.XAxis;

                            double scale = Math.Abs(1 / Math.Sin(Vector3d.VectorAngle(XAxis.UnitVector(), YAxis.UnitVector(), plane)));

            

                            Plane[] planes90 = new Plane[] {

                            plane.ChangeOrigin(planesInterpolatedN0[j].Origin + retreateV * (r * scale + retreate)),
                            plane.ChangeOrigin(planesInterpolated0[j].Origin + retreateV * (r * scale)),
                            plane.ChangeOrigin(planesInterpolated1[j].Origin + retreateV * (r * scale)),
                            plane.ChangeOrigin(planesInterpolatedN1[j].Origin + retreateV * (r * scale + retreate)),
                            plane.ChangeOrigin(planesInterpolatedN0[j].Origin + retreateV * (r * scale + retreate))

                        };



                            if (flip90) {
                                for (int k = 0; k < planes90.Length; k++) {
                                    planes90[k].Rotate(-Math.PI * 0.5, planes90[k].ZAxis);
                                    planes90[k].Translate(plane.ZAxis * 3.3);
                                }
                            }

                            planes.AddRange(planes90);


                        }
                      
                        planes.Add(planes.First());
                        planes.Insert(0, planes[1].ChangeOriginCoord(retreateZ));

                        //retreate plane
                        Plane retreatePlane = planes[1].Translation(v1.UnitVector() * retreate);
                        planes.Insert(1, retreatePlane);
                        planes.Add(retreatePlane);


                        planes.Add(planes.Last().ChangeOriginCoord(retreateZ));

                        planes[0]=planes.Last().ChangeOriginCoord(retreateZ);
                        toolPath.Add(planes);

                       
                    }



                    //Actual cuts
                    //2.1 Interpolate planes
                    pScaled = pScaled.ZigZag(d);


                    //2.2 Move start and end segment to back + retreat
                    pScaled.Insert(0, (pScaled.First().XForm(Rhino.Geometry.Transform.Translation(v1Unit * retreate))));
                    pScaled.Add(pScaled.Last().XForm(Rhino.Geometry.Transform.Translation(v1 + v1Unit * retreate)));

                    //2.3 Add first point to start and end tool path from the same point
                    if (pScaled.Last().Origin.DistanceToSquared(pScaled[0].Origin) > 0.01)
                        pScaled.Add(pScaled[0]);


                    //2.3 Safety Z
                    if (retreateZ > 0) {
                        pScaled.Insert(0, pScaled.First().ChangeOriginCoord(retreateZ));
                        pScaled.Add(pScaled.Last().ChangeOriginCoord(retreateZ));
                    }

                    //2.3 Add to list
                    //if (cut90Degrees == 0 || cut90Degrees == 2)
                        toolPath.Add(pScaled);
                }

            }

             //toolPath = new List<List<Plane>> { toolPath.Flatten()   };
             //Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(0.00, 0.00, -1.00));
             //homePlane.Rotate(Math.PI * 0.0, homePlane.ZAxis);
             //toolPath[0].Add(homePlane);
             //toolPath[0].Insert(0, homePlane);



            //Rhino.RhinoApp.WriteLine("");
            return toolPath;

        }

        //P0*************p3
        //|               *
        //|               *
        //V               *
        //P1------------>P2
        public List<Plane> ScalePolyline(Polyline p, Polyline n, double r, bool move=true, double extendSides = 1) {

            Polyline pScaled = new Polyline(p);

            Vector3d v0 = p[2] - p[1];
            v0.Unitize();

            Vector3d XAxis = p[1] - p[0];
            XAxis.Unitize();

            Vector3d vNormal = n[0] - p[0];

            Vector3d YAxis = p[1] - p[2];
            YAxis.Unitize();


            Vector3d ZAxis = Vector3d.CrossProduct(XAxis, YAxis);

           if ((p[0] + ZAxis).DistanceTo(n[0]) > (p[0] - ZAxis).DistanceTo(n[0]))
                YAxis *= -1;

                //Move left or right for cutting part
                pScaled[0] -= v0 * r* extendSides;
            pScaled[1] -= v0 * r * extendSides;
            pScaled[2] += v0 * r * extendSides;
            pScaled[3] += v0 * r * extendSides;

            //Move up
            for (int i = 0; i < pScaled.Count; i++) {
                if (move)
                    pScaled[i] -= XAxis * r;
            }


            //Convert polyline to planes
            List<Plane> planes = new List<Plane>(pScaled.Count);

            for (int i= 0; i< pScaled.Count-1; i++) {
                planes.Add(new Plane(pScaled[i], -XAxis, YAxis));
            }


            return planes;

        }

        
        
    }
}
