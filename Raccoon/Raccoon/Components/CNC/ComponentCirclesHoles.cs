using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.Components.CNC
{
    public class ComponentCirclesHoles : CustomComponent
    {
        /// <summary>
        /// Initializes a new instance of the Cutting2Polylines class.
        /// </summary>
        public ComponentCirclesHoles()
          : base("Circle Holes", "Circle Holes", "One curve is cutting line, other curve is followed as normal (takes only control points of polyline)", "Robot/CNC")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.secondary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("Line", "Line", "Line", GH_ParamAccess.list);
            pManager.AddNumberParameter("Radius", "Radius", "Radius", GH_ParamAccess.list);
            pManager.AddNumberParameter("Scale", "Scale", "Divisions, bigger number, less points", GH_ParamAccess.list);
            pManager.AddNumberParameter("Turns", "Turns", "Turns of Spiral (integer)", GH_ParamAccess.list);

            //pManager.AddTextParameter("Filename", "Filename", "filename - 8 - digit filename in format P0000000 you do not need to specify path, just right click and save to directory", GH_ParamAccess.item);// "P1234567"

            pManager.AddNumberParameter("ToolID", "ToolID", "Tool ID Number in the CNC machine", GH_ParamAccess.item);//0

            pManager.AddNumberParameter("Speed", "Speed", "velocity of horizontal cutting in mm/min", GH_ParamAccess.item);//5000

            pManager.AddNumberParameter("Zsec", "Zsec", "Safe Plane over workpiece.Program begins and starts at this Z-height", GH_ParamAccess.item);//700
            pManager.AddNumberParameter("Retreate", "Retreate", "Height of the XY Plane for tool retreat", GH_ParamAccess.item);//70

            pManager.AddNumberParameter("Infeed", "Infeed", "Interpolation between normal and path curve", GH_ParamAccess.item);//1
            pManager.AddNumberParameter("Angle", "Angle", "Angle to rotate in position", GH_ParamAccess.item);//1

            for (int i = 0; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
        }

        //Inputs
        public override void AddedToDocument(GH_Document document)
        {
            base.AddedToDocument(document);

            //Add Curve

            Curve[] recValues = new Curve[] {
              ( new Line( new Point3d(849.65,-1020.35,402.25),new Point3d(829.73,-1055.10,384.04) )).ToNurbsCurve()
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
                rect.AddVolatileData(new Grasshopper.Kernel.Data.GH_Path(0), 0, recValues[i].ToNurbsCurve());
                rect.CreateAttributes();
                rect.Attributes.Pivot = new System.Drawing.PointF(x, y);
                rect.Attributes.ExpireLayout();
                document.AddObject(rect, false);
                ri.AddSource(rect);
            }

            ////Add String
            //Grasshopper.Kernel.Parameters.Param_String ti = Params.Input[2] as Grasshopper.Kernel.Parameters.Param_String;
            //if (ti == null || ti.SourceCount > 0 || ti.PersistentDataCount > 0) return;
            //Attributes.PerformLayout();
            //int xT = (int)ti.Attributes.Pivot.X - 250;
            //int yT = (int)ti.Attributes.Pivot.Y - 10;
            //Grasshopper.Kernel.Special.GH_Panel panel = new Grasshopper.Kernel.Special.GH_Panel();
            //panel.CreateAttributes();
            //panel.SetUserText("P1234567");
            //panel.Attributes.Bounds = new System.Drawing.RectangleF(xT, yT, 200, 12);
            //panel.Attributes.Pivot = new System.Drawing.PointF(xT, yT);
            //panel.Attributes.ExpireLayout();
            //document.AddObject(panel, false);
            //ti.AddSource(panel);

            //Add sliders

            double[] sliderValue = new double[] { 12.5, 1.00, 5, 42, 20000, 650, 40, 1, 80 };
            double[] sliderMinValue = new double[] { 1.00, 0.01, 1, 1, 1000, 0, 300, 1, 79 };
            double[] sliderMaxValue = new double[] { 100, 2.00, 10, 110, 30000, 650, 0, 20, 81 };
            int[] sliderID = new int[] { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

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
            pManager.AddTextParameter("Info", "Info", "Info", GH_ParamAccess.item);
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            ////////////////////////////////////////////////////////////////////////////////////
            ///Get Lines and convert to spirals
            ////////////////////////////////////////////////////////////////////////////////////
            List<Curve> C = new List<Curve>(); //null;
            List<double> R = new List<double>();// 12.5;
            List<double> S = new List<double>(); //1;
            List<double> T = new List<double>(); //3;

            DA.GetDataList(0, C);
            DA.GetDataList(1, R);
            DA.GetDataList(2, S);
            DA.GetDataList(3, T);
            DA.GetData("ToolID", ref base.toolID);
            this.toolr = Raccoon.GCode.Tool.tools.ContainsKey((int)toolID) ? Raccoon.GCode.Tool.tools[(int)toolID].radius : 0;

            if (this.toolr != 0)
            {
                for (int i = 0; i < R.Count; i++)
                    R[i] -= this.toolr;
            }

            if (C.Count != R.Count)
                if (R.Count > 0)
                    R = Enumerable.Repeat(R[0], C.Count).ToList();
                else
                    return;

            if (C.Count != S.Count)
                if (S.Count > 0)
                    S = Enumerable.Repeat(S[0], C.Count).ToList();
                else
                    return;

            if (C.Count != T.Count)
                if (T.Count > 0)
                    T = Enumerable.Repeat(T[0], C.Count).ToList();
                else
                    return;

            for (int i = 0; i < R.Count; i++)
                R[i] = Math.Max(0.01, R[i]);

            List<Curve> crv = new List<Curve>();
            List<Curve> crvN = new List<Curve>();

            if (C.Count == 0)
                return;

            for (int i = 0; i < C.Count; i++)
            {
                Line L = new Line(C[i].PointAtStart, C[i].PointAtEnd);
                if (L != Line.Unset)
                {
                    Vector3d v = L.Direction;
                    Plane plane = new Plane(L.From, v);

                    Polyline p0 = Raccoon.Utilities.GeometryProcessing.Polygon((int)(Math.Max(3, R[i] * 1.00 / Math.Max(0.0001, S[i]))), (Math.Max(0.001, R[i])), plane, 0, false);
                    Polyline p1 = new Polyline(p0);

                    p1.Transform(Transform.Translation(v));

                    //SPiral
                    Polyline[] p = (T[i] != 0) ? Raccoon.Utilities.GeometryProcessing.InterpolatePolylines(p0, p1, (int)T[i]) : Raccoon.Utilities.GeometryProcessing.InterpolatePolylines(p0, p1, (int)Math.Max(0, (L.Length / R[i]) - 1));

                    //Rhino.RhinoApp.WriteLine(p.Length.ToString());

                    Rhino.Geometry.Interval interval = new Rhino.Geometry.Interval(0, 1);
                    int n = p0.Count;
                    double[] tInterval = new double[n];
                    for (int j = 0; j < n; j++)
                    {
                        tInterval[j] = interval.ParameterAt((double)j / (double)(n - 1));
                    }

                    //Spiral
                    Polyline spiral = new Polyline();

                    for (int j = 0; j < p.Length - 1; j++)
                    {
                        for (int k = 0; k < n; k++)
                        {
                            if (j == 1 && k == 0)
                                continue;
                            spiral.Add(Raccoon.Utilities.GeometryProcessing.Lerp(p[j][k], p[j + 1][k], tInterval[k]));
                        }
                    }

                    for (int j = 0; j < n; j++)
                    {
                        if (j == 0 || j == n - 1)
                            continue;
                        spiral.Add(p[p.Length - 1][j]);
                    }

                    spiral.Add(p[0][n - 2]);

                    Polyline spiral1 = new Polyline(spiral);
                    v.Unitize();
                    spiral1.Transform(Transform.Translation(-v * R[i] * 0.1));

                    crv.Add(spiral.ToNurbsCurve());
                    crvN.Add(spiral1.ToNurbsCurve());
                }
            }

            //double angle = DA.Fetch<double>("Angle");

            /////////////////////////////////////////////////////////////////////////////////////
            ///Convert spirals to tool-path
            /////////////////////////////////////////////////////////////////////////////////////

            DA.GetData("Zsec", ref base.Zsec);
            DA.GetData("Speed", ref base.XYfeed);
            DA.GetData("Retreate", ref base.Retreat);
            DA.GetData("Infeed", ref base.infeed);

            //Rhino.RhinoApp.WriteLine(angle.ToString());

            GCode = new List<string>();
            if (crv.Count > 0)
            {
                try
                {
                    List<Polyline> polylines = new List<Polyline>();
                    List<Polyline> normals = new List<Polyline>();

                    if (crv.Count == crvN.Count)
                    {
                        for (int i = 0; i < crv.Count; i++)
                        {
                            crv[i].TryGetPolyline(out Polyline path);
                            crvN[i].TryGetPolyline(out Polyline normal);

                            Polyline[] interpolatedPolylines = Utilities.GeometryProcessing.InterpolatePolylinesZigZag(normal, path, (int)base.infeed);

                            polylines.Add(interpolatedPolylines[0]);
                            normals.Add(interpolatedPolylines[1]);
                        }
                    }

                    //tools = Raccoon.GCode.Tool.ToolsFromAssembly();
                    if (Raccoon.GCode.Tool.tools.ContainsKey((int)toolID))
                    {
                        preview.PreviewLines0 = new List<Line>();
                        preview.PreviewLines1 = new List<Line>();
                        preview.PreviewLines2 = new List<Line>();
                        preview.vertices = new PointCloud();
                        double angle = 80;
                        GCode = Raccoon.GCode.Cutting.PolylineCutSimple(Raccoon.GCode.Tool.tools[(int)toolID], polylines, ref preview, normals, filename, Zsec, XYfeed, Retreat, (int)angle);
                        Raccoon.GCode.GCodeToGeometry.DrawToolpath(GCode, ref preview);

                        DA.SetData(0, preview.outputInformation);
                        DA.SetDataList(1, GCode);
                    }
                }
                catch (Exception ex)
                {
                    Rhino.RhinoApp.WriteLine(ex.ToString());
                }
            }
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.spiralDrill;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid => new Guid("1a2be664-fe1f-4d3b-b01f-49a78c488916");

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

            g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 150);

            ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            for (int i = 0; i < guids.Count; i++)
                g.AddObject(guids[i]);
            g.ExpireCaches();
        }
    }
}