using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeMaterialParameterValueCommand : AbstractCommand
    {
        ZMaterialParameter currentObject;
        string oldValue;
        string newValue;

        internal ChangeMaterialParameterValueCommand(ZMaterialParameter currentObject, string oldValue, string newValue)
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
            currentObject.Value = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Value = newValue;
        }
    }
}
