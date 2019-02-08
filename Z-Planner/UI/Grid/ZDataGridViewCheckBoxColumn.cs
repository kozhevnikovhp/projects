using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewCheckBoxColumn : ZDataGridViewColumn
    {
        public ZDataGridViewCheckBoxColumn()
            : base(new ZDataGridViewCheckBoxCell())
        {
            ValueType = typeof(bool);
            DataType = ZDataGridViewFilterMenuDataType.Boolean;
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
            this.DefaultCellStyle.NullValue = false;
        } 
    }
}
