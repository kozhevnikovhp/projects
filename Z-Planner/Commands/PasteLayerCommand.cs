using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class PasteLayerCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<LayersCommandStructure> layerStructureList;

        internal PasteLayerCommand(ZStackup currentObject, List<LayersCommandStructure> layerStructureList)
        {
            this.currentObject = currentObject;
            this.layerStructureList = layerStructureList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            ZPlannerManager.SuspendCollectionChangedEvent();

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            for (int i = layerStructureList.Count - 1; i >= 0; --i)
            {
                currentObject.InternalRemoveLayer(layerStructureList[i], i == 0);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            ZPlannerManager.SuspendCollectionChangedEvent();

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            for (int i = 0; i < layerStructureList.Count; ++i)
            {
                currentObject.InternalInsertLayer(layerStructureList[i], i == layerStructureList.Count - 1);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }
    }
}
