using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class ConfirmDmlSynchronizationDialog : Form
    {
        public ConfirmDmlSynchronizationDialog()
        {
            InitializeComponent();
            DialogResult = System.Windows.Forms.DialogResult.Ignore;
        }

        private void bOk_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.OK;
            Close();
        }

        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }

        private void bIgnore_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Ignore;
            Close();
        }
    }
}
