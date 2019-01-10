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
    public class ISU_Reader : IImport
    {
        private string fileName;
        private IXLWorksheet ws;
        private ZStackup stackup;
        private ZSingle single;
        private ZPair   pair;

        //header
        const int rowRevision = 7;
        const string colRevision = "C";
        const int rowDate = 8;
        const string colDate = "C";
        const int rowFabContact = 9;
        const string colFabContact = "C";

        const int rowThickness = 7;
        const string colThickness = "Y";
        const string colThickTol = "AB";
        const int rowFinishedThickness = 8;
        const string colFinishedThickness = "Y";
        const int rowLamination = 9;
        const string colLamination = "Y";
        const string colLamTol = "AB";

        const int rowPartNo = 4;
        const string colPartNo = "W";
        const int rowPartRevision = 5;
        const string colPartRevision = "W";
        
        //layers
        private const int iFirstRow = 14; //first layer row
        private int iLastRow = -1;

        //column names/numbers
        private string sMaterial = "B";
        private string sConstruction = "C";
        private string sMetalNum = "D";

        private string sFill = "Y";
        private string[] aLayerType = {"S/M", "Plt", "Sig", "Preg", "Pln", "Core" };
        private const string sLayerType = "Z"; //can't be empty

        private string sThickness = "AA";
        private string sDk = "AB";

        //single + diffs
        const int rowImpedanceTitle = 13;
        const string colImpedance1 = "AF";

        class impGroup
        {
            public double Z;
            public int column;
            public string name;
            public string layers; //empty = ALL, Internal, External, coma-separated layer number list

            public impGroup(int col, string id, string scope, double Ztarget)
            {
                column = col;
                name = id;
                layers = scope;
                Z = Ztarget;
            }
        }

        class impList
        {
            List<impGroup> groupList;
            int maxLayer;

            public impList(int maxMetal)
            {
                groupList = new List<impGroup>();
                maxLayer = maxMetal;
            }

            public void Add(impGroup g)
            {
                groupList.Add(g);
            }

            public string Get(int col, int metalNumber)
            {
                foreach (impGroup g in groupList)
                {
                    if (g.column != col) continue;

                    string scope = g.layers;
                    if (scope.Length == 0) return g.name;

                    switch (scope.ToUpper())
                    {
                        case "INTERNAL":
                            if (metalNumber > 1 && metalNumber < maxLayer)
                            {
                                return g.name;
                            }
                            break;
                        case "EXTERNAL":
                            if (metalNumber == 1 || metalNumber == maxLayer)
                            {
                                return g.name;
                            }
                            break;
                        default:
                            {
                                string [] lays = scope.Split(',');
                                foreach (string a in lays)
                                {
                                    int L;
                                    if (Int32.TryParse(a, out L) && L == metalNumber)
                                    {
                                        return g.name;
                                    }
                                }
                            }
                            break;
                    }
                }

                return "";
            }

            public bool Contains(string name)
            {
                return (groupList.FirstOrDefault(i => i.name == name) != null);
            }
        }

        impList impGroupList;
        int maxMetalLayer;

        private int iPlating;
        private double[] plating_thick;

        private bool bPressedThickness;
        private ZLibraryCategory[] libraryPriorityArray;
        
        public ISU_Reader(string file, bool bPressed = false, ZLibraryCategory[] libraryPriority = null)
        {
            fileName = file;
            iPlating = 0;
            plating_thick = new double[2];
            plating_thick[0] = 0;
            plating_thick[1] = 0;

            maxMetalLayer = -1;
            
            bPressedThickness = bPressed;
            libraryPriorityArray = libraryPriority;
        }

        public bool Import()
        {
            return ImportStackup();
        }

        public bool Convert()
        {
            string origFile = fileName;
            ZPlannerManager.StatusMenu.UpdateProgress();
            bool retval = false;
            bool bConvert = Path.GetExtension(origFile) == ".xls";
            if (bConvert)
            {
                //1. convert xls to xlsx or xlsm
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
            try
            {
                workbook = new XLWorkbook(fileName);
                ws = workbook.Worksheet(1);
                ZPlannerManager.StatusMenu.UpdateProgress();
                if (ws != null)
                {
                    string revision = ReadString(rowRevision, "B");
                    revision = revision.Trim();
                    string[] words = revision.Split(' ');
                    if (words[0] == "ISU")
                    {
                        retval = true;
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

            project.Stackup = stackup;
            ZPlannerManager.Project = project;
            ZPlannerManager.StatusMenu.UpdateProgress();

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

        private int ReadInt(int iRow, int colNum)
        {
            int num = 0;
            string snum = ReadString(iRow, colNum);
            try
            {
                num = Int32.Parse(snum);//ws.Cell(iRow, colNum).GetValue<int>();
            }
            catch { }

            return num;
        }

        private int ReadInt(int iRow, string colName)
        {
            int num = 0;
            string snum = ReadString(iRow, colName);
            if (!Int32.TryParse(snum, out num)) num = 0;

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

        private bool ReadBool(int iRow, int colNum)
        {
            bool num = false;
            string snum = ReadString(iRow, colNum);
            if (!Boolean.TryParse(snum, out num)) num = false;

            return num;
        }

        private bool ReadBool(int iRow, string colName)
        {
            bool num = false;
            string snum = ReadString(iRow, colName);
            if (!Boolean.TryParse(snum, out num)) num = false;

            return num;
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

            //read header
            ReadHeader();

            //define last stackup row
            for (int iRow = iFirstRow; iRow <= iLastUsedRow; iRow++)
            {
                string lType = ReadString(iRow, sLayerType);
                int pos = Array.IndexOf(aLayerType, lType);
                if (pos == -1){
                    iLastRow = iRow - 1;
                    break;
                }
            }
            if (iLastRow < 0) iLastRow = iLastUsedRow;

            //-- impedance groups
            for (int iRow = iFirstRow; iRow <= iLastRow; iRow++)
            {
                int num = ReadInt(iRow, sMetalNum);
                maxMetalLayer = Math.Max(maxMetalLayer, num);
            }
            impGroupList = new impList(maxMetalLayer);

            ZPlannerManager.StatusMenu.UpdateProgress();

            CreateImpedances();
            //-- 

            for (int iRow = iFirstRow; iRow <= iLastRow; iRow++)
            {
                ReadLayer(iRow, stackup);
            }
            
            ZPlannerManager.StatusMenu.UpdateProgress();

            UpdatePlating();

            ZPlannerManager.StatusMenu.UpdateProgress();

            MatchMaterials();
            //if (bPressedThickness)
            //{
            //    RestoreUnpressedThickness();
            //}

            ZPlannerManager.StatusMenu.UpdateProgress();

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
                if ((x.Length == 3) && Double.TryParse(x[1], out z) && (z < 100)) s.Title = x[0] + "~ " + z.ToString() + x[2];//" ohms";
            }
            foreach (ZPair s in stackup.Pairs)
            {
                string[] x = s.Title.Split(sep);
                double z;
                if ((x.Length == 3) && Double.TryParse(x[1], out z) && (z < 100)) s.Title = x[0] + "~ " + z.ToString() + x[2]; //" ohms";
            }

        }


        public class MaterialData
        {
            public string ID { get; set; }
            public string H { get; set; }
            public ZLibraryCategory Category { get; set; }
            public string Dk{ get; set; }

            public bool ContainsDk(double dk, double tol = 0.001)
            {
                Dk = Dk.Trim();
                //single value or sorted by argument table -> F(a)  a:F;a:F;a:F;..
                double val = 0;
                string[] x = Dk.Split(new char[]{';'}, StringSplitOptions.RemoveEmptyEntries); //by f points

                if (x.Length == 0) return false;

                if (x.Length == 1)
                {
                    //check single
                    return (Double.TryParse(Dk, out val) && Math.Abs(val - dk) <= tol);
                }
                else //dk(f)
                {
                    //get range
                    double yfirst, ylast;
                    string[] y1 = x[0].Split(':');
                    string[] y2 = x[x.Length - 1].Split(':');

                    if ( ((y1.Length == 2) && Double.TryParse(y1[1], out yfirst)) && ((y2.Length == 2) && Double.TryParse(y2[1], out ylast))){
                        double ymin = yfirst, ymax = ylast;
                        if (yfirst > ylast) {
                            ymin = ylast; ymax = yfirst;
                        }
                        return dk >= ymin - tol && dk <= ymax + tol;
                    }
                }

                return false;
            }
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

        void MatchMaterials()
        {
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
                        }
                        break;
                }
            }
        }

        void RestoreUnpressedThickness()
        {
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
                                 Category = GetLibraryCategory(material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDCategory).Value),
                                 Dk = material.MaterialParameters.GetByID(ZStringConstants.DMLParameterIDDk).Value
                             }).ToList();

                    //additional check for Dk: find the first match
                    int iMatch = -1;
                    double layerDk = 0;
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDDielectricConstant, ref layerDk);

                    MaterialData md;
                    for (int i = 0; i < mData.Count; i++)
                    {
                        md = mData[i];
                        if (md.ContainsDk(layerDk))
                        {
                            iMatch = i;
                            break;
                        }
                    }

                    double th = 0;
                    if (mData.Count > 0 && iMatch > 0 &&Double.TryParse(mData[iMatch].H, out th))
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
                        ZMaterial m = ZPlannerManager.Dml.Materials.Find(x => x.ID == mData[iMatch].ID);
                        List<ZLayer> l = new List<ZLayer>();
                        l.Add(zl);
                        stackup.AssignMaterialToStackup(m, l);
                        //---ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, "Unpressed thicknesses from Library.");
                    }                                            
                    else
                    {
                        th = stackup.RestorePrepregUnpressedThickness(zl);
                    }

                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDOriginThickness, th);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, th);                    
                }
            }
        }

        void UpdateUsed()
        {
            /*ParameterIDZo_IsUsed = "Zo_IsUsed";
        public const string ParameterIDZdiff_IsUsed = "Zdiff_IsUsed";
            */
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

        void UpdateDkDf()
        {
            foreach (ZSingle single in stackup.Singles){
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

        private void ReadHeader()
        {
            string revision = ReadString(rowRevision, colRevision);
            string date = ReadString(rowDate, colDate);
            double thickness = ReadDouble(rowThickness, colThickness);
            double tol = ReadDouble(rowThickness, colThickTol);

            double finishedThickness = ReadDouble(rowFinishedThickness, colFinishedThickness);

            double lamination = ReadDouble(rowLamination, colLamination);
            double lamTol = ReadDouble(rowLamination, colLamTol);

            string partNo = ReadString(rowPartNo, colPartNo);
            string partRevision = ReadString(rowPartRevision, colPartRevision);
            string fabContact = ReadString(rowFabContact, colFabContact);

            ExportOptions.TheOptions.Fabricator = "ISU Petasys";
            ExportOptions.TheOptions.FabContact = fabContact;
            ExportOptions.TheOptions.ProjectName = partNo;
            ExportOptions.TheOptions.DesignRevision = partRevision;
        }

        private ZLayerType? GetLayerType(string s)
        {
            ZLayerType? ret = null;
            switch (s) {
                case "S/M":
                    ret = ZLayerType.SolderMask;
                    break;
                case "Plt":
                    break;
                case "Sig":
                    ret = ZLayerType.Signal;
                    break;
                case "Preg":
                    ret = ZLayerType.Prepreg;
                    break;
                case "Pln":
                    ret = ZLayerType.Plane;
                    break;
                case "Core":
                    ret = ZLayerType.Core;
                    break;
            }

            return ret;
        }

        private void MetalConstruction(string s, out string weight, out string foilTreatment)
        {
            string a = s.Trim();
            string[] words = a.Split(' ');
            words = words.Where(x => !string.IsNullOrEmpty(x)).ToArray();

            weight = words.Length > 0 ? words[0] : "";
            foilTreatment = words.Length > 2 ? words[2] : "";
        }


        private void UpdatePlating()
        {
            //update layers with plating
            bool bPlating = (plating_thick[0] > 0) || (plating_thick[1] > 0);
            if (bPlating)
            {
                bool bFirstMetal = true;
                ZLayer zLast = null;
                foreach (ZLayer zl in stackup.Layers)
                {
                    if (zl.isMetal())
                    {
                        zLast = zl;
                        if (bFirstMetal)
                        {
                            bFirstMetal = false;
                            if (plating_thick[0] > 0)
                            {
                                double w = 0;
                                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref w);
                                string comment = String.Format("Includes plating ({0} + {1})", w, plating_thick[0]);
                                ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, comment);

                                w += plating_thick[0];
                                ZLayerParameter layerParameter = zl.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                                //string ws = w.ToString();
                                if (layerParameter != null) layerParameter.SetEditedValue(w.ToString());//"N2"));
                            }
                        }
                    }
                }
                if (plating_thick[1] > 0)
                {
                    double w2 = 0;
                    ZPlannerProject.GetLayerParameterValue(zLast, ZStringConstants.ParameterIDThickness, ref w2);
                    string comment = String.Format("Includes plating ({0} + {1})", w2, plating_thick[1]);
                    ZPlannerProject.SetLayerParameterValue(zLast, ZStringConstants.ParameterIDComments, comment);

                    w2 += plating_thick[1];
                    ZLayerParameter layerParameter = zLast.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                    if (layerParameter != null) layerParameter.SetEditedValue(w2.ToString());
                }
            }
        }

        private void CoreConstruction(string s, out double df, out string construction, out double resin, ref int plies)
        {
            //.0035  (2)1035  61%            
            string a = s.Trim();

            construction = "";
            df = 0;
            resin = 0;
            plies = 1;

            string[] z = a.Split(' ');
            z = z.Where(x => !string.IsNullOrEmpty(x)).ToArray();

            string sdf = z.Length > 0 ? z[0] : "";
            if (sdf.Length > 0) if (!Double.TryParse(sdf, out df)) df = 0;

            string constr = z.Length > 1 ? z[1] : "";
            if (constr[0] == '(')
            {
                char[] del = { '(', ')' };
                string[] y = constr.Split(del);
                plies = y.Length > 1 ? Int32.Parse(y[1]) : 1;
                construction = y.Length > 2 ? y[2] : "";
            }
            else construction = constr;

            string res = z.Length > 2 ? z[2].TrimEnd('%') : "";
            if (res.Length > 0) if (!Double.TryParse(res, out resin)) resin = 0;
        }

        private void PrepregConstruction(string s, out string construction, out double resin, ref int plies)
        {
            string a = s.Trim();

            string constr = a;
            construction = "";
            resin = 0;
            plies = 1;
            if (a[0] == '(')
            {
                char[] del = { '(', ')' };
                string[] z = a.Split(del);
                z = z.Where(x => !string.IsNullOrEmpty(x)).ToArray();
                plies = z.Length > 0 ? Int32.Parse(z[0]) : 1;
                constr = z.Length > 1 ? z[1] : "";
            }
            if (constr.Length > 0)
            {
                string [] x = constr.Split('\u2022');//bullet
                construction = x.Length > 0 ? x[0] : "";
                string res = x.Length > 1 ? x[1].TrimEnd('%') : "";
                if (res.Length > 0) resin = Double.Parse(res);
            }
        }

        private void ReadLayer(int iRow, ZStackup stackup)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            int plies = 1;
            string material = ReadString(iRow, sMaterial);
            string construction = ReadString(iRow, sConstruction);
            int num = ReadInt(iRow, sMetalNum);
            double fill = ReadDouble(iRow, sFill);
            string type = ReadString(iRow, sLayerType);
            double thickness = ReadDouble(iRow, sThickness);
            double dk = ReadDouble(iRow, sDk);

            ZLayerType? lType  = null;

            if (type.Length > 0)
            {
                lType = GetLayerType(type);
            }
            if (material.Length > 0)
            {
                switch (lType) { 
                    case ZLayerType.SolderMask:
                    case ZLayerType.Core:
                    case ZLayerType.Prepreg:
                    parMap.Add(ZStringConstants.ParameterIDMaterial, material);
                    break;
                }
            }
            if (construction.Length > 0)
            {
                double resin = 0;
                double df = 0;
                string con = "";

                string weight = "";
                string foilTreatment = "";
                if (lType != null){
                    switch (lType){
                        case ZLayerType.Core:
                            CoreConstruction(construction, out df, out con, out resin, ref plies);
                            if (con.Length > 0)
                            {
                                if (plies > 1)
                                {
                                    con = plies.ToString() + "x" + con;
                                    plies = 1;
                                }
                                parMap.Add(ZStringConstants.ParameterIDConstruction, con);
                            }
                            if (resin > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDResinContent, resin.ToString());
                            }
                            if (df > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDLossTangent, df.ToString());
                            }
                            break;
                        case ZLayerType.Prepreg:
                            PrepregConstruction(construction, out con, out resin, ref plies);
                            if (con.Length > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDConstruction, con);
                            }
                            if (resin > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDResinContent, resin.ToString());
                            }
                            break;
                        case ZLayerType.Signal:
                        case ZLayerType.Plane:
                        case ZLayerType.SplitMixed:
                            MetalConstruction(construction, out weight, out foilTreatment);
                            if (fill > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDCopperPercent, (fill*100).ToString());
                                switch (lType)
                                {
                                    case ZLayerType.Signal:
                                        stackup.ForSignal = fill * 100;
                                        break;
                                    case ZLayerType.Plane:
                                        stackup.ForPlane = fill * 100;
                                        break;
                                    case ZLayerType.SplitMixed:
                                        stackup.ForMixed = fill * 100;
                                        break;
                                }
                            }
                            if (foilTreatment.Length > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDFoilTreatment, ZPlannerManager.GetCopperFoilString(foilTreatment));
                            }
                            if (weight.Length > 0)
                            {
                                parMap.Add(ZStringConstants.ParameterIDCopperThickness, weight);
                            }
                            break;
                    }
                }
                else {
                    //plating
                    plating_thick[iPlating] += thickness;
                    return; //skip plating layer
                }
            }

            if (thickness > 0)
            {
                if (plies > 1) thickness /= plies;
                parMap.Add(ZStringConstants.ParameterIDThickness, thickness.ToString());
            }

            if (dk > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDDielectricConstant, dk.ToString());
            }

            for (int i = 1; i <= plies; i++)
            {
                ZLayerType zType = (ZLayerType)lType;
                string layerID = stackup.AddLayer(zType);
                ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

                if (zLayer.isMetal() && iPlating == 0) iPlating = 1;
                if (zLayer.isMetal())
                {
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
                    name += num.ToString();
                    parMap.Add(ZStringConstants.ParameterIDLayerName, name);
                }

                foreach (KeyValuePair<string, string> kvp in parMap)
                {
                    ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                    if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                }

                if (zType == ZLayerType.Signal)
                {
                    ReadImpedances(iRow, stackup, zLayer);
                }
            }
        }

        private string GetImpedanceTitle(int iColumn)
        {
            return ReadString(rowImpedanceTitle, iColumn);
        }


        private void CreateImpedances()
        {
            int iFirstColumn = ws.Column(colImpedance1).ColumnNumber();
            int iLastColumn = ws.LastColumnUsed().ColumnNumber();

            bool bFirstSingle = true;
            bool bFirstPair = true;

            ZSingle single = null;
            ZPair pair = null;

            for (int i = iFirstColumn; i <= iLastColumn; i++)
            {
                string t = GetImpedanceTitle(i);
                switch (t)
                {
                    case "Zo":
                        List<impGroup> ll = GetImpedanceTargets(i, true);
                        foreach (impGroup gr in ll)
                        {
                            gr.name = GetUniqueName(gr.name, true);
                            impGroupList.Add(gr);
                            if (bFirstSingle)
                            {
                                bFirstSingle = false;
                                single = stackup.ActiveSingle;
                            }
                            else
                            {
                                single = new ZSingle(stackup);
                                stackup.Singles.Add(single);
                            }
                            single.Title = gr.name;
                            single.ImpedanceTarget = gr.Z;
                        }
                        break;
                    case "Zd":
                        List<impGroup> pl = GetImpedanceTargets(i, false);
                        foreach (impGroup gr in pl)
                        {
                            gr.name = GetUniqueName(gr.name, false);
                            impGroupList.Add(gr);
                            if (bFirstPair)
                            {
                                bFirstPair = false;
                                pair = stackup.ActivePair;
                            }
                            else
                            {
                                pair = new ZPair(stackup);
                                stackup.Pairs.Add(pair);
                            }
                            pair.Title = gr.name;
                            pair.ImpedanceTarget = gr.Z;
                        }
                        break;
                }                
            }
        }

        private string GetUniqueName(string value, bool bSingle)
        {
            string retval = "";
            if (bSingle)
            {
                string prefix = "Zo~" + value + " ohms";
                int cnt = 0;
                string name = prefix;
                while (true)
                {
                    if (impGroupList.Contains(name))
                    {
                        cnt++;
                        name = prefix + "(" + cnt.ToString() + ")";
                    }
                    else
                    {
                        retval = name;
                        break;
                    }
                }
            }
            else
            {
                string prefix = "Zdiff~" + value + " ohms";
                int cnt = 0;
                string name = prefix;
                while (true)
                {
                    if (impGroupList.Contains(name))
                    {
                        cnt++;
                        name = prefix + "(" + cnt.ToString() + ")";
                    }
                    else
                    {
                        retval = name;
                        break;
                    }
                }
            }

            return retval;
        }

        private List<impGroup> GetImpedanceTargets(int iColumn, bool bSingle)
        {
            List <impGroup> retval = new List<impGroup>();
            if (bSingle)
            {
                int iCol1 = iColumn - 1;
                int iCol2 = iColumn + 1;
                int iRow1 = 2;
                int iRow2 = rowImpedanceTitle - 1;

                IXLRange range = ws.Range(iRow1, iCol1, iRow2, iCol2);
                IXLCells res = range.CellsUsed(cell => cell.GetValue<string>() == "Zo:");

                foreach (IXLCell cell in res)
                {
                    int _row = cell.WorksheetRow().RowNumber();
                    int _col = cell.WorksheetColumn().ColumnNumber();

                    string x = ReadString(_row, _col + 1);//<value> + sigma sign
                    if (x.Length > 0)
                    {
                        string target = "";
                        string [] y = x.Split(' ');
                        if (y[0].Length > 0)
                        {
                            target = y[0];
                            double zTarget;
                            if (!Double.TryParse(target, out zTarget)) zTarget = 0;
                            string lrs = ReadString(_row - 2, _col + 1); //layers scope
                            impGroup z = new impGroup(iColumn, target, lrs, zTarget);
                            retval.Add(z);
                        }
                    }
                }
            }
            else //pair
            {
                int iCol1 = iColumn;
                int iCol2 = iColumn + 3;
                int iRow1 = 2;
                int iRow2 = rowImpedanceTitle - 1;

                IXLRange range = ws.Range(iRow1, iCol1, iRow2, iCol2);
                IXLCells res = range.CellsUsed(cell => cell.GetValue<string>() == "Zd:");

                foreach (IXLCell cell in res)
                {
                    int _row = cell.WorksheetRow().RowNumber();
                    int _col = cell.WorksheetColumn().ColumnNumber();

                    string x = ReadString(_row, _col + 1);//<value> + sigma sign
                    if (x.Length > 0)
                    {
                        string target = "";
                        string[] y = x.Split(' ');
                        if (y[0].Length > 0)
                        {
                            target = y[0];
                            double zTarget;
                            if (!Double.TryParse(target, out zTarget)) zTarget = 0;
                            string lrs = ReadString(_row - 2, _col + 1); //layers scope
                            impGroup z = new impGroup(iColumn, target, lrs, zTarget);
                            retval.Add(z);
                        }
                    }
                }
            }

            return retval;
        }

        private void ReadImpedances(int iRow, ZStackup stackup, ZLayer zLayer)
        {
            int iFirstColumn = ws.Column(colImpedance1).ColumnNumber();
            int iLastColumn = ws.LastColumnUsed().ColumnNumber();
            string state = "";
            double Zo = 0, Zd = 0, w = 0, s = 0;
            bool bSecondTrace = false;
            int lNum;
            for (int i = iFirstColumn; i <= iLastColumn; i++)
            {
                if (!ws.Cell(iRow, i).IsEmpty())
                {
                    double v = ReadDouble(iRow, i);
                    string t = GetImpedanceTitle(i);
                    switch (t)
                    {
                        case "Zo":
                            state = t;
                            Zo = v;
                            //--string sname = impNames[i];

                            if (Int32.TryParse(zLayer.GetLayerNumber(), out lNum) && lNum > 0)
                            {
                                string sname = impGroupList.Get(i,lNum);
                                single = stackup.Singles.Find(x => x.Title == sname);
                            }
                            break;
                        case "Line":
                            w = v;
                            if (state == "Zo")
                            {
                                //write single
                                if (Zo > 0 && w > 0)
                                {
                                    ZLayer zSingleLayer = single.GetLayerOfSingleImpedance(zLayer.ID);
                                    ZLayerParameter layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_TraceWidth);
                                    if (layerParameter != null) layerParameter.SetEditedValue(w.ToString());
                                    layerParameter = zSingleLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_Zo);
                                    if (layerParameter != null) layerParameter.SetEditedValue(Zo.ToString());
                                }
                            }
                            else if (state == "Zd")
                            {
                                if (bSecondTrace)
                                {
                                    bSecondTrace = false;
                                    //write diff
                                    if (Zd > 0 && w > 0 && s > 0)
                                    {
                                        string aname = zLayer.GetLayerParameterValue(ZStringConstants.ParameterIDLayerName);

                                        ZLayer zDiffLayer = pair.GetLayerOfPairImpedance(zLayer.ID);
                                        ZLayerParameter layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
                                        if (layerParameter != null) layerParameter.SetEditedValue(w.ToString());
                                        layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
                                        if (layerParameter != null) layerParameter.SetEditedValue(s.ToString());
                                        layerParameter = zDiffLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
                                        if (layerParameter != null) layerParameter.SetEditedValue(Zd.ToString());
                                    }
                                }
                                else
                                {
                                    bSecondTrace = true;
                                }
                            }
                            break;
                        case "Space":
                            s = v;
                            break;
                        case "Zd":
                            state = t;
                            Zd = v;
                            
                            //--string dname = impNames[i];

                            if (Int32.TryParse(zLayer.GetLayerNumber(), out lNum) && lNum > 0)
                            {
                                string dname = impGroupList.Get(i,lNum);
                                pair = stackup.Pairs.Find(x => x.Title == dname);
                            }
                            break;
                    }
                }
            }
        }
    }
}