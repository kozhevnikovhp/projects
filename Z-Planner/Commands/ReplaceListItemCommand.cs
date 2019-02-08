using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ReplaceListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        T oldValue;
        T newValue;
        int itemIndex;

        internal ReplaceListItemCommand(ZList<T> currentObject, int itemIndex, T oldValue, T newValue)
        {
            this.currentObject = currentObject;
            this.itemIndex = itemIndex;
            this.oldValue = oldValue;
            this.newValue = newValue;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject[itemIndex] = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject[itemIndex] = newValue;
        }
    }
}
