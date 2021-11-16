using Grasshopper.Kernel;
using Rhino.Geometry;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Raccoon.Components.CNC
{
    public class CutModel : CustomComponent
    {

        public CutModel()
          : base("Cut", "Cut", "Cut", "Robot/CNC")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.quinary;

        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddCurveParameter("cut_polyline", " cut_polyline", "cut_polyline", GH_ParamAccess.item);
            pManager.AddCurveParameter("dir_polyline", " dir_polyline", "dir_polyline", GH_ParamAccess.item);

            pManager.AddNumberParameter("cut_type", "cut_type", "Cut = 0, Mill = 1, Drill = 2, SawBlade = 3,  \n SawBladeBisector = 4, Engrave = 5, MillPath = 6, MillCircular = 7,  \n SawCircular = 8, SawEnd = 9, Slice = 10, SawBladeSlice = 11,  OpenCut = 12", GH_ParamAccess.item);
            pManager.AddNumberParameter("notches_types", "notches_types", "1-bisector 2-translation 3-translation opposite 4-4 rounded outline", GH_ParamAccess.list);
            pManager.AddNumberParameter("fillet_radius", "fillet_radius", "fillet_radius", GH_ParamAccess.item);

            pManager.AddBooleanParameter("project", "project", "project", GH_ParamAccess.item);
            pManager.AddBooleanParameter("project_rotate", "project_rotate", "project_rotate", GH_ParamAccess.item);
            pManager.AddBooleanParameter("saw_flip_90Cut", "project", "project", GH_ParamAccess.item);

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

            int[] recID = new int[] { 0,1 };

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


            //    //Add sliders


            double[] sliderValue = new double[] {12, 2, 10 };
            double[] sliderMinValue = new double[] { 0, 0, 0 };
            double[] sliderMaxValue = new double[] { 12, 4, 100};
            int[] sliderID = new int[] {2, 3, 4};
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
            bool[] boolValue = new bool[] { false, false, false };
            int[] boolID = new int[] { 5, 6, 7 };

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
            pManager.AddGenericParameter("Cut", "Cut", "Cut", GH_ParamAccess.item);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {

            

            int id = this.RunCount;

            Polyline cutPolyline = new Polyline();
            Curve cutPolyline_ = null ; 
            DA.GetData(0,ref cutPolyline_);
            cutPolyline_.TryGetPolyline(out cutPolyline);

            Polyline normal = new Polyline();
            Curve normal_ = null;
            DA.GetData(1, ref normal_);
            normal_.TryGetPolyline(out normal);

            CutType cutType = CutType.OpenCut;
            double cutType_=0;
            DA.GetData(2, ref cutType_);
            cutType = (CutType)cutType_;


            var notchesTypes_ = new List<double>();
            DA.GetDataList(3,  notchesTypes_);
            byte[] notchesTypes = new byte[notchesTypes_.Count];//1-bisector 2-translation 3-translation opposite 4-4 rounded outline
           for (int i = 0; i < notchesTypes_.Count; i++)
               notchesTypes[i] = Convert.ToByte((int)notchesTypes_[i]);



            double filletR = 0;
            DA.GetData(4,ref filletR);

            bool project = false;
            DA.GetData(5, ref project);

            bool CutOrHole = true;//not cutting
            bool PolylineMergeTakeOutside = false;//not cutting
            bool merge = false;//not cutting

            bool projectRotate = false;
            DA.GetData(6, ref projectRotate);

            bool SawFlip90Cut = false;
            DA.GetData(7, ref SawFlip90Cut);

            Cut cut = new Cut(id, cutPolyline, normal, cutType, project, notchesTypes, filletR, CutOrHole, PolylineMergeTakeOutside, merge, projectRotate, SawFlip90Cut);
            DA.SetData(0,cut);

        }



        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.Cuts;
            }
        }
        public override Guid ComponentGuid => new Guid("1a2be114-fe1f-4d3b-b01f-11a78c487256");

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
