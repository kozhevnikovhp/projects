using System;
using System.Collections;
using System.Collections.Generic;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Data.Properties
{
    class ZDynamicComponentManager
    {
        internal static ZDynamicCollection GetDynamicCollection(ZPlannerProject project, IList collection)
        {
            if (collection != null)
            {
                ZList<ZLayer> rows = collection as ZList<ZLayer>;
                if (rows != null)
                {
                    return new ZDynamicCollection(project, project.Stackup, rows, typeof(ZLayer), ZStringConstants.TitleDictionary["Layers"], "Add, Edit or Delete collection items.");
                }

                ZList<ZStackup> stackups = collection as ZList<ZStackup>;
                if (stackups != null)
                {
                    return new ZDynamicCollection(project, project.Stackup, stackups, typeof(ZStackup), ZStringConstants.TitleDictionary["Stackups"], "Add, Edit or Delete collection items.");
                }

                ZList<ZSingle> singles = collection as ZList<ZSingle>;
                if (singles != null)
                {
                    return new ZDynamicCollection(project, project.Stackup, singles, typeof(ZSingle), ZStringConstants.TitleDictionary["Singles"], "Add, Edit or Delete collection items.");
                }

                ZList<ZPair> pairs = collection as ZList<ZPair>;
                if (pairs != null)
                {
                    return new ZDynamicCollection(project, project.Stackup, pairs, typeof(ZPair), ZStringConstants.TitleDictionary["Pairs"], "Add, Edit or Delete collection items.");
                }

                ZList<ZSpan> spans = collection as ZList<ZSpan>;
                if (spans != null)
                {
                    return new ZDynamicCollection(project, project.Stackup, spans, typeof(ZSpan), ZStringConstants.TitleDictionary["Spans"], "Add, Edit or Delete collection items.");
                }
            }

            return new ZDynamicCollection(project, project.Stackup, null, typeof(object), string.Empty, string.Empty);
        }

        internal static ZDynamicComponent GetDynamicComponent(ZParameter parameter)
        {
            ZDynamicComponent component = new ZDynamicComponent(parameter);
            component.AddProperty(parameter, ZStringConstants.TitleDictionary["ParameterTitle"], parameter.Title, ZStringConstants.DescriptionDictionary["Title"], ZStringConstants.CategoryParameterParameters, typeof(string), "", null, false, false, null, null);
            component.AddProperty(parameter, ZStringConstants.TitleDictionary["Description"], parameter.Description, ZStringConstants.DescriptionDictionary["Description"], ZStringConstants.CategoryParameterParameters, typeof(string), "", null, false, false, null, null);
            //component.AddProperty(parameter, ZStringConstants.TitleDictionary["DefaultValue"], parameter.DefaultValue, ZStringConstants.DescriptionDictionary["DefaultValue"], ZStringConstants.CategoryParameterParameters, typeof(string), null, false, false, null, null);
            return component;
        }

        internal static ZDynamicComponent GetDynamicComponent(ZStackup stackup)
        {
            //board thickness
            string format = "{0:N" + Options.TheOptions.lengthDigits.ToString() +"}";
            string thick = String.Format(format, stackup.GetBoardThickness());

            ZDynamicComponent component = new ZDynamicComponent(stackup);
            component.AddProperty(stackup, ZStringConstants.TitleDictionary["StackupTitle"], stackup.Title, ZStringConstants.DescriptionDictionary["Title"], ZStringConstants.CategoryStackupParameters, typeof(string), "", null, false, false, null, null);
            component.AddProperty(stackup, ZStringConstants.TitleDictionary["Frequency"], stackup.Frequency, ZStringConstants.DescriptionDictionary["Frequency"], ZStringConstants.CategoryStackupParameters, typeof(double), "N" + Settings.Options.TheOptions.frequencyDigits, null, false, false, null, null);
            component.AddProperty(stackup, ZStringConstants.TitleDictionary["BoardThickness"], thick, ZStringConstants.DescriptionDictionary["BoardThickness"], ZStringConstants.CategoryStackupParameters, typeof(string), "", null, true, false, null, null);
            return component;
        }

        internal static ZDynamicComponent GetDynamicComponent(ZLayer layer)
        {
            ZDynamicComponent component = new ZDynamicComponent(layer);
            component.AddProperty(layer, ZStringConstants.TitleDictionary["RowTitle"], layer.Title, ZStringConstants.DescriptionDictionary["Title"], "1 " + ZStringConstants.CategoryLayerParameters, typeof(string), "", null, false, false, null, null);

            foreach (ZLayerParameter layerParameter in layer.LayerParameters.OrderBy(X => X.Parameter.Order))
            {
                if (layerParameter.Parameter.IsPrivate || layerParameter.Parameter.IsHidden) continue;
                ICollection listCollection = (layerParameter.Parameter.List != null) ? layerParameter.Parameter.List.GetValues() : null;
                if (layerParameter.ID == ZStringConstants.ParameterIDLayerType) listCollection = ZStringConstants.LayerType;
                Type converterType = null;
                Type editorType = null;
                if (listCollection != null) converterType = typeof(ListTypeConverter);
                if (layerParameter.Parameter.ValueType == ZValueType.Link) continue;
                if (layerParameter.Parameter.ValueType == ZValueType.Number) converterType = typeof(DoubleTypeConverter);
                // This can be used in V2
                if (layerParameter.Parameter.ValueType == ZValueType.Table) { converterType = typeof(TableTypeConverter); /*editorType = typeof(TableTypeEditor);*/ }
                if (layerParameter.Parameter.ValueType == ZValueType.Percent) converterType = typeof(PercentTypeConverter);
                bool isReadOnly = layerParameter.IsReadOnly();
                component.AddProperty(layerParameter, layerParameter.Parameter.Title, GetParameterValue(layerParameter.Value, layerParameter.Parameter.ValueType), layerParameter.Parameter.Description, "1 " + ZStringConstants.CategoryLayerParameters, GetParameterType(layerParameter.Parameter.ValueType), ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), listCollection, isReadOnly, false, editorType, converterType);
            }

            ZSingle single = layer.Stackup.ActiveSingle;
            if (single != null)
            {
                ZLayer singleLayer = layer.Stackup.GetLayerOfSingleImpedance(layer.ID, single.ID);
                if (singleLayer != null)
                {
                    foreach (ZLayerParameter layerParameter in singleLayer.LayerParameters.OrderBy(X => X.Parameter.Order))
                    {
                        if (layerParameter.Parameter.IsPrivate || layerParameter.Parameter.IsHidden) continue;
                        ICollection listCollection = (layerParameter.Parameter.List != null) ? layerParameter.Parameter.List.GetValues() : null;
                        if (layerParameter.ID == ZStringConstants.ParameterIDLayerType) listCollection = ZStringConstants.LayerType;
                        Type converterType = null;
                        Type editorType = null;
                        if (listCollection != null) converterType = typeof(ListTypeConverter);
                        if (layerParameter.Parameter.ValueType == ZValueType.Link) continue;
                        if (layerParameter.Parameter.ValueType == ZValueType.Number) converterType = typeof(DoubleTypeConverter);
                        // This can be used in V2
                        if (layerParameter.Parameter.ValueType == ZValueType.Table) { converterType = typeof(TableTypeConverter); /*editorType = typeof(TableTypeEditor);*/ }
                        if (layerParameter.Parameter.ValueType == ZValueType.Percent) converterType = typeof(PercentTypeConverter);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZo_Frequency) converterType = typeof(FrequencyTypeConverter);
                        bool isReadOnly = (layerParameter.ID == ZStringConstants.ParameterIDZo_TopReference ||
                            layerParameter.ID == ZStringConstants.ParameterIDZo_BottomReference) ? true : layerParameter.IsReadOnly();
                        component.AddProperty(layerParameter, layerParameter.Parameter.Title, GetParameterValue(layerParameter.Value, layerParameter.Parameter.ValueType), layerParameter.Parameter.Description, "2 " + ZStringConstants.CategorySingleParameters, GetParameterType(layerParameter.Parameter.ValueType), ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), listCollection, isReadOnly, false, editorType, converterType);
                    }
                }
            }

            ZPair pair = layer.Stackup.ActivePair;
            if (pair != null)
            {
                ZLayer pairLayer = layer.Stackup.GetLayerOfPairImpedance(layer.ID, pair.ID);
                if (pairLayer != null)
                {
                    foreach (ZLayerParameter layerParameter in pairLayer.LayerParameters.OrderBy(X => X.Parameter.Order))
                    {
                        string value = layerParameter.Value;
                        if (layerParameter.Parameter.IsPrivate || layerParameter.Parameter.IsHidden) continue;
                        ICollection listCollection = (layerParameter.Parameter.List != null) ? layerParameter.Parameter.List.GetValues() : null;
                        if (layerParameter.ID == ZStringConstants.ParameterIDLayerType) listCollection = ZStringConstants.LayerType;
                        Type converterType = null;
                        Type editorType = null;
                        if (listCollection != null) converterType = typeof(ListTypeConverter);
                        if (layerParameter.Parameter.ValueType == ZValueType.Link) continue;
                        if (layerParameter.Parameter.ValueType == ZValueType.Number) converterType = typeof(DoubleTypeConverter);
                        // This can be used in V2
                        if (layerParameter.Parameter.ValueType == ZValueType.Table) { converterType = typeof(TableTypeConverter); /*editorType = typeof(TableTypeEditor);*/ }
                        if (layerParameter.Parameter.ValueType == ZValueType.Percent) converterType = typeof(PercentTypeConverter);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_Frequency) converterType = typeof(FrequencyTypeConverter);
                        if (ZStringConstants.MovedFroStackupColumnIDs.Contains(layerParameter.ID)) value = layer.Stackup.GetMovedFromStackupLayerParameterValue(layerParameter);
                        bool isReadOnly = (layerParameter.ID == ZStringConstants.ParameterIDZdiff_TopReference ||
                            layerParameter.ID == ZStringConstants.ParameterIDZdiff_BottomReference) ? true : layerParameter.IsReadOnly();
                        component.AddProperty(layerParameter, layerParameter.Parameter.Title, GetParameterValue(value, layerParameter.Parameter.ValueType), layerParameter.Parameter.Description, "3 " + ZStringConstants.CategoryPairParameters, GetParameterType(layerParameter.Parameter.ValueType), ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), listCollection, isReadOnly, false, editorType, converterType);
                    }
                }
            }

            //component.AddProperty(ZStringConstants.TitleDictionary["IsHidden"], layer.IsHidden, ZStringConstants.DescriptionDictionary["IsHidden"], ZStringConstants.CategoryLayerParameters, typeof(bool), false, false);
            //component.AddProperty(ZStringConstants.TitleDictionary["IsFrozen"], layer.IsFrozen, ZStringConstants.DescriptionDictionary["IsFrozen"], ZStringConstants.CategoryLayerParameters, typeof(bool), false, false);
            //component.AddProperty(ZStringConstants.TitleDictionary["Height"], layer.Height, ZStringConstants.DescriptionDictionary["Height"], ZStringConstants.CategoryLayerParameters, typeof(int), false, false);
            //component.AddProperty(ZStringConstants.TitleDictionary["Order"], layer.Order, ZStringConstants.DescriptionDictionary["Order"], ZStringConstants.CategoryLayerParameters, typeof(int), false, false);
            return component;
        }

        internal static ZDynamicComponent GetDynamicComponent(ZSpan span)
        {
            ZDynamicComponent component = new ZDynamicComponent(span);
            List<ZLayer> layerCollection = new List<ZLayer>();
            foreach(ZLayer layer in span.Stackup.Layers)
            {
                ZLayerType? layerType = layer.GetLayerType();
                if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                        layerCollection.Add(layer);
            }

            SelectList selectList = span.Stackup.Project.Lists.Find(x => x.ID == "ViaSpanFilledList");
            List<string> filledCollection = (selectList != null) ? selectList.GetValues() : new List<string>();

            Type converterType = typeof(ZLayerTypeConverter);
            Type aspectRatioConverterType = typeof(AspectRatioTypeConverter);
            Type fillConverterType = typeof(ViaSpanFillTypeConverter);

            component.AddProperty(span, ZStringConstants.TitleDictionary["SpanTitle"], span.Title, ZStringConstants.DescriptionDictionary["Title"], ZStringConstants.CategorySpanParameters, typeof(string), "", null, false, false, null, null);
            component.AddProperty(span, ZStringConstants.TitleDictionary["Enable"], span.IsEnable, ZStringConstants.DescriptionDictionary["Enable"], ZStringConstants.CategorySpanParameters, typeof(bool), "", null, false, false, null, null);
            component.AddProperty(span, ZStringConstants.TitleDictionary["FirstLayer"], span.FirstLayer, ZStringConstants.DescriptionDictionary["FirstLayer"], ZStringConstants.CategorySpanParameters, typeof(ZLayer), "", layerCollection, false, false, null, converterType);
            component.AddProperty(span, ZStringConstants.TitleDictionary["LastLayer"], span.LastLayer, ZStringConstants.DescriptionDictionary["LastLayer"], ZStringConstants.CategorySpanParameters, typeof(ZLayer), "", layerCollection, false, false, null, converterType);
            component.AddProperty(span, ZStringConstants.TitleDictionary["Size"], span.Size, ZStringConstants.DescriptionDictionary["Size"], ZStringConstants.CategorySpanParameters, typeof(double), "N" + Settings.Options.TheOptions.lengthDigits, null, false, false, null, typeof(DoubleTypeConverter));
            component.AddProperty(span, ZStringConstants.TitleDictionary["AspectRatio"], span.AspectRatio, ZStringConstants.DescriptionDictionary["AspectRatio"], ZStringConstants.CategorySpanParameters, typeof(double), "N" + Settings.Options.TheOptions.percentDigits, null, true, false, null, aspectRatioConverterType);
            component.AddProperty(span, ZStringConstants.TitleDictionary["Filled"], span.Filled, ZStringConstants.DescriptionDictionary["Filled"], ZStringConstants.CategorySpanParameters, typeof(string), "", filledCollection, false, false, null, fillConverterType);
            component.AddProperty(span, ZStringConstants.TitleDictionary["BackDrilled"], span.IsBackDrilled, ZStringConstants.DescriptionDictionary["BackDrilled"], ZStringConstants.CategorySpanParameters, typeof(bool), "", null, false, false, null, null);
            return component;
        }

        internal static ZDynamicComponent GetDynamicComponent(ZSingle single)
        {
            ZDynamicComponent component = new ZDynamicComponent(single);
            component.AddProperty(single, ZStringConstants.TitleDictionary["SingleTitle"], single.Title, ZStringConstants.DescriptionDictionary["Title"], ZStringConstants.CategoryImpedanceParameters, typeof(string), "", null, false, false, null, null);

            foreach (ZLayer layer in single.Layers)
            {
                ZLayerType? layerType = layer.GetLayerType();
                if (layerType == ZLayerType.Plane || layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                {
                    foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                    {
                        if (layerParameter.Parameter.IsPrivate || layerParameter.Parameter.IsHidden) continue;
                        ICollection listCollection = (layerParameter.Parameter.List != null) ? layerParameter.Parameter.List.GetValues() : null;
                        Type converterType = null;
                        Type editorType = null;
                        if (listCollection != null) converterType = typeof(ListTypeConverter);
                        if (layerParameter.Parameter.ValueType == ZValueType.Link) continue;
                        if (layerParameter.Parameter.ValueType == ZValueType.Number) converterType = typeof(DoubleTypeConverter);
                        // This can be used in V2
                        if (layerParameter.Parameter.ValueType == ZValueType.Table) { converterType = typeof(TableTypeConverter); /*editorType = typeof(TableTypeEditor);*/ }
                        if (layerParameter.Parameter.ValueType == ZValueType.Percent) converterType = typeof(PercentTypeConverter);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZo_Frequency) converterType = typeof(FrequencyTypeConverter);
                        ZLayer stackupLayer = single.Stackup.Layers.Find((ZLayer x) => x.ID == layer.ID);
                        string category = (stackupLayer != null) ? stackupLayer.Title : layer.ID;
                        component.AddProperty(layerParameter, layerParameter.Parameter.Title, GetParameterValue(layerParameter.Value, layerParameter.Parameter.ValueType), layerParameter.Parameter.Description, category, GetParameterType(layerParameter.Parameter.ValueType), ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), listCollection, layerParameter.IsReadOnly(), false, editorType, converterType);
                    }
                }
            }

            return component;
        }

        internal static ZDynamicComponent GetDynamicComponent(ZPair pair)
        {
            ZDynamicComponent component = new ZDynamicComponent(pair);
            component.AddProperty(pair, ZStringConstants.TitleDictionary["PairTitle"], pair.Title, ZStringConstants.DescriptionDictionary["Title"], ZStringConstants.CategoryImpedanceParameters, typeof(string), "", null, false, false, null, null);

            foreach (ZLayer layer in pair.Layers)
            {
                ZLayerType? layerType = layer.GetLayerType();
                if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                {
                    foreach (ZLayerParameter layerParameter in layer.LayerParameters)
                    {
                        string value = layerParameter.Value;
                        if (layerParameter.Parameter.IsPrivate || layerParameter.Parameter.IsHidden) continue;
                        ICollection listCollection = (layerParameter.Parameter.List != null) ? layerParameter.Parameter.List.GetValues() : null;
                        Type converterType = null;
                        Type editorType = null;
                        if (listCollection != null) converterType = typeof(ListTypeConverter);
                        if (layerParameter.Parameter.ValueType == ZValueType.Link) continue;
                        if (layerParameter.Parameter.ValueType == ZValueType.Number) converterType = typeof(DoubleTypeConverter);
                        // This can be used in V2
                        if (layerParameter.Parameter.ValueType == ZValueType.Table) { converterType = typeof(TableTypeConverter); /*editorType = typeof(TableTypeEditor);*/ }
                        if (layerParameter.Parameter.ValueType == ZValueType.Percent) converterType = typeof(PercentTypeConverter);
                        if (layerParameter.ID == ZStringConstants.ParameterIDZdiff_Frequency) converterType = typeof(FrequencyTypeConverter);
                        if (ZStringConstants.MovedFroStackupColumnIDs.Contains(layerParameter.ID)) value = layer.Stackup.GetMovedFromStackupLayerParameterValue(layerParameter);
                        ZLayer stackupLayer = pair.Stackup.Layers.Find((ZLayer x) => x.ID == layer.ID);
                        string category = (stackupLayer != null) ? stackupLayer.Title : layer.ID;
                        component.AddProperty(layerParameter, layerParameter.Parameter.Title, GetParameterValue(value, layerParameter.Parameter.ValueType), layerParameter.Parameter.Description, category, GetParameterType(layerParameter.Parameter.ValueType), ZPlannerManager.GetFormatByParameter(layerParameter.Parameter), listCollection, layerParameter.IsReadOnly(), false, editorType, converterType);
                    }
                }
            }

            return component;
        }

        private static Type GetParameterType(ZValueType valueType)
        {
            switch(valueType)
            {
                case ZValueType.Boolean:
                    return typeof(bool);
                case ZValueType.Number:
                case ZValueType.Percent:
                    return typeof(double);
                case ZValueType.Select:
                case ZValueType.Link:
                case ZValueType.Text:
                case ZValueType.Table:
                    return typeof(string);
                default:
                    return typeof(string);
            }
        }

        private static object GetParameterValue(string stringValue, ZValueType valueType)
        {
            switch (valueType)
            {
                case ZValueType.Boolean:
                    bool boolValue;
                    if (!string.IsNullOrEmpty(stringValue) && bool.TryParse(stringValue, out boolValue)) return boolValue;
                    return false;
                case ZValueType.Number:
                    double doubleValue;
                    if (!string.IsNullOrEmpty(stringValue) && double.TryParse(stringValue, NumberStyles.Any, CultureInfo.InvariantCulture, out doubleValue)) return doubleValue;
                    return double.NaN;
                case ZValueType.Table:
                    return stringValue;
                case ZValueType.Percent:
                    if (!string.IsNullOrEmpty(stringValue) && double.TryParse(stringValue.Replace("%", string.Empty).Trim(), NumberStyles.Any, CultureInfo.InvariantCulture, out doubleValue)) return doubleValue;
                    return double.NaN;
                case ZValueType.Select:
                    return stringValue;
                case ZValueType.Link:
                    return stringValue;
                case ZValueType.Text:
                    return stringValue;
                default:
                    return stringValue;
            }
        }
    }
}
