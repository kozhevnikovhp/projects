using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Menu
{
    public partial class ViaSpanMenu : UserControl
    {
        ZSpan spanToBeRemoveed;

        public ViaSpanMenu()
        {
            InitializeComponent();
            removeViaSpanBibbonButton.Enabled = false;
        }

        private void addViaSpanRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Project.Stackup.Spans.Add(new ZSpan(ZPlannerManager.Project.Stackup));
        }

        private void removeViaSpanBibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Project.Stackup.Spans.Remove(spanToBeRemoveed);
        }

        internal void EnableRemoveButton(ZSpan span)
        {
            spanToBeRemoveed = span;
            removeViaSpanBibbonButton.Enabled = (spanToBeRemoveed != null);
        }
    }
}
