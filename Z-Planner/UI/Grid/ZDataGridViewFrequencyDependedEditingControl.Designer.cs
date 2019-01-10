namespace ZZero.ZPlanner.UI.Grid
{
    partial class ZDataGridViewFrequencyDependedEditingControl
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.singleGroupBox = new System.Windows.Forms.GroupBox();
            this.singleDataGridView = new System.Windows.Forms.DataGridView();
            this.singleValueColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.singleRadioButton = new System.Windows.Forms.RadioButton();
            this.tableGroupBox = new System.Windows.Forms.GroupBox();
            this.tableDataGridView = new System.Windows.Forms.DataGridView();
            this.tableFrequencyColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tableValueColumn = new System.Windows.Forms.DataGridViewTextBoxColumn();
            this.tableRadioButton = new System.Windows.Forms.RadioButton();
            this.okButton = new System.Windows.Forms.Button();
            this.singleGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.singleDataGridView)).BeginInit();
            this.tableGroupBox.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tableDataGridView)).BeginInit();
            this.SuspendLayout();
            // 
            // singleGroupBox
            // 
            this.singleGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.singleGroupBox.Controls.Add(this.singleDataGridView);
            this.singleGroupBox.Controls.Add(this.singleRadioButton);
            this.singleGroupBox.Location = new System.Drawing.Point(4, 0);
            this.singleGroupBox.Name = "singleGroupBox";
            this.singleGroupBox.Size = new System.Drawing.Size(272, 81);
            this.singleGroupBox.TabIndex = 0;
            this.singleGroupBox.TabStop = false;
            this.singleGroupBox.Text = "Single value (frequency independ)";
            // 
            // singleDataGridView
            // 
            this.singleDataGridView.AllowUserToAddRows = false;
            this.singleDataGridView.AllowUserToDeleteRows = false;
            this.singleDataGridView.AllowUserToResizeColumns = false;
            this.singleDataGridView.AllowUserToResizeRows = false;
            this.singleDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.singleDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.singleDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.singleDataGridView.ColumnHeadersVisible = false;
            this.singleDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.singleValueColumn});
            this.singleDataGridView.Location = new System.Drawing.Point(7, 44);
            this.singleDataGridView.MultiSelect = false;
            this.singleDataGridView.Name = "singleDataGridView";
            this.singleDataGridView.Size = new System.Drawing.Size(259, 26);
            this.singleDataGridView.TabIndex = 1;
            // 
            // singleValueColumn
            // 
            this.singleValueColumn.HeaderText = "Value";
            this.singleValueColumn.Name = "singleValueColumn";
            // 
            // singleRadioButton
            // 
            this.singleRadioButton.AutoSize = true;
            this.singleRadioButton.Checked = true;
            this.singleRadioButton.Location = new System.Drawing.Point(7, 20);
            this.singleRadioButton.Name = "singleRadioButton";
            this.singleRadioButton.Size = new System.Drawing.Size(134, 17);
            this.singleRadioButton.TabIndex = 0;
            this.singleRadioButton.TabStop = true;
            this.singleRadioButton.Text = "Frequency independed";
            this.singleRadioButton.UseVisualStyleBackColor = true;
            this.singleRadioButton.CheckedChanged += new System.EventHandler(this.singleRadioButton_CheckedChanged);
            // 
            // tableGroupBox
            // 
            this.tableGroupBox.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableGroupBox.Controls.Add(this.tableDataGridView);
            this.tableGroupBox.Controls.Add(this.tableRadioButton);
            this.tableGroupBox.Location = new System.Drawing.Point(4, 87);
            this.tableGroupBox.Name = "tableGroupBox";
            this.tableGroupBox.Size = new System.Drawing.Size(272, 207);
            this.tableGroupBox.TabIndex = 0;
            this.tableGroupBox.TabStop = false;
            this.tableGroupBox.Text = "Table of values (frequency dependent)";
            // 
            // tableDataGridView
            // 
            this.tableDataGridView.AllowUserToResizeColumns = false;
            this.tableDataGridView.AllowUserToResizeRows = false;
            this.tableDataGridView.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableDataGridView.AutoSizeColumnsMode = System.Windows.Forms.DataGridViewAutoSizeColumnsMode.Fill;
            this.tableDataGridView.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.AutoSize;
            this.tableDataGridView.Columns.AddRange(new System.Windows.Forms.DataGridViewColumn[] {
            this.tableFrequencyColumn,
            this.tableValueColumn});
            this.tableDataGridView.Enabled = false;
            this.tableDataGridView.Location = new System.Drawing.Point(6, 42);
            this.tableDataGridView.MultiSelect = false;
            this.tableDataGridView.Name = "tableDataGridView";
            this.tableDataGridView.Size = new System.Drawing.Size(259, 159);
            this.tableDataGridView.TabIndex = 1;
            // 
            // tableFrequencyColumn
            // 
            this.tableFrequencyColumn.HeaderText = "Frequency";
            this.tableFrequencyColumn.Name = "tableFrequencyColumn";
            // 
            // tableValueColumn
            // 
            this.tableValueColumn.HeaderText = "Value";
            this.tableValueColumn.Name = "tableValueColumn";
            // 
            // tableRadioButton
            // 
            this.tableRadioButton.AutoSize = true;
            this.tableRadioButton.Location = new System.Drawing.Point(7, 19);
            this.tableRadioButton.Name = "tableRadioButton";
            this.tableRadioButton.Size = new System.Drawing.Size(126, 17);
            this.tableRadioButton.TabIndex = 0;
            this.tableRadioButton.TabStop = true;
            this.tableRadioButton.Text = "Frequency depended";
            this.tableRadioButton.UseVisualStyleBackColor = true;
            this.tableRadioButton.CheckedChanged += new System.EventHandler(this.tableRadioButton_CheckedChanged);
            // 
            // okButton
            // 
            this.okButton.Location = new System.Drawing.Point(194, 300);
            this.okButton.Name = "okButton";
            this.okButton.Size = new System.Drawing.Size(75, 23);
            this.okButton.TabIndex = 2;
            this.okButton.Text = "OK";
            this.okButton.UseVisualStyleBackColor = true;
            this.okButton.Click += new System.EventHandler(this.okButton_Click);
            // 
            // ZDataGridViewFrequencyDependedEditingControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.okButton);
            this.Controls.Add(this.tableGroupBox);
            this.Controls.Add(this.singleGroupBox);
            this.Name = "ZDataGridViewFrequencyDependedEditingControl";
            this.Size = new System.Drawing.Size(279, 329);
            this.singleGroupBox.ResumeLayout(false);
            this.singleGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.singleDataGridView)).EndInit();
            this.tableGroupBox.ResumeLayout(false);
            this.tableGroupBox.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.tableDataGridView)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox singleGroupBox;
        private System.Windows.Forms.DataGridView singleDataGridView;
        private System.Windows.Forms.RadioButton singleRadioButton;
        private System.Windows.Forms.GroupBox tableGroupBox;
        private System.Windows.Forms.RadioButton tableRadioButton;
        private System.Windows.Forms.DataGridViewTextBoxColumn singleValueColumn;
        private System.Windows.Forms.DataGridView tableDataGridView;
        private System.Windows.Forms.DataGridViewTextBoxColumn tableFrequencyColumn;
        private System.Windows.Forms.DataGridViewTextBoxColumn tableValueColumn;
        private System.Windows.Forms.Button okButton;
    }
}
