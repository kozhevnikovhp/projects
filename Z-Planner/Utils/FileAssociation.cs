using Microsoft.Win32;
using System;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using ZZero.ZPlanner;

namespace ZZero.ZPlanner.Utils
{
    public class FileAssociation
    {
        public enum FileToAssociate { ZPlannerProject, ZoLibrary, CorporateLibrary, LocalLibrary }

        private static string programPath = Path.Combine(Environment.CurrentDirectory, "Z-planner.exe");
        private static string programID = "ZZero.ZPlanner";

        public static void CheckAssociation(FileToAssociate fileToAssociate)
        {
            string extension = GetExtension(fileToAssociate);
            if (extension == string.Empty) return;

            if (!FileAssociation.IsAssociated(extension, programID))
                Associate(extension, programID, GetExtensionDescription(fileToAssociate), GetExtensionIcon(fileToAssociate), programPath);

        }

        private static string GetExtension(FileToAssociate fileToAssociate)
        {
            switch (fileToAssociate)
            {
                case FileToAssociate.ZPlannerProject:
                    return ".z0p";
                case FileToAssociate.ZoLibrary:
                    return ".z0library";
                case FileToAssociate.CorporateLibrary:
                    return ".corplibrary";
                case FileToAssociate.LocalLibrary:
                    return ".loclibrary";
                default:
                    return string.Empty;
            }
        }

        private static string GetExtensionDescription(FileToAssociate fileToAssociate)
        {
            switch (fileToAssociate)
            {
                case FileToAssociate.ZPlannerProject:
                    return "Z-Planer Project";
                case FileToAssociate.ZoLibrary:
                    return "Zo Library";
                case FileToAssociate.CorporateLibrary:
                    return "Corporate Library";
                case FileToAssociate.LocalLibrary:
                    return "Local Library";
                default:
                    return string.Empty;
            }
        }

        private static string GetExtensionIcon(FileToAssociate fileToAssociate)
    {
        switch (fileToAssociate)
        {
            case FileToAssociate.ZPlannerProject:
                return Path.Combine(Environment.CurrentDirectory, "ZPlanner Project Icon.ico");
            case FileToAssociate.ZoLibrary:
                return Path.Combine(Environment.CurrentDirectory, "ZPlanner Project Icon.ico");
            case FileToAssociate.CorporateLibrary:
                return Path.Combine(Environment.CurrentDirectory, "ZPlanner Project Icon.ico");
            case FileToAssociate.LocalLibrary:
                return Path.Combine(Environment.CurrentDirectory, "ZPlanner Project Icon.ico");
            default:
                return string.Empty;
        }
    }

        // Associate file extension with progID, description, icon and application
        public static void Associate(string extension,
               string progID, string description, string icon, string application)
        {
            Registry.CurrentUser.OpenSubKey("Software").OpenSubKey("Classes", true).CreateSubKey(extension).SetValue("", progID);
            if (progID != null && progID.Length > 0)
                using (RegistryKey key = Registry.CurrentUser.OpenSubKey("Software").OpenSubKey("Classes", true).CreateSubKey(progID))
                {
                    if (description != null)
                        key.SetValue("", description);
                    if (icon != null)
                        key.CreateSubKey("DefaultIcon").SetValue("", ToShortPathName(icon));
                    if (application != null)
                        key.CreateSubKey("WorkDirectory").SetValue("", Environment.CurrentDirectory);
                    if (application != null)
                        key.CreateSubKey(@"Shell\Open\Command").SetValue("",
                                    ToShortPathName(application) + " \"%1\"");
                }
        }

        // Return true if extension already associated in registry
        public static bool IsAssociated(string extension, string progID)
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey("Software").OpenSubKey("Classes").OpenSubKey(extension, false);
            return (key != null);
        }

        public static string GetWorkDirectory()
        {
            RegistryKey key = Registry.CurrentUser.OpenSubKey("Software").OpenSubKey("Classes").OpenSubKey(programID, false);
            if (key != null) 
            {
                return key.OpenSubKey("WorkDirectory").GetValue("").ToString();
            }

            return AppDomain.CurrentDomain.BaseDirectory;
        }

        [DllImport("Kernel32.dll")]
        private static extern uint GetShortPathName(string lpszLongPath,
            [Out] StringBuilder lpszShortPath, uint cchBuffer);

        // Return short path format of a file name
        private static string ToShortPathName(string longName)
        {
            StringBuilder s = new StringBuilder(1000);
            uint iSize = (uint)s.Capacity;
            uint iRet = GetShortPathName(longName, s, iSize);
            return s.ToString();
        }
    }
}