using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerViaSpanPanel : Form
    {
        ZList<ZSpan> viaSpans;
        List<ZLayer> copperLayers;
        Control curentEditingControl = null;

        public ZPlannerViaSpanPanel()
        {
            InitializeComponent();
            viaSpanGridView.EditMode = DataGridViewEditMode.EditOnKeystrokeOrF2;
            this.Load += ZPlannerViaSpanPanel_Load;
        }

        void ZPlannerViaSpanPanel_Load(object sender, EventArgs e)
        {
            LoadData();
        }

        internal void LoadData()
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            cbShowVias.Checked = ZPlannerManager.IsViasVisible;
            this.FormClosed += ZPlannerViaSpanPanel_FormClosed;
            this.viaSpans = new ZList<ZSpan>(ZPlannerManager.Project.Stackup);

            foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
            {
                this.viaSpans.Add(span.Clone(true));
            }

            for ( int i = 0; i < Options.TheOptions.NumberOfDisplayedViaSpans; ++i )
            {
                bool isFirst = (this.viaSpans.Count == 0);
                ZSpan span = new ZSpan(ZPlannerManager.Project.Stackup);
                span.Title = GenerateSpanTitle();
                if (isFirst) span.IsEnable = true;
                this.viaSpans.Add(span);
            }

            this.copperLayers = ZPlannerManager.Project.Stackup.GetMetallLayerCollection();
            FillGridView();
            FormatGridView();

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);

            viaSpans.CollectionChanged += Spans_CollectionChanged;
            this.viaSpanGridView.CellValueChanged += viaSpanGridView_CellValueChanged;
            this.viaSpanGridView.CellEndEdit += viaSpanGridView_CellEndEdit;
            this.viaSpanGridView.EditingControlShowing += viaSpanGridView_EditingControlShowing;
            this.viaSpanGridView.CurrentCellChanged += DataGridView_CurrentCellChanged;
        }

        void DataGridView_CurrentCellChanged(object sender, EventArgs e)
        {
            ZDataGridView grid = sender as ZDataGridView;
            if (grid == null) return;

            Rectangle headersRectangle = grid.DisplayRectangle;
            headersRectangle.Height = grid.ColumnHeadersHeight;
            grid.Invalidate(headersRectangle);
        }

        internal void FormatGridView()
        {
            Font font = new Font(viaSpanGridView.DefaultCellStyle.Font, FontStyle.Bold);
            foreach (ZDataGridViewRow row in viaSpanGridView.Rows)
            {
                foreach (DataGridViewCell cell in row.Cells)
                {
                    if (cell.ReadOnly) { cell.Style.ForeColor = ZColorConstants.CalculatedCellColor; cell.Style.Font = font; }
                    else { cell.Style.ForeColor = Color.Empty; cell.Style.Font = null; }
                }
            }
        }

        void ZPlannerViaSpanPanel_FormClosed(object sender, FormClosedEventArgs e)
        {
            Clear();
        }

        private void Clear()
        {
            cbShowVias.Checked = false;
            this.viaSpanGridView.CurrentCellChanged -= DataGridView_CurrentCellChanged;
            this.viaSpanGridView.CellValueChanged -= viaSpanGridView_CellValueChanged;
            this.viaSpanGridView.CellEndEdit -= viaSpanGridView_CellEndEdit;
            this.viaSpanGridView.EditingControlShowing -= viaSpanGridView_EditingControlShowing;
            viaSpans.CollectionChanged -= Spans_CollectionChanged;

            foreach(ZDataGridViewRow row in viaSpanGridView.Rows)
            {
                ZSpan span = row.Tag as ZSpan;
                if (span != null)
                    span.PropertyChanged -= row.ZSpan_PropertyChanged;
            }

            this.viaSpanGridView.Rows.Clear();
            this.viaSpanGridView.Columns.Clear();
        }

        private void FillGridView()
        {
            

            // Add Columns.
            AddColumns();

            // Add Rows and Cells.
            AddRows();

            RecalculateColumnHeaderName();
        }

        void RecalculateColumnHeaderName()
        {
            int index = 0;

            foreach (ZDataGridViewColumn column in viaSpanGridView.GetColumnsOrderedByDisplayIndex())
            {
                ZParameter parameter = column.Tag as ZParameter;
                column.HeaderName = column.GetHeaderNameByOrder(index++);
            }
        }

        private void AddColumns()
        {
            int index = 0;
            Dictionary<string, ZValueType> parameters = new Dictionary<string, ZValueType>();
            parameters.Add("Enable", ZValueType.Boolean);
            parameters.Add("FirstLayer", ZValueType.Select);
            parameters.Add("LastLayer", ZValueType.Select);
            parameters.Add("Size", ZValueType.Number);
            parameters.Add("AspectRatio", ZValueType.Ratio);
            parameters.Add("Filled", ZValueType.Select);
            parameters.Add("BackDrilled", ZValueType.Boolean);

            // Add Columns.
            foreach (string parameterName in parameters.Keys)
            {
                ZDataGridViewColumn column;
                switch (parameters[parameterName])
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
                    case ZValueType.Ratio:
                        column = new ZDataGridViewRatioBoxColumn();
                        column.ReadOnly = true;
                        break;
                    case ZValueType.Boolean:
                        column = new ZDataGridViewCheckBoxWithTextColumn();
                        break;
                    case ZValueType.Select:
                        List<string> availableValues = new List<string>();
                        if (parameterName == "Filled")
                        {
                            SelectList selectList = ZPlannerManager.Project.Lists.Find(x => x.ID == "ViaSpanFilledList");
                            if (selectList != null) availableValues = selectList.GetValues();
                        }
                        else
                        {
                            if (copperLayers != null)
                                foreach (ZLayer layer in copperLayers)
                                    availableValues.Add(layer.GetLayerNumber());
                        }
                        if (availableValues.Count == 0)  availableValues.Add("");

                        ZDataGridViewComboBoxColumn comboBoxColumn = new ZDataGridViewComboBoxColumn(availableValues.ToArray());
                        column = comboBoxColumn;
                        break;
                    default:
                        column = new ZDataGridViewTextBoxColumn();
                        break;
                }

                //column.DefaultCellStyle // TBD
                column.Resizable = DataGridViewTriState.True;

                column.Name = parameterName;
                column.HeaderText = ZStringConstants.TitleDictionary[parameterName];
                column.ReadOnly = false;
                column.DisplayIndex = index++;
                column.Frozen = false;
                column.ToolTipText = column.HeaderText;
                column.Tag = parameters[parameterName];
                column.Visible = true;

                this.viaSpanGridView.Columns.Add(column);
            }
        }

        private void AddRows()
        {
            List<ZDataGridViewRow> rowList = new List<ZDataGridViewRow>();
            foreach (var span in viaSpans)
            {
                ZDataGridViewRow row = CreateRow(span);
                if (row != null) rowList.Add(row);
            }

            if (rowList.Count > 0)
            {
                viaSpanGridView.SuspendLayout();
                this.viaSpanGridView.Rows.AddRange(rowList.ToArray());
                viaSpanGridView.ResumeLayout();
            }
        }

        private ZDataGridViewRow CreateRow(ZSpan span)
        {
            ZDataGridViewRow row = new ZDataGridViewRow();
            //row.DefaultCellStyle // TBD
            row.Resizable = DataGridViewTriState.True;
            row.Name = span.ID;
            row.Frozen = false;
            row.ReadOnly = false;
            row.Visible = true;
            row.Tag = span;
            row.HeaderCell = new ZDataGridViewRowHeaderCell();
            //row.HeaderCell.Value = (material.Order + 1).ToString();

            foreach (DataGridViewColumn column in this.viaSpanGridView.Columns)
            {
                DataGridViewCell cell;
                switch ((ZValueType)column.Tag)
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
                    case ZValueType.Ratio:
                        cell = new ZDataGridViewRatioBoxCell();
                        break;
                    case ZValueType.Boolean:
                        cell = new ZDataGridViewCheckBoxWithTextCell();
                        break;
                    case ZValueType.Select:
                        List<string> availableValues = new List<string>();
                        if (column.Name == "Filled")
                        {
                            SelectList selectList = ZPlannerManager.Project.Lists.Find(x => x.ID == "ViaSpanFilledList");
                            if (selectList != null) availableValues = selectList.GetValues();
                        }
                        else
                        {
                            if (copperLayers != null)
                                foreach (ZLayer layer in copperLayers)
                                    availableValues.Add(layer.GetLayerNumber());
                        }

                        if (availableValues.Count == 0)  availableValues.Add("");

                        ZDataGridViewComboBoxCell comboBoxCell = new ZDataGridViewComboBoxCell(availableValues.ToArray());
                        cell = comboBoxCell;
                        cell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
                        break;
                    default:
                        cell = new ZDataGridViewTextBoxCell();
                        break;
                }

                row.Cells.Add(cell);

                switch (column.Name)
                {
                    case "Enable":
                        cell.Value = span.IsEnable.ToString();
                        break;
                    case "FirstLayer":
                        cell.Value = span.FirstLayer.GetLayerNumber();
                        break;
                    case "LastLayer":
                        cell.Value = span.LastLayer.GetLayerNumber();
                        break;
                    case "Size":
                        cell.Value = span.Size.ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
                        break;
                    case "AspectRatio":
                        cell.Value = span.AspectRatio.ToString();
                        cell.ReadOnly = true;
                        break;
                    case "Filled":
                        cell.Value = span.Filled;
                        break;
                    case "BackDrilled":
                        cell.Value = span.IsBackDrilled.ToString();
                        break;
                    default:
                        cell.Value = "";
                        break;
                }
            }

            span.PropertyChanged += row.ZSpan_PropertyChanged;

            return row;
        }

        private void viaSpanGridView_EditingControlShowing(object sender, DataGridViewEditingControlShowingEventArgs e)
        {
            if (e.Control.GetType() == typeof(ZDataGridViewSelectBoxEditingControl))
            {
                ZDataGridViewSelectBoxEditingControl control = e.Control as ZDataGridViewSelectBoxEditingControl;
                ZDataGridViewSelectBoxCell cell = viaSpanGridView.CurrentCell as ZDataGridViewSelectBoxCell;
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
                ZDataGridViewComboBoxCell cell = viaSpanGridView.CurrentCell as ZDataGridViewComboBoxCell;
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
                control.Mask = "0.9 : 1";
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

        void viaSpanGridView_CellValueChanged(object sender, DataGridViewCellEventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

            ZSpan span = viaSpanGridView.Rows[e.RowIndex].Tag as ZSpan;
            DataGridViewCell cell = viaSpanGridView.Rows[e.RowIndex].Cells[e.ColumnIndex];
            if (span == null || cell == null) return;
            string value = (cell.Value != null) ? cell.Value.ToString() : null;
            if (value == null && viaSpanGridView.Columns[e.ColumnIndex].Name != "Filled") return;
            switch (viaSpanGridView.Columns[e.ColumnIndex].Name)
            {
                case "Enable":
                    bool boolVal;
                    if (bool.TryParse(value, out boolVal))
                        span.IsEnable = boolVal;
                    break;
                case "FirstLayer":
                    ZLayer layer = ZPlannerManager.Project.Stackup.Layers.Find(x => x.GetLayerNumber() == value);
                    if (layer != null)
                        span.FirstLayer = layer;
                    string realValue = span.FirstLayer.GetLayerNumber();
                    if (value != realValue) cell.Value = realValue;
                    break;
                case "LastLayer":
                    layer = ZPlannerManager.Project.Stackup.Layers.Find(x => x.GetLayerNumber() == value);
                    if (layer != null)
                        span.LastLayer = layer;
                    realValue = span.LastLayer.GetLayerNumber();
                    if (value != realValue) cell.Value = realValue;
                    break;
                case "Size":
                    double doubleVal;
                    if (double.TryParse(value.ToString(), NumberStyles.Any, CultureInfo.InvariantCulture, out doubleVal))
                        span.Size = doubleVal;
                    break;
                case "AspectRatio":
                    if (double.TryParse(value.ToString().Replace(" : 1", "").Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out doubleVal))
                        span.AspectRatio = doubleVal;
                    break;
                case "Filled":
                    span.Filled = value;
                    break;
                case "BackDrilled":
                    if (bool.TryParse(value, out boolVal))
                        span.IsBackDrilled = boolVal;
                    break;
                default:
                    break;
            }

            ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
        }

        void Spans_CollectionChanged(object sender, System.Collections.Specialized.NotifyCollectionChangedEventArgs e)
        {
            viaSpanGridView.ClearSelection();
            int index;
            switch (e.Action)
            {
                case NotifyCollectionChangedAction.Add:
                    index = e.NewStartingIndex;
                    if (e.NewItems == null || e.NewItems.Count == 0) break;
                    if (index == -1)
                    {
                        foreach (ZSpan span in e.NewItems)
                        {
                            ZDataGridViewRow row = CreateRow(span);
                            if (row != null)
                            {
                                viaSpanGridView.Rows.Add(row);
                                row.Selected = true;
                            }
                        }
                    }
                    else
                    {
                        foreach (ZSpan span in e.NewItems)
                        {
                            ZDataGridViewRow row = CreateRow(span);
                            if (row != null) viaSpanGridView.Rows.Insert(index, row);
                            row.Selected = true;
                            ++index;
                        }
                    }
                    return;
                case NotifyCollectionChangedAction.Remove:
                    if (e.OldItems == null || e.OldItems.Count == 0) break;
                    foreach (DataGridViewRow row in viaSpanGridView.Rows)
                    {
                        index = e.OldStartingIndex;
                        ZSpan span = row.Tag as ZSpan;
                        if (span == null) continue;

                        if (e.OldItems.Contains(span))
                        {
                            viaSpanGridView.Rows.Remove(row);
                            if (index >= 0 && index < viaSpanGridView.Rows.Count) viaSpanGridView.Rows[index].Selected = true; 
                        }
                    }
                    return;
                case NotifyCollectionChangedAction.Move:
                    if (e.NewStartingIndex == -1 || e.OldStartingIndex == -1 || e.NewStartingIndex >= viaSpanGridView.Rows.Count || e.OldStartingIndex >= viaSpanGridView.Rows.Count) break;
                    if (e.NewItems == null || e.NewItems.Count == 0)
                    {
                        DataGridViewRow row1 = viaSpanGridView.Rows[e.NewStartingIndex];
                        DataGridViewRow row2 = viaSpanGridView.Rows[e.OldStartingIndex];
                        viaSpanGridView.Rows.Remove(row1);
                        viaSpanGridView.Rows.Insert(e.OldStartingIndex, row1);
                        viaSpanGridView.Rows.Remove(row2);
                        viaSpanGridView.Rows.Insert(e.NewStartingIndex, row2);
                    }
                    else
                    {
                        DataGridViewRow row = viaSpanGridView.Rows[e.OldStartingIndex];
                        viaSpanGridView.Rows.Remove(row);
                        viaSpanGridView.Rows.Insert(e.NewStartingIndex, row);
                    }
                    return;
                case NotifyCollectionChangedAction.Replace:
                    index = e.NewStartingIndex;
                    if (e.NewItems == null || e.NewItems.Count == 0 || index == -1 || index >= viaSpanGridView.Rows.Count) break;
                    else
                    {
                        viaSpanGridView.Rows.Remove(viaSpanGridView.Rows[index]);
                        ZDataGridViewRow row = CreateRow(e.NewItems[0] as ZSpan);
                        if (row != null) viaSpanGridView.Rows.Insert(index, row);
                        return;
                    }
                case NotifyCollectionChangedAction.Reset:
                    break;
            }

            Clear();
            LoadData();
        }

        void viaSpanGridView_CellEndEdit(object sender, DataGridViewCellEventArgs e)
        {
            if (curentEditingControl != null)
            {
                curentEditingControl.KeyPress -= NumberBox_KeyPress;
                curentEditingControl.KeyPress -= PercentBox_KeyPress;
                curentEditingControl.KeyPress -= RatioBox_KeyPress;
            }
        }

        private void buttonCloseDialog_Click(object sender, EventArgs e)
        {
            this.Close();
        }

        private void buttonSaveDialog_Click(object sender, EventArgs e)
        {
            bool isComplexCommandStarted = ZPlannerManager.Commands.IsComplexCommandStarted();
            if (!isComplexCommandStarted) ZPlannerManager.Commands.StartComplexCommand();

            ZPlannerManager.SuspendCollectionChangedEvent();
            List<ZSpan> currentSpansList = new List<ZSpan>();
            List<ZSpan> changedSpansList = new List<ZSpan>();

            try
            {
                ZSpan templateSpan = new ZSpan(ZPlannerManager.Project.Stackup);
                ZSpan viaSpan;

                foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
                {
                    currentSpansList.Add(span.Clone(true));
                }

                foreach(ZSpan span in this.viaSpans)
                {
                    if (span.AspectRatio == templateSpan.AspectRatio && span.FirstLayer == templateSpan.FirstLayer && span.LastLayer == templateSpan.LastLayer &&
                        span.IsBackDrilled == templateSpan.IsBackDrilled && span.IsEnable == templateSpan.IsEnable && span.Filled == templateSpan.Filled &&
                        span.Size == templateSpan.Size)
                    {
                        viaSpan = ZPlannerManager.Project.Stackup.Spans.Find(x => x.ID == span.ID);
                        if (viaSpan != null) ZPlannerManager.Project.Stackup.Spans.Remove(viaSpan);
                        continue;
                    }

                    viaSpan = ZPlannerManager.Project.Stackup.Spans.Find(x => x.ID == span.ID);
                    if(viaSpan != null)
                    {
                        viaSpan.IsEnable = span.IsEnable;
                        viaSpan.FirstLayer = span.FirstLayer;
                        viaSpan.LastLayer = span.LastLayer;
                        viaSpan.Size = span.Size;
                        viaSpan.AspectRatio = span.AspectRatio;
                        viaSpan.IsBackDrilled = span.IsBackDrilled;
                        viaSpan.Filled = span.Filled;
                    }
                    else
                    {
                        ZPlannerManager.Project.Stackup.Spans.Add(span);
                    }
                }

                foreach (ZSpan span in ZPlannerManager.Project.Stackup.Spans)
                {
                    changedSpansList.Add(span.Clone(true));
                }

                ZPlannerManager.ResumeCollectionChangedEvent();
                ZPlannerManager.Project.Stackup.Spans.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeViaSpansCommand(ZPlannerManager.Project.Stackup.Spans, currentSpansList, changedSpansList);
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                if (!isComplexCommandStarted) ZPlannerManager.Commands.FinishComplexCommand();
                ZPlannerManager.ShowVias(cbShowVias.Checked);
                this.Close();
            }
        }

        protected string GenerateSpanTitle()
        {
            string prefix = "ViaSpan";
            string newTitle = prefix + "  0";

            for (uint i = 1; i < uint.MaxValue; ++i)
            {
                if (i < 10) newTitle = prefix + "  " + i;
                else newTitle = prefix + " " + i;
                if (ZPlannerManager.Project.Stackup.Spans.Find(x => x.Title == newTitle) == null && this.viaSpans.Find(x => x.Title == newTitle) == null) break;
            }

            return newTitle;
        }
    }
}
