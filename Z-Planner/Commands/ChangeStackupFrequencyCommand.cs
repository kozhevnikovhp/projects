using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupFrequencyCommand : AbstractCommand
    {
        ZStackup currentObject;
        double oldValue;
        double newValue;

        internal ChangeStackupFrequencyCommand(ZStackup currentObject, double oldValue, double newValue)
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
            currentObject.Frequency = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Frequency = newValue;
        }
    }
}
