using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Settings;
using ZZero.ZPlanner.Utils;

namespace ZZero.ZPlanner.Data.Properties
{
    class DoubleTypeMilsToMikronsConverter : DoubleTypeConverter
    {
        public override double getConversionCoefficient()
        {
            if (Options.TheOptions.isUnitsMetric())
                return Units.fMilsToMikrons;
            return 1; // no conversion
        }
    }

    class DoubleTypeMilsToMillimetersConverter : DoubleTypeConverter
    {
        public override double getConversionCoefficient()
        {
            if (Options.TheOptions.isUnitsMetric())
                return Units.fMilsToMillimeters;
            return 1; // no conversion
        }
    }
}
