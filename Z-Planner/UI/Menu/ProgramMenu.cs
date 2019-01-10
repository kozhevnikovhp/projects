using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.UI.Recent;
using ZZero.ZPlanner.UI.Dialogs;
using System.IO;
using ZZero.ZPlanner.Utils;
using System.Text.RegularExpressions;
using System.Globalization;
using ZZero.ZPlanner.Settings;

namespace ZZero.ZPlanner.UI.Menu
{
    public partial class ProgramMenu : UserControl
    {
        Dictionary<string, RibbonItem> ShortcutDictionary = new Dictionary<string, RibbonItem>();

        public Ribbon RibbonMenu { get { return programRibbonMenu; } }

        public ProgramMenu()
        {
            InitializeComponent();
            ZPlannerManager.MainMenu = this;
            InitShortcut();
            InitMenu();

            FillRecentFilesList();
            UpdateGenericStackups();
            UpdateCustomFilters();
            UpdateMenu();
            RecentFilesManager.RecentFilesChanged += RecentFilesManager_RecentFilesChanged;
            ZPlannerManager.Commands.CommandsChanged += Commands_CommandsChanged;
            ZPlannerManager.PropertyChanged += ZPlannerManager_PropertyChanged;

            ZPlannerManager.Commands.Clear();
        }

        public event MenuButtonClickedEventHandler MenuButtonClicked;
        public event MenuButtonCheckedEventHandler MenuButtonChecked;

        internal void NotifyMenuButtonClicked(object sender, EventArgs e)
        {
            RibbonButton button = sender as RibbonButton;
            if (button == null) return;

            if (MenuButtonClicked != null)
            {
                MenuButtonClicked(button, new MenuButtonClickedEventArgs(button.Value));
            }

            if (button.CheckOnClick && MenuButtonChecked != null)
            {
                MenuButtonChecked(button, new MenuButtonCheckedEventArgs(button.Value, button.Checked));
            }
        }

        void Commands_CommandsChanged(object sender, Commands.CommandsEventArgs e)
        {
            bool undoEnabled = ZPlannerManager.Commands.UndoCount > 0;
            bool redoEnabled = ZPlannerManager.Commands.RedoCount > 0;

            this.undoRibbonButton.Enabled = undoEnabled;
            this.qaUndoRibbonButton.Enabled = undoEnabled;

            this.redoRibbonButton.Enabled = redoEnabled;
            this.qaRedoRibbonButton.Enabled = redoEnabled;  
          
            this.undoRibbonButton.ToolTip = ZPlannerManager.Commands.UndoToolTip;
            this.qaUndoRibbonButton.ToolTip = ZPlannerManager.Commands.UndoToolTip;
            this.redoRibbonButton.ToolTip = ZPlannerManager.Commands.RedoToolTip;
            this.qaRedoRibbonButton.ToolTip = ZPlannerManager.Commands.RedoToolTip;
        }

        void ZPlannerManager_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsRoughness")
            {
                if (this.copperRoughnessShowRibbonButton.Checked != ZPlannerManager.IsRoughness) this.copperRoughnessShowRibbonButton.Checked = ZPlannerManager.IsRoughness; //this.copperRoughnessShowRibbonButton.PerformClick();
                return;
            }

            if (e.PropertyName == "IsTrapezoidalTraces")
            {
                if (this.etchEffectsIncludeRibbonButton.Checked != ZPlannerManager.IsTrapezoidalTraces) this.etchEffectsIncludeRibbonButton.Checked = ZPlannerManager.IsTrapezoidalTraces;  //this.etchEffectsIncludeRibbonButton.PerformClick();
                return;
            }

            if (e.PropertyName == "IsPressedThickness")
            {
                if (this.pressedThicknessIncludeRibbonButton.Checked != ZPlannerManager.IsPressedThickness)
                {
                    this.pressedThicknessIncludeRibbonButton.Checked = ZPlannerManager.IsPressedThickness; //this.pressedThicknessIncludeRibbonButton.PerformClick();
                    this.sequentialLaminationIncludeRibbonButton.Enabled = ZPlannerManager.IsPressedThickness;
                }

                return;
            }

            if (e.PropertyName == "IsSequentialLamination")
            {
                if (this.sequentialLaminationIncludeRibbonButton.Checked != ZPlannerManager.IsSequentialLamination) this.sequentialLaminationIncludeRibbonButton.Checked = ZPlannerManager.IsSequentialLamination; //this.sequentialLaminationIncludeRibbonButton.PerformClick();
                return;
            }

            if (e.PropertyName == "IsGlassPitch" || e.PropertyName == "IsGws")
            {
                glassPitchRibbonButton.Checked = (ZPlannerManager.IsGlassPitch || ZPlannerManager.IsGws);

                if (e.PropertyName == "IsGlassPitch")
                {
                    glassPitchShowRibbonButton.Checked = ZPlannerManager.IsGlassPitch;
                }
                return;
            }

            if (e.PropertyName == "IsHeadersVisible")
            {
                if (this.gridHeadersRibbonButton.Checked != ZPlannerManager.IsHeadersVisible) this.gridHeadersRibbonButton.Checked = ZPlannerManager.IsHeadersVisible;
                return;
            }

            if (e.PropertyName == "IsSinglesVisible")
            {
                if (this.showSinglesRibbonButton.Checked != ZPlannerManager.IsSinglesVisible) this.showSinglesRibbonButton.Checked = ZPlannerManager.IsSinglesVisible;
                return;
            }

            if (e.PropertyName == "IsPairsVisible")
            {
                if (this.showPairsRibbonButton.Checked != ZPlannerManager.IsPairsVisible) this.showPairsRibbonButton.Checked = ZPlannerManager.IsPairsVisible;
                return;
            }

            if (e.PropertyName == "IsColorDisabledCells")
            {
                if (this.disabledCellRibbonButton.Checked != ZPlannerManager.IsColorDisabledCells) this.disabledCellRibbonButton.Checked = ZPlannerManager.IsColorDisabledCells;
                return;
            }

            if (e.PropertyName == "IsCenterLineVisible")
            {
                if (this.centerLineRibbonButton.Checked != ZPlannerManager.IsCenterLineVisible) this.centerLineRibbonButton.Checked = ZPlannerManager.IsCenterLineVisible;
                return;
            }

            if (e.PropertyName == "IsAutoMirror")
            {
                if (this.autoMirrorStackupRibbonButton.Checked != ZPlannerManager.IsAutoMirror) this.autoMirrorStackupRibbonButton.Checked = ZPlannerManager.IsAutoMirror;
                return;
            }

            if (e.PropertyName == "IsCoreLocked")
            {
                if (this.coreLockingRibbonButton.Checked != ZPlannerManager.IsCoreLocked) this.coreLockingRibbonButton.Checked = ZPlannerManager.IsCoreLocked;
                return;
            }

            if (e.PropertyName == "IsLossPlanning")
            {
                if (this.lossPlanningRibbonButton.Checked != ZPlannerManager.IsLossPlanning) this.lossPlanningRibbonButton.Checked = ZPlannerManager.IsLossPlanning;
                return;
            }
        }

        public void InitShortcut()
        {

            this.qaOpenStackupRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaOpenStackupRibbonButton.Shortcut = "Ctrl+O";

            this.qaSaveStackupRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaSaveStackupRibbonButton.Shortcut = "Ctrl+S";

            this.qaSaveAndEmailRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaSaveAndEmailRibbonButton.Shortcut = "Ctrl+Shift+E";

            this.qaUndoRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaUndoRibbonButton.Shortcut = "Ctrl+Z";

            this.qaRedoRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaRedoRibbonButton.Shortcut = "Ctrl+Y";

            this.qaCutRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaCutRibbonButton.Shortcut = "Ctrl+X";

            this.qaCopyRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaCopyRibbonButton.Shortcut = "Ctrl+C";

            this.qaPasteRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaPasteRibbonButton.Shortcut = "Ctrl+V";

            this.qaPrintPreviewRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaPrintPreviewRibbonButton.Shortcut = "Ctrl+P";

            this.qaTutorialRibbonButton.ShortcutChanged += programRibbonMenuItem_ShortcutChanged;
            this.qaTutorialRibbonButton.Shortcut = "F1";
        }

