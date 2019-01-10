using ClosedXML.Excel;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Drawing.Imaging;
using System.Drawing.Printing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Windows.Forms.DataVisualization.Charting;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.FXInterface;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;



/*
In a response that assumes an X-Y graph:
X could always be Df, or X could be any of the following: Td, Tg, Dk, Z-CTE(%), or XY-CTE (ppm/C).
Y could be any of the following: Td, Tg, Dk, Z-CTE(%), or XY-CTE (ppm/C).
I guess that Y could also be Df.
 
In a response that corresponds to the radial graph, the short list of variables, I guess, could be 
Dk, Df, Td, Tg, Z-CTE(%), and XY-CTE (ppm/C).
 */
/*
Dk,Df,XY-CTE, z-CTE smaller
Tg,Td higher
*/
namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class MaterialMapper : Form
    {
        class MM_ExcelExporter
        {    
            public enum ExportFlags{
                Manufacturer,
                Material_Name,
                Halogen_free,
                Frequency,
                Dk,
                Df,
                Tg,
                Z_CTE,
                Z_CTEpre,
                Z_CTEpost,
                XY_CTE,
                Peel_Strength,
                WA,
                Td,
                T260,
                T288            
                }

            public class ExportOptions : HashSet<ExportFlags>
            {
                public ExportOptions(bool Default = true)
                {
                    if (Default)
                    {
                        Add(ExportFlags.Manufacturer);
                        Add(ExportFlags.Material_Name);
                        Add(ExportFlags.Halogen_free);
                        Add(ExportFlags.Frequency);
                        Add(ExportFlags.Dk);
                        Add(ExportFlags.Df);
                        Add(ExportFlags.Tg);
                        Add(ExportFlags.Z_CTE);
                        Add(ExportFlags.Z_CTEpre);
                        Add(ExportFlags.Z_CTEpost);
                        Add(ExportFlags.XY_CTE);
                        Add(ExportFlags.Peel_Strength);
                        Add(ExportFlags.WA);
                        Add(ExportFlags.Td);
                        Add(ExportFlags.T260);
                        Add(ExportFlags.T288);
                    }
                }

            }

            string fileName;
            ExportOptions options;

            public MM_ExcelExporter(ExportOptions opt, string file)
            {
                fileName = file;
                options = opt;
            }

            public double GetDouble(string value)
            {
                try
                {
                    return Double.Parse(value);
                }
                catch
                {
                    return 0;
                }
            }

            public class MaterialData
            {
                public ZMaterial material { get; set; }
                public double Tg { get; set; }
                public double Df { get; set; }
            }

            public List<MaterialData> Sort(List<cMaterialWrapper> materials)
            {
                List<MaterialData> mData, mDataSorted;
                    mData = (from m in materials
                            select new MaterialData
                            {
                                material = m.material,
                                Tg = GetDouble(m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDTg).Value),
                                Df = GetDouble(m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf_1GHz).Value)
                            }).ToList();

                    mDataSorted = (from x in mData
                           orderby x.Df, x.Tg
                           select x).ToList();

                    return mDataSorted;
            }

            public void Export(List<cMaterialWrapper> materials)
            {
                //sort materials by Tg and Df  
                List<MaterialData> mData = Sort(materials);

                var workbook = new XLWorkbook(fileName);
                IXLWorksheet worksheet = workbook.Worksheet("Materials");
                //fill in the first page
                int Row1 = 3, Col1 = 2;
                int iCol = Col1;
                double dValue;
                bool bValue;
                foreach (MaterialData m in mData)
                {
                    int iRow = Row1;
                    //get material properties
                    string name = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                    string manufacturer = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value;

                    string halogen = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDHalogenFree).Value;
                    string Dk = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk_1GHz).Value;
                    string Df = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf_1GHz).Value;
                    string Tg = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDTg).Value;
                    string zCTE = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDZCTE).Value;
                    string zCTEpre = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDZCTE_alpha1).Value;
                    string zCTEpost = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDZCTE_alpha2).Value;
                    string xyCTE = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDXYCTE).Value;
                    string PeelStrength = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDPeelStrength).Value;
                    string WA = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDWA).Value;
                    string Td = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDTd).Value;
                    string T260 = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDT260).Value;
                    string T288 = m.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDT288).Value;
                    /*Halogen free?
                    Dk (1GHz)
                    Df (1GHz)
                    Tg (DSC)
                    Z-CTE (%)
                    Z-CTE, Pre-Tg (ppm/C)
                    Z-CTE, Post-Tg (ppm/C)
                    XY-CTE (ppm/C)
                    Peel Strength (lb/in)
                    WA%
                    Td (C)
                    T260 (min)
                    T288 (min)
                    */

                    //fill in cells
                    worksheet.Cell(iRow, iCol).Value = manufacturer;
                    SetHeaderCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = name;
                    SetHeaderCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;

                    if (bool.TryParse(halogen, out bValue))
                    {
                        worksheet.Cell(iRow, iCol).Value = bValue ? "Yes" : "No"; ;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    else
                    {
                        worksheet.Cell(iRow, iCol).Value = string.Empty;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    iRow++;

                    worksheet.Cell(iRow, iCol).Value = Dk;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = Df;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = Tg;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;

                    if (double.TryParse(zCTE.Replace("%", String.Empty), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                    {
                        worksheet.Cell(iRow, iCol).Value = dValue/100;
                        worksheet.Cell(iRow, iCol).Style.NumberFormat.Format = "0.0%";
                        worksheet.Cell(iRow, iCol).DataType = XLCellValues.Number;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    else
                    {
                        worksheet.Cell(iRow, iCol).Value = zCTE;
                        worksheet.Cell(iRow, iCol).DataType = XLCellValues.Text;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    iRow++;
                    
                    worksheet.Cell(iRow, iCol).Value = zCTEpre;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = zCTEpost;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    if (xyCTE.IndexOf('-') >= 0)
                    {
                        //xyCTE = xyCTE.Replace("-", " - ");
                        //worksheet.Cell(iRow, iCol).DataType = XLCellValues.Text;
                        worksheet.Cell(iRow, iCol).SetValue(xyCTE);
                    }
                    else
                    {
                        worksheet.Cell(iRow, iCol).Value = xyCTE;
                    }
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = PeelStrength;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;

                    if (double.TryParse(WA.Replace("%", String.Empty), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                    {
                        worksheet.Cell(iRow, iCol).Value = dValue/100;
                        worksheet.Cell(iRow, iCol).Style.NumberFormat.Format = "0.00%";
                        worksheet.Cell(iRow, iCol).DataType = XLCellValues.Number;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    else 
                    {
                        worksheet.Cell(iRow, iCol).Value = WA;
                        worksheet.Cell(iRow, iCol).DataType = XLCellValues.Text;
                        SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    }
                    iRow++;
                    
                    worksheet.Cell(iRow, iCol).Value = Td;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = T260;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    iRow++;
                    worksheet.Cell(iRow, iCol).Value = T288;
                    SetDataCellStyle(worksheet.Cell(iRow, iCol), worksheet.Cell(iRow, 1));
                    //worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;

                    //next material => next column
                    iCol++;
                }
                
                worksheet.Columns(Col1, Col1 + materials.Count - 1).AdjustToContents();
                worksheet.Column(1).Style.Font.Bold = true;
                var rngTable = worksheet.Range(Row1, Col1, 17 , Col1 + materials.Count - 1);
                rngTable.Style.Border.TopBorder = XLBorderStyleValues.Thin;
                rngTable.Style.Border.LeftBorder = XLBorderStyleValues.Thin;

                var rngTableOut = worksheet.Range(Row1, Col1 - 1, 17, Col1 + materials.Count - 1);
                rngTableOut.Style.Border.OutsideBorder = XLBorderStyleValues.Thick;


                //save
                try
                {
                    workbook.Save();
                }
                catch (Exception e)
                {
                    MessageBox.Show(e.Message);
                }

            }

            private void SetDataCellStyle(IXLCell cell, IXLCell PatternCell)
            {
                cell.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                cell.Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
                cell.Style.Font = PatternCell.Style.Font;
                cell.Style.Font.Bold = false;
            }
            private void SetHeaderCellStyle(IXLCell cell, IXLCell PatternCell)
            {
                cell.Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                cell.Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
                cell.Style.Font = PatternCell.Style.Font;
                cell.Style.Fill.BackgroundColor = PatternCell.Style.Fill.BackgroundColor;
                cell.Style.Border = PatternCell.Style.Border;
            }
        }

        class cRange
        {
            public  double dMin, dMax;
            public cRange()
            {
                dMin = Double.MinValue;
                dMax = Double.MaxValue;
            }
            public cRange(double a, double b)
            {
                dMin = a;
                dMax = b;
            }
        }

        static List<string> xyProps = new List<string> { ZStringConstants.DMLParameterIDDk, ZStringConstants.DMLParameterIDDf, 
                                                         ZStringConstants.DMLParameterIDTg, ZStringConstants.DMLParameterIDTd,
                                                         ZStringConstants.DMLParameterIDXYCTE, ZStringConstants.DMLParameterIDZCTE
        };

        //--Dictionary<string, Color> colorList;

        List<ZMaterial> initialMaterialList, materialList;
        string xId, yId, xCaption, yCaption;
        double dFrequency, dResin, dResinTolerance;
        string filterName;
        string filterExpression;
        List<string> invalid_axis;

        private bool bFreezeCursor;

        //ManufacturerColor state
        Dictionary<string, Color> colorMap = null;
        Dictionary<string, bool> checkMap = null;

        private PrintDocument printDocument1 = new PrintDocument();
        internal MaterialMapper(List<ZMaterial> input, string comment = "", string filterName = "Untitled", string filterExpression = "", double frequency = 1, double resin = 50 )//F[GHz]
        {
            colorMap = new Dictionary<string, Color>();
            checkMap = new Dictionary<string, bool>();

            this.filterName = (string.IsNullOrWhiteSpace(filterName)) ? "Untitled" : filterName;
            this.filterExpression = filterExpression;
            InitializeComponent();
            lSandbox.Text = "Z-zero  " + AboutData.TheAbout.sVersion;
            invalid_axis = new List<string>();
            initialMaterialList = new List<ZMaterial>(input);
            
            Load += Chart_Load;
            Load += ChartRadar_Load;
            Load += ChartFrequency_Load;
            
            chartMapper.GetToolTipText += chartMapper_GetToolTipText;
            chartRadar.GetToolTipText += chartRadar_GetToolTipText;             

            printDocument1.PrintPage += new PrintPageEventHandler(printDocument1_PrintPage);

            tbComment.Text = comment;
            dFrequency = frequency;
            dResin = resin;
            dResinTolerance = 0.5;
            bFreezeCursor = false;

            InitMaterialList();
            initMaps();

            foreach (string s in xyProps)
            {
                cbX.Items.Add(s);
                cbY.Items.Add(s);
                lbRadarAxis.Items.Add(s);
            }
            for (int j = 0; j < lbRadarAxis.Items.Count; j++)
            {
                lbRadarAxis.SetItemCheckState(j, CheckState.Checked);
            }
            
            cbX.SelectedIndex = 1;
            cbY.SelectedIndex = 0;

            //initial state
            xId = "";
            yId = "";

            //--colorList = new Dictionary<string, Color>();
            //--chartRadar.Palette = ChartColorPalette.SemiTransparent;//Pastel;

            tbFrequency.Text = dFrequency.ToString();
            tbResin.Text = dResin.ToString();
        }

        private void SetFrequency(string val)
        {
            double f;
            if (Double.TryParse(val, out f))
            {
                if (f != dFrequency)
                {
                    dFrequency = f;
                    RecalcChart();
                    RecalcChartSpider();

                    string item = currentSelection.Text;//--lbMaterials.SelectedItem as string;
                    tbMaterial.Text = SelectedDescription(item);
                }
            }
        }

        private void chartMapper_GetToolTipText(object sender, ToolTipEventArgs e)
        {
            // Check selected chart element and set tooltip text for it
            switch (e.HitTestResult.ChartElementType)
            {
                case ChartElementType.DataPoint:
                    var dataPoint = e.HitTestResult.Series.Points[e.HitTestResult.PointIndex];
                    e.Text = string.Format("{0}\n({1}, {2})", dataPoint.GetCustomProperty("material"), dataPoint.XValue.ToString("N3"), dataPoint.YValues[0].ToString("N3"));
                    break;
            }
        }

        private void chartRadar_GetToolTipText(object sender, ToolTipEventArgs e)
        {
            switch (e.HitTestResult.ChartElementType)
            {
                case ChartElementType.DataPoint:
                    var dataPoint = e.HitTestResult.Series.Points[e.HitTestResult.PointIndex];
                    string seriesLabel = e.HitTestResult.Series.GetCustomProperty("material");
                    e.Text = seriesLabel;
                    break;
            }
        }

        private void CaptureControl(Control cont)
        {
            memoryImage = new Bitmap(cont.Width, cont.Height);
            cont.DrawToBitmap(memoryImage, new Rectangle(0, 0, memoryImage.Width, memoryImage.Height));
            Clipboard.SetImage(memoryImage);
        }

        private void AddTextToImage(Image img, RectangleF rectf, Font font, string text, StringAlignment alignment)
        {
            using (Graphics g = Graphics.FromImage(img))
            {
                //g.DrawRectangle(new Pen(Color.Red, 2), 655, 460, 535, 90); 
                g.SmoothingMode = SmoothingMode.AntiAlias;
                g.InterpolationMode = InterpolationMode.HighQualityBicubic;
                g.PixelOffsetMode = PixelOffsetMode.HighQuality;
                StringFormat sf = new StringFormat();
                sf.Alignment = alignment;
                sf.LineAlignment = StringAlignment.Center;
                g.DrawString(text, font, Brushes.Black, rectf, sf);
            }
        }

        private void btnExport_Click(object sender, EventArgs e)
        {

            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Excel files (*.xlsm)|*.xlsm";
            saveFileDialog.Title = "Export to Excel File";
            saveFileDialog.FileName = string.Format("Material Mapper ({0})", this.filterName);

            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
            {
                Size size;
                string path = Path.GetDirectoryName(saveFileDialog.FileName) + Path.DirectorySeparatorChar;
                //copy template
                string mmFile = Path.GetDirectoryName(ZSettings.DMLFile) + Path.DirectorySeparatorChar + "MM.xlsm";
                try
                {
                    File.Copy(mmFile, saveFileDialog.FileName, true);
                }
                catch (Exception err)
                {
                    MessageBox.Show(err.Message);
                    return;
                }

                MM_ExcelExporter.ExportOptions opt = new MM_ExcelExporter.ExportOptions();
                MM_ExcelExporter exporter = new MM_ExcelExporter(opt, saveFileDialog.FileName);

                Image img;
                string textExported = "Exported from Z-zero Z-planner. (Z-zero library values provided by laminate vendors.)";
                string textFrequency = string.Format("Frequency: {0}GHz", dFrequency);
                string textResin = string.Format("Resin: {0}%", dResin);
                Font font = new System.Drawing.Font("Tahoma", 8, FontStyle.Regular);

                CaptureControl(panelMaterialTable);
                img = Clipboard.GetImage();
                img.Save(path + "list.bmp", ImageFormat.Bmp);

                size = chartMapper.Size;
                chartMapper.Size = new Size(501, 535);
                CaptureControl(chartMapper);
                img = Clipboard.GetImage();
                AddTextToImage(img, new RectangleF(16, 6, img.Width - 32, 12), font, textExported, StringAlignment.Far);
                AddTextToImage(img, new RectangleF(16, img.Height - 16, 100, 12), font, textFrequency, StringAlignment.Near);
                AddTextToImage(img, new RectangleF(16, img.Height - 32, 100, 12), font, textResin, StringAlignment.Near);
                img.Save(path + "xy.bmp", ImageFormat.Bmp);
                chartMapper.Size = size;

                size = chartRadar.Size;
                chartRadar.Size = new Size(501, 535);
                CaptureControl(chartRadar);
                img = Clipboard.GetImage();
                AddTextToImage(img, new RectangleF(16, 6, img.Width - 32, 12), font, textExported, StringAlignment.Far);
                AddTextToImage(img, new RectangleF(16, img.Height - 16, 100, 12), font, textFrequency, StringAlignment.Near);
                AddTextToImage(img, new RectangleF(16, img.Height - 32, 100, 12), font, textResin, StringAlignment.Near);
                img.Save(path + "radial.bmp", ImageFormat.Bmp);
                chartRadar.Size = size;

                size = chartFrequency.Size;
                chartFrequency.Size = new Size(697, 506);
                bool state = rbDk.Checked;
                rbDk.Checked = true;
                CaptureControl(chartFrequency);
                img = Clipboard.GetImage();
                AddTextToImage(img, new RectangleF(16, 6, img.Width - 50, 12), font, textExported, StringAlignment.Far);
                AddTextToImage(img, new RectangleF(16, img.Height - 16, 100, 12), font, textResin, StringAlignment.Near);
                img.Save(path + "Dk.bmp", ImageFormat.Bmp);

                rbDk.Checked = false;
                CaptureControl(chartFrequency);
                img = Clipboard.GetImage();
                AddTextToImage(img, new RectangleF(16, 6, img.Width - 50, 12), font, textExported, StringAlignment.Far);
                AddTextToImage(img, new RectangleF(16, img.Height - 16, 100, 12), font, textResin, StringAlignment.Near);
                img.Save(path + "Df.bmp", ImageFormat.Bmp);
                chartFrequency.Size = size;

                rbDk.Checked = state;

                exporter.Export(wrapperList);


                //try to open excel app
                try
                {
                    string arg = String.Format("\"{0}\"", saveFileDialog.FileName);
                    ProcessStartInfo startInfo = new ProcessStartInfo();
                    startInfo.FileName = "EXCEL.EXE";
                    startInfo.Arguments = arg;
                    Process.Start(startInfo);
                }
                catch { }

            }
        }

        private void btnPrint_Click(object sender, EventArgs e)
        {
            CaptureScreen();

            PrintDialog dlg = new PrintDialog();
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
        }

        private void ChartRadar_Load(object sender, EventArgs e)
        {
            RecalcChartSpider();
        }

        private void ChartFrequency_Load(object sender, EventArgs e)
        {
            RecalcChartFrequency();
        }

        class cSeriesData
        {
            public string name;
            public string label;
            public List<string> xValues;
            public List<double> yValues;
            public List<string> labels;
            public Color color;

            public cSeriesData(string aname)
            {
                name = aname;
                xValues = new List<string>();
                yValues = new List<double>();
                labels = new List<string>();
                color = Color.Black;
            }

            public void SetColor(Color aC)
            {
                color = aC;
            }

            public void SetLabel(string alabel)
            {
                label = alabel;
            }
            
            public void SetXVal(List<string> data)
            {
                xValues.Clear();
                foreach (string s in data) xValues.Add(s);
            }

            public void SetYVal(List <double> data)
            {
                yValues.Clear();
                foreach (double d in data) yValues.Add(d);
            }

            public void SetPointLabels(List<string> data)
            {
                labels.Clear();
                foreach (string s in data) labels.Add(s);
            }

            public void Add(Chart chart, bool bLine = true, bool bBold = false)
            {
                chart.Series.Add(name);

                chart.Series[name].Points.DataBindXY(xValues, yValues);
                for (int i = 0; i < chart.Series[name].Points.Count; i++ )
                {
                    DataPoint p = chart.Series[name].Points[i];
                    p.Label = labels[i];
                    if (bBold)
                    {
                        p.Font = new System.Drawing.Font(p.Font.FontFamily, p.Font.Size, FontStyle.Bold);
                    }
                }

                chart.Series[name].Color = color;
                chart.Series[name].SetCustomProperty("material", label);

                chart.Series[name].ChartType = SeriesChartType.Radar;

                chart.Series[name].Enabled = true;

                // Set radar chart style (Area, Line or Marker)
                if (bLine)
                {
                    chart.Series[name]["RadarDrawingStyle"] = "Line";
                    chart.Series[name].BorderWidth = 4;
                }
                else
                {
                    chart.Series[name]["RadarDrawingStyle"] = "Area";
                }

                // Set circular area drawing style (Circle or Polygon)
                chart.Series[name]["AreaDrawingStyle"] = "Polygon";
                // Set labels style (Auto, Horizontal, Circular or Radial)
                chart.Series[name]["CircularLabelsStyle"] = "Horizontal";
                //chartRadar.Series[name].IsVisibleInLegend = true;

                //chartRadar.Series[name].Palette = ChartColorPalette.SemiTransparent;

                chart.Series[name].MarkerSize = chart.Series[name].BorderWidth + 5;
                chart.Series[name].MarkerStyle = MarkerStyle.Diamond;

                chart.Series[name].SmartLabelStyle.Enabled = true;
                chart.Series[name].SmartLabelStyle.IsOverlappedHidden = true;
                chart.Series[name].SmartLabelStyle.AllowOutsidePlotArea = LabelOutsidePlotAreaStyle.Partial;

            }
        }

        private bool IsMaterialValid(string name)
        {
            cMaterialWrapper w = FindWrapper(name);
            return (w != null) && w.enabled;//--lbMaterials.CheckedItems.Contains(name);
        }

        private bool IsAxisValid(string name)
        {
            return lbRadarAxis.CheckedItems.Contains(name);
        }

        private bool GetValue(string material, string param, string value, out double x)
        {
            bool bValid = false;
            x = 0;
            switch (param)
            {
                case ZStringConstants.DMLParameterIDDk:
                    x = DkFamily[material].Interpolate(dFrequency, dResin);
                    bValid = !Double.IsNaN(x);
                    break;
                case ZStringConstants.DMLParameterIDDf:
                    //--bValid = ZPlannerManager.TryParseDependedValue(value, dFrequency, out x);
                    x = DfFamily[material].Interpolate(dFrequency, dResin);
                    bValid = !Double.IsNaN(x);;
                    break;
                default:
                    bValid = Double.TryParse(value, out x);
                    break;
            }
            return bValid;
        }

        private bool TheBiggerTheBetter(string param)
        {
            switch (param)
            {
                case ZStringConstants.DMLParameterIDTg:
                case ZStringConstants.DMLParameterIDTd:
                    return true;
            }
            return false;
        }

        private void RecalcChartSpider(string topSeries = "")
        {
            invalid_axis.Clear();
            chartRadar.Series.Clear();
            Dictionary<string, cRange> rangeMap = new Dictionary<string, cRange>();

            foreach (string par in xyProps)
            {
                if (!IsAxisValid(par)) continue;
                if (invalid_axis.Contains(par)) continue;

                //normalize
                double ymin = Double.MaxValue, ymax = Double.MinValue;
                //--foreach (ZMaterial zM in materialList)
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    if (!mW.enabled)  continue;
                    ZMaterial zM = mW.material;
                    string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                    //--if (!IsMaterialValid(name))
                    //--    continue;

                    ZMaterialParameter p= zM.MaterialParameters.GetByID(par);
                    if (p == null || p.Value == null) continue;

                    double y;

                    string yVal = TrimPercent(p.Value);

                    bool yValid = GetValue(name, par, yVal, out y);
                    if (yValid)
                    {
                        if (y < ymin) ymin = y;
                        if (y > ymax) ymax = y;
                    }
                    else
                    {
                        invalid_axis.Add(par);
                        //--mW.enabled = false;//--lbMaterials.SetItemCheckState(lbMaterials.Items.IndexOf(name), CheckState.Unchecked);
                    }
                }

                rangeMap[par] = new cRange(ymin, ymax);
            }


            List<string> xValues = new List<string>();

            for (int i = 0; i < xyProps.Count; i++)
            {
                if (!IsAxisValid(xyProps[i])) continue;
                if (invalid_axis.Contains(xyProps[i])) continue;

                xValues.Add(xyProps[i]);
            }

            bool bTop = topSeries.Length > 0;
            cSeriesData topData =null;
            //--Color topColor = Color.Blue; ;
            //--foreach (ZMaterial zM in materialList)
            foreach (cMaterialWrapper mW in wrapperList)
            {
                if (!mW.enabled) continue;

                ZMaterial zM = mW.material;
                string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                //--if (!IsMaterialValid(name)) continue;

                string label = "";
                List<double> yValues = new List<double>();
                List<string> pointLabels = new List<string>();
                for (int i = 0; i < xyProps.Count; i++)
                {
                    string par = xyProps[i];
                    if (!IsAxisValid(par)) continue;
                    if (invalid_axis.Contains(par)) continue;

                    ZMaterialParameter p = zM.MaterialParameters.GetByID(par);
                    if (p == null || p.Value == null) continue;
 
                    double y, yNorm;
                    string yVal = TrimPercent(p.Value);
                    bool yValid = GetValue(name, par, yVal, out y);
                    bool bRegular = TheBiggerTheBetter(par);
                    if (yValid)
                    {
                        label += par + " = " + y.ToString("N3") + "\n";
                        cRange r = rangeMap[par];
                        try
                        {
                            if (rbOriginal.Checked)
                            {
                                yNorm = (y - r.dMin) * 100 / (r.dMax - r.dMin);
                            }
                            else //best to perimeter
                            {
                                if (bRegular)
                                {
                                    yNorm = y * 100 / r.dMax;
                                }
                                else //inverse
                                {
                                    yNorm = r.dMin * 100 / y;
                                }
                            }
                        }
                        catch{
                            yNorm = 100;
                        }
                        yValues.Add(yNorm);
                        pointLabels.Add(y.ToString("N3"));
                    }
                }

                label = name + "\n" + label;

                //skip material with invalid parameters
                if (xValues.Count != yValues.Count) continue;

                cSeriesData data = new cSeriesData(name);
                data.SetLabel(label);
                data.SetXVal(xValues);
                data.SetYVal(yValues);
                data.SetPointLabels(pointLabels);
                data.SetColor(mW.color);

                if (bTop && name == topSeries)
                {
                    topData = data;
                    //--topColor = mW.color;
                }
                else
                {
                    data.Add(chartRadar, rbLine.Checked);
                    //if (bTop)
                    //{
                    //    chartRadar.Series[data.name].Color = mW.color;//--colorList[data.name];
                    //}
                }
            }


            if (bTop && topData != null)
            {
                topData.Add(chartRadar, rbLine.Checked, true);
                //--chartRadar.Series[topData.name].Color = topColor;//--colorList[topData.name];
            }

            //uncheck invalid axis
            bSkipRecalc = true;
            foreach (string a in invalid_axis)
            {
                lbRadarAxis.SetItemCheckState(lbRadarAxis.Items.IndexOf(a), CheckState.Unchecked);
            }
            bSkipRecalc = false;

            /*
            else
            {
                chartRadar.ApplyPaletteColors();
                foreach (Series ser in chartRadar.Series)
                {
                    colorList[ser.Name] = ser.Color;
                }
            }
            */            
            chartRadar.ChartAreas[0].AxisX.IsMarginVisible = false;
            chartRadar.ChartAreas[0].AxisY.IsMarginVisible = false;

//            chartRadar.ChartAreas["ChartAreaRadar"].Area3DStyle.Enable3D = true;
//            chartRadar.ChartAreas[0].RecalculateAxesScale();
            //chartRadar.Refresh();
        }

        private string TrimPercent(string inp)
        {
            if (inp == null) return "";
            char[] charsToTrim = { '%', ' ' };
            return inp.Trim(charsToTrim);
        }

        private string GetAxisFormat(string id)
        {
            switch (id)
            {
                case ZStringConstants.DMLParameterIDDk: return "0.00";
                case ZStringConstants.DMLParameterIDDf: return "0.0000";
                case ZStringConstants.DMLParameterIDTg: return "0";
                case ZStringConstants.DMLParameterIDTd: return "0";
                case ZStringConstants.DMLParameterIDXYCTE: return "0";
                case ZStringConstants.DMLParameterIDZCTE: return "0.0";
            }
            return "0.0";
        }

        private void RecalcChart()
        {
            xId = cbX.SelectedItem as string;;
            yId = cbY.SelectedItem as string;
            if (xId == null || yId == null) return;

            ZMaterial theFirstMaterial = wrapperList[0].material;//--materialList[0];
            xCaption = theFirstMaterial.MaterialParameters.GetByID(xId).Parameter.Title;
            yCaption = theFirstMaterial.MaterialParameters.GetByID(yId).Parameter.Title;
            
            chartMapper.ChartAreas["chartAreaMapper"].AxisX.Title = xCaption == null ? "X" : xCaption;
            chartMapper.ChartAreas["chartAreaMapper"].AxisX.TitleAlignment = StringAlignment.Far;
            chartMapper.ChartAreas[0].AxisX.LabelStyle.Format = GetAxisFormat(xId);
            chartMapper.ChartAreas[0].AxisX.IsStartedFromZero = false;

            chartMapper.ChartAreas["chartAreaMapper"].AxisY.Title = yCaption == null ? "Y" : yCaption;
            chartMapper.ChartAreas["chartAreaMapper"].AxisY.TextOrientation = TextOrientation.Rotated270;
            chartMapper.ChartAreas["chartAreaMapper"].AxisY.TitleAlignment = StringAlignment.Far;
            chartMapper.ChartAreas[0].AxisY.LabelStyle.Format = GetAxisFormat(yId);
            chartMapper.ChartAreas[0].AxisY.IsStartedFromZero = false;

            chartMapper.Series.Clear();

            chartMapper.Series.Add("XY");
            chartMapper.Series["XY"].Enabled = true;
            chartMapper.Series["XY"].ChartType = SeriesChartType.Point;
            chartMapper.Series["XY"].BorderWidth = 2;
            //--chartMapper.Series["XY"].Color =  pictColorDots.BackColor;
        
            //get points
            //--foreach (ZMaterial zM in materialList)
            foreach (cMaterialWrapper mW in wrapperList)
            {
                ZMaterial zM = mW.material;
                if (!mW.enabled) continue;

                string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                double x, y;
                string xVal = TrimPercent(zM.MaterialParameters.GetByID(xId).Value);//remove % sign and trim
                string yVal = TrimPercent(zM.MaterialParameters.GetByID(yId).Value);
                bool xValid = GetValue(name, xId, xVal, out x);
                bool yValid = GetValue(name, yId, yVal, out y);
                string label = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                if (xValid && yValid)
                {
                    DataPoint pt = new DataPoint(x,y);
                    pt.SetCustomProperty("material", label);
                    pt.Label = label;
                    pt.Color = mW.color;
                    chartMapper.Series["XY"].Points.Add(pt);                    
                }
            }
            chartMapper.Series["XY"].SmartLabelStyle.Enabled = true;
            chartMapper.Series["XY"].SmartLabelStyle.IsOverlappedHidden = true;

            chartMapper.ChartAreas[0].AxisX.IntervalAutoMode = IntervalAutoMode.VariableCount;
            //chartMapper.Series["XY"].YAxisType = AxisType.Secondary;
            //chartMapper.ChartAreas[0].AxisY2.IsStartedFromZero = false;

            chartMapper.ChartAreas["chartAreaMapper"].RecalculateAxesScale();
        }

        private string getPointLabel(DataPoint pt, bool bSelected = false)
        {
            if (cbLabels.Checked || bSelected)
            {
                return String.Format("{0}, {1}", pt.XValue.ToString(), pt.YValues[0].ToString("N3"));
            }
            else return "";
        }
        private void RecalcChartFrequency()
        {
            string caption = rbDk.Checked ? "Dk" : "Df";
            string id = rbDk.Checked ? ZStringConstants.DMLParameterIDDk : ZStringConstants.DMLParameterIDDf;
            bool bDk = rbDk.Checked;

            //get range
            double rangeMin = double.MaxValue;
            double rangeMax = double.MinValue;
            foreach (cMaterialWrapper mW in wrapperList)
            {
                ZMaterial zM = mW.material;
                if (!mW.enabled) continue;

                string materialName = mW.materialName;
                //--string yVal = zM.MaterialParameters.GetByID(id).Value;
                //--List<ZPlannerManager.xy> points = ZPlannerManager.TableFunction(yVal);
                //--points.Sort((a, b) => a.x.CompareTo(b.x));
                List<PWL2.P> points = null;
                if (bDk){
                    points = DkFamily[materialName].Interpolate(dResin);
                }
                else
                {
                    points = DfFamily[materialName].Interpolate(dResin);
                }

                double min = points[0].x;
                if (min < rangeMin) rangeMin = min;
                double max = points[points.Count - 1].x;
                if (max > rangeMax) rangeMax = max;
            }
            if (rangeMin == double.MaxValue) rangeMin = 1;
            if (rangeMax == double.MinValue) rangeMax = 10;

            rangeMin = Math.Truncate(rangeMin);
            rangeMax = Math.Ceiling(rangeMax);


            chartFrequency.ChartAreas[0].AxisX.Title = "Frequency [GHz]";
            chartFrequency.ChartAreas[0].AxisX.TitleAlignment = StringAlignment.Far;
            chartFrequency.ChartAreas[0].AxisX.LabelStyle.Format = "0.0";
            chartFrequency.ChartAreas[0].AxisX.IsStartedFromZero = true;
            chartFrequency.ChartAreas[0].AxisX.IsMarginVisible = false;

            chartFrequency.ChartAreas[0].AxisY.Title = caption;
            chartFrequency.ChartAreas[0].AxisY.TextOrientation = TextOrientation.Rotated270;
            chartFrequency.ChartAreas[0].AxisY.TitleAlignment = StringAlignment.Far;
            chartFrequency.ChartAreas[0].AxisY.LabelStyle.Format = GetAxisFormat(id);
            chartFrequency.ChartAreas[0].AxisY.IsStartedFromZero = false;
            chartFrequency.ChartAreas[0].AxisX.Minimum = 0;

            //chartFrequency.ChartAreas[0].AxisX.MajorGrid.Interval = (int)((rangeMax - rangeMin) / 10 + 0.5); //closest whole number - ???????
            chartFrequency.ChartAreas[0].AxisX.MajorGrid.Interval = 1;
            chartFrequency.ChartAreas[0].AxisX.MinorGrid.Enabled = true;
            chartFrequency.ChartAreas[0].AxisX.MinorGrid.Interval = chartFrequency.ChartAreas[0].AxisX.MajorGrid.Interval / 2;
            chartFrequency.ChartAreas[0].AxisX.MinorGrid.LineDashStyle = ChartDashStyle.Dot;

            //chartFrequency.ChartAreas[0].AxisY.MajorGrid.Interval = (chartFrequency.ChartAreas[0].AxisY.Maximum - chartFrequency.ChartAreas[0].AxisY.Minimum) / 10;
            chartFrequency.ChartAreas[0].AxisY.MinorGrid.Enabled = true;
            chartFrequency.ChartAreas[0].AxisY.MinorGrid.Interval = chartFrequency.ChartAreas[0].AxisY.MajorGrid.Interval / 2;
            chartFrequency.ChartAreas[0].AxisY.MinorGrid.LineDashStyle = ChartDashStyle.Dot;
            //chartFrequency.Visible = true;

            //snap cursor
            chartFrequency.ChartAreas[0].CursorX.IsUserEnabled = true;
            chartFrequency.ChartAreas[0].CursorX.Interval = 0;
            chartFrequency.ChartAreas[0].CursorX.LineColor = Color.DarkBlue;
            chartFrequency.ChartAreas[0].CursorX.LineWidth = 2;
            chartFrequency.ChartAreas[0].CursorX.LineDashStyle = ChartDashStyle.Dash;
            chartFrequency.ChartAreas[0].CursorX.Interval = 0;

            chartFrequency.ChartAreas[0].CursorY.LineColor = Color.Black;
            chartFrequency.ChartAreas[0].CursorY.LineWidth = 0;
            chartFrequency.ChartAreas[0].CursorY.IsUserEnabled = false;    

            chartFrequency.Series.Clear();


            //get points
            foreach (cMaterialWrapper mW in wrapperList)
            {
               ZMaterial zM = mW.material;
               if (!mW.enabled) continue;

               string materialName = mW.materialName;
               //--string yVal = zM.MaterialParameters.GetByID(id).Value;
               //--List<ZPlannerManager.xy> points = ZPlannerManager.TableFunction(yVal);
               //--points.Sort((a, b) => a.x.CompareTo(b.x));

               List<PWL2.P> points = null;
               if (bDk)
               {
                   points = DkFamily[materialName].Interpolate(dResin);
               }
               else
               {
                   points = DfFamily[materialName].Interpolate(dResin);
               }


               bool bSelected = materialName == currentSelection.Text;
               chartFrequency.Series.Add(materialName);
               chartFrequency.Series[materialName].Enabled = true;
               chartFrequency.Series[materialName].ChartType = SeriesChartType.Line;
               chartFrequency.Series[materialName].BorderWidth = bSelected ? 4 : 2;
               chartFrequency.Series[materialName].ToolTip = materialName;

               chartFrequency.Series[materialName].SmartLabelStyle.Enabled = true;
               chartFrequency.Series[materialName].SmartLabelStyle.IsMarkerOverlappingAllowed = false;
               chartFrequency.Series[materialName].SmartLabelStyle.IsOverlappedHidden = true;
               chartFrequency.Series[materialName].SmartLabelStyle.MovingDirection = LabelAlignmentStyles.Top;


               string label = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
               bool bSingleValue = points.Count == 1;
               if (bSingleValue)
               {
                   //--ZPlannerManager.xy 
                   PWL2.P firstPoint = points[0];
                   DataPoint pt = new DataPoint(rangeMin, firstPoint.y);
                   pt.SetCustomProperty("material", label);
                   //pt.Label = label;
                   pt.Label = getPointLabel(pt, bSelected);//--String.Format("{0}, {1}",pt.XValue.ToString(),pt.YValues[0].ToString("N3"));
                   pt.Color = mW.color;
                   chartFrequency.Series[materialName].Points.Add(pt);

                   DataPoint pt1 = new DataPoint(rangeMax, firstPoint.y);
                   pt1.SetCustomProperty("material", label);
                   //pt.Label = label;
                   pt.Label = getPointLabel(pt, bSelected);//-- String.Format("{0}, {1}", pt.XValue.ToString(), pt.YValues[0].ToString("N3"));
                   pt1.Color = mW.color;
                   chartFrequency.Series[materialName].Points.Add(pt1);

               }
               else
               {
                   //--ZPlannerManager.xy 
                   //PWL2.P firstPoint = points[0];
                   //if (firstPoint.x > rangeMin)
                   //{
                   //    DataPoint pt = new DataPoint(rangeMin, firstPoint.y);
                   //    pt.SetCustomProperty("material", label);
                   //    //pt.Label = label;
                   //    pt.Label = getPointLabel(pt, bSelected);//--String.Format("{0}, {1}", pt.XValue.ToString(), pt.YValues[0].ToString("N3"));
                   //    pt.Color = mW.color;
                   //    chartFrequency.Series[materialName].Points.Add(pt);
                   //}
                   foreach (/*ZPlannerManager.xy*/PWL2.P point in points)
                   {
                       DataPoint pt = new DataPoint(point.x, point.y);
                       pt.SetCustomProperty("material", label);
                       //pt.Label = label;
                       pt.Label = getPointLabel(pt, bSelected);//--String.Format("{0}, {1}", pt.XValue.ToString(), pt.YValues[0].ToString("N3"));
                       pt.Color = mW.color;
                       //--pt.ToolTip = String.Format("{0}:{1}",point.x, point.y);
                       chartFrequency.Series[materialName].Points.Add(pt);
                   }
                   //--ZPlannerManager.xy 
                   //PWL2.P lastPoint = points[points.Count - 1];
                   //if (lastPoint.x < rangeMax)
                   //{
                   //    DataPoint pt = new DataPoint(rangeMax, lastPoint.y);
                   //    pt.SetCustomProperty("material", label);
                   //    //pt.Label = label;
                   //    pt.Label = getPointLabel(pt, bSelected);//--String.Format("{0}, {1}", pt.XValue.ToString(), pt.YValues[0].ToString("N3"));
                   //    pt.Color = mW.color;
                   //    chartFrequency.Series[materialName].Points.Add(pt);
                   //}
               }
               chartFrequency.Series[materialName].MarkerStyle = MarkerStyle.Diamond;
               chartFrequency.Series[materialName].MarkerSize = 8;
               chartFrequency.Series[materialName].SmartLabelStyle.Enabled = true;
               chartFrequency.Series[materialName].SmartLabelStyle.IsOverlappedHidden = true;
            }

            chartFrequency.ChartAreas[0].RecalculateAxesScale();
        }

        private void btnClose_Click(object sender, EventArgs e)
        {
            Close();
        }

        private void cbY_SelectedIndexChanged(object sender, EventArgs e)
        {
            RecalcChart();
        }

        private void cbX_SelectedIndexChanged(object sender, EventArgs e)
        {
            RecalcChart();
        }

        private string SelectedDescription(string item)
        {
            ZMaterial zM = materialList.Find(x => x.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value == item);
            if (zM == null) return "";


            string label = "";
            double[] yValues = new double[xyProps.Count];
            for (int i = 0; i < xyProps.Count; i++)
            {
                string par = xyProps[i];
                double y;
                string yVal = TrimPercent(zM.MaterialParameters.GetByID(par).Value);
                bool yValid = GetValue(item, par, yVal, out y);
                if (yValid)
                {
                    label += par + " = " + y.ToString("N3") + Environment.NewLine;
                }
            }
            //--label += "Resin = " + zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value + Environment.NewLine;

            //!DEBUG
            //--label += "Dk(f) = " + zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk).Value + Environment.NewLine;
            //--label += "Df(f) = " + zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf).Value + Environment.NewLine;
            //!

            label = item + Environment.NewLine + Environment.NewLine +label;

            return label;
        }

        private void lbRadarAxis_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!bSkipRecalc)
            {
                RecalcChartSpider();
            }
        }

        private void rbLine_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartSpider();
        }

        private void rbArea_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartSpider();
        }

        private void tbFrequency_Leave(object sender, EventArgs e)
        {
            SetFrequency(tbFrequency.Text);
        }

        private void tbFrequency_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SetFrequency(tbFrequency.Text);
            }
        }

        private void btnCheck_Click(object sender, EventArgs e)
        {
            bSkipRecalc = true;
            foreach (Control c in tblMaterialPanel.Controls)
            {
                if (tblMaterialPanel.GetColumn(c) == 0)
                {
                    cMaterialCheckBox mcb = c as cMaterialCheckBox;
                    mcb.Checked = true;
                    mcb.mWrapper.enabled = true;
                }
            }
            bSkipRecalc = false;
            RecalcChart();
            RecalcChartSpider();
            RecalcChartFrequency();
        }

        private void btnUncheck_Click(object sender, EventArgs e)
        {
            bSkipRecalc = true;
            foreach (Control c in tblMaterialPanel.Controls)
            {
                if (tblMaterialPanel.GetColumn(c) == 0)
                {
                    cMaterialCheckBox mcb = c as cMaterialCheckBox;
                    mcb.Checked = false;
                    mcb.mWrapper.enabled = false;
                }
            }
            bSkipRecalc = false;
            RecalcChart();
            RecalcChartSpider();
            RecalcChartFrequency();
        }

