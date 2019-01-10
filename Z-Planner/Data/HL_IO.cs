using System;
using System.IO;
using System.Collections.Generic;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Text;
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.UI;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.UI.Dialogs;

namespace ZZero.ZPlanner.Translators
{
    class HL_Parser
    {
        public HL_Parser(string file)
        {
            InputFile = file;
            text = "";
        }

        public bool Import()
        {
            ZPlannerManager.SuspendFSHandlers();
            
            ZPlannerProject project = new ZPlannerProject();
 
            // create stackup
            stackup = new ZStackup(project);
            stackup.Title = Path.GetFileNameWithoutExtension(InputFile);
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

            // parse file
            Translate(stackup);

            project.Stackup = stackup;
            ZPlannerManager.Project = project;

            CopperThicknessDialog dlg = new CopperThicknessDialog(stackup);
            dlg.ShowDialog();
            //if (dlg.DialogResult == DialogResult.OK) ;

            //ui
            ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);

            ZPlannerManager.ResumeFSHandlers();            
            return true;
        }

        private string InputFile;
        private string text;
        private ZStackup stackup;

        private bool ReadAll()
        {
            if (File.Exists(InputFile))
            {
                StreamReader reader = File.OpenText(InputFile);
                string line;
                while ((line = reader.ReadLine()) != null)
                {
                    text += line.Trim();
                }

                return true;
            }
            return false;
        }

