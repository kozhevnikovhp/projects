namespace ZZero.ZPlanner.UI
{
    partial class ZPlannerViaSpanPanel
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle1 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ZPlannerViaSpanPanel));
            this.viaSpanGridView = new ZZero.ZPlanner.UI.Grid.ZDataGridView();
            this.buttonCloseDialog = new System.Windows.Forms.Button();
            this.buttonSaveDialog = new System.Windows.Forms.Button();
            this.cbShowVias = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.viaSpanGridView)).BeginInit();
            this.groupBox2.SuspendLayout();
            this.SuspendLayout();
            // 
            // viaSpanGridView
            // 
            this.viaSpanGridView.AllowDrawCenterLine = false;
            this.viaSpanGridView.AllowUserToAddRows = false;
            this.viaSpanGridView.AllowUserToDeleteRows = false;
            this.viaSpanGridView.AllowUserToResizeColumns = false;
            this.viaSpanGridView.AllowUserToResizeRows = false;
            this.viaSpanGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.viaSpanGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.viaSpanGridView.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.Disable;
            this.viaSpanGridView.ColumnHeaderNamesVisible = true;
            this.viaSpanGridView.ColumnHeadersHeight = 58;
            this.viaSpanGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.viaSpanGridView.DefaultCellBehavior = ZZero.ZPlanner.UI.Grid.ZDataGridViewColumnHeaderCellBehavior.DisabledHidden;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.viaSpanGridView.DefaultCellStyle = dataGridViewCellStyle1;
            this.viaSpanGridView.DefaultDateTimeGrouping = ZZero.ZPlanner.UI.Grid.ZDataGridViewFilterMenuDateTimeGrouping.Second;
            this.viaSpanGridView.EditMode = System.Windows.Forms.DataGridViewEditMode.EditOnEnter;
            this.viaSpanGridView.FilterString = "";
            this.viaSpanGridView.Location = new System.Drawing.Point(15, 43);
            this.viaSpanGridView.Name = "viaSpanGridView";
            this.viaSpanGridView.RowHeadersVisible = false;
            this.viaSpanGridView.RowHeadersWidth = 56;
            this.viaSpanGridView.Size = new System.Drawing.Size(730, 206);
            this.viaSpanGridView.SortString = "";
            this.viaSpanGridView.TabIndex = 1;
            // 
            // buttonCloseDialog
            // 
            this.buttonCloseDialog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCloseDialog.Location = new System.Drawing.Point(663, 321);
            this.buttonCloseDialog.Name = "buttonCloseDialog";
            this.buttonCloseDialog.Size = new System.Drawing.Size(82, 23);
            this.buttonCloseDialog.TabIndex = 5;
            this.buttonCloseDialog.Text = "Cancel";
            this.buttonCloseDialog.UseVisualStyleBackColor = true;
            this.buttonCloseDialog.Click += new System.EventHandler(this.buttonCloseDialog_Click);
            // 
            // buttonSaveDialog
            // 
            this.buttonSaveDialog.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonSaveDialog.Location = new System.Drawing.Point(573, 321);
            this.buttonSaveDialog.Name = "buttonSaveDialog";
            this.buttonSaveDialog.Size = new System.Drawing.Size(82, 23);
            this.buttonSaveDialog.TabIndex = 4;
            this.buttonSaveDialog.Text = "OK";
            this.buttonSaveDialog.UseVisualStyleBackColor = true;
            this.buttonSaveDialog.Click += new System.EventHandler(this.buttonSaveDialog_Click);
            // 
            // cbShowVias
            // 
            this.cbShowVias.AutoSize = true;
            this.cbShowVias.Location = new System.Drawing.Point(15, 13);
            this.cbShowVias.Margin = new System.Windows.Forms.Padding(2);
            this.cbShowVias.Name = "cbShowVias";
            this.cbShowVias.Size = new System.Drawing.Size(75, 17);
            this.cbShowVias.TabIndex = 3;
            this.cbShowVias.Text = "Show vias";
            this.cbShowVias.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Location = new System.Drawing.Point(15, 262);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(2);
            this.groupBox2.Size = new System.Drawing.Size(730, 43);
            this.groupBox2.TabIndex = 33;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Hint:";
            // 
            // label4
            // 
            this.label4.Dock = System.Windows.Forms.DockStyle.Fill;
            this.label4.Location = new System.Drawing.Point(2, 15);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(726, 26);
            this.label4.TabIndex = 35;
            this.label4.Text = "Most PCB fabricators can manufacture 8 : 1 or lower aspect ratios. Above this (e." +
    "g., 10 : 1), requires extra effort for uniform plating.";
            // 
            // ZPlannerViaSpanPanel
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(760, 360);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.buttonCloseDialog);
            this.Controls.Add(this.buttonSaveDialog);
            this.Controls.Add(this.cbShowVias);
            this.Controls.Add(this.viaSpanGridView);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "ZPlannerViaSpanPanel";
            this.ShowInTaskbar = false;
            this.Text = "  Edit Via Spans";
            ((System.ComponentModel.ISupportInitialize)(this.viaSpanGridView)).EndInit();
            this.groupBox2.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private ZZero.ZPlanner.UI.Grid.ZDataGridView viaSpanGridView;
        private System.Windows.Forms.Button buttonCloseDialog;
        private System.Windows.Forms.Button buttonSaveDialog;
        private System.Windows.Forms.CheckBox cbShowVias;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label4;
    }
}