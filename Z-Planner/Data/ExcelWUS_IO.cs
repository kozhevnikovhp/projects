using System;
using System.IO;
using DocumentFormat.OpenXml.Packaging;
using DocumentFormat.OpenXml.Spreadsheet;
using System.Text;
//
using ClosedXML.Excel;
using Microsoft.Office.Interop.Excel;
//
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using System.Collections.Generic;
using ZZero.ZPlanner.UI;
using System.Windows.Forms;
using ZZero.ZPlanner.Settings;

using System.Linq;
using ZZero.ZPlanner.UI.Dialogs;
using System.Text.RegularExpressions;
using System.Collections.Specialized;

namespace ZZero.ZPlanner.Translators
{
    public class WUS_Reader : IImport
    {
        private string fileName;
        IXLWorksheet ws;
        private ZStackup stackup;
        string material;
        bool bDkDefined;

        private int rowFirstLayer;
        private int colWeight;
        private int rowFirstSingle;
        private int rowFirstDP;

        private Dictionary<int, int> mapSingle; //layer metal number to Single index
        private Dictionary<int, int> mapDP;     //layer metal number to DP index

        private int colLineWidth, colTarget, colImpedance, colRefLayer;

        private bool bPressedThickness;
        private ZLibraryCategory[] libraryPriorityArray;
        
        public WUS_Reader(string file, bool bPressed = false, ZLibraryCategory[] libraryPriority = null)
        {
            fileName = file;
            rowFirstLayer = -1;
            colWeight = -1;
            rowFirstSingle = -1;
            rowFirstDP = -1;
            material = "";
            bDkDefined = false;
            bPressedThickness = bPressed;
            libraryPriorityArray = libraryPriority;
            colLineWidth = colTarget = colImpedance = colRefLayer = -1;

             mapSingle = new Dictionary<int, int>();
             mapDP = new Dictionary<int, int>();
        }

        public bool Convert()
        {
            string origFile = fileName;
            bool retval = false;
            bool bConvert = Path.GetExtension(origFile) == ".xls";
            if (bConvert)
            {
                //1. convert xls to xlsx
                string retFile = "";

                try
                {
                    var application = new Microsoft.Office.Interop.Excel.Application();
                    var workbook = application.Workbooks.Open(origFile);
                    ZPlannerManager.StatusMenu.UpdateProgress();

                    string ext = ".xlsx";
                    XlFileFormat fmt = XlFileFormat.xlOpenXMLWorkbook;
                    if (workbook.HasVBProject)
                    {
                        ext = ".xlsm";
                        fmt = XlFileFormat.xlOpenXMLWorkbookMacroEnabled;
                    }

                    ZPlannerManager.StatusMenu.UpdateProgress();

                    retFile = Path.ChangeExtension(origFile, ext);

                    if (File.Exists(retFile))
                    {
                        File.Delete(retFile);
                    }
                    workbook.SaveAs(retFile, fmt);
                    ZPlannerManager.StatusMenu.UpdateProgress();
                    workbook.Close();
                    fileName = retFile;
                    retval = true;
                }
                catch (Exception e)
                {
                    //TODO: add warning dialog (no Office on PC or file is opened by another app)
                    //MessageBox.Show(e.Message);
                    retFile = "";
                    retval = false;
                    throw e;
                }
            }
            else
            {
                fileName = origFile;
                retval = true;
            }

            return retval;
        }

        public bool IsValidFile()
        {
            bool retval = false;
            XLWorkbook workbook = null;
            ZPlannerManager.StatusMenu.UpdateProgress();
            try
            {
                workbook = new XLWorkbook(fileName);
                ws = workbook.Worksheet(1);
                if (ws != null)
                {
                    //check WUS signature
                    string revision = ReadString(1, "A");
                    revision = revision.Trim();
                    string[] words = revision.Split(' ');
                    if (words[0].Trim() == "WUS")
                    {
                        retval = true;
                    }

                    //look for layers description title
                    bool bValid = false;
                    if (retval)
                    {
                        for (int i = 1; i < 20; i++) //20 is safe limit
                        {
                            string s = ReadString(i, "A");
                            if (s.Trim() == "DFM issues")
                            {
                                s = ReadString(i + 1, "A").Trim();
                                char[] del = { '(', ')' };
                                string[] y = s.Split(del);
                                y = y.Where(x => !string.IsNullOrEmpty(x)).ToArray();
                                string pattern = "1. Following is the";
                                if (String.Compare(y[0].Trim(), 0, pattern, 0, pattern.Length) == 0)
                                {
                                    rowFirstLayer = i + 2;
                                    material = y[1].Trim();

                                    bValid = true;
                                    break;
                                }
                            }
                        }
                        retval = bValid;
                    }
                }

                workbook.Dispose();
            }
            catch (Exception e)
            {
                //TODO: add warning dialog
                //MessageBox.Show(e.Message);
                if (workbook != null) workbook.Dispose();
                retval = false;
                throw e;
            }

            return retval;
        }

        private string ReadString(int iRow, int colNum)
        {
            string num = "";
            try
            {
                num = ws.Cell(iRow, colNum).Value.ToString();//GetValue<string>();
            }
            catch { }

            return num;
        }

        private string ReadString(int iRow, string colName)
        {
            string num = "";
            try
            {
                num = ws.Cell(iRow, colName).Value.ToString();//GetValue<string>();
            }
            catch { }

            return num;
        }

        private double ReadDouble(int iRow, int colNum)
        {
            double num = 0;
            string snum = ReadString(iRow, colNum);
            if (!Double.TryParse(snum, out num)) num = 0;

            return num;
        }

        private double ReadDouble(int iRow, string colName)
        {
            double num = 0;
            string snum = ReadString(iRow, colName);
            if (!Double.TryParse(snum, out num)) num = 0;

            return num;
        }

