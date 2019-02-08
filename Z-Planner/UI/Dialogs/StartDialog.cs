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
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI.Dialogs
{
    public partial class StartDialog : Form
    {
        public StartDialog()
        {
            InitializeComponent();
            LoadOptions();
        }

        private void bCompanyLogo_Click(object sender, EventArgs e)
        {
            string initial_path = Path.Combine(ZSettings.SettingsFolder);

            OpenFileDialog fd = new OpenFileDialog();
            fd.CheckFileExists = true;
            fd.Filter = "ImagePEG Files (*.jpeg; *.jpg; *.png; *.gif; *.bmp)|*.jpeg;*.jpg;*.png;*.gif;*.bmp|All files (*.*)|*.*";
            fd.Title = "Open company Logo file.";
            fd.RestoreDirectory = true;

            if (initial_path != String.Empty) fd.InitialDirectory = initial_path;

            DialogResult result = fd.ShowDialog();
            if (result == DialogResult.OK && !string.IsNullOrWhiteSpace(fd.FileName))
            {
                if (Path.GetFileName(fd.FileName) == Path.GetFileName(ZSettings.SettingsFile) || Path.Combine(ZSettings.SettingsFolder, tbCompanyLogo.Text) == fd.FileName) return;

                if (fd.FileName != Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName))) File.Copy(fd.FileName, Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName)), true);
                string logoPath = fd.FileName;

                tbCompanyLogo.Text = Path.GetFileName(fd.FileName);
                ShowImage(Path.Combine(ZSettings.SettingsFolder, Path.GetFileName(fd.FileName)));
            }
        }

        private void ShowImage(string fileToDisplay)
        {
            using (FileStream fs = File.OpenRead(fileToDisplay))
            {
                pbCompanyLogo.Image = new Bitmap(fs);
            }
        }

        private void btnOK_Click(object sender, EventArgs e)
        {
            SaveOptions();
            Close();
        }

        private void btnCancel_Click(object sender, EventArgs e)
        {
            CancelOptions();
            Close();
        }

        public void LoadOptions()
        {
            ZPlannerManager.ReadSettings();
            Options options = Options.TheOptions;

            // User
            tbUserName.Text = options.UserName ?? string.Empty;
            tbInitials.Text = options.Title ?? string.Empty;
            tbCompany.Text = options.Company ?? string.Empty;
            tbDivision.Text = options.Division ?? string.Empty;
            tbSiteLocation.Text = options.Site ?? string.Empty;
            tbEMail.Text = options.UserEMail ?? string.Empty;
            tbCompanyLogo.Text = options.UserLogo ?? string.Empty;
            if (!string.IsNullOrWhiteSpace(options.UserLogo)) ShowImage(Path.Combine(ZSettings.SettingsFolder, options.UserLogo));
        }

        public void SaveOptions()
        {
            Options options = Options.TheOptions;
            
            // User
            options.UserName = tbUserName.Text;
            options.Title = tbInitials.Text;
            options.Company = tbCompany.Text;
            options.Division = tbDivision.Text;
            options.Site = tbSiteLocation.Text;
            options.UserEMail = tbEMail.Text;
            options.UserLogo = tbCompanyLogo.Text;
            options.IsAlreadyStarted = true;

            Options.TheOptions = options;

            Settings_IO io = new Settings_IO();
            io.WriteSettings(ref options);
        }

        public void CancelOptions()
        {
            Options options = Options.TheOptions;

            // User
            options.IsAlreadyStarted = true;

            Options.TheOptions = options;

            Settings_IO io = new Settings_IO();
            io.WriteSettings(ref options);
        }
    }
}
