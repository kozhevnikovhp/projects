using System;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid.Filter
{
    public class LEFilter : IFilter
    {
        ZDataGridViewColumn Column {get;set;}
        double Value {get;set;}
        bool IsParsed {get;set;}

        public LEFilter(ZDataGridViewColumn column, string value)
        {
            Column = column;

            double dValue;
            IsParsed = double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue);
            if (IsParsed) Value = dValue;
        }

        public bool IsMatch(DataGridViewRow row)
        {
            if (!IsParsed) return true;

            double dValue;
            if (double.TryParse(Column.GetFormatedValue(row), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) return dValue <= Value;
            return false;
        }
    }
}
