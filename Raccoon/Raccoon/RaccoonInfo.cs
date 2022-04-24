using Grasshopper.Kernel;
using System;
using System.Drawing;

namespace Raccoon
{
    public class RaccoonInfo : GH_AssemblyInfo
    {
        public override string Name
        {
            get
            {
                return "Raccoon";
            }
        }

        public override Bitmap Icon
        {
            get
            {
                //Return a 24x24 pixel bitmap to represent this GHA library.
                return Properties.Resources.simulation;
            }
        }

        public override string Description
        {
            get
            {
                //Return a short string describing the purpose of this GHA library.
                return "Grasshopper plug-in for IBOIS CNC machine";
            }
        }

        public override Guid Id
        {
            get
            {
                return new Guid("7da10181-1c7b-4eeb-8221-b5aa4807c5b4");
            }
        }

        public override string AuthorName
        {
            get
            {
                //Return a string identifying you or your company.
                return "Petras Vestartas";
            }
        }

        public override string AuthorContact
        {
            get
            {
                //Return a string representing your preferred contact details.
                return "petrasvestartas@gmail.com";
            }
        }
    }
}