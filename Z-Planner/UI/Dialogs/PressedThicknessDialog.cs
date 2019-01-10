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
    public partial class PressedThicknessDialog : Form
    {
        public PressedThicknessDialog()
        {
            InitializeComponent();

            //
            string format = "N" + Options.TheOptions.percentDigits;

            rbPrepregPercent.Checked = !ZPlannerManager.Project.Stackup.ByCopperCoverage;
            rbPrepregProportional.Checked = ZPlannerManager.Project.Stackup.ByCopperCoverage;
            tbSignal_Signal.Text = ZPlannerManager.Project.Stackup.ForSignalSignal.ToString(format, CultureInfo.InvariantCulture);
            tbSignal_Mixed.Text = ZPlannerManager.Project.Stackup.ForSignalMixed.ToString(format, CultureInfo.InvariantCulture);
            tbSignal_Plane.Text = ZPlannerManager.Project.Stackup.ForSignalPlane.ToString(format, CultureInfo.InvariantCulture);
            tbMixed_Mixed.Text = ZPlannerManager.Project.Stackup.ForMixedMixed.ToString(format, CultureInfo.InvariantCulture);
            tbMixed_Plane.Text = ZPlannerManager.Project.Stackup.ForMixedPlane.ToString(format, CultureInfo.InvariantCulture);
            tbPlane_Plane.Text = ZPlannerManager.Project.Stackup.ForPlanePlane.ToString(format, CultureInfo.InvariantCulture);

            tbForSignal.Text = ZPlannerManager.Project.Stackup.ForSignal.ToString(format, CultureInfo.InvariantCulture);
            tbForMixed.Text = ZPlannerManager.Project.Stackup.ForMixed.ToString(format, CultureInfo.InvariantCulture);
            tbForPlane.Text = ZPlannerManager.Project.Stackup.ForPlane.ToString(format, CultureInfo.InvariantCulture);

            rbPrepregPercent_CheckedChanged(rbPrepregPercent, EventArgs.Empty);

            cbShowPressedThickness.Checked = ZPlannerManager.IsPressedThickness;
            cbSequentialLamination.Enabled = ZPlannerManager.IsPressedThickness;
            cbSequentialLamination.Checked = ZPlannerManager.IsSequentialLamination;
            checkBoxUpdateSettings.Checked = false;
            int x, n;
            ZPlannerManager.CalculateSequentialLaminationParams(out x, out n);

            lSequentialLaminationX.Text = x.ToString();
            lSequentialLaminationN.Text = n.ToString();
        }

        public bool isShowPressedThickness
        {
            get
            {
                return cbShowPressedThickness.Checked;
            }
        }

        public bool isShowSequentialLamination
        {
            get
            {
                return cbSequentialLamination.Checked;
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            PressedThicknessStructure oldStructure, newStructure;

            oldStructure.IsPressedThickness = ZPlannerManager.IsPressedThickness;
            newStructure.IsPressedThickness = cbShowPressedThickness.Checked;

            oldStructure.IsSequentialLamination = ZPlannerManager.IsSequentialLamination;
            newStructure.IsSequentialLamination = cbSequentialLamination.Checked;

            //save data
            oldStructure.ForSignal = ZPlannerManager.Project.Stackup.ForSignal;
            double val = 0;
            if (Double.TryParse(tbForSignal.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForSignal = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.forSignal = val;
            }
            newStructure.ForSignal = ZPlannerManager.Project.Stackup.ForSignal;

            oldStructure.ForMixed = ZPlannerManager.Project.Stackup.ForMixed;
            if (Double.TryParse(tbForMixed.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForMixed = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.forMixed = val;
            }
            newStructure.ForMixed = ZPlannerManager.Project.Stackup.ForMixed;

            oldStructure.ForPlane = ZPlannerManager.Project.Stackup.ForPlane;
            if (Double.TryParse(tbForPlane.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForPlane = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.forPlane = val;
            }
            newStructure.ForPlane = ZPlannerManager.Project.Stackup.ForPlane;

            oldStructure.ForSignalSignal = ZPlannerManager.Project.Stackup.ForSignalSignal;
            if (Double.TryParse(tbSignal_Signal.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForSignalSignal = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.signal_signal = val;
            }
            newStructure.ForSignalSignal = ZPlannerManager.Project.Stackup.ForSignalSignal;

            oldStructure.ForSignalMixed = ZPlannerManager.Project.Stackup.ForSignalMixed;
            if (Double.TryParse(tbSignal_Mixed.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForSignalMixed = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.signal_mixed = val;
            }
            newStructure.ForSignalMixed = ZPlannerManager.Project.Stackup.ForSignalMixed;

            oldStructure.ForSignalPlane = ZPlannerManager.Project.Stackup.ForSignalPlane;
            if (Double.TryParse(tbSignal_Plane.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForSignalPlane = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.signal_plane = val;
            }
            newStructure.ForSignalPlane = ZPlannerManager.Project.Stackup.ForSignalPlane;

            oldStructure.ForMixedMixed = ZPlannerManager.Project.Stackup.ForMixedMixed;
            if (Double.TryParse(tbMixed_Mixed.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForMixedMixed = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.mixed_mixed = val;
            }
            newStructure.ForMixedMixed = ZPlannerManager.Project.Stackup.ForMixedMixed;

            oldStructure.ForMixedPlane = ZPlannerManager.Project.Stackup.ForMixedPlane;
            if (Double.TryParse(tbMixed_Plane.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForMixedPlane = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.mixed_plane = val;
            }
            newStructure.ForMixedPlane = ZPlannerManager.Project.Stackup.ForMixedPlane;

            oldStructure.ForPlanePlane = ZPlannerManager.Project.Stackup.ForPlanePlane;
            if (Double.TryParse(tbPlane_Plane.Text, out val))
            {
                ZPlannerManager.Project.Stackup.ForPlanePlane = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.plane_plane = val;
            }
            newStructure.ForPlanePlane = ZPlannerManager.Project.Stackup.ForPlanePlane;

            oldStructure.ByCopperCoverage = ZPlannerManager.Project.Stackup.ByCopperCoverage;
            ZPlannerManager.Project.Stackup.ByCopperCoverage = rbPrepregProportional.Checked;
            if (checkBoxUpdateSettings.Checked) Options.TheOptions.ByCopperCoverage = rbPrepregProportional.Checked;
            newStructure.ByCopperCoverage = ZPlannerManager.Project.Stackup.ByCopperCoverage;

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeStackupPressedThicknessCommand(ZPlannerManager.Project.Stackup, oldStructure, newStructure);

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

        private void rbPrepregPercent_CheckedChanged(object sender, EventArgs e)
        {
            tbSignal_Signal.Enabled = rbPrepregPercent.Checked;
            tbSignal_Mixed.Enabled = rbPrepregPercent.Checked;
            tbSignal_Plane.Enabled = rbPrepregPercent.Checked;
            tbMixed_Mixed.Enabled = rbPrepregPercent.Checked;
            tbMixed_Plane.Enabled = rbPrepregPercent.Checked;
            tbPlane_Plane.Enabled = rbPrepregPercent.Checked;

            tbForSignal.Enabled = !rbPrepregPercent.Checked;
            tbForMixed.Enabled = !rbPrepregPercent.Checked;
            tbForPlane.Enabled = !rbPrepregPercent.Checked;
        }

        private void cbShowPressedThickness_CheckedChanged(object sender, EventArgs e)
        {
            cbSequentialLamination.Enabled = cbShowPressedThickness.Checked;
        }
    }
}
