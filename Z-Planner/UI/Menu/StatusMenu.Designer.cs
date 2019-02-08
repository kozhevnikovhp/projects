namespace ZZero.ZPlanner.UI.Menu
{
    partial class StatusMenu
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
            this.progressPictureBox = new System.Windows.Forms.PictureBox();
            this.panel1 = new System.Windows.Forms.Panel();
            this.tbBoardThickness = new System.Windows.Forms.Label();
            this.tbNumberOfLayers = new System.Windows.Forms.Label();
            this.labelBoardThickness = new System.Windows.Forms.Label();
            this.labelNumberOfLayers = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.tbStatus = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.progressPictureBox)).BeginInit();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // progressPictureBox
            // 
            this.progressPictureBox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.progressPictureBox.Enabled = false;
            this.progressPictureBox.Image = global::ZZero.ZPlanner.Properties.Resources.ProgressBar;
            this.progressPictureBox.Location = new System.Drawing.Point(482, 5);
            this.progressPictureBox.Margin = new System.Windows.Forms.Padding(0);
            this.progressPictureBox.Name = "progressPictureBox";
            this.progressPictureBox.Size = new System.Drawing.Size(100, 20);
            this.progressPictureBox.TabIndex = 1;
            this.progressPictureBox.TabStop = false;
            this.progressPictureBox.Visible = false;
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.tbBoardThickness);
            this.panel1.Controls.Add(this.tbNumberOfLayers);
            this.panel1.Controls.Add(this.labelBoardThickness);
            this.panel1.Controls.Add(this.labelNumberOfLayers);
            this.panel1.Location = new System.Drawing.Point(588, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(412, 29);
            this.panel1.TabIndex = 2;
            // 
            // tbBoardThickness
            // 
            this.tbBoardThickness.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbBoardThickness.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbBoardThickness.Location = new System.Drawing.Point(307, 5);
            this.tbBoardThickness.Name = "tbBoardThickness";
            this.tbBoardThickness.Size = new System.Drawing.Size(100, 20);
            this.tbBoardThickness.TabIndex = 3;
            this.tbBoardThickness.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // tbNumberOfLayers
            // 
            this.tbNumberOfLayers.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbNumberOfLayers.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbNumberOfLayers.Location = new System.Drawing.Point(104, 5);
            this.tbNumberOfLayers.Name = "tbNumberOfLayers";
            this.tbNumberOfLayers.Size = new System.Drawing.Size(100, 20);
            this.tbNumberOfLayers.TabIndex = 2;
            this.tbNumberOfLayers.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // labelBoardThickness
            // 
            this.labelBoardThickness.Location = new System.Drawing.Point(213, 5);
            this.labelBoardThickness.Name = "labelBoardThickness";
            this.labelBoardThickness.Size = new System.Drawing.Size(93, 20);
            this.labelBoardThickness.TabIndex = 0;
            this.labelBoardThickness.Text = "Board Thickness:";
            this.labelBoardThickness.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // labelNumberOfLayers
            // 
            this.labelNumberOfLayers.Location = new System.Drawing.Point(3, 5);
            this.labelNumberOfLayers.Name = "labelNumberOfLayers";
            this.labelNumberOfLayers.Size = new System.Drawing.Size(100, 20);
            this.labelNumberOfLayers.TabIndex = 0;
            this.labelNumberOfLayers.Text = "Number of Layers:";
            this.labelNumberOfLayers.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.progressPictureBox);
            this.panel2.Controls.Add(this.tbStatus);
            this.panel2.Location = new System.Drawing.Point(0, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(585, 29);
            this.panel2.TabIndex = 2;
            // 
            // tbStatus
            // 
            this.tbStatus.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbStatus.Location = new System.Drawing.Point(3, 5);
            this.tbStatus.Name = "tbStatus";
            this.tbStatus.Size = new System.Drawing.Size(579, 20);
            this.tbStatus.TabIndex = 2;
            this.tbStatus.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // StatusMenu
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "StatusMenu";
            this.Size = new System.Drawing.Size(1000, 29);
            ((System.ComponentModel.ISupportInitialize)(this.progressPictureBox)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.PictureBox progressPictureBox;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label labelBoardThickness;
        private System.Windows.Forms.Label labelNumberOfLayers;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label tbBoardThickness;
        private System.Windows.Forms.Label tbNumberOfLayers;
        private System.Windows.Forms.Label tbStatus;
    }
}
