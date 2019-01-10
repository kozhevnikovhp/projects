using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.Commands
{
    /// <summary>
    /// Acts as a base class for all commands.
    /// </summary>
    abstract class AbstractCommand
    {
        protected void RegisterCommand()
        {
            //Redo();
            ZPlannerManager.Commands.Add(this);
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal abstract void Undo();

        /// <summary>
        /// Redo command.
        /// </summary>
        internal abstract void Redo();
    }
}
