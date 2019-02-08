using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewSelectBoxColumn : ZDataGridViewColumn
    {
        public ZDataGridViewSelectBoxColumn(Dictionary<string, string> items)
            : base(new ZDataGridViewSelectBoxCell(items))
        {
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }
    }
}
