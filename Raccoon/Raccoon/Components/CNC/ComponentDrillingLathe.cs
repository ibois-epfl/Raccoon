using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Parameters;
using Rhino.Geometry;

namespace Raccoon.Components
{
    public class ComponentDrillingLathe : CustomComponent
    {
        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.straightdrill;
            }
        }

        public override GH_Exposure Exposure => GH_Exposure.secondary;

        public override void DrawViewportWires(IGH_PreviewArgs args)
        {
            if (this.Hidden || this.Locked) return;
            //Travelling path
            if (preview.PreviewLines0 != null)
            {
                args.Display.DrawLines(preview.PreviewLines0, Color.Orange, 1);
                args.Display.DrawArrows(lines, Color.MediumVioletRed);
            }
            //drilling path
            if (preview.PreviewLines1 != null)
                args.Display.DrawLines(preview.PreviewLines1, Color.DarkOrange, 2);
            //Works only with an angle
            if (preview.PreviewLines2 != null)
                args.Display.DrawLines(preview.PreviewLines2, Color.DarkBlue, 5);
        }

        public override void DrawViewportMeshes(IGH_PreviewArgs args)
        {
        }

        public ComponentDrillingLathe()
          : base("Drilling Lathe", "Drill Holes Lathe",
              "For drilling holes either 2D and 3D using BAxis in continuous rotation",
              "Robot/CNC")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Lines", "Lines", "Lines Pointing into direction of cutting, end point of line is end point of drilling and line is insertion vector", GH_ParamAccess.list);

            pManager.AddNumberParameter("ToolID", "ToolID", "Tool ID Number in the CNC machine", GH_ParamAccess.item);//0
            pManager.AddNumberParameter("Speed", "Speed", "velocity of horizontal cutting in mm/min", GH_ParamAccess.item);//5000
            pManager.AddNumberParameter("Zsec", "Zsec", "Safe Plane over workpiece.Program begins and starts at this Z-height", GH_ParamAccess.item);//700
            pManager.AddNumberParameter("Retreate", "Retreate", "Height of the XY Plane for tool retreat", GH_ParamAccess.item);//70
            pManager.AddNumberParameter("Infeed", "Infeed", "Interpolation between normal and path curve", GH_ParamAccess.item);//1

            pManager.AddBooleanParameter("Vertical", "Vertical", "Vertical", GH_ParamAccess.item); // true

            for (int i = 0; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
        }

        //Inputs
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add Curve

            Curve[] lines = new Curve[] {
                (new Line(new Point3d(200,-200,300),new Point3d(200,-200,400))).ToNurbsCurve()
                 };

            int[] recID = new int[] { 0 };

            for (int i = 0; i < recID.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Curve ri = Params.Input[recID[i]] as Grasshopper.Kernel.Parameters.Param_Curve;
                if (ri == null || ri.SourceCount > 0 || ri.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ri.Attributes.Pivot.X - 225;
                int y = (int)ri.Attributes.Pivot.Y;
                IGH_Param lineParam = new Grasshopper.Kernel.Parameters.Param_Curve();
                lineParam.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 0, lines[i]);
                lineParam.CreateAttributes();
                lineParam.Attributes.Pivot = new System.Drawing.PointF(x, y);
                lineParam.Attributes.ExpireLayout();
                document.AddObject(lineParam, false);
                ri.AddSource(lineParam);
            }

            //Add sliders
            double[] sliderValue = new double[] { 42, 20000, 750, 300, 1, };
            double[] sliderMinValue = new double[] { 1, 1000, 0, 300, 1, };
            double[] sliderMaxValue = new double[] { 110, 30000, 750, 0, 20 };
            int[] sliderID = new int[] { 1, 2, 3, 4, 5 };

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

            //Add Booleans
            bool[] boolValue = new bool[] { true };
            int[] boolID = new int[] { 6 };

            for (int i = 0; i < boolID.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Boolean bi = Params.Input[boolID[i]] as Grasshopper.Kernel.Parameters.Param_Boolean;
                if (bi == null || bi.SourceCount > 0 || bi.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)bi.Attributes.Pivot.X - 250;
                int y = (int)bi.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_BooleanToggle booleanToggle = new Grasshopper.Kernel.Special.GH_BooleanToggle();
                booleanToggle.CreateAttributes();
                booleanToggle.Attributes.Pivot = new PointF(x, y);
                booleanToggle.Attributes.ExpireLayout();
                booleanToggle.Value = boolValue[i];
                document.AddObject(booleanToggle, false);
                bi.AddSource(booleanToggle);
            }
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            pManager.AddTextParameter("Info", "Info", "Info", GH_ParamAccess.item);
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            lines = new List<Line>();
            List<Curve> curves = new List<Curve>();

            DA.GetData("ToolID", ref base.toolID);
            DA.GetData("Speed", ref base.XYfeed);
            DA.GetData("Zsec", ref base.Zsec);
            DA.GetData("Retreate", ref base.Retreat);
            DA.GetData("Infeed", ref base.infeed);

            bool vertical = DA.Fetch<bool>("Vertical");

            GCode = new List<string>();
            if (DA.GetDataList(0, curves))
            {
                foreach (Curve c in curves)
                {
                    if (c.IsValid)
                        lines.Add(new Line(c.PointAtStart, c.PointAtEnd));
                }

                try
                {
                    //Check if curves below zero
                    this.badCurves = Utilities.GeometryProcessing.IsCurvesBelowZero(lines);

                    if (badCurves.Count > 0)
                        this.AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "Geometry is below Zero");

                    lines = lines.OrderByDescending(l => l.From.X).Reverse().ToList();
                    List<Line> linesOrdered = new List<Line>();

                    if (vertical)
                    {
                        foreach (Line l in lines)
                            if (l.FromZ < l.ToZ)
                            {
                                linesOrdered.Add(new Line(l.To, l.From));
                            }
                            else
                            {
                                linesOrdered.Add(new Line(l.From, l.To));
                            }
                    }
                    else
                    {
                        linesOrdered = new List<Line>(lines);
                    }

                    //tools = Raccoon.GCode.Tool.ToolsFromAssembly();
                    if (Raccoon.GCode.Tool.tools.ContainsKey((int)toolID))
                    {
                        this.preview = new PreviewObject();
                        preview.PreviewLines0 = new List<Line>();
                        preview.PreviewLines1 = new List<Line>();
                        preview.PreviewLines2 = new List<Line>();
                        lines = linesOrdered;
                        preview.vertices = new PointCloud();
                        //Rhino.RhinoApp.WriteLine(lines.Count.ToString());
                        GCode = Raccoon.GCode.Cutting.CNC5X3DDrillLathe(Raccoon.GCode.Tool.tools[(int)toolID], lines, ref preview, filename, Zsec, XYfeed, Retreat, 54, (int)base.infeed);

                        DA.SetData(0, preview.outputInformation);
                        DA.SetDataList(1, GCode);
                    }
                }
                catch (Exception ex)
                {
                    Rhino.RhinoApp.WriteLine(ex.ToString());
                }
            }

            // GUID = guids;
        }

        public override Guid ComponentGuid => new Guid("4ebca11f-e6c6-4179-837d-9c18d1a5869e");

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

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 0);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }
    }
}