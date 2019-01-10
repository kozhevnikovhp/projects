using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing.Design;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ZZero.ZPlanner.Data.Entities;

namespace ZZero.ZPlanner.Data.Properties
{
    class ZDynamicComponent : Component, ICustomTypeDescriptor
    {

        private PropertyDescriptorCollection propertyCollection;
        private int valueMaxLength;
        public ZDynamicComponent(ZEntity componentObject)
        {
            ComponentObject = componentObject;
            propertyCollection = new PropertyDescriptorCollection(new PropertyDescriptor[] { });
        }

        public ZEntity ComponentObject { get; set; }

        public int MaxLength
        {
            get
            {
                return valueMaxLength;
            }
            set
            {
                if (value > valueMaxLength)
                    valueMaxLength = value;
            }
        }

        public void AddProperty(object propObject, string propName, object propValue, string propDesc, string propCat, System.Type propType, string propFormat, ICollection list, bool isReadOnly, bool isExpandable, Type editorType, Type converterType)
        {
            DynamicProperty p = new DynamicProperty(propObject, propName, propValue, propDesc, propCat,
                propType, propFormat, list, isReadOnly, isExpandable, editorType, converterType);
            propertyCollection.Add(p);
            //Set our layout helper value.
            this.MaxLength = propName.Length;
            if (propValue != null) this.MaxLength = propValue.ToString().Length;
        }

        /// <summary>
        /// Класс необходим для динамического добавления свойств
        /// </summary>
        public class DynamicProperty : PropertyDescriptor
        {
            private object propObject;
            private string propName;
            private object propValue;
            private string propDescription;
            private string propCategory;
            private Type propType;
            private string propFormat;
            internal ICollection propList;
            private bool isReadOnly;
            private bool isExpandable;

            public DynamicProperty(object pObject, string pName, object pValue, string pDesc, string pCat, Type pType, string pFormat, ICollection pList, bool readOnly, bool expandable, Type editorType, Type converterType)
                : base(pName, new Attribute[] { })
            {
                propObject = pObject;
                propName = pName;
                propValue = pValue;
                propDescription = pDesc;
                propCategory = pCat;
                propType = pType;
                propFormat = pFormat;
                propList = pList;
                isReadOnly = readOnly;
                isExpandable = expandable;

                List<Attribute> attrList = new List<Attribute>(this.AttributeArray);

                if (converterType != null) attrList.Add(new TypeConverterAttribute(converterType));

                if (editorType != null) attrList.Add(new EditorAttribute(editorType, typeof(UITypeEditor)));
                    
                this.AttributeArray = attrList.ToArray();
            }

            public override System.Type ComponentType
            {
                get
                {
                    return null;
                }
            }
            public override string Category
            {
                get
                {
                    return propCategory;
                }
            }
            public ICollection ValuesList
            {
                get
                {
                    return propList;
                }
            }
            public override bool IsReadOnly
            {
                get
                {
                    return isReadOnly;
                }
            }
            public object PropertyObject
            {
                get
                {
                    return propObject;
                }
            }

            public override System.Type PropertyType
            {
                get
                {
                    return propType;
                }
            }

            public string PropertyFormat
            {
                get
                {
                    return propFormat;
                }
            }

            public override bool CanResetValue(object component)
            {
                return true;
            }
            public override object GetValue(object component)
            {
                return propValue;
            }
            public override void SetValue(object component, object value)
            {
                propValue = value;
            }
            public override void ResetValue(object component)
            {
                propValue = null;
            }
            public override bool ShouldSerializeValue(object component)
            {
                return false;
            }
            public override string Description
            {
                get
                {
                    return propDescription;
                }
            }
        }

        #region Реализация ICustomTypeDescriptor
        public AttributeCollection GetAttributes()
        {
            return (TypeDescriptor.GetAttributes(this, true));
        }
        public string GetClassName()
        {
            return (TypeDescriptor.GetClassName(this, true));
        }
        public string GetComponentName()
        {
            return (TypeDescriptor.GetComponentName(this, true));
        }
        public TypeConverter GetConverter()
        {
            return (TypeDescriptor.GetConverter(this, true));
        }
        public EventDescriptor GetDefaultEvent()
        {
            return (TypeDescriptor.GetDefaultEvent(this, true));
        }
        public PropertyDescriptor GetDefaultProperty()
        {
            PropertyDescriptorCollection props = GetAllProperties();

            if (props.Count > 0)
                return (props[0]);
            else
                return (null);
        }
        public object GetEditor(Type editorBaseType)
        {
            return (TypeDescriptor.GetEditor(this, editorBaseType, true));
        }
        public EventDescriptorCollection GetEvents(Attribute[] attributes)
        {
            return (TypeDescriptor.GetEvents(this, attributes, true));
        }
        public EventDescriptorCollection GetEvents()
        {
            return (TypeDescriptor.GetEvents(this, true));
        }
        public PropertyDescriptorCollection GetProperties(Attribute[] attributes)
        {
            return (GetAllProperties());
        }
        public PropertyDescriptorCollection GetProperties()
        {
            return (GetAllProperties());
        }
        public object GetPropertyOwner(PropertyDescriptor pd)
        {
            return (this);
        }

        /// <summary>
        /// Helper method to return the PropertyDescriptorCollection or our Dynamic Properties
        /// </summary>
        private PropertyDescriptorCollection GetAllProperties()
        {
            return propertyCollection;
        }

        #endregion
    }
}
