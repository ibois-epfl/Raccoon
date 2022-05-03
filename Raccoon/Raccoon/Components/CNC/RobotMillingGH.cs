using System;
using System.Collections.Generic;
using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Raccoon;
using Rhino.Geometry;

namespace Raccoon
{
    public class MillingGH : CustomComponent
    {
        public override GH_Exposure Exposure => GH_Exposure.primary;

        public MillingGH()
          : base("Milling", "Milling",
              "Milling",
              "Robot/CNC")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("cut_polyline", " cut_polyline", "cut_polyline", GH_ParamAccess.list);
            pManager.AddCurveParameter("dir_polyline", " dir_polyline", "dir_polyline", GH_ParamAccess.list);
            pManager.AddNumberParameter("cut_type", "cut_type", "Cut = 0, Mill = 1, Drill = 2, SawBlade = 3,  \n SawBladeBisector = 4, Engrave = 5, MillPath = 6, MillCircular = 7,  \n SawCircular = 8, SawEnd = 9, Slice = 10, SawBladeSlice = 11,  OpenCut = 12", GH_ParamAccess.list);
            pManager.AddNumberParameter("notches_types", "notches_types", "1-bisector 2-translation 3-translation opposite 4-4 rounded outline", GH_ParamAccess.list);

            pManager.AddNumberParameter("Radius", "Radius", "Milling radius", GH_ParamAccess.item);
            pManager.AddNumberParameter("Height/Input", "Height/Input", "Height/Input", GH_ParamAccess.item);

            pManager.AddNumberParameter("ToolID", "ToolID", "Tool ID Number in the CNC machine", GH_ParamAccess.item);//0
            pManager.AddNumberParameter("Speed", "Speed", "velocity of horizontal cutting in mm/min", GH_ParamAccess.item);//5000
            pManager.AddNumberParameter("Zsec", "Zsec", "Safe Plane over workpiece.Program begins and starts at this Z-height", GH_ParamAccess.item);//700
            pManager.AddNumberParameter("Retreate", "Retreate", "Height of the XY Plane for tool retreat", GH_ParamAccess.item);//70
            pManager.AddNumberParameter("Angle", "Angle", "Rotate Angle to avoid curved movement", GH_ParamAccess.item);//70
        }

        //Inputs
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

