using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupActivePairCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZPair oldValue;
        ZPair newValue;

        internal ChangeStackupActivePairCommand(ZStackup currentObject, ZPair oldValue, ZPair newValue)
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
            currentObject.ActivePair = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.ActivePair = newValue;
        }
    }
}
