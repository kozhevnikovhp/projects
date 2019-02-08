namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerMessagePanel
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
            this.massageBox = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // massageBox
            // 
            this.massageBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.massageBox.Location = new System.Drawing.Point(0, 0);
            this.massageBox.Multiline = true;
            this.massageBox.Name = "massageBox";
            this.massageBox.ReadOnly = true;
            this.massageBox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.massageBox.Size = new System.Drawing.Size(284, 262);
            this.massageBox.TabIndex = 0;
            this.massageBox.KeyDown += new System.Windows.Forms.KeyEventHandler(this.massageBox_KeyDown);
            // 
            // ZPlannerMessagePanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.massageBox);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.HideOnClose = true;
            this.Name = "ZPlannerMessagePanel";
            this.ShowInTaskbar = false;
            this.Text = "Output";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox massageBox;
    }
}