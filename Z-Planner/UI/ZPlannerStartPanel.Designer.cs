namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerStartPanel
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ZPlannerStartPanel));
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.splitContainer2 = new System.Windows.Forms.SplitContainer();
            this.startRibbonMenu = new System.Windows.Forms.Ribbon();
            this.startRibbonTab = new System.Windows.Forms.RibbonTab();
            this.startRibbonPanel = new System.Windows.Forms.RibbonPanel();
            this.newStackupRibbonButton = new System.Windows.Forms.RibbonButton();
            this.stackupWizardRibbonButton = new System.Windows.Forms.RibbonButton();
            this.importWizardRibbonButton = new System.Windows.Forms.RibbonButton();
            this.genericStackupsRibbonButton = new System.Windows.Forms.RibbonButton();
            this.openStackupRibbonButton = new System.Windows.Forms.RibbonButton();
            this.fieldSolverRibbonButton = new System.Windows.Forms.RibbonButton();
            this.recentRibbonMenu = new System.Windows.Forms.Ribbon();
            this.recentRibbonTab = new System.Windows.Forms.RibbonTab();
            this.recentRibbonPanel = new System.Windows.Forms.RibbonPanel();
            this.label4 = new System.Windows.Forms.Label();
            this.tutorialPanel = new ZZero.ZPlanner.UI.Help.HelpTopicsPanel();
            this.syncZoLibRibbonButton = new System.Windows.Forms.RibbonButton();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).BeginInit();
            this.splitContainer2.Panel1.SuspendLayout();
            this.splitContainer2.Panel2.SuspendLayout();
            this.splitContainer2.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Dock = System.Windows.Forms.DockStyle.Top;
            this.label1.Font = new System.Drawing.Font("Bookman Old Style", 20F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(148, 32);
            this.label1.TabIndex = 0;
            this.label1.Text = "Z-planner";
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.label2.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label2.ForeColor = System.Drawing.SystemColors.GrayText;
            this.label2.Location = new System.Drawing.Point(3, 0);
            this.label2.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(524, 17);
            this.label2.TabIndex = 1;
            this.label2.Text = "Start";
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label3.ForeColor = System.Drawing.SystemColors.GrayText;
            this.label3.Location = new System.Drawing.Point(2, 0);
            this.label3.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(525, 17);
            this.label3.TabIndex = 2;
            this.label3.Text = "Recent Files";
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 32);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.label4);
            this.splitContainer1.Panel2.Controls.Add(this.tutorialPanel);
            this.splitContainer1.Size = new System.Drawing.Size(1098, 664);
            this.splitContainer1.SplitterDistance = 529;
            this.splitContainer1.TabIndex = 2;
            // 
            // splitContainer2
            // 
            this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer2.Location = new System.Drawing.Point(0, 0);
            this.splitContainer2.Name = "splitContainer2";
            this.splitContainer2.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer2.Panel1
            // 
            this.splitContainer2.Panel1.Controls.Add(this.startRibbonMenu);
            this.splitContainer2.Panel1.Controls.Add(this.label2);
            // 
            // splitContainer2.Panel2
            // 
            this.splitContainer2.Panel2.Controls.Add(this.recentRibbonMenu);
            this.splitContainer2.Panel2.Controls.Add(this.label3);
            this.splitContainer2.Size = new System.Drawing.Size(529, 664);
            this.splitContainer2.SplitterDistance = 273;
            this.splitContainer2.TabIndex = 0;
            // 
            // startRibbonMenu
            // 
            this.startRibbonMenu.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.startRibbonMenu.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.startRibbonMenu.CaptionBarVisible = false;
            this.startRibbonMenu.Font = new System.Drawing.Font("Segoe UI", 9F);
            this.startRibbonMenu.ItemMargin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.startRibbonMenu.ItemPadding = new System.Windows.Forms.Padding(1, 0, 1, 0);
            this.startRibbonMenu.Location = new System.Drawing.Point(0, 17);
            this.startRibbonMenu.Margin = new System.Windows.Forms.Padding(0);
            this.startRibbonMenu.MaximumSize = new System.Drawing.Size(0, 0);
            this.startRibbonMenu.Minimized = false;
            this.startRibbonMenu.MinimumSize = new System.Drawing.Size(0, 0);
            this.startRibbonMenu.Name = "startRibbonMenu";
            // 
            // 
            // 
            this.startRibbonMenu.OrbDropDown.BorderRoundness = 8;
            this.startRibbonMenu.OrbDropDown.Location = new System.Drawing.Point(0, 0);
            this.startRibbonMenu.OrbDropDown.Name = "";
            this.startRibbonMenu.OrbDropDown.Size = new System.Drawing.Size(527, 447);
            this.startRibbonMenu.OrbDropDown.TabIndex = 0;
            this.startRibbonMenu.OrbImage = null;
            this.startRibbonMenu.OrbStyle = System.Windows.Forms.RibbonOrbStyle.Office_2013;
            this.startRibbonMenu.OrbVisible = false;
            this.startRibbonMenu.PanelMargin = new System.Windows.Forms.Padding(3, 2, 3, 15);
            this.startRibbonMenu.PanelPadding = new System.Windows.Forms.Padding(3);
            // 
            // 
            // 
            this.startRibbonMenu.QuickAcessToolbar.Visible = false;
            this.startRibbonMenu.RibbonTabFont = new System.Drawing.Font("Trebuchet MS", 9F);
            this.startRibbonMenu.Size = new System.Drawing.Size(529, 256);
            this.startRibbonMenu.TabIndex = 0;
            this.startRibbonMenu.Tabs.Add(this.startRibbonTab);
            this.startRibbonMenu.TabsMargin = new System.Windows.Forms.Padding(5000, 0, 0, 0);
            this.startRibbonMenu.TabSpacing = 0;
            this.startRibbonMenu.Text = "ribbon1";
            this.startRibbonMenu.ThemeColor = System.Windows.Forms.RibbonTheme.Blue;
            // 
            // startRibbonTab
            // 
            this.startRibbonTab.Panels.Add(this.startRibbonPanel);
            this.startRibbonTab.Text = "";
            // 
            // startRibbonPanel
            // 
            this.startRibbonPanel.ButtonMoreEnabled = false;
            this.startRibbonPanel.ButtonMoreVisible = false;
            this.startRibbonPanel.Items.Add(this.newStackupRibbonButton);
            this.startRibbonPanel.Items.Add(this.stackupWizardRibbonButton);
            this.startRibbonPanel.Items.Add(this.importWizardRibbonButton);
            this.startRibbonPanel.Items.Add(this.genericStackupsRibbonButton);
            this.startRibbonPanel.Items.Add(this.openStackupRibbonButton);
            this.startRibbonPanel.Items.Add(this.fieldSolverRibbonButton);
            this.startRibbonPanel.PanelSeparatorVisible = false;
            this.startRibbonPanel.Text = "";
            // 
            // newStackupRibbonButton
            // 
            this.newStackupRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("newStackupRibbonButton.Image")));
            this.newStackupRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.newStackupRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.New_Stackup_32;
            this.newStackupRibbonButton.Text = "New Stackup";
            this.newStackupRibbonButton.ToolTip = "New Stackup";
            this.newStackupRibbonButton.Value = "NewStackup";
            this.newStackupRibbonButton.Click += new System.EventHandler(this.newStackupRibbonButton_Click);
            this.newStackupRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // stackupWizardRibbonButton
            // 
            this.stackupWizardRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("stackupWizardRibbonButton.Image")));
            this.stackupWizardRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.stackupWizardRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Run_Stackup_Wizard_32;
            this.stackupWizardRibbonButton.Text = "Stackup Wizard";
            this.stackupWizardRibbonButton.ToolTip = "Stackup Wizard";
            this.stackupWizardRibbonButton.Value = "StackupWizard";
            this.stackupWizardRibbonButton.Click += new System.EventHandler(this.stackupWizardRibbonButton_Click);
            this.stackupWizardRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // importWizardRibbonButton
            // 
            this.importWizardRibbonButton.Image = global::ZZero.ZPlanner.Properties.Resources.Import_32;
            this.importWizardRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.importWizardRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Import_32;
            this.importWizardRibbonButton.Text = "Import Stackup";
            this.importWizardRibbonButton.ToolTip = "Import Stackup";
            this.importWizardRibbonButton.Value = "ImportWizard";
            this.importWizardRibbonButton.Click += new System.EventHandler(this.importWizardRibbonButton_Click);
            this.importWizardRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // genericStackupsRibbonButton
            // 
            this.genericStackupsRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("genericStackupsRibbonButton.Image")));
            this.genericStackupsRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.genericStackupsRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Select_Stackup_32;
            this.genericStackupsRibbonButton.Style = System.Windows.Forms.RibbonButtonStyle.DropDown;
            this.genericStackupsRibbonButton.Text = "Generic Stackups";
            this.genericStackupsRibbonButton.ToolTip = "Generic Stackups";
            this.genericStackupsRibbonButton.Value = "GenericStackups";
            this.genericStackupsRibbonButton.DropDownItemClicked += new System.Windows.Forms.RibbonButton.RibbonItemEventHandler(this.genericStackupsRibbonButton_DropDownItemClicked);
            this.genericStackupsRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // openStackupRibbonButton
            // 
            this.openStackupRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("openStackupRibbonButton.Image")));
            this.openStackupRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.openStackupRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Open_32;
            this.openStackupRibbonButton.Text = "Open Stackup";
            this.openStackupRibbonButton.ToolTip = "Open Stackup";
            this.openStackupRibbonButton.Value = "OpenStackup";
            this.openStackupRibbonButton.Click += new System.EventHandler(this.openStackupRibbonButton_Click);
            this.openStackupRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // fieldSolverRibbonButton
            // 
            this.fieldSolverRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("fieldSolverRibbonButton.Image")));
            this.fieldSolverRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.fieldSolverRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Field_Solver_Sandbox_32;
            this.fieldSolverRibbonButton.Text = "Field Solver";
            this.fieldSolverRibbonButton.ToolTip = "Field Solver";
            this.fieldSolverRibbonButton.Value = "FieldSolver";
            this.fieldSolverRibbonButton.Click += new System.EventHandler(this.fieldSolverRibbonButton_Click);
            this.fieldSolverRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // recentRibbonMenu
            // 
            this.recentRibbonMenu.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.recentRibbonMenu.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.recentRibbonMenu.CaptionBarVisible = false;
            this.recentRibbonMenu.Font = new System.Drawing.Font("Segoe UI", 9F);
            this.recentRibbonMenu.ItemMargin = new System.Windows.Forms.Padding(4, 2, 4, 2);
            this.recentRibbonMenu.ItemPadding = new System.Windows.Forms.Padding(1, 0, 1, 0);
            this.recentRibbonMenu.Location = new System.Drawing.Point(0, 17);
            this.recentRibbonMenu.Margin = new System.Windows.Forms.Padding(0);
            this.recentRibbonMenu.MaximumSize = new System.Drawing.Size(0, 0);
            this.recentRibbonMenu.Minimized = false;
            this.recentRibbonMenu.MinimumSize = new System.Drawing.Size(0, 0);
            this.recentRibbonMenu.Name = "recentRibbonMenu";
            // 
            // 
            // 
            this.recentRibbonMenu.OrbDropDown.BorderRoundness = 8;
            this.recentRibbonMenu.OrbDropDown.Location = new System.Drawing.Point(0, 0);
            this.recentRibbonMenu.OrbDropDown.Name = "";
            this.recentRibbonMenu.OrbDropDown.Size = new System.Drawing.Size(527, 447);
            this.recentRibbonMenu.OrbDropDown.TabIndex = 0;
            this.recentRibbonMenu.OrbImage = null;
            this.recentRibbonMenu.OrbStyle = System.Windows.Forms.RibbonOrbStyle.Office_2013;
            this.recentRibbonMenu.OrbVisible = false;
            this.recentRibbonMenu.PanelMargin = new System.Windows.Forms.Padding(3, 2, 3, 15);
            this.recentRibbonMenu.PanelPadding = new System.Windows.Forms.Padding(3);
            // 
            // 
            // 
            this.recentRibbonMenu.QuickAcessToolbar.Visible = false;
            this.recentRibbonMenu.RibbonTabFont = new System.Drawing.Font("Trebuchet MS", 9F);
            this.recentRibbonMenu.Size = new System.Drawing.Size(529, 370);
            this.recentRibbonMenu.TabIndex = 3;
            this.recentRibbonMenu.Tabs.Add(this.recentRibbonTab);
            this.recentRibbonMenu.TabsMargin = new System.Windows.Forms.Padding(5000, 0, 0, 0);
            this.recentRibbonMenu.TabSpacing = 0;
            this.recentRibbonMenu.ThemeColor = System.Windows.Forms.RibbonTheme.Blue;
            // 
            // recentRibbonTab
            // 
            this.recentRibbonTab.Panels.Add(this.recentRibbonPanel);
            this.recentRibbonTab.Text = "";
            // 
            // recentRibbonPanel
            // 
            this.recentRibbonPanel.ButtonMoreEnabled = false;
            this.recentRibbonPanel.ButtonMoreVisible = false;
            this.recentRibbonPanel.PanelSeparatorVisible = false;
            this.recentRibbonPanel.Text = "";
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.BackColor = System.Drawing.SystemColors.ControlLightLight;
            this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.label4.ForeColor = System.Drawing.SystemColors.GrayText;
            this.label4.Location = new System.Drawing.Point(0, 0);
            this.label4.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(560, 17);
            this.label4.TabIndex = 3;
            this.label4.Text = "Tutorial";
            // 
            // tutorialPanel
            // 
            this.tutorialPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tutorialPanel.Location = new System.Drawing.Point(0, 23);
            this.tutorialPanel.Name = "tutorialPanel";
            this.tutorialPanel.Size = new System.Drawing.Size(560, 638);
            this.tutorialPanel.TabIndex = 2;
            // 
            // syncZoLibRibbonButton
            // 
            this.syncZoLibRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("syncZoLibRibbonButton.Image")));
            this.syncZoLibRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.syncZoLibRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Sync_Zo_Lib_32;
            this.syncZoLibRibbonButton.Text = "Sync Zo Lib";
            this.syncZoLibRibbonButton.ToolTip = "Sync Zo Lib";
            this.syncZoLibRibbonButton.Value = "SyncZzeroLib";
            this.syncZoLibRibbonButton.Click += new System.EventHandler(this.syncZoLibRibbonButton_Click);
            this.syncZoLibRibbonButton.ToolTipPopUp += new System.Windows.Forms.RibbonElementPopupEventHandler(this.programRibbonMenu_ToolTipPopUp);
            // 
            // ZPlannerStartPanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(1098, 696);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.label1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.HideOnClose = true;
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ZPlannerStartPanel";
            this.ShowInTaskbar = false;
            this.Text = "Start Page";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.splitContainer2.Panel1.ResumeLayout(false);
            this.splitContainer2.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer2)).EndInit();
            this.splitContainer2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.SplitContainer splitContainer2;
        private System.Windows.Forms.Ribbon startRibbonMenu;
        private System.Windows.Forms.RibbonTab startRibbonTab;
        private System.Windows.Forms.RibbonPanel startRibbonPanel;
        private System.Windows.Forms.RibbonButton newStackupRibbonButton;
        private System.Windows.Forms.RibbonButton stackupWizardRibbonButton;
        private System.Windows.Forms.RibbonButton genericStackupsRibbonButton;
        private System.Windows.Forms.RibbonButton openStackupRibbonButton;
        private System.Windows.Forms.RibbonButton fieldSolverRibbonButton;
        private System.Windows.Forms.RibbonButton syncZoLibRibbonButton;
        private System.Windows.Forms.Ribbon recentRibbonMenu;
        private System.Windows.Forms.RibbonTab recentRibbonTab;
        private System.Windows.Forms.RibbonPanel recentRibbonPanel;
        private System.Windows.Forms.Label label4;
        private ZZero.ZPlanner.UI.Help.HelpTopicsPanel tutorialPanel;
        private System.Windows.Forms.RibbonButton importWizardRibbonButton;
    }
}