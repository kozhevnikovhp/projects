using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterWidthCommand : AbstractCommand
    {
        ZParameter currentObject;
        int oldValue;
        int newValue;

        internal ChangeParameterWidthCommand(ZParameter currentObject, int oldValue, int newValue)
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
            currentObject.Width = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Width = newValue;
        }
    }
}
