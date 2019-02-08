using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class CustomFilterDialog : Form
    {
        private List<ZParameter> parameters;
        private string name;
        private string path;
        Dictionary<string, string> filterFiles = new Dictionary<string, string>();

        internal CustomFilterDialog()
        {
            InitializeComponent();
        }

        internal CustomFilterDialog(List<ZParameter> parameters)
        {
            this.parameters = parameters;
            InitializeComponent();
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.dataGridViewFilter.CurrentCellChanged += DataGridView_CurrentCellChanged;
            this.dataGridViewSort.CurrentCellChanged += DataGridView_CurrentCellChanged;
        }

        void DataGridView_CurrentCellChanged(object sender, EventArgs e)
        {
            DataGridView grid = sender as DataGridView;
            if (grid == null) return;

            DataGridViewCell cell = grid.CurrentCell;
            if (cell == null) return;

            grid.BeginEdit(true);
            ComboBox cb = grid.EditingControl as ComboBox;
            if (cb != null && string.IsNullOrWhiteSpace((string)cb.SelectedItem)) cb.DroppedDown = true;
        }

        public string FilterPath { get { return path; } }

        public void SetFilterAndSort(string path, string name, string filterString, string sortString)
        {
            filterString = filterString.Trim() + " AND ";
            sortString = sortString.Trim();
            this.name = name;
            this.path = path;
            if (string.IsNullOrWhiteSpace(this.path)) 
            {
                foreach (var filePath in Directory.EnumerateFiles(ZSettings.CustomFiltersFolder, "*.filter"))
                {
                    using (StreamReader filterReader = new StreamReader(File.Open(filePath, FileMode.Open, FileAccess.Read)))
                    {
                        filterFiles.Add(filterReader.ReadLine(), filePath);
                    }
                }

                this.path = string.Empty;

                while (this.path == string.Empty)
                {
                    string tempPath = Path.Combine(ZSettings.CustomFiltersFolder, DateTime.Now.ToString("yyyy MMM dd HH_mm_ss") + ".filter");
                    if (!filterFiles.Values.Contains(tempPath)) this.path = tempPath;
                }
            }

            textBoxFilterName.Text = name;

            Regex filtersPattern = new Regex(@"([(]?(?<filter>.+?)[)]? AND )+?", RegexOptions.Compiled | RegexOptions.Singleline);
            Regex columnPattern = new Regex(@"^[\[](?<column>.+?)[\]]\s(?<command>IN|NOT LIKE|LIKE|=|<>|>|<|>=|<=)\s[(]?(?<value>.+?)[)]?$", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match filterMatch in filtersPattern.Matches(filterString))
            {
                if (!filterMatch.Success) continue;
                string filter = filterMatch.Groups["filter"].Value;
                Match columnMatch = columnPattern.Match(filter);
                if (!columnMatch.Success) continue;
                string columnName = columnMatch.Groups["column"].Value;
                string command = columnMatch.Groups["command"].Value.Trim();
                bool isBeginLike = false;
                bool isEndLike = false;
                string complexValue = columnMatch.Groups["value"].Value;
                string[] values = complexValue.Split(new string[] { "," }, System.StringSplitOptions.None);
                if (values.Length == 0) continue;
                for (int i = 0; i < values.Length; ++i)
                {
                    values[i] = values[i].Trim(' ', '\'');
                    isEndLike = isEndLike || values[i].StartsWith("%");
                    isBeginLike = isBeginLike || values[i].EndsWith("%");
                    values[i] = values[i].Trim('%');
                }

                string condition = string.Empty;

                if (string.IsNullOrEmpty(columnName)) continue;

                ZParameter parameter = parameters.Find(x => x.ID == columnName);
                if (parameter == null) continue;

                switch (command)
                {
                    case "IN":
                        condition = "includes";
                        break;
                    case "=":
                        condition = "equals";
                        break;
                    case "<>":
                        condition = "does not equal";
                        break;
                    case "<":
                        condition = "less than";
                        break;
                    case "<=":
                        condition = "less than or equal to";
                        break;
                    case ">":
                        condition = "greater than";
                        break;
                    case ">=":
                        condition = "greater than or equal to";
                        break;
                    case "NOT LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike) condition = "does not contain";
                            else condition = "does not begin with";
                        }
                        else
                        {
                            if (isEndLike) condition = "does not end with";
                            else condition = "does not equal";
                        }
                        break;
                    case "LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike) condition = "contains";
                            else condition = "begins with";
                        }
                        else
                        {
                            if (isEndLike) condition = "ends with";
                            else condition = "equals";
                        }
                        break;
                }

                AddFilterLine(parameter.Title, condition, string.Join(", ", values));
            }

            Regex sortPattern = new Regex(@"[\[](?<column>\S+?)[\]]\s(?<direction>ASC|DESC)[,]?", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match sortMatch in sortPattern.Matches(sortString))
            {
                if (!sortMatch.Success) continue;
                string columnName = sortMatch.Groups["column"].Value;
                string sortDirection = sortMatch.Groups["direction"].Value;

                if (string.IsNullOrEmpty(columnName)) continue;

                ZParameter parameter = parameters.Find(x => x.ID == columnName);
                if (parameter == null) continue;

                AddSortLine(parameter.Title, sortDirection == "ASC" ? "Ascending" : "Descending");
            }
        }

        public void UpdateFilterAndSort(string path, string name, string filterString, string sortString)
        {
            filterString = filterString.Trim();
            sortString = sortString.Trim();
            name = name.Trim();
            this.path = path;

            if (InternalSaveFilterAndSort(name, filterString, sortString)) DialogResult = System.Windows.Forms.DialogResult.OK;
            else DialogResult = System.Windows.Forms.DialogResult.Cancel;
        }

        private bool SaveFilterAndSort()
        {
            string filterName = textBoxFilterName.Text;
            string filterString = GetFilterString();
            string sortString = GetSortString();

            return InternalSaveFilterAndSort(filterName, filterString, sortString);
        }

        private bool InternalSaveFilterAndSort(string filterName, string filterString, string sortString)
        {
            if (filterName != name && filterFiles.Keys.Contains(filterName))
            {
                MessageBox.Show("A filter with this name already exists.", "Filter Name", MessageBoxButtons.OK);
                return false;
            }

            using (StreamWriter filterWriter = new StreamWriter(File.Open(this.path, FileMode.Create, FileAccess.Write)))
            {
                filterWriter.WriteLine(filterName);
                filterWriter.WriteLine("FILTER:" + filterString);
                filterWriter.WriteLine("SORT:" + sortString);
            }

            return true;
        }

        private string GetFilterString()
        {
            string filterString = string.Empty;
            foreach (DataGridViewRow row in dataGridViewFilter.Rows)
            {
                if (row.Cells[0].Value == null || row.Cells[1].Value == null || row.Cells[2].Value == null) continue;
                string fieldName = row.Cells[0].Value.ToString();
                string condition = row.Cells[1].Value.ToString();
                string value = row.Cells[2].Value.ToString();
                if (string.IsNullOrWhiteSpace(fieldName) || string.IsNullOrWhiteSpace(condition)) continue;
                

                ZParameter parameter = parameters.Find(x => x.Title == fieldName);
                if (parameter == null) continue;

                string operation = GetOperation(parameter.ValueType, condition);
                if (operation == null) continue;

                string formatedValue = GetFormatedValue(parameter.ValueType, condition, value);
                if (formatedValue == null) continue;

                filterString += ((filterString == string.Empty) ? "" : " AND ") + "([" + parameter.ID + "] " + operation + " " + formatedValue + ")";
            }

            return filterString;
        }

        private string GetSortString()
        {
            string sortString = string.Empty;
            foreach(DataGridViewRow row in dataGridViewSort.Rows)
            {
                if (row.Cells[0].Value == null || row.Cells[1].Value == null) continue;
                string fieldName = row.Cells[0].Value.ToString();
                string sortOrder = row.Cells[1].Value.ToString();

                if (string.IsNullOrWhiteSpace(fieldName) || string.IsNullOrWhiteSpace(sortOrder)) continue;

                ZParameter parameter = parameters.Find(x => x.Title == fieldName);
                if (parameter == null) continue;

                sortString += ((sortString == string.Empty) ? "" : ", ") + "[" + parameter.ID + "] " + ((sortOrder == "Ascending") ? "ASC" : "DESC");
            }

            return sortString;
        }

        private void AddFilterLine()
        {
            AddFilterLine(null, null, null);
            if (dataGridViewFilter.RowCount > 0 && dataGridViewFilter.ColumnCount > 0)
            {
                dataGridViewFilter.CurrentCell = dataGridViewFilter.Rows[dataGridViewFilter.RowCount - 1].Cells[0];
            }
        }

        private void AddFilterLine(string val1, string val2, string val3)
        {
            DataGridViewRow row = new DataGridViewRow();
            DataGridViewComboBoxCell cell1 = new DataGridViewComboBoxCell();
            cell1.Items.AddRange(parameters.Select(x => x.Title).ToArray());
            cell1.DisplayStyle = DataGridViewComboBoxDisplayStyle.Nothing;
            cell1.FlatStyle = FlatStyle.Flat;
            row.Cells.Add(cell1);
            DataGridViewComboBoxCell cell2 = new DataGridViewComboBoxCell(); 
            cell2.DisplayStyle = DataGridViewComboBoxDisplayStyle.Nothing;
            cell2.FlatStyle = FlatStyle.Flat;
            row.Cells.Add(cell2);
            cell2.ReadOnly = true;
            DataGridViewTextBoxCell cell3 = new DataGridViewTextBoxCell();
            row.Cells.Add(cell3);
            cell3.ReadOnly = true;

            dataGridViewFilter.Rows.Add(row);

            if (val1 == null || !cell1.Items.Contains(val1)) return;
            cell1.Value = val1;

            if (val2 == null || !cell2.Items.Contains(val2)) return;
            cell2.Value = val2;

            if (val3 == null) return;
            cell3.Value = val3;
        }

        private void AddSortLine()
        {
            AddSortLine(null, null);
            if (dataGridViewSort.RowCount > 0 && dataGridViewSort.ColumnCount > 0)
            {
                dataGridViewSort.CurrentCell = dataGridViewSort.Rows[dataGridViewSort.RowCount - 1].Cells[0];
            }
        }

        private void AddSortLine(string val1, string val2)
        {
            DataGridViewRow row = new DataGridViewRow();
            DataGridViewComboBoxCell cell1 = new DataGridViewComboBoxCell();
            cell1.Items.AddRange(parameters.Select(x => x.Title).ToArray());
            cell1.DisplayStyle = DataGridViewComboBoxDisplayStyle.Nothing;
            cell1.FlatStyle = FlatStyle.Flat;
            row.Cells.Add(cell1);
            DataGridViewComboBoxCell cell2 = new DataGridViewComboBoxCell();
            cell2.DisplayStyle = DataGridViewComboBoxDisplayStyle.Nothing;
            cell2.FlatStyle = FlatStyle.Flat;
            row.Cells.Add(cell2);
            cell2.ReadOnly = true;

            dataGridViewSort.Rows.Add(row);

            if (val1 == null || !cell1.Items.Contains(val1)) return;
            cell1.Value = val1;

            if (val2 == null || !cell2.Items.Contains(val2)) return;
            cell2.Value = val2;
        }

        private void ClearFilter()
        {
            dataGridViewFilter.Rows.Clear();
        }

        private void ClearSort()
        {
            dataGridViewSort.Rows.Clear();
        }

        private string[] GetConditions(ZValueType valueType)
        {
            List<string> conditions = new List<string>();
            switch(valueType)
            {
                case ZValueType.Boolean:
                    conditions.AddRange(new String[] {
                        "includes"
                    });
                    break;
                case ZValueType.Number:
                case ZValueType.Percent:
                case ZValueType.Table:
                    conditions.AddRange(new String[] {
                        "equals",
                        "does not equal",
                        "greater than",
                        "greater than or equal to",
                        "less than",
                        "less than or equal to",
                        "includes"
                    });
                    break;
                case ZValueType.Text:
                case ZValueType.Select:
                case ZValueType.Link:
                    conditions.AddRange(new String[] {
                        "equals",
                        "does not equal",
                        "begins with",
                        "does not begin with",
                        "ends with",
                        "does not end with",
                        "contains",
                        "does not contain",
                        "includes"
                    });
                    break;
            }

            return conditions.ToArray();
        }

        private string GetOperation(ZValueType valueType, string condition)
        {
            switch (condition)
            {
                case "includes":
                    return "IN";
                case "equals":
                    if (valueType == ZValueType.Number || valueType == ZValueType.Percent || valueType == ZValueType.Table)
                        return "=";
                    else 
                        return "LIKE";
                case "does not equal":
                    if (valueType == ZValueType.Number || valueType == ZValueType.Percent || valueType == ZValueType.Table)
                        return "<>";
                    else
                        return "NOT LIKE";
                case "greater than":
                    return ">";
                case "greater than or equal to":
                    return ">=";
                case "less than":
                    return "<";
                case "less than or equal to":
                    return "<=";
                case "begins with":
                case "ends with":
                case "contains":
                    return "LIKE";
                case "does not begin with":
                case "does not end with":
                case "does not contain":
                    return "NOT LIKE";
            }

            return null;
        }

        private string GetFormatedValue(ZValueType valueType, string condition, string value)
        {
            string formatedValue = value;
            if (valueType == ZValueType.Boolean)
            {
                formatedValue = formatedValue.Replace("YES", "True");
                formatedValue = formatedValue.Replace("Yes", "True");
                formatedValue = formatedValue.Replace("yes", "True");
                formatedValue = formatedValue.Replace("true", "True");
                formatedValue = formatedValue.Replace("TRUE", "True");
                formatedValue = formatedValue.Replace("NO", "False");
                formatedValue = formatedValue.Replace("No", "False");
                formatedValue = formatedValue.Replace("no", "False");
                formatedValue = formatedValue.Replace("false", "False");
                formatedValue = formatedValue.Replace("FALSE", "False");
            }

            switch(condition)
            {
                case "includes":
                    formatedValue = "(" + formatedValue.Trim('(', ')') + ")";
                    break;
                case "begins with":
                case "does not begin with":
                    formatedValue = formatedValue + "%";
                    break;
                case "ends with":
                case "does not end with":
                    formatedValue = "%" + formatedValue;
                    break;
                case "contains":
                case "does not contain":
                    formatedValue = "%" + formatedValue + "%";
                    break;
            }

            return formatedValue;
        }

        private void dataGridViewFilter_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            DataGridViewSelectedRowCollection selectedRows = dataGridViewFilter.SelectedRows;
            buttonDeleteFilter.Enabled = (selectedRows.Count == 1);
        }

        private void buttonAddFilter_Click(object sender, EventArgs e)
        {
            AddFilterLine();
        }

        private void buttonDeleteFilter_Click(object sender, EventArgs e)
        {
            DataGridViewSelectedRowCollection selectedRows = dataGridViewFilter.SelectedRows;
            if(selectedRows.Count == 1)
            {
                dataGridViewFilter.Rows.Remove(selectedRows[0]);
            }
        }

        private void buttonClerFilter_Click(object sender, EventArgs e)
        {
            ClearFilter();
            AddFilterLine();
        }

        private void dataGridViewSort_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            DataGridViewSelectedRowCollection selectedRows = dataGridViewSort.SelectedRows;
            buttonDeleteSort.Enabled = (selectedRows.Count == 1);
        }

        private void buttonAddSort_Click(object sender, EventArgs e)
        {
            AddSortLine();
        }

        private void buttonDeleteSort_Click(object sender, EventArgs e)
        {
            DataGridViewSelectedRowCollection selectedRows = dataGridViewSort.SelectedRows;
            if (selectedRows.Count == 1)
            {
                dataGridViewSort.Rows.Remove(selectedRows[0]);
            }
        }

        private void buttonClearSort_Click(object sender, EventArgs e)
        {
            ClearSort();
            AddSortLine();
        }

        private void dataGridViewFilter_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.ColumnIndex < 0) return;

            DataGridViewCell currentCell = dataGridViewFilter.Rows[e.RowIndex].Cells[e.ColumnIndex];
            string currentValue = currentCell.Value.ToString();

            switch (e.ColumnIndex)
            {
                case 0:
                    {
                        DataGridViewComboBoxCell cell2 = dataGridViewFilter.Rows[e.RowIndex].Cells[1] as DataGridViewComboBoxCell;
                        if (cell2 == null) return;

                        cell2.Items.Clear();

                        DataGridViewTextBoxCell cell3 = dataGridViewFilter.Rows[e.RowIndex].Cells[2] as DataGridViewTextBoxCell;
                        if (cell3 == null) return;

                        cell3.Value = "";

                        ZParameter parameter = parameters.Find(x => x.Title == currentValue);
                        if (parameter == null) return;

                        cell2.Items.AddRange(GetConditions(parameter.ValueType));
                        cell2.ReadOnly = false;
                        cell2.Value = cell2.Items[0];
                    }
                    break;
                case 1:
                    {
                        DataGridViewComboBoxCell cell1 = dataGridViewFilter.Rows[e.RowIndex].Cells[0] as DataGridViewComboBoxCell;
                        if (cell1 == null) return;

                        ZParameter parameter = parameters.Find(x => x.Title == cell1.Value.ToString());
                        if (parameter == null) return;

                        DataGridViewTextBoxCell cell3 = dataGridViewFilter.Rows[e.RowIndex].Cells[2] as DataGridViewTextBoxCell;
                        if (cell3 == null) return;

                        cell3.ReadOnly = false;
                        if (parameter.ValueType == ZValueType.Number || parameter.ValueType == ZValueType.Percent || parameter.ValueType == ZValueType.Table)
                        {
                            //newCell.Style.Format = ;
                        }                        
                    }
                    break;
                default:
                    break;
            }
        }

        private void dataGridViewSort_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.ColumnIndex < 0) return;

            DataGridViewCell currentCell = dataGridViewSort.Rows[e.RowIndex].Cells[e.ColumnIndex];
            string currentValue = currentCell.Value.ToString();

            switch (e.ColumnIndex)
            {
                case 0:
                    {
                        DataGridViewComboBoxCell cell2 = dataGridViewSort.Rows[e.RowIndex].Cells[1] as DataGridViewComboBoxCell;
                        if (cell2 == null) return;

                        cell2.Items.Clear();

                        ZParameter parameter = parameters.Find(x => x.Title == currentValue);
                        if (parameter == null) return;

                        cell2.Items.AddRange(new string[] { "Ascending", "Descending" });
                        cell2.ReadOnly = false;
                        cell2.Value = cell2.Items[0];
                    }
                    break;
                default:
                    break;
            }
        }

        private void dataGridView_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            e.Control.BackColor = Color.White;
            e.CellStyle.BackColor = Color.White;
            
            DataGridViewComboBoxEditingControl cb = e.Control as DataGridViewComboBoxEditingControl;
            if (cb == null) return;

            cb.ApplyCellStyleToEditingControl(e.CellStyle);
        }

        private void buttonSave_Click(object sender, EventArgs e)
        {
            if (SaveFilterAndSort()) 
            {
                DialogResult = System.Windows.Forms.DialogResult.OK;
                this.Close();
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.Close();
        }

        private void textBoxFilterName_TextChanged(object sender, EventArgs e)
        {
            buttonSave.Enabled = !string.IsNullOrWhiteSpace(textBoxFilterName.Text);
        }
    }
}
