using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeViaSpansCommand : AbstractCommand
    {
        ZList<ZSpan> currentObject;
        List<ZSpan> oldSpanList;
        List<ZSpan> newSpanList;

        internal ChangeViaSpansCommand(ZList<ZSpan> currentObject, List<ZSpan> oldSpanList, List<ZSpan> newSpanList)
        {
            this.currentObject = currentObject;
            this.oldSpanList = oldSpanList;
            this.newSpanList = newSpanList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            ZPlannerManager.SuspendCollectionChangedEvent();

            currentObject.Clear();

            currentObject.AddRange(oldSpanList);

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            ZPlannerManager.SuspendCollectionChangedEvent();

            currentObject.Clear();

            currentObject.AddRange(newSpanList);

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
    }
}
