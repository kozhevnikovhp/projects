using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewPercentBoxColumn : ZDataGridViewColumn
    {
        public bool AllowNegative { get { return ((ZDataGridViewPercentBoxCell)CellTemplate).AllowNegative; } }
        public ZDataGridViewPercentBoxColumn(bool allowNegative)
            : base(new ZDataGridViewPercentBoxCell())
        {
            ValueType = typeof(double); 
            DataType = ZDataGridViewFilterMenuDataType.Float;
            ZDataGridViewPercentBoxCell cell = new ZDataGridViewPercentBoxCell();
            cell.AllowNegative = allowNegative;
            CellTemplate = cell;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }

        public ZDataGridViewPercentBoxColumn()
            : this(false)
        {
        }
    }
}
