using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSpanAspectRatioCommand : AbstractCommand
    {
        ZSpan currentObject;
        double oldValue;
        double newValue;

        internal ChangeSpanAspectRatioCommand(ZSpan currentObject, double oldValue, double newValue)
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
            currentObject.AspectRatio = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.AspectRatio = newValue;
        }
    }
}
