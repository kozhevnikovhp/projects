using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewFrequencyDependedColumn : ZDataGridViewColumn
    {
        public ZDataGridViewFrequencyDependedColumn(bool allowNegative)
            : base(new ZDataGridViewFrequencyDependedCell())
        {
            ValueType = typeof(string); 
            DataType = ZDataGridViewFilterMenuDataType.Text;
            ZDataGridViewFrequencyDependedCell cell = new ZDataGridViewFrequencyDependedCell();
            CellTemplate = cell;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }

        public ZDataGridViewFrequencyDependedColumn()
            : this(false)
        {
        }
    }
}
