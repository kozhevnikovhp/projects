namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerStackupPanel
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
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            this.rowContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.addRowBeforeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addRowAfterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.moveRowUpToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.moveRowDownToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.swapRowsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeRowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.separator1 = new System.Windows.Forms.ToolStripSeparator();
            this.duplicateRowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.copyRowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.cutRowToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pasteRowBeforeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pasteRowInsteadToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.pasteRowAfterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.separator2 = new System.Windows.Forms.ToolStripSeparator();
            this.pasteMaterialToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.clearMaterialToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.columnContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.insertHiddenColumnBeforeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.insertHiddenColumnAfterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.showColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.hideColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.addColumnBeforeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.addColumnAfterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.removeColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.insertExistingColumnBeforeToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.insertExistingColumnAfterToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.moveColumnToTheLeftToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.moveColumnToTheRightToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.minimizeColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.propertiesOfColumnToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.viaspanContextMenu = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.viaspanEditToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.stackupGridView = new ZZero.ZPlanner.UI.Grid.ZDataGridView();
            this.printPanel = new System.Windows.Forms.Panel();
            this.clearAllMaterialsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.rowContextMenu.SuspendLayout();
            this.columnContextMenu.SuspendLayout();
            this.viaspanContextMenu.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.stackupGridView)).BeginInit();
            this.printPanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // rowContextMenu
            // 
            this.rowContextMenu.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.rowContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.addRowBeforeToolStripMenuItem,
            this.addRowAfterToolStripMenuItem,
            this.moveRowUpToolStripMenuItem,
            this.moveRowDownToolStripMenuItem,
            this.swapRowsToolStripMenuItem,
            this.removeRowToolStripMenuItem,
            this.separator1,
            this.duplicateRowToolStripMenuItem,
            this.copyRowToolStripMenuItem,
            this.cutRowToolStripMenuItem,
            this.pasteRowBeforeToolStripMenuItem,
            this.pasteRowInsteadToolStripMenuItem,
            this.pasteRowAfterToolStripMenuItem,
            this.separator2,
            this.pasteMaterialToolStripMenuItem,
            this.clearMaterialToolStripMenuItem,
            this.clearAllMaterialsToolStripMenuItem});
            this.rowContextMenu.Name = "rowContextMenu";
            this.rowContextMenu.Size = new System.Drawing.Size(274, 368);
            // 
            // addRowBeforeToolStripMenuItem
            // 
            this.addRowBeforeToolStripMenuItem.Name = "addRowBeforeToolStripMenuItem";
            this.addRowBeforeToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.addRowBeforeToolStripMenuItem.Text = "Add Row Before";
            // 
            // addRowAfterToolStripMenuItem
            // 
            this.addRowAfterToolStripMenuItem.Name = "addRowAfterToolStripMenuItem";
            this.addRowAfterToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.addRowAfterToolStripMenuItem.Text = "Add Row After";
            // 
            // moveRowUpToolStripMenuItem
            // 
            this.moveRowUpToolStripMenuItem.Name = "moveRowUpToolStripMenuItem";
            this.moveRowUpToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.moveRowUpToolStripMenuItem.Text = "Move Row Up";
            // 
            // moveRowDownToolStripMenuItem
            // 
            this.moveRowDownToolStripMenuItem.Name = "moveRowDownToolStripMenuItem";
            this.moveRowDownToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.moveRowDownToolStripMenuItem.Text = "Move Row Down";
            // 
            // swapRowsToolStripMenuItem
            // 
            this.swapRowsToolStripMenuItem.Name = "swapRowsToolStripMenuItem";
            this.swapRowsToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.swapRowsToolStripMenuItem.Text = "Swap Rows";
            // 
            // removeRowToolStripMenuItem
            // 
            this.removeRowToolStripMenuItem.Name = "removeRowToolStripMenuItem";
            this.removeRowToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.removeRowToolStripMenuItem.Text = "Delete Row";
            // 
            // separator1
            // 
            this.separator1.Name = "separator1";
            this.separator1.Size = new System.Drawing.Size(270, 6);
            // 
            // duplicateRowToolStripMenuItem
            // 
            this.duplicateRowToolStripMenuItem.Name = "duplicateRowToolStripMenuItem";
            this.duplicateRowToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.duplicateRowToolStripMenuItem.Text = "Duplicate Row";
            // 
            // copyRowToolStripMenuItem
            // 
            this.copyRowToolStripMenuItem.Name = "copyRowToolStripMenuItem";
            this.copyRowToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.copyRowToolStripMenuItem.Text = "Copy Row";
            // 
            // cutRowToolStripMenuItem
            // 
            this.cutRowToolStripMenuItem.Name = "cutRowToolStripMenuItem";
            this.cutRowToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.cutRowToolStripMenuItem.Text = "Cut Row";
            // 
            // pasteRowBeforeToolStripMenuItem
            // 
            this.pasteRowBeforeToolStripMenuItem.Name = "pasteRowBeforeToolStripMenuItem";
            this.pasteRowBeforeToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.pasteRowBeforeToolStripMenuItem.Text = "Paste Row Before";
            // 
            // pasteRowInsteadToolStripMenuItem
            // 
            this.pasteRowInsteadToolStripMenuItem.Name = "pasteRowInsteadToolStripMenuItem";
            this.pasteRowInsteadToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.pasteRowInsteadToolStripMenuItem.Text = "Paste Row (Replace)";
            // 
            // pasteRowAfterToolStripMenuItem
            // 
            this.pasteRowAfterToolStripMenuItem.Name = "pasteRowAfterToolStripMenuItem";
            this.pasteRowAfterToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.pasteRowAfterToolStripMenuItem.Text = "Paste Row After";
            // 
            // separator2
            // 
            this.separator2.Name = "separator2";
            this.separator2.Size = new System.Drawing.Size(270, 6);
            // 
            // pasteMaterialToolStripMenuItem
            // 
            this.pasteMaterialToolStripMenuItem.Name = "pasteMaterialToolStripMenuItem";
            this.pasteMaterialToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.pasteMaterialToolStripMenuItem.Text = "Paste Material";
            // 
            // clearMaterialToolStripMenuItem
            // 
            this.clearMaterialToolStripMenuItem.Name = "clearMaterialToolStripMenuItem";
            this.clearMaterialToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.clearMaterialToolStripMenuItem.Text = "What If Analysis on Dielectric Row";
            // 
            // columnContextMenu
            // 
            this.columnContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.insertHiddenColumnBeforeToolStripMenuItem,
            this.insertHiddenColumnAfterToolStripMenuItem,
            this.showColumnToolStripMenuItem,
            this.hideColumnToolStripMenuItem,
            this.toolStripSeparator3,
            this.addColumnBeforeToolStripMenuItem,
            this.addColumnAfterToolStripMenuItem,
            this.removeColumnToolStripMenuItem,
            this.toolStripSeparator4,
            this.insertExistingColumnBeforeToolStripMenuItem,
            this.insertExistingColumnAfterToolStripMenuItem,
            this.moveColumnToTheLeftToolStripMenuItem,
            this.moveColumnToTheRightToolStripMenuItem,
            this.toolStripSeparator1,
            this.minimizeColumnToolStripMenuItem,
            this.toolStripSeparator2,
            this.propertiesOfColumnToolStripMenuItem});
            this.columnContextMenu.Name = "columnContextMenu";
            this.columnContextMenu.Size = new System.Drawing.Size(230, 314);
            // 
            // insertHiddenColumnBeforeToolStripMenuItem
            // 
            this.insertHiddenColumnBeforeToolStripMenuItem.Name = "insertHiddenColumnBeforeToolStripMenuItem";
            this.insertHiddenColumnBeforeToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.insertHiddenColumnBeforeToolStripMenuItem.Text = "Insert Hidden Column Before";
            // 
            // insertHiddenColumnAfterToolStripMenuItem
            // 
            this.insertHiddenColumnAfterToolStripMenuItem.Name = "insertHiddenColumnAfterToolStripMenuItem";
            this.insertHiddenColumnAfterToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.insertHiddenColumnAfterToolStripMenuItem.Text = "Insert Hidden Column After";
            // 
            // showColumnToolStripMenuItem
            // 
            this.showColumnToolStripMenuItem.Name = "showColumnToolStripMenuItem";
            this.showColumnToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.showColumnToolStripMenuItem.Text = "Show All Hidden Columns";
            // 
            // hideColumnToolStripMenuItem
            // 
            this.hideColumnToolStripMenuItem.Name = "hideColumnToolStripMenuItem";
            this.hideColumnToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.hideColumnToolStripMenuItem.Text = "Hide Column";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(226, 6);
            // 
            // addColumnBeforeToolStripMenuItem
            // 
            this.addColumnBeforeToolStripMenuItem.Name = "addColumnBeforeToolStripMenuItem";
            this.addColumnBeforeToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.addColumnBeforeToolStripMenuItem.Text = "Add New Column Before";
            // 
            // addColumnAfterToolStripMenuItem
            // 
            this.addColumnAfterToolStripMenuItem.Name = "addColumnAfterToolStripMenuItem";
            this.addColumnAfterToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.addColumnAfterToolStripMenuItem.Text = "Add New Column After";
            // 
            // removeColumnToolStripMenuItem
            // 
            this.removeColumnToolStripMenuItem.Name = "removeColumnToolStripMenuItem";
            this.removeColumnToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.removeColumnToolStripMenuItem.Text = "Delete Column";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(226, 6);
            // 
            // insertExistingColumnBeforeToolStripMenuItem
            // 
            this.insertExistingColumnBeforeToolStripMenuItem.Name = "insertExistingColumnBeforeToolStripMenuItem";
            this.insertExistingColumnBeforeToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.insertExistingColumnBeforeToolStripMenuItem.Text = "Insert Existing Column Before";
            // 
            // insertExistingColumnAfterToolStripMenuItem
            // 
            this.insertExistingColumnAfterToolStripMenuItem.Name = "insertExistingColumnAfterToolStripMenuItem";
            this.insertExistingColumnAfterToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.insertExistingColumnAfterToolStripMenuItem.Text = "Insert Existing Column After";
            // 
            // moveColumnToTheLeftToolStripMenuItem
            // 
            this.moveColumnToTheLeftToolStripMenuItem.Name = "moveColumnToTheLeftToolStripMenuItem";
            this.moveColumnToTheLeftToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.moveColumnToTheLeftToolStripMenuItem.Text = "Move Column to the Left";
            // 
            // moveColumnToTheRightToolStripMenuItem
            // 
            this.moveColumnToTheRightToolStripMenuItem.Name = "moveColumnToTheRightToolStripMenuItem";
            this.moveColumnToTheRightToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.moveColumnToTheRightToolStripMenuItem.Text = "Move Column to the Right";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(226, 6);
            // 
            // minimizeColumnToolStripMenuItem
            // 
            this.minimizeColumnToolStripMenuItem.CheckOnClick = true;
            this.minimizeColumnToolStripMenuItem.Name = "minimizeColumnToolStripMenuItem";
            this.minimizeColumnToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.minimizeColumnToolStripMenuItem.Text = "Minimize Column";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(226, 6);
            // 
            // propertiesOfColumnToolStripMenuItem
            // 
            this.propertiesOfColumnToolStripMenuItem.Name = "propertiesOfColumnToolStripMenuItem";
            this.propertiesOfColumnToolStripMenuItem.Size = new System.Drawing.Size(229, 22);
            this.propertiesOfColumnToolStripMenuItem.Text = "Properties";
            // 
            // viaspanContextMenu
            // 
            this.viaspanContextMenu.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.viaspanEditToolStripMenuItem});
            this.viaspanContextMenu.Name = "viaspanContextMenu";
            this.viaspanContextMenu.Size = new System.Drawing.Size(148, 26);
            // 
            // viaspanEditToolStripMenuItem
            // 
            this.viaspanEditToolStripMenuItem.Name = "viaspanEditToolStripMenuItem";
            this.viaspanEditToolStripMenuItem.Size = new System.Drawing.Size(147, 22);
            this.viaspanEditToolStripMenuItem.Text = "Edit Via Spans";
            // 
            // stackupGridView
            // 
            this.stackupGridView.AllowDrawCenterLine = true;
            this.stackupGridView.AllowUserToAddRows = false;
            this.stackupGridView.AllowUserToDeleteRows = false;
            this.stackupGridView.AllowUserToOrderColumns = true;
            this.stackupGridView.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.stackupGridView.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.EnableWithoutHeaderText;
            this.stackupGridView.ColumnHeaderNamesVisible = true;
            this.stackupGridView.ColumnHeadersHeight = 92;
            this.stackupGridView.DefaultCellBehavior = ZZero.ZPlanner.UI.Grid.ZDataGridViewColumnHeaderCellBehavior.DisabledHidden;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.stackupGridView.DefaultCellStyle = dataGridViewCellStyle1;
            this.stackupGridView.DefaultDateTimeGrouping = ZZero.ZPlanner.UI.Grid.ZDataGridViewFilterMenuDateTimeGrouping.Second;
            this.stackupGridView.Dock = System.Windows.Forms.DockStyle.Fill;
            this.stackupGridView.FilterString = "";
            this.stackupGridView.Location = new System.Drawing.Point(0, 0);
            this.stackupGridView.Name = "stackupGridView";
            this.stackupGridView.RowHeadersWidth = 56;
            this.stackupGridView.Size = new System.Drawing.Size(704, 384);
            this.stackupGridView.SortString = "";
            this.stackupGridView.TabIndex = 1;
            // 
            // printPanel
            // 
            this.printPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.printPanel.Controls.Add(this.stackupGridView);
            this.printPanel.Location = new System.Drawing.Point(0, 0);
            this.printPanel.Name = "printPanel";
            this.printPanel.Size = new System.Drawing.Size(704, 384);
            this.printPanel.TabIndex = 3;
            // 
            // clearAllMaterialsToolStripMenuItem
            // 
            this.clearAllMaterialsToolStripMenuItem.Name = "clearAllMaterialsToolStripMenuItem";
            this.clearAllMaterialsToolStripMenuItem.Size = new System.Drawing.Size(273, 22);
            this.clearAllMaterialsToolStripMenuItem.Text = "What If Analysis on all Dielectric Rows";
            // 
            // ZPlannerStackupPanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.AutoScroll = true;
            this.ClientSize = new System.Drawing.Size(704, 384);
            this.Controls.Add(this.printPanel);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.Name = "ZPlannerStackupPanel";
            this.ShowInTaskbar = false;
            this.Text = "Main View";
            this.rowContextMenu.ResumeLayout(false);
            this.columnContextMenu.ResumeLayout(false);
            this.viaspanContextMenu.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.stackupGridView)).EndInit();
            this.printPanel.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private ZZero.ZPlanner.UI.Grid.ZDataGridView stackupGridView;
        private System.Windows.Forms.ContextMenuStrip rowContextMenu;
        private System.Windows.Forms.ToolStripMenuItem addRowBeforeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addRowAfterToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem moveRowUpToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem moveRowDownToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeRowToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator separator1;
        private System.Windows.Forms.ToolStripSeparator separator2;
        private System.Windows.Forms.ToolStripMenuItem duplicateRowToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem copyRowToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem cutRowToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pasteRowBeforeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pasteRowInsteadToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pasteRowAfterToolStripMenuItem;
        private System.Windows.Forms.ContextMenuStrip columnContextMenu; 
        private System.Windows.Forms.ContextMenuStrip viaspanContextMenu;
        private System.Windows.Forms.ToolStripMenuItem addColumnBeforeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem addColumnAfterToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem moveColumnToTheLeftToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem moveColumnToTheRightToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem removeColumnToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem minimizeColumnToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem hideColumnToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem showColumnToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem propertiesOfColumnToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem swapRowsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem insertHiddenColumnBeforeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem insertHiddenColumnAfterToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem insertExistingColumnBeforeToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem insertExistingColumnAfterToolStripMenuItem;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem viaspanEditToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem pasteMaterialToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem clearMaterialToolStripMenuItem;
        private System.Windows.Forms.Panel printPanel;
        private System.Windows.Forms.ToolStripMenuItem clearAllMaterialsToolStripMenuItem;
    }
}