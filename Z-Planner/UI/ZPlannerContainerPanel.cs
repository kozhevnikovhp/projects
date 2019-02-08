using System;
using System.Collections.Generic;
using System.IO;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Properties;
using ZZero.ZPlanner.Data.Provider;
using ZZero.ZPlanner.UI.Recent;
using ZZero.ZPlanner.ZConfiguration;
using ZZero.ZPlanner.Translators;
using ZZero.ZPlanner.FXInterface;
using ZZero.ZPlanner.Settings;
using System.Drawing;
using ZZero.ZPlanner.UI.Menu;
using ZZero.ZPlanner.UI.Dialogs;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerContainerPanel : DockContent
    {
        public ZPlannerContainerPanel()
        {
            InitializeComponent();
            this.Text = "  Z-zero  " + AboutData.TheAbout.sVersion;
            this.KeyPreview = true;
            this.KeyDown += ZPlannerContainerPanel_KeyDown;
        }

        void ZPlannerContainerPanel_KeyDown(object sender, KeyEventArgs e)
        {
            ZPlannerManager.MainMenu.programRibbonMenu_KeyDown(sender, e);
        }

        internal DockPanel MainDockPanel { get { return this.mainDockPanel; } }

        internal ProgramMenu ProgramMenu { get { return programMenu; } }

        internal StatusMenu StatusMenu { get { return statusMenu; } }

        void ZPlannerManager_PropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "Project")
            {
                //ZPlannerManager.StackupPanel.LoadData(ZPlannerManager.Project.Stackup, null);
                ZPlannerManager.ProjectPanel.LoadData(ZPlannerManager.Project);
                return;
            }

            if (e.PropertyName == "ProjectIsNull")
            {
                ZPlannerManager.ShowDefaultStartPageView();
                return;
            }

            if (e.PropertyName == "ProjectFile")
            {
                ZPlannerManager.ProjectPanel.Text = "Project ";
                string projectName = Path.GetFileNameWithoutExtension(ZPlannerManager.ProjectFile);
                if (!string.IsNullOrEmpty(projectName)) ZPlannerManager.ProjectPanel.Text += "- " + projectName;
                ZPlannerManager.ProjectPanel.ToolTipText = ZPlannerManager.ProjectFile;

                this.Text = string.IsNullOrEmpty(projectName) ? "  Z-zero  " + AboutData.TheAbout.sVersion : "  " + projectName + " - " + "Z-zero  " + AboutData.TheAbout.sVersion;

                //ZPlannerManager.Project.Stackup.SetTitle(projectName);
                return;
            }
        }

        void ZPlannerManager_NeedToClear(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "ContainerPanel")
            {
                Clear();
            }
        }

        private void Clear()
        {
            ZPlannerManager.ProjectPanel.Clear();

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.StackupPanel = null;

            ZPlannerManager.PropertiesPanel.SelectedObject = null;
        }

        private void ZPlannerContainerPanel_Load(object sender, EventArgs e)
        {
            ZPlannerManager.InitDialog = new InitDialog("Z-planner");
            ZPlannerManager.InitDialog.Show();
            ZPlannerManager.AddInitDialogMessage("Starting Z-planner application ...");

            //Environment.SetEnvironmentVariable("ZZERO_LICENSE_FILE", @"c:\!ZZero\Repository\Trunk\Z-Zero\License\");
            //ZPlannerManager.Commands.IsIgnoreCommands = true;
            
            //read settings.xml on load
            ZPlannerManager.ReadSettings();

            ZPlannerManager.InitDockPanels();
            ZPlannerManager.ActiveMenuTab = ZPlannerManager.MainMenu.RibbonMenu.ActiveTab;

            if (ZPlannerManager.StackupPanel != null) ZPlannerManager.ShowStackupPanel();
            else ZPlannerManager.ShowStartPagePanel();

            ZPlannerManager.PropertyChanged += ZPlannerManager_PropertyChanged;
            ZPlannerManager.NeedToClear += ZPlannerManager_NeedToClear;

            ZPlannerManager.OpenPredefinedProjectFile();

            if (!Options.TheOptions.IsAlreadyStarted)
            {
                ZPlannerManager.AddInitDialogMessage("Entering user information ...");
                StartDialog startDlg = new StartDialog();
                startDlg.ShowDialog();
            }

            ZPlannerManager.StatusMenu.SetStatusReady();
            Application.Idle += Application_Idle;

            ZPlannerManager.AddInitDialogMessage("Application was successfully loaded.");
            ZPlannerManager.InitDialog.Close();
            ZPlannerManager.InitDialog = null;

            LoadFormLocation();
        }

        void Application_Idle(object sender, EventArgs e)
        {
            ZPlannerManager.AutoSave();
        }

        private void ZPlannerContainerPanel_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (!ZPlannerManager.ProposeToSaveProject())
            {
                e.Cancel = true;
                return;
            }

            if (!ZPlannerManager.ProposeToSaveDML())
            {
                e.Cancel = true;
                return;
            }

            SaveFormLocation();
            ZPlannerManager.WriteSettings();
            ZPlannerManager.SaveLastLayout();
            if (ZPlannerManager.ActiveMenuTab != null && ZPlannerManager.ActiveMenuTab.Value != "Library") ZPlannerManager.SaveLayoutForTab(ZPlannerManager.ActiveMenuTab.Value);

            ZPlannerManager.MessagePanel.ClearMessages();
            Application.Idle -= Application_Idle;
        }

        private void SaveFormLocation()
        {
            System.Drawing.Rectangle bounds = this.WindowState != FormWindowState.Normal ? this.RestoreBounds : this.DesktopBounds;
            Options.TheOptions.WindowLeft = bounds.Left;
            Options.TheOptions.WindowTop = bounds.Top;
            Options.TheOptions.WindowWidth = bounds.Width;
            Options.TheOptions.WindowHeight = bounds.Height;
            Options.TheOptions.WindowState = this.WindowState;
        }

        private void LoadFormLocation()
        {
            this.DesktopBounds = new Rectangle(Options.TheOptions.WindowLeft, Options.TheOptions.WindowTop, Options.TheOptions.WindowWidth, Options.TheOptions.WindowHeight);
            this.WindowState = Options.TheOptions.WindowState;
        }
    }
}
