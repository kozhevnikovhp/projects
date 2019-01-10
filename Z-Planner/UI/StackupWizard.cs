using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Wizard;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Commands;

namespace ZZero.ZPlanner.UI
{
    /*
        //================== Metal Defaults
        [DataMember(Order = 18)]
        public double trace_width = 5.0; //mils [2-100]
        [DataMember(Order = 19)]
        public double trace_spacing = 10; //mils [2-100]
        [DataMember(Order = 20)]
        public double etch = 0.9; //[0.5-1] //narrow_top = true;

        //outer metal layers
        [DataMember(Order = 21)]
        public double plating_thickness = 1; //oz [0-20]
        [DataMember(Order = 22)]
        public double base_trace_thickness = 0.5; //oz [0-20]
        [DataMember(Order = 23)]
        public double roughness = 5; //um [0.5-15]

        //inner metal layers
        [DataMember(Order = 24)]
        public double copper_foil_thickness = 0.5;//oz [0-20]
        [DataMember(Order = 25)]
        public double laminate_side_roughness = 5; //um [0.5-15]
        [DataMember(Order = 26)]
        public double prepreg_side_roughness = 3.5; //um [0.5-15]

            [DataMember(Order = 72)]
            public double outer_trace_width = 8.0; //mils [2-100]
            [DataMember(Order = 73)]
            public double inner_trace_width = 4.5; //mils [2-100]
            [DataMember(Order = 74)]
            public double outer_diff_trace_width = 5.5; //mils [2-100]
            [DataMember(Order = 75)]
            public double inner_diff_trace_width = 4.0; //mils [2-100]
            [DataMember(Order = 74)]
            public double outer_diff_trace_spacing = 10.0; //mils [2-100]
            [DataMember(Order = 75)]
            public double inner_diff_trace_spacing = 10.0; //mils [2-100]
    */
    public partial class StackupWizardDialog : Form
    {
        public WizardData theWizard;

        public static StackupWizardDialog CreateStackupWizardDialog()
        {
            if (!ZPlannerManager.CloseProject()) return null;
            ZPlannerManager.MessagePanel.ClearMessages();
            return new StackupWizardDialog();
        }

        private StackupWizardDialog()
        {
            InitializeComponent();
            theWizard = new WizardData();

            cbNumberOfLayers.Text = "4";
            cbSeqLam.Text = "0";
            cbSignalLayerWeight.Text = "0.5";
            cbSplitMixedLayerWeight.Text = "0.5";
            cbPlatingWeight.Text = "0.5";

            string wFormat = "N" + Options.TheOptions.lengthDigits;
            tbOuterMetalWidth.Text = Options.TheOptions.outer_trace_width.ToString(wFormat);
            tbOuterMetalDiffWidth.Text = Options.TheOptions.outer_diff_trace_width.ToString(wFormat);
            tbOuterMetalDiffSpacing.Text = Options.TheOptions.outer_diff_trace_spacing.ToString(wFormat);
            tbInnerMetalWidth.Text = Options.TheOptions.inner_trace_width.ToString(wFormat);
            tbInnerMetalDiffWidth.Text = Options.TheOptions.inner_diff_trace_width.ToString(wFormat);
            tbInnerMetalDiffSpacing.Text = Options.TheOptions.inner_diff_trace_spacing.ToString(wFormat);


            rbGeneric.Checked = true;
            cbPrepregOnOuterLayer.Checked = true;
            cbSolderMask.Checked = true;
            rbHeightFromBoardThickness.Checked = true;

            tbTargetDk.Text = Options.TheOptions.Dk.ToString();
            tbTargetDf.Text = Options.TheOptions.Df.ToString();

            rbTempAll.Checked = true;
            rbDkAll.Checked = true;
            cbUpperDf.Text = "0.020 - Standard Loss";
            cbLowerDf.Text = "0";

            tbDielectricHeight.Text = Options.TheOptions.height.ToString();

            cbPlies.Text = "1";
            cbParsPlies.Text = "1";

            SelectManufacturers();
            this.ActiveControl = btnNext;

#if DEBUG
#else
            //get rid of tab headers
            tabControlWizard.Appearance = TabAppearance.FlatButtons;
            tabControlWizard.ItemSize = new Size(0, 1);
            tabControlWizard.SizeMode = TabSizeMode.Fixed;

            foreach (TabPage tab in tabControlWizard.TabPages)
            {
                tab.Text = "";
            }
#endif
        }

