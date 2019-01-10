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
    public partial class MessageBoxDialog : Form
    {
        public MessageBoxDialog(string dialogTitle, string dialogMessage, Icon dialogIcon)
        {
            InitializeComponent();
            this.Text = dialogTitle;
            lMessage.Text = dialogMessage;
            if (dialogIcon != null) this.Icon = dialogIcon;
        }

        private void bYes_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Yes;
            Close();
        }

        private void bNo_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.No;
            Close();
        }

        private void bCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}
