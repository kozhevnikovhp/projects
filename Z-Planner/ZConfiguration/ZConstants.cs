using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.ZConfiguration
{
    class ZConstants
    {
        public const int WidthToCollapse = 16;
        public static List<ZLayerType?> CopperTypeList = new List<ZLayerType?>() { ZLayerType.Plane, ZLayerType.Signal, ZLayerType.SplitMixed };
    }
}
