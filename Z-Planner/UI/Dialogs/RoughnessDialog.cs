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
    public partial class RoughnessDialog : Form
    {
        public RoughnessDialog()
        {
            InitializeComponent();

            //
            string format = "N" + Options.TheOptions.lengthDigits;

            tbOuterRoughness.Text = ZPlannerManager.Project.Stackup.OuterLayerRoughness.ToString(format, CultureInfo.InvariantCulture);
            tbLaminate_Side_Rougness.Text = ZPlannerManager.Project.Stackup.LaminateSideRoughness.ToString(format, CultureInfo.InvariantCulture);
            tbPrepreg_Side_Roughness.Text = ZPlannerManager.Project.Stackup.PrepregSideRoughness.ToString(format, CultureInfo.InvariantCulture);

            cbShowRoughness.Checked = ZPlannerManager.IsRoughness;
            checkBoxUpdateSettings.Checked = false;
        }

        public bool isShowRoughness
        {
            get
            {
                return cbShowRoughness.Checked;
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            RoughnessStructure oldStructure, newStructure;

            //save data
            double val = 0;
            oldStructure.IsUsed = ZPlannerManager.IsRoughness;
            newStructure.IsUsed = cbShowRoughness.Checked;

            oldStructure.OuterLayerRoughness = ZPlannerManager.Project.Stackup.OuterLayerRoughness;
            if (Double.TryParse(tbOuterRoughness.Text, out val))
            {
                ZPlannerManager.Project.Stackup.OuterLayerRoughness = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.roughness = val;
            }
            newStructure.OuterLayerRoughness = ZPlannerManager.Project.Stackup.OuterLayerRoughness;

            oldStructure.LaminateSideRoughness = ZPlannerManager.Project.Stackup.LaminateSideRoughness;
            if (Double.TryParse(tbLaminate_Side_Rougness.Text, out val)) 
            {
                ZPlannerManager.Project.Stackup.LaminateSideRoughness = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.laminate_side_roughness = val;
            }
            newStructure.LaminateSideRoughness = ZPlannerManager.Project.Stackup.LaminateSideRoughness;

            oldStructure.PrepregSideRoughness = ZPlannerManager.Project.Stackup.PrepregSideRoughness;
            if (Double.TryParse(tbPrepreg_Side_Roughness.Text, out val))
            {
                ZPlannerManager.Project.Stackup.PrepregSideRoughness = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.prepreg_side_roughness = val;
            }
            newStructure.PrepregSideRoughness = ZPlannerManager.Project.Stackup.PrepregSideRoughness;

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupRoughnessCommand(ZPlannerManager.Project.Stackup, oldStructure, newStructure);

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
