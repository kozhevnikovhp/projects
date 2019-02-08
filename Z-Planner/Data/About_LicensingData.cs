using System;
using System.Diagnostics;
using System.Windows.Forms;
namespace ZZero.ZPlanner.Settings
{
    public class AboutData
    {
        private static AboutData about;
        private AboutData() 
        {
#if ZSANDBOX
            ZZero.ZSolver.ZSolverManager.GetProductVersionInfo(ref nVersion, ref nSubversion, ref nBuild, ref buildDate);
            ZZero.ZSolver.ZSolverManager.GetFSVersionInfo(ref nFSVersion, ref nFSSubversion, ref nFSBuild, ref FSBuildDate);
#else
            ZPlannerManager.GetProductVersionInfo(ref nVersion, ref nSubversion, ref nBuild, ref buildDate);
            ZPlannerManager.GetFSVersionInfo(ref nFSVersion, ref nFSSubversion, ref nFSBuild, ref FSBuildDate);
#endif
        }
        public static AboutData TheAbout
        {
            get
            {
                if (about == null)
                {
                    about = new AboutData();
                }
                return about;
            }
            set
            {
                if (about == null)
                {
                    about = new AboutData();
                }
                about = value;
            }
        }
        //----------------------------------------------------

        int nVersion = 1;
        int nSubversion = 0;
        int nBuild = 1;
        DateTime buildDate = new DateTime(2017, 6, 1);

        int nFSVersion = 9;
        int nFSSubversion = 4;
        int nFSBuild = 2;
        DateTime FSBuildDate = new DateTime(2017, 1, 12);

        public int Build 
        {
            get {
                return nBuild;
            }
            set
            {
                nBuild = value;
            }
        }
        public string sVersion
        {
            get
            {
#if ZSANDBOX
                return String.Format("Z-solver v{0}.{1}", nVersion, nSubversion);
#else
                return String.Format("Z-planner v{0}.{1}", nVersion, nSubversion);
#endif
            }
        }
        public string sBuild 
        {
            get
            {
                return String.Format("[64 bit, build {0}]", nBuild);
            }
        }

        public string sDate
        {
            get
            {
                return buildDate.ToString("MMMM d, yyyy");
            }
        }

        public string sProgramLocation
        {
            get
            {
                return System.IO.Path.GetDirectoryName(Application.ExecutablePath);
            }
        }

        //FS
        public string sFSVersion
        {
            get
            {
                return String.Format("HyperLynx Field Solver v{0}.{1}", nFSVersion, nFSSubversion);
            }
        }
        public string sFSBuild
        {
            get
            {
                return String.Format("[64 bit, build {0}]", nFSBuild);
            }
        }

        public string sFSDate
        {
            get
            {
                return FSBuildDate.ToString("MMMM d, yyyy");
            }
        }


    } 

}