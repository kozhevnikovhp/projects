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
    public partial class AutoMirrorDialog : Form
    {
        public AutoMirrorDialog()
        {
            InitializeComponent();

            //
            if (ZPlannerManager.StackupPanel == null) return;

            cbAutoMirror.Checked = ZPlannerManager.IsAutoMirror;
            cbCenterLine.Checked = ZPlannerManager.IsCenterLineVisible;
        }

        public bool isAutoMirror
        {
            get
            {
                return cbAutoMirror.Checked;
            }
        }

        public bool isCenterLine
        {
            get
            {
                return cbCenterLine.Checked;
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            bool wasMirrored = cbAutoMirror.Checked;
            if (cbAutoMirror.Checked && !ZPlannerManager.IsAutoMirror && !ZPlannerManager.Project.StackupIsEmpty && !ZPlannerManager.Project.Stackup.IsMirrored())
            {
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.EndGridEdit();
                wasMirrored = ZPlannerManager.Project.Stackup.MirrorStackup();
            }

            cbAutoMirror.Checked = wasMirrored;
            if ((cbAutoMirror.Checked) != ZPlannerManager.IsAutoMirror) ZPlannerManager.IsAutoMirror = cbAutoMirror.Checked;
            if (cbCenterLine.Checked != ZPlannerManager.IsCenterLineVisible) ZPlannerManager.ShowCenterLine(cbCenterLine.Checked);

            //
            DialogResult = DialogResult.OK;
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            DialogResult = DialogResult.Cancel;
            Close();
        }
    }
}