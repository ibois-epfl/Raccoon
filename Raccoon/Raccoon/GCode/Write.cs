using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.GCode
{
    public class Write
    {

        /// <summary>
        /// Postprocessing function
        /// 1. Takes care of formatting
        ///     1.1. Numbers are added in front of lines
        ///     1.2. Program is started and ended as expected by the machine
        /// 2. Saves file
        /// 3. Launches simulation
        /// </summary>
        /// <param name="arrnc"></param>
        /// <param name="filename"></param>
        /// <param name="strinfo"></param>
        public static void WriteAndCheck(ref List<string> arrnc, ref PreviewObject preview, string filename = "P1234567", string strinfo = "5X_3dCrvs", string tool = null)
        {

            //string filepath = @"C:\Maka\" + filename + "_" + strinfo + TimeStamp();

            int N = 20;

            for (int i = 0; i < arrnc.Count; i++)
            {
                arrnc[i] = "N" + N.ToString() + " " + arrnc[i];
                N += 10;
            }

            arrnc.Insert(0, filename);

            if (tool != null)
                arrnc.Insert(1, tool);
            else
                arrnc.Insert(1, ("(Tool Parameters unknown)"));
            //arrnc.Insert(1,"( " + (arrnc.Count-1).ToString() +" commands to process )");
            //arrnc.Insert(1, "(____________Loading____________)");
            //arrnc.Insert(2, "N10" + " G47" + " (G47 - 3 axis in plane)");//Tool offset double increase

            arrnc.Add("N" + N.ToString() + " M5" + " (stop the spindle from turning)");
            N += 10;
            arrnc.Add("N" + N.ToString() + " M30" + " (program end and rewind)");
            arrnc.Add("#\n");

            //Rhino.RhinoApp.WriteLine("Writing1");
            GCode.GCodeToGeometry.DrawToolpath(arrnc, ref preview);
        }



        public static List<string> WriteAndCheck2(List<string> arrstr)
        {
            List<string> str_out = new List<string>();
            int N = 10;
            foreach (string str in arrstr)
            {
                str_out.Add("N" + N.ToString() + " " + str);
                N += 10;
            }

            str_out.Add("N" + N.ToString() + " M5");
            N += 10;
            str_out.Add("N" + N.ToString() + " M30");
            str_out.Add("#");

            return str_out;
        }

        public static string TimeStamp()
        {
            var lt = DateTime.Now.ToLocalTime();
            return "_" + lt.Year.ToString() + "-" + lt.Month.ToString() + "-" + lt.Day.ToString() + "_" + lt.Hour.ToString() + "-" + lt.Minute.ToString() + "-" + lt.Second.ToString();
        }

    }
}
