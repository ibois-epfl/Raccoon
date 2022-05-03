using System;
using System.Collections.Generic;
using System.Linq;
using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;

using Rhino.Geometry;
using Raccoon;

namespace Raccoon
{
    public class SawBladeGH : CustomComponent
    {
        public override GH_Exposure Exposure => GH_Exposure.quarternary;

        public SawBladeGH()
          : base("SawBlade", "SawBlade",
              "SawBlade",
              "Robot/CNC")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            //pManager.AddGenericParameter("Element", "Element", "Element", GH_ParamAccess.item);
            pManager.AddCurveParameter("cut_polyline", " cut_polyline", "cut_polyline", GH_ParamAccess.list);
            pManager.AddCurveParameter("dir_polyline", " dir_polyline", "dir_polyline", GH_ParamAccess.list);
            pManager.AddNumberParameter("cut_type", "cut_type", "Cut = 0, Mill = 1, Drill = 2, SawBlade = 3,  \n SawBladeBisector = 4, Engrave = 5, MillPath = 6, MillCircular = 7,  \n SawCircular = 8, SawEnd = 9, Slice = 10, SawBladeSlice = 11,  OpenCut = 12", GH_ParamAccess.list);

            //pManager.AddPlaneParameter("RefPlane", "RefPlane", "RefPlane", GH_ParamAccess.item);//, Plane.WorldXY
            //pManager.AddNumberParameter("Angle", "Angle", "Angle", GH_ParamAccess.list);

            pManager.AddNumberParameter("Radius", "Radius", "Milling radius", GH_ParamAccess.item);
            pManager.AddNumberParameter("DivisionsU", "DivisionsU", "Height divisions", GH_ParamAccess.item);
            pManager.AddNumberParameter("DivisionsV", "DivisionsV", "Thickness Divisions", GH_ParamAccess.item);
            pManager.AddNumberParameter("cut90Degrees", "cut90Degrees", "cut90Degrees", GH_ParamAccess.item);

            pManager.AddNumberParameter("RetreateO", "RetreateO", "Retreate Distance", GH_ParamAccess.item);
            pManager.AddNumberParameter("RetreateZ", "RetreateZ", "Retreate Z coordinated", GH_ParamAccess.item);
            pManager.AddNumberParameter("Extend", "Extend", "Extend Sideways of toolR", GH_ParamAccess.item);

            pManager.AddNumberParameter("ToolID", "ToolID", "Tool ID Number in the CNC machine", GH_ParamAccess.item);//0
            pManager.AddNumberParameter("Speed", "Speed", "velocity of horizontal cutting in mm/min", GH_ParamAccess.item);//5000
            pManager.AddNumberParameter("Zsec", "Zsec", "Safe Plane over workpiece.Program begins and starts at this Z-height", GH_ParamAccess.item);//700
            pManager.AddNumberParameter("Retreate", "Retreate", "Height of the XY Plane for tool retreat", GH_ParamAccess.item);//70

            //pManager.AddTextParameter("Path", "Path", "Path", GH_ParamAccess.item, @"C:\Unity\20200715\ColabEPFL\ImaxProUnity\Assets\StreamingAssets\MachiningEPFL\test.txt");

            //pManager.AddNumberParameter("Speed", "Speed", "Speed", GH_ParamAccess.item, 100);
            //pManager.AddTextParameter("WObj", "WObj", "WObj", GH_ParamAccess.item, "RotatingTool default");
            //pManager.AddNumberParameter("90Rot", "90Rot", "90Rot rotates second paths for better reachability", GH_ParamAccess.item, 0);
            //pManager.AddMeshParameter("Object", "Object", "Object", GH_ParamAccess.item);

            pManager[1].Optional = true;
            pManager[2].Optional = true;
            pManager[3].Optional = true;
            pManager[4].Optional = true;
            pManager[5].Optional = true;
            pManager[6].Optional = true;
            pManager[7].Optional = true;
            pManager[8].Optional = true;

            pManager[9].Optional = true;
            pManager[10].Optional = true;
            pManager[11].Optional = true;
            pManager[12].Optional = true;
            pManager[13].Optional = true;
        }

        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add Curve

