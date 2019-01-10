using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Grid
{
    public partial class ZDataGridViewFilterForm : Form
    {
        private ZDataGridViewFilterMenuDataType filterType;
        private Control val1contol = null;
        private Control val2contol = null;

        public String FilterString
        {
            get
            {
                return filterString;
            }
        }

        public String ViewFilterString
        {
            get
            {
                return viewfilterString;
            }
        }

        private ZDataGridViewFilterForm()
        {
            InitializeComponent();
        }

        public ZDataGridViewFilterForm(ZDataGridViewFilterMenuDataType dataType)
            : this()
        {
            this.filterType = dataType;
            
            switch (this.filterType)
            {
                case ZDataGridViewFilterMenuDataType.DateTime:
                    this.val1contol = new DateTimePicker();
                    this.val2contol = new DateTimePicker();

                    System.Globalization.DateTimeFormatInfo dt = System.Threading.Thread.CurrentThread.CurrentCulture.DateTimeFormat;
                    (this.val1contol as DateTimePicker).CustomFormat = dt.ShortDatePattern + " " + dt.LongTimePattern;
                    (this.val2contol as DateTimePicker).CustomFormat = dt.ShortDatePattern + " " + dt.LongTimePattern;
                    (this.val1contol as DateTimePicker).Format = DateTimePickerFormat.Custom;
                    (this.val2contol as DateTimePicker).Format = DateTimePickerFormat.Custom;
                    
                    this.FilterTypeComboBox.Items.AddRange(new String[] {
                        "equals",
                        "does not equal",
                        "earlier than",
                        "later than",
                        "between"
                    });
                    break;

                case ZDataGridViewFilterMenuDataType.Int:
                case ZDataGridViewFilterMenuDataType.Float:
                    this.val1contol = new TextBox();
                    this.val2contol = new TextBox();
                    this.val1contol.TextChanged += eControlTextChanged;
                    this.val2contol.TextChanged += eControlTextChanged;
                    this.FilterTypeComboBox.Items.AddRange(new String[] {
                        "equals",
                        "does not equal",
                        "greater than",
                        "greater than or equal to",
                        "less than",
                        "less than or equal to",
                        "between"
                    });
                    this.val1contol.Tag = true;
                    this.val2contol.Tag = true;
                    this.okButton.Enabled = false;
                    break;

                default:
                    this.val1contol = new TextBox();
                    this.val2contol = new TextBox();
                    this.FilterTypeComboBox.Items.AddRange(new String[] {
                        "equals",
                        "does not equal",
                        "begins with",
                        "does not begin with",
                        "ends with",
                        "does not end with",
                        "contains",
                        "does not contain"
                    });
                    break;
            }
            this.FilterTypeComboBox.SelectedIndex = 0;

            this.val1contol.Name = "val1contol";
            this.val1contol.Location = new System.Drawing.Point(30, 66);
            this.val1contol.Size = new System.Drawing.Size(166, 20);
            this.val1contol.TabIndex = 4;
            this.val1contol.Visible = true;
            this.val1contol.KeyDown += eControlKeyDown;

            this.val2contol.Name = "val2contol";
            this.val2contol.Location = new System.Drawing.Point(30, 108);
            this.val2contol.Size = new System.Drawing.Size(166, 20);
            this.val2contol.TabIndex = 5;
            this.val2contol.Visible = false;
            this.val2contol.VisibleChanged += new EventHandler(val2contol_VisibleChanged);
            this.val2contol.KeyDown += eControlKeyDown;

            this.Controls.Add(this.val1contol);
            this.Controls.Add(this.val2contol);

            this.errorProvider.SetIconAlignment(this.val1contol, ErrorIconAlignment.MiddleRight);
            this.errorProvider.SetIconPadding(this.val1contol, -18);
            this.errorProvider.SetIconAlignment(this.val2contol, ErrorIconAlignment.MiddleRight);
            this.errorProvider.SetIconPadding(this.val2contol, -18);
            this.val1contol.Select();
        }

        private void val2contol_VisibleChanged(object sender, EventArgs e)
        {
            this.AndLabel.Visible = this.val2contol.Visible;
        }

        private void CancelButton_Click(object sender, EventArgs e)
        {
            this.viewfilterString = null;
            this.filterString = null;
            this.Close();
        }

        private void OKButton_Click(object sender, EventArgs e)
        {
            if (HasErrors())
            {
                this.okButton.Enabled = false;
                return;
            }

            String column = "[{0}] ";
            this.filterString = column;

            switch (this.filterType)
            {
                case ZDataGridViewFilterMenuDataType.DateTime:
                    DateTime dt = ((DateTimePicker)this.val1contol).Value;

                    if (this.FilterTypeComboBox.Text == "equals")
                    {
                        this.filterString += "= '" + dt.ToString("o") + "'";
                    }
                    else if (this.FilterTypeComboBox.Text == "earlier than")
                    {
                        this.filterString += "< '" + dt.ToString("o") + "'";
                    }
                    else if (this.FilterTypeComboBox.Text == "later than")
                    {
                        this.filterString += "> '" + dt.ToString("o") + "'";
                    }
                    else if (this.FilterTypeComboBox.Text == "between")
                    {
                        DateTime dt1 = ((DateTimePicker)this.val2contol).Value;
                        if (dt1 < dt)
                        {
                            DateTime d = dt;
                            dt = dt1;
                            dt1 = d;
                        }
                        this.filterString += ">= '" + dt.ToString("o") + "'";
                        this.filterString += " AND " + column + "<= '" + dt1.ToString("o") + "'";
                    }
                    else if (this.FilterTypeComboBox.Text == "does not equal")
                    {
                        this.filterString += "<> '" + dt.ToString("o") + "'";
                    }
                    break;

                case ZDataGridViewFilterMenuDataType.Int:
                case ZDataGridViewFilterMenuDataType.Float:

                    String num = this.val1contol.Text.Replace(",", ".");

                    if (this.FilterTypeComboBox.Text == "equals")
                        this.filterString += "= " + num;
                    else if (this.FilterTypeComboBox.Text == "between")
                    {
                        String num1 = this.val2contol.Text.Replace(",", ".");

                        double dnum1, dnum;
                        if (Double.TryParse(num, NumberStyles.Any, CultureInfo.InvariantCulture, out dnum) && Double.TryParse(num1, NumberStyles.Any, CultureInfo.InvariantCulture, out dnum1) && dnum > dnum1)
                        {
                            String nn = num;
                            num = num1;
                            num1 = nn;
                        }
                        this.filterString += ">= " + num + " AND " + column + "<= " + num1;
                    }
                    else if (this.FilterTypeComboBox.Text == "does not equal")
                        this.filterString += "<> " + num;
                    else if (this.FilterTypeComboBox.Text == "greater than")
                        this.filterString += "> " + num;
                    else if (this.FilterTypeComboBox.Text == "greater than or equal to")
                        this.filterString += ">= " + num;
                    else if (this.FilterTypeComboBox.Text == "less than")
                        this.filterString += "< " + num;
                    else if (this.FilterTypeComboBox.Text == "less than or equal to")
                        this.filterString += "<= " + num;
                    break;

                default:
                    String txt = this.val1contol.Text.Replace("'", "''").Replace("{", "{{").Replace("}", "}}");
                    if (this.FilterTypeComboBox.Text == "equals")
                        this.filterString += "LIKE '" + txt + "'";
                    else if (this.FilterTypeComboBox.Text == "does not equal")
                        this.filterString += "NOT LIKE '" + txt + "'";
                    else if (this.FilterTypeComboBox.Text == "begins with")
                        this.filterString += "LIKE '" + txt + "%'";
                    else if (this.FilterTypeComboBox.Text == "ends with")
                        this.filterString += "LIKE '%" + txt + "'";
                    else if (this.FilterTypeComboBox.Text == "does not begin with")
                        this.filterString += "NOT LIKE '" + txt + "%'";
                    else if (this.FilterTypeComboBox.Text == "does not end with")
                        this.filterString += "NOT LIKE '%" + txt + "'";
                    else if (this.FilterTypeComboBox.Text == "contains")
                        this.filterString += "LIKE '%" + txt + "%'";
                    else if (this.FilterTypeComboBox.Text == "does not contain")
                        this.filterString += "NOT LIKE '%" + txt + "%'";
                    break;
            }

            if (this.filterString != column)
            {
                this.viewfilterString = "Show rows where value" + " " + this.FilterTypeComboBox.Text + " \"" + this.val1contol.Text + "\"";
                if (this.val2contol.Visible)
                    this.viewfilterString += " " + this.AndLabel.Text + " \"" + this.val2contol.Text + "\"";
                this.DialogResult = System.Windows.Forms.DialogResult.OK;
            }
            else
            {
                this.filterString = null;
                this.viewfilterString = null;
                this.okButton.Enabled = false;
            }
        }

        private void FilterTypeComboBox_SelectedIndexChanged(object sender, EventArgs e)
        {
            this.val2contol.Visible = this.FilterTypeComboBox.Text == "between";
            if (String.IsNullOrEmpty(val1contol.Text) || !val2contol.Visible)
                this.val1contol.Select();
            else if (val2contol.Visible)
                this.val2contol.Select();
            this.okButton.Enabled = !HasErrors();
        }

        private void eControlTextChanged(object sender, EventArgs e)
        {
            Boolean hasErrors = false;
            switch (this.filterType)
            {
                case ZDataGridViewFilterMenuDataType.Int:
                    Int64 val;
                    hasErrors = !(Int64.TryParse((sender as TextBox).Text, out val));
                    break;

                case ZDataGridViewFilterMenuDataType.Float:
                    Double val1;
                    hasErrors = !(Double.TryParse((sender as TextBox).Text, out val1));
                    break;
            }

            (sender as Control).Tag = hasErrors || (sender as TextBox).Text.Length == 0;

            if (hasErrors && (sender as TextBox).Text.Length > 0)
                this.errorProvider.SetError((sender as Control), "Invalid value");
            else
                this.errorProvider.SetError((sender as Control), "");

            this.okButton.Enabled = !HasErrors();
        }

        private Boolean HasErrors()
        {
            return (this.val1contol.Visible && this.val1contol.Tag != null && ((Boolean)this.val1contol.Tag)) ||
                (this.val2contol.Visible && this.val2contol.Tag != null && ((Boolean)this.val2contol.Tag));
        }

        private void eControlKeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyData == Keys.Enter)
            {
                if (sender == this.val1contol)
                {
                    if (this.val2contol.Visible)
                        this.val2contol.Focus();
                    else
                        this.OKButton_Click(this.okButton, new EventArgs());
                }
                else
                {
                    this.OKButton_Click(this.okButton, new EventArgs());
                }

                e.SuppressKeyPress = false;
                e.Handled = true;
            }
        }
    }
}
