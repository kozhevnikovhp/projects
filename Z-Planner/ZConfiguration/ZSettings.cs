using System;
using System.Collections.Generic;
using System.Configuration;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ZZero.ZPlanner.ZConfiguration
{
    /// <summary>
    /// Provides configuration settings.
    /// </summary>
    class ZSettings
    {
        private static Configuration config = ConfigurationManager.OpenExeConfiguration(Application.ExecutablePath);

        /// <summary>
        /// Provides the value of "Commands Limit" settings.
        /// <remarks>Commands Limit is a maximum count of Undo/Redo commands.</remarks>
        /// </summary>
        internal static uint CommandsLimit
        {
            get
            {
                uint result;
                if (uint.TryParse(GetValue("Commands Limit"), out result)) return result;
                return 20;
            }

            /*set 
            {
                SetValue("Commands Limit", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "DML File" settings.
        /// <remarks>DML File is a file that contains DML data records.</remarks>
        /// </summary>
        internal static string DMLFile
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("DML File"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("DML File"));
#endif
            }

            /*set
            {
                SetValue("DML File", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "Use Local Settings" settings.
        /// <remarks>Use Local Settings is a flag indicating where the settings fhould be found.</remarks>
        /// </summary>
        internal static bool UseLocalSettings
        {
            get
            {
                bool useLocalSettings;
                if (bool.TryParse(GetValue("Use Local Settings"), out useLocalSettings)) return useLocalSettings;
                return false;
            }
        }

        /// <summary>
        /// Provides the value of "DML Network File" settings.
        /// <remarks>DML Network File is a file that contains DML source data records.</remarks>
        /// </summary>
        internal static string DMLNetworkFile
        {
            get
            {
                //return Path.GetFullPath(GetValue("DML Network File"));
                return GetValue("DML Network File");
            }

            /*set
            {
                SetValue("DML Network File", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "MRU File" settings.
        /// <remarks>MRU File is a file that contains Recent files paths.</remarks>
        /// </summary>
        internal static string MRUFile
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("MRU File"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("MRU File"));
#endif
            }

            /*set
            {
                SetValue("MRU File", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "Settings File" settings.
        /// <remarks>Settings File is a file that contains application settings.</remarks>
        /// </summary>
        internal static string SettingsFile
        {
            get
            {
                return Path.Combine(SettingsFolder, GetValue("Settings File"));
            }
        }

        /// <summary>
        /// Provides the value of "Settings Folder" settings.
        /// <remarks>Settings Folder contains setting file and logo.</remarks>
        /// </summary>
        internal static string SettingsFolder
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("Settings Folder"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("Settings Folder"));
#endif
            }
        }

        /// <summary>
        /// Provides the value of "Tutorial File" settings.
        /// <remarks>Tutorial File is a file that contains application tutorial.</remarks>
        /// </summary>
        internal static string TutorialFile
        {
            get
            {
                return Path.Combine(HelpFolder, GetValue("Tutorial File"));
            }
        }

        /// <summary>
        /// Provides the value of "Tutorial Topics File" settings.
        /// <remarks>Tutorial Topics File is a file that contains topics of application tutorial.</remarks>
        /// </summary>
        internal static string TutorialTopicsFile
        {
            get
            {
                return Path.Combine(HelpFolder, GetValue("Tutorial Topics File"));
            }
        }

        /// <summary>
        /// Provides the value of "Help Folder" settings.
        /// <remarks>Help Folder contains tutorial and help files.</remarks>
        /// </summary>
        internal static string HelpFolder
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("Help Folder"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("Help Folder"));
#endif
            }
        }
        
        /// <summary>
        /// Provides the value of "MRU Limit" settings.
        /// <remarks>MRU Limit is a maximum count of Recent files.</remarks>
        /// </summary>
        internal static uint MRULimit
        {
            get
            {
                uint result;
                if (uint.TryParse(GetValue("MRU Limit"), out result)) return result;
                return 10;
            }

            /*set
            {
                SetValue("MRU Limit", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "Generic Stackups Folder" settings.
        /// <remarks>Generic Stackups Folder contains predesigned stackup files.</remarks>
        /// </summary>
        internal static string GenericStackupsFolder
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerHomeDirectory, GetValue("Generic Stackups Folder"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerHomeDirectory, GetValue("Generic Stackups Folder"));
#endif
            }

            /*set
            {
                SetValue("Generic Stackups Folder", value);
            }*/
        }

        /// <summary>
        /// Provides the value of "Custom Filters Folder" settings.
        /// <remarks>Custom Filter Folders contains saved custom filters.</remarks>
        /// </summary>
        internal static string CustomFiltersFolder
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("Custom Filters Folder"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("Custom Filters Folder"));
#endif
            }

            /*set
            {
                SetValue("Custom Filters Folder", value);
            }*/
        }
        
        /// <summary>
        /// Provides the value of "Layout Folder" settings.
        /// <remarks>Layout Folder contains layout files.</remarks>
        /// </summary>
        internal static string LayoutFolder
        {
            get
            {
#if ZSANDBOX
                return Path.Combine(ZZero.ZSolver.ZSolverManager.ZPlannerDataDirectory, GetValue("Layout Folder"));
#else
                return Path.Combine(ZPlannerManager.ZPlannerDataDirectory, GetValue("Layout Folder"));
#endif
            }

            /*set
            {
                SetValue("Layout Folder", value);
            }*/
        }

        /// <summary>
        /// Provides the value of "Source Node" settings.
        /// <remarks>Source Node is a node of data record.</remarks>
        /// </summary>
        internal static string SourceType
        {
            get
            {
                return GetValue("Source Type");
            }

            /*set
            {
                SetValue("Source Type", value.ToString());
            }*/
        }

        /// <summary>
        /// Provides the value of "FS Build Version" settings.
        /// <remarks>Build version of HyperLynx Field Solver.</remarks>
        /// </summary>
        internal static string FSBuildVersion
        {
            get
            {
                return GetValue("FS Build Version");
            }
        }

        /// <summary>
        /// Provides the value of "FS Build Date" settings.
        /// <remarks>Build date of HyperLynx Field Solver.</remarks>
        /// </summary>
        internal static string FSBuildDate
        {
            get
            {
                return GetValue("FS Build Date");
            }
        }

        private static string GetValue(string name)
        {
            var item = config.AppSettings.Settings[name];
            if (item != null) return config.AppSettings.Settings[name].Value;
            return string.Empty;
        }

        private static void SetValue(string name, string value)
        {
            config.AppSettings.Settings[name].Value = value;
            config.Save(ConfigurationSaveMode.Modified);
            ConfigurationManager.RefreshSection("appSettings");
        }
    }
}
