using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class RemoveListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        int itemIndex;
        T currentItem;
        bool isEmpty;

        internal RemoveListItemCommand(ZList<T> currentObject, int itemIndex, T currentItem)
        {
            this.currentObject = currentObject;
            this.itemIndex = itemIndex;
            this.currentItem = currentItem;
            isEmpty = currentObject.Count == 0;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.Insert(itemIndex, currentItem);
            if (isEmpty)
                for (int i = currentObject.Count - 1; i >= 0; --i)
                {
                    if (i == itemIndex) continue;
                    currentObject.RemoveAt(i);
                }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Remove(currentItem);
        }
    }
}
