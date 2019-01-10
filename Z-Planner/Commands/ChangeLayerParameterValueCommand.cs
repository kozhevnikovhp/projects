using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeLayerParameterValueCommand : AbstractCommand
    {
        ZLayerParameter currentObject;
        string oldValue;
        string newValue;
        bool oldIsEdited;
        bool newIsEdited;

        internal ChangeLayerParameterValueCommand(ZLayerParameter currentObject, string oldValue, string newValue, bool oldIsEdited, bool newIsEdited)
        {
            this.currentObject = currentObject;
            this.oldValue = oldValue;
            this.newValue = newValue;
            this.oldIsEdited = oldIsEdited;
            this.newIsEdited = newIsEdited;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.Value = oldValue;
            currentObject.IsEdited = oldIsEdited;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Value = newValue;
            currentObject.IsEdited = newIsEdited;
        }
    }
}