        private int ReadInt(int iRow, string colName)
        {
            int num = 0;
            string snum = ReadString(iRow, colName);
            if (!Int32.TryParse(snum, out num)) num = 0;

            return num;
        }


        public bool Import()
        {
            return ImportStackup();
        }

        public bool ImportStackup()
        {
            ZPlannerManager.StatusMenu.UpdateProgress();
            ZPlannerManager.SuspendFSHandlers();

            ZPlannerProject project = new ZPlannerProject();

            // create stackup
            stackup = new ZStackup(project);
            stackup.Title = Path.GetFileNameWithoutExtension(fileName);
            project.Parameters.Clear();
            project.SubParameters.Clear();

            foreach (SelectList defaultSelectList in project.GetDefaultSelectLists())
            {
                project.Lists.Add(defaultSelectList);
            }

            foreach (ZParameter parameter in project.GetDefaultParameters())
            {
                if (parameter.ID == ZStringConstants.ParameterIDFoilTreatment)
                    parameter.IsHidden = false;

                project.Parameters.Add(parameter);
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0) project.SubParameters.AddRange(parameter.SubParameters);
            }

            int iLastParameter = project.Parameters.Count - 1;
            int order = project.Parameters[iLastParameter].Order;

            ZParameter par = new ZParameter(ZStringConstants.ParameterIDPlaneType);
            par = new ZParameter(ZStringConstants.ParameterIDPlaneType);
            par.Title = "Plane Type";
            par.Description = "Specifies if plane is power or ground";
            par.Table = ZTableType.Stackup;
            par.ValueType = ZValueType.Text;
            par.IsReadOnly = true;
            par.IsCustom = true;
            par.Width = 70;
            par.Order = order++;
            project.Parameters.Add(par);

            par = new ZParameter(ZStringConstants.ParameterIDLayerTypeDummyCore);
            par.Title = "Dummy Core";
            par.Description = "Specifies if core is dummy";
            par.Table = ZTableType.Stackup;
            par.ValueType = ZValueType.Boolean;
            par.IsReadOnly = true;
            par.IsCustom = true;
            par.IsHidden = true;
            par.IsPrivate = true;
            par.Width = 70;
            par.Order = order++;
            project.Parameters.Add(par);

            project.Stackup = stackup;
            ZPlannerManager.Project = project;

            // parse file
            Translate(stackup);

            //CopperThicknessDialog dlg = new CopperThicknessDialog(stackup);
            //dlg.ShowDialog();
            ////if (dlg.DialogResult == DialogResult.OK) ;
            ZPlannerManager.Project.Stackup.CopperCoverageType = ZCopperCoverageType.ManuallyEntered;

            //ui
            ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);
            ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();

            ZPlannerManager.IsPressedThickness = bPressedThickness;
            stackup.IsPressedThickness = bPressedThickness;

