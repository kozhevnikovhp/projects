using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class SwapListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        T firstItem;
        T secondItem;

        internal SwapListItemCommand(ZList<T> currentObject, T firstItem, T secondItem)
        {
            this.currentObject = currentObject;
            this.firstItem = firstItem;
            this.secondItem = secondItem;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.Swap(secondItem, firstItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Swap(firstItem, secondItem);
        }
    }
}
