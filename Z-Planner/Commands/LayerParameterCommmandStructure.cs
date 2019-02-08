using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    internal struct LayerParameterCommmandStructure
    {
        internal ZLayerParameter LayerParameter;
        internal string OldValue;
        internal string NewValue;
        internal Dictionary<ZSpan, int> RemovedSpans;
    }
}
