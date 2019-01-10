using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel.Design;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    class ZEntityCollectionEditor : CollectionEditor
    {
        public ZEntityCollectionEditor(Type type)
            : base(type)
        {
        }

        protected override bool CanSelectMultipleInstances()
        {
            return false;
        }

        /*protected override Type CreateCollectionItemType()
        {
            return this.CollectionType;
        }*/

        // Override this method in order to access the containing user controls from the default Collection Editor form or to add new ones...
        protected override CollectionForm CreateCollectionForm()
        {
            // Getting the default layout of the Collection Editor...
            CollectionForm collectionForm = base.CreateCollectionForm();
            System.Windows.Forms.Form frmCollectionEditorForm = collectionForm as System.Windows.Forms.Form;
            System.Windows.Forms.TableLayoutPanel tlpLayout = frmCollectionEditorForm.Controls[0] as System.Windows.Forms.TableLayoutPanel;

            if (tlpLayout != null)
            {
                // Get a reference to the inner PropertyGrid and hook an event handler to it.
                if (tlpLayout.Controls[5] is System.Windows.Forms.PropertyGrid)
                {
                    System.Windows.Forms.PropertyGrid propertyGrid = tlpLayout.Controls[5] as System.Windows.Forms.PropertyGrid;
                    tlpLayout.Controls.Remove(tlpLayout.Controls[5]);
                    tlpLayout.Controls.Remove(tlpLayout.Controls[2]);
                }
            }

            return collectionForm;
        }

        protected override object SetItems(object editValue, object[] value)
        {
            object ret_val = base.SetItems(editValue, value);

            return ret_val;
        }
    }
}
