using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterWidthToExpandCommand : AbstractCommand
    {
        ZParameter currentObject;
        int oldValue;
        int newValue;

        internal ChangeParameterWidthToExpandCommand(ZParameter currentObject, int oldValue, int newValue)
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
            currentObject.WidthToExpand = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.WidthToExpand = newValue;
        }
    }
}
