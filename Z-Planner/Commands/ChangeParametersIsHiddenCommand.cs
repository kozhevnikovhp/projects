using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParametersIsHiddenCommand : AbstractCommand
    {
        List<ZParameter> currentObject;
        bool oldValue;
        bool newValue;

        internal ChangeParametersIsHiddenCommand(List<ZParameter> currentObject, bool oldValue, bool newValue)
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
            foreach(ZParameter parameter in currentObject)
                parameter.IsHidden = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            foreach (ZParameter parameter in currentObject)
                parameter.IsHidden = newValue;
        }
    }
}
