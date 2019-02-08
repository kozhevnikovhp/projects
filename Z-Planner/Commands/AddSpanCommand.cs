using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class AddSpanCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZSpan currentItem;
        int index;

        internal AddSpanCommand(ZStackup currentObject, int index, ZSpan currentItem)
        {
            this.currentObject = currentObject;
            this.index = index;
            this.currentItem = currentItem;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.Spans.Remove(currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Spans.Insert(index, currentItem);
        }
    }
}
