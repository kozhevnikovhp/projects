using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    internal struct PressedThicknessStructure
    {
        internal ZCopperCoverageType CopperCoverageType;
        internal double ForSignalSignal;
        internal double ForSignalMixed;
        internal double ForSignalPlane;
        internal double ForMixedMixed;
        internal double ForMixedPlane;
        internal double ForPlanePlane;

        internal double ForSignal;
        internal double ForMixed;
        internal double ForPlane;

        internal bool IsSequentialLamination;
        internal bool IsKeepImportedPressedThickness;
        internal bool IsPressedThickness;
    }
}
