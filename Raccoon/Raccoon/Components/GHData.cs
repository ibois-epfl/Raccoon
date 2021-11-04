using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.Components
{
    public static class DataAccessHelper
    {

        public static List<Polyline> ToPolylines(this List<Curve> curves)
        {
            List<Polyline> polylines = new List<Polyline>();
            foreach (Curve c in curves)
            {
                if (c.TryGetPolyline(out Polyline polyline))
                    polylines.Add(polyline);
            }
            return polylines;
        }
        public static void WriteLine(this string s) => Rhino.RhinoApp.WriteLine(s);

        public static void Write(this string s) => Rhino.RhinoApp.Write(s);

        public static void Bake(this Mesh l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddMesh(l);

        public static void Bake(this Plane l, double w = 0.1) => Rhino.RhinoDoc.ActiveDoc.Objects.AddCurve((new Rectangle3d(l, new Interval(-w * 0.5, w * 0.5), new Interval(-w * 0.5, w * 0.5))).ToNurbsCurve());

        public static void Bake(this Point3d l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddPoint(l);

        public static void Bake(this IEnumerable<Point3d> l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddPoints(l);

        public static void Bake(this Line l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddLine(l);

        public static void Bake(this Polyline l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddPolyline(l);

        public static void Bake(this Curve l) => Rhino.RhinoDoc.ActiveDoc.Objects.AddCurve(l);

        public static void Bake(this IEnumerable<Line> L)
        {
            foreach (var l in L)
                l.Bake();
        }

        public static void Bake(this IEnumerable<Polyline> L)
        {
            foreach (var l in L)
                l.Bake();
        }

        public static void Bake(this IEnumerable<Curve> L)
        {
            foreach (var l in L)
                l.Bake();
        }

        public static void Bake(this IEnumerable<Plane> L, double w = 0.1)
        {
            foreach (var l in L)
                l.Bake(w);
        }

        public static void Bake(this IEnumerable<Mesh> L)
        {
            foreach (var l in L)
                l.Bake();
        }

        /// <summary>
        /// Fetch data at index position
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="position"></param>
        /// <returns></returns>
        static public T Fetch<T>(this IGH_DataAccess da, int position)
        {

            T temp = default(T);
            da.GetData<T>(position, ref temp);
            return temp;
        }
        /// <summary>
        /// Fetch data with name
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        static public T Fetch<T>(this IGH_DataAccess da, string name)
        {
            T temp = default(T);
            da.GetData<T>(name, ref temp);
            return temp;
        }

        /// <summary>
        /// Fetch data list with position
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="position"></param>
        /// <returns></returns>
        static public List<T> FetchList<T>(this IGH_DataAccess da, int position)
        {
            List<T> temp = new List<T>();
            da.GetDataList<T>(position, temp);
            return temp;
        }

        /// <summary>
        /// Fetch data list with name
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        static public List<T> FetchList<T>(this IGH_DataAccess da, string name)
        {
            List<T> temp = new List<T>();
            da.GetDataList<T>(name, temp);
            return temp;
        }
        /// <summary>
        /// Fetch structure with position
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="position"></param>
        /// <returns></returns>
        static public GH_Structure<T> FetchTree<T>(this IGH_DataAccess da, int position) where T : IGH_Goo
        {
            GH_Structure<T> temp;
            da.GetDataTree<T>(position, out temp);
            return temp;
        }

        /// <summary>
        /// Fetch structure with name
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="da"></param>
        /// <param name="name"></param>
        /// <returns></returns>
        static public GH_Structure<T> FetchTree<T>(this IGH_DataAccess da, string name) where T : IGH_Goo
        {
            GH_Structure<T> temp;
            da.GetDataTree<T>(name, out temp);
            return temp;
        }


    }
}
