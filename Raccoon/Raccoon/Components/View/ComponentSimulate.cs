using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Text;
using Grasshopper.Kernel;
using Rhino.Geometry;

namespace Raccoon.Components.View
{
    public class ComponentSimulate : GH_Component
    {
        private bool run_once = true;

        public ComponentSimulate()
          : base("Simulate", "Simulate",
              "Convert GCode to simulation", "Raccoon", "View")
        {
        }

        private List<Mesh> maka3D = new List<Mesh>();
        public Raccoon.PreviewObject preview;
        public BoundingBox bbox = Utilities.MakaDimensions.MakaBBox();
        public Rhino.Display.DisplayMaterial m = new Rhino.Display.DisplayMaterial(Color.White);
        public Rhino.Display.DisplayMaterial m_red = new Rhino.Display.DisplayMaterial(Color.Red);
        public override BoundingBox ClippingBox => bbox;
        private bool collision = false;

        public override void DrawViewportMeshes(IGH_PreviewArgs args)
        {
            if (this.Hidden || this.Locked) return;
            foreach (var b in maka3D)
            {
                if (!this.collision)
                    args.Display.DrawMeshShaded(b, this.m);
                else
                    args.Display.DrawMeshShaded(b, this.m_red);
            }
        }

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            if (this.Hidden || this.Locked) return;
            //Travelling path
            if (preview.PreviewLines0 != null)
            {
                args.Display.DrawLines(preview.PreviewLines0, Color.Orange, 2);
                //args.Display.DrawArrows(lines, Color.MediumVioletRed);
            }
            //drilling path
            if (preview.PreviewLines1 != null)
                args.Display.DrawLines(preview.PreviewLines1, Color.Orange, 1);
            //Works only with an angle
            if (preview.PreviewLines2 != null)
                args.Display.DrawLines(preview.PreviewLines2, Color.Black, 3);

            if (preview.PreviewPolyline != null)

                args.Display.DrawPolyline(preview.PreviewPolyline, Color.MediumVioletRed, 2);
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);
            pManager.AddNumberParameter("Position", "t", "Position", GH_ParamAccess.item);
            //pManager.AddNumberParameter("Rtool", "Tool Radius", "Rtool", GH_ParamAccess.item);
            //pManager.AddNumberParameter("Ltool", "Tool Length", "Ltool", GH_ParamAccess.item);
            pManager.AddMeshParameter("Mesh", "Mesh", "Mesh", GH_ParamAccess.list);
            pManager.AddIntegerParameter("n", "n", "Iterations to check collission", GH_ParamAccess.item);
            pManager.AddBooleanParameter("DetectTool", "DetectTool", "DetectTool while cheking collission", GH_ParamAccess.item);
            /*
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list, "0");
            pManager.AddNumberParameter("Position", "P", "Position", 0, 0.5);
            pManager.AddNumberParameter("Rtool", "R", "Rtool", 0, 10);
            pManager.AddNumberParameter("Ltool", "L", "Ltool", 0, 150);
            pManager.AddMeshParameter("Collision", "Collision", "Collision", GH_ParamAccess.list);
            */

