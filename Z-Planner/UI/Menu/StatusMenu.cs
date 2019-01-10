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
using System.Globalization;
using System.Threading;

namespace ZZero.ZPlanner.UI.Menu
{
    public partial class StatusMenu : UserControl
    {
        string progressMessage = string.Empty;
        int progressCount = 0;
        long updateTicks;

        public StatusMenu()
        {
            InitializeComponent();
            RibbonProfesionalRendererColorTable r = Theme.ColorTable;
            r.SetColor(RibbonColorPart.RibbonBackground_2013, SystemColors.Control);
        }

        public void StartProgress(string message, bool showProgressBar = false)
        {
            progressMessage = message;
            progressCount = 0;

            tbStatus.Text = message;
            tbStatus.Update();

            /*if (showProgressBar)
            {
                progressPictureBox.Enabled = true;
                progressPictureBox.Visible = true;
            }*/

            updateTicks = DateTime.Now.Ticks;
        }

        public bool UpdateProgress()
        {

            if (DateTime.Now.Ticks - updateTicks > 100000)
            {
                ++progressCount;
                if (progressCount >= 200)
                {
                    tbStatus.Text = progressMessage;
                    progressCount = 0;
                }
                else
                {
                    tbStatus.Text += ".";
                }

                tbStatus.Update();
                updateTicks = DateTime.Now.Ticks;
            }

            return true;
        }

        public void StopProgress(string message)
        {
            progressMessage = string.Empty;
            progressCount = 0;

            tbStatus.Text = "Ready";
            tbStatus.Update();

            /*if (progressPictureBox.Visible) progressPictureBox.Visible = false;
            if (progressPictureBox.Enabled) progressPictureBox.Enabled = false;*/
        }

        public void SetStatus(string message)
        {
            tbStatus.Text = message;
            tbStatus.Update();
        }

        public void SetStatusReady()
        {
            tbStatus.Text = "Ready";
            tbStatus.Update();
        }

        internal void SetStackupParameters(ZStackup stackup)
        {
            if (stackup == null)
            {
                tbNumberOfLayers.Text = string.Empty;
                tbBoardThickness.Text = string.Empty;
                return;
            }

            int layerCount = stackup.GetMetallLayerCount();
            string layerString = (layerCount == 1) ? " Layer" : " Layers";

            tbNumberOfLayers.Text = layerCount + layerString;
            tbBoardThickness.Text = stackup.GetBoardThickness().ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture) + " mils";
        }
    }
}
