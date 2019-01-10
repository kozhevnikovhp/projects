using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
namespace ZZero.ZPlanner.Commands
{
    internal struct ColumnsDisplayingStructure
    {
        internal ZParameter parameter;
        internal DataGridViewColumn column;
        internal int oldOrder;
        internal int newOrder;
        internal bool oldHidden;
        internal bool newHidden;

        internal ColumnsDisplayingStructure(DataGridViewColumn column, ZParameter parameter, int oldOrder, int newOrder, bool oldHidden, bool newHidden)
        {
            this.parameter = parameter;
            this.column = column;
            this.oldOrder = oldOrder;
            this.newOrder = newOrder;
            this.oldHidden = oldHidden;
            this.newHidden = newHidden;
        }

        internal ColumnsDisplayingStructure(DataGridViewColumn column, int oldOrder, int newOrder)
        {
            this.parameter = null;
            this.column = column;
            this.oldOrder = oldOrder;
            this.newOrder = newOrder;
            this.oldHidden = false;
            this.newHidden = false;
        }
    }
}