            Rectangle3d[] recValues = new Rectangle3d[] {
                new Rectangle3d(new Plane(new Point3d(200,-500,300),-Vector3d.ZAxis),100,400),
                new Rectangle3d(new Plane(new Point3d(200,-500,325),-Vector3d.ZAxis),100,400)
            };

            int[] recID = new int[] { 0, 1 };

            for (int i = 0; i < recID.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Curve ri = Params.Input[recID[i]] as Grasshopper.Kernel.Parameters.Param_Curve;
                if (ri == null || ri.SourceCount > 0 || ri.PersistentDataCount > 0) return;

                Attributes.PerformLayout();
                int x = (int)ri.Attributes.Pivot.X - 225;
                int y = (int)ri.Attributes.Pivot.Y;
                IGH_Param rect = new Grasshopper.Kernel.Parameters.Param_Curve();
                rect.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 0, recValues[i].ToNurbsCurve());
                rect.CreateAttributes();
                rect.Attributes.Pivot = new System.Drawing.PointF(x, y);
                rect.Attributes.ExpireLayout();
                document.AddObject(rect, false);
                ri.AddSource(rect);
            }

            //    //Add sliders

            double[] sliderValue = new double[] { 3, 0, 200, 0, 1, 80, 650, 1.00, 103, 7000, 700, 200 };
            double[] sliderMinValue = new double[] { 0, 0, 0, 0, 0, 0, 0, 0.00, 0, 5000, 0, 0 };
            double[] sliderMaxValue = new double[] { 12, 200, 400, 10, 1, 800, 800, 2.00, 200, 20000, 800, 800 };

            int[] sliderID = new int[] { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 };
            for (int i = 0; i < sliderValue.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Number ni = Params.Input[sliderID[i]] as Grasshopper.Kernel.Parameters.Param_Number;
                if (ni == null || ni.SourceCount > 0 || ni.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ni.Attributes.Pivot.X - 250;
                int y = (int)ni.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_NumberSlider slider = new Grasshopper.Kernel.Special.GH_NumberSlider();
                slider.SetInitCode(string.Format("{0}<{1}<{2}", sliderMinValue[i], sliderValue[i], sliderMaxValue[i]));
                slider.CreateAttributes();
                slider.Attributes.Pivot = new System.Drawing.PointF(x, y);
                slider.Attributes.ExpireLayout();
                document.AddObject(slider, false);
                ni.AddSource(slider);
            }
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddCurveParameter("C0", "C0", "Polylines connecting planes ", GH_ParamAccess.tree);
            pManager.AddCurveParameter("C1", "C1", "Polylines connecting planes ", GH_ParamAccess.tree);
            pManager.AddPlaneParameter("P", "Planes", "Planes", GH_ParamAccess.tree);
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);

            //pManager.AddTextParameter("ToolPath", "T", "ToolPath For Unity", GH_ParamAccess.tree);
            //pManager.AddTextParameter("MoveTypes", "M", "Move Types ", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            try
            {
                #region
                var cutPolyline = new List<Polyline>();
                var cutPolyline_ = new List<Curve>();
                DA.GetDataList(0, cutPolyline_);

                var flags = new List<bool>();

                int nn = 0;
                foreach (var o in cutPolyline_)
                {
                    o.TryGetPolyline(out Polyline o_);

                    Plane orientationPlane = o_.plane();

                    if (o_.ClosestPoint((orientationPlane.Origin + orientationPlane.Normal)).DistanceToSquared((orientationPlane.Origin + orientationPlane.Normal)) >
                         o_.ClosestPoint((orientationPlane.Origin - orientationPlane.Normal)).DistanceToSquared((orientationPlane.Origin - orientationPlane.Normal))
                        )
                    {
                        flags.Add(true);
                        o_.Reverse();
                        //o_= o_.ShiftPline(2);
                    }
                    else
                    {
                        flags.Add(false);
                    }

                    cutPolyline.Add(o_);
                    nn++;
                }

                var normal = new List<Polyline>();
                var normal_ = new List<Curve>();
                DA.GetDataList(1, normal_);

                nn = 0;
                foreach (var o in normal_)
                {
                    o.TryGetPolyline(out Polyline o_);

                    if (flags[nn])
                    {
                        o_.Reverse();
                        //o_ = o_.ShiftPline(2);
                    }

                    normal.Add(o_);
                    nn++;
                }

                var cutType_ = new List<double>();
                DA.GetDataList(2, cutType_);
                var cutType = new List<CutType>();
                foreach (var o in cutType_)
                    cutType.Add((CutType)o);

                Plane refPlane = Plane.WorldXY;
                // DA.GetData(1, ref refPlane);

                List<double> angles = new List<double>();
                //DA.GetDataList(2, angles);

                DA.GetData(3, ref this.toolr);

                DA.GetData("Zsec", ref base.Zsec);
                DA.GetData("Speed", ref base.XYfeed);
                DA.GetData("Retreate", ref base.Retreat);

                DA.GetData("ToolID", ref base.toolID);

                this.toolr = (this.toolr == 0) ? Raccoon.GCode.Tool.tools[(int)toolID].radius : this.toolr;

                double heightDivisions = 9;
                DA.GetData(4, ref heightDivisions);

                double ThicknessDivisions = 1;
                DA.GetData(5, ref ThicknessDivisions);
                ThicknessDivisions += ThicknessDivisions % 2;

                double cut90Degrees = 0;
                DA.GetData(6, ref cut90Degrees);

                double retreate = 100;
                DA.GetData(7, ref retreate);

                double retreateZ = 1400;
                DA.GetData(8, ref retreateZ);

                double extendSides = 1;
                DA.GetData(9, ref extendSides);

                string path = @"C:\Unity\20200715\ColabEPFL\ImaxProUnity\Assets\StreamingAssets\MachiningEPFL\test.txt";
                //DA.GetData(10, ref path);

                double speed = 100;
                //DA.GetData(11, ref speed);

                string WObj = "RotatingTool default";
                // DA.GetData(12, ref WObj);

                double rotationDeg = 0;
                //DA.GetData(10, ref rotationDeg);

                Mesh mesh = null;
                //DA.GetData(14, ref mesh);

                var c = new List<Cut>();

                for (int i = 0; i < cutPolyline.Count; i++)
                {
                    Cut cut = new Cut(i, cutPolyline[i], normal[i], cutType[i % cutType.Count], false, new byte[] { 0 }, this.toolr, true, false, false, false, cut90Degrees > 0);
                    c.Add(cut);
                }

                #endregion

                if (c.Count == 0) return;

                List<Cut> cCopy = new List<Cut>(c.Count);
                for (int i = 0; i < c.Count; i++)
                {
                    if (c[i] == null) continue;

                    var cutCopy = c[i].Duplicate();
                    cCopy.Add(cutCopy);
                }

                DataTree<string> commands = new DataTree<string>();
                DataTree<string> MoveTypes = new DataTree<string>();
                var planestree = GetSawBladeToolPath(
                    cCopy, refPlane, angles,
                    this.toolr, heightDivisions, retreate, retreateZ, extendSides, (int)ThicknessDivisions, (int)cut90Degrees,
                    speed, WObj, rotationDeg, mesh,
                    ref commands, ref MoveTypes);

                var plinesTree0 = new Grasshopper.DataTree<Polyline>();
                var plinesTree1 = new Grasshopper.DataTree<Polyline>();
                for (int i = 0; i < planestree.Paths.Count; i++)
                {
                    plinesTree0.Add(planestree.Branch(planestree.Paths[i]).ToPolyline(), planestree.Paths[i]);
                    plinesTree1.Add(planestree.Branch(planestree.Paths[i]).ToPolyline(-1), planestree.Paths[i]);
                }

                DA.SetDataTree(0, plinesTree0);
                DA.SetDataTree(1, plinesTree1);
                DA.SetDataTree(2, planestree);
                //DA.SetDataTree(2, commands);
                //DA.SetDataTree(3, MoveTypes);

                /////////////////////////////////////////////////////////////////////////////////////////////////////
                List<Polyline> polylines = plinesTree0.AllData();
                List<Polyline> normals = plinesTree1.AllData();

                //tools = Raccoon.GCode.Tool.ToolsFromAssembly();
                if (Raccoon.GCode.Tool.tools.ContainsKey((int)toolID))
                {
                    preview.PreviewLines0 = new List<Line>();
                    preview.PreviewLines1 = new List<Line>();
                    preview.PreviewLines2 = new List<Line>();
                    preview.vertices = new PointCloud();
                    GCode = Raccoon.GCode.Cutting.PolylineCutSimple(Raccoon.GCode.Tool.tools[(int)toolID], polylines, ref preview, normals, filename, Zsec, XYfeed, Retreat, 80);
                    Raccoon.GCode.GCodeToGeometry.DrawToolpath(GCode, ref preview);
                    DA.SetDataList(3, GCode);
                }
            }
            catch (Exception e)
            {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
        }

        public DataTree<Plane> GetSawBladeToolPath(
            List<Cut> cuts, Plane refPlane, List<double> angles,
            double doubleRadius, double heightDivisions, double retreate, double retreateZ, double extendSides, int ThicknessDivisions, int cut90Degrees,
            double speed, string WObj, double rotation90Deg, Mesh mesh,
            ref DataTree<string> commands, ref DataTree<string> MoveTypes)
        {
            var tree = new DataTree<Plane>();
            commands = new DataTree<string>();
            MoveTypes = new DataTree<string>();
            int rotations = 8;
            double step = 360.0 / rotations;

            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
            ///Rotation angles, instead of one angle, give always two to rotate the 90 degrees one 0 - 7 and 0 - 7
            ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

            //Plane homePlane = new Plane(new Point3d(1693.93, 65.95, 1749.00), new Vector3d(0.00, 0.00, -1.00));
            //homePlane.Rotate(Math.PI * 0.0, homePlane.ZAxis);

            for (int j = 0; j < cuts.Count; j++)
            {
                if (cuts[j].cutType == CutType.SawBlade || cuts[j].cutType == CutType.Cut || cuts[j].cutType == CutType.SawBladeSlice)
                {
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Create tool-path for milling
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    CutSawBlade cutSawBlade = new CutSawBlade(cuts[j]);//cut can contain multiple paths
                    int thicknessDivisions = cuts[j].cutType == CutType.SawBladeSlice ? 0 : ThicknessDivisions;
                    List<List<Plane>> planes = (new CutSawBlade(cuts[j]) { rotatable = cuts[j].rotatable }).CreateToolPath(doubleRadius, heightDivisions, retreate, retreateZ, thicknessDivisions, Convert.ToInt32(cutSawBlade.SawFlip90Cut), extendSides, false, rotation90Deg);//cutSawBlade.SawFlip90Cut

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Split sawblade path into two steps
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    #region split Toolpaths
                    List<Plane> planes0 = new List<Plane>();// { new Plane(homePlane) };
                    List<Plane> planes1 = new List<Plane>();// { new Plane(homePlane) };
                    var listID = new List<int[]>();

                    int count0 = 0;
                    int count1 = 0;
                    //int count0 = 1;
                    //int count1 = 1;

                    //listID.Add(new int[] { 0, 0 });
                    //listID.Add(new int[] { 0, 1 });

                    for (int k = 0; k < planes.Count; k++)
                    {
                        if (cutSawBlade.SawFlip90Cut)
                        {
                            if (k % 2 == 0)
                            {
                                foreach (var p in planes[k])
                                {
                                    planes0.Add(new Plane(p));
                                    listID.Add(new int[] { 0, count0 });
                                    count0++;
                                }
                            }
                            else
                            {
                                foreach (var p in planes[k])
                                {
                                    planes1.Add(new Plane(p));
                                    listID.Add(new int[] { 1, count1 });
                                    count1++;
                                }
                            }
                        }
                        else
                        {
                            //if (planes0.Count == 0) {
                            //    planes0.Add(new Plane(homePlane));
                            //    listID.Add(new int[] { 0, 0 });
                            //}

                            foreach (var p in planes[k])
                            {
                                planes0.Add(new Plane(p));
                                listID.Add(new int[] { 0, count0 });
                                count0++;
                            }
                        }
                    }

                    //planes0.Add(new Plane(homePlane));
                    //planes1.Add(new Plane(homePlane));
                    //listID.Add(new int[] { 0, count0 });
                    //if (cutSawBlade.SawFlip90Cut)
                    //    listID.Add(new int[] { 1, count1 });
                    List<Plane> planes0Successful = new List<Plane>();
                    List<Plane> planes1Successful = new List<Plane>();
                    #endregion

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Rotate Tool-path incrementally
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    #region first rotation
                    int rotation0 = 0;

                    double z0 = -100;//Most top

                    bool success0 = false;

                    for (int i = 0; i < rotations; i++)
                    {
                        if (i > 0)
                            for (int k = 1; k < planes0.Count - 1; k++)
                            {
                                planes0[k] = planes0[k].XForm(Rhino.Geometry.Transform.Rotation(Rhino.RhinoMath.ToRadians(step), planes0[k].ZAxis, planes0[k].Origin));//Error
                            }

                        //var commandsUnity0 = GetCommandsAndMoveTypes(refPlane, planes0, speed, 10, false, WObj, true);

                        //bool success0_ = RS.RobControllerToolPath.TestToolPath(commandsUnity0.Item1, RS.RobControllerToolPath.SawBladePlane);
                        bool success0_ = true;

                        if (success0_)
                        {
                            double value = planes0[3].XAxis.Z;

                            if (mesh != null)
                            {
                                Point3d cp = mesh.ClosestPoint(planes0[3].Origin);
                                double distance = cp.DistanceTo(planes0[3].Origin + planes0[3].XAxis);
                                value = distance;
                            }

                            if (value > z0)
                            {
                                //Rhino.RhinoApp.WriteLine("RotationA_" + i.ToString() + " Mesh " + (mesh != null).ToString() + " " + value.ToString());
                                z0 = value;
                                success0 = true;

                                planes0Successful.Clear();
                                foreach (var pl in planes0)
                                    planes0Successful.Add(new Plane(pl));
                            }
                            //break;
                        }
                        break;
                    }

                    #endregion

                    #region second rotation
                    //rotation0 = 6;
                    double z1 = -100;
                    bool success1 = true;
                    int rotation1 = 0;

                    if (cutSawBlade.SawFlip90Cut)
                    {
                        //Rhino.RhinoApp.WriteLine("RotationB_Enabled");
                        success1 = false;

                        for (int i = 0; i < rotations; i++)
                        {
                            if (i > 0)
                                for (int k = 1; k < planes1.Count - 1; k++)
                                    planes1[k] = planes1[k].XForm(Rhino.Geometry.Transform.Rotation(Rhino.RhinoMath.ToRadians(step), planes1[k].ZAxis, planes1[k].Origin));//Error

                            //var commandsUnity1 = GetCommandsAndMoveTypes(refPlane, planes1, speed, 10, false, WObj, true);

                            bool success1_ = true;
                            //bool success1_ = RS.RobControllerToolPath.TestToolPath(commandsUnity1.Item1, RS.RobControllerToolPath.SawBladePlane);

                            if (success1_)
                            {
                                double value = planes0[3].XAxis.Z;

                                if (mesh != null)
                                {
                                    Point3d cp = mesh.ClosestPoint(planes1[3].Origin);
                                    double distance = cp.DistanceTo(planes1[3].Origin + planes1[3].XAxis);
                                    value = distance;
                                }

                                if (value > z1)
                                {
                                    // Rhino.RhinoApp.WriteLine("RotationB_" + i.ToString() + " Mesh " + (mesh != null).ToString() + " " + value.ToString());
                                    z1 = value;
                                    success1 = true;

                                    planes1Successful.Clear();
                                    foreach (var pl in planes1)
                                        planes1Successful.Add(new Plane(pl));
                                }
                            }

                            break;
                        }
                    }
                    else
                    {
                        success1 = true;
                    }
                    #endregion

                    //Rhino.RhinoApp.WriteLine("Success " + success0.ToString() + " " + success1.ToString());

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Take correct rotation
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    #region  Take correct rotation
                    if (success0 && success1)
                    {
                        var planesRemapped = new List<List<Plane>>();
                        var planesRemappedTemp = new List<Plane>();

                        for (int k = 0; k < listID.Count; k++)
                        {
                            if (k > 0 && listID[k - 1][0] != listID[k][0])
                            {
                                List<Plane> planeList = new List<Plane>();
                                for (int m = 0; m < planesRemappedTemp.Count; m++)
                                    planeList.Add(new Plane(planesRemappedTemp[m]));
                                planesRemapped.Add(planeList);
                                planesRemappedTemp.Clear();
                            }

                            if (listID[k][0] == 0)
                            {
                                planesRemappedTemp.Add(planes0Successful[listID[k][1]]);
                            }
                            else
                            {
                                planesRemappedTemp.Add(planes1Successful[listID[k][1]]);
                            }
                        }

                        if (planesRemapped.Count > 0)
                        {
                            planesRemapped.Last().AddRange(planesRemappedTemp);
                        }
                        else
                        {
                            planesRemapped.Add(planesRemappedTemp);
                        }

                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        ///Convert to Unity TXT
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        GH_Path path = new GH_Path(j);

                        foreach (var pr in planesRemapped)
                        {
                            //var commandsUnity = GetCommandsAndMoveTypes(refPlane, pr, speed, 10, false, WObj, true);
                            //commands.AddRange(commandsUnity.Item1, path);
                            //MoveTypes.AddRange(commandsUnity.Item2, path);
                            tree.AddRange(pr, path);
                        }
                    }
                    #endregion
                }
            }

            return tree;
        }

        private Tuple<List<string>, List<string>> GetCommandsAndMoveTypes(Plane refPlane, List<Plane> planes, double speed, double smooth, bool AbsJ_L, object WObj, bool Mill)
        {
            var commands = new List<string>(planes.Count());
            var moveTypes = new List<string>(planes.Count());

            Plane planeLast = Plane.Unset;

            for (int i = 0; i < planes.Count(); i++)
            {
                //if(
                bool isEqual = planeLast == planes[i];
                if (isEqual)
                    continue;

                Plane plane = new Plane(planes[i]);

                double[] posRot = InitPlane(refPlane, plane);
                string moveType = (AbsJ_L) ? "MoveAbs " : "MoveL ";
                if (i < 2 || i == planes.Count - 1)
                    moveType = "MoveAbs ";

                moveTypes.Add(moveType);
                int speedAdjusted = (i > 2 && i < planes.Count() - 2) ? (int)(speed * 0.25) : (int)speed;
                string output = string.Format(moveType + WObj + " {0} {1} {2} {3} {4}", speedAdjusted, smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);
                commands.Add(output);
                //Add to last
                planeLast = new Plane(plane);
            }

            if (planes.Count() > 2)
            {
                commands.Insert(1, "Mill 6500");
                commands.Insert(commands.Count() - 2, "Mill 0");
            }

            return Tuple.Create(commands, moveTypes);
        }

        public double[] InitPlane(Plane refPlane, Plane p)
        {
            Rhino.Geometry.Quaternion quaternion = new Quaternion();
            quaternion.SetRotation(refPlane, p);

            double[] transformation = new double[]{
      p.OriginX,
      p.OriginY,
      p.OriginZ,
      quaternion.A,quaternion.B,quaternion.C,quaternion.D
      };

            for (int i = 0; i < transformation.Length; i++)
            {
                if (Math.Abs(transformation[i]) < 0.0001)
                    transformation[i] = 0;
            }
            return transformation;
        }

        protected override void AfterSolveInstance()
        {
            GH_Document ghdoc = base.OnPingDocument();
            for (int i = 0; i < ghdoc.ObjectCount; i++)
            {
                IGH_DocumentObject obj = ghdoc.Objects[i];
                if (obj.Attributes.DocObject.ToString().Equals("Grasshopper.Kernel.Special.GH_Group"))
                {
                    Grasshopper.Kernel.Special.GH_Group groupp = (Grasshopper.Kernel.Special.GH_Group)obj;
                    if (groupp.ObjectIDs.Contains(this.InstanceGuid))
                        return;
                }
            }

            List<Guid> guids = new List<Guid>() { this.InstanceGuid };

            foreach (var param in base.Params.Input)
                foreach (IGH_Param source in param.Sources)
                    guids.Add(source.InstanceGuid);

            Grasshopper.Kernel.Special.GH_Group g = new Grasshopper.Kernel.Special.GH_Group();
            g.NickName = base.Name.ToString();

            g.Colour = System.Drawing.Color.FromArgb(255, 0, 255, 150);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.Saw;
            }
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("73d2e86f-e1a5-4cd9-82c1-60e2f6bcf151"); }
        }
    }
}