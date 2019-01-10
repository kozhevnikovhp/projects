using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class DmlSearchDialog : Form
    {
        private string currentQuery;
        private int currentRow, currentColumn;

        public DmlSearchDialog()
        {
            InitializeComponent();
            this.Height = 127;
        }

        private void bSearchClose_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.OK;
        }

        private void bSearchAll_Click(object sender, EventArgs e)
        {
            if (this.cbSearch.Text == string.Empty) return;

            this.Height = 285;
            if (currentQuery != this.cbSearch.Text)
            {
                currentQuery = this.cbSearch.Text;
            }

            this.cbSearch.Items.Remove(currentQuery);
            this.cbSearch.Items.Insert(0, currentQuery);
            this.cbSearch.Text = currentQuery;
            FindAll();
        }

        private void bSearchNext_Click(object sender, EventArgs e)
        {
            if (this.cbSearch.Text == string.Empty) return;

            this.Height = 127;
            if (currentQuery != this.cbSearch.Text)
            {
                currentQuery = this.cbSearch.Text;
                currentRow = 0;
                currentColumn = 0;
            }

            this.cbSearch.Items.Remove(currentQuery);
            this.cbSearch.Items.Insert(0, currentQuery);
            this.cbSearch.Text = currentQuery;

            FindNext(currentRow, currentColumn + 1);
        }

        private void dgvSearch_CellClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.RowIndex >= dgvSearch.Rows.Count) return;

            DataGridViewCell cell = dgvSearch.Rows[e.RowIndex].Tag as DataGridViewCell;
            if (cell == null) return;

            cell.DataGridView.ClearSelection();

            if (cell.Visible)
            {
                cell.Selected = true;
                if (!cell.Displayed) cell.DataGridView.FirstDisplayedCell = cell;
            }
            else
            {
                if (!cell.OwningRow.Visible && !cell.OwningColumn.Visible)
                    MessageBox.Show(string.Format(ZStringConstants.FoundRowAndColumnAreHiddenMessage, cell.OwningColumn.HeaderText));
                else if (!cell.OwningRow.Visible)
                    MessageBox.Show(ZStringConstants.FoundRowIsHiddenMessage);
                else
                    MessageBox.Show(string.Format(ZStringConstants.FoundColumnIsHiddenMessage, cell.OwningColumn.HeaderText));
            }
        }

        private void FindNext(int startRow, int startColumn)
        {
            DataGridViewCell cell = ZPlannerManager.DMLPanel.FindNext(currentQuery, startRow, startColumn, cbDisplayedOnly.Checked);

            if (cell == null)
            {
                MessageBox.Show(ZStringConstants.NoResultsFoundMessage);
                return;
            }

            if (currentRow > cell.RowIndex || (currentRow == cell.RowIndex && currentColumn >= cell.ColumnIndex)) MessageBox.Show(ZStringConstants.EndOfLibraryMessage);
            currentRow = cell.RowIndex;
            currentColumn = cell.ColumnIndex;

            if (cell.Visible)
            {
                cell.Selected = true;
                if (!cell.Displayed) cell.DataGridView.FirstDisplayedCell = cell;
            }
            else
            {
                if (!cell.OwningRow.Visible && !cell.OwningColumn.Visible)
                    MessageBox.Show(string.Format(ZStringConstants.FoundRowAndColumnAreHiddenMessage, cell.OwningColumn.HeaderText));
                else if (!cell.OwningRow.Visible)
                    MessageBox.Show(ZStringConstants.FoundRowIsHiddenMessage);
                else
                    MessageBox.Show(string.Format(ZStringConstants.FoundColumnIsHiddenMessage, cell.OwningColumn.HeaderText));
            }
        }

        private void FindAll()
        {
            dgvSearch.Rows.Clear();

            List<DataGridViewCell> cells = ZPlannerManager.DMLPanel.FindAll(currentQuery, cbDisplayedOnly.Checked);

            if (cells == null || cells.Count == 0)
            {
                MessageBox.Show(ZStringConstants.NoResultsFoundMessage);
                return;
            }

            foreach(DataGridViewCell cell in cells)
            {
                DataGridViewRow resultsRow = new DataGridViewRow();

                DataGridViewCell valueCell = new DataGridViewTextBoxCell();
                valueCell.Value = cell.Value;
                resultsRow.Cells.Add(valueCell);

                DataGridViewCell rowCell = new DataGridViewTextBoxCell();
                rowCell.Value = cell.OwningRow.HeaderCell.Value;
                resultsRow.Cells.Add(rowCell);

                DataGridViewCell columnCell = new DataGridViewTextBoxCell();
                columnCell.Value = cell.OwningColumn.HeaderCell.Value;
                resultsRow.Cells.Add(columnCell);

                resultsRow.Tag = cell;

                dgvSearch.Rows.Add(resultsRow);
                dgvSearch.ClearSelection();
            }
        }
    }
}
