using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;


using Rhino.Geometry;

namespace Raccoon {


   
    public class Cut
    {
        #region properties
        //Indexing
        public int id;
        public int weight = 0; //biggest weight start first


        //Fabrication
        public CutType cutType = CutType.Cut;
        public Plane refPlane = Plane.Unset;
        public List<Plane> planes = new List<Plane>();
        public double toolR=1;
        public Vector3d insertionVector = Vector3d.Unset;//why not at the Elements
        public bool project = true;
        public bool notches = false;
        public List<byte[]> notchesTypes;// 2 - Edge A, 3 - Edge B , 1 - Corner, 10 - nothing
        public double filletR = 0;
        public bool projectRotate = false;
        public bool SawFlip90Cut = false;
        public bool DrillTwoSides = false;

        //SawBlade
        public bool rotatable = false;

        //Fabrication Binary Cut
        public List<Polyline> plinesBinary = new List<Polyline>();  //Binary cut, sometimes angles are two big or not possible to cut  //Additional pair of polylines are used to get the most orthogonal to plane axis cut
        public Polyline plineReference = new Polyline();

        //For plates
        public bool CutOrHole = true;
        public bool PolylineMergeTakeOutside = false;
        public bool merge = true;

        //Geometry Display
        public List<Polyline> plines = new List<Polyline>(); //why not at the Tile
        public List<Brep> breps = new List<Brep>();
        public List<Mesh> meshes = new List<Mesh>();


        public Polyline this[int i] {
            get { return plines[i]; }
            set { plines[i] = value; }
        }
        #endregion


        #region constructors
        public Cut() { }
        public Cut(int id) : this()
        {
            this.id = id;
        }

        public Cut(int id, List<Polyline> plines, CutType cutType, 
            bool project = true, bool notches = false, double filletR = 0, 
            bool CutOrHole = true, bool PolylineMergeTakeOutside = false, bool merge = false, 
            bool projectRotate = false, bool SawFlip90Cut = false) : this()//, List<double> speeds, int smooth
        {
            this.id = id;
            this.plines = plines.Duplicate();
            this.cutType = cutType;
            this.project = project;

            this.notches = notches;

            this.notchesTypes = new List<byte[]>(plines.Count);
            for (int i = 0; i < plines.Count; i++) {
                byte b =0;
                if (notches)
                    b = 1;
                this.notchesTypes.Add( Enumerable.Repeat(b, plines[i].SegmentCount).ToArray());
            }
            this.SawFlip90Cut = SawFlip90Cut;

            this.filletR = filletR;
            this.CutOrHole = CutOrHole;
            this.PolylineMergeTakeOutside = PolylineMergeTakeOutside;
            this.merge = merge;
            this.projectRotate = projectRotate;
            //Rhino.RhinoApp.WriteLine("Constructor " + this.merge.ToString());
        }

        public Cut(int id, Polyline cut, Polyline normal, CutType cutType, 
            bool project = false, bool notches = false, double filletR = 0,
            bool CutOrHole = true, bool PolylineMergeTakeOutside = false, bool merge = false, 
            bool projectRotate = false, bool SawFlip90Cut = false) : this()//, List<double> speeds, int smooth
{
            this.id = id;
            this.plines = new List<Polyline> { cut.Duplicate(), normal.Duplicate() };
            this.cutType = cutType;
            this.project = project;
            this.notches = notches;

            this.notchesTypes = new List<byte[]>(1);
            byte b = 0;
            if (notches)
                b = 1;
            this.notchesTypes.Add(Enumerable.Repeat(b, cut.SegmentCount).ToArray());
            this.SawFlip90Cut = SawFlip90Cut;

            this.filletR = filletR;
            this.CutOrHole = CutOrHole;
            this.PolylineMergeTakeOutside = PolylineMergeTakeOutside;
            this.merge = merge;
            this.projectRotate = projectRotate;
            // Rhino.RhinoApp.WriteLine("Constructor " + this.merge.ToString());

        }


        public Cut(int id, Polyline cut, Polyline normal, CutType cutType, 
            bool project , byte[] notchesTypes, double filletR = 0, 
            bool CutOrHole = true, bool PolylineMergeTakeOutside = false, bool merge = false, 
            bool projectRotate = false, bool SawFlip90Cut = false) : this()//, List<double> speeds, int smooth
{
            this.id = id;
            this.plines = new List<Polyline> { cut.Duplicate(), normal.Duplicate() };
            this.cutType = cutType;
            this.project = project;

          
            this.notches = notchesTypes == null ? false : true;

            this.notchesTypes = new List<byte[]>(1) {new byte[0] };
            byte b = 0;
            if (notchesTypes == null) {
                this.notchesTypes[0] = Enumerable.Repeat(b, cut.SegmentCount).ToArray();
            } else {
                this.notchesTypes[0] = notchesTypes;
            }
            this.SawFlip90Cut = SawFlip90Cut;
            


            this.filletR = filletR;
            this.CutOrHole = CutOrHole;
            this.PolylineMergeTakeOutside = PolylineMergeTakeOutside;
            this.merge = merge;
            this.projectRotate = projectRotate;
            // Rhino.RhinoApp.WriteLine("Constructor " + this.merge.ToString());

        }


