namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerProjectPanel
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
            this.stackupsTreeView = new System.Windows.Forms.TreeView();
            this.SuspendLayout();
            // 
            // stackupsTreeView
            // 
            this.stackupsTreeView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.stackupsTreeView.FullRowSelect = true;
            this.stackupsTreeView.Location = new System.Drawing.Point(0, 0);
            this.stackupsTreeView.Name = "stackupsTreeView";
            this.stackupsTreeView.Size = new System.Drawing.Size(284, 458);
            this.stackupsTreeView.TabIndex = 0;
            // 
            // ZPlannerProjectPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(284, 458);
            this.Controls.Add(this.stackupsTreeView);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.HideOnClose = true;
            this.Name = "ZPlannerProjectPanel";
            this.ShowInTaskbar = false;
            this.Text = "Project";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TreeView stackupsTreeView;
    }
}