namespace ZZero.ZPlanner.UI.Editor
{
    partial class ZDataCollectionEditor
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
            this.tableLayoutPanel = new System.Windows.Forms.TableLayoutPanel();
            this.flowLayoutPanelUpDown = new System.Windows.Forms.FlowLayoutPanel();
            this.buttonUp = new System.Windows.Forms.Button();
            this.buttonDown = new System.Windows.Forms.Button();
            this.flowLayoutPanelAddRemove = new System.Windows.Forms.FlowLayoutPanel();
            this.buttonAdd = new System.Windows.Forms.Button();
            this.buttonRemove = new System.Windows.Forms.Button();
            this.listBox = new System.Windows.Forms.ListBox();
            this.tableLayoutPanelTitleDescription = new System.Windows.Forms.TableLayoutPanel();
            this.labelTitle = new System.Windows.Forms.Label();
            this.labelDescription = new System.Windows.Forms.Label();
            this.panelTitleDescription = new System.Windows.Forms.Panel();
            this.tableLayoutPanel.SuspendLayout();
            this.flowLayoutPanelUpDown.SuspendLayout();
            this.flowLayoutPanelAddRemove.SuspendLayout();
            this.tableLayoutPanelTitleDescription.SuspendLayout();
            this.panelTitleDescription.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel
            // 
            this.tableLayoutPanel.ColumnCount = 2;
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 70F));
            this.tableLayoutPanel.Controls.Add(this.flowLayoutPanelUpDown, 1, 0);
            this.tableLayoutPanel.Controls.Add(this.flowLayoutPanelAddRemove, 1, 2);
            this.tableLayoutPanel.Controls.Add(this.listBox, 0, 0);
            this.tableLayoutPanel.Controls.Add(this.panelTitleDescription, 0, 3);
            this.tableLayoutPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.tableLayoutPanel.Name = "tableLayoutPanel";
            this.tableLayoutPanel.RowCount = 4;
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tableLayoutPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 62F));
            this.tableLayoutPanel.Size = new System.Drawing.Size(336, 258);
            this.tableLayoutPanel.TabIndex = 0;
            // 
            // flowLayoutPanelUpDown
            // 
            this.flowLayoutPanelUpDown.Controls.Add(this.buttonUp);
            this.flowLayoutPanelUpDown.Controls.Add(this.buttonDown);
            this.flowLayoutPanelUpDown.Dock = System.Windows.Forms.DockStyle.Top;
            this.flowLayoutPanelUpDown.Location = new System.Drawing.Point(269, 0);
            this.flowLayoutPanelUpDown.Margin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.flowLayoutPanelUpDown.Name = "flowLayoutPanelUpDown";
            this.flowLayoutPanelUpDown.Size = new System.Drawing.Size(67, 46);
            this.flowLayoutPanelUpDown.TabIndex = 0;
            // 
            // buttonUp
            // 
            this.buttonUp.Location = new System.Drawing.Point(0, 0);
            this.buttonUp.Margin = new System.Windows.Forms.Padding(0);
            this.buttonUp.Name = "buttonUp";
            this.buttonUp.Size = new System.Drawing.Size(67, 23);
            this.buttonUp.TabIndex = 0;
            this.buttonUp.Text = "▲";
            this.buttonUp.UseVisualStyleBackColor = true;
            this.buttonUp.Click += new System.EventHandler(this.buttonUp_Click);
            // 
            // buttonDown
            // 
            this.buttonDown.Location = new System.Drawing.Point(0, 23);
            this.buttonDown.Margin = new System.Windows.Forms.Padding(0);
            this.buttonDown.Name = "buttonDown";
            this.buttonDown.Size = new System.Drawing.Size(67, 23);
            this.buttonDown.TabIndex = 1;
            this.buttonDown.Text = "▼";
            this.buttonDown.UseVisualStyleBackColor = true;
            this.buttonDown.Click += new System.EventHandler(this.buttonDown_Click);
            // 
            // flowLayoutPanelAddRemove
            // 
            this.flowLayoutPanelAddRemove.Controls.Add(this.buttonAdd);
            this.flowLayoutPanelAddRemove.Controls.Add(this.buttonRemove);
            this.flowLayoutPanelAddRemove.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.flowLayoutPanelAddRemove.Location = new System.Drawing.Point(269, 150);
            this.flowLayoutPanelAddRemove.Margin = new System.Windows.Forms.Padding(3, 0, 0, 0);
            this.flowLayoutPanelAddRemove.Name = "flowLayoutPanelAddRemove";
            this.flowLayoutPanelAddRemove.Size = new System.Drawing.Size(67, 46);
            this.flowLayoutPanelAddRemove.TabIndex = 1;
            // 
            // buttonAdd
            // 
            this.buttonAdd.Location = new System.Drawing.Point(0, 0);
            this.buttonAdd.Margin = new System.Windows.Forms.Padding(0);
            this.buttonAdd.Name = "buttonAdd";
            this.buttonAdd.Size = new System.Drawing.Size(67, 23);
            this.buttonAdd.TabIndex = 0;
            this.buttonAdd.Text = "Add";
            this.buttonAdd.UseVisualStyleBackColor = true;
            this.buttonAdd.Click += new System.EventHandler(this.buttonAdd_Click);
            // 
            // buttonRemove
            // 
            this.buttonRemove.Location = new System.Drawing.Point(0, 23);
            this.buttonRemove.Margin = new System.Windows.Forms.Padding(0);
            this.buttonRemove.Name = "buttonRemove";
            this.buttonRemove.Size = new System.Drawing.Size(67, 23);
            this.buttonRemove.TabIndex = 1;
            this.buttonRemove.Text = "Delete";
            this.buttonRemove.UseVisualStyleBackColor = true;
            this.buttonRemove.Click += new System.EventHandler(this.buttonRemove_Click);
            // 
            // listBox
            // 
            this.listBox.Dock = System.Windows.Forms.DockStyle.Fill;
            this.listBox.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listBox.FormattingEnabled = true;
            this.listBox.Location = new System.Drawing.Point(0, 0);
            this.listBox.Margin = new System.Windows.Forms.Padding(0);
            this.listBox.Name = "listBox";
            this.tableLayoutPanel.SetRowSpan(this.listBox, 3);
            this.listBox.Size = new System.Drawing.Size(266, 196);
            this.listBox.TabIndex = 4;
            // 
            // tableLayoutPanelTitleDescription
            // 
            this.tableLayoutPanelTitleDescription.AutoScroll = true;
            this.tableLayoutPanelTitleDescription.ColumnCount = 1;
            this.tableLayoutPanelTitleDescription.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelTitleDescription.Controls.Add(this.labelTitle, 0, 0);
            this.tableLayoutPanelTitleDescription.Controls.Add(this.labelDescription, 0, 1);
            this.tableLayoutPanelTitleDescription.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanelTitleDescription.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanelTitleDescription.Name = "tableLayoutPanelTitleDescription";
            this.tableLayoutPanelTitleDescription.RowCount = 2;
            this.tableLayoutPanelTitleDescription.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 16F));
            this.tableLayoutPanelTitleDescription.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanelTitleDescription.Size = new System.Drawing.Size(334, 57);
            this.tableLayoutPanelTitleDescription.TabIndex = 5;
            // 
            // labelTitle
            // 
            this.labelTitle.AutoSize = true;
            this.labelTitle.Dock = System.Windows.Forms.DockStyle.Fill;
            this.labelTitle.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.labelTitle.Location = new System.Drawing.Point(3, 0);
            this.labelTitle.Name = "labelTitle";
            this.labelTitle.Size = new System.Drawing.Size(328, 16);
            this.labelTitle.TabIndex = 0;
            // 
            // labelDescription
            // 
            this.labelDescription.AutoEllipsis = true;
            this.labelDescription.AutoSize = true;
            this.labelDescription.Location = new System.Drawing.Point(3, 16);
            this.labelDescription.Name = "labelDescription";
            this.labelDescription.Size = new System.Drawing.Size(0, 13);
            this.labelDescription.TabIndex = 1;
            // 
            // panelTitleDescription
            // 
            this.panelTitleDescription.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tableLayoutPanel.SetColumnSpan(this.panelTitleDescription, 2);
            this.panelTitleDescription.Controls.Add(this.tableLayoutPanelTitleDescription);
            this.panelTitleDescription.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panelTitleDescription.Location = new System.Drawing.Point(0, 199);
            this.panelTitleDescription.Margin = new System.Windows.Forms.Padding(0, 3, 0, 0);
            this.panelTitleDescription.Name = "panelTitleDescription";
            this.panelTitleDescription.Size = new System.Drawing.Size(336, 59);
            this.panelTitleDescription.TabIndex = 6;
            // 
            // ZDataCollectionEditor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.tableLayoutPanel);
            this.MinimumSize = new System.Drawing.Size(200, 160);
            this.Name = "ZDataCollectionEditor";
            this.Size = new System.Drawing.Size(336, 258);
            this.tableLayoutPanel.ResumeLayout(false);
            this.flowLayoutPanelUpDown.ResumeLayout(false);
            this.flowLayoutPanelAddRemove.ResumeLayout(false);
            this.tableLayoutPanelTitleDescription.ResumeLayout(false);
            this.tableLayoutPanelTitleDescription.PerformLayout();
            this.panelTitleDescription.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanelUpDown;
        private System.Windows.Forms.Button buttonUp;
        private System.Windows.Forms.Button buttonDown;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanelAddRemove;
        private System.Windows.Forms.Button buttonAdd;
        private System.Windows.Forms.Button buttonRemove;
        private System.Windows.Forms.ListBox listBox;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanelTitleDescription;
        private System.Windows.Forms.Label labelTitle;
        private System.Windows.Forms.Label labelDescription;
        private System.Windows.Forms.Panel panelTitleDescription;
    }
}
