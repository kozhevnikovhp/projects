using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public enum SettingsOpenMode
    {
        full,
        license,
        ffs
    }
    public partial class SettingsDialog : Form
    {
        private bool bLicMode;
        public SettingsDialog(SettingsOpenMode mode = SettingsOpenMode.full)
        {
            InitializeComponent();
            LoadOptions();
#if ZSANDBOX==false
            if (ZPlannerManager.rights != null && !ZPlannerManager.rights.AllowDmlUpdate) { cbZZeroSynchronize.Checked = false; cbZZeroSynchronize.Enabled = false; }
#endif
            bLicMode = mode == SettingsOpenMode.license;

            switch (mode)
            {
                case SettingsOpenMode.license:
                    tabOptions.SelectedTab = tabPaths;
                    foreach (TabPage c in tabOptions.Controls)
                    {
                        if (c.Name != "tabPaths")
                        {
                            c.Enabled = false;
                        }
                    }
                    break;
                case SettingsOpenMode.ffs:
                    tabOptions.TabPages.RemoveByKey("tabPrepreg");
                    //tabOptions.TabPages.RemoveByKey("tabPaths");
                    tabOptions.TabPages.RemoveByKey("tabUser");
                    tabOptions.TabPages.RemoveByKey("tabGeneral");
                    ChangePathsTab();
                    break;
            }
        }

        private void ChangePathsTab()
        {
            groupBox8.Enabled = false;
            groupBox8.Visible = false;
            label6.Enabled = false;
            label6.Visible = false;
            tbExportPath.Enabled = false;
            tbExportPath.Visible = false;
            btnExportPath.Enabled = false;
            btnExportPath.Visible = false;

            groupBox9.Location = new Point(groupBox9.Location.X, cbUseLast.Location.Y);
            cbUseLast.Location = new Point(cbUseLast.Location.X, tbExportPath.Location.Y);

            label2.Text = "Sandbox data path";
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
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

        //get paths
        void SetPath(TextBox tb)
        {
            string initial_path = tb.Text;

            FolderBrowserDialog fbd = new FolderBrowserDialog();
            
            if (initial_path != String.Empty) fbd.SelectedPath = initial_path;

            DialogResult result = fbd.ShowDialog();
            if (result == DialogResult.OK && !string.IsNullOrWhiteSpace(fbd.SelectedPath))
            {
                tb.Text = fbd.SelectedPath;
            }
 
        }

        void SetFile(TextBox tb)
        {
            string initial_path = Path.GetDirectoryName(tb.Text);

            OpenFileDialog ofd = new OpenFileDialog();
            ofd.RestoreDirectory = true;

            if (initial_path != String.Empty) ofd.InitialDirectory = initial_path;

            DialogResult result = ofd.ShowDialog();
            if (result == DialogResult.OK && !File.Exists(ofd.FileName))
            {
                tb.Text = ofd.FileName;
            }
        }

// data exchange
        public void SaveOptions()
        {
            Options options = Options.TheOptions;
            // units
            if (rbUnitsEnglish.Checked) options.units = Options.Units.English; else  options.units = Options.Units.Metric;

            options.dateFormat = Options.DateFormat.European;
            if (rbDateFormatUS.Checked) options.dateFormat = Options.DateFormat.US;
            else if (rbDateFormatEnglish.Checked) options.dateFormat = Options.DateFormat.English;

            // Should be removed (#241: Settings dialog tab redesign)
            //if (rbMetalThicknessWeight.Checked) options.metalThickness = Options.MetalThickness.Weight; else options.metalThickness = Options.MetalThickness.Length;

            // paths
            options.ProjectPath = tbProjectPath.Text;
            options.UseLast = cbUseLast.Checked;
            options.ExportPath = tbExportPath.Text;
            options.DML_LocalPath = tbDML_LocalPath.Text;
            options.DML_NetworkPath = tbDML_NetworkPath.Text;
            options.LicensePath = tbLicPaths.Text;
            // galery??

            //dielectric
            options.drill = Settings.Options.TheOptions.convertCurrentDrillDiameterUnitsToMils(Convert.ToDouble(tbDrill.Text));
            options.soldermask_height = Settings.Options.TheOptions.convertCurrentSolderMaskHeightUnitsToMils(Convert.ToDouble(tbSoldermask_Height.Text));
            options.soldermask_Dk = Convert.ToDouble(tbSoldermask_Dk.Text);
            options.soldermask_Df = Convert.ToDouble(tbSoldermask_Df.Text);

            options.height = Settings.Options.TheOptions.convertCurrentDielectricHeightUnitsToMils(Convert.ToDouble(tbCorePrepregHeight.Text));

            options.Dk = Convert.ToDouble(tbDk.Text);
            options.Df = Convert.ToDouble(tbDf.Text);
            options.resin_Dk = Convert.ToDouble(tbResin_Dk.Text);
            options.resin_Df = Convert.ToDouble(tbResin_Df.Text);

            //metal
            options.trace_width = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbOuter_Diff_Trace_Width.Text), ZStringConstants.ParameterIDZo_TraceWidth);
            options.trace_spacing = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbOuter_Trace_Spacing.Text), ZStringConstants.ParameterIDZo_TraceSpacing);
            options.etch = Convert.ToDouble(tbEtch.Text);

            //outer metal layers
            options.plating_thickness = Convert.ToDouble(cbPlating_Thickness.Text);
            options.base_trace_thickness = Convert.ToDouble(cbBase_Trace_Thickness.Text);
            options.roughness = Convert.ToDouble(tbRoughness.Text);
            options.outer_trace_width = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbOuter_Trace_Width.Text), ZStringConstants.ParameterIDZo_TraceWidth);
            options.outer_diff_trace_width = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbOuter_Diff_Trace_Width.Text), ZStringConstants.ParameterIDZdiff_TraceWidth);
            options.outer_diff_trace_spacing = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbOuter_Trace_Spacing.Text), ZStringConstants.ParameterIDZdiff_TraceSpacing);

            //inner metal layers
            options.copper_foil_thickness = Convert.ToDouble(cbCopper_Foil_Thickness.Text);
            options.laminate_side_roughness = Convert.ToDouble(tbLaminate_Side_Rougness.Text);
            options.prepreg_side_roughness = Convert.ToDouble(tbPrepreg_Side_Roughness.Text);
            options.inner_trace_width = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbInner_Trace_Width.Text), ZStringConstants.ParameterIDZo_TraceWidth);
            options.inner_diff_trace_width = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbInner_Diff_Trace_Width.Text), ZStringConstants.ParameterIDZdiff_TraceWidth);
            options.inner_diff_trace_spacing = Settings.Options.TheOptions.convertCurrentUnitsToMils(Convert.ToDouble(tbInner_Trace_Spacing.Text), ZStringConstants.ParameterIDZdiff_TraceSpacing);

            // copper foil thickness for copper weight
            options.copper_thickness_05oz = Convert.ToDouble(tbCopperThickness_05.Text);
            options.copper_thickness_10oz = Convert.ToDouble(tbCopperThickness_10.Text);
            options.copper_thickness_15oz = Convert.ToDouble(tbCopperThickness_15.Text);
            options.copper_thickness_20oz = Convert.ToDouble(tbCopperThickness_20.Text);
            options.copper_thickness_25oz = Convert.ToDouble(tbCopperThickness_25.Text);

            options.bulkRes = Convert.ToDouble(tbBulkRes.Text);

            // Prepreg Thickness Adjustments
            options.ByCopperCoverage = rbPrepregProportional.Checked;
                    //ByCopperCoverage = true;
            options.forSignal = Convert.ToDouble(tbForSignal.Text);
            options.forMixed = Convert.ToDouble(tbForMixed.Text);
            options.forPlane = Convert.ToDouble(tbForPlane.Text);

            //ByCopperCoverage = false
            // prepreg percentage adjustment
            //prepreg is pressed between
            options.signal_signal = Convert.ToDouble(tbSignal_Signal.Text);
            options.signal_mixed = Convert.ToDouble(tbSignal_Mixed.Text);
            options.signal_plane = Convert.ToDouble(tbSignal_Plane.Text);
            options.mixed_mixed = Convert.ToDouble(tbMixed_Mixed.Text);
            options.mixed_plane = Convert.ToDouble(tbMixed_Plane.Text);
            options.plane_plane = Convert.ToDouble(tbPlane_Plane.Text);

            options.Frequency = Convert.ToDouble(tbFrequency.Text);

            options.weightDigits = Convert.ToByte(tbWeightDigits.Text);
            options.lengthDigits = Convert.ToByte(tbLengthDigits.Text); 
            options.impedanceDigits = Convert.ToByte(tbImpedanceDigits.Text); 
            options.temperatureDigits = Convert.ToByte(tbTemperatureDigits.Text); 
            options.dkDigits = Convert.ToByte(tbDkDigits.Text); 
            options.dfDigits = Convert.ToByte(tbDfDigits.Text); 
            options.percentDigits = Convert.ToByte(tbPercentDigits.Text);
            options.frequencyDigits = Convert.ToByte(tbFrequencyDigits.Text);

            options.allowZZeroLibrary = cbZZeroLibrary.Checked;
            options.allowCorporateLibrary = cbCorporateLibrary.Checked;
            options.allowLocalLibrary = cbLocalLibrary.Checked;
            options.allowZZeroSynchronization = cbZZeroSynchronize.Checked;

            options.allowStackupSwitches = cbStackupSwitches.Checked;

            options.autosaveInterval = (int)nudAutosave.Value;

            options.displayDRCErrors = cbDRCErrors.Checked;
            options.displayDRCWarnings = cbDRCWarnings.Checked;

            // User
            options.UserName = tbUserName.Text;
            options.Title = tbInitials.Text;
            options.Company = tbCompany.Text;
            options.Division = tbDivision.Text;
            options.Site = tbSiteLocation.Text;
            options.UserEMail = tbEMail.Text;
            options.UserLogo = tbCompanyLogo.Text;

            Options.TheOptions = options;

            Settings_IO io = new Settings_IO();
            io.WriteSettings(ref options);
        }
  
        public void LoadOptions()
        {
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ReadSettings();
#else
            ZPlannerManager.ReadSettings();
#endif
            Options options = Options.TheOptions;

            // units
            rbUnitsEnglish.Checked = (options.units == Options.Units.English);
            rbUnitsMetric.Checked = (options.units == Options.Units.Metric);

            switch (options.dateFormat)
            {
                case Options.DateFormat.US:
                    rbDateFormatUS.Checked = true;
                    break;
                case Options.DateFormat.English:
                    rbDateFormatEnglish.Checked = true;
                    break;
                case Options.DateFormat.European:
                    rbDateFormatEuropean.Checked = true;
                    break;
            }

            // Should be removed (#241: Settings dialog tab redesign)
            //rbMetalThicknessWeight.Checked =  (options.metalThickness == Options.MetalThickness.Weight); 

            // paths
            if (string.IsNullOrWhiteSpace(options.ProjectPath)) options.ProjectPath = 
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;
#else
                ZPlannerManager.ZPlannerDataDirectory;
#endif
            tbProjectPath.Text = options.ProjectPath;

            cbUseLast.Checked = options.UseLast;
            if (string.IsNullOrWhiteSpace(options.ExportPath)) options.ExportPath = 
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;
#else
                ZPlannerManager.ZPlannerDataDirectory;
#endif
            tbExportPath.Text = options.ExportPath;

            if (string.IsNullOrWhiteSpace(options.DML_LocalPath)) options.DML_LocalPath = 
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;
#else
                ZPlannerManager.ZPlannerDataDirectory;
#endif
            tbDML_LocalPath.Text = options.DML_LocalPath;
            if (string.IsNullOrWhiteSpace(options.DML_NetworkPath)) options.DML_NetworkPath = 
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;
#else
                ZPlannerManager.ZPlannerDataDirectory;
#endif
            tbDML_NetworkPath.Text = options.DML_NetworkPath;

            //license
            if (string.IsNullOrWhiteSpace(options.LicensePath))
            {
                string LicPath = Path.Combine(
#if ZSANDBOX
                    ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory,
#else
                    ZPlannerManager.ZPlannerDataDirectory,
#endif
                    "License");
                this.lDefaultLicFoldef.Text = LicPath;
                string p2 = Environment.GetEnvironmentVariable("ZZERO_LICENSE_FILE");
                if (!String.IsNullOrEmpty(p2))
                {
                    LicPath = LicPath + Environment.NewLine + p2;
                }
                options.LicensePath = LicPath;
            }
            tbLicPaths.Text = options.LicensePath;

            //dielectric
            tbDrill.Text = Settings.Options.TheOptions.convertMilsToCurrentDrillDiameterUnits(options.drill).ToString();
            tbSoldermask_Height.Text = Settings.Options.TheOptions.convertMilsToCurrentSolderMaskHeightUnits(options.soldermask_height).ToString();
            tbSoldermask_Dk.Text = options.soldermask_Dk.ToString();
            tbSoldermask_Df.Text = options.soldermask_Df.ToString();

            tbCorePrepregHeight.Text = Settings.Options.TheOptions.convertMilsToCurrentDielectricHeightUnits(options.height).ToString();
            tbDk.Text = options.Dk.ToString();
            tbDf.Text = options.Df.ToString();
            tbResin_Dk.Text = options.resin_Dk.ToString();
            tbResin_Df.Text = options.resin_Df.ToString();

            //metal
            tbEtch.Text = options.etch.ToString();

            //outer metal layers
            SetFormatedValue(cbPlating_Thickness, options.plating_thickness.ToString(), options.weightDigits);
            SetFormatedValue(cbBase_Trace_Thickness, options.base_trace_thickness.ToString(), options.weightDigits);
            tbRoughness.Text = options.roughness.ToString();
            tbOuter_Trace_Width.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.outer_trace_width, ZStringConstants.ParameterIDZo_TraceWidth).ToString();
            tbOuter_Diff_Trace_Width.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.outer_diff_trace_width, ZStringConstants.ParameterIDZdiff_TraceWidth).ToString();
            tbOuter_Trace_Spacing.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.outer_diff_trace_spacing, ZStringConstants.ParameterIDZdiff_TraceSpacing).ToString();

            //inner metal layers
            SetFormatedValue(cbCopper_Foil_Thickness, options.copper_foil_thickness.ToString(), options.weightDigits);
            tbLaminate_Side_Rougness.Text = options.laminate_side_roughness.ToString();
            tbPrepreg_Side_Roughness.Text = options.prepreg_side_roughness.ToString();
            tbInner_Trace_Width.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.inner_trace_width, ZStringConstants.ParameterIDZo_TraceWidth).ToString();
            tbInner_Diff_Trace_Width.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.inner_diff_trace_width, ZStringConstants.ParameterIDZdiff_TraceWidth).ToString();
            tbInner_Trace_Spacing.Text = Settings.Options.TheOptions.convertMilsToCurrentUnits(options.inner_diff_trace_spacing, ZStringConstants.ParameterIDZdiff_TraceSpacing).ToString();

            // copper foil thickness for copper weight
            SetFormatedValue(tbCopperThickness_05, options.copper_thickness_05oz.ToString(), options.lengthDigits);
            SetFormatedValue(tbCopperThickness_10, options.copper_thickness_10oz.ToString(), options.lengthDigits);
            SetFormatedValue(tbCopperThickness_15, options.copper_thickness_15oz.ToString(), options.lengthDigits);
            SetFormatedValue(tbCopperThickness_20, options.copper_thickness_20oz.ToString(), options.lengthDigits);
            SetFormatedValue(tbCopperThickness_25, options.copper_thickness_25oz.ToString(), options.lengthDigits);

            tbBulkRes.Text = options.bulkRes.ToString("e3");

            // Prepreg Thickness Adjustments
            rbPrepregProportional.Checked = options.ByCopperCoverage == true;
            tbForSignal.Text = options.forSignal.ToString();
            tbForMixed.Text = options.forMixed.ToString();
            tbForPlane.Text = options.forPlane.ToString();

            //ByCopperCoverage = false
            // prepreg percentage adjustment
            //prepreg is pressed between
            tbSignal_Signal.Text = options.signal_signal.ToString();
            tbSignal_Mixed.Text = options.signal_mixed.ToString();
            tbSignal_Plane.Text = options.signal_plane.ToString();
            tbMixed_Mixed.Text = options.mixed_mixed.ToString();
            tbMixed_Plane.Text = options.mixed_plane.ToString();
            tbPlane_Plane.Text = options.plane_plane.ToString();

            rbPrepregPercent_CheckedChanged(rbPrepregPercent, EventArgs.Empty);

            tbFrequency.Text = options.Frequency.ToString();

            tbWeightDigits.Text = options.weightDigits.ToString(); 
            tbLengthDigits.Text =  options.lengthDigits.ToString(); 
            tbImpedanceDigits.Text =  options.impedanceDigits.ToString(); 
            tbTemperatureDigits.Text =  options.temperatureDigits.ToString(); 
            tbDkDigits.Text =  options.dkDigits.ToString(); 
            tbDfDigits.Text =  options.dfDigits.ToString();
            tbPercentDigits.Text = options.percentDigits.ToString();
            tbFrequencyDigits.Text = options.frequencyDigits.ToString();

            cbZZeroLibrary.Checked = options.allowZZeroLibrary;
            cbCorporateLibrary.Checked = options.allowCorporateLibrary;
            cbLocalLibrary.Checked = options.allowLocalLibrary;
            cbZZeroSynchronize.Checked = options.allowZZeroSynchronization;

            cbStackupSwitches.Checked = options.allowStackupSwitches;

            nudAutosave.Value = (decimal)options.autosaveInterval;

            cbDRCErrors.Checked = options.displayDRCErrors;
            cbDRCWarnings.Checked = options.displayDRCWarnings;

            tbWeightDigits_TextChanged(tbWeightDigits, EventArgs.Empty);
            tbLengthDigits_TextChanged(tbWeightDigits, EventArgs.Empty);
            tbDkDigits_TextChanged(tbWeightDigits, EventArgs.Empty);
            tbDfDigits_TextChanged(tbWeightDigits, EventArgs.Empty);
            tbFrequencyDigits_TextChanged(tbWeightDigits, EventArgs.Empty);
            tbPercentDigits_TextChanged(tbWeightDigits, EventArgs.Empty);

            // User
            tbUserName.Text = options.UserName ?? string.Empty;
            tbInitials.Text = options.Title ?? string.Empty;
            tbCompany.Text = options.Company ?? string.Empty;
            tbDivision.Text = options.Division ?? string.Empty;
            tbSiteLocation.Text = options.Site ?? string.Empty;
            tbEMail.Text = options.UserEMail ?? string.Empty;
            tbCompanyLogo.Text = options.UserLogo ?? string.Empty;
            if (!string.IsNullOrWhiteSpace(options.UserLogo)) ShowImage(Path.Combine(ZSettings.SettingsFolder, options.UserLogo));
        }

        private void ShowImage(string fileToDisplay)
        {
            using(FileStream fs = File.OpenRead(fileToDisplay))
            {
                pbCompanyLogo.Image = new Bitmap(fs);
            }
        }

        private void btnProjectPath_Click(object sender, EventArgs e)
        {
            SetPath(tbProjectPath);
        }

        private void btnDML_NetworkPath_Click(object sender, EventArgs e)
        {
            SetFile(tbDML_NetworkPath);
        }

        private void btnDML_LocalPath_Click(object sender, EventArgs e)
        {
            SetFile(tbDML_LocalPath);
        }

        private void btnExportPath_Click(object sender, EventArgs e)
        {
            SetPath(tbExportPath);
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            Options.Units oldUnits = Options.TheOptions.units;
            SaveOptions();
            if (Options.TheOptions.units != oldUnits)
                ZPlannerManager.UnitsChangedForEverywhere();
            this.DialogResult = DialogResult.OK;
            Close();
        }

        private void tbSoldermask_Height_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Solder Mask Height",
                Settings.Options.TheOptions.convertMilsToCurrentSolderMaskHeightUnits(1.5),  // convert to current units - the logic is the same as for board thickness
                Settings.Options.TheOptions.convertMilsToCurrentSolderMaskHeightUnits(50));  // convert to current units - the logic is the same as for board thickness

        }

        private void tbHeight_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Core/Prepreg Height",
                Settings.Options.TheOptions.convertMilsToCurrentDielectricHeightUnits(1.5),  // convert to current units - the logic is the same as for board thickness
                Settings.Options.TheOptions.convertMilsToCurrentDielectricHeightUnits(50));  // convert to current units - the logic is the same as for board thickness
        }

        private void tbWeightDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbWeightDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(cbPlating_Thickness, digitsNumber);
                SetFormatedValue(cbBase_Trace_Thickness, digitsNumber);
                SetFormatedValue(cbCopper_Foil_Thickness, digitsNumber);
            }
        }

        private void tbLengthDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbLengthDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(tbDrill, digitsNumber);
                SetFormatedValue(tbSoldermask_Height, digitsNumber);
                SetFormatedValue(tbCorePrepregHeight, digitsNumber);
                SetFormatedValue(tbOuter_Diff_Trace_Width, digitsNumber);
                SetFormatedValue(tbOuter_Trace_Spacing, digitsNumber);
                SetFormatedValue(tbOuter_Trace_Width, digitsNumber);
                SetFormatedValue(tbInner_Diff_Trace_Width, digitsNumber);
                SetFormatedValue(tbInner_Trace_Spacing, digitsNumber);
                SetFormatedValue(tbInner_Trace_Width, digitsNumber);
                SetFormatedValue(tbRoughness, digitsNumber);
                SetFormatedValue(tbLaminate_Side_Rougness, digitsNumber);
                SetFormatedValue(tbPrepreg_Side_Roughness, digitsNumber);
                SetFormatedValue(tbCopperThickness_05, digitsNumber);
                SetFormatedValue(tbCopperThickness_10, digitsNumber);
                SetFormatedValue(tbCopperThickness_15, digitsNumber);
                SetFormatedValue(tbCopperThickness_20, digitsNumber);
                SetFormatedValue(tbCopperThickness_25, digitsNumber);
            }
        }

        private void tbDkDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbDkDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(tbSoldermask_Dk, digitsNumber);
                SetFormatedValue(tbDk, digitsNumber);
                SetFormatedValue(tbResin_Dk, digitsNumber);
            }
        }

        private void tbDfDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbDfDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(tbSoldermask_Df, digitsNumber);
                SetFormatedValue(tbDf, digitsNumber);
                SetFormatedValue(tbResin_Df, digitsNumber);
            }
        }

        private void tbFrequencyDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbFrequencyDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(tbFrequency, digitsNumber);
            }
        }

        private void tbPercentDigits_TextChanged(object sender, EventArgs e)
        {
            string digitsString = tbPercentDigits.Text.Trim();
            int digitsNumber;
            if (string.IsNullOrWhiteSpace(digitsString)) return;
            if (int.TryParse(digitsString, NumberStyles.Any, CultureInfo.InvariantCulture, out digitsNumber))
            {
                SetFormatedValue(tbEtch, digitsNumber);
                SetFormatedValue(tbSignal_Signal, digitsNumber);
                SetFormatedValue(tbSignal_Mixed, digitsNumber);
                SetFormatedValue(tbSignal_Plane, digitsNumber);
                SetFormatedValue(tbMixed_Mixed, digitsNumber);
                SetFormatedValue(tbMixed_Plane, digitsNumber);
                SetFormatedValue(tbPlane_Plane, digitsNumber);
                SetFormatedValue(tbForSignal, digitsNumber);
                SetFormatedValue(tbForMixed, digitsNumber);
                SetFormatedValue(tbForPlane, digitsNumber);
            }
        }

        private void SetFormatedValue(TextBox control, int digitsNumber)
        {
            string sValue = control.Text.Trim();
            double dValue;

            if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                control.Text = dValue.ToString("N" + digitsNumber, CultureInfo.InvariantCulture);
            }
        }

        private void SetFormatedValue(TextBox control, string sValue, int digitsNumber)
        {
            sValue = sValue.Trim();
            double dValue;

            if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                control.Text = dValue.ToString("N" + digitsNumber, CultureInfo.InvariantCulture);
            }
        }

        private void SetFormatedValue(ComboBox control, int digitsNumber)
        {
            string sValue = control.Text.Trim();
            SetFormatedValue(control, sValue, digitsNumber);
        }

        private void SetFormatedValue(ComboBox control, string sValue, int digitsNumber)
        {
            double dValue;

            string[] items = new string[control.Items.Count];
            for (int i = 0; i < control.Items.Count; ++i)
            {
                string sItem = (string)control.Items[i];
                double dItem;
                if (double.TryParse(sItem, NumberStyles.Any, CultureInfo.InvariantCulture, out dItem))
                {
                    items[i] = dItem.ToString("N" + digitsNumber, CultureInfo.InvariantCulture);
                }
                else
                {
                    items[i] = sItem;
                }
            }

            control.Items.Clear();
            control.Items.AddRange(items);

            if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                control.Text = dValue.ToString("N" + digitsNumber, CultureInfo.InvariantCulture);
            }
        }

        private void bCompanyLogo_Click(object sender, EventArgs e)
        {
            string initial_path = Path.Combine(ZSettings.SettingsFolder);

            OpenFileDialog fd = new OpenFileDialog();
            fd.RestoreDirectory = true;
            fd.CheckFileExists = true;
            fd.Filter = "ImagePEG Files (*.jpeg; *.jpg; *.png; *.gif; *.bmp)|*.jpeg;*.jpg;*.png;*.gif;*.bmp|All files (*.*)|*.*";
            fd.Title = "Open company Logo file.";

            if (initial_path != String.Empty) fd.InitialDirectory = initial_path;

            DialogResult result = fd.ShowDialog();
            if (result == DialogResult.OK && !string.IsNullOrWhiteSpace(fd.FileName))
            {
                if (Path.GetFileName(fd.FileName) == Path.GetFileName(ZSettings.SettingsFile) || Path.Combine(ZSettings.SettingsFolder, tbCompanyLogo.Text) == fd.FileName) return;

                if (fd.FileName != Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName))) File.Copy(fd.FileName, Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName)), true);
                string logoPath = fd.FileName;

                tbCompanyLogo.Text = Path.GetFileName(fd.FileName);
                ShowImage(Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName)));
            }
        }

        private void btnLicAdd_Click(object sender, EventArgs e)
        {
            OpenFileDialog licFileDlg = new OpenFileDialog();

            licFileDlg.Filter = "License files (*.lic)|*.lic";
            licFileDlg.FilterIndex = 1;
            licFileDlg.RestoreDirectory = true;

            if (licFileDlg.ShowDialog() == DialogResult.OK && licFileDlg.FileName != string.Empty)
            {
                tbLicPaths.Text += Environment.NewLine;
                tbLicPaths.Text += new FileInfo(licFileDlg.FileName).Directory.FullName;
            }
 
        }

        private void btnLicUpload_Click(object sender, EventArgs e)
        {
            string LicPath = Path.Combine(
#if ZSANDBOX
                    ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory,
#else
                    ZPlannerManager.ZPlannerDataDirectory,
#endif
                    "License");

            OpenFileDialog licFileDlg = new OpenFileDialog();

            licFileDlg.Filter = "License files (*.lic)|*.lic";
            licFileDlg.FilterIndex = 1;
            licFileDlg.RestoreDirectory = true;

            if (licFileDlg.ShowDialog() == DialogResult.OK && licFileDlg.FileName != string.Empty && File.Exists(licFileDlg.FileName))
            {
                File.Copy(licFileDlg.FileName, Path.Combine(LicPath, Path.GetFileName(licFileDlg.FileName)));

                if (!tbLicPaths.Text.Contains(LicPath))
                {
                    if (tbLicPaths.Text != string.Empty) tbLicPaths.Text += Environment.NewLine;
                    tbLicPaths.Text += LicPath;
                }
            }
        }

        private void tabOptions_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (bLicMode)
            {
                tabOptions.SelectedTab = tabPaths;
            }
        }

        private void combobox_DrawItem(object sender, DrawItemEventArgs e)
        {

            // By using Sender, one method could handle multiple ComboBoxes
            ComboBox cbx = sender as ComboBox;
            if (cbx != null)
            {
                // Always draw the background
                e.DrawBackground();
 
                // Drawing one of the items?
                if (e.Index >= 0)
                {
                    // Set the string alignment.  Choices are Center, Near and Far
                    StringFormat sf = new StringFormat();
                    sf.LineAlignment = StringAlignment.Center;
                    sf.Alignment = ((e.State & DrawItemState.ComboBoxEdit) == DrawItemState.ComboBoxEdit) ? StringAlignment.Far : StringAlignment.Center;
 
                    // Set the Brush to ComboBox ForeColor to maintain any ComboBox color settings
                    // Assumes Brush is solid
                    Brush brush = new SolidBrush(cbx.ForeColor);
 
                    // If drawing highlighted selection, change brush
                    if ((e.State & DrawItemState.Selected) == DrawItemState.Selected)
                    brush = SystemBrushes.HighlightText;
 
                    // Draw the string
                    e.Graphics.DrawString(cbx.Items[e.Index].ToString(), cbx.Font, brush, e.Bounds, sf);
                }
            }
        }
    }
}
