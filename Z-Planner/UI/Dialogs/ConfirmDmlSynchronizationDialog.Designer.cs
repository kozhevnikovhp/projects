namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class ConfirmDmlSynchronizationDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ConfirmDmlSynchronizationDialog));
            this.label1 = new System.Windows.Forms.Label();
            this.bOk = new System.Windows.Forms.Button();
            this.bIgnore = new System.Windows.Forms.Button();
            this.bCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(468, 23);
            this.label1.TabIndex = 0;
            this.label1.Text = "A new version of the Z-zero dielectric material library is available.";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // bOk
            // 
            this.bOk.Image = global::ZZero.ZPlanner.Properties.Resources.Sync_Zo_Lib_16;
            this.bOk.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.bOk.Location = new System.Drawing.Point(336, 44);
            this.bOk.Name = "bOk";
            this.bOk.Size = new System.Drawing.Size(143, 23);
            this.bOk.TabIndex = 1;
            this.bOk.Text = "   Update Z-zero library";
            this.bOk.UseVisualStyleBackColor = true;
            this.bOk.Click += new System.EventHandler(this.bOk_Click);
            // 
            // bIgnore
            // 
            this.bIgnore.Location = new System.Drawing.Point(12, 44);
            this.bIgnore.Name = "bIgnore";
            this.bIgnore.Size = new System.Drawing.Size(197, 23);
            this.bIgnore.TabIndex = 1;
            this.bIgnore.Text = "Do not update, and do not ask again";
            this.bIgnore.UseVisualStyleBackColor = true;
            this.bIgnore.Click += new System.EventHandler(this.bIgnore_Click);
            // 
            // bCancel
            // 
            this.bCancel.Location = new System.Drawing.Point(215, 44);
            this.bCancel.Name = "bCancel";
            this.bCancel.Size = new System.Drawing.Size(115, 23);
            this.bCancel.TabIndex = 1;
            this.bCancel.Text = "Do not update now";
            this.bCancel.UseVisualStyleBackColor = true;
            this.bCancel.Click += new System.EventHandler(this.bCancel_Click);
            // 
            // ConfirmDmlSynchronization
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(492, 86);
            this.Controls.Add(this.bCancel);
            this.Controls.Add(this.bIgnore);
            this.Controls.Add(this.bOk);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximumSize = new System.Drawing.Size(508, 124);
            this.MinimumSize = new System.Drawing.Size(508, 124);
            this.Name = "ConfirmDmlSynchronization";
            this.Text = "Z-zero Dielectric Material Library Update";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button bOk;
        private System.Windows.Forms.Button bIgnore;
        private System.Windows.Forms.Button bCancel;
    }
}