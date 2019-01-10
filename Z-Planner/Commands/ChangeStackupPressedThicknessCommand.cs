using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

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
            currentObject.ByCopperCoverage = oldValue.ByCopperCoverage;
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
            currentObject.ByCopperCoverage = newValue.ByCopperCoverage;
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