            double[] sliderValue = new double[] { 12, 2, 0, 10, 42, 20000, 750, 50, 80 };
            double[] sliderMinValue = new double[] { 0, 0, 0, 1, 0, 10000, 0, 0, 0 };
            double[] sliderMaxValue = new double[] { 12, 4, 100, 10, 150, 40000, 800, 800, 360 };
            int[] sliderID = new int[] { 2, 3, 4, 5, 6, 7, 8, 9, 10 };
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
            pManager.AddCurveParameter("Pline", "C0", "Polylines connecting planes ", GH_ParamAccess.tree);
            pManager.AddCurveParameter("Pline", "C1", "Polylines connecting planes ", GH_ParamAccess.tree);
            // pManager.AddTextParameter("ToolPath", "T", "ToolPath For Unity", GH_ParamAccess.tree);
            pManager.AddPlaneParameter("Plane", "P", "Planes", GH_ParamAccess.tree);
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);
            //pManager.AddTextParameter("MoveTypes", "M", "Move Types ", GH_ParamAccess.tree);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            var cutPolyline = new List<Polyline>();
            var cutPolyline_ = new List<Curve>();
            DA.GetDataList(0, cutPolyline_);

            foreach (var o in cutPolyline_)
            {
                o.TryGetPolyline(out Polyline o_);
                o_.Reverse();
                cutPolyline.Add(o_);
            }

            var normal = new List<Polyline>();
            var normal_ = new List<Curve>();
            DA.GetDataList(1, normal_);

            foreach (var o in normal_)
            {
                o.TryGetPolyline(out Polyline o_);
                o_.Reverse();
                normal.Add(o_);
            }

            var cutType_ = new List<double>();
            DA.GetDataList(2, cutType_);
            var cutType = new List<CutType>();
            foreach (var o in cutType_)
                cutType.Add((CutType)o);

            var notchesTypes_ = new List<double>();
            DA.GetDataList(3, notchesTypes_);
            byte[] notchesTypes = new byte[notchesTypes_.Count];//1-bisector 2-translation 3-translation opposite 4-4 rounded outline
            for (int i = 0; i < notchesTypes_.Count; i++)
                notchesTypes[i] = Convert.ToByte((int)notchesTypes_[i]);

            double filletR = 0;
            bool project = false;
            bool CutOrHole = true;//not cutting
            bool PolylineMergeTakeOutside = false;//not cutting
            bool merge = false;//not cutting
            bool projectRotate = false;
            bool SawFlip90Cut = false;
            double angle = 5;
            DA.GetData(10, ref angle);

            var c = new List<Cut>();

            for (int i = 0; i < cutPolyline.Count; i++)
            {
                Cut cut = new Cut(i, cutPolyline[i], normal[i], cutType[i % cutType.Count], project, new byte[] { notchesTypes[i % notchesTypes.Length] }, filletR, CutOrHole, PolylineMergeTakeOutside, merge, projectRotate, SawFlip90Cut);
                c.Add(cut);
            }

            try
            {
                #region input

                Plane refPlane = Plane.WorldXY;
                //List<double> angles = new List<double>();
                //DA.GetDataList(4, angles);

                double Radius = 0;
                DA.GetData(4, ref this.toolr);

                DA.GetData("Zsec", ref base.Zsec);
                DA.GetData("Speed", ref base.XYfeed);
                DA.GetData("Retreate", ref base.Retreat);

                DA.GetData("ToolID", ref base.toolID);
                this.toolr = (this.toolr == 0) ? Raccoon.GCode.Tool.tools[(int)toolID].radius : this.toolr;

                double heightDivisions = 5;
                DA.GetData(5, ref heightDivisions);

                double retreate = 100;
                double retreateZ = 1400;
                bool PlanarOffset = true;
                bool Sort = false;
                bool Soft = true;
                bool Notch = false;
                bool CutOnly = false;
                bool perpendicularToSurface = true;
                string path = @"C:\Unity\20200715\ColabEPFL\ImaxProUnity\Assets\StreamingAssets\MachiningEPFL\test.txt";
                double speed = 100;
                string WObj = "RotatingTool default";
                Mesh referenceMesh = null;

                #endregion input

                DataTree<Polyline> plinesTree0 = new DataTree<Polyline>();
                DataTree<Polyline> plinesTree1 = new DataTree<Polyline>();
                if (c.Count == 0) return;

                List<Cut> cCopy = new List<Cut>(c.Count);
                for (int i = 0; i < c.Count; i++)
                {
                    if (c[i] == null) continue;
                    var cutCopy = c[i].Duplicate();
                    //if (!Notch)
                    //    cutCopy.notches = Notch;
                    cCopy.Add(cutCopy);
                }

                DataTree<Plane> planestree = new DataTree<Plane>();
                DataTree<string> commands = new DataTree<string>();
                DataTree<string> MoveTypes = new DataTree<string>();

                if (true)
                {
                    planestree = GetMillingToolPath(cCopy, refPlane, new List<double>() { 0 }, this.toolr, heightDivisions, retreate, retreateZ, PlanarOffset, Sort, Soft, Notch, CutOnly, perpendicularToSurface, speed, WObj, ref commands, ref MoveTypes);

                    for (int i = 0; i < planestree.Paths.Count; i++)
                    {
                        plinesTree0.Add(planestree.Branch(planestree.Paths[i]).ToPolyline(), planestree.Paths[i]);
                        plinesTree1.Add(planestree.Branch(planestree.Paths[i]).ToPolyline(-1), planestree.Paths[i]);
                    }
                }
                else
                {
                    double step = 360.0 / 8.0;
                    int successfulPaths = 0;
                    for (int i = 0; i < cCopy.Count; i++)
                    {
                        double z0 = -100;
                        bool success = false;
                        int rotation0 = 0;

                        for (int j = 0; j < 8; j++)
                        {
                            DataTree<string> MoveTypes_ = new DataTree<string>();
                            DataTree<string> commands_ = new DataTree<string>();
                            DataTree<Plane> planestree_ = GetMillingToolPath(new List<Cut> { cCopy[i] }, refPlane, new List<double> { j }, Radius, heightDivisions, retreate, retreateZ, PlanarOffset, Sort, Soft, Notch, CutOnly, perpendicularToSurface, speed, WObj, ref commands_, ref MoveTypes_);

                            success = true;
                            //if(referenceMesh != null)
                            //    success = RS.RobControllerToolPath.TestToolPath(commands_.AllData(), RS.RobControllerToolPath.MillPlane, referenceMesh);
                            //else
                            //    success = RS.RobControllerToolPath.TestToolPath(commands_.AllData(), RS.RobControllerToolPath.MillPlane);

                            if (success)
                            {
                                rotation0 = i;
                                //Rhino.RhinoApp.WriteLine("RotationMilling_" + i.ToString() + "_" + j.ToString());

                                if (referenceMesh != null)
                                {
                                    Point3d pointMovedByXAxis = planestree_.Branches[0][1].Origin + planestree_.Branches[0][1].XAxis;
                                    Point3d cpOrigin = referenceMesh.ClosestPoint(planestree_.Branches[0][1].Origin);
                                    double dist = cpOrigin.DistanceToSquared(pointMovedByXAxis);
                                    if (dist > z0)
                                    {
                                        z0 = dist;

                                        if (planestree.PathExists(new GH_Path(i)))
                                        {
                                            planestree.Branch(new GH_Path(i)).Clear();
                                            //commands.Branch(new GH_Path(i)).Clear();
                                            //MoveTypes.Branch(new GH_Path(i)).Clear();
                                            plinesTree0.Branch(new GH_Path(i)).Clear();
                                            plinesTree1.Branch(new GH_Path(i)).Clear();
                                        }
                                        planestree.AddRange(planestree_.AllData(), new GH_Path(i));
                                        //commands.AddRange(commands_.AllData(), new GH_Path(i));
                                        //MoveTypes.AddRange(MoveTypes_.AllData(), new GH_Path(i));
                                        for (int k = 0; k < planestree_.Paths.Count; k++)
                                        {
                                            plinesTree0.Add(planestree_.Branch(planestree_.Paths[k]).ToPolyline(), new GH_Path(i));
                                            plinesTree1.Add(planestree_.Branch(planestree_.Paths[k]).ToPolyline(-1), new GH_Path(i));
                                        }
                                    }
                                }
                                else
                                {
                                    planestree.AddRange(planestree_.AllData(), new GH_Path(i));
                                    //commands.AddRange(commands_.AllData(), new GH_Path(i));
                                    //MoveTypes.AddRange(MoveTypes_.AllData(), new GH_Path(i));
                                    for (int k = 0; k < planestree_.Paths.Count; k++)
                                    {
                                        plinesTree0.Add(planestree_.Branch(planestree_.Paths[k]).ToPolyline(), new GH_Path(i));
                                        plinesTree1.Add(planestree_.Branch(planestree_.Paths[k]).ToPolyline(-1), new GH_Path(i));
                                    }
                                    break;
                                }
                                successfulPaths++;
                            }
                        }
                    }

                    if (successfulPaths > 0)
                        Rhino.RhinoApp.WriteLine("RotationMilling_True");
                    else
                        Rhino.RhinoApp.WriteLine("RotationMilling_False");
                }

                #region output

                DA.SetDataTree(0, plinesTree0);
                DA.SetDataTree(1, plinesTree1);
                //DA.SetDataTree(2, commands);
                DA.SetDataTree(2, planestree);
                //DA.SetDataTree(4, MoveTypes);

                #endregion output

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
                    GCode = Raccoon.GCode.Cutting.PolylineCutSimple(Raccoon.GCode.Tool.tools[(int)toolID], polylines, ref preview, normals, filename, Zsec, XYfeed, Retreat, angle);
                    Raccoon.GCode.GCodeToGeometry.DrawToolpath(GCode, ref preview);

                    DA.SetDataList(3, GCode);
                }
            }
            catch (Exception e)
            {
                Rhino.RhinoApp.WriteLine(e.ToString());
            }
        }

        public DataTree<Plane> GetMillingToolPath(List<Cut> cuts, Plane refPlane, List<double> angles, double doubleRadius, double heightDivisions, double retreate, double retreateZ, bool PlanarOffset, bool Sort,
    bool Soft, bool Notch, bool CutOnly, bool perpendicularToSurface, double speed, string WObj, ref DataTree<string> commands, ref DataTree<string> MoveTypes)
        {
            var tree = new DataTree<Plane>();
            //commands = new DataTree<string>();
            //MoveTypes = new DataTree<string>();

            double angle = 0;
            for (int j = 0; j < cuts.Count; j++)
            {
                if (cuts[j].cutType == CutType.OpenCut || cuts[j].cutType == CutType.Mill || cuts[j].cutType == CutType.MillPath || cuts[j].cutType == CutType.Cut || cuts[j].cutType == CutType.Slice)
                {
                    CutMill cutMill = new CutMill(cuts[j]);//cut can contain multiple paths
                    if (cuts[j].cutType == CutType.MillPath)
                        cutMill.MillOrCut = false;

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Create tool-path for milling
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    List<List<Plane>> planes = cutMill.CreateToolPath(doubleRadius, heightDivisions, retreate, retreateZ, PlanarOffset, Sort, Soft, cuts[j].notches, cuts[j].cutType, perpendicularToSurface, cuts[j].notchesTypes);

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Rotation angles
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    if (cuts.Count == angles.Count)
                        angle = angles[j];

                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    ///Iterate list of list planes
                    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                    for (int k = 0; k < planes.Count; k++)
                    {
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        ///Rotate Tool-path
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        double step = 360.0 / 8.0;
                        for (int l = 0; l < planes[k].Count; l++)
                        {
                            if (l > 0 && l < (planes[k].Count - 1))
                            {
                                planes[k][l] = planes[k][l].XForm(Rhino.Geometry.Transform.Rotation(Rhino.RhinoMath.ToRadians(step * angle), planes[k][l].ZAxis, planes[k][l].Origin));//Error
                            }
                        }

                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        ///Convert to Unity TXT
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        //var commandsUnity = GetCommandsAndMoveTypes(refPlane, planes[k], speed, 10, false, WObj, true);

                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        ///Output
                        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                        GH_Path path = new GH_Path(j, k);
                        //commands.AddRange(commandsUnity.Item1, path);
                        //MoveTypes.AddRange(commandsUnity.Item2, path);
                        //planes[k] = commandsUnity.Item3;
                        planes[k] = planes[k];
                        tree.AddRange(planes[k], path);
                    }
                }
            }

            return tree;
        }

        //This function produces code for running robot MoveL and MoveAbs
        private Tuple<List<string>, List<string>, List<Plane>> GetCommandsAndMoveTypes(Plane refPlane, List<Plane> planes, double speed, double smooth, bool AbsJ_L, object WObj, bool Mill)
        {
            var commands = new List<string>(planes.Count);
            var moveTypes = new List<string>(planes.Count);
            var planesWithoutDuplicates = new List<Plane>(planes.Count);

            Plane planeLast = Plane.Unset;
            for (int i = 0; i < planes.Count; i++)
            {
                bool isEqual = planeLast == planes[i];
                if (isEqual)
                    continue;

                planesWithoutDuplicates.Add(planes[i]);

                if (i == 1 && Mill)
                    commands.Add("Mill 20000");

                Plane plane = new Plane(planes[i]);

                double[] posRot = InitPlane(refPlane, plane);
                string moveType = (AbsJ_L) ? "MoveAbs " : "MoveL ";
                if (i < 3 || i == planes.Count - 1)
                    moveType = "MoveAbs ";

                string moveTypeFlag = (moveType == "MoveAbs ") ? "1" : "0";
                moveTypes.Add(moveTypeFlag);

                int speedAdjusted = (i > 2 && i < planes.Count - 2) ? (int)(speed * 0.25) : (int)speed;

                string output = string.Format(moveType + WObj + " {0} {1} {2} {3} {4}", speedAdjusted, smooth, posRot[0], posRot[1], posRot[2]) + " Quaternion " + string.Format("{0} {1} {2} {3}", posRot[3], posRot[4], posRot[5], posRot[6]);

                commands.Add(output);

                if (i == planes.Count - 2 && Mill)
                    commands.Add("Mill 0");

                //Add to last
                planeLast = new Plane(plane);
            }

            return Tuple.Create(commands, moveTypes, planesWithoutDuplicates);
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

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.Mill;
            }
            //get {
            //    string nickName = base.NickName;
            //    System.Drawing.Bitmap bitmap = new System.Drawing.Bitmap(24, 24);
            //    using (System.Drawing.Graphics graphic = System.Drawing.Graphics.FromImage(bitmap)) {
            //        graphic.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
            //        graphic.DrawString(nickName, new System.Drawing.Font(System.Drawing.FontFamily.GenericSansSerif, 7f, System.Drawing.GraphicsUnit.Pixel), System.Drawing.Brushes.Black, new System.Drawing.RectangleF(-0f, -0f, 24f, 24f));
            //    }
            //    return bitmap;
            //}
        }

        public override Guid ComponentGuid
        {
            get { return new Guid("73d2e86f-e1a5-4cd9-82c1-11e2f6bcf844"); }
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

            g.Colour = System.Drawing.Color.FromArgb(255, 0, 255, 200);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }
    }
}