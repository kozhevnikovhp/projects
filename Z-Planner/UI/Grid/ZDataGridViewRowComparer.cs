using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Grid
{
    public class ZDataGridViewRowComparer : System.Collections.IComparer
    {
        private List<CompareItem> Items;

        public ZDataGridViewRowComparer(List<CompareItem> items)
        {
            Items = items;
        }

        public int Compare(object r1, object r2)
        {
            ZDataGridViewRow row1 = (ZDataGridViewRow)r1;
            ZDataGridViewRow row2 = (ZDataGridViewRow)r2;
            
            foreach(CompareItem item in Items)
            {
                ZDataGridViewColumn column = row1.DataGridView.Columns[item.ColumnName] as ZDataGridViewColumn;
                if (column == null) continue;

                int CompareResult;

                if (column.ValueType == typeof(double))
                {
                    string s1 = column.GetFormatedValue(row1);
                    string s2 = column.GetFormatedValue(row2);
                    double d1;
                    double d2;

                    if (double.TryParse(s1, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(s2, NumberStyles.Any, CultureInfo.InvariantCulture, out d2))
                        CompareResult = d1.CompareTo(d2);
                    else
                        CompareResult = String.Compare(s1, s2);
                }
                else
                {
                    CompareResult = String.Compare(column.GetFormatedValue(row1), column.GetFormatedValue(row2));
                }

                if (CompareResult == 0) continue;

                if (item.SortDirection == ListSortDirection.Descending) CompareResult *= -1;

                return CompareResult;
            }

            {
                //string s1 = row1.HeaderCell.Value.ToString();
                //string s2 = row2.HeaderCell.Value.ToString();
                IOrder ordered1 = row1.Tag as IOrder;
                IOrder ordered2 = row2.Tag as IOrder;
                if (ordered1 != null && ordered2 != null && ordered1.Order != ordered2.Order) return ordered1.Order.CompareTo(ordered2.Order);

                string s1 = row1.HeaderCell.Value.ToString();
                string s2 = row2.HeaderCell.Value.ToString();
                double d1;
                double d2;

                if (double.TryParse(s1, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(s2, NumberStyles.Any, CultureInfo.InvariantCulture, out d2))
                    return d1.CompareTo(d2);
                else
                    return String.Compare(s1, s2);
            }
        }
    }

    public class CompareItem
    {
        public string ColumnName {get; private set;}
        public ListSortDirection SortDirection {get; private set;}

        public CompareItem(string columnName, ListSortDirection sortDirection)
        {
            ColumnName = columnName;
            SortDirection = sortDirection;
        }
    }
}
