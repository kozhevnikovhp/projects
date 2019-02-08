using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class RemoveCustomParameterCommand : AbstractCommand
    {
        ZParameter currentItem;
        int index;

        internal RemoveCustomParameterCommand(ZParameter currentItem, int index)
        {
            this.currentItem = currentItem;
            this.index = index;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            ZPlannerManager.Project.InternalCreateCustomParameter(currentItem, index);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            ZPlannerManager.Project.InternalRemoveCustomParameter(currentItem);
        }
    }
}
