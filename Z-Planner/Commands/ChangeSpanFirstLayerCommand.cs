using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSpanFirstLayerCommand : AbstractCommand
    {
        ZSpan currentObject;
        ZLayer oldValue;
        ZLayer newValue;

        internal ChangeSpanFirstLayerCommand(ZSpan currentObject, ZLayer oldValue, ZLayer newValue)
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
            currentObject.FirstLayer = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.FirstLayer = newValue;
        }
    }
}
