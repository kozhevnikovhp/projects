using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupEtchAdjustCommand : AbstractCommand
    {
        ZStackup currentObject;
        EtchAdjustStructure oldValue;
        EtchAdjustStructure newValue;

        internal ChangeStackupEtchAdjustCommand(ZStackup currentObject, EtchAdjustStructure oldValue, EtchAdjustStructure newValue)
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
            currentObject.Etch = oldValue.Etch;
            ZPlannerManager.IsTrapezoidalTraces = oldValue.IsUsed;
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                foreach (ZLayer layer in currentObject.Layers)
                {
                    layer.SetLayerParameterValue(ZStringConstants.ParameterIDEtchFactor, currentObject.Etch.ToString("N" + Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
                }
            }
            finally
            {
                ZPlannerManager.RecalculateThickness();
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            currentObject.Etch = newValue.Etch;
            ZPlannerManager.IsTrapezoidalTraces = newValue.IsUsed;
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

            try
            {
                foreach (ZLayer layer in currentObject.Layers)
                {
                    layer.SetLayerParameterValue(ZStringConstants.ParameterIDEtchFactor, currentObject.Etch.ToString("N" + Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
                }
            }
            finally
            {
                ZPlannerManager.RecalculateThickness();
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
            }
        }
    }
}
