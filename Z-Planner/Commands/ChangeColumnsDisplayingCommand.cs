using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Commands
{
    class ChangeColumnsDisplayingCommand : AbstractCommand
    {
        List<ColumnsDisplayingStructure> columnDisplayingStructureList;

        internal ChangeColumnsDisplayingCommand(List<ColumnsDisplayingStructure> columnDisplayingStructureList)
        {
            this.columnDisplayingStructureList = columnDisplayingStructureList;
            base.RegisterCommand();
        }

        /// <summary>
        /// Undo command.
        /// </summary>
        internal override void Undo()
        {
            foreach (ColumnsDisplayingStructure columnDisplayingStructure in columnDisplayingStructureList)
            {
                if (columnDisplayingStructure.parameter != null)
                {
                    columnDisplayingStructure.parameter.IsHidden = columnDisplayingStructure.oldHidden;
                }
                
                columnDisplayingStructure.column.DisplayIndex = columnDisplayingStructure.oldOrder;
            }
        }

        /// <summary>
        /// Redo command.
        /// </summary>
        internal override void Redo()
        {
            foreach(ColumnsDisplayingStructure columnDisplayingStructure in columnDisplayingStructureList)
            {
                if (columnDisplayingStructure.parameter != null)
                {
                    columnDisplayingStructure.parameter.IsHidden = columnDisplayingStructure.newHidden;
                }
                    
                columnDisplayingStructure.column.DisplayIndex = columnDisplayingStructure.newOrder;
            }
        }
    }
}
