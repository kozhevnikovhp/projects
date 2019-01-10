using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class AddMaterialCommand : AbstractCommand
    {
        ZMaterial currentItem;

        internal AddMaterialCommand(ZMaterial currentItem)
        {
            this.currentItem = currentItem;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            if (!ZPlannerManager.IsUserHaveAccessToMaterial(currentItem)) return;
            ZPlannerManager.Dml.Materials.Remove(currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            if (!ZPlannerManager.IsUserHaveAccessToMaterial(currentItem)) return;
            ZPlannerManager.Dml.Materials.Add(currentItem);
        }
    }
}