        void SaveData()
        {
            // pageMetals
            theWizard.numberSeqLam = Convert.ToInt32(cbSeqLam.SelectedItem);
            theWizard.numberOfLayers = Convert.ToInt32(cbNumberOfLayers.SelectedItem) + 2*theWizard.numberSeqLam;
            theWizard.numberSeqLam = Convert.ToInt32(cbSeqLam.SelectedItem);
            theWizard.targetBoardWidth = tbTargetBoardThickness.Text == String.Empty ? 0 : Convert.ToDouble(tbTargetBoardThickness.Text);

            theWizard.signals.Clear();
            theWizard.splitMixed.Clear();
            theWizard.planes.Clear();
            for (int i = 0; i < theWizard.numberOfLayers; i++)
            {
                switch (gridMetalLayers.Rows[i].Cells[1].Value as string)
                {
                    case "Signal":
                        theWizard.signals.Add(i + 1);
                        break;
                    case "Mixed":
                        theWizard.splitMixed.Add(i + 1);
                        break;
                    case "Plane":
                        theWizard.planes.Add(i + 1);
                        break;
                }
            }
            switch ((string)cbSignalLayerWeight.SelectedItem)
            {
                case "1/2": theWizard.signalLayerWeight = 0.5;
                    break;
                default:
                    theWizard.signalLayerWeight = Convert.ToDouble(cbSignalLayerWeight.SelectedItem);
                    break;
            }
            switch ((string)cbSplitMixedLayerWeight.SelectedItem)
            {
                case "1/2": theWizard.splitMixedLayerWeight = 0.5;
                    break;
                default:
                    theWizard.splitMixedLayerWeight = Convert.ToDouble(cbSplitMixedLayerWeight.SelectedItem);
                    break;
            }
            switch ((string)cbPlatingWeight.SelectedItem)
            {
                case "1/2": theWizard.platingWeight = 0.5;
                    break;
                default:
                    theWizard.platingWeight = Convert.ToDouble(cbPlatingWeight.SelectedItem);
                    break;
            }

            theWizard.outerMetalWidth = Convert.ToDouble(tbOuterMetalWidth.Text);
            theWizard.outerMetalDiffWidth = Convert.ToDouble(tbOuterMetalDiffWidth.Text);
            theWizard.outerMetalDiffSpacing = Convert.ToDouble(tbOuterMetalDiffSpacing.Text);
            theWizard.innerMetalWidth = Convert.ToDouble(tbInnerMetalWidth.Text);
            theWizard.innerMetalDiffWidth = Convert.ToDouble(tbInnerMetalDiffWidth.Text);
            theWizard.innerMetalDiffSpacing = Convert.ToDouble(tbInnerMetalDiffSpacing.Text);


            // pageDielectrics
            if (rbGeneric.Checked)
            {
                theWizard.selectDielectric = WizardData.SelectDielectric.Generic;
            }
            else if (rbByManufacturer.Checked)
            {
                theWizard.selectDielectric = WizardData.SelectDielectric.byManufacturer;
            }
            else
            {
                theWizard.selectDielectric = WizardData.SelectDielectric.byParameter;
            }
            theWizard.prepregOnOuterLayer = cbPrepregOnOuterLayer.Checked;
            theWizard.solderMask = cbSolderMask.Checked;
            theWizard.solserMaskThickness = 0.5;

            // pageGenericDielectric
            theWizard.targetDk = tbTargetDk.Text == String.Empty ? 0 : Convert.ToDouble(tbTargetDk.Text);
            theWizard.targetDf = tbTargetDf.Text == String.Empty ? 0 : Convert.ToDouble(tbTargetDf.Text);
            theWizard.specifyDielectricHeight = rbSpecifyDielectricHeight.Checked;
            theWizard.dielectricHeight = tbDielectricHeight.Text == String.Empty ? 0 : Convert.ToDouble(tbDielectricHeight.Text);

            //pageManufacturerDielectric            
            theWizard.manufacturerList.Clear();
            foreach (object x in cbManufacturer.Items)
            {
                theWizard.manufacturerList.Add(x as string);
            }

            theWizard.dmlList.Clear();
            foreach (object x in cbDMLList.Items)
            {
                theWizard.dmlList.Add(x as string);
            }
            if (cbManufacturer.SelectedItem != null)
            {
                theWizard.selManufacturer = cbManufacturer.SelectedItem as string;
            }
            if (cbDMLList.SelectedItem != null)
            {
                theWizard.selMaterialId = (cbDMLList.SelectedItem as MaterialData).ID;
            }
            if (cbCores.SelectedItem != null){
                theWizard.selCoreId = (cbCores.SelectedItem as MaterialData).ID;
            }
            if (cbPrepregs.SelectedItem != null){
                theWizard.selPrepregId = (cbPrepregs.SelectedItem as MaterialData).ID;
                theWizard.nPlies = cbPlies.Text;
            }


            //pageMaterial
            theWizard.glasTransitionTemp = WizardData.GlassTransitionTemp.All;
            if (rbTemp200.Checked)
            {
                theWizard.glasTransitionTemp = WizardData.GlassTransitionTemp.temp200;
            }
            else if (rbTemp170.Checked)
            {
                theWizard.glasTransitionTemp = WizardData.GlassTransitionTemp.temp170;
            }
            else if (rbTemp150.Checked)
            {
                theWizard.glasTransitionTemp = WizardData.GlassTransitionTemp.temp150;
            }
            else if (rbTemp110.Checked)
            {
                theWizard.glasTransitionTemp = WizardData.GlassTransitionTemp.temp110;
            }

            theWizard.dielectricConstant = WizardData.DielectricConstant.All;
            if (rbDk425.Checked)
            {
                theWizard.dielectricConstant = WizardData.DielectricConstant.dk425;
            }
            else if (rbDk400.Checked)
            {
                theWizard.dielectricConstant = WizardData.DielectricConstant.dk400;
            }
            else if (rbDk375.Checked)
            {
                theWizard.dielectricConstant = WizardData.DielectricConstant.dk375;
            }
            else if (rbDk350.Checked)
            {
                theWizard.dielectricConstant = WizardData.DielectricConstant.dk350;
            }
            else if (rbDk325.Checked)
            {
                theWizard.dielectricConstant = WizardData.DielectricConstant.dk325;
            }

            theWizard.UpperDf = ConvertDfValue(cbUpperDf.SelectedItem as string);
            theWizard.LowerDf = ConvertDfValue(cbLowerDf.SelectedItem as string);

            //pageThickness
            theWizard.dmlParList.Clear();
            foreach (object x in cbParMaterial.Items)
            {
                theWizard.dmlParList.Add(x as string);
            }
            if (cbParManufacturer.SelectedItem != null)
            {
                theWizard.selParManufacturer = cbParManufacturer.SelectedItem as string;
            }
            if (cbParMaterial.SelectedItem != null)
            {
                theWizard.selParMaterialId = (cbParMaterial.SelectedItem as MaterialData).ID;
            }
            if (cbParCores.SelectedItem != null)
            {
                theWizard.selParCoreId = (cbParCores.SelectedItem as MaterialData).ID;
            }
            if (cbParPrepregs.SelectedItem != null)
            {
                theWizard.selParPrepregId = (cbParPrepregs.SelectedItem as MaterialData).ID;
                theWizard.nPlies = cbParsPlies.Text;
            }
        }

