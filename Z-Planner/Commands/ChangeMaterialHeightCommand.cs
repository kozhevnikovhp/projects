using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeMaterialHeightCommand : AbstractCommand
    {
        ZMaterial currentObject;
        int oldValue;
        int newValue;

        internal ChangeMaterialHeightCommand(ZMaterial currentObject, int oldValue, int newValue)
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
            currentObject.Height = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Height = newValue;
        }
    }
}
