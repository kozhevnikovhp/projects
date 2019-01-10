using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.Serialization;
using System.Text;
using System.Windows.Forms;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.UI.Dialogs;
#if ZSANDBOX
using ZZero.ZSandbox;
#endif

namespace ZZero.ZPlanner
{
    public class License
    {
        private static License license;
        private License() { 
            dml = "All";
            timeZones = new List<int>();
        }
        public static License TheLicense
        {
            get
            {
                if (license == null)
                {
                    license = new License();
                }
                return license;
            }
            set
            {
                if (license == null)
                {
                    license = new License();
                }
                license = value;
            }
        }

        public string path;
        public string error;
        //generic parameters
        public string feature, daemon, daemonVersion, count, expire, type;
        //vendor string
        private string param;
        public string version; //N.NNN
        public string layers; //UL, 12L, 6L
        public string dml;
        public bool bAllowDmlUpdate, bAllowDmlSave;
        public List<int> timeZones;

        public void ParseParams(string param)//<name>:value;<name>:value
        {
            string[] pars = param.Split(';');
            foreach (string s in pars)
            {
                string[] pair = s.Split(':');
                string name = pair[0];
                string value = pair[1];
                switch (name)
                {
                    case "version":
                        License.TheLicense.version = value;
                        break;
                    case "layers":
                        License.TheLicense.layers = value;
                        break;
                    case "dml":
                        License.TheLicense.dml = value;
                        break;
                    case "update":
                        switch (value) {
                            case "0": License.TheLicense.bAllowDmlUpdate = false; break;
                            case "1": License.TheLicense.bAllowDmlUpdate = true; break;
                        }
                        break;
                    case "save":
                        switch (value)
                        {
                            case "0": License.TheLicense.bAllowDmlSave = false; break;
                            case "1": License.TheLicense.bAllowDmlSave = true; break;
                        }
                        break;
                    case "zones":
                        timeZones.Clear();
                        if (value != "All")
                        {
                            string[] zones = value.Split('/');
                            foreach (string zone in zones)
                            {
                                int z;
                                Int32.TryParse(zone, out z);
                                timeZones.Add(z);
                            }
                        }
                        break;
                }
            }
        }
        public void ParseLicInfo(string info)
        {
            string[] pars = info.Split(',');
            foreach (string s in pars)
            {
                string[] pair = s.Split('=');
                string name = pair[0];
                string value = pair[1];

                switch (name)
                {
                    case "error":
                        License.TheLicense.error = value;
                        break;
                    case "feature":
                        feature = value;
                        break;
                    case "version":
                        daemonVersion = value;
                        break;
                    case "daemon":
                        daemon = value;
                        break;
                    case "count":
                        count = value;
                        break;
                    case "expire":
                        expire = value;
                        break;
                    case "type":
                        type = value;
                        break;
                    case "param":
                        param = value;
                        if (value.Length > 0)
                        {
                            ParseParams(value);
                        }
                        break;
                }
            }
        }
    }


