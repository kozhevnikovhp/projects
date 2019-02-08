using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterIsHiddenCommand : AbstractCommand
    {
        ZParameter currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeParameterIsHiddenCommand(ZParameter currentObject, bool oldValue, bool newValue)
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
            currentObject.IsHidden = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.IsHidden = newValue;
        }
    }
}
