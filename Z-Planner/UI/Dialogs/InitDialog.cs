using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class InitDialog : Form
    {
        const int numberOfDispayingMessages = 3;
        public InitDialog(string product)
        {
            InitializeComponent();
            Text = product;
            labelProduct.Text = product;
        }

        public Label MessageBox { get { return labelMessages; } }

        public Label ProductBox { get { return labelProduct; } }

        public int NumberOfDispayingMessages { get { return numberOfDispayingMessages; } }
    }
}
