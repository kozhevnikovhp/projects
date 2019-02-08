using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterIsCustomCommand : AbstractCommand
    {
        ZParameter currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeParameterIsCustomCommand(ZParameter currentObject, bool oldValue, bool newValue)
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
            currentObject.IsCustom = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.IsCustom = newValue;
        }
    }
}
