using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class MoveListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        T currentItem;
        int newIndex;
        int oldIndex;

        internal MoveListItemCommand(ZList<T> currentObject, T currentItem, int oldIndex, int newIndex)
        {
            this.currentObject = currentObject;
            this.currentItem = currentItem;
            this.oldIndex = oldIndex;
            this.newIndex = newIndex;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.MoveAt(currentItem, oldIndex);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.MoveAt(currentItem, newIndex);
        }
    }
}
