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
using ZZero.ZPlanner.UI.Dialogs;
using System.Linq;
using System.Collections.Specialized;


namespace ZZero.ZPlanner.Translators
{

    public class TTM_Reader : IImport
    {
        private string fileName;
        private IXLWorksheet ws;
        private ZStackup stackup;

        private string material;
        private bool bPressedThickness;
        private ZLibraryCategory[] libraryPriorityArray;

        //header
        const int rowRevision = 8;
        const int rowImpedance = 12;
        const string colImpedance1 = "G";
        Dictionary<int, string> impNames;


        //Layers sheet
        private const int iFirstRow = 16; //first layer row
        int iLastUsedRow = -1;


        public TTM_Reader(string file, bool bPressed = false, ZLibraryCategory[] libraryPriority = null)
        {
            fileName = file;
            bPressedThickness = bPressed;
            libraryPriorityArray = libraryPriority;
            impNames = new Dictionary<int, string>();
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
                    string revision = ReadString(rowRevision, "A");
                    revision = revision.Trim();
                    string[] words = revision.Split(' ');
                    if (words[0] == "TTM")
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
                project.Parameters.Add(parameter);
                if (parameter.SubParameters != null && parameter.SubParameters.Count > 0) project.SubParameters.AddRange(parameter.SubParameters);
            }

            int iLastParameter = project.Parameters.Count - 1;
            int order = project.Parameters[iLastParameter].Order;

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

            ZPlannerManager.ResumeFSHandlers();
            stackup.Singles.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            stackup.Pairs.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            stackup.Layers.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            return true;
        }


