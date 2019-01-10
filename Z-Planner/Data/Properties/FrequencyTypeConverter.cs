using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.Data.Properties
{
    class FrequencyTypeConverter : DoubleConverter
    {
        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(string))
                return true;
            return base.CanConvertTo(context, destinationType);
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string))
            {
                return true;
            }
            return base.CanConvertFrom(context, sourceType);
        }

        public override object ConvertTo(ITypeDescriptorContext context,
        CultureInfo culture, object value, Type destinationType)
        {
            if ((destinationType == typeof(string)))
            {
                if (value == null) return string.Empty;
                if (value is double)
                {
                    double frequency = double.NaN;
                    ZLayerParameter layerParameter = (context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyObject as ZLayerParameter;
                    if (layerParameter != null)
                    {
                        ZLayerType? layerType = layerParameter.Layer.GetLayerType();
                        if (layerType == ZLayerType.Signal || layerType == ZLayerType.SplitMixed)
                        {
                            string isUsed = layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZdiff_IsUsed) ?? layerParameter.Layer.GetLayerParameterValue(ZStringConstants.ParameterIDZo_IsUsed);
                            if (isUsed == null || isUsed.ToLower() == "true")
                            {
                                frequency = layerParameter.Layer.GetFrequency();
                            }
                        }
                    }

                    if (double.IsNaN(frequency)) return string.Empty;

                    return frequency.ToString((context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyFormat, CultureInfo.InvariantCulture);
                }
            }

            return base.ConvertTo(context, CultureInfo.InvariantCulture, value, destinationType);
        }

        public override object ConvertFrom(ITypeDescriptorContext context,
            CultureInfo culture, object value)
        {
            if (value is string)
            {
                double num;
                if (value as string == string.Empty) return double.NaN;
                NumberStyles style = NumberStyles.AllowDecimalPoint;
                // AllowNegative was not implemented.
                //if (((context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyObject as ZLayerParameter).Parameter.IsAllowNegative) style |= NumberStyles.AllowLeadingSign;
                if (double.TryParse(value as string, style, CultureInfo.InvariantCulture, out num)) return num;
            }

            try
            {
                return base.ConvertFrom(context, CultureInfo.InvariantCulture, value);
            }
            catch (Exception)
            {
                // Process the exception 
                return context.PropertyDescriptor.GetValue(null);//context.Instance.
            }
        }
    }
}
