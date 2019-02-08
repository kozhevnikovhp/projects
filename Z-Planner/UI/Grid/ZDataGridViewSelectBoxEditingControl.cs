using PresentationControls;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Grid
{
    class ZDataGridViewSelectBoxEditingControl : CheckBoxComboBox, IDataGridViewEditingControl
    {
        DataGridView dataGridView;
        private bool valueChanged = false;
        int rowIndex;

        public ZDataGridViewSelectBoxEditingControl()
        {
            
        }

        // Implements the IDataGridViewEditingControl.EditingControlFormattedValue 
        // property.
        public object EditingControlFormattedValue
        {
            get
            {
                return this.Text;
            }
            set
            {
                this.Text = value.ToString();
            }
        }

        // Implements the 
        // IDataGridViewEditingControl.GetEditingControlFormattedValue method.
        public object GetEditingControlFormattedValue(
            DataGridViewDataErrorContexts context)
        {
            return EditingControlFormattedValue;
        }

        // Implements the 
        // IDataGridViewEditingControl.ApplyCellStyleToEditingControl method.
        public void ApplyCellStyleToEditingControl(
            DataGridViewCellStyle dataGridViewCellStyle)
        {
            this.Font = dataGridViewCellStyle.Font;
            this.BackColor = dataGridViewCellStyle.BackColor;
            this.ForeColor = dataGridViewCellStyle.ForeColor;
        }

        // Implements the IDataGridViewEditingControl.EditingControlRowIndex 
        // property.
        public int EditingControlRowIndex
        {
            get
            {
                return rowIndex;
            }
            set
            {
                rowIndex = value;
            }
        }

        // Implements the IDataGridViewEditingControl.EditingControlWantsInputKey 
        // method.
        public bool EditingControlWantsInputKey(
            Keys key, bool dataGridViewWantsInputKey)
        {
            //return !dataGridViewWantsInputKey;
            return true;
        }

        // Implements the IDataGridViewEditingControl.PrepareEditingControlForEdit 
        // method.
        public void PrepareEditingControlForEdit(bool selectAll)
        {
            this.ClearSelection();
            Text = string.Empty;
            if (CheckBoxItems.Count > 0)
            {
                CheckBoxItems[0] = new CheckBoxComboBoxItem(this, "");
                CheckBoxItems[0].Visible = false;
            }
            if (Items.Count > 0) Items[0] = ""; 
            ZParameter parameter = dataGridView.CurrentCell.OwningColumn.Tag as ZParameter;
            if (parameter == null || parameter.List == null) return;

            Dictionary<string, string> dict = parameter.List.GetDictionary();
            string textValue = dataGridView.CurrentCell.FormattedValue.ToString();
            if (textValue.Contains('/'))
            {
                string[] splitValues = textValue.Split(new string[]{","," "}, StringSplitOptions.RemoveEmptyEntries);
                foreach (string value in splitValues)
                {
                    if (dict.ContainsKey(value)) 
                    {
                        //Text += (Text == string.Empty) ? dict[value] : ", " + dict[value];                    
                        for(int i = 1; i < Items.Count; ++i)
                        {
                            if (CheckBoxItems[i].Text == dict[value]) CheckBoxItems[i].Checked = true;
                        }
                    }
                }
            }
        }

        // Implements the IDataGridViewEditingControl
        // .RepositionEditingControlOnValueChange property.
        public bool RepositionEditingControlOnValueChange
        {
            get
            {
                return false;
            }
        }

        // Implements the IDataGridViewEditingControl
        // .EditingControlDataGridView property.
        public DataGridView EditingControlDataGridView
        {
            get
            {
                return dataGridView;
            }
            set
            {
                dataGridView = value;
            }
        }

        // Implements the IDataGridViewEditingControl
        // .EditingControlValueChanged property.
        public bool EditingControlValueChanged
        {
            get
            {
                return valueChanged;
            }
            set
            {
                valueChanged = value;
            }
        }

        // Implements the IDataGridViewEditingControl
        // .EditingPanelCursor property.
        public Cursor EditingPanelCursor
        {
            get
            {
                return base.Cursor;
            }
        }

        protected override void OnCheckBoxCheckedChanged(object sender, EventArgs e)
        {
            // Notify the DataGridView that the contents of the cell
            // have changed.
            valueChanged = true;
            this.EditingControlDataGridView.NotifyCurrentCellDirty(true);
            base.OnCheckBoxCheckedChanged(sender, e);
        }
    }
}
