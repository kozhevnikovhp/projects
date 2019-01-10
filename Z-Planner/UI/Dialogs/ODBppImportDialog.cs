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
    public partial class ODBppImportDialog : Form
    {
        private bool bOpenTgz;

        public ODBppImportDialog()
        {
            InitializeComponent();
            bOpenTgz = rbFile.Checked;
            StartPosition = FormStartPosition.CenterScreen;
        }

        public bool OpenTgz()
        {
            return bOpenTgz;
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
        }

        private void rbFile_CheckedChanged(object sender, EventArgs e)
        {
            bOpenTgz = rbFile.Checked;
        }

        private void rbFolder_CheckedChanged(object sender, EventArgs e)
        {
            bOpenTgz = rbFile.Checked;
        }
    }
}
