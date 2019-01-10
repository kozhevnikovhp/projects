namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class LossPlot
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(LossPlot));
            this.chartLoss = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.label57 = new System.Windows.Forms.Label();
            this.tbDominates = new System.Windows.Forms.TextBox();
            this.label56 = new System.Windows.Forms.Label();
            this.label54 = new System.Windows.Forms.Label();
            this.tbFmax = new System.Windows.Forms.TextBox();
            this.label55 = new System.Windows.Forms.Label();
            this.label53 = new System.Windows.Forms.Label();
            this.tbFmin = new System.Windows.Forms.TextBox();
            this.label52 = new System.Windows.Forms.Label();
            this.tbCursorCuRoughness = new System.Windows.Forms.TextBox();
            this.pictColorCuRoughness = new System.Windows.Forms.PictureBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tbCursorX = new System.Windows.Forms.TextBox();
            this.tbCursorResistive = new System.Windows.Forms.TextBox();
            this.tbCursorTotal = new System.Windows.Forms.TextBox();
            this.pictColorResistive = new System.Windows.Forms.PictureBox();
            this.pictColorTotal = new System.Windows.Forms.PictureBox();
            this.cbRoughness = new System.Windows.Forms.CheckBox();
            this.cbDielectric = new System.Windows.Forms.CheckBox();
            this.cbResistive = new System.Windows.Forms.CheckBox();
            this.cbTotal = new System.Windows.Forms.CheckBox();
            this.rbCm = new System.Windows.Forms.RadioButton();
            this.rbInch = new System.Windows.Forms.RadioButton();
            this.tbCursorDielectric = new System.Windows.Forms.TextBox();
            this.pictColorDielectric = new System.Windows.Forms.PictureBox();
            this.btnPrint = new System.Windows.Forms.Button();
            this.btnCopy = new System.Windows.Forms.Button();
            this.btnClose = new System.Windows.Forms.Button();
            this.tbComment = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label6 = new System.Windows.Forms.Label();
            this.container = new System.Windows.Forms.Panel();
            this.lFieldSolver = new System.Windows.Forms.Label();
            this.lSandbox = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.chartLoss)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorCuRoughness)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorResistive)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorTotal)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorDielectric)).BeginInit();
            this.panel1.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel3.SuspendLayout();
            this.container.SuspendLayout();
            this.SuspendLayout();
            // 
            // chartLoss
            // 
            this.chartLoss.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            chartArea1.Name = "chartAreaLoss";
            this.chartLoss.ChartAreas.Add(chartArea1);
            this.chartLoss.Location = new System.Drawing.Point(8, 53);
            this.chartLoss.Margin = new System.Windows.Forms.Padding(2);
            this.chartLoss.Name = "chartLoss";
            this.chartLoss.Size = new System.Drawing.Size(426, 372);
            this.chartLoss.TabIndex = 0;
            this.chartLoss.Text = "chart1";
            this.chartLoss.Click += new System.EventHandler(this.chartLoss_Click);
            this.chartLoss.MouseMove += new System.Windows.Forms.MouseEventHandler(this.chart1_MouseMove);
            // 
            // label57
            // 
            this.label57.Location = new System.Drawing.Point(224, 174);
            this.label57.Name = "label57";
            this.label57.Size = new System.Drawing.Size(39, 22);
            this.label57.TabIndex = 14;
            this.label57.Text = "GHz";
            // 
            // tbDominates
            // 
            this.tbDominates.BackColor = System.Drawing.SystemColors.Control;
            this.tbDominates.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbDominates.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbDominates.Location = new System.Drawing.Point(178, 172);
            this.tbDominates.Name = "tbDominates";
            this.tbDominates.ReadOnly = true;
            this.tbDominates.Size = new System.Drawing.Size(40, 19);
            this.tbDominates.TabIndex = 13;
            this.tbDominates.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // label56
            // 
            this.label56.Location = new System.Drawing.Point(8, 174);
            this.label56.Name = "label56";
            this.label56.Size = new System.Drawing.Size(178, 22);
            this.label56.TabIndex = 12;
            this.label56.Text = "Dielectric loss dominates at:";
            // 
            // label54
            // 
            this.label54.Location = new System.Drawing.Point(98, 57);
            this.label54.Name = "label54";
            this.label54.Size = new System.Drawing.Size(49, 14);
            this.label54.TabIndex = 5;
            this.label54.Text = "GHz";
            // 
            // tbFmax
            // 
            this.tbFmax.Location = new System.Drawing.Point(55, 55);
            this.tbFmax.Name = "tbFmax";
            this.tbFmax.Size = new System.Drawing.Size(40, 20);
            this.tbFmax.TabIndex = 4;
            this.tbFmax.Text = "11";
            this.tbFmax.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tbFmax.Leave += new System.EventHandler(this.tbFmax_Leave);
            // 
            // label55
            // 
            this.label55.Location = new System.Drawing.Point(17, 57);
            this.label55.Name = "label55";
            this.label55.Size = new System.Drawing.Size(37, 14);
            this.label55.TabIndex = 3;
            this.label55.Text = "Max:";
            this.label55.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // label53
            // 
            this.label53.Location = new System.Drawing.Point(98, 31);
            this.label53.Name = "label53";
            this.label53.Size = new System.Drawing.Size(49, 15);
            this.label53.TabIndex = 2;
            this.label53.Text = "GHz";
            // 
            // tbFmin
            // 
            this.tbFmin.Location = new System.Drawing.Point(55, 28);
            this.tbFmin.Name = "tbFmin";
            this.tbFmin.Size = new System.Drawing.Size(40, 20);
            this.tbFmin.TabIndex = 1;
            this.tbFmin.Text = "0";
            this.tbFmin.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tbFmin.Leave += new System.EventHandler(this.tbFmin_Leave);
            // 
            // label52
            // 
            this.label52.Location = new System.Drawing.Point(17, 31);
            this.label52.Name = "label52";
            this.label52.Size = new System.Drawing.Size(37, 15);
            this.label52.TabIndex = 0;
            this.label52.Text = "Min:";
            this.label52.TextAlign = System.Drawing.ContentAlignment.TopRight;
            // 
            // tbCursorCuRoughness
            // 
            this.tbCursorCuRoughness.BackColor = System.Drawing.SystemColors.Control;
            this.tbCursorCuRoughness.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCursorCuRoughness.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbCursorCuRoughness.Location = new System.Drawing.Point(16, 92);
            this.tbCursorCuRoughness.Name = "tbCursorCuRoughness";
            this.tbCursorCuRoughness.ReadOnly = true;
            this.tbCursorCuRoughness.Size = new System.Drawing.Size(29, 12);
            this.tbCursorCuRoughness.TabIndex = 22;
            this.tbCursorCuRoughness.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // pictColorCuRoughness
            // 
            this.pictColorCuRoughness.BackColor = System.Drawing.Color.Orange;
            this.pictColorCuRoughness.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictColorCuRoughness.Location = new System.Drawing.Point(56, 91);
            this.pictColorCuRoughness.Name = "pictColorCuRoughness";
            this.pictColorCuRoughness.Size = new System.Drawing.Size(15, 15);
            this.pictColorCuRoughness.TabIndex = 21;
            this.pictColorCuRoughness.TabStop = false;
            this.pictColorCuRoughness.Click += new System.EventHandler(this.pictColorCuRoughness_Click);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(5, 5);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(62, 18);
            this.label3.TabIndex = 20;
            this.label3.Text = "Cursor:";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(53, 117);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(28, 13);
            this.label1.TabIndex = 19;
            this.label1.Text = "GHz";
            // 
            // tbCursorX
            // 
            this.tbCursorX.BackColor = System.Drawing.SystemColors.Control;
            this.tbCursorX.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCursorX.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbCursorX.Location = new System.Drawing.Point(7, 117);
            this.tbCursorX.Name = "tbCursorX";
            this.tbCursorX.ReadOnly = true;
            this.tbCursorX.Size = new System.Drawing.Size(38, 12);
            this.tbCursorX.TabIndex = 17;
            this.tbCursorX.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tbCursorResistive
            // 
            this.tbCursorResistive.BackColor = System.Drawing.SystemColors.Control;
            this.tbCursorResistive.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCursorResistive.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbCursorResistive.Location = new System.Drawing.Point(16, 70);
            this.tbCursorResistive.Name = "tbCursorResistive";
            this.tbCursorResistive.ReadOnly = true;
            this.tbCursorResistive.Size = new System.Drawing.Size(29, 12);
            this.tbCursorResistive.TabIndex = 15;
            this.tbCursorResistive.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tbCursorTotal
            // 
            this.tbCursorTotal.BackColor = System.Drawing.SystemColors.Control;
            this.tbCursorTotal.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCursorTotal.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbCursorTotal.Location = new System.Drawing.Point(16, 26);
            this.tbCursorTotal.Name = "tbCursorTotal";
            this.tbCursorTotal.ReadOnly = true;
            this.tbCursorTotal.Size = new System.Drawing.Size(29, 12);
            this.tbCursorTotal.TabIndex = 14;
            this.tbCursorTotal.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // pictColorResistive
            // 
            this.pictColorResistive.BackColor = System.Drawing.Color.Red;
            this.pictColorResistive.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictColorResistive.Location = new System.Drawing.Point(56, 70);
            this.pictColorResistive.Name = "pictColorResistive";
            this.pictColorResistive.Size = new System.Drawing.Size(15, 15);
            this.pictColorResistive.TabIndex = 8;
            this.pictColorResistive.TabStop = false;
            this.pictColorResistive.Click += new System.EventHandler(this.pictColorResistive_Click);
            // 
            // pictColorTotal
            // 
            this.pictColorTotal.BackColor = System.Drawing.Color.Blue;
            this.pictColorTotal.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictColorTotal.Location = new System.Drawing.Point(56, 25);
            this.pictColorTotal.Name = "pictColorTotal";
            this.pictColorTotal.Size = new System.Drawing.Size(15, 15);
            this.pictColorTotal.TabIndex = 7;
            this.pictColorTotal.TabStop = false;
            this.pictColorTotal.Click += new System.EventHandler(this.pictColorTotal_Click);
            // 
            // cbRoughness
            // 
            this.cbRoughness.AutoSize = true;
            this.cbRoughness.Location = new System.Drawing.Point(80, 91);
            this.cbRoughness.Name = "cbRoughness";
            this.cbRoughness.Size = new System.Drawing.Size(112, 17);
            this.cbRoughness.TabIndex = 5;
            this.cbRoughness.Text = "Copper roughness";
            this.cbRoughness.UseVisualStyleBackColor = true;
            this.cbRoughness.CheckedChanged += new System.EventHandler(this.cbRoughness_CheckedChanged);
            // 
            // cbDielectric
            // 
            this.cbDielectric.AutoSize = true;
            this.cbDielectric.Checked = true;
            this.cbDielectric.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbDielectric.Location = new System.Drawing.Point(80, 47);
            this.cbDielectric.Name = "cbDielectric";
            this.cbDielectric.Size = new System.Drawing.Size(70, 17);
            this.cbDielectric.TabIndex = 4;
            this.cbDielectric.Text = "Dielectric";
            this.cbDielectric.UseVisualStyleBackColor = true;
            this.cbDielectric.CheckedChanged += new System.EventHandler(this.cbDielectric_CheckedChanged);
            // 
            // cbResistive
            // 
            this.cbResistive.AutoSize = true;
            this.cbResistive.Checked = true;
            this.cbResistive.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbResistive.Location = new System.Drawing.Point(80, 69);
            this.cbResistive.Name = "cbResistive";
            this.cbResistive.Size = new System.Drawing.Size(77, 17);
            this.cbResistive.TabIndex = 3;
            this.cbResistive.Text = "Skin effect";
            this.cbResistive.UseVisualStyleBackColor = true;
            this.cbResistive.CheckedChanged += new System.EventHandler(this.cbResistive_CheckedChanged);
            // 
            // cbTotal
            // 
            this.cbTotal.AutoSize = true;
            this.cbTotal.Checked = true;
            this.cbTotal.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbTotal.Location = new System.Drawing.Point(80, 25);
            this.cbTotal.Name = "cbTotal";
            this.cbTotal.Size = new System.Drawing.Size(106, 17);
            this.cbTotal.TabIndex = 2;
            this.cbTotal.Text = "Total loss [dB/in]";
            this.cbTotal.UseVisualStyleBackColor = true;
            this.cbTotal.CheckedChanged += new System.EventHandler(this.cbTotal_CheckedChanged);
            // 
            // rbCm
            // 
            this.rbCm.AutoSize = true;
            this.rbCm.Location = new System.Drawing.Point(16, 56);
            this.rbCm.Name = "rbCm";
            this.rbCm.Size = new System.Drawing.Size(57, 17);
            this.rbCm.TabIndex = 1;
            this.rbCm.Text = "dB/cm";
            this.rbCm.UseVisualStyleBackColor = true;
            this.rbCm.CheckedChanged += new System.EventHandler(this.rbCm_CheckedChanged);
            // 
            // rbInch
            // 
            this.rbInch.AutoSize = true;
            this.rbInch.Checked = true;
            this.rbInch.Location = new System.Drawing.Point(16, 31);
            this.rbInch.Name = "rbInch";
            this.rbInch.Size = new System.Drawing.Size(51, 17);
            this.rbInch.TabIndex = 0;
            this.rbInch.TabStop = true;
            this.rbInch.Text = "dB/in";
            this.rbInch.UseVisualStyleBackColor = true;
            this.rbInch.CheckedChanged += new System.EventHandler(this.rbInch_CheckedChanged);
            // 
            // tbCursorDielectric
            // 
            this.tbCursorDielectric.BackColor = System.Drawing.SystemColors.Control;
            this.tbCursorDielectric.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbCursorDielectric.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbCursorDielectric.Location = new System.Drawing.Point(16, 48);
            this.tbCursorDielectric.Name = "tbCursorDielectric";
            this.tbCursorDielectric.ReadOnly = true;
            this.tbCursorDielectric.Size = new System.Drawing.Size(29, 12);
            this.tbCursorDielectric.TabIndex = 16;
            this.tbCursorDielectric.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // pictColorDielectric
            // 
            this.pictColorDielectric.BackColor = System.Drawing.Color.Green;
            this.pictColorDielectric.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pictColorDielectric.Location = new System.Drawing.Point(56, 48);
            this.pictColorDielectric.Name = "pictColorDielectric";
            this.pictColorDielectric.Size = new System.Drawing.Size(15, 15);
            this.pictColorDielectric.TabIndex = 9;
            this.pictColorDielectric.TabStop = false;
            this.pictColorDielectric.Click += new System.EventHandler(this.pictColorDielectric_Click);
            // 
            // btnPrint
            // 
            this.btnPrint.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPrint.Location = new System.Drawing.Point(543, 437);
            this.btnPrint.Name = "btnPrint";
            this.btnPrint.Size = new System.Drawing.Size(82, 26);
            this.btnPrint.TabIndex = 15;
            this.btnPrint.Text = "Print";
            this.btnPrint.UseVisualStyleBackColor = true;
            this.btnPrint.Click += new System.EventHandler(this.btnPrint_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCopy.Location = new System.Drawing.Point(444, 437);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(82, 26);
            this.btnCopy.TabIndex = 16;
            this.btnCopy.Text = "Copy";
            this.btnCopy.UseVisualStyleBackColor = true;
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // btnClose
            // 
            this.btnClose.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnClose.Location = new System.Drawing.Point(642, 437);
            this.btnClose.Name = "btnClose";
            this.btnClose.Size = new System.Drawing.Size(82, 26);
            this.btnClose.TabIndex = 17;
            this.btnClose.Text = "Close";
            this.btnClose.UseVisualStyleBackColor = true;
            this.btnClose.Click += new System.EventHandler(this.btnClose_Click);
            // 
            // tbComment
            // 
            this.tbComment.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tbComment.BackColor = System.Drawing.SystemColors.Window;
            this.tbComment.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.tbComment.Location = new System.Drawing.Point(8, 24);
            this.tbComment.Name = "tbComment";
            this.tbComment.Size = new System.Drawing.Size(716, 20);
            this.tbComment.TabIndex = 19;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(8, 7);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(162, 14);
            this.label2.TabIndex = 20;
            this.label2.Text = "Comment for clipboard or printer:";
            // 
            // panel1
            // 
            this.panel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panel1.Controls.Add(this.label4);
            this.panel1.Controls.Add(this.rbInch);
            this.panel1.Controls.Add(this.rbCm);
            this.panel1.Location = new System.Drawing.Point(441, 53);
            this.panel1.Margin = new System.Windows.Forms.Padding(2);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(283, 81);
            this.panel1.TabIndex = 21;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(13, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(130, 14);
            this.label4.TabIndex = 23;
            this.label4.Text = "Vertical axis (Attenuation):";
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.label7);
            this.panel2.Controls.Add(this.label1);
            this.panel2.Controls.Add(this.label5);
            this.panel2.Controls.Add(this.tbCursorResistive);
            this.panel2.Controls.Add(this.cbDielectric);
            this.panel2.Controls.Add(this.pictColorDielectric);
            this.panel2.Controls.Add(this.tbCursorX);
            this.panel2.Controls.Add(this.label3);
            this.panel2.Controls.Add(this.tbCursorCuRoughness);
            this.panel2.Controls.Add(this.cbTotal);
            this.panel2.Controls.Add(this.pictColorCuRoughness);
            this.panel2.Controls.Add(this.cbResistive);
            this.panel2.Controls.Add(this.tbCursorTotal);
            this.panel2.Controls.Add(this.tbCursorDielectric);
            this.panel2.Controls.Add(this.cbRoughness);
            this.panel2.Controls.Add(this.label57);
            this.panel2.Controls.Add(this.tbDominates);
            this.panel2.Controls.Add(this.pictColorTotal);
            this.panel2.Controls.Add(this.label56);
            this.panel2.Controls.Add(this.pictColorResistive);
            this.panel2.Location = new System.Drawing.Point(441, 139);
            this.panel2.Margin = new System.Windows.Forms.Padding(2);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(283, 201);
            this.panel2.TabIndex = 22;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(8, 146);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(248, 23);
            this.label7.TabIndex = 24;
            this.label7.Text = "Click on graph to toggle cursor location";
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(76, 5);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(49, 18);
            this.label5.TabIndex = 23;
            this.label5.Text = "Show:";
            // 
            // panel3
            // 
            this.panel3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panel3.Controls.Add(this.label54);
            this.panel3.Controls.Add(this.label6);
            this.panel3.Controls.Add(this.tbFmax);
            this.panel3.Controls.Add(this.tbFmin);
            this.panel3.Controls.Add(this.label55);
            this.panel3.Controls.Add(this.label52);
            this.panel3.Controls.Add(this.label53);
            this.panel3.Location = new System.Drawing.Point(441, 344);
            this.panel3.Margin = new System.Windows.Forms.Padding(2);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(283, 81);
            this.panel3.TabIndex = 23;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(5, 7);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(139, 14);
            this.label6.TabIndex = 24;
            this.label6.Text = "Horizontal axis (Frequency):";
            // 
            // container
            // 
            this.container.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.container.Controls.Add(this.lFieldSolver);
            this.container.Controls.Add(this.lSandbox);
            this.container.Controls.Add(this.panel3);
            this.container.Controls.Add(this.panel2);
            this.container.Controls.Add(this.panel1);
            this.container.Controls.Add(this.label2);
            this.container.Controls.Add(this.tbComment);
            this.container.Controls.Add(this.chartLoss);
            this.container.Location = new System.Drawing.Point(0, 0);
            this.container.Name = "container";
            this.container.Size = new System.Drawing.Size(731, 434);
            this.container.TabIndex = 24;
            // 
            // lFieldSolver
            // 
            this.lFieldSolver.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lFieldSolver.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lFieldSolver.Location = new System.Drawing.Point(441, 1);
            this.lFieldSolver.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.lFieldSolver.Name = "lFieldSolver";
            this.lFieldSolver.Size = new System.Drawing.Size(283, 19);
            this.lFieldSolver.TabIndex = 33;
            this.lFieldSolver.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // lSandbox
            // 
            this.lSandbox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lSandbox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lSandbox.Location = new System.Drawing.Point(213, 0);
            this.lSandbox.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.lSandbox.Name = "lSandbox";
            this.lSandbox.Size = new System.Drawing.Size(221, 19);
            this.lSandbox.TabIndex = 32;
            this.lSandbox.Text = "Z-zero";
            this.lSandbox.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // LossPlot
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(731, 471);
            this.Controls.Add(this.container);
            this.Controls.Add(this.btnClose);
            this.Controls.Add(this.btnPrint);
            this.Controls.Add(this.btnCopy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(747, 509);
            this.Name = "LossPlot";
            this.ShowInTaskbar = false;
            this.Text = "  Loss Viewer";
            ((System.ComponentModel.ISupportInitialize)(this.chartLoss)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorCuRoughness)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorResistive)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorTotal)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictColorDielectric)).EndInit();
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel3.PerformLayout();
            this.container.ResumeLayout(false);
            this.container.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart chartLoss;
        private System.Windows.Forms.Label label57;
        private System.Windows.Forms.TextBox tbDominates;
        private System.Windows.Forms.Label label56;
        private System.Windows.Forms.Label label54;
        private System.Windows.Forms.TextBox tbFmax;
        private System.Windows.Forms.Label label55;
        private System.Windows.Forms.Label label53;
        private System.Windows.Forms.TextBox tbFmin;
        private System.Windows.Forms.Label label52;
        private System.Windows.Forms.PictureBox pictColorDielectric;
        private System.Windows.Forms.PictureBox pictColorResistive;
        private System.Windows.Forms.PictureBox pictColorTotal;
        private System.Windows.Forms.CheckBox cbRoughness;
        private System.Windows.Forms.CheckBox cbDielectric;
        private System.Windows.Forms.CheckBox cbResistive;
        private System.Windows.Forms.CheckBox cbTotal;
        private System.Windows.Forms.RadioButton rbCm;
        private System.Windows.Forms.RadioButton rbInch;
        private System.Windows.Forms.TextBox tbCursorDielectric;
        private System.Windows.Forms.TextBox tbCursorResistive;
        private System.Windows.Forms.TextBox tbCursorTotal;
        private System.Windows.Forms.TextBox tbCursorX;
        private System.Windows.Forms.Button btnPrint;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbCursorCuRoughness;
        private System.Windows.Forms.PictureBox pictColorCuRoughness;
        private System.Windows.Forms.TextBox tbComment;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Panel container;
        private System.Windows.Forms.Label lSandbox;
        private System.Windows.Forms.Label lFieldSolver;
    }
}