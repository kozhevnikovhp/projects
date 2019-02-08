using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    class ViaSpanFillTypeConverter : StringConverter
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
    }
}
