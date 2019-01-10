using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupActiveSingleCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZSingle oldValue;
        ZSingle newValue;

        internal ChangeStackupActiveSingleCommand(ZStackup currentObject, ZSingle oldValue, ZSingle newValue)
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
            currentObject.ActiveSingle = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.ActiveSingle = newValue;
        }
    }
}