        private bool ParseStackup(string stk_section)
        {
            // get layers
            string[] layers = stk_section.Split(new char[] { '(', ')' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string layer in layers)
            {
                if (layer.StartsWith("LAYER"))
                {
                    //get layer parameters
                    string s = layer.Substring(5);
                    string[] layer_params = s.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);

                    Dictionary<string, string> parMap = new Dictionary<string, string>();
                    ZLayerType zType = ZLayerType.Signal;
                    bool bPrepreg = false;
                    //get parameter values
                    /* ---------- patterns ------------------
                    (LAYER NAME="Top" DESCRIPTION="" TYPE=SIGNAL COLOR=255 FILL=2 VISIBLE=1 THICKNESS=3.556e-005 
                                 ER=1 TG=0 THC=393.693 PLATING=0 RB=1.724e-008 TC=0.00393 TTW=0.000254 DZ0=75 MATERIAL="" 
                                 ROUGH_TOP=EDEPOSITED ROUGH_BOT=EDEPOSITED_TREATED ETCHFACTOR=0.741 NARROWTOP=1)
                    (LAYER NAME="" DESCRIPTION="" TYPE=DIELECTRIC COLOR=0 FILL=2 VISIBLE=1 THICKNESS=0.000254 
                                 ER=4.3 TG=0.02 THC=0.3 CONFORMAL=0 PREPREG=1 MATERIAL="" ATCHMETAL=0)
 
                     (OPTIONS_USE_DIE_FOR_METAL=0, LOCK_ATTACHED_LAYER=0)
                     * ---COMMON
                     *  NAME
                     *  DESCRIPTION
                     *  TYPE
                     *  THICKNESS
                     *  ER              //Dielectric Constant
                     *  TG              //Loss Tangent
                     *  THC             //ThermalConductivity [?]
                     *  MATERIAL
                     * 
                     * ---METAL
                     * PLATING
                     * RB               //BulkRes [ohm*m]
                     * TC               //TCoef [1/Deg. Celcius]
                     * TTW              //TestWidth
                     * DZ0              //Impedance [ohm]
                     * ROUGH_TOP        //Rolled, Rolled_T, Eldep, Eldep_T, None *OR* Value[m]
                     * ROUGH_BOTTOM
                     * ETCHFACTOR
                     * NARROWTOP
                     * 
                     * ----DIELECTRIC
                     * CONFORMAL
                     * PREPREG
                     * ATCHMETAL
                     * 
                    ------------------------------------------*/
                    double w0 = 0, thick = 0;
                    foreach (string par in layer_params)
                    {
                        string[] pair = par.Split(new char[] { '=' }, StringSplitOptions.RemoveEmptyEntries);
                        string name = pair[0];
                        string value = pair[1].Trim(new char[] { '"' });

                        switch (name)
                        {
                            case "NAME":
                                parMap.Add(ZStringConstants.ParameterIDLayerName, value);
                                break;
                            case "DESCRIPTION":
                                parMap.Add(ZStringConstants.ParameterIDDescription, value);
                                break;
                            case "TYPE":
                                switch (value)
                                {
                                    case "SIGNAL":
                                        zType = ZLayerType.Signal;
                                        break;
                                    case "PLANE":
                                        zType = ZLayerType.Plane;
                                        break;
                                    case "DIELECTRIC":
                                        zType = ZLayerType.Core; //default, value depends on the PREPREG field
                                        break;
                                }
                                break;
                            case "THICKNESS":
                                {
                                    //convert from meters to mils
                                    double val = Convert.ToDouble(value);
                                    thick = val;
                                    val *= Units.MetersToMils;
                                    parMap.Add(ZStringConstants.ParameterIDThickness, val.ToString());
                                }
                                break;
                            case "ER":
                                parMap.Add(ZStringConstants.ParameterIDZo_DielectricConstant, value);
                                break;
                            case "TG":
                                parMap.Add(ZStringConstants.ParameterIDZo_LossTangent, value);
                                break;
                            case "THC":
                                parMap.Add(ZStringConstants.ParameterIDThermalConductivity, value);
                                break;
                            case "PLATING":
                                {
                                    bool b = (value == "1");
                                    parMap.Add(ZStringConstants.ParameterIDPlating, b.ToString());
                                }
                                break;
                            case "RB":
                                parMap.Add(ZStringConstants.ParameterIDBulkRes, value);
                                break;
                            case "TC":
                                parMap.Add(ZStringConstants.ParameterIDThermalCoef, value);
                                break;
                            case "TTW":
                                {
                                    //convert from meters to mils
                                    double val = Convert.ToDouble(value);
                                    w0 = val;
                                    val *= Units.MetersToMils;
                                    parMap.Add(ZStringConstants.ParameterIDZo_TraceWidth, val.ToString());
                                    parMap.Add(ZStringConstants.ParameterIDZdiff_TraceWidth, val.ToString());
                                }
                                break;
                            case "DZ0":
                                parMap.Add(ZStringConstants.ParameterIDZo_Target, value);
                                break;
                            case "MATERIAL":
                                parMap.Add(ZStringConstants.ParameterIDMaterial, value);
                                break;
                            case "ROUGH_TOP":
                                {
                                    //convert from meters to mils
                                    double val = 0;
                                    try
                                    {
                                        val = Convert.ToDouble(value); //[m]
                                        val *= 1.0e6; //[um]
                                    }
                                    catch
                                    {//can be string
                                        val = 3.5; //default
                                        val *= RoughCoeff;//Units.RMSToRz;
                                    }

                                    parMap.Add(ZStringConstants.ParameterIDRoughTop, val.ToString());
                                }
                                break;
                            case "ROUGH_BOT":
                                {
                                    //convert from meters to mils
                                    double val = 0;
                                    try
                                    {
                                        val = Convert.ToDouble(value);//[m]
                                        val *= 1.0e6; //[um]
                                        val *= RoughCoeff;//Units.RMSToRz;
                                    }
                                    catch
                                    {//can be string
                                        val = 3.5; //[um]default
                                    }
                                    parMap.Add(ZStringConstants.ParameterIDRoughBottom, val.ToString());
                                }
                                break;
                            case "ETCHFACTOR":
                                //convert HL etch to Zo
                                if (w0 > 0 && thick > 0)
                                {
                                    double e = 0;
                                    try
                                    {
                                        e = Convert.ToDouble(value);
                                    }
                                    catch
                                    {
                                        e = 0.9;
                                    }
                                    double x = e * thick / 2;
                                    double eZo = (w0 - x) / (w0 + x);
                                    value = eZo.ToString("N2");
                                }
                                parMap.Add(ZStringConstants.ParameterIDEtchFactor, value);
                                break;
                            case "NARROWTOP":
                                {
                                    bool b = (value == "1");
                                    parMap.Add(ZStringConstants.ParameterIDNarrowTop, b.ToString());
                                }
                                break;
                            // dielectric specific
                            case "CONFORMAL":
                                {
                                    bool b = (value == "1");
                                    parMap.Add(ZStringConstants.ParameterIDConformal, b.ToString());
                                }
                                break;
                            case "PREPREG":
                                bPrepreg = (value == "1");
                                break;
                            case "ATCHMETAL":
                                {
                                    bool b = (value == "1");
                                    parMap.Add(ZStringConstants.ParameterIDAtchMetal, b.ToString());
                                }
                                break;
                        }
                    }

                    //create layer with default parameters
                    if (bPrepreg)
                    {
                        zType = ZLayerType.Prepreg;
                    }

                    string layerID = stackup.AddLayer(zType);
                    ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

                    //set actual parameters
                    foreach (KeyValuePair<string, string> kvp in parMap)
                    {
                        ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                        if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                    }

                    if (stackup.ActiveSingle != null)
                    {
                        zLayer = stackup.ActiveSingle.GetLayerOfSingleImpedance(layerID);
                        foreach (KeyValuePair<string, string> kvp in parMap)
                        {
                            ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                            if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                        }
                    }
                }
            }

            return true;
        }

