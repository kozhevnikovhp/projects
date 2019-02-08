#define ASSERT_OFF
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Globalization;
using System.IO;
using System.Linq;
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
    static class Program
    {
        public static string ProductFeature = "ZPlanner";
        private static Mutex MutexIstance;
        private const string MutexName = "Z-zero.Z-planner"; 

        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
            Thread.CurrentThread.CurrentUICulture = CultureInfo.InvariantCulture;

            // Add the event handler for handling UI thread exceptions to the event.
            Application.ThreadException += Application_ThreadException;
            // Set the unhandled exception mode to force all Windows Forms errors to go through
            // our handler.
            Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);
            // Add the event handler for handling non-UI thread exceptions to the event. 
            System.AppDomain.CurrentDomain.UnhandledException += CurrentDomain_UnhandledException;

            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);

#if ASSERT_OFF
            System.Diagnostics.Trace.Listeners.Clear();
#endif
            var singleInstanceManager = new ZPlannerSingleInstanceManager();
            singleInstanceManager.Run(Environment.GetCommandLineArgs());
        }

        static void Application_ThreadException(object sender, ThreadExceptionEventArgs e)
        {
            if (ZPlannerManager.ContainerPanel != null && ZPlannerManager.StatusMenu != null) ZPlannerManager.StatusMenu.SetStatus("ERROR: " + e.Exception.Message);
        }

        static void CurrentDomain_UnhandledException(object sender, UnhandledExceptionEventArgs e)
        {
            if (ZPlannerManager.ContainerPanel != null && ZPlannerManager.StatusMenu != null) ZPlannerManager.StatusMenu.SetStatus("ERROR: " + e.ExceptionObject.ToString());
        }
    }
}
