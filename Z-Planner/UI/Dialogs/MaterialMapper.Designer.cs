namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class MaterialMapper
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
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MaterialMapper));
            this.chartMapper = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.btnPrint = new System.Windows.Forms.Button();
            this.btnCopy = new System.Windows.Forms.Button();
            this.btnClose = new System.Windows.Forms.Button();
            this.tbComment = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.cbY = new System.Windows.Forms.ComboBox();
            this.cbX = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.chartRadar = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.tbMaterial = new System.Windows.Forms.TextBox();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPagePoints = new System.Windows.Forms.TabPage();
            this.panelMapper = new System.Windows.Forms.Panel();
            this.tabPageRadar = new System.Windows.Forms.TabPage();
            this.panelRadial = new System.Windows.Forms.Panel();
            this.gbAxis = new System.Windows.Forms.GroupBox();
            this.rbBest = new System.Windows.Forms.RadioButton();
            this.rbOriginal = new System.Windows.Forms.RadioButton();
            this.gbStyle = new System.Windows.Forms.GroupBox();
            this.rbArea = new System.Windows.Forms.RadioButton();
            this.rbLine = new System.Windows.Forms.RadioButton();
            this.lbRadarAxis = new System.Windows.Forms.CheckedListBox();
            this.label5 = new System.Windows.Forms.Label();
            this.tabPageFrequency = new System.Windows.Forms.TabPage();
            this.panelDkDf = new System.Windows.Forms.Panel();
            this.chartFrequency = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.cbLabels = new System.Windows.Forms.CheckBox();
            this.rbDf = new System.Windows.Forms.RadioButton();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.rbDk = new System.Windows.Forms.RadioButton();
            this.panelMaterialTable = new System.Windows.Forms.Panel();
            this.tblMaterialPanel = new System.Windows.Forms.TableLayoutPanel();
            this.tbFrequency = new System.Windows.Forms.TextBox();
            this.tbLabel1 = new System.Windows.Forms.TextBox();
            this.tbLabel2 = new System.Windows.Forms.TextBox();
            this.btnCheck = new System.Windows.Forms.Button();
            this.btnUncheck = new System.Windows.Forms.Button();
            this.container = new System.Windows.Forms.Panel();
            this.cbColorByMnaufacturer = new System.Windows.Forms.CheckBox();
            this.btnColorByManufacturer = new System.Windows.Forms.Button();
            this.panelCursor = new System.Windows.Forms.Panel();
            this.tbChartFreq = new System.Windows.Forms.TextBox();
            this.textBox2 = new System.Windows.Forms.TextBox();
            this.textBox4 = new System.Windows.Forms.TextBox();
            this.tbResin = new System.Windows.Forms.TextBox();
            this.lSandbox = new System.Windows.Forms.Label();
            this.panelFrequency = new System.Windows.Forms.Panel();
            this.btnExport = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.chartMapper)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartRadar)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPagePoints.SuspendLayout();
            this.panelMapper.SuspendLayout();
            this.tabPageRadar.SuspendLayout();
            this.panelRadial.SuspendLayout();
            this.gbAxis.SuspendLayout();
            this.gbStyle.SuspendLayout();
            this.tabPageFrequency.SuspendLayout();
            this.panelDkDf.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.chartFrequency)).BeginInit();
            this.panelMaterialTable.SuspendLayout();
            this.container.SuspendLayout();
            this.panelCursor.SuspendLayout();
            this.panelFrequency.SuspendLayout();
            this.SuspendLayout();
            // 
            // chartMapper
            // 
            this.chartMapper.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.chartMapper.BorderlineColor = System.Drawing.Color.Black;
            this.chartMapper.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea1.AxisX.IsLabelAutoFit = false;
            chartArea1.Name = "chartAreaMapper";
            this.chartMapper.ChartAreas.Add(chartArea1);
            this.chartMapper.Location = new System.Drawing.Point(0, 0);
            this.chartMapper.Name = "chartMapper";
            this.chartMapper.Size = new System.Drawing.Size(501, 535);
            this.chartMapper.TabIndex = 0;
            this.chartMapper.Text = "chart1";
            // 
            // btnPrint
            // 
            this.btnPrint.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPrint.Location = new System.Drawing.Point(840, 654);
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
            this.btnCopy.Location = new System.Drawing.Point(741, 654);
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
            this.btnClose.Location = new System.Drawing.Point(939, 654);
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
            this.tbComment.Size = new System.Drawing.Size(1011, 20);
            this.tbComment.TabIndex = 19;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(8, 4);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(236, 17);
            this.label2.TabIndex = 20;
            this.label2.Text = "Comment for clipboard or printer:";
            // 
            // label4
            // 
            this.label4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label4.Location = new System.Drawing.Point(536, 43);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(130, 14);
            this.label4.TabIndex = 24;
            this.label4.Text = "Vertical axis ";
            // 
            // label6
            // 
            this.label6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label6.Location = new System.Drawing.Point(536, 138);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(139, 14);
            this.label6.TabIndex = 25;
            this.label6.Text = "Horizontal axis ";
            // 
            // cbY
            // 
            this.cbY.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cbY.BackColor = System.Drawing.SystemColors.Window;
            this.cbY.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbY.FormattingEnabled = true;
            this.cbY.Location = new System.Drawing.Point(536, 69);
            this.cbY.Margin = new System.Windows.Forms.Padding(2);
            this.cbY.Name = "cbY";
            this.cbY.Size = new System.Drawing.Size(127, 21);
            this.cbY.TabIndex = 28;
            this.cbY.SelectedIndexChanged += new System.EventHandler(this.cbY_SelectedIndexChanged);
            // 
            // cbX
            // 
            this.cbX.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cbX.BackColor = System.Drawing.SystemColors.Window;
            this.cbX.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cbX.FormattingEnabled = true;
            this.cbX.Location = new System.Drawing.Point(536, 165);
            this.cbX.Margin = new System.Windows.Forms.Padding(2);
            this.cbX.Name = "cbX";
            this.cbX.Size = new System.Drawing.Size(127, 21);
            this.cbX.TabIndex = 29;
            this.cbX.SelectedIndexChanged += new System.EventHandler(this.cbX_SelectedIndexChanged);
            // 
            // label3
            // 
            this.label3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label3.Location = new System.Drawing.Point(736, 63);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(130, 14);
            this.label3.TabIndex = 31;
            this.label3.Text = "Material List";
            // 
            // chartRadar
            // 
            this.chartRadar.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.chartRadar.BorderlineColor = System.Drawing.Color.Black;
            this.chartRadar.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea2.AxisY.LabelStyle.Enabled = false;
            chartArea2.Name = "ChartAreaRadar";
            this.chartRadar.ChartAreas.Add(chartArea2);
            this.chartRadar.Location = new System.Drawing.Point(0, 0);
            this.chartRadar.Name = "chartRadar";
            this.chartRadar.Size = new System.Drawing.Size(501, 535);
            this.chartRadar.TabIndex = 0;
            this.chartRadar.Text = "chart1";
            // 
            // tbMaterial
            // 
            this.tbMaterial.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbMaterial.Location = new System.Drawing.Point(736, 486);
            this.tbMaterial.Multiline = true;
            this.tbMaterial.Name = "tbMaterial";
            this.tbMaterial.ReadOnly = true;
            this.tbMaterial.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.tbMaterial.Size = new System.Drawing.Size(283, 120);
            this.tbMaterial.TabIndex = 34;
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPagePoints);
            this.tabControl1.Controls.Add(this.tabPageRadar);
            this.tabControl1.Controls.Add(this.tabPageFrequency);
            this.tabControl1.Location = new System.Drawing.Point(12, 63);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(712, 573);
            this.tabControl1.TabIndex = 35;
            this.tabControl1.SelectedIndexChanged += new System.EventHandler(this.tabControl1_SelectedIndexChanged);
            // 
            // tabPagePoints
            // 
            this.tabPagePoints.Controls.Add(this.panelMapper);
            this.tabPagePoints.Controls.Add(this.label4);
            this.tabPagePoints.Controls.Add(this.label6);
            this.tabPagePoints.Controls.Add(this.cbX);
            this.tabPagePoints.Controls.Add(this.cbY);
            this.tabPagePoints.Location = new System.Drawing.Point(4, 22);
            this.tabPagePoints.Name = "tabPagePoints";
            this.tabPagePoints.Padding = new System.Windows.Forms.Padding(3);
            this.tabPagePoints.Size = new System.Drawing.Size(704, 547);
            this.tabPagePoints.TabIndex = 0;
            this.tabPagePoints.Text = "XY Plot";
            this.tabPagePoints.UseVisualStyleBackColor = true;
            // 
            // panelMapper
            // 
            this.panelMapper.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelMapper.Controls.Add(this.chartMapper);
            this.panelMapper.Location = new System.Drawing.Point(5, 5);
            this.panelMapper.Margin = new System.Windows.Forms.Padding(2);
            this.panelMapper.Name = "panelMapper";
            this.panelMapper.Size = new System.Drawing.Size(501, 535);
            this.panelMapper.TabIndex = 0;
            // 
            // tabPageRadar
            // 
            this.tabPageRadar.Controls.Add(this.panelRadial);
            this.tabPageRadar.Controls.Add(this.gbAxis);
            this.tabPageRadar.Controls.Add(this.gbStyle);
            this.tabPageRadar.Controls.Add(this.lbRadarAxis);
            this.tabPageRadar.Controls.Add(this.label5);
            this.tabPageRadar.Location = new System.Drawing.Point(4, 22);
            this.tabPageRadar.Name = "tabPageRadar";
            this.tabPageRadar.Padding = new System.Windows.Forms.Padding(3);
            this.tabPageRadar.Size = new System.Drawing.Size(704, 547);
            this.tabPageRadar.TabIndex = 1;
            this.tabPageRadar.Text = "Radial Plot";
            this.tabPageRadar.UseVisualStyleBackColor = true;
            // 
            // panelRadial
            // 
            this.panelRadial.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelRadial.Controls.Add(this.chartRadar);
            this.panelRadial.Location = new System.Drawing.Point(5, 5);
            this.panelRadial.Margin = new System.Windows.Forms.Padding(2);
            this.panelRadial.Name = "panelRadial";
            this.panelRadial.Size = new System.Drawing.Size(501, 535);
            this.panelRadial.TabIndex = 0;
            // 
            // gbAxis
            // 
            this.gbAxis.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.gbAxis.Controls.Add(this.rbBest);
            this.gbAxis.Controls.Add(this.rbOriginal);
            this.gbAxis.Location = new System.Drawing.Point(520, 204);
            this.gbAxis.Name = "gbAxis";
            this.gbAxis.Size = new System.Drawing.Size(163, 107);
            this.gbAxis.TabIndex = 37;
            this.gbAxis.TabStop = false;
            this.gbAxis.Text = "Axis orientation";
            // 
            // rbBest
            // 
            this.rbBest.AutoSize = true;
            this.rbBest.Checked = true;
            this.rbBest.Location = new System.Drawing.Point(11, 49);
            this.rbBest.Name = "rbBest";
            this.rbBest.Size = new System.Drawing.Size(117, 17);
            this.rbBest.TabIndex = 1;
            this.rbBest.TabStop = true;
            this.rbBest.Text = "\"Best\" on perimeter";
            this.rbBest.UseVisualStyleBackColor = true;
            this.rbBest.CheckedChanged += new System.EventHandler(this.rbBest_CheckedChanged);
            // 
            // rbOriginal
            // 
            this.rbOriginal.AutoSize = true;
            this.rbOriginal.Location = new System.Drawing.Point(11, 22);
            this.rbOriginal.Name = "rbOriginal";
            this.rbOriginal.Size = new System.Drawing.Size(84, 17);
            this.rbOriginal.TabIndex = 0;
            this.rbOriginal.Text = "Original data";
            this.rbOriginal.UseVisualStyleBackColor = true;
            this.rbOriginal.CheckedChanged += new System.EventHandler(this.rbOriginal_CheckedChanged);
            // 
            // gbStyle
            // 
            this.gbStyle.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.gbStyle.Controls.Add(this.rbArea);
            this.gbStyle.Controls.Add(this.rbLine);
            this.gbStyle.Location = new System.Drawing.Point(520, 324);
            this.gbStyle.Name = "gbStyle";
            this.gbStyle.Size = new System.Drawing.Size(163, 83);
            this.gbStyle.TabIndex = 36;
            this.gbStyle.TabStop = false;
            this.gbStyle.Text = "Style";
            // 
            // rbArea
            // 
            this.rbArea.AutoSize = true;
            this.rbArea.Location = new System.Drawing.Point(11, 49);
            this.rbArea.Name = "rbArea";
            this.rbArea.Size = new System.Drawing.Size(47, 17);
            this.rbArea.TabIndex = 1;
            this.rbArea.Text = "Area";
            this.rbArea.UseVisualStyleBackColor = true;
            this.rbArea.CheckedChanged += new System.EventHandler(this.rbArea_CheckedChanged);
            // 
            // rbLine
            // 
            this.rbLine.AutoSize = true;
            this.rbLine.Checked = true;
            this.rbLine.Location = new System.Drawing.Point(11, 22);
            this.rbLine.Name = "rbLine";
            this.rbLine.Size = new System.Drawing.Size(45, 17);
            this.rbLine.TabIndex = 0;
            this.rbLine.TabStop = true;
            this.rbLine.Text = "Line";
            this.rbLine.UseVisualStyleBackColor = true;
            this.rbLine.CheckedChanged += new System.EventHandler(this.rbLine_CheckedChanged);
            // 
            // lbRadarAxis
            // 
            this.lbRadarAxis.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lbRadarAxis.FormattingEnabled = true;
            this.lbRadarAxis.Location = new System.Drawing.Point(520, 34);
            this.lbRadarAxis.Name = "lbRadarAxis";
            this.lbRadarAxis.Size = new System.Drawing.Size(163, 109);
            this.lbRadarAxis.TabIndex = 35;
            this.lbRadarAxis.ItemCheck += new System.Windows.Forms.ItemCheckEventHandler(this.lbRadarAxis_ItemCheck);
            this.lbRadarAxis.SelectedIndexChanged += new System.EventHandler(this.lbRadarAxis_SelectedIndexChanged);
            // 
            // label5
            // 
            this.label5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.label5.Location = new System.Drawing.Point(520, 6);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(130, 14);
            this.label5.TabIndex = 34;
            this.label5.Text = "Axis";
            // 
            // tabPageFrequency
            // 
            this.tabPageFrequency.Controls.Add(this.panelDkDf);
            this.tabPageFrequency.Controls.Add(this.cbLabels);
            this.tabPageFrequency.Controls.Add(this.rbDf);
            this.tabPageFrequency.Controls.Add(this.textBox1);
            this.tabPageFrequency.Controls.Add(this.rbDk);
            this.tabPageFrequency.Location = new System.Drawing.Point(4, 22);
            this.tabPageFrequency.Name = "tabPageFrequency";
            this.tabPageFrequency.Size = new System.Drawing.Size(704, 547);
            this.tabPageFrequency.TabIndex = 2;
            this.tabPageFrequency.Text = "Dk, Df  (f)";
            this.tabPageFrequency.UseVisualStyleBackColor = true;
            // 
            // panelDkDf
            // 
            this.panelDkDf.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelDkDf.Controls.Add(this.chartFrequency);
            this.panelDkDf.Location = new System.Drawing.Point(5, 35);
            this.panelDkDf.Margin = new System.Windows.Forms.Padding(2);
            this.panelDkDf.Name = "panelDkDf";
            this.panelDkDf.Size = new System.Drawing.Size(697, 506);
            this.panelDkDf.TabIndex = 1;
            // 
            // chartFrequency
            // 
            this.chartFrequency.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.chartFrequency.BorderlineColor = System.Drawing.Color.Black;
            this.chartFrequency.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea3.AxisX.IsLabelAutoFit = false;
            chartArea3.Name = "chartAreaMapper";
            this.chartFrequency.ChartAreas.Add(chartArea3);
            this.chartFrequency.Location = new System.Drawing.Point(0, 0);
            this.chartFrequency.Margin = new System.Windows.Forms.Padding(2);
            this.chartFrequency.Name = "chartFrequency";
            this.chartFrequency.Size = new System.Drawing.Size(697, 506);
            this.chartFrequency.TabIndex = 1;
            this.chartFrequency.Text = "chart1";
            this.chartFrequency.Click += new System.EventHandler(this.chartFrequency_Click);
            this.chartFrequency.MouseLeave += new System.EventHandler(this.chartFrequency_MouseLeave);
            this.chartFrequency.MouseMove += new System.Windows.Forms.MouseEventHandler(this.chartFrequency_MouseMove);
            // 
            // cbLabels
            // 
            this.cbLabels.AutoSize = true;
            this.cbLabels.Checked = true;
            this.cbLabels.CheckState = System.Windows.Forms.CheckState.Checked;
            this.cbLabels.Location = new System.Drawing.Point(134, 10);
            this.cbLabels.Name = "cbLabels";
            this.cbLabels.Size = new System.Drawing.Size(96, 17);
            this.cbLabels.TabIndex = 41;
            this.cbLabels.Text = "Show all labels";
            this.cbLabels.UseVisualStyleBackColor = true;
            this.cbLabels.CheckedChanged += new System.EventHandler(this.cbLabels_CheckedChanged);
            // 
            // rbDf
            // 
            this.rbDf.AutoSize = true;
            this.rbDf.Location = new System.Drawing.Point(56, 9);
            this.rbDf.Name = "rbDf";
            this.rbDf.Size = new System.Drawing.Size(36, 17);
            this.rbDf.TabIndex = 1;
            this.rbDf.Text = "Df";
            this.rbDf.UseVisualStyleBackColor = true;
            this.rbDf.CheckedChanged += new System.EventHandler(this.rbDf_CheckedChanged);
            // 
            // textBox1
            // 
            this.textBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox1.BackColor = System.Drawing.SystemColors.Window;
            this.textBox1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox1.Location = new System.Drawing.Point(452, 10);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(249, 23);
            this.textBox1.TabIndex = 40;
            this.textBox1.TabStop = false;
            this.textBox1.Text = "Click on graph to toggle cursor location";
            // 
            // rbDk
            // 
            this.rbDk.AutoSize = true;
            this.rbDk.Checked = true;
            this.rbDk.Location = new System.Drawing.Point(6, 9);
            this.rbDk.Name = "rbDk";
            this.rbDk.Size = new System.Drawing.Size(39, 17);
            this.rbDk.TabIndex = 0;
            this.rbDk.TabStop = true;
            this.rbDk.Text = "Dk";
            this.rbDk.UseVisualStyleBackColor = true;
            this.rbDk.CheckedChanged += new System.EventHandler(this.rbDk_CheckedChanged);
            // 
            // panelMaterialTable
            // 
            this.panelMaterialTable.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panelMaterialTable.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panelMaterialTable.Controls.Add(this.tblMaterialPanel);
            this.panelMaterialTable.Location = new System.Drawing.Point(732, 83);
            this.panelMaterialTable.Name = "panelMaterialTable";
            this.panelMaterialTable.Size = new System.Drawing.Size(287, 300);
            this.panelMaterialTable.TabIndex = 2;
            // 
            // tblMaterialPanel
            // 
            this.tblMaterialPanel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tblMaterialPanel.AutoScroll = true;
            this.tblMaterialPanel.ColumnCount = 4;
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 16F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 170F));
            this.tblMaterialPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tblMaterialPanel.Location = new System.Drawing.Point(3, 3);
            this.tblMaterialPanel.Name = "tblMaterialPanel";
            this.tblMaterialPanel.RowCount = 1;
            this.tblMaterialPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 50F));
            this.tblMaterialPanel.Size = new System.Drawing.Size(277, 330);
            this.tblMaterialPanel.TabIndex = 0;
            // 
            // tbFrequency
            // 
            this.tbFrequency.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbFrequency.Location = new System.Drawing.Point(90, 6);
            this.tbFrequency.Name = "tbFrequency";
            this.tbFrequency.Size = new System.Drawing.Size(44, 20);
            this.tbFrequency.TabIndex = 36;
            this.tbFrequency.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbFrequency_KeyDown);
            this.tbFrequency.Leave += new System.EventHandler(this.tbFrequency_Leave);
            // 
            // tbLabel1
            // 
            this.tbLabel1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbLabel1.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbLabel1.Location = new System.Drawing.Point(18, 9);
            this.tbLabel1.Name = "tbLabel1";
            this.tbLabel1.ReadOnly = true;
            this.tbLabel1.Size = new System.Drawing.Size(66, 13);
            this.tbLabel1.TabIndex = 37;
            this.tbLabel1.Text = "Frequency";
            this.tbLabel1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tbLabel2
            // 
            this.tbLabel2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbLabel2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.tbLabel2.Location = new System.Drawing.Point(140, 9);
            this.tbLabel2.Name = "tbLabel2";
            this.tbLabel2.ReadOnly = true;
            this.tbLabel2.Size = new System.Drawing.Size(35, 13);
            this.tbLabel2.TabIndex = 38;
            this.tbLabel2.Text = "GHz";
            // 
            // btnCheck
            // 
            this.btnCheck.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCheck.Location = new System.Drawing.Point(732, 452);
            this.btnCheck.Name = "btnCheck";
            this.btnCheck.Size = new System.Drawing.Size(100, 26);
            this.btnCheck.TabIndex = 40;
            this.btnCheck.Text = "Check All";
            this.btnCheck.UseVisualStyleBackColor = true;
            this.btnCheck.Click += new System.EventHandler(this.btnCheck_Click);
            // 
            // btnUncheck
            // 
            this.btnUncheck.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnUncheck.Location = new System.Drawing.Point(919, 452);
            this.btnUncheck.Name = "btnUncheck";
            this.btnUncheck.Size = new System.Drawing.Size(100, 26);
            this.btnUncheck.TabIndex = 39;
            this.btnUncheck.Text = "Uncheck All";
            this.btnUncheck.UseVisualStyleBackColor = true;
            this.btnUncheck.Click += new System.EventHandler(this.btnUncheck_Click);
            // 
            // container
            // 
            this.container.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.container.Controls.Add(this.cbColorByMnaufacturer);
            this.container.Controls.Add(this.btnColorByManufacturer);
            this.container.Controls.Add(this.panelCursor);
            this.container.Controls.Add(this.textBox4);
            this.container.Controls.Add(this.tbResin);
            this.container.Controls.Add(this.lSandbox);
            this.container.Controls.Add(this.panelMaterialTable);
            this.container.Controls.Add(this.btnCheck);
            this.container.Controls.Add(this.btnUncheck);
            this.container.Controls.Add(this.tabControl1);
            this.container.Controls.Add(this.tbMaterial);
            this.container.Controls.Add(this.label3);
            this.container.Controls.Add(this.label2);
            this.container.Controls.Add(this.tbComment);
            this.container.Controls.Add(this.panelFrequency);
            this.container.Location = new System.Drawing.Point(0, 0);
            this.container.Name = "container";
            this.container.Size = new System.Drawing.Size(1034, 652);
            this.container.TabIndex = 41;
            // 
            // cbColorByMnaufacturer
            // 
            this.cbColorByMnaufacturer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.cbColorByMnaufacturer.AutoSize = true;
            this.cbColorByMnaufacturer.Location = new System.Drawing.Point(733, 423);
            this.cbColorByMnaufacturer.Name = "cbColorByMnaufacturer";
            this.cbColorByMnaufacturer.Size = new System.Drawing.Size(129, 17);
            this.cbColorByMnaufacturer.TabIndex = 46;
            this.cbColorByMnaufacturer.Text = "Color by manufacturer";
            this.cbColorByMnaufacturer.UseVisualStyleBackColor = true;
            this.cbColorByMnaufacturer.CheckedChanged += new System.EventHandler(this.cbColorByMnaufacturer_CheckedChanged);
            // 
            // btnColorByManufacturer
            // 
            this.btnColorByManufacturer.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnColorByManufacturer.Location = new System.Drawing.Point(919, 419);
            this.btnColorByManufacturer.Name = "btnColorByManufacturer";
            this.btnColorByManufacturer.Size = new System.Drawing.Size(100, 26);
            this.btnColorByManufacturer.TabIndex = 45;
            this.btnColorByManufacturer.Text = "Settings..";
            this.btnColorByManufacturer.UseVisualStyleBackColor = true;
            this.btnColorByManufacturer.Click += new System.EventHandler(this.btnColorByManufacturer_Click);
            // 
            // panelCursor
            // 
            this.panelCursor.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panelCursor.Controls.Add(this.tbChartFreq);
            this.panelCursor.Controls.Add(this.textBox2);
            this.panelCursor.Location = new System.Drawing.Point(826, 388);
            this.panelCursor.Name = "panelCursor";
            this.panelCursor.Size = new System.Drawing.Size(195, 25);
            this.panelCursor.TabIndex = 41;
            this.panelCursor.Visible = false;
            // 
            // tbChartFreq
            // 
            this.tbChartFreq.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.tbChartFreq.Font = new System.Drawing.Font("Microsoft Sans Serif", 7.8F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbChartFreq.Location = new System.Drawing.Point(125, 0);
            this.tbChartFreq.Name = "tbChartFreq";
            this.tbChartFreq.ReadOnly = true;
            this.tbChartFreq.Size = new System.Drawing.Size(45, 19);
            this.tbChartFreq.TabIndex = 3;
            this.tbChartFreq.TabStop = false;
            this.tbChartFreq.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // textBox2
            // 
            this.textBox2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox2.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox2.Location = new System.Drawing.Point(12, 3);
            this.textBox2.Name = "textBox2";
            this.textBox2.ReadOnly = true;
            this.textBox2.Size = new System.Drawing.Size(104, 13);
            this.textBox2.TabIndex = 38;
            this.textBox2.Text = "Frequency [GHz]";
            this.textBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // textBox4
            // 
            this.textBox4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.textBox4.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.textBox4.Location = new System.Drawing.Point(733, 620);
            this.textBox4.Name = "textBox4";
            this.textBox4.ReadOnly = true;
            this.textBox4.Size = new System.Drawing.Size(69, 13);
            this.textBox4.TabIndex = 44;
            this.textBox4.Text = "Resin [%]";
            this.textBox4.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // tbResin
            // 
            this.tbResin.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.tbResin.Location = new System.Drawing.Point(808, 617);
            this.tbResin.Name = "tbResin";
            this.tbResin.Size = new System.Drawing.Size(44, 20);
            this.tbResin.TabIndex = 43;
            this.tbResin.KeyDown += new System.Windows.Forms.KeyEventHandler(this.tbResin_KeyDown);
            this.tbResin.Leave += new System.EventHandler(this.tbResin_Leave);
            // 
            // lSandbox
            // 
            this.lSandbox.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.lSandbox.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lSandbox.Location = new System.Drawing.Point(657, 2);
            this.lSandbox.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.lSandbox.Name = "lSandbox";
            this.lSandbox.Size = new System.Drawing.Size(362, 19);
            this.lSandbox.TabIndex = 41;
            this.lSandbox.Text = "Z-zero";
            this.lSandbox.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // panelFrequency
            // 
            this.panelFrequency.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.panelFrequency.Controls.Add(this.tbFrequency);
            this.panelFrequency.Controls.Add(this.tbLabel1);
            this.panelFrequency.Controls.Add(this.tbLabel2);
            this.panelFrequency.Location = new System.Drawing.Point(840, 611);
            this.panelFrequency.Name = "panelFrequency";
            this.panelFrequency.Size = new System.Drawing.Size(191, 36);
            this.panelFrequency.TabIndex = 41;
            // 
            // btnExport
            // 
            this.btnExport.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnExport.Image = global::ZZero.ZPlanner.Properties.Resources.Export_Material_Mapper_16;
            this.btnExport.Location = new System.Drawing.Point(638, 654);
            this.btnExport.Name = "btnExport";
            this.btnExport.Size = new System.Drawing.Size(82, 26);
            this.btnExport.TabIndex = 42;
            this.btnExport.Text = "  Export";
            this.btnExport.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.btnExport.UseVisualStyleBackColor = true;
            this.btnExport.Click += new System.EventHandler(this.btnExport_Click);
            // 
            // MaterialMapper
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(1034, 692);
            this.Controls.Add(this.btnExport);
            this.Controls.Add(this.container);
            this.Controls.Add(this.btnClose);
            this.Controls.Add(this.btnPrint);
            this.Controls.Add(this.btnCopy);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Margin = new System.Windows.Forms.Padding(2);
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(1050, 730);
            this.Name = "MaterialMapper";
            this.ShowInTaskbar = false;
            this.Text = "  Material Mapper";
            ((System.ComponentModel.ISupportInitialize)(this.chartMapper)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chartRadar)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPagePoints.ResumeLayout(false);
            this.panelMapper.ResumeLayout(false);
            this.tabPageRadar.ResumeLayout(false);
            this.panelRadial.ResumeLayout(false);
            this.gbAxis.ResumeLayout(false);
            this.gbAxis.PerformLayout();
            this.gbStyle.ResumeLayout(false);
            this.gbStyle.PerformLayout();
            this.tabPageFrequency.ResumeLayout(false);
            this.tabPageFrequency.PerformLayout();
            this.panelDkDf.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.chartFrequency)).EndInit();
            this.panelMaterialTable.ResumeLayout(false);
            this.container.ResumeLayout(false);
            this.container.PerformLayout();
            this.panelCursor.ResumeLayout(false);
            this.panelCursor.PerformLayout();
            this.panelFrequency.ResumeLayout(false);
            this.panelFrequency.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart chartMapper;
        private System.Windows.Forms.Button btnPrint;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.Button btnClose;
        private System.Windows.Forms.TextBox tbComment;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbY;
        private System.Windows.Forms.ComboBox cbX;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartRadar;
        private System.Windows.Forms.TextBox tbMaterial;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPagePoints;
        private System.Windows.Forms.TabPage tabPageRadar;
        private System.Windows.Forms.TextBox tbFrequency;
        private System.Windows.Forms.TextBox tbLabel1;
        private System.Windows.Forms.TextBox tbLabel2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.CheckedListBox lbRadarAxis;
        private System.Windows.Forms.GroupBox gbStyle;
        private System.Windows.Forms.RadioButton rbArea;
        private System.Windows.Forms.RadioButton rbLine;
        private System.Windows.Forms.Button btnCheck;
        private System.Windows.Forms.Button btnUncheck;
        private System.Windows.Forms.TableLayoutPanel tblMaterialPanel;
        private System.Windows.Forms.Panel panelMaterialTable;
        private System.Windows.Forms.GroupBox gbAxis;
        private System.Windows.Forms.RadioButton rbBest;
        private System.Windows.Forms.RadioButton rbOriginal;
        private System.Windows.Forms.Panel container;
		private System.Windows.Forms.Label lSandbox;
        private System.Windows.Forms.TabPage tabPageFrequency;
        private System.Windows.Forms.DataVisualization.Charting.Chart chartFrequency;
        private System.Windows.Forms.RadioButton rbDf;
        private System.Windows.Forms.RadioButton rbDk;
        private System.Windows.Forms.TextBox textBox2;
        private System.Windows.Forms.TextBox tbChartFreq;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textBox4;
        private System.Windows.Forms.TextBox tbResin;
        private System.Windows.Forms.Panel panelFrequency;
        private System.Windows.Forms.Panel panelCursor;
        private System.Windows.Forms.CheckBox cbLabels;
        private System.Windows.Forms.Button btnExport;
        private System.Windows.Forms.Panel panelMapper;
        private System.Windows.Forms.Panel panelRadial;
        private System.Windows.Forms.Panel panelDkDf;
        private System.Windows.Forms.CheckBox cbColorByMnaufacturer;
        private System.Windows.Forms.Button btnColorByManufacturer;
    }
}