    class FlexLm
    {
        [DllImport("LicDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern int get_lic(String licSearchPath, String featureName, StringBuilder output);

        [DllImport("LicDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void return_lic(String feature);


        public bool GetLicense()
        {
#if ZSANDBOX
            ZZero.ZSandbox.ZSandboxManager.ReadSettings();
            string dataDictionary = ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;
#else
            ZPlannerManager.ReadSettings();
            string dataDictionary = ZPlannerManager.ZPlannerDataDirectory;
#endif

            string LicPaths = Options.TheOptions.LicensePath;

            //if not set try default
            if (String.IsNullOrEmpty(LicPaths))
            {
                LicPaths = Path.Combine(dataDictionary, "License");
                string p2 = Environment.GetEnvironmentVariable("ZZERO_LICENSE_FILE");
                if (!String.IsNullOrEmpty(p2))
                {
                    LicPaths = LicPaths + Environment.NewLine + p2;
                }
            }

            string[] lines = LicPaths.Split(new string[] { Environment.NewLine }, StringSplitOptions.None);
            foreach (string path in lines)
            {
                //check if folder exists and license files are there
                if (!Directory.Exists(path)) continue;
                if (Directory.GetFiles(path, "*.lic").Length == 0) continue;

                if (TryPath(path)) return true;
            }
            return false;
        }

        public bool TryPath(string path)
        {
            string licInfo;
            if ((checkOutFeature(path, Program.ProductFeature, out licInfo) != 0))
            {
                return false;
            }

            //get license info
            License.TheLicense.path = path;
            License.TheLicense.ParseLicInfo(licInfo);

            return true;
        }

        public int checkOutFeature(string path, string feature, out string licInfo)
        {
            int res = -1;
            licInfo = "";

            const int buffer_size = 10000;
            StringBuilder sb = new StringBuilder(buffer_size);
            res = get_lic(path, feature, sb);
            licInfo = sb.ToString();

            return res;
        }

    }

    class Rights
    {
        public enum Role {User, Admin, SuperUser}
        private Role role;
        public Role maxRole { get; private set; }
        public int MaxLayersNumber { get; private set; } 
        public int MajorVersion { get; private set; }
        public int MinorVersion { get; private set; } 
        public string DmlManufacturer { get; private set; }
        public bool AllowDmlUpdate { get; private set; }
        public bool AllowDmlSave { get; private set; }
        public List<int> AllowedTimeZones { get; private set; }

        public Role currentRole
        {
            get
            {
                return role;
            }

            set
            {
                if ((int)value < (int)maxRole)
                {
                    role = value;
                }
            }
        }

        public Rights()
        {
            MaxLayersNumber = 0;
            role = Role.User;

            //check-out license
            //
            FlexLm lic = new FlexLm();

            while (true)
            {
                if (lic.GetLicense())
                {
                    break;
                }
                else
                {
                    MessageBox.Show("To run the program, please click the OK button below to specify a path to a valid license file in the Settings > File Locations dialog; or use the Upload button to upload the your license file to the default location.",
                                    "Licensing", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);

                    SettingsDialog options = new SettingsDialog(SettingsOpenMode.license);
                    if (options.ShowDialog() != DialogResult.OK)
                    {
                        Environment.Exit(0);
                    }
                }
            }

            // MaxLayersNumber
            switch (License.TheLicense.layers)
            {
                case "UL": MaxLayersNumber = int.MaxValue; break;
                case "12L": MaxLayersNumber = 12; break;
                case "6L": MaxLayersNumber = 6; break;
                //default: MaxLayersNumber = 0; break;
            }

            // Version
            string[] versions = License.TheLicense.version.Trim().Split('.');
            if (License.TheLicense.version.Trim().ToUpper() == "ANY")
            {
                MajorVersion = int.MaxValue;
                MinorVersion = int.MaxValue;
            }
            else if (versions.Length == 2)
            {
                int major, minor;

                if (versions[0] == "*") MajorVersion = int.MaxValue;
                else if (int.TryParse(versions[0], out major)) MajorVersion = major;

                if (versions[1] == "*") MinorVersion = int.MaxValue;
                else if (int.TryParse(versions[1], out minor)) MinorVersion = minor;
            }

            AllowedTimeZones = License.TheLicense.timeZones;

            // DML Filter
            DmlManufacturer = License.TheLicense.dml;

            // Dml Synchronization
            AllowDmlUpdate = License.TheLicense.bAllowDmlUpdate;

            // DML Administrator
            AllowDmlSave = License.TheLicense.bAllowDmlSave;



            // Check Version
            {
                int majorVersion, minorVersion;

                string productFileLocation = Assembly.GetExecutingAssembly().Location;
                FileVersionInfo fileVersionInfo = FileVersionInfo.GetVersionInfo(productFileLocation);
                majorVersion = fileVersionInfo.ProductMajorPart;
                minorVersion = fileVersionInfo.ProductMinorPart;

                if (MajorVersion <= majorVersion)
                {
                    if (MajorVersion < majorVersion || MinorVersion < minorVersion)
                    {

                        MessageBox.Show("The found license is not valid because the restrictions set in this license are not met. " + Environment.NewLine + "To run the program, please click the OK button below to specify a path to a valid license file in the Settings > File Locations dialog; or use the Upload button to upload the your license file to the default location.",
                                                "Licensing", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);

                        SettingsDialog options = new SettingsDialog(SettingsOpenMode.license);
                        if (options.ShowDialog() != DialogResult.OK)
                        {
                            Environment.Exit(0);
                        }
                    }
                }
            }

            // Check TimeZone
            {
                if (AllowedTimeZones.Count > 0)
                {
                    int span = TimeZoneInfo.Local.BaseUtcOffset.Hours;
                    int span1 = (TimeZoneInfo.Local.BaseUtcOffset.Minutes > 0) ? span + 1 : span;
                    if (!AllowedTimeZones.Contains(span) && !AllowedTimeZones.Contains(span1))
                    {
                        MessageBox.Show("The found license is not valid because the restrictions set in this license are not met. " + Environment.NewLine + "To run the program, please click the OK button below to specify a path to a valid license file in the Settings > File Locations dialog; or use the Upload button to upload the your license file to the default location.",
                                                "Licensing", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);

                        SettingsDialog options = new SettingsDialog(SettingsOpenMode.license);
                        if (options.ShowDialog() != DialogResult.OK)
                        {
                            Environment.Exit(0);
                        }
                    }
                }
            }
        }
    }

    public class LicenseEcxeption : Exception
    {
        // Summary:
        //     Initializes a new instance of the LicenseEcxeption class.
        public LicenseEcxeption() : base()
        {}

        //
        // Summary:
        //     Initializes a new instance of the LicenseEcxeption class with a specified
        //     error message.
        //
        // Parameters:
        //   message:
        //     The message that describes the error.
        public LicenseEcxeption(string message) : base(message)
        {}

        //
        // Summary:
        //     Initializes a new instance of the LicenseEcxeption class with serialized
        //     data.
        //
        // Parameters:
        //   info:
        //     The System.Runtime.Serialization.SerializationInfo that holds the serialized
        //     object data about the exception being thrown.
        //
        //   context:
        //     The System.Runtime.Serialization.StreamingContext that contains contextual
        //     information about the source or destination.
        //
        // Exceptions:
        //   System.ArgumentNullException:
        //     The info parameter is null.
        //
        //   System.Runtime.Serialization.SerializationException:
        //     The class name is null or System.Exception.HResult is zero (0).
        protected LicenseEcxeption(SerializationInfo info, StreamingContext context) : base(info, context)
        {}

        //
        // Summary:
        //     Initializes a new instance of the LicenseEcxeption class with a specified
        //     error message and a reference to the inner exception that is the cause of
        //     this exception.
        //
        // Parameters:
        //   message:
        //     The error message that explains the reason for the exception.
        //
        //   innerException:
        //     The exception that is the cause of the current exception, or a null reference
        //     (Nothing in Visual Basic) if no inner exception is specified.
        public LicenseEcxeption(string message, Exception innerException) : base(message, innerException)
        { }
    }
}