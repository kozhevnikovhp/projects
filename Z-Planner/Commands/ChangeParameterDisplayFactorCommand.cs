using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterDisplayFactorCommand : AbstractCommand
    {
        ZParameter currentObject;
        double oldValue;
        double newValue;

        internal ChangeParameterDisplayFactorCommand(ZParameter currentObject, double oldValue, double newValue)
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
            currentObject.DisplayFactor = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.DisplayFactor = newValue;
        }
    }
}
