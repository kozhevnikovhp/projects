namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class CustomFilterDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(CustomFilterDialog));
            this.labelFilterName = new System.Windows.Forms.Label();
            this.textBoxFilterName = new System.Windows.Forms.TextBox();
            this.buttonAddFilter = new System.Windows.Forms.Button();
            this.buttonDeleteFilter = new System.Windows.Forms.Button();
            this.buttonClerFilter = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.buttonSave = new System.Windows.Forms.Button();
            this.dataGridViewFilter = new System.Windows.Forms.DataGridView();
            this.FieldName = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.Condition = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.Value = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.dataGridViewSort = new System.Windows.Forms.DataGridView();
            this.dataGridViewComboBoxColumn3 = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.dataGridViewComboBoxColumn4 = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.labelFilterTitle = new System.Windows.Forms.Label();
            this.labelSortTitle = new System.Windows.Forms.Label();
            this.buttonAddSort = new System.Windows.Forms.Button();
            this.buttonDeleteSort = new System.Windows.Forms.Button();
            this.buttonClearSort = new System.Windows.Forms.Button();
            this.dataGridViewComboBoxColumn1 = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.dataGridViewComboBoxColumn2 = new System.Windows.Forms.DataGridViewComboBoxColumn();
            this.dataGridViewTextBoxColumn1 = new System.Windows.Forms.DataGridViewTextBoxColumn();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewFilter)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewSort)).BeginInit();
            this.SuspendLayout();
            // 
            // labelFilterName
            // 
            this.labelFilterName.AutoSize = true;
            this.labelFilterName.Location = new System.Drawing.Point(5, 11);
            this.labelFilterName.Name = "labelFilterName";
            this.labelFilterName.Size = new System.Drawing.Size(63, 13);
            this.labelFilterName.TabIndex = 0;
            this.labelFilterName.Text = "Filter Name:";
            // 
            // textBoxFilterName
            // 
            this.textBoxFilterName.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFilterName.Location = new System.Drawing.Point(96, 8);
            this.textBoxFilterName.Name = "textBoxFilterName";
            this.textBoxFilterName.Size = new System.Drawing.Size(398, 20);
            this.textBoxFilterName.TabIndex = 1;
            this.textBoxFilterName.TextChanged += new System.EventHandler(this.textBoxFilterName_TextChanged);
            // 
            // buttonAddFilter
            // 
            this.buttonAddFilter.Location = new System.Drawing.Point(8, 195);
            this.buttonAddFilter.Name = "buttonAddFilter";
            this.buttonAddFilter.Size = new System.Drawing.Size(82, 23);
            this.buttonAddFilter.TabIndex = 2;
            this.buttonAddFilter.Text = "Add Line";
            this.buttonAddFilter.UseVisualStyleBackColor = true;
            this.buttonAddFilter.Click += new System.EventHandler(this.buttonAddFilter_Click);
            // 
            // buttonDeleteFilter
            // 
            this.buttonDeleteFilter.Enabled = false;
            this.buttonDeleteFilter.Location = new System.Drawing.Point(8, 226);
            this.buttonDeleteFilter.Name = "buttonDeleteFilter";
            this.buttonDeleteFilter.Size = new System.Drawing.Size(82, 23);
            this.buttonDeleteFilter.TabIndex = 2;
            this.buttonDeleteFilter.Text = "Delete";
            this.buttonDeleteFilter.UseVisualStyleBackColor = true;
            this.buttonDeleteFilter.Click += new System.EventHandler(this.buttonDeleteFilter_Click);
            // 
            // buttonClerFilter
            // 
            this.buttonClerFilter.Location = new System.Drawing.Point(8, 257);
            this.buttonClerFilter.Name = "buttonClerFilter";
            this.buttonClerFilter.Size = new System.Drawing.Size(82, 23);
            this.buttonClerFilter.TabIndex = 2;
            this.buttonClerFilter.Text = "Clear";
            this.buttonClerFilter.UseVisualStyleBackColor = true;
            this.buttonClerFilter.Click += new System.EventHandler(this.buttonClerFilter_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCancel.Location = new System.Drawing.Point(412, 386);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(82, 23);
            this.buttonCancel.TabIndex = 2;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // buttonSave
            // 
            this.buttonSave.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonSave.Enabled = false;
            this.buttonSave.Location = new System.Drawing.Point(324, 386);
            this.buttonSave.Name = "buttonSave";
            this.buttonSave.Size = new System.Drawing.Size(82, 23);
            this.buttonSave.TabIndex = 2;
            this.buttonSave.Text = "Save";
            this.buttonSave.UseVisualStyleBackColor = true;
            this.buttonSave.Click += new System.EventHandler(this.buttonSave_Click);
            // 
            // dataGridViewFilter
            // 
            this.dataGridViewFilter.AllowUserToAddRows = false;
            this.dataGridViewFilter.AllowUserToResizeColumns = false;
            this.dataGridViewFilter.AllowUserToResizeRows = false;
            this.dataGridViewFilter.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridViewFilter.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridViewFilter.BackgroundColor = System.Drawing.SystemColors.Control;
            this.dataGridViewFilter.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.Disable;
            this.dataGridViewFilter.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dataGridViewFilter.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.FieldName,
            this.Condition,
            this.Value});
            this.dataGridViewFilter.Location = new System.Drawing.Point(96, 179);
            this.dataGridViewFilter.MultiSelect = false;
            this.dataGridViewFilter.Name = "dataGridViewFilter";
            this.dataGridViewFilter.RowHeadersWidth = 25;
            this.dataGridViewFilter.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dataGridViewFilter.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.dataGridViewFilter.Size = new System.Drawing.Size(398, 192);
            this.dataGridViewFilter.TabIndex = 3;
            this.dataGridViewFilter.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewFilter_CellValueChanged);
            this.dataGridViewFilter.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.dataGridView_EditingControlShowing);
            this.dataGridViewFilter.RowStateChanged += new System.Windows.Forms.DataGridViewRowStateChangedEventHandler(this.dataGridViewFilter_RowStateChanged);
            // 
            // FieldName
            // 
            this.FieldName.DisplayStyle = System.Windows.Forms.DataGridViewComboBoxDisplayStyle.Nothing;
            this.FieldName.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.FieldName.HeaderText = "Field";
            this.FieldName.Name = "FieldName";
            // 
            // Condition
            // 
            this.Condition.HeaderText = "Condition";
            this.Condition.Name = "Condition";
            // 
            // Value
            // 
            this.Value.HeaderText = "Value";
            this.Value.Name = "Value";
            // 
            // dataGridViewSort
            // 
            this.dataGridViewSort.AllowUserToAddRows = false;
            this.dataGridViewSort.AllowUserToResizeColumns = false;
            this.dataGridViewSort.AllowUserToResizeRows = false;
            this.dataGridViewSort.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridViewSort.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.dataGridViewSort.BackgroundColor = System.Drawing.SystemColors.Control;
            this.dataGridViewSort.ClipboardCopyMode = System.Windows.Forms.DataGridViewClipboardCopyMode.Disable;
            this.dataGridViewSort.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dataGridViewSort.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.dataGridViewComboBoxColumn3,
            this.dataGridViewComboBoxColumn4});
            this.dataGridViewSort.Location = new System.Drawing.Point(96, 50);
            this.dataGridViewSort.MultiSelect = false;
            this.dataGridViewSort.Name = "dataGridViewSort";
            this.dataGridViewSort.RowHeadersWidth = 25;
            this.dataGridViewSort.RowHeadersWidthSizeMode = System.Windows.Forms.DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            this.dataGridViewSort.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.dataGridViewSort.Size = new System.Drawing.Size(398, 112);
            this.dataGridViewSort.TabIndex = 3;
            this.dataGridViewSort.CellValueChanged += new System.Windows.Forms.DataGridViewCellEventHandler(this.dataGridViewSort_CellValueChanged);
            this.dataGridViewSort.EditingControlShowing += new System.Windows.Forms.DataGridViewEditingControlShowingEventHandler(this.dataGridView_EditingControlShowing);
            this.dataGridViewSort.RowStateChanged += new System.Windows.Forms.DataGridViewRowStateChangedEventHandler(this.dataGridViewSort_RowStateChanged);
            // 
            // dataGridViewComboBoxColumn3
            // 
            this.dataGridViewComboBoxColumn3.DisplayStyle = System.Windows.Forms.DataGridViewComboBoxDisplayStyle.Nothing;
            this.dataGridViewComboBoxColumn3.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.dataGridViewComboBoxColumn3.HeaderText = "Field";
            this.dataGridViewComboBoxColumn3.Name = "dataGridViewComboBoxColumn3";
            // 
            // dataGridViewComboBoxColumn4
            // 
            this.dataGridViewComboBoxColumn4.HeaderText = "Direction";
            this.dataGridViewComboBoxColumn4.Name = "dataGridViewComboBoxColumn4";
            // 
            // labelFilterTitle
            // 
            this.labelFilterTitle.AutoSize = true;
            this.labelFilterTitle.Location = new System.Drawing.Point(5, 179);
            this.labelFilterTitle.Name = "labelFilterTitle";
            this.labelFilterTitle.Size = new System.Drawing.Size(46, 13);
            this.labelFilterTitle.TabIndex = 4;
            this.labelFilterTitle.Text = "Filtering:";
            // 
            // labelSortTitle
            // 
            this.labelSortTitle.AutoSize = true;
            this.labelSortTitle.Location = new System.Drawing.Point(5, 50);
            this.labelSortTitle.Name = "labelSortTitle";
            this.labelSortTitle.Size = new System.Drawing.Size(43, 13);
            this.labelSortTitle.TabIndex = 4;
            this.labelSortTitle.Text = "Sorting:";
            // 
            // buttonAddSort
            // 
            this.buttonAddSort.Location = new System.Drawing.Point(8, 66);
            this.buttonAddSort.Name = "buttonAddSort";
            this.buttonAddSort.Size = new System.Drawing.Size(82, 23);
            this.buttonAddSort.TabIndex = 2;
            this.buttonAddSort.Text = "Add Line";
            this.buttonAddSort.UseVisualStyleBackColor = true;
            this.buttonAddSort.Click += new System.EventHandler(this.buttonAddSort_Click);
            // 
            // buttonDeleteSort
            // 
            this.buttonDeleteSort.Enabled = false;
            this.buttonDeleteSort.Location = new System.Drawing.Point(8, 97);
            this.buttonDeleteSort.Name = "buttonDeleteSort";
            this.buttonDeleteSort.Size = new System.Drawing.Size(82, 23);
            this.buttonDeleteSort.TabIndex = 2;
            this.buttonDeleteSort.Text = "Delete";
            this.buttonDeleteSort.UseVisualStyleBackColor = true;
            this.buttonDeleteSort.Click += new System.EventHandler(this.buttonDeleteSort_Click);
            // 
            // buttonClearSort
            // 
            this.buttonClearSort.Location = new System.Drawing.Point(8, 128);
            this.buttonClearSort.Name = "buttonClearSort";
            this.buttonClearSort.Size = new System.Drawing.Size(82, 23);
            this.buttonClearSort.TabIndex = 2;
            this.buttonClearSort.Text = "Clear";
            this.buttonClearSort.UseVisualStyleBackColor = true;
            this.buttonClearSort.Click += new System.EventHandler(this.buttonClearSort_Click);
            // 
            // dataGridViewComboBoxColumn1
            // 
            this.dataGridViewComboBoxColumn1.DisplayStyle = System.Windows.Forms.DataGridViewComboBoxDisplayStyle.ComboBox;
            this.dataGridViewComboBoxColumn1.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.dataGridViewComboBoxColumn1.HeaderText = "Field";
            this.dataGridViewComboBoxColumn1.Name = "dataGridViewComboBoxColumn1";
            this.dataGridViewComboBoxColumn1.Width = 101;
            // 
            // dataGridViewComboBoxColumn2
            // 
            this.dataGridViewComboBoxColumn2.HeaderText = "Condition";
            this.dataGridViewComboBoxColumn2.Name = "dataGridViewComboBoxColumn2";
            this.dataGridViewComboBoxColumn2.Width = 101;
            // 
            // dataGridViewTextBoxColumn1
            // 
            this.dataGridViewTextBoxColumn1.HeaderText = "Value";
            this.dataGridViewTextBoxColumn1.Name = "dataGridViewTextBoxColumn1";
            this.dataGridViewTextBoxColumn1.Width = 101;
            // 
            // CustomFilterDialog
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(506, 423);
            this.Controls.Add(this.labelSortTitle);
            this.Controls.Add(this.labelFilterTitle);
            this.Controls.Add(this.dataGridViewSort);
            this.Controls.Add(this.dataGridViewFilter);
            this.Controls.Add(this.buttonSave);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.buttonClearSort);
            this.Controls.Add(this.buttonClerFilter);
            this.Controls.Add(this.buttonDeleteSort);
            this.Controls.Add(this.buttonDeleteFilter);
            this.Controls.Add(this.buttonAddSort);
            this.Controls.Add(this.buttonAddFilter);
            this.Controls.Add(this.textBoxFilterName);
            this.Controls.Add(this.labelFilterName);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(522, 461);
            this.Name = "CustomFilterDialog";
            this.ShowInTaskbar = false;
            this.Text = "  Custom Filter";
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewFilter)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridViewSort)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelFilterName;
        private System.Windows.Forms.TextBox textBoxFilterName;
        private System.Windows.Forms.Button buttonAddFilter;
        private System.Windows.Forms.Button buttonDeleteFilter;
        private System.Windows.Forms.Button buttonClerFilter;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.Button buttonSave;
        private System.Windows.Forms.DataGridView dataGridViewFilter;
        private System.Windows.Forms.DataGridViewComboBoxColumn dataGridViewComboBoxColumn1;
        private System.Windows.Forms.DataGridViewComboBoxColumn dataGridViewComboBoxColumn2;
        private System.Windows.Forms.DataGridViewTextBoxColumn dataGridViewTextBoxColumn1;
        private System.Windows.Forms.DataGridViewComboBoxColumn FieldName;
        private System.Windows.Forms.DataGridViewComboBoxColumn Condition;
        private System.Windows.Forms.DataGridViewTextBoxColumn Value;
        private System.Windows.Forms.DataGridView dataGridViewSort;
        private System.Windows.Forms.DataGridViewComboBoxColumn dataGridViewComboBoxColumn3;
        private System.Windows.Forms.DataGridViewComboBoxColumn dataGridViewComboBoxColumn4;
        private System.Windows.Forms.Label labelFilterTitle;
        private System.Windows.Forms.Label labelSortTitle;
        private System.Windows.Forms.Button buttonAddSort;
        private System.Windows.Forms.Button buttonDeleteSort;
        private System.Windows.Forms.Button buttonClearSort;
    }
}