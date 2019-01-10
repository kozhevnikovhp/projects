using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class MoveLayerCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<MoveCommandStructure> moveStructureList;
        bool isCoreLocked;

        internal MoveLayerCommand(ZStackup currentObject, List<MoveCommandStructure> moveStructureList, bool isCoreLocked = false)
        {
            this.currentObject = currentObject;
            this.moveStructureList = moveStructureList;
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

            for (int i = moveStructureList.Count - 1; i >= 0; --i)
            {
                MoveCommandStructure moveStructure = new MoveCommandStructure();
                moveStructure.Id = moveStructureList[i].Id;
                moveStructure.SourceIndex = moveStructureList[i].DestinIndex;
                moveStructure.DestinIndex = moveStructureList[i].SourceIndex;
                currentObject.InternalMoveLayer(moveStructure);
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

            for (int i = 0; i < moveStructureList.Count; ++i)
            {
                currentObject.InternalMoveLayer(moveStructureList[i]);
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
