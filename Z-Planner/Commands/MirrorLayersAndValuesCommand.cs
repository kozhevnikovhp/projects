using System.Collections.Generic;
using System.Collections.Specialized;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class MirrorLayersAndValuesCommand : AbstractCommand
    {
        ZStackup currentObject;
        Dictionary<LayersCommandStructure, LayersCommandStructure> layerStructureDictionary;

        internal MirrorLayersAndValuesCommand(ZStackup currentObject, Dictionary<LayersCommandStructure, LayersCommandStructure> layerStructureDictionary)
        {
            this.currentObject = currentObject;
            this.layerStructureDictionary = layerStructureDictionary;
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

            foreach (LayersCommandStructure layerStructure in layerStructureDictionary.Keys)
            {
                currentObject.InternalReplaceLayer(layerStructure, false);
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

            foreach (LayersCommandStructure layerStructure in layerStructureDictionary.Keys)
            {
                currentObject.InternalReplaceLayer(layerStructureDictionary[layerStructure], false);
            }

            ZPlannerManager.ResumeCollectionChangedEvent();
            currentObject.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = true;
            ZPlannerManager.MainMenu.UpdateMenu();
        }
    }
}
