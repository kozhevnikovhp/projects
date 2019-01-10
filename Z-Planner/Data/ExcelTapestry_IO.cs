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

namespace ZZero.ZPlanner.Translators
{

    public class TapestryReader
    {
        //properties sheet
        private const int iColPropName = 1;

        //Layers sheet
        private const int iFirstRow = 9; //first layer row
        private int iLastRow = -1;
        
        //column numbers
        private const int iLayerNum = 1; //A: empty for diel. and plating; 1-based for metal
        private const int sDescription = 2; //B: some types [Soldermask, Plating, Core, Pre-Preg]
        private const int dDielThick = 3;  //C: [mils]
        private const int dCuDensity = 4; //D: [%]
        private const int dCuWieght = 5; //E: [oz]
        private const int sName = 6; //F
        private const int sType = 7;//G: metal type [S,P,G]
        private const int sRefPlane = 8; //H: l1[,l2] - ref layer number(s)
        private const int sPlaneVoltage = 9; //I: list of doubles [3.3,2.5,..] or "Gnd" for zero
        private const int sFoilTreatment = 10; //J: spec syntax
        private const int sResinSystem = 11;//K: material name e.g. TU-768
        private const int sGlassStyle = 12;//L: normalized construction AxB/CxD/ExF !
        private const int sResinContent = 13;//M: for each glass style A/B/C !
        private const int dFinishedThickness = 14;//N: [mils]
        private const int dDkVendor = 15; //O
        private const int dDfVendor = 16; //P

        private const int firstImpedance = 17;//Q: the first colimn to look for single-ended or dp
        //single-ended: increments to colNumber
        private const int dTargetZ = 0;//[Ohms]
        private const int dTol = 1; //%
        private const int dDLW = 2; //width [mils]
        private const int dFLW = 3; //width [mils]
        private const int dCalcZ = 4; //[Ohms]
        private const int seSize = 5; //number of columns in SE

        //diff pairs: increments to colNumber
        private const int dTargetZ_dp = 0;//[Ohms]
        private const int dTol_dp = 1; //%
        private const int dDLW_dp = 2; //width [mils]
        private const int dSpace_dp = 3; //width [mils]
        private const int dPitch_dp = 4; //width
        private const int dFLW_dp = 5; //width [mils]
        private const int dCalcZ_dp = 6; //[Ohms]
        private const int dpSize = 7; //number of columns in DP


        private string fileName;
        private IXLWorksheet ws, wsProps;
        private ZStackup stackup;
        private ZSingle single;
        private ZPair   pair;
        private HashSet<int> skipped_rows;
        private Dictionary<string, string> ref_layers; //ID - references
        
        private int iPlating;
        private double[] plating_thick;

        public TapestryReader(string file)
        {
            fileName = file;
            skipped_rows = new HashSet<int>();
            iPlating = 0;
            plating_thick = new double[2];
            ref_layers = new Dictionary<string, string>();
        }

        public bool Import()
        {
            bool bConvert = Path.GetExtension(fileName).ToUpper() == ".XLS";
            string fileToImport = fileName;
            if (bConvert)
            {
                fileToImport = Convert(fileName);
                if (fileToImport.Length > 0)
                {
                    fileName = fileToImport;
                }
                else
                {
                    return false;
                }
            }

            return ImportStackup();
        }

        private string Convert(string origFile)
        {
            //1. convert xls to xlsx
            string retFile = Path.ChangeExtension(origFile, ".xlsx");

            try
            {
                var application = new Microsoft.Office.Interop.Excel.Application();
                var workbook = application.Workbooks.Open(origFile);
                if (File.Exists(retFile))
                {
                    File.Delete(retFile);
                }
                workbook.SaveAs(retFile, XlFileFormat.xlOpenXMLWorkbook);
                workbook.Close();
            }
            catch (Exception e)
            {
                //TODO: add warning dialog (no Office on PC or file is opened by another app)
                MessageBox.Show(e.Message);
                retFile =  "";
            }

            return retFile;
        }

        public bool ImportStackup()
        {
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

            //add additional Tapestry data ===============================================
            int iLastParameter = project.Parameters.Count - 1;
            int order = project.Parameters[iLastParameter].Order;

            ZParameter par = new ZParameter(ZStringConstants.ParameterIDPlaneVoltage);
            par.Title = "Plane Voltage";
            par.Description = "Comma separated list of plane voltages or Gnd for ground";
            par.Table = ZTableType.Stackup;
            par.ValueType = ZValueType.Text;
            par.IsReadOnly = true;
            par.IsCustom = true;
            par.Width = 70;
            par.Order = order++;
            project.Parameters.Add(par);
            //============================================================================

            // parse file
            Translate(stackup);

            project.Stackup = stackup;
            ZPlannerManager.Project = project;

            //--CopperThicknessDialog dlg = new CopperThicknessDialog(stackup);
            //--dlg.ShowDialog();
            //if (dlg.DialogResult == DialogResult.OK) ;

            //ui
            ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);
            ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();

