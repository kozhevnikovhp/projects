using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class SwapLayersCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<SwapCommandStructure> swapStructureList;
        bool isCoreLocked;

        internal SwapLayersCommand(ZStackup currentObject, List<SwapCommandStructure> swapStructureList, bool isCoreLocked = false)
        {
            this.currentObject = currentObject;
            this.swapStructureList = swapStructureList;
            this.isCoreLocked = isCoreLocked;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            Dictionary<string, LayersCommandStructure> underList = new Dictionary<string, LayersCommandStructure>();
            Dictionary<string, LayersCommandStructure> overList = new Dictionary<string, LayersCommandStructure>();

            ZPlannerManager.SuspendCollectionChangedEvent();

            if (isCoreLocked)
            {
                currentObject.CollapseLockedLayers(underList, overList);
            }

            for (int i = swapStructureList.Count - 1; i >= 0; --i)
            {
                currentObject.InternalSwapLayers(swapStructureList[i]);
            }

            if (isCoreLocked)
            {
                currentObject.ExpandLockedLayers(underList, overList);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            Dictionary<string, LayersCommandStructure> underList = new Dictionary<string, LayersCommandStructure>();
            Dictionary<string, LayersCommandStructure> overList = new Dictionary<string, LayersCommandStructure>();

            ZPlannerManager.SuspendCollectionChangedEvent();

            if (isCoreLocked)
            {
                currentObject.CollapseLockedLayers(underList, overList);
            }

            for (int i = 0; i < swapStructureList.Count; ++i)
            {
                currentObject.InternalSwapLayers(swapStructureList[i]);
            }

            if (isCoreLocked)
            {
                currentObject.ExpandLockedLayers(underList, overList);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
    }
}
