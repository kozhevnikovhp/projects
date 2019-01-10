using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeLayerParameterIsReadOnlyCommand : AbstractCommand
    {
        ZLayerParameter currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeLayerParameterIsReadOnlyCommand(ZLayerParameter currentObject, bool oldValue, bool newValue)
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
            //currentObject.IsReadOnly = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            //currentObject.IsReadOnly = newValue;
        }
    }
}
