using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Grid
{
    interface IZDataGridViewCell
    {
        int ColumnSpan { get; set; }
        int RowSpan { get; set; }
        DataGridViewCell OwnerCell { get; set; }
        void ZLayerParameter_PropertyChanged(object sender, PropertyChangedEventArgs e);
        void ZMaterialParameter_PropertyChanged(object sender, PropertyChangedEventArgs e);
    }
}