            ZPlannerManager.ResumeFSHandlers();
            return true;

        }

        private void UpdateRefPlanes()
        {
            /*
            if (ref_layers.Count > 0){
                int metalIdx = 0;
                foreach (ZLayer zl in stackup.Layers)
                {
                    if (zl.isMetal()){
                        metalIdx++;
                        if (ref_layers.Contains(metalIdx))
                        {
                            ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDPlaneReference, "PlaneReference");
                        }
                    }
                }
            }
             */
        }

        private void UpdatePlating()
        {
            //update layers with plating
            if (iPlating > 0)
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
                            double w = 0;
                            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref w);
                            w += plating_thick[0];
                            ZLayerParameter layerParameter = zl.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                            if (layerParameter != null) layerParameter.SetEditedValue(w.ToString("N2"));

                            ZPlannerProject.SetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, "Includes plating");
                        }
                    }
                }
                if (iPlating > 1)
                {
                    double w2 = 0;
                    ZPlannerProject.GetLayerParameterValue(zLast, ZStringConstants.ParameterIDThickness, ref w2);
                    w2 += plating_thick[1];
                    ZLayerParameter layerParameter = zLast.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                    if (layerParameter != null) layerParameter.SetEditedValue(w2.ToString("N2"));
                    ZPlannerProject.SetLayerParameterValue(zLast, ZStringConstants.ParameterIDComments, "Includes plating");
                }
            }
        }

        private double ParseFreq(string sfreq)
        {
            double retval = 0;
            string[] words = sfreq.Split(' ');
            if (words.Length > 0)
            {
                double d = 0;
                if (Double.TryParse(words[0], out d))
                {
                    retval = d;
                }
            }
            return retval;
        }
        private bool GetProps()
        {
            int maxRow = wsProps.LastRowUsed().RowNumber();
            string s = "";
            double freq = 0;
            for (int i = 1; i < maxRow; i++)
            {
                try
                {
                    s = wsProps.Cell(i, iColPropName).GetValue<string>();
                }
                catch { }

                if (s == "Target Frequency"){
                    try
                    {
                        string sfreq = wsProps.Cell(i, iColPropName + 1).GetValue<string>();
                        freq = ParseFreq(sfreq);
                    }
                    catch { }
                    break;
                }
            }
            if (freq > 0)
            {
                stackup.Frequency = freq;
            }
            return true;
        }

        private bool Translate(ZStackup stackup)
        {
            int iLastUsedRow = 0;
            try
            {
                var workbook = new XLWorkbook(fileName);
                ws = workbook.Worksheet("Layers");
                wsProps = workbook.Worksheet("Properties");
                iLastUsedRow = ws.LastRowUsed().RowNumber();
            }
            catch (Exception e)
            {
                //TODO: add warning dialog
                MessageBox.Show(e.Message);
                return false;
            }

            for (int iRow = iFirstRow; iRow <= iLastUsedRow; iRow++)
            {
                if (!isValidRow(iRow))
                {
                    iLastRow = iRow - 1;
                    break;
                }
                ReadLayer(iRow, stackup);
            }
            if (iLastRow < 0) iLastRow = iLastUsedRow - 1 ;


            //singles
            int iDiffPair = ReadAllSingleEnded();

            //diffpairs
            if (iDiffPair > 0)
            {
                ReadAllDiffPairs(iDiffPair);
            }

            UpdatePlating();

            UpdateRefPlanes();

            GetProps();

            return true;
        }

        private bool isValidRow(int iRow)
        {
            //either Description or Name MUST be non-empty
            string description = ReadString(iRow, sDescription);
            string name = ReadString(iRow,sName);
            
            return (description.Length > 0) || (name.Length > 0);
        }

        private int ReadInt(int iRow, int colNum)
        {
            int num = 0;
            try
            {
                num = ws.Cell(iRow, colNum).GetValue<int>();
            }
            catch { }

            return num;
        }

        private double ReadDouble(int iRow, int colNum)
        {
            double num = 0;
            try
            {
                num = ws.Cell(iRow, colNum).GetValue<double>();
            }
            catch { }

            return num;
        }

        private bool ReadBool(int iRow, int colNum)
        {
            bool num = false;
            try
            {
                num = ws.Cell(iRow, colNum).GetValue<bool>();
            }
            catch { }

            return num;
        }

        private string ReadString(int iRow, int colNum)
        {
            string num = "";
            try
            {
                num = ws.Cell(iRow, colNum).GetValue<string>();
            }
            catch { }

            return num;
        }

        private void ReadLayer(int iRow, ZStackup stackup)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            int num = ReadInt(iRow, iLayerNum);
            string description = ReadString(iRow, sDescription);
            double dielThick = ReadDouble(iRow, dDielThick);
            double cuDensity = ReadDouble(iRow, dCuDensity);
            double cuWeight = ReadDouble(iRow, dCuWieght);
            string name = ReadString(iRow, sName);
            string metalType = ReadString(iRow, sType);
            string refPlanes = ReadString(iRow, sRefPlane);
            string material = ReadString(iRow, sResinSystem);
            string construction = ReadString(iRow, sGlassStyle);
            string resin = ReadString(iRow, sResinContent);
            double thickness = ReadDouble(iRow, dFinishedThickness);
            double Dk = ReadDouble(iRow, dDkVendor);
            double Df = ReadDouble(iRow, dDfVendor);
            string planeVoltage = ReadString(iRow, sPlaneVoltage);

            //fill in layer parameters
            if (name.Length > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDLayerName, name);
            }
            //type
            ZLayerType? lType = null;

            if (description.Length > 0){
                switch (description)
                {
                    case "Soldermask":
                        lType = ZLayerType.SolderMask;
                        break;
                    case "Plating":
                        plating_thick[iPlating] = thickness;
                        iPlating ++;
                        break;
                    case "Pre-Preg":
                        lType = ZLayerType.Prepreg;
                        break;
                    case "Core":
                        lType = ZLayerType.Core;
                        break;
                }
            }
            else if (metalType.Length > 0)
            {
                switch (metalType){
                    case "S":
                        lType = ZLayerType.Signal;
                        break;
                    case "G":
                    case "P":
                        lType = ZLayerType.Plane;
                        break;
                }
            }
            if (lType == null){
                skipped_rows.Add(iRow);
                //no type - return;
                return;
            }

            if (material.Length > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDMaterial, material);
            }

            if (thickness > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDThickness, thickness.ToString());
            }

            if (Dk > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDDielectricConstant, Dk.ToString());
            }
            if (Df > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDLossTangent, Df.ToString());
            }

            if (cuWeight > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDCopperThickness, cuWeight.ToString());
            }

            if (cuDensity > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDCopperPercent, cuDensity.ToString());
            }

            if (planeVoltage.Length > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDPlaneVoltage, planeVoltage);
            }

            if (resin.Length > 0)
            {
                string[] r = resin.Split('/');
                parMap.Add(ZStringConstants.ParameterIDResinContent, r[0]);
            }
            if (construction.Length > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDConstruction, construction);
            }

            if (planeVoltage.Length > 0){
                parMap.Add("1", planeVoltage);
            }
            //set actual parameters
            ZLayerType zType = (ZLayerType)lType;
            string layerID = stackup.AddLayer(zType);
            ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

            if (refPlanes.Length > 0)
            {
                ref_layers.Add(layerID, refPlanes);
            }

            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }

        }

        //single-ended configurations
        private int ReadAllSingleEnded()
        {
            int retval = -1;
            int iCol = firstImpedance;
            int iTitleRow = iFirstRow - 2;

            while (true)
            {
                string title = ReadString(iTitleRow, iCol);
                if (title == "Single Ended")
                {
                    if (iCol == firstImpedance)
                    {
                        single = stackup.ActiveSingle;
                    }
                    else
                    {
                        single = new ZSingle(stackup);
                        stackup.Singles.Add(single);
                    }
                    ReadSingleEnded(iCol);
                }
                else if (title == "Edge Coupled")
                {
                    retval = iCol;
                    break;
                }
                else break;

                iCol += seSize;
            }

            return retval;
        }

        private void ReadSingleEnded(int iStartCol)
        {
            int iLayIndex = 0;
            for (int iRow = iFirstRow; iRow <= iLastRow; iRow++)
            {
                if (skipped_rows.Contains(iRow)) continue;
                iLayIndex++;
                ReadSingleEndedLayer(iRow, iStartCol, iLayIndex);
            }
        }

        private void ReadSingleEndedLayer(int iRow, int iStartCol, int iLayIndex)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            //Dk,Df
            double Dk = ReadDouble(iRow, dDkVendor);
            double Df = ReadDouble(iRow, dDfVendor);

            //
            double targetZ = ReadDouble(iRow, iStartCol + dTargetZ);
            double tol = ReadDouble(iRow, iStartCol + dTol);
            double DLW = ReadDouble(iRow, iStartCol + dDLW);
            double FLW = ReadDouble(iRow, iStartCol + dFLW);
            double calcZ = ReadDouble(iRow, iStartCol + dCalcZ);


            if (Dk > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZo_DielectricConstant, Dk.ToString());
            }
            if (Df > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZo_LossTangent, Df.ToString());
            }

            if (targetZ > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZo_Target, targetZ.ToString());

            }
            if (DLW > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZo_TraceWidth, DLW.ToString());
            }

            
            ZLayer zLayer = single.GetLayerOfSingleImpedance(iLayIndex - 1);
            ZLayer L = stackup.GetLayerOfStackup(zLayer.ID);
            int iLay = -1;
            if (ZPlannerProject.GetLayerParameterValue(L, ZStringConstants.ParameterIDLayerNumber, ref iLay) && iLay > 0)
            {
                string ID = zLayer.ID;
                if (ref_layers.ContainsKey(ID))
                {
                    string[] refs = ref_layers[ID].Split(',');
                    if (refs.Length > 0)
                    {
                        foreach (string s in refs)
                        {
                            int n = -1;
                            if (Int32.TryParse(s, out n))
                            {
                                ZLayer referenceLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == n.ToString());
                                if (referenceLayer != null)
                                {
                                    if (n < iLay)
                                    {
                                        parMap.Add(ZStringConstants.ParameterIDZo_TopReference, referenceLayer.ID);
                                    }
                                    else
                                    {
                                        parMap.Add(ZStringConstants.ParameterIDZo_BottomReference, referenceLayer.ID);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            
            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }
            
        }

        //differential pairs
        private void ReadAllDiffPairs(int firstDiffPair)
        {
            int iCol = firstDiffPair;
            int iTitleRow = iFirstRow - 2;

            while (true)
            {
                string title = ReadString(iTitleRow, iCol);
                if (title == "Edge Coupled")
                {
                    if (iCol == firstDiffPair)
                    {
                        pair = stackup.ActivePair;
                    }
                    else
                    {
                        pair = new ZPair(stackup);
                        stackup.Pairs.Add(pair);
                    }
                    ReadDiffPair(iCol);
                }
                else break;

                iCol += dpSize;
            }
        }

        private void ReadDiffPair(int iStartCol)
        {
            int iLayIndex = 0;
            for (int iRow = iFirstRow; iRow <= iLastRow; iRow++)
            {
                if (skipped_rows.Contains(iRow)) continue;
                iLayIndex++;
                ReadDiffPairLayer(iRow, iStartCol, iLayIndex);
            }
        }

        private void ReadDiffPairLayer(int iRow, int iStartCol, int iLayIndex)
        {
            Dictionary<string, string> parMap = new Dictionary<string, string>();

            //Dk,Df
            double Dk = ReadDouble(iRow, dDkVendor);
            double Df = ReadDouble(iRow, dDfVendor);

            //
            double targetZ = ReadDouble(iRow, iStartCol + dTargetZ_dp);
            double tol = ReadDouble(iRow, iStartCol + dTol_dp);
            double DLW = ReadDouble(iRow, iStartCol + dDLW_dp);
            double spacing = ReadDouble(iRow, iStartCol + dSpace_dp);
            double pitch = ReadDouble(iRow, iStartCol + dPitch_dp);
            double FLW = ReadDouble(iRow, iStartCol + dFLW_dp);
            double calcZ = ReadDouble(iRow, iStartCol + dCalcZ_dp);

            if (Dk > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZdiff_DielectricConstant, Dk.ToString());
            }
            if (Df > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZdiff_LossTangent, Df.ToString());
            }

            if (targetZ > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZo_Target, DLW.ToString());

            }
            if (DLW > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZdiff_TraceWidth, DLW.ToString());
            }
            if (spacing > 0)
            {
                parMap.Add(ZStringConstants.ParameterIDZdiff_TraceSpacing, spacing.ToString());
            }

            ZLayer zLayer = pair.GetLayerOfPairImpedance(iLayIndex - 1);

            ZLayer L = stackup.GetLayerOfStackup(zLayer.ID);
            int iLay = -1;
            if (ZPlannerProject.GetLayerParameterValue(L, ZStringConstants.ParameterIDLayerNumber, ref iLay) && iLay > 0)
            {
                string ID = zLayer.ID;
                if (ref_layers.ContainsKey(ID))
                {
                    string[] refs = ref_layers[ID].Split(',');
                    if (refs.Length > 0)
                    {
                        foreach (string s in refs)
                        {
                            int n = -1;
                            if (Int32.TryParse(s, out n))
                            {
                                ZLayer referenceLayer = stackup.Layers.Find(x => x.GetLayerParameterValue(ZStringConstants.ParameterIDLayerNumber) == n.ToString());
                                if (referenceLayer != null)
                                {
                                    if (n < iLay)
                                    {
                                        parMap.Add(ZStringConstants.ParameterIDZdiff_TopReference, referenceLayer.ID);
                                    }
                                    else
                                    {
                                        parMap.Add(ZStringConstants.ParameterIDZdiff_BottomReference, referenceLayer.ID);
                                    }
                                }
                            }
                        }
                    }
                }
            }

            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }

        }

    }
}