        double ConvertDfValue(string item)
        {
            double retval = 0;
            switch (item)
            {
                case "0.020 - Standard Loss" :
                    retval = 0.020;
                    break;
                case "0.0175" : 
                    retval = 0.00175;
                    break;
                case "0.015 - Mid Loss" : 
                    retval = 0.015;
                    break;
                case "0.0125" : 
                    retval = 0.0125;
                    break;
                case "0.010 - Low Loss" : 
                    retval = 0.010;
                    break;
                case "0.0075" : 
                    retval = 0.0075;
                    break;
                case "0.005 - Ultra-Low Loss" : 
                    retval = 0.005;
                    break;
                case "0.0025": 
                    retval = 0.0025;
                    break;
                case "0":
                    retval = 0;
                    break;
            }
            return retval;
        }
        const string theFirstManufacturer = " Select manufacturer...";
        const string theFirstMaterial = " Select material...";
        const string theFirstMaterialID = " ";

        // navigation
        void SelectManufacturers()
        {
            ZParameter parameter = ZPlannerManager.Dml.Parameters.GetByID("Manufacturer");
            List<string> s = new List<string>(parameter.MaterialParameters.Select(X => X.Value).Distinct());
            s.Insert(0, theFirstManufacturer);
            cbManufacturer.DataSource = s;
        }

