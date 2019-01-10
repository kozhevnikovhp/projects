using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms.Design;
using ZZero.ZPlanner.UI.Grid;

namespace ZZero.ZPlanner.Data.Properties
{
    class TableTypeEditor : UITypeEditor
    {
        public override UITypeEditorEditStyle GetEditStyle(ITypeDescriptorContext context)
        {
            return UITypeEditorEditStyle.DropDown;
        }

        public override object EditValue(ITypeDescriptorContext context, IServiceProvider provider, object value)
        {
            IWindowsFormsEditorService editorService;
            if (provider != null)
            {
                editorService =
                    provider.GetService(
                    typeof(IWindowsFormsEditorService))
                    as IWindowsFormsEditorService;


                if (editorService != null)
                {
                    ZDataGridViewFrequencyDependedEditingControl editingControl =
                        new ZDataGridViewFrequencyDependedEditingControl();
                    editingControl.Init(context.PropertyDescriptor.DisplayName, (value != null) ? value.ToString() : string.Empty, (context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyFormat);
                    editingControl.Tag = editorService;
                    editorService.DropDownControl(editingControl);

                    value = editingControl.Value;
                }
            }

            return value;
        }
    }
}
