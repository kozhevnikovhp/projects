using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class InsertListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        int itemIndex;
        T currentItem;

        internal InsertListItemCommand(ZList<T> currentObject, int itemIndex, T currentItem)
        {
            this.currentObject = currentObject;
            this.itemIndex = itemIndex;
            this.currentItem = currentItem;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.RemoveAt(itemIndex);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Insert(itemIndex, currentItem);
        }
    }
}
