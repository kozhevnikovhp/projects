using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSingleImpedanceTargetCommand : AbstractCommand
    {
        ZSingle currentObject;
        double oldValue;
        double newValue;

        internal ChangeSingleImpedanceTargetCommand(ZSingle currentObject, double oldValue, double newValue)
        {
            this.currentObject = currentObject;
            this.oldValue = oldValue;
            this.newValue = newValue;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.ImpedanceTarget = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.ImpedanceTarget = newValue;
        }
    }
}