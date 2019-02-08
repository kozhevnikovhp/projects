using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewFilterSet : List<ZDataGridViewFilterRecord>
    {
        internal void Add(ZDataGridViewColumnHeaderCell cell)
        {
            if (cell != null && cell.OwningColumn != null)
            {
                this.RemoveAll(r => r.Name == cell.OwningColumn.Name);
                this.Add(new ZDataGridViewFilterRecord(cell.OwningColumn.Name, cell.FilterString, cell.ActiveFilterType));
            }
        }

        internal void Remove(string columnName)
        {
            this.RemoveAll(r => r.Name == columnName);
        }

        internal void Remove(ZDataGridViewColumnHeaderCell cell)
        {
            if (cell != null && cell.OwningColumn != null)
                this.Remove(cell.OwningColumn.Name);
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder("");

            foreach (ZDataGridViewFilterRecord r in this)
            {
                sb.AppendFormat("(" + r.FilterString + ") AND ", r.Name);
            }

            if (sb.Length > 4)
                sb.Length -= 4;

            return sb.ToString();
        }
    }

    public class ZDataGridViewSortSet : List<ZDataGridViewSortRecord>
    {
        internal void Add(ZDataGridViewColumnHeaderCell cell)
        {
            if (cell != null && cell.OwningColumn != null)
            {
                this.RemoveAll(r => r.Name == cell.OwningColumn.Name);
                this.Add(new ZDataGridViewSortRecord(cell.OwningColumn.Name, cell.SortString, cell.ActiveSortType));
            }
        }

        internal void Remove(string columnName)
        {
            this.RemoveAll(r => r.Name == columnName);
        }

        internal void Remove(ZDataGridViewColumnHeaderCell cell)
        {
            if (cell != null && cell.OwningColumn != null)
                this.Remove(cell.OwningColumn.Name);
        }

        public override string ToString()
        {
            StringBuilder sb = new StringBuilder("");

            foreach (ZDataGridViewSortRecord r in this)
            {
                sb.AppendFormat(r.SortString + ", ", r.Name);
            }

            if (sb.Length > 4)
                sb.Length -= 4;

            return sb.ToString();
        }
    }

    public struct ZDataGridViewFilterRecord
    {
        public string Name;
        public string FilterString;
        public ZDataGridViewFilterType FilterType;

        internal ZDataGridViewFilterRecord(string column, string filter, ZDataGridViewFilterType filterType)
        {
            Name = column;
            FilterString = filter;
            FilterType = filterType;
        }
    }

    public struct ZDataGridViewSortRecord
    {
        public string Name;
        public ZDataGridViewSortType SortType;
        public string SortString;

        internal ZDataGridViewSortRecord(string column, string sort, ZDataGridViewSortType sortType)
        {
            Name = column;
            SortString = sort;
            SortType = sortType;
        }
    }
}