        private bool Parse()
        {
            //get STACKUP section
            string [] sections = text.Split(new char[] { '{', '}' }, StringSplitOptions.RemoveEmptyEntries);
            string stk_section = "";
            string app_section = "";
            foreach (string section in sections)
            {
                if (section.StartsWith("STACKUP"))
                {
                    stk_section = section.Substring(7);
                    if (stk_section == "") continue;
                    ParseStackup(stk_section);
                }
                if (section.StartsWith("APPLICATION_SETTINGS"))
                {
                    app_section = section.Substring("APPLICATION_SETTINGS".Length);
                    if (app_section == "") continue;
                    ParseSettings(app_section);
                }
            }


            return true;
        }

        double RoughCoeff = 1.0; //Rz

        bool ParseSettings(string app_section)
        {
            string[] options = app_section.Split(new char[] { '(', ')' }, StringSplitOptions.RemoveEmptyEntries);
            foreach (string opt in options)
            {
                string[] pair = opt.Split(new char[] { '=' }, StringSplitOptions.RemoveEmptyEntries);
                string value = pair[1].Trim(new char[] { '"' });
                int iVal = 0;
                Int32.TryParse(value, out iVal);
                if (opt.StartsWith("ENABLE_ROUGHNESS"))
                {
                }
                else if (opt.StartsWith("RMS_VS_RA"))
                {
                    switch (iVal){
                        case 0: //0 - Rms
                            RoughCoeff = Units.RMSToRz;
                            break;
                        case 1: //1 - Ra
                            //! unknown!
                            break;
                        case 2://2 - Rz
                            RoughCoeff = 1.0;
                            break;
                    }
                }
                else if (opt.StartsWith("ROUGHNESS_MODEL"))
                {
                    //0 - Modified Hammerstad
                    //1 - Cannonball
                    //2 - Hammerstad
                }
                else if (opt.StartsWith("ROUGHNESS_FACTOR"))
                {

                }
                else if (opt.StartsWith("TRAPEZOIDAL_TRACE"))
                {

                }
            }
            return true;
        }

