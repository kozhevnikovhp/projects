namespace ZZero.ZPlanner.UI.Menu
{
    partial class ViaSpanMenu
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ViaSpanMenu));
            this.viaSpanRibbonMenu = new System.Windows.Forms.Ribbon();
            this.viaSpanRibbonTab = new System.Windows.Forms.RibbonTab();
            this.viaSpanRibbonPanel = new System.Windows.Forms.RibbonPanel();
            this.addViaSpanRibbonButton = new System.Windows.Forms.RibbonButton();
            this.removeViaSpanBibbonButton = new System.Windows.Forms.RibbonButton();
            this.SuspendLayout();
            // 
            // viaSpanRibbonMenu
            // 
            this.viaSpanRibbonMenu.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.viaSpanRibbonMenu.CaptionBarVisible = false;
            this.viaSpanRibbonMenu.CausesValidation = false;
            this.viaSpanRibbonMenu.Font = new System.Drawing.Font("Segoe UI", 9F);
            this.viaSpanRibbonMenu.ImeMode = System.Windows.Forms.ImeMode.NoControl;
            this.viaSpanRibbonMenu.Location = new System.Drawing.Point(0, 0);
            this.viaSpanRibbonMenu.Margin = new System.Windows.Forms.Padding(0);
            this.viaSpanRibbonMenu.Minimized = false;
            this.viaSpanRibbonMenu.Name = "viaSpanRibbonMenu";
            // 
            // 
            // 
            this.viaSpanRibbonMenu.OrbDropDown.BorderRoundness = 8;
            this.viaSpanRibbonMenu.OrbDropDown.Location = new System.Drawing.Point(0, 0);
            this.viaSpanRibbonMenu.OrbDropDown.Margin = new System.Windows.Forms.Padding(0);
            this.viaSpanRibbonMenu.OrbDropDown.Name = "";
            this.viaSpanRibbonMenu.OrbDropDown.OptionItemsPadding = 1;
            this.viaSpanRibbonMenu.OrbDropDown.Size = new System.Drawing.Size(527, 447);
            this.viaSpanRibbonMenu.OrbDropDown.TabIndex = 0;
            this.viaSpanRibbonMenu.OrbImage = null;
            this.viaSpanRibbonMenu.OrbStyle = System.Windows.Forms.RibbonOrbStyle.Office_2013;
            this.viaSpanRibbonMenu.OrbVisible = false;
            this.viaSpanRibbonMenu.PanelCaptionHeight = 0;
            // 
            // 
            // 
            this.viaSpanRibbonMenu.QuickAcessToolbar.Visible = false;
            this.viaSpanRibbonMenu.RibbonTabFont = new System.Drawing.Font("Trebuchet MS", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.viaSpanRibbonMenu.Size = new System.Drawing.Size(835, 35);
            this.viaSpanRibbonMenu.TabIndex = 0;
            this.viaSpanRibbonMenu.Tabs.Add(this.viaSpanRibbonTab);
            this.viaSpanRibbonMenu.TabsMargin = new System.Windows.Forms.Padding(5000, 0, 0, 0);
            this.viaSpanRibbonMenu.TabSpacing = 0;
            this.viaSpanRibbonMenu.TabStop = false;
            this.viaSpanRibbonMenu.Text = "Via Spans";
            this.viaSpanRibbonMenu.ThemeColor = System.Windows.Forms.RibbonTheme.Blue;
            // 
            // viaSpanRibbonTab
            // 
            this.viaSpanRibbonTab.Panels.Add(this.viaSpanRibbonPanel);
            this.viaSpanRibbonTab.Text = "";
            // 
            // viaSpanRibbonPanel
            // 
            this.viaSpanRibbonPanel.ButtonMoreVisible = false;
            this.viaSpanRibbonPanel.Items.Add(this.addViaSpanRibbonButton);
            this.viaSpanRibbonPanel.Items.Add(this.removeViaSpanBibbonButton);
            this.viaSpanRibbonPanel.PanelSeparatorVisible = false;
            this.viaSpanRibbonPanel.Text = "";
            // 
            // addViaSpanRibbonButton
            // 
            this.addViaSpanRibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("addViaSpanRibbonButton.Image")));
            this.addViaSpanRibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.addViaSpanRibbonButton.MinSizeMode = System.Windows.Forms.RibbonElementSizeMode.Compact;
            this.addViaSpanRibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Insert_Blank_Row_16;
            this.addViaSpanRibbonButton.Text = "Add Via Span";
            this.addViaSpanRibbonButton.Value = "AddViaSpan";
            this.addViaSpanRibbonButton.Click += new System.EventHandler(this.addViaSpanRibbonButton_Click);
            // 
            // removeViaSpanBibbonButton
            // 
            this.removeViaSpanBibbonButton.Image = ((System.Drawing.Image)(resources.GetObject("removeViaSpanBibbonButton.Image")));
            this.removeViaSpanBibbonButton.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
            this.removeViaSpanBibbonButton.MinSizeMode = System.Windows.Forms.RibbonElementSizeMode.Compact;
            this.removeViaSpanBibbonButton.SmallImage = global::ZZero.ZPlanner.Properties.Resources.Delete_Row_16;
            this.removeViaSpanBibbonButton.Text = "Delete Via Span";
            this.removeViaSpanBibbonButton.Value = "RemoveViaSpan";
            this.removeViaSpanBibbonButton.Click += new System.EventHandler(this.removeViaSpanBibbonButton_Click);
            // 
            // ViaSpanMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.viaSpanRibbonMenu);
            this.Name = "ViaSpanMenu";
            this.Size = new System.Drawing.Size(835, 35);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.RibbonTab viaSpanRibbonTab;
        private System.Windows.Forms.RibbonPanel viaSpanRibbonPanel;
        private System.Windows.Forms.Ribbon viaSpanRibbonMenu;
        private System.Windows.Forms.RibbonButton addViaSpanRibbonButton;
        private System.Windows.Forms.RibbonButton removeViaSpanBibbonButton;
    }
}
