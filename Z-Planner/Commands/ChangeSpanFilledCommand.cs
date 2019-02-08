using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSpanFilledCommand : AbstractCommand
    {
        ZSpan currentObject;
        string oldValue;
        string newValue;

        internal ChangeSpanFilledCommand(ZSpan currentObject, string oldValue, string newValue)
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
            currentObject.Filled = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Filled = newValue;
        }
    }
}
