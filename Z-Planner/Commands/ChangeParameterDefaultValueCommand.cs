using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterDefaultValueCommand : AbstractCommand
    {
        ZParameter currentObject;
        string oldValue;
        string newValue;

        internal ChangeParameterDefaultValueCommand(ZParameter currentObject, string oldValue, string newValue)
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
            currentObject.DefaultValue = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.DefaultValue = newValue;
        }
    }
}
