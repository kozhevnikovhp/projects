using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeGlassPitchCommand : AbstractCommand
    {
        ZStackup currentObject;
        GlassPitchStructure oldValue;
        GlassPitchStructure newValue;

        internal ChangeGlassPitchCommand(ZStackup currentObject, GlassPitchStructure oldValue, GlassPitchStructure newValue)
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
            ZPlannerManager.IsGlassPitch = oldValue.IsGlassPitch;
            ZPlannerManager.IsGws = oldValue.IsGws;
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            ZPlannerManager.IsGlassPitch = newValue.IsGlassPitch;
            ZPlannerManager.IsGws = newValue.IsGws;
        }
    }
}