        void MarkStepNumber(int n, int nSteps)
        {
            switch (n)
            {
                case 0:
                    pictStep1.BackColor = Color.DarkGray;
                    pictStep2.BackColor = Color.DarkGray;
                    pictStep3.BackColor = Color.DarkGray;
                    pictStep4.BackColor = Color.DarkGray;
                    break;
                case 1:
                    pictStep1.BackColor = Color.Blue;
                    pictStep2.BackColor = Color.DarkGray;
                    pictStep3.BackColor = Color.DarkGray;
                    pictStep4.BackColor = Color.DarkGray;
                    break;
                case 2:
                    pictStep1.BackColor = Color.Blue;
                    pictStep2.BackColor = Color.Blue;
                    pictStep3.BackColor = Color.DarkGray;
                    pictStep4.BackColor = Color.DarkGray;
                    break;
                case 3:
                    pictStep1.BackColor = Color.Blue;
                    pictStep2.BackColor = Color.Blue;
                    pictStep3.BackColor = Color.Blue;
                    pictStep4.BackColor = Color.DarkGray;
                    break;
                case 4:
                    pictStep1.BackColor = Color.Blue;
                    pictStep2.BackColor = Color.Blue;
                    pictStep3.BackColor = Color.Blue;
                    pictStep4.BackColor = Color.Blue;
                    break;
            }
            switch (nSteps)
            {
                case 3: pictStep4.Visible = false;
                    break;
                case 4: pictStep4.Visible = true;
                    break;
            }
        }
        void SelectTab()
        {
            string prefix = "Stackup Wizard - ";
            switch (theWizard.state)
            {
                case WizardData.State.pageMetals:
                    tabControlWizard.SelectedTab = tabMetals;
                    this.Text = prefix + "Metal Layers";
                    MarkStepNumber(1,4);
                    break;
                case WizardData.State.pageDielectrics:
                    tabControlWizard.SelectedTab = tabDielectrics;
                    this.Text = prefix + "Dielectrics";
                    MarkStepNumber(2,4);
                    break;
                case WizardData.State.pageGenericDielectric:
                    tabControlWizard.SelectedTab = tabGenericDielectric;
                    this.Text = prefix + "Generic Dielectric Selection";
                    MarkStepNumber(3,3);
                    break;
                case WizardData.State.pageManufacturerDielectric:
                    tabControlWizard.SelectedTab = tabManufacturer;
                    this.Text = prefix + "Manufacturer-Specific Dielectric Selection";
                    MarkStepNumber(3,3);
                    break;
                case WizardData.State.pageMaterial:
                    tabControlWizard.SelectedTab = tabMaterial;
                    this.Text = prefix + "Select Material by Parameter";
                    MarkStepNumber(3,4);
                    break;
                case WizardData.State.pageThickness:
                    tabControlWizard.SelectedTab = tabThickness;
                    this.Text = prefix + "Manufacturer-Specific Dielectric Selection";
                    MarkStepNumber(4,4);
                    break;
            }

            this.ActiveControl = btnNext;
        }
        void OnPrev()
        {
            SaveData();

            theWizard.Prev();

            btnBack.Enabled = theWizard.state != WizardData.State.pageMetals;
            
            switch (theWizard.state)
            {
                case WizardData.State.pageMetals:
                    btnBack.Enabled = false;
                    break;
            }
            btnNext.Text = "Next >";
            btnNext.Enabled = true;

            SelectTab();
        }

