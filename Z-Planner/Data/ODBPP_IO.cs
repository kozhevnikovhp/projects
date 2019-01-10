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

using ICSharpCode.SharpZipLib.Tar;
using ICSharpCode.SharpZipLib.GZip;

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

    class ODBPP_Parser
    {
        private string odb_folder;
        private string matrixFile;
        private string step;
        private ZStackup stackup;

        //stackup structure
        class odbLayer
        {
            public string   name;
            public string   type;
            public bool     physical;
            public bool     core;
        }

        List<odbLayer> layerList;
        //

        public ODBPP_Parser(string folder)
        {
            odb_folder = folder;
            layerList = new List<odbLayer>();
        }

        public bool Import()
        {
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

            // parse file
            ParseStackup(stackup);

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


        private bool ParseStackup(ZStackup stackup)
        {
            if (ReadMatrix() && ReadLayers(step))
            {
                stackup.Title = step;
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

                string layerName = layer.name;
                string layerAttrFile = stepFolder + "/layers/" + layerName + "/" + "attrlist";
                if (!File.Exists(layerAttrFile)) return false;

                if (!ReadLayer(layerAttrFile, layer)) return false;
            }

            return true;
        }

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
                        dval = Convert.ToDouble(val);
                        dval *= coef;
                        parMap.Add(ZStringConstants.ParameterIDThickness, dval.ToString());

                        break;
                    case ".comment":
                        parMap.Add(ZStringConstants.ParameterIDDescription, val);
                        break;

                    //metal
                    case ".copper_weight":
                        if (bImperialUnits)
                        {
                            dval = Convert.ToDouble(val);
                            dval *= Units.fOzToMils;
                            parMap.Add(ZStringConstants.ParameterIDThickness, dval.ToString());
                        }
                        else
                        {
                            parMap.Add(ZStringConstants.ParameterIDThickness, val);
                        }
                        break;

                    case ".bulk_resistivity": //nano-ohm-m
                        dval = Convert.ToDouble(val);
                        dval *= 1.0e-9;
                        parMap.Add(ZStringConstants.ParameterIDBulkRes, dval.ToString());
                        break;
                    case ".z0impedance":
                        parMap.Add(ZStringConstants.ParameterIDZo_Target, val);
                        break;
                }
            }
            reader.Close();


            //----------------------------------------------------------
            //add and define stackup layer
            string layerID = stackup.AddLayer(zType);
            ZLayer zLayer = stackup.GetLayerOfStackup(layerID);

            //set actual parameters
            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }

            zLayer = stackup.ActiveSingle.GetLayerOfSingleImpedance(layerID);
            foreach (KeyValuePair<string, string> kvp in parMap)
            {
                ZLayerParameter layerParameter = zLayer.GetLayerParameter(kvp.Key);
                if (layerParameter != null) layerParameter.SetEditedValue(kvp.Value);
            }
            
            return true;
        }
    }

    class ODBPP_Exporter
    {
        ZStackup stackup;
        private string OutputFolder;
        private string odbpp_folder;

        public ODBPP_Exporter(string folder)
        {
            OutputFolder = folder;
        }

        public bool Export()
        {
            stackup = ZPlannerManager.Project.Stackup;
            if (stackup == null) return false;

            if (Export(stackup)){
                string fileName = OutputFolder + "/" + stackup.Title + ".tgz";

                DirectoryInfo parentDir = Directory.GetParent(odbpp_folder);
                int len = parentDir.FullName.Length + 1;

                TarGz zip = new TarGz();
                zip.Create(odbpp_folder, fileName, len);
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
            odbpp_folder = OutputFolder + "/" + stackup.Title + "_ODBPP";
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
            s += "JOB_NAME=" + stackup.Title + "_ODBPP" + Environment.NewLine;
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
            ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDDescription, ref sval);
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

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);
                dval *= Units.fMilToOz;
                s += ".copper_weight=" + dval.ToString() + Environment.NewLine;

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDBulkRes, ref dval);
                dval *= 1.0e9;
                s += ".bulk_resistivity=" + dval.ToString() + Environment.NewLine;

                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zSingleLayer, ZStringConstants.ParameterIDZo_Zo, ref dval);
                s += ".z0impedance=" + dval.ToString() + Environment.NewLine;
            }
            else
            {
                dval = 0;
                ZPlannerProject.GetLayerParameterValue(zl, ZStringConstants.ParameterIDThickness, ref dval);
                dval *= Units.MilsToInch;
                s += ".layer_dielectric=" + dval.ToString() + Environment.NewLine;
            }

            File.WriteAllText(file, s);
        }
    }
}

