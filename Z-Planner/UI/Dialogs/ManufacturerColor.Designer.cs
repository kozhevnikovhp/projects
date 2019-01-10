namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class ManufacturerColor
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
            this.panelMaterialTable = new System.Windows.Forms.Panel();
            this.tblMaterialPanel = new System.Windows.Forms.TableLayoutPanel();
            this.btnOK = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.panelMaterialTable.SuspendLayout();
            this.SuspendLayout();
            // 
            // panelMaterialTable
            // 
            this.panelMaterialTable.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelMaterialTable.Controls.Add(this.tblMaterialPanel);
            this.panelMaterialTable.Location = new System.Drawing.Point(9, 9);
            this.panelMaterialTable.Name = "panelMaterialTable";
            this.panelMaterialTable.Size = new System.Drawing.Size(287, 241);
            this.panelMaterialTable.TabIndex = 3;
            // 
            // tblMaterialPanel
            // 
            this.tblMaterialPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tblMaterialPanel.AutoScroll = true;
            this.tblMaterialPanel.ColumnCount = 4;
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 16F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 170F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 71F));
            this.tblMaterialPanel.Location = new System.Drawing.Point(3, 3);
            this.tblMaterialPanel.Name = "tblMaterialPanel";
            this.tblMaterialPanel.RowCount = 1;
            this.tblMaterialPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 330F));
            this.tblMaterialPanel.Size = new System.Drawing.Size(277, 330);
            this.tblMaterialPanel.TabIndex = 0;
            // 
            // btnOK
            // 
            this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
            this.btnOK.Location = new System.Drawing.Point(214, 263);
            this.btnOK.Name = "btnOK";
            this.btnOK.Size = new System.Drawing.Size(82, 26);
            this.btnOK.TabIndex = 4;
            this.btnOK.Text = "OK";
            this.btnOK.UseVisualStyleBackColor = true;
            this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(114, 263);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(82, 26);
            this.button1.TabIndex = 5;
            this.button1.Text = "Cancel";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // ManufacturerColor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(306, 301);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.btnOK);
            this.Controls.Add(this.panelMaterialTable);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ManufacturerColor";
            this.Text = "Color by manufacturer";
            this.panelMaterialTable.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panelMaterialTable;
        private System.Windows.Forms.TableLayoutPanel tblMaterialPanel;
        private System.Windows.Forms.Button btnOK;
        private System.Windows.Forms.Button button1;
    }
}