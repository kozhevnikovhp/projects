using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Grid
{
    public class ZDataGridViewColumn : DataGridViewTextBoxColumn
    {
        public ZDataGridViewColumn(DataGridViewCell cellTemplate) : base() 
        {
            SortMode = DataGridViewColumnSortMode.Programmatic;
            DataType = ZDataGridViewFilterMenuDataType.Text;
        }

        public ZDataGridViewFilterMenuDataType DataType { get; protected set; }

        public ZTableType TableType { get; set; }

        public string HeaderName 
        { 
            get 
            {
                ZDataGridViewColumnHeaderCell cell = this.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (cell == null) return string.Empty;

                return cell.HeaderName;
            }
            set
            {
                ZDataGridViewColumnHeaderCell cell = this.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (cell == null) return;

                cell.HeaderName = value;
            }
        }

        public string GetHeaderNameByOrder(int index)
        {
            string result = "";
            if (index >= ZStringConstants.Alphabet.Length)
                result = result + GetHeaderNameByOrder((int)(index / ZStringConstants.Alphabet.Length - 1));
            return result + ZStringConstants.Alphabet[(int)(index % ZStringConstants.Alphabet.Length)];
        }

        public string GetFormatedValue(DataGridViewRow row)
        {
            ZDataGridViewNumberBoxCell numberBoxCell = row.Cells[this.Name] as ZDataGridViewNumberBoxCell;
            if (numberBoxCell != null)
            {
                return numberBoxCell.FormattedValue.ToString();
            }
            else
            {
                ZDataGridViewPercentBoxCell percentBoxCell = row.Cells[this.Name] as ZDataGridViewPercentBoxCell;
                if (percentBoxCell != null)
                {
                    return percentBoxCell.FormattedValue.ToString();
                }
            }

            return row.Cells[this.Name].FormattedValue.ToString();
        }

        public void ZParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZParameter parameter = sender as ZParameter;
            if (parameter == null) return;

            switch (e.PropertyName)
            {
                case "IsHidden":
                case "IsPrivate":
                    this.Visible = !parameter.IsHidden && !parameter.IsPrivate;
                    break;
                case "Order":
                    if (this.DisplayIndex != parameter.Order) this.DisplayIndex = parameter.Order;
                    break;
                case "Title":
                case "DisplayMeasure":
                    ZDataGridViewColumnHeaderCell headerCell = this.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (headerCell != null)
                    {
                        if (headerCell.ColumnSpan > 1)
                        {
                            headerCell.SubTitle = parameter.Title;
                            headerCell.Value = headerCell.SubTitle;
                        }
                        else
                        {
                            headerCell.Title = parameter.Title;
                            headerCell.Value = headerCell.Title;
                        }
                    }
                    break;
            }
        }
    }
}
