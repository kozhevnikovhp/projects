using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Grid
{
    public enum ZDataGridViewColumnHeaderCellBehavior : byte
    {
        DisabledHidden = 0,
        Disabled,
        SortingStandartGlyph,
        Sorting,
        SortingFiltering
    }

    public delegate void ZDataGridViewFilterEventHandler(object sender, ZDataGridViewFilterEventArgs e);

    public class ZDataGridViewFilterEventArgs : EventArgs
    {
        public ZDataGridViewFilterMenu FilterMenu { get; private set; }

        public DataGridViewColumn Column { get; private set; }

        internal ZDataGridViewColumnHeaderCell HeaderCell
        {
            get
            {
                return this.Column.HeaderCell as ZDataGridViewColumnHeaderCell;
            }
        }

        public ZDataGridViewFilterEventArgs(ZDataGridViewFilterMenu filterMenu, DataGridViewColumn column)
        {
            this.FilterMenu = filterMenu;
            this.Column = column;
        }
    }

    class ZDataGridViewColumnHeaderCell : DataGridViewColumnHeaderCell, IZDataGridViewCell
    {
        #region Fields
        private static Font headerFont = new System.Drawing.Font("Arial", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));

        protected int valueColumnSpan = 1;
        protected int valueRowSpan = 1;
        private ZDataGridViewColumnHeaderCell valueOwnerCell;

        private Size filterButtonImageSize = new Size(16, 16);
        private bool filterButtonPressed = false;
        private bool filterButtonOver = false;
        private Rectangle filterButtonOffsetBounds = Rectangle.Empty;
        private Rectangle filterButtonImageBounds = Rectangle.Empty;
        private Padding filterButtonMargin = new Padding(3, 4, 3, 4);
        private ZDataGridViewColumnHeaderCellBehavior cellBehavior;
        #endregion

        public ZDataGridViewColumnHeaderCell(DataGridViewColumnHeaderCell oldCell)
        {
            this.Tag = oldCell.Tag;
            this.ErrorText = oldCell.ErrorText;
            this.ToolTipText = oldCell.ToolTipText;
            this.Value = oldCell.Value;
            this.ValueType = oldCell.ValueType;
            this.ContextMenuStrip = oldCell.ContextMenuStrip;
            this.Style = oldCell.Style;
            Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            Style.Font = new Font("Microsoft Sans Serif", 8.25F);

            ZDataGridViewColumnHeaderCell oldZDataGridViewCell = oldCell as ZDataGridViewColumnHeaderCell;

            if (oldZDataGridViewCell != null)
            {
                if (oldZDataGridViewCell.FilterMenu != null)
                    this.FilterMenu = oldZDataGridViewCell.FilterMenu.Clone() as ZDataGridViewFilterMenu;
                /*else
                    this.FilterMenu = new ZDataGridViewFilterMenu(oldCell.OwningColumn.ValueType);*/

                this.filterButtonPressed = oldZDataGridViewCell.filterButtonPressed;
                this.filterButtonOver = oldZDataGridViewCell.filterButtonOver;
                this.filterButtonOffsetBounds = oldZDataGridViewCell.filterButtonOffsetBounds;
                this.filterButtonImageBounds = oldZDataGridViewCell.filterButtonImageBounds;
            }
            /*else 
            {
                this.FilterMenu = new ZDataGridViewFilterMenu(oldCell.OwningColumn.ValueType);
            }

            this.FilterMenu.FilterChanged += FilterMenu_FilterChanged;
            this.FilterMenu.SortChanged += FilterMenu_SortChanged;
            //this.FilterMenu.DateTimeGroupingChanged += FilterMenu_DateTimeGroupingChanged;*/
        }

        public ZDataGridViewColumnHeaderCell(DataGridViewColumnHeaderCell oldCell, ZDataGridViewColumnHeaderCellBehavior cellBehavior)
            : this(oldCell)
        {
            this.cellBehavior = cellBehavior;
        }

        #region Properties

        public string Title { get; set; }

        public string SubTitle { get; set; }

        public string HeaderName { get; set; }

        public int ColumnSpan
        {
            get { return valueColumnSpan; }
            set
            {
                if (DataGridView == null || OwnerCell != null)
                    return;
                if (value < 1 || ColumnIndex + value - 1 >= DataGridView.ColumnCount)
                    throw new System.ArgumentOutOfRangeException("value");
                if (valueColumnSpan != value)
                    SetSpan(value, RowSpan);
            }
        }

        public int RowSpan
        {
            get { return valueRowSpan; }
            set
            {
                if (DataGridView == null || OwnerCell != null)
                    return;
                if (value < 1 || RowIndex + value - 1 >= DataGridView.RowCount)
                    throw new System.ArgumentOutOfRangeException("value");
                if (valueRowSpan != value)
                    SetSpan(ColumnSpan, value);
            }
        }

        public DataGridViewCell OwnerCell
        {
            get { return valueOwnerCell; }
            set { valueOwnerCell = value as ZDataGridViewColumnHeaderCell; }
        }

        public override bool ReadOnly
        {
            get
            {
                return base.ReadOnly;
            }
            set
            {
                base.ReadOnly = value;

                if (OwnerCell == null
                    && (ColumnSpan > 1)
                    && DataGridView != null)
                {
                    for (int col = ColumnIndex; col < ColumnIndex + ColumnSpan; col++)
                        if (col != ColumnIndex)
                        {
                            DataGridView.Columns[col].HeaderCell.ReadOnly = value;
                        }
                }
            }
        }

        #endregion

        #region Painting.

        protected override void Paint(Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
        {
            ZParameter parameter = (OwningColumn == null) ? null : OwningColumn.Tag as ZParameter;
            var ownerCell = this;

            //Style.Alignment = DataGridViewContentAlignment.MiddleCenter;
            //cellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
            //cellStyle.Font = new Font(cellStyle.Font.FontFamily, 12);

            if (OwnerCell != null && OwnerCell.DataGridView == null)
                OwnerCell = null; //owner cell was removed.

            var headerNameCellBounds = new Rectangle(cellBounds.X, cellBounds.Y, cellBounds.Width, 0);
            var headerNameClipBounds = new Rectangle(clipBounds.X, clipBounds.Y, clipBounds.Width, headerNameCellBounds.Height);

            if ((this.DataGridView as ZDataGridView).ColumnHeaderNamesVisible)
            {
                // Paint header name.
                var formattedHeaderName = ownerCell.GetFormattedValue(HeaderName,
                        rowIndex, ref cellStyle, null, null, DataGridViewDataErrorContexts.Display);
                headerNameCellBounds = new Rectangle(cellBounds.X, cellBounds.Y, cellBounds.Width, 24);
                headerNameClipBounds = new Rectangle(clipBounds.X, clipBounds.Y, clipBounds.Width, headerNameCellBounds.Height);

                using (var g = DataGridView.CreateGraphics())
                {
                    g.SetClip(headerNameClipBounds);
                    //Paint the content.
                    this.NativePaint(parameter, g, headerNameClipBounds, headerNameCellBounds, rowIndex, cellState, HeaderName, formattedHeaderName, errorText, cellStyle, advancedBorderStyle, paintParts & ~DataGridViewPaintParts.Border);
                    //Paint the borders.
                    if ((paintParts & DataGridViewPaintParts.Border) != DataGridViewPaintParts.None)
                    {
                        var advancedBorderStyleForHeader = new DataGridViewAdvancedBorderStyle
                        {
                            Left = DataGridViewAdvancedCellBorderStyle.Outset,
                            Top = DataGridViewAdvancedCellBorderStyle.OutsetDouble,
                            Right = DataGridViewAdvancedCellBorderStyle.Inset,
                            Bottom = DataGridViewAdvancedCellBorderStyle.Inset
                        };
                        this.PaintBorder(g, headerNameClipBounds, headerNameCellBounds, cellStyle, advancedBorderStyleForHeader);
                    }
                }
            }

            // Paint NotSpanned cell
            if (DataGridView == null
                || (OwnerCell == null && ColumnSpan == 1 && RowSpan == 1 && !(this.OwningColumn is ZDataGridViewViaSpanColumn)))
            {
                graphics.SetClip(clipBounds);

                advancedBorderStyle = new DataGridViewAdvancedBorderStyle
                {
                    Left = DataGridViewAdvancedCellBorderStyle.Outset,
                    Top = DataGridViewAdvancedCellBorderStyle.Outset,
                    Right = DataGridViewAdvancedCellBorderStyle.Inset,
                    Bottom = DataGridViewAdvancedCellBorderStyle.Outset
                };

                var valueCellBounds = new Rectangle(cellBounds.X, cellBounds.Y + headerNameCellBounds.Height, cellBounds.Width, cellBounds.Height - headerNameCellBounds.Height);
                var valueClipBounds = new Rectangle(clipBounds.X, clipBounds.Y + headerNameClipBounds.Height, clipBounds.Width, clipBounds.Height - headerNameClipBounds.Height);

                NativePaintWithFiltering(parameter, graphics, valueClipBounds, valueCellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);
                return;
            }

            var columnIndex = ColumnIndex;
            var columnSpan = ColumnSpan;
            var rowSpan = RowSpan;
            var title = Title;
            var formattedTitle = ownerCell.GetFormattedValue(title,
                    rowIndex, ref cellStyle, null, null, DataGridViewDataErrorContexts.Display);
            var formattedSubTitle = ownerCell.GetFormattedValue(SubTitle,
                    rowIndex, ref cellStyle, null, null, DataGridViewDataErrorContexts.Display);
            if (valueOwnerCell != null)
            {
                ownerCell = valueOwnerCell;
                columnIndex = ownerCell.ColumnIndex;
                rowIndex = ownerCell.RowIndex;
                columnSpan = ownerCell.ColumnSpan;
                rowSpan = ownerCell.RowSpan;
                //value = ownerCell.GetValue(rowIndex);
                title = ownerCell.Title;
                errorText = ownerCell.GetErrorText(rowIndex);
                cellState = ownerCell.State;
                cellStyle = ownerCell.GetInheritedStyle(null, rowIndex, true);
                formattedTitle = ownerCell.GetFormattedValue(title,
                    rowIndex, ref cellStyle, null, null, DataGridViewDataErrorContexts.Display);
            }
            if (CellsRegionContainsSelectedCell(columnIndex, rowIndex, columnSpan, rowSpan))
                cellState |= DataGridViewElementStates.Selected;

            // Paint title.
            var titleCellBounds = ZDataGridView.GetSpannedColumnHeaderCellBoundsFromChildCellBounds(
                this,
                cellBounds,
                ZDataGridView.SingleVerticalHeaderBorderAdded(DataGridView),
                ZDataGridView.SingleHorizontalHeaderBorderAdded(DataGridView));
            titleCellBounds.Y += headerNameCellBounds.Height;
            titleCellBounds.Height -= headerNameCellBounds.Height;
            titleCellBounds.Height = (int)(titleCellBounds.Height * 0.3);
            var titleClipBounds = ZDataGridView.GetSpannedColumnHeaderCellClipBounds(ownerCell, titleCellBounds,
                ZDataGridView.SingleVerticalHeaderBorderAdded(DataGridView),
                ZDataGridView.SingleHorizontalHeaderBorderAdded(DataGridView));
            //using (var g = DataGridView.CreateGraphics())
            {
                Graphics g = graphics;
                g.SetClip(titleClipBounds);
                //Paint the content.
                advancedBorderStyle = ZDataGridView.AdjustHeaderBorderStyle(ownerCell);
                ownerCell.OwningColumn.SortMode = DataGridViewColumnSortMode.NotSortable;
                cellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
                ownerCell.NativePaint(parameter, g, titleClipBounds, titleCellBounds, rowIndex, cellState, title, formattedTitle, errorText, cellStyle, advancedBorderStyle, paintParts & ~DataGridViewPaintParts.Border);
                cellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
                ownerCell.OwningColumn.SortMode = DataGridViewColumnSortMode.Programmatic;
                //Paint the borders.
                if ((paintParts & DataGridViewPaintParts.Border) != DataGridViewPaintParts.None)
                {
                    var advancedBorderStyleForHeader = new DataGridViewAdvancedBorderStyle
                    {
                        Left = DataGridViewAdvancedCellBorderStyle.Outset,
                        Top = DataGridViewAdvancedCellBorderStyle.Outset,
                        Right = DataGridViewAdvancedCellBorderStyle.Inset,
                        Bottom = DataGridViewAdvancedCellBorderStyle.Inset
                    };
                    ownerCell.PaintBorder(g, titleClipBounds, titleCellBounds, cellStyle, advancedBorderStyleForHeader);
                }
            }

            // Paint subtitle.
            var subtitleCellBounds = new Rectangle(cellBounds.X, cellBounds.Y + titleCellBounds.Height + headerNameCellBounds.Height, cellBounds.Width, cellBounds.Height - titleCellBounds.Height - headerNameCellBounds.Height);
            var subtitleClipBounds = new Rectangle(clipBounds.X, clipBounds.Y + titleCellBounds.Height + headerNameClipBounds.Height, clipBounds.Width, clipBounds.Height - titleCellBounds.Height - headerNameClipBounds.Height);

            //using (var g = DataGridView.CreateGraphics())
            {
                Graphics g = graphics;
                g.SetClip(subtitleClipBounds);
                //Paint the content.
                this.NativePaintWithFiltering(parameter, g, subtitleClipBounds, subtitleCellBounds, rowIndex, cellState, SubTitle, formattedSubTitle, errorText, cellStyle, advancedBorderStyle, paintParts & ~DataGridViewPaintParts.Border);
                //Paint the borders.
                if ((paintParts & DataGridViewPaintParts.Border) != DataGridViewPaintParts.None)
                {
                    var advancedBorderStyleForHeader = new DataGridViewAdvancedBorderStyle
                    {
                        Left = DataGridViewAdvancedCellBorderStyle.Outset,
                        Top = DataGridViewAdvancedCellBorderStyle.Outset,
                        Right = DataGridViewAdvancedCellBorderStyle.Inset,
                        Bottom = DataGridViewAdvancedCellBorderStyle.Outset
                    };
                    this.PaintBorder(g, subtitleClipBounds, subtitleCellBounds, cellStyle, advancedBorderStyleForHeader);
                }
            }
        }

        protected void NativePaintWithFiltering(ZParameter parameter, Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
        {
            // For Filtering
            if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingStandartGlyph)
            {
                switch (this.ActiveSortType)
                {
                    case ZDataGridViewSortType.ASC:
                        this.SortGlyphDirection = SortOrder.Ascending;
                        break;
                    case ZDataGridViewSortType.DESC:
                        this.SortGlyphDirection = SortOrder.Descending;
                        break;
                    case ZDataGridViewSortType.None:
                        this.SortGlyphDirection = SortOrder.None;
                        break;
                }
            }
            else
                this.SortGlyphDirection = SortOrder.None;

            // Paint
            NativePaint(parameter, graphics, clipBounds, cellBounds, rowIndex, cellState, value, formattedValue, errorText, cellStyle, advancedBorderStyle, paintParts);

            // For Filtering
            if (this.CellBehavior != ZDataGridViewColumnHeaderCellBehavior.SortingStandartGlyph && this.CellBehavior != ZDataGridViewColumnHeaderCellBehavior.DisabledHidden && paintParts.HasFlag(DataGridViewPaintParts.ContentBackground))
            {
                Image filterImage = global::ZZero.ZPlanner.Properties.Resources.FilterButton4;

                if (this.cellBehavior == ZDataGridViewColumnHeaderCellBehavior.Sorting || this.ActiveFilterType == ZDataGridViewFilterType.None)
                    switch (this.ActiveSortType)
                    {
                        case ZDataGridViewSortType.ASC:
                            filterImage = global::ZZero.ZPlanner.Properties.Resources.ASC;
                            break;
                        case ZDataGridViewSortType.DESC:
                            filterImage = global::ZZero.ZPlanner.Properties.Resources.DESC;
                            break;
                        case ZDataGridViewSortType.None:
                            filterImage = global::ZZero.ZPlanner.Properties.Resources.FilterButton4;
                            break;
                    }
                else
                    switch (this.ActiveSortType)
                    {
                        case ZDataGridViewSortType.ASC:
                            filterImage = Properties.Resources.FilterASC;
                            break;
                        case ZDataGridViewSortType.DESC:
                            filterImage = Properties.Resources.FilterDESC;
                            break;
                        case ZDataGridViewSortType.None:
                            filterImage = global::ZZero.ZPlanner.Properties.Resources.ColumnFilter4;
                            break;
                    }

                this.filterButtonOffsetBounds = this.GetFilterButtonBounds(true);
                this.filterButtonImageBounds = this.GetFilterButtonBounds(false);
                Rectangle buttonBounds = this.filterButtonOffsetBounds;
                if (buttonBounds != null && clipBounds.IntersectsWith(buttonBounds))
                {
                    ControlPaint.DrawBorder(graphics, buttonBounds, Color.Gray, ButtonBorderStyle.Solid);
                    buttonBounds.Inflate(-1, -1);
                    using (Brush b = new SolidBrush((this.CellBehavior != ZDataGridViewColumnHeaderCellBehavior.Disabled && this.filterButtonOver) ? Color.LightGray : Color.White))
                        graphics.FillRectangle(b, buttonBounds);

                    graphics.DrawImage(filterImage, buttonBounds);
                }
            }
        }

        protected void NativePaint(ZParameter parameter, Graphics graphics, Rectangle clipBounds, Rectangle cellBounds, int rowIndex, DataGridViewElementStates cellState, object value, object formattedValue, string errorText, DataGridViewCellStyle cellStyle, DataGridViewAdvancedBorderStyle advancedBorderStyle, DataGridViewPaintParts paintParts)
        {
            object sValue = (parameter != null && parameter.IsCollapsed) ? "►" : value;
            object sFormatedValue = (parameter != null && parameter.IsCollapsed) ? "►" : formattedValue;

            cellStyle.Font = headerFont;

            if (this.DataGridView.CurrentCell != null && this.DataGridView.CurrentCell.OwningColumn == this.OwningColumn)
            {
                cellState = DataGridViewElementStates.Selected;
            }

            //
            // Paint
            if ((paintParts & DataGridViewPaintParts.Background) != DataGridViewPaintParts.None)
            {
                OwningColumn.SortMode = DataGridViewColumnSortMode.Programmatic;
                base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, sValue, sFormatedValue, errorText, cellStyle, advancedBorderStyle, DataGridViewPaintParts.Background);
            }

            OwningColumn.SortMode = DataGridViewColumnSortMode.NotSortable;
            base.Paint(graphics, clipBounds, cellBounds, rowIndex, cellState, sValue, sFormatedValue, errorText, cellStyle, advancedBorderStyle, paintParts & ~DataGridViewPaintParts.Background);
            OwningColumn.SortMode = DataGridViewColumnSortMode.Programmatic;
        }

        #endregion
        #region Spanning.

        protected void SetSpan(int columnSpan, int rowSpan)
        {
            int prevColumnSpan = ColumnSpan;
            int prevRowSpan = RowSpan;
            valueColumnSpan = columnSpan;
            valueRowSpan = rowSpan;

            if (DataGridView != null)
            {
                // clear.
                for (int columnIndex = ColumnIndex; columnIndex < ColumnIndex + prevColumnSpan; columnIndex++)
                {
                    var cell = DataGridView.Columns[columnIndex].HeaderCell as IZDataGridViewCell;
                    if (cell != null)
                        cell.OwnerCell = null;
                }

                // set.
               for (int columnIndex = ColumnIndex; columnIndex < ColumnIndex + ColumnSpan; columnIndex++)
                {
                    var cell = DataGridView.Columns[columnIndex].HeaderCell as IZDataGridViewCell;
                    if (cell != null && cell != this)
                    {
                        if (cell.ColumnSpan > 1) cell.ColumnSpan = 1;
                        if (cell.RowSpan > 1) cell.RowSpan = 1;
                        cell.OwnerCell = this;
                    }
                }

                OwnerCell = null;
                DataGridView.Invalidate();
            }
        }

        #endregion

        #region Editing.

        public override Rectangle PositionEditingPanel(Rectangle cellBounds, Rectangle cellClip, DataGridViewCellStyle cellStyle, bool singleVerticalBorderAdded, bool singleHorizontalBorderAdded, bool isFirstDisplayedColumn, bool isFirstDisplayedRow)
        {
            if (OwnerCell == null
                && ColumnSpan == 1 && RowSpan == 1)
            {
                return base.PositionEditingPanel(cellBounds, cellClip, cellStyle, singleVerticalBorderAdded, singleHorizontalBorderAdded, isFirstDisplayedColumn, isFirstDisplayedRow);
            }

            var ownerCell = this;
            if (valueOwnerCell != null)
            {
                var rowIndex = valueOwnerCell.RowIndex;
                cellStyle = valueOwnerCell.GetInheritedStyle(null, rowIndex, true);
                valueOwnerCell.GetFormattedValue(valueOwnerCell.Value, rowIndex, ref cellStyle, null, null, DataGridViewDataErrorContexts.Formatting);
                var editingControl = DataGridView.EditingControl as IDataGridViewEditingControl;
                if (editingControl != null)
                {
                    editingControl.ApplyCellStyleToEditingControl(cellStyle);
                    var editingPanel = DataGridView.EditingControl.Parent;
                    if (editingPanel != null)
                        editingPanel.BackColor = cellStyle.BackColor;
                }
                ownerCell = valueOwnerCell;
            }
            cellBounds = ZDataGridView.GetSpannedColumnHeaderCellBoundsFromChildCellBounds(
                this,
                cellBounds,
                singleVerticalBorderAdded,
                singleHorizontalBorderAdded);
            cellClip = ZDataGridView.GetSpannedColumnHeaderCellClipBounds(ownerCell, cellBounds, singleVerticalBorderAdded, singleHorizontalBorderAdded);
            return base.PositionEditingPanel(
                 cellBounds, cellClip, cellStyle,
                 singleVerticalBorderAdded,
                 singleHorizontalBorderAdded,
                 ZDataGridView.InFirstDisplayedColumn(ownerCell),
                 ZDataGridView.InFirstDisplayedRow(ownerCell));
        }

        protected override object GetValue(int rowIndex)
        {
            if (OwnerCell != null)
                return valueOwnerCell.GetValue(OwnerCell.RowIndex);
            return base.GetValue(rowIndex);
        }

        protected override bool SetValue(int rowIndex, object value)
        {
            if (OwnerCell != null)
                return valueOwnerCell.SetValue(OwnerCell.RowIndex, value);
            return base.SetValue(rowIndex, value);
        }

        #endregion

        #region Filtering.

        public ZDataGridViewFilterMenuDateTimeGrouping DateTimeGrouping
        {
            get
            {
                return this.FilterMenu.DateTimeGrouping;
            }
            set
            {
                if (this.DateTimeGrouping != value)
                {
                    this.FilterMenu.DateTimeGrouping = value;
                }
            }
        }

        private ZDataGridViewFilterMenu filterMenu;
        public ZDataGridViewFilterMenu FilterMenu 
        { 
            get 
            {
                if (filterMenu == null && this.CellBehavior != ZDataGridViewColumnHeaderCellBehavior.Disabled && this.CellBehavior != ZDataGridViewColumnHeaderCellBehavior.DisabledHidden)
                {
                    //this.FilterMenu = new ZDataGridViewFilterMenu(OwningColumn.ValueType);
                    this.FilterMenu = new ZDataGridViewFilterMenu((OwningColumn as ZDataGridViewColumn).DataType);

                    this.FilterMenu.FilterChanged += FilterMenu_FilterChanged;
                    this.FilterMenu.SortChanged += FilterMenu_SortChanged;
                }

                return filterMenu;
            }

            private set 
            {
                if (filterMenu != value) filterMenu = value;
            }
        }

        public bool FilterMenuIsNotInitialized { get { return filterMenu == null || !filterMenu.Initialized; } }

        public event ZDataGridViewFilterEventHandler FilterPopup = delegate { };

        public event ZDataGridViewFilterEventHandler SortChanged = delegate { };

        public event ZDataGridViewFilterEventHandler FilterChanged = delegate { };

        public event ZDataGridViewFilterEventHandler CellBehaviorChanged = delegate { };

        public event ZDataGridViewFilterEventHandler DateTimeGroupingChanged = delegate { };

        public Size MinimumSize
        {
            get
            {
                return new Size(this.filterButtonImageSize.Width + this.filterButtonMargin.Left + this.filterButtonMargin.Right,
                    this.filterButtonImageSize.Height + this.filterButtonMargin.Bottom + this.filterButtonMargin.Top);
            }
        }

        public ZDataGridViewSortType ActiveSortType
        {
            get
            {
                return this.FilterMenu.ActiveSortType;
            }
        }

        public ZDataGridViewFilterType ActiveFilterType
        {
            get
            {
                return this.FilterMenu.ActiveFilterType;
            }
        }

        public string SortString
        {
            get
            {
                return this.FilterMenu.SortString;
            }
        }

        public string FilterString
        {
            get
            {
                return this.FilterMenu.FilterString;
            }
        }

        public ZDataGridViewColumnHeaderCellBehavior CellBehavior
        {
            get
            {
                return this.cellBehavior;
            }
            set
            {
                if (value != this.CellBehavior)
                {
                    this.cellBehavior = value;
                    this.RepaintCell();
                    this.CellBehaviorChanged(this, new ZDataGridViewFilterEventArgs(this.filterMenu, this.OwningColumn));
                }
            }
        }

        protected override void Dispose(bool disposing)
        {
            if (this.filterMenu != null)
            {
                this.FilterMenu.FilterChanged -= FilterMenu_FilterChanged;
                this.FilterMenu.SortChanged -= FilterMenu_SortChanged;
                //this.FilterMenu.DateTimeGroupingChanged -= FilterMenu_DateTimeGroupingChanged;
                this.FilterMenu.Dispose();
                this.FilterMenu = null;
            }

            base.Dispose(disposing);
        }

        public void SetCustomFilter(string filter, string filterName = null, bool fireEvent = false)
        {
            this.FilterMenu.SetCustomFilter(filter, filterName, fireEvent);
        }

        void FilterMenu_DateTimeGroupingChanged(object sender, EventArgs e)
        {
            this.DateTimeGroupingChanged(this, new ZDataGridViewFilterEventArgs(this.FilterMenu, this.OwningColumn));
        }

        private void RepaintCell()
        {
            if (this.Displayed && this.DataGridView != null)
                this.DataGridView.InvalidateCell(this);
        }

        public void ClearSorting(bool fireEvent = false)
        {
            if (FilterMenuIsNotInitialized) return;
            this.FilterMenu.ClearSorting(fireEvent);
            this.RepaintCell();
        }

        public void SetSorting(ZDataGridViewSortType sort, bool fireEvent = false)
        {
            this.FilterMenu.SetSorting(sort, fireEvent);
            this.RepaintCell();
        }

        public void ClearFilter(bool fireEvent = false)
        {
            if (FilterMenuIsNotInitialized) return;
            this.FilterMenu.ClearFilter(fireEvent);
            this.RepaintCell();
        }

        public void SaveCurrentFilter()
        {
            this.FilterMenu.SaveCurrentFilter();
        }

        private void FilterMenu_FilterChanged(object sender, EventArgs e)
        {
            if (this.OwningColumn.Name == ZStringConstants.DMLParameterIDManufacturer)
            {
                ZDataGridViewColumn column = this.OwningColumn.DataGridView.Columns[ZStringConstants.DMLParameterIDMaterial] as ZDataGridViewColumn;
                if (column != null)
                {
                    ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null && cell.ActiveFilterType != ZDataGridViewFilterType.None)
                    {
                        cell.ClearFilter();
                    }
                }
            }

            this.RepaintCell();
            this.FilterChanged(this, new ZDataGridViewFilterEventArgs(this.FilterMenu, this.OwningColumn));

            /*if (this.OwningColumn.Name == ZStringConstants.DMLParameterIDManufacturer)
            {
                ZDataGridViewColumn column = this.OwningColumn.DataGridView.Columns[ZStringConstants.DMLParameterIDMaterial] as ZDataGridViewColumn;
                if (column != null)
                {
                    ZDataGridViewColumnHeaderCell cell = column.HeaderCell as ZDataGridViewColumnHeaderCell;
                    if (cell != null)
                    {
                        ZDataGridViewFilterMenu filterMenu = cell.FilterMenu;
                        if (filterMenu == null) return;

                        filterMenu.InitializeComponent();
                        (column.DataGridView as ZDataGridView).PrepareFilterWithoutShowing(filterMenu, column.Name);
                    }
                }
            }*/
        }

        private void FilterMenu_SortChanged(object sender, EventArgs e)
        {
            this.RepaintCell();
            this.SortChanged(this, new ZDataGridViewFilterEventArgs(this.FilterMenu, this.OwningColumn));
        }

        private Rectangle GetFilterButtonBounds(Boolean withOffset = true)
        {
            Rectangle cell = this.DataGridView.GetCellDisplayRectangle(this.ColumnIndex, -1, false);

            Point p = new Point((withOffset ? cell.Right : this.OwningColumn.Width) - this.filterButtonImageSize.Width - this.filterButtonMargin.Right,
                (withOffset ? cell.Bottom : cell.Height) - this.filterButtonImageSize.Height - this.filterButtonMargin.Bottom);

            return new Rectangle(p, this.filterButtonImageSize);
        }

        protected override void OnMouseMove(DataGridViewCellMouseEventArgs e)
        {
            if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingFiltering || this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.Sorting)
            {
                if (this.filterButtonImageBounds.Contains(e.X, e.Y) && !this.filterButtonOver)
                {
                    this.filterButtonOver = true;
                    this.RepaintCell();
                }
                else if (!this.filterButtonImageBounds.Contains(e.X, e.Y) && this.filterButtonOver)
                {
                    this.filterButtonOver = false;
                    this.RepaintCell();
                }
            }

            base.OnMouseMove(e);
        }

        protected override void OnMouseDown(DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && !this.filterButtonPressed)
            {
                if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingFiltering || this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.Sorting)
                {
                    if (this.filterButtonImageBounds.Contains(e.X, e.Y))
                    {
                        this.filterButtonPressed = true;
                        this.filterButtonOver = true;
                        this.RepaintCell();
                    }
                }
                else if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingStandartGlyph)
                {
                    this.filterButtonPressed = true;
                    this.RepaintCell();
                }
            }

            base.OnMouseDown(e);
        }

        protected override void OnMouseUp(DataGridViewCellMouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left && this.filterButtonPressed)
            {
                this.filterButtonPressed = false;

                if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingStandartGlyph)
                    switch (this.ActiveSortType)
                    {
                        case ZDataGridViewSortType.ASC:
                            this.FilterMenu.SetSorting(ZDataGridViewSortType.DESC, true);
                            break;
                        case ZDataGridViewSortType.DESC:
                            this.FilterMenu.SetSorting(ZDataGridViewSortType.None, true);
                            break;
                        case ZDataGridViewSortType.None:
                            this.FilterMenu.SetSorting(ZDataGridViewSortType.ASC, true);
                            break;
                    }
                else if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingFiltering || this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.Sorting)
                {
                    this.filterButtonOver = false;

                    if (this.filterButtonImageBounds.Contains(e.X, e.Y))
                    {
                        if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingFiltering)
                        {
                            this.FilterMenu.InitializeComponent();
                            this.FilterPopup(this, new ZDataGridViewFilterEventArgs(this.FilterMenu, this.OwningColumn));
                        }
                        else
                            switch (this.ActiveSortType)
                            {
                                case ZDataGridViewSortType.ASC:
                                    this.FilterMenu.SetSorting(ZDataGridViewSortType.DESC, true);
                                    break;
                                case ZDataGridViewSortType.DESC:
                                    this.FilterMenu.SetSorting(ZDataGridViewSortType.None, true);
                                    break;
                                case ZDataGridViewSortType.None:
                                    this.FilterMenu.SetSorting(ZDataGridViewSortType.ASC, true);
                                    break;
                            }
                    }
                }
                this.RepaintCell();
            }

            base.OnMouseUp(e);
        }

        protected override void OnMouseLeave(int rowIndex)
        {
            if (this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.SortingFiltering || this.CellBehavior == ZDataGridViewColumnHeaderCellBehavior.Sorting)
            {
                this.filterButtonOver = false;
                this.RepaintCell();
            }

            base.OnMouseLeave(rowIndex);
        }

        #endregion

        public void ZLayerParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
        }

        public void ZMaterialParameter_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
        }

        #region Other overridden

        protected override void OnDataGridViewChanged()
        {
            base.OnDataGridViewChanged();

            if (DataGridView == null)
            {
                ColumnSpan = 1;
                RowSpan = 1;
            }
        }

        protected override Rectangle BorderWidths(DataGridViewAdvancedBorderStyle advancedBorderStyle)
        {
            if (OwnerCell == null
                && ColumnSpan == 1 && RowSpan == 1)
            {
                return base.BorderWidths(advancedBorderStyle);
            }

            if (OwnerCell != null)
                return valueOwnerCell.BorderWidths(advancedBorderStyle);

            var leftTop = base.BorderWidths(advancedBorderStyle);
            var rightBottomCell = DataGridView.Columns[
                ColumnIndex + ColumnSpan - 1].HeaderCell as ZDataGridViewColumnHeaderCell;
            var rightBottom = rightBottomCell != null
                ? rightBottomCell.NativeBorderWidths(advancedBorderStyle)
                : leftTop;
            return new Rectangle(leftTop.X, leftTop.Y, rightBottom.Width, rightBottom.Height);
        }

        protected Rectangle NativeBorderWidths(DataGridViewAdvancedBorderStyle advancedBorderStyle)
        {
            return base.BorderWidths(advancedBorderStyle);
        }

        protected override Size GetPreferredSize(Graphics graphics, DataGridViewCellStyle cellStyle, int rowIndex, Size constraintSize)
        {
            //if (OwnerCell != null) 
            //    return new Size(0, 0);
            Size size = base.GetPreferredSize(graphics, cellStyle, rowIndex, constraintSize);
            DataGridView grid = DataGridView;
            int width = size.Width;
            //for (int col = ColumnIndex + 1; col < ColumnIndex + ColumnSpan; col++)
            //    width -= grid.Columns[col].Width;
            int height = size.Height;
            for (int row = RowIndex + 1; row < RowIndex + RowSpan; row++)
                height -= grid.Rows[row].Height;
            return new Size(width, height * 2);
        }

        protected bool CellsRegionContainsSelectedCell(int columnIndex, int rowIndex, int columnSpan, int rowSpan)
        {
            if (DataGridView == null)
                return false;

            for (int col = columnIndex; col < columnIndex + columnSpan; col++)
                if (DataGridView.Columns[col].HeaderCell.Selected) return true;
            return false;
        }

        #endregion
    }
}
