using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.Utils
{
    class Units
    {
        public const double MetersToMils = 100000 / 2.54;
        public const double MilsToMeters = 1 / MetersToMils;
        public const float fMetersToMils = 100000 / 2.54f;
        public const float fMilsToMeters = 1 / fMetersToMils;
        public const double fMilsToMikrons = 1000 * 1000 * fMilsToMeters;
        public const double fMikronsToMils = 1 / fMilsToMikrons;
        public const double fMilsToMillimeters = 1000 * fMilsToMeters;
        public const double fMillimetersToMils = 1 / fMilsToMillimeters;

        public static float fOzToMils 
        { 
            get 
            {
#if ZSANDBOX
                return (ZZero.ZSandbox.ZSandboxManager.IsTrapezoidalTraces) ? 1.2F : 1.3F;
#else
                return (ZPlannerManager.IsTrapezoidalTraces) ? 1.2F : 1.3F;
#endif
            }
        }

        public static float fMilToOz = 1 / fOzToMils;

        public static double RzToRMS = 1 / (2 * Math.Sqrt(3));
        public static double RMSToRz = 2 * Math.Sqrt(3);


        public const double MilimeterToMils = 100 / 2.54;
        public const double MilsToMilimeter = 1 / MilimeterToMils;
        public const double MicronToMils = 1 / 25.4;
        public const double MilToMicrons = 1 / MicronToMils;
        public const double InchToMils = 1000;
        public const double MilsToInch = 0.001;
    }
}
