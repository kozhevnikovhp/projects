using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Linq.Expressions;
using System.Text.RegularExpressions;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.UI.Grid.Filter;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerDMLPanel : DockContent
    {
        bool isIgnoreFilterChanged = false;
        private ZMaterialLibrary library;
        Control curentEditingControl = null;
        private static string defaultDMLSortString = "[Resin] ASC, [H] ASC, [Type] ASC, [Material] ASC, [Manufacturer] ASC";

        private static Task task = Task.FromResult(false);

        public ZPlannerDMLPanel()
        {
            InitializeComponent();

            this.dmlGridView.DefaultCellBehavior = ZDataGridViewColumnHeaderCellBehavior.SortingFiltering;

            this.dmlGridView.EditingControlShowing += dmlGridView_EditingControlShowing;
            this.dmlGridView.CellEndEdit += dmlGridView_CellEndEdit;
            this.dmlGridView.CellValueChanged += dmlGridView_CellValueChanged;
            this.dmlGridView.ColumnHeaderMouseDoubleClick += dmlGridView_ColumnHeaderMouseDoubleClick;
            this.dmlGridView.ColumnHeaderMouseClick += dmlGridView_ColumnHeaderMouseClick; 
            columnContextMenu.ItemClicked += columnContextMenu_ItemClicked;
            insertHiddenColumnBeforeToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertHiddenColumnAfterToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertExistingColumnBeforeToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertExistingColumnAfterToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            materialContextMenu.ItemClicked += materialContextMenu_ItemClicked;            
            dmlGridView.RowHeaderMouseClick += dmlGridView_MaterialHeaderMouseClick;
            dmlGridView.ColumnDisplayIndexChanged += DataGridView_ColumnDisplayIndexChanged;
            dmlGridView.FilterStringChanged += dmlGridView_FilterStringChanged;
            dmlGridView.SortStringChanged += dmlGridView_SortStringChanged;
            dmlGridView.SelectionChanged += dmlGridView_SelectionChanged;

            InitContextMenuSubitems();

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;
        }

        void dmlGridView_FilterStringChanged(object sender, EventArgs e)
        {
            if (isIgnoreFilterChanged) return;

            Cursor currentCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;
            ZPlannerManager.StatusMenu.StartProgress("Applying the filter ...");

            //dmlGridView.ScrollBars = ScrollBars.Horizontal;
            dmlGridView.SuspendState();
            dmlGridView.SuspendLayout();
            FilterDMLGrid(sender as ZDataGridView);
            dmlGridView.ResumeLayout();
            dmlGridView.ResumeState();
            //dmlGridView.ScrollBars = ScrollBars.Both;
            ZPlannerManager.MainMenu.ClearCustomFilterList();

            ZPlannerManager.StatusMenu.StopProgress("Applying the filter ...");
            Cursor.Current = currentCursor;
            SelectFirstMaterial();
        }

        void FilterDMLGrid(ZDataGridView grid)
        {
            List<IFilter> filters = new List<IFilter>();
            string filterString = grid.FilterString.Trim() + " AND ";
            //UnFilterAllRows();

            Regex filtersPattern = new Regex(@"([(]?(?<filter>.+?)[)]? AND )+?", RegexOptions.Compiled | RegexOptions.Singleline);
            Regex columnPattern = new Regex(@"^[\[](?<column>.+?)[\]]\s(?<command>IN|NOT LIKE|LIKE|=|<>|>|<|>=|<=)\s[(]?(?<value>.+?)[)]?$", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match filterMatch in filtersPattern.Matches(filterString))
            {
                if (!filterMatch.Success) continue;
                string filterValue = filterMatch.Groups["filter"].Value;
                Match columnMatch = columnPattern.Match(filterValue);
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

                if (string.IsNullOrEmpty(columnName)) continue;

                ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
                if (column == null) continue;

                switch (command)
                {
                    case "IN":
                        filters.Add(new InFilter(column, values));
                        break;
                    case "=":
                        filters.Add(new EqFilter(column, values[0]));
                        break;
                    case "<>":
                        filters.Add(new NEFilter(column, values[0]));
                        break;
                    case "<":
                        filters.Add(new LtFilter(column, values[0]));
                        break;
                    case "<=":
                        filters.Add(new LEFilter(column, values[0]));
                        break;
                    case ">":
                        filters.Add(new GtFilter(column, values[0]));
                        break;
                    case ">=":
                        filters.Add(new GEFilter(column, values[0]));
                        break;
                    case "NOT LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike)
                            {
                                filters.Add(new NCFilter(column, values[0]));
                            }
                            else
                            {
                                filters.Add(new NBlFilter(column, values[0]));
                            }
                        }
                        else
                        {
                            if (isEndLike)
                            {
                                filters.Add(new NElFilter(column, values[0]));
                            }
                            else
                            {
                                filters.Add(new NEFilter(column, values[0]));
                            }
                        }
                        break;
                    case "LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike)
                            {
                                filters.Add(new CnFilter(column, values[0]));
                            }
                            else
                            {
                                filters.Add(new BlFilter(column, values[0]));
                            }
                        }
                        else
                        {
                            if (isEndLike)
                            {
                                filters.Add(new ElFilter(column, values[0]));
                            }
                            else
                            {
                                filters.Add(new EqFilter(column, values[0]));
                            }
                        }
                        break;
                }

                /*switch (command)
                {
                    case "IN":
                        InFilterRows(columnName, values);
                        break;
                    case "=":
                        EqFilterRows(columnName, values[0]);
                        break;
                    case "<>":
                        NEFilterRows(columnName, values[0]);
                        break;
                    case "<":
                        LtFilterRows(columnName, values[0]);
                        break;
                    case "<=":
                        LEFilterRows(columnName, values[0]);
                        break;
                    case ">":
                        GtFilterRows(columnName, values[0]);
                        break;
                    case ">=":
                        GEFilterRows(columnName, values[0]);
                        break;
                    case "NOT LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike)
                            {
                                NCFilterRows(columnName, values[0]);
                            }
                            else
                            {
                                NLbFilterRows(columnName, values[0]);
                            }
                        }
                        else
                        {
                            if (isEndLike)
                            {
                                NLeFilterRows(columnName, values[0]);
                            }
                            else
                            {
                                NEFilterRows(columnName, values[0]);
                            }
                        }
                        break;
                    case "LIKE":
                        if (isBeginLike)
                        {
                            if (isEndLike)
                            {
                                CnFilterRows(columnName, values[0]);
                            }
                            else
                            {
                                LbFilterRows(columnName, values[0]);
                            }
                        }
                        else
                        {
                            if (isEndLike)
                            {
                                LeFilterRows(columnName, values[0]);
                            }
                            else
                            {
                                EqFilterRows(columnName, values[0]);
                            }
                        }
                        break;
                }*/
            }

            foreach (ZDataGridViewRow row in dmlGridView.Rows)
            {
                ZMaterial material = row.Tag as ZMaterial;
                if (material == null) continue;

                bool isFiltered = false;
                foreach (IFilter filter in filters)
                {
                    if (!filter.IsMatch(row)) { isFiltered = true; break; }
                }
                material.IsFiltered = isFiltered;
            }
        }

        private void dmlGridView_SortStringChanged(object sender, EventArgs e)
        {
            if (isIgnoreFilterChanged) return;

            ZDataGridView grid = sender as ZDataGridView;
            string sortString = grid.SortString.Trim();

            //if (string.IsNullOrWhiteSpace(sortString)) sortString = defaultDMLSortString;

            List<CompareItem> compareItems = new List<CompareItem>();
            Regex sortPattern = new Regex(@"[\[](?<column>\S+?)[\]]\s(?<direction>ASC|DESC)[,]?", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match sortMatch in sortPattern.Matches(sortString))
            {
                if (!sortMatch.Success) continue;
                string columnName = sortMatch.Groups["column"].Value;
                string sortDirection = sortMatch.Groups["direction"].Value;

                if (string.IsNullOrEmpty(columnName)) continue;

                compareItems.Insert(0, new CompareItem(columnName, sortDirection == "ASC" ? ListSortDirection.Ascending : ListSortDirection.Descending));
            }

            ZDataGridViewRowComparer comparer = new ZDataGridViewRowComparer(compareItems);
            grid.Sort(comparer);
            if (grid.FirstDisplayedScrollingRowIndex > 0) grid.FirstDisplayedScrollingRowIndex = grid.Rows.GetFirstRow(DataGridViewElementStates.Visible);
            ZPlannerManager.MainMenu.ClearCustomFilterList();
        }

        void UnFilterAllRows()
        {
            ZPlannerManager.Dml.Materials.Where(x => x.IsFiltered == true).Select(x => x.IsFiltered = false).ToList();
        }

        void InFilterRows(string columnName, string[] values)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            /*(from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !values.Contains(GetFormatedValue(column, r))
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);*/

            //dmlGridView.Rows.Cast<DataGridViewRow>().Where(r => r.Visible == true && !values.Contains(GetFormatedValue(column, r))).Select(r => (r.Tag as ZMaterial).IsFiltered = true).ToList();

            //materials.Where(m => m.IsHidden == false && m.IsFiltered == false && !values.Contains(m.MaterialParameters.Find(x => x.ID == columnName).FormatedValue)).Select(m => m.IsFiltered = true).ToList();

            /*foreach(ZMaterial material in ZPlannerManager.Dml.Materials)
            {
                if (material.IsHidden || material.IsFiltered) continue;
                if (!values.Contains(material.GetMaterialParameterValue(columnName))) material.IsFiltered = true;
            }*/

            foreach(ZDataGridViewRow row in dmlGridView.Rows)
            {
                if (!row.Visible) continue;
                if (!values.Contains(column.GetFormatedValue(row))) (row.Tag as ZMaterial).IsFiltered = true;
            }
        }

        /*
        void EqFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !value.Equals(column.GetFormatedValue(r))
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void NEFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && value.Equals(column.GetFormatedValue(r))
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void LtFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            double d1, d2;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !(double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(column.GetFormatedValue(r), NumberStyles.Any, CultureInfo.InvariantCulture, out d2) && d2 < d1)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void LEFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            double d1, d2;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !(double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(column.GetFormatedValue(r), NumberStyles.Any, CultureInfo.InvariantCulture, out d2) && d2 <= d1)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void GtFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            double d1, d2;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !(double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(column.GetFormatedValue(r), NumberStyles.Any, CultureInfo.InvariantCulture, out d2) && d2 > d1)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void GEFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            double d1, d2;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !(double.TryParse(value, NumberStyles.Any, CultureInfo.InvariantCulture, out d1) && double.TryParse(column.GetFormatedValue(r), NumberStyles.Any, CultureInfo.InvariantCulture, out d2) && d2 >= d1)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void CnFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !column.GetFormatedValue(r).Contains(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void NCFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && column.GetFormatedValue(r).Contains(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void LbFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !column.GetFormatedValue(r).StartsWith(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void NLbFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && column.GetFormatedValue(r).StartsWith(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void LeFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && !column.GetFormatedValue(r).EndsWith(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }

        void NLeFilterRows(string columnName, string value)
        {
            ZDataGridViewColumn column = dmlGridView.Columns[columnName] as ZDataGridViewColumn;
            if (column == null) return;

            (from DataGridViewRow r in dmlGridView.Rows
             where r.Visible == true && column.GetFormatedValue(r).EndsWith(value)
             select r.Tag as ZMaterial).ToList().ForEach(m => m.IsFiltered = true);
        }
        */

        public void ClearFilter()
        {
            Cursor currentCursor = Cursor.Current;
            Cursor.Current = Cursors.WaitCursor;

            //isIgnoreFilterChanged = true;

            //dmlGridView.ClearFilter(true);
            //dmlGridView.ClearSort(true);
            SetDefaultFilter();

            //isIgnoreFilterChanged = false;

            //dmlGridView.OnFilterStringChanged();
            //dmlGridView.OnFiltered();
            //dmlGridView.OnSortStringChanged();
            //dmlGridView.OnSorted();

            Cursor.Current = currentCursor;
        }

        public void SetDefaultFilter()
        {
            //SetLibraryCategoryFilter(true, false, false);
            //SetMaterialTypeFilter(true, true, false);
            //SetHiddenMaterialsFilter(true);
            SetCurrentFilterAndSort(string.Format("([Category] IN ({0})) AND ([Type] IN ('Core', 'Prepreg')) AND ([Hide] IN (False)) ", ZPlannerManager.CategoryFilter), "[H] ASC, [Type] ASC, [Material] ASC, [Manufacturer] ASC");
        }

        private void InitContextMenuSubitems()
        {
            foreach (string type in ZStringConstants.DielectricType)
            {
                ToolStripMenuItem materialTypeToolStripMenuItem = new ToolStripMenuItem(type);
                materialTypeToolStripMenuItem.Name = type;
                addMaterialToolStripMenuItem.DropDownItems.Add(materialTypeToolStripMenuItem);
            }

            addMaterialToolStripMenuItem.DropDownItemClicked += materialContextMenu_ItemClicked;
            sendToStackupMaterialToolStripMenuItem.DropDownItemClicked += materialContextMenu_ItemClicked;
        }
        
        /// <summary>
        /// Loads form's data.
        /// </summary>
        internal void LoadData(ZMaterialLibrary library)
        {
            this.library = library;
            LoadDMLSettings();
            FillGridView();
            FormatGridView();

            //isIgnoreFilterChanged = true;
            SetDefaultFilter();
            //isIgnoreFilterChanged = false;

            //dmlGridView.OnFilterStringChanged();
            //dmlGridView.OnFiltered();
            //dmlGridView.OnSortStringChanged();
            //dmlGridView.OnSorted();

            library.Materials.CollectionChanged += Materials_CollectionChanged;
            dmlGridView.CurrentCellChanged += DataGridView_CurrentCellChanged;
        }

        void DataGridView_CurrentCellChanged(object sender, EventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            //grid.CurrentRowIndex = (grid.CurrentCell != null) ? grid.CurrentCell.RowIndex : -2;

            Rectangle headersRectangle = grid.DisplayRectangle;
            headersRectangle.Height = grid.ColumnHeadersHeight;
            grid.Invalidate(headersRectangle);

            ZPlannerManager.IsMaterialSelected = dmlGridView.CurrentRow != null;
        }

        public void UnitsChanged()
        {
            // TODO: make it more smart than to remove everything and re-fill it from scratch. We just need to rename 3 columns and recalculate their values.
            // But good enough for now
            // (EvgenyK)
            Clear();
            FillGridView();
        }

        private void FillGridView()
        {
            this.dmlGridView.ClipboardCopyMode = DataGridViewClipboardCopyMode.Disable;
            this.dmlGridView.EditMode = DataGridViewEditMode.EditOnKeystrokeOrF2;

            RecalculateOrder();
            
            // Add Columns.
            AddColumns(library.Parameters);

            // Add Rows and Cells.
            AddRows(library.Materials);

            RecalculateColumnHeaderName();
            ResizeColumns();

            RecalculateRowHeaderName();

            dmlGridView.Sorted += dmlGridView_Sorted;
            dmlGridView.Filtered += dmlGridView_Filtered;
        }

        void dmlGridView_Filtered(object sender, EventArgs e)
        {
            if (isIgnoreFilterChanged) return;

            RecalculateRowHeaderName();
        }

        void dmlGridView_Sorted(object sender, EventArgs e)
        {
            if (isIgnoreFilterChanged) return;

            RecalculateRowHeaderName();
        }

        private void AddColumns(ZList<ZParameter> parameters)
        {
            dmlGridView.SuspendLayout();
            // Add Columns.
            foreach (var parameter in parameters.OrderBy(x => x.Order))
            {
                ZDataGridViewColumn column;
                switch (parameter.ValueType)
                {
                    case ZValueType.Text:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                    case ZValueType.Link:
                        column = new ZDataGridViewLinkBoxColumn();
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
                        column = new ZDataGridViewCheckBoxWithTextColumn();
                        break;
                    case ZValueType.Select:
                        string[] availableValues;
                        if (parameter.List != null && parameter.List.GetValues() != null) availableValues = parameter.List.GetValues().ToArray();
                        else if (parameter.ID == ZStringConstants.ParameterIDLayerType) availableValues = ZStringConstants.LayerType;
                        else availableValues = new string[] { "" };

                        if (parameter.List != null && parameter.List.GetDictionary().Count > 0)
                        {
                            ZDataGridViewSelectBoxColumn selectBoxColumn = new ZDataGridViewSelectBoxColumn(parameter.List.GetDictionary());
                            column = selectBoxColumn;
                        }
                        else
                        {
                            ZDataGridViewComboBoxColumn comboBoxColumn = new ZDataGridViewComboBoxColumn(availableValues);
                            column = comboBoxColumn;
                        }

                        break;
                    default:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                }

                //column.DefaultCellStyle // TBD
                column.Resizable = DataGridViewTriState.True;

                column.Name = parameter.ID;
                column.HeaderText = parameter.Title;
                
                // Hack: if the units are English, set the header as it was read from the library. If Metric - compose name form ID and "um"
                // Probably, changing parameter.Title is better.. 
                if (parameter.ID == ZStringConstants.DMLParameterIDH ||
                    parameter.ID == ZStringConstants.DMLParameterIDWeavePitch ||
                    parameter.ID == ZStringConstants.DMLParameterIDFillPitch)
                {
                    if (Options.TheOptions.isUnitsMetric())
                    { // rename column
                        column.HeaderText = parameter.ID + " (mm)";
                    }
                }

                column.ReadOnly = parameter.IsReadOnly;
                column.DisplayIndex = parameter.Order;
                column.Frozen = parameter.IsFrozen;
                column.ToolTipText = parameter.Title;
                column.Visible = !parameter.IsHidden && !parameter.IsPrivate;

                column.Tag = parameter; // Remove it in case it is not required.
                parameter.PropertyChanged += column.ZParameter_PropertyChanged;
                parameter.PropertyChanged += ZParameter_PropertyChanged;

                this.dmlGridView.Columns.Add(column);
            }
            dmlGridView.ResumeLayout();
        }

        void ZParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "IsHidden":
                case "IsPrivate":
                    RecalculateColumnHeaderName();
                    break;
            }
        }

        private void AddRows(ZList<ZMaterial> materials)
        {
            List<ZDataGridViewRow> rowList = new List<ZDataGridViewRow>();
            foreach (var material in materials)
            {
                ZDataGridViewRow row = CreateRow(material);
                if (row != null) rowList.Add(row);
            }

            if (rowList.Count > 0)
            {
                dmlGridView.SuspendLayout();
                this.dmlGridView.Rows.AddRange(rowList.ToArray());
                dmlGridView.ResumeLayout();
            }
        }

        private void AddRow(ZMaterial material)
        {
            ZDataGridViewRow row = CreateRow(material);

            if (row != null)
                this.dmlGridView.Rows.Add(row);
        }

        private void InsertRow(int index, ZMaterial material)
        {
            ZDataGridViewRow row = CreateRow(material);

            if (row != null)
                this.dmlGridView.Rows.Insert(index, row);
        }

        private ZDataGridViewRow CreateRow(ZMaterial material)
        {
            ZDataGridViewRow row = new ZDataGridViewRow();
            //row.DefaultCellStyle // TBD
            row.Resizable = DataGridViewTriState.True;
            row.Name = material.ID;
            row.Frozen = material.IsFrozen;
            row.ReadOnly = material.IsReadOnly || !material.IsCustom;
            row.Visible = !material.IsHidden && !material.IsFiltered;
            if (material.Height > 0) row.Height = material.Height;
            row.Tag = material;
            row.HeaderCell = new ZDataGridViewRowHeaderCell();
            row.HeaderCell.Value = (material.Order + 1).ToString();
            material.PropertyChanged += row.ZMaterial_PropertyChanged;

            foreach (DataGridViewColumn column in this.dmlGridView.Columns)
            {
                DataGridViewCell cell;
                ZParameter parameter = (ZParameter)column.Tag;
                switch (parameter.ValueType)
                {
                    case ZValueType.Text:
                        cell = new ZDataGridViewTextBoxCell();
                        break;
                    case ZValueType.Link:
                        cell = new ZDataGridViewLinkBoxCell();
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
                        cell = new ZDataGridViewCheckBoxWithTextCell();
                        break;
                    case ZValueType.Select:
                        string[] availableValues;
                        bool editable = false;
                        if (parameter.List != null && parameter.List.GetValues() != null)
                        {
                            availableValues = parameter.List.GetValues().ToArray();
                            editable = true;
                        }
                        else if (parameter.ID == ZStringConstants.ParameterIDLayerType) availableValues = ZStringConstants.LayerType;
                        else availableValues = new string[] { "" };

                        if (parameter.List != null && parameter.List.GetDictionary().Count > 0)
                        {
                            ZDataGridViewSelectBoxCell selectBoxCell = new ZDataGridViewSelectBoxCell(parameter.List.GetDictionary());
                            selectBoxCell.Editable = editable;
                            selectBoxCell.List = parameter.List;
                            cell = selectBoxCell;
                        }
                        else
                        {
                            ZDataGridViewComboBoxCell comboBoxCell = new ZDataGridViewComboBoxCell(availableValues);
                            comboBoxCell.Editable = editable;
                            cell = comboBoxCell;
                        }

                        break;
                    default:
                        cell = new ZDataGridViewTextBoxCell();
                        break;
                }

                row.Cells.Add(cell);

                ZMaterialParameter materialParameter = material.MaterialParameters.Find(X => X.ID == parameter.ID);

                if (materialParameter != null)
                {
                    //cell.ToolTipText = materialParameter.Value;
                    string value = materialParameter.Value;
                    if (parameter.ID == ZStringConstants.DMLParameterIDH         ||
                        parameter.ID == ZStringConstants.DMLParameterIDWeavePitch ||
                        parameter.ID == ZStringConstants.DMLParameterIDFillPitch)
                    {
                        if (Options.TheOptions.isUnitsMetric())
                        {
                            double fValue = 0;
                            double.TryParse(value, out fValue);
                            fValue *= Units.fMilsToMillimeters;
                            value = fValue.ToString();
                        }
                    }
                    cell.Value = value;
                    cell.Tag = materialParameter; // Remove it in case it is not required.

                    if (materialParameter.ID == ZStringConstants.DMLParameterIDHide) cell.ReadOnly = false;
                    if (materialParameter.ID == ZStringConstants.DMLParameterIDQualified) cell.ReadOnly = ZPlannerManager.IsCorporateDeltaLoaded;

                    materialParameter.PropertyChanged += (cell as IZDataGridViewCell).ZMaterialParameter_PropertyChanged;
                    materialParameter.PropertyChanged += ZMaterialParameter_PropertyChanged;

                }
                else
                {
                    cell.Value = "";
                }

            }

            return row;
        }

        private void RecalculateOrder()
        {
            int index = 0;
            foreach (ZMaterial material in ZPlannerManager.Dml.Materials)
            {
                material.Order = index;
                ++index;
            }
        }

        private void FormatGridView()
        {
            for (int i = 0; i < dmlGridView.RowCount; ++i)
            {
                FormatGridRow(i);
            }
        }

        private void FormatGridRow(int index)
        {
            ZMaterial material = dmlGridView.Rows[index].Tag as ZMaterial;
            if (material != null)
            {
                ZMaterialParameter materialParameter = material.MaterialParameters.Find(x => x.ID == ZStringConstants.DMLParameterIDType);
                if (materialParameter != null)
                {
                    ColorGridsRow(index, ZColorConstants.GetLayerColor(materialParameter.Value));
                }

                ColorCategoryCell(index);
            }
        }

        private void ResizeColumns()
        {
            foreach (DataGridViewColumn column in this.dmlGridView.Columns)
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter != null)
                {
                    if (parameter.Width <= 0)
                    {
                        column.AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
                        parameter.Width = column.Width;
                    }
                    column.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                    column.Width = parameter.Width;
                }
            }

            this.dmlGridView.AutoResizeColumnHeadersHeight();
        }

        private void ColorGridsRow(int rowIndex, Color color)
        {
            dmlGridView.Rows[rowIndex].DefaultCellStyle.BackColor = color;
        }

        private void ColorCategoryCell(int rowIndex)
        {
            var cell = dmlGridView.Rows[rowIndex].Cells[ZStringConstants.DMLParameterIDCategory];
            if (cell == null) return;

            if (cell.Value as string != ZStringConstants.MaterialCategoryZZero) cell.Style.ForeColor = ZColorConstants.LocalLibraryColor;
        }

        internal void ShowHeaders(bool isVisible)
        {
            dmlGridView.ColumnHeaderNamesVisible = isVisible;

            dmlGridView.RowHeadersVisible = isVisible;

            //dmlGridView.ResizeGrid();
        }

        void DataGridView_ColumnDisplayIndexChanged(object sender, DataGridViewColumnEventArgs e)
        {
            DataGridViewColumn column = e.Column;
            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;

            if (parameter.Order != column.DisplayIndex) parameter.Order = column.DisplayIndex;

            RecalculateColumnHeaderName();
        }

        void ZMaterialParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            ZMaterialParameter materialParameter = sender as ZMaterialParameter;

            if (materialParameter == null) return;

            if (materialParameter.ID == ZStringConstants.DMLParameterIDWarpYarnCount)
            {
                double dvalue;

                if (double.TryParse(materialParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out dvalue) && dvalue != 0)
                    materialParameter.Material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDWeavePitch, (1000 / dvalue).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
            }

            if (materialParameter.ID == ZStringConstants.DMLParameterIDFillYarnCount)
            {
                double dvalue;

                if (double.TryParse(materialParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out dvalue) && dvalue != 0)
                    materialParameter.Material.SetMaterialParameterValue(ZStringConstants.DMLParameterIDFillPitch, (1000 / dvalue).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
            }

            if (materialParameter.ID == ZStringConstants.DMLParameterIDType)
            {
                FormatGridView();
            }

            ZPlannerManager.PropertiesPanel.UpdateProperties();
        }

        private void dmlGridView_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (e.Control.GetType() == typeof(ZDataGridViewSelectBoxEditingControl))
            {
                ZDataGridViewSelectBoxEditingControl control = e.Control as ZDataGridViewSelectBoxEditingControl;
                ZDataGridViewSelectBoxCell cell = dmlGridView.CurrentCell as ZDataGridViewSelectBoxCell;
                if (cell != null && cell.Editable)
                {
                    control.DropDownStyle = ComboBoxStyle.DropDown;
                    control.Validating += cell.Validating;
                    return;
                }
            }

            if (e.Control.GetType() == typeof(DataGridViewComboBoxEditingControl))
            {
                DataGridViewComboBoxEditingControl control = e.Control as DataGridViewComboBoxEditingControl;
                ZDataGridViewComboBoxCell cell = dmlGridView.CurrentCell as ZDataGridViewComboBoxCell;
                if (cell != null && cell.Editable)
                {
                    control.DropDownStyle = ComboBoxStyle.DropDown;
                    control.Validating += cell.Validating;
                    return;
                }
            }

            if (e.Control.GetType() == typeof(ZDataGridViewMaskedTextBoxEditingControl))
            {
                ZDataGridViewMaskedTextBoxEditingControl control = e.Control as ZDataGridViewMaskedTextBoxEditingControl;
                control.Mask = "";
                return;
            }

            if (e.Control.GetType() == typeof(DataGridViewTextBoxEditingControl))
            {
                DataGridViewTextBoxEditingControl control = e.Control as DataGridViewTextBoxEditingControl;
                DataGridView grid = sender as DataGridView;
                ZDataGridViewNumberBoxCell numberCell = grid.CurrentCell as ZDataGridViewNumberBoxCell;
                if (numberCell != null)
                {
                    curentEditingControl = control;
                    control.KeyPress += NumberBox_KeyPress;
                }

                ZDataGridViewPercentBoxCell percentCell = grid.CurrentCell as ZDataGridViewPercentBoxCell;
                if (percentCell != null)
                {
                    curentEditingControl = control;
                    control.KeyPress += PercentBox_KeyPress;
                }

                ZDataGridViewRatioBoxCell ratioCell = grid.CurrentCell as ZDataGridViewRatioBoxCell;
                if (ratioCell != null)
                {
                    curentEditingControl = control;
                    control.KeyPress += RatioBox_KeyPress;
                }
            }
        }

        void dmlGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (curentEditingControl != null)
            {
                curentEditingControl.KeyPress -= NumberBox_KeyPress;
                curentEditingControl.KeyPress -= PercentBox_KeyPress;
                curentEditingControl.KeyPress -= RatioBox_KeyPress;
            }
        }

        private void NumberBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ds = (CultureInfo.InvariantCulture.NumberFormat.NumberDecimalSeparator)[0];
            char gs = (CultureInfo.InvariantCulture.NumberFormat.NumberGroupSeparator)[0];
            char ns = (CultureInfo.InvariantCulture.NumberFormat.NegativeSign)[0];
            DataGridViewTextBoxEditingControl control = sender as DataGridViewTextBoxEditingControl;
            ZDataGridViewNumberBoxCell cell = control.EditingControlDataGridView.CurrentCell as ZDataGridViewNumberBoxCell;

            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && (e.KeyChar != ds) && (e.KeyChar != gs) && (e.KeyChar != ns || !cell.AllowNegative))
            {
                e.Handled = true;
            }

            // only allow one decimal point
            if ((e.KeyChar == ds) && ((sender as TextBox).Text.IndexOf(ds) > -1))
            {
                e.Handled = true;
            }
        }

        private void PercentBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ds = (CultureInfo.InvariantCulture.NumberFormat.NumberDecimalSeparator)[0];
            char gs = (CultureInfo.InvariantCulture.NumberFormat.NumberGroupSeparator)[0];
            char ns = (CultureInfo.InvariantCulture.NumberFormat.NegativeSign)[0];
            char ps = '%';
            DataGridViewTextBoxEditingControl control = sender as DataGridViewTextBoxEditingControl;
            ZDataGridViewPercentBoxCell cell = control.EditingControlDataGridView.CurrentCell as ZDataGridViewPercentBoxCell;

            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && (e.KeyChar != ds) && (e.KeyChar != gs) && (e.KeyChar != ps) && (e.KeyChar != ns || !cell.AllowNegative))
            {
                e.Handled = true;
            }

            // only allow one decimal point
            if ((e.KeyChar == ds) && ((sender as TextBox).Text.IndexOf(ds) > -1))
            {
                e.Handled = true;
            }

            // only allow one percent sign
            if ((e.KeyChar == ps) && ((sender as TextBox).Text.IndexOf(ps) > -1))
            {
                e.Handled = true;
            }
        }

        private void RatioBox_KeyPress(object sender, KeyPressEventArgs e)
        {
            char ds = (CultureInfo.InvariantCulture.NumberFormat.NumberDecimalSeparator)[0];
            char gs = (CultureInfo.InvariantCulture.NumberFormat.NumberGroupSeparator)[0];
            char rs = ':';
            DataGridViewTextBoxEditingControl control = sender as DataGridViewTextBoxEditingControl;
            ZDataGridViewPercentBoxCell cell = control.EditingControlDataGridView.CurrentCell as ZDataGridViewPercentBoxCell;

            if (!char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && (e.KeyChar != ds) && (e.KeyChar != gs) && (e.KeyChar != rs))
            {
                e.Handled = true;
            }

            // only allow one decimal point
            if ((e.KeyChar == ds) && ((sender as TextBox).Text.IndexOf(ds) > -1))
            {
                e.Handled = true;
            }

            // only allow one percent sign
            if ((e.KeyChar == rs) && ((sender as TextBox).Text.IndexOf(rs) > -1))
            {
                e.Handled = true;
            }
        }

        void dmlGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            ZMaterialParameter materialParameter = (sender as DataGridView).Rows[e.RowIndex].Cells[e.ColumnIndex].Tag as ZMaterialParameter;
            object objValue = (sender as DataGridView).Rows[e.RowIndex].Cells[e.ColumnIndex].Value;
            if (materialParameter != null) materialParameter.Value = (objValue != null) ? objValue.ToString() : null;
        }

        void dmlGridView_ColumnHeaderMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
            {
                DataGridView grid = sender as DataGridView;
                if (grid == null) return;

                DataGridViewColumn column = grid.Columns[e.ColumnIndex];
                if (column == null) return;

                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null) return;

                MinimizeColumn(column, parameter);
            }
        }

        void dmlGridView_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            /*if (grid.CurrentRowIndex >= 0)
            {
                int rowIndex = grid.CurrentRowIndex;
                if (grid.Rows.Count > rowIndex && e.ColumnIndex >= 0) grid.CurrentCell = grid[e.ColumnIndex, rowIndex];
            }*/

            if (e.Button != MouseButtons.Right) return;
            if (e.ColumnIndex < 0) return;
            DataGridViewColumn column = grid.Columns[e.ColumnIndex];
            columnContextMenu.Tag = column;
            ZParameter parameter = column.Tag as ZParameter;
            if (parameter != null)
            {
                DataGridViewColumn prevColumn = grid.Columns.GetPreviousColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
                DataGridViewColumn nextColumn = grid.Columns.GetNextColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
                this.moveColumnToTheLeftToolStripMenuItem.Enabled = (prevColumn != null);
                this.moveColumnToTheRightToolStripMenuItem.Enabled = (nextColumn != null);
                this.minimizeColumnToolStripMenuItem.Checked = parameter.IsCollapsed;
            }

            this.insertHiddenColumnBeforeToolStripMenuItem.DropDownItems.Clear();
            this.insertHiddenColumnAfterToolStripMenuItem.DropDownItems.Clear();
            this.insertExistingColumnBeforeToolStripMenuItem.DropDownItems.Clear();
            this.insertExistingColumnAfterToolStripMenuItem.DropDownItems.Clear();

            this.showColumnToolStripMenuItem.Enabled = false;
            this.insertHiddenColumnBeforeToolStripMenuItem.Enabled = false;
            this.insertHiddenColumnAfterToolStripMenuItem.Enabled = false;
            this.insertExistingColumnBeforeToolStripMenuItem.Enabled = false;
            this.insertExistingColumnAfterToolStripMenuItem.Enabled = false;

            Dictionary<string, List<ZDataGridViewColumn>> grouppedColumns = grid.GetColumnsOrderedByTitle();
            foreach (string group in grouppedColumns.Keys)
            {
                //if (group != string.Empty)
                foreach (ZDataGridViewColumn columnToShow in grouppedColumns[group])
                {
                    ZParameter parameterToShow = columnToShow.Tag as ZParameter;
                    if (parameterToShow != null && parameter != parameterToShow && !parameterToShow.IsPrivate)
                    {
                        if (parameterToShow.IsHidden)
                        {
                            ToolStripMenuItem newBeforeItem = new ToolStripMenuItem(parameterToShow.Title);
                            newBeforeItem.Name = parameterToShow.ID;
                            ToolStripMenuItem newAfterItem = new ToolStripMenuItem(parameterToShow.Title);
                            newAfterItem.Name = parameterToShow.ID;
                            this.insertHiddenColumnBeforeToolStripMenuItem.DropDownItems.Add(newBeforeItem);
                            this.insertHiddenColumnAfterToolStripMenuItem.DropDownItems.Add(newAfterItem);
                            this.showColumnToolStripMenuItem.Enabled = true;
                            this.insertHiddenColumnBeforeToolStripMenuItem.Enabled = true;
                            this.insertHiddenColumnAfterToolStripMenuItem.Enabled = true;
                        }
                        else
                        {
                            if (column.DisplayIndex - columnToShow.DisplayIndex != 1)
                            {
                                ToolStripMenuItem newBeforeItem = new ToolStripMenuItem(parameterToShow.Title);
                                newBeforeItem.Name = parameterToShow.ID;
                                this.insertExistingColumnBeforeToolStripMenuItem.DropDownItems.Add(newBeforeItem);
                                this.insertExistingColumnBeforeToolStripMenuItem.Enabled = true;
                            }

                            if (column.DisplayIndex - columnToShow.DisplayIndex != -1)
                            {
                                ToolStripMenuItem newAfterItem = new ToolStripMenuItem(parameterToShow.Title);
                                newAfterItem.Name = parameterToShow.ID;
                                this.insertExistingColumnAfterToolStripMenuItem.DropDownItems.Add(newAfterItem);
                                this.insertExistingColumnAfterToolStripMenuItem.Enabled = true;
                            }
                        }
                    }
                }
            }

            columnContextMenu.Show(Cursor.Position);
        }

        void columnContextMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string itemName = e.ClickedItem.Name;
            string parameterName = string.Empty;

            DataGridViewColumn column = columnContextMenu.Tag as DataGridViewColumn;
            if (column == null) return;

            ZDataGridView grid = column.DataGridView as ZDataGridView;

            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;

            if ((grid.Columns.Contains(e.ClickedItem.Name.Trim('_'))) && e.ClickedItem.OwnerItem != null && (e.ClickedItem.OwnerItem.Name == "insertHiddenColumnBeforeToolStripMenuItem" || e.ClickedItem.OwnerItem.Name == "insertHiddenColumnAfterToolStripMenuItem" || e.ClickedItem.OwnerItem.Name == "insertExistingColumnBeforeToolStripMenuItem" || e.ClickedItem.OwnerItem.Name == "insertExistingColumnAfterToolStripMenuItem"))
            {
                parameterName = e.ClickedItem.Name.Trim('_');
                itemName = e.ClickedItem.OwnerItem.Name;
            }

            int order, orderOld;
            bool hiddenOld;
            ZDataGridViewColumn columnToShow;
            ZParameter parameterToShow;
            List<ColumnsDisplayingStructure> columnsDisplayingStructureList = new List<ColumnsDisplayingStructure>();
            List<ZParameter> parametersToShow = new List<ZParameter>();

            switch (itemName)
            {
                case "moveColumnToTheLeftToolStripMenuItem":
                    order = column.DisplayIndex;
                    DataGridViewColumn previousColumn = grid.Columns.GetPreviousColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
                    
                    columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(column, column.DisplayIndex, previousColumn.DisplayIndex));
                    columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(previousColumn, previousColumn.DisplayIndex, order));
                    column.DisplayIndex = previousColumn.DisplayIndex;
                    previousColumn.DisplayIndex = order;
                    break;
                case "moveColumnToTheRightToolStripMenuItem":
                    order = column.DisplayIndex;
                    DataGridViewColumn nextColumn = grid.Columns.GetNextColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
                    
                    columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(column, column.DisplayIndex, nextColumn.DisplayIndex));
                    columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(nextColumn, nextColumn.DisplayIndex, order));
                    column.DisplayIndex = nextColumn.DisplayIndex;
                    nextColumn.DisplayIndex = order;
                    break;
                case "minimizeColumnToolStripMenuItem":
                    MinimizeColumn(column, parameter);
                    break;
                case "hideColumnToolStripMenuItem":
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        parameter.IsHidden = true;
                        parametersToShow.Add(parameter);
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }

                    new ChangeParametersIsHiddenCommand(parametersToShow, false, true);
                    break;
                case "showColumnToolStripMenuItem":
                    isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        foreach (DataGridViewColumn columnToShown in grid.Columns)
                        {
                            parameterToShow = columnToShown.Tag as ZParameter;
                            if (parameterToShow != null && parameterToShow.IsHidden)
                            {
                                parameterToShow.IsHidden = false;
                                parametersToShow.Add(parameterToShow);
                            }
                        }
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }

                    new ChangeParametersIsHiddenCommand(parametersToShow, true, false);
                    break;
                case "insertHiddenColumnBeforeToolStripMenuItem":
                    columnToShow = grid.GetColumnByName(parameterName);
                    if (columnToShow == null) break;

                    parameterToShow = columnToShow.Tag as ZParameter;
                    if (parameterToShow == null) break;

                    hiddenOld = parameterToShow.IsHidden;
                    if (parameterToShow.IsHidden) parameterToShow.IsHidden = false;

                    order = column.DisplayIndex;
                    orderOld = columnToShow.DisplayIndex;

                    if (order < orderOld)
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex >= order && col.DisplayIndex < orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex + 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex += 1;
                            }
                        }

                        columnToShow.DisplayIndex = order;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    else
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex < order && col.DisplayIndex > orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex - 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex -= 1;
                            }
                        }

                        columnToShow.DisplayIndex = order - 1;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    break;
                case "insertHiddenColumnAfterToolStripMenuItem":
                    columnToShow = grid.GetColumnByName(parameterName);
                    if (columnToShow == null) break;

                    parameterToShow = columnToShow.Tag as ZParameter;
                    if (parameterToShow == null) break;

                    hiddenOld = parameterToShow.IsHidden;
                    if (parameterToShow.IsHidden) parameterToShow.IsHidden = false;

                    order = column.DisplayIndex;
                    orderOld = columnToShow.DisplayIndex;

                    if (order < orderOld)
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex > order && col.DisplayIndex < orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex + 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex += 1;
                            }
                        }

                        columnToShow.DisplayIndex = order + 1;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    else
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex <= order && col.DisplayIndex > orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex - 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex -= 1;
                            }
                        }

                        columnToShow.DisplayIndex = order;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    break;
                case "insertExistingColumnBeforeToolStripMenuItem":
                    columnToShow = grid.GetColumnByName(parameterName);
                    if (columnToShow == null) break;

                    parameterToShow = columnToShow.Tag as ZParameter;
                    if (parameterToShow == null) break;

                    hiddenOld = parameterToShow.IsHidden;
                    if (parameterToShow.IsHidden) parameterToShow.IsHidden = false;

                    order = column.DisplayIndex;
                    orderOld = columnToShow.DisplayIndex;

                    if (order < orderOld)
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex >= order && col.DisplayIndex < orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex + 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex += 1;
                            }
                        }

                        columnToShow.DisplayIndex = order;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    else
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex < order && col.DisplayIndex > orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex - 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex -= 1;
                            }
                        }

                        columnToShow.DisplayIndex = order - 1;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    break;
                case "insertExistingColumnAfterToolStripMenuItem":
                    columnToShow = grid.GetColumnByName(parameterName);
                    if (columnToShow == null) break;

                    parameterToShow = columnToShow.Tag as ZParameter;
                    if (parameterToShow == null) break;

                    hiddenOld = parameterToShow.IsHidden;
                    if (parameterToShow.IsHidden) parameterToShow.IsHidden = false;

                    order = column.DisplayIndex;
                    orderOld = columnToShow.DisplayIndex;

                    if (order < orderOld)
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex > order && col.DisplayIndex < orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex + 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex += 1;
                            }
                        }

                        columnToShow.DisplayIndex = order + 1;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    else
                    {
                        foreach (ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
                        {
                            if (col.DisplayIndex <= order && col.DisplayIndex > orderOld)
                            {
                                ZParameter par = col.Tag as ZParameter;
                                if (par == null) continue;
                                columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(col, par, col.DisplayIndex, col.DisplayIndex - 1, par.IsHidden, par.IsHidden));
                                col.DisplayIndex -= 1;
                            }
                        }

                        columnToShow.DisplayIndex = order;
                        columnsDisplayingStructureList.Add(new ColumnsDisplayingStructure(columnToShow, parameterToShow, orderOld, columnToShow.DisplayIndex, hiddenOld, parameterToShow.IsHidden));
                    }
                    break;
            }

            if (columnsDisplayingStructureList.Count > 0) new ChangeColumnsDisplayingCommand(columnsDisplayingStructureList);
        }

        void dmlGridView_MaterialHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right) return;
            if (e.RowIndex < 0) return;
            DataGridView grid = sender as DataGridView;
            DataGridViewRow row = grid.Rows[e.RowIndex];
            materialContextMenu.Tag = row;
            ZMaterial material = row.Tag as ZMaterial;
            if (!ZPlannerManager.IsLibraryLocked)
            {
                if (material != null)
                {
                    this.addMaterialToolStripMenuItem.Enabled = true;
                    this.removeMaterialToolStripMenuItem.Enabled = ZPlannerManager.IsUserHaveAccessToMaterial(material);
                    bool clipboardNotEmpty = ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials);
                    this.pasteMaterialToolStripMenuItem.Enabled = clipboardNotEmpty;
                    this.copyMaterialToolStripMenuItem.Enabled = true;
                }
            }
            else
            {
                this.addMaterialToolStripMenuItem.Enabled = false;
                this.removeMaterialToolStripMenuItem.Enabled = false;
                this.pasteMaterialToolStripMenuItem.Enabled = false;
                this.copyMaterialToolStripMenuItem.Enabled = false;
            }
                    
            this.sendToStackupMaterialToolStripMenuItem.Enabled = (material != null && !ZPlannerManager.ProjectIsEmpty && !ZPlannerManager.Project.StackupIsEmpty && ZPlannerManager.StackupPanel != null);

            if (this.sendToStackupMaterialToolStripMenuItem.Enabled)
            {
                sendToStackupMaterialToolStripMenuItem.DropDownItems.Clear();

                // Create New Row
                ToolStripMenuItem createButton = new ToolStripMenuItem("Apply selected material to a new stackup row.  ");
                createButton.Tag = string.Empty;
                
                sendToStackupMaterialToolStripMenuItem.DropDownItems.Add(createButton);

                ZLayerType? type = material.GetMaterialType();
                List<ZLayer> allLayers = new List<ZLayer>();

                foreach (ZLayer layer in ZPlannerManager.Project.Stackup.Layers.FindAll(x => x.GetLayerType() == type))
                {
                    ToolStripMenuItem button = new ToolStripMenuItem("Apply selected material to Row " + (layer.Order + 1) + ".  ");
                    List<ZLayer> oneLayer = new List<ZLayer>();
                    oneLayer.Add(layer);
                    allLayers.Add(layer);
                    button.Tag = oneLayer;
                    sendToStackupMaterialToolStripMenuItem.DropDownItems.Add(button);
                }
                
                ToolStripMenuItem allButton = new ToolStripMenuItem(string.Format("Apply selected material to all {0}s.  ", type.ToString()));
                allButton.Tag = allLayers;
                sendToStackupMaterialToolStripMenuItem.DropDownItems.Add(allButton);
            }

            materialContextMenu.Show(Cursor.Position);
        }

        void materialContextMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string itemName = e.ClickedItem.Name;
            ZLayerType type = ZLayerType.Core;

            DataGridViewRow row = materialContextMenu.Tag as DataGridViewRow;
            if (row == null) return;

            DataGridView grid = row.DataGridView;

            ZMaterial material = row.Tag as ZMaterial;
            if (material == null) return;

            if (ZStringConstants.DielectricType.Contains(itemName) && e.ClickedItem.OwnerItem != null && (e.ClickedItem.OwnerItem.Name == "addMaterialToolStripMenuItem"))
            {
                type = ZLayer.LayerTypeDictionary[itemName];
                itemName = e.ClickedItem.OwnerItem.Name;
            }

            if (e.ClickedItem.OwnerItem != null && (e.ClickedItem.OwnerItem.Name == "sendToStackupMaterialToolStripMenuItem")) itemName = e.ClickedItem.OwnerItem.Name;

            switch (itemName)
            {
                case "addMaterialToolStripMenuItem":
                    ZPlannerManager.Dml.AddMaterial(type);
                    break;
                case "removeMaterialToolStripMenuItem":
                    ZPlannerManager.Dml.RemoveMaterial(material);
                    break;
                case "copyMaterialToolStripMenuItem":
                    ZPlannerManager.Dml.CopyMaterial(material);
                    break;
                case "pasteMaterialToolStripMenuItem":
                    ZPlannerManager.Dml.PasteMaterial();
                    break;
                case "sendToStackupMaterialToolStripMenuItem":
                    SendToStackup(material, e.ClickedItem.Tag as List<ZLayer>);
                    break;
            }
        }

        void Materials_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            switch(e.Action)
            {
                case NotifyCollectionChangedAction.Add:
                    int index = e.NewStartingIndex;
                    if (e.NewItems == null || e.NewItems.Count == 0) break;
                    if (index == -1)
                    {
                        foreach (ZMaterial material in e.NewItems)
                        {
                            AddRow(material);
                            FormatGridRow(dmlGridView.Rows.Count - 1);
                        }

                        dmlGridView.CurrentCell = dmlGridView.Rows[dmlGridView.Rows.Count - 1].Cells[0];
                        dmlGridView.FirstDisplayedScrollingRowIndex = dmlGridView.Rows.Count - 1;
                    }
                    else
                    {
                        foreach (ZMaterial material in e.NewItems)
                        {
                            InsertRow(index, material);
                            FormatGridRow(index);
                            ++index;
                        }

                        dmlGridView.CurrentCell = dmlGridView.Rows[index - 1].Cells[0];
                        dmlGridView.FirstDisplayedScrollingRowIndex = index - 1;
                    }
                    RecalculateRowHeaderName();
                    return;
                case NotifyCollectionChangedAction.Remove:
                    if (e.OldItems == null || e.OldItems.Count == 0) break;
                    foreach(DataGridViewRow row in dmlGridView.Rows)
                    {
                        ZMaterial material = row.Tag as ZMaterial;
                        if (material == null) continue;

                        if (e.OldItems.Contains(material)) dmlGridView.Rows.Remove(row);
                    }
                    RecalculateRowHeaderName();
                    return;
                case NotifyCollectionChangedAction.Move:
                    if (e.NewStartingIndex == -1 || e.OldStartingIndex == -1 || e.NewStartingIndex >= dmlGridView.Rows.Count || e.OldStartingIndex >= dmlGridView.Rows.Count) break;
                    if (e.NewItems == null || e.NewItems.Count == 0)
                    {
                        DataGridViewRow row1 = dmlGridView.Rows[e.NewStartingIndex];
                        DataGridViewRow row2 = dmlGridView.Rows[e.OldStartingIndex];
                        dmlGridView.Rows.Remove(row1);
                        dmlGridView.Rows.Insert(e.OldStartingIndex, row1);
                        dmlGridView.Rows.Remove(row2);
                        dmlGridView.Rows.Insert(e.NewStartingIndex, row2);
                        FormatGridRow(e.NewStartingIndex);
                        FormatGridRow(e.OldStartingIndex);
                    }
                    else
                    {
                        DataGridViewRow row = dmlGridView.Rows[e.OldStartingIndex];
                        dmlGridView.Rows.Remove(row);
                        dmlGridView.Rows.Insert(e.NewStartingIndex, row);
                        FormatGridRow(e.NewStartingIndex);
                    }
                    RecalculateRowHeaderName();
                    return;
                case NotifyCollectionChangedAction.Replace:
                    index = e.NewStartingIndex;
                    if (e.NewItems == null || e.NewItems.Count == 0 || index == -1 || index >= dmlGridView.Rows.Count) break;
                    dmlGridView.Rows.Remove(dmlGridView.Rows[index]);
                    InsertRow(index, e.NewItems[0] as ZMaterial);
                    FormatGridRow(index);
                    RecalculateRowHeaderName();
                    return;
                case NotifyCollectionChangedAction.Reset:
                    break;
            }

            Clear();
            FillGridView();
            FormatGridView();
        }

        private void Clear()
        {
            foreach (DataGridViewRow row in dmlGridView.Rows)
            {
                ZMaterial material = row.Tag as ZMaterial;
                if (material == null) continue;

                ZDataGridViewRow zRow = row as ZDataGridViewRow;
                if (zRow != null) material.PropertyChanged -= zRow.ZMaterial_PropertyChanged;

                foreach(DataGridViewCell cell in row.Cells)
                {
                    ZMaterialParameter materialParameter = cell.Tag as ZMaterialParameter;
                    if (materialParameter == null) continue;

                    materialParameter.PropertyChanged -= ZMaterialParameter_PropertyChanged;

                    IZDataGridViewCell zCell = cell as IZDataGridViewCell;
                    if (zCell == null) continue;

                    materialParameter.PropertyChanged -= zCell.ZMaterialParameter_PropertyChanged;
                }
            }

            foreach (DataGridViewColumn column in dmlGridView.Columns)
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null) continue;

                ZDataGridViewColumn zColumn = column as ZDataGridViewColumn;
                if (zColumn == null) continue;

                parameter.PropertyChanged -= zColumn.ZParameter_PropertyChanged;
                parameter.PropertyChanged -= ZParameter_PropertyChanged;
            }

            dmlGridView.Rows.Clear();
            dmlGridView.Columns.Clear();
        }

        void MinimizeColumn(DataGridViewColumn column, ZParameter parameter)
        {
            if (parameter.IsCollapsed)
            {
                parameter.IsCollapsed = false;
                column.Width = parameter.WidthToExpand;
            }
            else
            {
                parameter.IsCollapsed = true;
                parameter.WidthToExpand = column.Width;
                column.Width = ZConstants.WidthToCollapse;
            }
        }

        internal void SaveDMLSettings()
        {
            string orderOption = string.Empty, hiddenOption = string.Empty, collapsedOption = string.Empty, widthOption = string.Empty;
            foreach (ZDataGridViewColumn column in dmlGridView.GetColumnsOrderedByDisplayIndex())
            {
                ZParameter parameter = column.Tag as ZParameter;

                if (parameter == null) continue;

                orderOption += column.Name + ",";
                widthOption += (parameter.IsCollapsed) ? parameter.WidthToExpand + "," : column.Width + ",";
                hiddenOption += parameter.IsHidden + ",";
                collapsedOption += parameter.IsCollapsed + ",";
            }

            Options.TheOptions.dmlColumnsOrder = orderOption.Trim(',');
            Options.TheOptions.dmlColumnsWidth = widthOption.Trim(',');
            Options.TheOptions.dmlColumnsIsHidden = hiddenOption.Trim(',');
            Options.TheOptions.dmlColumnsIsCollapsed = collapsedOption.Trim(',');
        }

        internal void LoadDMLSettings()
        {
            if (Options.TheOptions.dmlColumnsOrder == null ||
                Options.TheOptions.dmlColumnsWidth == null ||
                Options.TheOptions.dmlColumnsIsHidden == null ||
                Options.TheOptions.dmlColumnsIsCollapsed == null) return;

            string[] orderArray = Options.TheOptions.dmlColumnsOrder.Split(',');
            string[] widthArray = Options.TheOptions.dmlColumnsWidth.Split(',');
            string[] hiddenArray = Options.TheOptions.dmlColumnsIsHidden.Split(',');
            string[] collapsedArray = Options.TheOptions.dmlColumnsIsCollapsed.Split(',');

            if (orderArray.Length != widthArray.Length || orderArray.Length != hiddenArray.Length || orderArray.Length != collapsedArray.Length) return;

            int order = 0;
            for (int i = 0; i < orderArray.Length; ++i)
            {
                ZParameter parameter = this.library.Parameters.GetByID(orderArray[i]);
                if (parameter == null) continue;

                if (parameter.Order != order) 
                {
                    ShiftParametersOrder(this.library.Parameters, order, parameter.Order);
                    parameter.Order = order;
                    ++order;
                }
                else ++order;

                bool collapsed;
                if (bool.TryParse(collapsedArray[i], out collapsed)) parameter.IsCollapsed = collapsed;

                int width;
                if (int.TryParse(widthArray[i], out width))
                {
                    parameter.WidthToExpand = width;
                    parameter.Width = (collapsed) ? ZConstants.WidthToCollapse : width;
                }

                bool hidden;
                if (bool.TryParse(hiddenArray[i], out hidden)) parameter.IsHidden = hidden;
            }
        }

        private static void ShiftParametersOrder(ZList<ZParameter> parameters, int orderNew, int orderOld)
        {
            foreach (var parameter in parameters)
            {
                if (parameter.Order >= orderNew && parameter.Order < orderOld) ++parameter.Order;
            }
        }

        internal DataGridViewCell FindNext(string query, int startRow, int startColumn, bool findInVisibleOnly)
        {
            dmlGridView.ClearSelection();

            int rIndex = startRow, cIndex = startColumn;
            for(; rIndex < dmlGridView.Rows.Count; ++rIndex, cIndex = 0)
            {
                if (findInVisibleOnly && !dmlGridView.Rows[rIndex].Visible) continue;

                for (; cIndex < dmlGridView.Columns.Count; ++cIndex)
                {
                    if (findInVisibleOnly && (!dmlGridView.Columns[cIndex].Visible || !dmlGridView.Rows[rIndex].Cells[cIndex].Visible)) continue;

                    if (dmlGridView.Rows[rIndex].Cells[cIndex].Value != null && dmlGridView.Rows[rIndex].Cells[cIndex].Value.ToString().ToLower().Contains(query.ToLower())) return dmlGridView.Rows[rIndex].Cells[cIndex];
                }
            }

            rIndex = 0;
            cIndex = 0;

            for (; rIndex <= startRow && rIndex < dmlGridView.Rows.Count; ++rIndex, cIndex = 0)
            {
                if (findInVisibleOnly && !dmlGridView.Rows[rIndex].Visible) continue;

                for (; cIndex < dmlGridView.Columns.Count; ++cIndex)
                {
                    if (findInVisibleOnly && (!dmlGridView.Columns[cIndex].Visible || !dmlGridView.Rows[rIndex].Cells[cIndex].Visible)) continue;

                    if (dmlGridView.Rows[rIndex].Cells[cIndex].Value != null && dmlGridView.Rows[rIndex].Cells[cIndex].Value.ToString().ToLower().Contains(query.ToLower())) return dmlGridView.Rows[rIndex].Cells[cIndex];
                }
            }

            return null;
        }

        internal List<DataGridViewCell> FindAll(string query, bool findInVisibleOnly)
        {
            dmlGridView.ClearSelection();

            List<DataGridViewCell> cells = new List<DataGridViewCell>();

            for (int rIndex = 0; rIndex < dmlGridView.Rows.Count; ++rIndex)
            {
                if (findInVisibleOnly && !dmlGridView.Rows[rIndex].Visible) continue;

                for (int cIndex = 0; cIndex < dmlGridView.Columns.Count; ++cIndex)
                {
                    if (findInVisibleOnly && (!dmlGridView.Columns[cIndex].Visible || !dmlGridView.Rows[rIndex].Cells[cIndex].Visible)) continue;

                    if (dmlGridView.Rows[rIndex].Cells[cIndex].Value != null && dmlGridView.Rows[rIndex].Cells[cIndex].Value.ToString().ToLower().Contains(query.ToLower())) cells.Add(dmlGridView.Rows[rIndex].Cells[cIndex]);
                }
            }

            return cells;
        }

        void RecalculateColumnHeaderName()
        {
            int index = 0;

            foreach (ZDataGridViewColumn column in dmlGridView.GetColumnsOrderedByDisplayIndex())
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null || parameter.IsPrivate || parameter.IsHidden) continue;
                column.HeaderName = column.GetHeaderNameByOrder(index++);
            }
        }

        void RecalculateRowHeaderName()
        {
            int index = 1;

            foreach (ZDataGridViewRow row in dmlGridView.Rows)
            {
                if (!row.Visible) continue;
                row.HeaderCell.Value = index.ToString();
                ++index;
            }
        }

        private void dmlGridView_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            if (e.StateChanged == DataGridViewElementStates.Selected)
            {
                ZPlannerManager.MainMenu.SetSendToStackupButtonEnabled(!ZPlannerManager.ProjectIsEmpty && !ZPlannerManager.Project.StackupIsEmpty && ZPlannerManager.StackupPanel != null && dmlGridView.SelectedRows.Count == 1);
            }
        }

        internal void SendToStackup(ZMaterial material, List<ZLayer> layers)
        {
            if (material == null)
            {
                ZPlannerManager.MainMenu.SetSendToStackupButtonEnabled(false);
                return;
            }

            if (ZPlannerManager.ProjectIsEmpty || ZPlannerManager.Project.StackupIsEmpty || ZPlannerManager.StackupPanel == null) return;

            //ZPlannerManager.Dml.CopyMaterialForStackup(material);

            ZLayerType? materialType = material.GetMaterialType();

            if (layers == null)
            {
                int index = (ZPlannerManager.IsCoreLocked && ZPlannerManager.StackupPanel != null) ? ZPlannerManager.StackupPanel.GetCoreLockedLayerOrder(ZPlannerManager.Project.Stackup.Layers.Count - 1) : ZPlannerManager.Project.Stackup.Layers.Count - 1;
                string layerID = ZPlannerManager.Project.Stackup.AddLayer(index + 1, (ZLayerType)materialType, ZPlannerManager.IsCoreLocked);
                ZLayer layer = ZPlannerManager.Project.Stackup.Layers.GetByID(layerID);
                if (layer == null) return;
                layers = new List<ZLayer>();
                layers.Add(layer);
            }

            ZPlannerManager.Project.Stackup.AssignMaterialToStackup(material, layers);
        }

        internal ZMaterial GetSelectedMaterial()
        {
            if (dmlGridView.SelectedRows.Count != 1) return null;
            return dmlGridView.SelectedRows[0].Tag as ZMaterial;
        }

        internal List<ZParameter> GetColumnParametersToEdit()
        {
            List<ZParameter> parameters = new List<ZParameter>();
            foreach (DataGridViewColumn column in dmlGridView.Columns)
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter != null && !parameter.IsPrivate && !parameter.IsReadOnly && !parameter.IsHidden /*&& (ZStringConstants.BulkEditIDs.Contains(parameter.ID) || parameter.ID.StartsWith(ZStringConstants.DkIDsPrefix))*/) parameters.Add(parameter);
            }

            return parameters;
        }

        internal List<ZMaterialParameter> GetMaterialParametersByValue(string id, string value)
        {
            List<ZMaterialParameter> materialParameters = new List<ZMaterialParameter>();
            ZDataGridViewColumn column = dmlGridView.Columns[id] as ZDataGridViewColumn;
            if (column != null)
            {
                foreach (DataGridViewRow row in dmlGridView.Rows)
                {
                    string formatedValue = column.GetFormatedValue(row);
                    if (row.Cells[id] != null && !row.Cells[id].ReadOnly && formatedValue == value)
                    {
                        ZMaterialParameter materialParameter = row.Cells[id].Tag as ZMaterialParameter;
                        if (materialParameter != null) materialParameters.Add(materialParameter);
                    }
                }
            }

            return materialParameters;
        }

        internal List<ZMaterialParameter> GetMaterialParametersToEdit(string id)
        {
            List<ZMaterialParameter> materialParameters = new List<ZMaterialParameter>();
            ZDataGridViewColumn column = dmlGridView.Columns[id] as ZDataGridViewColumn;
            if (column != null)
            {
                foreach (DataGridViewRow row in dmlGridView.Rows)
                {
                    string formatedValue = column.GetFormatedValue(row);
                    if (row.Cells[id] != null && !row.Cells[id].ReadOnly)
                    {
                        ZMaterialParameter materialParameter = row.Cells[id].Tag as ZMaterialParameter;
                        if (materialParameter != null) materialParameters.Add(materialParameter);
                    }
                }
            }

            return materialParameters;
        }

        internal string GetCurrentFilter()
        {
            return dmlGridView.FilterString;
        }

        internal string GetCurrentSort()
        {
            return dmlGridView.SortString;
        }

        internal string GetCurrentFilterName()
        {
            return ZPlannerManager.MainMenu.GetCurrentCustomFilterName();
        }

        internal void SetCurrentFilterAndSort(string filterString, string sortString)
        {
            /*foreach(DataGridViewColumn column in dmlGridView.Columns)
            {
                ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (headerCell != null && !headerCell.FilterMenuIsNotInitialized)
                {
                    headerCell.FilterMenu.ClearFilter();
                    headerCell.FilterMenu.ClearSorting();
                }
            }*/

            isIgnoreFilterChanged = true;

            dmlGridView.ClearFilter();
            dmlGridView.ClearSort();

            Dictionary<string, string> filterDictionary = new Dictionary<string,string>();
            Regex filtersPattern = new Regex(@"([(]?(?<filter>.+?)[)]? AND )+?", RegexOptions.Compiled | RegexOptions.Singleline);
            Regex columnPattern = new Regex(@"^[\[](?<column>.+?)[\]]\s(?<other>.+)$", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match filterMatch in filtersPattern.Matches(filterString.Trim() + " AND "))
            {
                if (!filterMatch.Success) continue;
                string filter = filterMatch.Groups["filter"].Value;
                Match columnMatch = columnPattern.Match(filter);
                if (!columnMatch.Success) continue;
                string columnName = columnMatch.Groups["column"].Value;
                string otherString = columnMatch.Groups["other"].Value;

                if (string.IsNullOrEmpty(columnName)) continue;
                if (filterDictionary.ContainsKey(columnName)) filterDictionary[columnName] += " AND [{0}] " + otherString;
                else filterDictionary.Add(columnName, "[{0}] " + otherString);
            }

            foreach (string columnName in filterDictionary.Keys)
            {
                DataGridViewColumn column = dmlGridView.Columns[columnName];
                if (column == null) continue;
                ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (headerCell == null) continue;
                ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
                if (filterMenu == null) continue;

                filterMenu.InitializeComponent();
                filterMenu.SetSetupFilterMenuItem(filterDictionary[columnName]);
            }

            Regex sortPattern = new Regex(@"[\[](?<column>\S+?)[\]]\s(?<direction>ASC|DESC)[,]?", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match sortMatch in sortPattern.Matches(sortString.Trim()))
            {
                if (!sortMatch.Success) continue;
                string columnName = sortMatch.Groups["column"].Value;
                string sortDirection = sortMatch.Groups["direction"].Value;

                if (string.IsNullOrEmpty(columnName)) continue;

                DataGridViewColumn column = dmlGridView.Columns[columnName];
                if (column == null) continue;
                ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (headerCell == null) continue;
                ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
                if (filterMenu == null) continue;
                ZDataGridViewSortType sortType = ZDataGridViewSortType.None;
                if (sortDirection.ToUpper() == "ASC") sortType = ZDataGridViewSortType.ASC;
                else if (sortDirection.ToUpper() == "DESC") sortType = ZDataGridViewSortType.DESC;
                filterMenu.SetSorting(sortType);
            }

            isIgnoreFilterChanged = false;

            dmlGridView.LoadFilter(filterString, sortString);

            /*dmlGridView.OnFilterStringChanged();
            dmlGridView.OnFiltered();
            dmlGridView.OnSortStringChanged();
            dmlGridView.OnSorted();*/

            foreach (string columnName in filterDictionary.Keys)
            {
                DataGridViewColumn column = dmlGridView.Columns[columnName];
                if (column == null) continue;
                ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                if (headerCell == null) continue;
                ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
                if (filterMenu == null) continue;

                dmlGridView.SetFilterButtons(columnName, filterDictionary[columnName]);
            }

            /*
            if (dmlGridView.Columns.Contains(e.Column))
            {

                dmlGridView.filterOrder.Remove(e.Column.Name);
                if (e.FilterMenu.ActiveFilterType != ZDataGridViewFilterType.None)
                    this.filterOrder.Add(e.Column.Name);

                this.FilterString = this.CreateFilterString();
            }
            */
        }

        private ZDataGridViewFilterMenu GetGridHeaderCellFilterMenu(string columnName)
        {
            DataGridViewColumn column = dmlGridView.Columns[columnName];
            if (column == null) return null;

            return (column.HeaderCell as ZDataGridViewColumnHeaderCell).FilterMenu;
        }

        public void AddInFilter(string columnName, string filterValue)
        {
            RemoveFilter(columnName);

            ZDataGridViewFilterMenu filterMenu = GetGridHeaderCellFilterMenu(columnName);
            if (filterMenu != null) filterMenu.FilterString = "[{0}] IN (" + filterValue + ")";

            if (string.IsNullOrEmpty(dmlGridView.FilterString)) 
                dmlGridView.FilterString = "([" + columnName + "] IN (" + filterValue + "))";
            else
                dmlGridView.FilterString += " AND ([" + columnName + "] IN (" + filterValue + "))";
        }

        public void AddLikeFilter(string columnName, string filterValue)
        {
            RemoveFilter(columnName);

            ZDataGridViewFilterMenu filterMenu = GetGridHeaderCellFilterMenu(columnName);
            if (filterMenu != null) filterMenu.FilterString = "[{0}] LIKE %" + filterValue + "%";

            if (string.IsNullOrEmpty(dmlGridView.FilterString))
                dmlGridView.FilterString = "([" + columnName + "] LIKE %" + filterValue + "%)";
            else
                dmlGridView.FilterString += " AND ([" + columnName + "] LIKE %" + filterValue + "%)";
        }

        public void RemoveFilter(string columnName)
        {
            ZDataGridViewFilterMenu filterMenu = GetGridHeaderCellFilterMenu(columnName);
            if (filterMenu != null) filterMenu.FilterString = null;

            string newFilterString = string.Empty;
            Regex filtersPattern = new Regex(@"([(]?(?<filter>.+?)[)]? AND )+?", RegexOptions.Compiled | RegexOptions.Singleline);
            foreach (Match filterMatch in filtersPattern.Matches(dmlGridView.FilterString.Trim() + " AND "))
            {
                if (!filterMatch.Success) continue;
                string filter = filterMatch.Groups["filter"].Value;

                if (filter.Contains("[" + columnName + "]")) continue;

                if (!string.IsNullOrEmpty(newFilterString)) newFilterString += " AND ";
                newFilterString += "(" + filter + ")";
            }

            dmlGridView.FilterString = (string.IsNullOrEmpty(newFilterString)) ? null : newFilterString;
        }

        public void LockLibrary(bool isLocked)
        {
            foreach (DataGridViewRow row in dmlGridView.Rows)
            {
                foreach (DataGridViewCell cell in row.Cells)
                {
                    if (isLocked) cell.ReadOnly = true;
                    else
                    {
                        ZMaterialParameter materialParameter = cell.Tag as ZMaterialParameter;
                        if (materialParameter != null && materialParameter.Material != null)
                        {
                            cell.ReadOnly = materialParameter.Material.IsReadOnly || !materialParameter.Material.IsCustom;
                        }
                    }
                }                
            }

            ZPlannerManager.MainMenu.LockLibrary(isLocked);
        }

        internal void SetMaterialTypeFilter(bool isCoreShow, bool isPrepregShow, bool isSolderMaskShow)
        {
            DataGridViewColumn column = dmlGridView.Columns[ZStringConstants.DMLParameterIDType];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = isCoreShow && isPrepregShow && isSolderMaskShow;            

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    if (tripleNode.Text == ZStringConstants.LayerTypeCore) { tripleNode.Checked = isCoreShow; continue; }
                    if (tripleNode.Text == ZStringConstants.LayerTypePrepreg) { tripleNode.Checked = isPrepregShow; continue; }
                    if (tripleNode.Text == ZStringConstants.LayerTypeSolderMask) { tripleNode.Checked = isSolderMaskShow; continue; }
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal void SetLibraryCategoryFilter(bool isZzeroLibraryShow, bool isCorporateLibraryShow, bool isLocalLibraryShow)
        {
            DataGridViewColumn column = dmlGridView.Columns[ZStringConstants.DMLParameterIDCategory];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = isZzeroLibraryShow && isCorporateLibraryShow && isLocalLibraryShow;

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    if (tripleNode.Text == ZStringConstants.MaterialCategoryZZero) { tripleNode.Checked = isZzeroLibraryShow; continue; }
                    if (tripleNode.Text == ZStringConstants.MaterialCategoryCorporate) { tripleNode.Checked = isCorporateLibraryShow; continue; }
                    if (tripleNode.Text == ZStringConstants.MaterialCategoryLocal) { tripleNode.Checked = isLocalLibraryShow; continue; }
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal void SetQualifiedMaterialsFilter(bool isQualifiedMaterialsShow)
        {
            SetBooleanFilter(ZStringConstants.DMLParameterIDQualified, isQualifiedMaterialsShow);
        }

        internal void SetHiddenMaterialsFilter(bool isHiddenMaterialsHide)
        {
            SetInversBooleanFilter(ZStringConstants.DMLParameterIDHide, isHiddenMaterialsHide);
        }

        internal void SetHalogenFreeFilter(bool isHalogenFreeShow)
        {
            SetBooleanFilter(ZStringConstants.DMLParameterIDHalogenFree, isHalogenFreeShow);
        }

        internal void Set2PlyMaterialsFilter(bool is2PlyMaterialsShow)
        {
            SetContainCharactersFilter(ZStringConstants.DMLParameterIDNormalizedConstruction, is2PlyMaterialsShow, new string[] { "x", "/" });
        }

        internal void SetContainCharactersFilter(string columnName, bool isContains, string[] characters)
        {
            DataGridViewColumn column = dmlGridView.Columns[columnName];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = !isContains;

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    if (isContains)
                    {
                        foreach (string character in characters)
                        {
                            if (tripleNode.Text.Contains(character)) { tripleNode.Checked = true; break; }
                            else { tripleNode.Checked = false; }
                        }
                    
                        continue;
                    }
                    else { tripleNode.Checked = true; continue; }
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal void SetBooleanFilter(string columnName, bool isYesValueShow)
        {
            DataGridViewColumn column = dmlGridView.Columns[columnName];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = !isYesValueShow;

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    if (tripleNode.Text == "Yes") { tripleNode.Checked = true; continue; }
                    if (tripleNode.Text == "No") { tripleNode.Checked = !isYesValueShow; continue; }
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal void SetInversBooleanFilter(string columnName, bool isYesValueHide)
        {
            DataGridViewColumn column = dmlGridView.Columns[columnName];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = !isYesValueHide;

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    if (tripleNode.Text == "Yes") { tripleNode.Checked = !isYesValueHide; ; continue; }
                    if (tripleNode.Text == "No") { tripleNode.Checked = true; continue; }
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal void SetGlassStyleFilter(string value, bool isClearFilter)
        {
            SetListFilter(ZStringConstants.DMLParameterIDConstruction, value, isClearFilter);
        }

        internal void SetSlashSheetFilter(string value, bool isClearFilter)
        {
            SetListFilter(ZStringConstants.DMLParameterIDPrimaryIPCSlashSheet, value, isClearFilter);
        }

        internal void SetListFilter(string columnName, string value, bool isClearFilter)
        {
            DataGridViewColumn column = dmlGridView.Columns[columnName];
            if (column == null) return;
            ZDataGridViewColumnHeaderCell headerCell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
            if (headerCell == null) return;
            ZDataGridViewFilterMenu filterMenu = headerCell.FilterMenu;
            if (filterMenu == null) return;

            filterMenu.InitializeComponent();
            dmlGridView.PrepareFilterWithoutShowing(filterMenu, column.Name);
            filterMenu.ClearFilter();

            bool allChecked = isClearFilter;

            foreach (TreeNode node in filterMenu.CheckList.Nodes)
            {
                TripleTreeNode tripleNode = node as TripleTreeNode;
                if (tripleNode != null)
                {
                    if (filterMenu.AllsNode != null && tripleNode == filterMenu.AllsNode) { filterMenu.AllsNode.Checked = allChecked; continue; }
                    tripleNode.Checked = isClearFilter || tripleNode.Text.Contains(value);
                    continue;
                }

                node.Checked = false;
            }

            filterMenu.okButton_Click(null, new EventArgs());
        }

        internal List<string> GetListOfGlassStyles()
        {
            List<string> values = new List<string>();
            Regex glassPattern = new Regex(@"(?<glass>[0123456789]{3,})", RegexOptions.Compiled | RegexOptions.Singleline);

            foreach (string valueString in ZDataGridViewFilterMenu.GetVisibleValuesForFilter(dmlGridView, ZStringConstants.DMLParameterIDConstruction, true))
            {
                foreach (Match glassMatch in glassPattern.Matches(valueString))
                {
                    if (!glassMatch.Success) continue;
                    string value = glassMatch.Groups["glass"].Value;
                    if (string.IsNullOrEmpty(value)) continue;
                    values.Add(value);
                }
            }

            return values;
        }

        internal List<string> GetListOfSlashSheet()
        {
            List<string> values = new List<string>();

            foreach (string valueString in ZDataGridViewFilterMenu.GetVisibleValuesForFilter(dmlGridView, ZStringConstants.DMLParameterIDPrimaryIPCSlashSheet, true))
            {
                foreach (string value in valueString.Split('/'))
                {
                    if (string.IsNullOrWhiteSpace(value)) continue;                    
                    values.Add("/" + value.Trim());
                }
            }

            return new List<string>(values.Distinct());
        }

        internal void EndGridEdit()
        {
            dmlGridView.CommitEdit(DataGridViewDataErrorContexts.Commit);
            dmlGridView.EndEdit();
            
            /*dmlGridView.CurrentCell = null;*/
        }

        internal void SelectFirstMaterial()
        {
            int index = dmlGridView.Rows.GetFirstRow(DataGridViewElementStates.Visible, DataGridViewElementStates.None);
            if (index >= 0 && index < dmlGridView.Rows.Count)
            {
                dmlGridView.CurrentCell = null;
                dmlGridView.FirstDisplayedScrollingRowIndex = index;
            }
            dmlGridView.ClearSelection();
        }

        internal ZMaterial GetCurrentMaterial()
        {
            ZMaterial material = null;
            DataGridViewRow row = dmlGridView.CurrentRow;
            if (row != null && row.Selected)
            {
                material = row.Tag as ZMaterial;
                if (material != null && material.GetMaterialCategory() == ZLibraryCategory.Local) return material;
            }

            return null;
        }

        void dmlGridView_SelectionChanged(object sender, EventArgs e)
        {
            ZPlannerManager.MainMenu.UpdateDeleteMaterial();
        }

        internal void DeleteSelectedMaterial()
        {
            ZMaterial material = GetCurrentMaterial();
            if (material != null)
                ZPlannerManager.Dml.RemoveMaterial(material);
        }

        internal void AddBlankMaterial(ZLayerType layerType)
        {
            ZPlannerManager.Dml.AddMaterial(layerType);
        }

        public void Copy()
        {
            if (!IsHidden)
            {
                ZMaterial material = null;
                DataGridViewRow row = dmlGridView.CurrentRow;
                if (row != null && row.Selected) material = row.Tag as ZMaterial;

                if (material != null) ZPlannerManager.Dml.CopyMaterial(material);
            }
        }

        public void Cut()
        {
            if (!IsHidden)
            {
                ZMaterial material = null;
                DataGridViewRow row = dmlGridView.CurrentRow;
                if (row != null && row.Selected) material = row.Tag as ZMaterial;
                if (material != null) ZPlannerManager.Dml.CutMaterial(material);
            }
        }

        public void Paste()
        {
            if (!IsHidden)
            {
                ZPlannerManager.Dml.PasteMaterial();
            }
        }
    }
}
