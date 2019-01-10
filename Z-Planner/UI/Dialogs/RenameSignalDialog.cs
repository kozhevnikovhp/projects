using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class RenameSignalDialog : Form
    {
        ZEntity signal;

        public RenameSignalDialog()
        {
            InitializeComponent();
        }

        internal void SetCurrentSignal(ZEntity signal)
        {
            this.signal = signal;
            newFileName.Text = signal.Title;
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrWhiteSpace(newFileName.Text))
            {
                string newName = newFileName.Text;

                if (signal.Title != newName)
                {
                    signal.Title = newName;
                    DialogResult = System.Windows.Forms.DialogResult.OK;
                    Close();
                    return;
                }
            }

            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }
    }
}
