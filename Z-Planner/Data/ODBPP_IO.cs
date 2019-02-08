using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Xml.Linq;
using ZZero.ZPlanner;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI;
using ZZero.ZPlanner.UI.Dialogs;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

using System.Runtime.Serialization;
using System.Runtime.Serialization.Json;

using ICSharpCode.SharpZipLib.Tar;
using ICSharpCode.SharpZipLib.GZip;
using System.Collections.Specialized;

namespace ZZero.ZPlanner.Translators
{

    class TarGz
    {
        private string   rootFolder;
        private string   outputName;
        private int      rootLen;

        public TarGz()
        {
        }

        public void Create(string root, string name, int Len)
        {
            rootFolder = root;
            outputName = name;
            rootLen = Len;

            CreateTar(outputName, rootFolder);
        }

        public void Extract(String gzArchiveName, String destFolder)
        {    
    	    Stream inStream = File.OpenRead(gzArchiveName);
    	    Stream gzipStream = new GZipInputStream(inStream);
    
    	    TarArchive tarArchive = TarArchive.CreateInputTarArchive(gzipStream);
    	    tarArchive.ExtractContents(destFolder);
    	    tarArchive.Close();
    
    	    gzipStream.Close();
    	    inStream.Close();
        }

        // Creates a GZipped Tar file from a source directory
        // <param name="outputTarFilename">Output .tar.gz file</param>
        // <param name="sourceDirectory">Input directory containing files to be added to GZipped tar archive</param>
        private void CreateTar(string outputTarFilename, string sourceDirectory)
        {
            using (FileStream fs = new FileStream(outputTarFilename, FileMode.Create, FileAccess.Write, FileShare.None))
            using (Stream gzipStream = new GZipOutputStream(fs))
            using (TarArchive tarArchive = TarArchive.CreateOutputTarArchive(gzipStream))
            {
                AddDirectoryFilesToTar(tarArchive, sourceDirectory, true);
            }
        }

        // Recursively adds folders and files to archive
        // <param name="tarArchive"></param>
        // <param name="sourceDirectory"></param>
        // <param name="recurse"></param>
        private void AddDirectoryFilesToTar(TarArchive tarArchive, string sourceDirectory, bool recurse)
        {
            // Recursively add sub-folders
            if (recurse)
            {
                string[] directories = Directory.GetDirectories(sourceDirectory);
                foreach (string directory in directories)
                    AddDirectoryFilesToTar(tarArchive, directory, recurse);
            }

            // Add files
            string[] filenames = Directory.GetFiles(sourceDirectory);
            foreach (string filename in filenames)
            {
                TarEntry tarEntry = TarEntry.CreateEntryFromFile(filename);
                tarEntry.Name = filename.Remove(0, rootLen);
                tarArchive.WriteEntry(tarEntry, true);
            }
        }
    }

    class ODBPP_Parser : IImport
    {
        private string odb_folder;
        private string matrixFile;
        private string step;
        private ZStackup stackup;
        bool bValid;

        //stackup structure
        class odbLayer
        {
            public string   name;
            public string   type;
            public bool     physical;
            public bool     core;
        }

        List<odbLayer> layerList;
        private bool bPressedThickness;private ZLibraryCategory[] libraryPriorityArray;

        public ODBPP_Parser(string folder, bool bPressed = false, ZLibraryCategory[] libraryPriority = null)
        {
            odb_folder = folder;
            if (File.Exists(folder))
            {
                string ext = Path.GetExtension(folder);
                bValid = ext.ToLower() == ".tgz";

                if (bValid)
                {
                    try
                    {
                        //this is TGZ file
                        string tgz = folder;

                        string path = Path.GetRandomFileName();
                        DirectoryInfo dirInfo = Directory.CreateDirectory(Path.Combine(Path.GetTempPath(), path));
                        string tempDir = dirInfo.FullName;

                        TarGz zip = new TarGz();
                        zip.Extract(tgz, tempDir);

                        string[] dirs = Directory.GetDirectories(tempDir);
                        odb_folder = dirs[0];
                    }
                    catch(Exception e)
                    {
                        bValid = false;
                        throw e;
                    }
                }
            }

            if (Directory.Exists(folder))
                bValid = true;

            bPressedThickness = bPressed;
            libraryPriorityArray = libraryPriority;
            layerList = new List<odbLayer>();
        }

        public bool Convert()
        {
            ZPlannerManager.StatusMenu.UpdateProgress();
            return true; //(Directory.Exists(filePath) || Path.GetExtension(filePath).ToUpper() == ".TGZ" && File.Exists(filePath));
        }

