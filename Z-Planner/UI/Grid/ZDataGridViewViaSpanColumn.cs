using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewViaSpanColumn : ZDataGridViewColumn
    {
        internal double SpanSize { get; set; }
        internal double SpanRatio { get; set; }
        internal ZLayer FirstLayer { get; set; }
        internal ZLayer LastLayer { get; set; }

        public ZDataGridViewViaSpanColumn()
            : base(new ZDataGridViewViaSpanCell())
        {
            this.HeaderCell = new ZDataGridViewColumnHeaderCell(this.HeaderCell);
            this.DefaultCellStyle.NullValue = "";
            this.ReadOnly = true;
        }

        public void SetSpan(ZSpan span)
        {
            this.SpanSize = span.Size;
            this.SpanRatio = span.AspectRatio;
            this.FirstLayer = span.FirstLayer;
            this.LastLayer = span.LastLayer;

            bool isInRange = false;
            foreach (ZDataGridViewRow row in this.DataGridView.Rows)
            {
                if (this.FirstLayer == row.Tag as ZLayer) isInRange = true;
                (row.Cells[this.Index] as ZDataGridViewViaSpanCell).IsInRange = isInRange;
                if (this.LastLayer == row.Tag as ZLayer) isInRange = false;
            }
        }

        public void ZSpan_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZSpan span = sender as ZSpan;

            if (span == null) return;

            switch (e.PropertyName)
            {
                case "Size":
                case "AspectRatio":
                    (this.HeaderCell as ZDataGridViewColumnHeaderCell).SubTitle = span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    this.ToolTipText = "Hole Diameter: " + span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture) + " mils, Length: " + span.CalculateHoleLength().ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture) + " mils, and Aspect Ratio: " + span.AspectRatio.ToString("N" + Settings.Options.TheOptions.percentDigits, CultureInfo.InvariantCulture) + " : 1";
                    this.DataGridView.Invalidate();
                    break;
                case "FirstLayer":
                case "LastLayer":
                    this.SetSpan(span);
                    this.DataGridView.Invalidate();
                    break;
                default:
                    break;
            }

            ZPlannerManager.PropertiesPanel.UpdateProperties();
        }
    }
}
