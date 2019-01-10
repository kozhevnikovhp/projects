using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewCheckBoxWithTextColumn : ZDataGridViewColumn
    {
        public ZDataGridViewCheckBoxWithTextColumn()
            : base(new ZDataGridViewCheckBoxWithTextCell())
        {
            ValueType = typeof(bool);
            DataType = ZDataGridViewFilterMenuDataType.Boolean;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
            this.DefaultCellStyle.NullValue = false;
        }
    }
}
