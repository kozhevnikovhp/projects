using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class BulkEditDialog : Form
    {
        ZPlannerDMLPanel panel;
        string parameterID;
        List<ZMaterialParameter> materialParameters = new List<ZMaterialParameter>();
        string newValue;

        internal BulkEditDialog()
        {
            panel = ZPlannerManager.DMLPanel;
            panel.EndGridEdit();
            InitializeComponent();
            FillColumnsComboBox();
        }

        private void FillColumnsComboBox()
        {
            List<ZParameter> parameters = panel.GetColumnParametersToEdit();
            foreach (ZParameter parameter in parameters)
            {
                comboBoxSelectColumn.Items.Add(parameter);
            }
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            foreach (ZMaterialParameter materialParameter in materialParameters)
            {
                materialParameter.Value = newValue;
            }

            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void comboBoxSelectColumn_SelectedValueChanged(object sender, EventArgs e)
        {
            ZParameter parameter = comboBoxSelectColumn.SelectedItem as ZParameter;
            checkBoxChangeAll.Enabled = parameter != null && parameter.ID != string.Empty;
            textBoxReplaceWith.Enabled = parameter != null && parameter.ID != string.Empty;
            parameterID = (parameter != null) ? parameter.ID : string.Empty;
            checkBoxChangeAll_CheckedChanged(checkBoxChangeAll, e);
        }

        private void checkBoxChangeAll_CheckedChanged(object sender, EventArgs e)
        {
            textBoxEqualTo.Enabled = !checkBoxChangeAll.Checked;
            textBoxEqualTo_TextChanged(textBoxEqualTo, e);
        }

        private void textBoxEqualTo_TextChanged(object sender, EventArgs e)
        {
            if (checkBoxChangeAll.Checked)
            {
                materialParameters = panel.GetMaterialParametersToEdit(parameterID);
            }
            else
            {
                string sValue = textBoxEqualTo.Text;
                materialParameters = panel.GetMaterialParametersByValue(parameterID, sValue);
                if (materialParameters.Count == 0)
                {
                    ZParameter parameter = comboBoxSelectColumn.SelectedItem as ZParameter;
                    if (parameter != null)
                    {
                        double dValue;
                        if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                            sValue = dValue.ToString(ZPlannerManager.GetFormatByParameter(parameter), CultureInfo.InvariantCulture);
                    }

                    materialParameters = panel.GetMaterialParametersByValue(parameterID, sValue);
                }
            }

            labelAffectedValues.Text = materialParameters.Count.ToString();
        }

        private void textBoxReplaceWith_TextChanged(object sender, EventArgs e)
        {
            // TBD Add ReplaceWith Value Validation

            newValue = textBoxReplaceWith.Text;
        }

    }
}