//-------------------------------------------------------------------------------------------------
        class cMaterialWrapper
        {
            public ZMaterial material;
            public Color color, indColor;
            public bool enabled, indEnabled;
            public string materialName;
            public string value;
        }

        List<cMaterialWrapper> wrapperList;

        cMaterialWrapper FindWrapper(string name)
        {
            cMaterialWrapper w = wrapperList.Find(x => x.materialName == name);
            return w;
        }

        class cMaterialCheckBox :  CheckBox
        {
            public cMaterialWrapper mWrapper;
        }

        class cMaterialPictureBox : PictureBox
        {
            public cMaterialWrapper mWrapper;
        }

        class cMaterialLabel : Label
        {
            public cMaterialWrapper mWrapper;
        }


        double GetResinValue(string s)
        {
            string v = TrimPercent(s);
            double d;
            if (Double.TryParse(v, out d))
            {
                return d;
            }
            return 0;
        }

        bool ResinInRange(string sValue, double rMin, double rMax)
        {
            double dval = GetResinValue(sValue);
            bool retval = dval >= rMin && dval <= rMax;
            return retval;
        }

        List <ZMaterial> GetMaterialsByResinRange(List<ZMaterial> input, double rMin, double rMax)
        {
            var query = from material in input
                     where (ResinInRange(material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value, rMin, rMax))
                     select material;
            List<ZMaterial> mData = new List<ZMaterial>();
            mData = query.ToList();

            List<ZMaterial> groupedMaterials;
            groupedMaterials = mData.GroupBy(m => m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value)
                   .Select(grp => grp.First())
                   .ToList();

            return groupedMaterials;
        }

        List<ZMaterial> GetAllMaterials(List<ZMaterial> input)
        {
            List<ZMaterial> groupedMaterials;
            groupedMaterials = input.GroupBy(m => m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value)
                   .Select(grp => grp.First())
                   .ToList();

            return groupedMaterials;
        }

        private Dictionary<string, PWL2> DkFamily = null; //family of pwl functions Dk(f) for different resin content
        private Dictionary<string, PWL2> DfFamily = null; //family of pwl functions Df(f) for different resin content

        List<ZMaterial> CreateFunctionFamilies()
        {
            List<ZMaterial> groupedMaterials;
            groupedMaterials = initialMaterialList.GroupBy(m => m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value)
                   .Select(grp => grp.First())
                   .ToList();

            DkFamily = new Dictionary<string, PWL2>();
            DfFamily = new Dictionary<string, PWL2>();

            foreach (ZMaterial zM in groupedMaterials){
                string key = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
                DkFamily[key] = new PWL2();
                DfFamily[key] = new PWL2();
            }

            foreach (ZMaterial zM in initialMaterialList)
            {
                double z = GetResinValue(zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value);
                string key = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;

                PWL2.Fz funcDk = DkFamily[key].NewFz(z); //Dk(f) for resin = z
                PWL2.Fz funcDf = DfFamily[key].NewFz(z); //Dk(f) for resin = z

                string DkVal = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk).Value;
                List<ZPlannerManager.xy> pointsDk = ZPlannerManager.TableFunction(DkVal);
                foreach (ZPlannerManager.xy xy in pointsDk)
                {
                    funcDk.Add(xy.x, xy.y);
                }

                string DfVal = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDf).Value;
                List<ZPlannerManager.xy> pointsDf = ZPlannerManager.TableFunction(DfVal);
                foreach (ZPlannerManager.xy xy in pointsDf)
                {
                    funcDf.Add(xy.x, xy.y);
                }
            }

            return groupedMaterials;
        }

        cMaterialLabel currentSelection = null;
        private Dictionary<cMaterialWrapper, cMaterialPictureBox> mapMaterial_ColorBox;
        private Dictionary<cMaterialWrapper, cMaterialCheckBox> mapMaterial_CheckBox;
        private void InitMaterialList()
        {
            //if (dResin > 0)
            //{
            //    materialList = GetMaterialsByResinRange(initialMaterialList, dResin - dResinTolerance, dResin + dResinTolerance);
            //}
            //else
            //{
            //    materialList = GetAllMaterials(initialMaterialList); 
            //}
            
            //--materialList = GetAllMaterials(initialMaterialList);
            mapMaterial_ColorBox = new Dictionary<cMaterialWrapper, cMaterialPictureBox>();
            mapMaterial_CheckBox = new Dictionary<cMaterialWrapper, cMaterialCheckBox>();
            
            materialList = CreateFunctionFamilies();

            currentSelection = null;
            wrapperList = new List<cMaterialWrapper>();

            tblMaterialPanel.Controls.Clear();
            tblMaterialPanel.RowStyles.Clear();
            tblMaterialPanel.RowCount = 1;
            int vertScrollWidth = SystemInformation.VerticalScrollBarWidth;
            tblMaterialPanel.Padding = new Padding(0, 0, vertScrollWidth, 0);

            //--List<Color> palette = new List<Color> { Color.Red, Color.Orange, Color.Green, Color.Blue, Color.Brown };
            List<Color> palette = new List<Color> { Color.Red, Color.Green, Color.Blue, Color.Orange, Color.LimeGreen, Color.Magenta, Color.RoyalBlue, Color.Brown , Color.BlueViolet};
            //Color.FromArgb(180, Color.Blue);

            bool bFirst = true;
            int idx = 0;
            foreach (ZMaterial zM in materialList)
            {
                Color c = palette[idx % palette.Count];
                idx++;

                string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;

                cMaterialWrapper w = new cMaterialWrapper();
                w.material = zM;
                w.color = c;
                w.enabled = true;
                w.materialName = name;

                w.indColor = c;
                w.indEnabled = true;

                wrapperList.Add(w);

                if (bFirst)
                {
                    bFirst = false;
                }
                else
                {
                    tblMaterialPanel.RowCount += 1;
                }
                tblMaterialPanel.RowStyles.Add(new RowStyle(SizeType.Absolute, 17F));

                var checkBox = new cMaterialCheckBox() { Checked = true, Text = "", Height = 15, Margin = new Padding(0, 0, 0, 0), mWrapper = w };
                mapMaterial_CheckBox.Add(w, checkBox);
                checkBox.CheckedChanged += new System.EventHandler(genericCheckBox_CheckedChanged);
                tblMaterialPanel.Controls.Add(checkBox, 0, tblMaterialPanel.RowCount - 1);

                bSkipRecalc = false;

                var pictureBox = new cMaterialPictureBox() { BackColor = c, Width = 15, Height = 15, Margin = new Padding(2, 0, 0, 0), BorderStyle = BorderStyle.FixedSingle, mWrapper = w };
                mapMaterial_ColorBox.Add(w, pictureBox);
                pictureBox.Click += new System.EventHandler(genericPictureBox_Click);
                tblMaterialPanel.Controls.Add(pictureBox, 1, tblMaterialPanel.RowCount - 1);
                int wLabel = (int)tblMaterialPanel.ColumnStyles[2].Width;

                var label = new cMaterialLabel() { Text = name, AutoSize = false, Width = wLabel/*180*/, Height = 15, BorderStyle = BorderStyle.None, BackColor = SystemColors.Window, Margin = new Padding(0, 0, 0, 0), mWrapper = w };
                label.Click += new System.EventHandler(genericLabel_Click);
                tblMaterialPanel.Controls.Add(label, 2, tblMaterialPanel.RowCount - 1);
                if (currentSelection == null)
                {
                    currentSelection = label;
                }

                var label1 = new cMaterialLabel() { AutoSize = false, Width = (int)tblMaterialPanel.ColumnStyles[3].Width, Height = 15, BorderStyle = BorderStyle.None, BackColor = SystemColors.Window, Margin = new Padding(0, 0, 0, 0), mWrapper = w };
                label1.Font = new Font(label1.Font, FontStyle.Bold);
                tblMaterialPanel.Controls.Add(label1, 3, tblMaterialPanel.RowCount - 1);
            }
            tblMaterialPanel.HorizontalScroll.Enabled = false;
            genericLabel_Click(currentSelection, new EventArgs());
        }

        private void genericLabel_Click(object sender, EventArgs e)
        {
            currentSelection.BackColor = SystemColors.Window;
            TableLayoutPanelCellPosition selpos = tblMaterialPanel.GetPositionFromControl(currentSelection);
            cMaterialLabel selValLabel = tblMaterialPanel.GetControlFromPosition(selpos.Column + 1, selpos.Row) as cMaterialLabel;
            selValLabel.BackColor = SystemColors.Window;

            cMaterialLabel lb = sender as cMaterialLabel;
            currentSelection = lb;
            lb.BackColor = ZColorConstants.MaterialMapperHighlightColor;
            TableLayoutPanelCellPosition tpos = tblMaterialPanel.GetPositionFromControl(lb);
            cMaterialLabel valLabel = tblMaterialPanel.GetControlFromPosition(tpos.Column + 1, tpos.Row) as cMaterialLabel;
            valLabel.BackColor = ZColorConstants.MaterialMapperHighlightColor;


            tbMaterial.Text = SelectedDescription(lb.Text);
            RecalcChart();
            RecalcChartSpider(lb.Text);//make selection drawn on top
            RecalcChartFrequency();

            if (chartMapper.Series.Count > 0)
            {
                int idx = 0;
                foreach (DataPoint pt in chartMapper.Series["XY"].Points)
                {
                    string s = pt.GetCustomProperty("material");
                    if (s == lb.Text)
                    {
                        break;
                    }
                    idx++;
                }
                if (idx < chartMapper.Series["XY"].Points.Count)
                {
                    chartMapper.Series["XY"].Points[idx].MarkerSize += 3;
                }
            }
        }

        bool bSkipRecalc;
        private void genericCheckBox_CheckedChanged(object sender, EventArgs e)
        {
            cMaterialCheckBox mcb = sender as cMaterialCheckBox;
            mcb.mWrapper.enabled = mcb.Checked;
            //series on/off
            if (!bSkipRecalc)
            {
                RecalcChart();
                RecalcChartSpider();
                RecalcChartFrequency();
            }
        }

        private void genericPictureBox_Click(object sender, EventArgs e)
        {
            cMaterialPictureBox mpb = sender as cMaterialPictureBox;
            if (!mpb.mWrapper.enabled) return;

            ColorDialog MyDialog = new ColorDialog();
            MyDialog.AllowFullOpen = false;
            MyDialog.Color = mpb.BackColor;

            if (MyDialog.ShowDialog() == DialogResult.OK)
            {
                mpb.BackColor = MyDialog.Color;
                mpb.mWrapper.color = MyDialog.Color;
                //series color
                RecalcChart();
                RecalcChartSpider();
                RecalcChartFrequency();
            }
        }

        private void rbOriginal_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartSpider();
        }

        private void rbBest_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartSpider();
        }
        private void lbRadarAxis_ItemCheck(object sender, ItemCheckEventArgs e)
        {
            string item = lbRadarAxis.Items[e.Index] as string;
            if (e.CurrentValue == CheckState.Unchecked && invalid_axis.Contains(item))
            {
                e.NewValue = e.CurrentValue;
            }
        }

        private void rbDk_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartFrequency();
        }

        private void rbDf_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartFrequency();
        }

        private void chartFrequency_MouseMove(object sender, MouseEventArgs e)
        {
            if (bFreezeCursor) return;

            var pos = e.Location;

            Point mousePoint = new Point(e.X, e.Y);
            chartFrequency.ChartAreas[0].CursorX.SetCursorPixelPosition(mousePoint, true);
            chartFrequency.ChartAreas[0].CursorY.SetCursorPixelPosition(mousePoint, true);

            int x = e.X;
            if (x < 0) x = 0;
            if (x > chartFrequency.ClientSize.Width - 2) x = chartFrequency.ClientSize.Width - 2;
            double f = chartFrequency.ChartAreas[0].AxisX.PixelPositionToValue(x);
            bool isValid = (f >= chartFrequency.ChartAreas[0].AxisX.Minimum) && (f <= chartFrequency.ChartAreas[0].AxisX.Maximum);

            if (isValid)
            {
                tbChartFreq.Text = f.ToString("N2"); ;
                //calculate cursor values
                string caption = rbDk.Checked ? "Dk" : "Df";
                string id = rbDk.Checked ? ZStringConstants.DMLParameterIDDk : ZStringConstants.DMLParameterIDDf;

                foreach (cMaterialWrapper mW in wrapperList)
                {
                    ZMaterial zM = mW.material;
                    if (!mW.enabled) continue;

                    string materialName = mW.materialName;
                    //--string yVal = zM.MaterialParameters.GetByID(id).Value;
                    //--ZPlannerManager.cTableFunctionParser tabFunc = new ZPlannerManager.cTableFunctionParser(yVal);
                    //--double y = tabFunc.GetFunc(f);

                    double y = 0;
                    if (rbDk.Checked){
                        y = DkFamily[materialName].Interpolate(f, dResin);
                    }
                    else
                    {
                        y = DfFamily[materialName].Interpolate(f, dResin);
                    }
                     
                    mW.value = y.ToString("N3");
                }
            }
            else
            {
                tbChartFreq.Text = "";
                //invalidate cursor values
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    mW.value = "";
                }
            }

            foreach (Control c in tblMaterialPanel.Controls)
            {
                if (tblMaterialPanel.GetColumn(c) == 3)
                {
                    cMaterialLabel ml = c as cMaterialLabel;
                    ml.Text = ml.mWrapper.value;
                }
            }

        }

        private void chartFrequency_Click(object sender, EventArgs e)
        {
            bFreezeCursor = !bFreezeCursor;
        }

        private void tbResin_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Enter)
            {
                SetResin(tbResin.Text);
            }
        }

        private void tbResin_Leave(object sender, EventArgs e)
        {
            SetResin(tbResin.Text);
        }

        void SetResin(string val)
        {
            bool bValid = false;
            double r;
            if (val == "") //don't account for resin at all
            {
                r = 0;
                bValid = true;
            }
            else
            {
                bValid = Double.TryParse(val, out r);
            }

            if (bValid)
            {
                if (r != dResin)
                {
                    dResin = r;

                    //--tblMaterialPanel.SuspendLayout();
                    //--InitMaterialList();
                    //--tblMaterialPanel.ResumeLayout();

                    RecalcChart();
                    RecalcChartSpider();
                    RecalcChartFrequency();

                    string item = currentSelection.Text;//--lbMaterials.SelectedItem as string;
                    tbMaterial.Text = SelectedDescription(item);
                }
            }

        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            panelFrequency.Visible = tabControl1.SelectedIndex != 2;
            panelCursor.Visible = tabControl1.SelectedIndex == 2;
        }

        private void chartFrequency_MouseLeave(object sender, EventArgs e)
        {
            if (!bFreezeCursor)
            {
                tbChartFreq.Text = "";
                //invalidate cursor values
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    mW.value = "";
                }
            }
        }

        private void cbLabels_CheckedChanged(object sender, EventArgs e)
        {
            RecalcChartFrequency();
        }

        private void cbManufacturerColor_CheckedChanged(object sender, EventArgs e)
        {
        }


        private void initMaps()
        {
            Dictionary<string, Color> defaultColors = new Dictionary<string,Color>();
            defaultColors.Add("TUC", Color.Red);
            defaultColors.Add("ITEQ", Color.Green);
            defaultColors.Add("PANASONIC", Color.Blue);
            defaultColors.Add("PARK ELECTROCHEMICAL (NELCO)", Color.Orange);
            defaultColors.Add("SHENGYI", Color.LimeGreen);
            defaultColors.Add("ISOLA", Color.Magenta);
            defaultColors.Add("NANYA", Color.RoyalBlue);
            defaultColors.Add("EMC", Color.Brown);
            defaultColors.Add("DOOSAN", Color.BlueViolet);

            List<Color> palette = new List<Color> { Color.Red, Color.Green, Color.Blue, Color.Orange, Color.LimeGreen, Color.Magenta, Color.RoyalBlue, Color.Brown, Color.BlueViolet };
            int idx = -1;
            HashSet<string> materialSet = new HashSet<string>();
            foreach (ZMaterial zM in materialList)
            {
                string name = zM.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value;
                if (materialSet.Contains(name)) continue;

                materialSet.Add(name);
                idx++;
                bool bCheck = true;
                if (!checkMap.ContainsKey(name))
                {
                    checkMap.Add(name, bCheck);
                }
                Color c = palette[0];
                if (!colorMap.ContainsKey(name))
                {                    
                    if (defaultColors.ContainsKey(name.ToUpper()))
                    {
                        c = defaultColors[name.ToUpper()];
                    }
                    else
                    {
                        c = palette[idx % palette.Count];
                    }
                    colorMap.Add(name, c);
                }
            }
        }

        private void btnColorByManufacturer_Click(object sender, EventArgs e)
        {
            //set checked state and color by manufacturer
            ManufacturerColor mC = new ManufacturerColor(initialMaterialList, colorMap, checkMap);
            mC.StartPosition = FormStartPosition.Manual;

            mC.Location = new Point(btnColorByManufacturer.Location.X - 300, btnColorByManufacturer.Location.Y );
            
            if (mC.ShowDialog() != DialogResult.OK) return;

            if (cbColorByMnaufacturer.Checked)
            {
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    string name = mW.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value;

                    mW.color = colorMap[name];
                    mW.enabled = checkMap[name];

                    mapMaterial_ColorBox[mW].BackColor = mW.color;
                    mapMaterial_CheckBox[mW].Checked = mW.enabled;
                }
                RecalcChart();
                RecalcChartSpider();
                RecalcChartFrequency();
            }
        }

        private void cbColorByMnaufacturer_CheckedChanged(object sender, EventArgs e)
        {
            if (cbColorByMnaufacturer.Checked)
            {
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    string name = mW.material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDManufacturer).Value;

                    //store ind. params
                    mW.indColor = mW.color;
                    mW.indEnabled = mW.enabled;

                    mW.color = colorMap[name];
                    mW.enabled = checkMap[name];

                    mapMaterial_ColorBox[mW].BackColor = mW.color;
                    mapMaterial_CheckBox[mW].Checked = mW.enabled;
                }
            }
            else
            {
                foreach (cMaterialWrapper mW in wrapperList)
                {
                    //restore individual colors
                    mW.color = mW.indColor;
                    mW.enabled = mW.indEnabled;

                    mapMaterial_ColorBox[mW].BackColor = mW.color;
                    mapMaterial_CheckBox[mW].Checked = mW.enabled;
                }
            }

            RecalcChart();
            RecalcChartSpider();
            RecalcChartFrequency();
        }
    }

    public class cMaterialFilter
    {
        public cMaterialFilter() { }
        internal List<ZMaterial> MaterialList()
        {
            List<ZMaterial> materials;
            materials = (from material in ZPlannerManager.Dml.Materials
                         where
                         (!material.IsFiltered && !material.IsHidden)
                         select material).ToList();

//            List<ZMaterial> groupedMaterials;
//            groupedMaterials = materials.GroupBy(m => m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value)
//                   .Select(grp => grp.First())
//                   .ToList();
            
            List<ZMaterial> retval = new List<ZMaterial>();
            foreach (ZMaterial zM in materials)//groupedMaterials)
            {
                retval.Add(zM);
            }
            return retval;
        }
    }

    class MaterialMapperRunner
    {
        public MaterialMapperRunner() { }

        public void Run()
        {
            cMaterialFilter flt = new cMaterialFilter();
            string fltName = ZPlannerManager.DMLPanel.GetCurrentFilterName();
            string fltExpression = ZPlannerManager.DMLPanel.GetCurrentFilter();
            
            string filter="Filter: ";
            if (fltName.Length > 0){
                filter += fltName;
                filter += " = ";
            }
            if (fltExpression.Length > 0)
            {
                filter += fltExpression;
            }

            if (fltName.Length == 0 && fltExpression.Length == 0)
            {
                filter += "None";
            }

            System.Windows.Forms.Cursor currentCursor = System.Windows.Forms.Cursor.Current;
            System.Windows.Forms.Cursor.Current = Cursors.WaitCursor;
            ZPlannerManager.StatusMenu.StartProgress("Preparing the Material Mapper ...");
            MaterialMapper mapper = new MaterialMapper(flt.MaterialList(), filter, fltName, fltExpression);
            ZPlannerManager.StatusMenu.StopProgress("Preparing the Material Mapper ...");
            System.Windows.Forms.Cursor.Current = currentCursor;

            mapper.ShowDialog();
        }
    }
}