        public Cut(int id, Polyline cut, Polyline normal, Polyline cutBinary, Polyline normalBinary, Polyline plineNormal,CutType cutType, bool project = false, bool notches = false, double filletR = 0, bool CutOrHole = true, bool PolylineMergeTakeOutside = false, bool merge = false, bool SawFlip90Cut = false) : this()//, List<double> speeds, int smooth
{
            this.id = id;
            this.plines = new List<Polyline> { cut.Duplicate(), normal.Duplicate() };
            this.cutType = cutType;
            this.project = project;

            this.notches = notches;

            this.notchesTypes = new List<byte[]>(1);
            byte b = 0;
            if (notches)
                b = 1;
            this.notchesTypes.Add(Enumerable.Repeat(b, cut.SegmentCount).ToArray());
            this.SawFlip90Cut = SawFlip90Cut;

            this.filletR = filletR;
            this.CutOrHole = CutOrHole;
            this.PolylineMergeTakeOutside = PolylineMergeTakeOutside;
            this.merge = merge;

            this.plinesBinary = new List<Polyline> { cutBinary.Duplicate(), normalBinary.Duplicate() };
            this.plineReference = plineNormal.Duplicate();


        }

        public Cut(int id, CutType cutType) : this(id)//, List<double> speeds, int smooth
        {
            this.cutType = cutType;
        }

        public Cut(int id, CutType cutType, Plane refPlane, List<Plane> planes) : this(id, cutType)//, List<double> speeds, int smooth
        {
            this.refPlane = new Plane(refPlane);

            this.planes = new List<Plane>();
            foreach (var p in planes)
                planes.Add(new Plane(p));

        }
        #endregion


        public void SetBinary(Polyline reference, int shift = 0) {
            this.plineReference = reference;
            Polyline[] binary0 = PolylineUtil.LoftTwoPolylines(new Polyline[] { this.plines[0], this.plines[1] });
            if (shift == 0) {
                this.plinesBinary = new List<Polyline> { binary0[2 + shift].Flip().ShiftPline(), binary0[0 + shift] };
            } else {
                this.plinesBinary = new List<Polyline> { binary0[0 + shift].Flip().ShiftPline(), binary0[2 + shift] };
            }
        
        }

        #region Duplicate Transform XForm ToString
        public Cut Duplicate()
        {
            Cut copy = new Cut(id, cutType, refPlane, planes);//,speeds, smooth

            foreach (var g in this.plines) {
                copy.plines.Add(g.Duplicate());
            }


            
            foreach (var g in this.breps)
                copy.breps.Add(g.DuplicateBrep());

            foreach (var g in this.meshes)
                copy.meshes.Add(g.DuplicateMesh());



            copy.toolR = this.toolR;
            copy.insertionVector = new Vector3d(this.insertionVector);
            copy.CutOrHole = this.CutOrHole;
            copy.PolylineMergeTakeOutside = this.PolylineMergeTakeOutside;
            copy.merge = this.merge;

            //copy.cutTypes = new List<CutType>();
            //for (int i = 0; i < cutTypes.Count; i++)
            //    copy.cutTypes.Add(cutTypes[i]);


            copy.project = this.project;
            
            copy.notches = this.notches;
            //Rhino.RhinoApp.WriteLine("                       A");
            //Rhino.RhinoApp.WriteLine((this.notchesTypes==null).ToString());
            copy.notchesTypes = new List<byte[]>(this.notchesTypes.Count);
            //Rhino.RhinoApp.WriteLine("B");
            for (int i = 0; i < this.notchesTypes.Count; i++) {
                //Rhino.RhinoApp.WriteLine(this.notchesTypes[i].Length.ToString());
                var bytes = new byte[this.notchesTypes[i].Length];
                for (int j = 0; j < this.notchesTypes[i].Length; j++) {
                    bytes[j] = this.notchesTypes[i][j];
                }
                copy.notchesTypes.Add(bytes);
            }
            //Rhino.RhinoApp.WriteLine("                       C");

            copy.filletR = this.filletR;
            copy.CutOrHole = this.CutOrHole;
            copy.PolylineMergeTakeOutside = this.PolylineMergeTakeOutside;
            copy.merge = this.merge;
            copy.SawFlip90Cut = this.SawFlip90Cut;

            copy.plinesBinary = plinesBinary.Duplicate();
            copy.plineReference = plineReference.Duplicate();
            copy.weight = this.weight;
            copy.projectRotate = this.projectRotate;
            //Rhino.RhinoApp.WriteLine(copy.weight.ToString());
            copy.DrillTwoSides = this.DrillTwoSides;

            //SawBlade
            copy.rotatable = this.rotatable;
            return copy;
        }

        public void Transform(Transform xform) {


            refPlane.Transform(xform);


            foreach (var g in this.planes)
                g.Transform(xform);

            foreach (var g in this.plines)
                g.Transform(xform);

            foreach (var g in this.plinesBinary)
                g.Transform(xform);

            plineReference.Transform(xform);

            foreach (var g in this.breps)
                g.Transform(xform);

            foreach (var g in this.meshes)
                g.Transform(xform);


            insertionVector.Transform(xform);

        }

        public Cut XForm(Transform xform) {

            var cut = this.Duplicate();
            cut.refPlane.Transform(xform);


            foreach (var g in cut.planes)
                g.Transform(xform);

            foreach (var g in cut.plines)
                g.Transform(xform);

            foreach (var g in cut.breps)
                g.Transform(xform);

            foreach (var g in cut.meshes)
                g.Transform(xform);

            foreach (var g in cut.plinesBinary)
                g.Transform(xform);

            cut.plineReference.Transform(xform);
            cut.insertionVector.Transform(xform);
            return cut;
        }

        public override string ToString()
        {
            return "Cut: " + cutType.ToString() + " Plines: " + plines.Count.ToString();// + " CutTypes: " + cutTypes.Count.ToString();
        }
        #endregion

    }
}
