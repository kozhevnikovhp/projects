namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerContainerPanel
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ZPlannerContainerPanel));
            this.mainDockPanel = new WeifenLuo.WinFormsUI.Docking.DockPanel();
            this.statusMenu = new ZZero.ZPlanner.UI.Menu.StatusMenu();
            this.programMenu = new ZZero.ZPlanner.UI.Menu.ProgramMenu();
            this.SuspendLayout();
            // 
            // mainDockPanel
            // 
            this.mainDockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.mainDockPanel.DockBackColor = System.Drawing.SystemColors.Control;
            this.mainDockPanel.DockLeftPortion = 0.12D;
            this.mainDockPanel.DocumentStyle = WeifenLuo.WinFormsUI.Docking.DocumentStyle.DockingWindow;
            this.mainDockPanel.Location = new System.Drawing.Point(0, 158);
            this.mainDockPanel.Name = "mainDockPanel";
            this.mainDockPanel.Size = new System.Drawing.Size(1008, 484);
            this.mainDockPanel.TabIndex = 4;
            // 
            // statusMenu
            // 
            this.statusMenu.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.statusMenu.Location = new System.Drawing.Point(0, 615);
            this.statusMenu.Name = "statusMenu";
            this.statusMenu.Size = new System.Drawing.Size(1008, 27);
            this.statusMenu.TabIndex = 6;
            // 
            // programMenu
            // 
            this.programMenu.Dock = System.Windows.Forms.DockStyle.Top;
            this.programMenu.Location = new System.Drawing.Point(0, 0);
            this.programMenu.Name = "programMenu";
            this.programMenu.Size = new System.Drawing.Size(1008, 155);
            this.programMenu.TabIndex = 1;
            // 
            // ZPlannerContainerPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(1008, 642);
            this.Controls.Add(this.mainDockPanel);
            this.Controls.Add(this.statusMenu);
            this.Controls.Add(this.programMenu);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.IsMdiContainer = true;
            this.MinimumSize = new System.Drawing.Size(1200, 800);
            this.Name = "ZPlannerContainerPanel";
            this.Text = "  Z-planner";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ZPlannerContainerPanel_FormClosing);
            this.Load += new System.EventHandler(this.ZPlannerContainerPanel_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private WeifenLuo.WinFormsUI.Docking.DockPanel mainDockPanel;
        private ZZero.ZPlanner.UI.Menu.ProgramMenu programMenu;
        private ZZero.ZPlanner.UI.Menu.StatusMenu statusMenu;
    }
}