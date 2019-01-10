using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Translators;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class ExcelOptionsDlg : Form
    {

        public ExcelOptionsDlg()
        {
            InitializeComponent();
            Init();
        }

        public void Init()
        {
            ExportOptions options = ExportOptions.TheOptions;
            // text boxes
            tbProject.Text = options.ProjectName;
            tbRevision.Text = options.DesignRevision;
            tbFab.Text = options.Fabricator;
            tbFabContact.Text = options.FabContact;

            foreach (ExportFlags flag in options)
            {
                switch (flag)
                {
                    //layer data
                    case ExportFlags.LayerNumber: cbLayerNumber.Checked = true; break;
                    case ExportFlags.ViaSpans: cbViaSpans.Checked = true; break;
                    case ExportFlags.LayerType: cbLayerType.Checked = true; break;
                    case ExportFlags.LayerName: cbLayerName.Checked = true; break;

                    //dielectric material data
                    case ExportFlags.Material: cbMaterial.Checked = true; break;
                    case ExportFlags.Construction: cbConstruction.Checked = true; break;
                    case ExportFlags.Resin: cbResin.Checked = true; break;
                    case ExportFlags.Thickness: cbThickness.Checked = true; break;
                    case ExportFlags.Dk: cbDk.Checked = true; break;
                    case ExportFlags.Df: cbDf.Checked = true; break;
                    case ExportFlags.Frequency: cbFrequency.Checked = true; break;

                    // copper, impedance, notes
                    case ExportFlags.SingleEnded: cbSE.Checked = true; break;
                    case ExportFlags.Differential: cbDiff.Checked = true; break;
                    case ExportFlags.CopperWeight: cbCopperWeight.Checked = true; break;
                    case ExportFlags.Copper: cbCopper.Checked = true; break;
                    case ExportFlags.RzTop: cbRz.Checked = true; break;
                    case ExportFlags.TraceWidth: cbTraceWidth.Checked = true; break;
                    case ExportFlags.Impedance: cbImpedance.Checked = true; break;
                    case ExportFlags.DiffImpedance: cbDiffImpedance.Checked = true; break;
                    case ExportFlags.RefLayer: cbRefLayer.Checked = true; break;
                    case ExportFlags.Notes: cbNotes.Checked = true; break;
                    case ExportFlags.Loss: cbLoss.Checked = true; break;
                }
            }
        }

        public void SaveState()
        {
            ExportOptions options = ExportOptions.TheOptions;
            // text boxes
            options.ProjectName = tbProject.Text;
            options.DesignRevision = tbRevision.Text;
            options.Fabricator = tbFab.Text;
            options.FabContact = tbFabContact.Text;

            // check boxes
            options.Clear();
            //layer data
            if (cbLayerNumber.Checked) options.Add(ExportFlags.LayerNumber);
            if (cbViaSpans.Checked) options.Add(ExportFlags.ViaSpans);
            if (cbLayerType.Checked) options.Add(ExportFlags.LayerType);
            if (cbLayerName.Checked) options.Add(ExportFlags.LayerName);

            //dielectric material data
            if (cbMaterial.Checked) options.Add(ExportFlags.Material);
            if (cbConstruction.Checked) options.Add(ExportFlags.Construction);
            if (cbResin.Checked) options.Add(ExportFlags.Resin);
            if (cbThickness.Checked) options.Add(ExportFlags.Thickness);
            if (cbDk.Checked) options.Add(ExportFlags.Dk);
            if (cbDf.Checked) options.Add(ExportFlags.Df);
            if (cbFrequency.Checked) options.Add(ExportFlags.Frequency);

            //copper, impedance, notes
            if (cbSE.Checked) options.Add(ExportFlags.SingleEnded);
            if (cbDiff.Checked) options.Add(ExportFlags.Differential);
            if (cbCopperWeight.Checked) options.Add(ExportFlags.CopperWeight);
            if (cbCopper.Checked) options.Add(ExportFlags.Copper);
            if (cbRz.Checked)
            {
                options.Add(ExportFlags.RzTop);
                options.Add(ExportFlags.RzBot);
            }
            if (cbTraceWidth.Checked) options.Add(ExportFlags.TraceWidth);
            if (cbImpedance.Checked) options.Add(ExportFlags.Impedance);
            if (cbLoss.Checked) options.Add(ExportFlags.Loss);
            if (cbDiffImpedance.Checked) options.Add(ExportFlags.DiffImpedance);
            if (cbRefLayer.Checked) options.Add(ExportFlags.RefLayer);
            if (cbNotes.Checked) options.Add(ExportFlags.Notes);

            ExportOptions.TheOptions = options;
        }

        private void btnExport_Click(object sender, EventArgs e)
        {
            SaveState();
            this.DialogResult = DialogResult.OK;
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            this.DialogResult = DialogResult.Cancel;
            Close();
        }

        private void cbThickness_CheckedChanged(object sender, EventArgs e)
        {
            cbMetalThickness.Checked = cbThickness.Checked;
        }

        private void cbMetalThickness_CheckedChanged(object sender, EventArgs e)
        {
            cbThickness.Checked = cbMetalThickness.Checked;
        }

        private void ExcelOptionsDlg_Load(object sender, EventArgs e)
        {

        }

    }
}
