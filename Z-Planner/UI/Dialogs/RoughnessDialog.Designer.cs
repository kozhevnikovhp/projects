namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class RoughnessDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(RoughnessDialog));
            this.label45 = new System.Windows.Forms.Label();
            this.tbOuterRoughness = new System.Windows.Forms.TextBox();
            this.label35 = new System.Windows.Forms.Label();
            this.groupBox10 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label6 = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label48 = new System.Windows.Forms.Label();
            this.label49 = new System.Windows.Forms.Label();
            this.label47 = new System.Windows.Forms.Label();
            this.tbPrepreg_Side_Roughness = new System.Windows.Forms.TextBox();
            this.label22 = new System.Windows.Forms.Label();
            this.tbLaminate_Side_Rougness = new System.Windows.Forms.TextBox();
            this.label25 = new System.Windows.Forms.Label();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.cbShowRoughness = new System.Windows.Forms.CheckBox();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.checkBoxUpdateSettings = new System.Windows.Forms.CheckBox();
            this.groupBox10.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.SuspendLayout();
            // 
            // label45
            // 
            this.label45.AutoSize = true;
            this.label45.Location = new System.Drawing.Point(146, 39);
            this.label45.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label45.Name = "label45";
            this.label45.Size = new System.Drawing.Size(21, 13);
            this.label45.TabIndex = 28;
            this.label45.Text = "um";
            // 
            // tbOuterRoughness
            // 
            this.tbOuterRoughness.Location = new System.Drawing.Point(111, 36);
            this.tbOuterRoughness.Margin = new System.Windows.Forms.Padding(2);
            this.tbOuterRoughness.Name = "tbOuterRoughness";
            this.tbOuterRoughness.Size = new System.Drawing.Size(32, 20);
            this.tbOuterRoughness.TabIndex = 1;
            // 
            // label35
            // 
            this.label35.AutoSize = true;
            this.label35.Location = new System.Drawing.Point(16, 39);
            this.label35.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label35.Name = "label35";
            this.label35.Size = new System.Drawing.Size(94, 13);
            this.label35.TabIndex = 24;
            this.label35.Text = "Outer metal layers:";
            this.label35.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // groupBox10
            // 
            this.groupBox10.Controls.Add(this.label1);
            this.groupBox10.Controls.Add(this.groupBox4);
            this.groupBox10.Controls.Add(this.groupBox3);
            this.groupBox10.Controls.Add(this.label48);
            this.groupBox10.Controls.Add(this.label49);
            this.groupBox10.Controls.Add(this.label47);
            this.groupBox10.Controls.Add(this.tbPrepreg_Side_Roughness);
            this.groupBox10.Controls.Add(this.label22);
            this.groupBox10.Controls.Add(this.tbLaminate_Side_Rougness);
            this.groupBox10.Controls.Add(this.label25);
            this.groupBox10.Location = new System.Drawing.Point(16, 325);
            this.groupBox10.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox10.Name = "groupBox10";
            this.groupBox10.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox10.Size = new System.Drawing.Size(544, 183);
            this.groupBox10.TabIndex = 5;
            this.groupBox10.TabStop = false;
            this.groupBox10.Text = "Inner metal layers";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(113, 148);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(54, 13);
            this.label1.TabIndex = 34;
            this.label1.Text = "0.5-15 um";
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label6);
            this.groupBox4.Location = new System.Drawing.Point(226, 87);
            this.groupBox4.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox4.Size = new System.Drawing.Size(296, 74);
            this.groupBox4.TabIndex = 35;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Hint:";
            // 
            // label6
            // 
            this.label6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label6.Location = new System.Drawing.Point(2, 15);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(292, 57);
            this.label6.TabIndex = 34;
            this.label6.Text = "Prepreg-side roughness (Rz) is determined by the oxide treatment used by the PCB " +
    "manufacturer to facilitate the laminate’s adhesion to the prepreg.  Consult your" +
    " PCB fabricator for prepreg-side Rz.";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.label5);
            this.groupBox3.Location = new System.Drawing.Point(226, 23);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox3.Size = new System.Drawing.Size(296, 60);
            this.groupBox3.TabIndex = 34;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Hint:";
            // 
            // label5
            // 
            this.label5.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label5.Location = new System.Drawing.Point(2, 15);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(292, 43);
            this.label5.TabIndex = 34;
            this.label5.Text = "Laminate-side roughness (Rz) is controlled by the laminate manufacturer.  Consult" +
    " your PCB fabricator or the laminate manufacturer for laminate-side Rz.";
            // 
            // label48
            // 
            this.label48.AutoSize = true;
            this.label48.Location = new System.Drawing.Point(57, 148);
            this.label48.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label48.Name = "label48";
            this.label48.Size = new System.Drawing.Size(53, 13);
            this.label48.TabIndex = 33;
            this.label48.Text = "Rz range:";
            // 
            // label49
            // 
            this.label49.AutoSize = true;
            this.label49.Location = new System.Drawing.Point(146, 102);
            this.label49.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label49.Name = "label49";
            this.label49.Size = new System.Drawing.Size(21, 13);
            this.label49.TabIndex = 32;
            this.label49.Text = "um";
            // 
            // label47
            // 
            this.label47.AutoSize = true;
            this.label47.Location = new System.Drawing.Point(146, 39);
            this.label47.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label47.Name = "label47";
            this.label47.Size = new System.Drawing.Size(21, 13);
            this.label47.TabIndex = 30;
            this.label47.Text = "um";
            // 
            // tbPrepreg_Side_Roughness
            // 
            this.tbPrepreg_Side_Roughness.Location = new System.Drawing.Point(111, 99);
            this.tbPrepreg_Side_Roughness.Margin = new System.Windows.Forms.Padding(2);
            this.tbPrepreg_Side_Roughness.Name = "tbPrepreg_Side_Roughness";
            this.tbPrepreg_Side_Roughness.Size = new System.Drawing.Size(32, 20);
            this.tbPrepreg_Side_Roughness.TabIndex = 3;
            // 
            // label22
            // 
            this.label22.AutoSize = true;
            this.label22.Location = new System.Drawing.Point(19, 102);
            this.label22.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label22.Name = "label22";
            this.label22.Size = new System.Drawing.Size(91, 13);
            this.label22.TabIndex = 24;
            this.label22.Text = "Prepreg-side (Rz):";
            this.label22.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // tbLaminate_Side_Rougness
            // 
            this.tbLaminate_Side_Rougness.Location = new System.Drawing.Point(111, 36);
            this.tbLaminate_Side_Rougness.Margin = new System.Windows.Forms.Padding(2);
            this.tbLaminate_Side_Rougness.Name = "tbLaminate_Side_Rougness";
            this.tbLaminate_Side_Rougness.Size = new System.Drawing.Size(32, 20);
            this.tbLaminate_Side_Rougness.TabIndex = 2;
            // 
            // label25
            // 
            this.label25.AutoSize = true;
            this.label25.Location = new System.Drawing.Point(13, 39);
            this.label25.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(97, 13);
            this.label25.TabIndex = 20;
            this.label25.Text = "Laminate-side (Rz):";
            this.label25.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(392, 525);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(2);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(82, 23);
            this.btnCancel.TabIndex = 4;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(478, 525);
            this.btnOK.Margin = new System.Windows.Forms.Padding(2);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 23);
            this.btnOK.TabIndex = 5;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // cbShowRoughness
            // 
            this.cbShowRoughness.AutoSize = true;
            this.cbShowRoughness.Location = new System.Drawing.Point(20, 11);
            this.cbShowRoughness.Margin = new System.Windows.Forms.Padding(2);
            this.cbShowRoughness.Name = "cbShowRoughness";
            this.cbShowRoughness.Size = new System.Drawing.Size(206, 17);
            this.cbShowRoughness.TabIndex = 0;
            this.cbShowRoughness.Text = "Show copper roughness in loss results";
            this.cbShowRoughness.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(16, 41);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox1.Size = new System.Drawing.Size(544, 137);
            this.groupBox1.TabIndex = 31;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Hints:";
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(5, 82);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(534, 47);
            this.label3.TabIndex = 35;
            this.label3.Text = resources.GetString("label3.Text");
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(5, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(534, 56);
            this.label2.TabIndex = 34;
            this.label2.Text = resources.GetString("label2.Text");
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Location = new System.Drawing.Point(226, 24);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox2.Size = new System.Drawing.Size(296, 71);
            this.groupBox2.TabIndex = 32;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Hint:";
            // 
            // label4
            // 
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Location = new System.Drawing.Point(2, 15);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(292, 54);
            this.label4.TabIndex = 35;
            this.label4.Text = "Copper foils on outer layers are typically provided by the PCB fabricator.  If yo" +
    "u’re interested in roughness effects on microstrip signals, consult your PCB fab" +
    "ricator for outer foil topography.";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.groupBox2);
            this.groupBox5.Controls.Add(this.label35);
            this.groupBox5.Controls.Add(this.tbOuterRoughness);
            this.groupBox5.Controls.Add(this.label45);
            this.groupBox5.Location = new System.Drawing.Point(16, 194);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(544, 118);
            this.groupBox5.TabIndex = 33;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Outer metal layers";
            // 
            // checkBoxUpdateSettings
            // 
            this.checkBoxUpdateSettings.AutoSize = true;
            this.checkBoxUpdateSettings.Location = new System.Drawing.Point(311, 11);
            this.checkBoxUpdateSettings.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxUpdateSettings.Name = "checkBoxUpdateSettings";
            this.checkBoxUpdateSettings.Size = new System.Drawing.Size(257, 17);
            this.checkBoxUpdateSettings.TabIndex = 0;
            this.checkBoxUpdateSettings.Text = "Use these values by default for all new stackups.";
            this.checkBoxUpdateSettings.UseVisualStyleBackColor = true;
            // 
            // RoughnessDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(579, 565);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.checkBoxUpdateSettings);
            this.Controls.Add(this.cbShowRoughness);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.groupBox10);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "RoughnessDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "  Copper Roughness Settings";
            this.groupBox10.ResumeLayout(false);
            this.groupBox10.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label45;
        private System.Windows.Forms.TextBox tbOuterRoughness;
        private System.Windows.Forms.Label label35;
        private System.Windows.Forms.GroupBox groupBox10;
        private System.Windows.Forms.Label label48;
        private System.Windows.Forms.Label label49;
        private System.Windows.Forms.Label label47;
        private System.Windows.Forms.TextBox tbPrepreg_Side_Roughness;
        private System.Windows.Forms.Label label22;
        private System.Windows.Forms.TextBox tbLaminate_Side_Rougness;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.CheckBox cbShowRoughness;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.CheckBox checkBoxUpdateSettings;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
    }
}