using System;
using System.Collections.Generic;
using Grasshopper;
using Grasshopper.Kernel;
using Grasshopper.Kernel.Data;
using Grasshopper.Kernel.Types;
using Rhino.Geometry;

namespace Raccoon.Components.CNC
{
    public class ComponentMerge : CustomComponent, IGH_VariableParameterComponent
    {

        public ComponentMerge()
          : base("Merge", "Merge", "Merge several G-Codes into one","Util")
        {
        }

        public override GH_Exposure Exposure => GH_Exposure.tertiary;


        protected override void RegisterInputParams(GH_Component.GH_InputParamManager pManager)
        {
            pManager.AddNumberParameter("BAxis", "BAxis", "BAxis", GH_ParamAccess.list);
            pManager.AddTextParameter("GCode", "GCode", "G-Code as DataTree", GH_ParamAccess.tree);
            //pManager.AddTextParameter("Filename", "Filename", "Filename", GH_ParamAccess.item, "P1234567");


            for (int i = 2; i < pManager.ParamCount; i++)
                pManager[i].Optional = true;
            pManager[0].Optional = true;
        }



        protected override void RegisterOutputParams(GH_Component.GH_OutputParamManager pManager)
        {

          
            pManager.AddTextParameter("GCode", "GCode", "GCode", GH_ParamAccess.list);
        }

        protected override void SolveInstance(IGH_DataAccess DA)
        {
         
            var angles = new List<double>();
            DA.GetDataList(0, angles);
         
            this.GCode = new List<string>();
            var dt = new GH_Structure<GH_String>();
            DA.GetDataTree(1, out dt);

        
            var y = new GH_Structure<GH_String>();
            for (int i = 2; i < Params.Input.Count; i++)
            {

                var moreStrings = new GH_Structure<GH_String>();
                DA.GetDataTree(i, out moreStrings);
                y.MergeStructure(moreStrings);
            }


            var x = new GH_Structure<GH_String>();
            x.MergeStructure(dt);
            x.MergeStructure(y);


            // var name = "P1234567";
            //DA.GetData<string>(1, ref name);


      
            var t = new List<string>();
            t.Add(this.filename);
            int counter = 10;
            int replace_counter = 0;

            Dictionary<string, bool> repeated_values = new Dictionary<string, bool>();
            repeated_values.Add("M5", false);
            repeated_values.Add("M30", false);
            repeated_values.Add("(endpos)", false);
            repeated_values.Add("(g49_means_5axis_toolpath_-_startpos)", true);
            
            for (int i = 0; i < x.Branches.Count; i++)
            {

                for (int j = 0; j < x[i].Count; j++)
                {
                    string s = x[i][j].Value;

                    if (s[0] == 'N')
                    {


                        string[] words = s.Split(' ');
                        string newWord = "N" + counter.ToString();

                        bool insert = true;
                        for (int k = 1; k < words.Length; k++)
                        {

                            if (repeated_values.ContainsKey(words[k])) {
                                //Rhino.RhinoApp.WriteLine(words[k]);
                                if (repeated_values[words[k]])
                                {
                                    newWord += (" " + words[k]);
                                    repeated_values[words[k]] = false;
                                }
                                else
                                {
                                    insert = false;
                                    break;
                                }

                            } else if (words[k] == "(ReplaceB)" && angles.Count > replace_counter*0.5)
                            {
                                //
                                //int oddEven = replace_counter % 2 == 0 ? 1 : -1;
                                // words[k] = "B"+(angles[(int)(replace_counter*0.5)]* oddEven).ToString();
                                words[k] = "B" + (angles[(int)(replace_counter )] ).ToString();
                                replace_counter++;
                             }
                        

                                newWord += (" " + words[k]);
                          

                        }

                        if (insert)
                        {
                            t.Add(newWord);
                            counter += 10;
                        }

                    }
                    else if (s[0] == '(' && s[1] == '*')
                    {
                        t.Add(s);
                    }

                }

            }
            t.Add("N" + (counter + 10).ToString() + " G0" + Raccoon.GCode.Axes.HomePosition2 + Raccoon.GCode.Axes.DefaultRotation + " (end pos)");
            //t.Add("N" + (counter + 0).ToString() + " G0 X0 Y3500 Z400 A0 B0");
            t.Add("N" + (counter + 20).ToString() + " M5");
            t.Add("N" + (counter + 30).ToString() + " M30");
            t.Add("#");

            this.GCode = t;
            DA.SetDataList(0, t);


        }


        //#region Methods of IGH_VariableParameterComponent interface

        bool IGH_VariableParameterComponent.CanInsertParameter(GH_ParameterSide side, int index)
        {
            //We only let input parameters to be added (output number is fixed at one)
            if (side == GH_ParameterSide.Input)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        bool IGH_VariableParameterComponent.CanRemoveParameter(GH_ParameterSide side, int index)
        {
            //We can only remove from the input
            if (side == GH_ParameterSide.Input && Params.Input.Count > 0)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        IGH_Param IGH_VariableParameterComponent.CreateParameter(GH_ParameterSide side, int index)
        {
            //Param_Plane param = new Param_Plane();
            Grasshopper.Kernel.Parameters.Param_String param = new Grasshopper.Kernel.Parameters.Param_String();
            param.Access = GH_ParamAccess.tree;
            param.Optional = true;
            param.Name = "GCode";
            param.NickName = param.Name;
            param.Description = "GCode" + (Params.Input.Count + 1);

            return param;
        }

        bool IGH_VariableParameterComponent.DestroyParameter(GH_ParameterSide side, int index)
        {
            //Nothing to do here by the moment
            return true;
        }

        void IGH_VariableParameterComponent.VariableParameterMaintenance()
        {
            //Nothing to do here by the moment
        }


        protected override System.Drawing.Bitmap Icon
        {
            get
            {
                return Properties.Resources.merge;
            }
        }
        /// <summary>
        /// Gets the unique ID for this component. Do not change this ID after release.
        /// </summary>
        public override Guid ComponentGuid => new Guid("1a2be664-fe1f-1d3b-b01f-44a78c481244");

        protected override void AfterSolveInstance()
        {




            //GH_Document ghdoc = base.OnPingDocument();
            //for (int i = 0; i < ghdoc.ObjectCount; i++)
            //{
            //    IGH_DocumentObject obj = ghdoc.Objects[i];
            //    if (obj.Attributes.DocObject.ToString().Equals("Grasshopper.Kernel.Special.GH_Group"))
            //    {
            //        Grasshopper.Kernel.Special.GH_Group groupp = (Grasshopper.Kernel.Special.GH_Group)obj;
            //        if (groupp.ObjectIDs.Contains(this.InstanceGuid))
            //            return;
            //    }

            //}


            //List<Guid> guids = new List<Guid>() { this.InstanceGuid };

            //foreach (var param in base.Params.Input)
            //    foreach (IGH_Param source in param.Sources)
            //        guids.Add(source.InstanceGuid);


            //Grasshopper.Kernel.Special.GH_Group g = new Grasshopper.Kernel.Special.GH_Group();
            //g.NickName = base.Name.ToString() + " Click + or -";
            //g.Colour = System.Drawing.Color.FromArgb(255, 255, 255, 255);

            //for (int i = 0; i < guids.Count; i++)
            //    g.AddObject(guids[i]);

            //ghdoc.AddObject(g, false, ghdoc.ObjectCount);
            //g.ExpireCaches();




        }
    }
}