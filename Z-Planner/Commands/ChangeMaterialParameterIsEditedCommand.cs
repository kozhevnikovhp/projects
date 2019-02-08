using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeMaterialParameterIsEditedCommand : AbstractCommand
    {
        ZMaterialParameter currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeMaterialParameterIsEditedCommand(ZMaterialParameter currentObject, bool oldValue, bool newValue)
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
            currentObject.IsEdited = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.IsEdited = newValue;
        }
    }
}
