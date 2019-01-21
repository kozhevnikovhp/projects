using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Threading;

namespace ZZero.ZPlanner.UI.Menu
{
    public partial class ProgressPanel : Form
    {
        private Panel[] markers;
        private int markerIndex = 0;
        bool isActive = false;
        long updateTicks;

        public ProgressPanel()
        {
            InitializeComponent();
            markers = new Panel[] { pictStep1, pictStep2, pictStep3, pictStep4, pictStep5, pictStep6, pictStep7, pictStep8, pictStep9, pictStep10, pictStep11, pictStep12 };
        }

        public void Progress()
        {
            ++markerIndex;
            if (markerIndex > markers.Length || markerIndex == 0)
            {
                ClearProgress();
            }
            else
            {
                markers[markerIndex - 1].BackColor = Color.Blue;
            }
        }

        private void ClearProgress()
        {
            markerIndex = 0;
            foreach (Panel pictStep in markers)
            {
                pictStep.BackColor = Color.DarkGray;
            }
        }

        public void SetMessage(string message)
        {
            progressMessageLabel.Text = message;
            Update();
        }

        private void ProgressPanel_Load(object sender, EventArgs e)
        {
            isActive = true;
            ClearProgress();
            updateTicks = DateTime.Now.Ticks;
        }

        private void ProgressPanel_FormClosed(object sender, FormClosedEventArgs e)
        {
            isActive = false;
        }

        public void UpdateProgress()
        {
            if (!isActive) return ;

            if (DateTime.Now.Ticks - updateTicks > 5000000)
            {
                Progress();
                Update();
                updateTicks = DateTime.Now.Ticks;
            }
        }
    }
}