            ZPlannerManager.ResumeFSHandlers();
            stackup.Singles.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            stackup.Pairs.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            stackup.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            return true;
        }

        private bool Translate(ZStackup stackup)
        {
            int iLastUsedRow = 0;
            string stackupName = "";
            try
            {
                var workbook = new XLWorkbook(fileName);
                ws = workbook.Worksheet(1);
                stackupName = ws.Name;
                iLastUsedRow = ws.LastRowUsed().RowNumber();
            }
            catch (Exception e)
            {
                MessageBox.Show(e.Message);
                return false;
            }

            ZPlannerManager.StatusMenu.UpdateProgress();

            ReadHeader();

            ZPlannerManager.StatusMenu.UpdateProgress();

            //find the last row
            int rowLastLayer = -1;
            for (int i = rowFirstLayer; i <= iLastUsedRow; i++)
            {
                string s = ReadString(i, "A");
                s = s.Trim();
                if (s == "WUS stack-up")
                {
                    rowLastLayer = i - 1;
                    break;
                }
            }
            if (rowLastLayer < 0) return false;

            ZPlannerManager.StatusMenu.UpdateProgress();

            // try column H for Dk
            string DkTitle = ReadString(rowFirstLayer, "H");
            if (String.Compare(DkTitle, 0, "Dk", 0, 2) == 0)
            {//has Dk,Df definition
                double Frequency = 1;//GHz
                char[] del = { '@', 'G' };
                string[] y = DkTitle.Split(del);
                y = y.Where(x => !string.IsNullOrEmpty(x)).ToArray();
                if (y.Length > 2)
                {
                    Frequency = Double.Parse(y[1]);
                    bDkDefined = true;
                }
                //string DfTitle = ReadString(rowFirstLayer, "I");
            }

            ZPlannerManager.StatusMenu.UpdateProgress();

            //find metal weight column
            for (int col = 10; col <= 12; col++) //from "J" to "L"
            {
                string spec = ReadString(rowFirstLayer - 1, col);
                if (spec.Trim() == "Customer's spec:")
                {
                    colWeight = col + 1;
                    break;
                }
            }

            for (int i = rowFirstLayer; i <= rowLastLayer; i++)
            {
                ReadLayer(i, stackup);
            }

            ZPlannerManager.StatusMenu.UpdateProgress();

            // Single-ended       
            int rowLastSingle = -1;
            for (int r = rowLastLayer + 1; r < iLastUsedRow; r++)
            {
                string val = ReadString(r, "A");
                if (val == "1) single-ended  impedance")
                {
                    rowFirstSingle = r + 3;
                    break;
                }
            }
            if (rowFirstSingle > 0)
            {
                for (int r = rowFirstSingle + 1; r <= iLastUsedRow + 1; r++)
                {
                    string val = ReadString(r, "A");
                    int n = -1;
                    if (val.Length == 0 || !Int32.TryParse(val, out n))
                    //if (val.Length == 0)
                    {
                        rowLastSingle = r - 1;
                        break;
                    }
                }
            }
            if (rowLastSingle > 0)
            {
                IXLRange range = ws.Range(rowFirstSingle - 2, 7, rowFirstSingle - 1, 20);
                IXLCells res = range.CellsUsed(cell => cell.GetValue<string>() == "Reference layer");
                foreach (IXLCell cell in res)
                {
                    //int _row = cell.WorksheetRow().RowNumber();
                    colRefLayer = cell.WorksheetColumn().ColumnNumber();
                }

                //find columns
                for (int col = 7; col < 20; col++ ) //from "G"
                {
                    string val = ReadString(rowFirstSingle - 1, col).Trim();
                    switch (val)
                    {
                        case "Line width(mil)":
                            colLineWidth = col;
                            break;
                        case "Impedance(ohm)":
                            colTarget = col;
                            break;
                        case "Cal impedance(ohm)":
                            colImpedance = col;
                            break;
                    }
                }

                if (colLineWidth > 0 && colTarget > 0 && colImpedance > 0 && colRefLayer > 0){
                    for (int i = rowFirstSingle; i <= rowLastSingle; i++)
                    {
                        ReadSingleEnded(i, stackup);
                    }
                    //remove ActiveSingle
                    if (stackup.Singles.Count > 1)
                    {
                        stackup.Singles.RemoveAt(0);
                        stackup.ActiveSingle = stackup.Singles[0];
                    }

                }
            }

            ZPlannerManager.StatusMenu.UpdateProgress();
            
            //diferential pairs
            int rowLastDP = iLastUsedRow;
            for (int r = rowLastSingle + 1; r < iLastUsedRow; r++)
            {
                string val = ReadString(r, "A");
                if (val == "2)differential impedance")
                {
                    rowFirstDP = r + 4;
                    break;
                }
            }
            if (rowFirstDP > 0)
            {
                for (int r = rowFirstDP + 1; r < iLastUsedRow; r++)
                {
                    string val = ReadString(r, "A");
                    int n = -1;
                    if (val.Length == 0 || !Int32.TryParse(val, out n))
                    //if (val.Length == 0)
                    {
                        rowLastDP = r - 1;
                        break;
                    }
                }
            }

            if (colLineWidth > 0 && colTarget > 0 && colImpedance > 0 && colRefLayer > 0 && rowFirstDP > 0)
            {
                for (int i = rowFirstDP; i <= rowLastDP; i++)
                {
                    ReadDP(i, stackup);
                }
                //remove ActivePair
                if (stackup.Pairs.Count > 1)
                {
                    stackup.Pairs.RemoveAt(0);
                    stackup.ActivePair = stackup.Pairs[0];
                }
            }

            ZPlannerManager.StatusMenu.UpdateProgress();

            bool bMatch = MatchMaterials();

            ZPlannerManager.StatusMenu.UpdateProgress();

            //if (bPressedThickness)
            //{
            //    stackup.RestoreCopperPercent();
            //}

            UpdateDkDf();

            ZPlannerManager.StatusMenu.UpdateProgress();

            UpdateUsed();

            SortImpedances();

            ZPlannerManager.StatusMenu.UpdateProgress();

            return true;
        }

        private void SortImpedances()
        {
            stackup.Singles.Sort((x, y) => x.ImpedanceTarget.CompareTo(y.ImpedanceTarget));
            stackup.Pairs.Sort((x, y) => x.ImpedanceTarget.CompareTo(y.ImpedanceTarget));

            //rename impedances to force correct order by Title
            char[] sep = { '~', ' ' };
            foreach (ZSingle s in stackup.Singles)
            {
                string[] x = s.Title.Split(sep);
                double z;
                if ((x.Length == 3) && Double.TryParse(x[1], out z) && (z < 100)) s.Title = x[0] + "~ " + z.ToString() + " ohms";
            }
            foreach (ZPair s in stackup.Pairs)
            {
                string[] x = s.Title.Split(sep);
                double z;
                if ((x.Length == 3) && Double.TryParse(x[1], out z) && (z < 100)) s.Title = x[0] + "~ " + z.ToString() + " ohms";
            }
        }

        public class MaterialData
        {
            public string ID { get; set; }
            public string H { get; set; }
            public ZLibraryCategory Category { get; set; }
        }


        internal static bool CompareResin(ZMaterial m, double value, double threshold)
        {
            try
            {
                string s = m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDResin).Value.TrimEnd('%');
                double a = System.Convert.ToDouble(s);
                return Math.Abs(a - value) <= threshold;
            }
            catch
            {
                return false;
            }
        }

        internal static bool CompareMaterial(ZMaterial m, string mName)
        {
            string materialName = m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDMaterial).Value;
            materialName = Regex.Replace(materialName, @"[- ]", String.Empty);
            materialName = materialName.ToUpper();
            mName = Regex.Replace(mName, @"[- ]", String.Empty);
            mName = mName.ToUpper();

            return mName == materialName;
        }

        internal static ZLibraryCategory GetLibraryCategory(string sCat)
        {
            ZLibraryCategory z = ZLibraryCategory.ZZero;
            if (ZMaterial.LibraryCategoryDictionary.ContainsKey(sCat))
            {
                z = ZMaterial.LibraryCategoryDictionary[sCat];
            }
            return z;
        }

        bool MatchMaterials()
        {
            bool retval = true;
            foreach (ZLayer zl in stackup.Layers)
            {
                switch (zl.GetLayerType())
                {
                    case ZLayerType.Prepreg:
                        {
                            if (bPressedThickness)
                            {
                                //ParameterIDThickness has pressed thickness
                                //ParameterIDPrepregThickness should have it
                                //Unpressed thickness should go to ParameterIDOrigThickness and ParameterIDThickness

                                //look for material in DML and get thickness from there
                                double pressedThickness = 0;
                                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref pressedThickness);
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, pressedThickness);
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDFabricatorThickness, pressedThickness);
                            }
                            DMLMatch match = new DMLMatch(libraryPriorityArray);
                            ZMaterial m;
                            double th = 0;
                            if (match.MatchMaterial(zl, out m))
                            {
                                List<ZLayer> l = new List<ZLayer>();
                                l.Add(zl);
                                stackup.AssignMaterialToStackup(m, l);
                                Double.TryParse(m.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value, out th);
                            }
                            else
                            {
                                DMLMatch.MatchDkDf(zl);
                                retval = false;
                                if (bPressedThickness)
                                {
                                    th = stackup.RestorePrepregUnpressedThickness(zl);
                                }
                            }
                            if (th > 0)
                            {
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDOriginThickness, th);
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, th);
                            }
                        }
                        break;
                    case ZLayerType.Core:
                        {
                            DMLMatch match = new DMLMatch(libraryPriorityArray, true);
                            ZMaterial m;
                            if (match.MatchMaterial(zl, out m))
                            {
                                List<ZLayer> l = new List<ZLayer>();
                                l.Add(zl);
                                stackup.AssignMaterialToStackup(m, l);
                            }
                            else
                            {
                                DMLMatch.MatchDkDf(zl);
                                retval = false;
                            }
                        }
                        break;
                }
            }
            return retval;
        }


        bool RestoreUnpressedThickness()
        {
            bool matched = true;
            foreach (ZLayer zl in stackup.Layers)
            {
                if (zl.GetLayerType() == ZLayerType.Prepreg)
                {
                    //ParameterIDThickness has pressed thickness
                    //ParameterIDPrepregThickness should have it
                    //Unpressed thickness should go to ParameterIDOrigThickness and ParameterIDThickness


                    //look for material in DML and get thickness from there
                    double pressedThickness = 0;
                    string construction = "";
                    double resin = 0;
                    string materialName = "";
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref pressedThickness);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, pressedThickness);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDFabricatorThickness, pressedThickness);

                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDConstruction, ref construction);
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDResinContent, ref resin);
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDMaterial, ref materialName);

                    List<MaterialData> mData;
                    mData = (from material in ZPlannerManager.Dml.Materials
                             where (
                                    (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDType).Value == "Prepreg") &&
                                    CompareMaterial(material, materialName) &&
                                    (material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDNormalizedConstruction).Value == construction) &&
                                    (libraryPriorityArray.Contains(GetLibraryCategory(material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDCategory).Value))) &&
                                    CompareResin(material, resin, 0.01)
                                    )
                             select new MaterialData
                             {
                                 ID = material.ID,
                                 H = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDH).Value,//GetValue(material, ZStringConstants.DMLParameterIDH)//
                             }).ToList();

                    double th = 0;
                    if (mData.Count > 0 && Double.TryParse(mData[0].H, out th))
                    {
                        mData.Sort(
                            delegate(MaterialData m1, MaterialData m2)
                            {
                                int retval = 0;
                                int p1 = Array.IndexOf(libraryPriorityArray, m1.Category);
                                int p2 = Array.IndexOf(libraryPriorityArray, m2.Category);
                                if (p1 > p2) retval = 1;
                                if (p1 < p2) retval = -1;

                                return retval;
                            }
                        );
                        ZMaterial m = ZPlannerManager.Dml.Materials.Find(x => x.ID == mData[0].ID);
                        List<ZLayer> l = new List<ZLayer>();
                        l.Add(zl);
                        stackup.AssignMaterialToStackup(m, l);
                        //---ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, "Unpressed thicknesses from Library.");
                    }
                    else
                    {
                        th = stackup.RestorePrepregUnpressedThickness(zl);
                        matched = false;
                    }

                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDOriginThickness, th);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, th);
                }
            }

            return matched;
        }


        void UpdateUsed()
        {
            foreach (ZSingle single in stackup.Singles)
            {
                foreach (ZLayer singleLayer in single.Layers)
                {
                    switch (singleLayer.GetLayerType())
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            string Zo = singleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Zo);
                            if (!string.IsNullOrEmpty(Zo))
                            {
                                singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_IsUsed, "true");
                            }
                            else
                            {
                                singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_IsUsed, "false");
                            }
                            break;
                    }
                }
            }

            foreach (ZPair pair in stackup.Pairs)
            {
                foreach (ZLayer pairLayer in pair.Layers)
                {
                    switch (pairLayer.GetLayerType())
                    {
                        case ZLayerType.Signal:
                        case ZLayerType.SplitMixed:
                            //ZLayer baseLayer = stackup.GetLayerOfStackup(pairLayer.ID);
                            //string aname = baseLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName);
                            string Zo = pairLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Zdiff);
                            if (!string.IsNullOrEmpty(Zo))
                            {
                                pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed, "true");
                            }
                            else
                            {
                                pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed, "false");
                            }
                            break;
                    }
                }
            }

        }


        private void ReadHeader()
        {
            ExportOptions.TheOptions.Fabricator = "WUS PRINTED CIRCUIT";
            for (int row = 1; row < rowFirstLayer; row++)
            {
                string str = ReadString(row, "A").Trim();
                if (str.Length > 0)
                {
                    string[] words = str.Split(':');
                    if (words.Length > 1)
                    {
                        string key = words[0].Trim();
                        string val = words[1].Trim();
                        if (key.Length > 0 && val.Length > 0)
                        {
                            switch (key)
                            {
                                case "Part Number":
                                    ExportOptions.TheOptions.ProjectName = val;
                                    break;
                                case "Prepared by":
                                    ExportOptions.TheOptions.FabContact = val;
                                    break;
                            }
                        }
                    }
                }
            }
        }

        private string GetUniqueName(string value, bool bSingle)
        {
            string retval = "";
            if (bSingle)
            {
                string prefix = "Zo~" + value + " ohms";
                ZSingle foundItem = null;
                int cnt = 0;
                string name = prefix;
                while (true)
                {
                    foundItem = stackup.Singles.FirstOrDefault(i => i.Title == name);
                    if (foundItem == null)
                    {
                        retval = name;
                        break;
                    }
                    else
                    {
                        cnt++;
                        name = prefix + "(" + cnt.ToString() + ")";
                    }
                }
            }
            else
            {
                string prefix = "Zdiff~" + value + " ohms";
                ZPair foundItem = null;
                int cnt = 0;
                string name = prefix;
                while (true)
                {
                    foundItem = stackup.Pairs.FirstOrDefault(i => i.Title == name);
                    if (foundItem == null)
                    {
                        retval = name;
                        break;
                    }
                    else
                    {
                        cnt++;
                        name = prefix + "(" + cnt.ToString() + ")";
                    }
                }
            }

            return retval;
        }

        ZSingle GetSingle(int metalNumber, string target, out ZLayer singleLayer)
        {
            ZLayer baseLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == metalNumber.ToString());

            singleLayer = null;
            //find single with *target* in the name where layer *metalNumber* is not used (doesn't have Ztarget set)
            //Zo-<value> ohms[(<N>)]"
            foreach (ZSingle sing in stackup.Singles)
            {
                char [] sep = {'~',' '};
                string[] x = sing.Title.Split(sep);
                if (x.Length != 3) continue;
                if (x[1] != target) continue;

                ZLayer zSingleLayer = sing.GetLayerOfSingleImpedance(baseLayer.ID);
                string v = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Target);
                if (v.Length == 0)
                {
                    singleLayer = zSingleLayer;
                    return sing;
                }
            }

            return null;
        }

        ZPair GetPair(int metalNumber, string target, out ZLayer pairLayer)
        {
            ZLayer baseLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == metalNumber.ToString());

            pairLayer = null;
            //find single with *target* in the name where layer *metalNumber* is not used (doesn't have Ztarget set)
            //Zdiff-<target> ohms
            foreach (ZPair p in stackup.Pairs)
            {
                char[] sep = { '~', ' ' };
                string[] x = p.Title.Split(sep);
                if (x.Length != 3) continue;
                if (x[1] != target) continue;

                ZLayer pLayer = p.GetLayerOfPairImpedance(baseLayer.ID);
                string v = pLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Target);
                if (v.Length == 0)
                {
                    pairLayer = pLayer;
                    return p;
                }
            }

            return null;
        }

        private void ReadSingleEnded(int row, ZStackup stackup)
        {
            // colLineWidth, colTarget, colImpedance, colRefLayer;
            int nLayer = ReadInt(row,"A");
            double width = ReadDouble(row, colLineWidth);
            double Zo = ReadDouble(row, colImpedance);
            string refLayers = ReadString(row, colRefLayer).Trim();
            string [] refs = refLayers.Split('&');
            int ref1 = -1;
            if (refs.Length > 0) Int32.TryParse(refs[0], out ref1);
            int ref2 = -1;
            if (refs.Length > 1) Int32.TryParse(refs[1], out ref2);
            string Zt = ReadString(row, colTarget);
            string target = "";
            bool bValid = false;
            if (Zt.Length > 0)
            {
                string[] x = Zt.Split('+');
                if (x.Length > 0)
                {
                    target = x[0];
                    bValid = true;
                }
            }
            if (!bValid) return;

            //find appropriate ZSingle object for the given target
            ZLayer zSingleLayer;
            ZSingle Z = GetSingle(nLayer, target, out zSingleLayer);
            if (Z == null)
            {
                Z = new ZSingle(stackup);
                stackup.Singles.Add(Z);
                string t = GetUniqueName(target, true);
                if (t.Length > 0) Z.Title = t;

                double imp;
                if (Double.TryParse(target, out imp)){
                    Z.ImpedanceTarget = imp;
                }

                ZLayer baseLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == nLayer.ToString());
                zSingleLayer = Z.GetLayerOfSingleImpedance(baseLayer.ID);
            }

            ZLayerParameter layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_TraceWidth);
            if (layerParameter != null) layerParameter.SetEditedValue(width.ToString());
            layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_Zo);
            if (layerParameter != null) layerParameter.SetEditedValue(Zo.ToString());
            layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_Target);
            if (layerParameter != null) layerParameter.SetEditedValue(Zo.ToString());

            if (ref1 > 0)
            {
                if (ref1 < nLayer)
                {
                    layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
                else
                {
                    layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
            }
            if (ref2 > 0)
            {
                if (ref2 < nLayer)
                {
                    layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
                else
                {
                    layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
            }


        }
        
        private void ReadDP(int row, ZStackup stackup)
        {
            // colLineWidth, colTarget, colImpedance, colRefLayer;
            int nLayer = ReadInt(row, "A");
            string widthString = ReadString(row, colLineWidth);
            string[] w = widthString.Split('/');
            double w1 = w.Length > 0 ? Double.Parse(w[0]) : 0;
            double s = w.Length > 1 ? Double.Parse(w[1]) : 0;
            double w2 = w.Length > 2 ? Double.Parse(w[2]) : 0;

            double Zd = ReadDouble(row, colImpedance);
            string refLayers = ReadString(row, colRefLayer);
            string[] refs = refLayers.Split('&');
            int ref1 = refs.Length > 0 ? Int32.Parse(refs[0]) : -1;
            int ref2 = refs.Length > 1 ? Int32.Parse(refs[1]) : -1;
            string Zt = ReadString(row, colTarget);
            string target = "";
            if (Zt.Length > 0)
            {
                string[] x = Zt.Split('+');
                if (x.Length > 0)
                {
                    target = x[0];
                }
            }

            //find appropriate ZSingle object for the given target
            ZLayer zDiffLayer;
            ZPair Z = GetPair(nLayer, target, out zDiffLayer);
            if (Z == null)
            {
                Z = new ZPair(stackup);
                stackup.Pairs.Add(Z);
                string t = GetUniqueName(target, false);
                if (t.Length > 0) Z.Title = t;

                double imp;
                if (Double.TryParse(target, out imp)){
                    Z.ImpedanceTarget = imp;
                }

                ZLayer baseLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == nLayer.ToString());
                zDiffLayer = Z.GetLayerOfPairImpedance(baseLayer.ID);
            }

            ZLayerParameter layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
            if (layerParameter != null) layerParameter.SetEditedValue(w1.ToString());
            layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
            if (layerParameter != null) layerParameter.SetEditedValue(s.ToString());
            layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
            if (layerParameter != null) layerParameter.SetEditedValue(Zd.ToString());
            layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Target);
            if (layerParameter != null) layerParameter.SetEditedValue(Zd.ToString());

            if (ref1 > 0)
            {
                if (ref1 < nLayer)
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
                else
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
            }
            if (ref2 > 0)
            {
                if (ref2 < nLayer)
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
                else
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
            }

            /*
            ZPair pair = null;
            if (!mapDP.ContainsKey(nLayer))
            {
                mapDP[nLayer] = 0; // stackup.ActiveSingle;
                pair = stackup.ActivePair;
            }
            else
            {
                //next single for nLayer should be at index mapSingle[nLayer] + 1
                int index = mapDP[nLayer] + 1;
                //check if exists
                if (stackup.Pairs.Count > index)
                {
                    pair = stackup.Pairs[index];
                }
                else
                {
                    //create new
                    pair = new ZPair(stackup);
                    stackup.Pairs.Add(pair);
                    //here stackup.Pairs.Count should be = index
                }
                mapDP[nLayer] = index;
            }

            if (target.Length > 0)
            {
                if (!pair.Title.Contains("ohms"))
                {
                    string t = GetUniqueName(target, false);
                    if (t.Length > 0) pair.Title = t;

                    //pair.Title = "Zdiff~" + target + " ohms";
                }
            }

            ZLayer baseLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == nLayer.ToString());
            ZLayer zDiffLayer = pair.GetLayerOfPairImpedance(baseLayer.ID);
            ZLayerParameter layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
            if (layerParameter != null) layerParameter.SetEditedValue(w1.ToString());
            layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
            if (layerParameter != null) layerParameter.SetEditedValue(s.ToString());
            layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
            if (layerParameter != null) layerParameter.SetEditedValue(Zd.ToString());

            if (ref1 > 0)
            {
                if (ref1 < nLayer)
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
                else
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
            }
            if (ref2 > 0)
            {
                if (ref2 < nLayer)
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
                else
                {
                    layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
            }
 */ 
        }

        private void MetalConstruction(string s, out string foilTreatment, out bool bPlating, out double platingThickness)
        {
            platingThickness = 0;
            string a = s.Trim();
            string[] words = a.Split(' ');
            words = words.Where(x => !string.IsNullOrEmpty(x)).ToArray();

            foilTreatment = words.Length > 1 ? words[1] : "";
            string plating = words.Length > 2 ? words[2] : "";
            bPlating = false;
            if (plating.Length > 0)
            {
                string[] y = plating.Split('=');
                if (y.Length == 2)
                {
                    string thick = y[1];
                    string[] val = thick.Split('m');
                    if (val.Length == 2)
                    {
                        if (Double.TryParse(val[0], out platingThickness))
                        {
                            bPlating = true;
                        }
                    }
                }
            }
        }

        private bool ParseOneDielectricConstruction(string s, out string constr, out double resin, out double dummyH, ref int plies)
        {
            bool retval = true;

            constr = "";
            resin = 0;
            plies = 1;
            dummyH = 0;

            char[] del = { '(', ')' };
            string[] z = s.Split(del);
            z = z.Where(x => !string.IsNullOrEmpty(x)).ToArray();
            string rc = "";
            string ply = "";
            switch (z.Length)
            {
                case 1:
                    {
                        string[] u = z[0].Split('X');
                        constr = u[0];
                        ply = u[1];
                    }
                    break;
                case 3:
                    if (z[2].ToLower() == "core")
                    {
                        //dummy core
                        //thickness
                        char[] digits = {'0','1', '2','3','4','5','6','7','8','9','0','.'};
                        string coreH = new string(z[0].TakeWhile(a => digits.Contains(a)).ToArray());
                        double cH = 0;
                        retval = Double.TryParse(coreH, out cH);

                        //construction
                        if (retval)
                        {
                            dummyH = cH; //just to mark dummy core case

                            char[] d = { 'x', 'X' };
                            string[] con = z[1].Split(d);
                            if (con.Length == 2)
                            {
                                constr = con[0];
                                ply = con[1];
                            }
                            else retval = false;
                        }
                    }
                    else
                    {
                        constr = z[0];
                        rc = z[1];
                        ply = z[2];
                    }
                    break;
            }

            //resin
            if (rc.Length > 0)
            {
                string percent = rc.Substring(2).TrimEnd('%');
                if (!Double.TryParse(percent, out resin)) retval = false;
            }

            //plies
            if (ply.Length > 0)
            {
                Int32.TryParse(ply.TrimStart('X'), out plies);
            }

            return retval;
        }

        class Construction
        {
            public string constr;
            public double resin;
            public int plies;
            public double dk;
            public double df;
            public bool dummyCore;
            public double dummyCoreH;

            public Construction(string c, double r, int p, double dummyH = 0)
            {
                constr = c;
                resin = r;
                plies = p;
                dummyCore = dummyH > 0;
                dummyCoreH = dummyH;

                dk = 0;
                df = 0;
            }
        }

        private void DielectricConstruction(string s, out List<Construction> conList)
        {
            conList = new List<Construction>();
            //split into separate constructions
            string[] z = s.Split('+');
            foreach (string a in z)
            {
                string constr;
                double resin;
                double dummyH;
                int plies = 1;
                if (!ParseOneDielectricConstruction(a, out constr, out resin, out dummyH, ref plies))
                {
                    constr = "Undefined construction";
                    plies = 1;
                    resin = 50;
                }

                if (dummyH > 0)
                {
                    conList.Add(new Construction(constr, 0, plies, dummyH));
                }
                else
                {
                    conList.Add(new Construction(constr, resin, plies));
                }
            }            
        }

        private void DefineDk(string s, ref List<Construction> conList)
        {
            char[] del = {'\n' };
            string[] z = s.Split(del);
            z = z.Where(x => !string.IsNullOrEmpty(x)).ToArray();

            if (z.Length == 1)
            {
                double res = 0;
                if (Double.TryParse(z[0], out res))
                {
                    conList[0].dk = res;
                }
            }
            else
            {
                int j = 0;
                foreach (string a in z)
                {
                    string[] y = a.Split(':');
                    if (y.Length > 1)
                    {
                        //by rc
                        string rc = y[0].Substring(2).Trim('%');
                        foreach (Construction C in conList)
                        {
                            if (C.resin.ToString() == rc)
                            {
                                double res = 0;
                                if (Double.TryParse(y[1].Trim(), out res))
                                {
                                    C.dk = res;
                                    break;
                                }
                            }
                        }
                    }
                    else //by order
                    {
                        double res = 0;
                        if (Double.TryParse(y[0], out res))
                        {
                            conList[j].dk = res;
                        }
                    }

                    j++;
                }
            }
        }

        private void DefineDf(string s, ref List<Construction> conList)
        {
            char[] del = {'\n' };
            string[] z = s.Split(del);
            z = z.Where(x => !string.IsNullOrEmpty(x)).ToArray();

            if (z.Length == 1)
            {
                double res = 0;
                if (Double.TryParse(z[0], out res))
                {
                    conList[0].df = res;
                }
            }
            else
            {
                int j = 0;
                foreach (string a in z)
                {
                    double res = 0;
                    if (Double.TryParse(a, out res))
                    {
                        conList[j].df = res;
                    }
                    j++;
                }
            }
        }

        private void ReadLayer(int row, ZStackup stackup)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            string sNumber = "";
            int metalNumber = 0;
            double thickness = 0;
            string construction = ""; 
            string weight = "";
            string dielType = "";
            string resinContent = "";
            string Dk = "";
            string Df = "";

            sNumber = ReadString(row, "A");

            ZLayerType zType = ZLayerType.Signal;
            //coulumns A-G are stable
            if (sNumber.Length > 0)
            {//metal
                metalNumber = ReadInt(row, "A");
                string sType = ReadString(row, "B");
                switch (sType)
                {
                    case "S":
                        zType = ZLayerType.Signal;
                        break;
                    case "G":
                        zType = ZLayerType.Plane;
                        parMap.Add(ZStringConstants.ParameterIDPlaneType, ZStringConstants.LayerTypePlaneGround);
                        break;
                    case "P":
                        zType = ZLayerType.Plane;
                        parMap.Add(ZStringConstants.ParameterIDPlaneType, ZStringConstants.LayerTypePlanePower);
                        break;
                }

                thickness = ReadDouble(row, "D");
                construction = ReadString(row, "E");
                
                bool bPlating;
                string foilTreatment;
                double platingThickness = -1;//sum of layer + plating
                MetalConstruction(construction, out foilTreatment, out bPlating, out platingThickness);
                if (foilTreatment.Length > 0)
                {
                    if (foilTreatment == "STD") foilTreatment = "HTE";
                    if (foilTreatment == "H-VLP") foilTreatment = "HVLP";
                    parMap.Add(ZStringConstants.ParameterIDFoilTreatment, ZPlannerManager.GetCopperFoilString(foilTreatment));
                }

                if (colWeight > 0) weight = ReadString(row,colWeight);

                string sPlating = "Includes plating";
                double h = thickness;
                if (thickness > 0)
                {
                    if (platingThickness > 0)
                    {
                        sPlating += String.Format(" ({0} + {1})", h, platingThickness - thickness);
                        //thickness += platingThickness;
                        thickness = platingThickness;
                    }
                    double val = thickness;
                    parMap.Add(ZStringConstants.ParameterIDThickness, val.ToString());

                    //sPlating += " " + platingThickness.ToString() + " mil";
                }
                else
                {
                    sPlating += ". Plating thickness unknown.";
                }

                if (weight.Length > 0)
                {
                    //check if weight is relevant to thickness for signal+plating
                    if (bPlating)
                    {
                        double w = 0;
                        if (Double.TryParse(weight, out w) && (thickness > 0))
                        {
                            if (Math.Abs(thickness - w * 1.2) > 0.1)
                            {
                                //---sPlating += ". Plating weight unknown.";
                                //w = newThick / 1.2;
                                //weight = w.ToString("N1");
                            }
                        }
                    }
                    parMap.Add(ZStringConstants.ParameterIDCopperThickness, weight);
                }

                if (bPlating)
                {
                    parMap.Add(ZStringConstants.ParameterIDComments, sPlating);
                }
                
                string name = "";
                switch (zType)
                {
                    case ZLayerType.Signal: name = "Signal_";
                        break;
                    case ZLayerType.Plane: name = "Plane_";
                        break;
                    case ZLayerType.SplitMixed: name = "Split_";
                        break;
                }
                name += metalNumber.ToString();
                parMap.Add(ZStringConstants.ParameterIDLayerName, name);

                string layerID = stackup.AddLayer(zType);
                ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

                foreach (KeyValuePair<string, string> kvp in parMap)
                {
                    ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                    if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                }
            }            
            else
            {//dielectric
                thickness = ReadDouble(row,"B");
                dielType = ReadString(row,"C").Trim();
                construction = ReadString(row,"E");
                resinContent = ReadString(row,"G");


                ZLayerType zDielType = ZLayerType.Prepreg;
                string[] y = dielType.Split(' ');
                if (y.Length > 1)
                {
                    switch (y[1].Trim())
                    {
                        case "core": 
                            zDielType = ZLayerType.Core;
                            break;
                        case "B-stage":
                            zDielType = ZLayerType.Prepreg;
                            break;
                    }
                }

                double coreResin = -1;
                if ((zDielType == ZLayerType.Core) && resinContent.Length > 0)
                {
                    Double.TryParse(resinContent.TrimEnd('%'), out coreResin);
                }
                double pregResin = -1;
                if ((zDielType == ZLayerType.Prepreg) && resinContent.Length > 0)
                {
                    string[] r = resinContent.Split(':');
                    if (r.Length == 2)
                    {
                        Double.TryParse(r[1].TrimEnd('%'), out pregResin);
                    }
                }

                List<Construction> conList = null;
                int totalPlies = 0;
                double dummyCoreH = 0;
                if (construction.Length > 0)
                {
                    DielectricConstruction(construction, out conList);

                    foreach (Construction C in conList)
                    {
                        if (zDielType == ZLayerType.Core)
                        {
                            if (C.plies > 1)
                            {
                                C.constr = C.plies.ToString() + "x" + C.constr;
                                C.plies = 1;
                            }
                        }
                        if (zDielType == ZLayerType.Prepreg && C.dummyCore)
                        {
                            C.constr = C.plies.ToString() + "x" + C.constr;
                            C.plies = 1;
                            C.resin = pregResin;
                            dummyCoreH += C.dummyCoreH;
                            continue; //don't add dummy core plies
                        }
                        totalPlies += C.plies;
                    }
                }

                if (thickness > 0)
                {//recalculate for nPlies
                    thickness -= dummyCoreH;
                    if (totalPlies > 1) thickness /= totalPlies;
                    parMap.Add(ZStringConstants.ParameterIDThickness, thickness.ToString());
                }

                if (bDkDefined)
                {
                    Dk = ReadString(row, "H");
                    if (Dk.Length > 0)
                    {
                        DefineDk(Dk, ref conList);
                    }
                    Df = ReadString(row, "I");
                    if (Df.Length > 0)
                    {
                        DefineDf(Df, ref conList);
                    }
                }

                if (material.Length > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDMaterial, material);
                }

                foreach (Construction C in conList)
                {
                    int nPlies = C.plies;
                    for (int i = 1; i <= nPlies; i++)
                    {
                        ZLayerType layerType = zDielType;
                        if (C.dummyCore) layerType = ZLayerType.Core;

                        string layerID = stackup.AddLayer(layerType);
                        ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

                        foreach (KeyValuePair<string, string> kvp in parMap)
                        {
                            ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                            if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                        }

                        //rewrite thickness for dummy core
                        if (C.dummyCore)
                        {
                            ZLayerParameter layerParameter = zLayer.GetLayerParameter(ZStringConstants.ParameterIDLayerTypeDummyCore);
                            if (layerParameter != null) layerParameter.SetEditedValue(C.dummyCore.ToString());

                            layerParameter = zLayer.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                            if (layerParameter != null) layerParameter.SetEditedValue(C.dummyCoreH.ToString());
                        }

                        //add Construction parameters
                        if (C.constr.Length > 0)
                        {
                            zLayer.SetLayerParameterValue(ZStringConstants.ParameterIDConstruction, C.constr);
                        }
                        if (C.resin > 0)
                        {
                            zLayer.SetLayerParameterValue(ZStringConstants.ParameterIDResinContent, C.resin.ToString());
                        }
                        else
                        {
                            if ((zDielType == ZLayerType.Core) && (coreResin > 0))
                            {
                                zLayer.SetLayerParameterValue(ZStringConstants.ParameterIDResinContent, coreResin.ToString());
                            }
                        }
                        if (C.dk > 0)
                        {
                            zLayer.SetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant, C.dk.ToString());
                        }
                        if (C.df > 0)
                        {
                            zLayer.SetLayerParameterValue(ZStringConstants.ParameterIDLossTangent, C.df.ToString());

                        }
                    }
                }
            }
        }

        private void UpdateDkDf()
        {
            foreach (ZSingle single in stackup.Singles)
            {
                foreach (ZLayer singleLayer in single.Layers)
                {
                    if (!singleLayer.isMetal())
                    {
                        ZLayer layer = stackup.GetLayerOfStackup(singleLayer.ID);
                        string sDk = layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                        string sDf = layer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);

                        if (!string.IsNullOrEmpty(sDk))
                        {
                            singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant, sDk);
                        }
                        if (!string.IsNullOrEmpty(sDf))
                        {
                            singleLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent, sDf);
                        }
                    }
                }
            }
            foreach (ZPair pair in stackup.Pairs)
            {
                foreach (ZLayer pairLayer in pair.Layers)
                {
                    if (!pairLayer.isMetal())
                    {
                        ZLayer layer = stackup.GetLayerOfStackup(pairLayer.ID);
                        string sDk = layer.GetLayerParameterValue(ZStringConstants.ParameterIDDielectricConstant);
                        string sDf = layer.GetLayerParameterValue(ZStringConstants.ParameterIDLossTangent);

                        if (!string.IsNullOrEmpty(sDk))
                        {
                            pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant, sDk);
                        }
                        if (!string.IsNullOrEmpty(sDf))
                        {
                            pairLayer.SetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent, sDf);
                        }
                    }
                }
            }
        }


    }
}