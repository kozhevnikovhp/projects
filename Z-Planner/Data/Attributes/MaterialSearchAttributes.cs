using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Attributes
{
    public class MaterialSearchAttributes
    {
        internal ZLayerType? LayerType { get; set; }
        public string Thickness { get; set; }
        public string Dk { get; set; }
        public string Construction { get; set; }
        public string Resin { get; set; }
        public string MaterialName { get; set; }

        public override bool Equals(object obj)
        {
            var item = obj as MaterialSearchAttributes;

            if (item == null)
            {
                return false;
            }

            return (this.LayerType == item.LayerType &&
                this.Thickness == item.Thickness &&
                this.Dk == item.Dk &&
                this.Construction == item.Construction &&
                this.Resin == item.Resin &&
                this.MaterialName == item.MaterialName);
        }

        public override int GetHashCode()
        {
            return string.Format("{0}#{1}#{2}#{3}#{4}#{5}", ZLayer.GetLayerTypeString(LayerType), MaterialName, Construction, Resin, Dk, Thickness).GetHashCode();
        }

        public MaterialSearchAttributes Clone()
        {
            MaterialSearchAttributes newItem = new MaterialSearchAttributes();
            newItem.LayerType = LayerType;
            newItem.Thickness = Thickness;
            newItem.Dk = Dk;
            newItem.Construction = Construction;
            newItem.Resin = Resin;
            newItem.MaterialName = MaterialName;

            return newItem;
        }
    }
}
