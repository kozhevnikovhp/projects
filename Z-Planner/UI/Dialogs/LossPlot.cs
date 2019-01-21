using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Printing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using ZZero.ZPlanner.FXInterface;
using ZZero.ZPlanner.Settings;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class LossPlot : Form
    {
        private bool bFreeze;
        private FX_Output fxOut;
        private PrintDocument printDocument1 = new PrintDocument();
        bool bRoughnessEnabled;
        public LossPlot(FX_Output fx, bool bRoughness = false, string comment = "")
        {
            InitializeComponent();
            lFieldSolver.Text = AboutData.TheAbout.sFSVersion;
            lSandbox.Text = "Z-zero  " + AboutData.TheAbout.sVersion;
            Load += Chart_Load;
            fxOut = fx;
            bFreeze = false;
            printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);

            //cbRoughness.Enabled = bRoughness;//ZPlannerManager.StackupPanel != null ? ZPlannerManager.StackupPanel.IsRoughness : false;
            bRoughnessEnabled = bRoughness;
            tbComment.Text = comment;
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            CaptureScreen();

            /*PrintPreviewDialog*/PrintDialog dlg = new PrintDialog();//PrintPreviewDialog();
            dlg.Document = printDocument1;
            DialogResult result = dlg.ShowDialog();

            if (result == DialogResult.OK) { 
                printDocument1.Print();
            }        
        }

        Bitmap memoryImage;

        private void CaptureScreen()
        {
            memoryImage = new Bitmap(container.Width, container.Height);
            container.DrawToBitmap(memoryImage, new Rectangle(0, 0, memoryImage.Width, memoryImage.Height));
        }

        private void printDocument1_PrintPage(System.Object sender,  
           System.Drawing.Printing.PrintPageEventArgs e)
        {
            e.Graphics.DrawImage(memoryImage, 0, 0);
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            CaptureScreen();
            Clipboard.SetImage(memoryImage);
        }


        private void Chart_Load(object sender, EventArgs e)
        {
            RecalcChart();
            cbRoughness.Checked = bRoughnessEnabled;
        }

        private void RecalcChart()
        {
            tbDominates.Text = "";

            double coef = 1;
            if (rbCm.Checked)
            {
                coef = 0.01;
            }
            else
            {
                coef = 0.0254;
            }


            chartLoss.ChartAreas["chartAreaLoss"].AxisX.Title = "Frequency [GHz]";
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.TitleAlignment = StringAlignment.Far;

            chartLoss.ChartAreas["chartAreaLoss"].AxisY.Title = rbCm.Checked ? "Attenuation [db/cm]" : "Attenuation [db/in]";
            chartLoss.ChartAreas["chartAreaLoss"].AxisY.TextOrientation = TextOrientation.Rotated270;
            chartLoss.ChartAreas["chartAreaLoss"].AxisY.TitleAlignment = StringAlignment.Far;
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.LabelStyle.Format = "N1";

            chartLoss.Series.Clear();

            chartLoss.Series.Add("Roughness");
            chartLoss.Series["Roughness"].Enabled = true;
            chartLoss.Series["Roughness"].ChartType = SeriesChartType.Spline;
            chartLoss.Series["Roughness"].BorderWidth = 2;
            chartLoss.Series["Roughness"].Color = pictColorCuRoughness.BackColor;

            chartLoss.Series.Add("Resistive");
            chartLoss.Series["Resistive"].Enabled = true;
            chartLoss.Series["Resistive"].ChartType = SeriesChartType.Spline;
            chartLoss.Series["Resistive"].BorderWidth = 2;
            chartLoss.Series["Resistive"].Color = pictColorResistive.BackColor;

            chartLoss.Series.Add("Dielectric");
            chartLoss.Series["Dielectric"].Enabled = true;
            chartLoss.Series["Dielectric"].ChartType = SeriesChartType.Spline;
            chartLoss.Series["Dielectric"].BorderWidth = 2;
            chartLoss.Series["Dielectric"].Color = pictColorDielectric.BackColor;

            chartLoss.Series.Add("Total");
            chartLoss.Series["Total"].Enabled = true;
            chartLoss.Series["Total"].ChartType = SeriesChartType.Spline;
            chartLoss.Series["Total"].BorderWidth = 2;
            chartLoss.Series["Total"].Color = pictColorTotal.BackColor;

            //data
            FXLosses loss = new FXLosses(fxOut);

            int NumPts = 500;
            double Fmin = Convert.ToDouble(tbFmin.Text);
            double Fmax = Convert.ToDouble(tbFmax.Text);
            double df = (Fmax - Fmin) / NumPts;
            double f = 0;// Fmin;
            double xPoint = -1; //point where dielectric loss becomes dominant
            double aR_prev = -1, aD_prev = -1;
            bool bxPoint = false;

            bool bTotal = cbRoughness.Checked || cbResistive.Checked || cbDielectric.Checked;
            bool bRD = (cbRoughness.Checked || cbResistive.Checked) && cbDielectric.Checked;

            //account for Fcrossover > Fmax
            int NumPtsInf = NumPts;
            const double Finf = 200; //GHz
            if (bRD && Fmax < Finf)
            {
                NumPtsInf = Convert.ToInt32((Finf - Fmin) / df);
            }

            for (int i = 0; i < NumPtsInf - 1; i++)
            {
                double aTotal = 0;
                double aD = 0, aR = 0;

                if (cbRoughness.Checked)
                {
                    double aRo = loss.Attenuation_Ro(f) * coef;
                    if (f >= Fmin && f <= Fmax)
                    {
                        chartLoss.Series["Roughness"].Points.AddXY(f, aRo);
                    }
                    //--aR += aRo;
                    aTotal += aRo;
                }

                if (cbResistive.Checked)
                {
                    double aRe = loss.Attenuation_R(f) * coef;
                    if (f >= Fmin && f <= Fmax)
                    {
                        chartLoss.Series["Resistive"].Points.AddXY(f, aRe);
                    }
                    aR += aRe;
                    aTotal += aRe;
                }

                if (cbDielectric.Checked)
                {
                    aD = loss.Attenuation_D(f) * coef;
                    if (f >= Fmin && f <= Fmax)
                    {
                        chartLoss.Series["Dielectric"].Points.AddXY(f, aD);
                    }
                    aTotal += aD;
                }

                if (bTotal && cbTotal.Checked)
                {
                    if (f >= Fmin && f <= Fmax)
                    {
                        chartLoss.Series["Total"].Points.AddXY(f, aTotal);
                    }
                }

                //check xPoint
                if (bRD && i > 0 && !bxPoint)
                {//not the first point and not already found
                    if ((aD_prev <= aR_prev) && (aD > aR))
                    {
                        // aD_prev + ((aD-aD_prev) / df) * x = aR_prev + ((aR-aR_prev) / df) * x
                        double kR = (aR - aR_prev) / df;
                        double kD = (aD - aD_prev) / df;

                        xPoint = (f - df) + (aD_prev - aR_prev) / (kR - kD);
                        bxPoint = true;

                        if (f >= Fmax) break;
                    }
                }

                aR_prev = aR;
                aD_prev = aD;

                f += df;
            }
            if (bRD && bxPoint)
            {
                tbDominates.Text = xPoint.ToString("N2");
            }
            
            chartLoss.Series["Total"].Enabled = cbTotal.Checked;
            chartLoss.Series["Roughness"].Enabled = cbRoughness.Checked;
            chartLoss.Series["Resistive"].Enabled = cbResistive.Checked;
            chartLoss.Series["Dielectric"].Enabled = cbDielectric.Checked;

            chartLoss.ChartAreas["chartAreaLoss"].AxisX.IsMarginVisible = false;
            chartLoss.ChartAreas["chartAreaLoss"].RecalculateAxesScale();
            //chartLoss.ChartAreas["chartAreaLoss"].AxisX.IntervalOffset = -(chartLoss.ChartAreas["chartAreaLoss"].AxisX.Minimum - Math.Truncate(chartLoss.ChartAreas["chartAreaLoss"].AxisX.Minimum));
            //chartLoss.ChartAreas["chartAreaLoss"].AxisX.MajorGrid.Interval = (int)((Fmax - Fmin) / 10 + 0.5); //closest whole number - ???????
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.Enabled = true;
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.Interval = chartLoss.ChartAreas["chartAreaLoss"].AxisX.MajorGrid.Interval / 2;
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.LineDashStyle = ChartDashStyle.Dot;

            //chartLoss.ChartAreas["chartAreaLoss"].AxisY.MajorGrid.Interval = (chartLoss.ChartAreas["chartAreaLoss"].AxisY.Maximum - chartLoss.ChartAreas["chartAreaLoss"].AxisY.Minimum) / 10;
            chartLoss.ChartAreas["chartAreaLoss"].AxisY.MinorGrid.Enabled = true;
            chartLoss.ChartAreas["chartAreaLoss"].AxisY.MinorGrid.Interval = chartLoss.ChartAreas["chartAreaLoss"].AxisY.MajorGrid.Interval / 2;
            chartLoss.ChartAreas["chartAreaLoss"].AxisY.MinorGrid.LineDashStyle = ChartDashStyle.Dot;
            //chartLoss.Visible = true;

            //snap cursor
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.IsUserEnabled = true;
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.Interval = 0;
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.LineColor = Color.DarkGray;
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.LineWidth = 1;
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.LineDashStyle = ChartDashStyle.Solid;
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.Interval = 0;

            chartLoss.ChartAreas["chartAreaLoss"].CursorY.LineColor = Color.Black;
            chartLoss.ChartAreas["chartAreaLoss"].CursorY.LineWidth = 0;
            chartLoss.ChartAreas["chartAreaLoss"].CursorY.IsUserEnabled = false;    
        }

        void chart1_MouseMove(object sender, MouseEventArgs e)
        {
            if (bFreeze) return;

            var pos = e.Location;

            Point mousePoint = new Point(e.X, e.Y);
            chartLoss.ChartAreas["chartAreaLoss"].CursorX.SetCursorPixelPosition(mousePoint, true);
            chartLoss.ChartAreas["chartAreaLoss"].CursorY.SetCursorPixelPosition(mousePoint, true);

            int x = e.X;
            if (x < 0) x = 0;
            if (x > chartLoss.ClientSize.Width - 2) x = chartLoss.ClientSize.Width - 2;
            double f = chartLoss.ChartAreas["chartAreaLoss"].AxisX.PixelPositionToValue(x);
            bool isValid = (f >= chartLoss.ChartAreas["chartAreaLoss"].AxisX.Minimum) && (f <= chartLoss.ChartAreas["chartAreaLoss"].AxisX.Maximum);

            if (isValid)
            {
                tbCursorX.Text = f.ToString("N2");

                FXLosses loss = new FXLosses(fxOut);
                double coef = 1;
                if (rbCm.Checked)
                {
                    coef = 0.01;
                }
                else
                {
                    coef = 0.0254;
                }

                double aT = 0, aD = 0, aRe = 0, aRo = 0;
                
                aRe = loss.Attenuation_R(f) * coef;
                aRo = loss.Attenuation_Ro(f) * coef;
                aD = loss.Attenuation_D(f) * coef;

                tbCursorCuRoughness.Text = cbRoughness.Checked ? aRo.ToString("N2") : "";
                tbCursorResistive.Text = cbResistive.Checked ? aRe.ToString("N2") : ""; 
                tbCursorDielectric.Text = cbDielectric.Checked ? aD.ToString("N2"): "";

                if (cbRoughness.Checked) aT += aRo;
                if (cbResistive.Checked) aT += aRe;
                if (cbDielectric.Checked) aT += aD;

                tbCursorTotal.Text = cbTotal.Checked ? aT.ToString("N2"): ""; 
            }
            else
            {
                tbCursorX.Text = "";
                tbCursorResistive.Text = "";
                tbCursorDielectric.Text = "";
                tbCursorTotal.Text = "";
                tbCursorCuRoughness.Text = "";
            }
        }

        private void pictColorResistive_Click(object sender, EventArgs e)
        {
            if (!cbResistive.Checked) return;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = pictColorResistive.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                pictColorResistive.BackColor = MyDialog.Color;
                this.chartLoss.Series["Resistive"].Color = MyDialog.Color;
            }
        }

        private void pictColorTotal_Click(object sender, EventArgs e)
        {
            if (!cbTotal.Checked) return;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = pictColorTotal.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                pictColorTotal.BackColor = MyDialog.Color;
                this.chartLoss.Series["Total"].Color = MyDialog.Color;
            }
        }

        private void pictColorDielectric_Click(object sender, EventArgs e)
        {
            if (!cbDielectric.Checked) return;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = pictColorDielectric.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                pictColorDielectric.BackColor = MyDialog.Color;
                this.chartLoss.Series["Dielectric"].Color = MyDialog.Color;
            }
        }

        private void pictColorCuRoughness_Click(object sender, EventArgs e)
        {
            if (!cbRoughness.Checked) return;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = pictColorCuRoughness.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                pictColorCuRoughness.BackColor = MyDialog.Color;
                this.chartLoss.Series["Roughness"].Color = MyDialog.Color;
            }
        }


        private void cbTotal_CheckedChanged(object sender, EventArgs e)
        {
            chartLoss.Series["Total"].Enabled = cbTotal.Checked;
            RecalcChart();
        }

        private void cbResistive_CheckedChanged(object sender, EventArgs e)
        {
            chartLoss.Series["Resistive"].Enabled = cbResistive.Checked;
            RecalcChart();
        }

        private void cbDielectric_CheckedChanged(object sender, EventArgs e)
        {
            chartLoss.Series["Dielectric"].Enabled = cbDielectric.Checked;
            RecalcChart();
        }

        private void rbInch_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChart();
            cbTotal.Text = rbInch.Checked ? "Total loss [dB/in]" : "Total loss [dB/cm]";
        }

        private void rbCm_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChart();
            cbTotal.Text = rbInch.Checked ? "Total loss [dB/in]" : "Total loss [dB/cm]";
        }

        private void cbRoughness_CheckedChanged(object sender, EventArgs e)
        {
            //--ZPlannerManager.IsRoughness = cbRoughness.Checked;
            chartLoss.Series["Roughness"].Enabled = cbRoughness.Checked;//--cbRoughness.Enabled ? cbRoughness.Checked : false;
            RecalcChart();
        }

        private void tbFmin_Leave(object sender, EventArgs e)
        {
            RecalcChart();
        }

        private void tbFmax_Leave(object sender, EventArgs e)
        {
            RecalcChart();
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void chartLoss_Click(object sender, EventArgs e)
        {
            bFreeze = !bFreeze;
        }

    }
}
