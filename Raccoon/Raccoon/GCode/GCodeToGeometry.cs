using Raccoon.Utilities;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.GCode
{
    public struct AnimationValues
    {
        public List<XYZAB> values;
        public List<string> linesCleaned;
        public ToolParameters tool;
        public Dictionary<int, ToolParameters> tools;
        public double t;
    }

    public struct XYZAB
    {
        public double X;
        public double Y;
        public double Z;
        public double B;
        public double A;

        public bool boolX;
        public bool boolY;
        public bool boolZ;
        public bool boolA;
        public bool boolB;
    }

    public static class GCodeToGeometry
    {
        public static List<string> GCodeManual()
        {
            return new List<string>() {
                "P1234567 - Filename - Maximum seven number name",
                "P4010:250 - Plastic Cover for ventilation - covered - 0, lifted - 250",
                "T42 - Tool Name",
                "S18000 - Spindle Speed",
                "A - Rotation Axis in Plane XY -270 to 270 degrees - horizontal rotation",
                "B - Rotation Axis in Plane XZ or YZ -105 to 105 degrees - vertical rotation ",
                "X - first point coordinate",
                "Y - second point coordinate",
                "Z - third point coordinate",

                " ",
                "M-Code Reference",
                "_______________________________________",
                "M0 - Program Stop",
                "M1 - Optional Program Stop",
                "M2 - Program End",
                "M3 - clockwise",
                "M4 - counterclockwise",
                "M5 - stop the spindle from turning",
                "M6 - change tool",
                "M7 - Mist coolant on",
                "M8 - Air supply (noisy stuff) To turn flood coolant on",
                "M9 - All coolant off",
                "M30 - Program end and Rewind",
                "M48 - Enable speed and feed override",
                "M49 - Disable speed and feed override",
                " ",
                "G-Code Reference",
                "_______________________________________",
                "G0 - rapid positioning outside of the workpiece",
                "G1 - linear interpolation",
                "G2 - Clockwise circular/helical interpolation",
                "G3 - Counterclockwise circular/helical interpolation",
                "G4 - Dwell",
                "G10 - Coordinate system origin setting",
                "G17 - XY Plane select",
                "G18 - XZ Plane select",
                "G19 - YZ Plane select",
                "G20/G21 - Inch/millimeter unit",
                "G28 - Return home",
                "G28.1 - Reference axes",
                "G30 - Return home",
                "G38 - Straight probe",
                "G40 - Cancel cutter radius compensation",
                "G41/G42 - Start cutter radius compensation left/right",
                "G43 - Apply tool length offset",
                "G47 - Neutral position - Engrave Sequential Serial Number",
                "G49 - Cancel tool length offset / Origin Point",
                "G50 - Reset all scale factors to 1.0",
                "G51 - Set axis data input scale factors",
                "G53 - Move in absolute machine coordinate system",
                "G54 - Use fixture offset 1",
                "G55 - Use fixture offsdet 2",
                "G56-58 - Use fixture offset 3,4,5",
                "G59 - Use fixture offset 6 / use general fixture number",
                "G61/64 - Exact stop/Constant Velocity mode",
                "G73 - Canned cycle - peck drilling",
                "G80 - Cancel motion mode (including canned cycles)",
                "G81 - Canned cycle - drilling",
                "G82 - Canned cycle - drilling with dwell",
                "G83 - Canned cycle - pack drilling",
                "G85 - Canned cycle - boring, no dwell, feed out",
                "G86 - Canned cycle – boring, spindle stop, rapid out",
                "G88 - Canned cycle – boring, spindle stop, manual out",
                "G89 - Canned cycle – boring, dwell, feed out",
                "G90 - Absolute distance mode",
                "G91 - Incremental distance mode",
                "G92 - Offset coordinates and set parameters",
                "G92.x - Cancel G92 etc.",
                "G93 - Inverse time feed mode",
                "G94 - Feed per minute mode",
                "G95 - Feed per rev mode",
                "G98 - Initial level return after canned cycles",
                "G99 - R-point level return after canned cycles",
            };
        }

        public static List<string> defaultToolPath = new List<string>() {
            "P0000000",
            "N20 T0 M6",
            "N30 G47 A0 B0 F5000",
            "N40 S4500 M3",
            "N50 G49 G54",
            "N60 G0 X0 Y0 Z300 (startpos)",
            "N70 G0 X32.4 Y2376.72 Z300",
            "N80 G0 X32.4 Y2376.72 Z40 (over)",
            "N90 G1 X32.4 Y2376.72 Z0 F2000 (dive)",
            "N100 G0 X32.4 Y2376.72 Z40 (retreat)",
            "N110 G0 X32.4 Y2376.72 Z300",
            "N120 G0 X0 Y3770 Z300 (endpos)",
            "N130 M5",
            "N140 M30",
            "#"
        };

        /// <summary>
        /// GCode of each line
        /// </summary>
        /// <param name="GCode"></param>
        ///Read GCode string and vizualize it in Rhino
        ///Wont draw if there is more than one white space sequentually

        public static void DrawToolpath(List<string> GCode, ref PreviewObject preview, double position = -1)
        {
            //try {
            if (GCode == null) return;

            //Rhino.RhinoApp.WriteLine("Drawing");

            List<string> linesCleaned = new List<string>();
            foreach (string l in GCode)
            {
                if (l.Contains('X') || l.Contains('Y') || l.Contains('Z') || l.Contains('A') || l.Contains('B'))
                    linesCleaned.Add(l);
            }

            GCode = linesCleaned;

            // int layer_index0 = Rhino.RhinoDoc.ActiveDoc.Layers.Add(IBOIS.GCode.Write.TimeStamp() + "_CNC", Color.Red);
            Color col0 = Color.FromArgb(255, 0, 0);
            Color col1 = Color.FromArgb(0, 127, 0);
            Color col2 = Color.FromArgb(0, 255, 255);

            double defaultVal = double.NaN;
            double A = defaultVal, B = defaultVal, X = defaultVal, Y = 0, Z = defaultVal, d0 = defaultVal, d1 = defaultVal;
            Point3d pt = new Point3d(0, 0, defaultVal), lastPt = new Point3d(0, 0, defaultVal);

            List<double> maxA = new List<double>();
            List<double> maxB = new List<double>();
            List<double> minZ = new List<double>();

            for (int i = 0; i < GCode.Count; i++)
            {
                //RhinoApp.WriteLine(breakVaue.ToString());

                //RhinoApp.WriteLine(preview.PreviewLines0.Count.ToString());
                //RhinoApp.WriteLine(preview.PreviewLines1.Count.ToString());
                //RhinoApp.WriteLine(preview.PreviewLines2.Count.ToString());
                //Rhino.RhinoApp.WriteLine(i.ToString());

                bool boolAP = false;
                bool boolG0 = false;
                bool boolG1 = false;

                string[] tokens = GCode[i].Split(new char[] { ' ' }); //Split into words by whitespaces

                foreach (string obj in tokens)
                { //Look at each token
                    if (obj.Length == 0) continue;
                    if (obj[0] != '(')
                    {
                        if (obj[0] == 'G')
                        {
                            if (obj[1] == '1') boolG1 = true; //col=col1 lay=layer_index1
                            else if (obj[1] == '0') boolG0 = true; //col=col0 lay=layer_index0
                        }
                        else if (obj[0] == 'X')
                        {
                            double.TryParse(obj.Remove(0, 1), out X);
                            boolAP = true;
                        }
                        else if (obj[0] == 'Y')
                        {
                            double.TryParse(obj.Remove(0, 1), out Y);
                            boolAP = true;
                        }
                        else if (obj[0] == 'Z')
                        {
                            double.TryParse(obj.Remove(0, 1), out Z);
                            boolAP = true;
                        }
                        else if (obj[0] == 'A')
                        {
                            double.TryParse(obj.Remove(0, 1), out A);
                        }
                        else if (obj[0] == 'B')
                        {
                            double.TryParse(obj.Remove(0, 1), out B);
                        }
                    }
                }//foreach

                if (boolAP == true)
                {//only if a point is defined in NC-Code, point will be created
                    pt = new Point3d(X, Y, Z);
                    preview.vertices.Add(pt);
                }

                //Bake lines
                if (pt.Z != defaultVal & lastPt.Z != defaultVal)
                {
                    if (pt != lastPt)
                    {
                        Line line = new Line(lastPt, pt);
                        double d3 = lastPt.DistanceTo(pt);
                        if (boolG0 == true) d0 += d3;
                        if (boolG1 == true) d1 += d3;

                        //var attr = new Rhino.DocObjects.ObjectAttributes();
                        //attr.ColorSource = Rhino.DocObjects.ObjectColorSource.ColorFromObject;
                        //if (boolG0 == true) attr.ObjectColor = col0;
                        //if (boolG1 == true) attr.ObjectColor = col1;
                        //attr.LayerIndex = layer_index0;//put new layers
                        //Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(line, attr);

                        if (boolG1 == true) preview.PreviewLines0.Add(line);
                        if (boolG0 == true) preview.PreviewLines1.Add(line);
                        preview.PreviewLines0.Add(line);
                    }
                }

                lastPt = pt;

                if (pt.Z != defaultVal) minZ.Add(pt.Z);//add to checklist
                if (A != defaultVal) maxA.Add(A);//add to checklist
                if (B != defaultVal) maxB.Add(B);//add to checklist

                if (A != 0 || B != 0)
                {
                    Vector3d v0 = new Vector3d(0, 0, 1);
                    Vector3d XV = new Vector3d(1, 0, 0); //X-Axis unit vector
                    Vector3d ZV = new Vector3d(0, 0, 1); //Y-Azis unit vector

                    Vector3d XV2 = new Vector3d(XV);
                    Vector3d v1 = new Vector3d(v0);
                    XV2.Rotate(Rhino.RhinoMath.ToRadians(A), ZV);
                    v1.Rotate(Rhino.RhinoMath.ToRadians(B), XV2);
                    //ToDegrees
                    //ToRadians

                    Vector3d v2 = v1 * 3;//*3
                    Point3d pt1 = pt + v2;
                    Point3d p3 = new Point3d(pt);
                    Point3d p4 = new Point3d(pt1);
                    //Line line1 = new Line(p3,p4);
                    Line line1 = new Line(p3, p4 + (p4 - p3) * 5);

                    // preview.PreviewLines2.Add(line1);

                    //Rhino.RhinoApp.WriteLine(line1.Length.ToString());

                    A = 0; //Reset double A
                    B = 0; // Reset double B
                }

                int id = (int)Math.Ceiling((position / (1.0 / (GCode.Count - 1))));
                if (i == id) break;
            }//end of all lines looping

            string strZ = (minZ.Count != 0) ? "Z Positions: " + minZ.Min().ToString() + " to " + minZ.Max().ToString() : "Z Positions: None";

            string strA = (minZ.Count != 0) ? "A Angles: " + maxA.Min().ToString() + " to " + maxA.Max().ToString() : "A Angles: None";
            string strB = (minZ.Count != 0) ? "B Angles: " + maxB.Min().ToString() + " to " + maxB.Max().ToString() : "B Angles: None";

            string strG0 = (minZ.Count != 0) ? "G0 Length: " + (Math.Round(d0 / 1000, 2)).ToString() + "m / " + (Math.Round(d1 / 10000, 2)).ToString() + "min (@10m/min)" : "G0 Length: None";
            string strG1 = (minZ.Count != 0) ? "G1 Length: " + (Math.Round(d1 / 1000, 2)).ToString() + "m / " + (Math.Round(d1 / 5000, 2)).ToString() + "min(@5m / min)" : "G1 Length: None";
            preview.outputInformation = "\n\n" + strZ + "\n" + strA + "\n" + strB + "\n\n" + strG0 + "\n" + strG1;
            //IBOIS.UI.RhinoUI.MessageBox( "\n\n" + strZ + "\n" + strA + "\n" + strB + "\n\n" + strG0 + "\n" + strG1,  64, "Analyzing GCode");

            //} catch (Exception e) {
            //Rhino.RhinoApp.WriteLine(e.ToString());
            //}
        }

        /// <summary>
        /// Animate 3D Maka
        /// </summary>
        /// <param name="lines"></param>
        /// <param name="t"></param>
        /// <param name="Rtool"></param>
        /// <param name="Ltool"></param>
        /// <returns></returns>
        public static AnimationValues CNCAnim(List<string> lines = null, double t = 0.5)
        {
            //IF NOTHING IS SUPPLIED ASSIGN DEFAULT TOOLPATH
            if (lines == null)
                lines = defaultToolPath;

            ToolParameters tool = new ToolParameters() { id = -1, radius = 10, length = 150, maxSpindleSpeed = -1, prescribedSpindleSpeed = -1, turn = 3, saw = 0, cutLength = 150 * 0.25, holderRadius = 25.000 };

            //REMOVE ALL STUFF THAT IS NOT GEOMETRY
            List<string> linesCleaned = new List<string>();
            List<XYZAB> values = new List<XYZAB>();
            //double x = 0; double y = 3500; double z = 650; double a = 0; double b = 0;
            double x = 0; double y = 0; double z = 650; double a = 0; double b = 0;
            Dictionary<int, ToolParameters> tools = new Dictionary<int, ToolParameters>();

            foreach (string l in lines)
            {
                //Get values from string, update current coordinates, if some of them are missing add to current values
                if (l[0] == 'N')
                {
                    if (l.Contains(Axes.X) || l.Contains(Axes.Y) || l.Contains(Axes.Z) || l.Contains(Axes.A) || l.Contains(Axes.B))
                    {
                        XYZAB value = l.Value();
                        if (value.boolX) x = value.X; else value.X = x;
                        if (value.boolY) y = value.Y; else value.Y = y;
                        if (value.boolZ) z = value.Z; else value.Z = z;
                        if (value.boolA) a = value.A; else value.A = a;
                        if (value.boolB) b = value.B; else value.B = b;
                        values.Add(value);
                        linesCleaned.Add(l);
                    }
                }
                //Retrieve information about tool
                else if (l[0] == '(' && l[1] == '*')
                {
                    ToolParameters GCodeTool = Raccoon.GCode.Tool.ToolFromOneString(l);
                    if (tool.id != 0)
                    {
                        tool = GCodeTool;
                        tools.Add(values.Count, GCodeTool);
                    }
                }
            }

            return new AnimationValues() { values = values, linesCleaned = linesCleaned, tool = tool, t = t, tools = tools };
        }

        public static XYZAB Value(this string gcodeLine)
        {
            //XYZAB c = new XYZAB() { boolX = last.boolX, boolY = last.boolY, boolZ = last.boolZ, boolA = last.boolA, boolB = last.boolB, X = last.X, Y = last.Y, Z = last.Z, A = last.A, B = last.B };
            XYZAB c = new XYZAB()
            {
                X = 0,
                Y = 0,
                Z = 0,
                A = 0,
                B = 0,
                boolX = false,
                boolY = false,
                boolZ = false,
                boolA = false,
                boolB = false,
            };

            //Text Splitter
            string[] words = gcodeLine.Split(new Char[] { ' ' });
            string currentString = " ";

            foreach (string word in words)
            {
                currentString += (" " + word);

                if ((word.Length > 1))
                {
                    if (word[0] == Axes.X)
                    {
                        c.boolX = double.TryParse(word.Remove(0, 1), out c.X);
                    }
                    if (word[0] == Axes.Y)
                    {
                        string str1 = word.Remove(0, 1);
                        c.boolY = double.TryParse(str1, out c.Y);
                    }
                    if (word[0] == Axes.Z)
                    {
                        string str2 = word.Remove(0, 1);
                        c.boolZ = double.TryParse(str2, out c.Z);
                    }
                    if (word[0] == Axes.B)
                    {
                        string str3 = word.Remove(0, 1);
                        c.boolB = double.TryParse(str3, out c.B);
                    }
                    if (word[0] == Axes.A)
                    {
                        string str4 = word.Remove(0, 1);
                        c.boolA = double.TryParse(str4, out c.A);
                    }
                }
            }

            return c;
        }

        public static Tuple<List<Mesh>, int, string> FromValuesToGeometry(AnimationValues animationValues)
        {
            var lines = animationValues.linesCleaned; //strings only used for maka movement
            string currentString = ""; //current string
            int tInt = 0; //current line
            //List<Brep> breps = new List<Brep>();//Output geometry
            List<Mesh> geo = new List<Mesh>();//Output geometry

            double T = animationValues.t * (animationValues.values.Count - 1);//current position + interpolation
            int curr = (int)Math.Ceiling(T);//next position
            int prev = (int)Math.Floor(T);//prev position

            int toolID = 0;
            foreach (var keyValue in animationValues.tools)
            {
                if (keyValue.Key <= curr)
                {
                    toolID = keyValue.Key;
                }
            }

            if (curr >= 0 && animationValues.t <= animationValues.values.Count - 1)
            {
                var currentTool = animationValues.tools[toolID];

                currentString = lines[curr];
                tInt = curr;
                XYZAB valuesCurr = animationValues.values[curr];
                XYZAB valuesPrev = animationValues.values[prev];
                XYZAB v = LerpXYZAB(valuesPrev, valuesCurr, T % 1);//interpolate current and previous positions

                Point3d pt = new Point3d(v.X, v.Y, v.Z);
                Mesh pipe = GeometryProcessing.MeshPipe(Plane.WorldXY);
                Mesh pipe1 = GeometryProcessing.MeshPipe(Plane.WorldXY, 20);

                //Point3d tablecenter = new Point3d(750, 1250, -25);// table
                //Box table = new Box(new Plane(tablecenter, Plane.WorldXY.Normal, Plane.WorldXY.XAxis), new Interval(-25, 25), new Interval(-750, 750), new Interval(-1250, 1250));
                //geo.Add(Mesh.CreateFromBox(table,1,1,1));
                //breps.Add(table.ToBrep());

                Mesh meshLast = new Mesh();

                Plane f0 = new Plane(pt, pt + new Vector3d(-1, 0, 0), pt + new Vector3d(0, -1, 0));
                Plane f1 = new Plane(pt, pt + new Vector3d(-1, 0, 0), pt + new Vector3d(0, -1, 0)); //horizontal plane of machine

                f0.Rotate((Math.PI / 180) * v.A, f1.ZAxis);
                f1.Rotate((Math.PI / 180) * v.A, f1.ZAxis);
                f1.Rotate((Math.PI / 180) * (v.B * -1), f1.XAxis);

                ////////////////////  SPINDLE
                //////         ______________
                //////        |             |
                //////--------|      B      |
                //////        |_____________|

                if (currentTool.saw != 1)
                    meshLast.Append(pipe1.TransformMesh(f1, currentTool.radius, currentTool.length));//toolCutting
                else
                    geo.Add(pipe.TransformMesh(f1, currentTool.holderRadius, currentTool.length));//holder 2

                if (currentTool.saw == 1)
                    meshLast.Append(pipe1.TransformMesh(f1, currentTool.radius, 5));//saw

                //Change frame normal by tool length
                f1.Origin += f1.Normal * currentTool.length;
                geo.Add(pipe.TransformMesh(f1, 55, 106));//toolHolderFixed
                geo.Add(pipe.TransformMesh(f1, currentTool.holderRadius, currentTool.cutLength - currentTool.length));//toolHolder2

                Box spbox = new Box(new Plane(f1.Origin + f1.Normal * 266.30, f1.XAxis, f1.YAxis), new Interval(-71.25, 71.25), new Interval(-70.50, 70.50), new Interval(-160.30, 160.30));
                geo.Add(Mesh.CreateFromBox(spbox, 1, 1, 1));

                // Tool Holder Axis A
                //  _____
                // |     |
                // |     |
                // |     |
                // |     |
                // |     |
                // |  A  |
                // \    /
                //  \__/

                Point3d pt1 = new Point3d((f1.Origin + (f1.Normal * 190)) + (f1.XAxis * -121));
                geo.Add(pipe.TransformMesh(new Plane(pt1, f1.XAxis), 92, -268.85));//cylinder3

                Point3d pt2 = new Point3d((pt1 + (f1.XAxis * -134.42)) + (f0.ZAxis * 199.5));
                Box box2 = new Box(new Plane(pt2, f0.XAxis, f0.YAxis), new Interval(-134.425, 134.425), new Interval(-92, 92), new Interval(-199.5, 199.5));
                geo.Add(Mesh.CreateFromBox(box2, 1, 1, 1));

                Point3d pt3 = new Point3d((pt2 + (f0.ZAxis * 120.5)) - (f0.XAxis * -215.92));
                geo.Add(pipe.TransformMesh(new Plane(pt3, f0.ZAxis), 90, 105));//cylinder4

                Mesh joined = new Mesh();
                foreach (Mesh m in geo)
                    joined.Append(m);

                geo.Clear();
                geo.Add(joined);
                geo.Add(meshLast);
            }

            return new Tuple<List<Mesh>, int, string>(geo, tInt, currentString);
        }

        public static XYZAB LerpXYZAB(XYZAB t0, XYZAB t1, double t)
        {
            return new XYZAB()
            {
                X = GeometryProcessing.Lerp(t0.X, t1.X, t),
                Y = GeometryProcessing.Lerp(t0.Y, t1.Y, t),
                Z = GeometryProcessing.Lerp(t0.Z, t1.Z, t),
                A = GeometryProcessing.Lerp(t0.A, t1.A, t),
                B = GeometryProcessing.Lerp(t0.B, t1.B, t),
            };
        }

        public static Tuple<List<Mesh>, int, string> FromValuesToGeometryCardan(AnimationValues animationValues)
        {
            var lines = animationValues.linesCleaned; //strings only used for maka movement
            string currentString = ""; //current string
            int tInt = 0; //current line
            //List<Brep> breps = new List<Brep>();//Output geometry
            List<Mesh> geo = new List<Mesh>();//Output geometry

            double T = animationValues.t * (animationValues.values.Count - 1);//current position + interpolation
            int curr = (int)Math.Ceiling(T);//next position
            int prev = (int)Math.Floor(T);//prev position

            int toolID = 0;
            foreach (var keyValue in animationValues.tools)
            {
                if (keyValue.Key <= curr)
                {
                    toolID = keyValue.Key;
                }
            }

            if (curr >= 0 && animationValues.t <= animationValues.values.Count - 1)
            {
                var currentTool = animationValues.tools[toolID];

                currentString = lines[curr];
                tInt = curr;
                XYZAB valuesCurr = animationValues.values[curr];
                XYZAB valuesPrev = animationValues.values[prev];
                XYZAB v = LerpXYZAB(valuesPrev, valuesCurr, T % 1);//interpolate current and previous positions

                Point3d pt = new Point3d(v.X, v.Y, v.Z);
                Mesh pipe = GeometryProcessing.MeshPipe(Plane.WorldXY);
                Mesh pipe1 = GeometryProcessing.MeshPipe(Plane.WorldXY, 20);

                //Point3d tablecenter = new Point3d(750, 1250, -25);// table
                //Box table = new Box(new Plane(tablecenter, Plane.WorldXY.Normal, Plane.WorldXY.XAxis), new Interval(-25, 25), new Interval(-750, 750), new Interval(-1250, 1250));
                //geo.Add(Mesh.CreateFromBox(table,1,1,1));
                //breps.Add(table.ToBrep());

                Mesh meshLast = new Mesh();

                Plane f0 = new Plane(pt, pt + new Vector3d(-1, 0, 0), pt + new Vector3d(0, -1, 0));
                Plane f1 = new Plane(pt, pt + new Vector3d(-1, 0, 0), pt + new Vector3d(0, -1, 0)); //horizontal plane of machine

                f0.Rotate((Math.PI / 180) * v.A, f1.ZAxis);
                f1.Rotate((Math.PI / 180) * v.A, f1.ZAxis);
                f1.Rotate((Math.PI / 180) * (v.B * -1), f1.XAxis);

                geo = Raccoon.Components.View.CardanAngle.Perform_Cardan_Simulation(
                    f1.Origin,
                    f1.Normal,
                    currentTool.length * 0.1,
                    currentTool.radius * 0.1,
                    currentTool.cutLength * 0.1,
                    currentTool.holderRadius * 0.1,
                    currentTool.saw == 1
                    ).ToList();

                //////////////////////  SPINDLE
                ////////         ______________
                ////////        |             |
                ////////--------|      B      |
                ////////        |_____________|

                //if (currentTool.saw != 1)
                //    meshLast.Append(pipe1.TransformMesh(f1, currentTool.radius, currentTool.length));//toolCutting
                //else
                //    geo.Add(pipe.TransformMesh(f1, currentTool.holderRadius, currentTool.length));//holder 2

                //if (currentTool.saw == 1)
                //    meshLast.Append(pipe1.TransformMesh(f1, currentTool.radius, 5));//saw

                ////Change frame normal by tool length
                //f1.Origin += f1.Normal * currentTool.length;
                //geo.Add(pipe.TransformMesh(f1, 55, 106));//toolHolderFixed
                //geo.Add(pipe.TransformMesh(f1, currentTool.holderRadius, currentTool.cutLength - currentTool.length));//toolHolder2

                //Box spbox = new Box(new Plane(f1.Origin + f1.Normal * 266.30, f1.XAxis, f1.YAxis), new Interval(-71.25, 71.25), new Interval(-70.50, 70.50), new Interval(-160.30, 160.30));
                //geo.Add(Mesh.CreateFromBox(spbox, 1, 1, 1));

                //// Tool Holder Axis A
                ////  _____
                //// |     |
                //// |     |
                //// |     |
                //// |     |
                //// |     |
                //// |  A  |
                //// \    /
                ////  \__/

                //Point3d pt1 = new Point3d((f1.Origin + (f1.Normal * 190)) + (f1.XAxis * -121));
                //geo.Add(pipe.TransformMesh(new Plane(pt1, f1.XAxis), 92, -268.85));//cylinder3

                //Point3d pt2 = new Point3d((pt1 + (f1.XAxis * -134.42)) + (f0.ZAxis * 199.5));
                //Box box2 = new Box(new Plane(pt2, f0.XAxis, f0.YAxis), new Interval(-134.425, 134.425), new Interval(-92, 92), new Interval(-199.5, 199.5));
                //geo.Add(Mesh.CreateFromBox(box2, 1, 1, 1));

                //Point3d pt3 = new Point3d((pt2 + (f0.ZAxis * 120.5)) - (f0.XAxis * -215.92));
                //geo.Add(pipe.TransformMesh(new Plane(pt3, f0.ZAxis), 90, 105));//cylinder4

                //Mesh joined = new Mesh();
                //foreach (Mesh m in geo)
                //    joined.Append(m);

                //geo.Clear();
                //geo.Add(joined);
                //geo.Add(meshLast);
            }

            return new Tuple<List<Mesh>, int, string>(geo, tInt, currentString);
        }
    }
}