using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class MirrorLayersCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<LayersCommandStructure> removeLayerStructureList;
        List<LayersCommandStructure> addLayerStructureList;

        internal MirrorLayersCommand(ZStackup currentObject, List<LayersCommandStructure> removeLayerStructureList, List<LayersCommandStructure> addLayerStructureList)
        {
            this.currentObject = currentObject;
            this.removeLayerStructureList = removeLayerStructureList;
            this.addLayerStructureList = addLayerStructureList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
            
            ZPlannerManager.SuspendCollectionChangedEvent();

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            for (int i = addLayerStructureList.Count - 1; i >= 0; --i)
            {
                currentObject.InternalRemoveLayer(addLayerStructureList[i], false);
            }

            for (int i = removeLayerStructureList.Count - 1; i >= 0 ; --i)
            {
                currentObject.InternalInsertLayer(removeLayerStructureList[i], i == 0);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset)); 
            
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
            ZPlannerManager.MainMenu.UpdateMenu();
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
            
            ZPlannerManager.SuspendCollectionChangedEvent();

            // Clear Layer Selection
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearRowSelection();

            for (int i = 0; i < removeLayerStructureList.Count; ++i)
            {
                currentObject.InternalRemoveLayer(removeLayerStructureList[i], false);
            }

            for (int i = 0; i < addLayerStructureList.Count; ++i)
            {
                currentObject.InternalInsertLayer(addLayerStructureList[i], i == addLayerStructureList.Count - 1);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = true;
            ZPlannerManager.MainMenu.UpdateMenu();
        }
    }
}
