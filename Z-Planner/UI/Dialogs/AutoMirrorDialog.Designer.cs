namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class AutoMirrorDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AutoMirrorDialog));
            this.btnCancel = new System.Windows.Forms.Button();
            this.btnOK = new System.Windows.Forms.Button();
            this.groupBox12 = new System.Windows.Forms.GroupBox();
            this.label1 = new System.Windows.Forms.Label();
            this.label42 = new System.Windows.Forms.Label();
            this.cbAutoMirror = new System.Windows.Forms.CheckBox();
            this.cbCenterLine = new System.Windows.Forms.CheckBox();
            this.label2 = new System.Windows.Forms.Label();
            this.groupBox12.SuspendLayout();
            this.SuspendLayout();
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.Location = new System.Drawing.Point(422, 226);
            this.btnCancel.Margin = new System.Windows.Forms.Padding(2);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(82, 23);
            this.btnCancel.TabIndex = 15;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
            // 
            // btnOK
            // 
            this.btnOK.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOK.Location = new System.Drawing.Point(508, 226);
            this.btnOK.Margin = new System.Windows.Forms.Padding(2);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 23);
            this.btnOK.TabIndex = 16;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // groupBox12
            // 
            this.groupBox12.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox12.Controls.Add(this.label1);
            this.groupBox12.Controls.Add(this.label42);
            this.groupBox12.Location = new System.Drawing.Point(17, 88);
            this.groupBox12.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox12.Name = "groupBox12";
            this.groupBox12.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox12.Size = new System.Drawing.Size(573, 122);
            this.groupBox12.TabIndex = 12;
            this.groupBox12.TabStop = false;
            this.groupBox12.Text = "Hint";
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(4, 85);
            this.label1.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(565, 29);
            this.label1.TabIndex = 28;
            this.label1.Text = "The stackup center line—a red, dashed box—shows you the row of symmetry.  The sta" +
    "ckup should be the same above and below this dielectric row.";
            // 
            // label42
            // 
            this.label42.Location = new System.Drawing.Point(4, 15);
            this.label42.Margin = new System.Windows.Forms.Padding(2, 0, 2, 0);
            this.label42.Name = "label42";
            this.label42.Size = new System.Drawing.Size(565, 68);
            this.label42.TabIndex = 28;
            this.label42.Text = resources.GetString("label42.Text");
            // 
            // cbAutoMirror
            // 
            this.cbAutoMirror.AutoSize = true;
            this.cbAutoMirror.Location = new System.Drawing.Point(25, 48);
            this.cbAutoMirror.Margin = new System.Windows.Forms.Padding(2);
            this.cbAutoMirror.Name = "cbAutoMirror";
            this.cbAutoMirror.Size = new System.Drawing.Size(411, 17);
            this.cbAutoMirror.TabIndex = 13;
            this.cbAutoMirror.Text = "Auto-Mirror Stackup about the current center line. (Force Top > Bottom symmetry.)" +
    "";
            this.cbAutoMirror.UseVisualStyleBackColor = true;
            // 
            // cbCenterLine
            // 
            this.cbCenterLine.AutoSize = true;
            this.cbCenterLine.Location = new System.Drawing.Point(25, 19);
            this.cbCenterLine.Margin = new System.Windows.Forms.Padding(2);
            this.cbCenterLine.Name = "cbCenterLine";
            this.cbCenterLine.Size = new System.Drawing.Size(156, 17);
            this.cbCenterLine.TabIndex = 13;
            this.cbCenterLine.Text = "Turn on stackup center line";
            this.cbCenterLine.UseVisualStyleBackColor = true;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(41, 66);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(392, 23);
            this.label2.TabIndex = 17;
            this.label2.Text = "Future edits in Top half will be mirrored in Bottom half.";
            // 
            // AutoMirrorDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(607, 267);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.cbCenterLine);
            this.Controls.Add(this.cbAutoMirror);
            this.Controls.Add(this.groupBox12);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "AutoMirrorDialog";
            this.Text = "Auto-Mirror Settings";
            this.groupBox12.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.GroupBox groupBox12;
        private System.Windows.Forms.Label label42;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox cbAutoMirror;
        private System.Windows.Forms.CheckBox cbCenterLine;
        private System.Windows.Forms.Label label2;

    }
}