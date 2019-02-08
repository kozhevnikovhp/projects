using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Settings;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class EtchAdjustDialog : Form
    {
        public EtchAdjustDialog()
        {
            InitializeComponent();

            //
            string format = "N" + Options.TheOptions.lengthDigits;

            tbEtch.Text = ZPlannerManager.Project.Stackup.Etch.ToString(format, CultureInfo.InvariantCulture);

            cbShowEtchEffects.Checked = ZPlannerManager.IsTrapezoidalTraces;
            checkBoxUpdateSettings.Checked = false;
        }

        public bool isEnableEtchEffects
        {
            get
            {
                return cbShowEtchEffects.Checked;
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            EtchAdjustStructure oldStructure, newStructure;

            //save data
            double val = 0;
            oldStructure.IsUsed = ZPlannerManager.IsTrapezoidalTraces;
            newStructure.IsUsed = this.cbShowEtchEffects.Checked;

            oldStructure.Etch = ZPlannerManager.Project.Stackup.Etch;
            if (Double.TryParse(tbEtch.Text, out val))
            {
                ZPlannerManager.Project.Stackup.Etch = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.etch = val;
            }
            newStructure.Etch = ZPlannerManager.Project.Stackup.Etch;

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupEtchAdjustCommand(ZPlannerManager.Project.Stackup, oldStructure, newStructure);

            if (checkBoxUpdateSettings.Checked)
            {
                Options options = Options.TheOptions;
                Settings_IO io = new Settings_IO();
                io.WriteSettings(ref options);
            }

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
