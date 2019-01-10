using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class AddRangeListItemCommand<T> : AbstractCommand
        where T : ZEntity
    {
        ZList<T> currentObject;
        IEnumerable<T> itemCollection;

        internal AddRangeListItemCommand(ZList<T> currentObject, IEnumerable<T> itemCollection)
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
            currentObject.RemoveRange(itemCollection);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.AddRange(itemCollection);
        }
    }
}
