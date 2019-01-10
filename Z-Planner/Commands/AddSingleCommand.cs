using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class AddSingleCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZSingle currentItem;
        int index;

        internal AddSingleCommand(ZStackup currentObject, int index, ZSingle currentItem)
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
            currentObject.Singles.Remove(currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Singles.Insert(index, currentItem);
        }
    }
}
