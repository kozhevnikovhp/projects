using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterValueTypeCommand : AbstractCommand
    {
        ZParameter currentObject;
        ZValueType oldValue;
        ZValueType newValue;

        internal ChangeParameterValueTypeCommand(ZParameter currentObject, ZValueType oldValue, ZValueType newValue)
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
            currentObject.ValueType = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.ValueType = newValue;
        }
    }
}
