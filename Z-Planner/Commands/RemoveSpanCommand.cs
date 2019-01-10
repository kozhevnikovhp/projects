using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class RemoveSpanCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZSpan currentItem;
        int index;

        internal RemoveSpanCommand(ZStackup currentObject, int index, ZSpan currentItem)
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
            currentObject.Spans.Insert(index, currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Spans.Remove(currentItem);
        }
    }
}
