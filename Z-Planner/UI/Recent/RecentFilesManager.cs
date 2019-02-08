using System.Collections.Generic;
using System.ComponentModel;
using System.IO;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Recent
{
    public partial class RecentFilesManager
    {
        private static List<string> recentFiles;

        internal static List<string> RecentFiles
        {
            get
            {
                if (recentFiles == null) recentFiles = LoadRecentFiles();
                return recentFiles;
            }
        }

        private static List<string> LoadRecentFiles()
        {
            List<string> files = new List<string>();
            string mruFilePath = ZSettings.MRUFile;
            if (!string.IsNullOrEmpty(mruFilePath))
            {
                if (!File.Exists(mruFilePath))
                {
                    using (FileStream fs = File.Create(mruFilePath))
                    {
                        fs.Close();
                    }
                }
                else foreach(string fileName in File.ReadLines(mruFilePath))
                    {
                        if (File.Exists(fileName)) files.Add(fileName);
                    }
            }

            return files;
        }

        internal static void AddToRecentFiles(string file)
        {
            RecentFiles.Remove(file);

            while (RecentFiles.Count >= ZSettings.MRULimit) RecentFiles.RemoveAt(RecentFiles.Count - 1);

            RecentFiles.Insert(0, file);

            NotifyRecentFilesChanged();

            string mruFilePath = ZSettings.MRUFile;
            if (!File.Exists(mruFilePath)) File.Create(mruFilePath);
            File.WriteAllLines(mruFilePath, RecentFiles);
        }

        public static event PropertyChangedEventHandler RecentFilesChanged;

        internal static void NotifyRecentFilesChanged()
        {
            if (RecentFilesChanged != null)
            {
                RecentFilesChanged(null, new PropertyChangedEventArgs("RecentFiles"));
            }
        }
    }
}
