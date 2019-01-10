namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class GlassPitchDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(GlassPitchDialog));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.cbShowGlassPitch = new System.Windows.Forms.CheckBox();
            this.cbShowGws = new System.Windows.Forms.CheckBox();
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Location = new System.Drawing.Point(11, 56);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox1.Size = new System.Drawing.Size(542, 175);
            this.groupBox1.TabIndex = 33;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Hints:";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(5, 122);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(534, 47);
            this.label1.TabIndex = 35;
            this.label1.Text = resources.GetString("label1.Text");
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(5, 82);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(534, 36);
            this.label3.TabIndex = 35;
            this.label3.Text = "Enter the frequency for your differential channel in GHz (i.e., 2 x the bit rate " +
    "in Gbps) and Z-planner will calculate the associated bit rate (Gbps), unit inter" +
    "val (ps), and Skew Tolerance (ps).";
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(5, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(534, 56);
            this.label2.TabIndex = 34;
            this.label2.Text = resources.GetString("label2.Text");
            // 
            // cbShowGlassPitch
            // 
            this.cbShowGlassPitch.AutoSize = true;
            this.cbShowGlassPitch.Location = new System.Drawing.Point(15, 14);
            this.cbShowGlassPitch.Margin = new System.Windows.Forms.Padding(2);
            this.cbShowGlassPitch.Name = "cbShowGlassPitch";
            this.cbShowGlassPitch.Size = new System.Drawing.Size(160, 17);
            this.cbShowGlassPitch.TabIndex = 32;
            this.cbShowGlassPitch.Text = "Show Differential-Pitch fields";
            this.cbShowGlassPitch.UseVisualStyleBackColor = true;
            // 
            // cbShowGws
            // 
            this.cbShowGws.AutoSize = true;
            this.cbShowGws.Location = new System.Drawing.Point(15, 35);
            this.cbShowGws.Margin = new System.Windows.Forms.Padding(2);
            this.cbShowGws.Name = "cbShowGws";
            this.cbShowGws.Size = new System.Drawing.Size(187, 17);
            this.cbShowGws.TabIndex = 32;
            this.cbShowGws.Text = "Show Frequency-dependent fields";
            this.cbShowGws.UseVisualStyleBackColor = true;
            // 
            // btnCancel
            // 
            this.btnCancel.Location = new System.Drawing.Point(385, 242);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(2);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(82, 23);
            this.btnCancel.TabIndex = 34;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Location = new System.Drawing.Point(471, 242);
            this.btnOK.Margin = new System.Windows.Forms.Padding(2);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 23);
            this.btnOK.TabIndex = 35;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // GlassPitchDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(564, 276);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.cbShowGws);
            this.Controls.Add(this.cbShowGlassPitch);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximumSize = new System.Drawing.Size(580, 314);
            this.MinimumSize = new System.Drawing.Size(580, 314);
            this.Name = "GlassPitchDialog";
            this.Text = "Glass Pitch Settings";
            this.groupBox1.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.CheckBox cbShowGlassPitch;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox cbShowGws;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
    }
}