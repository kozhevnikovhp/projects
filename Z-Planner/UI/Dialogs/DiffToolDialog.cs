using System;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Printing;
using System.Globalization;
using System.IO;
using System.Windows.Forms;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Provider;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class DiffToolDialog : Form
    {
        string fileName1, fileName2;
        DateTime dateTime1 = DateTime.MinValue, dateTime2 = DateTime.MinValue;
        ZPlannerProject project1 = null;
        ZPlannerProject project2 = null;
        string oldFrequencyValue = Options.TheOptions.Frequency.ToString(ZPlannerManager.GetFormatByParameterID(ZStringConstants.ParameterIDZo_Frequency), CultureInfo.InvariantCulture);
        private PrintDocument printDocument1 = new PrintDocument();

        List<ZParameter> parameters1 = new List<ZParameter>();
        List<ZParameter> parameters2 = new List<ZParameter>();

        bool isCenterLineVisible;

        public DiffToolDialog()
        {
            InitializeComponent();

            lProduct.Text = "Z-zero  " + AboutData.TheAbout.sVersion;
            printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);
            this.Activated += DiffToolDialog_Activated;
        }

        int sensitivity;
        int Sensitivity
        {
            get
            {
                return sensitivity;
            }
        }

        public void CompareWithStackup(string stackupFile)
        {
            isCenterLineVisible = ZPlannerManager.IsCenterLineVisible;
            checkBoxCenterLine_CheckedChanged(checkBoxCenterLine, EventArgs.Empty);

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            bool isAutoMirror = false;
            if (ZPlannerManager.StackupPanel != null)
            {
                isAutoMirror = ZPlannerManager.IsAutoMirror;
                ZPlannerManager.IsAutoMirror = false;
            }

            ClearAll();

            if (string.IsNullOrEmpty(stackupFile)) return;

            fileName1 = stackupFile;
            textBoxFile1.Text = fileName1;
            //buttonFile2_Click(buttonFile2, new EventArgs());

            if (fileName1 != "UNSAVED" && !string.IsNullOrWhiteSpace(fileName1) && File.Exists(fileName1))
            {
                dateTime1 = File.GetLastWriteTime(fileName1);
            }

            UpdateDiff();

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = isAutoMirror;
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        public void CompareWithStackup(ZPlannerProject project, string stackupFile)
        {
            isCenterLineVisible = ZPlannerManager.IsCenterLineVisible;
            checkBoxCenterLine_CheckedChanged(checkBoxCenterLine, EventArgs.Empty);

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            bool isAutoMirror = false;
            if (ZPlannerManager.StackupPanel != null)
            {
                isAutoMirror = ZPlannerManager.IsAutoMirror;
                ZPlannerManager.IsAutoMirror = false;
            }

            ClearAll();

            if (project == null) return;

            if (string.IsNullOrWhiteSpace(stackupFile))
            {
                fileName1 = "UNSAVED";
            }
            else 
            {
                fileName1 = stackupFile;
            }
            
            textBoxFile1.Text = fileName1;
            project1 = project;
            //buttonFile2_Click(buttonFile2, new EventArgs());

            if (fileName1 != "UNSAVED" && !string.IsNullOrWhiteSpace(fileName1) && File.Exists(fileName1))
            {
                dateTime1 = File.GetLastWriteTime(fileName1);
            }

            UpdateDiff();

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = isAutoMirror;
            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void DiffToolDialog_Activated(object sender, EventArgs e)
        {
            DateTime dt;
            bool isUpdateRequired = false;

            if (fileName1 != "UNSAVED" && !string.IsNullOrWhiteSpace(fileName1) && File.Exists(fileName1))
            {
                dt = File.GetLastWriteTime(fileName1);
                if (dateTime1 != DateTime.MinValue && dateTime1 != dt)
                {
                    dateTime1 = dt;
                    isUpdateRequired = true;
                }
            }

            if (!string.IsNullOrWhiteSpace(fileName2) && File.Exists(fileName2))
            {
                dt = File.GetLastWriteTime(fileName2);
                if (dateTime2 != DateTime.MinValue && dateTime2 != dt)
                {
                    dateTime2 = dt;
                    isUpdateRequired = true;
                }
            }

            if (isUpdateRequired)
            {
                DialogResult dialogResult = MessageBox.Show("One of the Stackups was changed. Would you like to refresh the Diff Stackups?", "Refresh Stackups Confirmation", MessageBoxButtons.YesNo);
                switch (dialogResult)
                {
                    case DialogResult.Yes:
                        UpdateDiff();
                        break;
                    case DialogResult.No:
                        break;
                }
            }
        }

        private string GetFileToCompare( string currentName)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "Z-planner Stackup|*.z0p";
            openFileDialog.Title = "Open Z-planner Stackup for Comparison";
            if (!Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ProjectPath;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != "")
            {
                return openFileDialog.FileName;
            }

            return currentName;
        }

        private void ClearAll()
        {
            //Clear();

            textBoxFile1.Text = string.Empty;
            textBoxFile2.Text = string.Empty;
            fileName1 = string.Empty;
            fileName2 = string.Empty;
            project1 = null;
            project2 = null;
        }

        private void Clear()
        {
            dataGridView1.Rows.Clear();
            dataGridView1.Columns.Clear();
            dataGridView2.Rows.Clear();
            dataGridView2.Columns.Clear();

            parameters1.Clear();
            parameters2.Clear();
        }

        private void MergeParameters()
        {
            if (!string.IsNullOrWhiteSpace(Options.TheOptions.stackupColumnsOrder))
            {
                List<string> orderList = new List<string>(Options.TheOptions.stackupColumnsOrder.Split(','));
                
                // Stackup
                for (int i = 0; i < orderList.Count; ++i )
                {
                    string parameterId = orderList[i];
                    ZParameter parameter1 = project1.Parameters.Find(x => x.ID == parameterId) ?? project1.SubParameters.Find(x => x.ID == parameterId);
                    ZParameter parameter2 = project2.Parameters.Find(x => x.ID == parameterId) ?? project2.SubParameters.Find(x => x.ID == parameterId);
                    if (parameter1 == null && parameter2 == null) continue;
                    if (parameter1 != null && (parameter1.Table != ZTableType.Stackup || parameter1.IsPrivate || parameter1.IsHidden) ||
                        parameter2 != null && (parameter2.Table != ZTableType.Stackup || parameter2.IsPrivate)) continue;

                    parameters1.Add(parameter1 ?? new ZParameter(parameterId) { ValueType = parameter2.ValueType });
                    parameters2.Add(parameter2 ?? new ZParameter(parameterId) { ValueType = parameter1.ValueType });
                }

                foreach(ZParameter parameter in project1.Parameters)
                {
                    if (parameter.Table != ZTableType.Stackup || parameter.IsPrivate || parameter.IsHidden) continue;
                    if (parameters1.Contains(parameter)) continue;

                    if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                    {
                        foreach(ZParameter subparameter in parameter.SubParameters)
                        {
                            if (subparameter.IsPrivate || subparameter.IsHidden) continue;
                            if (parameters1.Contains(subparameter)) continue;

                            parameters1.Add(subparameter);
                            parameters2.Add(project2.SubParameters.Find(x => x.ID == subparameter.ID) ?? new ZParameter(subparameter.ID) { ValueType = subparameter.ValueType });
                        }
                    }
                    else
                    {
                        parameters1.Add(parameter);
                        parameters2.Add(project2.Parameters.Find(x => x.ID == parameter.ID) ?? new ZParameter(parameter.ID) { ValueType = parameter.ValueType });
                    }
                }

                foreach (ZParameter parameter in project2.Parameters)
                {
                    if (parameter.Table != ZTableType.Stackup || parameter.IsPrivate || parameter.IsHidden) continue;
                    if (parameters2.Contains(parameter)) continue;

                    if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                    {
                        foreach (ZParameter subparameter in parameter.SubParameters)
                        {
                            if (subparameter.IsPrivate || subparameter.IsHidden) continue;
                            if (parameters2.Contains(subparameter)) continue;

                            parameters2.Add(subparameter);
                            parameters1.Add(project1.SubParameters.Find(x => x.ID == subparameter.ID) ?? new ZParameter(subparameter.ID) { ValueType = subparameter.ValueType });
                        }
                    }
                    else
                    {
                        parameters2.Add(parameter);
                        parameters1.Add(project1.Parameters.Find(x => x.ID == parameter.ID) ?? new ZParameter(parameter.ID) { ValueType = parameter.ValueType });
                    }
                }

                // Single
                for (int i = 0; i < orderList.Count; ++i)
                {
                    string parameterId = orderList[i];
                    ZParameter parameter1 = project1.Parameters.Find(x => x.ID == parameterId) ?? project1.SubParameters.Find(x => x.ID == parameterId);
                    ZParameter parameter2 = project2.Parameters.Find(x => x.ID == parameterId) ?? project2.SubParameters.Find(x => x.ID == parameterId);
                    if (parameter1 == null && parameter2 == null) continue;
                    if (parameter1 != null && (parameter1.Table != ZTableType.Single || parameter1.IsPrivate || parameter1.IsHidden) ||
                        parameter2 != null && (parameter2.Table != ZTableType.Single || parameter2.IsPrivate)) continue;

                    parameters1.Add(parameter1 ?? new ZParameter(parameterId) { ValueType = parameter2.ValueType });
                    parameters2.Add(parameter2 ?? new ZParameter(parameterId) { ValueType = parameter1.ValueType });
                }

                foreach (ZParameter parameter in project1.Parameters)
                {
                    if (parameter.Table != ZTableType.Single || parameter.IsPrivate || parameter.IsHidden) continue;
                    if (parameters1.Contains(parameter)) continue;

                    if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                    {
                        foreach (ZParameter subparameter in parameter.SubParameters)
                        {
                            if (subparameter.IsPrivate || subparameter.IsHidden) continue;
                            if (parameters1.Contains(subparameter)) continue;

                            parameters1.Add(subparameter);
                            parameters2.Add(project2.SubParameters.Find(x => x.ID == subparameter.ID) ?? new ZParameter(subparameter.ID) { ValueType = subparameter.ValueType });
                        }
                    }
                    else
                    {
                        parameters1.Add(parameter);
                        parameters2.Add(project2.Parameters.Find(x => x.ID == parameter.ID) ?? new ZParameter(parameter.ID) { ValueType = parameter.ValueType });
                    }
                }

                foreach (ZParameter parameter in project2.Parameters)
                {
                    if (parameter.Table != ZTableType.Single || parameter.IsPrivate || parameter.IsHidden) continue;
                    if (parameters2.Contains(parameter)) continue;

                    if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                    {
                        foreach (ZParameter subparameter in parameter.SubParameters)
                        {
                            if (subparameter.IsPrivate || subparameter.IsHidden) continue;
                            if (parameters2.Contains(subparameter)) continue;

                            parameters2.Add(subparameter);
                            parameters1.Add(project1.SubParameters.Find(x => x.ID == subparameter.ID) ?? new ZParameter(subparameter.ID) { ValueType = subparameter.ValueType });
                        }
                    }
                    else
                    {
                        parameters2.Add(parameter);
                        parameters1.Add(project1.Parameters.Find(x => x.ID == parameter.ID) ?? new ZParameter(parameter.ID) { ValueType = parameter.ValueType });
                    }
                }
            }
        }

        private void FillGrid(ZDataGridView grid, ZPlannerProject project, List<ZParameter> parameters, int rowCount)
        {
            grid.CurrentCellChanged -= DataGridView_CurrentCellChanged;

            foreach (ZParameter parameter in parameters)
            {
                ZDataGridViewColumn column = new ZDataGridViewTextBoxColumn();
                /*ZDataGridViewColumn column;
                switch (parameter.ValueType)
                {
                    case ZValueType.Text:
                    case ZValueType.Link:
                    case ZValueType.Select:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                    case ZValueType.Number:
                        column = new ZDataGridViewNumberBoxColumn();
                        break;
                    case ZValueType.Table:
                        column = new ZDataGridViewFrequencyDependedColumn();
                        break;
                    case ZValueType.Percent:
                        column = new ZDataGridViewPercentBoxColumn();
                        break;
                    case ZValueType.Boolean:
                        column = new ZDataGridViewCheckBoxColumn();
                        break;
                    default:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                }*/

                column.Name = parameter.ID;
                column.HeaderText = parameter.Title ?? string.Empty;
                column.ReadOnly = true;
                column.Tag = parameter;
                grid.Columns.Add(column);
            }

            for(int i = 0; i < rowCount; ++i)
            {
                ZLayer layer = (i < project.Stackup.Layers.Count) ? project.Stackup.Layers[i] : null;
                ZLayer singleLayer = (project.Stackup.ActiveSingle != null && i < project.Stackup.ActiveSingle.Layers.Count) ? project.Stackup.ActiveSingle.Layers[i] : null;
                ZDataGridViewRow row = new ZDataGridViewRow();
                row.ReadOnly = true;
                row.Tag = layer;

                foreach(ZParameter parameter in parameters)
                {
                    ZLayerParameter layerParameter = null;
                    if (layer != null) layerParameter = layer.LayerParameters.Find(x => x.ID == parameter.ID);
                    if (layerParameter == null && singleLayer != null) layerParameter = singleLayer.LayerParameters.Find(x => x.ID == parameter.ID);
                    if (layerParameter == null) layerParameter = new ZLayerParameter(parameter.ID);

                    ZDataGridViewTextBoxCell cell = new ZDataGridViewTextBoxCell();
                    /*DataGridViewCell cell;
                    switch (parameter.ValueType)
                    {
                        case ZValueType.Text:
                        case ZValueType.Link:
                        case ZValueType.Select:
                            cell = new ZDataGridViewTextBoxCell();
                            break;
                        case ZValueType.Number:
                            cell = new ZDataGridViewNumberBoxCell();
                            break;
                        case ZValueType.Table:
                            cell = new ZDataGridViewFrequencyDependedCell();
                            break;
                        case ZValueType.Percent:
                            cell = new ZDataGridViewPercentBoxCell();
                            break;
                        case ZValueType.Boolean:
                            cell = new ZDataGridViewCheckBoxCell();
                            break;
                        default:
                            cell = new ZDataGridViewTextBoxCell();
                            break;
                    }*/

                    cell.Value = layerParameter.Value;
                    cell.Tag = layerParameter;
                    switch(parameter.ValueType)
                    {
                        case ZValueType.Number:
                        case ZValueType.Percent:
                        case ZValueType.Ratio:
                        case ZValueType.Table:
                            cell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                            break;
                        default:
                            cell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft;
                            break;
                    }

                    row.Cells.Add(cell);
                    cell.ReadOnly = true;

                    row.HeaderCell = new ZDataGridViewRowHeaderCell();
                    row.HeaderCell.Value = (i + 1).ToString();
                }

                grid.Rows.Add(row);
            }

            grid.ColumnHeaderNamesVisible = ZPlannerManager.IsHeadersVisible;
            grid.ColumnHeadersHeight = (ZPlannerManager.IsHeadersVisible) ? 73 : 50;
            grid.RowHeadersVisible = ZPlannerManager.IsHeadersVisible;

            if (ZPlannerManager.IsHeadersVisible) RecalculateColumnHeaderName(grid);

            grid.CurrentCellChanged += DataGridView_CurrentCellChanged;
        }

        void DataGridView_CurrentCellChanged(object sender, EventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            Rectangle headersRectangle = grid.DisplayRectangle;
            headersRectangle.Height = grid.ColumnHeadersHeight;
            grid.Invalidate(headersRectangle);
        }

        private void CompareGridsData()
        {
            //if (dataGridView1.Columns.Count != dataGridView2.Columns.Count || dataGridView1.Rows.Count != dataGridView2.Rows.Count) return;

            /*for (int cIndex = 0; cIndex < dataGridView1.Columns.Count; ++cIndex)
            {
                if (string.Compare(dataGridView1.Columns[cIndex].HeaderText, dataGridView2.Columns[cIndex].HeaderText) != 0)
                {
                    (dataGridView1.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                    (dataGridView2.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                }
            }*/
            for (int cIndex = 0; cIndex < Math.Max(dataGridView1.Columns.Count, dataGridView2.Columns.Count); ++cIndex)
            {
                if (dataGridView1.Columns.Count > cIndex && dataGridView2.Columns.Count > cIndex)
                {
                    if (string.Compare(dataGridView1.Columns[cIndex].HeaderText, dataGridView2.Columns[cIndex].HeaderText) != 0)
                    {
                        (dataGridView1.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                        (dataGridView2.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                    }
                    else
                    {
                        (dataGridView1.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = Color.Empty;
                        (dataGridView2.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = Color.Empty;
                    }
                }
                else
                {
                    if (dataGridView1.Columns.Count > cIndex) (dataGridView1.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                    if (dataGridView2.Columns.Count > cIndex) (dataGridView2.Columns[cIndex].HeaderCell as ZDataGridViewColumnHeaderCell).Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                }
            }

            /*for (int rIndex = 0; rIndex < dataGridView1.Rows.Count; ++rIndex)
            {
                ZLayer layer1 = dataGridView1.Rows[rIndex].Tag as ZLayer;
                ZLayer layer2 = dataGridView2.Rows[rIndex].Tag as ZLayer;

                ZLayerType? layerType1 = (layer1 == null) ? null : layer1.GetLayerType();
                ZLayerType? layerType2 = (layer2 == null) ? null : layer2.GetLayerType();
                if (layerType1 != null) dataGridView1.Rows[rIndex].DefaultCellStyle.BackColor = ZColorConstants.GetLayerColor(layerType1);
                if (layerType2 != null) dataGridView2.Rows[rIndex].DefaultCellStyle.BackColor = ZColorConstants.GetLayerColor(layerType2);

                for (int cIndex = 0; cIndex < dataGridView1.Columns.Count; ++cIndex)
                    if (string.Compare(dataGridView1.Rows[rIndex].Cells[cIndex].Value as string, dataGridView2.Rows[rIndex].Cells[cIndex].Value as string) != 0)
                    {                    
                        dataGridView1.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                        dataGridView2.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                    }
            }*/
            for (int rIndex = 0; rIndex < Math.Max(dataGridView1.Rows.Count, dataGridView2.Rows.Count); ++rIndex)
            {
                if (dataGridView1.Rows.Count > rIndex)
                {
                    ZLayer layer1 = dataGridView1.Rows[rIndex].Tag as ZLayer;
                    ZLayerType? layerType1 = (layer1 == null) ? null : layer1.GetLayerType();
                    if (layerType1 != null) dataGridView1.Rows[rIndex].DefaultCellStyle.BackColor = ZColorConstants.GetLayerColor(layerType1);
                    else dataGridView1.Rows[rIndex].DefaultCellStyle.BackColor = Color.Empty;
                }

                if (dataGridView2.Rows.Count > rIndex)
                {
                    ZLayer layer2 = dataGridView2.Rows[rIndex].Tag as ZLayer;
                    ZLayerType? layerType2 = (layer2 == null) ? null : layer2.GetLayerType();
                    if (layerType2 != null) dataGridView2.Rows[rIndex].DefaultCellStyle.BackColor = ZColorConstants.GetLayerColor(layerType2);
                    else dataGridView2.Rows[rIndex].DefaultCellStyle.BackColor = Color.Empty;
                }

                for (int cIndex = 0; cIndex < Math.Max(dataGridView1.Columns.Count, dataGridView2.Columns.Count); ++cIndex)
                {
                    if (dataGridView1.Rows.Count > rIndex && dataGridView2.Rows.Count > rIndex && dataGridView1.Columns.Count > cIndex && dataGridView2.Columns.Count > cIndex)
                    {
                        if (string.Compare(dataGridView1.Rows[rIndex].Cells[cIndex].FormattedValue as string, dataGridView2.Rows[rIndex].Cells[cIndex].FormattedValue as string) != 0)
                        {
                            ZParameter parameter = dataGridView1.Columns[cIndex].Tag as ZParameter;
                            double d1, d2;
                            if (parameter != null && (parameter.ValueType != ZValueType.Number || parameter.ValueType != ZValueType.Percent) &&
                                    double.TryParse(dataGridView1.Rows[rIndex].Cells[cIndex].FormattedValue as string, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) &&
                                    double.TryParse(dataGridView2.Rows[rIndex].Cells[cIndex].FormattedValue as string, NumberStyles.Any, CultureInfo.InvariantCulture, out d2) &&
                                    !double.IsNaN(d1) && !double.IsNaN(d2) && (d1 == d2 || Math.Abs(d1 - d2) / Math.Max(Math.Abs(d1), Math.Abs(d2)) * 100 <= Sensitivity))
                            {
                                dataGridView1.Rows[rIndex].Cells[cIndex].Style.ForeColor = Color.Empty;
                                dataGridView2.Rows[rIndex].Cells[cIndex].Style.ForeColor = Color.Empty;
                            }
                            else
                            {
                                dataGridView1.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                                dataGridView2.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                            }
                        }
                        else
                        {
                            dataGridView1.Rows[rIndex].Cells[cIndex].Style.ForeColor = Color.Empty;
                            dataGridView2.Rows[rIndex].Cells[cIndex].Style.ForeColor = Color.Empty;
                        }
                    }
                    else 
                    {
                        if (dataGridView1.Rows.Count > rIndex && dataGridView1.Columns.Count > cIndex) dataGridView1.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                        if (dataGridView2.Rows.Count > rIndex && dataGridView2.Columns.Count > cIndex) dataGridView2.Rows[rIndex].Cells[cIndex].Style.ForeColor = ZColorConstants.DiffToolNoMatchColor;
                    }
                }
            }
        }

        private void ResizeGrids()
        {
            dataGridView1.Refresh();
            dataGridView2.Refresh();

            int headerHeight = Math.Max(dataGridView1.ColumnHeadersHeight, dataGridView2.ColumnHeadersHeight);
            dataGridView1.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            dataGridView2.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;
            dataGridView1.ColumnHeadersHeight = headerHeight;
            dataGridView2.ColumnHeadersHeight = headerHeight;

            int headerWidth = Math.Max(dataGridView1.RowHeadersWidth, dataGridView2.RowHeadersWidth);
            dataGridView1.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            dataGridView2.RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing;
            dataGridView1.RowHeadersWidth = headerWidth;
            dataGridView2.RowHeadersWidth = headerWidth;

            for(int cIndex = 0; cIndex < Math.Max(dataGridView1.Columns.Count, dataGridView2.Columns.Count); ++cIndex)
            {
                int columnWidth = 0;

                if (dataGridView1.Columns.Count > cIndex)
                {
                    dataGridView1.Columns[cIndex].SortMode = DataGridViewColumnSortMode.NotSortable;
                    dataGridView1.Columns[cIndex].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
                    columnWidth = dataGridView1.Columns[cIndex].Width;
                }

                if (dataGridView2.Columns.Count > cIndex)
                {
                    dataGridView2.Columns[cIndex].SortMode = DataGridViewColumnSortMode.NotSortable;
                    dataGridView2.Columns[cIndex].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
                    columnWidth = dataGridView2.Columns[cIndex].Width;
                }

                if (dataGridView1.Columns.Count > cIndex && dataGridView2.Columns.Count > cIndex) columnWidth = Math.Max(dataGridView1.Columns[cIndex].Width, dataGridView2.Columns[cIndex].Width);

                if (dataGridView1.Columns.Count > cIndex)
                {
                    dataGridView1.Columns[cIndex].AutoSizeMode = DataGridViewAutoSizeColumnMode.NotSet;
                    dataGridView1.Columns[cIndex].Width = columnWidth;
                }

                if (dataGridView2.Columns.Count > cIndex)
                {
                    dataGridView2.Columns[cIndex].AutoSizeMode = DataGridViewAutoSizeColumnMode.NotSet;
                    dataGridView2.Columns[cIndex].Width = columnWidth;
                }
            }

            for (int rIndex = 0; rIndex < Math.Max(dataGridView1.Rows.Count, dataGridView2.Rows.Count); ++rIndex)
            {
                if (dataGridView1.Rows.Count > rIndex && dataGridView2.Rows.Count > rIndex)
                {
                    int rowHeight = Math.Max(dataGridView1.Rows[rIndex].Height, dataGridView2.Rows[rIndex].Height);
                    dataGridView1.Rows[rIndex].Height = rowHeight;
                    dataGridView2.Rows[rIndex].Height = rowHeight;
                }
            }
        }

        private void UpdateDiff()
        {
            Clear();

            bool ignoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                if (fileName1 != "UNSAVED" && !string.IsNullOrWhiteSpace(fileName1)) project1 = DataProvider.Instance.OpenZPlannerProject(fileName1);
                if (!string.IsNullOrWhiteSpace(fileName2)) project2 = DataProvider.Instance.OpenZPlannerProject(fileName2);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(ignoreCommands);
            }

            if (project1 == null || project1.Stackup == null) 
            {
                project1 = new ZPlannerProject();
                project1.Stackup.Layers.Clear();
                foreach (var single in project1.Stackup.Singles) single.Layers.Clear();
                project1.Stackup.Singles.Clear();
                foreach (var pair in project1.Stackup.Pairs) pair.Layers.Clear();
                project1.Stackup.Pairs.Clear();
            }

            if (project2 == null || project2.Stackup == null)
            {
                project2 = new ZPlannerProject();
                project2.Stackup.Layers.Clear();
                foreach (var single in project2.Stackup.Singles) single.Layers.Clear();
                project2.Stackup.Singles.Clear();
                foreach (var pair in project2.Stackup.Pairs) pair.Layers.Clear();
                project2.Stackup.Pairs.Clear();
            }

            MergeParameters();

            FillGrid(dataGridView1, project1, parameters1, project1.Stackup.Layers.Count);
            FillGrid(dataGridView2, project2, parameters2, project2.Stackup.Layers.Count);

            //FillGrid(dataGridView1, project1, parameters1, Math.Max(project1.Stackup.Layers.Count, project2.Stackup.Layers.Count));
            //FillGrid(dataGridView2, project2, parameters2, Math.Max(project1.Stackup.Layers.Count, project2.Stackup.Layers.Count));

            CompareGridsData();

            ResizeGrids();
        }

        private void buttonFile1_Click(object sender, EventArgs e)
        {
            fileName1 = GetFileToCompare(textBoxFile1.Text);
            textBoxFile1.Text = fileName1;

            if (fileName1 != "UNSAVED" && !string.IsNullOrWhiteSpace(fileName1) && File.Exists(fileName1))
            {
                dateTime1 = File.GetLastWriteTime(fileName1);
            }

            UpdateDiff();
        }

        private void buttonFile2_Click(object sender, EventArgs e)
        {
            fileName2 = GetFileToCompare(textBoxFile2.Text);
            textBoxFile2.Text = fileName2;

            if (!string.IsNullOrWhiteSpace(fileName2) && File.Exists(fileName2))
            {
                dateTime2 = File.GetLastWriteTime(fileName2);
            }

            UpdateDiff();
        }

        private void buttonClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void checkBoxTileHorizontal_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxTileHorizontal.Checked && splitContainer.ClientSize.Height > 0 && splitContainer.ClientSize.Width > 0)
            {
                splitContainer.Orientation = Orientation.Horizontal;
                splitContainer.SplitterWidth = (splitContainer.ClientSize.Height % 2 > 0) ? 3 : 2;
                splitContainer.SplitterDistance = (int)(splitContainer.ClientSize.Height * 0.5) - 1;
                dataGridView2.Location = dataGridView1.Location;
                dataGridView2.Size = dataGridView1.Size;
            }

            checkBoxTileVertical.Checked = !checkBoxTileHorizontal.Checked;
        }

        private void checkBoxTileVertical_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxTileVertical.Checked && splitContainer.ClientSize.Height > 0 && splitContainer.ClientSize.Width > 0)
            {
                splitContainer.Orientation = Orientation.Vertical;
                splitContainer.SplitterWidth = (splitContainer.ClientSize.Width % 2 > 0) ? 3 : 2;
                splitContainer.SplitterDistance = (int)(splitContainer.ClientSize.Width * 0.5) - 1;
                dataGridView2.Location = dataGridView1.Location;
                dataGridView2.Size = dataGridView1.Size;
            }

            checkBoxTileHorizontal.Checked = !checkBoxTileVertical.Checked;
        }

        private void splitContainer_Resize(object sender, EventArgs e)
        {
            if (checkBoxTileHorizontal.Checked) checkBoxTileHorizontal_CheckedChanged(checkBoxTileHorizontal, new EventArgs());
            else checkBoxTileVertical_CheckedChanged(checkBoxTileVertical, new EventArgs());
        }

        private void dataGridView1_Scroll(object sender, ScrollEventArgs e)
        {
            if (e.ScrollOrientation == ScrollOrientation.HorizontalScroll)
                dataGridView2.HorizontalScrollingOffset = e.NewValue;
            else
                dataGridView2.FirstDisplayedScrollingRowIndex = e.NewValue;
        }

        private void dataGridView2_Scroll(object sender, ScrollEventArgs e)
        {
            if (e.ScrollOrientation == ScrollOrientation.HorizontalScroll)
                dataGridView1.HorizontalScrollingOffset = e.NewValue;
            else
                dataGridView1.FirstDisplayedScrollingRowIndex = e.NewValue;
        }

        private void dataGridView1_CellStateChanged(object sender, DataGridViewCellStateChangedEventArgs e)
        {
            if (e.StateChanged != DataGridViewElementStates.Selected) return;
            int rowIndex = e.Cell.RowIndex;
            int columnIndex = e.Cell.ColumnIndex;

            if (dataGridView2.Columns.Count <= columnIndex || dataGridView2.Rows.Count <= rowIndex || dataGridView2[columnIndex, rowIndex].Selected == e.Cell.Selected) return;
            dataGridView2[columnIndex, rowIndex].Selected = e.Cell.Selected;
        }

        private void dataGridView1_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            if (e.StateChanged != DataGridViewElementStates.Selected) return;
            int rowIndex = e.Row.Index;

            if (dataGridView2.Rows.Count <= rowIndex || dataGridView2.Rows[rowIndex].Selected == e.Row.Selected) return;
            dataGridView2.Rows[rowIndex].Selected = e.Row.Selected;
        }

        private void dataGridView2_CellStateChanged(object sender, DataGridViewCellStateChangedEventArgs e)
        {
            if (e.StateChanged != DataGridViewElementStates.Selected) return;
            int rowIndex = e.Cell.RowIndex;
            int columnIndex = e.Cell.ColumnIndex;

            if (dataGridView1.Columns.Count <= columnIndex || dataGridView1.Rows.Count <= rowIndex || dataGridView1[columnIndex, rowIndex].Selected == e.Cell.Selected) return;
            dataGridView1[columnIndex, rowIndex].Selected = e.Cell.Selected;
        }

        private void dataGridView2_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            if (e.StateChanged != DataGridViewElementStates.Selected) return;
            int rowIndex = e.Row.Index;

            if (dataGridView1.Rows.Count <= rowIndex || dataGridView1.Rows[rowIndex].Selected == e.Row.Selected) return;
            dataGridView1.Rows[rowIndex].Selected = e.Row.Selected;
        }

        void DiffToolDialog_VisibleChanged(object sender, System.EventArgs e)
        {
            if (this.Visible && project1 != null && project2 == null) buttonFile2_Click(buttonFile2, new EventArgs());
        }

        private void RecalculateColumnHeaderName(ZDataGridView grid)
        {
            int index = 0;

            foreach (ZDataGridViewColumn column in grid.GetColumnsOrderedByDisplayIndex())
            {
                column.HeaderName = column.GetHeaderNameByOrder(index++);
            }
        }

        private void checkBoxCenterLine_CheckedChanged(object sender, EventArgs e)
        {
            ZPlannerManager.IsCenterLineVisible = checkBoxCenterLine.Checked;
            dataGridView1.Invalidate();
            dataGridView2.Invalidate();
        }

        private void DiffToolDialog_FormClosing(object sender, FormClosingEventArgs e)
        {
            ZPlannerManager.IsCenterLineVisible = isCenterLineVisible;
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            CaptureScreen();
            Clipboard.SetImage(memoryImage);
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            CaptureScreen();

            /*PrintPreviewDialog*/
            PrintDialog dlg = new PrintDialog();//PrintPreviewDialog();
            dlg.Document = printDocument1;
            DialogResult result = dlg.ShowDialog();

            if (result == DialogResult.OK)
            {
                printDocument1.Print();
            }
        }

        Bitmap memoryImage;

        private void CaptureScreen()
        {
            memoryImage = new Bitmap(panelContent.Width, panelContent.Height);
            panelContent.DrawToBitmap(memoryImage, new Rectangle(0, 0, memoryImage.Width, memoryImage.Height));
        }

        private void printDocument1_PrintPage(System.Object sender,
           System.Drawing.Printing.PrintPageEventArgs e)
        {
            e.Graphics.DrawImage(memoryImage, 0, 0);
        }

        void sensitivityMaskedTextBox_Enter(object sender, System.EventArgs e)
        {
            sensitivityMaskedTextBox.Focus();
            sensitivityMaskedTextBox.SelectionStart = 0;
            sensitivityMaskedTextBox.SelectionLength = 2;
        }

        void sensitivityMaskedTextBox_MouseClick(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            sensitivityMaskedTextBox.Focus();
            sensitivityMaskedTextBox.SelectionStart = 0;
            sensitivityMaskedTextBox.SelectionLength = 2;
        }

        private void sensitivityMaskedTextBox_TextChanged(object sender, EventArgs e)
        {
            int value;
            string text = sensitivityMaskedTextBox.Text;
            if (text != null && int.TryParse(text, out value)) sensitivity = value;
            else sensitivity = 0;

            CompareGridsData();
        }
    }
}
