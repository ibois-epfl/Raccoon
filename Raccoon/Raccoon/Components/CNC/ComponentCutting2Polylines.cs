using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.Components.CNC
{
    public class ComponentCutting2Polylines : CustomComponent
    {
        /// <summary>
        /// Initializes a new instance of the Cutting2Polylines class.
        /// </summary>
        public ComponentCutting2Polylines()
          : base("Two Polys", "Two Polys", "Cutting Two Polylines", "Robot/CNC")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.primary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Curves", "Curves", "Pairs of Polylines defining loft surface to cut", GH_ParamAccess.list);
            //pManager.AddTextParameter("Filename", "Filename", "filename - 8 - digit filename in format P0000000 you do not need to specify path, just right click and save to directory", GH_ParamAccess.item);// "P1234567"

            //pManager.AddNumberParameter("Zero", "Zero", "CNC Zeropoint (e.g.G54, G55, G56... )", GH_ParamAccess.item);// 54
            pManager.AddNumberParameter("ToolRadius", "ToolRadius", "Milling tool radius in mm", GH_ParamAccess.item);//10
            pManager.AddNumberParameter("ToolID", "ToolID", "Tool ID Number in the CNC machine", GH_ParamAccess.item);//54
            pManager.AddNumberParameter("Zsec", "Zsec", "Safe Plane over workpiece.Program begins and starts at this Z-height", GH_ParamAccess.item);//300
            pManager.AddNumberParameter("Speed", "Speed", "velocity of horizontal cutting in mm/min", GH_ParamAccess.item);//5000
                                                                                                                           // pManager.AddNumberParameter("Zfeed", "Zfeed", "velocity of vertical cutting in mm/min", GH_ParamAccess.item);// 2000
            pManager.AddNumberParameter("Retreate", "Retreate", "Height of the XY Plane for tool retreat", GH_ParamAccess.item);//70
            pManager.AddNumberParameter("Infeed", "Infeed", "Number of vertical infeeds", GH_ParamAccess.item);//2
            pManager.AddNumberParameter("Angle", "Angle", "if angle is bigger than defined it will be skipped from cutting", GH_ParamAccess.item);// 60
            pManager.AddNumberParameter("MaxRot", "MaxRot", "If the next angle is bigger than this, the CNC retreates and rotate back", GH_ParamAccess.item);// 60

            pManager.AddBooleanParameter("Notch", "Notch", "Cut notches on concave corners", GH_ParamAccess.item);//true
            pManager.AddBooleanParameter("Pairs", "Pairs", "If paired is off, the curves list must by supplied in order i.e. For plate 1: curve bottom and curve top, For plate2: curve bottom and curve top and so on as flattened list", GH_ParamAccess.item);//true

            for (int i = 0; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
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

            int[] recID = new int[] { 0 };

            for (int i = 0; i < recID.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Curve ri = Params.Input[recID[i]] as Grasshopper.Kernel.Parameters.Param_Curve;
                if (ri == null || ri.SourceCount > 0 || ri.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)ri.Attributes.Pivot.X - 225;
                int y = (int)ri.Attributes.Pivot.Y;
                IGH_Param rect = new Grasshopper.Kernel.Parameters.Param_Curve();
                rect.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 0, recValues[0].ToNurbsCurve());
                rect.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 1, recValues[1].ToNurbsCurve());
                rect.CreateAttributes();
                rect.Attributes.Pivot = new System.Drawing.PointF(x, y);
                rect.Attributes.ExpireLayout();
                document.AddObject(rect, false);
                ri.AddSource(rect);
            }

            //Add sliders

            double[] sliderValue = new double[] { 0, 42, 400, 17000, 70, 2, 60, 80 };
            double[] sliderMinValue = new double[] { 0, 10, 50, 2000, 50, 1, 30, 80 };
            double[] sliderMaxValue = new double[] { 20.001, 400, 650, 30000, 600, 7, 60, 1500 };
            int[] sliderID = new int[] { 1, 2, 3, 4, 5, 6, 7, 8 };
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
            bool[] boolValue = new bool[] { true, false };
            int[] boolID = new int[] { 9, 10 };

            for (int i = 0; i < boolID.Length; i++)
            {
                Grasshopper.Kernel.Parameters.Param_Boolean bi = Params.Input[boolID[i]] as Grasshopper.Kernel.Parameters.Param_Boolean;
                if (bi == null || bi.SourceCount > 0 || bi.PersistentDataCount > 0) return;
                Attributes.PerformLayout();
                int x = (int)bi.Attributes.Pivot.X - 250;
                int y = (int)bi.Attributes.Pivot.Y - 10;
                Grasshopper.Kernel.Special.GH_BooleanToggle booleanToggle = new Grasshopper.Kernel.Special.GH_BooleanToggle();
                booleanToggle.CreateAttributes();
                booleanToggle.Attributes.Pivot = new System.Drawing.PointF(x, y);
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
            pManager.AddCurveParameter("SharpAngle", "SharpAngle", "SharpAngle", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            List<Curve> curves = new List<Curve>();

            //DA.GetData(1, ref base.filename);
            //DA.GetData(2, ref base.zero1);
            DA.GetData("ToolRadius", ref base.toolr);
            DA.GetData("ToolID", ref base.toolID);
            DA.GetData("Zsec", ref base.Zsec);
            DA.GetData("Speed", ref base.XYfeed);

            DA.GetData("Retreate", ref base.Retreat);
            DA.GetData("Infeed", ref base.infeed);
            double angleTol = 60;
            double maxAngle = 80;
            DA.GetData("Angle", ref angleTol);
            DA.GetData("MaxRot", ref maxAngle);
            DA.GetData("Notch", ref base.notch);
            bool pairing = false;
            DA.GetData("Pairs", ref pairing);

            GCode = new List<string>();
            if (DA.GetDataList(0, curves))
            {
                try
                {
                    //Check if curves below zero
                    var belowZero = Utilities.GeometryProcessing.IsCurvesBelowZero(curves);

                    if (belowZero.Count > 0)
                        this.AddRuntimeMessage(GH_RuntimeMessageLevel.Warning, "Geometry is below Zero");

                    //tools = Raccoon.GCode.Tool.ToolsFromAssembly();
                    if (Raccoon.GCode.Tool.tools.ContainsKey((int)toolID))
                    {
                        this.toolr = (this.toolr == 0) ? Raccoon.GCode.Tool.tools[(int)toolID].radius : this.toolr;
                        //Rhino.RhinoApp.WriteLine(toolr.ToString());

                        this.preview = new PreviewObject();
                        preview.PreviewLines0 = new List<Line>();
                        preview.PreviewLines1 = new List<Line>();
                        preview.PreviewLines2 = new List<Line>();
                        preview.vertices = new PointCloud();
                        List<Curve> sharpPolylines = new List<Curve>();

                        GCode = Raccoon.GCode.Cutting.CNC5XCut2Polylines(Raccoon.GCode.Tool.tools[(int)toolID], curves, ref preview, ref sharpPolylines,
                             toolr, Zsec, XYfeed, Retreat, (int)infeed, notch, pairing, angleTol, maxAngle, 54, filename);
                        Raccoon.GCode.GCodeToGeometry.DrawToolpath(GCode, ref preview);

                        this.preview.badCurves = new List<Curve>();
                        this.preview.badCurves.AddRange(belowZero);
                        this.preview.badCurves.AddRange(sharpPolylines);

                        //output
                        DA.SetData(0, preview.outputInformation);
                        DA.SetDataList(1, GCode);
                        DA.SetDataList(2, sharpPolylines);
                    }
                }
                catch (Exception ex)
                {
                    Rhino.RhinoApp.WriteLine(ex.ToString());
                }
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        ///

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.pair;
            }
        }

        public override Guid ComponentGuid => new Guid("1a2be114-fe1f-4d3b-b01f-49a78c487256");

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

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 0, 150);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }
    }
}