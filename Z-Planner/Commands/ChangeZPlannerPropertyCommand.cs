
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
                    ZPlannerManager.SetCopperRoughness((bool)oldValue);
                    break;
                case "IsTrapezoidalTraces":
                    ZPlannerManager.SetTrapezoidalTraces((bool)oldValue);
                    break;
                case "IsPressedThickness":
                    ZPlannerManager.SetPressedThickness((bool)oldValue);
                    break;
                case "IsSequentialLamination":
                    ZPlannerManager.SetSequentialLamination((bool)oldValue);
                    break;
                case "IsKeepImportedPressedThickness":
                    ZPlannerManager.SetKeepImportedPressedThickness((bool)oldValue);
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
                    ZPlannerManager.SetCopperRoughness((bool)newValue);
                    break;
                case "IsTrapezoidalTraces":
                    ZPlannerManager.SetTrapezoidalTraces((bool)newValue);
                    break;
                case "IsPressedThickness":
                    ZPlannerManager.SetPressedThickness((bool)newValue);
                    break;
                case "IsSequentialLamination":
                    ZPlannerManager.SetSequentialLamination((bool)newValue);
                    break;
                case "IsKeepImportedPressedThickness":
                    ZPlannerManager.SetKeepImportedPressedThickness((bool)newValue);
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