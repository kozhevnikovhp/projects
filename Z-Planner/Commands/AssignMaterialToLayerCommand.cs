﻿using System.Collections.Generic;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class AssignMaterialToLayerCommand : AbstractCommand
    {
        ZStackup currentObject;
        List<LayerMaterialCommandStructure> layersMaterialStructureList;

        internal AssignMaterialToLayerCommand(ZStackup currentObject, List<LayerMaterialCommandStructure> layersMaterialStructureList)
        {
            this.currentObject = currentObject;
            this.layersMaterialStructureList = layersMaterialStructureList;

            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                currentObject.InternalAssignMaterialToStackup(layersMaterialStructureList, false, true);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                currentObject.InternalAssignMaterialToStackup(layersMaterialStructureList, true, false);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
            }
        }
    }
}
