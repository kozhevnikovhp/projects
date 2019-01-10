using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterListCommand : AbstractCommand
    {
        ZParameter currentObject;
        SelectList oldValue;
        SelectList newValue;

        internal ChangeParameterListCommand(ZParameter currentObject, SelectList oldValue, SelectList newValue)
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
            currentObject.List = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.List = newValue;
        }
    }
}