        public void InitMenu()
        {
            programRibbonMenu.Dock = DockStyle.Fill;
            showViaSpansRibbonButton.Checked = ZPlannerManager.IsViasVisible;
            showSinglesRibbonButton.Checked = ZPlannerManager.IsSinglesVisible;
            showPairsRibbonButton.Checked = ZPlannerManager.IsPairsVisible;
            gridHeadersRibbonButton.Checked = ZPlannerManager.IsHeadersVisible;
            centerLineRibbonButton.Checked = ZPlannerManager.IsCenterLineVisible;
            disabledCellRibbonButton.Checked = ZPlannerManager.IsColorDisabledCells;

            if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.ZZero))
            {
                if (ZPlannerManager.rights.AllowDmlSave) 
                    this.libraryEditsRibbonPanel.Items.Add(this.saveZoLibRibbonButton);
                // TBD
                //this.libraryEditsRibbonPanel.Items.Add(this.encryptSaveRibbonButton);
                //this.libraryEditsRibbonPanel.Items.Add(this.uploadToWebRibbonButton);
            }
            else if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Corporate))
            {
                this.libraryEditsRibbonPanel.Items.Add(this.saveCorpLibRibbonButton);
                this.libraryEditsRibbonPanel.Items.Add(this.uploadCorpLibRibbonButton);

                this.libraryEditsRibbonPanel.Items.Add(this.syncZ0LibRibbonButton);
                if (!ZPlannerManager.rights.AllowDmlUpdate)
                {
                    this.syncZ0LibRibbonButton.Enabled = false;
                    this.syncZ0LibRibbonButton.ToolTip = "Please contact support@z-zero.com, if you would like to purchase software support, which is required for library updates.";
                }
                //this.libraryEditsRibbonPanel.Items.Add(this.hideZoLibRibbonButton);
            }
            else if (ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Local))
            {
                /*if (ZPlannerManager.rights.AllowDmlUpdate)
                    this.libraryEditsRibbonPanel.Items.Add(this.syncZ0LibRibbonButton);*/
                this.libraryEditsRibbonPanel.Items.Add(this.syncZ0LibRibbonButton);
                if (!ZPlannerManager.rights.AllowDmlUpdate)
                {
                    this.syncZ0LibRibbonButton.Enabled = false;
                    this.syncZ0LibRibbonButton.ToolTip = "Please contact support@z-zero.com, if you would like to purchase software support, which is required for library updates.";
                }
            }

            if (!ZPlannerManager.IsUserHaveAccessToTapestryImport())
            {
                if (this.importStackupRibbonButton.DropDownItems.Contains(this.importTapestryRibbonButton))
                    this.importStackupRibbonButton.DropDownItems.Remove(this.importTapestryRibbonButton);

                if (this.exportStackupRibbonButton.DropDownItems.Contains(this.importTapestryRibbonButton2))
                    this.exportStackupRibbonButton.DropDownItems.Remove(this.importTapestryRibbonButton2);
            }

            foreach (string type in ZStringConstants.LayerType)
            {
                RibbonButton button = new RibbonButton(type);
                button.ToolTip = type;
                button.Value = "BlankRow";
                button.Tag = type;
                blankRowRibbonButton.DropDownItems.Add(button);
            }

            foreach (string type in ZStringConstants.DielectricType)
            {
                RibbonButton button = new RibbonButton(type);
                button.ToolTip = type;
                button.Value = "AddMaterial";
                button.Tag = type;
                addMaterialRibbonButton.DropDownItems.Add(button);
            }

            //UpdateGlassStyles();
        }

        private void UpdateGenericStackups()
        {
            string folderPath = ZSettings.GenericStackupsFolder;
            if (string.IsNullOrEmpty(folderPath)) return;

            if (!Directory.Exists(folderPath)) return;

            foreach (var filePath in Directory.EnumerateFiles(folderPath, "*.z0p"))
            {
                string fileName = Path.GetFileNameWithoutExtension(filePath);
                RibbonButton button = new RibbonButton(fileName);
                button.ToolTip = fileName;
                button.Value = "GenericStackups";
                button.Tag = Path.GetFullPath(filePath);
                genericStackupsRibbonButton.DropDownItems.Add(button);
            }
        }

        public void UpdateCustomFilters(string pathToBeChecked = null)
        {
            string folderPath = ZSettings.CustomFiltersFolder;
            if (string.IsNullOrEmpty(folderPath)) return;

            if (!Directory.Exists(folderPath)) return;

            filterToolbarRibbonButton.DropDownItems.Clear();

            // Create New Filter
            RibbonButton createButton = new RibbonButton("Create New Filter ...");
            createButton.ToolTip = "Create New Filter";
            createButton.Value = "CreateNewFilter";
            createButton.Tag = string.Empty;
            filterToolbarRibbonButton.DropDownItems.Add(createButton);

            RibbonSeparator renameSeparator = new RibbonSeparator("Rename Filter");
            filterToolbarRibbonButton.DropDownItems.Add(renameSeparator);

            Dictionary<string, string> filterFiles = new Dictionary<string, string>();

            foreach (var filePath in Directory.EnumerateFiles(folderPath, "*.filter"))
            {
                using (StreamReader filterReader = new StreamReader(File.Open(filePath, FileMode.Open, FileAccess.Read)))
                {
                    filterFiles.Add(filterReader.ReadLine(), filePath);
                }
            }

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = new RibbonButton("Rename: " + fileName + " ...");
                button.ToolTip = "Rename: " + fileName;
                button.Value = "RenameFilter";
                button.Tag = Path.GetFullPath(filePath);
                filterToolbarRibbonButton.DropDownItems.Add(button);
            }

            RibbonSeparator editSeparator = new RibbonSeparator("Edit Filter");
            filterToolbarRibbonButton.DropDownItems.Add(editSeparator);

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = new RibbonButton("Edit: " + fileName + " ...");
                button.ToolTip = "Edit: " + fileName;
                button.Value = "EditFilter";
                button.Tag = Path.GetFullPath(filePath);
                filterToolbarRibbonButton.DropDownItems.Add(button);
            }

            RibbonSeparator updateSeparator = new RibbonSeparator("Update Filter");
            filterToolbarRibbonButton.DropDownItems.Add(updateSeparator);

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = new RibbonButton("Update: " + fileName);
                button.ToolTip = "Update: " + fileName;
                button.Value = "UpdateFilter";
                button.Tag = Path.GetFullPath(filePath);
                filterToolbarRibbonButton.DropDownItems.Add(button);
            }

            RibbonSeparator deleteSeparator = new RibbonSeparator("Delete Filter");
            filterToolbarRibbonButton.DropDownItems.Add(deleteSeparator);

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = new RibbonButton("Delete: " + fileName);
                button.ToolTip = "Delete: " + fileName;
                button.Value = "DeleteFilter";
                button.Tag = Path.GetFullPath(filePath);
                filterToolbarRibbonButton.DropDownItems.Add(button);
            }

            /*RibbonSeparator applySeparator = new RibbonSeparator("Apply Filter");
            filterToolbarRibbonButton.DropDownItems.Add(applySeparator);

            foreach (var filePath in Directory.EnumerateFiles(folderPath, "*.filter"))
            {
                string fileName = Path.GetFileNameWithoutExtension(filePath);
                RibbonButton button = new RibbonButton("Apply: " + fileName);
                button.ToolTip = "Apply: " + fileName;
                button.Value = "ApplyFilter";
                button.Tag = Path.GetFullPath(filePath);
                filterToolbarRibbonButton.DropDownItems.Add(button);
            }
            */



            this.programRibbonMenu.SuspendUpdating();
            filterRibbonButtonList.Buttons.Clear();
            filterRibbonButtonList.DropDownItems.Clear();

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = GetFilterButton(fileName, filePath, pathToBeChecked);

                filterRibbonButtonList.DropDownItems.Add(button);
                button.RedrawItem();
            }

            foreach (var fileName in filterFiles.Keys)
            {
                string filePath = filterFiles[fileName];
                RibbonButton button = GetFilterButton(fileName, filePath, pathToBeChecked);                

                filterRibbonButtonList.Buttons.Add(button);
                button.RedrawItem();
            }

            if (filterFiles.Count == 0)
            {
                RibbonButton button = new RibbonButton();
                button.Text = "No custom filters";
                button.ToolTip = "No custom filters";
                button.Value = "NoCustomFilters";
                button.SmallMinimumSize = new System.Drawing.Size(100, 0);
                button.Style = RibbonButtonStyle.Normal;
                button.DrawIconsBar = false;
                button.Enabled = false;

                filterRibbonButtonList.Buttons.Add(button);
                button.RedrawItem();
            }

            filterRibbonButtonList.RedrawItem();
            this.programRibbonMenu.ResumeUpdating(true);
        }

        private RibbonButton GetFilterButton(string fileName, string filePath, string pathToBeChecked)
        {
            RibbonButton button = new RibbonButton(global::ZZero.ZPlanner.Properties.Resources.Filter_16);
            button.Text = fileName;
            button.ToolTip = fileName;
            button.Value = "ApplyFilter";
            button.Tag = Path.GetFullPath(filePath);
            button.SmallMinimumSize = new System.Drawing.Size(100, 0);
            button.CheckOnClick = true;
            button.Click += filterRibbonButton_Click;
            button.Style = RibbonButtonStyle.SplitDropDown;
            button.DrawIconsBar = false;
            if (filePath == pathToBeChecked) button.Checked = true;

            RibbonButton filterRename = new RibbonButton("Rename: " + button.Text + " ...");
            filterRename.ToolTip = "Rename: " + button.Text;
            filterRename.Value = "RenameFilter";
            filterRename.Tag = button.Tag;
            filterRename.Click += filterRename_Click;
            button.DropDownItems.Add(filterRename);

            RibbonButton filterEdit = new RibbonButton("Edit: " + button.Text + " ...");
            filterEdit.ToolTip = "Edit: " + button.Text;
            filterEdit.Value = "EditFilter";
            filterEdit.Tag = button.Tag;
            filterEdit.Click += filterEdit_Click;
            button.DropDownItems.Add(filterEdit);

            RibbonButton filterUpdate = new RibbonButton("Update: " + button.Text);
            filterUpdate.ToolTip = "Update: " + button.Text;
            filterUpdate.Value = "UpdateFilter";
            filterUpdate.Tag = button.Tag;
            filterUpdate.Click += filterUpdate_Click;
            button.DropDownItems.Add(filterUpdate);

            RibbonButton filterDelete = new RibbonButton("Delete: " + button.Text);
            filterDelete.ToolTip = "Delete: " + button.Text;
            filterDelete.Value = "DeleteFilter";
            filterDelete.Tag = button.Tag;
            filterDelete.Click += filterDelete_Click;
            button.DropDownItems.Add(filterDelete);

            return button;
        }

        public string GetCurrentCustomFilterName()
        {
            foreach(var button in filterRibbonButtonList.Buttons)
            {
                if (button.Checked) return button.Text;
            }

            return string.Empty;
        }

        private void filterRibbonButton_Click(object sender, EventArgs e)
        {
            RibbonButton item = sender as RibbonButton;
            if (item == null) return;

            /*MouseEventArgs me = e as MouseEventArgs;
            if (me != null && me.Button == MouseButtons.Right) 
            { 
                item.Checked = !item.Checked; 
                filterRibbonButton_RightClick(item, e as MouseEventArgs); 
                return; 
            }*/
            
            string filePath = item.Tag as string;
            if (string.IsNullOrWhiteSpace(filePath)) return;

            if (item.Checked)
            {
                foreach (RibbonButton button in filterRibbonButtonList.Buttons)
                {
                    if (button.Tag as string != filePath) button.Checked = false;
                }

                foreach (RibbonButton button in filterRibbonButtonList.DropDownItems)
                {
                    if (button.Tag as string != filePath) button.Checked = false;
                }

                ZPlannerManager.ApplyCustomFilter(item.Tag as string);

                foreach (RibbonButton button in filterRibbonButtonList.Buttons)
                {
                    if (button.Tag as string == filePath) button.Checked = true;
                }

                foreach (RibbonButton button in filterRibbonButtonList.DropDownItems)
                {
                    if (button.Tag as string == filePath) button.Checked = true;
                }
            }
            else
            {
                clearFiltersRibbonButton_Click(clearFiltersRibbonButton, EventArgs.Empty);
            }

            NotifyMenuButtonClicked(sender, e);
        }

        private void filterRibbonButton_RightClick(RibbonButton item, MouseEventArgs e)
        {
            /*
            item.DropDownItems.Clear();
            item.DrawIconsBar = false;

            RibbonButton filterRename = new RibbonButton("Rename: " + item.Text + " ...");
            filterRename.ToolTip = "Rename: " + item.Text;
            filterRename.Value = "RenameFilter";
            filterRename.Tag = item.Tag;
            filterRename.Click += filterRename_Click;
            item.DropDownItems.Add(filterRename);

            RibbonButton filterEdit = new RibbonButton("Edit: " + item.Text + " ...");
            filterEdit.ToolTip = "Edit: " + item.Text;
            filterEdit.Value = "EditFilter";
            filterEdit.Tag = item.Tag;
            filterEdit.Click += filterEdit_Click;
            item.DropDownItems.Add(filterEdit);

            RibbonButton filterDelete = new RibbonButton("Delete: " + item.Text);
            filterDelete.ToolTip = "Delete: " + item.Text;
            filterDelete.Value = "DeleteFilter";
            filterDelete.Tag = item.Tag;
            filterDelete.Click += filterDelete_Click;
            item.DropDownItems.Add(filterDelete);
            */

            /*item.Style = RibbonButtonStyle.DropDown;
            item.ShowDropDown();
            item.Style = RibbonButtonStyle.Normal;*/
        }

        void filterRename_Click(object sender, EventArgs e)
        {
            ZPlannerManager.RenameCustomFilter((sender as RibbonItem).Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterRibbonButtonList.Bounds.Bottom + 40));
        }

        void filterEdit_Click(object sender, EventArgs e)
        {
            ZPlannerManager.EditCustomFilter((sender as RibbonItem).Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterRibbonButtonList.Bounds.Bottom + 40));
        }

        void filterUpdate_Click(object sender, EventArgs e)
        {
            ZPlannerManager.UpdateCustomFilter((sender as RibbonItem).Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterRibbonButtonList.Bounds.Bottom + 40));
        }

        void filterDelete_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DeleteCustomFilter((sender as RibbonItem).Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterRibbonButtonList.Bounds.Bottom + 40));
        }

        internal void ClearCustomFilterList()
        {
            foreach (RibbonButton button in filterRibbonButtonList.Buttons)
            {
                button.Checked = false;
            }

            foreach (RibbonButton button in filterRibbonButtonList.DropDownItems)
            {
                button.Checked = false;
            }
        }

        internal void UpdateSingles(ZList<ZSingle> singles, string singleId)
        {
            this.programRibbonMenu.SuspendUpdating();
            singlesRibbonButtonList.Buttons.Clear();
            foreach (ZSingle single in singles)
            {
                RibbonButton item = new RibbonButton(global::ZZero.ZPlanner.Properties.Resources.Single_ended_161);
                item.Text = single.Title;
                item.ToolTip = single.Title;
                item.Value = "SetSingle";
                item.Tag = single;
                item.SmallMinimumSize = new System.Drawing.Size(100, 0);
                item.CheckOnClick = true;
                item.Click += singleRibbonButton_Click;
                if (singleId == single.ID) item.Checked = true;

                item.Style = RibbonButtonStyle.SplitDropDown;
                item.DrawIconsBar = false;

                RibbonButton singleRename = new RibbonButton("Rename: " + item.Text + " ...");
                singleRename.ToolTip = "Rename: " + item.Text;
                singleRename.Value = "RenameSingle";
                singleRename.Tag = item.Tag;
                singleRename.Click += singleRename_Click;
                item.DropDownItems.Add(singleRename);

                RibbonButton singleDelete = new RibbonButton("Delete: " + item.Text);
                singleDelete.ToolTip = "Delete: " + item.Text;
                singleDelete.Value = "DeleteSingle";
                singleDelete.Tag = item.Tag;
                singleDelete.Click += singleDelete_Click;
                item.DropDownItems.Add(singleDelete);

                singlesRibbonButtonList.Buttons.Add(item);
                item.RedrawItem();
            }
            //stackup_singleended_RibbonPanel
            singlesRibbonButtonList.RedrawItem();
            this.programRibbonMenu.ResumeUpdating(true);
        }

        void singleRename_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.RenameSingle((sender as RibbonItem).Tag as ZSingle, new Point(singlesRibbonButtonList.Bounds.X, singlesRibbonButtonList.Bounds.Bottom + 40));
        }

        void singleDelete_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.DeleteSingle((sender as RibbonItem).Tag as ZSingle, new Point(singlesRibbonButtonList.Bounds.X, singlesRibbonButtonList.Bounds.Bottom + 40));
        }

        internal void UpdateSingles(ZList<ZSingle> singles)
        {
            string singleID = "";
            foreach (RibbonButton button in singlesRibbonButtonList.Buttons)
            {
                if (button.Checked)
                {
                    ZSingle single = button.Tag as ZSingle;
                    if (single == null) continue;
                    singleID = single.ID;
                    break;
                }

            }

            UpdateSingles(singles, singleID);
        }

        internal void UpdatePairs(ZList<ZPair> pairs, string pairId)
        {
            this.programRibbonMenu.SuspendUpdating();
            pairsRibbonButtonList.Buttons.Clear();
            foreach (ZPair pair in pairs)
            {
                RibbonButton item = new RibbonButton(global::ZZero.ZPlanner.Properties.Resources.Differential_Pair_16);
                item.Text = pair.Title;
                item.ToolTip = pair.Title;
                item.Value = "SetPair";
                item.Tag = pair;
                item.SmallMinimumSize = new System.Drawing.Size(100, 0);
                item.CheckOnClick = true;
                item.Click += pairRibbonButton_Click;
                if (pairId == pair.ID) item.Checked = true;

                item.Style = RibbonButtonStyle.SplitDropDown;
                item.DrawIconsBar = false;

                RibbonButton pairRename = new RibbonButton("Rename: " + item.Text + " ...");
                pairRename.ToolTip = "Rename: " + item.Text;
                pairRename.Value = "RenamePair";
                pairRename.Tag = item.Tag;
                pairRename.Click += pairRename_Click;
                item.DropDownItems.Add(pairRename);

                RibbonButton pairDelete = new RibbonButton("Delete: " + item.Text);
                pairDelete.ToolTip = "Delete: " + item.Text;
                pairDelete.Value = "DeletePair";
                pairDelete.Tag = item.Tag;
                pairDelete.Click += pairDelete_Click;
                item.DropDownItems.Add(pairDelete);

                pairsRibbonButtonList.Buttons.Add(item);
                item.RedrawItem();
            }
            //stackup_diffpairs_RibbonPanel
            pairsRibbonButtonList.RedrawItem();
            this.programRibbonMenu.ResumeUpdating(true);
        }

        void pairRename_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.RenamePair((sender as RibbonItem).Tag as ZPair, new Point(pairsRibbonButtonList.Bounds.X, pairsRibbonButtonList.Bounds.Bottom + 40));
        }

        void pairDelete_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.DeletePair((sender as RibbonItem).Tag as ZPair, new Point(pairsRibbonButtonList.Bounds.X, pairsRibbonButtonList.Bounds.Bottom + 40));
        }

        internal void UpdatePairs(ZList<ZPair> pairs)
        {
            string pairID = "";
            foreach (RibbonButton button in pairsRibbonButtonList.Buttons)
            {
                if (button.Checked) 
                {
                    ZPair pair = button.Tag as ZPair;
                    if (pair == null) continue;
                    pairID = pair.ID;
                    break;
                }

            }

            UpdatePairs(pairs, pairID);
        }

        internal void SetActiveTabByValue(string value)
        {
            foreach (RibbonTab tab in RibbonMenu.Tabs)
            {
                if (tab.Value == value) { RibbonMenu.ActiveTab = tab; return; }
            }
        }

        internal void SetFrequency(double frequency)
        {
            string sValue = frequency.ToString("N" + Settings.Options.TheOptions.frequencyDigits, CultureInfo.InvariantCulture);
            if (this.frequencyRibbonTextBox.TextBoxText != sValue)
                this.frequencyRibbonTextBox.TextBoxText = sValue;
        }

        private void frequencyRibbonTextBox_TextBoxTextChanged(object sender, EventArgs e)
        {
            if (ZPlannerManager.ProjectIsEmpty || ZPlannerManager.Project.StackupIsEmpty) return;
            double dValue;
            if (double.TryParse(this.frequencyRibbonTextBox.TextBoxText.Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out dValue))
            {
                ZPlannerManager.Project.Stackup.Frequency = dValue;
                ZPlannerManager.PropertiesPanel.UpdateProperties();
            }
        }

        private void singleRibbonButton_Click(object sender, EventArgs e)
        {
            RibbonButton item = sender as RibbonButton;
            if (item == null) return;
            ZSingle single = item.Tag as ZSingle;
            if (single == null) return;

            if (item.Checked)
            {
                foreach (RibbonButton button in singlesRibbonButtonList.Buttons)
                {
                    if (button != item) button.Checked = false;
                }

                ZPlannerManager.ProjectPanel.SelectSingle(single.ID);
            }
            else
            {
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearSingle();
                ZPlannerManager.ProjectPanel.SelectStackup();
            }

            NotifyMenuButtonClicked(sender, e);
        }

        private void pairRibbonButton_Click(object sender, EventArgs e)
        {
            RibbonButton item = sender as RibbonButton;
            if (item == null) return;
            ZPair pair = item.Tag as ZPair;
            if (pair == null) return;

            if (item.Checked)
            {
                foreach (RibbonButton button in pairsRibbonButtonList.Buttons)
                {
                    if (button != item) button.Checked = false;
                }

                ZPlannerManager.ProjectPanel.SelectPair(pair.ID);
            }
            else
            {
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ClearPair();
                ZPlannerManager.ProjectPanel.SelectStackup();
            }

            NotifyMenuButtonClicked(sender, e);
        }

        public void UpdateMenu()
        {
            if (!ZPlannerManager.ProjectIsEmpty && !ZPlannerManager.Project.StackupIsEmpty && ZPlannerManager.StackupPanel != null)
            {
                autoMirrorRibbonButton.Enabled = true;
                autoMirrorStackupRibbonButton.Enabled = true; //ZPlannerManager.Project.Stackup.IsMirrored();
                //if (!autoMirrorRibbonButton.Enabled) ZPlannerManager.StackupPanel.IsAutoMirror = false;
                autoMirrorRibbonButton.Checked = ZPlannerManager.IsAutoMirror;
                autoMirrorStackupRibbonButton.Checked = ZPlannerManager.IsAutoMirror;
                roughnessRibbonButton.Enabled = true;
                roughnessRibbonButton.Checked = ZPlannerManager.IsRoughness;
                ZPlannerManager.StackupPanel.ShowRoughnessColumns(ZPlannerManager.IsRoughness);
                etchAdjustRibbonButton.Enabled = true;
                etchAdjustRibbonButton.Checked = ZPlannerManager.IsTrapezoidalTraces;
                coreLockingRibbonButton.Checked = ZPlannerManager.IsCoreLocked;
                pressedThicknessRibbonButton.Enabled = true;
                pressedThicknessRibbonButton.Checked = ZPlannerManager.IsPressedThickness;
                pageSetupRibbonOrbMenuItem.Enabled = true;
                printPreviewRibbonOrbMenuItem.Enabled = true;
                printRibbonOrbMenuItem.Enabled = true;
                qaPrintPreviewRibbonButton.Enabled = true;
                qaSaveAndEmailRibbonButton.Enabled = true;
                saveAsRibbonButton.Enabled = true;
                saveRibbonButton.Enabled = true;
                qaSaveStackupRibbonButton.Enabled = true;
                saveProjectRibbonOrbMenuItem.Enabled = true;
                saveProjectAsRibbonOrbMenuItem.Enabled = true;
                emailStackupRibbonOrbMenuItem.Enabled = true;
                closeProjectRibbonOrbMenuItem.Enabled = true;
                deleteRowRibbonButton.Enabled = (ZPlannerManager.StackupPanel.GetSelectedLayers().Count > 0); 
                frequencyRibbonTextBox.Enabled = true;
                showLViewRibbonButton.Enabled = true;
                projectView2RibbonButton.Enabled = true;
                libViewRibbonButton.Enabled = true;
                sendToStackupRibbonButton.Enabled = (ZPlannerManager.DMLPanel.GetSelectedMaterial() != null);
                copperRoughnessShowRibbonButton.Checked = ZPlannerManager.IsRoughness;
                pressedThicknessIncludeRibbonButton.Checked = ZPlannerManager.IsPressedThickness;
                sequentialLaminationIncludeRibbonButton.Checked = ZPlannerManager.IsSequentialLamination;
                etchEffectsIncludeRibbonButton.Checked = ZPlannerManager.IsTrapezoidalTraces;
                //if (ZPlannerManager.StackupPanel.IsGlassPitchShown() != glassPitchRibbonButton.Checked) glassPitchRibbonButton.PerformClick();
                //if (ZPlannerManager.StackupPanel.IsGWSShown() != gwsRibbonButton.Checked) gwsRibbonButton_Click(gwsRibbonButton, EventArgs.Empty);
                glassPitchRibbonButton.Checked = (ZPlannerManager.IsGlassPitch || ZPlannerManager.IsGws);
            }
            else
            {
                autoMirrorRibbonButton.Enabled = false;
                autoMirrorStackupRibbonButton.Enabled = false;
                autoMirrorRibbonButton.Checked = false;
                autoMirrorStackupRibbonButton.Checked = false;
                roughnessRibbonButton.Enabled = false;
                etchAdjustRibbonButton.Enabled = false;
                coreLockingRibbonButton.Checked = false;
                pressedThicknessRibbonButton.Enabled = false;
                pageSetupRibbonOrbMenuItem.Enabled = false;
                printPreviewRibbonOrbMenuItem.Enabled = false;
                printRibbonOrbMenuItem.Enabled = false;
                qaPrintPreviewRibbonButton.Enabled = false;
                qaSaveAndEmailRibbonButton.Enabled = false;
                saveAsRibbonButton.Enabled = false;
                saveRibbonButton.Enabled = false;
                qaSaveStackupRibbonButton.Enabled = false;
                saveProjectRibbonOrbMenuItem.Enabled = false;
                saveProjectAsRibbonOrbMenuItem.Enabled = false;
                emailStackupRibbonOrbMenuItem.Enabled = false;
                closeProjectRibbonOrbMenuItem.Enabled = false;
                deleteRowRibbonButton.Enabled = false;
                frequencyRibbonTextBox.Enabled = false;
                sendToStackupRibbonButton.Enabled = false;
                showLViewRibbonButton.Enabled = false;
                projectView2RibbonButton.Enabled = false;
                libViewRibbonButton.Enabled = false;
                copperRoughnessShowRibbonButton.Checked = false;
                pressedThicknessIncludeRibbonButton.Checked = false;
                sequentialLaminationIncludeRibbonButton.Checked = false;
                etchEffectsIncludeRibbonButton.Checked = false;
                glassPitchRibbonButton.Checked = false;
                //gwsRibbonButton.Checked = false;
            }
        }

        void RecentFilesManager_RecentFilesChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "RecentFiles") FillRecentFilesList();
        }

        private void programRibbonMenu_OrbDropDown_Opening(object sender, CancelEventArgs e)
        {
            FillBackupFilesList();
            FillAutosaveFilesList();
        }

        private void FillRecentFilesList()
        {
            this.programRibbonMenu.OrbDropDown.RecentItems.Clear();

            RibbonLabel label = new RibbonLabel();
            label.Text = "Recent Files";
            label.TextAlignment = RibbonItem.RibbonItemTextAlignment.Center;
            this.programRibbonMenu.OrbDropDown.RecentItems.Add(label);

            foreach (string file in RecentFilesManager.RecentFiles)
            {
                RibbonOrbRecentItem item = new RibbonOrbRecentItem(Path.GetFileName(file));
                item.Value = "RecentFile";
                item.ToolTipTitle = Path.GetFileName(file);
                item.ToolTip = Path.GetFullPath(file);
                item.Tag = Path.GetFullPath(file);
                item.Click += ribbonOrbRecentItem_Click;
                this.programRibbonMenu.OrbDropDown.RecentItems.Add(item);
            }
        }

        private void FillAutosaveFilesList()
        {
            this.autosaveRibbonOrbMenuItem.DropDownItems.Clear();

            RibbonLabel label = new RibbonLabel();
            label.Text = "Auto-saved Files";
            label.TextAlignment = RibbonItem.RibbonItemTextAlignment.Center;
            this.autosaveRibbonOrbMenuItem.DropDownItems.Add(label);

            string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "AutoSave");
            if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

            int itemCount = 0;
            foreach (string file in Directory.EnumerateFiles(dirPath, "*.z0p"))
            {
                RibbonOrbRecentItem item = new RibbonOrbRecentItem(Path.GetFileName(file));
                item.Value = "AutoSaveFile";
                item.ToolTipTitle = Path.GetFileName(file);
                item.ToolTip = Path.GetFullPath(file);
                item.Tag = Path.GetFullPath(file);
                item.Click += ribbonOrbRecentItem_Click;
                this.autosaveRibbonOrbMenuItem.DropDownItems.Add(item);
                ++itemCount;
            }

            if (itemCount > 0)
            {
                RibbonButton button = new RibbonButton();
                button.Text = "Clear Auto-saved Files";
                button.TextAlignment = RibbonItem.RibbonItemTextAlignment.Center;
                button.Click += clearAutoSave_Click;
                button.Checked = true;
                this.autosaveRibbonOrbMenuItem.DropDownItems.Add(button);
            }
        }

        private void FillBackupFilesList()
        {
            this.backupRibbonOrbMenuItem.DropDownItems.Clear();

            RibbonLabel label = new RibbonLabel();
            label.Text = "Backup Files";
            label.TextAlignment = RibbonItem.RibbonItemTextAlignment.Center;
            this.backupRibbonOrbMenuItem.DropDownItems.Add(label);

            string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "Backup");
            if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

            int itemCount = 0;
            foreach (string file in Directory.EnumerateFiles(dirPath, "*.z0p"))
            {
                RibbonOrbRecentItem item = new RibbonOrbRecentItem(Path.GetFileName(file));
                item.Value = "BackupFile";
                item.ToolTipTitle = Path.GetFileName(file);
                item.ToolTip = Path.GetFullPath(file);
                item.Tag = Path.GetFullPath(file);
                item.Click += ribbonOrbRecentItem_Click;
                this.backupRibbonOrbMenuItem.DropDownItems.Add(item);
                ++itemCount;
            }

            if (itemCount > 0)
            {
                RibbonButton button = new RibbonButton();
                button.Text = "Clear Backup Files";
                button.TextAlignment = RibbonItem.RibbonItemTextAlignment.Center;
                button.Click += clearBackup_Click;
                button.Checked = true;
                this.backupRibbonOrbMenuItem.DropDownItems.Add(button);
            }
        }

        void clearAutoSave_Click(object sender, EventArgs e)
        {
            string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "AutoSave");
            if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

            foreach (string file in Directory.EnumerateFiles(dirPath, "*.z0p"))
            {
                File.Delete(file);
            }
        }

        void clearBackup_Click(object sender, EventArgs e)
        {
            string dirPath = Path.Combine(Options.TheOptions.ProjectPath, "Backup");
            if (!Directory.Exists(dirPath)) Directory.CreateDirectory(dirPath);

            foreach (string file in Directory.EnumerateFiles(dirPath, "*.z0p"))
            {
                File.Delete(file);
            }
        }

        void ribbonOrbRecentItem_Click(object sender, EventArgs e)
        {
            string filePath = (sender as RibbonOrbRecentItem).Tag as string;
            if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath)) return;
            ZPlannerManager.OpenRecentFile(filePath);
            NotifyMenuButtonClicked(sender, e);
        }

        private void newProjectRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                ZPlannerManager.CreateNewProject();
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
                ZPlannerManager.UpdateActiveStackup();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void openProjectRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveProjectRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveProjectAsRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveProjectAs();
            NotifyMenuButtonClicked(sender, e);
        }

        private void closeProjectRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.CloseProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void recentFilesRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This menu item has not yet been implemented.");
            NotifyMenuButtonClicked(sender, e);
        }

        private void openStackupRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void fieldSolverRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowFXSandbox();
            NotifyMenuButtonClicked(sender, e);
        }

        private void newStackupRibbonButton_Click(object sender, EventArgs e)
        {
            //ZPlannerManager.CreateNewStackup(ZPlannerManager.Project);
            bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
            try
            {
                ZPlannerManager.CreateNewProject();
            }
            finally
            {
                ZPlannerManager.ResumeUpdateActiveStackupEvent();
                ZPlannerManager.UpdateActiveStackup();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void stackupWizardRibbonButton_Click(object sender, EventArgs e)
        {
            StackupWizardDialog wiz = StackupWizardDialog.CreateStackupWizardDialog();
            if (wiz != null) wiz.ShowDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void excelExportRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ExportExcel();
            NotifyMenuButtonClicked(sender, e);
        }

        private void pageSetupRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ShowPageSetup();
            NotifyMenuButtonClicked(sender, e);
        }

        private void printPreviewRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ShowPrintPreview();
            NotifyMenuButtonClicked(sender, e);
        }

        private void printRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ShowPrintStackup();
            NotifyMenuButtonClicked(sender, e);
        }

        private void emailStackupRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveStackupAndEmailIt();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveAsRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveProjectAs();
            NotifyMenuButtonClicked(sender, e);
        }

        private void preferencesRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenSetupDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void projectViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDefaultProjectView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void messageViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowMessagePanel();
            NotifyMenuButtonClicked(sender, e);
        }

        private void startpageViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDefaultStartPageView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void propertiesViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDefaultStackupPropertiesView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void showViasRibbonButton_Click(object sender, EventArgs e)
        {
            /*ZPlannerManager.ShowVias(showViasRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);*/
        }

        private void showSinglesRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowSingles(showSinglesRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void showPairsRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowPairs(showPairsRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void libViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDefaultStackupDmlView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void showLibRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDMLPanel();
            NotifyMenuButtonClicked(sender, e);
        }

        private void openViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenCustomLayout();
            SelectView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveCustomLayout();
            NotifyMenuButtonClicked(sender, e);
        }

        private void gridHeadersRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowHeaders(gridHeadersRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void gridHeadersRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            gridHeadersRibbonButton.SmallImage = (gridHeadersRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("gridHeadersRibbonButton.SmallImage")));
        }

        private void fieldSolverUtilRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowFXSandbox();
            NotifyMenuButtonClicked(sender, e);
        }

        private void diffStackupsRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.ProjectIsEmpty)
            {
                ZPlannerManager.DiffPanel.CompareWithStackup(null, null);
            }
            else
            {
                ZPlannerManager.DiffPanel.CompareWithStackup(ZPlannerManager.Project, ZPlannerManager.ProjectFile);
            }

            ZPlannerManager.ShowDiffPanel();
            NotifyMenuButtonClicked(sender, e);
        }

        private void helpRibbonButton_Click(object sender, EventArgs e)
        {
            //--MessageBox.Show("This menu item has not yet been implemented.");
            NotifyMenuButtonClicked(sender, e);
        }

        private void undoRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Commands.Undo();
            NotifyMenuButtonClicked(sender, e);
        }

        private void redoRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Commands.Redo();
            NotifyMenuButtonClicked(sender, e);
        }

        private void columnValuesRibbonButton_Click(object sender, EventArgs e)
        {
            using (ChangeColumnValueDialog dialog = new ChangeColumnValueDialog())
            {
                dialog.StartPosition = FormStartPosition.Manual;
                dialog.Location = new Point(columnValuesRibbonButton.Bounds.Left, columnValuesRibbonButton.Bounds.Bottom + 40);
            
                if (dialog.ShowDialog() == DialogResult.OK)
                { }
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void newZDiffRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;
            if (stackup != null) ZPlannerManager.ProjectPanel.SelectPair(stackup.AddPair());
            NotifyMenuButtonClicked(sender, e);
        }

        private void newZoRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;
            if (stackup != null) ZPlannerManager.ProjectPanel.SelectSingle(stackup.AddSingle());
            NotifyMenuButtonClicked(sender, e);
        }

        private void drcCheckRibbonButton_Click(object sender, EventArgs e)
        {
            bool bOn = (sender as RibbonButton).Checked;
            if (bOn)
            {
                ZPlannerManager.ShowDRCPanel();
                ZPlannerManager.DRCPanel.UpdateReport();
            }
            else
            {
                ZPlannerManager.DRCPanel.Hide();
            }

            NotifyMenuButtonClicked(sender, e);
        }

        private void coreLockingRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.IsCoreLocked = coreLockingRibbonButton.Checked;
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void etchAdjustRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.IsTrapezoidalTraces = etchAdjustRibbonButton.Checked;
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void pressedThicknessRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.IsPressedThickness = pressedThicknessRibbonButton.Checked;
                ZPlannerManager.CalculatePressedThickness(ZPlannerManager.IsPressedThickness);
                ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void importHyperLynxSTKRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ImportHL();
            NotifyMenuButtonClicked(sender, e);
        }

        private void importIRibbonButtonPC2581_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ImportIPC2581();
            NotifyMenuButtonClicked(sender, e);
        }

        private void importODBRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ImportODB();
            NotifyMenuButtonClicked(sender, e);
        }

        private void importTapestryRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ImportTapestry();
            NotifyMenuButtonClicked(sender, e);
        }

        private void exportHyperLynxFFSSTKRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ExportHL();
            NotifyMenuButtonClicked(sender, e);
        }

        private void exportIPC2581RibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ExportIPC2581();
            NotifyMenuButtonClicked(sender, e);
        }

        private void exportODBRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ExportODB();
            NotifyMenuButtonClicked(sender, e);
        }

        private void materialTypeRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLPanel.SetMaterialTypeFilter(coresRibbonButton.Checked, prepregsRibbonButton.Checked, solderMaskRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void coresRibbonButton_Click(object sender, EventArgs e)
        {
            solderMaskRibbonButton.Checked = !(coresRibbonButton.Checked || prepregsRibbonButton.Checked);
            if (solderMaskRibbonButton.Checked)
            {
                ZPlannerManager.DMLPanel.SetGlassStyleFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.SetSlashSheetFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.Set2PlyMaterialsFilter(false);
            }
            ZPlannerManager.DMLPanel.SetMaterialTypeFilter(coresRibbonButton.Checked, prepregsRibbonButton.Checked, solderMaskRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void prepregsRibbonButton_Click(object sender, EventArgs e)
        {
            solderMaskRibbonButton.Checked = !(coresRibbonButton.Checked || prepregsRibbonButton.Checked);
            ZPlannerManager.DMLPanel.SetMaterialTypeFilter(coresRibbonButton.Checked, prepregsRibbonButton.Checked, solderMaskRibbonButton.Checked);
            if (solderMaskRibbonButton.Checked)
            {
                ZPlannerManager.DMLPanel.SetGlassStyleFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.SetSlashSheetFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.Set2PlyMaterialsFilter(false);
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void solderMaskRibbonButton_Click(object sender, EventArgs e)
        {
            if (solderMaskRibbonButton.Checked)
            {
                ZPlannerManager.DMLPanel.SetGlassStyleFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.SetSlashSheetFilter(ZStringConstants.ClearFilter, true);
                ZPlannerManager.DMLPanel.Set2PlyMaterialsFilter(false);
            }
            coresRibbonButton.Checked = !solderMaskRibbonButton.Checked;
            prepregsRibbonButton.Checked = !solderMaskRibbonButton.Checked;
            ZPlannerManager.DMLPanel.SetMaterialTypeFilter(coresRibbonButton.Checked, prepregsRibbonButton.Checked, solderMaskRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void qualifiedMaterialsRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLPanel.SetQualifiedMaterialsFilter(qualifiedMaterialsRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void halogenFreeRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLPanel.SetHalogenFreeFilter(halogenFreeRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void plyMaterialsRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLPanel.Set2PlyMaterialsFilter(plyMaterialsRibbonButton.Checked);
            if (plyMaterialsRibbonButton.Checked && solderMaskRibbonButton.Checked)
            {
                ZPlannerManager.DMLPanel.SetMaterialTypeFilter(true, true, false);
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void clearFiltersRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLPanel.ClearFilter();
            NotifyMenuButtonClicked(sender, e);
        }

        private void customSortRibbonButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This menu item has not yet been implemented.");
            NotifyMenuButtonClicked(sender, e);
        }

        private void bulkEditRibbonButton_Click(object sender, EventArgs e)
        {
            using (BulkEditDialog dialog = new BulkEditDialog())
            {
                dialog.StartPosition = FormStartPosition.Manual;
                dialog.Location = new Point(bulkEditRibbonButton.Bounds.Left, bulkEditRibbonButton.Bounds.Bottom + 40);

                if (dialog.ShowDialog() == DialogResult.OK)
                { }
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void importCSVRibbonButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This menu item has not yet been implemented.");
            NotifyMenuButtonClicked(sender, e);
        }

        private void lockLibraryRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.IsLibraryLocked = lockLibraryRibbonButton.Checked;
            NotifyMenuButtonClicked(sender, e);
        }

        private void syncZ0LibRibbonButton_Click(object sender, EventArgs e)
        {
            Options.TheOptions.allowZZeroSynchronization = true;
            if (ZPlannerManager.SyncronizeDML(false)) ZPlannerManager.ReloadDML();
            NotifyMenuButtonClicked(sender, e);
        }

        private void libraryRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.CategoryFilter = (((z0LibraryRibbonButton.Checked) ? "'Z-zero', " : "") + ((corporateLibraryRibbonButton.Checked) ? "'Corporate', " : "") + ((localLibraryRibbonButton.Checked) ? "'Local', " : "")).Trim(new char[] { ' ', ',' });
            ZPlannerManager.DMLPanel.SetLibraryCategoryFilter(z0LibraryRibbonButton.Checked, corporateLibraryRibbonButton.Checked, localLibraryRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void findRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.DMLSearchDialog.ShowDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void dmlStackupViewRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDefaultStackupDmlView();
            NotifyMenuButtonClicked(sender, e);
        }

        private void deleteRowRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.DeleteSelectedRows();
            NotifyMenuButtonClicked(sender, e);
        }

        private void blankRowRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            string type = e.Item.Tag as string;
            ZLayerType layerType = ZLayer.LayerTypeDictionary[type];
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.AddBlankRows(layerType);
            NotifyMenuButtonClicked(sender, e);
        }

        private void cutRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Cut();
            NotifyMenuButtonClicked(sender, e);
        }

        private void copyRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Copy();
            NotifyMenuButtonClicked(sender, e);
        }

        private void pasteRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Paste();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveCorpLibRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Corporate)) return;
            ZPlannerManager.SaveDml(ZLibraryCategory.Corporate);
            NotifyMenuButtonClicked(sender, e);
        }

        private void uploadCorpLibRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Corporate)) return;
            // TBD
            NotifyMenuButtonClicked(sender, e);
        }

        private void hideZoLibRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.Corporate)) return;
            // TBD
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveZoLibRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.ZZero)) return;
            ZPlannerManager.SaveDml(ZLibraryCategory.ZZero);
            NotifyMenuButtonClicked(sender, e);
        }

        private void encryptSaveRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.ZZero)) return;
            // TBD
            NotifyMenuButtonClicked(sender, e);
        }

        private void uploadToWebRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.IsUserHaveAccessToLibrary(ZLibraryCategory.ZZero)) return;
            // TBD
            NotifyMenuButtonClicked(sender, e);
        }

        private void genericStackupsRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            ZPlannerManager.NewFromGenericStackup(e.Item.Tag as string);
            NotifyMenuButtonClicked(sender, e);
        }

        private void programRibbonMenu_ActiveTabChanged(object sender, EventArgs e)
        {
            ZPlannerManager.ActiveMenuTab = programRibbonMenu.ActiveTab;
        }

        private void qaOpenStackupRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaSaveStackupRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveProject();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaSaveAndEmailRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveStackupAndEmailIt();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaUndoRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Commands.Undo();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaRedoRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.Commands.Redo();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaCutRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.IsLayerSelected && ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Cut();
            else if (ZPlannerManager.IsMaterialSelected && ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.Cut();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaCopyRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.IsLayerSelected && ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Copy();
            else if (ZPlannerManager.IsMaterialSelected && ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.Copy();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaPasteRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.IsLayerSelected && ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.Paste();
            else if (ZPlannerManager.IsMaterialSelected && ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.Paste();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaPrintPreviewRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.ShowPrintPreview();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaHelpRibbonButton_Click(object sender, EventArgs e)
        {
            MessageBox.Show("This menu item has not yet been implemented.");
            NotifyMenuButtonClicked(sender, e);
        }        

        public void SetSendToStackupButtonEnabled(bool isEnable)
        {
            sendToStackupRibbonButton.Enabled = isEnable;
            if (isEnable) UpdateSendToStackup();
        }

        public void SetTypeFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            this.prepregsRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.LayerTypePrepreg);
            this.coresRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.LayerTypeCore);
            this.solderMaskRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.LayerTypeSolderMask);
        }

        public void SetCategoryFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            this.z0LibraryRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.MaterialCategoryZZero);
            this.corporateLibraryRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.MaterialCategoryCorporate);
            this.localLibraryRibbonButton.Checked = clearFilter || listOfValues.Contains(ZStringConstants.MaterialCategoryLocal);
        }

        public void SetQualifiedFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            this.qualifiedMaterialsRibbonButton.Checked = !clearFilter && !listOfValues.Contains(false);
        }

        public void SetHalogenFreeFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            this.halogenFreeRibbonButton.Checked = !clearFilter && !listOfValues.Contains(false);
        }

        public void Set2PlyMaterialsFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            bool notFiltered = true;
            foreach(var value in listOfValues)
            {
                if (!value.ToString().Contains("x") && !value.ToString().Contains("/")) {notFiltered = true; break;}
                else { notFiltered = false; }
            }

            this.plyMaterialsRibbonButton.Checked = !clearFilter && !notFiltered;
        }

        public void SetGlassStyleFilterButtons(string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            this.glassStylesRibbonButton.Checked = !clearFilter;
            foreach (var item in this.glassStylesRibbonButton.DropDownItems)
            {
                item.Checked = (item.Value != null) && filterString.Contains(item.Value);
            }
        }

        public void SetSlashSheetFilterButtons(IEnumerable<object> listOfValues, string filterString)
        {
            bool clearFilter = string.IsNullOrWhiteSpace(filterString);
            List<object> list = new List<object>(listOfValues);
            this.slashSheetsRibbonButton.Checked = !clearFilter;
            foreach(var item in this.slashSheetsRibbonButton.DropDownItems)
            {
                item.Checked = (item.Value != null) && list.Find(x => x.ToString().Contains(item.Value)) != null;
            }
        }

        private void openLocalLibraryRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenLocalMenu();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveLocalLibraryRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveLocalMenu();
            NotifyMenuButtonClicked(sender, e);
        }

        private void saveAsLocalLibraryRibbonOrbMenuItem_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SaveAsLocalMenu();
            NotifyMenuButtonClicked(sender, e);
        }

        private void filterCreateRibbonButton_Click(object sender, EventArgs e)
        {
            List<ZParameter> parameters = ZPlannerManager.Dml.Parameters.FindAll(x => x.IsPrivate == false);// && x.IsHidden == false);
            
            CustomFilterDialog dlg = new CustomFilterDialog(parameters);
            dlg.SetFilterAndSort(string.Empty, string.Empty, ZPlannerManager.DMLPanel.GetCurrentFilter(), ZPlannerManager.DMLPanel.GetCurrentSort());
            dlg.ShowDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void filterToolbarRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            if (e.Item.Value == "CreateNewFilter") ZPlannerManager.CreateCustomFilter();
            if (e.Item.Value == "RenameFilter") ZPlannerManager.RenameCustomFilter(e.Item.Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterToolbarRibbonButton.Bounds.Bottom + 40));
            if (e.Item.Value == "EditFilter") ZPlannerManager.EditCustomFilter(e.Item.Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterToolbarRibbonButton.Bounds.Bottom + 40));
            if (e.Item.Value == "UpdateFilter") ZPlannerManager.UpdateCustomFilter(e.Item.Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterToolbarRibbonButton.Bounds.Bottom + 40));
            if (e.Item.Value == "DeleteFilter") ZPlannerManager.DeleteCustomFilter(e.Item.Tag as string, new Point(filterRibbonButtonList.Bounds.X, filterToolbarRibbonButton.Bounds.Bottom + 40));
            if (e.Item.Value == "ApplyFilter") ZPlannerManager.ApplyCustomFilter(e.Item.Tag as string);
            NotifyMenuButtonClicked(sender, e);
        }

        private void glassStylesRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            string value = e.Item.Value;
            ZPlannerManager.DMLPanel.SetGlassStyleFilter(value, value == ZStringConstants.ClearFilter);
            NotifyMenuButtonClicked(sender, e);
        }

        internal void UpdateGlassStyles()
        {
            this.programRibbonMenu.SuspendUpdating();
            glassStylesRibbonButton.DropDownItems.Clear();

            RibbonButton clearItem = new RibbonButton(ZStringConstants.ClearFilter);
            clearItem.ToolTip = ZStringConstants.ClearFilter;
            clearItem.Value = ZStringConstants.ClearFilter;
            glassStylesRibbonButton.DropDownItems.Add(clearItem);

            foreach (string value in ZPlannerManager.DMLPanel.GetListOfGlassStyles().Distinct().OrderBy(x => int.Parse(x)))
            {
                RibbonButton item = new RibbonButton(value);
                item.ToolTip = value;
                item.Value = value;
                glassStylesRibbonButton.DropDownItems.Add(item);
            }

            this.programRibbonMenu.ResumeUpdating(true);
        }

        private void slashSheetsRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            string value = e.Item.Value;
            ZPlannerManager.DMLPanel.SetSlashSheetFilter(value, value == ZStringConstants.ClearFilter);
            NotifyMenuButtonClicked(sender, e);
        }

        internal void UpdateSlashSheet()
        {
            this.programRibbonMenu.SuspendUpdating();
            slashSheetsRibbonButton.DropDownItems.Clear();

            RibbonButton clearItem = new RibbonButton(ZStringConstants.ClearFilter);
            clearItem.ToolTip = ZStringConstants.ClearFilter;
            clearItem.Value = ZStringConstants.ClearFilter;
            slashSheetsRibbonButton.DropDownItems.Add(clearItem);

            SelectList list = ZPlannerManager.Dml.Lists.Find(x => x.ID == ZStringConstants.SelectListIDSlashSheet);
            if (list == null) return;

            List<string> values = new List<string>();

            List<string> visibleValues = ZPlannerManager.DMLPanel.GetListOfSlashSheet(); 

            Dictionary<string, string> dict = list.GetDictionary();
            foreach (string value in dict.Keys)
            {
                if (!visibleValues.Contains(value)) continue;
                
                RibbonButton item = new RibbonButton(dict[value]);
                item.ToolTip = dict[value];
                item.Value = value;
                slashSheetsRibbonButton.DropDownItems.Add(item);
            }

            slashSheetsRibbonButton.RedrawItem();
            this.programRibbonMenu.ResumeUpdating(true);
        }

        public void LockLibrary(bool isLocked)
        {
            this.openLocalLibraryRibbonOrbMenuItem.Enabled = !isLocked;
            this.bulkEditRibbonButton.Enabled = !isLocked;
        }

        private void lossPlanningRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SetLossPlanning(lossPlanningRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void UpdateSendToStackup()
        {
            sendToStackupRibbonButton.DropDownItems.Clear();
            ZMaterial material = ZPlannerManager.DMLPanel.GetSelectedMaterial();

            // Create New Row
            RibbonButton createButton = new RibbonButton("Apply selected material to a new stackup row.  ");
            createButton.ToolTip = "Apply selected material to a new stackup row.  ";
            createButton.Value = "AssignMaterialToNewLayer";
            createButton.Tag = null;
            sendToStackupRibbonButton.DropDownItems.Add(createButton);

            if (ZPlannerManager.ProjectIsEmpty || ZPlannerManager.Project.StackupIsEmpty || ZPlannerManager.StackupPanel == null || material == null)
            {
                SetSendToStackupButtonEnabled(false);
                return;
            }

            RibbonSeparator existedSeparator = new RibbonSeparator("");
            sendToStackupRibbonButton.DropDownItems.Add(existedSeparator);

            ZLayerType? type = material.GetMaterialType();
            List<ZLayer> allLayers = new List<ZLayer>();

            foreach(ZLayer layer in ZPlannerManager.Project.Stackup.Layers.FindAll(x=>x.GetLayerType() == type))
            {
                List<ZLayer> oneLayer = new List<ZLayer>();
                RibbonButton button = new RibbonButton("Apply selected material to Row " + (layer.Order + 1) + ".  ");
                button.ToolTip = "Apply selected material to Row " + (layer.Order + 1) + ".  ";
                button.Value = "AssignMaterialToExistingLayer";
                oneLayer.Add(layer);
                allLayers.Add(layer);
                button.Tag = oneLayer;
                sendToStackupRibbonButton.DropDownItems.Add(button);
            }

            RibbonButton allButton = new RibbonButton(string.Format("Apply selected material to all {0}s.  ", type.ToString()));
            allButton.ToolTip = string.Format("Apply selected material to all {0}s.  ", type.ToString());
            allButton.Value = "AssignMaterialToAllExistingLayers";
            allButton.Tag = allLayers;
            sendToStackupRibbonButton.DropDownItems.Add(allButton);
        }

        internal void UpdateDeleteMaterial()
        {
            deleteMaterialRibbonButton.Enabled = (ZPlannerManager.DMLPanel != null && ZPlannerManager.DMLPanel.GetCurrentMaterial() != null);
        }

        private void sendToStackupRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            ZPlannerManager.DMLPanel.SendToStackup(ZPlannerManager.DMLPanel.GetSelectedMaterial(), e.Item.Tag as List<ZLayer>);
            NotifyMenuButtonClicked(sender, e);
        }

        public void SelectView()
        {
            if (ZPlannerManager.IsIgnorePanelVisibleChanged) return;
            projectViewRibbonButton.Checked = false;
            propertiesViewRibbonButton.Checked = false;
            dmlStackupViewRibbonButton.Checked = false;
            startpageViewRibbonButton.Checked = false;
            drcCheckRibbonButton.Checked = false;

            //if (showViasRibbonButton.Checked || drcCheckRibbonButton.Checked || lossPlanningRibbonButton.Checked) return;

            if (ZPlannerManager.StackupPanel != null)
            {

                if (!ZPlannerManager.StackupPanel.IsHidden && !ZPlannerManager.DRCPanel.IsHidden) drcCheckRibbonButton.Checked = true;
                if (!ZPlannerManager.StackupPanel.IsHidden && !ZPlannerManager.ProjectPanel.IsHidden && !ZPlannerManager.PropertiesPanel.IsHidden && !ZPlannerManager.DMLPanel.IsHidden && !ZPlannerManager.MessagePanel.IsHidden) 
                { projectViewRibbonButton.Checked = true; return; }
                if (!ZPlannerManager.StackupPanel.IsHidden && !ZPlannerManager.PropertiesPanel.IsHidden && !ZPlannerManager.DMLPanel.IsHidden) 
                { dmlStackupViewRibbonButton.Checked = true; return; }
                if (!ZPlannerManager.StackupPanel.IsHidden && !ZPlannerManager.PropertiesPanel.IsHidden) 
                { propertiesViewRibbonButton.Checked = true; return; }
            }

            if (!ZPlannerManager.StartPanel.IsHidden)
            { startpageViewRibbonButton.Checked = true; return; }
        }

        private void roughnessRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                RoughnessDialog dlg = new RoughnessDialog();
                dlg.StartPosition = FormStartPosition.Manual;
                dlg.Location = new Point(roughnessRibbonButton.Bounds.Right - dlg.Width, roughnessRibbonButton.Bounds.Bottom + 40);
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    //roughnessRibbonButton.Checked = dlg.isShowRoughness;
                    copperRoughnessShowRibbonButton.Checked = dlg.isShowRoughness;
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        ZPlannerManager.IsRoughness = dlg.isShowRoughness;
                        ZPlannerManager.ShowSingles(ZPlannerManager.IsSinglesShouldBeShown());  
                        ZPlannerManager.StackupPanel.ShowRoughnessColumns(ZPlannerManager.IsRoughness);
                        ZPlannerManager.UpdateActiveStackup();
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }
            else
            {
                UpdateMenu();
            }
            
        }

        private void copperRoughnessShowRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SetCopperRoughness(copperRoughnessShowRibbonButton.Checked);
        }

        private void copperRoughnessShowRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            copperRoughnessShowRibbonButton.SmallImage = (copperRoughnessShowRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("copperRoughnessShowRibbonButton.SmallImage")));
            roughnessRibbonButton.Checked = copperRoughnessShowRibbonButton.Checked;
        }

        private void pressedThicknessIncludeRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            pressedThicknessIncludeRibbonButton.SmallImage = (pressedThicknessIncludeRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("pressedThicknessIncludeRibbonButton.SmallImage")));
            pressedThicknessRibbonButton.Checked = pressedThicknessIncludeRibbonButton.Checked;
        }

        private void pressedThicknessIncludeRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SetPressedThickness(pressedThicknessIncludeRibbonButton.Checked);
        }

        private void sequentialLaminationIncludeRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            sequentialLaminationIncludeRibbonButton.SmallImage = (sequentialLaminationIncludeRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("sequentialLaminationIncludeRibbonButton.SmallImage")));
        }

        private void sequentialLaminationIncludeRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.SetSequentialLamination(sequentialLaminationIncludeRibbonButton.Checked);
            if (ZPlannerManager.StackupPanel != null && ZPlannerManager.StackupPanel.Stackup != null) ZPlannerManager.StackupPanel.Stackup.IsSequentialLaminationSetByUser = true; 
        }

        private void pressedThicknessSettingsRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                PressedThicknessDialog dlg = new PressedThicknessDialog();
                dlg.StartPosition = FormStartPosition.Manual;
                dlg.Location = new Point(pressedThicknessRibbonButton.Bounds.Left, pressedThicknessRibbonButton.Bounds.Bottom + 40);
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    pressedThicknessIncludeRibbonButton.Checked = dlg.isShowPressedThickness;
                    sequentialLaminationIncludeRibbonButton.Checked = dlg.isShowSequentialLamination;

                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                    try
                    {
                        ZPlannerManager.IsPressedThickness = dlg.isShowPressedThickness;
                        ZPlannerManager.IsSequentialLamination = dlg.isShowSequentialLamination;
                        ZPlannerManager.CalculatePressedThickness(dlg.isShowPressedThickness);
                        ZPlannerManager.StackupPanel.RecalculateViaSpanAspectRatio();
                        stackup.IsSequentialLaminationSetByUser = true; 
                    }
                    finally
                    {
                        ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                        ZPlannerManager.UpdateActiveStackup();
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void etchEffectsIncludeRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            etchEffectsIncludeRibbonButton.SmallImage = (etchEffectsIncludeRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("etchEffectsIncludeRibbonButton.SmallImage")));
            etchAdjustRibbonButton.Checked = etchEffectsIncludeRibbonButton.Checked;
        }

        private void etchEffectsIncludeRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.SetTrapezoidalTraces(etchAdjustRibbonButton.Checked);
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void etchEffectsSettingsRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                EtchAdjustDialog dlg = new EtchAdjustDialog();
                dlg.StartPosition = FormStartPosition.Manual;
                dlg.Location = new Point(etchAdjustRibbonButton.Bounds.Left, etchAdjustRibbonButton.Bounds.Bottom + 40);
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    etchEffectsIncludeRibbonButton.Checked = dlg.isEnableEtchEffects;
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    bool isIgnoreActive = ZPlannerManager.SuspendUpdateActiveStackupEvent();
                    try
                    {
                        ZPlannerManager.IsTrapezoidalTraces = dlg.isEnableEtchEffects;
                        foreach (ZLayer layer in stackup.Layers)
                        {
                            layer.SetLayerParameterValue(ZStringConstants.ParameterIDEtchFactor, stackup.Etch.ToString("N" + Options.TheOptions.lengthDigits, CultureInfo.InvariantCulture));
                        }
                        ZPlannerManager.RecalculateThickness();
                    }
                    finally
                    {
                        ZPlannerManager.ResumeUpdateActiveStackupEvent(isIgnoreActive);
                        ZPlannerManager.UpdateActiveStackup();
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void disabledCellRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowDisabledCells(disabledCellRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void disabledCellRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            disabledCellRibbonButton.SmallImage = (disabledCellRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("disabledCellRibbonButton.SmallImage")));
        }

        private void glassStylesRibbonButton_DropDownBeforeShowing(object sender, EventArgs e)
        {
            UpdateGlassStyles();
        }

        private void slashSheetsRibbonButton_DropDownBeforeShowing(object sender, EventArgs e)
        {
            ZPlannerManager.MainMenu.UpdateSlashSheet();
        }

        private void mirrorRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;
            if (stackup != null)
            {
                if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel.EndGridEdit();
                if (stackup.IsMirrored())
                {
                    MessageBox.Show("Mirroring is generally performed when you’ve designed the top half of a stackup, and want to mirror it to create the bottom half.  This stackup is already symmetrical.");
                }
                else if (stackup.MirrorStackup(true))
                {
                    if (ZPlannerManager.ConfirmYesNo("Auto-Mirroring", "Would you like to turn on Auto-Mirroring for future stackup edits?  (Forces top > bottom symmetry, and turns on the stackup center line.)"))
                    {
                        autoMirrorStackupRibbonButton.Checked = true;
                        autoMirrorRibbonButton.Checked = true;
                        centerLineRibbonButton.Checked = true;
                    }
                }
            }
            NotifyMenuButtonClicked(sender, e);
        }

        private void autoMirrorRibbonButton_Click(object sender, EventArgs e)
        {
            /*ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                ZPlannerManager.StackupPanel.IsAutoMirror = autoMirrorRibbonButton.Checked;
            }
            else
            {
                UpdateMenu();
            }
            NotifyMenuButtonClicked(sender, e);*/
        }

        private void centerLineRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowCenterLine(centerLineRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void centerLineRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            centerLineRibbonButton.SmallImage = (centerLineRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("centerLineRibbonButton.SmallImage")));
        }

        private void autoMirrorStackupRibbonButton_Click(object sender, EventArgs e)
        {
            if (!ZPlannerManager.Project.StackupIsEmpty && ZPlannerManager.StackupPanel != null)
            {
                autoMirrorStackupRibbonButton.Checked = ZPlannerManager.TryAutoMirror(autoMirrorStackupRibbonButton.Checked);
            }
            else
            {
                UpdateMenu();
            }

            NotifyMenuButtonClicked(sender, e);
        }

        private void autoMirrorStackupRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            autoMirrorStackupRibbonButton.SmallImage = (autoMirrorStackupRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("autoMirrorStackupRibbonButton.SmallImage")));
            autoMirrorRibbonButton.Checked = autoMirrorStackupRibbonButton.Checked;
        }

        private void autoMirrorSettingsRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                AutoMirrorDialog dlg = new AutoMirrorDialog();
                dlg.StartPosition = FormStartPosition.Manual;
                dlg.Location = new Point(autoMirrorRibbonButton.Bounds.Left, autoMirrorRibbonButton.Bounds.Bottom + 40);
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    autoMirrorStackupRibbonButton.Checked = dlg.isAutoMirror;
                    autoMirrorRibbonButton.Checked = dlg.isAutoMirror;
                    //ZPlannerManager.StackupPanel.IsAutoMirror = dlg.isAutoMirror;

                    centerLineRibbonButton.Checked = dlg.isCenterLine;
                    //ZPlannerManager.IsCenterLineVisible = dlg.isCenterLine;
                }
            }
            else
            {
                UpdateMenu();
            }
        }

        private void showViaSpansRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowVias(showViaSpansRibbonButton.Checked);
            NotifyMenuButtonClicked(sender, e);
        }

        private void showViaSpansRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            showViaSpansRibbonButton.SmallImage = (showViaSpansRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("showViaSpansRibbonButton.SmallImage")));
            showViasRibbonButton.Checked = showViaSpansRibbonButton.Checked;
        }

        private void editViaSpansRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowViaSpanPanel();

            if (showViaSpansRibbonButton.Checked != ZPlannerManager.IsViasVisible)
            {
                showViaSpansRibbonButton.Checked = ZPlannerManager.IsViasVisible; 
            }
        }

        public void programRibbonMenu_KeyDown(object sender, KeyEventArgs e)
        {
            string shortcut = string.Empty;

            if (e.Control) shortcut += "Ctrl+";
            if (e.Shift) shortcut += "Shift+";
            if (e.Alt) shortcut += "Alt+";

            KeysConverter kc = new KeysConverter();
            string keyChar = kc.ConvertToString(e.KeyCode);

            shortcut += keyChar.ToUpper();

            if (ShortcutDictionary.Keys.Contains(shortcut))
            {
                RibbonButton button = ShortcutDictionary[shortcut] as RibbonButton;
                if (button != null) button.PerformClick();
            }
        }

        void programRibbonMenuItem_ShortcutChanged(object sender, RibbonItemShortcutEventArgs e)
        {
            RibbonItem item = sender as RibbonItem;
            if (item == null || string.IsNullOrWhiteSpace(item.Shortcut)) return;

            if (!ShortcutDictionary.Keys.Contains(item.Shortcut)) ShortcutDictionary.Add(item.Shortcut, item);
        }

        public void programRibbonMenu_ToolTipPopUp(object sender, RibbonElementPopupEventArgs e)
        {
            RibbonButton button = e.AssociatedRibbonElement as RibbonButton;
            if (button != null)
            {
                string shortcut = GetShortcutByValue(button.Value);
                if (!string.IsNullOrWhiteSpace(shortcut)) button.ToolTip = button.Text + " (" + shortcut + ")";
            }
        }

        private string GetShortcutByValue(string value)
        {
            foreach(string shortcut in ShortcutDictionary.Keys)
            {
                if (ShortcutDictionary[shortcut].Value == value) return shortcut;
            }

            return null;
        }

        private void aboutRibbonButton_Click(object sender, EventArgs e)
        {
            AboutDialog dlg = new AboutDialog();
            dlg.ShowDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void materialMapperRibbonButton_Click(object sender, EventArgs e)
        {
            MaterialMapperRunner runner = new MaterialMapperRunner();
            runner.Run();
        }

        private void tutorialRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenTutorial();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaTutorialRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenTutorial();
            NotifyMenuButtonClicked(sender, e);
        }

        private void qaSettingsRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenSetupDialog();
            NotifyMenuButtonClicked(sender, e);
        }

        private void deleteMaterialRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.DeleteSelectedMaterial();
            NotifyMenuButtonClicked(sender, e);
        }

        private void addMaterialRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            string type = e.Item.Tag as string;
            ZLayerType layerType = ZLayer.LayerTypeDictionary[type];
            if (ZPlannerManager.DMLPanel != null) ZPlannerManager.DMLPanel.AddBlankMaterial(layerType);
            NotifyMenuButtonClicked(sender, e);
        }

        private void glassPitchRibbonButton_Click(object sender, EventArgs e)
        {
            ZStackup stackup = ZPlannerManager.Project.Stackup;

            if (stackup != null)
            {
                GlassPitchDialog dlg = new GlassPitchDialog();
                dlg.StartPosition = FormStartPosition.Manual;
                dlg.Location = new Point(glassPitchRibbonButton.Bounds.Right - dlg.Width, glassPitchRibbonButton.Bounds.Bottom + 40);
                dlg.InitValues(ZPlannerManager.IsGlassPitch, ZPlannerManager.IsGws);
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
                    try
                    {
                        ZPlannerManager.ShowSingles(ZPlannerManager.IsSinglesShouldBeShown());
                    }
                    finally
                    {
                        ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
                    }
                }
            }
            else
            {
                UpdateMenu();
            }
        }

        /*
        private void glassPitchRibbonButton_Click(object sender, EventArgs e)
        {
            if (ZPlannerManager.StackupPanel != null)
            {
                ZPlannerManager.ShowSingles(IsSinglesShouldBeShown()); 
                ZPlannerManager.StackupPanel.ShowGlassPitch(glassPitchRibbonButton.Checked);
            }
            NotifyMenuButtonClicked(sender, e);
        }
         */

        private void gwsRibbonButton_Click(object sender, EventArgs e)
        {
            bool isChecked = gwsRibbonButton.Checked;
            ZPlannerManager.ShowSingles(ZPlannerManager.IsSinglesShouldBeShown()); 
            ZPlannerManager.StackupPanel.ShowGWSColumns(isChecked);
            dataRateShowRibbonButton.Checked = isChecked;
            unitIntervalShowRibbonButton.Checked = isChecked;
            skewToleranceShowRibbonButton.Checked = isChecked;
        }

        private void dataRateShowRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            dataRateShowRibbonButton.SmallImage = (dataRateShowRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("dataRateShowRibbonButton.SmallImage")));
            gwsRibbonButton.Checked = (dataRateShowRibbonButton.Checked || unitIntervalShowRibbonButton.Checked || skewToleranceShowRibbonButton.Checked);
        }

        private void dataRateShowRibbonButton_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                ZPlannerManager.StackupPanel.ShowGWSColumns(dataRateShowRibbonButton.Checked, ZStringConstants.ParameterIDZdiff_BitRate);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        private void unitIntervalShowRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            unitIntervalShowRibbonButton.SmallImage = (unitIntervalShowRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("unitIntervalShowRibbonButton.SmallImage")));
            gwsRibbonButton.Checked = (dataRateShowRibbonButton.Checked || unitIntervalShowRibbonButton.Checked || skewToleranceShowRibbonButton.Checked);
        }

        private void unitIntervalShowRibbonButton_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                ZPlannerManager.StackupPanel.ShowGWSColumns(unitIntervalShowRibbonButton.Checked, ZStringConstants.ParameterIDZdiff_UnitInterval);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        private void skewToleranceShowRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            skewToleranceShowRibbonButton.SmallImage = (skewToleranceShowRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("skewToleranceShowRibbonButton.SmallImage")));
            gwsRibbonButton.Checked = (dataRateShowRibbonButton.Checked || unitIntervalShowRibbonButton.Checked || skewToleranceShowRibbonButton.Checked);
        }

        private void skewToleranceShowRibbonButton_Click(object sender, EventArgs e)
        {
            bool isIgnore = ZPlannerManager.Commands.SuspendCommandEvent();
            try
            {
                ZPlannerManager.StackupPanel.ShowGWSColumns(skewToleranceShowRibbonButton.Checked, ZStringConstants.ParameterIDZdiff_SkewTolerance);
            }
            finally
            {
                ZPlannerManager.Commands.ResumeCommandEvent(isIgnore);
            }
        }

        private void glassPitchShowRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowGlassPitch(glassPitchShowRibbonButton.Checked, ZPlannerManager.IsGws);
        }

        private void glassPitchShowRibbonButton_Check(object sender, EventArgs e)
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ProgramMenu));
            glassPitchShowRibbonButton.SmallImage = (glassPitchShowRibbonButton.Checked) ? global::ZZero.ZPlanner.Properties.Resources.Checked_16 : ((System.Drawing.Image)(resources.GetObject("glassPitchShowRibbonButton.SmallImage")));
            //glassPitchRibbonButton.Checked = glassPitchShowRibbonButton.Checked;
        }
    }

    public class MenuButtonClickedEventArgs : EventArgs
    {
        public MenuButtonClickedEventArgs(string value)
        {
            Value = value;
        }

        public string Value { get; set; }
    }

    public class MenuButtonCheckedEventArgs : EventArgs
    {
        public MenuButtonCheckedEventArgs(string value, bool isChecked)
        {
            Value = value;
            Checked = isChecked;
        }

        public string Value { get; set; }

        public bool Checked { get; set; }
    }

    public delegate void MenuButtonClickedEventHandler(RibbonButton button, MenuButtonClickedEventArgs e);
    public delegate void MenuButtonCheckedEventHandler(RibbonButton button, MenuButtonCheckedEventArgs e);

}
