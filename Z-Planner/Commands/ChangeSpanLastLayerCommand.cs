using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeSpanLastLayerCommand : AbstractCommand
    {
        ZSpan currentObject;
        ZLayer oldValue;
        ZLayer newValue;

        internal ChangeSpanLastLayerCommand(ZSpan currentObject, ZLayer oldValue, ZLayer newValue)
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
            currentObject.LastLayer = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.LastLayer = newValue;
        }
    }
}
