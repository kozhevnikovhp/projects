﻿using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class RemovePairCommand : AbstractCommand
    {
        ZStackup currentObject;
        ZPair currentItem;
        int index;

        internal RemovePairCommand(ZStackup currentObject, int index, ZPair currentItem)
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
            currentObject.Pairs.Insert(index, currentItem);
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Pairs.Remove(currentItem);
        }
    }
}
