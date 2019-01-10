﻿using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeLayerParameterIsEditedCommand : AbstractCommand
    {
        ZLayerParameter currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeLayerParameterIsEditedCommand(ZLayerParameter currentObject, bool oldValue, bool newValue)
        {
            this.currentObject = currentObject;
            this.oldValue = oldValue;
            this.newValue = newValue;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            currentObject.IsEdited = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.IsEdited = newValue;
        }
    }
}
