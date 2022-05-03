using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Text;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Raccoon
{
    public struct PreviewObject
    {
        public PointCloud vertices;
        public List<Line> PreviewLines0;
        public List<Line> PreviewLines1;
        public List<Line> PreviewLines2;
        public Polyline PreviewPolyline;
        public List<Curve> badCurves;
        public string outputInformation;
    }

    public class CustomComponent : GH_Component
    {
        public bool run_once = true;
        public PreviewObject preview;
        public BoundingBox bbox = Utilities.MakaDimensions.MakaBBox();
        public Rhino.Display.DisplayMaterial m = new Rhino.Display.DisplayMaterial(Color.White);
        public override BoundingBox ClippingBox => bbox;
        public double tolerance = 0.01;

        public List<Line> lines = new List<Line>();
        public List<Polyline> polylines = new List<Polyline>();
        public List<Curve> badCurves = new List<Curve>();
        public string directory;
        public List<string> GCode = new List<string>();
        public string filename = "P1234567";
        public double zero1 = 54;
        public double toolr = 5;
        public double toolID = 57;
        public double Zsec = 350;
        public double XYfeed = 5000;
        public double Zfeed = 1500;
        public double Retreat = 60;
        public double infeed = 2;
        public bool notch = true;

        //public static Dictionary<int, Raccoon.GCode.ToolParameters> tools = Raccoon.GCode.Tool.ToolsFromAssembly();

        public CustomComponent(string Name, string Nick, string Desc) : base(Name, Nick, Desc, "Raccoon", "CNC")
        {
        }

        public CustomComponent(string Name, string Nick, string Desc, string subCategory) : base(Name, Nick, Desc, "Raccoon", subCategory)
        {
            Raccoon.GCode.Tool.SetDefaultTools();
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
                args.Display.DrawPolyline(preview.PreviewPolyline, Color.Orange, 2);

            if (preview.badCurves != null)

                foreach (Curve c in preview.badCurves)
                {
                    args.Display.DrawCurve(c, Color.Red, 5);
                }

            //Vertices
            if (preview.vertices != null)
            {
                args.Display.DrawPointCloud(preview.vertices, 3, Color.Blue);
                //args.Display.DrawArrows(lines, Color.MediumVioletRed);
            }
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            for (int i = 0; i < pManager.ParamCount; i++)
                pManager.HideParameter(i);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
            for (int i = 0; i < pManager.ParamCount; i++)
                pManager.HideParameter(i);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
        }

        public override Guid ComponentGuid => new Guid("e6eda5fb-6967-425f-8cee-15eade0f26f1");

        //https://stackoverflow.com/questions/439007/extracting-path-from-openfiledialog-path-filename
        //https://msdn.microsoft.com/en-us/library/system.windows.forms.savefiledialog(v=vs.110).aspx
        //https://discourse.mcneel.com/t/save-file-directory/62784/2
        protected override void AppendAdditionalComponentMenuItems(ToolStripDropDown menu)
        {
            Menu_AppendItem(menu, "Select Directory with Address...", (_, __) =>
            {
                SaveFileDialog saveFileDialog1 = new SaveFileDialog();

                saveFileDialog1.FileName = filename;
                saveFileDialog1.Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
                saveFileDialog1.FilterIndex = 2;
                saveFileDialog1.RestoreDirectory = true;

                if (saveFileDialog1.ShowDialog() == DialogResult.OK)
                {
                    using (System.IO.StreamWriter sw = new System.IO.StreamWriter(saveFileDialog1.FileName))
                    {
                        int i = 0;
                        foreach (string s in GCode)
                        {
                            if (i == 0)
                            {
                                sw.WriteLine(System.IO.Path.GetFileName(saveFileDialog1.FileName));
                            }
                            else
                            {
                                sw.WriteLine(s);
                            }
                            i++;
                        }
                        //GCode
                    }

                    ExpireSolution(true);
                }
            });

            Menu_AppendItem(menu, "Select Directory...", (_, __) =>
            {
                var folderDialog = new FolderBrowserDialog();
                var run = folderDialog.ShowDialog();

                if (run == DialogResult.OK)
                {
                    directory = folderDialog.SelectedPath;
                    System.IO.File.WriteAllLines(folderDialog.SelectedPath + "/" + filename, GCode);
                    Rhino.RhinoApp.WriteLine(directory);
                    Rhino.RhinoApp.WriteLine((GCode.Count > 0).ToString());
                    ExpireSolution(true);
                }
            });
        }

        //protected override System.Drawing.Bitmap Icon
        //{
        //    get
        //    {
        //        return CustomComponent.GetIcon(this);
        //    }
        //}

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
    }
}