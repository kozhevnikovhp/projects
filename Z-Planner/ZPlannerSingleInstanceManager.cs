using Microsoft.VisualBasic.ApplicationServices;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.UI;

namespace ZZero.ZPlanner
{
    class ZPlannerSingleInstanceManager: WindowsFormsApplicationBase
    {
        private ZPlannerContainerPanel containerPanel;
        private System.Collections.ObjectModel.ReadOnlyCollection<string> commandLine;

        public ZPlannerSingleInstanceManager()
        {
            IsSingleInstance = true;
        }

        protected override bool OnStartup(Microsoft.VisualBasic.ApplicationServices.StartupEventArgs eventArgs)
        {
            // First time _application is launched
            commandLine = eventArgs.CommandLine;

            foreach (string arg in commandLine)
            {
                if (arg.ToLower().EndsWith(".z0p")) ZPlannerManager.ProjectFileToBeOpened = arg;
                if (arg.ToLower().EndsWith(".z0lib")) ZPlannerManager.LibraryFileToBeOpened = arg;
            }

            //check license
            ZPlannerManager.rights = new Rights();

            ZPlannerManager.InitializeUndoRedo();

            containerPanel = ZPlannerManager.ContainerPanel;
            Application.Run(containerPanel);

            return false;
        }

        protected override void OnStartupNextInstance(StartupNextInstanceEventArgs eventArgs)
        {
            // Subsequent launches
            base.OnStartupNextInstance(eventArgs);
            commandLine = eventArgs.CommandLine;

            // Reactivate the main window
            if (containerPanel.WindowState == FormWindowState.Minimized)
            {
                containerPanel.WindowState = FormWindowState.Normal;
            }

            ((Form)containerPanel).Activate();

            foreach (string arg in commandLine)
            {
                if (arg.ToLower().EndsWith(".z0p")) ZPlannerManager.OpenProject(arg);
            }
        }
    }
}
