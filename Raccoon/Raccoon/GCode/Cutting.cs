using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.GCode
{
    public static class Cutting
    {
        public static List<string> PolylineCutSimple(GCode.ToolParameters tool, List<Polyline> polylines, ref PreviewObject previewGeometry, List<Polyline> normals = null,
 string filename = "P1234567", double Zsec = 650, double Speed = 20000, double RetreatDistance = 70, double angles = 80, int zero = 54)
        {
            List<string> ncc = new List<string> {
                "G"+zero.ToString() + " (G54/G55/G56_zero_in_maka)",
                //"P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)", //lift aspiration
                "T" + tool.id.ToString() + " M6" + " (txx_-_tool_name,_m6_-_change_tool)", //get Toool
                "S" + Utilities.GeometryProcessing.Lerp(tool.prescribedSpindleSpeed, tool.maxSpindleSpeed, 0.7).ToString() + " M3" + " (sxx_-_speed,_m3_-_clockwise)", //slow rmp "S2500 M3 M1"
                //"G47" + Axes.DefaultRotation +" F5000" + " (g47_-_3_axis_in_plane)",//neutral position "G1 G47 A0 B0 F5000 M1"
                //"G0 G49 G" + zero.ToString() + " X0 Y0 Z" + Zsec.ToString() + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "G0 G49 Z750" + " (go_to_safe_z_position_to_avoid_collision)" ,
                "G0 " + Axes.HomePosition + Axes.DefaultRotation + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "G1 F"+Speed.ToString() ,
                "M08" + " (air_supply)" ,

                "(ReplaceB)",
                "(____________start_cutting____________)",
                //"G0 X0 Y0 Z" + Zsec.ToString() + Axes.DefaultRotation+ " (startpos)"
            };

            for (int i = 0; i < polylines.Count; i++)
            {
                //Polyline polyline = new Polyline(polylines[i].Count);
                //Polyline normal = new Polyline(polylines[i].Count);
                Polyline polyline = polylines[i];
                Polyline normal = normals[i];

                //for (int j = 0; j < polylines[i].Count - 1; j++)
                //{
                //    Vector3d v0 = polylines[i][j] - normals[i][j];
                //    Vector3d v1 = polylines[i][j + 1] - normals[i][j + 1];
                //    if (v0.IsParallelTo(v1, 0.01) == 1)
                //    {
                //        polyline.Add(polylines[i][j]);
                //        normal.Add(normals[i][j]);
                //    }
                //    else
                //    {
                //        polyline.Add(polylines[i][j]);
                //        normal.Add(normals[i][j]);
                //        double dist = Math.Max(polylines[i][j].DistanceTo(polylines[i][j + 1]), normals[i][j].DistanceTo(normals[i][j + 1]));
                //        int divisions = (int)Math.Ceiling(dist / 10000000.0);
                //        Point3d[] interpolated_segment_polyline = Raccoon.PointUtil.InterpolatePoints(polylines[i][j], polylines[i][j + 1], divisions, false);
                //        Point3d[] interpolated_segment_normal = Raccoon.PointUtil.InterpolatePoints(normals[i][j], normals[i][j + 1], divisions, false);

                //        polyline.AddRange(interpolated_segment_polyline);
                //        normal.AddRange(interpolated_segment_normal);
                //    }
                //}

                ncc.Add("(____________polyline " + i.ToString() + "____________)");

                if (polylines[i].Count != normals[i].Count)//number of pt equal
                    continue;

                Vector3d n = Vector3d.Unset;
                Vector3d n_last = Vector3d.Unset;
                Point3d p = Point3d.Unset;
                Point3d p_last = Point3d.Unset;
                Point3d safety = Point3d.Unset;
                Tuple<double, double, string> AB = null;
                Tuple<double, double, string> AB_last = null;
                string speed = " F" + (Speed).ToString();

                for (int j = 0; j < polyline.Count; j++)
                {
                    //get normal
                    p = polyline[j];
                    n = normal[j] - polyline[j];
                    n.Unitize();
                    if (n.IsParallelTo(n_last) != 1)
                        AB = GCode.CoordinateSystem.AB180(n);

                    if (j == 0)
                    {
                        safety = p + (n * RetreatDistance);
                        double z_first = i == 0 ? Axes.ZCoord : Zsec;
                        ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(new Point3d(safety.X, safety.Y, z_first)) + " (first_point)");
                        ncc.Add("G1" + " F" + Speed.ToString() + "." + " (first_point)");
                        ncc.Add(GCode.CoordinateSystem.Pt2nc(new Point3d(safety.X, safety.Y, Zsec), 3, "") + AB.Item3 + " (first_point rotate)");//G1
                        ncc.Add(GCode.CoordinateSystem.Pt2nc(safety, 3, "") + AB.Item3 + " (safety_first_retreated)");//G1
                        ncc.Add(GCode.CoordinateSystem.Pt2nc(p, 3, "") + AB.Item3 + " (first_point_into_rotation)");//G1
                    }
                    else
                    {
                        if (AB.Item1 > AB_last.Item1 + angles || AB.Item1 < AB_last.Item1 - angles)
                        {
                            ncc.Add("(**********************_turn_*************************)");

                            //Move until middle to the next point
                            Point3d mid = (p + p_last) * 0.5;
                            ncc.Add("G1 " + GCode.CoordinateSystem.Pt2nc(mid, 3, ""));

                            //retreate, change position
                            Point3d retreatPoint = p_last + n_last * RetreatDistance;
                            Point3d retreatPointMid = mid + (n + n_last).Unit() * RetreatDistance;
                            Point3d retreatPointSafety = new Point3d(retreatPoint.X, retreatPoint.Y, Zsec);

                            //ncc.Add("G0 " + GCode.CoordinateSystem.Pt2nc(retreatPointMid, 3, "") + AB_last.Item3 + " (retreat)");
                            //ncc.Add("G0 " + GCode.CoordinateSystem.Pt2nc(retreatPointMid, 3, "") + AB.Item3 + " (rotate_In_Safety)");

                            ncc.Add("G0 " + GCode.CoordinateSystem.Pt2nc(retreatPointMid, 3, "") + " (retreat)");
                            ncc.Add("G0" + AB.Item3 + " (rotate_in_retreat)");

                            //Back to middle - finish edge with another direction
                            ncc.Add("G1 " + GCode.CoordinateSystem.Pt2nc(mid, 3, "") + " F" + (Speed).ToString() + " (return)");
                            //ncc.Add("G1 F" + (Speed).ToString());
                            ncc.Add("(********************_end_turn_**********************)");
                        }

                        //then add current position
                        string angle = AB == AB_last ? " " : AB.Item3;
                        ncc.Add(GCode.CoordinateSystem.Pt2nc(p, 3, "") + angle);
                    } //if first

                    if (j == polyline.Count - 1)
                    {
                        safety = p + (n * RetreatDistance);
                        ncc.Add("G1 " + GCode.CoordinateSystem.Pt2nc(safety, 3, "") + speed + " (safety_point_end)");
                    }

                    n_last = n;
                    p_last = p;
                    AB_last = AB;
                }//for j
                double z_last = i == polylines.Count - 1 ? Axes.ZCoord : Zsec;
                ncc.Add("G1 " + GCode.CoordinateSystem.Pt2nc(safety.X, safety.Y, z_last) + AB.Item3 + speed + " (last_point)");

                //Zsec
            }//for i

            ncc.Add("(____________end_cutting____________)");
            //ncc.Add("(ReplaceB)");
            ncc.Add("G0 Z" + Axes.ZCoord + " (endpos)");
            ncc.Add("G0" + Axes.HomePosition2 + Axes.DefaultRotation + " (endpos)");
            GCode.Write.WriteAndCheck(ref ncc, ref previewGeometry, filename, "5x_normal", tool.ToString());
            return ncc;
        }

        public static List<string> CNC5XCut2Polylines(GCode.ToolParameters tool, List<Curve> crvs, ref PreviewObject previewGeometry, ref List<Curve> sharpPolylines,
double toolr = 10, double Zsec = 650, double Speed = 20000, double RetreatDistance = 70, int infeed = 2, bool Notch = true, bool pairing = false, double angleTol = 60, double maxAngle = 80, int zero = 54, string filename = "P1234567")
        {
            List<string> ncc = new List<string> {
                "G"+zero.ToString() + " (G54/G55/G56_zero_in_maka)",
                //"P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)", //lift aspiration
                "T" + tool.id.ToString() + " M6" + " (txx_-_tool_name,_m6_-_change_tool)", //get Toool
                "S" + Utilities.GeometryProcessing.Lerp(tool.prescribedSpindleSpeed, tool.maxSpindleSpeed, 0.7).ToString() + " M3" + " (sxx_-_speed,_m3_-_clockwise)", //slow rmp "S2500 M3 M1"
                //"G47" + Axes.DefaultRotation +" F5000" + " (g47_-_3_axis_in_plane)",//neutral position "G1 G47 A0 B0 F5000 M1"
                //"G0 G49 G" + zero.ToString() + " X0 Y0 Z" + Zsec.ToString() + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "G0 G49 Z750" + " (go_to_safe_z_position_to_avoid_collision)" ,
                "G0 " + Axes.HomePosition + Axes.DefaultRotation + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "M08" + " (air_supply)" ,
                 "G1 F"+Speed.ToString() ,
                 "(ReplaceB)",
                "(____________start_cutting____________)",
                //"G0 X0 Y0 Z" + Zsec.ToString() + Axes.DefaultRotation+ " (startpos)"
            };

            List<List<Curve>> pairs = Utilities.GeometryProcessing.FindPairs(crvs, pairing);
            //Rhino.RhinoApp.WriteLine(pairs.Count.ToString());

            for (int p = 0; p != pairs.Count; p++)
            {
                Polyline ply0;
                Polyline ply1;

                pairs[p][0].TryGetPolyline(out ply0);
                pairs[p][1].TryGetPolyline(out ply1);

                if (angleTol > 0)
                {
                    var check = Utilities.GeometryProcessing.CheckAngle(ply0, ply1, angleTol);
                    ply0 = check.Item1;
                    ply1 = check.Item2;

                    sharpPolylines.AddRange(check.Item3);
                }

                string orient = (Utilities.GeometryProcessing.IsClockwiseClosedPolylineOnXYPlane(ply0)) ? "CounterClockwise" : "Clockwise";

                ncc.Add("(start " + orient + " pair no" + p.ToString() + ")");

                if (ply0.IsValid && ply1.IsValid)
                {
                    Point3d[] vrts = ply0.ToArray();
                    Point3d[] uvrts = ply1.ToArray();

                    Plane.FitPlaneToPoints(vrts, out Plane notPairingPlane);

                    for (int k = infeed; k >= 1; k--)                           // iterate infeeds
                    {
                        ncc.Add("(start infeed no" + k.ToString() + ")");
                        for (int i = 0; i != vrts.Length - 1; i++)              // iterate segments
                        {
                            Point3d p0 = vrts[i]; //#always
                            Point3d p0u = uvrts[i];
                            Point3d p2b = new Point3d();
                            Point3d p1b = new Point3d();
                            Point3d p1 = new Point3d();
                            Point3d p2 = new Point3d();
                            Point3d p1u = new Point3d();

                            if (i == 0)
                            {
                                p2b = vrts[vrts.Length - 3];
                                p1b = vrts[vrts.Length - 2];
                                p1 = vrts[i + 1];
                                p2 = vrts[i + 2];
                                p1u = uvrts[i + 1];
                            }
                            else if (i == 1)
                            {
                                p2b = vrts[vrts.Length - 2];
                                p1b = vrts[i - 1];
                                p1 = vrts[i + 1];
                                p2 = vrts[i + 2];
                                p1u = uvrts[i + 1];
                            }
                            else if (i == vrts.Length - 2)
                            {
                                p2b = vrts[i - 2];
                                p1b = vrts[i - 1];
                                p1 = vrts[0];
                                p2 = vrts[1];
                                p1u = uvrts[0];
                            }
                            else if (i == vrts.Length - 3)
                            {
                                p2b = vrts[i - 2];
                                p1b = vrts[i - 1];
                                p1 = vrts[i + 1];
                                p2 = vrts[0];
                                p1u = uvrts[i + 1];
                            }
                            else
                            {
                                p2b = vrts[i - 2];
                                p1b = vrts[i - 1];
                                p1 = vrts[i + 1];
                                p2 = vrts[i + 2];
                                p1u = uvrts[i + 1];
                            }

                            // ## DET TOOLPATH

                            Vector3d n1b = Rhino.Geometry.Vector3d.CrossProduct(p1b - p0, p0u - p0);        //#Srf Normal (last)
                            Vector3d n0 = Rhino.Geometry.Vector3d.CrossProduct(p0 - p1, p0u - p0);          //#Srf Normal (current)
                            Vector3d n1 = Rhino.Geometry.Vector3d.CrossProduct(p2 - p1, p1u - p1);          //#Srf Normal (next)
                            n1b.Unitize();
                            n1b *= toolr * -1;
                            n0.Unitize();
                            n0 *= toolr;
                            n1.Unitize();
                            n1 *= toolr;

                            Plane pl0 = new Plane(p0, (n1b + n0) / 2);                         //# ext bisector plane last/current
                            Plane pl1 = new Plane(p1, ((n0 + n1) / 2));                        //# ext bisector plane current/next

                            Line ln0 = new Line(p0 + (n0 * -1), p1 + (n0 * -1));                                     //# toolpath Line

                            double pm0;
                            double pm1;
                            Rhino.Geometry.Intersect.Intersection.LinePlane(ln0, pl0, out pm0);
                            Rhino.Geometry.Intersect.Intersection.LinePlane(ln0, pl1, out pm1);
                            Point3d pt0 = ln0.PointAt(pm0);                                                 //# intersection with Plane 0
                            Point3d pt1 = ln0.PointAt(pm1);                                                 //# intersection with Plane 1

                            Point3d pt6 = new Point3d();
                            bool boolN = false;

                            Vector3d n44 = p1u - p1;
                            n44.Unitize();
                            Point3d ptXX = pt1 + n44 * 45;

                            Line XXA = new Line(p1, p1u);

                            Point3d ptC = XXA.ClosestPoint(pt1, false);
                            double l0 = ptC.DistanceTo(pt1) - toolr;                                        //# offset dist
                            Vector3d nnn = ptC - pt1;
                            nnn.Unitize();
                            Point3d pt3 = pt1 + nnn * l0;
                            Point3d pt4 = pt3 + (p1u - p1);
                            Line ln1 = new Line(pt3, pt4);                                                 //# cylinder axis

                            //## IDENTIFY INSIDE CORNERS
                            Vector3d r1l = p2b - p1b;                                 //# last back
                            Vector3d r1n = p0 - p1b;                                  //# last front
                            Vector3d al = p1b - p0;                                   //# current back
                            Vector3d an = p1 - p0;                                    //# current front
                            Vector3d bl = p0 - p1;                                    //# next back
                            Vector3d bn = p2 - p1;                                    //# next front
                            r1l.Unitize();
                            r1n.Unitize();
                            al.Unitize();
                            an.Unitize();
                            bl.Unitize();
                            bn.Unitize();

                            Vector3d cpr1 = Vector3d.CrossProduct(r1l, r1n);                           //# +- look 1 back
                            Vector3d cp0 = Vector3d.CrossProduct(al, an);                              //# +- look current
                            Vector3d cp1 = Vector3d.CrossProduct(bl, bn);                              //# +- look 1 ahead

                            if (orient == "Clockwise")
                            {
                                if (cpr1.Z < 0 && cp0.Z < 0 && cp1.Z > 0)     //# --+
                                    boolN = true;
                                else if (cpr1.Z < 0 && cp0.Z > 0 && cp1.Z > 0)   //# -++
                                    boolN = true;
                                else if (cpr1.Z > 0 && cp0.Z < 0 && cp1.Z > 0)       //# +-+
                                    boolN = true;
                            }
                            else if (orient == "CounterClockwise")
                            {
                                if (cp0.Z > 0)       //# +-+
                                    boolN = true;
                            }

                            Point3d[] pts = { pt0, pt1, pt1 + (p1u - p1), pt0 + (p0u - p0) };

                            Vector3d nh0 = ((pts[3] - pts[0]) / infeed) * (k - 1);
                            Vector3d nh1 = ((pts[2] - pts[1]) / infeed) * (k - 1);

                            Point3d p21 = pts[1] + nh1;  // infeed pts
                            Point3d p30 = pts[0] + nh0;

                            if (Notch == true)
                            {
                                if (k == 1)
                                {
                                    double pm2;

                                    //Rhino.RhinoApp.WriteLine(pairing.ToString());
                                    if (!pairing)
                                        Rhino.Geometry.Intersect.Intersection.LinePlane(ln1, notPairingPlane, out pm2);
                                    else
                                        Rhino.Geometry.Intersect.Intersection.LinePlane(ln1, Plane.WorldXY, out pm2);

                                    Point3d pt5 = ln1.PointAt(pm2);                                                     // pt at zero

                                    double l3 = p1.DistanceTo(new Point3d(p1u.X, p1u.Y, p1.Z));                         // gkath
                                    double l4 = p1.DistanceTo(p1u);                                                     // hypo
                                    double beta = Math.Asin(l3 / l4);                                                   // incl

                                    double l1 = Math.Abs(Math.Tan(beta) / toolr);                                       // compensate incl
                                    if (l1 >= toolr)
                                        l1 *= 0.2;
                                    l1 = 3.5;
                                    Vector3d vbv = p1 - p1u;
                                    vbv.Unitize();
                                    pt6 = pt5 + vbv * l1;                                                               // notch point for last infeed
                                    //Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(pt5);
                                }
                                else
                                {
                                    double pm3;                                                                         // notch point for regular infeeds (all but last)
                                    Plane newpl1 = new Plane(new Point3d(0, 0, p21.Z), new Vector3d(0, 0, 1));
                                    Rhino.Geometry.Intersect.Intersection.LinePlane(ln1, newpl1, out pm3);
                                    pt6 = ln1.PointAt(pm3);
                                }
                            }

                            int IPDtemp = 3;                                                                        // division for sim mach
                            if (Math.Abs(pts[0].DistanceTo(pts[1])) <= Math.Abs(pts[3].DistanceTo(pts[2])) + 0.5 && Math.Abs(pts[0].DistanceTo(pts[1])) >= Math.Abs(pts[3].DistanceTo(pts[2])) - 0.5)
                            {
                                IPDtemp = 1;                                                                        // simple cut
                            }

                            List<Point3d> ptsl = Utilities.GeometryProcessing.DividePoints(pts[0], pts[1], IPDtemp);
                            List<Point3d> ptsm = Utilities.GeometryProcessing.DividePoints(p30, p21, IPDtemp);
                            List<Point3d> ptsu = Utilities.GeometryProcessing.DividePoints(pts[3], pts[2], IPDtemp);

                            Point3d TCP2 = new Point3d();
                            Point3d TCP3 = new Point3d();
                            Point3d TCP_last = new Point3d();
                            //List<double> AB_last = new List<double> { 999, 999 };
                            Tuple<double, double, string> AB_last = new Tuple<double, double, string>(double.NaN, double.NaN, "ERROR");
                            Point3d TCP2_last = new Point3d();
                            Point3d TCP = new Point3d();
                            //List<double> AB = new List<double>();
                            Tuple<double, double, string> AB;
                            string strAB = null;
                            Vector3d nX = new Vector3d();

                            for (int m = 0; m != ptsl.Count; m++)
                            {
                                if (k == 0)
                                    TCP = ptsl[m];                                                                  // use base pts
                                else
                                    TCP = ptsm[m];                  // use infeed pts

                                Point3d ORP = ptsu[m];
                                Vector3d n = ORP - TCP;
                                n.Unitize();
                                AB = GCode.CoordinateSystem.AB180(n);
                                nX = n;

                                TCP2 = Utilities.GeometryProcessing.VecPlnInt(TCP, n, RetreatDistance);              //#retreat pt
                                TCP3 = Utilities.GeometryProcessing.VecPlnInt(TCP, n, Zsec);             //#safe pt

                                strAB = " " + Axes.A + AB.Item1.ToString() + " " + Axes.B + AB.Item2.ToString();

                                if (m == 0 && i == 0 && k == infeed)                                                                         //# 1.Infeed to 1.point in 1.segment: additional safe point !!! k must be set to 1 when only 1 infeed !!!
                                {
                                    ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(new Point3d(TCP3.X, TCP3.Y, Zsec)) + Axes.DefaultRotation + " (first security)");
                                    ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(TCP3) + strAB + " (first security)");
                                }
                                else
                                {
                                    if (AB_last.Item1 != double.NaN)//(m != 0)
                                    {
                                        if (AB.Item1 > AB_last.Item1 + maxAngle || AB.Item1 < AB_last.Item1 - maxAngle)                                  //# check for turn
                                        {
                                            ncc.Add("(********************** turn *************************)");
                                            ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(TCP2_last) + " (retreat)");
                                            ncc.Add("G0" + strAB + " (new ab)");
                                            ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(TCP_last) + strAB + " F" + ((int)(Speed * 0.1)).ToString() + " (return)");
                                            ncc.Add("(******************** end turn **********************)");
                                        }
                                    }
                                }

                                if (m == 0 && i == 0)
                                    ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(TCP) + strAB + " F" + ((int)(Speed * 0.1)).ToString() + " (dive)");                                               //# machine vertical  + " (i" + i.ToString() + " m" + m.ToString() + " k" + k.ToString() + ")"
                                else
                                    ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(TCP) + strAB + " F" + Speed.ToString());                  //# normal +" (i"+i.ToString()+" m"+m.ToString()+" k"+k.ToString()+")"

                                TCP_last = TCP;                                                                          //# save data from this loop for next one
                                TCP2_last = TCP2;
                                AB_last = AB;
                            }

                            if (Notch == true)
                            {
                                if (boolN == true)
                                {
                                    ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(pt6) + strAB + " F" + Speed.ToString() + " (notch 1)");
                                    previewGeometry.PreviewLines0.Add(new Line(pt6, pt6 + nX * 50));
                                    // Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(new Line(pt6, pt6+nX*50));

                                    if (i == vrts.Length - 2)
                                        ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(TCP) + strAB + " F" + Speed.ToString() + "(notch on extraction pt, ret to tcp)");
                                }
                            }

                            if (i == vrts.Length - 2 && k == 1)
                            {
                                ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(TCP3) + " (last security)");
                                ncc.Add("G0" + GCode.CoordinateSystem.Pt2nc(new Point3d(TCP3.X, TCP3.Y, Zsec)) + strAB + " (last security)");
                            }
                        }
                    }
                }
            }
            ncc.Add("G0 Z" + Axes.ZCoord + " (endpos)");
            ncc.Add("G0" + Axes.HomePosition2 + Axes.DefaultRotation + " (end pos)");

            GCode.Write.WriteAndCheck(ref ncc, ref previewGeometry, filename, "5x_3dcrvs", tool.ToString());
            //ncc.Add("(ReplaceB)");
            return ncc;
        }

        /// <summary>
        ///     Returns:
        ///     CNC-file saved to directory E:\
        ///     generated CNC-file will be read and printed to screen(for verification)
        ///     generated CNC-file will be read, toolpath will be added to document(for verification)
        /// </summary>
        /// <param name="drillingLines"></param>
        /// <param name="previewGeometry"></param>
        /// <param name="filename - 8 - digit filename in format P0000000"></param>
        /// <param name="zero  - CNC Zeropoint (e.g.G54, G55, G56... )"></param>
        /// <param name="toolr - Milling tool radius in mm"></param>
        /// <param name="toolID -  Tool ID Number in the CNC machine"></param>
        /// <param name="Zsec - Safe Plane over workpiece.Program begins and starts at this Z-height"></param>
        /// <param name="XYfeed Speed"></param>
        /// <param name="Zfeed Speed"></param>
        /// <param name="RetreatDistance"></param>
        /// <returns></returns>
        public static List<string> CNC5X3DDrill(GCode.ToolParameters tool, List<Line> lines, ref PreviewObject previewGeometry,
   string filename = "P1234567", double Zsec = 650, double Speed = 20000, double RetreatDistance = 70, int zero = 54, int infeed = 1)
        {
            List<string> ncc = new List<string> {
                "G"+zero.ToString() + " (G54 - default Maka/G55/G59_zero_in_maka)",
                //"P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)", //lift aspiration
                "T" + tool.id.ToString() + " M6" + " (txx_-_tool_name,_m6_-_change_tool)", //get Toool
                "S" + Utilities.GeometryProcessing.Lerp(tool.prescribedSpindleSpeed, tool.maxSpindleSpeed, 0.7).ToString() + " M3" + " (sxx_-_speed,_m3_-_clockwise)", //slow rmp "S2500 M3 M1"
                //"G47" +Axes.DefaultRotation +" F5000" + " (g47_-_3_axis_in_plane)",//neutral position "G1 G47 A0 B0 F5000 M1"
                //"G0 G49 G" + zero.ToString() + " X0 Y0 Z" + Zsec.ToString() + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "G0 G49 Z750" + " (go_to_safe_z_position_to_avoid_collision)" ,
                "G0 " + Axes.HomePosition + Axes.DefaultRotation + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "M08" + " (air_supply)" ,
                 "G1 F"+Speed.ToString() ,
                 "(ReplaceB)",
                "(____________start_cutting____________)"
            };

            // double BeforeCuttingZSecturity = 800;

            Point3d p0, p1, sp;
            Vector3d n0;

            int i = 0;
            foreach (Line l in lines)
            {
                if (!l.IsValid) continue;

                List<Line> multiLines = new List<Line>();
                double step = 1.0 / (double)infeed;
                for (int j = 0; j < infeed; j++)
                {
                    double t = (step * (j + 1));
                    Line line = new Line(l.From, l.PointAt(t));//
                    multiLines.Add(line);
                }

                for (int j = 0; j < multiLines.Count; j++)
                {
                    //l = multiLines[j];

                    p0 = multiLines[j].From;
                    p1 = multiLines[j].To;
                    n0 = (p0 - p1);
                    n0.Unitize();
                    sp = p0 + (n0 * RetreatDistance);
                    var AB = GCode.CoordinateSystem.AB180(n0);

                    //In order not to bump into high elements on the table, do movements on Zsec axis, before drilling
                    //if (j == 0)
                    //ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);
                    //ncc.Add("G1" + Axes.DefaultRotation + " F4000");

                    if (i == 0)
                        ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);

                    //Perform drilling
                    ncc.Add("(Hole " + i++.ToString() + ")");
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(sp, 3, "") + AB.Item3);              //safety
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(p1, 3, "") + AB.Item3 + " F" + ((int)(Speed * 0.2)).ToString());        //plunge
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(sp, 3, "") + AB.Item3);    //safety
                    ncc.Add("F" + Speed.ToString());    //safety

                    //In order not to bump into high elements on the table, do movements on Zsec axis, before drilling
                    // if (i == lines.Count  )
                    //ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);
                }
            }

            ncc.Add("(____________end_drilling____________)");
            //ncc.Add("(ReplaceB)");
            //ncc.Add("G0 X0 Y0 Z" + Zsec.ToString() + Axes.DefaultRotation + " (endpos)");
            ncc.Add("G0 Z" + Zsec.ToString() + " (BeforeCuttingZSecturity)");
            ncc.Add("G0 Z" + Axes.ZCoord + " (endpos)");
            ncc.Add("G0" + Axes.HomePosition2 + Axes.DefaultRotation + " (endpos)");

            ncc.Add("P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)");

            //Rhino.RhinoApp.WriteLine("Writing");
            Raccoon.GCode.Write.WriteAndCheck(ref ncc, ref previewGeometry, filename, "5x_3dcrvs", tool.ToString());
            return ncc;
        }

        /// <summary>
        ///     Returns:
        ///     CNC-file saved to directory E:\
        ///     generated CNC-file will be read and printed to screen(for verification)
        ///     generated CNC-file will be read, toolpath will be added to document(for verification)
        /// </summary>
        /// <param name="drillingLines"></param>
        /// <param name="previewGeometry"></param>
        /// <param name="filename - 8 - digit filename in format P0000000"></param>
        /// <param name="zero  - CNC Zeropoint (e.g.G54, G55, G56... )"></param>
        /// <param name="toolr - Milling tool radius in mm"></param>
        /// <param name="toolID -  Tool ID Number in the CNC machine"></param>
        /// <param name="Zsec - Safe Plane over workpiece.Program begins and starts at this Z-height"></param>
        /// <param name="XYfeed Speed"></param>
        /// <param name="Zfeed Speed"></param>
        /// <param name="RetreatDistance"></param>
        /// <returns></returns>
        public static List<string> CNC5X3DDrillLathe(GCode.ToolParameters tool, List<Line> lines, ref PreviewObject previewGeometry,
   string filename = "P1234567", double Zsec = 650, double Speed = 20000, double RetreatDistance = 70, int zero = 54, int infeed = 1)
        {
            List<string> ncc = new List<string> {
                "G"+zero.ToString() + " (G54 - default Maka/G55/G59_zero_in_maka)",
                //"P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)", //lift aspiration
                "T" + tool.id.ToString() + " M6" + " (txx_-_tool_name,_m6_-_change_tool)", //get Toool
                "S" + Utilities.GeometryProcessing.Lerp(tool.prescribedSpindleSpeed, tool.maxSpindleSpeed, 0.7).ToString() + " M3" + " (sxx_-_speed,_m3_-_clockwise)", //slow rmp "S2500 M3 M1"
                //"G47" +Axes.DefaultRotation +" F5000" + " (g47_-_3_axis_in_plane)",//neutral position "G1 G47 A0 B0 F5000 M1"
                //"G0 G49 G" + zero.ToString() + " X0 Y0 Z" + Zsec.ToString() + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "G0 G49 Z750" + " (go_to_safe_z_position_to_avoid_collision)" ,
                "G0 " + Axes.HomePosition + Axes.DefaultRotation + " (g49_means_5axis_toolpath_-_startpos)" ,// Tool length compenstatio cancel
                "M08" + " (air_supply)" ,
                 "G1 F"+Speed.ToString() ,
                 "(ReplaceB)",

                "(____________start_cutting____________)"
            };

            // double BeforeCuttingZSecturity = 800;
            ncc.Add("M13 S3:10");
            Point3d p0, p1, sp;
            Vector3d n0;

            int i = 0;
            foreach (Line l in lines)
            {
                if (!l.IsValid) continue;

                List<Line> multiLines = new List<Line>();
                double step = 1.0 / (double)infeed;
                for (int j = 0; j < infeed; j++)
                {
                    double t = (step * (j + 1));
                    Line line = new Line(l.From, l.PointAt(t));//
                    multiLines.Add(line);
                }

                for (int j = 0; j < multiLines.Count; j++)
                {
                    //l = multiLines[j];

                    p0 = multiLines[j].From;
                    p1 = multiLines[j].To;
                    n0 = (p0 - p1);
                    n0.Unitize();
                    sp = p0 + (n0 * RetreatDistance);
                    var AB = GCode.CoordinateSystem.AB180(n0);

                    //In order not to bump into high elements on the table, do movements on Zsec axis, before drilling
                    //if (j == 0)
                    //ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);
                    //ncc.Add("G1" + Axes.DefaultRotation + " F4000");

                    if (i == 0)
                        ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);

                    //Perform drilling
                    ncc.Add("(Hole " + i++.ToString() + ")");
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(sp, 3, "") + AB.Item3);              //safety
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(p1, 3, "") + AB.Item3 + " F" + 500.ToString());        //plunge ((int)(Speed * 0.2)).ToString());
                    ncc.Add(GCode.CoordinateSystem.Pt2nc(sp, 3, "") + AB.Item3);    //safety
                    ncc.Add("F" + Speed.ToString());    //safety

                    //In order not to bump into high elements on the table, do movements on Zsec axis, before drilling
                    // if (i == lines.Count  )
                    //ncc.Add("G1" + GCode.CoordinateSystem.Pt2nc(new Point3d(sp.X, sp.Y, Zsec)) + AB.Item3);
                }
            }

            ncc.Add("M15");
            ncc.Add("(____________end_drilling____________)");
            //ncc.Add("(ReplaceB)");
            //ncc.Add("G0 X0 Y0 Z" + Zsec.ToString() + Axes.DefaultRotation + " (endpos)");
            ncc.Add("G0 Z" + Zsec.ToString() + " (BeforeCuttingZSecturity)");
            ncc.Add("G0 Z" + Axes.ZCoord + " (endpos)");
            ncc.Add("G0" + Axes.HomePosition2 + Axes.DefaultRotation + " (endpos)");

            ncc.Add("P4010:250 (lift_aspiration,_plastic_cover_for_ventilation)");

            //Rhino.RhinoApp.WriteLine("Writing");
            Raccoon.GCode.Write.WriteAndCheck(ref ncc, ref previewGeometry, filename, "5x_3dcrvs", tool.ToString());
            return ncc;
        }
    }
}