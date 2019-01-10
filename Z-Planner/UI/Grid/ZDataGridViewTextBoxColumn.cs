using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewTextBoxColumn : ZDataGridViewColumn
    {
        public ZDataGridViewTextBoxColumn()
            : base(new ZDataGridViewTextBoxCell())
        {
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }
    }
}
