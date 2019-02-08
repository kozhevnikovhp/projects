namespace ZZero.ZPlanner.UI.Dialogs
{
    partial class DiffToolDialog
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
            System.Windows.Forms.DataGridViewCellStyle dataGridViewCellStyle2 = new System.Windows.Forms.DataGridViewCellStyle();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(DiffToolDialog));
            this.splitContainer = new System.Windows.Forms.SplitContainer();
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.panel11 = new System.Windows.Forms.Panel();
            this.label9 = new System.Windows.Forms.Label();
            this.textBoxTotalThickness1 = new System.Windows.Forms.TextBox();
            this.panel13 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.comboBoxPairs1 = new System.Windows.Forms.ComboBox();
            this.panel12 = new System.Windows.Forms.Panel();
            this.label6 = new System.Windows.Forms.Label();
            this.comboBoxSingles1 = new System.Windows.Forms.ComboBox();
            this.dataGridView1 = new ZZero.ZPlanner.UI.Grid.ZDataGridView();
            this.label1 = new System.Windows.Forms.Label();
            this.textBoxFile1 = new System.Windows.Forms.TextBox();
            this.buttonFile1 = new System.Windows.Forms.Button();
            this.tableLayoutPanel2 = new System.Windows.Forms.TableLayoutPanel();
            this.panel1 = new System.Windows.Forms.Panel();
            this.label10 = new System.Windows.Forms.Label();
            this.textBoxTotalThickness2 = new System.Windows.Forms.TextBox();
            this.panel3 = new System.Windows.Forms.Panel();
            this.label7 = new System.Windows.Forms.Label();
            this.comboBoxPairs2 = new System.Windows.Forms.ComboBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.label8 = new System.Windows.Forms.Label();
            this.comboBoxSingles2 = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.dataGridView2 = new ZZero.ZPlanner.UI.Grid.ZDataGridView();
            this.textBoxFile2 = new System.Windows.Forms.TextBox();
            this.buttonFile2 = new System.Windows.Forms.Button();
            this.panelHeader = new System.Windows.Forms.Panel();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.sensitivityNumericUpDown = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.btnPrint = new System.Windows.Forms.Button();
            this.btnCopy = new System.Windows.Forms.Button();
            this.checkBoxCenterLine = new System.Windows.Forms.CheckBox();
            this.checkBoxTileVertical = new System.Windows.Forms.CheckBox();
            this.checkBoxTileHorizontal = new System.Windows.Forms.CheckBox();
            this.lProduct = new System.Windows.Forms.Label();
            this.panelContent = new System.Windows.Forms.Panel();
            this.labelRedText = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).BeginInit();
            this.splitContainer.Panel1.SuspendLayout();
            this.splitContainer.Panel2.SuspendLayout();
            this.splitContainer.SuspendLayout();
            this.tableLayoutPanel1.SuspendLayout();
            this.panel11.SuspendLayout();
            this.panel13.SuspendLayout();
            this.panel12.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).BeginInit();
            this.tableLayoutPanel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).BeginInit();
            this.panelHeader.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.sensitivityNumericUpDown)).BeginInit();
            this.panelContent.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer
            // 
            this.splitContainer.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer.Location = new System.Drawing.Point(12, 26);
            this.splitContainer.Name = "splitContainer";
            this.splitContainer.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer.Panel1
            // 
            this.splitContainer.Panel1.Controls.Add(this.tableLayoutPanel1);
            this.splitContainer.Panel1.Controls.Add(this.dataGridView1);
            this.splitContainer.Panel1.Controls.Add(this.label1);
            this.splitContainer.Panel1.Controls.Add(this.textBoxFile1);
            this.splitContainer.Panel1.Controls.Add(this.buttonFile1);
            // 
            // splitContainer.Panel2
            // 
            this.splitContainer.Panel2.Controls.Add(this.tableLayoutPanel2);
            this.splitContainer.Panel2.Controls.Add(this.label2);
            this.splitContainer.Panel2.Controls.Add(this.dataGridView2);
            this.splitContainer.Panel2.Controls.Add(this.textBoxFile2);
            this.splitContainer.Panel2.Controls.Add(this.buttonFile2);
            this.splitContainer.Size = new System.Drawing.Size(1420, 716);
            this.splitContainer.SplitterDistance = 357;
            this.splitContainer.SplitterWidth = 2;
            this.splitContainer.TabIndex = 0;
            this.splitContainer.Resize += new System.EventHandler(this.splitContainer_Resize);
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel1.ColumnCount = 3;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel1.Controls.Add(this.panel11, 0, 0);
            this.tableLayoutPanel1.Controls.Add(this.panel13, 2, 0);
            this.tableLayoutPanel1.Controls.Add(this.panel12, 1, 0);
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 33);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 1;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(1388, 28);
            this.tableLayoutPanel1.TabIndex = 11;
            // 
            // panel11
            // 
            this.panel11.Controls.Add(this.label9);
            this.panel11.Controls.Add(this.textBoxTotalThickness1);
            this.panel11.Location = new System.Drawing.Point(3, 3);
            this.panel11.Name = "panel11";
            this.panel11.Size = new System.Drawing.Size(174, 22);
            this.panel11.TabIndex = 10;
            // 
            // label9
            // 
            this.label9.Location = new System.Drawing.Point(8, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(92, 20);
            this.label9.TabIndex = 8;
            this.label9.Text = "Board Thickness:";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // textBoxTotalThickness1
            // 
            this.textBoxTotalThickness1.ForeColor = System.Drawing.Color.Red;
            this.textBoxTotalThickness1.Location = new System.Drawing.Point(102, 1);
            this.textBoxTotalThickness1.Name = "textBoxTotalThickness1";
            this.textBoxTotalThickness1.ReadOnly = true;
            this.textBoxTotalThickness1.Size = new System.Drawing.Size(70, 20);
            this.textBoxTotalThickness1.TabIndex = 0;
            // 
            // panel13
            // 
            this.panel13.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel13.Controls.Add(this.label5);
            this.panel13.Controls.Add(this.comboBoxPairs1);
            this.panel13.Location = new System.Drawing.Point(787, 3);
            this.panel13.Name = "panel13";
            this.panel13.Size = new System.Drawing.Size(598, 22);
            this.panel13.TabIndex = 10;
            // 
            // label5
            // 
            this.label5.Location = new System.Drawing.Point(3, 1);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(100, 20);
            this.label5.TabIndex = 8;
            this.label5.Text = "Differential signal:";
            this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboBoxPairs1
            // 
            this.comboBoxPairs1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxPairs1.DropDownWidth = 120;
            this.comboBoxPairs1.FormattingEnabled = true;
            this.comboBoxPairs1.Location = new System.Drawing.Point(105, 1);
            this.comboBoxPairs1.Name = "comboBoxPairs1";
            this.comboBoxPairs1.Size = new System.Drawing.Size(490, 21);
            this.comboBoxPairs1.TabIndex = 9;
            // 
            // panel12
            // 
            this.panel12.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel12.Controls.Add(this.label6);
            this.panel12.Controls.Add(this.comboBoxSingles1);
            this.panel12.Location = new System.Drawing.Point(183, 3);
            this.panel12.Name = "panel12";
            this.panel12.Size = new System.Drawing.Size(598, 22);
            this.panel12.TabIndex = 10;
            // 
            // label6
            // 
            this.label6.Location = new System.Drawing.Point(3, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(100, 20);
            this.label6.TabIndex = 8;
            this.label6.Text = "Single-ended signal:";
            this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboBoxSingles1
            // 
            this.comboBoxSingles1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxSingles1.DropDownWidth = 120;
            this.comboBoxSingles1.FormattingEnabled = true;
            this.comboBoxSingles1.Location = new System.Drawing.Point(105, 1);
            this.comboBoxSingles1.Name = "comboBoxSingles1";
            this.comboBoxSingles1.Size = new System.Drawing.Size(490, 21);
            this.comboBoxSingles1.TabIndex = 9;
            // 
            // dataGridView1
            // 
            this.dataGridView1.AllowDrawCenterLine = true;
            this.dataGridView1.AllowUserToAddRows = false;
            this.dataGridView1.AllowUserToDeleteRows = false;
            this.dataGridView1.AllowUserToResizeColumns = false;
            this.dataGridView1.AllowUserToResizeRows = false;
            this.dataGridView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView1.ColumnHeaderNamesVisible = true;
            this.dataGridView1.ColumnHeadersHeight = 47;
            this.dataGridView1.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dataGridView1.DefaultCellBehavior = ZZero.ZPlanner.UI.Grid.ZDataGridViewColumnHeaderCellBehavior.DisabledHidden;
            dataGridViewCellStyle1.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle1.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle1.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            dataGridViewCellStyle1.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle1.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle1.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle1.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dataGridView1.DefaultCellStyle = dataGridViewCellStyle1;
            this.dataGridView1.DefaultDateTimeGrouping = ZZero.ZPlanner.UI.Grid.ZDataGridViewFilterMenuDateTimeGrouping.Second;
            this.dataGridView1.FilterString = "";
            this.dataGridView1.Location = new System.Drawing.Point(0, 64);
            this.dataGridView1.MultiSelect = false;
            this.dataGridView1.Name = "dataGridView1";
            this.dataGridView1.ReadOnly = true;
            this.dataGridView1.RowHeadersWidth = 56;
            this.dataGridView1.Size = new System.Drawing.Size(1420, 290);
            this.dataGridView1.SortString = "";
            this.dataGridView1.TabIndex = 0;
            this.dataGridView1.CellStateChanged += new System.Windows.Forms.DataGridViewCellStateChangedEventHandler(this.dataGridView1_CellStateChanged);
            this.dataGridView1.RowStateChanged += new System.Windows.Forms.DataGridViewRowStateChangedEventHandler(this.dataGridView1_RowStateChanged);
            this.dataGridView1.Scroll += new System.Windows.Forms.ScrollEventHandler(this.dataGridView1_Scroll);
            // 
            // label1
            // 
            this.label1.Location = new System.Drawing.Point(3, 10);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(217, 20);
            this.label1.TabIndex = 8;
            this.label1.Text = "Select first Stackup file for comparison:";
            this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // textBoxFile1
            // 
            this.textBoxFile1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFile1.Location = new System.Drawing.Point(226, 11);
            this.textBoxFile1.Name = "textBoxFile1";
            this.textBoxFile1.ReadOnly = true;
            this.textBoxFile1.Size = new System.Drawing.Size(1156, 20);
            this.textBoxFile1.TabIndex = 0;
            // 
            // buttonFile1
            // 
            this.buttonFile1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonFile1.Location = new System.Drawing.Point(1388, 9);
            this.buttonFile1.Name = "buttonFile1";
            this.buttonFile1.Size = new System.Drawing.Size(32, 23);
            this.buttonFile1.TabIndex = 1;
            this.buttonFile1.Text = "...";
            this.buttonFile1.UseVisualStyleBackColor = true;
            this.buttonFile1.Click += new System.EventHandler(this.buttonFile1_Click);
            // 
            // tableLayoutPanel2
            // 
            this.tableLayoutPanel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tableLayoutPanel2.ColumnCount = 3;
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Absolute, 180F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 50F));
            this.tableLayoutPanel2.Controls.Add(this.panel1, 0, 0);
            this.tableLayoutPanel2.Controls.Add(this.panel3, 2, 0);
            this.tableLayoutPanel2.Controls.Add(this.panel2, 1, 0);
            this.tableLayoutPanel2.Location = new System.Drawing.Point(0, 33);
            this.tableLayoutPanel2.Name = "tableLayoutPanel2";
            this.tableLayoutPanel2.RowCount = 1;
            this.tableLayoutPanel2.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel2.Size = new System.Drawing.Size(1388, 28);
            this.tableLayoutPanel2.TabIndex = 11;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label10);
            this.panel1.Controls.Add(this.textBoxTotalThickness2);
            this.panel1.Location = new System.Drawing.Point(3, 3);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(174, 22);
            this.panel1.TabIndex = 10;
            // 
            // label10
            // 
            this.label10.Location = new System.Drawing.Point(3, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(92, 20);
            this.label10.TabIndex = 8;
            this.label10.Text = "Board Thickness:";
            this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // textBoxTotalThickness2
            // 
            this.textBoxTotalThickness2.Location = new System.Drawing.Point(102, 1);
            this.textBoxTotalThickness2.Name = "textBoxTotalThickness2";
            this.textBoxTotalThickness2.ReadOnly = true;
            this.textBoxTotalThickness2.Size = new System.Drawing.Size(70, 20);
            this.textBoxTotalThickness2.TabIndex = 0;
            // 
            // panel3
            // 
            this.panel3.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel3.Controls.Add(this.label7);
            this.panel3.Controls.Add(this.comboBoxPairs2);
            this.panel3.Location = new System.Drawing.Point(787, 3);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(598, 22);
            this.panel3.TabIndex = 10;
            // 
            // label7
            // 
            this.label7.Location = new System.Drawing.Point(3, 1);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(100, 20);
            this.label7.TabIndex = 8;
            this.label7.Text = "Differential signal:";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboBoxPairs2
            // 
            this.comboBoxPairs2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxPairs2.DropDownWidth = 120;
            this.comboBoxPairs2.FormattingEnabled = true;
            this.comboBoxPairs2.Location = new System.Drawing.Point(105, 1);
            this.comboBoxPairs2.Name = "comboBoxPairs2";
            this.comboBoxPairs2.Size = new System.Drawing.Size(490, 21);
            this.comboBoxPairs2.TabIndex = 9;
            // 
            // panel2
            // 
            this.panel2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panel2.Controls.Add(this.label8);
            this.panel2.Controls.Add(this.comboBoxSingles2);
            this.panel2.Location = new System.Drawing.Point(183, 3);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(598, 22);
            this.panel2.TabIndex = 10;
            // 
            // label8
            // 
            this.label8.Location = new System.Drawing.Point(3, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(100, 20);
            this.label8.TabIndex = 8;
            this.label8.Text = "Single-ended signal:";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // comboBoxSingles2
            // 
            this.comboBoxSingles2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.comboBoxSingles2.DropDownWidth = 120;
            this.comboBoxSingles2.FormattingEnabled = true;
            this.comboBoxSingles2.Location = new System.Drawing.Point(105, 1);
            this.comboBoxSingles2.Name = "comboBoxSingles2";
            this.comboBoxSingles2.Size = new System.Drawing.Size(490, 21);
            this.comboBoxSingles2.TabIndex = 9;
            // 
            // label2
            // 
            this.label2.Location = new System.Drawing.Point(3, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(217, 20);
            this.label2.TabIndex = 8;
            this.label2.Text = "Select second Stackup file for comparison:";
            this.label2.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // dataGridView2
            // 
            this.dataGridView2.AllowDrawCenterLine = true;
            this.dataGridView2.AllowUserToAddRows = false;
            this.dataGridView2.AllowUserToDeleteRows = false;
            this.dataGridView2.AllowUserToResizeColumns = false;
            this.dataGridView2.AllowUserToResizeRows = false;
            this.dataGridView2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.dataGridView2.ColumnHeaderNamesVisible = true;
            this.dataGridView2.ColumnHeadersHeight = 47;
            this.dataGridView2.ColumnHeadersHeightSizeMode = System.Windows.Forms.DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            this.dataGridView2.DefaultCellBehavior = ZZero.ZPlanner.UI.Grid.ZDataGridViewColumnHeaderCellBehavior.DisabledHidden;
            dataGridViewCellStyle2.Alignment = System.Windows.Forms.DataGridViewContentAlignment.MiddleLeft;
            dataGridViewCellStyle2.BackColor = System.Drawing.SystemColors.Window;
            dataGridViewCellStyle2.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
            dataGridViewCellStyle2.ForeColor = System.Drawing.SystemColors.ControlText;
            dataGridViewCellStyle2.SelectionBackColor = System.Drawing.SystemColors.Highlight;
            dataGridViewCellStyle2.SelectionForeColor = System.Drawing.SystemColors.HighlightText;
            dataGridViewCellStyle2.WrapMode = System.Windows.Forms.DataGridViewTriState.False;
            this.dataGridView2.DefaultCellStyle = dataGridViewCellStyle2;
            this.dataGridView2.DefaultDateTimeGrouping = ZZero.ZPlanner.UI.Grid.ZDataGridViewFilterMenuDateTimeGrouping.Second;
            this.dataGridView2.FilterString = "";
            this.dataGridView2.Location = new System.Drawing.Point(0, 64);
            this.dataGridView2.MultiSelect = false;
            this.dataGridView2.Name = "dataGridView2";
            this.dataGridView2.ReadOnly = true;
            this.dataGridView2.RowHeadersWidth = 56;
            this.dataGridView2.Size = new System.Drawing.Size(1420, 326);
            this.dataGridView2.SortString = "";
            this.dataGridView2.TabIndex = 0;
            this.dataGridView2.CellStateChanged += new System.Windows.Forms.DataGridViewCellStateChangedEventHandler(this.dataGridView2_CellStateChanged);
            this.dataGridView2.RowStateChanged += new System.Windows.Forms.DataGridViewRowStateChangedEventHandler(this.dataGridView2_RowStateChanged);
            this.dataGridView2.Scroll += new System.Windows.Forms.ScrollEventHandler(this.dataGridView2_Scroll);
            // 
            // textBoxFile2
            // 
            this.textBoxFile2.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.textBoxFile2.Location = new System.Drawing.Point(226, 11);
            this.textBoxFile2.Name = "textBoxFile2";
            this.textBoxFile2.ReadOnly = true;
            this.textBoxFile2.Size = new System.Drawing.Size(1156, 20);
            this.textBoxFile2.TabIndex = 2;
            // 
            // buttonFile2
            // 
            this.buttonFile2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.buttonFile2.Location = new System.Drawing.Point(1388, 9);
            this.buttonFile2.Name = "buttonFile2";
            this.buttonFile2.Size = new System.Drawing.Size(32, 23);
            this.buttonFile2.TabIndex = 3;
            this.buttonFile2.Text = "...";
            this.buttonFile2.UseVisualStyleBackColor = true;
            this.buttonFile2.Click += new System.EventHandler(this.buttonFile2_Click);
            // 
            // panelHeader
            // 
            this.panelHeader.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelHeader.Controls.Add(this.groupBox1);
            this.panelHeader.Controls.Add(this.btnPrint);
            this.panelHeader.Controls.Add(this.btnCopy);
            this.panelHeader.Controls.Add(this.checkBoxCenterLine);
            this.panelHeader.Controls.Add(this.checkBoxTileVertical);
            this.panelHeader.Controls.Add(this.checkBoxTileHorizontal);
            this.panelHeader.Location = new System.Drawing.Point(0, 0);
            this.panelHeader.Name = "panelHeader";
            this.panelHeader.Size = new System.Drawing.Size(1444, 36);
            this.panelHeader.TabIndex = 1;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.sensitivityNumericUpDown);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Location = new System.Drawing.Point(352, 5);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(120, 29);
            this.groupBox1.TabIndex = 35;
            this.groupBox1.TabStop = false;
            // 
            // sensitivityNumericUpDown
            // 
            this.sensitivityNumericUpDown.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.sensitivityNumericUpDown.Location = new System.Drawing.Point(59, 11);
            this.sensitivityNumericUpDown.Maximum = new decimal(new int[] {
            999,
            0,
            0,
            0});
            this.sensitivityNumericUpDown.Name = "sensitivityNumericUpDown";
            this.sensitivityNumericUpDown.Size = new System.Drawing.Size(40, 16);
            this.sensitivityNumericUpDown.TabIndex = 35;
            this.sensitivityNumericUpDown.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.sensitivityNumericUpDown.ValueChanged += new System.EventHandler(this.sensitivityNumericUpDown_ValueChanged);
            // 
            // label3
            // 
            this.label3.Location = new System.Drawing.Point(1, 7);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(56, 20);
            this.label3.TabIndex = 8;
            this.label3.Text = "Sensitivity";
            this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
            // 
            // label4
            // 
            this.label4.Location = new System.Drawing.Point(99, 7);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(18, 20);
            this.label4.TabIndex = 8;
            this.label4.Text = "‰";
            this.label4.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // btnPrint
            // 
            this.btnPrint.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnPrint.Location = new System.Drawing.Point(1350, 11);
            this.btnPrint.Margin = new System.Windows.Forms.Padding(4);
            this.btnPrint.Name = "btnPrint";
            this.btnPrint.Size = new System.Drawing.Size(82, 23);
            this.btnPrint.TabIndex = 33;
            this.btnPrint.Text = "Print";
            this.btnPrint.UseVisualStyleBackColor = true;
            this.btnPrint.Click += new System.EventHandler(this.btnPrint_Click);
            // 
            // btnCopy
            // 
            this.btnCopy.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCopy.Location = new System.Drawing.Point(1260, 11);
            this.btnCopy.Margin = new System.Windows.Forms.Padding(4);
            this.btnCopy.Name = "btnCopy";
            this.btnCopy.Size = new System.Drawing.Size(82, 23);
            this.btnCopy.TabIndex = 32;
            this.btnCopy.Text = "Copy";
            this.btnCopy.UseVisualStyleBackColor = true;
            this.btnCopy.Click += new System.EventHandler(this.btnCopy_Click);
            // 
            // checkBoxCenterLine
            // 
            this.checkBoxCenterLine.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBoxCenterLine.Checked = true;
            this.checkBoxCenterLine.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxCenterLine.Image = global::ZZero.ZPlanner.Properties.Resources.Stackup_Center_Line_16;
            this.checkBoxCenterLine.Location = new System.Drawing.Point(234, 11);
            this.checkBoxCenterLine.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxCenterLine.Name = "checkBoxCenterLine";
            this.checkBoxCenterLine.Size = new System.Drawing.Size(100, 23);
            this.checkBoxCenterLine.TabIndex = 7;
            this.checkBoxCenterLine.Text = "Center Line";
            this.checkBoxCenterLine.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.checkBoxCenterLine.UseVisualStyleBackColor = true;
            this.checkBoxCenterLine.CheckedChanged += new System.EventHandler(this.checkBoxCenterLine_CheckedChanged);
            // 
            // checkBoxTileVertical
            // 
            this.checkBoxTileVertical.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBoxTileVertical.Checked = true;
            this.checkBoxTileVertical.CheckState = System.Windows.Forms.CheckState.Checked;
            this.checkBoxTileVertical.Image = global::ZZero.ZPlanner.Properties.Resources.Tile_Vertical_16;
            this.checkBoxTileVertical.Location = new System.Drawing.Point(117, 11);
            this.checkBoxTileVertical.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxTileVertical.Name = "checkBoxTileVertical";
            this.checkBoxTileVertical.Size = new System.Drawing.Size(100, 23);
            this.checkBoxTileVertical.TabIndex = 7;
            this.checkBoxTileVertical.Text = "Tile Vertical";
            this.checkBoxTileVertical.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.checkBoxTileVertical.UseVisualStyleBackColor = true;
            this.checkBoxTileVertical.CheckedChanged += new System.EventHandler(this.checkBoxTileVertical_CheckedChanged);
            // 
            // checkBoxTileHorizontal
            // 
            this.checkBoxTileHorizontal.Appearance = System.Windows.Forms.Appearance.Button;
            this.checkBoxTileHorizontal.Image = global::ZZero.ZPlanner.Properties.Resources.Tile_Horizontal_16;
            this.checkBoxTileHorizontal.Location = new System.Drawing.Point(12, 11);
            this.checkBoxTileHorizontal.Margin = new System.Windows.Forms.Padding(2);
            this.checkBoxTileHorizontal.Name = "checkBoxTileHorizontal";
            this.checkBoxTileHorizontal.Size = new System.Drawing.Size(100, 23);
            this.checkBoxTileHorizontal.TabIndex = 7;
            this.checkBoxTileHorizontal.Text = "Tile Horizontal";
            this.checkBoxTileHorizontal.TextImageRelation = System.Windows.Forms.TextImageRelation.ImageBeforeText;
            this.checkBoxTileHorizontal.UseVisualStyleBackColor = true;
            this.checkBoxTileHorizontal.CheckedChanged += new System.EventHandler(this.checkBoxTileHorizontal_CheckedChanged);
            // 
            // lProduct
            // 
            this.lProduct.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.lProduct.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lProduct.Location = new System.Drawing.Point(422, 6);
            this.lProduct.Margin = new System.Windows.Forms.Padding(5, 0, 5, 0);
            this.lProduct.Name = "lProduct";
            this.lProduct.Size = new System.Drawing.Size(600, 19);
            this.lProduct.TabIndex = 31;
            this.lProduct.Text = "Z-zero";
            this.lProduct.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panelContent
            // 
            this.panelContent.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.panelContent.Controls.Add(this.splitContainer);
            this.panelContent.Controls.Add(this.lProduct);
            this.panelContent.Controls.Add(this.labelRedText);
            this.panelContent.Location = new System.Drawing.Point(0, 38);
            this.panelContent.Name = "panelContent";
            this.panelContent.Size = new System.Drawing.Size(1444, 742);
            this.panelContent.TabIndex = 2;
            // 
            // labelRedText
            // 
            this.labelRedText.ForeColor = System.Drawing.Color.Red;
            this.labelRedText.Location = new System.Drawing.Point(12, 3);
            this.labelRedText.Name = "labelRedText";
            this.labelRedText.Size = new System.Drawing.Size(217, 20);
            this.labelRedText.TabIndex = 8;
            this.labelRedText.Text = "Stackup differences are shown in red.";
            this.labelRedText.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // DiffToolDialog
            // 
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.None;
            this.ClientSize = new System.Drawing.Size(1444, 788);
            this.Controls.Add(this.panelHeader);
            this.Controls.Add(this.panelContent);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MinimumSize = new System.Drawing.Size(1200, 800);
            this.Name = "DiffToolDialog";
            this.Text = "  Compare Stackups";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.DiffToolDialog_FormClosing);
            this.splitContainer.Panel1.ResumeLayout(false);
            this.splitContainer.Panel1.PerformLayout();
            this.splitContainer.Panel2.ResumeLayout(false);
            this.splitContainer.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer)).EndInit();
            this.splitContainer.ResumeLayout(false);
            this.tableLayoutPanel1.ResumeLayout(false);
            this.panel11.ResumeLayout(false);
            this.panel11.PerformLayout();
            this.panel13.ResumeLayout(false);
            this.panel12.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView1)).EndInit();
            this.tableLayoutPanel2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel3.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.dataGridView2)).EndInit();
            this.panelHeader.ResumeLayout(false);
            this.groupBox1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.sensitivityNumericUpDown)).EndInit();
            this.panelContent.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer;
        private ZZero.ZPlanner.UI.Grid.ZDataGridView dataGridView1;
        private ZZero.ZPlanner.UI.Grid.ZDataGridView dataGridView2;
        private System.Windows.Forms.Panel panelHeader;
        private System.Windows.Forms.Panel panelContent;
        private System.Windows.Forms.TextBox textBoxFile1;
        private System.Windows.Forms.Button buttonFile2;
        private System.Windows.Forms.TextBox textBoxFile2;
        private System.Windows.Forms.Button buttonFile1;
        private System.Windows.Forms.CheckBox checkBoxTileVertical;
        private System.Windows.Forms.CheckBox checkBoxTileHorizontal;
        private System.Windows.Forms.CheckBox checkBoxCenterLine;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label lProduct;
        private System.Windows.Forms.Button btnPrint;
        private System.Windows.Forms.Button btnCopy;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.ComboBox comboBoxSingles1;
        private System.Windows.Forms.ComboBox comboBoxPairs1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox comboBoxSingles2;
        private System.Windows.Forms.ComboBox comboBoxPairs2;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label labelRedText;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.TextBox textBoxTotalThickness1;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.TextBox textBoxTotalThickness2;
        private System.Windows.Forms.NumericUpDown sensitivityNumericUpDown;
        private System.Windows.Forms.Panel panel12;
        private System.Windows.Forms.Panel panel11;
        private System.Windows.Forms.Panel panel13;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel2;
    }
}