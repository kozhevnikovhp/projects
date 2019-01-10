using System;
using System.Collections.Generic;
using System.Globalization;
using System.Windows.Forms;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class ChangeColumnValueDialog : Form
    {
        ZPlannerStackupPanel panel;
        string parameterID;
        List<ZLayerParameter> layerParameters = new List<ZLayerParameter>();
        string newValue;

        internal ChangeColumnValueDialog()
        {
            panel = ZPlannerManager.StackupPanel;
            panel.EndGridEdit();
            InitializeComponent();
            FillColumnsComboBox();
        }

        private void FillColumnsComboBox()
        {
            List<ZParameter> parameters = panel.GetColumnParametersToEdit();
            foreach(ZParameter parameter in parameters)
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
            if (layerParameters.Count > 0)
            {
                if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeColumnValuesCommand(layerParameters, layerParameters[0].Value, newValue);
                bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();

                foreach (ZLayerParameter layerParameter in layerParameters)
                {
                    layerParameter.Value = newValue;
                }

                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }

            DialogResult = System.Windows.Forms.DialogResult.OK;
        }

        private void comboBoxSelectColumn_SelectedValueChanged(object sender, EventArgs e)
        {
            ZParameter parameter = comboBoxSelectColumn.SelectedItem as ZParameter;
            checkBoxChangeAll.Enabled = parameter != null && parameter.ID != string.Empty;
            textBoxReplaceWith.Enabled = parameter != null && parameter.ID != string.Empty;
            comboBoxReplaceWith.Enabled = parameter != null && parameter.ID != string.Empty;
            parameterID = (parameter != null) ? parameter.ID : string.Empty;

            if (parameter != null && parameter.List != null && parameter.List.GetValues().Count > 0)
            {
                List<string> values = parameter.List.GetValues();
                comboBoxReplaceWith.Location = textBoxReplaceWith.Location;
                comboBoxReplaceWith.Items.Clear();
                comboBoxReplaceWith.Items.Add("");
                comboBoxReplaceWith.Items.AddRange(values.ToArray());
                textBoxReplaceWith.Visible = false;
                comboBoxReplaceWith.Visible = true;
            }
            else
            {
                comboBoxReplaceWith.Visible = false;
                textBoxReplaceWith.Visible = true;
            }

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
                layerParameters = panel.GetLayerParametersToEdit(parameterID);
            }
            else
            {
                string sValue = textBoxEqualTo.Text;
                layerParameters = panel.GetLayerParametersByValue(parameterID, sValue);
                if (layerParameters.Count == 0)
                {
                    ZParameter parameter = comboBoxSelectColumn.SelectedItem as ZParameter;
                    if (parameter != null)
                    {
                        double dValue;
                        if (double.TryParse(sValue, NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                            sValue = dValue.ToString(ZPlannerManager.GetFormatByParameter(parameter), CultureInfo.InvariantCulture);
                    }

                    layerParameters = panel.GetLayerParametersByValue(parameterID, sValue);
                }
            }

            labelAffectedValues.Text = layerParameters.Count.ToString();
        }

        private void textBoxReplaceWith_TextChanged(object sender, EventArgs e)
        {
            // TBD Add ReplaceWith Value Validation

            newValue = textBoxReplaceWith.Text;
        }

        private void comboBoxReplaceWith_TextChanged(object sender, EventArgs e)
        {
            // TBD Add ReplaceWith Value Validation

            newValue = comboBoxReplaceWith.Text;
        }

    }
}
