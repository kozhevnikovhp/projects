using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Commands
{
    class ChangeStackupPressedThicknessCommand : AbstractCommand
    {
        ZStackup currentObject;
        PressedThicknessStructure oldValue;
        PressedThicknessStructure newValue;

        internal ChangeStackupPressedThicknessCommand(ZStackup currentObject, PressedThicknessStructure oldValue, PressedThicknessStructure newValue)
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
            //string format = ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent);
            //if (currentObject.ForSignal != oldValue.ForSignal) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForSignal.ToString(format, CultureInfo.InvariantCulture), oldValue.ForSignal.ToString(format, CultureInfo.InvariantCulture));
            //if (currentObject.ForMixed != oldValue.ForMixed) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForMixed.ToString(format, CultureInfo.InvariantCulture), oldValue.ForMixed.ToString(format, CultureInfo.InvariantCulture));
            //if (currentObject.ForPlane != oldValue.ForPlane) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForPlane.ToString(format, CultureInfo.InvariantCulture), oldValue.ForPlane.ToString(format, CultureInfo.InvariantCulture));
            
            currentObject.CopperCoverageType = oldValue.CopperCoverageType;
            currentObject.ForSignal = oldValue.ForSignal;
            currentObject.ForMixed = oldValue.ForMixed;
            currentObject.ForPlane = oldValue.ForPlane;
            currentObject.ForSignalSignal = oldValue.ForSignalSignal;
            currentObject.ForSignalMixed = oldValue.ForSignalMixed;
            currentObject.ForSignalPlane = oldValue.ForSignalPlane;
            currentObject.ForMixedMixed = oldValue.ForMixedMixed;
            currentObject.ForMixedPlane = oldValue.ForMixedPlane;
            currentObject.ForPlanePlane = oldValue.ForPlanePlane;

            ZPlannerManager.IsPressedThickness = oldValue.IsPressedThickness;
            ZPlannerManager.IsSequentialLamination = oldValue.IsSequentialLamination;

            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            //string format = ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent);
            //if (currentObject.ForSignal != newValue.ForSignal) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForSignal.ToString(format, CultureInfo.InvariantCulture), newValue.ForSignal.ToString(format, CultureInfo.InvariantCulture));
            //if (currentObject.ForMixed != newValue.ForMixed) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForMixed.ToString(format, CultureInfo.InvariantCulture), newValue.ForMixed.ToString(format, CultureInfo.InvariantCulture));
            //if (currentObject.ForPlane != newValue.ForPlane) currentObject.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, currentObject.ForPlane.ToString(format, CultureInfo.InvariantCulture), newValue.ForPlane.ToString(format, CultureInfo.InvariantCulture));

            currentObject.CopperCoverageType = newValue.CopperCoverageType;
            currentObject.ForSignal = newValue.ForSignal;
            currentObject.ForMixed = newValue.ForMixed;
            currentObject.ForPlane = newValue.ForPlane;
            currentObject.ForSignalSignal = newValue.ForSignalSignal;
            currentObject.ForSignalMixed = newValue.ForSignalMixed;
            currentObject.ForSignalPlane = newValue.ForSignalPlane;
            currentObject.ForMixedMixed = newValue.ForMixedMixed;
            currentObject.ForMixedPlane = newValue.ForMixedPlane;
            currentObject.ForPlanePlane = newValue.ForPlanePlane;

            ZPlannerManager.IsPressedThickness = newValue.IsPressedThickness;
            ZPlannerManager.IsSequentialLamination = newValue.IsSequentialLamination;
            
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
            }
        }
    }
}
