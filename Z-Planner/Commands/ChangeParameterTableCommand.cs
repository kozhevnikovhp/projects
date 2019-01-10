using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterTableCommand : AbstractCommand
    {
        ZParameter currentObject;
        ZTableType oldValue;
        ZTableType newValue;

        internal ChangeParameterTableCommand(ZParameter currentObject, ZTableType oldValue, ZTableType newValue)
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
            currentObject.Table = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Table = newValue;
        }
    }
}
