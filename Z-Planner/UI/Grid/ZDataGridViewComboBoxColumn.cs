using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewComboBoxColumn : ZDataGridViewColumn
    {
        public ZDataGridViewComboBoxColumn(string[] items)
            : base(new ZDataGridViewComboBoxCell(items))
        {
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
        }
    }
}
