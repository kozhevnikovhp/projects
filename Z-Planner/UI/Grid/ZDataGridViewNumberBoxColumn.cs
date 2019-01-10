using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewNumberBoxColumn : ZDataGridViewColumn
    {
        public bool AllowNegative { get { return ((ZDataGridViewNumberBoxCell)CellTemplate).AllowNegative; } }
        public ZDataGridViewNumberBoxColumn(bool allowNegative)
            : base(new ZDataGridViewNumberBoxCell())
        {
            ValueType = typeof(double); 
            DataType = ZDataGridViewFilterMenuDataType.Float;
            ZDataGridViewNumberBoxCell cell = new ZDataGridViewNumberBoxCell();
            cell.AllowNegative = allowNegative;
            CellTemplate = cell;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }

        public ZDataGridViewNumberBoxColumn()
            : this(false)
        {
        }
    }
}