            pManager[1].Optional = true;
            pManager[2].Optional = true;
            pManager[3].Optional = true;
            pManager[4].Optional = true;
        }

        //Inputs
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add String
            //Grasshopper.Kernel.Parameters.Param_String si = Params.Input[0] as Grasshopper.Kernel.Parameters.Param_String;
            //if (si == null || si.SourceCount > 0 || si.PersistentDataCount > 0) return;
            //Attributes.PerformLayout();
            //int xs = (int)si.Attributes.Pivot.X - 225;
            //int ys = (int)si.Attributes.Pivot.Y;
            //IGH_Param text = new Grasshopper.Kernel.Parameters.Param_String();
            //List<string> gcode = IBOIS.GCode.GCodeToGeometry.defaultToolPath;
            //for (int i = 0; i < gcode.Count; i++)
            //    text.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), i, gcode[i]);
            //text.CreateAttributes();
            //text.Attributes.Pivot = new PointF(xs, ys);
            //text.Attributes.ExpireLayout();
            //document.AddObject(text, false);
            //si.AddSource(text);

            //Add sliders
            double[] sliderValue = new double[] { 0.500001 };
            double[] sliderMinValue = new double[] { 0 };
            double[] sliderMaxValue = new double[] { 1 };
            int[] sliderID = new int[] { 1 };

            for (int i = 0; i < sliderValue.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Number ni = Params.Input[sliderID[i]] as Grasshopper.Kernel.Parameters.Param_Number;
                if (ni == null || ni.SourceCount > 0 || ni.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ni.Attributes.Pivot.X - 50;
                int y = (int)ni.Attributes.Pivot.Y - 55;

                Grasshopper.Kernel.Special.GH_NumberSlider slider = new Grasshopper.Kernel.Special.GH_NumberSlider();
                slider.SetInitCode(string.Format("{0}<{1}<{2}", sliderMinValue[i], sliderValue[i], sliderMaxValue[i]));
                slider.CreateAttributes();
                slider.Attributes.Pivot = new PointF(x, y);
                slider.Attributes.ExpireLayout();
                document.AddObject(slider, false);
                ni.AddSource(slider);
            }
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddMeshParameter("tool", "tool", "tool", GH_ParamAccess.list);
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.item);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            List<string> GCode = new List<string>();
            double position = Math.Min(1, Math.Max(0, DA.Fetch<double>("Position")));
            List<Mesh> meshes = DA.FetchList<Mesh>("Mesh");
            int it = DA.Fetch<int>("n");
            bool toolDetect = DA.Fetch<bool>("DetectTool");
            int detectTool = (toolDetect) ? 0 : 1;

            Point3d tablecenter = new Point3d(750, 1250, -25);// table
            Box table = new Box(new Plane(tablecenter, Plane.WorldXY.Normal, Plane.WorldXY.XAxis), new Interval(-25, 25), new Interval(-750, 750), new Interval(-1250, 1250));
            meshes.Add(Mesh.CreateFromBox(table, 1, 1, 1));

            //try
            //{
            if (DA.GetDataList(0, GCode))
            {
            }

            //Animation
            var animationValues = Raccoon.GCode.GCodeToGeometry.CNCAnim(GCode, position);
            var previewGCode = Raccoon.GCode.GCodeToGeometry.FromValuesToGeometry(animationValues);

            //Preview Maka3D
            maka3D = previewGCode.Item1;
            foreach (var b in maka3D)
                this.bbox.Union(b.GetBoundingBox(false));

            //Preview Tool-path
            preview = new PreviewObject();
            preview.PreviewLines0 = new List<Line>();
            preview.PreviewLines1 = new List<Line>();
            preview.PreviewLines2 = new List<Line>();
            preview.PreviewPolyline = new Polyline();
            preview.vertices = new PointCloud();

            int id = (int)Math.Ceiling(position * (GCode.Count - 1));
            Raccoon.GCode.GCodeToGeometry.DrawToolpath(GCode, ref preview, position);

            //Message
            string[] messages = previewGCode.Item3.Split(new char[] { ' ' });
            base.Message = "";
            string message = "";
            foreach (string s in messages)
                message += (s + "\n ");
            base.Message = message;

            //Collision detection
            collision = false;
            for (int i = 0; i < meshes.Count; i++)
            {
                if (meshes[i] == null) continue;

                for (int j = 0; j < maka3D.Count - detectTool; j++)
                {
                    Line[] lines = Rhino.Geometry.Intersect.Intersection.MeshMeshFast(meshes[i], maka3D[j]);
                    if (lines.Length > 0)
                    {
                        this.collision = true;
                        base.Message += "\n Collision";
                        break;
                    }
                    //else
                    //{
                    //    this.collision = false;
                    //}
                    if (this.collision)
                        break;
                }
            }

            //Output
            DA.SetData(1, previewGCode.Item3);
            DA.SetDataList(0, maka3D);

            for (int k = 0; k < it; k++)
            {
                double t = k * 1.0 / it;

                //Animation
                var animationValues_ = Raccoon.GCode.GCodeToGeometry.CNCAnim(GCode, t);
                var previewGCode_ = Raccoon.GCode.GCodeToGeometry.FromValuesToGeometry(animationValues_);

                ///Rhino.RhinoApp.WriteLine(t.ToString());

                //Collision detection
                bool collision_ = false;
                for (int i = 0; i < previewGCode_.Item1.Count; i++)
                {
                    if (meshes[i] == null) continue;
                    for (int j = 0; j < maka3D.Count - detectTool; j++)
                    {
                        //Rhino.RhinoApp.WriteLine(t.ToString());
                        Line[] lines = Rhino.Geometry.Intersect.Intersection.MeshMeshFast(meshes[i], previewGCode_.Item1[j]);
                        if (lines.Length > 0)
                        {
                            base.Message = ("Collission " + t.ToString());
                            collision_ = true;
                            break;
                        }

                        if (collision_)
                            break;
                    }
                }
                if (collision_)
                    break;
            }
            //} catch (Exception e) {
            // Rhino.RhinoApp.WriteLine(e.ToString());
            //}
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.simulation;
            }
        }

        internal static Bitmap GetIcon(GH_ActiveObject comp)
        {
            string nickName = comp.NickName;
            Bitmap bitmap = new Bitmap(24, 24);
            using (Graphics graphic = Graphics.FromImage(bitmap))
            {
                graphic.TextRenderingHint = TextRenderingHint.AntiAlias;
                graphic.DrawString(nickName, new Font(FontFamily.GenericSansSerif, 6f), Brushes.Black, new RectangleF(-1f, -1f, 26f, 26f));
            }
            return bitmap;
        }

        public override Guid ComponentGuid => new Guid("aa6739cc-5ce6-46a0-abd6-45df31f53975");

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

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 255);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }
    }
}