        void OnNext()
        {
            if (theWizard.state == WizardData.State.pageMaterial)
            {
                SelectMaterialsByParameters();
                if (cbParManufacturer.Items.Count > 0) cbParManufacturer.SelectedIndex = 0;
                cbParManufacturer_SelectedIndexChanged(cbParManufacturer, new EventArgs());
                if (cbParMaterial.Items.Count > 0) cbParMaterial.SelectedIndex = 0;
                cbParMaterial_SelectedIndexChanged(cbParMaterial, new EventArgs());
            }

            SaveData();

            theWizard.Next();

            btnBack.Enabled = true;

            btnNext.Text = "Next >";
            switch (theWizard.state)
            {
                case WizardData.State.pageGenericDielectric:
                    tbTargetBoardThickness.Enabled = rbHeightFromBoardThickness.Checked;
                    tbDielectricHeight.Enabled = rbSpecifyDielectricHeight.Checked;
                    btnNext.Text = "Finish";

                    //board thickness estimate
                    theWizard.EstimateBoardThickness();
                    tbTargetBoardThickness.Text = theWizard.targetBoardWidth.ToString("N1"); 
                    break;
                case WizardData.State.pageManufacturerDielectric:
                    btnNext.Enabled = theWizard.selManufacturer != theFirstManufacturer;
                    btnNext.Text = "Finish";
                    break;
                case WizardData.State.pageThickness:
                    btnNext.Enabled = theWizard.selParMaterialId != theFirstMaterialID;
                    btnNext.Text = "Finish";
                    break;
            }

            SelectTab();
        }

        private void btnBack_Click(object sender, EventArgs e)
        {
            OnPrev();
        }

        private void btnNext_Click(object sender, EventArgs e)
        {
            //OnNext();

            //this may take time
            Cursor currentCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;

            switch (theWizard.state)
            {
                case WizardData.State.pageGenericDielectric:
                case WizardData.State.pageManufacturerDielectric:
                case WizardData.State.pageThickness:
                    SaveData();
                    Close();

                    //-------------
                    //if (!ZPlannerManager.CloseProject()) return;
                    //ZPlannerManager.MessagePanel.ClearMessages();

                    ZPlannerManager.Commands.IsIgnoreCommands = true;
                    if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
                    bool commentsShouldBeDisplayed = theWizard.CreateStackup();
                    ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);

                    if (commentsShouldBeDisplayed) ShowComments();
                    ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();
                    ZPlannerManager.StatusMenu.SetStatusReady();
                    ZPlannerManager.MessagePanel.AddMessage(ZPlannerManager.Project.Stackup.GetMetallLayerCount() + " layer Stackup was created.");

                    ZPlannerManager.UpdateActiveStackup();
                    ZPlannerManager.Commands.Clear();
                    ZPlannerManager.Commands.Add(new RunWizardCommand(ZPlannerManager.Project));
                    break;
                default:
                    OnNext();
                    break;
            }

            //restore cursor
            Cursor.Current = currentCursor;
        }

        private void ShowComments()
        {

            ZParameter commentsParameter = ZPlannerManager.Project.Parameters.Find(x => x.ID == ZStringConstants.ParameterIDComments);
            ZParameter thicknessParameter = ZPlannerManager.Project.Parameters.Find(x => x.ID == ZStringConstants.ParameterIDThickness);
            if (commentsParameter != null && thicknessParameter != null)
            {
                commentsParameter.Order = thicknessParameter.Order + 1;
                commentsParameter.IsHidden = false;
            }
        }

        private void UpdateLayerGrid()
        {
            //gridMetalLayer
            gridMetalLayers.Columns.Clear();
            gridMetalLayers.Rows.Clear();

            gridMetalLayers.ColumnCount = 1;
            gridMetalLayers.Columns[0].Name = "Layer";
            for (int j = 0; j < theWizard.numberOfLayers; j++)
            {
                string[] row = new string[] { (j + 1).ToString() };
                gridMetalLayers.Rows.Add(row);
            }
            DataGridViewComboBoxColumn cmb = new DataGridViewComboBoxColumn();
            cmb.Name = "Type";
            cmb.MaxDropDownItems = 3;
            cmb.Items.Add("Signal");
            cmb.Items.Add("Plane");
            cmb.Items.Add("Mixed");

            gridMetalLayers.Columns.Add(cmb);

            for (int j = 0; j < theWizard.numberOfLayers; j++)
            {
                gridMetalLayers.Rows[j].Cells[1].Value = (gridMetalLayers.Rows[j].Cells[1] as DataGridViewComboBoxCell).Items[0];
            }
            gridMetalLayers.Columns["Layer"].HeaderText = "Metal layer #";
            gridMetalLayers.Columns["Type"].HeaderText = "Metal layer type";
            gridMetalLayers.Columns["Layer"].DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

            gridMetalLayers.Columns["Layer"].ReadOnly = true;
            gridMetalLayers.Columns["Layer"].DefaultCellStyle.BackColor = SystemColors.Control;

            suggestPlanes();
        }

