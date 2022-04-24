using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.GCode
{
    public struct ToolParameters
    {
        public int id;
        public double radius;
        public double length;
        public int maxSpindleSpeed;
        public int prescribedSpindleSpeed;
        public int turn;
        public double cutLength;
        public int saw;
        public double holderRadius;

        public override string ToString()
        {
            return String.Format("(*ToolParams Id_{0} Radius_{1} Length_{2} MaxSpindleSpeed_{3} PrescribedSpindleSpeed_{4} Turn_{5} CutLength_{6} Saw_{7} HolderRadius_{8} )",
              id, radius, length, maxSpindleSpeed, prescribedSpindleSpeed, turn, cutLength, saw, holderRadius);
        }
    }

    public static class Tool
    {
        public static Dictionary<int, GCode.ToolParameters> tools = new Dictionary<int, ToolParameters>();

        public static void SetDefaultTools()
        {
            if (Raccoon.GCode.Tool.tools.Count == 0)
                Raccoon.GCode.Tool.tools = Raccoon.GCode.Tool.ToolsDefault();
        }

        public static ToolParameters ToolFromOneString(string s)
        {
            //try
            //{
            string[] words = s.Split(' ');
            ToolParameters t = new ToolParameters();
            int count = 0;

            //Iterate through each parameter and fill ToolParameters struct
            foreach (string w in words)
            {
                string[] parameter = w.Split('_');

                if (w.Length == 0)
                    continue;

                switch (parameter[0][0])
                {
                    case ('I'):
                        t.id = Convert.ToInt32(parameter[1]);
                        count++;
                        break;

                    case ('R'):
                        t.radius = Convert.ToDouble(parameter[1]);
                        count++;
                        break;

                    case ('L'):
                        t.length = Convert.ToDouble(parameter[1]);
                        count++;
                        break;

                    case ('M'):
                        t.maxSpindleSpeed = Convert.ToInt32(parameter[1]);
                        count++;
                        break;

                    case ('P'):
                        t.prescribedSpindleSpeed = Convert.ToInt32(parameter[1]);
                        count++;
                        break;

                    case ('T'):
                        t.turn = Convert.ToInt32(parameter[1]);
                        count++;
                        break;

                    case ('C'):
                        t.cutLength = Convert.ToDouble(parameter[1]);
                        count++;
                        break;

                    case ('S'):
                        t.saw = Convert.ToInt32(parameter[1]);
                        count++;
                        break;

                    case ('H'):
                        t.holderRadius = Convert.ToDouble(parameter[1]);
                        count++;
                        break;
                }
            }

            //Only if all parameters are found
            if (count == 9)
            {
                return t;
            }
            //}
            //catch (Exception e)
            //{
            //    Rhino.RhinoApp.WriteLine(e.ToString());
            //}
            return new ToolParameters();
        }

        public static Dictionary<int, ToolParameters> ToolsFromAssembly()
        {
            return ToolsDefault();
            //string assemblyLocation = System.Reflection.Assembly.GetExecutingAssembly().Location;
            //string assemblyPath = System.IO.Path.GetDirectoryName(assemblyLocation);
            //assemblyPath += @"\Tools.txt";

            //string[] lines = System.IO.File.ReadAllLines(assemblyPath);

            //return ToolsFromText(lines);
        }

        public static Dictionary<int, ToolParameters> ToolsDefault()
        {
            Dictionary<int, ToolParameters> tools = new Dictionary<int, ToolParameters>();
            ToolParameters default_tool = new ToolParameters
            {
                id = 42,
                radius = 9.901,
                length = 169.830,
                maxSpindleSpeed = 24000,
                prescribedSpindleSpeed = 12000,
                turn = 3,
                cutLength = 90.000,
                saw = 0,
                holderRadius = 30
            };
            tools.Add(default_tool.id, default_tool);
            return tools;
        }

        public static Dictionary<int, ToolParameters> ToolsFromText(string[] lines)
        {
            Dictionary<int, ToolParameters> tools = new Dictionary<int, ToolParameters>();

            //iterate through each paramaeter line split by enter
            foreach (string s in lines)
            {
                var t = ToolFromOneString(s);

                //Only if all parameters are found
                if (t.id != 0)
                    tools.Add(t.id, t);
            }

            return tools;
        }

        public static Dictionary<int, ToolParameters> ToolsFromText(string Tool)
        {
            string[] lines = Tool.Split(new[] { Environment.NewLine }, StringSplitOptions.None);
            return ToolsFromText(lines);
        }
    }
}