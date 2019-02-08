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
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Translators;
using ZZero.ZPlanner.UI.Dialogs;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ImportWizard : Form
    {
        private ZStackup stackup;
        private string importType = string.Empty;
        string fabricator;
        private int curIndex = 0;
        private bool materialWasFound = false;
        private PictureBox[] markers;
        private TabPage[] pages;
        //private ImportWizardData wizardData;
        private const string dialogTitlePrefix = "Stackup Import Wizard";
        private const string unknownValue = "<select or type>";
        private const string NextButtonText = "Next >";
        private const string FinishButtonText = "Finish";
        ZLibraryCategory[] libraryPriority;

        private Dictionary<string, List<string>> materialDictionary = new Dictionary<string, List<string>>();

        private Dictionary<CopperThicknessStructure, TextBox[]> CopperThicknessTextBoxes = new Dictionary<CopperThicknessStructure, TextBox[]>();
        private Dictionary<string, TextBox[]> SingleTextBoxes = new Dictionary<string, TextBox[]>();
        private Dictionary<string, TextBox[]> PairTextBoxes = new Dictionary<string, TextBox[]>();

        private bool isWizardFinished = false;

        public ImportWizard()
        {
            InitializeComponent();

            markers = new PictureBox[] { pictStep1, pictStep2, pictStep3, pictStep4, pictStep5, pictStep6 };
            pages = new TabPage[] { tabPage1, tabPage2, tabPage3, tabPage4, tabPage5, tabPage6 };

            //wizardData = new ImportWizardData();

            //get rid of tab headers
            tabControlWizard.Appearance = TabAppearance.FlatButtons;
            tabControlWizard.ItemSize = new Size(0, 1);
            tabControlWizard.SizeMode = TabSizeMode.Fixed;

            foreach (TabPage tab in tabControlWizard.TabPages)
            {
                tab.Text = "";
            }

            ChangeState(1, 0);

            FillManufacturerAndMaterialLists();
        }

        private void ChangeState(int newIndex, int oldIndex)
        {
            string progressMessage = "Preparing Stackup ...";
            bool isIgnoreUpdateActiveStackup = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            bool isIgnoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();
            ZPlannerManager.ContainerPanel.SuspendLayout();
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.SuspendLayout();
            bool back=false, next=false;

            try
            {
                ZPlannerManager.StatusMenu.StartProgress(progressMessage, true);
                bBack.Enabled = false;
                bNext.Enabled = false;
                this.Update();

                if (oldIndex < newIndex)
                {
                    switch (oldIndex)
                    {
                        case 1:
                            if (!File.Exists(tbFile.Text) && !Directory.Exists(tbFile.Text)) return;
                            if (!ImportFile()) return;
                            break;
                        case 2:
                            SetManufacturer();
                            break;
                        case 3:
                            SetResinDkDf();
                            SetSequentialLamination();
                            break;
                        case 4:
                            SetCopperThickness();
                            SetPressedThickness();
                            break;
                        case 5:
                            SetImpedances();
                            break;
                        case 6:
                            SetRoughness();
                            break;
                        default:

                            break;
                    }
                }
            
                curIndex = newIndex;

                switch (newIndex)
                {
                    case 1:
                        FillLibraryList();
                        back = false;
                        next = File.Exists(tbFile.Text);
                        bNext.Text = NextButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Select Stackup File and Library Search Sequence");
                        break;
                    case 2:
                        back = true;
                        next = FindManufacturer();
                        bNext.Text = NextButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Laminate Manufacturer and Material");
                        break;
                    case 3:
                        FillResinDkDf();
                        CalculateSequentialLamination();
                        back = true;
                        next = true;
                        bNext.Text = NextButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Resin Properties and Sequential Lamination");
                        break;
                    case 4:
                        FindCopperThickness();
                        FillCopperThickness();
                        FillPressedThickness();
                        back = true;
                        next = true;
                        bNext.Text = NextButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Copper Foil Thickness and Copper Coverage");
                        break;
                    case 5:
                        FillImpedances();
                        back = true;
                        next = true;
                        bNext.Text = NextButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Target Impedances");
                        break;
                    case 6:
                        FillRoughness();
                        back = true;
                        next = true;
                        bNext.Text = FinishButtonText;
                        this.Text = string.Format("{0} - {1}", dialogTitlePrefix, "Copper Roughness");
                        break;
                    default:
                        FinishImport();
                        return;
                }

                if (curIndex > pages.Length) return;

                this.tabControlWizard.SelectedTab = pages[curIndex - 1];

                if (oldIndex > 0) markers[oldIndex - 1].BackColor = Color.DarkGray;
                markers[newIndex - 1].BackColor = Color.Blue;

                this.Update();
            }
            finally
            {
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ResumeLayout();
                ZPlannerManager.ContainerPanel.ResumeLayout();
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreUpdateActiveStackup);
                if (curIndex > 1) ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnoreCommands);
                ZPlannerManager.StatusMenu.StopProgress(progressMessage);
            }

            bBack.Enabled = back;
            bNext.Enabled = next;
        }

        private void FillLibraryList()
        {
            clbLibraryOrder.SelectedIndex = 0;
            for (int i = 0; i < clbLibraryOrder.Items.Count; ++i)
                clbLibraryOrder.SetItemChecked(i, true);
        }

        private bool ImportFile()
        {
            bool isIgnoreCommands = ZPlannerManager.Commands.IsIgnoreCommands;
            try
            {
                if (!ZPlannerManager.CloseProject()) return false;
                stackup = null;
                ZPlannerManager.Commands.SuspendCommandEvent();

                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.StartProgress("Importing Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ZPlannerManager.IsAutoMirror = false;
                Options.TheOptions.enabledIsAutoMirror = false;

                string filePath = tbFile.Text;
                bool pressedThickness = cbPressedThicknessEnabled.Checked;
                libraryPriority = GetLibraryPriority();
                string[] formatNames = new string[] { "ODBPP", "HL", "IPC", "ISU", "Tapestry", "TTM", "WUS" };

                IImport parser = null;

                foreach (string format in formatNames)
                {
                    switch (format)
                    {
                        case "ODBPP":
                            parser = new ODBPP_Parser(filePath, pressedThickness, libraryPriority);
                            break;
                        case "HL":
                            parser = new HL_Parser(filePath, pressedThickness, libraryPriority);
                            break;
                        case "IPC":
                            parser = new IPC_Parser(filePath, pressedThickness, libraryPriority);
                            break;
                        case "ISU":
                            parser = new ISU_Reader(filePath, pressedThickness, libraryPriority);
                            break;
                        case "Tapestry":
                            parser = new TapestryReader(filePath, pressedThickness, libraryPriority);
                            break;
                        case "TTM":
                            parser = new TTM_Reader(filePath, pressedThickness, libraryPriority);
                            break;
                        case "WUS":
                            parser = new WUS_Reader(filePath, pressedThickness, libraryPriority);
                            break;
                    }

                    if (!parser.Convert()) continue;
                    if (!parser.IsValidFile()) continue;
                    parser.Import();
                    importType = format;
                    fabricator = GetFabricator();
                    break;
                }

                if (!ZPlannerManager.ProjectIsEmpty && !ZPlannerManager.Project.StackupIsEmpty)
                {
                    stackup = ZPlannerManager.Project.Stackup;
                    if (stackup.Title.Length == 0) stackup.Title = Path.GetFileNameWithoutExtension(filePath);
                    ZPlannerManager.SetKeepImportedPressedThickness(ZPlannerManager.IsImported);
                }

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Importing Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was imported from " + filePath + ".");
            }
            catch(Exception exc)
            {
                MessageBox.Show(string.Format("An error occurred while importing the \"{0}\" file. \n" + exc.Message, tbFile.Text));
                return false;
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnoreCommands);
            }

            if (stackup == null)
            {
                MessageBox.Show(string.Format("An error occurred while importing the \"{0}\" file. \n" + "File is not in the expected format.", tbFile.Text));
                return false;
            }

            return true;
        }

        private bool GetResinDkDfEstimate(string materialName, out double Dkr, out double Dfr)
        {
            //defaults
            Dkr = Options.TheOptions.resin_Dk;
            Dfr = Options.TheOptions.resin_Df;

            //derating coefs for RC=50% (caculted for NPG-170D RC=50%, F=3GHz)
            double xDk = 0.8;
            double xDf = 1.5;

            //find material instance = argmin|RC-50%| and get its Dk/Df
            double ddk = 0, ddf = 0, rcDelta = 100;
            string sdk = "", sdf ="";

            IEnumerable<ZMaterial> Q = ZPlannerManager.Dml.Materials.Where(m => m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value == materialName);
            
            foreach (var x in Q)
            {
                string rc = x.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value;
                double drc;
                if (rc.Length > 0)
                {
                    rc = rc.TrimEnd('%');
                    if (Double.TryParse(rc, out drc))
                    {
                        if (Math.Abs(drc - 50) < rcDelta){
                            rcDelta = drc - 50;
                            sdk = x.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk_1GHz).Value;
                            sdf = x.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf_1GHz).Value;
                        }
                    }
                }
            }

            if (sdk.Length > 0 && sdf.Length > 0)
            {
                if (Double.TryParse(sdk, out ddk) && (Double.TryParse(sdf, out ddf)))
                {
                    //make corrections
                    Dkr = xDk * ddk;
                    Dfr = xDf * ddf;
                    return true;
                }
            }

            return false;
        }

        private void FillManufacturerAndMaterialLists()
        {
            materialDictionary.Clear();
            materialDictionary.Add(unknownValue, new List<string>(new string[] { unknownValue }));

            var manufacturerQuery =
            from manufacturerItem in ZPlannerManager.Dml.Materials
            where manufacturerItem.ID != ZStringConstants.EmptyMaterialID
            group manufacturerItem by manufacturerItem.GetMaterialParameterValue(ZStringConstants.DMLParameterIDManufacturer) into manufacturerGroup
            select new 
            { 
                Manufacturer = manufacturerGroup.Key, 
                materialQuery = 
                from materialItem in manufacturerGroup
                where materialItem.ID != ZStringConstants.EmptyMaterialID
                group materialItem by materialItem.GetMaterialParameterValue(ZStringConstants.DMLParameterIDMaterial) into materialGroup
                select new { Material = materialGroup.Key }
            };

            foreach (var manufacturers in manufacturerQuery)
            { 
                List<string> list = new List<string>(new string[] { unknownValue });
                materialDictionary.Add(manufacturers.Manufacturer, list);

                foreach(var materials in manufacturers.materialQuery)
                {
                    list.Add(materials.Material);
                }
            }

            foreach (string manufacturer in materialDictionary.Keys)
            {
                cbManufacturer.Items.Add(manufacturer);
            }
            cbManufacturer.SelectedIndex = 0;
        }

        private bool FindManufacturer()
        {
            tbMaterialName.Text = string.Empty;
            cbManufacturer.SelectedItem = unknownValue;

            materialWasFound = true;
 
            string originalMaterial = string.Empty;
            foreach (ZLayer layer in stackup.Layers)
            {
                ZLayerType? layerType = layer.GetLayerType();
                if (layerType != ZLayerType.Core && layerType != ZLayerType.Prepreg) continue;


                materialWasFound = materialWasFound && layer.IsMaterialAssigned;
                originalMaterial = layer.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial);
                if (!string.IsNullOrWhiteSpace(originalMaterial))
                {
                    tbMaterialName.Text = originalMaterial;
                }
            }

            originalMaterial = originalMaterial.Replace("-", "").Replace("_", "").Replace("(", "").Replace(")", "").Replace(" ", "").ToLower();

            if (originalMaterial.StartsWith("ds")) cbManufacturer.SelectedItem = "Doosan";
            else if (originalMaterial.StartsWith("em")) cbManufacturer.SelectedItem = "EMC";
            else if (originalMaterial.StartsWith("is")) cbManufacturer.SelectedItem = "Isola";
            else if (originalMaterial.StartsWith("it")) cbManufacturer.SelectedItem = "ITEQ";
            else if (originalMaterial.StartsWith("np")) cbManufacturer.SelectedItem = "Nanya";
            else if (originalMaterial.StartsWith("tu")) cbManufacturer.SelectedItem = "TUC";
            else if (originalMaterial.StartsWith("meg")) cbManufacturer.SelectedItem = "Panasonic";

            foreach(string manufacturer in materialDictionary.Keys)
            {
                foreach(string material in materialDictionary[manufacturer])
                {
                    if (originalMaterial == material.Replace("-", "").Replace("_", "").Replace("(", "").Replace(")", "").Replace(" ", "").ToLower())
                    {
                        cbManufacturer.SelectedItem = manufacturer;
                        cbMaterial.SelectedItem = material;
                    }
                }
            }

            if (string.IsNullOrWhiteSpace(originalMaterial)) 
            {
                cbManufacturer.SelectedItem = unknownValue;
            }

            bool isMaterialSelected = IsMaterialSelected();
            materialWasFound = materialWasFound && isMaterialSelected;

            return isMaterialSelected;
        }

        private void SetManufacturer()
        {
            if (!materialWasFound)
            {
                string manufacturer = cbManufacturer.Text;
                string material = cbMaterial.Text;

                if (string.IsNullOrWhiteSpace(material) || material == unknownValue) return;

                foreach (ZLayer layer in stackup.Layers.FindAll(l => l.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial) == tbMaterialName.Text))
                {
                    layer.SetLayerParameterValue(ZStringConstants.ParameterIDMaterial, material);
                    if (!string.IsNullOrWhiteSpace(manufacturer) && manufacturer != unknownValue)
                        layer.SetLayerParameterValue(ZStringConstants.ParameterIDManufacturer, manufacturer);
                }

                materialWasFound = stackup.FinedMaterialForAnyLayer(libraryPriority, fabricator);
            }

            if (materialWasFound && stackup.IsPressedThickness && importType == "WUS")
            {
                stackup.RestoreCopperPercent();
            }
        }

        private void FillResinDkDf()
        {
            foreach(ZLayer layer in stackup.Layers)
            {
                ZLayerType? layerType;
                if (layer.isMetal(out layerType) && (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed))
                {
                    string dk = layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                    if (!string.IsNullOrWhiteSpace(dk)) tbResinDk.Text = dk;

                    string df = layer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);
                    if (!string.IsNullOrWhiteSpace(df)) tbResinDf.Text = df;
                }

                if (!string.IsNullOrWhiteSpace(tbResinDk.Text) && !string.IsNullOrWhiteSpace(tbResinDf.Text)) break;
            }

            double estDk, estDf;
            if (GetResinDkDfEstimate(cbMaterial.Text, out estDk, out estDf))
            {
                tbResinDk.Text = estDk.ToString("N" + Settings.Options.TheOptions.dkDigits, CultureInfo.InvariantCulture);
                tbResinDf.Text = estDf.ToString("N" + Settings.Options.TheOptions.dfDigits, CultureInfo.InvariantCulture);
            }
            else
            {
                if (string.IsNullOrWhiteSpace(tbResinDk.Text)) tbResinDk.Text = estDk.ToString("N" + Settings.Options.TheOptions.dkDigits, CultureInfo.InvariantCulture);
                if (string.IsNullOrWhiteSpace(tbResinDf.Text)) tbResinDf.Text = estDf.ToString("N" + Settings.Options.TheOptions.dfDigits, CultureInfo.InvariantCulture);
            }
        }

        private void SetResinDkDf()
        {
            string formatDk = "N" + Options.TheOptions.dkDigits;
            string formatDf = "N" + Options.TheOptions.dfDigits;

            string dk = tbResinDk.Text;
            string df = tbResinDf.Text;

            if (cbResinDkDf.Checked)
            {
                double dValue;
                if (double.TryParse(dk, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) Options.TheOptions.resin_Dk = dValue;
                if (double.TryParse(df, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) Options.TheOptions.resin_Df = dValue;
            }

            foreach (ZLayer layer in stackup.Layers)
            {
                ZLayerType? layerType;
                if (layer.isMetal(out layerType) && (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed))
                {
                    layer.SetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant, dk);
                    layer.SetLayerParameterValue(ZStringConstants.ParameterIDLossTangent, df);

                    foreach (ZSingle single in stackup.Singles)
                    {
                        ZLayer singleLayer = stackup.GetLayerOfSingleImpedance(layer.ID, single.ID);
                        if (singleLayer != null)
                        {
                            singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant, dk);
                            singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent, df);
                        }
                    }

                    foreach (ZPair pair in stackup.Pairs)
                    {
                        ZLayer pairLayer = stackup.GetLayerOfPairImpedance(layer.ID, pair.ID);
                        if (pairLayer != null)
                        {
                            pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant, dk);
                            pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent, df);
                        }
                    }
                }
            }
        }

        private void CalculateSequentialLamination()
        {
            cbSequentialLamination.Checked = ZPlannerManager.IsSequentialLamination;
            cbSequentialLamination_CheckedChanged(cbSequentialLamination, EventArgs.Empty);

            int xSequentialLamination, nSequentialLamination;
            ZPlannerManager.CalculateSequentialLaminationParams(out xSequentialLamination, out nSequentialLamination);

            lSequentialLamination.Text = string.Format("This stackup uses sequential lamination, with {0} build-up layer(s) added after the first lamination pass involving {1} layers. (A {0}-{1}-{0} stackup.)", xSequentialLamination, nSequentialLamination);
        }

        private void SetSequentialLamination()
        {
            ZPlannerManager.SetSequentialLamination(cbSequentialLamination.Checked);
            stackup.IsSequentialLamination = cbSequentialLamination.Checked;
        }

        private void FindCopperThickness()
        {
            CopperThicknessTextBoxes.Clear();
            foreach (ZLayer layer in stackup.Layers)
            {
                if (!layer.isMetal()) continue;

                CopperThicknessStructure item =
                    new CopperThicknessStructure(
                        layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness),
                        layer.GetLayerParameterValue(ZStringConstants.ParameterIDThickness),
                        layer.GetLayerParameterValue(ZStringConstants.ParameterIDComments));

                if (!CopperThicknessTextBoxes.ContainsKey(item)) CopperThicknessTextBoxes.Add(item, new TextBox[2]);
            }
        }

        private void FillCopperThickness()
        {
            pCopperThickness.Controls.Clear();
            int i = 0;

            foreach (CopperThicknessStructure key in CopperThicknessTextBoxes.Keys.OrderBy(x => x.Weight))
            {
                Label lCopperWeight = new Label();
                lCopperWeight.Text = key.CopperWeight + " oz.:";
                lCopperWeight.Location = new System.Drawing.Point(20, 5 + 30 * i);
                lCopperWeight.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
                lCopperWeight.Name = "lCopperWeight"+i;
                lCopperWeight.Size = new System.Drawing.Size(68, 13);
                lCopperWeight.TabIndex = i * 10 + 11;
                lCopperWeight.TextAlign = System.Drawing.ContentAlignment.MiddleRight;

                pCopperThickness.Controls.Add(lCopperWeight);

                TextBox tbCopperThickness = new TextBox();
                double dValue;
                if (double.TryParse(key.CopperThickness, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) 
                    tbCopperThickness.Text = dValue.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                else 
                    tbCopperThickness.Text = key.CopperThickness;
                tbCopperThickness.Location = new System.Drawing.Point(92, 2 + 30 * i);
                tbCopperThickness.Margin = new System.Windows.Forms.Padding(2);
                tbCopperThickness.Name = "tbCopperThickness"+i;
                tbCopperThickness.Size = new System.Drawing.Size(61, 20);
                tbCopperThickness.TabIndex = i * 10 + 12;
                tbCopperThickness.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;

                pCopperThickness.Controls.Add(tbCopperThickness);

                Label lCopperThicknessUnit = new Label();
                lCopperThicknessUnit.Location = new System.Drawing.Point(157, 5 + 30 * i);
                lCopperThicknessUnit.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
                lCopperThicknessUnit.Name = "lCopperThicknessUnit"+i;
                lCopperThicknessUnit.Size = new System.Drawing.Size(24, 13);
                lCopperThicknessUnit.TabIndex = i * 10 + 13;
                lCopperThicknessUnit.Text = "mils";

                pCopperThickness.Controls.Add(lCopperThicknessUnit);

                TextBox tbCopperThicknessComment = new TextBox();
                tbCopperThicknessComment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
            | System.Windows.Forms.AnchorStyles.Right)));
                tbCopperThicknessComment.Location = new System.Drawing.Point(215, 2 + 30 * i);
                tbCopperThicknessComment.Margin = new System.Windows.Forms.Padding(2);
                tbCopperThicknessComment.Name = "tbCopperThicknessComment"+i;
                tbCopperThicknessComment.Size = new System.Drawing.Size(213, 20);
                tbCopperThicknessComment.TabIndex = i * 10 + 14;
                tbCopperThicknessComment.Text = key.Comment;

                pCopperThickness.Controls.Add(tbCopperThicknessComment);
                
                CopperThicknessTextBoxes[key][0] = tbCopperThickness;
                CopperThicknessTextBoxes[key][1] = tbCopperThicknessComment;

                i++;
            }
        }

        private void SetCopperThickness()
        {
            if (cbGetDefaultCopperThickness.Checked)
            {
                foreach (CopperThicknessStructure key in CopperThicknessTextBoxes.Keys)
                {
                    double dWeight;
                    double dThickness;
                    if (ZPlannerManager.TryToParseCopperWeight(key.CopperWeight, out dWeight) && double.TryParse(CopperThicknessTextBoxes[key][0].Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dThickness))
                    {
                        if(dWeight == 0.5) 
                            Options.TheOptions.copper_thickness_05oz = dThickness;
                        else if (dWeight == 1.0) 
                            Options.TheOptions.copper_thickness_10oz = dThickness;
                        else if (dWeight == 1.5) 
                            Options.TheOptions.copper_thickness_15oz = dThickness;
                        else if (dWeight == 2.0) 
                            Options.TheOptions.copper_thickness_20oz = dThickness;
                        else if (dWeight == 2.5) 
                            Options.TheOptions.copper_thickness_25oz = dThickness;
                    }
                }
            }

            foreach (CopperThicknessStructure key in CopperThicknessTextBoxes.Keys)
            {
                double dValue;
                string sValue = CopperThicknessTextBoxes[key][0].Text;
                string sOriginValue = key.CopperThickness;
                string comment = CopperThicknessTextBoxes[key][1].Text;

                if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) sValue = dValue.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                if (double.TryParse(sOriginValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) sOriginValue = dValue.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);

                if ((key.CopperThickness == CopperThicknessTextBoxes[key][0].Text || sOriginValue == sValue) && key.Comment == comment) continue;

                foreach(ZLayer layer in stackup.Layers.FindAll(l => (l.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness) == key.CopperWeight && l.GetLayerParameterValue(ZStringConstants.ParameterIDThickness) == key.CopperThickness && l.GetLayerParameterValue(ZStringConstants.ParameterIDComments) == key.Comment)))
                {
                    if (key.CopperThickness != CopperThicknessTextBoxes[key][0].Text && sOriginValue != sValue)
                        layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, CopperThicknessTextBoxes[key][0].Text);
                    if (key.Comment != comment)
                        layer.SetLayerParameterValue(ZStringConstants.ParameterIDComments, comment);
                }                
            }
        }

        private void FillPressedThickness()
        {
            cbShowPressedThickness.Checked = ZPlannerManager.IsPressedThickness;
            cbKeepImportedPressedThickness.Checked = (ZPlannerManager.IsPressedThickness && ZPlannerManager.IsKeepImportedPressedThickness  && ZPlannerManager.IsImported);

            string format = "N" + Options.TheOptions.percentDigits;

            tbManualForSignal.Tag = string.Empty;
            tbManualForMixed.Tag = string.Empty;
            tbManualForPlane.Tag = string.Empty;

            foreach (ZLayer layer in stackup.Layers)
            {
                switch(layer.GetLayerType())
                {
                    case ZLayerType.Signal:
                        tbManualForSignal.Tag = layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperPercent);
                        break;
                    case ZLayerType.SplitMixed:
                        tbManualForMixed.Tag = layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperPercent);
                        break;
                    case ZLayerType.Plane:
                        tbManualForPlane.Tag = layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperPercent);
                        break;
                }
            }

            if (string.IsNullOrWhiteSpace((string)tbManualForSignal.Tag)) tbManualForSignal.Tag = stackup.ForSignal.ToString(format, CultureInfo.InvariantCulture);
            if (string.IsNullOrWhiteSpace((string)tbManualForMixed.Tag)) tbManualForMixed.Tag = stackup.ForMixed.ToString(format, CultureInfo.InvariantCulture);
            if (string.IsNullOrWhiteSpace((string)tbManualForPlane.Tag)) tbManualForPlane.Tag = stackup.ForPlane.ToString(format, CultureInfo.InvariantCulture);

            double dValue;
            if (double.TryParse((string)tbManualForSignal.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                tbManualForSignal.Text = dValue.ToString(format, CultureInfo.InvariantCulture);
            if (double.TryParse((string)tbManualForMixed.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                tbManualForMixed.Text = dValue.ToString(format, CultureInfo.InvariantCulture);
            if (double.TryParse((string)tbManualForPlane.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                tbManualForPlane.Text = dValue.ToString(format, CultureInfo.InvariantCulture);
        }

        private void SetPressedThickness()
        {
            ZPlannerManager.SetPressedThickness(cbShowPressedThickness.Checked);
            stackup.IsPressedThickness = cbShowPressedThickness.Checked;
            ZPlannerManager.SetKeepImportedPressedThickness(cbKeepImportedPressedThickness.Checked && ZPlannerManager.IsImported);

            double dValue;

            if (double.TryParse((string)tbManualForSignal.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.ForSignal = dValue;
                if (cbDefaultPressedThickness.Checked) Options.TheOptions.forSignal = dValue;
            }

            if (double.TryParse((string)tbManualForMixed.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.ForMixed = dValue;
                if (cbDefaultPressedThickness.Checked) Options.TheOptions.forMixed = dValue;
            }

            if (double.TryParse((string)tbManualForPlane.Tag, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.ForPlane = dValue;
                if (cbDefaultPressedThickness.Checked) Options.TheOptions.forPlane = dValue;
            }
        }

        /*private void FillEtch()
        {
            cbShowEtchEffects.Checked = ZPlannerManager.IsTrapezoidalTraces;

            string format = "N" + Options.TheOptions.lengthDigits;

            tbEtch.Text = stackup.Etch.ToString(format, CultureInfo.InvariantCulture);
        }

        private void SetEtch()
        {
            ZPlannerManager.SetTrapezoidalTraces(cbShowEtchEffects.Checked);
            stackup.IsTrapezoidalTraces = cbShowEtchEffects.Checked;

            double dValue;
            if (cbDefaultEtchEffects.Checked)
            {
                stackup.Etch = Options.TheOptions.etch;
            }
            else
            {
                if (double.TryParse(tbEtch.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue)) stackup.Etch = dValue;
            }
        }*/

        private void FillImpedances()
        {
            string format = "N0"; // "N" + Options.TheOptions.impedanceDigits;

            int i = 0;

            pSingleEnded.Controls.Clear();
            SingleTextBoxes = new Dictionary<string, TextBox[]>();

            foreach (ZSingle single in stackup.Singles)
            {
                if (!single.IsUsed()) break;

                TextBox tbSingleName = new TextBox();
                tbSingleName.Text = single.Title;
                tbSingleName.Location = new System.Drawing.Point(123, 3 + 30 * i);
                tbSingleName.Name = "tbSingleName" + i;
                tbSingleName.Size = new System.Drawing.Size(204, 20);
                tbSingleName.TabIndex = 0;

                Label lSingleImpedance = new Label();
                lSingleImpedance.AutoSize = true;
                lSingleImpedance.Location = new System.Drawing.Point(74, 6 + 30 * i);
                lSingleImpedance.Name = "lSingleImpedance" + i;
                lSingleImpedance.Size = new System.Drawing.Size(32, 13);
                lSingleImpedance.TabIndex = 0;
                lSingleImpedance.Text = "ohms";

                TextBox tbSingleImpedance = new TextBox();
                tbSingleImpedance.Text = single.ImpedanceTarget.ToString(format, CultureInfo.InvariantCulture);
                tbSingleImpedance.Location = new System.Drawing.Point(11, 3 + 30 * i);
                tbSingleImpedance.Name = "tbSingleImpedance" + i;
                tbSingleImpedance.Size = new System.Drawing.Size(61, 20);
                tbSingleImpedance.TabIndex = 0;
                tbSingleImpedance.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;

                pSingleEnded.Controls.Add(tbSingleName);
                pSingleEnded.Controls.Add(lSingleImpedance);
                pSingleEnded.Controls.Add(tbSingleImpedance);

                SingleTextBoxes.Add(single.ID, new TextBox[] { tbSingleName, tbSingleImpedance });

                i++;
            }

            i = 0;

            pDifferential.Controls.Clear();
            PairTextBoxes = new Dictionary<string, TextBox[]>();

            foreach(ZPair pair in stackup.Pairs)
            {
                if (!pair.IsUsed()) break;

                TextBox tbDiffName = new TextBox();
                tbDiffName.Text = pair.Title;
                tbDiffName.Location = new System.Drawing.Point(123, 3 + 30 * i);
                tbDiffName.Name = "tbDiffName" + i;
                tbDiffName.Size = new System.Drawing.Size(204, 20);
                tbDiffName.TabIndex = 0;

                Label lDiffImpedance = new Label();
                lDiffImpedance.AutoSize = true;
                lDiffImpedance.Location = new System.Drawing.Point(74, 6 + 30 * i);
                lDiffImpedance.Name = "lDiffImpedance" + i;
                lDiffImpedance.Size = new System.Drawing.Size(32, 13);
                lDiffImpedance.TabIndex = 0;
                lDiffImpedance.Text = "ohms";

                TextBox tbDiffImpedance = new TextBox();
                tbDiffImpedance.Text = pair.ImpedanceTarget.ToString(format, CultureInfo.InvariantCulture);
                tbDiffImpedance.Location = new System.Drawing.Point(11, 3 + 30 * i);
                tbDiffImpedance.Name = "tbDiffImpedance" + i;
                tbDiffImpedance.Size = new System.Drawing.Size(61, 20);
                tbDiffImpedance.TabIndex = 0;
                tbDiffImpedance.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;

                this.pDifferential.Controls.Add(tbDiffName);
                this.pDifferential.Controls.Add(tbDiffImpedance);
                this.pDifferential.Controls.Add(lDiffImpedance);

                PairTextBoxes.Add(pair.ID, new TextBox[] { tbDiffName, tbDiffImpedance });

                i++;
            }
        }

        private void SetImpedances()
        {
            foreach (ZSingle single in stackup.Singles)
            {
                TextBox[] textboxes;
                if (SingleTextBoxes.TryGetValue(single.ID, out  textboxes))
                {
                    if (single.Title != textboxes[0].Text) single.Title = textboxes[0].Text;

                    double dValue;
                    if (double.TryParse(textboxes[1].Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue) && single.ImpedanceTarget != dValue) single.ImpedanceTarget = dValue;
                }
            }

            foreach (ZPair pair in stackup.Pairs)
            {
                TextBox[] textboxes;
                if (PairTextBoxes.TryGetValue(pair.ID, out  textboxes))
                {
                    if (pair.Title != textboxes[0].Text) pair.Title = textboxes[0].Text;

                    double dValue;
                    if (double.TryParse(textboxes[1].Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue) && pair.ImpedanceTarget != dValue) pair.ImpedanceTarget = dValue;
                }
            }
        }

        private void FillRoughness()
        {
            string format = "N" + Options.TheOptions.lengthDigits;

            cbCopperRoughness.Checked = ZPlannerManager.IsRoughness;

            tbCoreSideRoughnessHTE.Text = stackup.CoreSideRoughnessHTE.ToString(format, CultureInfo.InvariantCulture);
            tbCoreSideRoughnessRTF.Text = stackup.CoreSideRoughnessRTF.ToString(format, CultureInfo.InvariantCulture);
            tbCoreSideRoughnessVLP.Text = stackup.CoreSideRoughnessVLP.ToString(format, CultureInfo.InvariantCulture);
            tbCoreSideRoughnessVLP2.Text = stackup.CoreSideRoughnessVLP2.ToString(format, CultureInfo.InvariantCulture);
            tbCoreSideRoughnessHVLP.Text = stackup.CoreSideRoughnessHVLP.ToString(format, CultureInfo.InvariantCulture);

            tbPrepregSideRoughnessMB100LE.Text = stackup.PrepregSideRoughnessMB100LE.ToString(format, CultureInfo.InvariantCulture);
            tbPrepregSideRoughnessAlphaPrep.Text = stackup.PrepregSideRoughnessAlphaPrep.ToString(format, CultureInfo.InvariantCulture);
            tbPrepregSideRoughnessMB100ZK.Text = stackup.PrepregSideRoughnessMB100ZK.ToString(format, CultureInfo.InvariantCulture);
            tbPrepregSideRoughnessBF.Text = stackup.PrepregSideRoughnessBF.ToString(format, CultureInfo.InvariantCulture);
            tbPrepregSideRoughnessCZ8100.Text = stackup.PrepregSideRoughnessCZ8100.ToString(format, CultureInfo.InvariantCulture);
            tbPrepregSideRoughnessMB100HP.Text = stackup.PrepregSideRoughnessMB100HP.ToString(format, CultureInfo.InvariantCulture);

            cbCoreSideRoughnessHTE.Checked = false;
            cbCoreSideRoughnessRTF.Checked = false;
            cbCoreSideRoughnessVLP.Checked = false;
            cbCoreSideRoughnessVLP2.Checked = false;
            cbCoreSideRoughnessHVLP.Checked = false;

            foreach (ZLayer layer in stackup.Layers)
            {
                string sValue = layer.GetLayerParameterValue(ZStringConstants.ParameterIDFoilTreatment);
                if (string.IsNullOrWhiteSpace(sValue)) continue;

                if (sValue.Contains("HTE"))
                {
                    cbCoreSideRoughnessHTE.Checked = true;
                }
                else if (sValue.Contains("RTF"))
                {
                    cbCoreSideRoughnessRTF.Checked = true;
                }
                else if (sValue.Contains("VLP"))
                {
                    if (sValue.Contains("VLP-2") || sValue.Contains("VLP 2") || sValue.Contains("VLP2"))
                    {
                        cbCoreSideRoughnessVLP2.Checked = true;
                    }
                    else if (sValue.Contains("HVLP"))
                    {
                        cbCoreSideRoughnessHVLP.Checked = true;
                    }
                    else
                    {
                        cbCoreSideRoughnessVLP.Checked = true;
                    }
                }
            }

            tbCoreSideRoughnessHTE.Enabled = cbCoreSideRoughnessHTE.Checked;
            tbCoreSideRoughnessRTF.Enabled = cbCoreSideRoughnessRTF.Checked;
            tbCoreSideRoughnessVLP.Enabled = cbCoreSideRoughnessVLP.Checked;
            tbCoreSideRoughnessVLP2.Enabled = cbCoreSideRoughnessVLP2.Checked;
            tbCoreSideRoughnessHVLP.Enabled = cbCoreSideRoughnessHVLP.Checked;

            cbPrepregSideRoughnessMB100HP.Checked = false;
            cbPrepregSideRoughnessCZ8100.Checked = false;
            cbPrepregSideRoughnessBF.Checked = (fabricator == "ISU" || fabricator == "WUS" || importType == "Tapestry");
            cbPrepregSideRoughnessMB100ZK.Checked = false;
            cbPrepregSideRoughnessAlphaPrep.Checked = false;
            cbPrepregSideRoughnessMB100LE.Checked = false;
        }

        private void SetRoughness()
        {
            ZPlannerManager.SetCopperRoughness(cbCopperRoughness.Checked);
            stackup.IsRoughness = cbCopperRoughness.Checked;
            
            double dValue;
            if (cbCoreSideRoughnessHTE.Checked && double.TryParse(tbCoreSideRoughnessHTE.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.CoreSideRoughnessHTE = dValue;
                if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.core_side_roughness_hte = dValue;
            }

            if (cbCoreSideRoughnessRTF.Checked && double.TryParse(tbCoreSideRoughnessRTF.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.CoreSideRoughnessRTF = dValue;
                if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.core_side_roughness_rtf = dValue;
            }

            if (cbCoreSideRoughnessVLP.Checked && double.TryParse(tbCoreSideRoughnessVLP.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.CoreSideRoughnessVLP = dValue;
                if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.core_side_roughness_vlp = dValue;
            }

            if (cbCoreSideRoughnessVLP2.Checked && double.TryParse(tbCoreSideRoughnessVLP2.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.CoreSideRoughnessVLP2 = dValue;
                if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.core_side_roughness_vlp2 = dValue;
            }

            if (cbCoreSideRoughnessHVLP.Checked && double.TryParse(tbCoreSideRoughnessHVLP.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                stackup.CoreSideRoughnessHVLP = dValue;
                if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.core_side_roughness_hvlp = dValue;
            }

            stackup.OxideAlternativeTreatments = null;

            if (cbPrepregSideRoughnessMB100LE.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.MacDermidMultiBond100LE;
                if (double.TryParse(tbPrepregSideRoughnessMB100LE.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessMB100LE = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_mb100le = dValue;
                }
            }

            if (cbPrepregSideRoughnessAlphaPrep.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.MacDermidEnthoneAlphaPrep;
                if (double.TryParse(tbPrepregSideRoughnessAlphaPrep.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessAlphaPrep = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_alphaprep = dValue;
                }
            }

            if (cbPrepregSideRoughnessMB100ZK.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.MacDermidMultiBond100ZK;
                if (double.TryParse(tbPrepregSideRoughnessMB100ZK.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessMB100ZK = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_mb100zk = dValue;
                }
            }

            if (cbPrepregSideRoughnessBF.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.AtotechBondFilm;
                if (double.TryParse(tbPrepregSideRoughnessBF.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessBF = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_bf = dValue;
                }
            }

            if (cbPrepregSideRoughnessCZ8100.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.MECetchBONDCZ8100;
                if (double.TryParse(tbPrepregSideRoughnessCZ8100.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessCZ8100 = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_cz8100 = dValue;
                }
            }

            if (cbPrepregSideRoughnessMB100HP.Checked)
            {
                stackup.OxideAlternativeTreatments = OxideAlternativeTreatmentsType.MacDermidMultiBond100HP;
                if (double.TryParse(tbPrepregSideRoughnessMB100HP.Text, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                {
                    stackup.PrepregSideRoughnessMB100HP = dValue;
                    if (cbGetDefaultCopperRoughness.Checked) Options.TheOptions.prepreg_side_roughness_mb100hp = dValue;
                }
            }
        }

        private string GetFabricator()
        {
            if (ExportOptions.TheOptions.Fabricator == null) return importType;
            if(ExportOptions.TheOptions.Fabricator.Contains("ISU")) return "ISU";
            if(ExportOptions.TheOptions.Fabricator.Contains("WUS")) return "WUS";
            if(ExportOptions.TheOptions.Fabricator.Contains("TTM")) return "TTM";
            if(ExportOptions.TheOptions.Fabricator.Contains("IPC")) return "IPC";
            if(ExportOptions.TheOptions.Fabricator.Contains("HL")) return "HL";
            if(ExportOptions.TheOptions.Fabricator.Contains("ODBPP")) return "ODBPP";

            return ExportOptions.TheOptions.Fabricator;
        }

        private ZLibraryCategory[] GetLibraryPriority()
        {
            ZLibraryCategory[] array = new ZLibraryCategory[clbLibraryOrder.CheckedItems.Count];
            int i = 0;
            foreach (string item in clbLibraryOrder.CheckedItems)
            {
                array[i] = GetLibraryCategoryByString(item);
                ++i;
            }

            if (array.Length == 0) return new ZLibraryCategory[] { ZLibraryCategory.ZZero };

            return array;
        }

        private ZLibraryCategory GetLibraryCategoryByString(string value)
        {
            switch(value)
            {
                case "Z-zero Library":
                    return ZLibraryCategory.ZZero;
                case "Corporate Library":
                    return ZLibraryCategory.Corporate;
                case "Local Library":
                    return ZLibraryCategory.Local;
                default:
                    return ZLibraryCategory.ZZero;
            }
        }

        private bool IsMaterialSelected()
        {
            return (!(cbManufacturer.Text == unknownValue) && !(cbMaterial.Text == unknownValue));
        }

        private void CancelImport()
        {
            if (ZPlannerManager.CloseProject()) Close();
        }

        private void FinishImport()
        {
            ZPlannerManager.MainMenu.UpdateSingles(stackup.Singles);
            ZPlannerManager.MainMenu.UpdatePairs(stackup.Pairs);

            bool isUsedSingle = false;
            foreach (ZSingle single in stackup.Singles)
            {
                if (single.IsUsed())
                {
                    isUsedSingle = true;
                    break;
                }
            }
            ZPlannerManager.ShowSingles(isUsedSingle);

            bool isUsedPair = false;
            foreach (ZPair pair in stackup.Pairs)
            {
                if (pair.IsUsed())
                {
                    isUsedPair = true;
                    break;
                }
            }
            ZPlannerManager.ShowPairs(isUsedPair);

            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.StackupPanel.SetReadOnlyCells();
                ZPlannerManager.StackupPanel.FormatGridView();
                ZPlannerManager.StackupPanel.Update();
            }
            //ZPlannerManager.UpdateActiveStackup();

            if (stackup.IsMirrored())
            {
                //if (ZPlannerManager.ConfirmYesNo("Auto-Mirroring", "Would you like to turn on Auto-Mirroring for future stackup edits?  (Forces top > bottom symmetry, and turns on the stackup center line.)"))
                //{
                    //ZPlannerManager.IsAutoMirror = true;
                    ZPlannerManager.IsCenterLineVisible = true;
                //}
            }

            isWizardFinished = true;

            string newFilePath = string.Empty;
            try
            {
                ZPlannerManager.Project.IsProtected = true;
                newFilePath = Path.Combine(Path.GetDirectoryName(tbFile.Text), Path.GetFileNameWithoutExtension(tbFile.Text) + ".z0p");
                if (File.Exists(newFilePath))
                {
                    File.SetAttributes(newFilePath, File.GetAttributes(newFilePath) & ~FileAttributes.ReadOnly);
                    File.Delete(newFilePath);
                }
                ZPlannerManager.SaveProjectSilent(newFilePath);
                File.SetAttributes(newFilePath, File.GetAttributes(newFilePath) | FileAttributes.ReadOnly);
            }
            catch
            {
                ZPlannerManager.MessagePanel.AddMessage("An error occurred while saving the imported stackup as " + newFilePath);
            }

            Close();
        }

        private void ImportWizard_Load(object sender, EventArgs e)
        {
            isWizardFinished = false;
        }

        private void bNext_Click(object sender, EventArgs e)
        {
            ChangeState(curIndex + 1, curIndex);
        }

        private void bBack_Click(object sender, EventArgs e)
        {
            ChangeState(curIndex - 1, curIndex);
        }

        private void bCancel_Click(object sender, EventArgs e)
        {
            CancelImport();
        }

        private void bFile_Click(object sender, EventArgs e)
        {
            FileFolderDialog openFileDialog = new FileFolderDialog();

            openFileDialog.Filter = "All Files|*.FFS;*.STK;*.XML;*.TGZ;*.XLS;*.XLSX;*.XLSM|Schematic Files (*.FFS)|*.FFS|Stackup Files (*.STK)|*.STK|IPC-2581 Files (*.XML)|*.XML|ODB++ Files (*.TGZ)|*.TGZ|Excel Files (*.XLS;*.XLSX;*.XLSM)|*.XLS;*.XLSX;*.XLSM";
            openFileDialog.FilterIndex = 1;
            //openFileDialog.InitialDirectory = Options.TheOptions.ExportPath;
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.SelectedPath != string.Empty)
            {
                tbFile.Text = openFileDialog.SelectedPath;
            }

            bNext.Enabled = File.Exists(tbFile.Text) || Directory.Exists(tbFile.Text);
        }

        private void rbLibrary_CheckedChanged(object sender, EventArgs e)
        {
        }

        private void tbMaterialName_TextChanged(object sender, EventArgs e)
        {
            materialWasFound = false;
            bNext.Enabled = IsMaterialSelected();
        }

        private void cbManufacturer_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbMaterial.Items.Clear();

            List<string> list;
            if (materialDictionary.TryGetValue(cbManufacturer.SelectedItem as string, out list) || materialDictionary.TryGetValue(unknownValue, out list))
            {
                cbMaterial.Items.AddRange(list.ToArray());
                cbMaterial.SelectedIndex = 0;
            }
        }

        private void ImportWizard_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!isWizardFinished)  CancelImport();
        }

        private class CopperThicknessStructure
        {
            public CopperThicknessStructure(string copperWeight, string copperThickness, string comment)
            {
                CopperWeight = copperWeight; 
                CopperThickness = copperThickness;
                Comment = comment;

                double dValue;
                if (ZPlannerManager.TryToParseCopperWeight(copperWeight, out dValue)) Weight = dValue;
            }

            public double Weight = double.NaN;

            public string CopperWeight = string.Empty;

            public string CopperThickness = string.Empty;

            public string Comment = string.Empty;

            public override bool Equals(object obj)
            {
                var item = obj as CopperThicknessStructure;

                if (item == null)
                {
                    return false;
                }

                return (this.CopperWeight == item.CopperWeight && this.CopperThickness == item.CopperThickness && this.Comment == item.Comment);
            }

            public override int GetHashCode()
            {
                return string.Format("{0}#{1}#{2}", CopperWeight, CopperThickness, Comment).GetHashCode();
            }
        }

        private void cbPrepregSideRoughness_CheckedChanged(object sender, EventArgs e)
        {
            CheckBox checkbox = sender as CheckBox;
            if (checkbox == null) return;

            if (checkbox.Checked)
            {
                if (cbPrepregSideRoughnessMB100HP != checkbox) cbPrepregSideRoughnessMB100HP.Checked = false;
                if (cbPrepregSideRoughnessCZ8100 != checkbox) cbPrepregSideRoughnessCZ8100.Checked = false;
                if (cbPrepregSideRoughnessBF != checkbox) cbPrepregSideRoughnessBF.Checked = false;
                if (cbPrepregSideRoughnessMB100ZK != checkbox) cbPrepregSideRoughnessMB100ZK.Checked = false;
                if (cbPrepregSideRoughnessAlphaPrep != checkbox) cbPrepregSideRoughnessAlphaPrep.Checked = false;
                if (cbPrepregSideRoughnessMB100LE != checkbox) cbPrepregSideRoughnessMB100LE.Checked = false;
            }
        }

        private void bLibraryOrderUp_Click(object sender, EventArgs e)
        {
            object item = clbLibraryOrder.SelectedItem;

            if (item != null)
            {
                int index = clbLibraryOrder.Items.IndexOf(item);
                if (index <= 0) return;
                bool itemChecked = clbLibraryOrder.GetItemChecked(index);
                clbLibraryOrder.Items.Remove(item);
                clbLibraryOrder.Items.Insert(index - 1, item);
                clbLibraryOrder.SetItemChecked(index - 1, itemChecked);
                clbLibraryOrder.SelectedIndex = index - 1;
            }
        }

        private void bLibraryOrderDown_Click(object sender, EventArgs e)
        {
            object item = clbLibraryOrder.SelectedItem;

            if (item != null)
            {
                int index = clbLibraryOrder.Items.IndexOf(item);
                if (index >= 2 || index < 0) return;
                bool itemChecked = clbLibraryOrder.GetItemChecked(index);
                clbLibraryOrder.Items.Remove(item);
                clbLibraryOrder.Items.Insert(index + 1, item);
                clbLibraryOrder.SetItemChecked(index + 1, itemChecked);
                clbLibraryOrder.SelectedIndex = index + 1;
            }
        }

        private void cbSequentialLamination_CheckedChanged(object sender, EventArgs e)
        {
            lSequentialLamination.Enabled = cbSequentialLamination.Checked;
        }

        private void tbManualForCopper_TextChanged(object sender, EventArgs e)
        {
            TextBox tb = (TextBox)sender;
            tb.Tag = tb.Text;
        }
    }
}
