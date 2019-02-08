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
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class PressedThicknessDialog : Form
    {
        int xSequentialLamination = 0;
        int nSequentialLamination = 0;
        bool isRestoredOnImport = false;

        public PressedThicknessDialog()
        {
            InitializeComponent();

            //
            string format = "N" + Options.TheOptions.percentDigits;

            isRestoredOnImport = ZPlannerManager.Project.Stackup.CopperCoverageType == Data.Entities.ZCopperCoverageType.RestoredOnImport;
            rbPrepregPercent.Checked = ZPlannerManager.Project.Stackup.CopperCoverageType == Data.Entities.ZCopperCoverageType.PrepregPercent;
            rbPrepregProportional.Checked = ZPlannerManager.Project.Stackup.CopperCoverageType == Data.Entities.ZCopperCoverageType.PrepregProportional;
            rbPrepregManual.Checked = ZPlannerManager.Project.Stackup.CopperCoverageType == Data.Entities.ZCopperCoverageType.ManuallyEntered || isRestoredOnImport;

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
            rbPrepregProportional_CheckedChanged(rbPrepregProportional, EventArgs.Empty);

            cbShowPressedThickness.Checked = ZPlannerManager.IsPressedThickness;
            cbSequentialLamination.Checked = ZPlannerManager.IsSequentialLamination;
            cbKeepImportedPressedThickness.Enabled = ZPlannerManager.IsImported;
            cbKeepImportedPressedThickness.Checked = cbKeepImportedPressedThickness.Enabled && ZPlannerManager.IsKeepImportedPressedThickness;
            checkBoxUpdateSettings.Checked = false;
            
            ZPlannerManager.CalculateSequentialLaminationParams(out xSequentialLamination, out nSequentialLamination);

            cbSequentialLamination_CheckedChanged(cbSequentialLamination, EventArgs.Empty);
        }

        private void DisplaySequentialLamination(int x, int n)
        {
            lSequentialLamination.Text = string.Format("This stackup uses sequential lamination, with {0} build-up layers added after the first lamination pass involving {1} layers. (A {0}-{1}-{0} stackup.)", x, n);
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

        public bool isKeepImportedPressedThickness
        {
            get
            {
                return cbKeepImportedPressedThickness.Checked;
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

            oldStructure.IsKeepImportedPressedThickness = ZPlannerManager.IsKeepImportedPressedThickness;
            newStructure.IsKeepImportedPressedThickness = cbKeepImportedPressedThickness.Checked;

            //save data
            //string format = ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDCopperPercent);
            oldStructure.ForSignal = ZPlannerManager.Project.Stackup.ForSignal;
            double val = 0;
            if (Double.TryParse(tbForSignal.Text, out val))
            {
                //if (ZPlannerManager.Project.Stackup.ForSignal != val) ZPlannerManager.Project.Stackup.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, ZPlannerManager.Project.Stackup.ForSignal.ToString(format, CultureInfo.InvariantCulture), val.ToString(format, CultureInfo.InvariantCulture));
                ZPlannerManager.Project.Stackup.ForSignal = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.forSignal = val;
            }
            newStructure.ForSignal = ZPlannerManager.Project.Stackup.ForSignal;

            oldStructure.ForMixed = ZPlannerManager.Project.Stackup.ForMixed;
            if (Double.TryParse(tbForMixed.Text, out val))
            {
                //if (ZPlannerManager.Project.Stackup.ForMixed != val) ZPlannerManager.Project.Stackup.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, ZPlannerManager.Project.Stackup.ForMixed.ToString(format, CultureInfo.InvariantCulture), val.ToString(format, CultureInfo.InvariantCulture));
                ZPlannerManager.Project.Stackup.ForMixed = val;
                if (checkBoxUpdateSettings.Checked) Options.TheOptions.forMixed = val;
            }
            newStructure.ForMixed = ZPlannerManager.Project.Stackup.ForMixed;

            oldStructure.ForPlane = ZPlannerManager.Project.Stackup.ForPlane;
            if (Double.TryParse(tbForPlane.Text, out val))
            {
                //if (ZPlannerManager.Project.Stackup.ForPlane != val) ZPlannerManager.Project.Stackup.SetAllLayerParameterValues(ZStringConstants.ParameterIDCopperPercent, ZPlannerManager.Project.Stackup.ForPlane.ToString(format, CultureInfo.InvariantCulture), val.ToString(format, CultureInfo.InvariantCulture));
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

            oldStructure.CopperCoverageType = ZPlannerManager.Project.Stackup.CopperCoverageType;
            ZPlannerManager.Project.Stackup.CopperCoverageType = (rbPrepregManual.Checked)
                ? ((isRestoredOnImport) ? ZCopperCoverageType.RestoredOnImport : ZCopperCoverageType.ManuallyEntered)
                : ((rbPrepregProportional.Checked) ? ZCopperCoverageType.PrepregProportional : ZCopperCoverageType.PrepregPercent);
            if (checkBoxUpdateSettings.Checked) Options.TheOptions.ByCopperCoverage = rbPrepregProportional.Checked;
            newStructure.CopperCoverageType = ZPlannerManager.Project.Stackup.CopperCoverageType;

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
        }

        private void rbPrepregProportional_CheckedChanged(object sender, EventArgs e)
        {
            tbForSignal.Enabled = rbPrepregProportional.Checked;
            tbForMixed.Enabled = rbPrepregProportional.Checked;
            tbForPlane.Enabled = rbPrepregProportional.Checked;

        }

        private void cbShowPressedThickness_CheckedChanged(object sender, EventArgs e)
        {
            
        }

        private void cbSequentialLamination_CheckedChanged(object sender, EventArgs e)
        {
            lSequentialLamination.Enabled = cbSequentialLamination.Checked;

            //if (cbSequentialLamination.Checked) 
            DisplaySequentialLamination(xSequentialLamination, nSequentialLamination);
            //else DisplaySequentialLamination(0, 2 * xSequentialLamination + nSequentialLamination);
        }
    }
}
