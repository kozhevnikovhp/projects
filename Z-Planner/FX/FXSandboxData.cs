using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.FXInterface
{
    enum csType {SurfaceMicrostrip, BuriedMicrostrip, Stripline }
    class sbData
    {
        /*
         Solder Mask thickness (c)
Dk (sm)
Df (sm)*/
        public sbData(csType aType) //from defaults
        {
            type = aType;

            //get defaults from Options
            w = w2 = Options.TheOptions.outer_trace_width; //conductor width [mils]
            h1 = Options.TheOptions.height;
            h2 = h1; //dielectric height [mils]
            h2_2 = h1; //for solder mask (h2 - above trace, h2_2 between traces)
            Dk1 = Options.TheOptions.Dk;
            Dk2 = Dk1; //dielectric constant
            Dkr = Options.TheOptions.resin_Dk;
            Df1 = Options.TheOptions.Df;
            Df2 = Df1;//loss tangent
            Dfr = Options.TheOptions.resin_Df;

            if (type == csType.SurfaceMicrostrip)
            {
#if ZSANDBOX
                t = ZZero.ZSolver.ZSolverManager.GetThicknesByCopperWeight(Options.TheOptions.base_trace_thickness + Options.TheOptions.plating_thickness); //[oz] - conductor thickness [mils]
#else
                t = ZPlannerManager.GetThicknesByCopperWeight(Options.TheOptions.base_trace_thickness + Options.TheOptions.plating_thickness); //[oz] - conductor thickness [mils]
#endif
                h2 = Options.TheOptions.soldermask_height;
                h2_2 = h2;
                Dk2 = Options.TheOptions.soldermask_Dk;
                Df2 = Options.TheOptions.soldermask_Df;
            }
            else
            {
#if ZSANDBOX
                t = ZZero.ZSolver.ZSolverManager.GetThicknesByCopperWeight(Options.TheOptions.copper_foil_thickness); //[oz] - conductor thickness [mils]
#else
                t = ZPlannerManager.GetThicknesByCopperWeight(Options.TheOptions.copper_foil_thickness); //[oz] - conductor thickness [mils]
#endif
            }
            s = Options.TheOptions.trace_spacing; //diff. spacing [mils]
            Rz1 = Rz2 = Options.TheOptions.roughness;//um to meters

            len = 1;//inch
            f = 1; //GHz
            delay = 0; //ns
            loss = 0; //dB
            etch = Options.TheOptions.etch;
        }

        public csType type;
        public bool hasSolderMask = false; //for SurfaceMicrostrip only
// fields: general defaults
#if ZSANDBOX
        public double t = ZZero.ZSolver.ZSolverManager.GetThicknesByCopperWeight(0.5); //[oz] - conductor thickness [mils]
#else
        public double t = ZPlannerManager.GetThicknesByCopperWeight(0.5); //[oz] - conductor thickness [mils]
#endif
        public double w = 5, w2 = 5; //conductor width [mils], w2 = top width (etching effect)
        public double h1 = 5, h2 = 5, h2_2 = 5; //dielectric height [mils]
        public double Dk1 = 4, Dk2 = 4, Dkr = 4.0; //dielectric constant
        public double Df1 = 0.018, Df2 = 0.018, Dfr = 0.018;//loss tangent
        public double s = 10; //diff. spacing [mils]
        public double Rz1 = 3.5, Rz2 = 3.5; //roughness [um]

        public double Z0 = 0, Z0diff = 0, Zdiff = 0;
        //losses data
        public double R1 = 0, G1 = 0, G0 = 0, RRough1 = 0; //fSkinResistance(1GHz), fGMatrix(1GHz), fGzero, RRoughness(1GHz)
        public double R1diff = 0, G1diff = 0, G0diff = 0, RRough1diff = 0; //fSkinResistance(1GHz), fGMatrix(1GHz), fGzero, RRoughness(1GHz)

        public FX_Output outSE = null, outDiff = null;

        // delay & loss
        public double len = 1; //inch
        public double f = 1; //GHz
        public double delay = 0; //ns
        public double loss = 0;//dB
        
        // etch effects
        public double etch = 0;
        public bool isEtch = false;
        public bool isSymmetrical = true;
        public bool isDiffImpedance = false;

// static instances
        private static sbData csSurface, csBuried, csStripline;

        public static sbData SurfaceMicrostrip
        {
            get{
                if (csSurface == null)
                {
                    csSurface = new sbData(csType.SurfaceMicrostrip);
                }
                return csSurface;
            }
            set
            {
                csSurface = value;
            }
        }
        public static sbData BuriedMicrostrip
        {
            get
            {
                if (csBuried == null)
                {
                    csBuried = new sbData(csType.BuriedMicrostrip);
                }
                return csBuried;
            }
            set
            {
                csBuried = value;
            }
        }
        public static sbData Stripline
        {
            get
            {
                if (csStripline == null)
                {
                    csStripline = new sbData(csType.Stripline);
                }
                return csStripline;
            }
            set
            {
                csStripline = value;
            }
        }
    }

}