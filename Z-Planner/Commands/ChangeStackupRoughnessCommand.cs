using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupRoughnessCommand : AbstractCommand
    {
        ZStackup currentObject;
        RoughnessStructure oldValue;
        RoughnessStructure newValue;

        internal ChangeStackupRoughnessCommand(ZStackup currentObject, RoughnessStructure oldValue, RoughnessStructure newValue)
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
            currentObject.OuterLayerRoughness = oldValue.OuterLayerRoughness;
            currentObject.LaminateSideRoughness = oldValue.LaminateSideRoughness;
            currentObject.PrepregSideRoughness = oldValue.PrepregSideRoughness;

            ZPlannerManager.IsRoughness = oldValue.IsUsed;

            ZPlannerManager.UpdateActiveStackup();
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.OuterLayerRoughness = newValue.OuterLayerRoughness;
            currentObject.LaminateSideRoughness = newValue.LaminateSideRoughness;
            currentObject.PrepregSideRoughness = newValue.PrepregSideRoughness;

            ZPlannerManager.IsRoughness = newValue.IsUsed;
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ShowRoughnessColumns(ZPlannerManager.IsRoughness);
            ZPlannerManager.UpdateActiveStackup();
        }
    }
}
