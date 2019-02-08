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
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class CopperThicknessDialog : Form
    {
        private ZStackup stackup;

        internal CopperThicknessDialog(ZStackup stackup)
        {
            InitializeComponent();
            this.stackup = stackup;
            LoadValues();
            this.DialogResult = DialogResult.Cancel;
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            if (rbSetAsDefault.Checked) SaveValues();
            if (rbApplyValues.Checked) ApplyValues();
            this.DialogResult = DialogResult.OK;
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            Close();
        }

        private void LoadValues()
        {
            if (stackup == null) return;

            foreach (ZLayer layer in stackup.GetMetallLayerCollection())
            {
                double copperWeight, copperThickness;
                if (ZPlannerManager.TryToParseCopperWeight(layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness).Trim(), out copperWeight) &&
                    double.TryParse(layer.GetLayerParameterValue(ZStringConstants.ParameterIDThickness).Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out copperThickness))
                {
                    if (copperWeight == 0.5)
                        tbCopperThickness_05.Text = copperThickness.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    else if (copperWeight == 1.0)
                        tbCopperThickness_10.Text = copperThickness.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    else if (copperWeight == 1.5)
                        tbCopperThickness_15.Text = copperThickness.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    else if (copperWeight == 2.0)
                        tbCopperThickness_20.Text = copperThickness.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                    else if (copperWeight == 2.5)
                        tbCopperThickness_25.Text = copperThickness.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                }
            }
        }

        private void SaveValues()
        {
            double dValue;
            if (!string.IsNullOrWhiteSpace(tbCopperThickness_05.Text) && 
                double.TryParse(tbCopperThickness_05.Text.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) 
                Settings.Options.TheOptions.copper_thickness_05oz = dValue;

            if (!string.IsNullOrWhiteSpace(tbCopperThickness_10.Text) &&
                double.TryParse(tbCopperThickness_10.Text.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                Settings.Options.TheOptions.copper_thickness_10oz = dValue;

            if (!string.IsNullOrWhiteSpace(tbCopperThickness_15.Text) &&
                double.TryParse(tbCopperThickness_15.Text.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                Settings.Options.TheOptions.copper_thickness_15oz = dValue;

            if (!string.IsNullOrWhiteSpace(tbCopperThickness_20.Text) &&
                double.TryParse(tbCopperThickness_20.Text.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                Settings.Options.TheOptions.copper_thickness_20oz = dValue;

            if (!string.IsNullOrWhiteSpace(tbCopperThickness_25.Text) &&
                double.TryParse(tbCopperThickness_25.Text.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                Settings.Options.TheOptions.copper_thickness_25oz = dValue;

            Options options = Options.TheOptions;
            Settings_IO io = new Settings_IO();
            io.WriteSettings(ref options);
        }

        private void ApplyValues()
        {
            if (stackup == null) return;

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            try
            {
                foreach (ZLayer layer in stackup.GetMetallLayerCollection())
                {
                    double copperWeight;
                    if (ZPlannerManager.TryToParseCopperWeight(layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness).Trim(), out copperWeight))
                    {
                        /*
                        if (copperWeight == 0.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, tbCopperThickness_05.Text, true);
                        else if (copperWeight == 1.0)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, tbCopperThickness_10.Text, true);
                        else if (copperWeight == 1.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, tbCopperThickness_15.Text, true);
                        else if (copperWeight == 2.0)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, tbCopperThickness_20.Text, true);
                        else if (copperWeight == 2.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, tbCopperThickness_25.Text, true);
                         */

                        if (copperWeight == 0.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, Options.TheOptions.copper_thickness_05oz.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture), true);
                        else if (copperWeight == 1.0)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, Options.TheOptions.copper_thickness_10oz.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture), true);
                        else if (copperWeight == 1.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, Options.TheOptions.copper_thickness_15oz.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture), true);
                        else if (copperWeight == 2.0)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, Options.TheOptions.copper_thickness_20oz.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture), true);
                        else if (copperWeight == 2.5)
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, Options.TheOptions.copper_thickness_25oz.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture), true);
                    }
                }
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }
    }
}
