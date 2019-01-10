using System;
using System.IO;
using System.Runtime.Serialization;
using System.Windows.Forms;
using System.Xml;
using ZZero.ZPlanner.Utils;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Settings
{
    [DataContract]
    public class Options
    {
        private static Options options;
        private Options(){ }
        public static Options TheOptions
        {
            get
            {
                if (options == null)
                {
                    options = new Options();
                }
                return options;
            }
            set
            {
                if (options == null)
                {
                    options = new Options();
                }
                options = value;
            }
        }

        public static bool IsOptionsEmpty
        {
            get { return (options == null); }
        }


        //============= Units
        public enum Units {English, Metric}
        public enum DateFormat {US, English, European} //MM/DD/YY, DD/MM/YY, YY/MM/DD
        public enum MetalThickness { Weight, Length} //oz, mils

        [DataMember(Name = "DataFormatUnits")]
        public Units units;
        [DataMember(Name = "DataFormatDate")]
        public DateFormat dateFormat;
        [DataMember(Name = "DataFormatMetalThickness")]
        public MetalThickness metalThickness;

        //=============== File Locations
        [DataMember(Name = "DataPathForProject")]
        public String ProjectPath; //z0p
        [DataMember(Name = "DataPathUseLast")]
        public bool UseLast;
        [DataMember(Name = "DataPathForExport")]
        public String ExportPath;

        [DataMember(Name = "DataPathForDMLLocal")]
        public String DML_LocalPath;
        [DataMember(Name = "DataPathForDMLNetwork")]
        public String DML_NetworkPath;

        [DataMember(Name = "DataPathForProgramLicense")]
        public String LicensePath;

        //=============== Dielectric Defaults
        [DataMember(Name = "DielectricDefaultDrill")]
        public double drill; //mils
        [DataMember(Name = "DielectricDefaultSoldermaskHeight")]
        public double soldermask_height; //range [0.25-1.5] mils
        [DataMember(Name = "DielectricDefaultSoldermaskDk")]
        public double soldermask_Dk; //range [1-7]
        [DataMember(Name = "DielectricDefaultSoldermaskDf")]
        public double soldermask_Df;

        //Core and Prepreg
        [DataMember(Name = "DielectricDefaultCorePrepregHeight")]
        public double height; //mils range[??..??]
        [DataMember(Name = "DielectricDefaultCorePrepregDk")]
        public double Dk; //[1-7]
        [DataMember(Name = "DielectricDefaultCorePrepregDf")]
        public double Df;

        //resin only
        [DataMember(Name = "DielectricDefaultResinDk")]
        public double resin_Dk; //[1-7]
        [DataMember(Name = "DielectricDefaultResinDf")]
        public double resin_Df;

        //================== Metal Defaults
        [DataMember(Name = "MetalDefaultTraceWidth")]
        public double trace_width; //mils [2-100]
        [DataMember(Name = "MetalDefaultTraceSpacing")]
        public double trace_spacing; //mils [2-100]
        [DataMember(Name = "MetalDefaultEtch")]
        public double etch; //[0.5-1] //narrow_top = true;

        //outer metal layers
        [DataMember(Name = "MetalDefaultOuterLayerPlatingThickness")]
        public double plating_thickness; //oz [0-20]
        [DataMember(Name = "MetalDefaultOuterLayerBaseTraceThickness")]
        public double base_trace_thickness; //oz [0-20]
        [DataMember(Name = "MetalDefaultOuterLayerSingleTraceWidth")]
        public double outer_trace_width; //mils [2-100]
        [DataMember(Name = "MetalDefaultOuterLayerDiffTraceWidth")]
        public double outer_diff_trace_width; //mils [2-100]
        [DataMember(Name = "MetalDefaultOuterLayerDiffTraceSpacing")]
        public double outer_diff_trace_spacing; //mils [2-100]

        //inner metal layers
        [DataMember(Name = "MetalDefaultInnerLayerCopperFoilThickness")]
        public double copper_foil_thickness;//oz [0-20]
        [DataMember(Name = "MetalDefaultInnerLayerSingleTraceWidth")]
        public double inner_trace_width; //mils [2-100]
        [DataMember(Name = "MetalDefaultInnerLayerDiffTraceWidth")]
        public double inner_diff_trace_width; //mils [2-100]
        [DataMember(Name = "MetalDefaultInnerLayerDiffTraceSpacing")]
        public double inner_diff_trace_spacing; //mils [2-100]

        [DataMember(Name = "MetalDefaultCopperThicknessFor0.5oz")]
        public double copper_thickness_05oz;
        [DataMember(Name = "MetalDefaultCopperThicknessFor1.0oz")]
        public double copper_thickness_10oz;
        [DataMember(Name = "MetalDefaultCopperThicknessFor1.5oz")]
        public double copper_thickness_15oz;
        [DataMember(Name = "MetalDefaultCopperThicknessFor2.0oz")]
        public double copper_thickness_20oz;
        [DataMember(Name = "MetalDefaultCopperThicknessFor2.5oz")]
        public double copper_thickness_25oz;

        //one more metal layer parameter    
        [DataMember(Name = "MetalDefaultBulkRes")]
        public double bulkRes;//[ohm-m];


        //======================== Copper Roughness
        //outer metal layers
        [DataMember(Name = "MetalDefaultCopperOuterLayerRoughness")]
        public double roughness; //um [0.5-15]

        //inner metal layers
        [DataMember(Name = "MetalDefaultCopperInnerLayerLaminateSideRoughness")]
        public double laminate_side_roughness; //um [0.5-15]
        [DataMember(Name = "MetalDefaultCopperInnerLayerPrepregSideRoughness")]
        public double prepreg_side_roughness; //um [0.5-15]

        
        //======================== Prepreg Thickness Adjustments
        [DataMember(Name = "PrepregThicknessAdjustmentsByCopperCoverage")]
        public bool ByCopperCoverage;
        //====== ByCopperCoverage = true;
        [DataMember(Name = "PrepregThicknessAdjustmentsForSignal")]
        public double forSignal;//%
        [DataMember(Name = "PrepregThicknessAdjustmentsForMixed")]
        public double forMixed;//%
        [DataMember(Name = "PrepregThicknessAdjustmentsForPlane")]
        public double forPlane;//%

        //====== ByCopperCoverage = false
        // prepreg percentage adjustment
        // prepreg is pressed between
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenSignalAndSignal")]
        public double signal_signal; //%
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenSignalAndMixed")]
        public double signal_mixed;
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenSignalAndPlane")]
        public double signal_plane;
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenMixedAndMixed")]
        public double mixed_mixed;
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenMixedAndPlane")]
        public double mixed_plane;
        [DataMember(Name = "PrepregThicknessAdjustmentsBetweenPlaneAndPlane")]
        public double plane_plane;

        //============================ User Info
        [DataMember(Name = "UserInfoName")]
        public String UserName;
        [DataMember(Name = "UserInfoTitle")]
        public String Title;
        [DataMember(Name = "UserInfoCompany")]
        public String Company;
        [DataMember(Name = "UserInfoDivision")]
        public String Division;
        [DataMember(Name = "UserInfoSite")]
        public String Site;

        [DataMember(Name = "UserInfoEMail")]
        public string UserEMail;
        [DataMember(Name = "UserInfoLogo")]
        public string UserLogo;
        [DataMember(Name = "UserInfoIsAlreadyStarted")]
        public bool IsAlreadyStarted;


        // Frequency
        [DataMember(Name = "DefaultFrequency")]
        public double Frequency;

        //precision
        [DataMember(Name = "DataPrecisionWeightDigits")]
        public int weightDigits;
        [DataMember(Name = "DataPrecisionLengthDigits")]
        public int lengthDigits;
        [DataMember(Name = "DataPrecisionImpedanceDigits")]
        public int impedanceDigits;
        [DataMember(Name = "DataPrecisionTemperatureDigits")]
        public int temperatureDigits;
        [DataMember(Name = "DataPrecisionDkDigits")]
        public int dkDigits;
        [DataMember(Name = "DataPrecisionDfDigits")]
        public int dfDigits;
        [DataMember(Name = "DataPrecisionPercentDigits")]
        public int percentDigits;
        [DataMember(Name = "DataPrecisionFrequencyDigits")]
        public int frequencyDigits;


        // Stackup Grid settings
        [DataMember(Name = "UIGridSettingsStackupColumnsOrder")]
        public string stackupColumnsOrder = "";
        [DataMember(Name = "UIGridSettingsStackupColumnsWidth")]
        public string stackupColumnsWidth = "";
        [DataMember(Name = "UIGridSettingsStackupColumnsIsHidden")]
        public string stackupColumnsIsHidden = "";
        [DataMember(Name = "UIGridSettingsStackupColumnsIsCollapsed")]
        public string stackupColumnsIsCollapsed = "";


        [DataMember(Name = "UIGridSettingsViaSpanNumberOfDisplayedSpans")]
        public int NumberOfDisplayedViaSpans;


        // DML Grid settings
        [DataMember(Name = "UIGridSettingsDmlColumnsOrder")]
        public string dmlColumnsOrder;
        [DataMember(Name = "UIGridSettingsDmlColumnsWidth")]
        public string dmlColumnsWidth;
        [DataMember(Name = "UIGridSettingsDmlColumnsIsHidden")]
        public string dmlColumnsIsHidden;
        [DataMember(Name = "UIGridSettingsDmlColumnsIsCollapsed")]
        public string dmlColumnsIsCollapsed;


        // DML
        [DataMember(Name = "DataPermissionsAllowZZeroLibrary")]
        public bool allowZZeroLibrary;
        [DataMember(Name = "DataPermissionsAllowCorporateLibrary")]
        public bool allowCorporateLibrary;
        [DataMember(Name = "DataPermissionsAllowLocalLibrary")]
        public bool allowLocalLibrary;
        [DataMember(Name = "DataPermissionsAllowZZeroSynchronization")]
        public bool allowZZeroSynchronization;

        // Stackup
        [DataMember(Name = "DataPermissionsAllowStackupSwitches")]
        public bool allowStackupSwitches;

        // Auto-save
        [DataMember(Name = "DataPermissionsAutoSaveInterval")]
        public int autosaveInterval;

        // DRC Errors/Warnings
        [DataMember(Name = "DataPermissionsDisplayDRCErrors")]
        public bool displayDRCErrors;
        [DataMember(Name = "DataPermissionsDisplayDRCWarnings")]
        public bool displayDRCWarnings;

        // Switches
        [DataMember(Name = "SwitchesEnableRoughness")]
        public bool enabledIsRoughness;
        [DataMember(Name = "SwitchesEnablePressedThickness")]
        public bool enabledIsPressedThickness;
        [DataMember(Name = "SwitchesEnableSequentialLamination")]
        public bool enabledIsSequentialLamination;
        [DataMember(Name = "SwitchesEnableTrapezoidalTraces")]
        public bool enabledIsTrapezoidalTraces;
        [DataMember(Name = "SwitchesEnableLossPlanning")]
        public bool enabledIsLossPlanning;
        [DataMember(Name = "SwitchesEnableGlassPitch")]
        public bool enabledIsGlassPitch;
        [DataMember(Name = "SwitchesEnableGws")]
        public bool enabledIsGws;
        [DataMember(Name = "SwitchesEnableCenterLine")]
        public bool enabledIsCenterLineVisible;
        [DataMember(Name = "SwitchesEnableCoreLocked")]
        public bool enabledIsCoreLocked;
        [DataMember(Name = "SwitchesEnableAutoMirror")]
        public bool enabledIsAutoMirror;
        [DataMember(Name = "SwitchesEnableHeaders")]
        public bool enabledIsHeadersVisible;
        [DataMember(Name = "SwitchesEnableDisabledCells")]
        public bool enabledIsColorDisabledCells;

        // Filters
        [DataMember(Name = "FiltersCategory")]
        public string filtersCategory;


        // Main Form
        [DataMember(Name = "UIWindowSettingsLeft")]
        public int WindowLeft;
        [DataMember(Name = "UIWindowSettingsTop")]
        public int WindowTop;
        [DataMember(Name = "UIWindowSettingsWidth")]
        public int WindowWidth;
        [DataMember(Name = "UIWindowSettingsHeight")]
        public int WindowHeight;
        [DataMember(Name = "UIWindowSettingsState")]
        public FormWindowState WindowState;




        // Default Values
        [OnDeserializing]
        internal void OnDeserializing(StreamingContext context)
        {
            units = Units.English;
            dateFormat = DateFormat.English;
            metalThickness = MetalThickness.Weight;

            //=============== File Locations
#if ZSANDBOX
            ProjectPath = ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory; //z0p
            UseLast = true;
            ExportPath = ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory;

            DML_LocalPath = Path.Combine(ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory, "DML", "LocalLibrary.xml");
            DML_NetworkPath = Path.Combine(ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory, "DML", "CorporateLibrary.xml");

            LicensePath = Path.Combine(ZZero.ZSandbox.ZSandboxManager.ZPlannerDataDirectory, "License");
#else
            ProjectPath = ZPlannerManager.ZPlannerDataDirectory; //z0p
            UseLast = true;
            ExportPath = ZPlannerManager.ZPlannerDataDirectory;

            DML_LocalPath = Path.Combine(ZPlannerManager.ZPlannerDataDirectory, "DML", "LocalLibrary.xml");
            DML_NetworkPath = Path.Combine(ZPlannerManager.ZPlannerDataDirectory, "DML", "CorporateLibrary.xml");

            LicensePath = Path.Combine(ZPlannerManager.ZPlannerDataDirectory, "License");
#endif
            string p2 = Environment.GetEnvironmentVariable("ZZERO_LICENSE_FILE");
            if (!String.IsNullOrEmpty(p2))
            {
                LicensePath = LicensePath + Environment.NewLine + p2;
            }

            //=============== Dielectric Defaults
            drill = 8; //mils
            soldermask_height = 0.5; //range [0.25-1.5] mils
            soldermask_Dk = 3.3; //range [1-7]
            soldermask_Df = 0.02;

            //Core and Prepreg
            height = 5; //mils range[??..??]
            Dk = 4; //[1-7]
            Df = 0.018;

            //resin only
            resin_Dk = 3.3; //[1-7]
            resin_Df = 0.022;

            //================== Metal Defaults
            trace_width = 5.0; //mils [2-100]
            trace_spacing = 10; //mils [2-100]
            etch = 0.9; //[0.5-1] //narrow_top = true;

            //outer metal layers
            plating_thickness = 1; //oz [0-20]
            base_trace_thickness = 0.5; //oz [0-20]
            roughness = 5; //um [0.5-15]

            //inner metal layers
            copper_foil_thickness = 0.5;//oz [0-20]
            laminate_side_roughness = 5; //um [0.5-15]
            prepreg_side_roughness = 3.5; //um [0.5-15]

            copper_thickness_05oz = 0.6;
            copper_thickness_10oz = 1.2;
            copper_thickness_15oz = 1.8;
            copper_thickness_20oz = 2.4;
            copper_thickness_25oz = 3.0;

            //======================== Prepreg Thickness Adjustments
            ByCopperCoverage = true;
            // For ByCopperCoverage = true;
            forSignal = 50;//%
            forMixed = 66;//%
            forPlane = 90;//%

            // For ByCopperCoverage = false
            // prepreg percentage adjustment
            //prepreg is pressed between
            signal_signal = 80; //%
            signal_mixed = 85;
            signal_plane = 90;
            mixed_mixed = 90;
            mixed_plane = 92;
            plane_plane = 95;

            Frequency = 1;

            //precision
            weightDigits = 1;
            lengthDigits = 1;
            impedanceDigits = 1;
            temperatureDigits = 0;
            dkDigits = 2;
            dfDigits = 3;
            percentDigits = 1;
            frequencyDigits = 2;

            // DML Grid settings
            dmlColumnsOrder = "";
            dmlColumnsWidth = "";
            dmlColumnsIsHidden = "";
            dmlColumnsIsCollapsed = "";

            // DML
            allowZZeroLibrary = true;
            allowCorporateLibrary = true;
            allowLocalLibrary = true;
            allowZZeroSynchronization = true;

            // Stackup
            allowStackupSwitches = false;

            // Auto-save
            autosaveInterval = 10;

            // DRC Errors/Warnings
            displayDRCErrors = false;
            displayDRCWarnings = false;

            // Switches
            enabledIsRoughness = false;
            enabledIsPressedThickness = true;
            enabledIsSequentialLamination = true;
            enabledIsTrapezoidalTraces = true;
            enabledIsLossPlanning = false;
            enabledIsGlassPitch = false;
            enabledIsGws = false ;
            enabledIsCenterLineVisible = false;
            enabledIsCoreLocked = true;
            enabledIsAutoMirror = false;
            enabledIsHeadersVisible = true;
            enabledIsColorDisabledCells = false;

            // Filters
            filtersCategory = "'Z-zero'";

            // Main Form
            WindowLeft = 0;
            WindowTop = 0;
            WindowWidth = 640;
            WindowHeight = 480;
            WindowState = FormWindowState.Maximized;

            //one more metal layer parameter    
            bulkRes = 1.724e-8;//[ohm-m];

            // Stackup Grid settings
            stackupColumnsOrder = "";
            stackupColumnsWidth = "";
            stackupColumnsIsHidden = "";
            stackupColumnsIsCollapsed = "";
            
            NumberOfDisplayedViaSpans = 5;

            //================== Metal Defaults
            outer_trace_width = 8.0; //mils [2-100]
            inner_trace_width = 4.5; //mils [2-100]
            outer_diff_trace_width = 5.5; //mils [2-100]
            inner_diff_trace_width = 4.0; //mils [2-100]
            outer_diff_trace_spacing = 10.0; //mils [2-100]
            inner_diff_trace_spacing = 10.0; //mils [2-100]
        }
    }

    public class Settings_IO
    {
        public void WriteSettings(ref Options opt)
        {
            try
            {
                using (FileStream writer = new FileStream(ZSettings.SettingsFile, FileMode.Create))
                {
                    DataContractSerializer ser = new DataContractSerializer(typeof(Options));
                    ser.WriteObject(writer, opt);
                }
            }
            catch { }

            try
            {
                using (var writer = XmlWriter.Create(ZSettings.SettingsFile, new XmlWriterSettings { Indent = true, IndentChars = ("\t"), OmitXmlDeclaration = true }))
                {
                    DataContractSerializer ser = new DataContractSerializer(typeof(Options));
                    ser.WriteObject(writer, opt);
                }
            }
            catch { }
        }

        public void ReadSettings()
        {
            try
            {
                using (FileStream fs = new FileStream(ZSettings.SettingsFile, FileMode.Open))
                {
                    XmlDictionaryReader reader = XmlDictionaryReader.CreateTextReader(fs, new XmlDictionaryReaderQuotas());
                    DataContractSerializer ser = new DataContractSerializer(typeof(Options));

                    // Deserialize the data and read it from the instance.
                    Options.TheOptions = (Options)ser.ReadObject(reader, true);
                }
            }
            catch
            {
                Options opt = Options.TheOptions;
                opt.OnDeserializing(new StreamingContext());
                WriteSettings(ref opt);
            }
        }
    }
}