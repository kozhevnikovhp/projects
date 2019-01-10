using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSpanBackDrilledCommand : AbstractCommand
    {
        ZSpan currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeSpanBackDrilledCommand(ZSpan currentObject, bool oldValue, bool newValue)
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
            currentObject.IsBackDrilled = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.IsBackDrilled = newValue;
        }
    }
}
