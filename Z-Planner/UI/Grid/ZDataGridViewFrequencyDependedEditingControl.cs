using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Globalization;
using System.Windows.Forms.Design;

namespace ZZero.ZPlanner.UI.Grid
{
    public partial class ZDataGridViewFrequencyDependedEditingControl : UserControl
    {
        string frequencyFormat;
        string valueFormat;

        public ZDataGridViewFrequencyDependedEditingControl()
        {
            InitializeComponent();
            DataGridViewRow singleRow = new DataGridViewRow();
            DataGridViewCell singleCell = new DataGridViewTextBoxCell();
            singleRow.Cells.Add(singleCell);
            singleRow.HeaderCell.Value = "Value";
            singleDataGridView.RowHeadersWidth = (int)(singleDataGridView.Width * 0.5);
            singleDataGridView.Rows.Add(singleRow);
            singleDataGridView.CellValidating += dataGridView_CellValidating;
            tableDataGridView.CellValidating += dataGridView_CellValidating;
        }

        void dataGridView_CellValidating(object sender, DataGridViewCellValidatingEventArgs e)
        {
            string stringValue = e.FormattedValue as string;
            if (!string.IsNullOrWhiteSpace(stringValue))
            {
                double doubleValue;

                DataGridView grid = sender as DataGridView;

                if (!double.TryParse(stringValue.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out doubleValue))
                {
                    e.Cancel = true;
                    ZPlannerManager.StatusMenu.SetStatus("Please enter numeric value.");
                    if (grid.EditingControl != null) grid.EditingControl.Text = (sender as DataGridView)[e.ColumnIndex, e.RowIndex].Value as string ?? string.Empty;
                }
                else 
                {
                    string format = (grid.Columns[e.ColumnIndex] == tableFrequencyColumn) ? frequencyFormat : valueFormat;
                    string formatedString = doubleValue.ToString(format, CultureInfo.InvariantCulture);
                    if (grid.EditingControl != null && grid.EditingControl.Text != formatedString) grid.EditingControl.Text = formatedString;
                }
            }
        }

        public void Init(string title, string value, string valueFormat)
        {
            double singleValue = double.NaN;
            Dictionary<double, double> tableValues = new Dictionary<double, double>();
            this.frequencyFormat = "N" + Settings.Options.TheOptions.frequencyDigits;
            this.valueFormat = valueFormat;

            Value = value;
            if (string.IsNullOrWhiteSpace(value) || !value.Contains(":") && !value.Contains(";") && double.TryParse(value.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out singleValue))
            {
                singleRadioButton.Checked = true;
            }
            else
            {
                tableRadioButton.Checked = true;

                Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(value))
                {
                    if (!frequencyValueMatch.Success) continue;
                    string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                    string valueString = frequencyValueMatch.Groups["value"].Value;
                    double frequencyDouble;
                    double valueDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble))
                        tableValues.Add(frequencyDouble, valueDouble);
                }
            }

            tableDataGridView.Rows.Clear();
            singleDataGridView.Rows[0].HeaderCell.Value = title;
            singleDataGridView.Rows[0].Cells[0].Value = (double.IsNaN(singleValue)) ? string.Empty : singleValue.ToString(valueFormat, CultureInfo.InvariantCulture);

            tableValueColumn.HeaderText = title;
            foreach(double key in tableValues.Keys)
            {
                DataGridViewRow tableRow = new DataGridViewRow();
                DataGridViewCell tableFrequencyCell = new DataGridViewTextBoxCell();
                DataGridViewCell tableValueCell = new DataGridViewTextBoxCell();
                tableFrequencyCell.Value = (double.IsNaN(key)) ? string.Empty : key.ToString(frequencyFormat, CultureInfo.InvariantCulture);
                tableValueCell.Value = (double.IsNaN(tableValues[key])) ? string.Empty : tableValues[key].ToString(valueFormat, CultureInfo.InvariantCulture);
                tableRow.Cells.Add(tableFrequencyCell);
                tableRow.Cells.Add(tableValueCell);
                tableDataGridView.Rows.Add(tableRow);
            }
        }

        private void UpdateValue()
        {
            if (singleRadioButton.Checked)
            {
                string stringValue = singleDataGridView[0, 0].Value as string ?? string.Empty;
                double doubleValue;
                if (double.TryParse(stringValue.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out doubleValue))
                {
                    _value = doubleValue.ToString(valueFormat, CultureInfo.InvariantCulture);
                    return;
                }
            }
            if (tableRadioButton.Checked)
            {
                Dictionary<double, double> tableValues = new Dictionary<double, double>();
                foreach(DataGridViewRow row in tableDataGridView.Rows)
                {
                    string frequencyString = row.Cells[0].Value as string ?? string.Empty;
                    string valueString = row.Cells[1].Value as string ?? string.Empty;
                    double frequencyDouble;
                    double valueDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) && !double.IsNaN(frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble) && !double.IsNaN(valueDouble))
                        tableValues.Add(frequencyDouble, valueDouble);
                }

                if (tableValues.Count > 0)
                {
                    string value = string.Empty;
                    foreach (double key in tableValues.Keys.OrderBy(d=>d))
                    {
                        value += key.ToString(frequencyFormat, CultureInfo.InvariantCulture) + ":" + tableValues[key].ToString(valueFormat, CultureInfo.InvariantCulture) + ";";
                    }

                    _value = value;
                    return;
                }
            }
            _value = string.Empty;
        }

        private string _value;
        public string Value 
        {
            get
            {
                UpdateValue();
                return _value;
            }
            set
            {
                _value = value;
            }
        }

        private void singleRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            singleDataGridView.Enabled = singleRadioButton.Checked;
            if (singleRadioButton.Checked)
            {
                tableRadioButton.Checked = false;
                tableDataGridView.ClearSelection();
            }
        }

        private void tableRadioButton_CheckedChanged(object sender, EventArgs e)
        {
            tableDataGridView.Enabled = tableRadioButton.Checked;
            if (tableRadioButton.Checked)
            {
                singleRadioButton.Checked = false;
                singleDataGridView.ClearSelection();
            }
        }

        private void okButton_Click(object sender, EventArgs e)
        {
            ToolStripDropDown tsdd = Parent as ToolStripDropDown;
            if (tsdd != null) tsdd.Close();
            IWindowsFormsEditorService wfes = Tag as IWindowsFormsEditorService;
            if (wfes != null) wfes.CloseDropDown();
        } 
    }
}