        private void cbNumberOfLayers_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            string sel = box.SelectedItem as string;
            int layerNum = Convert.ToInt32(sel);
            int iSeqLam = Convert.ToInt32(cbSeqLam.SelectedItem as string);

            theWizard.numberOfLayers = layerNum + 2*iSeqLam;

            UpdateLayerGrid();
        }

        private void cbSeqLam_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            string sel = box.SelectedItem as string;
            int iSeqLam = Convert.ToInt32(sel);
            int  layerNum = Convert.ToInt32(cbNumberOfLayers.SelectedItem as string);

            theWizard.numberOfLayers = layerNum + 2 * iSeqLam;

            UpdateLayerGrid();
        }


        private void cbManufacturer_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            string sel = box.SelectedItem as string;

            bool bFirst = sel == theFirstManufacturer;
            cbDMLList.Enabled = !bFirst;
            cbPrepregs.Enabled = !bFirst;
            cbPlies.Enabled = !bFirst;
            cbCores.Enabled = !bFirst;
            cbPlies.Text = "1";

            lbDMLList.Enabled = !bFirst;
            lbPrepregs.Enabled = !bFirst;
            lbCores.Enabled = !bFirst;

            if (tabControlWizard.SelectedTab == tabManufacturer) //tabGenericDielectric)
            {
                btnNext.Enabled = !bFirst;
            }

//            if (bFirst)
//            {
//                return;
//            }

