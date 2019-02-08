using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangePairImpedanceTargetCommand : AbstractCommand
    {
        ZPair currentObject;
        double oldValue;
        double newValue;

        internal ChangePairImpedanceTargetCommand(ZPair currentObject, double oldValue, double newValue)
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
