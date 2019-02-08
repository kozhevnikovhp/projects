using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewRatioBoxColumn : ZDataGridViewColumn
    {
        public bool AllowNegative { get { return ((ZDataGridViewRatioBoxCell)CellTemplate).AllowNegative; } }
        public ZDataGridViewRatioBoxColumn()
            : base(new ZDataGridViewRatioBoxCell())
        {
            ValueType = typeof(double);
            DataType = ZDataGridViewFilterMenuDataType.Float;
            ZDataGridViewRatioBoxCell cell = new ZDataGridViewRatioBoxCell();
            cell.AllowNegative = false;
            CellTemplate = cell;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }
    }
}
