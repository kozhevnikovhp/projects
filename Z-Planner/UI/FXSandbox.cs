using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Printing;
using System.IO;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.FXInterface;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.UI
{
    public partial class FXSandboxPanel : DockContent
    {
        private bool bFreeze;
        private FX_Output fxOut;
        private PrintDocument printDocument1 = new PrintDocument();
        private bool isCalculationCanceled = false;
        private bool isIgnoreSolve = false;

        public FXSandboxPanel()
        {
            InitializeComponent();

#if ZSANDBOX
            btnClose.Enabled = false;
            btnClose.Visible = false;
            btnCopy.Location = btnPrint.Location;
            btnPrint.Location = btnClose.Location;
#endif

            lFieldSolver.Text = AboutData.TheAbout.sFSVersion;
            lSandbox.Text = "Z-zero  " + AboutData.TheAbout.sVersion;
            lFieldSolver2.Text = AboutData.TheAbout.sFSVersion;
            lSandbox2.Text = "Z-zero  " + AboutData.TheAbout.sVersion;
            printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);
            //--tabCSType.Dock = DockStyle.Fill;

            LoadData();
            SolveAll();
        }

        private void cbDiffSurface_CheckedChanged(object sender, EventArgs e)
        {
            //gbDifferential.Enabled = cbDiffSurface.Checked;
            panelDifferential.Enabled = cbDiffSurface.Checked;
            if (isCalculationCanceled) return;

            DefinePlotSource();
            RecalcChart();
        }

        private void cbDiffBurried_CheckedChanged(object sender, EventArgs e)
        {
            panelBurriedDifferential.Enabled = cbDiffBuried.Checked;
            if (isCalculationCanceled) return;

            DefinePlotSource();
            RecalcChart();
        }

        private void cbDiffStripline_CheckedChanged(object sender, EventArgs e)
        {
            panelStriplineDifferential.Enabled = cbDiffStripline.Checked;
            if (isCalculationCanceled) return;

            DefinePlotSource();
            RecalcChart();
        }


//====================== data exchange 
        internal List<sbData> GetFSSData()
        {
            SaveData();

            List<sbData> dataList = new List<sbData>();
            dataList.Add(sbData.SurfaceMicrostrip);
            dataList.Add(sbData.BuriedMicrostrip);
            dataList.Add(sbData.Stripline);
            return dataList;
        }

        internal void SetFSSData(List<sbData> dataList)
        {
            foreach (sbData data in dataList)
            {
                switch(data.type)
                {
                    case csType.SurfaceMicrostrip:
                        sbData.SurfaceMicrostrip = data;
                        break;
                    case csType.BuriedMicrostrip:
                        sbData.BuriedMicrostrip = data;
                        break;
                    case csType.Stripline:
                        sbData.Stripline = data;
                        break;
                }
            }

            LoadData();
            SolveAll();
        }
        
        private void OnCSTypeLeave()
        {
            SaveData();
        }

        private void SaveData()
        {
            //surface microstrip
            sbData d = sbData.SurfaceMicrostrip;//--new sbData(csType.SurfaceMicrostrip);
            d.t = Convert.ToDouble(tb_Surface_t.Text);
            d.w = Convert.ToDouble(tb_Surface_w.Text);
            d.w2 = Convert.ToDouble(tb_Surface_w2.Text);
            d.h1 = Convert.ToDouble(tb_Surface_h.Text);
            d.Dk1 = Convert.ToDouble(tb_Surface_Dk.Text);
            d.Df1 = Convert.ToDouble(tb_Surface_Df.Text);
            d.Z0 = Convert.ToDouble(tb_Surface_Z0.Text);
            d.Rz1 = Convert.ToDouble(tb_Surface_Rz.Text);
            d.Rz2 = d.Rz1;

            //solder mask
            d.hasSolderMask = cbSolderMask.Checked;
            d.h2 = Convert.ToDouble(tb_Surface_sm_h.Text);
            d.h2_2 = tb_Surface_sm_h2.Text.Length > 0 ? Convert.ToDouble(tb_Surface_sm_h2.Text) : d.h2;
            d.Dk2 = Convert.ToDouble(tb_Surface_sm_Dk.Text);
            d.Df2 = Convert.ToDouble(tb_Surface_sm_Df.Text);

            d.s = Convert.ToDouble(tb_Surface_s.Text);
             //d.Z0diff = Convert.ToDouble(tb_Surface_Z0diff.Text);
            d.Zdiff = Convert.ToDouble(tb_Surface_Zdiff.Text);

            d.len = Convert.ToDouble(tb_Surface_len.Text);
            d.f = Convert.ToDouble(tb_Surface_f.Text);

            d.etch = CalculateEtch(d);
            d.isEtch = d.etch != 0;//!!!
            d.isDiffImpedance = cbDiffSurface.Checked;

             //--sbData.SurfaceMicrostrip = d;

            // buried microstrip
            d = sbData.BuriedMicrostrip;//--new sbData(csType.BuriedMicrostrip);
            d.t = Convert.ToDouble(tb_Buried_t.Text);
            d.w = Convert.ToDouble(tb_Buried_w.Text);
            d.w2 = Convert.ToDouble(tb_Buried_w2.Text);
            d.h1 = Convert.ToDouble(tb_Buried_h1.Text);
            d.h2 = Convert.ToDouble(tb_Buried_h2.Text);
            d.Dk1 = Convert.ToDouble(tb_Buried_Dk1.Text);
            d.Dk2 = Convert.ToDouble(tb_Buried_Dk2.Text);
            d.Dkr = Convert.ToDouble(tb_Buried_Dkr.Text);
            d.Df1 = Convert.ToDouble(tb_Buried_Df1.Text);
            d.Df2 = Convert.ToDouble(tb_Buried_Df2.Text);
            d.Dfr = Convert.ToDouble(tb_Buried_Dfr.Text);

            d.Z0 = Convert.ToDouble(tb_Buried_Z0.Text);
            d.Rz1 = Convert.ToDouble(tb_Buried_Rz.Text);
            d.Rz2 = d.Rz1;

            d.s = Convert.ToDouble(tb_Buried_s.Text);
            //--d.Z0diff = Convert.ToDouble(tb_Buried_Z0diff.Text);
            d.Zdiff = Convert.ToDouble(tb_Buried_Zdiff.Text);

            d.len = Convert.ToDouble(tb_Buried_len.Text);
            d.f = Convert.ToDouble(tb_Buried_f.Text);

            d.etch = CalculateEtch(d);
            d.isEtch = d.etch != 0;
            d.isSymmetrical = cb_Buried_Symmetrical.Checked;
            d.isDiffImpedance = cbDiffBuried.Checked;

            //--sbData.BuriedMicrostrip = d;

        //stripline
            d = sbData.Stripline;//--new sbData(csType.Stripline);
            d.t = Convert.ToDouble(tb_Stripline_t.Text);
            d.w = Convert.ToDouble(tb_Stripline_w.Text);
            d.w2 = Convert.ToDouble(tb_Stripline_w2.Text);
            d.h1 = Convert.ToDouble(tb_Stripline_h1.Text);
            d.h2 = Convert.ToDouble(tb_Stripline_h2.Text);
            d.Dk1 = Convert.ToDouble(tb_Stripline_Dk1.Text);
            d.Dk2 = Convert.ToDouble(tb_Stripline_Dk2.Text);
            d.Dkr = Convert.ToDouble(tb_Stripline_Dkr.Text);
            d.Df1 = Convert.ToDouble(tb_Stripline_Df1.Text);
            d.Df2 = Convert.ToDouble(tb_Stripline_Df2.Text);
            d.Dfr = Convert.ToDouble(tb_Stripline_Dfr.Text);
            d.Z0 = Convert.ToDouble(tb_Stripline_Z0.Text);
            d.Rz1 = Convert.ToDouble(tb_Stripline_Rz1.Text);
            d.Rz2 = Convert.ToDouble(tb_Stripline_Rz2.Text);

            d.s = Convert.ToDouble(tb_Stripline_s.Text);
            //--d.Z0diff = Convert.ToDouble(tb_Stripline_Z0diff.Text);
            d.Zdiff = Convert.ToDouble(tb_Stripline_Zdiff.Text);

            d.len = Convert.ToDouble(tb_Stripline_len.Text);
            d.f = Convert.ToDouble(tb_Stripline_f.Text);

            d.etch = CalculateEtch(d);
            d.isEtch = d.etch != 0;
            d.isSymmetrical = cb_Stripline_Symmetrical.Checked;
            d.isDiffImpedance = cbDiffStripline.Checked;

            //--sbData.Stripline = d;
        }

        private void tabsCSType_Enter(object sender, EventArgs e)
        {
            /*LoadData();
            FillInComment();*/
        }
        
        private void LoadData()
        {
            bool isCalculationCanceledValue = isCalculationCanceled;
            isCalculationCanceled = true;
            bool isIgnoreSolveValue = isIgnoreSolve;
            isIgnoreSolve = true;

            //fill in the data
            //surface microstrip
            sbData d = sbData.SurfaceMicrostrip;
            tb_Surface_t.Text = d.t.ToString(Format("thickness"));
            tb_Surface_w.Text = d.w.ToString(Format("width"));
            tb_Surface_w2.Text = d.w2.ToString(Format("width"));
            tb_Surface_h.Text = d.h1.ToString(Format("height"));
            tb_Surface_Dk.Text = d.Dk1.ToString(Format("dk"));
            tb_Surface_Df.Text = d.Df1.ToString(Format("df"));
            tb_Surface_Z0.Text = d.Z0.ToString(Format("impedance"));
            tb_Surface_Rz.Text = d.Rz1.ToString(Format("height"));

            //solder mask
            cbSolderMask.Checked = d.hasSolderMask;
            tb_Surface_sm_h.Text = d.h2.ToString(Format("height"));
            tb_Surface_sm_h2.Text = d.h2_2.ToString(Format("height"));
            tb_Surface_sm_Dk.Text = d.Dk2.ToString(Format("dk"));
            tb_Surface_sm_Df.Text = d.Df2.ToString(Format("df"));


            tb_Surface_s.Text = d.s.ToString(Format("spacing"));
            //--tb_Surface_Z0diff.Text = d.Z0diff.ToString(Format("impedance"));
            tb_Surface_Zdiff.Text = d.Zdiff.ToString(Format("impedance"));

            tb_Surface_len.Text = d.len.ToString(Format("width"));
            tb_Surface_f.Text = d.f.ToString(Format("frequency"));

            cbDiffSurface.Checked = d.isDiffImpedance;

            // buried microstrip
            d = sbData.BuriedMicrostrip;
            tb_Buried_t.Text = d.t.ToString(Format("thickness"));
            tb_Buried_w.Text = d.w.ToString(Format("width"));
            tb_Buried_w2.Text = d.w2.ToString(Format("width"));
            tb_Buried_h1.Text = d.h1.ToString(Format("height"));
            tb_Buried_h2.Text = d.h2.ToString(Format("height"));
            tb_Buried_Dk1.Text = d.Dk1.ToString(Format("dk"));
            tb_Buried_Dk2.Text = d.Dk2.ToString(Format("dk"));
            tb_Buried_Dkr.Text = d.Dkr.ToString(Format("dk"));
            tb_Buried_Df1.Text = d.Df1.ToString(Format("df"));
            tb_Buried_Df2.Text = d.Df2.ToString(Format("df"));
            tb_Buried_Dfr.Text = d.Dfr.ToString(Format("df"));
            tb_Buried_Z0.Text = d.Z0.ToString(Format("impedance"));
            tb_Buried_Rz.Text = d.Rz1.ToString(Format("height"));

            tb_Buried_s.Text = d.s.ToString(Format("spacing"));
            //--tb_Buried_Z0diff.Text = d.Z0diff.ToString(Format("impedance"));
            tb_Buried_Zdiff.Text = d.Zdiff.ToString(Format("impedance"));

            tb_Buried_len.Text = d.len.ToString(Format("width"));
            tb_Buried_f.Text = d.f.ToString(Format("frequency"));

            cb_Buried_Symmetrical.Checked = d.isSymmetrical;
            cbDiffBuried.Checked = d.isDiffImpedance;

            //stripline
            d = sbData.Stripline;
            tb_Stripline_t.Text = d.t.ToString(Format("thickness"));
            tb_Stripline_w.Text = d.w.ToString(Format("width"));
            tb_Stripline_w2.Text = d.w2.ToString(Format("width"));
            tb_Stripline_h1.Text = d.h1.ToString(Format("height"));
            tb_Stripline_h2.Text = d.h2.ToString(Format("height"));
            tb_Stripline_Dk1.Text = d.Dk1.ToString(Format("dk"));
            tb_Stripline_Dk2.Text = d.Dk2.ToString(Format("dk"));
            tb_Stripline_Dkr.Text = d.Dkr.ToString(Format("dk"));
            tb_Stripline_Df1.Text = d.Df1.ToString(Format("df"));
            tb_Stripline_Dfr.Text = d.Dfr.ToString(Format("df"));
            tb_Stripline_Df2.Text = d.Df2.ToString(Format("df"));
            tb_Stripline_Z0.Text = d.Z0.ToString(Format("impedance"));
            tb_Stripline_Rz1.Text = d.Rz1.ToString(Format("height"));
            tb_Stripline_Rz2.Text = d.Rz2.ToString(Format("height"));

            tb_Stripline_s.Text = d.s.ToString(Format("spacing"));
            //--tb_Stripline_Z0diff.Text = d.Z0diff.ToString(Format("impedance"));
            tb_Stripline_Zdiff.Text = d.Zdiff.ToString(Format("impedance"));

            tb_Stripline_len.Text = d.len.ToString(Format("width"));
            tb_Stripline_f.Text = d.f.ToString(Format("frequency"));

            cb_Stripline_Symmetrical.Checked = d.isSymmetrical;
            cbDiffStripline.Checked = d.isDiffImpedance;
#if ZSANDBOX
            ZZero.ZSolver.ZSolverManager.IsTrapezoidalTraces = d.isEtch;
#endif

            isIgnoreSolve = isIgnoreSolveValue;
            isCalculationCanceled = isCalculationCanceledValue;
        }

        private string Format(string field)
        {
            switch (field) {
                case "impedance" : return "N" + Options.TheOptions.impedanceDigits.ToString();
                case "thickness": return "N" + Options.TheOptions.lengthDigits.ToString();
                case "width": return "N" + Options.TheOptions.lengthDigits.ToString();
                case "height": return "N" + Options.TheOptions.lengthDigits.ToString();
                case "spacing": return "N" + Options.TheOptions.lengthDigits.ToString();
                case "dk": return "N" + Options.TheOptions.dkDigits.ToString();
                case "df": return "N" + Options.TheOptions.dfDigits.ToString();
                case "frequency": return "N" + Options.TheOptions.frequencyDigits.ToString();
            }
            return "N1";
        }
        private string ImpedanceFormat()
        {
            return "N" + Options.TheOptions.impedanceDigits.ToString();
        }


        private void tabsCSType_Leave(object sender, EventArgs e)
        {
            /*OnCSTypeLeave();*/  //get data changes
        }

        private void SolveAll()
        {
            if (isIgnoreSolve) return;
            SolveInternal();

            // loss plot
            //--fxOut = sbData.SurfaceMicrostrip.outSE;
            DefinePlotSource();
            RecalcChart();
            FillInComment();
        }

        private double PropagationSpeed(FX_Output output) //inch/ns
        {
            double c1 = Units.MetersToMils * 0.001; //meters to inch
            double c2 = 1.0e-9; //S to ns
            return output.fPropSpeeds[0] * c1 * c2;
        }

        private double Delay(FX_Output output, double len) //ns
        {
            double c1 = Units.MetersToMils * 0.001; //meters to inch
            double c2 = 1.0e-9; //S to ns
            return len / (output.fPropSpeeds[0] * c1 * c2);
        }

        private double LossPerLength(FX_Output output, double freq, bool bRough)
        {
            FXLosses loss = new FXLosses(output);
            double f = freq;
            double R = loss.Attenuation_R(f);
            double Ro = loss.Attenuation_Ro(f);
            double D = loss.Attenuation_D(f);

            double totalLoss = R + D;
            if (bRough)
            {
                totalLoss += Ro;
            }
            totalLoss *= 0.0254;//[db/inch]

            return totalLoss;
        }

        private double TotalLoss(FX_Output output, double freq, bool bRough, double len) //dB
        {
            return LossPerLength(output, freq, bRough ) * len;
        }

        private void SolveInternal()
        {
            if (isIgnoreSolve) return;
            OnCSTypeLeave();  //get data changes
            
            sbData d = sbData.SurfaceMicrostrip;
            FX.RunOnSandboxCX(ref d);
            tb_Surface_Z0.Text = d.Z0.ToString(ImpedanceFormat());
            //--tb_Surface_Z0diff.Text = d.Z0diff.ToString(ImpedanceFormat());
            tb_Surface_Zdiff.Text = d.Zdiff.ToString(ImpedanceFormat());
            //delay & loss
            tb_Surface_propagation_speed.Text = PropagationSpeed(d.outSE).ToString("N2");
            tb_Surface_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Surface_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Surface_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");

            d = sbData.BuriedMicrostrip;
            FX.RunOnSandboxCX(ref d);
            tb_Buried_Z0.Text = d.Z0.ToString(ImpedanceFormat());
            //--tb_Buried_Z0diff.Text = d.Z0diff.ToString(ImpedanceFormat());
            tb_Buried_Zdiff.Text = d.Zdiff.ToString(ImpedanceFormat());
            //delay & loss
            tb_Buried_propagation_speed.Text = PropagationSpeed(d.outSE).ToString("N2");
            tb_Buried_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Buried_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Buried_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");


            d = sbData.Stripline;
            FX.RunOnSandboxCX(ref d);
            tb_Stripline_Z0.Text = d.Z0.ToString(ImpedanceFormat());
            //--tb_Stripline_Z0diff.Text = d.Z0diff.ToString(ImpedanceFormat());
            tb_Stripline_Zdiff.Text = d.Zdiff.ToString(ImpedanceFormat());
            //delay & loss
            tb_Stripline_propagation_speed.Text = PropagationSpeed(d.outSE).ToString("N2");
            tb_Stripline_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Stripline_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Stripline_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

//validation
        private void tb_Surface_t_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Thickness", 0.1, 30))
            {
                SolveAll();
            }
        }

        private void tb_Surface_w_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Width", 0.5, 100))
            {
                SolveAll();
            }
        }

        private void tb_Surface_h_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Dielectric Height", 1.5, 50))
            {
                if (sender == tb_Buried_h2) { if (cb_Buried_Symmetrical.Checked) tb_Buried_h1.Text = tb_Buried_h2.Text; }
                if (sender == tb_Stripline_h2) { if (cb_Stripline_Symmetrical.Checked) tb_Stripline_h1.Text = tb_Stripline_h2.Text; }
                SolveAll();
            }
        }

        private void tb_Surface_Dk_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Dk", 1, 7))
            {
                if (sender == tb_Buried_Dk2) { if (cb_Buried_Symmetrical.Checked) tb_Buried_Dk1.Text = tb_Buried_Dk2.Text; }
                if (sender == tb_Stripline_Dk2) { if (cb_Stripline_Symmetrical.Checked) tb_Stripline_Dk1.Text = tb_Stripline_Dk2.Text; }
                SolveAll();
            }
        }

        private void tb_Surface_Df_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Df", 0.00, 1.00))
            {
                if (sender == tb_Buried_Df2) { if (cb_Buried_Symmetrical.Checked) tb_Buried_Df1.Text = tb_Buried_Df2.Text; }
                if (sender == tb_Stripline_Df2) { if (cb_Stripline_Symmetrical.Checked) tb_Stripline_Df1.Text = tb_Stripline_Df2.Text; }
                SolveAll();
            }
        }

        private void tb_Surface_s_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Spacing", 0.5, 100))
            {
                SolveAll();
            }
        }

