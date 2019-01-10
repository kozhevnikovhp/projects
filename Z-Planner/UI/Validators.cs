using System;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI
{
    class TextBoxValidator
    {
        static private void ReportError(string message)
        {
            //SHOULD BE OUTPUTED TO STATUS BAR            
            //--ZPlannerManager.MessagePanel.AddMessage(message);
            MessageBox.Show(message, "Input Error", MessageBoxButtons.OK);
        }

        static public bool CheckRange(object obj, string name, double min, double max)
        {
            bool retval = true;

            TextBox tb = obj as TextBox;
            if (tb.Text == String.Empty) return retval;

            double val = 0;
            try
            {
                val = Convert.ToDouble(tb.Text);
                if (val < min || val > max)
                {
                    // TODO: improve number formatting to avoid smth like 1.9382498237434 when mils are being recalculated to millimeters
                    // TODO: or, create special validator for metric support
                    ReportError(String.Format("{0} value must be in the range [{1} : {2}] ", name, min, max));
                    retval = false;
                }
            }
            catch
            {
                ReportError(String.Format("Wrong {0} value.", name));
                retval = false;
            }

            if (!retval)
            {
                tb.Undo();
                tb.ClearUndo();
            }

            return retval;
        }

    }
}