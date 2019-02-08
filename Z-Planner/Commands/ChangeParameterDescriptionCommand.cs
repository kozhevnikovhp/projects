using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterDescriptionCommand : AbstractCommand
    {
        ZParameter currentObject;
        string oldValue;
        string newValue;

        internal ChangeParameterDescriptionCommand(ZParameter currentObject, string oldValue, string newValue)
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
            currentObject.Description = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Description = newValue;
        }
    }
}
