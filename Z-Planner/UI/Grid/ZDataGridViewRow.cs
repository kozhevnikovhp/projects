using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewRow : DataGridViewRow
    {
        internal string Name { get; set; }
        public ZDataGridViewRow() : base()
        {
        }

        public void ZMaterial_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZMaterial material = sender as ZMaterial;
            if (material == null) return;

            switch (e.PropertyName)
            {
                case "IsHidden":
                case "IsFiltered":
                    this.Visible = !material.IsHidden && !material.IsFiltered;
                    break;
            }
        }

        public void ZSpan_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZSpan span = sender as ZSpan;
            if (span == null) return;

            switch (e.PropertyName)
            {
                case "Enable":
                    this.Cells[e.PropertyName].Value = span.IsEnable.ToString();
                    break;
                case "FirstLayer":
                    this.Cells[e.PropertyName].Value = span.FirstLayer.GetLayerNumber();
                    break;
                case "LastLayer":
                    this.Cells[e.PropertyName].Value = span.LastLayer.GetLayerNumber();
                    break;
                case "Size":
                    this.Cells[e.PropertyName].Value = span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    break;
                case "AspectRatio":
                    this.Cells[e.PropertyName].Value = span.AspectRatio.ToString("N" + Settings.Options.TheOptions.percentDigits, CultureInfo.InvariantCulture);
                    break;
                case "Filled":
                    this.Cells[e.PropertyName].Value = span.Filled;
                    break;
                case "BackDrilled":
                    this.Cells[e.PropertyName].Value = span.IsBackDrilled.ToString();
                    break;
                default:
                    break;
            }

            ZPlannerManager.PropertiesPanel.UpdateProperties();
        }

        protected override void PaintCells(System.Drawing.Graphics graphics, System.Drawing.Rectangle clipBounds, System.Drawing.Rectangle rowBounds, int rowIndex, DataGridViewElementStates rowState, bool isFirstDisplayedRow, bool isLastVisibleRow, DataGridViewPaintParts paintParts)
        {
            if (rowIndex == 0)
            {
                ZMaterial material = this.Tag as ZMaterial;
                if (material != null && material.ID == ZStringConstants.EmptyMaterialID)
                {
                    ZPlannerManager.DMLPanel.EmptyMaterialLabel.Location = new Point(DataGridView.RowHeadersVisible ? DataGridView.RowHeadersWidth : 0, rowBounds.Y);
                    ZPlannerManager.DMLPanel.EmptyMaterialLabel.Size = new Size(DataGridView.RowHeadersVisible ? rowBounds.Width - DataGridView.RowHeadersWidth : rowBounds.Width, rowBounds.Height);
                    return;
                }
            }

            base.PaintCells(graphics, clipBounds, rowBounds, rowIndex, rowState, isFirstDisplayedRow, isLastVisibleRow, paintParts);
        }
    }
}
