
namespace ZZero.ZPlanner.Commands
{
    class ChangeZPlannerPropertyCommand : AbstractCommand
    {
        string propertyName;
        object oldValue;
        object newValue;

        internal ChangeZPlannerPropertyCommand(string propertyName, object oldValue, object newValue)
        {
            this.propertyName = propertyName;
            this.oldValue = oldValue;
            this.newValue = newValue;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            switch(propertyName)
            {
                case "IsRoughness":
                    ZPlannerManager.IsRoughness = (bool)oldValue;
                    break;
                case "IsTrapezoidalTraces":
                    ZPlannerManager.IsTrapezoidalTraces = (bool)oldValue;
                    break;
                case "IsPressedThickness":
                    ZPlannerManager.IsPressedThickness = (bool)oldValue;
                    break;
                case "IsSequentialLamination":
                    ZPlannerManager.IsSequentialLamination = (bool)oldValue;
                    break;
                case "IsGlassPitch":
                    ZPlannerManager.IsGlassPitch = (bool)oldValue;
                    break;
                case "IsGws":
                    ZPlannerManager.IsGws = (bool)oldValue;
                    break;
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            switch (propertyName)
            {
                case "IsRoughness":
                    ZPlannerManager.IsRoughness = (bool)newValue;
                    break;
                case "IsTrapezoidalTraces":
                    ZPlannerManager.IsTrapezoidalTraces = (bool)newValue;
                    break;
                case "IsPressedThickness":
                    ZPlannerManager.IsPressedThickness = (bool)newValue;
                    break;
                case "IsGlassPitch":
                    ZPlannerManager.IsGlassPitch = (bool)newValue;
                    break;
                case "IsGws":
                    ZPlannerManager.IsGws = (bool)newValue;
                    break;
            }
        }
    }
}