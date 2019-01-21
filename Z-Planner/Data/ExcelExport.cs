using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Spreadsheet;
using System.Text;
using System.Linq;
//
using ClosedXML.Excel;
//
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using System.Windows.Forms;
using ZZero.ZPlanner.UI;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.Translators
{
    public enum ExportFlags{ 
        //layer data
        LayerNumber, LayerType, LayerName, ViaSpans,
        //dielectric material data
        Material, Construction, Resin, Dk, Df, Frequency, Thickness, 
        //copper, impedance, notes
        SingleEnded, Differential,
        CopperWeight, Copper, RzTop, RzBot, TraceWidth, Impedance, DiffImpedance, DiffZ0, RefLayer, Notes, Loss
    }
    public class ExportOptions : HashSet<ExportFlags>
    {
        private ExportOptions(bool Default = true)
        {
            if (Default)
            {
                Add(ExportFlags.LayerNumber);
                Add(ExportFlags.LayerType);
                Add(ExportFlags.LayerName);

                Add(ExportFlags.SingleEnded);
                Add(ExportFlags.Differential);
                Add(ExportFlags.Thickness);
                Add(ExportFlags.Dk);
                Add(ExportFlags.TraceWidth);
                Add(ExportFlags.Impedance);
                Add(ExportFlags.DiffImpedance);
                Add(ExportFlags.Loss);

                Add(ExportFlags.Material);
                Add(ExportFlags.Construction);
                Add(ExportFlags.Resin);
            }
        }

        public string ProjectName { get; set; }
        public string DesignRevision { get; set; }
        public string Fabricator { get; set; }
        public string FabContact { get; set; }

        //static instance to store data
        private static ExportOptions exportOptions;
        public static ExportOptions TheOptions
        {
            get
            {
                if (exportOptions == null)
                {
                    exportOptions = new ExportOptions();//default
                }
                return exportOptions;
            }
            set
            {
                if (exportOptions == null)
                {
                    exportOptions = new ExportOptions();//defaults
                }
                exportOptions = value;
            }
        }

    }

    class ExcelExporter
    {
        //upper-left cell
        const int ulRow = 5;
        const int ulCol = 3;


        private string OutputFile;
        IXLWorksheet worksheet;
        ZStackup z;
        ExportOptions options;

        public ExcelExporter(string file, ExportOptions opt)
        {
            OutputFile = file;
            options = opt;
        }
        public void Export()
        {
            ExportClosedXML();
        }

        //---------------------------------------- Export ClosedXML ---------------------------------------------------------
        private XLColor ConvertColor(System.Drawing.Color c)
        {
            XLColor xc = XLColor.FromArgb(c.R, c.G, c.B);
            return xc;
        }

        public void ExportClosedXML()
        {
            z = ZPlannerManager.Project.Stackup;
            if (z == null) return;

            var workbook = new XLWorkbook(OutputFile); //template with inserted logo
            worksheet = workbook.Worksheet(1);//--workbook.Worksheets.Add(z.Title);
            worksheet.Name = (z.Title.Length > 31) ? z.Title.Substring(0, 31) : z.Title;

            //worksheet.PageSetup.PaperSize = XLPaperSize.A4Paper;
            worksheet.PageSetup.PageOrientation = XLPageOrientation.Landscape;
            worksheet.PageSetup.FitToPages(1, 1);
            worksheet.PageSetup.CenterHorizontally = true;
            worksheet.PageSetup.CenterVertically = true;
            //narrow margins
            worksheet.PageSetup.Margins.Left = 0.25;
            worksheet.PageSetup.Margins.Right = 0.25;
            worksheet.PageSetup.Margins.Top = 0.75;
            worksheet.PageSetup.Margins.Bottom = 0.75;
            worksheet.PageSetup.Margins.Header = 0.3;
            worksheet.PageSetup.Margins.Footer = 0.3;


            int rightColumn;
            ExportStackup(out rightColumn);
            ExportHeader();

            int iRow = ulRow, iCol = ulCol;
            iRow += z.Layers.Count + 5;
            /*
            if (options.Contains(ExportFlags.DiffImpedance))
            {
                iRow += ExportDiff(iRow, iCol) + 5;
            }
            */
            int footerRight = ExportFooter(iRow, iCol);

            //external border
            IXLRow lRow = worksheet.LastRowUsed();
            int iLRow = lRow.RowNumber();
            IXLColumn lCol = worksheet.LastColumnUsed();
            int iLCol = lCol.ColumnNumber();

            int rightmostColumn = Math.Max(rightColumn, footerRight);

            var rngTable = worksheet.Range(2, 2, iLRow, rightmostColumn + 2);//iLCol + 2);
            rngTable.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

            //
            worksheet.ShowGridLines = false;
            try
            {
                workbook.SaveAs(OutputFile);
                workbook.Dispose();

                //open again to fire Open event
                var workbook1 = new XLWorkbook(OutputFile); //template with inserted logo
                worksheet = workbook1.Worksheet(1);//--workbook.Worksheets.Add(z.Title);            
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
            }

        }

        private void ExportHeader()
        {
            string units = Options.TheOptions.units == Options.Units.English ? "mils" : "mm";
            worksheet.Cell(3, 3).Value = "Units: " + units;
            string prepreg = ZPlannerManager.StackupPanel != null && ZPlannerManager.IsPressedThickness ? "Finished" : "Unpressed";
            worksheet.Cell(3, 5).Value = "Prepreg Thickness: " + prepreg;
            double fThickness = z.GetBoardThickness();
            if (Settings.Options.TheOptions.isUnitsMetric())
                fThickness *= Units.fMilsToMillimeters;
            worksheet.Cell(3, 8).Value = "Total Board Thickness: " + fThickness.ToString();
        }

        private int ExportFooter(int iRow, int iCol)
        {
            int row = iRow, col = iCol;
            //row 1
            worksheet.Cell(row, col).Value = "Company:";
            InitFooterCell(row,col);
            row++;

            worksheet.Cell(row, col).Value = "Project Name/Part #:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Design Revision:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Designer/Email:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Z-planner File Name:";
            InitFooterCell(row, col);
            row++;

            for (int i = iRow; i < iRow + 5; i++)
            {
                string s = worksheet.Cell(i, iCol).Value.ToString();
                worksheet.Range(i, iCol, i, iCol + 2).Merge();
                worksheet.Cell(i, iCol).Value = s;

                worksheet.Range(i, iCol + 3, i, iCol + 8).Merge();
            }
            worksheet.Cell(iRow, iCol + 3).Value = Options.TheOptions.Company;
            worksheet.Cell(iRow + 1, iCol + 3).Value = options.ProjectName;//project
            worksheet.Cell(iRow + 2, iCol + 3).SetValue(options.DesignRevision);
            worksheet.Cell(iRow + 2, iCol + 3).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Left;

            worksheet.Cell(iRow + 3, iCol + 3).Value = Options.TheOptions.UserEMail;
            worksheet.Cell(iRow + 4, iCol + 3).Value = Path.GetFileName(ZPlannerManager.ProjectFile);//z0p file


            //row 2
            row = iRow; col = iCol + 12;
            worksheet.Cell(row, col).Value = "Fabricator:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Fab Contact/Email:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Date:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Excel File Name:";
            InitFooterCell(row, col);
            row++;

            worksheet.Cell(row, col).Value = "Field Solver Software:";
            InitFooterCell(row, col);
            row++;

            for (int i = iRow; i < iRow + 5; i++)
            {
                string s = worksheet.Cell(i, col).Value.ToString();
                worksheet.Range(i, col-2, i, col + 1).Merge();
                worksheet.Cell(i, col - 2).Value = s;
                worksheet.Cell(i, col - 2).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Right;
                worksheet.Cell(i, col - 2).Style.Font.Bold = true;

                
                worksheet.Range(i, col + 2, i, col + 7).Merge();
            }

            worksheet.Cell(iRow, col + 2).Value = options.Fabricator;//Fabricator
            worksheet.Cell(iRow + 1, col + 2).Value = options.FabContact;//fab contact
            worksheet.Cell(iRow + 2, col + 2).Value = DateTime.Now.ToLongDateString();
            worksheet.Cell(iRow + 2, col + 2).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Left;

            worksheet.Cell(iRow + 3, col + 2).Value = Path.GetFileName(OutputFile);//xlsx file

            worksheet.Cell(iRow + 4, col + 2).Value = "Z-zero / HyperLynx";

            var rngTable = worksheet.Range(iRow, iCol + 3, iRow + 4, iCol + 8);
            rngTable.Style.Border.TopBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.LeftBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

            rngTable = worksheet.Range(iRow, iCol + 14, iRow + 4, iCol + 19);
            rngTable.Style.Border.TopBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.LeftBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

            return iCol + 19;
        }

        private void AddHeader(int row, int col, string Title)
        {
            worksheet.Cell(row, col).Value = "";
            InitHeaderCell(row, col);
            worksheet.Cell(row + 1, col).Value = Title;
            InitHeaderCell(row + 1, col);
        }

        private bool CommonFrequency(out double F)
        {
            F = 0;
            HashSet<double> ff = new HashSet<double>();
            foreach (ZSingle single in z.Singles)
            {
                foreach (ZLayer lay in single.Layers)
                {
                    switch (lay.GetLayerType()) { 
                        case ZLayerType.Signal: 
                        case ZLayerType.SplitMixed:
                            double f = lay.GetFrequency();
                            ff.Add(f);
                            break;
                    }
                }
            }

            foreach (ZPair pair in z.Pairs)
            {
                foreach (ZLayer lay in pair.Layers)
                {
                    switch (lay.GetLayerType())
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            double f = lay.GetFrequency();
                            ff.Add(f);
                            break;
                    }
                }
            }

            if (ff.Count == 1){
                foreach (double x in ff){
                    F = x;
                }
            }
            return ff.Count == 1;
        }

        private bool GetCommonDk(ZLayer lay, out double Dk)
        {
            Dk = Options.TheOptions.resin_Dk;

            double x;
            ZSingle single = z.ActiveSingle;
            ZLayer slay = single.GetLayerOfSingleImpedance(lay.ID);
            string sDk = slay.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
            if (ZPlannerManager.GetFirstValueFromTable(sDk, out x))
            {
                Dk = x;
                return true;
            }

            ZPair pair = z.ActivePair;
            ZLayer play = pair.GetLayerOfPairImpedance(lay.ID);
            string pDk = play.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant);
            if (ZPlannerManager.GetFirstValueFromTable(pDk, out x))
            {
                Dk = x;
                return true;
            }

            string lDk = lay.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
            if (ZPlannerManager.GetFirstValueFromTable(lDk, out x))
            {
                Dk = x;
                return true;
            }

            return false;
        }

        private bool GetCommonDf(ZLayer lay, out double Df)
        {
            Df = Options.TheOptions.resin_Df;

            double x;
            ZSingle single = z.ActiveSingle;
            ZLayer slay = single.GetLayerOfSingleImpedance(lay.ID);
            string sDf = slay.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
            if (ZPlannerManager.GetFirstValueFromTable(sDf, out x))
            {
                Df = x;
                return true;
            }

            ZPair pair = z.ActivePair;
            ZLayer play = pair.GetLayerOfPairImpedance(lay.ID);
            string pDf = play.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent);
            if (ZPlannerManager.GetFirstValueFromTable(pDf, out x))
            {
                Df = x;
                return true;
            }

            string lDf = lay.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);
            if (ZPlannerManager.GetFirstValueFromTable(lDf, out x))
            {
                Df = x;
                return true;
            }

            return false;
        }

        private void ExportStackup(out int rightColumn)
        {
            double F1;
            bool bOneF = CommonFrequency(out F1);

            int row1 = ulRow, col1 = ulCol;
            int colImpedance1 = -1;

            //Header row(s)
            int headerRow = row1, headerColumn = col1;
            //RefLayer, Notes 

            //Layer Data
            if (options.Contains(ExportFlags.LayerNumber))
            {
                AddHeader(headerRow, headerColumn, "Layer #");
                headerColumn++;
            }

            //via span header
            int nSpans = z.Spans.Count;
            int spanCol1 = headerColumn;
            if (options.Contains(ExportFlags.ViaSpans))
            {
                if (nSpans > 0)
                {
                    foreach (ZSpan span in z.Spans)
                    {
                        AddHeader(headerRow, headerColumn, span.Size.ToString());
                        headerColumn++;
                    }
                }
            }
            //

            if (options.Contains(ExportFlags.LayerType))
            {
                AddHeader(headerRow, headerColumn, "Type");
                headerColumn++;
            }

            if (options.Contains(ExportFlags.LayerName))
            {
                AddHeader(headerRow, headerColumn, "Layer Name");
                headerColumn++;
            }

            //Dielectric material data
            if (options.Contains(ExportFlags.Material))
            {
                AddHeader(headerRow, headerColumn, "Material");
                headerColumn++;
            }

            if (options.Contains(ExportFlags.Construction))
            {
                AddHeader(headerRow, headerColumn, "Construction");
                headerColumn++;
            }

            if (options.Contains(ExportFlags.Resin))
            {
                AddHeader(headerRow, headerColumn, "Resin" + Environment.NewLine + "(%)");
                headerColumn++;
            }

            if (bOneF)
            {
                if (options.Contains(ExportFlags.Dk))
                {
                    AddHeader(headerRow, headerColumn, "Dk");
                    headerColumn++;
                }

                if (options.Contains(ExportFlags.Df))
                {
                    AddHeader(headerRow, headerColumn, "Df");
                    headerColumn++;
                }

                if (options.Contains(ExportFlags.Frequency))
                {
                    AddHeader(headerRow, headerColumn, "Frequency" + Environment.NewLine + "(GHz)");
                    headerColumn++;
                }
            }

            if (options.Contains(ExportFlags.Copper))
            {
                AddHeader(headerRow, headerColumn, "%" + Environment.NewLine + "Copper");
                headerColumn++;
            }

            if (options.Contains(ExportFlags.Thickness))
            {
                AddHeader(headerRow, headerColumn, "Thickness" + Environment.NewLine + "(mils)");
                headerColumn++;
            }
            
            //copper, impedance, notes
            if (options.Contains(ExportFlags.CopperWeight))
            {
                AddHeader(headerRow, headerColumn, "Copper"+ Environment.NewLine + "Weight" + Environment.NewLine + "(oz)");
                headerColumn++;
            }
            if (options.Contains(ExportFlags.Notes))
            {
                AddHeader(headerRow, headerColumn, "Comments");
                headerColumn++;
            }

            if (options.Contains(ExportFlags.RzTop))
            {
                AddHeader(headerRow, headerColumn, "Rz-Top" + Environment.NewLine + "(um)");
                headerColumn++;
            }
            if (options.Contains(ExportFlags.RzBot))
            {
                AddHeader(headerRow, headerColumn, "Rz-Bot" + Environment.NewLine + "(um)");
                headerColumn++;
            }

            //single-ended
            if (options.Contains(ExportFlags.SingleEnded))
            {
                foreach (ZSingle single in z.Singles)
                {
                    if (options.Contains(ExportFlags.TraceWidth))
                    {
                        AddHeader(headerRow, headerColumn, "Trace" + Environment.NewLine + "Width" + Environment.NewLine + "(mils)");
                        headerColumn++;
                    }

                    if (options.Contains(ExportFlags.Impedance))
                    {
                        AddHeader(headerRow, headerColumn, "Zo" + Environment.NewLine + "(ohms)");
                        headerColumn++;
                    }

                    if (!bOneF)
                    {
                        if (options.Contains(ExportFlags.Dk))
                        {
                            AddHeader(headerRow, headerColumn, "Dk");
                            headerColumn++;
                        }

                        if (options.Contains(ExportFlags.Df))
                        {
                            AddHeader(headerRow, headerColumn, "Df");
                            headerColumn++;
                        }

                        if (options.Contains(ExportFlags.Frequency))
                        {
                            AddHeader(headerRow, headerColumn, "Freq." + Environment.NewLine + "(GHz)");
                            headerColumn++;
                        }
                    }
                    /*
                    if (options.Contains(ExportFlags.Loss))
                    {
                        AddHeader(headerRow, headerColumn, "Insertion Loss" + Environment.NewLine + "(dB/in)");
                        headerColumn++;
                    }
                    */
                    if (options.Contains(ExportFlags.RefLayer))
                    {
                        AddHeader(headerRow, headerColumn, "Plane" + Environment.NewLine + "Ref.");
                        headerColumn++;
                    }
                }
            }

            //differential pairs
            if (options.Contains(ExportFlags.Differential))
            {
                foreach (ZPair pair in z.Pairs)
                {
                    if (options.Contains(ExportFlags.TraceWidth))
                    {
                        AddHeader(headerRow, headerColumn, "Trace" + Environment.NewLine + "Width" + Environment.NewLine + "(mils)");
                        headerColumn++;
                    }
                    if (options.Contains(ExportFlags.TraceWidth)) //no flag for separation!!!!
                    {
                        AddHeader(headerRow, headerColumn, "Trace" + Environment.NewLine + "Separation" + Environment.NewLine + "(mils)");
                        headerColumn++;
                    }
                    if (options.Contains(ExportFlags.DiffZ0))
                    {
                        AddHeader(headerRow, headerColumn, "Zo" + Environment.NewLine + "(ohms)");
                        headerColumn++;
                    }
                    if (options.Contains(ExportFlags.DiffImpedance))
                    {
                        AddHeader(headerRow, headerColumn, "Zdiff" + Environment.NewLine + "(ohms)");
                        headerColumn++;
                    }
                    if (!bOneF)
                    {
                        if (options.Contains(ExportFlags.Dk))
                        {
                            AddHeader(headerRow, headerColumn, "Dk");
                            headerColumn++;
                        }

                        if (options.Contains(ExportFlags.Df))
                        {
                            AddHeader(headerRow, headerColumn, "Df");
                            headerColumn++;
                        }

                        if (options.Contains(ExportFlags.Frequency))
                        {
                            AddHeader(headerRow, headerColumn, "Freq." + Environment.NewLine + "(GHz)");
                            headerColumn++;
                        }
                    }

                    if (options.Contains(ExportFlags.Loss))
                    {
                        AddHeader(headerRow, headerColumn, "Insertion" + Environment.NewLine + "Loss" + Environment.NewLine + "(dB/in)");
                        headerColumn++;
                    }

                    if (options.Contains(ExportFlags.RefLayer))
                    {
                        AddHeader(headerRow, headerColumn, "Plane" + Environment.NewLine + "Ref.");
                        headerColumn++;
                    }
                }
            }

            rightColumn = headerColumn - 1;

            //layers data
            int layerRow = row1 + 2, layerColumn = col1;
            foreach (ZLayer zl in z.Layers)
            {
                //--ZLayer zlSingle = z.ActiveSingle.GetLayerOfSingleImpedance(zl.ID);

                layerColumn = col1;

                XLColor backColor = XLColor.White;
                switch (zl.GetLayerType())
                {
                    case ZLayerType.Core:
                        backColor = ConvertColor(ZColorConstants.CoreLayerColor);
                        break;
                    case ZLayerType.Prepreg:
                        backColor = ConvertColor(ZColorConstants.PrepregLayerColor);
                        break;
                    case ZLayerType.SolderMask:
                        backColor = ConvertColor(ZColorConstants.SolderMaskLayerColor);
                        break;

                    case ZLayerType.Plane:
                        backColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                        break;
                    case ZLayerType.Signal:
                        backColor = ConvertColor(ZColorConstants.SignalLayerColor);
                        break;
                    case ZLayerType.SplitMixed:
                        backColor = ConvertColor(ZColorConstants.SplitMixedLayerColor);
                        break;
                }

                //Layer Data
                int ival = 0;
                if (options.Contains(ExportFlags.LayerNumber))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDLayerNumber, ref ival);
                    worksheet.Cell(layerRow, layerColumn).Value = ival > 0 ? ival.ToString() : "";
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    layerColumn++;
                }

                //spans
                if (options.Contains(ExportFlags.ViaSpans))
                {
                    for (int ix = layerColumn; ix < layerColumn + nSpans; ix++)
                    {
                        worksheet.Cell(layerRow, ix).Style.Fill.BackgroundColor = backColor;
                    }
                    layerColumn += nSpans;
                }

                string sval = "";
                if (options.Contains(ExportFlags.LayerType))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDLayerType, ref sval);
                    worksheet.Cell(layerRow, layerColumn).Value = sval;
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    layerColumn++;
                }

                sval = "";
                if (options.Contains(ExportFlags.LayerName))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDLayerName, ref sval);
                    worksheet.Cell(layerRow, layerColumn).Value = sval;
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    layerColumn++;
                }

                //Dielectric material data
                //material, construction, resin
                if (options.Contains(ExportFlags.Material))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDMaterial, ref sval);
                    worksheet.Cell(layerRow, layerColumn).Value = sval;
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    layerColumn++;
                }

                if (options.Contains(ExportFlags.Construction))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDConstruction, ref sval);
                    worksheet.Cell(layerRow, layerColumn).Value = sval;
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    layerColumn++;
                }

                double dval = 0;
                if (options.Contains(ExportFlags.Resin))
                {
                    string value = zl.GetLayerParameterValue(ZStringConstants.ParameterIDResinContent);
                    char[] charsToTrim = { '%', ' ' };
                    value =  value.Trim(charsToTrim);

                    double dValue = 0;
                    if (Double.TryParse(value, out dValue)){
                        sval = String.Format("N" + Options.TheOptions.percentDigits.ToString());
                        worksheet.Cell(layerRow, layerColumn).Value = dValue > 0 ? dValue.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    }
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    layerColumn++;
                }

                dval = 0;
                if (bOneF)
                {
                    if (options.Contains(ExportFlags.Dk))
                    {
                        GetCommonDk(zl, out dval);
                        sval = String.Format("N" + Options.TheOptions.dkDigits.ToString(), dval);
                        worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;
                    }

                    dval = 0;

                    if (options.Contains(ExportFlags.Df))
                    {
                        GetCommonDf(zl, out dval);
                        sval = String.Format("N" + Options.TheOptions.dfDigits.ToString(), dval);
                        worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;
                    }

                    dval = 0;

                    if (options.Contains(ExportFlags.Frequency))
                    {
                        switch (zl.GetLayerType())
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                dval = F1;
                                break;
                        }
                        worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;
                    }
                }

                dval = 0;
                if (options.Contains(ExportFlags.Copper))
                {
                    //ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDCopperPercent, ref dval);
                    //sval = String.Format("N" + Options.TheOptions.percentDigits.ToString(), dval);
                    //worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                    worksheet.Cell(layerRow, layerColumn).Value = zl.GetLayerCopperCoverage("N1");

                    ZLayerParameter layerParameter = zl.GetLayerParameter(ZStringConstants.ParameterIDCopperPercent);
                    bool isBold = (layerParameter != null) && layerParameter.IsReadOnly(false, true, true);

                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    if (isBold) worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;

                    layerColumn++;
                }

                dval = 0;
                bool bPressed = false;
                if (options.Contains(ExportFlags.Thickness))
                {
                    XLColor bkColor = backColor;
                    if (zl.GetLayerType() == ZLayerType.Signal)
                    {
                        bkColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                    }
                    if (zl.GetLayerType() == ZLayerType.Prepreg && ZPlannerManager.StackupPanel != null && ZPlannerManager.IsPressedThickness)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, ref dval);
                        bPressed = true;
                    }
                    else
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);
                    }
                    sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString(), dval);
                    worksheet.Cell(layerRow, layerColumn).Value = dval.ToString(sval);
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = bkColor;                    
                    if (bPressed) worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    layerColumn++;
                }

                //copper, impedance, notes

                dval = 0;
                if (options.Contains(ExportFlags.CopperWeight))
                {
                    string sW = zl.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness);
                    if (sW.Length > 0)
                    {
                        //sval = String.Format("N" + Options.TheOptions.weightDigits.ToString(), dval);
                        worksheet.Cell(layerRow, layerColumn).DataType = XLCellValues.Text;
                        worksheet.Cell(layerRow, layerColumn).Value = "'"+sW;
                        worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    }
                    layerColumn++;
                }

                if (options.Contains(ExportFlags.Notes))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, ref sval);
                    worksheet.Cell(layerRow, layerColumn).Value = sval;
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    layerColumn++;
                }

                dval = 0;
                if (options.Contains(ExportFlags.RzTop))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughTop, ref dval);
                    if (dval == 0)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughTop, ref dval);
                    }
                    //sval = String.Format("F1", dval);
                    worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString("F1") : "";
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.0";
                    layerColumn++;
                }

                dval = 0;
                if (options.Contains(ExportFlags.RzBot))
                {
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDRoughBottom, ref dval);
                    if (dval == 0)
                    {
                        ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDCalcRoughBottom, ref dval);
                    }
                    //sval = String.Format("F1", dval);
                    worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString("F1") : "";
                    worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                    worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.0";
                    layerColumn++;
                }

                // Single-ended signals
                if (options.Contains(ExportFlags.SingleEnded))
                {
                    bool bFirst = true;
                    foreach (ZSingle single in z.Singles)
                    {
                        if (bFirst)
                        {
                            colImpedance1 = layerColumn;
                        }
                        int impCol1 = layerColumn;

                        ZLayer zlSingle = single.GetLayerOfSingleImpedance(zl.ID);
                        double width = 0, z0 = 0, loss = 0;
                        switch (zl.GetLayerType())
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                //-- ZLayer zlSingle = z.GetLayerOfSingleImpedance(zl.ID);
                                ZPlannerProject.GetLayerParameterValue(zlSingle, ZStringConstants.ParameterIDZo_TraceWidth, ref width);
                                ZPlannerProject.GetLayerParameterValue(zlSingle, ZStringConstants.ParameterIDZo_Zo, ref z0);
                                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDZo_InsertionLoss, ref loss);
                                break;
                        }

                        if (options.Contains(ExportFlags.TraceWidth))
                        {
                            XLColor bkColor = backColor;
                            if (zl.GetLayerType() == ZLayerType.Signal)
                            {
                                bkColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                            }

                            sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString(), width);
                            worksheet.Cell(layerRow, layerColumn).Value = width > 0 ? width.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = bkColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            layerColumn++;
                        }

                        if (options.Contains(ExportFlags.Impedance))
                        {
                            sval = String.Format("N" + Options.TheOptions.impedanceDigits.ToString(), z0);
                            worksheet.Cell(layerRow, layerColumn).Value = z0 > 0 ? z0.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;
                            layerColumn++;
                        }
                        /*
                        if (options.Contains(ExportFlags.Loss))
                        {
                            sval = String.Format("N2", loss);
                            worksheet.Cell(layerRow, layerColumn).Value = loss > 0 ? loss.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            layerColumn++;
                        }
                        */
                        if (!bOneF)
                        {
                            if (options.Contains(ExportFlags.Dk))
                            {
                                string sDk = zlSingle.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
                                ZPlannerManager.GetFirstValueFromTable(sDk, out dval);

                                sval = String.Format("N" + Options.TheOptions.dkDigits.ToString(), dval);
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.00";
                                layerColumn++;
                            }

                            dval = 0;

                            if (options.Contains(ExportFlags.Df))
                            {
                                string sDk = zlSingle.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                                ZPlannerManager.GetFirstValueFromTable(sDk, out dval);

                                sval = String.Format("N" + Options.TheOptions.dfDigits.ToString(), dval);
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.000";
                                layerColumn++;
                            }

                            dval = 0;

                            if (options.Contains(ExportFlags.Frequency))
                            {
                                //--string sDk = zl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Frequency);
                                //--ZPlannerManager.GetFirstValueFromTable(sDk, out dval);
                                sval = "";
                                switch (zl.GetLayerType())
                                {
                                    case ZLayerType.Signal:
                                    case ZLayerType.SplitMixed:
                                        dval = zlSingle.GetFrequency();
                                        sval = String.Format("N" + Options.TheOptions.weightDigits.ToString(), dval);
                                        break;
                                }
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.0";
                                layerColumn++;
                            }
                        }

                        if (options.Contains(ExportFlags.RefLayer))
                        {
                            sval = "";
                            //--ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPlaneReference, ref sval);
                            string topId = zlSingle.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TopReference);
                            string botId = zlSingle.GetLayerParameterValue(ZStringConstants.ParameterIDZo_BottomReference);
                            ZLayer topLayer = null, botLayer = null; 
                            if (topId != null && topId.Length > 0) topLayer = z.GetLayerOfStackup(topId);
                            if (botId != null && botId.Length > 0) botLayer = z.GetLayerOfStackup(botId);
                            string topNum = "", botNum = "";
                            if (topLayer != null)
                            {
                                topNum = topLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber);
                            }
                            if (botLayer != null)
                            {
                                botNum = botLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber);
                            }
                            if (topNum.Length > 0)
                            {
                                sval = topNum;
                                if (botNum.Length > 0) sval += ", " + botNum;
                            }
                            else
                            {
                                if (botNum.Length > 0) sval = botNum;
                            }

                            worksheet.Cell(layerRow, layerColumn).SetValue(sval);
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;

                            layerColumn++;
                        }

                        if (bFirst)
                        {
                            bFirst = false;
                        }
                        worksheet.Range(headerRow, impCol1, headerRow, layerColumn - 1).Merge();
                        worksheet.Cell(headerRow, impCol1).Value = "SE (" + single.Title + ")";
                        
                    }
                }

                if (options.Contains(ExportFlags.Differential))
                {
                    bool bFirstDiff = true;
                    foreach (ZPair pair in z.Pairs)
                    {
                        if (colImpedance1 < 0 && bFirstDiff)
                        {
                            colImpedance1 = layerColumn;
                        }

                        int impCol1 = layerColumn;

                        ZLayer zlPair = pair.GetLayerOfPairImpedance(zl.ID);
                        double width = 0, z0 = 0, zdiff = 0, separation = 0, loss = 0;
                        switch (zl.GetLayerType())
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                //-- ZLayer zlSingle = z.GetLayerOfSingleImpedance(zl.ID);
                                ZPlannerProject.GetLayerParameterValue(zlPair, ZStringConstants.ParameterIDZdiff_TraceWidth, ref width);
                                ZPlannerProject.GetLayerParameterValue(zlPair, ZStringConstants.ParameterIDZdiff_TraceSpacing, ref separation);
                                ZPlannerProject.GetLayerParameterValue(zlPair, ZStringConstants.ParameterIDZdiff_Zo, ref z0);
                                ZPlannerProject.GetLayerParameterValue(zlPair, ZStringConstants.ParameterIDZdiff_Zdiff, ref zdiff);
                                ZPlannerProject.GetLayerParameterValue(zlPair, ZStringConstants.ParameterIDZdiff_InsertionLoss, ref loss);
                                break;
                        }

                        if (options.Contains(ExportFlags.TraceWidth))
                        {
                            XLColor bkColor = backColor;
                            if (zl.GetLayerType() == ZLayerType.Signal)
                            {
                                bkColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                            }

                            sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString(), width);
                            worksheet.Cell(layerRow, layerColumn).Value = width > 0 ? width.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = bkColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            layerColumn++;
                        }

                        if (options.Contains(ExportFlags.TraceWidth))
                        {
                            XLColor bkColor = backColor;
                            if (zl.GetLayerType() == ZLayerType.Signal)
                            {
                                bkColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                            }

                            sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString(), separation);
                            worksheet.Cell(layerRow, layerColumn).Value = separation > 0 ? separation.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = bkColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            layerColumn++;
                        }

                        if (options.Contains(ExportFlags.DiffZ0))
                        {
                            sval = String.Format("N" + Options.TheOptions.impedanceDigits.ToString(), z0);
                            worksheet.Cell(layerRow, layerColumn).Value = z0 > 0 ? z0.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;
                            layerColumn++;
                        }
                        if (options.Contains(ExportFlags.DiffImpedance))
                        {
                            sval = String.Format("N" + Options.TheOptions.impedanceDigits.ToString(), zdiff);
                            worksheet.Cell(layerRow, layerColumn).Value = zdiff > 0 ? zdiff.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;
                            layerColumn++;
                        }

                        if (!bOneF)
                        {
                            if (options.Contains(ExportFlags.Dk))
                            {
                                string sDk = zlPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant);
                                ZPlannerManager.GetFirstValueFromTable(sDk, out dval);

                                sval = String.Format("N" + Options.TheOptions.dkDigits.ToString(), dval);
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.00";
                                layerColumn++;
                            }

                            dval = 0;

                            if (options.Contains(ExportFlags.Df))
                            {
                                string sDk = zlPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent);
                                ZPlannerManager.GetFirstValueFromTable(sDk, out dval);

                                sval = String.Format("N" + Options.TheOptions.dfDigits.ToString(), dval);
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.000";
                                layerColumn++;
                            }

                            dval = 0;

                            if (options.Contains(ExportFlags.Frequency))
                            {
                                //--string sDk = zl.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Frequency);
                                //--ZPlannerManager.GetFirstValueFromTable(sDk, out dval);
                                sval = "";
                                switch (zl.GetLayerType())
                                {
                                    case ZLayerType.Signal:
                                    case ZLayerType.SplitMixed:
                                        dval = zlPair.GetFrequency();
                                        sval = String.Format("N" + Options.TheOptions.weightDigits.ToString(), dval);
                                        break;
                                }
                                worksheet.Cell(layerRow, layerColumn).Value = dval > 0 ? dval.ToString(sval) : "";
                                worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                                worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                                worksheet.Cell(layerRow, layerColumn).Style.NumberFormat.Format = "0.0";
                                layerColumn++;
                            }
                        }

                        if (options.Contains(ExportFlags.Loss))
                        {
                            sval = String.Format("N2", loss);
                            worksheet.Cell(layerRow, layerColumn).Value = loss > 0 ? loss.ToString(sval) : "";
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            worksheet.Cell(layerRow, layerColumn).Style.Font.Bold = true;
                            layerColumn++;
                        }


                        if (options.Contains(ExportFlags.RefLayer))
                        {
                            sval = "";
                            //--ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDPlaneReference, ref sval);
                            string topId = zlPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TopReference);
                            string botId = zlPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_BottomReference);
                            ZLayer topLayer = null, botLayer = null; ;
                            if (topId != null && topId.Length > 0) topLayer = z.GetLayerOfStackup(topId);
                            if (botId != null && botId.Length > 0) botLayer = z.GetLayerOfStackup(botId);
                            string topNum = "", botNum = "";
                            if (topLayer != null)
                            {
                                topNum = topLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber);
                            }
                            if (botLayer != null)
                            {
                                botNum = botLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber);
                            }
                            if (topNum.Length > 0)
                            {
                                sval = topNum;
                                if (botNum.Length > 0) sval += ", " + botNum;
                            }
                            else
                            {
                                if (botNum.Length > 0) sval = botNum;
                            }

                            worksheet.Cell(layerRow, layerColumn).SetValue(sval);
                            worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = backColor;
                            worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                            layerColumn++;
                        }

                        if (bFirstDiff)
                        {
                            bFirstDiff = false;
                        }
                        worksheet.Range(headerRow, impCol1, headerRow, layerColumn - 1).Merge();
                        worksheet.Cell(headerRow, impCol1).Value = "DIFFERENTIAL (" + pair.Title + ")";
                        
                    }
                }

                //
                //
                //
                layerRow++;
            }

            //fill in via span data
            if (options.Contains(ExportFlags.ViaSpans))
            {
                if (nSpans > 0)
                {
                    //XLColor Color1 = ConvertColor(ZColorConstants.ViaSpanGradientOuterColor);
                    //XLColor Color2 = ConvertColor(ZColorConstants.ViaSpanGradientInnerColor);
                    int spanCol = spanCol1;
                    int spanRow1 = row1 + 2;
                    foreach (ZSpan span in z.Spans)
                    {
                        int spanIndex1 = z.GetLayerOfStackupIndex(span.FirstLayer.ID);
                        int spanIndex2 = z.GetLayerOfStackupIndex(span.LastLayer.ID);
                        for (int i = spanIndex1; i <= spanIndex2; i++)
                        {
                            worksheet.Cell(spanRow1 + i, spanCol).Style.Fill.BackgroundColor = XLColor.DarkGreen;
                        }
                        spanCol++;
                    }
                }
            }
            //

            worksheet.Columns(col1, headerColumn).AdjustToContents();
            //make Dk column width equal to Df column
            for (int i = col1; i < headerColumn; i++)
            {
                string a = worksheet.Cell(headerRow + 1, i).Value.ToString();
                string b = worksheet.Cell(headerRow + 1, i + 1).Value.ToString();
                if ((a == "Dk") && (b == "Df"))
                {
                    worksheet.Column(i).Width = worksheet.Column(i + 1).Width;
                }
            }

            var rngTable = worksheet.Range(row1, col1, layerRow - 1, layerColumn - 1);
            rngTable.Style.Border.TopBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.LeftBorder = XLBorderStyleValues.Thin;
            rngTable.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;

            if (colImpedance1 > 0)
            {
                for (int i = col1; i < colImpedance1/*layerColumn*/; i++)
                {
                    string s = worksheet.Cell(row1 + 1, i).Value.ToString();
                    worksheet.Range(row1, i, row1 + 1, i).Merge();
                    worksheet.Cell(row1, i).Value = s;

                    worksheet.Cell(row1, i).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    worksheet.Cell(row1, i).Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
                }

                for (int i = colImpedance1 + 1; i < layerColumn; i++)
                {
                    worksheet.Cell(row1 + 1, i).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                    worksheet.Cell(row1 + 1, i).Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
                }
            }


            if (options.Contains(ExportFlags.ViaSpans))
            {
                if (nSpans > 0)
                {
                    worksheet.Range(row1, spanCol1, row1, spanCol1 + nSpans - 1).Merge();
                    worksheet.Cell(row1, spanCol1).Value = "Via Span";
                }
            }

            //impedance group
            /*
            if (options.Contains(ExportFlags.TraceWidth) && options.Contains(ExportFlags.Impedance))
            {
                worksheet.Range(row1, impCol1, row1, impCol1 + 1).Merge();
                worksheet.Cell(row1, impCol1).Value = "Impedance";
            }
            */
        }

        private void InitHeaderCell(int headerRow, int headerColumn)
        {
            worksheet.Cell(headerRow, headerColumn).Style.Fill.BackgroundColor = XLColor.BattleshipGrey;//XLColor.Black;//
            worksheet.Cell(headerRow, headerColumn).Style.Font.FontColor = XLColor.White;
            worksheet.Cell(headerRow, headerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
            worksheet.Cell(headerRow, headerColumn).Style.Font.Bold = true;
        }

        private void InitFooterCell(int footerRow, int footerColumn)
        {
            //worksheet.Cell(footerRow, footerColumn).Style.Fill.BackgroundColor = XLColor.AshGrey;
            worksheet.Cell(footerRow, footerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Right;
            worksheet.Cell(footerRow, footerColumn).Style.Font.Bold = true;
        }

        private int ExportDiff(int iRow, int iCol)
        {
            //headers
            worksheet.Cell(iRow, iCol).Value = "Differential Pairs";
            int row1 = iRow + 1, col1 = iCol;

            int headerRow = row1, headerColumn = col1;
            int ZDiffColumn = 0, ZDiffRow_1 = 0, ZDiffRow_2 = 0;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Layer #";
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Pair Name";
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Trace" + Environment.NewLine + "Width" + Environment.NewLine +"(mils)";
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Trace" + Environment.NewLine + "Separation" + Environment.NewLine + "(mils)"; ;
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Zo"+ Environment.NewLine + "(ohms)";
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;

            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Zdiff"+ Environment.NewLine + "(ohms)";
            InitHeaderCell(headerRow + 1, headerColumn);
            ZDiffColumn = headerColumn;
            headerColumn++;

            //Notes
            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "Comments";
            InitHeaderCell(headerRow + 1, headerColumn);
            headerColumn++;
            worksheet.Cell(headerRow, headerColumn).Value = "";
            InitHeaderCell(headerRow, headerColumn);
            worksheet.Cell(headerRow + 1, headerColumn).Value = "";
            InitHeaderCell(headerRow + 1, headerColumn);
            worksheet.Cell(headerRow, headerColumn + 1).Value = "";
            InitHeaderCell(headerRow, headerColumn + 1);
            worksheet.Cell(headerRow + 1, headerColumn + 1).Value = "";
            InitHeaderCell(headerRow + 1, headerColumn + 1);
            worksheet.Cell(headerRow, headerColumn + 2).Value = "";
            InitHeaderCell(headerRow, headerColumn + 2);
            worksheet.Cell(headerRow + 1, headerColumn + 2).Value = "";
            InitHeaderCell(headerRow + 1, headerColumn + 2);
            worksheet.Cell(headerRow, headerColumn + 3).Value = "";
            InitHeaderCell(headerRow, headerColumn + 3);
            worksheet.Cell(headerRow + 1, headerColumn + 3).Value = "";
            InitHeaderCell(headerRow + 1, headerColumn + 3);
            worksheet.Range(row1, headerColumn - 1, row1 + 1, headerColumn + 3).Merge();
            worksheet.Cell(row1, headerColumn - 1).Value = "Comments";
            worksheet.Cell(row1, headerColumn - 1).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
            worksheet.Cell(row1, headerColumn - 1).Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;

            for (int i = col1; i < headerColumn - 2; i++)
            {
                string s = worksheet.Cell(row1 + 1, i).Value.ToString();
                worksheet.Range(row1, i, row1 + 1, i).Merge();
                worksheet.Cell(row1, i).Value = s;

                worksheet.Cell(row1, i).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                worksheet.Cell(row1, i).Style.Alignment.Vertical = XLAlignmentVerticalValues.Center;
                worksheet.Cell(row1, i).Style.Alignment.WrapText = true;
            }
            worksheet.Range(row1, headerColumn - 3, row1, headerColumn - 2).Merge();
            worksheet.Cell(row1, headerColumn - 3).Value = "Impedance";


            //cell data
            int layerRow = row1 + 2, layerColumn = col1;
            ZDiffRow_2 = ZDiffRow_1 = layerRow;
            foreach (ZPair pair in z.Pairs)
            {
                string pName = pair.Title;
                foreach (ZLayer zDpLayer in pair.Layers)
                {
                    layerColumn = col1;

                    ZLayer zl = z.GetLayerOfStackup(zDpLayer.ID);
                    int ival = 0;
                    double width = 0, sep = 0, Z0 = 0, Zdiff = 0;
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDLayerNumber, ref ival);
                    ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_TraceWidth, ref width);
                    ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_TraceSpacing, ref sep);
                    ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_Zo, ref Z0);
                    ZPlannerProject.GetLayerParameterValue(zDpLayer, ZStringConstants.ParameterIDZdiff_Zdiff, ref Zdiff);

                    if (ival > 0 && width > 0)
                    {

                        //write layer data
                        worksheet.Cell(layerRow, layerColumn).Value = ival > 0 ? ival.ToString() : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;

                        worksheet.Cell(layerRow, layerColumn).Value = pName;
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Left;
                        layerColumn++;

                        string sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString());
                        worksheet.Cell(layerRow, layerColumn).Value = width > 0 ? width.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        worksheet.Cell(layerRow, layerColumn).Style.Fill.BackgroundColor = ConvertColor(ZColorConstants.PlaneLayerColor);
                        layerColumn++;

                        sval = String.Format("N" + Options.TheOptions.lengthDigits.ToString());
                        worksheet.Cell(layerRow, layerColumn).Value = sep > 0 ? sep.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;

                        sval = String.Format("N" + Options.TheOptions.impedanceDigits.ToString());
                        worksheet.Cell(layerRow, layerColumn).Value = Z0 > 0 ? Z0.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;

                        sval = String.Format("N" + Options.TheOptions.impedanceDigits.ToString());
                        worksheet.Cell(layerRow, layerColumn).Value = Zdiff > 0 ? Zdiff.ToString(sval) : "";
                        worksheet.Cell(layerRow, layerColumn).Style.Alignment.Horizontal = XLAlignmentHorizontalValues.Center;
                        layerColumn++;

                        worksheet.Range(layerRow, headerColumn - 1, layerRow, headerColumn + 3).Merge();

                        //
                        layerRow++;
                        ZDiffRow_2++;
                    }
                }

                var rngTable = worksheet.Range(row1, col1, layerRow - 1, col1 + 10);
                rngTable.Style.Border.TopBorder = XLBorderStyleValues.Thin;
                rngTable.Style.Border.LeftBorder = XLBorderStyleValues.Thin;
                rngTable.Style.Border.OutsideBorder = XLBorderStyleValues.Thin;
            }

            //adjust ZDiff to contents
            worksheet.Column(ZDiffColumn).AdjustToContents(ZDiffRow_1 - 1, ZDiffRow_2 - 1);
            
            return layerRow - iRow;
        }
    }
}