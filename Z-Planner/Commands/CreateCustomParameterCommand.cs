using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class CreateCustomParameterCommand : AbstractCommand
    {
        ZParameter currentItem;
        int index;

        internal CreateCustomParameterCommand(ZParameter currentItem, int index)
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
            ZPlannerManager.Project.InternalRemoveCustomParameter(currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            ZPlannerManager.Project.InternalCreateCustomParameter(currentItem, index);
        }
    }
}
