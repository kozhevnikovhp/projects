using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid.Filter
{
    public class InFilter : IFilter
    {
        ZDataGridViewColumn Column {get;set;}
        string[] Values {get;set;}

        public InFilter(ZDataGridViewColumn column, string[] values)
        {
            Column = column;
            Values = values;
        }

        public bool IsMatch(DataGridViewRow row)
        {
            return Values.Contains(Column.GetFormatedValue(row));
        }
    }
}
