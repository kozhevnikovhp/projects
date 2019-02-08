using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    class ZLayerTypeConverter : StringConverter
    {
        /// <summary>
        /// Select from the list.
        /// </summary>
        public override bool GetStandardValuesSupported(
          ITypeDescriptorContext context)
        {
            return true;
        }

        /// <summary>
        /// Disable custom value.
        /// </summary>
        public override bool GetStandardValuesExclusive(
          ITypeDescriptorContext context)
        {
            // false - enable custom value.
            // true - disable custom value.
            return true;
        }

        /// <summary>
        /// The list.
        /// </summary>
        public override StandardValuesCollection GetStandardValues(
          ITypeDescriptorContext context)
        {
            return new StandardValuesCollection((context.PropertyDescriptor as ZZero.ZPlanner.Data.Properties.ZDynamicComponent.DynamicProperty).ValuesList);
        }

        public override bool CanConvertFrom(ITypeDescriptorContext context, Type sourceType)
        {
            if (sourceType == typeof(string)) return true;
            return base.CanConvertFrom(context, sourceType);
        }

        public override bool CanConvertTo(ITypeDescriptorContext context, Type destinationType)
        {
            if (destinationType == typeof(string)) return true;
            return base.CanConvertTo(context, destinationType);
        }

        public override object ConvertTo(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value, Type destinationType)
        {
            ZLayer layer = value as ZLayer;
            if (layer != null) return layer.GetLayerNumber();
            return value.ToString();
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            ZLayer layer = ((context.PropertyDescriptor as ZDynamicComponent.DynamicProperty).ValuesList as List<ZLayer>).Find(x => x.GetLayerNumber() == value.ToString());
            return layer;
        }
    }
}
