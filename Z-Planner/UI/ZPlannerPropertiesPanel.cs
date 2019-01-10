using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using ZZero.ZPlanner.Data.Entities;
using ZZero.ZPlanner.Data.Properties;
using ZZero.ZPlanner.ZConfiguration;

namespace ZZero.ZPlanner.UI
{
    public partial class ZPlannerPropertiesPanel : DockContent
    {
        Control gridView;

        public ZPlannerPropertiesPanel()
        {
            InitializeComponent();
            gridView = GetPropertyGridView();
            gridView.SizeChanged += gridView_SizeChanged;
            propertyGrid.PropertyValueChanged += propertyGrid_PropertyValueChanged;

            this.VisibleChanged += ZPlannerManager.Panel_VisibleChanged;
        }

        void gridView_SizeChanged(object sender, EventArgs e)
        {
            int gridWidth = gridView.Size.Width;
            //int splitterPosition = GetInternalLabelWidth();
            if (gridWidth > 0) MoveSplitterTo((int)(gridWidth * 0.6));
        }

        internal void Clear()
        {
            propertyGrid.PropertyValueChanged -= propertyGrid_PropertyValueChanged;
        }

        internal object SelectedObject { get { return propertyGrid.SelectedObject; } set { propertyGrid.SelectedObject = value; ColorPropertyGrid(); ShowPropertyGrid(true); } }

        internal ZDynamicCollection CollectionObject 
        {
            get { return collectionEditor.CollectionObject; }
            set { collectionEditor.CollectionObject = value; ShowPropertyGrid(false); } 
        }

        private void ShowPropertyGrid(bool isPropertyGridVisible)
        {
            propertyGrid.Visible = isPropertyGridVisible;
            collectionEditor.Visible = !isPropertyGridVisible;
        }

        internal void UpdateProperties()
        {
            ZDynamicComponent component = propertyGrid.SelectedObject as ZDynamicComponent;
            if (component == null) return;

            ZStackup stackup = component.ComponentObject as ZStackup;
            if (stackup != null) propertyGrid.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(stackup);

            ZLayer layer = component.ComponentObject as ZLayer;
            if (layer != null && layer.Stackup.Layers.Contains(layer)) propertyGrid.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(layer);

            ZSingle single = component.ComponentObject as ZSingle;
            if (single != null) propertyGrid.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(single);

            ZPair pair = component.ComponentObject as ZPair;
            if (pair != null) propertyGrid.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(pair);

            ZSpan span = component.ComponentObject as ZSpan;
            if (span != null) propertyGrid.SelectedObject = ZDynamicComponentManager.GetDynamicComponent(span);
        }

