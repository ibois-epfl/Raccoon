

using Raccoon.Utilities;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;



namespace Raccoon
{
    public class CutMill : Cut {

        public bool MillOrCut = true;

        public CutMill(int id, IEnumerable<Polyline> geo) : base(id) {
            cutType = CutType.Mill;
            this.plines = geo.ToList();

        }

        public CutMill(int id, CutType cutType, Plane refPlane, List<Plane> planes) : base(id, cutType, refPlane, planes) {

        }

        public CutMill(Cut cut) : base() {//cut.id, cut.cutType, cut.refPlane, cut.planes

            //this.plines = cut.plines;
            //this.cutType = cut.cutType;

            this.id = cut.id;
            this.cutType = CutType.Mill;
            this.refPlane = cut.refPlane;
            this.planes = cut.planes;


            foreach (var g in cut.plines)
                this.plines.Add(g.Duplicate());


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

        public CutMill(int id, IEnumerable<Polyline> geo, CutType cutType, Plane refPlane, List<Plane> planes) : base(id, cutType, refPlane, planes) {

            this.plines = geo.ToList();
        }

        public List<List<Plane>> CreateToolPathCircular(double Radius, double heightDivisions, double retreate, double retreateZ, bool planarOffset, bool sort, bool soft, bool notch, bool milling, bool perpendicularToSurface) {


            Rhino.RhinoApp.WriteLine("ToolPath Circular");
            List<List<Plane>> ArcsAll = new List<List<Plane>>();

            if (MillOrCut) {

            } else {

            }


            return ArcsAll;
        }

        public List<List<Plane>> CreateToolPath(double Radius, double heightDivisions, double retreate, double retreateZ, bool planarOffset, bool sort, bool soft, bool notch, CutType milling, bool perpendicularToSurface, List<byte[]> notchesTypes) {

          
            List<List<Plane>> planesAll = new List<List<Plane>>();

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //All Milling or Cutting outlines must contain top and bottom curves indicating cutting outline and direction
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            for (int m = 0; m < this.plines.Count; m += 2) {

    

                if (milling==CutType.Mill || milling == CutType.Cut ) {
                    #region Milling



                    List<Polyline> C0 = new List<Polyline>((int)(this.plines.Count * 0.5));
                    List<Polyline> C1 = new List<Polyline>((int)(this.plines.Count * 0.5));

                    Polyline pline0 = this.plines[m + 0];
                    Polyline pline1 = this.plines[m + 1];


                    if (perpendicularToSurface) {
                        Plane plane0 = this.plines[m + 0].plane();
                        Plane plane1 = this.plines[m + 1].plane();

                        Point3d p1 = plane1.ClosestPoint(plane0.Origin);
                        Point3d p0 = plane0.Origin;
                        Vector3d v = p1 - p0;

                        pline1 = new Polyline(this.plines[m + 0]);
                        pline1.Transform(Rhino.Geometry.Transform.Translation(v));
                    }

                    C0.Add(pline0);
                    C1.Add(pline1);

                    List<Polyline> P0 = new List<Polyline>();
                    List<Polyline> P1 = new List<Polyline>();

                    for (int i = 0; i < C0.Count; i++) {
                        P0.Add(PolylineUtil.ToPolylineFromCP(C0[i].ToNurbsCurve()));
                        P1.Add(PolylineUtil.ToPolylineFromCP(C1[i].ToNurbsCurve()));
                    }

                    //1.0 Offset Curves
                    Polyline[] P0Sorted = P0.ToArray();
                    Polyline[] P1Sorted = P1.ToArray();

                    if (sort) {
                        P0Sorted = GeometryProcessing.SortPolylines(P0);//last biggest
                        P1Sorted = GeometryProcessing.SortPolylines(P1);//last biggest

                    }


                    Polyline[] P0Sorted_ = new Polyline[P0Sorted.Length];
                    Polyline[] P1Sorted_ = new Polyline[P1Sorted.Length];

                    for (int i = 0; i < C0.Count; i++) {
                        P0Sorted_[i] = new Polyline(P0Sorted[i]);
                        P1Sorted_[i] = new Polyline(P1Sorted[i]);
                    }

                    //3.0 Notches
                    List<Line> notchLines = new List<Line>();
                    if (notch && notchesTypes[m][0] !=0) {
                        for (int i = 0; i < P0Sorted.Length; i++) {
                            notchLines.AddRange(Ears.DrillingHoleForConvexCorners(P0Sorted[i], P1Sorted[i], Radius, notchesTypes[i]));
                        }
                    }


                    //base.Message = sort.ToString();




                    Polyline[] offset = Offset.OffsetPolyline(P0Sorted[0], P1Sorted[0], -Radius * Convert.ToInt32(planarOffset));
                    P0Sorted[0] = offset[0];
                    P1Sorted[0] = offset[1];


                    for (int i = 1; i < P0Sorted.Length; i++) {
                        offset = Offset.OffsetPolyline(P0Sorted[i], P1Sorted[i], Radius);
                        P0Sorted[i] = offset[0];
                        P1Sorted[i] = offset[1];
                    }

                    //offset.Bake();

                    //2.0 Offset multiple times
                    List<Polyline> OutputPlines0 = new List<Polyline>();
                    List<Polyline> OutputPlines1 = new List<Polyline>();

                    OutputPlines0.AddRange(Offset.OffsetClipper(P0Sorted.ToList(), Radius * Convert.ToInt32(!planarOffset)));
                    OutputPlines1.AddRange(Offset.OffsetClipper(P1Sorted.ToList(), Radius * Convert.ToInt32(!planarOffset)));




                    //P0Sorted[0].Bake();
                    Polyline multipleOffset0;
                    //bool cutOutlineInstedOfMill = P0Sorted[0].SegmentAt(0).Length < Radius * 4 && P0Sorted[0].Count == 5;
            
                        if ((P0Sorted.Length == 1 && P0Sorted[0].Count == 5))
                            multipleOffset0 = milling == CutType.Mill ? Offset.OffsetRectangle(P0Sorted[0], Radius) : P0Sorted[0];//Radius* Convert.ToInt32(!planarOffset)
                        else
                            multipleOffset0 = milling == CutType.Mill ? Offset.OffsetMultiple(P0Sorted.ToList(), 200, 0.001, Radius * Convert.ToInt32(!planarOffset), Radius, soft, milling == CutType.Mill) : P0Sorted[0];
                  
                    //multipleOffset0.Bake();

                    List<Curve> innerCurves = new List<Curve>();
                    for (int i = 1; i < P0Sorted.Length; i++)
                        innerCurves.Add(P0Sorted[i].ToNurbsCurve());
                    Mesh m0 = Rhino.Geometry.Mesh.CreatePatch(P0Sorted[0], 0.01, null, innerCurves, null, null, true, 1);

                    //Mesh m0 = Triangulate.MeshFromClosedPolylineWithHoles(P0Sorted);
                    Mesh m1 = new Mesh();


                    PointCloud pc0 = new PointCloud();
                    PointCloud pc1 = new PointCloud();
                    for (int i = 0; i < P0Sorted_.Length; i++) {
                        pc0.AddRange(P0Sorted[i]);
                        pc1.AddRange(P1Sorted[i]);
                    }

                    for (int i = 0; i < m0.Vertices.Count; i++) {
                        m1.Vertices.Add(pc1[pc0.ClosestPoint(m0.Vertices[i])].Location);
                    }

                    m1.Faces.AddFaces(m0.Faces);


                    //multipleOffset0.Bake();

                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Map polylines
                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Map.Clean(m0);
                    Map.Clean(m1);
                    List<Polyline> polylinesToMap = new List<Polyline> { multipleOffset0 };
                    List<Polyline> multipleOffset1 = Map.MappedFromMeshToMesh(polylinesToMap, m0, m1);

                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Interpolate polylines
                    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    int hd = (int)(multipleOffset0[0].DistanceTo(multipleOffset1[0][0]) / heightDivisions);
                    hd = Math.Max(1, hd);
                    var interpolated = Interpolate.InterpolateTwoPolylinesToOnePath(multipleOffset0, multipleOffset1[0], (int)hd, Radius, soft, notchLines);//notchLines

                    //notchLines.Bake();


                    //Output

                    Polyline OutputPolylineSeam1 = Offset.ChangeClosedPolylineSeam(interpolated[0], interpolated[0].ClosestParameter(C1[0].ToNurbsCurve().PointAtStart) + 0.5);
                    Polyline OutputPolylineSeam0 = Offset.ChangeClosedPolylineSeam(interpolated[1], interpolated[1].ClosestParameter(C0[0].ToNurbsCurve().PointAtStart) + 0.5);
                    OutputPolylineSeam1.Reverse();
                    OutputPolylineSeam0.Reverse();
                    List<Plane> planes = new List<Plane>();

                    if (OutputPolylineSeam0.Count > 0) {

                        Plane planeLast = new Plane(OutputPolylineSeam0[0], OutputPolylineSeam1[0] - OutputPolylineSeam0[0]);
                        planes.Add(planeLast);

                        for (int i = 1; i < OutputPolylineSeam0.Count; i++) {
                            Plane plane = Vector3d.VectorAngle(planeLast.ZAxis, OutputPolylineSeam1[i] - OutputPolylineSeam0[i]) < 0.01
                                ? planeLast.ChangeOrigin(OutputPolylineSeam0[i])
                                : new Plane(OutputPolylineSeam0[i], OutputPolylineSeam1[i] - OutputPolylineSeam0[i])
                                ;


                            planes.Add(plane);
                        }

                        Plane planeFirst = new Plane(planes.First());

                        //planes.Insert(0, planes[0].ChangeOrigin(planes.First().Origin + planes.First().ZAxis * retreate));
                        //planes.Insert(0, planes[0].ChangeOriginCoord(retreateZ));

                        //Plane retreatePlane = planeFirst.ChangeOrigin(planeFirst.Origin + planeFirst.ZAxis * retreate);
                        //planes.Add(retreatePlane);

                        //Plane lastRetreat = planes.Last().ChangeOriginCoord(retreateZ);
                        //Plane lastVertical = new Plane(lastRetreat.Origin + Vector3d.ZAxis * 100, Vector3d.YAxis, -Vector3d.XAxis);
                        //Plane planeInterpolated = lastRetreat;
                        //planes.Add(planeInterpolated);
                        planesAll.Add(planes);

                    }




                    //Open path
                    #endregion
                } else if(milling == CutType.Cut) {
                    #region Cutting Outline

                    Vector3d ZAxis = this.plines[m + 1][0] - this.plines[m + 0][0];
                    Polyline[] offset = Offset.OffsetPolyline(this.plines[m + 0], this.plines[m + 1], -Radius * Convert.ToInt32(planarOffset));

                    int hd = (int)(offset[0][0].DistanceTo(offset[1][0]) / heightDivisions);
                    hd = Math.Max(1, hd);
                    Polyline[] interpolatedPlines = PointUtil.InterpolatePolylines(offset[0], offset[1], (int)hd, true);

                    for (int i = 0; i < interpolatedPlines.Length - 1; i++) {

                        Polyline pline0 = interpolatedPlines[i];
                        Polyline pline1 = interpolatedPlines[i + 1];

                        List<Plane> normalPlanes = new List<Plane>();
                        Plane normalPlane = new Plane(pline0[0], ZAxis);

                        //Retreate Planes Start
                        Plane retreateStart = new Plane(normalPlane);
                        Vector3d retreateVector = Vector3d.CrossProduct(normalPlane.XAxis, ZAxis);
                        retreateVector.Unitize();
                        retreateStart.Translate(retreateVector * retreate);
                        normalPlanes.Add(retreateStart);

                        //Point-planes
                        for (int n = 0; n < pline0.Count; n++) {
                            Plane pointPlane = new Plane(normalPlane);
                            pointPlane.Origin = pline0[n];
                            normalPlanes.Add(pointPlane);
                        }

                        //Retreate Planes End
                        Plane retreateEnd = new Plane(normalPlanes.Last());
                        retreateVector = Vector3d.CrossProduct(normalPlane.XAxis, ZAxis);
                        retreateVector.Unitize();
                        retreateEnd.Translate(retreateVector * retreate);
                        normalPlanes.Add(retreateEnd);

                        planesAll.Add(normalPlanes);
                    }

                    #endregion
                } else if(false) {//milling == CutType.Slice
                    #region Cutting One Edge



                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Offset polylines by thickness of a tool
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Polyline[] offset = Offset.OffsetPolyline(this.plines[m + 0], this.plines[m + 1], -Radius * Convert.ToInt32(planarOffset));
                    offset = new Polyline[] { offset[1].SegmentAt(0).ExtendLine(Radius*2, Radius * 2).ToP(), offset[0].SegmentAt(0).ExtendLine(Radius * 2, Radius * 2).ToP() };
                    if(offset[0][0].Z< offset[0][1].Z) {
                        offset[0] = offset[0].Flip();
                        offset[1] = offset[1].Flip();
                    }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Create plane
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Vector3d XAxis = offset[0][1] - offset[0][0];
                    Vector3d ZAxis = this.plines[m + 1][0] - this.plines[m + 0][0];
                    Vector3d YAxis = Vector3d.CrossProduct(ZAxis, XAxis);
                    Plane normalPlane = new Plane(Point3d.Origin, XAxis,YAxis);

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Interpolate lines vertically
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    int hd = (int)(offset[0][0].DistanceTo(offset[1][0]) / heightDivisions);
                    hd = Math.Max(1, hd);
                    Polyline[] interpolatedPlines = PointUtil.InterpolatePolylines(offset[0], offset[1], (int)(hd/2), true);

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Retreate Plane
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    List<Plane> normalPlanes = new List<Plane>() ;
                    Plane planeRetreate = (new Plane(normalPlane.ChangeOrigin(interpolatedPlines[0][0]))).MovePlanebyAxis(retreate);
                    normalPlanes.Add(planeRetreate.ChangeOriginCoord(retreateZ));
                    normalPlanes.Add(planeRetreate);


                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Loop interpolated lines and create planes on these lines
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    for (int i = 0; i < interpolatedPlines.Length; i++) {
                        for (int n = 0; n < interpolatedPlines[i].Count; n++) {
                            normalPlanes.Add(normalPlane.ChangeOrigin(interpolatedPlines[i][n]));
                        }
                    }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Retreate Plane
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    normalPlanes.Add(normalPlanes[normalPlanes.Count - 2]);
                    normalPlanes.Add(normalPlanes[1]);
                    normalPlanes.Add(planeRetreate);
                    normalPlanes.Add(planeRetreate.ChangeOriginCoord(retreateZ));

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Add to the global list
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    planesAll.Add(normalPlanes);

                    #endregion
                } else if(milling == CutType.OpenCut || milling == CutType.Slice) {

                    #region Cutting One Open Path
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Create plane
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    Vector3d XAxis = this.plines[m + 0][1] - this.plines[m + 0][0];
                    Vector3d ZAxis = this.plines[m + 1][0] - this.plines[m + 0][0];

                    Vector3d YAxis = Vector3d.CrossProduct(ZAxis, XAxis);
                    Plane normalPlane = new Plane(Point3d.Origin, ZAxis);
                    normalPlane = normalPlane.AlignPlane(XAxis);


                    //double retreateZBasedOnZAxis = ZAxis.Unit().Z < 0 ? retreateZ - Raccoon.GCode.Axes.ZCoord * ZAxis.Unit().Z : retreateZ;
                    //double retreateZUpwards = ZAxis.Unit().Z < 0 ? retreate : retreate;
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Offset polylines by thickness of a tool
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

                    Polyline[] offset = Offset.OffsetParallelograms(this.plines[m + 0], this.plines[m + 1], -Radius * Convert.ToInt32(planarOffset));//

                    if (milling == CutType.Slice) {
                        offset = new Polyline[] { 
                            offset[0].SegmentAt(offset[0].SegmentCount-1).ExtendLine(Radius * 2, Radius * 2).ToP(), 
                            offset[1].SegmentAt(offset[1].SegmentCount - 1).ExtendLine(Radius * 2, Radius * 2).ToP() };
                    }

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Add Notches
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    List<Line> notchLines = new List<Line>();
               
                    if (notch) {
                        offset =(Ears.Notches(offset[0], offset[1], Radius, notchesTypes[0]));
                      }


                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Interpolate lines vertically
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    if (Point3d.Origin.DistanceToSquared(offset.Last().Last()) < Point3d.Origin.DistanceToSquared(offset.First().First()))
                        for (int i = 0; i < offset.Length; i++) {
                            offset[i] = offset[i].Flip();
                    }

                    //Rhino.RhinoApp.WriteLine((offset[0][0].DistanceTo(offset[1][0]) / heightDivisions).ToString());
                    int hd = (int)(offset[0][0].DistanceTo(offset[1][0]) / heightDivisions);
                    // Polyline spiral = PolylineUtil.CreateSpiral(offset[0], offset[1], Math.Max(1, (int)(hd * 0.5)));
                    Polyline spiral_normal =new Polyline();
                    Polyline spiral = PolylineUtil.CreateSpiral(offset[0], offset[1], Math.Max(1, (int)(hd )),ref spiral_normal);
                    List<Plane> normalPlanes = new List<Plane>();

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ////Retreate Plane
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


                    //Plane planeRetreate = (new Plane(normalPlane.ChangeOrigin(spiral[0]))).MovePlanebyAxis(retreateZUpwards);
                    //Vector3d v = (new Point3d(planeRetreate.Origin.X, planeRetreate.Origin.Y, 0) - new Point3d(spiral[0].X, spiral[0].Y, 0)).UnitVector() * 300;

                    //double scale = planeRetreate.ZAxis.UnitVector().Z > 0 ? 0 : MathUtil.Constrain(PlaneUtil.VectorExtensionByRadiusAndNormal(planeRetreate.ZAxis, Plane.WorldXY, 200), 20, 100);

                    //bool flagDir = (planeRetreate.Origin + v).DistanceToSquared(Point3d.Origin) < (planeRetreate.Origin - v).DistanceToSquared(Point3d.Origin);
                    //v = flagDir ? -Vector3d.XAxis * scale : Vector3d.XAxis * scale;

                    //Plane planeretreateZBasedOnZAxis = planeRetreate.ChangeOriginCoord(retreateZBasedOnZAxis);
                    //normalPlanes.Add(planeretreateZBasedOnZAxis.Translation(v));
                    //Point3d newOrigin = (new Line(planeretreateZBasedOnZAxis.Origin, planeRetreate.Origin)).PointAt(0.25);
                    //normalPlanes.Add(planeretreateZBasedOnZAxis.ChangeOrigin(newOrigin).Translation(v));
                    //if (scale > 0)
                    //    normalPlanes.Add(planeRetreate.Translation(v));
                    //normalPlanes.Add(planeRetreate);


                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Loop interpolated lines and create planes on these lines
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    for (int n = 0; n < spiral.Count; n++)
                    {
                        Plane plane = new Plane(spiral[n], spiral_normal[n]- spiral[n]);
                        plane= PlaneUtil.AlignPlane(plane, normalPlane.XAxis);
                        normalPlanes.Add(plane);//normalPlane.ChangeOrigin(spiral[n])
                    }


                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ////Retreate Plane
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //normalPlanes.Add(planeRetreate);
                    //if (scale > 0)
                    //    normalPlanes.Add(planeRetreate.Translation(v));
                    //normalPlanes.Add(planeRetreate.ChangeOriginCoord(retreateZBasedOnZAxis).Translation(v));

                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    //Add to the global list
                    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    planesAll.Add(normalPlanes);

                    #endregion
                }


            }

            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            //Add vertical home plane, so that robot will come back to reachable position after each tool-path
            /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            #region Home
            //Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(1.00, 0.00, 0.00), new Vector3d(0.00, 1.00, 0.00));
            Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 2200.00), new Vector3d(1.00, 0.00, 0.00), new Vector3d(0.00, 1.00, 0.00));
            homePlane.Rotate(Math.PI * 0.5, homePlane.ZAxis);
            //planesAll[0].Insert(0, homePlane);
            //planesAll[planesAll.Count - 1].Add(homePlane);

            for (int i = 0; i < planesAll.Count; i++) {
                for (int j = 0; j < planesAll[i].Count; j++) {
                    planesAll[i][j] = planesAll[i][j].Switch("YX");
                }
            }

            #endregion

            return planesAll;
        }



    }
}
