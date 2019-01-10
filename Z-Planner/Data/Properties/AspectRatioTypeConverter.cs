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
    class AspectRatioTypeConverter : StringConverter
    {
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
            if (value is double)
            {
                if (double.IsNaN((double)value)) return string.Empty;
                return ((double)value).ToString("N" + Settings.Options.TheOptions.percentDigits, CultureInfo.InvariantCulture) + " : 1";
            }
            return value.ToString();
        }

        public override object ConvertFrom(ITypeDescriptorContext context, System.Globalization.CultureInfo culture, object value)
        {
            double num;
            if (!string.IsNullOrEmpty(value as string) && double.TryParse((value as string).Replace(" : 1", string.Empty).Trim(), NumberStyles.AllowDecimalPoint, CultureInfo.InvariantCulture, out num)) return num;
            return 0;
        }
    }
}
