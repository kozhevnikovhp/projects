using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.DRC;
using ZZero.ZPlanner.Settings;

namespace ZZero.ZPlanner.UI
{
    public partial class DRCPanel : DockContent
    {
        public DRCPanel()
        {
            InitializeComponent();
            VisibleChanged += panel_VisibleChanges;
        }

        void panel_VisibleChanges(object sender, EventArgs e)
        {
            if (this.Visible)
            {
                UpdateReport();
            }
        }

        public void UpdateReport()
        {
            Checker check = new Checker();
            check.CheckActive();
            if (IsHidden)
            {
                foreach (Problem problem in check.report)
                {
                    if (Options.TheOptions.displayDRCErrors && problem.severity == Problem.Severity.Error ||
                        Options.TheOptions.displayDRCWarnings && problem.severity == Problem.Severity.Warning)
                    {
                        ZPlannerManager.ShowDRCPanel();
                        ZPlannerManager.MainMenu.SelectView();
                        break;
                    }
                }
            }

            UpdateReportGrid(check.report);
            //--tbReport.Text = check.TextReport();            
        }

        private void button1_Click(object sender, EventArgs e)
        {
            UpdateReport();
        }

        private void UpdateReportGrid(List<Problem> problems)
        {
            while (gridReport.Rows.Count != 0)
            {
                gridReport.Rows.RemoveAt(0);
            }
        
            int i = 0;
            foreach (Problem p in problems){
                i++;
                gridReport.Rows.Add(i.ToString(), p.GetSeverity(), p.GetScope(), p.GetDescription(), p.layer != null ? p.layer.ID : "");
            }
        }

        private void gridReport_SelectionChanged(object sender, EventArgs e)
        {
            if (gridReport.SelectedRows.Count > 0)
            {
                string id = gridReport.SelectedRows[0].Cells[4].Value.ToString();
                //MessageBox.Show(s);
                ZPlannerManager.StackupPanel.SelectRow(id);
            }
        }
    }
}