        private bool Translate(ZStackup stackup)
        {
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

            ZPlannerManager.StatusMenu.UpdateProgress();

            CreateImpedances();

            ZPlannerManager.StatusMenu.UpdateProgress();

            ReadLayers();

            ZPlannerManager.StatusMenu.UpdateProgress();

            RenameImpedances();

            ZPlannerManager.StatusMenu.UpdateProgress();

            UpdatePlating();

            ZPlannerManager.StatusMenu.UpdateProgress();

            MatchMaterials();

            ZPlannerManager.StatusMenu.UpdateProgress();

            //if (bPressedThickness)
            //{
            //    RestoreUnpressedThickness();
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

        private void RenameImpedances()
        {
            foreach (ZSingle single in stackup.Singles)
            {
                foreach (ZLayer lay in single.Layers)
                {
                    if (lay.GetLayerType() == ZLayerType.Signal)
                    {
                        string target = lay.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Target);
                        if (target.Length > 0)
                        {
                            single.Title = GetUniqueName(target, true);
                            break;
                        }
                    }
                }
            }
            foreach (ZPair pair in stackup.Pairs)
            {
                foreach (ZLayer lay in pair.Layers)
                {
                    if (lay.GetLayerType() == ZLayerType.Signal)
                    {
                        string target = lay.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Target);
                        if (target.Length > 0)
                        {
                            pair.Title = GetUniqueName(target, false);
                            break;
                        }
                    }
                }
            }
        }

        private void ReadHeader()
        {
            string jobName = ReadString(4, "C");
            string partNumber = ReadString(6, "C");
            material = ReadString(9, "C");

            double targetThickness = ReadDouble(8,"I");
            double totalThickness = ReadDouble(9,"I");


            string date = ReadString(4, "O");
                       
            ExportOptions.TheOptions.Fabricator = "TTM Technologies";
            ExportOptions.TheOptions.ProjectName = partNumber;
        }

        void CreateImpedances()
        {
            int iFirstColumn = ws.Column(colImpedance1).ColumnNumber();
            int iLastColumn = ws.LastColumnUsed().ColumnNumber();

            bool bFirstSingle = true;
            bool bFirstPair = true;

            ZSingle single = null;
            ZPair pair = null;

            List<int> targetPos = new List<int>();
            for (int i = iFirstColumn; i <= iLastColumn; i++)
            {
                string sTarget = ReadString(rowImpedance,i);
                if (sTarget.Contains("Target"))
                {
                    targetPos.Add(i);
                }
            }
            targetPos.Add(iLastColumn + 1);

            int prevPos = -1;
            foreach (int p in targetPos)
            {
                if (prevPos > 0)
                {
                    //create [prevPos .. p]
                    if ((p - prevPos) > 7)
                    {//diff
                        pair = new ZPair(stackup);
                        stackup.Pairs.Add(pair);
                        impNames.Add(prevPos, pair.Title);
                    }
                    else
                    {//single
                        single = new ZSingle(stackup);
                        stackup.Singles.Add(single);
                        impNames.Add(prevPos, single.Title);
                    }
                }
                prevPos = p;
            }
            //remove ActiveSingle
            if (stackup.Singles.Count > 1)
            {
                stackup.Singles.RemoveAt(0);
                stackup.ActiveSingle = stackup.Singles[0];
            }
            //remove ActivePair
            if (stackup.Pairs.Count > 1)
            {
                stackup.Pairs.RemoveAt(0);
                stackup.ActivePair = stackup.Pairs[0];
            }

        }

        private void ReadLayers()
        {
            for (int iRow = iFirstRow; iRow <= iLastUsedRow; iRow++)
            {
                ReadLayer(iRow, stackup);
            }
        }

        private void ReadLayer(int iRow, ZStackup stackup)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            int iNum = ReadInt(iRow, "A");
            string layerType = ReadString(iRow,"B");
            if (iNum == 0 && layerType.Length == 0) return;

            ZLayerType ltype = ZLayerType.Signal;
            switch (layerType)
            {
                case "Signal":
                    ltype = ZLayerType.Signal;
                    break;
                case "Plane":
                    ltype = ZLayerType.Plane;
                    break;
                case "Prepreg":
                    ltype = ZLayerType.Prepreg;
                    break;
                case "Core":
                    ltype = ZLayerType.Core;
                    break;
                case "Mask":
                    ltype = ZLayerType.SolderMask;
                    break;
            }
            string construction = ReadString(iRow, "C");
            double thick = ReadDouble(iRow, "D"); //calculated: pressed dielectrics, plating added(w/o weight)
            double dk = ReadDouble(iRow, "E");
            double df = ReadDouble(iRow, "F");

            //construction
            double copperWeight = 0;
            int plies = 1;
            string constr = "";

            bool isMetal = false;
            switch (ltype)
            {
                case ZLayerType.Signal:
                case ZLayerType.Plane:
                    isMetal = true;
                    Double.TryParse(construction, out copperWeight);
                    break;
                case ZLayerType.Prepreg:
                    string[] x = construction.Split('/');
                    if (x.Length > 1) plies = x.Length;
                    constr = x[0];
                    break;
                case ZLayerType.Core:
                    constr = construction;
                    //string[] y = construction.Split('x');
                    //if (y.Length > 1) {
                    //    Int32.TryParse(y[0], out plies);
                    //    constr = y[0] + "x" +y[1];
                    //}
                    break;
            }

            //parameters
            if (isMetal)
            {
                string name = "";
                switch (ltype)
                {
                    case ZLayerType.Signal: name = "Signal_";
                        break;
                    case ZLayerType.Plane: name = "Plane_";
                        break;
                    case ZLayerType.SplitMixed: name = "Split_";
                        break;
                }
                name += iNum.ToString();
                parMap.Add(ZStringConstants.ParameterIDLayerName, name);

                if (thick > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDThickness, thick.ToString());
                }
                if (copperWeight > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDCopperThickness, copperWeight.ToString());
                }

            }
            else //dielectric
            {
                if (material.Length > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDMaterial, material);
                }

                if (thick > 0)
                {
                    if (ltype == ZLayerType.Prepreg)
                    {
                        if (plies > 1) thick /= plies;
                    }
                    parMap.Add(ZStringConstants.ParameterIDThickness, thick.ToString());
                }
                if (dk > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDDielectricConstant, dk.ToString());
                }
                if (df > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDLossTangent, df.ToString());
                }
                if (constr.Length > 0)
                {
                    parMap.Add(ZStringConstants.ParameterIDConstruction, constr);
                }

            }
            
            for (int i = 1; i <= plies; i++)
            {
                ZLayerType zType = (ZLayerType)ltype;
                string layerID = stackup.AddLayer(zType);
                ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

                foreach (KeyValuePair<string, string> kvp in parMap)
                {
                    ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                    if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                }

                if (zType == ZLayerType.Signal)
                {
                    ReadImpedances(iRow, stackup, zLayer, iNum);
                }
            }
        }

        private bool ReadReferences(ZLayer layer, int nLayer, string refLayers)
        {
            string [] refs = refLayers.Split(',');
            int ref1 = -1;
            if (refs.Length > 0) Int32.TryParse(refs[0], out ref1);
            int ref2 = -1;
            if (refs.Length > 1) Int32.TryParse(refs[1], out ref2);

            ZLayerParameter layerParameter = null;
            if (ref1 > 0)
            {
                if (ref1 < nLayer)
                {
                    layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
                else
                {
                    layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref1.ToString());
                }
            }
            if (ref2 > 0)
            {
                if (ref2 < nLayer)
                {
                    layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_TopReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
                else
                {
                    layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDZo_BottomReference);
                    if (layerParameter != null) layerParameter.SetEditedValue(ref2.ToString());
                }
            }

            return true;
        }

        private void ReadImpedances(int iRow, ZStackup stackup, ZLayer layer, int metalNum)
        {
            foreach (KeyValuePair<int, string> entry in impNames)
            {
                ZLayer impLayer = null;

                int pos = entry.Key;
                string name = entry.Value;
                ZSingle single = stackup.Singles.Find(x => x.Title == name);
                if (single != null)
                {
                    impLayer = single.GetLayerOfSingleImpedance(layer.ID);
                    double targetZ = ReadDouble(iRow, pos);
                    if (targetZ > 0)
                    {
                        single.ImpedanceTarget = targetZ;
                        string refPlane = ReadString(iRow, pos + 3);
                        double designW = ReadDouble(iRow, pos + 4);
                        double calcW = ReadDouble(iRow, pos + 5);
                        double calcZ = ReadDouble(iRow, pos + 6);

                        ZLayerParameter layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_TraceWidth);
                        if (layerParameter != null) layerParameter.SetEditedValue(calcW.ToString());
                        layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_Zo);
                        if (layerParameter != null) layerParameter.SetEditedValue(calcZ.ToString());
                        layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZo_Target);
                        if (layerParameter != null) layerParameter.SetEditedValue(targetZ.ToString());

                        ReadReferences(impLayer, metalNum, refPlane);
                    }
                    continue;
                }
                ZPair pair = stackup.Pairs.Find(x => x.Title == name);
                if (pair != null)
                {
                    impLayer = pair.GetLayerOfPairImpedance(layer.ID);
                    double targetZ = ReadDouble(iRow, pos);
                    if (targetZ > 0)
                    {
                        pair.ImpedanceTarget = targetZ;
                        string refPlane = ReadString(iRow, pos + 3);
                        double designW = ReadDouble(iRow, pos + 4);
                        double designS = ReadDouble(iRow, pos + 5);
                        double calcW = ReadDouble(iRow, pos + 6);
                        double calcS = ReadDouble(iRow, pos + 7);
                        double calcZ = ReadDouble(iRow, pos + 8);

                        ZLayerParameter layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
                        if (layerParameter != null) layerParameter.SetEditedValue(calcW.ToString());
                        layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
                        if (layerParameter != null) layerParameter.SetEditedValue(calcS.ToString());
                        layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
                        if (layerParameter != null) layerParameter.SetEditedValue(calcZ.ToString());
                        layerParameter = impLayer.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Target);
                        if (layerParameter != null) layerParameter.SetEditedValue(targetZ.ToString());

                        ReadReferences(impLayer, metalNum, refPlane);
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

        private void UpdatePlating()
        {
            foreach (ZLayer zl in stackup.Layers)
            {
                if (zl.isMetal())
                {
                    double t = 0, w = 0;
                    if (zl.GetLayerParameterValue(ZStringConstants.ParameterIDThickness, out t) &&
                        zl.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness, out w))
                    {
                        if (Math.Abs(t - w * 1.2) > 0.1)
                        {
                            ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, "Includes plating. Plating weight unknown.");
                        }

                    }
                }
            }
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
                    double pressedThickness = 0;
                    ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref pressedThickness);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDPrepregThickness, pressedThickness);
                    ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDFabricatorThickness, pressedThickness);

                    double th = stackup.RestorePrepregUnpressedThickness(zl);

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


    }

}