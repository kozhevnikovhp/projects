using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Commands;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class GlassPitchDialog : Form
    {
        public GlassPitchDialog()
        {
            InitializeComponent();
        }

        internal void InitValues(bool isGlassPitch, bool isGws)
        {
            cbShowGlassPitch.Checked = isGlassPitch;
            cbShowGws.Checked = isGws;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            GlassPitchStructure oldStructure, newStructure;

            oldStructure.IsGlassPitch = ZPlannerManager.IsGlassPitch;
            newStructure.IsGlassPitch = cbShowGlassPitch.Checked;
            ZPlannerManager.IsGlassPitch = cbShowGlassPitch.Checked;

            oldStructure.IsGws = ZPlannerManager.IsGws;
            newStructure.IsGws = cbShowGws.Checked;
            ZPlannerManager.IsGws = cbShowGws.Checked;

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeGlassPitchCommand(ZPlannerManager.Project.Stackup, oldStructure, newStructure);

            //
            DialogResult = DialogResult.OK;
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}
