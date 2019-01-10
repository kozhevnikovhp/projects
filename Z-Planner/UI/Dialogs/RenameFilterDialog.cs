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

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class RenameFilterDialog : Form
    {
        string path;
        string oldName;
        Dictionary<string, string> filterFiles = new Dictionary<string, string>();

        public RenameFilterDialog()
        {
            InitializeComponent();
        }

        public void SetCurrentPath(string path)
        {
            this.path = path;

            foreach (var filePath in Directory.EnumerateFiles(Path.GetDirectoryName(path)))
            {
                using (StreamReader filterReader = new StreamReader(File.Open(filePath, FileMode.Open, FileAccess.Read)))
                {
                    filterFiles.Add(filterReader.ReadLine(), filePath);
                }
            }

            using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
            {
                oldName = filterReader.ReadLine();
                newFileName.Text = oldName;
            }
        }

        private void buttonOk_Click(object sender, EventArgs e)
        {
            if (!string.IsNullOrWhiteSpace(newFileName.Text))
            {
                string newName = newFileName.Text;
                
                if (newName != oldName)
                {
                    if (filterFiles.Keys.Contains(newName))
                    {
                        MessageBox.Show("A filter with this name already exists.", "Filter Name", MessageBoxButtons.OK);
                        DialogResult = System.Windows.Forms.DialogResult.Cancel;
                        Close();
                        return;
                    }

                    List<string> fileLines = new List<string>();
                    using (StreamReader filterReader = new StreamReader(File.Open(path, FileMode.Open, FileAccess.Read)))
                    {
                        while (filterReader.Peek() >= 0)
                        {
                            fileLines.Add(filterReader.ReadLine());
                        }
                    }

                    if (fileLines.Count > 0) fileLines[0] = newName;

                    using (StreamWriter filterWriter = new StreamWriter(File.Open(path, FileMode.Create, FileAccess.Write)))
                    {
                        foreach (string line in fileLines)
                        {
                            filterWriter.WriteLine(line);
                        }
                    }

                    DialogResult = System.Windows.Forms.DialogResult.OK;
                    Close();
                    return;
                }
            }

            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }

        private void buttonCancel_Click(object sender, EventArgs e)
        {
            DialogResult = System.Windows.Forms.DialogResult.Cancel;
            Close();
        }
    }
}