//===================================================================================
// Loss Plot
//===================================================================================
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

            chartLoss.ChartAreas["chartAreaLoss"].AxisY.Title = rbCm.Checked ? "Attenuation [dB/cm]" : "Attenuation [dB/in]";
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
            double f = 0;//start from 0 to always calc dielectric loss domination point //Fmin;
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
            //chartLoss.ChartAreas["chartAreaLoss"].AxisX.MajorGrid.Interval = 1; // (int)((Fmax - Fmin) / 10 + 0.5); //closest whole number - ???????
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.Enabled = true;
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.Interval = chartLoss.ChartAreas["chartAreaLoss"].AxisX.MajorGrid.Interval / 2;
            chartLoss.ChartAreas["chartAreaLoss"].AxisX.MinorGrid.LineDashStyle = ChartDashStyle.Dot;

            chartLoss.ChartAreas["chartAreaLoss"].AxisY.IntervalAutoMode = IntervalAutoMode.FixedCount;
            //chartLoss.ChartAreas["chartAreaLoss"].AxisY.MajorGrid.Interval = 0.1; // (chartLoss.ChartAreas["chartAreaLoss"].AxisY.Maximum - chartLoss.ChartAreas["chartAreaLoss"].AxisY.Minimum) / 10;
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

            chartLoss.Update();
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
                tbCursorDielectric.Text = cbDielectric.Checked ? aD.ToString("N2") : "";

                if (cbRoughness.Checked) aT += aRo;
                if (cbResistive.Checked) aT += aRe;
                if (cbDielectric.Checked) aT += aD;

                tbCursorTotal.Text = cbTotal.Checked ? aT.ToString("N2") : ""; 
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
            chartLoss.Series["Roughness"].Enabled = cbRoughness.Enabled ? cbRoughness.Checked : false;
            //RecalcChart();
            SolveAll();
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


        private void btnPrint_Click(object sender, EventArgs e)
        {
            CaptureScreen();

            /*PrintPreviewDialog*/
            PrintDialog dlg = new PrintDialog();//PrintPreviewDialog();
            dlg.Document = printDocument1;
            DialogResult result = dlg.ShowDialog();

            if (result == DialogResult.OK)
            {
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

        private void btnClose_Click_1(object sender, EventArgs e)
        {
            Close();
        }

        private void FillInComment()
        {
            string s;
            switch (tabCSType.SelectedIndex)
            {
                case 0:
                    s = String.Format("Microstrip: W = {0}, T = {1}, H = {2} (mils)", tb_Surface_w.Text, tb_Surface_t.Text, tb_Surface_h.Text);
                    tbComment.Text = s;
                    break;
                case 1:
                    s = String.Format("Buried Microstrip: W = {0}, T = {1}, H(above) = {2} H(below) = {3} (mils)", tb_Buried_w.Text, tb_Buried_t.Text, tb_Buried_h2.Text, tb_Buried_h1.Text);
                    tbComment.Text = s;
                    break;
                case 2:
                    s = String.Format("Stripline: W = {0}, T = {1}, H(above) = {2} H(below) = {3} (mils)", tb_Stripline_w.Text, tb_Stripline_t.Text, tb_Stripline_h2.Text, tb_Stripline_h1.Text);
                    tbComment.Text = s;
                    break;
            }
        }

        private void tabCSType_SelectedIndexChanged(object sender, EventArgs e)
        {
            tabCSType.SelectedTab.Controls.Add(this.panel1);
            DefinePlotSource();
            RecalcChart();
            FillInComment();
        }

        private void DefinePlotSource()
        {
            bFreeze = false;

            switch (tabCSType.SelectedIndex)
            {
                case 0:
                    fxOut = cbDiffSurface.Checked ? sbData.SurfaceMicrostrip.outDiff : sbData.SurfaceMicrostrip.outSE;
                    break;
                case 1:
                    fxOut = cbDiffBuried.Checked ? sbData.BuriedMicrostrip.outDiff : sbData.BuriedMicrostrip.outSE;
                    break;
                case 2:
                    fxOut = cbDiffStripline.Checked ? sbData.Stripline.outDiff : sbData.Stripline.outSE;
                    break;
            }
        }

        private void tb_Surface_Rz_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Roughness", 0.5, 15))
            {
                SolveAll();
            }
        }

        private void tb_Buried_Rz_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Roughness", 0.5, 15))
            {
                SolveAll();
            }
        }

        private void FXSandboxPanel_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SelectNextControl(ActiveControl, true, true, true, true);
                e.Handled = true;
            }
        }

        private void tb_Surface_len_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.SurfaceMicrostrip;
            tb_Surface_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Surface_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void tb_Buried_len_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.BuriedMicrostrip;
            tb_Buried_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Buried_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void tb_Stripline_len_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.Stripline;
            tb_Stripline_delay.Text = Delay(d.outSE, d.len).ToString("N1");
            tb_Stripline_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void tb_Surface_f_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.SurfaceMicrostrip;
            tb_Surface_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Surface_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void tb_Buried_f_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.BuriedMicrostrip;
            tb_Buried_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Buried_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void tb_Stripline_f_Validating(object sender, CancelEventArgs e)
        {
            SaveData();

            sbData d = sbData.Stripline;
            tb_Stripline_insertion_loss.Text = LossPerLength(d.outSE, d.f, cbRoughness.Checked).ToString("N2");
            tb_Stripline_loss.Text = TotalLoss(d.outSE, d.f, cbRoughness.Checked, d.len).ToString("N2");
        }

        private void FXSandboxPanel_Load(object sender, EventArgs e)
        {
            LoadData();
            FillInComment();
        }

        private void FXSandboxPanel_FormClosing(object sender, FormClosingEventArgs e)
        {
            OnCSTypeLeave();  //get data changes
        }

        private string buried_h1_text = string.Empty;
        private string buried_dk1_text = string.Empty;
        private string buried_df1_text = string.Empty;

        private void cb_Stripline_Symmetrical_CheckedChanged(object sender, EventArgs e)
        {
            tb_Stripline_h1.Enabled = !cb_Stripline_Symmetrical.Checked;
            tb_Stripline_Dk1.Enabled = !cb_Stripline_Symmetrical.Checked;
            tb_Stripline_Df1.Enabled = !cb_Stripline_Symmetrical.Checked;

            if (cb_Stripline_Symmetrical.Checked)
            {
                buried_h1_text = tb_Stripline_h2.Text;//sbData.Stripline.h2.ToString(Format("height"));
                buried_dk1_text = tb_Stripline_Dk1.Text;
                buried_df1_text = tb_Stripline_Df1.Text;

                tb_Stripline_h1.Text = tb_Stripline_h2.Text;
                tb_Stripline_Dk1.Text = tb_Stripline_Dk2.Text;
                tb_Stripline_Df1.Text = tb_Stripline_Df2.Text;
            }
            else
            {
                tb_Stripline_h1.Text = buried_h1_text;
                tb_Stripline_Dk1.Text = buried_dk1_text;
                tb_Stripline_Df1.Text = buried_df1_text;
            }

            SolveAll();
        }

        private void cb_Buried_Symmetrical_CheckedChanged(object sender, EventArgs e)
        {
            tb_Buried_h1.Enabled = !cb_Buried_Symmetrical.Checked;
            tb_Buried_Dk1.Enabled = !cb_Buried_Symmetrical.Checked;
            tb_Buried_Df1.Enabled = !cb_Buried_Symmetrical.Checked;

            if (cb_Buried_Symmetrical.Checked)
            {
                buried_h1_text = tb_Buried_h1.Text;
                buried_dk1_text = tb_Buried_Dk1.Text;
                buried_df1_text = tb_Buried_Df1.Text;

                tb_Buried_h1.Text = tb_Buried_h2.Text;
                tb_Buried_Dk1.Text = tb_Buried_Dk2.Text;
                tb_Buried_Df1.Text = tb_Buried_Df2.Text;
            }
            else
            {
                tb_Buried_h1.Text = buried_h1_text;
                tb_Buried_Dk1.Text = buried_dk1_text;
                tb_Buried_Df1.Text = buried_df1_text;
            }

            SolveAll();
        }

        bool ignoreEtchUpdate = false;

        private double CalculateEtch(sbData data)
        {
            //HL etch factor
            double hlEtch = 0; //(dw/t)

            try
            {
                hlEtch = Math.Abs(data.w - data.w2) / data.t;
            }
            catch {
                hlEtch = 0;
            }
            return hlEtch;
        }


        private void EtchUpdate(bool enabled)
        {
            if (!ignoreEtchUpdate)
            {
#if ZSANDBOX
            ZZero.ZSolver.ZSolverManager.IsTrapezoidalTraces = enabled;
#endif
                SolveInternal();
                DefinePlotSource();
                RecalcChart();
            }
        }

        private void cbSolderMask_CheckedChanged(object sender, EventArgs e)
        {
            panelSolderMask.Enabled = cbSolderMask.Checked;
            SolveAll();
        }

        private void tb_Surface_sm_h_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Thickness", 0.1, 30))
            {
                SolveAll();
            }
        }
        private void tb_Surface_sm_h2_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Thickness", 0.1, 30))
            {
                SolveAll();
            }
        }

        private void tb_Surface_sm_Dk_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Dk", 1, 7))
            {
                SolveAll();
            }
        }

        private void tb_Surface_sm_Df_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Df", 0.00, 1.00))
            {
                SolveAll();
            }
        }

        private void tb_Surface_w2_Validating(object sender, CancelEventArgs e)
        {
            double w1, minW, maxW;
            if (Double.TryParse(tb_Surface_w.Text, out w1))
            {
                minW = 0.5 * w1;
                maxW = w1;
                if (TextBoxValidator.CheckRange(sender, "Width", minW, maxW))
                {
                    SolveAll();
                }
            }
        }

        private string ReportLayer(sbData data, int stk_index, LAYER_TYPE type, bool bFirst = true, bool bConformal = false)
        {
            string s = "";
            double h = bFirst ? data.h1 : data.h2;
            double dk = bFirst ? data.Dk1 : data.Dk2;
            double df = bFirst ? data.Df1 : data.Df2;
            double metalDk = stk_index == 0 ? 1 : 4;
            double metalDf = stk_index == 0 ? 0 : 0.02;
            //internal signal layer
            if (stk_index > 0 && type == LAYER_TYPE.FS_ROUTING)
            {
                metalDk = (data.Dk1 + data.Dk2) / 2; //average of adjacent dielectrics
            }

            switch (type)
            {
                case LAYER_TYPE.FS_DIEL:
                    if (bConformal)
                    {
                        s += "Conformal\t";
                    }
                    else
                    {
                        s += "Dielectric\t";
                    }
                    break;
                case LAYER_TYPE.FS_PLANE:
                    s += "Plane\t\t";
                    break;
                case  LAYER_TYPE.FS_ROUTING:
                    s += "Signal\t\t";
                    break;
            }
            double t = type == LAYER_TYPE.FS_DIEL ? h : data.t;
            s += t.ToString("N2");

            double diel = type == LAYER_TYPE.FS_DIEL ? dk : metalDk;
            s += "\t" + diel.ToString("N2");

            return s;
        }

        private string ReportStackup(sbData data)
        {
            string s = "Type\tThickness[mils]\tDk" + Environment.NewLine;
            switch (data.type)
            {
                case csType.SurfaceMicrostrip:
                    if (data.hasSolderMask)
                    {
                        s += ReportLayer(data, 0, LAYER_TYPE.FS_DIEL, false, true) + Environment.NewLine;
                        s += ReportLayer(data, 1, LAYER_TYPE.FS_ROUTING) + Environment.NewLine;
                        s += ReportLayer(data, 2, LAYER_TYPE.FS_DIEL) + Environment.NewLine;
                        s += ReportLayer(data, 3, LAYER_TYPE.FS_PLANE) + Environment.NewLine;
                    }
                    else
                    {
                        s += ReportLayer(data, 0, LAYER_TYPE.FS_ROUTING) + Environment.NewLine;
                        s += ReportLayer(data, 1, LAYER_TYPE.FS_DIEL) + Environment.NewLine;
                        s += ReportLayer(data, 2, LAYER_TYPE.FS_PLANE) + Environment.NewLine;
                    }
                    break;
                case csType.BuriedMicrostrip:
                    s += ReportLayer(data, 0, LAYER_TYPE.FS_DIEL, false) + Environment.NewLine;
                    s += ReportLayer(data, 1, LAYER_TYPE.FS_ROUTING) + Environment.NewLine;
                    s += ReportLayer(data, 2, LAYER_TYPE.FS_DIEL) + Environment.NewLine;
                    s += ReportLayer(data, 3, LAYER_TYPE.FS_PLANE) + Environment.NewLine;
                    break;
                case csType.Stripline:
                    s += ReportLayer(data, 0, LAYER_TYPE.FS_PLANE) + Environment.NewLine;
                    s += ReportLayer(data, 1, LAYER_TYPE.FS_DIEL, false) + Environment.NewLine;
                    s += ReportLayer(data, 2, LAYER_TYPE.FS_ROUTING) + Environment.NewLine;
                    s += ReportLayer(data, 3, LAYER_TYPE.FS_DIEL) + Environment.NewLine;
                    s += ReportLayer(data, 4, LAYER_TYPE.FS_PLANE) + Environment.NewLine;
                    break;
            }

            return s;
        }

        private string ReportTraces(sbData data)
        {
            string s = "";
            s += String.Format("Layer : Signal, Width : {0} [mils], Separation : {1} [mils]", data.w, data.s);
            return s;
        }

        string DashLine()
        {
            return "----------------------------------------------------------------------------" + Environment.NewLine; 
        }

        string AsteriskLine()
        {
            return "****************************************************************************" + Environment.NewLine; 
        }

        string EndLine()
        {
            return "***************************** (END) ****************************************" + Environment.NewLine; 
        }

        string ReportHeader(sbData data)
        {
            string s = AsteriskLine();
            s += "Z-ZERO Z-SOLVER" + Environment.NewLine;
            s += AsteriskLine();
            s += "HyperLynx Field Solver Results" + Environment.NewLine;
            s += Environment.NewLine;

            s += AsteriskLine();
            switch (data.type)
            {
                case csType.SurfaceMicrostrip:
                    s += "SURFACE MICROSTRIP";
                    break;
                case csType.BuriedMicrostrip:
                    s += "BURIED MICROSTRIP";
                    break;
                case csType.Stripline:
                    s += "STRIPLINE";
                    break;
            }
            s += Environment.NewLine;
            s += AsteriskLine();
            s += Environment.NewLine;

            return s;
        }

        string ReportPicture(sbData data)
        {
            string s  = "";
            switch (data.type)
            {
                case csType.SurfaceMicrostrip:
                    s +="                AIR" + Environment.NewLine;
                    s +="        w2                 c1" + Environment.NewLine;
                    s +="     +======+           +======+ "     + Environment.NewLine;
                    s +=" t //        \\\\       //        \\\\  c2" + Environment.NewLine;
                    s +="===+----------+=======+----------+=====" + Environment.NewLine;
                    s +="  ^     w1    |<--s-->|" + Environment.NewLine;
                    s +="  |" + Environment.NewLine;
                    s +="  h        DIELECTRIC  (Dk, Df)" + Environment.NewLine;
                    s +="  |" + Environment.NewLine;
                    s +="  v" + Environment.NewLine;
                    s += "---------------------------------------" + Environment.NewLine;
                    s += "              PLANE " + Environment.NewLine;                  
                    s += "---------------------------------------" + Environment.NewLine;
                    s += Environment.NewLine;
                    break;
                case csType.BuriedMicrostrip:                    
                    s += "                 AIR" + Environment.NewLine;
				    s +=  Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;				 
                    s += " ^" + Environment.NewLine;
                    s += " |" + Environment.NewLine;
                    s += " h2    DIELECTRIC2  (Dk2, Df2)" + Environment.NewLine;
                    s += " |" + Environment.NewLine;
                    s += " v      w2" + Environment.NewLine;
                    s += " --  +------+         +------+" + Environment.NewLine;     
                    s += "    /        \\|<-s->|/        \\  t  (Dkr, Dfr)" + Environment.NewLine;
                    s += "---+----------+     +----------+" + Environment.NewLine;
                    s += " ^      w1" + Environment.NewLine;
                    s += " |" + Environment.NewLine;
                    s += " h1    DIELECTRIC1  (Dk1, Df1)" + Environment.NewLine;
                    s += " |" + Environment.NewLine;
                    s += " v" + Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += "              PLANE" + Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += Environment.NewLine;                     
                    break;
                case csType.Stripline:                    
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += "                   PLANE" + Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += "  ^" + Environment.NewLine;
                    s += "  |" + Environment.NewLine;
                    s += "  h2        DIELECTRIC2  (Dk2, Df2)" + Environment.NewLine;
                    s += "  |" + Environment.NewLine;
                    s += "  v         w2" + Environment.NewLine;
                    s += "------   +------+         +------+  -------" + Environment.NewLine;
                    s += "        /        \\|<-s->|/        \\  t  (Dkr, Dfr)" + Environment.NewLine;
                    s += "-------+----------+     +----------+ ------" + Environment.NewLine;
                    s += "  ^         w1" + Environment.NewLine;
                    s += "  |" + Environment.NewLine;
                    s += "  h1        DIELECTRIC1  (Dk1, Df1)" + Environment.NewLine;
                    s += "  |" + Environment.NewLine;
                    s += "  v" + Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += "                   PLANE" + Environment.NewLine;
                    s += "-----------------------------------------------------" + Environment.NewLine;
                    s += Environment.NewLine;                     
                    break;
            }
            return s;
        }

        string ReportSolderMask(sbData data)
        {
            if (data.type != csType.SurfaceMicrostrip) return "";
            if (!cbSolderMask.Checked) return "";

            string s = AsteriskLine();
            s += "SOLDER MASK (CONFORMAL COATING):" + Environment.NewLine;
            s += AsteriskLine();

            s += "Above trace (c1):" + "\t\t";
            s += data.h2.ToString(Format("height"));
            s += " (mils)" + Environment.NewLine;

            s += "Between traces (c2):" + "\t\t";
            s += data.h2_2.ToString(Format("height"));
            s += " (mils)" + Environment.NewLine;

            s += "Dielectric constant (Dk):" + "\t";
            s += data.Dk2.ToString(Format("dk")) + Environment.NewLine;

            s += "Dissipation factor (Df):" + "\t";
            s += data.Df2.ToString(Format("df")) + Environment.NewLine;

            s += Environment.NewLine;

            return s;
        }

        string ReportRLGC(sbData data)
        {
            string s = AsteriskLine();
            s += "RLGC MATRICES:" + Environment.NewLine;
            s += AsteriskLine();

            s += ReportMatrix("Characteristic Impedance Matrix (Ohms):", data.outDiff.fZzero);

            s += ReportMatrix("Capacitance Matrix [pF/m]:", data.outDiff.fCapacitance, 1.0E12);
            s += "(Note: Off-diagonal values are negative by mathematical convention." + Environment.NewLine; 
            s += "However, the capacitances are physically real; you can effectively"+ Environment.NewLine; 
            s += "ignore the signs. The signs are required, though, if transporting"+ Environment.NewLine;
            s += "matrix data to another tool.)" + Environment.NewLine;
            s += Environment.NewLine;

            s += ReportMatrix("Inductance Matrix [nH/m]:", data.outDiff.fInductance, 1.0E9);
            s += ReportMatrix("DC Resistance Matrix (Ohms/m):", data.outDiff.fDcResistance);

            s += "Propagation Speed [m/s]:" + Environment.NewLine;
            if (data.outDiff.fPropSpeeds[0] != data.outDiff.fPropSpeeds[1])
            {
                s += " Differential:" + "\t" + data.outDiff.fPropSpeeds[0].ToString("0.######E+00") + Environment.NewLine;
                s += " Common:      " + "\t" + data.outDiff.fPropSpeeds[1].ToString("0.######E+00") + Environment.NewLine;
            }
            else
            {
                s += "\t" + data.outDiff.fPropSpeeds[1].ToString("0.######E+00") + Environment.NewLine;
            }
            s += Environment.NewLine;

            s += ReportMatrix("Skin-effect Resistance Matrix (Ohms/m):", data.outDiff.fSkinResistance);
            s += ReportMatrix("Dielectric Loss Matrix (1/(ohm*m)):", data.outDiff.fGMatrix, 1.0, "0.######E+00");

            return s;
        }

        string ReportMatrix(string title, List<float> m, double k = 1.0, string fmt = "N2")
        {
            string s = title + Environment.NewLine;
            s += "\t" + (m[0] * k).ToString(fmt) + "\t" + (m[1] * k).ToString(fmt) + Environment.NewLine;
            s += "\t" + (m[2] * k).ToString(fmt) + "\t" + (m[3] * k).ToString(fmt) + Environment.NewLine;
            s += Environment.NewLine;
            return s;
        }

        string ReportSingleEndedImpedance(sbData data)
        {
            string s = AsteriskLine();
            s += "SINGLE-ENDED IMPEDANCE:" + Environment.NewLine;
            s += AsteriskLine();

            s += "Trace thickness (t):" + "\t\t";
            s += data.t.ToString(Format("thickness"));
            s += " (mils)" + Environment.NewLine;

            s += "Top width (w2):" + "\t\t\t";
            s += data.w2.ToString(Format("width"));
            s += " (mils)" + Environment.NewLine;

            s += "Bottom width (w1):" + "\t\t";
            s += data.w.ToString(Format("width"));
            s += " (mils)" + Environment.NewLine;
            
            bool bSurface = data.type == csType.SurfaceMicrostrip;
            if (!bSurface)
            {
                //h2, dk2, df2, dkr, dfr
                s += "Dielectric height (h2):" + "\t\t";
                s += (data.h2 + data.t).ToString(Format("height"));
                s += " (mils)" + Environment.NewLine;

                s += "Dielectric constant (Dk2):" + "\t";
                s += data.Dk2.ToString(Format("dk")) + Environment.NewLine;

                s += "Dissipation factor (Df2):" + "\t";
                s += data.Df2.ToString(Format("df")) + Environment.NewLine;

                s += "Dielectric constant (Dkr):" + "\t";
                s += data.Dkr.ToString(Format("dk")) + Environment.NewLine;

                s += "Dissipation factor (Dfr):" + "\t";
                s += data.Dfr.ToString(Format("df")) + Environment.NewLine;

            }

            string sH = bSurface ? "h" : "h1";
            s += "Dielectric height (" + sH + "):\t\t";
            s += data.h1.ToString(Format("height"));
            s += " (mils)" + Environment.NewLine;

            string sDk = bSurface ? "Dk" : "Dk1";
            s += "Dielectric constant (" + sDk + "):\t";
            s += data.Dk1.ToString(Format("dk")) + Environment.NewLine;

            string sDf = bSurface ? "Df" : "Df1";
            s += "Dissipation factor (" + sDf + "):\t";
            s += data.Df1.ToString(Format("df")) + Environment.NewLine;

            s += "Impedance (Z0):" + "\t\t\t";
            s += data.Z0.ToString(ImpedanceFormat());
            s += " (Ohms)" + Environment.NewLine;

            s += Environment.NewLine;
            return s;
        }

        string ReportDiffImpedance(sbData data)
        {
            string s = AsteriskLine();
            s += "DIFFERENTIAL IMPEDANCE:" + Environment.NewLine;
            s += AsteriskLine();

            s += "Differential spacing (s):" + "\t";
            s += data.s.ToString(Format("spacing"));
            s += " (mils)" + Environment.NewLine;

            s += "Differential impedance (Zdiff):" + "\t";
            s += data.Zdiff.ToString(ImpedanceFormat());
            s += " (Ohms)" + Environment.NewLine;

            s += Environment.NewLine;
            return s;
        }

        string GetFSResultsReport(sbData data)
        {
            string s = ReportHeader(data);

            s += ReportPicture(data);

            s += ReportSolderMask(data);

            s += ReportSingleEndedImpedance(data);

            s += ReportDiffImpedance(data);

            s += ReportRLGC(data);

            s += EndLine();
            return s;
        }

        private void btnResults_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Text files (*.txt)|*.txt";
            saveFileDialog.Title = "Export Numerical Results";
            saveFileDialog.InitialDirectory = Options.TheOptions.ExportPath;
            
            //YY-MM-DD-Field Solver-Microstrip-w.txt
            sbData output = null;
            string fName = DateTime.Now.ToString("yy-MM-dd") + "-Z-solver-";
            switch (tabCSType.SelectedIndex)
            {
                case 0:
                    output = sbData.SurfaceMicrostrip;
                    fName += "Microstrip-";
                    break;
                case 1:
                    output = sbData.BuriedMicrostrip;
                    fName += "Embedded-";
                    break;
                case 2:
                    output = sbData.Stripline;
                    fName += "Stripline-";
                    break;
            }
            saveFileDialog.FileName = fName + output.w.ToString("N2") + ".txt";


            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
            {
                //write data to file
                File.WriteAllText(saveFileDialog.FileName, GetFSResultsReport(output));
                try
                {
                    string arg = String.Format("\"{0}\"", saveFileDialog.FileName);
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = "NOTEPAD.EXE";
                    startInfo.Arguments = arg;
                    Process.Start(startInfo);
                }
                catch { }
            }
        }

        private void tb_Buried_Dkr_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Dk", 1, 7))
            {
                SolveAll();
            }
        }

        private void tb_Buried_Dfr_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Df", 0.00, 1.00))
            {
                SolveAll();
            }
        }

        private void tb_Stripline_Dkr_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Dk", 1, 7))
            {
                SolveAll();
            }
        }

        private void tb_Stripline_Dfr_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Df", 0.00, 1.00))
            {
                SolveAll();
            }
        }

        private void tb_Buried_w2_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Width", 0.5, 100))
            {
                SolveAll();
            }
        }

        private void tb_Stripline_w2_Validating(object sender, CancelEventArgs e)
        {
            if (TextBoxValidator.CheckRange(sender, "Width", 0.5, 100))
            {
                SolveAll();
            }
        }
    }
}
