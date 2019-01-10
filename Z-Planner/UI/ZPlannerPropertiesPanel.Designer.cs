namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerPropertiesPanel
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
            this.propertyGrid = new System.Windows.Forms.PropertyGrid();
            this.collectionEditor = new ZZero.ZPlanner.UI.Editor.ZDataCollectionEditor();
            this.SuspendLayout();
            // 
            // propertyGrid
            // 
            this.propertyGrid.Dock = System.Windows.Forms.DockStyle.Fill;
            this.propertyGrid.HelpVisible = false;
            this.propertyGrid.LineColor = System.Drawing.SystemColors.ActiveBorder;
            this.propertyGrid.Location = new System.Drawing.Point(0, 0);
            this.propertyGrid.Name = "propertyGrid";
            this.propertyGrid.PropertySort = System.Windows.Forms.PropertySort.Categorized;
            this.propertyGrid.Size = new System.Drawing.Size(284, 262);
            this.propertyGrid.TabIndex = 0;
            this.propertyGrid.ToolbarVisible = false;
            this.propertyGrid.Visible = false;
            // 
            // collectionEditor
            // 
            this.collectionEditor.CollectionObject = null;
            this.collectionEditor.Dock = System.Windows.Forms.DockStyle.Fill;
            this.collectionEditor.Location = new System.Drawing.Point(0, 0);
            this.collectionEditor.MinimumSize = new System.Drawing.Size(200, 160);
            this.collectionEditor.Name = "collectionEditor";
            this.collectionEditor.Size = new System.Drawing.Size(284, 262);
            this.collectionEditor.TabIndex = 0;
            this.collectionEditor.Visible = false;
            // 
            // ZPlannerPropertiesPanel
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(284, 262);
            this.Controls.Add(this.propertyGrid);
            this.Controls.Add(this.collectionEditor);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.HideOnClose = true;
            this.Name = "ZPlannerPropertiesPanel";
            this.ShowInTaskbar = false;
            this.Text = "Row Properties";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PropertyGrid propertyGrid;
        private ZZero.ZPlanner.UI.Editor.ZDataCollectionEditor collectionEditor;
    }
}