        public bool IsValidFile()
        {
            return bValid;
        }

        public bool Import()
        {
            ZPlannerManager.StatusMenu.UpdateProgress();
            ZPlannerManager.SuspendFSHandlers();

            ZPlannerProject project = new ZPlannerProject();

            // create stackup
            stackup = new ZStackup(project);
            //--stackup.Title = Path.GetFileNameWithoutExtension(InputFile);
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

            project.Stackup = stackup;
            ZPlannerManager.Project = project;

            // parse file
            ParseStackup(stackup);

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


        private bool ParseStackup(ZStackup stackup)
        {
            if (ReadMatrix() && ReadLayers(step))
            {
                stackup.Title = step;
                SetTargetImpedances();
                UpdateUsed();
                return true;
            }

            return false;
        }

        private bool ReadMatrix()
        {
            matrixFile = Path.Combine(odb_folder, "matrix" + Path.DirectorySeparatorChar + "matrix");
            if (!File.Exists(matrixFile)) return false;
            
            StreamReader reader = File.OpenText(matrixFile);
            string line;
            bool bStep = false;
            odbLayer layer = null;
            while ((line = reader.ReadLine()) != null)
            {
                line = line.Trim();
                //skip empty line
                if (line.Length == 0) continue;
                //skip comment
                if (line[0] == '#') continue;

                //get the first word in line
                string[] words = line.Split(new char[] { ' ', '=' }, StringSplitOptions.RemoveEmptyEntries);
                string theWord = words[0];
                
                string val = "";
                if (words.Length > 1){
                    val = line.Remove(0, theWord.Length + 1);
                    val = val.Trim();
                }

                switch (theWord)
                {
                    case "STEP":
                        bStep = true;
                        break;
                    case "LAYER":
                        layer = new odbLayer();
                        layer.physical = false;
                        layer.core = false;
                        layerList.Add(layer);
                        break;
                    case "}":
                        if (bStep) bStep = false;
                        break;
                    //---------
                    case "NAME": //can be step name or layer name
                        if (bStep)
                        {
                            step = val;
                        }
                        else
                        {
                            layer.name = val;
                        }
                        break;

                    case "TYPE":
                        switch (val)
                        {
                            case "SIGNAL":
                            case "POWER_GROUND":
                            case "MIXED":
                            case "SOLDER_MASK":
                            case "DIELECTRIC":
                                layer.physical = true;
                                layer.type = val;
                                break;
                        }
                        break;
                    case "DIELECTRIC_TYPE":
                        switch (val)
                        {
                            case "PREPREG" :
                                layer.core = false;
                                break;
                            case "CORE":
                                layer.core = true;
                                break;
                        }
                        break;
                }
            }

            ZPlannerManager.StatusMenu.UpdateProgress();

            return true;
        }

        private bool ReadLayers(string step = "") //the first step by default
        {
            string stepsFolder = Path.Combine(odb_folder, "steps");
            if (!Directory.Exists(stepsFolder)) return false;

            string stepFolder = "";
            if (step.Length > 0)
            {
                stepFolder = Path.Combine(stepsFolder, step);
                if (!Directory.Exists(stepFolder)) return false;
            }
            else
            { //get the first step folder
                DirectoryInfo directory = new DirectoryInfo(stepsFolder);
                DirectoryInfo[] subdirs = directory.GetDirectories();
                if (subdirs.Length == 0) return false;

                stepFolder = subdirs[0].FullName;
            }

            string layersFolder = stepFolder + Path.DirectorySeparatorChar + "layers";
            if (!Directory.Exists(layersFolder)) return false;
                
            foreach (odbLayer layer in layerList)
            {
                if (!layer.physical) continue;

                ZPlannerManager.StatusMenu.UpdateProgress();

                string layerName = layer.name;
                string layerAttrFile = stepFolder + "/layers/" + layerName + "/" + "attrlist";
                if (!File.Exists(layerAttrFile)) return false;

                if (!ReadLayer(layerAttrFile, layer)) return false;
            }

            return true;
        }

        class ImpedanceData
        {
            public bool bSingle;
            public string title;
            public double dk;
            public double df;
            public string ref1, ref2;

            public double frequency;
            public double width, spacing;
            public double z0, zdiff;

            private void Init()
            {
              bSingle = true;
              title = "";
              dk = 0;
              df = 0;
              ref1 = ref2 = "";
              width = spacing = 0;
              z0 = zdiff = 0;
              frequency = 0;
            }

            private string Unquote(string s)
            {
                return s.Trim('"');
            }
            private void Parse(string s)
            {
                string str = s.TrimStart('{').TrimEnd('}');
                string[] pairs = str.Split(',');
                foreach (string pair in pairs)
                {
                    string[] x = pair.Split(':');
                    string name = Unquote(x[0]);
                    string val = Unquote(x[1]);

                    switch (name)
                    {
                        case "name":
                            title = val;
                            break;
                        case "width":
                            Double.TryParse(val, out width);
                            break;
                        case "spacing":
                            Double.TryParse(val, out spacing);
                            break;
                        case "ref1":
                            ref1 = val;
                            break;
                        case "ref2":
                            ref2 = val;
                            break;
                        case "dk":
                            Double.TryParse(val, out dk);
                            break;
                        case "df":
                            Double.TryParse(val, out df);
                            break;
                        case "z0":
                            Double.TryParse(val, out z0);
                            break;
                        case "zdiff":
                            Double.TryParse(val, out zdiff);
                            break;
                        case "frequency":
                            Double.TryParse(val, out frequency);
                            break;
                    }
                }
            }

            public ImpedanceData(bool single, string json)
            {
                Init();
                bSingle = single;
                Parse(json);
            }

            public void SetupLayer(ZLayer lay)
            {
                ZLayerParameter layerParameter = null;
                if (bSingle)
                {
                    if (lay.isMetal()){
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_TraceWidth);
                        if (layerParameter != null) layerParameter.SetEditedValue((width * Units.InchToMils).ToString());

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_TopReference);
                        if (layerParameter != null) layerParameter.SetEditedValue(ref1);

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_BottomReference);
                        if (layerParameter != null) layerParameter.SetEditedValue(ref2);

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_Zo);
                        if (layerParameter != null) layerParameter.SetEditedValue(z0.ToString());

                    }
                    else {
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_DielectricConstant);
                        if (layerParameter != null) layerParameter.SetEditedValue(dk.ToString());
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_LossTangent);
                        if (layerParameter != null) layerParameter.SetEditedValue(df.ToString());
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_Frequency);
                        if (layerParameter != null) layerParameter.SetEditedValue(frequency.ToString());
                    }
                }
                else //diff  pair
                {
                    if (lay.isMetal())
                    {
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceWidth);
                        if (layerParameter != null) layerParameter.SetEditedValue((width * Units.InchToMils).ToString());

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TraceSpacing);
                        if (layerParameter != null) layerParameter.SetEditedValue((spacing * Units.InchToMils).ToString());

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_TopReference);
                        if (layerParameter != null) layerParameter.SetEditedValue(ref1);

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_BottomReference);
                        if (layerParameter != null) layerParameter.SetEditedValue(ref2);

                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_Zdiff);
                        if (layerParameter != null) layerParameter.SetEditedValue(zdiff.ToString());
                    }
                    else
                    {
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_DielectricConstant);
                        if (layerParameter != null) layerParameter.SetEditedValue(dk.ToString());
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZdiff_LossTangent);
                        if (layerParameter != null) layerParameter.SetEditedValue(df.ToString());
                        layerParameter = lay.GetLayerParameter(ZStringConstants.ParameterIDZo_Frequency);
                        if (layerParameter != null) layerParameter.SetEditedValue(frequency.ToString());
                    }
                }
            }
        }

        // Layer Attributes:
        // All:
        // System: .dielectric_constant, .loss_tangent, .comment, .eda_layers
        //
        // Metal:
        // System: .copper_weight, .bulk_resistivity, .z0impedance
        // Custom: thickness, copper_fill, copper_foil, top_roughness, bottom_roughness, {<single>/z0_impedance, width, ref1, ref2} {<pair>/zdiff_impedance, width, spacing, ref1, ref2}
        //
        //Dielectric:
        //System: .layer_dielectric
        //Custom: material, glass_style, resin_content, {<single>/dielectric_constant, loss_tangent} {<pair>/dielectric_constant, loss_tangent}

        private bool ReadLayer(string layerFile, odbLayer layer)
        {
            StreamReader reader = File.OpenText(layerFile);
            Dictionary<string, string> parMap = new Dictionary<string, string>();
            ZLayerType zType = ZLayerType.Signal;

            switch (layer.type)
            {
                case "SIGNAL":
                    zType = ZLayerType.Signal;
                    break;
                case "POWER_GROUND":
                    zType = ZLayerType.Plane;
                    break;
                case "MIXED":
                    zType = ZLayerType.SplitMixed;
                    break;
                case "SOLDER_MASK":
                    zType = ZLayerType.SolderMask;
                    break;
                case "DIELECTRIC":
                    if (layer.core)
                    {
                        zType = ZLayerType.Core;
                    }
                    else
                    {
                        zType = ZLayerType.Prepreg;
                    }
                    break;
            }

            string layerID = stackup.AddLayer(zType);

            parMap.Add(ZStringConstants.ParameterIDLayerName, layer.name);

            double coef = Units.InchToMils;

            string line;
            while ((line = reader.ReadLine()) != null)
            {
                line = line.Trim();
                //skip empty line
                if (line.Length == 0) continue;
                //skip comment
                if (line[0] == '#') continue;

                //get the first word in line
                string[] words = line.Split(new char[] {'=' }, StringSplitOptions.RemoveEmptyEntries);
                string theWord = words[0];
                string val = words.Length > 1 ? words[1] : "";

                double dval = 0;
                bool bImperialUnits = true;
                switch (theWord)
                {
                    case "UNITS":
                        switch (val)
                        {
                            case "MM":
                                coef = Units.MilimeterToMils;
                                bImperialUnits = false;
                                break;
                            case "INCH":
                                coef = Units.InchToMils;
                                bImperialUnits = true;
                                break;
                        }
                        break;

                    //dielectric
                    case ".dielectric_constant":
                        parMap.Add(ZStringConstants.ParameterIDZo_DielectricConstant, val);
                        break;
                    case ".loss_tangent":
                        parMap.Add(ZStringConstants.ParameterIDZo_LossTangent, val);
                        break;
                    case ".layer_dielectric": //thickness
                        Double.TryParse(val, out dval);
                        dval *= coef;
                        parMap.Add(ZStringConstants.ParameterIDThickness, dval.ToString());
                        break;
                    case ".comment":
                        parMap.Add(ZStringConstants.ParameterIDComments, val);
                        break;

                    //metal
                    case ".copper_weight":
                        parMap.Add(ZStringConstants.ParameterIDCopperThickness, val);
                        break;

                    case ".bulk_resistivity": //nano-ohm-m
                        dval = System.Convert.ToDouble(val);
                        dval *= 1.0e-9;
                        parMap.Add(ZStringConstants.ParameterIDBulkRes, dval.ToString());
                        break;
                    case ".z0impedance":
                        parMap.Add(ZStringConstants.ParameterIDZo_Target, val);
                        break;

                    //custom attributes
                    case "thickness":
                        Double.TryParse(val, out dval);
                        dval *= coef;
                        parMap.Add(ZStringConstants.ParameterIDThickness, dval.ToString());
                        break;
                    case "copper_fill":
                        parMap.Add(ZStringConstants.ParameterIDCopperPercent, val);
                        break;
                    case "copper_foil":
                        parMap.Add(ZStringConstants.ParameterIDFoilTreatment, val);
                        break;
                    case "top_roughness":
                        parMap.Add(ZStringConstants.ParameterIDRoughTop, val);
                        break;
                    case "bottom_roughness":
                        parMap.Add(ZStringConstants.ParameterIDRoughBottom, val);
                        break;
                    case "material":
                        parMap.Add(ZStringConstants.ParameterIDMaterial, val);
                        break;
                    case "glass_style":
                        parMap.Add(ZStringConstants.ParameterIDConstruction, val);
                        break;
                    case "resin_content":
                        parMap.Add(ZStringConstants.ParameterIDResinContent, val);
                        break;
                    case "single_ended":
                        {
                            ImpedanceData data = new ImpedanceData(true, val);
                            ZSingle single = stackup.Singles.Find(x => x.Title == data.title);
                            if (single == null)
                            {
                                single = new ZSingle(stackup);
                                single.Title = data.title;
                                stackup.Singles.Add(single);
                            }
                            ZLayer lSingle = single.GetLayerOfSingleImpedance(layerID);
                            data.SetupLayer(lSingle);                            
                        }
                        break;
                    case "diff_pair":
                        {
                            ImpedanceData data = new ImpedanceData(false, val);
                            ZPair pair = stackup.Pairs.Find(x => x.Title == data.title);
                            if (pair == null)
                            {
                                pair = new ZPair(stackup);
                                pair.Title = data.title;
                                stackup.Pairs.Add(pair);
                            }
                            ZLayer lPair = pair.GetLayerOfPairImpedance(layerID);
                            data.SetupLayer(lPair);
                        }
                        break;
                }
            }
            reader.Close();


            //----------------------------------------------------------
            //add and define stackup layer
            //--string layerID = stackup.AddLayer(zType);
            ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

            //set actual parameters
            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }

            if (stackup.Singles.Count == 1)
            {
                zLayer = stackup.ActiveSingle.GetLayerOfSingleImpedance(layerID);
                foreach (KeyValuePair<string, string> kvp in parMap)
                {
                    ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                    if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
                }
            }
            return true;
        }

        private void SetTargetImpedances()
        {
            char[] sep = { '~', ' '};
            foreach (ZSingle s in stackup.Singles)
            {
                string y = s.Title.Replace("Zo~", "");
                y = y.Replace("ohms", "");
                y = y.Trim();
                double z;
                if (Double.TryParse(y, out z))
                {
                    s.ImpedanceTarget = z;
                }
            }
            foreach (ZPair s in stackup.Pairs)
            {
                string y = s.Title.Replace("Zdiff~", "");
                y = y.Replace("ohms", "");
                y = y.Trim();
                double z;
                if (Double.TryParse(y, out z))
                {
                    s.ImpedanceTarget = z;
                }
            }

            ZSingle x = stackup.ActiveSingle;
            if (x != null  && x.ImpedanceTarget == 0 && stackup.Singles.Count > 1)
            {
                stackup.Singles.Remove(x);
                stackup.ActiveSingle = stackup.Singles[0];
            }
            ZPair p = stackup.ActivePair;
            if (p != null && p.ImpedanceTarget == 0 && stackup.Pairs.Count > 1)
            {
                stackup.Pairs.Remove(p);
                stackup.ActivePair = stackup.Pairs[0];
            }
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

    }

    class ODBPP_Exporter
    {
        ZStackup stackup;
        private string OutputFolder;
        private string odbpp_folder;
        private string tgzFile;

        public ODBPP_Exporter(string tgzFileName)
        {
            tgzFile = tgzFileName;
            OutputFolder = Path.GetDirectoryName(tgzFileName);
        }

        public bool Export()
        {
            stackup = ZPlannerManager.Project.Stackup;
            if (stackup == null) return false;

            if (Export(stackup)){
                //--string fileName = OutputFolder + "/" + stackup.Title + ".tgz";

                DirectoryInfo parentDir = Directory.GetParent(odbpp_folder);
                int len = parentDir.FullName.Length + 1;

                TarGz zip = new TarGz();
                zip.Create(odbpp_folder, tgzFile, len);
             return true;
            }

            return false;
        }

        private string GetUniqueName(ZLayer layer)
        {
            string retval = "";
            string s = "";
            ZPlannerProject.GetLayerParameterValue(layer, ZStringConstants.ParameterIDLayerName, ref s);
            if (s.Length == 0) //name by type
            {
                ZLayerType? lType = layer.GetLayerType();
                switch (lType)
                {
                    case ZLayerType.SolderMask : s = "SolderMask";
                    break;
                    case ZLayerType.Core: s = "Core";
                    break;
                    case ZLayerType.Prepreg: s = "Prepreg";
                    break;
                    case ZLayerType.Signal: s = "Signal";
                    break;
                    case ZLayerType.Plane: s = "Plane";
                    break;
                    case ZLayerType.SplitMixed: s = "Mixed";
                    break;
                }
                retval = s;
                //make unique
                ZStackup stk = layer.Stackup;
                int cnt = 0;
                foreach (ZLayer zL in stk.Layers)
                {
                    if (zL.ID == layer.ID)
                    {
                        retval += "_" + (cnt + 1).ToString();
                        return retval;
                    }

                    string name = "";
                    ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDLayerName, ref name);
                    if ((name.Length == 0) && (zL.GetLayerType() == lType)) cnt++;
                }
            }
            else {
                retval = s;
                //make unique
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

                    string name = "";
                    ZPlannerProject.GetLayerParameterValue(zL, ZStringConstants.ParameterIDLayerName, ref name);
                    if (name == s) cnt++;
                }
            }

            return retval;
        }

        private bool Export(ZStackup stackup)
        {
            // Creates minimal required ODBBPP folder/file structure. If file exists - overwrites it.

            //create design export folder
            string folder = Path.GetFileNameWithoutExtension(tgzFile);
            odbpp_folder = OutputFolder + "/" + folder /*stackup.Title*/ + "_ODB++";
            Directory.CreateDirectory(odbpp_folder);

            //create design/matrix
            string matrix_folder = odbpp_folder + "/matrix";
            Directory.CreateDirectory(matrix_folder);

            //create design/steps/<step>
            string step_folder = odbpp_folder + "/steps/" + stackup.Title;
            Directory.CreateDirectory(step_folder);

            //create design/steps/<step>/layers folder
            string layers_folder = step_folder + "/layers";
            Directory.CreateDirectory(layers_folder);

            //create design/fonts folder
            string fonts_folder = odbpp_folder + "/fonts";
            Directory.CreateDirectory(fonts_folder);
            
            //create design/misc folder
            string misc_folder = odbpp_folder + "/misc";
            Directory.CreateDirectory(misc_folder);


            //write required files

            string file = fonts_folder + "/standard";
            //copy - design/fonts/standard file
            string src = ZSettings.SettingsFolder + "/ODBPP/" + "ODBPP_fonts_standard";
            System.IO.File.Copy(src, file, true);

            //design misc/info file
            file = misc_folder + "/info";
            WriteInfo(file);

            //write design/matrix/matrix file
            file = matrix_folder + "/matrix";
            WriteMatrix(file);

            //for all layers
            //write design/steps/<step>/layers/layers/<layer>/attrlist file

            foreach (ZLayer zl in stackup.Layers)
            {
                string layer_folder = layers_folder + "/" + GetUniqueName(zl);
                Directory.CreateDirectory(layer_folder);

                file = layer_folder + "/attrlist";
                WriteLayer(file, zl);
            }

            return true;
        }

        private void WriteInfo(string file)
        {
            string s = "";
            s += "UNITS=INCH" + Environment.NewLine;
            s += "JOB_NAME=" + stackup.Title + "_ODB++" + Environment.NewLine;
            s += "ODB_VERSION_MAJOR=8" + Environment.NewLine;
            s += "ODB_VERSION_MINOR=0" + Environment.NewLine;
            s += "ODB_SOURCE=ZPLanner" + Environment.NewLine;
            DateTime theTime = DateTime.Now;
            string sNow = theTime.ToString("yyyyMMdd") + "." + theTime.ToString("HHmmss");
            s += "CREATION_DATE=" + sNow + Environment.NewLine; //yyyymmdd.hhmmss
            s += "SAVE_DATE=" + sNow + Environment.NewLine;
            s += "SAVE_APP=" + AboutData.TheAbout.sVersion + Environment.NewLine;
            s += "SAVE_USER=" + Options.TheOptions.UserName + Environment.NewLine;
            s += "MAX_UID=" + stackup.Layers.Count.ToString() + Environment.NewLine;

            File.WriteAllText(file, s);
        }

        private void WriteMatrix(string file)
        {
            string s = "";
            s += "STEP {" + Environment.NewLine;
            s += "\tCOL=1" + Environment.NewLine;
            s += "\tID=0" + Environment.NewLine;
            s += "\tNAME=" + stackup.Title + Environment.NewLine;
            s += "}" + Environment.NewLine;
            s += Environment.NewLine;

            int row = 1;
            foreach (ZLayer zl in stackup.Layers)
            {
                string DielType = "";
                string name = GetUniqueName(zl);
                ZLayerType? ztype = zl.GetLayerType();
                string type = "";
                switch (ztype)
                {
                    case ZLayerType.Signal :
                        type = "SIGNAL";
                        break;
                    case ZLayerType.Plane:
                        type = "POWER_GROUND";
                        break;
                    case ZLayerType.SplitMixed:
                        type = "MIXED";
                        break;
                    case ZLayerType.SolderMask:
                        type = "SOLDER_MASK";
                        break;
                    case ZLayerType.Prepreg:
                        type = "DIELECTRIC";
                        DielType = "PREPREG";
                        break;
                    case ZLayerType.Core:
                        type = "DIELECTRIC";
                        DielType = "CORE";
                        break;
                }

                s += "LAYER {" + Environment.NewLine;
                s += "\tROW=" + row.ToString() + Environment.NewLine;
                row++;
                s += "\tCONTEXT=BOARD" + Environment.NewLine;
                s += "\tTYPE=" + type + Environment.NewLine;
                if (DielType.Length > 0){
                    s += "\tDIELECTRIC_TYPE=" + DielType + Environment.NewLine;
                }
                s += "\tID=" + zl.ID + Environment.NewLine;
                s += "\tNAME=" + name + Environment.NewLine;
                s += "\tPOLARITY=POSITIVE" + Environment.NewLine;
                s += "\tSTART_NAME=" + Environment.NewLine;
                s += "\tEND_NAME=" + Environment.NewLine;
                s += "\tOLD_NAME=" + Environment.NewLine;
                s += "}" + Environment.NewLine;
                s += Environment.NewLine;
            }

            
            File.WriteAllText(file, s);
        }


        // Layer Attributes:
        // All:
        // System: .dielectric_constant, .loss_tangent, .comment, .eda_layers
        //
        // Metal:
        // System: .copper_weight, .bulk_resistivity, .z0impedance
        // Custom: thickness, copper_fill, copper_foil, top_roughness, bottom_roughness, {<single>/z0_impedance, width, ref1, ref2} {<pair>/zdiff_impedance, width, spacing, ref1, ref2}
        //
        //Dielectric:
        //System: .layer_dielectric
        //Custom: material, glass_style, resin_content, {<single>/dielectric_constant, loss_tangent} {<pair>/dielectric_constant, loss_tangent}

        private string Quote(string s)
        {
            return "\"" + s + "\"";
        }

        private void WriteLayer(string file, ZLayer zl)
        {
            ZLayer zSingleLayer = zl.Stackup.ActiveSingle.GetLayerOfSingleImpedance(zl.ID);

            string s = "";
            s += "UNITS=INCH" + Environment.NewLine;

            double dval = 0;
            string str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
            ZPlannerManager.GetFirstValueFromTable(str, out dval);
            if (Double.IsNaN(dval))
            {
                dval = 1;
            }
            s += ".dielectric_constant=" + dval.ToString() + Environment.NewLine;

            dval = 0;
            str = zSingleLayer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
            ZPlannerManager.GetFirstValueFromTable(str, out dval);
            if (Double.IsNaN(dval))
            {
                dval = 0;
            }
            s += ".loss_tangent=" + dval.ToString() + Environment.NewLine;

            string sval = "";
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDComments, ref sval);
            s += ".comment=" + sval + Environment.NewLine;

            s += ".eda_layers=" + GetUniqueName(zl) + Environment.NewLine;

            // metal
            if (zl.isMetal())
            {
                s += ".cu_base=NO" + Environment.NewLine;
                s += ".out_angle=0.0" + Environment.NewLine;
                s += ".out_comp=0" + Environment.NewLine;
                s += ".out_mirror=NO" + Environment.NewLine;
                s += ".out_polarity=Positive" + Environment.NewLine;
                s += ".out_x_scale=1" + Environment.NewLine;
                s += ".out_y_scale=1" + Environment.NewLine;
                s += ".layer_hdi_type=Core" + Environment.NewLine;

                //roughness
                double dVal = 0;
                string rTop = zl.GetLayerParameterValue(ZStringConstants.ParameterIDRoughTop);
                if (string.IsNullOrWhiteSpace(rTop)) rTop = zl.GetLayerParameterValue(ZStringConstants.ParameterIDCalcRoughTop);
                rTop = Double.TryParse(rTop, out dVal) ? dVal.ToString() : "";

                string rBot = zl.GetLayerParameterValue(ZStringConstants.ParameterIDRoughBottom);
                if (string.IsNullOrWhiteSpace(rBot)) rBot = zl.GetLayerParameterValue(ZStringConstants.ParameterIDCalcRoughBottom);
                rBot = Double.TryParse(rBot, out dVal) ? dVal.ToString() : "";

                s += "top_roughness=" + rTop + Environment.NewLine;
                s += "bottom_roughness=" + rBot + Environment.NewLine;

                //copper fill %
                string fill = zl.GetLayerParameterValue(ZStringConstants.ParameterIDCopperPercent);
                if (fill.Length > 0)
                {
                    s += "copper_fill=" + fill + Environment.NewLine;
                }
                //copper foil
                string foil = zl.GetLayerParameterValue(ZStringConstants.ParameterIDFoilTreatment);
                if (foil.Length > 0)
                {
                    s += "copper_foil=" + foil + Environment.NewLine;
                }

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);
                dval *= Units.MilsToInch;
                s += "thickness=" + dval.ToString() + Environment.NewLine;

                //copper weight
                string weight = zl.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness);
                if (weight.Length > 0)
                {
                    s += ".copper_weight=" + weight + Environment.NewLine;
                }

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDBulkRes, ref dval);
                dval *= 1.0e9;
                s += ".bulk_resistivity=" + dval.ToString() + Environment.NewLine;

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zSingleLayer, ZStringConstants.ParameterIDZo_Zo, ref dval);
                s += ".z0impedance=" + dval.ToString() + Environment.NewLine;

                foreach (ZSingle single in stackup.Singles)
                {
                    string title = single.Title;
                    ZLayer singleEnded = single.GetLayerOfSingleImpedance(zl.ID);

                    bool bUsed = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_IsUsed) == "true";
                    if (bUsed)
                    {
                        switch (zl.GetLayerType()) { 
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:
                                string wSingle = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TraceWidth);
                                wSingle = Double.TryParse(wSingle, out dVal) ? (dVal * Units.MilsToInch).ToString() : "";

                                string zSingle = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Zo);
                                zSingle = Double.TryParse(zSingle, out dVal) ? dVal.ToString("N2") : "";

                                string ref1 = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_TopReference);
                                string ref2 = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_BottomReference);

                                //custom attributes
                                string se = String.Format("single_ended={{\"name\":{0},\"width\":{1},\"ref1\":{2},\"ref2\":{3},\"z0\":{4}}}",Quote(title),wSingle,Quote(ref1),Quote(ref2),zSingle);
                                s += se + Environment.NewLine;
                                break;
                        }
                    }
                }

                foreach (ZPair pair in stackup.Pairs)
                {
                    string title = pair.Title;
                    ZLayer diffPair = pair.GetLayerOfPairImpedance(zl.ID);

                    bool bUsed = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed) == "true";
                    if (bUsed)
                    {
                        switch (zl.GetLayerType())
                        {
                            case ZLayerType.Signal:
                            case ZLayerType.SplitMixed:

                                string wDiff = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceWidth);
                                wDiff = Double.TryParse(wDiff, out dVal) ? (dVal * Units.MilsToInch).ToString() : "";

                                string sDiff = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TraceSpacing);
                                sDiff = Double.TryParse(sDiff, out dVal) ? (dVal * Units.MilsToInch).ToString() : "";

                                string zDiff = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Zdiff);
                                zDiff = Double.TryParse(zDiff, out dVal) ? (dVal).ToString("N2") : "";

                                string ref1 = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_TopReference);
                                string ref2 = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_BottomReference);

                                //custom attributes
                                string dp = String.Format("diff_pair={{\"name\":{0},\"width\":{1},\"spacing\":{2},\"ref1\":{3},\"ref2\":{4},\"zdiff\":{5}}}", Quote(title), wDiff, sDiff, Quote(ref1), Quote(ref2), zDiff);
                                s += dp + Environment.NewLine;
                                break;
                        }
                    }
                }

            }
            else
            {
                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);
                dval *= Units.MilsToInch;
                s += ".layer_dielectric=" + dval.ToString() + Environment.NewLine;

                //material, type, glass_style, resin_content
                if (zl.GetLayerType() != ZLayerType.SolderMask)
                {
                    string materialName = zl.GetLayerParameterValue(ZStringConstants.ParameterIDMaterial);
                    string construction = zl.GetLayerParameterValue(ZStringConstants.ParameterIDConstruction);
                    string resin = zl.GetLayerParameterValue(ZStringConstants.ParameterIDResinContent);

                    s += "material=" + materialName + Environment.NewLine;
                    s += "glass_style=" + construction + Environment.NewLine;
                    s += "resin_content=" + resin + Environment.NewLine;
                }

                foreach (ZSingle single in stackup.Singles)
                {
                    string title = single.Title;
                    ZLayer singleEnded = single.GetLayerOfSingleImpedance(zl.ID);

                    string dk = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_DielectricConstant);
                    double dk_dbl = 0;
                    double df_dbl = 0;
                    string sDk = "", sDf = "";
                    if (ZPlannerManager.GetFirstValueFromTable(dk, out dk_dbl))
                    {
                        sDk = dk_dbl.ToString();
                    }

                    string df = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_LossTangent);
                    if (ZPlannerManager.GetFirstValueFromTable(df, out df_dbl))
                    {
                        sDf = df_dbl.ToString();
                    }

                    string sf = singleEnded.GetLayerParameterValue(ZStringConstants.ParameterIDZo_Frequency);

                    string se = String.Format("single_ended={{\"name\":{0},\"dk\":{1},\"df\":{2},\"frequency\":{3}}}", Quote(title), sDk, sDf, sf);
                    s += se + Environment.NewLine;

                }

                foreach (ZPair pair in stackup.Pairs)
                {
                    string title = pair.Title;
                    ZLayer diffPair = pair.GetLayerOfPairImpedance(zl.ID);

                    string dk = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_DielectricConstant);
                    double dk_dbl = 0;
                    double df_dbl = 0;
                    string sDk = "", sDf = "";
                    if (ZPlannerManager.GetFirstValueFromTable(dk, out dk_dbl))
                    {
                        sDk = dk_dbl.ToString();
                    }

                    string df = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_LossTangent);
                    if (ZPlannerManager.GetFirstValueFromTable(df, out df_dbl))
                    {
                        sDf = df_dbl.ToString();
                    }

                    string sf = diffPair.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_Frequency);

                    string dp = String.Format("diff_pair={{\"name\":{0},\"dk\":{1},\"df\":{2},\"frequency\":{3}}}", Quote(title), sDk, sDf,sf);
                    s += dp + Environment.NewLine;

                }
            }

            File.WriteAllText(file, s);
        }
    }
}