        private bool Translate(ZStackup stackup)
        {
            ReadAll();
            Parse();
            return true;
        }
    }
        //export
        //----------------------------------------------------------
    class HL_Exporter
    {
        private ZStackup stackup;
        private string OutputFile;
        private bool isFFS;

        public HL_Exporter(string file)
        {
            OutputFile = file;
            string ext = Path.GetExtension(OutputFile).Trim(new char[] { '.' });
            switch (ext.ToUpper())
            {
                case "STK":
                    {
                        isFFS = false;
                    }
                    break;
                case "FFS":
                    {
                        isFFS = true;
                    }
                    break;
            }
        }

        private string GetUniqueName(ZLayer layer)
        {
            string retval = "";
            string s = "";
            ZPlannerProject.GetLayerParameterValue(layer, ZStringConstants.ParameterIDLayerName, ref s);
            retval = s;

            if (!layer.isMetal()) return retval;

            ZStackup stk = layer.Stackup;
            int cnt = 0;
            foreach (ZLayer zL in stk.Layers)
            {
                if (zL.ID == layer.ID)
                {
                    if (cnt > 0)
                    {
                        retval += "_" + cnt.ToString();
                    }
                    return retval;
                }

                if (zL.isMetal())
                {
                    string name = "";
                    ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDLayerName, ref name);
                    if (name == s) cnt++;
                }
            }

            return retval;
        }

        private double GetPlaneTraceWidth(ZLayer zl)
        {
            ZStackup stk = zl.Stackup;
            int idx = stk.GetLayerOfStackupIndex(zl.ID);
            int delta = 1024;
            double retval = 0;

            //get trace width from closest signal/mixed layer
            foreach (ZLayer lay in stk.Layers)
            {
                ZLayerType? t = lay.GetLayerType();
                if (t == ZLayerType.Signal || t == ZLayerType.SplitMixed)
                {
                    int idx1 = stk.GetLayerOfStackupIndex(zl.ID);
                    double w = 0;
                    ZLayer singleEnded = stackup.ActiveSingle.GetLayerOfSingleImpedance(lay.ID);
                    ZPlannerProject.GetLayerParameterValue(singleEnded, ZStringConstants.ParameterIDZo_TraceWidth, ref w);
                    if (w > 0)
                    {
                        if (Math.Abs(idx - idx1) < delta)
                        {
                            delta = Math.Abs(idx - idx1);
                            retval = w;
                        }
                    }
                }
            }

            return retval;
        }

        private string GetLayerClause(ZLayer zL)
        {
            bool isMetal = false;
            bool isPrepreg = false;
            bool isSolderMask = false;

            string s = "(LAYER NAME=";
            string sval = "";
            //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDLayerName, ref sval);
            sval = GetUniqueName(zL);
            //replace spaces with underscore
            sval = sval.Replace(" ", "_");
            s += "\"" + sval +"\"";

            sval = "";
            ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDDescription, ref sval);
            s += " DESCRIPTION=\"" + sval + "\"";

            sval = "DIELECTRIC";
            ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDLayerType, ref sval);
            sval = sval.ToUpper();
            switch (sval)
            {
                case "SIGNAL":
                case "PLANE":
                    isMetal = true;
                    break;
                case "SPLIT/MIXED":
                    isMetal = true;
                    sval = "SIGNAL";
                    break;
                case "CORE":
                    sval = "DIELECTRIC";
                    isPrepreg = false;
                    break;
                case "SOLDER MASK":
                    isSolderMask = true;
                    sval = "DIELECTRIC";
                    isPrepreg = true;
                    break;
                case "PREPREG":
                    sval = "DIELECTRIC";
                    isPrepreg = true;
                    break;
            }
            s += " TYPE=" + sval;

            //default visibility parameters
            s += " COLOR=0";
            s += " FILL=2";
            s += " VISIBLE=1";
            //

            double thickness = 0;
            double traceWidth = 0;

            double dval = 0;
            ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDThickness, ref dval);
            thickness = dval;
            dval *= Units.MilsToMeters;

            s += " THICKNESS=" + dval.ToString();

            ZLayer zSingleLayer = zL.Stackup.ActiveSingle.GetLayerOfSingleImpedance(zL.ID);
            if (isMetal)
            {
                dval = 0;
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDZo_DielectricConstant, ref dval);
                string str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
                ZPlannerManager.GetFirstValueFromTable(str, out dval);
                if (Double.IsNaN(dval))
                {
                    dval = 1;
                }
                s += " ER=" + dval.ToString();

                dval = 0;
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDZo_LossTangent, ref dval);
                str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                ZPlannerManager.GetFirstValueFromTable(str, out dval);
                if (Double.IsNaN(dval))
                {
                    dval = 0;
                }
                s += " TG=" + dval.ToString();

                dval = 393.693; //default for all metal layers
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDThermalConductivity, ref dval);
                s += " THC=" + dval.ToString();


                bool bval = false;
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDPlating, ref bval);
                sval = "0";
                if (bval) sval = "1";
                s += " PLATING=" + sval;

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDBulkRes, ref dval);
                s += " RB=" + dval.ToString();

                dval = 0.00393; //default for all metal layers
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDThermalCoef, ref dval);
                s += " TC=" + dval.ToString();

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zSingleLayer, ZStringConstants.ParameterIDZo_TraceWidth, ref dval);
                traceWidth = dval;

                if (zL.GetLayerType() == ZLayerType.Plane)
                {
                    dval = GetPlaneTraceWidth(zL);
                }

                dval *= Units.MilsToMeters;
                s += " TTW=" + dval.ToString();

                dval = 0;
                switch (zL.GetLayerType())
                {
                    case ZLayerType.SplitMixed:
                    case ZLayerType.Signal:
                        ZPlannerProject.GetLayerParameterValue(zSingleLayer, ZStringConstants.ParameterIDZo_Zo, ref dval);
                        int ival = (int)Math.Floor(dval);
                        s += " DZ0=" + ival.ToString("D");
                    break;
                
                }

                sval = "";
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDMaterial, ref sval);
                s += " MATERIAL=\"" + sval + "\"";

                dval = 3.5;//[um] default
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDRoughTop, ref dval);
                if (dval == 0.0) ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDCalcRoughTop, ref dval);
                dval *= 1.0e-6; //[m]
                //--dval *= Units.RzToRMS;
                s += " ROUGH_TOP=" + dval.ToString();

                dval = 3.5;//[um] default
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDRoughBottom, ref dval);
                if (dval == 0.0) ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDCalcRoughBottom, ref dval);
                dval *= 1.0e-6; //[m]
                //--dval *= Units.RzToRMS;
                s += " ROUGH_BOT=" + dval.ToString();

                if (ZPlannerManager.IsTrapezoidalTraces)
                {
                    dval = 0;
                    ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDEtchFactor, ref dval);
                    double etch = dval;
                    //covert etch to HL
                    if (thickness > 0 && traceWidth > 0) {
                        double wWide = 2 * traceWidth / (1 + etch);
                        double wNarrow = wWide * etch;
                        double delta = wWide - wNarrow;
                        etch = delta / thickness;//Math.Min(0.9, delta / thickness);
                    }
                    s += " ETCHFACTOR=" + etch.ToString("N2");

                    bval = false;
                    ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDNarrowTop, ref bval);
                    sval = "0";
                    if (bval) sval = "1";
                    s += " NARROWTOP=" + sval;
                }
            }
            else
            {
                dval = 0;
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDZo_DielectricConstant, ref dval);
                string str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
                ZPlannerManager.GetFirstValueFromTable(str, out dval);
                s += " ER=" + dval.ToString();

                dval = 0;
                //--ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDZo_LossTangent, ref dval);
                str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                ZPlannerManager.GetFirstValueFromTable(str, out dval);
                s += " TG=" + dval.ToString();

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDThermalConductivity, ref dval);
                s += " THC=" + dval.ToString();

                bool bval = false;
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDConformal, ref bval);
                sval = "0";
                if (bval || isSolderMask) sval = "1";
                s += " CONFORMAL=" + sval;
                
                sval = "0";
                if (isPrepreg) sval = "1";
                s += " PREPREG=" + sval;

                sval = "";
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDMaterial, ref sval);
                s += " MATERIAL=\"" + sval + "\"";

                bval = false;
                ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDAtchMetal, ref bval);
                sval = "0";
                if (bval) sval = "1";
                s += " ATCHMETAL=" + sval;
            }


            s += ")";
            return s;
        }
        private string CreateStackupSection(ZStackup stackup)
        {
            string s = "{STACKUP" + Environment.NewLine;
            foreach (ZLayer zL in stackup.Layers)
            {
                s += "\t" + GetLayerClause(zL) + Environment.NewLine;
            }
            s += "}" + Environment.NewLine;
            return s;
        }
        public string CreateText_STK(ZStackup stackup)
        {
            string s = "{APPLICATION_SETTINGS" + Environment.NewLine;

            if (ZPlannerManager.IsRoughness)
            {
                s += "\t(ENABLE_ROUGHNESS=1)" + Environment.NewLine;
            }
            else
            {
                s += "\t(ENABLE_ROUGHNESS=0)" + Environment.NewLine;
            }
            s += "\t(RMS_VS_RA=2)" + Environment.NewLine;       //Rz
            s += "\t(ROUGHNESS_MODEL=2)" + Environment.NewLine; //Hammerstad
            s += "\t(ROUGHNESS_FACTOR=2)" + Environment.NewLine;

            if (ZPlannerManager.IsTrapezoidalTraces){
                s += "\t(TRAPEZOIDAL_TRACE=1)" + Environment.NewLine;
            }
            else
            {
                s += "\t(TRAPEZOIDAL_TRACE=0)" + Environment.NewLine;
            }
            s += "}" + Environment.NewLine;
            s += CreateStackupSection(stackup);
            return s;
        }

        public string CreateText_FFS(ZStackup stackup)
        {
            string s = "{FFS_FILE}" + Environment.NewLine;
            s += "{VERSION=3.0}" + Environment.NewLine;
            s += CreateStackupSection(stackup);
            s += "{END}" + Environment.NewLine;
            return s;
        }
        
        public bool Export()
        {
            stackup = ZPlannerManager.Project.Stackup;
            if (stackup == null) return false;

            if (isFFS)
            {
                return ExportFFS(stackup);
            }
            else
            {
                return ExportSTK(stackup);
            }
        }

        public bool ExportSTK(ZStackup stackup)
        {
            File.WriteAllText(OutputFile, CreateText_STK(stackup));
            return true;
        }
        public bool ExportFFS(ZStackup stackup)
        {
            File.WriteAllText(OutputFile, CreateText_FFS(stackup));

            //try to start HL
            HL_Launcher.Run(OutputFile);

            return true;
        }
    }