        internal void propertyGrid_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
        {
            ZLayerParameter layerParameter = (e.ChangedItem.PropertyDescriptor as ZDynamicComponent.DynamicProperty).PropertyObject as ZLayerParameter;
            if (layerParameter != null && layerParameter.Value != e.ChangedItem.Value.ToString())
            {

                if ((layerParameter.Parameter.ValueType == ZValueType.Number || layerParameter.Parameter.ValueType == ZValueType.Percent) && e.ChangedItem.Value is double && double.IsNaN((double)e.ChangedItem.Value)) layerParameter.Value = "";
                else
                {
                    switch (layerParameter.ID)
                    {
                        case ZStringConstants.ParameterIDThickness:
                            layerParameter.SetEditedValue(e.ChangedItem.Value.ToString());
                            break;
                        default:
                            layerParameter.Value = e.ChangedItem.Value.ToString();
                            break;
                    }
                }
            }

            ZSpan span = (e.ChangedItem.PropertyDescriptor as ZDynamicComponent.DynamicProperty).PropertyObject as ZSpan;
            if (span != null)
            {
                int intValue;
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["SpanTitle"] && span.Title != e.ChangedItem.Value.ToString())
                    span.Title = e.ChangedItem.Value.ToString();
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["Size"] && int.TryParse(e.ChangedItem.Value.ToString(), out intValue) && span.Size != intValue)
                    span.Size = intValue;
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["FirstLayer"] && span.FirstLayer != e.ChangedItem.Value as ZLayer)
                {
                    span.FirstLayer = e.ChangedItem.Value as ZLayer;
                    if (span.FirstLayer != e.ChangedItem.Value as ZLayer) UpdateProperties();
                }
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["LastLayer"] && span.LastLayer != e.ChangedItem.Value as ZLayer)
                {
                    span.LastLayer = e.ChangedItem.Value as ZLayer;
                    if (span.LastLayer != e.ChangedItem.Value as ZLayer) UpdateProperties();
                }
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["AspectRatio"] && span.AspectRatio != (double)e.ChangedItem.Value)
                    span.AspectRatio = (double)e.ChangedItem.Value;
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["Enable"] && span.IsEnable != (bool)e.ChangedItem.Value)
                    span.IsEnable = (bool)e.ChangedItem.Value;
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["Filled"] && span.Filled != (string)e.ChangedItem.Value)
                    span.Filled = (string)e.ChangedItem.Value;
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["BackDrilled"] && span.IsBackDrilled != (bool)e.ChangedItem.Value)
                    span.IsBackDrilled = (bool)e.ChangedItem.Value;
            }

            ZStackup stackup = (e.ChangedItem.PropertyDescriptor as ZDynamicComponent.DynamicProperty).PropertyObject as ZStackup;
            if (stackup != null)
            {
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["StackupTitle"] && stackup.Title != e.ChangedItem.Value.ToString())
                    stackup.Title = e.ChangedItem.Value.ToString();
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["Frequency"] && stackup.Frequency != (double)e.ChangedItem.Value)
                    stackup.Frequency = (double)e.ChangedItem.Value;
            }

            ZParameter parameter = (e.ChangedItem.PropertyDescriptor as ZDynamicComponent.DynamicProperty).PropertyObject as ZParameter;
            if (parameter != null)
            {
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["ParameterTitle"] && parameter.Title != e.ChangedItem.Value.ToString())
                    parameter.Title = e.ChangedItem.Value.ToString();
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["Description"] && parameter.Description != e.ChangedItem.Value.ToString())
                    parameter.Description = e.ChangedItem.Value.ToString();
                if (e.ChangedItem.Label == ZStringConstants.TitleDictionary["DefaultValue"] && parameter.DefaultValue != e.ChangedItem.Value.ToString())
                    parameter.DefaultValue = e.ChangedItem.Value.ToString();
            }

            ZEntity entity = (e.ChangedItem.PropertyDescriptor as ZDynamicComponent.DynamicProperty).PropertyObject as ZEntity;
            if (entity != null)
            {
                if ((e.ChangedItem.Label == ZStringConstants.TitleDictionary["RowTitle"] ||
                    e.ChangedItem.Label == ZStringConstants.TitleDictionary["SingleTitle"] ||
                    e.ChangedItem.Label == ZStringConstants.TitleDictionary["PairTitle"] ||
                    e.ChangedItem.Label == ZStringConstants.TitleDictionary["Title"]) && entity.Title != e.ChangedItem.Value.ToString()) 
                    entity.Title = e.ChangedItem.Value.ToString();
            }

            ColorPropertyGrid();
        }

        private void ColorPropertyGrid()
        {
            this.propertyGrid.ViewBackColor = Color.White;

            ZDynamicComponent dynamicComponent = propertyGrid.SelectedObject as ZDynamicComponent;
            if (dynamicComponent == null) return;

            ZLayer layer = dynamicComponent.ComponentObject as ZLayer;
            if (layer == null) return;

            this.propertyGrid.ViewBackColor = ZColorConstants.GetLayerColor(layer.GetLayerType());
        }

        /// <summary>
        /// Gets the (private) PropertyGridView instance.
        /// </summary>
        /// <param name="propertyGrid">The property grid.</param>
        /// <returns>The PropertyGridView instance.</returns>
        private Control GetPropertyGridView()
        {
            MethodInfo methodInfo = typeof(PropertyGrid).GetMethod("GetPropertyGridView", BindingFlags.NonPublic | BindingFlags.Instance);
            return (Control)methodInfo.Invoke(propertyGrid, new object[] { });
        }

        /// <summary>
        /// Gets the width of the left column.
        /// </summary>
        /// <param name="propertyGrid">The property grid.</param>
        /// <returns>
        /// The width of the left column.
        /// </returns>
        private int GetInternalLabelWidth()
        {
            //protected int InternalLabelWidth
            PropertyInfo propInfo = gridView.GetType().GetProperty("InternalLabelWidth", BindingFlags.NonPublic | BindingFlags.Instance);
            return (int)propInfo.GetValue(gridView);
        }

        /// <summary>
        /// Moves the splitter to the supplied horizontal position.
        /// </summary>
        /// <param name="propertyGrid">The property grid.</param>
        /// <param name="xpos">The horizontal position.</param>
        private void MoveSplitterTo(int xpos)
        {
            //private void MoveSplitterTo(int xpos);
            MethodInfo methodInfo = gridView.GetType().GetMethod("MoveSplitterTo", BindingFlags.NonPublic | BindingFlags.Instance);
            methodInfo.Invoke(gridView, new object[] { xpos });
        }
    }
}
