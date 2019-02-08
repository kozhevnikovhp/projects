using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeColumnValuesCommand : AbstractCommand
    {
        List<ZLayerParameter> currentObjects;
        string oldValue;
        string newValue;

        internal ChangeColumnValuesCommand(List<ZLayerParameter> currentObjects, string oldValue, string newValue)
        {
            this.currentObjects = currentObjects;
            this.oldValue = oldValue;
            this.newValue = newValue;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            foreach (ZLayerParameter layerParameter in currentObjects)
            {
                layerParameter.Value = oldValue;
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            foreach (ZLayerParameter layerParameter in currentObjects)
            {
                layerParameter.Value = newValue;
            }
        }
    }
}
