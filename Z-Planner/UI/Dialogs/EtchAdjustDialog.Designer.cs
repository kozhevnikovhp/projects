namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class EtchAdjustDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(EtchAdjustDialog));
            this.groupBox12 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label42 = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.tbEtch = new System.Windows.Forms.TextBox();
            this.label41 = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.cbShowEtchEffects = new System.Windows.Forms.CheckBox();
            this.checkBoxUpdateSettings = new System.Windows.Forms.CheckBox();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.groupBox12.SuspendLayout();
            this.groupBox3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // groupBox12
            // 
            this.groupBox12.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox12.Controls.Add(this.groupBox3);
            this.groupBox12.Controls.Add(this.label9);
            this.groupBox12.Controls.Add(this.tbEtch);
            this.groupBox12.Controls.Add(this.label41);
            this.groupBox12.Controls.Add(this.pictureBox1);
            this.groupBox12.Location = new System.Drawing.Point(16, 47);
            this.groupBox12.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox12.Name = "groupBox12";
            this.groupBox12.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox12.Size = new System.Drawing.Size(573, 170);
            this.groupBox12.TabIndex = 7;
            this.groupBox12.TabStop = false;
            this.groupBox12.Text = "Etch effects";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label42);
            this.groupBox3.Location = new System.Drawing.Point(288, 24);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(265, 102);
            this.groupBox3.TabIndex = 28;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Hint";
            // 
            // label42
            // 
            this.label42.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label42.Location = new System.Drawing.Point(3, 16);
            this.label42.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label42.Name = "label42";
            this.label42.Size = new System.Drawing.Size(259, 83);
            this.label42.TabIndex = 27;
            this.label42.Text = resources.GetString("label42.Text");
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(285, 141);
            this.label9.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(78, 13);
            this.label9.TabIndex = 26;
            this.label9.Text = "Range: 0.5-1.0";
            // 
            // tbEtch
            // 
            this.tbEtch.Location = new System.Drawing.Point(174, 137);
            this.tbEtch.Margin = new System.Windows.Forms.Padding(2);
            this.tbEtch.Name = "tbEtch";
            this.tbEtch.Size = new System.Drawing.Size(61, 20);
            this.tbEtch.TabIndex = 25;
            this.tbEtch.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label41
            // 
            this.label41.Location = new System.Drawing.Point(11, 140);
            this.label41.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label41.Name = "label41";
            this.label41.Size = new System.Drawing.Size(159, 13);
            this.label41.TabIndex = 24;
            this.label41.Text = "Ratio of top to bottom (w2/w1):";
            this.label41.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::ZZero.ZPlanner.Properties.Resources.EtchEffect;
            this.pictureBox1.Location = new System.Drawing.Point(8, 24);
            this.pictureBox1.Margin = new System.Windows.Forms.Padding(2);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(159, 102);
            this.pictureBox1.TabIndex = 5;
            this.pictureBox1.TabStop = false;
            // 
            // cbShowEtchEffects
            // 
            this.cbShowEtchEffects.AutoSize = true;
            this.cbShowEtchEffects.Location = new System.Drawing.Point(24, 15);
            this.cbShowEtchEffects.Margin = new System.Windows.Forms.Padding(2);
            this.cbShowEtchEffects.Name = "cbShowEtchEffects";
            this.cbShowEtchEffects.Size = new System.Drawing.Size(120, 17);
            this.cbShowEtchEffects.TabIndex = 8;
            this.cbShowEtchEffects.Text = "Enable Etch Effects";
            this.cbShowEtchEffects.UseVisualStyleBackColor = true;
            // 
            // checkBoxUpdateSettings
            // 
            this.checkBoxUpdateSettings.AutoSize = true;
            this.checkBoxUpdateSettings.Location = new System.Drawing.Point(24, 237);
            this.checkBoxUpdateSettings.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxUpdateSettings.Name = "checkBoxUpdateSettings";
            this.checkBoxUpdateSettings.Size = new System.Drawing.Size(257, 17);
            this.checkBoxUpdateSettings.TabIndex = 9;
            this.checkBoxUpdateSettings.Text = "Use these values by default for all new stackups.";
            this.checkBoxUpdateSettings.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(421, 274);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(2);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(82, 23);
            this.btnCancel.TabIndex = 10;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(507, 274);
            this.btnOK.Margin = new System.Windows.Forms.Padding(2);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 23);
            this.btnOK.TabIndex = 11;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // EtchAdjustDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(607, 314);
            this.Controls.Add(this.checkBoxUpdateSettings);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.cbShowEtchEffects);
            this.Controls.Add(this.groupBox12);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "EtchAdjustDialog";
            this.Text = "  Etch Effect Settings";
            this.groupBox12.ResumeLayout(false);
            this.groupBox12.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox12;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label42;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox tbEtch;
        private System.Windows.Forms.Label label41;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.CheckBox cbShowEtchEffects;
        private System.Windows.Forms.CheckBox checkBoxUpdateSettings;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
    }
}