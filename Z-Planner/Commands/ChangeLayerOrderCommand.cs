using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeLayerOrderCommand : AbstractCommand
    {
        ZLayer currentObject;
        int oldValue;
        int newValue;

        internal ChangeLayerOrderCommand(ZLayer currentObject, int oldValue, int newValue)
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
            currentObject.Order = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Order = newValue;
        }
    }
}