//-----------------------------------------------------------------------------

    class HL_Launcher
    {
        [DllImport("Shlwapi.dll", CharSet = CharSet.Unicode)]
        public static extern uint AssocQueryString(AssocF flags, AssocStr str,
         string pszAssoc, string pszExtra, [Out]StringBuilder pszOut, ref uint pcchOut);

        public enum AssocStr
        {
            ASSOCSTR_COMMAND = 1,
            ASSOCSTR_EXECUTABLE,
            ASSOCSTR_FRIENDLYDOCNAME,
            ASSOCSTR_FRIENDLYAPPNAME,
            ASSOCSTR_NOOPEN,
            ASSOCSTR_SHELLNEWVALUE,
            ASSOCSTR_DDECOMMAND,
            ASSOCSTR_DDEIFEXEC,
            ASSOCSTR_DDEAPPLICATION,
            ASSOCSTR_DDETOPIC,
            ASSOCSTR_INFOTIP,
            ASSOCSTR_QUICKTIP,
            ASSOCSTR_TILEINFO,
            ASSOCSTR_CONTENTTYPE,
            ASSOCSTR_DEFAULTICON,
            ASSOCSTR_SHELLEXTENSION,
            ASSOCSTR_DROPTARGET,
            ASSOCSTR_DELEGATEEXECUTE,
            ASSOCSTR_SUPPORTED_URI_PROTOCOLS,
            ASSOCSTR_MAX
        }
        public enum AssocF
        {
            ASSOCF_NONE = 0x00000000,
            ASSOCF_INIT_NOREMAPCLSID = 0x00000001,
            ASSOCF_INIT_BYEXENAME = 0x00000002,
            ASSOCF_OPEN_BYEXENAME = 0x00000002,
            ASSOCF_INIT_DEFAULTTOSTAR = 0x00000004,
            ASSOCF_INIT_DEFAULTTOFOLDER = 0x00000008,
            ASSOCF_NOUSERSETTINGS = 0x00000010,
            ASSOCF_NOTRUNCATE = 0x00000020,
            ASSOCF_VERIFY = 0x00000040,
            ASSOCF_REMAPRUNDLL = 0x00000080,
            ASSOCF_NOFIXUPS = 0x00000100,
            ASSOCF_IGNOREBASECLASS = 0x00000200,
            ASSOCF_INIT_IGNOREUNKNOWN = 0x00000400,
            ASSOCF_INIT_FIXED_PROGID = 0x00000800,
            ASSOCF_IS_PROTOCOL = 0x00001000,
            ASSOCF_INIT_FOR_FILE = 0x00002000
        }

        static private string AssocQueryString(string extension)
        {
            const int S_OK = 0;

            uint length = 2048;

            var sb = new StringBuilder((int)length);
            uint ret = AssocQueryString(AssocF.ASSOCF_INIT_IGNOREUNKNOWN, AssocStr.ASSOCSTR_EXECUTABLE, extension, null, sb, ref length);
            if (ret != S_OK)
            {
                throw new InvalidOperationException("Could not determine associated string");
            }

            return sb.ToString();
        }

        private static string GetExecutable()
        {
            try
            {
                return AssocQueryString(".hyp");
            }
            catch
            {
                return "";
            }
        }

        public static void Run(string ffs_path)
        {
            ffs_path = "\"" + ffs_path + "\"";
            string exe = GetExecutable();
            if (exe != String.Empty)
            {
                Process hl = new Process();
                hl.StartInfo.FileName = exe;
                hl.StartInfo.Arguments = ffs_path;
                hl.Start();
            }
        }
    }
}