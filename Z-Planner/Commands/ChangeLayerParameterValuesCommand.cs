using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeLayerParameterValuesCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<LayerParameterCommmandStructure> layerParameterStructureList;

        internal ChangeLayerParameterValuesCommand(ZStackup currentObject, List<LayerParameterCommmandStructure> layerParameterStructureList)
        {
            this.currentObject = currentObject;
            this.layerParameterStructureList = layerParameterStructureList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            for (int i = layerParameterStructureList.Count - 1; i >= 0; --i)
            {
                currentObject.InternalChangeLayerType(layerParameterStructureList[i], false);
            }

            currentObject.CalculateLayerNumber();
            ZPlannerManager.MainMenu.UpdateMenu();
            ZPlannerManager.UpdateActiveStackup();

            ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
            ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            for (int i = 0; i < layerParameterStructureList.Count; ++i)
            {
                currentObject.InternalChangeLayerType(layerParameterStructureList[i], true);
            }

            currentObject.CalculateLayerNumber();
            ZPlannerManager.MainMenu.UpdateMenu();
            ZPlannerManager.UpdateActiveStackup();

            ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
            ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
        }
    }
}
