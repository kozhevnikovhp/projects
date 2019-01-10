using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.Commands
{
    internal struct RoughnessStructure
    {
        internal double OuterLayerRoughness;
        internal double LaminateSideRoughness;
        internal double PrepregSideRoughness;

        internal bool IsUsed;
    }
}
