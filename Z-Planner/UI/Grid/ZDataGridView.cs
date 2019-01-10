using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridView : DataGridView
    {
        // This can be used in V2
        //ZDataGridViewFrequencyDependedEditingControl frequencyDependedEditingControl;
        //ZDataGridViewDropDown frequencyDependedDropDown;

        //
        // Summary:
        //     Occurs when the System.Windows.Forms.DataGridView control completes a sorting
        //     operation.
        public event EventHandler Filtered;

        public ZDataGridView()
        {
            //CurrentRowIndex = -2;

            //this.DefaultCellBehavior = ZDataGridViewColumnHeaderCellBehavior.SortingFiltering;
            this.DefaultCellBehavior = ZDataGridViewColumnHeaderCellBehavior.DisabledHidden;
            this.DefaultDateTimeGrouping = ZDataGridViewFilterMenuDateTimeGrouping.Second;
            this.ColumnHeaderNamesVisible = true;
            AllowDrawCenterLine = false;

            this.DefaultCellStyle.Font = new Font("Microsoft Sans Serif", 8.25F);

            ZDataGridViewRow rowTemplate = new ZDataGridViewRow();
            this.RowTemplate = rowTemplate;

            // This can be used in V2
            //frequencyDependedEditingControl = new ZDataGridViewFrequencyDependedEditingControl();
            //frequencyDependedDropDown = new ZDataGridViewDropDown(frequencyDependedEditingControl);
            //frequencyDependedDropDown.Closing += frequencyDependedDropDown_Closing;
            //this.CellBeginEdit += ZDataGridView_CellBeginEdit;

            this.KeyDown += ZDataGridView_KeyDown;
            this.CurrentCellDirtyStateChanged += ZDataGridView_CurrentCellDirtyStateChanged;
        }

        protected override void OnRowStateChanged(int rowIndex, DataGridViewRowStateChangedEventArgs e)
        {
            if (!isSuspended || e.StateChanged != DataGridViewElementStates.Visible) base.OnRowStateChanged(rowIndex, e);
        }

        private bool isSuspended = false;

        public void SuspendState()
        {
            isSuspended = true;
        }

        public void ResumeState()
        {
            isSuspended = false;
            bool visible = this.Rows[0].Visible;
            this.Rows[0].Visible = !visible;
            this.Rows[0].Visible = visible;
            Visible = false;
            Visible = true;
        }

        void ZDataGridView_CurrentCellDirtyStateChanged(object sender, EventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            if (CurrentCell is ZDataGridViewComboBoxCell)
            {
                grid.CommitEdit(DataGridViewDataErrorContexts.Commit);
                grid.EndEdit();
            }
        }

        void ZDataGridView_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Delete && !e.Alt && !e.Control && !e.Shift)
            {
                foreach (DataGridViewCell cell in this.SelectedCells)
                {
                    if (cell.ColumnIndex >= 0 && cell.RowIndex >= 0 && !cell.ReadOnly && (cell.OwningColumn.Name != ZStringConstants.ParameterIDLayerType || cell.OwningColumn.Name != ZStringConstants.DMLParameterIDType))
                    {
                        cell.Value = null;
                        //this.CurrentCell = null;
                        //this.CurrentCell = cell;
                    }
                }
            }

            DataGridViewCell currentCell = this.CurrentCell;
            if (currentCell != null && currentCell.ColumnIndex >= 0 && currentCell.RowIndex >= 0 && currentCell.ReadOnly && !currentCell.OwningColumn.ReadOnly)
            {
                ZLayer layer = currentCell.OwningRow.Tag as ZLayer;
                if (layer != null && layer.IsMaterialAssigned && ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.TryToEditMaterialAssignedRow();
            }
        }

        void ZDataGridView_CellBeginEdit(object sender, DataGridViewCellCancelEventArgs e)
        {
            // This can be used in V2
            /*ZDataGridView grid = sender as ZDataGridView;
                if (grid == null || e.ColumnIndex < 0 || e.RowIndex < 0) return;
                ZDataGridViewFrequencyDependedCell cell = grid.Rows[e.RowIndex].Cells[e.ColumnIndex] as ZDataGridViewFrequencyDependedCell;
                if (cell != null)
                {
                    ZParameter parameter = grid.Columns[e.ColumnIndex].Tag as ZParameter;
                    string valueFormat = (parameter != null) ? ZPlannerManager.GetFormatByParameter(parameter) : "N2";

                    frequencyDependedEditingControl.Init(parameter.Title, (cell.Value != null) ? cell.Value.ToString() : string.Empty, valueFormat);
                    frequencyDependedDropDown.Show(grid, grid.GetCellDisplayRectangle(e.ColumnIndex, e.RowIndex, false).Location);
                }*/
        }

        void frequencyDependedDropDown_Closing(object sender, ToolStripDropDownClosingEventArgs e)
        {
            // This can be used in V2
            //this.CurrentCell.Value = frequencyDependedEditingControl.Value;
        }

        private List<string> sortOrder = new List<string>();
        private List<string> filterOrder = new List<string>();
        private List<string> readyToShowFilters = new List<string>();

        private string sortString = null;
        private string filterString = null;

        public ZDataGridViewFilterMenuDateTimeGrouping DefaultDateTimeGrouping { get; set; }

        public ZDataGridViewColumnHeaderCellBehavior DefaultCellBehavior { get; set; }

        public bool ColumnHeaderNamesVisible { get; set; }

        public bool AllowDrawCenterLine { get; set; }

        public event EventHandler SortStringChanged = delegate { };

        public event EventHandler FilterStringChanged = delegate { };

        private IEnumerable<ZDataGridViewColumnHeaderCell> filterCells
        {
            get
            {
                return from DataGridViewColumn c in this.Columns
                       where c.HeaderCell != null && c.HeaderCell is ZDataGridViewColumnHeaderCell
                       select (c.HeaderCell as ZDataGridViewColumnHeaderCell);
            }
        }

        public string SortString
        {
            get
            {
                return String.IsNullOrWhiteSpace(this.sortString) ? "" : this.sortString;
            }
            set
            {
                if (String.IsNullOrWhiteSpace(value))
                    value = null;

                //if (value != this.sortString)
                {
                    this.sortString = value;
                    this.SortStringChanged(this, new EventArgs());
                }
            }
        }

        public void OnSortStringChanged()
        {
            this.SortStringChanged(this, new EventArgs());
        }

        public string FilterString
        {
            get
            {
                return String.IsNullOrWhiteSpace(this.filterString) ? "" : this.filterString;
            }
            set
            {
                if (String.IsNullOrWhiteSpace(value))
                    value = null;

                //if (value != this.filterString)
                {
                    this.filterString = value;
                    this.FilterStringChanged(this, new EventArgs());
                    OnFiltered();
                }
            }
        }

        public void OnFilterStringChanged()
        {
            this.FilterStringChanged(this, new EventArgs());
        }

        //public int CurrentRowIndex {get; set;}

        protected override void OnColumnAdded(DataGridViewColumnEventArgs e)
        {
            e.Column.SortMode = DataGridViewColumnSortMode.Programmatic;
            ZDataGridViewColumnHeaderCell cell = e.Column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (cell == null) cell = new ZDataGridViewColumnHeaderCell(e.Column.HeaderCell, this.DefaultCellBehavior);
            else cell.CellBehavior = this.DefaultCellBehavior;
            if (cell.FilterMenu != null)
            {
                //cell.DateTimeGrouping = this.DefaultDateTimeGrouping;
                cell.SortChanged += new ZDataGridViewFilterEventHandler(eSortChanged);
                cell.FilterChanged += new ZDataGridViewFilterEventHandler(eFilterChanged);
                cell.FilterPopup += new ZDataGridViewFilterEventHandler(eFilterPopup);
            }

            e.Column.MinimumWidth = cell.MinimumSize.Width;
            if (this.ColumnHeadersHeight < cell.MinimumSize.Height)
                this.ColumnHeadersHeight = cell.MinimumSize.Height;
            e.Column.HeaderCell = cell;

            base.OnColumnAdded(e);
        }

        protected override void OnColumnRemoved(DataGridViewColumnEventArgs e)
        {
            this.readyToShowFilters.Remove(e.Column.Name);
            this.filterOrder.Remove(e.Column.Name);
            this.sortOrder.Remove(e.Column.Name);

            ZDataGridViewColumnHeaderCell cell = e.Column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (cell != null)
            {
                cell.SortChanged -= eSortChanged;
                cell.FilterChanged -= eFilterChanged;
                cell.FilterPopup -= eFilterPopup;
                cell.Dispose();
            }
            base.OnColumnRemoved(e);
        }

        protected override void OnRowsAdded(DataGridViewRowsAddedEventArgs e)
        {
            this.readyToShowFilters.Clear();
            base.OnRowsAdded(e);
        }

        protected override void OnRowsRemoved(DataGridViewRowsRemovedEventArgs e)
        {
            this.readyToShowFilters.Clear();
            base.OnRowsRemoved(e);
        }

        protected override void OnCellValueChanged(DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex < 0) return;
            this.readyToShowFilters.Remove(this.Columns[e.ColumnIndex].Name);
            base.OnCellValueChanged(e);
        }

        internal void PrepareFilterWithoutShowing(ZDataGridViewFilterMenu filterMenu, string columnName)
        {
            if (!this.readyToShowFilters.Contains(columnName))
            {
                this.readyToShowFilters.Add(columnName);

                if (this.filterOrder.Count() == 0 || this.filterOrder.Last() != columnName)
                {
                    Cursor currentCursor = Cursor.Current;
                    Cursor.Current = Cursors.WaitCursor;
                    if (columnName == ZStringConstants.DMLParameterIDMaterial && this.filterOrder.Contains(ZStringConstants.DMLParameterIDManufacturer))
                    {
                        ZPlannerManager.Dml.ExtractMaterialManufacturerDictionary();
                        filterMenu.PrepareNodes(ZPlannerManager.Dml.GetMaterialsByManufacturers(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, ZStringConstants.DMLParameterIDManufacturer, true)), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true));
                        //filterMenu.PrepareNodes(ZDataGridViewFilterMenu.GetMaterialOfManufacturerValuesForFilter(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, ZStringConstants.DMLParameterIDManufacturer, true), this, columnName, true), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true));
                    }
                    else
                        filterMenu.PrepareNodes(ZDataGridViewFilterMenu.GetValuesForFilter(this, columnName, true), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true));
                    Cursor.Current = currentCursor; 
                }
            }
        }

        private void eFilterPopup(object sender, ZDataGridViewFilterEventArgs e)
        {
            if (this.Columns.Contains(e.Column))
            {
                System.Drawing.Rectangle rect = this.GetCellDisplayRectangle(e.Column.Index, -1, true);

                if (this.readyToShowFilters.Contains(e.Column.Name))
                    e.FilterMenu.Show(this, rect.Left, rect.Bottom, false);
                else
                {
                    this.readyToShowFilters.Add(e.Column.Name);

                    Cursor currentCursor = Cursor.Current;
                    Cursor.Current = Cursors.WaitCursor; 
                    
                    /*if (this.filterOrder.Count() > 0 && this.filterOrder.Last() == e.Column.Name)
                        e.FilterMenu.Show(this, rect.Left, rect.Bottom, true);
                    else*/ if (e.Column.Name == ZStringConstants.DMLParameterIDMaterial && this.filterOrder.Contains(ZStringConstants.DMLParameterIDManufacturer))
                    {
                        ZPlannerManager.Dml.ExtractMaterialManufacturerDictionary();
                        e.FilterMenu.Show(this, rect.Left, rect.Bottom, ZPlannerManager.Dml.GetMaterialsByManufacturers(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, ZStringConstants.DMLParameterIDManufacturer, true)), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true));
                        //e.FilterMenu.Show(this, rect.Left, rect.Bottom, ZDataGridViewFilterMenu.GetMaterialOfManufacturerValuesForFilter(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, ZStringConstants.DMLParameterIDManufacturer, true), this, e.Column.Name, true), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true));
                    }
                    else
                        e.FilterMenu.Show(this, rect.Left, rect.Bottom, ZDataGridViewFilterMenu.GetValuesForFilter(this, e.Column.Name, true), ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true));

                    Cursor.Current = currentCursor; 
                }
            }
        }

        private void eFilterChanged(object sender, ZDataGridViewFilterEventArgs e)
        {
            if (this.Columns.Contains(e.Column))
            {
                this.filterOrder.Remove(e.Column.Name);
                if (e.FilterMenu.ActiveFilterType != ZDataGridViewFilterType.None)
                    this.filterOrder.Add(e.Column.Name);

                this.FilterString = this.CreateFilterString();

                /*switch(e.Column.Name)
                {
                    case ZStringConstants.DMLParameterIDType:
                        ZPlannerManager.MainMenu.SetTypeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDCategory:
                        ZPlannerManager.MainMenu.SetCategoryFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDQualified:
                        ZPlannerManager.MainMenu.SetQualifiedFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDHalogenFree:
                        ZPlannerManager.MainMenu.SetHalogenFreeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDNormalizedConstruction:
                        ZPlannerManager.MainMenu.Set2PlyMaterialsFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDConstruction:
                        ZPlannerManager.MainMenu.SetGlassStyleFilterButtons(e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDPrimaryIPCSlashSheet:
                        ZPlannerManager.MainMenu.SetSlashSheetFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), e.FilterMenu.FilterString);
                        break;
                    case ZStringConstants.DMLParameterIDManufacturer:
                        this.readyToShowFilters.Remove(ZStringConstants.DMLParameterIDMaterial);
                        this.filterOrder.Remove(ZStringConstants.DMLParameterIDMaterial);
                        break;
                }*/

                SetFilterButtons(e.Column.Name, e.FilterMenu.FilterString);
            }
        }

        private void eSortChanged(object sender, ZDataGridViewFilterEventArgs e)
        {
            if (this.Columns.Contains(e.Column))
            {
                this.sortOrder.Remove(e.Column.Name);

                if (e.FilterMenu.ActiveSortType != ZDataGridViewSortType.None)
                    this.sortOrder.Add(e.Column.Name);

                this.SortString = this.CreateSortString();
            }
        }

        public void SetFilterButtons(string columnName, string filterString)
        {
            switch (columnName)
            {
                case ZStringConstants.DMLParameterIDType:
                    ZPlannerManager.MainMenu.SetTypeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDCategory:
                    ZPlannerManager.MainMenu.SetCategoryFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDQualified:
                    ZPlannerManager.MainMenu.SetQualifiedFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDHalogenFree:
                    ZPlannerManager.MainMenu.SetHalogenFreeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDNormalizedConstruction:
                    ZPlannerManager.MainMenu.Set2PlyMaterialsFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDConstruction:
                    ZPlannerManager.MainMenu.SetGlassStyleFilterButtons(filterString);
                    break;
                case ZStringConstants.DMLParameterIDPrimaryIPCSlashSheet:
                    ZPlannerManager.MainMenu.SetSlashSheetFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, columnName, true), filterString);
                    break;
                case ZStringConstants.DMLParameterIDManufacturer:
                    this.readyToShowFilters.Remove(ZStringConstants.DMLParameterIDMaterial);
                    this.filterOrder.Remove(ZStringConstants.DMLParameterIDMaterial);
                    break;
            }

            /*ZPlannerManager.MainMenu.SetTypeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.SetCategoryFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.SetQualifiedFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.SetHalogenFreeFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.Set2PlyMaterialsFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.SetGlassStyleFilterButtons(e.FilterMenu.FilterString, string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
            ZPlannerManager.MainMenu.SetSlashSheetFilterButtons(ZDataGridViewFilterMenu.GetVisibleValuesForFilter(this, e.Column.Name, true), string.IsNullOrWhiteSpace(e.FilterMenu.FilterString));
                
            if (e.Column.Name == ZStringConstants.DMLParameterIDManufacturer)
            {
                this.readyToShowFilters.Remove(ZStringConstants.DMLParameterIDMaterial);
            }*/
        }

        private String CreateFilterString()
        {
            StringBuilder sb = new StringBuilder("");

            foreach (string name in this.filterOrder)
            {
                DataGridViewColumn column = this.Columns[name];

                if (column != null)
                {
                    ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null && cell.ActiveFilterType != ZDataGridViewFilterType.None)
                    {
                        sb.AppendFormat("(" + cell.FilterString + ") AND ", column.Name);
                    }
                }
            }
            if (sb.Length > 4)
                sb.Length -= 4;

            return sb.ToString();
        }

        private String CreateSortString()
        {
            StringBuilder sb = new StringBuilder("");

            foreach (string name in this.sortOrder)
            {
                DataGridViewColumn column = this.Columns[name];

                if (column != null)
                {
                    ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null && cell.ActiveSortType != ZDataGridViewSortType.None)
                    {
                        sb.AppendFormat(cell.SortString + ", ", column.Name);
                    }
                }
            }

            if (sb.Length > 2)
                sb.Length -= 2;

            return sb.ToString();
        }

        public void SetFilterBehavior(ZDataGridViewColumnHeaderCellBehavior behavior, DataGridViewColumn column = null)
        {
            if (column == null)
            {
                foreach (DataGridViewColumn c in this.Columns)
                {
                    ZDataGridViewColumnHeaderCell cell = c.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null)
                        cell.CellBehavior = behavior;
                }
            }
            else if (this.Columns.Contains(column))
            {
                ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (cell != null)
                    cell.CellBehavior = behavior;
            }
        }

        public void SetFilterDateTimeGrouping(ZDataGridViewFilterMenuDateTimeGrouping grouping, DataGridViewColumn column = null)
        {
            if (column == null)
            {
                foreach (DataGridViewColumn c in this.Columns)
                {
                    ZDataGridViewColumnHeaderCell cell = c.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null)
                        cell.DateTimeGrouping = grouping;
                }
            }
            else if (this.Columns.Contains(column))
            {
                ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (cell != null)
                    cell.DateTimeGrouping = grouping;
            }
        }

        public void LoadFilter(string filter, string sorting = null)
        {
            //this.filterOrder.Clear();
            //this.sortOrder.Clear();
            //this.readyToShowFilters.Clear();

            if (filter != null)
                this.FilterString = filter;
            if (sorting != null)
                this.SortString = sorting;
        }

        public void ClearSort(bool fireEvent = false, DataGridViewColumn column = null)
        {
            if (column == null)
            {
                foreach (ZDataGridViewColumnHeaderCell c in this.filterCells)
                    c.ClearSorting();

                this.sortOrder.Clear();
            }
            else if (this.Columns.Contains(column))
            {
                ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;

                if (cell != null)
                {
                    cell.ClearSorting();
                    this.sortOrder.Remove(column.Name);
                }
            }

            if (fireEvent)
                this.SortString = null;
            else
                this.sortString = null;
        }

        public void ClearFilter(bool fireEvent = false, DataGridViewColumn column = null)
        {
            if (column == null)
            {
                foreach (ZDataGridViewColumnHeaderCell c in this.filterCells)
                    c.ClearFilter(true);

                this.filterOrder.Clear();
                this.readyToShowFilters.Clear();
            }
            else if (this.Columns.Contains(column))
            {
                ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;

                if (cell != null)
                {
                    cell.ClearFilter(true);
                    this.filterOrder.Remove(column.Name);
                    this.readyToShowFilters.Remove(column.Name);
                    if (column.Name == ZStringConstants.DMLParameterIDManufacturer) this.readyToShowFilters.Remove(ZStringConstants.DMLParameterIDMaterial);
                }
            }

            if (fireEvent)
                this.FilterString = null;
            else
                this.filterString = null;
        }

        public void OnFiltered()
        {
            if (Filtered != null)
            {
                Filtered(this, new EventArgs());
            }
        }

        public void OnSorted()
        {
            base.OnSorted(new EventArgs());
        }

        public DataGridViewCell FindCell(string valueToFind, string columnName = null, int startRowIndex = 0, int startColumnIndex = 0, bool isWholeWordSearch = true, bool isCaseSensitive = false)
        {
            if (valueToFind != null && this.RowCount > 0 && this.ColumnCount > 0 && (columnName == null || (this.Columns.Contains(columnName) && this.Columns[columnName].Visible)))
            {
                startRowIndex = Math.Max(0, startRowIndex);

                if (!isCaseSensitive)
                    valueToFind = valueToFind.ToLower();

                if (columnName != null)
                {
                    int c = this.Columns[columnName].Index;
                    if (startColumnIndex > c)
                        startRowIndex++;
                    for (int r = startRowIndex; r < this.RowCount; r++)
                    {
                        string value = this.Rows[r].Cells[c].FormattedValue.ToString();
                        if (!isCaseSensitive)
                            value = value.ToLower();

                        if ((!isWholeWordSearch && value.Contains(valueToFind)) || value.Equals(valueToFind))
                            return this.Rows[r].Cells[c];
                    }
                }
                else
                {
                    startColumnIndex = Math.Max(0, startColumnIndex);

                    for (int r = startRowIndex; r < this.RowCount; r++)
                    {
                        for (int c = startColumnIndex; c < this.ColumnCount; c++)
                        {
                            string value = this.Rows[r].Cells[c].FormattedValue.ToString();
                            if (!isCaseSensitive)
                                value = value.ToLower();

                            if ((!isWholeWordSearch && value.Contains(valueToFind)) || value.Equals(valueToFind))
                                return this.Rows[r].Cells[c];
                        }

                        startColumnIndex = 0;
                    }
                }
            }

            return null;
        }

        /*internal void ResizeGrid()
        {
            int width = 0, height = 0;
            if (this.ColumnHeadersVisible) height += this.ColumnHeadersHeight;
            height += this.Rows.GetRowsHeight(DataGridViewElementStates.Visible);

            if (this.RowHeadersVisible) width += this.RowHeadersWidth;
            else width += 1;
            width += this.Columns.GetColumnsWidth(DataGridViewElementStates.Visible);

            int horizontalScrollOffset = this.HorizontalScrollingOffset;
            this.Height = height;
            this.Width = width;
            this.HorizontalScrollingOffset = horizontalScrollOffset;
        }*/

        internal ZDataGridViewColumn GetColumnByName(string name)
        {
            foreach(ZDataGridViewColumn column in this.Columns)
            {
                if (column.Name == name) return column;
            }

            return null;
        }

        internal List<ZDataGridViewColumn> GetColumnsOrderedByDisplayIndex()
        {
            List<ZDataGridViewColumn> columns = new List<ZDataGridViewColumn>();
            ZDataGridViewColumn column = this.Columns.GetFirstColumn(DataGridViewElementStates.None) as ZDataGridViewColumn;
            while (column != null)
            {
                columns.Add(column);
                column = this.Columns.GetNextColumn(column, DataGridViewElementStates.None, DataGridViewElementStates.None) as ZDataGridViewColumn;
            }

            return columns;
        
        }

        internal Dictionary<string, List<ZDataGridViewColumn>> GetColumnsOrderedByTitle(ZTableType tableType = ZTableType.Undefined)
        {
            List<ZDataGridViewColumn> columns = new List<ZDataGridViewColumn>();
            foreach(ZDataGridViewColumn column in this.Columns)
            {
                if (tableType == ZTableType.Undefined || tableType == column.TableType) columns.Add(column);
            }

            //columns.Sort((x1, x2) => (x1.Tag as ZParameter).Title.CompareTo((x2.Tag as ZParameter).Title));
            Dictionary<string, List<ZDataGridViewColumn>> grouppedColumns = new Dictionary<string, List<ZDataGridViewColumn>>();
            grouppedColumns.Add(string.Empty, new List<ZDataGridViewColumn>());

            foreach (ZDataGridViewColumn column in columns)
            {
                if (column.Tag == null) continue;
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null) continue;
                string group = parameter.ParameterGroup;
                if (!grouppedColumns.ContainsKey(group)) grouppedColumns.Add(group, new List<ZDataGridViewColumn>());
                grouppedColumns[group].Add(column);
            }

            if (grouppedColumns.Keys.Count == 1) grouppedColumns[string.Empty].Sort((x1, x2) => (x1.Tag as ZParameter).Title.CompareTo((x2.Tag as ZParameter).Title));

            return grouppedColumns;
        }

        protected override void OnPaint(PaintEventArgs e)
        {
            base.OnPaint(e);
            DrawCenterLine(e.Graphics);
        }

        private void DrawCenterLine(Graphics g)
        {
            if (AllowDrawCenterLine && ZPlannerManager.IsCenterLineVisible)
            {
                if (this.Rows.Count <= 2) return;

                int centerLineY;
                using (Pen pen = new Pen(ZColorConstants.CenterLineColor, 1.5F))
                {
                    pen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;

                    if (Rows.Count % 2 == 0)
                    {
                        centerLineY = GetRowDisplayRectangle((int)(Rows.Count * 0.5 - 1), false).Bottom - 1;
                        g.DrawLine(pen, new Point(0, centerLineY), new Point(Bounds.Width, centerLineY));
                    }
                    else
                    {
                        centerLineY = GetRowDisplayRectangle((int)(Rows.Count * 0.5 - 0.5 - 1), false).Bottom - 1;
                        g.DrawLine(pen, new Point(0, centerLineY), new Point(Bounds.Width, centerLineY));

                        centerLineY = GetRowDisplayRectangle((int)(Rows.Count * 0.5 + 0.5 - 1), false).Bottom - 1;
                        g.DrawLine(pen, new Point(0, centerLineY), new Point(Bounds.Width, centerLineY));
                    }
                }
            }
        }

        public static bool SingleHorizontalCellBorderAdded(DataGridView dataGridView)
        {
            return !dataGridView.ColumnHeadersVisible &&
                (dataGridView.AdvancedCellBorderStyle.All == DataGridViewAdvancedCellBorderStyle.Single ||
                 dataGridView.CellBorderStyle == DataGridViewCellBorderStyle.SingleHorizontal);
        }

        public static bool SingleHorizontalHeaderBorderAdded(DataGridView dataGridView)
        {
            return dataGridView.ColumnHeadersVisible &&
                (dataGridView.AdvancedColumnHeadersBorderStyle.All == DataGridViewAdvancedCellBorderStyle.Single ||
                 dataGridView.ColumnHeadersBorderStyle == DataGridViewHeaderBorderStyle.Single);
        }

        public static bool SingleVerticalCellBorderAdded(DataGridView dataGridView)
        {
            return !dataGridView.RowHeadersVisible &&
                (dataGridView.AdvancedCellBorderStyle.All == DataGridViewAdvancedCellBorderStyle.Single ||
                 dataGridView.CellBorderStyle == DataGridViewCellBorderStyle.SingleVertical);
        }

        public static bool SingleVerticalHeaderBorderAdded(DataGridView dataGridView)
        {
            return dataGridView.RowHeadersVisible &&
                (dataGridView.AdvancedRowHeadersBorderStyle.All == DataGridViewAdvancedCellBorderStyle.Single ||
                 dataGridView.ColumnHeadersBorderStyle == DataGridViewHeaderBorderStyle.Single);
        }

        public static Rectangle GetSpannedCellClipBounds<TCell>(
            TCell ownerCell,
            Rectangle cellBounds,
            bool singleVerticalBorderAdded,
            bool singleHorizontalBorderAdded)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = ownerCell.DataGridView;
            var clipBounds = cellBounds;
            //Setting X (skip invisible columns).
            for (int columnIndex = ownerCell.ColumnIndex; columnIndex < ownerCell.ColumnIndex + ownerCell.ColumnSpan; columnIndex++)
            {
                DataGridViewColumn column = dataGridView.Columns[columnIndex];
                if (!column.Visible)
                    continue;
                if (column.Frozen
                    || columnIndex > dataGridView.FirstDisplayedScrollingColumnIndex)
                {
                    break;
                }
                if (columnIndex == dataGridView.FirstDisplayedScrollingColumnIndex)
                {
                    clipBounds.Width -= dataGridView.FirstDisplayedScrollingColumnHiddenWidth;
                    if (dataGridView.RightToLeft != RightToLeft.Yes)
                    {
                        clipBounds.X += dataGridView.FirstDisplayedScrollingColumnHiddenWidth;
                    }
                    break;
                }
                clipBounds.Width -= column.Width;
                if (dataGridView.RightToLeft != RightToLeft.Yes)
                {
                    clipBounds.X += column.Width;
                }
            }

            //Setting Y.
            for (int rowIndex = ownerCell.RowIndex; rowIndex < ownerCell.RowIndex + ownerCell.RowSpan; rowIndex++)
            {
                DataGridViewRow row = dataGridView.Rows[rowIndex];
                if (!row.Visible)
                    continue;
                if (row.Frozen || rowIndex >= dataGridView.FirstDisplayedScrollingRowIndex)
                {
                    break;
                }
                clipBounds.Y += row.Height;
                clipBounds.Height -= row.Height;
            }

            // exclude borders.
            if (dataGridView.BorderStyle != BorderStyle.None)
            {
                var clientRectangle = dataGridView.ClientRectangle;
                clientRectangle.Width--;
                clientRectangle.Height--;
                if (dataGridView.RightToLeft == RightToLeft.Yes)
                {
                    clientRectangle.X++;
                    clientRectangle.Y++;
                }
                clipBounds.Intersect(clientRectangle);
            }
            return clipBounds;
        }

        public static Rectangle GetSpannedColumnHeaderCellClipBounds<TCell>(
            TCell ownerCell,
            Rectangle cellBounds,
            bool singleVerticalBorderAdded,
            bool singleHorizontalBorderAdded)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = ownerCell.DataGridView;
            var clipBounds = cellBounds;
            int firstColumnIndex = dataGridView.FirstDisplayedScrollingColumnIndex;
            int firstDisplayedColumnDisplayIndex = firstColumnIndex;
            if (firstColumnIndex >= 0) firstDisplayedColumnDisplayIndex = dataGridView.Columns[firstColumnIndex].DisplayIndex;
            //Setting X (skip invisible columns).
            for (int columnIndex = ownerCell.ColumnIndex; columnIndex < ownerCell.ColumnIndex + ownerCell.ColumnSpan; columnIndex++)
            {
                DataGridViewColumn column = dataGridView.Columns[columnIndex];
                if (!column.Visible)
                    continue;
                if (column.Frozen
                    || column.DisplayIndex > firstDisplayedColumnDisplayIndex)
                {
                    break;
                }

                if (column.DisplayIndex == firstDisplayedColumnDisplayIndex)
                {
                    clipBounds.Width -= dataGridView.FirstDisplayedScrollingColumnHiddenWidth;
                    if (dataGridView.RightToLeft != RightToLeft.Yes)
                    {
                        clipBounds.X += dataGridView.FirstDisplayedScrollingColumnHiddenWidth;
                    }
                    break;
                }

                clipBounds.Width -= column.Width;
                if (dataGridView.RightToLeft != RightToLeft.Yes)
                {
                    clipBounds.X += column.Width;
                }
            }

            //Setting Y.
            if (!dataGridView.ColumnHeadersVisible)
            {
                clipBounds.Height = 0;
            }

            // exclude borders.
            if (dataGridView.BorderStyle != BorderStyle.None)
            {
                var clientRectangle = dataGridView.ClientRectangle;
                clientRectangle.Width--;
                clientRectangle.Height--;
                if (dataGridView.RightToLeft == RightToLeft.Yes)
                {
                    clientRectangle.X++;
                    clientRectangle.Y++;
                }
                clipBounds.Intersect(clientRectangle);
            }
            return clipBounds;
        }

        public static Rectangle GetSpannedCellBoundsFromChildCellBounds<TCell>(
            TCell childCell,
            Rectangle childCellBounds,
            bool singleVerticalBorderAdded,
            bool singleHorizontalBorderAdded)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = childCell.DataGridView;
            var ownerCell = childCell.OwnerCell as TCell ?? childCell;
            var spannedCellBounds = childCellBounds;
            //
            int firstVisibleColumnIndex = GetFirstVisibleColumnIndex(dataGridView, ownerCell.ColumnIndex,
                                                                     ownerCell.ColumnSpan);
            if (dataGridView.Columns[firstVisibleColumnIndex].Frozen)
            {
                spannedCellBounds.X = dataGridView.GetColumnDisplayRectangle(firstVisibleColumnIndex, false).X;
            }
            else
            {
                int dx = 0;
                for (int i = firstVisibleColumnIndex; i < childCell.ColumnIndex; i++)
                {
                    DataGridViewColumn column = dataGridView.Columns[i];
                    if (!column.Visible) continue;
                    dx += column.Width;
                }
                spannedCellBounds.X = dataGridView.RightToLeft == RightToLeft.Yes
                                          ? spannedCellBounds.X + dx
                                          : spannedCellBounds.X - dx;
            }
            //
            var firstVisibleRowIndex = GetFirstVisibleRowIndex(dataGridView, ownerCell.RowIndex, ownerCell.RowSpan);
            if (dataGridView.Rows[firstVisibleRowIndex].Frozen)
            {
                spannedCellBounds.Y = dataGridView.GetRowDisplayRectangle(firstVisibleRowIndex, false).Y;
            }
            else
            {
                int dy = 0;
                for (int i = firstVisibleRowIndex; i < childCell.RowIndex; i++)
                {
                    DataGridViewRow row = dataGridView.Rows[i];
                    if (!row.Visible) continue;
                    dy += row.Height;
                }
                spannedCellBounds.Y -= dy;
            }
            //
            int spannedCellWidth = 0;
            for (int i = ownerCell.ColumnIndex; i < ownerCell.ColumnIndex + ownerCell.ColumnSpan; i++)
            {
                DataGridViewColumn column = dataGridView.Columns[i];
                if (!column.Visible) continue;
                spannedCellWidth += column.Width;
            }

            if (dataGridView.RightToLeft == RightToLeft.Yes)
            {
                spannedCellBounds.X = spannedCellBounds.Right - spannedCellWidth;
            }
            spannedCellBounds.Width = spannedCellWidth;
            //
            int spannedCellHieght = 0;
            for (int i = ownerCell.RowIndex; i < ownerCell.RowIndex + ownerCell.RowSpan; i++)
            {
                DataGridViewRow row = dataGridView.Rows[i];
                if (!row.Visible) continue;
                spannedCellHieght += row.Height;
            }
            spannedCellBounds.Height = spannedCellHieght;

            if (singleVerticalBorderAdded && InFirstDisplayedColumn(ownerCell))
            {
                spannedCellBounds.Width++;
                if (dataGridView.RightToLeft != RightToLeft.Yes)
                {
                    if (childCell.ColumnIndex != dataGridView.FirstDisplayedScrollingColumnIndex)
                    {
                        spannedCellBounds.X--;
                    }
                }
                else
                {
                    if (childCell.ColumnIndex == dataGridView.FirstDisplayedScrollingColumnIndex)
                    {
                        spannedCellBounds.X--;
                    }
                }
            }
            if (singleHorizontalBorderAdded && InFirstDisplayedRow(ownerCell))
            {
                spannedCellBounds.Height++;
                if (childCell.RowIndex != dataGridView.FirstDisplayedScrollingRowIndex)
                {
                    spannedCellBounds.Y--;
                }
            }
            return spannedCellBounds;
        }

        public static Rectangle GetSpannedColumnHeaderCellBoundsFromChildCellBounds<TCell>(
            TCell childCell,
            Rectangle childCellBounds,
            bool singleVerticalBorderAdded,
            bool singleHorizontalBorderAdded)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = childCell.DataGridView;
            var ownerCell = childCell.OwnerCell as TCell ?? childCell;
            var spannedCellBounds = childCellBounds;
            //
            int firstVisibleColumnIndex = GetFirstVisibleColumnIndex(dataGridView, ownerCell.ColumnIndex,
                                                                     ownerCell.ColumnSpan);
            if (dataGridView.Columns[firstVisibleColumnIndex].Frozen)
            {
                spannedCellBounds.X = dataGridView.GetColumnDisplayRectangle(firstVisibleColumnIndex, false).X;
            }
            else
            {
                spannedCellBounds.X = dataGridView.GetColumnDisplayRectangle(firstVisibleColumnIndex, false).X;
                /*int dx = 0;
                for (int i = firstVisibleColumnIndex; i < childCell.ColumnIndex; i++)
                {
                    DataGridViewColumn column = dataGridView.Columns[i];
                    if (!column.Visible) continue;
                    dx += column.Width;
                }
                spannedCellBounds.X = dataGridView.RightToLeft == RightToLeft.Yes
                                          ? spannedCellBounds.X + dx
                                          : spannedCellBounds.X - dx;*/
            }
            
            //
            int spannedCellWidth = 0;
            for (int i = ownerCell.ColumnIndex; i < ownerCell.ColumnIndex + ownerCell.ColumnSpan; i++)
            {
                DataGridViewColumn column = dataGridView.Columns[i];
                if (!column.Visible) continue;
                spannedCellWidth += column.Width;
            }

            if (dataGridView.RightToLeft == RightToLeft.Yes)
            {
                spannedCellBounds.X = spannedCellBounds.Right - spannedCellWidth;
            }
            spannedCellBounds.Width = spannedCellWidth;
            //
            int spannedCellHieght = (dataGridView.ColumnHeadersVisible) ? dataGridView.ColumnHeadersHeight : 0;
            spannedCellBounds.Height = spannedCellHieght;

            if (singleVerticalBorderAdded && InFirstDisplayedColumn(ownerCell))
            {
                spannedCellBounds.Width++;
                if (dataGridView.RightToLeft != RightToLeft.Yes)
                {
                    if (childCell.ColumnIndex != dataGridView.FirstDisplayedScrollingColumnIndex)
                    {
                        spannedCellBounds.X--;
                    }
                }
                else
                {
                    if (childCell.ColumnIndex == dataGridView.FirstDisplayedScrollingColumnIndex)
                    {
                        spannedCellBounds.X--;
                    }
                }
            }
            if (singleHorizontalBorderAdded && InFirstDisplayedRow(ownerCell))
            {
                spannedCellBounds.Height++;
                if (childCell.RowIndex != dataGridView.FirstDisplayedScrollingRowIndex)
                {
                    spannedCellBounds.Y--;
                }
            }
            return spannedCellBounds;
        }

        public static DataGridViewAdvancedBorderStyle AdjustCellBorderStyle<TCell>(TCell cell)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridViewAdvancedBorderStylePlaceholder = new DataGridViewAdvancedBorderStyle();
            var dataGridView = cell.DataGridView;
            return cell.AdjustCellBorderStyle(
                dataGridView.AdvancedCellBorderStyle,
                dataGridViewAdvancedBorderStylePlaceholder,
                SingleVerticalCellBorderAdded(dataGridView),
                SingleHorizontalCellBorderAdded(dataGridView),
                InFirstDisplayedColumn(cell),
                InFirstDisplayedRow(cell));
        }

        public static DataGridViewAdvancedBorderStyle AdjustHeaderBorderStyle<TCell>(TCell cell)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridViewAdvancedBorderStylePlaceholder = new DataGridViewAdvancedBorderStyle();
            var dataGridView = cell.DataGridView;
            return cell.AdjustCellBorderStyle(
                dataGridView.AdvancedColumnHeadersBorderStyle,
                dataGridViewAdvancedBorderStylePlaceholder,
                SingleVerticalHeaderBorderAdded(dataGridView),
                SingleHorizontalHeaderBorderAdded(dataGridView),
                InFirstDisplayedColumn(cell),
                true);
        }

        public static bool InFirstDisplayedColumn<TCell>(TCell cell)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = cell.DataGridView;
            return dataGridView.FirstDisplayedScrollingColumnIndex >= cell.ColumnIndex
                   && dataGridView.FirstDisplayedScrollingColumnIndex < cell.ColumnIndex + cell.ColumnSpan;
        }

        public static bool InFirstDisplayedRow<TCell>(TCell cell)
            where TCell : DataGridViewCell, IZDataGridViewCell
        {
            var dataGridView = cell.DataGridView;
            return dataGridView.FirstDisplayedScrollingRowIndex >= cell.RowIndex
                   && dataGridView.FirstDisplayedScrollingRowIndex < cell.RowIndex + cell.RowSpan;
        }

        public void SetToolTipText(DataGridViewCell cell, bool showMeasure)
        {
            if (cell == null) return;

            DataGridViewColumn column = this.Columns[cell.ColumnIndex];
            if (column == null) return;

            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;

            object formattedValue = cell.FormattedValue;
            if (formattedValue == null || string.IsNullOrEmpty(formattedValue.ToString()))
            {
                cell.ToolTipText = string.Empty;
                return;
            }

            if (showMeasure && parameter.Measure != ZMeasures.None) cell.ToolTipText = formattedValue + " " + GetMeasureString(parameter.Measure);
            else cell.ToolTipText = formattedValue.ToString();
        }

        private string GetMeasureString(ZMeasures measure)
        {
            switch (measure)
            {
                case ZMeasures.C:
                    return "C";
                case ZMeasures.GHz:
                    return "GHz";
                case ZMeasures.In:
                    return "in";
                case ZMeasures.Mils:
                    return "mils";
                case ZMeasures.Oz:
                    return "oz";
                case ZMeasures.Percent:
                    return "%";
                case ZMeasures.Um:
                    return "um";
                case ZMeasures.Db:
                    return "dB";
                case ZMeasures.Db_per_In:
                    return "dB/in";
                case ZMeasures.Ns:
                    return "ns";
                case ZMeasures.In_per_Ns:
                    return "in/ns";
                case ZMeasures.Ohm:
                    return "ohms";
                case ZMeasures.Ohm_by_M:
                    return "ohm*m";
                default:
                    return "";
            }
        }

        #region Private Methods

        private static int GetFirstVisibleColumnIndex(DataGridView dataGridView, int startIndex, int span)
        {
            int index = -1;
            int displayIndex = -1;
            for (int i = startIndex; i < startIndex + span; i++)
            {
                if (dataGridView.Columns[i].Visible)
                {
                    if (displayIndex == -1 || dataGridView.Columns[i].DisplayIndex < displayIndex)
                    {
                        index = i;
                        displayIndex = dataGridView.Columns[i].DisplayIndex;
                    }
                }
            }

            return index;
        }

        private static int GetFirstVisibleRowIndex(DataGridView dataGridView, int startIndex, int span)
        {
            for (int i = startIndex; i < startIndex + span; i++)
            {
                if (dataGridView.Rows[i].Visible)
                {
                    return i;
                }
            }
            return -1;
        }

        #endregion  
    }
}
