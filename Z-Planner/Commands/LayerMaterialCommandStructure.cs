using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    internal struct LayerMaterialCommandStructure
    {
        internal string MaterialParameterValue;

        internal string LayerParameterValue;

        internal ZLayerParameter LayerParameter;

        internal bool IsMaterialAssigned;
    }
}
