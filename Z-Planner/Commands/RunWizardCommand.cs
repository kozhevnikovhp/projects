using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.UI;

namespace ZZero.ZPlanner.Commands
{
    class RunWizardCommand : AbstractCommand
    {
        ZPlannerProject currentObject;

        internal RunWizardCommand(ZPlannerProject currentObject)
        {
            this.currentObject = currentObject;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            ZPlannerManager.ProjectFile = null;
            StackupWizardDialog wiz = StackupWizardDialog.CreateStackupWizardDialog();
            if (wiz != null) wiz.ShowDialog();
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            // No redo actions
        }
    }
}
