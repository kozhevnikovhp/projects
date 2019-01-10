using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Provider;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI.Recent;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerStartPanel : DockContent
    {
        public ZPlannerStartPanel()
        {
            InitializeComponent();
            /*if (ZPlannerManager.rights.AllowDmlUpdate) 
                this.startRibbonPanel.Items.Add(this.syncZoLibRibbonButton);*/
            this.startRibbonPanel.Items.Add(this.syncZoLibRibbonButton);
            if (!ZPlannerManager.rights.AllowDmlUpdate) 
            {
                this.syncZoLibRibbonButton.Enabled = false;
                this.syncZoLibRibbonButton.ToolTip = "Please contact support@z-zero.com, if you would like to purchase software support, which is required for library updates.";
            }

            FillRecentFilesList();
            UpdateGenericStackups();

            UpdateTutorial();

            RecentFilesManager.RecentFilesChanged += RecentFilesManager_RecentFilesChanged;

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;
        }

        private void UpdateTutorial()
        {
            tutorialPanel.FillData();
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

        void RecentFilesManager_RecentFilesChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "RecentFiles") FillRecentFilesList();
        }

        private void FillRecentFilesList()
        {
            this.recentRibbonPanel.Items.Clear();

            foreach (string file in RecentFilesManager.RecentFiles)
            {
                RibbonOrbRecentItem item = new RibbonOrbRecentItem(Path.GetFileName(file));
                item.ToolTipTitle = Path.GetFileName(file);
                item.ToolTip = Path.GetFullPath(file);
                item.Tag = Path.GetFullPath(file);
                item.MaxSizeMode = System.Windows.Forms.RibbonElementSizeMode.Medium;
                item.Click += recentRibbonItem_Click;
                this.recentRibbonPanel.Items.Add(item);
            }
        }

        private void recentRibbonItem_Click(object sender, EventArgs e)
        {
            string filePath = (sender as RibbonOrbRecentItem).Tag as string;
            if (string.IsNullOrEmpty(filePath) || !File.Exists(filePath)) return;
            ZPlannerManager.OpenRecentFile(filePath);
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void newStackupRibbonButton_Click(object sender, EventArgs e)
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

            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void stackupWizardRibbonButton_Click(object sender, EventArgs e)
        {
            StackupWizardDialog wiz = StackupWizardDialog.CreateStackupWizardDialog();
            if (wiz != null) wiz.ShowDialog();
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void genericStackupsRibbonButton_DropDownItemClicked(object sender, RibbonItemEventArgs e)
        {
            ZPlannerManager.NewFromGenericStackup(e.Item.Tag as string);
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void openStackupRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.OpenProject();
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void fieldSolverRibbonButton_Click(object sender, EventArgs e)
        {
            ZPlannerManager.ShowFXSandbox();
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void syncZoLibRibbonButton_Click(object sender, EventArgs e)
        {
            Options.TheOptions.allowZZeroSynchronization = true;
            if (ZPlannerManager.SyncronizeDML(false)) ZPlannerManager.ReloadDML();
            ZPlannerManager.MainMenu.NotifyMenuButtonClicked(sender, e);
        }

        private void programRibbonMenu_ToolTipPopUp(object sender, RibbonElementPopupEventArgs e)
        {
            ZPlannerManager.MainMenu.programRibbonMenu_ToolTipPopUp(sender, e);
        }
    }
}
