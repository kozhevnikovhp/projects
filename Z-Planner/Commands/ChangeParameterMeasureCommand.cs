using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeParameterMeasureCommand : AbstractCommand
    {
        ZParameter currentObject;
        ZMeasures oldValue;
        ZMeasures newValue;

        internal ChangeParameterMeasureCommand(ZParameter currentObject, ZMeasures oldValue, ZMeasures newValue)
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
            currentObject.Measure = oldValue;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Measure = newValue;
        }
    }
}
