using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerMessagePanel : DockContent
    {
        public ZPlannerMessagePanel()
        {
            InitializeComponent();

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;
        }

        public void AddMessage(string message)
        {
            this.massageBox.AppendText(message + "\n");
        }

        public void ClearMessages()
        {
            this.massageBox.Clear();
        }

        private void massageBox_KeyDown(object sender, KeyEventArgs e)
        {
            KeysConverter kc = new KeysConverter();
            if (e.Control && !e.Shift && !e.Alt && kc.ConvertToString(e.KeyCode).ToUpper() == "A") this.massageBox.SelectAll();
        }
    }
}
