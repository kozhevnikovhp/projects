using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    class TableTypeConverter : StringConverter
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
            string stringValue = value as string;
            if (stringValue != null)
            {
                double frequency = ZPlannerManager.Project.Stackup.Frequency;
                ZLayerParameter layerParameter = (context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyObject as ZLayerParameter;
                if (layerParameter != null)
                {
                    double layerFrequency = (layerParameter.Layer.Stackup.Layers.Find(x => x.ID == layerParameter.Layer.ID) != null) ? layerParameter.Layer.GetFrequency() : double.NaN;
                    if (!double.IsNaN(layerFrequency)) frequency = layerFrequency;
                }

                double doubleValue;
                if (!string.IsNullOrEmpty(stringValue) && ZPlannerManager.TryParseDependedValue(stringValue, frequency, out doubleValue))
                {
                    return doubleValue.ToString((context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).PropertyFormat, CultureInfo.InvariantCulture);
                }

                return string.Empty;
            }

            return value.ToString();
        }

        public override object ConvertFrom(ITypeDescriptorContext context,
            CultureInfo culture, object value)
        {
            return value;
        }
    }
}
