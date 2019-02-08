using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class RemoveRangeListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        IEnumerable<T> itemCollection;

        internal RemoveRangeListItemCommand(ZList<T> currentObject, IEnumerable<T> itemCollection)
        {
            this.currentObject = currentObject;
            this.itemCollection = itemCollection;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            foreach (var item in itemCollection)
            {
                currentObject.AddRange(itemCollection);
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.RemoveRange(itemCollection);
        }
    }
}
