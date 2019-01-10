namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class BulkEditDialog
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BulkEditDialog));
            this.labelSelectColumn = new System.Windows.Forms.Label();
            this.comboBoxSelectColumn = new System.Windows.Forms.ComboBox();
            this.labelEqualTo = new System.Windows.Forms.Label();
            this.textBoxEqualTo = new System.Windows.Forms.TextBox();
            this.labelReplaceWIth = new System.Windows.Forms.Label();
            this.textBoxReplaceWith = new System.Windows.Forms.TextBox();
            this.buttonOk = new System.Windows.Forms.Button();
            this.buttonCancel = new System.Windows.Forms.Button();
            this.checkBoxChangeAll = new System.Windows.Forms.CheckBox();
            this.labelAffectedValues = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // labelSelectColumn
            // 
            this.labelSelectColumn.Location = new System.Drawing.Point(19, 15);
            this.labelSelectColumn.Name = "labelSelectColumn";
            this.labelSelectColumn.Size = new System.Drawing.Size(140, 13);
            this.labelSelectColumn.TabIndex = 0;
            this.labelSelectColumn.Text = "Change value of the column";
            this.labelSelectColumn.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboBoxSelectColumn
            // 
            this.comboBoxSelectColumn.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxSelectColumn.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.comboBoxSelectColumn.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.comboBoxSelectColumn.FormattingEnabled = true;
            this.comboBoxSelectColumn.Location = new System.Drawing.Point(168, 12);
            this.comboBoxSelectColumn.Name = "comboBoxSelectColumn";
            this.comboBoxSelectColumn.Size = new System.Drawing.Size(167, 21);
            this.comboBoxSelectColumn.TabIndex = 1;
            this.comboBoxSelectColumn.SelectedValueChanged += new System.EventHandler(this.comboBoxSelectColumn_SelectedValueChanged);
            // 
            // labelEqualTo
            // 
            this.labelEqualTo.Location = new System.Drawing.Point(14, 50);
            this.labelEqualTo.Name = "labelEqualTo";
            this.labelEqualTo.Size = new System.Drawing.Size(145, 17);
            this.labelEqualTo.TabIndex = 2;
            this.labelEqualTo.Text = "Change only values equal to";
            this.labelEqualTo.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBoxEqualTo
            // 
            this.textBoxEqualTo.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxEqualTo.Enabled = false;
            this.textBoxEqualTo.Location = new System.Drawing.Point(168, 49);
            this.textBoxEqualTo.Name = "textBoxEqualTo";
            this.textBoxEqualTo.Size = new System.Drawing.Size(79, 20);
            this.textBoxEqualTo.TabIndex = 3;
            this.textBoxEqualTo.TextChanged += new System.EventHandler(this.textBoxEqualTo_TextChanged);
            // 
            // labelReplaceWIth
            // 
            this.labelReplaceWIth.Location = new System.Drawing.Point(22, 85);
            this.labelReplaceWIth.Name = "labelReplaceWIth";
            this.labelReplaceWIth.Size = new System.Drawing.Size(137, 17);
            this.labelReplaceWIth.TabIndex = 4;
            this.labelReplaceWIth.Text = "Replace with value";
            this.labelReplaceWIth.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBoxReplaceWith
            // 
            this.textBoxReplaceWith.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxReplaceWith.Enabled = false;
            this.textBoxReplaceWith.Location = new System.Drawing.Point(168, 84);
            this.textBoxReplaceWith.Name = "textBoxReplaceWith";
            this.textBoxReplaceWith.Size = new System.Drawing.Size(79, 20);
            this.textBoxReplaceWith.TabIndex = 5;
            this.textBoxReplaceWith.TextChanged += new System.EventHandler(this.textBoxReplaceWith_TextChanged);
            // 
            // buttonOk
            // 
            this.buttonOk.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonOk.Location = new System.Drawing.Point(165, 119);
            this.buttonOk.Name = "buttonOk";
            this.buttonOk.Size = new System.Drawing.Size(82, 23);
            this.buttonOk.TabIndex = 6;
            this.buttonOk.Text = "Change";
            this.buttonOk.UseVisualStyleBackColor = true;
            this.buttonOk.Click += new System.EventHandler(this.buttonOk_Click);
            // 
            // buttonCancel
            // 
            this.buttonCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonCancel.Location = new System.Drawing.Point(253, 119);
            this.buttonCancel.Name = "buttonCancel";
            this.buttonCancel.Size = new System.Drawing.Size(82, 23);
            this.buttonCancel.TabIndex = 7;
            this.buttonCancel.Text = "Cancel";
            this.buttonCancel.UseVisualStyleBackColor = true;
            this.buttonCancel.Click += new System.EventHandler(this.buttonCancel_Click);
            // 
            // checkBoxChangeAll
            // 
            this.checkBoxChangeAll.AutoSize = true;
            this.checkBoxChangeAll.Enabled = false;
            this.checkBoxChangeAll.Location = new System.Drawing.Point(258, 51);
            this.checkBoxChangeAll.Name = "checkBoxChangeAll";
            this.checkBoxChangeAll.Size = new System.Drawing.Size(77, 17);
            this.checkBoxChangeAll.TabIndex = 10;
            this.checkBoxChangeAll.Text = "Change All";
            this.checkBoxChangeAll.UseVisualStyleBackColor = true;
            this.checkBoxChangeAll.CheckedChanged += new System.EventHandler(this.checkBoxChangeAll_CheckedChanged);
            // 
            // labelAffectedValues
            // 
            this.labelAffectedValues.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.labelAffectedValues.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.labelAffectedValues.Location = new System.Drawing.Point(3, 122);
            this.labelAffectedValues.Name = "labelAffectedValues";
            this.labelAffectedValues.Size = new System.Drawing.Size(40, 17);
            this.labelAffectedValues.TabIndex = 11;
            this.labelAffectedValues.Text = "0";
            this.labelAffectedValues.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label1
            // 
            this.label1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label1.ForeColor = System.Drawing.SystemColors.ControlDarkDark;
            this.label1.Location = new System.Drawing.Point(39, 122);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(120, 17);
            this.label1.TabIndex = 12;
            this.label1.Text = "value(s) will be affected";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // BulkEditDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(351, 156);
            this.Controls.Add(this.labelAffectedValues);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.checkBoxChangeAll);
            this.Controls.Add(this.buttonOk);
            this.Controls.Add(this.textBoxReplaceWith);
            this.Controls.Add(this.buttonCancel);
            this.Controls.Add(this.labelReplaceWIth);
            this.Controls.Add(this.textBoxEqualTo);
            this.Controls.Add(this.labelEqualTo);
            this.Controls.Add(this.comboBoxSelectColumn);
            this.Controls.Add(this.labelSelectColumn);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "BulkEditDialog";
            this.ShowInTaskbar = false;
            this.Text = "  Edit Column Values";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label labelSelectColumn;
        private System.Windows.Forms.ComboBox comboBoxSelectColumn;
        private System.Windows.Forms.Label labelEqualTo;
        private System.Windows.Forms.TextBox textBoxEqualTo;
        private System.Windows.Forms.Label labelReplaceWIth;
        private System.Windows.Forms.TextBox textBoxReplaceWith;
        private System.Windows.Forms.Button buttonOk;
        private System.Windows.Forms.Button buttonCancel;
        private System.Windows.Forms.CheckBox checkBoxChangeAll;
        private System.Windows.Forms.Label labelAffectedValues;
        private System.Windows.Forms.Label label1;
    }
}