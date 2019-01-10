using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.FXInterface;
using System.Threading;
using System;
using ZZero.ZPlanner.Data.Properties;
using System.Drawing.Printing;
using System.IO;
using ZZero.ZPlanner.UI.Menu;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.UI.Dialogs;
using System.Collections;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerStackupPanel : DockContent
    {
        ZStackup stackup;
        ZPair pair;
        ZSingle single;
        Control curentEditingControl = null;
        bool isLastColumnResizing = false;
        List<string> rowSelection = new List<string>();

        internal ZPlannerStackupPanel(ZStackup stackup)
        {
            InitializeComponent();
            
            ZPlannerManager.ResumeCollectionChangedEvent();
            InitContextMenuSubitems();

            this.FormClosing += ZPlannerStackupPanel_FormClosing;
            this.Activated += ZPlannerStackupPanel_Activated;

            this.stackup = stackup;
            SetSingleComboBox();
            SetPairComboBox();
            ZPlannerManager.MainMenu.SetFrequency(stackup.Frequency);
            stackup.PropertyChanged += ZStackup_PropertyChanged;

            stackupGridView.CellEnter += dataGridView_CellEnter;
            stackupGridView.CellMouseClick += dataGridView_CellMouseClick;
            stackupGridView.CellContentClick += dataGridView_CellContentClick;
            stackupGridView.RowHeaderMouseClick += dataGridView_RowHeaderMouseClick;
            stackupGridView.ColumnHeaderMouseClick += dataGridView_ColumnHeaderMouseClick;

            stackupGridView.CellMouseDown += dataGridView_CellMouseDown;
            stackupGridView.CellMouseUp += dataGridView_CellMouseUp;
            stackupGridView.MouseMove += dataGridView_MouseMove;

            rowContextMenu.ItemClicked += rowContextMenu_ItemClicked;
            columnContextMenu.ItemClicked += columnContextMenu_ItemClicked;
            insertHiddenColumnBeforeToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertHiddenColumnAfterToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertExistingColumnBeforeToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;
            insertExistingColumnAfterToolStripMenuItem.DropDownItemClicked += columnContextMenu_ItemClicked;

            viaspanContextMenu.ItemClicked += viaspanContextMenu_ItemClicked;

            //stackupGridView.ColumnRemoved
            //stackupGridView.ColumnAdded
            //stackupGridView.RowsAdded
            //stackupGridView.RowsRemoved
            //stackupGridView.VisibleChanged
            
            stackupGridView.ColumnDisplayIndexChanged += dataGridView_ColumnDisplayIndexChanged;
            stackupGridView.ColumnWidthChanged += dataGridView_ColumnWidthChanged;
            stackupGridView.ColumnHeaderMouseDoubleClick += dataGridView_ColumnHeaderMouseDoubleClick;

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;

            ZPlannerManager.PropertyChanged += Program_PropertyChanged;
        }

        private void InitContextMenuSubitems()
        {
            foreach (string type in ZStringConstants.LayerType)
            {
                ToolStripMenuItem beforeToolStripMenuItem = new ToolStripMenuItem(type);
                beforeToolStripMenuItem.Name = type;
                addRowBeforeToolStripMenuItem.DropDownItems.Add(beforeToolStripMenuItem);
                ToolStripMenuItem afterToolStripMenuItem = new ToolStripMenuItem(type);
                afterToolStripMenuItem.Name = type;
                addRowAfterToolStripMenuItem.DropDownItems.Add(afterToolStripMenuItem);
            }

            addRowBeforeToolStripMenuItem.DropDownItemClicked += rowContextMenu_ItemClicked;
            addRowAfterToolStripMenuItem.DropDownItemClicked += rowContextMenu_ItemClicked;
            pasteMaterialToolStripMenuItem.DropDownItemClicked += rowContextMenu_ItemClicked;
        }

        internal void Clear()
        {
            this.FormClosing -= ZPlannerStackupPanel_FormClosing;
            if (stackup != null) stackup.PropertyChanged -= ZStackup_PropertyChanged;
            if (single != null) single.PropertyChanged -= ZSingle_PropertyChanged;
            if (pair != null) pair.PropertyChanged -= ZPair_PropertyChanged;

            stackupGridView.CellEnter -= dataGridView_CellEnter;
            stackupGridView.CellMouseClick -= dataGridView_CellMouseClick;
            stackupGridView.CellContentClick -= dataGridView_CellContentClick;
            stackupGridView.RowHeaderMouseClick -= dataGridView_RowHeaderMouseClick;
            stackupGridView.ColumnHeaderMouseClick -= dataGridView_ColumnHeaderMouseClick;

            stackupGridView.CellMouseDown -= dataGridView_CellMouseDown;
            stackupGridView.CellMouseUp -= dataGridView_CellMouseUp;
            stackupGridView.MouseMove -= dataGridView_MouseMove;

            rowContextMenu.ItemClicked -= rowContextMenu_ItemClicked;
            addRowBeforeToolStripMenuItem.DropDownItemClicked -= rowContextMenu_ItemClicked;
            addRowAfterToolStripMenuItem.DropDownItemClicked -= rowContextMenu_ItemClicked;
            columnContextMenu.ItemClicked -= columnContextMenu_ItemClicked;
            insertHiddenColumnBeforeToolStripMenuItem.DropDownItemClicked -= columnContextMenu_ItemClicked;
            insertHiddenColumnAfterToolStripMenuItem.DropDownItemClicked -= columnContextMenu_ItemClicked;
            insertExistingColumnBeforeToolStripMenuItem.DropDownItemClicked -= columnContextMenu_ItemClicked;
            insertExistingColumnAfterToolStripMenuItem.DropDownItemClicked -= columnContextMenu_ItemClicked;

            viaspanContextMenu.ItemClicked -= viaspanContextMenu_ItemClicked;

            stackupGridView.ColumnDisplayIndexChanged -= dataGridView_ColumnDisplayIndexChanged;
            stackupGridView.ColumnWidthChanged -= dataGridView_ColumnWidthChanged;
            stackupGridView.ColumnHeaderMouseDoubleClick -= dataGridView_ColumnHeaderMouseDoubleClick;

            ZPlannerManager.PropertyChanged -= Program_PropertyChanged;

            ClearStakup();
            ClearGrids();
        }

        internal void ClearGrids()
        {
            stackupGridView.Rows.Clear();
            stackupGridView.Columns.Clear();
        }

        internal void ClearStakup()
        {

            //stackupGridView.CurrentCellChanged -= dataGridView_CurrentCellChanged;

            stackup.Singles.CollectionChanged -= Singles_CollectionChanged;
            stackup.Pairs.CollectionChanged -= Pairs_CollectionChanged;
            stackup.Layers.CollectionChanged -= Layers_CollectionChanged;
            stackup.Spans.CollectionChanged -= Spans_CollectionChanged;
            stackup.Project.Parameters.CollectionChanged -= Parameters_CollectionChanged;

            ZPlannerManager.MainMenu.MenuButtonChecked -= MainMenu_MenuButtonChecked; 

            //stackupGridView.ColumnWidthChanged -= dataGridView_ElementsResize;
            //stackupGridView.ColumnHeadersHeightChanged -= dataGridView_ElementsResize;
            //stackupGridView.RowHeadersWidthChanged -= dataGridView_ElementsResize;
            //stackupGridView.RowHeightChanged -= dataGridView_ElementsResize;

            stackupGridView.CellValueChanged -= dataGridView_CellValueChanged;
            stackupGridView.EditingControlShowing -= dataGridView_EditingControlShowing;
            stackupGridView.CellEndEdit -= dataGridView_CellEndEdit;

            stackupGridView.CellDoubleClick -= dataGridView_CellDoubleClick;

            stackupGridView.RowStateChanged -= dataGridView_RowStateChanged;

            stackupGridView.CellStateChanged -= dataGridView_CellStateChanged;
            stackupGridView.CurrentCellChanged -= dataGridView_CurrentCellChanged;

            foreach (ZDataGridViewRow row in stackupGridView.Rows)
            {
                ZLayer layer = row.Tag as ZLayer;

                if (layer != null)
                {
                    layer.PropertyChanged -= (row.HeaderCell as ZDataGridViewRowHeaderCell).ZLayer_PropertyChanged;
                }

                foreach (DataGridViewCell cell in row.Cells)
                {
                    ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                    if (layerParameter != null)
                    {
                        layerParameter.PropertyChanged -= (cell as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                        layerParameter.PropertyChanged -= ZLayerParameter_PropertyChanged;
                    }
                }
            }

            foreach (DataGridViewColumn column in stackupGridView.Columns)
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter != null)
                {
                    if (parameter.Table == ZTableType.Viaspan)
                    {
                        ZDataGridViewViaSpanColumn spanColumn = column as ZDataGridViewViaSpanColumn;
                        if (spanColumn != null)
                        {
                            ZSpan span = stackup.Spans.Find((ZSpan x) => x.ID == spanColumn.Name);
                            if (span != null) span.PropertyChanged -= spanColumn.ZSpan_PropertyChanged;
                        }
                    }
                    else
                    {
                        parameter.PropertyChanged -= (column as ZDataGridViewColumn).ZParameter_PropertyChanged;
                        parameter.PropertyChanged -= ZParameter_PropertyChanged;
                    }
                }
            }

            foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
            {
                span.PropertyChanged -= ZSpan_PropertyChanged;
            }
        }

        void MainMenu_MenuButtonChecked(RibbonButton button, MenuButtonCheckedEventArgs e)
        {
            if (ZPlannerManager.IsIgnoreMenuButtonChecked) return;

            string which = e.Value;
            switch (which)
            {
                case "Roughness":
                case "EtchAdjust":
                case "PressedThickness":
                    ZPlannerManager.UpdateActiveStackup();
                    break;
            }
        }

        internal void ClearSingle()
        {
            if (single != null)
            {
                single.PropertyChanged -= ZSingle_PropertyChanged;

                foreach (var row in stackupGridView.Rows)
                {
                    ZDataGridViewRow zRow = row as ZDataGridViewRow;
                    if (zRow != null)
                    {
                        ZLayer singleLayer = single.Layers.Find((ZLayer x) => x.ID == zRow.Name);
                        if (singleLayer != null)
                        {
                            foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Single))
                            {
                                // TBD  (next section is for Single columns only)
                                ZLayerParameter layerParameter = singleLayer.LayerParameters.Find((ZLayerParameter x) => x.ID == column.Name);
                                if (layerParameter != null)
                                {
                                    layerParameter.PropertyChanged -= (zRow.Cells[column.Index] as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                                    layerParameter.PropertyChanged -= ZLayerParameter_PropertyChanged;
                                    zRow.Cells[column.Index].Tag = null;
                                    zRow.Cells[column.Index].Value = "";
                                }
                            }
                        }
                    }
                }

                single = null;
                stackupGridView.Invalidate();
            }
        }

        internal void ClearPair()
        {
            if (pair != null)
            {
                pair.PropertyChanged -= ZPair_PropertyChanged;

                foreach (var row in stackupGridView.Rows)
                {
                    ZDataGridViewRow zRow = row as ZDataGridViewRow;
                    if (zRow != null)
                    {
                        ZLayer pairLayer = pair.Layers.Find((ZLayer x) => x.ID == zRow.Name);
                        if (pairLayer != null)
                        {
                            foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Pair))
                            {
                                // TBD  (next section is for Pair columns only)
                                ZLayerParameter layerParameter = pairLayer.LayerParameters.Find((ZLayerParameter x) => x.ID == column.Name);
                                if (layerParameter != null)
                                {
                                    layerParameter.PropertyChanged -= (zRow.Cells[column.Index] as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                                    layerParameter.PropertyChanged -= ZLayerParameter_PropertyChanged;
                                    zRow.Cells[column.Index].Tag = null;
                                    zRow.Cells[column.Index].Value = "";
                                }
                            }
                        }
                    }
                }

                pair = null;
                stackupGridView.Invalidate();
            }
        }

        public string StackupID { get { return (stackup != null) ? stackup.ID : null; } }

        internal ZStackup Stackup { get { return stackup; } }

        internal ZSingle Single { get { return single; } }

        internal ZPair Pair { get { return pair; } }

        /// <summary>
        /// Loads form's data.
        /// </summary>
        internal void LoadData(ZStackup stackup, ZSingle single, ZPair pair)
        {
            if (this.stackup != stackup)
            {
                ClearStakup();
                this.stackup = stackup;
            }

            if (stackup == null)
            {
                ZPlannerManager.MessagePanel.AddMessage("Stackup data were not loaded. Stackup is undefined.");
                return;
            }

            if (single == null && stackup.Singles.Count > 0) single = stackup.Singles[0];

            this.single = single;

            if (pair == null && stackup.Pairs.Count > 0) pair = stackup.Pairs[0];

            this.pair = pair;

            LoadStackupSettings();

            ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);

            FillGridView();
            SetReadOnlyCells();
            FormatGridView(); 
            stackup.ActiveSingle = single;
            SetSingleComboBox();
            stackup.ActivePair = pair;
            SetPairComboBox();

            //stackupGridView.ColumnWidthChanged += dataGridView_ElementsResize;
            //stackupGridView.ColumnHeadersHeightChanged += dataGridView_ElementsResize;
            //stackupGridView.RowHeadersWidthChanged += dataGridView_ElementsResize;
            //stackupGridView.RowHeightChanged += dataGridView_ElementsResize;

            stackupGridView.CellValueChanged += dataGridView_CellValueChanged;
            UpdateReferences();

            stackupGridView.EditingControlShowing += dataGridView_EditingControlShowing;
            stackupGridView.CellEndEdit += dataGridView_CellEndEdit;

            stackupGridView.CellDoubleClick += dataGridView_CellDoubleClick;

            stackupGridView.RowStateChanged += dataGridView_RowStateChanged;

            stackupGridView.CellStateChanged += dataGridView_CellStateChanged;

            //stackupGridView.Resize += dataGridView_Resize;

            stackup.Singles.CollectionChanged += Singles_CollectionChanged;
            stackup.Pairs.CollectionChanged += Pairs_CollectionChanged;
            stackup.Layers.CollectionChanged += Layers_CollectionChanged;
            stackup.Spans.CollectionChanged += Spans_CollectionChanged;
            stackup.Project.Parameters.CollectionChanged += Parameters_CollectionChanged;

            stackupGridView.CurrentCellChanged += dataGridView_CurrentCellChanged;

            stackupGridView.CurrentCell = null;

            //stackupGridView.CurrentCellChanged += dataGridView_CurrentCellChanged;

            ZPlannerManager.MainMenu.MenuButtonChecked += MainMenu_MenuButtonChecked;

            SetTabName();
            ZPlannerManager.StatusMenu.SetStackupParameters(stackup);
        }

        void dataGridView_CellDoubleClick(object sender, DataGridViewCellEventArgs e)
        {
            if (e.ColumnIndex >= 0 && e.RowIndex >= 0)
            {
                DataGridViewCell cell = stackupGridView.Rows[e.RowIndex].Cells[e.ColumnIndex];
                if (cell != null && cell.ReadOnly && !cell.OwningColumn.ReadOnly)
                {
                    ZLayer layer = stackupGridView.Rows[e.RowIndex].Tag as ZLayer;
                    if (layer != null && layer.IsMaterialAssigned) TryToEditMaterialAssignedRow();
                }
            }
        }

        void dataGridView_CurrentCellChanged(object sender, EventArgs e)
        {
            ZPlannerManager.IsLayerSelected = stackupGridView.CurrentRow != null;
        }

        internal void LoadSingle(ZSingle single)
        {
            ClearSingle();

            if (stackup == null)
            {
                ZPlannerManager.MessagePanel.AddMessage("Single data were not loaded. Stackup is undefined.");
                return;
            }

            if (single == null && stackup.Singles.Count > 0) single = stackup.Singles[0];

            this.single = single;

            if (single != null)
            {
                AddSingle(single);
                UpdateReferences();
                SetReadOnlyCells();
                FormatGridView();
                stackup.ActiveSingle = single;
                SetSingleComboBox();
                stackupGridView.Invalidate();
            }
        }

        private void SetSingleComboBox()
        {
            ZPlannerManager.MainMenu.UpdateSingles(stackup.Singles, (this.single == null) ? "" : this.single.ID);
        }

        internal void LoadPair(ZPair pair)
        {
            ClearPair();

            if (stackup == null)
            {
                ZPlannerManager.MessagePanel.AddMessage("Pair data were not loaded. Stackup is undefined.");
                return;
            }

            if (pair == null && stackup.Pairs.Count > 0) pair = stackup.Pairs[0];

            this.pair = pair;

            if (pair != null)
            {
                AddPair(pair);
                UpdateReferences();
                SetReadOnlyCells();
                FormatGridView();
                stackup.ActivePair = pair;
                SetPairComboBox();
                stackupGridView.Invalidate();
            }
        }

        private void SetPairComboBox()
        {
            ZPlannerManager.MainMenu.UpdatePairs(stackup.Pairs, (this.pair == null) ? "" : this.pair.ID);
        }

        private void FillGridView()
        {
            stackupGridView.EditMode = DataGridViewEditMode.EditOnKeystrokeOrF2;

            RecalculateParametersOrder();

            // Add Columns.
            AddColumns(stackup.Project.Parameters, new ZList<ZSpan>(stackup, stackup.Spans.FindAll(x => x.IsEnable == true)));

            // Add Rows and Cells.
            AddRows(stackup.Layers);

            AddViaSpan(stackup.Spans);
            if (single != null) AddSingle(single);
            if (pair != null) AddPair(pair);

            // Show the first row header only.
            ShowVias(ZPlannerManager.IsViasVisible);
            ShowSingles(ZPlannerManager.IsSinglesVisible);
            ShowPairs(ZPlannerManager.IsPairsVisible);

            //RecalculateColumnHeaderName();

            ResizeColumns(stackupGridView);
        }

        private void AddColumns(ZList<ZParameter> parameters, ZList<ZSpan> spans)
        {
            // Add Columns.
            foreach (var parameter in parameters.OrderBy(X => X.Order))
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
                        column = new ZDataGridViewCheckBoxColumn();
                        break;
                    case ZValueType.Select:
                        string[] availableValues;
                        if (parameter.List != null && parameter.List.GetValues() != null) availableValues = parameter.List.GetValues().ToArray();
                        else if (parameter.ID == ZStringConstants.ParameterIDLayerType) availableValues = ZStringConstants.LayerType;
                        else if (parameter.ID == ZStringConstants.ParameterIDPlaneReference) availableValues = ZStringConstants.PlaneReferenceType;
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
                    case ZValueType.Span:
                        ZDataGridViewColumn firstSpanColumn = null;
                        if (spans.Count > 0)
                        {
                            AddSpanColumns(parameter, spans, ref firstSpanColumn);
                            if (firstSpanColumn != null) (firstSpanColumn.HeaderCell as IZDataGridViewCell).ColumnSpan = spans.Count;
                        }
                        else
                        {
                            AddEmptySpanColumn(parameter, ref firstSpanColumn);
                            if (firstSpanColumn != null) (firstSpanColumn.HeaderCell as IZDataGridViewCell).ColumnSpan = 1;
                        }
                        continue;
                    case ZValueType.Container:
                        ZDataGridViewColumn firstSubColumn = null;
                        AddSubColumns(parameter, ref firstSubColumn);
                        if (firstSubColumn != null) (firstSubColumn.HeaderCell as IZDataGridViewCell).ColumnSpan = parameter.SubParameters.Count;
                        continue;
                    default:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                }

                //column.DefaultCellStyle // TBD
                column.Resizable = DataGridViewTriState.True;

                column.Name = parameter.ID;
                column.TableType = parameter.Table;
                column.HeaderText = parameter.Title;
                column.ReadOnly = parameter.IsReadOnly;
                column.DisplayIndex = parameter.Order;
                column.Frozen = parameter.IsFrozen;
                column.ToolTipText = parameter.Description;
                column.Visible = !parameter.IsHidden && !parameter.IsPrivate;

                column.Tag = parameter; // Remove it in case it is not required.
                parameter.PropertyChanged += column.ZParameter_PropertyChanged;
                parameter.PropertyChanged += ZParameter_PropertyChanged;

                stackupGridView.Columns.Add(column);
            }
        }

        void ZParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "IsHidden":
                case "IsPrivate":
                    RecalculateColumnHeaderName();
                    ZPlannerManager.PropertiesPanel.UpdateProperties();
                    break;
            }
        }

        private void AddSubColumns(ZParameter parameter, ref ZDataGridViewColumn firstSubColumn)
        {
            // Add SubColumns.
            foreach (var subparameter in parameter.SubParameters)
            {
                ZDataGridViewColumn column;
                switch (subparameter.ValueType)
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
                        column = new ZDataGridViewCheckBoxColumn();
                        break;
                    case ZValueType.Select:
                        string[] availableValues;
                        if (subparameter.List != null && subparameter.List.GetValues() != null) availableValues = subparameter.List.GetValues().ToArray();
                        else if (subparameter.ID == ZStringConstants.ParameterIDLayerType) availableValues = ZStringConstants.LayerType;
                        else if (parameter.ID == ZStringConstants.ParameterIDPlaneReference) availableValues = ZStringConstants.PlaneReferenceType;
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
                    case ZValueType.Span:
                        column = new ZDataGridViewViaSpanColumn();
                        break;
                    default:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                }

                //column.DefaultCellStyle // TBD
                column.Resizable = DataGridViewTriState.True;

                column.Name = subparameter.ID;
                column.TableType = parameter.Table;
                column.HeaderText = parameter.Title;// +"\n" + subparameter.Title;
                (column.HeaderCell as ZDataGridViewColumnHeaderCell).Title = parameter.Title;
                (column.HeaderCell as ZDataGridViewColumnHeaderCell).SubTitle = subparameter.Title;
                column.ReadOnly = subparameter.IsReadOnly;
                column.DisplayIndex = subparameter.Order;
                column.Frozen = subparameter.IsFrozen;
                column.ToolTipText = subparameter.Description;
                column.Visible = !subparameter.IsHidden && !subparameter.IsPrivate;

                column.Tag = subparameter; // Remove it in case it is not required.
                subparameter.PropertyChanged += column.ZParameter_PropertyChanged;
                subparameter.PropertyChanged += ZParameter_PropertyChanged;

                stackupGridView.Columns.Add(column);

                if (firstSubColumn == null) firstSubColumn = column;
            }
        }

        private void AddSpanColumns(ZParameter parameter, ZList<ZSpan> spans, ref ZDataGridViewColumn firstSpanColumn)
        {
            // Add SubColumns.
            int i = 0;
            foreach (var span in spans)
            {
                ZDataGridViewViaSpanColumn column = new ZDataGridViewViaSpanColumn();

                //column.DefaultCellStyle // TBD
                column.Resizable = DataGridViewTriState.True;

                column.Name = span.ID;
                column.TableType = parameter.Table; 
                column.DisplayIndex = parameter.Order + i++;
                column.HeaderText = parameter.Title + "\n" + span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                (column.HeaderCell as ZDataGridViewColumnHeaderCell).Title = parameter.Title;
                (column.HeaderCell as ZDataGridViewColumnHeaderCell).SubTitle = span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                column.ReadOnly = true;
                column.Frozen = parameter.IsFrozen;
                column.ToolTipText = "Hole Diameter: " + span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture) + " mils, Length: " + span.CalculateHoleLength().ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture) + " mils, and Aspect Ratio: " + span.AspectRatio.ToString("N" + Settings.Options.TheOptions.percentDigits, CultureInfo.InvariantCulture) + " : 1";
                int autoSizeWidth = parameter.Width;
                if (parameter.Width <= 0)
                {
                    //parameter.Width = 27;
                    autoSizeWidth = 27;
                }
                column.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
                //column.Width = parameter.Width;
                column.Width = autoSizeWidth;
                column.Tag = span; // Remove it in case it is not required.

                stackupGridView.Columns.Add(column);

                if (firstSpanColumn == null) firstSpanColumn = column;
            }
        }

        private void AddEmptySpanColumn(ZParameter parameter, ref ZDataGridViewColumn firstSpanColumn)
        {
            // Add SubColumns.
            
            ZDataGridViewViaSpanColumn column = new ZDataGridViewViaSpanColumn();

            column.Name = "EmptySpanColumn";
            column.TableType = ZTableType.Viaspan;
            column.Resizable = DataGridViewTriState.False;
            column.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
            column.Width = 1;

            stackupGridView.Columns.Add(column);

            if (firstSpanColumn == null) firstSpanColumn = column;
        }

        private void AddRows(ZList<ZLayer> layers)
        {
            foreach (var layer in layers)
            {
                ZDataGridViewRow row = new ZDataGridViewRow();
                //row.DefaultCellStyle // TBD
                row.Name = layer.ID;
                row.Resizable = DataGridViewTriState.True;

                row.Frozen = layer.IsFrozen;
                row.ReadOnly = layer.IsReadOnly;
                row.Visible = !layer.IsHidden;
                if (layer.Height > 0) row.Height = layer.Height;
                row.Tag = layer; // Remove it in case it is not required.

                row.HeaderCell = new ZDataGridViewRowHeaderCell();
                row.HeaderCell.Value = (layer.Order + 1).ToString();
                row.HeaderCell.ToolTipText = layer.Title;
                layer.PropertyChanged += (row.HeaderCell as ZDataGridViewRowHeaderCell).ZLayer_PropertyChanged;

                if (this.stackupGridView.Columns.Count > 0)
                {
                    foreach (ZDataGridViewColumn column in this.stackupGridView.Columns)
                    {
                        DataGridViewCell cell;
                        ZParameter parameter = column.Tag as ZParameter;

                        if (column.TableType == ZTableType.Viaspan) cell = new ZDataGridViewViaSpanCell();
                        else switch (parameter.ValueType)
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
                                cell = new ZDataGridViewCheckBoxCell();
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
                                else if (parameter.ID == ZStringConstants.ParameterIDPlaneReference) availableValues = ZStringConstants.PlaneReferenceType;
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

                        if (column.TableType == ZTableType.Stackup)
                        {
                            ZLayerParameter layerParameter = layer.LayerParameters.Find(X => X.ID == parameter.ID);

                            if (layerParameter != null)
                            {
                                cell.ToolTipText = layerParameter.Value;
                                cell.Value = layerParameter.Value;
                                cell.Tag = layerParameter; // Remove it in case it is not required.
                                //if (layerParameter.ID == ZStringConstants.ParameterIDLayerNumber) cell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                                layerParameter.PropertyChanged += (cell as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                                layerParameter.PropertyChanged += ZLayerParameter_PropertyChanged;
                            }
                            else
                            {
                                cell.Value = "";
                            }
                        }
                        else
                        {
                            cell.Value = "";
                        }

                        row.Cells.Add(cell);
                    }
                }

                this.stackupGridView.Rows.Add(row);
            }
        }

        private void AddViaSpan(ZList<ZSpan> spans)
        {
            foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Viaspan))
            {
                ZDataGridViewViaSpanColumn spanColumn = column as ZDataGridViewViaSpanColumn;
                if (spanColumn != null)
                {
                    ZSpan span = spans.Find(x => x.ID == spanColumn.Name);
                    if (span != null)
                    {
                        spanColumn.SetSpan(span);
                        span.PropertyChanged += spanColumn.ZSpan_PropertyChanged;
                    }
                }
            }

            foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
            {
                span.PropertyChanged += ZSpan_PropertyChanged;
            }
        }

        private void AddSingle(ZSingle single)
        {
            single.PropertyChanged += ZSingle_PropertyChanged;

            foreach (var row in stackupGridView.Rows)
            {
                ZDataGridViewRow zRow = row as ZDataGridViewRow;
                if (zRow != null)
                {
                    ZLayer singleLayer = single.Layers.Find((ZLayer x) => x.ID == zRow.Name);
                    if (singleLayer != null)
                    {
                        foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Single))
                        {
                            ZLayerParameter layerParameter = singleLayer.LayerParameters.Find((ZLayerParameter x) => x.ID == column.Name);
                            if (layerParameter != null)
                            {
                                zRow.Cells[column.Index].Value = layerParameter.Value;
                                zRow.Cells[column.Index].Tag = layerParameter;
                                layerParameter.PropertyChanged += (zRow.Cells[column.Index] as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                                layerParameter.PropertyChanged += ZLayerParameter_PropertyChanged;

                                if (layerParameter.ID == ZStringConstants.ParameterIDZo_TopReference ||
                                    layerParameter.ID == ZStringConstants.ParameterIDZo_BottomReference) zRow.Cells[column.Index].Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                            }
                        }
                    }
                }
            }
        }

        internal void RenameSingle(ZSingle singleToRename, Point location)
        {
            if (singleToRename == null) return;

            RenameSignalDialog dlg = new RenameSignalDialog();
            dlg.SetCurrentSignal(singleToRename);
            dlg.StartPosition = FormStartPosition.Manual;
            dlg.Location = location;
            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.OK)
            {
                ZPlannerManager.ProjectPanel.SelectSingle(singleToRename.ID);
                //ZPlannerManager.MainMenu.UpdateSingles(Stackup.Singles,);
                ZPlannerManager.PropertiesPanel.UpdateProperties();
            }
        }

        internal void DeleteSingle(ZSingle singleToDelete, Point location)
        {
            if (singleToDelete == null) return;

            DialogResult result = MessageBox.Show("Do you realy want to delete " + singleToDelete.Title + "?", "Confirmation", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                if (single == singleToDelete) single = null;
                Stackup.Singles.Remove(singleToDelete);

                bool ignoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();
                try
                {
                    if (Stackup.Singles.Count > 0) ZPlannerManager.ProjectPanel.SelectSingle(Stackup.Singles[0].ID);
                    else
                    {
                        ZPlannerManager.StackupPanel.ClearSingle();
                        ZPlannerManager.ProjectPanel.SelectStackup();
                    }
                }
                finally
                {
                    ZPlannerManager.Commands.ResumeCommandEvent(ignoreCommands);
                }
            }
        }

        private void AddPair(ZPair pair)
        {
            pair.PropertyChanged += ZPair_PropertyChanged;

            foreach (var row in stackupGridView.Rows)
            {
                ZDataGridViewRow zRow = row as ZDataGridViewRow;
                if (zRow != null)
                {
                    ZLayer pairLayer = pair.Layers.Find((ZLayer x) => x.ID == zRow.Name);
                    if (pairLayer != null)
                    {
                        foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Pair))
                        {
                            ZLayerParameter layerParameter = pairLayer.LayerParameters.Find((ZLayerParameter x) => x.ID == column.Name);
                            if (layerParameter != null)
                            {
                                zRow.Cells[column.Index].Value = layerParameter.Value;
                                zRow.Cells[column.Index].Tag = layerParameter;
                                layerParameter.PropertyChanged += (zRow.Cells[column.Index] as IZDataGridViewCell).ZLayerParameter_PropertyChanged;
                                layerParameter.PropertyChanged += ZLayerParameter_PropertyChanged;

                                if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_TopReference ||
                                    layerParameter.ID == ZStringConstants.ParameterIDZdiff_BottomReference) zRow.Cells[column.Index].Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                            }
                        }
                    }
                }
            }
        }

        internal void RenamePair(ZPair pairToRename, Point location)
        {
            if (pairToRename == null) return;

            RenameSignalDialog dlg = new RenameSignalDialog();
            dlg.SetCurrentSignal(pairToRename);
            dlg.StartPosition = FormStartPosition.Manual;
            dlg.Location = location;
            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.OK)
            {
                ZPlannerManager.ProjectPanel.SelectPair(pairToRename.ID);
                //ZPlannerManager.MainMenu.UpdatePairs(Stackup.Pairs,);
                ZPlannerManager.PropertiesPanel.UpdateProperties();
            }
        }

        internal void DeletePair(ZPair pairToDelete, Point location)
        {
            if (pairToDelete == null) return;

            DialogResult result = MessageBox.Show("Do you realy want to delete " + pairToDelete.Title + "?", "Confirmation", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes)
            {
                if (pair == pairToDelete) pair = null;
                Stackup.Pairs.Remove(pairToDelete);

                bool ignoreCommands = ZPlannerManager.Commands.SuspendCommandEvent();
                try
                {
                    if (Stackup.Pairs.Count > 0) ZPlannerManager.ProjectPanel.SelectPair(Stackup.Pairs[0].ID);
                    else
                    {
                        ZPlannerManager.StackupPanel.ClearPair();
                        ZPlannerManager.ProjectPanel.SelectStackup();
                    }
                }
                finally
                {
                    ZPlannerManager.Commands.ResumeCommandEvent(ignoreCommands);
                }
            }
        }

        private void ResizeColumns(DataGridView grid)
        {
            foreach (DataGridViewColumn column in grid.Columns)
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

            grid.AutoResizeColumnHeadersHeight();

        }

        internal void FormatGridView()
        {
            for(int i = 0; i < stackupGridView.RowCount; ++i)
            {
                FormatGridRow(i);
            }
        }

        internal void FormatGridRow(int index)
        {
            ZLayer layer = stackupGridView.Rows[index].Tag as ZLayer;
            if (layer != null)
            {
                ZLayerType? layerType = layer.GetLayerType();
                ColorGridsRow(index, ZColorConstants.GetLayerColor(layerType));
                if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed) ColorThicknessCells(index, ZColorConstants.PlaneLayerColor);
                if (ZPlannerManager.IsColorDisabledCells) ColorDisabledCells(index, ZColorConstants.DisabledCellColor);
                ColorCalculatedCells(index, ZColorConstants.CalculatedCellColor, new Font(stackupGridView.DefaultCellStyle.Font, FontStyle.Bold));
            }
        }

        private void ColorGridsRow(int rowIndex, Color color)
        {
            if (stackupGridView.Rows.Count > rowIndex)
            {
                stackupGridView.Rows[rowIndex].DefaultCellStyle.BackColor = color;
                foreach (DataGridViewCell cell in stackupGridView.Rows[rowIndex].Cells) cell.Style.BackColor = Color.Empty;
            }
        }

        private void ColorDisabledCells(int rowIndex, Color backColor)
        {
            foreach (DataGridViewCell cell in stackupGridView.Rows[rowIndex].Cells)
            {
                ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                if (layerParameter != null) 
                {
                    if (layerParameter.IsReadOnly(true)) cell.Style.BackColor = backColor;
                    else cell.Style.BackColor = Color.Empty;
                }
            }
        }

        private void ColorCalculatedCells(int rowIndex, Color foreColor, Font font)
        {
            foreach (DataGridViewCell cell in stackupGridView.Rows[rowIndex].Cells)
            {
                ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                if (layerParameter != null)
                {
                    if (layerParameter.IsReadOnly(false, true, true)) { cell.Style.ForeColor = foreColor; cell.Style.Font = font; }
                    else { cell.Style.ForeColor = Color.Empty; cell.Style.Font = null; }
                }
            }
        }

        private void ColorThicknessCells(int rowIndex, Color color)
        {
            foreach (DataGridViewCell cell in stackupGridView.Rows[rowIndex].Cells)
            {
                ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                if (layerParameter != null && layerParameter.Parameter != null && 
                    (layerParameter.Parameter.ID == ZStringConstants.ParameterIDThickness ||
                    layerParameter.Parameter.ID == ZStringConstants.ParameterIDZo_TraceWidth || 
                    layerParameter.Parameter.ID == ZStringConstants.ParameterIDZdiff_TraceWidth) &&  
                    !string.IsNullOrEmpty(layerParameter.Value))
                    cell.Style.BackColor = color;
                else cell.Style.BackColor = Color.Empty;
            }
        }

        internal void SetReadOnlyCells()
        {
            for(int index = 0; index < stackupGridView.Rows.Count; ++index)
            {
                //ZLayer layer = stackupGridView.Rows[index].Tag as ZLayer;
                //if (layer == null) continue;
                //ZLayerType? type = layer.GetLayerType();
                //if (type == null) continue;
                //ZLayerType layerType = (ZLayerType)type;

                foreach (DataGridViewCell cell in stackupGridView.Rows[index].Cells)
                {
                    if (cell == null) continue;
                    ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                    if(layerParameter == null) continue;
                    cell.ReadOnly = layerParameter.IsReadOnly();
                }
            }
        }

        internal void ShowVias(bool isVisible)
        {
            foreach (ZDataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Viaspan))
            {
                ZParameter parameter = column.Tag as ZParameter;
                column.Visible = isVisible && (parameter == null || (!parameter.IsHidden && !parameter.IsPrivate));
            }
            //viaspanGridView.Visible = isVisible;
            RecalculateColumnHeaderName();
        }

        internal void ShowSingles(bool isVisible)
        {
            foreach (ZDataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Single))
            {
                ZParameter parameter = column.Tag as ZParameter;
                column.Visible = isVisible && (parameter == null || (!parameter.IsHidden && !parameter.IsPrivate));
            }
            //singleGridView.Visible = isVisible;
            RecalculateColumnHeaderName();
        }

        internal void ShowPairs(bool isVisible)
        {
            foreach (ZDataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Pair))
            {
                ZParameter parameter = column.Tag as ZParameter;
                column.Visible = isVisible && (parameter == null || (!parameter.IsHidden && !parameter.IsPrivate));
            }
            //pairGridView.Visible = isVisible;
            RecalculateColumnHeaderName();
        }

        internal void ShowHeaders(bool isVisible)
        {
            stackupGridView.ColumnHeaderNamesVisible = isVisible;
            stackupGridView.RowHeadersVisible = isVisible;
            stackupGridView.Invalidate();
        }

        internal void ShowCenterLine(bool isVisible)
        {
            stackupGridView.Invalidate();
        }

        internal List<ZParameter> GetColumnParametersToEdit()
        {
            List<ZParameter> parameters = new List<ZParameter>();
            foreach(DataGridViewColumn column in stackupGridView.Columns)
            {
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter != null && !parameter.IsPrivate && !parameter.IsReadOnly && ZStringConstants.EditColumnIDs.Contains(parameter.ID)) parameters.Add(parameter);
            }

            return parameters;
        }

        void UpdateTotalLoss(ZLayer layer)
        {
            if (layer == null) return;
            string stringInsertionLoss = layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_InsertionLoss) ?? layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_InsertionLoss);
            string stringTraceLength = layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TraceLength) ?? layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceLength);
            double doubleInsertionLoss;
            double doubleTraceLength;
            if (double.TryParse(stringInsertionLoss, NumberStyles.Any, CultureInfo.InvariantCulture, out doubleInsertionLoss) && 
                double.TryParse(stringTraceLength, NumberStyles.Any, CultureInfo.InvariantCulture, out doubleTraceLength))
            {
                ZLayerParameter parameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_TotalLoss) ?? layer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TotalLoss);
                if (parameter == null) return;
                parameter.Value = (doubleInsertionLoss * doubleTraceLength).ToString(ZPlannerManager.GetFormatByParameter(parameter.Parameter), CultureInfo.InvariantCulture);
            }

            stackupGridView.Invalidate();
        }

        void UpdatePropagationDelay(ZLayer layer)
        {
            if (layer == null) return;
            string stringPropagationVelocity = layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_PropagationVelocity) ?? layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_PropagationVelocity);
            string stringTraceLength = layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TraceLength) ?? layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceLength);
            double doublePropagationVelocity;
            double doubleTraceLength;
            if (double.TryParse(stringPropagationVelocity, NumberStyles.Any, CultureInfo.InvariantCulture, out doublePropagationVelocity) && 
                double.TryParse(stringTraceLength, NumberStyles.Any, CultureInfo.InvariantCulture, out doubleTraceLength))
            {
                ZLayerParameter parameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_PropagationDelay) ?? layer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_PropagationDelay);
                if (parameter == null) return;
                parameter.Value = (doubleTraceLength / doublePropagationVelocity).ToString(ZPlannerManager.GetFormatByParameter(parameter.Parameter), CultureInfo.InvariantCulture);
            }

            stackupGridView.Invalidate();
        }

        void ZStackup_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "Title":
                    SetTabName();
                    break;
                case "Frequency":
                    ZPlannerManager.MainMenu.SetFrequency((sender as ZStackup).Frequency);
                    stackupGridView.Invalidate();
                    ZPlannerManager.UpdateActiveStackup();
                    break;
                case "ActivePair":
                    if (Stackup.ActivePair != null)
                    {
                        ZPlannerManager.MainMenu.UpdatePairs(Stackup.Pairs, Stackup.ActivePair.ID);
                        LoadPair(Stackup.ActivePair);
                    }
                    else
                    {
                        ZPlannerManager.MainMenu.UpdatePairs(Stackup.Pairs, null);
                        ClearPair();
                    }
                    ZPlannerManager.UpdateActiveStackup();
                    break;
                case "ActiveSingle":
                    if (Stackup.ActiveSingle != null)
                    {
                        ZPlannerManager.MainMenu.UpdateSingles(Stackup.Singles, Stackup.ActiveSingle.ID);
                        LoadSingle(Stackup.ActiveSingle);
                    }
                    else
                    {
                        ZPlannerManager.MainMenu.UpdateSingles(Stackup.Singles, null);
                        ClearSingle();
                    }
                    ZPlannerManager.UpdateActiveStackup();
                    break;
            }
        }

        void ZLayerParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            List<string> layerParametersToBeIgnored = new List<string>(new string[] { ZStringConstants.ParameterIDLayerType, ZStringConstants.ParameterIDLayerNumber, ZStringConstants.ParameterIDLayerName, ZStringConstants.ParameterIDCopperPercent, ZStringConstants.ParameterIDNarrowTop, ZStringConstants.ParameterIDCalcRoughTop, ZStringConstants.ParameterIDCalcRoughBottom, ZStringConstants.ParameterIDZo_InsertionLoss, ZStringConstants.ParameterIDZo_TotalLoss, ZStringConstants.ParameterIDZdiff_InsertionLoss, ZStringConstants.ParameterIDZdiff_TotalLoss, ZStringConstants.ParameterIDZo_TopReference, ZStringConstants.ParameterIDZo_BottomReference, ZStringConstants.ParameterIDZdiff_TopReference, ZStringConstants.ParameterIDZdiff_BottomReference });
            ZLayerParameter layerParameter = sender as ZLayerParameter;

            if (layerParameter == null) return;

            if (ZPlannerManager.StackupPanel != null && ZPlannerManager.IsAutoMirror && !layerParametersToBeIgnored.Contains(layerParameter.ID) && !layerParameter.IsReadOnly())
            {
                int lastIndex = ZPlannerManager.StackupPanel.Stackup.Layers.Count - 1;
                int currentIndex = ZPlannerManager.StackupPanel.Stackup.Layers.FindIndex(x => x.ID == layerParameter.Layer.ID);
                int mirrorIndex = lastIndex - currentIndex;
                if (currentIndex >= 0 && currentIndex <= lastIndex && mirrorIndex >= 0 && mirrorIndex <= lastIndex && currentIndex != mirrorIndex)
                {
                    ZStackup stackup = layerParameter.Layer.Owner as ZStackup;
                    if (stackup != null) stackup.Layers[mirrorIndex].SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
                    else
                    {
                        ZSingle single = layerParameter.Layer.Owner as ZSingle;
                        if (single != null) single.Layers[mirrorIndex].SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
                        else
                        {
                            ZPair pair = layerParameter.Layer.Owner as ZPair;
                            if (pair != null) pair.Layers[mirrorIndex].SetLayerParameterValue(layerParameter.ID, layerParameter.Value);
                        }
                    }
                }
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_TraceWidth || layerParameter.ID == ZStringConstants.ParameterIDZdiff_TraceSpacing)
            {
                double traceWidth;
                double traceSpacing;

                if (double.TryParse(layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceWidth), NumberStyles.Any, CultureInfo.InvariantCulture, out traceWidth) &&
                    double.TryParse(layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceSpacing), NumberStyles.Any, CultureInfo.InvariantCulture, out traceSpacing))
                    layerParameter.Layer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TracePitch, (traceWidth + traceSpacing).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_IsUsed && layerParameter.Layer.isMetal())
            {
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

                try
                {
                    ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                    foreach (var lParameter in layerParameter.Layer.LayerParameters)
                    {
                        if (layerParameter.ID == lParameter.ID || layerParameter.ID == ZStringConstants.ParameterIDZdiff_TopReference || layerParameter.ID == ZStringConstants.ParameterIDZdiff_BottomReference) continue;

                        if (layerParameter.Value.ToLower() == "yes" || layerParameter.Value.ToLower() == "true")
                        {
                            if (layerType != null && !lParameter.IsReadOnly(true)) lParameter.Value = lParameter.GetDefaulLayerParameterValue((ZLayerType)layerType);
                        }
                        else
                        {
                            lParameter.Value = string.Empty;
                        }
                    }
                }

                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZo_IsUsed && layerParameter.Layer.isMetal())
            {
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();

                try
                {
                    ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                    foreach (var lParameter in layerParameter.Layer.LayerParameters)
                    {
                        if (layerParameter.ID == lParameter.ID || layerParameter.ID == ZStringConstants.ParameterIDZo_TopReference || layerParameter.ID == ZStringConstants.ParameterIDZo_BottomReference) continue;

                        if (layerParameter.Value.ToLower() == "yes" || layerParameter.Value.ToLower() == "true")
                        {
                            if (layerType != null && !lParameter.IsReadOnly(true)) lParameter.Value = lParameter.GetDefaulLayerParameterValue((ZLayerType)layerType);
                        }
                        else
                        {
                            lParameter.Value = string.Empty;
                        }
                    }
                }

                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDWarpYarnCount)
            {
                double dvalue;

                if (double.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out dvalue) && dvalue != 0)
                    layerParameter.Layer.SetLayerParameterValue(ZStringConstants.ParameterIDWeavePitch, (1000 / dvalue).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
                stackupGridView.Invalidate();
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDFillYarnCount)
            {
                double dvalue;

                if (double.TryParse(layerParameter.Value, NumberStyles.Any, CultureInfo.InvariantCulture, out dvalue) && dvalue != 0)
                    layerParameter.Layer.SetLayerParameterValue(ZStringConstants.ParameterIDFillPitch, (1000 / dvalue).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
                stackupGridView.Invalidate();
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDThickness) ZPlannerManager.StatusMenu.SetStackupParameters(Stackup);

            if (layerParameter.ID == ZStringConstants.ParameterIDLayerType) SetReadOnlyCells();

            if (layerParameter.ID == ZStringConstants.ParameterIDThickness)
            {
                if (!ZPlannerManager.IsPressedThickness) layerParameter.Layer.SetLayerParameterValue(ZStringConstants.ParameterIDOriginThickness, layerParameter.Value);
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDLayerType ||
                layerParameter.ID == ZStringConstants.ParameterIDThickness ||
                layerParameter.ID == ZStringConstants.ParameterIDZo_TraceWidth ||
                layerParameter.ID == ZStringConstants.ParameterIDZdiff_TraceWidth)
            {
                FormatGridView();
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZo_InsertionLoss || layerParameter.ID == ZStringConstants.ParameterIDZdiff_InsertionLoss ||
                layerParameter.ID == ZStringConstants.ParameterIDZo_TraceLength || layerParameter.ID == ZStringConstants.ParameterIDZdiff_TraceLength)
            {

                UpdateTotalLoss(layerParameter.Layer);
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZo_PropagationVelocity || layerParameter.ID == ZStringConstants.ParameterIDZdiff_PropagationVelocity ||
                layerParameter.ID == ZStringConstants.ParameterIDZo_TraceLength || layerParameter.ID == ZStringConstants.ParameterIDZdiff_TraceLength)
            {
                UpdatePropagationDelay(layerParameter.Layer);
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDCopperThickness)
            {
                string thickness = null;
                ZLayerParameter thicknessLayerParameter = layerParameter.Layer.GetLayerParameter(ZStringConstants.ParameterIDThickness);

                if (thicknessLayerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value) && ZPlannerManager.TryToCalculateThicknesByCopperWeight(layerParameter.Value, out thickness) && !string.IsNullOrWhiteSpace(thickness))
                    thicknessLayerParameter.SetUnEditedValue(thickness);
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDCopperThickness || layerParameter.ID == ZStringConstants.ParameterIDCopperPercent)
            {
                if (ZPlannerManager.IsPressedThickness)
                {
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                            ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                            ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }

            //run FS
            switch (layerParameter.ID)
            {
                case ZStringConstants.ParameterIDLayerType:
                //case ZStringConstants.ParameterIDLayerName:
                case ZStringConstants.ParameterIDThickness:
                case ZStringConstants.ParameterIDZo_TraceWidth:
                case ZStringConstants.ParameterIDZdiff_TraceWidth:
                case ZStringConstants.ParameterIDZdiff_TraceSpacing:
                case ZStringConstants.ParameterIDZo_DielectricConstant:
                case ZStringConstants.ParameterIDZdiff_DielectricConstant:
                case ZStringConstants.ParameterIDZo_LossTangent:
                case ZStringConstants.ParameterIDZdiff_LossTangent:
                case ZStringConstants.ParameterIDZo_IsUsed:
                case ZStringConstants.ParameterIDZdiff_IsUsed:
                case ZStringConstants.ParameterIDZo_Frequency:
                case ZStringConstants.ParameterIDZdiff_Frequency:
                case ZStringConstants.ParameterIDBulkRes:
                case ZStringConstants.ParameterIDRoughTop:
                case ZStringConstants.ParameterIDRoughBottom:
                case ZStringConstants.ParameterIDEtchFactor:
                case ZStringConstants.ParameterIDNarrowTop:
                case ZStringConstants.ParameterIDPlaneReference:
                case ZStringConstants.ParameterIDCopperPercent:
                case ZStringConstants.ParameterIDZo_TopReference:
                case ZStringConstants.ParameterIDZo_BottomReference:
                case ZStringConstants.ParameterIDZdiff_TopReference:
                case ZStringConstants.ParameterIDZdiff_BottomReference:

                    if (!ZPlannerManager.IsIgnorePropertyChanged)
                    {
                        ZPlannerManager.UpdateActiveStackup();
                    }
                    break;
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDLayerName)
            {
                if (Visible)
                {
                    ZPlannerManager.DRCPanel.UpdateReport();
                }
            }

            if (layerParameter.ID == ZStringConstants.ParameterIDZo_Frequency || layerParameter.ID == ZStringConstants.ParameterIDZdiff_Frequency)
                stackupGridView.Invalidate();
            //if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_IsUsed || layerParameter.ID == ZStringConstants.ParameterIDZo_IsUsed) stackupGridView.Invalidate();

            ZPlannerManager.PropertiesPanel.UpdateProperties();
        }

        void ZSingle_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "Title":
                    SetSingleComboBox();
                    break;
            }
        }

        void ZPair_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "Title":
                    SetPairComboBox();
                    break;
            }
        }

        void dataGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.ColumnIndex < 0) return;
            DataGridViewCell cell = (sender as DataGridView).Rows[e.RowIndex].Cells[e.ColumnIndex];
            ZLayerParameter layerParameter = (sender as DataGridView).Rows[e.RowIndex].Cells[e.ColumnIndex].Tag as ZLayerParameter;

            if (layerParameter != null)
            {
                switch(layerParameter.ID)
                {
                    case ZStringConstants.ParameterIDZdiff_RoughTop:
                        ZPlannerProject.SetLayerParameterValue(layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID), ZStringConstants.ParameterIDRoughTop, (cell.Value != null) ? (cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString() : "");
                        break;
                    case ZStringConstants.ParameterIDZdiff_RoughBottom:
                        ZPlannerProject.SetLayerParameterValue(layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID), ZStringConstants.ParameterIDRoughBottom, (cell.Value != null) ? (cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString() : "");
                        break;
                    case ZStringConstants.ParameterIDZdiff_WeavePitch:
                        ZPlannerProject.SetLayerParameterValue(layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID), ZStringConstants.ParameterIDWeavePitch, (cell.Value != null) ? (cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString() : "");
                        break;
                    case ZStringConstants.ParameterIDZdiff_FillPitch:
                        ZPlannerProject.SetLayerParameterValue(layerParameter.Layer.Stackup.GetLayerOfStackup(layerParameter.Layer.ID), ZStringConstants.ParameterIDFillPitch, (cell.Value != null) ? (cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString() : "");
                        break;
                    case ZStringConstants.ParameterIDThickness:
                        layerParameter.SetEditedValue((cell.Value != null) ? (cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString() : "");
                        break;
                    case ZStringConstants.ParameterIDZo_TopReference:
                    case ZStringConstants.ParameterIDZo_BottomReference:
                    case ZStringConstants.ParameterIDZdiff_TopReference:
                    case ZStringConstants.ParameterIDZdiff_BottomReference:
                        ZLayer referenceLayer = cell.Value as ZLayer;
                        if (referenceLayer == null)
                        {
                            string sValue = cell.Value as string;
                            referenceLayer = Stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == sValue);
                        }
                        if (referenceLayer != null && layerParameter.Value != referenceLayer.ID) layerParameter.Value = referenceLayer.ID;
                        break;
                    default:
                        layerParameter.Value = (cell.Value != null) ? ((cell.Value is double) ? ((double)cell.Value).ToString(CultureInfo.InvariantCulture) : cell.Value.ToString()) : "";
                        break;
                }
            }
        }

        private void dataGridView_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (isLastColumnResizing)
            {
                (sender as DataGridView).EndEdit();
                return;
            }

            DataGridView grid = sender as DataGridView;
            
            if (e.Control.GetType() == typeof(ZDataGridViewSelectBoxEditingControl))
            {
                ZDataGridViewSelectBoxEditingControl control = e.Control as ZDataGridViewSelectBoxEditingControl;
                ZDataGridViewSelectBoxCell cell = grid.CurrentCell as ZDataGridViewSelectBoxCell;
                if (cell != null && cell.Editable)
                {
                    /*string columnName = (cell.OwningColumn == null) ? string.Empty : cell.OwningColumn.Name;
                    ZLayer layer = cell.OwningRow.Tag as ZLayer;

                    if (layer != null)
                    {
                        ZLayer.PlaneReferences references;
                        if (layer.GetPlaneReferences(out references))
                        {
                            if (columnName == ZStringConstants.ParameterIDZo_TopReference ||
                                columnName == ZStringConstants.ParameterIDZdiff_TopReference)
                            {
                                control.Items.AddRange(references.refsUp.ToArray());
                            }

                            if (columnName == ZStringConstants.ParameterIDZo_BottomReference ||
                                columnName == ZStringConstants.ParameterIDZdiff_BottomReference)
                            {
                                control.Items.AddRange(references.refsDown.ToArray());
                            }
                        }
                    }*/

                    control.DropDownStyle = ComboBoxStyle.DropDown;
                    control.Validating += cell.Validating;

                    return;
                }
            }

            if (e.Control.GetType() == typeof(DataGridViewComboBoxEditingControl))
            {
                DataGridViewComboBoxEditingControl control = e.Control as DataGridViewComboBoxEditingControl;
                ZDataGridViewComboBoxCell cell = grid.CurrentCell as ZDataGridViewComboBoxCell;
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

                return;
            }
        }

        void dataGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (curentEditingControl != null)
            {
                curentEditingControl.KeyPress -= NumberBox_KeyPress;
                curentEditingControl.KeyPress -= PercentBox_KeyPress;
                curentEditingControl.KeyPress -= RatioBox_KeyPress;
            }

            ZDataGridViewPercentBoxCell cell = (sender as DataGridView).CurrentCell as ZDataGridViewPercentBoxCell;
            if (cell != null && curentEditingControl.Text.Contains("%")) curentEditingControl.Text = curentEditingControl.Text.Replace("%", string.Empty);
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

        public void SelectRow(string id)
        {
            foreach(ZDataGridViewRow row in stackupGridView.Rows)
            {
                ZLayer layer = row.Tag as ZLayer;
                if (layer != null && layer.ID == id)
                {
                    stackupGridView.ClearSelection();
                    row.Selected = true;
                }
            }
        }

        public void SetCurrentRow(int index)
        {
            if (index >= 0 && index < stackupGridView.Rows.Count)
            {
                stackupGridView.ClearSelection();
                int colIndex = stackupGridView.FirstDisplayedScrollingColumnIndex;
                if (colIndex >= 0) stackupGridView.CurrentCell = stackupGridView.Rows[index].Cells[colIndex];
                stackupGridView.Rows[index].Selected = true;
            }
        }

        public void AddRowSelection(int index)
        {
            if (index >= 0 && index < Stackup.Layers.Count)
            {
                if (!rowSelection.Contains(Stackup.Layers[index].ID)) rowSelection.Add(Stackup.Layers[index].ID); 
            }

            foreach(DataGridViewRow row in stackupGridView.Rows)
            {
                ZLayer layer = row.Tag as ZLayer;
                if (layer != null && rowSelection.Contains(layer.ID)) row.Selected = true;
            }
        }

        public void ClearRowSelection()
        {
            stackupGridView.ClearSelection();
            rowSelection.Clear();
        }

        private void RecalculateParametersOrder()
        {
            int order = 0;

            ZList<ZParameter> parameters = ZPlannerManager.Project.Parameters;

            foreach (var parameter in parameters.Where(X => X.IsFrozen == true).OrderBy(X => X.Order))
            {
                parameter.Order = order;
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                {
                    foreach (var subparameter in parameter.SubParameters.OrderBy(X => X.Order))
                    {
                        subparameter.Order = order;
                        ++order;
                    }
                }
                else
                {
                    ++order;
                }
            }

            foreach (var parameter in parameters.Where(X => X.Table == ZTableType.Viaspan && X.IsFrozen == false).OrderBy(X => X.Order))
            {
                parameter.Order = order;
                if (stackup != null && stackup.Spans.Count > 1)
                {
                    order += stackup.Spans.Count;
                }
                else
                {
                    ++order;
                }
            }

            foreach (var parameter in parameters.Where(X => X.Table == ZTableType.Stackup && X.IsFrozen == false).OrderBy(X => X.Order))
            {
                parameter.Order = order;
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                {
                    foreach (var subparameter in parameter.SubParameters.OrderBy(X => X.Order))
                    {
                        subparameter.Order = order;
                        ++order;
                    }
                }
                else
                {
                    ++order;
                }
            }

            foreach (var parameter in parameters.Where(X => X.Table == ZTableType.Single && X.IsFrozen == false).OrderBy(X => X.Order))
            {
                parameter.Order = order;
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                {
                    foreach (var subparameter in parameter.SubParameters.OrderBy(X => X.Order))
                    {
                        subparameter.Order = order;
                        ++order;
                    }
                }
                else
                {
                    ++order;
                }
            }

            foreach (var parameter in parameters.Where(X => X.Table == ZTableType.Pair && X.IsFrozen == false).OrderBy(X => X.Order))
            {
                parameter.Order = order;
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0)
                {
                    foreach (var subparameter in parameter.SubParameters.OrderBy(X => X.Order))
                    {
                        subparameter.Order = order;
                        ++order;
                    }
                }
                else
                {
                    ++order;
                }
            }

            RecalculateColumnHeaderName();
        }

        internal void SetTabName()
        {
             this.Text = "Main View - " + stackup.Title;
        }

        void ZPlannerStackupPanel_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!ZPlannerManager.CloseProject(Clear))
            {
                e.Cancel = true;
                return;
            }
                        
            Close();
        }

        private static int Max(params int[] numbers)
        {
            return numbers.Max();
        }

        void Parameters_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            ClearStakup();
            ClearGrids();
            LoadData(ZPlannerManager.Project.Stackup, single, pair);
        }

        void Layers_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            ClearStakup();
            ClearGrids();
            LoadData(ZPlannerManager.Project.Stackup, single, pair);

            ZPlannerManager.MainMenu.UpdateMenu();
            ZPlannerManager.UpdateActiveStackup();
        }

        void Spans_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (ZPlannerManager.IsIgnoreCollectionChanged) return;
            //ReLoadSpans();
            ClearStakup();
            ClearGrids();
            LoadData(ZPlannerManager.Project.Stackup, single, pair);
        }

        void ZSpan_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            switch (e.PropertyName)
            {
                case "Enable":
                    if (ZPlannerManager.IsIgnoreCollectionChanged) return;
                    //ReLoadSpans();
                    ClearStakup();
                    ClearGrids();
                    LoadData(ZPlannerManager.Project.Stackup, single, pair);
                    break;
                default:
                    break;
            }
        }

        private void ReLoadSpans()
        {
            foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Viaspan))
            {
                ZDataGridViewViaSpanColumn spanColumn = column as ZDataGridViewViaSpanColumn;
                if (spanColumn != null)
                {
                    ZSpan span = stackup.Spans.Find((ZSpan x) => x.ID == spanColumn.Name);
                    if (span != null) span.PropertyChanged -= spanColumn.ZSpan_PropertyChanged;
                }
            }

            foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
            {
                span.PropertyChanged -= ZSpan_PropertyChanged;
            }

            foreach (ZDataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Viaspan))
            {
                stackupGridView.Columns.Remove(column);
            }

            // Load Spans
            ZList<ZSpan> enabledSpans = new ZList<ZSpan>(stackup, stackup.Spans.FindAll(x => x.IsEnable == true));
            AddColumns(new ZList<ZParameter>(stackup.Project, stackup.Project.Parameters.FindAll(x => x.Table == ZTableType.Viaspan)), enabledSpans);
            
            foreach (ZDataGridViewRow row in stackupGridView.Rows)
            {
                foreach (DataGridViewColumn column in stackupGridView.Columns.Cast<ZDataGridViewColumn>().Where(x => x.TableType == ZTableType.Viaspan))
                {
                    DataGridViewCell cell = new ZDataGridViewViaSpanCell();
                    cell.Value = "";
                    row.Cells.Add(cell);
                }
            }

            AddViaSpan(enabledSpans);

            RecalculateColumnHeaderName();
            stackupGridView.Invalidate();
        }

        void Singles_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (!stackup.Singles.Contains(single)) ClearSingle();
            SetSingleComboBox();
        }

        void Pairs_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            if (!stackup.Pairs.Contains(pair)) ClearPair();
            SetPairComboBox();
        }

        void dataGridView_RowStateChanged(object sender, DataGridViewRowStateChangedEventArgs e)
        {
            if (e.StateChanged != DataGridViewElementStates.Selected) return;
                        
            DataGridViewSelectedRowCollection selectedRows = (sender as DataGridView).SelectedRows;
                
            /*for (int i = 0; i < stackupGridView.Rows.Count; ++i)
            {
                bool isSelected = false;
                foreach (ZDataGridViewRow selectedRow in selectedRows)
                    if (selectedRow.Index == i)
                    {
                        isSelected = true;
                        break;
                    }
                stackupGridView.Rows[i].Selected = isSelected;
            }*/

            if (sender == stackupGridView && selectedRows.Count == 1)
            {
                ZLayer layer = selectedRows[0].Tag as ZLayer;
                if (layer != null) ZPlannerManager.ProjectPanel.SelectLayer(layer.ID);
            }

            ZPlannerManager.MainMenu.UpdateMenu();
        }

        void dataGridView_CellStateChanged(object sender, DataGridViewCellStateChangedEventArgs e)
        {
            if (e.StateChanged == DataGridViewElementStates.Selected)
            {
                DataGridViewSelectedCellCollection selectedCells = (sender as DataGridView).SelectedCells;

                if (selectedCells.Count == 1)
                {
                    ZLayerParameter layerParameter = e.Cell.Tag as ZLayerParameter;
                    if (layerParameter != null)
                    {
                        ZLayer layer = layerParameter.Layer;
                        if (layer != null) ZPlannerManager.ProjectPanel.SelectLayer(layer.ID);
                    }
                    else
                    {
                        ZSpan span = e.Cell.OwningColumn.Tag as ZSpan;
                        if (span != null) ZPlannerManager.ProjectPanel.SelectSpan(span.ID);
                    }

                    (sender as DataGridView).Focus();
                }
            }
        }

        void dataGridView_RowHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            if (e.Button != MouseButtons.Right) return;
            if (e.RowIndex < 0) return;
            DataGridView grid = sender as DataGridView;
            DataGridViewRow row = grid.Rows[e.RowIndex];
            rowContextMenu.Tag = row;
            ZLayer layer = row.Tag as ZLayer;
            if (layer != null)
            {
                ZLayerType? layerType = layer.GetLayerType();
                this.duplicateRowToolStripMenuItem.Enabled = layer.GetLayerType() == ZLayerType.Prepreg;
                this.moveRowUpToolStripMenuItem.Enabled = layer.Order > 0;
                this.moveRowDownToolStripMenuItem.Enabled = layer.Order < layer.Stackup.Layers.Count - 1;
                bool clipboardNotEmpty = ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyStackupLayers);
                this.pasteRowBeforeToolStripMenuItem.Enabled = clipboardNotEmpty;
                this.pasteRowAfterToolStripMenuItem.Enabled = clipboardNotEmpty; 
                clipboardNotEmpty = ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials].Count > 0;
                pasteMaterialToolStripMenuItem.Enabled = clipboardNotEmpty;
                clearMaterialToolStripMenuItem.Enabled = (layer.isDielectric() && layer.IsMaterialAssigned);

                if (clipboardNotEmpty)
                {
                    ZMaterial material = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials][0] as ZMaterial;
                    if (material == null)
                    {
                        pasteMaterialToolStripMenuItem.Enabled = false;
                    }
                    else
                    {
                        pasteMaterialToolStripMenuItem.DropDownItems.Clear();

                        ZLayerType? materialType = material.GetMaterialType();

                        if (materialType != null && layerType == materialType)
                        {
                            // Use Current Row
                            ToolStripMenuItem pasteMaterialToCurrentLayerButton = new ToolStripMenuItem("Apply selected material to current row.  ");
                            pasteMaterialToCurrentLayerButton.Tag = layer;
                            pasteMaterialToCurrentLayerButton.Name = "pasteMaterialToCurrentLayerButton";
                            pasteMaterialToolStripMenuItem.DropDownItems.Add(pasteMaterialToCurrentLayerButton);

                            // Use All Rows of current type
                            ToolStripMenuItem pasteMaterialToAllLayersButton = new ToolStripMenuItem(string.Format("Apply selected material to all {0}s.  ", materialType.ToString()));
                            pasteMaterialToAllLayersButton.Tag = layer;
                            pasteMaterialToAllLayersButton.Name = "pasteMaterialToAllLayersButton";
                            pasteMaterialToolStripMenuItem.DropDownItems.Add(pasteMaterialToAllLayersButton);
                        }

                        // Create New Row Before
                        ToolStripMenuItem pasteMaterialToNewLayerBeforeButton = new ToolStripMenuItem("Apply selected material to a new stackup row before this.  ");
                        pasteMaterialToNewLayerBeforeButton.Tag = string.Empty;
                        pasteMaterialToNewLayerBeforeButton.Name = "pasteMaterialToNewLayerBeforeButton";

                        pasteMaterialToolStripMenuItem.DropDownItems.Add(pasteMaterialToNewLayerBeforeButton);

                        // Create New Row After
                        ToolStripMenuItem pasteMaterialToNewLayerAfterButton = new ToolStripMenuItem("Apply selected material to a new stackup row after this.  ");
                        pasteMaterialToNewLayerAfterButton.Tag = string.Empty;
                        pasteMaterialToNewLayerAfterButton.Name = "pasteMaterialToNewLayerAfterButton";

                        pasteMaterialToolStripMenuItem.DropDownItems.Add(pasteMaterialToNewLayerAfterButton);                        
                    }
                }

                this.swapRowsToolStripMenuItem.Enabled = (grid.SelectedRows.Count == 2 && grid.SelectedRows.Contains(row) || grid.SelectedRows.Count == 1 && grid.SelectedRows[0] != row);
            }

            rowContextMenu.Show(Cursor.Position);
        }

        void dataGridView_ColumnHeaderMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            //int rowIndex = (grid.CurrentRowIndex >= 0) ? grid.CurrentRowIndex : 0;
            //if (grid.Rows.Count > rowIndex && e.ColumnIndex >= 0) grid.CurrentCell = grid[e.ColumnIndex, rowIndex];

            if (grid.Rows.Count > 0 && grid.Columns.Count > 0) { grid[0, 0].Selected = false; grid.InvalidateCell(0, 0); }

            if (e.Button != MouseButtons.Right) return;
            if (e.ColumnIndex < 0) return;
            ZDataGridViewColumn column = grid.Columns[e.ColumnIndex] as ZDataGridViewColumn;
            if (column == null) return;
            if (column.TableType == ZTableType.Viaspan)
            {
                viaspanContextMenu.Show(Cursor.Position);
                return;
            }
            columnContextMenu.Tag = column;
            ZParameter parameter = column.Tag as ZParameter;
            if (parameter != null)
            {
                ZDataGridViewColumn prevColumn = grid.Columns.GetPreviousColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None) as ZDataGridViewColumn;
                ZDataGridViewColumn nextColumn = grid.Columns.GetNextColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None) as ZDataGridViewColumn;
                this.moveColumnToTheLeftToolStripMenuItem.Enabled = (!column.Frozen && prevColumn != null && prevColumn.TableType == column.TableType);
                this.moveColumnToTheRightToolStripMenuItem.Enabled = (!column.Frozen && nextColumn != null && nextColumn.TableType == column.TableType);
                this.removeColumnToolStripMenuItem.Enabled = parameter.IsCustom;
                this.minimizeColumnToolStripMenuItem.Checked = parameter.IsCollapsed;
                this.addColumnBeforeToolStripMenuItem.Enabled = (!column.Frozen && column.TableType == ZTableType.Stackup);
                this.addColumnAfterToolStripMenuItem.Enabled = (!column.Frozen && column.TableType == ZTableType.Stackup);
                this.hideColumnToolStripMenuItem.Enabled = (!column.Frozen);
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
            if (!column.Frozen)
            {
                Dictionary<string, List<ZDataGridViewColumn>> grouppedColumns = grid.GetColumnsOrderedByTitle(column.TableType);
                foreach (string group in grouppedColumns.Keys)
                {
                    bool insertHiddenColumnBeforeSeparatorRequired = (group != string.Empty);
                    bool insertHiddenColumnAfterSeparatorRequired = (group != string.Empty);
                    bool insertExistingColumnBeforeSeparatorRequired = (group != string.Empty);
                    bool insertExistingColumnAfterSeparatorRequired = (group != string.Empty);

                    foreach (ZDataGridViewColumn columnToShow in grouppedColumns[group])
                    {
                        ZParameter parameterToShow = columnToShow.Tag as ZParameter;
                        if (parameterToShow != null && parameter != parameterToShow && !parameterToShow.IsPrivate)
                        {
                            if (parameterToShow.IsHidden)
                            {
                                if (insertHiddenColumnBeforeSeparatorRequired)
                                {
                                    this.insertHiddenColumnBeforeToolStripMenuItem.DropDownItems.Add(new ToolStripSeparator());
                                    insertHiddenColumnBeforeSeparatorRequired = false;
                                }

                                if (insertHiddenColumnAfterSeparatorRequired)
                                {
                                    this.insertHiddenColumnAfterToolStripMenuItem.DropDownItems.Add(new ToolStripSeparator());
                                    insertHiddenColumnAfterSeparatorRequired = false;
                                }

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
                                    if (insertExistingColumnBeforeSeparatorRequired)
                                    {
                                        this.insertExistingColumnBeforeToolStripMenuItem.DropDownItems.Add(new ToolStripSeparator());
                                        insertExistingColumnBeforeSeparatorRequired = false;
                                    }

                                    ToolStripMenuItem newBeforeItem = new ToolStripMenuItem(parameterToShow.Title);
                                    newBeforeItem.Name = parameterToShow.ID;
                                    this.insertExistingColumnBeforeToolStripMenuItem.DropDownItems.Add(newBeforeItem);
                                    this.insertExistingColumnBeforeToolStripMenuItem.Enabled = true;
                                }

                                if (column.DisplayIndex - columnToShow.DisplayIndex != -1)
                                {
                                    if (insertExistingColumnAfterSeparatorRequired)
                                    {
                                        this.insertExistingColumnAfterToolStripMenuItem.DropDownItems.Add(new ToolStripSeparator());
                                        insertExistingColumnAfterSeparatorRequired = false;
                                    }

                                    ToolStripMenuItem newAfterItem = new ToolStripMenuItem(parameterToShow.Title);
                                    newAfterItem.Name = parameterToShow.ID;
                                    this.insertExistingColumnAfterToolStripMenuItem.DropDownItems.Add(newAfterItem);
                                    this.insertExistingColumnAfterToolStripMenuItem.Enabled = true;
                                }
                            }
                        }
                    }
                }
            }

            /*foreach (ZDataGridViewColumn columnToShow in stackupGridView.Columns)
            {
                PrepareParameterToShowCollection(columnToShow.Tag as ZParameter);
            }

            foreach (ZDataGridViewColumn columnToShow in singleGridView.Columns)
            {
                PrepareParameterToShowCollection(columnToShow.Tag as ZParameter);
            }

            foreach (ZDataGridViewColumn columnToShow in pairGridView.Columns)
            {
                PrepareParameterToShowCollection(columnToShow.Tag as ZParameter);
            }*/

            columnContextMenu.Show(Cursor.Position);
        }

        private void dataGridView_CellContentClick(object sender, DataGridViewCellEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null || e.RowIndex < 0 || e.ColumnIndex < 0) return;

            ZDataGridViewColumn column = grid.Columns[e.ColumnIndex] as ZDataGridViewColumn;
            if (column == null) return;

            ZDataGridViewRow row = grid.Rows[e.RowIndex] as ZDataGridViewRow;
            if (row == null) return;

            ZDataGridViewLinkBoxCell linkCell = row.Cells[e.ColumnIndex] as ZDataGridViewLinkBoxCell;
            if (linkCell != null && linkCell.ButtonEnabled)
            {
                if (column.Name == ZStringConstants.ParameterIDZo_LossViewer)
                {
                    ZLayer layer = row.Tag as ZLayer;
                    if (layer != null) ZPlannerManager.ShowLossPanel(layer, ZTableType.Single);
                }

                if (column.Name == ZStringConstants.ParameterIDZdiff_LossViewer)
                {
                    ZLayer layer = row.Tag as ZLayer;
                    if (layer != null) ZPlannerManager.ShowLossPanel(layer, ZTableType.Pair);
                }
            }
        }

        void dataGridView_CellEnter(object sender, DataGridViewCellEventArgs e)
        {
            if (e.RowIndex < 0 || e.ColumnIndex < 0) return;
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            // Check to make sure the cell clicked is the cell containing the combobox 
            if (grid[e.ColumnIndex, e.RowIndex] is ZDataGridViewComboBoxCell)
            {
                grid.BeginEdit(true);
            }
        }

        void dataGridView_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null || e.RowIndex < 0 || e.ColumnIndex < 0) return;

            ZDataGridViewColumn column = grid.Columns[e.ColumnIndex] as ZDataGridViewColumn;
            if (column == null) return;

            ZDataGridViewRow row = grid.Rows[e.RowIndex] as ZDataGridViewRow;
            if (row == null) return;

            ZDataGridViewViaSpanCell viaspanCell = row.Cells[e.ColumnIndex] as ZDataGridViewViaSpanCell;
            if (viaspanCell != null) ZPlannerManager.ShowViaSpanPanel();
        }

        void rowContextMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string itemName = e.ClickedItem.Name;
            ZLayerType layerType = ZLayerType.Signal;

            DataGridViewRow row = rowContextMenu.Tag as DataGridViewRow;
            if (row == null) return;

            DataGridView grid = row.DataGridView;
            if (grid == null) return;

            ZLayer layer = row.Tag as ZLayer;

            DataGridViewRow selectedRow = null;
            ZLayer selectedLayer = null;

            if (grid.SelectedRows.Count == 1 && grid.SelectedRows[0] != row) selectedRow = grid.SelectedRows[0];
            if (grid.SelectedRows.Count == 2 && grid.SelectedRows[0] == row) selectedRow = grid.SelectedRows[1];
            if (grid.SelectedRows.Count == 2 && grid.SelectedRows[1] == row) selectedRow = grid.SelectedRows[2];

            if (selectedRow != null && selectedRow != row) selectedLayer = selectedRow.Tag as ZLayer;

            if (ZStringConstants.LayerType.Contains(e.ClickedItem.Name) && e.ClickedItem.OwnerItem != null && (e.ClickedItem.OwnerItem.Name == "addRowBeforeToolStripMenuItem" || e.ClickedItem.OwnerItem.Name == "addRowAfterToolStripMenuItem"))
            {
                layerType = ZLayer.LayerTypeDictionary[itemName];
                itemName = e.ClickedItem.OwnerItem.Name;
            }

            RowContextMenuOperation(itemName, grid, layer, selectedLayer, layerType);
        }

        private void RowContextMenuOperation(string itemName, DataGridView grid, ZLayer layer, ZLayer selectedLayer, ZLayerType layerType = ZLayerType.Signal)
        {
            if (grid == null) grid = stackupGridView;
            int layerOrder = 0;

            if (layer == null && Stackup.Layers.Count > 0) layer = Stackup.Layers[0];

            if (layer != null)
            {

                if (ZPlannerManager.IsCoreLocked)
                {
                    layer = GetCoreLockedLayer(grid, layer);
                    if (selectedLayer != null) selectedLayer = GetCoreLockedLayer(grid, selectedLayer);
                }

                if (selectedLayer != null && selectedLayer.ID == layer.ID) selectedLayer = null;

                if (ZPlannerManager.IsAutoMirror)
                {
                    if (stackup.Layers.Count % 2 != 0)
                    {
                        if (layer.Order == (int)(stackup.Layers.Count * 0.5) && (itemName == "moveRowUpToolStripMenuItem" || itemName == "moveRowDownToolStripMenuItem") ||
                            (layer.Order == (int)(stackup.Layers.Count * 0.5) || selectedLayer != null && selectedLayer.Order == (int)(stackup.Layers.Count * 0.5)) && itemName == "swapRowsToolStripMenuItem")
                        {
                            ZPlannerManager.MessagePanel.AddMessage("Uncheck 'Auto-Mirror' to perform this operation.");
                            return;
                        }
                    }
                }

                layerOrder = (ZPlannerManager.IsCoreLocked) ? GetCoreLockedLayerOrder(layer.Order) : layer.Order;
            }
            
            switch (itemName)
            {
                case "addRowBeforeToolStripMenuItem":
                    stackup.AddLayer(layerOrder, layerType, ZPlannerManager.IsCoreLocked);
                    break;
                case "addRowAfterToolStripMenuItem":
                    stackup.AddLayer(layerOrder + 1, layerType, ZPlannerManager.IsCoreLocked);
                    break;
                case "moveRowUpToolStripMenuItem":
                    if (layerOrder > 0)
                        stackup.MoveLayer(layer.ID, layerOrder - 1, ZPlannerManager.IsCoreLocked);
                    break;
                case "moveRowDownToolStripMenuItem":
                    int layerCount = (ZPlannerManager.IsCoreLocked) ? GetCoreLockedLayerCount() : stackup.Layers.Count;
                    if (layerOrder + 1 < layerCount)
                        stackup.MoveLayer(layer.ID, layerOrder + 1, ZPlannerManager.IsCoreLocked);
                    break;
                case "swapRowsToolStripMenuItem":
                    if (selectedLayer != null)
                        stackup.SwapLayers(layer.ID, selectedLayer.ID, ZPlannerManager.IsCoreLocked);
                    break;
                case "removeRowToolStripMenuItem":
                    stackup.RemoveLayer(layer.ID, ZPlannerManager.IsCoreLocked);
                    break;
                case "duplicateRowToolStripMenuItem":
                    if (layer.GetLayerType() == ZLayerType.Prepreg) stackup.DuplicateLayer(layer.ID);
                    break;
                case "copyRowToolStripMenuItem":
                    if (ZPlannerManager.IsCoreLocked) stackup.CopyLayers(GetCoreLockedLayerIDs(grid, layer));
                    else stackup.CopyLayer(layer.ID);
                    break;
                case "cutRowToolStripMenuItem":
                    if (ZPlannerManager.IsCoreLocked) stackup.CutLayers(GetCoreLockedLayerIDs(grid, layer));
                    else stackup.CutLayer(layer.ID);
                    break;
                case "pasteRowBeforeToolStripMenuItem":
                    stackup.PasteLayers(layerOrder, ZPlannerManager.IsCoreLocked);
                    break;
                case "pasteRowAfterToolStripMenuItem":
                    stackup.PasteLayers(layerOrder + 1, ZPlannerManager.IsCoreLocked);
                    break;
                case "pasteMaterialToCurrentLayerButton":
                    ZMaterial material = (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials].Count > 0) ? ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials][0] as ZMaterial : null;
                    if (material == null) return;
                    List<ZLayer> layers = new List<ZLayer>();
                    layers.Add(layer);
                    ZPlannerManager.Project.Stackup.AssignMaterialToStackup(material, layers);
                    break;
                case "pasteMaterialToAllLayersButton":
                    material = (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials].Count > 0) ? ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials][0] as ZMaterial : null;
                    if (material == null) return;
                    layers = layer.Stackup.Layers.FindAll(x => x.GetLayerType() == layer.GetLayerType());
                    ZPlannerManager.Project.Stackup.AssignMaterialToStackup(material, layers);
                    break;
                case "pasteMaterialToNewLayerBeforeButton":
                    material = (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials].Count > 0) ? ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials][0] as ZMaterial : null;
                    if (material == null) return;
                    ZLayerType? materialType = material.GetMaterialType();
                    if (materialType == null) return;
                    string newLayerID = stackup.AddLayer(layerOrder, (ZLayerType)materialType, ZPlannerManager.IsCoreLocked);
                    ZLayer newLayer = Stackup.Layers.Find(x => x.ID == newLayerID);
                    if (newLayer == null) return;
                    layers = new List<ZLayer>();
                    layers.Add(newLayer);
                    ZPlannerManager.Project.Stackup.AssignMaterialToStackup(material, layers);
                    break;
                case "pasteMaterialToNewLayerAfterButton":
                    material = (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterials) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials].Count > 0) ? ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterials][0] as ZMaterial : null;
                    if (material == null) return;
                    materialType = material.GetMaterialType();
                    if (materialType == null) return;
                    newLayerID = stackup.AddLayer(layerOrder + 1, (ZLayerType)materialType, ZPlannerManager.IsCoreLocked);
                    newLayer = Stackup.Layers.Find(x => x.ID == newLayerID);
                    if (newLayer == null) return;
                    layers = new List<ZLayer>();
                    layers.Add(newLayer);
                    ZPlannerManager.Project.Stackup.AssignMaterialToStackup(material, layers);
                    break;
                case "clearMaterialToolStripMenuItem":
                    layers = new List<ZLayer>();
                    layers.Add(layer);
                    ZPlannerManager.Project.Stackup.UnAssignMaterialToStackup(layers);
                    break;
            }
        }

        internal List<string> GetCoreLockedLayerIDs(DataGridView grid, ZLayer layer)
        {
            List<string> idList = new List<string>();
            idList.Add(layer.ID);

            if (layer.GetLayerType() == ZLayerType.Core)
            {
                int index;
                if (0 < layer.Order)
                {
                    index = layer.Order - 1;
                    ZLayer copperLayer = grid.Rows[index].Tag as ZLayer;
                    if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                    {
                        idList.Insert(0, copperLayer.ID);
                    }
                }

                if (grid.Rows.Count > layer.Order + 1)
                {
                    index = layer.Order + 1;
                    ZLayer copperLayer = grid.Rows[index].Tag as ZLayer;
                    if (copperLayer != null && ZConstants.CopperTypeList.Contains(copperLayer.GetLayerType()))
                    {
                        idList.Add(copperLayer.ID);
                    }
                }
            }

            return idList;
        }

        internal ZLayer GetCoreLockedLayer(DataGridView grid, ZLayer layer)
        {
            if(ZConstants.CopperTypeList.Contains(layer.GetLayerType()))
            {
                int index;
                if (0 < layer.Order)
                {
                    index = layer.Order - 1;
                    ZLayer coreLayer = grid.Rows[index].Tag as ZLayer;
                    if (coreLayer != null && coreLayer.GetLayerType() == ZLayerType.Core)
                    {
                        return coreLayer;
                    }
                }
            
                if (grid.Rows.Count > layer.Order + 1)
                {
                    index = layer.Order + 1;
                    ZLayer coreLayer = grid.Rows[index].Tag as ZLayer;
                    if (coreLayer != null && coreLayer.GetLayerType() == ZLayerType.Core)
                    {
                        return coreLayer;
                    }
                }
            }

            return layer;
        }

        internal int GetCoreLockedLayerOrder(int order)
        {
            int index = order;
            for(int i = order; i >= 0; --i)
            {
                if (Stackup.Layers[i].GetLayerType() != ZLayerType.Core) continue;

                if (i + 1 < index && ZConstants.CopperTypeList.Contains(Stackup.Layers[i + 1].GetLayerType())) { --order; index = i; }

                if (i - 1 >= 0 && ZConstants.CopperTypeList.Contains(Stackup.Layers[i + 1].GetLayerType())) { --order; index = i - 1; }
            }

            return order;
        }

        internal int GetCoreLockedLayerCount()
        {
            int count = Stackup.Layers.Count;
            int index = Stackup.Layers.Count - 1;
            for (int i = index; i >= 0; --i)
            {
                if (Stackup.Layers[i].GetLayerType() != ZLayerType.Core) continue;

                if (i + 1 <= index && ZConstants.CopperTypeList.Contains(Stackup.Layers[i + 1].GetLayerType())) { --count; index = i; }

                if (i - 1 >= 0 && ZConstants.CopperTypeList.Contains(Stackup.Layers[i + 1].GetLayerType())) { --count; index = i - 1; }
            }

            return count;
        }

        void viaspanContextMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string itemName = e.ClickedItem.Name;

            switch (itemName)
            {
                case "viaspanEditToolStripMenuItem":
                    ZPlannerManager.ShowViaSpanPanel();
                    break;
            }
        }

        void columnContextMenu_ItemClicked(object sender, ToolStripItemClickedEventArgs e)
        {
            string itemName = e.ClickedItem.Name;
            string parameterName = string.Empty;

            ZDataGridViewColumn column = columnContextMenu.Tag as ZDataGridViewColumn;
            if (column == null) return;

            ZDataGridView grid = column.DataGridView as ZDataGridView;

            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;

            ZPlannerProject project = ZPlannerManager.Project;

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
                case "addColumnBeforeToolStripMenuItem":
                    project.CreateCustomParameter(parameter.Table, parameter.Order, grid.ColumnCount);
                    SaveStackupSettings();
                    break;
                case "addColumnAfterToolStripMenuItem":
                    project.CreateCustomParameter(parameter.Table, parameter.Order + 1, grid.ColumnCount);
                    SaveStackupSettings();
                    break;
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
                case "removeColumnToolStripMenuItem":
                    project.RemoveCustomParameter(parameter.Table, column.Name);
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
                        foreach (ZDataGridViewColumn columnToShown in stackupGridView.Columns)
                        {
                            if (columnToShown.TableType != column.TableType) continue;
                            parameterToShow = columnToShown.Tag as ZParameter;
                            if (parameterToShow == null) continue;

                            if (!parameterToShow.IsPrivate && parameterToShow.IsHidden)
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
                        foreach(ZDataGridViewColumn col in grid.GetColumnsOrderedByDisplayIndex())
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
                case "propertiesOfColumnToolStripMenuItem":
                    ZPlannerManager.PropertiesPanel.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(parameter);
                    break;
            }

            if (columnsDisplayingStructureList.Count > 0) new ChangeColumnsDisplayingCommand(columnsDisplayingStructureList);
        }

        void dataGridView_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null || e.ColumnIndex < 0) return;

            ZDataGridViewColumn curentColumn = grid.Columns[e.ColumnIndex] as ZDataGridViewColumn;
            ZDataGridViewColumn lastColumn = grid.Columns.GetLastColumn(DataGridViewElementStates.Visible, DataGridViewElementStates.None) as ZDataGridViewColumn;

            if (curentColumn == null || lastColumn == null) return;

            if (Cursor.Current == Cursors.SizeWE && curentColumn.Name == lastColumn.Name)
            {
                isLastColumnResizing = true;
            }
        }

        void dataGridView_CellMouseUp(object sender, DataGridViewCellMouseEventArgs e)
        {
            isLastColumnResizing = false;
        }

        void dataGridView_MouseMove(object sender, MouseEventArgs e)
        {
            if (Cursor.Current != Cursors.SizeWE || e.Button != System.Windows.Forms.MouseButtons.Left) isLastColumnResizing = false;

            if (isLastColumnResizing)
            {
                ZDataGridView grid = sender as ZDataGridView;
                if (grid == null) return;

                DataGridViewColumn curentColumn = grid.Columns.GetLastColumn(DataGridViewElementStates.Visible, DataGridViewElementStates.None);
                int columnIndex = curentColumn.Index;
                Rectangle columnRectangle = grid.GetColumnDisplayRectangle(columnIndex, false);

                if (columnRectangle.X > e.X)
                {
                    isLastColumnResizing = false;
                    return;
                }

                this.Cursor = new Cursor(Cursor.Current.Handle);
                int newWidth = e.X - columnRectangle.X;
                curentColumn.Width = newWidth;
                Cursor.Clip = new Rectangle(grid.RectangleToScreen(columnRectangle).X, grid.RectangleToScreen(columnRectangle).Y, e.X + 1, columnRectangle.Height);
            }
        }

        void dataGridView_ColumnDisplayIndexChanged(object sender, DataGridViewColumnEventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            DataGridViewColumn column = e.Column;

            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;

            if (parameter.Order != column.DisplayIndex)
            {
                parameter.Order = column.DisplayIndex;
                grid.BeginInvoke(new Action(RecalculateParametersOrder));
            }
        }

        void dataGridView_ColumnWidthChanged(object sender, DataGridViewColumnEventArgs e)
        {
            DataGridViewColumn column = e.Column;
            if (column == null) return;
            ZParameter parameter = column.Tag as ZParameter;
            if (parameter == null) return;
            parameter.Width = column.Width;
        }

        void dataGridView_ColumnHeaderMouseDoubleClick(object sender, DataGridViewCellMouseEventArgs e)
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

        internal void SaveStackupSettings()
        {
            string orderOption = string.Empty, hiddenOption = string.Empty, collapsedOption = string.Empty, widthOption = string.Empty;
            foreach (ZDataGridViewColumn column in stackupGridView.GetColumnsOrderedByDisplayIndex())
            {
                ZParameter parameter = column.Tag as ZParameter;

                if (parameter == null) continue;

                orderOption += column.Name + ",";
                widthOption += (parameter.IsCollapsed) ? parameter.WidthToExpand + "," : column.Width + ",";
                hiddenOption += parameter.IsHidden + ",";
                collapsedOption += parameter.IsCollapsed + ",";
            }

            Options.TheOptions.stackupColumnsOrder = orderOption.Trim(',');
            Options.TheOptions.stackupColumnsWidth = widthOption.Trim(',');
            Options.TheOptions.stackupColumnsIsHidden = hiddenOption.Trim(',');
            Options.TheOptions.stackupColumnsIsCollapsed = collapsedOption.Trim(',');
        }

        internal void LoadStackupSettings()
        {
            if (Options.TheOptions.stackupColumnsOrder == null ||
                Options.TheOptions.stackupColumnsWidth == null ||
                Options.TheOptions.stackupColumnsIsHidden == null ||
                Options.TheOptions.stackupColumnsIsCollapsed == null) return;

            string[] orderArray = Options.TheOptions.stackupColumnsOrder.Split(',');
            string[] widthArray = Options.TheOptions.stackupColumnsWidth.Split(',');
            string[] hiddenArray = Options.TheOptions.stackupColumnsIsHidden.Split(',');
            string[] collapsedArray = Options.TheOptions.stackupColumnsIsCollapsed.Split(',');

            if (orderArray.Length != widthArray.Length || orderArray.Length != hiddenArray.Length || orderArray.Length != collapsedArray.Length) return;

            int order = 0;
            for (int i = 0; i < orderArray.Length; ++i)
            {
                ZList<ZParameter> parameters =  Stackup.Project.Parameters;
                ZParameter parameter = parameters.GetByID(orderArray[i]); 
                if (parameter == null)
                {
                    parameters = Stackup.Project.SubParameters;
                    parameter = parameters.GetByID(orderArray[i]);
                }
                if (parameter == null) continue;

                if (parameter.Order != order)
                {
                    ShiftParametersOrder(parameters, order, parameter.Order);
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

        void RecalculateColumnHeaderName()
        {
            int index = 0;

            foreach (ZDataGridViewColumn column in stackupGridView.GetColumnsOrderedByDisplayIndex())
            {
                if (column is ZDataGridViewViaSpanColumn)
                {
                    if (ZPlannerManager.IsViasVisible) column.HeaderName = column.GetHeaderNameByOrder(index++);
                }
                else
                {
                    ZParameter parameter = column.Tag as ZParameter;
                    if (parameter == null || parameter.IsPrivate || parameter.IsHidden) continue;
                    column.HeaderName = column.GetHeaderNameByOrder(index++);
                }
            }
        }

        void ZPlannerStackupPanel_Activated(object sender, EventArgs e)
        {
            ZPlannerManager.ProjectPanel.SelectStackup();
            ZPlannerManager.MainMenu.UpdateMenu();
        }

        internal ZLayer GetCurrentLayer()
        {
            DataGridViewRow row = stackupGridView.CurrentRow;
            if (row != null && row.Selected) return row.Tag as ZLayer;

            return null;
        }

        internal List<ZLayer> GetSelectedLayers()
        {
            List<ZLayer> layers = new List<ZLayer>();

            foreach (DataGridViewRow row in stackupGridView.SelectedRows)
            {
                ZLayer layer = row.Tag as ZLayer;
                if (layer != null) layers.Add(layer);
            }

            return layers;
        }

        internal ZLayerParameter GetSelectedLayerParameter()
        {
            DataGridViewCell cell = stackupGridView.CurrentCell;
            if (cell != null && cell.Selected) return cell.Tag as ZLayerParameter;

            return null;
        }

        public void Copy()
        {
            if (!IsHidden)
            {
                ZLayer layer = GetCurrentLayer();
                if (layer != null)
                    RowContextMenuOperation("copyRowToolStripMenuItem", null, layer, null);

                /*ZStackup stackup = null;
                List<ZLayer> layers = StackupPanel.GetSelectedLayers();
                if (layers.Count == 0)
                {
                    ZLayer layer = StackupPanel.GetCurrentLayer();
                    if (layer != null) layer.Stackup.CopyLayer(layer.ID);
                    return;
                }

                List<string> ids = new List<string>();
                foreach(ZLayer layer in layers.OrderBy(x => x.Order))
                {
                    if (layer != null) ids.Add(layer.ID);
                    stackup = layer.Stackup;
                }

                if (stackup != null && ids.Count > 0) stackup.CopyLayers(ids);*/
            }
        }

        public void Paste()
        {
            if (!IsHidden)
            {
                if (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyStackupLayers) && ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyStackupLayers] is List<ZEntity>)
                {
                    ZLayer layer = GetCurrentLayer();
                    if (layer != null)
                        RowContextMenuOperation("pasteRowAfterToolStripMenuItem", null, layer, null);
                    else
                        RowContextMenuOperation("pasteRowBeforeToolStripMenuItem", null, layer, null);
                }
                else if (ZPlannerManager.Clipboard.ContainsKey(ZStringConstants.ClipbordKeyMaterialsForStackup))
                {
                    List<ZEntity> entities = ZPlannerManager.Clipboard[ZStringConstants.ClipbordKeyMaterialsForStackup] as List<ZEntity>;
                    if (entities != null && entities.Count > 0)
                    {
                        ZMaterial material = entities[0] as ZMaterial;
                        ZLayer layer = GetCurrentLayer();
                        if (layer != null && material != null && layer.GetLayerType() == material.GetMaterialType())
                            RowContextMenuOperation("pasteMaterialToCurrentLayerButton", null, layer, null);
                    }
                }
            }
        }

        public void Cut()
        {
            if (!IsHidden)
            {
                ZLayer layer = GetCurrentLayer();
                if (layer != null)
                    RowContextMenuOperation("cutRowToolStripMenuItem", null, layer, null);

                /*int indexToSelect = -1;
                ZStackup stackup = null;
                List<ZLayer> layers = StackupPanel.GetSelectedLayers();
                if (layers.Count == 0)
                {
                    ZLayer layer = StackupPanel.GetCurrentLayer();
                    if (layer != null)
                    {
                        indexToSelect = layer.Order;
                        layer.Stackup.CutLayer(layer.ID);
                    }
                    return;
                }

                List<string> ids = new List<string>();
                foreach (ZLayer layer in layers.OrderBy(x => x.Order))
                {
                    if (layer != null)
                    {
                        ids.Add(layer.ID);
                        stackup = layer.Stackup;
                        if (indexToSelect == -1) indexToSelect = layer.Order;
                    }
                }

                if (stackup != null && ids.Count > 0)
                {
                    stackup.CutLayers(ids);
                    StackupPanel.SetCurrentRow(indexToSelect);
                }*/
            }
        }

        internal void DeleteSelectedRows()
        {
            /*List<ZLayer> layers = GetSelectedLayers();
            foreach(ZLayer layer in layers)
            {
                if (layer != null)
                    RowContextMenuOperation("removeRowToolStripMenuItem", null, layer, null);
            }*/

            ZLayer layer = GetCurrentLayer();
            if (layer != null)
                RowContextMenuOperation("removeRowToolStripMenuItem", null, layer, null);
        }

        internal void AddBlankRows(ZLayerType layerType)
        {
            ZLayer layer = GetCurrentLayer();
            if (layer != null)
                RowContextMenuOperation("addRowBeforeToolStripMenuItem", null, layer, null, layerType);
            else
            {
                int index = Stackup.Layers.Count - 1;
                if (index >= 0)
                {
                    layer = Stackup.Layers[index];
                    RowContextMenuOperation("addRowAfterToolStripMenuItem", null, layer, null, layerType);
                }
                else
                {
                    RowContextMenuOperation("addRowBeforeToolStripMenuItem", null, layer, null, layerType);
                }
            }
        }

        internal List<ZLayerParameter> GetLayerParametersByValue(string id, string value)
        {
            List<ZLayerParameter> layerParameters = new List<ZLayerParameter>();
            ZDataGridViewColumn column = stackupGridView.Columns[id] as ZDataGridViewColumn;
            if (column != null)
            {
                foreach (DataGridViewRow row in stackupGridView.Rows)
                {
                    string formatedValue = column.GetFormatedValue(row);
                    if (row.Cells[id] != null && !row.Cells[id].ReadOnly && formatedValue == value)
                    {
                        ZLayerParameter layerParameter = row.Cells[id].Tag as ZLayerParameter;
                        if (layerParameter != null) layerParameters.Add(layerParameter);
                    }
                }
            }

            return layerParameters;
        }

        internal List<ZLayerParameter> GetLayerParametersToEdit(string id)
        {
            List<ZLayerParameter> layerParameters = new List<ZLayerParameter>();
            ZDataGridViewColumn column = stackupGridView.Columns[id] as ZDataGridViewColumn;
            if (column != null)
            {
                foreach (DataGridViewRow row in stackupGridView.Rows)
                {
                    string formatedValue = column.GetFormatedValue(row);
                    if (row.Cells[id] != null && !row.Cells[id].ReadOnly)
                    {
                        ZLayerParameter layerParameter = row.Cells[id].Tag as ZLayerParameter;
                        if (layerParameter != null) layerParameters.Add(layerParameter);
                    }
                }
            }

            return layerParameters;
        }

        internal void RecalculateViaSpanAspectRatio()
        {
            foreach(ZSpan span in Stackup.Spans)
            {
                span.CalculateAspectRatio();
            }
        }

        internal void ShowLossPlanning(bool showLossPlanning)
        {
            string[] columnIDs = new string[] {/*ZStringConstants.ParameterIDThickness, ZStringConstants.ParameterIDZdiff_Frequency, ZStringConstants.ParameterIDZdiff_DielectricConstant, ZStringConstants.ParameterIDZdiff_LossTangent, */
                                                ZStringConstants.ParameterIDZdiff_LossViewer, ZStringConstants.ParameterIDZdiff_InsertionLoss, ZStringConstants.ParameterIDZdiff_TraceLength, ZStringConstants.ParameterIDZdiff_TotalLoss};

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            stackupGridView.SuspendLayout();

            try
            {
                foreach (string columnID in (showLossPlanning) ? columnIDs : columnIDs.Reverse())
                {
                    DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                    if (column == null) continue;
                    ZParameter parameter = column.Tag as ZParameter;
                    if (parameter == null) continue;

                    parameter.IsHidden = !showLossPlanning;
                }
            }
            finally
            {                
                stackupGridView.ResumeLayout();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal void ShowRoughnessColumns(bool showRoughnessColumns)
        {
            string[] columnIDs = new string[] { ZStringConstants.ParameterIDZdiff_RoughTop, ZStringConstants.ParameterIDZdiff_RoughBottom };

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            stackupGridView.SuspendLayout();

            try
            {
                foreach (string columnID in (showRoughnessColumns) ? columnIDs : columnIDs.Reverse())
                {
                    DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                    if (column == null) continue;
                    ZParameter parameter = column.Tag as ZParameter;
                    if (parameter == null) continue;

                    parameter.IsHidden = !showRoughnessColumns;
                }
            }
            finally
            {
                stackupGridView.ResumeLayout();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                stackupGridView.Invalidate();
            }
        }

        internal void ShowGWSColumns(bool showGWSColumns, string columnName = null)
        {
            string[] columnIDs = new string[] { ZStringConstants.ParameterIDZdiff_BitRate, ZStringConstants.ParameterIDZdiff_UnitInterval, ZStringConstants.ParameterIDZdiff_SkewTolerance };

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            stackupGridView.SuspendLayout();

            try
            {
                foreach (string columnID in (showGWSColumns) ? columnIDs : columnIDs.Reverse())
                {
                    if (columnName != null && columnID != columnName) continue;

                    DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                    if (column == null) continue;
                    ZParameter parameter = column.Tag as ZParameter;
                    if (parameter == null) continue;

                    parameter.IsHidden = !showGWSColumns;
                }
            }
            finally
            {
                stackupGridView.ResumeLayout();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                stackupGridView.Invalidate();
            }
        }

        internal void ShowGlassPitch(bool showGlassPitchColumns)
        {
            string[] columnIDs = new string[] { ZStringConstants.ParameterIDZdiff_TracePitch, ZStringConstants.ParameterIDZdiff_WeavePitch, ZStringConstants.ParameterIDZdiff_FillPitch };

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            stackupGridView.SuspendLayout();

            try
            {
                foreach (string columnID in (showGlassPitchColumns) ? columnIDs : columnIDs.Reverse())
                {
                    DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                    if (column == null) continue;
                    ZParameter parameter = column.Tag as ZParameter;
                    if (parameter == null) continue;

                    parameter.IsHidden = !showGlassPitchColumns;
                }
            }
            finally
            {
                stackupGridView.ResumeLayout();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                stackupGridView.Invalidate();
            }
        }

        internal bool IsGlassPitchShown()
        {
            int shownColumnsNumber = 0;
            string[] columnIDs = new string[] { ZStringConstants.ParameterIDZdiff_TracePitch, ZStringConstants.ParameterIDZdiff_WeavePitch, ZStringConstants.ParameterIDZdiff_FillPitch };

            foreach (string columnID in columnIDs)
            {
                DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                if (column == null) continue;
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null) continue;

                if (parameter.IsHidden == false) ++shownColumnsNumber;
            }

            return (columnIDs.Length == shownColumnsNumber);
        }

        internal bool IsGWSShown()
        {
            int shownColumnsNumber = 0;
            string[] columnIDs = new string[] { ZStringConstants.ParameterIDZdiff_BitRate, ZStringConstants.ParameterIDZdiff_UnitInterval, ZStringConstants.ParameterIDZdiff_SkewTolerance };

            foreach (string columnID in columnIDs)
            {
                DataGridViewColumn column = stackupGridView.GetColumnByName(columnID);
                if (column == null) continue;
                ZParameter parameter = column.Tag as ZParameter;
                if (parameter == null) continue;

                if (parameter.IsHidden == false) ++shownColumnsNumber;
            }

            return (shownColumnsNumber > 0); //(columnIDs.Length == shownColumnsNumber);
        }

        // Print functionality.
        private PrintDocument printDocument = new PrintDocument();
        List<Rectangle> pageRectangles = new List<Rectangle>();
        Rectangle printRectangle;
        int pageNumber = 0;

        public void ShowPageSetup()
        {
            if (printDocument == null) printDocument = new PrintDocument();
            
            PageSetupDialog pageSetupDialog = new PageSetupDialog();
            pageSetupDialog.Document = printDocument;
            pageSetupDialog.PageSettings.Landscape = true;
            pageSetupDialog.ShowDialog();
        }

        public void ShowPrintPreview()
        {
            ClearRowSelection();
            stackupGridView.Invalidate();

            if (printDocument == null) printDocument = new PrintDocument();
            
            CaptureScreen();
            printDocument.PrintPage += new PrintPageEventHandler(printDocument_PrintPage);
            printDocument.BeginPrint += printDocument_BeginPrint;

            PreparePrintPages();

            PrintPreviewDialog printPreviewDialog = new PrintPreviewDialog();
            printPreviewDialog.Document = printDocument;
            printPreviewDialog.ShowDialog();

            printDocument.PrintPage -= new PrintPageEventHandler(printDocument_PrintPage);
            printDocument.BeginPrint -= printDocument_BeginPrint;
        }

        public void ShowPrintStackup()
        {
            ClearRowSelection();
            stackupGridView.Invalidate();

            if (printDocument == null) printDocument = new PrintDocument();

            CaptureScreen();
            printDocument.PrintPage += new PrintPageEventHandler(printDocument_PrintPage);
            printDocument.BeginPrint += printDocument_BeginPrint;

            PreparePrintPages();

            PrintDialog printDialog = new PrintDialog();
            printDialog.Document = printDocument;
            printDialog.AllowCurrentPage = false;
            printDialog.AllowSomePages = true;
            if (printDialog.ShowDialog() == DialogResult.OK)
            {
                printDocument.Print();
            }
            /*if (ppdlg.DialogResult == System.Windows.Forms.DialogResult.OK)
            {
                printDocument.Print();
            }*/

            printDocument.PrintPage -= new PrintPageEventHandler(printDocument_PrintPage);
            printDocument.BeginPrint -= printDocument_BeginPrint;
        }

        [System.Runtime.InteropServices.DllImport("gdi32.dll")]
        public static extern long BitBlt(IntPtr hdcDest, int nXDest, int nYDest,
            int nWidth, int nHeight, IntPtr hdcSrc, int nXSrc, int nYSrc, int dwRop);
        private Bitmap memoryImage;
        private void CaptureScreen()
        {
            /*Graphics mygraphics = this.flowLayoutPanel.CreateGraphics();
            Size s = this.flowLayoutPanel.Size;
            memoryImage = new Bitmap(s.Width, s.Height, mygraphics);
            Graphics memoryGraphics = Graphics.FromImage(memoryImage);
            IntPtr dc1 = mygraphics.GetHdc();
            IntPtr dc2 = memoryGraphics.GetHdc();
            BitBlt(dc2, 0, 0, s.Width,
                s.Height, dc1, 0, 0, 13369376);
            mygraphics.ReleaseHdc(dc1);
            memoryGraphics.ReleaseHdc(dc2);*/

            memoryImage = new Bitmap(stackupGridView.Width, stackupGridView.Height);
            stackupGridView.DrawToBitmap(memoryImage, new Rectangle(0, 0, memoryImage.Width, memoryImage.Height));
        }

        private void PreparePrintPages()
        {
            printDocument.DocumentName = ZStringConstants.ZplannerStackupDocument + ((ZPlannerManager.ProjectFile != null) ? " (" + Path.GetFileName(ZPlannerManager.ProjectFile) + ")" : "");
            pageRectangles.Clear();
            int printableAreaWidth = printDocument.DefaultPageSettings.Bounds.Width - printDocument.DefaultPageSettings.Margins.Left - printDocument.DefaultPageSettings.Margins.Right;
            int printableAreaHeight = printDocument.DefaultPageSettings.Bounds.Height - printDocument.DefaultPageSettings.Margins.Top - printDocument.DefaultPageSettings.Margins.Bottom;
            printRectangle = new Rectangle(printDocument.DefaultPageSettings.Margins.Left, printDocument.DefaultPageSettings.Margins.Top, printableAreaWidth, printableAreaHeight);
            List<int> pageWidthList = new List<int>();
            List<int> pageHeightList = new List<int>();

            int width = 0;
            int height = 0;

            // Width
            width += (stackupGridView.RowHeadersVisible) ? stackupGridView.RowHeadersWidth - 1 : 0;

            DataGridViewColumn column = stackupGridView.Columns.GetFirstColumn(DataGridViewElementStates.Visible, DataGridViewElementStates.None);
            while (column != null)
            {
                int columnWidth = column.Width;
                if (width + columnWidth == printableAreaWidth) { pageWidthList.Add(width + columnWidth); width = 0; }
                else if (width + columnWidth > printableAreaWidth) { pageWidthList.Add(width); width = columnWidth; }
                else width += columnWidth;

                column = stackupGridView.Columns.GetNextColumn(column, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
            }

            width += (stackupGridView.RowHeadersVisible) ? 1 : 0;
            

            if (width != 0) pageWidthList.Add(width);

            // Height
            if (stackupGridView.ColumnHeadersVisible) height += stackupGridView.ColumnHeadersHeight - 1;

            int rowIndex = stackupGridView.Rows.GetFirstRow(DataGridViewElementStates.Visible, DataGridViewElementStates.None);
            while (rowIndex >= 0)
            {
                DataGridViewRow row = stackupGridView.Rows[rowIndex];
                int rowHeight = row.Height;
                if (height + rowHeight == printableAreaHeight) { pageHeightList.Add(height + rowHeight); height = 0; }
                else if (height + rowHeight > printableAreaHeight) { pageHeightList.Add(height); height = rowHeight; }
                else height += rowHeight;

                rowIndex = stackupGridView.Rows.GetNextRow(rowIndex, DataGridViewElementStates.Visible, DataGridViewElementStates.None);
            }

            if (stackupGridView.ColumnHeadersVisible) height += 1;

            if (height != 0) pageHeightList.Add(height);

            int pageTop = 0;
            foreach(int pageHeight in pageHeightList)
            {
                int pageLeft = 0;            
                foreach(int pageWidth in pageWidthList)
                {
                    pageRectangles.Add(new Rectangle(pageLeft, pageTop, pageWidth, pageHeight));
                    pageLeft += pageWidth;
                }

                pageTop += pageHeight;
            }
        }

        void printDocument_BeginPrint(object sender, PrintEventArgs e)
        {
            pageNumber = 0;
        }

        private void printDocument_PrintPage(System.Object sender, System.Drawing.Printing.PrintPageEventArgs e)
        {
            //e.Graphics.DrawImage(memoryImage, e.PageSettings.PrintableArea.Left, e.PageSettings.PrintableArea.Top, e.PageSettings.PrintableArea.Width, e.PageSettings.PrintableArea.Height);
            e.Graphics.DrawImage(memoryImage, new Rectangle(printRectangle.Left, printRectangle.Top, pageRectangles[pageNumber].Width, pageRectangles[pageNumber].Height), pageRectangles[pageNumber], GraphicsUnit.Pixel);
            e.HasMorePages = (pageRectangles.Count > ++pageNumber);
        }
        void Program_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ProjectFile")
            {
                string projectName = Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile);
                if (!string.IsNullOrEmpty(projectName) && !ZPlannerManager.ProjectIsEmpty && !ZPlannerManager.Project.StackupIsEmpty && (string.IsNullOrWhiteSpace(ZPlannerManager.Project.Stackup.Title) || ZPlannerManager.Project.Stackup.Title == "UNSAVED")) ZPlannerManager.Project.Stackup.Title = projectName;
                ZPlannerManager.PropertiesPanel.UpdateProperties();
                return;
            }

            if (e.PropertyName == "IsGlassPitch")
            {
                ShowGlassPitch(ZPlannerManager.IsGlassPitch);
                return;
            }

            if (e.PropertyName == "IsGws")
            {
                ShowGWSColumns(ZPlannerManager.IsGws);
                return;
            }
        }

        internal void EndGridEdit()
        {
            stackupGridView.CommitEdit(DataGridViewDataErrorContexts.Commit);
            stackupGridView.EndEdit();

            //stackupGridView.CurrentCell = null;
        }

        internal Point GetViaSpanDialogPoint()
        {
            DataGridViewColumn column = stackupGridView.GetColumnByName(ZStringConstants.ParameterIDLayerName);
            if (column == null) return new Point(600, 200);

            Point p = stackupGridView.GetCellDisplayRectangle(column.Index, 0, false).Location;
            p.Offset(stackupGridView.Location);
            p = this.PointToScreen(p);
            return p;
        }

        internal void TryToEditMaterialAssignedRow()
        {
            ZPlannerManager.NoteOK("Warning", "This dielectric row, which comes from the PCB laminate manufacturer or your corporate library, cannot be edited in a stackup.\n" +
                "\nIf you would like to experiment with different dielectric properties, right click on a row's header cell on the left of the stackup view, select \"Clear Material\" from the pop-up menu.\n" +
                "\nThe material name will default to \"Dielectric\", and it is recommended that you keep this name or any other name that would distinguish it from the original PCB material to reduce the possibility of creating an invalid stackup.");
        }

        internal void UpdateReferences()
        {
            foreach(ZDataGridViewRow row in stackupGridView.Rows)
            {
                ZLayer layer = row.Tag as ZLayer;
                if (layer == null) continue;

                ZLayer.PlaneReferences references;
                if (layer.GetPlaneReferences(out references))
                {
                    foreach(DataGridViewCell cell in row.Cells)
                    {
                        if (cell.OwningColumn.Name == ZStringConstants.ParameterIDZo_TopReference ||
                            cell.OwningColumn.Name == ZStringConstants.ParameterIDZdiff_TopReference)
                        {
                            if (cell.Value == null || cell.Value.ToString() == string.Empty)
                            { 
                                ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                                if (layerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value)) cell.Value = Stackup.GetLayerOfStackup(layerParameter.Value);
                            }

                            (cell as ZDataGridViewComboBoxCell).Items.Clear();
                            (cell as ZDataGridViewComboBoxCell).Items.AddRange(references.refsUp.ToArray());
                            if (!references.refsUp.Contains(cell.Value as ZLayer)) cell.Value = references.defUp;
                        }

                        if (cell.OwningColumn.Name == ZStringConstants.ParameterIDZo_BottomReference ||
                            cell.OwningColumn.Name == ZStringConstants.ParameterIDZdiff_BottomReference)
                        {
                            if (cell.Value == null || cell.Value.ToString() == string.Empty)
                            {
                                ZLayerParameter layerParameter = cell.Tag as ZLayerParameter;
                                if (layerParameter != null && !string.IsNullOrWhiteSpace(layerParameter.Value)) cell.Value = Stackup.Layers.Find(x => x.ID == layerParameter.Value);
                            }

                            (cell as ZDataGridViewComboBoxCell).Items.Clear();
                            (cell as ZDataGridViewComboBoxCell).Items.AddRange(references.refsDown.ToArray());
                            if (!references.refsDown.Contains(cell.Value as ZLayer)) cell.Value = references.defDown;
                        }
                    }
                }
                else 
                {
                    foreach (DataGridViewCell cell in row.Cells)
                    {
                        if (cell.OwningColumn.Name == ZStringConstants.ParameterIDZo_TopReference ||
                            cell.OwningColumn.Name == ZStringConstants.ParameterIDZdiff_TopReference ||
                            cell.OwningColumn.Name == ZStringConstants.ParameterIDZo_BottomReference ||
                            cell.OwningColumn.Name == ZStringConstants.ParameterIDZdiff_BottomReference)
                        {
                            cell.Value = string.Empty;
                        }
                    }
                }
            }
        }
    }
}
