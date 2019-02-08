using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeEntityTitleCommand : AbstractCommand
    {
        ZEntity currentObject;
        string oldValue;
        string newValue;

        internal ChangeEntityTitleCommand(ZEntity currentObject, string oldValue, string newValue)
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
            currentObject.Title = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Title = newValue;
        }
    }
}
