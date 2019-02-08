using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterDisplayMeasureCommand : AbstractCommand
    {
        ZParameter currentObject;
        ZMeasures oldValue;
        ZMeasures newValue;

        internal ChangeParameterDisplayMeasureCommand(ZParameter currentObject, ZMeasures oldValue, ZMeasures newValue)
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
            currentObject.DisplayMeasure = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.DisplayMeasure = newValue;
        }
    }
}
