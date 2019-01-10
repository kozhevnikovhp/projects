using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    internal struct LayersCommandStructure
    {
        internal string Id;
        internal int Index;
        internal ZLayer StackupLayer;
        internal Dictionary<string, ZLayer> SingleLayers;
        internal Dictionary<string, ZLayer> PairLayers;
        internal Dictionary<ZSingle, int> RemovedSingles;
        internal Dictionary<ZPair, int> RemovedPairs;
        internal Dictionary<ZSpan, int> RemovedSpans;
    }
}
