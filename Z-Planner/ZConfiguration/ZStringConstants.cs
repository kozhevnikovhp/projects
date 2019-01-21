using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ZZero.ZPlanner.ZConfiguration
{
    class ZStringConstants
    {
        public static string[] Alphabet = new string[] { "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z" };

        public static string[] LayerType = new string[] { LayerTypeCore, LayerTypePrepreg, LayerTypeSolderMask, LayerTypePlane, LayerTypeSignal, LayerTypeSplitMixed };
        public static string[] DielectricType = new string[] { LayerTypeCore, LayerTypePrepreg, LayerTypeSolderMask };
        public static string[] CopperType = new string[] { LayerTypePlane, LayerTypeSignal, LayerTypeSplitMixed };
        public static string DefaultDielectricMaterial = "Dielectric";
        public static string DefaultCopperMaterial = "Conductive";
        public static string EmptyMaterialID = "EmptyMaterialID";

        public const string ZplannerStackupDocument = "Z-planner Stackup Document";

        public const string ZplannerCRD = "Set your Z-planner credentials";

        public const string LayerTypeCore = "Core";
        public const string LayerTypePrepreg = "Prepreg";
        public const string LayerTypeSolderMask = "Solder Mask";
        public const string LayerTypeSignal = "Signal";
        public const string LayerTypeSplitMixed = "Split/Mixed";
        public const string LayerTypePlane = "Plane";
        public const string LayerTypePlaneGround = "Ground";
        public const string LayerTypePlanePower = "Power";
        public const string LayerTypeDummyCore = "DummyCore";
        public const string MaterialCategoryZZero = "Z-zero";
        public const string MaterialCategoryCorporate = "Corporate";
        public const string MaterialCategoryLocal = "Local";

        public static string[] PlaneReferenceType = new string[] { PlaneReferenceNA, PlaneReference, NonPlaneReference };
        public const string PlaneReference = "Reference";
        public const string NonPlaneReference = "Non-reference";
        public const string PlaneReferenceNA = "";

        public const string ClearFilter = "(Clear filter)";
        public const string eMailAccount = "zplannermail@gmail.com";

        public static string[] EditColumnIDs = new string[] { ParameterIDMaterial, ParameterIDCopperPercent, ParameterIDCopperThickness, ParameterIDThickness, ParameterIDZo_DielectricConstant, ParameterIDZdiff_DielectricConstant, ParameterIDZo_LossTangent, ParameterIDZdiff_LossTangent, ParameterIDZo_Frequency, ParameterIDZdiff_Frequency, ParameterIDZo_TraceWidth, ParameterIDZdiff_TraceWidth, ParameterIDZdiff_TraceSpacing };
        public static string[] BulkEditIDs = new string[] { DMLParameterIDMaterial, DMLParameterIDH, DMLParameterIDToleranceH, DMLParameterIDDk_1GHz, DMLParameterIDDk_3GHz, DMLParameterIDDk_5GHz, DMLParameterIDDk_7GHz, DMLParameterIDDk_10GHz, DMLParameterIDDk_20GHz };
        public static string[] MovedFroStackupColumnIDs = new string[] { ParameterIDZdiff_BitRate, ParameterIDZdiff_UnitInterval, ParameterIDZdiff_SkewTolerance, ParameterIDZdiff_WeavePitch, ParameterIDZdiff_FillPitch, ParameterIDZdiff_RoughTop, ParameterIDZdiff_RoughBottom };
        public static string DkIDsPrefix = "Dk_";

        public const string ListIDCopperThickness = "CopperThicknessList";
        public const string ListIDCopperFoil = "CopperFoilList";
        public const string ListIDViaSpanFilled = "ViaSpanFilledList";

        public const string ParameterIDViaSpan = "ViaSpan";
        public const string ParameterIDLayerNumber = "LayerNumber";
        public const string ParameterIDLayerType = "LayerType";
        public const string ParameterIDPlaneType = "PlaneType";
        public const string ParameterIDLayerTypeDummyCore = "LayerTypeDummyCore";
        public const string ParameterIDLayerName = "LayerName";
        public const string ParameterIDMaterial = "Material";
        public const string ParameterIDManufacturer = "Manufacturer";
        public const string ParameterIDConstruction = "Construction";
        public const string ParameterIDResinContent = "ResinContent";
        public const string ParameterIDCopperPercent = "CopperPercent";
        public const string ParameterIDCopperThickness = "CopperThickness";
        public const string ParameterIDThickness = "Thickness";
        public const string ParameterIDOriginThickness = "OriginThickness";
        public const string ParameterIDFabricatorThickness = "FabricatorThickness";
        public const string ParameterIDPrepregThickness = "PrepregThickness";
        public const string ParameterIDDielectricConstant = "DielectricConstant";
        public const string ParameterIDLossTangent = "LossTangent";
        public const string ParameterIDZo_DielectricConstant = "Zo_DielectricConstant";
        public const string ParameterIDZo_LossTangent = "Zo_LossTangent";
        public const string ParameterIDZo_Frequency = "Zo_Frequency";
        public const string ParameterIDZo_InsertionLoss = "Zo_InsertionLoss";
        public const string ParameterIDZo_TraceLength = "Zo_TraceLength";
        public const string ParameterIDZo_TotalLoss = "Zo_TotalLoss";
        public const string ParameterIDZo_LossViewer = "Zo_LossViewer";
        public const string ParameterIDZo_PropagationVelocity = "Zo_PropagationVelocity";
        public const string ParameterIDZo_PropagationDelay = "Zo_PropagationDelay";
        public const string ParameterIDZdiff_DielectricConstant = "Zdiff_DielectricConstant";
        public const string ParameterIDZdiff_LossTangent = "Zdiff_LossTangent";
        public const string ParameterIDZdiff_Frequency = "Zdiff_Frequency";
        public const string ParameterIDZdiff_InsertionLoss = "Zdiff_InsertionLoss";
        public const string ParameterIDZdiff_TraceLength = "Zdiff_TraceLength";
        public const string ParameterIDZdiff_TotalLoss = "Zdiff_TotalLoss";
        public const string ParameterIDZdiff_LossViewer = "Zdiff_LossViewer";
        public const string ParameterIDZdiff_PropagationVelocity = "Zdiff_PropagationVelocity";
        public const string ParameterIDZdiff_PropagationDelay = "Zdiff_PropagationDelay";
        public const string ParameterIDFillPitch = "FillPitch";
        public const string ParameterIDWeavePitch = "WeavePitch";
        public const string ParameterIDWarpYarnCount = "WarpYarnCount";
        public const string ParameterIDFillYarnCount = "FillYarnCount";

        public const string ParameterIDSingleEndedImpedanceZo = "SingleEndedImpedanceZo";
        public const string ParameterIDZo_Target = "Zo_TargetImpedance";
        public const string ParameterIDZo_TraceWidth = "Zo_TraceWidth";
        public const string ParameterIDZo_TraceSpacing = "Zo_TraceSpacing";
        public const string ParameterIDZo_Zo = "Zo_Zo";
        public const string ParameterIDZo_TopReference = "Zo_TopReference";
        public const string ParameterIDZo_BottomReference = "Zo_BottomReference";
        public const string ParameterIDZdiff_Target = "Zdiff_TargetImpedance";
        public const string ParameterIDZdiff_TopReference = "Zdiff_TopReference";
        public const string ParameterIDZdiff_BottomReference = "Zdiff_BottomReference";
        public const string ParameterIDDifferentialImpedanceZdiff = "DifferentialImpedanceZdiff";
        public const string ParameterIDZdiff_TraceWidth = "Zdiff_TraceWidth";
        public const string ParameterIDZdiff_TraceSpacing = "Zdiff_TraceSpacing";
        public const string ParameterIDZdiff_TracePitch = "Zdiff_TracePitch";
        public const string ParameterIDZdiff_FillPitch = "Zdiff_FillPitch";
        public const string ParameterIDZdiff_WeavePitch = "Zdiff_WeavePitch";
        public const string ParameterIDZdiff_Zo = "Zdiff_Zo";
        public const string ParameterIDZdiff_Zdiff = "Zdiff_Zdiff";
        public const string ParameterIDZo_IsUsed = "Zo_IsUsed";
        public const string ParameterIDZdiff_IsUsed = "Zdiff_IsUsed";
        public const string ParameterIDZdiff_BitRate = "Zdiff_BitRate";
        public const string ParameterIDZdiff_UnitInterval = "Zdiff_UnitInterval";
        public const string ParameterIDZdiff_SkewTolerance = "Zdiff_SkewTolerance";

        public const string ParameterIDThermalConductivity = "ThermalConductivity";
        public const string ParameterIDPlating = "Plating";
        public const string ParameterIDBulkRes = "BulkRes";
        public const string ParameterIDThermalCoef = "ThermalCoef";
        public const string ParameterIDFoilTreatment = "FoilTreatment";
        public const string ParameterIDRoughTop = "RoughTop";
        public const string ParameterIDRoughBottom = "RoughBottom";
        public const string ParameterIDZdiff_RoughTop = "Zdiff_RoughTop";
        public const string ParameterIDZdiff_RoughBottom = "Zdiff_RoughBottom";
        public const string ParameterIDCalcRoughTop = "CalculatedRoughTop";
        public const string ParameterIDCalcRoughBottom = "CalculatedRoughBottom";
        public const string ParameterIDEtchFactor = "EtchFactor";
        public const string ParameterIDNarrowTop = "NarrowTop";
        public const string ParameterIDConformal = "Conformal";
        public const string ParameterIDAtchMetal = "AtchMetal";
        public const string ParameterIDDescription = "Description";
        public const string ParameterIDPlaneReference = "PlaneReference";
        public const string ParameterIDComments = "Comments";
        public const string ParameterIDSequentialLamination = "SequentialLamination";

        //tapestry specific
        public const string ParameterIDPlaneVoltage = "PlaneVoltage";

        public const string DMLParameterIDCategory = "Category";
        public const string DMLParameterIDQualified  = "Qualified";
        public const string DMLParameterIDManufacturer  = "Manufacturer";
        public const string DMLParameterIDMaterial  = "Material";
        public const string DMLParameterIDType  = "Type";
        public const string DMLParameterIDConstruction = "Construction";
        public const string DMLParameterIDNormalizedConstruction = "NConstruction";
        public const string DMLParameterIDResin  = "Resin";
        public const string DMLParameterIDH = "H";
        public const string DMLParameterIDDk = "Dk";
        public const string DMLParameterIDDf = "Df";
        public const string DMLParameterIDTg  = "Tg";
        public const string DMLParameterIDTd  = "Td";
        public const string DMLParameterIDHalogenFree  = "HalogenFree";
        public const string DMLParameterIDPrimaryIPCSlashSheet  = "PrimaryIPCSlashSheet";
        public const string DMLParameterIDZCTE  = "z-CTE";
        public const string DMLParameterIDZCTE_alpha1  = "z-CTE_alpha1";
        public const string DMLParameterIDZCTE_alpha2  = "z-CTE_alpha2";
        public const string DMLParameterIDXYCTE = "XY-CTE";
        public const string DMLParameterIDToleranceH  = "ToleranceH";
        public const string DMLParameterIDDf_01GHz = "Df_0.1GHz";
        public const string DMLParameterIDDk_01GHz = "Dk_0.1GHz";
        public const string DMLParameterIDDf_05GHz = "Df_0.5GHz";
        public const string DMLParameterIDDk_05GHz = "Dk_0.5GHz";
        public const string DMLParameterIDDk_1GHz = "Dk_1GHz";
        public const string DMLParameterIDDf_1GHz = "Df_1GHz";
        public const string DMLParameterIDDk_2GHz = "Dk_2GHz";
        public const string DMLParameterIDDf_2GHz = "Df_2GHz";
        public const string DMLParameterIDDk_3GHz = "Dk_3GHz";
        public const string DMLParameterIDDf_3GHz = "Df_3GHz";
        public const string DMLParameterIDDk_4GHz = "Dk_4GHz";
        public const string DMLParameterIDDf_4GHz = "Df_4GHz";
        public const string DMLParameterIDDk_5GHz = "Dk_5GHz";
        public const string DMLParameterIDDf_5GHz = "Df_5GHz";
        public const string DMLParameterIDDk_6GHz = "Dk_6GHz";
        public const string DMLParameterIDDf_6GHz = "Df_6GHz";
        public const string DMLParameterIDDk_7GHz = "Dk_7GHz";
        public const string DMLParameterIDDf_7GHz = "Df_7GHz";
        public const string DMLParameterIDDk_8GHz = "Dk_8GHz";
        public const string DMLParameterIDDf_8GHz = "Df_8GHz";
        public const string DMLParameterIDDk_9GHz = "Dk_9GHz";
        public const string DMLParameterIDDf_9GHz = "Df_9GHz";
        public const string DMLParameterIDDk_10GHz = "Dk_10GHz";
        public const string DMLParameterIDDf_10GHz = "Df_10GHz";
        public const string DMLParameterIDDk_12GHz = "Dk_12GHz";
        public const string DMLParameterIDDf_12GHz = "Df_12GHz";
        public const string DMLParameterIDDk_15GHz = "Dk_15GHz";
        public const string DMLParameterIDDf_15GHz = "Df_15GHz";
        public const string DMLParameterIDDk_18GHz = "Dk_18GHz";
        public const string DMLParameterIDDf_18GHz = "Df_18GHz";
        public const string DMLParameterIDDk_20GHz = "Dk_20GHz";
        public const string DMLParameterIDDf_20GHz = "Df_20GHz";
        public const string DMLParameterIDDk_23GHz = "Dk_23GHz";
        public const string DMLParameterIDDf_23GHz = "Df_23GHz";
        public const string DMLParameterIDDk_29GHz = "Dk_29GHz";
        public const string DMLParameterIDDf_29GHz = "Df_29GHz";
        public const string DMLParameterIDDk_34GHz = "Dk_34GHz";
        public const string DMLParameterIDDf_34GHz = "Df_34GHz";
        public const string DMLParameterIDHide  = "Hide";
        public const string DMLParameterIDCost  = "Cost";
        public const string DMLParameterIDDfType = "Df_Type";
        public const string DMLParameterIDTgType = "Tg_Type";
        public const string DMLParameterIDT  = "T";
        public const string DMLParameterIDT260  = "T260";
        public const string DMLParameterIDT280 = "T280";
        public const string DMLParameterIDT288 = "T288";
        public const string DMLParameterIDPeelStrength = "PeelStrength";
        public const string DMLParameterIDWA  = "WA";
        public const string DMLParameterIDDatasheetURL  = "DatasheetURL";
        public const string DMLParameterIDThermalConductivity  = "ThermalConductivity";
        public const string DMLParameterIDCAFResistant  = "CAFResistant";
        public const string DMLParameterIDLaserDrillable  = "LaserDrillable";
        public const string DMLParameterIDYoungsModulus  = "Young'sModulus";
        public const string DMLParameterIDPoissonsRatio  = "Poisson'sRatio";
        public const string DMLParameterIDWarpYarnCount  = "WarpYarnCount";
        public const string DMLParameterIDFillYarnCount  = "FillYarnCount";
        public const string DMLParameterIDWeavePitch = "WeavePitch";
        public const string DMLParameterIDFillPitch = "FillPitch";
        public const string DMLParameterIDSquareWeave  = "SquareWeave";
        public const string DMLParameterIDLeadTime  = "LeadTime";
        public const string DMLParameterIDDescription  = "Description";
        public const string DMLParameterIDComments  = "Comments";

        public const string SelectListIDSlashSheet = "PrimaryIPCSlashSheet";

        public const string CategoryStackupParameters = " Stackup";
        public const string CategoryParameterParameters = " Column";
        public const string CategoryLayerParameters = "Stackup Row";
        public const string CategorySpanParameters = " Via Span";
        public const string CategoryImpedanceParameters = " Signal";
        public const string CategoryCollectionParameters = "Collecton";
        public const string CategorySingleParameters = "Single-Ended Signal";
        public const string CategoryPairParameters = "Differential Signal";

        public const string ClipbordKeyStackupLayers = "Stackup Layers";
        public const string ClipbordKeySingleLayers = "Single Layers";
        public const string ClipbordKeyPairLayers = "Pair Layers ";
        public const string ClipbordKeyMaterials = "Materials";
        public const string ClipbordKeyMaterialsForStackup = "Materials"; //"Materials For Stackup";
        public const string ClipbordKeyStackupLayerParameters = "Stackup Layer Parameters";

        public const string NoResultsFoundMessage = "Search text not found.";
        public const string EndOfLibraryMessage = "The end of the library has been reached.";
        public const string FoundRowAndColumnAreHiddenMessage = "Search text found. Turn off filter(s) and insert the hidden column ({0}). Then search again to see this search result.";
        public const string FoundRowIsHiddenMessage = "Search text found. Turn off filter(s) and then search again to see this search result.";
        public const string FoundColumnIsHiddenMessage = "Search text found. Insert the hidden column ({0}) and then search again to see this search result.";

        public const string DateTimePattern = "MMM dd, yyyy HH:mm:ss zzz";

        public static Dictionary<string, string> TitleDictionary = new Dictionary<string, string>{
            {"ID","ID"},    
            {"Title","Title"},
            {"RowTitle","Row"},
            {"SpanTitle","Via Span"},
            {"SingleTitle","Single-Ended Signal"},
            {"PairTitle","Differential Signal"},
            {"StackupTitle","Stackup"},
            {"ParameterTitle","Column"},
            {"Size","Hole Diameter (mils)"},
            {"FirstLayer","Top Layer"},
            {"LastLayer","Bottom Layer"},
            {"IsHidden","Is Hidden"},
            {"IsFrozen", "Is Frozen"},
            {"Height","Height"},
            {"Order","Order"},
            {"Collection", "Collection"},
            {"Layers", "Rows"},
            {"Stackups", "Stackups"},
            {"Singles", "Singles"},
            {"Pairs", "Pairs"},
            {"Spans", "Via Spans"},
            {"Frequency", "Frequency (GHz)"},
            {"Description", "Description"},
            {"DefaultValue", "Default Value"},
            {"BoardThicknessEnglish", "Board Thickness (mils)"},
            {"BoardThicknessMetric", "Board Thickness (mm)"},
            {"AspectRatio", "Aspect Ratio"},
            {"Enable", "Enable"},
            {"Filled", "Fill"},
            {"BackDrilled", "Back Drilled"},
            {"ImpedanceTarget", "Impedance Target (ohms)"}
        };

        public static Dictionary<string, string> DescriptionDictionary = new Dictionary<string, string>{
            {"ID","Defines the identifier."},
            {"Title","Defines the dispalyed caption."},
            {"Size","Defines span diameter."},
            {"FirstLayer","Defines first layer of the span."},
            {"LastLayer","Defines last layer of the span."},
            {"IsHidden","Defines whether the corresponding row in the grid is hidden."},
            {"IsFrozen", "Defines whether the corresponding row in the grid is frozen."},
            {"Height","Defines the height of corresponding row in the grid."},
            {"Order","Defines the order of corresponding row in the grid."},
            {"Frequency","Defines the work frequency for of stackup."},
            {"Description","Defines the Description of the column."},
            {"DefaultValue","Defines the default value for the column."},
            {"BoardThicknessEnglish", "Total Board Thickness"},
            {"BoardThicknessMetric", "Total Board Thickness"},
            {"AspectRatio", "Defines the aspect ratio of Via Span"},
            {"Enable", "Defines whether the Via Span is enabled"},
            {"Filled", "Defines whether the Via Span is filled"},
            {"BackDrilled", "Defines whether the Via Span is back drilled"},
            {"ImpedanceTarget", "Defines the impedance target."}
        };

    }
}
