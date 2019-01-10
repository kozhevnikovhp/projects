using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewLinkBoxColumn : ZDataGridViewColumn
    {
        public ZDataGridViewLinkBoxColumn()
            : base(new ZDataGridViewLinkBoxCell())
        {
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }
    }
}
