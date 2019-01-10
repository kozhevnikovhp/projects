using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ReplaceLayerCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<LayersCommandStructure> oldLayerStructureList = new List<LayersCommandStructure>();
        List<LayersCommandStructure> newLayerStructureList = new List<LayersCommandStructure>();

        internal ReplaceLayerCommand(ZStackup currentObject, List<LayersCommandStructure> oldLayerStructureList, List<LayersCommandStructure> newLayerStructureList)
        {
            this.currentObject = currentObject;
            this.oldLayerStructureList = oldLayerStructureList;
            this.newLayerStructureList = newLayerStructureList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            bool isAutoMirror = ZPlannerManager.IsAutoMirror;
            ZPlannerManager.IsAutoMirror = false;
            ZPlannerManager.SuspendCollectionChangedEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            int i = 0;
            try
            {
                // Clear Layer Selection
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

                foreach (LayersCommandStructure layerStructure in newLayerStructureList)
                {
                    currentObject.InternalRemoveLayer(layerStructure, false);
                }

                foreach (LayersCommandStructure layerStructure in oldLayerStructureList)
                {
                    currentObject.InternalInsertLayer(layerStructure, i==0);
                    ++i;
                }
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                //ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.ResumeCollectionChangedEvent();
                currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));

                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = isAutoMirror;
                ZPlannerManager.MainMenu.UpdateMenu();
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            bool isAutoMirror = ZPlannerManager.IsAutoMirror;
            ZPlannerManager.IsAutoMirror = false;
            ZPlannerManager.SuspendCollectionChangedEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            int i = 0;
            try
            {
                // Clear Layer Selection
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

                foreach (LayersCommandStructure layerStructure in oldLayerStructureList)
                {
                    currentObject.InternalRemoveLayer(layerStructure, false);
                }

                foreach (LayersCommandStructure layerStructure in newLayerStructureList)
                {
                    currentObject.InternalInsertLayer(layerStructure, i==0);
                    ++i;
                }
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                //ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.ResumeCollectionChangedEvent();
                currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));

                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = isAutoMirror;
                ZPlannerManager.MainMenu.UpdateMenu();
            }
        }
    }
}
