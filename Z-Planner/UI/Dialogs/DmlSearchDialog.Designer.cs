namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class DmlSearchDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DmlSearchDialog));
            this.label1 = new System.Windows.Forms.Label();
            this.cbSearch = new System.Windows.Forms.ComboBox();
            this.bSearchClose = new System.Windows.Forms.Button();
            this.bSearchAll = new System.Windows.Forms.Button();
            this.bSearchNext = new System.Windows.Forms.Button();
            this.dgvSearch = new System.Windows.Forms.DataGridView();
            this.cValue = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cRow = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn2 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewTextBoxColumn3 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.cbDisplayedOnly = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.dgvSearch)).BeginInit();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 13);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(30, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "Find:";
            // 
            // cbSearch
            // 
            this.cbSearch.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cbSearch.FormattingEnabled = true;
            this.cbSearch.Location = new System.Drawing.Point(49, 10);
            this.cbSearch.Name = "cbSearch";
            this.cbSearch.Size = new System.Drawing.Size(468, 21);
            this.cbSearch.TabIndex = 1;
            // 
            // bSearchClose
            // 
            this.bSearchClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.bSearchClose.Location = new System.Drawing.Point(435, 60);
            this.bSearchClose.Name = "bSearchClose";
            this.bSearchClose.Size = new System.Drawing.Size(82, 23);
            this.bSearchClose.TabIndex = 2;
            this.bSearchClose.Text = "Close";
            this.bSearchClose.UseVisualStyleBackColor = true;
            this.bSearchClose.Click += new System.EventHandler(this.bSearchClose_Click);
            // 
            // bSearchAll
            // 
            this.bSearchAll.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.bSearchAll.Location = new System.Drawing.Point(347, 60);
            this.bSearchAll.Name = "bSearchAll";
            this.bSearchAll.Size = new System.Drawing.Size(82, 23);
            this.bSearchAll.TabIndex = 3;
            this.bSearchAll.Text = "Find All";
            this.bSearchAll.UseVisualStyleBackColor = true;
            this.bSearchAll.Click += new System.EventHandler(this.bSearchAll_Click);
            // 
            // bSearchNext
            // 
            this.bSearchNext.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.bSearchNext.Location = new System.Drawing.Point(259, 60);
            this.bSearchNext.Name = "bSearchNext";
            this.bSearchNext.Size = new System.Drawing.Size(82, 23);
            this.bSearchNext.TabIndex = 4;
            this.bSearchNext.Text = "Find Next";
            this.bSearchNext.UseVisualStyleBackColor = true;
            this.bSearchNext.Click += new System.EventHandler(this.bSearchNext_Click);
            // 
            // dgvSearch
            // 
            this.dgvSearch.AllowUserToAddRows = false;
            this.dgvSearch.AllowUserToDeleteRows = false;
            this.dgvSearch.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dgvSearch.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.AllCells;
            this.dgvSearch.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.dgvSearch.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.cValue,
            this.cRow,
            this.cColumn});
            this.dgvSearch.Location = new System.Drawing.Point(12, 89);
            this.dgvSearch.Name = "dgvSearch";
            this.dgvSearch.ReadOnly = true;
            this.dgvSearch.RowHeadersVisible = false;
            this.dgvSearch.Size = new System.Drawing.Size(505, 211);
            this.dgvSearch.TabIndex = 5;
            this.dgvSearch.CellClick += new System.Windows.Forms.DataGridViewCellEventHandler(this.dgvSearch_CellClick);
            // 
            // cValue
            // 
            this.cValue.HeaderText = "Value";
            this.cValue.Name = "cValue";
            this.cValue.ReadOnly = true;
            this.cValue.Width = 59;
            // 
            // cRow
            // 
            this.cRow.HeaderText = "Row";
            this.cRow.Name = "cRow";
            this.cRow.ReadOnly = true;
            this.cRow.Width = 54;
            // 
            // cColumn
            // 
            this.cColumn.HeaderText = "Column";
            this.cColumn.Name = "cColumn";
            this.cColumn.ReadOnly = true;
            this.cColumn.Width = 67;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "Value";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Width = 220;
            // 
            // dataGridViewTextBoxColumn2
            // 
            this.dataGridViewTextBoxColumn2.HeaderText = "Row";
            this.dataGridViewTextBoxColumn2.Name = "dataGridViewTextBoxColumn2";
            this.dataGridViewTextBoxColumn2.Width = 50;
            // 
            // dataGridViewTextBoxColumn3
            // 
            this.dataGridViewTextBoxColumn3.HeaderText = "Column";
            this.dataGridViewTextBoxColumn3.Name = "dataGridViewTextBoxColumn3";
            // 
            // cbDisplayedOnly
            // 
            this.cbDisplayedOnly.AutoSize = true;
            this.cbDisplayedOnly.Checked = true;
            this.cbDisplayedOnly.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbDisplayedOnly.Location = new System.Drawing.Point(49, 37);
            this.cbDisplayedOnly.Name = "cbDisplayedOnly";
            this.cbDisplayedOnly.Size = new System.Drawing.Size(222, 17);
            this.cbDisplayedOnly.TabIndex = 6;
            this.cbDisplayedOnly.Text = "Perform search in the displayed data only.";
            this.cbDisplayedOnly.UseVisualStyleBackColor = true;
            // 
            // DmlSearchDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(529, 312);
            this.Controls.Add(this.cbDisplayedOnly);
            this.Controls.Add(this.dgvSearch);
            this.Controls.Add(this.bSearchNext);
            this.Controls.Add(this.bSearchAll);
            this.Controls.Add(this.bSearchClose);
            this.Controls.Add(this.cbSearch);
            this.Controls.Add(this.label1);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "DmlSearchDialog";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
            this.Text = "  Library Search";
            ((System.ComponentModel.ISupportInitialize)(this.dgvSearch)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox cbSearch;
        private System.Windows.Forms.Button bSearchClose;
        private System.Windows.Forms.Button bSearchAll;
        private System.Windows.Forms.Button bSearchNext;
        private System.Windows.Forms.DataGridView dgvSearch;
        private System.Windows.Forms.DataGridViewTextBoxColumn cValue;
        private System.Windows.Forms.DataGridViewTextBoxColumn cRow;
        private System.Windows.Forms.DataGridViewTextBoxColumn cColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn3;
        private System.Windows.Forms.CheckBox cbDisplayedOnly;
    }
}