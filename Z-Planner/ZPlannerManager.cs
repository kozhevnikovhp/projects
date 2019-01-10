using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Net.Http;
using System.Net.Mail;
using System.Reflection;
using System.Resources;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Xml.Linq;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Commands;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Provider;
using ZZero.ZPlanner.FXInterface;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Translators;
using ZZero.ZPlanner.UI;
using ZZero.ZPlanner.UI.Dialogs;
using ZZero.ZPlanner.UI.EMail;
using ZZero.ZPlanner.UI.Grid;
using ZZero.ZPlanner.UI.Menu;
using ZZero.ZPlanner.UI.Recent;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner
{
    public static class ZPlannerManager
    {
        private static DeserializeDockContent deserializeDockContent = new DeserializeDockContent(GetContentFromPersistString);
        private static CommandManager commandManager;
        private static ZPlannerContainerPanel containerPanel;
        private static ZPlannerPropertiesPanel propertiesPanel;
        private static ZPlannerProjectPanel projectPanel;
        private static ZPlannerDMLPanel dmlPanel;
        private static ZPlannerMessagePanel messagePanel;
        private static ZPlannerStartPanel startPanel;
        private static ZPlannerStackupPanel stackupPanel;
        private static DmlSearchDialog dmlSearchDialog;
        private static ZPlannerProject project;
        private static ZMaterialLibrary dml;
        private static string projectFile;
        private static FXSandboxPanel fxSandboxPanel;
        private static DRCPanel drcPanel;
        private static DiffToolDialog diffPanel;
        private static ProgramMenu mainRibbonMenu = null;
        private static RibbonTab activeMenuTab = null;
        
        static ZPlannerManager()
        {
            Clipboard = new Dictionary<string, List<ZEntity>>();
            IsViasVisible = false;
            IsSinglesVisible = true;
            IsPairsVisible = true;
            IsHeadersVisible = true;
            IsCenterLineVisible = true;
            IsColorDisabledCells = false;

            ZPlannerManager.IsAutoMirror = false;
            ZPlannerManager.IsRoughness = false;
            ZPlannerManager.IsTrapezoidalTraces = true;
            ZPlannerManager.IsCoreLocked = false;
            ZPlannerManager.IsPressedThickness = false;
            ZPlannerManager.IsCorporateDeltaLoaded = false;
        }

        internal static string ProjectFileToBeOpened {get; set;}
        internal static string LibraryFileToBeOpened {get; set;}

        private static string zplannerHomeDirectory = string.Empty;
        internal static string ZPlannerHomeDirectory
        {
            get 
            { 
                if (string.IsNullOrEmpty(zplannerHomeDirectory)) 
                {
                    //zplannerHomeDirectory = FileAssociation.GetWorkDirectory();
                    zplannerHomeDirectory = AppDomain.CurrentDomain.BaseDirectory;
                } 
                return zplannerHomeDirectory; 
            }
        }

        private static string zplannerDataDirectory = string.Empty;
        internal static string ZPlannerDataDirectory
        {
            get 
            { 
                if (string.IsNullOrEmpty(zplannerDataDirectory)) 
                {
                    zplannerDataDirectory = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), "Z-zero", "Z-planner");
                }

                // TEMP for dev/test environment;
                if (ZSettings.UseLocalSettings || !Directory.Exists(zplannerDataDirectory)) return ZPlannerHomeDirectory;

                return zplannerDataDirectory; 
            }
        }

        internal static string ProjectFile
        {
            get { return projectFile; }
            set { projectFile = value; NotifyPropertyChanged("ProjectFile"); }
        }

        internal static ZPlannerProject Project 
        {
            get 
            { 
                if (project == null) 
                {
                    CreateNewProject(); 
                    NotifyPropertyChanged("Project"); 
                } 
                return project; 
            }
            set 
            { 
                if (value == null) ZPlannerManager.GoToHomeMenuTab();
                    
                if (project != null) NotifyNeedToClear("ContainerPanel"); 
                project = value;
                if (project != null) NotifyPropertyChanged("Project");
                else NotifyPropertyChanged("ProjectIsNull");
            }
        }

        internal static bool ProjectIsEmpty { get { return project == null; } }

        internal static bool IsIgnoreCollectionChanged { get; set; }

        internal static bool IsIgnoreUpdateActiveStackup { get; set; }

        internal static bool IsCorporateDeltaLoaded { get; set; }

        internal static string CategoryFilter { get; set; }

        internal static void SuspendCollectionChangedEvent()
        {
            IsIgnoreCollectionChanged = true;
        }

        internal static void ResumeCollectionChangedEvent()
        {
            IsIgnoreCollectionChanged = false;
        }

        internal static bool SuspendUpdateActiveStackupEvent()
        {
            bool oldValue = IsIgnoreUpdateActiveStackup;
            IsIgnoreUpdateActiveStackup = true;
            return oldValue;
        }

        internal static void ResumeUpdateActiveStackupEvent(bool isIgnore = false)
        {
            IsIgnoreUpdateActiveStackup = isIgnore;
        }

        internal static bool IsIgnorePropertyChanged { get; set; }

        internal static void SuspendPropertyChangedEvent()
        {
            IsIgnorePropertyChanged = true;
        }

        internal static void ResumePropertyChangedEvent()
        {
            IsIgnorePropertyChanged = false;
        }

        internal static bool IsIgnoreMenuButtonClicked { get; set; }

        internal static void SuspendMenuButtonClickedEvent()
        {
            IsIgnoreMenuButtonClicked = true;
        }

        internal static void ResumeMenuButtonClickedEvent()
        {
            IsIgnoreMenuButtonClicked = false;
        }

        internal static bool IsIgnoreMenuButtonChecked { get; set; }

        internal static void SuspendMenuButtonCheckedEvent()
        {
            IsIgnoreMenuButtonChecked = true;
        }

        internal static void ResumeMenuButtonCheckedEvent()
        {
            IsIgnoreMenuButtonChecked = false;
        }

        private static bool isLayerSelected;
        internal static bool IsLayerSelected
        {
            get { return isLayerSelected; }
            set { isLayerSelected = value; if (isLayerSelected) IsMaterialSelected = false; }
        }

        private static bool isMaterialSelected;
        internal static bool IsMaterialSelected
        {
            get { return isMaterialSelected; }
            set { isMaterialSelected = value; if (isMaterialSelected) IsLayerSelected = false; }
        }

        private static bool isAutoMirror = false;
        internal static bool IsAutoMirror 
        {
            get
            {
                return isAutoMirror;
            }
            set
            {
                if (isAutoMirror != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsAutoMirror", isAutoMirror, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isAutoMirror = value;
                    NotifyPropertyChanged("IsAutoMirror");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isRoughness;
        internal static bool IsRoughness 
        { 
            get
            {
                return isRoughness;
            }
            set
            {
                if (isRoughness != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsRoughness", isRoughness, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isRoughness = value;
                    NotifyPropertyChanged("IsRoughness");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isTrapezoidalTraces;
        internal static bool IsTrapezoidalTraces
        {
            get
            {
                return isTrapezoidalTraces;
            }
            set
            {
                if (isTrapezoidalTraces != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsTrapezoidalTraces", isTrapezoidalTraces, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isTrapezoidalTraces = value;
                    NotifyPropertyChanged("IsTrapezoidalTraces");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isPressedThickness;
        internal static bool IsPressedThickness
        {
            get
            {
                return isPressedThickness;
            }
            set
            {
                if (isPressedThickness != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsPressedThickness", isPressedThickness, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isPressedThickness = value;
                    NotifyPropertyChanged("IsPressedThickness");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isSequentialLamination;
        internal static bool IsSequentialLamination
        {
            get
            {
                return isSequentialLamination;
            }
            set
            {
                if (isSequentialLamination != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsSequentialLamination", isSequentialLamination, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isSequentialLamination = value;
                    NotifyPropertyChanged("IsSequentialLamination");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isGlassPitch;
        internal static bool IsGlassPitch
        {
            get
            {
                return isGlassPitch;
            }
            set
            {
                //if (isGlassPitch != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsGlassPitch", isGlassPitch, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isGlassPitch = value;
                    NotifyPropertyChanged("IsGlassPitch");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isGws;
        internal static bool IsGws
        {
            get
            {
                return isGws;
            }
            set
            {
                //if (isGws != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsGws", isGws, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isGws = value;
                    NotifyPropertyChanged("IsGws");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isLossPlanning;
        internal static bool IsLossPlanning
        {
            get
            {
                return isLossPlanning;
            }
            set
            {
                if (isLossPlanning != value)
                {
                    if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsLossPlanning", isLossPlanning, value);
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isLossPlanning = value;
                    NotifyPropertyChanged("IsLossPlanning");
                    ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }
        
        private static bool isLibraryLocked = false;
        internal static bool IsLibraryLocked 
        { 
            get 
            { 
                return isLibraryLocked; 
            } 
            set 
            { 
                if (isLibraryLocked != value)
                {
                    isLibraryLocked = value;
                    if (ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.LockLibrary(isLibraryLocked);
                }
            } 
        }

        internal static RibbonTab ActiveMenuTab 
        { 
            get
            {
                return activeMenuTab;
            }
            set
            {
                if (value == activeMenuTab) return;

                string oldMenuTabValue = (activeMenuTab != null) ? activeMenuTab.Value : null;
                activeMenuTab = value;

                switch(value.Value)
                {
                    case "Home":
                        if (oldMenuTabValue != null && oldMenuTabValue != "Library") SaveLayoutForTab(oldMenuTabValue);
                        /*if (!OpenLayoutForTab(activeMenuTab.Value)) OpenDefaultLayoutForTab(activeMenuTab.Value);*/
                        break;
                    case "Stackup":
                        //if (oldMenuTabValue != null && oldMenuTabValue != "Library") SaveLayoutForTab(oldMenuTabValue);
                        bool isShouldBeUpdated = false;
                        if (stackupPanel == null) 
                        {
                            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                            try
                            {
                                ZPlannerManager.CreateNewProject();
                                isShouldBeUpdated = true;
                            }
                            finally
                            {
                                ZPlannerManager.ResumeUpdateActiveStackupEvent();
                            }
                        }
                        if (!OpenLayoutForTab(activeMenuTab.Value)) OpenDefaultLayoutForTab(activeMenuTab.Value);
                        if (isShouldBeUpdated) ZPlannerManager.UpdateActiveStackup();
                        break;
                    case "Library":
                        if (oldMenuTabValue != null) SaveLayoutForTab(oldMenuTabValue);
                        OpenDefaultLayoutForTab(activeMenuTab.Value);
                        break;
                }
            }
        }

        internal static void GoToStackupMenuTab()
        {
            ZPlannerManager.MainMenu.SetActiveTabByValue("Stackup");
        }

        internal static void GoToHomeMenuTab()
        {
            ZPlannerManager.MainMenu.SetActiveTabByValue("Home");
        }

        internal static void GoToLibraryMenuTab()
        {
            ZPlannerManager.MainMenu.SetActiveTabByValue("Library");
        }

        internal static DeserializeDockContent DeserializeDockContent { get { return deserializeDockContent; } }

        internal static ZMaterialLibrary Dml
        {
            get
            {
                if (dml == null)
                {
                    SyncronizeDML(true);

                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        //Zo library                
                        string sZzero = (!string.IsNullOrWhiteSpace(LibraryFileToBeOpened) && File.Exists(LibraryFileToBeOpened)) ? LibraryFileToBeOpened : ZSettings.DMLFile;
                        if (!String.IsNullOrEmpty(sZzero) && File.Exists(sZzero))
                        {
                            dml = DataProvider.Instance.OpenZMaterialLibrary(sZzero);
                            //set Category to Z-zero
                            foreach (ZMaterial zm in dml.Materials)
                            {
                                zm.SetMaterialCategory(ZLibraryCategory.ZZero);
                            }
                        }

                        //corporate library
                        string sCorporate = Options.TheOptions.DML_NetworkPath;
                        if (!String.IsNullOrEmpty(sCorporate))
                        {
                            LoadLocalDml(sCorporate, ZLibraryCategory.Corporate);
                        }

                        //local library
                        string sLocal = Options.TheOptions.DML_LocalPath;
                        if (!String.IsNullOrEmpty(sLocal))
                        {
                            LoadLocalDml(sLocal);
                        }

                        dml.IsEdited = false;
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
                return dml;
            }
        }

//---------------------------------------------------------------------------------------
// saving local materials to DML
// SaveAs: false - overwrites library in the corresponding location, true - asks to specify location
//
        internal static void SaveDml(ZLibraryCategory category, bool SaveAs = false)
        {
            if (category == ZLibraryCategory.ZZero && !ZPlannerManager.rights.AllowDmlSave) return;
            // define saving location
            string  dmlFile;
            bool bOK = GetDmlFile(category, !SaveAs, out dmlFile);
            if (bOK)
            {
                int count;
                string sLib = "<DMLDocument>" + Environment.NewLine; 
                // write dml file (encrypt if required)
                sLib += GetSelectLists();
                sLib += "<DMLData>" + Environment.NewLine;
                sLib += GetProperties();
                sLib += GetMaterials(category, out count);
                sLib += "</DMLData>" + Environment.NewLine;
                sLib += "</DMLDocument>";

                if (count > 0)
                {
                    //encrypt
                    if (category == ZLibraryCategory.ZZero)
                    {
                        //--cLightEncryptor.EncyptToFile(dmlFile, sLib);
                    }
                    else
                    {
                        File.WriteAllText(dmlFile, sLib);
                        // write delta file if required
                    }
                }

                SaveZoDelta(dmlFile);
                Dml.IsEdited = false;
            }
        }

        internal static void OpenLocalDml()
        {

            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "Dielectric Material Local Library files (*.xml)|*.xml";
            openFileDialog.FilterIndex = 1;
            openFileDialog.InitialDirectory = Options.TheOptions.DML_LocalPath;
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty)
            {
                if (!String.IsNullOrEmpty(openFileDialog.FileName))
                {
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        LoadLocalDml(openFileDialog.FileName);
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }

                Dml.IsEdited = true;
            }
        }

        internal static void LoadLocalDml(string dmlFile, ZLibraryCategory loadAs = ZLibraryCategory.Local)
        {
            if (File.Exists(dmlFile))
            {
                ZMaterialLibrary dml_local = DataProvider.Instance.OpenZMaterialLibrary(dmlFile, dml);
                Dml.AddLibrary(dml_local, loadAs);
            }

            string folder = Path.GetDirectoryName(dmlFile);
            string name = Path.GetFileNameWithoutExtension(dmlFile);
            string delta_file = Path.Combine(folder, name + ".delta");

            if (File.Exists(delta_file))
            {
                LoadZoDelta(delta_file);
                if (loadAs == ZLibraryCategory.Corporate) IsCorporateDeltaLoaded = true;
            }
        }

        internal static bool ProposeToSaveDML()
        {
            if (dml != null && dml.Materials != null) ZPlannerManager.DMLPanel.SaveDMLSettings();

            if (dml == null || dml.Materials == null || !Dml.IsEdited) return true;

            MessageBoxDialog dialog = new MessageBoxDialog("Save Dielectric Material Library", "Would you like to save Library changes?", ((System.Drawing.Icon)(global::ZZero.ZPlanner.Properties.Resources.SaveLibrary)));
            DialogResult dialogResult = dialog.ShowDialog();
            switch (dialogResult)
            {
                case DialogResult.Yes:
                    if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Local)) SaveDml(ZLibraryCategory.Local);
                    if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Corporate)) SaveDml(ZLibraryCategory.Corporate);
                    if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.ZZero)) SaveDml(ZLibraryCategory.ZZero);
                    return true;
                case DialogResult.No:
                    return true;
                case DialogResult.Cancel:
                    break;
            }

            return false;
        }

        private static string GetSelectLists()
        {
            ZMaterialLibrary zlib = Dml;
            ZList<SelectList> lists = zlib.Lists;
            string s = "<SelectLists>" + Environment.NewLine;
            foreach (SelectList sl in lists)
            {
                s += "<SelectList id=" + "\"" + sl.ID + "\">" + Environment.NewLine;
                foreach (string val in sl.GetValues())
                {
                    s += "<SelectItem>" + val + "</SelectItem>" + Environment.NewLine;
                }
                s += "</SelectList>" + Environment.NewLine; 
            }
            s += "</SelectLists>" + Environment.NewLine; 
            return s;
        }

        private static string GetProperties()
        {
            ZMaterialLibrary zlib = Dml;
            ZList<ZParameter> pars = zlib.Parameters;
            string s = "<Parameters>" + Environment.NewLine;
            foreach (ZParameter p in pars)
            {
                s += "<Parameter id=" + "\"" + p.ID + "\" ";
                s += "title=" + "\"" + p.Title + "\" ";
                s += "type=" + "\"" + p.ValueType + "\" ";
                if (p.ValueType == ZValueType.Select && p.List != null)
                {
                    s += "list=" + "\"" + p.List.ID + "\" ";
                }
                s += "measure=" + "\"" + p.Measure + "\" ";
                s += "/>" + Environment.NewLine;
            }
            s += "</Parameters>" + Environment.NewLine;

            return s;
        }

        private static string GetMaterials(ZLibraryCategory category, out int count)
        {
            string sCategory = "";
            switch(category){
                case ZLibraryCategory.Local: sCategory = "Local"; break;
                case ZLibraryCategory.Corporate: sCategory = "Corporate"; break;
                case ZLibraryCategory.ZZero: sCategory = "Z-zero"; break;
            }
            
            string s = "<Library>" + Environment.NewLine;
            ZMaterialLibrary zlib = Dml;

            //set of materials to save
            List<ZMaterial> materials = null;
            switch (category)
            {
                case ZLibraryCategory.Local:
                    materials = (from material in zlib.Materials
                                 where (!material.IsFiltered && material.MaterialParameters.GetByID("Category").Value == "Local")
                                 select material).ToList();
                    break;
                case ZLibraryCategory.Corporate: 
                    materials = (from material in zlib.Materials
                                 where ((!material.IsFiltered && (material.MaterialParameters.GetByID("Category").Value == "Local") ||
                                           (material.MaterialParameters.GetByID("Category").Value == "Corporate"))
                                    )
                                    select material).ToList(); 
                    break;
                case ZLibraryCategory.ZZero: materials = (from material in zlib.Materials
                                              where (!material.IsFiltered)
                                              select material).ToList();
                                            //materials = zlib.Materials; 
                    break;
            }

            count = materials.Count;
            foreach (ZMaterial m in materials)
            {
                s += "<Material id=" + "\"" + m.ID + "\">" + Environment.NewLine;
                foreach (ZMaterialParameter p in m.MaterialParameters)
                {
                    string parVal = p.Value;
                    if (p.ID == ZStringConstants.DMLParameterIDCategory)
                    {
                        parVal = sCategory;
                    }
                    s += "<MaterialParameter value=" + "\"" + parVal + "\"" + " parameterId=" + "\"" + p.ID + "\"/>" + Environment.NewLine;
                }
                s += "</Material>" + Environment.NewLine;
            }
            s += "</Library>" + Environment.NewLine;
            return s;
        }

        private static bool GetDmlFile(ZLibraryCategory category, bool overwrite, out string fileName)
        {
            //original location
            string initialPath = "";
            string filter = "";
            string title = "";
            fileName = string.Empty;

            switch (category)
            {
                case ZLibraryCategory.Local:
                    fileName = Options.TheOptions.DML_LocalPath;
                    title = "Save Local DML";
                    filter = "Z-planner DML files (*.xml)|*.xml";
                    break;
                case ZLibraryCategory.Corporate:
                    fileName = Options.TheOptions.DML_NetworkPath;
                    title = "Save Corporate DML";
                    filter = "Z-planner DML files (*.xml)|*.xml";
                    break;
                case ZLibraryCategory.ZZero:
                    fileName = ZSettings.DMLFile; 
                    title = "Save Zo DML";
                    filter = "Z-planner DML files (*.z0lib)|*.z0lib";
                    break;
            }

            initialPath = Path.GetDirectoryName(fileName);
            if (string.IsNullOrEmpty(fileName) || !overwrite)
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Filter = filter;
                saveFileDialog.Title = title;
                saveFileDialog.RestoreDirectory = true;
                if (!string.IsNullOrEmpty(initialPath))
                {
                    saveFileDialog.InitialDirectory = initialPath;
                }

                // If the file name is not an empty string open it for saving.
                if (saveFileDialog.ShowDialog() == DialogResult.OK)
                {
                    fileName = saveFileDialog.FileName;
                }
                else
                {
                    return false;
                }
            }

            return !String.IsNullOrEmpty(fileName);               
        }

        private static void SaveZoDelta(string dmlFile)
        {
            string folder = Path.GetDirectoryName(dmlFile);
            string name = Path.GetFileNameWithoutExtension(dmlFile);
            string fileName = Path.Combine(folder, name + ".delta");

            int count;
            string sLib = "<DMLDocument>" + Environment.NewLine;
            sLib += "<DMLData>" + Environment.NewLine;
            sLib += GetDelta(out count);
            sLib += "</DMLData>" + Environment.NewLine;
            sLib += "</DMLDocument>";

            if (count > 0) { 
                File.WriteAllText(fileName, sLib);
            }
        }

        private static string GetDelta(out int count)
        {
            string s = "<Library>" + Environment.NewLine;
            ZMaterialLibrary zlib = Dml;
            ZList<ZMaterial> materials = zlib.Materials;
            count = materials.Count;
            foreach (ZMaterial m in materials)
            {
//                if (m.IsEdited)
//                {
                    bool b = false;
                    string spar = "";
                    foreach (ZMaterialParameter p in m.MaterialParameters)
                    {
                        switch (p.ID) {
                            case ZStringConstants.DMLParameterIDCategory: b = p.Value == "Z-zero" || p.Value == "Corporate"; 
                                break;
                            case ZStringConstants.DMLParameterIDQualified:
                                if (!IsCorporateDeltaLoaded && p.Value.ToLower() == "true") spar += "<MaterialParameter value=" + "\"" + p.Value + "\"" + " parameterId=" + "\"" + p.ID + "\"/>" + Environment.NewLine;
                                break;
                            case ZStringConstants.DMLParameterIDHide:
                                if (p.Value.ToLower() == "true") spar += "<MaterialParameter value=" + "\"" + p.Value + "\"" + " parameterId=" + "\"" + p.ID + "\"/>" + Environment.NewLine;
                                break;
                        }
                    }
                    if (b && !string.IsNullOrWhiteSpace(spar))
                    {
                        s += "<Material id=" + "\"" + m.ID + "\">";
                        s += spar;
                        s += "</Material>" + Environment.NewLine;
                    }
 //               }
            }
            s += "</Library>" + Environment.NewLine;
            return s;
        }

        internal static void LoadZoDelta(string path)
        {
            ZMaterialLibrary zlib = Dml;
            ZList<ZMaterial> materials = zlib.Materials;

            XElement root = XElement.Load(path);
            var query = from item in root.Descendants("MaterialParameter")
            group item by item.Parent into itemsGroup
            select new { Key = itemsGroup.Key.Attribute("id").Value, Material = itemsGroup.FirstOrDefault().Parent, MaterialParameters = itemsGroup };

            foreach (var materialGroup in query)
            {
                ZMaterial materialInLibrary = materials.Find(s => s.ID == materialGroup.Key);
                if (materialInLibrary != null)
                {
                    foreach (XElement materialParameterElement in materialGroup.MaterialParameters)
                    {
                        string parId = materialParameterElement.Attribute("parameterId").Value;
                        if (IsCorporateDeltaLoaded && parId == ZStringConstants.DMLParameterIDQualified) continue;

                        string parVal = materialParameterElement.Attribute("value").Value;
                        ZMaterialParameter p = materialInLibrary.MaterialParameters.Find(x => x.ID == parId);
                        p.Value = parVal;
                    }
                }
            }
        
        }
//---------------------------------------------------------------------------------------

        internal static CommandManager Commands 
        {
            get 
            {
                if (commandManager == null) InitializeUndoRedo();
                return commandManager;
            }
        }

        internal static ZPlannerContainerPanel ContainerPanel
        {
            get
            {
                if (containerPanel == null) containerPanel = new ZPlannerContainerPanel();
                return containerPanel;
            }
        }

        internal static ZPlannerPropertiesPanel PropertiesPanel
        {
            get
            {
                if (propertiesPanel == null || propertiesPanel.IsDisposed) propertiesPanel = new ZPlannerPropertiesPanel();
                return propertiesPanel;
            }
        }

        internal static FXSandboxPanel FXSandboxPanel
        {
           get
            {
                if (fxSandboxPanel == null || fxSandboxPanel.IsDisposed) fxSandboxPanel = new FXSandboxPanel();
                return fxSandboxPanel;
            }
        }

        internal static DRCPanel DRCPanel
        {
            get
            {
                if (drcPanel == null || drcPanel.IsDisposed) drcPanel = new DRCPanel();
                return drcPanel;
            }
        }

        internal static DiffToolDialog DiffPanel
        {
            get
            {
                if (diffPanel == null || diffPanel.IsDisposed)
                {
                    diffPanel = new DiffToolDialog();
                    diffPanel.WindowState = FormWindowState.Maximized;
                }
                return diffPanel;
            }
        }    

        internal static ZPlannerProjectPanel ProjectPanel
        {
            get
            {
                if (projectPanel == null || projectPanel.IsDisposed) projectPanel = new ZPlannerProjectPanel();
                return projectPanel;
            }
        }

        internal static ZPlannerDMLPanel DMLPanel
        {
            get
            {
                if (dmlPanel == null || dmlPanel.IsDisposed)
                {
                    dmlPanel = new ZPlannerDMLPanel();
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        InitFilters();
                        dmlPanel.LoadData(ZPlannerManager.Dml);
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
                return dmlPanel;
            }
        }

        internal static ZPlannerMessagePanel MessagePanel
        {
            get
            {
                if (messagePanel == null || messagePanel.IsDisposed) messagePanel = new ZPlannerMessagePanel();
                return messagePanel;
            }
        }

        internal static ZPlannerStartPanel StartPanel
        {
            get
            {
                if (startPanel == null || startPanel.IsDisposed) startPanel = new ZPlannerStartPanel();
                return startPanel;
            }
        }

        internal static ZPlannerStackupPanel StackupPanel
        {
            get
            {
                return stackupPanel;
            }

            set 
            {
                if (stackupPanel != null)
                {
                    stackupPanel.Clear();
                    stackupPanel.Close();
                }

                DockContent contentToClose = null;
                if (ContainerPanel.MainDockPanel.DocumentStyle == DocumentStyle.SystemMdi)
                {
                    foreach (Form form in ContainerPanel.MdiChildren)
                        if (form.GetType().ToString().Contains("ZPlannerStackupPanel")) form.Close();
                }
                else
                {
                    foreach (DockContent document in ContainerPanel.MainDockPanel.DocumentsToArray())
                    {
                        if (document.GetType().ToString().Contains("ZPlannerStackupPanel")) document.DockHandler.DockPanel = null;
                    }
                }

                
                foreach (DockContent content in ContainerPanel.MainDockPanel.Contents)
                {
                    if (content.GetType().ToString().Contains("ZPlannerStackupPanel"))
                    {
                        contentToClose = content;
                        content.DockHandler.Close();
                        break;
                    }
                }

                if (contentToClose != null && contentToClose.DockHandler != null) contentToClose.DockHandler.DockPanel = null;

                stackupPanel = value;
                if (stackupPanel != null)
                {
                    ZPlannerManager.IsAutoMirror = false; // stackupPanel.Stackup.IsMirrored();
                    stackupPanel.LoadData(ZPlannerManager.Project.Stackup, null, null);
                    InitSwitches();
                    ShowHideComments(ZPlannerManager.Project.Stackup);

                    //stackupPanel.SetCurrentRow(0);
                    stackupPanel.ClearRowSelection();

                    //ShowProjectView();
                    //StartPanel.Hide();
                    //stackupPanel.Show(ContainerPanel.MainDockPanel);

                    ZPlannerManager.GoToStackupMenuTab();
                    ZPlannerManager.ProjectPanel.SelectStackup();
                }

                MainMenu.UpdateMenu();
            }
        }

        internal static DmlSearchDialog DMLSearchDialog { get { if (dmlSearchDialog == null) dmlSearchDialog = new DmlSearchDialog(); return dmlSearchDialog; } }

        internal static ProgramMenu MainMenu
        {
            get { return mainRibbonMenu; }
            set { if (mainRibbonMenu == null) mainRibbonMenu = value; }
        }

        internal static StatusMenu StatusMenu { get { return ContainerPanel.StatusMenu; } }

        internal static Dictionary<string, List<ZEntity>> Clipboard;

        internal static Rights rights { get; set; }

        /// <summary>
        /// Initializes command manager for Undo/Redo functionality.
        /// </summary>
        internal static void InitializeUndoRedo()
        {
            commandManager = new CommandManager(ZSettings.CommandsLimit);
        }

        public static event PropertyChangedEventHandler PropertyChanged;

        internal static void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(null, new PropertyChangedEventArgs(propertyName));
            }
        }

        public static event PropertyChangedEventHandler NeedToClear;

        internal static void NotifyNeedToClear(string name)
        {
            if (NeedToClear != null)
            {
                NeedToClear(null, new PropertyChangedEventArgs(name));
            }
        }

        internal static void CreateNewProject()
        {
            if (!ZPlannerManager.CloseProject()) return;

            ZPlannerManager.MessagePanel.ClearMessages();
            ZPlannerManager.StatusMenu.SetStatus("Creating new Stackup ...");

            ZPlannerManager.Commands.IsIgnoreCommands = true; 

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
            ZPlannerProject project = new ZPlannerProject();
            CreateNewStackup(project);
            Project = project;

            ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);
            ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();
            ZPlannerManager.Commands.Clear();
            ZPlannerManager.StatusMenu.SetStatusReady();
            ZPlannerManager.MessagePanel.AddMessage("New Stackup was created.");
        }

        internal static void CreateNewStackup(ZPlannerProject project)
        {
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
            project.Stackup = new ZStackup(project);
            project.Stackup.AddLayer(0);
        }

        internal static void OpenProject()
        {
            if (!ZPlannerManager.CloseProject()) return;

            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "Z-planner stackup files (*.Z0p)|*.z0p";
            openFileDialog.FilterIndex = 2;
            if (!Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ProjectPath;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty)
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.SetStatus("Opening a Stackup from file ...");

                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                try
                {
                    InternalOpenProject(openFileDialog.FileName);
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                ZPlannerManager.StatusMenu.SetStatusReady();
                ZPlannerManager.MessagePanel.AddMessage(string.Format("\"{0}\" Stackup was opened.", Project.Stackup.Title));

                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();
            }
        }

        internal static void OpenProject(string fileName)
        {
            if (string.IsNullOrWhiteSpace(fileName) || !File.Exists(fileName)) return;

            if (!ZPlannerManager.CloseProject()) return;

            ZPlannerManager.MessagePanel.ClearMessages();
            ZPlannerManager.StatusMenu.SetStatus("Opening a Stackup from file ...");

            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                InternalOpenProject(fileName);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
            }

            ZPlannerManager.StatusMenu.SetStatusReady();
            ZPlannerManager.MessagePanel.AddMessage(string.Format("\"{0}\" Stackup was opened.", Project.Stackup.Title));

            UpdateActiveStackup();
            ZPlannerManager.Commands.Clear();
        }

        private static void InternalOpenProject(string fileName)
        {
            ZPlannerManager.Commands.IsIgnoreCommands = true;
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;
            ProjectFile = fileName;
            Project = DataProvider.Instance.OpenZPlannerProject(fileName);
            ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);

            string dir = Path.GetDirectoryName(fileName);
            if (dir != Path.Combine(Options.TheOptions.ProjectPath, "AutoSave") && dir != Path.Combine(Options.TheOptions.ProjectPath, "Backup")) RecentFilesManager.AddToRecentFiles(fileName);
        }

        internal static void OpenPredefinedProjectFile()
        {
            if (!string.IsNullOrEmpty(ProjectFileToBeOpened))
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.SetStatus("Opening a Stackup from file ...");

                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                try
                {
                    InternalOpenProject(ProjectFileToBeOpened);
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                ZPlannerManager.StatusMenu.SetStatusReady();
                ZPlannerManager.MessagePanel.AddMessage(string.Format("\"{0}\" Stackup was opened.", Project.Stackup.Title));

                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();
            }
        }

        internal static void OpenRecentFile(string file)
        {
            if (!ZPlannerManager.CloseProject()) return;

            ZPlannerManager.MessagePanel.ClearMessages();
            ZPlannerManager.StatusMenu.SetStatus("Opening a Stackup from recent file ...");

            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                InternalOpenProject(file);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
            }
            
            ZPlannerManager.StatusMenu.SetStatusReady();
            ZPlannerManager.MessagePanel.AddMessage(string.Format("\"{0}\" Stackup was opened.", Project.Stackup.Title));

            ZPlannerManager.UpdateActiveStackup();
            ZPlannerManager.Commands.Clear();
        }

        internal static bool SaveProject()
        {
            // If the file name is not an empty string open it for saving.
            if (string.IsNullOrEmpty(ZPlannerManager.ProjectFile) || 
                Path.GetDirectoryName(ZPlannerManager.ProjectFile) == Path.Combine(Options.TheOptions.ProjectPath, "AutoSave") || 
                Path.GetDirectoryName(ZPlannerManager.ProjectFile) == Path.Combine(Options.TheOptions.ProjectPath, "Backup"))
            {
                return SaveProjectAs();
            }
            else
            {
                BackupProject(ZPlannerManager.ProjectFile);
                DataProvider.Instance.SaveZPlannerProject(ZPlannerManager.ProjectFile, ZPlannerManager.Project);
                ZPlannerManager.Commands.Clear();
                RecentFilesManager.AddToRecentFiles(ZPlannerManager.ProjectFile);
                return true;
            }
        }

        internal static bool SaveProjectAs()
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Z-planner stackup files (*.Z0p)|*.z0p";
            saveFileDialog.Title = "Save a Stackup";
            if (!Options.TheOptions.UseLast) saveFileDialog.InitialDirectory = Options.TheOptions.ProjectPath;
            string owner = Options.TheOptions.Company;
            if (string.IsNullOrWhiteSpace(owner)) owner = Options.TheOptions.UserName;
            if (string.IsNullOrWhiteSpace(owner)) owner = "Z-planner";

            string oldFileName = (!ProjectIsEmpty && !Project.StackupIsEmpty) ?  Path.GetFileNameWithoutExtension(ProjectFile) : null;
            DateTime dtValue;
            if (oldFileName != null && oldFileName.Length > 8 && DateTime.TryParseExact(oldFileName.Substring(0, 8), "yy-MM-dd", CultureInfo.InvariantCulture, DateTimeStyles.None, out dtValue)) oldFileName = oldFileName.Substring(8);
            if (oldFileName != null && oldFileName.StartsWith("-")) oldFileName = oldFileName.Substring(1);
            if (string.IsNullOrWhiteSpace(saveFileDialog.FileName))
            {
                if (!string.IsNullOrWhiteSpace(oldFileName)) saveFileDialog.FileName = string.Format("{0}-{1}.z0p", DateTime.Now.ToString("yy-MM-dd"), oldFileName);
                else if (!ProjectIsEmpty && !Project.StackupIsEmpty) saveFileDialog.FileName = string.Format("{0}-{1}-{2}L {3}.z0p", DateTime.Now.ToString("yy-MM-dd"), owner, Project.Stackup.GetMetallLayerCount().ToString(), Project.Stackup.Title);
            }


            // If the file name is not an empty string open it for saving.
            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
            {
                ZPlannerManager.ProjectFile = saveFileDialog.FileName;
                BackupProject(ZPlannerManager.ProjectFile);
                DataProvider.Instance.SaveZPlannerProject(saveFileDialog.FileName, ZPlannerManager.Project);
                ZPlannerManager.Commands.Clear();
                RecentFilesManager.AddToRecentFiles(saveFileDialog.FileName);
                return true;
            }

            return false;
        }

        private static long previewTicks = DateTime.Now.Ticks;

        internal static void AutoSave()
        {
            int interval = Options.TheOptions.autosaveInterval;
            if (interval <= 0) return;

            long currentTicks = DateTime.Now.Ticks;
            if (currentTicks - previewTicks < interval * 600000000L) return;
            previewTicks = currentTicks;

            if (StackupPanel != null && !ProjectIsEmpty && !Project.StackupIsEmpty)
            {
                string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "AutoSave");
                string fileName = Path.Combine(dirPath, (!string.IsNullOrWhiteSpace(ProjectFile)) ? Path.GetFileNameWithoutExtension(ProjectFile) : "Unnamed");
                if (fileName.EndsWith("_AutoSave")) fileName = fileName.Substring(0, fileName.LastIndexOf("_AutoSave"));

                if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

                if (File.Exists(fileName + "_AutoSave.z0p")) File.Delete(fileName + "_AutoSave.z0p");
                DataProvider.Instance.SaveZPlannerProject(fileName + "_AutoSave.z0p", ZPlannerManager.Project);
            }
        }

        internal static void BackupProject(string fileName)
        {
            if (!string.IsNullOrWhiteSpace(fileName) && File.Exists(fileName) && 
                 Path.GetDirectoryName(fileName) != Path.Combine(Options.TheOptions.ProjectPath, "AutoSave") &&
                 Path.GetDirectoryName(fileName) != Path.Combine(Options.TheOptions.ProjectPath, "Backup"))
            {
                string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "Backup");
                string bacupName = Path.Combine(dirPath, Path.GetFileNameWithoutExtension(fileName));

                if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

                if (File.Exists(bacupName + "_Backup_3.z0p")) File.Delete(bacupName + "_Backup_3.z0p");
                if (File.Exists(bacupName + "_Backup_2.z0p")) File.Move(bacupName + "_Backup_2.z0p", bacupName + "_Backup_3.z0p");
                if (File.Exists(bacupName + "_Backup_1.z0p")) File.Move(bacupName + "_Backup_1.z0p", bacupName + "_Backup_2.z0p");
                File.Copy(fileName, bacupName + "_Backup_1.z0p");
            }
        }

        internal static bool ProposeToSaveProject()
        {
            if (project != null && !project.StackupIsEmpty && ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.StackupPanel.SaveStackupSettings();
            }

            if (project == null || project.StackupIsEmpty || Commands.UndoCount <= 0) return true;

            MessageBoxDialog dialog = new MessageBoxDialog("Save Stackup Changes", "Would you like to save Stackup changes?", ((System.Drawing.Icon)(global::ZZero.ZPlanner.Properties.Resources.SaveStackup)));
            DialogResult dialogResult = dialog.ShowDialog();
            switch(dialogResult)
            {
                case DialogResult.Yes:
                    SaveProject();
                    return true;
                case DialogResult.No:
                    return true;
                case DialogResult.Cancel:
                    break;
            }

            return false;
        }

        internal static void SaveStackupAndEmailIt()
        {
            if (SaveProject())
            {
                try
                {
                    MAPIW mapi = new MAPIW();
                    mapi.AddAttachment(ZPlannerManager.ProjectFile);
                    mapi.SendMailPopup(Path.GetFileName(ZPlannerManager.ProjectFile), "Stackup attached.");
                }
                catch
                {
                    try
                    {
                        MAPI mapi = new MAPI();
                        mapi.AddAttachment(ZPlannerManager.ProjectFile);
                        mapi.SendMailPopup(Path.GetFileName(ZPlannerManager.ProjectFile), "Stackup attached.");
                    }
                    catch
                    {
                        try
                        {
                            var mailMessage = new MailMessage();
                            mailMessage.From = new MailAddress(!string.IsNullOrWhiteSpace(Options.TheOptions.UserEMail) ? Options.TheOptions.UserEMail : "yourname@yourcompany.com");
                            mailMessage.Subject = Path.GetFileName(ZPlannerManager.ProjectFile);
                            mailMessage.IsBodyHtml = false;
                            mailMessage.Body = "Stackup attached.";

                            mailMessage.Attachments.Add(new Attachment(ZPlannerManager.ProjectFile));

                            var filename = Path.Combine(ZSettings.LayoutFolder, "message.eml");

                            //save the MailMessage to the filesystem
                            mailMessage.Save(filename);

                            //Open the file with the default associated application registered on the local machine
                            Process.Start(filename);
                        }
                        catch
                        {
                            EMailForm form = new EMailForm();
                            form.Attachment = ZPlannerManager.ProjectFile;
                            form.From = Options.TheOptions.UserEMail;
                            form.Message = "Stackup attached.";
                            form.Subject = Path.GetFileName(ZPlannerManager.ProjectFile);
                            form.ShowDialog();
                        }
                    }
                }
            }
        }

        internal static void OpenLocalMenu()
        {
            ZPlannerManager.OpenLocalDml();
        }

        internal static void SaveLocalMenu()
        {
            SaveDml(ZLibraryCategory.Local, false);
        }

        internal static void SaveAsLocalMenu()
        {
            SaveDml(ZLibraryCategory.Local, true);
        }

        internal static bool CloseProject(Action beforeCloseAction = null)
        {
            if (!ProposeToSaveProject()) return false;

            if (project != null) ZPlannerManager.WriteSettings();
            if (beforeCloseAction != null) beforeCloseAction();
            ZPlannerManager.MessagePanel.ClearMessages();
            ZPlannerManager.StatusMenu.SetStatus("Closing stackup ...");

            if (project != null) Project = null;
            if (projectFile != null) ProjectFile = null;
            if (ZPlannerManager.Commands.UndoCount > 0 || ZPlannerManager.Commands.RedoCount > 0) ZPlannerManager.Commands.Clear();
            ZEntity.IdList.Clear();

            ZPlannerManager.StatusMenu.SetStatusReady();
            return true;
        }

        internal static void OpenSetupDialog()
        {
            SettingsDialog options = new SettingsDialog();
            options.ShowDialog();
        }

        internal static void ShowFXSandbox()
        {
            ZPlannerManager.FXSandboxPanel.ShowDialog();
        }

        internal static void ShowDRCPanel()
        {
            //ZPlannerManager.DRCPanel.Show();
            //ZPlannerManager.DRCPanel.Show(ZPlannerManager.StartPanel.Pane, DockAlignment.Right, 0.2);
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.DRCPanel.Show(ZPlannerManager.StackupPanel.Pane, DockAlignment.Bottom, 0.3);
        }

        internal static void ShowViaSpanPanel()
        {
            ZPlannerViaSpanPanel panel = new ZPlannerViaSpanPanel();
            if (StackupPanel != null)
            {
                panel.StartPosition = FormStartPosition.Manual;
                panel.Location = StackupPanel.GetViaSpanDialogPoint();
            }
            panel.ShowDialog();
            //panel.Show();
        }

        internal static void ShowDiffPanel()
        {
            ZPlannerManager.DiffPanel.Show();
            //ZPlannerManager.DiffPanel.ShowDialog();
        }

        internal static void ShowProjectPanel()
        {
            ZPlannerManager.ProjectPanel.Show(ZPlannerManager.PropertiesPanel.Pane, DockAlignment.Top, 0.4);
        }

        internal static void ShowPropertiesPanel()
        {
            if (ZPlannerManager.StackupPanel != null) 
                ZPlannerManager.PropertiesPanel.Show(ZPlannerManager.StackupPanel.Pane, DockAlignment.Left, 0.15);
            else 
                ZPlannerManager.PropertiesPanel.Show(ZPlannerManager.StartPanel.Pane, DockAlignment.Left, 0.15);
        }

        internal static void ShowMessagePanel()
        {
            ZPlannerManager.MessagePanel.Show(ZPlannerManager.PropertiesPanel.Pane, DockAlignment.Bottom, 0.2);
        }

        internal static void ShowStackupPanel()
        {
            StartPanel.Hide();
            StackupPanel.Show(ContainerPanel.MainDockPanel, DockState.Document);
        }

        internal static void ShowStartPagePanel()
        {
            //ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel, DockState.Document);
            ZPlannerManager.ShowStartPageView();
        }

        internal static void ShowDMLPanel()
        {
            if (ZPlannerManager.StackupPanel != null) 
                ZPlannerManager.DMLPanel.Show(ZPlannerManager.StackupPanel.Pane, DockAlignment.Bottom, 0.3);
            else 
                ZPlannerManager.DMLPanel.Show(ZPlannerManager.StartPanel.Pane, DockAlignment.Bottom, 0.3);
        }

        internal static void ShowLossPanel(ZLayer layer, ZTableType tableType)
        {
            switch(tableType)
            { 
                case ZTableType.Single:
                // Add code to Open LossViewer here.
                //for single-ended nets
                FX.LossViewer(layer, layer.Stackup.ActiveSingle);
                break;
            
                case ZTableType.Pair:
                //for diff pairs
                FX.LossViewer(layer, layer.Stackup.ActivePair);
                break;
            }
        }

        internal static void ShowProjectView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.ShowStackupPanel();
            else ZPlannerManager.ShowStartPagePanel();
            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.ShowDMLPanel(); 

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDefaultProjectView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            if (ZPlannerManager.StackupPanel != null) ShowStackupPanel();

            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.ShowDMLPanel();

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();
            
            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowStackupPropertiesView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.ShowStackupPanel();
            else ZPlannerManager.ShowStartPagePanel();
            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.DMLPanel.Hide(); 
            
            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDefaultStackupPropertiesView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            if (ZPlannerManager.StackupPanel != null) ShowStackupPanel();

            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.ShowDMLPanel();
            ZPlannerManager.DMLPanel.Hide(); 

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowStartPageView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.StackupPanel.Hide();
            }

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel, DockState.Document);
            ZPlannerManager.PropertiesPanel.Hide();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.DMLPanel.Hide();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDefaultStartPageView()
        {
            IsIgnorePanelVisibleChanged = true;

            /*if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else*/ 
            ZPlannerManager.GoToHomeMenuTab();

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            if (ZPlannerManager.StackupPanel != null) { StackupPanel.Show(ContainerPanel.MainDockPanel, DockState.Document); ZPlannerManager.StackupPanel.Hide(); }

            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.PropertiesPanel.Hide();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.ShowDMLPanel();
            ZPlannerManager.DMLPanel.Hide();

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowStackupDmlView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.ShowStackupPanel();
            else ZPlannerManager.ShowStartPagePanel();
            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.ShowDMLPanel();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDefaultStackupDmlView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.GoToStackupMenuTab();
            else ZPlannerManager.GoToHomeMenuTab();

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            if (ZPlannerManager.StackupPanel != null) ShowStackupPanel();

            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.ShowDMLPanel();

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDmlView()
        {
            IsIgnorePanelVisibleChanged = true;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Hide();
            ZPlannerManager.StartPanel.Hide();
            ZPlannerManager.PropertiesPanel.Hide();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.DRCPanel.Hide();
            ZPlannerManager.DMLPanel.SelectFirstMaterial();
            ZPlannerManager.DMLPanel.Show(ContainerPanel.MainDockPanel, DockState.Document);

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        internal static void ShowDefaultDmlView()
        {
            IsIgnorePanelVisibleChanged = true;

            ZPlannerManager.GoToLibraryMenuTab();

            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            if (ZPlannerManager.StackupPanel != null) { ShowStackupPanel(); ZPlannerManager.StackupPanel.Hide(); }
            else ZPlannerManager.StartPanel.Hide();

            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.PropertiesPanel.Hide();
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.DMLPanel.Show(ContainerPanel.MainDockPanel, DockState.Document);

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();

            IsIgnorePanelVisibleChanged = false;
            MainMenu.SelectView();
        }

        private static bool isViasVisible;
        internal static bool IsViasVisible
        {
            get
            {
                return isViasVisible;
            }
            set
            {
                if (isViasVisible != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsViasVisible", isViasVisible, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isViasVisible = value;
                    NotifyPropertyChanged("IsViasVisible");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isSinglesVisible;
        internal static bool IsSinglesVisible
        {
            get
            {
                return isSinglesVisible;
            }
            set
            {
                if (isSinglesVisible != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsSinglesVisible", isSinglesVisible, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isSinglesVisible = value;
                    NotifyPropertyChanged("IsSinglesVisible");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isPairsVisible;
        internal static bool IsPairsVisible
        {
            get
            {
                return isPairsVisible;
            }
            set
            {
                if (isPairsVisible != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsPairsVisible", isPairsVisible, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isPairsVisible = value;
                    NotifyPropertyChanged("IsPairsVisible");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isHeadersVisible;
        internal static bool IsHeadersVisible
        {
            get
            {
                return isHeadersVisible;
            }
            set
            {
                if (isHeadersVisible != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsHeadersVisible", isHeadersVisible, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isHeadersVisible = value;
                    NotifyPropertyChanged("IsHeadersVisible");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isCenterLineVisible;
        internal static bool IsCenterLineVisible
        {
            get
            {
                return isCenterLineVisible;
            }
            set
            {
                if (isCenterLineVisible != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsCenterLineVisible", isCenterLineVisible, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isCenterLineVisible = value;
                    NotifyPropertyChanged("IsCenterLineVisible");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isColorDisabledCells;
        internal static bool IsColorDisabledCells
        {
            get
            {
                return isColorDisabledCells;
            }
            set
            {
                if (isColorDisabledCells != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsColorDisabledCells", isColorDisabledCells, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isColorDisabledCells = value;
                    NotifyPropertyChanged("IsColorDisabledCells");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        private static bool isCoreLocked;
        internal static bool IsCoreLocked
        {
            get
            {
                return isCoreLocked;
            }
            set
            {
                if (isCoreLocked != value)
                {
                    //if (!ZPlannerManager.Commands.IsIgnoreCommands) new ChangeZPlannerPropertyCommand("IsCoreLocked", isCoreLocked, value);
                    //bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    isCoreLocked = value;
                    NotifyPropertyChanged("IsCoreLocked");
                    //ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                }
            }
        }

        internal static void ShowVias(bool isVisible)
        {
            IsViasVisible = isVisible;

            ZPlannerManager.StackupPanel.ShowVias(isVisible);
        }

        internal static void ShowSingles(bool isVisible)
        {
            IsSinglesVisible = isVisible;

            ZPlannerManager.StackupPanel.ShowSingles(isVisible);
        }

        internal static void ShowPairs(bool isVisible)
        {
            IsPairsVisible = isVisible;

            ZPlannerManager.StackupPanel.ShowPairs(isVisible);
        }

        internal static void ShowHeaders(bool isVisible)
        {
            IsHeadersVisible = isVisible;

            if (StackupPanel != null) StackupPanel.ShowHeaders(isVisible);
            if (DMLPanel != null) DMLPanel.ShowHeaders(isVisible);
        }

        internal static void ShowDisabledCells(bool isVisible)
        {
            IsColorDisabledCells = isVisible;
            if (StackupPanel != null) StackupPanel.FormatGridView();
        }

        internal static void ShowCenterLine(bool isVisible)
        {
            IsCenterLineVisible = isVisible;
            if (StackupPanel != null) StackupPanel.ShowCenterLine(isVisible);
        }

        internal static bool TryAutoMirror(bool isMirror)
        {
            bool wasMirrored = isMirror;
            if (isMirror && !ZPlannerManager.IsAutoMirror && !ZPlannerManager.Project.Stackup.IsMirrored())
            {
                if (StackupPanel != null) ZPlannerManager.StackupPanel.EndGridEdit();
                wasMirrored = ZPlannerManager.Project.Stackup.MirrorStackup();
            }

            ZPlannerManager.IsAutoMirror = wasMirrored;

            return wasMirrored;
        }

        internal static void SetTrapezoidalTraces(bool isTrapezoidal)
        {
            ZPlannerManager.IsTrapezoidalTraces = isTrapezoidal;
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                ZPlannerManager.RecalculateThickness();
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static void SetPressedThickness(bool isPressed)
        {
            ZPlannerManager.IsPressedThickness = isPressed;

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                if (StackupPanel != null) StackupPanel.RecalculateViaSpanAspectRatio();
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static void SetSequentialLamination(bool isPressed)
        {
            ZPlannerManager.IsSequentialLamination = isPressed;

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                if (StackupPanel != null) StackupPanel.RecalculateViaSpanAspectRatio();
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                ZPlannerManager.UpdateActiveStackup();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static void SetCopperRoughness(bool isRoughness)
        {
            ZPlannerManager.IsRoughness = isRoughness;
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                ZPlannerManager.ShowSingles(IsSinglesShouldBeShown());
                ZPlannerManager.StackupPanel.ShowRoughnessColumns(ZPlannerManager.IsRoughness);
                ZPlannerManager.UpdateActiveStackup();
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static void SetLossPlanning(bool isLossPlanning)
        {
            IsLossPlanning = isLossPlanning;

            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.ShowSingles(ZPlannerManager.IsSinglesShouldBeShown());
                ZPlannerManager.StackupPanel.ShowLossPlanning(isLossPlanning);
            }
        }

        internal static void ShowGlassPitch(bool isGlassPitch, bool isGws)
        {
            ZPlannerManager.IsGlassPitch = isGlassPitch;
            ZPlannerManager.IsGws = isGws;
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                ZPlannerManager.ShowSingles(IsSinglesShouldBeShown());
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static bool IsSinglesShouldBeShown()
        {
            int pressedButtonsCount = 0;

            if (IsLossPlanning) ++pressedButtonsCount;
            if (IsGlassPitch) ++pressedButtonsCount;
            if (IsGws) ++pressedButtonsCount;
            if (IsRoughness) ++pressedButtonsCount;
            //if (gwsRibbonButton.Checked) ++pressedButtonsCount;

            return pressedButtonsCount < 2;
        }

        internal static void CalculatePressedThickness(bool isPressed)
        {
            foreach (ZLayer layer in Project.Stackup.Layers)
            {
                if (layer.GetLayerType() == ZLayerType.Prepreg)
                {
                    ZLayerParameter originThicknessParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDOriginThickness);
                    if (originThicknessParameter == null) continue;

                    if (string.IsNullOrWhiteSpace(originThicknessParameter.Value)) originThicknessParameter.Value = layer.GetLayerParameterValue(ZStringConstants.ParameterIDThickness);

                    if (!isPressed) layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, originThicknessParameter.Value);
                    /*else
                        originThicknessParameter.Value = thicknessParameter.Value;*/
                }
            }

            Project.Stackup.CalculatePrepregAdjustedThickness(isPressed);

            foreach (ZLayer layer in Project.Stackup.Layers)
            {
                if (layer.GetLayerType() == ZLayerType.Prepreg)
                {
                    if (isPressed) layer.SetLayerParameterValue(ZStringConstants.ParameterIDThickness, layer.GetLayerParameterValue(ZStringConstants.ParameterIDPrepregThickness));
                }
            }

            ZParameter parameter = Project.Parameters.GetByID(ZStringConstants.ParameterIDThickness) ?? Project.SubParameters.GetByID(ZStringConstants.ParameterIDThickness);
            int thicknessOrder = (parameter != null) ? parameter.Order : -1;

            //parameter = Project.Parameters.GetByID(ZStringConstants.ParameterIDCopperThickness) ?? Project.SubParameters.GetByID(ZStringConstants.ParameterIDCopperThickness);
            //int copperOrder = (parameter != null) ? parameter.Order : -1;

            parameter = Project.Parameters.GetByID(ZStringConstants.ParameterIDCopperPercent) ?? Project.SubParameters.GetByID(ZStringConstants.ParameterIDCopperPercent);
            if (parameter != null)
            {
                parameter.IsHidden = !isPressed;
                //if (copperOrder >= 0)
                //{
                //    if (parameter.Order < copperOrder) parameter.Order = copperOrder;
                //    else parameter.Order = copperOrder + 1;
                //}

                if (thicknessOrder >= 0)
                {
                    if (parameter.Order < thicknessOrder) parameter.Order = thicknessOrder - 1;
                    else parameter.Order = thicknessOrder;
                }
            }

            if (StackupPanel != null) StackupPanel.SetReadOnlyCells();
        }

        internal static void ImportODB()
        {
            if (!ZPlannerManager.CloseProject()) return;


            ODBppImportDialog odbppDialog = new ODBppImportDialog();
            DialogResult res = odbppDialog.ShowDialog();
            if (res != DialogResult.OK) return;

            bool bUseTgz = odbppDialog.OpenTgz();
            string tempDir = "";

            bool bOk = false;
            string selection = "";
            if (bUseTgz)
            {
                OpenFileDialog openFileDialog = new OpenFileDialog();

                openFileDialog.Filter = "ODB++ Files (*.TGZ)|*.TGZ";
                if (Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ExportPath;

                bOk = (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty);
                if (bOk)
                {
                    string tgz = openFileDialog.FileName;

                    string path = Path.GetRandomFileName();
                    DirectoryInfo dirInfo = Directory.CreateDirectory(Path.Combine(Path.GetTempPath(), path));
                    tempDir = dirInfo.FullName;

                    TarGz zip = new TarGz();
                    zip.Extract(tgz, tempDir);

                    string[] dirs = Directory.GetDirectories(tempDir);
                    selection = dirs[0];
                }
            }
            else
            {
                FolderBrowserDialog openFolderDialog = new FolderBrowserDialog();

                DialogResult result = openFolderDialog.ShowDialog();
                bOk = (result == DialogResult.OK && !string.IsNullOrWhiteSpace(openFolderDialog.SelectedPath));
                selection = openFolderDialog.SelectedPath;
            }

            if (bOk)
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.StartProgress("Importing a Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ZPlannerManager.Commands.IsIgnoreCommands = true;
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;

                try
                {
                    ODBPP_Parser parser = new ODBPP_Parser(selection);
                    parser.Import();
                    if (bUseTgz)
                    {
                        try
                        {
                            Directory.Delete(tempDir, true);
                        }
                        catch { }
                    }
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                if (!ProjectIsEmpty && !Project.StackupIsEmpty) Project.Stackup.Title = Path.GetFileNameWithoutExtension(selection);

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Importing a Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was imported from " + selection + ".");

                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();
            }

        }

        internal static void ExportODB()
        {
            ZStackup zs = ZPlannerManager.Project.Stackup;
            if (zs == null) return;

            FolderBrowserDialog openFolderDialog = new FolderBrowserDialog();
            if (Options.TheOptions.UseLast) openFolderDialog.SelectedPath = Options.TheOptions.ExportPath;
            
            DialogResult result = openFolderDialog.ShowDialog();
            if (result == DialogResult.OK && !string.IsNullOrWhiteSpace(openFolderDialog.SelectedPath))
            {
                ZPlannerManager.StatusMenu.StartProgress("Exporting a Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ODBPP_Exporter exp = new ODBPP_Exporter(openFolderDialog.SelectedPath);
                exp.Export();

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Exporting a Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was exported to " + openFolderDialog.SelectedPath + ".");
            }
        }


        internal static void ImportHL()
        {
            if (!ZPlannerManager.CloseProject()) return;

            OpenFileDialog openFileDialog = new OpenFileDialog();

            //--openFileDialog.Filter = "Stackup Files (*.STK)|*.STK|Schematic Files (*.FFS)|*.FFS";
            openFileDialog.Filter = "Schematic Files (*.FFS)|*.FFS|Stackup Files (*.STK)|*.STK";
            if (!Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ExportPath;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty)
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.StartProgress("Importing a Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ZPlannerManager.Commands.IsIgnoreCommands = true;
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;

                try
                {
                    HL_Parser parser = new HL_Parser(openFileDialog.FileName);
                    parser.Import();
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                if (!ProjectIsEmpty && !Project.StackupIsEmpty) Project.Stackup.Title = Path.GetFileNameWithoutExtension(openFileDialog.FileName);

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Importing a Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was imported from " + openFileDialog.FileName + ".");
                
                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();

            }
        }

        internal static void ExportHL()
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Stackup Files (Schematic Files (*.FFS)|*.FFS|*.STK)|*.STK";
            saveFileDialog.Title = "Export to HyperLynx File";
            if (!Options.TheOptions.UseLast) saveFileDialog.InitialDirectory = Options.TheOptions.ExportPath;
            
            ZStackup zs = ZPlannerManager.Project.Stackup;
            if (zs != null)
            {
                saveFileDialog.FileName = (!string.IsNullOrWhiteSpace(ZPlannerManager.ProjectFile)) ? Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile) : zs.Title;

                if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
                {
                    ZPlannerManager.StatusMenu.StartProgress("Exporting a Stackup ...");
                    Cursor currentCursor = Cursor.Current;
                    Cursor.Current = Cursors.WaitCursor;

                    HL_Exporter exp = new HL_Exporter(saveFileDialog.FileName);
                    exp.Export();

                    Cursor.Current = currentCursor;
                    ZPlannerManager.StatusMenu.StopProgress("Exporting a Stackup ...");
                    ZPlannerManager.MessagePanel.AddMessage("Stackup was exported to " + saveFileDialog.FileName + ".");
                }
            }
        }

        internal static void ExportExcel()
        {
            //options
            ExportOptions.TheOptions.ProjectName = (!string.IsNullOrWhiteSpace(ZPlannerManager.ProjectFile)) ? Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile) : string.Empty;
            if (string.IsNullOrWhiteSpace(ExportOptions.TheOptions.DesignRevision)) ExportOptions.TheOptions.DesignRevision = "1.0";

            ExcelOptionsDlg optDlg = new ExcelOptionsDlg();
            optDlg.StartPosition = FormStartPosition.CenterParent;
            if (optDlg.ShowDialog() == DialogResult.Cancel) return;

            //
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "Excel files (*.xlsm)|*.xlsm";
            saveFileDialog.Title = "Export to Excel File";
            if (!Options.TheOptions.UseLast) saveFileDialog.InitialDirectory = Options.TheOptions.ExportPath;

            ZStackup zs = ZPlannerManager.Project.Stackup;
            if (zs != null)
            {
                //default filename for export
                string sDate = DateTime.Now.ToString("yy-MM-dd");
                string fileForExport = sDate;
                if (ExportOptions.TheOptions.ProjectName.Length > 0)
                {
                    string pName = ExportOptions.TheOptions.ProjectName;
                    //check for standard SaveAs Name
                    bool bAddProject = true;
                    string aDate = pName.Substring(0, 8);
                    if (aDate.Length == 8)
                    {
                        string[] res = aDate.Split(new char [] {'-'});
                        if (res.Length == 3)
                        {
                            int temp;
                            if (Int32.TryParse(res[0], out temp) && Int32.TryParse(res[1], out temp) && Int32.TryParse(res[2], out temp)){
                                bAddProject = false;
                            }
                        }
                    }

                    if (bAddProject)
                    {
                        fileForExport += "-" + ExportOptions.TheOptions.ProjectName;
                    }
                    else
                    {
                        string aName = pName.Substring(8);
                        if (aName.Length > 0)
                        {
                            fileForExport += aName;
                        }
                    }
                }
                if (ExportOptions.TheOptions.DesignRevision.Length > 0)
                {
                    fileForExport += "-" + ExportOptions.TheOptions.DesignRevision;
                }
                if (Options.TheOptions.Title.Length > 0)
                {
                    fileForExport += "-" + Options.TheOptions.Title;
                }

                //--string fname = zs.Title;
                saveFileDialog.FileName = fileForExport;//fname;

                if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
                {
                    //copy template
                    string template = ZSettings.SettingsFolder + Path.DirectorySeparatorChar + "Excel" + Path.DirectorySeparatorChar + "ExcelExportTemplate.xlsm";
                    try
                    {
                        File.Copy(template, saveFileDialog.FileName, true);
                    }
                    catch (Exception err)
                    {
                        MessageBox.Show(err.Message);
                        return;
                    }
                    //copy logo
                    if (Options.TheOptions.UserLogo.Length > 0)
                    {
                        string logo = ZSettings.SettingsFolder + Path.DirectorySeparatorChar + Options.TheOptions.UserLogo;
                        string new_logo = Path.GetDirectoryName(saveFileDialog.FileName) + Path.DirectorySeparatorChar + "logo" + Path.GetExtension(logo);
                        try
                        {
                            File.Copy(logo, new_logo, true);
                        }
                        catch (Exception err)
                        {
                            MessageBox.Show(err.Message);
                            return;
                        }
                    }


                    ZPlannerManager.StatusMenu.StartProgress("Exporting a Stackup ...");
                    Cursor currentCursor = Cursor.Current;
                    Cursor.Current = Cursors.WaitCursor;

                    ExcelExporter exp = new ExcelExporter(saveFileDialog.FileName, ExportOptions.TheOptions);
                    exp.Export();

                    Cursor.Current = currentCursor;
                    ZPlannerManager.StatusMenu.StopProgress("Exporting a Stackup ...");
                    ZPlannerManager.MessagePanel.AddMessage("Stackup was exported to " + saveFileDialog.FileName + ".");
                    
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
        }

        internal static void ImportIPC2581()
        {
            if (!ZPlannerManager.CloseProject()) return;

            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "IPC-2581 Files (*.XML)|*.XML";
            if (!Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ExportPath;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty)
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.StartProgress("Importing a Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ZPlannerManager.Commands.IsIgnoreCommands = true;
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;

                try
                {
                    IPC_Parser parser = new IPC_Parser(openFileDialog.FileName);
                    parser.Import();
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                if (!ProjectIsEmpty && !Project.StackupIsEmpty) Project.Stackup.Title = Path.GetFileNameWithoutExtension(openFileDialog.FileName);

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Importing a Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was imported from " + openFileDialog.FileName + ".");

                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();
            }
        }

        internal static void ImportTapestry()
        {
            if (!ZPlannerManager.CloseProject()) return;

            OpenFileDialog openFileDialog = new OpenFileDialog();

            openFileDialog.Filter = "Excel Files (*.XLS;*.XLSX)|*.XLS;*.XLSX";
            //"Schematic Files (*.FFS)|*.FFS|Stackup Files (*.STK)|*.STK";
            if (!Options.TheOptions.UseLast) openFileDialog.InitialDirectory = Options.TheOptions.ExportPath;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != string.Empty)
            {
                ZPlannerManager.MessagePanel.ClearMessages();
                ZPlannerManager.StatusMenu.StartProgress("Importing a Stackup ...");
                Cursor currentCursor = Cursor.Current;
                Cursor.Current = Cursors.WaitCursor;

                ZPlannerManager.Commands.IsIgnoreCommands = true;
                bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;

                try
                {
                    TapestryReader parser = new TapestryReader(openFileDialog.FileName);
                    parser.Import();
                }
                finally
                {
                    ZPlannerManager.ResumeUpdateActiveStackupEvent();
                }

                if (!ProjectIsEmpty && !Project.StackupIsEmpty) Project.Stackup.Title = Path.GetFileNameWithoutExtension(openFileDialog.FileName);

                Cursor.Current = currentCursor;
                ZPlannerManager.StatusMenu.StopProgress("Importing a Stackup ...");
                ZPlannerManager.MessagePanel.AddMessage("Stackup was imported from " + openFileDialog.FileName + ".");

                UpdateActiveStackup();
                ZPlannerManager.Commands.Clear();
            }
        }

        internal static void ExportIPC2581()
        {
            ZStackup zs = ZPlannerManager.Project.Stackup;
            if (zs != null)
            {
                SaveFileDialog saveFileDialog = new SaveFileDialog();
                saveFileDialog.Filter = "IPC-2581 Files (*.XML)|*.XML";
                saveFileDialog.Title = "Export to IPC-2581 File";
                if (!Options.TheOptions.UseLast) saveFileDialog.InitialDirectory = Options.TheOptions.ExportPath;
                saveFileDialog.FileName = (!string.IsNullOrWhiteSpace(ZPlannerManager.ProjectFile)) ? Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile) : zs.Title;

                // If the file name is not an empty string open it for saving.
                if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != string.Empty)
                {
                    ZPlannerManager.StatusMenu.StartProgress("Exporting a Stackup ...");
                    Cursor currentCursor = Cursor.Current;
                    Cursor.Current = Cursors.WaitCursor;

                    IPC_Exporter exp = new IPC_Exporter(saveFileDialog.FileName);
                    exp.Export(); 
                    
                    Cursor.Current = currentCursor;
                    ZPlannerManager.StatusMenu.StopProgress("Exporting a Stackup ...");
                    ZPlannerManager.MessagePanel.AddMessage("Stackup was exported to " + saveFileDialog.FileName + ".");
                }
            }
        }

        internal static void SuspendFSHandlers()
        {
            SuspendPropertyChangedEvent();
            SuspendCollectionChangedEvent();
            SuspendMenuButtonCheckedEvent();
        }

        internal static void ResumeFSHandlers()
        {
            ZPlannerManager.ResumePropertyChangedEvent();
            ZPlannerManager.ResumeCollectionChangedEvent();
            ZPlannerManager.ResumeMenuButtonCheckedEvent();
        }

        internal static void UpdateActiveStackup()
        {
            if (IsIgnoreUpdateActiveStackup) return;

            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            SuspendFSHandlers();

            try
            {
                // check if stackup is valid
                if (ProjectIsEmpty || Project.StackupIsEmpty || Project.Stackup.Layers.Count <= 0) return;

                ZStackup stk = ZPlannerManager.Project.Stackup;

                if (!stk.IsSequentialLaminationSetByUser) ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();

                string message;
                bool bCheck = stk.IsValid(out message);

                //ensure solder mask layers are conformal
                //top
                ZLayer lOutter = stk.GetLayerOfStackup(0);
                if (!lOutter.isMetal())
                {
                    bool bConformal = lOutter.GetLayerType() == ZLayerType.SolderMask;
                    lOutter.SetLayerParameterValue(ZStringConstants.ParameterIDConformal, bConformal.ToString());
                }
                //bottom
                lOutter = stk.GetLayerOfStackup(stk.Layers.Count - 1);
                if (!lOutter.isMetal())
                {
                    bool bConformal = lOutter.GetLayerType() == ZLayerType.SolderMask;
                    lOutter.SetLayerParameterValue(ZStringConstants.ParameterIDConformal, bConformal.ToString());
                }

                //recalc dynamic properties

                //--if (ZPlannerManager.IsRoughness)
                //--{
                stk.DefineRoughness();
                //--}

                stk.UpdateSequentialLamination();
                stk.CalculatePrepregAdjustedThickness(ZPlannerManager.IsPressedThickness);

                if (ZPlannerManager.IsTrapezoidalTraces)
                {
                    stk.DefineTrapezoids();
                }

                if (bCheck)
                {
                    //run fx
                    FX.Run(stk);
                    if (ZPlannerManager.ContainerPanel != null && ZPlannerManager.StatusMenu != null)
                    {
                        ZPlannerManager.StatusMenu.SetStatus("Ready");
                    }
                }
                else
                {
                    //output message to status line
                    string err = String.Format("Error in stackup {0}: {1}", stk.ID, message);
                    //--ZPlannerManager.MessagePanel.AddMessage(err);
                    if (ZPlannerManager.ContainerPanel != null && ZPlannerManager.StatusMenu != null)
                    {
                        ZPlannerManager.StatusMenu.SetStatus(err);
                    }
                }

                if (ZPlannerManager.StackupPanel != null && ZPlannerManager.StackupPanel.Visible)
                {
                    ZPlannerManager.DRCPanel.UpdateReport();
                }
            }
            finally
            {
                ResumeFSHandlers();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        internal static void RecalculateThickness()
        {
            foreach (ZLayer layer in Project.Stackup.GetMetallLayerCollection())
            {
                ZLayerParameter layerParameter = layer.GetLayerParameter(ZStringConstants.ParameterIDThickness);
                if (layerParameter == null || layerParameter.IsEdited) continue;

                string thickness = null;
                string weight = layer.GetLayerParameterValue(ZStringConstants.ParameterIDCopperThickness);
                if (!string.IsNullOrWhiteSpace(weight) && TryToCalculateThicknesByCopperWeight(weight, out thickness) && !string.IsNullOrWhiteSpace(thickness))
                    layerParameter.Value = thickness;
            }
        }

        internal static bool TryToParseCopperWeight(string weight, out double dWeight)
        {
            dWeight = double.NaN;
            double dCooperThicknessNumerator;
            double dCooperThicknessDenominator;

            if (string.IsNullOrWhiteSpace(weight)) return false;

            if (weight.Contains('/'))
            {
                string[] cooperThicknessStrings = weight.Split('/');
                if (cooperThicknessStrings.Length != 2) return false;

                if (double.TryParse(cooperThicknessStrings[0].Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dCooperThicknessNumerator) &&
                    double.TryParse(cooperThicknessStrings[1].Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dCooperThicknessDenominator) &&
                    dCooperThicknessDenominator != 0)
                {
                    dWeight = dCooperThicknessNumerator / dCooperThicknessDenominator;
                    return true;
                }
            }
            else if (double.TryParse(weight.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dWeight))
                return true;

            return false;
        }

        internal static bool TryToCalculateThicknesByCopperWeight(string weight, out string thickness)
        {
            double dThickness;
            thickness = null;

            if (TryToParseCopperWeight(weight, out dThickness)) thickness = GetThicknesStringByCopperWeight(dThickness);

            return (!string.IsNullOrWhiteSpace(thickness));
        }

        internal static double GetThicknesByCopperWeight(double weight)
        {
            if (weight == 0.5)
                    return Options.TheOptions.copper_thickness_05oz;
            if (weight == 1.0)
                    return Options.TheOptions.copper_thickness_10oz;
            if (weight == 1.5)
                    return Options.TheOptions.copper_thickness_15oz;
            if (weight == 2.0)
                    return Options.TheOptions.copper_thickness_20oz;
            if (weight == 2.5)
                    return Options.TheOptions.copper_thickness_25oz;
            
            return weight * Units.fOzToMils;
        }

        internal static string GetThicknesStringByCopperWeight(double weight)
        {
            return GetThicknesByCopperWeight(weight).ToString("N" + Settings.Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture);
        }

        internal static void RunFXSandbox()
        {
            sbData d = sbData.SurfaceMicrostrip;
            FX.RunOnSandboxCX(ref d);

            d = sbData.BuriedMicrostrip;
            FX.RunOnSandboxCX(ref d);

            d = sbData.Stripline;
            FX.RunOnSandboxCX(ref d);
        }

        internal static void NewFromGenericStackup(string filePath)
        {
            if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath)) return;

            if (!ZPlannerManager.CloseProject()) return;

            ZPlannerManager.MessagePanel.ClearMessages();
            ZPlannerManager.StatusMenu.SetStatus("Creating a Generic Stackup ...");

            ZPlannerManager.Commands.IsIgnoreCommands = true;

            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.IsAutoMirror = false;

            try
            {
                ZPlannerManager.Project = DataProvider.Instance.OpenZPlannerProject(filePath);
                ZPlannerManager.Project.Stackup.CorrectDefaultParametersForInnerOuterLayers();
                ZPlannerManager.StackupPanel = new ZPlannerStackupPanel(ZPlannerManager.Project.Stackup);
                ZPlannerManager.IsSequentialLamination = ZPlannerManager.IsSequentialLaminationCanBeEnabled();
                //RecentFilesManager.AddToRecentFiles(filePath);
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
            }

            ZPlannerManager.StatusMenu.SetStatusReady();
            ZPlannerManager.MessagePanel.AddMessage(Project.Stackup.GetMetallLayerCount() + " layer Stackup was created.");
            
            UpdateActiveStackup();
            ZPlannerManager.Commands.Clear();
        }

        internal static void WriteSettings()
        {
            Options opt = Options.TheOptions;
            Settings_IO io = new Settings_IO();

            opt.enabledIsRoughness = IsRoughness;
            opt.enabledIsPressedThickness = IsPressedThickness;
            opt.enabledIsSequentialLamination = IsSequentialLamination;
            opt.enabledIsTrapezoidalTraces = IsTrapezoidalTraces;
            opt.enabledIsLossPlanning = IsLossPlanning;
            opt.enabledIsGlassPitch = IsGlassPitch; 
            opt.enabledIsGws = IsGws;
            opt.enabledIsCenterLineVisible = IsCenterLineVisible;
            opt.enabledIsCoreLocked = IsCoreLocked;
            opt.enabledIsAutoMirror = IsAutoMirror;
            opt.enabledIsHeadersVisible = IsHeadersVisible;
            opt.enabledIsColorDisabledCells = IsColorDisabledCells;

            opt.filtersCategory = CategoryFilter;

            io.WriteSettings(ref opt);
        }

        public static void ReadSettings()
        {
            if (Options.IsOptionsEmpty)
            {
                Settings_IO io = new Settings_IO();
                io.ReadSettings();
            }
        }

        internal static void InitDockPanels()
        {
            if (!OpenLastLayout()) CreateDefaultLayout();
            ZPlannerManager.StartPanel.Focus();
        }

        internal static void CreateDefaultLayout()
        {
            ZPlannerManager.StartPanel.Show(ContainerPanel.MainDockPanel);
            //ZPlannerManager.FXSandboxPanel.Show();
            //ZPlannerManager.FXSandboxPanel.Hide();
            //ZPlannerManager.DiffPanel.Show();
            //ZPlannerManager.DiffPanel.Hide();

            
            ZPlannerManager.ShowPropertiesPanel();
            ZPlannerManager.PropertiesPanel.Hide(); 
            ZPlannerManager.ShowProjectPanel();
            ZPlannerManager.ProjectPanel.Hide();
            ZPlannerManager.ShowMessagePanel();
            ZPlannerManager.MessagePanel.Hide();
            ZPlannerManager.DMLPanel.Show(StartPanel.Pane, DockAlignment.Bottom, 0.3);
            ZPlannerManager.DMLPanel.Hide();

            ZPlannerManager.ShowDRCPanel();
            ZPlannerManager.DRCPanel.Hide();
        }

        internal static bool OpenLastLayout()
        {
            string configFile = GetLastUsedLayoutFileName();
            /*if (File.Exists(configFile))
            {
                ContainerPanel.MainDockPanel.LoadFromXml(configFile, ZPlannerManager.DeserializeDockContent);
                return true;
            }*/
            return false;
        }

        internal static void SaveLastLayout()
        {
            string configFile = GetLastUsedLayoutFileName();
            //ContainerPanel.MainDockPanel.SaveAsXml(configFile);
        }

        private static string GetLastUsedLayoutFileName()
        {
            return Path.Combine(ZSettings.LayoutFolder, "LastUsed.layout");
        }

        internal static void SaveLayoutForTab(string activeMenuTab)
        {
            if (activeMenuTab == null) return;
            string filePath = GetLayoutFileNameForTab(activeMenuTab);
            ContainerPanel.MainDockPanel.SaveAsXml(filePath);
        }

        internal static bool OpenLayoutForTab(string activeMenuTab)
        {
            if (activeMenuTab == null) return false;
            string filePath = GetLayoutFileNameForTab(activeMenuTab);
            if (File.Exists(filePath))
            {
                ContainerPanel.MainDockPanel.SuspendLayout(true);
                ClearDockPanels();
                ContainerPanel.MainDockPanel.LoadFromXml(filePath, ZPlannerManager.DeserializeDockContent);
                ContainerPanel.MainDockPanel.ResumeLayout(true, true);

                //if (ZPlannerManager.StackupPanel != null && ZPlannerManager.StartPanel.IsActivated) ShowStackupPanel();
                //else if (ZPlannerManager.StartPanel == null && !ZPlannerManager.StackupPanel.IsActivated) ShowStackupPanel();
                return true;
            }
            return false;
        }

        private static string GetLayoutFileNameForTab(string activeMenuTab)
        {
            string folderPath = ZSettings.LayoutFolder;
            if (!Directory.Exists(folderPath)) Directory.CreateDirectory(folderPath);
            string filePath = Path.Combine(folderPath, activeMenuTab + ".default_layout");
            return filePath;
        }

        internal static void OpenDefaultLayoutForTab(string activeMenuTab)
        {
            switch(activeMenuTab)
            {
                case "Home":
                    ShowStartPageView();
                    break;
                case "Stackup":
                    ShowProjectView();
                    break;
                case "Library":
                    ShowDmlView();
                    break;
            }
        }

        internal static void OpenCustomLayout()
        {
            IDockContent focusedPanel = ContainerPanel.MainDockPanel.ActiveDocument;

            OpenFileDialog openFileDialog = new OpenFileDialog();
            string folderPath = ZSettings.LayoutFolder;
            if (!Directory.Exists(folderPath)) Directory.CreateDirectory(folderPath);
            openFileDialog.InitialDirectory = folderPath;
            openFileDialog.Filter = "Z-planner Layout|*.layout";
            openFileDialog.Title = "Open Z-planner Layout";
            openFileDialog.RestoreDirectory = true;

            if (openFileDialog.ShowDialog() == DialogResult.OK && openFileDialog.FileName != "")
            {
                ContainerPanel.MainDockPanel.SuspendLayout(true);
                ClearDockPanels();
                ContainerPanel.MainDockPanel.LoadFromXml(openFileDialog.FileName, DeserializeDockContent);
                
                ZPlannerManager.StackupPanel.Show(ContainerPanel.MainDockPanel);

                if (focusedPanel != null) (focusedPanel as Form).Focus();
                
                ContainerPanel.MainDockPanel.ResumeLayout(true, true);
            }
        }

        internal static void SaveCustomLayout()
        {
            SaveFileDialog saveFileDialog = new SaveFileDialog();
            string folderPath = ZSettings.LayoutFolder;
            if (!Directory.Exists(folderPath)) Directory.CreateDirectory(folderPath);
            saveFileDialog.InitialDirectory = folderPath;
            saveFileDialog.RestoreDirectory = true;
            saveFileDialog.Filter = "Z-planner Layout|*.layout";
            saveFileDialog.Title = "Save Z-planner Layout";
            saveFileDialog.OverwritePrompt = true;

            if (saveFileDialog.ShowDialog() == DialogResult.OK && saveFileDialog.FileName != "")
            {
                ContainerPanel.MainDockPanel.SaveAsXml(saveFileDialog.FileName);
            }
        }

        private static void ClearDockPanels()
        {
            ProjectPanel.DockPanel = null;
            PropertiesPanel.DockPanel = null;
            DMLPanel.DockPanel = null;
            MessagePanel.DockPanel = null;
            StartPanel.DockPanel = null;
            DRCPanel.DockPanel = null;
            //--FXSandboxPanel.DockPanel = null;
            
            if (ContainerPanel.MainDockPanel.DocumentStyle == DocumentStyle.SystemMdi)
            {
                foreach (Form form in ContainerPanel.MdiChildren)
                    form.Close();
            }
            else
            {
                foreach (IDockContent document in ContainerPanel.MainDockPanel.DocumentsToArray())
                {
                    document.DockHandler.DockPanel = null;
                }
            }

            foreach (var window in ContainerPanel.MainDockPanel.FloatWindows.ToList())
                window.Dispose();

            for (int i = ContainerPanel.MainDockPanel.Contents.Count - 1; i >= 0; --i)
            {
                IDockContent content = ContainerPanel.MainDockPanel.Contents[i];
                //content.DockHandler.Close();
                if (content != null && content.DockHandler != null) content.DockHandler.DockPanel = null;
            }

        }

        private static IDockContent GetContentFromPersistString(string persistString)
        {
            if (persistString == typeof(ZPlannerContainerPanel).ToString())
                return ZPlannerManager.ContainerPanel;
            else if (persistString == typeof(ZPlannerStackupPanel).ToString())
                return ZPlannerManager.StackupPanel;
            else if (persistString == typeof(ZPlannerStartPanel).ToString())
                return ZPlannerManager.StartPanel;
            else if (persistString == typeof(ZPlannerDMLPanel).ToString())
                return ZPlannerManager.DMLPanel;
            else if (persistString == typeof(ZPlannerMessagePanel).ToString())
                return ZPlannerManager.MessagePanel;
            else if (persistString == typeof(ZPlannerProjectPanel).ToString())
                return ZPlannerManager.ProjectPanel;
            else if (persistString == typeof(ZPlannerPropertiesPanel).ToString())
                return ZPlannerManager.PropertiesPanel;
            else if (persistString == typeof(DRCPanel).ToString())
                return ZPlannerManager.DRCPanel;
            //--else if (persistString == typeof(FXSandboxPanel).ToString())
                //--return ZPlannerManager.FXSandboxPanel;
            else
            {
                return null;
            }
        }

        internal static bool IsUserHaveAccessToMaterial(ZMaterial material)
        {
            ZLibraryCategory? library = material.GetMaterialCategory();
            if (library == null) return false;
            return IsUserHaveAccessToLibrary((ZLibraryCategory)library);
        }

        internal static bool IsUserHaveAccessToLibrary(ZLibraryCategory library)
        {
            if (ZPlannerManager.rights != null)
                switch(library)
                {
                    case ZLibraryCategory.Local:
                        return (ZPlannerManager.rights.maxRole ==  Rights.Role.User || ZPlannerManager.rights.maxRole ==  Rights.Role.Admin || ZPlannerManager.rights.maxRole ==  Rights.Role.SuperUser);
                    case ZLibraryCategory.Corporate:
                        return (ZPlannerManager.rights.maxRole == Rights.Role.Admin || ZPlannerManager.rights.maxRole == Rights.Role.SuperUser);
                    case ZLibraryCategory.ZZero:
                        return (ZPlannerManager.rights.maxRole == Rights.Role.SuperUser);
                }
            return false;
        }

        internal static bool IsUserHaveAccessToTapestryImport()
        {
            if (ZPlannerManager.rights != null) return (ZPlannerManager.rights.AllowTapestry);
            return false;
        }

        internal static ZLibraryCategory GetMeterialCategoryByUser()
        {
            switch (ZPlannerManager.rights.maxRole)
            {
                case Rights.Role.User:
                    return ZLibraryCategory.Local;
                case Rights.Role.Admin:
                    return ZLibraryCategory.Corporate;
                case Rights.Role.SuperUser:
                    return ZLibraryCategory.ZZero;
            }
            return ZLibraryCategory.Local;
        }

        internal static string GetMeterialCategoryStringByUser()
        {
            // New material iscreating as a Local, after its  category can be changed.
            return ZStringConstants.MaterialCategoryLocal;
        }

        internal static string GetFormatByParameter(ZParameter parameter)
        {
            string defaultFormat = "N2";
            if (parameter.ValueType == ZValueType.Percent) defaultFormat = "N" + Settings.Options.TheOptions.percentDigits;
            return GetFormatByParameterID(parameter.ID, defaultFormat);
        }

        internal static string GetFormatByParameterID(string id, string defaultFormat = "N2")
        {
            switch (id)
            {
                case ZStringConstants.ParameterIDLayerNumber:
                case ZStringConstants.ParameterIDWarpYarnCount:
                case ZStringConstants.ParameterIDFillYarnCount:
                case ZStringConstants.DMLParameterIDZCTE_alpha1:
                case ZStringConstants.DMLParameterIDZCTE_alpha2:
                case ZStringConstants.DMLParameterIDXYCTE:
                case ZStringConstants.DMLParameterIDT260:
                case ZStringConstants.DMLParameterIDT280:
                case ZStringConstants.ParameterIDZdiff_BitRate:
                case ZStringConstants.ParameterIDZdiff_UnitInterval:
                case ZStringConstants.ParameterIDZdiff_SkewTolerance:
                    return "N0";
                case ZStringConstants.DMLParameterIDWA:
                    return "N2";
                case ZStringConstants.ParameterIDBulkRes:
                    return "G";
                case ZStringConstants.ParameterIDZo_DielectricConstant:
                case ZStringConstants.ParameterIDZdiff_DielectricConstant:
                case ZStringConstants.DMLParameterIDDk_1GHz:
                case ZStringConstants.DMLParameterIDDk_3GHz:
                case ZStringConstants.DMLParameterIDDk_5GHz:
                case ZStringConstants.DMLParameterIDDk_7GHz:
                case ZStringConstants.DMLParameterIDDk_10GHz:
                case ZStringConstants.DMLParameterIDDk_20GHz:
                    return "N" + Settings.Options.TheOptions.dkDigits;
                case ZStringConstants.ParameterIDZo_LossTangent:
                case ZStringConstants.ParameterIDZdiff_LossTangent:
                case ZStringConstants.DMLParameterIDDf_1GHz:
                case ZStringConstants.DMLParameterIDDf_3GHz:
                case ZStringConstants.DMLParameterIDDf_5GHz:
                case ZStringConstants.DMLParameterIDDf_7GHz:
                case ZStringConstants.DMLParameterIDDf_10GHz:
                case ZStringConstants.DMLParameterIDDf_20GHz:
                    return "N" + Settings.Options.TheOptions.dfDigits;
                case ZStringConstants.ParameterIDZdiff_Zdiff:
                case ZStringConstants.ParameterIDZdiff_Zo:
                case ZStringConstants.ParameterIDZo_Zo:
                    return "N" + Settings.Options.TheOptions.impedanceDigits;
                case ZStringConstants.ParameterIDZo_TraceWidth:
                case ZStringConstants.ParameterIDZdiff_TraceWidth:
                case ZStringConstants.ParameterIDZdiff_TraceSpacing:
                case ZStringConstants.ParameterIDZdiff_TracePitch:
                case ZStringConstants.ParameterIDCopperThickness:
                case ZStringConstants.ParameterIDThickness:
                case ZStringConstants.ParameterIDZo_TraceLength:
                case ZStringConstants.ParameterIDZdiff_TraceLength:
                case ZStringConstants.ParameterIDPrepregThickness:
                case ZStringConstants.DMLParameterIDH:
                    return "N" + Settings.Options.TheOptions.lengthDigits;
                case "TBD_TBD_TBD_TBD":
                    return "N" + Settings.Options.TheOptions.weightDigits;
                case ZStringConstants.DMLParameterIDTg:
                case ZStringConstants.DMLParameterIDTd:
                    return "N" + Settings.Options.TheOptions.temperatureDigits;
                default:
                    return defaultFormat;
            }
        }

        public static void CreateCustomFilter()
        {
            List<ZParameter> parameters = Dml.Parameters.FindAll(x => x.IsPrivate == false);// && x.IsHidden == false);

            CustomFilterDialog dlg = new CustomFilterDialog(parameters);
            dlg.SetFilterAndSort(string.Empty, string.Empty, DMLPanel.GetCurrentFilter(), DMLPanel.GetCurrentSort());
            dlg.ShowDialog();
            if (dlg.DialogResult == DialogResult.OK) ZPlannerManager.ApplyCustomFilter(dlg.FilterPath);

            MainMenu.UpdateCustomFilters(dlg.FilterPath);
        }

        public static void RenameCustomFilter(string path, Point location)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return;

            RenameFilterDialog dlg = new RenameFilterDialog();
            dlg.SetCurrentPath(path);
            dlg.StartPosition = FormStartPosition.Manual;
            dlg.Location = location;
            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.OK) MainMenu.UpdateCustomFilters();
        }

        public static void EditCustomFilter(string path, Point location)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return;

            List<ZParameter> parameters = Dml.Parameters.FindAll(x => x.IsPrivate == false);// && x.IsHidden == false);
            string filterName = string.Empty;
            string filterString = string.Empty;
            string sortString = string.Empty;

            using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
            {
                filterName = filterReader.ReadLine();
                filterString = filterReader.ReadLine();
                sortString = filterReader.ReadLine();
            }

            if (filterString.StartsWith("FILTER:")) filterString = filterString.Substring("FILTER:".Length);
            else filterString = string.Empty;

            if (sortString.StartsWith("SORT:")) sortString = sortString.Substring("SORT:".Length);
            else sortString = string.Empty;

            CustomFilterDialog dlg = new CustomFilterDialog(parameters);
            dlg.SetFilterAndSort(path, filterName, filterString, sortString);
            dlg.StartPosition = FormStartPosition.Manual;
            dlg.Location = location;
            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.OK) ZPlannerManager.ApplyCustomFilter(dlg.FilterPath);

            MainMenu.UpdateCustomFilters(dlg.FilterPath);
        }

        public static void UpdateCustomFilter(string path, Point location)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return;

            //List<ZParameter> parameters = Dml.Parameters.FindAll(x => x.IsPrivate == false);// && x.IsHidden == false);
            string filterName = string.Empty;
            string filterString = string.Empty;
            string sortString = string.Empty;

            using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
            {
                filterName = filterReader.ReadLine();
                filterString = filterReader.ReadLine();
                sortString = filterReader.ReadLine();
            }

            if (filterString.StartsWith("FILTER:")) filterString = filterString.Substring("FILTER:".Length);
            else filterString = string.Empty;

            if (sortString.StartsWith("SORT:")) sortString = sortString.Substring("SORT:".Length);
            else sortString = string.Empty;

            string newFilterString = DMLPanel.GetCurrentFilter();
            string newSortString = DMLPanel.GetCurrentSort();

            if (newFilterString != filterString || newSortString != sortString) 
            {
                CustomFilterDialog dlg = new CustomFilterDialog();
                dlg.UpdateFilterAndSort(path, filterName, newFilterString, newSortString);
                
                if (dlg.DialogResult == DialogResult.OK) MainMenu.UpdateCustomFilters(path);
            }
        }

        public static void DeleteCustomFilter(string path, Point location)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return;

            string filterName = string.Empty;

            using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
            {
                filterName = filterReader.ReadLine();
            }

            DialogResult result = MessageBox.Show("Do you realy want to delete " + filterName + " filter?", "Confirmation", MessageBoxButtons.YesNo);
            if (result == DialogResult.Yes && File.Exists(path))
            {
                File.Delete(path);
                MainMenu.UpdateCustomFilters();
            }
        }

        public static void ApplyCustomFilter(string path)
        {
            if (string.IsNullOrEmpty(path) || !File.Exists(path)) return;

            List<ZParameter> parameters = Dml.Parameters.FindAll(x => x.IsPrivate == false);// && x.IsHidden == false);
            string filterName = string.Empty;
            string filterString = string.Empty;
            string sortString = string.Empty;

            using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
            {
                filterName = filterReader.ReadLine();
                filterString = filterReader.ReadLine();
                sortString = filterReader.ReadLine();
            }

            if (filterString.StartsWith("FILTER:")) filterString = filterString.Substring("FILTER:".Length);
            else filterString = string.Empty;

            if (sortString.StartsWith("SORT:")) sortString = sortString.Substring("SORT:".Length);
            else sortString = string.Empty;

            DMLPanel.SetCurrentFilterAndSort(filterString, sortString);
        }

        public static bool SyncronizeDML(bool isSilently)
        {
            if (!rights.AllowDmlUpdate) return false;
            if (!Options.TheOptions.allowZZeroSynchronization) return false;

            string localLibraryPath = ZSettings.DMLFile;
            if (String.IsNullOrEmpty(localLibraryPath)) return false;
            string networkLibraryPath = ZSettings.DMLNetworkFile;

            if (File.Exists(localLibraryPath))
            {
                DateTime localLibraryModifiedDate = File.GetLastWriteTimeUtc(localLibraryPath);
                DateTime networkLibraryModifiedDate = GetLastModifiedDateOfHttpFile(networkLibraryPath);

                if (localLibraryModifiedDate >= networkLibraryModifiedDate)
                {
                    if (!isSilently) MessageBox.Show("Your Dielectric Materials Library is already up to date.");
                    return false;
                }
            }

            return DownloadHttpFile(networkLibraryPath, localLibraryPath);
        }

        private static DateTime GetLastModifiedDateOfHttpFile (string httpURL)
        {
            try
            {
                using (HttpClient httpClient = new HttpClient())
                {
                    using (HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Head, new Uri(httpURL)))
                    {
                        HttpResponseMessage response = httpClient.SendAsync(request).Result;
                        if (response != null && response.Content != null && response.Content.Headers != null && response.Content.Headers.LastModified != null && response.Content.Headers.LastModified.HasValue) return response.Content.Headers.LastModified.Value.UtcDateTime;
                    }
                }
            }
            catch
            {
                return DateTime.MinValue;
            }

            return DateTime.MinValue;
        }

        private static bool DownloadHttpFile(string httpURL, string localPath)
        {
            //if (!Confirm("Z-zero Dielectric Material Library Update", "A new version of the Z-zero dielectric material library is available." + Environment.NewLine + "Select “Yes” to perform the update.")) return false;
            ConfirmDmlSynchronizationDialog dlg = new ConfirmDmlSynchronizationDialog();
            dlg.StartPosition = FormStartPosition.CenterScreen;
            dlg.ShowDialog();

            if (dlg.DialogResult == DialogResult.Ignore)
            {
                Options.TheOptions.allowZZeroSynchronization = false;
                return false;
            }

            if (dlg.DialogResult != DialogResult.OK) return false;

            ZPlannerManager.StatusMenu.StartProgress("Z-zero Dielectric Material Library updating ...", true); 
            using (var httpClient = new HttpClient())
            {
                using (HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, httpURL))
                {
                    if (File.Exists(localPath)) File.Copy(localPath, localPath + ".backup", true);
                    try
                    {
                        using (Stream contentStream = (httpClient.SendAsync(request).Result).Content.ReadAsStreamAsync().Result,
                            stream = new FileStream(localPath, FileMode.Create, FileAccess.Write, FileShare.None))
                        {

                            contentStream.CopyToAsync(stream);
                            MessagePanel.AddMessage("Z-zero dielectric material library was successfully updated.");
                            return true;
                        }
                    }
                    catch (Exception ex)
                    {
                        if (File.Exists(localPath + ".backup")) File.Copy(localPath + ".backup", localPath, true);
                        MessagePanel.AddMessage(string.Format("Error accured on Z-zero dielectric material library updating: ({0}).", ex.Message));
                        return false;
                    }
                    finally
                    {
                        ZPlannerManager.StatusMenu.StopProgress("Z-zero dielectric material library updating ..."); 
                    }
                }
            }
        }

        public static void ReloadDML()
        {
            ZPlannerManager.StatusMenu.StartProgress("Syncronize Z-zero library.", true);
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            ZPlannerManager.SuspendCollectionChangedEvent();

            try{
                Dml.ClearLibrary(ZLibraryCategory.ZZero);
                IsCorporateDeltaLoaded = false;

                string zzeroLibraryPath = ZSettings.DMLFile;
                if (String.IsNullOrEmpty(zzeroLibraryPath) || !File.Exists(zzeroLibraryPath)) return;

                ZMaterialLibrary dml_zzero = DataProvider.Instance.OpenZMaterialLibrary(zzeroLibraryPath);
                Dml.AddLibrary(dml_zzero, ZLibraryCategory.ZZero);

                string corporateLibraryPath = Options.TheOptions.DML_NetworkPath;
                if (!String.IsNullOrEmpty(corporateLibraryPath))
                {
                    string corporateLibraryFolder = Path.GetDirectoryName(corporateLibraryPath);
                    string corporateLibraryName = Path.GetFileNameWithoutExtension(corporateLibraryPath);
                    string delta_file = Path.Combine(corporateLibraryFolder, corporateLibraryName + ".delta");

                    if (File.Exists(delta_file))
                    {
                        LoadZoDelta(delta_file);
                        IsCorporateDeltaLoaded = true;
                    }
                }
            
                ZPlannerManager.ResumeCollectionChangedEvent();
                Dml.Materials.OnCollectionChanged(new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
            finally
            {
                ZPlannerManager.ResumeCollectionChangedEvent();
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                ZPlannerManager.StatusMenu.StopProgress("Syncronize Z-zero library.");
            }
        }

        public struct xy
        {
            public double x;
            public double y;
            public xy(double a, double b)
            {
                x = a; y = b;
            }
        }
        public static List<xy>  TableFunction(string expression)
        {
            List<xy> points = new List<xy>();
            double singleValue = 0;
            if (string.IsNullOrWhiteSpace(expression) || !expression.Contains(":") && !expression.Contains(";") && double.TryParse(expression.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out singleValue))
            {//one value -> F(a) = Const
                xy point = new xy(0, singleValue);
                points.Add(point);
            }
            else
            {
                //sorted by argument table -> F(a)  a:F;a:F;a:F;.
                Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(expression))
                {
                    if (!frequencyValueMatch.Success) continue;
                    string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                    string valueString = frequencyValueMatch.Groups["value"].Value;
                    double frequencyDouble;
                    double valueDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble))
                    {
                        xy point = new xy(frequencyDouble, valueDouble);
                        points.Add(point);
                    }
                }
            }
            return points;
        }

        public class cTableFunctionParser
        {
            private bool bSingle, bValid;
            private double dSingleValue;
            private string expression;
            //--NewtonPoly poly;
            Spline spline;

            public cTableFunctionParser(string exprString)
            {
                //--poly = new NewtonPoly();
                spline = new Spline();

                expression = exprString;
                if (string.IsNullOrWhiteSpace(expression) || !expression.Contains(":") && !expression.Contains(";") && double.TryParse(expression.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dSingleValue))
                {//one value -> F(a) = Const
                    bSingle = true;
                    bValid = true;
                }
                else
                {
                    //sorted by argument table -> F(a)  a:F;a:F;a:F;.
                    bValid = false;
                    Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                    foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(expression))
                    {
                        if (!frequencyValueMatch.Success) continue;
                        string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                        string valueString = frequencyValueMatch.Groups["value"].Value;
                        double frequencyDouble;
                        double valueDouble;
                        if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                            double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble))
                        {
                            //--poly.Add(frequencyDouble, valueDouble);
                            spline.Add(frequencyDouble, valueDouble);
                            bValid = true;
                        }
                    }
                    spline.Define();
                }
            }

            public double GetFunc(double argument)
            {
                if (bValid)
                {
                    if (bSingle)
                    {
                        return dSingleValue;
                    }
                    else
                    {
                        return spline.Interpolate(argument);//--poly.F(argument);
                    }
                }
                return double.NaN;
            }

            public double GetXmin()
            {
                if (bSingle) return double.NaN;
                return spline.GetXMin();
            }

            public double GetXmax()
            {
                if (bSingle) return double.NaN;
                return spline.GetXMax();
            }
        }

        public static bool TryParseDependedValue(string stringValue, double argument, out double doubleValue)
        {
            double singleValue = double.NaN;

            if (string.IsNullOrWhiteSpace(stringValue) || !stringValue.Contains(":") && !stringValue.Contains(";") && double.TryParse(stringValue.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out singleValue))
            {//one value -> F(a) = Const
                doubleValue = singleValue;
                return true;
            }
            else //sorted by argument table -> F(a)  a:F;a:F;a:F;..
            {
                //--NewtonPoly poly = new NewtonPoly();
                Spline spline = new Spline();
                bool bOK = false;

                Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(stringValue))
                {
                    if (!frequencyValueMatch.Success) continue;
                    string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                    string valueString = frequencyValueMatch.Groups["value"].Value;
                    double frequencyDouble;
                    double valueDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble))
                    {
                        //--poly.Add(frequencyDouble, valueDouble);
                        spline.Add(frequencyDouble, valueDouble);
                        bOK = true;
                    }
                }
                //interpolation
                if (bOK)
                {
                    spline.Define();
                    doubleValue = spline.Interpolate(argument);//--poly.F(argument);
                    return true;
                }
            }
            
            doubleValue = double.NaN;
            return false;
        }

        public static bool GetFirstValueFromTable(string stringValue, out double val)
        {
            double singleValue = double.NaN;

            if (string.IsNullOrWhiteSpace(stringValue) || !stringValue.Contains(":") && !stringValue.Contains(";") && double.TryParse(stringValue.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out singleValue))
            {//one value -> F(a) = Const
                val = singleValue;
                return true;
            }
            else //sorted by argument table -> F(a)  a:F;a:F;a:F;..
            {
                Regex frequencyValuePattern = new Regex(@"(?<frequency>[^:;]+?):(?<value>[^:;]+?);", RegexOptions.Compiled | RegexOptions.Singleline);
                foreach (Match frequencyValueMatch in frequencyValuePattern.Matches(stringValue))
                {
                    if (!frequencyValueMatch.Success) continue;
                    string frequencyString = frequencyValueMatch.Groups["frequency"].Value;
                    string valueString = frequencyValueMatch.Groups["value"].Value;
                    double frequencyDouble;
                    double valueDouble;
                    if (double.TryParse(frequencyString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out frequencyDouble) &&
                        double.TryParse(valueString.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out valueDouble))
                    {
                        val = valueDouble;
                        return true;
                    }
                }
            }

            val = double.NaN;
            return false;
        }

        public static bool IsIgnorePanelVisibleChanged{ get; set; }

        public static void Panel_VisibleChanged(object sender, EventArgs e)
        {
            MainMenu.SelectView();
        }

        public static double GetProductVersion()
        {
            int majorVersion = 1,  minorVersion = 0, buildVersion = 0;
            DateTime buildDate = DateTime.Now;
            double dValue;

            if (GetProductVersionInfo(ref majorVersion, ref minorVersion, ref buildVersion, ref buildDate) && double.TryParse(string.Format("{0}.{1}", majorVersion, minorVersion), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
                return dValue;
            else return 1.0;
        }

        public static bool GetProductVersionInfo(ref int majorVersion, ref int minorVersion, ref int buildVersion, ref DateTime buildDate)
        {
            try
            {
                string productFileLocation = Assembly.GetExecutingAssembly().Location;
                FileVersionInfo fileVersionInfo = FileVersionInfo.GetVersionInfo(productFileLocation);
                majorVersion = fileVersionInfo.ProductMajorPart;
                minorVersion = fileVersionInfo.ProductMinorPart;
                buildVersion = fileVersionInfo.ProductBuildPart;
                FileInfo fileInfo = new FileInfo(productFileLocation);
                buildDate = fileInfo.LastWriteTime;
                return true;
            }
            catch { return false; }
        }

        public static bool GetFSVersionInfo(ref int majorVersion, ref int minorVersion, ref int buildVersion, ref DateTime buildDate)
        {
            string stringVersion = ZSettings.FSBuildVersion;
            string stringDate = ZSettings.FSBuildDate;

            if (string.IsNullOrWhiteSpace(stringVersion) || string.IsNullOrWhiteSpace(stringDate)) return false;

            string[] versionStrings = stringVersion.Split('.');
            if (versionStrings.Length < 3) return false;

            int[] versionNumbers = new int[3];

            for (int i = 0; i < 3; ++i)
            {
                if (!int.TryParse(versionStrings[i], out versionNumbers[i])) return false;
            }

            DateTime dt;
            if (!DateTime.TryParse(stringDate, out dt)) return false;

            majorVersion = versionNumbers[0];
            minorVersion = versionNumbers[1];
            buildVersion = versionNumbers[2];
            buildDate = dt;

            return true;
        }

        internal static void OpenTutorial()
        {
            switch (ActiveMenuTab.Value)
            {
                case "Home":
                    OpenTutorial("topic_1");
                    break;
                case "Stackup":
                    OpenTutorial("topic_5");
                    break;
                case "Library":
                    OpenTutorial("topic_6");
                    break;
                default:
                    Help.ShowHelp(ContainerPanel, ZSettings.TutorialFile);
                    break;
            }
        }

        internal static void OpenTutorial(string topic)
        {
            Help.ShowHelp(ContainerPanel, ZSettings.TutorialFile, HelpNavigator.Topic, topic + ".html");
        }

        internal static void InitSwitches()
        {
            bool isIgnoreActive = SuspendUpdateActiveStackupEvent();

            try
            {
                if (Options.TheOptions.allowStackupSwitches && Project.Stackup.IsFileOpened)
                {
                    ZStackup stackup = Project.Stackup;
                    SetCopperRoughness(stackup.IsRoughness);
                    SetPressedThickness(stackup.IsPressedThickness);
                    SetSequentialLamination(stackup.IsSequentialLamination);
                    SetTrapezoidalTraces(stackup.IsTrapezoidalTraces);
                    SetLossPlanning(stackup.IsLossPlanning);
                    ShowGlassPitch(stackup.IsGlassPitch, stackup.IsGws);
                    ShowCenterLine(stackup.IsCenterLineVisible);
                    IsCoreLocked = stackup.IsCoreLocked;
                    TryAutoMirror(stackup.IsAutoMirror);
                    ShowHeaders(stackup.IsHeadersVisible);
                    ShowDisabledCells(stackup.IsColorDisabledCells);
                }
                else
                {
                    SetCopperRoughness(Options.TheOptions.enabledIsRoughness);
                    SetPressedThickness(Options.TheOptions.enabledIsPressedThickness);
                    SetSequentialLamination(Options.TheOptions.enabledIsSequentialLamination);
                    SetTrapezoidalTraces(Options.TheOptions.enabledIsTrapezoidalTraces);
                    SetLossPlanning(Options.TheOptions.enabledIsLossPlanning);
                    ShowGlassPitch(Options.TheOptions.enabledIsGlassPitch, Options.TheOptions.enabledIsGws);
                    ShowCenterLine(Options.TheOptions.enabledIsCenterLineVisible);
                    IsCoreLocked = Options.TheOptions.enabledIsCoreLocked;
                    TryAutoMirror(Options.TheOptions.enabledIsAutoMirror);
                    ShowHeaders(Options.TheOptions.enabledIsHeadersVisible);
                    ShowDisabledCells(Options.TheOptions.enabledIsColorDisabledCells);
                }
            }
            finally
            {
                ResumeUpdateActiveStackupEvent(isIgnoreActive);
                UpdateActiveStackup();
            }
        }

        internal static void InitFilters()
        {
            CategoryFilter = Options.TheOptions.filtersCategory;
        }

        private static void ShowHideComments(ZStackup stackup)
        {
            ZParameter commentsParameter = null;

            foreach (ZLayer layer in stackup.Layers)
            {
                if (layer == null) continue;
                ZLayerParameter commentsLayerParameter = layer.LayerParameters.Find(x => x.ID == ZStringConstants.ParameterIDComments);
                if (commentsLayerParameter != null)
                {
                    if (!string.IsNullOrWhiteSpace(commentsLayerParameter.Value))
                    {
                        commentsLayerParameter.Parameter.IsHidden = false;
                        return;
                    }

                    if (commentsParameter == null) commentsParameter = commentsLayerParameter.Parameter;
                }
            }

            if (commentsParameter != null) commentsParameter.IsHidden = true;
        }

        internal static bool ConfirmYesNo(string title, string message)
        {
            return MessageBox.Show(message, title, MessageBoxButtons.YesNo, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == System.Windows.Forms.DialogResult.Yes;
        }

        internal static bool ConfirmOKCancel(string title, string message)
        {
            return MessageBox.Show(message, title, MessageBoxButtons.OKCancel, MessageBoxIcon.Question, MessageBoxDefaultButton.Button1) == System.Windows.Forms.DialogResult.OK;
        }

        internal static void NoteOK(string title, string message)
        {
            MessageBox.Show(message, title, MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        internal static void CalculateSequentialLaminationParams(out int x, out int n)
        {
            x = 0;
            n = 0;
            if (!ProjectIsEmpty && !Project.StackupIsEmpty)
            {
                x = Project.Stackup.SequentialLamination();
                if (x < 0) x = 0;
                n = Project.Stackup.GetMetallLayerCount() - (2 * x);
                if (n < 0) n = 0;
            }
        }

        internal static bool IsSequentialLaminationCanBeEnabled()
        {
            int x, n;
            CalculateSequentialLaminationParams(out x, out n);

            return (x > 0);
        }
    }
}
