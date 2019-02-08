using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.ZConfiguration
{
    class ZColorConstants
    {
        public static Color ViaSpanGradientOuterColor = Color.FromArgb(229, 120, 31);
        public static Color ViaSpanGradientInnerColor = Color.FromArgb(251, 209, 173);

        public static Color MaterialMapperHighlightColor = Color.FromArgb(250, 200, 110);

        public static Color CoreLayerColor = Color.FromArgb(200, 230, 240);
        public static Color PrepregLayerColor = Color.FromArgb(234, 242, 220);
        public static Color SolderMaskLayerColor = Color.FromArgb(195, 212, 157);
        public static Color PlaneLayerColor = Color.FromArgb(255, 212, 181);
        public static Color SignalLayerColor = Color.FromArgb(255, 255, 255);
        public static Color SplitMixedLayerColor = Color.FromArgb(255, 255, 255);

        public static Color CenterLineColor = Color.FromArgb(255, 0, 0);

        public static Color DiffToolLayerColor = Color.FromArgb(128, 128, 128);
        public static Color DiffToolMatchColor = Color.FromArgb(0, 128, 0);
        public static Color DiffToolNoMatchColor = Color.FromArgb(255, 0, 0);
        public static Color DiffToolMissingColor = Color.FromArgb(196, 196, 196);

        //public static Color DisabledCellColor = Color.FromArgb(160, 160, 160);
        public static Color DisabledCellColor = SystemColors.Control;
        public static Color CalculatedCellColor = Color.FromArgb(0, 0, 0);

        public static Color ActiveHeaderColor = Color.FromArgb(255, 0, 0);

        public static Color LocalLibraryColor = Color.FromArgb(255, 0, 0);

        internal static Color GetLayerColor(string layerTypeString)
        {
            switch (layerTypeString)
            {
                case ZStringConstants.LayerTypeCore:
                    return ZColorConstants.CoreLayerColor;
                case ZStringConstants.LayerTypePrepreg:
                    return ZColorConstants.PrepregLayerColor;
                case ZStringConstants.LayerTypeSolderMask:
                    return ZColorConstants.SolderMaskLayerColor;
                case ZStringConstants.LayerTypePlane:
                    return ZColorConstants.PlaneLayerColor;
                case ZStringConstants.LayerTypeSignal:
                    return ZColorConstants.SignalLayerColor;
                case ZStringConstants.LayerTypeSplitMixed:
                    return ZColorConstants.SplitMixedLayerColor;
                default:
                    return Color.White;
            }
        }

        internal static Color GetLayerColor(ZLayerType? layerType)
        {
            switch (layerType)
            {
                case ZLayerType.Core:
                    return ZColorConstants.CoreLayerColor;
                case ZLayerType.Prepreg:
                    return ZColorConstants.PrepregLayerColor;
                case ZLayerType.SolderMask:
                    return ZColorConstants.SolderMaskLayerColor;
                case ZLayerType.Plane:
                    return ZColorConstants.PlaneLayerColor;
                case ZLayerType.Signal:
                    return ZColorConstants.SignalLayerColor;
                case ZLayerType.SplitMixed:
                    return ZColorConstants.SplitMixedLayerColor;
                default: 
                    return Color.White;
            }
        }
    }
}
