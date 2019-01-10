namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class AboutDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AboutDialog));
            this.pictLogo = new System.Windows.Forms.PictureBox();
            this.panelLogo = new System.Windows.Forms.Panel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.tbTM = new System.Windows.Forms.TextBox();
            this.tbBuild = new System.Windows.Forms.TextBox();
            this.tbVersion = new System.Windows.Forms.TextBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.tbTMHL = new System.Windows.Forms.TextBox();
            this.tbFSBuild = new System.Windows.Forms.TextBox();
            this.tbFSVersion = new System.Windows.Forms.TextBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.textBox8 = new System.Windows.Forms.TextBox();
            this.tbProgram = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.btnClose = new System.Windows.Forms.Button();
            this.tbCopyright = new System.Windows.Forms.TextBox();
            this.panel4 = new System.Windows.Forms.Panel();
            this.btBrowse = new System.Windows.Forms.Button();
            this.tbEditedLicensePath = new System.Windows.Forms.TextBox();
            this.textBox7 = new System.Windows.Forms.TextBox();
            this.tbLicensePath = new System.Windows.Forms.TextBox();
            this.textBox5 = new System.Windows.Forms.TextBox();
            this.tbExpiration = new System.Windows.Forms.TextBox();
            this.textBox6 = new System.Windows.Forms.TextBox();
            this.tbStatus = new System.Windows.Forms.TextBox();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.panelFeature = new System.Windows.Forms.Panel();
            this.rb6L = new System.Windows.Forms.RadioButton();
            this.rb12L = new System.Windows.Forms.RadioButton();
            this.rbUL = new System.Windows.Forms.RadioButton();
            ((System.ComponentModel.ISupportInitialize)(this.pictLogo)).BeginInit();
            this.panelLogo.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panelFeature.SuspendLayout();
            this.SuspendLayout();
            // 
            // pictLogo
            // 
            this.pictLogo.BackColor = System.Drawing.Color.White;
            this.pictLogo.Image = ((System.Drawing.Image)(resources.GetObject("pictLogo.Image")));
            this.pictLogo.Location = new System.Drawing.Point(11, 7);
            this.pictLogo.Name = "pictLogo";
            this.pictLogo.Size = new System.Drawing.Size(469, 145);
            this.pictLogo.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictLogo.TabIndex = 0;
            this.pictLogo.TabStop = false;
            // 
            // panelLogo
            // 
            this.panelLogo.BackColor = System.Drawing.Color.White;
            this.panelLogo.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelLogo.Controls.Add(this.pictLogo);
            this.panelLogo.Location = new System.Drawing.Point(362, 12);
            this.panelLogo.Name = "panelLogo";
            this.panelLogo.Size = new System.Drawing.Size(492, 165);
            this.panelLogo.TabIndex = 1;
            this.panelLogo.Paint += new System.Windows.Forms.PaintEventHandler(this.panelLogo_Paint);
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.tbTM);
            this.panel1.Controls.Add(this.tbBuild);
            this.panel1.Controls.Add(this.tbVersion);
            this.panel1.Location = new System.Drawing.Point(17, 213);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(326, 142);
            this.panel1.TabIndex = 17;
            // 
            // tbTM
            // 
            this.tbTM.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbTM.Location = new System.Drawing.Point(3, 101);
            this.tbTM.Multiline = true;
            this.tbTM.Name = "tbTM";
            this.tbTM.ReadOnly = true;
            this.tbTM.Size = new System.Drawing.Size(318, 34);
            this.tbTM.TabIndex = 25;
            this.tbTM.TabStop = false;
            this.tbTM.Text = "Z-zero® and Z-planner® are registered trademarks of Z-zero, LP.";
            // 
            // tbBuild
            // 
            this.tbBuild.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbBuild.Location = new System.Drawing.Point(93, 42);
            this.tbBuild.Multiline = true;
            this.tbBuild.Name = "tbBuild";
            this.tbBuild.ReadOnly = true;
            this.tbBuild.Size = new System.Drawing.Size(127, 40);
            this.tbBuild.TabIndex = 1;
            this.tbBuild.TabStop = false;
            this.tbBuild.Text = "[build xxx]\r\n64 bit, Month DD, YYYY";
            this.tbBuild.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // tbVersion
            // 
            this.tbVersion.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbVersion.Location = new System.Drawing.Point(16, 8);
            this.tbVersion.Name = "tbVersion";
            this.tbVersion.ReadOnly = true;
            this.tbVersion.Size = new System.Drawing.Size(295, 12);
            this.tbVersion.TabIndex = 0;
            this.tbVersion.TabStop = false;
            this.tbVersion.Text = "Z-planner";
            this.tbVersion.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel2.Controls.Add(this.tbTMHL);
            this.panel2.Controls.Add(this.tbFSBuild);
            this.panel2.Controls.Add(this.tbFSVersion);
            this.panel2.Location = new System.Drawing.Point(17, 369);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(326, 134);
            this.panel2.TabIndex = 18;
            // 
            // tbTMHL
            // 
            this.tbTMHL.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbTMHL.Location = new System.Drawing.Point(3, 89);
            this.tbTMHL.Multiline = true;
            this.tbTMHL.Name = "tbTMHL";
            this.tbTMHL.ReadOnly = true;
            this.tbTMHL.Size = new System.Drawing.Size(308, 34);
            this.tbTMHL.TabIndex = 26;
            this.tbTMHL.TabStop = false;
            this.tbTMHL.Text = "HyperLynx® is a registered trademark of Mentor Graphics Corporation";
            // 
            // tbFSBuild
            // 
            this.tbFSBuild.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbFSBuild.Location = new System.Drawing.Point(93, 35);
            this.tbFSBuild.Multiline = true;
            this.tbFSBuild.Name = "tbFSBuild";
            this.tbFSBuild.ReadOnly = true;
            this.tbFSBuild.Size = new System.Drawing.Size(127, 40);
            this.tbFSBuild.TabIndex = 3;
            this.tbFSBuild.TabStop = false;
            this.tbFSBuild.Text = "[build xxx]\r\n64 bit, Month DD, YYYY";
            this.tbFSBuild.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // tbFSVersion
            // 
            this.tbFSVersion.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbFSVersion.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbFSVersion.Location = new System.Drawing.Point(44, 6);
            this.tbFSVersion.Name = "tbFSVersion";
            this.tbFSVersion.ReadOnly = true;
            this.tbFSVersion.Size = new System.Drawing.Size(225, 12);
            this.tbFSVersion.TabIndex = 2;
            this.tbFSVersion.TabStop = false;
            this.tbFSVersion.Text = "FS";
            this.tbFSVersion.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel3.Controls.Add(this.textBox8);
            this.panel3.Controls.Add(this.tbProgram);
            this.panel3.Location = new System.Drawing.Point(17, 517);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(837, 74);
            this.panel3.TabIndex = 18;
            // 
            // textBox8
            // 
            this.textBox8.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox8.Location = new System.Drawing.Point(3, 10);
            this.textBox8.Multiline = true;
            this.textBox8.Name = "textBox8";
            this.textBox8.ReadOnly = true;
            this.textBox8.Size = new System.Drawing.Size(128, 19);
            this.textBox8.TabIndex = 30;
            this.textBox8.TabStop = false;
            this.textBox8.Text = "Program Location:";
            // 
            // tbProgram
            // 
            this.tbProgram.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbProgram.Location = new System.Drawing.Point(3, 35);
            this.tbProgram.Multiline = true;
            this.tbProgram.Name = "tbProgram";
            this.tbProgram.ReadOnly = true;
            this.tbProgram.Size = new System.Drawing.Size(808, 25);
            this.tbProgram.TabIndex = 4;
            this.tbProgram.TabStop = false;
            this.tbProgram.Text = "program location";
            // 
            // textBox1
            // 
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Location = new System.Drawing.Point(17, 186);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(326, 19);
            this.textBox1.TabIndex = 20;
            this.textBox1.TabStop = false;
            this.textBox1.Text = "About Z-planner";
            // 
            // textBox2
            // 
            this.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox2.Location = new System.Drawing.Point(362, 186);
            this.textBox2.Multiline = true;
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(100, 19);
            this.textBox2.TabIndex = 21;
            this.textBox2.TabStop = false;
            this.textBox2.Text = "Licensing";
            // 
            // btnClose
            // 
            this.btnClose.Location = new System.Drawing.Point(745, 662);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(109, 28);
            this.btnClose.TabIndex = 22;
            this.btnClose.Text = "Close";
            this.btnClose.UseVisualStyleBackColor = true;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // tbCopyright
            // 
            this.tbCopyright.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCopyright.Location = new System.Drawing.Point(17, 603);
            this.tbCopyright.Multiline = true;
            this.tbCopyright.Name = "tbCopyright";
            this.tbCopyright.ReadOnly = true;
            this.tbCopyright.Size = new System.Drawing.Size(837, 53);
            this.tbCopyright.TabIndex = 23;
            this.tbCopyright.TabStop = false;
            this.tbCopyright.Text = resources.GetString("tbCopyright.Text");
            // 
            // panel4
            // 
            this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel4.Controls.Add(this.btBrowse);
            this.panel4.Controls.Add(this.tbEditedLicensePath);
            this.panel4.Controls.Add(this.textBox7);
            this.panel4.Controls.Add(this.tbLicensePath);
            this.panel4.Controls.Add(this.textBox5);
            this.panel4.Controls.Add(this.tbExpiration);
            this.panel4.Controls.Add(this.textBox6);
            this.panel4.Controls.Add(this.tbStatus);
            this.panel4.Controls.Add(this.textBox4);
            this.panel4.Controls.Add(this.textBox3);
            this.panel4.Controls.Add(this.panelFeature);
            this.panel4.Location = new System.Drawing.Point(362, 213);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(492, 290);
            this.panel4.TabIndex = 19;
            // 
            // btBrowse
            // 
            this.btBrowse.Enabled = false;
            this.btBrowse.Location = new System.Drawing.Point(357, 245);
            this.btBrowse.Name = "btBrowse";
            this.btBrowse.Size = new System.Drawing.Size(109, 28);
            this.btBrowse.TabIndex = 31;
            this.btBrowse.Text = "Browse";
            this.btBrowse.UseVisualStyleBackColor = true;
            this.btBrowse.Visible = false;
            this.btBrowse.Click += new System.EventHandler(this.btBrowse_Click);
            // 
            // tbEditedLicensePath
            // 
            this.tbEditedLicensePath.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbEditedLicensePath.Enabled = false;
            this.tbEditedLicensePath.Location = new System.Drawing.Point(17, 249);
            this.tbEditedLicensePath.Multiline = true;
            this.tbEditedLicensePath.Name = "tbEditedLicensePath";
            this.tbEditedLicensePath.Size = new System.Drawing.Size(311, 22);
            this.tbEditedLicensePath.TabIndex = 30;
            this.tbEditedLicensePath.Text = "license path";
            this.tbEditedLicensePath.Visible = false;
            // 
            // textBox7
            // 
            this.textBox7.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox7.Location = new System.Drawing.Point(17, 224);
            this.textBox7.Multiline = true;
            this.textBox7.Name = "textBox7";
            this.textBox7.ReadOnly = true;
            this.textBox7.Size = new System.Drawing.Size(157, 19);
            this.textBox7.TabIndex = 29;
            this.textBox7.TabStop = false;
            this.textBox7.Text = "Edit license path:";
            this.textBox7.Visible = false;
            // 
            // tbLicensePath
            // 
            this.tbLicensePath.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbLicensePath.Location = new System.Drawing.Point(17, 180);
            this.tbLicensePath.Multiline = true;
            this.tbLicensePath.Name = "tbLicensePath";
            this.tbLicensePath.ReadOnly = true;
            this.tbLicensePath.Size = new System.Drawing.Size(445, 22);
            this.tbLicensePath.TabIndex = 28;
            this.tbLicensePath.TabStop = false;
            this.tbLicensePath.Text = "license path";
            // 
            // textBox5
            // 
            this.textBox5.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox5.Location = new System.Drawing.Point(17, 155);
            this.textBox5.Multiline = true;
            this.textBox5.Name = "textBox5";
            this.textBox5.ReadOnly = true;
            this.textBox5.Size = new System.Drawing.Size(157, 19);
            this.textBox5.TabIndex = 27;
            this.textBox5.TabStop = false;
            this.textBox5.Text = "License path (.Lic):";
            // 
            // tbExpiration
            // 
            this.tbExpiration.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbExpiration.Location = new System.Drawing.Point(362, 39);
            this.tbExpiration.Multiline = true;
            this.tbExpiration.Name = "tbExpiration";
            this.tbExpiration.ReadOnly = true;
            this.tbExpiration.Size = new System.Drawing.Size(100, 19);
            this.tbExpiration.TabIndex = 26;
            this.tbExpiration.TabStop = false;
            this.tbExpiration.Text = "Expiration date";
            // 
            // textBox6
            // 
            this.textBox6.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox6.Location = new System.Drawing.Point(362, 6);
            this.textBox6.Multiline = true;
            this.textBox6.Name = "textBox6";
            this.textBox6.ReadOnly = true;
            this.textBox6.Size = new System.Drawing.Size(100, 19);
            this.textBox6.TabIndex = 25;
            this.textBox6.TabStop = false;
            this.textBox6.Text = "Expiration:";
            // 
            // tbStatus
            // 
            this.tbStatus.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbStatus.Location = new System.Drawing.Point(188, 39);
            this.tbStatus.Multiline = true;
            this.tbStatus.Name = "tbStatus";
            this.tbStatus.ReadOnly = true;
            this.tbStatus.Size = new System.Drawing.Size(168, 19);
            this.tbStatus.TabIndex = 24;
            this.tbStatus.TabStop = false;
            this.tbStatus.Text = "Status";
            // 
            // textBox4
            // 
            this.textBox4.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox4.Location = new System.Drawing.Point(188, 6);
            this.textBox4.Multiline = true;
            this.textBox4.Name = "textBox4";
            this.textBox4.ReadOnly = true;
            this.textBox4.Size = new System.Drawing.Size(100, 19);
            this.textBox4.TabIndex = 23;
            this.textBox4.TabStop = false;
            this.textBox4.Text = "Status:";
            // 
            // textBox3
            // 
            this.textBox3.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox3.Location = new System.Drawing.Point(17, 6);
            this.textBox3.Multiline = true;
            this.textBox3.Name = "textBox3";
            this.textBox3.ReadOnly = true;
            this.textBox3.Size = new System.Drawing.Size(100, 19);
            this.textBox3.TabIndex = 22;
            this.textBox3.TabStop = false;
            this.textBox3.Text = "Feature:";
            // 
            // panelFeature
            // 
            this.panelFeature.Controls.Add(this.rb6L);
            this.panelFeature.Controls.Add(this.rb12L);
            this.panelFeature.Controls.Add(this.rbUL);
            this.panelFeature.Location = new System.Drawing.Point(9, 31);
            this.panelFeature.Name = "panelFeature";
            this.panelFeature.Size = new System.Drawing.Size(157, 110);
            this.panelFeature.TabIndex = 0;
            // 
            // rb6L
            // 
            this.rb6L.AutoSize = true;
            this.rb6L.Enabled = false;
            this.rb6L.Location = new System.Drawing.Point(6, 66);
            this.rb6L.Name = "rb6L";
            this.rb6L.Size = new System.Drawing.Size(85, 17);
            this.rb6L.TabIndex = 2;
            this.rb6L.Text = "Z-planner 6L";
            this.rb6L.UseVisualStyleBackColor = true;
            // 
            // rb12L
            // 
            this.rb12L.AutoSize = true;
            this.rb12L.Enabled = false;
            this.rb12L.Location = new System.Drawing.Point(6, 36);
            this.rb12L.Name = "rb12L";
            this.rb12L.Size = new System.Drawing.Size(91, 17);
            this.rb12L.TabIndex = 1;
            this.rb12L.Text = "Z-planner 12L";
            this.rb12L.UseVisualStyleBackColor = true;
            // 
            // rbUL
            // 
            this.rbUL.AutoSize = true;
            this.rbUL.Checked = true;
            this.rbUL.Location = new System.Drawing.Point(6, 6);
            this.rbUL.Name = "rbUL";
            this.rbUL.Size = new System.Drawing.Size(87, 17);
            this.rbUL.TabIndex = 0;
            this.rbUL.TabStop = true;
            this.rbUL.Text = "Z-planner UL";
            this.rbUL.UseVisualStyleBackColor = true;
            // 
            // AboutDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.ClientSize = new System.Drawing.Size(866, 702);
            this.Controls.Add(this.tbCopyright);
            this.Controls.Add(this.btnClose);
            this.Controls.Add(this.textBox2);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.panel4);
            this.Controls.Add(this.panel3);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.panelLogo);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AboutDialog";
            this.ShowInTaskbar = false;
            this.Text = "About Z-planner / Licensing";
            ((System.ComponentModel.ISupportInitialize)(this.pictLogo)).EndInit();
            this.panelLogo.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.panel4.ResumeLayout(false);
            this.panel4.PerformLayout();
            this.panelFeature.ResumeLayout(false);
            this.panelFeature.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictLogo;
        private System.Windows.Forms.Panel panelLogo;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.TextBox tbCopyright;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.TextBox tbBuild;
        private System.Windows.Forms.TextBox tbVersion;
        private System.Windows.Forms.TextBox tbFSBuild;
        private System.Windows.Forms.TextBox tbFSVersion;
        private System.Windows.Forms.TextBox tbProgram;
        private System.Windows.Forms.Panel panelFeature;
        private System.Windows.Forms.RadioButton rb6L;
        private System.Windows.Forms.RadioButton rb12L;
        private System.Windows.Forms.RadioButton rbUL;
        private System.Windows.Forms.TextBox tbExpiration;
        private System.Windows.Forms.TextBox textBox6;
        private System.Windows.Forms.TextBox tbStatus;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.TextBox tbLicensePath;
        private System.Windows.Forms.TextBox textBox5;
        private System.Windows.Forms.Button btBrowse;
        private System.Windows.Forms.TextBox tbEditedLicensePath;
        private System.Windows.Forms.TextBox textBox7;
        private System.Windows.Forms.TextBox textBox8;
        private System.Windows.Forms.TextBox tbTM;
        private System.Windows.Forms.TextBox tbTMHL;
    }
}