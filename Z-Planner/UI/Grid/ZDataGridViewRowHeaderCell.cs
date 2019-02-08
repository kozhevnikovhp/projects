using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewRowHeaderCell : DataGridViewRowHeaderCell
    {
        public ZDataGridViewRowHeaderCell() : base()
        {
            this.Style.Alignment = DataGridViewContentAlignment.MiddleRight;
        }

        public void ZLayer_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZLayer layer = sender as ZLayer;

            if (layer == null) return;

            switch (e.PropertyName)
            {
                case "Title":
                    ToolTipText = layer.Title;
                    break;
                default:
                    break;
            }
        }

        protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
        {
            if ((this.DataGridView.CurrentCell != null && this.DataGridView.CurrentCell.OwningRow == this.OwningRow) ) // || (this.DataGridView as ZDataGridView).CurrentRowIndex == rowIndex)
            {
                cellState = DataGridViewElementStates.Selected;
            }

            base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
        }
    }
}
