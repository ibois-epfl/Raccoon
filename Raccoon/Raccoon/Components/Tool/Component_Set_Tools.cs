using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;

namespace Raccoon.Components.Tool
{
    public class Component_Set_Tools : CustomComponent
    {
        public Component_Set_Tools()
          : base("Tools", "Tools",
              "Set Tools from Text file",
             "Tool")
        {
        }

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddTextParameter("Folder_and_File_Name_of_Tools", "Folder_and_File_Name_of_Tools", "Path with the set of tools \n Each line in the Tools.txt must contain such code: \n ID_140 Radius_7.098 Length_215.03 MaxSpindleSpeed_24000 PrescribedSpindleSpeed_18000 Turn_3 CutLength_5.000 Saw_0 HolderRadius_30.000", GH_ParamAccess.item,
                @"C:\Users\petra\AppData\Roaming\Grasshopper\Libraries\Raccoon\Tools.txt");
            pManager.AddNumberParameter("Tolerance", "Tolerance", "This parameter increase/decrease tool radius", GH_ParamAccess.item, 0.0);
        }

        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
            string filename = "";
            DA.GetData(0, ref filename);

            double tolerance = 0.0;

            DA.GetData(1, ref tolerance);

            //Raccoon.GCode.Tool.ToolsFromAssembly();
            //string assemblyLocation = System.Reflection.Assembly.GetExecutingAssembly().Location;
            //string assemblyPath = System.IO.Path.GetDirectoryName(assemblyLocation);
            //assemblyPath += @"\Tools.txt";

            string[] lines = System.IO.File.ReadAllLines(filename);

            Raccoon.GCode.Tool.tools = Raccoon.GCode.Tool.ToolsFromText(lines);

            foreach (var t in System.Linq.Enumerable.ToList(Raccoon.GCode.Tool.tools))
            {
                var parameters = Raccoon.GCode.Tool.tools[t.Key];
                parameters.radius += tolerance;
                Raccoon.GCode.Tool.tools[t.Key] = parameters;
            }

            string message = "\nLATER REFRESH GRASSHOPPER\nSOLUTION -> RECOMPUTE\n\n";
            foreach (var t in Raccoon.GCode.Tool.tools)
            {
                int n = Math.Abs(4 - t.Key.ToString().Length);
                string id = new string('_', n);

                n = Math.Abs(7 - string.Format("{0:0.000}", t.Value.radius).Length);
                string r = new string('_', n);

                n = Math.Abs(7 - string.Format("{0:0.000}", t.Value.cutLength).Length);
                string l = new string('_', n);

                message += "ID" + id + t.Key.ToString() + "     R_" + r + string.Format("{0:0.000}", t.Value.radius) + "     L_" + l + string.Format("{0:0.000}", t.Value.cutLength) + "\n";
            }
            base.Message = message;

            Raccoon.GCode.Tool.SetDefaultTools();
            //GH_Document doc = OnPingDocument();
            //doc.NewSolution(true);
        }

        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                //You can add image files to your project resources and access them like this:
                // return Resources.IconForThisComponent;
                return Properties.Resources.tools;
            }
        }

        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid
        {
            get { return new Guid("ba02b67b-fef7-478f-9fec-ac5df6fd5524"); }
        }
    }
}