//            btnNext.Enabled = true;

            List<MaterialData> mData = DMLQuery.SelectUniqueMaterialData(sel);

            if (mData.Count > 0)
            {
                cbDMLList.DataSource = mData;
                cbDMLList.DisplayMember = "material";
                cbDMLList.ValueMember = "ID";
            }
            else
            {
                cbDMLList.DataSource = null;
                cbCores.DataSource = null;
                cbPrepregs.DataSource = null;
            }
            
        }

        private void cbDMLList_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            MaterialData sel = box.SelectedItem as MaterialData;
            if (sel == null) return;

            List<MaterialData> mData = DMLQuery.SelectMaterialByManufacturer(sel.company, true, sel.material);
            if (mData.Count >= 0)
            {
                foreach (MaterialData md in mData)
                {
                    md.title = md.formatTitle();
                }
                cbCores.DataSource = mData;
                cbCores.DisplayMember = "title";
                cbCores.ValueMember = "ID";
            }

            List<MaterialData> mData1 = DMLQuery.SelectMaterialByManufacturer(sel.company, false, sel.material);
            if (mData1.Count >= 0)
            {
                foreach (MaterialData md in mData1)
                {
                    md.title = md.formatTitle();
                }
                cbPrepregs.DataSource = mData1;
                cbPrepregs.DisplayMember = "title";
                cbPrepregs.ValueMember = "ID";

                cbPlies.Text = "1";
            }            

        }

        private void GetMaterialParameters(out double minTg, out double maxDk, out double minDf, out double maxDf)
        {
            //get parameters value
            minTg = 0; //all
            if (rbTemp200.Checked)
            {
                minTg = 200;
            }
            else if (rbTemp170.Checked)
            {
                minTg = 170;
            }
            else if (rbTemp150.Checked)
            {
                minTg = 150;
            }
            else if (rbTemp110.Checked)
            {
                minTg = 110;
            }

            maxDk = 10; //all
            if (rbDk425.Checked)
            {
                maxDk = 4.25;
            }
            else if (rbDk400.Checked)
            {
                maxDk = 4;
            }
            else if (rbDk375.Checked)
            {
                maxDk = 3.75;
            }
            else if (rbDk350.Checked)
            {
                maxDk = 3.5;
            }
            else if (rbDk325.Checked)
            {
                maxDk = 3.25;
            }

            double a = ConvertDfValue(cbLowerDf.SelectedItem as string);
            double b = ConvertDfValue(cbUpperDf.SelectedItem as string);

            minDf = Math.Min(a, b);
            maxDf = Math.Max(a, b);
        }

        private void SelectMaterialsByParameters()
        {
            //get parameters value
            double minTg, maxDk, minDf, maxDf;
            GetMaterialParameters(out minTg, out maxDk, out minDf, out maxDf);

            //query DML
            List<MaterialData> ml = DMLQuery.SelectMaterialsByParameters(minTg, maxDk, minDf, maxDf);
            MaterialData md1 = new MaterialData();
            md1.material = theFirstMaterial;
            md1.company = theFirstManufacturer;
            md1.ID = theFirstMaterialID;
            ml.Insert(0, md1);
            theWizard.groupList = ml;

            // manufacturers list
            List<string> s = new List<string>(ml.Select(X => X.company).Distinct());
            //--s.Insert(0, theFirstManufacturer);
            //--cbParManufacturer.DataSource = null;
            //-- cbParManufacturer.DataSource = s;
            cbParManufacturer.Items.Clear();
            foreach (string s1 in s)
            {
                cbParManufacturer.Items.Add(s1);
            }

        }


        private void tbTargetBoardThickness_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Thickness", 0.5, 100);
        }

        private void tbSignalTraceWidth_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Trace width", 0.5, 100);
        }

        private void tbTargetDk_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Dielectric Constant", 1, 7);
        }

        private void tbTargetDf_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Dissipation Factor", 0.01, 0.05);
        }

        private void tbDielectricHeight_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Dielectric Height", 1.5, 50);
        }

        private void tbThickness_DielectricHeight_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Dielectric Height", 1.5, 50);
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void enforceSymmetry()
        {
            int nLayers = theWizard.numberOfLayers;
                for (int i = 1; i <= nLayers / 2; i++){
                    //find twin
                    int j = nLayers + 1 - i;
                    switch (gridMetalLayers.Rows[i - 1].Cells[1].Value as string)
                    {
                        case "Signal":
                            gridMetalLayers.Rows[j - 1].Cells[1].Value = "Signal";
                            break;
                        case "Mixed":
                            gridMetalLayers.Rows[j - 1].Cells[1].Value = "Mixed";
                            break;
                        case "Plane":
                            gridMetalLayers.Rows[j - 1].Cells[1].Value = "Plane";
                            break;
                    }
                }            
        }

        private void cbSymmetrical_CheckedChanged(object sender, EventArgs e)
        {
            if (cbSymmetrical.Checked)
            {
                enforceSymmetry();
            }
        }

        private void gridMetalLayers_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            if (gridMetalLayers.CurrentCell is DataGridViewComboBoxCell)
            {
                gridMetalLayers.CommitEdit(DataGridViewDataErrorContexts.Commit);
                //gridMetalLayers.EndEdit();
            }
        }

        private bool checkPlanes()
        {
            int nLayers = Convert.ToInt32(cbNumberOfLayers.SelectedItem);
            if (nLayers < 3)
            {
                return true;
            }

            int nPlanes = 0, nSignals = 0, nMixed = 0;
            for (int i = 0; i < nLayers; i++)
            {
                switch (gridMetalLayers.Rows[i].Cells[1].Value as string)
                {
                    case "Signal":
                        nSignals++;
                        break;
                    case "Mixed":
                        nMixed++;
                        break;
                    case "Plane":
                        nPlanes++;
                        break;
                }
            }
            return nPlanes + nMixed > 0;
        }

        private void gridMetalLayers_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (cbSymmetrical.Checked)
            {
                enforceSymmetry();
            }

            bool bPlanes = checkPlanes();
            btnNext.Enabled = bPlanes;
            rtbHintPlanes.ForeColor = bPlanes ? SystemColors.WindowText : Color.Red;            
        }

        private void cbParManufacturer_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            string sel = box.SelectedItem as string;

            bool bFirst = sel == theFirstManufacturer;
            cbParsPlies.Text = "1";
            /*
            cbParMaterial.Enabled = !bFirst;
            cbParPrepregs.Enabled = !bFirst;
            cbParCores.Enabled = !bFirst;

            lbParMaterial.Enabled = !bFirst;
            lbParPrepregs.Enabled = !bFirst;
            lbParCores.Enabled = !bFirst;
            
            if (tabControlWizard.SelectedTab == tabThickness)
            {
                btnNext.Enabled = !bFirst;
            }
            if (bFirst)
            {
                return;
            }
            */


            List<MaterialData> mData = bFirst ? theWizard.groupList : DMLQuery.SelectMaterialDataByManufacturer(theWizard.groupList, sel);

            IEnumerable<MaterialData> iMD = mData.GroupBy(x => x.material).Select(grp => grp.First());
            List<MaterialData> materialList = iMD.ToList();

            if (mData.Count > 0)
            {
                cbParMaterial.DataSource = materialList;
                cbParMaterial.DisplayMember = "material";
                cbParMaterial.ValueMember = "ID";
            }
            else
            {
                cbParMaterial.DataSource = null;
            }
            
        }

        private void cbParMaterial_SelectedIndexChanged(object sender, EventArgs e)
        {
            ComboBox box = sender as ComboBox;
            MaterialData sel = box.SelectedItem as MaterialData;
            if (sel == null) return;
            
            bool bFirst = sel.material == theFirstMaterial;

            List<MaterialData> mData = DMLQuery.SelectMaterialDataByManufacturer(theWizard.groupList, sel.company, true, sel.material);
            if (mData.Count >= 0)
            {
                foreach (MaterialData md in mData)
                {
                    md.title = md.formatTitle();
                }
                cbParCores.DataSource = mData;
                cbParCores.DisplayMember = "title";
                cbParCores.ValueMember = "ID";
            }

            List<MaterialData> mData1 = DMLQuery.SelectMaterialDataByManufacturer(theWizard.groupList, sel.company, false, sel.material);
            if (mData1.Count >= 0)
            {
                foreach (MaterialData md in mData1)
                {
                    md.title = md.formatTitle();
                }
                cbParPrepregs.DataSource = mData1;
                cbParPrepregs.DisplayMember = "title";
                cbParPrepregs.ValueMember = "ID";
                cbParsPlies.Text = "1";
            }

            cbParPrepregs.Enabled = !bFirst;
            cbParCores.Enabled = !bFirst;

            lbParPrepregs.Enabled = !bFirst;
            cbParsPlies.Enabled = !bFirst;
            lbParCores.Enabled = !bFirst;

            btnNext.Enabled = !bFirst;
        }

        private void rbHeightFromBoardThickness_CheckedChanged(object sender, EventArgs e)
        {
            tbTargetBoardThickness.Enabled = rbHeightFromBoardThickness.Checked;
            tbDielectricHeight.Enabled = rbSpecifyDielectricHeight.Checked;
        }

        private void rbSpecifyDielectricHeight_CheckedChanged(object sender, EventArgs e)
        {
            tbTargetBoardThickness.Enabled = rbHeightFromBoardThickness.Checked;
            tbDielectricHeight.Enabled = rbSpecifyDielectricHeight.Checked;
        }

        private void tbTargetBoardThickness_Validating_1(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Board Thickness", 10, 300);
        }

        private void suggestPlanes()
        {
            int nLayers = theWizard.numberOfLayers;//Convert.ToInt32(cbNumberOfLayers.SelectedItem);
            if (nLayers < 3) return;

            List <int> lPlanes = new List<int>();
            switch (nLayers){
                default: //TEMP workaround
                    lPlanes.Add(2);
                    lPlanes.Add(nLayers - 1);
                    break;
            }

            for (int i = 0; i < lPlanes.Count; i++)
            {
                int row = lPlanes[i] - 1;
                gridMetalLayers.Rows[row].Cells[1].Value = "Plane";
            }
        }


        private void tbOuterMetalWidth_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Trace width", 0.5, 100);
        }

        private void tbOuterMetalDiffSpacing_Validating(object sender, CancelEventArgs e)
        {
            TextBoxValidator.CheckRange(sender, "Trace spacing", 0.5, 100);
        }

        private void cbPrepregs_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbPlies.Text = "1";
        }

        private void cbParPrepregs_SelectedIndexChanged(object sender, EventArgs e)
        {
            cbParsPlies.Text = "1";
        }

    }

}
